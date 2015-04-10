# -*- coding: utf-8  -*-
"""
This bot will standardize footnote references. It will retrieve information on
which pages might need changes either from an SQL dump (no longer supported)
or a text file, or only change a single page.

At present it converts to [[Wikipedia:Footnote3]] format (ref/note).

NOTE: This script is not capable of handling the <ref></ref> syntax. It just
handles the {{ref}} syntax, which is still used, but DEPRECATED on the English
wikipedia.

You can run the bot with the following commandline parameters:

-file        - Work on all pages given in a local text file.
               Will read any [[wiki link]] and use these articles.
               Argument can also be given as "-file:filename".
-cat         - Work on all pages which are in a specific category.
               Argument can also be given as "-cat:categoryname".
-page        - Only edit a single page.
               Argument can also be given as "-page:pagename". You can give this
               parameter multiple times to edit multiple pages.
-regex       - Make replacements using regular expressions.
               (Obsolete; always True)
-except:XYZ  - Ignore pages which contain XYZ. If the -regex argument is given,
               XYZ will be regarded as a regular expression.
-namespace:n - Namespace to process. Works only with a sql dump
-always      - Don't prompt you for each replacement
other:       - First argument is the old text, second argument is the new text.
               If the -regex argument is given, the first argument will be
               regarded as a regular expression, and the second argument might
               contain expressions like \\1 or \g<name>.

NOTE: Only use either -sql or -file or -page, but don't mix them.
"""
# Derived from replace.py
#
# (C) Daniel Herding, 2004
# Copyright Scot E. Wilcoxon 2005
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: standardize_notes.py 9042 2011-03-13 10:14:47Z xqt $'
#
# 2005-07-15: Find name of section containing citations: doFindRefSection().
#             (SEWilco)
# 2005-07-15: Obey robots.txt restrictions. (SEWilco)
# 2005-07-15: Build list of all sections which may contain citations:
#             doFindAllCitationSections(). (SEWilco)
#

import subprocess, sys, re, random
import socket, urllib, robotparser
from datetime import date
import wikipedia as pywikibot
import pagegenerators, config

# httpcache is optional
have_httpcache = True
try:
    from httpcache import HTTPCache
except ImportError:
    have_httpcache = False

# Summary messages in different languages
msg = {
       'ar':u'روبوت: معالجة مراجع تلقائية %s',
       'de':u'Bot: Automatisierte Textersetzung %s',
       'en':u'Robot: Automated reference processing %s',
       'es':u'Robot: Reemplazo automático de texto %s',
       'fr':u'Robot : Remplacement de texte automatisé %s',
       'he':u'בוט: הופך את הערת השוליים %s לאוטומטית',
       'hu':u'Robot: Automatikus szövegcsere %s',
       'ia':u'Robot: Reimplaciamento automatic de texto %s',
       'is':u'Vélmenni: breyti texta %s',
       'nl':u'Bot: geautomatiseerde verwerking van referenties %s',
       'pl':u'Robot automatycznie przetwarza źródła %s',
       'pt':u'Bot: Mudança automática %s',
       }

fixes = {
    # These replacements will convert alternate reference formats to format used
    # by this tool.
    'ALTREFS': {
        'regex': True,
        # We don't want to mess up pages which discuss HTML tags, so we skip
        # all pages which contain nowiki tags.
        'exceptions':  ['<nowiki>[^<]{3,}</nowiki>'],
        'msg': {
               'ar':u'روبوت: إضافة/ترتيب المراجع.',
               'en':u'Robot: Adding/sorting references.',
               'ar':u'روبوت: إضافة/ترتيب المراجع.',
               'fr':u'Robot : Ajoute/trie les références.',
               'he':u'בוט: מוסיף/מסדר הערות שוליים',
               'ia':u'Robot: Addition/assortimento de referentias',
               'nl':u'Bot: referenties toegevoegd/gesorteerd',
               'pl':u'Robot dodaje/sortuje źródła',
              },
        'replacements': [
            # Everything case-insensitive (?i)
            # These translate variations of footnote templates to ref|note
            # format.
            (r'(?i){{an\|(.*?)}}',              r"{{ref|\1}}"),
            (r'(?i){{anb\|(.*?)}}',             r"{{note|\1}}"),
            (r'(?i){{endnote\|(.*?)}}',         r"{{note|\1}}"),
            (r'(?i){{fn\|(.*?)}}',              r"{{ref|fn\1}}"),
            (r'(?i){{fnb\|(.*?)}}',             r"{{note|fn\1}}"),
            (r'(?i){{namedref\|(.*?)\|.*?}}',             r"{{ref|fn\1}}"),
            (r'(?i){{namednote\|(.*?)\|.*?}}',             r"{{note|fn\1}}"),
            # subst: fn and fnb
            (r'(?i)<sup id=".*?">[[][[]#fn(.*?)[|][0-9a-z]*[]][]]</sup>',              r"{{ref|fn\1}}"),
            (r'(?i)<cite id="fn_(.*?)">[[][[]#fn.*?[]][]]</cite>',             r"{{note|fn_\1}}"),
            (r'(?i){{mn\|(.*?)\|(.*?)}}',              r"{{ref|mn\1_\2}}"),
            (r'(?i){{mnb\|(.*?)\|(.*?)}}',             r"{{note|mn\1_\2}}"),
            # a header where only spaces are in the same line
            (r'(?i)([\r\n]) *<h1> *([^<]+?) *</h1> *([\r\n])',  r"\1= \2 =\3"),
            (r'(?i)([\r\n]) *<h2> *([^<]+?) *</h2> *([\r\n])',  r"\1== \2 ==\3"),
            (r'(?i)([\r\n]) *<h3> *([^<]+?) *</h3> *([\r\n])',  r"\1=== \2 ===\3"),
            (r'(?i)([\r\n]) *<h4> *([^<]+?) *</h4> *([\r\n])',  r"\1==== \2 ====\3"),
            (r'(?i)([\r\n]) *<h5> *([^<]+?) *</h5> *([\r\n])',  r"\1===== \2 =====\3"),
            (r'(?i)([\r\n]) *<h6> *([^<]+?) *</h6> *([\r\n])',  r"\1====== \2 ======\3"),
            # A bare http URL; does not recognize all formats
            #(r'(?i) http://([^ ]*)',              r" [http://\1]"),
        ]
    }
}

# names of reference section names
referencesectionnames = [
    'bibliography',
    'citation',
    'citations',
    'external link',
    'external links',
    'external links and references',
    'footnotes',
    'links',
    'notes',
    'notes and references',
    'reference',
    'references',
    'source',
    'sources',
 ]

# news sites for which to generate 'news reference' citations, the org name, and prefix to strip
newssites = [
    ('abcnews.go.com', 'ABC News', 'ABC News: '),
    ('books.guardian.co.uk', 'The Guardian',
     'Guardian Unlimited : The Guardian : '),
    ('edition.cnn.com', 'CNN', 'CNN.com - '),
    ('news.bbc.co.uk', 'BBC', 'BBC NEWS : '),
    ('news.scotsman.com', 'The Scotsman', 'Scotsman.com News - '),
    ('nyobserver.com', 'New York Observer', ''),
    ('observer.guardian.co.uk', 'The Guardian', 'The Observer  : '),
    ('politics.guardian.co.uk', 'The Guardian',
     'Guardian Unlimited Politics : '),
    ('seattletimes.nwsource.com', 'The Seattle Times', 'The Seattle Times: '),
    ('service.spiegel.de', 'Der Spiegel', ''),
    ('thescotsman.scotsman.com', 'The Scotsman', 'The Scotsman - '),
    ('today.reuters.com', 'Reuters', 'Latest News and Financial Information : '),
    ('today.reuters.co.uk', 'Reuters',
     'Latest News and Financial Information : '),
    ('www.boston.com', 'The Boston Globe', 'Boston.com / '),
    ('www.cbsnews.com', 'CBS News', 'CBS News : '),
    ('www.cnn.com', 'CNN', 'CNN.com - '),
    ('www.cnsnews.com', 'Cybercast News Service', ''),
    ('www.csmonitor.com', 'Christian Science Monitor', ''),
    ('www.dallasnews.com', 'The Dallas Morning News', ''),
    ('www.forbes.com', 'Forbes', ''),
    ('www.foxnews.com', 'Fox News Channel', 'FOXNews.com - '),
    ('www.gnn.com', 'Government News Network', 'GNN - '),
    ('www.guardian.co.uk', 'The Guardian',
     'Guardian Unlimited : The Guardian : '),
    ('www.latimes.com', 'Los Angeles Times', ''),
    ('www.msnbc.msn.com', 'MSNBC', ''),
    ('www.nationalreview.com', 'National Review', ''),
    ('www.nytimes.com', 'The New York Times', ''),
    ('www.sfgate.com', 'San Francisco Chronicle', ''),
    ('www.socialistworker.co.uk', 'Socialist Worker', ''),
    ('www.spectator.org', 'The American Spectator', ''),
    ('www.telegraph.co.uk', 'The Daily Telegraph',
     'Telegraph newspaper online - '),
    ('www.time.com', 'TIME', ''),
    ('www.timesonline.co.uk', 'The Times',
     'World news from The Times and the Sunday Times - '),
    ('www.usatoday.com', 'USA Today', 'USATODAY.com - '),
    ('www.washingtonpost.com', 'The Washington Post', ''),
    ('www.washtimes.com', 'The Washington Times', ''),
    ('www.weeklystandard.com', 'The Weekly Standard', ''),
    ('www.wired.com', 'Wired magazine', 'Wired News: '),
    ('wwwimage.cbsnews.com', 'CBS News', 'CBS News : '),
]


class ReplacePageGenerator:
    """ Generator which will yield Pages for pages that might contain text to
    replace. These pages might be retrieved from a local SQL dump file or a
    text file, or as a list of pages entered by the user.

    Arguments:
        * source       - Where the bot should retrieve the page list from.
                         Can be 'sqldump', 'textfile' or 'userinput'.
        * replacements - A dictionary where keys are original texts and values
                         are replacement texts.
        * exceptions   - A list of strings; pages which contain one of these
                         won't be changed.
        * regex        - If the entries of replacements and exceptions should
                         be interpreted as regular expressions
        * namespace    - Namespace to process in case of a SQL dump. -1 means
                         that all namespaces should be searched.
        * textfilename - The textfile's path, either absolute or relative, which
                         will be used when source is 'textfile'.
        * sqlfilename  - The dump's path, either absolute or relative, which
                         will be used when source is 'sqldump'.
        * pagenames    - a list of pages which will be used when source is
                         'userinput'.

    """

    def __init__(self, source, replacements, exceptions, regex = False, namespace = -1, textfilename = None, sqlfilename = None, categoryname = None, pagenames = None):
        self.source = source
        self.replacements = replacements
        self.exceptions = exceptions
        self.regex = regex
        self.namespace = namespace
        self.textfilename = textfilename
        self.sqlfilename = sqlfilename
        self.categoryname = categoryname
        self.pagenames = pagenames

    def read_pages_from_sql_dump(self):
        """ Generator which will yield Pages to pages that might contain text to
        replace. These pages will be retrieved from a local sql dump file
        (cur table).

        Arguments:
            * sqlfilename  - the dump's path, either absolute or relative
            * replacements - a dictionary where old texts are keys and new texts
                             are values
            * exceptions   - a list of strings; pages which contain one of these
                             won't be changed.
            * regex        - if the entries of replacements and exceptions
                             should be interpreted as regular expressions

        """
        mysite = pywikibot.getSite()
        import sqldump
        dump = sqldump.SQLdump(self.sqlfilename, pywikibot.getSite().encoding())
        for entry in dump.entries():
            skip_page = False
            if self.namespace != -1 and self.namespace != entry.namespace:
                continue
            else:
                for exception in self.exceptions:
                    if self.regex:
                        exception = re.compile(exception)
                        if exception.search(entry.text):
                            skip_page = True
                            break
                    else:
                        if exception in entry.text:
                            skip_page = True
                            break
            if not skip_page:
                for old, new in self.replacements:
                    if self.regex:
                        old = re.compile(old)
                        if old.search(entry.text):
                            yield pywikibot.Page(mysite, entry.full_title())
                            break
                    else:
                        if old in entry.text:
                            yield pywikibot.Page(mysite, entry.full_title())
                            break

    def read_pages_from_category(self):
        """
        Generator which will yield pages that are listed in a text file created by
        the bot operator. Will regard everything inside [[double brackets]] as a
        page name, and yield Pages for these pages.

        Arguments:
            * textfilename - the textfile's path, either absolute or relative

        """
        import catlib
        category = catlib.Category(pywikibot.getSite(), self.categoryname)
        for page in category.articles(recurse = False):
            yield page

    def read_pages_from_text_file(self):
        """
        Generator which will yield pages that are listed in a text file created by
        the bot operator. Will regard everything inside [[double brackets]] as a
        page name, and yield Pages for these pages.

        Arguments:
            * textfilename - the textfile's path, either absolute or relative

        """
        f = open(self.textfilename, 'r')
        # regular expression which will find [[wiki links]]
        R = re.compile(r'.*\[\[([^\]]*)\]\].*')
        m = False
        for line in f.readlines():
            # BUG: this will only find one link per line.
            # TODO: use findall() instead.
            m=R.match(line)
            if m:
                yield pywikibot.Page(pywikibot.getSite(), m.group(1))
        f.close()

    def read_pages_from_wiki_page(self):
        '''
        Generator which will yield pages that are listed in a wiki page. Will
        regard everything inside [[double brackets]] as a page name, except for
        interwiki and category links, and yield Pages for these pages.

        Arguments:
            * pagetitle - the title of a page on the home wiki

        '''
        listpage = pywikibot.Page(pywikibot.getSite(), self.pagetitle)
        list = pywikibot.get(listpage)
        # TODO - UNFINISHED

    # TODO: Make MediaWiki's search feature available.
    def __iter__(self):
        '''
        Starts the generator.
        '''
        if self.source == 'sqldump':
            for pl in self.read_pages_from_sql_dump():
                yield pl
        elif self.source == 'textfile':
            for pl in self.read_pages_from_text_file():
                yield pl
        elif self.source == 'category':
            for pl in self.read_pages_from_category():
                yield pl
        elif self.source == 'userinput':
            for pagename in self.pagenames:
                yield pywikibot.Page(pywikibot.getSite(), pagename)

class ReplaceRobot:
    def __init__(self, generator, replacements, refsequence, references,
                 refusage, exceptions = [], regex = False, acceptall = False,
                 summary = ''):
        self.generator = generator
        self.replacements = replacements
        self.exceptions = exceptions
        self.regex = regex
        self.acceptall = acceptall
        self.references = references
        self.refsequence = refsequence
        self.refusage = refusage
        self.summary = summary

    def checkExceptions(self, original_text):
        """
        If one of the exceptions applies for the given text, returns the
        substring. which matches the exception. Otherwise it returns None.
        """
        for exception in self.exceptions:
            if self.regex:
                exception = re.compile(exception)
                hit = exception.search(original_text)
                if hit:
                    return hit.group(0)
            else:
                hit = original_text.find(exception)
                if hit != -1:
                    return original_text[hit:hit + len(exception)]
        return None

    def doReplacements(self, new_text):
        """
        Returns the text which is generated by applying all replacements to the
        given text.
        """

        # For any additional replacements, loop through them
        for old, new in self.replacements:
            if self.regex:
                # TODO: compiling the regex each time might be inefficient
                oldR = re.compile(old)
                new_text = oldR.sub(new, new_text)
            else:
                new_text = new_text.replace(old, new)

        # Find name of Notes section.
        refsectionname = self.doFindRefSection(new_text)
        # Get list of all sections which may contain citations.
        refsectionlist = self.doFindAllCitationSections(new_text,
                                                        refsectionname)
        # Read existing Notes section contents into references list
        pywikibot.output(u"Reading existing Notes section")
        self.doReadReferencesSection( new_text, refsectionname )
        while self.references and self.references[len(self.references)-1] == u'\n':
            del self.references[len(self.references)-1]    # delete trailing empty lines
        # Convert any external links to footnote references
        pywikibot.output(u"Converting external links" )
        new_text = self.doConvertExternalLinks(new_text)
        # Accumulate ordered list of all references
        pywikibot.output(u"Collecting references")
        (duplicatefound, self.refusage) = self.doBuildSequenceListOfReferences( new_text )
        # Rewrite references, including dealing with duplicates.
        pywikibot.output(u"Rewriting references")
        new_text = self.doRewriteReferences(new_text, self.refusage,
                                            refsectionname)
        # Reorder Notes to match sequence of ordered list
        pywikibot.output(u"Collating references")
        self.references = self.doReorderReferences(self.references,
                                                   self.refusage)
        # Rebuild Notes section
        pywikibot.output(u"Rebuilding References section" )
        new_text = self.doUpdateReferencesSection(new_text, self.refusage,
                                                  refsectionname)
        return new_text

    def doConvertExternalLinks(self, original_text):
        """ Returns the text which is generated by converting external links to
        References. Adds References to reference list.

        """
        new_text = ''                # Default is no text
        skipsection = False
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check for protected sections
            m = re.search("== *(?P<sectionname>[^\]\|=]*) *==", text_line)
            # TODO: support subheadings within Notes section
            # TODO: support Notes in alphabetic order
            # TODO: support Notes in other orders
            if m:    # if in a section, check if should skip this section
                if m.group('sectionname').lower().strip() in referencesectionnames:
                    skipsection = True        # skipsection left True so no further links converted
            if skipsection:
                new_text = new_text + text_line        # skip section, so retain text.
            else:
                # TODO: recognize {{inline}} invisible footnotes when something can be done with them
                #
                # Ignore lines within comments
                if not text_line.startswith( u'<!--'):
                    # Fix erroneous external links in double brackets
                    Rextlink = re.compile(r'(?i)\[\[(?P<linkname>http://[^\]]+?)\]\]')
                    # TODO: compiling the regex each time might be inefficient
                    text_lineR = re.compile(Rextlink)
                    MOextlink = text_lineR.search(text_line)
                    while MOextlink:    # find all links on line
                        extlink_linkname = MOextlink.group('linkname')
                        # Rewrite double brackets to single ones
                        text_line=text_line[:MOextlink.start()] + '[%s]' % extlink_linkname + text_line[MOextlink.end(0):]
                        MOextlink = text_lineR.search(text_line,MOextlink.start(0)+1)
                    # Regular expression to look for external link [linkname linktext] - linktext is optional.
                    # Also accepts erroneous pipe symbol as separator.
                    # Accepts wikilinks within <linktext>
                    #Rextlink = re.compile(r'[^\[]\[(?P<linkname>[h]*[ft]+tp:[^ [\]\|]+?)(?P<linktext>[ \|]+(( *[^\]\|]*)|( *\[\[.+?\]\])*)+)*\][^\]]')
                    #Rextlink = re.compile(r'\[(?P<linkname>[h]*[ft]+tp:[^ [\]\|]+?)(?P<linktext>[ \|]+(( *[^\]\|]*)|( *\[\[.+?\]\])*)+)*\]')
                    Rextlink = re.compile(r'(?i)\[(?P<linkname>[h]*[ft]+tp:[^ [\]\|]+?)(?P<linktext>[ \|]+(( *[^\]\|]*)|( *\[\[.+?\]\])*)+)*\]')
                    # TODO: compiling the regex each time might be inefficient
                    text_lineR = re.compile(Rextlink)
                    MOextlink = text_lineR.search(text_line)
                    while MOextlink:    # find all links on line
                        extlink_linkname = MOextlink.group('linkname')
                        extlink_linktext = MOextlink.group('linktext')
                        self.refsequence += 1
                        ( refname, reftext ) = self.doConvertLinkTextToReference(self.refsequence, extlink_linkname, extlink_linktext)
                        self.references.append( reftext )    # append new entry to References
                        if extlink_linktext:
                            # If there was text as part of link, reinsert text before footnote.
                            text_line=text_line[:MOextlink.start(0)] + '%s{{ref|%s}}' % (extlink_linktext, refname) + text_line[MOextlink.end(0):]
                        else:
                            text_line=text_line[:MOextlink.start(0)] + '{{ref|%s}}' % refname + text_line[MOextlink.end(0):]
                        MOextlink = text_lineR.search(text_line,MOextlink.start(0)+1)
                    # Search for {{doi}}
                    Rdoi = re.compile(r'(?i){{doi\|(?P<doilink>[^}|]*)}}')
                    # TODO: compiling the regex each time might be inefficient
                    doiR = re.compile(Rdoi)
                    MOdoi = doiR.search(text_line)
                    while MOdoi:        # find all doi on line
                        doi_link = MOdoi.group('doilink')
                        if doi_link:
                            self.refsequence += 1
                            ( refname, reftext ) = self.doConvertDOIToReference( self.refsequence, doi_link )
                            self.references.append( reftext )        # append new entry to References
                            text_line=text_line[:MOdoi.start(0)] + '{{ref|%s}}' % refname + text_line[MOdoi.end(0):]
                            MOdoi = doiR.search(text_line, MOdoi.start(0)+1)
                new_text = new_text + text_line    # append new line to new text
        if new_text == '':
            new_text = original_text    # If somehow no new text, return original text
        return new_text

    def doFindRefSection(self, original_text):
        """
        Returns name of section which contains citations.
        Finds first section with reference note template.
        """
        refsectionname = ''
        sectionname = ''
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            if refsectionname == '':    # if ref section not found
                # Check if line has a section name
                m = re.search( r'==+(?P<sectionname>[^=]+)==', text_line )
                if m:    # if in a section, remember section name
                    sectionname = m.group('sectionname').strip()
                    pywikibot.output( u'Section: %s' % sectionname )
                else:    # else not a section name so look for reference
                    n = re.search( r'(i?){{(note|ibid)[|]', text_line )
                    if n:    # if reference found
                        refsectionname = sectionname    # found reference section
                        pywikibot.output( u'Ref section: %s' % refsectionname )
                        break    # stop looking
        return refsectionname

    def doFindAllCitationSections(self, original_text, refsectionname):
        """ Returns list of sections which may contain citations. """
        refsectionlist = [ ( refsectionname) ]
        sectionname = ''
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check if line has a section name
            m = re.search( "==[ ]*(?P<sectionname>[^=]+)[ ]*==", text_line )
            if m:    # if in a section, remember section name
                sectionname = m.group('sectionname').strip()
                if sectionname.lower().strip() in referencesectionnames:
                    if sectionname not in refsectionlist:    # if not already in list, add to list.
                        refsectionlist.extend( sectionname )
        return refsectionlist

    def doRewriteReferences(self, original_text, refusage, refsectionname):
        """
        Returns the text which is generated by rewriting references, including duplicate refs.
        """
        new_text = ''                # Default is no text
        skipsection = False
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check for protected sections
            m = re.search( r'==+(?P<sectionname>[^=]+)==', text_line )
            if m:    # if in a section, check if should skip this section
                if refsectionname != '':    # if a certain section name has been identified
                    m_section = m.group('sectionname')
                    pywikibot.output( u'Looking for "%s": "%s"' % (refsectionname,unicode(m_section)) )
                    if unicode(m_section.strip()) == unicode(refsectionname):
                        pywikibot.output( u'Found Ref section.')
                        skipsection = True        # skipsection left True so no further links converted
                else:                # else grab all possible sections
                    if m.group('sectionname').lower().strip() in referencesectionnames:
                        pywikibot.output('RefSection found by default names: %s' % m.group('sectionname') )
                        skipsection = True        # skipsection left True so no further links converted
            if skipsection:
                new_text = new_text + text_line        # skip section, so retain text.
            else:
                # TODO: recognize {{inline}} invisible footnotes when something can be done with them
                #
                # Data structure: refusage[reference_key] = [ sequence_in_document, count, count_during_dup_handling ]
                # Check for various references
                # TODO: compiling the regex each time might be inefficient
                Rtext_line = re.compile(r'(?i){{(?P<reftype>ref|ref_num|ref_label)\|(?P<refname>[^}|]+?)}}')
                m = Rtext_line.search( text_line )
                alphabet26 = u'abcdefghijklmnopqrstuvwxyz'
                while m:    # if found a reference
                    if m.group('reftype').lower() in ('ref', 'ref_num', 'ref_label'):    # confirm ref
                        refkey = m.group('refname').strip()
                        if refkey != '':
                            if refkey in refusage:
                                # pywikibot.output( u'refusage[%s] = %s' % (refkey,refusage[refkey]) )
                                if refusage[refkey][2] == 0:    # if first use of reference
                                    text_line=text_line[:m.start(0)] + '{{ref|%s}}' % (refkey) + text_line[m.end(0):]
                                    refusage[refkey][2] += 1    # count use of reference
                                else:                # else not first use of reference
                                    text_line=text_line[:m.start(0)] + '{{ref_label|%s|%d|%s}}' % (refkey,(refusage[refkey][0])+1,alphabet26[((refusage[refkey][2])-1)%26]) + text_line[m.end(0):]
                                    refusage[refkey][2] += 1    # count use of reference
                            else:
                                # Odd, because refusage list is populated the key should exist already.
                                refusage[refkey] = [len(refusage),1,1]    # remember this reference
                                text_line=text_line[:m.start(0)] + '{{ref|%s}}' % refkey + text_line[m.end(0):]
                    m = Rtext_line.search( text_line, m.start(0)+1 )
                new_text = new_text + text_line    # append new line to new text
        if new_text == '':
            new_text = original_text    # If somehow no new text, return original text
        return new_text

    def doGetTitleFromURL(self, extlink_linkname ):
        """
        Returns text derived from between <title>...</title> tags through a URL.
        Obeys robots.txt restrictions.
        """
        # if no descriptive text get from web site, if not PDF
        urltitle = u''
        urlfile = None
        urlheaders = None
        if len(extlink_linkname) > 5:
            socket.setdefaulttimeout(20)    # timeout in seconds
            pywikibot.get_throttle()    # throttle down to Wikipedia rate
            # Obey robots.txt restrictions
            rp = robotparser.RobotFileParser()
            rp.set_url( extlink_linkname )
            try:
                rp.read()    # read robots.txt
            except (IOError, socket.timeout):
                pywikibot.output(u'Error accessing URL: %s'
                                 % unicode(extlink_linkname))
            else:
                urlobj = None
                if not rp.can_fetch( "*", extlink_linkname ):
                    pywikibot.output(u'Robot prohibited: %s'
                                     % unicode(extlink_linkname))
                else:    # else access allowed
                    try:
                        if have_httpcache:
                            cache = HTTPCache(extlink_linkname)
                            urlfile = cache.filename()    # filename of cached date
                            urlheaders = cache.info()
                        else:
                            (urlfile, urlheaders) = urllib.urlretrieve(extlink_linkname)
                    except IOError:
                        pywikibot.output(u'Error accessing URL. %s'
                                         % unicode(extlink_linkname))
                    except (socket.herror, socket.gaierror), (err, msg):
                        pywikibot.output(u'Error %i accessing URL, %s. %s'
                                         % (err, unicode(msg),
                                            unicode(extlink_linkname)))
                    except socket.timeout, msg:
                        pywikibot.output(u'Error accessing URL, %s. %s'
                                         % (unicode(msg),
                                            unicode(extlink_linkname)))
                    except:    # Ignore other errors
                        pass
                if urlfile != None:
                    urlobj = open( urlfile )
                    if extlink_linkname.lower().endswith('.pdf'):
                        # If file has a PDF suffix
                        pywikibot.output( u'PDF file.')
                        try:
                            pdfinfo_out = subprocess.Popen([r"pdfinfo","/dev/stdin"], stdin=urlobj, stdout=subprocess.PIPE, shell=False).communicate()[0]
                            for aline in pdfinfo_out.splitlines():
                                if aline.lower().startswith('title'):
                                    urltitle = aline.split(None)[1:]
                                    urltitle = ' '.join(urltitle)
                                    if urltitle:
                                        pywikibot.output(u'title: %s'
                                                         % urltitle)
                                else:
                                    if aline.lower().startswith('author'):
                                        urlauthor = aline.split(None)[1:]
                                        urlauthor = ' '.join(urlauthor)
                                        if urlauthor:
                                            pywikibot.output(u'author: %s'
                                                             % urlauthor )
                        except ValueError:
                            pywikibot.output( u'pdfinfo value error.')
                        except OSError:
                            pywikibot.output( u'pdfinfo OS error.')
                        except:    # Ignore errors
                            pywikibot.output( u'PDF processing error.')
                            pass
                        pywikibot.output( u'PDF done.')
                        if urlobj:
                            urlobj.close()
                    else:
                        # urlinfo = urlobj.info()
                        aline = urlobj.read()
                        maxalines = 100
                        while maxalines > 0 and aline and urltitle == '':
                            maxalines -= 1    # reduce number of lines left to consider
                            titleRE = re.search("(?i)<title>(?P<HTMLtitle>[^<>]+)", aline)
                            if titleRE:
                                try:
                                    urltitle = unicode(titleRE.group('HTMLtitle'), 'utf-8')
                                except:
                                    urltitle = u' '    # error, no title
                                urltitle = u' '.join(urltitle.split())    # merge whitespace
                                pywikibot.output( u'::::Title: %s' % urltitle )
                                break    # found a title so stop looking
                            else:
                                if maxalines < 1:
                                    pywikibot.output(
                                        u'No title in URL. %s'
                                        % unicode(extlink_linkname) )
                        else:
                            if urlobj != None:
                                pywikibot.output( u'::+URL: ' + extlink_linkname )
                                # urlinfo = urlobj.info()
                                aline = urlobj.read()
                                full_page = ''
                                # while aline and urltitle == '':
                                while aline:
                                    full_page = full_page + aline
                                    titleRE = re.search("(?i)<title>(?P<HTMLtitle>[^<>]+)", aline)
                                    if titleRE:
                                        if titleRE.group('HTMLtitle'):
                                            urltitle = u''
                                            try:
                                                urltitle = unicode(titleRE.group('HTMLtitle'), 'utf-8')
                                                urltitle = u' '.join(urltitle.split())    # merge whitespace
                                                pywikibot.output( u'::::Title: %s' % urltitle )
                                            except:
                                                aline = urlobj.read()
                                                continue
                                            else:
                                                aline = urlobj.read()
                                                continue
                                            break    # found a title so stop looking
                                        else:
                                            aline = urlobj.read()
                                    else:
                                        aline = urlobj.read()
                                if urltitle != '': pywikibot.output( u'title: ' + urltitle )
                                # Try a more advanced search
                                ##from nltk.parser.probabilistic import *
                                ##from nltk.tokenizer import *
                                #from nltk.tagger import *
                                #from nltk.tagger.brill import *
                                #from nltk.corpus import brown
                                ##pcfg_parser = ViterbiPCFGParser(grammar)
                                ##text_token = Token(TEXT=full_page)
                                ##WhitespaceTokenizer(SUBTOKENS='WORDS').tokenize(text_token)
                                ##tree = pcfg_parser.get_parse(sent_token)
                                ##print tree.prob()
                                # Train tagger
                                #train_tokens = []
                                #for item in brown.items()[:10]: train_tokens.append(brown.read(item))
                                #unitagger = UnigramTagger(SUBTOKENS='WORDS')
                                #brilltemplates = ()
                                #britaggerrules = BrillTaggerTrainer(initial_tagger=unitagger, templates=brilltemplates, trace=True, SUBTOKENS='WORDS', max_rules=200, min_score=2)
                                #for tok in train_tokens: unitagger.train(tok)
                                #for tok in train_tokens: britaggerrules.train(tok, max_rules=200, min_score=2)
                                # brittaggerrul = britaggerrules.train(train_tokens, max_rules=200, min_score=2)
                                #britaggerrul = ()
                                #britagger = BrillTagger(initial_tagger=unitagger, rules=britaggerrul, SUBTOKENS='WORDS')
                                # Training completed
                                # Examine text
                                ##text_token = Token(TEXT=full_page)
                                ##WhitespaceTokenizer(SUBTOKENS='WORDS').tokenize(text_token)
                                #unitagger.tag(text_token)
                                #britagger.tag(text_token)
                                ### pywikibot.output( unicode(text_token) )
                else:
                    pywikibot.output( u'No data retrieved.')
            socket.setdefaulttimeout(200)
            urltitle = urltitle.replace(u'|',u':')
        return urltitle.strip()

    def doConvertLinkTextToReference(self, refsequence, extlink_linkname, extlink_linktext):
        """
        Returns the text which is generated by converting a link to
        a format suitable for the References section.
        """
        refname = u'refbot.%d' % refsequence
        m = re.search("[\w]+://([\w]\.)*(?P<siteend>[\w.]+)[/\Z]", extlink_linkname)
        if m:
            refname = m.group('siteend') + u'.%d' % refsequence    # use end of site URL as reference name
        new_text = u'# {{note|%s}} %s' % (refname, self.doConvertRefToCitation( extlink_linktext, extlink_linkname, refname ) ) + '\n'
        return (refname, new_text)

    def doConvertRefToCitation(self, extlink_linktext, extlink_linkname, refname ):
        """
        Returns text with a citation created from link information
        """
        new_text = u''
        now = date.today()
        if extlink_linktext == None or len(extlink_linktext.strip()) < 20:
            pywikibot.output( u'Fetching URL: %s' % unicode(extlink_linkname) )
            urltitle = self.doGetTitleFromURL( extlink_linkname )    # try to get title from URL
            if urltitle == None or urltitle == '':
                urltitle = extlink_linkname
            pywikibot.output( u'Title is: %s' % urltitle )
            extlink_linktext = urltitle
            for newref in self.references:        # scan through all references
                if extlink_linkname in newref:        # if undescribed linkname same as a previous entry
                    if urltitle:
                        extlink_linktext = urltitle + ' (See above)'
                    else:
                        extlink_linktext = extlink_linkname + ' (See above)'
                    break    # found a matching previous linkname so stop looking
            if extlink_linktext == None or len(extlink_linktext) < 20:
                exlink_linktext = urltitle
        # Look for a news web site
        for (sitename, newscompany, stripprefix) in newssites:
            if refname.startswith( sitename ):
            # If there is a prefix to strip from the title
                if stripprefix and extlink_linktext.startswith(stripprefix):
                    extlink_linktext = extlink_linktext[len(stripprefix):]
                    new_text = u'{{news reference | title=%s | url=%s | urldate=%s | org=%s }}' % ( extlink_linktext, extlink_linkname, now.isoformat(), newscompany ) + '\n'
                    break
        else:        # else no special site found
            new_text = u'{{web reference | title=%s | url=%s | date=%s }}' % ( extlink_linktext, extlink_linkname, now.isoformat() )
        return (new_text)

    def doConvertDOIToReference(self, refsequence, doi_linktext):
        """
        Returns the text which is generated by converting a DOI reference to
        a format suitable for the Notes section.
        """
        # TODO: look up DOI info and create full reference
        urltitle = self.doGetTitleFromURL('http://dx.doi.org/' + doi_linktext ) # try to get title from URL
        refname = 'refbot%d' % refsequence
        if urltitle:
            new_text = '# {{note|%s}} %s {{doi|%s}}\n' \
                       % (refname, urltitle, doi_linktext)
        else:
            new_text = '# {{note|%s}} {{doi|%s}}\n' \
                       % (refname, doi_linktext)
        return (refname, new_text)

    def doBuildSequenceListOfReferences(self, original_text):
        """
        Returns a list with all found references and sequence numbers.
        """
        duplicatefound = False
        refusage = {}
        # Data structure: refusage[reference_key] = [ sequence_in_document, count, count_during_dup_handling ]
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check for various references
            Rtext_line = re.compile(r'(?i){{(?P<reftype>ref|ref_num|ref_label)\|(?P<refname>[^}|]+?)}}')
            m = Rtext_line.search( text_line )
            while m:    # if found a reference
                if m.group('reftype').lower() in ('ref', 'ref_num', 'ref_label'):    # confirm ref
                    refkey = m.group('refname').strip()
                    if refkey != '':
                        if refkey in refusage:
                            refusage[refkey][1] += 1    # duplicate use of reference
                            duplicatefound = True
                        else:
                            refusage[refkey] = [len(refusage),0,0]    # remember this reference
                m = Rtext_line.search( text_line, m.end() )
        pywikibot.output( u'Number of refs: %d' % (len(refusage)) )
        return (duplicatefound, refusage)

    def doReadReferencesSection(self, original_text, refsectionname):
        """
        Returns the text which is generated by reading the Notes section.
        Also appends references to self.references.
        Contents of all Notes sections will be read.
        """
        # TODO: support subsections within Notes
        new_text = ''
        intargetsection = False
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check for target section
            m = re.search( r'==+(?P<sectionname>[^=]+)==', text_line )
            if m:    # if in a section, check if Notes section
                if refsectionname != '':    # if a certain section name has been identified
                    m_section = m.group('sectionname')
                    pywikibot.output(u'Looking for "%s": "%s"'
                                    % (refsectionname,m_section) )
                    if unicode(m_section.strip()) == unicode(refsectionname):
                        pywikibot.output(u'Read Ref section.')
                        intargetsection = True
                        new_text = new_text + text_line
                    else:
                        intargetsection = False
                else:                # else grab all possible sections
                    if m.group('sectionname').lower().strip() in referencesectionnames:
                        intargetsection = True
                        new_text = new_text + text_line
                    else:
                        intargetsection = False
            else:
                if intargetsection:    # if inside target section, remember this reference line
                    if text_line.strip() != '':
                        if text_line.lstrip()[0] in u'[{':    # if line starts with non-Ref WikiSyntax
                            intargetsection = False        # flag as not being in section
                        # TODO: need better way to handle special cases at end of refs
                        if text_line.strip() == u'<!--READ ME!! PLEASE DO NOT JUST ADD NEW NOTES AT THE BOTTOM. See the instructions above on ordering. -->':    # This line ends some Notes sections
                            intargetsection = False        # flag as not being in section
                        if text_line.strip() == u'</div>':    # This line ends some Notes sections
                            intargetsection = False        # flag as not being in section
                    if intargetsection:    # if still inside target section
                        # Convert any # wiki list to *; will be converted later if a reference
                        if text_line[0] == '#':
                            text_line = '*' + text_line[1:]
                        self.references.append(text_line.rstrip() + u'\n')
                        new_text = new_text + text_line.rstrip() + u'\n'
        return new_text

    def doReorderReferences(self, references, refusage):
        """
        Returns the new references list after reordering to match refusage list
        Non-references are moved to top, unused references to bottom.
        """
        # TODO: add tests for duplicate references/Ibid handling.
        newreferences = references
        if references != [] and refusage != {}:
            newreferences = []
            for i in range(len(references)):        # move nonrefs to top of list
                text_line = references[i]
                # TODO: compile search?
                m = re.search(r'(?i)[*#][\s]*{{(?P<reftype>note)\|(?P<refname>[^}|]+?)}}', text_line)
                # Special test to ignore Footnote instructions comment.
                text_line_stripped = text_line.strip()
                if text_line_stripped.startswith(u'4) Add ') or not m:    # if no ref found
                    newreferences.append(text_line)    # add nonref to new list
                    references[i] = None
            refsort = {}
            for refkey in refusage.keys():        # build list of keys in document order
                refsort[ refusage[refkey][0] ] = refkey    # refsort contains reference key names
            alphabet26 = u'abcdefghijklmnopqrstuvwxyz'
            for i in range(len(refsort)):        # collect references in document order
                for search_num in range(len(references)):    # find desired entry
                    search_line = references[search_num]
                    if search_line:
                        # TODO: compile search?
                        # Note that the expression finds all neighboring note|note_label expressions.
                        m2 = re.search(r'(?i)[*#]([\s]*{{(?P<reftype>note|note_label)\|(?P<refname>[^}|]+?)}})+', search_line)
                        if m2:
                            refkey = m2.group('refname').strip()
                            if refkey == refsort[i]:    # if expected ref found
                                # Rewrite references
                                note_text = '# {{note|%s}}' % refkey    # rewrite note tag
                                if refusage[refkey][1] > 1:        # if more than one reference to citation
                                    for n in range(refusage[refkey][1]):    # loop through all repetitions
                                        note_text = note_text + '{{note_label|%s|%d|%s}}' % (refkey,(refusage[refkey][0])+1,alphabet26[n%26])
                                search_line=search_line[:m2.start(0)] + note_text + search_line[m2.end(0):]
                                newreferences.append(search_line)    # found, add entry
                                del references[search_num]        # delete used reference
                                break    # stop the search loop after entry found
            newreferences = newreferences + references        # append any unused references
        return newreferences

    def doUpdateReferencesSection(self, original_text, refusage, refsectionname):
        """
        Returns the text which is generated by rebuilding the Notes section.
        Rewrite Notes section from references list.
        """
        new_text = ''
        intargetsection = False
        for text_line in original_text.splitlines(True):  # Scan all text line by line
            # Check for target section
            m = re.search( r'==+(?P<sectionname>[^=]+)==', text_line )
            if m:    # if in a section, check if Notes section
                if refsectionname != '':    # if a certain section name has been identified
                    m_section = m.group('sectionname')
                    pywikibot.output( u'Looking for "%s": "%s"' % (refsectionname,m_section) )
                    if unicode(m_section.strip()) == unicode(refsectionname):
                        pywikibot.output( u'Updating Ref section.')
                        intargetsection = True        # flag as being in section
                    else:
                        intargetsection = False        # flag as not being in section
                else:                # else grab all possible sections
                    if m.group('sectionname').lower().strip() in referencesectionnames:
                        intargetsection = True        # flag as being in section
                    else:
                        intargetsection = False        # flag as not being in section
                if intargetsection:
                    new_text = new_text + text_line    # append new line to new text
                    if self.references != []:
                        for newref in self.references:        # scan through all references
                            if newref != None:
                                new_text = new_text + newref.rstrip() + u'\n'    # insert references
                        new_text = new_text + u'\n'    # one trailing blank line
                        self.references = []            # empty references
                else:
                    new_text = new_text + text_line    # copy section headline
            else:
                if intargetsection:
                    if text_line.strip() != '':
                        if text_line.lstrip()[0] in u'[{':    # if line starts with non-Ref WikiSyntax
                            intargetsection = False        # flag as not being in section
                        # TODO: need better way to handle special cases at end of refs
                        if text_line.strip() == u'<!--READ ME!! PLEASE DO NOT JUST ADD NEW NOTES AT THE BOTTOM. See the instructions above on ordering. -->':    # This line ends some Notes sections
                            intargetsection = False        # flag as not being in section
                        if text_line.strip() == u'</div>':    # This line ends some Notes sections
                            intargetsection = False        # flag as not being in section
                if not intargetsection:            # if not in Notes section, remember line
                    new_text = new_text + text_line    # append new line to new text
        # If references list not emptied, there was no Notes section found
        if self.references != []:
            # New Notes section needs to be created at bottom.
            text_line_counter = 0        # current line
            last_text_line_counter_value = 0    # number of last line of possible text
            for text_line in original_text.splitlines(True):  # Search for last normal text line
                text_line_counter += 1        # count this line
                if text_line.strip() != '':
                    if text_line.lstrip()[0].isalnum():    # if line starts with alphanumeric
                        last_text_line_counter = text_line_counter    # number of last line of possible text
                    else:
                        if text_line.lstrip()[0] in u'<=!|*#':    # if line starts with recognized wiki char
                            if not text_line.startswith(u'<!--'):    # if line not start with a comment
                                last_text_line_counter = text_line_counter    # number of last line of possible content
            new_text = ''            # erase previous new_text
            text_line_counter = 0        # current line
            for text_line in original_text.splitlines(True):  # Search for last normal text line
                text_line_counter += 1        # count this line
                if last_text_line_counter == text_line_counter:    # if found insertion point
                    new_text = new_text + text_line    # append new line to new text
                    new_text = new_text + '\n== Notes ==\n'    # set to standard name
                    new_text = new_text + u'{{subst:Footnote3text}}\n'
                    if self.references != []:
                        for newref in self.references:        # scan through all references
                            if newref is not None:
                                new_text = new_text + newref    # insert references
                        new_text = new_text + u'\n'    # one trailing blank line
                        self.references = []            # empty references
                else:
                    new_text = new_text + text_line    # append new line to new text
        if new_text == '':
            new_text = original_text    # If somehow no new text, return original text
        return new_text

    def run(self):
        """
        Starts the robot.
        """
        # Run the generator which will yield Pages to pages which might need to be
        # changed.
        for pl in self.generator:
            print ''
            try:
                # Load the page's text from the wiki
                original_text = pl.get()
                if pl.editRestriction:
                    pywikibot.output(u'Skipping locked page %s' % pl.title())
                    continue
            except pywikibot.NoPage:
                pywikibot.output(u'Page %s not found' % pl.title())
                continue
            except pywikibot.IsRedirectPage:
                continue
            match = self.checkExceptions(original_text)
            # skip all pages that contain certain texts
            if match:
                pywikibot.output(u'Skipping %s because it contains %s'
                                 % (pl.title(), match))
            else:
                new_text = self.doReplacements(original_text)
                if new_text == original_text:
                    pywikibot.output('No changes were necessary in %s'
                                     % pl.title())
                else:
                    pywikibot.output(u'>>> %s <<<' % pl.title())
                    pywikibot.showDiff(original_text, new_text)
                    if not self.acceptall:
                        choice = pywikibot.input(
                            u'Do you want to accept these changes? [y|n|a(ll)]')
                        if choice in ['a', 'A']:
                            self.acceptall = True
                    if self.acceptall or choice in ['y', 'Y']:
                        pl.put(new_text, self.summary)

def main():
    # How we want to retrieve information on which pages need to be changed.
    # Can either be 'sqldump', 'textfile' or 'userinput'.
    source = None
    # Array which will collect commandline parameters.
    # First element is original text, second element is replacement text.
    commandline_replacements = []
    # A dictionary where keys are original texts and values are replacement texts.
    replacements = {}
    # Don't edit pages which contain certain texts.
    exceptions = []
    # Should the elements of 'replacements' and 'exceptions' be interpreted
    # as regular expressions?
    regex = False
    # the dump's path, either absolute or relative, which will be used when source
    # is 'sqldump'.
    sqlfilename = None
    # the textfile's path, either absolute or relative, which will be used when
    # source is 'textfile'.
    textfilename = None
    # the category name which will be used when source is 'category'.
    categoryname = None
    # a list of pages which will be used when source is 'userinput'.
    pagenames = []
    # will become True when the user presses a ('yes to all') or uses the -always
    # commandline paramater.
    acceptall = False
    # Which namespace should be processed when using a SQL dump
    # default to -1 which means all namespaces will be processed
    namespace = -1
    # Load default summary message.
    editSummary = pywikibot.translate(pywikibot.getSite(), msg)
    # List of references in Notes section
    references = []
    # Notes sequence number
    refsequence = random.randrange(1000)
    # Dictionary of references used in sequence
    refusage = {}

    # Read commandline parameters.
    for arg in pywikibot.handleArgs():
        if arg == '-regex':
            regex = True
        elif arg.startswith('-file'):
            if len(arg) == 5:
                textfilename = pywikibot.input(u'Please enter the filename:')
            else:
                textfilename = arg[6:]
            source = 'textfile'
        elif arg.startswith('-cat'):
            if len(arg) == 4:
                categoryname = pywikibot.input(
                    u'Please enter the category name:')
            else:
                categoryname = arg[5:]
            source = 'category'
        elif arg.startswith('-sql'):
            if len(arg) == 4:
                sqlfilename = pywikibot.input(
                    u'Please enter the SQL dump\'s filename:')
            else:
                sqlfilename = arg[5:]
            source = 'sqldump'
        elif arg.startswith('-page'):
            if len(arg) == 5:
                pagenames.append(
                    pywikibot.input(u'Which page do you want to change?'))
            else:
                pagenames.append(arg[6:])
            source = 'userinput'
        elif arg.startswith('-except:'):
            exceptions.append(arg[8:])
        elif arg == '-always':
            acceptall = True
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[11:]))
            except ValueError:
                namespaces.append(arg[11:])
        else:
            commandline_replacements.append(arg)

    if source == None or len(commandline_replacements) not in [0, 2]:
        # syntax error, show help text from the top of this file
        pywikibot.output(__doc__, 'utf-8')
        return
    if (len(commandline_replacements) == 2):
        replacements[commandline_replacements[0]] = commandline_replacements[1]
        editSummary = pywikibot.translate(pywikibot.getSite(), msg)
        % ' (-' + commandline_replacements[0] + ' +' + commandline_replacements[1] + ')'
    else:
        change = ''
        default_summary_message =  pywikibot.translate(pywikibot.getSite(), msg) % change
        pywikibot.output(u'The summary message will default to: %s'
                         % default_summary_message)
        summary_message = pywikibot.input(
            u'Press Enter to use this default message, or enter a description of the changes your bot will make:')
        if summary_message == '':
            summary_message = default_summary_message
        editSummary = summary_message

        # Perform the predefined actions.
        try:
            fix = fixes['ALTREFS']
        except KeyError:
            pywikibot.output(u'Available predefined fixes are: %s'
                             % fixes.keys())
            return
        if 'regex' in fix:
            regex = fix['regex']
        if 'msg' in fix:
            editSummary = pywikibot.translate(pywikibot.getSite(), fix['msg'])
        if 'exceptions' in fix:
            exceptions = fix['exceptions']
        replacements = fix['replacements']

    gen = ReplacePageGenerator(source, replacements, exceptions, regex,
                               namespace, textfilename, sqlfilename,
                               categoryname, pagenames)
    preloadingGen = pagegenerators.PreloadingGenerator(gen, pageNumber = 20)
    bot = ReplaceRobot(preloadingGen, replacements, refsequence, references,
                       refusage, exceptions, regex, acceptall, editSummary)
    bot.run()


if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
