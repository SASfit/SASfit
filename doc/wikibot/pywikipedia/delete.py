# -*- coding: utf-8 -*-
"""
This script can be used to delete and undelete pages en masse.
Of course, you will need an admin account on the relevant wiki.

Syntax: python delete.py [-category categoryName]

Command line options:

-page:       Delete specified page
-cat:        Delete all pages in the given category.
-nosubcats:  Don't delete pages in the subcategories.
-links:      Delete all pages linked from a given page.
-file:       Delete all pages listed in a text file.
-ref:        Delete all pages referring from a given page.
-images:     Delete all images used on a given page.
-always:     Don't prompt to delete pages, just do it.
-summary:    Supply a custom edit summary.
-undelete:   Actually undelete pages instead of deleting.
             Obviously makes sense only with -page and -file.

Examples:

Delete everything in the category "To delete" without prompting.

    python delete.py -cat:"To delete" -always
"""
__version__ = '$Id: delete.py 9213 2011-05-04 04:40:11Z xqt $'
#
# Distributed under the terms of the MIT license.
#
import wikipedia as pywikibot
import config, catlib
import pagegenerators

# Summary messages for deleting from a category.
msg_simple_delete = {
    'ar': u'بوت: حذف قائمة من الملفات.',
    'cs': u'Robot smazal podle seznamu',
    'en': u'Bot: Deleting a list of files.',
    'fa': u'ربات:حذف فهرستی از صفحه‌ها',
    'fr': u'Robot: Suppression d’une liste de fichiers.',
    'he': u'בוט: מוחק רשימת דפים מתוך קובץ.',
    'nl': u'Bot: verwijdert een lijst met pagina\'s.',
    'pl': u'Robot usuwa pliki z listy.',
    'pt': u'Bot: Apagando um lista de arquivos.',
    'ru': u'Бот: Удаление списка файлов.',
    'sv': u'Bot: Tar bort fillista.',
    'uk': u'Бот: Видалення списку файлів.',
}
msg_delete_category = {
    'ar': u'روبوت - حذف كل الصفحات من التصنيف %s',
    'cs': u'Robot smazal obsah kategorie %s',
    'de': u'Bot: Lösche alle Seiten in Kategorie %s',
    'en': u'Bot: Deleting all pages from category %s',
    'fa': u'ربات:حذف تمام صفحه‌های رده %s',
    'fr': u'Robot - Suppresion de toutes les pages de la actégorie %s',
    'he': u'בוט: מוחק את כל הדפים מהקטגוריה %s.',
    'fr': u'Bot: Supprime toutes pages de la catégorie %s',
    'lt': u'robotas: Trinami visi puslapiai iš kategorijos %s',
    'nl': u'Bot: verwijdert alle pagina\'s uit categorie %s',
    'pl': u'Robot usuwa wszystkie artykuły z kategorii %s',
    'pt': u'Bot: Apagando todas as páginas da categoria %s',
    'ru': u'Бот: Удаление всех страниц из категории %s',
    'sv': u'Bot: Tar bort alla sidor i kategori %s',
    'uk': u'Бот: Видалення усіх сторінок з категорії %s',
}
msg_delete_links = {
    'ar': u'روبوت - حذف كل الصفحات الموصولة من %s',
    'cs': u'Robot smazal vše odkazované z %s',
    'de': u'Bot: Lösche alle Seiten in %s verlinkten Seiten',
    'en': u'Bot: Deleting all pages linked from %s',
    'fa': u'ربات: حذف تمام صفحه‌هایی که در %s پیوند شده‌اند',
    'fr': u'Robot : Supprime toutes les pages liées depuis %s',
    'he': u'בוט: מוחק את כל הדפים המקושרים מהדף %s.',
    'lt': u'robotas: Trinami visi puslapiai į kuriuos yra nuoroda iš %s',
    'nl': u'Bot: verwijdert alle pagina\'s met een verwijzing naar %s',
    'pl': u'Robot usuwa wszystkie artykuły zlinkowane z %s',
    'pt': u'Bot: Apagando todas as páginas ligadas a %s',
    'ru': u'Бот: Удаление всех страниц связанных с %s',
    'sv': u'Bot: Tar bort alla sidor länkade från %s',
    'uk': u'Бот: Видалення усіх сторінок, пов\'язаних із %s',
}
msg_delete_ref = {
    'ar': u'روبوت - حذف كل الصفحات الراجعة من %s',
    'cs': u'Robot smazal vše odkazující na %s',
    'de': u'Bot: Lösche alle auf %s linkenden Seiten',
    'en': u'Bot: Deleting all pages referring from %s',
    'fa': u'ربات:حذف تمام صفحه‌هایی که به %s پیوند داده‌اند',
    'fr': u'Robot : Supprime toutes les pages référant à %s',
    'he': u'בוט: מוחק את כל הדפים המקשרים לדף %s.',
    'lt': u'robotas: Trinami visi puslapiai rodantys į %s',
    'nl': u'Bot: verwijdert alle pagina\'s met referentie van %s',
    'pl': u'Robot usuwa wszystkie artykuły odnoszące się do %s',
    'pt': u'Bot: Apagando todas as páginas afluentes a %s',
    'ru': u'Бот: Удаление всех страниц, относящихся к %s',
    'uk': u'Бот: Видалення усіх сторінок, що відносяться до %s',
}
msg_delete_images = {
    'ar': u'روبوت -حذف كل الصور في الصفحة %s',
    'cs': u'Robot smazal všechny obrázky z %s',
    'en': u'Bot: Deleting all images on page %s',
    'fa': u'ربات: حذف تمام تصویرهای به کار رفته در صفحه %s',
    'fr': u'Robot : Supprime tous les fichiers sur la page %s',
    'he': u'בוט: מוחק את כל התמונות בדף %s.',
    'nl': u'Bot: verwijdert alle media op pagina %s',
    'pl': u'Robot usuwa wszystkie obrazy w artykule %s',
    'pt': u'Bot: Apagando todas as imagens da página %s',
    'ru': u'Бот: Удаление всех изображений на странице %s',
    'sv': u'Bot: Tar bort alla bilder på sida %s',
    'uk': u'Бот: Видалення усіх зображень із сторінки %s',
}


class DeletionRobot:
    """ This robot allows deletion of pages en masse. """

    def __init__(self, generator, summary, always = False, undelete=True):
        """ Arguments:
        * generator - A page generator.
        * always - Delete without prompting?

        """
        self.generator = generator
        self.summary = summary
        self.always = always
        self.undelete = undelete

    def run(self):
        """ Starts the robot's action. """
        #Loop through everything in the page generator and delete it.
        for page in self.generator:
            pywikibot.output(u'Processing page %s' % page.title())
            if self.undelete:
                page.undelete(self.summary, throttle = True)
            else:
                page.delete(self.summary, not self.always, throttle = True)

def main():
    pageName = ''
    singlePage = ''
    summary = ''
    always = False
    doSinglePage = False
    doCategory = False
    deleteSubcategories = True
    doRef = False
    doLinks = False
    doImages = False
    undelete = False
    fileName = ''
    gen = None

    # read command line parameters
    for arg in pywikibot.handleArgs():
        if arg == '-always':
            always = True
        elif arg.startswith('-file'):
            if len(arg) == len('-file'):
                fileName = pywikibot.input(
                    u'Enter name of file to delete pages from:')
            else:
                fileName = arg[len('-file:'):]
        elif arg.startswith('-summary'):
            if len(arg) == len('-summary'):
                summary = pywikibot.input(u'Enter a reason for the deletion:')
            else:
                summary = arg[len('-summary:'):]
        elif arg.startswith('-cat'):
            doCategory = True
            if len(arg) == len('-cat'):
                pageName = pywikibot.input(
                    u'Enter the category to delete from:')
            else:
                pageName = arg[len('-cat:'):]
        elif arg.startswith('-nosubcats'):
            deleteSubcategories = False
        elif arg.startswith('-links'):
            doLinks = True
            if len(arg) == len('-links'):
                pageName = pywikibot.input(u'Enter the page to delete from:')
            else:
                pageName = arg[len('-links:'):]
        elif arg.startswith('-ref'):
            doRef = True
            if len(arg) == len('-ref'):
                pageName = pywikibot.input(u'Enter the page to delete from:')
            else:
                pageName = arg[len('-ref:'):]
        elif arg.startswith('-page'):
            doSinglePage = True
            if len(arg) == len('-page'):
                pageName = pywikibot.input(u'Enter the page to delete:')
            else:
                pageName = arg[len('-page:'):]
        elif arg.startswith('-images'):
            doImages = True
            if len(arg) == len('-images'):
                pageName = pywikibot.input(
                    u'Enter the page with the images to delete:')
            else:
                pageName = arg[len('-images'):]
        elif arg.startswith('-undelete'):
            undelete = True

    mysite = pywikibot.getSite()
    if doSinglePage:
        if not summary:
            summary = pywikibot.input(u'Enter a reason for the %sdeletion:'
                                      % ['', 'un'][undelete])
        page = pywikibot.Page(mysite, pageName)
        gen = iter([page])
    elif doCategory:
        if not summary:
            summary = pywikibot.translate(mysite, msg_delete_category) \
                      % pageName
        ns = mysite.category_namespace()
        categoryPage = catlib.Category(mysite, ns + ':' + pageName)
        gen = pagegenerators.CategorizedPageGenerator(
            categoryPage, recurse=deleteSubcategories)
    elif doLinks:
        if not summary:
            summary = pywikibot.translate(mysite, msg_delete_links) % pageName
        pywikibot.setAction(summary)
        linksPage = pywikibot.Page(mysite, pageName)
        gen = pagegenerators.LinkedPageGenerator(linksPage)
    elif doRef:
        if not summary:
            summary = pywikibot.translate(mysite, msg_delete_ref) % pageName
        refPage = pywikibot.Page(mysite, pageName)
        gen = pagegenerators.ReferringPageGenerator(refPage)
    elif fileName:
        if not summary:
            summary = pywikibot.translate(mysite, msg_simple_delete)
        gen = pagegenerators.TextfilePageGenerator(fileName)
    elif doImages:
        if not summary:
            summary = pywikibot.translate(mysite, msg_delete_images)
        gen = pagegenerators.ImagesPageGenerator(pywikibot.Page(mysite,
                                                                pageName))

    if gen:
        pywikibot.setAction(summary)
        # We are just deleting pages, so we have no need of using a preloading page generator
        # to actually get the text of those pages.
        bot = DeletionRobot(gen, summary, always, undelete)
        bot.run()
    else:
        pywikibot.showHelp(u'delete')

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
