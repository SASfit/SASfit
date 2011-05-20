#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This script has the intention to correct all redirect
links in featured pages or only one page of each wiki.

Can be using with:
&params;

-featured         Run over featured pages

Run fixing_redirects.py -help to see all the command-line
options -file, -ref, -links, ...

"""
#
# This script based on disambredir.py and solve_disambiguation.py
#
# (C) Pywikipedia team, 2004-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: fixing_redirects.py 9042 2011-03-13 10:14:47Z xqt $'
#
import wikipedia as pywikibot
import pagegenerators
import re, sys

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;':     pagegenerators.parameterHelp,
}

msg = {
    'ar': u'بوت: إصلاح التحويلات',
    'cs': u'Robot opravil přesměrování',
    'en': u'Bot: Fixing redirects',
    'fa': u'ربات:تصحیح تغییرمسیرها',
    'he': u'בוט: מתקן הפניות',
    'ja': u'ロボットによる:リダイレクト回避',
    'nn': u'robot: retta omdirigeringar',
    'no': u'Robot: Retter omdirigeringer',
    'pl': u'Bot: naprawa przekierowań',
    'pt': u'Bot: Arrumando redirects',
    'sv': u'Bot: Rättar omdirigeringar',
    'vi': u'Robot: Sửa đổi hướng',
    'zh': u'機器人: 修復重定向',
}

featured_articles = {
    'ar': u'ويكيبيديا:مقالات مختارة',
    'cs': u'Wikipedie:Nejlepší články',
    'de': u'Wikipedia:Exzellente_Artikel',
    'en': u'Wikipedia:Featured_articles',
    'es': u'Wikipedia:Artículos_destacados',
    'fa': u'ویکی‌پدیا:نوشتارهای برگزیده',
    'fr': u'Wikipédia:Articles_de_qualité',
    'he': u'פורטל:ערכים_מומלצים',
    'it': u'Wikipedia:Articoli_in_vetrina',
    'ja': u'Wikipedia:秀逸な記事',
    'nl': u'Wikipedia:Etalage',
    'nn': u'Wikipedia:Gode artiklar',
    'no': u'Wikipedia:Anbefalte artikler',
    'pl': u'Wikipedia:Artykuły_na_medal',
    'pt': u'Wikipedia:Os_melhores_artigos',
    'sv': u'Wikipedia:Utvalda_artiklar',
    'vi': u'Wikipedia:Bài_viết_chọn_lọc',
    'zh': u'Wikipedia:特色条目',
}

def firstcap(string):
    return string[0].upper()+string[1:]

def treat(text, linkedPage, targetPage):
    """
    Based on the method of the same name in solve_disambiguation.py
    """
    mysite = pywikibot.getSite()
    linktrail = mysite.linktrail()

    # make a backup of the original text so we can show the changes later
    linkR = re.compile(r'\[\[(?P<title>[^\]\|#]*)(?P<section>#[^\]\|]*)?(\|(?P<label>[^\]]*))?\]\](?P<linktrail>' + linktrail + ')')
    curpos = 0
    # This loop will run until we have finished the current page
    while True:
        m = linkR.search(text, pos = curpos)
        if not m:
            break
        # Make sure that next time around we will not find this same hit.
        curpos = m.start() + 1
        # ignore interwiki links and links to sections of the same page
        if m.group('title').strip() == '' or \
           mysite.isInterwikiLink(m.group('title')):
            continue
        else:
            actualLinkPage = pywikibot.Page(targetPage.site(), m.group('title'))
            # Check whether the link found is to page.
            if actualLinkPage != linkedPage:
                continue

        # how many bytes should be displayed around the current link
        context = 15
        # at the beginning of the link, start red color.
        # at the end of the link, reset the color to default
        #pywikibot.output(text[max(0, m.start() - context) : m.start()] + '\03{lightred}' + text[m.start() : m.end()] + '\03{default}' + text[m.end() : m.end() + context])
        choice = 'y'

        # The link looks like this:
        # [[page_title|link_text]]trailing_chars
        page_title = m.group('title')
        link_text = m.group('label')

        if not link_text:
            # or like this: [[page_title]]trailing_chars
            link_text = page_title
        if m.group('section') == None:
            section = ''
        else:
            section = m.group('section')
        trailing_chars = m.group('linktrail')
        if trailing_chars:
            link_text += trailing_chars

        if choice in "uU":
            # unlink - we remove the section if there's any
            text = text[:m.start()] + link_text + text[m.end():]
            continue
        replaceit = choice in "rR"

        # remove preleading ":"
        if link_text[0]==':':
            link_text = link_text[1:]
        if link_text[0].isupper():
            new_page_title = targetPage.title()
        else:
            new_page_title = targetPage.title()[0].lower() + \
                             targetPage.title()[1:]

        # remove preleading ":"
        if new_page_title[0]==':':
            new_page_title = new_page_title[1:]

        if replaceit and trailing_chars:
            newlink = "[[%s%s]]%s" % (new_page_title, section, trailing_chars)
        elif replaceit or (new_page_title == link_text and not section):
            newlink = "[[%s]]" % new_page_title
        # check if we can create a link with trailing characters instead of a
        # pipelink
        elif len(new_page_title) <= len(link_text) and \
             firstcap(link_text[:len(new_page_title)]) == \
             firstcap(new_page_title) and \
             re.sub(re.compile(linktrail), '', link_text[len(new_page_title):]) == '' and not section:
            newlink = "[[%s]]%s" % (link_text[:len(new_page_title)],
                                    link_text[len(new_page_title):])
        else:
            newlink = "[[%s%s|%s]]" % (new_page_title, section, link_text)
        text = text[:m.start()] + newlink + text[m.end():]
        continue
    return text

pageCache = []

def workon(page):
    mysite = pywikibot.getSite()
    try:
        text = page.get()
    except pywikibot.IsRedirectPage:
        pywikibot.output(u'%s is a redirect page. Skipping'
                         % page.title(asLink=True))
        return
    except pywikibot.NoPage:
        pywikibot.output(u'%s does not exist. Skipping'
                         % page.title(asLink=True))
        return
    pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                     % page.title())
    links = page.linkedPages()
    if len(links) > 0:
        pywikibot.getall(mysite,links)
    else:
        pywikibot.output('Nothing left to do.')
        return

    for page2 in links:
        try:
            target = page2.getRedirectTarget()
        except (pywikibot.Error,pywikibot.SectionError):
            continue
        text = treat(text, page2, target)
    if text != page.get():
        comment = pywikibot.translate(mysite, msg)
        pywikibot.showDiff(page.get() ,text)
        try:
            page.put(text, comment)
        except (pywikibot.Error):
            pywikibot.output('Error: unable to put %s'
                             % page.title(asLink=True))

def main():
    featured = False
    gen = None

    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()

    for arg in pywikibot.handleArgs():
        if arg == '-featured':
            featured = True
        else:
            genFactory.handleArg(arg)

    mysite = pywikibot.getSite()
    if mysite.sitename() == 'wikipedia:nl':
        pywikibot.output(
            u'\03{lightred}There is consensus on the Dutch Wikipedia that bots should not be used to fix redirects.\03{default}')
        sys.exit()

    if featured:
        featuredList = pywikibot.translate(mysite, featured_articles)
        ref = pywikibot.Page(pywikibot.getSite(), featuredList)
        gen = pagegenerators.ReferringPageGenerator(ref)
        gen = pagegenerators.NamespaceFilterPageGenerator(gen, [0])
    if not gen:
        gen = genFactory.getCombinedGenerator()
    if gen:
        for page in pagegenerators.PreloadingGenerator(gen):
            workon(page)
    else:
        pywikibot.showHelp('fixing_redirects')

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
