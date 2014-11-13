#!/usr/bin/python
# -*- coding: UTF-8 -*-
"""
This bot spellchecks Wikipedia pages. It is very simple, only checking
whether a word, stripped to its 'essence' is in the list or not, it does
not do any grammar checking or such. It can be used in four ways:

spellcheck.py Title
    Check a single page; after this the bot will ask whether you want to
    check another page
spellcheck.py -start:Title
    Go through the wiki, starting at title 'Title'.
spellcheck.py -newpages
    Go through the pages on [[Special:Newpages]]
spellcheck.py -longpages
    Go through the pages on [[Special:Longpages]]

For each unknown word, you get a couple of options:
    numbered options: replace by known alternatives
    a: This word is correct; add it to the list of known words
    c: The uncapitalized form of this word is correct; add it
    i: Do not edit this word, but do also not add it to the list
    p: Do not edit this word, and consider it correct for this page only
    r: Replace the word, and add the replacement as a known alternative
    s: Replace the word, but do not add the replacement
    *: Edit the page using the gui
    g: Give a list of 'guessed' words, which are similar to the given one
    x: Ignore this word, and do not check the rest of the page

When the bot is ended, it will save the extensions to its word list;
there is one word list for each language.

The bot does not rely on Latin script, but does rely on Latin punctuation.
It is therefore expected to work on for example Russian and Korean, but not
on for example Japanese.

Command-line options:
-html           change HTML-entities like &uuml; into their respective letters.
               This is done both before and after the normal check.
-rebuild       save the complete wordlist, not just the changes, removing the
               old wordlist.
-noname        skip all words that start with a capital
-checklang:xx  use the file for language xx: instead of that for my local
               language; for example on simple: one would use the language
               file of en:
-knownonly     only check words that have been marked as a mis-spelling in
               the spelling list; words that are not on the list are skipped
-knownplus     finds words in the same way as knownonly, but once a word to
               be changed has been found, also goes through the rest of the
               page.
"""
#
# (C) Andre Engels, 2005
# (C) Pywikipedia bot team, 2006-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: spellcheck.py 9153 2011-04-09 06:29:19Z a_engels $'
#

import re, sys
import string, codecs
import wikipedia as pywikibot
import pagegenerators

msg={
    'ar':u'تدقيق إملائي بمساعدة البوت',
    'en':u'Bot-aided spell checker',
    'es':u'Bot asistido de correción ortográfica',
    'fr':u'Correction orthographique par robot',
    'he':u'בדיקת איות באמצעות בוט',
    'ia':u'Correction de orthographia per robot',
    'nl':u'Spellingscontrole',
    'pl':u'Wspomagane przez robota sprawdzanie pisowni',
    'pt':u'Bot de correção ortográfica',
}


class SpecialTerm(object):
    def __init__(self, text):
        self.style = text


def distance(a,b):
    # Calculates the Levenshtein distance between a and b.
    # That is, the number of edits needed to change one into
    # the other, where one edit is the addition, removal or
    # change of a single character.
    # Copied from Magnus Lie Hetland at http://hetland.org/python/
    n, m = len(a), len(b)
    if n > m:
        # Make sure n <= m, to use O(min(n,m)) space
        a,b = b,a
        n,m = m,n
    current = range(n+1)
    for i in range(1,m+1):
        previous, current = current, [i]+[0]*m
        for j in range(1,n+1):
            add, delete = previous[j]+1, current[j-1]+1
            change = previous[j-1]
            if a[j-1] != b[i-1]:
                change = change + 1
            current[j] = min(add, delete, change)
    return current[n]

def getalternatives(string):
    # Find possible correct words for the incorrect word string
    basetext = pywikibot.input(
        u"Give a text that should occur in the words to be checked.\nYou can choose to give no text, but this will make searching slow:")
    basetext = basetext.lower()
    simwords = {}
    for i in xrange(11):
        simwords[i] = []
    for alt in knownwords.keys():
        dothis = not basetext or basetext in alt.lower()
        if dothis:
            diff = distance(string,alt)
            if diff < 11:
                if knownwords[alt] == alt:
                    simwords[diff] += [alt]
                else:
                    simwords[diff] += knownwords[alt]
    posswords = []
    for i in xrange(11):
        if not simwords[i] in posswords:
            posswords += simwords[i]
    return posswords[:30]

def uncap(string):
    # uncapitalize the first word of the string
    if len(string) > 1:
        return string[0].lower()+string[1:]
    else:
        return string.lower()

def cap(string):
    # uncapitalize the first word of the string
    return string[0].upper()+string[1:]

def askAlternative(word,context=None):
    correct = None
    pywikibot.output(u"="*60)
    pywikibot.output(u"Found unknown word '%s'"%word)
    if context:
        pywikibot.output(u"Context:")
        pywikibot.output(u""+context)
        pywikibot.output(u"-"*60)
    while not correct:
        for i in xrange(len(Word(word).getAlternatives())):
            pywikibot.output(u"%s: Replace by '%s'"
                             % (i+1,
                                Word(word).getAlternatives()[i].replace('_',' ')))
        pywikibot.output(u"a: Add '%s' as correct"%word)
        if word[0].isupper():
            pywikibot.output(u"c: Add '%s' as correct" % (uncap(word)))
        pywikibot.output(u"i: Ignore once (default)")
        pywikibot.output(u"p: Ignore on this page")
        pywikibot.output(u"r: Replace text")
        pywikibot.output(u"s: Replace text, but do not save as alternative")
        pywikibot.output(u"g: Guess (give me a list of similar words)")
        pywikibot.output(u"*: Edit by hand")
        pywikibot.output(u"x: Do not check the rest of this page")
        answer = pywikibot.input(u":")
        if answer == "": answer = "i"
        if answer in "aAiIpP":
            correct = word
            if answer in "aA":
                knownwords[word] = word
                newwords.append(word)
            elif answer in "pP":
                pageskip.append(word)
        elif answer in "rRsS":
            correct = pywikibot.input(u"What should I replace it by?")
            if answer in "rR":
                if correct_html_codes:
                    correct = removeHTML(correct)
                if correct != cap(word) and \
                   correct != uncap(word) and \
                   correct != word:
                    try:
                        knownwords[word] += [correct.replace(' ','_')]
                    except KeyError:
                        knownwords[word] = [correct.replace(' ','_')]
                    newwords.append(word)
                knownwords[correct] = correct
                newwords.append(correct)
        elif answer in "cC" and word[0].isupper():
            correct = word
            knownwords[uncap(word)] = uncap(word)
            newwords.append(uncap(word))
        elif answer in "gG":
            possible = getalternatives(word)
            if possible:
                print "Found alternatives:"
                for pos in possible:
                    pywikibot.output("  %s"%pos)
            else:
                print "No similar words found."
        elif answer=="*":
            correct = edit
        elif answer=="x":
            correct = endpage
        else:
            for i in xrange(len(Word(word).getAlternatives())):
                if answer == str(i+1):
                    correct = Word(word).getAlternatives()[i].replace('_',' ')
    return correct

def removeHTML(page):
    # TODO: Consider removing this; this stuff can be done by
    # cosmetic_changes.py
    result = page
    result = result.replace('&Auml;',u'Ä')
    result = result.replace('&auml;',u'ä')
    result = result.replace('&Euml;',u'Ë')
    result = result.replace('&euml;',u'ë')
    result = result.replace('&Iuml;',u'Ï')
    result = result.replace('&iuml;',u'ï')
    result = result.replace('&Ouml;',u'Ö')
    result = result.replace('&ouml;',u'ö')
    result = result.replace('&Uuml;',u'Ü')
    result = result.replace('&uuml;',u'ü')
    result = result.replace('&Aacute;',u'Á')
    result = result.replace('&aacute;',u'á')
    result = result.replace('&Eacute;',u'É')
    result = result.replace('&eacute;',u'é')
    result = result.replace('&Iacute;',u'Í')
    result = result.replace('&iacute;',u'í')
    result = result.replace('&Oacute;',u'Ó')
    result = result.replace('&oacute;',u'ó')
    result = result.replace('&Uacute;',u'Ú')
    result = result.replace('&uacute;',u'ú')
    result = result.replace('&Agrave;',u'À')
    result = result.replace('&agrave;',u'à')
    result = result.replace('&Egrave;',u'È')
    result = result.replace('&egrave;',u'è')
    result = result.replace('&Igrave;',u'Ì')
    result = result.replace('&igrave;',u'ì')
    result = result.replace('&Ograve;',u'Ò')
    result = result.replace('&ograve;',u'ò')
    result = result.replace('&Ugrave;',u'Ù')
    result = result.replace('&ugrave;',u'ù')
    result = result.replace('&Acirc;',u'Â')
    result = result.replace('&acirc;',u'â')
    result = result.replace('&Ecirc;',u'Ê')
    result = result.replace('&ecirc;',u'ê')
    result = result.replace('&Icirc;',u'Î')
    result = result.replace('&icirc;',u'î')
    result = result.replace('&Ocirc;',u'Ô')
    result = result.replace('&ocirc;',u'ô')
    result = result.replace('&Ucirc;',u'Û')
    result = result.replace('&ucirc;',u'û')
    result = result.replace('&Aring;',u'Å')
    result = result.replace('&aring;',u'å')
    result = result.replace('&deg;',u'°')
    return result

def spellcheck(page, checknames = True, knownonly = False):
    pageskip = []
    text = page
    if correct_html_codes:
        text = removeHTML(text)
    loc = 0
    while True:
        wordsearch = re.compile(r'([\s\=\<\>\_]*)([^\s\=\<\>\_]+)')
        match = wordsearch.search(text,loc)
        if not match:
            # No more words on this page
            break
        loc += len(match.group(1))
        bigword = Word(match.group(2))
        smallword = bigword.derive()
        if not Word(smallword).isCorrect(checkalternative = knownonly) and \
           (checknames or not smallword[0].isupper()):
            replacement = askAlternative(smallword,
                                         context=text[max(0,loc-40):loc + len(match.group(2))+40])
            if replacement == edit:
                import editarticle
                editor = editarticle.TextEditor()
                # TODO: Don't know to which index to jump
                newtxt = editor.edit(text, jumpIndex = 0, highlight=smallword)
                if newtxt:
                    text = newtxt
            elif replacement == endpage:
                loc = len(text)
            else:
                replacement = bigword.replace(replacement)
                text = text[:loc] + replacement + text[loc+len(match.group(2)):]
                loc += len(replacement)
            if knownonly == 'plus' and text != page:
                knownonly = False
                loc = 0
        else:
            loc += len(match.group(2))
    if correct_html_codes:
        text = removeHTML(text)
    pageskip = []
    return text


class Word(object):
    def __init__(self,text):
        self.word = text

    def __str__(self):
        return self.word

    def __cmp__(self,other):
        return self.word.__cmp__(str(other))

    def derive(self):
        # Get the short form of the word, without punctuation, square
        # brackets etcetera
        shortword = self.word
        # Remove all words of the form [[something:something - these are
        # usually interwiki links or category links
        if shortword.rfind(':') != -1:
            if -1 < shortword.rfind('[[') < shortword.rfind(':'):
                shortword = ""
        # Remove barred links
        if shortword.rfind('|') != -1:
            if -1 < shortword.rfind('[[') < shortword.rfind('|'):
                shortword = shortword[:shortword.rfind('[[')] + shortword[shortword.rfind('|')+1:]
            else:
                shortword = shortword[shortword.rfind('|')+1:]
        shortword = shortword.replace('[','')
        shortword = shortword.replace(']','')
        # Remove non-alphanumerical characters at the start
        try:
            while shortword[0] in string.punctuation:
                shortword=shortword[1:]
        except IndexError:
            return ""
        # Remove non-alphanumerical characters at the end; no need for the
        # try here because if things go wrong here, they should have gone
        # wrong before
        while shortword[-1] in string.punctuation:
            shortword=shortword[:-1]
        # Do not check URLs
        if shortword.startswith("http://"):
            shortword=""
        # Do not check 'words' with only numerical characters
        number = True
        for i in xrange(len(shortword)):
            if not (shortword[i] in string.punctuation or shortword[i] in string.digits):
                number = False
        if number:
            shortword = ""
        return shortword

    def replace(self,rep):
        # Replace the short form by 'rep'. Keeping simple for now - if the
        # short form is part of the long form, replace it. If it is not, ask
        # the user
        if rep == self.derive():
            return self.word
        if self.derive() not in self.word:
            return pywikibot.input(
                u"Please give the result of replacing %s by %s in %s:"
                % (self.derive(), rep, self.word))
        return self.word.replace(self.derive(),rep)

    def isCorrect(self,checkalternative = False):
        # If checkalternative is True, the word will only be found incorrect if
        # it is on the spelling list as a spelling error. Otherwise it will
        # be found incorrect if it is not on the list as a correctly spelled
        # word.
        if self.word == "":
            return True
        if self.word in pageskip:
            return True
        try:
            if knownwords[self.word] == self.word:
                return True
            else:
                return False
        except KeyError:
            pass
        if self.word != uncap(self.word):
            return Word(uncap(self.word)).isCorrect(checkalternative=checkalternative)
        else:
            if checkalternative:
                if checklang == 'nl' and self.word.endswith("'s"):
                    # often these are incorrect (English-style) possessives
                    return False
                if self.word != cap(self.word):
                    if Word(cap(self.word)).isCorrect():
                        return False
                    else:
                        return True
                else:
                    return True
            else:
                return False

    def getAlternatives(self):
        alts = []
        if self.word[0].islower():
            if Word(cap(self.word)).isCorrect():
                alts = [cap(self.word)]
        try:
            alts += knownwords[self.word]
        except KeyError:
            pass
        if self.word[0].isupper():
            try:
                alts += [cap(w) for w in knownwords[uncap(self.word)]]
            except KeyError:
                pass
        return alts

    def declare_correct(self):
        knownwords[self.word] = self.word

    def declare_alternative(self,alt):
        if not alt in knownwords[self.word]:
            knownwords[self.word].append(word)
            newwords.append(self.word)
        return self.alternatives

try:
    pageskip = []
    edit = SpecialTerm("edit")
    endpage = SpecialTerm("end page")
    title = []
    knownwords = {}
    newwords = []
    start = None
    newpages = False
    longpages = False
    correct_html_codes = False
    rebuild = False
    checknames = True
    checklang = None
    knownonly = False

    for arg in pywikibot.handleArgs():
        if arg.startswith("-start:"):
            start = arg[7:]
        elif arg.startswith("-newpages"):
            newpages = True
        elif arg.startswith("-longpages"):
            longpages = True
        elif arg.startswith("-html"):
            correct_html_codes = True
        elif arg.startswith("-rebuild"):
            rebuild = True
        elif arg.startswith("-noname"):
            checknames = False
        elif arg.startswith("-checklang:"):
            checklang = arg[11:]
        elif arg.startswith("-knownonly"):
            knownonly = True
        elif arg.startswith("-knownplus"):
            knownonly = 'plus'
        else:
            title.append(arg)

    mysite = pywikibot.getSite()
    if not checklang:
        checklang = mysite.language()
    pywikibot.setAction(pywikibot.translate(mysite,msg))
    filename = pywikibot.config.datafilepath('spelling',
                                      'spelling-' + checklang + '.txt')
    print "Getting wordlist"
    try:
        f = codecs.open(filename, 'r', encoding = mysite.encoding())
        for line in f.readlines():
            # remove trailing newlines and carriage returns
            try:
                while line[-1] in ['\n', '\r']:
                    line = line[:-1]
            except IndexError:
                pass
            #skip empty lines
            if line != '':
                if line[0] == "1":
                    word = line[2:]
                    knownwords[word] = word
                else:
                    line = line.split(' ')
                    word = line[1]
                    knownwords[word] = line[2:]
                    for word2 in line[2:]:
                        if not '_' in word2:
                            knownwords[word2] = word2
        f.close()
    except IOError:
        print "Warning! There is no wordlist for your language!"
    else:
        print "Wordlist successfully loaded."
    # This is a purely interactive bot, we therefore do not want to put-throttle
    pywikibot.put_throttle.setDelay(1)
except:
    pywikibot.stopme()
    raise
try:
    if newpages:
        for (page, date, length, loggedIn, user, comment) in pywikibot.getSite().newpages(1000):
            try:
                text = page.get()
            except pywikibot.Error:
                pass
            else:
                text = spellcheck(text, checknames=checknames,
                                  knownonly=knownonly)
                if text != page.get():
                    page.put(text)
    elif start:
        for page in pagegenerators.PreloadingGenerator(pagegenerators.AllpagesPageGenerator(start=start,includeredirects=False)):
            try:
                text = page.get()
            except pywikibot.Error:
                pass
            else:
                text = spellcheck(text, checknames=checknames,
                                  knownonly=knownonly)
                if text != page.get():
                    page.put(text)

    if longpages:
        for (page, length) in pywikibot.getSite().longpages(500):
            try:
                text = page.get()
            except pywikibot.Error:
                pass
            else:
                text = spellcheck(text, checknames=checknames,
                                  knownonly=knownonly)
                if text != page.get():
                    page.put(text)

    else:
        title = ' '.join(title)
        while title != '':
            try:
                page = pywikibot.Page(mysite,title)
                text = page.get()
            except pywikibot.NoPage:
                print "Page does not exist."
            except pywikibot.IsRedirectPage:
                print "Page is a redirect page"
            else:
                text = spellcheck(text,knownonly=knownonly)
                if text != page.get():
                    page.put(text)
            title = pywikibot.input(u"Which page to check now? (enter to stop)")
finally:
    pywikibot.stopme()
    filename = pywikibot.config.datafilepath('spelling',
                                      'spelling-' + checklang + '.txt')
    if rebuild:
        list = knownwords.keys()
        list.sort()
        f = codecs.open(filename, 'w', encoding = mysite.encoding())
    else:
        list = newwords
        f = codecs.open(filename, 'a', encoding = mysite.encoding())
    for word in list:
        if Word(word).isCorrect():
            if word != uncap(word):
                if Word(uncap(word)).isCorrect():
                    # Capitalized form of a word that is in the list
                    # uncapitalized
                    continue
            f.write("1 %s\n"%word)
        else:
            f.write("0 %s %s\n"%(word," ".join(knownwords[word])))
    f.close()
