#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This library allows the use of the pywikipediabot directly from COM-aware
applications.

Calling this class from Visual Basic:
    set wiki = CreateObject("Mediawiki.Wiki")
    set site = wiki.getSite("en")
    print site.loggedin()
    set page = wiki.getPage(site, "User:Yurik/sandbox1")
    txt = page.get()
    print txt
    res = page.put("new text 1", "testing bot")
"""
#
# (C) Yuri Astrakhan, 2006
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: wikicomserver.py 8499 2010-09-08 14:42:19Z xqt $'


#
#
#   IMPORTANT!!!   Make sure this points to the pywikipedia installation
#                  directory
#
#
pywikipediaDir = "c:\\Projects\\Personal\\wiki\\pywikipedia"


import sys, os
from win32com.server.util import wrap, unwrap
import win32com.client

# Although we are able to register our Parent object for debugging,
# our Child object is not registered, so this won't work. To get
# the debugging behavior for our wrapped objects, we must do it ourself.
debugging = 1
if debugging:
    from win32com.server.dispatcher import DefaultDebugDispatcher
    useDispatcher = DefaultDebugDispatcher
else:
    useDispatcher = None

currDir = os.getcwdu()
os.chdir(pywikipediaDir)
import wikipedia

class Wiki:
    _reg_clsid_ = "{AEA5AC14-ED3D-42E9-AACF-871ED9D95346}"
    _reg_desc_ = "Mediawiki Wiki"
    _reg_progid_ = "Mediawiki.Wiki"
    _public_methods_ = ['getSite', 'getPage']
    _public_attrs_ = ['objectVer', 'objectName', 'argv', 'path']
    _readonly_attrs_ = _public_attrs_

    def __init__(self):
        os.chdir(pywikipediaDir)
        self.objectVer = __version__
        self.objectName = "Wiki"
        self.argv = sys.argv
        self.path = os.path.realpath( self.argv[0] )

    def getSite(self, code = None, fam = None, user = None):
        os.chdir(pywikipediaDir)
        site = wikipedia.getSite(code, fam, user)
        site.objectVer = wikipedia.__version__
        site.objectName = "WikiSite"
        site._public_methods_ = ['__cmp__', '__repr__', 'allmessages_address',
                                 'allpages', 'allpages_address', 'ancientpages',
                                 'ancientpages_address',
                                 'broken_redirects_address', 'categories',
                                 'categories_address', 'category_namespace',
                                 'category_namespaces', 'category_on_one_line',
                                 'checkCharset', 'cookies', 'deadendpages',
                                 'deadendpages_address', 'delete_address',
                                 'double_redirects_address', 'edit_address',
                                 'encoding', 'encodings', 'export_address',
                                 'family', 'forceLogin', 'getSite', 'getToken',
                                 'getUrl', 'get_address', 'hostname',
                                 'image_namespace', 'interwiki_putfirst',
                                 'interwiki_putfirst_doubled', 'language',
                                 'languages', 'linkto', 'loggedin',
                                 'login_address', 'lonelypages',
                                 'lonelypages_address', 'longpages',
                                 'longpages_address', 'namespace',
                                 'namespaces', 'newpages', 'newpages_address',
                                 'purge_address', 'putToken', 'put_address',
                                 'redirect', 'redirectRegex',
                                 'references_address', 'shortpages',
                                 'shortpages_address', 'sitename',
                                 'template_namespace',
                                 'uncategorizedcategories',
                                 'uncategorizedcategories_address',
                                 'uncategorizedpages',
                                 'uncategorizedpages_address',
                                 'unusedcategories', 'unusedcategories_address',
                                 'upload_address', 'version',
                                 'watchlist_address']
        site._public_attrs_ = ['objectVer', 'objectName']
        site._readonly_attrs_ = site._public_attrs_
        return wrap(site, useDispatcher=useDispatcher)

    def getPage(self, site, title):
        os.chdir(pywikipediaDir)
        siteObj = unwrap(site)
        page = WikiPage(siteObj, title)
        return wrap(page)

    def __del__(self):
        os.chdir(currDir)
        print "ChDir to original ", currDir

class WikiPage(wikipedia.Page):
    _reg_clsid_ = "{318CC152-D2A9-4C11-BA01-78B9B91DBDDE}"
    _reg_desc_ = "Mediawiki Wiki Page"
    _reg_progid_ = "Mediawiki.WikiPage"
    _public_methods_ = ['__cmp__', '__repr__', '__str__', 'aslink',
                        'autoFormat', 'canBeEdited', 'categories',
                        'contributingUsers', 'delete', 'encoding',
                        'exists', 'get', 'getEditPage', 'getRedirectTarget',
                        'getReferences', 'getVersionHistory',
                        'getVersionHistoryTable', 'imagelinks', 'interwiki',
                        'isAutoTitle', 'isCategory', 'isDisambig', 'isEmpty',
                        'isImage', 'isRedirectPage', 'isTalkPage',
                        'linkedPages', 'namespace', 'permalink', 'put',
                        'putPage', 'section', 'sectionFreeTitle', 'site',
                        'toggleTalkPage', 'templates', 'title',
                        'titleWithoutNamespace', 'urlname']
    _public_attrs_ = ['objectVer', 'objectName']
    _readonly_attrs_ = _public_attrs_

    def __init__(self, site, title):
        wikipedia.Page.__init__(self, site, title)
        self.objectVer = __version__
        self.objectName = "WikiPage"

if __name__=='__main__':
    import win32com.server.register
    try:
        win32com.server.register.UseCommandLine(Wiki)
    finally:
        wikipedia.stopme()
