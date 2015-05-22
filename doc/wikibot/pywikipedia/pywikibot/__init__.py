# -*- coding: utf-8  -*-
"""
The initialization file for the Pywikibot framework.
"""
#
# (C) Pywikipedia bot team, 2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: __init__.py 8266 2010-06-09 15:13:25Z xqt $'

import datetime
import difflib

from exceptions import *
from textlib import *
from throttle import *

import wikipedia

class Timestamp(datetime.datetime):
    """Class for handling Mediawiki timestamps.

    This inherits from datetime.datetime, so it can use all of the methods
    and operations of a datetime object.  To ensure that the results of any
    operation are also a Timestamp object, be sure to use only Timestamp
    objects (and datetime.timedeltas) in any operation.

    Use Timestamp.fromISOformat() and Timestamp.fromtimestampformat() to
    create Timestamp objects from Mediawiki string formats.

    Use Site.getcurrenttime() for the current time; this is more reliable
    than using Timestamp.utcnow().

    """
    mediawikiTSFormat = "%Y%m%d%H%M%S"
    ISO8601Format = "%Y-%m-%dT%H:%M:%SZ"

    @classmethod
    def fromISOformat(cls, ts):
        """Convert an ISO 8601 timestamp to a Timestamp object."""
        return cls.strptime(ts, cls.ISO8601Format)

    @classmethod
    def fromtimestampformat(cls, ts):
        """Convert the internal MediaWiki timestamp format to a Timestamp object."""
        return cls.strptime(ts, cls.mediawikiTSFormat)

    def __str__(self):
        """Return a string format recognized by the API"""
        return self.strftime(self.ISO8601Format)

    def __add__(self, other):
        newdt = datetime.datetime.__add__(self, other)
        if isinstance(newdt, datetime.datetime):
            return Timestamp(newdt.year, newdt.month, newdt.day, newdt.hour,
                             newdt.minute, newdt.second, newdt.microsecond,
                             newdt.tzinfo)
        else:
            return newdt

    def __sub__(self, other):
        newdt = datetime.datetime.__sub__(self, other)
        if isinstance(newdt, datetime.datetime):
            return Timestamp(newdt.year, newdt.month, newdt.day, newdt.hour,
                             newdt.minute, newdt.second, newdt.microsecond,
                             newdt.tzinfo)
        else:
            return newdt


def deprecated(instead=None):
    """Decorator to output a method deprecation warning.

    @param instead: if provided, will be used to specify the replacement
    @type instead: string
    """
    def decorator(method):
        def wrapper(*args, **kwargs):
            funcname = method.func_name
            classname = args[0].__class__.__name__
            if instead:
                wikipedia.output(u"%s.%s is DEPRECATED, use %s instead."
                                 % (classname, funcname, instead))
            else:
                wikipedia.output(u"%s.%s is DEPRECATED." % (classname, funcname))
            return method(*args, **kwargs)
        wrapper.func_name = method.func_name
        return wrapper
    return decorator

def deprecate_arg(old_arg, new_arg):
    """Decorator to declare old_arg deprecated and replace it with new_arg"""
    #_logger = ""
    def decorator(method):
        def wrapper(*__args, **__kw):
            meth_name = method.__name__
            if old_arg in __kw:
                if new_arg:
                    if new_arg in __kw:
                        wikipedia.output(
u"%(new_arg)s argument of %(meth_name)s replaces %(old_arg)s; cannot use both."
                            % locals())
                    else:
                        wikipedia.output(
u"%(old_arg)s argument of %(meth_name)s is deprecated; use %(new_arg)s instead."
                            % locals())
                        __kw[new_arg] = __kw[old_arg]
                else:
                    wikipedia.output(
                        u"%(old_arg)s argument of %(meth_name)s is deprecated."
                        % locals())
                del __kw[old_arg]
            return method(*__args, **__kw)
        wrapper.__doc__ = method.__doc__
        wrapper.__name__ = method.__name__
        return wrapper
    return decorator

link_regex = re.compile(r'\[\[(?P<title>[^\]|[#<>{}]*)(\|.*?)?\]\]')


def showDiff(oldtext, newtext):
    """
    Output a string showing the differences between oldtext and newtext.
    The differences are highlighted (only on compatible systems) to show which
    changes were made.

    """
    # This is probably not portable to non-terminal interfaces....
    # For information on difflib, see http://pydoc.org/2.3/difflib.html
    color = {
        '+': 'lightgreen',
        '-': 'lightred',
    }
    diff = u''
    colors = []
    # This will store the last line beginning with + or -.
    lastline = None
    # For testing purposes only: show original, uncolored diff
    #     for line in difflib.ndiff(oldtext.splitlines(), newtext.splitlines()):
    #         print line
    for line in difflib.ndiff(oldtext.splitlines(), newtext.splitlines()):
        if line.startswith('?'):
            # initialize color vector with None, which means default color
            lastcolors = [None for c in lastline]
            # colorize the + or - sign
            lastcolors[0] = color[lastline[0]]
            # colorize changed parts in red or green
            for i in range(min(len(line), len(lastline))):
                if line[i] != ' ':
                    lastcolors[i] = color[lastline[0]]
            diff += lastline + '\n'
            # append one None (default color) for the newline character
            colors += lastcolors + [None]
        elif lastline:
            diff += lastline + '\n'
            # colorize the + or - sign only
            lastcolors = [None for c in lastline]
            lastcolors[0] = color[lastline[0]]
            colors += lastcolors + [None]
        lastline = None
        if line[0] in ('+', '-'):
            lastline = line
    # there might be one + or - line left that wasn't followed by a ? line.
    if lastline:
        diff += lastline + '\n'
        # colorize the + or - sign only
        lastcolors = [None for c in lastline]
        lastcolors[0] = color[lastline[0]]
        colors += lastcolors + [None]

    result = u''
    lastcolor = None
    for i in range(len(diff)):
        if colors[i] != lastcolor:
            if lastcolor is None:
                result += '\03{%s}' % colors[i]
            else:
                result += '\03{default}'
        lastcolor = colors[i]
        result += diff[i]
    wikipedia.output(result)

