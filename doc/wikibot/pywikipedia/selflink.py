#!/usr/bin/python
# -*- coding: utf-8  -*-

"""
This bot goes over multiple pages of the home wiki, searches for selflinks, and
allows removing them.

These command line parameters can be used to specify which pages to work on:

&params;

    -xml           Retrieve information from a local XML dump (pages-articles
                   or pages-meta-current, see http://download.wikimedia.org).
                   Argument can also be given as "-xml:filename".

    -always        Unlink always but don't prompt you for each replacement.
                   ATTENTION: Use this with care!

    -namespace:n   Number of namespace to process. The parameter can be used
                   multiple times. It works in combination with all other
                   parameters, except for the -start parameter. If you e.g.
                   want to iterate over all categories starting at M, use
                   -start:Category:M.

All other parameters will be regarded as part of the title of a single page,
and the bot will only work on that single page.
"""
#
# (C) Pywikipedia bot team, 2006-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: selflink.py 9146 2011-04-06 16:04:58Z xqt $'
#

import re, sys
import wikipedia as pywikibot
import pagegenerators, catlib
import editarticle

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;':     pagegenerators.parameterHelp,
}

# Summary messages in different languages
# NOTE: Predefined replacement tasks might use their own dictionary, see 'fixes'
# in fixes.py.
msg = {
    'ar':u'روبوت: إزالة وصلات ذاتية',
    'cs':u'Robot odstranil odkaz na název článku',
    'da':u'Bot: fjerner selvreference',
    'de':u'Bot: Entferne Selbstlinks',
    'en':u'Robot: Removing selflinks',
    'es':u'Bot: Eliminando enlaces al mismo artículo',
    'fa':u'ربات:برداشتن پیوند به خود',
    'fr':u'Robot: Enlève autoliens',
    'he':u'בוט: מסיר קישורים של הדף לעצמו',
    'hu':u'Bot: Önmagukra mutató hivatkozások eltávolítása',
    'ja':u'ロボットによる 自己リンクの解除',
    'ksh':u'Bot: Ene Lengk vun de Sigg op sesch sellver, erus jenumme.',
    'nl':u'Bot: verwijzingen naar pagina zelf verwijderd',
    'nn':u'robot: fjerna sjølvlenkjer',
    'no':u'robot: fjerner selvlenker',
    'pl':u'Robot automatycznie usuwa linki zwrotne',
    'pt':u'Bot: Retirando link para o próprio artigo',
    'ru':u'Бот: удалил заголовок-ссылку в тексте. см. ',
    'zh':u'機器人:移除自我連結',
}

class XmlDumpSelflinkPageGenerator:
    """
    Generator which will yield Pages that might contain selflinks.
    These pages will be retrieved from a local XML dump file
    (cur table).
    """
    def __init__(self, xmlFilename):
        """
        Arguments:
            * xmlFilename  - The dump's path, either absolute or relative
        """

        self.xmlFilename = xmlFilename

    def __iter__(self):
        import xmlreader
        mysite = pywikibot.getSite()
        dump = xmlreader.XmlDump(self.xmlFilename)
        for entry in dump.parse():
            if mysite.nocapitalize:
                title = re.escape(entry.title)
            else:
                title = '[%s%s]%s' % (re.escape(entry.title[0].lower()),
                                      re.escape(entry.title[0].upper()),
                                      re.escape(entry.title[1:]))
            selflinkR = re.compile(r'\[\[' + title + '(\|[^\]]*)?\]\]')
            if selflinkR.search(entry.text):
                yield pywikibot.Page(mysite, entry.title)
                continue

class SelflinkBot:

    def __init__(self, generator, always=False):
        self.generator = generator
        linktrail = pywikibot.getSite().linktrail()
        # The regular expression which finds links. Results consist of four groups:
        # group title is the target page title, that is, everything before | or ].
        # group section is the page section. It'll include the # to make life easier for us.
        # group label is the alternative link title, that's everything between | and ].
        # group linktrail is the link trail, that's letters after ]] which are part of the word.
        # note that the definition of 'letter' varies from language to language.
        self.linkR = re.compile(r'\[\[(?P<title>[^\]\|#]*)(?P<section>#[^\]\|]*)?(\|(?P<label>[^\]]*))?\]\](?P<linktrail>' + linktrail + ')')
        self.always = always
        self.done = False

    def handleNextLink(self, page, text, match, context = 100):
        """
        Returns a tuple (text, jumpToBeginning).
        text is the unicode string after the current link has been processed.
        jumpToBeginning is a boolean which specifies if the cursor position
        should be reset to 0. This is required after the user has edited the
        article.
        """
        # ignore interwiki links and links to sections of the same page as well
        # as section links
        if not match.group('title') \
           or page.site().isInterwikiLink(match.group('title')) \
           or match.group('section'):
            return text, False
        try:
            linkedPage = pywikibot.Page(page.site(), match.group('title'))
        except pywikibot.InvalidTitle, err:
            pywikibot.output(u'Warning: %s' % err)
            return text, False

        # Check whether the link found is to the current page itself.
        if linkedPage != page:
            # not a self-link
            return text, False
        else:
            # at the beginning of the link, start red color.
            # at the end of the link, reset the color to default
            if self.always:
                choice = 'a'
            else:
                pywikibot.output(
                    text[max(0, match.start() - context) : match.start()] \
                    + '\03{lightred}' + text[match.start() : match.end()] \
                    + '\03{default}' + text[match.end() : match.end() + context])
                choice = pywikibot.inputChoice(
                    u'\nWhat shall be done with this selflink?\n',
                    ['unlink', 'make bold', 'skip', 'edit', 'more context',
                     'unlink all', 'quit'],
                    ['U', 'b', 's', 'e', 'm', 'a', 'q'], 'u')
                pywikibot.output(u'')

                if choice == 's':
                    # skip this link
                    return text, False
                elif choice == 'e':
                    editor = editarticle.TextEditor()
                    newText = editor.edit(text, jumpIndex = match.start())
                    # if user didn't press Cancel
                    if newText:
                        return newText, True
                    else:
                        return text, True
                elif choice == 'm':
                    # show more context by recursive self-call
                    return self.handleNextLink(page, text, match,
                                               context=context + 100)
                elif choice == 'a':
                    self.always = True
                elif choice == 'q':
                    self.done = True
                    return text, False
            new = match.group('label') or match.group('title')
            new += match.group('linktrail')
            if choice == 'b':
                # make bold
                return text[:match.start()] + "'''" + new + "'''" + text[match.end():], False
            else:
                return text[:match.start()] + new + text[match.end():], False

    def treat(self, page):
        # Show the title of the page we're working on.
        # Highlight the title in purple.
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                         % page.title())
        try:
            oldText = page.get()
            # Inside image maps, don't touch selflinks, as they're used
            # to create tooltip labels. See for example:
            # http://de.wikipedia.org/w/index.php?title=Innenstadt_%28Bautzen%29&diff=next&oldid=35721641
            if '<imagemap>' in oldText:
                pywikibot.output(
                    u'Skipping page %s because it contains an image map.'
                    % page.title(asLink=True))
                return
            text = oldText
            curpos = 0
            while curpos < len(text):
                match = self.linkR.search(text, pos = curpos)
                if not match:
                    break
                # Make sure that next time around we will not find this same
                # hit.
                curpos = match.start() + 1
                text, jumpToBeginning = self.handleNextLink(page, text, match)
                if jumpToBeginning:
                    curpos = 0

            if oldText == text:
                pywikibot.output(u'No changes necessary.')
            else:
                pywikibot.showDiff(oldText, text)
                page.put_async(text)
        except pywikibot.NoPage:
            pywikibot.output(u"Page %s does not exist?!"
                             % page.title(asLink=True))
        except pywikibot.IsRedirectPage:
            pywikibot.output(u"Page %s is a redirect; skipping."
                             % page.title(asLink=True))
        except pywikibot.LockedPage:
            pywikibot.output(u"Page %s is locked?!" % page.title(asLink=True))

    def run(self):
        comment = pywikibot.translate(pywikibot.getSite(), msg)
        pywikibot.setAction(comment)

        for page in self.generator:
            if self.done: break
            self.treat(page)

def main():
    #page generator
    gen = None
    # This temporary array is used to read the page title if one single
    # page to work on is specified by the arguments.
    pageTitle = []
    # Which namespaces should be processed?
    # default to [] which means all namespaces will be processed
    namespaces = []
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()
    always = False

    for arg in pywikibot.handleArgs():
        if arg.startswith('-xml'):
            if len(arg) == 4:
                xmlFilename = pywikibot.input(
                    u'Please enter the XML dump\'s filename:')
            else:
                xmlFilename = arg[5:]
            gen = XmlDumpSelflinkPageGenerator(xmlFilename)
        elif arg == '-sql':
            # NOT WORKING YET
            query = """
SELECT page_namespace, page_title
FROM page JOIN pagelinks JOIN text ON (page_id = pl_from AND page_id = old_id)
WHERE pl_title = page_title
AND pl_namespace = page_namespace
AND page_namespace = 0
AND (old_text LIKE concat('%[[', page_title, ']]%')
    OR old_text LIKE concat('%[[', page_title, '|%'))
LIMIT 100"""
            gen = pagegenerators.MySQLPageGenerator(query)
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[11:]))
            except ValueError:
                namespaces.append(arg[11:])
        elif arg == '-always':
            always = True
        else:
            if not genFactory.handleArg(arg):
                pageTitle.append(arg)

    if pageTitle:
        page = pywikibot.Page(pywikibot.getSite(), ' '.join(pageTitle))
        gen = iter([page])
    if not gen:
        gen = genFactory.getCombinedGenerator()
    if not gen:
        pywikibot.showHelp('selflink')
    else:
        if namespaces != []:
            gen =  pagegenerators.NamespaceFilterPageGenerator(gen, namespaces)
        preloadingGen = pagegenerators.PreloadingGenerator(gen)
        bot = SelflinkBot(preloadingGen, always)
        bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
