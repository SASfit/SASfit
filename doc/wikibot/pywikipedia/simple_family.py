# -*- coding: utf-8  -*-

# ============================================
# NOTE FOR USERS: Unlike the Family files in
# the family # directory, you do not need to
# edit this file to configure anything.
# ============================================

# (C) Kim Bruning for Wikiation, sponsored by Kennisnet, 2009
#
# Distributed under the terms of the MIT license.
#

import sys #, settings    # No such module settings
#if settings.pywikipedia_path not in sys.path:
#    sys.path.append(settings.pywikipedia_path)

import config, family, urllib
class Family(family.Family):
    """Friendlier version of the pywikipedia family class.
    We can use this in conjunction with none-pywikipedia
    config files.

    Note that this just handles most common cases.
    If you run into a special case, you'll have to fall back
    to your regular pywikipedia.
    """

    def __init__(self,
        name='MY_NAME_FOR_THIS_SERVER',
        protocol='http',
        server='www.my_server.com',
        scriptpath='/my/script/path',
        version='1.13.2',
        lang='en',
        encoding='utf-8',
        api_supported=False,
        RversionTab=None    # very rare beast, you probably won't need it.
        ):
        """name: arbitrary name. Pick something easy to remember
        protocol: http|https
        server: dns address or ip address
        scriptpath: path on server itself
            (ie:  protocol:server/scriptpath   http://6.wikiation.nl/revisions/REL1.13.2)
        version: mediawiki version of the target mediawiki instance
        lang: default language, as configured on target mediawiki instance
        encoding: should (almost) always be utf-8
        api_supported: Does this mediawiki instance support the mediawiki api?
        RversionTab: Magic. See superclass for information.
        """

        family.Family.__init__(self)
        self.name = name        # REQUIRED; replace with actual name

        self.langs = {                # REQUIRED
            lang: server,  # Include one line for each wiki in family
        }
        self._protocol=protocol
        self._scriptpath=scriptpath
        self._version=version
        self._encoding=encoding
        # may as well add these here, so we can have a 1 stop shop
        self._lang=lang
        self._server=server
        self._api_supported=api_supported
        self._RversionTab=RversionTab

    def protocol(self, code):
        """
        returns "http" or "https"
    """
        return self._protocol

    def scriptpath(self, code):
        """returns the prefix used to locate scripts on this wiki.
        """
        return self._scriptpath

    def apipath(self, code):
        """returns whether or not this wiki
        """
        if self._api_supported:
            return '%s/api.php' % self.scriptpath(code)
        else:
            raise NotImplementedError, "%s wiki family does not support api.php" % self.name

    # Which version of MediaWiki is used?
    def version(self, code):
        # Replace with the actual version being run on your wiki
        return self._version

    def code2encoding(self, code):
        """Return the encoding for a specific language wiki"""
        # Most wikis nowadays use UTF-8, but change this if yours uses
        # a different encoding
        return self._encoding

    def RversionTab(self, code):
        return self._RversionTab
