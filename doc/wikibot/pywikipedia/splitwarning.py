# -*- coding: utf-8  -*-
"""Splits a interwiki.log file into chunks of warnings separated by language"""
#
# (C) Rob W.W. Hooft, 2003
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: splitwarning.py 7336 2009-09-29 18:27:04Z alexsh $'
#

import wikipedia
import codecs
import re

wikipedia.stopme() # No need to have me on the stack - I don't contact the wiki
files={}
count={}

# TODO: Variable log filename
fn = wikipedia.config.datafilepath("logs", "interwiki.log")
logFile = codecs.open(fn, 'r', 'utf-8')
rWarning = re.compile('WARNING: (?P<family>.+?): \[\[(?P<code>.+?):.*')
for line in logFile:
    m = rWarning.match(line)
    if m:
        family = m.group('family')
        code = m.group('code')
        if code in wikipedia.getSite().languages():
            if not code in files:
                files[code] = codecs.open(
                                  wikipedia.config.datafilepath('logs',
                                         'warning-%s-%s.log' % (family, code)),
                                  'w', 'utf-8')
                count[code] = 0
            files[code].write(line)
            count[code] += 1
for code in files.keys():
    print '*%s (%d)' % (code, count[code])

