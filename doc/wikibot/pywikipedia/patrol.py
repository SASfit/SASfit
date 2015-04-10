#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This bot obtains a list of recentchanges and newpages and marks the
edits as patrolled based on a whitelist.
See http://en.wikisource.org/wiki/User:JVbot/patrol_whitelist

The following parameters are supported:

&params;

"""
#
# (C) Pywikipedia bot team, 2011
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: patrol.py 9139 2011-03-31 13:42:35Z xqt $'
import wikipedia as pywikibot
import pagegenerators
import mwlib.uparser # used to parse the whitelist
import mwlib.parser # used to parse the whitelist
import time

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;': pagegenerators.parameterHelp
}

class PatrolBot:
    # Localised name of the whitelist page
    whitelist_subpage_name = {
        'en': u'patrol_whitelist',
    }

    def __init__(self, feed, user=None, ask=True, whitelist=None):
        """
        Constructor. Parameters:
            * feed      - The changes feed to work on (Newpages or Recentchanges)
            * user      - Limit whitelist parsing and patrolling to a specific user
            * ask       - If True, confirm each patrol action
            * whitelist - page title for whitelist (optional)
        """
        self.feed = feed
        self.user = user
        self.ask = ask
        self.site = pywikibot.getSite()
        if whitelist:
            self.whitelist_pagename = whitelist
        else:
            local_whitelist_subpage_name = pywikibot.translate(self.site, self.whitelist_subpage_name)
            self.whitelist_pagename = u'%s:%s/%s' % (self.site.namespace(2),self.site.username(),local_whitelist_subpage_name)
        self.whitelist = None
        self.whitelist_ts = 0
        self.whitelist_load_ts = 0

        self.autopatroluserns = False
        self.highest_rcid = 0 # used to track loops
        self.last_rcid = 0
        self.repeat_start_ts = 0

        self.rc_item_counter = 0 # counts how many items have been reviewed
        self.patrol_counter = 0  # and how many times an action was taken

    def load_whitelist(self):
        # Check for a more recent version after 5 minutes
        if self.whitelist_load_ts and ((time.time() - self.whitelist_load_ts) < 300):
            if pywikibot.verbose:
                pywikibot.output(u'Whitelist not stale yet')
            return

        whitelist_page = pywikibot.Page(pywikibot.getSite(), self.whitelist_pagename)

        if not self.whitelist:
            pywikibot.output(u'Loading %s' % self.whitelist_pagename)

        try:
            if self.whitelist_ts:
                # check for a more recent version
                h = whitelist_page.getVersionHistory(forceReload=True,revCount=1)
                last_edit_ts = pywikibot.parsetime2stamp(h[0][1])
                if last_edit_ts == self.whitelist_ts:
                    # As there hasn't been any changed to the whitelist
                    # it has been effectively reloaded 'now'
                    self.whitelist_load_ts = time.time()
                    if pywikibot.verbose:
                        pywikibot.output(u'Whitelist not modified')
                    return

            if self.whitelist:
                pywikibot.output(u'Reloading whitelist')

            # Fetch whitelist
            wikitext = whitelist_page.get()
            # Parse whitelist
            self.whitelist = self.parse_page_tuples (wikitext, self.user)
            # Record timestamp
            self.whitelist_ts = whitelist_page.editTime()
            self.whitelist_load_ts = time.time()
        except Exception as e:
            # cascade if there isnt a whitelist to fallback on
            if not self.whitelist:
                raise
            pywikibot.output(u'Error: ' + e)

    def add_to_tuples(self, tuples, user, page):
        if pywikibot.verbose:
            pywikibot.output(u"Adding %s:%s" % (user, page.title()) )

        if user in tuples:
            tuples[user].append(page)
        else:
            tuples[user] = [page]

    def in_list(self, pagelist, title, laxyload=True):
        if pywikibot.verbose:
            pywikibot.output(u'Checking whitelist for: %s' % title)

        # quick check for exact match
        if title in pagelist:
            return title

        # quick check for wildcard
        if '' in pagelist:
            if pywikibot.verbose:
                pywikibot.output(u"wildcarded")
            return '.*'

        for item in pagelist:
            if pywikibot.verbose:
                pywikibot.output(u"checking against whitelist item = %s" % item)

            if isinstance(item, PatrolRule):
                if pywikibot.verbose:
                    pywikibot.output(u"invoking programmed rule")
                if item.match(title):
                    return item

            elif title_match(item, title):
                return item

        if pywikibot.verbose:
            pywikibot.output(u'not found')

    def parse_page_tuples(self, wikitext, user=None):
        tuples = {}

        # for any structure, the only first 'user:' page
        # is registered as the user the rest of the structure
        # refers to.
        def process_children(obj,current_user):
            if pywikibot.debug:
                pywikibot.output(u'parsing node: %s' % obj)
            for c in obj.children:
                temp = process_node(c,current_user)
                if temp and not current_user:
                    current_user = temp

        def process_node(obj,current_user):
            # links are analysed; interwiki links are included because mwlib
            # incorrectly calls 'Wikisource:' namespace links an interwiki
            if isinstance(obj, mwlib.parser.NamespaceLink) or isinstance(obj, mwlib.parser.InterwikiLink) or isinstance(obj, mwlib.parser.ArticleLink):
                if obj.namespace == -1:
                    # the parser accepts 'special:prefixindex/' as a wildcard
                    # this allows a prefix that doesnt match an existing page
                    # to be a blue link, and can be clicked to see what pages
                    # will be included in the whitelist
                    if obj.target[:20].lower() == 'special:prefixindex/':
                        if len(obj.target) == 20:
                            if pywikibot.verbose:
                                pywikibot.output(u'Whitelist everything')
                            page = ''
                        else:
                            page = obj.target[20:]
                            if pywikibot.verbose:
                                pywikibot.output(u'Whitelist prefixindex hack for: %s' % page)
                            #p = pywikibot.Page(self.site, obj.target[20:])
                            #obj.namespace = p.namespace
                            #obj.target = p.title()

                elif obj.namespace == 2 and not current_user:
                    # if a target user hasnt been found yet, and the link is 'user:'
                    # the user will be the target of subsequent rules
                    page_prefix_len = len(self.site.namespace(2))
                    current_user = obj.target[(page_prefix_len+1):]
                    if pywikibot.verbose:
                        pywikibot.output(u'Whitelist user: %s' % current_user)
                    return current_user
                else:
                    page = obj.target

                if current_user:
                    if not user or current_user == user:
                        if self.is_wikisource_author_page(page):
                            if pywikibot.verbose:
                                pywikibot.output(u'Whitelist author: %s' % page)
                            author = LinkedPagesRule(page)
                            self.add_to_tuples(tuples, current_user, author)
                        else:
                            if pywikibot.verbose:
                                pywikibot.output(u'Whitelist page: %s' % page)
                            self.add_to_tuples(tuples, current_user, page)
                    elif pywikibot.verbose:
                        pywikibot.output(u'Discarding whitelist page for another user: %s' % page)
                else:
                    raise Exception(u"No user set for page %s" % page)
            else:
                process_children(obj,current_user)

        subject_map = []
        root = mwlib.uparser.parseString(title='Not used',raw=wikitext)
        process_children(root,None)

        return tuples

    def is_wikisource_author_page(self, title):
        if not self.site.family.name == 'wikisource':
            return False

        author_ns = 0
        try:
            author_ns = self.site.family.authornamespaces[self.site.lang][0]
        except:
            pass

        if author_ns:
            author_ns_prefix = self.site.namespace(author_ns)

        if pywikibot.debug:
            pywikibot.output(u'Author ns: %d; name: %s' % (author_ns, author_ns_prefix))

        if title.find(author_ns_prefix+':') == 0:
            return True

        if pywikibot.verbose:
            author_page_name = title[len(author_ns_prefix)+1:]
            pywikibot.output(u'Found author %s' % author_page_name)

        return False

    def run(self, feed = None):
        if self.whitelist == None:
            self.load_whitelist()

        if not feed:
            feed = self.feed

        for page in feed:
            self.treat(page)

    def treat(self, page):
        """
        Loads the given page, does some changes, and saves it.
        """
        choice = None
        try:
            # page: title, date, username, comment, loginfo, rcid, token
            username = page[1]
            # when the feed isnt from the API, it used to contain
            # '(not yet written)' or '(page does not exist)' when it was
            # a redlink
            revid = page[2]
            rcid = page[3]
            if not rcid:
                raise Exception('rcid not present')

            # check whether we have wrapped around to higher rcids
            # which indicates a new RC feed is being processed
            if rcid > self.last_rcid:
                # refresh the whitelist
                self.load_whitelist()
                self.repeat_start_ts = time.time()

            title = page[0].title()
            if pywikibot.verbose or self.ask:
                pywikibot.output(u"User %s has created or modified page %s" % (username, title) )

            if self.autopatroluserns and (page[0].namespace() == 2 or page[0].namespace() == 3):
                # simple rule to whitelist any user editing their own userspace
                if page[0].titleWithoutNamespace().startswith(username):
                    if pywikibot.verbose:
                        pywikibot.output(u'%s is whitelisted to modify %s' % (username, page[0].title()))
                    choice = 'y'

            if choice != 'y' and username in self.whitelist:
                if self.in_list(self.whitelist[username], page[0].title() ):
                    if pywikibot.verbose:
                        pywikibot.output(u'%s is whitelisted to modify %s' % (username, page[0].title()))
                    choice = 'y'

            if self.ask:
                options = ['y', 'N']
                # default to automatic choice
                if choice == 'y':
                    options = ['Y', 'n']
                else:
                    choice = 'N'

                choice = pywikibot.inputChoice(u'Do you want to mark page as patrolled?', ['Yes', 'No'], options, choice)

            # Patrol the page
            if choice == 'y':
                response = self.site.patrol(rcid)
                self.patrol_counter = self.patrol_counter + 1
                pywikibot.output(u"Patrolled %s (rcid %d) by user %s" % (title, rcid, username))
            else:
                if pywikibot.verbose:
                    pywikibot.output(u"skipped")

            if rcid > self.highest_rcid:
                self.highest_rcid = rcid
            self.last_rcid = rcid
            self.rc_item_counter = self.rc_item_counter + 1

        except pywikibot.NoPage:
            pywikibot.output(u"Page %s does not exist; skipping." % page.aslink())
            return
        except pywikibot.IsRedirectPage:
            pywikibot.output(u"Page %s is a redirect; skipping." % page.aslink())
            return

def title_match(prefix, title):
    if pywikibot.verbose:
        pywikibot.output(u'matching %s to prefix %s' % (title,prefix))
    prefix_len=len(prefix)
    title_trimmed = title[:prefix_len]
    if title_trimmed == prefix:
        if pywikibot.verbose:
            pywikibot.output(u"substr match")
        return True
    return False

class PatrolRule:
    def __init__(self, page_title):
        """
        Constructor. Parameters:
            * page_title - The page title for this rule
        """
        self.page_title = page_title

    def title(self):
        return self.page_title

    def match(self, page):
        pass

class LinkedPagesRule(PatrolRule):
    def __init__(self, page_title):
        self.site = pywikibot.getSite()
        self.page_title = page_title
        self.linkedpages = None

    def match(self, page_title):
        if page_title == self.page_title:
            return True

        if not self.site.family.name == 'wikisource':
            raise Exception('This is a wikisource rule')

        if not self.linkedpages:
            if pywikibot.verbose:
                pywikibot.output(u"loading page links on %s" % self.page_title)

            p = pywikibot.Page(self.site, self.page_title)
            linkedpages = list()
            for linkedpage in p.linkedPages():
                linkedpages.append(linkedpage.title())
            self.linkedpages = linkedpages

            if pywikibot.verbose:
                pywikibot.output(u"loaded %d page links" % len(linkedpages) )

        for p in self.linkedpages:
            if pywikibot.verbose:
                pywikibot.output(u"checking against '%s'" % p )

            if title_match(p, page_title):
                if pywikibot.verbose:
                    pywikibot.output(u"Matched.")
                return p

def api_feed_repeater(gen, delay=0, repeat=False, number = 1000, namespace=None, user=None):
    while True:
        for page in gen(number=number, namespace=namespace, user=user, rcshow = '!patrolled', returndict = True):
            attrs = page[1]
            yield page[0], attrs['user'], attrs['revid'], attrs['rcid']
        if repeat:
            pywikibot.output(u'Sleeping for %d seconds' % delay)
            time.sleep(delay)
        else:
            break

def main():
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()
    # The generator gives the pages that should be worked upon.
    gen = None
    # This temporary array is used to read the page title if one single
    # page to work on is specified by the arguments.
    pageTitleParts = []
    ask = False
    repeat = False
    autopatroluserns = False
    recentchanges = False
    newpages = False
    namespace = None
    user = None

    # Parse command line arguments
    for arg in pywikibot.handleArgs():
        if arg.startswith("-ask"):
            ask = True
        elif arg.startswith("-autopatroluserns"):
            autopatroluserns = True
        elif arg.startswith("-repeat"):
            repeat = True
        elif arg.startswith("-newpages"):
            newpages = True
        elif arg.startswith("-recentchanges"):
            recentchanges = True
        elif arg.startswith("-namespace:"):
            namespace = arg[11:]
            namespace = int(namespace)
        elif arg.startswith("-user:"):
            user = arg[6:]
        else:
            # check if a standard argument like
            # -start:XYZ or -ref:Asdf was given.
            generator = genFactory.handleArg(arg)
            if generator:
                gen = generator
            else:
                pageTitleParts.append(arg)

    site = pywikibot.getSite()
    site.forceLogin()

    if user:
        pywikibot.output(u"processing user: %s" % user)

    newpage_count = 300
    if not newpages and not recentchanges and not user:
        if site.family.name == 'wikipedia':
            newpages = True
            newpage_count = 5000
        else:
            recentchanges = True

    bot = PatrolBot(None, user, ask)
    bot.autopatroluserns = autopatroluserns

    if newpages or user:
        pywikibot.output(u"Newpages:")
        gen = site.newpages
        feed = api_feed_repeater(gen, delay=60, repeat=repeat, number=newpage_count, namespace=namespace, user=user)
        bot.run(feed)

    if recentchanges or user:
        pywikibot.output(u"Recentchanges:")
        gen = site.recentchanges
        feed = api_feed_repeater(gen, delay=60, repeat=repeat, number = 1000, namespace=namespace, user=user)
        bot.run(feed)

    pywikibot.output(u'%d/%d patrolled' % (bot.patrol_counter, bot.rc_item_counter))

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
