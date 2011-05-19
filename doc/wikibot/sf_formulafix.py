#!/usr/bin/python2.7
# -*- coding: utf-8  -*-

import sys, re, os.path

major, minor = sys.version_info[0:2]
if minor < 7 and major <= 2:
    print >>sys.stderr, "This script needs at least python 2.7, giving up."
    sys.exit(1)

from pywikipedia import wikipedia
import pagegenerators, catlib, weblinkchecker, upload
import difflib, pprint
import pickle

FLAGS = re.DOTALL

class IgnorePage(wikipedia.Error):
    pass

def waitForUserInput(msg):
    print >>sys.stderr, "## "+msg+"\n[<enter> to continue, any input aborts:] "
    answer = sys.stdin.readline()
    if answer != "\n":
        return False
    return True

def printmatch(m):
    print m.group(0)
#    print m.groups()
    print "==========="

def fixformula(m):
    formula = m.group()
#    print "in:  '{0}'".format(formula)
    # fix broken math newlines \ -> \\
    formula = re.sub(r'([^\\]\\)( )', r'\1\\\2', formula)
    # break code at newline
    formula = re.sub(r'([^\\]\\\\) *(\S)', r'\1\n\2', formula)
    # remove lonely \begin or \end
    if re.search(r'\\begin\{(cases|align)\}', formula, flags=FLAGS) and \
       not re.search(r'\\end\{(cases|align)\}', formula, flags=FLAGS):
        formula = re.sub(r'(\s*\\begin\{(cases|align)\}\s*)', '', formula, flags=FLAGS)
    if re.search(r'\\end\{(cases|align)\}', formula, flags=FLAGS) and \
       not re.search(r'\\begin\{(cases|align)\}', formula, flags=FLAGS):
        formula = re.sub(r'(\s*\\end\{(cases|align)\}\s*)', '', formula, flags=FLAGS)
    if not re.search(r'\\begin', formula, flags=FLAGS):
        formula = formula.replace('&','')
        formula = formula.replace('\\\\','')
        formula = re.sub(r'\s*\[[0-9]+mm\]\s*', '', formula, flags=FLAGS)
    # break line after \begin or before \end statements
    formula = re.sub(r'(\\begin\{[^}]+\}) *(\S)', r'\1\n\2', formula)
    formula = re.sub(r'(\S) *(\\end\{[^}]+\})', r'\1\n\2', formula)
    # break formulae block into single formulae (separate references)
    def separate(m):
        g = m.group(1)
        g = re.sub(r'&(.*\\begin\{(cases|align)\})',r'\1',g, flags=FLAGS)
        result = u"</math>|dummy}}\n{{eqn|<math>"+g
        if not re.search(r'\\begin\{(cases|align)\}', result, flags=FLAGS):
            result = result.replace('&','')
#        print "------"
#        print result
        return result
    # USE WITH CARE
#    formula = re.sub(r'\\\\\s+((.(?!\\\\))+?=(.(?!\\\\))+?[^\\])', 
#                     separate, formula, flags=FLAGS)
#    print "out: '{0}'".format(formula)
    return formula

mathopentag = u"<math>"
mathclosetag = u"</math>"

def getTemplateCall(text, templateName, start=0):
    start = text.find(templateName+"|", start)
    if start < 0:
        return None
    end = text.find("}}", start)
    # see if we have a formula in between and ignore that
    mathstart = mathend = text.find(mathopentag, start)
    if mathstart >= 0 and mathstart < end:
        # refine mathstart
        mathstart = text.rfind("|", start, mathstart)+1
        mathend = text.find(mathclosetag, start)
        if mathstart <= 0 or mathend < 0: # no closing tag ?
            return None
        end = text.find("}}", mathend)
        pos = text.find("|", mathend, end) # refine
        if pos >= 0: mathend = pos

    if end < 0:
        return None
    snippet = text[start:end]
    # exclude/remove math formula
    hasMath = (mathstart >= 0 and mathstart != mathend)
    if hasMath:
        snippet = text[start:mathstart]+text[mathend:end]
    # separate template parameters
    lst = snippet.split("|")
    if hasMath:
        lst[1] = text[mathstart:mathend]
    return start, end, lst

class SFRobot:
    def __init__(self):
        self.count = 0
        self.formulaIds = dict() # for tracking math formulae references ids

    def setGenerator(self, generator):
        self.generator = generator

    def updateFids(self, pagetitle, id, newid, force=False):
        formulaId = (pagetitle,id)
        if formulaId not in self.formulaIds or force:
            self.formulaIds[formulaId] = newid
            # store data in case of script abort
            with open("/tmp/formulaeids",'w') as fd:
                pickle.dump(self.formulaIds, fd)
        return self.formulaIds[formulaId]

    def fixReferences(self, page, pagetext):
        if page is None or \
           pagetext is None or len(pagetext) <= 0:
            return pagetext
#        print " # fixReferences"

        refend = 0
        while True: # until no more eqnrefs found
            temp = getTemplateCall(pagetext, "eqnref", refend)
            if temp is None or len(temp) < 3:
                break
            refstart, refend, lst = temp
            print lst
            pagetitle = page.title() # reset to this page
            if len(lst) < 2:
                print >>sys.stderr, "Errorneous 'eqnref' stmt '{0}' in '{1}'"\
                        .format(pagetext[refstart:refend],pagetitle)
                continue

            id = lst[1]
            isCrossRef = (len(lst) > 2)
            if isCrossRef: # get pagetitle for this formula if external
                pagetitle = lst[2]
            formulaId = (pagetitle,id)
            # init with old ID, otherwise it's already updated
            storedId = self.updateFids(pagetitle, id, id)
            if storedId != id:
#                print "fid replace"
                # updated already, fix eqnref
                eqnref = u"eqnref|{0}".format(storedId)
                if isCrossRef:
                    eqnref = u"eqnref|{0}|{1}".format(storedId, pagetitle)
                # update current eqnref in page
                pagetext = pagetext[:refstart]+eqnref+pagetext[refend:]

        return pagetext

    def numberFormulae(self, page, pagetext):
        if pagetext is None or len(pagetext) <= 0:
            return pagetext
#        print " # numberFormulae"
        num = 1
        eqnend = 0
        while True:
            temp = getTemplateCall(pagetext, "eqn", eqnend)
            if temp is None or len(temp) < 3:
                break
            eqnstart, eqnend, lst = temp
            if len(lst) < 3: # no reference number
                continue
#            if len(lst) > 3:
#                with open("/tmp/test","a") as fd:
#                    print >>fd, lst[2:]
#            print lst
            lst[2] = self.updateFids(page.title(), lst[2], str(num), force=True)
            num += 1
            # update page
            eqn = "|".join(lst)
            pagetext = pagetext[:eqnstart]+eqn+pagetext[eqnend:]

        return pagetext

    def run(self):
        found = False
        if not waitForUserInput("start?"): 
            return
        for page in self.generator:
            try:
                pagetitle = page.title()
                print "=>", pagetitle
                newtext = text = page.get()

                # sanitize all formulae
                for i in range(0,2): # process each formula twice
                    newtext = re.sub(r'<math>(.+?)</math>', 
                                     fixformula, newtext, flags=FLAGS)

                newtext = self.numberFormulae(page, newtext)
                newtext = self.fixReferences(page, newtext)

                # update if appropriate
                if newtext != text:
                    print "".join(list(difflib.unified_diff(text.splitlines(1),newtext.splitlines(1),n=0)))
#                    print "================"
                    # edit
                    page.put(newtext, comment="formulae renumbering")
                    self.count += 1

            except IgnorePage:
                print "Skipping."
            except wikipedia.NoPage:
                print "Page %s does not exist?!" % page.aslink()
            except wikipedia.IsRedirectPage:
                print "Page %s is a redirect; skipping." % page.aslink()
            except wikipedia.LockedPage:
                print "Page %s is locked?!" % page.aslink()
            else:
                # one for testing
                if self.count > 5:
                    if not waitForUserInput("continue?"):
                        return
                    self.count = 0
                pass

        print "done:", self.count, "pages"

def main():

    site = wikipedia.getSite()
    bot = SFRobot()
    gen = None

    def allPagesGenerator(site):
        gen = site.allpages()
        gen = pagegenerators.PreloadingGenerator(gen)
        return gen

    def singlePageGenerator(site):
        page = wikipedia.Page(site, u"Fitting")
        gen = iter([page])
        return gen

    if 0:
        bot.setGenerator(singlePageGenerator(site))
        bot.run()
    else:
        bot.setGenerator(allPagesGenerator(site))
        bot.run()
        # run twice, fixing cross references
        bot.setGenerator(allPagesGenerator(site))
        bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()

# vim: set ts=4 sw=4 tw=0:

