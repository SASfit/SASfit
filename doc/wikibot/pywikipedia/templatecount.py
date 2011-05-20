#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This script will display the list of pages transcluding a given list of
templates. It can also be used to simply count the number of pages (rather than
listing each individually).

Syntax: python templatecount.py command [arguments]

Command line options:

-count        Counts the number of times each template (passed in as an
              argument) is transcluded.

-list         Gives the list of all of the pages transcluding the templates
              (rather than just counting them).

-namespace:   Filters the search to a given namespace.  If this is specified
              multiple times it will search all given namespaces

Examples:

Counts how many times {{ref}} and {{note}} are transcluded in articles.

     python templatecount.py -count -namespace:0 ref note

Lists all the category pages that transclude {{cfd}} and {{cfdu}}.

     python templatecount.py -list -namespace:14 cfd cfdu

"""
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: templatecount.py 8631 2010-10-09 21:01:00Z xqt $'

import re, sys, string
import datetime
import wikipedia as pywikibot
import config
import replace, pagegenerators

templates = ['ref', 'note', 'ref label', 'note label', 'reflist']

class TemplateCountRobot:
    #def __init__(self):
        #Nothing
    def countTemplates(self, templates, namespaces):
        mysite = pywikibot.getSite()
        mytpl  = mysite.template_namespace()+':'
        finalText = [u'Number of transclusions per template', u'-' * 36]
        total = 0
        # The names of the templates are the keys, and the numbers of
        # transclusions are the values.
        templateDict = {}
        for template in templates:
            gen = pagegenerators.ReferringPageGenerator(
                pywikibot.Page(mysite, mytpl + template),
                onlyTemplateInclusion = True)
            if namespaces:
                gen = pagegenerators.NamespaceFilterPageGenerator(gen,
                                                                  namespaces)
            count = 0
            for page in gen:
                count += 1
            templateDict[template] = count
            finalText.append(u'%s: %d' % (template, count))
            total += count
        for line in finalText:
            pywikibot.output(line, toStdout=True)
        pywikibot.output(u'TOTAL: %d' % total, toStdout=True)
        pywikibot.output(u'Report generated on %s'
                         % datetime.datetime.utcnow().isoformat(),
                         toStdout=True)
        return templateDict

    def listTemplates(self, templates, namespaces):
        mysite = pywikibot.getSite()
        count = 0
        # The names of the templates are the keys, and lists of pages
        # transcluding templates are the values.
        templateDict = {}
        finalText = [u'List of pages transcluding templates:']
        for template in templates:
            finalText.append(u'* %s' % template)
        finalText.append(u'-' * 36)
        for template in templates:
            transcludingArray = []
            gen = pagegenerators.ReferringPageGenerator(
                pywikibot.Page(mysite,
                               mysite.template_namespace() + ':' + template),
                onlyTemplateInclusion=True)
            if namespaces:
                gen = pagegenerators.NamespaceFilterPageGenerator(gen,
                                                                  namespaces)
            for page in gen:
                finalText.append(u'%s' % page.title())
                count += 1
                transcludingArray.append(page)
            templateDict[template] = transcludingArray;
        finalText.append(u'Total page count: %d' % count)
        for line in finalText:
            pywikibot.output(line, toStdout=True)
        pywikibot.output(u'Report generated on %s'
                         % datetime.datetime.utcnow().isoformat(),
                         toStdout=True)
        return templateDict

def main():
    operation = None
    argsList = []
    namespaces = []

    for arg in pywikibot.handleArgs():
        if arg == '-count':
            operation = "Count"
        elif arg == '-list':
            operation = "List"
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[len('-namespace:'):]))
            except ValueError:
                namespaces.append(arg[len('-namespace:'):])
        else:
            argsList.append(arg)

    if operation == None:
        pywikibot.showHelp('templatecount')
    else:
        robot = TemplateCountRobot()
        if not argsList:
            argsList = templates
        choice = ''
        if 'reflist' in argsList:
            pywikibot.output(
                u'NOTE: it will take a long time to count "reflist".')
            choice = pywikibot.inputChoice(
                u'Proceed anyway?', ['yes', 'no', 'skip'], ['y', 'n', 's'], 'y')
            if choice == 's':
                argsList.remove('reflist')
        if choice == 'n':
            return
        elif operation == "Count":
            robot.countTemplates(argsList, namespaces)
        elif operation == "List":
            robot.listTemplates(argsList, namespaces)

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
