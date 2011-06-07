# -*- coding: utf-8  -*-
import family

__version__ = '$Id: wikibooks_family.py 9285 2011-06-03 10:36:34Z xqt $'

# The Wikimedia family that is known as Wikibooks

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wikibooks'

        self.languages_by_size = [
            'en', 'de', 'fr', 'hu', 'ja', 'pt', 'nl', 'es', 'pl', 'it', 'he',
            'sq', 'fi', 'ca', 'ru', 'vi', 'cs', 'zh', 'hr', 'sv', 'tr', 'da',
            'id', 'ko', 'th', 'gl', 'fa', 'sr', 'tl', 'ar', 'no', 'mk', 'is',
            'ta', 'ka', 'lt', 'eo', 'bg', 'ro', 'sk', 'uk', 'el', 'az', 'si',
            'li', 'la', 'tt', 'ang', 'ia', 'cv', 'sl', 'et', 'mr', 'ur', 'bn',
            'oc', 'ml', 'ms', 'eu', 'hi', 'fy', 'ie', 'te', 'hy', 'tg', 'af',
            'ne', 'pa', 'bs', 'sa', 'ky', 'be', 'ast', 'zh-min-nan', 'mg', 'cy',
            'ku', 'tk', 'su', 'uz', 'vo', 'kk', 'mn', 'my',
        ]

        if family.config.SSL_connection:
            self.langs = dict([(lang, None) for lang in self.languages_by_size])
        else:
            self.langs = dict([(lang, '%s.wikibooks.org' % lang) for lang in self.languages_by_size])

        # Override defaults
        self.namespaces[2]['pl'] = u'Wikipedysta'
        self.namespaces[3]['pl'] = u'Dyskusja Wikipedysty'

        # Most namespaces are inherited from family.Family.
        # Translation used on all wikis for the different namespaces.
        # (Please sort languages alphabetically)
        # You only need to enter translations that differ from _default.
        self.namespaces[4] = {
            '_default': [u'Wikibooks', self.namespaces[4]['_default']],
            'ar': u'ويكي الكتب',
            'bg': u'Уикикниги',
            'bn': u'উইকিবই',
            'bs': u'Wikiknjige',
            'ca': u'Viquillibres',
            'cs': u'Wikiknihy',
            'cy': u'Wicillyfrau',
            'el': u'Βικιβιβλία',
            'eo': u'Vikilibroj',
            'es': u'Wikilibros',
            'et': u'Vikiõpikud',
            'fa': u'ویکی‌نسک',
            'fi': u'Wikikirjasto',
            'fr': u'Wikilivres',
            'ga': u'Vicíleabhair',
            'he': u'ויקיספר',
            'hr': u'Wikiknjige',
            'hu': u'Wikikönyvek',
            'hy': u'Վիքիգրքեր',
            'is': u'Wikibækur',
            'ka': u'ვიკიწიგნები',
            'kk': u'Уикикітап',
            'ko': u'위키책',
            'la': u'Vicilibri',
            'li': u'Wikibeuk',
            'ml': u'വിക്കിപാഠശാല',
            'no': u'Wikibøker',
            'oc': u'Wikilibres',
            'ps': u'ويکيتابونه',
            'pt': u'Wikilivros',
            'ro': u'Wikimanuale',
            'ru': u'Викиучебник',
            'si': u'විකිපොත්',
            'sl': u'Wikiknjige',
            'sr': u'Викикњиге',
            'tr': u'Vikikitap',
            'uk': u'Вікіпідручник',
            'ur': u'وکی کتب',
            'uz': u'Vikikitob',
            'vo': u'Vükibuks',
            'zh': [u'Wikibooks', u'维基教科书'],
        }

        self.namespaces[5] = {
            '_default': [u'Wikibooks talk', self.namespaces[5]['_default']],
            'af': u'Wikibooksbespreking',
            'als': u'Wikibooks Diskussion',
            'ar': u'نقاش ويكي الكتب',
            'ast': u'Wikibooks alderique',
            'ay': u'Wikibooks Discusión',
            'az': u'Wikibooks müzakirəsi',
            'ba': u'Wikibooks б-са фекер алышыу',
            'be': u'Wikibooks размовы',
            'bg': u'Уикикниги беседа',
            'bm': u'Discussion Wikibooks',
            'bn': u'উইকিবই আলোচনা',
            'bs': u'Razgovor s Wikiknjigama',
            'ca': u'Viquillibres Discussió',
            'cs': u'Diskuse k Wikiknihám',
            'cv': u'Wikibooks сӳтсе явмалли',
            'cy': u'Sgwrs Wicillyfrau',
            'da': [u'Wikibooks diskussion', u'Wikibooks-diskussion'],
            'de': u'Wikibooks Diskussion',
            'el': u'Βικιβιβλία συζήτηση',
            'eo': [u'Vikilibroj-Diskuto', u'Vikilibroj diskuto'],
            'es': u'Wikilibros Discusión',
            'et': u'Vikiõpikute arutelu',
            'eu': u'Wikibooks eztabaida',
            'fa': u'بحث ویکی‌نسک',
            'fi': u'Keskustelu Wikikirjastosta',
            'fr': u'Discussion Wikilivres',
            'fy': u'Wikibooks oerlis',
            'ga': u'Plé Vicíleabhar',
            'gl': u'Conversa Wikibooks',
            'gn': u'Wikibooks myangekõi',
            'he': u'שיחת ויקיספר',
            'hi': u'Wikibooks वार्ता',
            'hr': u'Razgovor Wikiknjige',
            'hu': u'Wikikönyvek-vita',
            'hy': u'Վիքիգրքերի քննարկում',
            'ia': u'Discussion Wikibooks',
            'id': u'Pembicaraan Wikibooks',
            'is': u'Wikibækurspjall',
            'it': u'Discussioni Wikibooks',
            'ja': u'Wikibooks・トーク',
            'ka': u'ვიკიწიგნები განხილვა',
            'kk': u'Уикикітап талқылауы',
            'km': u'ការពិភាក្សាអំពីWikibooks',
            'kn': u'Wikibooks ಚರ್ಚೆ',
            'ko': u'위키책토론',
            'ku': u'Wikibooks nîqaş',
            'la': u'Disputatio Vicilibrorum',
            'lb': u'Wikibooks Diskussioun',
            'li': u'Euverlèk Wikibeuk',
            'ln': u'Discussion Wikibooks',
            'lt': u'Wikibooks aptarimas',
            'lv': u'Wikibooks diskusija',
            'mg': u'Dinika amin\'ny Wikibooks',
            'mk': u'Разговор за Wikibooks',
            'ml': u'വിക്കിപാഠശാല സംവാദം',
            'mn': u'Wikibooks-н хэлэлцүүлэг',
            'mr': u'Wikibooks चर्चा',
            'ms': u'Perbincangan Wikibooks',
            'nah': u'Wikibooks Discusión',
            'nds': u'Wikibooks Diskuschoon',
            'ne': u'Wikibooks वार्ता',
            'nl': u'Overleg Wikibooks',
            'no': u'Wikibøker-diskusjon',
            'oc': u'Discussion Wikilibres',
            'pa': u'Wikibooks ਚਰਚਾ',
            'pl': u'Dyskusja Wikibooks',
            'ps': u'د ويکيتابونه خبرې اترې',
            'pt': u'Wikilivros Discussão',
            'qu': u'Wikibooks rimanakuy',
            'ro': u'Discuție Wikimanuale',
            'ru': u'Обсуждение Викиучебника',
            'sa': [u'Wikibooksसम्भाषणम्', u'Wikibooksसंभाषणं'],
            'si': u'විකිපොත් සාකච්ඡාව',
            'sk': u'Diskusia k Wikibooks',
            'sl': u'Pogovor o Wikiknjigah',
            'sq': u'Wikibooks diskutim',
            'sr': u'Разговор о викикњигама',
            'su': u'Obrolan Wikibooks',
            'sv': u'Wikibooksdiskussion',
            'sw': u'Majadiliano ya Wikibooks',
            'ta': u'Wikibooks பேச்சு',
            'te': u'Wikibooks చర్చ',
            'tg': u'Баҳси Wikibooks',
            'th': u'คุยเรื่องWikibooks',
            'tk': u'Wikibooks çekişme',
            'tl': u'Usapang Wikibooks',
            'tr': u'Vikikitap tartışma',
            'tt': [u'Wikibooks бәхәсе', u'Wikibooks bäxäse'],
            'uk': u'Обговорення Вікіпідручника',
            'ur': u'تبادلۂ خیال وکی کتب',
            'uz': u'Vikikitob munozarasi',
            'vi': u'Thảo luận Wikibooks',
            'vo': u'Bespik dö Vükibuks',
            'wa': u'Wikibooks copene',
            'za': u'Wikibooks讨论',
            'zh': [u'Wikibooks talk', u'维基教科书讨论'],
        }

        self.namespaces[90] = {
            'pt': u'Tópico',
        }

        self.namespaces[91] = {
            'pt': u'Tópico discussão',
        }

        self.namespaces[92] = {
            'pt': u'Resumo',
        }

        self.namespaces[93] = {
            'pt': u'Resumo discussão',
        }

        self.namespaces[100] = {
            'bn': u'উইকিশৈশব',
            'fr': u'Transwiki',
            'he': u'שער',
            'id': u'Resep',
            'it': u'Progetto',
            'ja': u'Transwiki',
            'ml': u'പാചകപുസ്തകം',
            'ms': u'Resipi',
            'ro': u'Raft',
            'ru': u'Полка',
            'tr': u'Yemek',
            'uk': u'Полиця',
        }

        self.namespaces[101] = {
            'bn': u'উইকিশৈশব আলাপ',
            'fr': u'Discussion Transwiki',
            'he': u'שיחת שער',
            'id': u'Pembicaraan Resep',
            'it': u'Discussioni progetto',
            'ja': u'Transwiki‐ノート',
            'ml': u'പാചകപുസ്തകസം‌വാദം',
            'ms': u'Perbualan Resipi',
            'ro': u'Discuţie Raft',
            'ru': u'Обсуждение полки',
            'tr': u'Yemek tartışma',
            'uk': u'Обговорення полиці',
        }

        self.namespaces[102] = {
            'bn': u'বিষয়',
            'ca': u'Viquiprojecte',
            'cy': u'Silff lyfrau',
            'de': u'Regal',
            'en': u'Cookbook',
            'es': u'Wikiversidad',
            'id': u'Wisata',
            'it': u'Ripiano',
            'ml': u'വിഷയം',
            'nl': u'Transwiki',
            'ro': u'Wikijunior',
            'ru': u'Импортировано',
            'sr': u'Кувар',
            'uk': u'Рецепт',
        }

        self.namespaces[103] = {
            'bn': u'বিষয় আলাপ',
            'ca': u'Viquiprojecte Discussió',
            'cy': u'Sgwrs Silff lyfrau',
            'de': u'Regal Diskussion',
            'en': u'Cookbook talk',
            'es': u'Wikiversidad Discusión',
            'id': u'Pembicaraan Wisata',
            'it': u'Discussioni ripiano',
            'ml': u'വിഷയസം‌വാദം',
            'nl': u'Overleg transwiki',
            'ro': u'Discuţie Wikijunior',
            'ru': u'Обсуждение импортированного',
            'sr': u'Разговор о кувару',
            'uk': u'Обговорення рецепта',
        }

        self.namespaces[104] = {
            'he': u'מדף',
            'ka': u'თარო',
            'nl': u'Wikijunior',
            'pl': u'Wikijunior',
            'ro': u'Carte de bucate',
            'ru': u'Рецепт',
        }

        self.namespaces[105] = {
            'he': u'שיחת מדף',
            'ka': u'თარო განხილვა',
            'nl': u'Overleg Wikijunior',
            'pl': u'Dyskusja Wikijuniora',
            'ro': u'Discuţie Carte de bucate',
            'ru': u'Обсуждение рецепта',
        }

        self.namespaces[106] = {
            'ru': u'Задача',
        }

        self.namespaces[107] = {
            'ru': u'Обсуждение задачи',
        }

        self.namespaces[108] = {
            'en': u'Transwiki',
        }

        self.namespaces[109] = {
            'en': u'Transwiki talk',
        }

        self.namespaces[110] = {
            'en': u'Wikijunior',
            'tr': u'Vikiçocuk',
        }

        self.namespaces[111] = {
            'en': u'Wikijunior talk',
            'tr': u'Vikiçocuk tartışma',
        }

        self.namespaces[112] = {
            'en': u'Subject',
            'si': u'විෂයය',
            'tr': u'Kitaplık',
        }

        self.namespaces[113] = {
            'en': u'Subject talk',
            'si': u'විෂයය සාකච්ඡාව',
            'tr': u'Kitaplık tartışma',
        }

        self.namespaces[114] = {
            'si': u'කණිෂ්ඨ විකි',
        }

        self.namespaces[115] = {
            'si': u'කණිෂ්ඨ විකි සාකච්ඡාව',
        }

        # Which languages have a special order for putting interlanguage links,
        # and what order is it? If a language is not in interwiki_putfirst,
        # alphabetical order on language code is used. For languages that are in
        # interwiki_putfirst, interwiki_putfirst is checked first, and
        # languages are put in the order given there. All other languages are put
        # after those, in code-alphabetical order.

        alphabetic = ['af', 'ar', 'roa-rup', 'om', 'bg', 'be', 'bn', 'bs', 'ca',
                      'chr', 'co', 'cs', 'cy', 'da', 'de', 'als', 'et', 'el',
                      'en', 'es', 'eo', 'eu', 'fa', 'fr', 'fy', 'gv', 'gd',
                      'gl', 'ko', 'hi', 'hr', 'io', 'id', 'ia', 'is', 'it',
                      'he', 'jv', 'ka', 'csb', 'sw', 'la', 'lv', 'lt', 'hu',
                      'mk', 'mg', 'ml', 'mi', 'mr', 'ms', 'zh-cfr', 'mn', 'nah',
                      'na', 'nl', 'ja', 'no', 'nb', 'oc', 'nds', 'pl', 'pt',
                      'ro', 'ru', 'sa', 'st', 'sq', 'si', 'simple', 'sk', 'sl',
                      'sr', 'su', 'fi', 'sv', 'ta', 'tt', 'th', 'ur', 'vi',
                      'tpi', 'tr', 'uk', 'vo', 'yi', 'za', 'zh', 'zh-cn',
                      'zh-tw']

        self.obsolete = {
            'aa': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Afar_Wikibooks
            'ak': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Akan_Wikibooks
            'als': None, # http://als.wikipedia.org/wiki/Wikipedia:Stammtisch/Archiv_2008-1#Afterwards.2C_closure_and_deletion_of_Wiktionary.2C_Wikibooks_and_Wikiquote_sites
            'as': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Assamese_Wikibooks
            'ay': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Aymar_Wikibooks
            'ba': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Bashkir_Wikibooks
            'bi': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Bislama_Wikibooks
            'bm': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Bambara_Wikibooks
            'bo': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Tibetan_Wikibooks
            'ch': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Chamorro_Wikibooks
            'co': None, # https://bugzilla.wikimedia.org/show_bug.cgi?id=28644
            'dk': 'da',
            'ga':None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Gaeilge_Wikibooks
            'got': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Gothic_Wikibooks
            'gn': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Guarani_Wikibooks
            'gu': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Gujarati_Wikibooks
            'jp': 'ja',
            'km': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Khmer_Wikibooks
            'kn': None, # https://bugzilla.wikimedia.org/show_bug.cgi?id=20325
            'ks': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Kashmiri_Wikibooks
            'lb': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_L%C3%ABtzebuergesch_Wikibooks
            'ln': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Lingala_Wikibooks
            'lv': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Latvian_Wikibooks
            'mi': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Maori_Wikibooks
            'minnan':'zh-min-nan',
            'na': None, #http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Nauruan_Wikibooks
            'nah': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Nahuatl_Wikibooks
            'nb': 'no',
            'nds': None, #http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Plattd%C3%BC%C3%BCtsch_Wikibooks
            'ps': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Pashto_Wikibooks
            'qu': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Quechua_Wikibooks
            'rm': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Rumantsch_Wikibooks
            'se': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Sami_Wikibooks
            'simple' : 'en', # https://bugzilla.wikimedia.org/show_bug.cgi?id=20325
            'sw' : None, #https://bugzilla.wikimedia.org/show_bug.cgi?id=25170
            'tokipona': None,
            'ug': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Uyghur_Wikibooks
            'wa': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Walon_Wikibooks
            'xh': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Xhosa_Wikibooks
            'yo': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Yoruba_Wikibooks
            'za': None, # https://bugzilla.wikimedia.org/show_bug.cgi?id=20325
            'zh-tw': 'zh',
            'zh-cn': 'zh',
            'zu': None, # https://bugzilla.wikimedia.org/show_bug.cgi?id=25425
        }

        self.interwiki_putfirst = {
            'en': alphabetic,
            'fi': alphabetic,
            'fr': alphabetic,
            'he': ['en'],
            'hu': ['en'],
            'pl': alphabetic,
            'simple': alphabetic
        }
        # Global bot allowed languages on http://meta.wikimedia.org/wiki/Bot_policy/Implementation#Current_implementation
        self.cross_allowed = ['fa', 'fy', 'it', 'nl', 'ru', 'simple', 'zh']
        # CentralAuth cross avaliable projects.
        self.cross_projects = [
            'wikipedia', 'wiktionary', 'wikiquote', 'wikiquote', 'wikinews', 'wikiversity',
            'meta', 'mediawiki', 'test', 'incubator', 'commons', 'species'
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
            return '/%s/%s/w' % (self.name, code)

        def nicepath(self, code):
            return '/%s/%s/wiki/' % (self.name, code)
