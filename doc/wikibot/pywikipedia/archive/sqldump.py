# -*- coding: utf-8  -*-
"""
Reads a cur SQL dump and offers a generator over SQLentry objects which can be
used by other bots. Each SQLentry object represents a page.

Can also be run directly from the command line to retrieve page lists from
an SQL dump.

Syntax:

    python sqldump.py -sql:filename.sql action

Where action can be one of these:

* find               - List pages which contain a certain text
* findr              - List pages containing text matching a regular expression
* shortpages         - List pages with short contents
* unmountedcats      - List categories that don't have a supercategory
* percentnames       - List pages that contain internal links where special
                       characters are encoded as hexadecimal codes, e.g. %F6
* baddisambiguations - Created for de.wikipedia to fix primary topic
                       disambiguations (Begriffsklärung nach Modell 2).
"""
#
# (C) Daniel Herding, 2004
#
# Distributed under the terms of the MIT license.
# 

__version__ = '$Id:'

from __future__ import generators
import re, time
import wikipedia, config


class SQLentry(object):
    '''
    Represents a wiki page, read from an SQL dump. 
    
    An instance of this class will have the following attributes:
    * self.id is the page ID (integer)
    * self.namespace is the namespace ID (integer)
    * self.title is the page title without namespace (unicode)
    * self.text is the text on that page (unicode)
    * self.comment is the last edit summary (unicode)
    * self.userid is the last editor's ID (integer)
    * self.username is the last editor's username (unicode)
    * self.timestamp is the time of the last edit (time tuple)
    * self.restrictions is True if the page is locked (boolean)
    * self.counter is the # of page views, disabled on Wikimedia wikis (integer)
    * self.redirect is True if the page is a redirect (boolean)
    * self.minor is True if the last edit was marked as minor (boolean)
    * self.new is True if the last edit was the first one (boolean)
    * self.random is a random number used for the 'Random Page' function (float)
    * self.touched is the date of the last cache update (time tuple)

    See http://meta.wikimedia.org/wiki/Cur_table for details.
    '''

    def __init__(self, id, namespace, title, text, comment, userid, username, timestamp, restrictions, counter, redirect, minor, new, random, inversetimestamp, touched):
        '''
        Constructor. All parameters should be strings, as read from the SQL
        dump. This function will convert them to formats which are more
        appropriate for the data types.
        '''
        self.id = int(id)
        self.namespace = int(namespace)
        self.title = title
        self.text = text
        self.comment = comment
        self.userid = int(userid)
        self.username = username
        # convert to a 9-dimensional time tuple, see http://python.org/doc/2.3.4/lib/module-time.html
        self.timestamp = time.strptime(timestamp, '%Y%m%d%H%M%S')
        # convert to boolean
        self.restrictions = (restrictions != '')
        self.counter = int(counter)
        self.redirect = (redirect == '1')
        self.minor = (minor == '1')
        self.new = (new == '1')
        self.random = float(random)
        # Inversetimestamp is obsolete, so we ignore it.
        #self.inversetimestamp = inversetimestamp
        
        # Basically, I would want to convert touched to time tuple, as I did
        # with timestamp. But I noticed a problem: in the nds: dump touched
        # comes before inversetimestamp, and that would cause strptime to crash.
        # So we simply leave touched as it is and hope that this is the only
        # exception where entries are mixed up. If you find other such cases,
        # please report.
        #self.touched = time.strptime(touched, '%Y%m%d%H%M%S')
        self.touched = touched

        # MediaWiki escapes apostrophes, backslashes and quotes with
        # backslashes. We need to unescape them again.
        # This regular expression matches a backslash followed by a group, where
        # the group matches either an apostrophe, a backslashes or a quote.
        escapedR = re.compile(r'\\([\\\"\'])')
        # The group \1 is the character we really want, while the leading
        # backslash is only escape information we don't need.
        self.title = escapedR.sub(r"\1", self.title)
        self.text = escapedR.sub(r"\1", self.text)
        self.comment = escapedR.sub(r"\1", self.comment)
        self.username = escapedR.sub(r"\1", self.username)
        
        # convert \n and \r to newlines and carriage returns.
        self.text = self.text.replace('\\r', '\r')
        self.text = self.text.replace('\\n', '\n')
        # comments can also contain newline characters 
        self.comment = self.comment.replace('\\r', '\r')
        self.comment = self.comment.replace('\\n', '\n')
        # I hope that titles and usernames can't :-)

    def full_title(self, underline = True):
        '''
        Returns the full page title in the form 'namespace:title', using the
        localized namespace titles defined in your family file.
        If underline is True, returns the page title with underlines instead of
        spaces.
        '''
        if not underline:
            title = self.title.replace('_', ' ')
        else:
            title = self.title
        namespace_title = wikipedia.getSite().namespace(self.namespace)
        if namespace_title == None:
            return self.title
        else:
            if underline:
                namespace_title = namespace_title.replace(' ', '_') 
            return namespace_title + ':' + self.title

    def age(self):
        '''
        Returns the time passed since the last edit, in relation to the current
        system time, in seconds (floating point number).
        '''
        return time.time() - time.mktime(self.timestamp)

# Represents one parsed SQL dump file. Reads the local file at initialization,
# parses it with a regular expression, and offers access to the resulting
# SQLentry objects through the entries() generator.
class SQLdump(object):
    def __init__(self, filename, encoding):
        self.filename = filename
        self.encoding = encoding
    
    def entries(self):
        '''
        Generator which reads one line at a time from the SQL dump file, and
        parses it to create SQLentry objects. Stops when the end of file is
        reached.
        '''
        # This regular expression will match one SQL database entry (i.e. a
        # page), and each group represents an attribute of that entry.
        # NOTE: We don't need re.DOTALL because newlines are escaped.
        pageR = re.compile("\((\d+),"      # cur_id             (page ID number)
                         + "(\d+),"        # cur_namespace      (namespace number)
                         + "'(.*?)',"      # cur_title          (page title w/o namespace)
                         + "'(.*?)',"      # cur_text           (page contents)
                         + "'(.*?)',"      # cur_comment        (last edit's summary text)
                         + "(\d+),"        # cur_user           (user ID of last contributor)
                         + "'(.*?)',"      # cur_user_text      (user name)
                         + "'(\d{14})',"   # cur_timestamp      (time of last edit)
                         + "'(.*?)',"      # cur_restrictions   (protected pages have 'sysop' here)
                         + "(\d+),"        # cur_counter        (view counter, disabled on WP)
                         + "([01]),"       # cur_is_redirect
                         + "([01]),"       # cur_minor_edit
                         + "([01]),"       # cur_is_new
                         + "([\d\.]+?)," # cur_random         (for random page function)
                         + "'(\d{14})',"   # inverse_timestamp  (obsolete)
                         + "'(\d{14})'\)") # cur_touched        (cache update timestamp)
        print 'Reading SQL dump'
        # Open the file, read it using the given encoding, and replace invalid
        # characters with question marks.
        import codecs
        f=codecs.open(self.filename, 'r', encoding = self.encoding, errors='replace')
        eof = False
        while not eof:
            # Read only one (very long) line because we would risk out of memory
            # errors if we read the entire file at once
            line = f.readline()
            if line == '':
                print 'End of file.'
                eof = True
            self.entries = []
            for id, namespace, title, text, comment, userid, username, timestamp, restrictions, counter, redirect, minor, new, random, inversetimestamp, touched in pageR.findall(line):
                 new_entry = SQLentry(id, namespace, title, text, comment, userid, username, timestamp, restrictions, counter, redirect, minor, new, random, inversetimestamp, touched)
                 yield new_entry
        f.close()

    def query_percentnames(self):
        '''
        yields pages that contain internal links where special characters are
        encoded as hexadecimal codes, e.g. %F6
        '''
        Rpercentlink = re.compile('\[\[[^\]]*?%[A-F0-9][A-F0-9][^\]]*?\]\]')
        for entry in self.entries():
            text = wikipedia.removeLanguageLinks(entry.text)
            if Rpercentlink.search(text):
                yield entry
    
    def query_shortpages(self, minsize):
        '''
        yields articles that have less than minsize bytes of text
        '''
        for entry in self.entries():
            if entry.namespace == 0 and not entry.redirect and len(entry.text) < minsize:
                yield entry
    
    def query_find(self, keyword):
        '''
        yields pages which contain keyword
        '''
        for entry in self.entries():
            if entry.text.find(keyword) != -1:
                yield entry
    
    def query_findr(self, regex, namespace = None):
        '''
        yields pages which contain a string matching the given regular expression
        '''
        r = re.compile(regex)
        for entry in self.entries():
            if r.search(entry.text) and (namespace == None or entry.namespace == namespace):
                yield entry
    
    def query_unmountedcats(self):
        '''
        yields categories which don't have any supercategory
        '''
        for entry in self.entries():
            if entry.namespace == 14:
                has_supercategory = False
                for ns in wikipedia.getSite().category_namespaces():
                    if entry.text.find('[[%s:' % ns) != -1:
                        has_supercategory = True
                        break
                if not has_supercategory:
                    yield entry
            
def query(sqldump, action):
    if action == 'percentnames':
        for entry in sqldump.query_percentnames():
            yield entry
    elif action == 'shortpages':
        minsize = int(wikipedia.input(u'Minimum size:'))
        for entry in sqldump.query_shortpages(minsize):
            yield entry
    elif action == 'find':
        keyword = wikipedia.input(u'Search for:')
        for entry in sqldump.query_find(keyword):
            yield entry
    elif action == 'findr':
        keyword = wikipedia.input(u'Search for:')
        for entry in sqldump.query_findr(keyword):
            yield entry
    elif action == 'unmountedcats':
        for entry in sqldump.query_unmountedcats():
            yield entry
    elif action == 'baddisambiguation':
        for entry in sqldump.entries():
            if entry.namespace == 0 and entry.title.endswith(')') and entry.text.startswith("''") and not entry.text.startswith("'''"):
                yield entry

if __name__=="__main__":
    wikipedia.stopme() # No need to have me on the stack, as I'm not contacting the wiki
    import sys
    action = None
    filename = None
    for arg in sys.argv[1:]:
        arg = wikipedia.argHandler(arg, 'sqldump')
        if arg:
            if arg.startswith('-sql'):
                if len(arg) == 4:
                    filename = wikipedia.input(u'Please enter the SQL dump\'s filename: ')
                else:
                    filename = arg[5:]
            else:
                action = arg
    if not filename or not action:
        wikipedia.output(__doc__, 'utf-8')
    else:
        sqldump = SQLdump(filename, wikipedia.myencoding())
        
        for entry in query(sqldump, action):
            wikipedia.output(u'*[[%s]]' % entry.full_title())
    
