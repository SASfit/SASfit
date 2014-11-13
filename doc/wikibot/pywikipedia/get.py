# -*- coding: utf-8 -*-
"""
Very simple script which gets a page and writes its contents to
standard output. This makes it possible to pipe the text to another
process.

Syntax: python get.py Title of the page

Example: python get.py Wikipedia | grep MediaWiki > results.txt
"""

# (C) Daniel Herding, 2005
#
# Distributed under the terms of the MIT license.

__version__='$Id: get.py 8525 2010-09-11 16:21:58Z xqt $'

import wikipedia as pywikibot

def main():
    singlePageTitleParts = []
    for arg in pywikibot.handleArgs():
        singlePageTitleParts.append(arg)

    pageTitle = " ".join(singlePageTitleParts)
    page = pywikibot.Page(pywikibot.getSite(), pageTitle)

    # TODO: catch exceptions
    pywikibot.output(page.get(), toStdout = True)

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()

