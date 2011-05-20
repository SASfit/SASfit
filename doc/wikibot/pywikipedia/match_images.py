#!/usr/bin/python
# -*- coding: utf-8  -*-
'''
Program to match two images based on histograms.

Usage:
match_images.py ImageA ImageB

This is just a first version so that other people can play around with it.
Expect the code to change a lot!

'''
#
# (C) Multichill, 2009
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: match_images.py 9042 2011-03-13 10:14:47Z xqt $'

import sys, math, StringIO
import wikipedia, config
from PIL import Image

def matchImagePages(imagePageA, imagePageB):
    '''
    This functions expects two image page objects.
    It will return True if the image are the same and False if the images are not the same
    '''

    imageA = getImageFromImagePage(imagePageA)
    imageB = getImageFromImagePage(imagePageB)

    (imA_width, imA_height) = imageA.size
    (imB_width, imB_height) = imageB.size

    imageB = imageB.resize((imA_width, imA_height))

    imageA_topleft = imageA.crop((0,0, int(imA_width/2), int(imA_height/2)))
    imageB_topleft = imageB.crop((0,0, int(imA_width/2), int(imA_height/2)))

    imageA_topright = imageA.crop((int(imA_width/2),0, imA_width, int(imA_height/2)))
    imageB_topright = imageB.crop((int(imA_width/2),0, imA_width, int(imA_height/2)))

    imageA_bottomleft = imageA.crop((0, int(imA_height/2), int(imA_width/2), imA_height))
    imageB_bottomleft = imageB.crop((0, int(imA_height/2), int(imA_width/2), imA_height))

    imageA_bottomright = imageA.crop((int(imA_width/2),int(imA_height/2), imA_width, imA_height))
    imageB_bottomright = imageB.crop((int(imA_width/2),int(imA_height/2), imA_width, imA_height))

    imageA_center = imageA.crop((int(imA_width * 0.25), int(imA_height * 0.25), int(imA_width * 0.75), int(imA_height * 0.75)))
    imageB_center = imageB.crop((int(imA_width * 0.25), int(imA_height * 0.25), int(imA_width * 0.75), int(imA_height * 0.75)))

    wholeScore = matchImages(imageA, imageB)
    topleftScore = matchImages(imageA_topleft, imageB_topleft)
    toprightScore = matchImages(imageA_topright, imageB_topright)
    bottomleftScore = matchImages(imageA_bottomleft, imageB_bottomleft)
    bottomrightScore = matchImages(imageA_bottomright, imageB_bottomright)
    centerScore = matchImages(imageA_center, imageB_center)
    averageScore = (wholeScore + topleftScore + toprightScore + bottomleftScore + bottomrightScore + centerScore)/6

    print u'Whole image           ' + str(wholeScore)
    print u'Top left of image     ' + str(topleftScore)
    print u'Top right of image    ' + str(toprightScore)
    print u'Bottom left of image  ' + str(bottomleftScore)
    print u'Bottom right of image ' + str(bottomrightScore)
    print u'Center of image       ' + str(centerScore)
    print u'                      -------------'
    print u'Average               ' + str(averageScore)

    # Hard coded at 80%, change this later on.
    if ((averageScore*100) > 80):
        print u'We have a match!'
        return True
    else:
        print u'Not the same.'
        return False

def getImageFromImagePage(imagePage):
    '''
    Get the image object to work based on an imagePage object
    '''
    imageURL=imagePage.fileUrl()
    imageURLopener= wikipedia.MyURLopener
    imageWebFile = imageURLopener.open(imageURL)
    imageBuffer = StringIO.StringIO(imageWebFile.read())
    image = Image.open(imageBuffer)

    return image

def matchImages(imageA, imageB):
    '''
    Match two image objects. Return the ratio of pixels that match
    '''
    histogramA = imageA.histogram()
    histogramB = imageB.histogram()

    totalMatch = 0
    totalPixels = 0

    if not (len(histogramA)==len(histogramB)):
        return 0

    for i in range(0, len(histogramA)):
        totalMatch = totalMatch + min(histogramA[i], histogramB[i])
        totalPixels = totalPixels + max(histogramA[i], histogramB[i])

    if (totalPixels==0):
        return 0;

    return float(totalMatch)/float(totalPixels)*100



def main():
    site = wikipedia.getSite(u'commons', u'commons')

    #Array of images to work on
    images = []
    imageTitleA = u''
    imageTitleB = u''
    familyA = u''
    familyB = u''
    langA = u''
    langB = u''
    imagePageA = None
    imagePageB = None

    for arg in wikipedia.handleArgs():
        if arg.startswith('-familyA:'):
            if len(arg) == len('-familyA:'):
                familyA = wikipedia.input(u'What family do you want to use?')
            else:
                familyA = arg[len('-familyA:'):]
        elif arg.startswith('-familyB:'):
            if len(arg) == len('-familyB:'):
                familyB = wikipedia.input(u'What family do you want to use?')
            else:
                familyB = arg[len('-familyB:'):]
        elif arg.startswith('-langA:'):
            if len(arg) == len('-langA:'):
                langA = wikipedia.input(u'What language do you want to use?')
            else:
                langA = arg[len('-langA:'):]
        elif arg.startswith('-langB:'):
            if len(arg) == len('-langB:'):
                langB = wikipedia.input(u'What language do you want to use?')
            else:
                langB = arg[len('langB:'):]
        else:
            images.append(arg)

    if not (len(images)==2):
        raise wikipedia.Error, 'require two images to work on.'
    else:
        imageTitleA=images[0]
        imageTitleB=images[1]

    if not (imageTitleA == u''):
        if not (langA == u''):
            if not (familyA == u''):
                imagePageA = wikipedia.ImagePage(wikipedia.getSite(langA, familyA), imageTitleA)
            else:
                imagePageA = wikipedia.ImagePage(wikipedia.getSite(langA, u'wikipedia'), imageTitleA)
        else:
            imagePageA = wikipedia.ImagePage(wikipedia.getSite(u'commons', u'commons'), imageTitleA)

    if not (imageTitleB == u''):
        if not (langB == u''):
            if not (familyB == u''):
                imagePageB = wikipedia.ImagePage(wikipedia.getSite(langB, familyB), imageTitleB)
            else:
                imagePageB = wikipedia.ImagePage(wikipedia.getSite(langB, u'wikipedia'), imageTitleB)
        else:
            imagePageB = wikipedia.ImagePage(wikipedia.getSite(u'commons', u'commons'), imageTitleB)

    if (imagePageA and imagePageB):
        matchImagePages(imagePageA, imagePageB)


if __name__ == "__main__":
    try:
        main()
    finally:
        wikipedia.stopme()
