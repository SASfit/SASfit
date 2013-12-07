# -*- coding: utf-8  -*-

__version__ = '$Id: wowwiki_family.py 9281 2011-05-29 11:13:33Z xqt $'

import config, family, urllib

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wowwiki'

        self.langs = {
            'cs': 'cs.wow.wikia.com',
            'da': 'da.wowwiki.com',
            'de': 'de.wow.wikia.com',
            'el': 'el.wow.wikia.com',
            'en': 'www.wowwiki.com',
            'es': 'es.wow.wikia.com',
            'fa': 'fa.wow.wikia.com',
            'fi': 'fi.wow.wikia.com',
            'fr': 'fr.wowwiki.com',
            'he': 'he.wow.wikia.com',
            'hr': 'hr.wow.wikia.com',
            'hu': 'hu.wow.wikia.com',
            'is': 'is.wow.wikia.com',
            'it': 'it.wow.wikia.com',
            'ja': 'ja.wow.wikia.com',
            'ko': 'ko.wow.wikia.com',
            'lt': 'lt.wow.wikia.com',
            'lv': 'lv.wow.wikia.com',
            'nl': 'nl.wow.wikia.com',
            'no': 'no.wowwiki.com',
            'pl': 'pl.wow.wikia.com',
            'pt': 'pt.wow.wikia.com',
            'pt-br': 'pt-br.wow.wikia.com',
            'ro': 'ro.wow.wikia.com',
            'ru': 'ru.wow.wikia.com',
            'sk': 'sk.wow.wikia.com',
            'sr': 'sr.wow.wikia.com',
            'sv': 'sv.warcraft.wikia.com',
            'tr': 'tr.wow.wikia.com',
            'zh-tw': 'zh-tw.wow.wikia.com',
            'zh': 'zh.wow.wikia.com'
        }

        # Override defaults
        self.namespaces[3]['cs'] = [u'Uživatel diskuse']
        self.namespaces[7]['cs'] = [u'Soubor diskuse']
        self.namespaces[9]['cs'] = [u'MediaWiki diskuse']
        self.namespaces[11]['cs'] = [u'Šablona diskuse']
        self.namespaces[13]['cs'] = [u'Nápověda diskuse']
        self.namespaces[15]['cs'] = [u'Kategorie diskuse']

        self.namespaces[9]['da'] = [u'MediaWiki-diskussion']
        self.namespaces[13]['da'] = [u'Hjælp-diskussion']

        self.namespaces[-2]['de'] = [u'Media']

        self.namespaces[1]['ja'] = [u'ノート']
        self.namespaces[3]['ja'] = [u'利用者‐会話']
        self.namespaces[7]['ja'] = [u'ファイル‐ノート', u'Image talk', u'画像‐ノート']
        self.namespaces[9]['ja'] = [u'MediaWiki‐ノート']
        self.namespaces[10]['ja'] = [u'Template']
        self.namespaces[11]['ja'] = [u'Template‐ノート']
        self.namespaces[12]['ja'] = [u'Help']
        self.namespaces[13]['ja'] = [u'Help‐ノート']
        self.namespaces[14]['ja'] = [u'Category']
        self.namespaces[15]['ja'] = [u'Category‐ノート']

        self.namespaces[-1]['pt-br'] = [u'Especial']
        self.namespaces[1]['pt-br'] = [u'Discussão']
        self.namespaces[2]['pt-br'] = [u'Usuário']
        self.namespaces[3]['pt-br'] = [u'Usuário Discussão']
        self.namespaces[6]['pt-br'] = [u'Arquivo']
        self.namespaces[7]['pt-br'] = [u'Arquivo Discussão']
        self.namespaces[9]['pt-br'] = [u'MediaWiki Discussão']
        self.namespaces[10]['pt-br'] = [u'Predefinição']
        self.namespaces[11]['pt-br'] = [u'Predefinição Discussão']
        self.namespaces[12]['pt-br'] = [u'Ajuda']
        self.namespaces[13]['pt-br'] = [u'Ajuda Discussão']
        self.namespaces[14]['pt-br'] = [u'Categoria']
        self.namespaces[15]['pt-br'] = [u'Categoria Discussão']

        self.namespaces[-2]['zh-tw'] = [u'媒體']
        self.namespaces[-1]['zh-tw'] = [u'特殊']
        self.namespaces[1]['zh-tw'] = [u'討論']
        self.namespaces[2]['zh-tw'] = [u'使用者']
        self.namespaces[3]['zh-tw'] = [u'使用者討論']
        self.namespaces[6]['zh-tw'] = [u'檔案']
        self.namespaces[7]['zh-tw'] = [u'檔案討論']
        self.namespaces[9]['zh-tw'] = [u'MediaWiki討論']
        self.namespaces[10]['zh-tw'] = [u'模板']
        self.namespaces[11]['zh-tw'] = [u'模板討論']
        self.namespaces[12]['zh-tw'] = [u'使用說明']
        self.namespaces[13]['zh-tw'] = [u'使用說明討論']
        self.namespaces[14]['zh-tw'] = [u'分類']
        self.namespaces[15]['zh-tw'] = [u'分類討論']

        self.namespaces[4] = {
            'cs': u'WoWWiki',
            'da': u'WoWWiki Danmark',
            'de': u'WoWWiki',
            'el': u'WoWWiki Ελληνικός οδηγός',
            'en': u'WoWWiki',
            'es': u'WarcraftWiki',
            'fa': u'دنیای وارکرفت',
            'fi': u'WoWWiki Suomi',
            'fr': u'WikiWoW',
            'he': u'Worldofwiki',
            'hr': u'World of Warcraft Wiki',
            'hu': u'World of Warcraft Wiki',
            'is': u'WoWWiki',
            'it': u'WoWWiki Italia',
            'ja': u'World of Warcraft Wiki',
            'ko': u'World of Warcraft Wiki',
            'lt': u'World of Warcraft Wiki',
            'lv': u'World of Warcraft',
            'nl': u'WoWWiki',
            'no': u'Wowwiki Norge',
            'pl': u'WoWWiki',
            'pt': u'World of Warcraft',
            'pt-br': u'WowWiki Br',
            'ro': u'World of Warcraft Romania',
            'sk': u'WoWwiki',
            'sr': u'Wow wiki',
            'sv': u'WoWWiki Sverige',
            'ru': u'WoWWiki',
            'tr': u'WoWWiki Türkçe',
            'zh': u'World of Warcraft Wiki',
            'zh-tw': u'魔獸世界百科全書'
        }

        self.namespaces[5] = {
            'cs': u'WoWWiki diskuse',
            'da': u'WoWWiki Danmark-diskussion',
            'de': u'WoWWiki Diskussion',
            'el': u'WoWWiki Ελληνικός οδηγός συζήτηση',
            'en': u'WoWWiki talk',
            'es': u'WarcraftWiki Discusión',
            'fa': u'بحث دنیای وارکرفت',
            'fi': u'Keskustelu WoWWiki Suomista',
            'fr': u'Discussion WikiWoW',
            'he': u'שיחת Worldofwiki',
            'hr': u'Razgovor World of Warcraft Wiki',
            'hu': u'World of Warcraft Wiki-vita',
            'is': u'WoWWikispjall',
            'it': u'Discussioni WoWWiki Italia',
            'ja': u'World of Warcraft Wiki‐ノート',
            'ko': u'World of Warcraft Wiki토론',
            'lt': u'World of Warcraft Wiki aptarimas',
            'lv': u'World of Warcraft diskusija',
            'nl': u'Overleg WoWWiki',
            'no': u'Wowwiki Norge-diskusjon',
            'pl': u'Dyskusja WoWWiki',
            'pt': u'World of Warcraft Discussão',
            'pt-br': u'WowWiki Br Discussão',
            'ro': u'Discuție World of Warcraft Romania',
            'ru': u'Обсуждение WoWWiki',
            'sk': u'Diskusia k WoWwiki',
            'sr': u'Разговор о Wow wiki',
            'sv': u'WoWWiki Sverigediskussion',
            'tr': u'WoWWiki Türkçe tartışma',
            'zh': u'World of Warcraft Wiki talk',
            'zh-tw': u'魔獸世界百科全書討論'
        }

        #wikia-wide defaults
        self.namespaces[110] = {
             '_default': 'Forum',
             'es': u'Foro',
             'fa': u'فوروم',
             'fi': u'Foorumi',
             'ru': u'Форум'
        }
        self.namespaces[111] = {
             '_default': 'Forum talk',
             'es': u'Foro Discusión',
             'fa': u'بحث فوروم',
             'fi': u'Keskustelu foorumista',
             'pl': u'Dyskusja forum',
             'no': u'Forumdiskusjon',
             'ru': u'Обсуждение форума'
        }

        self.namespaces[400] = {
            '_default': u'Video',
            'ru': u'Видео'
        }
        self.namespaces[401] = {
            '_default': u'Video talk',
            'de': u'Video Diskussion',
            'no': u'Videodiskusjon',
            'pl': u'Dyskusja Video',
            'ru': u'Обсуждение видео'
        }
        self.namespaces[500] = {
            '_default': u'User blog',
            'de': u'Benutzer Blog',
            'es': u'Usuario Blog',
            'no': u'Brukerblogg',
            'ru': u'Блог участника'
        }
        self.namespaces[501] = {
            '_default': u'User blog comment',
            'de': u'Benutzer Blog Kommentare',
            'es': u'Usuario Blog Comentario',
            'no': u'Brukerbloggkommentar',
            'ru': u'Комментарий блога участника'
        }
        self.namespaces[502] = {
            '_default': u'Blog',
            'no': u'Blogg',
            'ru': u'Блог'
        }
        self.namespaces[503] = {
            '_default': u'Blog talk',
            'de': u'Blog Diskussion',
            'es': u'Blog Discusión',
            'no': u'Bloggdiskusjon',
            'ru': u'Обсуждение блога'
        }

        #a few edge cases:
        self.namespaces[112] = {
            'en': u'Guild',
            'de': u'Portal',
            'ru': u'Портал'
        }
        self.namespaces[113] = {
            'en': u'Guild talk',
            'de': u'Portal Diskussion',
            'ru': u'Портал talk'
        }
        self.namespaces[114] = {
            'en': u'Server',
            'ru': u'Гильдия'
        }
        self.namespaces[115] = {
            'en': u'Server talk',
            'ru': u'Гильдия talk'
        }
        self.namespaces[116] = {
             'en': u'Portal',
             'ru': u'Сервер'
        }
        self.namespaces[117] = {
            'en': u'Portal talk',
            'ru': u'Сервер talk'
        }
        self.namespaces[118] = {
            'en': u'Quest'
        }
        self.namespaces[119] = {
            'en': u'Quest talk'
        }
        self.namespaces[120] = {
            'no': u'Oppdrag'
        }
        self.namespaces[121] = {
            'no': u'Oppdrag Kommentar'
        }
        self.namespaces[122] = {
            'no': u'Retningslinje'
        }
        self.namespaces[123] = {
            'no': u'Retningslinje Kommentar'
        }
        self.namespaces[124] = {
            'no': u'Portal'
        }
        self.namespaces[125] = {
            'no': u'Portal diskusjon'
        }
        self.namespaces[126] = {
            'no': u'Tinget'
        }
        self.namespaces[127] = {
            'no': u'Tinget Diskusjon'
        }
        self.namespaces[128] = {
            'no': u'Blogg'
        }
        self.namespaces[129] = {
            'no': u'Blogg Kommentar'
        }
        self.namespaces[302] = {
            'en': u'Property',
            'ru': u'Свойство'
        }
        self.namespaces[303] = {
            'en': u'Property talk',
            'ru': u'Обсуждение свойства'
        }
        self.namespaces[304] = {
            'en': u'Type',
            'ru': u'Тип'
        }
        self.namespaces[305] = {
            'en': u'Type talk',
            'ru': u'Обсуждение типа'
        }
        self.namespaces[306] = {
            'en': u'Form',
            'ru': u'Form'
        }
        self.namespaces[307] = {
            'en': u'Form talk',
            'ru': u'Form talk'
        }
        self.namespaces[308] = {
            'en': u'Concept',
            'ru': u'Концепция'
        }
        self.namespaces[309] = {
            'en': u'Concept talk',
            'ru': u'Обсуждение концепции'
        }
        self.namespaces[370] = {
            'en': u'Filter',
            'ru': u'Filter'
        }
        self.namespaces[371] = {
            'en': u'Filter talk',
            'ru': u'Filter talk'
        }
        self.namespaces[800] = {
            'en': u'Poll'
        }
        self.namespaces[801] = {
            'en': u'Poll talk'
        }
        
        self.namespaces[902] = {
            'es': u'Layout'
        }

        self.content_id = "article"

        self.disambiguationTemplates['en'] = ['disambig', 'disambig/quest',
                                              'disambig/quest2',
                                              'disambig/achievement2']
        self.disambcatname['en'] = "Disambiguations"

        # Wikia's default CategorySelect extension always puts categories last
        self.categories_last = ['cs', 'da', 'de', 'el', 'en', 'es', 'fa', 'fi', 'fr', 'he', 'hr', 'hu', 'is', 'it', 'ja', 'ko', 'lt', 'lv', 'nl', 'no', 'pl', 'pt', 'pt-br', 'ro', 'ru', 'sk', 'sr', 'sv', 'tr', 'zh-tw', 'zh']

    def protocol(self, code):
        return 'http'

    def scriptpath(self, code):
        return ''

    def apipath(self, code):
        return '%s/api.php' % self.scriptpath(code)

    def version(self, code):
        return '1.16.5'

    def code2encoding(self, code):
        return 'utf-8'
