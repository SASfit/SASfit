#!/usr/bin/python
# -*- coding: utf-8  -*-
""" Script to enumerate all pages on the wiki and find all titles
with mixed latin and cyrilic alphabets.
"""

#
# Permutations code was taken from http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/190465
#
from __future__ import generators

def xuniqueCombinations(items, n):
    if n==0: yield []
    else:
        for i in xrange(len(items)):
            for cc in xuniqueCombinations(items[i+1:],n-1):
                yield [items[i]]+cc
# End of permutation code
#
# (C) Pywikipedia bot team, 2006-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: casechecker.py 8240 2010-06-03 07:19:33Z xqt $'

#
# Windows Concose colors
# This code makes this script Windows ONLY!!!
# Feel free to adapt it to another platform
#
# Adapted from http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/496901
#
STD_OUTPUT_HANDLE= -11

FOREGROUND_BLUE = 0x01 # text color contains blue.
FOREGROUND_GREEN= 0x02 # text color contains green.
FOREGROUND_RED  = 0x04 # text color contains red.
FOREGROUND_INTENSITY = 0x08 # text color is intensified.
BACKGROUND_BLUE = 0x10 # background color contains blue.
BACKGROUND_GREEN= 0x20 # background color contains green.
BACKGROUND_RED  = 0x40 # background color contains red.
BACKGROUND_INTENSITY = 0x80 # background color is intensified.

FOREGROUND_WHITE = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED

try:
    import ctypes
    std_out_handle = ctypes.windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
except:
    std_out_handle = None


def SetColor(color):
    if std_out_handle:
        try:
            return ctypes.windll.kernel32.SetConsoleTextAttribute(
                std_out_handle, color)
        except:
            pass

    if color == FOREGROUND_BLUE: print '(b:',
    if color == FOREGROUND_GREEN: print '(g:',
    if color == FOREGROUND_RED: print '(r:',

# end of console code

import os
import sys, query, re, codecs
import wikipedia as pywikibot


class CaseChecker( object ):
    msgRename = {
        'ar': u'تغيير اسم لحالة مخلوطة',
        'en': u'mixed case rename',
        'ru': u'[[ВП:КЛ]]',
    }
    msgLinkReplacement = {
        'en': u'Case Replacements',
        'ar': u'استبدالات الحالة',
        'ru': u'[[ВП:КЛ]]',
    }

    # These words are always in one language, even though they could be typed
    # in both
    alwaysInLocal = [ u'СССР', u'Как', u'как' ]
    alwaysInLatin = [ u'II', u'III' ]

    localUpperLtr = u'ЁІЇЎАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯҐ'
    localLowerLtr = u'ёіїўабвгдежзийклмнопрстуфхцчшщъыьэюяґ'
    localLtr = localUpperLtr + localLowerLtr

    localSuspects = u'АВЕКМНОРСТХІЁЇаеорсухіёї'
    latinSuspects = u'ABEKMHOPCTXIËÏaeopcyxiëï'

    # possibly try to fix one character mistypes in an alternative keyboard
    # layout
    localKeyboard = u'йцукенгшщзфывапролдячсмить'
    latinKeyboard = u'qwertyuiopasdfghjklzxcvbnm'

    romanNumChars = u'IVXLMC'
    # all letters that may be used as suffixes after roman numbers:  "Iый"
    romannumSuffixes = localLowerLtr
    romanNumSfxPtrn = re.compile(
        u'^[' + romanNumChars + ']+[' + localLowerLtr + ']+$')

    whitelists = {
        'ru': u'ВП:КЛ/Whitelist'
        }

    latLtr = u'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'

    lclClrFnt = u'<font color=green>'
    latClrFnt = u'<font color=brown>'
    suffixClr = u'</font>'

    wordBreaker = re.compile(u'[ _\-/\|#[\]()]')

    titles = True
    links = False
    aplimit = 500
    apfrom = u''
    title = None
    replace = False
    stopAfter = 0
    wikilog = None
    wikilogfile = 'wikilog.txt'
    autonomous = False
    namespaces = []

    def __init__(self):

        for arg in pywikibot.handleArgs():
            if arg.startswith('-from'):
                if arg.startswith('-from:'):
                    self.apfrom = arg[6:]
                else:
                    self.apfrom = pywikibot.input(u'Which page to start from: ')
            elif arg.startswith('-reqsize:'):
                self.aplimit = int(arg[9:])
            elif arg == '-links':
                self.links = True
            elif arg == '-linksonly':
                self.links = True
                self.titles = False
            elif arg == '-replace':
                self.replace = True
            elif arg.startswith('-limit:'):
                self.stopAfter = int(arg[7:])
            elif arg == '-autonomous' or arg == '-a':
                self.autonomous = True
            elif arg.startswith('-ns:'):
                self.namespaces.append( int(arg[4:]) )
            elif arg.startswith('-wikilog:'):
                self.wikilogfile = arg[9:]
            else:
                pywikibot.output(u'Unknown argument %s.' % arg)
                pywikibot.showHelp()
                sys.exit()

        if self.namespaces == []:
            if self.apfrom == u'':
                # 0 should be after templates ns
                self.namespaces = [14, 10, 12, 0]
            else:
                self.namespaces = [0]

        self.params = { 'action'        : 'query',
                        'generator'     : 'allpages',
                        'gaplimit'      : self.aplimit,
                        'gapfilterredir': 'nonredirects'}

        if self.links:
            self.params['prop'] = 'links|categories'


        self.site = pywikibot.getSite()

        if len(self.localSuspects) != len(self.latinSuspects):
            raise ValueError(u'Suspects must be the same size')
        if len(self.localKeyboard) != len(self.latinKeyboard):
            raise ValueError(u'Keyboard info must be the same size')

        if not os.path.isabs(self.wikilogfile):
            self.wikilogfile = pywikibot.config.datafilepath(self.wikilogfile)
        try:
            self.wikilog = codecs.open(self.wikilogfile, 'a', 'utf-8')
        except IOError:
            self.wikilog = codecs.open(self.wikilogfile, 'w', 'utf-8')

        self.lclToLatDict = dict([(ord(self.localSuspects[i]),
                                   self.latinSuspects[i])
                                  for i in range(len(self.localSuspects))])
        self.latToLclDict = dict([(ord(self.latinSuspects[i]),
                                   self.localSuspects[i])
                                  for i in range(len(self.localSuspects))])

        if self.localKeyboard is not None:
            self.lclToLatKeybDict = dict(
                [(ord(self.localKeyboard[i]),
                  self.latinKeyboard[i])
                 for i in range(len(self.localKeyboard))])
            self.latToLclKeybDict = dict(
                [(ord(self.latinKeyboard[i]),
                  self.localKeyboard[i])
                 for i in range(len(self.localKeyboard))])
        else:
            self.lclToLatKeybDict = {}
            self.latToLclKeybDict = {}

        badPtrnStr = u'([%s][%s]|[%s][%s])' \
                     % (self.latLtr, self.localLtr, self.localLtr, self.latLtr)
        self.badWordPtrn = re.compile(u'[%s%s]*%s[%s%s]*'
                                      % (self.latLtr, self.localLtr,
                                         badPtrnStr, self.latLtr,
                                         self.localLtr))

        # Get whitelist
        if self.site.lang in self.whitelists:
            wlpage = self.whitelists[self.site.lang]
            pywikibot.output(u'Loading whitelist from %s' % wlpage)
            wlparams = {
                'action'    : 'query',
                'prop'      : 'links',
                'titles'    : wlpage,
                'redirects' : '',
                'indexpageids' : '',
            }

            data = query.GetData(wlparams, pywikibot.getSite(self.site.lang),
                                 encodeTitle=False)
            if len(data['query']['pageids']) == 1:
                pageid = data['query']['pageids'][0]
                links = data['query']['pages'][pageid]['links']
                self.knownWords = set( [n['title'] for n in links] )
            else:
                raise "The number of pageids is not 1"
            pywikibot.output(u'Loaded whitelist with %i items'
                             % len(self.knownWords))
            if pywikibot.verbose and len(self.knownWords) > 0:
                pywikibot.output(
                    u'Whitelist: [[%s]]' % u']], [['.join(self.knownWords))
        else:
            pywikibot.output(u'Whitelist is not known for language %s'
                             % self.site.lang)
            self.knownWords = set()

    def Run(self):
        try:
            count = 0
            lastLetter = ''
            for namespace in self.namespaces:
                self.params['gapnamespace'] = namespace
                title = None

                while True:
                    # Get data
                    self.params['gapfrom'] = self.apfrom
                    data = query.GetData(self.params, self.site)
                    try:
                        self.apfrom = data['query-continue'] \
                                          ['allpages']['gapfrom']
                    except:
                        self.apfrom = None

                    # Process received data
                    if 'query' in data and 'pages' in data['query']:
                        firstItem = True
                        for pageID, page in data['query']['pages'].iteritems():
                            printed = False
                            title = page['title']
                            if firstItem:
                                if lastLetter != title[0]:
                                    try:
                                        print 'Processing %s' % title
                                    except:
                                        print 'Processing unprintable title'
                                    lastLetter = title[0]
                                firstItem = False
                            if self.titles:
                                err = self.ProcessTitle(title)
                                if err:
                                    changed = False
                                    if self.replace:
                                        newTitle = self.PickTarget(False,
                                                                   title,
                                                                   title,
                                                                   err[1])
                                        if newTitle:
                                            editSummary = pywikibot.translate(
                                                self.site, self.msgRename)
                                            src = pywikibot.Page(self.site,
                                                                 title)
                                            if page['ns'] == 14:
                                                import category
                                                dst = pywikibot.Page(self.site,
                                                                     newTitle)
                                                bot = category.CategoryMoveRobot(
                                                    src.titleWithoutNamespace(),
                                                    dst.titleWithoutNamespace(),
                                                    self.autonomous,
                                                    editSummary, True)
                                                bot.run()
                                            else:
                                                src.move(newTitle, editSummary)
                                            changed = True

                                    if not changed:
                                        self.WikiLog(u"* " + err[0])
                                        printed = True

                            if self.links:
                                allLinks = None
                                if 'links' in page:
                                    allLinks = page['links']
                                if 'categories' in page:
                                    if allLinks:
                                        allLinks = allLinks + page['categories']
                                    else:
                                        allLinks = page['categories']

                                if allLinks:
                                    pageObj = None
                                    pageTxt = None
                                    msg = []

                                    for l in allLinks:
                                        ltxt = l['title']
                                        err = self.ProcessTitle(ltxt)
                                        if err:
                                            newTitle = None
                                            if self.replace:
                                                newTitle = self.PickTarget(
                                                    True, title, ltxt, err[1])
                                                if newTitle:
                                                    if pageObj is None:
                                                        pageObj = pywikibot.Page(
                                                            self.site, title)
                                                        pageTxt = pageObj.get()
                                                    msg.append(
                                                        u'[[%s]] => [[%s]]'
                                                        % (ltxt, newTitle))
#                                                    pageTxt = pageTxt.replace(ltxt, newTitle)
#                                                    pageTxt = pageTxt.replace(ltxt[0].lower() + ltxt[1:], newTitle[0].lower() + newTitle[1:])
#                                                    pageTxt = pageTxt.replace(ltxt.replace(u' ', '_'), newTitle)

                                                    frmParts = self.wordBreaker.split(
                                                        ltxt)
                                                    toParts = self.wordBreaker.split(
                                                        newTitle)
                                                    if len(frmParts) != len(
                                                        toParts):
                                                        raise ValueError(
u'Splitting parts do not match counts')
                                                    for i in range(0,
                                                                   len(frmParts)):
                                                        if len(frmParts[i]) != len(
                                                            toParts[i]):
                                                            raise ValueError(
u'Splitting parts do not match word length')
                                                        if len(frmParts[i]) > 0:
                                                            pageTxt = pageTxt.replace(
                                                                frmParts[i], toParts[i])
                                                            pageTxt = pageTxt.replace(
                                                                frmParts[i][0].lower() + frmParts[i][1:],
                                                                toParts[i][0].lower() + toParts[i][1:])

                                            if not newTitle:
                                                if not printed:
                                                    self.WikiLog(
u"* [[:%s]]: link to %s" % (title, err[0]))
                                                    printed = True
                                                else:
                                                    self.WikiLog(
                                                        u"** link to %s"
                                                        % err[0])


                                    if pageObj is not None:
                                        coloredMsg = u', '.join(
                                            [self.ColorCodeWord(m)
                                             for m in msg])
                                        if pageObj.get() == pageTxt:
                                            self.WikiLog(
u"* Error: Text replacement failed in [[:%s]] (%s)" % (title, coloredMsg))
                                        else:
                                            pywikibot.output(
                                                u'Case Replacements: %s'
                                                % u', '.join(msg))
                                            try:
                                                pageObj.put(
                                                    pageTxt,
                                                    u'%s: %s'
                                                    % (pywikibot.translate(
                                                        self.site,
                                                        self.msgLinkReplacement),
                                                       u', '.join(msg)))
                                            except KeyboardInterrupt:
                                                raise
                                            except:
                                                self.WikiLog(
u"* Error: Could not save updated page [[:%s]] (%s)" % (title, coloredMsg))


                            count += 1
                            if self.stopAfter > 0 and count == self.stopAfter:
                                raise "Stopping because we are done"

                    if self.apfrom is None:
                        break

                self.apfrom = u''    # Restart apfrom for other namespaces

            print "***************************** Done"

        except:
            if self.apfrom is not None:
                pywikibot.output(u'Exception at Title = %s, Next = %s' % (title, self.apfrom))
            raise

    def WikiLog(self, text):
        pywikibot.output(text)
        self.wikilog.write(text + u'\n')
        self.wikilog.flush()

    def ProcessTitle(self, title):

        found = False
        for m in self.badWordPtrn.finditer(title):

            badWord = title[m.span()[0] : m.span()[1]]
            if badWord in self.knownWords:
                continue

            # Allow any roman numerals with local suffixes
            if self.romanNumSfxPtrn.match(badWord) is not None:
                continue

            if not found:
                # lazy-initialization of the local variables
                possibleWords = []
                tempWords = []
                count = 0
                duplWordCount = 0
                ambigBadWords = set()
                ambigBadWordsCount = 0
                mapLcl = {}
                mapLat = {}
                found = True

            # See if it would make sense to treat the whole word as either
            # cyrilic or latin
            mightBeLat = mightBeLcl = True
            for l in badWord:
                if l in self.localLtr:
                    if mightBeLat and l not in self.localSuspects:
                        mightBeLat = False
                else:
                    if mightBeLcl and l not in self.latinSuspects:
                        mightBeLcl = False
                    if l not in self.latLtr: raise "Assert failed"

            # Some words are well known and frequently mixed-typed
            if mightBeLcl and mightBeLat:
                if badWord in self.alwaysInLocal:
                    mightBeLat = False
                elif badWord in self.alwaysInLatin:
                    mightBeLoc = False

            if mightBeLcl:
                mapLcl[badWord] = badWord.translate(self.latToLclDict)
            if mightBeLat:
                mapLat[badWord] = badWord.translate(self.lclToLatDict)
            if mightBeLcl and mightBeLat:
                ambigBadWords.add(badWord)
                # Cannot do len(ambigBadWords) because they might be duplicates
                ambigBadWordsCount += 1
            count += 1

        if not found:
            return None

        infoText = self.MakeLink(title)
        possibleAlternatives = []

        if len(mapLcl) + len(mapLat) - ambigBadWordsCount < count:
            # We cannot auto-translate - offer a list of suggested words
            suggestions = mapLcl.values() + mapLat.values()
            if len(suggestions) > 0:
                infoText += u", word suggestions: " + u', '.join(
                    [self.ColorCodeWord(t) for t in suggestions])
            else:
                infoText += u", no suggestions"
        else:

            # Replace all unambiguous bad words
            for k,v in mapLat.items() + mapLcl.items():
                if k not in ambigBadWords:
                    title = title.replace(k,v)

            if len(ambigBadWords) == 0:
                # There are no ambiguity, we can safelly convert
                possibleAlternatives.append(title)
                infoText += u", convert to " + self.MakeLink(title)
            else:
                # Try to pick 0, 1, 2, ..., len(ambiguous words) unique
                # combinations from the bad words list, and convert just the
                # picked words to cyrilic, whereas making all other words as
                # latin character.
                for itemCntToPick in range(0, len(ambigBadWords)+1):
                    title2 = title
                    for uc in xuniqueCombinations(list(ambigBadWords),
                                                  itemCntToPick):
                        wordsToLat = ambigBadWords.copy()
                        for bw in uc:
                            title2 = title2.replace(bw, mapLcl[bw])
                            wordsToLat.remove(bw)
                        for bw in wordsToLat:
                            title2 = title2.replace(bw, mapLat[bw])
                        possibleAlternatives.append(title2)

                if len(possibleAlternatives) > 0:
                    infoText += u", can be converted to " + u', '.join(
                        [self.MakeLink(t) for t in possibleAlternatives])
                else:
                    infoText += u", no suggestions"

        return (infoText, possibleAlternatives)

    def PickTarget(self, isLink, title, original, candidates):
        if len(candidates) == 0:
            return None

        if isLink:
            if len(candidates) == 1:
                return candidates[0]

            pagesDontExist = []
            pagesRedir = {}
            pagesExist = []

            for newTitle in candidates:
                dst = pywikibot.Page(self.site, newTitle)
                if not dst.exists():
                    pagesDontExist.append(newTitle)
                elif dst.isRedirectPage():
                    pagesRedir[newTitle] = dst.getRedirectTarget().title()
                else:
                    pagesExist.append(newTitle)

            if len(pagesExist) == 1:
                return pagesExist[0]
            elif len(pagesExist) == 0 and len(pagesRedir) > 0:
                if len(pagesRedir) == 1:
                    return pagesRedir.keys()[0]
                t = None
                for k,v in pagesRedir.iteritems():
                    if not t:
                        t = v # first item
                    elif t != v:
                        break
                else:
                    # all redirects point to the same target
                    # pick the first one, doesn't matter what it is
                    return pagesRedir.keys()[0]

            if not self.autonomous:
                pywikibot.output(u'Could not auto-decide for page [[%s]]. Which link should be chosen?' % title)
                pywikibot.output(u'Original title: ', newline=False)
                self.ColorCodeWord(original + "\n", True)
                count = 1
                for t in candidates:
                    if t in pagesDontExist: msg = u'missing'
                    elif t in pagesRedir: msg = u'Redirect to ' + pagesRedir[t]
                    else: msg = u'page exists'
                    self.ColorCodeWord(u'  %d: %s (%s)\n'
                                       % (count, t, msg), True)
                    count += 1

                answers = [str(i) for i in range(0, count)]
                choice = int(pywikibot.inputChoice(
                    u'Which link to choose? (0 to skip)',
                    answers, [a[0] for a in answers]))
                if choice > 0:
                    return candidates[choice-1]

        else:
            if len(candidates) == 1:
                newTitle = candidates[0]
                dst = pywikibot.Page(self.site, newTitle)
                if not dst.exists():
                    # choice = pywikibot.inputChoice(u'Move %s to %s?' % (title, newTitle), ['Yes', 'No'], ['y', 'n'])
                    return newTitle

        return None

    def ColorCodeWord(self, word, toScreen = False):

        if not toScreen: res = u"<b>"
        lastIsCyr = word[0] in self.localLtr
        if lastIsCyr:
            if toScreen: SetColor(FOREGROUND_GREEN)
            else: res += self.lclClrFnt
        else:
            if toScreen: SetColor(FOREGROUND_RED)
            else: res += self.latClrFnt

        for l in word:
            if l in self.localLtr:
                if not lastIsCyr:
                    if toScreen: SetColor(FOREGROUND_GREEN)
                    else: res += self.suffixClr + self.lclClrFnt
                    lastIsCyr = True
            elif l in self.latLtr:
                if lastIsCyr:
                    if toScreen: SetColor(FOREGROUND_RED)
                    else: res += self.suffixClr + self.latClrFnt
                    lastIsCyr = False
            if toScreen: pywikibot.output(l, newline=False)
            else: res += l

        if toScreen: SetColor(FOREGROUND_WHITE)
        else: return res + self.suffixClr + u"</b>"


    def MakeLink(self, title):
        return u"[[:%s|««« %s »»»]]" % (title, self.ColorCodeWord(title))

if __name__ == "__main__":
    try:
        bot = CaseChecker()
        bot.Run()
    finally:
        pywikibot.stopme()
