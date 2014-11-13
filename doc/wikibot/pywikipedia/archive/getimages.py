"""
Script to transfer many images from one wiki to another. Your
language (which can be changed with the -lang: argument) is the
language to upload to. The images should be in a file as interwiki
links (that is in the form [[en:Image:myimage.png]]); they do not
need to be all from the same Wiki. This file can be created with
extract_wikilinks.py.

Arguments:

  -lang:xx Log in to the given wikipedia language to upload to

The first other argument is taken to be the name of the file you get
the links from; other arguments are ignored.
"""

#
# (C) Andre Engels 2004
#
# Distributed under the terms of the MIT license.
#
# Modified by Gerrit Holl, 01-11-2004
__version__='$Id: getimages.py,v 1.15 2005/12/21 17:51:26 wikipedian Exp $'

import sys
import wikipedia, lib_images, pagegenerators

def getfn():
    fns = []

    for arg in sys.argv[1:]:
        arg = wikipedia.argHandler(arg, 'getimages')
        if arg:
            fns.append(arg)

    if len(fns) == 0:
        fns.append(raw_input("Please enter a filename: "))

    return fns

def main():
    for filename in getfn():
        print "Handling images from %s" % filename
        gen = pagegenerators.TextfilePageGenerator(filename)
        for image in gen:
            if image.isImage():
                print "-" * 50
                print "Image: %s" % image.title()
                try:
                    # show the image description page's contents
                    print image.get()
                except wikipedia.NoPage:
                    print "Description empty."
                except wikipedia.IsRedirectPage:
                    print "Description page is redirect?!"
                answer=wikipedia.input(u"Copy this image (y/N)?")
                if answer.lower().startswith('y'):
                    lib_images.transfer_image(image)

if __name__ == "__main__":
    try:
        main()
    except:
        wikipedia.stopme()
        raise
    else:
        wikipedia.stopme()
