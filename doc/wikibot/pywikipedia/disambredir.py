#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Goes through the disambiguation pages, checks their links, and asks for
each link that goes to a redirect page whether it should be replaced.
"""
#
# (C) André Engels and others, 2006-2009
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: disambredir.py 8805 2010-12-26 16:43:08Z xqt $'
#
import wikipedia as pywikibot
import pagegenerators
import sys, re
import catlib

msg = {
    'ar': u'تغيير التحويلات في صفحة توضيح',
    'en': u'Changing redirects on a disambiguation page',
    'he': u'משנה קישורים להפניות בדף פירושונים',
    'fa': u'اصلاح تغییرمسیرها در یک صفحه ابهام‌زدایی',
    'ja': u'ロボットによる: 曖昧さ回避ページのリダイレクト修正',
    'nl': u'Verandering van redirects op een doorverwijspagina',
    'pl': u'Zmiana przekierowań na stronie ujednoznaczającej',
    'pt': u'Arrumando redirects na página de desambiguação',
    'ru': u'Изменение перенаправлений на странице неоднозначности',
    'uk': u'Зміна перенаправлень на сторінці багатозначності',
    'zh': u'機器人: 修改消歧義頁中的重定向連結',
}

def firstcap(string):
    return string[0].upper()+string[1:]

def treat(text, linkedPage, targetPage):
    """
    Based on the method of the same name in solve_disambiguation.py.
    """
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
        if m.group('title') == '' or mysite.isInterwikiLink(m.group('title')):
            continue
        else:
            actualLinkPage = pywikibot.Page(page.site(), m.group('title'))
            # Check whether the link found is to page.
            if actualLinkPage != linkedPage:
                continue

        # how many bytes should be displayed around the current link
        context = 30
        # at the beginning of the link, start red color.
        # at the end of the link, reset the color to default
        pywikibot.output(text[max(0, m.start() - context) : m.start()] +
                         '\03{lightred}' + text[m.start() : m.end()] +
                         '\03{default}' + text[m.end() : m.end() + context])
        while True:
            choice = pywikibot.input(
                u"Option (N=do not change, y=change link to \03{lightpurple}%s\03{default}, r=change and replace text, u=unlink)"%targetPage.title())
            try:
                choice = choice[0]
            except:
                choice = 'N'
            if choice in 'nNyYrRuU':
                break
        if choice in "nN":
            continue

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

        if link_text[0].isupper():
            new_page_title = targetPage.title()
        else:
            new_page_title = targetPage.title()[0].lower() + \
                             targetPage.title()[1:]
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

def workon(page, links):
    text = page.get()
    # Show the title of the page we're working on.
    # Highlight the title in purple.
    pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                     % page.title())
    for page2 in links:
        try:
            target = page2.getRedirectTarget()
        except (pywikibot.Error,pywikibot.SectionError):
            continue
        text = treat(text, page2, target)
    if text != page.get():
        comment = pywikibot.translate(mysite, msg)
        page.put(text, comment)

def main():
    global mysite, linktrail, page
    start = []
    for arg in pywikibot.handleArgs():
        start.append(arg)
    if start:
        start = " ".join(start)
    else:
        start = "!"
    mysite = pywikibot.getSite()
    linktrail = mysite.linktrail()
    try:
        generator = pagegenerators.CategorizedPageGenerator(
            mysite.disambcategory(), start = start)
    except pywikibot.NoPage:
        pywikibot.output(
            "The bot does not know the disambiguation category for your wiki.")
        raise
    # only work on articles
    generator = pagegenerators.NamespaceFilterPageGenerator(generator, [0])
    generator = pagegenerators.PreloadingGenerator(generator)
    pagestodo = []
    pagestoload = []
    for page in generator:
        if page.isRedirectPage():
            continue
        linked = page.linkedPages()
        pagestodo.append((page,linked))
        pagestoload += linked
        if len(pagestoload) > 49:
            pywikibot.getall(mysite,pagestoload)
            for page, links in pagestodo:
                workon(page,links)
            pagestoload = []
            pagestodo = []

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()

