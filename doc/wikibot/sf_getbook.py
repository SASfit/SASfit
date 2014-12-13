#!/usr/bin/python
# -*- coding: utf-8  -*-

import sys, re, os.path
import wikipedia, pagegenerators, catlib, weblinkchecker, upload
import StringIO

output = StringIO.StringIO()
shortforms = {
    'Form Factors': 'FF',
    'Structure Factors': 'SQ',
    'Size Distributions': 'SD',
}

# recursively parse a category tree
def processCategory(cat, prefix=''):
    if not cat.isCategory():
        return
    # output current cat title
    title = cat.title()
    title = title[title.index(':')+1:]
    if title != 'Index':
        print >>output, ';'+prefix+title
    # output all articles within
    for a in cat.articles():
        print >>output, ':[['+a.title()+']]'
    # generate prefix for sub cats
    if title in shortforms:
        prefix = shortforms[title] + ' - '
    # continue with sub cats
    for c in cat.subcategories():
        processCategory(c, prefix)

class SFRobot:
    def __init__(self, generator):
        self.generator = generator

    def run(self):
        for page in self.generator:
            try:
                if not page.isCategory():
                    continue
                processCategory(page)

            except wikipedia.NoPage:
                print "Page %s does not exist?!" % page.aslink()
            except wikipedia.IsRedirectPage:
                print "Page %s is a redirect; skipping." % page.aslink()
            except wikipedia.LockedPage:
                print "Page %s is locked?!" % page.aslink()

def main():
    #page generator
    gen = None
    # If the user chooses to work on a single page, this temporary array is
    # used to read the words from the page title. The words will later be
    # joined with spaces to retrieve the full title.
    pageTitle = ['Category:Index']
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()

    if pageTitle:
        # work on a single page
        page = catlib.Category(wikipedia.getSite(), ' '.join(pageTitle))
        gen = iter([page])

    preloadingGen = pagegenerators.PreloadingGenerator(gen)
    bot = SFRobot(preloadingGen)
    bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()

print output.getvalue()
#output.close()

# vim: set ts=4 sw=4 tw=0: 
