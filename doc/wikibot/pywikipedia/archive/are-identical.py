"""
Simple bot to check whether two pages with the same name on different language
'pedias have interwiki links to the same page on another language.

Call the script with 3 arguments:

   python are-identical.py lang1 lang2 name

The script will either print "Yes" and return exit code 0,
                    or print "No"  and return exit code 1,
                    or print "Both links are already present"
                                   and return exit code 2,
                    or print "One links already present"
                                   and return exit code 0.
                                   
It may raise exceptions on pages that disappeared or whatever. This is
a simple framework at least for the moment.
"""
#
# (C) Rob Hooft, 2005
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: are-identical.py,v 1.3 2005/12/21 17:51:26 wikipedian Exp $'
#
from __future__ import generators

import sys, wikipedia

class TwoPageGenerator:
    def __init__(self, lang1, lang2, name):
        self.lang1 = lang1
        self.lang2 = lang2
        self.name = name

    def __iter__(self):
        yield wikipedia.Page(wikipedia.getSite(self.lang1), self.name)
        yield wikipedia.Page(wikipedia.getSite(self.lang2), self.name)


class IdenticalRobot:
    def __init__(self, generator):
        self.generator = generator

    def run(self):
        arr = []
        for x in self.generator:
            arr.append(x)
        pg1 = arr[0]
        pg2 = arr[1]
        iw1 = pg1.interwiki()
        iw2 = pg2.interwiki()
        if pg2 in iw1 and pg1 in iw2:
            print "Both links are already present"
            sys.exit(2)
        if pg2 in iw1 or pg1 in iw2:
            print "One link already present"
            sys.exit(0)
        for iw in iw1:
            if iw in iw2:
                print "Yes"
                sys.exit(0)
        print "No"
        sys.exit(1)
        
def main():
    args = []
    for arg in sys.argv[1:]:
        arg = wikipedia.argHandler(arg, 'are-identical')
        if arg:
            args.append(arg)
    g = TwoPageGenerator(*args)
    r = IdenticalRobot(g)
    r.run()
    
try:
    main()
finally:
    wikipedia.stopme()
            
        
