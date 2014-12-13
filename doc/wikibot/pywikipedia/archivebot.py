#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
archivebot.py - discussion page archiving bot.

usage:

    python archivebot.py [OPTIONS] TEMPLATE_PAGE

Bot examines backlinks (Special:Whatlinkshere) to TEMPLATE_PAGE.
Then goes through all pages (unless a specific page specified using options)
and archives old discussions. This is done by breaking a page into threads,
then scanning each thread for timestamps. Threads older than a specified
treshold are then moved to another page (the archive), which can be named
either basing on the thread's name or then name can contain a counter which
will be incremented when the archive reaches a certain size.

Trancluded template may contain the following parameters:

{{TEMPLATE_PAGE
|archive             =
|algo                =
|counter             =
|maxarchivesize      =
|minthreadsleft      =
|minthreadstoarchive =
|archiveheader       =
|key                 =
}}

Meanings of parameters are:

archive              Name of the page to which archived threads will be put.
                     Must be a subpage of the current page. Variables are
                     supported.
algo                 specifies the maximum age of a thread. Must be in the form
                     old(<delay>) where <delay> specifies the age in hours or
                     days like 24h or 5d.
                     Default ist old(24h)
counter              The current value of a counter which could be assigned as
                     variable. Will be actualized by bot. Initial value is 1.
maxarchivesize       The maximum archive size before incrementing the counter.
                     Value can be given with appending letter like K or M which
                     indicates KByte or MByte. Default value is 1000M.
minthreadsleft       Minimum number of threads that should be left on a page.
                     Default value is 5.
minthreadstoarchive  The minimum number of threads to archive at once. Default
                     value is 2.
archiveheader        Content that will be put on new archive pages as the
                     header. This parameter supports the use of variables.
                     Default value is {{talkarchive}}
key                  A secret key that (if valid) allows archives to not be
                     subpages of the page being archived.


Options:
  -h, --help            show this help message and exit
  -f FILE, --file=FILE  load list of pages from FILE
  -p PAGE, --page=PAGE  archive a single PAGE
  -n NAMESPACE, --namespace=NAMESPACE
                        only archive pages from a given namespace
  -s SALT, --salt=SALT  specify salt
  -F, --force           override security options
  -c PAGE, --calc=PAGE  calculate key for PAGE and exit
  -l LOCALE, --locale=LOCALE
                        switch to locale LOCALE
  -L LANG, --lang=LANG  set the language code to work on
"""
#
# (C) Misza13, 2006-2010
# (C) Pywikipedia bot team, 2007-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: archivebot.py 9188 2011-04-20 16:02:00Z xqt $'
#
import wikipedia as pywikibot
import pagegenerators, query
Site = pywikibot.getSite()

import os, re, time, locale, traceback, string, urllib

try: #Get a constructor for the MD5 hash object
    import hashlib
    new_hash = hashlib.md5
except ImportError: #Old python?
    import md5
    new_hash = md5.md5

language = Site.language()
messages = {
        '_default': {
            'ArchiveFull': u'(ARCHIVE FULL)',
            'InitialArchiveHeader': u'{{talkarchive}}',
            'PageSummary': u'Archiving %(count)d thread(s) (%(why)s) to %(archives)s.',
            'ArchiveSummary': u'Archiving %(count)d thread(s) from [[%(from)s]].',
            'OlderThanSummary': u'older than',
            },
        'ar': {
            'ArchiveFull': u'(الأرشيف ممتلئ)',
            'InitialArchiveHeader': u'{{أرشيف نقاش}}',
            'PageSummary': u'أرشفة %(count)d قسم(أقسام) (%(why)s) إلى %(archives)s.',
            'ArchiveSummary': u'أرشفة %(count)d قسم(أقسام) من [[%(from)s]].',
            'OlderThanSummary': u'أقدم من',
            },
        'pl': {
            'ArchiveFull': u'(ARCHIWUM PEŁNE)',
            'InitialArchiveHeader': u'{{archiwum}}',
            'PageSummary': u'Archiwizacja %(count)d wątków (%(why)s) do %(archives)s.',
            'ArchiveSummary': u'Archiwizacja %(count)d wątków z [[%(from)s]].',
            'OlderThanSummary': u'starsze niż',
            },
        'hu': {
            'ArchiveFull': u'(ARCHÍVUM BETELT)',
            'InitialArchiveHeader': u'{{archív lap}}',
            'PageSummary': u'%(count)d szakasz archiválása (%(why)s) a(z) %(archives)s lapra.',
            'ArchiveSummary': u'%(count)d szakasz archiválása a(z) [[%(from)s]] lapról.',
            'OlderThanSummary': u'régebbi, mint',
            },

        'ksh': {
            'ArchiveFull': u'(DAT ASCHIHV ES VOLL)',
            'InitialArchiveHeader': u'{{Sigg weed aschiveet}}',
            'PageSummary': u'* %(count)d Schtöe) (%(why)s) en et Aschif %(archives)s jedonn.',
            'ArchiveSummary': u'* %(count)d Schtö) vun [[%(from)s]] noh heh en et Aschihf jedonn.',
            'OlderThanSummary': u'äer wi',
             },
        'no': {
            'ArchiveFull': u'(ARKIV FULLT)',
            'InitialArchiveHeader': u'{{arkiv}}',
            'PageSummary': u'Arkiverer %(count)d tråder (%(why)s) til %(archives)s.',
            'ArchiveSummary': u'Arkiverer %(count)d tråder fra [[%(from)s]].',
            'OlderThanSummary': u'eldre enn',
            },
        'nn': {
            'ArchiveFull': u'(ARKIV FULLT)',
            'InitialArchiveHeader': u'{{arkiv}}',
            'PageSummary': u'Arkiverer %(count)d trådar (%(why)s) til %(archives)s.',
            'ArchiveSummary': u'Arkiverer %(count)d trådar frå [[%(from)s]].',
            'OlderThanSummary': u'eldre enn',
            },
        'pt': {
            'ArchiveFull': u'(ARQUIVO COMPLETO)',
            'InitialArchiveHeader': u'{{talkarchive}}',
            'PageSummary': u'Arquivando %(count)d thread(s) (%(why)s) to %(archives)s.',
            'ArchiveSummary': u'Archiving %(count)d thread(s) from [[%(from)s]].',
            'OlderThanSummary': u'older than',
            },

        'sv': {
            'ArchiveFull': u'(ARKIV FULLT)',
            'InitialArchiveHeader': u'{{arkiv}}',
            'PageSummary': u'Arkiverar %(count)d trådar (%(why)s) till %(archives)s.',
            'ArchiveSummary': u'Arkiverar %(count)d trådar från [[%(from)s]].',
            'OlderThanSummary': u'äldre än',
            },
        'fi': {
            'ArchiveFull' : u'(ARKISTO TÄYSI)',
            'InitialArchiveHeader': u'{{arkisto}}',
            'PageSummary': u'Arkistoidaan %(count)d keskustelua (%(why)s) %(archives)s arkistoon.',
            'ArchiveSummary': u'Arkistoidaan %(count)d keskustelua sivulta [[%(from)s]].',
            'OlderThanSummary': u'vanhempi kuin',
            },
}

def message(key, lang=Site.language()):
    if not lang in messages:
        lang = '_default'
    return messages[lang][key]

class MalformedConfigError(pywikibot.Error):
    """There is an error in the configuration template."""

class MissingConfigError(pywikibot.Error):
    """The config is missing in the header (either it's in one of the threads
    or transcluded from another page)."""

class AlgorithmError(MalformedConfigError):
    """Invalid specification of archiving algorithm."""

class ArchiveSecurityError(pywikibot.Error):
    """Archive is not a subpage of page being archived and key not specified
    (or incorrect)."""

def str2time(str):
    """Accepts a string defining a time period:
    7d - 7 days
    36h - 36 hours
    Returns the corresponding time, measured in seconds."""
    if str[-1] == 'd':
        return int(str[:-1])*24*3600
    elif str[-1] == 'h':
        return int(str[:-1])*3600
    else:
        return int(str)

def str2size(str):
    """Accepts a string defining a size:
    1337 - 1337 bytes
    150K - 150 kilobytes
    2M - 2 megabytes
    Returns a tuple (size,unit), where size is an integer and unit is
    'B' (bytes) or 'T' (threads)."""
    if str[-1] in string.digits: #TODO: de-uglify
        return (int(str),'B')
    elif str[-1] in ['K', 'k']:
        return (int(str[:-1])*1024,'B')
    elif str[-1] == 'M':
        return (int(str[:-1])*1024*1024,'B')
    elif str[-1] == 'T':
        return (int(str[:-1]),'T')
    else:
        return (int(str[:-1])*1024,'B')

def int2month(num):
    """Returns the locale's full name of month 'num' (1-12)."""
    if hasattr(locale, 'nl_langinfo'):
        return locale.nl_langinfo(locale.MON_1+num-1).decode('utf-8')
    Months = ['january', 'february', 'march', 'april', 'may_long', 'june',
              'july', 'august', 'september', 'october', 'november', 'december']
    return Site.mediawiki_message(Months[num-1])

def int2month_short(num):
    """Returns the locale's abbreviated name of month 'num' (1-12)."""
    if hasattr(locale, 'nl_langinfo'):
        #filter out non-alpha characters
        return ''.join([c for c in locale.nl_langinfo(locale.ABMON_1+num-1).decode('utf-8') if c.isalpha()])
    Months = ['jan', 'feb', 'mar', 'apr', 'may', 'jun',
              'jul', 'aug', 'sep', 'oct', 'nov', 'dec']
    return Site.mediawiki_message(Months[num-1])

def txt2timestamp(txt, format):
    """Attempts to convert the timestamp 'txt' according to given 'format'.
    On success, returns the time tuple; on failure, returns None."""
    #print txt, format
    try:
        return time.strptime(txt,format)
    except ValueError:
        try:
            return time.strptime(txt.encode('utf8'),format)
        except:
            pass
        return None

def generateTransclusions(Site, template, namespaces=[], eicontinue=''):
    qdata = {
        'action' : 'query',
        'list' : 'embeddedin',
        'eititle' : template,
        'einamespace' : '|'.join(namespaces),
        'eilimit' : '100',
        'format' : 'json',
        }
    if eicontinue:
        qdata['eicontinue'] = eicontinue

    pywikibot.output(u'Fetching template transclusions...')
    response, result = query.GetData(qdata, Site, back_response = True)

    for page_d in result['query']['embeddedin']:
        yield pywikibot.Page(Site, page_d['title'])

    if 'query-continue' in result:
        eicontinue = result['query-continue']['embeddedin']['eicontinue']
        for page in generateTransclusions(Site, template, namespaces,
                                          eicontinue):
            yield page

class DiscussionThread(object):
    """An object representing a discussion thread on a page, that is something of the form:

    == Title of thread ==

    Thread content here. ~~~~
    :Reply, etc. ~~~~
    """

    def __init__(self, title):
        self.title = title
        self.content = ""
        self.timestamp = None

    def __repr__(self):
        return '%s("%s",%d bytes)' \
               % (self.__class__.__name__,self.title,len(self.content))

    def feedLine(self, line):
        if not self.content and not line:
            return
        self.content += line + '\n'
        #Update timestamp
# nnwiki:
# 19:42, 25 mars 2008 (CET)
# enwiki
# 16:36, 30 March 2008 (UTC)
# huwiki
# 2007. december 8., 13:42 (CET)
        TM = re.search(r'(\d\d):(\d\d), (\d\d?) (\S+) (\d\d\d\d) \(.*?\)', line)
        if not TM:
            TM = re.search(r'(\d\d):(\d\d), (\S+) (\d\d?), (\d\d\d\d) \(.*?\)', line)
        if not TM:
            TM = re.search(r'(\d{4})\. (\S+) (\d\d?)\., (\d\d:\d\d) \(.*?\)', line)
# 18. apr 2006 kl.18:39 (UTC)
# 4. nov 2006 kl. 20:46 (CET)
        if not TM:
            TM = re.search(r'(\d\d?)\. (\S+) (\d\d\d\d) kl\.\W*(\d\d):(\d\d) \(.*?\)', line)
#3. joulukuuta 2008 kello 16.26 (EET)
        if not TM:
            TM = re.search(r'(\d\d?)\. (\S+) (\d\d\d\d) kello \W*(\d\d).(\d\d) \(.*?\)', line)
        if not TM:
# 14:23, 12. Jan. 2009 (UTC)
            pat = re.compile(r'(\d\d):(\d\d), (\d\d?)\. (\S+)\.? (\d\d\d\d) \(UTC\)')
            TM = pat.search(line)
        if TM:
#            pywikibot.output(TM)
            TIME = txt2timestamp(TM.group(0),"%d. %b %Y kl. %H:%M (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0), "%Y. %B %d., %H:%M (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%d. %b %Y kl.%H:%M (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %d %B %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %d %b %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(re.sub(' *\([^ ]+\) *','',TM.group(0)),"%H:%M, %d %b %Y")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %b %d %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %B %d %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %b %d, %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %B %d, %Y (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%d. %Bta %Y kello %H.%M (%Z)")
            if not TIME:
                TIME = txt2timestamp(TM.group(0),"%H:%M, %d. %b. %Y (%Z)")
            if TIME:
                self.timestamp = max(self.timestamp,time.mktime(TIME))
#                pywikibot.output(u'Time to be parsed: %s' % TM.group(0))
#                pywikibot.output(u'Parsed time: %s' % TIME)
#                pywikibot.output(u'Newest timestamp in thread: %s' % TIME)

    def size(self):
        return len(self.title) + len(self.content) + 12

    def toText(self):
        return "== " + self.title + ' ==\n\n' + self.content

    def shouldBeArchived(self,Archiver):
        algo = Archiver.get('algo')
        reT = re.search(r'^old\((.*)\)$',algo)
        if reT:
            if not self.timestamp:
                return ''
            #TODO: handle this:
                #return 'unsigned'
            maxage = str2time(reT.group(1))
            if self.timestamp + maxage < time.time():
                return message('OlderThanSummary') + ' ' + reT.group(1)
        return ''

class DiscussionPage(object):
    """A class that represents a single discussion page as well as an archive
    page. Feed threads to it and run an update() afterwards."""
    #TODO: Make it a subclass of pywikibot.Page

    def __init__(self, title, archiver, vars=None):
        self.title = title
        self.threads = []
        self.Page = pywikibot.Page(Site,self.title)
        self.full = False
        self.archiver = archiver
        self.vars = vars
        try:
            self.loadPage()
        except pywikibot.NoPage:
            self.header = archiver.get('archiveheader',
                                       message('InitialArchiveHeader'))
            if self.vars:
                self.header = self.header % self.vars

    def loadPage(self):
        """Loads the page to be archived and breaks it up into threads."""
        self.header = ''
        self.threads = []
        self.archives = {}
        self.archivedThreads = 0
        lines = self.Page.get().split('\n')
        state = 0 #Reading header
        curThread = None
        for line in lines:
            threadHeader = re.search('^== *([^=].*?) *== *$',line)
            if threadHeader:
                state = 1 #Reading threads now
                if curThread:
                    self.threads.append(curThread)
                curThread = DiscussionThread(threadHeader.group(1))
            else:
                if state == 1:
                    curThread.feedLine(line)
                else:
                    self.header += line + '\n'
        if curThread:
            self.threads.append(curThread)

    def feedThread(self, thread, maxArchiveSize=(250*1024,'B')):
        self.threads.append(thread)
        self.archivedThreads += 1
        if maxArchiveSize[1] == 'B':
            if self.size() >= maxArchiveSize[0]:
                self.full = True
        elif maxArchiveSize[1] == 'T':
            if len(self.threads) >= maxArchiveSize[0]:
                self.full = True
        return self.full

    def size(self):
        return len(self.header) + sum([t.size() for t in self.threads])

    def update(self, summary, sortThreads = False):
        if sortThreads:
            pywikibot.output(u'Sorting threads...')
            self.threads.sort(key = lambda t: t.timestamp)
        newtext = re.sub('\n*$','\n\n',self.header) #Fix trailing newlines
        for t in self.threads:
            newtext += t.toText()
        if self.full:
            summary += ' ' + message('ArchiveFull')
        self.Page.put(newtext, minorEdit=True, comment=summary)

class PageArchiver(object):
    """A class that encapsulates all archiving methods.
    __init__ expects a pywikibot.Page object.
    Execute by running the .run() method."""

    algo = 'none'
    pageSummary = message('PageSummary')
    archiveSummary = message('ArchiveSummary')

    def __init__(self, Page, tpl, salt, force=False):
        self.attributes = {
                'algo' : ['old(24h)',False],
                'archive' : ['',False],
                'maxarchivesize' : ['1000M',False],
                'counter' : ['1',False],
                'key' : ['',False],
                }
        self.tpl = tpl
        self.salt = salt
        self.force = force
        self.Page = DiscussionPage(Page.title(),self)
        self.loadConfig()
        self.commentParams = {
                'from' : self.Page.title,
                }
        self.archives = {}
        self.archivedThreads = 0

    def get(self, attr, default=''):
        return self.attributes.get(attr,[default])[0]

    def set(self, attr, value, out=True):
        self.attributes[attr] = [value, out]

    def saveables(self):
        return [a for a in self.attributes if self.attributes[a][1]
                and a != 'maxage']

    def attr2text(self):
        return '{{%s\n%s\n}}' \
               % (self.tpl,
                  '\n'.join(['|%s = %s'%(a,self.get(a))
                             for a in self.saveables()]))

    def key_ok(self):
        s = new_hash()
        s.update(self.salt+'\n')
        s.update(self.Page.title.encode('utf8')+'\n')
        return self.get('key') == s.hexdigest()

    def loadConfig(self):
        hdrlines = self.Page.header.split('\n')
#        pywikibot.output(u'Looking for: %s' % self.tpl)
        mode = 0
        for line in hdrlines:
            if mode == 0 and re.search('{{'+self.tpl,line):
                mode = 1
                continue
            if mode == 1 and re.match('}}',line):
                break
            attRE = re.search(r'^\| *(\w+) *= *(.*?) *$',line)
            if mode == 1 and attRE:
                self.set(attRE.group(1),attRE.group(2))
                continue

        if mode == 0 or not self.get('algo',''):
            raise MissingConfigError

        #Last minute fix:
        self.set('archive', self.get('archive').replace('_',' '), True)

    def feedArchive(self, archive, thread, maxArchiveSize, vars=None):
        """Feed the thread to one of the archives.
        If it doesn't exist yet, create it.
        If archive name is an empty string (or None),
        discard the thread (/dev/null).
        Also checks for security violations."""
        if not archive:
            return
        if not self.force \
           and not self.Page.title+'/' == archive[:len(self.Page.title)+1] \
           and not self.key_ok():
            raise ArchiveSecurityError
        if not archive in self.archives:
            self.archives[archive] = DiscussionPage(archive,self,vars)
        return self.archives[archive].feedThread(thread,maxArchiveSize)

    def analyzePage(self):
        maxArchSize = str2size(self.get('maxarchivesize'))
        archCounter = int(self.get('counter','1'))
        oldthreads = self.Page.threads
        self.Page.threads = []
        T = time.mktime(time.gmtime())
        whys = []
        for t in oldthreads:
            if len(oldthreads) - self.archivedThreads \
               <= int(self.get('minthreadsleft',5)):
                self.Page.threads.append(t)
                continue #Because there's too little threads left.
            # TODO: Make an option so that unstamped (unsigned) posts get
            # archived.
            why = t.shouldBeArchived(self)
            if why:
                archive = self.get('archive')
                TStuple = time.gmtime(t.timestamp)
                vars = {
                        'counter' : archCounter,
                        'year' : TStuple[0],
                        'month' : TStuple[1],
                        'monthname' : int2month(TStuple[1]),
                        'monthnameshort' : int2month_short(TStuple[1]),
                        'week' : int(time.strftime('%W',TStuple)),
                        }
                archive = archive % vars
                if self.feedArchive(archive,t,maxArchSize,vars):
                    archCounter += 1
                    self.set('counter',str(archCounter))
                whys.append(why)
                self.archivedThreads += 1
            else:
                self.Page.threads.append(t)
        return set(whys)

    def run(self):
        if not self.Page.Page.botMayEdit(Site.username):
            return
        whys = self.analyzePage()
        if self.archivedThreads < int(self.get('minthreadstoarchive',2)):
            # We might not want to archive a measly few threads
            # (lowers edit frequency)
            return
        if whys:
            #Save the archives first (so that bugs don't cause a loss of data)
            for a in sorted(self.archives.keys()):
                self.commentParams['count'] = self.archives[a].archivedThreads
                comment = self.archiveSummary % self.commentParams
                self.archives[a].update(comment)

            #Save the page itself
            rx = re.compile('{{'+self.tpl+'\n.*?\n}}',re.DOTALL)
            self.Page.header = rx.sub(self.attr2text(),self.Page.header)
            self.commentParams['count'] = self.archivedThreads
            self.commentParams['archives'] \
                = ', '.join(['[['+a.title+']]' for a in self.archives.values()])
            if not self.commentParams['archives']:
                self.commentParams['archives'] = '/dev/null'
            self.commentParams['why'] = ', '.join(whys)
            comment = self.pageSummary % self.commentParams
            self.Page.update(comment)

def main():
    global Site, language
    from optparse import OptionParser
    parser = OptionParser(usage='usage: %prog [options] [LINKPAGE(s)]')
    parser.add_option('-f', '--file', dest='filename',
            help='load list of pages from FILE', metavar='FILE')
    parser.add_option('-p', '--page', dest='pagename',
            help='archive a single PAGE', metavar='PAGE')
    parser.add_option('-n', '--namespace', dest='namespace', type='int',
            help='only archive pages from a given namespace')
    parser.add_option('-s', '--salt', dest='salt',
            help='specify salt')
    parser.add_option('-F', '--force', action='store_true', dest='force',
            help='override security options')
    parser.add_option('-c', '--calc', dest='calc',
            help='calculate key for PAGE and exit', metavar='PAGE')
    parser.add_option('-l', '--locale', dest='locale',
            help='switch to locale LOCALE', metavar='LOCALE')
    parser.add_option('-L', '--lang', dest='lang',
            help='current language code', metavar='lang')
    parser.add_option('-T', '--timezone', dest='timezone',
            help='switch timezone to TIMEZONE', metavar='TIMEZONE')
    (options, args) = parser.parse_args()

    if options.locale:
        #Required for english month names
        locale.setlocale(locale.LC_TIME,options.locale)

    if options.timezone:
        os.environ['TZ'] = options.timezone
    #Or use the preset value
    if hasattr(time, 'tzset'):
        time.tzset()

    if options.calc:
        if not options.salt:
            parser.error('you must specify a salt to calculate a key')
        s = new_hash()
        s.update(options.salt+'\n')
        s.update(options.calc+'\n')
        pywikibot.output(u'key = ' + s.hexdigest())
        return

    if options.salt:
        salt = options.salt
    else:
        salt = ''

    if options.force:
        force = True
    else:
        force = False

    if options.lang:
        Site = pywikibot.getSite(options.lang)
        language = Site.language()
        if pywikibot.debug: print Site

    for a in args:
        pagelist = []
        if not options.filename and not options.pagename:
            #for pg in pywikibot.Page(Site,a).getReferences(follow_redirects=False,onlyTemplateInclusion=True):
            if not options.namespace == None:
                ns = [str(options.namespace)]
            else:
                ns = []
            for pg in generateTransclusions(Site, a, ns):
                pagelist.append(pg)
        if options.filename:
            for pg in file(options.filename,'r').readlines():
                pagelist.append(pywikibot.Page(Site,pg))
        if options.pagename:
            pagelist.append(pywikibot.Page(Site,options.pagename))

        pagelist = sorted(pagelist)
        #if not options.namespace == None:
        #    pagelist = [pg for pg in pagelist if pg.namespace()==options.namespace]

        for pg in pagelist:
            # Catching exceptions, so that errors in one page do not bail out
            # the entire process
            try:
                Archiver = PageArchiver(pg, a, salt, force)
                Archiver.run()
                time.sleep(10)
            except:
                pywikibot.output(u'Error occured while processing page %s'
                                 % pg.aslink(True))
                traceback.print_exc()

if __name__ == '__main__':
    try:
        main()
    finally:
        pywikibot.stopme()
