# -*- coding: utf-8  -*-
"""
(C) 2004 Thomas R. Koll, <tomk32@tomk32.de>
 Distributed under the terms of the MIT license.

This bot consists of WdT.py and WdTXMLpaser.py and
imports XML-files into Wikipedia.
The XML-file contains the an automatic generated list
of the most significant word in current events
which the bot use as article-links and compare to
a local list of all articles. Only the not-yet-written
articles will be saved on wikipedia.

"""

__version__='$Id: WdT.py,v 1.9 2005/12/21 17:51:26 wikipedian Exp $'


import WdTXMLParser,wikipedia,re,datetime,xml.sax,fileinput
import string

DEBUG = 0
host = "http://wortschatz.uni-leipzig.de/wort-des-tages/RDF/WdT/"

localArticleList = "Stichwortliste_de_Wikipedia_2004-04-17_sortiert.txt"

XMLfiles = {
    "ort.xml"           : "Orte",
    "ereignis.xml"      : "Ereignisse",
    "kuenstler.xml"     : "Kunst, Kultur und Wissenschaft", 
    "organisation.xml"  : "Organisationen",
    "politiker.xml"     : "Politiker",
    "schlagwort.xml"    : u"Schlagwörter",
    "sportler.xml"      : "Sportler",
    "sport.xml"         : "Sport",
    "person.xml"        : "sonstige"
    }
article = "Wikipedia:Wort_des_Tages"

newText = "\n== " + str(datetime.date.today()) + " =="

#start the xml parser
ch = WdTXMLParser.WdTXMLParser()
parser = xml.sax.make_parser()
parser.setContentHandler(ch)

# first we get the XML-File
for file in XMLfiles:
    print "\ngetting: " + file,
    parser.parse(host + file)
    data = ch.result
    print " parsing..."
    # now we parse the file


    # and make a result text for wikipedia
    skip = []
    if localArticleList != "":
        import string
        add = {}
        for a in data:
            print "\nchecking: " + a,
            userCommand = raw_input('[C]orrect, [S]kip or [K]eep?')
            if userCommand == 'c':
                b = raw_input('Correct it: ')
                if b != a:
                    add[b] = data[a]
                    skip.append(a)
                    a = b
            if userCommand == 's':
                print "...skipping ",
                skip.append(a)
                continue              
            for line in fileinput.input(localArticleList):
                if unicode(string.strip(line),"iso-8859-1") == a:
                    skip.append(a)
                    print "...skipping ",
                    break
            fileinput.close()
            if skip.count(a) == 0:
                try:
                    pl = wikipedia.Page(wikipedia.getSite(), a)
                    text = pl.get()
                    if len(text) > 500:
                        skip.append(a)
                        print "...skipping ",
                        break
                    else:
                        print "...stub ",
                except wikipedia.NoPage:
                    print "...doesn't exist yet",
                    continue
                except:
                    skip.append(a)
                    print "...skipping ",
                    break
    for b in add:
        data[b] = add[b]
    for a in skip:
        del data[a]

    if DEBUG >= 2:
        print data

    if data:
        newText = newText + "\n* '''" +  XMLfiles[file] + ":''' "
    for a in data:
        newText = newText + "[[" + a + "]] ([" + \
                  data[a]['link'] + ' ' + data[a]['count'] + ']) '
    if DEBUG >= 2:
        print newText

pl = wikipedia.Page(wikipedia.getSite(), article)
text = pl.get()
newText = text + newText


if DEBUG:
    print newText
else:
    status, reason, data = pl.put(newText, "WdT: updated")
    print status, reason
