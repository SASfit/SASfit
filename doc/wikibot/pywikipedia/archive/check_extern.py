# -*- coding: utf-8  -*-
"""
This bot is used for checking external links from Wikipedia. It checks
all external links in groups of 480 pages, gives the error code for each
that causes problems, and counts the number of links with and without
problems.

It accepts all general Wikipediabot arguments as well as:
-start:xxx  Check starting at 'xxx'.
-nolog      Do not log to a file, only give output to a screen.

Anything else is assumed to be a page that is to be checked. Spaces in
page titles have to be replaced by underscores, otherwise the bot assumes
the parts are separate pages. If no page has been specified and also no
-start argument has been provided, the bot acts as if -start:! had been
specified, starting at the beginning.

The bot returns all links that have some problem, with the errorcode
provided by the server, or the artificial errorcode -1 if the server
could not be reached at all. Output is sent both to the screen and the
file check_extern.txt
"""

#
# (C) Andre Engels, 2004
#
# Distributed under the terms of the MIT license.
#

__version__='$Id: check_extern.py,v 1.16 2005/12/21 17:51:26 wikipedian Exp $'

import wikipedia, urllib, re, sys, httplib

class URLerrorFinder(urllib.FancyURLopener):
    version="RobHooftWikiRobot/1.0"
    def open_http(self, url):
        """Use HTTP protocol."""
        if isinstance(url, str):
            host, selector = urllib.splithost(url)
            if host:
                user_passwd, host = urllib.splituser(host)
                host = urllib.unquote(host)
            realhost = host
        else:
            host, selector = url
            urltype, rest = urllib.splittype(selector)
            url = rest
            user_passwd = None
            if urltype.lower() != 'http':
                realhost = None
            else:
                realhost, rest = splithost(rest)
                if realhost:
                    user_passwd, realhost = splituser(realhost)
                if user_passwd:
                    selector = "%s://%s%s" % (urltype, realhost, rest)
                if proxy_bypass(realhost):
                    host = realhost
        if not host: return -2
        h = httplib.HTTP(host)
        h.putrequest('GET', selector)
        if realhost: h.putheader('Host', realhost)
        for args in self.addheaders: h.putheader(*args)
        h.endheaders()
        errcode, errmsg, headers = h.getreply()
        return errcode

# Which error codes do we not consider errors? 
allowederrorcodes = [100,101,200,201,202,203,205,304]

errname = {
    -1:'No contact to server',
    -2:'No host found',
    100:'Continue',
    101:'Switching Protocols',
    200:'OK',
    201:'Created',
    202:'Accepted',
    203:'Non-Authorative Information',
    204:'No Content',
    205:'Reset Content',
    206:'Partial Content',
    300:'Multiple Choices',
    301:'Moved Permanently',
    302:'Moved Temporarily',
    303:'See Other',
    304:'Not Modified',
    305:'Use Proxy',
    307:'Temporary Redirect',
    400:'Bad Request',
    401:'Unauthorized',
    402:'Payment Required',
    403:'Forbidden',
    404:'Not Found',
    405:'Method Not Allowed',
    406:'None Acceptable',
    407:'Proxy Authentication Required',
    408:'Request Timeout',
    409:'Conflict',
    410:'Gone',
    411:'Authorization Refused',
    412:'Precondition Failed',
    413:'Request Entity Too Large',
    414:'Request-URI Too Large',
    415:'Unsupported Media Type',
    416:'Requested Range not satisfiable',
    417:'Expectation Failed',
    500:'Internal Server Error',
    501:'Not Implemented',
    502:'Bad Gateway',
    503:'Service Unavailable',
    504:'Gateway Timeout',
    505:'HTTP Version not supported',
    8181:'Certificate Expired',
    12002:'Timeout',
    12007:'No such host',
    12029:'No connection',
    12031:'Connection Reset'
 }

def errorname(error):
    # Given a numerical HTML error, give its actual identity
    if error in errname:
        return errname[error]
    elif (error > 300) and (error < 400):
        return 'Unknown Redirection Response'
    else:
        return 'Unknown Error'
    
start = '!'
log = True
todo = []
do_all = False

for arg in sys.argv[1:]:
    url=sys.argv[1]
    arg = wikipedia.argHandler(arg, 'check_extern')
    if arg:
        if arg.startswith('-start:'):
            start=arg[7:]
            do_all=True
        elif arg=='-nolog':
            log = False
        else:
            mysite = wikipedia.getSite()
            todo.append(wikipedia.Page(mysite,arg))

# Make sure we have the final site
mysite = wikipedia.getSite()

if todo == []:
    # No pages have been given; if also no start is given, we start at
    # the beginning
    do_all = True

if log:
    import logger
    sys.stdout = logger.Logger(sys.stdout, filename = 'check_extern.log')

cont = True
checked = 0
working = 0
nonworking = 0
totalchecked = 0

try:
    while cont:
        print
        i = 0
        if len(todo)<61 and do_all:
            for pl in wikipedia.allpages(start = start):
                todo.append(pl)
                i += 1
                if i==480:
                    break
            start = todo[len(todo)-1].title() + '_0'
        # todo is a list of pages to do, donow are the pages we will be doing in this run.
        if len(todo)>60:
            # Take the first 60.
            donow = todo[0:60]
            todo = todo[60:]
        else:
            donow = todo
            # If there was more to do, the 'if len(todo)<61' part would have extended
            # todo beyond this size.
            cont = False
        try:
            wikipedia.getall(mysite, donow)
        except wikipedia.SaxError:
            # Ignore this error, and get the pages the traditional way.
            pass
        checked +=len(donow)
        for pl in donow:
            R = re.compile(r'http://[^\s}<\]]+[^\s.,:;)\?!\]}<]')
            try:
                for url in R.findall(pl.get()):
                    url = wikipedia.unicode2html(url,'ascii')
                    try:
                        error = URLerrorFinder().open(url)
                    except IOError:
                        error = -1
                    if error in allowederrorcodes:
                        working += 1
                    else:
                        nonworking += 1
                        print
                        wikipedia.output(u'Page "%s" links to:'%pl.title())
                        wikipedia.output(url)
                        wikipedia.output(u'Which gave error: %s %s'%(error,errorname(error)))
            # If anything is wrong with the Wikipedia page, just ignore
            except (wikipedia.NoPage,wikipedia.IsRedirectPage,wikipedia.LockedPage):
                pass
        if checked>499 or not cont:
            totalchecked += 500
            checked -= 500
            print
            print '======================================================================'
            wikipedia.output(u'%s pages checked, last was [[%s]]'%(totalchecked+checked,donow[len(donow)-1]))
            print 'In those pages there were %s correct and %s problematic external links.'%(working,nonworking)
except:
    wikipedia.stopme()
    raise
wikipedia.stopme()
