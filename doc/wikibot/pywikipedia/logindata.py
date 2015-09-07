# -*- coding: utf-8  -*-

# Usable example module: Use of pywikipedia as a
# library.
#
# Looks up the path to pywikipedia (pywikipedia_path)
# in a settings.py file. You'll need to provide that,
# and/or refactor.

# (C) Kim Bruning for Wikiation, sponsored by Kennisnet, 2009
#
# Distributed under the terms of the MIT license.
#

import sys, os
import settings
if settings.pywikipedia_path not in sys.path:
    sys.path.append(settings.pywikipedia_path)

# pywikipedia can only set itself up if everything is
# done in its own directory. This needs fixing sometime.
# for now, we live with it.
cwd=os.getcwd()
os.chdir(settings.pywikipedia_path)
import wikipedia as pywikibot
import login
from simple_family import Family
os.chdir(cwd)

class LoginData:
    """An example class that uses pywikipedia as a library.
    usage example:

    from logindata import LoginData, pywikibot
    target_wiki=LoginData( ... ) # for example, fill in from a settings file, or use code to generate, or ...
    site=target_wiki.login()
    page=pywikibot.Page(site,"Main Page")
    """

    def __init__(
        self,
        name='MY_NAME_FOR_THIS_SERVER',
        protocol='http',
        server='www.my_server.com',
        scriptpath='/my/script/path/',
        version='1.13.2',
        lang='en',
        encoding='utf-8',
        user='MY_BOT_USER',
        password='MY_SECRET_PASSWORD',
        RversionTab=None,
        api_supported=False
        ):
        """
        paramaters:
        name: arbitrary name. Pick something easy to remember
        protocol: http|https
        server: dns address or ip address
        scriptpath: path on server itself
            (ie:  protocol:server/scriptpath   http://6.wikiation.nl/revisions/REL1.13.2)
        version: mediawiki version of the target mediawiki instance
        lang: default language, as configured on target mediawiki instance
        encoding: should (almost) always be utf-8
        api_supported: Does this mediawiki instance support the mediawiki api?
        RversionTab: Magic. See superclass for information.
        user: Username on wiki
        password: password for this user
        """

        self.lang=lang
        self.user=user
        self.password=password
        self.family=base_family.Family(
            name=name,
            protocol=protocol,
            server=server,
            scriptpath=scriptpath,
            version=version,
            lang=lang,
            encoding=encoding,
            RversionTab=RversionTab,
            api_supported=api_supported)
        self.site=None

    def login(self):
        """Attempt to log in on the site described
        by this class. Returns a pywikipedia site object"""
        self.site=pywikibot.Site(
            code=self.lang,
            fam=self.family,
            user=self.user
            )
        loginManager=login.LoginManager(
            password=self.password,
            site=self.site,
            username=self.user
            )
        loginManager.login()
        return self.site
