# -*- coding: utf-8 -*-
#
# (C) Rob W.W. Hooft, 2003
#     parts by holger@trillke.net 2002/03/18
#     Purodha Blissenbach (Modifier), 2010
# (C) Pywikipedia bot team, 2007-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: config.py 9042 2011-03-13 10:14:47Z xqt $'

import os, re
import sys as __sys
# IMPORTANT:
# Do not change any of the variables in this file. Instead, make
# a file user-config.py, and overwrite values in there.

# Note: all variables defined in this module are made available to bots as
# configuration settings, *except* variable names beginning with an
# underscore (example: _variable).  Be sure to use an underscore on any
# variables that are intended only for internal use and not to be exported
# to other modules.

############## ACCOUNT SETTINGS ##############

# The family of sites we are working on. wikipedia.py will import
# families/xxx_family.py so if you want to change this variable,
# you need to write such a file.
family = 'wikipedia'
# The language code of the site we're working on.
mylang = 'language'

# The dictionary usernames should contain a username for each site where you
# have a bot account. Please set your usernames by adding such lines to your
# user-config.py:
#
# usernames['wikipedia']['de'] = 'myGermanUsername'
# usernames['wiktionary']['en'] = 'myEnglishUsername'
#
# If you have a sysop account on some wikis, this will be used to delete pages
# or to edit locked pages if you add such lines to your
# user-config.py:
#
# sysopnames['wikipedia']['de'] = 'myGermanUsername'
# sysopnames['wiktionary']['en'] = 'myEnglishUsername'
usernames = {}
sysopnames = {}

# See section SOLVE_DISAMBIGUATION SETTINGS for details.
disambiguation_comment = {}
# This is currently not used anywhere:
gdab_namespaces = {}
# This is currently not used anywhere:
account_global = False

# Solve captchas in the webbrowser. Setting this to False will result in the
# exception CaptchaError being thrown if a captcha is encountered.
#TODO: allow more flexibility, such as runtime choices, skipping, and postponing
solve_captcha = True

# Some sites will require password authentication to access the HTML pages at
# the site. If you have any such site, add lines to your user-config.py of
# the following form:
#
# authenticate['en.wikipedia.org'] = ('John','XXXXX')
#
# where John is your login name, and XXXXX your password.
# Note:
# 1. This is only for sites that use authentication in the form that gives
#    you a popup for name and password when you try to access any data, NOT
#    for, for example, wiki usernames
# 2. You must use the hostname of the site, not its family/language pair
authenticate = {}

#    Secure Connection to all Wikimedia Projects
SSL_connection = False

# password_file = ".passwd"
# A password file with default passwords. For more information, please
# see LoginManager.readPassword in login.py.
# By default you are asked for a password on the terminal.
password_file = None

# Login using the API. This is less likely to break.
use_api_login = True

# Enable data recieve from all avalible API.
use_api = True

# Display a warning message if your edits appear in recent changes page
notify_unflagged_bot = True

# Set to True to override the {{bots}} exclusion protocol (at your own risk!)
ignore_bot_templates = False

############## USER INTERFACE SETTINGS ##############

# The encoding that's used in the user's console, i.e. how strings are encoded
# when they are read by raw_input(). On Windows systems' DOS box, this should
# be 'cp850' ('cp437' for older versions). Linux users might try 'iso-8859-1'
# or 'utf-8'.
# This default code should work fine, so you don't have to think about it.
# TODO: consider getting rid of this config variable.
try:
    console_encoding = __sys.stdout.encoding
except:
    #When using pywikipedia inside a daemonized twisted application,
    #we get "StdioOnnaStick instance has no attribute 'encoding'"
    console_encoding = None

# The encoding in which textfiles are stored, which contain lists of page
# titles. The most used is: 'utf-8'. 'utf-8-sig' recognizes BOM but it is
# available on Python 2.5 or higher. For a complete list please see:
# http://docs.python.org/library/codecs.html#standard-encodings
textfile_encoding = 'utf-8'

# tkinter isn't yet ready
userinterface = 'terminal'

# Should we transliterate characters that do not exist in the console
# character set?
# True: whenever possible
# False: never - always replace them by question marks
# Currently only works if interface 'terminal' is set.
transliterate = True

# Should the system bell ring if the bot expects user input?
ring_bell = False

# Colorization can be used to markup important text parts of the output.
# On Linux/Unix terminals, ANSI escape codes are used for this. On Windows,
# it is done by a DLL call via ctypes. ctypes is only available since
# Python 2.5, so if you're using Python 2.4 or lower on Windows, you should
# upgrade.
# Set this to False if you're using Linux and your tty doesn't support
# ANSI colors.
try:
    # Don't print colorized when the output is, for example, piped to a file.
    colorized_output = __sys.stdout.isatty()
except:
    colorized_output = False

# An indication of the size of your screen, or rather the size of the screen
# to be shown, for flickrripper
tkhorsize = 1600
tkvertsize = 1000

############## EXTERNAL EDITOR SETTINGS ##############
# The command for the editor you want to use. If set to None, a simple Tkinter
# editor will be used.
# On Windows systems, this script tries to determine the default text editor.
if __sys.platform == 'win32':
    try:
        import _winreg
        _key1 = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, 'Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.txt\OpenWithProgids')
        _progID = _winreg.EnumValue(_key1, 1)[0]
        _key2 = _winreg.OpenKey(_winreg.HKEY_CLASSES_ROOT, '%s\shell\open\command' % _progID)
        _cmd = _winreg.QueryValueEx(_key2, None)[0]
        editor = _cmd.replace('%1', '')
        # Notepad is even worse than our Tkinter editor.
        # Nobody has deserved to use it.
        if editor.lower().endswith('notepad.exe'):
            editor = None
    except:
        # XXX what are we catching here?
        #raise
        editor = None
else:
    editor = None

# Warning: DO NOT use an editor which doesn't support Unicode to edit pages!
# You will BREAK non-ASCII symbols!
editor_encoding = 'utf-8'

# The temporary file name extension can be set in order to use syntax
# highlighting in your text editor.
editor_filename_extension = 'wiki'

############## LOGFILE SETTINGS ##############

# Defines for which scripts a logfile should be enabled. Logfiles will be
# saved in the 'logs' subdirectory.
# Example:
#     log = ['interwiki', 'weblinkchecker', 'table2wiki']
# It is also possible to enable logging for all scripts, using this line:
#     log = ['*']
# To disable all logging, use this:
#     log = []
# Per default, logging of interwiki.py is enabled because its logfiles can
# be used to generate so-called warnfiles.
# This setting can be overridden by the -log or -nolog command-line arguments.
log = ['interwiki']

############## INTERWIKI SETTINGS ##############

# Should interwiki.py report warnings for missing links between foreign
# languages?
interwiki_backlink = True

# Should interwiki.py display every new link it discovers?
interwiki_shownew = True

# Should interwiki.py output a graph PNG file on conflicts?
# You need pydot for this: http://dkbza.org/pydot.html
interwiki_graph = False

# Specifies that the robot should process that amount of subjects at a time,
# only starting to load new pages in the original language when the total
# falls below that number. Default is to process (at least) 100 subjects at
# once.
interwiki_min_subjects = 100

# If interwiki graphs are enabled, which format(s) should be used?
# Supported formats include png, jpg, ps, and svg. See:
# http://www.graphviz.org/doc/info/output.html
# If you want to also dump the dot files, you can use this in your
# user-config.py:
# interwiki_graph_formats = ['dot', 'png']
# If you need a PNG image with an HTML image map, use this:
# interwiki_graph_formats = ['png', 'cmap']
# If you only need SVG images, use:
# interwiki_graph_formats = ['svg']
interwiki_graph_formats = ['png']

# You can post the contents of your autonomous_problems.dat to the wiki,
# e.g. to http://de.wikipedia.org/wiki/Wikipedia:Interwiki-Konflikte .
# This allows others to assist you in resolving interwiki problems.
# To help these people, you can upload the interwiki graphs to your
# webspace somewhere. Set the base URL here, e.g.:
# 'http://www.example.org/~yourname/interwiki-graphs/'
interwiki_graph_url = None

# Save file with local articles without interwikis.
without_interwiki = False

# Experimental feature:
# Store the page contents on disk (/cache/ directory) instead of loading
# them in RAM.
interwiki_contents_on_disk = False

############## SOLVE_DISAMBIGUATION SETTINGS ############
#
# Set disambiguation_comment[FAMILY][LANG] to a non-empty string to override
# the default edit comment for the solve_disambiguation bot.
# Use %s to represent the name of the disambiguation page being treated.
# Example:
#
# disambiguation_comment['wikipedia']['en'] = \
#    "Robot-assisted disambiguation ([[WP:DPL|you can help!]]): %s"

sort_ignore_case = False

############## IMAGE RELATED SETTINGS ##############
# If you set this to True, images will be uploaded to Wikimedia
# Commons by default.
upload_to_commons = False

############## SETTINGS TO AVOID SERVER OVERLOAD ##############

# Slow down the robot such that it never requests a second page within
# 'minthrottle' seconds. This can be lengthened if the server is slow,
# but never more than 'maxthrottle' seconds. However - if you are running
# more than one bot in parallel the times are lengthened.
minthrottle = 1
maxthrottle = 60

# Slow down the robot such that it never makes a second page edit within
# 'put_throttle' seconds.
put_throttle = 10

# Sometimes you want to know when a delay is inserted. If a delay is larger
# than 'noisysleep' seconds, it is logged on the screen.
noisysleep = 3.0

# Defer bot edits during periods of database server lag.  For details, see
# http://www.mediawiki.org/wiki/Maxlag_parameter
# You can set this variable to a number of seconds, or to None (or 0) to
# disable this behavior. Higher values are more aggressive in seeking
# access to the wiki.
# It is recommended that you do not change this parameter unless you know
# what you are doing and have a good reason for it!
maxlag = 5

# Number of retries before giving up when putting a page
maxretries = 5

# Maximum of pages which can be retrieved by special pages. Increase this if
# you heavily use redirect.py with action "double", and especially if you're
# running solve_disambiguation.py with the -primary argument.
special_page_limit = 500

############## TABLE CONVERSION BOT SETTINGS ##############

# will split long paragraphs for better reading the source.
# only table2wiki.py use it by now
splitLongParagraphs = False
# sometimes HTML-tables are indented for better reading.
# That can do very ugly results.
deIndentTables = True
# table2wiki.py works quite stable, so you might switch to True
table2wikiAskOnlyWarnings = True
table2wikiSkipWarnings = False

############## WEBLINK CHECKER SETTINGS ##############

# How many external links should weblinkchecker.py check at the same time?
# If you have a fast connection, you might want to increase this number so
# that slow servers won't slow you down.
max_external_links = 50

report_dead_links_on_talk = False

############## DATABASE SETTINGS ##############
db_hostname = 'localhost'
db_username = 'wikiuser'
db_password = ''

############## SEARCH ENGINE SETTINGS ##############

# Some scripts allow querying Google via the Google Web API. To use this feature,
# you must install the pyGoogle module from http://pygoogle.sf.net/ and have a
# Google Web API license key. Note that Google doesn't give out license keys
# anymore.
# --------------------
# Google web API is obsoleted for long time, now we can use Google AJAX Search API,
# You can signup an API key from http://code.google.com/apis/ajaxsearch/signup.html.
google_key = ''

# using Google AJAX Search API, it requires the referer website, this variable saves the referer web address
# when you sign up with the key.
google_api_refer = ''

# Some scripts allow using the Yahoo! Search Web Services. To use this feature,
# you must install the pYsearch module from http://pysearch.sourceforge.net/
# and get a Yahoo AppID from http://developer.yahoo.com
yahoo_appid = ''

# To use Windows Live Search web service you must get an AppID from
# http://search.msn.com/developer
msn_appid = ''

# Using the Flickr api
flickr = {
    'api_key': u'',  # Provide your key!
    'review': False,  # Do we use automatically make our uploads reviewed?
    'reviewer': u'', # If so, under what reviewer name?
    }

# Using the Panoramio api
panoramio = {
    'review': False,  # Do we use automatically make our uploads reviewed?
    'reviewer': u'', # If so, under what reviewer name?
    }

# for all connections: proxy handle
# to use it, proxy['host'] have to support HTTP and include port number (e.g. localhost:8080)
# if the proxy server needs authentication, set ('ID', 'PASSWORD') to proxy['auth'].
proxy = {
    'host': None,
    'auth': None,
}

############## COPYRIGHT SETTINGS ##############

# Enable/disable search engine in copyright.py script
copyright_google = True
copyright_yahoo = True
copyright_msn = False

# Perform a deep check, loading URLs to search if 'Wikipedia' is present.
# This may be useful to increase the number of correct results. If you haven't
# a fast connection, you might want to keep them disabled.
copyright_check_in_source_google = False
copyright_check_in_source_yahoo = False
copyright_check_in_source_msn = False

# Web pages may contain a Wikipedia text without the word 'Wikipedia' but with
# the typical '[edit]' tag as a result of a copy & paste procedure. You want
# no report for this kind of URLs, even if they are copyright violations.
# However, when enabled, these URLs are logged in a file.
copyright_check_in_source_section_names = False

# Limit number of queries for page.
copyright_max_query_for_page = 25

# Skip a specified number of queries
copyright_skip_query = 0

# Number of attempts on connection error.
copyright_connection_tries = 10

# Behavior if an exceeded error occur.
#
# Possibilities:
#
#    0 = None
#    1 = Disable search engine
#    2 = Sleep (default)
#    3 = Stop
copyright_exceeded_in_queries = 2
copyright_exceeded_in_queries_sleep_hours = 6

# Append last modified date of URL to script result
copyright_show_date = True

# Append length of URL to script result
copyright_show_length = True

# By default the script tries to identify and skip text that contains a large
# comma separated list or only numbers. But sometimes that might be the
# only part unmodified of a slightly edited and not otherwise reported
# copyright violation. You can disable this feature to try to increase the
# number of results.
copyright_economize_query = True

############## HTTP SETTINGS ##############
# Use a persistent http connection. An http connection has to be established
# only once per site object, making stuff a whole lot faster. Do NOT EVER
# use this if you share Site objects across threads without proper locking.
## DISABLED FUNCTION. Setting this variable will not have any effect.
persistent_http = False

# Default socket timeout. Set to None to disable timeouts.
socket_timeout = 120  # set a pretty long timeout just in case...


############## FURTHER SETTINGS ##############
# The bot can make some additional changes to each page it edits, e.g. fix
# whitespace or positioning of interwiki and category links.

# This is an experimental feature; handle with care and consider re-checking
# each bot edit if enabling this!
cosmetic_changes = False

# If cosmetic changes are switched on, and you also have several accounts at
# projects where you're not familiar with the local conventions, you probably
# only want the bot to do cosmetic changes on your "home" wiki which you
# specified in config.mylang and config.family.
# If you want the bot to also do cosmetic changes when editing a page on a
# foreign wiki, set cosmetic_changes_mylang_only to False, but be careful!
cosmetic_changes_mylang_only = True
# The dictionary cosmetic_changes_enable should contain a tuple of languages
# for each site where you wish to enable in addition to your own langlanguage
# (if cosmetic_changes_mylang_only is set)
# Please set your dictionary by adding such lines to your user-config.py:
# cosmetic_changes_enable['wikipedia'] = ('de', 'en', 'fr')
cosmetic_changes_enable = {}
# The dictionary cosmetic_changes_disable should contain a tuple of languages
# for each site where you wish to disable cosmetic changes. You may use it with
# cosmetic_changes_mylang_only is False, but you can also disable your own
# language. This also overrides the settings in the cosmetic_changes_enable
# dictionary. Please set your dict by adding such lines to your user-config.py:
# cosmetic_changes_disable['wikipedia'] = ('de', 'en', 'fr')
cosmetic_changes_disable = {}
# Use the experimental disk cache to prevent huge memory usage
use_diskcache = False

# Retry loading a page on failure (back off 1 minute, 2 minutes, 4 minutes
# up to 30 minutes)
retry_on_fail = True

# How many pages should be put to a queue in asynchroneous mode.
# If maxsize is <= 0, the queue size is infinite.
# Increasing this value will increase memory space but could speed up
# processing. As higher this value this effect will decrease.
max_queue_size = 64

# End of configuration section
# ============================

def makepath(path):
    """Return a normalized absolute version of the path argument.

    - if the given path already exists in the filesystem
      the filesystem is not modified.

    - otherwise makepath creates directories along the given path
      using the dirname() of the path. You may append
      a '/' to the path if you want it to be a directory path.

    from holger@trillke.net 2002/03/18

    """
    from os import makedirs
    from os.path import normpath, dirname, exists, abspath

    dpath = normpath(dirname(path))
    if not exists(dpath): makedirs(dpath)
    return normpath(abspath(path))

def datafilepath(*filename):
    """Return an absolute path to a data file in a standard location.

    Argument(s) are zero or more directory names, optionally followed by a
    data file name. The return path is offset to config.base_dir. Any
    directories in the path that do not already exist are created.

    """
    import os
    return makepath(os.path.join(base_dir, *filename))

def shortpath(path):
    """Return a file path relative to config.base_dir."""
    import os
    if path.startswith(base_dir):
        return path[len(base_dir) + len(os.path.sep) : ]
    return path

# is config verbose?
_verbose = False
for _arg in __sys.argv[1:]:
    if _arg == "-v" or _arg == "-verbose":
        _verbose = True
        break
if _verbose:
    print "Config.py"

# Get the names of all known families, and initialize
# with empty dictionaries
import wikipediatools as _wt
_base_dir = _wt.get_base_dir()
_RfamilyFile = re.compile('(?P<name>.+)_family.py$')
for _filename in os.listdir(os.path.join(_base_dir, 'families')):
    _m = _RfamilyFile.match(_filename)
    if _m:
        familyName = _m.group('name')
        usernames[familyName] = {}
        sysopnames[familyName] = {}
        disambiguation_comment[familyName] = {}
# System-level and User-level changes.
# Store current variables and their types.
_glv = {}
_glv.update(globals())
_gl = _glv.keys()
_tp = {}
for _key in _gl:
    if _key[0] != '_':
        _tp[_key] = type(globals()[_key])

# Get the user files
_thislevel = 0
_fns = [os.path.join(_base_dir, "user-config.py")]
for _filename in _fns:
    _thislevel += 1
    if os.path.exists(_filename):
        _filestatus = os.stat(_filename)
        _filemode = _filestatus[0]
        _fileuid = _filestatus[4]
        if __sys.platform == 'win32' or _fileuid in [os.getuid(), 0]:
            if __sys.platform == 'win32' or _filemode & 002 == 0:
                execfile(_filename)
            else:
                print "WARNING: Skipped '%s': writeable by others."%_filename
        else:
            print "WARNING: Skipped '%s': owned by someone else."%_filename

# Test for obsoleted and/or unknown variables.
for _key, _val in globals().items():
    if _key.startswith('_'):
        pass
    elif _key in _gl:
        nt = type(_val)
        ot = _tp[_key]
        if nt == ot or _val is None or ot == type(None):
            pass
        elif nt is int and (ot is float or ot is bool):
            pass
        elif ot is int and (nt is float or nt is bool):
            pass
        else:
            print "WARNING: Type of '%s' changed"%_key
            print "       Was: ",ot
            print "       Now: ",nt
        del nt, ot
    else:
        print "WARNING: Configuration variable %r is defined but unknown. Misspelled?" %_key

# Fix up default console_encoding
if console_encoding is None:
    if __sys.platform == 'win32':
        console_encoding = 'cp850'
    else:
        console_encoding = 'iso-8859-1'

# Save base_dir for use by other modules
base_dir = _base_dir
if _verbose:
    print "- base_dir: ", base_dir

# Exit message
if _verbose:
    print "- done."

#
# When called as main program, list all configuration variables
#
if __name__ == "__main__":
    import types
    _all = 1
    for _arg in __sys.argv[1:]:
        if _arg == "modified":
            _all = 0
        elif _arg == "-v":
            pass
        elif _arg == "-verbose":
            pass
        elif _arg.startswith("-dir:"):
            pass
        else:
            print "Unknown arg %s ignored"%_arg
    _k = globals().keys()
    _k.sort()
    for _name in _k:
        if _name[0] != '_':
            if not type(globals()[_name]) in [types.FunctionType, types.ModuleType]:
                try:
                    if _all or _glv[_name] != globals()[_name]:
                        print _name, "=", repr(globals()[_name])
                except KeyError:
                    print _name, "=(new)=", repr(globals()[_name])

# cleanup all locally-defined variables
for __var in globals().keys():
    if __var.startswith("_") and not __var.startswith("__"):
        del __sys.modules[__name__].__dict__[__var]

del __var, __sys
del os, re

