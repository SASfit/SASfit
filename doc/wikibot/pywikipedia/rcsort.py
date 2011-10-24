#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
A tool to see the recentchanges ordered by user instead of by date. This
is meant to be run as a CGI script.
Apart from the normal options of the recent changes page, you can add an option
?newbies=true which will make the bot go over recently registered users only.
Currently only works on Dutch Wikipedia, I do intend to make it more generally
usable.
Permission has been asked to run this on the toolserver.
"""
# (C) Pywikipedia bot team, 2007-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: rcsort.py 8629 2010-10-09 16:11:46Z xqt $'
#

import cgi
import cgitb
import re
import wikipedia

cgitb.enable()

form = cgi.FieldStorage()
print "Content-Type: text/html"
print
print
print "<html>"
print "<head>"
print '<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />'
print '<style type="text/css" media="screen,projection">/*<![CDATA[*/ @import "http://nl.pywikibot.org/skins-1.5/monobook/main.css?59"; /*]]>*/</style>'
print "</head>"
print "<body>"
print "<!--"
print "-->"
mysite = pywikibot.getSite()

newbies = 'newbies' in form

if newbies:
    path = mysite.contribs_address(self, target='newbies')
else:
    path = mysite.get_address("Special:RecentChanges")

for element in form:
    if element != 'newbies':
        path += '&%s=%s' % (element, form[element].value)
if not 'limit' in form:
    path += '&limit=1000'

text = mysite.getUrl(path)

text = text.split('\n')
rcoptions = False
lines = []
if newbies:
    Ruser = re.compile('\"Overleg gebruiker:([^\"]*)\"\>Overleg\<\/a\>\)')
else:
    Ruser = re.compile('title=\"Speciaal\:Bijdragen\/([^\"]*)\"')
Rnumber = re.compile('tabindex=\"(\d*)\"')
count = 0
for line in text:
    if rcoptions:
        if 'gesch' in line:
            try:
                user = Ruser.search(line).group(1)
            except AttributeError:
                user = None
            count += 1
            lines.append((user,count,line))
    elif 'rcoptions' in line:
        print line.replace(mysite.path() + "?title=Speciaal:RecenteWijzigingen&amp;",
                           "rcsort.py?")
        rcoptions = True
    elif newbies and 'Nieuwste' in line:
        line =  line.replace(mysite.path() + "?title=Speciaal:Bijdragen&amp;",
                             "rcsort.py?").replace("target=newbies",
                                                   "newbies=true")
        if '</fieldset>' in line:
            line = line[line.find('</fieldset>')+11:]
        print line
        rcoptions = True
lines.sort()
last = 0

for line in lines:
    if line[0] != last:
        print "</ul>"
        if line[0] == None:
            print "<h2>Gebruiker onbekend</h2>"
        else:
            pywikibot.output(u"<h2>%s</h2>"%line[0],toStdout=True)
        print "<ul>"
        last = line[0]
    pywikibot.output(line[2].replace('href="/w','href="http://nl.wikipedia.org/w'), toStdout = True)
    print

print "</ul>"
print "<p>&nbsp;</p>"
print "</body>"
print "</html>"
