#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This is a script to import pages from a certain wiki to another.

This requires administrator privileges.

Here there is an example of how to use it:

from pageimport import *
def main():
    # Defing what page to load..
    pagetoload = 'Apple'
    site = wikipedia.getSite()
    importerbot = Importer(site) # Inizializing
    importerbot.Import(pagetoload, prompt = True)
try:
    main()
finally:
    wikipedia.stopme()
"""
#
# (C) Filnik, 2007
# (C) Pywikipedia bot team, 2008-2010
#
# Greetings:
# Lorenzo Paulatto and Misza13
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: pageimport.py 8629 2010-10-09 16:11:46Z xqt $'
#

import urllib
import wikipedia as pywikibot,
import login, config

class Importer(pywikibot.Page):
    def __init__(self, site):
        self.importsite = site
        pywikibot.Page.__init__(self, site, 'Special:Import', None, 0)

    def Import(self, target, project = 'w', crono = '1', namespace = '', prompt = True):
        """Import the page from the wiki. Requires administrator status.
        If prompt is True, asks the user if he wants to delete the page.
        """
        if project == 'w':
            site = pywikibot.getSite(fam = 'wikipedia')
        elif project == 'b':
            site = pywikibot.getSite(fam = 'wikibooks')
        elif project == 'wikt':
            site = pywikibot.getSite(fam = 'wiktionary')
        elif project == 's':
            site = pywikibot.getSite(fam = 'wikisource')
        elif project == 'q':
            site = pywikibot.getSite(fam = 'wikiquote')
        else:
            site = pywikibot.getSite()
        # Fixing the crono value...
        if crono == True:
            crono = '1'
        elif crono == False:
            crono = '0'
        # Fixing namespace's value.
        if namespace == '0':
            namespace == ''
        answer = 'y'
        if prompt:
            answer = pywikibot.inputChoice(u'Do you want to import %s?' % target, ['Yes', 'No'], ['y', 'N'], 'N')
        if answer == 'y':
            host = self.site().hostname()
            address = self.site().path() + '?title=%s&action=submit' % self.urlname()
            # You need to be a sysop for the import.
            self.site().forceLogin(sysop = True)
            # Getting the token.
            token = self.site().getToken(self, sysop = True)
            # Defing the predata.
            predata = {
                'action' : 'submit',
                'source' : 'interwiki',
                # from what project do you want to import the page?
                'interwiki' : project,
                # What is the page that you want to import?
                'frompage' : target,
                # The entire history... or not?
                'interwikiHistory' : crono,
                # What namespace do you want?
                'namespace': '',
            }
            response, data = self.site().postForm(address, predata, sysop = True)
            if data:
                pywikibot.output(u'Page imported, checking...')
                if pywikibot.Page(self.importsite, target).exists():
                    pywikibot.output(u'Import success!')
                    return True
                else:
                    pywikibot.output(u'Import failed!')
                    return False

if __name__=='__main__':
    pywikibot.output(u'This is just a module! Read the documentation and write your own script!')
    pywikibot.stopme()
