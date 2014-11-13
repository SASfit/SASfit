#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
This script understands various command-line arguments:

-interactive:     ask before changing each page

-nocache          doesn't include /cache/featured /cache/lists or /cache/good
                  file to remember if the article already was verified.

-fromlang:xx,yy   xx,yy,zz,.. are the languages to be verified.
-fromlang:ar--fi  Another possible with range the languages

-fromall          to verify all languages.

-after:zzzz       process pages after and including page zzzz

-side             use -side if you want to move all {{Link FA|lang}} next to the
                  corresponding interwiki links. Default is placing
                  {{Link FA|lang}} on top of the interwiki links.

-count            Only counts how many featured/good articles exist
                  on all wikis (given with the "-fromlang" argument) or
                  on several language(s) (when using the "-fromall" argument).
                  Example: featured.py -fromlang:en,he -count
                  counts how many featured articles exist in the en and he
                  wikipedias.

-lists            use this script for featured lists.

-good             use this script for good articles.

-former           use this script for removing {{Link FA|xx}} from former
                  fearured articles

-quiet            no corresponding pages are displayed.

-dry              for debug purposes. No changes will be made.

-query:#          a int. that determain number of pages will be checked each while
                  (use for computers with a small amount of RAM e.g. toolserver users)
                  default is 500

usage: featured.py [-interactive] [-nocache] [-top] [-after:zzzz] [-fromlang:xx,yy--zz|-fromall]

"""
__version__ = '$Id: featured.py 9250 2011-05-16 14:32:31Z xqt $'

#
# (C) Maxim Razin, 2005
# (C) Leonardo Gregianin, 2005-2008
# (C) xqt, 2009-2010
# (C) Pywikipedia bot team, 2005-2010
#
# Distributed under the terms of the MIT license.
#

import sys, re, pickle, os.path
import wikipedia as pywikibot
import catlib, config

def CAT(site,name):
    name = site.namespace(14) + ':' + name
    cat=catlib.Category(site, name)
    return cat.articles()

def BACK(site,name):
    name = site.namespace(10) + ':' + name
    p=pywikibot.Page(site, name)
    return [page for page in p.getReferences(follow_redirects=False,
                                             onlyTemplateInclusion=True)]

msg = {
    'als':u'Bötli: [[%s:%s]] isch en bsunders glungener Artikel',
    'ar': u'بوت: [[%s:%s]] هي مقالة مختارة',
    'bat-smg': u'robots: Pavīzdėnė straipsnė nūruoda [[%s:%s]]',
    'be-x-old': u'Робат: [[%s:%s]] — абраны артыкул',
    'bs': u'Bot: Interwiki za izabrane članke za [[%s:%s]]',
    'ca': u'Bot: Enllaç a article destacat per: [[%s:%s]]',
    'cs': u'Robot přidal nejlepší článek: [[%s:%s]]',
    'cy': u'Robot: Mae [[%s:%s]] yn erthygl ddethol',
    'de': u'Bot: [[%s:%s]] ist ein ausgezeichneter Artikel',
    'dsb':u'Bot: [[%s:%s]] jo wuběrny nastawk',
    'en': u'Bot: [[%s:%s]] is a featured article',
    'eo': u'roboto: [[%s:%s]] estas artikolo elstara',
    'es': u'Bot: Enlace a artículo destacado para: [[%s:%s]]',
    'fa': u'ربات: [[%s:%s]] یک مقاله برگزیده است',
    'fi': u'Botti: [[%s:%s]] on suositeltu artikkeli',
    'fr': u'Bot: Lien AdQ pour [[%s:%s]]',
    'frr':u'Bot: [[%s:%s]] as en auer a miaten guden artiikel',
    'he': u'בוט: קישור לערך מומלץ עבור [[%s:%s]]',
    'hr': u'Bot: Interwiki za izabrane članke za [[%s:%s]]',
    'hsb':u'Bot: [[%s:%s]] je wuběrny nastawk',
    'hu': u'Bot: a(z) [[%s:%s]] kiemelt szócikk',
    'ia': u'Robot: Ligamine verso articulo eminente [[%s:%s]]',
    'it': u'Bot: collegamento articolo in vetrina [[%s:%s]]',
    'ja': u'ロボットによる: 秀逸な記事へのリンク [[%s:%s]]',
    'ka': u'ბოტი: რჩეული სტატიის ბმული გვერდისათვის [[%s:%s]]',
    'ko': u'로봇: 알찬 글 [[%s:%s]]',
    'ksh':u'bot: [[%s:%s]] ess_enen ußjezëijshneten Atikkel',
    'lb': u'Bot: Exzellenten Arikel Link op [[%s:%s]]',
    'lt': u'Bot: Pavyzdinis straipsnis [[%s:%s]]',
    'nl': u'Bot: verwijzing naar etalage-artikel voor [[%s:%s]]',
    'no': u'bot: [[%s:%s]] er en utmerka artikkel',
    'nn': u'bot: [[%s:%s]] er ein god artikkel',
    'nv': u'Naaltsoos [[%s:%s]] kʼad nizhónígo ályaa',
    'mk': u'Бот: Интервики за избрани статии за [[%s:%s]]',
    'pl': u'Bot: Link do artykułu wyróżnionego [[%s:%s]]',
    'pt': u'Bot: Ligando artigos destacados para [[%s:%s]]',
    'ru': u'Робот: избранная статья [[%s:%s]]',
    'sk': u'Bot: [[%s:%s]] je najlepší článok',
    'sr': u'Bot: Међувики за изабране чланке за [[%s:%s]]',
    'sv': u'Bot: [[%s:%s]] är en utmärkt artikel',
    'th': u'บอต: ลิงก์บทความคัดสรร [[%s:%s]]',
    'tr': u'Bot değişikliği: [[%s:%s]] madde bağlantısı eklendi',
    'vo': u'Bot: Yüm yegeda gudik tefü [[%s:%s]]',
    'zh': u'機器人: 連結特色條目 [[%s:%s]]',
}

msg_good = {
    'als': u'Bötli: [[%s:%s]] isch en glungener Artikel',
    'ar': u'بوت: [[%s:%s]] هي مقالة جيدة',
    'cy': u'Robot: Mae [[%s:%s]] yn erthygl dda',
    'de': u'Bot: [[%s:%s]] ist ein lesenswerter Artikel',
    'en': u'Bot: [[%s:%s]] is a good article',
    'eo': u'roboto: [[%s:%s]] estas artikolo leginda',
    'es': u'Bot: Enlace a artículo bueno para: [[%s:%s]]',
    'fa': u'ربات: [[%s:%s]] یک مقاله خوب است',
    'fi': u'Botti: [[%s:%s]] on hyvä artikkeli',
    'fr': u'Bot: Lien BA pour [[%s:%s]]',
    'frr':u'Bot: [[%s:%s]] as en guden artiikel',
    'it': u'Bot: collegamento articolo di qualità [[%s:%s]]',
    'ja': u'ロボットによる: 良質な記事へのリンク [[%s:%s]]',
    'ko': u'로봇: 좋은 글 [[%s:%s]]',
    'ksh':u'bot: [[%s:%s]] ess_enen jooden Atikkel',
    'no': u'bot: [[%s:%s]] er en anbefalt artikkel',
    'nn': u'bot: [[%s:%s]] er ein god artikkel',
    'pl': u'Bot: Link do dobrego artykułu: [[%s:%s]]',
    'pt': u'Bot: [[%s:%s]] é um artigo bom',
    'ru': u'Робот: хорошая статья [[%s:%s]]',
    'sv': u'Bot: [[%s:%s]] är en läsvärd artikel',
}

msg_lists = {
    'als':u'Bötli: [[%s:%s]] isch e bsunders glungene Lischte',
    'ar': u'بوت: [[%s:%s]] هي قائمة مختارة',
    'de': u'Bot: [[%s:%s]] ist eine informative Liste',
    'en': u'Bot: [[%s:%s]] is a featured list',
    'es': u'Bot: Enlace a lista destacada para: [[%s:%s]]',
    'fa': u'ربات: [[%s:%s]] یک فهرست برگزیده است',
    'fi': u'Botti: [[%s:%s]] on suositeltu luettelo',
    'frr':u'Bot: [[%s:%s]] as en wäärdag list',
    'ja': u'ロボットによる: 秀逸な一覧へのリンク [[%s:%s]]',
    'ksh':u'bot: [[%s:%s]] ess_en joode Leß',
    'pl': u'Bot: [[%s:%s]] jest listą medalową',
    'pt': u'Bot: [[%s:%s]] é uma lista destacada',
    'sv': u'Bot: [[%s:%s]] är en utmärkt list',
}

msg_former = {
    'ar': u'بوت: [[%s:%s]] مقالة مختارة سابقة',
    'de': u'Bot: [[%s:%s]] ist ein ehemaliger ausgezeichneter Artikel',
    'en': u'Bot: [[%s:%s]] is a former featured article',
    'es': u'Bot: [[%s:%s]] ya no es un artículo destacado',
    'fa': u'ربات:نوشتار [[%s:%s]] یک نوشتار برگزیده پیشین است.',
    'fi': u'Botti: [[%s:%s]] on entinen suositeltu artikkeli',
    'frr':u'Bot: [[%s:%s]] wiar ans en auer a miaten guden artiikel',
    'pl': u'Bot: [[%s:%s]] jest byłym artykułem na medal',
}

# ALL wikis use 'Link FA', and sometimes other localized templates.
# We use _default AND the localized ones
template = {
    '_default': ['Link FA'],
    'als': ['LinkFA'],
    'an': ['Destacato', 'Destacau'],
    'ar': [u'وصلة مقالة مختارة'],
    'ast':['Enllaz AD'],
    'az': ['Link FM'],
    'br': ['Liamm PuB', 'Lien AdQ'],
    'ca': [u'Enllaç AD', 'Destacat'],
    'cy': ['Cyswllt erthygl ddethol', 'Dolen ED'],
    'eo': ['LigoElstara'],
    'en': ['Link FA', 'FA link'],
    'es': ['Destacado'],
    'eu': ['NA lotura'],
    'fr': ['Lien AdQ'],
    'fur':['Leam VdC'],
    'ga': ['Nasc AR'],
    'hi': ['Link FA', 'Lien AdQ'],
    'is': [u'Tengill ÚG'],
    'it': ['Link AdQ'],
    'no': ['Link UA'],
    'oc': ['Ligam AdQ', 'Lien AdQ'],
    'ro': [u'Legătură AC', u'Legătură AF'],
    'sv': ['UA', 'Link UA'],
    'tr': ['Link SM'],
    'vi': [u'Liên kết chọn lọc'],
    'vo': [u'Yüm YG'],
    'yi': [u'רא'],
}

template_good = {
    '_default': ['Link GA'],
    'ar': [u'وصلة مقالة جيدة'],
    'da': ['Link GA', 'Link AA'],
    'eo': ['LigoLeginda'],
    'es': ['Bueno'],
    'fr': ['Lien BA'],
    'is': ['Tengill GG'],
    'it': ['Link VdQ'],
    'nn': ['Link AA'],
    'no': ['Link AA'],
    'pt': ['Bom interwiki'],
   #'tr': ['Link GA', 'Link KM'],
    'vi': [u'Liên kết bài chất lượng tốt'],
    'wo': ['Lien BA'],
}

template_lists = {
    '_default': ['Link FL'],
}

featured_name = {
    'af': (BACK,u"Voorbladster"),
    'als':(CAT, u"Wikipedia:Bsunders glungener Artikel"),
    'am': (CAT, u"Wikipedia:Featured article"),
    'an': (CAT, u"Articlos destacatos"),
    'ar': (CAT, u"مقالات مختارة"),
    'ast':(CAT, u"Uiquipedia:Artículos destacaos"),
    'az': (BACK,u"Seçilmiş məqalə"),
    'bar':(CAT, u"Berig"),
    'bat-smg': (CAT, u"Vikipedėjės pavīzdėnē straipsnē"),
    'be-x-old':(CAT, u"Вікіпэдыя:Абраныя артыкулы"),
    'bg': (CAT, u"Избрани статии"),
    'bn': (BACK,u"নির্বাচিত নিবন্ধ"),
    'br': (CAT, u"Pennadoù eus an dibab"),
    'bs': (CAT, u"Odabrani članci"),
    'ca': (CAT, u"Llista d'articles de qualitat"),
    'ceb':(CAT, u"Mga napiling artikulo"),
    'cs': (CAT, u"Nejlepší články"),
    'cy': (CAT, u"Erthyglau dethol"),
    'da': (CAT, u"Fremragende artikler"),
    'de': (CAT, u"Wikipedia:Exzellent"),
   #'dsb':(CAT, u"Ekscelentny"),
    'dv': (BACK, u"Featured article"),
   #'dv': (CAT, u"Featured Articles"),
    'el': (BACK,u"Αξιόλογο άρθρο"),
    'eo': (CAT, u"Elstaraj artikoloj"),
    'en': (CAT, u"Featured articles"),
    'es': (BACK, u"Artículo destacado"),
    'et': (CAT, u"Eeskujulikud artiklid"),
    'eu': (CAT, u"Nabarmendutako artikuluak"),
    'ext':(BACK,u"Destacau"),
    'fa': (BACK,u"نوشتارهای برگزیده"),
    'fi': (CAT, u"Suositellut sivut"),
    'fo': (CAT, u"Mánaðargrein"),
    'fr': (CAT, u"Article de qualité"),
    'frr':(BACK,u"Exzellent"),
    'gv': (CAT, u"Artyn reiht"),
    'he': (CAT, u"ערכים מומלצים"),
    'hi': (BACK,u"निर्वाचित लेख"),
    'hr': (CAT, u"Izabrani članci"),
    'hsb':(CAT, u"Ekscelentny"),
    'hu': (CAT, u"Kiemelt cikkek"),
    'hy': (BACK,u"Ընտրված հոդված"),
    'ia': (CAT, u"Wikipedia:Articulos eminente"),
    'id': (BACK, u"Featured article"),
   #'id': (CAT, u"Artikel bagus utama"),
    'is': (CAT, u"Wikipedia:Úrvalsgreinar"),
    'it': (CAT, u"Voci in vetrina"),
    'ja': (BACK,u"Featured article"),
    'ka': (CAT, u"რჩეული სტატიები"),
    'km': (BACK,u"អត្ថបទពិសេស"),
    'kn': (BACK,u"ವಿಶೇಷ ಲೇಖನ"),
    'ko': (CAT, u"알찬 글"),
    'ksh':(CAT, u"Exzälenter Aatikkel"),
    'kv': (CAT, u"Википедия:Бур гижӧдъяс"),
    'la': (CAT, u"Paginae mensis"),
    'li': (CAT, u"Wikipedia:Sjterartikele"),
    'lmo':(CAT, u"Articol ben faa"),
    'lo': (CAT, u"ບົດຄວາມດີເດັ່ນ"),
    'lt': (CAT, u"Vikipedijos pavyzdiniai straipsniai"),
    'lv': (CAT, u"Vērtīgi raksti"),
   #'lv': (CAT, u"Nedēļas raksti"),
    'mk': (BACK, u"Избрана"),
    'ml': (BACK,u"Featured"),
    'mr': (CAT, u"मुखपृष्ठ सदर लेख"),
    'ms': (BACK,u"Rencana pilihan"),
    'nah':(BACK,u"Featured article"),
    'nds-nl': (BACK, u"Etelazie"),
    'nl': (CAT, u"Wikipedia:Etalage-artikelen"),
    'nn': (BACK,u"God artikkel"),
    'no': (CAT, u"Utmerkede artikler"),
    'nv': (CAT, u"Naaltsoos nizhónígo ályaaígíí"),
    'oc': (CAT, u"Article de qualitat"),
    'pl': (CAT, u"Artykuły na medal"),
    'pt': (CAT, u"!Artigos destacados"),
    'ro': (CAT, u"Articole de calitate"),
    'ru': (BACK, u"Избранная статья"),
    'sco':(CAT, u"Featurt"),
    'sh': (CAT, u"Izabrani članci"),
    'simple': (CAT, u"Very good articles"),
    'sk': (BACK,u"Perfektný článok"),
    'sl': (CAT, u"Vsi izbrani članki"),
    'sq': (BACK,u"Artikulli perfekt"),
    'sr': (CAT, u"Изабрани"),
    'sv': (CAT, u"Wikipedia:Utmärkta artiklar"),
    'sw': (BACK,u"Makala_nzuri_sana"),
    'szl':(CAT, u"Wyrůżńůne artikle"),
    'ta': (CAT, u"சிறப்புக் கட்டுரைகள்"),
    'te': (CAT, u"విశేషవ్యాసాలు"),
    'th': (BACK,u"บทความคัดสรร"),
    'tl': (BACK,u"Napiling artikulo"),
    'tr': (BACK,u"Seçkin madde"),
    'tt': (CAT, u"Сайланган мәкаләләр"),
    'uk': (CAT, u"Вибрані статті"),
    'ur': (CAT, u"منتخب مقالے"),
    'uz': (CAT, u"Vikipediya:Tanlangan maqolalar"),
    'vec':(BACK,u"Vetrina"),
    'vi': (CAT, u"Bài viết chọn lọc"),
    'vo': (CAT, u"Yegeds gudik"),
    'wa': (CAT, u"Raspepyî årtike"),
    'yi': (CAT, u"רעקאמענדירטע ארטיקלען"),
    'yo': (BACK,u"Àyọkà pàtàkì"),
    'zh': (CAT, u"特色条目"),
    'zh-classical': (CAT, u"卓著"),
    'zh-yue': (BACK, u"正文"),
}

good_name = {
    'ar': (CAT, u"مقالات جيدة"),
    'ca': (CAT, u"Llista d'articles bons"),
    'cs': (CAT, u"Wikipedie:Dobré články"),
    'da': (CAT, u"Gode artikler"),
    'de': (CAT, u"Wikipedia:Lesenswert"),
   #'dsb':(CAT, u"Naraźenje za pógódnośenje"),
    'en': (CAT, u"Wikipedia good articles"),
    'eo': (CAT, u"Legindaj artikoloj"),
    'es': (CAT, u"Wikipedia:Artículos buenos"),
    'et': (CAT, u"Head artiklid"),
    'fa': (CAT, u"نوشتارهای خوب"),
    'fi': (CAT, u"Hyvät artikkelit"),
    'fr': (CAT, u"Bon article"),
    'hsb':(CAT, u"Namjet za pohódnoćenje"),
    'id': (BACK,u"Artikel bagus"),
   #'id': (CAT, u"Artikel bagus"),
    'is': (CAT, u"Wikipedia:Gæðagreinar"),
    'ja': (BACK,u"Good article"),
    'ko': (CAT, u"좋은 글"),
    'ksh':(CAT, u"Joode Aatikkel"),
    'lt': (CAT, u"Vertingi straipsniai"),
    'lv': (CAT, u"Labi raksti"),
    'no': (CAT, u"Anbefalte artikler"),
    'oc': (CAT, u"Bon article"),
    'pl': (CAT, u"Dobre artykuły"),
    'pt': (CAT, u"Artigos bons"),
    'ro': (BACK, u"Articol bun"),
    'ru': (CAT, u"Википедия:Хорошие статьи"),
    'simple': (CAT, u"Good articles"),
    'sr': (BACK,u"Иконица добар"),
    'sv': (CAT, u"Wikipedia:Bra artiklar"),
    'tr': (BACK,u"Kaliteli madde"),
    'uk': (CAT, u"Вікіпедія:Добрі статті"),
    'uz': (CAT, u"Vikipediya:Yaxshi maqolalar"),
    'yi': (CAT, u"וויקיפעדיע גוטע ארטיקלען"),
    'zh': (CAT, u"優良條目"),
    'zh-classical': (CAT, u"正典"),
}

lists_name = {
    'ar': (BACK, u'قائمة مختارة'),
    'da': (BACK, u'FremragendeListe'),
    'de': (BACK, u'Informativ'),
    'en': (BACK, u'Featured list'),
    'fa': (BACK, u"فهرست برگزیده"),
    'id': (BACK, u'Featured list'),
    'ja': (BACK, u'Featured List'),
    'ksh':(CAT,  u"Joode Leß"),
    'no': (BACK, u'God liste'),
    'pl': (BACK, u'Medalista'),
    'pt': (BACK, u'Anexo destacado'),
    'ro': (BACK, u'Listă de calitate'),
    'ru': (BACK, u'Избранный список или портал'),
    'tr': (BACK, u'Seçkin liste'),
    'uk': (BACK, u'Вибраний список'),
    'vi': (BACK, u'Sao danh sách chọn lọc'),
    'zh': (BACK, u'Featured list'),
    'da': (BACK, u'FremragendeListe'),
}

former_name = {
    'th': (CAT, u"บทความคัดสรรในอดีต"),
    'pt': (CAT, u"!Ex-Artigos_destacados"),
    'fa': (CAT, u"مقاله‌های برگزیده پیشین"),
    'es': (CAT, u"Wikipedia:Artículos anteriormente destacados"),
    'hu': (CAT, u"Korábbi kiemelt cikkek"),
    'ru': (CAT, u"Википедия:Устаревшие избранные статьи"),
    'ca': (CAT, u"Arxiu de propostes de la retirada de la distinció"),
    'es': (CAT, u"Wikipedia:Artículos anteriormente destacados"),
    'tr': (CAT, u"Vikipedi eski seçkin maddeler"),
    'zh': (CAT, u"Wikipedia_former_featured_articles"),
}

# globals
interactive=0
nocache=0
afterpage=u"!"
cache={}

def featuredArticles(site, pType):
    arts=[]
    try:
        if pType == 'good':
            method=good_name[site.lang][0]
        elif pType == 'former':
            method=former_name[site.lang][0]
        elif pType == 'list':
            method=lists_name[site.lang][0]
        else:
            method=featured_name[site.lang][0]
    except KeyError:
        pywikibot.output(
            u'Error: language %s doesn\'t has %s category source.'
            % (site.lang, pType))
        return arts
    if pType == 'good':
        name=good_name[site.lang][1]
    elif pType == 'former':
        name=former_name[site.lang][1]
    elif pType == 'list':
        name=lists_name[site.lang][1]
    else:
        name=featured_name[site.lang][1]
    raw=method(site, name)
    for p in raw:
        if p.namespace()==0: # Article
            arts.append(p)
        # Article talk (like in English)
        elif p.namespace()==1 and site.lang <> 'el':
            arts.append(pywikibot.Page(p.site(), p.titleWithoutNamespace()))
    pywikibot.output(
        '\03{lightred}** wikipedia:%s has %i %s articles\03{default}'
        % (site.lang, len(arts), pType))
    return arts

def findTranslated(page, oursite=None, quiet=False):
    if not oursite:
        oursite=pywikibot.getSite()
    if page.isRedirectPage():
        page = page.getRedirectTarget()
    try:
        iw=page.interwiki()
    except:
        pywikibot.output(u"%s -> no interwiki, giving up" % page.title())
        return None
    ourpage=None
    for p in iw:
        if p.site()==oursite:
            ourpage=p
            break
    if not ourpage:
        if not quiet:
            pywikibot.output(u"%s -> no corresponding page in %s"
                             % (page.title(), oursite))
        return None
    if not ourpage.exists():
        pywikibot.output(u"%s -> our page doesn't exist: %s"
                         % (page.title(), ourpage.title()))
        return None
    if ourpage.isRedirectPage():
        ourpage = ourpage.getRedirectTarget()
    pywikibot.output(u"%s -> corresponding page is %s"
                     % (page.title(), ourpage.title()))
    if ourpage.namespace() != 0:
        pywikibot.output(u"%s -> not in the main namespace, skipping"
                         % page.title())
        return None
    if ourpage.isRedirectPage():
        pywikibot.output(u"%s -> double redirect, skipping" % page.title())
        return None
    if not ourpage.exists():
        pywikibot.output(u"%s -> page doesn't exist, skipping" % ourpage.title())
        return None
    try:
        iw=ourpage.interwiki()
    except:
        return None
    backpage=None
    for p in iw:
        if p.site()==page.site():
            backpage=p
            break
    if not backpage:
        pywikibot.output(u"%s -> no back interwiki ref" % page.title())
        return None
    if backpage==page:
        # everything is ok
        return ourpage
    if backpage.isRedirectPage():
        backpage = backpage.getRedirectTarget()
    if backpage==page:
        # everything is ok
        return ourpage
    pywikibot.output(u"%s -> back interwiki ref target is %s"
                     % (page.title(), backpage.title()))
    return None

def getTemplateList (lang, pType):
    if pType == 'good':
        try:
            templates = template_good[lang]
            templates+= template_good['_default']
        except KeyError:
            templates = template_good['_default']
    elif pType == 'list':
        try:
            templates = template_lists[lang]
            templatest+= template_lists['_default']
        except KeyError:
            templates = template_lists['_default']
    else: #pType in ['former', 'featured']
        try:
            templates = template[lang]
            templates+= template['_default']
        except KeyError:
            templates = template['_default']
    return templates

def featuredbot(arts, cc, tosite, template_on_top, pType, quiet, dry):
    templatelist = getTemplateList(tosite.lang, pType)
    findtemplate = '(' + '|'.join(templatelist) + ')'
    re_Link_FA=re.compile(ur"\{\{%s\|%s\}\}"
                          % (findtemplate.replace(u' ', u'[ _]'),
                             fromsite.lang), re.IGNORECASE)
    re_this_iw=re.compile(ur"\[\[%s:[^]]+\]\]" % fromsite.lang)
    pairs=[]
    for a in arts:
                if a.title()<afterpage:
                    continue
                if u"/" in a.title() and a.namespace() != 0:
                    pywikibot.output(u"%s is a subpage" % a.title())
                    continue
                if a.title() in cc:
                    pywikibot.output(u"(cached) %s -> %s"%(a.title(), cc[a.title()]))
                    continue
                if a.isRedirectPage():
                    a=a.getRedirectTarget()
                try:
                    if not a.exists():
                        pywikibot.output(u"source page doesn't exist: %s" % a.title())
                        continue
                    atrans = findTranslated(a, tosite, quiet)
                    if pType!='former':
                        if atrans:
                            text=atrans.get()
                            m=re_Link_FA.search(text)
                            if m:
                                pywikibot.output(u"(already done)")
                            else:
                                # insert just before interwiki
                                if (not interactive or
                                    pywikibot.input(
                                        u'Connecting %s -> %s. Proceed? [Y/N]'
                                        % (a.title(), atrans.title())) in ['Y','y']
                                    ):
                                    m=re_this_iw.search(text)
                                    if not m:
                                        pywikibot.output(
                                            u"no interwiki record, very strange")
                                        continue
                                    site = pywikibot.getSite()
                                    if pType == 'good':
                                        comment = pywikibot.setAction(
                                            pywikibot.translate(site, msg_good)
                                            % (fromsite.lang, a.title()))
                                    elif pType == 'list':
                                        comment = pywikibot.setAction(
                                            pywikibot.translate(site, msg_lists)
                                            % (fromsite.lang, a.title()))
                                    else:
                                        comment = pywikibot.setAction(
                                            pywikibot.translate(site, msg)
                                            % (fromsite.lang, a.title()))
                                    ### Moving {{Link FA|xx}} to top of interwikis ###
                                    if template_on_top == True:
                                        # Getting the interwiki
                                        iw = pywikibot.getLanguageLinks(text, site)
                                        # Removing the interwiki
                                        text = pywikibot.removeLanguageLinks(text, site)
                                        text += u"\r\n{{%s|%s}}\r\n" % (templatelist[0],
                                                                        fromsite.lang)
                                        # Adding the interwiki
                                        text = pywikibot.replaceLanguageLinks(text,
                                                                              iw, site)

                                    ### Placing {{Link FA|xx}} right next to corresponding interwiki ###
                                    else:
                                        text=(text[:m.end()]
                                              + (u" {{%s|%s}}" % (templatelist[0],
                                                                  fromsite.lang))
                                              + text[m.end():])
                                    if not dry:
                                        try:
                                            atrans.put(text, comment)
                                        except pywikibot.LockedPage:
                                            pywikibot.output(u'Page %s is locked!'
                                                             % atrans.title())
                            cc[a.title()]=atrans.title()
                    else:
                        if atrans:
                            text=atrans.get()
                            m=re_Link_FA.search(text)
                            if m:
                                # insert just before interwiki
                                if (not interactive or
                                    pywikibot.input(
                                        u'Connecting %s -> %s. Proceed? [Y/N]'
                                        % (a.title(), atrans.title())) in ['Y','y']
                                    ):
                                    m=re_this_iw.search(text)
                                    if not m:
                                        pywikibot.output(
                                            u"no interwiki record, very strange")
                                        continue
                                    site = pywikibot.getSite()
                                    comment = pywikibot.setAction(
                                        pywikibot.translate(site, msg_former)
                                        % (fromsite.lang, a.title()))
                                    text=re.sub(re_Link_FA,'',text)
                                    if not dry:
                                        try:
                                            atrans.put(text, comment)
                                        except pywikibot.LockedPage:
                                            pywikibot.output(u'Page %s is locked!'
                                                             % atrans.title())
                            else:
                                pywikibot.output(u"(already done)")
                            cc[a.title()]=atrans.title()
                except pywikibot.PageNotSaved, e:
                    pywikibot.output(u"Page not saved")

def featuredWithInterwiki(fromsite, tosite, template_on_top, pType, quiet,
                          dry=False, query=500):
    if not fromsite.lang in cache:
        cache[fromsite.lang] = {}
    if not tosite.lang in cache[fromsite.lang]:
        cache[fromsite.lang][tosite.lang] = {}
    cc = cache[fromsite.lang][tosite.lang]
    if nocache:
        cc={}

    arts=featuredArticles(fromsite, pType)
    top=0
    if len(arts) > query:
        while top < len(arts):
            bottom = top
            top += query
            featuredbot(arts[bottom:top], cc, tosite, template_on_top, pType,
                        quiet, dry)
    else:
        featuredbot(arts, cc, tosite, template_on_top, pType, quiet, dry)

if __name__=="__main__":
    template_on_top = True
    featuredcount = False
    fromlang=[]
    processType = 'featured'
    doAll = False
    part  = False
    quiet = False
    dry = False
    query=500
    for arg in pywikibot.handleArgs():
        if arg == '-interactive':
            interactive=1
        elif arg == '-nocache':
            nocache=1
        elif arg.startswith('-fromlang:'):
            fromlang=arg[10:].split(",")
            part = True
        elif arg.startswith('-query:'):
            try:
                query=int(arg[7:])
            except:
                query=500
        elif arg == '-fromall':
            doAll = True
        elif arg.startswith('-after:'):
            afterpage=arg[7:]
        elif arg == '-side':
            template_on_top = False
        elif arg == '-count':
            featuredcount = True
        elif arg == '-good':
            processType = 'good'
        elif arg == '-lists':
            processType = 'list'
        elif arg == '-former':
            processType = 'former'
        elif arg == '-quiet':
            quiet = True
        elif arg == '-dry':
            dry = True

    if part:
        try:
            # BUG: range with zh-min-nan (3 "-")
            if len(fromlang)==1 and fromlang[0].index("-")>=0:
                ll1,ll2=fromlang[0].split("--",1)
                if not ll1: ll1=""
                if not ll2: ll2="zzzzzzz"
                if processType == 'good':
                    fromlang=[ll for ll in good_name.keys()
                              if ll>=ll1 and ll<=ll2]
                elif processType == 'list':
                    fromlang=[ll for ll in good_lists.keys()
                              if ll>=ll1 and ll<=ll2]
                elif processType == 'former':
                    fromlang=[ll for ll in former_lists.keys()
                              if ll>=ll1 and ll<=ll2]
                else:
                    fromlang=[ll for ll in featured_name.keys()
                              if ll>=ll1 and ll<=ll2]
        except:
            pass

    if doAll:
        if processType == 'good':
            fromlang=good_name.keys()
        elif processType == 'list':
            fromlang=lists_name.keys()
        elif processType == 'former':
            fromlang=former_name.keys()
        else:
            fromlang=featured_name.keys()

    filename="cache/" + processType
    try:
        cache=pickle.load(file(filename,"rb"))
    except:
        cache={}


    if not fromlang:
        pywikibot.showHelp('featured')
        sys.exit(1)

    fromlang.sort()

    #test whether this site has template enabled
    hasTemplate = False
    if not featuredcount:
        for tl in getTemplateList(pywikibot.getSite().lang, processType):
            t = pywikibot.Page(pywikibot.getSite(), u'Template:'+tl)
            if t.exists():
                hasTemplate = True
                break
    try:
        for ll in fromlang:
            fromsite = pywikibot.getSite(ll)
            if featuredcount:
                featuredArticles(fromsite, processType)
            elif not hasTemplate:
                pywikibot.output(
                    u'\nNOTE: %s arcticles are not implemented at %s-wiki.'
                    % (processType, pywikibot.getSite().lang))
                pywikibot.output('Quitting program...')
                break
            elif  fromsite != pywikibot.getSite():
                featuredWithInterwiki(fromsite, pywikibot.getSite(),
                                      template_on_top, processType, quiet, dry,query)
    except KeyboardInterrupt:
        pywikibot.output('\nQuitting program...')
    finally:
        pywikibot.stopme()
        if not nocache:
            pickle.dump(cache,file(filename,"wb"))

