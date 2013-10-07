# -*- coding: utf-8  -*-

__version__ = '$Id: wikitravel_shared_family.py 7087 2009-07-24 16:36:55Z alexsh $'

import family

# The Wikitravel shared family

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wikitravel_shared'
        self.langs = {
            'wikitravel_shared': 'wikitravel.org',
        }

        self.namespaces[4] = {
            '_default': u'Wikitravel Shared',
        }
        self.namespaces[5] = {
            '_default': u'Wikitravel Shared talk',
        }
        self.namespaces[6] = {
            '_default': u'Image',
        }
        self.namespaces[7] = {
            '_default': u'Image talk',
        }
        self.namespaces[200] = {
            '_default': u'Tech',
        }
        self.namespaces[201] = {
            '_default': u'Tech talk',
        }

        self.interwiki_forward = 'wikitravel'

    def scriptpath(self, code):
        return '/wiki/shared'

    def shared_image_repository(self, code):
        return ('wikitravel_shared', 'wikitravel_shared')

    def version(self, code):
        return "1.10.1"

