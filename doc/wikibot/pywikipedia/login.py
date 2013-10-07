#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Script to log the robot in to a wiki account.

Suggestion is to make a special account to use for robot use only. Make
sure this robot account is well known on your home wiki before using.

Parameters:

   -all         Try to log in on all sites where a username is defined in
                user-config.py.

   -clean       Use this option for logout. In combination with -all it
                will log out on all sites where a username is defined.

   -force       Ignores if the user is already logged in, and tries to log in.

   -pass        Useful in combination with -all when you have accounts for
                several sites and use the same password for all of them.
                Asks you for the password, then logs in on all given sites.

   -pass:XXXX   Uses XXXX as password. Be careful if you use this
                parameter because your password will be shown on your
                screen, and will probably be saved in your command line
                history. This is NOT RECOMMENDED for use on computers
                where others have either physical or remote access.
                Use -pass instead.

   -sysop       Log in with your sysop account.

   -test        test whether you are logged-in

   -v -v        (Doubly verbose) Shows http requests made when logging in. This
                might leak private data (password, session id), so make sure to
                check the output. Using -log is recommended: this will output a
                lot of data

If not given as parameter, the script will ask for your username and
password (password entry will be hidden), log in to your home wiki using
this combination, and store the resulting cookies (containing your password
hash, so keep it secured!) in a file in the login-data subdirectory.

All scripts in this library will be looking for this cookie file and will
use the login information if it is present.

To log out, throw away the XX-login.data file that is created in the login-data
subdirectory.
"""
#
# (C) Rob W.W. Hooft, 2003
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: login.py 8114 2010-04-19 20:52:19Z xqt $'

import re, os, query
import urllib2
import wikipedia as pywikibot
import config

# On some wikis you are only allowed to run a bot if there is a link to
# the bot's user page in a specific list.
# If bots are listed in a template, the templates name must be given as
# second parameter, otherwise it must be None
botList = {
    'wikipedia': {
        'en': [u'Wikipedia:Bots/Status', 'BotS'],
        'simple': [u'Wikipedia:Bots', '/links']
    },
    'gentoo': {
        'en': [u'Help:Bots', None],
    }
}

def show (mysite, sysop = False):
    if mysite.loggedInAs(sysop = sysop):
        pywikibot.output(u"You are logged in on %s as %s."
                         % (repr(mysite), mysite.loggedInAs(sysop=sysop)))
    else:
        pywikibot.output(u"You are not logged in on %s." % repr(mysite))

class LoginManager:
    def __init__(self, password=None, sysop=False, site=None, username=None,
                 verbose=False):
        self.site = site or pywikibot.getSite()
        self.sysop = sysop
        if username:
            self.username = username
            # perform writeback.
            if site.family.name not in config.usernames:
                config.usernames[site.family.name]={}
            config.usernames[site.family.name][self.site.lang]=username
        else:
            if sysop:
                try:
                    self.username = config.sysopnames\
                                    [self.site.family.name][self.site.lang]
                except:
                    raise pywikibot.NoUsername(u'ERROR: Sysop username for %s:%s is undefined.\nIf you have a sysop account for that site, please add such a line to user-config.py:\n\nsysopnames[\'%s\'][\'%s\'] = \'myUsername\'' % (self.site.family.name, self.site.lang, self.site.family.name, self.site.lang))
            else:
                try:
                    self.username = config.usernames[self.site.family.name][self.site.lang]
                except:
                    raise pywikibot.NoUsername(u'ERROR: Username for %s:%s is undefined.\nIf you have an account for that site, please add such a line to user-config.py:\n\nusernames[\'%s\'][\'%s\'] = \'myUsername\'' % (self.site.family.name, self.site.lang, self.site.family.name, self.site.lang))
        self.password = password
        self.verbose = verbose
        if getattr(config, 'password_file', ''):
            self.readPassword()

    def botAllowed(self):
        """
        Checks whether the bot is listed on a specific page to comply with
        the policy on the respective wiki.
        """
        if self.site.family.name in botList \
           and self.site.language() in botList[self.site.family.name]:
            botListPageTitle, botTemplate = botList[self.site.family.name][self.site.language()]
            botListPage = pywikibot.Page(self.site, botListPageTitle)
            if botTemplate:
                for template in botListPage.templatesWithParams():
                    if template[0] == botTemplate \
                       and template[1][0] == self.username:
                        return True
            else:
                for linkedPage in botListPage.linkedPages():
                    if linkedPage.titleWithoutNamespace() == self.username:
                        return True
            return False
        else:
            # No bot policies on other sites
            return True

    def getCookie(self, api=config.use_api_login, remember=True, captcha = None):
        """
        Login to the site.

        remember    Remember login (default: True)
        captchaId   A dictionary containing the captcha id and answer, if any

        Returns cookie data if succesful, None otherwise.
        """
        if api:
            predata = {
                'action': 'login',
                'lgname': self.username,
                'lgpassword': self.password,
            }
            if self.site.family.ldapDomain:
                predata['lgdomain'] = self.site.family.ldapDomain
            address = self.site.api_address()
        else:
            predata = {
                "wpName": self.username.encode(self.site.encoding()),
                "wpPassword": self.password,
                "wpLoginattempt": "Aanmelden & Inschrijven", # dutch button label seems to work for all wikis
                "wpRemember": str(int(bool(remember))),
                "wpSkipCookieCheck": '1'
            }
            if self.site.family.ldapDomain:     # VistaPrint fix
                predata["wpDomain"] = self.site.family.ldapDomain
            if captcha:
                predata["wpCaptchaId"] = captcha['id']
                predata["wpCaptchaWord"] = captcha['answer']
            login_address = self.site.login_address()
            address = login_address + '&action=submit'

        if api:
            while True:
                # build the cookie
                L = {}
                L["cookieprefix"] = None
                index = self.site._userIndex(self.sysop)
                if self.site._cookies[index]:
                    #if user is trying re-login, update the new information
                    self.site.updateCookies(L, self.sysop)
                else:
                    # clean type login, setup the new cookies files.
                    self.site._setupCookies(L, self.sysop)
                response, data = query.GetData(predata, self.site, sysop=self.sysop, back_response = True)
                result = data['login']['result']
                if result == "NeedToken":
                    predata["lgtoken"] = data["login"]["token"]
                    if ['lgtoken'] in data['login'].keys():
                        self.site._userName[index] = data['login']['lgusername']
                        self.site._token[index] = data['login']['lgtoken'] + "+\\"
                    continue
                break
            if result != "Success":
                #if result == "NotExists":
                #
                #elif result == "WrongPass":
                #
                #elif result == "Throttled":
                #
                return False
        else:
            response, data = self.site.postData(address, self.site.urlEncode(predata), sysop=self.sysop)
            if self.verbose:
                fakepredata = predata
                fakepredata['wpPassword'] = u'XXXXX'
                pywikibot.output(u"self.site.postData(%s, %s)" % (address, self.site.urlEncode(fakepredata)))
                trans = config.transliterate
                config.transliterate = False #transliteration breaks for some reason
                #pywikibot.output(fakedata.decode(self.site.encoding()))
                config.transliterate = trans
                fakeresponsemsg = re.sub(r"(session|Token)=..........", r"session=XXXXXXXXXX", data)
                pywikibot.output(u"%s/%s\n%s" % (response.code, response.msg, fakeresponsemsg))
            #pywikibot.cj.save(pywikibot.COOKIEFILE)

        Reat=re.compile(': (.*?)=(.*?);')

        L = {}
        if hasattr(response, 'sheaders'):
            ck = response.sheaders
        else:
            ck = response.info().getallmatchingheaders('set-cookie')
        for eat in ck:
            m = Reat.search(eat)
            if m:
                L[m.group(1)] = m.group(2)

        got_token = got_user = False
        for Ldata in L.keys():
            if 'Token' in Ldata:
                got_token = True
            if 'User' in Ldata or 'UserName' in Ldata:
                got_user = True

        if got_token and got_user:
            #process the basic information to Site()
            index = self.site._userIndex(self.sysop)
            if api:
                #API result came back username, token and sessions.
                self.site._userName[index] = data['login']['lgusername']
                self.site._token[index] = data['login']['lgtoken'] + "+\\"
            else:
                self.site._userName[index] = self.username

            if self.site._cookies[index]:
                #if user is trying re-login, update the new information
                self.site.updateCookies(L, self.sysop)
            else:
                # clean type login, setup the new cookies files.
                self.site._setupCookies(L, self.sysop)

            return True
        elif not captcha:
            solve = self.site.solveCaptcha(data)
            if solve:
                return self.getCookie(api = api, remember = remember, captcha = solve)
        return None

    def storecookiedata(self, filename, data):
        """
        Store cookie data.

        The argument data is the raw data, as returned by getCookie().

        Returns nothing.
        """
        s = u''
        for v, k in data.iteritems():
            s += "%s=%s\n" % (v, k)
        f = open(pywikibot.config.datafilepath('login-data',filename), 'w')
        f.write(s)
        f.close()

    def readPassword(self):
        """
        Read passwords from a file.

        DO NOT FORGET TO REMOVE READ ACCESS FOR OTHER USERS!!!
        Use chmod 600 password-file.
        All lines below should be valid Python tuples in the form
        (code, family, username, password) or (username, password)
        to set a default password for an username. Default usernames
        should occur above specific usernames.

        Example:

        ("my_username", "my_default_password")
        ("my_sysop_user", "my_sysop_password")
        ("en", "wikipedia", "my_en_user", "my_en_pass")
        """
        password_f = open(pywikibot.config.datafilepath(config.password_file), 'r')
        for line in password_f:
            if not line.strip(): continue
            entry = eval(line)
            if len(entry) == 2:   #for default userinfo
                if entry[0] == self.username: self.password = entry[1]
            elif len(entry) == 4: #for userinfo included code and family
                if entry[0] == self.site.lang and \
                  entry[1] == self.site.family.name and \
                  entry[2] == self.username:
                    self.password = entry[3]
        password_f.close()

    def login(self, api=config.use_api_login, retry = False):
        if not self.password:
            # As we don't want the password to appear on the screen, we set
            # password = True
            self.password = pywikibot.input(
                                u'Password for user %(name)s on %(site)s:'
                                % {'name': self.username, 'site': self.site},
                                password = True)

        self.password = self.password.encode(self.site.encoding())

        if api:
            pywikibot.output(u"Logging in to %(site)s as %(name)s via API."
                             % {'name': self.username, 'site': self.site})
        else:
            pywikibot.output(u"Logging in to %(site)s as %(name)s"
                             % {'name': self.username, 'site': self.site})

        try:
            cookiedata = self.getCookie(api)
        except NotImplementedError:
            pywikibot.output('API disabled because this site does not support.\nRetrying by ordinary way...')
            api = False
            return self.login(False, retry)
        if cookiedata:
            fn = '%s-%s-%s-login.data' % (self.site.family.name, self.site.lang, self.username)
            #self.storecookiedata(fn,cookiedata)
            pywikibot.output(u"Should be logged in now")
            # Show a warning according to the local bot policy
            if not self.botAllowed():
                pywikibot.output(u'*** Your username is not listed on [[%s]].\n*** Please make sure you are allowed to use the robot before actually using it!' % botList[self.site.family.name][self.site.lang])
            return True
        else:
            pywikibot.output(u"Login failed. Wrong password or CAPTCHA answer?")
            if api:
                pywikibot.output(u"API login failed, retrying using standard webpage.")
                return self.login(False, retry)

            if retry:
                self.password = None
                return self.login(api, True)
            else:
                return False

    def logout(self, api = config.use_api):
        flushCk = False
        if api and self.site.versionnumber() >= 12:
            if query.GetData({'action':'logout'}, self.site) == []:
                flushCk = True
        else:
            text = self.site.getUrl(self.site.get_address("Special:UserLogout"))
            if self.site.mediawiki_message('logouttext') in text: #confirm loggedout
                flushCk = True

        if flushCk:
            self.site._removeCookies(self.username)
            return True

        return False

    def showCaptchaWindow(self, url):
        pass

def main():
    username = password = None
    sysop = False
    logall = False
    forceLogin = False
    verbose = False
    clean = False
    testonly = False

    for arg in pywikibot.handleArgs():
        if arg.startswith("-pass"):
            if len(arg) == 5:
                password = pywikibot.input(u'Password for all accounts:',
                                           password = True)
            else:
                password = arg[6:]
        elif arg == "-clean":
            clean = True
        elif arg == "-sysop":
            sysop = True
        elif arg == "-all":
            logall = True
        elif arg == "-force":
            forceLogin = True
        elif arg == "-test":
            testonly = True
        else:
            pywikibot.showHelp('login')
            return

    if pywikibot.verbose > 1:
        pywikibot.output(u"WARNING: Using -v -v on login.py might leak private data. When sharing, please double check your password is not readable and log out your bots session.")
        verbose = True # only use this verbose when running from login.py
    if logall:
        if sysop:
            namedict = config.sysopnames
        else:
            namedict = config.usernames

        for familyName in namedict.iterkeys():
            for lang in namedict[familyName].iterkeys():
                if testonly:
                    show(pywikibot.getSite(lang, familyName), sysop)
                else:
                    try:
                        site = pywikibot.getSite(lang, familyName)
                        loginMan = LoginManager(password, sysop=sysop,
                                                site=site, verbose=verbose)
                        if clean:
                            loginMan.logout()
                        else:
                            if not forceLogin and site.loggedInAs(sysop = sysop):
                                pywikibot.output(u'Already logged in on %s' % site)
                            else:
                                loginMan.login()
                    except pywikibot.NoSuchSite:
                        pywikibot.output(lang+ u'.' + familyName + u' is not a valid site, please remove it from your config')

    elif testonly:
        show(pywikibot.getSite(), sysop)
    elif clean:
        try:
            site = pywikibot.getSite()
            lgm = LoginManager(site = site)
            lgm.logout()
        except pywikibot.NoSuchSite:
            pass
    else:
        loginMan = LoginManager(password, sysop = sysop, verbose=verbose)
        loginMan.login()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
