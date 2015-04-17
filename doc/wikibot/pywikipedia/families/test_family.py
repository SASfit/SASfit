# -*- coding: utf-8  -*-

__version__ = '$Id: test_family.py 8970 2011-02-16 11:57:50Z xqt $'

import family

# The test wikipedia family

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'test'
        self.langs = {
            'test': 'test.wikipedia.org',
        }
        if family.config.SSL_connection:
            self.langs['test'] = None


        self.namespaces[4] = {
            '_default': [u'Wikipedia', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'Wikipedia talk', self.namespaces[5]['_default']],
        }
        self.namespaces[100] = {
            '_default': u'Hilfe',
        }
        self.namespaces[101] = {
            '_default': u'Hilfe Diskussion',
        }
        self.namespaces[102] = {
            '_default': u'Aide',
        }
        self.namespaces[103] = {
            '_default': u'Discussion Aide',
        }
        self.namespaces[104] = {
            '_default': u'Hjælp',
        }
        self.namespaces[105] = {
            '_default': u'Hjælp diskussion',
        }
        self.namespaces[106] = {
            '_default': u'Helpo',
        }
        self.namespaces[107] = {
            '_default': u'Helpa diskuto',
        }
        self.namespaces[108] = {
            '_default': u'Hjälp',
        }
        self.namespaces[109] = {
            '_default': u'Hjälp diskussion',
        }
        self.namespaces[110] = {
            '_default': u'Ayuda',
        }
        self.namespaces[111] = {
            '_default': u'Ayuda Discusión',
        }
        self.namespaces[112] = {
            '_default': u'Aiuto',
        }
        self.namespaces[113] = {
            '_default': u'Discussioni aiuto',
        }
        self.namespaces[114] = {
            '_default': u'ヘルプ',
        }
        self.namespaces[115] = {
            '_default': u'ヘルプ‐ノート',
        }
        self.namespaces[116] = {
            '_default': u'NL Help',
        }
        self.namespaces[117] = {
            '_default': u'Overleg help',
        }
        self.namespaces[118] = {
            '_default': u'Pomoc',
        }
        self.namespaces[119] = {
            '_default': u'Dyskusja pomocy',
        }
        self.namespaces[120] = {
            '_default': u'Ajuda',
        }
        self.namespaces[121] = {
            '_default': u'Ajuda Discussão',
        }
        self.namespaces[122] = {
            '_default': u'CA Ajuda',
        }
        self.namespaces[123] = {
            '_default': u'CA Ajuda Discussió',
        }
        self.namespaces[124] = {
            '_default': u'Hjelp',
        }
        self.namespaces[125] = {
            '_default': u'Hjelp diskusjon',
        }
        self.namespaces[126] = {
            '_default': u'帮助',
        }
        self.namespaces[127] = {
            '_default': u'帮助 对话',
        }
        self.namespaces[128] = {
            '_default': u'Помощь',
        }
        self.namespaces[129] = {
            '_default': u'Помощь Дискуссия',
        }
        self.namespaces[130] = {
            '_default': u'Pomoč',
        }
        self.namespaces[131] = {
            '_default': u'Pogovor o pomoči',
        }
        self.namespaces[132] = {
            '_default': u'مساعدة',
        }
        self.namespaces[133] = {
            '_default': u'نقاش المساعدة',
        }
        self.interwiki_forward = 'wikipedia'
        self.cross_projects = [
            'wikipedia', 'wiktionary', 'wikibooks', 'wikiquote', 'wikisource', 'wikinews', 'wikiversity',
            'meta', 'mediawiki', 'incubator', 'commons', 'species',
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
            return '/wikipedia/test/w'

        def nicepath(self, code):
            return '/wikipedia/test/wiki/'
