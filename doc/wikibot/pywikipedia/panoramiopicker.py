#!/usr/bin/python
# -*- coding: utf-8  -*-
'''
Tool to copy a Panoramio set to Commons

'''
#
# (C) Multichill, 2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: panoramiopicker.py 9042 2011-03-13 10:14:47Z xqt $'

import sys, urllib, re,  StringIO, hashlib, base64, time
try:
    #For Python 2.6 newer
    import json
    if not hasattr(json, 'loads'):
        # 'json' can also be the name in for
        # http://pypi.python.org/pypi/python-json
        raise ImportError
except ImportError:
    import simplejson as json
import wikipedia as pywikibot
import config, query, imagerecat, upload

from Tkinter import *
from PIL import Image, ImageTk    # see: http://www.pythonware.com/products/pil/
from BeautifulSoup import BeautifulSoup

def isAllowedLicense(photoInfo = None):
    '''
    Check if the image contains the right license

    TODO: Maybe add more licenses
    '''
    allowed = [u'by-sa']
    if photoInfo[u'license'] in allowed:
        return True
    else:
        return False

def downloadPhoto(photoUrl = ''):
    '''
    Download the photo and store it in a StrinIO.StringIO object.

    TODO: Add exception handling

    '''
    imageFile=urllib.urlopen(photoUrl).read()
    return StringIO.StringIO(imageFile)

def findDuplicateImages(photo=None,
                        site=pywikibot.getSite(u'commons', u'commons')):
    ''' Takes the photo, calculates the SHA1 hash and asks the mediawiki api
    for a list of duplicates.

    TODO: Add exception handling, fix site thing

    '''
    hashObject = hashlib.sha1()
    hashObject.update(photo.getvalue())
    return site.getFilesFromAnHash(base64.b16encode(hashObject.digest()))

def getTags(photoInfo = None):
    ''' Get all the tags on a photo '''
    result = []

    return result

def getLicense(photoInfo=None):
    '''
    Currently the Panoramio API doesn't expose the license
    Adding it with a beautiful soup hack
    '''

    photoInfo['license']=u'c'
    page = urllib.urlopen(photoInfo.get(u'photo_url'))
    data = page.read()
    soup = BeautifulSoup(data)
    if soup.find("div", {'id' : 'photo-info'}):
        pointer = soup.find("div", {'id' : 'photo-info'})
        if pointer.find("div", {'id' : 'photo-details'}):
            pointer = pointer.find("div", {'id' : 'photo-details'})
            if pointer.find("ul", {'id' : 'details'}):
                pointer = pointer.find("ul", {'id' : 'details'})
                if pointer.find("li", {'class' : 'license by-sa'}):
                    photoInfo['license']=u'by-sa'
                # Does Panoramio have more license options?

    return photoInfo



def getFilename(photoInfo=None, site=pywikibot.getSite(u'commons', u'commons'),
                project=u'Panoramio'):
    ''' Build a good filename for the upload based on the username and the
    title. Prevents naming collisions.

    '''
    username = photoInfo.get(u'owner_name')
    title = photoInfo.get(u'photo_title')
    if title:
        title =  cleanUpTitle(title)
    else:
        title = u''

    if pywikibot.Page(site, u'File:%s - %s - %s.jpg'
                      % (project, username, title) ).exists():
        i = 1
        while True:
            if (pywikibot.Page(site, u'File:%s - %s - %s (%s).jpg'
                               % (project, username, title, str(i))).exists()):
                i = i + 1
            else:
                return u'%s - %s - %s (%s).jpg' % (project, username, title,
                                                   str(i))
    else:
        return u'%s - %s - %s.jpg' % (project, username, title)

def cleanUpTitle(title):
    ''' Clean up the title of a potential mediawiki page. Otherwise the title of
    the page might not be allowed by the software.

    '''
    title = title.strip()
    title = re.sub(u"[<{\\[]", u"(", title)
    title = re.sub(u"[>}\\]]", u")", title)
    title = re.sub(u"[ _]?\\(!\\)", u"", title)
    title = re.sub(u",:[ _]", u", ", title)
    title = re.sub(u"[;:][ _]", u", ", title)
    title = re.sub(u"[\t\n ]+", u" ", title)
    title = re.sub(u"[\r\n ]+", u" ", title)
    title = re.sub(u"[\n]+", u"", title)
    title = re.sub(u"[?!]([.\"]|$)", u"\\1", title)
    title = re.sub(u"[&#%?!]", u"^", title)
    title = re.sub(u"[;]", u",", title)
    title = re.sub(u"[/+\\\\:]", u"-", title)
    title = re.sub(u"--+", u"-", title)
    title = re.sub(u",,+", u",", title)
    title = re.sub(u"[-,^]([.]|$)", u"\\1", title)
    title = title.replace(u" ", u"_")
    return title


def getDescription(photoInfo=None, panoramioreview=False, reviewer=u'',
                     override=u'', addCategory=u''):
    '''
    Build description for the image.
    '''

    desc = u''
    desc = desc + u'{{Information\n'
    desc = desc + u'|description=%(photo_title)s\n'
    desc = desc + u'|date=%(upload_date)s (upload date)\n'
    desc = desc + u'|source=[%(photo_url)s Panoramio]\n'
    desc = desc + u'|author=[%(owner_url)s?with_photo_id=%(photo_id)s %(owner_name)s] \n'
    desc = desc + u'|permission=\n'
    desc = desc + u'|other_versions=\n'
    desc = desc + u'|other_fields=\n'
    desc = desc + u'}}\n'
    if photoInfo.get(u'latitude') and photoInfo.get(u'longitude'):
        desc = desc + u'{{Location dec|%(latitude)s|%(longitude)s|source:Panoramio}}\n'
    desc = desc + u'\n'
    desc = desc + u'=={{int:license-header}}==\n'

    if override:
        desc = desc + override
    else:
        if photoInfo.get(u'license')==u'by-sa':
            desc = desc + u'{{Cc-by-sa-3.0}}\n'
        if panoramioreview:
            desc = desc + u'{{Panoramioreview|%s|{{subst:CURRENTYEAR}}-{{subst:CURRENTMONTH}}-{{subst:CURRENTDAY2}}}}\n' % (reviewer,)
        else:
            desc = desc + u'{{Panoramioreview}}\n'

    desc = desc + u'\n'
    cats = u''
    if addCategory:
        desc = desc + u'\n[[Category:%s]]\n' % (addCategory,)
        cats = True

    # Get categories based on location
    if photoInfo.get(u'latitude') and photoInfo.get(u'longitude'):
        cats=imagerecat.getOpenStreetMapCats(photoInfo.get(u'latitude'), photoInfo.get(u'longitude'))
        cats=imagerecat.applyAllFilters(cats)
        for cat in cats:
            desc = desc + u'[[Category:%s]]\n' % (cat,)
    if not cats:
        desc = desc + u'{{subst:Unc}}\n'

    return desc % photoInfo

def processPhoto(photoInfo=None, panoramioreview=False, reviewer=u'',
                 override=u'', addCategory=u'', autonomous=False):
    ''' Process a single Panoramio photo '''


    if isAllowedLicense(photoInfo) or override:
        #Should download the photo only once
        photo = downloadPhoto(photoInfo.get(u'photo_file_url'))

        #Don't upload duplicate images, should add override option
        duplicates = findDuplicateImages(photo)
        if duplicates:
            pywikibot.output(u'Found duplicate image at %s' % duplicates.pop())
        else:
            filename = getFilename(photoInfo)
            pywikibot.output(filename)
            description = getDescription(photoInfo, panoramioreview,
                                         reviewer, override, addCategory)

            pywikibot.output(description)
            if not autonomous:
                (newDescription, newFilename, skip) = Tkdialog(
                    description, photo, filename).run()
            else:
                newDescription = description
                newFilename = filename
                skip = False
        #pywikibot.output(newPhotoDescription)
        #if (pywikibot.Page(title=u'File:'+ filename, site=pywikibot.getSite()).exists()):
        # I should probably check if the hash is the same and if not upload it under a different name
        #pywikibot.output(u'File:' + filename + u' already exists!')
        #else:
            #Do the actual upload
            #Would be nice to check before I upload if the file is already at Commons
            #Not that important for this program, but maybe for derived programs
            if not skip:
                bot = upload.UploadRobot(photoInfo.get(u'photo_file_url'),
                                         description=newDescription,
                                         useFilename=newFilename,
                                         keepFilename=True,
                                         verifyDescription=False)
                bot.upload_image(debug=False)
                return 1
    return 0


class Tkdialog:
    ''' The user dialog. '''
    def __init__(self, photoDescription, photo, filename):
        self.root=Tk()
        #"%dx%d%+d%+d" % (width, height, xoffset, yoffset)
        self.root.geometry("%ix%i+10-10"%(config.tkhorsize, config.tkvertsize))

        self.root.title(filename)
        self.photoDescription = photoDescription
        self.filename = filename
        self.photo = photo
        self.skip=False
        self.exit=False

        ## Init of the widgets
        # The image
        self.image=self.getImage(self.photo, 800, 600)
        self.imagePanel=Label(self.root, image=self.image)

        self.imagePanel.image = self.image

        # The filename
        self.filenameLabel=Label(self.root,text=u"Suggested filename")
        self.filenameField=Entry(self.root, width=100)
        self.filenameField.insert(END, filename)

        # The description
        self.descriptionLabel=Label(self.root,text=u"Suggested description")
        self.descriptionScrollbar=Scrollbar(self.root, orient=VERTICAL)
        self.descriptionField=Text(self.root)
        self.descriptionField.insert(END, photoDescription)
        self.descriptionField.config(state=NORMAL, height=12, width=100, padx=0, pady=0, wrap=WORD, yscrollcommand=self.descriptionScrollbar.set)
        self.descriptionScrollbar.config(command=self.descriptionField.yview)

        # The buttons
        self.okButton=Button(self.root, text="OK", command=self.okFile)
        self.skipButton=Button(self.root, text="Skip", command=self.skipFile)

        ## Start grid

        # The image
        self.imagePanel.grid(row=0, column=0, rowspan=11, columnspan=4)

        # The buttons
        self.okButton.grid(row=11, column=1, rowspan=2)
        self.skipButton.grid(row=11, column=2, rowspan=2)

        # The filename
        self.filenameLabel.grid(row=13, column=0)
        self.filenameField.grid(row=13, column=1, columnspan=3)

        # The description
        self.descriptionLabel.grid(row=14, column=0)
        self.descriptionField.grid(row=14, column=1, columnspan=3)
        self.descriptionScrollbar.grid(row=14, column=5)

    def getImage(self, photo, width, height):
        ''' Take the StringIO object and build an imageTK thumbnail '''
        image = Image.open(photo)
        image.thumbnail((width, height))
        imageTk = ImageTk.PhotoImage(image)
        return imageTk

    def okFile(self):
        ''' The user pressed the OK button. '''
        self.filename=self.filenameField.get()
        self.photoDescription=self.descriptionField.get(0.0, END)
        self.root.destroy()

    def skipFile(self):
        ''' The user pressed the Skip button. '''
        self.skip=True
        self.root.destroy()

    def run(self):
        ''' Activate the dialog and return the new name and if the image is
        skipped.

        '''
        self.root.mainloop()
        return (self.photoDescription, self.filename, self.skip)


def getPhotos(photoset=u'', start_id='', end_id='', interval=100):
    ''' Loop over a set of Panoramio photos. '''
    i=0
    has_more=True
    url = u'http://www.panoramio.com/map/get_panoramas.php?set=%s&from=%s&to=%s&size=original'
    while has_more:
        gotInfo = False
        maxtries = 10
        tries = 0
        while(not gotInfo):
            try:
                if ( tries < maxtries ):
                    tries = tries + 1
                    panoramioApiPage = urllib.urlopen(url % (photoset, i, i+interval))
                    contents = panoramioApiPage.read().decode('utf-8')
                    gotInfo = True
                    i = i + interval
                else:
                    break
            except IOError:
                pywikibot.output(u'Got an IOError, let\'s try again')
            except socket.timeout:
                pywikibot.output(u'Got a timeout, let\'s try again')

        metadata = json.loads(contents)
        count = metadata.get(u'count') # Useless?
        photos = metadata.get(u'photos')
        for photo in photos:
            yield photo
        has_more = metadata.get(u'has_more')

    return

def usage():
    '''
    Print usage information

    TODO : Need more.
    '''
    pywikibot.output(
        u"Panoramiopicker is a tool to transfer Panaramio photos to Wikimedia Commons")
    pywikibot.output(u"-set:<set_id>\n")
    return

def main():
    site = pywikibot.getSite(u'commons', u'commons')
    pywikibot.setSite(site)
    #imagerecat.initLists()

    photoset = u'' #public (popular photos), full (all photos), user ID number
    size = u'original'
    minx = u''
    miny = u''
    maxx = u''
    maxy = u''
    start_id = u''
    end_id = u''
    addCategory = u''
    autonomous = False
    totalPhotos = 0
    uploadedPhotos = 0

    # Do we mark the images as reviewed right away?
    if config.panoramio ['review']:
        panoramioreview = config.panoramio['review']
    else:
        panoramioreview = False

    # Set the Panoramio reviewer
    if config.panoramio['reviewer']:
        reviewer = config.panoramio['reviewer']
    elif 'commons' in config.sysopnames['commons']:
        print config.sysopnames['commons']
        reviewer = config.sysopnames['commons']['commons']
    elif 'commons' in config.usernames['commons']:
        reviewer = config.usernames['commons']['commons']
    else:
        reviewer = u''

    # Should be renamed to overrideLicense or something like that
    override = u''
    for arg in pywikibot.handleArgs():
        if arg.startswith('-set'):
            if len(arg) == 4:
                photoset = pywikibot.input(u'What is the set?')
            else:
                photoset = arg[5:]
        elif arg.startswith('-start_id'):
            if len(arg) == 9:
                start_id = pywikibot.input(
                    u'What is the id of the photo you want to start at?')
            else:
                start_id = arg[10:]
        elif arg.startswith('-end_id'):
            if len(arg) == 7:
                end_id = pywikibot.input(
                    u'What is the id of the photo you want to end at?')
            else:
                end_id = arg[8:]
        elif arg.startswith('-tags'):
            if len(arg) == 5:
                tags = pywikibot.input(
                    u'What is the tag you want to filter out (currently only one supported)?')
            else:
                tags = arg[6:]
        elif arg == '-panoramioreview':
            panoramioreview = True
        elif arg.startswith('-reviewer'):
            if len(arg) == 9:
                reviewer = pywikibot.input(u'Who is the reviewer?')
            else:
                reviewer = arg[10:]
        elif arg.startswith('-override'):
            if len(arg) == 9:
                override = pywikibot.input(u'What is the override text?')
            else:
                override = arg[10:]
        elif arg.startswith('-addcategory'):
            if len(arg) == 12:
                addCategory = pywikibot.input(
                    u'What category do you want to add?')
            else:
                addCategory = arg[13:]
        elif arg == '-autonomous':
            autonomous = True

    if photoset:
        for photoInfo in getPhotos(photoset, start_id, end_id):
            photoInfo = getLicense(photoInfo)
            #time.sleep(10)
            uploadedPhotos += processPhoto(photoInfo, panoramioreview,
                                           reviewer, override, addCategory,
                                           autonomous)
            totalPhotos += 1
    else:
        usage()
    pywikibot.output(u'Finished running')
    pywikibot.output(u'Total photos: ' + str(totalPhotos))
    pywikibot.output(u'Uploaded photos: ' + str(uploadedPhotos))

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
