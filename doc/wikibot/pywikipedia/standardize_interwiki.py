#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Loop over all pages in the home wiki, standardizing the interwiki links.

Parameters:

-start:     - Set from what page you want to start
"""
#
# (C) Rob W.W. Hooft, 2003
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: standardize_interwiki.py 8925 2011-02-05 22:40:10Z amir $'
#

import os, sys
import difflib
import wikipedia as pywikibot
import config

# The summary that the Bot will use.
comment = {
    'ar':u'روبوت: توحيد قياسي للإنترويكي',
    'cs':u'Standadizace interwiki',
    'de':u'Bot: Interwikilinks standardisieren',
    'en':u'Robot: Interwiki standardization',
    'fa':u'ربات: تصحیح جایگذاری میان‌ویکی‌ها',
    'fr':u'Robot : Standardisation des interwikis',
    'he':u'בוט: מסדר את האינטרוויקי',
    'it':u'Bot: Standardizzo interwiki',
    'ja':u'ロボットによる: 言語間リンクを標準化',
    'ml':u'യന്ത്രം: അന്തർവിക്കി ക്രമവൽക്കരണം',
    'nl':u'Bot: standaardisatie interwikiverwijzingen',
    'no':u'bot: Språklenkestandardisering',
    'ksh':u'Bot: Engerwiki Lengks opprüühme',
    'nds':u'Bot: Links twüschen Wikis standardisseern',
    'zh':u'機器人: 跨語連結標準化',
    }

# Some parameters
options = list()
start = list()
filelist = list()
hints = {}
debug = 0
start = '!'
nothing = False

# Load the default parameters and start
for arg in pywikibot.handleArgs():
    if arg.startswith('-start'):
        if len(arg) == 6:
            start = unicode(pywikibot.input(
                u'From what page do you want to start?'))
        else:
            start = unicode(arg[7:])

site = pywikibot.getSite()
comm = pywikibot.translate(site, comment)

# What follows is the main part of the code.
try:
    for pl in site.allpages(start):
        plname = pl.title()
        pywikibot.output(u'\nLoading %s...' % plname)
        try:
            oldtext = pl.get()
        except pywikibot.IsRedirectPage:
            pywikibot.output(u"%s is a redirect!" % plname)
            continue
        old = pl.interwiki()
        new = {}
        for pl2 in old:
            new[pl2.site()] = pl2
        newtext = pywikibot.replaceLanguageLinks(oldtext, new)
        if new:
            if oldtext != newtext:
                pywikibot.showDiff(oldtext, newtext)
                # Submit changes
                try:
                    status, reason, data = pl.put(newtext, comment=comm)
                    if str(status) != '302':
                        pywikibot.output(status, reason)
                except pywikibot.LockedPage:
                    pywikibot.output(u"%s is locked" % plname)
                    continue
            else:
                pywikibot.output(u'No changes needed.')
                continue
        else:
            pywikibot.output(u'No interwiki found.')
            continue
finally:
    pywikibot.stopme()
