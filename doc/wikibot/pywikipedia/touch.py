#!/usr/bin/python
# -*- coding: utf-8  -*-

"""
This bot goes over multiple pages of a wiki, and edits them without
changing. This is for example used to get category links in templates
working.

This script understands various command-line arguments:

&params;

-redir            specifies that the robot should touch redirect pages;
                  otherwise, they will be skipped.

All other parameters will be regarded as a page title; in this case, the bot
will only touch a single page.
"""

#
# (C) Pywikipedia team
#
__version__='$Id: touch.py 6549 2009-03-24 02:58:56Z nicdumz $'
#
# Distributed under the terms of the MIT license.
#

import wikipedia as pywikibot
import pagegenerators, config
import sys

docuReplacements = {'&params;': pagegenerators.parameterHelp}


class TouchBot:
    def __init__(self, generator, touch_redirects):
        self.generator = generator
        self.touch_redirects = touch_redirects

    def run(self):
        for page in self.generator:
            try:
                # get the page, and save it using the unmodified text.
                # whether or not getting a redirect throws an exception
                # depends on the variable self.touch_redirects.
                text = page.get(get_redirect = self.touch_redirects)
                page.put(text)
            except pywikibot.NoPage:
                pywikibot.output(u"Page %s does not exist?!" % page.aslink())
            except pywikibot.IsRedirectPage:
                pywikibot.output(u"Page %s is a redirect; skipping." % page.aslink())
            except pywikibot.LockedPage:
                pywikibot.output(u"Page %s is locked?!" % page.aslink())
            except pywikibot.PageNotSaved:
                pywikibot.output(u"Page %s not saved" % page.aslink())

def main():
    # Disable cosmetic changes because we don't want to modify any page
    # content, so that we don't flood the histories with minor changes.
    config.cosmetic_changes = False
    #page generator
    gen = None
    genFactory = pagegenerators.GeneratorFactory()
    redirs = False
    # If the user chooses to work on a single page, this temporary array is
    # used to read the words from the page title. The words will later be
    # joined with spaces to retrieve the full title.
    pageTitle = []
    for arg in pywikibot.handleArgs():
        if genFactory.handleArg(arg):
            continue
        if arg == '-redir':
            redirs = True
        else:
            pageTitle.append(arg)

    gen = genFactory.getCombinedGenerator()
    if not gen:
        if pageTitle:
            # work on a single page
            page = pywikibot.Page(pywikibot.getSite(), ' '.join(pageTitle))
            gen = iter([page])
        else:
            pywikibot.showHelp('touch')
            return
    preloadingGen = pagegenerators.PreloadingGenerator(gen)
    bot = TouchBot(preloadingGen, redirs)
    bot.run()


if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
