# -*- coding: utf-8 -*-
"""
Script to replace bad Windows-1252 (cp1252) characters with
HTML entities on ISO 8859-1 wikis. Don't run this script on a UTF-8 wiki.

Syntax: python windows_chars.py [pageTitle] [file[:filename]] [sql[:filename]]

Command line options:

   -file:XYZ  reads a list of pages, which can for exampagee be gotten through
              Looxix's robot. XYZ is the name of the file from which the
              list is taken. If XYZ is not given, the user is asked for a
              filename.
              Page titles should be in [[double-square brackets]].

   -sql:XYZ   reads a local SQL cur dump, available at
              http://download.wikimedia.org/. Searches for pages with
              Windows-1252 characters, and tries to repair them on the live
              wiki. Example:
              python windows_chars.py -sql:20040711_cur_table.sql.sql -lang:es

"""
#
# (C) Daniel Herding, 2004
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: windows_chars.py,v 1.27 2005/12/21 17:51:26 wikipedian Exp $'
#
import wikipedia, config
import replace, pagegenerators
import re, sys

# Summary message
msg={
    'en':u'robot: changing Windows-1252 characters to HTML entities',
    'fa':u'ربات: تغییر نویسه‌های Windows-1252 به نهادهای اچ‌تی‌ام‌ال',
    'de':u'Bot: Wandle Windows-1252-Zeichen in HTML-Entitäten um',
    'fr':u'Bot: Modifie caracteres Windows-1252 vers entités HTML',
    'he':u'רובוט: משנה תווים בקידוד Windows-1252 ליישויות HTML',
    'ia':u'Robot: modification de characteres Windows-1252 a entitates HTML',
    }

# characters that are in Windows-1252), but not in ISO 8859-1
replacements = [
    (u"\x80", u"&euro;"),   # euro sign
    (u"\x82", u"&sbquo;"),   # single low-9 quotation mark
    (u"\x83", u"&fnof;"),   # latin small f with hook = function = florin
    (u"\x84", u"&bdquo;"),  # double low-9 quotation mark
    (u"\x85", u"&hellip;"), # horizontal ellipsis = three dot leader
    (u"\x86", u"&dagger;"), # dagger
    (u"\x87", u"&Dagger;"), # double dagger
    (u"\x88", u"&circ;"),   # modifier letter circumflex accent
    (u"\x89", u"&permil;"), # per mille sign
    (u"\x8A", u"&Scaron;"), # latin capital letter S with caron
    (u"\x8B", u"&#8249;"),  # single left-pointing angle quotation mark
    (u"\x8C", u"&OElig;"),  # latin capital ligature OE
    (u"\x8E", u"&#381;"),   # latin capital letter Z with caron
    (u"\x91", u"&lsquo;"),  # left single quotation mark
    (u"\x92", u"&rsquo;"),  # right single quotation mark
    (u"\x93", u"&ldquo;"),  # left double quotation mark
    (u"\x94", u"&rdquo;"),  # right double quotation mark
    (u"\x95", u"&bull;"),   # bullet = black small circle
    (u"\x96", u"&ndash;"),  # en dash
    (u"\x97", u"&mdash;"),  # em dash
    (u"\x98", u"&tilde;"),  # small tilde
    (u"\x99", u"&trade;"),  # trade mark sign
    (u"\x9A", u"&scaron;"), # latin small letter s with caron
    (u"\x9B", u"&8250;"),   # single right-pointing angle quotation mark
    (u"\x9C", u"&oelig;"),  # latin small ligature oe
    (u"\x9E", u"&#382;"),   # latin small letter z with caron
    (u"\x9F", u"&Yuml;")    # latin capital letter Y with diaeresis
]

class SqlWindows1252PageGenerator:
    """
    opens a local SQL dump file, searches for pages with Windows-1252
    characters.
    """
    def __init__(self, filename):
        self.filename = filename

    def __iter__(self):
        # open SQL dump and read page titles out of it
        import sqldump
        sqldump = sqldump.SQLdump(self.filename, 'latin-1')
        for entry in sqldump.entries():
            for char in replacements.keys():
                if entry.text.find(char) != -1:
                    page = wikipedia.Page(wikipedia.getSite(), entry.full_title())
                    yield page
                    break

class WindowsCharsBot:
    def __init__(self, generator):
        self.generator = generator

    def run(self):
        replaceBot = replace.ReplaceRobot(self.generator, replacements)
        replaceBot.run()

def main():
    # this temporary array is used to read the page title.
    pageTitle = []
    gen = None

    for arg in sys.argv[1:]:
        arg = wikipedia.argHandler(arg, 'windows_chars')
        if arg:
            if arg.startswith('-file'):
                if len(arg) == 5:
                    filename = wikipedia.input(u'please enter the list\'s filename: ')
                else:
                    filename = arg[6:]
                gen = pagegenerators.TextfilePageGenerator(filename)
            elif arg.startswith('-sql'):
                if len(arg) == 4:
                    sqlfilename = wikipedia.input(u'please enter the SQL dump\'s filename: ')
                else:
                    sqlfilename = arg[5:]
                gen = SqlWindows1252PageGenerator(sqlfilename)
            else:
                pageTitle.append(arg)

    # if a single page is given as a command line argument,
    # reconnect the title's parts with spaces
    if pageTitle != []:
        page = wikipedia.Page(wikipedia.getSite(), ' '.join(pageTitle))
        gen = iter([page])

    # get edit summary message
    wikipedia.setAction(wikipedia.translate(wikipedia.getSite(), msg))

    if not gen:
        wikipedia.showHelp('windows_chars')
    elif wikipedia.getSite().encoding() == "utf-8":
        print "There is no need to run this robot on UTF-8 wikis."
    else:
        preloadingGen = pagegenerators.PreloadingGenerator(gen)
        bot = WindowsCharsBot(preloadingGen)
        bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()
