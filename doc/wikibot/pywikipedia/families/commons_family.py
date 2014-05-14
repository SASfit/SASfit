# -*- coding: utf-8  -*-

__version__ = '$Id: commons_family.py 8963 2011-02-16 08:34:10Z xqt $'

import family

# The Wikimedia Commons family

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'commons'
        self.langs = {
            'commons': 'commons.wikimedia.org',
        }
        if family.config.SSL_connection:
            self.langs['commons'] = None

        self.namespaces[4] = {
            '_default': [u'Commons', 'Project'],
        }
        self.namespaces[5] = {
            '_default': [u'Commons talk', 'Project talk'],
        }
        self.namespaces[100] = {
            '_default': u'Creator',
        }
        self.namespaces[101] = {
            '_default': u'Creator talk',
        }
        self.namespaces[102] = {
            '_default': [u'TimedText'],
        }
        self.namespaces[103] = {
            '_default': [u'TimedText talk'],
        }
        self.namespaces[104] = {
            '_default': [u'Sequence'],
        }
        self.namespaces[105] = {
            '_default': [u'Sequence talk'],
        }
        self.namespaces[106] = {
            '_default': [u'Institution'],
        }
        self.namespaces[107] = {
            '_default': [u'Institution talk'],
        }

        self.interwiki_forward = 'wikipedia'

        self.category_redirect_templates = {
            'commons': (u'Category redirect',
                        u'Categoryredirect',
                        u'See cat',
                        u'Seecat',
                        u'Catredirect',
                        u'Cat redirect',
                        u'CatRed',
                        u'Cat-red',
                        u'Catredir',
                        u'Redirect category'),
        }

        self.disambiguationTemplates = {
            'commons': [u'Disambig', u'Disambiguation', u'Razločitev',
                        u'Begriffsklärung']
        }

        self.disambcatname = {
            'commons':  u'Disambiguation'
        }
        self.cross_projects = [
            'wikipedia', 'wiktionary', 'wikibooks', 'wikiquote', 'wikisource', 'wikinews', 'wikiversity',
            'meta', 'mediawiki', 'test', 'incubator', 'species',
        ]


    def version(self, code):
        return '1.17wmf1'

    def dbName(self, code):
        return 'commonswiki_p'

    def shared_image_repository(self, code):
        return ('commons', 'commons')

    if family.config.SSL_connection:
        def hostname(self, code):
            return 'secure.wikimedia.org'

        def protocol(self, code):
            return 'https'

        def scriptpath(self, code):
            return '/wikipedia/commons/w'

        def nicepath(self, code):
            return '/wikipedia/commons/wiki/'
