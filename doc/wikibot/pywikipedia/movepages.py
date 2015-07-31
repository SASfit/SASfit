#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This script can move pages.

These command line parameters can be used to specify which pages to work on:

&params;

Furthermore, the following command line parameters are supported:

-from and -to     The page to move from and the page to move to.

-noredirect       Leave no redirect behind.

-prefix           Move pages by adding a namespace prefix to the names of the
                  pages. (Will remove the old namespace prefix if any)
                  Argument can also be given as "-prefix:namespace:".

-always           Don't prompt to make changes, just do them.

-skipredirects    Skip redirect pages (Warning: increases server load)

-summary          Prompt for a custom summary, bypassing the predefined message
                  texts. Argument can also be given as "-summary:XYZ".

-pairs            Read pairs of file names from a file. The file must be in a
                  format [[frompage]] [[topage]] [[frompage]] [[topage]] ...
                  Argument can also be given as "-pairs:filename"

"""
#
# (C) Leonardo Gregianin, 2006
# (C) Andreas J. Schwab, 2007
# (C) Pywikipedia bot team, 2006-2010
#
# Distributed under the terms of the MIT license.
#

__version__='$Id: movepages.py 8629 2010-10-09 16:11:46Z xqt $'

import sys, re
import wikipedia as pywikibot
import pagegenerators

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;':     pagegenerators.parameterHelp,
}

summary={
    'ar': u'روبوت: نقل الصفحة',
    'cs': u'Robot přesunul stránku',
    'de': u'Bot: Seite verschoben',
    'el': u'Μετακίνηση σελίδων με bot',
    'en': u'Bot: Moved page',
    'fa': u'ربات: صفحه منتقل شد',
    'fi': u'Botti siirsi sivun',
    'fr': u'Bot: Page renommée',
    'frr':u'Bot: Sidj fersköwen',
    'ja': u'ロボットによる: ページの移動',
    'he': u'בוט: מעביר דף',
    'nl': u'Bot: paginanaam gewijzigd',
    'nn': u'robot: flytta sida',
    'pl': u'Przeniesienie artykułu przez robota',
    'pt': u'Bot: Página movida',
    'ru': u'Переименование страницы при помощи робота',
    'uk': u'Робот:перейменування сторінки',
    'zh': u'機器人:移動頁面',
}


class MovePagesBot:
    def __init__(self, generator, addprefix, noredirect, always, skipredirects,
                 summary):
        self.generator = generator
        self.addprefix = addprefix
        self.noredirect = noredirect
        self.always = always
        self.skipredirects = skipredirects
        self.summary = summary

    def moveOne(self, page, newPageTitle):
        try:
            msg = self.summary
            if not msg:
                msg = pywikibot.translate(pywikibot.getSite(), summary)
            pywikibot.output(u'Moving page %s to [[%s]]'
                             % (page.title(asLink=True), newPageTitle))
            page.move(newPageTitle, msg, throttle=True,
                      leaveRedirect=self.noredirect)
        except pywikibot.NoPage:
            pywikibot.output(u'Page %s does not exist!' % page.title())
        except pywikibot.IsRedirectPage:
            pywikibot.output(u'Page %s is a redirect; skipping.' % page.title())
        except pywikibot.LockedPage:
            pywikibot.output(u'Page %s is locked!' % page.title())
        except pywikibot.PageNotSaved, e:
            #target newPageTitle already exists
            pywikibot.output(e.message)

    def treat(self, page):
        # Show the title of the page we're working on.
        # Highlight the title in purple.
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                         % page.title())
        if self.skipredirects and page.isRedirectPage():
            pywikibot.output(u'Page %s is a redirect; skipping.' % page.title())
            return
        pagetitle = page.titleWithoutNamespace()
        namesp = page.site().namespace(page.namespace())
        if self.appendAll:
            newPageTitle = (u'%s%s%s'
                            % (self.pagestart, pagetitle, self.pageend))
            if not self.noNamespace and namesp:
                newPageTitle = (u'%s:%s' % (namesp, newPageTitle))
        elif self.regexAll:
            newPageTitle = self.regex.sub(self.replacePattern, pagetitle)
            if not self.noNamespace and namesp:
                newPageTitle = (u'%s:%s' % (namesp, newPageTitle))
        if self.addprefix:
            newPageTitle = (u'%s%s' % (self.addprefix, pagetitle))
        if self.addprefix or self.appendAll or self.regexAll:
            if not self.always:
                choice2 = pywikibot.inputChoice(
                    u'Change the page title to "%s"?' % newPageTitle,
                    ['yes', 'no', 'all', 'quit'], ['y', 'n', 'a', 'q'])
                if choice2 == 'y':
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'a':
                    self.always = True
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'q':
                    sys.exit()
                elif choice2 == 'n':
                    pass
                else:
                    self.treat(page)
            else:
                self.moveOne(page, newPageTitle)
        else:
            choice = pywikibot.inputChoice(u'What do you want to do?',
                                           ['change page name',
                                            'append to page name',
                                            'use a regular expression',
                                            'next page', 'quit'],
                                           ['c', 'a', 'r', 'n', 'q'])
            if choice == 'c':
                newPageTitle = pywikibot.input(u'New page name:')
                self.moveOne(page, newPageTitle)
            elif choice == 'a':
                self.pagestart = pywikibot.input(u'Append this to the start:')
                self.pageend = pywikibot.input(u'Append this to the end:')
                newPageTitle = (u'%s%s%s'
                                % (self.pagestart, pagetitle, self.pageend))
                if namesp:
                    choice2 = pywikibot.inputChoice(
                        u'Do you want to remove the namespace prefix "%s:"?'
                        % namesp, ['yes', 'no'], ['y', 'n'])
                    if choice2 == 'y':
                        noNamespace = True
                    else:
                        newPageTitle = (u'%s:%s' % (namesp, newPageTitle))
                choice2 = pywikibot.inputChoice(
                    u'Change the page title to "%s"?'
                    % newPageTitle, ['yes', 'no', 'all', 'quit'],
                    ['y', 'n', 'a', 'q'])
                if choice2  == 'y':
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'a':
                    self.appendAll = True
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'q':
                    sys.exit()
                elif choice2 == 'n':
                    pass
                else:
                    self.treat(page)
            elif choice == 'r':
                searchPattern = pywikibot.input(u'Enter the search pattern:')
                self.replacePattern = pywikibot.input(
                    u'Enter the replace pattern:')
                self.regex=re.compile(searchPattern)
                if page.title() == page.titleWithoutNamespace():
                    newPageTitle = self.regex.sub(self.replacePattern,
                                                  page.title())
                else:
                    choice2 = pywikibot.inputChoice(
                        u'Do you want to remove the namespace prefix "%s:"?'
                        % namesp, ['yes', 'no'], ['y', 'n'])
                    if choice2 == 'y':
                        newPageTitle = self.regex.sub(
                            self.replacePattern, page.titleWithoutNamespace())
                        noNamespace = True
                    else:
                        newPageTitle = self.regex.sub(self.replacePattern,
                                                      page.title())
                choice2 = pywikibot.inputChoice(
                    u'Change the page title to "%s"?'
                    % newPageTitle, ['yes', 'no', 'all', 'quit'],
                    ['y', 'n', 'a', 'q'])
                if choice2 == 'y':
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'a':
                    self.regexAll = True
                    self.moveOne(page, newPageTitle)
                elif choice2 == 'q':
                    sys.exit()
                elif choice2 == 'n':
                    pass
                else:
                    self.treat(page)
            elif choice == 'n':
                pass
            elif choice == 'q':
                sys.exit()
            else:
                self.treat(page)

    def run(self):
        self.appendAll = False
        self.regexAll = False
        self.noNamespace = False
        for page in self.generator:
            self.treat(page)


def main():
    gen = None
    prefix = None
    oldName = None
    newName = None
    noredirect = True
    always = False
    skipredirects = False
    summary = None
    fromToPairs = []

    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()

    for arg in pywikibot.handleArgs():
        if arg.startswith('-pairs'):
            if len(arg) == len('-pairs'):
                filename = pywikibot.input(
                    u'Enter the name of the file containing pairs:')
            else:
                filename = arg[len('-pairs:'):]
            oldName1 = None
            for page in pagegenerators.TextfilePageGenerator(filename):
                if oldName1:
                    fromToPairs.append([oldName1, page.title()])
                    oldName1 = None
                else:
                    oldName1 = page.title()
            if oldName1:
                pywikibot.output(
                    u'WARNING: file %s contains odd number of links' % filename)
        elif arg == '-noredirect':
            noredirect = False
        elif arg == '-always':
            always = True
        elif arg == '-skipredirects':
            skipredirects = True
        elif arg.startswith('-from:'):
            if oldName:
                pywikibot.output(u'WARNING: -from:%s without -to:' % oldName)
            oldName = arg[len('-from:'):]
        elif arg.startswith('-to:'):
            if oldName:
                fromToPairs.append([oldName, arg[len('-to:'):]])
                oldName = None
            else:
                pywikibot.output(u'WARNING: %s without -from' % arg)
        elif arg.startswith('-prefix'):
            if len(arg) == len('-prefix'):
                prefix = pywikibot.input(u'Enter the prefix:')
            else:
                prefix = arg[8:]
        elif arg.startswith('-summary'):
            if len(arg) == len('-summary'):
                summary = pywikibot.input(u'Enter the summary:')
            else:
                summary = arg[9:]
        else:
            genFactory.handleArg(arg)

    if oldName:
        pywikibot.output(u'WARNING: -from:%s without -to:' % oldName)
    for pair in fromToPairs:
        page = pywikibot.Page(pywikibot.getSite(), pair[0])
        bot = MovePagesBot(None, prefix, noredirect, always, skipredirects,
                           summary)
        bot.moveOne(page, pair[1])

    if not gen:
        gen = genFactory.getCombinedGenerator()
    if gen:
        preloadingGen = pagegenerators.PreloadingGenerator(gen)
        bot = MovePagesBot(preloadingGen, prefix, noredirect, always,
                           skipredirects, summary)
        bot.run()
    elif not fromToPairs:
        pywikibot.showHelp('movepages')

if __name__ == '__main__':
    try:
        main()
    finally:
        pywikibot.stopme()
