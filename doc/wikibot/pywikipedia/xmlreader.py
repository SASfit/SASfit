# -*- coding: utf-8  -*-

"""
Each XmlEntry object represents a page, as read from an XML source

The MediaWikiXmlHandler can be used for the XML given by Special:Export
as well as for XML dumps.

The XmlDump class reads a pages_current XML dump (like the ones offered on
http://download.wikimedia.org/wikipedia/de/) and offers a generator over
XmlEntry objects which can be used by other bots.

For fastest processing, XmlDump uses the cElementTree library if available
(this comes included with Python 2.5, and can be downloaded from
http://www.effbot.org/ for earlier versions). If not found, it falls back
to the older method using regular expressions.
"""
#
# (C) Pywikipedia bot team, 2005-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: xmlreader.py 9042 2011-03-13 10:14:47Z xqt $'
#

import threading
import xml.sax
import codecs, re
import wikipedia as pywikibot

try:
    from xml.etree.cElementTree import iterparse
except ImportError:
    try:
        from cElementTree import iterparse
    except ImportError:
        pass

def parseRestrictions(restrictions):
    """
    Parses the characters within a restrictions tag and returns
    strings representing user groups allowed to edit and to move
    a page, where None means there are no restrictions.
    """
    if not restrictions:
        return None, None
    editRestriction = None
    moveRestriction = None
    editLockMatch = re.search('edit=([^:]*)', restrictions)
    if editLockMatch:
        editRestriction = editLockMatch.group(1)
    moveLockMatch = re.search('move=([^:]*)', restrictions)
    if moveLockMatch:
        moveRestriction = moveLockMatch.group(1)
    if restrictions == 'sysop':
        editRestriction = 'sysop'
        moveRestriction = 'sysop'
    return editRestriction, moveRestriction


class XmlEntry:
    """
    Represents a page.
    """
    def __init__(self, title, id, text, username, ipedit, timestamp,
                 editRestriction, moveRestriction, revisionid, comment,
                 redirect):
        # TODO: there are more tags we can read.
        self.title = title
        self.id = id
        self.text = text
        self.username = username.strip()
        self.ipedit = ipedit
        self.timestamp = timestamp
        self.editRestriction = editRestriction
        self.moveRestriction = moveRestriction
        self.revisionid = revisionid
        self.comment = comment
        self.isredirect = redirect


class XmlHeaderEntry:
    """
    Represents a header entry
    """
    def __init__(self):
        self.sitename = u''
        self.base = u''
        self.generator = u''
        self.case = u''
        self.namespaces = {}


class MediaWikiXmlHandler(xml.sax.handler.ContentHandler):
    def __init__(self):
        xml.sax.handler.ContentHandler.__init__(self)
        self.inRevisionTag = False
        self.inContributorTag = False
        self.headercallback = None
        # Older Mediawiki version sometimes do not have these elements.
        # They are initialized here so they at least have some value when
        # asked for
        self.id = u''
        self.revisionid = u''
        self.comment = u''
        self.isredirect = False

    def setCallback(self, callback):
        self.callback = callback

    def setHeaderCallback(self, headercallback):
        self.headercallback = headercallback

    def startElement(self, name, attrs):
        self.destination = None
        if name == 'page':
            self.editRestriction = None
            self.moveRestriction = None
            self.ipedit = None
        elif name == 'revision':
            self.inRevisionTag = True
        elif name == 'contributor':
            self.inContributorTag = True
            # username might be deleted
            try:
                if attrs['deleted'] == 'deleted':
                    self.username = u''
            except KeyError:
                pass
        elif name == 'text':
            self.destination = 'text'
            self.text=u''
        elif name == 'id':
            if self.inContributorTag:
                self.destination = 'userid'
                self.userid = u''
            elif self.inRevisionTag:
                self.destination = 'revisionid'
                self.revisionid = u''
            else:
                self.destination = 'id'
                self.id = u''
        elif name == 'username':
            self.destination = 'username'
            self.username = u''
            self.ipedit = False
        elif name == 'ip':
            self.destination = 'username'  # store it in the username
            self.username = u''
            self.ipedit = True
        elif name == 'comment':
            self.destination = 'comment'
            self.comment = u''
        elif name == 'restrictions':
            self.destination = 'restrictions'
            self.restrictions = u''
        elif name == 'title':
            self.destination = 'title'
            self.title=u''
        elif name == 'timestamp':
            self.destination = 'timestamp'
            self.timestamp=u''
        elif self.headercallback:
            if name == 'siteinfo':
                self.header = XmlHeaderEntry()
            elif name in ['sitename', 'base', 'generator', 'case']:
                self.destination = name
            elif name == 'namespace':
                self.destination = 'namespace'
                self.namespace = u''
                self.namespaceid = int(attrs['key'])

    def endElement(self, name):
        if name == 'contributor':
            self.inContributorTag = False
        elif name == 'restrictions':
            self.editRestriction, self.moveRestriction \
                                  = parseRestrictions(self.restrictions)
        elif name == 'redirect':
            self.isredirect = True
        elif name == 'revision':
            # All done for this.
            # Remove trailing newlines and spaces
            text = self.text.rstrip('\n ')
            # Replace newline by cr/nl
            text = u'\r\n'.join(text.split('\n'))
            # Decode the timestamp
            timestamp = (self.timestamp[0:4]+
                         self.timestamp[5:7]+
                         self.timestamp[8:10]+
                         self.timestamp[11:13]+
                         self.timestamp[14:16]+
                         self.timestamp[17:19])
            self.title = self.title.strip()
            # Report back to the caller
            entry = XmlEntry(self.title, self.id,
                             text, self.username,
                             self.ipedit, timestamp,
                             self.editRestriction, self.moveRestriction,
                             self.revisionid, self.comment, self.isredirect)
            self.inRevisionTag = False
            self.callback(entry)
        elif self.headercallback:
            if name == 'namespace':
                self.header.namespaces[self.namespaceid] = self.namespace
            elif name == 'siteinfo':
                self.headercallback(self.header)
                self.header = None
        # Characters between this closing tag and the next opening tag is
        # ignored, it's just whitespace for XML formatting.
        self.destination = None

    def characters(self, data):
        if self.destination == 'text':
            self.text += data
        elif self.destination == 'id':
            self.id += data
        elif self.destination == 'revisionid':
            self.revisionid += data
        elif self.destination == 'comment':
            self.comment += data
        elif self.destination == 'restrictions':
            self.restrictions += data
        elif self.destination == 'title':
            self.title += data
        elif self.destination == 'username':
            self.username += data
        elif self.destination == 'timestamp':
            self.timestamp += data
        elif self.headercallback:
            if self.destination == 'sitename':
                self.header.sitename += data
            elif self.destination == 'base':
                self.header.base += data
            elif self.destination == 'generator':
                self.header.generator += data
            elif self.destination == 'case':
                self.header.case += data
            elif self.destination == 'namespace':
                self.namespace += data


class XmlParserThread(threading.Thread):
    """
    This XML parser will run as a single thread. This allows the XmlDump
    generator to yield pages before the parser has finished reading the
    entire dump.

    There surely are more elegant ways to do this.
    """
    def __init__(self, filename, handler):
        threading.Thread.__init__(self)
        self.filename = filename
        self.handler = handler

    def run(self):
        xml.sax.parse(self.filename, self.handler)


class XmlDump(object):
    """
    Represents an XML dump file. Reads the local file at initialization,
    parses it, and offers access to the resulting XmlEntries via a generator.

    NOTE: This used to be done by a SAX parser, but the solution with regular
    expressions is about 10 to 20 times faster. The cElementTree version is
    again much, much faster than the regex solution.

    @param allrevisions: boolean
        Only available for cElementTree version:
        If True, parse all revisions instead of only the latest one.
        Default: False.
    """
    def __init__(self, filename, allrevisions=False):
        self.filename = filename
        if allrevisions:
            self._parse = self._parse_all
        else:
            self._parse = self._parse_only_latest

    def parse(self):
        """Return a generator that will yield XmlEntry objects"""
        print 'Reading XML dump...'
        if not 'iterparse' in globals():
            pywikibot.output(
u'''WARNING: cElementTree not found. Using slower fallback solution.
Consider installing the python-celementtree package.''')
            return self.regex_parse()
        else:
            return self.new_parse()

    def new_parse(self):
        """Generator using cElementTree iterparse function"""
        if self.filename.endswith('.bz2'):
            import bz2
            source = bz2.BZ2File(self.filename)
        elif self.filename.endswith('.gz'):
            import gzip
            source = gzip.open(self.filename)
        elif self.filename.endswith('.7z'):
            import subprocess
            source = subprocess.Popen('7za e -bd -so %s 2>/dev/null'
                                      % self.filename,
                                      shell=True,
                                      stdout=subprocess.PIPE,
                                      bufsize=65535).stdout
        else:
            # assume it's an uncompressed XML file
            source = open(self.filename)
        context = iterparse(source, events=("start", "end", "start-ns"))
        self.root = None

        for event, elem in context:
            if event == "start-ns" and elem[0] == "":
                self.uri = elem[1]
                continue
            if event == "start" and self.root is None:
                self.root = elem
                continue
            for rev in self._parse(event, elem):
                yield rev

    def _parse_only_latest(self, event, elem):
        """Parser that yields only the latest revision"""
        if event == "end" and elem.tag == "{%s}page" % self.uri:
            self._headers(elem)
            revision = elem.find("{%s}revision" % self.uri)
            yield self._create_revision(revision)
            elem.clear()
            self.root.clear()

    def _parse_all(self, event, elem):
        """Parser that yields all revisions"""
        if event == "start" and elem.tag == "{%s}page" % self.uri:
            self._headers(elem)
        if event == "end" and elem.tag == "{%s}revision" % self.uri:
            yield self._create_revision(elem)
            elem.clear()
            self.root.clear()

    def _headers(self, elem):
        self.title = elem.findtext("{%s}title" % self.uri)
        self.pageid = elem.findtext("{%s}id" % self.uri)
        self.restrictions = elem.findtext("{%s}restrictions" % self.uri)
        self.isredirect = elem.findtext("{%s}redirect" % self.uri) is not None
        self.editRestriction, self.moveRestriction \
                              = parseRestrictions(self.restrictions)


    def _create_revision(self, revision):
        """Creates a Single revision"""
        revisionid = revision.findtext("{%s}id" % self.uri)
        timestamp = revision.findtext("{%s}timestamp" % self.uri)
        comment = revision.findtext("{%s}comment" % self.uri)
        contributor = revision.find("{%s}contributor" % self.uri)
        ipeditor = contributor.findtext("{%s}ip" % self.uri)
        username = ipeditor or contributor.findtext("{%s}username" % self.uri)
        # could get comment, minor as well
        text = revision.findtext("{%s}text" % self.uri)
        return XmlEntry(title=self.title,
                        id=self.pageid,
                        text=text or u'',
                        username=username or u'', #username might be deleted
                        ipedit=bool(ipeditor),
                        timestamp=timestamp,
                        editRestriction=self.editRestriction,
                        moveRestriction=self.moveRestriction,
                        revisionid=revisionid,
                        comment=comment,
                        redirect=self.isredirect
                       )

    def regex_parse(self):
        """
        Generator which reads some lines from the XML dump file, and
        parses them to create XmlEntry objects. Stops when the end of file is
        reached.

        NOTE: This is very slow. It's only a fallback solution for users who
        haven't installed cElementTree.
        """
        Rpage = re.compile(
            '<page>\s*'+
            '<title>(?P<title>.+?)</title>\s*'+
            '<id>(?P<pageid>\d+?)</id>\s*'+
            '(<restrictions>(?P<restrictions>.+?)</restrictions>)?\s*'+
            '<revision>\s*'+
              '<id>(?P<revisionid>\d+?)</id>\s*'+
              '<timestamp>(?P<timestamp>.+?)</timestamp>\s*'+
              '<contributor>\s*'+
                '(<username>(?P<username>.+?)</username>\s*'+
                '<id>(?P<userid>\d+?)</id>|<ip>(?P<ip>.+?)</ip>)\s*'+
              '</contributor>\s*'+
              '(?P<minor>(<minor />))?\s*'+
              '(?:<comment>(?P<comment>.+?)</comment>\s*)?'+
              '(<text xml:space="preserve">(?P<text>.*?)</text>|<text xml:space="preserve" />)\s*'+
            '</revision>\s*'+
            '</page>',
                re.DOTALL)
        f = codecs.open(self.filename, 'r', encoding = pywikibot.getSite().encoding(),
                        errors='replace')
        eof = False
        lines = u''
        while not eof:
            line = f.readline()
            lines += line
            if line == '':
                eof = True
            elif line.endswith(u'</page>\n'):
                # unescape characters
                lines = lines.replace('&gt;', '>')
                lines = lines.replace('&lt;', '<')
                lines = lines.replace('&quot;', '"')
                lines = lines.replace('&amp;', '&')
                m = Rpage.search(lines)
                if not m:
                    print 'ERROR: could not parse these lines:'
                    print lines
                    lines = u''
                else:
                    lines = u''
                    text = m.group('text') or u''
                    restrictions = m.group('restrictions')
                    editRestriction, moveRestriction \
                                     = parseRestrictions(restrictions)

                    if m.group('username'):
                        username = m.group('username')
                        ipedit = False
                    else:
                        username = m.group('ip')
                        ipedit = True
                    yield XmlEntry(title = m.group('title'),
                                   id=m.group('pageid'), text=text,
                                   username=username, ipedit=ipedit,
                                   timestamp=m.group('timestamp'),
                                   editRestriction = editRestriction,
                                   moveRestriction=moveRestriction,
                                   revisionid=m.group('revisionid')
                                  )
