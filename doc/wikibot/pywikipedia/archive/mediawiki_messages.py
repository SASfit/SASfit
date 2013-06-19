# -*- coding: utf-8 -*-
"""
Allows access to the MediaWiki messages, that's the label texts of the MediaWiki
software in the current language. These can be used in other bots.

The function refresh_messages() downloads all the current messages and saves
them to disk. It is run automatically when a bot first tries to access one of
the messages. It can be updated manually by running this script, e.g. when
somebody changed the current message at the wiki. The texts will also be
reloaded automatically once a month.

Syntax: python mediawiki_messages [-all]

Command line options:
    -refresh - Reloads messages for the home wiki or for the one defined via
               the -lang and -family parameters.

    -all     - Reloads messages for all wikis where messages are already present

    If another parameter is given, it will be interpreted as a MediaWiki key.
    The script will then output the respective value, without refreshing..
    
"""

# (C) Daniel Herding, 2004
#
# Distributed under the terms of the MIT license.

##THIS MODULE IS DEPRECATED AND HAS BEEN REPLACED BY NEW FUNCTIONALITY IN
##WIKIPEDIA.PY.  It is being retained solely for compatibility in case any
##custom-written bots rely upon it.  Bot authors should replace any uses
##of this module as follows:
##
##    OLD:    mediawiki_messages.get(key, site)
##    NEW:    site.mediawiki_message(key)
##
##    OLD:    mediawiki_messages.has(key, site)
##    NEW:    site.has_mediawiki_message(key)
##
##    OLD:    mediawiki_messages.makepath(path)
##    NEW:    wikipedia.makepath(path)
##
##########################################################################

import warnings
warnings.warn(
"""The mediawiki_messages module is deprecated and no longer
maintained; see the source code for new methods to replace
calls to this module.""",
            DeprecationWarning, stacklevel=2)


import wikipedia
import re, sys, pickle
import os.path
import time
import codecs
import urllib
from BeautifulSoup import *

__version__='$Id: mediawiki_messages.py 3731 2007-06-20 14:42:55Z russblau $'

loaded = {}

def get(key, site = None, allowreload = True):
    site = site or wikipedia.getSite()
    if site in loaded:
        # Use cached copy if it exists.
        dictionary = loaded[site]
    else:
        fn = 'mediawiki-messages/mediawiki-messages-%s-%s.dat' % (site.family.name, site.lang)
        try:
            # find out how old our saved dump is (in seconds)
            file_age = time.time() - os.path.getmtime(fn)
            # if it's older than 1 month, reload it
            if file_age > 30 * 24 * 60 * 60:
                print 'Current MediaWiki message dump is one month old, reloading'
                refresh_messages(site)
        except OSError:
            # no saved dumped exists yet
            refresh_messages(site)
        f = open(fn, 'r')
        dictionary = pickle.load(f)
        f.close()
        loaded[site] = dictionary
    key = key[0].lower() + key[1:]
    if key in dictionary:
        return dictionary[key]
    elif allowreload:
        refresh_messages(site = site)
        return get(key, site = site, allowreload = False)
    else:
        raise KeyError('MediaWiki Key %s not found' % key)

def has(key, site = None, allowreload = True):
    try:
        get(key, site, allowreload)
        return True
    except KeyError:
        return False

def makepath(path):
    """ creates missing directories for the given path and
        returns a normalized absolute version of the path.

    - if the given path already exists in the filesystem
      the filesystem is not modified.

    - otherwise makepath creates directories along the given path
      using the dirname() of the path. You may append
      a '/' to the path if you want it to be a directory path.

    from holger@trillke.net 2002/03/18
    """
    from os import makedirs
    from os.path import normpath,dirname,exists,abspath

    dpath = normpath(dirname(path))
    if not exists(dpath): makedirs(dpath)
    return normpath(abspath(path))
    
def refresh_messages(site = None):
    site = site or wikipedia.getSite()
    # get 'all messages' special page's path
    path = site.allmessages_address()
    print 'Retrieving MediaWiki messages for %s' % repr(site)
    wikipedia.put_throttle() # It actually is a get, but a heavy one.
    allmessages = site.getUrl(path)

    print 'Parsing MediaWiki messages'
    soup = BeautifulSoup(allmessages,
                         convertEntities=BeautifulSoup.HTML_ENTITIES)
    # The MediaWiki namespace in URL-encoded format, as it can contain
    # non-ASCII characters and spaces.
    quotedMwNs = urllib.quote(site.namespace(8).replace(' ', '_').encode(site.encoding()))
    mw_url = site.path() + "?title=" + quotedMwNs + ":"
    altmw_url = site.path() + "/" + quotedMwNs + ":"
    nicemw_url = site.nice_get_address(quotedMwNs + ":")
    shortmw_url = "/" + quotedMwNs + ":"
    ismediawiki = lambda url:url and (url.startswith(mw_url)
                                      or url.startswith(altmw_url)
                                      or url.startswith(nicemw_url)
                                      or url.startswith(shortmw_url))
    # we will save the found key:value pairs here
    dictionary = {}

    try:
        for keytag in soup('a', href=ismediawiki):
            # Key strings only contain ASCII characters, so we can save them as
            # strs
            key = str(keytag.find(text=True))
            keyrow = keytag.parent.parent
            if keyrow['class'] == "orig":
                valrow = keyrow.findNextSibling('tr')
                assert valrow['class'] == "new"
                value = unicode(valrow.td.string).strip()
            elif keyrow['class'] == 'def':
                value = unicode(keyrow('td')[1].string).strip()
            else:
                raise AssertionError("Unknown tr class value: %s" % keyrow['class'])
            dictionary[key] = value
    except Exception, e:
        wikipedia.debugDump( 'MediaWiki_Msg', site, u'%s: %s while processing URL: %s' % (repr(e), str(e), unicode(path)), allmessages)
        raise

    # Save the dictionary to disk
    # The file is stored in the mediawiki_messages subdir. Create if necessary. 
    if dictionary == {}:
        wikipedia.debugDump( 'MediaWiki_Msg', site, u'Error URL: '+unicode(path), allmessages )
        sys.exit()
    else:
        f = open(makepath('mediawiki-messages/mediawiki-messages-%s-%s.dat' % (site.family.name, site.lang)), 'w')
        pickle.dump(dictionary, f)
        f.close()
    print "Loaded %i values from %s" % (len(dictionary.keys()), site)
    #print dictionary['sitestatstext']

def refresh_all_messages():
    import dircache, time
    filenames = dircache.listdir('mediawiki-messages')
    message_filenameR = re.compile('mediawiki-messages-([a-z:]+)-([a-z:]+).dat')
    for filename in filenames:
        match = message_filenameR.match(filename)
        if match:
            family = match.group(1)
            lang = match.group(2)
            site = wikipedia.getSite(code = lang, fam = family)
            refresh_messages(site)

def main():
    refresh_all = False
    refresh = False
    key = None
    for arg in wikipedia.handleArgs():
        if arg == '-all':
            refresh_all = True
        elif arg == '-refresh':
            refresh = True
        else:
            key = arg
    if key:
        wikipedia.output(get(key), toStdout = True)
    elif refresh_all:
        refresh_all_messages()
    elif refresh:
        refresh_messages(wikipedia.getSite())
    else:
        wikipedia.showHelp('mediawiki_messages')

if __name__ == "__main__":
    try:
        main()
    except:
        wikipedia.stopme()
        raise
    else:
        wikipedia.stopme()

