#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Tool to read all your talk pages.

This tool will go through all the normal (not sysop) accounts configured in user-config and output the contents of the talk page.

TODO:
*Error checking
"""
import sys, re
sys.path.append(re.sub('/[^/]*$', '', sys.path[0]))
sys.path.append('..')
import wikipedia, config, userlib


def readtalk(lang, familyName, sysop = False):
    site = wikipedia.getSite(code=lang, fam=familyName)
    if sysop:
        user = userlib.User(site, config.sysopnames[familyName][lang])
    else:
        user = userlib.User(site, config.usernames[familyName][lang])
    page = user.getUserTalkPage()
    if not site.loggedInAs(sysop):
        site.forceLogin()
    if site.messages(sysop):
        wikipedia.output("cleanning up the account new message notice")
        pagetext = site.getUrl(site.get_address(page.urlname()), sysop=sysop)
        del pagetext
    wikipedia.output(u'Reading talk page from %s' % user)
    try:
        wikipedia.output( page.get(get_redirect=True)+"\n")
    except wikipedia.NoPage:
        wikipedia.output("Talk page is not exist.")
    except wikipedia.UserBlocked:
        wikipedia.output("Account is blocked.")

def main():
    # Get a dictionary of all the usernames
    all = sysop = False

    for arg in wikipedia.handleArgs():
        if arg.startswith('-all'):
            all = True
        elif arg.startswith('-sysop'):
            sysop = True
    if all == True:
        if sysop:
            namedict = config.sysopnames
        else:
            namedict = config.usernames
        for familyName in namedict.iterkeys():
            for lang in namedict[familyName].iterkeys():
                readtalk(lang, familyName, sysop)
    else:
        readtalk(wikipedia.default_code, wikipedia.default_family, sysop)

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()

