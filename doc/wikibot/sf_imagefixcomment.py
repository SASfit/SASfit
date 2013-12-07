#!/usr/bin/python
# -*- coding: utf-8  -*-

import sys, re, os.path
import wikipedia, pagegenerators, catlib, weblinkchecker, upload
import StringIO

newComment = u"""[[category:Image]]
[[category:Original Content]]

== Licensing ==
{{cc-by-sa-3.0}}""".replace('\n', '\r\n')

class SFRobot:
    def __init__(self, generator):
        self.generator = generator

    def run(self):
        for page in self.generator:
            try:
                comment = page.get()
                if not comment.find("category"):
                    print repr(comment)
                #if comment != newComment:
#                if len(comment) <= 0:
#                    print "-> editing",page.title()
#                    page.put_async(newComment)
#                    print repr(comment)
#                    print repr(newComment)

            except wikipedia.NoPage:
                print "Page %s does not exist?!" % page.aslink()
            except wikipedia.IsRedirectPage:
                print "Page %s is a redirect; skipping." % page.aslink()
            except wikipedia.LockedPage:
                print "Page %s is locked?!" % page.aslink()

# self: Family
def filelist_address(self, code, limit=500):
    return "%s?title=%s:ListFiles&limit=%d&useskin=monobook" % (self.path(code), self.special_namespace_url(code), limit)

def listfiles_generator():
    site = wikipedia.getSite()
    path = filelist_address(site.family, site.lang)
    ns = site.image_namespace()
    html = site.getUrl(path)
    entryR = re.compile('<a href=".+?" title="(?P<title>%s:.+?)">.+?</a>' % ns)
    for m in entryR.finditer(html):
        title = m.group('title')
        image = wikipedia.ImagePage(site, title)
        yield image

def main():
    gen = listfiles_generator()

    preloadingGen = pagegenerators.PreloadingGenerator(gen)
    bot = SFRobot(preloadingGen)
    bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()


# vim: set ts=4 sw=4 tw=0: 
