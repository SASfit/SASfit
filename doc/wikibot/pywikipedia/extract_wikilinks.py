#!/usr/bin/python

"""
Script to extract all wiki page names a certain HTML file points to in
interwiki-link format

The output can be used as input to interwiki.py.

This script takes a single file name argument, the file should be a HTML file
as captured from one of the wikipedia servers.

Arguments:
-bare       Extract as internal links: [[Title]] instead of [[Family:xx:Title]]
-sorted     Print the pages sorted alphabetically (default: the order in which
            they occur in the HTML file)
"""
#
# (C) Rob W.W. Hooft, Andre Engels, 2003-2005
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: extract_wikilinks.py 8520 2010-09-11 10:40:05Z xqt $'
#
import sys,re
import codecs
import wikipedia as pywikibot
# This bot does not contact the Wiki, so no need to get it on the list
pywikibot.stopme()
R = re.compile('/wiki/(.*?)" *')
fn = []
sorted = False
list = []
complete = True

for arg in pywikibot.handleArgs():
    if arg.startswith("-sorted"):
        sorted = True
    elif arg.startswith("-bare"):
        complete = False
    elif fn:
        print "Ignoring argument %s"%arg
    else:
        fn = arg

if not fn:
    print "No file specified to get the links from"
    sys.exit(1)

mysite = pywikibot.getSite()
f=open(fn,'r')
text=f.read()
f.close()
for hit in R.findall(text):
    if complete:
        list.append(mysite.linkto(hit))
    else:
        list.append("[[%s]]"%hit)
if sorted:
    list.sort()
for page in list:
    print page
