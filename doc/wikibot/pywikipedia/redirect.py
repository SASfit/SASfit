# -*- coding: utf-8 -*-
"""
Script to resolve double redirects, and to delete broken redirects. Requires
access to MediaWiki's maintenance pages or to a XML dump file. Delete
function requires adminship.

Syntax:

    python redirect.py action [-arguments ...]

where action can be one of these:

double         Fix redirects which point to other redirects
broken         Delete redirects where targets don\'t exist. Requires adminship.
both           Both of the above. Permitted only with -api. Implies -api.

and arguments can be:

-xml           Retrieve information from a local XML dump
               (http://download.wikimedia.org). Argument can also be given as
               "-xml:filename.xml". Cannot be used with -api or -moves.

-api           Retrieve information from the wiki via MediaWikis application
               program interface (API). Cannot be used with -xml.

-moves         Use the page move log to find double-redirect candidates. Only
               works with action "double", does not work with -xml. You may
               use -api option for retrieving pages via API

               NOTE: If neither of -xml -api -moves is given, info will be
               loaded from a special page of the live wiki.

-namespace:n   Namespace to process. Can be given multiple times, for several
               namespaces. If omitted, only the main (article) namespace is
               is treated with -api, with -xml all namespaces are treated,
               Works only with an XML dump, or the API interface.

-offset:n      With -moves, the number of hours ago to start scanning moved
               pages. With -xml, the number of the redirect to restart with
               (see progress). Otherwise, ignored.

-start:title   With -api, the starting page title in each namespace.
               Otherwise ignored. Page needs not exist.

-until:title   With -api, the possible last page title in each namespace.
               Otherwise ignored. Page needs not exist.

-total:n       With -api, the maximum count of redirects to work upon.
               Otherwise ignored. Use 0 for unlimited

-always        Don't prompt you for each replacement.

"""
from __future__ import generators
#
# (C) Daniel Herding, 2004.
# (C) Purodha Blissenbach, 2009.
# (C) xqt, 2009-2011
# (C) Pywikipedia bot team, 2004-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: redirect.py 9202 2011-04-29 04:59:18Z xqt $'
#
import re, sys, datetime
import wikipedia as pywikibot
from pywikibot import i18n
import config
import query
import xmlreader

# Summary message for fixing double redirects
msg_double = 'redirect-fix-double'

# Reason for deleting broken redirects
reason_broken = 'redirect-remove-broken'

# Reason for deleting redirect loops
reason_loop = 'redirect-remove-loop'

# Insert deletion template into page with a broken redirect
sd_template = 'redirect-broken-redirect-template'

class RedirectGenerator:
    def __init__(self, xmlFilename=None, namespaces=[], offset=-1,
                 use_move_log=False, use_api=False, start=None, until=None,
                 number=None):
        self.site = pywikibot.getSite()
        self.xmlFilename = xmlFilename
        self.namespaces = namespaces
        if use_api and self.namespaces == []:
            self.namespaces = [ 0 ]
        self.offset = offset
        self.use_move_log = use_move_log
        self.use_api = use_api
        self.api_start = start
        self.api_until = until
        self.api_number = number
        if self.api_number is None:
            # since 'max' does not works with wikia 1.15.5 use a number instead
            if self.site.versionnumber() < 16 or use_move_log:
                self.api_number = config.special_page_limit
            else:
                self.api_number = 'max'

    def get_redirects_from_dump(self, alsoGetPageTitles=False):
        '''
        Load a local XML dump file, look at all pages which have the
        redirect flag set, and find out where they're pointing at. Return
        a dictionary where the redirect names are the keys and the redirect
        targets are the values.
        '''
        xmlFilename = self.xmlFilename
        redict = {}
        # open xml dump and read page titles out of it
        dump = xmlreader.XmlDump(xmlFilename)
        redirR = self.site.redirectRegex()
        readPagesCount = 0
        if alsoGetPageTitles:
            pageTitles = set()
        for entry in dump.parse():
            readPagesCount += 1
            # always print status message after 10000 pages
            if readPagesCount % 10000 == 0:
                pywikibot.output(u'%i pages read...' % readPagesCount)
            if len(self.namespaces) > 0:
                if pywikibot.Page(self.site, entry.title).namespace() \
                        not in self.namespaces:
                    continue
            if alsoGetPageTitles:
                pageTitles.add(entry.title.replace(' ', '_'))

            m = redirR.match(entry.text)
            if m:
                target = m.group(1)
                # There might be redirects to another wiki. Ignore these.
                for code in self.site.family.iwkeys:
                    if target.startswith('%s:' % code) \
                            or target.startswith(':%s:' % code):
                        if code == self.site.language():
                        # link to our wiki, but with the lang prefix
                            target = target[(len(code)+1):]
                            if target.startswith(':'):
                                target = target[1:]
                        else:
                            pywikibot.output(
                                u'NOTE: Ignoring %s which is a redirect to %s:'
                                % (entry.title, code))
                            target = None
                            break
                # if the redirect does not link to another wiki
                if target:
                    source = entry.title.replace(' ', '_')
                    target = target.replace(' ', '_')
                    # remove leading and trailing whitespace
                    target = target.strip('_')
                    # capitalize the first letter
                    if not pywikibot.getSite().nocapitalize:
                        source = source[:1].upper() + source[1:]
                        target = target[:1].upper() + target[1:]
                    if '#' in target:
                        target = target[:target.index('#')].rstrip("_")
                    if '|' in target:
                        pywikibot.output(
                            u'HINT: %s is a redirect with a pipelink.'
                            % entry.title)
                        target = target[:target.index('|')].rstrip("_")
                    if target: # in case preceding steps left nothing
                        redict[source] = target
        if alsoGetPageTitles:
            return redict, pageTitles
        else:
            return redict

    def get_redirect_pageids_via_api(self):
        """Return generator that yields
        page IDs of Pages that are redirects.

        """
        params = {
            'action': 'query',
            'list': 'allpages',
            'apfilterredir': 'redirects',
            'aplimit': self.api_number,
            'apdir': 'ascending',
        }
        for ns in self.namespaces:
            params['apnamespace'] = ns
            if self.api_start:
                params['apfrom'] = self.api_start
            done = False
            while not done:
                pywikibot.output(u'\nRetrieving pages...', newline=False)
                data = query.GetData(params, self.site)
                if 'error' in data:
                    raise RuntimeError("API query error: %s" % data['error'])
                if "limits" in data: # process aplimit = max
                    params['aplimit'] = int(data['limits']['allpages'])
                for x in data['query']['allpages']:
                    done = self.api_until and x['title'] >= self.api_until
                    if done: break
                    yield x['pageid']
                if not done and 'query-continue' in data:
                    params['apfrom'] = data['query-continue']['allpages']['apfrom']
                else:
                    break

    def _next_redirect_group(self):
        """
        Return a generator that retrieves pageids from the API 500 at a time
        and yields them as a list
        """
        apiQ = []
        for pageid in self.get_redirect_pageids_via_api():
            apiQ.append(pageid)
            if len(apiQ) >= 500:
                yield apiQ
                apiQ = []
        if apiQ:
            yield apiQ

    def get_redirects_via_api(self, maxlen=8):
        """
        Return a generator that yields tuples of data about redirect Pages:
            0 - page title of a redirect page
            1 - type of redirect:
                         0 - broken redirect, target page title missing
                         1 - normal redirect, target page exists and is not a
                             redirect
                 2..maxlen - start of a redirect chain of that many redirects
                             (currently, the API seems not to return sufficient
                             data to make these return values possible, but
                             that may change)
                  maxlen+1 - start of an even longer chain, or a loop
                             (currently, the API seems not to return sufficient
                             data to allow this return values, but that may
                             change)
                      None - start of a redirect chain of unknown length, or loop
            2 - target page title of the redirect, or chain (may not exist)
            3 - target page of the redirect, or end of chain, or page title where
                chain or loop detecton was halted, or None if unknown
        """
        import urllib
        params = {
            'action':'query',
            'redirects':1,
            #'':'',
        }
        for apiQ in self._next_redirect_group():
            params['pageids'] = apiQ
            pywikibot.output(u'.', newline=False)
            data = query.GetData(params, self.site)
            if 'error' in data:
                raise RuntimeError("API query error: %s" % data)
            if data == [] or 'query' not in data:
                raise RuntimeError("No results given.")
            redirects = {}
            pages = {}
            redirects = dict((x['from'], x['to'])
                             for x in data['query']['redirects'])

            for pagetitle in data['query']['pages'].values():
                if 'missing' in pagetitle and 'pageid' not in pagetitle:
                    pages[pagetitle['title']] = False
                else:
                    pages[pagetitle['title']] = True
            for redirect in redirects:
                target = redirects[redirect]
                result = 0
                final = None
                try:
                    if pages[target]:
                        final = target
                        try:
                            while result <= maxlen:
                               result += 1
                               final = redirects[final]
                            # result = None
                        except KeyError:
                            pass
                except KeyError:
                    result = None
                    pass
                yield (redirect, result, target, final)

    def retrieve_broken_redirects(self):
        if self.use_api:
            count = 0
            for (pagetitle, type, target, final) \
                    in self.get_redirects_via_api(maxlen=2):
                if type == 0:
                    yield pagetitle
                    if self.api_number:
                        count += 1
                        if count >= self.api_number:
                            break

        elif self.xmlFilename == None:
            # retrieve information from the live wiki's maintenance page
            # broken redirect maintenance page's URL
            path = self.site.broken_redirects_address(default_limit=False)
            pywikibot.output(u'Retrieving special page...')
            maintenance_txt = self.site.getUrl(path)

            # regular expression which finds redirects which point to a
            # non-existing page inside the HTML
            Rredir = re.compile('\<li\>\<a href=".+?" title="(.*?)"')

            redir_names = Rredir.findall(maintenance_txt)
            pywikibot.output(u'Retrieved %d redirects from special page.\n'
                             % len(redir_names))
            for redir_name in redir_names:
                yield redir_name
        else:
            # retrieve information from XML dump
            pywikibot.output(
                u'Getting a list of all redirects and of all page titles...')
            redirs, pageTitles = self.get_redirects_from_dump(
                                            alsoGetPageTitles=True)
            for (key, value) in redirs.iteritems():
                if value not in pageTitles:
                    yield key

    def retrieve_double_redirects(self):
        if self.use_api and not self.use_move_log:
            count = 0
            for (pagetitle, type, target, final) \
                    in self.get_redirects_via_api(maxlen=2):
                if type != 0 and type != 1:
                    yield pagetitle
                    if self.api_number:
                        count += 1
                        if count >= self.api_number:
                            break

        elif self.xmlFilename == None:
            if self.use_move_log:
                if self.use_api:
                    gen = self.get_moved_pages_redirects()
                else:
                    gen = self.get_moved_pages_redirects_old()
                for redir_page in gen:
                    yield redir_page.title()
                return
            # retrieve information from the live wiki's maintenance page
            # double redirect maintenance page's URL
#            pywikibot.config.special_page_limit = 1000
            path = self.site.double_redirects_address(default_limit = False)
            pywikibot.output(u'Retrieving special page...')
            maintenance_txt = self.site.getUrl(path)

            # regular expression which finds redirects which point to
            # another redirect inside the HTML
            Rredir = re.compile('\<li\>\<a href=".+?" title="(.*?)">')
            redir_names = Rredir.findall(maintenance_txt)
            pywikibot.output(u'Retrieved %i redirects from special page.\n'
                             % len(redir_names))
            for redir_name in redir_names:
                yield redir_name
        else:
            redict = self.get_redirects_from_dump()
            num = 0
            for (key, value) in redict.iteritems():
                num += 1
                # check if the value - that is, the redirect target - is a
                # redirect as well
                if num > self.offset and value in redict:
                    yield key
                    pywikibot.output(u'\nChecking redirect %i of %i...'
                                     % (num + 1, len(redict)))

    def get_moved_pages_redirects(self):
        '''generate redirects to recently-moved pages'''
        # this will run forever, until user interrupts it

        if self.offset <= 0:
            self.offset = 1
        start = datetime.datetime.utcnow() \
                - datetime.timedelta(0, self.offset*3600)
        # self.offset hours ago
        offset_time = start.strftime("%Y%m%d%H%M%S")
        pywikibot.output(u'Retrieving %s moved pages via API...'
                         % str(self.api_number))
        if pywikibot.verbose:
            pywikibot.output(u"[%s]" % offset_time)
        for moved_page, u, t, c in self.site.logpages(number=self.api_number,
                                                      mode='move',
                                                      start=offset_time):
            try:
                if not moved_page.isRedirectPage():
                    continue
            except pywikibot.BadTitle:
                continue
            except pywikibot.ServerError:
                continue
            # moved_page is now a redirect, so any redirects pointing
            # to it need to be changed
            try:
                for page in moved_page.getReferences(follow_redirects=True,
                                                     redirectsOnly=True):
                    yield page
            except pywikibot.NoPage:
                # original title must have been deleted after move
                continue

    def get_moved_pages_redirects_old(self):

        move_regex = re.compile(
                r'moved <a href.*?>(.*?)</a> to <a href=.*?>.*?</a>.*?</li>')

        if self.offset <= 0:
            self.offset = 1
        offsetpattern = re.compile(
            r"""\(<a href="/w/index\.php\?title=Special:Log&amp;offset=(\d+)"""
            r"""&amp;limit=500&amp;type=move" title="Special:Log" rel="next">"""
            r"""older 500</a>\)""")
        start = datetime.datetime.utcnow() \
                 - datetime.timedelta(0, self.offset*3600)
        # self.offset hours ago
        offset_time = start.strftime("%Y%m%d%H%M%S")
        while True:
            move_url = \
                self.site.path() + "?title=Special:Log&limit=500&offset=%s&type=move"\
                       % offset_time
            try:
                move_list = self.site.getUrl(move_url)
                if pywikibot.verbose:
                    pywikibot.output(u"[%s]" % offset_time)
            except:
                import traceback
                pywikibot.output(unicode(traceback.format_exc()))
                return
            g = move_regex.findall(move_list)
            if pywikibot.verbose:
                pywikibot.output(u"%s moved pages" % len(g))
            for moved_title in g:
                moved_page = pywikibot.Page(self.site, moved_title)
                try:
                    if not moved_page.isRedirectPage():
                        continue
                except pywikibot.BadTitle:
                    continue
                except pywikibot.ServerError:
                    continue
                # moved_page is now a redirect, so any redirects pointing
                # to it need to be changed
                try:
                    for page in moved_page.getReferences(follow_redirects=True,
                                                         redirectsOnly=True):
                        yield page
                except pywikibot.NoPage:
                    # original title must have been deleted after move
                    continue
            m = offsetpattern.search(move_list)
            if not m:
                break
            offset_time = m.group(1)

class RedirectRobot:
    def __init__(self, action, generator, always=False, number=None):
        self.site = pywikibot.getSite()
        self.action = action
        self.generator = generator
        self.always = always
        self.number = number
        self.exiting = False

    def prompt(self, question):
        if not self.always:
            choice = pywikibot.inputChoice(question,
                                           ['Yes', 'No', 'All', 'Quit'],
                                           ['y', 'N', 'a', 'q'], 'N')
            if choice == 'n':
                return False
            elif choice == 'q':
                self.exiting = True
                return False
            elif choice == 'a':
                self.always = True
        return True

    def delete_broken_redirects(self):
        # get reason for deletion text
        reason = i18n.twtranslate(self.site, reason_broken)
        for redir_name in self.generator.retrieve_broken_redirects():
            self.delete_1_broken_redirect( redir_name, reason)
            if self.exiting:
                break

    def delete_1_broken_redirect(self, redir_name, reason):
        redir_page = pywikibot.Page(self.site, redir_name)
        # Show the title of the page we're working on.
        # Highlight the title in purple.
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                          % redir_page.title())
        try:
            targetPage = redir_page.getRedirectTarget()
        except pywikibot.IsNotRedirectPage:
            pywikibot.output(u'%s is not a redirect.' % redir_page.title())
        except pywikibot.NoPage:
            pywikibot.output(u'%s doesn\'t exist.' % redir_page.title())
        else:
            try:
                targetPage.get()
            except pywikibot.NoPage:
                if self.prompt(
        u'Redirect target %s does not exist. Do you want to delete %s?'
                               % (targetPage.title(asLink=True),
                                  redir_page.title(asLink=True))):
                    try:
                        redir_page.delete(reason, prompt = False)
                    except pywikibot.NoUsername:
                        if i18n.twhas_key(
                            targetPage.site().lang, sd_template) and \
                            i18n.twhas_key(targetPage.site().lang,
                                           reason_broken):
                            pywikibot.output(
        u"No sysop in user-config.py, put page to speedy deletion.")
                            content = redir_page.get(get_redirect=True)
                            ### TODO: Add bot's signature if needed
                            ###       Not supported via TW yet
                            content = i18n.twtranslate(
                                targetPage.site().lang,
                                sd_template) + "\n" + content
                            redir_page.put(content, reason)
            except pywikibot.IsRedirectPage:
                pywikibot.output(
        u'Redirect target %s is also a redirect! Won\'t delete anything.'
                    % targetPage.title(asLink=True))
            else:
                #we successfully get the target page, meaning that
                #it exists and is not a redirect: no reason to touch it.
                pywikibot.output(
                    u'Redirect target %s does exist! Won\'t delete anything.'
                    % targetPage.title(asLink=True))
        pywikibot.output(u'')

    def fix_double_redirects(self):
        for redir_name in self.generator.retrieve_double_redirects():
            self.fix_1_double_redirect(redir_name)
            if self.exiting:
                break

    def fix_1_double_redirect(self,  redir_name):
            redir = pywikibot.Page(self.site, redir_name)
            # Show the title of the page we're working on.
            # Highlight the title in purple.
            pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                              % redir.title())
            newRedir = redir
            redirList = []  # bookkeeping to detect loops
            while True:
                redirList.append(u'%s:%s' % (newRedir.site().lang,
                                             newRedir.sectionFreeTitle()))
                try:
                    targetPage = newRedir.getRedirectTarget()
                except pywikibot.IsNotRedirectPage:
                    if len(redirList) == 1:
                        pywikibot.output(u'Skipping: Page %s is not a redirect.'
                                         % redir.title(asLink=True))
                        break  #do nothing
                    elif len(redirList) == 2:
                        pywikibot.output(
                            u'Skipping: Redirect target %s is not a redirect.'
                            % newRedir.title(asLink=True))
                        break  # do nothing
                    else:
                        pass # target found
                except pywikibot.SectionError:
                    pywikibot.output(
                        u'Warning: Redirect target section %s doesn\'t exist.'
                          % newRedir.title(asLink=True))
                except pywikibot.BadTitle, e:
                    # str(e) is in the format 'BadTitle: [[Foo]]'
                    pywikibot.output(
                        u'Warning: Redirect target %s is not a valid page title.'
                          % str(e)[10:])
                #sometimes this error occures. Invalid Title starting with a '#'
                except pywikibot.InvalidTitle, err:
                    pywikibot.output(u'Warning: %s' % err)
                    break
                except pywikibot.NoPage:
                    if len(redirList) == 1:
                        pywikibot.output(u'Skipping: Page %s does not exist.'
                                            % redir.title(asLink=True))
                        break
                    else:
                        if self.always:
                            pywikibot.output(
                                u"Skipping: Redirect target %s doesn't exist."
                                % newRedir.title(asLink=True))
                            break  # skip if automatic
                        else:
                            pywikibot.output(
                                u"Warning: Redirect target %s doesn't exist."
                                % newRedir.title(asLink=True))
                except pywikibot.ServerError:
                    pywikibot.output(u'Skipping: Server Error')
                    break
                else:
                    pywikibot.output(
                        u'   Links to: %s.'
                        % targetPage.title(asLink=True))
                    if targetPage.site().sitename() == 'wikipedia:en':
                        mw_msg = targetPage.site().mediawiki_message(
                                     'wikieditor-toolbar-tool-redirect-example')
                        if targetPage.title() == mw_msg:
                            pywikibot.output(
                                u"Skipping toolbar example: Redirect source is potentially vandalized.")
                            break
                    if targetPage.site() != self.site:
                        pywikibot.output(
                            u'Warning: redirect target (%s) is on a different site.'
                            % targetPage.title(asLink=True))
                        if self.always:
                            break  # skip if automatic
                    # watch out for redirect loops
                    if redirList.count(u'%s:%s'
                                       % (targetPage.site().lang,
                                          targetPage.sectionFreeTitle())
                                      ) > 0:
                        pywikibot.output(
                            u'Warning: Redirect target %s forms a redirect loop.'
                            % targetPage.title(asLink=True))
                        break ### doesn't work. edits twice!
##                        try:
##                            content = targetPage.get(get_redirect=True)
##                        except pywikibot.SectionError:
##                            content = pywikibot.Page(
##                                          targetPage.site(),
##                                          targetPage.sectionFreeTitle()
##                                      ).get(get_redirect=True)
##                        if targetPage.site().lang in sd_template and \
##                           targetPage.site().lang in sd_tagging_sum:
##                            pywikibot.output(u"Tagging redirect for deletion")
##                            # Delete the two redirects
##                            content = pywikibot.translate(
##                                        targetPage.site().lang,
##                                        sd_template)+"\n"+content
##                            summ = pywikibot.translate(targetPage.site().lang,
##                                                       reason_loop)
##                            targetPage.put(content, summ)
##                            redir.put(content, summ)
##                        break # TODO Better implement loop redirect
                    else: # redirect target found
                        if targetPage.isStaticRedirect():
                            pywikibot.output(
                                u"   Redirect target is STATICREDIRECT.")
                            pass
                        else:
                            newRedir = targetPage
                            continue
                try:
                    oldText = redir.get(get_redirect=True)
                except pywikibot.BadTitle:
                    pywikibot.output(u"Bad Title Error")
                    break
                text = self.site.redirectRegex().sub(
                    '#%s %s' % (self.site.redirect(True),
                                targetPage.title(asLink=True)), oldText)
                if text == oldText:
                    pywikibot.output(u"Note: Nothing left to do on %s"
                                     % redir.title(asLink=True))
                    break
                summary = i18n.twtranslate(self.site, msg_double) \
                          % {'to': targetPage.title(asLink=True)}
                pywikibot.showDiff(oldText, text)
                if self.prompt(u'Do you want to accept the changes?'):
                    try:
                        redir.put(text, summary)
                    except pywikibot.LockedPage:
                        pywikibot.output(u'%s is locked.' % redir.title())
                    except pywikibot.SpamfilterError, error:
                        pywikibot.output(
                            u"Saving page [[%s]] prevented by spam filter: %s"
                            % (redir.title(), error.url))
                    except pywikibot.PageNotSaved, error:
                        pywikibot.output(u"Saving page [[%s]] failed: %s"
                                         % (redir.title(), error))
                    except pywikibot.NoUsername:
                        pywikibot.output(
                            u"Page [[%s]] not saved; sysop privileges required."
                            % redir.title())
                    except pywikibot.Error, error:
                        pywikibot.output(
                            u"Unexpected error occurred trying to save [[%s]]: %s"
                            % (redir.title(), error))
                break

    def fix_double_or_delete_broken_redirects(self):
        # TODO: part of this should be moved to generator, the rest merged into self.run()
        # get reason for deletion text
        delete_reason = i18n.twtranslate(self.site, reason_broken)
        count = 0
        for (redir_name, code, target, final)\
                in self.generator.get_redirects_via_api(maxlen=2):
            if code == 1:
                continue
            elif code == 0:
                self.delete_1_broken_redirect(redir_name, delete_reason)
                count += 1
            else:
                self.fix_1_double_redirect(redir_name)
                count += 1
            if self.exiting or (self.number and count >= self.number):
                break

    def run(self):
        # TODO: make all generators return a redirect type indicator,
        #       thus make them usable with 'both'
        if self.action == 'double':
            self.fix_double_redirects()
        elif self.action == 'broken':
            self.delete_broken_redirects()
        elif self.action == 'both':
            self.fix_double_or_delete_broken_redirects()

def main(*args):
    # read command line parameters
    # what the bot should do (either resolve double redirs, or delete broken
    # redirs)
    action = None
    # where the bot should get his infos from (either None to load the
    # maintenance special page from the live wiki, or the filename of a
    # local XML dump file)
    xmlFilename = None
    # Which namespace should be processed when using a XML dump
    # default to -1 which means all namespaces will be processed
    namespaces = []
    # at which redirect shall we start searching double redirects again
    # (only with dump); default to -1 which means all redirects are checked
    offset = -1
    moved_pages = False
    api = False
    start = ''
    until = ''
    number = None
    always = False
    for arg in pywikibot.handleArgs(*args):
        if arg == 'double' or arg == 'do':
            action = 'double'
        elif arg == 'broken' or arg == 'br':
            action = 'broken'
        elif arg == 'both':
            action = 'both'
        elif arg == '-api':
            api = True
        elif arg.startswith('-xml'):
            if len(arg) == 4:
                xmlFilename = pywikibot.input(
                                u'Please enter the XML dump\'s filename: ')
            else:
                xmlFilename = arg[5:]
        elif arg.startswith('-moves'):
            moved_pages = True
        elif arg.startswith('-namespace:'):
            ns = arg[11:]
            if ns == '':
        ## "-namespace:" does NOT yield -namespace:0 further down the road!
                ns = pywikibot.input(
                        u'Please enter a namespace by its number: ')
#                       u'Please enter a namespace by its name or number: ')
#  TODO! at least for some generators.
            if ns == '':
               ns = '0'
            try:
                ns = int(ns)
            except ValueError:
#-namespace:all Process all namespaces. Works only with the API read interface.
               pass
            if not ns in namespaces:
               namespaces.append(ns)
        elif arg.startswith('-offset:'):
            offset = int(arg[8:])
        elif arg.startswith('-start:'):
            start = arg[7:]
        elif arg.startswith('-until:'):
            until = arg[7:]
        elif arg.startswith('-total:'):
            number = int(arg[8:])
        # old param, use -total instead
        elif arg.startswith('-number:'):
            number = int(arg[8:])
        elif arg == '-always':
            always = True
        else:
            pywikibot.output(u'Unknown argument: %s' % arg)

    if not action or (xmlFilename and moved_pages)\
                  or (api and xmlFilename):
        pywikibot.showHelp('redirect')
    else:
        gen = RedirectGenerator(xmlFilename, namespaces, offset, moved_pages,
                                api, start, until, number)
        bot = RedirectRobot(action, gen, always, number)
        bot.run()

if __name__ == '__main__':
    try:
        main()
    finally:
        pywikibot.stopme()
