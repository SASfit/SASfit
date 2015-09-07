#!/usr/bin/python
"""
##################################################
This script with all its function has been merged
to templatecount.py. please use:

  templatecount.py -count

xqt 2009-10-30
##################################################
This script checks references to see if they are properly formatted.  Right now
it just counts the total number of transclusions of any number of given templates.

NOTE: This script is not capable of handling the <ref></ref> syntax. It just
handles the {{ref}} syntax, which is still used, but DEPRECATED on the English
Wikipedia.

Syntax: python refcheck.py command [arguments]

Command line options:

-count        Counts the number of times each template (passed in as an argument)
              is transcluded.
-namespace:   Filters the search to a given namespace.  If this is specified
              multiple times it will search all given namespaces

Examples:

Counts how many time {{ref}} and {{note}} are transcluded in articles.

     python refcheck.py -count ref note -namespace:0

"""
__version__ = '$Id: refcheck.py 7574 2009-10-30 09:51:24Z xqt $'

import wikipedia, config
import replace, pagegenerators
import re, sys, string

templates = ['ref', 'note', 'ref label', 'note label', 'reflist']

class ReferencesRobot:
    #def __init__(self):
        #Nothing
    def countRefs(self, templates, namespaces):
        mysite = wikipedia.getSite()
        mytpl  = mysite.template_namespace()+':'
        finalText = [u'Number of transclusions per template',u'------------------------------------']
        for template in templates:
            gen = pagegenerators.ReferringPageGenerator(wikipedia.Page(mysite, mytpl + template), onlyTemplateInclusion = True)
            if namespaces:
                gen = pagegenerators.NamespaceFilterPageGenerator(gen, namespaces)
            count = 0
            for page in gen:
                count += 1
            finalText.append(u'%s: %d' % (template, count))
        for line in finalText:
            wikipedia.output(line)

def main():
    doCount = False
    argsList = []
    namespaces = []
    for arg in wikipedia.handleArgs():
        if arg == '-count':
            doCount = True
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[len('-namespace:'):]))
            except ValueError:
                namespaces.append(arg[len('-namespace:'):])
        else:
            argsList.append(arg)

    if doCount:
        robot = ReferencesRobot()
        if not argsList:
           argsList = templates
        choice = ''
        if 'reflist' in argsList:
            wikipedia.output(u'NOTE: it will take a long time to count "reflist".')
            choice = wikipedia.inputChoice(u'Proceed anyway?', ['yes', 'no', 'skip'], ['y', 'n', 's'], 'y')
            if choice == 's':
                argsList.remove('reflist')
        if choice <> 'n':
            robot.countRefs(argsList, namespaces)
    else:
        wikipedia.showHelp('refcheck')

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()

