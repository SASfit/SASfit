#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Script to help a human solve disambiguations by presenting a set of options.

Specify the disambiguation page on the command line, or enter it at the
prompt after starting the program. (If the disambiguation page title starts
with a '-', you cannot name it on the command line, but you can enter it at
the prompt.)  The program will pick up the page, and look for all
alternative links, and show them with a number adjacent to them.  It will
then automatically loop over all pages referring to the disambiguation page,
and show 30 characters of context on each side of the reference to help you
make the decision between the alternatives.  It will ask you to type the
number of the appropriate replacement, and perform the change.

It is possible to choose to replace only the link (just type the number) or
replace both link and link-text (type 'r' followed by the number).

Multiple references in one page will be scanned in order, but typing 'n'
(next) on any one of them will leave the complete page unchanged. To leave
only some reference unchanged, use the 's' (skip) option.

Command line options:

   -pos:XXXX   adds XXXX as an alternative disambiguation

   -just       only use the alternatives given on the command line, do not
               read the page for other possibilities

   -dnskip     Skip links already marked with a disambiguation-needed
               template (e.g., {{dn}})

   -primary    "primary topic" disambiguation (Begriffsklärung nach Modell 2).
               That's titles where one topic is much more important, the
               disambiguation page is saved somewhere else, and the important
               topic gets the nice name.

   -primary:XY like the above, but use XY as the only alternative, instead of
               searching for alternatives in [[Keyword (disambiguation)]].
               Note: this is the same as -primary -just -pos:XY

   -file:XYZ   reads a list of pages from a text file. XYZ is the name of the
               file from which the list is taken. If XYZ is not given, the
               user is asked for a filename. Page titles should be inside
               [[double brackets]]. The -pos parameter won't work if -file
               is used.

   -always:XY  instead of asking the user what to do, always perform the same
               action. For example, XY can be "r0", "u" or "2". Be careful with
               this option, and check the changes made by the bot. Note that
               some choices for XY don't make sense and will result in a loop,
               e.g. "l" or "m".

   -main       only check pages in the main namespace, not in the talk,
               wikipedia, user, etc. namespaces.

   -start:XY   goes through all disambiguation pages in the category on your
               wiki that is defined (to the bot) as the category containing
               disambiguation pages, starting at XY. If only '-start' or
               '-start:' is given, it starts at the beginning.

   -min:XX     (XX being a number) only work on disambiguation pages for which
               at least XX are to be worked on.

To complete a move of a page, one can use:

    python solve_disambiguation.py -just -pos:New_Name Old_Name

"""

#
# (C) Rob W.W. Hooft, 2003
# (C) Daniel Herding, 2004
# (C) Andre Engels, 2003-2004
# (C) WikiWichtel, 2004
# (C) Pywikipedia team, 2003-2009
#
__version__='$Id: solve_disambiguation.py 9167 2011-04-12 09:28:03Z xqt $'
#
# Distributed under the terms of the MIT license.
#


# Standard library imports
import re, sys, codecs

# Application specific imports
import wikipedia as pywikibot
import editarticle
import pagegenerators

# Summary message when working on disambiguation pages
msg = {
    'als': u'Bot-unterstitzti Begriffsklärig: %s - Link uustuscht dur %s',
    'ar': u'توضيح بمساعدة روبوت: %s - غير الوصلة أو الوصلات إلى %s',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — зьмененая спасылка(і) на %s',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - cheñchet liamm(où) e %s',
    'ca': u'Bot:Desambiguació assistida: %s - Canviant enllaç(os) per %s',
    'cs': u'Odstranění linku na rozcestník [[%s]] s použitím robota - Změněn(y) odkaz(y) na %s',
    'da': u'Retter flertydigt link til: %s - Ændrede link(s) til %s',
    'de': u'Bot-unterstützte Begriffsklärung: %s - Link(s) ersetzt durch %s',
    'en': u'Robot-assisted disambiguation: %s - Changed link(s) to %s',
    'eo': u'Robota unusencigo: %s - Ŝanĝis ligo(j)n al %s',
    'es': u'Bot:Desambiguación asistida: %s - Cambiando enlace(s) para %s',
    'fa': u'ابهام زدایی به کمک ربات: %s - پیوند به [[%s]]',
    'fi': u'Täsmennystä botin avulla: %s korvattiin link(e)illä %s',
    'fr': u'Homonymie résolue à l’aide du robot: %s - Modifications du (des) lien(s) pour %s',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Changements du (des) lim(s) por %s',
    'he': u'תיקון קישור לדף פירושונים באמצעות בוט: %s – שינוי הקישור(ים) ל%s',
    'hu': u'Bottal végzett egyértelműsítés: %s –> %s',
    'ia': u'Disambiguation assistite per robot: %s - Changed link(s) to %s',
    'it': u'Sistemazione automatica della disambigua: %s - Inversione di redirect %s',
    'ja': u'ロボット補助による曖昧さ回避：　%s - リンクを%sに変更しました',
    'kk': u'Айрықты мағыналарды бот көмегімен шешу: %s - Changed link(s) to %s',
    'ko': u'로봇의 도움을 받아 동음이의 처리 : [[%s]] - %s 문서로 링크 걸음',
    'lb': u'Bot-ënnerstetzten Homonymie: %s - Geännert(e) Link(en) op %s',
    'lt': u'Nuorodų į nukrepiamąjį straipsnį keitimas: %s - Pakeistos nuorodos į %s',
    'mk': u'Роботизирано појаснување: %s - Измена на врска/ки во %s',
    'ne': u'रोबोट-सहायक अस्पष्ट: %s बाट - लिङ्क(हरु) लाइ %s मा परिवर्तन गरियो',
    'nl': u'Botgeholpen doorverwijzing: [[%s]] - Verwijzing(en) gewijzigd naar %s',
    'no': u'bot: Retter lenke til peker: %s - Endret lenke(r) til %s',
    'pl': u'Wspomagane przez robota ujednoznacznienie: %s - Zmieniono link(i) %s',
    'pt': u'Desambiguação assistida por bot: %s link(s) mudado(s) para %s',
    'ru': u'Разрешение значений с помощью бота: %s - Changed link(s) to %s',
    'sr': u'Решавање вишезначних одредница помоћу бота: %s - Changed link(s) to %s',
    'sv': u'Länkar direkt till rätt artikel för: %s - Bytte länk(ar) till %s',
    'tt-cyrl': u'Бот ярдәмендә мәгънәләр киңәйтелмәсе: %s - Changed link(s) to %s',
    'uk': u'Виправлення посилання на багатозначність за допомогою бота: %s змінено на %s',
    'vi': u'Rôbốt giúp định hướng: %s – Đổi liên kết thành %s',
    }

# Summary message when working on disambiguation pages and the link is removed
msg_unlink = {
    'af': u'Robot-ondersteunde aanstuur: %s - skakel(s) verwyder.',
    'als': u'Bot-unterstitzti Begriffsklärig: %s - Link uusegnuu',
    'ar': u'توضيح بمساعدة روبوت: %s - أزال الوصلة أو الوصلات.',
    'ast': u'Dixebra asistida por robot: %s - Enllaz desaniciáu.',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — выдаленая спасылка(і).',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - liamm(où) tennet.',
    'bs': u'Čvor podržan od bota: %s - Uklonjen(i) link(ovi).',
    'ca': u'Desambiguació assistida: %s - Eliminant enllaç(os).',
    'cs': u'Odstranění linku na rozcestník [[%s]] s použitím robota - Odstraněn(y) odkaz(y)',
    'cy': u'Gwahaniaethu gyda chymorth robot: %s - Dolenni a dynnwyd.',
    'da': u'Retter flertydigt link til: %s - Fjernede link(s)',
    'de': u'Bot-unterstützte Begriffsklärung: %s - Link(s) entfernt',
    'el': u'Υποβοηθούμενη αποσαφήνιση από Ρομπότ: %s - Σύνδεσμος(οι) αφαιρέθηκε(αν).',
    'en': u'Robot-assisted disambiguation: %s - Removed link(s).',
    'eo': u'Robota unusencigo: %s - Forigis ligo(j)n',
    'fa': u'ابهام زدایی به کمک ربات: حذف %s',
    'fi': u'Täsmennystä botin avulla: %s - poistettiin linkkejä.',
    'fr': u'Homonymie résolue à l’aide du robot: %s - Retrait du (des) lien(s)',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Retrèt du (des) lim(s).',
    'gl': u'Páxina de homónimos resolta polo bot: eliminou unha ou varias ligazóns de "%s"',
    'gsw': u'Bot-unterstitzti Begriffsklärig: %s - Link uusegnuu',
    'he': u'הסרת קישור לדף פירושונים באמצעות בוט: %s',
    'hu': u'Bottal végzett egyértelműsítés: %s – hivatkozások eltávolítása',
    'ia': u'Disambiguation assistite per robot: %s - Removed link(s).',
    'id': u'Disambiguasi berbantuan bot: %s - Tautan dihapus',
    'it': u'Sistemazione automatica della disambigua: %s - Collegamenti rimossi',
    'ja': u'ロボット補助による曖昧さ回避：　%s - リンクを除去しました',
    'kk': u'Айрықты мағыналарды бот көмегімен шешу: %s - Removed link(s).',
    'ko': u'로봇의 도움을 받아 동음이의 처리: [[%s]] - 링크 제거',
    'ksh': u'Bot: Watt-ėßß-datt?-Ömleidongs-Sigg %s — Lengk udder Lengks eruß jenumme.',
    'lb': u'Bot-ënnerstetzten Homonymie: %s - Link(en) ewechgeholl',
    'lt': u'Nuorodų į nukrepiamąjį straipsnį keitimas: %s - Pašalintos nuorodos',
    'mk': u'Роботизирано појаснување: %s - Отстранување на врска/ки',
    'ms': u'Penyahkekaburan bantuan bot: %s - Pautan digugurkan',
    'mt': u'Sistemazzjoni awtomatika tar-rindirizz: %s - Ħolqa/Ħoloq imneħħija',
    'nds': u'Bot-stütt MB: %s - Lenken rutnahmen',
    'ne': u'रोबोट-सहायक अस्पष्टता: %s लाइ - लिङ्क(हरु) हटाइयो',
    'nl': u'Robotgeholpen doorverwijzing: [[%s]] - Verwijzing(en) verwijderd',
    'no': u'bot: Retter lenke til peker: %s - Fjernet lenke(r)',
    'pl': u'Wspomagane przez robota ujednoznacznienie: %s - Usunięto link(i)',
    'pt': u'Desambiguação auxiliada por robô: %s - Links removidos.',
    'pt-br': u'Desambiguação auxiliada por robô: %s link(s) removido(s)',
    'ru': u'Разрешение значений с помощью бота: %s - Removed link(s)',
    'sl': u'Razločitev s pomočjo robota: %s – Odstranitev povezav',
    'sr': u'Решавање вишезначних одредница помоћу бота: %s - Removed link(s)',
    'sr-ec': u'Роботова вишезначна одредница: %s – уклоњене везе.',
    'sr-el': u'Robotova višeznačna odrednica: %s – uklonjene veze.',
    'sv': u'Länkar direkt till rätt artikel för: %s - Tog bort länk(ar)',
    'tl': u'Paglilinaw na may tulong ng robot: %s - Tinanggal na (mga) kawing.',
    'tr': u'Robot destekli anlam ayrımı: %s - Bağlantı(lar) kaldırıldı.',
    'tt-cyrl': u'Бот ярдәмендә мәгънәләр киңәйтелмәсе: %s - Removed link(s)',
    'uk': u'Виправлення посилання на багатозначність за допомогою бота: %s вилучено',
    'vi': u'Robot giúp định hướng: %s – Dời liên kết',
    'zh-hans': u'机器人辅助消歧义：%s - 移除链接',
    'zh-hant': u'機器人輔助消除歧義：%s - 移除鏈接。',
    }

# Summary message when working on redirects
msg_redir = {
    'als': u'Bot-unterstitzti Wyterleitigsuflesig: %s - Link uustuscht dur %s',
    'ar': u'توضيح بمساعدة روبوت: %s - غير الوصلة أو الوصلات إلى %s',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — зьмененая спасылка(і) на %s',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - cheñchet liamm(où) e %s',
    'ca': u'Desambiguació assistida: %s - Canviant enllaç(os) a %s',
    'cs': u'Robot opravil přesměrování na %s - Změněn(y) odkaz(y) na %s',
    'da': u'Retter flertydigt link til: %s - Ændrede link(s) til %s',
    'de': u'Bot-unterstützte Weiterleitungsauflösung: %s - Link(s) ersetzt durch %s',
    'en': u'Robot-assisted disambiguation: %s - Changed link(s) to %s',
    'eo': u'Robota unusencigo: %s - Ŝanĝis ligo(j)n al %s',
    'fa': u'ابهام زدایی به کمک ربات: %s - پیوند به [[%s]]',
    'fi': u'Täsmennystä botin avulla: %s korvattiin link(e)illä %s',
    'fr': u'Correction de lien vers redirect: %s - Modifications du (des) lien(s) pour %s',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Changements du (des) lim(s) por %s',
    'he': u'תיקון קישור לדף פירושונים באמצעות בוט: %s שונה ל%s',
    'hu': u'Bottal végzett egyértelműsítés: %s –> %s',
    'ia': u'Resolution de redirectiones assistite per robot: %s - Changed link(s) to %s',
    'it': u'Sistemazione automatica del redirect: %s - Inversione di redirect %s',
    'ja': u'ロボット補助による曖昧さ回避：　%s - リンクを%sに変更しました',
    'kk': u'Айрықты мағыналарды бот көмегімен шешу: %s - Changed link(s) to %s',
    'ko': u'로봇의 도움을 받아 동음이의 처리: [[%s]] - %s 문서로 링크 걸음',
    'lb': u'Bot-ënnerstetzten Homonymie: %s - Geännert(e) Link(en) op %s',
    'lt': u'Nuorodų į peradresavimo straipsnį keitimas: %s - Pakeistos nuorodos į %s',
    'mk': u'Роботизирано појаснување: %s - Измена на врска/ки во %s',
    'ne': u'रोबोट-सहायक अस्पष्ट: %s लाइ - लिङ्क(हरु) लाइ %s मा परिवर्तन गरियो',
    'nl': u'Botgeholpen oplossing voor doorverwijzing: [[%s]] - Verwijzing(en) gewijzigd naar %s',
    'no': u'bot: Endrer omdirigeringslenke: %s - Endret lenke(r) til %s',
    'pl': u'Wspomagane przez robota ujednoznacznienie: %s - Zmieniono link(i) %s',
    'pt': u'Desambiguação assistida por bot: %s link(s) mudados para %s',
    'ru': u'Разрешение значений с помощью бота: %s - Changed link(s) to %s',
    'sr': u'Решавање вишезначних одредница помоћу бота: %s - Changed link(s) to %s',
    'sv': u'Länkar direkt till rätt artikel för: %s - Bytte länk(ar) till %s',
    'tt-cyrl': u'Бот ярдәмендә мәгънәләр киңәйтелмәсе: %s - Changed link(s) to %s',
    'uk': u'Виправлення посилання на багатозначність за допомогою бота: %s змінено на %s',
    'vi': u'Rôbốt giúp định hướng: %s – Đổi liên kết thành %s',
    }

# Summary message when working on redirects and the link is removed
msg_redir_unlink = {
    'als': u'Bot-unterstitzti Wyterleitigsuflesig: %s - Link uusegnuu',
    'ar': u'توضيح بمساعدة روبوت: %s - أزال الوصلة أو الوصلات',
    'ast': u'Dixebra asistida por robot: %s - Enllaz desaniciáu',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — выдаленая спасылка(і)',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - liamm(où) tennet',
    'bs': u'Čvor podržan od bota: %s - Uklonjen(i) link(ovi)',
    'ca': u'Desambiguació assistida: %s - Eliminant enllaç(os)',
    'cs': u'Robot opravil přesměrování na %s - Odstraněn(y) odkaz(y)',
    'cy': u'Gwahaniaethu gyda chymorth robot: %s - Dolenni a dynnwyd',
    'da': u'Retter flertydigt link til: %s - Fjernede link(s)',
    'de': u'Bot-unterstützte Weiterleitungsauflösung: %s - Link(s) entfernt',
    'el': u'Υποβοηθούμενη αποσαφήνιση από ρομπότ: %s - Σύνδεσμος(οι) αφαιρέθηκε(αν)',
    'en': u'Robot-assisted disambiguation: %s - Removed link(s)',
    'eo': u'Robota unusencigo: %s - Forigis ligo(j)n',
    'fa': u'ابهام زدایی به کمک ربات: حذف %s',
    'fi': u'Täsmennystä botin avulla: %s - poistettiin linkkejä',
    'fr': u'Correction de lien vers redirect: %s - Retrait du (des) lien(s)',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Retrèt du (des) lim(s)',
    'gl': u'Páxina de homónimos resolta polo bot: eliminou unha ou varias ligazóns de "%s"',
    'gsw': u'Bot-unterstitzti Wyterleitigsuflesig: %s - Link uusegnuu',
    'he': u'הסרת קישור לדף פירושונים באמצעות בוט: %s',
    'hu': u'Bottal támogatott egyértelműsítés: %s – hivatkozások eltávolítása',
    'ia': u'Resolution de redirectiones assistite per robot: %s - Removed link(s).',
    'id': u'Disambiguasi berbantuan bot: %s - Tautan dihapus',
    'it': u'Sistemazione automatica del redirect: %s - Collegamenti rimossi',
    'ja': u'ロボット補助による曖昧さ回避：　%s - リンクを除去しました',
    'kk': u'Айрықты мағыналарды бот көмегімен шешу: %s - Removed link(s).',
    'ko': u'로봇의 도움을 받아 동음이의 처리: [[%s]] - 링크 제거',
    'ksh': u'Bot: Watt-ėßß-datt?-Ömleidongs-Sigg %s — Lengk udder Lengks eruß jenumme.',
    'lb': u'Bot-ënnerstetzten Homonymie: %s - Link(en) ewechgeholl',
    'lt': u'Nuorodų į peradresavimo straipsnį keitimas: %s - Pašalintos nuorodos',
    'mg': u'Fanitsiana rohy mankany amin\'ny fihodinana %s - rohy voala',
    'mk': u'Роботизирано појаснување: %s - Отстранување на врска/ки',
    'ms': u'Penyahkekaburan bantuan bot: %s - Pautan digugurkan',
    'mt': u'Sistemazzjoni awtomatika tar-rindirizz: %s - Ħolqa/Ħoloq imneħħija',
    'nds': u'Bot-stütt MB: %s - Lenken rutnahmen',
    'ne': u'रोबोट-सहायक अस्पष्ट: %s लाइ - लिङ्क(हरु) हटाइयो',
    'nl': u'Robotgeholpen oplossing voor doorverwijzing: [[%s]] - Verwijzing(en) verwijderd',
    'no': u'bot: Endrer omdirigeringslenke: %s - Fjernet lenke(r)',
    'pl': u'Wspomagane przez robota ujednoznacznienie: %s - Usunięto link(i)',
    'pt': u'Desambiguação auxiliada por robô: %s - Link(s) removidos',
    'pt-br': u'Desambiguação auxiliada por robô: %s - link(s) removido(s)',
    'ru': u'Разрешение значений с помощью бота: %s - Removed link(s)',
    'sl': u'Razločitev s pomočjo robota: %s – Odstranitev povezav',
    'sr': u'Решавање вишезначних одредница помоћу бота: %s - Removed link(s)',
    'sr-ec': u'Роботова вишезначна одредница: %s – уклоњене везе',
    'sr-el': u'Robotova višeznačna odrednica: %s – uklonjene veze',
    'sv': u'Länkar direkt till rätt artikel för: %s - Tog bort länk(ar)',
    'tl': u'Paglilinaw na tinutulungan ng robot: %s - Tinanggal na (mga) kawing',
    'tr': u'Robot destekli anlam ayrımı: %s - Bağlantı(lar) kaldırıldı.',
    'tt-cyrl': u'Бот ярдәмендә мәгънәләр киңәйтелмәсе: %s - Removed link(s)',
    'uk': u'Виправлення посилання на багатозначність за допомогою бота: %s вилучено',
    'vi': u'Robot giúp định hướng: %s – Dời liên kết',
    'zh-hans': u'机器人辅助消歧义：%s - 移除链接',
    'zh-hant': u'機器人輔助消除歧義：%s - 刪除鏈接',
    }

# Disambiguation Needed template
dn_template = {
    'en' : u'{{dn}}',
    }

# Summary message when adding Disambiguation Needed template
msg_dn = {
    'als': u'Bot-unterstitzti Begriffsklärig: %s - brucht Ufmerksamkeit vun eme Expert',
    'ar': u'توضيح بمساعدة روبوت: %s - التعليم كمحتاجة لانتباه خبير',
    'ast': u'Dixebra asistida por robot: %s - Marcada pa pidir l\'atención d\'un espertu',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — пазначаная як патрабуючая увагі экспэртаў',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - merket evel da vezañ pledet ganti gant ur mailh',
    'bs': u'Čvor podržan od bota: %s - Označen za pregled od strane stručnjaka',
    'ca': u'Desambiguació assistida per robot: %s - Necessita la revisió d\'un expert',
    'cs': u'Robotem asistovaný rozcestník: %s - Označeno, že vyžaduje pozornost odborníka',
    'cy': u'Gwahaniaethu gyda chymorth robot: %s - Marcir ar gyfer sylw arbenigwr',
    'de': u'Bot-unterstützte Begriffsklärung: %s - markiert, benötigt kundige Wartung',
    'en': u'Robot-assisted disambiguation: %s - Marked as needing expert attention',
    'eo': u'Robota unusencigo: %s - Markis ke ĝi bezonas atenton de eksperto.',
    'fa': u'ربات نیمه خودکار: علامت زدن %s به عنوان نیازمند بررسی بیشتر',
    'fr': u'Homonymie résolue à l’aide du robot : %s - marquée comme demandant l\'attention d\'un expert',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Marcâ coment demandent l’atencion d’un èxpèrt',
    'gl': u'Páxina de homónimos resolta polo bot: "%s" necesita atención dun experto',
    'gsw': u'Bot-unterstitzti Begriffsklärig: %s - brucht Ufmerksamkeit vun eme Expert',
    'he': u'טיפול בפירושונים בעזרת רובוט: %s - סומן כדורש תשומת לב ממומחה',
    'ia': u'Disambiguation con robot: %s - Marcate como necessitante le attention de un experto',
    'id': u'Disambiguasi berbantuan bot: %s - Perlu perhatian pakar',
    'ja': u'ロボット補助による曖昧さ回避：　%s - 専門家のチェックが必要として印付けしました',
    'ksh': u'Bot: Han de Watt-ėßß-datt?-Sigg %s makeet, doh moß ene Minsch noh looere.',
    'lb': u'Bot-assistéiert Homonymie: %s - markéiert fir duerch en Expert nogekuckt ze ginn',
    'mk': u'Роботизирано појаснување: %s - Означено како „потребно внимание од стручњак“',
    'ms': u'Penyahkekaburan bantuan bot: %s - Ditandai kerana memerlukan perhatian pakar',
    'mt': u'Sistemazzjoni awtomatika tar-rindirizz: %s - Immarkata bħala li teħtieġ attenzjoni minn espert/i',
    'nds': u'Bot-stütt MB: %s - Hülp von en Experten nödig',
    'ne': u'रोबोट-सहायक अस्पष्ट: %s लाइ - विशेषज्ञ को ध्यानाकर्षण गराउँदै',
    'nl': u'Robotgeholpen disambiguatie: %s - heeft aandacht van een expert nodig',
    'no': u'Robothjulpet løsing av flertydig lenke: %s – trenger eksperthjelp',
    'pl': u'Wspomagane robotem ujednoznacznienie – %s – oznaczone jako wymagające uwagi eksperta',
    'pt': u'Desambiguação auxiliada por robô: %s - Marcada como necessitando de atenção especializada',
    'pt-br': u'Desambiguação assistida por bot: %s - Marcada como necessitando de atenção especializada',
    'ru': u'Неоднозначность с помощью робота: %s — помечена как требующая внимания эксперта',
    'sl': u'Razločitev s pomočjo robota: %s – Označeno kot potrebno pozornosti strokovnjaka',
    'sr': u'Роботова вишезначна одредница: %s – означено као „потребна стручна пажња“',
    'sr-ec': u'Роботова вишезначна одредница: %s – означено као „потребна стручна пажња“',
    'sr-el': u'Robotova višeznačna odrednica: %s – označeno kao „potrebna stručna pažnja“',
    'sv': u'Robot-assisterad olika betydelser: %s - Märkt som i behov av uppmärksamhet från expert',
    'tl': u'Paglilinaw na may tulong ng robot: %s - Tinatakan bilang kailangan ng pagpansin ng dalubhasa',
    'tr': u'Robot destekli anlam ayrımı: %s - Uzman ilgisine ihtiyaç duyduğuna dair işaretleme gerçekleştirildi',
    'tt-cyrl': u'Робот ярдәмендә: %s — экспертның игътибарын сораучы дип билгеләнде',
    'uk': u'Усунення неоднозначності за допомогою робота: %s — Позначена як така, що потребує уваги експерта',
    'vi': u'Robot giúp định hướng: %s – Đánh dấu là cần chuyên gia chú ý',
    'zh-hans': u'机器人辅助消歧义：%s - 已标记为需要专家关注',
    'zh-hant': u'機器人輔助消除歧義：%s - 標記為需要專家關注',
    }

# Summary message when adding Disambiguation Needed template to a redirect link
msg_redir_dn = {
    'als': u'Bot-unterstitzti Begriffsklärig: %s - brucht Ufmerksamkeit vun eme Expert',
    'ar': u'توضيح بمساعدة روبوت: %s - التعليم كمحتاجة لانتباه خبير',
    'ast': u'Dixebra asistida por robot: %s - Marcada pa pidir l\'atención d\'un espertu',
    'be-tarask': u'Аўтаматычнае выпраўленьне неадназначнасьцяў: %s — пазначаная як патрабуючая ўвагі экспэртаў',
    'br': u'Kudenn diforc\'hañ diskoulmet dre ar robot : %s - merket evel da vezañ pledet ganti gant ur mailh',
    'bs': u'Čvor podržan od bota: %s - Označen za pregled od strane stručnjaka',
    'ca': u'Desambiguació assistida per robot: %s - Necessita la revisió d\'un expert',
    'cs': u'Robotem asistovaný rozcestník: %s - Označeno, že vyžaduje pozornost odborníka',
    'cy': u'Gwahaniaethu gyda chymorth robot: %s - Marcir ar gyfer sylw arbenigwr',
    'da': u'Robotassisteret flertydig: %s - markeret til at kræve en eksperts opmærksomhed',
    'de': u'Bot-unterstützte Begriffsklärung: %s - markiert, benötigt kundige Wartung',
    'el': u'Υποβοηθούμενη αποσαφήνιση από Ρομπότ: %s - Επισημάνθηκε ως χρήζουσα της προσοχής ενος ειδικού',
    'en': u'Robot-assisted disambiguation: %s - Marked as needing expert attention',
    'eo': u'Robota unusencigo: %s - Markis ke ĝi bezonas atenton de eksperto.',
    'fa': u'ربات نیمه خودکار: علامت زدن %s به عنوان نیازمند بررسی بیشتر',
    'fr': u'Homonymie résolue à l’aide du robot : %s - marquée comme demandant l\'attention d\'un expert',
    'frp': u'Homonimia solucionâ avouéc un robot : %s - Marcâ coment demandent l’atencion d’un èxpèrt',
    'gl': u'Páxina de homónimos resolta polo bot: "%s" necesita atención dun experto',
    'gsw': u'Bot-unterstitzti Begriffsklärig: %s - brucht Ufmerksamkeit vun eme Expert',
    'he': u'טיפול בפירושונים בעזרת רובוט: %s - סומן כדורש תשומת לב ממומחה',
    'ia': u'Disambiguation con robot: %s - Marcate como necessitante le attention de un experto',
    'id': u'Disambiguasi berbantuan bot: %s - Perlu perhatian pakar',
    'ja': u'ロボット補助による曖昧さ回避：　%s - 専門家のチェックが必要として印付けしました',
    'ksh': u'Bot: Han de Watt-ėßß-datt?-Ömleidongs-Sigg %s makeet, doh moß ene Minsch noh looere.',
    'lb': u'Bot-assistéiert Homonymie: %s - markéiert fir duerch en Expert nogekuckt ze ginn',
    'mg': u'Fitovizana anarana voavahaolana tamin\'ny alàlan\'ny rôbô : %s - mila fitandreman\'ny mpahay.',
    'mk': u'Роботизирано појаснување: %s - Означено како „потребно внимание од стручњак“',
    'ms': u'Penyahkekaburan bantuan bot: %s - Ditandai kerana memerlukan perhatian pakar',
    'mt': u'Sistemazzjoni awtomatika tar-rindirizz: %s - Immarkata bħala li teħtieġ attenzjoni minn espert/i',
    'nds': u'Bot-stütt MB: %s - Hülp von en Experten nödig',
    'ne': u'रोबोट-सहायक अस्पष्ट: %s लाइ - विशेषज्ञ को ध्यानाकर्षण गराउँदै',
    'nl': u'Robotgeholpen disambiguatie: %s - heeft aandacht van een expert nodig',
    'no': u'Robothjulpet løsing av flertydig lenke: %s – trenger eksperthjelp',
    'pl': u'Wspomagane robotem ujednoznacznienie – %s – oznaczone jako wymagające uwagi eksperta',
    'pt': u'Desambiguação auxiliada por robô: %s - Marcada como necessitando de atenção especializada',
    'pt-br': u'Desambiguação auxiliada por robô: %s - Marcado como precisando de atenção de especialistas',
    'ru': u'Неоднозначность с помощью робота: %s — помечена как требующая внимания эксперта',
    'sl': u'Razločitev s pomočjo robota: %s – Označeno kot potrebno pozornosti strokovnjaka',
    'sr-ec': u'Роботова вишезначна одредница: %s – означено као „потребна стручна пажња“',
    'sr-el': u'Robotova višeznačna odrednica: %s – označeno kao „potrebna stručna pažnja“',
    'sv': u'Robot-assisterad olika betydelser: %s - Märkt som i behov av uppmärksamhet från expert',
    'tl': u'Paglilinaw na may tulong ng robot: %s - Minarkahan bilang nangangailangan ng pagpansin ng dalubhasa',
    'tr': u'Robot destekli anlam ayrımı: %s - Uzman ilgisine ihtiyaç duyduğuna dair işaretleme gerçekleştirildi',
    'tt-cyrl': u'Робот ярдәмендә: %s — экспертның игътибарын сораучы дип билгеләнде',
    'uk': u'Усунення неоднозначності за допомогою робота: %s - Позначена як така, що потребує уваги експерта',
    'vi': u'Robot giúp định hướng: %s – Đánh dấu là cần chuyên gia chú ý',
    'zh-hans': u'机器人辅助消歧义：%s - 已标记为需要专家关注',
    'zh-hant': u'機器人輔助消除歧義：%s - 標記為需要專家關注',
    }

# Summary message to (unknown)
unknown_msg = {
    'af': u'(onbekend)',
    'als': u'(nit bekannt)',
    'ar': u'(غير معروف)',
    'ast': u'(desconocíu)',
    'be-tarask': u'(невядома)',
    'bn': u'(অজানা)',
    'br': u'(dianav)',
    'bs': u'(nepoznato)',
    'ca': u'(desconegut)',
    'ckb': u'(نەناسراو)',
    'cs': u'(neznámé)',
    'cy': u'(anhysbys)',
    'da': u'(ukendt)',
    'de': u'(unbekannt)',
    'el': u'(άγνωστο)',
    'en': u'(unknown)',
    'eo': u'(nesciata)',
    'es': u'(desconocido)',
    'eu': u'(ezezaguna)',
    'fa': u'(نامعلوم)',
    'fi': u'(tuntematon)',
    'fr': u'(inconnu)',
    'frp': u'(encognu)',
    'fur': u'(no cognossût)',
    'gl': u'(descoñecido)',
    'gsw': u'(nit bekannt)',
    'he': u'(לא ידוע)',
    'hsb': u'[njeznaty]',
    'hu': u'(ismeretlen)',
    'ia': u'(incognite)',
    'id': u'(tidak dikenal)',
    'it': u'(sconosciuto)',
    'ja': u'（不明）',
    'ksh': u'(onbekannt)',
    'ku-latn': u'(nenas)',
    'lb': u'(onbekannt)',
    'lt': u'(nežinomas)',
    'ltg': u'(nazynoms)',
    'lv': u'(nezināms)',
    'mg': u'(tsy fantatra)',
    'mk': u'(непозната)',
    'ml': u'(അപരിചിതം)',
    'ms': u'(tidak diketahui)',
    'mt': u'(mhux magħruf)',
    'my': u'(အမည်မသိ)',
    'nah': u'(âmò ìxmatkàyö)',
    'nds': u'(nich kennt)',
    'ne': u'[अज्ञात]',
    'nl': u'(onbekend)',
    'nn': u'(ukjend)',
    'no': u'(ukjent)',
    'pl': u'(nieznana)',
    'ps': u'(ناڅرګند)',
    'pt': u'(desconhecido)',
    'pt-br': u'(desconhecido)',
    'ro': u'(necunoscut)',
    'ru': u'(неизвестно)',
    'rue': u'(незнаме)',
    'si': u'(නොදත්)',
    'sl': u'(neznano)',
    'sr': u'(непознато)',
    'sr-ec': u'(непознато)',
    'sr-el': u'(nepoznato)',
    'sv': u'(okänd)',
    'tl': u'(hindi nalalaman)',
    'tr': u'(bilinmiyor)',
    'tt-cyrl': u'(билгесез)',
    'uk': u'(невідома)',
    'vi': u'(không rõ)',
    'vo': u'(nesevädik)',
    'yi': u'(אומבאַקאַנט)',
    'zh-hans': u'(未知)',
    'zh-hant': u'(未知)',
    }

# disambiguation page name format for "primary topic" disambiguations
# (Begriffsklärungen nach Modell 2)
primary_topic_format = {
    'ar': u'%s_(توضيح)',
    'ca': u'%s_(desambiguació)',
    'cs': u'%s_(rozcestník)',
    'de': u'%s_(Begriffsklärung)',
    'en': u'%s_(disambiguation)',
    'fa': u'%s_(ابهام‌زدایی)',
    'fi': u'%s_(täsmennyssivu)',
    'hu': u'%s_(egyértelműsítő lap)',
    'ia': u'%s_(disambiguation)',
    'it': u'%s_(disambigua)',
    'lt': u'%s_(reikšmės)',
    'kk': u'%s_(айрық)',
    'ko': u'%s_(동음이의)',
    'nl': u'%s_(doorverwijspagina)',
    'no': u'%s_(peker)',
    'pl': u'%s_(ujednoznacznienie)',
    'pt': u'%s_(desambiguação)',
    'he': u'%s_(פירושונים)',
    'ru': u'%s_(значения)',
    'sr': u'%s_(вишезначна одредница)',
    'sv': u'%s_(olika betydelser)',
    'uk': u'%s_(значення)',
    }

# List pages that will be ignored if they got a link to a disambiguation
# page. An example is a page listing disambiguations articles.
# Special chars should be encoded with unicode (\x##) and space used
# instead of _

ignore_title = {
    'wikipedia': {
        'ar': [
            u'تصنيف:صفحات توضيح',
        ],
        'ca': [
            u'Viquipèdia:Enllaços incorrectes a pàgines de desambiguació',
            u'Viquipèdia:Registre de pàgines de desambiguació òrfenes',
            u'.*Discussió:.+',
            u'.*Usuari:.+',
            u'.+/[aA]rxiu.*',
        ],
        'cs': [
            u'Wikipedie:Chybějící interwiki/.+',
            u'Wikipedie:Rozcestníky',
            u'Wikipedie diskuse:Rozcestníky',
            u'Wikipedie:Seznam nejvíce odkazovaných rozcestníků',
            u'Wikipedie:Seznam rozcestníků/první typ',
            u'Wikipedie:Seznam rozcestníků/druhý typ',
            u'Wikipedista:Zirland/okres',
        ],
        'da': [
            u'Wikipedia:Links til sider med flertydige titler'
        ],
        'de': [
            u'.+/[aA]rchiv.*',
            u'.+/Baustelle.*',
            u'.+/Index',
            u'.+/Spielwiese',
            u'.+/[tT]est.*',
            u'.*Diskussion:.+',
            u'Benutzer:.+/[Ll]og.*',
            u'Benutzer:C.Löser/.+',
            u'Benutzer:Katharina/Begriffsklärungen',
            u'Benutzer:Kirschblut/.+buchstabenkürzel',
            u'Benutzer:Mathias Schindler/.+',
            u'Benutzer:Noisper/Dingliste/[A-Z]',
            u'Benutzer:Professor Einstein.*',
            u'Benutzer:Sebbot/.+',
            u'Benutzer:SirJective/.+',
            u'Benutzer:Srbauer.*',
            u'Benutzer:SteEis.',
            u'Benutzer:Steindy.*',
            u'Benutzer:SrbBot.*',
            u'Benutzer:PortalBot/.+',
            u'Benutzer:Xqbot/.+',
            u'Lehnwort',
            u'Liste griechischer Wortstämme in deutschen Fremdwörtern',
            u'Liste von Gräzismen',
            u'Portal:Abkürzungen/.+',
            u'Portal:Astronomie/Moves',
            u'Portal:Astronomie/Index/.+',
            u'Portal:Hund',
            u'Portal:Hund/Beobachtungsliste',
            u'Portal:Marxismus',
            u'Portal:Täuferbewegung/Seitenindex',
            u'Wikipedia:Administratoren/Anfragen',
            u'Wikipedia:Archiv/.+',
            u'Wikipedia:Artikelwünsche/Ding-Liste/[A-Z]',
            u'Wikipedia:Begriffsklärung.*',
            u'Wikipedia:Bots/.+',
            u'Wikipedia:Interwiki-Konflikte',
            u'Wikipedia:ISBN-Suche',
            u'Wikipedia:Liste mathematischer Themen/BKS',
            u'Wikipedia:Liste mathematischer Themen/Redirects',
            u'Wikipedia:Meinungsbilder/.+',
            u'Wikipedia:Löschkandidaten/.+',
            u'Wikipedia:WikiProjekt Altertumswissenschaft/.+',
            u'Wikipedia:WikiProjekt Verwaiste Seiten/Begriffsklärungen',
            u'Wikipedia:Qualitätssicherung/.+',
            u'Vorlage:Infobox Weltraum',
            u'Vorlage:Navigationsleiste Raumfahrt',
        ],
        'en': [
            u'Wikipedia:Links to disambiguating pages',
            u'Wikipedia:Disambiguation pages with links',
            u'Wikipedia:Multiple-place names \([A-Z]\)',
            u'Wikipedia:Non-unique personal name',
            u"User:Jerzy/Disambiguation Pages i've Editted",
            u'User:Gareth Owen/inprogress',
            u'TLAs from [A-Z][A-Z][A-Z] to [A-Z][A-Z][A-Z]',
            u'List of all two-letter combinations',
            u'User:Daniel Quinlan/redirects.+',
            u'User:Oliver Pereira/stuff',
            u'Wikipedia:French Wikipedia language links',
            u'Wikipedia:Polish language links',
            u'Wikipedia:Undisambiguated abbreviations/.+',
            u'List of acronyms and initialisms',
            u'Wikipedia:Usemod article histories',
            u'User:Pizza Puzzle/stuff',
            u'List of generic names of political parties',
            u'Talk:List of initialisms/marked',
            u'Talk:List of initialisms/sorted',
            u'Talk:Programming language',
            u'Talk:SAMPA/To do',
            u"Wikipedia:Outline of Roget's Thesaurus",
            u'User:Wik/Articles',
            u'User:Egil/Sandbox',
            u'Wikipedia talk:Make only links relevant to the context',
            u'Wikipedia:Common words, searching for which is not possible',
        ],
        'fa': [
            u'ویکی‌پدیا:فهرست صفحات ابهام‌زدایی',
        ],
        'fi': [
            u'Wikipedia:Luettelo täsmennyssivuista',
            u'Wikipedia:Luettelo (täsmennyssivuista)',
            u'Wikipedia:Täsmennyssivu',
        ],
        'fr': [
            u'Wikipédia:Liens aux pages d’homonymie',
            u'Wikipédia:Homonymie',
            u'Wikipédia:Homonymie/Homonymes dynastiques',
            u'Wikipédia:Prise de décision, noms des membres de dynasties/liste des dynastiens',
            u'Liste de toutes les combinaisons de deux lettres',
            u'Wikipédia:Log d’upload/.*',
            u'Sigles de trois lettres de [A-Z]AA à [A-Z]ZZ',
            u'Wikipédia:Pages sans interwiki,.'
        ],
        'fy': [
            u'Wikipedy:Fangnet',
        ],
        'ia': [
            u'Categoria:Disambiguation',
            u'Wikipedia:.+',
            u'Usator:.+',
            u'Discussion Usator:.+',
        ],
        'it': [
            u'Aiuto:Disambigua/Disorfanamento',
            u'Discussioni utente:.+',
            u'Utente:Civvì/disorfanamento',
        ],
        'kk': [
            u'Санат:Айрықты бет',
        ],
        'ko': [
            u'위키백과:(동음이의) 문서의 목록',
            u'위키백과:동음이의어 문서의 목록',
        ],
        'lt': [
            u'Wikipedia:Rodomi nukreipiamieji straipsniai',
        ],
        'nl': [
            u"Gebruiker:.*",
            u"Overleg gebruiker:.+[aA]rchief.*",
            u"Overleg gebruiker:Pven",
            u"Portaal:.+[aA]rchief.*",
            u"Wikipedia:Humor en onzin.*",
            u"Wikipedia:Links naar doorverwijspagina's/Winkeldochters.*",
            u"Wikipedia:Project aanmelding bij startpagina's",
            u"Wikipedia:Wikiproject Roemeense gemeenten/Doorverwijspagina's",
            u'Categorie:Doorverwijspagina',
            u'Lijst van Nederlandse namen van pausen',
            u'Overleg Wikipedia:Discussie spelling 2005',
            u'Overleg Wikipedia:Doorverwijspagina',
            u'Overleg Wikipedia:Logboek.*',
            u'Wikipedia:Logboek.*',
            u'Overleg gebruiker:Sybren/test.*',
            u'Overleg gebruiker:[0-9][0-9]?[0-9]?\.[0-9][0-9]?[0-9]?\.[0-9][0-9]?[0-9]?\.[0-9][0-9]?[0-9]?',
            u'Overleg:Lage Landen (staatkunde)',
            u'Wikipedia:.*[aA]rchief.*',
            u'Wikipedia:Doorverwijspagina',
            u'Wikipedia:Lijst van alle tweeletter-combinaties',
            u'Wikipedia:Onderhoudspagina',
            u'Wikipedia:Ongelijke redirects',
            u'Wikipedia:Protection log',
            u'Wikipedia:Te verwijderen.*',
            u'Wikipedia:Top 1000 van meest bekeken artikelen',
            u'Wikipedia:Wikipedianen met een encyclopedisch artikel',
            u'Wikipedia:Woorden die niet als zoekterm gebruikt kunnen worden',
            u'Overleg gebruiker:Taka(/.*)?',
            u"Wikipedia:Links naar doorverwijspagina's/Artikelen",
         ],
        'pl': [
            u'Wikipedysta:.+',
            u'Dyskusja.+:.+',
         ],
        'pt': [
            u'Usuário:.+',
            u'Usuário Discussão:.+',
            u'Discussão:.+',
            u'Lista de combinações de duas letras',
            u'Wikipedia:Lista de páginas de desambiguação.+',
            u'Wikipedia:Páginas para eliminar/.+',
        ],
        'ru': [
            u'Категория:Disambig',
            u'Википедия:Страницы разрешения неоднозначностей',
            u'Википедия:Вики-уборка/Статьи без языковых ссылок',
            u'Википедия:Страницы с пометкой «(значения)»',
            u'Список общерусских фамилий',
        ],
    },
    'memoryalpha': {
        'en': [
            u'Memory Alpha:Links to disambiguating pages'
        ],
        'de': [
            u'Memory Alpha:Liste der Wortklärungsseiten'
        ],
    },
}

def firstcap(string):
    return string[0].upper()+string[1:]

def correctcap(link, text):
    # If text links to a page with title link uncapitalized, uncapitalize link,
    # otherwise capitalize it
    linkupper = link.title()
    linklower = linkupper[0].lower() + linkupper[1:]
    if "[[%s]]"%linklower in text or "[[%s|"%linklower in text:
        return linklower
    else:
        return linkupper

class ReferringPageGeneratorWithIgnore:
    def __init__(self, disambPage, primary=False, minimum = 0):
        self.disambPage = disambPage
        self.minimum = minimum

    def __iter__(self):
        generator = pagegenerators.ReferringPageGenerator(
                        self.disambPage,
                        followRedirects=False,
                        withTemplateInclusion=False)
        generator = pagegenerators.PageTitleFilterPageGenerator(generator,
                                                                ignore_title)

        refs = [page for page in generator]

        if len(refs) < self.minimum:
            pywikibot.output(u"Found only %d pages to work on; skipping."
                             % len(refs))
            return
        pywikibot.output(u"Will work on %d pages." % len(refs))
        for ref in refs:
            yield ref

class PrimaryIgnoreManager(object):
    '''
    If run with the -primary argument, reads from a file which pages should
    not be worked on; these are the ones where the user pressed n last time.
    If run without the -primary argument, doesn't ignore any pages.
    '''
    def __init__(self, disambPage, enabled = False):
        self.disambPage = disambPage
        self.enabled = enabled

        self.ignorelist = []
        filename = pywikibot.config.datafilepath(
                            'disambiguations',
                            self.disambPage.titleForFilename() + '.txt')
        try:
            # The file is stored in the disambiguation/ subdir.
            # Create if necessary.
            f = codecs.open(filename, 'r', 'utf-8')
            for line in f.readlines():
                # remove trailing newlines and carriage returns
                while line[-1] in ['\n', '\r']:
                    line = line[:-1]
                #skip empty lines
                if line != '':
                    self.ignorelist.append(line)
            f.close()
        except IOError:
            pass

    def isIgnored(self, refPage):
        return self.enabled and refPage.urlname() in self.ignorelist

    def ignore(self, refPage):
        if self.enabled:
            # Skip this occurence next time.
            filename = pywikibot.config.datafilepath(
                                'disambiguations',
                                self.disambPage.urlname() + '.txt')
            try:
                # Open file for appending. If none exists yet, create a new one.
                f = codecs.open(filename, 'a', 'utf-8')
                f.write(refPage.urlname() + '\n')
                f.close()
            except IOError:
                pass


class DisambiguationRobot(object):
    ignore_contents = {
        'de':(u'{{[Ii]nuse}}',
              u'{{[Ll]öschen}}',
            ),
        'fi':(u'{{[Tt]yöstetään}}',
            ),
        'kk':(u'{{[Ii]nuse}}',
              u'{{[Pp]rocessing}}',
            ),
        'nl':(u'{{wiu2}}',
              u'{{nuweg}}',
            ),
        'ru':(u'{{[Ii]nuse}}',
              u'{{[Pp]rocessing}}',
            ),
    }

    primary_redir_template = {
        # Page.templates() format, first letter uppercase
        'hu': u'Egyért-redir',
    }

    def __init__(self, always, alternatives, getAlternatives, dnSkip, generator,
                 primary, main_only, minimum = 0):
        self.always = always
        self.alternatives = alternatives
        self.getAlternatives = getAlternatives
        self.dnSkip = dnSkip
        self.generator = generator
        self.primary = primary
        self.main_only = main_only
        self.minimum = minimum

        self.mysite = pywikibot.getSite()
        self.mylang = self.mysite.language()
        self.comment = None

        self.setupRegexes()

    def checkContents(self, text):
        '''
        For a given text, returns False if none of the regular
        expressions given in the dictionary at the top of this class
        matches a substring of the text.
        Otherwise returns the substring which is matched by one of
        the regular expressions.
        '''
        for ig in self.ignore_contents_regexes:
            match = ig.search(text)
            if match:
                return match.group()
        return None

    def makeAlternativesUnique(self):
        # remove duplicate entries
        result={}
        for i in self.alternatives:
            result[i]=None
        self.alternatives = result.keys()

    def listAlternatives(self):
        list = u'\n'
        for i in range(len(self.alternatives)):
            list += (u"%3i - %s\n" % (i, self.alternatives[i]))
        pywikibot.output(list)

    def setupRegexes(self):
        # compile regular expressions
        self.ignore_contents_regexes = []
        if self.mylang in self.ignore_contents:
            for ig in self.ignore_contents[self.mylang]:
                self.ignore_contents_regexes.append(re.compile(ig))

        linktrail = self.mysite.linktrail()
        self.trailR = re.compile(linktrail)
        # The regular expression which finds links. Results consist of four
        # groups:
        # group title is the target page title, that is, everything before
        # | or ].
        # group section is the page section. It'll include the # to make life
        # easier for us.
        # group label is the alternative link title, that's everything
        # between | and ].
        # group linktrail is the link trail, that's letters after ]] which
        # are part of the word.
        # note that the definition of 'letter' varies from language to language.
        self.linkR = re.compile(r'\[\[(?P<title>[^\]\|#]*)(?P<section>#[^\]\|]*)?(\|(?P<label>[^\]]*))?\]\](?P<linktrail>' + linktrail + ')')

    def treat(self, refPage, disambPage):
        """
        Parameters:
            disambPage - The disambiguation page or redirect we don't want
                anything to link to
            refPage - A page linking to disambPage
        Returns False if the user pressed q to completely quit the program.
        Otherwise, returns True.

        """
        # TODO: break this function up into subroutines!

        dn_template_str = pywikibot.translate(self.mysite, dn_template)
        include = False
        unlink = False
        new_targets = []
        try:
            text=refPage.get(throttle=False)
            ignoreReason = self.checkContents(text)
            if ignoreReason:
                pywikibot.output('\n\nSkipping %s because it contains %s.\n\n'
                                 % (refPage.title(), ignoreReason))
            else:
                include = True
        except pywikibot.IsRedirectPage:
            pywikibot.output(u'%s is a redirect to %s'
                             % (refPage.title(), disambPage.title()))
            if disambPage.isRedirectPage():
                target = self.alternatives[0]
                choice = pywikibot.inputChoice(
                    u'Do you want to make redirect %s point to %s?'
                    % (refPage.title(), target), ['yes', 'no'], ['y', 'N'], 'N')
                if choice == 'y':
                    redir_text = '#%s [[%s]]' \
                                 % (self.mysite.redirect(default=True), target)
                    try:
                        refPage.put_async(redir_text,comment=self.comment)
                    except pywikibot.PageNotSaved, error:
                        pywikibot.output(u'Page not saved: %s' % error.args)
            else:
                choice = pywikibot.inputChoice(
                    u'Do you want to work on pages linking to %s?'
                    % refPage.title(), ['yes', 'no', 'change redirect'],
                                       ['y', 'N', 'c'], 'N')
                if choice == 'y':
                    gen = ReferringPageGeneratorWithIgnore(refPage,
                                                           self.primary)
                    preloadingGen = pagegenerators.PreloadingGenerator(gen)
                    for refPage2 in preloadingGen:
                        # run until the user selected 'quit'
                        if not self.treat(refPage2, refPage):
                            break
                elif choice == 'c':
                    text=refPage.get(throttle=False,get_redirect=True)
                    include = "redirect"
        except pywikibot.NoPage:
            pywikibot.output(
                u'Page [[%s]] does not seem to exist?! Skipping.'
                % refPage.title())
            include = False
        if include in (True, "redirect"):
            # make a backup of the original text so we can show the changes later
            original_text = text
            n = 0
            curpos = 0
            dn = False
            edited = False
            # This loop will run until we have finished the current page
            while True:
                m = self.linkR.search(text, pos = curpos)
                if not m:
                    if n == 0:
                        pywikibot.output(u"No changes necessary in %s"
                                         % refPage.title())
                        return True
                    else:
                        # stop loop and save page
                        break
                # Make sure that next time around we will not find this same hit.
                curpos = m.start() + 1
                # ignore interwiki links and links to sections of the same page
                if m.group('title') == '' or self.mysite.isInterwikiLink(m.group('title')):
                    continue
                else:
                    try:
                        linkPage = pywikibot.Page(disambPage.site(), m.group('title'))
                        # Check whether the link found is to disambPage.
                    except pywikibot.InvalidTitle:
                        continue
                    if linkPage != disambPage:
                        continue

                n += 1
                # how many bytes should be displayed around the current link
                context = 60
                #there's a {{dn}} here already
                already_dn = text[m.end() : m.end() + 8].find(dn_template_str[:4]) > -1
                if already_dn and self.dnSkip:
                    continue

                # This loop will run while the user doesn't choose an option
                # that will actually change the page
                while True:
                    # Show the title of the page where the link was found.
                    # Highlight the title in purple.
                    pywikibot.output(
                        u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                        % refPage.title())

                    if not self.always:
                        # at the beginning of the link, start red color.
                        # at the end of the link, reset the color to default
                        pywikibot.output(text[max(0, m.start() - context)
                                              : m.start()]
                                         + '\03{lightred}'
                                         + text[m.start() : m.end()]
                                         + '\03{default}'
                                         + text[m.end() : m.end() + context])
                        if edited:
                            choice = pywikibot.input(
u"Option (#, r#, [s]kip link, [e]dit page, [n]ext page, [u]nlink, [q]uit,\n"
u"        [t]ag template " + dn_template_str + ",\n"
u"        [m]ore context, [l]ist, [a]dd new, x=save in this form):")
                        else:
                            choice = pywikibot.input(
u"Option (#, r#, [s]kip link, [e]dit page, [n]ext page, [u]nlink, [q]uit,\n"
u"        [t]ag template " + dn_template_str + ",\n"
u"        [m]ore context, show [d]isambiguation page, [l]ist, [a]dd new):")
                    else:
                        choice = self.always
                    if choice in ['a', 'A']:
                        newAlternative = pywikibot.input(u'New alternative:')
                        self.alternatives.append(newAlternative)
                        self.listAlternatives()
                    elif choice in ['e', 'E']:
                        editor = editarticle.TextEditor()
                        newText = editor.edit(text, jumpIndex=m.start(),
                                              highlight=disambPage.title())
                        # if user didn't press Cancel
                        if newText and newText != text:
                            text = newText
                            break
                    elif choice in ['d', 'D']:
                        editor = editarticle.TextEditor()
                        if disambPage.isRedirectPage():
                            disambredir = disambPage.getRedirectTarget()
                            disambigText = editor.edit(
                                               disambredir.get(),
                                               jumpIndex=m.start(),
                                               highlight=disambredir.title())
                        else:
                            disambigText = editor.edit(
                                               disambPage.get(),
                                               jumpIndex=m.start(),
                                               highlight=disambPage.title())
                    elif choice in ['l', 'L']:
                        self.listAlternatives()
                    elif choice in ['m', 'M']:
                        # show more text around the link we're working on
                        context *= 2
                    else:
                        break

                if choice in ['e', 'E']:
                    # user has edited the page and then pressed 'OK'
                    edited = True
                    curpos = 0
                    continue
                elif choice in ['n', 'N']:
                    # skip this page
                    if self.primary:
                        # If run with the -primary argument, skip this occurence next time.
                        self.primaryIgnoreManager.ignore(refPage)
                    return True
                elif choice in ['q', 'Q']:
                    # quit the program
                    return False
                elif choice in ['s', 'S']:
                    # Next link on this page
                    n -= 1
                    continue
                elif choice in ['x', 'X'] and edited:
                    # Save the page as is
                    break

                # The link looks like this:
                # [[page_title|link_text]]trailing_chars
                page_title = m.group('title')
                link_text = m.group('label')

                if not link_text:
                    # or like this: [[page_title]]trailing_chars
                    link_text = page_title
                if m.group('section') == None:
                    section = ''
                else:
                    section = m.group('section')
                trailing_chars = m.group('linktrail')
                if trailing_chars:
                    link_text += trailing_chars
                # '?', '/' for old choice
                if choice in ['t', 'T', '?', '/']:
                    #small chunk of text to search
                    search_text = text[m.end() : m.end() + context]
                    #figure out where the link (and sentance) ends, put note there
                    end_of_word_match = re.search("\s", search_text)
                    if end_of_word_match:
                        position_split = end_of_word_match.start(0)
                    else:
                        position_split = 0
                    #insert dab needed template
                    text = text[:m.end() + position_split] + dn_template_str \
                           + text[m.end() + position_split:]
                    dn = True
                    continue
                elif choice in ['u', 'U']:
                    # unlink - we remove the section if there's any
                    text = text[:m.start()] + link_text + text[m.end():]
                    unlink = True
                    continue
                else:
                    if len(choice)>0 and choice[0] == 'r':
                        # we want to throw away the original link text
                        replaceit = True
                        choice = choice[1:]
                    elif include == "redirect":
                        replaceit = True
                    else:
                        replaceit = False

                    try:
                        choice=int(choice)
                    except ValueError:
                        pywikibot.output(u"Unknown option")
                        # step back to ask the user again what to do with the
                        # current link
                        curpos -= 1
                        continue
                    if choice >= len(self.alternatives) or choice < 0:
                        pywikibot.output(
u"Choice out of range. Please select a number between 0 and %i."
                            % (len(self.alternatives) - 1))
                        # show list of possible choices
                        self.listAlternatives()
                        # step back to ask the user again what to do with the
                        # current link
                        curpos -= 1
                        continue
                    new_page_title = self.alternatives[choice]
                    repPl = pywikibot.Page(disambPage.site(), new_page_title)
                    if (new_page_title[0].isupper()
                        or link_text[0].isupper()):
                        new_page_title = repPl.title()
                    else:
                        new_page_title = repPl.title()
                        new_page_title = new_page_title[0].lower() \
                                         + new_page_title[1:]
                    if new_page_title not in new_targets:
                        new_targets.append(new_page_title)
                    if replaceit and trailing_chars:
                        newlink = "[[%s%s]]%s" % (new_page_title,
                                                  section,
                                                  trailing_chars)
                    elif replaceit or (new_page_title == link_text
                                       and not section):
                        newlink = "[[%s]]" % new_page_title
                    # check if we can create a link with trailing characters
                    # instead of a pipelink
                    elif len(new_page_title) <= len(link_text) \
                         and firstcap(link_text[:len(new_page_title)]) \
                         == firstcap(new_page_title) \
                         and re.sub(self.trailR, '',
                                    link_text[len(new_page_title):]) == '' \
                         and not section:
                        newlink = "[[%s]]%s" \
                                  % (link_text[:len(new_page_title)],
                                     link_text[len(new_page_title):])
                    else:
                        newlink = "[[%s%s|%s]]" \
                                  % (new_page_title, section, link_text)
                    text = text[:m.start()] + newlink + text[m.end():]
                    continue

                pywikibot.output(text[max(0,m.start()-30):m.end()+30])
            if text == original_text:
                pywikibot.output(u'\nNo changes have been made:\n')
            else:
                pywikibot.output(u'\nThe following changes have been made:\n')
                pywikibot.showDiff(original_text, text)
                pywikibot.output(u'')
                # save the page
                self.setSummaryMessage(disambPage, new_targets, unlink, dn)
                try:
                    refPage.put_async(text,comment=self.comment)
                except pywikibot.LockedPage:
                    pywikibot.output(u'Page not saved: page is locked')
                except pywikibot.PageNotSaved, error:
                    pywikibot.output(u'Page not saved: %s' % error.args)
        return True

    def findAlternatives(self, disambPage):
        if disambPage.isRedirectPage() and not self.primary:
            if (disambPage.site().lang in self.primary_redir_template
                and self.primary_redir_template[disambPage.site().lang]
                    in disambPage.templates(get_redirect = True)):
                baseTerm = disambPage.title()
                for template in disambPage.templatesWithParams(
                    get_redirect=True):
                    if template[0] == self.primary_redir_template[
                        disambPage.site().lang] \
                        and len(template[1]) > 0:
                        baseTerm = template[1][1]
                disambTitle = primary_topic_format[self.mylang] % baseTerm
                try:
                    disambPage2 = pywikibot.Page(
                        self.mysite, disambTitle)
                    links = disambPage2.linkedPages()
                    links = [correctcap(l,disambPage2.get()) for l in links]
                except pywikibot.NoPage:
                    pywikibot.output(u"No page at %s, using redirect target."
                                     % disambTitle)
                    links = disambPage.linkedPages()[:1]
                    links = [correctcap(l, disambPage.get(get_redirect=True))
                             for l in links]
                self.alternatives += links
            else:
                try:
                    target = disambPage.getRedirectTarget().title()
                    self.alternatives.append(target)
                except pywikibot.NoPage:
                    pywikibot.output(u"The specified page was not found.")
                    user_input = pywikibot.input(u"""\
Please enter the name of the page where the redirect should have pointed at,
or press enter to quit:""")
                    if user_input == "":
                        sys.exit(1)
                    else:
                        self.alternatives.append(user_input)
                except pywikibot.IsNotRedirectPage:
                    pywikibot.output(
                        u"The specified page is not a redirect. Skipping.")
                    return False
        elif self.getAlternatives:
            try:
                if self.primary:
                    try:
                        disambPage2 = pywikibot.Page(
                            self.mysite,
                            primary_topic_format[self.mylang]
                            % disambPage.title())
                        links = disambPage2.linkedPages()
                        links = [correctcap(l, disambPage2.get())
                                 for l in links]
                    except pywikibot.NoPage:
                        pywikibot.output(
u"Page does not exist, using the first link in page %s."
                            % disambPage.title())
                        links = disambPage.linkedPages()[:1]
                        links = [correctcap(l, disambPage.get())
                                 for l in links]
                else:
                    try:
                        links = disambPage.linkedPages()
                        links = [correctcap(l, disambPage.get())
                                 for l in links]
                    except pywikibot.NoPage:
                        pywikibot.output(u"Page does not exist, skipping.")
                        return False
            except pywikibot.IsRedirectPage:
                pywikibot.output(u"Page is a redirect, skipping.")
                return False
            self.alternatives += links
        return True

    def setSummaryMessage(self, disambPage, new_targets=[], unlink=False,
                          dn=False):
        # make list of new targets
        targets = ''
        for page_title in new_targets:
            targets += u'[[%s]], ' % page_title
        # remove last comma
        targets = targets[:-2]

        if not targets:
            targets = pywikibot.translate(self.mysite, unknown_msg)

        # first check whether user has customized the edit comment
        if (self.mysite.family.name in pywikibot.config.disambiguation_comment
            and self.mylang in pywikibot.config.disambiguation_comment
                                         [self.mysite.family.name]):
            try:
                self.comment = pywikibot.translate(
                                   self.mysite,
                                   pywikibot.config.disambiguation_comment[
                                       self.mysite.family.name]
                                   ) % (disambPage.title(), targets)
            # Backwards compatibility, type error probably caused by too
            # many arguments for format string
            except TypeError:
                self.comment = pywikibot.translate(
                                   self.mysite,
                                   pywikibot.config.disambiguation_comment[
                                       self.mysite.family.name]
                                   ) % disambPage.title()
        elif disambPage.isRedirectPage():
            # when working on redirects, there's another summary message
            if unlink and not new_targets:
                self.comment = pywikibot.translate(self.mysite,
                                                   msg_redir_unlink) \
                               % disambPage.title()
            elif dn and not new_targets:
                self.comment = pywikibot.translate(self.mysite, msg_redir_dn) \
                               % disambPage.title()
            else:
                self.comment = pywikibot.translate(self.mysite, msg_redir) \
                               % (disambPage.title(), targets)
        else:
            if unlink and not new_targets:
                self.comment = pywikibot.translate(self.mysite, msg_unlink) \
                               % disambPage.title()
            elif dn and not new_targets:
                self.comment = pywikibot.translate(self.mysite, msg_dn) \
                               % disambPage.title()
            else:
                self.comment = pywikibot.translate(self.mysite, msg) \
                               % (disambPage.title(), targets)

    def run(self):
        if self.main_only:
            if self.mysite.family.name not in ignore_title:
                ignore_title[self.mysite.family.name] = {}
            if self.mylang not in ignore_title[self.mysite.family.name]:
                ignore_title[self.mysite.family.name][self.mylang] = []
            ignore_title[self.mysite.family.name][self.mylang] += [
                u'%s:' % namespace for namespace in self.mysite.namespaces()]

        for disambPage in self.generator:
            self.primaryIgnoreManager = PrimaryIgnoreManager(
                                            disambPage, enabled=self.primary)

            if not self.findAlternatives(disambPage):
                continue

            self.makeAlternativesUnique()
            # sort possible choices
            if pywikibot.config.sort_ignore_case:
                self.alternatives.sort(lambda x,y: cmp(x.lower(), y.lower()))
            else:
                self.alternatives.sort()
            self.listAlternatives()

            gen = ReferringPageGeneratorWithIgnore(disambPage, self.primary,
                                                   minimum = self.minimum)
            preloadingGen = pagegenerators.PreloadingGenerator(gen)
            for refPage in preloadingGen:
                if not self.primaryIgnoreManager.isIgnored(refPage):
                    # run until the user selected 'quit'
                    if not self.treat(refPage, disambPage):
                        break

            # clear alternatives before working on next disambiguation page
            self.alternatives = []

def main(*args):
    # the option that's always selected when the bot wonders what to do with
    # a link. If it's None, the user is prompted (default behaviour).
    always = None
    alternatives = []
    getAlternatives = True
    dnSkip = False
    # if the -file argument is used, page titles are dumped in this array.
    # otherwise it will only contain one page.
    generator = None
    # This temporary array is used to read the page title if one single
    # page to work on is specified by the arguments.
    pageTitle = []
    primary = False
    main_only = False

    # For sorting the linked pages, case can be ignored
    ignoreCase = False
    minimum = 0

    for arg in pywikibot.handleArgs(*args):
        if arg.startswith('-primary:'):
            primary = True
            getAlternatives = False
            alternatives.append(arg[9:])
        elif arg == '-primary':
            primary = True
        elif arg.startswith('-always:'):
            always = arg[8:]
        elif arg.startswith('-file'):
            if len(arg) == 5:
                generator = pagegenerators.TextfilePageGenerator(
                                filename=None)
            else:
                generator = pagegenerators.TextfilePageGenerator(
                                filename=arg[6:])
        elif arg.startswith('-pos:'):
            if arg[5]!=':':
                mysite = pywikibot.getSite()
                page = pywikibot.Page(mysite, arg[5:])
                if page.exists():
                    alternatives.append(page.title())
                else:
                    answer = pywikibot.inputChoice(
                    u'Possibility %s does not actually exist. Use it anyway?'
                             % page.title(), ['yes', 'no'], ['y', 'N'], 'N')
                    if answer == 'y':
                        alternatives.append(page.title())
            else:
                alternatives.append(arg[5:])
        elif arg == '-just':
            getAlternatives = False
        elif arg == '-dnskip':
            dnSkip = True
        elif arg == '-main':
            main_only = True
        elif arg.startswith('-min:'):
            minimum = int(arg[5:])
        elif arg.startswith('-start'):
            try:
                if len(arg) <= len('-start:'):
                    generator = pagegenerators.CategorizedPageGenerator(
                                    pywikibot.getSite().disambcategory())
                else:
                    generator = pagegenerators.CategorizedPageGenerator(
                                    pywikibot.getSite().disambcategory(),
                                    start=arg[7:])
                generator = pagegenerators.NamespaceFilterPageGenerator(
                                generator, [0])
            except pywikibot.NoPage:
                print "Disambiguation category for your wiki is not known."
                raise
        elif arg.startswith("-"):
            print "Unrecognized command line argument: %s" % arg
            # show help text and exit
            pywikibot.showHelp()
        else:
            pageTitle.append(arg)

    # if the disambiguation page is given as a command line argument,
    # connect the title's parts with spaces
    if pageTitle != []:
        pageTitle = ' '.join(pageTitle)
        page = pywikibot.Page(pywikibot.getSite(), pageTitle)
        generator = iter([page])

    # if no disambiguation page was given as an argument, and none was
    # read from a file, query the user
    if not generator:
        pageTitle = pywikibot.input(
            u'On which disambiguation page do you want to work?')
        page = pywikibot.Page(pywikibot.getSite(), pageTitle)
        generator = iter([page])

    bot = DisambiguationRobot(always, alternatives, getAlternatives, dnSkip,
                              generator, primary, main_only,
                              minimum=minimum)
    bot.run()


if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
