#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
"""
#
# (C) Bryan Tong Minh, 2008
# (C) Pywikipedia bot team, 2008-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: revertbot.py 9042 2011-03-13 10:14:47Z xqt $'
#

import re
import wikipedia as pywikibot
import query, userlib


class BaseRevertBot(object):
    """ Base revert bot

    Subclass this bot and override callback to get it to do something useful.
    """
    def __init__(self, site, comment = None):
        self.site = site
        self.comment = comment

    def get_contributions(self, max = -1, ns = None):
        predata = {
            'action': 'query',
            'list': 'usercontribs',
            'uclimit': '500',
            'ucuser': self.site.username(),
        }
        if ns: predata['ucnamespace'] = ns
        if max < 500 and max != -1: predata['uclimit'] = str(max)

        count = 0
        iterator = iter(xrange(0))
        never_continue = False
        while count != max or never_continue:
            try:
                item = iterator.next()
            except StopIteration:
                self.log(u'Fetching new batch of contributions')
                data = query.GetData(predata, self.site)
                if 'error' in data:
                    raise RuntimeError(data['error'])
                if 'query-continue' in data:
                    predata['uccontinue'] = data['query-continue']['usercontribs']
                else:
                    never_continue = True
                iterator = iter(data['query']['usercontribs'])
            else:
                count += 1
                yield item

    def revert_contribs(self, callback = None):
        self.site.forceLogin()

        if callback is None:
            callback = self.callback

        contribs = self.get_contributions()
        for item in contribs:
            try:
                if callback(item):
                    result = self.revert(item)
                    if result:
                        self.log(u'%s: %s' % (item['title'], result))
                    else:
                        self.log(u'Skipped %s' % item['title'])
                else:
                    self.log(u'Skipped %s by callback' % item['title'])
            except StopIteration:
                return

    def callback(self, item):
        return 'top' in item

    def revert(self, item):
        predata = {
            'action': 'query',
            'titles': item['title'],
            'prop': 'revisions',
            'rvprop': 'ids|timestamp|user|content',
            'rvlimit': '2',
            'rvstart': item['timestamp'],
        }
        data = query.GetData(predata, self.site)

        if 'error' in data:
            raise RuntimeError(data['error'])

        pages = data['query'].get('pages', ())
        if not pages: return False
        page = pages.itervalues().next()
        if len(page.get('revisions', ())) != 2: return False
        rev = page['revisions'][1]

        comment = u'Reverted to revision %s by %s on %s' % (rev['revid'],
            rev['user'], rev['timestamp'])
        if self.comment: comment += ': ' + self.comment

        page = pywikibot.Page(self.site, item['title'])
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                         % page.aslink(True, True))
        old = page.get()
        new = rev['*']
        pywikibot.showDiff(old, new)
        page.put(new, comment)
        return comment

    def log(self, msg):
        pywikibot.output(msg)


class myRevertBot(BaseRevertBot):

    def callback(self, item):
        if 'top' in item:
            page = pywikibot.Page(self.site, item['title'])
            text=page.get()
            pattern = re.compile(u'\[\[.+?:.+?\..+?\]\]', re.UNICODE)
            return pattern.search(text) >= 0
        return False


def main():
    item = None
    for arg in pywikibot.handleArgs():
        continue
    bot = myRevertBot(site = pywikibot.getSite())
    bot.revert_contribs()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
