#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""
A simple IRC script to check for Recent Changes through IRC,
and to check for interwikis in those recently modified articles.

Can not be run manually/directly, but automatically by maintainer.py

In use on hu:, not sure if this scales well on a large wiki such
as en: (Depending on the edit rate, the number of IW threads
could grow continuously without ever decreasing)

Params:

-safe  Does not handle the same page more than once in a session

Warning: experimental software, use at your own risk
"""

# Authors: Kisbes
# http://hu.wikipedia.org/wiki/User:Kisbes
# License : GFDL
#
# (C) Pywikipedia bot team, 2008, 2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: rciw.py 8629 2010-10-09 16:11:46Z xqt $'
#

import threading
import re
import time
from Queue import Queue
import wikipedia as pywikibot
import interwiki

class IWRCBot():
    def __init__(self, site, safe = True):
        self.other_ns = re.compile(u'14\[\[07(' + u'|'.join(site.namespaces()) + u')')
        interwiki.globalvar.autonomous = True
        self.site = site
        self.queue = Queue()
        self.processed = []
        self.safe = safe
        # Start 20 threads
        for i in range(20):
            t = threading.Thread(target=self.worker)
            t.setDaemon(True)
            t.start()

    def worker(self):
        bot = interwiki.InterwikiBot()
        while True:
            # Will wait until one page is available
            bot.add(self.queue.get())
            bot.queryStep()
            self.queue.task_done()

    def addQueue(self, name):
        if self.other_ns.match(name):
            return
        if self.safe:
            if name in self.processed:
                return
            self.processed.append(name)
        page = pywikibot.Page(self.site, name)
        # the Queue has for now an unlimited size,
        # it is a simple atomic append(), no need to acquire a semaphore
        self.queue.put_nowait(page)

def main():
    pywikibot.output('Warning: this script can not be run manually/directly, but automatically by maintainer.py')

if __name__ == "__main__":
    main()
