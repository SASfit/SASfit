# -*- coding: utf-8  -*-

__version__ = '$Id: incubator_family.py 8970 2011-02-16 11:57:50Z xqt $'

import family

# The Wikimedia Incubator family

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'incubator'
        self.langs = {
            'incubator': 'incubator.wikimedia.org',
        }
        if family.config.SSL_connection:
            self.langs['incubator'] = None

        self.namespaces[4] = {
            '_default': [u'Incubator', u'I', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'Incubator talk', self.namespaces[5]['_default']],
        }
        self.namespaces[100] = {
            '_default': u'Lost',
        }
        self.namespaces[101] = {
            '_default': u'Lost talk',
        }
        self.interwiki_forward = 'wikipedia'
        self.cross_projects = [
            'wikipedia', 'wiktionary', 'wikibooks', 'wikiquote', 'wikisource', 'wikinews', 'wikiversity',
            'meta', 'mediawiki', 'test', 'commons', 'species',
        ]

    def version(self, code):
        return '1.17wmf1'

    def shared_image_repository(self, code):
        return ('commons', 'commons')

    if family.config.SSL_connection:
        def hostname(self, code):
            return 'secure.wikimedia.org'

        def protocol(self, code):
            return 'https'

        def scriptpath(self, code):
            return '/wikipedia/incubator/w'

        def nicepath(self, code):
            return '/wikipedia/incubator/wiki/'

