# -*- coding: utf-8 -*-
"""
Allows access to the site's bot user list.

The function refresh() downloads the current bot user list and saves
it to disk. It is run automatically when a bot first tries to get this
data.
"""

# (C) Daniel Herding, 2005
# (C) Dr. Trigon, 2009-2010
#
# DrTrigonBot: http://de.wikipedia.org/wiki/Benutzer:DrTrigonBot
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: botlist.py 9042 2011-03-13 10:14:47Z xqt $'
#

import re, sys, pickle
import os.path
import time
import wikipedia as pywikibot

cache = {}

def get(site = None):
    if site is None:
        site = pywikibot.getSite()
    if site in cache:
        # Use cached copy if it exists.
        botlist = cache[site]
    else:
        fn = pywikibot.config.datafilepath('botlists',
                  'botlist-%s-%s.dat' % (site.family.name, site.lang))
        try:
            # find out how old our saved dump is (in seconds)
            file_age = time.time() - os.path.getmtime(fn)
            # if it's older than 1 day, reload it
            if file_age > 1 * 24 * 60 * 60:
                pywikibot.output(u'Copy of bot user list is one day old, reloading')
                refresh(site)
        except OSError:
            # no saved botlist exists yet, retrieve one
            refresh(site)
        f = open(fn, 'r')
        botlist = pickle.load(f)
        f.close()
        # create cached copy
        cache[site] = botlist
    return botlist

def isBot(user, site=None):
    botlist = get(site)
    return user in botlist

def refresh(site, sysop=False, witheditsonly=True):
    #if not site.has_api() or site.versionnumber() < 10:
    #    _refreshOld(site)

    # get botlist special page's URL
    if not site.loggedInAs(sysop=sysop):
        site.forceLogin(sysop=sysop)

    params = {
        'action': 'query',
        'list': 'allusers',
        'augroup': 'bot',
    }
    if witheditsonly:
        params['auwitheditsonly'] = ''

    pywikibot.output(u'Retrieving bot user list for %s via API.' % repr(site))
    pywikibot.put_throttle() # It actually is a get, but a heavy one.
    botlist = []
    while True:
        data = pywikibot.query.GetData(params, site, sysop=sysop)
        if 'error' in data:
            raise RuntimeError('ERROR: %s' % data)
        botlist.extend([w['name'] for w in data['query']['allusers']])

        if 'query-continue' in data:
            params['aufrom'] = data['query-continue']['allusers']['aufrom']
        else:
            break

    pywikibot.output(u'Retrieving global bot user list for %s.' % repr(site))
    pywikibot.put_throttle() # It actually is a get, but a heavy one.
    m1 = True
    offset = ''
    if site.versionnumber() >= 17:
        PATTERN = u'<li>(.*?) *\((.*?),\s(.*?)\)(?:.*?)</li>'
    else:
        PATTERN = u'<li>(.*?) *\((.*?),\s(.*?)\)</li>'
    while m1:
        text = site.getUrl(site.globalusers_address(offset=offset, group='Global_bot'))

        m1 = re.findall(u'<li>.*?</li>', text)
        for item in m1:
            m2 = re.search(PATTERN, item)
            (bot, flag_local, flag_global) = m2.groups()
            flag_local  = (flag_local[:2] == u'<a')
            flag_global = True # since group='Global_bot'

            if bot not in botlist:
                botlist.append( bot )

        #print len(botlist)
        offset = bot.encode(site.encoding())

    # Save the botlist to disk
    # The file is stored in the botlists subdir. Create if necessary.
    if sysop:
        f = open(pywikibot.config.datafilepath('botlists',
                 'botlist-%s-%s-sysop.dat' % (site.family.name, site.lang)), 'w')
    else:
        f = open(pywikibot.config.datafilepath('botlists',
                 'botlist-%s-%s.dat' % (site.family.name, site.lang)), 'w')
    pickle.dump(botlist, f)
    f.close()

#def refresh_all(new = False, sysop=False):
#    if new:
#        import config
#        pywikibot.output('Downloading All bot user lists for your accounts in user-config.py');
#        for family in config.usernames:
#            for lang in config.usernames[ family ]:
#                refresh(pywikibot.getSite( code = lang, fam = family ), sysop=sysop )
#        for family in config.sysopnames:
#            for lang in config.sysopnames[ family ]:
#                refresh(pywikibot.getSite( code = lang, fam = family ), sysop=sysop )
#
#    else:
#        import dircache, time
#        filenames = dircache.listdir(pywikibot.config.datafilepath('botlists'))
#        botlist_filenameR = re.compile('botlist-([a-z\-:]+).dat')
#        for filename in filenames:
#            match = botlist_filenameR.match(filename)
#            if match:
#                arr = match.group(1).split('-')
#                family = arr[0]
#                lang = '-'.join(arr[1:])
#                refresh(pywikibot.getSite(code = lang, fam = family))
#
#def main():
#    all = False
#    new = False
#    sysop = False
#    for arg in pywikibot.handleArgs():
#        if arg == '-all' or arg == '-update':
#            all = True
#        elif arg == '-new':
#            new = True
#        elif arg == '-sysop':
#            sysop = True
#    if all:
#        refresh_all(sysop=sysop)
#    elif new:
#        refresh_all(new, sysop=sysop)
#    else:
#        refresh(pywikibot.getSite(), sysop=sysop)
#
#        botlist = get(pywikibot.getSite())
#        pywikibot.output(u'%i pages in the bot user list.' % len(botlist))
#        for pageName in botlist:
#            pywikibot.output( pageName, toStdout = True )
#
#if __name__ == "__main__":
#    try:
#        main()
#    finally:
#        pywikibot.stopme()


