# -*- coding: utf-8  -*-

__version__ = '$Id: strategy_family.py 8963 2011-02-16 08:34:10Z xqt $'

import family, config

# The Wikimedia Strategy family

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'strategy'
        self.langs = {
            'strategy': 'strategy.wikimedia.org',
        }
        if config.SSL_connection:
            self.langs['strategy'] = None

        self.namespaces[4] = {
            '_default': [u'Strategic Planning', 'Project'],
        }
        self.namespaces[5] = {
            '_default': [u'Strategic Planning talk', 'Project talk'],
        }
        self.namespaces[106] = {
            '_default': [u'Proposal'],
        }
        self.namespaces[107] = {
            '_default': [u'Proposal talk'],
        }

        self.interwiki_forward = 'wikipedia'

    def version(self, code):
        return '1.17wmf1'

    def dbName(self, code):
        return 'strategywiki_p'

    def shared_image_repository(self, code):
        return ('commons', 'commons')

    if config.SSL_connection:
        def hostname(self, code):
            return 'secure.wikimedia.org'

        def protocol(self, code):
            return 'https'

        def scriptpath(self, code):
            return '/wikipedia/strategy/w'

        def nicepath(self, code):
            return '/wikipedia/strategy/wiki/'
