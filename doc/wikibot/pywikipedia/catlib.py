#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Library to work with category pages on Wikipedia
"""
#
# (C) Rob W.W. Hooft, Andre Engels, 2004-2007
# (C) Daniel Herding, 2004-2007
# (C) Russell Blau, 2005
# (C) Cyde Weys, 2005-2007
# (C) Leonardo Gregianin, 2005-2007
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: catlib.py 9226 2011-05-09 18:42:05Z cydeweys $'
#
import re, time, urllib, query
import wikipedia
try:
    set # introduced in Python 2.4: faster and future
except NameError:
    # fallback solution for Python 2.3
    from sets import Set as set

msg_created_for_renaming = {
    'ar':u'روبوت: نقل من %s. المؤلفون: %s',
    'de':u'Bot: Verschoben von %s. Autoren: %s',
    'en':u'Robot: Moved from %s. Authors: %s',
    'es':u'Bot: Traslado desde %s. Autores: %s',
    'fa':u'ربات: انتقال از %s. نویسندگان: %s',
    'fi':u'Botti siirsi luokan %s. Muokkaajat: %s',
    'fr':u'Robot : déplacé depuis %s. Auteurs: %s',
    'he':u'בוט: הועבר מהשם %s. כותבים: %s',
    'hu':u'Bottal áthelyezve innen: %s. Eredeti szerzők: %s',
    'ia':u'Robot: Transferite de %s. Autores: %s',
    'id':u'Bot: Memindahkan dari %s. Kontributor: %s',
    'it':u'Bot: Voce spostata da %s. Autori: %s',
    'ja': u'ロボットによる: %s から移動しました。原作者は %s',
    'ksh':u'Bot: hääjeholldt von %s. Schriiver: %s',
    'nds':u'Kat-Bot: herschaven von %s. Schriever: %s',
    'nl':u'Bot: hernoemd van %s. Auteurs: %s',
    'nn':u'robot: flytta frå %s. Bidragsytarar: %s',
    'pl':u'Robot przenosi z %s. Autorzy: %s',
    'pt':u'Bot: Movido de %s. Autor: %s',
    'zh':u'機器人: 已從 %s 移動。原作者是 %s',
    }

# some constants that are used internally
ARTICLE = 0
SUBCATEGORY = 1

def isCatTitle(title, site):
    return ':' in title and title[:title.index(':')] in site.category_namespaces()

def unique(l):
    """Given a list of hashable object, return an alphabetized unique list.
    """
    l=dict.fromkeys(l).keys()
    l.sort()
    return l


class Category(wikipedia.Page):
    """Subclass of Page that has some special tricks that only work for
       category: pages"""

    def __init__(self, site, title = None, insite = None, sortKey = None):
        wikipedia.Page.__init__(self, site = site, title = title, insite = insite, defaultNamespace = 14)
        self.sortKey = sortKey
        if self.namespace() != 14:
            raise ValueError(u'BUG: %s is not in the category namespace!' % title)
        self.completelyCached = False
        self.articleCache = []
        self.subcatCache = []

    def aslink(self, forceInterwiki=False, textlink=False, noInterwiki=False):
        """A string representation in the form of a link.

        This method is different from Page.aslink() as the sortkey may have
        to be included.

        """
        if self.sortKey:
            titleWithSortKey = '%s|%s' % (self.title(savetitle=True), self.sortKey)
        else:
            titleWithSortKey = self.title(savetitle=True)
        if not noInterwiki and (forceInterwiki
                                or self.site() != wikipedia.getSite()):
            if self.site().family != wikipedia.getSite().family \
                    and self.site().family.name != self.site().lang:
                return '[[%s:%s:%s]]' % (self.site().family.name,
                                         self.site().lang, self.title(savetitle=True))
            else:
                return '[[%s:%s]]' % (self.site().lang, self.title(savetitle=True))
        elif textlink:
            return '[[:%s]]' % self.title(savetitle=True)
        else:
            return '[[%s]]' % titleWithSortKey

    def _getAndCacheContents(self, recurse=False, purge=False, startFrom=None, cache=None,
                                   sortby=None, sortdir=None):
        """
        Cache results of _parseCategory for a second call.

        If recurse is a bool, and value is True, then recursively retrieves
        contents of all subcategories without limit. If recurse is an int,
        recursively retrieves contents of subcategories to that depth only.

        Other parameters are analogous to _parseCategory(). If purge is True,
        cached results will be discarded. If startFrom is used, nothing
        will be cached.

        This should not be used outside of this module.
        """
        if cache is None:
            cache = []
        if purge:
            self.completelyCached = False
        if recurse:
            if type(recurse) is int:
                newrecurse = recurse - 1
            else:
                newrecurse = recurse
        if self.completelyCached:
            for article in self.articleCache:
                if article not in cache:
                    cache.append(article)
                    yield ARTICLE, article
            for subcat in self.subcatCache:
                if subcat not in cache:
                    cache.append(subcat)
                    yield SUBCATEGORY, subcat
                    if recurse:
                        # contents of subcategory are cached by calling
                        # this method recursively; therefore, do not cache
                        # them again
                        for item in subcat._getAndCacheContents(newrecurse, purge, cache=cache,
                                            sortby=sortby, sortdir=sortdir):
                            yield item
        else:
            for tag, page in self._parseCategory(purge, startFrom, sortby, sortdir):
                if tag == ARTICLE:
                    self.articleCache.append(page)
                    if not page in cache:
                        cache.append(page)
                        yield ARTICLE, page
                elif tag == SUBCATEGORY:
                    self.subcatCache.append(page)
                    if not page in cache:
                        cache.append(page)
                        yield SUBCATEGORY, page
                        if recurse:
                            # contents of subcategory are cached by calling
                            # this method recursively; therefore, do not cache
                            # them again
                            for item in page._getAndCacheContents(newrecurse, purge, cache=cache,
                                             sortby=sortby, sortdir=sortdir):
                                yield item
            if not startFrom:
                self.completelyCached = True

    def _getContentsNaive(self, recurse=False, startFrom=None, sortby=None, sortdir=None):
        """
        Simple category content yielder. Naive, do not attempts to
        cache anything
        """
        for tag, page in self._parseCategory(startFrom=startFrom,
                                             sortby=sortby, sortdir=sortdir):
            yield tag, page
            if tag == SUBCATEGORY and recurse:
                for item in page._getContentsNaive(recurse=True,
                                                   sortby=sortby, sortdir=sortdir):
                    yield item

    def _parseCategory(self, purge=False, startFrom=None, sortby=None, sortdir=None):
        """
        Yields all articles and subcategories that are in this category by API.

        Set startFrom to a string which is the title of the page to start from.

        Yielded results are tuples in the form (tag, page) where tag is one
        of the constants ARTICLE and SUBCATEGORY, and title is the Page or Category
        object.

        Note that results of this method need not be unique.

        This should not be used outside of this module.
        """
        if not self.site().has_api() or self.site().versionnumber() < 11:
            for tag, page in self._oldParseCategory(purge, startFrom):
                yield tag, page
            return

        currentPageOffset = None
        params = {
            'action': 'query',
            'list': 'categorymembers',
            'cmtitle': self.title(),
            'cmprop': ['title', 'ids', 'sortkey', 'timestamp'],
            #'': '',
        }
        if sortby:
            params['cmsort'] = sortby
        if sortdir:
            params['cmdir'] = sortdir
        while True:
            if wikipedia.config.special_page_limit > 500:
                params['cmlimit'] = 500
            else:
                params['cmlimit'] = wikipedia.config.special_page_limit

            if currentPageOffset:
                params.update(currentPageOffset)
                wikipedia.output('Getting [[%s]] list from %s...'
                                 % (self.title(), "%s=%s" % currentPageOffset.popitem()))
            elif startFrom:
                startFrom = startFrom.upper() # category sort keys are uppercase
                params['cmstartsortkey'] = startFrom
                wikipedia.output('Getting [[%s]] list starting at %s...'
                                 % (self.title(), startFrom))
            else:
                wikipedia.output('Getting [[%s]]...' % self.title())

            wikipedia.get_throttle()
            data = query.GetData(params, self.site())
            if 'error' in data:
                raise RuntimeError("%s" % data['error'])
            count = 0

            for memb in data['query']['categorymembers']:
                count += 1
                # For MediaWiki versions where subcats look like articles
                if memb['ns'] == 14:
                    yield SUBCATEGORY, Category(self.site(), memb['title'], sortKey=memb['sortkey'])
                elif memb['ns'] == 6:
                    yield ARTICLE, wikipedia.ImagePage(self.site(), memb['title'])
                else:
                    yield ARTICLE, wikipedia.Page(self.site(), memb['title'], defaultNamespace=memb['ns'])
                if count >= params['cmlimit']:
                    break
            # try to find a link to the next list page
            if 'query-continue' in data and count < params['cmlimit']:
                currentPageOffset = data['query-continue']['categorymembers']
            else:
                break

    def _oldParseCategory(self, purge=False, startFrom=None):
        """
        Yields all articles and subcategories that are in this category.

        Set purge to True to instruct MediaWiki not to serve a cached version.

        Set startFrom to a string which is the title of the page to start from.

        Yielded results are tuples in the form (tag, page) where tag is one
        of the constants ARTICLE and SUBCATEGORY, and title is the Page or Category
        object.

        Note that results of this method need not be unique.

        This should not be used outside of this module.
        """
        if self.site().versionnumber() < 4:
            Rtitle = re.compile('title\s?=\s?\"([^\"]*)\"')
        elif self.site().versionnumber() < 8:
            # FIXME seems to parse all links
            Rtitle = re.compile('/\S*(?: title\s?=\s?)?\"([^\"]*)\"')
        else:
            Rtitle = re.compile(
            '<li>(?:<span.*?>)?<a href=\".*?\"\s?title\s?=\s?\"([^\"]*)\"\>\+?[^\<\+]')
        if self.site().versionnumber() < 8:
            Rsubcat = None
            Rimage = None
        else:
            Rsubcat = re.compile(
                'CategoryTreeLabelCategory\"\s?href=\".+?\">(.+?)</a>')
            Rimage = re.compile(
                '<div class\s?=\s?\"thumb\"\sstyle=\"[^\"]*\">(?:<div style=\"[^\"]*\">)?<a href=\".*?\"(?:\sclass="image")?\stitle\s?=\s?\"([^\"]*)\"')
        ns = self.site().category_namespaces()
        # regular expression matching the "(next 200)" link
        RLinkToNextPage = re.compile('&amp;from=(.*?)" title="');

        if startFrom:
            currentPageOffset = urllib.quote(startFrom.encode(self.site().encoding()))
        else:
            currentPageOffset = None
        while True:
            path = self.site().get_address(self.urlname())
            if purge:
                path += '&action=purge'
            if currentPageOffset:
                path += '&from=' + currentPageOffset
                wikipedia.output('Getting [[%s]] starting at %s...'
                                 % (self.title(), wikipedia.url2link(currentPageOffset, self.site(), self.site())))
            else:
                wikipedia.output('Getting [[%s]]...' % self.title())
            wikipedia.get_throttle()
            txt = self.site().getUrl(path)
            # index where subcategory listing begins
            if self.site().versionnumber() >= 9:
                # These IDs were introduced in 1.9
                if '<div id="mw-subcategories">' in txt:
                    ibegin = txt.index('<div id="mw-subcategories">')
                elif '<div id="mw-pages">' in txt:
                    ibegin = txt.index('<div id="mw-pages">')
                elif '<div id="mw-category-media">' in txt:
                    ibegin = txt.index('<div id="mw-category-media">')
                else:
                    # No pages
                    return
            else:
                ibegin = txt.index('<!-- start content -->') # does not work for cats without text
                # TODO: This parses category text and may think they are
                # pages in category! Check for versions before 1.9
            # index where article listing ends
            if '<div class="printfooter">' in txt:
                iend = txt.index('<div class="printfooter">')
            elif '<div class="catlinks">' in txt:
                iend = txt.index('<div class="catlinks">')
            else:
                iend = txt.index('<!-- end content -->')
            txt = txt[ibegin:iend]
            for title in Rtitle.findall(txt):
                if title == self.title():
                    # This is only a link to "previous 200" or "next 200".
                    # Ignore it.
                    pass
                # For MediaWiki versions where subcats look like articles
                elif isCatTitle(title, self.site()):
                    yield SUBCATEGORY, Category(self.site(), title)
                else:
                    yield ARTICLE, wikipedia.Page(self.site(), title)
            if Rsubcat:
                # For MediaWiki versions where subcats look differently
                for titleWithoutNamespace in Rsubcat.findall(txt):
                    title = 'Category:%s' % titleWithoutNamespace
                    yield SUBCATEGORY, Category(self.site(), title)
            if Rimage:
                # For MediaWiki versions where images work through galleries
                for title in Rimage.findall(txt):
                    # In some MediaWiki versions, the titles contain the namespace,
                    # but they don't in other (newer) versions. Use the ImagePage's
                    # defaultNamespace feature to get everything correctly.
                    yield ARTICLE, wikipedia.ImagePage(self.site(), title)
            # try to find a link to the next list page
            matchObj = RLinkToNextPage.search(txt)
            if matchObj:
                currentPageOffset = matchObj.group(1)
            else:
                break

    def subcategories(self, recurse=False, startFrom=None, cacheResults=False,
                            sortby=None, sortdir=None):
        """
        Yields all subcategories of the current category.

        If recurse is True, also yields subcategories of the subcategories.
        If recurse is a number, also yields subcategories of subcategories,
        but only at most that number of levels deep (that is, recurse = 0 is
        equivalent to recurse = False, recurse = 1 gives first-level
        subcategories of subcategories but no deeper, etcetera).

        cacheResults - cache the category contents: useful if you need to
        do several passes on the category members list. The simple cache
        system is *not* meant to be memory or cpu efficient for large
        categories

        Results a sorted (as sorted by MediaWiki), but need not be unique.
        """
        if cacheResults:
            gen = self._getAndCacheContents
        else:
            gen = self._getContentsNaive
        for tag, subcat in gen(recurse=recurse, startFrom=startFrom, sortby=sortby,
                               sortdir=sortdir):
            if tag == SUBCATEGORY:
                yield subcat

    def subcategoriesList(self, recurse=False, sortby=None, sortdir=None):
        """
        Creates a list of all subcategories of the current category.

        If recurse is True, also return subcategories of the subcategories.
        Recurse can also be a number, as explained above.

        The elements of the returned list are sorted and unique.
        """
        subcats = []
        for cat in self.subcategories(recurse, sortby=sortby, sortdir=sortdir):
            subcats.append(cat)
        return unique(subcats)

    def articles(self, recurse=False, startFrom=None, cacheResults=False,
                       sortby=None, sortdir=None):
        """
        Yields all articles of the current category.

        If recurse is True, also yields articles of the subcategories.
        Recurse can be a number to restrict the depth at which subcategories
        are included.

        cacheResults - cache the category contents: useful if you need to
        do several passes on the category members list. The simple cache
        system is *not* meant to be memory or cpu efficient for large
        categories

        Results are unsorted (except as sorted by MediaWiki), and need not
        be unique.
        """
        if cacheResults:
            gen = self._getAndCacheContents
        else:
            gen = self._getContentsNaive
        for tag, page in gen(recurse=recurse, startFrom=startFrom,
                             sortby=sortby, sortdir=sortdir):
            if tag == ARTICLE:
                yield page

    def articlesList(self, recurse=False, sortby=None, sortdir=None):
        """
        Creates a list of all articles of the current category.

        If recurse is True, also return articles of the subcategories.
        Recurse can be a number to restrict the depth at which subcategories
        are included.

        The elements of the returned list are sorted and unique.
        """
        articles = []
        for article in self.articles(recurse, sortby=sortby, sortdir=sortdir):
            articles.append(article)
        return unique(articles)

    def supercategories(self):
        """
        Yields all supercategories of the current category.

        Results are stored in the order in which they were entered, and need
        not be unique.
        """
        for supercat in self.categories():
            yield supercat

    def supercategoriesList(self):
        """
        Creates a list of all supercategories of the current category.

        The elements of the returned list are sorted and unique.
        """
        supercats = []
        for cat in self.supercategories():
            supercats.append(cat)
        return unique(supercats)

    def isEmptyCategory(self):
        """Return True if category has no members (including subcategories)."""
        for tag, title in self._parseCategory():
            return False
        return True

    def copyTo(self, catname):
        """
        Returns true if copying was successful, false if target page already
        existed.
        """
        catname = self.site().category_namespace() + ':' + catname
        targetCat = wikipedia.Page(self.site(), catname)
        if targetCat.exists():
            wikipedia.output('Target page %s already exists!' % targetCat.title())
            return False
        else:
            wikipedia.output('Moving text from %s to %s.' % (self.title(), targetCat.title()))
            authors = ', '.join(self.contributingUsers())
            creationSummary = wikipedia.translate(wikipedia.getSite(), msg_created_for_renaming) % (self.title(), authors)
            #Maybe sometimes length of summary is more than 200 characters and thus will not be shown.
            #For avoidning copyright violation bot must listify authors in another place
            if len(creationSummary)>200:
                talkpage=targetCat.toggleTalkPage()
                try:
                    talktext=talkpage.get()
                except wikipedia.NoPage:
                    talkpage.put(u"==Authors==\n%s-~~~~" % authors, u"Bot:Listifying authors")
                else:
                    talkpage.put(talktext+u"\n==Authors==\n%s-~~~~" % authors, u"Bot:Listifying authors")
            targetCat.put(self.get(), creationSummary)
            return True

    #Like copyTo above, except this removes a list of templates (like deletion templates) that appear in
    #the old category text.  It also removes all text between the two HTML comments BEGIN CFD TEMPLATE
    #and END CFD TEMPLATE. (This is to deal with CFD templates that are substituted.)
    def copyAndKeep(self, catname, cfdTemplates):
        """
        Returns true if copying was successful, false if target page already
        existed.
        """
        targetCat = wikipedia.Page(self.site(), catname, defaultNamespace=14)
        if targetCat.exists():
            wikipedia.output('Target page %s already exists!' % targetCat.title())
            return False
        else:
            wikipedia.output('Moving text from %s to %s.' % (self.title(), targetCat.title()))
            authors = ', '.join(self.contributingUsers())
            creationSummary = wikipedia.translate(wikipedia.getSite(), msg_created_for_renaming) % (self.title(), authors)
            newtext = remove_cfd_templates(cfdTemplates, self.get())
        targetCat.put(newtext, creationSummary)
        return True

def remove_cfd_templates(cfdTemplates, pageText):
    for regexName in cfdTemplates:
        matchcfd = re.compile(r"{{%s.*?}}" % regexName, re.IGNORECASE)
        pageText = matchcfd.sub('',pageText)
        matchcomment = re.compile(r"<!--BEGIN CFD TEMPLATE-->.*<!--END CFD TEMPLATE-->", re.IGNORECASE | re.MULTILINE | re.DOTALL)
        pageText = matchcomment.sub('',pageText)
        pos = 0
        while (pageText[pos:pos+1] == "\n"):
            pos = pos + 1
        pageText = pageText[pos:]
    return pageText

def add_category(article, category, comment=None, createEmptyPages=False):
    """
    Given an article and a category, adds the article to the category.
    """
    cats = article.categories(get_redirect=True)
    if not category in cats:
        cats.append(category)
        try:
            text = article.get()
        except wikipedia.NoPage:
            if createEmptyPages:
                text = ''
            else:
                raise

        text = wikipedia.replaceCategoryLinks(text, cats)
        try:
            article.put(text, comment=comment)
        except wikipedia.EditConflict:
            wikipedia.output(u'Skipping %s because of edit conflict' % article.title())


#def Category(code, name):
#    """Factory method to create category link objects from the category name"""
#    # Standardized namespace
#    ns = wikipedia.getSite().category_namespaces()[0]
#    # Prepend it
#    return Category(code, "%s:%s" % (ns, name))

def change_category(article, oldCat, newCat, comment=None, sortKey=None, inPlace=False):
    """
    Given an article which is in category oldCat, moves it to
    category newCat. Moves subcategories of oldCat as well.
    oldCat and newCat should be Category objects.
    If newCat is None, the category will be removed.
    """
    cats = article.categories(get_redirect=True)
    site = article.site()
    changesMade = False

    if not article.canBeEdited():
        wikipedia.output("Can't edit %s, skipping it..." % article.aslink())
        return False
    if inPlace == True:
        oldtext = article.get(get_redirect=True)
        newtext = wikipedia.replaceCategoryInPlace(oldtext, oldCat, newCat)
        if newtext == oldtext:
            wikipedia.output(
                u'No changes in made in page %s.' % article.aslink())
            return False
        try:
            article.put(newtext, comment)
            return True
        except wikipedia.EditConflict:
            wikipedia.output(
                u'Skipping %s because of edit conflict' % article.aslink())
        except wikipedia.LockedPage:
            wikipedia.output(u'Skipping locked page %s' % article.aslink())
        except wikipedia.SpamfilterError, error:
            wikipedia.output(
                u'Changing page %s blocked by spam filter (URL=%s)'
                             % (article.aslink(), error.url))
        except wikipedia.NoUsername:
            wikipedia.output(
                u"Page %s not saved; sysop privileges required."
                             % article.aslink())
        except wikipedia.PageNotSaved, error:
            wikipedia.output(u"Saving page %s failed: %s"
                             % (article.aslink(), error.message))
        return False

    # This loop will replace all occurrences of the category to be changed,
    # and remove duplicates.
    newCatList = []
    newCatSet = set()
    for i in range(len(cats)):
        cat = cats[i]
        if cat == oldCat:
            changesMade = True
            if not sortKey:
                sortKey = cat.sortKey
            if newCat:
                if newCat.title() not in newCatSet:
                    newCategory = Category(site, newCat.title(),
                                           sortKey=sortKey)
                    newCatSet.add(newCat.title())
                    newCatList.append(newCategory)
        elif cat.title() not in newCatSet:
            newCatSet.add(cat.title())
            newCatList.append(cat)

    if not changesMade:
        wikipedia.output(u'ERROR: %s is not in category %s!' % (article.aslink(), oldCat.title()))
    else:
        text = article.get(get_redirect=True)
        try:
            text = wikipedia.replaceCategoryLinks(text, newCatList)
        except ValueError:
            # Make sure that the only way replaceCategoryLinks() can return
            # a ValueError is in the case of interwiki links to self.
            wikipedia.output(
                    u'Skipping %s because of interwiki link to self' % article)
        try:
            article.put(text, comment)
        except wikipedia.EditConflict:
            wikipedia.output(
                    u'Skipping %s because of edit conflict' % article.title())
        except wikipedia.SpamfilterError, e:
            wikipedia.output(
                    u'Skipping %s because of blacklist entry %s'
                    % (article.title(), e.url))
        except wikipedia.LockedPage:
            wikipedia.output(
                    u'Skipping %s because page is locked' % article.title())
        except wikipedia.PageNotSaved, error:
            wikipedia.output(u"Saving page %s failed: %s"
                             % (article.aslink(), error.message))

def categoryAllElementsAPI(CatName, cmlimit = 5000, categories_parsed = [], site = None):
    #action=query&list=categorymembers&cmlimit=500&cmtitle=Category:License_tags
    """
    Category to load all the elements in a category using the APIs. Limit: 5000 elements.
    """
    wikipedia.output("Loading %s..." % CatName)

    params = {
        'action'    :'query',
        'list'      :'categorymembers',
        'cmlimit'   :cmlimit,
        'cmtitle'   :CatName,
        }

    data = query.GetData(params, site, encodeTitle = False)
    categories_parsed.append(CatName)
    try:
        members = data['query']['categorymembers']
    except KeyError:
        if int(cmlimit) != 500:
            wikipedia.output(u'An Error occured, trying to reload the category.')
            return categoryAllElementsAPI(CatName, cmlimit = 500)
        else:
            raise wikipedia.Error(data)
    if len(members) == int(cmlimit):
        raise wikipedia.Error(u'The category selected has >= %s elements, limit reached.' % cmlimit)
    allmembers = members
    results = list()
    for subcat in members:
        ns = subcat['ns']
        pageid = subcat['pageid']
        title = subcat['title']
        if ns == 14:
            if title not in categories_parsed:
                categories_parsed.append(title)
                (results_part, categories_parsed) = categoryAllElementsAPI(title, 5000, categories_parsed)
                allmembers.extend(results_part)
    for member in allmembers:
        ns = member['ns']
        pageid = member['pageid']
        title = member['title']
        results.append(member)
    return (results, categories_parsed)

def categoryAllPageObjectsAPI(CatName, cmlimit = 5000, categories_parsed = [], site = None):
    """
    From a list of dictionaries, return a list of page objects.
    """
    final = list()
    if site == None:
        site = wikipedia.getSite()
    for element in categoryAllElementsAPI(CatName, cmlimit, categories_parsed, site)[0]:
        final.append(wikipedia.Page(site, element['title']))
    return final

def test():
    site = wikipedia.getSite()

    cat = Category(site, 'Category:Software')

    wikipedia.output(u'SUBCATEGORIES:')
    for subcat in cat.subcategories():
        wikipedia.output(subcat.title())
    wikipedia.output(u'\nARTICLES:')
    for article in cat.articles():
        wikipedia.output(article.title())

if __name__=="__main__":
    import sys
    for arg in sys.argv[1:]:
        wikipedia.output(u'Ignored argument: %s' % arg)
    test()
