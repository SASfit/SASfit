# -*- coding: utf-8  -*-

__version__ = '$Id: wikitech_family.py 8782 2010-12-20 04:46:29Z xqt $'

import family

# The Wikitech family

class Family(family.Family):

    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wikitech'
        self.langs = {
            'en': 'wikitech.wikimedia.org',
        }

        self.namespaces[4] = {
            '_default': [u'Wikitech', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'Wikitech talk', self.namespaces[5]['_default']],
        }

    def version(self, code):
        return '1.16wmf4'

    def scriptpath(self, code):
        return ''
