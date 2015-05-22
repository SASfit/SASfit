# -*- coding: utf-8  -*-

__version__ = '$Id: openttd_family.py 9015 2011-02-28 17:57:55Z lcawte $'

import family

# The project wiki of OpenTTD, an open source game (Transport Tycoon Deluxe clone).

class Family(family.Family):

    def __init__(self):
        family.Family.__init__(self)
        self.name = 'openttd'
        self.langs = {
            'en': 'wiki.openttd.org',
        }

        self.namespaces[4] = {
            '_default': [u'OpenTTD', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'OpenTTD talk', self.namespaces[5]['_default']],
        }
        self.namespaces[100] = {
            '_default': u'Manual',
        }
        self.namespaces[101] = {
            '_default': u'Development',
        }

    def scriptpath(self, code):
        return ''

    def version(self, code):
        return "1.12.0"

    def apipath(self, code):
        raise NotImplementedError(
            "The openttd family does not support api.php")
