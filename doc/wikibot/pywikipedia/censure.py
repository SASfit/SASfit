#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""Bad word checker bot
Should not be run manually/directly, but automatically by maintainer.py
Warning: experimental software, use at your own risk
"""

__version__ = '$Id: censure.py 9042 2011-03-13 10:14:47Z xqt $'

# Author: Balasyum
# http://hu.wikipedia.org/wiki/User:Balasyum

import wikipedia as pywikibot
import sys
import thread

# The indexes for projects are as: <language code>.<family>

# The page, where the bot logs to

logPages = {
    'hu.wikipedia': u'Wikipédia:Potenciálisan vandalizmus áldozatául esett szócikkek',
    }

# To add a new language, create or find the bad word page
# similarly to the 'hu.wikipedia' one (one word per line, starting with <pre> and ending with </pre> lines),
# and add to the badWordList lines below.

badWordList = {
    'hu.wikipedia': u'User:Cenzúrabot/lista',
    }

site = pywikibot.getSite()
if not (site.language() + '.' + site.family.name) in badWordList or not (site.language() + '.' + site.family.name) in logPages:
    pywikibot.output('Error: your language isn\'t supported, see the source code for further details')
    sys.exit(1)
ownWordPage = pywikibot.Page(site, badWordList[site.language() + '.' + site.family.name])
try:
    ownWordList = ownWordPage.get(get_redirect = True)
except pywikibot.NoPage:
    pywikibot.output('Error: the page containing the bad word list of your language doesn\'t exist')
    sys.exit(1)
ownWordList = ownWordList.split('\n')
del ownWordList[0]
del ownWordList[len(ownWordList) - 1]

def seekbpos(str1, str2):
        i = 0
        while i < len(str1):
                if str1[i] != str2[i]:
                        return i
                i += 1
        return i

def seekepos(str1, str2, bpos):
        i1 = len(str1) - 1
        i2 = len(str2) - 1
        while i1 > -1 and i2 > -1:
                if i1 == bpos:
                        return i2
                elif i1 < bpos or str1[i1] != str2[i2]:
                        return i2 + 1
                i1 -= 1
                i2 -= 1
        return -1

def checkPage(title, onlyLastDiff = False):
    if title == logPages[site.language() + '.' + site.family.name]:
        return
    pywikibot.output(u'Checking %s for bad word list' %title)
    page = pywikibot.Page(site, title)
    try:
        text = page.get()
        if onlyLastDiff:
            try:
                oldver = page.getOldVersion(page.previousRevision())
            except IndexError:
                pywikibot.output(u'Page %s has no version history, skipping' %title)
                return
            if len(text) > len(oldver):
                bpos = seekbpos(oldver, text)
                epos = seekepos(oldver, text, bpos)
                diff = text[bpos:epos]
                text = diff
    except pywikibot.NoPage:
        pywikibot.output(u'Page %s doesn\'t exist, skipping' %title)
        return
    except pywikibot.IsRedirectPage:
        pywikibot.output(u'Page %s is a redirect, skipping' %title)
        return

    report = False
    wordsIn = []
    for badWord in ownWordList:
        if (' ' + badWord + ' ') in text:
            wordsIn.append(badWord)
            report = True
    if report:
        logPage = pywikibot.Page(site, logPages[site.language() + '.' + site.family.name])
        try:
            log = logPage.get()
        except:
            pass
        pywikibot.output(u'%s matches the bad word list' %title)
        log = '* [' + page.permalink()+ ' ' + title + '] - ' + ' '.join(wordsIn) + '\n' + log
        logPage.put(log, title)
    else:
        pywikibot.output(u'%s doesn\'t match any of the bad word list' %title)

def main():
    pywikibot.output('Warning: this script should not be run manually/directly, but automatically by maintainer.py')
    if len(sys.argv) == 1:
        pywikibot.output("Usage: censure.py <article title>")
        sys.exit(1)
    del sys.argv[0]
    checkPage(' '.join(sys.argv).decode('utf-8'))

if __name__ == "__main__":
    main()
