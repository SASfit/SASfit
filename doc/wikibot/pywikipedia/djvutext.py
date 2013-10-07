#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This bot uploads text from djvu files onto pages in the "Page"
namespace.  It is intended to be used for Wikisource.

The following parameters are supported:

    -dry           If given, doesn't do any real changes, but only shows
                   what would have been changed.
    -ask           Ask for confirmation before uploading each page.
                   (Default: ask when overwriting pages)
    -djvu:...      Filename of the djvu file
    -index:...     Name of the index page
                   (Default: the djvu filename)
    -pages:<start>-<end> Page range to upload; <end> is optional

All other parameters will be regarded as part of the title of a single page,
and the bot will only work on that single page.
"""
#
# (C) Pywikipedia bot team, 2008-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: djvutext.py 9246 2011-05-15 15:42:46Z xqt $'
import wikipedia as pywikibot
import os, sys
import config, codecs

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
}


class DjVuTextBot:
    # Edit summary message that should be used.
    # NOTE: Put a good description here, and add translations, if possible!
    msg = {
        'ar': u'روبوت: إنشاء صفحة بنص مستخرج من DjVu',
        'en': u'Robot: Creating page with text extracted from DjVu',
        'fa': u'ربات: ایجاد صفحه با متنی که از دژاوو استخراج شد',
        'fr': u'Bot: Creating page with texte extracted from DjVu',
        'nl': u'Bot: pagina aangemaakt met tekst geëxtraheerd uit DjVu-bestand',
        'pt': u'Bot: criando página com texto extraído do DjVu',
    }

    def __init__(self, djvu, index, pages, ask=False, debug=False):
        """
        Constructor. Parameters:
        djvu : filename
        index : page name
        pages : page range
        """
        self.djvu = djvu
        self.index = index
        self.pages = pages
        self.dry = debug
        self.ask = ask

    def NoOfImages(self):
        cmd = u"djvused -e 'n' \"%s\"" % (self.djvu)
        count = os.popen( cmd.encode(sys.stdout.encoding) ).readline().rstrip()
        count = int(count)
        pywikibot.output("page count = %d" % count)
        return count

    def PagesGenerator(self):
        start = 1
        end = self.NoOfImages()

        if self.pages:
            pos = self.pages.find('-')
            if pos != -1:
                start = int(self.pages[:pos])
                if pos < len(self.pages)-1:
                    end = int(self.pages[pos+1:])
            else:
                start = int(self.pages)
                end = start
        pywikibot.output(u"Processing pages %d-%d" % (start, end))
        return range(start, end+1)

    def run(self):
        # Set the edit summary message
        pywikibot.setAction(pywikibot.translate(pywikibot.getSite(), self.msg))

        linkingPage = pywikibot.Page(pywikibot.getSite(), self.index)
        self.prefix = linkingPage.titleWithoutNamespace()
        if self.prefix[0:6] == 'Liber:':
            self.prefix = self.prefix[6:]
        pywikibot.output(u"Using prefix %s" % self.prefix)
        gen = self.PagesGenerator()

        site = pywikibot.getSite()
        self.username = config.usernames[site.family.name][site.lang]

        for pageno in gen:
            pywikibot.output("Processing page %d" % pageno)
            self.treat(pageno)

    def has_text(self):
        cmd = u"djvudump \"%s\" > \"%s\".out" % (self.djvu, self.djvu)
        os.system ( cmd.encode(sys.stdout.encoding) )
        f = codecs.open(u"%s.out" % self.djvu, 'r',
                        config.textfile_encoding, 'replace')
        s = f.read()
        f.close()
        return s.find('TXTz') >= 0

    def get_page(self, pageno):
        pywikibot.output(unicode("fetching page %d" % (pageno)))
        cmd = u"djvutxt --page=%d \"%s\" \"%s.out\"" \
              % (pageno, self.djvu, self.djvu)
        os.system ( cmd.encode(sys.stdout.encoding) )
        f = codecs.open(u"%s.out" % self.djvu, 'r',
                        config.textfile_encoding, 'replace')
        djvu_text = f.read()
        f.close()
        return djvu_text

    def treat(self, pageno):
        """
        Loads the given page, does some changes, and saves it.
        """
        site = pywikibot.getSite()
        page_namespace = site.family.namespaces[104][site.lang]
        page = pywikibot.Page(site, u'%s:%s/%d'
                              % (page_namespace, self.prefix, pageno))
        exists = page.exists()
        djvutxt = self.get_page(pageno)

        if not djvutxt:
            text = u'<noinclude><pagequality level="0" user="%s" /><div class="pagetext">\n\n\n</noinclude><noinclude><references/></div></noinclude>' % (self.username)
        else:
            text = u'<noinclude><pagequality level="1" user="%s" /><div class="pagetext">\n\n\n</noinclude>%s<noinclude><references/></div></noinclude>' % (self.username,djvutxt)

            # convert to wikisyntax
            # this adds a second line feed, which makes a new paragraph
            text = text.replace('', "\n") # US /x1F
            text = text.replace('', "\n") # GS /x1D
            text = text.replace('', "\n") # FF /x0C

        # only save if something was changed
        # automatically ask if overwriting an existing page
        ask = self.ask

        if exists:
            ask = True
            old_text = page.get()
            if old_text == text:
                pywikibot.output(u"No changes were needed on %s" % page.aslink())
                return
        else:
            old_text = ''
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                         % page.title())
        pywikibot.showDiff(old_text, text)
        if self.dry:
            pywikibot.inputChoice(u'Dry mode... Press enter to continue', [],
                                  [], 'dummy')
            return
        if ask:
            choice = pywikibot.inputChoice(u'Do you want to accept these changes?', ['Yes', 'No'], ['y', 'N'], 'N')
        else:
            choice = 'y'
        if choice == 'y':
            try:
                # Save the page
                page.put_async(text)
            except pywikibot.LockedPage:
                pywikibot.output(u"Page %s is locked; skipping." % page.aslink())
            except pywikibot.EditConflict:
                pywikibot.output(u'Skipping %s because of edit conflict' % (page.title()))
            except pywikibot.SpamfilterError, error:
                pywikibot.output(u'Cannot change %s because of spam blacklist entry %s' % (page.title(), error.url))


def main():
    import os
    index = None
    djvu = None
    pages = None
    # what would have been changed.
    dry = False
    ask = False

    # Parse command line arguments
    for arg in pywikibot.handleArgs():
        if arg.startswith("-dry"):
            dry = True
        elif arg.startswith("-ask"):
            ask = True
        elif arg.startswith("-djvu:"):
            djvu = arg[6:]
        elif arg.startswith("-index:"):
            index = arg[7:]
        elif arg.startswith("-pages:"):
            pages = arg[7:]
        else:
            pywikibot.output(u"Unknown argument %s" % arg)

    # Check the djvu file exists
    if djvu:
        os.stat(djvu)

        if not index:
            import os.path
            index = os.path.basename(djvu)

    if djvu and index:
        site = pywikibot.getSite()
        index_page = pywikibot.Page(site, index)

        if site.family.name != 'wikisource':
            raise pywikibot.PageNotFound(u"Found family '%s'; Wikisource required." % site.family.name)

        if not index_page.exists() and index_page.namespace() == 0:
            index_namespace = pywikibot.Page(site, 'MediaWiki:Proofreadpage index namespace').get()

            index_page = pywikibot.Page(pywikibot.getSite(),
                                        u"%s:%s" % (index_namespace, index))
        if not index_page.exists():
            raise pywikibot.NoPage(u"Page '%s' does not exist" % index)
        pywikibot.output(u"uploading text from %s to %s" % (djvu, index_page.aslink()) )
        bot = DjVuTextBot(djvu, index, pages, ask, dry)
        if not bot.has_text():
            raise ValueError("No text layer in djvu file")
        bot.run()
    else:
        pywikibot.showHelp()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
