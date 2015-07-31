# -*- coding: utf-8  -*-
"""
Library to work with users, their pages and talk pages.
"""
#
# (C) Pywikipedia bot team, 2008-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: userlib.py 9042 2011-03-13 10:14:47Z xqt $'

import re
import wikipedia as pywikibot
import query, config

class AutoblockUser(pywikibot.Error):
    """
    The class AutoblockUserError is an exception that is raised whenever
    an action is requested on a virtual autoblock user that's not available
    for him (i.e. roughly everything except unblock).
    """
class UserActionRefuse(pywikibot.Error): pass

class BlockError(UserActionRefuse): pass

class AlreadyBlocked(BlockError): pass

class UnblockError(UserActionRefuse): pass

class BlockIDError(UnblockError): pass

class AlreadyUnblocked(UnblockError): pass

class InvalidUser(pywikibot.InvalidTitle):
    """The mediawiki API does not allow IP lookups."""
    pass

ip_regexp = re.compile(r'^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}' \
                       r'(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$')

class User(object):
    """A class that represents a Wiki user.
    """

    def __init__(self, site, name):
        """Initializer for a User object.

        Parameters:
        site - a pywikibot.Site object
        name - name of the user, without the trailing User:
        """
        if len(name) > 1 and name[0] == u'#':
            self._isAutoblock = True
        else:
            self._isAutoblock = False
        if self._isAutoblock:
            # This user is probably being queried for purpose of lifting
            # an autoblock.
            pywikibot.output(
                "This is an autoblock ID, you can only use to unblock it.")
        if type(site) in [str, unicode]:
            self._site = pywikibot.getSite(site)
        else:
            self._site = site
        # None means not loaded
        self._name = name
        self._blocked = None
        self._groups = None
        self._registrationTime = -1
        #if self.site().versionnumber() >= 16:
        #    self._urToken = None


    def site(self):
        return self._site

    def name(self):
        return self.username

    @property
    def username(self):
        return self._name

    def isAnonymous(self):
        return ip_regexp.match(self.username) is not None

    def __str__(self):
        return (u'%s:%s'
                % (self.site(), self.name())).encode(config.console_encoding,
                                                     'replace')

    def __repr__(self):
        return self.__str__()

    def _load(self):
        getall(self.site(), [self], force=True)
        return

    def registrationTime(self, force=False):
        if not hasattr(self, '_registrationTime') or force:
            self._load()
        return self._registrationTime

    def editCount(self, force=False):
        """ Return edit count for this user as int.

        @param force: if True, forces reloading the data
        @type force: bool
        """
        if not hasattr(self, '_editcount') or force:
            self._load()
        return self._editcount

    def isBlocked(self, force=False):
        """ Return True if this user is currently blocked, False otherwise.

        @param force: if True, forces reloading the data
        @type force: bool
        """
        if not self._blocked or force:
            self._load()
        return self._blocked

    def isEmailable(self, force=False):
        """ Return True if emails can be send to this user through mediawiki,
        False otherwise.

        @param force: if True, forces reloading the data
        @type force: bool
        """
        if not hasattr(self, '_mailable'):
            self._load()
        return self._mailable

    def groups(self, force=False):
        """ Return a list of groups to wich this user belongs. The return value
        is guaranteed to be a list object, possibly empty.

        @param force: if True, forces reloading the data
        @type force: bool
        """
        if not self._groups or force:
            self._load()
        return self._groups

    def getUserPage(self, subpage=u''):
        """ Return a pywikibot.Page object corresponding to this user's main
        page, or a subpage of it if subpage is set.

        @param subpage: subpage part to be appended to the main
                            page title (optional)
        @type subpage: unicode
        """
        if self._isAutoblock:
            #This user is probably being queried for purpose of lifting
            #an autoblock, so has no user pages per se.
            raise AutoblockUser("This is an autoblock ID, you can only use to unblock it.")
        if subpage:
            subpage = u'/' + subpage
        return pywikibot.Page(self.site(), self.name() + subpage, defaultNamespace=2)

    def getUserTalkPage(self, subpage=u''):
        """ Return a pywikibot.Page object corresponding to this user's main
        talk page, or a subpage of it if subpage is set.

        @param subpage: subpage part to be appended to the main
                            talk page title (optional)
        @type subpage: unicode
        """
        if self._isAutoblock:
            #This user is probably being queried for purpose of lifting
            #an autoblock, so has no user talk pages per se.
            raise AutoblockUser("This is an autoblock ID, you can only use to unblock it.")
        if subpage:
            subpage = u'/' + subpage
        return pywikibot.Page(self.site(), self.name() + subpage,
                              defaultNamespace=3)

    def sendMail(self, subject=u'', text=u'', ccMe = False):
        """ Send an email to this user via mediawiki's email interface.
        Return True on success, False otherwise.
        This method can raise an UserActionRefuse exception in case this user
        doesn't allow sending email to him or the currently logged in bot
        doesn't have the right to send emails.

        @param subject: the subject header of the mail
        @type subject: unicode
        @param text: mail body
        @type text: unicode
        @param ccme: if True, sends a copy of this email to the bot
        @type ccme: bool
        """
        if not self.isEmailable():
            raise UserActionRefuse('This user is not mailable')
        if not self.site().isAllowed('sendemail'):
            raise UserActionRefuse('You don\'t have permission to send mail')

        if not self.site().has_api() or self.site().versionnumber() < 14:
            return self.sendMailOld(subject, text, ccMe)

        params = {
            'action': 'emailuser',
            'target': self.name(),
            'token': self.site().getToken(),
            'subject': subject,
            'text': text,
        }
        if ccMe:
            params['ccme'] = 1
        maildata = query.GetData(params, self.site())
        if 'error' in maildata:
            code = maildata['error']['code']
            if code == u'usermaildisabled ':
                pywikibot.output(u'User mail has been disabled')
        elif 'emailuser' in maildata:
            if maildata['emailuser']['result'] == u'Success':
                pywikibot.output(u'Email sent.')
                return True
        return False

    def sendMailOld(self, subject = u'', text = u'', ccMe = False):
        address = self.site().put_address('Special:EmailUser')
        predata = {
            "wpSubject" : subject,
            "wpText" : text,
            'wpSend' : "Send",
            'wpCCMe' : '0',
        }
        if ccMe:
            predata['wpCCMe'] = '1'
        predata['wpEditToken'] = self.site().getToken()

        response, data = self.site().postForm(address, predata, sysop = False)
        if data:
            if 'var wgAction = "success";' in data:
                pywikibot.output(u'Email sent.')
                return True
            else:
                pywikibot.output(u'Email not sent.')
                return False
        else:
            pywikibot.output(u'No data found.')
            return False


    @pywikibot.deprecated('contributions()')
    def editedPages(self, limit=500):
        """ Deprecated function that wraps 'contributions' for backwards
        compatibility. Yields pywikibot.Page objects that this user has
        edited, with an upper bound of 'limit'. Pages returned are not
        guaranteed to be unique.

        @param limit: limit result to this number of pages.
        @type limit: int.
        """
        for item in self.contributions(limit):
            yield item[0]

    def contributions(self, limit=500, namespace=[]):
        """ Yield tuples describing this user edits with an upper bound of
        'limit'. Each tuple is composed of a pywikibot.Page object,
        the revision id (int), the edit timestamp and the comment (unicode).
        Pages returned are not guaranteed to be unique.

        @param limit: limit result to this number of pages
        @type limit: int
        @param namespace: only iterate links in these namespaces
        @type namespace: list
        """
        if not self.site().has_api():
            raise NotImplementedError

        params = {
            'action': 'query',
            'list': 'usercontribs',
            'ucuser': self.name(),
            'ucprop': ['ids','title','timestamp','comment'],# 'size','flags'],
            'uclimit': int(limit),
            'ucdir': 'older',
        }
        if limit > pywikibot.config.special_page_limit:
            params['uclimit'] = pywikibot.config.special_page_limit
            if limit > 5000 and self.site().isAllowed('apihighlimits'):
                params['uclimit'] = 5000
        if namespace:
            params['ucnamespace'] = namespace
        # An user is likely to contribute on several pages,
        # keeping track of titles
        nbresults = 0
        while True:
            result = query.GetData(params, self.site())
            if 'error' in result:
                pywikibot.output('%s' % result)
                raise pywikibot.Error
            for contrib in result['query']['usercontribs']:
                ts = pywikibot.parsetime2stamp(contrib['timestamp'])
                yield (pywikibot.Page(self.site(), contrib['title'],
                                      defaultNamespace=contrib['ns']),
                       contrib['revid'], ts, contrib.get('comment', None)
                )
                nbresults += 1
                if nbresults >= limit:
                    break
            if 'query-continue' in result and nbresults < limit:
                params['ucstart'] = result['query-continue']['usercontribs']['ucstart']
            else:
                break
        return

    def uploadedImages(self, number=10):
        """ Yield tuples describing files uploaded by this user.
        Each tuple is composed of a pywikibot.Page, the timestamp
        comment (unicode) and a bool (always False...).
        Pages returned are not guaranteed to be unique.

        @param total: limit result to this number of pages
        @type total: int
        """
        if self.isAnonymous():
            raise StopIteration
        if not self.site().has_api() or self.site().versionnumber() < 11:
            for c in self._uploadedImagesOld(number):
                yield c
            return

        for item in self.site().logpages(number, mode='upload', user=self.username, dump=True):
            yield pywikibot.ImagePage(self.site(), item['title']), item['timestamp'], item['comment'], item['pageid'] > 0
        return

    def _uploadedImagesOld(self, number = 10):
        """Yield ImagePages from Special:Log&type=upload"""

        regexp = re.compile('<li[^>]*>(?P<date>.+?)\s+<a href=.*?>(?P<user>.+?)</a> .* uploaded "<a href=".*?"(?P<new> class="new")? title="(Image|File):(?P<image>.+?)"\s*>(?:.*?<span class="comment">(?P<comment>.*?)</span>)?', re.UNICODE)
        path = self.site().log_address(number, mode = 'upload', user = self.name())
        html = self.site().getUrl(path)
        redlink_key = self.site().mediawiki_message('red-link-title')
        redlink_tail_len = None
        if redlink_key.startswith('$1 '):
            redlink_tail_len = len(redlink_key[3:])
        for m in regexp.finditer(html):
            image = m.group('image')
            deleted = False
            if m.group('new'):
                deleted = True
                if redlink_tail_len:
                    image = image[0:0-redlink_tail_len]

            date = m.group('date')
            comment = m.group('comment') or ''
            yield pywikibot.ImagePage(self.site(), image), date, comment, deleted

    def block(self, expiry=None, reason=None, anon=True, noCreate=False,
          onAutoblock=False, banMail=False, watchUser=False, allowUsertalk=True,
          reBlock=False, hidename=False):
        """
        Block the user by API.

        Parameters (from http://en.wikipedia.org/w/api.php)
        expiry        - Expiry time of block, may be a period of time
                        or the block's expiry time
                        If set to 'infinite', 'indefinite' or 'never',
                        the block will never expire.
        reason        - Reason for block
        anon          - Block anonymous users only
        noCreate      - Prevent account creation
        onAutoblock   - Automatically block the last used IP address, and any
                        subsequent IP addresses they try to login from
        banMail       - Prevent user from sending e-mail through the wiki.
        hidename      - Hide the username from the block log. (API only)
        allowUsertalk - Allow the user to edit their own talk page
        reBlock       - If user is already blocked, overwrite the existing block
        watchUser     - watch the user's user and talk pages (not used with API)

        The default values for block options are set to as most unrestrictive
        """

        if self._isAutoblock:
            #This user is probably being queried for purpose of lifting
            #an autoblock, so can't be blocked.
            raise AutoblockUser
        if self.isBlocked() and not reBlock:
            raise AlreadyBlocked()
        if not self.site().isAllowed('block', sysop=True):
            raise UserActionRefuse('You don\'t have permission to block')
        if not expiry:
            expiry = pywikibot.input(u'Please enter the expiry time for the block:')
        if not reason:
            reason = pywikibot.input(u'Please enter a reason for the block:')

        if not self.site().has_api() or self.site().versionnumber() < 12:
            return self._blockOld(expiry, reason, anon, noCreate,
                                  onAutoblock, banMail, watchUser,
                                  allowUsertalk, reBlock)
        params = {
            'action': 'block',
            'user': self.name(),
            'token': self.site().getToken(self, sysop = True),
            'reason': reason,
        }
        if expiry:
            params['expiry'] = expiry
        if anon:
            params['anononly'] = 1
        if noCreate:
            params['nocreate'] = 1
        if onAutoblock:
            params['autoblock'] = 1
        if banMail:
            params['noemail'] = 1
        if hidename:
            params['hidename'] = 1
        if allowUsertalk:
            params['allowusertalk'] = 1
        if reBlock:
            params['reblock'] = 1

        data = query.GetData(params, self.site(), sysop=True)
        if 'error' in data: #error occured
            errCode = data['error']['code']
            if errCode == 'alreadyblocked':
                raise AlreadyBlocked()
            elif errCode == 'blockedasrange':
                raise AlreadyBlocked("Range Blocked")
            #elif errCode == 'invalidrange':
            #    pass
            elif errCode == 'invalidexpiry':
                raise BlockError("Invaild expiry")
            elif errCode == 'pastexpiry ':
                raise BlockError("expiry time is the past")
            elif errCode == 'cantblock-email':
                raise BlockError("You don't have permission to ban mail")

        elif 'block' in data: #success
                return True
        else:
            pywikibot.output("Unknown Error, result: %s" % data)
            raise BlockError
        raise False

    def _blockOld(self, expiry, reason, anonOnly, noSignup, enableAutoblock, emailBan,
                watchUser, allowUsertalk):
        """
        Internal use to block the user by web page.
        Don't use this function directly.

        """

        token = self.site().getToken(self, sysop = True)
        pywikibot.output(u"Blocking [[User:%s]]..." % self.name())
        boolStr = ['0','1']
        predata = {
            'wpBlockAddress': self.name(),
            'wpBlockOther': expiry,
            'wpBlockReasonList': reason,
            'wpAnonOnly': boolStr[anonOnly],
            'wpCreateAccount': boolStr[noSignup],
            'wpEnableAutoblock': boolStr[enableAutoblock],
            'wpEmailBan': boolStr[emailBan],
            'wpWatchUser': boolStr[watchUser],
            'wpAllowUsertalk': boolStr[allowUsertalk],
            'wpBlock': 'Block this user',
            'wpEditToken': token
        }
        address = self.site().block_address()

        response, data = self.site().postForm(address, predata, sysop = True)
        if data:
            if self.site().mediawiki_message('ipb_already_blocked').replace('$1', self.name()) in data:
                raise AlreadyBlockedError

            raise BlockError
        return True

    def unblock(self, reason=None):
        """
        Unblock the user.

        Parameter:
        reason - reason for block
        """

        if self.name()[0] == '#':
            blockID = self.name()[1:]
        else:
            blockID = self._getBlockID()
        self._unblock(blockID,reason)

    def _getBlockID(self):
        pywikibot.output(u"Getting block id for [[User:%s]]..." % self.name())
        address = self.site().blocksearch_address(self.name())
        data = self.site().getUrl(address)
        bIDre = re.search(r'action=unblock&amp;id=(\d+)', data)
        if not bIDre:
            pywikibot.output(data)
            raise BlockIDError
        return bIDre.group(1)

    def _unblock(self, blockID, reason):
        pywikibot.output(u"Unblocking [[User:%s]]..." % self.name())
        token = self.site().getToken(self, sysop = True)
        predata = {
            'id': blockID,
            'wpUnblockReason': reason,
            'wpBlock': 'Unblock this address',
            'wpEditToken': token,
        }
        address = self.site().unblock_address()
        response, data = self.site().postForm(address, predata, sysop = True)
        if response.code != 302:
            if self.site().mediawiki_message('ipb_cant_unblock').replace('$1',blockID) in data:
                raise AlreadyUnblockedError
            raise UnblockError, data
        return True

def getall(site, users, throttle=True, force=False):
    """Bulk-retrieve users data from site

    Arguments: site = Site object
               users = iterable that yields User objects

    """
    users = list(users)  # if pages is an iterator, we need to make it a list
    if len(users) > 1:
        pywikibot.output(u'Getting %d users data from %s...'
                         % (len(users), site))

    if len(users) > 250: # max load prevents HTTPError 400
        for urg in range(0, len(users), 250):
            if urg == range(0, len(users), 250)[-1]: #latest
                k = users[urg:]
                _GetAllUI(site, k, throttle, force).run()
                users[urg:] = k
            else:
                k = users[urg:urg + 250]
                _GetAllUI(site, k, throttle, force).run()
                users[urg:urg + 250] = k
    else:
        _GetAllUI(site, users, throttle, force).run()

class _GetAllUI(object):
    def __init__(self, site, users, throttle, force):
        self.site = site
        self.users = []
        self.throttle = throttle
        self.force = force
        self.sleeptime = 15
        for user in users:
            if not hasattr(user, '_editcount') or force:
                self.users.append(user)
            elif pywikibot.verbose:
                pywikibot.output(u"BUGWARNING: %s already done!" % user.name())

    def run(self):
        if self.users:
            while True:
                try:
                    data = self.getData()
                except Exception, e:
                    # Print the traceback of the caught exception
                    print e
                    raise
                else:
                    break
            for uj in self.users:
                try:
                    x = data[uj.name()]
                except KeyError:
                    break
                uj._editcount = x['editcount']
                if 'groups' in x:
                    uj._groups = x['groups']
                else:
                    uj._groups = []
                if x['registration']:
                    uj._registrationTime = pywikibot.parsetime2stamp(x['registration'])
                else:
                    uj._registrationTime = 0
                uj._mailable = ("emailable" in x)
                uj._blocked = ('blockedby' in x)
                #if self._blocked: #Get block ID

    def getData(self):
        users = {}
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['blockinfo', 'groups', 'editcount', 'registration', 'emailable', 'gender'],
            'ususers': u'|'.join([n.name() for n in self.users]),
        }
        data = query.GetData(params, self.site)
        for user in data['query']['users']:
            if u'invalid' in user:
                raise InvalidUser("User name '%s' is invalid. IP addresses are not supported." % user['name'])
            users[user['name']] = user
        return users

if __name__ == '__main__':
    """
    Simple testing code for the [[User:Example]] on the English pywikibot.
    """
    pywikibot.output("""
    This module is not for direct usage from the command prompt.
    In code, the usage is as follows:

    >>> exampleUser = User("en", 'Example')
    >>> pywikibot.output(exampleUser.getUserPage().get())
    >>> pywikibot.output(exampleUser.getUserPage('Lipsum').get())
    >>> pywikibot.output(exampleUser.getUserTalkPage().get())
    """)
    # unit tests
    import tests.test_userlib
    import unittest
    unittest.main(tests.test_userlib)
