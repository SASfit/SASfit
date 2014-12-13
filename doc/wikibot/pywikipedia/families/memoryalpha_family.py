# -*- coding: utf-8  -*-
import family

# The Memory Alpha family, a set of StarTrek wikis.

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'memoryalpha'
        self.languages_by_size = ['bg', 'cs', 'en', 'de', 'es', 'mu', 'nl', 'sv', 'fr', 'eo', 'pl', 'zh-cn', 'ja', 'it', 'pt', 'sr']
        self.langs = {
            'bg':'bg',
            'cs':'cs',
            'de':'de',
            'en':'en',
            'eo':'eo',
            'es':'es',
            'mu':'mu',
            'fr':'fr',
            'it':'it',
            'ja':'ja',
            'nl':'nl',
            'pl':'pl',
            'pt':'pt',
            'ru':'ru',
            'sr':'sr',
            'sv':'sv',
            'zh-cn':'zh-cn',
        }


        # Override defaults
        self.namespaces[2]['pl'] = u'Użytkownik'
        self.namespaces[3]['pl'] = u'Dyskusja użytkownika'

        # Most namespaces are inherited from family.Family.

        self.namespaces[4] = {
            '_default': u'Memory Alpha',
            'cs': u'encyklopedie Star Treku',
            'pt': u'Memória Alfa',
            'ru': u'Memory Alpha - A Wikia wiki',
            'sr': u'Успомене Алфе',
            'zh-cn': u'阿尔法记忆',
        }
        self.namespaces[5] = {
            '_default': u'Memory Alpha talk',
            'bg': u'Memory Alpha беседа',
            'cs': u'encyklopedie Star Treku diskuse',
            'de': u'Memory Alpha Diskussion',
            'eo': u'Memory Alpha diskuto',
            'es': u'Memory Alpha Discusión',
            'fr': u'Discussion Memory Alpha',
            'it': u'Discussioni Memory Alpha',
            'ja': u'Memory Alpha‐ノート',
            'nl': u'Overleg Memory Alpha',
            'pl': u'Dyskusja Memory Alpha',
            'pt': u'Memória Alfa Discussão',
            'ru': u'Обсуждение Memory Alpha - A Wikia wiki',
            'sr': u'Разговор о Успомене Алфе',
            'sv': u'Memory Alphadiskussion',
            'zh-cn': u'阿尔法记忆讨论',
        }
        self.namespaces[6]['pt'] = u'Arquivo'

        self.namespaces[7]['pt'] = u'Arquivo Discussão'
        self.namespaces[7]['fr'] = u'Discussion Fichier'

        self.namespaces[100] = {
            '_default': u'Forum',
            'ru': u'Форум',
            'pt': u'Fórum',
        }
        self.namespaces[101] = {
            '_default': u'Forum talk',
            'pl': u'Dyskusja forum',
            'ru': u'Обсуждение форума',
            'de': u'Forum Diskussion',
            'pt': u'Fórum Discussão',
        }
        self.namespaces[102] = {
            '_default': u'Portal',
        }
        self.namespaces[103] = {
            '_default': u'Portal talk',
            'de': u'Portal Diskussion',
            'pt': u'Portal Discussão',
        }
        self.namespaces[110] = {
            '_default': u'Forum',
            'ru': u'Форум',
        }
        self.namespaces[111] = {
            '_default': u'Forum talk',
            'pl': u'Dyskusja forum',
            'ru': u'Обсуждение форума',
        }
        self.namespaces[400] = {
            '_default': u'Video',
            'ru': u'Видео',
        }
        self.namespaces[401] = {
            '_default': u'Video talk',
            'de': u'Video talk',
            'pl': u'Video talk',
            'ru': u'Обсуждение видео',
        }
        self.namespaces[402] = {
            '_default': u'Video Template',
        }
        self.namespaces[500] = {
            '_default': u'User blog',
            'de': u'Benutzer Blog',
            'ru': u'Блог участника',
        }
        self.namespaces[501] = {
            '_default': u'User blog comment',
            'de': u'Benutzer Blog Kommentare',
            'ru': u'Комментарий блога участника',
        }
        self.namespaces[502] = {
            '_default': u'Blog',
            'ru': u'Блог',
        }
        self.namespaces[503] = {
            '_default': u'Blog talk',
            'de': u'Blog Diskussion',
            'ru': u'Обсуждение блога',
        }

        # A few selected big languages for things that we do not want to loop over
        # all languages. This is only needed by the titletranslate.py module, so
        # if you carefully avoid the options, you could get away without these
        # for another wiki family.

        self.alphabetic_revised = ['de', 'en', 'es', 'eo', 'fr', 'nl', 'pl', 'sv']

        self.obsolete = { 'zh':'zh-cn',}


    def hostname(self,code):
        return 'memory-alpha.org'

    def scriptpath(self, code):
        return '/%s' % code

    def version(self, code):
        return "1.12alpha"
