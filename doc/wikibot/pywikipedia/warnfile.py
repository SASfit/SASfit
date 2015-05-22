"""
A robot to implement backlinks from a interwiki.log file without checking them
against the live wikipedia.

Just run this with the warnfile name as parameter. If not specified, the
default filename for the family and language given by global parameters or
user-config.py will be used.

Example:

   python warnfile.py -lang:es

"""
#
# (C) Rob W.W. Hooft, 2003
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: warnfile.py 8632 2010-10-09 21:59:27Z xqt $'
#
import sys, os, re
import wikipedia as pywikibot
import interwiki


class WarnfileReader:
    def __init__(self, filename):
        self.filename = filename

    def getHints(self):
        print "Parsing warnfile..."
        R=re.compile(
            r'WARNING: (?P<family>.+?): \[\[(?P<locallang>.+?):(?P<localtitle>.+?)\]\](?P<warningtype>.+?)\[\[(?P<targetlang>.+?):(?P<targettitle>.+?)\]\]')
        import codecs
        f = codecs.open(self.filename, 'r', 'utf-8')
        hints={}
        removeHints={}
        mysite=pywikibot.getSite()
        for line in f.readlines():
            m=R.search(line)
            if m:
                #print "DBG>",line
                if m.group('locallang') == mysite.lang and \
                   m.group('family') == mysite.family.name:
                    #pywikibot.output(u' '.join([m.group('locallang'), m.group('localtitle'), m.group('warningtype'), m.group('targetsite'), m.group('targettitle')]))
                    #print m.group(3)
                    page = pywikibot.Page(mysite, m.group('localtitle'))
                    removing = (m.group('warningtype') == ' links to incorrect ')
                    try:
                        targetSite = mysite.getSite(code=m.group('targetlang'))
                        targetPage = pywikibot.Page(targetSite,
                                                    m.group('targettitle'))
                        if removing:
                            if page not in removeHints:
                                removeHints[page]=[]
                            removeHints[page].append(targetPage)
                        else:
                            if page not in hints:
                                hints[page]=[]
                            hints[page].append(targetPage)
                    except pywikibot.Error:
                        print "DBG> Failed to add", line
        f.close()
        return hints, removeHints

class WarnfileRobot:
    def __init__(self, warnfileReader):
        self.warnfileReader = warnfileReader

    def run(self):
        hints, removeHints = self.warnfileReader.getHints()
        k=hints.keys()
        k.sort()
        print "Fixing... %i pages" % len(k)
        for page in k:
            old={}
            try:
                for page2 in page.interwiki():
                    old[page2.site()] = page2
            except pywikibot.IsRedirectPage:
                pywikibot.output(u"%s is a redirect page; not changing"
                                 % page.title(asLink=True))
                continue
            except pywikibot.NoPage:
                pywikibot.output(u"Page %s not found; skipping"
                                 % page.title(asLink=True))
                continue
            new={}
            new.update(old)
            if page in hints:
                for page2 in hints[page]:
                    site = page2.site()
                    new[site] = page2
            if page in removeHints:
                for page2 in removeHints[page]:
                    site = page2.site()
                    try:
                        del new[site]
                    except KeyError:
                        pass
            mods, adding, removing, modifying = interwiki.compareLanguages(old,
                                                                           new,
                                                                           insite=page.site())
            if mods:
                pywikibot.output(page.title(asLink=True) + mods)
                oldtext = page.get()
                newtext = pywikibot.replaceLanguageLinks(oldtext, new)
                if 1:
                    pywikibot.showDiff(oldtext, newtext)
                    try:
                        status, reason, data = page.put(newtext,
                                                        comment='warnfile '+mods)
                    except pywikibot.LockedPage:
                        pywikibot.output(u"Page is locked. Skipping.")
                        continue
                    except pywikibot.SpamfilterError, e:
                        pywikibot.output(
                            u'Cannot change %s because of blacklist entry %s'
                            % (page.title(), e.url))
                        continue
                    except pywikibot.Error:
                        pywikibot.output(u"Error while saving page.")
                        continue
                    if str(status) != '302':
                        print status, reason

def main():
    filename = None
    for arg in pywikibot.handleArgs():
        if os.path.isabs(arg):
            filename = arg
        else:
            filename = pywikibot.config.datafilepath("logs", arg)

    if not filename:
        mysite = pywikibot.getSite()
        filename = pywikibot.config.datafilepath('logs',
                       'warning-%s-%s.log' % (mysite.family.name, mysite.lang))
    reader = WarnfileReader(filename)
    bot = WarnfileRobot(reader)
    bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()

