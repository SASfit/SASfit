#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Script to check recently uploaded files. This script checks if a file
description is present and if there are other problems in the image's description.

This script will have to be configured for each language. Please submit
translations as addition to the pywikipediabot framework.

Everything that needs customisation is indicated by comments.

This script understands the following command-line arguments:

-limit              The number of images to check (default: 80)

-commons            The Bot will check if an image on Commons has the same name
                    and if true it report the image.

-duplicates[:#]     Checking if the image has duplicates (if arg, set how many
                    rollback wait before reporting the image in the report
                    instead of tag the image) default: 1 rollback.

-duplicatesreport   Report the duplicates in a log *AND* put the template in
                    the images.

-sendemail          Send an email after tagging.

-break              To break the bot after the first check (default: recursive)

-time[:#]           Time in seconds between repeat runs (default: 30)

-wait[:#]           Wait x second before check the images (default: 0)

-skip[:#]           The bot skip the first [:#] images (default: 0)

-start[:#]          Use allpages() as generator
                    (it starts already form File:[:#])

-cat[:#]            Use a category as generator

-regex[:#]          Use regex, must be used with -url or -page

-page[:#]           Define the name of the wikipage where are the images

-url[:#]            Define the url where are the images

-untagged[:#]       Use daniel's tool as generator:
                    http://toolserver.org/~daniel/WikiSense/UntaggedImages.php

-nologerror         If given, this option will disable the error that is risen
                    when the log is full.

---- Instructions for the real-time settings  ----
* For every new block you have to add:

<------- ------->

In this way the Bot can understand where the block starts in order to take the
right parameter.

* Name=     Set the name of the block
* Find=     Use it to define what search in the text of the image's description,
            while
  Findonly= search only if the exactly text that you give is in the image's
            description.
* Summary=  That's the summary that the bot will use when it will notify the
            problem.
* Head=     That's the incipit that the bot will use for the message.
* Text=     This is the template that the bot will use when it will report the
            image's problem.

---- Known issues/FIXMEs: ----
* Clean the code, some passages are pretty difficult to understand if you're not the coder.
* Add the "catch the language" function for commons.
* Fix and reorganise the new documentation
* Add a report for the image tagged.

"""

#
# (C) Kyle/Orgullomoore, 2006-2007 (newimage.py)
# (C) Siebrand Mazeland, 2007
# (C) Filnik, 2007-2008
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: checkimages.py 9042 2011-03-13 10:14:47Z xqt $'
#

import re, time, urllib, urllib2, os, locale, sys, datetime
import wikipedia as pywikibot
import config, pagegenerators, catlib, query, userlib

locale.setlocale(locale.LC_ALL, '')

###############################################################################
# <--------------------------- Change only below! --------------------------->#
###############################################################################

# NOTE: in the messages used by the Bot if you put __botnick__ in the text, it
# will automatically replaced with the bot's nickname.

# That's what you want that will be added. (i.e. the {{no source}} with the
# right day/month/year )
n_txt = {
    'commons':u'\n{{subst:nld}}',
    'ar'     :u'\n{{subst:لم}}',
    'de'     :u'{{Benutzer:ABF/D|~~~~}} {{Dateiüberprüfung/benachrichtigt (Kategorie)|{{subst:LOCALYEAR}}|{{subst:LOCALMONTH}}|{{subst:LOCALDAY}}}} {{Dateiüberprüfung/benachrichtigt (Text)|Lizenz|||||}} --This was added by ~~~~-- ',
    'en'     :u'\n{{subst:nld}}',
    'fa'     :u'{{جا:حق تکثیر تصویر نامعلوم}}',
    'fr'     :u'\n{{subst:lid}}',
    'ga'     :u'\n{{subst:Ceadúnas de dhíth}}',
    'hu'     :u'\n{{nincslicenc|~~~~~}}',
    'it'     :u'\n{{subst:unverdata}}',
    'ja'     :u'{{subst:Nld}}',
    'ko'     :u'\n{{subst:nld}}',
    'ta'     :u'\n{{subst:nld}}',
    'zh'     :u'{{subst:No license/auto}}',
}

# Text that the bot will try to see if there's already or not. If there's a
# {{ I'll use a regex to make a better check.
# This will work so:
# '{{nld' --> '\{\{(?:template:|)no[ _]license ?(?:\||\n|\}) ?' (case
# insensitive).
# If there's not a {{ it will work as usual (if x in Text)
txt_find =  {
    'commons':[u'{{no license', u'{{no license/en', u'{{nld', u'{{no permission', u'{{no permission since'],
    'ar':[u'{{لت', u'{{لا ترخيص'],
    'de':[u'{{DÜP', u'{{Dateiüberprüfung'],
    'en':[u'{{nld', u'{{no license'],
    'fa':[u'{{حق تکثیر تصویر نامعلوم۲'],
    'ga':[u'{{Ceadúnas de dhíth', u'{{Ceadúnas de dhíth'],
    'hu':[u'{{nincsforrás',u'{{nincslicenc'],
    'it':[u'{{unverdata', u'{{unverified'],
    'ja':[u'{{no source', u'{{unknown', u'{{non free', u'<!--削除についての議論が終了するまで',],
    'ta':[u'{{no source', u'{{nld', u'{{no license'],
    'ko':[u'{{출처 없음', u'{{라이선스 없음',u'{{Unknown',],
    'zh':[u'{{no source', u'{{unknown', u'{{No license',],
}

# Summary for when the will add the no source
comm = {
    'ar'     :u'بوت: التعليم على ملف مرفوع حديثا غير موسوم',
    'commons':u'Bot: Marking newly uploaded untagged file',
    'de'     :u'Bot: Markierung als Bild ohne Lizenz',
    'en'     :u'Bot: Marking newly uploaded untagged file',
    'fa'     :u'ربات: حق تکثیر تصویر تازه بارگذاری شده نامعلوم است.',
    'ga'     :u'Róbó: Ag márcáil comhad nua-uaslódáilte gan ceadúnas',
    'hu'     :u'Robot: Frissen feltöltött licencsablon nélküli fájl megjelölése',
    'it'     :u"Bot: Aggiungo unverified",
    'ja'     :u'ロボットによる:著作権情報なしの画像をタグ',
    'ko'     :u'로봇:라이선스 없음',
    'ta'     :u'தானியங்கி:காப்புரிமை வழங்கப்படா படிமத்தை சுட்டுதல்',
    'zh'     :u'機器人:標示新上傳且未包含必要資訊的檔案',
}

# When the Bot find that the usertalk is empty is not pretty to put only the
# no source without the welcome, isn't it?
empty = {
    'commons':u'{{subst:welcome}}\n~~~~\n',
    'ar'     :u'{{ترحيب}}\n~~~~\n',
    'de'     :u'{{subst:willkommen}} ~~~~',
    'en'     :u'{{welcome}}\n~~~~\n',
    'fa'     :u'{{جا:خوشامدید|%s}}',
    'fr'     :u'{{Bienvenue nouveau\n~~~~\n',
    'ga'     :u'{{subst:Fáilte}} - ~~~~\n',
    'hu'     :u'{{subst:Üdvözlet|~~~~}}\n',
    'it'     :u'<!-- inizio template di benvenuto -->\n{{subst:Benvebot}}\n~~~~\n<!-- fine template di benvenuto -->',
    'ja'     :u'{{subst:Welcome/intro}}\n{{subst:welcome|--~~~~}}\n',
    'ko'     :u'{{환영}}--~~~~\n',
    'ta'     :u'{{welcome}}\n~~~~\n',
    'zh'     :u'{{subst:welcome|sign=~~~~}}',
}

# Summary that the bot use when it notify the problem with the image's license
comm2 = {
    'ar'     :u'بوت: طلب معلومات المصدر.',
    'commons':u'Bot: Requesting source information.',
    'de'     :u'Bot:Notify User',
    'en'     :u'Robot: Requesting source information.',
    'fa'     :u'ربات: درخواست منبع تصویر',
    'ga'     :u'Róbó: Ag iarraidh eolais foinse.',
    'it'     :u"Bot: Notifico l'unverified",
    'hu'     :u'Robot: Forrásinformáció kérése',
    'ja'     :u'ロボットによる:著作権情報明記のお願い',
    'ko'     :u'로봇:라이선스 정보 요청',
    'ta'     :u'தானியங்கி:மூலம் வழங்கப்படா படிமத்தை சுட்டுதல்',
    'zh'     :u'機器人：告知用戶',
}

# if the file has an unknown extension it will be tagged with this template.
# In reality, there aren't unknown extension, they are only not allowed...
delete_immediately = {
    'commons':u"{{speedy|The file has .%s as extension. Is it ok? Please check.}}",
    'ar'     :u"{{شطب|الملف له .%s كامتداد.}}",
    'en'     :u"{{db-meta|The file has .%s as extension.}}",
    'fa'     :u"{{حذف سریع|تصویر %s اضافی است.}}",
    'ga'     :u"{{scrios|Tá iarmhír .%s ar an comhad seo.}}",
    'hu'     :u'{{azonnali|A fájlnak .%s a kiterjesztése}}',
    'it'     :u'{{cancella subito|motivo=Il file ha come estensione ".%s"}}',
    'ja'     :u'{{db|知らないファイルフォーマット %s}}',
    'ko'     :u'{{delete|잘못된 파일 형식 (.%s)}}',
    'ta'     :u'{{delete|இந்தக் கோப்பு .%s என்றக் கோப்பு நீட்சியைக் கொண்டுள்ளது.}}',
    'zh'     :u'{{delete|未知檔案格式%s}}',
}

# The header of the Unknown extension's message.
delete_immediately_head = {
    'commons':u"\n== Unknown extension! ==\n",
    'ar'     :u"\n== امتداد غير معروف! ==\n",
    'en'     :u"\n== Unknown extension! ==\n",
    'fa'     :u"\n==بارگذاری تصاویر موجود در انبار==\n",
    'ga'     :u"\n== Iarmhír neamhaithnid! ==\n",
    'fr'     :u'\n== Extension inconnue ==\n',
    'hu'     :u'\n== Ismeretlen kiterjesztésű fájl ==\n',
    'it'     :u'\n\n== File non specificato ==\n',
    'ko'     :u'\n== 잘못된 파일 형식 ==\n',
    'ta'     :u'\n== இனங்காணப்படாத கோப்பு நீட்சி! ==\n',
    'zh'     :u'\n==您上載的檔案格式可能有誤==\n',
}

# Text that will be add if the bot find a unknown extension.
delete_immediately_notification = {
    'ar'     :u'الملف [[:File:%s]] يبدو أن امتداده خاطيء, من فضلك تحقق. ~~~~',
    'commons':u'The [[:File:%s]] file seems to have a wrong extension, please check. ~~~~',
    'en'     :u'The [[:File:%s]] file seems to have a wrong extension, please check. ~~~~',
    'fa'     :u'به نظر می‌آید تصویر [[:تصویر:%s]] مسیر نادرستی داشته باشد لطفا بررسی کنید.~~~~',
    'ga'     :u'Tá iarmhír mícheart ar an comhad [[:File:%s]], scrúdaigh le d\'thoil. ~~~~',
    'fr'     :u'Le fichier [[:File:%s]] semble avoir une mauvaise extension, veuillez vérifier. ~~~~',
    'hu'     :u'A [[:Kép:%s]] fájlnak rossz a kiterjesztése, kérlek ellenőrízd. ~~~~',
    'it'     :u'{{subst:Progetto:Coordinamento/Immagini/Bot/Messaggi/Ext|%s|__botnick__}} --~~~~',
    'ko'     :u'[[:그림:%s]]의 파일 형식이 잘못되었습니다. 확인 바랍니다.--~~~~',
    'ta'     :u'[[:படிமம்:%s]] இனங்காணப்படாத கோப்பு நீட்சியை கொண்டுள்ளது தயவு செய்து ஒரு முறை சரி பார்க்கவும் ~~~~',
    'zh'    :u'您好，你上傳的[[:File:%s]]無法被識別，請檢查您的檔案，謝謝。--~~~~',
}

# Summary of the delete immediately.
# (e.g: Adding {{db-meta|The file has .%s as extension.}})
del_comm = {
    'ar'     :u'بوت: إضافة %s',
    'commons':u'Bot: Adding %s',
    'en'     :u'Bot: Adding %s',
    'fa'     :u'ربات: اضافه کردن %s',
    'ga'     :u'Róbó: Cuir %s leis',
    'fr'     :u'Robot : Ajouté %s',
    'hu'     :u'Robot:"%s" hozzáadása',
    'it'     :u'Bot: Aggiungo %s',
    'ja'     :u'ロボットによる: 追加 %s',
    'ko'     :u'로봇 : %s 추가',
    'ta'     :u'Bot: Adding %s',
    'zh'     :u'機器人: 正在新增 %s',
}

# This is the most important header, because it will be used a lot. That's the
# header that the bot will add if the image hasn't the license.
nothing_head = {
    'ar'     :u"\n== صورة بدون ترخيص ==\n",
    'commons':u"",# Nothing, the template has already the header inside.
    'de'     :u"\n== Bild ohne Lizenz ==\n",
    'en'     :u"\n== Image without license ==\n",
    'ga'     :u"\n== Comhad gan ceadúnas ==\n",
    'fr'     :u"\n== Fichier sans licence ==\n",
    'hu'     :u"\n== Licenc nélküli kép ==\n",
    'it'     :u"\n\n== File senza licenza ==\n",
    'ja'     :u'',
    'ko'     :u'',
    'fa'     :u'',
    'ta'     :u'',
    'zh'     :u'',
    }
# That's the text that the bot will add if it doesn't find the license.
# Note: every __botnick__ will be repleaced with your bot's nickname (feel free not to use if you don't need it)
nothing_notification = {
    'commons':u"\n{{subst:User:Filnik/untagged|File:%s}}\n\n''This message was '''added automatically by [[User:" + \
               "__botnick__|__botnick__]]''', if you need some help about it, please use the [[Commons:Help desk]]''. --~~~~",
    'ar'     :u"{{subst:مصدر الصورة|File:%s}} --~~~~",
    'de'     :u'\n{{subst:Benutzer:ABF/D2|%s}} ~~~~ ',
    'en'     :u"{{subst:image source|File:%s}} --~~~~",
    'fa'     :u"{{جا:اخطار نگاره|%s}}",
    'ga'     :u"{{subst:Foinse na híomhá|File:%s}} --~~~~",
    'hu'     :u"{{subst:adjforrást|Kép:%s}} \n Ezt az üzenetet ~~~ automatikusan helyezte el a vitalapodon, kérdéseddel fordulj a gazdájához, vagy a [[WP:KF|Kocsmafalhoz]]. --~~~~",
    'it'     :u"{{subst:Progetto:Coordinamento/Immagini/Bot/Messaggi/Senza licenza|%s|__botnick__}} --~~~~",
    'ja'     :u"\n{{subst:Image copyright|File:%s}}--~~~~",
    'ko'     :u'\n{{subst:User:Kwjbot IV/untagged|%s}} --~~~~',
    'ta'     :u'\n{{subst:Di-no license-notice|படிமம்:%s}} ~~~~ ',
    'zh'     :u'\n{{subst:Uploadvionotice|File:%s}} ~~~~ ',
}

# This is a list of what bots used this script in your project.
# NOTE: YOUR Botnick is automatically added. It's not required to add it twice.
bot_list = {
    'commons':[u'Siebot', u'CommonsDelinker', u'Filbot', u'John Bot', u'Sz-iwbot', u'ABFbot'],
    'de'     :[u'ABFbot'],
    'en'     :[u'OrphanBot'],
    'fa'     :[u'Amirobot'],
    'ga'     :[u'AllieBot'],
    'it'     :[u'Filbot', u'Nikbot', u'.snoopyBot.'],
    'ja'     :[u'Alexbot'],
    'ko'     :[u'Kwjbot IV'],
    'ta'     :[u'TrengarasuBOT'],
    'zh'     :[u'Alexbot'],
}

# The message that the bot will add the second time that find another license
# problem.
second_message_without_license = {
    '_default':None,
    'hu':u'\nSzia! Úgy tűnik a [[:Kép:%s]] képpel is hasonló a probléma, mint az előbbivel. Kérlek olvasd el a [[WP:KÉPLIC|feltölthető képek]]ről szóló oldalunk, és segítségért fordulj a [[WP:KF-JO|Jogi kocsmafalhoz]]. Köszönöm --~~~~',
    'it':u':{{subst:Progetto:Coordinamento/Immagini/Bot/Messaggi/Senza licenza2|%s|__botnick__}} --~~~~',
}

# You can add some settings to wikipedia. In this way, you can change them
# without touching the code. That's useful if you are running the bot on
# Toolserver.
page_with_settings = {
    '_default':None,
    'commons':u'User:Filbot/Settings',
    'it':u'Progetto:Coordinamento/Immagini/Bot/Settings#Settings',
    'zh':u"User:Alexbot/cisettings#Settings",
}

# The bot can report some images (like the images that have the same name of an
# image on commons) This is the page where the bot will store them.
report_page = {
    'commons':u'User:Filbot/Report',
    'de'     :u'Benutzer:ABFbot/Report',
    'en'     :u'User:Filnik/Report',
    'fa'     :u'کاربر:Amirobot/گزارش تصویر',
    'ga'     :u'User:AllieBot/ReportImages',
    'hu'     :u'User:Bdamokos/Report',
    'it'     :u'Progetto:Coordinamento/Immagini/Bot/Report',
    'ja'     :u'User:Alexbot/report',
    'ko'     :u'User:Kwjbot IV/Report',
    'ta'     :u'User:Trengarasu/commonsimages',
    'zh'     :u'User:Alexsh/checkimagereport',
}

# Adding the date after the signature.
timeselected = u' ~~~~~'

# The text added in the report
report_text = {
    'commons':u"\n*[[:File:%s]] " + timeselected,
    'ar':u"\n*[[:ملف:%s]] " + timeselected,
    'de':u"\n*[[:Bild:%s]] " + timeselected,
    'en':u"\n*[[:File:%s]] " + timeselected,
    'fa':u"n*[[:پرونده:%s]] "+ timeselected,
    'ga':u"\n*[[:File:%s]] " + timeselected,
    'hu':u"\n*[[:Kép:%s]] " + timeselected,
    'it':u"\n*[[:File:%s]] " + timeselected,
    'ja':u"\n*[[:File:%s]] " + timeselected,
    'ko':u"\n*[[:그림:%s]] " + timeselected,
    'ta':u"\n*[[:படிமம்:%s]] " + timeselected,
    'zh':u"\n*[[:File:%s]] " + timeselected,
}

# The summary of the report
comm10 = {
    'commons':u'Bot: Updating the log',
    'ar'     :u'بوت: تحديث السجل',
    'de'     :u'Bot: schreibe Log',
    'en'     :u'Bot: Updating the log',
    'fa'     :u'ربات: به‌روزرسانی سیاهه',
    'fr'     :u'Robot: Mise à jour du journal',
    'ga'     :u'Róbó: Log a thabhairt suas chun dáta',
    'hu'     :u'Robot: A napló frissítése',
    'it'     :u'Bot: Aggiorno il log',
    'ja'     :u'ロボットによる:更新',
    'ko'     :u'로봇:로그 업데이트',
    'ta'     :u'தானியங்கி:பட்டியலை இற்றைப்படுத்தல்',
    'zh'     :u'機器人:更新記錄',
}

# If a template isn't a license but it's included on a lot of images, that can
# be skipped to analyze the image without taking care of it. (the template must
# be in a list)
# Warning:   Don't add template like "en, de, it" because they are already in
#            (added in the code, below
# Warning 2: The bot will use regex, make the names compatible, please (don't
#            add "Template:" or {{because they are already put in the regex).
# Warning 3: the part that use this regex is case-insensitive (just to let you
#            know..)
HiddenTemplate = {
    'commons':[u'Template:Information'], # Put the other in the page on the project defined below
    'ar':[u'Template:معلومات'],
    'de':[u'Template:Information'],
    'en':[u'Template:Information'],
    'fa':[u'الگو:اطلاعات'],
    'fr':[u'Template:Information'],
    'ga':[u'Template:Information'],
    'hu':[u'Template:Információ', u'Template:Enwiki', u'Template:Azonnali'],
    'it':[u'Template:EDP', u'Template:Informazioni file', u'Template:Information', u'Template:Trademark', u'Template:Permissionotrs'], # Put the other in the page on the project defined below
    'ja':[u'Template:Information'],
    'ko':[u'Template:그림 정보'],
    'ta':[u'Template:Information'],
    'zh':[u'Template:Information'],
}

# A page where there's a list of template to skip.
PageWithHiddenTemplates = {
    '_default':None,
    'commons': u'User:Filbot/White_templates#White_templates',
    'it':u'Progetto:Coordinamento/Immagini/Bot/WhiteTemplates',
    'ko': u'User:Kwjbot_IV/whitetemplates/list',
}

# A page where there's a list of template to consider as licenses.
PageWithAllowedTemplates = {
    '_default':None,
    'commons': u'User:Filbot/Allowed templates',
    'it':u'Progetto:Coordinamento/Immagini/Bot/AllowedTemplates',
    'ko':u'User:Kwjbot_IV/AllowedTemplates',
}

# Template added when the bot finds only an hidden template and nothing else.
# Note: every __botnick__ will be repleaced with your bot's nickname (feel free not to use if you don't need it)
HiddenTemplateNotification = {
    '_default':None,
    'commons': u"""\n{{subst:User:Filnik/whitetemplate|File:%s}}\n\n''This message was added automatically by __botnick__, if you need some help about it please read the text above again and follow the links in it, if you still need help ask at the [[File:Human-help-browser.svg|18px|link=Commons:Help desk|?]] '''[[Commons:Help desk|→]] [[Commons:Help desk]]''' in any language you like to use.'' --__botnick__""",
    'it'     : u"{{subst:Progetto:Coordinamento/Immagini/Bot/Messaggi/Template_insufficiente|%s|__botnick__}} --~~~~",
    'ko'     : u"\n{{subst:User:Kwj2772/whitetemplates|%s}} --~~~~",
}

# In this part there are the parameters for the dupe images.

# Put here the template that you want to put in the image to warn that it's a dupe
# put __image__ if you want only one image, __images__ if you want the whole list
duplicatesText = {
    '_default':None,
    'commons': u'\n{{Dupe|__image__}}',
    'it'     : u'\n{{Progetto:Coordinamento/Immagini/Bot/Template duplicati|__images__}}',
}

# Head of the message given to the author
duplicate_user_talk_head = {
    '_default':None,
    'it'     : u'\n\n== File doppio ==\n',
}

# Message to put in the talk
duplicates_user_talk_text = {
    '_default':None,
    'commons': u'{{subst:User:Filnik/duplicates|File:%s|File:%s}}', # FIXME: it doesn't exist
    'it'     : u"{{subst:Progetto:Coordinamento/Immagini/Bot/Messaggi/Duplicati|%s|%s|__botnick__}} --~~~~",
}

# Comment used by the bot while it reports the problem in the uploader's talk
duplicates_comment_talk = {
    '_default':None,
    'commons': u'Bot: Dupe file found',
    'ar'     : u'بوت: ملف مكرر تم العثور عليه',
    'it'     : u"Bot: Notifico il file doppio trovato",
}

# Comment used by the bot while it reports the problem in the image
duplicates_comment_image = {
    '_default':None,
    'commons': u'Bot: Tagging dupe file',
    'ar'     : u'بوت: وسم ملف مكرر',
    'it'     : u'Bot: File doppio, da cancellare',
}

# Regex to detect the template put in the image's decription to find the dupe
duplicatesRegex = {
    '_default':None,
    'commons': r'\{\{(?:[Tt]emplate:|)(?:[Dd]up(?:licat|)e|[Bb]ad[ _][Nn]ame)[|}]',
    'it'     : r'\{\{(?:[Tt]emplate:|)[Pp]rogetto:[Cc]oordinamento/Immagini/Bot/Template duplicati[|}]',
}

# Category with the licenses and / or with subcategories with the other licenses.
category_with_licenses = {
    'commons': 'Category:License tags',
    'ar': 'تصنيف:قوالب حقوق الصور',
    'en': 'Category:Wikipedia image copyright templates',
    'fa': u'رده:برچسب‌های حق تکثیر نگاره',
    'ga': 'Catagóir:Clibeanna cóipchirt d\'íomhánna',
    'it': 'Categoria:Template Licenze copyright',
    'ja': 'Category:画像の著作権表示テンプレート',
    'ko': '분류:위키백과 그림 저작권 틀',
    'ta': 'Category:காப்புரிமை வார்ப்புருக்கள்',
    'zh': 'Category:版權申告模板',
}

## Put None if you don't use this option or simply add nothing if en
## is still None.
# Page where is stored the message to send as email to the users
emailPageWithText = {
    '_default':None,
    'de':'Benutzer:ABF/D3',
}

# Title of the email
emailSubject = {
    '_default':None,
    'de':'Problemen mit Deinem Bild auf der Deutschen Wikipedia',
}

# Seems that uploaderBots aren't interested to get messages regarding the
# files that they upload.. strange, uh?
# Format: [[user,regex], [user,regex]...] the regex is needed to match the user where to send the warning-msg
uploadBots = {
        '_default':None,
        'commons':[['File Upload Bot (Magnus Manske)',
                    r'\|[Ss]ource=Transferred from .*?; transferred to Commons by \[\[User:(.*?)\]\]']],
}

# Service images that don't have to be deleted and/or reported has a template inside them
# (you can let this param as None)
serviceTemplates = {
    '_default': None,
    'it': ['Template:Immagine di servizio'],
}

# Add your project (in alphabetical order) if you want that the bot start
project_inserted = [u'ar', u'commons', u'de', u'en', u'fa', u'ga', u'hu', u'it',
                    u'ja', u'ko', u'ta', u'zh']

# Ok, that's all. What is below, is the rest of code, now the code is fixed and it will run correctly in your project.
################################################################################
# <--------------------------- Change only above! ---------------------------> #
################################################################################

# Error Classes
class LogIsFull(pywikibot.Error):
    """An exception indicating that the log is full and the Bot cannot add
    other data to prevent Errors.

    """

class NothingFound(pywikibot.Error):
    """ An exception indicating that a regex has return [] instead of results.

    """

# Other common useful functions
def printWithTimeZone(message):
    """ Function to print the messages followed by the TimeZone encoded
    correctly.

    """
    if message[-1] != ' ':
        message = '%s ' % unicode(message)
    if locale.getlocale()[1]:
        time_zone = unicode(time.strftime(u"%d %b %Y %H:%M:%S (UTC)", time.gmtime()), locale.getlocale()[1])
    else:
        time_zone = unicode(time.strftime(u"%d %b %Y %H:%M:%S (UTC)", time.gmtime()))
    pywikibot.output(u"%s%s" % (message, time_zone))

class Global(object):
    # default environment settings
    # Command line configurable parameters
    repeat = True            # Restart after having check all the images?
    limit = 80               # How many images check?
    time_sleep = 30          # How many time sleep after the check?
    skip_number = 0          # How many images to skip before checking?
    waitTime = 0             # How many time sleep before the check?
    commonsActive = False    # Check if on commons there's an image with the same name?
    normal = False           # Check the new images or use another generator?
    urlUsed = False          # Use the url-related function instead of the new-pages generator
    regexGen = False         # Use the regex generator
    untagged = False         # Use the untagged generator
    duplicatesActive = False # Use the duplicate option
    duplicatesReport = False # Use the duplicate-report option
    sendemailActive = False  # Use the send-email
    logFullError = True      # Raise an error when the log is full


class main:
    def __init__(self, site, logFulNumber = 25000, sendemailActive = False,
                 duplicatesReport = False, logFullError = True):
        """ Constructor, define some global variable """
        self.site = site
        self.logFullError = logFullError
        self.logFulNumber = logFulNumber
        self.rep_page = pywikibot.translate(self.site, report_page)
        self.rep_text = pywikibot.translate(self.site, report_text)
        self.com = pywikibot.translate(self.site, comm10)
        hiddentemplatesRaw = pywikibot.translate(self.site, HiddenTemplate)
        self.hiddentemplates = [pywikibot.Page(self.site, tmp)
                                for tmp in hiddentemplatesRaw]
        self.pageHidden = pywikibot.translate(self.site,
                                              PageWithHiddenTemplates)
        self.pageAllowed = pywikibot.translate(self.site,
                                               PageWithAllowedTemplates)
        # Commento = Summary in italian
        self.commento = pywikibot.translate(self.site, comm)
        # Adding the bot's nickname at the notification text if needed.
        botolist = pywikibot.translate(self.site, bot_list)
        project = pywikibot.getSite().family.name
        self.project = project
        bot = config.usernames[project]
        try:
            botnick = bot[self.site.lang]
        except KeyError:
            raise pywikibot.NoUsername(
                u"You have to specify an username for your bot in this project in the user-config.py file.")

        self.botnick = botnick
        botolist.append(botnick)

        self.botolist = botolist

        self.sendemailActive = sendemailActive
        # Inizialize the skip list used below
        self.skip_list = list()

        self.duplicatesReport = duplicatesReport

        self.image_namespace = u"File:"
        # Load the licenses only once, so do it once
        self.list_licenses = self.load_licenses()

    def setParameters(self, imageName, timestamp, uploader):
        """ Function to set parameters, now only image but maybe it can be used
        for others in "future"

        """
        self.imageName = imageName
        # Defing the image's Page Object
        self.image = pywikibot.ImagePage(self.site, self.imageName)
        self.timestamp = timestamp
        self.uploader = uploader

    def report(self, newtext, image_to_report, notification=None, head=None,
               notification2 = None, unver=True, commTalk=None, commImage=None):
        """ Function to make the reports easier. """
        # Defining some useful variable for next...
        self.image_to_report = image_to_report
        self.newtext = newtext
        self.head = head
        self.notification = notification
        self.notification2 = notification2

        if self.notification:
            self.notification = re.sub(r'__botnick__', self.botnick,
                                       notification)

        if self.notification2:
            self.notification2 = re.sub(r'__botnick__', self.botnick,
                                        notification2)
        self.commTalk = commTalk

        if commImage:
            self.commImage = commImage
        else:
            self.commImage = self.commento

        # Ok, done, let's loop.
        while 1:
            if unver:
                try:
                    resPutMex = self.tag_image()
                except pywikibot.NoPage:
                    pywikibot.output(u"The page has been deleted! Skip!")
                    break
                except pywikibot.EditConflict:
                    pywikibot.output(u"Edit conflict! Skip!")
                    break
                else:
                    if not resPutMex:
                        break
            else:
                try:
                    resPutMex = self.tag_image(False)
                except pywikibot.NoPage:
                    pywikibot.output(u"The page has been deleted!")
                    break
                except pywikibot.EditConflict:
                    pywikibot.output(u"Edit conflict! Skip!")
                    break
                else:
                    if not resPutMex:
                        break
            if self.notification:
                try:
                    self.put_mex_in_talk()
                except pywikibot.EditConflict:
                    pywikibot.output(u"Edit Conflict! Retrying...")
                    try:
                        self.put_mex_in_talk()
                    except:
                        pywikibot.output(
                            u"Another error... skipping the user..")
                        break
                else:
                    break
            else:
                break

    def uploadBotChangeFunction(self, reportPageText, upBotArray):
        """Detect the user that has uploaded the file through the upload bot"""
        regex = upBotArray[1]
        results = re.findall(regex, reportPageText)

        if results:
            luser = results[0]
            return luser
        else:
            return upBotArray[0] # we can't find the user, report the problem to the bot

    def tag_image(self, put = True):
        """ Function to add the template in the image and to find out
        who's the user that has uploaded the file.

        """
        # Get the image's description
        reportPageObject = pywikibot.ImagePage(self.site, self.image_to_report)

        try:
            reportPageText = reportPageObject.get()
        except pywikibot.NoPage:
            pywikibot.output(u'%s has been deleted...' % self.imageName)
            return False
        # You can use this function also to find only the user that
        # has upload the image (FixME: Rewrite a bit this part)
        if put:
            reportPageObject.put(reportPageText + self.newtext,
                                 comment=self.commImage)
        # paginetta it's the image page object.
        try:
            if reportPageObject == self.image and self.uploader:
                nick = self.uploader
            else:
                nick = reportPageObject.getLatestUploader()[0]
        except pywikibot.NoPage:
            pywikibot.output(
                u"Seems that %s has only the description and not the file..."
                % self.image_to_report)
            repme = u"\n*[[:File:%s]] problems '''with the APIs'''"
            # We have a problem! Report and exit!
            self.report_image(self.image_to_report, self.rep_page, self.com,
                              repme)
            return False
        upBots = pywikibot.translate(self.site, uploadBots)
        luser = pywikibot.url2link(nick, self.site, self.site)

        if upBots:
            for upBot in upBots:
                if upBot[0] == luser:
                    luser = self.uploadBotChangeFunction(reportPageText, upBot)
        talk_page = pywikibot.Page(self.site, u"%s:%s" % (self.site.namespace(3), luser))
        self.talk_page = talk_page
        self.luser = luser
        return True

    def put_mex_in_talk(self):
        """ Function to put the warning in talk page of the uploader."""
        commento2 = pywikibot.translate(self.site, comm2)
        emailPageName = pywikibot.translate(self.site, emailPageWithText)
        emailSubj = pywikibot.translate(self.site, emailSubject)
        if self.notification2:
            self.notification2 = self.notification2 % self.image_to_report
        else:
            self.notification2 = self.notification
        second_text = False
        # Getting the talk page's history, to check if there is another
        # advise...
        # The try block is used to prevent error if you use an old
        # wikipedia.py's version.
        try:
            testoattuale = self.talk_page.get()
            history = self.talk_page.getLatestEditors(limit = 10)
            latest_user = history[0]["user"]
            pywikibot.output(
                u'The latest user that has written something is: %s'
                % latest_user)
            for i in self.botolist:
                if latest_user == i:
                    second_text = True
                    # A block to prevent the second message if the bot also
                    # welcomed users...
                    if history[0]['timestamp'] == history[-1]['timestamp']:
                        second_text = False
        except pywikibot.IsRedirectPage:
            pywikibot.output(
                u'The user talk is a redirect, trying to get the right talk...')
            try:
                self.talk_page = self.talk_page.getRedirectTarget()
                testoattuale = self.talk_page.get()
            except pywikibot.NoPage:
                second_text = False
                testoattuale  = pywikibot.translate(self.site, empty)
        except pywikibot.NoPage:
            pywikibot.output(u'The user page is blank')
            second_text = False
            testoattuale = pywikibot.translate(self.site, empty)
        if self.commTalk:
            commentox = self.commTalk
        else:
            commentox = commento2

        if second_text:
            newText = u"%s\n\n%s" % (testoattuale, self.notification2)
        else:
            newText = testoattuale + self.head + self.notification

        try:
            self.talk_page.put(newText, comment = commentox, minorEdit = False)
        except pywikibot.LockedPage:
            pywikibot.output(u'Talk page blocked, skip.')

        if emailPageName and emailSubj:
            emailPage = pywikibot.Page(self.site, emailPageName)
            try:
                emailText = emailPage.get()
            except (pywikibot.NoPage, pywikibot.IsRedirectPage):
                return # Exit
            if self.sendemailActive:
                text_to_send = re.sub(r'__user-nickname__', r'%s'
                                      % self.luser, emailText)
                emailClass = userlib.User(self.site, self.luser)
                try:
                    emailClass.sendMail(emailSubj, text_to_send)
                except userlib.UserActionRefuse:
                    pywikibot.output("User is not mailable, aborted")
                    return # exit

    def untaggedGenerator(self, untaggedProject, limit):
        """ Generator that yield the files without license. It's based on a
        tool of the toolserver.

        """
        lang = untaggedProject.split('.', 1)[0]
        project = '.%s' % untaggedProject.split('.', 1)[1]

        if lang == 'commons':
            link = 'http://toolserver.org/~daniel/WikiSense/UntaggedImages.php?wikifam=commons.wikimedia.org&since=-100d&until=&img_user_text=&order=img_timestamp&max=100&order=img_timestamp&format=html'
        else:
            link = 'http://toolserver.org/~daniel/WikiSense/UntaggedImages.php?wikilang=%s&wikifam=%s&order=img_timestamp&max=%s&ofs=0&max=%s' % (lang, project, limit, limit)
        text = self.site.getUrl(link, no_hostname = True)
        results = re.findall(r"""<td valign='top' title='Name'><a href='http://.*?\.org/w/index\.php\?title=(.*?)'>.*?</a></td>""", text)

        if results:
            for result in results:
                wikiPage = pywikibot.ImagePage(self.site, result)
                yield wikiPage
        else:
            pywikibot.output(link)
            raise NothingFound(
                u'Nothing found! Try to use the tool by yourself to be sure that it works!')

    def regexGenerator(self, regexp, textrun):
        """ Generator used when an user use a regex parsing a page to yield the
        results

        """
        regex = re.compile(r'%s' % regexp, re.UNICODE|re.DOTALL)
        results = regex.findall(textrun)
        for image in results:
            yield pywikibot.ImagePage(self.site, image)

    def loadHiddenTemplates(self):
        """ Function to load the white templates """
        # A template as {{en is not a license! Adding also them in the whitelist template...
        for langK in pywikibot.Family(u'wikipedia').langs.keys():
            self.hiddentemplates.append(pywikibot.Page(self.site,
                                                       u'Template:%s' % langK))

        # The template #if: and #switch: aren't something to care about
        #self.hiddentemplates.extend([u'#if:', u'#switch:']) FIXME

        # Hidden template loading
        if self.pageHidden:
            try:
                pageHiddenText = pywikibot.Page(self.site,
                                                self.pageHidden).get()
            except (pywikibot.NoPage, pywikibot.IsRedirectPage):
                pageHiddenText = ''

            for element in self.load(pageHiddenText):
                self.hiddentemplates.append(pywikibot.Page(self.site, element))
        return self.hiddentemplates

    def returnOlderTime(self, listGiven, timeListGiven):
        """ Get some time and return the oldest of them """
        # print listGiven; print timeListGiven
        # -- Output: --
        # [[1210596312.0, u'Autoritratto.png'], [1210590240.0, u'Duplicato.png'], [1210592052.0, u'Duplicato_2.png']]
        # [1210596312.0, 1210590240.0, 1210592052.0]
        usage = False
        num = 0
        num_older = None
        max_usage = 0
        for element in listGiven:
            imageName = element[1]
            imagePage = pywikibot.ImagePage(self.site, imageName)
            imageUsage = [page for page in imagePage.usingPages()]
            if len(imageUsage) > 0 and len(imageUsage) > max_usage:
                max_usage = len(imageUsage)
                num_older = num
            num += 1

        if num_older:
            return listGiven[num_older][1]

        for element in listGiven:
            time = element[0]
            imageName = element[1]
            not_the_oldest = False

            for time_selected in timeListGiven:
                if time > time_selected:
                    not_the_oldest = True
                    break

            if not not_the_oldest:
                return imageName

    def convert_to_url(self, page):
        # Function stolen from wikipedia.py
        """The name of the page this Page refers to, in a form suitable for the
        URL of the page.

        """
        title = page.replace(u" ", u"_")
        encodedTitle = title.encode(self.site.encoding())
        return urllib.quote(encodedTitle)

    def countEdits(self, pagename, userlist):
        """Function to count the edit of a user or a list of users in a page."""
        # self.botolist
        if type(userlist) == str:
            userlist = [userlist]
        page = pywikibot.Page(self.site, pagename)
        history = page.getVersionHistory()
        user_list = list()

        for data in history:
            user_list.append(data[2])
        number_edits = 0

        for username in userlist:
            number_edits += user_list.count(username)
        return number_edits

    def checkImageOnCommons(self):
        """ Checking if the file is on commons """
        pywikibot.output(u'Checking if %s is on commons...' % self.imageName)
        commons_site = pywikibot.getSite('commons', 'commons')
        regexOnCommons = r"\[\[:File:%s\]\] is also on '''Commons''': \[\[commons:File:.*?\]\](?: \(same name\)|)$" % re.escape(self.imageName)
        hash_found = self.image.getHash()
        if not hash_found:
            return False # Image deleted, no hash found. Skip the image.
        else:
            commons_image_with_this_hash = commons_site.getFilesFromAnHash(hash_found)
            if commons_image_with_this_hash != [] and commons_image_with_this_hash != 'None':
                servTMP = pywikibot.translate(self.site, serviceTemplates)
                templatesInTheImage = self.image.getTemplates()
                if servTMP != None:
                    for template in servTMP:
                        if pywikibot.Page(self.site, template) in templatesInTheImage:
                            pywikibot.output(u"%s is on commons but it's a service image." % self.imageName)
                            return True # Problems? No, return True and continue with the check-part
                pywikibot.output(u'%s is on commons!' % self.imageName)
                on_commons_text = self.image.getImagePageHtml()
                if u"<div class='sharedUploadNotice'>" in on_commons_text:
                    pywikibot.output(u"But, the file doesn't exist on your project! Skip...")
                    # Problems? Yes! We have to skip the check part for that image
                    # Because it's on commons but someone has added something on your project.
                    return False
                elif re.findall(r'\bstemma\b', self.imageName.lower()) != [] and self.site.lang == 'it':
                    pywikibot.output(u'%s has "stemma" inside, means that it\'s ok.' % self.imageName)
                    return True # Problems? No, it's only not on commons but the image needs a check
                else:
                    # the second usually is a url or something like that. Compare the two in equal way, both url.
                    if self.convert_to_url(self.imageName) == self.convert_to_url(commons_image_with_this_hash[0]):
                        repme = u"\n*[[:File:%s]] is also on '''Commons''': [[commons:File:%s]] (same name)" % (self.imageName, commons_image_with_this_hash[0])
                    else:
                        repme = u"\n*[[:File:%s]] is also on '''Commons''': [[commons:File:%s]]" % (self.imageName, commons_image_with_this_hash[0])
                    self.report_image(self.imageName, self.rep_page, self.com, repme, addings = False, regex = regexOnCommons)
                    return True # Problems? No, return True
            else:
                return True # Problems? No, return True

    def checkImageDuplicated(self, duplicates_rollback):
        """ Function to check the duplicated files. """
        # {{Dupe|File:Blanche_Montel.jpg}}
        # Skip the stub images
        #if 'stub' in self.imageName.lower() and self.project == 'wikipedia' and self.site.lang == 'it':
        #    return True # Skip the stub, ok
        dupText = pywikibot.translate(self.site, duplicatesText)
        dupRegex = pywikibot.translate(self.site, duplicatesRegex)
        dupTalkHead = pywikibot.translate(self.site, duplicate_user_talk_head)
        dupTalkText = pywikibot.translate(self.site, duplicates_user_talk_text)
        dupComment_talk = pywikibot.translate(self.site, duplicates_comment_talk)
        dupComment_image = pywikibot.translate(self.site, duplicates_comment_image)
        duplicateRegex = r'\[\[:File:%s\]\] has the following duplicates' % re.escape(self.convert_to_url(self.imageName))
        imagePage = pywikibot.ImagePage(self.site, self.imageName)
        hash_found = imagePage.getHash()
        duplicates = self.site.getFilesFromAnHash(hash_found)

        if not duplicates:
            return False # Error, image deleted, no hash found. Skip the image.

        if len(duplicates) > 1:
            if len(duplicates) == 2:
                pywikibot.output(u'%s has a duplicate! Reporting it...' % self.imageName)
            else:
                pywikibot.output(u'%s has %s duplicates! Reporting them...' % (self.imageName, len(duplicates) - 1))

            if dupText and dupRegex:
                time_image_list = list()
                time_list = list()

                for duplicate in duplicates:
                    DupePage = pywikibot.ImagePage(self.site, duplicate)

                    if DupePage.urlname() != self.image.urlname() or self.timestamp == None:
                        self.timestamp = DupePage.getLatestUploader()[1]
                    data = time.strptime(self.timestamp, u"%Y-%m-%dT%H:%M:%SZ")
                    data_seconds = time.mktime(data)
                    time_image_list.append([data_seconds, duplicate])
                    time_list.append(data_seconds)
                older_image = self.returnOlderTime(time_image_list, time_list)
                # And if the images are more than two?
                Page_oder_image = pywikibot.ImagePage(self.site, older_image)
                string = ''
                images_to_tag_list = []

                for duplicate in duplicates:
                    if pywikibot.ImagePage(self.site, duplicate) == pywikibot.ImagePage(self.site, older_image):
                        continue # the older image, not report also this as duplicate
                    DupePage = pywikibot.ImagePage(self.site, duplicate)
                    try:
                        DupPageText = DupePage.get()
                        older_page_text = Page_oder_image.get()
                    except pywikibot.NoPage:
                        continue # The page doesn't exists

                    if not re.findall(dupRegex, DupPageText) and not re.findall(dupRegex, older_page_text):
                        pywikibot.output(u'%s is a duplicate and has to be tagged...' % duplicate)
                        images_to_tag_list.append(duplicate)
                        #if duplicate != duplicates[-1]:
                        string += u"*[[:%s%s]]\n" % (self.image_namespace, duplicate)
                        #else:
                        #    string += "*[[:%s%s]]" % (self.image_namespace, duplicate)
                    else:
                        pywikibot.output(u"Already put the dupe-template in the files's page or in the dupe's page. Skip.")
                        return False # Ok - No problem. Let's continue the checking phase
                older_image_ns = u'%s%s' % (self.image_namespace, older_image) # adding the namespace
                only_report = False # true if the image are not to be tagged as dupes

                # put only one image or the whole list according to the request
                if u'__images__' in dupText:
                    text_for_the_report = re.sub(r'__images__', r'\n%s*[[:%s]]\n' % (string, older_image_ns), dupText)
                else:
                    text_for_the_report = re.sub(r'__image__', r'%s' % older_image_ns, dupText)

                # Two iteration: report the "problem" to the user only once (the last)
                if len(images_to_tag_list) > 1:
                    for image_to_tag in images_to_tag_list[:-1]:
                        already_reported_in_past = self.countEdits(u'File:%s' % image_to_tag, self.botolist)
                        # if you want only one edit, the edit found should be more than 0 -> num - 1
                        if already_reported_in_past > duplicates_rollback - 1:
                            only_report = True
                            break
                        # Delete the image in the list where we're write on
                        text_for_the_report = re.sub(r'\n\*\[\[:%s\]\]' % re.escape(self.image_namespace + image_to_tag), '', text_for_the_report)
                        self.report(text_for_the_report, image_to_tag,
                                    commImage = dupComment_image, unver = True)

                if len(images_to_tag_list) != 0 and not only_report:
                    already_reported_in_past = self.countEdits(u'File:%s' % images_to_tag_list[-1], self.botolist)
                    image_to_resub = images_to_tag_list[-1]
                    from_regex = r'\n\*\[\[:File:%s\]\]' % re.escape(self.convert_to_url(self.imageName))
                    # Delete the image in the list where we're write on
                    text_for_the_report = re.sub(from_regex, '', text_for_the_report)
                    # if you want only one edit, the edit found should be more than 0 -> num - 1
                    if already_reported_in_past > duplicates_rollback - 1:
                        only_report = True
                    else:
                        self.report(text_for_the_report, images_to_tag_list[-1],
                            dupTalkText % (older_image_ns, string), dupTalkHead, commTalk = dupComment_talk,
                                commImage = dupComment_image, unver = True)

            if self.duplicatesReport or only_report:
                if only_report:
                    repme = u"\n*[[:File:%s]] has the following duplicates ('''forced mode'''):" % self.convert_to_url(self.imageName)
                else:
                    repme = u"\n*[[:File:%s]] has the following duplicates:" % self.convert_to_url(self.imageName)

                for duplicate in duplicates:
                    if self.convert_to_url(duplicate) == self.convert_to_url(self.imageName):
                        continue # the image itself, not report also this as duplicate
                    repme += u"\n**[[:File:%s]]" % self.convert_to_url(duplicate)
                result = self.report_image(self.imageName, self.rep_page, self.com, repme, addings = False, regex = duplicateRegex)
                if not result:
                    return True # If Errors, exit (but continue the check)

            if older_image != self.imageName:
                return False # The image is a duplicate, it will be deleted. So skip the check-part, useless
        return True # Ok - No problem. Let's continue the checking phase

    def report_image(self, image_to_report, rep_page = None, com = None, rep_text = None, addings = True, regex = None):
        """ Report the files to the report page when needed. """
        if not rep_page:
            rep_page = self.rep_page

        if not com:
            com = self.com

        if not rep_text:
            rep_text = self.rep_text

        another_page = pywikibot.Page(self.site, rep_page)

        if not regex:
            regex = image_to_report
        try:
            text_get = another_page.get()
        except pywikibot.NoPage:
            text_get = ''
        except pywikibot.IsRedirectPage:
            text_get = another_page.getRedirectTarget().get()

        if len(text_get) >= self.logFulNumber:
            if self.logFullError:
                raise LogIsFull(u"The log page (%s) is full! Please delete the old files reported." % another_page.title())
            else:
                pywikibot.output(u"The log page (%s) is full! Please delete the old files reported. Skip!" % another_page.title())
                return True # Don't report, but continue with the check (we don't now if this is the first time we check this file or not)
        # The talk page includes "_" between the two names, in this way i replace them to " "
        n = re.compile(regex, re.UNICODE|re.DOTALL)
        y = n.findall(text_get)

        if y:
            pywikibot.output(u"%s is already in the report page." % image_to_report)
            reported = False
        else:
            # Adding the log
            if addings:
                rep_text = rep_text % image_to_report # Adding the name of the image in the report if not done already
            another_page.put(text_get + rep_text, comment = com, minorEdit = False)
            pywikibot.output(u"...Reported...")
            reported = True
        return reported

    def takesettings(self):
        """ Function to take the settings from the wiki. """
        settingsPage = pywikibot.translate(self.site, page_with_settings)
        try:
            if not settingsPage:
                self.settingsData = None
            else:
                wikiPage = pywikibot.Page(self.site, settingsPage)
                self.settingsData = list()
                try:
                    testo = wikiPage.get()
                    r = re.compile(r"<------- ------->\n"
                        "\*[Nn]ame ?= ?['\"](.*?)['\"]\n"
                        "\*([Ff]ind|[Ff]indonly)=(.*?)\n"
                        "\*[Ii]magechanges=(.*?)\n"
                        "\*[Ss]ummary=['\"](.*?)['\"]\n"
                        "\*[Hh]ead=['\"](.*?)['\"]\n"
                        "\*[Tt]ext ?= ?['\"](.*?)['\"]\n"
                        "\*[Mm]ex ?= ?['\"]?([^\n]*?)['\"]?\n", re.UNICODE|re.DOTALL)
                    number = 1

                    for m in r.finditer(testo):
                        name = str(m.group(1))
                        find_tipe = str(m.group(2))
                        find = str(m.group(3))
                        imagechanges = str(m.group(4))
                        summary = str(m.group(5))
                        head = str(m.group(6))
                        text = str(m.group(7))
                        mexcatched = str(m.group(8))
                        tupla = [number, name, find_tipe, find, imagechanges, summary, head, text, mexcatched]
                        self.settingsData += [tupla]
                        number += 1

                    if self.settingsData == list():
                        pywikibot.output(u"You've set wrongly your settings, please take a look to the relative page. (run without them)")
                        self.settingsData = None
                except pywikibot.NoPage:
                    pywikibot.output(u"The settings' page doesn't exist!")
                    self.settingsData = None
        except pywikibot.Error:
            # Error? Settings = None
            pywikibot.output(u'Problems with loading the settigs, run without them.')
            self.settingsData = None
            self.some_problem = False

        if not self.settingsData:
            self.settingsData = None

        # Real-Time page loaded
        if self.settingsData:
            pywikibot.output(u'\t   >> Loaded the real-time page... <<')
        # No settings found, No problem, continue.
        else:
            pywikibot.output(u'\t   >> No additional settings found! <<')
        return self.settingsData # Useless, but it doesn't harm..

    def load_licenses(self):
        """ Load the list of the licenses """
##        catName = pywikibot.translate(self.site, category_with_licenses)
##        cat = catlib.Category(pywikibot.getSite(), catName)
##        categories = [page.title() for page in pagegenerators.SubCategoriesPageGenerator(cat)]
##        categories.append(catName)
##        list_licenses = list()
##        pywikibot.output(u'\n\t...Loading the licenses allowed...\n')
##        for catName in categories:
##            cat = catlib.Category(pywikibot.getSite(), catName)
##            gen = pagegenerators.CategorizedPageGenerator(cat)
##            pages = [page for page in gen]
##            list_licenses.extend(pages)
        catName = pywikibot.translate(self.site, category_with_licenses)
        if not catName:
            raise pywikibot.Error(u'No licenses allowed provided, add that option to the code to make the script working correctly')
        pywikibot.output(u'\n\t...Loading the licenses allowed...\n')
        list_licenses = catlib.categoryAllPageObjectsAPI(catName)
        if self.site.lang == 'commons':
            no_licenses_to_skip = catlib.categoryAllPageObjectsAPI('Category:License-related tags')
            for license_given in no_licenses_to_skip:
                list_licenses.remove(license_given)
        pywikibot.output('') # blank line

        # Add the licenses set in the default page as licenses
        # to check
        if self.pageAllowed:
            try:
                pageAllowedText = pywikibot.Page(self.site, self.pageAllowed).get()
            except (pywikibot.NoPage, pywikibot.IsRedirectPage):
                pageAllowedText = ''

            for nameLicense in self.load(pageAllowedText):
                pageLicense = pywikibot.Page(self.site, nameLicense)
                if pageLicense not in list_licenses:
                    list_licenses.append(pageLicense) # the list has wiki-pages
        return list_licenses

    def miniTemplateCheck(self, template):
        """
        Is the template given in the licenses allowed or in the licenses to skip?
        This function check this.
        """
        if template in self.list_licenses: # the list_licenses are loaded in the __init__ (not to load them multimple times)
            self.license_selected = template.titleWithoutNamespace()
            self.seems_ok = True
            self.license_found = self.license_selected # let the last "fake" license normally detected
            return True

        if template in self.hiddentemplates:
            # if the whitetemplate is not in the images description, we don't care
            try:
                self.allLicenses.remove(template)
            except ValueError:
                return False
            else:
                self.whiteTemplatesFound = True
                return False

    def templateInList(self):
        """
        The problem is the calls to the Mediawiki system because they can be pretty slow.
        While searching in a list of objects is really fast, so first of all let's see if
        we can find something in the info that we already have, then make a deeper check.
        """
        for template in self.licenses_found:
            result = self.miniTemplateCheck(template)
            if result:
                break
        if not self.license_found:
            for template in self.licenses_found:
                try:
                    template.pageAPInfo()
                except pywikibot.IsRedirectPage:
                    template = template.getRedirectTarget()
                    result = self.miniTemplateCheck(template)
                    if result:
                        break
                except pywikibot.NoPage:
                    continue

    def smartDetection(self):
        """ The bot instead of checking if there's a simple template in the
            image's description, checks also if that template is a license or
            something else. In this sense this type of check is smart.
            """
        self.seems_ok = False
        self.license_found = None
        self.whiteTemplatesFound = False
        regex_find_licenses = re.compile(r'(?<!\{)\{\{(?:[Tt]emplate:|)([^{]+?)[|\n<}]', re.DOTALL)
        # see below to understand the use of this regex
        regex_are_licenses = re.compile(r'(?<!\{)\{\{(?:[Tt]emplate:|)([^{]+?)\}\}', re.DOTALL)
        #dummy_edit = False
        while 1:
            self.hiddentemplates = self.loadHiddenTemplates()
            self.licenses_found = self.image.getTemplates()
            templatesInTheImageRaw = regex_find_licenses.findall(self.imageCheckText)

            if not self.licenses_found and templatesInTheImageRaw:
                # {{nameTemplate|something <- this is not a template, be sure that we haven't catch something like that.
                licenses_TEST = regex_are_licenses.findall(self.imageCheckText)
                if not self.licenses_found and licenses_TEST:
                    raise pywikibot.Error("APIs seems down. No templates found with them but actually there are templates used in the image's page!")
            self.allLicenses = list()

            if not self.list_licenses:
                raise pywikibot.Error(u'No licenses allowed provided, add that option to the code to make the script working correctly')

            # Found the templates ONLY in the image's description
            for template_selected in templatesInTheImageRaw:
                for templateReal in self.licenses_found:
                    if self.convert_to_url(template_selected).lower().replace('template%3a', '') == \
                           self.convert_to_url(templateReal.title()).lower().replace('template%3a', ''):
                        if templateReal not in self.allLicenses: # don't put the same template, twice.
                            self.allLicenses.append(templateReal)
            # perform a dummy edit, sometimes there are problems with the Job queue
            # it happends that there is listed only the template used and not all the template that are in the templates used in the page
            # for example: there's only self, and not GFDL and the other licenses.
            #if self.allLicenses == self.licenses_found and not dummy_edit and self.licenses_found != []:
            #    pywikibot.output(u"Seems that there's a problem regarding the Job queue, trying with a dummy edit to solve the problem.")
            #    try:
            #        self.imageCheckText = self.image.get()
            #        self.image.put(self.imageCheckText, 'Bot: Dummy edit,if you see this comment write [[User talk:%s|here]].' % self.botnick)
            #    except (pywikibot.NoPage, pywikibot.IsRedirectPage):
            #        return (None, list())
            #    dummy_edit = True
            #else:
            break

        if self.licenses_found:
            self.templateInList()

            if not self.license_found and self.allLicenses:
                # If only iterlist = self.AllLicenses if I remove something
                # from iterlist it will be remove from self.AllLicenses too
                iterlist = list(self.allLicenses)

                for template in iterlist:
                    try:
                        template.pageAPInfo()
                    except pywikibot.IsRedirectPage:
                        template = template.getRedirectTarget()
                    except pywikibot.NoPage:
                        self.allLicenses.remove(template)

                if self.allLicenses:
                    self.license_found = self.allLicenses[0].title()
        self.some_problem = False # If it has "some_problem" it must check
                  # the additional settings.
        # if self.settingsData, use addictional settings
        if self.settingsData:
            self.findAdditionalProblems()

        if self.some_problem:
            if self.mex_used in self.imageCheckText:
                pywikibot.output(u'File already fixed. Skip.')
            else:
                pywikibot.output(u"The file's description for %s contains %s..." % (self.imageName, self.name_used))
                if self.mex_used.lower() == 'default':
                    self.mex_used = self.unvertext
                if self.imagestatus_used:
                    reported = True
                else:
                    reported = self.report_image(self.imageName)
                if reported:
                    #if self.imagestatus_used == True:
                    self.report(self.mex_used, self.imageName, self.text_used, u"\n%s\n" % self.head_used, None, self.imagestatus_used, self.summary_used)
                else:
                    pywikibot.output(u"Skipping the file...")
                self.some_problem = False
        else:
            if not self.seems_ok and self.license_found:
                rep_text_license_fake = u"\n*[[:File:%s]] seems to have " % self.imageName + \
                        "a ''fake license'', license detected: <nowiki>%s</nowiki>" % self.license_found
                regexFakeLicense = r"\* ?\[\[:File:%s\]\] seems to have " % (re.escape(self.imageName)) + \
                        "a ''fake license'', license detected: <nowiki>%s</nowiki>$" % (re.escape(self.license_found))
                printWithTimeZone(u"%s seems to have a fake license: %s, reporting..." % (self.imageName, self.license_found))
                self.report_image(self.imageName, rep_text = rep_text_license_fake,
                                       addings = False, regex = regexFakeLicense)
            elif self.license_found:
                printWithTimeZone(u"%s seems ok, license found: %s..." % (self.imageName, self.license_found))
        return (self.license_found, self.whiteTemplatesFound)

    def load(self, raw):
        """ Load a list of object from a string using regex. """
        list_loaded = list()
        pos = 0
        load_2 = True
        # I search with a regex how many user have not the talk page
        # and i put them in a list (i find it more easy and secure)
        regl = r"(\"|\')(.*?)\1(?:,|\])"
        pl = re.compile(regl, re.UNICODE)
        for xl in pl.finditer(raw):
            word = xl.group(2).replace(u'\\\\', u'\\')
            if word not in list_loaded:
                list_loaded.append(word)
        return list_loaded

    def skipImages(self, skip_number, limit):
        """ Given a number of files, skip the first -number- files. """
        # If the images to skip are more the images to check, make them the same number
        if skip_number == 0:
            pywikibot.output(u'\t\t>> No files to skip...<<')
            return False
        if skip_number > limit: skip_number = limit
        # Print a starting message only if no images has been skipped
        if not self.skip_list:
            if skip_number == 1:
                pywikibot.output(u'Skipping the first file:\n')
            else:
                pywikibot.output(u'Skipping the first %s files:\n' % skip_number)
        # If we still have pages to skip:
        if len(self.skip_list) < skip_number:
            pywikibot.output(u'Skipping %s...' % self.imageName)
            self.skip_list.append(self.imageName)
            if skip_number == 1:
                pywikibot.output('')
            return True
        else:
            pywikibot.output('') # Print a blank line.
            return False

    def wait(self, waitTime, generator, normal, limit):
        """ Skip the images uploaded before x seconds to let
            the users to fix the image's problem alone in the
            first x seconds.
        """
        imagesToSkip = 0
        # if normal, we can take as many images as "limit" has told us, otherwise, sorry, nope.
        if normal:
            printWithTimeZone(u'Skipping the files uploaded less than %s seconds ago..' % waitTime)
            imagesToSkip = 0
            while 1:
                loadOtherImages = True # ensure that all the images loaded aren't to skip!
                for image in generator:
                    if normal:
                        imageData = image
                        image = imageData[0]
                        #20100511133318L --- 15:33, 11 mag 2010 e 18 sec
                        b = str(imageData[1]) # use b as variable to make smaller the timestamp-formula used below..
                        # fixing the timestamp to the format that we normally use..
                        timestamp = "%s-%s-%sT%s:%s:%sZ" % (b[0:4], b[4:6], b[6:8], b[8:10], b[10:12], b[12:14])
                    else:
                        #http://pytz.sourceforge.net/ <- maybe useful?
                        # '2008-06-18T08:04:29Z'
                        timestamp = image.getLatestUploader()[1]
                    img_time = datetime.datetime.strptime(timestamp, u"%Y-%m-%dT%H:%M:%SZ") #not relative to localtime

                    now = datetime.datetime.strptime(str(datetime.datetime.utcnow()).split('.')[0], "%Y-%m-%d %H:%M:%S") #timezones are UTC
                    # + seconds to be sure that now > img_time
                    while now < img_time:
                        now = (now + datetime.timedelta(seconds=1))
                    delta = now - img_time
                    secs_of_diff = delta.seconds
                    if waitTime > secs_of_diff:
                        pywikibot.output(u'Skipping %s, uploaded %s seconds ago..' % (image.title(), int(secs_of_diff)))
                        imagesToSkip += 1
                        continue # Still wait
                    else:
                        loadOtherImages = False
                        break # No ok, continue
                # if yes, we have skipped all the images given!
                if loadOtherImages:
                    generator = self.site.newimages(number = limit, lestart = timestamp)
                    imagesToSkip = 0
                    # continue to load images! continue
                    continue
                else:
                    break # ok some other images, go below
            newGen = list()
            imagesToSkip += 1 # some calcs, better add 1
            # Add new images, instead of the images skipped
            newImages = self.site.newimages(number = imagesToSkip, lestart = timestamp)
            for imageData in generator:
                if normal:
                    image = imageData[0]
                    #20100511133318L --- 15:33, 11 mag 2010 e 18 sec
                    b = str(imageData[1]) # use b as variable to make smaller the timestamp-formula used below..
                    # fixing the timestamp to the format that we normally use..
                    timestamp = "%s-%s-%sT%s:%s:%sZ" % (b[0:4], b[4:6], b[6:8], b[8:10], b[10:12], b[12:14])
                    uploader = imageData[2]
                    comment = imageData[3]
                    newGen.append([image, timestamp, uploader, comment])
                else:
                    image = imageData
                    newGen.append(image)
            num = 0
            for imageData in newImages:
                newGen.append(imageData)
            return newGen
        else:
            pywikibot.output(u"The wait option is available only with the standard generator.")
            return generator

    def isTagged(self):
        """ Understand if a file is already tagged or not. """
        # Is the image already tagged? If yes, no need to double-check, skip
        for i in pywikibot.translate(self.site, txt_find):
            # If there are {{ use regex, otherwise no (if there's not the {{ may not be a template
            # and the regex will be wrong)
            if '{{' in i:
                regexP = re.compile(r'\{\{(?:template|)%s ?(?:\||\n|\}|<) ?' % i.split('{{')[1].replace(u' ', u'[ _]'), re.I)
                result = regexP.findall(self.imageCheckText)
                if result:
                    return True
            elif i.lower() in self.imageCheckText:
                return True

        return False # Nothing Found

    def findAdditionalProblems(self):
        # In every tupla there's a setting configuration
        for tupla in self.settingsData:
            name = tupla[1]
            find_tipe = tupla[2]
            find = tupla[3]
            find_list = self.load(find)
            imagechanges = tupla[4]
            if imagechanges.lower() == 'false':
                imagestatus = False
            elif imagechanges.lower() == 'true':
                imagestatus = True
            else:
                pywikibot.output(u"Error! Imagechanges set wrongly!")
                self.settingsData = None
                break
            summary = tupla[5]
            head_2 = tupla[6]
            text = tupla[7] % self.imageName
            mexCatched = tupla[8]
            for k in find_list:
                if find_tipe.lower() == 'findonly':
                    searchResults = re.findall(r'%s' % k.lower(), self.imageCheckText.lower())
                    if searchResults != []:
                        if searchResults[0] == self.imageCheckText.lower():
                            self.some_problem = True
                            self.text_used = text
                            self.head_used = head_2
                            self.imagestatus_used = imagestatus
                            self.name_used = name
                            self.summary_used = summary
                            self.mex_used = mexCatched
                            break
                elif find_tipe.lower() == 'find':
                    if re.findall(r'%s' % k.lower(), self.imageCheckText.lower()) != []:
                        self.some_problem = True
                        self.text_used = text
                        self.head_used = head_2
                        self.imagestatus_used = imagestatus
                        self.name_used = name
                        self.summary_used = summary
                        self.mex_used = mexCatched
                        continue

    def checkStep(self):
        # nothing = Defining an empty image description
        nothing = ['', ' ', '  ', '   ', '\n', '\n ', '\n  ', '\n\n', '\n \n', ' \n', ' \n ', ' \n \n']
        # something = Minimal requirements for an image description.
        # If this fits, no tagging will take place (if there aren't other issues)
        # MIT license is ok on italian wikipedia, let also this here
        something = ['{{'] # Don't put "}}" here, please. Useless and can give problems.
        # Unused file extensions. Does not contain PDF.
        notallowed = ("xcf", "xls", "sxw", "sxi", "sxc", "sxd")
        brackets = False
        delete = False
        extension = self.imageName.split('.')[-1] # get the extension from the image's name
        # Load the notification messages
        HiddenTN = pywikibot.translate(self.site, HiddenTemplateNotification)
        self.unvertext = pywikibot.translate(self.site, n_txt)
        di = pywikibot.translate(self.site, delete_immediately)
        dih = pywikibot.translate(self.site, delete_immediately_head)
        din = pywikibot.translate(self.site, delete_immediately_notification)
        nh = pywikibot.translate(self.site, nothing_head)
        nn = pywikibot.translate(self.site, nothing_notification)
        dels = pywikibot.translate(self.site, del_comm)
        smwl = pywikibot.translate(self.site, second_message_without_license)

        # Some formatting for delete immediately template
        di = u'\n%s' % di
        dels = dels % di

        # Page => ImagePage
        # Get the text in the image (called imageCheckText)
        try:
            # the checkText will be modified in order to make the check phase easier
            # the imageFullText will be used when the full text is needed without changes
            self.imageCheckText = self.image.get()
            self.imageFullText = self.imageCheckText
        except pywikibot.NoPage:
            pywikibot.output(u"Skipping %s because it has been deleted." % self.imageName)
            return True
        except pywikibot.IsRedirectPage:
            pywikibot.output(u"Skipping %s because it's a redirect." % self.imageName)
            return True
        # Delete the fields where the templates cannot be loaded
        regex_nowiki = re.compile(r'<nowiki>(.*?)</nowiki>', re.DOTALL)
        regex_pre = re.compile(r'<pre>(.*?)</pre>', re.DOTALL)
        self.imageCheckText = regex_nowiki.sub('', self.imageCheckText); self.imageCheckText = regex_pre.sub('', self.imageCheckText)
        # Deleting the useless template from the description (before adding something
        # in the image the original text will be reloaded, don't worry).
        if self.isTagged():
            # Tagged? Yes, skip.
            printWithTimeZone(u'%s is already tagged...' % self.imageName)
            return True
        for a_word in something: # something is the array with {{, MIT License and so on.
            if a_word in self.imageCheckText:
                # There's a template, probably a license (or I hope so)
                brackets = True
        # Is the extension allowed? (is it an image or f.e. a .xls file?)
        for parl in notallowed:
            if parl.lower() in extension.lower():
                delete = True
        (license_found, hiddenTemplateFound) = self.smartDetection()
        # If the image exists (maybe it has been deleting during the oder
        # checking parts or something, who knows? ;-))
        #if p.exists(): <-- improve thebot, better to make as
        #                   less call to the server as possible
        # Here begins the check block.
        if brackets == True and license_found != None:
            # It works also without this... but i want only to be sure ^^
            brackets = False
            return True
        elif delete:
            pywikibot.output(u"%s is not a file!" % self.imageName)
            # Modify summary text
            pywikibot.setAction(dels)
            canctext = di % extension
            notification = din % self.imageName
            head = dih
            self.report(canctext, self.imageName, notification, head)
            delete = False
            return True
        elif self.imageCheckText in nothing:
            pywikibot.output(u"The file's description for %s does not contain a license template!" % self.imageName)
            if hiddenTemplateFound and HiddenTN != None and HiddenTN != '' and HiddenTN != ' ':
                notification = HiddenTN % self.imageName
            else:
                notification = nn % self.imageName
            head = nh
            self.report(self.unvertext, self.imageName, notification, head, smwl)
            return True
        else:
            pywikibot.output(u"%s has only text and not the specific license..." % self.imageName)
            if hiddenTemplateFound and HiddenTN != None and HiddenTN != '' and HiddenTN != ' ':
                notification = HiddenTN % self.imageName
            else:
                notification = nn % self.imageName
            head = nh
            self.report(self.unvertext, self.imageName, notification, head, smwl)
            return True

gbv = Global()

def checkbot():
    """ Main function """
    # Command line configurable parameters
    repeat = True # Restart after having check all the images?
    limit = 80 # How many images check?
    time_sleep = 30 # How many time sleep after the check?
    skip_number = 0 # How many images to skip before checking?
    waitTime = 0 # How many time sleep before the check?
    commonsActive = False # Check if on commons there's an image with the same name?
    normal = False # Check the new images or use another generator?
    urlUsed = False # Use the url-related function instead of the new-pages generator
    regexGen = False # Use the regex generator
    untagged = False # Use the untagged generator
    duplicatesActive = False # Use the duplicate option
    duplicatesReport = False # Use the duplicate-report option
    sendemailActive = False # Use the send-email
    logFullError = True # Raise an error when the log is full

    # Here below there are the parameters.
    for arg in pywikibot.handleArgs():
        if arg.startswith('-limit'):
            if len(arg) == 7:
                limit = int(pywikibot.input(u'How many files do you want to check?'))
            else:
                limit = int(arg[7:])
        if arg.startswith('-time'):
            if len(arg) == 5:
                time_sleep = int(pywikibot.input(u'How many seconds do you want runs to be apart?'))
            else:
                time_sleep = int(arg[6:])
        elif arg == '-break':
            repeat = False
        elif arg == '-nologerror':
            logFullError = False
        elif arg == '-commons':
            commonsActive = True
        elif arg.startswith('-duplicates'):
            duplicatesActive = True
            if len(arg) == 11:
                duplicates_rollback = 1
            elif len(arg) > 11:
                duplicates_rollback = int(arg[12:])
        elif arg == '-duplicatereport':
            duplicatesReport = True
        elif arg == '-sendemail':
            sendemailActive = True
        elif arg.startswith('-skip'):
            if len(arg) == 5:
                skip = True
                skip_number = int(pywikibot.input(u'How many files do you want to skip?'))
            elif len(arg) > 5:
                skip = True
                skip_number = int(arg[6:])
        elif arg.startswith('-wait'):
            if len(arg) == 5:
                wait = True
                waitTime = int(pywikibot.input(u'How many time do you want to wait before checking the files?'))
            elif len(arg) > 5:
                wait = True
                waitTime = int(arg[6:])
        elif arg.startswith('-start'):
            if len(arg) == 6:
                firstPageTitle = pywikibot.input(u'From witch page do you want to start?')
            elif len(arg) > 6:
                firstPageTitle = arg[7:]
            firstPageTitle = firstPageTitle.split(":")[1:]
            generator = pywikibot.getSite().allpages(start=firstPageTitle, namespace=6)
            repeat = False
        elif arg.startswith('-page'):
            if len(arg) == 5:
                regexPageName = str(pywikibot.input(u'Which page do you want to use for the regex?'))
            elif len(arg) > 5:
                regexPageName = str(arg[6:])
            repeat = False
            regexGen = True
        elif arg.startswith('-url'):
            if len(arg) == 4:
                regexPageUrl = str(pywikibot.input(u'Which url do you want to use for the regex?'))
            elif len(arg) > 4:
                regexPageUrl = str(arg[5:])
            urlUsed = True
            repeat = False
            regexGen = True
        elif arg.startswith('-regex'):
            if len(arg) == 6:
                regexpToUse = str(pywikibot.input(u'Which regex do you want to use?'))
            elif len(arg) > 6:
                regexpToUse = str(arg[7:])
            generator = 'regex'
            repeat = False
        elif arg.startswith('-cat'):
            if len(arg) == 4:
                catName = str(pywikibot.input(u'In which category do I work?'))
            elif len(arg) > 4:
                catName = str(arg[5:])
            catSelected = catlib.Category(pywikibot.getSite(), 'Category:%s' % catName)
            generator = pagegenerators.CategorizedPageGenerator(catSelected)
            repeat = False
        elif arg.startswith('-ref'):
            if len(arg) == 4:
                refName = str(pywikibot.input(u'The references of what page should I parse?'))
            elif len(arg) > 4:
                refName = str(arg[5:])
            generator = pagegenerators.ReferringPageGenerator(pywikibot.Page(pywikibot.getSite(), refName))
            repeat = False
        elif arg.startswith('-untagged'):
            untagged = True
            if len(arg) == 9:
                projectUntagged = str(pywikibot.input(u'In which project should I work?'))
            elif len(arg) > 9:
                projectUntagged = str(arg[10:])

    # Understand if the generator it's the default or not.
    try:
        generator
    except NameError:
        normal = True

    # Define the site.
    site = pywikibot.getSite()

    # Block of text to translate the parameters set above.
    image_old_namespace = u"%s:" % site.image_namespace()
    image_namespace = u"File:"

    # If the images to skip are 0, set the skip variable to False (the same for the wait time)
    if skip_number == 0:
        skip = False
    if waitTime == 0:
        wait = False

    # A little block-statement to ensure that the bot will not start with en-parameters
    if site.lang not in project_inserted:
        pywikibot.output(u"Your project is not supported by this script. You have to edit the script and add it!")
        return

    # Reading the log of the new images if another generator is not given.
    if normal == True:
        if limit == 1:
            pywikibot.output(u"Retrieving the latest file for checking...")
        else:
            pywikibot.output(u"Retrieving the latest %d files for checking..." % limit)
    # Main Loop
    while 1:
        # Defing the Main Class.
        mainClass = main(site, sendemailActive = sendemailActive,
                         duplicatesReport = duplicatesReport, logFullError = logFullError)
        # Untagged is True? Let's take that generator
        if untagged == True:
            generator =  mainClass.untaggedGenerator(projectUntagged, limit)
            normal = False # Ensure that normal is False
        # Normal True? Take the default generator
        if normal == True:
            generator = site.newimages(number = limit)
        # if urlUsed and regexGen, get the source for the generator
        if urlUsed == True and regexGen == True:
            textRegex = site.getUrl(regexPageUrl, no_hostname = True)
        # Not an url but a wiki page as "source" for the regex
        elif regexGen == True:
            pageRegex = pywikibot.Page(site, regexPageName)
            try:
                textRegex = pageRegex.get()
            except pywikibot.NoPage:
                pywikibot.output(u"%s doesn't exist!" % pageRegex.title())
                textRegex = '' # No source, so the bot will quit later.
        # If generator is the regex' one, use your own Generator using an url or page and a regex.
        if generator == 'regex' and regexGen == True:
            generator = mainClass.regexGenerator(regexpToUse, textRegex)
        # Ok, We (should) have a generator, so let's go on.
        # Take the additional settings for the Project
        mainClass.takesettings()
        # Not the main, but the most important loop.
        #parsed = False
        if wait:
            # Let's sleep...
            generator = mainClass.wait(waitTime, generator, normal, limit)
        for image in generator:
            # When you've a lot of image to skip before working use this workaround, otherwise
            # let this commented, thanks. [ decoment also parsed = False if you want to use it
            #
            #if image.title() != u'File:Nytlogo379x64.gif' and not parsed:
            #    pywikibot.output(u"%s already parsed." % image.title())
            #    continue
            #else:
            #    parsed = True
            # If the generator returns something that is not an image, simply skip it.
            if normal == False and regexGen == False:
                if image_namespace.lower() not in image.title().lower() and \
                image_old_namespace.lower() not in image.title().lower() and 'file:' not in image.title().lower():
                    pywikibot.output(u'%s seems not an file, skip it...' % image.title())
                    continue
            if normal:
                imageData = image
                image = imageData[0]
                #20100511133318L --- 15:33, 11 mag 2010 e 18 sec
                #b = str(imageData[1]) # use b as variable to make smaller the timestamp-formula used below..
                # fixing the timestamp to the format that we normally use..
                timestamp = imageData[1]#"%s-%s-%sT%s:%s:%sZ" % (b[0:4], b[4:6], b[6:8], b[8:10], b[10:12], b[12:14])
                uploader = imageData[2]
                comment = imageData[3] # useless, in reality..
            else:
                timestamp = None
                uploader = None
                comment = None # useless, also this, let it here for further developments
            try:
                imageName = image.title().split(image_namespace)[1] # Deleting the namespace (useless here)
            except IndexError:# Namespace image not found, that's not an image! Let's skip...
                try:
                    imageName = image.title().split(image_old_namespace)[1]
                except IndexError:
                    pywikibot.output(u"%s is not a file, skipping..." % image.title())
                    continue
            mainClass.setParameters(imageName, timestamp, uploader) # Setting the image for the main class
            # Skip block
            if skip == True:
                skip = mainClass.skipImages(skip_number, limit)
                if skip == True:
                    continue
            # Check on commons if there's already an image with the same name
            if commonsActive == True and site.family.name != "commons":
                response = mainClass.checkImageOnCommons()
                if response == False:
                    continue
            # Check if there are duplicates of the image on the project selected
            if duplicatesActive == True:
                response2 = mainClass.checkImageDuplicated(duplicates_rollback)
                if response2 == False:
                    continue
            resultCheck = mainClass.checkStep()
            if resultCheck:
                continue
    # A little block to perform the repeat or to break.
        if repeat == True:
            printWithTimeZone(u"Waiting for %s seconds," % time_sleep)
            time.sleep(time_sleep)
        else:
            pywikibot.output(u"\t\t\t>> STOP! <<")
            break # Exit


# Main loop will take all the (name of the) images and then i'll check them.
if __name__ == "__main__":
    old = datetime.datetime.strptime(str(datetime.datetime.utcnow()).split('.')[0], "%Y-%m-%d %H:%M:%S") #timezones are UTC
    try:
        checkbot()
    finally:
        final = datetime.datetime.strptime(str(datetime.datetime.utcnow()).split('.')[0], "%Y-%m-%d %H:%M:%S") #timezones are UTC
        delta = final - old
        secs_of_diff = delta.seconds
        pywikibot.output("Execution time: %s" % secs_of_diff)
        pywikibot.stopme()
