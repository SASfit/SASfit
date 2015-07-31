# -*- coding: utf-8 -*-
"""
Script to copy files from a local Wikimedia wiki to Wikimedia Commons
using CommonsHelper to not leave any information out and CommonSense
to automatically categorise the file. After copying, a NowCommons
template is added to the local wiki's file. It uses a local exclusion
list to skip files with templates not allow on Wikimedia Commons. If no
categories have been found, the file will be tagged on Commons.

This bot uses a graphical interface and may not work from commandline
only environment.

Requests for improvement for CommonsHelper output should be directed to
Magnus Manske at his talk page. Please be very specific in your request
(describe current output and expected output) and note an example file,
so he can test at: [[de:Benutzer Diskussion:Magnus Manske]]. You can
write him in German and English.

Examples

Work on a single image
 python imagecopy.py -page:Image:<imagename>
Work on the 100 newest images:
 python imagecopy.py -newimages:100
Work on all images in a category:<cat>
 python imagecopy.py -cat:<cat>
Work on all images which transclude a template
 python imagecopy.py -transcludes:<template>

See pagegenerators.py for more ways to get a list of images.
By default the bot works on your home wiki (set in user-config)

Known issues/FIXMEs (no critical issues known):
* make it use pagegenerators.py
** Implemented in rewrite
* Some variable names are in Spanish, which makes the code harder to read.
** Almost all variables are now in English
* Depending on sorting within a file category, the "next batch" is sometimes
  not working, leading to an endless loop
** Using pagegenerators now
* Different wikis can have different exclusion lists. A parameter for the
  exclusion list Uploadbot.localskips.txt would probably be nice.
* Bot should probably use API instead of query.php
** Api? Query? Wikipedia.py!
* Should request alternative name if file name already exists on Commons
** Implemented in rewrite
* Exits after last file in category was processed, aborting all pending
  threads.
** Implemented proper threading in rewrite
* Should take user-config.py as input for project and lang variables
** Implemented in rewrite
* Should require a Commons user to be present in user-config.py before
  working
* Should probably have an input field for additional categories
* Should probably have an option to change uploadtext with file
* required i18n options for NowCommons template (f.e. {{subst:ncd}} on
  en.wp. Currently needs customisation to work properly. Bot was tested
  succesfully on nl.wp (12k+ files copied and deleted locally) and en.wp
  (about 100 files copied and SieBot has bot approval for tagging {{ncd}}
  with this bot)
** Implemented
* {{NowCommons|xxx}} requires the namespace prefix Image: on most wikis
  and can be left out on others. This needs to be taken care of when
  implementing i18n
** Implemented
* This bot should probably get a small tutorial at meta with a few
  screenshots.
"""
#
# Based on upload.py by:
# (C) Rob W.W. Hooft, Andre Engels 2003-2007
# (C) Wikipedian, Keichwa, Leogregianin, Rikwade, Misza13 2003-2007
#
# New bot by:
# (C) Kyle/Orgullomoore, Siebrand Mazeland 2007-2008
#
# Another rewrite by:
#  (C) Multichill 2008-2010
#
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: imagecopy.py 9042 2011-03-13 10:14:47Z xqt $'
#

from Tkinter import *
import os, sys, re, codecs
import urllib, httplib, urllib2
import webbrowser
import time, threading
import wikipedia as pywikibot
import config, socket
import pagegenerators, add_text
from upload import *
from image import *
NL=''

nowCommonsTemplate = {
    '_default': u'{{NowCommons|%s}}',
    'af': u'{{NowCommons|File:%s}}',
    'als': u'{{NowCommons|%s}}',
    'am': u'{{NowCommons|File:%s}}',
    'ang': u'{{NowCommons|File:%s}}',
    'ar': u'{{الآن كومنز|%s}}',
    'ast': u'{{EnCommons|File:%s}}',
    'az': u'{{NowCommons|%s}}',
    'bar': u'{{NowCommons|%s}}',
    'bg': u'{{NowCommons|%s}}',
    'bn': u'{{NowCommons|File:%s}}',
    'bs': u'{{NowCommons|%s}}',
    'ca': u'{{AraCommons|%s}}',
    'cs': u'{{NowCommons|%s}}',
    'cy': u'{{NowCommons|File:%s}}',
    'da': u'{{NowCommons|File:%s}}',
    'de': u'{{NowCommons|%s}}',
    'dsb': u'{{NowCommons|%s}}',
    'el': u'{{NowCommons|%s}}',
    'en': u'{{subst:ncd|%s}}',
    'eo': u'{{Nun en komunejo|%s}}',
    'es': u'{{EnCommons|File:%s}}',
    'et': u'{{NüüdCommonsis|File:%s}}',
    'fa': u'{{NowCommons|%s}}',
    'fi': u'{{NowCommons|%s}}',
    'fo': u'{{NowCommons|File:%s}}',
    'fr': u'{{Image sur Commons|%s}}',
    'fy': u'{{NowCommons|%s}}',
    'ga': u'{{Ag Cómhaoin|File:%s}}',
    'gl': u'{{EnCommons]|File:%s}}',
    'gv': u'{{NowCommons|File:%s}}',
    'he': u'{{גם בוויקישיתוף|%s}}',
    'hr': u'{{NowCommons|%s}}',
    'hsb': u'{{NowCommons|%s}}',
    'hu': u'{{Azonnali-commons|%s}}',
    'ia': u'{{NowCommons|File:%s}}',
    'id': u'{{NowCommons|File:%s}}',
    'ilo': u'{{NowCommons|File:%s}}',
    'io': u'{{NowCommons|%s}}',
    'is': u'{{NowCommons|%s}}',
    'it': u'{{NowCommons|%s}}',
    'ja': u'{{NowCommons|File:%s}}',
    'jv': u'{{NowCommons|File:%s}}',
    'ka': u'{{NowCommons|File:%s}}',
    'kn': u'{{NowCommons|File:%s}}',
    'ko': u'{{NowCommons|File:%s}}',
    'ku': u'{{NowCommons|%s}}',
    'lb': u'{{Elo op Commons|%s}}',
    'li': u'{{NowCommons|%s}}',
    'lt': u'{{NowCommons|File:%s}}',
    'lv': u'{{NowCommons|File:%s}}',
    'mk': u'{{NowCommons|File:%s}}',
    'mn': u'{{NowCommons|File:%s}}',
    'ms': u'{{NowCommons|%s}}',
    'nds-nl': u'{{NoenCommons|File:%s}}',
    'nl': u'{{NuCommons|%s}}',
    'nn': u'{{No på Commons|File:%s}}',
    'no': u'{{NowCommons|%s}}',
    'oc': u'{{NowCommons|File:%s}}',
    'pl': u'{{NowCommons|%s}}',
    'pt': u'{{NowCommons|%s}}',
    'ro': u'{{AcumCommons|File:%s}}',
    'ru': u'{{Перенесено на Викисклад|%s}}',
    'sa': u'{{NowCommons|File:%s}}',
    'scn': u'{{NowCommons|File:%s}}',
    'sh': u'{{NowCommons|File:%s}}',
    'sk': u'{{NowCommons|File:%s}}',
    'sl': u'{{OdslejZbirka|%s}}',
    'sq': u'{{NowCommons|File:%s}}',
    'sr': u'{{NowCommons|File:%s}}',
    'st': u'{{NowCommons|File:%s}}',
    'su': u'{{IlaharKiwari|File:%s}}',
    'sv': u'{{NowCommons|%s}}',
    'sw': u'{{NowCommons|%s}}',
    'ta': u'{{NowCommons|File:%s}}',
    'th': u'{{มีที่คอมมอนส์|File:%s}}',
    'tl': u'{{NasaCommons|File:%s}}',
    'tr': u'{{NowCommons|%s}}',
    'uk': u'{{NowCommons|File:%s}}',
    'ur': u'{{NowCommons|File:%s}}',
    'vec': u'{{NowCommons|%s}}',
    'vi': u'{{NowCommons|File:%s}}',
    'vo': u'{{InKobädikos|%s}}',
    'wa': u'{{NowCommons|%s}}',
    'zh': u'{{NowCommons|File:%s}}',
    'zh-min-nan': u'{{Commons ū|%s}}',
    'zh-yue': u'{{subst:Ncd|File:%s}}',
}

nowCommonsMessage = {
    '_default': u'File is now available on Wikimedia Commons.',
    'ar': u'الملف الآن متوفر في ويكيميديا كومنز.',
    'de': u'Datei ist jetzt auf Wikimedia Commons verfügbar.',
    'en': u'File is now available on Wikimedia Commons.',
    'eo': u'Dosiero nun estas havebla en la Wikimedia-Komunejo.',
    'fa': u'اینک پرونده در ویکی‌انبار قابل دسترسی است.',
    'he': u'הקובץ זמין כעת בוויקישיתוף.',
    'hu': u'A fájl most már elérhető a Wikimedia Commonson.',
    'ia': u'Le file es ora disponibile in Wikimedia Commons.',
    'it': u'L\'immagine è adesso disponibile su Wikimedia Commons.',
    'kk': u'Файлды енді Wikimedia Ортаққорынан қатынауға болады.',
    'lt': u'Failas įkeltas į Wikimedia Commons projektą.',
    'nl': u'Dit bestand staat nu op [[w:nl:Wikimedia Commons|Wikimedia Commons]].',
    'pl': u'Plik jest teraz dostępny na Wikimedia Commons.',
    'pt': u'Arquivo está agora na Wikimedia Commons.',
    'ru': u'Файл теперь доступен на Викискладе.',
    'sr': u'Слика је сада доступна и на Викимедија Остави.',
    'zh': u'檔案已存在於維基共享資源。',
}

moveToCommonsTemplate = {
    'ar': [u'نقل إلى كومنز'],
    'en': [u'Commons ok', u'Copy to Wikimedia Commons', u'Move to commons', u'Movetocommons', u'To commons', u'Copy to Wikimedia Commons by BotMultichill'],
    'fi': [u'Commonsiin'],
    'fr': [u'Image pour Commons'],
    'hsb':[u'Kopěruj do Wikimedia Commons'],
    'hu': [u'Commonsba'],
    'is': [u'Færa á Commons'],
    'ms': [u'Hantar ke Wikimedia Commons'],
    'nl': [u'Verplaats naar Wikimedia Commons', u'VNC'],
    'pl': [u'Do Commons', u'NaCommons', u'Na Commons'],
    'ru': [u'На Викисклад'],
    'sl': [u'Skopiraj v Zbirko'],
    'sr': [u'За оставу'],
    'sv': [u'Till Commons'],
    'zh': [u'Copy to Wikimedia Commons'],
}

imageMoveMessage = {
    '_default': u'[[:File:%s|File]] moved to [[:commons:File:%s|commons]].',
    'ar': u'[[:File:%s|الصورة]] تم نقلها إلى [[:commons:File:%s|كومنز]].',
    'en': u'[[:File:%s|File]] moved to [[:commons:File:%s|commons]].',
    'fa': u'[[:پرونده:%s|پرونده]] به [[:commons:File:%s|commons]] منتقل شد.',
    'hu': u'[[:File:%s|Kép]] átmozgatva a [[:commons:File:%s|Commons]]ba.',
    'nl': u'[[:File:%s|Bestand]] is verplaatst naar [[:commons:File:%s|commons]].',
    'pl': u'[[:File:%s|Plik]] przeniesiona do [[:commons:File:%s|commons]].',
    'ru': u'[[:File:%s|Файл]] перемещён на [[:commons:File:%s|Викисклад]].',
    'zh': u'[[:File:%s|本檔案]]已移至[[:commons:File:%s|維基共享資源]]',
}

def pageTextPost(url,parameters):
    gotInfo = False;
    while(not gotInfo):
        try:
            commonsHelperPage = urllib.urlopen("http://toolserver.org/~magnus/commonshelper.php", parameters)
            data = commonsHelperPage.read().decode('utf-8')
            gotInfo = True;
        except IOError:
            pywikibot.output(u'Got an IOError, let\'s try again')
        except socket.timeout:
            pywikibot.output(u'Got a timeout, let\'s try again')
    return data

class imageTransfer (threading.Thread):

    def __init__ ( self, imagePage, newname, category):
        self.imagePage = imagePage
        self.newname = newname
        self.category = category
        threading.Thread.__init__ ( self )

    def run(self):
        tosend={'language':self.imagePage.site().language().encode('utf-8'),
                'image':self.imagePage.titleWithoutNamespace().encode('utf-8'),
                'newname':self.newname.encode('utf-8'),
                'project':self.imagePage.site().family.name.encode('utf-8'),
                'username':'',
                'commonsense':'1',
                'remove_categories':'1',
                'ignorewarnings':'1',
                'doit':'Uitvoeren'
                }

        tosend=urllib.urlencode(tosend)
        print tosend
        CH=pageTextPost('http://www.toolserver.org/~magnus/commonshelper.php', tosend)
        print 'Got CH desc.'

        tablock=CH.split('<textarea ')[1].split('>')[0]
        CH=CH.split('<textarea '+tablock+'>')[1].split('</textarea>')[0]
        CH=CH.replace(u'&times;', u'×')
        CH = self.fixAuthor(CH)
        pywikibot.output(CH);

        # I want every picture to be tagged with the bottemplate so i can check my contributions later.
        CH=u'\n\n{{BotMoveToCommons|'+ self.imagePage.site().language() + '.' + self.imagePage.site().family.name +'|year={{subst:CURRENTYEAR}}|month={{subst:CURRENTMONTHNAME}}|day={{subst:CURRENTDAY}}}}' + CH

        if self.category:
            CH = CH.replace(u'{{subst:Unc}} <!-- Remove this line once you have added categories -->', u'')
            CH = CH + u'[[Category:' + self.category + u']]'

        bot = UploadRobot(url=self.imagePage.fileUrl(), description=CH, useFilename=self.newname, keepFilename=True, verifyDescription=False, ignoreWarning = True, targetSite = pywikibot.getSite('commons', 'commons'))
        bot.run()

        #Should check if the image actually was uploaded
        if pywikibot.Page(pywikibot.getSite('commons', 'commons'), u'Image:' + self.newname).exists():
            #Get a fresh copy, force to get the page so we dont run into edit conflicts
            imtxt=self.imagePage.get(force=True)

            #Remove the move to commons templates
            if self.imagePage.site().language() in moveToCommonsTemplate:
                for moveTemplate in moveToCommonsTemplate[self.imagePage.site().language()]:
                    imtxt = re.sub(u'(?i)\{\{' + moveTemplate + u'[^\}]*\}\}', u'', imtxt)

            #add {{NowCommons}}
            if self.imagePage.site().language() in nowCommonsTemplate:
                addTemplate = nowCommonsTemplate[self.imagePage.site().language()] % self.newname
            else:
                addTemplate = nowCommonsTemplate['_default'] % self.newname

            if self.imagePage.site().language() in nowCommonsMessage:
                commentText = nowCommonsMessage[self.imagePage.site().language()]
            else:
                commentText = nowCommonsMessage['_default']

            pywikibot.showDiff(self.imagePage.get(), imtxt+addTemplate)
            self.imagePage.put(imtxt + addTemplate, comment = commentText)

            self.gen = pagegenerators.FileLinksGenerator(self.imagePage)
            self.preloadingGen = pagegenerators.PreloadingGenerator(self.gen)

            #If the image is uploaded under a different name, replace all instances
            if self.imagePage.titleWithoutNamespace() != self.newname:
                if self.imagePage.site().language() in imageMoveMessage:
                    moveSummary = imageMoveMessage[self.imagePage.site().language()] % (self.imagePage.titleWithoutNamespace(), self.newname)
                else:
                    moveSummary = imageMoveMessage['_default'] % (self.imagePage.titleWithoutNamespace(), self.newname)
                imagebot = ImageRobot(generator = self.preloadingGen, oldImage = self.imagePage.titleWithoutNamespace(), newImage = self.newname, summary = moveSummary, always = True, loose = True)
                imagebot.run()
        return

    def fixAuthor(self, pageText):
        '''
        Fix the author field in the information template.
        '''
        informationRegex = re.compile(u'\|Author\=Original uploader was (?P<author>\[\[:\w+:\w+:\w+\|\w+\]\] at \[.+\])')
        selfRegex = re.compile(u'\{\{self\|author\=(?P<author>\[\[:\w+:\w+:\w+\|\w+\]\] at \[.+\])\|')

        #Find the |Author=Original uploader was ....
        informationMatch = informationRegex.search(pageText)

        #Find the {{self|author=
        selfMatch = selfRegex.search(pageText)

        #Check if both are found and are equal
        if (informationMatch and selfMatch):
            if(informationMatch.group('author')==selfMatch.group('author')):
                #Replace |Author=Original uploader was ... with |Author= ...
                pageText = informationRegex.sub(r'|Author=\g<author>', pageText)

        return pageText


#-label ok skip view
#textarea
archivo=pywikibot.config.datafilepath("Uploadbot.localskips.txt")
try:
    open(archivo, 'r')
except IOError:
    tocreate=open(archivo, 'w')
    tocreate.write("{{NowCommons")
    tocreate.close()

def getautoskip():
    '''
    Get a list of templates to skip.
    '''
    f=codecs.open(archivo, 'r', 'utf-8')
    txt=f.read()
    f.close()
    toreturn=txt.split('{{')[1:]
    return toreturn

class Tkdialog:
    def __init__(self, image_title, content, uploader, url, templates, commonsconflict=0):
        self.root=Tk()
        #"%dx%d%+d%+d" % (width, height, xoffset, yoffset)
        #Always appear the same size and in the bottom-left corner
        self.root.geometry("600x200+100-100")
        #self.nP=wikipediaPage
        self.root.title(image_title)
        self.changename=''
        self.skip=0
        self.url=url
        self.uploader="Unkown"
        #uploader.decode('utf-8')
        scrollbar=Scrollbar(self.root, orient=VERTICAL)
        label=Label(self.root,text=u"Enter new name or leave blank.")
        imageinfo=Label(self.root, text='Uploaded by '+uploader+'.')
        textarea=Text(self.root)
        textarea.insert(END, content.encode('utf-8'))
        textarea.config(state=DISABLED, height=8, width=40, padx=0, pady=0, wrap=WORD, yscrollcommand=scrollbar.set)
        scrollbar.config(command=textarea.yview)
        self.entry=Entry(self.root)

        self.templatelist=Listbox(self.root, bg="white", height=5)

        for template in templates:
            self.templatelist.insert(END, template)
        autoskipButton=Button(self.root, text="Add to AutoSkip", command=self.add2autoskip)
        browserButton=Button(self.root, text='View in browser', command=self.openInBrowser)
        skipButton=Button(self.root, text="Skip", command=self.skipFile)
        okButton=Button(self.root, text="OK", command=self.okFile)

        ##Start grid
        label.grid(row=0)
        okButton.grid(row=0, column=1, rowspan=2)
        skipButton.grid(row=0, column=2, rowspan=2)
        browserButton.grid(row=0, column=3, rowspan=2)

        self.entry.grid(row=1)

        textarea.grid(row=2, column=1, columnspan=3)
        scrollbar.grid(row=2, column=5)
        self.templatelist.grid(row=2, column=0)

        autoskipButton.grid(row=3, column=0)
        imageinfo.grid(row=3, column=1, columnspan=4)

    def okFile(self):
        '''
        The user pressed the OK button.
        '''
        self.changename=self.entry.get()
        self.root.destroy()

    def skipFile(self):
        '''
        The user pressed the Skip button.
        '''
        self.skip=1
        self.root.destroy()

    def openInBrowser(self):
        '''
        The user pressed the View in browser button.
        '''
        webbrowser.open(self.url)

    def add2autoskip(self):
        '''
        The user pressed the Add to AutoSkip button.
        '''
        templateid=int(self.templatelist.curselection()[0])
        template=self.templatelist.get(templateid)
        toadd=codecs.open(archivo, 'a', 'utf-8')
        toadd.write('{{'+template)
        toadd.close()
        self.skipFile()

    def getnewname(self):
        '''
        Activate the dialog and return the new name and if the image is skipped.
        '''
        self.root.mainloop()
        return (self.changename, self.skip)


def doiskip(pagetext):
    '''
    Skip this image or not.
    Returns True if the image is on the skip list, otherwise False
    '''
    saltos=getautoskip()
    #print saltos
    for salto in saltos:
        rex=ur'\{\{\s*['+salto[0].upper()+salto[0].lower()+']'+salto[1:]+'(\}\}|\|)'
        #print rex
        if re.search(rex, pagetext):
            return True
    return False


def main(args):
    generator = None;
    #newname = "";
    imagepage = None;
    always = False
    category = u''
    # Load a lot of default generators
    genFactory = pagegenerators.GeneratorFactory()

    for arg in pywikibot.handleArgs():
        if arg == '-always':
            always = True
        elif arg.startswith('-cc:'):
            category = arg [len('-cc:'):]
        else:
            genFactory.handleArg(arg)

    generator = genFactory.getCombinedGenerator()
    if not generator:
        raise add_text.NoEnoughData('You have to specify the generator you want to use for the script!')

    pregenerator = pagegenerators.PreloadingGenerator(generator)

    for page in pregenerator:
        skip = False
        if page.exists() and (page.namespace() == 6) and (not page.isRedirectPage()) :
            imagepage = pywikibot.ImagePage(page.site(), page.title())

            #First do autoskip.
            if doiskip(imagepage.get()):
                pywikibot.output("Skipping " + page.title())
                skip = True
            else:
                # The first upload is last in the list.
                try:
                    username = imagepage.getLatestUploader()[0]
                except NotImplementedError:
                    #No API, using the page file instead
                    (datetime, username, resolution, size, comment) = imagepage.getFileVersionHistory().pop()
                if always:
                    newname=imagepage.titleWithoutNamespace()
                    CommonsPage=pywikibot.Page(pywikibot.getSite('commons', 'commons'), u'File:'+newname)
                    if CommonsPage.exists():
                        skip = True
                else:
                    while True:

                        # Do the Tkdialog to accept/reject and change te name
                        (newname, skip)=Tkdialog(imagepage.titleWithoutNamespace(), imagepage.get(), username, imagepage.permalink(), imagepage.templates()).getnewname()

                        if skip:
                            pywikibot.output('Skipping this image')
                            break

                        # Did we enter a new name?
                        if len(newname)==0:
                            #Take the old name
                            newname=imagepage.titleWithoutNamespace()
                        else:
                            newname = newname.decode('utf-8')

                        # Check if the image already exists
                        CommonsPage=pywikibot.Page(
                                       pywikibot.getSite('commons', 'commons'),
                                       u'File:'+newname)
                        if not CommonsPage.exists():
                            break
                        else:
                            pywikibot.output('Image already exists, pick another name or skip this image')
                        # We dont overwrite images, pick another name, go to the start of the loop

            if not skip:
                imageTransfer(imagepage, newname, category).start()

    pywikibot.output(u'Still ' + str(threading.activeCount()) + u' active threads, lets wait')
    for openthread in threading.enumerate():
        if openthread != threading.currentThread():
            openthread.join()
    pywikibot.output(u'All threads are done')

if __name__ == "__main__":
    try:
        main(sys.argv[1:])
    finally:
        pywikibot.stopme()
