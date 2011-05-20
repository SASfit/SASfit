# -*- coding: utf-8  -*-
"""
Library to get and put pages on a MediaWiki.

Contents of the library (objects and functions to be used outside)

Classes:
    Page(site, title): A page on a MediaWiki site
    ImagePage(site, title): An image descriptor Page
    Site(lang, fam): A MediaWiki site

Factory functions:
    Family(name): Import the named family
    getSite(lang, fam): Return a Site instance

Exceptions:
    Error:              Base class for all exceptions in this module
    NoUsername:         Username is not in user-config.py
    NoPage:             Page does not exist on the wiki
    NoSuchSite:         Site does not exist
    IsRedirectPage:     Page is a redirect page
    IsNotRedirectPage:  Page is not a redirect page
    LockedPage:         Page is locked
    SectionError:       The section specified in the Page title does not exist
    PageNotSaved:       Saving the page has failed
      EditConflict:     PageNotSaved due to edit conflict while uploading
      SpamfilterError:  PageNotSaved due to MediaWiki spam filter
      LongPageError:    PageNotSaved due to length limit
    ServerError:        Got unexpected response from wiki server
    BadTitle:           Server responded with BadTitle
    UserBlocked:        Client's username or IP has been blocked
    PageNotFound:       Page not found in list

Objects:
    get_throttle:       Call to limit rate of read-access to wiki
    put_throttle:       Call to limit rate of write-access to wiki

Other functions:
    getall(): Load a group of pages via Special:Export
    handleArgs(): Process all standard command line arguments (such as
        -family, -lang, -log and others)
    translate(xx, dict): dict is a dictionary, giving text depending on
        language, xx is a language. Returns the text in the most applicable
        language for the xx: wiki
    setAction(text): Use 'text' instead of "Wikipedia python library" in
        edit summaries
    setUserAgent(text): Sets the string being passed to the HTTP server as
        the User-agent: header. Defaults to 'Pywikipediabot/1.0'.

    output(text): Prints the text 'text' in the encoding of the user's
        console. **Use this instead of "print" statements**
    input(text): Asks input from the user, printing the text 'text' first.
    inputChoice: Shows user a list of choices and returns user's selection.

    showDiff(oldtext, newtext): Prints the differences between oldtext and
        newtext on the screen

Wikitext manipulation functions: each of these takes a unicode string
containing wiki text as its first argument, and returns a modified version
of the text unless otherwise noted --

    replaceExcept: replace all instances of 'old' by 'new', skipping any
        instances of 'old' within comments and other special text blocks
    removeDisabledParts: remove text portions exempt from wiki markup
    isDisabled(text,index): return boolean indicating whether text[index] is
        within a non-wiki-markup section of text
    decodeEsperantoX: decode Esperanto text using the x convention.
    encodeEsperantoX: convert wikitext to the Esperanto x-encoding.
    sectionencode: encode text for use as a section title in wiki-links.
    findmarker(text, startwith, append): return a string which is not part
        of text
    expandmarker(text, marker, separator): return marker string expanded
        backwards to include separator occurrences plus whitespace

Wikitext manipulation functions for interlanguage links:

    getLanguageLinks(text,xx): extract interlanguage links from text and
        return in a dict
    removeLanguageLinks(text): remove all interlanguage links from text
    removeLanguageLinksAndSeparator(text, site, marker, separator = ''):
        remove language links, whitespace, preceeding separators from text
    replaceLanguageLinks(oldtext, new): remove the language links and
        replace them with links from a dict like the one returned by
        getLanguageLinks
    interwikiFormat(links): convert a dict of interlanguage links to text
        (using same dict format as getLanguageLinks)
    interwikiSort(sites, inSite): sorts a list of sites according to interwiki
        sort preference of inSite.
    url2link: Convert urlname of a wiki page into interwiki link format.

Wikitext manipulation functions for category links:

    getCategoryLinks(text): return list of Category objects corresponding
        to links in text
    removeCategoryLinks(text): remove all category links from text
    replaceCategoryLinksAndSeparator(text, site, marker, separator = ''):
        remove language links, whitespace, preceeding separators from text
    replaceCategoryLinks(oldtext,new): replace the category links in oldtext by
        those in a list of Category objects
    replaceCategoryInPlace(text,oldcat,newtitle): replace a single link to
        oldcat with a link to category given by newtitle
    categoryFormat(links): return a string containing links to all
        Categories in a list.

Unicode utility functions:
    UnicodeToAsciiHtml: Convert unicode to a bytestring using HTML entities.
    url2unicode: Convert url-encoded text to unicode using a site's encoding.
    unicode2html: Ensure unicode string is encodable; if not, convert it to
        ASCII for HTML.
    html2unicode: Replace HTML entities in text with unicode characters.

stopme(): Put this on a bot when it is not or not communicating with the Wiki
    any longer. It will remove the bot from the list of running processes,
    and thus not slow down other bot threads anymore.

"""
from __future__ import generators
#
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: wikipedia.py 9212 2011-05-04 03:59:16Z xqt $'

import os, sys
import httplib, socket, urllib, urllib2, cookielib
import traceback
import time, threading, Queue
import math
import re, codecs, difflib, locale
try:
    from hashlib import md5
except ImportError:             # Python 2.4 compatibility
    from md5 import new as md5
import xml.sax, xml.sax.handler
import htmlentitydefs
import warnings
import unicodedata
import xmlreader
from BeautifulSoup import BeautifulSoup, BeautifulStoneSoup, SoupStrainer
import weakref
# Splitting the bot into library parts
from pywikibot import *

# Set the locale to system default. This will ensure correct string
# handling for non-latin characters on Python 2.3.x. For Python 2.4.x it's no
# longer needed.
locale.setlocale(locale.LC_ALL, '')

import config, login, query, version

try:
    set # introduced in Python2.4: faster and future
except NameError:
    from sets import Set as set

# Check Unicode support (is this a wide or narrow python build?)
# See http://www.python.org/doc/peps/pep-0261/
try:
    unichr(66365)  # a character in th: alphabet, uses 32 bit encoding
    WIDEBUILD = True
except ValueError:
    WIDEBUILD = False


SaxError = xml.sax._exceptions.SAXParseException

# Pre-compile re expressions
reNamespace = re.compile("^(.+?) *: *(.*)$")
Rwatch = re.compile(
         r"<input type='hidden' value=\"(.*?)\" name=\"wpEditToken\"")
Rwatchlist = re.compile(r"<input tabindex='[\d]+' type='checkbox' "
                        r"name='wpWatchthis' checked='checked'")
Rlink = re.compile(r'\[\[(?P<title>[^\]\|\[]*)(\|[^\]]*)?\]\]')
resectiondecodeescapes = re.compile(r"\.(?=[0-9a-f]{2})",re.I)
resectiondecodeleadingnonalpha = re.compile(r'^x(?=[^a-zA-Z])')


class Page(object):
    """Page: A MediaWiki page

    Constructor has two required parameters:
      1) The wiki Site on which the page resides [note that, if the
         title is in the form of an interwiki link, the Page object may
         have a different Site than this]
      2) The title of the page as a unicode string

    Optional parameters:
      insite - the wiki Site where this link was found (to help decode
               interwiki links)
      defaultNamespace - A namespace to use if the link does not contain one

    Methods available:

    title                 : The name of the page, including namespace and
                            section if any
    urlname               : Title, in a form suitable for a URL
    namespace             : The namespace in which the page is found
    titleWithoutNamespace : Title, with the namespace part removed
    section               : The section of the page (the part of the title
                            after '#', if any)
    sectionFreeTitle      : Title, without the section part
    aslink                : Title in the form [[Title]] or [[lang:Title]]
    site                  : The wiki this page is in
    encoding              : The encoding of the page
    isAutoTitle           : Title can be translated using the autoFormat method
    autoFormat            : Auto-format certain dates and other standard
                            format page titles
    isCategory            : True if the page is a category
    isDisambig (*)        : True if the page is a disambiguation page
    isImage               : True if the page is an image
    isRedirectPage (*)    : True if the page is a redirect, false otherwise
    getRedirectTarget (*) : The page the page redirects to
    isTalkPage            : True if the page is in any "talk" namespace
    toggleTalkPage        : Return the talk page (if this is one, return the
                            non-talk page)
    get (*)               : The text of the page
    latestRevision (*)    : The page's current revision id
    userName              : Last user to edit page
    isIpEdit              : True if last editor was unregistered
    editTime              : Timestamp of the last revision to the page
    previousRevision (*)  : The revision id of the previous version
    permalink (*)         : The url of the permalink of the current version
    getOldVersion(id) (*) : The text of a previous version of the page
    getRestrictions       : Returns a protection dictionary
    getVersionHistory     : Load the version history information from wiki
    getVersionHistoryTable: Create a wiki table from the history data
    fullVersionHistory    : Return all past versions including wikitext
    contributingUsers     : Return set of users who have edited page
    exists (*)            : True if the page actually exists, false otherwise
    isEmpty (*)           : True if the page has 4 characters or less content,
                            not counting interwiki and category links
    interwiki (*)         : The interwiki links from the page (list of Pages)
    categories (*)        : The categories the page is in (list of Pages)
    linkedPages (*)       : The normal pages linked from the page (list of
                            Pages)
    imagelinks (*)        : The pictures on the page (list of ImagePages)
    templates (*)         : All templates referenced on the page (list of
                            Pages)
    templatesWithParams(*): All templates on the page, with list of parameters
    getReferences         : List of pages linking to the page
    canBeEdited (*)       : True if page is unprotected or user has edit
                            privileges
    protection(*)         : This page protection level
    botMayEdit (*)        : True if bot is allowed to edit page
    put(newtext)          : Saves the page
    put_async(newtext)    : Queues the page to be saved asynchronously
    watch                 : Add the page to the watchlist
    unwatch               : Remove the page from the watchlist
    move                  : Move the page to another title
    delete                : Deletes the page (requires being logged in)
    protect               : Protect or unprotect a page (requires sysop status)
    removeImage           : Remove all instances of an image from this page
    replaceImage          : Replace all instances of an image with another
    loadDeletedRevisions  : Load all deleted versions of this page
    getDeletedRevision    : Return a particular deleted revision
    markDeletedRevision   : Mark a version to be undeleted, or not
    undelete              : Undelete past version(s) of the page

    (*) : This loads the page if it has not been loaded before; permalink might
          even reload it if it has been loaded before

    """
    def __init__(self, site, title, insite=None, defaultNamespace=0):
        try:
            # if _editrestriction is True, it means that the page has been found
            # to have an edit restriction, but we do not know yet whether the
            # restriction affects us or not
            self._editrestriction = False

            if site is None:
                site = getSite()
            elif type(site) in [str, unicode]:
                site = getSite(site)

            self._site = site

            if not insite:
                insite = site

            # Convert HTML entities to unicode
            t = html2unicode(title)

            # Convert URL-encoded characters to unicode
            # Sometimes users copy the link to a site from one to another.
            # Try both the source site and the destination site to decode.
            try:
                t = url2unicode(t, site = insite, site2 = site)
            except UnicodeDecodeError:
                raise InvalidTitle(u'Bad page title : %s' % t)

            # Normalize unicode string to a NFC (composed) format to allow
            # proper string comparisons. According to
            # http://svn.wikimedia.org/viewvc/mediawiki/branches/REL1_6/phase3/includes/normal/UtfNormal.php?view=markup
            # the mediawiki code normalizes everything to NFC, not NFKC
            # (which might result in information loss).
            t = unicodedata.normalize('NFC', t)

            # Clean up the name, it can come from anywhere.
            # Replace underscores by spaces, also multiple spaces and underscores with a single space
            t = t.replace(u"_", u" ")
            while u"  " in t:
                t = t.replace(u"  ", u" ")
            # Strip spaces at both ends
            t = t.strip(u" ")
            # Remove left-to-right and right-to-left markers.
            t = t.replace(u'\u200e', '').replace(u'\u200f', '')
            # leading colon implies main namespace instead of the default
            if t.startswith(':'):
                t = t[1:]
                self._namespace = 0
            else:
                self._namespace = defaultNamespace

            if not t:
                raise InvalidTitle(u"Invalid title '%s'" % title )

            self._namespace = defaultNamespace
            #
            # This code was adapted from Title.php : secureAndSplit()
            #
            # Namespace or interwiki prefix
            while True:
                m = reNamespace.match(t)
                if not m:
                    break
                p = m.group(1)
                lowerNs = p.lower()
                ns = self._site.getNamespaceIndex(lowerNs)
                if ns:
                    t = m.group(2)
                    self._namespace = ns
                    break

                if lowerNs in self._site.family.langs.keys():
                    # Interwiki link
                    t = m.group(2)

                    # Redundant interwiki prefix to the local wiki
                    if lowerNs == self._site.lang:
                        if t == '':
                            raise Error("Can't have an empty self-link")
                    else:
                        self._site = getSite(lowerNs, self._site.family.name)

                    # If there's an initial colon after the interwiki, that also
                    # resets the default namespace
                    if t != '' and t[0] == ':':
                        self._namespace = 0
                        t = t[1:]
                elif lowerNs in self._site.family.get_known_families(site = self._site):
                    if self._site.family.get_known_families(site = self._site)[lowerNs] == self._site.family.name:
                        t = m.group(2)
                    else:
                        # This page is from a different family
                        if verbose:
                            output(u"Target link '%s' has different family '%s'" % (title, lowerNs))
                        if self._site.family.name in ['commons', 'meta']:
                            #When the source wiki is commons or meta,
                            #w:page redirects you to w:en:page
                            otherlang = 'en'
                        else:
                            otherlang = self._site.lang
                        familyName = self._site.family.get_known_families(site = self._site)[lowerNs]
                        if familyName in ['commons', 'meta']:
                            otherlang = familyName
                        try:
                            self._site = getSite(otherlang, familyName)
                        except ValueError:
                            raise NoPage("""\
%s is not a local page on %s, and the %s family is
not supported by PyWikipediaBot!"""
                              % (title, self._site, familyName))
                        t = m.group(2)
                else:
                    # If there's no recognized interwiki or namespace,
                    # then let the colon expression be part of the title.
                    break

            sectionStart = t.find(u'#')
            # But maybe there are magic words like {{#time|}}
            # TODO: recognize magic word and templates inside links
            # see http://la.wikipedia.org/w/index.php?title=997_Priska&diff=prev&oldid=1038880
            if sectionStart > 0:
                # Categories does not have sections.
                if self._namespace == 14:
                    raise InvalidTitle(u"Invalid section in category '%s'" % t)
                else:
                    self._section = t[sectionStart+1 : ].lstrip(" ")
                    self._section = sectionencode(self._section,
                                                  self._site.encoding())
                    if not self._section:
                        self._section = None
                    t = t[ : sectionStart].rstrip(" ")
            elif sectionStart == 0:
                raise InvalidTitle(u"Invalid title starting with a #: '%s'" % t)
            else:
                self._section = None

            if t:
                if not self._site.nocapitalize:
                    t = t[:1].upper() + t[1:]

            # reassemble the title from its parts
            if self._namespace != 0:
                t = self._site.namespace(self._namespace) + u':' + t
            if self._section:
                t += u'#' + self._section

            self._title = t
            self.editRestriction = None
            self.moveRestriction = None
            self._permalink = None
            self._userName = None
            self._ipedit = None
            self._editTime = '0'
            self._startTime = '0'
            # For the Flagged Revisions MediaWiki extension
            self._revisionId = None
            self._deletedRevs = None
        except NoSuchSite:
            raise
        except:
            if verbose:
                output(u"Exception in Page constructor")
                output(
                    u"site=%s, title=%s, insite=%s, defaultNamespace=%i"
                    % (site, title, insite, defaultNamespace)
                )
            raise

    def site(self):
        """Return the Site object for the wiki on which this Page resides."""
        return self._site

    def encoding(self):
        """Return the character encoding used on this Page's wiki Site."""
        return self._site.encoding()

    def title(self, underscore=False, savetitle=False, decode=False,
              withNamespace=True,
              withSection=True, asUrl=False, asLink=False,
              allowInterwiki=True, forceInterwiki=False, textlink=False,
              as_filename=False):
        """Return the title of this Page, as a Unicode string.

        @param underscore: if true, replace all ' ' characters with '_'
        @param withNamespace: - not implemented yet -
        @param withSection: - not implemented yet -
        @param asUrl: - not implemented yet -
        @param asLink: if true, return the title in the form of a wikilink
        @param allowInterwiki: (only used if asLink is true) if true, format
            the link as an interwiki link if necessary
        @param forceInterwiki: (only used if asLink is true) if true, always
            format the link as an interwiki link
        @param textlink: (only used if asLink is true) if true, place a ':'
            before Category: and Image: links
        @param as_filename:  - not implemented yet -

        If underscore is True, replace all ' ' characters with '_'.
        If savetitle is True, encode any wiki syntax in the title.
        If decode is True, decodes the section title
        """
        title = self._title
        if decode or asLink:
            begin = title.find('#')
            if begin != -1:
                anchor = self.section(underscore=underscore, decode=True)
                try:
                    title = title[:begin + 1] + anchor
                except TypeError:
                    print title, begin, anchor
                    raise

        if asLink:
            iw_target_site = getSite()
            iw_target_family = getSite().family
            if iw_target_family.interwiki_forward:
                iw_target_family = pywikibot.Family(iw_target_family.interwiki_forward)

            if allowInterwiki and (forceInterwiki or self._site != iw_target_site):
                colon = ""
                if textlink:
                    colon = ":"
                if  self._site.family != iw_target_family \
                        and self._site.family.name != self._site.lang:
                    title =  u'[[%s%s:%s:%s]]' % (colon, self._site.family.name,
                                                self._site.lang, title)
                else:
                    title = u'[[%s%s:%s]]' % (colon, self._site.lang, title)
            elif textlink and (self.isImage() or self.isCategory()):
                    title = u'[[:%s]]' % title
            else:
                title = u'[[%s]]' % title
        if savetitle or asLink:
            # Ensure there's no wiki syntax in the title
            title = title.replace(u"''", u'%27%27')
        if underscore:
            title = title.replace(' ', '_')
        return title

    def titleWithoutNamespace(self, underscore=False):
        """Return title of Page without namespace and without section."""
        if self.namespace() == 0:
            return self.sectionFreeTitle(underscore=underscore)
        else:
            return self.sectionFreeTitle(underscore=underscore).split(':', 1)[1]

    def titleForFilename(self):
        """
        Return the title of the page in a form suitable for a filename on
        the user's file system.
        """
        result = self.title()
        # Replace characters that are not possible in file names on some
        # systems.
        # Spaces are possible on most systems, but are bad for URLs.
        for forbiddenChar in ':*?/\\ ':
            result = result.replace(forbiddenChar, '_')
        return result

    def section(self, underscore = False, decode=False):
        """Return the name of the section this Page refers to.

        The section is the part of the title following a '#' character, if any.
        If no section is present, return None.
        """
        section = self._section
        if section and decode:
            section = resectiondecodeleadingnonalpha.sub('',section)
            section = resectiondecodeescapes.sub('%',section)
            section = url2unicode(section, self._site)
            if not underscore:
                section = section.replace('_', ' ')
        return section

    def sectionFreeTitle(self, underscore=False):
        """Return the title of this Page, without the section (if any)."""
        sectionName = self.section(underscore=underscore)
        title = self.title(underscore=underscore)
        if sectionName:
            return title[:-len(sectionName)-1]
        else:
            return title

    def urlname(self, withNamespace=True):
        """Return the Page title encoded for use in an URL."""
        if withNamespace:
            title = self.title(underscore=True)
        else:
            title = self.titleWithoutNamespace(underscore=True)
        encodedTitle = title.encode(self.site().encoding())
        return urllib.quote(encodedTitle)

    def __str__(self):
        """Return a console representation of the pagelink."""
        return self.aslink().encode(config.console_encoding, 'replace')

    def __repr__(self):
        """Return a more complete string representation."""
        return "%s{%s}" % (self.__class__.__name__, str(self))

    #@deprecated("Page.title(asLink=True)")
    def aslink(self, forceInterwiki=False, textlink=False, noInterwiki=False):
        """Return a string representation in the form of a wikilink.

        If forceInterwiki is True, return an interwiki link even if it
        points to the home wiki. If False, return an interwiki link only if
        needed.

        If textlink is True, always return a link in text form (that is,
        interwiki links and internal links to the Category: and Image:
        namespaces will be preceded by a : character).

        DEPRECATED to merge to rewrite branch:
        use self.title(asLink=True) instead.
        """
        return self.title(asLink=True, forceInterwiki=forceInterwiki,
                          allowInterwiki=not noInterwiki, textlink=textlink)

    def autoFormat(self):
        """Return (dictName, value) if title is in date.autoFormat dictionary.

        Value can be a year, date, etc., and dictName is 'YearBC',
        'Year_December', or another dictionary name. Please note that two
        entries may have exactly the same autoFormat, but be in two
        different namespaces, as some sites have categories with the
        same names. Regular titles return (None, None).

        """
        if not hasattr(self, '_autoFormat'):
            import date
            _autoFormat = date.getAutoFormat(self.site().language(),
                                             self.titleWithoutNamespace())
        return _autoFormat

    def isAutoTitle(self):
        """Return True if title of this Page is in the autoFormat dictionary."""
        return self.autoFormat()[0] is not None

    def get(self, force=False, get_redirect=False, throttle=True,
            sysop=False, change_edit_time=True, expandtemplates=False):
        """Return the wiki-text of the page.

        This will retrieve the page from the server if it has not been
        retrieved yet, or if force is True. This can raise the following
        exceptions that should be caught by the calling code:

        @exception NoPage         The page does not exist
        @exception IsRedirectPage The page is a redirect. The argument of the
                                  exception is the title of the page it
                                  redirects to.
        @exception SectionError   The section does not exist on a page with
                                  a # link

        @param force            reload all page attributes, including errors.
        @param get_redirect     return the redirect text, do not follow the
                                redirect, do not raise an exception.
        @param sysop            if the user has a sysop account, use it to
                                retrieve this page
        @param change_edit_time if False, do not check this version for
                                changes before saving. This should be used only
                                if the page has been loaded previously.
        @param expandtemplates  all templates in the page content are fully
                                resolved too (if API is used).

        """
        # NOTE: The following few NoPage exceptions could already be thrown at
        # the Page() constructor. They are raised here instead for convenience,
        # because all scripts are prepared for NoPage exceptions raised by
        # get(), but not for such raised by the constructor.
        # \ufffd represents a badly encoded character, the other characters are
        # disallowed by MediaWiki.
        for illegalChar in u'#<>[]|{}\n\ufffd':
            if illegalChar in self.sectionFreeTitle():
                if verbose:
                    output(u'Illegal character in %s!' % self.aslink())
                raise NoPage('Illegal character in %s!' % self.aslink())
        if self.namespace() == -1:
            raise NoPage('%s is in the Special namespace!' % self.aslink())
        if self.site().isInterwikiLink(self.title()):
            raise NoPage('%s is not a local page on %s!'
                         % (self.aslink(), self.site()))
        if force:
            # When forcing, we retry the page no matter what:
            # * Old exceptions and contents do not apply any more
            # * Deleting _contents and _expandcontents to force reload
            for attr in ['_redirarg', '_getexception',
                         '_contents', '_expandcontents']:
                if hasattr(self, attr):
                    delattr(self, attr)
        else:
            # Make sure we re-raise an exception we got on an earlier attempt
            if hasattr(self, '_redirarg') and not get_redirect:
                raise IsRedirectPage, self._redirarg
            elif hasattr(self, '_getexception'):
                if self._getexception == IsRedirectPage and get_redirect:
                    pass
                else:
                    raise self._getexception
        # Make sure we did try to get the contents once
        if expandtemplates:
            attr = '_expandcontents'
        else:
            attr = '_contents'
        if not hasattr(self, attr):
            try:
                contents = self._getEditPage(get_redirect=get_redirect, throttle=throttle, sysop=sysop,
                                             expandtemplates = expandtemplates)
                if expandtemplates:
                    self._expandcontents = contents
                else:
                    self._contents = contents
                hn = self.section()
                if hn:
                    m = re.search("=+ *%s *=+" % hn, self._contents)
                    if verbose and not m:
                        output(u"WARNING: Section does not exist: %s" % self.aslink(forceInterwiki = True))
            # Store any exceptions for later reference
            except NoPage:
                self._getexception = NoPage
                raise
            except IsRedirectPage, arg:
                self._getexception = IsRedirectPage
                self._redirarg = arg
                if not get_redirect:
                    raise
            except SectionError:
                self._getexception = SectionError
                raise
            except UserBlocked:
                if self.site().loggedInAs(sysop=sysop):
                    raise UserBlocked(self.site(), self.aslink(forceInterwiki = True))
                else:
                    if verbose:
                        output("The IP address is blocked, retry by login.")
                    self.site().forceLogin(sysop=sysop)
                    return self.get(force, get_redirect, throttle, sysop, change_edit_time)
        if expandtemplates:
            return self._expandcontents
        return self._contents

    def _getEditPage(self, get_redirect=False, throttle=True, sysop=False,
                     oldid=None, change_edit_time=True, expandtemplates=False):
        """Get the contents of the Page via API query

        Do not use this directly, use get() instead.

        Arguments:
            oldid - Retrieve an old revision (by id), not the current one
            get_redirect  - Get the contents, even if it is a redirect page
        expandtemplates - Fully resolve templates within page content
                          (if API is used)

        This method returns the raw wiki text as a unicode string.
        """
        if not self.site().has_api() or self.site().versionnumber() < 12:
            return self._getEditPageOld(get_redirect, throttle, sysop, oldid, change_edit_time)

        params = {
            'action': 'query',
            'titles': self.title(),
            'prop': ['revisions', 'info'],
            'rvprop': ['content', 'ids', 'flags', 'timestamp', 'user', 'comment', 'size'],
            'rvlimit': 1,
            #'talkid' valid for release > 1.12
            #'url', 'readable' valid for release > 1.14
            'inprop': ['protection', 'subjectid'],
            #'intoken': 'edit',
        }
        if oldid:
            params['rvstartid'] = oldid
        if expandtemplates:
            params[u'rvexpandtemplates'] = u''

        if throttle:
            get_throttle()
        textareaFound = False
        # retrying loop is done by query.GetData
        data = query.GetData(params, self.site(), sysop=sysop)
        if 'error' in data:
            raise RuntimeError("API query error: %s" % data)
        if not 'pages' in data['query']:
            raise RuntimeError("API query error, no pages found: %s" % data)
        pageInfo = data['query']['pages'].values()[0]
        if data['query']['pages'].keys()[0] == "-1":
            if 'missing' in pageInfo:
                raise NoPage(self.site(), self.aslink(forceInterwiki = True),"Page does not exist. In rare cases, if you are certain the page does exist, look into overriding family.RversionTab" )
            elif 'invalid' in pageInfo:
                raise BadTitle('BadTitle: %s' % self)
        elif 'revisions' in pageInfo: #valid Title
            lastRev = pageInfo['revisions'][0]
            if lastRev['*'] is not None:
                textareaFound = True
        # I got page date with 'revisions' in pageInfo but
        # lastRev['*'] = False instead of the content. The Page itself was
        # deleted but there was not 'missing' in pageInfo as expected
        # I raise a ServerError() yet, but maybe it should be NoPage().
        if not textareaFound:
            raise ServerError('ServerError: No textarea found in %s' % self)

        self.editRestriction = ''
        self.moveRestriction = ''

        # Note: user may be hidden and mw returns 'userhidden' flag
        if 'userhidden' in lastRev:
            self._userName = None
        else:
            self._userName = lastRev['user']
            self._ipedit = 'anon' in lastRev
        for restr in pageInfo['protection']:
            if restr['type'] == 'edit':
                self.editRestriction = restr['level']
            elif restr['type'] == 'move':
                self.moveRestriction = restr['level']

        self._revisionId = lastRev['revid']

        if change_edit_time:
            self._editTime = parsetime2stamp(lastRev['timestamp'])
            if "starttimestamp" in pageInfo:
                self._startTime = parsetime2stamp(pageInfo["starttimestamp"])

        self._isWatched = False #cannot handle in API in my research for now.

        pagetext = lastRev['*']
        pagetext = pagetext.rstrip()
        # pagetext must not decodeEsperantoX() if loaded via API
        m = self.site().redirectRegex().match(pagetext)
        if m:
            # page text matches the redirect pattern
            if self.section() and not "#" in m.group(1):
                redirtarget = "%s#%s" % (m.group(1), self.section())
            else:
                redirtarget = m.group(1)
            if get_redirect:
                self._redirarg = redirtarget
            else:
                raise IsRedirectPage(redirtarget)
        if self.section():
            # TODO: What the hell is this? Docu please.
            m = re.search("\.3D\_*(\.27\.27+)?(\.5B\.5B)?\_*%s\_*(\.5B\.5B)?(\.27\.27+)?\_*\.3D" % re.escape(self.section()), sectionencode(pageInfo['revisions'][0]['*'],self.site().encoding()))
            if not m:
                try:
                    self._getexception
                except AttributeError:
                    raise SectionError # Page has no section by this name
        return pagetext

    def _getEditPageOld(self, get_redirect=False, throttle=True, sysop=False,
                     oldid=None, change_edit_time=True):
        """Get the contents of the Page via the edit page."""

        if verbose:
            output(u'Getting page %s' % self.aslink())
        path = self.site().edit_address(self.urlname())
        if oldid:
            path += "&oldid="+oldid
        # Make sure Brion doesn't get angry by waiting if the last time a page
        # was retrieved was not long enough ago.
        if throttle:
            get_throttle()
        textareaFound = False
        retry_idle_time = 1
        while not textareaFound:
            text = self.site().getUrl(path, sysop = sysop)

            if "<title>Wiki does not exist</title>" in text:
                raise NoSuchSite(u'Wiki %s does not exist yet' % self.site())

            # Extract the actual text from the textarea
            m1 = re.search('<textarea([^>]*)>', text)
            m2 = re.search('</textarea>', text)
            if m1 and m2:
                i1 = m1.end()
                i2 = m2.start()
                textareaFound = True
            else:
                # search for messages with no "view source" (aren't used in new versions)
                if self.site().mediawiki_message('whitelistedittitle') in text:
                    raise NoPage(u'Page editing is forbidden for anonymous users.')
                elif self.site().has_mediawiki_message('nocreatetitle') and self.site().mediawiki_message('nocreatetitle') in text:
                    raise NoPage(self.site(), self.aslink(forceInterwiki = True))
                # Bad title
                elif 'var wgPageName = "Special:Badtitle";' in text \
                or self.site().mediawiki_message('badtitle') in text:
                    raise BadTitle('BadTitle: %s' % self)
                # find out if the username or IP has been blocked
                elif self.site().isBlocked():
                    raise UserBlocked(self.site(), self.aslink(forceInterwiki = True))
                # If there is no text area and the heading is 'View Source'
                # but user is not blocked, the page does not exist, and is
                # locked
                elif self.site().mediawiki_message('viewsource') in text:
                    raise NoPage(self.site(), self.aslink(forceInterwiki = True))
                # Some of the newest versions don't have a "view source" tag for
                # non-existant pages
                # Check also the div class because if the language is not english
                # the bot can not seeing that the page is blocked.
                elif self.site().mediawiki_message('badaccess') in text or \
                "<div class=\"permissions-errors\">" in text:
                    raise NoPage(self.site(), self.aslink(forceInterwiki = True))
                elif config.retry_on_fail:
                    if "<title>Wikimedia Error</title>" in text:
                        output( u"Wikimedia has technical problems; will retry in %i minutes." % retry_idle_time)
                    else:
                        output( unicode(text) )
                        # We assume that the server is down. Wait some time, then try again.
                        output( u"WARNING: No text area found on %s%s. Maybe the server is down. Retrying in %i minutes..." % (self.site().hostname(), path, retry_idle_time) )
                    time.sleep(retry_idle_time * 60)
                    # Next time wait longer, but not longer than half an hour
                    retry_idle_time *= 2
                    if retry_idle_time > 30:
                        retry_idle_time = 30
                else:
                    output( u"Failed to access wiki")
                    sys.exit(1)
        # Check for restrictions
        m = re.search('var wgRestrictionEdit = \\["(\w+)"\\]', text)
        if m:
            if verbose:
                output(u"DBG> page is locked for group %s" % m.group(1))
            self.editRestriction = m.group(1);
        else:
            self.editRestriction = ''
        m = re.search('var wgRestrictionMove = \\["(\w+)"\\]', text)
        if m:
            self.moveRestriction = m.group(1);
        else:
            self.moveRestriction = ''
        m = re.search('name=["\']baseRevId["\'] type=["\']hidden["\'] value="(\d+)"', text)
        if m:
            self._revisionId = m.group(1)
        if change_edit_time:
            # Get timestamps
            m = re.search('value="(\d+)" name=["\']wpEdittime["\']', text)
            if m:
                self._editTime = m.group(1)
            else:
                self._editTime = "0"
            m = re.search('value="(\d+)" name=["\']wpStarttime["\']', text)
            if m:
                self._startTime = m.group(1)
            else:
                self._startTime = "0"
        # Find out if page actually exists. Only existing pages have a
        # version history tab.
        if self.site().family.RversionTab(self.site().language()):
            # In case a family does not have version history tabs, or in
            # another form
            RversionTab = re.compile(self.site().family.RversionTab(self.site().language()))
        else:
            RversionTab = re.compile(r'<li id="ca-history"><a href=".*?title=.*?&amp;action=history".*?>.*?</a></li>', re.DOTALL)
        matchVersionTab = RversionTab.search(text)
        if not matchVersionTab and not self.site().family.name == 'wikitravel':
            raise NoPage(self.site(), self.aslink(forceInterwiki = True),"Page does not exist. In rare cases, if you are certain the page does exist, look into overriding family.RversionTab" )
        # Look if the page is on our watchlist
        matchWatching = Rwatchlist.search(text)
        if matchWatching:
            self._isWatched = True
        else:
            self._isWatched = False
        # Now process the contents of the textarea
        # Unescape HTML characters, strip whitespace
        pagetext = text[i1:i2]
        pagetext = unescape(pagetext)
        pagetext = pagetext.rstrip()
        if self.site().lang == 'eo':
            pagetext = decodeEsperantoX(pagetext)
        m = self.site().redirectRegex().match(pagetext)
        if m:
            # page text matches the redirect pattern
            if self.section() and not "#" in m.group(1):
                redirtarget = "%s#%s" % (m.group(1), self.section())
            else:
                redirtarget = m.group(1)
            if get_redirect:
                self._redirarg = redirtarget
            else:
                raise IsRedirectPage(redirtarget)
        if self.section():
            # TODO: What the hell is this? Docu please.
            m = re.search("\.3D\_*(\.27\.27+)?(\.5B\.5B)?\_*%s\_*(\.5B\.5B)?(\.27\.27+)?\_*\.3D" % re.escape(self.section()), sectionencode(text,self.site().encoding()))
            if not m:
                try:
                    self._getexception
                except AttributeError:
                    raise SectionError # Page has no section by this name

        return pagetext

    def getOldVersion(self, oldid, force=False, get_redirect=False,
                      throttle=True, sysop=False, change_edit_time=True):
        """Return text of an old revision of this page; same options as get().

        @param oldid: The revid of the revision desired.

        """
        # TODO: should probably check for bad pagename, NoPage, and other
        # exceptions that would prevent retrieving text, as get() does

        # TODO: should this default to change_edit_time = False? If we're not
        # getting the current version, why change the timestamps?
        return self._getEditPage(
                        get_redirect=get_redirect, throttle=throttle,
                        sysop=sysop, oldid=oldid,
                        change_edit_time=change_edit_time
                    )

    def permalink(self):
        """Return the permalink URL for current revision of this page."""
        return "%s://%s%s&oldid=%i" % (self.site().protocol(),
                                       self.site().hostname(),
                                       self.site().get_address(self.title()),
                                       self.latestRevision())

    def latestRevision(self):
        """Return the current revision id for this page."""
        if not self._permalink:
            # When we get the page with getall, the permalink is received
            # automatically
            getall(self.site(),[self],force=True)
            # Check for exceptions
            if hasattr(self, '_getexception'):
                raise self._getexception
        return int(self._permalink)
    def userName(self):
        """Return name or IP address of last user to edit page.

        Returns None unless page was retrieved with getAll().

        """
        return self._userName

    def isIpEdit(self):
        """Return True if last editor was unregistered.

        Returns None unless page was retrieved with getAll() or _getEditPage().

        """
        return self._ipedit

    def editTime(self):
        """Return timestamp (in MediaWiki format) of last revision to page.

        Returns None if last edit time is unknown.

        """
        return self._editTime


    def previousRevision(self):
        """Return the revision id for the previous revision of this Page."""
        vh = self.getVersionHistory(revCount=2)
        return vh[1][0]

    def exists(self):
        """Return True if page exists on the wiki, even if it's a redirect.

        If the title includes a section, return False if this section isn't
        found.

        """
        try:
            self.get()
        except NoPage:
            return False
        except IsRedirectPage:
            return True
        except SectionError:
            return False
        return True

    def pageAPInfo(self):
        """Return the last revid if page exists on the wiki,
           Raise IsRedirectPage if it's a redirect
           Raise NoPage if the page doesn't exist

        Using the API should be a lot faster.
        Function done in order to improve the scripts performance.

        """
        params = {
            'action'    :'query',
            'prop'      :'info',
            'titles'    :self.title(),
            }
        data = query.GetData(params, self.site(), encodeTitle = False)['query']['pages'].values()[0]
        if 'redirect' in data:
            raise IsRedirectPage
        elif 'missing' in data:
            raise NoPage
        elif 'lastrevid' in data:
            return data['lastrevid'] # if ok, return the last revid
        else:
            # should not exists, OR we have problems.
            # better double check in this situations
            x = self.get()
            return True # if we reach this point, we had no problems.

    def getTemplates(self, tllimit = 5000):
        #action=query&prop=templates&titles=Main Page
        """
        Returns the templates that are used in the page given by API.

        If no templates found, returns None.

        """
        params = {
            'action': 'query',
            'prop': 'templates',
            'titles': self.title(),
            'tllimit': tllimit,
        }
        if tllimit > config.special_page_limit:
            params['tllimit'] = config.special_page_limit
            if tllimit > 5000 and self.site.isAllowed('apihighlimits'):
                params['tllimit'] = 5000

        tmpsFound = []
        count = 0
        while True:
            data = query.GetData(params, self.site(), encodeTitle = False)['query']['pages'].values()[0]
            if "templates" not in data:
                return []

            for tmp in data['templates']:
                count += 1
                tmpsFound.append(Page(self.site(), tmp['title'], defaultNamespace=tmp['ns']) )
                if count >= tllimit:
                    break

            if 'query-continue' in data and count < tllimit:
                params["tlcontinue"] = data["query-continue"]["templates"]["tlcontinue"]
            else:
                break

        return tmpsFound

    def isRedirectPage(self):
        """Return True if this is a redirect, False if not or not existing."""
        try:
            self.get()
        except NoPage:
            return False
        except IsRedirectPage:
            return True
        except SectionError:
            return False
        return False

    def isStaticRedirect(self, force=False):
        """Return True if this is a redirect containing the magic word
        __STATICREDIRECT__, False if not or not existing.

        """
        found = False
        if self.isRedirectPage() and self.site().versionnumber() > 13:
            staticKeys = self.site().getmagicwords('staticredirect')
            text = self.get(get_redirect=True, force=force)
            if staticKeys:
                for key in staticKeys:
                    if key in text:
                        found = True
                        break
        return found

    def isCategoryRedirect(self, text=None):
        """Return True if this is a category redirect page, False otherwise."""

        if not self.isCategory():
            return False
        if not hasattr(self, "_catredirect"):
            if not text:
                try:
                    text = self.get(get_redirect=True)
                except NoPage:
                    return False
            catredirs = self.site().category_redirects()
            for (t, args) in self.templatesWithParams(thistxt=text):
                template = Page(self.site(), t, defaultNamespace=10
                                ).titleWithoutNamespace() # normalize title
                if template in catredirs:
                    # Get target (first template argument)
                    self._catredirect = self.site().namespace(14) + ":" + args[0]
                    break
            else:
                self._catredirect = False
        return bool(self._catredirect)

    def getCategoryRedirectTarget(self):
        """If this is a category redirect, return the target category title."""
        if self.isCategoryRedirect():
            import catlib
            return catlib.Category(self.site(), self._catredirect)
        raise IsNotRedirectPage

    def isEmpty(self):
        """Return True if the page text has less than 4 characters.

        Character count ignores language links and category links.
        Can raise the same exceptions as get().

        """
        txt = self.get()
        txt = removeLanguageLinks(txt, site = self.site())
        txt = removeCategoryLinks(txt, site = self.site())
        if len(txt) < 4:
            return True
        else:
            return False

    def isTalkPage(self):
        """Return True if this page is in any talk namespace."""
        ns = self.namespace()
        return ns >= 0 and ns % 2 == 1

    def toggleTalkPage(self):
        """Return other member of the article-talk page pair for this Page.

        If self is a talk page, returns the associated content page;
        otherwise, returns the associated talk page.
        Returns None if self is a special page.

        """
        ns = self.namespace()
        if ns < 0: # Special page
            return None
        if self.isTalkPage():
            ns -= 1
        else:
            ns += 1

        if ns == 6:
            return ImagePage(self.site(), self.titleWithoutNamespace())

        return Page(self.site(), self.titleWithoutNamespace(), defaultNamespace=ns)

    def isCategory(self):
        """Return True if the page is a Category, False otherwise."""
        return self.namespace() == 14

    def isImage(self):
        """Return True if this is an image description page, False otherwise."""
        return self.namespace() == 6

    def isDisambig(self, get_Index=True):
        """Return True if this is a disambiguation page, False otherwise.

        Relies on the presence of specific templates, identified in
        the Family file or on a wiki page, to identify disambiguation
        pages.

        By default, loads a list of template names from the Family file;
        if the value in the Family file is None no entry was made, looks for
        the list on [[MediaWiki:Disambiguationspage]]. If this page does not
        exist, take the mediawiki message.

        If get_Index is True then also load the templates for index articles
        which are given on en-wiki

        Template:Disambig is always assumed to be default, and will be
        appended regardless of its existence.

        """
        if not hasattr(self, "_isDisambig"):
            if not hasattr(self._site, "_disambigtemplates"):
                try:
                    default = set(self._site.family.disambig('_default'))
                except KeyError:
                    default = set([u'Disambig'])
                try:
                    distl = self._site.family.disambig(self._site.lang,
                                                       fallback=False)
                except KeyError:
                    distl = None
                if distl is None:
                    try:
                        disambigpages = Page(self._site,
                                             "MediaWiki:Disambiguationspage")
                        disambigs = set(link.titleWithoutNamespace()
                                        for link in disambigpages.linkedPages()
                                        if link.namespace() == 10)
                        # add index article templates
                        if get_Index and \
                           self._site.sitename() == 'wikipedia:en':
                            regex = re.compile('\(\((.+?)\)\)')
                            content = disambigpages.get()
                            for index in regex.findall(content):
                                disambigs.add(index[:1].upper() + index[1:])
                    except NoPage:
                        disambigs = set([self._site.mediawiki_message(
                            'Disambiguationspage').split(':', 1)[1]])
                    # add the default template(s)
                    self._site._disambigtemplates = disambigs | default
                else:
                    # Normalize template capitalization
                    self._site._disambigtemplates = set(
                        t[:1].upper() + t[1:] for t in distl
                    )
            disambigInPage = self._site._disambigtemplates.intersection(
                self.templates())
            self._isDisambig = self.namespace() != 10 and \
                               len(disambigInPage) > 0
        return self._isDisambig

    def canBeEdited(self):
        """Return bool indicating whether this page can be edited.

        This returns True if and only if:
          - page is unprotected, and bot has an account for this site, or
          - page is protected, and bot has a sysop account for this site.

        """
        try:
            self.get()
        except:
            pass
        if self.editRestriction == 'sysop':
            userdict = config.sysopnames
        else:
            userdict = config.usernames
        try:
            userdict[self.site().family.name][self.site().lang]
            return True
        except:
            # We don't have a user account for that wiki, or the
            # page is locked and we don't have a sysop account.
            return False

    def botMayEdit(self, username):
        """Return True if this page allows bots to edit it.

        This will be True if the page doesn't contain {{bots}} or
        {{nobots}}, or it contains them and the active bot is allowed to
        edit this page. (This method is only useful on those sites that
        recognize the bot-exclusion protocol; on other sites, it will always
        return True.)

        The framework enforces this restriction by default. It is possible
        to override this by setting ignore_bot_templates=True in
        user_config.py, or using page.put(force=True).

        """

        if self.site().family.name == 'wikitravel':        # Wikitravel's bot control.
            self.site().family.bot_control(self.site())

        if config.ignore_bot_templates: #Check the "master ignore switch"
            return True

        try:
            templates = self.templatesWithParams(get_redirect=True);
        except (NoPage, IsRedirectPage, SectionError):
            return True

        for template in templates:
            if template[0].lower() == 'nobots':
                return False
            elif template[0].lower() == 'bots':
                if len(template[1]) == 0:
                    return True
                else:
                    (ttype, bots) = template[1][0].split('=', 1)
                    bots = bots.split(',')
                    if ttype == 'allow':
                        if 'all' in bots or username in bots:
                            return True
                        else:
                            return False
                    if ttype == 'deny':
                        if 'all' in bots or username in bots:
                            return False
                        else:
                            return True
        # no restricting template found
        return True

    def namespace(self):
        """Return the number of the namespace of the page.

        Only recognizes those namespaces defined in family.py.
        If not defined, it will return 0 (the main namespace).

        """
        return self._namespace

    def getReferences(self, follow_redirects=True, withTemplateInclusion=True,
            onlyTemplateInclusion=False, redirectsOnly=False, internal = False):
        """Yield all pages that link to the page by API

        If you need a full list of referring pages, use this:
            pages = [page for page in s.getReferences()]
        Parameters:
        * follow_redirects      - if True, also returns pages that link to a
                                  redirect pointing to the page.
        * withTemplateInclusion - if True, also returns pages where self is
                                  used as a template.
        * onlyTemplateInclusion - if True, only returns pages where self is
                                  used as a template.
        * redirectsOnly         - if True, only returns redirects to self.

        """
        if not self.site().has_api():
            for s in self.getReferencesOld(follow_redirects, withTemplateInclusion, onlyTemplateInclusion, redirectsOnly):
                yield s
            return

        params = {
            'action': 'query',
            'list': [],
        }
        if not onlyTemplateInclusion:
            params['list'].append('backlinks')
            params['bltitle'] = self.title()
            params['bllimit'] = config.special_page_limit
            params['blfilterredir'] = 'all'
            if follow_redirects:
                params['blredirect'] = 1
            if redirectsOnly:
                params['blfilterredir'] = 'redirects'
            if not self.site().isAllowed('apihighlimits') and config.special_page_limit > 500:
                params['bllimit'] = 500

        if withTemplateInclusion or onlyTemplateInclusion:
            params['list'].append('embeddedin')
            params['eititle'] = self.title()
            params['eilimit'] = config.special_page_limit
            params['eifilterredir'] = 'all'
            if follow_redirects:
                params['eiredirect'] = 1
            if redirectsOnly:
                params['eifilterredir'] = 'redirects'
            if not self.site().isAllowed('apihighlimits') and config.special_page_limit > 500:
                params['eilimit'] = 500

        allDone = False

        while not allDone:
            if not internal:
                output(u'Getting references to %s via API...' % self.aslink())

            datas = query.GetData(params, self.site())
            data = datas['query'].values()
            if len(data) == 2:
                data = data[0] + data[1]
            else:
                data = data[0]

            refPages = set()
            for blp in data:
                pg = Page(self.site(), blp['title'], defaultNamespace = blp['ns'])
                if pg in refPages:
                    continue

                yield pg
                refPages.add(pg)
                if follow_redirects and 'redirect' in blp and 'redirlinks' in blp:
                    for p in blp['redirlinks']:
                        plk = Page(self.site(), p['title'], defaultNamespace = p['ns'])
                        if plk in refPages:
                            continue

                        yield plk
                        refPages.add(plk)
                        if follow_redirects and 'redirect' in p and plk != self:
                            for zms in plk.getReferences(follow_redirects, withTemplateInclusion,
                                              onlyTemplateInclusion, redirectsOnly, internal=True):
                                yield zms
                        else:
                            continue
                else:
                    continue

            if 'query-continue' in datas:
                if 'backlinks' in datas['query-continue']:
                    params['blcontinue'] = datas['query-continue']['backlinks']['blcontinue']

                if 'embeddedin' in datas['query-continue']:
                    params['eicontinue'] = datas['query-continue']['embeddedin']['eicontinue']
            else:
                allDone = True


    def getReferencesOld(self,
            follow_redirects=True, withTemplateInclusion=True,
            onlyTemplateInclusion=False, redirectsOnly=False):
        """Yield all pages that link to the page.
        """
        # Temporary bug-fix while researching more robust solution:
        if config.special_page_limit > 999:
            config.special_page_limit = 999
        site = self.site()
        path = self.site().references_address(self.urlname())
        if withTemplateInclusion:
            path+=u'&hidetrans=0'
        if onlyTemplateInclusion:
            path+=u'&hidetrans=0&hidelinks=1&hideredirs=1&hideimages=1'
        if redirectsOnly:
            path+=u'&hideredirs=0&hidetrans=1&hidelinks=1&hideimages=1'
        content = SoupStrainer("div", id=self.site().family.content_id)
        try:
            next_msg = self.site().mediawiki_message('whatlinkshere-next')
        except KeyError:
            next_msg = "next %i" % config.special_page_limit
        plural = (config.special_page_limit == 1) and "\\1" or "\\2"
        next_msg = re.sub(r"{{PLURAL:\$1\|(.*?)\|(.*?)}}", plural, next_msg)
        nextpattern = re.compile("^%s$" % next_msg.replace("$1", "[0-9]+"))
        delay = 1
        if self.site().has_mediawiki_message("Isredirect"):
            self._isredirectmessage = self.site().mediawiki_message("Isredirect")
        if self.site().has_mediawiki_message("Istemplate"):
            self._istemplatemessage = self.site().mediawiki_message("Istemplate")
        # to avoid duplicates:
        refPages = set()
        while path:
            output(u'Getting references to %s' % self.aslink())
            get_throttle()
            txt = self.site().getUrl(path)
            body = BeautifulSoup(txt,
                                 convertEntities=BeautifulSoup.HTML_ENTITIES,
                                 parseOnlyThese=content)
            next_text = body.find(text=nextpattern)
            if next_text is not None and next_text.parent.has_key('href'):
                path = next_text.parent['href'].replace("&amp;", "&")
            else:
                path = ""
            reflist = body.find("ul")
            if reflist is None:
                return
            for page in self._parse_reflist(reflist,
                                follow_redirects, withTemplateInclusion,
                                onlyTemplateInclusion, redirectsOnly):
                if page not in refPages:
                    yield page
                    refPages.add(page)

    def _parse_reflist(self, reflist,
            follow_redirects=True, withTemplateInclusion=True,
            onlyTemplateInclusion=False, redirectsOnly=False):
        """For internal use only

        Parse a "Special:Whatlinkshere" list of references and yield Page
        objects that meet the criteria (used by getReferences)
        """
        for link in reflist("li", recursive=False):
            title = link.a.string
            if title is None:
                output(u"DBG> invalid <li> item in Whatlinkshere: %s" % link)
            try:
                p = Page(self.site(), title)
            except InvalidTitle:
                output(u"DBG> Whatlinkshere:%s contains invalid link to %s"
                        % (self.title(), title))
                continue
            isredirect, istemplate = False, False
            textafter = link.a.findNextSibling(text=True)
            if textafter is not None:
                if self.site().has_mediawiki_message("Isredirect") \
                        and self._isredirectmessage in textafter:
                    # make sure this is really a redirect to this page
                    # (MediaWiki will mark as a redirect any link that follows
                    # a #REDIRECT marker, not just the first one).
                    if p.getRedirectTarget().sectionFreeTitle() == self.sectionFreeTitle():
                        isredirect = True
                if self.site().has_mediawiki_message("Istemplate") \
                        and self._istemplatemessage in textafter:
                    istemplate = True
            if (withTemplateInclusion or onlyTemplateInclusion or not istemplate
                    ) and (not redirectsOnly or isredirect
                    ) and (not onlyTemplateInclusion or istemplate
                    ):
                yield p
                continue

            if isredirect and follow_redirects:
                sublist = link.find("ul")
                if sublist is not None:
                    for p in self._parse_reflist(sublist,
                                follow_redirects, withTemplateInclusion,
                                onlyTemplateInclusion, redirectsOnly):
                        yield p

    def _getActionUser(self, action, restriction = '', sysop = False):
        """
        Get the user to do an action: sysop or not sysop, or raise an exception
        if the user cannot do that.

        Parameters:
        * action - the action to be done, which is the name of the right
        * restriction - the restriction level or an empty string for no restriction
        * sysop - initially use sysop user?
        """
        # Login
        self.site().forceLogin(sysop = sysop)

        # Check permissions
        if not self.site().isAllowed(action, sysop):
            if sysop:
                raise LockedPage(u'The sysop user is not allowed to %s in site %s' % (action, self.site()))
            else:
                try:
                    user = self._getActionUser(action, restriction, sysop = True)
                    output(u'The user is not allowed to %s on site %s. Using sysop account.' % (action, self.site()))
                    return user
                except NoUsername:
                    raise LockedPage(u'The user is not allowed to %s on site %s, and no sysop account is defined.' % (action, self.site()))
                except LockedPage:
                    raise

        # Check restrictions
        if not self.site().isAllowed(restriction, sysop):
            if sysop:
                raise LockedPage(u'Page on %s is locked in a way that sysop user cannot %s it' % (self.site(), action))
            else:
                try:
                    user = self._getActionUser(action, restriction, sysop = True)
                    output(u'Page is locked on %s - cannot %s, using sysop account.' % (self.site(), action))
                    return user
                except NoUsername:
                    raise LockedPage(u'Page is locked on %s - cannot %s, and no sysop account is defined.' % (self.site(), action))
                except LockedPage:
                    raise

        return sysop

    def getRestrictions(self):
        """
        Get the protections on the page.
        * Returns a restrictions dictionary. Keys are 'edit' and 'move',
          Values are None (no restriction for that action) or [level, expiry] :
            * level is the level of auth needed to perform that action
                ('autoconfirmed' or 'sysop')
            * expiry is the expiration time of the restriction
        """
        #, titles = None
        #if titles:
        #    restrictions = {}
        #else:
        restrictions = { 'edit': None, 'move': None }
        try:
            api_url = self.site().api_address()
        except NotImplementedError:
            return restrictions

        predata = {
            'action': 'query',
            'prop': 'info',
            'inprop': 'protection',
            'titles': self.title(),
        }
        #if titles:
        #    predata['titles'] = titles

        text = query.GetData(predata, self.site())['query']['pages']

        for pageid in text:
            if 'missing' in text[pageid]:
                self._getexception = NoPage
                raise NoPage('Page %s does not exist' % self.aslink())
            elif not 'pageid' in text[pageid]:
                # Don't know what may happen here.
                # We may want to have better error handling
                raise Error("BUG> API problem.")
            if text[pageid]['protection'] != []:
                #if titles:
                #    restrictions = dict([ detail['type'], [ detail['level'], detail['expiry'] ] ]
                #        for detail in text[pageid]['protection'])
                #else:
                restrictions = dict([ detail['type'], [ detail['level'], detail['expiry'] ] ]
                    for detail in text[pageid]['protection'])

        return restrictions

    def put_async(self, newtext,
                  comment=None, watchArticle=None, minorEdit=True, force=False,
                  callback=None):
        """Put page on queue to be saved to wiki asynchronously.

        Asynchronous version of put (takes the same arguments), which places
        pages on a queue to be saved by a daemon thread. All arguments  are
        the same as for .put(), except --

        callback: a callable object that will be called after the page put
                  operation; this object must take two arguments:
                  (1) a Page object, and (2) an exception instance, which
                  will be None if the page was saved successfully.

        The callback is intended to be used by bots that need to keep track
        of which saves were successful.

        """
        try:
            page_put_queue.mutex.acquire()
            try:
                _putthread.start()
            except (AssertionError, RuntimeError):
                pass
        finally:
            page_put_queue.mutex.release()
        page_put_queue.put((self, newtext, comment, watchArticle, minorEdit,
                            force, callback))

    def put(self, newtext, comment=None, watchArticle=None, minorEdit=True,
            force=False, sysop=False, botflag=True, maxTries=-1):
        """Save the page with the contents of the first argument as the text.

        Optional parameters:
          comment:  a unicode string that is to be used as the summary for
                    the modification.
          watchArticle: a bool, add or remove this Page to/from bot user's
                        watchlist (if None, leave watchlist status unchanged)
          minorEdit: mark this edit as minor if True
          force: ignore botMayEdit() setting.
          maxTries: the maximum amount of save attempts. -1 for infinite.
        """
        # Login
        try:
            self.get()
        except:
            pass
        sysop = self._getActionUser(action = 'edit', restriction = self.editRestriction, sysop = sysop)
        username = self.site().loggedInAs()

        # Check blocks
        self.site().checkBlocks(sysop = sysop)

        # Determine if we are allowed to edit
        if not force:
            if not self.botMayEdit(username):
                raise LockedPage(
                    u'Not allowed to edit %s because of a restricting template'
                    % self.aslink())
            elif self.site().has_api() and self.namespace() in [2,3] \
                 and (self.title().endswith('.css') or \
                      self.title().endswith('.js')):
                titleparts = self.title().split("/")
                userpageowner = titleparts[0].split(":")[1]
                if userpageowner != username:
                    # API enable: if title ends with .css or .js in ns2,3
                    # it needs permission to edit user pages
                    if self.title().endswith('css'):
                        permission = 'editusercss'
                    else:
                        permission = 'edituserjs'
                    sysop = self._getActionUser(action=permission,
                                                restriction=self.editRestriction,
                                                sysop=True)

        # If there is an unchecked edit restriction, we need to load the page
        if self._editrestriction:
            output(
u'Page %s is semi-protected. Getting edit page to find out if we are allowed to edit.'
                   % self.aslink())
            oldtime = self.editTime()
            # Note: change_edit_time=True is always True since
            #       self.get() calls self._getEditPage without this parameter
            self.get(force=True, change_edit_time=True)
            newtime = self.editTime()
            if oldtime != newtime: # page was changed
                raise EditConflict(u'Page has been changed after first read.')
            self._editrestriction = False
        # If no comment is given for the change, use the default
        comment = comment or action
        if config.cosmetic_changes and not self.isTalkPage() and not calledModuleName() == 'cosmetic_changes':
            if config.cosmetic_changes_mylang_only:
                cc = (self.site().family.name == config.family and self.site().lang == config.mylang) or \
                     self.site().family.name in config.cosmetic_changes_enable.keys() and \
                     self.site().lang in config.cosmetic_changes_enable[self.site().family.name]
            else:
                cc = True
            cc = cc and not \
                 (self.site().family.name in config.cosmetic_changes_disable.keys() and \
                 self.site().lang in config.cosmetic_changes_disable[self.site().family.name])
            if cc:
                old = newtext
                if verbose:
                    output(u'Cosmetic Changes for %s-%s enabled.' % (self.site().family.name, self.site().lang))
                import cosmetic_changes
                ccToolkit = cosmetic_changes.CosmeticChangesToolkit(self.site(), redirect=self.isRedirectPage(), namespace = self.namespace(), pageTitle=self.title())
                newtext = ccToolkit.change(newtext)
                if comment and old.strip().replace('\r\n', '\n') != newtext.strip().replace('\r\n', '\n'):
                    comment += translate(self.site(), cosmetic_changes.msg_append)

        if watchArticle is None:
            # if the page was loaded via get(), we know its status
            if hasattr(self, '_isWatched'):
                watchArticle = self._isWatched
            else:
                import watchlist
                watchArticle = watchlist.isWatched(self.title(), site = self.site())
        newPage = not self.exists()
        # if posting to an Esperanto wiki, we must e.g. write Bordeauxx instead
        # of Bordeaux
        if self.site().lang == 'eo' and not self.site().has_api():
            newtext = encodeEsperantoX(newtext)
            comment = encodeEsperantoX(comment)

        return self._putPage(newtext, comment, watchArticle, minorEdit,
                             newPage, self.site().getToken(sysop = sysop), sysop = sysop, botflag=botflag, maxTries=maxTries)

    def _encodeArg(self, arg, msgForError):
        """Encode an ascii string/Unicode string to the site's encoding"""
        try:
            return arg.encode(self.site().encoding())
        except UnicodeDecodeError, e:
            # happens when arg is a non-ascii bytestring :
            # when reencoding bytestrings, python decodes first to ascii
            e.reason += ' (cannot convert input %s string to unicode)' % msgForError
            raise e
        except UnicodeEncodeError, e:
            # happens when arg is unicode
            e.reason += ' (cannot convert %s to wiki encoding %s)' % (msgForError, self.site().encoding())
            raise e

    def _putPage(self, text, comment=None, watchArticle=False, minorEdit=True,
                newPage=False, token=None, newToken=False, sysop=False,
                captcha=None, botflag=True, maxTries=-1):
        """Upload 'text' as new content of Page by API

        Don't use this directly, use put() instead.

        """
        if not self.site().has_api() or self.site().versionnumber() < 13:
            # api not enabled or version not supported
            return self._putPageOld(text, comment, watchArticle, minorEdit,
                newPage, token, newToken, sysop, captcha, botflag, maxTries)

        retry_attempt = 0
        retry_delay = 1
        dblagged = False
        params = {
            'action': 'edit',
            'title': self.title(),
            'text': self._encodeArg(text, 'text'),
            'summary': self._encodeArg(comment, 'summary'),
        }

        if token:
            params['token'] = token
        else:
            params['token'] = self.site().getToken(sysop = sysop)

        # Add server lag parameter (see config.py for details)
        if config.maxlag:
            params['maxlag'] = str(config.maxlag)

        if self._editTime:
            params['basetimestamp'] = self._editTime
        else:
            params['basetimestamp'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())

        if self._startTime:
            params['starttimestamp'] = self._startTime
        else:
            params['starttimestamp'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())

        if botflag:
            params['bot'] = 1

        if minorEdit:
            params['minor'] = 1
        else:
            params['notminor'] = 1

        if watchArticle:
            params['watch'] = 1
        #else:
        #    params['unwatch'] = 1

        if captcha:
            params['captchaid'] = captcha['id']
            params['captchaword'] = captcha['answer']

        while True:
            if (maxTries == 0):
                raise MaxTriesExceededError()
            maxTries -= 1
            # Check whether we are not too quickly after the previous
            # putPage, and wait a bit until the interval is acceptable
            if not dblagged:
                put_throttle()
            # Which web-site host are we submitting to?
            if newPage:
                output(u'Creating page %s via API' % self.aslink())
                params['createonly'] = 1
            else:
                output(u'Updating page %s via API' % self.aslink())
                params['nocreate'] = 1
            # Submit the prepared information
            try:
                response, data = query.GetData(params, self.site(), sysop=sysop, back_response = True)
                if isinstance(data,basestring):
                    raise KeyError
            except httplib.BadStatusLine, line:
                raise PageNotSaved('Bad status line: %s' % line.line)
            except ServerError:
                output(u''.join(traceback.format_exception(*sys.exc_info())))
                retry_attempt += 1
                if retry_attempt > config.maxretries:
                    raise
                output(u'Got a server error when putting %s; will retry in %i minute%s.' % (self.aslink(), retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            except ValueError: # API result cannot decode
                output(u"Server error encountered; will retry in %i minute%s."
                       % (retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            # If it has gotten this far then we should reset dblagged
            dblagged = False
            # Check blocks
            self.site().checkBlocks(sysop = sysop)
            # A second text area means that an edit conflict has occured.
            if response.code == 500:
                output(u"Server error encountered; will retry in %i minute%s."
                       % (retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            if 'error' in data:
                #All available error key in edit mode: (from ApiBase.php)
                # 'noimageredirect-anon':"Anonymous users can't create image redirects",
                # 'noimageredirect':"You don't have permission to create image redirects",
                # 'filtered':"The filter callback function refused your edit",
                # 'noedit-anon':"Anonymous users can't edit pages",
                # 'noedit':"You don't have permission to edit pages",
                # 'emptypage':"Creating new, empty pages is not allowed",
                # 'badmd5':"The supplied MD5 hash was incorrect",
                # 'notext':"One of the text, appendtext, prependtext and undo parameters must be set",
                # 'emptynewsection':'Creating empty new sections is not possible.',
                # 'revwrongpage':"r\$1 is not a revision of ``\$2''",
                # 'undofailure':'Undo failed due to conflicting intermediate edits',

                #for debug only
                #------------------------
                if verbose:
                    output("error occured, code:%s\ninfo:%s\nstatus:%s\nresponse:%s" % (
                        data['error']['code'], data['error']['info'], response.code, response.msg))
                    faked = params
                    if 'text' in faked:
                        del faked['text']
                    output("OriginalData:%s" % faked)
                    del faked
                #------------------------
                errorCode = data['error']['code']
                #cannot handle longpageerror and PageNoSave yet
                if errorCode == 'maxlag' or response.code == 503:
                    # server lag; wait for the lag time and retry
                    lagpattern = re.compile(r"Waiting for [\d.]+: (?P<lag>\d+) seconds? lagged")
                    lag = lagpattern.search(data['error']['info'])
                    timelag = int(lag.group("lag"))
                    output(u"Pausing %d seconds due to database server lag." % min(timelag,300))
                    dblagged = True
                    time.sleep(min(timelag,300))
                    continue
                elif errorCode == 'editconflict':
                    # 'editconflict':"Edit conflict detected",
                    raise EditConflict(u'An edit conflict has occured.')
                elif errorCode == 'spamdetected':
                    # 'spamdetected':"Your edit was refused because it contained a spam fragment: ``\$1''",
                    raise SpamfilterError(data['error']['info'][62:-2])
                elif errorCode == 'pagedeleted':
                    # 'pagedeleted':"The page has been deleted since you fetched its timestamp",
                    # Make sure your system clock is correct if this error occurs
                    # without any reason!
                    # raise EditConflict(u'Someone deleted the page.')
                    # No raise, simply define these variables and retry:
                    params['recreate'] = 1
                    if self._editTime:
                        params['basetimestamp'] = self._editTime
                    else:
                        params['basetimestamp'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())

                    if self._startTime:
                        params['starttimestamp'] = self._startTime
                    else:
                        params['starttimestamp'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())
                    continue
                elif errorCode == 'readonly':
                    # 'readonly':"The wiki is currently in read-only mode"
                    output(u"The database is currently locked for write access; will retry in %i minute%s."
                           % (retry_delay, retry_delay != 1 and "s" or ""))
                    time.sleep(60 * retry_delay)
                    retry_delay *= 2
                    if retry_delay > 30:
                        retry_delay = 30
                    continue
                elif errorCode == 'contenttoobig':
                    # 'contenttoobig':"The content you supplied exceeds the article size limit of \$1 kilobytes",
                    raise LongPageError(len(params['text']), int(data['error']['info'][59:-10]))
                elif errorCode in ['protectedpage', 'customcssjsprotected', 'cascadeprotected', 'protectednamespace', 'protectednamespace-interface']:
                    # 'protectedpage':"The ``\$1'' right is required to edit this page"
                    # 'cascadeprotected':"The page you're trying to edit is protected because it's included in a cascade-protected page"
                    # 'customcssjsprotected': "You're not allowed to edit custom CSS and JavaScript pages"
                    # 'protectednamespace': "You're not allowed to edit pages in the ``\$1'' namespace"
                    # 'protectednamespace-interface':"You're not allowed to edit interface messages"
                    #
                    # The page is locked. This should have already been
                    # detected when getting the page, but there are some
                    # reasons why this didn't work, e.g. the page might be
                    # locked via a cascade lock.
                    try:
                        # Page is locked - try using the sysop account, unless we're using one already
                        if sysop:# Unknown permissions error
                            raise LockedPage()
                        else:
                            self.site().forceLogin(sysop = True)
                            output(u'Page is locked, retrying using sysop account.')
                            return self._putPage(text, comment, watchArticle, minorEdit, newPage, token=self.site().getToken(sysop = True), sysop = True)
                    except NoUsername:
                        raise LockedPage()
                elif errorCode == 'badtoken':
                    if newToken:
                        output(u"Edit token has failed. Giving up.")
                    else:
                        # We might have been using an outdated token
                        output(u"Edit token has failed. Retrying.")
                        return self._putPage(text, comment, watchArticle, minorEdit, newPage, token=self.site().getToken(sysop = sysop, getagain = True), newToken = True, sysop = sysop)
                # I think the error message title was changed from "Wikimedia Error"
                # to "Wikipedia has a problem", but I'm not sure. Maybe we could
                # just check for HTTP Status 500 (Internal Server Error)?
                else:
                    output("Unknown Error. API Error code:%s" % data['error']['code'] )
                    output("Information:%s" % data['error']['info'])
            else:
                if data['edit']['result'] == u"Success":
                    #
                    # The status code for update page completed in ordinary mode is 302 - Found
                    # But API is always 200 - OK because it only send "success" back in string.
                    # if the page update is successed, we need to return code 302 for cheat script who
                    # using status code
                    #
                    return 302, response.msg, data['edit']

            solve = self.site().solveCaptcha(data)
            if solve:
                return self._putPage(text, comment, watchArticle, minorEdit, newPage, token, newToken, sysop, captcha=solve)

            return response.code, response.msg, data


    def _putPageOld(self, text, comment=None, watchArticle=False, minorEdit=True,
                newPage=False, token=None, newToken=False, sysop=False,
                captcha=None, botflag=True, maxTries=-1):
        """Upload 'text' as new content of Page by filling out the edit form.

        Don't use this directly, use put() instead.

        """
        host = self.site().hostname()
        # Get the address of the page on that host.
        address = self.site().put_address(self.urlname())
        predata = {
            'wpSave': '1',
            'wpSummary': self._encodeArg(comment, 'edit summary'),
            'wpTextbox1': self._encodeArg(text, 'wikitext'),
            # As of October 2008, MW HEAD requires wpSection to be set.
            # We will need to fill this more smartly if we ever decide to edit by section
            'wpSection': '',
        }
        if not botflag:
            predata['bot']='0'
        if captcha:
            predata["wpCaptchaId"] = captcha['id']
            predata["wpCaptchaWord"] = captcha['answer']
        # Add server lag parameter (see config.py for details)
        if config.maxlag:
            predata['maxlag'] = str(config.maxlag)
        # <s>Except if the page is new, we need to supply the time of the
        # previous version to the wiki to prevent edit collisions</s>
        # As of Oct 2008, these must be filled also for new pages
        if self._editTime:
            predata['wpEdittime'] = self._editTime
        else:
            predata['wpEdittime'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())
        if self._startTime:
            predata['wpStarttime'] = self._startTime
        else:
            predata['wpStarttime'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())
        if self._revisionId:
            predata['baseRevId'] = self._revisionId
        # Pass the minorEdit and watchArticle arguments to the Wiki.
        if minorEdit:
            predata['wpMinoredit'] = '1'
        if watchArticle:
            predata['wpWatchthis'] = '1'
        # Give the token, but only if one is supplied.
        if token:
            predata['wpEditToken'] = token

        # Sorry, single-site exception...
        if self.site().fam().name == 'loveto' and self.site().language() == 'recipes':
            predata['masteredit'] = '1'

        retry_delay = 1
        retry_attempt = 0
        dblagged = False
        wait = 5
        while True:
            if (maxTries == 0):
                raise MaxTriesExceededError()
            maxTries -= 1
            # Check whether we are not too quickly after the previous
            # putPage, and wait a bit until the interval is acceptable
            if not dblagged:
                put_throttle()
            # Which web-site host are we submitting to?
            if newPage:
                output(u'Creating page %s' % self.aslink())
            else:
                output(u'Changing page %s' % self.aslink())
            # Submit the prepared information
            try:
                response, data = self.site().postForm(address, predata, sysop)
                if response.code == 503:
                    if 'x-database-lag' in response.msg.keys():
                        # server lag; Mediawiki recommends waiting 5 seconds
                        # and retrying
                        if verbose:
                            output(data, newline=False)
                        output(u"Pausing %d seconds due to database server lag." % wait)
                        dblagged = True
                        time.sleep(wait)
                        wait = min(wait*2, 300)
                        continue
                    # Squid error 503
                    raise ServerError(response.code)
            except httplib.BadStatusLine, line:
                raise PageNotSaved('Bad status line: %s' % line.line)
            except ServerError:
                output(u''.join(traceback.format_exception(*sys.exc_info())))
                retry_attempt += 1
                if retry_attempt > config.maxretries:
                    raise
                output(
            u'Got a server error when putting %s; will retry in %i minute%s.'
                       % (self.aslink(), retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            # If it has gotten this far then we should reset dblagged
            dblagged = False
            # Check blocks
            self.site().checkBlocks(sysop = sysop)
            # A second text area means that an edit conflict has occured.
            editconflict1 = re.compile('id=["\']wpTextbox2[\'"] name="wpTextbox2"')
            editconflict2 = re.compile('name="wpTextbox2" id="wpTextbox2"')
            if editconflict1.search(data) or editconflict2.search(data):
                raise EditConflict(u'An edit conflict has occured.')

            # remove the wpAntispam keyword before checking for Spamfilter
            data = re.sub(u'(?s)<label for="wpAntispam">.*?</label>', '', data)
            if self.site().has_mediawiki_message("spamprotectiontitle")\
                    and self.site().mediawiki_message('spamprotectiontitle') in data:
                try:
                    reasonR = re.compile(re.escape(self.site().mediawiki_message('spamprotectionmatch')).replace('\$1', '(?P<url>[^<]*)'))
                    url = reasonR.search(data).group('url')
                except:
                    # Some wikis have modified the spamprotectionmatch
                    # template in a way that the above regex doesn't work,
                    # e.g. on he.wikipedia the template includes a
                    # wikilink, and on fr.wikipedia there is bold text.
                    # This is a workaround for this: it takes the region
                    # which should contain the spamfilter report and the
                    # URL. It then searches for a plaintext URL.
                    relevant = data[data.find('<!-- start content -->')+22:data.find('<!-- end content -->')].strip()
                    # Throw away all the other links etc.
                    relevant = re.sub('<.*?>', '', relevant)
                    relevant = relevant.replace('&#58;', ':')
                    # MediaWiki only spam-checks HTTP links, and only the
                    # domain name part of the URL.
                    m = re.search('http://[\w\-\.]+', relevant)
                    if m:
                        url = m.group()
                    else:
                        # Can't extract the exact URL. Let the user search.
                        url = relevant
                raise SpamfilterError(url)
            if '<label for=\'wpRecreate\'' in data:
                # Make sure your system clock is correct if this error occurs
                # without any reason!
                # raise EditConflict(u'Someone deleted the page.')
                # No raise, simply define these variables and retry:
                if self._editTime:
                    predata['wpEdittime'] = self._editTime
                else:
                    predata['wpEdittime'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())
                if self._startTime:
                    predata['wpStarttime'] = self._startTime
                else:
                    predata['wpStarttime'] = time.strftime('%Y%m%d%H%M%S', time.gmtime())
                continue
            if self.site().has_mediawiki_message("viewsource")\
                    and self.site().mediawiki_message('viewsource') in data:
                # The page is locked. This should have already been
                # detected when getting the page, but there are some
                # reasons why this didn't work, e.g. the page might be
                # locked via a cascade lock.
                try:
                    # Page is locked - try using the sysop account, unless we're using one already
                    if sysop:
                        # Unknown permissions error
                        raise LockedPage()
                    else:
                        self.site().forceLogin(sysop = True)
                        output(u'Page is locked, retrying using sysop account.')
                        return self._putPageOld(text, comment, watchArticle, minorEdit, newPage, token=self.site().getToken(sysop = True), sysop = True)
                except NoUsername:
                    raise LockedPage()
            if not newToken and "<textarea" in data:
                ##if "<textarea" in data: # for debug use only, if badtoken still happen
                # We might have been using an outdated token
                output(u"Changing page has failed. Retrying.")
                return self._putPageOld(text, comment, watchArticle, minorEdit, newPage, token=self.site().getToken(sysop = sysop, getagain = True), newToken = True, sysop = sysop)
            # I think the error message title was changed from "Wikimedia Error"
            # to "Wikipedia has a problem", but I'm not sure. Maybe we could
            # just check for HTTP Status 500 (Internal Server Error)?
            if ("<title>Wikimedia Error</title>" in data or "has a problem</title>" in data) \
                or response.code == 500:
                output(u"Server error encountered; will retry in %i minute%s."
                       % (retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            if ("1213: Deadlock found when trying to get lock" in data):
                output(u"Deadlock error encountered; will retry in %i minute%s."
                       % (retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            if self.site().mediawiki_message('readonly') in data  or self.site().mediawiki_message('readonly_lag') in data:
                output(u"The database is currently locked for write access; will retry in %i minute%s."
                       % (retry_delay, retry_delay != 1 and "s" or ""))
                time.sleep(60 * retry_delay)
                retry_delay *= 2
                if retry_delay > 30:
                    retry_delay = 30
                continue
            if self.site().has_mediawiki_message('longpageerror'):
                # FIXME: Long page error detection isn't working in Vietnamese Wikipedia.
                long_page_errorR = re.compile(
                    # Some wikis (e.g. Lithuanian and Slovak Wikipedia) use {{plural}} in
                    # [[MediaWiki:longpageerror]]
                    re.sub(r'\\{\\{plural\\:.*?\\}\\}', '.*?',
                        re.escape(
                            html2unicode(
                                self.site().mediawiki_message('longpageerror')
                            )
                        )
                    ).replace("\$1", "(?P<length>[\d,.\s]+)", 1).replace("\$2", "(?P<limit>[\d,.\s]+)", 1),
                re.UNICODE)

                match = long_page_errorR.search(data)
                if match:
                    # Some wikis (e.g. Lithuanian Wikipedia) don't use $2 parameter in
                    # [[MediaWiki:longpageerror]]
                    longpage_length = 0 ; longpage_limit = 0
                    if 'length' in match.groups():
                        longpage_length = match.group('length')
                    if 'limit' in match.groups():
                        longpage_limit = match.group('limit')
                    raise LongPageError(longpage_length, longpage_limit)

            # We might have been prompted for a captcha if the
            # account is not autoconfirmed, checking....
            ## output('%s' % data) # WHY?
            solve = self.site().solveCaptcha(data)
            if solve:
                return self._putPageOld(text, comment, watchArticle, minorEdit, newPage, token, newToken, sysop, captcha=solve)

            # We are expecting a 302 to the action=view page. I'm not sure why this was removed in r5019
            if  response.code != 302 and data.strip() != u"":
                # Something went wrong, and we don't know what. Show the
                # HTML code that hopefully includes some error message.
                output(u"ERROR: Unexpected response from wiki server.")
                output(u"       %s (%s) " % (response.code, response.msg))
                output(data)
                # Unexpected responses should raise an error and not pass,
                # be it silently or loudly. This should raise an error

            if 'name="wpTextbox1"' in data and 'var wgAction = "submit"' in data:
                # We are on the preview page, so the page was not saved
                raise PageNotSaved

            return response.code, response.msg, data

    def protection(self):
        """Return list of dicts of this page protection level. like:
        [{u'expiry': u'2010-05-26T14:41:51Z', u'type': u'edit', u'level': u'autoconfirmed'}, {u'expiry': u'2010-05-26T14:41:51Z', u'type': u'move', u'level': u'sysop'}]

        if the page non protection, return []
        """

        params = {
            'action': 'query',
            'prop'  : 'info',
            'inprop': 'protection',
            'titles' : self.title(),
            }

        datas = query.GetData(params, self.site())
        data=datas['query']['pages'].values()[0]['protection']
        return data

    def interwiki(self):
        """Return a list of interwiki links in the page text.

        This will retrieve the page to do its work, so it can raise
        the same exceptions that are raised by the get() method.

        The return value is a list of Page objects for each of the
        interwiki links in the page text.

        """
        if hasattr(self, "_interwikis"):
            return self._interwikis

        text = self.get()

        # Replace {{PAGENAME}} by its value
        for pagenametext in self.site().pagenamecodes(
                                                   self.site().language()):
            text = text.replace(u"{{%s}}" % pagenametext, self.title())

        ll = getLanguageLinks(text, insite=self.site(), pageLink=self.aslink())

        result = ll.values()

        self._interwikis = result
        return result



    def categories(self, get_redirect=False, api=False):
        """Return a list of Category objects that the article is in.
        Please be aware: the api call returns also categies which are included
        by templates. This differs to the old non-api code. If you need only
        these categories which are in the page text please use getCategoryLinks
        (or set api=False but this could be deprecated in future).
        """
        if not (self.site().has_api() and api):
            try:
                category_links_to_return = getCategoryLinks(self.get(get_redirect=get_redirect), self.site())
            except NoPage:
                category_links_to_return = []
            return category_links_to_return

        else:
            import catlib
            params = {
                'action': 'query',
                'prop'  : 'categories',
                'titles' : self.title(),
            }
            if not self.site().isAllowed('apihighlimits') and config.special_page_limit > 500:
                params['cllimit'] = 500

            output(u'Getting categories in %s via API...' % self.aslink())
            allDone = False
            cats=[]
            while not allDone:
                datas = query.GetData(params, self.site())
                data=datas['query']['pages'].values()[0]
                if "categories" in data:
                    for c in data['categories']:
                        if c['ns'] is 14:
                            cat = catlib.Category(self.site(), c['title'])
                            cats.append(cat)

                if 'query-continue' in datas:
                    if 'categories' in datas['query-continue']:
                        params['clcontinue'] = datas['query-continue']['categories']['clcontinue']
                else:
                    allDone = True
            return cats

    def __cmp__(self, other):
        """Test for equality and inequality of Page objects"""
        if not isinstance(other, Page):
            # especially, return -1 if other is None
            return -1
        if self._site == other._site:
            return cmp(self._title, other._title)
        else:
            return cmp(self._site, other._site)

    def __hash__(self):
        # Pseudo method that makes it possible to store Page objects as keys
        # in hash-tables. This relies on the fact that the string
        # representation of an instance can not change after the construction.
        return hash(str(self))

    def linkedPages(self, withImageLinks = False):
        """Return a list of Pages that this Page links to.

        Excludes interwiki and category links, and also image links by default.
        """
        result = []
        try:
            thistxt = removeLanguageLinks(self.get(get_redirect=True),
                                          self.site())
        except NoPage:
            raise
        except IsRedirectPage:
            raise
        except SectionError:
            return []
        thistxt = removeCategoryLinks(thistxt, self.site())

        # remove HTML comments, pre, nowiki, and includeonly sections
        # from text before processing
        thistxt = removeDisabledParts(thistxt)

        # resolve {{ns:-1}} or {{ns:Help}}
        thistxt = self.site().resolvemagicwords(thistxt)

        for match in Rlink.finditer(thistxt):
            title = match.group('title')
            title = title.replace("_", " ").strip(" ")
            if title.startswith("#"):
                # this is an internal section link
                continue
            if not self.site().isInterwikiLink(title):
                try:
                    page = Page(self.site(), title)
                    try:
                        hash(str(page))
                    except Exception:
                        raise Error(u"Page %s contains invalid link to [[%s]]."
                                    % (self.title(), title))
                except Error:
                    if verbose:
                        output(u"Page %s contains invalid link to [[%s]]."
                               % (self.title(), title))
                    continue
                if not withImageLinks and page.isImage():
                    continue
                if page.sectionFreeTitle() and page not in result:
                    result.append(page)
        return result

    def imagelinks(self, followRedirects=False, loose=False):
        """Return a list of ImagePage objects for images displayed on this Page.

        Includes images in galleries.
        If loose is True, this will find anything that looks like it
        could be an image. This is useful for finding, say, images that are
        passed as parameters to templates.

        """
        results = []
        # Find normal images
        for page in self.linkedPages(withImageLinks = True):
            if page.isImage():
                # convert Page object to ImagePage object
                results.append( ImagePage(page.site(), page.title()) )
        # Find images in galleries
        pageText = self.get(get_redirect=followRedirects)
        galleryR = re.compile('<gallery>.*?</gallery>', re.DOTALL)
        galleryEntryR = re.compile('(?P<title>(%s|%s):.+?)(\|.+)?\n' % (self.site().image_namespace(), self.site().family.image_namespace(code = '_default')))
        for gallery in galleryR.findall(pageText):
            for match in galleryEntryR.finditer(gallery):
                results.append( ImagePage(self.site(), match.group('title')) )
        if loose:
            ns = getSite().image_namespace()
            imageR = re.compile('\w\w\w+\.(?:gif|png|jpg|jpeg|svg|JPG|xcf|pdf|mid|ogg|djvu)', re.IGNORECASE)
            for imageName in imageR.findall(pageText):
                results.append( ImagePage(self.site(), imageName) )
        return list(set(results))

    def templates(self, get_redirect=False):
        """Return a list of titles (unicode) of templates used on this Page.

        Template parameters are ignored.
        """
        if not hasattr(self, "_templates"):
            self._templates = list(set([template
                                       for (template, param)
                                       in self.templatesWithParams(
                                               get_redirect=get_redirect)]))
        return self._templates

    def templatesWithParams(self, thistxt=None, get_redirect=False):
        """Return a list of templates used on this Page.

        Return value is a list of tuples. There is one tuple for each use of
        a template in the page, with the template title as the first entry
        and a list of parameters as the second entry.

        If thistxt is set, it is used instead of current page content.
        """
        if not thistxt:
            try:
                thistxt = self.get(get_redirect=get_redirect)
            except (IsRedirectPage, NoPage):
                return []

        # remove commented-out stuff etc.
        thistxt  = removeDisabledParts(thistxt)

        # marker for inside templates or parameters
        marker = findmarker(thistxt,  u'@@', u'@')

        # marker for links
        marker2 = findmarker(thistxt,  u'##', u'#')

        # marker for math
        marker3 = findmarker(thistxt,  u'%%', u'%')

        result = []
        inside = {}
        count = 0
        Rtemplate = re.compile(
                    ur'{{(msg:)?(?P<name>[^{\|]+?)(\|(?P<params>[^{]*?))?}}')
        Rlink = re.compile(ur'\[\[[^\]]+\]\]')
        Rmath = re.compile(ur'<math>[^<]+</math>')
        Rmarker = re.compile(ur'%s(\d+)%s' % (marker, marker))
        Rmarker2 = re.compile(ur'%s(\d+)%s' % (marker2, marker2))
        Rmarker3 = re.compile(ur'%s(\d+)%s' % (marker3, marker3))

        # Replace math with markers
        maths = {}
        count = 0
        for m in Rmath.finditer(thistxt):
            count += 1
            text = m.group()
            thistxt = thistxt.replace(text, '%s%d%s' % (marker3, count, marker3))
            maths[count] = text

        while Rtemplate.search(thistxt) is not None:
            for m in Rtemplate.finditer(thistxt):
                # Make sure it is not detected again
                count += 1
                text = m.group()
                thistxt = thistxt.replace(text,
                                          '%s%d%s' % (marker, count, marker))
                # Make sure stored templates don't contain markers
                for m2 in Rmarker.finditer(text):
                    text = text.replace(m2.group(), inside[int(m2.group(1))])
                for m2 in Rmarker3.finditer(text):
                    text = text.replace(m2.group(), maths[int(m2.group(1))])
                inside[count] = text

                # Name
                name = m.group('name').strip()
                m2 = Rmarker.search(name) or Rmath.search(name)
                if m2 is not None:
                    # Doesn't detect templates whose name changes,
                    # or templates whose name contains math tags
                    continue
                if self.site().isInterwikiLink(name):
                    continue

                # {{#if: }}
                if name.startswith('#'):
                    continue
                # {{DEFAULTSORT:...}}
                defaultKeys = self.site().versionnumber() > 13 and \
                              self.site().getmagicwords('defaultsort')
                # It seems some wikis does not have this magic key
                if defaultKeys:
                    found = False
                    for key in defaultKeys:
                        if name.startswith(key):
                            found = True
                            break
                    if found: continue

                try:
                    name = Page(self.site(), name).title()
                except InvalidTitle:
                    if name:
                        output(
                            u"Page %s contains invalid template name {{%s}}."
                           % (self.title(), name.strip()))
                    continue
                # Parameters
                paramString = m.group('params')
                params = []
                if paramString:
                    # Replace links to markers
                    links = {}
                    count2 = 0
                    for m2 in Rlink.finditer(paramString):
                        count2 += 1
                        text = m2.group()
                        paramString = paramString.replace(text,
                                        '%s%d%s' % (marker2, count2, marker2))
                        links[count2] = text
                    # Parse string
                    markedParams = paramString.split('|')
                    # Replace markers
                    for param in markedParams:
                        for m2 in Rmarker.finditer(param):
                            param = param.replace(m2.group(),
                                                  inside[int(m2.group(1))])
                        for m2 in Rmarker2.finditer(param):
                            param = param.replace(m2.group(),
                                                  links[int(m2.group(1))])
                        for m2 in Rmarker3.finditer(param):
                            param = param.replace(m2.group(),
                                                  maths[int(m2.group(1))])
                        params.append(param)

                # Add it to the result
                result.append((name, params))
        return result

    def getRedirectTarget(self):
        """Return a Page object for the target this Page redirects to.

        If this page is not a redirect page, will raise an IsNotRedirectPage
        exception. This method also can raise a NoPage exception.

        """
        try:
            self.get()
        except NoPage:
            raise
        except IsRedirectPage, err:
            # otherwise it will return error pages with " inside.
            target = err[0].replace('&amp;quot;', '"')

            if '|' in target:
                warnings.warn("'%s' has a | character, this makes no sense"
                              % target, Warning)
            return Page(self.site(), target)
        else:
            raise IsNotRedirectPage(self)

    def getVersionHistory(self, forceReload=False, reverseOrder=False,
                          getAll=False, revCount=500):
        """Load the version history page and return history information.

        Return value is a list of tuples, where each tuple represents one
        edit and is built of revision id, edit date/time, user name,
        edit summary, size and tags. Starts with the most current revision,
        unless reverseOrder is True.
        Defaults to getting the first revCount edits, unless getAll is True.

        @param revCount: iterate no more than this number of revisions in total
        """

        # regular expression matching one edit in the version history.
        # results will have 4 groups: oldid, edit date/time, user name, and edit
        # summary.
        thisHistoryDone = False
        skip = False # Used in determining whether we need to skip the first page
        dataQuery = []
        hasData = False


        # Are we getting by Earliest first?
        if reverseOrder:
            # Check if _versionhistoryearliest exists
            if not hasattr(self, '_versionhistoryearliest') or forceReload:
                self._versionhistoryearliest = []
            elif getAll and len(self._versionhistoryearliest) == revCount:
                # Cause a reload, or at least make the loop run
                thisHistoryDone = False
                skip = True
                dataQuery = self._versionhistoryearliest
            else:
                thisHistoryDone = True
        elif not hasattr(self, '_versionhistory') or forceReload or \
             len(self._versionhistory) < revCount:
            self._versionhistory = []
        # ?? does not load if len(self._versionhistory) > revCount
        # shouldn't it
        elif getAll and len(self._versionhistory) == revCount:
            # Cause a reload, or at least make the loop run
            thisHistoryDone = False
            skip = True
            dataQuery = self._versionhistory
        else:
            thisHistoryDone = True

        if not thisHistoryDone:
            dataQuery.extend(self._getVersionHistory(getAll, skip, reverseOrder, revCount))

        if reverseOrder:
            # Return only revCount edits, even if the version history is extensive
            if dataQuery != []:
                self._versionhistoryearliest = dataQuery
                del dataQuery
            if len(self._versionhistoryearliest) > revCount and not getAll:
                return self._versionhistoryearliest[:revCount]
            return self._versionhistoryearliest

        if dataQuery != []:
            self._versionhistory = dataQuery
            del dataQuery
        # Return only revCount edits, even if the version history is extensive
        if len(self._versionhistory) > revCount and not getAll:
            return self._versionhistory[:revCount]
        return self._versionhistory

    def _getVersionHistory(self, getAll=False, skipFirst=False, reverseOrder=False,
                           revCount=500):
        """Load history informations by API query.
           Internal use for self.getVersionHistory(), don't use this function directly.
        """
        if not self.site().has_api() or self.site().versionnumber() < 8:
            return self._getVersionHistoryOld(reExist, getAll, skipFirst, reverseOrder, revCount)
        dataQ = []
        thisHistoryDone = False
        params = {
            'action': 'query',
            'prop': 'revisions',
            'titles': self.title(),
            'rvprop': 'ids|timestamp|flags|comment|user|size|tags',
            'rvlimit': revCount,
        }
        while not thisHistoryDone:
            if reverseOrder:
                params['rvdir'] = 'newer'

            result = query.GetData(params, self.site())
            if 'error' in result:
                raise RuntimeError("%s" % result['error'])
            pageInfo = result['query']['pages'].values()[0]
            if result['query']['pages'].keys()[0] == "-1":
                if 'missing' in pageInfo:
                    raise NoPage(self.site(), self.aslink(forceInterwiki=True),
                                 "Page does not exist.")
                elif 'invalid' in pageInfo:
                    raise BadTitle('BadTitle: %s' % self)

            if 'query-continue' in result and getAll:
                params['rvstartid'] = result['query-continue']['revisions']['rvstartid']
            else:
                thisHistoryDone = True

            if skipFirst:
                skipFirst = False
            else:
                for r in pageInfo['revisions']:
                    c = ''
                    if 'comment' in r:
                        c = r['comment']
                    #revision id, edit date/time, user name, edit summary
                    (revidStrr, timestampStrr, userStrr) = (None, None, None)
                    if 'revid' in r:
                        revidStrr = r['revid']
                    if 'timestamp' in r:
                        timestampStrr = r['timestamp']
                    if 'user' in r:
                        userStrr = r['user']
                    s=-1 #Will return -1 if not found
                    if 'size' in r:
                        s = r['size']
                    tags=[]
                    if 'tags' in r:
                        tags = r['tags']
                    dataQ.append((revidStrr, timestampStrr, userStrr, c, s, tags))
                if len(result['query']['pages'].values()[0]['revisions']) < revCount:
                    thisHistoryDone = True
        return dataQ

    def _getVersionHistoryOld(self, getAll = False, skipFirst = False,
                               reverseOrder = False, revCount=500):
        """Load the version history page and return history information.
           Internal use for self.getVersionHistory(), don't use this function directly.
        """
        dataQ = []
        thisHistoryDone = False
        startFromPage = None
        if self.site().versionnumber() < 4:
            editR = re.compile('<li>\(.*?\)\s+\(.*\).*?<a href=".*?oldid=([0-9]*)" title=".*?">([^<]*)</a> <span class=\'user\'><a href=".*?" title=".*?">([^<]*?)</a></span>.*?(?:<span class=\'comment\'>(.*?)</span>)?</li>')
        elif self.site().versionnumber() < 15:
            editR = re.compile('<li>\(.*?\)\s+\(.*\).*?<a href=".*?oldid=([0-9]*)" title=".*?">([^<]*)</a> (?:<span class=\'history-user\'>|)<a href=".*?" title=".*?">([^<]*?)</a>.*?(?:</span>|).*?(?:<span class=[\'"]comment[\'"]>(.*?)</span>)?</li>')
        elif self.site().versionnumber() < 16:
            editR = re.compile(r'<li class=".*?">\((?:\w*|<a[^<]*</a>)\)\s\((?:\w*|<a[^<]*</a>)\).*?<a href=".*?([0-9]*)" title=".*?">([^<]*)</a> <span class=\'history-user\'><a [^>]*?>([^<]*?)</a>.*?</span></span>(?: <span class="minor">.*?</span>|)(?: <span class="history-size">.*?</span>|)(?: <span class=[\'"]comment[\'"]>\((?:<span class="autocomment">|)(.*?)(?:</span>|)\)</span>)?(?: \(<span class="mw-history-undo">.*?</span>\)|)\s*</li>', re.UNICODE)
        else:
            editR = re.compile(r'<li(?: class="mw-tag[^>]+)?>\((?:\w+|<a[^<]*</a>)\)\s\((?:\w+|<a[^<]*</a>)\).*?<a href=".*?([0-9]*)" title=".*?">([^<]*)</a> <span class=\'history-user\'><a [^>]*?>([^<]*?)</a>.*?</span></span>(?: <abbr class="minor"[^>]*?>.*?</abbr>|)(?: <span class="history-size">.*?</span>|)(?: <span class="comment">\((?:<span class="autocomment">|)(.*?)(?:</span>|)\)</span>)?(?: \(<span class="mw-history-undo">.*?</span>\))?(?: <span class="mw-tag-markers">.*?</span>\)</span>)?\s*</li>', re.UNICODE)

        RLinkToNextPage = re.compile('&amp;offset=(.*?)&amp;')

        while not thisHistoryDone:
            path = self.site().family.version_history_address(self.site().language(), self.urlname(), config.special_page_limit)

            if reverseOrder:
                path += '&dir=prev'

            if startFromPage:
                path += '&offset=' + startFromPage

            # this loop will run until the page could be retrieved
            # Try to retrieve the page until it was successfully loaded (just in case
            # the server is down or overloaded)
            # wait for retry_idle_time minutes (growing!) between retries.
            retry_idle_time = 1

            if verbose:
                if startFromPage:
                    output(u'Continuing to get version history of %s' % self.aslink(forceInterwiki = True))
                else:
                    output(u'Getting version history of %s' % self.aslink(forceInterwiki = True))

            txt = self.site().getUrl(path)

            # save a copy of the text
            self_txt = txt

            #Find the nextPage link, if not exist, the page is last history page
            matchObj = RLinkToNextPage.search(self_txt)
            if getAll and matchObj:
                startFromPage = matchObj.group(1)
            else:
                thisHistoryDone = True

            if not skipFirst:
                edits = editR.findall(self_txt)

            if skipFirst:
                # Skip the first page only,
                skipFirst = False
            else:
                if reverseOrder:
                    edits.reverse()
                #for edit in edits:
                dataQ.extend(edits)
                if len(edits) < revCount:
                    thisHistoryDone = True
        return dataQ

    def getVersionHistoryTable(self, forceReload=False, reverseOrder=False,
                               getAll=False, revCount=500):
        """Return the version history as a wiki table."""

        result = '{| class="wikitable"\n'
        result += '! oldid || date/time || size || username || edit summary\n'
        for oldid, time, username, summary, size, tags \
                in self.getVersionHistory(forceReload=forceReload,
                                          reverseOrder=reverseOrder,
                                          getAll=getAll, revCount=revCount):
            result += '|----\n'
            result += '| %s || %s || %d || %s || <nowiki>%s</nowiki>\n' \
                      % (oldid, time, size, username, summary)
        result += '|}\n'
        return result

    def fullVersionHistory(self, getAll=False, skipFirst=False, reverseOrder=False,
                           revCount=500):
        """Iterate previous versions including wikitext.

        Gives a list of tuples consisting of revision ID, edit date/time, user name and
        content

        """
        if not self.site().has_api() or self.site().versionnumber() < 8:
            address = self.site().export_address()
            predata = {
                'action': 'submit',
                'pages': self.title()
            }
            get_throttle(requestsize = 10)
            now = time.time()
            response, data = self.site().postForm(address, predata)
            data = data.encode(self.site().encoding())
#        get_throttle.setDelay(time.time() - now)
            output = []
        # TODO: parse XML using an actual XML parser instead of regex!
            r = re.compile("\<revision\>.*?\<id\>(?P<id>.*?)\<\/id\>.*?\<timestamp\>(?P<timestamp>.*?)\<\/timestamp\>.*?\<(?:ip|username)\>(?P<user>.*?)\</(?:ip|username)\>.*?\<text.*?\>(?P<content>.*?)\<\/text\>",re.DOTALL)
        #r = re.compile("\<revision\>.*?\<timestamp\>(.*?)\<\/timestamp\>.*?\<(?:ip|username)\>(.*?)\<",re.DOTALL)
            return [  (match.group('id'),
                       match.group('timestamp'),
                       unescape(match.group('user')),
                       unescape(match.group('content')))
                    for match in r.finditer(data)  ]

        # Load history informations by API query.

        dataQ = []
        thisHistoryDone = False
        params = {
            'action': 'query',
            'prop': 'revisions',
            'titles': self.title(),
            'rvprop': 'ids|timestamp|user|content',
            'rvlimit': revCount,
        }
        while not thisHistoryDone:
            if reverseOrder:
                params['rvdir'] = 'newer'

            result = query.GetData(params, self.site())
            if 'error' in result:
                raise RuntimeError("%s" % result['error'])
            pageInfo = result['query']['pages'].values()[0]
            if result['query']['pages'].keys()[0] == "-1":
                if 'missing' in pageInfo:
                    raise NoPage(self.site(), self.aslink(forceInterwiki=True),
                                 "Page does not exist.")
                elif 'invalid' in pageInfo:
                    raise BadTitle('BadTitle: %s' % self)

            if 'query-continue' in result and getAll:
                params['rvstartid'] = result['query-continue']['revisions']['rvstartid']
            else:
                thisHistoryDone = True

            if skipFirst:
                skipFirst = False
            else:
                for r in pageInfo['revisions']:
                    c = ''
                    if 'comment' in r:
                        c = r['comment']
                    #revision id, edit date/time, user name, edit summary
                    (revidStrr, timestampStrr, userStrr) = (None, None, None)
                    if 'revid' in r:
                        revidStrr = r['revid']
                    if 'timestamp' in r:
                        timestampStrr = r['timestamp']
                    if 'user' in r:
                        userStrr = r['user']
                    s='' #Will return -1 if not found
                    if '*' in r:
                        s = r['*']
                    dataQ.append((revidStrr, timestampStrr, userStrr, s))
                if len(result['query']['pages'].values()[0]['revisions']) < revCount:
                    thisHistoryDone = True
        return dataQ

    def contributingUsers(self, step=None, total=None):
        """Return a set of usernames (or IPs) of users who edited this page.

        @param step: limit each API call to this number of revisions
                     - not used yet, only in rewrite branch -
        @param total: iterate no more than this number of revisions in total

        """
        if total == None:
            total = 500 #set to default of getVersionHistory
        edits = self.getVersionHistory(revCount=total)
        users = set([edit[2] for edit in edits])
        return users

    def watch(self, unwatch=False):
        """Add this page to the watchlist"""
        if self.site().has_api:
            params = {
                'action': 'watch',
                'title': self.title()
            }
            if unwatch:
                params['unwatch'] = ''

            data = query.GetData(params, self.site())
            if 'error' in data:
                raise RuntimeError("API query error: %s" % data['error'])
        else:
            urlname = self.urlname()
            if not unwatch:
                address = self.site().watch_address(urlname)
            else:
                address = self.site().unwatch_address(urlname)
            response = self.site().getUrl(address)
            return response

    def unwatch(self):
        self.watch(unwatch=True)

    def move(self, newtitle, reason=None, movetalkpage=True, movesubpages=False, sysop=False,
             throttle=True, deleteAndMove=False, safe=True, fixredirects=True, leaveRedirect=True):
        """Move this page to new title given by newtitle. If safe, don't try
        to move and delete if not directly requested.

        * fixredirects has no effect in MW < 1.13

        @param newtitle: The new page title.
        @param reason: The edit summary for the move.
        @param movetalkpage: If true, move this page's talk page (if it exists)
        @param sysop: Try to move using sysop account, if available
        @param deleteAndMove: if move succeeds, delete the old page
            (usually requires sysop privileges, depending on wiki settings)
        @param safe: If false, attempt to delete existing page at newtitle
            (if there is one) and then move this page to that title
        """
        if not self.site().has_api() or self.site().versionnumber() < 12:
            return self._moveOld(newtitle, reason, movetalkpage, sysop,
              throttle, deleteAndMove, safe, fixredirects, leaveRedirect)
        # Login
        try:
            self.get()
        except:
            pass
        sysop = self._getActionUser(action = 'move', restriction = self.moveRestriction, sysop = False)
        if deleteAndMove:
            sysop = self._getActionUser(action = 'delete', restriction = '', sysop = True)
            Page(self.site(), newtitle).delete(self.site().mediawiki_message('delete_and_move_reason'), False, False)

        # Check blocks
        self.site().checkBlocks(sysop = sysop)

        if throttle:
            put_throttle()
        if reason is None:
            pywikibot.output(u'Moving %s to [[%s]].'
                             % (self.title(asLink=True), newtitle))
            reason = input(u'Please enter a reason for the move:')
        if self.isTalkPage():
            movetalkpage = False

        params = {
            'action': 'move',
            'from': self.title(),
            'to': newtitle,
            'token': self.site().getToken(sysop=sysop),
            'reason': reason,
        }
        if movesubpages:
            params['movesubpages'] = 1

        if movetalkpage:
            params['movetalk'] = 1

        if not leaveRedirect:
            params['noredirect'] = 1

        result = query.GetData(params, self.site(), sysop=sysop)
        if 'error' in result:
            err = result['error']['code']
            if err == 'articleexists':
                if safe:
                    output(u'Page move failed: Target page [[%s]] already exists.' % newtitle)
                else:
                    try:
                        # Try to delete and move
                        return self.move(newtitle, reason, movetalkpage, movesubpages, throttle = throttle, deleteAndMove = True)
                    except NoUsername:
                        # We dont have the user rights to delete
                        output(u'Page moved failed: Target page [[%s]] already exists.' % newtitle)
            #elif err == 'protectedpage':
            #
            else:
                output("Unknown Error: %s" % result)
            return False
        elif 'move' in result:
            if deleteAndMove:
                output(u'Page %s moved to %s, deleting the existing page' % (self.title(), newtitle))
            else:
                output(u'Page %s moved to %s' % (self.title(), newtitle))

            if hasattr(self, '_contents'):
                #self.__init__(self.site(), newtitle, defaultNamespace = self._namespace)
                try:
                    self.get(force=True, get_redirect=True, throttle=False)
                except NoPage:
                    output(u'Page %s is moved and no longer exist.' % self.title() )
                    #delattr(self, '_contents')
            return True

    def _moveOld(self, newtitle, reason=None, movetalkpage=True, movesubpages=False, sysop=False,
             throttle=True, deleteAndMove=False, safe=True, fixredirects=True, leaveRedirect=True):

        # Login
        try:
            self.get()
        except:
            pass
        sysop = self._getActionUser(action = 'move', restriction = self.moveRestriction, sysop = False)
        if deleteAndMove:
            sysop = self._getActionUser(action = 'delete', restriction = '', sysop = True)

        # Check blocks
        self.site().checkBlocks(sysop = sysop)

        if throttle:
            put_throttle()
        if reason is None:
            reason = input(u'Please enter a reason for the move:')
        if self.isTalkPage():
            movetalkpage = False

        host = self.site().hostname()
        address = self.site().move_address()
        token = self.site().getToken(sysop = sysop)
        predata = {
            'wpOldTitle': self.title().encode(self.site().encoding()),
            'wpNewTitle': newtitle.encode(self.site().encoding()),
            'wpReason': reason.encode(self.site().encoding()),
        }
        if deleteAndMove:
            predata['wpDeleteAndMove'] = self.site().mediawiki_message('delete_and_move_confirm')
            predata['wpConfirm'] = '1'

        if movetalkpage:
            predata['wpMovetalk'] = '1'
        else:
            predata['wpMovetalk'] = '0'

        if self.site().versionnumber() >= 13:
            if fixredirects:
                predata['wpFixRedirects'] = '1'
            else:
                predata['wpFixRedirects'] = '0'

        if leaveRedirect:
            predata['wpLeaveRedirect'] = '1'
        else:
            predata['wpLeaveRedirect'] = '0'

        if movesubpages:
            predata['wpMovesubpages'] = '1'
        else:
            predata['wpMovesubpages'] = '0'

        if token:
            predata['wpEditToken'] = token

        response, data = self.site().postForm(address, predata, sysop = sysop)

        if data == u'' or self.site().mediawiki_message('pagemovedsub') in data:
            #Move Success
            if deleteAndMove:
                output(u'Page %s moved to %s, deleting the existing page' % (self.title(), newtitle))
            else:
                output(u'Page %s moved to %s' % (self.title(), newtitle))

            if hasattr(self, '_contents'):
                #self.__init__(self.site(), newtitle, defaultNamespace = self._namespace)
                try:
                    self.get(force=True, get_redirect=True, throttle=False)
                except NoPage:
                    output(u'Page %s is moved and no longer exist.' % self.title() )
                    #delattr(self, '_contents')

            return True
        else:
            #Move Failure
            self.site().checkBlocks(sysop = sysop)
            if self.site().mediawiki_message('articleexists') in data or self.site().mediawiki_message('delete_and_move') in data:
                if safe:
                    output(u'Page move failed: Target page [[%s]] already exists.' % newtitle)
                    return False
                else:
                    try:
                        # Try to delete and move
                        return self._moveOld(newtitle, reason, movetalkpage, movesubpages, throttle = throttle, deleteAndMove = True)
                    except NoUsername:
                        # We dont have the user rights to delete
                        output(u'Page moved failed: Target page [[%s]] already exists.' % newtitle)
                        return False
            elif not self.exists():
                raise NoPage(u'Page move failed: Source page [[%s]] does not exist.' % newtitle)
            elif Page(self.site(),newtitle).exists():
                # XXX : This might be buggy : if the move was successful, the target pase *has* been created
                raise PageNotSaved(u'Page move failed: Target page [[%s]] already exists.' % newtitle)
            else:
                output(u'Page move failed for unknown reason.')
                try:
                    ibegin = data.index('<!-- start content -->') + 22
                    iend = data.index('<!-- end content -->')
                except ValueError:
                    # if begin/end markers weren't found, show entire HTML file
                    output(data)
                else:
                    # otherwise, remove the irrelevant sections
                    data = data[ibegin:iend]
                output(data)
                return False

    def delete(self, reason=None, prompt=True, throttle=True, mark=False):
        """Deletes the page from the wiki.

        Requires administrator status. If reason is None, asks for a
        reason. If prompt is True, asks the user if he wants to delete the
        page.

        If the user does not have admin rights and mark is True,
        the page is marked for deletion instead.
        """
        # Login
        try:
            self._getActionUser(action = 'delete', sysop = True)
        except NoUsername:
             if mark and self.exists():
                 text = self.get(get_redirect = True)
                 output(u'Cannot delete page %s - marking the page for deletion instead:' % self.aslink())
                 # Note: Parameters to {{delete}}, and their meanings, vary from one Wikipedia to another.
                 # If you want or need to use them, you must be careful not to break others. Else don't.
                 self.put(u'{{delete|bot=yes}}\n%s --~~~~\n----\n\n%s' % (reason, text), comment = reason)
                 return
             else:
                 raise

        # Check blocks
        self.site().checkBlocks(sysop = True)

        if throttle:
            put_throttle()
        if reason is None:
            reason = input(u'Please enter a reason for the deletion:')
        answer = 'y'
        if prompt and not hasattr(self.site(), '_noDeletePrompt'):
            answer = inputChoice(u'Do you want to delete %s?' % self.aslink(forceInterwiki = True), ['yes', 'no', 'all'], ['y', 'N', 'a'], 'N')
            if answer == 'a':
                answer = 'y'
                self.site()._noDeletePrompt = True
        if answer == 'y':

            token = self.site().getToken(self, sysop = True)
            reason = reason.encode(self.site().encoding())

            if self.site().has_api() and self.site().versionnumber() >= 12:
                #API Mode
                params = {
                    'action': 'delete',
                    'title': self.title(),
                    'token': token,
                    'reason': reason,
                }
                datas = query.GetData(params, self.site(), sysop = True)
                if 'delete' in datas:
                    output(u'Page %s deleted' % self.aslink(forceInterwiki = True))
                    return True
                else:
                    if datas['error']['code'] == 'missingtitle':
                        output(u'Page %s could not be deleted - it doesn\'t exist' % self.aslink(forceInterwiki = True))
                    else:
                        output(u'Deletion of %s failed for an unknown reason. The response text is:' % self.aslink(forceInterwiki = True))
                        output('%s' % datas)

                    return False
            else:
                #Ordinary mode from webpage.
                host = self.site().hostname()
                address = self.site().delete_address(self.urlname())

                predata = {
                    'wpDeleteReasonList': 'other',
                    'wpReason': reason,
                    #'wpComment': reason, <- which version?
                    'wpConfirm': '1',
                    'wpConfirmB': '1',
                    'wpEditToken': token,
                }
                response, data = self.site().postForm(address, predata, sysop = True)
                if data:
                    self.site().checkBlocks(sysop = True)
                    if self.site().mediawiki_message('actioncomplete') in data:
                        output(u'Page %s deleted' % self.aslink(forceInterwiki = True))
                        return True
                    elif self.site().mediawiki_message('cannotdelete') in data:
                        output(u'Page %s could not be deleted - it doesn\'t exist' % self.aslink(forceInterwiki = True))
                        return False
                    else:
                        output(u'Deletion of %s failed for an unknown reason. The response text is:' % self.aslink(forceInterwiki = True))
                        try:
                            ibegin = data.index('<!-- start content -->') + 22
                            iend = data.index('<!-- end content -->')
                        except ValueError:
                            # if begin/end markers weren't found, show entire HTML file
                            output(data)
                        else:
                            # otherwise, remove the irrelevant sections
                            data = data[ibegin:iend]
                        output(data)
                        return False

    def loadDeletedRevisions(self):
        """Retrieve all deleted revisions for this Page from Special/Undelete.

        Stores all revisions' timestamps, dates, editors and comments.
        Returns list of timestamps (which can be used to retrieve revisions
        later on).

        """
        # Login
        self._getActionUser(action = 'deletedhistory', sysop = True)

        #TODO: Handle image file revisions too.
        output(u'Loading list of deleted revisions for [[%s]]...' % self.title())

        self._deletedRevs = {}

        if self.site().has_api() and self.site().versionnumber() >= 12:
            params = {
                'action': 'query',
                'list': 'deletedrevs',
                'drfrom': self.titleWithoutNamespace(),
                'drnamespace': self.namespace(),
                'drprop': ['revid','user','comment','content'],#','minor','len','token'],
                'drlimit': 100,
                'drdir': 'older',
                #'': '',
            }
            count = 0
            while True:
                data = query.GetData(params, self.site(), sysop=True)
                for x in data['query']['deletedrevs']:
                    if x['title'] != self.title():
                        continue

                    for y in x['revisions']:
                        count += 1
                        self._deletedRevs[parsetime2stamp(y['timestamp'])] = [y['timestamp'], y['user'], y['comment'] , y['*'], False]

                if 'query-continue' in data and data['query-continue']['deletedrevs']['drcontinue'].split('|')[1] == self.titleWithoutNamespace():
                    params['drcontinue'] = data['query-continue']['deletedrevs']['drcontinue']
                else:
                    break
            self._deletedRevsModified = False

        else:
            address = self.site().undelete_view_address(self.urlname())
            text = self.site().getUrl(address, sysop = True)
            #TODO: Handle non-existent pages etc

            rxRevs = re.compile(r'<input name="(?P<ts>(?:ts|fileid)\d+)".*?title=".*?">(?P<date>.*?)</a>.*?title=".*?">(?P<editor>.*?)</a>.*?<span class="comment">\((?P<comment>.*?)\)</span>',re.DOTALL)
            for rev in rxRevs.finditer(text):
                self._deletedRevs[rev.group('ts')] = [
                        rev.group('date'),
                        rev.group('editor'),
                        rev.group('comment'),
                        None,  #Revision text
                        False, #Restoration marker
                        ]

            self._deletedRevsModified = False

        return self._deletedRevs.keys()

    def getDeletedRevision(self, timestamp, retrieveText=False):
        """Return a particular deleted revision by timestamp.

        Return value is a list of [date, editor, comment, text, restoration
        marker]. text will be None, unless retrieveText is True (or has been
        retrieved earlier).

        """
        if self._deletedRevs is None:
            self.loadDeletedRevisions()
        if timestamp not in self._deletedRevs:
            #TODO: Throw an exception instead?
            return None

        if retrieveText and not self._deletedRevs[timestamp][3] and timestamp[:2]=='ts':
            # Login
            self._getActionUser(action = 'delete', sysop = True)

            output(u'Retrieving text of deleted revision...')
            address = self.site().undelete_view_address(self.urlname(),timestamp)
            text = self.site().getUrl(address, sysop = True)
            und = re.search('<textarea readonly="1" cols="80" rows="25">(.*?)</textarea><div><form method="post"',text,re.DOTALL)
            if und:
                self._deletedRevs[timestamp][3] = und.group(1)

        return self._deletedRevs[timestamp]

    def markDeletedRevision(self, timestamp, undelete=True):
        """Mark the revision identified by timestamp for undeletion.

        If undelete is False, mark the revision to remain deleted.

        """
        if self._deletedRevs is None:
            self.loadDeletedRevisions()
        if timestamp not in self._deletedRevs:
            #TODO: Throw an exception?
            return None
        self._deletedRevs[timestamp][4] = undelete
        self._deletedRevsModified = True

    def undelete(self, comment='', throttle=True):
        """Undeletes page based on the undeletion markers set by previous calls.

        If no calls have been made since loadDeletedRevisions(), everything
        will be restored.

        Simplest case:
            wikipedia.Page(...).undelete('This will restore all revisions')

        More complex:
            pg = wikipedia.Page(...)
            revs = pg.loadDeletedRevsions()
            for rev in revs:
                if ... #decide whether to undelete a revision
                    pg.markDeletedRevision(rev) #mark for undeletion
            pg.undelete('This will restore only selected revisions.')

        """
        # Login
        self._getActionUser(action = 'undelete', sysop = True)

        # Check blocks
        self.site().checkBlocks(sysop = True)

        token = self.site().getToken(self, sysop=True)

        if throttle:
            put_throttle()

        if self.site().has_api() and self.site().versionnumber() >= 12:
            params = {
                'action': 'undelete',
                'title': self.title(),
                'reason': comment,
                'token': token,
            }
            if self._deletedRevs and self._deletedRevsModified:
                selected = []

                for ts in self._deletedRevs:
                    if self._deletedRevs[ts][4]:
                        selected.append(ts)
                params['timestamps'] = ts,

            result = query.GetData(params, self.site(), sysop=True)
            if 'error' in result:
                raise RuntimeError("%s" % result['error'])
            elif 'undelete' in result:
                output(u'Page %s undeleted' % self.aslink())

            return result

        else:
            address = self.site().undelete_address()

            formdata = {
                'target': self.title(),
                'wpComment': comment,
                'wpEditToken': token,
                'restore': self.site().mediawiki_message('undeletebtn')
            }

            if self._deletedRevs and self._deletedRevsModified:
                for ts in self._deletedRevs:
                    if self._deletedRevs[ts][4]:
                        formdata['ts'+ts] = '1'

            self._deletedRevs = None
            #TODO: Check for errors below (have we succeeded? etc):
            result = self.site().postForm(address,formdata,sysop=True)
            output(u'Page %s undeleted' % self.aslink())

            return result

    def protect(self, editcreate = 'sysop', move = 'sysop', unprotect = False, reason = None, editcreate_duration = 'infinite',
                move_duration = 'infinite', cascading = False, prompt = True, throttle = True):
        """(Un)protect a wiki title. Requires administrator status.

        If the title is not exist, the protection only ec (aka edit/create) available
        If reason is None,  asks for a reason. If prompt is True, asks the
        user if he wants to protect the page. Valid values for ec and move
        are:
           * '' (equivalent to 'none')
           * 'autoconfirmed'
           * 'sysop'

        """
        # Login
        self._getActionUser(action = 'protect', sysop = True)

        # Check blocks
        self.site().checkBlocks(sysop = True)

        #if self.exists() and editcreate != move: # check protect level if edit/move not same
        #    if editcreate == 'sysop' and move != 'sysop':
        #        raise Error("The level configuration is not safe")

        if unprotect:
            editcreate = move = ''
        else:
            editcreate, move = editcreate.lower(), move.lower()
        if throttle:
            put_throttle()
        if reason is None:
            reason = input(
              u'Please enter a reason for the change of the protection level:')
        reason = reason.encode(self.site().encoding())
        answer = 'y'
        if prompt and not hasattr(self.site(), '_noProtectPrompt'):
            answer = inputChoice(
                u'Do you want to change the protection level of %s?'
                    % self.aslink(forceInterwiki = True),
                ['Yes', 'No', 'All'], ['Y', 'N', 'A'], 'N')
            if answer == 'a':
                answer = 'y'
                self.site()._noProtectPrompt = True
        if answer == 'y':
            if not self.site().has_api() or self.site().versionnumber() < 12:
                return self._oldProtect(editcreate, move, unprotect, reason,
                                        editcreate_duration, move_duration,
                                        cascading, prompt, throttle)

            token = self.site().getToken(self, sysop = True)

            # Translate 'none' to ''
            protections = []
            expiry = []
            if editcreate == 'none':
                editcreate = 'all'
            if move == 'none':
                move = 'all'

            if editcreate_duration == 'none' or not editcreate_duration:
                editcreate_duration = 'infinite'
            if move_duration == 'none' or not move_duration:
                move_duration = 'infinite'

            if self.exists():
                protections.append("edit=%s" % editcreate)

                protections.append("move=%s" % move)
                expiry.append(move_duration)
            else:
                protections.append("create=%s" % editcreate)

            expiry.append(editcreate_duration)

            params = {
                'action': 'protect',
                'title': self.title(),
                'token': token,
                'protections': protections,
                'expiry': expiry,
                #'': '',
            }
            if reason:
                params['reason'] = reason

            if cascading:
                if editcreate != 'sysop' or move != 'sysop' or not self.exists():
                    # You can't protect a page as autoconfirmed and cascading, prevent the error
                    # Cascade only available exists page, create prot. not.
                    output(u"NOTE: The page can't be protected with cascading and not also with only-sysop. Set cascading \"off\"")
                else:
                    params['cascade'] = 1

            result = query.GetData(params, self.site(), sysop=True)

            if 'error' in result: #error occured
                err = result['error']['code']
                output('%s' % result)
                #if err == '':
                #
                #elif err == '':
                #
            else:
                if result['protect']:
                    output(u'Changed protection level of page %s.' % self.aslink())
                    return True

        return False

    def _oldProtect(self, editcreate = 'sysop', move = 'sysop', unprotect = False, reason = None, editcreate_duration = 'infinite',
                move_duration = 'infinite', cascading = False, prompt = True, throttle = True):
        """internal use for protect page by ordinary web page form"""
        host = self.site().hostname()
        token = self.site().getToken(sysop = True)

        # Translate 'none' to ''
        if editcreate == 'none': editcreate = ''
        if move == 'none': move = ''

        # Translate no duration to infinite
        if editcreate_duration == 'none' or not editcreate_duration: editcreate_duration = 'infinite'
        if move_duration == 'none' or not move_duration: move_duration = 'infinite'

        # Get cascading
        if cascading == False:
            cascading = '0'
        else:
            if editcreate != 'sysop' or move != 'sysop' or not self.exists():
                # You can't protect a page as autoconfirmed and cascading, prevent the error
                # Cascade only available exists page, create prot. not.
                cascading = '0'
                output(u"NOTE: The page can't be protected with cascading and not also with only-sysop. Set cascading \"off\"")
            else:
                cascading = '1'

        if unprotect:
            address = self.site().unprotect_address(self.urlname())
        else:
            address = self.site().protect_address(self.urlname())

        predata = {}
        if self.site().versionnumber >= 10:
            predata['mwProtect-cascade'] = cascading

        predata['mwProtect-reason'] = reason

        if not self.exists(): #and self.site().versionnumber() >= :
            #create protect
            predata['mwProtect-level-create'] = editcreate
            predata['wpProtectExpirySelection-create'] = editcreate_duration
        else:
            #edit/move Protect
            predata['mwProtect-level-edit'] = editcreate
            predata['mwProtect-level-move'] = move

            if self.site().versionnumber() >= 14:
                predata['wpProtectExpirySelection-edit'] = editcreate_duration
                predata['wpProtectExpirySelection-move'] = move_duration
            else:
                predata['mwProtect-expiry'] = editcreate_duration

        if token:
            predata['wpEditToken'] = token

        response, data = self.site().postForm(address, predata, sysop=True)

        if response.code == 302 and not data:
            output(u'Changed protection level of page %s.' % self.aslink())
            return True
        else:
            #Normally, we expect a 302 with no data, so this means an error
            self.site().checkBlocks(sysop = True)
            output(u'Failed to change protection level of page %s:'
                   % self.aslink())
            output(u"HTTP response code %s" % response.code)
            output(data)
            return False

    def removeImage(self, image, put=False, summary=None, safe=True):
        """Remove all occurrences of an image from this Page."""
        # TODO: this should be grouped with other functions that operate on
        # wiki-text rather than the Page object
        return self.replaceImage(image, None, put, summary, safe)

    def replaceImage(self, image, replacement=None, put=False, summary=None,
                     safe=True):
        """Replace all occurences of an image by another image.

        Giving None as argument for replacement will delink instead of
        replace.

        The argument image must be without namespace and all spaces replaced
        by underscores.

        If put is False, the new text will be returned.  If put is True, the
        edits will be saved to the wiki and True will be returned on succes,
        and otherwise False. Edit errors propagate.

        """
        # TODO: this should be grouped with other functions that operate on
        # wiki-text rather than the Page object

        # Copyright (c) Orgullomoore, Bryan

        # TODO: document and simplify the code
        site = self.site()

        text = self.get()
        new_text = text

        def capitalizationPattern(s):
            """
            Given a string, creates a pattern that matches the string, with
            the first letter case-insensitive if capitalization is switched
            on on the site you're working on.
            """
            if self.site().nocapitalize:
                return re.escape(s)
            else:
                return ur'(?:[%s%s]%s)' % (re.escape(s[0].upper()), re.escape(s[0].lower()), re.escape(s[1:]))

        namespaces = set(site.namespace(6, all = True) + site.namespace(-2, all = True))
        # note that the colon is already included here
        namespacePattern = ur'\s*(?:%s)\s*\:\s*' % u'|'.join(namespaces)

        imagePattern = u'(%s)' % capitalizationPattern(image).replace(r'\_', '[ _]')

        def filename_replacer(match):
            if replacement is None:
                return u''
            else:
                old = match.group()
                return old[:match.start('filename')] + replacement + old[match.end('filename'):]

        # The group params contains parameters such as thumb and 200px, as well
        # as the image caption. The caption can contain wiki links, but each
        # link has to be closed properly.
        paramPattern = r'(?:\|(?:(?!\[\[).|\[\[.*?\]\])*?)'
        rImage = re.compile(ur'\[\[(?P<namespace>%s)(?P<filename>%s)(?P<params>%s*?)\]\]' % (namespacePattern, imagePattern, paramPattern))
        if replacement is None:
            new_text = rImage.sub('', new_text)
        else:
            new_text = rImage.sub('[[\g<namespace>%s\g<params>]]' % replacement, new_text)

        # Remove the image from galleries
        galleryR = re.compile(r'(?is)<gallery>(?P<items>.*?)</gallery>')
        galleryItemR = re.compile(r'(?m)^%s?(?P<filename>%s)\s*(?P<label>\|.*?)?\s*$' % (namespacePattern, imagePattern))

        def gallery_replacer(match):
            return ur'<gallery>%s</gallery>' % galleryItemR.sub(filename_replacer, match.group('items'))

        new_text = galleryR.sub(gallery_replacer, new_text)

        if (text == new_text) or (not safe):
            # All previous steps did not work, so the image is
            # likely embedded in a complicated template.
            # Note: this regular expression can't handle nested templates.
            templateR = re.compile(ur'(?s)\{\{(?P<contents>.*?)\}\}')
            fileReferenceR = re.compile(u'%s(?P<filename>(?:%s)?)' % (namespacePattern, imagePattern))

            def template_replacer(match):
                return fileReferenceR.sub(filename_replacer, match.group(0))

            new_text = templateR.sub(template_replacer, new_text)

        if put:
            if text != new_text:
                # Save to the wiki
                self.put(new_text, summary)
                return True
            return False
        else:
            return new_text

    def getLatestEditors(self, limit = 1):
        """ Function to get the last editors of a page """
        #action=query&prop=revisions&titles=API&rvprop=timestamp|user|comment
        if hasattr(self, '_versionhistory'):
            data = self.getVersionHistory(getAll=True, revCount = limit)
        else:
            data = self.getVersionHistory(revCount = limit)

        result = []
        for i in data:
            result.append({'user':i[2],'timestamp':i[1]})

        return result

class ImagePage(Page):
    """A subclass of Page representing an image descriptor wiki page.

    Supports the same interface as Page, with the following added methods:

    getImagePageHtml          : Download image page and return raw HTML text.
    fileURL                   : Return the URL for the image described on this
                                page.
    fileIsOnCommons           : Return True if image stored on Wikimedia
                                Commons.
    fileIsShared              : Return True if image stored on Wikitravel
                                shared repository.
    getFileMd5Sum             : Return image file's MD5 checksum.
    getFileVersionHistory     : Return the image file's version history.
    getFileVersionHistoryTable: Return the version history in the form of a
                                wiki table.
    usingPages                : Yield Pages on which the image is displayed.

    """
    def __init__(self, site, title, insite = None):
        Page.__init__(self, site, title, insite, defaultNamespace=6)
        if self.namespace() != 6:
            raise ValueError(u'BUG: %s is not in the image namespace!' % title)
        self._imagePageHtml = None
        self._local = None
        self._latestInfo = {}
        self._infoLoaded = False

    def getImagePageHtml(self):
        """
        Download the image page, and return the HTML, as a unicode string.

        Caches the HTML code, so that if you run this method twice on the
        same ImagePage object, the page will only be downloaded once.
        """
        if not self._imagePageHtml:
            path = self.site().get_address(self.urlname())
            self._imagePageHtml = self.site().getUrl(path)
        return self._imagePageHtml

    def _loadInfo(self, limit=1):
        params = {
            'action': 'query',
            'prop': 'imageinfo',
            'titles': self.title(),
            'iiprop': ['timestamp', 'user', 'comment', 'url', 'size',
                       'dimensions', 'sha1', 'mime', 'metadata', 'archivename',
                       'bitdepth'],
            'iilimit': limit,
        }
        try:
            data = query.GetData(params, self.site())
        except NotImplementedError:
            output("API not work, loading page HTML.")
            self.getImagePageHtml()
            return

        if 'error' in data:
            raise RuntimeError("%s" %data['error'])
        count = 0
        pageInfo = data['query']['pages'].values()[0]
        self._local = pageInfo["imagerepository"] != "shared"
        if data['query']['pages'].keys()[0] == "-1":
            if 'missing' in pageInfo and self._local:
                raise NoPage(self.site(), self.aslink(forceInterwiki=True),
                             "Page does not exist.")
            elif 'invalid' in pageInfo:
                raise BadTitle('BadTitle: %s' % self)
        infos = []

        try:
            while True:
                for info in pageInfo['imageinfo']:
                    count += 1
                    if count == 1 and 'iistart' not in params:
                    # count 1 and no iicontinue mean first image revision is latest.
                        self._latestInfo = info
                    infos.append(info)
                    if limit == 1:
                        break

                if 'query-continue' in data and limit != 1:
                    params['iistart'] = data['query-continue']['imageinfo']['iistart']
                else:
                    break
        except KeyError:
            output("Not image in imagepage")
        self._infoLoaded = True
        if limit > 1:
            return infos

    def fileUrl(self):
        """Return the URL for the image described on this page."""
        # There are three types of image pages:
        # * normal, small images with links like: filename.png (10KB, MIME type: image/png)
        # * normal, large images with links like: Download high resolution version (1024x768, 200 KB)
        # * SVG images with links like: filename.svg (1KB, MIME type: image/svg)
        # This regular expression seems to work with all of them.
        # The part after the | is required for copying .ogg files from en:, as they do not
        # have a "full image link" div. This might change in the future; on commons, there
        # is a full image link for .ogg and .mid files.
        #***********************
        #change to API query: action=query&titles=File:wiki.jpg&prop=imageinfo&iiprop=url
        if not self._infoLoaded:
            self._loadInfo()

        if self._infoLoaded:
            return self._latestInfo['url']

        urlR = re.compile(r'<div class="fullImageLink" id="file">.*?<a href="(?P<url>[^ ]+?)"(?! class="image")|<span class="dangerousLink"><a href="(?P<url2>.+?)"', re.DOTALL)
        m = urlR.search(self.getImagePageHtml())

        url = m.group('url') or m.group('url2')
        return url

    def fileIsOnCommons(self):
        """Return True if the image is stored on Wikimedia Commons"""
        if not self._infoLoaded:
            self._loadInfo()

        if self._infoLoaded:
            return not self._local

        return self.fileUrl().startswith(u'http://upload.wikimedia.org/wikipedia/commons/')

    def fileIsShared(self):
        """Return True if image is stored on Wikitravel shared repository."""
        if 'wikitravel_shared' in self.site().shared_image_repository():
            return self.fileUrl().startswith(u'http://wikitravel.org/upload/shared/')
        return self.fileIsOnCommons()

    # FIXME: MD5 might be performed on not complete file due to server disconnection
    # (see bug #1795683).
    def getFileMd5Sum(self):
        """Return image file's MD5 checksum."""
        f = MyURLopener.open(self.fileUrl())
        return md5(f.read()).hexdigest()

    def getFileVersionHistory(self):
        """Return the image file's version history.

        Return value is a list of tuples containing (timestamp, username,
        resolution, filesize, comment).

        """
        result = []
        infos = self._loadInfo(500)
        #API query
        if infos:
            for i in infos:
                result.append((i['timestamp'], i['user'], u"%s×%s" % (i['width'], i['height']), i['size'], i['comment']))

            return result

        #from ImagePage HTML
        history = re.search('(?s)<table class="wikitable filehistory">.+?</table>', self.getImagePageHtml())
        if history:
            lineR = re.compile(r'<tr>(?:<td>.*?</td>){1,2}<td.*?><a href=".+?">(?P<datetime>.+?)</a></td><td>.*?(?P<resolution>\d+\xd7\d+) <span.*?>\((?P<filesize>.+?)\)</span></td><td><a href=".+?"(?: class="new"|) title=".+?">(?P<username>.+?)</a>.*?</td><td>(?:.*?<span class="comment">\((?P<comment>.*?)\)</span>)?</td></tr>')
            if not lineR.search(history.group()):
                # b/c code
                lineR = re.compile(r'<tr>(?:<td>.*?</td>){1,2}<td><a href=".+?">(?P<datetime>.+?)</a></td><td><a href=".+?"(?: class="new"|) title=".+?">(?P<username>.+?)</a>.*?</td><td>(?P<resolution>.*?)</td><td class=".+?">(?P<filesize>.+?)</td><td>(?P<comment>.*?)</td></tr>')
        else:
            # backward compatible code
            history = re.search('(?s)<ul class="special">.+?</ul>', self.getImagePageHtml())
            if history:
                lineR = re.compile('<li> \(.+?\) \(.+?\) <a href=".+?" title=".+?">(?P<datetime>.+?)</a> . . <a href=".+?" title=".+?">(?P<username>.+?)</a> \(.+?\) . . (?P<resolution>\d+.+?\d+) \((?P<filesize>[\d,\.]+) .+?\)( <span class="comment">(?P<comment>.*?)</span>)?</li>')

        if history:
            for match in lineR.finditer(history.group()):
                datetime = match.group('datetime')
                username = match.group('username')
                resolution = match.group('resolution')
                size = match.group('filesize')
                comment = match.group('comment') or ''
                result.append((datetime, username, resolution, size, comment))
        return result

    def getLatestUploader(self):
        """ Function that uses the APIs to detect the latest uploader of the image """
        if not self._infoLoaded:
            self._loadInfo()
        if self._infoLoaded:
            return [self._latestInfo['user'], self._latestInfo['timestamp']]

        inf = self.getFileVersionHistory()[0]
        return [inf[1], inf[0]]

    def getHash(self):
        """ Function that return the Hash of an file in oder to understand if two
            Files are the same or not.
            """
        if self.exists():
            if not self._infoLoaded:
                self._loadInfo()
            try:
                return self._latestInfo['sha1']
            except (KeyError, IndexError, TypeError):
                try:
                    self.get()
                except NoPage:
                    output(u'%s has been deleted before getting the Hash. Skipping...' % self.title())
                    return None
                except IsRedirectPage:
                    output("Skipping %s because it's a redirect." % self.title())
                    return None
                else:
                    raise NoHash('No Hash found in the APIs! Maybe the regex to catch it is wrong or someone has changed the APIs structure.')
        else:
            output(u'File deleted before getting the Hash. Skipping...')
            return None

    def getFileVersionHistoryTable(self):
        """Return the version history in the form of a wiki table."""
        lines = []
        for (datetime, username, resolution, size, comment) in self.getFileVersionHistory():
            lines.append(u'| %s || %s || %s || %s || <nowiki>%s</nowiki>' % (datetime, username, resolution, size, comment))
        return u'{| border="1"\n! date/time || username || resolution || size || edit summary\n|----\n' + u'\n|----\n'.join(lines) + '\n|}'

    def usingPages(self):
        if not self.site().has_api() or self.site().versionnumber() < 11:
            for a in self._usingPagesOld():
                yield a
            return

        params = {
            'action': 'query',
            'list': 'imageusage',
            'iutitle': self.title(),
            'iulimit': config.special_page_limit,
            #'': '',
        }

        while True:
            data = query.GetData(params, self.site())
            if 'error' in data:
                raise RuntimeError("%s" % data['error'])

            for iu in data['query']["imageusage"]:
                yield Page(self.site(), iu['title'], defaultNamespace=iu['ns'])

            if 'query-continue' in data:
                params['iucontinue'] = data['query-continue']['imageusage']['iucontinue']
            else:
                break

    def _usingPagesOld(self):
        """Yield Pages on which the image is displayed."""
        titleList = re.search('(?s)<h2 id="filelinks">.+?<!-- end content -->',
                              self.getImagePageHtml()).group()
        lineR = re.compile(
                    '<li><a href="[^\"]+" title=".+?">(?P<title>.+?)</a></li>')

        for match in lineR.finditer(titleList):
            try:
                yield Page(self.site(), match.group('title'))
            except InvalidTitle:
                output(
        u"Image description page %s contains invalid reference to [[%s]]."
                    % (self.title(), match.group('title')))


class _GetAll(object):
    """For internal use only - supports getall() function"""
    def __init__(self, site, pages, throttle, force):
        self.site = site
        self.pages = []
        self.throttle = throttle
        self.force = force
        self.sleeptime = 15

        for page in pages:
            if (not hasattr(page, '_contents') and not hasattr(page, '_getexception')) or force:
                self.pages.append(page)
            elif verbose:
                output(u"BUGWARNING: %s already done!" % page.aslink())

    def sleep(self):
        time.sleep(self.sleeptime)
        if self.sleeptime <= 60:
            self.sleeptime += 15
        elif self.sleeptime < 360:
            self.sleeptime += 60

    def run(self):
        if self.pages:
            # Sometimes query does not contains revisions
            if  self.site.has_api() and debug:
                while True:
                    try:
                        data = self.getDataApi()
                    except (socket.error, httplib.BadStatusLine, ServerError):
                        # Print the traceback of the caught exception
                        s = ''.join(traceback.format_exception(*sys.exc_info()))
                        if not isinstance(s, unicode):
                            s = s.decode('utf-8')
                        output(u'%s\nDBG> got network error in _GetAll.run. ' \
                                'Sleeping for %d seconds...' % (s, self.sleeptime))
                        self.sleep()
                    else:
                        if 'error' in data:
                            raise RuntimeError(data['error'])
                        else:
                            break

                self.headerDoneApi(data['query'])
                if 'normalized' in data['query']:
                    self._norm = dict([(x['from'],x['to']) for x in data['query']['normalized']])
                for vals in data['query']['pages'].values():
                    self.oneDoneApi(vals)
            else:
                while True:
                    try:
                        data = self.getData()
                    except (socket.error, httplib.BadStatusLine, ServerError):
                        # Print the traceback of the caught exception
                        s = ''.join(traceback.format_exception(*sys.exc_info()))
                        if not isinstance(s, unicode):
                            s = s.decode('utf-8')
                        output(u'%s\nDBG> got network error in _GetAll.run. ' \
                                'Sleeping for %d seconds...' % (s, self.sleeptime))
                        self.sleep()
                    else:
                        if "<title>Wiki does not exist</title>" in data:
                            raise NoSuchSite(u'Wiki %s does not exist yet' % self.site)
                        elif "</mediawiki>" not in data[-20:]:
                            # HTML error Page got thrown because of an internal
                            # error when fetching a revision.
                            output(u'Received incomplete XML data. ' \
                                'Sleeping for %d seconds...' % self.sleeptime)
                            self.sleep()
                        elif "<siteinfo>" not in data: # This probably means we got a 'temporary unaivalable'
                            output(u'Got incorrect export page. ' \
                                'Sleeping for %d seconds...' % self.sleeptime)
                            self.sleep()
                        else:
                            break
                R = re.compile(r"\s*<\?xml([^>]*)\?>(.*)",re.DOTALL)
                m = R.match(data)
                if m:
                    data = m.group(2)
                handler = xmlreader.MediaWikiXmlHandler()
                handler.setCallback(self.oneDone)
                handler.setHeaderCallback(self.headerDone)
                #f = open("backup.txt", "w")
                #f.write(data)
                #f.close()
                try:
                    xml.sax.parseString(data, handler)
                except (xml.sax._exceptions.SAXParseException, ValueError), err:
                    debugDump( 'SaxParseBug', self.site, err, data )
                    raise
                except PageNotFound:
                    return
                # All of the ones that have not been found apparently do not exist

            for pl in self.pages:
                if not hasattr(pl,'_contents') and not hasattr(pl,'_getexception'):
                    pl._getexception = NoPage

    def oneDone(self, entry):
        title = entry.title
        username = entry.username
        ipedit = entry.ipedit
        timestamp = entry.timestamp
        text = entry.text
        editRestriction = entry.editRestriction
        moveRestriction = entry.moveRestriction
        revisionId = entry.revisionid

        page = Page(self.site, title)
        successful = False
        for page2 in self.pages:
            if page2.sectionFreeTitle() == page.sectionFreeTitle():
                if not (hasattr(page2,'_contents') or \
                        hasattr(page2, '_getexception')) or self.force:
                    page2.editRestriction = entry.editRestriction
                    page2.moveRestriction = entry.moveRestriction
                    if editRestriction == 'autoconfirmed':
                        page2._editrestriction = True
                    page2._permalink = entry.revisionid
                    page2._userName = username
                    page2._ipedit = ipedit
                    page2._revisionId = revisionId
                    page2._editTime = timestamp
                    page2._versionhistory = [
                        (revisionId,
                         time.strftime("%Y-%m-%dT%H:%M:%SZ",
                                       time.strptime(str(timestamp),
                                                     "%Y%m%d%H%M%S")),
                         username, entry.comment)]
                    section = page2.section()
                    # Store the content
                    page2._contents = text
                    m = self.site.redirectRegex().match(text)
                    if m:
                        ## output(u"%s is a redirect" % page2.aslink())
                        redirectto = m.group(1)
                        if section and not "#" in redirectto:
                            redirectto += "#" + section
                        page2._getexception = IsRedirectPage
                        page2._redirarg = redirectto

                    # This is used for checking deletion conflict.
                    # Use the data loading time.
                    page2._startTime = time.strftime('%Y%m%d%H%M%S',
                                                     time.gmtime())
                    if section:
                        m = re.search("\.3D\_*(\.27\.27+)?(\.5B\.5B)?\_*%s\_*(\.5B\.5B)?(\.27\.27+)?\_*\.3D"
                                      % re.escape(section), sectionencode(text,page2.site().encoding()))
                        if not m:
                            try:
                                page2._getexception
                                output(u"WARNING: Section not found: %s"
                                       % page2.aslink(forceInterwiki = True))
                            except AttributeError:
                                # There is no exception yet
                                page2._getexception = SectionError
                successful = True
                # Note that there is no break here. The reason is that there
                # might be duplicates in the pages list.
        if not successful:
            output(u"BUG>> title %s (%s) not found in list"
                   % (title, page.aslink(forceInterwiki=True)))
            output(u'Expected one of: %s'
                   % u','.join([page2.aslink(forceInterwiki=True) for page2 in self.pages]))
            raise PageNotFound

    def headerDone(self, header):
        # Verify version
        version = header.generator
        p = re.compile('^MediaWiki (.+)$')
        m = p.match(version)
        if m:
            version = m.group(1)
            if version != self.site.version():
                output(u'WARNING: Family file %s contains version number %s, but it should be %s' % (self.site.family.name, self.site.version(), version))

        # Verify case
        if self.site.nocapitalize:
            case = 'case-sensitive'
        else:
            case = 'first-letter'
        if case != header.case.strip():
            output(u'WARNING: Family file %s contains case %s, but it should be %s' % (self.site.family.name, case, header.case.strip()))

        # Verify namespaces
        lang = self.site.lang
        ids = header.namespaces.keys()
        ids.sort()
        for id in ids:
            nshdr = header.namespaces[id]
            if self.site.family.isDefinedNSLanguage(id, lang):
                ns = self.site.namespace(id) or u''
                if ns != nshdr:
                    try:
                        dflt = self.site.family.namespace('_default', id)
                    except KeyError:
                        dflt = u''
                    if not ns and not dflt:
                        flag = u"is not set, but should be '%s'" % nshdr
                    elif dflt == ns:
                        flag = u"is set to default ('%s'), but should be '%s'" % (ns, nshdr)
                    elif dflt == nshdr:
                        flag = u"is '%s', but should be removed (default value '%s')" % (ns, nshdr)
                    else:
                        flag = u"is '%s', but should be '%s'" % (ns, nshdr)
                    output(u"WARNING: Outdated family file %s: namespace['%s'][%i] %s" % (self.site.family.name, lang, id, flag))
                    #self.site.family.namespaces[id][lang] = nshdr
            else:
                output(u"WARNING: Missing namespace in family file %s: namespace['%s'][%i] (it is set to '%s')" % (self.site.family.name, lang, id, nshdr))
        for id in self.site.family.namespaces:
            if self.site.family.isDefinedNSLanguage(id, lang) and id not in header.namespaces:
                output(u"WARNING: Family file %s includes namespace['%s'][%i], but it should be removed (namespace doesn't exist in the site)" % (self.site.family.name, lang, id))

    def getData(self):
        address = self.site.export_address()
        pagenames = [page.sectionFreeTitle() for page in self.pages]
        # We need to use X convention for requested page titles.
        if self.site.lang == 'eo':
            pagenames = [encodeEsperantoX(pagetitle) for pagetitle in pagenames]
        pagenames = u'\r\n'.join(pagenames)
        if type(pagenames) is not unicode:
            output(u'Warning: xmlreader.WikipediaXMLHandler.getData() got non-unicode page names. Please report this.')
            print pagenames
        # convert Unicode string to the encoding used on that wiki
        pagenames = pagenames.encode(self.site.encoding())
        predata = {
            'action': 'submit',
            'pages': pagenames,
            'curonly': 'True',
        }
        # Slow ourselves down
        get_throttle(requestsize = len(self.pages))
        # Now make the actual request to the server
        now = time.time()
        response, data = self.site.postForm(address, predata)
        # The XML parser doesn't expect a Unicode string, but an encoded one,
        # so we'll encode it back.
        data = data.encode(self.site.encoding())
        #get_throttle.setDelay(time.time() - now)
        return data

    def oneDoneApi(self, data):
        title = data['title']
        if not ('missing' in data or 'invalid' in data):
            revisionId = data['lastrevid']
            rev = None
            try:
                rev = data['revisions']
            except KeyError:
                raise u'NOTE: Last revision of [[%s]] not found' % title
            else:
                username = rev[0]['user']
                ipedit = 'anon' in rev[0]
                timestamp = rev[0]['timestamp']
                text = rev[0]['*']
            editRestriction = ''
            moveRestriction = ''
            for revs in data['protection']:
                if revs['type'] == 'edit':
                    editRestriction = revs['level']
                elif revs['type'] == 'move':
                    moveRestriction = revs['level']

        page = Page(self.site, title)
        successful = False
        for page2 in self.pages:
            if hasattr(self, '_norm') and page2.sectionFreeTitle() in self._norm:
                page2._title = self._norm[page2.sectionFreeTitle()]

            if page2.sectionFreeTitle() == page.sectionFreeTitle():
                if 'missing' in data:
                    page2._getexception = NoPage
                    successful = True
                    break

                if 'invalid' in data:
                    page2._getexception = BadTitle
                    successful = True
                    break

                if not (hasattr(page2,'_contents') or hasattr(page2,'_getexception')) or self.force:
                    page2.editRestriction = editRestriction
                    page2.moveRestriction = moveRestriction
                    if editRestriction == 'autoconfirmed':
                        page2._editrestriction = True
                    page2._permalink = revisionId
                    if rev:
                        page2._userName = username
                        page2._ipedit = ipedit
                        page2._editTime = timestamp
                        page2._contents = text
                    else:
                        raise u'BUG?>>: Last revision of [[%s]] not found' % title
                    page2._revisionId = revisionId
                    section = page2.section()
                    if 'redirect' in data:
                        ## output(u"%s is a redirect" % page2.aslink())
                        m = self.site.redirectRegex().match(text)
                        redirectto = m.group(1)
                        if section and not "#" in redirectto:
                            redirectto += "#" + section
                        page2._getexception = IsRedirectPage
                        page2._redirarg = redirectto

                    # This is used for checking deletion conflict.
                    # Use the data loading time.
                    page2._startTime = time.strftime('%Y%m%d%H%M%S', time.gmtime())
                    if section:
                        m = re.search("\.3D\_*(\.27\.27+)?(\.5B\.5B)?\_*%s\_*(\.5B\.5B)?(\.27\.27+)?\_*\.3D" % re.escape(section), sectionencode(text,page2.site().encoding()))
                        if not m:
                            try:
                                page2._getexception
                                output(u"WARNING: Section not found: %s" % page2.aslink(forceInterwiki = True))
                            except AttributeError:
                                # There is no exception yet
                                page2._getexception = SectionError
                successful = True
                # Note that there is no break here. The reason is that there
                # might be duplicates in the pages list.
        if not successful:
            output(u"BUG>> title %s (%s) not found in list" % (title, page.aslink(forceInterwiki=True)))
            output(u'Expected one of: %s' % u','.join([page2.aslink(forceInterwiki=True) for page2 in self.pages]))
            raise PageNotFound

    def headerDoneApi(self, header):
        p = re.compile('^MediaWiki (.+)$')
        m = p.match(header['general']['generator'])
        if m:
            version = m.group(1)
            if version != self.site.version():
                output(u'WARNING: Family file %s contains version number %s, but it should be %s' % (self.site.family.name, self.site.version(), version))

        # Verify case
        if self.site.nocapitalize:
            case = 'case-sensitive'
        else:
            case = 'first-letter'
        if case != header['general']['case'].strip():
            output(u'WARNING: Family file %s contains case %s, but it should be %s' % (self.site.family.name, case, header.case.strip()))

        # Verify namespaces
        lang = self.site.lang
        ids = header['namespaces'].keys()
        ids.sort()
        for id in ids:
            nshdr = header['namespaces'][id]['*']
            id = header['namespaces'][id]['id']
            if self.site.family.isDefinedNSLanguage(id, lang):
                ns = self.site.namespace(id) or u''
                if ns != nshdr:
                    try:
                        dflt = self.site.family.namespace('_default', id)
                    except KeyError:
                        dflt = u''
                    if not ns and not dflt:
                        flag = u"is not set, but should be '%s'" % nshdr
                    elif dflt == ns:
                        flag = u"is set to default ('%s'), but should be '%s'" % (ns, nshdr)
                    elif dflt == nshdr:
                        flag = u"is '%s', but should be removed (default value '%s')" % (ns, nshdr)
                    else:
                        flag = u"is '%s', but should be '%s'" % (ns, nshdr)
                    output(u"WARNING: Outdated family file %s: namespace['%s'][%i] %s" % (self.site.family.name, lang, id, flag))
                    #self.site.family.namespaces[id][lang] = nshdr
            else:
                output(u"WARNING: Missing namespace in family file %s: namespace['%s'][%i] (it is set to '%s')" % (self.site.family.name, lang, id, nshdr))
        for id in self.site.family.namespaces:
            if self.site.family.isDefinedNSLanguage(id, lang) and u'%i' % id not in header['namespaces']:
                output(u"WARNING: Family file %s includes namespace['%s'][%i], but it should be removed (namespace doesn't exist in the site)" % (self.site.family.name, lang, id ) )

    def getDataApi(self):
        pagenames = [page.sectionFreeTitle() for page in self.pages]
        params = {
            'action': 'query',
            'meta':'siteinfo',
            'prop': ['info', 'revisions'],
            'titles': pagenames,
            'siprop': ['general', 'namespaces'],
            'rvprop': ['content', 'timestamp', 'user', 'comment', 'size'],#'ids',
            'inprop': ['protection', 'subjectid'], #, 'talkid', 'url', 'readable'
        }

        # Slow ourselves down
        get_throttle(requestsize = len(self.pages))
        # Now make the actual request to the server
        now = time.time()

        #get_throttle.setDelay(time.time() - now)
        return query.GetData(params, self.site)

def getall(site, pages, throttle=True, force=False):
    """Use Special:Export to bulk-retrieve a group of pages from site

    Arguments: site = Site object
               pages = iterable that yields Page objects

    """
    # TODO: why isn't this a Site method?
    pages = list(pages)  # if pages is an iterator, we need to make it a list
    output(u'Getting %d pages %sfrom %s...'
           % (len(pages), iif(site.has_api() and debug, u'via API ', u''), site))
    limit = config.special_page_limit / 4 # default is 500/4, but It might have good point for server.
    if len(pages) > limit:
        # separate export pages for bulk-retrieve

        for pagg in range(0, len(pages), limit):
            if pagg == range(0, len(pages), limit)[-1]: #latest retrieve
                k = pages[pagg:]
                output(u'Getting pages %d - %d of %d...' % (pagg + 1, len(pages), len(pages)))
                _GetAll(site, k, throttle, force).run()
                pages[pagg:] = k
            else:
                k = pages[pagg:pagg + limit]
                output(u'Getting pages %d - %d of %d...' % (pagg + 1, pagg + limit, len(pages)))
                _GetAll(site, k, throttle, force).run()
                pages[pagg:pagg + limit] = k
            get_throttle(requestsize = len(pages) / 10) # one time to retrieve is 7.7 sec.
    else:
        _GetAll(site, pages, throttle, force).run()


# Library functions

def setAction(s):
    """Set a summary to use for changed page submissions"""
    global action
    action = s

def iif(q, a, b):
    """inline if"""
    if q: return a
    else: return b

# Default action
setAction('Wikipedia python library')

def setUserAgent(s):
    """Set a User-agent: header passed to the HTTP server"""
    global useragent
    useragent = s

# Default User-agent
setUserAgent('PythonWikipediaBot/1.0')

def url2link(percentname, insite, site):
    """Convert urlname of a wiki page into interwiki link format.

    'percentname' is the page title as given by Page.urlname();
    'insite' specifies the target Site;
    'site' is the Site on which the page is found.

    """
    # Note: this is only needed if linking between wikis that use different
    # encodings, so it is now largely obsolete.  [CONFIRM]
    percentname = percentname.replace('_', ' ')
    x = url2unicode(percentname, site = site)
    return unicode2html(x, insite.encoding())

def decodeEsperantoX(text):
    """
    Decode Esperanto text encoded using the x convention.

    E.g., Cxefpagxo and CXefpagXo will both be converted to Ĉefpaĝo.
    Note that to encode non-Esperanto words like Bordeaux, one uses a
    double x, i.e. Bordeauxx or BordeauxX.

    """
    chars = {
        u'c': u'ĉ',
        u'C': u'Ĉ',
        u'g': u'ĝ',
        u'G': u'Ĝ',
        u'h': u'ĥ',
        u'H': u'Ĥ',
        u'j': u'ĵ',
        u'J': u'Ĵ',
        u's': u'ŝ',
        u'S': u'Ŝ',
        u'u': u'ŭ',
        u'U': u'Ŭ',
    }
    for latin, esperanto in chars.iteritems():
        # A regular expression that matches a letter combination which IS
        # encoded using x-convention.
        xConvR = re.compile(latin + '[xX]+')
        pos = 0
        result = ''
        # Each matching substring will be regarded exactly once.
        while True:
            match = xConvR.search(text[pos:])
            if match:
                old = match.group()
                if len(old) % 2 == 0:
                    # The first two chars represent an Esperanto letter.
                    # Following x's are doubled.
                    new = esperanto + ''.join([old[2 * i]
                                               for i in range(1, len(old)/2)])
                else:
                    # The first character stays latin; only the x's are doubled.
                    new = latin + ''.join([old[2 * i + 1]
                                           for i in range(0, len(old)/2)])
                result += text[pos : match.start() + pos] + new
                pos += match.start() + len(old)
            else:
                result += text[pos:]
                text = result
                break
    return text

def encodeEsperantoX(text):
    """
    Convert standard wikitext to the Esperanto x-encoding.

    Double X-es where necessary so that we can submit a page to an Esperanto
    wiki. Again, we have to keep stupid stuff like cXxXxxX in mind. Maybe
    someone wants to write about the Sony Cyber-shot DSC-Uxx camera series on
    eo: ;)
    """
    # A regular expression that matches a letter combination which is NOT
    # encoded in x-convention.
    notXConvR = re.compile('[cghjsuCGHJSU][xX]+')
    pos = 0
    result = ''
    while True:
        match = notXConvR.search(text[pos:])
        if match:
            old = match.group()
            # the first letter stays; add an x after each X or x.
            new = old[0] + ''.join([old[i] + 'x' for i in range(1, len(old))])
            result += text[pos : match.start() + pos] + new
            pos += match.start() + len(old)
        else:
            result += text[pos:]
            text = result
            break
    return text

def sectionencode(text, encoding):
    """Encode text so that it can be used as a section title in wiki-links."""
    return urllib.quote(text.replace(" ","_").encode(encoding)).replace("%",".")

######## Unicode library functions ########

def UnicodeToAsciiHtml(s):
    """Convert unicode to a bytestring using HTML entities."""
    html = []
    for c in s:
        cord = ord(c)
        if 31 < cord < 128:
            html.append(c)
        else:
            html.append('&#%d;'%cord)
    return ''.join(html)

def url2unicode(title, site, site2 = None):
    """Convert url-encoded text to unicode using site's encoding.

    If site2 is provided, try its encodings as well.  Uses the first encoding
    that doesn't cause an error.

    """
    # create a list of all possible encodings for both hint sites
    encList = [site.encoding()] + list(site.encodings())
    if site2 and site2 <> site:
        encList.append(site2.encoding())
        encList += list(site2.encodings())
    firstException = None
    # try to handle all encodings (will probably retry utf-8)
    for enc in encList:
        try:
            t = title.encode(enc)
            t = urllib.unquote(t)
            return unicode(t, enc)
        except UnicodeError, ex:
            if not firstException:
                firstException = ex
            pass
    # Couldn't convert, raise the original exception
    raise firstException

def unicode2html(x, encoding):
    """
    Ensure unicode string is encodable, or else convert to ASCII for HTML.

    Arguments are a unicode string and an encoding. Attempt to encode the
    string into the desired format; if that doesn't work, encode the unicode
    into html &#; entities. If it does work, return it unchanged.

    """
    try:
        x.encode(encoding)
    except UnicodeError:
        x = UnicodeToAsciiHtml(x)
    return x

def html2unicode(text, ignore = []):
    """Replace all HTML entities in text by equivalent unicode characters."""
    # This regular expression will match any decimal and hexadecimal entity and
    # also entities that might be named entities.
    entityR = re.compile(r'&(#(?P<decimal>\d+)|#x(?P<hex>[0-9a-fA-F]+)|(?P<name>[A-Za-z]+));')
    #These characters are Html-illegal, but sadly you *can* find some of these and
    #converting them to unichr(decimal) is unsuitable
    convertIllegalHtmlEntities = {
        128 : 8364, # €
        130 : 8218, # ‚
        131 : 402,  # ƒ
        132 : 8222, # „
        133 : 8230, # …
        134 : 8224, # †
        135 : 8225, # ‡
        136 : 710,  # ˆ
        137 : 8240, # ‰
        138 : 352,  # Š
        139 : 8249, # ‹
        140 : 338,  # Œ
        142 : 381,  # Ž
        145 : 8216, # ‘
        146 : 8217, # ’
        147 : 8220, # “
        148 : 8221, # ”
        149 : 8226, # •
        150 : 8211, # –
        151 : 8212, # —
        152 : 732,  # ˜
        153 : 8482, # ™
        154 : 353,  # š
        155 : 8250, # ›
        156 : 339,  # œ
        158 : 382,  # ž
        159 : 376   # Ÿ
    }
    #ensuring that illegal &#129; &#141; and &#157, which have no known values,
    #don't get converted to unichr(129), unichr(141) or unichr(157)
    ignore = set(ignore) | set([129, 141, 157])
    result = u''
    i = 0
    found = True
    while found:
        text = text[i:]
        match = entityR.search(text)
        if match:
            unicodeCodepoint = None
            if match.group('decimal'):
                unicodeCodepoint = int(match.group('decimal'))
            elif match.group('hex'):
                unicodeCodepoint = int(match.group('hex'), 16)
            elif match.group('name'):
                name = match.group('name')
                if name in htmlentitydefs.name2codepoint:
                    # We found a known HTML entity.
                    unicodeCodepoint = htmlentitydefs.name2codepoint[name]
            result += text[:match.start()]
            try:
                unicodeCodepoint=convertIllegalHtmlEntities[unicodeCodepoint]
            except KeyError:
                pass
            if unicodeCodepoint and unicodeCodepoint not in ignore and (WIDEBUILD or unicodeCodepoint < 65534):
                result += unichr(unicodeCodepoint)
            else:
                # Leave the entity unchanged
                result += text[match.start():match.end()]
            i = match.end()
        else:
            result += text
            found = False
    return result

# Warning! _familyCache does not necessarily have to be consistent between
# two statements. Always ensure that a local reference is created when
# accessing Family objects
_familyCache = weakref.WeakValueDictionary()
def Family(fam = None, fatal = True, force = False):
    """
    Import the named family.

    If fatal is True, the bot will stop running when the given family is
    unknown. If fatal is False, it will only raise a ValueError exception.
    """
    if fam is None:
        fam = config.family

    family = _familyCache.get(fam)
    if family and not force:
        return family

    try:
        # search for family module in the 'families' subdirectory
        sys.path.append(config.datafilepath('families'))
        myfamily = __import__('%s_family' % fam)
    except ImportError:
        if fatal:
            output(u"""\
Error importing the %s family. This probably means the family
does not exist. Also check your configuration file."""
                       % fam)
            import traceback
            traceback.print_stack()
            sys.exit(1)
        else:
            raise ValueError("Family %s does not exist" % repr(fam))

    family = myfamily.Family()
    _familyCache[fam] = family
    return family


class Site(object):
    """A MediaWiki site. Do not instantiate directly; use getSite() function.

    Constructor takes four arguments; only code is mandatory:

    code            language code for Site
    fam             Wiki family (optional: defaults to configured).
                    Can either be a string or a Family object.
    user            User to use (optional: defaults to configured)
    persistent_http Use a persistent http connection. An http connection
                    has to be established only once, making stuff a whole
                    lot faster. Do NOT EVER use this if you share Site
                    objects across threads without proper locking.

    Methods:

    language: This Site's language code.
    family: This Site's Family object.
    sitename: A string representing this Site.
    languages: A list of all languages contained in this site's Family.
    validLanguageLinks: A list of language codes that can be used in interwiki
        links.

    loggedInAs: return current username, or None if not logged in.
    forceLogin: require the user to log in to the site
    messages: return True if there are new messages on the site
    cookies: return user's cookies as a string

    getUrl: retrieve an URL from the site
    urlEncode: Encode a query to be sent using an http POST request.
    postForm: Post form data to an address at this site.
    postData: Post encoded form data to an http address at this site.

    namespace(num): Return local name of namespace 'num'.
    normalizeNamespace(value): Return preferred name for namespace 'value' in
        this Site's language.
    namespaces: Return list of canonical namespace names for this Site.
    getNamespaceIndex(name): Return the int index of namespace 'name', or None
        if invalid.

    redirect: Return the localized redirect tag for the site.
    redirectRegex: Return compiled regular expression matching on redirect
                   pages.
    mediawiki_message: Retrieve the text of a specified MediaWiki message
    has_mediawiki_message: True if this site defines specified MediaWiki
                           message
    has_api: True if this site's family provides api interface

    shared_image_repository: Return tuple of image repositories used by this
        site.
    category_on_one_line: Return True if this site wants all category links
        on one line.
    interwiki_putfirst: Return list of language codes for ordering of
        interwiki links.
    linkto(title): Return string in the form of a wikilink to 'title'
    isInterwikiLink(s): Return True if 's' is in the form of an interwiki
                        link.
    getSite(lang): Return Site object for wiki in same family, language
                   'lang'.
    version: Return MediaWiki version string from Family file.
    versionnumber: Return int identifying the MediaWiki version.
    live_version: Return version number read from Special:Version.
    checkCharset(charset): Warn if charset doesn't match family file.
    server_time : returns server time (currently userclock depending)

    linktrail: Return regex for trailing chars displayed as part of a link.
    disambcategory: Category in which disambiguation pages are listed.

    Methods that yield Page objects derived from a wiki's Special: pages
    (note, some methods yield other information in a tuple along with the
    Pages; see method docs for details) --

        search(query): query results from Special:Search
        allpages(): Special:Allpages
        prefixindex(): Special:Prefixindex
        protectedpages(): Special:ProtectedPages
        newpages(): Special:Newpages
        newimages(): Special:Log&type=upload
        longpages(): Special:Longpages
        shortpages(): Special:Shortpages
        categories(): Special:Categories (yields Category objects)
        deadendpages(): Special:Deadendpages
        ancientpages(): Special:Ancientpages
        lonelypages(): Special:Lonelypages
        recentchanges(): Special:Recentchanges
        unwatchedpages(): Special:Unwatchedpages (sysop accounts only)
        uncategorizedcategories(): Special:Uncategorizedcategories (yields
            Category objects)
        uncategorizedpages(): Special:Uncategorizedpages
        uncategorizedimages(): Special:Uncategorizedimages (yields
            ImagePage objects)
        uncategorizedtemplates(): Special:UncategorizedTemplates
        unusedcategories(): Special:Unusuedcategories (yields Category)
        unusedfiles(): Special:Unusedimages (yields ImagePage)
        randompage: Special:Random
        randomredirectpage: Special:RandomRedirect
        withoutinterwiki: Special:Withoutinterwiki
        linksearch: Special:Linksearch

    Convenience methods that provide access to properties of the wiki Family
    object; all of these are read-only and return a unicode string unless
    noted --

        encoding: The current encoding for this site.
        encodings: List of all historical encodings for this site.
        category_namespace: Canonical name of the Category namespace on this
            site.
        category_namespaces: List of all valid names for the Category
            namespace.
        image_namespace: Canonical name of the Image namespace on this site.
        template_namespace: Canonical name of the Template namespace on this
            site.
        protocol: Protocol ('http' or 'https') for access to this site.
        hostname: Host portion of site URL.
        path: URL path for index.php on this Site.
        dbName: MySQL database name.

    Methods that return addresses to pages on this site (usually in
    Special: namespace); these methods only return URL paths, they do not
    interact with the wiki --

        export_address: Special:Export.
        query_address: URL path + '?' for query.php
        api_address: URL path + '?' for api.php
        apipath: URL path for api.php
        move_address: Special:Movepage.
        delete_address(s): Delete title 's'.
        undelete_view_address(s): Special:Undelete for title 's'
        undelete_address: Special:Undelete.
        protect_address(s): Protect title 's'.
        unprotect_address(s): Unprotect title 's'.
        put_address(s): Submit revision to page titled 's'.
        get_address(s): Retrieve page titled 's'.
        nice_get_address(s): Short URL path to retrieve page titled 's'.
        edit_address(s): Edit form for page titled 's'.
        purge_address(s): Purge cache and retrieve page 's'.
        block_address: Block an IP address.
        unblock_address: Unblock an IP address.
        blocksearch_address(s): Search for blocks on IP address 's'.
        linksearch_address(s): Special:Linksearch for target 's'.
        search_address(q): Special:Search for query 'q'.
        allpages_address(s): Special:Allpages.
        newpages_address: Special:Newpages.
        longpages_address: Special:Longpages.
        shortpages_address: Special:Shortpages.
        unusedfiles_address: Special:Unusedimages.
        categories_address: Special:Categories.
        deadendpages_address: Special:Deadendpages.
        ancientpages_address: Special:Ancientpages.
        lonelypages_address: Special:Lonelypages.
        protectedpages_address: Special:ProtectedPages
        unwatchedpages_address: Special:Unwatchedpages.
        uncategorizedcategories_address: Special:Uncategorizedcategories.
        uncategorizedimages_address: Special:Uncategorizedimages.
        uncategorizedpages_address: Special:Uncategorizedpages.
        uncategorizedtemplates_address: Special:UncategorizedTemplates.
        unusedcategories_address: Special:Unusedcategories.
        withoutinterwiki_address: Special:Withoutinterwiki.
        references_address(s): Special:Whatlinksere for page 's'.
        allmessages_address: Special:Allmessages.
        upload_address: Special:Upload.
        double_redirects_address: Special:Doubleredirects.
        broken_redirects_address: Special:Brokenredirects.
        random_address: Special:Random.
        randomredirect_address: Special:Random.
        login_address: Special:Userlogin.
        captcha_image_address(id): Special:Captcha for image 'id'.
        watchlist_address: Special:Watchlist editor.
        contribs_address(target): Special:Contributions for user 'target'.

    """
    def __init__(self, code, fam=None, user=None, persistent_http = None):
        self.lang = code.lower()
        if isinstance(fam, basestring) or fam is None:
            self.family = Family(fam, fatal = False)
        else:
            self.family = fam

        # if we got an outdated language code, use the new one instead.
        if self.lang in self.family.obsolete:
            if self.family.obsolete[self.lang] is not None:
                self.lang = self.family.obsolete[self.lang]
            else:
                # no such language anymore
                raise NoSuchSite("Language %s in family %s is obsolete"
                                 % (self.lang, self.family.name))

        if self.lang not in self.languages():
            if self.lang == 'zh-classic' \
               and 'zh-classical' in self.languages():
                self.lang = 'zh-classical'
                # database hack (database is varchar[10], so zh-classical
                # is cut to zh-classic)
            elif self.family.name in self.family.langs.keys() \
                 or len(self.family.langs) == 1:
                self.lang = self.family.name
            else:
                raise NoSuchSite("Language %s does not exist in family %s"
                                 %(self.lang,self.family.name))

        self._mediawiki_messages = {}
        self._info = {}
        self.nocapitalize = self.lang in self.family.nocapitalize
        self.user = user
        self._userData = [False, False]
        self._userName = [None, None]
        self._isLoggedIn = [None, None]
        self._isBlocked = [None, None]
        self._messages = [None, None]
        self._rights = [None, None]
        self._token = [None, None]
        self._patrolToken = [None, None]
        self._cookies = [None, None]
        # Calculating valid languages took quite long, so we calculate it once
        # in initialization instead of each time it is used.
        self._validlanguages = []
        for language in self.languages():
            if not language[0].upper() + language[1:] in self.namespaces():
                self._validlanguages.append(language)

        self.persistent_http = False

    def _userIndex(self, sysop = False):
        """Returns the internal index of the user."""
        if sysop:
            return 1
        else:
            return 0

    def username(self, sysop = False):
        return self._userName[self._userIndex(sysop = sysop)]

    def loggedInAs(self, sysop = False):
        """Return the current username if logged in, otherwise return None.

        Checks if we're logged in by loading a page and looking for the login
        link. We assume that we're not being logged out during a bot run, so
        loading the test page is only required once.

        """
        index = self._userIndex(sysop)
        if self._isLoggedIn[index] is None:
            # Load the details only if you don't know the login status.
            # Don't load them just because the other details aren't known.
            self._load(sysop = sysop)
        if self._isLoggedIn[index]:
            return self._userName[index]
        else:
            return None

    def forceLogin(self, sysop = False):
        """Log the user in if not already logged in."""
        if not self.loggedInAs(sysop = sysop):
            loginMan = login.LoginManager(site = self, sysop = sysop)
            #loginMan.logout()
            if loginMan.login(retry = True):
                index = self._userIndex(sysop)
                self._isLoggedIn[index] = True
                self._userName[index] = loginMan.username
                # We know nothing about the new user (but its name)
                # Old info is about the anonymous user
                self._userData[index] = False

    def checkBlocks(self, sysop = False):
        """Check if the user is blocked, and raise an exception if so."""
        self._load(sysop = sysop)
        index = self._userIndex(sysop)
        if self._isBlocked[index]:
            # User blocked
            raise UserBlocked('User is blocked in site %s' % self)

    def isBlocked(self, sysop = False):
        """Check if the user is blocked."""
        self._load(sysop = sysop)
        index = self._userIndex(sysop)
        if self._isBlocked[index]:
            # User blocked
            return True
        else:
            return False

    def _getBlock(self, sysop = False):
        """Get user block data from the API."""
        try:
            params = {
                'action': 'query',
                'meta': 'userinfo',
                'uiprop': 'blockinfo',
            }
            data = query.GetData(params, self)
            if not data or 'error' in data:
                return False
            if self.versionnumber() == 11:     # fix for version 1.11 API.
                data = data['userinfo']
            else:
                data = data['query']['userinfo']
            return 'blockedby' in data
        except NotImplementedError:
            return False

    def isAllowed(self, right, sysop = False):
        """Check if the user has a specific right.
        Among possible rights:
        * Actions: edit, move, delete, protect, upload
        * User levels: autoconfirmed, sysop, bot, empty string (always true)
        """
        if right == '' or right is None:
            return True
        else:
            self._load(sysop = sysop)
            index = self._userIndex(sysop)
            # Handle obsolete editusercssjs permission
            if right in ['editusercss', 'edituserjs'] \
               and right not in self._rights[index]:
                return 'editusercssjs' in self._rights[index]
            return right in self._rights[index]

    def server_time(self):
        """returns a datetime object representing server time"""
        # It is currently user-clock depending
        return self.family.server_time()

    def messages(self, sysop = False):
        """Returns true if the user has new messages, and false otherwise."""
        self._load(sysop = sysop)
        index = self._userIndex(sysop)
        return self._messages[index]

    def cookies(self, sysop = False):
        """Return a string containing the user's current cookies."""
        self._loadCookies(sysop = sysop)
        index = self._userIndex(sysop)
        if self._cookies[index]:
            #convert cookies dictionary data to string.
            outputDatas = ""
            for k, v in self._cookies[index].iteritems():
                if v:
                    outputDatas += "%s=%s; " % (k,v)
                else:
                    # protection for value ''
                    outputDatas += "%s=none; " % k
            return outputDatas
        else:
            return None

    def _loadCookies(self, sysop = False):
        """
         Retrieve session cookies for login
         if family datas define the cross projects, this function will search
         the central login file made by self or cross available project
         functioin will read the cookiedata if got one of them is exist
        """
        index = self._userIndex(sysop)
        if self._cookies[index] is not None:
            return
        try:
            if sysop:
                try:
                    username = config.sysopnames[self.family.name][self.lang]
                except KeyError:
                    raise NoUsername("""\
You tried to perform an action that requires admin privileges, but you haven't
entered your sysop name in your user-config.py. Please add
sysopnames['%s']['%s']='name' to your user-config.py"""
                                     % (self.family.name, self.lang))
            else:
                username = config.usernames[self.family.name][self.lang]
        except KeyError:
            self._cookies[index] = None
            self._isLoggedIn[index] = False
        else:
            # check central login data if cross_projects is available.
            localFn = '%s-%s-%s-login.data' % (self.family.name, self.lang, username)
            localPa = config.datafilepath('login-data', localFn)
            if self.family.cross_projects:
                for proj in [self.family.name] + self.family.cross_projects:
                    #find all central data in all cross_projects
                    centralFn = '%s-%s-central-login.data' % (proj, username)
                    centralPa = config.datafilepath('login-data', centralFn)
                    if os.path.exists(centralPa):
                        self._cookies[index] = self._readCookies(centralFn)
                        break

            if os.path.exists(localPa):
                #read and dump local logindata into self._cookies[index]
                # if self._cookies[index] is not availabe, read the local data and set the dictionary.
                if type(self._cookies[index]) == dict:
                    for k, v in self._readCookies(localFn).iteritems():
                        if k not in self._cookies[index]:
                            self._cookies[index][k] = v
                else:
                    self._cookies[index] = dict([(k,v) for k,v in self._readCookies(localFn).iteritems()])
                #self._cookies[index] = query.CombineParams(self._cookies[index], self._readCookies(localFn))
            elif not os.path.exists(localPa) and not self.family.cross_projects:
                #keep anonymous mode if not login and centralauth not enable
                self._cookies[index] = None
                self._isLoggedIn[index] = False

    def _readCookies(self, filename):
        """read login cookie file and return a dictionary."""
        try:
            f = open( config.datafilepath('login-data', filename), 'r')
            ck = re.compile("(.*?)=(.*?)\r?\n")
            data = dict([(x[0],x[1]) for x in ck.findall(f.read())])
            #data = dict(ck.findall(f.read()))
            f.close()
            return data
        except IOError:
            return None

    def _setupCookies(self, datas, sysop = False):
        """save the cookie dictionary to files
           if cross_project enable, savefiles will separate two, centraldata and localdata.
        """
        index = self._userIndex(sysop)
        if not self._cookies[index]:
            self._cookies[index] = datas
        cache = {0:"",1:""} #0 is central auth, 1 is local.
        if not self.username(sysop):
            if not self._cookies[index]:
                return
            elif self.family.cross_projects_cookie_username in self._cookies[index]:
                # Using centralauth to cross login data, it's not necessary to forceLogin, but Site() didn't know it.
                # So we need add centralauth username data into siteattribute
                self._userName[index] = self._cookies[index][self.family.cross_projects_cookie_username]


        for k, v in datas.iteritems():
            #put key and values into save cache
            if self.family.cross_projects and k in self.family.cross_projects_cookies:
                cache[0] += "%s=%s\n" % (k,v)
            else:
                cache[1] += "%s=%s\n" % (k,v)

        # write the data.
        if self.family.cross_projects and cache[0]:
            filename = '%s-%s-central-login.data' % (self.family.name, self.username(sysop))
            f = open(config.datafilepath('login-data', filename), 'w')
            f.write(cache[0])
            f.close()

        filename = '%s-%s-%s-login.data' % (self.family.name, self.lang, self.username(sysop))
        f = open(config.datafilepath('login-data', filename), 'w')
        f.write(cache[1])
        f.close()

    def _removeCookies(self, name):
        # remove cookies.
        # ToDo: remove all local datas if cross_projects enable.
        #
        if self.family.cross_projects:
            file = config.datafilepath('login-data', '%s-%s-central-login.data' % (self.family.name, name))
            if os.path.exists(file):
                os.remove( file )
        file = config.datafilepath('login-data', '%s-%s-%s-login.data' % (self.family.name, self.lang, name))
        if os.path.exists(file):
            os.remove(file)

    def updateCookies(self, datas, sysop = False):
        """Check and update the current cookies datas and save back to files."""
        index = self._userIndex(sysop)
        if not self._cookies[index]:
            self._setupCookies(datas, sysop)

        for k, v in datas.iteritems():
            if k in self._cookies[index]:
                if v != self._cookies[index][k]:
                    self._cookies[index][k] = v
            else:
                self._cookies[index][k] = v

        self._setupCookies(self._cookies[index], sysop)

    def urlEncode(self, query):
        """Encode a query so that it can be sent using an http POST request."""
        if not query:
            return None
        if hasattr(query, 'iteritems'):
            iterator = query.iteritems()
        else:
            iterator = iter(query)
        l = []
        wpEditToken = None
        for key, value in iterator:
            if isinstance(key, unicode):
                key = key.encode('utf-8')
            if isinstance(value, unicode):
                value = value.encode('utf-8')
            key = urllib.quote(key)
            value = urllib.quote(value)
            if key == 'wpEditToken':
                wpEditToken = value
                continue
            l.append(key + '=' + value)

        # wpEditToken is explicitly added as last value.
        # If a premature connection abort occurs while putting, the server will
        # not have received an edit token and thus refuse saving the page
        if wpEditToken is not None:
            l.append('wpEditToken=' + wpEditToken)
        return '&'.join(l)

    def solveCaptcha(self, data):
        if type(data) == dict: # API Mode result
            if 'edit' in data and  data['edit']['result'] != u"Success":
                data = data['edit']
            if "captcha" in data:
                data = data['captcha']
                captype = data['type']
                id = data['id']
                if captype in ['simple', 'math', 'question']:
                    answer = input('What is the answer to the captcha "%s" ?' % data['question'])
                elif captype == 'image':
                    url = self.protocol() + '://' + self.hostname() + self.captcha_image_address(id)
                    answer = ui.askForCaptcha(url)
                else: #no captcha id result, maybe ReCaptcha.
                    raise CaptchaError('We have been prompted for a ReCaptcha, but pywikipedia does not yet support ReCaptchas')
                return {'id':id, 'answer':answer}
            return None
        else:
            captchaW = re.compile('<label for="wpCaptchaWord">(?P<question>[^<]*)</label>')
            captchaR = re.compile('<input type="hidden" name="wpCaptchaId" id="wpCaptchaId" value="(?P<id>\d+)" />')
            match = captchaR.search(data)
            if match:
                id = match.group('id')
                match = captchaW.search(data)
                if match:
                    answer = input('What is the answer to the captcha "%s" ?' % match.group('question'))
                else:
                    if not config.solve_captcha:
                        raise CaptchaError(id)
                    url = self.protocol() + '://' + self.hostname() + self.captcha_image_address(id)
                    answer = ui.askForCaptcha(url)
                return {'id':id, 'answer':answer}
            Recaptcha = re.compile('<script type="text/javascript" src="http://api\.recaptcha\.net/[^"]*"></script>')
            if Recaptcha.search(data):
                raise CaptchaError('We have been prompted for a ReCaptcha, but pywikipedia does not yet support ReCaptchas')
            return None

    def postForm(self, address, predata, sysop = False, cookies = None):
        """Post http form data to the given address at this site.

        address - the absolute path without hostname.
        predata - a dict or any iterable that can be converted to a dict,
        containing keys and values for the http form.
        cookies - the cookies to send with the form. If None, send self.cookies

        Return a (response, data) tuple, where response is the HTTP
        response object and data is a Unicode string containing the
        body of the response.

        """
        data = self.urlEncode(predata)
        try:
            if cookies:
                return self.postData(address, data, sysop=sysop,
                                        cookies=cookies)
            else:
                return self.postData(address, data, sysop=sysop,
                                    cookies=self.cookies(sysop = sysop))
        except socket.error, e:
            raise ServerError(e)

    def postData(self, address, data,
                 contentType = 'application/x-www-form-urlencoded',
                 sysop = False, compress = True, cookies = None):
        """Post encoded data to the given http address at this site.

        address is the absolute path without hostname.
        data is an ASCII string that has been URL-encoded.

        Returns a (response, data) tuple where response is the HTTP
        response object and data is a Unicode string containing the
        body of the response.
        """

        if address[-1] == "?":
            address = address[:-1]

        headers = {
            'User-agent': useragent,
            'Content-Length': str(len(data)),
            'Content-type':contentType,
        }
        if cookies:
            headers['Cookie'] = cookies

        if compress:
            headers['Accept-encoding'] = 'gzip'
        #print '%s' % headers

        url = '%s://%s%s' % (self.protocol(), self.hostname(), address)
        # Try to retrieve the page until it was successfully loaded (just in
        # case the server is down or overloaded).
        # Wait for retry_idle_time minutes (growing!) between retries.
        retry_idle_time = 1
        retry_attempt = 0
        while True:
            try:
                request = urllib2.Request(url, data, headers)
                f = MyURLopener.open(request)

                # read & info can raise socket.error
                text = f.read()
                headers = f.info()
                break
            except KeyboardInterrupt:
                raise
            except urllib2.HTTPError, e:
                if e.code in [401, 404]:
                    raise PageNotFound(u'Page %s could not be retrieved. Check your family file ?' % url)
                # just check for HTTP Status 500 (Internal Server Error)?
                elif e.code in [500, 502, 504]:
                    output(u'HTTPError: %s %s' % (e.code, e.msg))
                    if config.retry_on_fail:
                        retry_attempt += 1
                        if retry_attempt > config.maxretries:
                            raise MaxTriesExceededError()
                        output(u"WARNING: Could not open '%s'.\nMaybe the server is down. Retrying in %i minutes..."
                               % (url, retry_idle_time))
                        time.sleep(retry_idle_time * 60)
                        # Next time wait longer, but not longer than half an hour
                        retry_idle_time *= 2
                        if retry_idle_time > 30:
                            retry_idle_time = 30
                        continue
                    raise
                else:
                    output(u"Result: %s %s" % (e.code, e.msg))
                    raise
            except Exception, e:
                output(u'%s' %e)
                if config.retry_on_fail:
                    retry_attempt += 1
                    if retry_attempt > config.maxretries:
                        raise MaxTriesExceededError()
                    output(u"WARNING: Could not open '%s'. Maybe the server or\n your connection is down. Retrying in %i minutes..."
                           % (url, retry_idle_time))
                    time.sleep(retry_idle_time * 60)
                    retry_idle_time *= 2
                    if retry_idle_time > 30:
                        retry_idle_time = 30
                    continue
                raise

        # check cookies return or not, if return, send its to update.
        if hasattr(f, 'sheaders'):
            ck = f.sheaders
        else:
            ck = f.info().getallmatchingheaders('set-cookie')
        if ck:
            Reat=re.compile(': (.*?)=(.*?);')
            tmpc = {}
            for d in ck:
                m = Reat.search(d)
                if m: tmpc[m.group(1)] = m.group(2)
            if self.cookies(sysop):
                self.updateCookies(tmpc, sysop)

        resContentType = headers.get('content-type', '')
        contentEncoding = headers.get('content-encoding', '')

        # Ensure that all sent data is received
        if int(headers.get('content-length', '0')) != len(text) and 'content-length' in headers:
            output(u'Warning! len(text) does not match content-length: %s != %s' % \
                (len(text), headers.get('content-length')))
            return self.postData(address, data, contentType, sysop, compress, cookies)

        if compress and contentEncoding == 'gzip':
            text = decompress_gzip(text)

        R = re.compile('charset=([^\'\";]+)')
        m = R.search(resContentType)
        if m:
            charset = m.group(1)
        else:
            if verbose:
                output(u"WARNING: No character set found.")
            # UTF-8 as default
            charset = 'utf-8'
        # Check if this is the charset we expected
        self.checkCharset(charset)
        # Convert HTML to Unicode
        try:
            text = unicode(text, charset, errors = 'strict')
        except UnicodeDecodeError, e:
            print e
            output(u'ERROR: Invalid characters found on %s://%s%s, replaced by \\ufffd.'
                   % (self.protocol(), self.hostname(), path))
            # We use error='replace' in case of bad encoding.
            text = unicode(text, charset, errors = 'replace')

        # If a wiki page, get user data
        self._getUserDataOld(text, sysop = sysop)

        return f, text

    def getUrl(self, path, retry = None, sysop = False, data = None, compress = True,
               no_hostname = False, cookie_only=False, refer=None, back_response=False):
        """
        Low-level routine to get a URL from the wiki.

        Parameters:
            path        - The absolute path, without the hostname.
            retry       - If True, retries loading the page when a network error
                        occurs.
            sysop       - If True, the sysop account's cookie will be used.
            data        - An optional dict providing extra post request parameters.
            cookie_only - Only return the cookie the server sent us back

           Returns the HTML text of the page converted to unicode.
        """

        if retry is None:
            retry = config.retry_on_fail

        headers = {
            'User-agent': useragent,
            #'Accept-Language': config.mylang,
            #'Accept-Charset': config.textfile_encoding,
            #'Keep-Alive': '115',
            #'Connection': 'keep-alive',
            #'Cache-Control': 'max-age=0',
            #'': '',
        }

        if not no_hostname and self.cookies(sysop = sysop):
            headers['Cookie'] = self.cookies(sysop = sysop)
        if compress:
            headers['Accept-encoding'] = 'gzip'

        if refer:
            headers['Refer'] = refer

        if no_hostname: # This allow users to parse also toolserver's script
            url = path  # and other useful pages without using some other functions.
        else:
            url = '%s://%s%s' % (self.protocol(), self.hostname(), path)
        data = self.urlEncode(data)

        # Try to retrieve the page until it was successfully loaded (just in
        # case the server is down or overloaded).
        # Wait for retry_idle_time minutes (growing!) between retries.
        retry_idle_time = 1
        retry_attempt = 0
        while True:
            try:
                request = urllib2.Request(url, data, headers)
                f = MyURLopener.open(request)

                # read & info can raise socket.error
                text = f.read()
                headers = f.info()
                break
            except KeyboardInterrupt:
                raise
            except urllib2.HTTPError, e:
                if e.code in [401, 404]:
                    raise PageNotFound(
u'Page %s could not be retrieved. Check your family file.'
                                       % url)
                elif e.code in [403]:
                    raise PageNotFound(
u'Page %s could not be retrieved. Check your virus wall.'
                                       % url)
                elif e.code == 504:
                    output(u'HTTPError: %s %s' % (e.code, e.msg))
                    if retry:
                        retry_attempt += 1
                        if retry_attempt > config.maxretries:
                            raise MaxTriesExceededError()
                        output(
u"WARNING: Could not open '%s'.Maybe the server or\n your connection is down. Retrying in %i minutes..."
                               % (url, retry_idle_time))
                        time.sleep(retry_idle_time * 60)
                        # Next time wait longer,
                        # but not longer than half an hour
                        retry_idle_time *= 2
                        if retry_idle_time > 30:
                            retry_idle_time = 30
                        continue
                    raise
                else:
                    output(u"Result: %s %s" % (e.code, e.msg))
                    raise
            except Exception, e:
                output(u'%s' %e)
                if retry:
                    retry_attempt += 1
                    if retry_attempt > config.maxretries:
                        raise MaxTriesExceededError()
                    output(
u"WARNING: Could not open '%s'. Maybe the server or\n your connection is down. Retrying in %i minutes..."
                           % (url, retry_idle_time))
                    time.sleep(retry_idle_time * 60)
                    retry_idle_time *= 2
                    if retry_idle_time > 30:
                        retry_idle_time = 30
                    continue

                raise
        # check cookies return or not, if return, send its to update.
        if hasattr(f, 'sheaders'):
            ck = f.sheaders
        else:
            ck = f.info().getallmatchingheaders('set-cookie')
        if not no_hostname and ck:
            Reat=re.compile(': (.*?)=(.*?);')
            tmpc = {}
            for d in ck:
                m = Reat.search(d)
                if m: tmpc[m.group(1)] = m.group(2)
            self.updateCookies(tmpc, sysop)

        if cookie_only:
            return headers.get('set-cookie', '')
        contentType = headers.get('content-type', '')
        contentEncoding = headers.get('content-encoding', '')

        # Ensure that all sent data is received
        if int(headers.get('content-length', '0')) != len(text) and 'content-length' in headers:
            output(u'Warning! len(text) does not match content-length: %s != %s' % \
                (len(text), headers.get('content-length')))
            return self.getUrl(path, retry, sysop, data, compress, no_hostname, cookie_only, back_response)

        if compress and contentEncoding == 'gzip':
            text = decompress_gzip(text)

        R = re.compile('charset=([^\'\";]+)')
        m = R.search(contentType)
        if m:
            charset = m.group(1)
        else:
            if verbose:
                output(u"WARNING: No character set found.")
            # UTF-8 as default
            charset = 'utf-8'
        # Check if this is the charset we expected
        self.checkCharset(charset)
        # Convert HTML to Unicode
        try:
            text = unicode(text, charset, errors = 'strict')
        except UnicodeDecodeError, e:
            print e
            if no_hostname:
                output(u'ERROR: Invalid characters found on %s, replaced by \\ufffd.' % path)
            else:
                output(u'ERROR: Invalid characters found on %s://%s%s, replaced by \\ufffd.' % (self.protocol(), self.hostname(), path))
            # We use error='replace' in case of bad encoding.
            text = unicode(text, charset, errors = 'replace')

        # If a wiki page, get user data
        self._getUserDataOld(text, sysop = sysop)

        if back_response:
            return f, text

        return text

    def _getUserData(self, text, sysop = False, force = True):
        """
        Get the user data from an API query dict.

        Parameters:
        * text - the page text
        * sysop - is the user a sysop?
        """

        index = self._userIndex(sysop)
        # Check for blocks

        if 'blockedby' in text and not self._isBlocked[index]:
            # Write a warning if not shown earlier
            if sysop:
                account = 'Your sysop account'
            else:
                account = 'Your account'
            output(u'\nWARNING: %s on %s is blocked by %s.\nReason: %s\nEditing using this account will stop the run.\n'
                   % (account, self, text['blockedby'], text['blockreason']))
        self._isBlocked[index] = 'blockedby' in text

        # Check for new messages, the data must had key 'messages' in dict.
        if 'messages' in text:
            if not self._messages[index]:
                # User has *new* messages
                if sysop:
                    output(u'NOTE: You have new messages in your sysop account on %s' % self)
                else:
                    output(u'NOTE: You have new messages on %s' % self)
            self._messages[index] = True
        else:
            self._messages[index] = False

        # Don't perform other checks if the data was already loaded
        if self._userData[index] and not force:
            return

        # Get username.
        # The data in anonymous mode had key 'anon'
        # if 'anon' exist, username is IP address, not to collect it right now
        if not 'anon' in text:
            self._isLoggedIn[index] = True
            self._userName[index] = text['name']
        else:
            self._isLoggedIn[index] = False
            self._userName[index] = None

        # Get user groups and rights
        if 'groups' in text:
            self._rights[index] = text['groups']
            self._rights[index].extend(text['rights'])
            # Warnings
            # Don't show warnings for not logged in users, they will just fail to
            # do any action
            if self._isLoggedIn[index]:
                if 'bot' not in self._rights[index] and config.notify_unflagged_bot:
                    # Sysop + bot flag = Sysop flag in MediaWiki < 1.7.1?
                    if sysop:
                        output(u'Note: Your sysop account on %s does not have a bot flag. Its edits will be visible in the recent changes.' % self)
                    else:
                        output(u'WARNING: Your account on %s does not have a bot flag. Its edits will be visible in the recent changes and it may get blocked.' % self)
                if sysop and 'sysop' not in self._rights[index]:
                    output(u'WARNING: Your sysop account on %s does not seem to have sysop rights. You may not be able to perform any sysop-restricted actions using it.' % self)
        else:
            # 'groups' is not exists, set default rights
            self._rights[index] = []
            if self._isLoggedIn[index]:
                # Logged in user
                self._rights[index].append('user')
                # Assume bot, and thus autoconfirmed
                self._rights[index].extend(['bot', 'autoconfirmed'])
                if sysop:
                    # Assume user reported as a sysop indeed has the sysop rights
                    self._rights[index].append('sysop')
        # Assume the user has the default rights if API not query back
        self._rights[index].extend(['read', 'createaccount', 'edit', 'upload', 'createpage', 'createtalk', 'move', 'upload'])
        #remove Duplicate rights
        self._rights[index] = list(set(self._rights[index]))

        # Get token
        if 'preferencestoken' in text:
            self._token[index] = text['preferencestoken']
            if self._rights[index] is not None:
                # Token and rights are loaded - user data is now loaded
                self._userData[index] = True
        elif self.versionnumber() < 14:
            # uiprop 'preferencestoken' is start from 1.14, if 1.8~13, we need to use other way to get token
            params = {
                'action': 'query',
                'prop': 'info',
                'titles':'Non-existing page',
                'intoken': 'edit',
            }
            data = query.GetData(params, self, sysop=sysop)['query']['pages'].values()[0]
            if 'edittoken' in data:
                self._token[index] = data['edittoken']
                self._userData[index] = True
            else:
                output(u'WARNING: Token not found on %s. You will not be able to edit any page.' % self)
        else:
            if not self._isBlocked[index]:
                output(u'WARNING: Token not found on %s. You will not be able to edit any page.' % self)

    def _getUserDataOld(self, text, sysop = False, force = True):
        """
        Get the user data from a wiki page data.

        Parameters:
        * text - the page text
        * sysop - is the user a sysop?
        """

        index = self._userIndex(sysop)

        if '<div id="globalWrapper">' not in text:
            # Not a wiki page
            return
        # Check for blocks - but only if version is 1.11 (userinfo is available)
        # and the user data was not yet loaded
        if self.versionnumber() >= 11 and (not self._userData[index] or force):
            blocked = self._getBlock(sysop = sysop)
            if blocked and not self._isBlocked[index]:
                # Write a warning if not shown earlier
                if sysop:
                    account = 'Your sysop account'
                else:
                    account = 'Your account'
                output(u'WARNING: %s on %s is blocked. Editing using this account will stop the run.' % (account, self))
            self._isBlocked[index] = blocked

        # Check for new messages
        if '<div class="usermessage">' in text:
            if not self._messages[index]:
                # User has *new* messages
                if sysop:
                    output(u'NOTE: You have new messages in your sysop account on %s' % self)
                else:
                    output(u'NOTE: You have new messages on %s' % self)
            self._messages[index] = True
        else:
            self._messages[index] = False
        # Don't perform other checks if the data was already loaded
        if self._userData[index] and not force:
            return

        # Search for the the user page link at the top.
        # Note that the link of anonymous users (which doesn't exist at all
        # in Wikimedia sites) has the ID pt-anonuserpage, and thus won't be
        # found here.
        userpageR = re.compile('<li id="pt-userpage"><a href=".+?">(?P<username>.+?)</a></li>')
        m = userpageR.search(text)
        if m:
            self._isLoggedIn[index] = True
            self._userName[index] = m.group('username')
        else:
            self._isLoggedIn[index] = False
            # No idea what is the user name, and it isn't important
            self._userName[index] = None

        if self.family.name == 'wikitravel':    # fix for Wikitravel's user page link.
            self = self.family.user_page_link(self,index)

        # Check user groups, if possible (introduced in 1.10)
        groupsR = re.compile(r'var wgUserGroups = \[\"(.+)\"\];')
        m = groupsR.search(text)
        checkLocal = True
        if default_code in self.family.cross_allowed: # if current languages in cross allowed list, check global bot flag.
            globalgroupsR = re.compile(r'var wgGlobalGroups = \[\"(.+)\"\];')
            mg = globalgroupsR.search(text)
            if mg: # the account had global permission
                globalRights = mg.group(1)
                globalRights = globalRights.split('","')
                self._rights[index] = globalRights
                if self._isLoggedIn[index]:
                    if 'Global_bot' in globalRights: # This account has the global bot flag, no need to check local flags.
                        checkLocal = False
                    else:
                        output(u'Your bot account does not have global the bot flag, checking local flag.')
        else:
            if verbose: output(u'Note: this language does not allow global bots.')
        if m and checkLocal:
            rights = m.group(1)
            rights = rights.split('", "')
            if '*' in rights:
                rights.remove('*')
            self._rights[index] = rights
            # Warnings
            # Don't show warnings for not logged in users, they will just fail to
            # do any action
            if self._isLoggedIn[index]:
                if 'bot' not in self._rights[index] and config.notify_unflagged_bot:
                    # Sysop + bot flag = Sysop flag in MediaWiki < 1.7.1?
                    if sysop:
                        output(u'Note: Your sysop account on %s does not have a bot flag. Its edits will be visible in the recent changes.' % self)
                    else:
                        output(u'WARNING: Your account on %s does not have a bot flag. Its edits will be visible in the recent changes and it may get blocked.' % self)
                if sysop and 'sysop' not in self._rights[index]:
                    output(u'WARNING: Your sysop account on %s does not seem to have sysop rights. You may not be able to perform any sysop-restricted actions using it.' % self)
        else:
            # We don't have wgUserGroups, and can't check the rights
            self._rights[index] = []
            if self._isLoggedIn[index]:
                # Logged in user
                self._rights[index].append('user')
                # Assume bot, and thus autoconfirmed
                self._rights[index].extend(['bot', 'autoconfirmed'])
                if sysop:
                    # Assume user reported as a sysop indeed has the sysop rights
                    self._rights[index].append('sysop')
        # Assume the user has the default rights
        self._rights[index].extend(['read', 'createaccount', 'edit', 'upload', 'createpage', 'createtalk', 'move', 'upload'])
        if 'bot' in self._rights[index] or 'sysop' in self._rights[index]:
            self._rights[index].append('apihighlimits')
        if 'sysop' in self._rights[index]:
            self._rights[index].extend(['delete', 'undelete', 'block', 'protect', 'import', 'deletedhistory', 'unwatchedpages'])

        # Search for a token
        tokenR = re.compile(r"\<input type='hidden' value=\"(.*?)\" name=\"wpEditToken\"")
        tokenloc = tokenR.search(text)
        if tokenloc:
            self._token[index] = tokenloc.group(1)
            if self._rights[index] is not None:
                # In this case, token and rights are loaded - user data is now loaded
                self._userData[index] = True
        else:
            # Token not found
            # Possible reason for this is the user is blocked, don't show a
            # warning in this case, otherwise do show a warning
            # Another possible reason is that the page cannot be edited - ensure
            # there is a textarea and the tab "view source" is not shown
            if u'<textarea' in text and u'<li id="ca-viewsource"' not in text and not self._isBlocked[index]:
                # Token not found
                output(u'WARNING: Token not found on %s. You will not be able to edit any page.' % self)

    def siteinfo(self, key = 'general', force = False, dump = False):
        """Get Mediawiki Site informations by API
           dump - return all siteinfo datas

           some siprop params is huge data for MediaWiki, they take long times to read by testment.
           these params could get, but only one by one.

        """
        # protection for key in other datatype
        if type(key) not in [str, unicode]:
           key = 'general'

        if self._info and key in self._info and not force:
            if dump:
                return self._info
            else:
                return self._info[key]

        params = {
            'action':'query',
            'meta':'siteinfo',
            'siprop':['general', 'namespaces', ],
        }
        #ver 1.10 handle
        if self.versionnumber() > 10:
            params['siprop'].extend(['statistics', ])
            if key in ['specialpagealiases', 'interwikimap', 'namespacealiases', 'usergroups', ]:
                if verbose: print 'getting huge siprop %s...' % key
                params['siprop'] = [key]

        #ver 1.13 handle
        if self.versionnumber() > 13:
            if key not in ['specialpagealiases', 'interwikimap', 'namespacealiases', 'usergroups', ]:
                params['siprop'].extend(['fileextensions', 'rightsinfo', ])
            if key in ['magicwords', 'extensions', ]:
                if verbose: print 'getting huge siprop %s...' % key
                params['siprop'] = [key]
        try:
            data = query.GetData(params, self)['query']
        except NotImplementedError:
            return None

        if not hasattr(self, '_info'):
            self._info = data
        else:
            if key == 'magicwords':
                if self.versionnumber() <= 13:
                    return None #Not implemented
                self._info[key]={}
                for entry in data[key]:
                    self._info[key][entry['name']] = entry['aliases']
            else:
                for k, v in data.iteritems():
                    self._info[k] = v
        #data pre-process
        if dump:
            return self._info
        else:
            return self._info.get(key)

    def mediawiki_message(self, key, forceReload = False):
        """Return the MediaWiki message text for key "key" """
        # Allmessages is retrieved once for all per created Site object
        if (not self._mediawiki_messages) or forceReload:
            api = self.has_api()
            if verbose:
                output(u"Retrieving mediawiki messages from Special:Allmessages")
            # Only MediaWiki r27393/1.12 and higher support XML output for Special:Allmessages
            if self.versionnumber() < 12:
                usePHP = True
            else:
                usePHP = False
                elementtree = True
                try:
                    try:
                        from xml.etree.cElementTree import XML # 2.5
                    except ImportError:
                        try:
                            from cElementTree import XML
                        except ImportError:
                            from elementtree.ElementTree import XML
                except ImportError:
                    if verbose:
                        output(u'Elementtree was not found, using BeautifulSoup instead')
                    elementtree = False

            if config.use_diskcache and not api:
                import diskcache
                _dict = lambda x : diskcache.CachedReadOnlyDictI(x, prefix = "msg-%s-%s-" % (self.family.name, self.lang))
            else:
                _dict = dict

            retry_idle_time = 1
            retry_attempt = 0
            while True:
                if api and self.versionnumber() >= 12 or self.versionnumber() >= 16:
                    params = {
                        'action': 'query',
                        'meta': 'allmessages',
                        'ammessages': key,
                    }
                    datas = query.GetData(params, self)['query']['allmessages'][0]
                    if "missing" in datas:
                        raise KeyError("message is not exist.")
                    elif datas['name'] not in self._mediawiki_messages:
                        self._mediawiki_messages[datas['name']] = datas['*']
                    #self._mediawiki_messages = _dict([(tag['name'].lower(), tag['*'])
                    #        for tag in datas if not 'missing' in tag])
                elif usePHP:
                    phppage = self.getUrl(self.get_address("Special:Allmessages") + "&ot=php")
                    Rphpvals = re.compile(r"(?ms)'([^']*)' =&gt; '(.*?[^\\])',")
                    # Previous regexp don't match empty messages. Fast workaround...
                    phppage = re.sub("(?m)^('.*?' =&gt;) '',", r"\1 ' ',", phppage)
                    self._mediawiki_messages = _dict([(name.strip().lower(),
                        html2unicode(message.replace("\\'", "'")))
                            for (name, message) in Rphpvals.findall(phppage)])
                else:
                    xml = self.getUrl(self.get_address("Special:Allmessages") + "&ot=xml")
                    # xml structure is :
                    # <messages lang="fr">
                    #    <message name="about">À propos</message>
                    #    ...
                    # </messages>
                    if elementtree:
                        decode = xml.encode(self.encoding())

                        # Skip extraneous data such as PHP warning or extra
                        # whitespaces added from some MediaWiki extensions
                        xml_dcl_pos = decode.find('<?xml')
                        if xml_dcl_pos > 0:
                            decode = decode[xml_dcl_pos:]

                        tree = XML(decode)
                        self._mediawiki_messages = _dict([(tag.get('name').lower(), tag.text)
                                for tag in tree.getiterator('message')])
                    else:
                        tree = BeautifulStoneSoup(xml)
                        self._mediawiki_messages = _dict([(tag.get('name').lower(), html2unicode(tag.string))
                                for tag in tree.findAll('message') if tag.string])

                if not self._mediawiki_messages:
                    # No messages could be added.
                    # We assume that the server is down.
                    # Wait some time, then try again.
                    output(u'WARNING: No messages found in Special:Allmessages. Maybe the server is down. Retrying in %i minutes...' % retry_idle_time)
                    time.sleep(retry_idle_time * 60)
                    # Next time wait longer, but not longer than half an hour
                    retry_attempt += 1
                    if retry_attempt > config.maxretries:
                        raise ServerError()
                    retry_idle_time *= 2
                    if retry_idle_time > 30:
                        retry_idle_time = 30
                    continue
                break

        if self.family.name == 'wikitravel':    # fix for Wikitravel's mediawiki message setting
            self = self.family.mediawiki_message(self)

        key = key.lower()
        try:
            return self._mediawiki_messages[key]
        except KeyError:
            if not forceReload:
                return self.mediawiki_message(key, True)
            else:
                raise KeyError("MediaWiki key '%s' does not exist on %s" % (key, self))

    def has_mediawiki_message(self, key):
        """Return True if this site defines a MediaWiki message for 'key'."""
        #return key in self._mediawiki_messages
        try:
            v = self.mediawiki_message(key)
            return True
        except KeyError:
            return False

    def has_api(self):
        """Return True if this sites family has api interface."""
        try:
            if config.use_api:
                x = self.apipath()
                del x
                return True
        except NotImplementedError:
            pass
        return False

    def _load(self, sysop = False, force = False):
        """
        Loads user data.
        This is only done if we didn't do get any page yet and the information
        is requested, otherwise we should already have this data.

        Parameters:
        * sysop - Get sysop user data?
        """
        index = self._userIndex(sysop)
        if self._userData[index] and not force:
            return
        if verbose:
            output(u'Getting information for site %s' % self)

        # Get data
        # API Userinfo is available from version 1.11
        # preferencetoken available from 1.14
        if self.has_api() and self.versionnumber() >= 11:
            #Query userinfo
            params = {
                'action': 'query',
                'meta': 'userinfo',
                'uiprop': ['blockinfo','groups','rights','hasmsg'],
            }
            if self.versionnumber() >= 12:
                params['uiprop'].append('ratelimits')
            if self.versionnumber() >= 14:
                params['uiprop'].append('preferencestoken')

            data = query.GetData(params, self, sysop=sysop)

            # Show the API error code instead making an index error
            if 'error' in data:
                raise RuntimeError('%s' % data['error'])

            if self.versionnumber() == 11:
                text = data['userinfo']
            else:
                text = data['query']['userinfo']

            self._getUserData(text, sysop = sysop, force = force)
        else:
            url = self.edit_address('Non-existing_page')
            text = self.getUrl(url, sysop = sysop)

            self._getUserDataOld(text, sysop = sysop, force = force)

    def search(self, key, number=10, namespaces=None):
        """
        Yield search results for query.
        Use API when enabled use_api and version >= 1.11,
        or use Special:Search.
        """
        if self.has_api() and self.versionnumber() >= 11:
            #Yield search results (using api) for query.
            params = {
                'action': 'query',
                'list': 'search',
                'srsearch': key,
            }
            if number:
                params['srlimit'] = number
            if namespaces:
                params['srnamespace'] = namespaces

            offset = 0
            while offset < number or not number:
                params['sroffset'] = offset
                data = query.GetData(params, self)
                if 'error'in data:
                    raise NotImplementedError('%s' % data['error']['info'])
                data = data['query']
                if 'error' in data:
                    raise RuntimeError('%s' % data['error'])
                if not data['search']:
                    break
                for s in data['search']:
                    offset += 1
                    page = Page(self, s['title'])
                    yield page, s['snippet'], '', s['size'], s['wordcount'], s['timestamp']
        else:
            #Yield search results (using Special:Search page) for query.
            throttle = True
            path = self.search_address(urllib.quote_plus(key.encode('utf-8')),
                                       n=number, ns=namespaces)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(ur'<li><a href=".+?" title="(?P<title>.+?)">.+?</a>',
                                re.DOTALL)
            for m in entryR.finditer(html):
                page = Page(self, m.group('title'))
                yield page, '', '', '', '', ''

    # TODO: avoid code duplication for the following methods

    def logpages(self, number = 50, mode = '', title = None, user = None, repeat = False,
                 namespace = [], start = None, end = None, tag = None, newer = False, dump = False):

        if not self.has_api() or self.versionnumber() < 11 or \
           mode not in ('block', 'protect', 'rights', 'delete', 'upload',
                        'move', 'import', 'patrol', 'merge', 'suppress',
                        'review', 'stable', 'gblblock', 'renameuser',
                        'globalauth', 'gblrights', 'abusefilter', 'newusers'):
            raise NotImplementedError, mode
        params = {
            'action'    : 'query',
            'list'      : 'logevents',
            'letype'    : mode,
            'lelimit'   : int(number),
            'ledir'     : 'older',
            'leprop'    : ['ids', 'title', 'type', 'user', 'timestamp', 'comment', 'details',],
        }

        if number > config.special_page_limit:
            params['lelimit'] = config.special_page_limit
            if number > 5000 and self.isAllowed('apihighlimits'):
                params['lelimit'] = 5000
        if newer:
            params['ledir'] = 'newer'
        if user:
            params['leuser'] = user
        if title:
            params['letitle'] = title
        if start:
            params['lestart'] = start
        if end:
            params['leend'] = end
        if tag and self.versionnumber() >= 16: # tag support from mw:r58399
            params['letag'] = tag

        nbresults = 0
        while True:
            result = query.GetData(params, self)
            if 'error' in result or 'warnings' in result:
                output('%s' % result)
                raise Error
            for c in result['query']['logevents']:
                if (not namespace or c['ns'] in namespace) and \
                   not 'actionhidden' in c.keys():
                    if dump:
                        # dump result only.
                        yield c
                    else:
                        if c['ns'] == 6:
                            p_ret = ImagePage(self, c['title'])
                        else:
                            p_ret = Page(self, c['title'], defaultNamespace=c['ns'])

                        yield (p_ret, c['user'],
                          parsetime2stamp(c['timestamp']),
                          c['comment'], )

                nbresults += 1
                if nbresults >= number:
                    break
            if 'query-continue' in result and nbresults < number:
                params['lestart'] = result['query-continue']['logevents']['lestart']
            elif repeat:
                nbresults = 0
                try:
                    params.pop('lestart')
                except KeyError:
                    pass
            else:
                break
        return

    def newpages(self, number = 10, get_redirect = False, repeat = False, namespace = 0, rcshow = ['!bot','!redirect'], user = None, returndict = False):
        """Yield new articles (as Page objects) from Special:Newpages.

        Starts with the newest article and fetches the number of articles
        specified in the first argument. If repeat is True, it fetches
        Newpages again. If there is no new page, it blocks until there is
        one, sleeping between subsequent fetches of Newpages.

        The objects yielded are dependent on parmater returndict.
        When true, it yields a tuple composed of a Page object and a dict of attributes.
        When false, it yields a tuple composed of the Page object,
        timestamp (unicode), length (int), an empty unicode string, username
        or IP address (str), comment (unicode).

        """
        # TODO: in recent MW versions Special:Newpages takes a namespace parameter,
        #       and defaults to 0 if not specified.
        # TODO: Detection of unregistered users is broken
        # TODO: Repeat mechanism doesn't make much sense as implemented;
        #       should use both offset and limit parameters, and have an
        #       option to fetch older rather than newer pages
        seen = set()
        while True:
            if self.has_api() and self.versionnumber() >= 10:
                params = {
                    'action': 'query',
                    'list': 'recentchanges',
                    'rctype': 'new',
                    'rcnamespace': namespace,
                    'rclimit': int(number),
                    'rcprop': ['ids','title','timestamp','sizes','user','comment'],
                    'rcshow': rcshow,
                }
                if user: params['rcuser'] = user
                data = query.GetData(params, self)['query']['recentchanges']

                for np in data:
                    if np['pageid'] not in seen:
                        seen.add(np['pageid'])
                        page = Page(self, np['title'], defaultNamespace=np['ns'])
                        if returndict:
                            yield page, np
                        else:
                            yield page, np['timestamp'], np['newlen'], u'', np['user'], np['comment']
            else:
                path = self.newpages_address(n=number, namespace=namespace)
                # The throttling is important here, so always enabled.
                get_throttle()
                html = self.getUrl(path)

                entryR = re.compile('<li[^>]*>(?P<date>.+?) \S*?<a href=".+?"'
                    ' title="(?P<title>.+?)">.+?</a>.+?[\(\[](?P<length>[\d,.]+)[^\)\]]*[\)\]]'
                    ' .?<a href=".+?" title=".+?:(?P<username>.+?)">')
                for m in entryR.finditer(html):
                    date = m.group('date')
                    title = m.group('title')
                    title = title.replace('&quot;', '"')
                    length = int(re.sub("[,.]", "", m.group('length')))
                    loggedIn = u''
                    username = m.group('username')
                    comment = u''

                    if title not in seen:
                        seen.add(title)
                        page = Page(self, title)
                        yield page, date, length, loggedIn, username, comment
            if not repeat:
                break

    def longpages(self, number = 10, repeat = False):
        """Yield Pages from Special:Longpages.

        Return values are a tuple of Page object, length(int).

        """
        #TODO: should use offset and limit parameters; 'repeat' as now
        #      implemented is fairly useless
        # this comment applies to all the XXXXpages methods following, as well
        seen = set()
        path = self.longpages_address(n=number)
        entryR = re.compile(ur'<li>\(<a href=".+?" title=".+?">.+?</a>\) .<a href=".+?" title="(?P<title>.+?)">.+?</a> .\[(?P<length>[\d.,]+).*?\]</li>', re.UNICODE)

        while True:
            get_throttle()
            html = self.getUrl(path)
            for m in entryR.finditer(html):
                title = m.group('title')
                length = int(re.sub('[.,]', '', m.group('length')))
                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page, length
            if not repeat:
                break

    def shortpages(self, number = 10, repeat = False):
        """Yield Pages and lengths from Special:Shortpages."""
        throttle = True
        seen = set()
        path = self.shortpages_address(n = number)
        entryR = re.compile(ur'<li>\(<a href=".+?" title=".+?">.+?</a>\) .<a href=".+?" title="(?P<title>.+?)">.+?</a> .\[(?P<length>[\d.,]+).*?\]</li>', re.UNICODE)

        while True:
            get_throttle()
            html = self.getUrl(path)

            for m in entryR.finditer(html):
                title = m.group('title')
                length = int(re.sub('[., ]', '', m.group('length')))

                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page, length
            if not repeat:
                break

    def categories(self, number=10, repeat=False):
        """Yield Category objects from Special:Categories"""
        import catlib
        seen = set()
        while True:
            path = self.categories_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a>.*?</li>')
            for m in entryR.finditer(html):
                title = m.group('title')
                if title not in seen:
                    seen.add(title)
                    page = catlib.Category(self, title)
                    yield page
            if not repeat:
                break

    def deadendpages(self, number = 10, repeat = False):
        """Yield Page objects retrieved from Special:Deadendpages."""
        seen = set()
        while True:
            path = self.deadendpages_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def ancientpages(self, number = 10, repeat = False):
        """Yield Pages, datestamps from Special:Ancientpages."""
        seen = set()
        while True:
            path = self.ancientpages_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile('<li><a href=".+?" title="(?P<title>.+?)">.+?</a> (?P<date>.+?)</li>')
            for m in entryR.finditer(html):
                title = m.group('title')
                date = m.group('date')
                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page, date
            if not repeat:
                break

    def lonelypages(self, number = 10, repeat = False):
        """Yield Pages retrieved from Special:Lonelypages."""
        throttle = True
        seen = set()
        while True:
            path = self.lonelypages_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def unwatchedpages(self, number = 10, repeat = False):
        """Yield Pages from Special:Unwatchedpages (requires Admin privileges)."""
        seen = set()
        while True:
            path = self.unwatchedpages_address(n=number)
            get_throttle()
            html = self.getUrl(path, sysop = True)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a>.+?</li>')
            for m in entryR.finditer(html):
                title = m.group('title')
                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def uncategorizedcategories(self, number = 10, repeat = False):
        """Yield Categories from Special:Uncategorizedcategories."""
        import catlib
        seen = set()
        while True:
            path = self.uncategorizedcategories_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')
                if title not in seen:
                    seen.add(title)
                    page = catlib.Category(self, title)
                    yield page
            if not repeat:
                break

    def newimages(self, number = 100, lestart = None, leend = None, leuser = None, letitle = None, repeat = False):
        """
        Yield ImagePages from APIs, call: action=query&list=logevents&letype=upload&lelimit=500

        Options directly from APIs:
        ---
        Parameters:
                           Default: ids|title|type|user|timestamp|comment|details
          lestart        - The timestamp to start enumerating from.
          leend          - The timestamp to end enumerating.
          ledir          - In which direction to enumerate.
                           One value: newer, older
                           Default: older
          leuser         - Filter entries to those made by the given user.
          letitle        - Filter entries to those related to a page.
          lelimit        - How many total event entries to return.
                           No more than 500 (5000 for bots) allowed.
                           Default: 10
        """

        for o, u, t, c in self.logpages(number = number, mode = 'upload', title = letitle, user = leuser,
                 repeat = repeat, start = lestart, end = leend):
            yield o, t, u, c
        return

    def recentchanges(self, number = 100, rcstart = None, rcend = None, rcshow = None, rcdir='older', rctype ='edit|new', namespace=None, includeredirects=True, repeat = False, user = None, returndict = False):
        """
        Yield recent changes as Page objects
        uses API call: action=query&list=recentchanges&rctype=edit|new&rclimit=500

        Starts with the newest change and fetches the number of changes
        specified in the first argument. If repeat is True, it fetches
        again.

        Options directly from APIs:
        ---
        Parameters:
          rcstart        - The timestamp to start enumerating from.
          rcend          - The timestamp to end enumerating.
          rcdir          - In which direction to enumerate.
                           One value: newer, older
                           Default: older
          rcnamespace    - Filter log entries to only this namespace(s)
                           Values (separate with '|'):
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
          rcprop         - Include additional pieces of information
                           Values (separate with '|'):
                           user, comment, flags, timestamp, title, ids, sizes,
                           redirect, patrolled, loginfo
                           Default: title|timestamp|ids
          rcshow         - Show only items that meet this criteria.
                           For example, to see only minor edits done by
                           logged-in users, set show=minor|!anon
                           Values (separate with '|'):
                           minor, !minor, bot, !bot, anon, !anon,
                           redirect, !redirect, patrolled, !patrolled
          rclimit        - How many total changes to return.
                           No more than 500 (5000 for bots) allowed.
                           Default: 10
          rctype         - Which types of changes to show.
                           Values (separate with '|'): edit, new, log

        The objects yielded are dependent on parmater returndict.
        When true, it yields a tuple composed of a Page object and a dict of attributes.
        When false, it yields a tuple composed of the Page object,
        timestamp (unicode), length (int), an empty unicode string, username
        or IP address (str), comment (unicode).

        # TODO: Detection of unregistered users is broken
        """
        if rctype is None:
            rctype = 'edit|new'
        params = {
            'action'    : 'query',
            'list'      : 'recentchanges',
            'rcdir'     : rcdir,
            'rctype'    : rctype,
            'rcprop'    : ['user','comment','timestamp','title','ids','loginfo','sizes'],    #','flags','redirect','patrolled']
            'rcnamespace' : namespace,
            'rclimit'   : int(number),
            }
        if user: params['rcuser'] = user
        if rcstart: params['rcstart'] = rcstart
        if rcend: params['rcend'] = rcend
        if rcshow: params['rcshow'] = rcshow
        if rctype: params['rctype'] = rctype

        while True:
            data = query.GetData(params, self, encodeTitle = False)
            if 'error' in data:
                raise RuntimeError('%s' % data['error'])
            try:
                rcData = data['query']['recentchanges']
            except KeyError:
                raise ServerError("The APIs don't return data, the site may be down")

            for i in rcData:
                page = Page(self, i['title'], defaultNamespace=i['ns'])
                if returndict:
                    yield page, i
                else:
                    comment = ''
                    if 'comment' in i:
                        comment = i['comment']
                    yield page, i['timestamp'], i['newlen'], True, i['user'], comment
            if not repeat:
                break

    def patrol(self, rcid, token = None):
        if not self.has_api() or self.versionnumber() < 12:
            raise Exception('patrol: no API: not implemented')

        if not token:
            token = self.getPatrolToken()

        params = {
            'action': 'patrol',
            'rcid':   rcid,
            'token':  token,
        }

        result = query.GetData(params, self)
        if 'error' in result:
            raise RuntimeError("%s" % result['error'])

        return True

    def uncategorizedimages(self, number = 10, repeat = False):
        """Yield ImagePages from Special:Uncategorizedimages."""
        seen = set()
        ns = self.image_namespace()
        entryR = re.compile(
            '<a href=".+?" title="(?P<title>%s:.+?)">.+?</a>' % ns)
        while True:
            path = self.uncategorizedimages_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            for m in entryR.finditer(html):
                title = m.group('title')
                if title not in seen:
                    seen.add(title)
                    page = ImagePage(self, title)
                    yield page
            if not repeat:
                break

    def uncategorizedpages(self, number = 10, repeat = False):
        """Yield Pages from Special:Uncategorizedpages."""
        seen = set()
        while True:
            path = self.uncategorizedpages_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def uncategorizedtemplates(self, number = 10, repeat = False):
        """Yield Pages from Special:UncategorizedTemplates."""
        seen = set()
        while True:
            path = self.uncategorizedtemplates_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def unusedcategories(self, number = 10, repeat = False):
        """Yield Category objects from Special:Unusedcategories."""
        import catlib
        seen = set()
        while True:
            path = self.unusedcategories_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile('<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = catlib.Category(self, title)
                    yield page
            if not repeat:
                break

    def wantedcategories(self, number=10, repeat=False):
        """Yield Category objects from Special:wantedcategories."""
        import catlib
        seen = set()
        while True:
            path = self.wantedcategories_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile(
                '<li><a href=".+?" class="new" title="(?P<title>.+?) \(page does not exist\)">.+?</a> .+?\)</li>')
            for m in entryR.finditer(html):
                title = m.group('title')

                if title not in seen:
                    seen.add(title)
                    page = catlib.Category(self, title)
                    yield page
            if not repeat:
                break

    def unusedfiles(self, number = 10, repeat = False, extension = None):
        """Yield ImagePage objects from Special:Unusedimages."""
        seen = set()
        ns = self.image_namespace()
        entryR = re.compile(
            '<a href=".+?" title="(?P<title>%s:.+?)">.+?</a>' % ns)
        while True:
            path = self.unusedfiles_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            for m in entryR.finditer(html):
                fileext = None
                title = m.group('title')
                if extension:
                    fileext = title[len(title)-3:]
                if title not in seen and fileext == extension:
                    ## Check whether the media is used in a Proofread page
                    # code disabled because it slows this method down, and
                    # because it is unclear what it's supposed to do.
                    #basename = title[6:]
                    #page = Page(self, 'Page:' + basename)

                    #if not page.exists():
                    seen.add(title)
                    image = ImagePage(self, title)
                    yield image
            if not repeat:
                break

    def withoutinterwiki(self, number=10, repeat=False):
        """Yield Pages without language links from Special:Withoutinterwiki."""
        seen = set()
        while True:
            path = self.withoutinterwiki_address(n=number)
            get_throttle()
            html = self.getUrl(path)
            entryR = re.compile('<li><a href=".+?" title="(?P<title>.+?)">.+?</a></li>')
            for m in entryR.finditer(html):
                title = m.group('title')
                if title not in seen:
                    seen.add(title)
                    page = Page(self, title)
                    yield page
            if not repeat:
                break

    def randompage(self, redirect = False):
        if self.has_api() and self.versionnumber() >= 12:
            params = {
                'action': 'query',
                'list': 'random',
                #'rnnamespace': '0',
                'rnlimit': '1',
                #'': '',
            }
            if redirect:
                params['rnredirect'] = 1

            data = query.GetData(params, self)
            return Page(self, data['query']['random'][0]['title'])
        else:
            if redirect:
                """Yield random redirect page via Special:RandomRedirect."""
                html = self.getUrl(self.randomredirect_address())
            else:
                """Yield random page via Special:Random"""
                html = self.getUrl(self.random_address())
            m = re.search('var wgPageName = "(?P<title>.+?)";', html)
            if m is not None:
                return Page(self, m.group('title'))

    def randomredirectpage(self):
        return self.randompage(redirect = True)

    def allpages(self, start='!', namespace=None, includeredirects=True,
                 throttle=True):
        """
        Yield all Pages in alphabetical order.

        Parameters:
        start   Start at this page. By default, it starts at '!', and yields
                all pages.
        namespace Yield all pages in this namespace; defaults to 0.
                MediaWiki software will only return pages in one namespace
                at a time.

        If includeredirects is False, redirects will not be found.

        It is advised not to use this directly, but to use the
        AllpagesPageGenerator from pagegenerators.py instead.

        """
        if namespace is None:
            page = Page(self, start)
            namespace = page.namespace()
            start = page.titleWithoutNamespace()

        if not self.has_api():
            for page in self._allpagesOld(start, namespace, includeredirects, throttle):
                yield page
            return

        params = {
            'action'     : 'query',
            'list'       : 'allpages',
            'aplimit'    : config.special_page_limit,
            'apnamespace': namespace,
            'apfrom'     : start
        }

        if not includeredirects:
            params['apfilterredir'] = 'nonredirects'
        elif includeredirects == 'only':
            params['apfilterredir'] = 'redirects'

        while True:
            if throttle:
                get_throttle()
            data = query.GetData(params, self)
            if verbose:
                print 'DEBUG allpages>>> data.keys()', data.keys()
            if 'warnings' in data:
                warning = data['warnings']['allpages']['*']
                raise RuntimeError("API query warning: %s" % warning)
            if 'error' in data:
                raise RuntimeError("API query error: %s" % data)
            if not 'allpages' in data['query']:
                raise RuntimeError("API query error, no pages found: %s" % data)
            count = 0
            for p in data['query']['allpages']:
                count += 1
                yield Page(self, p['title'])
                if count >= config.special_page_limit:
                    break
            if 'query-continue' in data and count < params['aplimit']:
                params['apfrom'] = data['query-continue']['allpages']['apfrom']
            else:
                break

    def _allpagesOld(self, start='!', namespace=0, includeredirects=True,
                 throttle=True):
        """
        Yield all Pages from Special:Allpages.

        This method doesn't work with MediaWiki 1.14 because of a change to
        Special:Allpages. It is only left here for compatibility with older
        MediaWiki versions, which don't support the API.

        Parameters:
        start   Start at this page. By default, it starts at '!', and yields
                all pages.
        namespace Yield all pages in this namespace; defaults to 0.
                MediaWiki software will only return pages in one namespace
                at a time.

        If includeredirects is False, redirects will not be found.
        If includeredirects equals the string 'only', only redirects
        will be found. Note that this has not been tested on older
        versions of the MediaWiki code.

        It is advised not to use this directly, but to use the
        AllpagesPageGenerator from pagegenerators.py instead.

        """
        monobook_error = True
        if start == '':
            start='!'

        while True:
            # encode Non-ASCII characters in hexadecimal format (e.g. %F6)
            start = start.encode(self.encoding())
            start = urllib.quote(start)
            # load a list which contains a series of article names (always 480)
            path = self.allpages_address(start, namespace)
            output(u'Retrieving Allpages special page for %s from %s, namespace %i' % (repr(self), start, namespace))
            returned_html = self.getUrl(path)
            # Try to find begin and end markers
            try:
                # In 1.4, another table was added above the navigational links
                if self.versionnumber() >= 4:
                    begin_s = '</table><hr /><table'
                    end_s = '</table'
                else:
                    begin_s = '<table'
                    end_s = '</table'
                ibegin = returned_html.index(begin_s)
                iend = returned_html.index(end_s,ibegin + 3)
            except ValueError:
                if monobook_error:
                    raise ServerError("Couldn't extract allpages special page. Make sure you're using MonoBook skin.")
                else:
                    # No list of wikilinks
                    break
            monobook_error = False
            # remove the irrelevant sections
            returned_html = returned_html[ibegin:iend]
            if self.versionnumber()==2:
                R = re.compile('/wiki/(.*?)\" *class=[\'\"]printable')
            elif self.versionnumber()<5:
                # Apparently the special code for redirects was added in 1.5
                R = re.compile('title ?=\"(.*?)\"')
            elif not includeredirects:
                R = re.compile('\<td(?: width="33%")?\>\<a href=\"\S*\" +title ?="(.*?)"')
            elif includeredirects == 'only':
                R = re.compile('\<td(?: width="33%")?>\<[^\<\>]*allpagesredirect\"\>\<a href=\"\S*\" +title ?="(.*?)"')
            else:
                R = re.compile('title ?=\"(.*?)\"')
            # Count the number of useful links on this page
            n = 0
            for hit in R.findall(returned_html):
                # count how many articles we found on the current page
                n = n + 1
                if self.versionnumber()==2:
                    yield Page(self, url2link(hit, site = self, insite = self))
                else:
                    yield Page(self, hit)
                # save the last hit, so that we know where to continue when we
                # finished all articles on the current page. Append a '!' so that
                # we don't yield a page twice.
                start = Page(self,hit).titleWithoutNamespace() + '!'
            # A small shortcut: if there are less than 100 pages listed on this
            # page, there is certainly no next. Probably 480 would do as well,
            # but better be safe than sorry.
            if n < 100:
                if (not includeredirects) or includeredirects == 'only':
                    # Maybe there were only so few because the rest is or is not a redirect
                    R = re.compile('title ?=\"(.*?)\"')
                    allLinks = R.findall(returned_html)
                    if len(allLinks) < 100:
                        break
                    elif n == 0:
                        # In this special case, no pages of the requested type
                        # were found, and "start" will remain and be double-encoded.
                        # Use the last page as the start of the next page.
                        start = Page(self, allLinks[-1]).titleWithoutNamespace() + '!'
                else:
                    break
            #else:
            #    # Don't send a new request if "Next page (pagename)" isn't present
            #    Rnonext = re.compile(r'title="(Special|%s):.+?">%s</a></td></tr></table>' % (
            #        self.mediawiki_message('nstab-special'),
            #        re.escape(self.mediawiki_message('nextpage')).replace('\$1', '.*?')))
            #    if not Rnonext.search(full_returned_html):
            #        break

    def prefixindex(self, prefix, namespace=0, includeredirects=True):
        """Yield all pages with a given prefix.

        Parameters:
        prefix   The prefix of the pages.
        namespace Namespace number; defaults to 0.
                MediaWiki software will only return pages in one namespace
                at a time.

        If includeredirects is False, redirects will not be found.
        If includeredirects equals the string 'only', only redirects
        will be found. Note that this has not been tested on older
        versions of the MediaWiki code.

        It is advised not to use this directly, but to use the
        PrefixingPageGenerator from pagegenerators.py instead.
        """
        for page in self.allpages(start = prefix, namespace = namespace, includeredirects = includeredirects):
            if page.titleWithoutNamespace().startswith(prefix):
                yield page
            else:
                break

    def protectedpages(self, namespace = None, type = 'edit', lvl = 0):
        """ Yield all the protected pages, using Special:ProtectedPages
            * namespace is a namespace number
            * type can be 'edit' or 'move
            * lvl : protection level, can be 0, 'autoconfirmed', or 'sysop'
        """
        # Avoid problems of encoding and stuff like that, let it divided please
        url = self.protectedpages_address()
        url += '&type=%s&level=%s' % (type, lvl)
        if namespace is not None: # /!\ if namespace seems simpler, but returns false when ns=0

            url += '&namespace=%s' % namespace
        parser_text = self.getUrl(url)
        while 1:
            #<li><a href="/wiki/Pagina_principale" title="Pagina principale">Pagina principale</a>‎ <small>(6.522 byte)</small> ‎(protetta)</li>
            m = re.findall(r'<li><a href=".*?" title=".*?">(.*?)</a>.*?<small>\((.*?)\)</small>.*?\((.*?)\)</li>', parser_text)
            for data in m:
                title = data[0]
                size = data[1]
                status = data[2]
                yield Page(self, title)
            nextpage = re.findall(r'<.ul>\(.*?\).*?\(.*?\).*?\(<a href="(.*?)".*?</a>\) +?\(<a href=', parser_text)
            if nextpage != []:
                parser_text = self.getUrl(nextpage[0].replace('&amp;', '&'))
                continue
            else:
                break

    def linksearch(self, siteurl, limit=500):
        """Yield Pages from results of Special:Linksearch for 'siteurl'."""
        cache = []
        R = re.compile('title ?=\"([^<>]*?)\">[^<>]*</a></li>')
        urlsToRetrieve = [siteurl]
        if not siteurl.startswith('*.'):
            urlsToRetrieve.append('*.' + siteurl)

        if self.has_api() and self.versionnumber() >= 11:
            output(u'Querying API exturlusage...')
            for url in urlsToRetrieve:
                params = {
                    'action': 'query',
                    'list'  : 'exturlusage',
                    'eulimit': limit,
                    'euquery': url,
                }
                count = 0
                while True:
                    data = query.GetData(params, self)
                    if data['query']['exturlusage'] == []:
                        break
                    for pages in data['query']['exturlusage']:
                        count += 1
                        if not siteurl in pages['title']:
                            # the links themselves have similar form
                            if pages['pageid'] not in cache:
                                cache.append(pages['pageid'])
                                yield Page(self, pages['title'], defaultNamespace=pages['ns'])
                        if count >= limit:
                            break

                    if 'query-continue' in data and count < limit:
                            params['euoffset'] = data[u'query-continue'][u'exturlusage'][u'euoffset']
                    else:
                            break
        else:
            output(u'Querying [[Special:Linksearch]]...')
            for url in urlsToRetrieve:
                offset = 0
                while True:
                    path = self.linksearch_address(url, limit=limit, offset=offset)
                    get_throttle()
                    html = self.getUrl(path)
                    #restricting the HTML source :
                    #when in the source, this div marks the beginning of the input
                    loc = html.find('<div class="mw-spcontent">')
                    if loc > -1:
                        html = html[loc:]
                    #when in the source, marks the end of the linklist
                    loc = html.find('<div class="printfooter">')
                    if loc > -1:
                        html = html[:loc]

                    #our regex fetches internal page links and the link they contain
                    links = R.findall(html)
                    if not links:
                        #no more page to be fetched for that link
                        break
                    for title in links:
                        if not siteurl in title:
                            # the links themselves have similar form
                            if title in cache:
                                continue
                            else:
                                cache.append(title)
                                yield Page(self, title)
                    offset += limit

    def __repr__(self):
        return self.family.name+":"+self.lang

    def linkto(self, title, othersite = None):
        """Return unicode string in the form of a wikilink to 'title'

        Use optional Site argument 'othersite' to generate an interwiki link
        from the other site to the current site.

        """
        if othersite and othersite.lang != self.lang:
            return u'[[%s:%s]]' % (self.lang, title)
        else:
            return u'[[%s]]' % title

    def isInterwikiLink(self, s):
        """Return True if s is in the form of an interwiki link.

        Interwiki links have the form "foo:bar" or ":foo:bar" where foo is a
        known language code or family. Called recursively if the first part
        of the link refers to this site's own family and/or language.

        """
        s = s.replace("_", " ").strip(" ").lstrip(":")
        if not ':' in s:
            return False
        first, rest = s.split(':',1)
        # interwiki codes are case-insensitive
        first = first.lower().strip(" ")
        # commons: forwards interlanguage links to wikipedia:, etc.
        if self.family.interwiki_forward:
            interlangTargetFamily = Family(self.family.interwiki_forward)
        else:
            interlangTargetFamily = self.family
        if self.getNamespaceIndex(first):
            return False
        if first in interlangTargetFamily.langs:
            if first == self.lang:
                return self.isInterwikiLink(rest)
            else:
                return True
        if first in self.family.get_known_families(site = self):
            if first == self.family.name:
                return self.isInterwikiLink(rest)
            else:
                return True
        return False

    def getmagicwords(self, word):
        """Return list of localized "word" magic words for the site."""
        if self.versionnumber() <= 13:
            raise NotImplementedError
        return self.siteinfo('magicwords').get(word)

    def redirect(self, default=False):
        """Return the localized redirect tag for the site.

        """
        # return the magic word without the preceding '#' character
        if default or self.versionnumber() <= 13:
            return u'REDIRECT'
        else:
            return self.getmagicwords('redirect')[0].lstrip("#")

    def redirectRegex(self):
        """Return a compiled regular expression matching on redirect pages.

        Group 1 in the regex match object will be the target title.

        """
        #NOTE: this is needed, since the API can give false positives!
        default = 'REDIRECT'
        keywords = self.versionnumber() > 13 and self.getmagicwords('redirect')
        if keywords:
            pattern = r'(?:' + '|'.join(keywords) + ')'
        else:
            # no localized keyword for redirects
            pattern = r'#%s' % default
        if self.versionnumber() > 12:
            # in MW 1.13 (at least) a redirect directive can follow whitespace
            prefix = r'\s*'
        else:
            prefix = r'[\r\n]*'
        # A redirect starts with hash (#), followed by a keyword, then
        # arbitrary stuff, then a wikilink. The wikilink may contain
        # a label, although this is not useful.
        return re.compile(prefix + pattern
                                 + '\s*:?\s*\[\[(.+?)(?:\|.*?)?\]\]',
                          re.IGNORECASE | re.UNICODE | re.DOTALL)

    def pagenamecodes(self, default=True):
        """Return list of localized PAGENAME tags for the site."""
        return self.versionnumber() > 13 and self.getmagicwords('pagename') \
               or u'PAGENAME'

    def pagename2codes(self, default=True):
        """Return list of localized PAGENAMEE tags for the site."""
        return self.versionnumber() > 13 and self.getmagicwords('pagenamee') \
               or u'PAGENAMEE'

    def resolvemagicwords(self, wikitext):
        """Replace the {{ns:xx}} marks in a wikitext with the namespace names"""

        defaults = []
        for namespace in self.family.namespaces.itervalues():
            value = namespace.get('_default', None)
            if value:
                if isinstance(value, list):
                    defaults += value
                else:
                    defaults.append(value)

        named = re.compile(u'{{ns:(' + '|'.join(defaults) + ')}}', re.I)

        def replacenamed(match):
            return self.normalizeNamespace(match.group(1))

        wikitext = named.sub(replacenamed, wikitext)

        numbered = re.compile('{{ns:(-?\d{1,2})}}', re.I)

        def replacenumbered(match):
            return self.namespace(int(match.group(1)))

        return numbered.sub(replacenumbered, wikitext)

    # The following methods are for convenience, so that you can access
    # methods of the Family class easier.
    def encoding(self):
        """Return the current encoding for this site."""
        return self.family.code2encoding(self.lang)

    def encodings(self):
        """Return a list of all historical encodings for this site."""
        return self.family.code2encodings(self.lang)

    def category_namespace(self):
        """Return the canonical name of the Category namespace on this site."""
        # equivalent to self.namespace(14)?
        return self.family.category_namespace(self.lang)

    def category_namespaces(self):
        """Return a list of all valid names for the Category namespace."""
        return self.family.category_namespaces(self.lang)

    def category_redirects(self):
        return self.family.category_redirects(self.lang)

    def image_namespace(self, fallback = '_default'):
        """Return the canonical name of the Image namespace on this site."""
        # equivalent to self.namespace(6)?
        return self.family.image_namespace(self.lang, fallback)

    def template_namespace(self, fallback = '_default'):
        """Return the canonical name of the Template namespace on this site."""
        # equivalent to self.namespace(10)?
        return self.family.template_namespace(self.lang, fallback)

    def export_address(self):
        """Return URL path for Special:Export."""
        return self.family.export_address(self.lang)

    def query_address(self):
        """Return URL path + '?' for query.php (if enabled on this Site)."""
        return self.family.query_address(self.lang)

    def api_address(self):
        """Return URL path + '?' for api.php (if enabled on this Site)."""
        return self.family.api_address(self.lang)

    def apipath(self):
        """Return URL path for api.php (if enabled on this Site)."""
        return self.family.apipath(self.lang)

    def scriptpath(self):
        """Return URL prefix for scripts on this site ({{SCRIPTPATH}} value)"""
        return self.family.scriptpath(self.lang)

    def protocol(self):
        """Return protocol ('http' or 'https') for access to this site."""
        return self.family.protocol(self.lang)

    def hostname(self):
        """Return host portion of site URL."""
        return self.family.hostname(self.lang)

    def path(self):
        """Return URL path for index.php on this Site."""
        return self.family.path(self.lang)

    def dbName(self):
        """Return MySQL database name."""
        return self.family.dbName(self.lang)

    def move_address(self):
        """Return URL path for Special:Movepage."""
        return self.family.move_address(self.lang)

    def delete_address(self, s):
        """Return URL path to delete title 's'."""
        return self.family.delete_address(self.lang, s)

    def undelete_view_address(self, s, ts=''):
        """Return URL path to view Special:Undelete for title 's'

        Optional argument 'ts' returns path to view specific deleted version.

        """
        return self.family.undelete_view_address(self.lang, s, ts)

    def undelete_address(self):
        """Return URL path to Special:Undelete."""
        return self.family.undelete_address(self.lang)

    def protect_address(self, s):
        """Return URL path to protect title 's'."""
        return self.family.protect_address(self.lang, s)

    def unprotect_address(self, s):
        """Return URL path to unprotect title 's'."""
        return self.family.unprotect_address(self.lang, s)

    def put_address(self, s):
        """Return URL path to submit revision to page titled 's'."""
        return self.family.put_address(self.lang, s)

    def get_address(self, s):
        """Return URL path to retrieve page titled 's'."""
        title = s.replace(' ', '_')
        return self.family.get_address(self.lang, title)

    def nice_get_address(self, s):
        """Return shorter URL path to retrieve page titled 's'."""
        return self.family.nice_get_address(self.lang, s)

    def edit_address(self, s):
        """Return URL path for edit form for page titled 's'."""
        return self.family.edit_address(self.lang, s)

    def watch_address(self, s):
        """Return URL path for watching the titled 's'."""
        return self.family.watch_address(self.lang, s)

    def unwatch_address(self, s):
        """Return URL path for unwatching the titled 's'."""
        return self.family.unwatch_address(self.lang, s)

    def purge_address(self, s):
        """Return URL path to purge cache and retrieve page 's'."""
        return self.family.purge_address(self.lang, s)

    def block_address(self):
        """Return path to block an IP address."""
        return self.family.block_address(self.lang)

    def unblock_address(self):
        """Return path to unblock an IP address."""
        return self.family.unblock_address(self.lang)

    def blocksearch_address(self, s):
        """Return path to search for blocks on IP address 's'."""
        return self.family.blocksearch_address(self.lang, s)

    def linksearch_address(self, s, limit=500, offset=0):
        """Return path to Special:Linksearch for target 's'."""
        return self.family.linksearch_address(self.lang, s, limit=limit, offset=offset)

    def search_address(self, q, n=50, ns=0):
        """Return path to Special:Search for query 'q'."""
        return self.family.search_address(self.lang, q, n, ns)

    def allpages_address(self, s, ns = 0):
        """Return path to Special:Allpages."""
        return self.family.allpages_address(self.lang, start=s, namespace = ns)

    def log_address(self, n=50, mode = '', user = ''):
        """Return path to Special:Log."""
        return self.family.log_address(self.lang, n, mode, user)

    def newpages_address(self, n=50, namespace=0):
        """Return path to Special:Newpages."""
        return self.family.newpages_address(self.lang, n, namespace)

    def longpages_address(self, n=500):
        """Return path to Special:Longpages."""
        return self.family.longpages_address(self.lang, n)

    def shortpages_address(self, n=500):
        """Return path to Special:Shortpages."""
        return self.family.shortpages_address(self.lang, n)

    def unusedfiles_address(self, n=500):
        """Return path to Special:Unusedimages."""
        return self.family.unusedfiles_address(self.lang, n)

    def categories_address(self, n=500):
        """Return path to Special:Categories."""
        return self.family.categories_address(self.lang, n)

    def deadendpages_address(self, n=500):
        """Return path to Special:Deadendpages."""
        return self.family.deadendpages_address(self.lang, n)

    def ancientpages_address(self, n=500):
        """Return path to Special:Ancientpages."""
        return self.family.ancientpages_address(self.lang, n)

    def lonelypages_address(self, n=500):
        """Return path to Special:Lonelypages."""
        return self.family.lonelypages_address(self.lang, n)

    def protectedpages_address(self, n=500):
        """Return path to Special:ProtectedPages"""
        return self.family.protectedpages_address(self.lang, n)

    def unwatchedpages_address(self, n=500):
        """Return path to Special:Unwatchedpages."""
        return self.family.unwatchedpages_address(self.lang, n)

    def uncategorizedcategories_address(self, n=500):
        """Return path to Special:Uncategorizedcategories."""
        return self.family.uncategorizedcategories_address(self.lang, n)

    def uncategorizedimages_address(self, n=500):
        """Return path to Special:Uncategorizedimages."""
        return self.family.uncategorizedimages_address(self.lang, n)

    def uncategorizedpages_address(self, n=500):
        """Return path to Special:Uncategorizedpages."""
        return self.family.uncategorizedpages_address(self.lang, n)

    def uncategorizedtemplates_address(self, n=500):
        """Return path to Special:Uncategorizedpages."""
        return self.family.uncategorizedtemplates_address(self.lang, n)

    def unusedcategories_address(self, n=500):
        """Return path to Special:Unusedcategories."""
        return self.family.unusedcategories_address(self.lang, n)

    def wantedcategories_address(self, n=500):
        """Return path to Special:Wantedcategories."""
        return self.family.wantedcategories_address(self.lang, n)

    def withoutinterwiki_address(self, n=500):
        """Return path to Special:Withoutinterwiki."""
        return self.family.withoutinterwiki_address(self.lang, n)

    def references_address(self, s):
        """Return path to Special:Whatlinksere for page 's'."""
        return self.family.references_address(self.lang, s)

    def allmessages_address(self):
        """Return path to Special:Allmessages."""
        return self.family.allmessages_address(self.lang)

    def upload_address(self):
        """Return path to Special:Upload."""
        return self.family.upload_address(self.lang)

    def double_redirects_address(self, default_limit = True):
        """Return path to Special:Doubleredirects."""
        return self.family.double_redirects_address(self.lang, default_limit)

    def broken_redirects_address(self, default_limit = True):
        """Return path to Special:Brokenredirects."""
        return self.family.broken_redirects_address(self.lang, default_limit)

    def random_address(self):
        """Return path to Special:Random."""
        return self.family.random_address(self.lang)

    def randomredirect_address(self):
        """Return path to Special:RandomRedirect."""
        return self.family.randomredirect_address(self.lang)

    def login_address(self):
        """Return path to Special:Userlogin."""
        return self.family.login_address(self.lang)

    def captcha_image_address(self, id):
        """Return path to Special:Captcha for image 'id'."""
        return self.family.captcha_image_address(self.lang, id)

    def watchlist_address(self):
        """Return path to Special:Watchlist editor."""
        return self.family.watchlist_address(self.lang)

    def contribs_address(self, target, limit=500, offset=''):
        """Return path to Special:Contributions for user 'target'."""
        return self.family.contribs_address(self.lang,target,limit,offset)

    def globalusers_address(self, target='', limit=500, offset='', group=''):
        """Return path to Special:GlobalUsers for user 'target' and/or group 'group'."""
        return self.family.globalusers_address(self.lang, target, limit, offset, group)

    def __hash__(self):
        return hash(repr(self))

    def version(self):
        """Return MediaWiki version number as a string."""
        return self.family.version(self.lang)

    def versionnumber(self):
        """Return an int identifying MediaWiki version.

        Currently this is implemented as returning the minor version
        number; i.e., 'X' in version '1.X.Y'

        """
        return self.family.versionnumber(self.lang)

    def live_version(self):
        """Return the 'real' version number found on [[Special:Version]]

        Return value is a tuple (int, int, str) of the major and minor
        version numbers and any other text contained in the version.

        """
        global htmldata
        if not hasattr(self, "_mw_version"):
            PATTERN = r"^(?:: )?([0-9]+)\.([0-9]+)(.*)$"
            versionpage = self.getUrl(self.get_address("Special:Version"))
            htmldata = BeautifulSoup(versionpage, convertEntities="html")
            # try to find the live version
            # 1st try is for mw < 1.17wmf1
            # 2nd try is for mw 1.17wmf1
            # 3rd uses family file which is not live
            for versionstring in [htmldata.findAll(
                                      text="MediaWiki")[1].parent.nextSibling,
                                  htmldata.body.table.findAll(
                                      'td')[1].contents[0],
                                  self.family.version(self.lang)]:
                m = re.match(PATTERN, str(versionstring).strip())
                if m: break
            else:
                raise Error(u'Cannot find any live version!')
            self._mw_version = (int(m.group(1)), int(m.group(2)), m.group(3))
        return self._mw_version

    def checkCharset(self, charset):
        """Warn if charset returned by wiki doesn't match family file."""
        fromFamily = self.encoding()
        assert fromFamily.lower() == charset.lower(), \
               "charset for %s changed from %s to %s" \
                   % (repr(self), fromFamily, charset)
        if fromFamily.lower() != charset.lower():
            raise ValueError(
"code2encodings has wrong charset for %s. It should be %s, but is %s"
                             % (repr(self), charset, self.encoding()))

    def shared_image_repository(self):
        """Return a tuple of image repositories used by this site."""
        return self.family.shared_image_repository(self.lang)

    def __cmp__(self, other):
        """Perform equality and inequality tests on Site objects."""
        if not isinstance(other, Site):
            return 1
        if self.family.name == other.family.name:
            return cmp(self.lang ,other.lang)
        return cmp(self.family.name, other.family.name)

    def category_on_one_line(self):
        """Return True if this site wants all category links on one line."""
        return self.lang in self.family.category_on_one_line

    def interwiki_putfirst(self):
        """Return list of language codes for ordering of interwiki links."""
        return self.family.interwiki_putfirst.get(self.lang, None)

    def interwiki_putfirst_doubled(self, list_of_links):
        # TODO: is this even needed?  No family in the framework has this
        # dictionary defined!
        if self.lang in self.family.interwiki_putfirst_doubled:
            if len(list_of_links) >= self.family.interwiki_putfirst_doubled[self.lang][0]:
                list_of_links2 = []
                for lang in list_of_links:
                    list_of_links2.append(lang.language())
                list = []
                for lang in self.family.interwiki_putfirst_doubled[self.lang][1]:
                    try:
                        list.append(list_of_links[list_of_links2.index(lang)])
                    except ValueError:
                        pass
                return list
            else:
                return False
        else:
            return False

    def getSite(self, code):
        """Return Site object for language 'code' in this Family."""
        return getSite(code = code, fam = self.family, user=self.user)

    def namespace(self, num, all = False):
        """Return string containing local name of namespace 'num'.

        If optional argument 'all' is true, return a tuple of all recognized
        values for this namespace.

        """
        return self.family.namespace(self.lang, num, all = all)

    def normalizeNamespace(self, value):
        """Return canonical name for namespace 'value' in this Site's language.

        'Value' should be a string or unicode.
        If no match, return 'value' unmodified.

        """
        if not self.nocapitalize:
            # make sure first letter gets normalized; there is at least
            # one case ("İ") in which s.lower().upper() != s
            value = value[0].lower().upper() + value[1:]
        return self.family.normalizeNamespace(self.lang, value)

    def namespaces(self):
        """Return list of canonical namespace names for this Site."""

        # n.b.: this does not return namespace numbers; to determine which
        # numeric namespaces the framework recognizes for this Site (which
        # may or may not actually exist on the wiki), use
        # self.family.namespaces.keys()

        if self in _namespaceCache:
            return _namespaceCache[self]
        else:
            nslist = []
            for n in self.family.namespaces:
                try:
                    ns = self.family.namespace(self.lang, n)
                except KeyError:
                    # No default namespace defined
                    continue
                if ns is not None:
                    nslist.append(self.family.namespace(self.lang, n))
            _namespaceCache[self] = nslist
            return nslist

    def getNamespaceIndex(self, namespace):
        """Given a namespace name, return its int index, or None if invalid."""
        return self.family.getNamespaceIndex(self.lang, namespace)

    def linktrail(self):
        """Return regex for trailing chars displayed as part of a link."""
        return self.family.linktrail(self.lang)

    def language(self):
        """Return Site's language code."""
        return self.lang

    def fam(self):
        """Return Family object for this Site."""
        return self.family

    def sitename(self):
        """Return string representing this Site's name and language."""
        return self.family.name+':'+self.lang

    def languages(self):
        """Return list of all valid language codes for this site's Family."""
        return self.family.iwkeys

    def validLanguageLinks(self):
        """Return list of language codes that can be used in interwiki links."""
        return self._validlanguages

    def disambcategory(self):
        """Return Category in which disambig pages are listed."""
        import catlib
        try:
            return catlib.Category(self,
                    self.namespace(14)+':'+self.family.disambcatname[self.lang])
        except KeyError:
            raise NoPage

    def getToken(self, getalways = True, getagain = False, sysop = False):
        index = self._userIndex(sysop)
        if getagain or (getalways and self._token[index] is None):
            output(u'Getting a token.')
            self._load(sysop = sysop, force = True)
        if self._token[index] is not None:
            return self._token[index]
        else:
            return False

    def getPatrolToken(self, sysop = False):
        index = self._userIndex(sysop)

        if self._patrolToken[index] is None:
            output(u'Getting a patrol token.')
            params = {
                'action'    : 'query',
                'list'      : 'recentchanges',
                'rcshow'    : '!patrolled',
                'rctoken'   : 'patrol',
                'rclimit'   : 1,
            }
            data = query.GetData(params, self, encodeTitle = False)
            if 'error' in data:
                raise RuntimeError('%s' % data['error'])
            try:
                rcData = data['query']['recentchanges']
            except KeyError:
                raise ServerError("The APIs don't return data, the site may be down")

            self._patrolToken[index] = rcData[0]['patroltoken']

        return self._patrolToken[index]

    def getFilesFromAnHash(self, hash_found = None):
        """ Function that uses APIs to give the images that has the same hash. Useful
            to find duplicates or nowcommons.

            NOTE: it returns also the image itself, if you don't want it, just
            filter the list returned.

            NOTE 2: it returns the image WITHOUT the image namespace.
        """
        if self.versionnumber() < 12:
            return None

        if hash_found is None: # If the hash is none return None and not continue
            return None
        # Now get all the images with the same hash
        #action=query&format=xml&list=allimages&aisha1=%s
        image_namespace = "%s:" % self.image_namespace() # Image:
        params = {
            'action'    :'query',
            'list'      :'allimages',
            'aisha1'    :hash_found,
        }
        allimages = query.GetData(params, self, encodeTitle = False)['query']['allimages']
        files = list()
        for imagedata in allimages:
            image = imagedata[u'name']
            files.append(image)
        return files

# Caches to provide faster access
_sites = {}
_namespaceCache = {}

def getSite(code=None, fam=None, user=None, persistent_http=None, noLogin=False):
    if code is None:
        code = default_code
    if fam is None:
        fam = default_family
    key = '%s:%s:%s:%s' % (fam, code, user, persistent_http)
    if key not in _sites:
        _sites[key] = Site(code=code, fam=fam, user=user,
                           persistent_http=persistent_http)
    ret =  _sites[key]
    if not ret.family.isPublic() and not noLogin:
        ret.forceLogin()
    return ret

def setSite(site):
    global default_code, default_family
    default_code = site.language()
    default_family = site.family

def calledModuleName():
    """Return the name of the module calling this function.

    This is required because the -help option loads the module's docstring
    and because the module name will be used for the filename of the log.

    """
    # get commandline arguments
    args = sys.argv
    try:
        # clip off the '.py' filename extension
        return os.path.basename(args[0][:args[0].rindex('.')])
    except ValueError:
        return os.path.basename(args[0])

def decodeArg(arg):
    if sys.platform=='win32':
        if config.console_encoding == 'cp850':
            # Western Windows versions give parameters encoded as windows-1252
            # even though the console encoding is cp850.
            return unicode(arg, 'windows-1252')
        elif config.console_encoding == 'cp852':
            # Central/Eastern European Windows versions give parameters encoded
            # as windows-1250 even though the console encoding is cp852.
            return unicode(arg, 'windows-1250')
        else:
            return unicode(arg, config.console_encoding)
    else:
        # Linux uses the same encoding for both.
        # I don't know how non-Western Windows versions behave.
        return unicode(arg, config.console_encoding)

def handleArgs(*args):
    """Handle standard command line arguments, return the rest as a list.

    Takes the commandline arguments, converts them to Unicode, processes all
    global parameters such as -lang or -log. Returns a list of all arguments
    that are not global. This makes sure that global arguments are applied
    first, regardless of the order in which the arguments were given.

    args may be passed as an argument, thereby overriding sys.argv

    """
    global default_code, default_family, verbose, debug
    # get commandline arguments
    if not args:
        args = sys.argv[1:]
    # get the name of the module calling this function. This is
    # required because the -help option loads the module's docstring and because
    # the module name will be used for the filename of the log.
    # TODO: check if the following line is platform-independent
    moduleName = calledModuleName()
    nonGlobalArgs = []
    for arg in args:
        arg = decodeArg(arg)
        if arg == '-help':
            showHelp(moduleName)
            sys.exit(0)
        elif arg.startswith('-dir:'):
            pass # config_dir = arg[5:] // currently handled in wikipediatools.py - possibly before this routine is called.
        elif arg.startswith('-family:'):
            default_family = arg[8:]
        elif arg.startswith('-lang:'):
            default_code = arg[6:]
        elif arg.startswith('-putthrottle:'):
            config.put_throttle = int(arg[len("-putthrottle:") : ])
            put_throttle.setDelay()
        elif arg.startswith('-pt:'):
            config.put_throttle = int(arg[len("-pt:") : ])
            put_throttle.setDelay()
        elif arg == '-log':
            setLogfileStatus(True)
        elif arg.startswith('-log:'):
            setLogfileStatus(True, arg[5:])
        elif arg == '-nolog':
            setLogfileStatus(False)
        elif arg == '-verbose' or arg == "-v":
            verbose += 1
        elif arg == '-daemonize':
            import daemonize
            daemonize.daemonize()
        elif arg.startswith('-daemonize:'):
            import daemonize
            daemonize.daemonize(redirect_std = arg[11:])
        elif arg == '-cosmeticchanges' or arg == '-cc':
            config.cosmetic_changes = not config.cosmetic_changes
            output(u'NOTE: option cosmetic_changes is %s\n' % config.cosmetic_changes)
        # global debug option for development purposes. Normally does nothing.
        elif arg == '-debug':
            debug = True
            config.special_page_limit = 500
        else:
            # the argument is not global. Let the specific bot script care
            # about it.
            nonGlobalArgs.append(arg)

    # TEST for bug #3081100
    if unicode_error and (default_code == 'hi' or moduleName=='interwiki'):
        output("""

================================================================================
\03{lightyellow}WARNING:\03{lightred} your python version might trigger issue #3081100\03{default}
See http://goo.gl/W8lJB for more information.
\03{lightyellow}Use an older python version (<2.6.5) if you are running on wikimedia sites!\03{default}
================================================================================

""")
    if verbose:
      output(u'Pywikipediabot %s' % (version.getversion()))
      output(u'Python %s' % (sys.version))
    return nonGlobalArgs

#########################
# Interpret configuration
#########################

# search for user interface module in the 'userinterfaces' subdirectory
sys.path.append(config.datafilepath('userinterfaces'))
exec "import %s_interface as uiModule" % config.userinterface
ui = uiModule.UI()
verbose = 0
debug = False

# TEST for bug #3081100
unicode_error = __import__('unicodedata').normalize(
    'NFC',
    u'\u092e\u093e\u0930\u094d\u0915 \u091c\u093c\u0941\u0915\u0947\u0930\u092c\u0930\u094d\u0917'
    ) != u'\u092e\u093e\u0930\u094d\u0915 \u091c\u093c\u0941\u0915\u0947\u0930\u092c\u0930\u094d\u0917'
if unicode_error:
    print u'unicode test: triggers problem #3081100'

default_family = config.family
default_code = config.mylang
logfile = None
# Check

# if the default family+wiki is a non-public one,
# getSite will try login in. We don't want that, the module
# is not yet loaded.
getSite(noLogin=True)

# Set socket timeout
socket.setdefaulttimeout(config.socket_timeout)

def writeToCommandLogFile():
    """
    Save the name of the called module along with all parameters to
    logs/commands.log so that the user can look it up later to track errors
    or report bugs.
    """
    modname = os.path.basename(sys.argv[0])
    # put quotation marks around all parameters
    args = [decodeArg(modname)] + [decodeArg('"%s"' % s) for s in sys.argv[1:]]
    commandLogFilename = config.datafilepath('logs', 'commands.log')
    try:
        commandLogFile = codecs.open(commandLogFilename, 'a', 'utf-8')
    except IOError:
        commandLogFile = codecs.open(commandLogFilename, 'w', 'utf-8')
    # add a timestamp in ISO 8601 formulation
    isoDate = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
    commandLogFile.write("%s r%s Python %s "
                         % (isoDate, version.getversiondict()['rev'],
                            sys.version.split()[0]))
    s = u' '.join(args)
    commandLogFile.write(s + os.linesep)
    commandLogFile.close()

def setLogfileStatus(enabled, logname = None):
    global logfile
    if enabled:
        if not logname:
            logname = '%s.log' % calledModuleName()
        logfn = config.datafilepath('logs', logname)
        try:
            logfile = codecs.open(logfn, 'a', 'utf-8')
        except IOError:
            logfile = codecs.open(logfn, 'w', 'utf-8')
    else:
        # disable the log file
        logfile = None

if '*' in config.log or calledModuleName() in config.log:
    setLogfileStatus(True)

writeToCommandLogFile()

colorTagR = re.compile('\03{.*?}', re.UNICODE)

def log(text):
    """Write the given text to the logfile."""
    if logfile:
        # remove all color markup
        plaintext = colorTagR.sub('', text)
        # save the text in a logfile (will be written in utf-8)
        logfile.write(plaintext)
        logfile.flush()

output_lock = threading.Lock()
input_lock = threading.Lock()
output_cache = []
def output(text, decoder = None, newline = True, toStdout = False):
    """Output a message to the user via the userinterface.

    Works like print, but uses the encoding used by the user's console
    (console_encoding in the configuration file) instead of ASCII.
    If decoder is None, text should be a unicode string. Otherwise it
    should be encoded in the given encoding.

    If newline is True, a linebreak will be added after printing the text.

    If toStdout is True, the text will be sent to standard output,
    so that it can be piped to another process. All other text will
    be sent to stderr. See: http://en.wikipedia.org/wiki/Pipeline_%28Unix%29

    text can contain special sequences to create colored output. These
    consist of the escape character \03 and the color name in curly braces,
    e. g. \03{lightpurple}. \03{default} resets the color.

    """
    output_lock.acquire()
    try:
        if decoder:
            text = unicode(text, decoder)
        elif type(text) is not unicode:
            if verbose and sys.platform != 'win32':
                print "DBG> BUG: Non-unicode (%s) passed to wikipedia.output without decoder!" % type(text)
                print traceback.print_stack()
                print "DBG> Attempting to recover, but please report this problem"
            try:
                text = unicode(text, 'utf-8')
            except UnicodeDecodeError:
                text = unicode(text, 'iso8859-1')
        if newline:
            text += u'\n'
        log(text)
        if input_lock.locked():
            cache_output(text, toStdout = toStdout)
        else:
            ui.output(text, toStdout = toStdout)
    finally:
        output_lock.release()

def cache_output(*args, **kwargs):
    output_cache.append((args, kwargs))

def flush_output_cache():
    while(output_cache):
        (args, kwargs) = output_cache.pop(0)
        ui.output(*args, **kwargs)

def input(question, password = False):
    """Ask the user a question, return the user's answer.

    Parameters:
    * question - a unicode string that will be shown to the user. Don't add a
                 space after the question mark/colon, this method will do this
                 for you.
    * password - if True, hides the user's input (for password entry).

    Returns a unicode string.

    """
    input_lock.acquire()
    try:
        data = ui.input(question, password)
    finally:
        flush_output_cache()
        input_lock.release()

    return data

def inputChoice(question, answers, hotkeys, default = None):
    """Ask the user a question with several options, return the user's choice.

    The user's input will be case-insensitive, so the hotkeys should be
    distinctive case-insensitively.

    Parameters:
    * question - a unicode string that will be shown to the user. Don't add a
                 space after the question mark, this method will do this
                 for you.
    * answers  - a list of strings that represent the options.
    * hotkeys  - a list of one-letter strings, one for each answer.
    * default  - an element of hotkeys, or None. The default choice that will
                 be returned when the user just presses Enter.

    Returns a one-letter string in lowercase.

    """
    input_lock.acquire()
    try:
        data = ui.inputChoice(question, answers, hotkeys, default).lower()
    finally:
        flush_output_cache()
        input_lock.release()

    return data

def showHelp(moduleName = None):
    # the parameter moduleName is deprecated and should be left out.
    moduleName = moduleName or sys.argv[0][:sys.argv[0].rindex('.')]
    try:
        moduleName = moduleName[moduleName.rindex("\\")+1:]
    except ValueError: # There was no \ in the module name, so presumably no problem
        pass
    globalHelp =u'''

Global arguments available for all bots:

-dir:PATH         Read the bot's configuration data from directory given by
                  PATH, instead of from the default directory.

-lang:xx          Set the language of the wiki you want to work on, overriding
                  the configuration in user-config.py. xx should be the
                  language code.

-family:xyz       Set the family of the wiki you want to work on, e.g.
                  wikipedia, wiktionary, wikitravel, ...
                  This will override the configuration in user-config.py.

-daemonize:xyz    Immediately returns control to the terminal and redirects
                  stdout and stderr to xyz (only use for bots that require
                  no input from stdin).

-help             Shows this help text.

-log              Enable the logfile. Logs will be stored in the logs
                  subdirectory.

-log:xyz          Enable the logfile, using xyz as the filename.

-nolog            Disable the logfile (if it is enabled by default).

-putthrottle:n    Set the minimum time (in seconds) the bot will wait between
-pt:n             saving pages.

-verbose          Have the bot provide additional output that may be useful in
-v                debugging.

-cosmeticchanges  Toggles the cosmetic_changes setting made in config.py or
-cc               user_config.py to its inverse and overrules it. All other
                  settings and restrictions are untouched.
'''
    output(globalHelp, toStdout = True)
    try:
        exec('import %s as module' % moduleName)
        helpText = module.__doc__.decode('utf-8')
        if hasattr(module, 'docuReplacements'):
            for key, value in module.docuReplacements.iteritems():
                helpText = helpText.replace(key, value.strip('\n\r'))
        output(helpText, toStdout = True)
    except:
        raise
        output(u'Sorry, no help available for %s' % moduleName)

page_put_queue = Queue.Queue(config.max_queue_size)
def async_put():
    """Daemon; take pages from the queue and try to save them on the wiki."""
    while True:
        (page, newtext, comment, watchArticle,
                 minorEdit, force, callback) = page_put_queue.get()
        if page is None:
            # an explicit end-of-Queue marker is needed for compatibility
            # with Python 2.4; in 2.5, we could use the Queue's task_done()
            # and join() methods
            return
        try:
            page.put(newtext, comment, watchArticle, minorEdit, force)
            error = None
        except Exception, error:
            pass
        if callback is not None:
            callback(page, error)
            # if callback is provided, it is responsible for exception handling
            continue
        if isinstance(error, SpamfilterError):
            output(u"Saving page %s prevented by spam filter: %s"
                   % (page.aslink(True), error.url))
        elif isinstance(error, PageNotSaved):
            output(u"Saving page %s failed: %s" % (page.aslink(True), error))
        elif isinstance(error, LockedPage):
            output(u"Page %s is locked; not saved." % page.aslink(True))
        elif isinstance(error, NoUsername):
            output(u"Page %s not saved; sysop privileges required."
                   % page.aslink(True))
        elif error is not None:
            tb = traceback.format_exception(*sys.exc_info())
            output(u"Saving page %s failed:\n%s"
                   % (page.aslink(True), "".join(tb)))

_putthread = threading.Thread(target=async_put)
# identification for debugging purposes
_putthread.setName('Put-Thread')
_putthread.setDaemon(True)
## Don't start the queue if it is not necessary.
#_putthread.start()

def stopme():
    """This should be run when a bot does not interact with the Wiki, or
       when it has stopped doing so. After a bot has run stopme() it will
       not slow down other bots any more.
    """
    get_throttle.drop()

def _flush():
    """Wait for the page-putter to flush its queue.

    Called automatically upon exiting from Python.

    """
    def remaining():
        import datetime
        remainingPages = page_put_queue.qsize() - 1
            # -1 because we added a None element to stop the queue
        remainingSeconds = datetime.timedelta(
            seconds=(remainingPages * put_throttle.getDelay(True)))
        return (remainingPages, remainingSeconds)

    page_put_queue.put((None, None, None, None, None, None, None))

    if page_put_queue.qsize() > 1:
        output(u'Waiting for %i pages to be put. Estimated time remaining: %s'
               % remaining())

    while(_putthread.isAlive()):
        try:
            _putthread.join(1)
        except KeyboardInterrupt:
            answer = inputChoice(u"""\
There are %i pages remaining in the queue. Estimated time remaining: %s
Really exit?"""
                                     % remaining(),
                                 ['yes', 'no'], ['y', 'N'], 'N')
            if answer == 'y':
                return
    try:
        get_throttle.drop()
    except NameError:
        pass
    if config.use_diskcache and not config.use_api:
        for site in _sites.itervalues():
            if site._mediawiki_messages:
                try:
                    site._mediawiki_messages.delete()
                except OSError:
                    pass

import atexit
atexit.register(_flush)

def debugDump(name, site, error, data):
    import time
    name = unicode(name)
    error = unicode(error)
    site = unicode(repr(site).replace(u':',u'_'))
    filename = '%s_%s__%s.dump' % (name, site, time.asctime())
    filename = filename.replace(' ','_').replace(':','-')
    f = file(filename, 'wb') #trying to write it in binary
    #f = codecs.open(filename, 'w', 'utf-8')
    f.write(u'Error reported: %s\n\n' % error)
    try:
        f.write(data.encode("utf8"))
    except UnicodeDecodeError:
        f.write(data)
    f.close()
    output( u'ERROR: %s caused error %s. Dump %s created.' % (name,error,filename) )

get_throttle = Throttle()
put_throttle = Throttle(write=True)

def decompress_gzip(data):
    # Use cStringIO if available
    # TODO: rewrite gzip.py such that it supports unseekable fileobjects.
    if data:
        try:
            from cStringIO import StringIO
        except ImportError:
            from StringIO import StringIO
        import gzip
        try:
            data = gzip.GzipFile(fileobj = StringIO(data)).read()
        except IOError:
            raise
    return data

def parsetime2stamp(tz):
    s = time.strptime(tz, "%Y-%m-%dT%H:%M:%SZ")
    return int(time.strftime("%Y%m%d%H%M%S", s))

#Redirect Handler for urllib2
class U2RedirectHandler(urllib2.HTTPRedirectHandler):

    def redirect_request(self, req, fp, code, msg, headers, newurl):
        newreq = urllib2.HTTPRedirectHandler.redirect_request(self, req, fp, code, msg, headers, newurl)
        if (newreq.get_method() == "GET"):
            for cl in "Content-Length", "Content-length", "content-length", "CONTENT-LENGTH":
                if newreq.has_header(cl):
                    del newreq.headers[cl]
        return newreq

    def http_error_301(self, req, fp, code, msg, headers):
        result = urllib2.HTTPRedirectHandler.http_error_301(
            self, req, fp, code, msg, headers)
        result.code = code
        result.sheaders = [v for v in headers.__str__().split('\n') if v.startswith('Set-Cookie:')]
        return result

    def http_error_302(self, req, fp, code, msg, headers):
        result = urllib2.HTTPRedirectHandler.http_error_302(
            self, req, fp, code, msg, headers)
        result.code = code
        result.sheaders = [v for v in headers.__str__().split('\n') if v.startswith('Set-Cookie:')]
        return result

# Site Cookies handler
COOKIEFILE = config.datafilepath('login-data', 'cookies.lwp')
cj = cookielib.LWPCookieJar()
if os.path.isfile(COOKIEFILE):
    cj.load(COOKIEFILE)

cookieProcessor = urllib2.HTTPCookieProcessor(cj)


MyURLopener = urllib2.build_opener(U2RedirectHandler)

if config.proxy['host']:
    proxyHandler = urllib2.ProxyHandler({'http':'http://%s/' % config.proxy['host'] })

    MyURLopener.add_handler(proxyHandler)
    if config.proxy['auth']:
        proxyAuth = urllib2.HTTPPasswordMgrWithDefaultRealm()
        proxyAuth.add_password(None, config.proxy['host'], config.proxy['auth'][0], config.proxy['auth'][1])
        proxyAuthHandler = urllib2.ProxyBasicAuthHandler(proxyAuth)

        MyURLopener.add_handler(proxyAuthHandler)

if config.authenticate:
    passman = urllib2.HTTPPasswordMgrWithDefaultRealm()
    for site in config.authenticate:
        passman.add_password(None, site, config.authenticate[site][0], config.authenticate[site][1])
    authhandler = urllib2.HTTPBasicAuthHandler(passman)

    MyURLopener.add_handler(authhandler)

MyURLopener.addheaders = [('User-agent', useragent)]

if __name__ == '__main__':
    import doctest
    print 'Pywikipediabot %s' % version.getversion()
    print 'Python %s' % sys.version
    doctest.testmod()

