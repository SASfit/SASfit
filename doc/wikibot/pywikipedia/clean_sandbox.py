# -*- coding: utf-8 -*-
"""
This bot cleans a sandbox by replacing the current contents with predefined
text.

This script understands the following command-line arguments:

    -hours:#       Use this parameter if to make the script repeat itself
                   after # hours. Hours can be defined as a decimal. 0.01
                   hours are 36 seconds; 0.1 are 6 minutes.

    -delay:#       Use this parameter for a wait time after the last edit
                   was made. If no parameter is given it takes it from
                   hours and limits it between 5 and 15 minutes.
                   The minimum delay time is 5 minutes.

    -userlist      Use this parameter to run the script in the user name-
                   space.
                   > ATTENTION: on most wiki THIS IS FORBIDEN FOR BOTS ! <
                   > (please talk with your admin first)                 <
                   Since it is considered bad style to edit user page with-
                   out permission, you have to pass a page containing a
                   list of user to process. Argument e.g. is given as
                   "-userlist:Benutzer:DrTrigonBot/Diene_Mir\!".
                   Please be also aware that the rules when to clean the
                   user sandbox differ from those for project sandbox.

"""
#
# (C) Leonardo Gregianin, 2006
# (C) Wikipedian, 2006-2007
# (C) Andre Engels, 2007
# (C) Siebrand Mazeland, 2007
# (C) xqt, 2009
# (C) Dr. Trigon, 2011
#
# DrTrigonBot: http://de.wikipedia.org/wiki/Benutzer:DrTrigonBot
# Clean User Sandbox Robot (clean_user_sandbox.py)
# https://fisheye.toolserver.org/browse/drtrigon/pywikipedia/clean_user_sandbox.py?hb=true
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: clean_sandbox.py 9236 2011-05-10 09:29:16Z xqt $'
#

import time
import wikipedia as pywikibot
from pywikibot import i18n

content = {
    'commons': u'{{Sandbox}}\n<!-- Please edit only below this line. -->',
    'als':u'{{subst:/Vorlage}}',
    'ar': u'{{من فضلك اترك هذا السطر ولا تعدله (عنوان ساحة التجربة)}}\n<!-- مرحبا! خذ راحتك في تجربة مهارتك في التنسيق والتحرير أسفل هذا السطر. هذه الصفحة لتجارب التعديل ، سيتم تفريغ هذه الصفحة كل 6 ساعات. -->',
    'bar':u'{{Bitte erst NACH dieser Zeile schreiben! (Begrüßungskasten)}}\r\n',
    'cs': u'{{subst:/uhrabat}}',
    'da': u'{{subst:Sandkasse tekst}}',
    'de': u'{{Bitte erst NACH dieser Zeile schreiben! (Begrüßungskasten)}}\r\n',
    'en': u'{{Please leave this line alone (sandbox heading)}}\n<!-- Hello! Feel free to try your formatting and editing skills below this line. As this page is for editing experiments, this page will automatically be cleaned every 12 hours. -->',
    'fa': u'{{subst:User:Amirobot/sandbox}}',
    'fi': u'{{subst:Hiekka}}',
    'he': u'{{ארגז חול}}\n<!-- נא לערוך מתחת לשורה זו בלבד, תודה. -->',
    'id': u'{{Bakpasir}}\n<!-- Uji coba dilakukan di baris di bawah ini -->',
    'it': u'{{sandbox}}<!-- Scrivi SOTTO questa riga senza cancellarla. Grazie. -->',
    'ja': u'{{subst:サンドボックス}}',
    'ko': u'{{연습장 안내문}}',
    'ksh':u'{{subst:/Schablon}}',
    'nds':u'{{subst:/Vörlaag}}',
    'nl': u'{{subst:Wikipedia:Zandbak/schoon zand}}',
    'nn': u'{{sandkasse}}\n<!-- Ver snill og IKKJE FJERN DENNE LINA OG LINA OVER ({{sandkasse}}) Nedanføre kan du derimot ha det artig og prøve deg fram! Lykke til! :-)  -->',
    'no': u'{{Sandkasse}}\n<!-- VENNLIGST EKSPERIMENTER NEDENFOR DENNE SKJULTE TEKSTLINJEN! SANDKASSEMALEN {{Sandkasse}} SKAL IKKE FJERNES! -->}}',
    'pl': u'{{Prosimy - NIE ZMIENIAJ, NIE KASUJ, NIE PRZENOŚ tej linijki - pisz niżej}}',
    'pt': u'<!--não apague esta linha-->{{página de testes}}<!--não apagar-->\r\n',
    'ru': u'{{/Пишите ниже}}\n<!-- Не удаляйте, пожалуйста, эту строку, тестируйте ниже -->',
    'sr': u'{{песак}}\n<!-- Молимо, испробавајте испод ове линије. Хвала. -->',
    'sv': u'{{subst:Sandlådan}}',
    'th': u'{{กระบะทราย}}\n<!-- กรุณาอย่าแก้ไขบรรทัดนี้ ขอบคุณครับ/ค่ะ -- Please leave this line as they are. Thank you! -->',
    'zh': u'{{subst:User:Sz-iwbot/sandbox}}\r\n',
    }

sandboxTitle = {
    'commons': u'Project:Sandbox',
    'als':u'Project:Sandchaschte',
    'ar': u'Project:ساحة التجربة',
    'bar':u'Project:Spielwiese',
    'cs': u'Project:Pískoviště',
    'da': u'Project:Sandkassen',
    'de': u'Project:Spielwiese',
    'en': u'Project:Sandbox',
    'fa': [u'Project:صفحه تمرین', u'Project:آشنایی'],
    'fi': u'Project:Hiekkalaatikko',
    'fr': u'Project:Bac à sable',
    'he': u'Project:ארגז חול',
    'id': u'Project:Bak pasir',
    'it': u'Project:Pagina delle prove',
    'ja': u'Project:サンドボックス',
    'ko': u'Project:연습장',
    'ksh':u'Project:Shpillplaz',
    'nds':u'Project:Speelwisch',
    'nl': u'Project:Zandbak',
    'no': u'Project:Sandkasse',
    'pl': u'Project:Brudnopis',
    'pt': u'Project:Página de testes',
    'ru': u'Project:Песочница',
    'sr': u'Project:Песак',
    'sv': u'Project:Sandlådan',
    'th': u'Project:ทดลองเขียน',
    'zh': u'Project:沙盒',
    }

class SandboxBot:
    def __init__(self, hours, no_repeat, delay, userlist):
        self.hours = hours
        self.no_repeat = no_repeat
        if delay == None:
            self.delay = min(15, max(5, int(self.hours *60)))
        else:
            self.delay = max(5, delay)
        self.site = pywikibot.getSite()
        if userlist == None:
            self.userlist = None
        else:
            self.userlist = [page.title().split(u'/')[0] for page in pywikibot.Page(self.site, userlist).linkedPages()]

    def run(self):

        def minutesDiff(time1, time2):
            if type(time1) in [long, int]:
                time1 = str(time1)
            if type(time2) in [long, int]:
                time2 = str(time2)
            t1 = (((int(time1[0:4]) * 12 + int(time1[4:6])) * 30 +
                   int(time1[6:8])) * 24 + int(time1[8:10])) * 60 + \
                   int(time1[10:12])
            t2 = (((int(time2[0:4]) * 12 + int(time2[4:6])) * 30 +
                   int(time2[6:8])) * 24 + int(time2[8:10])) * 60 + \
                   int(time2[10:12])
            return abs(t2-t1)

        mySite = self.site
        while True:
            wait = False
            now = time.strftime("%d %b %Y %H:%M:%S (UTC)", time.gmtime())
            localSandboxTitle = pywikibot.translate(mySite, sandboxTitle)
            IsUserSandbox = (self.userlist is not None)  # DrTrigonBot (Clean User Sandbox Robot)
            if IsUserSandbox:
                localSandboxTitle = u'%s/' + localSandboxTitle.split(u':')[-1]
                localSandboxTitle = [localSandboxTitle % user for user in self.userlist]
            if type(localSandboxTitle) is list:
                titles = localSandboxTitle
            else:
                titles = [localSandboxTitle,]
            for title in titles:
                sandboxPage = pywikibot.Page(mySite, title)
                pywikibot.output(u'Preparing to process sandbox page %s' % sandboxPage.title(asLink=True))
                try:
                    text = sandboxPage.get()
                    translatedContent = pywikibot.translate(mySite, content)
                    translatedMsg = i18n.twtranslate(mySite,
                                                     'clean_sandbox-cleaned')
                    subst = 'subst:' in translatedContent
                    pos = text.find(translatedContent.strip())
                    if text.strip() == translatedContent.strip():
                        pywikibot.output(u'The sandbox is still clean, no change necessary.')
                    elif subst and sandboxPage.userName() == mySite.loggedInAs():
                        pywikibot.output(u'The sandbox might be clean, no change necessary.')
                    elif pos <> 0 and not subst:
                        if IsUserSandbox:
                            endpos = pos + len(translatedContent.strip())
                            if (pos < 0) or (endpos == len(text)):
                                pywikibot.output(u'The user sandbox is still clean or not set up, no change necessary.')
                            else:
                                sandboxPage.put(text[:endpos], translatedMsg)
                                pywikibot.output(u'Standard content was changed, user sandbox cleaned.')
                        else:
                            sandboxPage.put(translatedContent, translatedMsg)
                            pywikibot.output(u'Standard content was changed, sandbox cleaned.')
                    else:
                        diff = minutesDiff(sandboxPage.editTime(), time.strftime("%Y%m%d%H%M%S", time.gmtime()))
                        if pywikibot.verbose:
                            print sandboxPage.editTime(), time.strftime("%Y%m%d%H%M%S", time.gmtime())
                        #Is the last edit more than 5 minutes ago?
                        if diff >= self.delay:
                            sandboxPage.put(translatedContent, translatedMsg)
                        else: #wait for the rest
                            pywikibot.output(u'Sleeping for %d minutes.' % (self.delay-diff))
                            time.sleep((self.delay-diff)*60)
                            wait = True
                except pywikibot.EditConflict:
                    pywikibot.output(u'*** Loading again because of edit conflict.\n')
                except pywikibot.NoPage:
                    pywikibot.output(u'*** The sandbox is not existent, skipping.')
                    continue
            if self.no_repeat:
                pywikibot.output(u'\nDone.')
                return
            elif not wait:
                if self.hours < 1.0:
                    pywikibot.output('\nSleeping %s minutes, now %s' % ((self.hours*60), now) )
                else:
                    pywikibot.output('\nSleeping %s hours, now %s' % (self.hours, now) )
                time.sleep(self.hours * 60 * 60)

def main():
    hours = 1
    delay = None
    userlist = None
    no_repeat = True
    for arg in pywikibot.handleArgs():
        if arg.startswith('-hours:'):
            hours = float(arg[7:])
            no_repeat = False
        elif arg.startswith('-delay:'):
            delay = int(arg[7:])
        elif arg.startswith('-userlist:'):
            userlist = arg[10:]
        else:
            pywikibot.showHelp('clean_sandbox')
            return

    bot = SandboxBot(hours, no_repeat, delay, userlist)
    try:
        bot.run()
    except KeyboardInterrupt:
        pywikibot.output('\nQuitting program...')

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
