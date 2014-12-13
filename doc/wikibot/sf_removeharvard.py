#!/usr/bin/python
# -*- coding: utf-8  -*-

import sys, re, os.path
import wikipedia
import pagegenerators, catlib, weblinkchecker, upload

HARVARDTAG = "<HarvardReferences"

class IgnorePage(wikipedia.Error):
    pass

class SFRobot:
    def __init__(self, generator):
        self.generator = generator
        self.count = 0

    def run(self):
        for page in self.generator:
            try:
                self.count += 1
                text = page.get()
                textlines = text.splitlines()
                hasHarvard = False
                references = dict()

                # get harvard reference entries
                for line in reversed(textlines):
                    line = line.strip()

                    if not hasHarvard:
                        if line.startswith(HARVARDTAG):
                            hasHarvard = True
                        continue

                    # hasHarvard
                    if line.startswith("*"):
                        # line is reference
                        key, reftext = line.split("[*",1)[1].split("]",1)
                        key, reftext = key.strip(), reftext.strip()
                        if len(key) > 0 and len(reftext) > 0:
                            references[key] = reftext
                            textlines.remove(line)
                    elif line.startswith("="):
                        break # done

                if not hasHarvard: # not relevant pages
                    raise IgnorePage

                newtext = "\n".join(textlines).replace(HARVARDTAG, u"<references")
#                print "===========\n",page.title()

#                print references
                for key, reftext in references.items():
                    keylst = key.split()
                    textkey = " ".join(keylst[0:-1])+' ('+keylst[-1]+')'
                    refkey = key.replace(' ','')
                    first = u'{0}<ref name="{1}">{2}</ref>'\
                            .format(textkey,refkey,reftext)
                    multi = u'{0}<ref name="{1}"/>'\
                            .format(textkey,refkey)
                    pattern = r'\[{0}(:.+)?\]'.format(' +'.join(keylst))
#                    print pattern
                    newtext,n = re.subn(pattern, first, newtext, count=1)
                    if n > 0:
                        newtext,n = re.subn(pattern, multi, newtext, count=0)
#                    print newtext
#                    print [first, multi]

                page.put(newtext, comment="removed HarvardReferences")
#                print "Press enter to continue"
#                sys.stdin.readline()

            except IgnorePage:
                print "Skipping", page.title()
            except wikipedia.NoPage:
                print "Page %s does not exist?!" % page.aslink()
            except wikipedia.IsRedirectPage:
                print "Page %s is a redirect; skipping." % page.aslink()
            except wikipedia.LockedPage:
                print "Page %s is locked?!" % page.aslink()

        print "done:", self.count, "pages"

def main():

    site = wikipedia.getSite()
    gen = site.allpages()
    preloadingGen = pagegenerators.PreloadingGenerator(gen)
    bot = SFRobot(preloadingGen)
    bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()
# vim: set ts=4 sw=4 tw=0: 
