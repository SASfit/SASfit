#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""
The controller bot for maintainer.py
Exactly one instance should be running of it. To check, use /whois maintcont on irc.freenode.net

This script requires the Python IRC library http://python-irclib.sourceforge.net/

Warning: experimental software, use at your own risk
"""
__version__ = '$Id: maintcont.py 7336 2009-09-29 18:27:04Z alexsh $'

# Author: Balasyum
# http://hu.wikipedia.org/wiki/User:Balasyum

from ircbot import SingleServerIRCBot
from irclib import nm_to_n
import threading
import time
import math

tasks = 'rciw|censure'
projtasks = {}
mainters = []
activity = {}

class MaintcontBot(SingleServerIRCBot):
    def __init__(self, nickname, server, port=6667):
        SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)

    def on_nicknameinuse(self, c, e):
        c.nick(c.get_nickname() + "_")

    def on_welcome(self, c, e):
        t = threading.Thread(target=self.lister)
        t.setDaemon(True)
        t.start()

    def on_privmsg(self, c, e):
        nick = nm_to_n(e.source())
        c = self.connection
        cmd = e.arguments()[0]
        do = cmd.split()
        if do[0] == "workerjoin":
            c.privmsg(nick, "accepted")
            mainters.append([nick, do[1]])
            activity[nick] = time.time()
            print "worker got, name:", nick, "job:", do[1]
            self.retasker(do[1])
        elif do[0] == "active":
            activity[nick] = time.time()

    def on_dccmsg(self, c, e):
        pass

    def on_dccchat(self, c, e):
        pass

    def lister(self):
        while True:
            print
            print "worker list:"
            for mainter in mainters:
                if time.time() - activity[mainter[0]] > 30:
                    print "*", mainter[0], "has been removed"
                    mainters.remove(mainter)
                    del activity[mainter[0]]
                    self.retasker(mainter[1])
                    continue
                print "mainter name:", mainter[0], "job:", mainter[1]
            print "--------------------"
            print
            time.sleep(1*60)

    def retasker(self, group, optask = ''):
        ingroup = 0
        for mainter in mainters:
            if mainter[1] == group:
                ingroup += 1
        if ingroup == 0:
            return
        if group in projtasks:
            grt = projtasks[group]
        else:
            grt = tasks
        tpc = grt.split('|')
        tpcn = round(len(tpc) / ingroup)
        i = 0
        for mainter in mainters:
            if mainter[1] != group:
                continue
            tts = '|'.join(tpc[int(round(i * tpcn)):int(round((i + 1) * tpcn))])
            if tts != False:
                self.connection.privmsg(mainter[0], "tasklist " + tts)
            i += 1

def main():
    bot = MaintcontBot("maintcont", "irc.freenode.net")
    bot.start()

if __name__ == "__main__":
    main()
