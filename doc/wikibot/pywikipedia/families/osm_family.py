# -*- coding: utf-8  -*-

__version__ = '$Id: osm_family.py 9182 2011-04-19 09:07:26Z xqt $'

import family

# The project wiki of OpenStreetMap (OSM).

class Family(family.Family):

    def __init__(self):
        family.Family.__init__(self)
        self.name = 'osm'
        self.langs = {
            'en': 'wiki.openstreetmap.org',
        }

        self.namespaces[4] = {
            '_default': [u'OpenStreetMap', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'OpenStreetMap talk', self.namespaces[5]['_default']],
        }
        self.namespaces[6] = {
            '_default': u'File',
        }
        self.namespaces[7] = {
            '_default': u'File talk',
        }
        self.namespaces[200] = {
            '_default': u'DE',
        }
        self.namespaces[201] = {
            '_default': u'DE talk',
        }
        self.namespaces[202] = {
            '_default': u'FR',
        }
        self.namespaces[203] = {
            '_default': u'FR talk',
        }
        self.namespaces[204] = {
            '_default': u'ES',
        }
        self.namespaces[205] = {
            '_default': u'ES talk',
        }
        self.namespaces[206] = {
            '_default': u'IT',
        }
        self.namespaces[207] = {
            '_default': u'IT talk',
        }
        self.namespaces[208] = {
            '_default': u'NL',
        }
        self.namespaces[209] = {
            '_default': u'NL talk',
        }
        self.namespaces[210] = {
            '_default': u'RU',
        }
        self.namespaces[211] = {
            '_default': u'RU talk',
        }
        self.namespaces[212] = {
            '_default': u'JA',
        }
        self.namespaces[213] = {
            '_default': u'JA talk',
        }                

    def version(self, code):
        return "1.16.2"
