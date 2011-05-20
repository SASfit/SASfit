#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This bot appends some text to all unused images and other text to the
respective uploaders.

Parameters:

-always     Don't be asked every time.

"""

#
# (C) Leonardo Gregianin, 2007
# (C) Filnik, 2008
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: unusedfiles.py 8540 2010-09-12 18:09:11Z amir $'
#

import wikipedia as pywikibot
import pagegenerators
import sys

#***** SETTINGS *******#
#                      #
#   - EDIT BELOW -     #
#                      #
#**********************#

comment = {
    'ar': u'صور للاستبعاد',
    'en': u'images for elimination',
    'fa': u'تصویر استفاده نشده',
    'he': u'תמונות להסרה',
    'it': u'Bot: segnalo immagine orfana da eliminare',
    'pt': u'Bot: marcação de imagens para eliminação',
    }

template_to_the_image = {
    'en': u'{{subst:No-use2}}',
    'it': u'{{immagine orfana}}',
    'fa': u'{{تصاویر بدون استفاده}}',
    }
template_to_the_user = {
    'en': u'\n\n{{img-sem-uso|%s}}',
    'fa': u'\n\n{{اخطار به کاربر برای تصاویر بدون استفاده|%s}}--~~~~',
    'it': u'\n\n{{Utente:Filbot/Immagine orfana}}',
    }
except_text = {
    'en': u'<table id="mw_metadata" class="mw_metadata">',
    'it': u'<table id="mw_metadata" class="mw_metadata">',
    }

#***** SETTINGS *******#
#                      #
#   - EDIT ABOVE -     #
#                      #
#**********************#

def appendtext(page, apptext):
    global always
    try:
        text = page.get()
    except pywikibot.IsRedirectPage:
        return
    # Here you can go editing. If you find you do not
    # want to edit this page, just return
    text += apptext;
    if text != page.get():
        pywikibot.showDiff(page.get(),text)
        if not always:
            choice = pywikibot.inputChoice(
                u'Do you want to accept these changes?', ['Yes', 'No', 'All'],
                ['y', 'N', 'a'], 'N')
            if choice == 'a':
                always = True
        if always or choice == 'y':
            page.put(text, pywikibot.translate(pywikibot.getSite(), comment))

def main():
    global always
    always = False

    for arg in pywikibot.handleArgs():
        if arg == '-always':
            always = True

    mysite = pywikibot.getSite()
    # If anything needs to be prepared, you can do it here
    template_image = pywikibot.translate(pywikibot.getSite(),
                                         template_to_the_image)
    template_user = pywikibot.translate(pywikibot.getSite(),
                                        template_to_the_user)
    except_text_translated = pywikibot.translate(pywikibot.getSite(),
                                                 except_text)
    basicgenerator = pagegenerators.UnusedFilesGenerator()
    generator = pagegenerators.PreloadingGenerator(basicgenerator)
    for page in generator:
        pywikibot.output(u"\n\n>>> \03{lightpurple}%s\03{default} <<<"
                         % page.title())
        if except_text_translated not in page.getImagePageHtml() and \
           'http://' not in page.get():
            pywikibot.output(u'\n' + page.title())
            if template_image in page.get():
                pywikibot.output(u"%s done already" % page.aslink())
                continue
            appendtext(page, u"\n\n"+template_image)
            uploader = page.getFileVersionHistory().pop()[1]
            usertalkname = u'User Talk:%s' % uploader
            usertalkpage = pywikibot.Page(mysite, usertalkname)
            msg2uploader = template_user % page.title()
            appendtext(usertalkpage, msg2uploader)

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
