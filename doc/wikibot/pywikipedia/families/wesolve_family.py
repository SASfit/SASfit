# -*- coding: utf-8  -*-

import urllib
import family, wikipedia

# The wikimedia family that is known as Wesolve

class UploadDisabled(wikipedia.Error):
    """Uploads are disabled on this wiki"""

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wesolve'
        self.langs = {
            'wesolve': None,
        }

        self.namespaces[4] = {
            '_default': u'Wordsandmore',
        }
        self.namespaces[5] = {
            '_default': u'Wordsandmore talk',
        }


    def version(self, code):
        return "1.5.7"

    def scriptpath(self, code):
        return ''

    def hostname(self, code):
        return 'wordsandmore.org'

    def apipath(self, code):
        raise NotImplementedError(
            "The wesolve family does not support api.php")
