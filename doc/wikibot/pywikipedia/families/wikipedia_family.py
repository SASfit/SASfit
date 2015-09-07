# -*- coding: utf-8  -*-
import family

__version__ = '$Id: wikipedia_family.py 9285 2011-06-03 10:36:34Z xqt $'

# The Wikimedia family that is known as Wikipedia, the Free Encyclopedia

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'wikipedia'

        self.languages_by_size = [
            'en', 'de', 'fr', 'it', 'pl', 'es', 'ja', 'ru', 'nl', 'pt', 'sv',
            'zh', 'ca', 'no', 'uk', 'fi', 'vi', 'cs', 'hu', 'tr', 'id', 'ko',
            'ro', 'fa', 'da', 'ar', 'eo', 'sr', 'lt', 'sk', 'ms', 'he', 'vo',
            'bg', 'sl', 'war', 'eu', 'hr', 'hi', 'et', 'gl', 'simple', 'new',
            'th', 'nn', 'az', 'el', 'roa-rup', 'la', 'ht', 'tl', 'ka', 'te',
            'mk', 'ceb', 'nap', 'sh', 'pms', 'br', 'be-x-old', 'lv', 'mr', 'jv',
            'lb', 'ta', 'sq', 'cy', 'is', 'bs', 'be', 'oc', 'an', 'bpy', 'bn',
            'io', 'sw', 'kk', 'lmo', 'fy', 'gu', 'ml', 'af', 'nds', 'ur', 'scn',
            'qu', 'mg', 'ku', 'zh-yue', 'su', 'ast', 'ne', 'hy', 'yo',
            'bat-smg', 'pnb', 'ga', 'cv', 'wa', 'kn', 'am', 'als', 'tg', 'vec',
            'roa-tara', 'zh-min-nan', 'yi', 'tt', 'bug', 'gd', 'os', 'uz',
            'sah', 'pam', 'arz', 'mi', 'li', 'hsb', 'sco', 'nah', 'mn', 'co',
            'gan', 'glk', 'my', 'ia', 'bcl', 'fo', 'sa', 'si', 'fiu-vro',
            'nds-nl', 'vls', 'tk', 'bar', 'ckb', 'mrj', 'gv', 'ilo', 'map-bms',
            'dv', 'nrm', 'pag', 'diq', 'se', 'rm', 'mzn', 'wuu', 'fur', 'ug',
            'bo', 'lij', 'mt', 'hif', 'csb', 'km', 'bh', 'ang', 'lad', 'nov',
            'udm', 'sc', 'zh-classical', 'cbk-zam', 'ps', 'pi', 'mhr', 'kv',
            'ksh', 'frp', 'rue', 'hak', 'kw', 'nv', 'pa', 'so', 'szl', 'xal',
            'ie', 'rw', 'stq', 'haw', 'pdc', 'koi', 'ln', 'krc', 'to', 'pcd',
            'crh', 'ext', 'ky', 'ace', 'myv', 'gn', 'ba', 'eml', 'ce', 'arc',
            'pap', 'ay', 'kl', 'bjn', 'frr', 'jbo', 'wo', 'or', 'tpi', 'kab',
            'ty', 'srn', 'zea', 'gag', 'dsb', 'ab', 'lo', 'ig', 'mdf', 'tet',
            'av', 'kg', 'mwl', 'rmy', 'ltg', 'cu', 'lbe', 'kaa', 'kbd', 'sm',
            'na', 'pfl', 'got', 'bm', 'as', 'ik', 'sd', 'bi', 'ks', 'iu', 'pih',
            'ss', 'pnt', 'chr', 'cdo', 'ee', 'ha', 'ti', 'bxr', 'za', 'om',
            'zu', 've', 'ts', 'rn', 'sg', 'dz', 'tum', 'cr', 'ch', 'lg', 'fj',
            'ny', 'st', 'xh', 'ff', 'tn', 'ki', 'sn', 'ak', 'chy', 'tw',
        ]

        if family.config.SSL_connection:
            self.langs = dict([(lang, None) for lang in self.languages_by_size])
        else:
            self.langs = dict([(lang, '%s.wikipedia.org' % lang) for lang in self.languages_by_size])

        # Override defaults
        self.namespaces[2]['cs'] = u'Wikipedista'
        self.namespaces[3]['cs'] = u'Diskuse s wikipedistou'
        self.namespaces[9]['da'] = u'MediaWiki-diskussion'
        self.namespaces[13]['da'] = u'Hjælp-diskussion'
        self.namespaces[1]['ja'] = [u'ノート', u'トーク']
        self.namespaces[3]['ja'] = [u'利用者‐会話', u'利用者・トーク']
        self.namespaces[7]['ja'] = [u'ファイル‐ノート', u'ファイル・トーク',
                                    u'Image talk', u'画像‐ノート']
        self.namespaces[9]['ja'] = [u'MediaWiki‐ノート', u'MediaWiki・トーク']
        self.namespaces[10]['ja'] = [u'Template', u'テンプレート']
        self.namespaces[11]['ja'] = [u'Template‐ノート', u'テンプレート・トーク']
        self.namespaces[12]['ja'] = [u'Help', u'ヘルプ']
        self.namespaces[13]['ja'] = [u'Help‐ノート', u'ヘルプ・トーク']
        self.namespaces[14]['ja'] = [u'Category', u'カテゴリ']
        self.namespaces[15]['ja'] = [u'Category‐ノート', u'カテゴリ・トーク']
        self.namespaces[2]['ka'] = [u'ვიკიპედიელი', u'მომხმარებელი']
        self.namespaces[3]['ka'] = [u'ვიკიპედიელის განხილვა', u'მომხმარებელი განხილვა', u'მომხმარებლის განხილვა']
        self.namespaces[2]['pl'] = u'Wikipedysta'
        self.namespaces[3]['pl'] = u'Dyskusja wikipedysty'
        self.namespaces[2]['pt'] = u'Usuário'
        self.namespaces[3]['pt'] = u'Usuário Discussão'

        # Most namespaces are inherited from family.Family.
        # Translation used on all wikis for the different namespaces.
        # (Please sort languages alphabetically)
        # You only need to enter translations that differ from _default.
        self.namespaces[4] = {
            '_default': [u'Wikipedia', self.namespaces[4]['_default']],
            'ab': u'Авикипедиа',
            'am': u'ውክፔዲያ',
            'ar': u'ويكيبيديا',
            'arc': u'ܘܝܩܝܦܕܝܐ',
            'arz': u'ويكيبيديا',
            'ast': u'Uiquipedia',
            'az': u'Vikipediya',
            'bat-smg': u'Vikipedėjė',
            'be': u'Вікіпедыя',
            'be-x-old': u'Вікіпэдыя',
            'bg': u'Уикипедия',
            'bh': u'विकिपीडिया',
            'bjn': u'Wikipidia',
            'bn': u'উইকিপিডিয়া',
            'bpy': u'উইকিপিডিয়া',
            'ca': u'Viquipèdia',
            'ce': u'Википедийа',
            'ckb': u'ویکیپیدیا',
            'crh': u'Vikipediya',
            'cs': u'Wikipedie',
            'csb': u'Wiki',
            'cu': [u'Википє́дїꙗ', u'Википедї'],
            'cv': u'Википеди',
            'cy': u'Wicipedia',
            'dsb': u'Wikipedija',
            'el': u'Βικιπαίδεια',
            'en': [u'Wikipedia', u'WP'],
            'eo': [u'Vikipedio', u'VP'],
            'et': u'Vikipeedia',
            'ext': u'Güiquipeya',
            'fa': [u'ویکی‌پدیا', u'وپ'],
            'fr': [u'Wikipédia', u'Wikipedia'],
            'frp': u'Vouiquipèdia',
            'fur': u'Vichipedie',
            'fy': u'Wikipedy',
            'ga': u'Vicipéid',
            'gag': u'Vikipediya',
            'gd': u'Uicipeid',
            'gn': u'Vikipetã',
            'gu': u'વિકિપીડિયા',
            'he': u'ויקיפדיה',
            'hi': u'विकिपीडिया',
            'hr': u'Wikipedija',
            'hsb': u'Wikipedija',
            'ht': u'Wikipedya',
            'hu': u'Wikipédia',
            'hy': u'Վիքիփեդիա',
            'io': u'Wikipedio',
            'ka': u'ვიკიპედია',
            'kbd': u'Уикипедиэ',
            'kk': u'Уикипедия',
            'km': u'វិគីភីឌា',
            'kn': u'ವಿಕಿಪೀಡಿಯ',
            'ko': u'위키백과',
            'koi': u'Википедия',
            'krc': u'Википедия',
            'ku': u'Wîkîpediya',
            'la': u'Vicipaedia',
            'lbe': u'Википедия',
            'lo': u'ວິກິພີເດຍ',
            'lt': u'Vikipedija',
            'ltg': u'Vikipedeja',
            'lv': u'Vikipēdija',
            'mdf': u'Википедиесь',
            'mhr': u'Википедий',
            'mk': u'Википедија',
            'ml': u'വിക്കിപീഡിയ',
            'mr': u'विकिपीडिया',
            'mrj': u'Википеди',
            'mt': u'Wikipedija',
            'mwl': u'Biquipédia',
            'myv': u'Википедиясь',
            'nah': u'Huiquipedia',
            'nds-nl': u'Wikipedie',
            'ne': u'विकिपीडिया',
            'new': u'विकिपिडिया',
            'nv': u'Wikiibíídiiya',
            'oc': u'Wikipèdia',
            'or': u'ଉଇକିପିଡ଼ିଆ',
            'os': u'Википеди',
            'pa': u'ਵਿਕਿਪੀਡਿਆ',
            'pnt': u'Βικιπαίδεια',
            'ps': u'ويکيپېډيا',
            'pt': u'Wikipédia',
            'rmy': u'Vikipidiya',
            'ru': u'Википедия',
            'rue': u'Вікіпедія',
            'sa': u'विकिपीडिया',
            'sah': u'Бикипиэдьийэ',
            'si': u'විකිපීඩියා',
            'sk': u'Wikipédia',
            'sl': u'Wikipedija',
            'sr': u'Википедија',
            'szl': u'Wikipedyjo',
            'ta': u'விக்கிப்பீடியா',
            'te': u'వికీపీడియా',
            'tg': u'Википедиа',
            'th': u'วิกิพีเดีย',
            'tk': u'Wikipediýa',
            'tr': u'Vikipedi',
            'tt': u'Википедия',
            'uk': u'Вікіпедія',
            'ur': u'منصوبہ',
            'uz': u'Vikipediya',
            'vo': u'Vükiped',
            'yi': [u'װיקיפּעדיע', u'וויקיפעדיע'],
            'zh': [u'Wikipedia', u'维基百科'],
            'zh-classical': u'維基大典',
        }

        self.namespaces[5] = {
            '_default': [u'Wikipedia talk', self.namespaces[5]['_default']],
            'ab': u'Авикипедиа ахцәажәара',
            'ace': u'Marit Wikipedia',
            'af': u'Wikipediabespreking',
            'als': u'Wikipedia Diskussion',
            'am': u'ውክፔዲያ ውይይት',
            'an': u'Descusión Wikipedia',
            'ar': u'نقاش ويكيبيديا',
            'arc': [u'ܡܡܠܠܐ ܕܘܝܩܝܦܕܝܐ', u'ܘܝܩܝܦܕܝܐ talk'],
            'arz': u'نقاش ويكيبيديا',
            'as': [u'Wikipedia বাৰ্তা', u'Wikipedia वार्ता', u'Wikipedia বার্তা'],
            'ast': u'Uiquipedia alderique',
            'av': u'Обсуждение Wikipedia',
            'ay': u'Wikipedia Discusión',
            'az': u'Vikipediya müzakirəsi',
            'ba': u'Wikipedia б-са фекер алышыу',
            'bar': u'Wikipedia Diskussion',
            'bat-smg': u'Vikipedėjės aptarėms',
            'bcl': u'Olay sa Wikipedia',
            'be': u'Вікіпедыя размовы',
            'be-x-old': u'Абмеркаваньне Вікіпэдыя',
            'bg': u'Уикипедия беседа',
            'bh': u'विकिपीडिया वार्ता',
            'bjn': u'Wikipidia pamandiran',
            'bm': u'Discussion Wikipedia',
            'bn': u'উইকিপিডিয়া আলোচনা',
            'bpy': u'উইকিপিডিয়া য়্যারী',
            'br': [u'Kaozeadenn Wikipedia', u'Discussion Wikipedia'],
            'bs': u'Razgovor s Wikipediom',
            'bug': u'Pembicaraan Wikipedia',
            'ca': u'Viquipèdia Discussió',
            'cbk-zam': u'Wikipedia Discusión',
            'ce': [u'Википедийа дийцаре', u'Википедийа Дийца', u'Дийцаре Википедийа'],
            'ceb': [u'Hisgot sa Wikipedia', u'Hisgot saWikipedia'],
            'ch': u'Kombetsasion nu Wikipedia',
            'ckb': [u'لێدوانی ویکیپیدیا', u'لێدوانی Wikipedia'],
            'crh': [u'Vikipediya muzakeresi', u'Vikipediya музакереси'],
            'cs': u'Diskuse k Wikipedii',
            'csb': u'Diskùsëjô Wiki',
            'cu': [u'Википє́дїѩ бєсѣ́да', u'Википедїѩ бєсѣ́да'],
            'cv': u'Википеди сӳтсе явмалли',
            'cy': u'Sgwrs Wicipedia',
            'da': u'Wikipedia-diskussion',
            'de': u'Wikipedia Diskussion',
            'diq': u'Wikipedia talk',
            'dsb': u'Wikipedija diskusija',
            'el': u'Βικιπαίδεια συζήτηση',
            'eml': u'Discussioni Wikipedia',
            'en': [u"Wikipedia talk", u"WT"],
            'eo': u'Vikipedia diskuto',
            'es': u'Wikipedia Discusión',
            'et': u'Vikipeedia arutelu',
            'eu': u'Wikipedia eztabaida',
            'ext': u'Güiquipeya talk',
            'fa': u'بحث ویکی‌پدیا',
            'ff': u'Discussion Wikipedia',
            'fi': u'Keskustelu Wikipediasta',
            'fiu-vro': u'Wikipedia arotus',
            'fo': u'Wikipedia-kjak',
            'fr': [u'Discussion Wikipédia', u'Discussion Wikipedia'],
            'frp': u'Discussion Vouiquipèdia',
            'frr': u'Wikipedia Diskussion',
            'fur': u'Discussion Vichipedie',
            'fy': u'Wikipedy oerlis',
            'ga': u'Plé Vicipéide',
            'gag': u'Vikipediyanın laflanması',
            'gan': u'Wikipedia talk',
            'gd': u'Uicipeid talk',
            'gl': u'Conversa Wikipedia',
            'glk': u'بحث Wikipedia',
            'gn': u'Vikipetã myangekõi',
            'gu': u'વિકિપીડિયા ચર્ચા',
            'gv': u'Resooney Wikipedia',
            'haw': u'Kūkākūkā o Wikipikia',
            'he': u'שיחת ויקיפדיה',
            'hi': u'विकिपीडिया वार्ता',
            'hr': u'Razgovor Wikipedija',
            'hsb': u'Wikipedija diskusija',
            'ht': u'Diskisyon Wikipedya',
            'hu': u'Wikipédia-vita',
            'hy': u'Վիքիփեդիայի քննարկում',
            'ia': u'Discussion Wikipedia',
            'id': u'Pembicaraan Wikipedia',
            'ig': u'Okwu Wikipedia',
            'io': u'Wikipedio Debato',
            'is': u'Wikipediaspjall',
            'it': u'Discussioni Wikipedia',
            'ja': [u'Wikipedia‐ノート', u'Wikipedia・トーク'],
            'jv': u'Dhiskusi Wikipedia',
            'ka': u'ვიკიპედია განხილვა',
            'kaa': u'Wikipedia sa\'wbeti',
            'kab': u'Amyannan n Wikipedia',
            'kbd': u'Уикипедиэм и тепсэлъыхьыгъуэ',
            'kk': u'Уикипедия талқылауы',
            'kl': u'Wikipedia-p oqalliffia',
            'km': u'ការពិភាក្សាអំពីវិគីភីឌា',
            'kn': u'ವಿಕಿಪೀಡಿಯ ಚರ್ಚೆ',
            'ko': u'위키백과토론',
            'koi': u'Баитам Википедия йылiсь',
            'krc': u'Википедия сюзюу',
            'ksh': u'Wikipedia Klaaf',
            'ku': u'Wîkîpediya nîqaş',
            'kv': u'Обсуждение Wikipedia',
            'kw': [u'Kescows Wikipedia', u'Cows Wikipedia', u'Keskows Wikipedia'],
            'la': [u'Disputatio Vicipaediae', u'Disputatio Wikipedia'],
            'lad': u'Diskusyón de Wikipedia',
            'lb': u'Wikipedia Diskussioun',
            'lbe': u'Википедиялиясса ихтилат',
            'li': u'Euverlèk Wikipedia',
            'lij': u'Discûscioîn Wikipedia',
            'lmo': u'Wikipedia Ciciarada',
            'ln': u'Discussion Wikipedia',
            'lo': u'ສົນທະນາກ່ຽວກັບວິກິພີເດຍ',
            'lt': [u'Vikipedijos aptarimas', u'Wikipedia aptarimas'],
            'ltg': u'Vikipedejis sprīža',
            'lv': u'Vikipēdijas diskusija',
            'map-bms': u'Dhiskusi Wikipedia',
            'mdf': u'Википедиесь корхнема',
            'mg': u'Dinika amin\'ny Wikipedia',
            'mhr': [u'Википедийын каҥашымаш', u'Wikipediaын каҥашымаш'],
            'mk': u'Разговор за Википедија',
            'ml': u'വിക്കിപീഡിയ സംവാദം',
            'mn': u'Wikipedia-н хэлэлцүүлэг',
            'mr': u'विकिपीडिया चर्चा',
            'mrj': u'Википедим кӓнгӓшӹмӓш',
            'ms': u'Perbincangan Wikipedia',
            'mt': u'Diskussjoni Wikipedija',
            'mwl': u'Biquipédia cumbersa',
            'myv': u'Википедиясь кортамось',
            'mzn': u'Wikipedia گپ',
            'nah': [u'Huiquipedia tēixnāmiquiliztli', u'Wikipedia tēixnāmiquiliztli', u'Wikipedia Discusión'],
            'nap': [u'Wikipedia chiàcchiera', u'Discussioni Wikipedia'],
            'nds': u'Wikipedia Diskuschoon',
            'nds-nl': u'Overleg Wikipedie',
            'ne': u'विकिपीडिया वार्ता',
            'new': u'विकिपिडिया खँलाबँला',
            'nl': u'Overleg Wikipedia',
            'nn': u'Wikipedia-diskusjon',
            'no': u'Wikipedia-diskusjon',
            'nv': u'Wikiibíídiiya baa yáshtiʼ',
            'oc': u'Discussion Wikipèdia',
            'or': u'ଉଇକିପିଡ଼ିଆ ଆଲୋଚନା',
            'os': u'Дискусси Википеди',
            'pa': u'ਵਿਕਿਪੀਡਿਆ ਚਰਚਾ',
            'pcd': u'Discussion Wikipedia',
            'pdc': u'Wikipedia Diskussion',
            'pfl': u'Wikipedia Diskussion',
            'pl': u'Dyskusja Wikipedii',
            'pms': u'Discussion ant sla Wikipedia',
            'pnt': u'Βικιπαίδεια καλάτσεμαν',
            'ps': u'د ويکيپېډيا خبرې اترې',
            'pt': u'Wikipédia Discussão',
            'qu': u'Wikipedia rimanakuy',
            'rm': u'Wikipedia discussiun',
            'rmy': u'Vikipidiyake vakyarimata',
            'ro': u'Discuție Wikipedia',
            'ru': u'Обсуждение Википедии',
            'rue': u'Діскузія ку Вікіпедії',
            'sa': [u'विकिपीडियासम्भाषणम्', u'विकिपीडियासंभाषणं'],
            'sah': u'Бикипиэдьийэ ырытыыта',
            'sc': u'Cuntierra Wikipedia',
            'scn': u'Discussioni Wikipedia',
            'sd': u'Wikipedia بحث',
            'sg': u'Discussion Wikipedia',
            'sh': u'Razgovor o Wikipedia',
            'si': u'විකිපීඩියා සාකච්ඡාව',
            'sk': u'Diskusia k Wikipédii',
            'sl': u'Pogovor o Wikipediji',
            'sq': u'Wikipedia diskutim',
            'sr': u'Разговор о Википедији',
            'srn': u'Taki fu Wikipedia',
            'stq': u'Wikipedia Diskussion',
            'su': u'Obrolan Wikipedia',
            'sv': u'Wikipediadiskussion',
            'sw': u'Majadiliano ya Wikipedia',
            'szl': u'Dyskusja Wikipedyjo',
            'ta': u'விக்கிப்பீடியா பேச்சு',
            'te': u'వికీపీడియా చర్చ',
            'tet': u'Diskusaun Wikipedia',
            'tg': u'Баҳси Википедиа',
            'th': u'คุยเรื่องวิกิพีเดีย',
            'tk': u'Wikipediýa çekişme',
            'tl': u'Usapang Wikipedia',
            'tr': u'Vikipedi tartışma',
            'tt': u'Википедия бәхәсе',
            'ty': u'Discussion Wikipedia',
            'udm': u'Wikipedia сярысь вераськон',
            'ug': u'مۇنازىرىسىWikipedia',
            'uk': u'Обговорення Вікіпедії',
            'ur': u'تبادلۂ خیال منصوبہ',
            'uz': u'Vikipediya munozarasi',
            'vec': u'Discussion Wikipedia',
            'vi': u'Thảo luận Wikipedia',
            'vls': u'Discuusje Wikipedia',
            'vo': u'Bespik dö Vükiped',
            'wa': u'Wikipedia copene',
            'wo': u'Wikipedia waxtaan',
            'wuu': u'Wikipedia讨论',
            'xal': u'Wikipedia туск меткән',
            'yi': [u'װיקיפּעדיע רעדן', u'וויקיפעדיע רעדן'],
            'yo': u'Ọ̀rọ̀ Wikipedia',
            'za': u'Wikipedia讨论',
            'zea': u'Overleg Wikipedia',
            'zh': [u'Wikipedia talk', u'维基百科讨论'],
            'zh-classical': u'維基大典 talk',
        }

        self.namespaces[90] = {
            'hu': u'Téma',
        }

        self.namespaces[91] = {
            'hu': u'Témavita',
        }

        self.namespaces[92] = {
            'hu': u'Összefoglaló',
        }

        self.namespaces[93] = {
            'hu': u'Összefoglaló-vita',
        }

        self.namespaces[100] = {
            'af': u'Portaal',
            'als': u'Portal',
            'am': u'በር',
            'an': u'Portal',
            'ar': u'بوابة',
            'arz': u'بوابة',
            'az': u'Portal',
            'bar': u'Portal',
            'be-x-old': u'Партал',
            'bg': u'Портал',
            'bn': u'প্রবেশদ্বার',
            'bpy': u'হমিলদুৱার',
            'ca': u'Portal',
            'ce': u'Ков',
            'ckb': u'دەروازە',
            'cs': u'Portál',
            'cy': u'Porth',
            'da': u'Portal',
            'de': u'Portal',
            'dv': u'ނެރު',
            'el': u'Πύλη',
            'en': u'Portal',
            'eo': u'Portalo',
            'es': u'Portal',
            'et': u'Portaal',
            'eu': u'Atari',
            'fa': u'درگاه',
            'fi': u'Teemasivu',
            'fr': u'Portail',
            'gl': u'Portal',
            'he': u'פורטל',
            'hi': u'प्रवेशद्वार',
            'hr': u'Portal',
            'hu': u'Portál',
            'ia': u'Portal',
            'id': u'Portal',
            'is': u'Gátt',
            'it': u'Portale',
            'ja': u'Portal',
            'ka': u'პორტალი',
            'kk': u'Портал',
            'ko': u'들머리',
            'kw': u'Porth',
            'la': u'Porta',
            'li': u'Portaol',
            'lmo': u'Portal',
            'lt': u'Vikisritis',
            'lv': u'Portāls',
            'mk': u'Портал',
            'ml': u'കവാടം',
            'mr': u'दालन',
            'ms': u'Portal',
            'mt': u'Portal',
            'nds': u'Portal',
            'new': u'दबू',
            'nl': u'Portaal',
            'nn': u'Tema',
            'no': u'Portal',
            'oc': u'Portal',
            'pl': u'Portal',
            'pt': u'Portal',
            'ro': u'Portal',
            'roa-tara': u'Portale',
            'ru': u'Портал',
            'scn': u'Purtali',
            'si': u'ද්වාරය',
            'sk': u'Portál',
            'sl': u'Portal',
            'sq': u'Portal',
            'sr': u'Портал',
            'su': u'Portal',
            'sv': u'Portal',
            'sw': u'Lango',
            'ta': u'வலைவாசல்',
            'te': u'వేదిక',
            'tg': u'Портал',
            'th': u'สถานีย่อย',
            'tr': u'Portal',
            'tt': u'Портал',
            'uk': u'Портал',
            'vec': u'Portałe',
            'vi': u'Chủ đề',
            'wuu': u'Transwiki',
            'yi': u'פארטאל',
            'yo': u'Èbúté',
            'zh': u'Portal',
            'zh-classical': u'門',
            'zh-min-nan': u'Portal',
            'zh-yue': u'Portal',
        }

        self.namespaces[101] = {
            'af': u'Portaalbespreking',
            'als': u'Portal Diskussion',
            'am': u'በር ውይይት',
            'an': u'Descusión Portal',
            'ar': u'نقاش البوابة',
            'arz': u'مناقشة بوابة',
            'az': u'Portal müzakirəsi',
            'bar': u'Portal Diskussion',
            'be-x-old': u'Абмеркаваньне парталу',
            'bg': u'Портал беседа',
            'bn': u'প্রবেশদ্বার আলোচনা',
            'bpy': u'হমিলদুৱার য়্যারী',
            'ca': u'Portal Discussió',
            'ce': u'Ков дийцаре',
            'ckb': u'لێدوانی دەروازە',
            'cs': u'Diskuse k portálu',
            'cy': u'Sgwrs Porth',
            'da': [u'Portaldiskussion', u'Portal diskussion'],
            'de': u'Portal Diskussion',
            'dv': u'ނެރު ޚ ޔާލު',
            'el': u'Συζήτηση πύλης',
            'en': u'Portal talk',
            'eo': u'Portala diskuto',
            'es': u'Portal Discusión',
            'et': u'Portaali arutelu',
            'eu': u'Atari eztabaida',
            'fa': u'بحث درگاه',
            'fi': u'Keskustelu teemasivusta',
            'fr': u'Discussion Portail',
            'gl': u'Portal talk',
            'he': u'שיחת פורטל',
            'hi': u'प्रवेशद्वार वार्ता',
            'hr': u'Razgovor o portalu',
            'hu': u'Portálvita',
            'ia': u'Discussion Portal',
            'id': u'Pembicaraan Portal',
            'is': u'Gáttaspjall',
            'it': u'Discussioni portale',
            'ja': [u'Portal‐ノート', u'ポータル‐ノート', u'Portal・トーク'],
            'ka': u'პორტალი განხილვა',
            'kk': u'Портал талқылауы',
            'ko': u'들머리토론',
            'kw': u'Keskows Porth',
            'la': u'Disputatio Portae',
            'li': u'Euverlèk portaol',
            'lmo': u'Descüssiú Portal',
            'lt': u'Vikisrities aptarimas',
            'lv': u'Portāla diskusija',
            'mk': u'Разговор за Портал',
            'ml': u'കവാടത്തിന്റെ സംവാദം',
            'mr': u'दालन चर्चा',
            'ms': [u'Perbualan Portal', u'Portal talk'],
            'mt': u'Diskussjoni portal',
            'nds': u'Portal Diskuschoon',
            'new': u'दबू खँलाबँला',
            'nl': u'Overleg portaal',
            'nn': u'Temadiskusjon',
            'no': u'Portaldiskusjon',
            'oc': u'Discussion Portal',
            'pl': u'Dyskusja portalu',
            'pt': [u'Portal Discussão', u'Discussão Portal'],
            'ro': u'Discuție Portal',
            'roa-tara': u'\'Ngazzaminde d\'u Portale',
            'ru': u'Обсуждение портала',
            'scn': u'Discussioni purtali',
            'si': u'ද්වාරය සාකච්ඡාව',
            'sk': u'Diskusia k portálu',
            'sl': u'Pogovor o portalu',
            'sq': u'Portal diskutim',
            'sr': u'Разговор о порталу',
            'su': u'Obrolan portal',
            'sv': u'Portaldiskussion',
            'sw': u'Majadiliano ya lango',
            'ta': u'வலைவாசல் பேச்சு',
            'te': u'వేదిక చర్చ',
            'tg': u'Баҳси портал',
            'th': u'คุยเรื่องสถานีย่อย',
            'tr': u'Portal tartışma',
            'tt': u'Портал бәхәсе',
            'uk': u'Обговорення порталу',
            'vec': u'Discussion portałe',
            'vi': u'Thảo luận Chủ đề',
            'wuu': u'Transwiki talk',
            'yi': u'פארטאל רעדן',
            'yo': u'Ọ̀rọ̀ èbúté',
            'zh': u'Portal talk',
            'zh-classical': u'議',
            'zh-min-nam': u'Portal talk',
            'zh-min-nan': u'Portal talk',
            'zh-yue': u'Portal talk',
        }

        self.namespaces[102] = {
            'als': u'Buech',
            'ca': u'Viquiprojecte',
            'cs': u'Rejstřík',
            'da': u'Artikeldata',
            'eo': u'Projekto',
            'es': u'Wikiproyecto',
            'eu': u'Wikiproiektu',
            'fi': u'Metasivu',
            'fr': u'Projet',
            'gl': u'Libro',
            'hr': u'Dodatak',
            'ia': u'Appendice',
            'it': u'Progetto',
            'ja': u'プロジェクト',
            'ko': u'위키프로젝트',
            'lmo': u'Purtaal',
            'lt': u'Vikiprojektas',
            'lv': u'Vikiprojekts',
            'oc': u'Projècte',
            'pl': u'Wikiprojekt',
            'pt': u'Anexo',
            'ro': u'Proiect',
            'scn': u'Pruggettu',
            'vec': u'Projeto',
        }

        self.namespaces[103] = {
            'als': u'Buech Diskussion',
            'ca': u'Viquiprojecte Discussió',
            'cs': u'Diskuse k rejstříku',
            'da': u'Artikeldatadiskussion',
            'eo': u'Projekta diskuto',
            'es': u'Wikiproyecto Discusión',
            'eu': u'Wikiproiektu eztabaida',
            'fi': u'Keskustelu metasivusta',
            'fr': u'Discussion Projet',
            'gl': u'Conversa libro',
            'hr': u'Razgovor o dodatku',
            'ia': u'Discussion Appendice',
            'it': u'Discussioni progetto',
            'ja': u'プロジェクト‐ノート',
            'ko': u'위키프로젝트토론',
            'lmo': u'Descüssiun Purtaal',
            'lt': u'Vikiprojekto aptarimas',
            'lv': u'Vikiprojekta diskusija',
            'oc': u'Discussion Projècte',
            'pl': u'Dyskusja Wikiprojektu',
            'pt': u'Anexo Discussão',
            'ro': u'Discuție Proiect',
            'scn': u'Discussioni pruggettu',
            'vec': u'Discussion projeto',
        }

        self.namespaces[104] = {
            'als': u'Wort',
            'ar': u'ملحق',
            'es': u'Anexo',
            'fi': u'Kirja',
            'fr': u'Référence',
            'lt': u'Sąrašas',
            'pt': u'Livro',
        }

        self.namespaces[105] = {
            'als': u'Wort Diskussion',
            'ar': u'نقاش الملحق',
            'es': u'Anexo Discusión',
            'fi': u'Keskustelu kirjasta',
            'fr': u'Discussion Référence',
            'lt': u'Sąrašo aptarimas',
            'pt': u'Livro Discussão',
        }

        self.namespaces[106] = {
            'als': u'Text',
        }

        self.namespaces[107] = {
            'als': u'Text Diskussion',
        }

        self.namespaces[108] = {
            'als': u'Spruch',
            'en': u'Book',
            'yo': u'Ìwé',
        }

        self.namespaces[109] = {
            'als': u'Spruch Diskussion',
            'en': u'Book talk',
            'yo': u'Ọ̀rọ̀ ìwé',
        }

        self.namespaces[110] = {
            'als': u'Nochricht',
        }

        self.namespaces[111] = {
            'als': u'Nochricht Diskussion',
        }

        self.category_redirect_templates = {
            '_default': (),
            'ar': (u"تحويل تصنيف",
                   u"تحويلة تصنيف",
                   u"Category redirect",
                   u"تحويلة تصنيف",),
            'arz': (u'تحويل تصنيف',),
            'cs': (u'Zastaralá kategorie',),
            'da': (u'Kategoriomdirigering',),
            'de': (u'Kategorieweiterleitung',),
            'en': (u"Category redirect",
                   u"Category redirect3",
                   u"Categoryredirect",
                   u"CR",
                   u"Catredirect",
                   u"Cat redirect",
                   u"Catredir",
                   u"Seecat",),
            'es': (u'Categoría redirigida',),
            'eu': (u'Kategoria redirect',),
            'fa': (u'رده بهتر',
                   u'انتقال رده',
                   u'فیلم‌های امریکایی',),
            'fr': (u'Redirection de catégorie',),
            'hi': (u'श्रेणीअनुप्रेषित',
                   u'Categoryredirect',),
            'hu': (u'Kat-redir',
                   u'Katredir',
                   u'Kat-redirekt',),
            'id': (u'Alih kategori',
                   u'Alihkategori',),
            # 'it' has removed its template
            # 'ja' is discussing to remove this template
            'ja': (u"Category redirect",),
            'ko': (u'분류 넘겨주기',),
            'mk': (u'Премести категорија',),
            'ms': (u'Pengalihan kategori',
                   u'Categoryredirect',
                   u'Category redirect',),
            'mt': (u'Redirect kategorija',),
            # 'nl' has removed its template
            'no': (u"Category redirect",
                   u"Kategoriomdirigering",
                   u"Kategori-omdirigering",),
            'pl': (u'Przekierowanie kategorii',
                   u'Category redirect',),
            'pt': (u'Redirecionamento de categoria',
                   u'Redircat',
                   u'Redirect-categoria',),
            'ro': (u'Redirect categorie',),
            'ru': (u'Переименованная категория',
                   u'Categoryredirect',
                   u'CategoryRedirect',
                   u'Category redirect',
                   u'Catredirect',),
            'simple': (u"Category redirect",
                       u"Categoryredirect",
                       u"Catredirect",),
            'sq': (u'Kategori e zhvendosur',
                   u'Category redirect',),
            'sv': (u'Kategoriomdirigering',
                   u'Omdirigering kategori',),
            'tl': (u'Category redirect',),
            'tr': (u'Kategori yönlendirme',
                   u'Kat redir',),
            'uk': (u'Categoryredirect',),
            'vi': (u'Đổi hướng thể loại',
                   u'Thể loại đổi hướng',
                   u'Chuyển hướng thể loại',
                   u'Categoryredirect',
                   u'Category redirect',
                   u'Catredirect',),
            'yi': (u'קאטעגאריע אריבערפירן',),
            'zh': (u'分类重定向',
                   u'Cr',
                   u'CR',
                   u'Cat-redirect',),
            'zh-yue': (u'Category redirect',
                       u'分類彈去',
                       u'分類跳轉',),
        }

        self.disambiguationTemplates = {
            # If no templates are given, retrieve names from  the live wiki
            # ([[MediaWiki:Disambiguationspage]])
            '_default': [u'Disambig'],
            'ang': [u'Disambig', u'Geodis'],
            'arc': [u'ܕ'],
            'ast': [u'Dixebra'],
            'az':  [u'Dəqiqləşdirmə'],
            'ba':  [u'Күп мәғәнәлелек'],
            'bcl': [u'Clarip'],
            'bn':  [u'দ্ব্যর্থতা নিরসন', u'Disambig'],
            'cdo': [u'Gì-ngiê'],
            'ext': [u'Desambiguáncia'],
            'fo':  [u'Fleiri týdningar'],
            'frp': [u'Homonimos'],
            'fur': [u'Disambiguazion', u'Disambig'],
            'fy':  [u'Tfs', u'Neibetsjuttings'],
            'gan': [u'扤清楚', u'Disambig'],
            'gd':  [u'Soilleireachadh'],
            'haw': [u'Huaʻōlelo puana like'],
            'hr':  [u'Preusmjerenje u razdvojbu', u'Razdvojba', u'razdvojba1',
                    u'Nova razdvojba'],
            'hy':  [u'Երկիմաստ', u'Բազմիմաստություն', u'Բազմանշանակ', u'Disambig'],
            'kg':  [u'Bisongidila'],
            'krc': [u'Кёб магъаналы', u'Disambig'],
            'la':  [u'Discretiva', u'Disnomen'], #{{Disambig}} is not a disambig
            'lb':  [u'Homonymie', u'Disambig', u'Homonymie Ofkierzungen'],
            'li':  [u'Verdudeliking', u'Verdudelikingpazjena', u'Vp'],
            'lmo': [u'Desambiguació', u'Dezambiguasiú', u'Desambiguazion',
                    u'Desambiguassiú', u'Desambiguació'],
            'mn':  [u'Салаа утгатай', u'Disambig'],
            'nl':  [u'Dp', u'DP', u'Dp2', u'Dpintro', u'Cognomen',
                    u'Dp cognomen'],
            'nn':  [u'Fleirtyding', u'Tobokstavforkorting', u'Pekerside',
                    u'Peikar', u'Disambig'],
            'no':  [u'Peker', u'Etternavn', u'Disambig',
                    u'Tobokstavsforkortelse', u'Trebokstavsforkortelse',
                    u'Flertydig', u'Pekerside'],
            'nov': [u'Desambig'],
            'nrm': [u'Page dé frouque'],
            'qu':  [u"Sut'ichana qillqa", u'Disambig', u'SJM'],
            'rmy': [u'Dudalipen'],
            'ro':  [u'Dezambiguizare', u'Disambig', u'Hndis', u'Dez',
                    u'Dezamb'],
            'sc':  [u'Disambigua'],
            'sk':  [u'Disambig', u'Rozlišovacia stránka', u'Disambiguation'],
            'srn': [u'Dp'],
            'tg':  [u'Ибҳомзудоӣ', u'Disambig', u'Рафъи ибҳом',
                    u'Disambiguation'],
            'tr':  [u'Anlam ayrım', u'Disambig', u'Anlam ayrımı',
                    u'Kişi adları (anlam ayrımı)',
                    u'Yerleşim yerleri (anlam ayrımı)',
                    u'kısaltmalar (anlam ayrımı)', u'Coğrafya (anlam ayrımı)',
                    u'Yerleşim yerleri (anlam ayrımı)'],
            'vls': [u'Db', u'Dp', u'Dpintro'],
            'wo':  [u'Bokktekki'],
            'yi':  [u'באדייטען'],
            'zea': [u'dp', u'Deurverwiespagina'],
            'zh':  [u'Disambig', u'消歧义', u'消歧义页', u'消歧義', u'消歧義頁',
                    u'Letter disambig', u'Isomerdab'],
            'zh-classical':  [u'Disambig', u'釋義', u'消歧義'],
            'zh-yue': [u'搞清楚', u'Disambig'],
        }

        self.disambcatname = {
            'af':  u'dubbelsinnig',
            'als': u'Begriffsklärung',
            'ang': u'Scīrung',
            'ast': u'Dixebra',
            'ar':  u'صفحات توضيح',
            'be':  u'Disambig',
            'be-x-old':  u'Вікіпэдыя:Неадназначнасьці',
            'bg':  u'Пояснителни страници',
            'ca':  u'Viquipèdia:Registre de pàgines de desambiguació',
            'cbk-zam': u'Desambiguo',
            'cs':  u'Rozcestníky',
            'cy':  u'Gwahaniaethu',
            'da':  u'Flertdig',
            'de':  u'Begriffsklärung',
            'el':  u'Αποσαφήνιση',
            'en':  u'All disambiguation pages',
            'eo':  u'Apartigiloj',
            'es':  u'Desambiguación',
            'et':  u'Täpsustusleheküljed',
            'eu':  u'Argipen orriak',
            'fa':  u'صفحه‌های ابهام‌زدایی',
            'fi':  u'Täsmennyssivut',
            'fo':  u'Fleiri týdningar',
            'fr':  u'Homonymie',
            'fy':  u'Trochferwiisside',
            'ga':  u'Idirdhealáin',
            'gl':  u'Homónimos',
            'he':  u'פירושונים',
            'hu':  u'Egyértelműsítő lapok',
            'ia':  u'Disambiguation',
            'id':  u'Disambiguasi',
            'io':  u'Homonimi',
            'is':  u'Aðgreiningarsíður',
            'it':  u'Disambigua',
            'ja':  u'曖昧さ回避',
            'ka':  u'მრავალმნიშვნელოვანი',
            'kw':  u'Folennow klerheans',
            'ko':  u'동음이의어 문서',
            'ku':  u'Rûpelên cudakirinê',
            'krc': u'Кёб магъаналы терминле',
            'ksh': u'Woot met mieh wi ëijnem Senn',
            'la':  u'Discretiva',
            'lb':  u'Homonymie',
            'li':  u'Verdudelikingspazjena',
            'ln':  u'Bokokani',
            'lt':  u'Nuorodiniai straipsniai',
            'ms':  u'Nyahkekaburan',
            'mt':  u'Diżambigwazzjoni',
            'nds': u'Mehrdüdig Begreep',
            'nds-nl': u'Deurverwiespagina',
            'nl':  u'Wikipedia:Doorverwijspagina',
            'nn':  u'Fleirtydingssider',
            'no':  u'Pekere',
            'pl':  u'Strony ujednoznaczniające',
            'pt':  u'Desambiguação',
            'ro':  u'Dezambiguizare',
            'ru':  u'Многозначные термины',
            'scn': u'Disambigua',
            'sk':  u'Rozlišovacie stránky',
            'sl':  u'Razločitev',
            'sq':  u'Kthjellime',
            'sr':  u'Вишезначна одредница',
            'su':  u'Disambiguasi',
            'sv':  u'Förgreningssider',
            'szl': u'Zajty ujydnoznačńajůnce',
            'th':  u'การแก้ความกำกวม',
            'tl':  u'Paglilinaw',
            'tr':  u'Anlam ayrım',
            'uk':  u'Багатозначні геопункти',
            'vi':  u'Trang định hướng',
            'vo':  u'Telplänovapads',
            'wa':  u'Omonimeye',
            'zea': u'Wikipedia:Deurverwiespagina',
            'zh':  u'消歧义',
            'zh-min-nan': u'Khu-pia̍t-ia̍h',
            }

        # families that redirect their interlanguage links here.
        self.interwiki_forwarded_from = [
            'commons',
            'incubator',
            'meta',
            'species',
            'strategy',
            'test',
        ]

        # CentralAuth cross avaliable projects.
        self.cross_projects = [
            'wiktionary', 'wikibooks', 'wikiquote', 'wikisource', 'wikinews', 'wikiversity',
            'meta', 'mediawiki', 'test', 'incubator', 'commons', 'species',
        ]
        # Global bot allowed languages on
        # http://meta.wikimedia.org/wiki/Bot_policy/Implementation#Current_implementation
        self.cross_allowed = [
            'ab', 'ace', 'af', 'ak', 'als', 'am', 'an', 'ang', 'arc', 'arz',
            'as', 'ast', 'av', 'ay', 'az', 'ba', 'bat-smg', 'bar', 'bcl',
            'be-x-old', 'be', 'bg', 'bh', 'bi', 'bm', 'bo', 'bpy', 'bug', 'bxr',
            'cbk-zam', 'cdo', 'ce', 'ceb', 'ch', 'chr', 'chy', 'ckb', 'co',
            'crh', 'cr', 'csb', 'cu', 'cv', 'cy', 'diq', 'dsb', 'dz', 'ee',
            'el', 'eml', 'eo', 'et', 'eu', 'ext', 'fa', 'ff', 'fj', 'fo', 'frp',
            'frr', 'fur', 'ga', 'gan', 'gd', 'glk', 'gn', 'got', 'gu', 'gv',
            'ha', 'hak', 'haw', 'hif', 'hi', 'hr', 'hsb', 'ht', 'hu', 'hy',
            'ia', 'id', 'ie', 'ig', 'ik', 'ilo', 'iow', 'is', 'iu', 'ja', 'jbo',
            'jv', 'kaa', 'kab', 'ka', 'kg', 'ki', 'kk', 'kl', 'km', 'kn', 'ko',
            'koi', 'ks', 'ku', 'kv', 'kw', 'ky', 'lad', 'lb', 'lbe', 'lg', 'li',
            'lij', 'lmo', 'ln', 'lo', 'lv', 'map-bms', 'mdf', 'mg', 'mhr', 'mi',
            'mk', 'mn', 'mrj', 'ms', 'mt', 'mwl', 'myv', 'my', 'mzn', 'nah',
            'na', 'nap', 'nds-nl', 'ne', 'new', 'ng', 'nl', 'nov', 'nrm', 'nv',
            'ny', 'oc', 'om', 'or', 'os', 'pam', 'pap', 'pa', 'pag', 'pdc',
            'pfl', 'pi', 'pih', 'pms', 'pnb', 'pnt', 'ps', 'qu', 'rm', 'rmy',
            'rn', 'roa-rup', 'roa-tara', 'rw', 'sah', 'sa', 'sc', 'scn', 'sco',
            'sd', 'se', 'sg', 'sh', 'simple', 'si', 'sk', 'sm', 'sn', 'so',
            'srn', 'stq', 'st', 'su', 'sw', 'szl', 'ta', 'te', 'tet', 'tg',
            'th', 'ti', 'tk', 'tl', 'tn', 'to', 'tpi', 'ts', 'tt', 'tum', 'tw',
            'ty', 'udm', 'ug', 'uz', 've', 'vls', 'wa', 'war', 'wo', 'wuu',
            'xal', 'xh', 'yi', 'yo', 'za', 'zea', 'zh', 'zh-classical',
            'zh-min-nan', 'zu',
        ]

        # On most Wikipedias page names must start with a capital letter,
        # but some languages don't use this.
        self.nocapitalize = ['jbo',]

        self.alphabetic_latin = [
            'ace', 'af', 'ak', 'als', 'am', 'ang', 'ab', 'ar', 'an', 'arc',
            'roa-rup', 'frp', 'arz', 'as', 'ast', 'gn', 'av', 'ay', 'az', 'bjn',
            'id', 'ms', 'bg', 'bm', 'zh-min-nan', 'nan', 'map-bms', 'jv', 'su',
            'ba', 'be', 'be-x-old', 'bh', 'bcl', 'bi', 'bn', 'bo', 'bar', 'bs',
            'bpy', 'br', 'bug', 'bxr', 'ca', 'ceb', 'ch', 'cbk-zam', 'sn',
            'tum', 'ny', 'cho', 'chr', 'co', 'cy', 'cv', 'cs', 'da', 'dk',
            'pdc', 'de', 'nv', 'dsb', 'na', 'dv', 'dz', 'mh', 'et', 'el', 'eml',
            'en', 'myv', 'es', 'eo', 'ext', 'eu', 'ee', 'fa', 'hif', 'fo', 'fr',
            'fy', 'ff', 'fur', 'ga', 'gv', 'sm', 'gag', 'gd', 'gl', 'gan', 'ki',
            'glk', 'got', 'gu', 'ha', 'hak', 'xal', 'haw', 'he', 'hi', 'ho',
            'hsb', 'hr', 'hy', 'io', 'ig', 'ii', 'ilo', 'ia', 'ie', 'iu', 'ik',
            'os', 'xh', 'zu', 'is', 'it', 'ja', 'ka', 'kl', 'kr', 'pam', 'kbd',
            'krc', 'csb', 'kk', 'kw', 'rw', 'ky', 'mrj', 'rn', 'sw', 'km', 'kn',
            'ko', 'kv', 'kg', 'ht', 'ks', 'ku', 'kj', 'lad', 'lbe', 'la', 'ltg',
            'lv', 'to', 'lb', 'lt', 'lij', 'li', 'ln', 'lo', 'jbo', 'lg', 'lmo',
            'hu', 'mk', 'mg', 'mt', 'mi', 'cdo', 'mwl', 'ml', 'mdf', 'mo', 'mn',
            'mr', 'mus', 'my', 'mzn', 'nah', 'fj', 'ne', 'nl', 'nds-nl', 'cr',
            'new', 'nap', 'ce', 'frr', 'pih', 'no', 'nb', 'nn', 'nrm', 'nov',
            'oc', 'mhr', 'or', 'om', 'ng', 'hz', 'uz', 'pa', 'pfl', 'pag',
            'pap', 'koi', 'pi', 'pcd', 'pms', 'nds', 'pnb', 'pl', 'pt', 'pnt',
            'ps', 'aa', 'kaa', 'crh', 'ty', 'ksh', 'ro', 'rmy', 'rm', 'qu',
            'ru', 'rue', 'sa', 'sah', 'se', 'sg', 'sc', 'sco', 'sd', 'stq',
            'st', 'tn', 'sq', 'si', 'scn', 'simple', 'ss', 'sk', 'sl', 'cu',
            'szl', 'so', 'ckb', 'srn', 'sr', 'sh', 'fi', 'sv', 'ta', 'tl',
            'kab', 'roa-tara', 'tt', 'te', 'tet', 'th', 'ti', 'vi', 'tg',
            'tokipona', 'tp', 'tpi', 'chy', 've', 'tr', 'tk', 'tw', 'udm', 'uk',
            'ur', 'ug', 'za', 'vec', 'vo', 'fiu-vro', 'wa', 'vls', 'war', 'wo',
            'wuu', 'ts', 'yi', 'yo', 'diq', 'zea', 'zh', 'zh-tw', 'zh-cn',
            'zh-classical', 'zh-yue', 'bat-smg',
        ]

        # Which languages have a special order for putting interlanguage links,
        # and what order is it? If a language is not in interwiki_putfirst,
        # alphabetical order on language code is used. For languages that are in
        # interwiki_putfirst, interwiki_putfirst is checked first, and
        # languages are put in the order given there. All other languages are
        # put after those, in code-alphabetical order.

        self.interwiki_putfirst = {
            'be-x-old': self.alphabetic,
            'en': self.alphabetic,
            'et': self.alphabetic_revised,
            'fi': self.alphabetic_revised,
            'fiu-vro': self.alphabetic_revised,
            'fy': self.fyinterwiki,
            'he': ['en'],
            'hu': ['en'],
            'lb': self.alphabetic,
            'mk': self.alphabetic,
            'ms': self.alphabetic_revised,
            'nds': ['nds-nl', 'pdt'], # Note: as of 2008-02-24, pdt:
            'nds-nl': ['nds', 'pdt'], # (Plautdietsch) is still in the Incubator.
            'nn': ['no', 'nb', 'sv', 'da'] + self.alphabetic,
            'no': self.alphabetic,
            'pdc': ['de', 'en'],
            'pl': self.alphabetic,
            'simple': self.alphabetic,
            'sr': self.alphabetic_latin,
            'sv': self.alphabetic,
            'te': ['en', 'hi', 'kn', 'ta', 'ml'],
            'ur': ['ar', 'fa', 'en'] + self.alphabetic,
            'vi': self.alphabetic_revised,
            'yi': ['en', 'he', 'de']
        }

        self.obsolete = {
            'aa': None,  # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Afar_Wikipedia
            'cho': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Choctaw_Wikipedia
            'dk': 'da',
            'ho': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Hiri_Motu_Wikipedia
            'hz': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Herero_Wikipedia
            'ii': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Yi_Wikipedia
            'kj': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Kwanyama_Wikipedia
            'kr': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Kanuri_Wikipedia
            'mh': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Marshallese_Wikipedia
            'minnan': 'zh-min-nan',
            'mo': 'ro', # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Moldovan_Wikipedia
            'mus': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Muscogee_Wikipedia
            'nb': 'no',
            'ng': None, #(not reachable) http://meta.wikimedia.org/wiki/Inactive_wikis
            'jp': 'ja',
            'ru-sib': None, # http://meta.wikimedia.org/wiki/Proposals_for_closing_projects/Closure_of_Siberian_Wikipedia
            'tlh': None,
            'tokipona': None,
            'zh-tw': 'zh',
            'zh-cn': 'zh'
        }

        # Languages that used to be coded in iso-8859-1
        self.latin1old = ['de', 'en', 'et', 'es', 'ia', 'la', 'af', 'cs',
                    'fr', 'pt', 'sl', 'bs', 'fy', 'vi', 'lt', 'fi', 'it',
                    'no', 'simple', 'gl', 'eu', 'nds', 'co', 'mi', 'mr',
                    'id', 'lv', 'sw', 'tt', 'uk', 'vo', 'ga', 'na', 'es',
                    'nl', 'da', 'dk', 'sv', 'test']

        self.crossnamespace[0] = {
            '_default': {
                'pt': [102],
                'als': [104],
                'ar': [104],
                'de': [4],
                'en': [12],
                'es': [104],
                'fi': [4],
                'fr': [104],
                'hr': [102],
                'lt': [104],
            },
            'km': {
                '_default': [0, 4, 12],
            },
            #wrong wikipedia namespace alias
            'mzn': {
                '_default': [0, 4],

            },
        }
        self.crossnamespace[1] = {
            '_default': {
                'pt': [103],
                'als': [105],
                'ar': [105],
                'en': [13],
                'es': [105],
                'fi': [5],
                'fr': [105],
                'hr': [103],
                'lt': [105],
            },
        }
        self.crossnamespace[4] = {
            '_default': {
                '_default': [12],
            },
            'de': {
                '_default': [0, 10, 12],
                'el': [100, 12],
                'es': [104, 12],
            },
            'fi': {
                '_default': [0, 12]
            },
            'mzn': {
                '_default': [0, 12]
            },
        }
        self.crossnamespace[5] = {
            'fi': {
                '_default': [1]}
        }
        self.crossnamespace[12] = {
            '_default': {
                '_default': [4],
            },
            'en': {
                '_default': [0, 4],
            },
        }
        self.crossnamespace[13] = {
            'en': {
                '_default': [0],
            },
        }
        self.crossnamespace[102] = {
            'pt': {
                '_default': [0],
                'als': [0, 104],
                'ar': [0, 104],
                'es': [0, 104],
                'fr': [0, 104],
                'lt': [0, 104]
            },
            'hr': {
                '_default': [0],
                'als': [0, 104],
                'ar': [0, 104],
                'es': [0, 104],
                'fr': [0, 104],
                'lt': [0, 104]
            },
        }
        self.crossnamespace[103] = {
            'pt': {
                '_default': [1],
                'als': [1, 105],
                'es': [1, 105],
                'fr': [1, 105],
                'lt': [1, 105]
            },
            'hr': {
                '_default': [1],
                'als': [1, 105],
                'es': [1, 105],
                'fr': [1, 105],
                'lt': [1, 105]
            },
        }
        self.crossnamespace[104] = {
            'als': {
                '_default': [0],
                'pt': [0, 102],
                'hr': [0, 102],
            },
            'ar': {
                '_default': [0, 100],
                'hr': [0, 102],
                'pt': [0, 102],
            },
            'es': {
                '_default': [0],
                'pt': [0, 102],
                'hr': [0, 102],
            },
            'fr': {
                '_default': [0],
                'pt': [0, 102],
                'hr': [0, 102],
            },
            'lt': {
                '_default': [0],
                'pt': [0, 102],
                'hr': [0, 102],
            },
        }
        self.crossnamespace[105] = {
            'als': {
                '_default': [1],
                'pt': [0, 103],
                'hr': [0, 103],
            },
            'ar': {
                '_default': [1, 101],
            },
            'es': {
                '_default': [1],
                'pt': [0, 103],
                'hr': [0, 103],
            },
            'fr': {
                '_default': [1],
                'pt': [0, 103],
                'hr': [0, 103],
            },
            'lt': {
                '_default': [1],
                'pt': [0, 103],
                'hr': [0, 103],
            },
        }

    def get_known_families(self, site):
        # In Swedish Wikipedia 's:' is part of page title not a family
        # prefix for 'wikisource'.
        if site.language() == 'sv':
            d = self.known_families.copy()
            d.pop('s') ; d['src'] = 'wikisource'
            return d
        else:
            return self.known_families

    def version(self, code):
        return '1.17wmf1'

    def dbName(self, code):
        # returns the name of the MySQL database
        # for historic reasons, the databases are called xxwiki instead of
        # xxwikipedia for Wikipedias.
        return '%swiki_p' % code

    def code2encodings(self, code):
        """Return a list of historical encodings for a specific language
           wikipedia"""
        # Historic compatibility
        if code == 'pl':
            return 'utf-8', 'iso8859-2'
        if code == 'ru':
            return 'utf-8', 'iso8859-5'
        if code in self.latin1old:
            return 'utf-8', 'iso-8859-1'
        return self.code2encoding(code),

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
