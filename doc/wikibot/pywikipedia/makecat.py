# -*- coding: UTF-8 -*-
"""
This bot takes as its argument (or, if no argument is given, asks for it), the
name of a new or existing category. It will then try to find new articles for
this category (pages linked to and from pages already in the category), asking
the user which pages to include and which not.

Arguments:
   -nodates  automatically skip all pages that are years or dates (years
             only work AD, dates only for certain languages)
   -forward  only check pages linked from pages already in the category,
             not pages linking to them. Is less precise but quite a bit
             faster.
   -exist    only ask about pages that do actually exist; drop any
             titles of non-existing pages silently. If -forward is chosen,
             -exist is automatically implied.
   -keepparent  do not remove parent categories of the category to be
             worked on.
   -all      work on all pages (default: only main namespace)

When running the bot, you will get one by one a number by pages. You can
choose:
Y(es) - include the page
N(o) - do not include the page or
I(gnore) - do not include the page, but if you meet it again, ask again.
X - add the page, but do not check links to and from it
Other possiblities:
A(dd) - add another page, which may have been one that was included before
C(heck) - check links to and from the page, but do not add the page itself
R(emove) - remove a page that is already in the list
L(ist) - show current list of pages to include or to check
"""

# (C) Andre Engels, 2004
# (C) Pywikipedia bot team 2005-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: makecat.py 8629 2010-10-09 16:11:46Z xqt $'
#

import sys, codecs, re
import date
import wikipedia as pywikibot
import catlib

msg={
    'ar':u'إنشاء أو تحديث التصنيف:',
    'en':u'Creation or update of category:',
    'es':u'Creación o actualiza de la categoría:',
    'fa':u'ایجاد یا تصحیح رده:',
    'fr':u'Création ou mise à jour de categorie:',
    'he':u'יצירה או עדכון של קטגוריה:',
    'ia':u'Creation o actualisation de categoria:',
    'it':u'La creazione o laggiornamento di categoria:',
    'nl':u'Aanmaak of uitbreiding van categorie:',
    'nn':u'oppretting eller oppdatering av kategori:',
    'no':u'opprettelse eller oppdatering av kategori:',
    'pl':u'Stworzenie lub aktualizacja kategorii:',
    'pt':u'Criando ou atualizando categoria:',
    }

def rawtoclean(c):
    #Given the 'raw' category, provides the 'clean' category
    c2 = c.title().split('|')[0]
    return pywikibot.Page(mysite,c2)

def isdate(s):
    """returns true iff s is a date or year
    """
    dict,val = date.getAutoFormat( pywikibot.getSite().language(), s )
    return dict is not None

def needcheck(pl):
    if main:
        if pl.namespace() != 0:
            return False
    if pl in checked:
        return False
    if skipdates:
        if isdate(pl.title()):
            return False
    return True

def include(pl,checklinks=True,realinclude=True,linkterm=None):
    cl = checklinks
    if linkterm:
        actualworkingcat = catlib.Category(mysite,workingcat.title(),
                                           sortKey=linkterm)
    else:
        actualworkingcat = workingcat
    if realinclude:
        try:
            text = pl.get()
        except pywikibot.NoPage:
            pass
        except pywikibot.IsRedirectPage:
            cl = True
            pass
        else:
            cats = pl.categories()
            if not workingcat in cats:
                cats = pl.categories()
                for c in cats:
                    if c in parentcats:
                        if removeparent:
                            catlib.change_category(pl,c,actualworkingcat)
                            break
                else:
                    pl.put(pywikibot.replaceCategoryLinks(
                        text, cats + [actualworkingcat]))
    if cl:
        if checkforward:
            for page2 in pl.linkedPages():
                if needcheck(page2):
                    tocheck.append(page2)
                    checked[page2] = page2
        if checkbackward:
            for refPage in pl.getReferences():
                 if needcheck(refPage):
                    tocheck.append(refPage)
                    checked[refPage] = refPage

def exclude(pl,real_exclude=True):
    if real_exclude:
        excludefile.write('%s\n'%pl.title())

def asktoadd(pl):
    if pl.site() != mysite:
        return
    if pl.isRedirectPage():
        pl2 = pl.getRedirectTarget()
        if needcheck(pl2):
            tocheck.append(pl2)
            checked[pl2]=pl2
        return
    ctoshow = 500
    pywikibot.output(u'')
    pywikibot.output(u"==%s=="%pl.title())
    while 1:
        answer = raw_input("y(es)/n(o)/i(gnore)/(o)ther options? ")
        if answer=='y':
            include(pl)
            break
        if answer=='c':
            include(pl,realinclude=False)
            break
        if answer=='z':
            if pl.exists():
                if not pl.isRedirectPage():
                    linkterm = pywikibot.input(
                        u"In what manner should it be alphabetized?")
                    include(pl,linkterm=linkterm)
                    break
            include(pl)
            break
        elif answer=='n':
            exclude(pl)
            break
        elif answer=='i':
            exclude(pl,real_exclude=False)
            break
        elif answer=='o':
            pywikibot.output(u"t: Give the beginning of the text of the page")
            pywikibot.output(
                u"z: Add under another title (as [[Category|Title]])")
            pywikibot.output(
                u"x: Add the page, but do not check links to and from it")
            pywikibot.output(u"c: Do not add the page, but do check links")
            pywikibot.output(u"a: Add another page")
            pywikibot.output(u"l: Give a list of the pages to check")
        elif answer=='a':
            pagetitle = raw_input("Specify page to add:")
            page=pywikibot.Page(pywikibot.getSite(),pagetitle)
            if not page in checked.keys():
                include(page)
        elif answer=='x':
            if pl.exists():
                if pl.isRedirectPage():
                    pywikibot.output(
                        u"Redirect page. Will be included normally.")
                    include(pl,realinclude=False)
                else:
                    include(pl,checklinks=False)
            else:
                pywikibot.output(u"Page does not exist; not added.")
                exclude(pl,real_exclude=False)
            break
        elif answer=='l':
            pywikibot.output(u"Number of pages still to check: %s"
                             % len(tocheck))
            pywikibot.output(u"Pages to be checked:")
            pywikibot.output(u" - ".join(page.title() for page in tocheck))
            pywikibot.output(u"==%s=="%pl.title())
        elif answer=='t':
            pywikibot.output(u"==%s=="%pl.title())
            try:
                pywikibot.output(u''+pl.get(get_redirect=True)[0:ctoshow])
            except pywikibot.NoPage:
                pywikibot.output(u"Page does not exist.")
            ctoshow += 500
        else:
            pywikibot.output(u"Not understood.")

try:
    checked = {}
    skipdates = False
    checkforward = True
    checkbackward = True
    checkbroken = True
    removeparent = True
    main = True
    workingcatname = []
    tocheck = []
    for arg in pywikibot.handleArgs():
        if arg.startswith('-nodate'):
            skipdates = True
        elif arg.startswith('-forward'):
            checkbackward = False
            checkbroken = False
        elif arg.startswith('-exist'):
            checkbroken = False
        elif arg.startswith('-keepparent'):
            removeparent = False
        elif arg.startswith('-all'):
            main = False
        else:
            workingcatname.append(arg)

    if len(workingcatname) == 0:
        workingcatname = raw_input("Which page to start with? ")
    else:
        workingcatname = ' '.join(workingcatname)
    mysite = pywikibot.getSite()
    pywikibot.setAction(pywikibot.translate(mysite,msg) + ' ' + workingcatname)
    workingcat = catlib.Category(mysite,
                                 u'%s:%s'
                                 % (mysite.category_namespace(),
                                    workingcatname))
    filename = pywikibot.config.datafilepath('category',
                   pywikibot.UnicodeToAsciiHtml(workingcatname) +'_exclude.txt')
    try:
        f = codecs.open(filename, 'r', encoding = mysite.encoding())
        for line in f.readlines():
            # remove trailing newlines and carriage returns
            try:
                while line[-1] in ['\n', '\r']:
                    line = line[:-1]
            except IndexError:
                pass
            exclude(line,real_exclude=False)
            pl = pywikibot.Page(mysite,line)
            checked[pl] = pl
        f.close()
        excludefile = codecs.open(filename, 'a', encoding = mysite.encoding())
    except IOError:
        # File does not exist
        excludefile = codecs.open(filename, 'w', encoding = mysite.encoding())
    try:
        parentcats = workingcat.categories()
    except pywikibot.Error:
        parentcats = []
    # Do not include articles already in subcats; only checking direct subcats
    subcatlist = workingcat.subcategoriesList()
    if subcatlist:
        pywikibot.getall(mysite,subcatlist)
        for cat in subcatlist:
            list = cat.articlesList()
            for page in list:
                exclude(page.title(),real_exclude=False)
                checked[page] = page
    list = workingcat.articlesList()
    if list:
        for pl in list:
            checked[pl]=pl
        pywikibot.getall(mysite,list)
        for pl in list:
            include(pl)
    else:
        pywikibot.output(
            u"Category %s does not exist or is empty. Which page to start with?"
            % workingcatname)
        answer = pywikibot.input(u"(Default is [[%s]]):" % workingcatname)
        if not answer:
            answer = workingcatname
        pywikibot.output(u''+answer)
        pl = pywikibot.Page(mysite,answer)
        tocheck = []
        checked[pl] = pl
        include(pl)
    loaded = 0
    while tocheck:
        if loaded == 0:
            if len(tocheck) < 50:
                loaded = len(tocheck)
            else:
                loaded = 50
            pywikibot.getall(mysite,tocheck[:loaded])
        if not checkbroken:
            if not tocheck[0].exists():
                pass
            else:
                asktoadd(tocheck[0])
        else:
            asktoadd(tocheck[0])
        tocheck = tocheck[1:]
        loaded -= 1

finally:
    pywikibot.stopme()
    try:
        excludefile.close()
    except:
        pass
