# -*- coding: utf-8  -*-
"""
This bot is used for checking external links found at the wiki. It checks
several pages at once, with a limit set by the config variable
max_external_links, which defaults to 50.

The bot won't change any wiki pages, it will only report dead links such that
people can fix or remove the links themselves.

The bot will store all links found dead in a .dat file in the deadlinks
subdirectory. To avoid the removing of links which are only temporarily
unavailable, the bot ONLY reports links which were reported dead at least
two times, with a time lag of at least one week. Such links will be logged to a
.txt file in the deadlinks subdirectory.

After running the bot and waiting for at least one week, you can re-check those
pages where dead links were found, using the -repeat parameter.

In addition to the logging step, it is possible to automatically report dead
links to the talk page of the article where the link was found. To use this
feature, set report_dead_links_on_talk = True in your user-config.py, or
specify "-talk" on the command line. Adding "-notalk" switches this off
irrespective of the configuration variable.

When a link is found alive, it will be removed from the .dat file.

These command line parameters can be used to specify which pages to work on:

&params;

-repeat      Work on all pages were dead links were found before. This is
             useful to confirm that the links are dead after some time (at
             least one week), which is required before the script will report
             the problem.

-namespace   Only process templates in the namespace with the given number or
             name. This parameter may be used multiple times.

-ignore      HTTP return codes to ignore. Can be provided several times :
                -ignore:401 -ignore:500

Furthermore, the following command line parameters are supported:

-talk        Overrides the report_dead_links_on_talk config variable, enabling
             the feature.

-notalk      Overrides the report_dead_links_on_talk config variable, disabling
             the feature.
-day         the first time found dead link longer than x day ago, it should
             probably be fixed or removed. if no set, default is 7 day.

All other parameters will be regarded as part of the title of a single page,
and the bot will only work on that single page.

The following config variables are supported:

max_external_links        - The maximum number of web pages that should be
                            loaded simultaneously. You should change this
                            according to your Internet connection speed.
                            Be careful: if it is set too high, the script
                            might get socket errors because your network
                            is congested, and will then think that the page
                            is offline.

report_dead_links_on_talk - If set to true, causes the script to report dead
                            links on the article's talk page if (and ONLY if)
                            the linked page has been unavailable at least two
                            times during a timespan of at least one week.

Syntax examples:
    python weblinkchecker.py -start:!
        Loads all wiki pages in alphabetical order using the Special:Allpages
        feature.

    python weblinkchecker.py -start:Example_page
        Loads all wiki pages using the Special:Allpages feature, starting at
        "Example page"

    python weblinkchecker.py -weblink:www.example.org
        Loads all wiki pages that link to www.example.org

    python weblinkchecker.py Example page
        Only checks links found in the wiki page "Example page"

    python weblinkchecker.py -repeat
        Loads all wiki pages where dead links were found during a prior run
"""

#
# (C) Daniel Herding, 2005
# (C) Pywikipedia bot team, 2005-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: weblinkchecker.py 9042 2011-03-13 10:14:47Z xqt $'

import sys, re
import codecs, pickle
import httplib, socket, urlparse, urllib, urllib2
import threading, time
import wikipedia as pywikibot
import config, pagegenerators
try:
    set # introduced in Python 2.4: faster and future
except NameError:
    from sets import Set as set

docuReplacements = {
    '&params;': pagegenerators.parameterHelp
}

talk_report_msg = {
    'ar': u'روبوت: الإبلاغ عن وصلات خارجية غير متوفرة',
    'de': u'Bot: Berichte nicht verfügbaren Weblink',
    'en': u'Robot: Reporting unavailable external link',
    'fa': u'ربات:گزارش پیوند غیرقابل دسترسی',
    'fr': u'Robot : Rapporte lien externe inaccessible',
    'he': u'בוט: מדווח על קישור חיצוני בלתי זמין',
    'ia': u'Robot: Reporto de un ligamine externe non functionante',
    'kk': u'Бот: Қатынаулы емес сілтеме туралы есеп беру',
    'ksh': u'Bot: Ene Weblengk jeijt nit mih.',
    'ja': u'ロボットによる: 利用不可能な外部リンクの報告',
    'nds': u'Lenk-Bot: Weblenk geiht nich mehr',
    'nl': u'Bot: melding (tijdelijk) onbereikbare externe verwijzing',
    'no': u'bot: Rapporter død eksternlenke',
    'pl': u'Robot zgłasza niedostępny link zewnętrzny',
    'pt': u'Bot: Link externo não funcionando',
    'sr': u'Бот: Пријављивање непостојећих спољашњих повезница',
    'uk': u'Бот: Сповіщення про мертві зовнішні посилання',
    'zh': u'BOT: 报告失效的外部链接',
}

# The first %s will be replaced by the URL and the error report.
# The second %s will be replaced by a hint to the Internet Archive,
# in case the page has been archived there.
talk_report = {
    'ar': u'== %s ==\n\nخلال عدة عمليات أوتوماتيكية من البوت الوصلة الخارجية التالية كانت غير متوفرة. من فضلك تحقق من أن الوصلة لا تعمل وأزلها أو أصلحها في هذه الحالة!\n\n%s\n%s--~~~~',
    'de': u'== %s ==\n\nBei mehreren automatisierten Botläufen wurde der folgende Weblink als nicht verfügbar erkannt. Bitte überprüfe, ob der Link tatsächlich unerreichbar ist, und korrigiere oder entferne ihn in diesem Fall!\n\n%s\n%s--~~~~',
    'en': u'== %s ==\n\nDuring several automated bot runs the following external link was found to be unavailable. Please check if the link is in fact down and fix or remove it in that case!\n\n%s\n%s--~~~~',
    'fa': u'== %s ==\n\nبر طبق بررسی‌های رباتیکی من چندین پیوند غیرقابل دسترس پیدا شد. لطفا آنها بررسی و در صورت لزوم درستش کنید.تشکر!\n\n%s\n%s--~~~~',
    'fr': u'== %s ==\n\nPendant plusieurs patrouilles par un robot, le lien suivant a été inaccessible. Veuillez vérifier si le lien est effectivement mort et si oui corrigez ou retirez-le.\n\n%s\n%s--~~~~',
    'he': u'== %s ==\n\nבמהלך מספר ריצות אוטומטיות של הבוט, נמצא שהקישור החיצוני הבא אינו זמין. אנא בדקו אם הקישור אכן שבור, ותקנו אותו או הסירו אותו במקרה זה!\n\n%s\n%s--~~~~',
    'ia': u'== %s ==\n\nDurante plure sessiones automatic, le robot ha constatate que le sequente ligamine externe non es disponibile. Per favor confirma que le ligamine de facto es defuncte, e in caso de si, repara o elimina lo!\n\n%s\n%s--~~~~',
    'kk': u'== %s ==\n\nӨздікті бот бірнеше жегілгенде келесі сыртқы сілтемеге қатынай алмады. Бұл сілтеменің қатыналуын тексеріп шығыңыз да, не түзетіңіз, не аластаңыз!\n\n%s\n%s--~~~~',
    'ksh': u'== %s ==\n\nEsch han bonge die Weblingks paa Mol jetschäck. Se han allemoolde nit jedon Doht ens donnoh loore, un dä Lengk reparreere odo eruß nämme.\n\n%s\n%s--~~~~',
    'ja': u'== %s ==\n\nボットが何度か自動運行する間に、以下の外部リンクが利用不可能であると判明しました。本当にリンク切れしているかを調べて、修復するか取り除いてください!\n\n%s\n%s--~~~~',
    'nds': u'== %s ==\n\nDe Bot hett en poor Mal al versöcht, disse Siet optoropen un kunn dor nich bikamen. Schall man een nakieken, wat de Siet noch dor is un den Lenk richten oder rutnehmen.\n\n%s\n%s--~~~~',
    'nl': u'== %s ==\nTijdens enkele automatische controles bleek de onderstaande externe verwijzing onbereikbaar. Controleer alstublieft of de verwijzing inderdaad onbereikbaar is. Verwijder deze tekst alstublieft na een succesvolle controle of na het verwijderen of corrigeren van de externe verwijzing.\n\n%s\n%s--~~~~[[Categorie:Wikipedia:Onbereikbare externe link]]',
    # This is not a good solution as it only works on the Norwegian Wikipedia, not on Wiktionary etc.
    'no': u'%s{{subst:Bruker:JhsBot/Død lenke}}\n\n%s\n%s~~~~\n\n{{ødelagt lenke}}',
    'pl': u'{{Martwy link dyskusja|numer=%s|link=%s|IA=%s}}',
    'pt': u'== %s ==\n\nFoi checado os links externos deste artigo por vários minutos. Alguém verifique por favor se a ligação estiver fora do ar e tente arrumá-lo ou removê-la!\n\n%s\n --~~~~ ',
    'sr': u'== %s ==\n\nТоком неколико аутоматски провера, бот је пронашао покварене спољашње повезнице. Молимо вас проверите да ли је повезница добра, поправите је или је уклоните!\n\n%s\n%s--~~~~',
    'uk': u'== %s ==\n\nПротягом кількох автоматичних перевірок наступне зовнішнє посилання було недоступне. Будь ласка, перевірте чи посилання справді "мертве" і в такому випадку виправіть або видаліть його!\n\n%s\n%s--~~~~',
    'zh': u'== %s ==\n\n一个自动运行的bot发现下列外部链接可能已经失效。请帮助修复错误的链接或者移除它!\n\n%s\n%s--~~~~',
}

talk_report_caption = {
    'ar': u'وصلة ميتة',
    'de': u'Toter Weblink',
    'en': u'Dead link',
    'fa': u'پیوند مرده',
    'fr': u'Lien mort',
    'he': u'קישור שבור',
    'ia': u'Ligamine defuncte',
    'kk': u'Өлі сілтем',
    'ksh': u'Han enne kappodde Weblengk jefonge',
    'ja': u'リンク切れ',
    'nds': u'Weblenk geiht nich mehr',
    'nl': u'Dode verwijzing',
    'no': u'',
    'pl': u'',
    'pt': u'Link quebrado',
    'sr': u'Покварене спољашње повезнице',
    'uk': u'Недоступне зовнішнє посилання',
    'zh': u'失效链接',
}

talk_report_archive = {
    'ar': u'\nصفحة الويب تم حفظها بواسطة أرشيف الإنترنت. من فضلك ضع في الاعتبار الوصل لنسخة مؤرشفة مناسبة: [%s]. ',
    'de': u'Die Webseite wurde vom Internet Archive gespeichert. Bitte verlinke gegebenenfalls eine geeignete archivierte Version: [%s]. ',
    'en': u'\nThe web page has been saved by the Internet Archive. Please consider linking to an appropriate archived version: [%s]. ',
    'fa': u'\nوب‌گاه اینترنت آرشیو یک نسخه بایگانی شده از این پیوند دارد لطفا از آن استفاده نمایید:[%s]',
    'fr': u"\nLa page a été sauvegardée dans l’''Internet Archive''. Il serait peut-être utile de faire pointer le lien vers une des versions archivées : [%s]. ",
    'he': u'\nעמוד האינטרנט נשמר על־ידי ארכיון האינטרנט. אנא שקלו לקשר לגרסה המאורכבת המתאימה: [%s]',
    'kk': u'\nБұл ғаламтордың беті Интернет Мұрағатында сақталған. Мұрағатталған нұсқасына сәйкесті сілтеуді ескеріңіз: [%s]. ',
    'ksh': u"De Websick es em ''Internet Archive'' faßjehallde. Kannß jo felleijsj_obb_en Koppi doh verlengke, süsh hee: [%s]. ",
    'ja': u'\nウェブ・ページはインターネット・アーカイブによって保存されました。アーカイブに保管された適切なバージョンにリンクすることを検討してください: [%s]. ',
    'nl': u'\nDeze website is bewaard in het Internet Archive. Overweeg te verwijzen naar een gearchiveerde pagina: [%s]. ',
    'no': u'\nDenne nettsiden er lagra i Internet Archive. Vurder om lenka kan endres til å peke til en av de arkiverte versjonene: [%s]. ',
    'pl': u'%s',
    'pt': u'Esta página web foi gravada na Internet Archive. Por favor considere o link para a versão arquivada: [%s]. ',
    'uk': u'\nВеб-сторінка була збережена у Internet Archive. Будь ласка, подумайте над заміною посилання на відповідну збережену версію: [%s]. ',
    'zh': u'这个网页已经被保存在互联网档案馆（Internet Archive）。请为该网页提供一个合适的存档版本： [%s]。',
}

ignorelist = [
    # Officialy reserved for testing, documentation, etc. in
    # http://tools.ietf.org/html/rfc2606#page-2
    # top-level domains:
    re.compile('.*[\./@]test(/.*)?'),
    re.compile('.*[\./@]example(/.*)?'),
    re.compile('.*[\./@]invalid(/.*)?'),
    re.compile('.*[\./@]localhost(/.*)?'),
    # second-level domains:
    re.compile('.*[\./@]example\.com(/.*)?'),
    re.compile('.*[\./@]example\.net(/.*)?'),
    re.compile('.*[\./@]example\.org(/.*)?'),

    # Other special cases
    re.compile('.*[\./@]gso\.gbv\.de(/.*)?'),  # bot somehow can't handle their redirects
    re.compile('.*[\./@]berlinonline\.de(/.*)?'), # a de: user wants to fix them by hand and doesn't want them to be deleted, see [[de:Benutzer:BLueFiSH.as/BZ]].
    re.compile('.*[\./@]bodo\.kommune\.no(/.*)?'), # bot can't handle their redirects
    re.compile('.*[\./@]jpl\.nasa\.gov(/.*)?'), # bot rejected on the site
    re.compile('.*[\./@]itis\.gov(/.*)?'), # bot rejected on the site
    re.compile('.*[\./@]cev\.lu(/.*)?'), # bot rejected on the site
    re.compile('.*[\./@]science\.ksc\.nasa\.gov(/.*)?'), # very slow response resulting in bot error
]

def weblinksIn(text, withoutBracketed = False, onlyBracketed = False):
    text = pywikibot.removeDisabledParts(text)

    # MediaWiki parses templates before parsing external links. Thus, there
    # might be a | or a } directly after a URL which does not belong to
    # the URL itself.

    # First, remove the curly braces of inner templates:
    nestedTemplateR = re.compile(r'{{([^}]*?){{(.*?)}}(.*?)}}')
    while nestedTemplateR.search(text):
        text = nestedTemplateR.sub(r'{{\1 \2 \3}}', text)

    # Then blow up the templates with spaces so that the | and }} will not be regarded as part of the link:.
    templateWithParamsR = re.compile(r'{{([^}]*?[^ ])\|([^ ][^}]*?)}}',
                                     re.DOTALL)
    while templateWithParamsR.search(text):
        text = templateWithParamsR.sub(r'{{ \1 | \2 }}', text)

    linkR = pywikibot.compileLinkR(withoutBracketed, onlyBracketed)

    # Remove HTML comments in URLs as well as URLs in HTML comments.
    # Also remove text inside nowiki links etc.
    text = pywikibot.removeDisabledParts(text)
    for m in linkR.finditer(text):
        yield m.group('url')

class InternetArchiveConsulter:
    def __init__(self, url):
        self.url = url

    def getArchiveURL(self):
        pywikibot.output(u'Consulting the Internet Archive for %s' % self.url)
        archiveURL = 'http://web.archive.org/web/*/%s' % self.url
        try:
            f = urllib2.urlopen(archiveURL)
        except urllib2.HTTPError:
            # The Internet Archive yields a 403 error when the site was not
            # archived due to robots.txt restrictions.
            return None
        except UnicodeEncodeError:
            return None
        data = f.read()
        if f.headers.get('content-encoding', None) == 'gzip':
            # Since 2008, the Internet Archive returns pages in GZIPed
            # compression format. Unfortunatelly urllib2 doesn't handle
            # the decompression for us, so we have to do it ourselves.
            import gzip, StringIO
            data = gzip.GzipFile(fileobj=StringIO.StringIO(data)).read()
        if "Search Results for " in data:
            return archiveURL
        else:
            return None

class LinkChecker(object):
    '''
    Given a HTTP URL, tries to load the page from the Internet and checks if it
    is still online.

    Returns a (boolean, string) tuple saying if the page is online and including
    a status reason.

    Warning: Also returns false if your Internet connection isn't working
    correctly! (This will give a Socket Error)
    '''
    def __init__(self, url, redirectChain = [], serverEncoding=None, HTTPignore=[]):
        """
        redirectChain is a list of redirects which were resolved by
        resolveRedirect(). This is needed to detect redirect loops.
        """
        self.url = url
        self.serverEncoding = serverEncoding
        self.header = {
            # 'User-agent': pywikibot.useragent,
            # we fake being Firefox because some webservers block unknown
            # clients, e.g. http://images.google.de/images?q=Albit gives a 403
            # when using the PyWikipediaBot user agent.
            'User-agent': 'Mozilla/5.0 (X11; U; Linux i686; de; rv:1.8) Gecko/20051128 SUSE/1.5-0.1 Firefox/1.5',
            'Accept': 'text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5',
            'Accept-Language': 'de-de,de;q=0.8,en-us;q=0.5,en;q=0.3',
            'Accept-Charset': 'ISO-8859-1,utf-8;q=0.7,*;q=0.7',
            'Keep-Alive': '30',
            'Connection': 'keep-alive',
        }
        self.redirectChain = redirectChain + [url]
        self.changeUrl(url)
        self.HTTPignore = HTTPignore

    def getConnection(self):
        if self.scheme == 'http':
            return httplib.HTTPConnection(self.host)
        elif self.scheme == 'https':
            return httplib.HTTPSConnection(self.host)

    def getEncodingUsedByServer(self):
        if not self.serverEncoding:
            try:
                pywikibot.output(
                    u'Contacting server %s to find out its default encoding...'
                    % self.host)
                conn = self.getConnection()
                conn.request('HEAD', '/', None, self.header)
                response = conn.getresponse()

                self.readEncodingFromResponse(response)
            except:
                pass
            if not self.serverEncoding:
                # TODO: We might also load a page, then check for an encoding
                # definition in a HTML meta tag.
                pywikibot.output(
                    u'Error retrieving server\'s default charset. Using ISO 8859-1.')
                # most browsers use ISO 8859-1 (Latin-1) as the default.
                self.serverEncoding = 'iso8859-1'
        return self.serverEncoding

    def readEncodingFromResponse(self, response):
        if not self.serverEncoding:
            try:
                ct = response.getheader('Content-Type')
                charsetR = re.compile('charset=(.+)')
                charset = charsetR.search(ct).group(1)
                self.serverEncoding = charset
            except:
                pass

    def changeUrl(self, url):
        self.url = url
        # we ignore the fragment
        self.scheme, self.host, self.path, self.query, self.fragment = urlparse.urlsplit(self.url)
        if not self.path:
            self.path = '/'
        if self.query:
            self.query = '?' + self.query
        self.protocol = url.split(':', 1)[0]
        # check if there are non-ASCII characters inside path or query, and if
        # so, encode them in an encoding that hopefully is the right one.
        try:
            self.path.encode('ascii')
            self.query.encode('ascii')
        except UnicodeEncodeError:
            encoding = self.getEncodingUsedByServer()
            self.path = unicode(urllib.quote(self.path.encode(encoding)))
            self.query = unicode(urllib.quote(self.query.encode(encoding), '=&'))

    def resolveRedirect(self, useHEAD = False):
        '''
        Requests the header from the server. If the page is an HTTP redirect,
        returns the redirect target URL as a string. Otherwise returns None.

        If useHEAD is true, uses the HTTP HEAD method, which saves bandwidth
        by not downloading the body. Otherwise, the HTTP GET method is used.
        '''
        conn = self.getConnection()
        try:
            if useHEAD:
                conn.request('HEAD', '%s%s' % (self.path, self.query), None,
                             self.header)
            else:
                conn.request('GET', '%s%s' % (self.path, self.query), None,
                             self.header)
            response = conn.getresponse()
            # read the server's encoding, in case we need it later
            self.readEncodingFromResponse(response)
        except httplib.BadStatusLine:
            # Some servers don't seem to handle HEAD requests properly,
            # e.g. http://www.radiorus.ru/ which is running on a very old
            # Apache server. Using GET instead works on these (but it uses
            # more bandwidth).
            if useHEAD:
                return self.resolveRedirect(useHEAD = False)
            else:
                raise
        if response.status >= 300 and response.status <= 399:
            #print response.getheaders()
            redirTarget = response.getheader('Location')
            if redirTarget:
                try:
                    redirTarget.encode('ascii')
                except UnicodeError:
                    redirTarget = redirTarget.decode(
                        self.getEncodingUsedByServer())
                if redirTarget.startswith('http://') or \
                   redirTarget.startswith('https://'):
                    self.changeUrl(redirTarget)
                    return True
                elif redirTarget.startswith('/'):
                    self.changeUrl(u'%s://%s%s'
                                   % (self.protocol, self.host, redirTarget))
                    return True
                else: # redirect to relative position
                    # cut off filename
                    directory = self.path[:self.path.rindex('/') + 1]
                    # handle redirect to parent directory
                    while redirTarget.startswith('../'):
                        redirTarget = redirTarget[3:]
                        # some servers redirect to .. although we are already
                        # in the root directory; ignore this.
                        if directory != '/':
                            # change /foo/bar/ to /foo/
                            directory = directory[:-1]
                            directory = directory[:directory.rindex('/') + 1]
                    self.changeUrl('%s://%s%s%s'
                                   % (self.protocol, self.host, directory,
                                      redirTarget))
                    return True
        else:
            return False # not a redirect

    def check(self, useHEAD = False):
        """
        Returns True and the server status message if the page is alive.
        Otherwise returns false
        """
        try:
            wasRedirected = self.resolveRedirect(useHEAD = useHEAD)
        except UnicodeError, error:
            return False, u'Encoding Error: %s (%s)' \
                   % (error.__class__.__name__, unicode(error))
        except httplib.error, error:
            return False, u'HTTP Error: %s' % error.__class__.__name__
        except socket.error, error:
            # http://docs.python.org/lib/module-socket.html :
            # socket.error :
            # The accompanying value is either a string telling what went
            # wrong or a pair (errno, string) representing an error
            # returned by a system call, similar to the value
            # accompanying os.error
            if isinstance(error, basestring):
                msg = error
            else:
                try:
                    msg = error[1]
                except IndexError:
                    print u'### DEBUG information for #2972249'
                    raise IndexError, type(error)
            # TODO: decode msg. On Linux, it's encoded in UTF-8.
            # How is it encoded in Windows? Or can we somehow just
            # get the English message?
            return False, u'Socket Error: %s' % repr(msg)
        if wasRedirected:
            if self.url in self.redirectChain:
                if useHEAD:
                    # Some servers don't seem to handle HEAD requests properly,
                    # which leads to a cyclic list of redirects.
                    # We simply start from the beginning, but this time,
                    # we don't use HEAD, but GET requests.
                    redirChecker = LinkChecker(self.redirectChain[0],
                                               serverEncoding=self.serverEncoding,
                                               HTTPignore=self.HTTPignore)
                    return redirChecker.check(useHEAD = False)
                else:
                    urlList = ['[%s]' % url for url in self.redirectChain + [self.url]]
                    return False, u'HTTP Redirect Loop: %s' % ' -> '.join(urlList)
            elif len(self.redirectChain) >= 19:
                if useHEAD:
                    # Some servers don't seem to handle HEAD requests properly,
                    # which leads to a long (or infinite) list of redirects.
                    # We simply start from the beginning, but this time,
                    # we don't use HEAD, but GET requests.
                    redirChecker = LinkChecker(self.redirectChain[0],
                                               serverEncoding=self.serverEncoding,
                                               HTTPignore = self.HTTPignore)
                    return redirChecker.check(useHEAD = False)
                else:
                    urlList = ['[%s]' % url for url in self.redirectChain + [self.url]]
                    return False, u'Long Chain of Redirects: %s' % ' -> '.join(urlList)
            else:
                redirChecker = LinkChecker(self.url, self.redirectChain,
                                           self.serverEncoding,
                                           HTTPignore=self.HTTPignore)
                return redirChecker.check(useHEAD = useHEAD)
        else:
            try:
                conn = self.getConnection()
            except httplib.error, error:
                return False, u'HTTP Error: %s' % error.__class__.__name__
            try:
                conn.request('GET', '%s%s'
                             % (self.path, self.query), None, self.header)
            except socket.error, error:
                return False, u'Socket Error: %s' % repr(error[1])
            try:
                response = conn.getresponse()
            except Exception, error:
                return False, u'Error: %s' % error
            # read the server's encoding, in case we need it later
            self.readEncodingFromResponse(response)
            # site down if the server status is between 400 and 499
            alive = response.status not in range(400, 500)
            if response.status in self.HTTPignore:
                alive = False
            return alive, '%s %s' % (response.status, response.reason)

class LinkCheckThread(threading.Thread):
    '''
    A thread responsible for checking one URL. After checking the page, it
    will die.
    '''
    def __init__(self, page, url, history, HTTPignore):
        threading.Thread.__init__(self)
        self.page = page
        self.url = url
        self.history = history
        # identification for debugging purposes
        self.setName((u'%s - %s' % (page.title(), url)).encode('utf-8', 'replace'))
        self.HTTPignore = HTTPignore

    def run(self):
        linkChecker = LinkChecker(self.url, HTTPignore = self.HTTPignore)
        try:
            ok, message = linkChecker.check()
        except:
            pywikibot.output('Exception while processing URL %s in page %s'
                             % (self.url, self.page.title()))
            raise
        if ok:
            if self.history.setLinkAlive(self.url):
                pywikibot.output('*Link to %s in [[%s]] is back alive.'
                                 % (self.url, self.page.title()))
        else:
            pywikibot.output('*[[%s]] links to %s - %s.'
                             % (self.page.title(), self.url, message))
            self.history.setLinkDead(self.url, message, self.page, day)


class History:
    ''' Stores previously found dead links. The URLs are dictionary keys, and
    values are lists of tuples where each tuple represents one time the URL was
    found dead. Tuples have the form (title, date, error) where title is the
    wiki page where the URL was found, date is an instance of time, and error is
    a string with error code and message.

    We assume that the first element in the list represents the first time we
    found this dead link, and the last element represents the last time.

    Example:

    dict = {
        'http://www.example.org/page': [
            ('WikiPageTitle', DATE, '404: File not found'),
            ('WikiPageName2', DATE, '404: File not found'),
        ]

    '''

    def __init__(self, reportThread):
        self.reportThread = reportThread
        site = pywikibot.getSite()
        self.semaphore = threading.Semaphore()
        self.datfilename = pywikibot.config.datafilepath('deadlinks',
                               'deadlinks-%s-%s.dat'
                               % (site.family.name, site.lang))
        # Count the number of logged links, so that we can insert captions
        # from time to time
        self.logCount = 0
        try:
            datfile = open(self.datfilename, 'r')
            self.historyDict = pickle.load(datfile)
            datfile.close()
        except (IOError, EOFError):
            # no saved history exists yet, or history dump broken
            self.historyDict = {}

    def log(self, url, error, containingPage, archiveURL):
        """
        Logs an error report to a text file in the deadlinks subdirectory.
        """
        site = pywikibot.getSite()
        if archiveURL:
            errorReport = u'* %s ([%s archive])\n' % (url, archiveURL)
        else:
            errorReport = u'* %s\n' % url
        for (pageTitle, date, error) in self.historyDict[url]:
            # ISO 8601 formulation
            isoDate = time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(date))
            errorReport += "** In [[%s]] on %s, %s\n" % (pageTitle, isoDate,
                                                         error)
        pywikibot.output(u"** Logging link for deletion.")
        txtfilename = pywikibot.config.datafilepath('deadlinks',
                                                    'results-%s-%s.txt'
                                                    % (site.family.name,
                                                       site.lang))
        txtfile = codecs.open(txtfilename, 'a', 'utf-8')
        self.logCount += 1
        if self.logCount % 30 == 0:
            # insert a caption
            txtfile.write('=== %s ===\n' % containingPage.title()[:3])
        txtfile.write(errorReport)
        txtfile.close()

        if self.reportThread and not containingPage.isTalkPage():
            self.reportThread.report(url, errorReport, containingPage,
                                     archiveURL)


    def setLinkDead(self, url, error, page, day):
        """
        Adds the fact that the link was found dead to the .dat file.
        """
        self.semaphore.acquire()
        now = time.time()
        if url in self.historyDict:
            timeSinceFirstFound = now - self.historyDict[url][0][1]
            timeSinceLastFound= now - self.historyDict[url][-1][1]
            # if the last time we found this dead link is less than an hour
            # ago, we won't save it in the history this time.
            if timeSinceLastFound > 60 * 60:
                self.historyDict[url].append((page.title(), now, error))
            # if the first time we found this link longer than x day ago
            # (default is a week), it should probably be fixed or removed.
            # We'll list it in a file so that it can be removed manually.
            if timeSinceFirstFound > 60 * 60 * 24 * day:
                # search for archived page
                iac = InternetArchiveConsulter(url)
                archiveURL = iac.getArchiveURL()
                self.log(url, error, page, archiveURL)
        else:
            self.historyDict[url] = [(page.title(), now, error)]
        self.semaphore.release()

    def setLinkAlive(self, url):
        """
        If the link was previously found dead, removes it from the .dat file
        and returns True, else returns False.
        """
        if url in self.historyDict:
            self.semaphore.acquire()
            try:
                del self.historyDict[url]
            except KeyError:
                # Not sure why this can happen, but I guess we can ignore this...
                pass
            self.semaphore.release()
            return True
        else:
            return False

    def save(self):
        """
        Saves the .dat file to disk.
        """
        datfile = open(self.datfilename, 'w')
        pickle.dump(self.historyDict, datfile)
        datfile.close()

class DeadLinkReportThread(threading.Thread):
    '''
    A Thread that is responsible for posting error reports on talk pages. There
    will only be one DeadLinkReportThread, and it is using a semaphore to make
    sure that two LinkCheckerThreads can not access the queue at the same time.
    '''
    def __init__(self):
        threading.Thread.__init__(self)
        self.semaphore = threading.Semaphore()
        self.queue =  [];
        self.finishing = False
        self.killed = False

    def report(self, url, errorReport, containingPage, archiveURL):
        """ Tries to add an error report to the talk page belonging to the page
        containing the dead link.

        """
        self.semaphore.acquire()
        self.queue.append((url, errorReport, containingPage, archiveURL))
        self.semaphore.release()

    def shutdown(self):
        self.finishing = True

    def kill(self):
        # TODO: remove if unneeded
        self.killed = True

    def run(self):
        while not self.killed:
            if len(self.queue) == 0:
                if self.finishing:
                    break
                else:
                    time.sleep(0.1)
            else:
                self.semaphore.acquire()
                (url, errorReport, containingPage, archiveURL) = self.queue[0]
                self.queue = self.queue[1:]
                talkPage = containingPage.toggleTalkPage()
                pywikibot.output(
                    u'\03{lightaqua}** Reporting dead link on %s...\03{default}'
                    % talkPage.title(asLink=True))
                try:
                    content = talkPage.get() + "\n\n"
                    if url in content:
                        pywikibot.output(
                            u'\03{lightaqua}** Dead link seems to have already been reported on %s\03{default}'
                            % talkPage.title(asLink=True))
                        self.semaphore.release()
                        continue
                except (pywikibot.NoPage, pywikibot.IsRedirectPage):
                    content = u''

                if archiveURL:
                    archiveMsg = pywikibot.translate(pywikibot.getSite(),
                                                     talk_report_archive) % archiveURL
                else:
                    archiveMsg = u''
                # The caption will default to "Dead link". But if there is
                # already such a caption, we'll use "Dead link 2",
                # "Dead link 3", etc.
                caption = pywikibot.translate(pywikibot.getSite(),
                                              talk_report_caption)
                i = 1
                # Check if there is already such a caption on the talk page.
                while re.search('= *' + caption + ' *=', content) is not None:
                    i += 1
                    caption = pywikibot.translate(pywikibot.getSite(),
                                                  talk_report_caption) + " " + str(i)
                content += pywikibot.translate(pywikibot.getSite(),
                                               talk_report) % (caption,
                                                               errorReport,
                                                               archiveMsg)
                comment = u'[[%s#%s|→]]%s' % (talkPage.title(), caption,
                                              pywikibot.translate(pywikibot.getSite(),
                                                                  talk_report_msg))
                try:
                    talkPage.put(content, comment)
                except pywikibot.SpamfilterError, error:
                    pywikibot.output(
                        u'\03{lightaqua}** SpamfilterError while trying to change %s: %s\03{default}'
                        % (talkPage.title(asLink=True), error.url))

                self.semaphore.release()


class WeblinkCheckerRobot:
    '''
    Robot which will use several LinkCheckThreads at once to search for dead
    weblinks on pages provided by the given generator.
    '''
    def __init__(self, generator, HTTPignore = []):
        self.generator = generator
        if config.report_dead_links_on_talk:
            #pywikibot.output("Starting talk page thread")
            reportThread = DeadLinkReportThread()
            # thread dies when program terminates
            # reportThread.setDaemon(True)
            reportThread.start()
        else:
            reportThread = None
        self.history = History(reportThread)
        self.HTTPignore = HTTPignore

    def run(self):
        for page in self.generator:
           self.checkLinksIn(page)

    def checkLinksIn(self, page):
        try:
            text = page.get()
        except pywikibot.NoPage:
            pywikibot.output(u'%s does not exist.' % page.title())
            return
        for url in weblinksIn(text):
            ignoreUrl = False
            for ignoreR in ignorelist:
                if ignoreR.match(url):
                    ignoreUrl = True
            if not ignoreUrl:
                # Limit the number of threads started at the same time. Each
                # thread will check one page, then die.
                while threading.activeCount() >= config.max_external_links:
                    # wait 100 ms
                    time.sleep(0.1)
                thread = LinkCheckThread(page, url, self.history,
                                         self.HTTPignore)
                # thread dies when program terminates
                thread.setDaemon(True)
                thread.start()


def RepeatPageGenerator():
    history = History(None)
    pageTitles = set()
    for (key, value) in history.historyDict.iteritems():
        for entry in value:
            pageTitle = entry[0]
            pageTitles.add(pageTitle)
    pageTitles = list(pageTitles)
    pageTitles.sort()
    for pageTitle in pageTitles:
        page = pywikibot.Page(pywikibot.getSite(), pageTitle)
        yield page

def countLinkCheckThreads():
    i = 0
    for thread in threading.enumerate():
        if isinstance(thread, LinkCheckThread):
            i += 1
    return i

def main():
    gen = None
    singlePageTitle = []
    # Which namespaces should be processed?
    # default to [] which means all namespaces will be processed
    namespaces = []
    HTTPignore = []
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()
    global day
    day = 7
    for arg in pywikibot.handleArgs():
        if arg == '-talk':
            config.report_dead_links_on_talk = True
        elif arg == '-notalk':
            config.report_dead_links_on_talk = False
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[11:]))
            except ValueError:
                namespaces.append(arg[11:])
        elif arg == '-repeat':
            gen = RepeatPageGenerator()
        elif arg.startswith('-ignore:'):
            HTTPignore.append(int(arg[8:]))
        elif arg.startswith('-day:'):
            day = int(arg[5:])
        else:
            if not genFactory.handleArg(arg):
                singlePageTitle.append(arg)

    if singlePageTitle:
        singlePageTitle = ' '.join(singlePageTitle)
        page = pywikibot.Page(pywikibot.getSite(), singlePageTitle)
        gen = iter([page])

    if not gen:
        gen = genFactory.getCombinedGenerator()
    if gen:
        if namespaces != []:
            gen =  pagegenerators.NamespaceFilterPageGenerator(gen, namespaces)
        # fetch at least 240 pages simultaneously from the wiki, but more if
        # a high thread number is set.
        pageNumber = max(240, config.max_external_links * 2)
        gen = pagegenerators.PreloadingGenerator(gen, pageNumber = pageNumber)
        gen = pagegenerators.RedirectFilterPageGenerator(gen)
        bot = WeblinkCheckerRobot(gen, HTTPignore)
        try:
            bot.run()
        finally:
            waitTime = 0
            # Don't wait longer than 30 seconds for threads to finish.
            while countLinkCheckThreads() > 0 and waitTime < 30:
                try:
                    pywikibot.output(
                        u"Waiting for remaining %i threads to finish, please wait..." % countLinkCheckThreads())
                    # wait 1 second
                    time.sleep(1)
                    waitTime += 1
                except KeyboardInterrupt:
                    pywikibot.output(u'Interrupted.')
                    break
            if countLinkCheckThreads() > 0:
                pywikibot.output(u'Remaining %i threads will be killed.'
                                 % countLinkCheckThreads())
                # Threads will die automatically because they are daemonic.
            if bot.history.reportThread:
                bot.history.reportThread.shutdown()
                # wait until the report thread is shut down; the user can
                # interrupt it by pressing CTRL-C.
                try:
                    while bot.history.reportThread.isAlive():
                        time.sleep(0.1)
                except KeyboardInterrupt:
                    pywikibot.output(u'Report thread interrupted.')
                    bot.history.reportThread.kill()
            pywikibot.output(u'Saving history...')
            bot.history.save()
    else:
        pywikibot.showHelp()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
