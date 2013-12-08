#!/usr/bin/python
"""
##################################################
This script with all its function has been merged
to login.py. please use:

  login.py -test

xqt 2009-10-26
##################################################

Script to test whether you are logged-in

Parameters:

   -all         Try to test on all sites where a username is defined in
                user-config.py.
   -sysop       test your sysop account. (Works only with -all)
"""
#
# (C) Rob W.W. Hooft, 2003
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: test.py 7543 2009-10-26 07:35:10Z xqt $'
#
import re,sys,wikipedia,config

def show (mysite, sysop = False):
    if mysite.loggedInAs(sysop = sysop):
        wikipedia.output(u"You are logged in on %s as %s." % (repr(mysite), mysite.loggedInAs(sysop=sysop)))
    else:
        wikipedia.output(u"You are not logged in on %s." % repr(mysite))

def main():
    testall = False
    sysop   = False
    for arg in wikipedia.handleArgs():
        if arg == "-all":
            testall = True
        elif arg == "-sysop":
            sysop = True
        else:
            wikipedia.showHelp()
            return
    if testall:
        if sysop:
            namedict = config.sysopnames
        else:
            namedict = config.usernames
        for familyName in namedict.iterkeys():
            for lang in namedict[familyName].iterkeys():
                 show(wikipedia.getSite(lang, familyName), sysop)
    else:
        show(wikipedia.getSite(), sysop)

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()
