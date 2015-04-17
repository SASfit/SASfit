#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Script to welcome new users. This script works out of the box for Wikis that
have been defined in the script. It is currently used on the Dutch, Norwegian,
Albanian, Italian Wikipedia, Wikimedia Commons and English Wikiquote.

Note: You can download the latest version available
from here: http://botwiki.sno.cc/wiki/Python:Welcome.py

Ensure you have community support before running this bot!

URLs to current implementations:
* Wikimedia Commons: http://commons.wikimedia.org/wiki/Commons:Welcome_log
* Dutch Wikipedia: http://nl.wikipedia.org/wiki/Wikipedia:Logboek_welkom
* Italian Wikipedia: http://it.wikipedia.org/wiki/Wikipedia:Benvenuto_log
* English Wikiquote: http://en.wikiquote.org/wiki/Wikiquote:Welcome_log
* Persian Wikipedia: http://fa.wikipedia.org/wiki/ویکی‌پدیا:سیاهه_خوشامد
* Korean Wikipedia: http://ko.wikipedia.org/wiki/위키백과:Welcome_log

Everything that needs customisation to support additional projects is
indicated by comments.

Description of basic functionality:
* Request a list of new users every period (default: 3600 seconds)
  You can choose to break the script after the first check (see arguments)
* Check if new user has passed a threshold for a number of edits
  (default: 1 edit)
* Optional: check username for bad words in the username or if the username
  consists solely of numbers; log this somewhere on the wiki (default: False)
  Update: Added a whitelist (explanation below).
* If user has made enough edits (it can be also 0), check if user has an empty
  talk page
* If user has an empty talk page, add a welcome message.
* Optional: Once the set number of users have been welcomed, add this to the
  configured log page, one for each day (default: True)
* If no log page exists, create a header for the log page first.

This script (by default not yet implemented) uses two templates that need to
be on the local wiki:
* {{WLE}}: contains mark up code for log entries (just copy it from Commons)
* {{welcome}}: contains the information for new users

This script understands the following command-line arguments:

   -edit[:#]       Define how many edits a new user needs to be welcomed
                   (default: 1, max: 50)

   -time[:#]       Define how many seconds the bot sleeps before restart
                   (default: 3600)

   -break          Use it if you don't want that the Bot restart at the end
                   (it will break) (default: False)

   -nlog           Use this parameter if you do not want the bot to log all
                   welcomed users (default: False)

   -limit[:#]      Use this parameter to define how may users should be
                   checked (default:50)

   -offset[:TIME]  Skip the latest new users (those newer than TIME)
                   to give interactive users a chance to welcome the
                   new users (default: now)
                   Timezone is the server timezone, GMT for Wikimedia
                   TIME format : yyyymmddhhmmss

   -timeoffset[:#] Skip the latest new users, accounts newer than
                   # minutes

   -numberlog[:#]  The number of users to welcome before refreshing the
                   welcome log (default: 4)

   -filter         Enable the username checks for bad names (default: False)

   -ask            Use this parameter if you want to confirm each possible
                   bad username (default: False)

   -random         Use a random signature, taking the signatures from a wiki
                   page (for instruction, see below).

   -file[:#]       Use a file instead of a wikipage to take the random sign.
                   If you use this parameter, you don't need to use -random.

   -sign           Use one signature from command line instead of the default

   -savedata       This feature saves the random signature index to allow to
                   continue to welcome with the last signature used.

   -sul            Welcome the auto-created users (default: False)

   -quiet          Prevents users without contributions are displayed

   -quick          Provide quick check by API bulk-retrieve user datas

********************************* GUIDE ***********************************

Report, Bad and white list guide:

1)  Set in the code which page it will use to load the badword, the
    whitelist and the report
2)  In these page you have to add a "tuple" with the names that you want to
    add in the two list. For example: ('cat', 'mouse', 'dog')
    You can write also other text in the page, it will work without problem.
3)  What will do the two pages? Well, the Bot will check if a badword is in
    the username and set the "warning" as True. Then the Bot check if a word
    of the whitelist is in the username. If yes it remove the word and
    recheck in the bad word list to see if there are other badword in the
    username.
    Example:
        * dio is a badword
        * Claudio is a normal name
        * The username is "Claudio90 fuck!"
        * The Bot find dio and set "warning"
        * The Bot find Claudio and set "ok"
        * The Bot find fuck at the end and set "warning"
        * Result: The username is reported.
4)  When a user is reported you have to check him and do:
        * If he's ok, put the {{welcome}}
        * If he's not, block him
        * You can decide to put a "you are blocked, change another username"
          template or not.
        * Delete the username from the page.
        IMPORTANT : The Bot check the user in this order:
            * Search if he has a talkpage (if yes, skip)
            * Search if he's blocked, if yes he will be skipped
            * Search if he's in the report page, if yes he will be skipped
            * If no, he will be reported.

Random signature guide:

Some welcomed users will answer to the one who has signed the welcome message.
When you welcome many new users, you might be overwhelmed with such answers.
Therefore you can define usernames of other users who are willing to receive
some of these messages from newbies.

1) Set the page that the bot will load
2) Add the signatures in this way:

*<SPACE>SIGNATURE
<NEW LINE>

Example:
<pre>
* [[User:Filnik|Filnik]]
* [[User:Rock|Rock]]
</pre>

NOTE: The white space and <pre></pre> aren't required but I suggest you to
      use them.

*************************** Known issues/FIXMEs ****************************
*                                                                          *
* The regex to load the user might be slightly different from project to   *
* project. (In this case, write to Filnik or the PWRF for help...)         *
* Use a class to group toghether the functions used.                       *
*                                                                          *
******************************** Badwords **********************************

The list of Badwords of the code is opened. If you think that a word is
international and it must be blocked in all the projects feel free to add it.
If also you think that a word isn't so international, feel free to delete it.

However, there is a dinamic-wikipage to load that badwords of your project or
you can add them directly in the source code that you are using without adding
or deleting.

Some words, like "Administrator" or "Dio" (God in italian) or "Jimbo" aren't
badword at all but can be used for some bad-nickname.
"""
#
# (C) Alfio, 2005
# (C) Kyle/Orgullomoore, 2006-2007
# (C) Siebrand Mazeland, 2006-2007
# (C) Filnik, 2007-2010
# (C) Daniel Herding, 2007
# (C) Alex Shih-Han Lin, 2009
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: welcome.py 9042 2011-03-13 10:14:47Z xqt $'
#

import time, re, urllib, locale
import codecs
from datetime import timedelta
from random import choice
from string import capitalize
import wikipedia as pywikibot
import config, query, userlib

locale.setlocale(locale.LC_ALL, '')

# Script users the class pywikibot.translate() to find the right
# page/user/summary/etc so the need to specify language and project have
# been eliminated.
# FIXME: Not all language/project combinations have been defined yet.
#       Add the following strings to customise for a language:
#       logbook, talk_page, summary, netext, summary2, user, con, report_page
#       comment, bad_pag, report_text, logt, random_sign and whitelist_pg.

############################################################################
############################################################################
############################################################################

# The text below are dictionaries. Copy the 'en' line, change 'en' in your
# language (e.g. 'de') and modify/translate the text.

# The page where the bot will save the log (e.g. Wikipedia:Welcome log).
#
# ATTENTION: "Log disabled comment is listed the projects not to log welcomed users, and no necessary to set deatils.
logbook = {
    'commons': {'_default': u'Project:Welcome log', },
    'wikipedia': {
        '_default': None,
        # Log disabled: da, de, en, fa, he, id, ka, pdc, pt, ru, vo.
        'ar': u'Project:سجل الترحيب',
        'fr': u'Wikipedia:Prise de décision/Accueil automatique des nouveaux par un robot/log',
        'ga': u'Project:Log fáilte',
        'it': u'Project:Benvenuto Bot/Log',
        'ja': u'利用者:Alexbot/Welcomebotログ',
        'nl': u'Project:Logboek welkom',
        'no': u'Project:Velkomstlogg',
        'sq': u'Project:Tung log',
        'sr': u'Project:Добродошлице',
        'zh': u'User:Welcomebot/欢迎日志',
    }
}
#The edit summary for the welcome message (e.g. Welcome!).
summary = {
    'commons': {'_default': u'Welcome!', },
    'ar': u'مرحبا!',
    'bn': u'Welcome!',
    'da': u'Velkommen',
    'de': u'Herzlich willkommen!',
    'en': u'Welcome!',
    'fa': u'خوش آمدید!',
    'fr': u'Bienvenue sur Wikipedia !',
    'ga': u'Fáilte!',
    'he': u'ברוך הבא!',
    'id': u'Selamat datang',
    'it': u'Benvenuto!',
    'ja': u'ウィキペディア日本語版へようこそ！',
    'ka': u'კეთილი იყოს თქვენი მობრძანება!',
    'nl': u'Welkom!',
    'no': u'Velkommen!',
    'pdc':u'Wilkum!',
    'pt': u'Bem vindo!',
    'ru': u'Добро пожаловать!',
    'sq': u'Tung',
    'sr': u'Добродошли!',
    'vo': u'Benokömö!',
    'zh': u'欢迎！',
    'zh-yue': u'歡迎',
}
# The text for the welcome message (e.g. {{welcome}}) and %s at the end
# that is your signature (the bot has a random parameter to add different
# sign, so in this way it will change according to your parameters).
netext = {
    'commons': {'_default': u'{{subst:welcome}} %s', } ,
    'wikipedia': {
        'ar': u'{{subst:ترحيب}} %s',
        'da': u'{{velkommen|%s}}',
        'de': u'{{subst:Hallo}} %s',
        'en': u'{{subst:welcome}} %s',
        'fa': u'{{جا:خوشامد}} %s',
        'fr': u'{{subst:Discussion Projet:Aide/Bienvenue}} %s',
        'ga': u'{{subst:fáilte}} %s',
        'he': u'{{ס:ברוך הבא}} %s',
        'id': u'{{sdbot|%s}}',
        'it': u'<!-- inizio template di benvenuto -->\n{{subst:Benvebot}} %s',
        'ja': u'{{subst:Welcome/intro}}\n{{subst:welcome|%s}} ',
        'ka': u'{{ახალი მომხმარებელი}}--%s',
        'nl': u'{{hola|bot|%s}}',
        'no': u'{{subst:bruker:jhs/vk}} %s',
        'pdc':u'{{subst:Wilkum}}%s',
        'pt': u'{{subst:bem vindo}} %s',
        'ru': u'{{Hello}} %s',
        'sq': u'{{subst:tung}} %s',
        'sr': u'{{Добродошлица}} %s',
        'vo': u'{{benokömö}} %s',
        'zh': u'{{subst:welcome|sign=%s}}',
        'zh-yue': u'{{歡迎}}--%s',
    },
    'wikinews':{
        'fa': u'{{خوشامد۲|%s}}',
        'it': u'{{subst:benvenuto}}',
        'zh': u'{{subst:welcome}} %s',
    },
    'wiktionary':{
        'bn': u'{{subst:User:Wikitanvir/welcome|firma=%s}}',
        'fa': u'{{جا:خوشامد|%s}}',
        'it': u'{{subst:Utente:Filnik/Benve|firma=%s}}',
    },
    'wikiversity':{
        'de': u'{{subst:Willkommen|%s}}',
        'el': u'{{subst:καλωσόρισμα}} %s',
        'en': u'{{subst:Welcome}}\n\n{{subst:Talktome}} %s',
        'es': u'{{subst:bienvenido usuario}} %s',
        'fr': u'{{Bienvenue}} %s',
        'it': u'{{subst:Benvenuto}} %s',
    },
}
# The edit summary for updating the welcome log (e.g. Updating log).
summary2 = {
    'commons': u'Updating log',
    'ar': u'تحديث السجل',
    'da': u'Updating log',
    'de': u'Aktualisiere Logdatei',
    'en': u'Updating log',
    'fa': u'به روز رسانی سیاهه',
    'fr': u'Mise a jour du journal',
    'ga': u'Log a thabhairt suas chun dáta',
    'it': u'Aggiorno il log',
    'ja': u'更新記録',
    'nl': u'Logboek bijwerken',
    'no': u'Oppdaterer logg',
    'pdc':u'Logfeil ennere',
    'ru': u'Обновление',
    'sq': u'Rifreskoj log',
    'sr': u'Освежавање записа',
    'zh': u'更新日志',
    'zh-yue':u'更新日誌',
}
# The page where the bot will report users with a possibly bad username.
report_page = {
    'commons': {'_default': u'Project:Administrators\' noticeboard/User problems/Usernames to be checked', },
    'wikipedia': {
        'ar': 'Project:إخطار الإداريين/أسماء مستخدمين للفحص',
        'da': u'Bruger:Broadbot/Report',
        'de': u'Benutzer:Filnik/Report',
        'en': u'Project:Administrator intervention against vandalism',
        'fa': u'Project:تابلوی اعلانات مدیران/گزارش ربات',
        'ga': u'Project:Log fáilte/Drochainmneacha',
        'it': u'Project:Benvenuto_Bot/Report',
        'ja': u'利用者:Alexbot/report',
        'nl': u'Project:Verzoekpagina voor moderatoren/RegBlok/Te controleren gebruikersnamen',
        'no': u'Bruker:JhsBot II/Rapport',
        'pdc':u'Benutzer:Xqt/Report',
        'ru': u'Участник:LatitudeBot/Рапорт',
        'sq': u'User:EagleBot/Report',
        'sr': u'User:SashatoBot/Записи',
        'zh': u'User:Welcomebot/report',
        'zh-yue': u'User:Alexbot/report',
    }
}
# The edit summary for reporting a possibly bad username.
comment = {
    'commons': {'_default': u'Adding a username that needs to be checked',},
    'wikipedia':{
        'ar': u'إضافة اسم مستخدم يحتاج للفحص',
        'da': u'Adding a username that needs to be checked',
        'de': u'Ergänze zu überprüfenden Benutzernamen',
        'en': u'Adding a username that needs to be checked',
        'fa': u'افزودن حساب کاربری نیازمند بررسی',
        'it': u'Aggiunto utente da controllare',
        'ja': u'不適切な利用者名の報告',
        'nl': u'Te controleren gebruikersnaam toegevoegd',
        'no': u'Legger til et brukernavn som m? sjekkes',
        'pdc':u'Yuusernaame zum iwwerpriefe',
        'ru': u'Добавлено подозрительное имя участника',
        'sq': u'Added username to be checked',
        'zh': u'回報不適當的用戶名稱',
    }
}
# The page where the bot reads the real-time bad words page
# (this parameter is optional).
bad_pag = {
    'commons': {'_default': u'Project:Welcome log/Bad_names', },
    'wikipedia':{
        'ar': u'Project:سجل الترحيب/أسماء سيئة',
        'en': u'Project:Welcome log/Bad_names',
        'fa': u'Project:سیاهه خوشامد/نام بد',
        'it': u'Project:Benvenuto_Bot/Lista_Badwords',
        'ja': u'Project:不適切な名前の利用者',
        'nl': u'Project:Logboek_welkom/Bad_names',
        'no': u'Bruker:JhsBot/Daarlige ord',
        'ru': u'Участник:LatitudeBot/Чёрный список',
        'sq': u'User:Eagleal/Bad_names',
        'sr': u'Додавање корисника за проверу',
        'zh': u'User:Welcomebot/badname',
        'zh-yue': u'User:Welcomebot/badname',
    }
}

timeselected = u' ~~~~~' # Defining the time used after the signature

# The text for reporting a possibly bad username (e.g. *[[Talk_page:Username|Username]]).
report_text = {
    'commons': {'_default': u"\n*{{user3|%s}}" + timeselected,},
    'wikipedia':{
        'ar': u"\n*{{user13|%s}}" + timeselected,
        'da': u'\n*[[Bruger Diskussion:%s]] ' + timeselected,
        'de': u'\n*[[Benutzer Diskussion:%s]] ' + timeselected,
        'en': u'\n*{{Userlinks|%s}} ' + timeselected,
        'fa': u'\n*{{کاربر|%s}}' + timeselected,
        'fr': u'\n*{{u|%s}} ' + timeselected,
        'ga': u'\n*[[Plé úsáideora:%s]] ' + timeselected,
        'it': u"\n{{Reported|%s|",
        'ja': u"\n*{{User2|%s}}" + timeselected,
        'nl': u'\n*{{linkgebruiker%s}} ' + timeselected,
        'no': u'\n*{{bruker|%s}} ' + timeselected,
        'pdc':u'\n*[[Benutzer Diskussion:%s]] ' + timeselected,
        'sq': u'\n*[[User:%s]] ' + timeselected,
        'zh': u"\n*{{User|%s}}" + timeselected
    }
}
# Set where you load your list of signatures that the bot will load if you use
# the random argument (this parameter is optional).
random_sign = {
    'ar': u'Project:سجل الترحيب/توقيعات',
    'da': u'Wikipedia:Velkommen/Signaturer',
    'en': u'Project:Welcome log/Sign',
    'fa': u'Project:سیاهه خوشامد/امضاها',
    'fr': u'Projet:Service de Parrainage Actif/Signatures',
    'it': u'Project:Benvenuto_Bot/Firme',
    'ja': None, #jawiki comminuty discussion oppose , [[ja:Wikipedia:Bot作業依頼/ウェルカムメッセージ貼り付け依頼]]
    'ru': u'Участник:LatitudeBot/Sign',
    'zh': u'User:Welcomebot/欢迎日志/用户',
    }
# The page where the bot reads the real-time whitelist page.
# (this parameter is optional).
whitelist_pg = {
    '_default': None,
    'ar':u'Project:سجل الترحيب/قائمة بيضاء',
    'en':u'User:Filnik/whitelist',
    'ga':u'Project:Log fáilte/Bánliosta',
    'it':u'Project:Benvenuto_Bot/Lista_Whitewords',
    'ru':u'Участник:LatitudeBot/Белый_список',
    }

# Text after the {{welcome}} template, if you want to add something
# Default (en): nothing.
final_new_text_additions = {
    'it':u'\n<!-- fine template di benvenuto -->',
    'zh':'<small>(via ~~~)</small>',
    }

#
#
logpage_header = {
    '_default' :u'{|border="2" cellpadding="4" cellspacing="0" style="margin: 0.5em 0.5em 0.5em 1em; padding: 0.5em; background: #bfcda5; border: 1px #b6fd2c solid; border-collapse: collapse; font-size: 95%;"',
    'no':u'[[Kategori:Velkomstlogg|{{PAGENAME}}]]\n{| class="wikitable"',
    'it':u'[[Categoria:Benvenuto log|{{subst:PAGENAME}}]]\n{|border="2" cellpadding="4" cellspacing="0" style="margin: 0.5em 0.5em 0.5em 1em; padding: 0.5em; background: #bfcda5; border: 1px #b6fd2c solid; border-collapse: collapse; font-size: 95%;"'
}

# Ok, that's all. What is below, is the rest of code, now the code is fixed
# and it will run correctly in your project ;)
############################################################################
############################################################################
############################################################################

class FilenameNotSet(pywikibot.Error):
    """An exception indicating that a signature filename was not specifed."""

class Global(object):
    """Container class for global settings.
       Use of globals outside of this is to be avoided."""

    attachEditCount = 1     # number of edits that an user required to be welcomed
    dumpToLog = 15          # number of users that are required to add the log :)
    offset = 0              # skip users newer than that timestamp
    timeoffset = 0          # skip users newer than # minutes
    recursive = True        # define if the Bot is recursive or not
    timeRecur = 3600        # how much time (sec.) the bot sleeps before restart
    makeWelcomeLog = True   # create the welcome log or not
    confirm = False         # should bot ask to add username to bad-username list
    welcomeAuto = False     # should bot welcome auto-created users
    filtBadName = False     # check if the username is ok or not
    randomSign = False      # should signature be random or not
    saveSignIndex = False   # should save the signature index or not
    signFileName = None     # File name, default: None
    defaultSign = '--~~~~'  # default signature
    queryLimit = 50         # number of users that the bot load to check
    quiet = False           # Prevents users without contributions are displayed
    quick = False           # Provide quick check by API bulk-retrieve user datas

    #fileOption = False     # check if the user wants to use a file or the wikipage

class WelcomeBot(object):

    def __init__(self):
        #Initial
        self.site = pywikibot.getSite()
        self.bname = dict()

        self._totallyCount = 0
        self.welcomed_users = list()

        if globalvar.randomSign:
            self.defineSign(True)
        if __name__ != '__main__': #use only in module call
            self._checkQueue = []

    def badNameFilter(self, name, force = False):
        if not globalvar.filtBadName:
            return False

        #initialize blacklist
        if not hasattr(self, '_blacklist') or force:
            elenco = [
                ' ano', ' anus', 'anal ', 'babies', 'baldracca', 'balle', 'bastardo',
                'bestiali', 'bestiale', 'bastarda', 'b.i.t.c.h.', 'bitch', 'boobie',
                'bordello', 'breast', 'cacata', 'cacca', 'cachapera', 'cagata',
                'cane', 'cazz', 'cazzo', 'cazzata', 'chiavare', 'chiavata', 'chick',
                'christ ', 'cristo', 'clitoride', 'coione', 'cojdioonear', 'cojones',
                'cojo', 'coglione', 'coglioni', 'cornuto', 'cula', 'culatone',
                'culattone', 'culo', 'deficiente', 'deficente', 'dio', 'die ',
                'died ', 'ditalino', 'ejackulate', 'enculer', 'eroticunt', 'fanculo',
                'fellatio', 'fica ', 'ficken', 'figa', 'sfiga', 'fottere', 'fotter',
                'fottuto', 'fuck', 'f.u.c.k.', "funkyass",
                'gay', 'hentai.com', 'horne', 'horney', 'virgin', 'hotties', 'idiot',
                '@alice.it', 'incest', 'jesus', 'gesu', 'gesù', 'kazzo', 'kill',
                'leccaculo', 'lesbian', 'lesbica', 'lesbo', 'masturbazione',
                'masturbare', 'masturbo', 'merda', 'merdata', 'merdoso', 'mignotta',
                'minchia', 'minkia', 'minchione', 'mona', 'nudo', 'nuda', 'nudi',
                'oral', 'sex', 'orgasmso', 'porc', 'pompa', 'pompino', 'porno',
                'puttana', 'puzza', 'puzzone', "racchia", 'sborone', 'sborrone',
                'sborata', 'sborolata', 'sboro', 'scopata', 'scopare', 'scroto',
                'scrotum', 'sega', 'sesso', 'shit', 'shiz', 's.h.i.t.', 'sadomaso',
                'sodomist', 'stronzata', 'stronzo', 'succhiamelo', 'succhiacazzi',
                'testicol', 'troia', 'universetoday.net', 'vaffanculo', 'vagina',
                'vibrator', "vacca", 'yiddiot', "zoccola",
            ]
            elenco_others = ['@', ".com", ".sex", ".org", ".uk", ".en", ".it", "admin",
                "administrator", "amministratore", '@yahoo.com', '@alice.com', "amministratrice",
                "burocrate", "checkuser", "developer", "http://", "jimbo", "mediawiki",
                "on wheals", "on wheal", "on wheel", "planante", "razinger", "sysop", "troll",
                "vandal", " v.f. ", "v. fighter", "vandal f.", "vandal fighter", 'wales jimmy',
                "wheels", "wales", "www.",
            ]

            #blacklist from wikipage
            badword_page = pywikibot.Page(self.site, pywikibot.translate(self.site, bad_pag) )
            list_loaded = list()
            if badword_page.exists():
                pywikibot.output(u'\nLoading the bad words list from %s...' % self.site )
                list_loaded = load_word_function(badword_page.get())
            else:
                showStatus(4)
                pywikibot.output(u'The bad word page doesn\'t exist!')
            self._blacklist = elenco + elenco_others + list_loaded
            del elenco, elenco_others, list_loaded

        if not hasattr(self, '_whitelist') or force:
            #initialize whitelist
            whitelist_default = ['emiliano']
            wtlpg = pywikibot.translate(self.site, whitelist_pg)
            list_white = list()
            if wtlpg:
                whitelist_page = pywikibot.Page(self.site, wtlpg)
                if whitelist_page.exists():
                    pywikibot.output(u'\nLoading the whitelist from %s...' % self.site )
                    list_white = load_word_function(whitelist_page.get())
                else:
                    showStatus(4)
                    pywikibot.output(u"The whitelist's page doesn't exist!")
            else:
                showStatus(4)
                pywikibot.output(u"WARNING: The whitelist hasn't been setted!")

            # Join the whitelist words.
            self._whitelist = list_white + whitelist_default
            del list_white, whitelist_default

        try:
            for wname in self._whitelist:
                if wname.lower() in str(name).lower():
                    name = name.lower().replace(wname.lower(), '')
                    for bname in self._blacklist:
                        self.bname[name] = bname
                        return bname.lower() in name.lower()
        except UnicodeEncodeError:
            pass
        try:
            for bname in self._blacklist:
                if bname.lower() in str(name).lower(): #bad name positive
                    self.bname[name] = bname
                    return True
        except UnicodeEncodeError:
            pass
        return False

    def reportBadAccount(self, name = None, final = False):
        #Queue process
        if name:
            if globalvar.confirm:
                answer = pywikibot.inputChoice(u'%s may have an unwanted username, do you want to report this user?'
                                % name, ['Yes', 'No', 'All'], ['y', 'N', 'a'], 'N')
                if answer in ['a', 'all']:
                    answer = 'y'
                    globalvar.confirm = False
            else:
                answer = 'y'

            if answer.lower() in ['yes', 'y'] or not globalvar.confirm:
                showStatus()
                pywikibot.output(u'%s is possibly an unwanted username. It will be reported.' % name)
                if hasattr(self, '_BAQueue'):
                    self._BAQueue.append(name)
                else:
                    self._BAQueue = [name]

        if len(self._BAQueue) >= globalvar.dumpToLog or final:
            rep_text = ''
            #name in queue is max, put detail to report page
            pywikibot.output("Updating badname accounts to report page...")
            rep_page = pywikibot.Page(self.site, pywikibot.translate(self.site, report_page) )
            if rep_page.exists():
                text_get = rep_page.get()
            else:
                text_get = u'This is a report page for the Bad-username, please translate me. --~~~'
            pos = 0
            # The talk page includes "_" between the two names, in this way i replace them to " ".
            for usrna in self._BAQueue:
                username = pywikibot.url2link(usrna, self.site, self.site)
                n = re.compile(re.escape(username), re.UNICODE)
                y = n.search(text_get, pos)
                if y:
                    pywikibot.output(u'%s is already in the report page.' % username)
                else:
                    # Adding the log.
                    rep_text += pywikibot.translate(self.site, report_text) % username
                    if self.site.lang == 'it':
                        rep_text = "%s%s}}" % (rep_text, self.bname[username])

            com = pywikibot.translate(self.site, comment)
            if rep_text != '':
                rep_page.put(text_get + rep_text, comment = com, minorEdit = True)
                showStatus(5)
                pywikibot.output(u'Reported')
            self.BAQueue = list()
        else:
            return True

    def makelogpage(self, queue = []):
        if not globalvar.makeWelcomeLog:
            return None
        if len(queue) == 0:
            return None

        text = u''
        logg = pywikibot.translate(self.site, logbook)
        if not logg:
            return None

        target = logg + '/' + time.strftime('%Y/%m/%d', time.localtime(time.time()))
        if self.site.lang == 'it':
            target = logg + '/' + time.strftime('%d/%m/%Y', time.localtime(time.time()))

        logPage = pywikibot.Page(self.site, target)
        if logPage.exists():
            text = logPage.get()
        else:
            #make new log page
            showStatus()
            pywikibot.output('Log page is not exist, getting information for page creation')
            text = pywikibot.translate(self.site, logpage_header)
            text += u'\n!%s' % self.site.namespace(2)
            text += u'\n!%s' % capitalize(self.site.mediawiki_message('contribslink'))

        for result in queue:
            # Adding the log... (don't take care of the variable's name...).
            luser = pywikibot.url2link(result.name(), self.site, self.site)
            text += u'\n{{WLE|user=%s|contribs=%d}}' % (luser, result.editCount())
        #update log page.
        while True:
            try:
                logPage.put(text, pywikibot.translate(self.site, summary2) )
                return True
            except pywikibot.EditConflict:
                pywikibot.output(u'An edit conflict has occured. Pausing for 10 seconds before continuing.')
                time.sleep(10)

    def parseNewUserLog(self):
        #if __name__ != '__main__':
        #    if self._checkQueue:
        #        for nm in self._checkQueue:
        #            yield userlib.User(self.site, nm)

        if not self.site.has_api() or self.site.versionnumber() < 13:
            for x in self._parseNewUserLogOld():
                yield x
            return

        starttime = None
        if globalvar.timeoffset != 0:
            now = self.site.server_time() - timedelta(minutes=globalvar.timeoffset)
            starttime = int(now.strftime("%Y%m%d%H%M%S"))
        elif globalvar.offset != 0:
            starttime = globalvar.offset

        count_auto = 0
        pywikibot.output("Querying new user log from API....")
        for x in self.site.logpages(number = globalvar.queryLimit, mode = 'newusers',start = starttime, dump = True):
            someone_found = True
            if 'user' not in x:
                continue
            #created twice?
            if not globalvar.welcomeAuto and x['action'] == 'create2':
                continue
            if not globalvar.welcomeAuto and x['action'] == 'autocreate':
                if not globalvar.quick:
                    showStatus(3)
                    pywikibot.output(u'%s has been created automatically.' % x['user'])
                count_auto += 1
                continue

            yield userlib.User(self.site, x['user'])

        if someone_found:
            if globalvar.quick and count_auto > 0:
                showStatus()
                pywikibot.output(u'Ignored %d user(s) by auto-create' % count_auto)

            showStatus(5)
            pywikibot.output(u'There is nobody left to be welcomed...')
        else:
            pywikibot.output(u'\nLoaded all users...')

    def _parseNewUserLogOld(self):
        someone_found = False
        URL = self.site.log_address(globalvar.queryLimit, 'newusers')
        if globalvar.timeoffset != 0:
            now = self.site.server_time() - timedelta(minutes=globalvar.timeoffset)
            globalvar.offset = int(now.strftime("%Y%m%d%H%M%S"))
        if globalvar.offset != 0:
            URL += "&offset=%d" % globalvar.offset
        pywikibot.output("Getting new user log from Special:Log/newusers....")
        raw = self.site.getUrl(URL)

        # I search with a regex how many user have not the talk page
        # and i put them in a list (i find it more easy and secure).
        # XXX: That's the regex, if there are problems, take a look here.
        reg =  u'\(<a href=\"' + re.escape(self.site.path())
        reg += u'\?title=%s(?P<user>.*?)&(?:amp;|)action=(?:edit|editredlink|edit&amp;redlink=1)\"' % re.escape('%s:' % urllib.quote(self.site.namespace(3).replace(" ", "_").encode(self.site.encoding())))
        reg += u'.*?</span> (?P<reason>.*?) *?</li>'

        p = re.compile(reg, re.UNICODE)

        for x in p.finditer(raw):
            someone_found = True
            userN = unicode(urllib.unquote(str(x.group('user'))), 'utf-8')
            #skip autocreated users (SUL)
            if not globalvar.welcomeAuto and self.site.mediawiki_message('newuserlog-autocreate-entry') in x.group('reason'):
                showStatus(3)
                pywikibot.output(u'%s has been created automatically, skipping...' % userN)
                continue

            #FIXME: It counts the first 50 edits
            # if number > 50, it won't work
            # (not *so* useful, it should be enough).
            yield userlib.User(self.site, userN)

        if someone_found:
            showStatus(5)
            pywikibot.output(u'There is nobody left to be welcomed...')
        else:
            pywikibot.output(u'\nLoaded all users...')

    def defineSign(self, force = False):
        if hasattr(self,'_randomSignature') and not force:
            return self._randomSignature

        signText = u''
        creg = re.compile(r"^\* ?(.*?)$", re.M)
        if not globalvar.signFileName:
            signPageName = pywikibot.translate(self.site, random_sign)
            if not signPageName:
                showStatus(4)
                pywikibot.output("%s doesn't allow random signature, force disable." % self.site)
                globalvar.randomSign = False
                return None

            signPage = pywikibot.Page(self.site, signPageName )
            if signPage.exists():
                pywikibot.output('Loading signature list...')
                signText = signPage.get()
            else:
                pywikibot.output('The Signature list page is not exist, random signature will disable.')
                globalvar.randomSign = False
        else:
            try:
                f = codecs.open(pywikibot.config.datafilepath(globalvar.signFileName), 'r',
                                encoding=config.console_encoding)
            except LookupError:
                f = codecs.open(pywikibot.config.datafilepath(globalvar.signFileName), 'r',
                                encoding='utf-8')
            except IOError:
                pywikibot.output(u'Error! - No fileName!')
                raise FilenameNotSet("No signature filename specified.")

            signText = f.read()
            f.close()
        self._randomSignature = creg.findall(signText)
        return self._randomSignature

    def run(self):
        while True:
            welcomed_count = 0
            if globalvar.quick and self.site.has_api():
                us = [x for x in self.parseNewUserLog()]
                showStatus()
                try:
                    userlib.getall(self.site, us)
                except NotImplementedError:
                    globalvar.quick = False
                    us = self._parseNewUserLogOld()
            else:
                us = self.parseNewUserLog()
            for users in us:
                if users.isBlocked():
                    showStatus(3)
                    pywikibot.output(u'%s has been blocked!' % users.name() )
                    continue
                if 'bot' in users.groups():
                    showStatus(3)
                    pywikibot.output(u'%s is a bot!' % users.name() )
                    continue
                if 'bot' in users.name().lower():
                    showStatus(3)
                    pywikibot.output(u'%s might be a global bot!' % users.name() )
                    continue
                #if globalvar.offset != 0 and time.strptime(users.registrationTime(), "%Y-%m-%dT%H:%M:%SZ") >= globalvar.offset:
                #
                if users.editCount() >= globalvar.attachEditCount:
                    showStatus(2)
                    pywikibot.output(u'%s has enough edits to be welcomed.' % users.name() )
                    ustp = users.getUserTalkPage()
                    if ustp.exists():
                        showStatus(3)
                        pywikibot.output(u'%s has been already welcomed.' % users.name())
                        continue
                    else:
                        if self.badNameFilter(users.name()):
                            self.reportBadAccount(users.name())
                            continue
                        welcome_text = pywikibot.translate(self.site, netext)
                        if globalvar.randomSign:
                            if self.site.family != 'wikinews':
                                welcome_text = welcome_text % choice(self.defineSign())
                            if self.site.family == 'wiktionary' and self.site.lang == 'it':
                                pass
                            else:
                                welcome_text += timeselected
                        elif (self.site.family != 'wikinews' and self.site.lang != 'it'):
                            welcome_text = welcome_text % globalvar.defaultSign
                        if self.site.lang in final_new_text_additions:
                            welcome_text += pywikibot.translate(self.site, final_new_text_additions)
                        welcome_comment = pywikibot.translate(self.site, summary)
                        try:
                            #append welcomed, welcome_count++
                            ustp.put(welcome_text, welcome_comment, minorEdit=False)
                            welcomed_count += 1
                            self._totallyCount += 1
                            self.welcomed_users.append(users)
                        except pywikibot.EditConflict:
                            showStatus(4)
                            pywikibot.output(u'An edit conflict has occured, skipping this user.')

                    if globalvar.makeWelcomeLog and pywikibot.translate(self.site, logbook):
                        showStatus(5)
                        if welcomed_count == 1:
                            pywikibot.output(u'One user has been welcomed.')
                        elif welcomed_count == 0:
                            pywikibot.output(u'No users have been welcomed.')
                        else:
                            pywikibot.output(u'%s users have been welcomed.' % welcomed_count )
                        if welcomed_count >= globalvar.dumpToLog:
                            if self.makelogpage(self.welcomed_users):
                                self.welcomed_users = list()
                                welcomed_count = 0
                            else:
                                continue
                    # If we haven't to report, do nothing.
                else:
                    if users.editCount() == 0:
                        if not globalvar.quiet:
                            showStatus(1)
                            pywikibot.output(u'%s has no contributions.' % users.name() )
                    else:
                        showStatus(1)
                        pywikibot.output(u'%s has only %d contributions.' % (users.name(), users.editCount() ) )
                    # That user mustn't be welcomed.
                    continue
            if globalvar.makeWelcomeLog and pywikibot.translate(self.site, logbook) and welcomed_count > 0:
                showStatus()
                if welcomed_count == 1:
                    pywikibot.output(u'Putting the log of the latest user...')
                else:
                    pywikibot.output(u'Putting the log of the latest %d users...' % welcomed_count)
                if self.makelogpage(self.welcomed_users):
                    self.welcomed_users = list()
                else:
                    continue
                self.welcomed_users = list()
            if hasattr(self, '_BAQueue'):
                showStatus()
                pywikibot.output("Putting bad name to report page....")
                self.reportBadAccount(None, final = True)
            try:
                if globalvar.recursive:
                    showStatus()
                    if locale.getlocale()[1]:
                        strfstr = unicode(time.strftime(u"%d %b %Y %H:%M:%S (UTC)", time.gmtime()), locale.getlocale()[1])
                    else:
                        strfstr = unicode(time.strftime(u"%d %b %Y %H:%M:%S (UTC)", time.gmtime()))
                    pywikibot.output(u'Sleeping %d seconds before rerun. %s' % (globalvar.timeRecur, strfstr))
                    time.sleep(globalvar.timeRecur)
                else:
                    raise KeyboardInterrupt
            except KeyboardInterrupt:
                #if globalvar.makeWelcomeLog and len(self.welcomed_users) > 0:
                #    pywikibot.output("Update log before qutting script.")
                #    self.makelogpage(self.welcomed_users)
                #if hasattr(self, '_BAQueue') and len(self._BAQueue) > 0 and globalvar.filtBadName:
                #    self.reportBadAccount(None, final = True)
                break
    #if __name__ != '__main__':
    #    globalvar.offset = int(time.strftime("%Y%m%d%H%M%S", time.gmtime()))
    #
    #    def putName(nm):
    #
    #        self._checkQueue.append(name)
    #        if len(self._checkQueue) >= globalvar.dumpToLog:
    #            self.run()
    #            self._checkQueue = []

def showStatus(n = 0):
    staColor = {
        0:'lightpurple',
        1:'lightaqua',
        2:'lightgreen',
        3:'lightyellow',
        4:'lightred',
        5:'lightblue'
    }
    staMsg = {
        0:'MSG  ',
        1:'NoAct',
        2:'Match',
        3:'Skip ',
        4:'Warning',
        5:'Done ',
    }
    pywikibot.output("\03{%s}[%s]\03{default} " % (staColor[n], staMsg[n]) , newline = False)

def load_word_function(raw):
    """ This is a function used to load the badword and the whitelist."""
    page = re.compile(r"(?:\"|\')(.*?)(?:\"|\')(?:, |\))", re.UNICODE)
    list_loaded = page.findall(raw)
    if len(list_loaded) == 0:
        pywikibot.output(u'There was no input on the real-time page.')
    return list_loaded

globalvar = Global()

if __name__ == "__main__":
    try:
        number_user = 0
        for arg in pywikibot.handleArgs():
            if arg.startswith('-edit'):
                if len(arg) == 5:
                    globalvar.attachEditCount = int(pywikibot.input(u'After how many edits would you like to welcome new users? (0 is allowed)'))
                else:
                    globalvar.attachEditCount = int(arg[6:])
            elif arg.startswith('-timeoffset'):
                if len(arg) == 11:
                    globalvar.timeoffset = int(pywikibot.input(u'Which time offset (in minutest) for new users would you like to use?'))
                else:
                    globalvar.timeoffset = int(arg[12:])
            elif arg.startswith('-time'):
                if len(arg) == 5:
                    globalvar.timeRecur = int(pywikibot.input(u'For how many seconds would you like to bot to sleep before checking again?'))
                else:
                    globalvar.timeRecur = int(arg[6:])
            elif arg.startswith('-offset'):
                if len(arg) == 7:
                    globalvar.offset = int(pywikibot.input(u'Which time offset for new users would you like to use? (yyyymmddhhmmss)'))
                else:
                    globalvar.offset = int(arg[8:])
                if len(str(globalvar.offset)) != 14:
                    # upon request, we might want to check for software version here
                    raise ValueError("Mediawiki has changed, -offset:# is not supported anymore, but -offset:TIMESTAMP is, assuming TIMESTAMP is yyyymmddhhmmss. -timeoffset is now also supported. Please read this script source header for documentation.")
            elif arg.startswith('-file:'):
                globalvar.randomSign = True
                if len(arg) == 6:
                    globalvar.signFileName = pywikibot.input(u'Where have you saved your signatures?')
                else:
                    globalvar.signFileName = arg[6:]
            elif arg.startswith('-sign:'):
                if len(arg) == 6:
                    globalvar.defaultSign = pywikibot.input(u'Which signature to use?')
                else:
                    globalvar.defaultSign = arg[6:]
                globalvar.defaultSign += timeselected
            elif arg == '-break':
                globalvar.recursive = False
            elif arg == '-nlog':
                globalvar.makeWelcomeLog = False
            elif arg == '-ask':
                globalvar.confirm = True
            elif arg == '-filter':
                globalvar.filtBadName = True
            #elif arg == '-savedata':
            #    globalvar.saveSignIndex = True
            elif arg == '-random':
                globalvar.randomSign = True
            elif arg == '-sul':
                globalvar.welcomeAuto = True
            elif arg.startswith('-limit'):
                if len(arg) == 6:
                    globalvar.queryLimit = int(pywikibot.input(u'How many of the latest new users would you like to load?'))
                else:
                    globalvar.queryLimit = int(arg[7:])
            elif arg.startswith('-numberlog'):
                if len(arg) == 10:
                    globalvar.dumpToLog = int(pywikibot.input(u'After how many welcomed users would you like to update the welcome log?'))
                else:
                    globalvar.dumpToLog = int(arg[11:])
            elif arg == '-quiet':
                globalvar.quiet = True
            elif arg == '-quick':
                globalvar.quick = True

        # Filename and pywikipedia path
        # file where is stored the random signature index
        filename = pywikibot.config.datafilepath('welcome-%s-%s.data' % (pywikibot.default_family, pywikibot.default_code))
        if globalvar.offset and globalvar.timeoffset:
            pywikibot.output('WARING: both -offset and -timeoffset were provided, ignoring -offset')
            globalvar.offset = 0
        bot = WelcomeBot()
        try:
            bot.run()
        except KeyboardInterrupt:
            if bot.welcomed_users:
                showStatus()
                pywikibot.output("Put welcomed users before quit...")
                bot.makelogpage(bot.welcomed_users)
            pywikibot.output("\nQuitting...")
    finally:
        # If there is the savedata, the script must save the number_user.
        if globalvar.randomSign and globalvar.saveSignIndex and bot.welcomed_users:
            import cPickle
            f = file(filename, 'w')
            cPickle.dump(bot.welcomed_users, f)
            f.close()
        pywikibot.stopme()
