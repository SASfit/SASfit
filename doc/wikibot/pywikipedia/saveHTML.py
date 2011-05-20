# -*- coding: utf-8  -*-
"""
This bot downloads the HTML-pages of articles and images
and saves the interesting parts, i.e. the article-text
and the footer to a file like Hauptseite.txt.

TODO:
   change the paths in the HTML-file


Options:

      -o:                Specifies the output-directory where to save the files

      -images:           Downlaod all images
      -overwrite:[I|A|B] Ignore existing Images|Article|Both and
                         download them even if the exist


Features, not bugs:
* Won't d/l images of an article if you set -overwrite:A

"""

# (C) 2004 Thomas R. Koll, <tomk32@tomk32.de>
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: saveHTML.py 7650 2009-11-15 13:17:01Z alexsh $'

import wikipedia,httplib,StringIO,re,sys,md5,os, string
from htmlentitydefs import *

def extractArticle(data):
    """ takes a string with the complete HTML-file
    and returns the article which is contained in
    <div id='article'> and  the pagestats which
    contain information on last change """

    images = []
    s = StringIO.StringIO(data)
    rPagestats = re.compile('.*(\<span id\=(\"|\')pagestats(\"|\')\>.*\<\/span\>).*')
    rBody = re.compile('.*<div id\=\"content\">.*')
    rFooter = re.compile('.*<div id\=\"footer\">.*')
    rDivOpen = re.compile('.*<div ')
    rDivClose = re.compile('.*<\/div>.*')
    divLevel = 1
    divLast = -1
    inArticle = 0
    inFooter  = 0
    result = {'article':"",
              'footer':""}
    for line in s:
        if line == "<p><br /></p>":
            continue
        line = line.replace("&#160;", " ")
        line = line.replace("&nbsp;", " ")

        if rDivOpen.match(line):
            divLevel = divLevel + 1
        if rBody.match(line):
            inArticle = 1
            divLast = divLevel-2
        elif rFooter.match(line):
            divLast = divLevel-1
            inFooter  = 1
        if inArticle:
            result['article'] += line
        elif inFooter:
            result['footer'] += line
        if rDivClose.match(line):
            divLevel = divLevel - 1
            if divLevel == divLast:
                inArticle = 0
                inFooter = 0
                divLast = -1


    return result

def html2txt(str):
    dict = {"%C3%A4": "ä",
            "%C3%B6": "ö",
            "%C3%BC": "ü",
            "%C3%84": "Ä",
            "%C3%96": "Ö",
            "%C3%9C": "Ü",
            "%C3%9F": "ß",
            "%27": "'",
            "%28": "(",
            "%29": ")",
            "%2C": ","
            }

    for entry in dict:
        str = re.sub(entry, dict[entry], str)
    return str

def extractImages(data):
    """ takes a string with the complete HTML-file
    and returns the article which is contained in
    <div id='article'> and  the pagestats which
    contain information on last change """

    images = []
    rImage = re.compile('<a href=[\r\n]*?"/wiki/.*?:(.*?)".*?[\r\n]*?.*?class=[\r\n]*?"image"', re.MULTILINE)
    rThumb = re.compile('<a href=[\r\n]*?"/wiki/.*?:(.*?)".*?[\r\n]*?.*?class=[\r\n]*?"internal".*?[\r\n]*?.*?<img', re.MULTILINE or re.DOTALL)
    last = ""
    img = rImage.findall(data)
    timg = rThumb.findall(data)
    for i in timg:
        try:
            img.index(i)
        except:
            img.append(i)
    print "Bilder: ", img

    for image in img:
        path = md5.new(html2txt(image)).hexdigest()
        images.append( {'image': image,
                        'path' : str(path[0])+"/"+str(path[0:2])+"/"})
    images.sort()
    return images


def main():
    mysite = wikipedia.getSite()
    sa = []
    output_directory = ""
    save_images = False
    overwrite_images = False
    overwrite_articles = False

    for arg in wikipedia.handleArgs():
        if arg.startswith("-lang:"):
            lang = arg[6:]
        elif arg.startswith("-file:"):
            f=open(arg[6:], 'r')
            R=re.compile(r'.*\[\[([^\]]*)\]\].*')
            m = False
            for line in f.readlines():
                m=R.match(line)
                if m:
                    sa.append(string.replace(m.group(1), " ", "_"))
                else:
                    print "ERROR: Did not understand %s line:\n%s" % (
                        arg[6:], repr(line))
            f.close()
        elif arg.startswith("-o:"):
            output_directory = arg[3:]
        elif arg.startswith("-images"):
            save_images = True
        elif arg.startswith("-overwrite:"):
            if arg[11] == "I":
                overwrite_images = True
            elif arg[11] == "A":
                overwrite_articles = True
            elif arg[11] == "B":
                overwrite_images = True
                overwrite_articles = True
        else:
            sa.append(arg.replace(" ", "_"))

    headers = {"Content-type": "application/x-www-form-urlencoded",
               "User-agent": wikipedia.useragent}
    print "opening connection to", mysite.hostname(),
    conn = httplib.HTTPConnection(mysite.hostname())
    print " done"

    R = re.compile('.*/wiki/(.*)')
    data = ""
    for article in sa:
        filename = article.replace("/", "_")
        if os.path.isfile(output_directory + filename + ".txt") and overwrite_articles == False:
            print "skipping " + article
            continue
        data = ""
        ua = article
        while len(data) < 2:
            url = '/wiki/'+ ua
            conn.request("GET", url, "", headers)
            response = conn.getresponse()
            data = response.read()
            if len(data) < 2:
                print ua + " failed. reading",
                result = R.match(response.getheader("Location", ))
                ua = result.group(1)
                print ua

        data = extractArticle(data)
        f = open (output_directory + filename + ".txt", 'w')
        f.write (data['article'] + '\n' + data['footer'])
        f.close()
        print "saved " + article

        if save_images:
            images = extractImages(data['article'])
            for i in images:
                if overwrite_images == False and os.path.isfile(output_directory + i['image']):
                    print "skipping existing " + i['image']
                    continue
                print 'downloading ' + i['image'],
                uo = wikipedia.MyURLopener
                file = uo.open( "http://upload.wikimedia.org/wikipedia/"
                                +mysite.lang + '/' + i['path'] + i['image'])
                content = file.read()
                if (len(content) < 500):
                    uo.close()
                    print "downloading from commons",
                    uo = wikipedia.MyURLopener
                    file = uo.open( "http://commons.wikimedia.org/upload/"
                                    + i['path'] + i['image'])
                    #print "http://commons.wikimedia.org/upload/", i['path'] , i['image'], file
                    content = file.read()
                f = open(output_directory + i['image'], "wb")
                f.write(content)
                f.close()
                print "\t\t", (len(content)/1024), "KB done"
    conn.close()

if __name__ == "__main__":
    main()
