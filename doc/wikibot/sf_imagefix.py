#!/usr/bin/python
# -*- coding: utf-8  -*-

import sys, re, os.path
import wikipedia as pywikibot
import pagegenerators, catlib, weblinkchecker, upload

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;': pagegenerators.parameterHelp,
}

def getImgName(text, start, end):
    if text is None or len(text) <= 0 or start < 0: return
    gfx = text.find(u"includegraphics",start,end)
    if gfx < 0:
        return u""
    idxOpen = text.find(u"{",gfx,end)
    idxClose = text.find(u"}",idxOpen,end)
    return text[idxOpen+1:idxClose]

def getImgCaption(text, start, end):
    if text is None or len(text) <= 0 or start < 0: return
    cap = text.find(u"caption",start)
    if cap < 0:
        return u""
    idxOpen = text.find(u"{",cap)
    count = 1
    for (idxClose,char) in zip(range(idxOpen+1,end),text[idxOpen+1:end]):
        if char not in [u"{", u"}"]: continue
        if char == u"{": count += 1
        if char == u"}": count -= 1
        if count == 0:
            break
    return text[idxOpen+1:idxClose]

def convertLatex(text):
    text = re.sub(u"\\\\texttt\{([^}{]+)\}", u"{{mono|\\1}}", text)
    text = re.sub(u"\\$\\[", u"<math>", text)
    text = re.sub(u"\\]\\$", u"</math>", text)
    idx = text.find(u"$")
    isOpen = False
    while idx >= 0:
        if isOpen:
            text = text[:idx] + u"</math>" + text[idx+1:]
            isOpen = False
        else:
            text = text[:idx] + u"<math>" + text[idx+1:]
            isOpen = True
        idx = text.find(u"$")
    text = text.replace(u"\r\n",u" ")
    return text

class FileNotFound(pywikibot.Error):
    """Specified file could not be found in file system."""
    def __init__(s, fn):
        s.msg = fn

    def __str__(s):
        return s.msg

class SFRobot:
    def __init__(self, generator):
        self.generator = generator
        self.basepath = "/home/ingo/code/sasfit/trunk/from_cvs/doc/manual"
        if not os.path.exists(self.basepath):
            self.basepath = None

        self.descrGeneric = u"[[category:Image]]\n[[category:Original Content]]\n\n== Licensing ==\n{{cc-by-sa-3.0}}"

    def run(self):
        for page in self.generator:
            try:
                # get the page, and save it using the unmodified text.
                # whether or not getting a redirect throws an exception
                # depends on the variable self.touch_redirects.
                text = page.get()
                originalText = text
                key = u" FIGURE DELETED"
                dstIdx = text.find(key)
                blkStart = 0
                while dstIdx >= 0:
                    blkStart = text.find(u"begin{figure}",blkStart)
                    blkEnd = text.find(u"end{figure}",blkStart+1)
                    filename = getImgName(text, blkStart, blkEnd)
                    caption = getImgCaption(text, blkStart, blkEnd)
                    caption = convertLatex(caption)
                    if len(filename) > 0:
                        filename = os.path.abspath( os.path.join(self.basepath,filename) )
                        if not os.path.exists(filename):
                            filename = filename[:-3] + filename[-3:].upper()
    #                    print(page.title(), dstIdx, filename, caption)
                        print(caption, page.title())
                        if not os.path.exists(filename):
                            raise FileNotFound(filename)

                        bot = upload.UploadRobot(filename, description = self.descrGeneric,
                                                 keepFilename = True,
                                                 verifyDescription = False, ignoreWarning = True)
                        filename = bot.upload_image()
                        if filename is not None and len(filename) > 0:
                            filename =  u'[[File:%(a)s|400px|thumb|center|%(b)s]]' % {"a": filename, "b": caption}

                        text = text[:dstIdx] + filename + text[dstIdx+len(key):]

                    # done in this round, prepare the next
                    dstIdx = text.find(key,dstIdx+1)
                    blkStart = blkEnd+1

#                print(text)
                if text != originalText:
                    page.put(text)

            except FileNotFound, e:
                print "File not found: '%s'" % str(e)
            except pywikibot.NoPage:
                print "Page %s does not exist?!" % page.aslink()
            except pywikibot.IsRedirectPage:
                print "Page %s is a redirect; skipping." % page.aslink()
            except pywikibot.LockedPage:
                print "Page %s is locked?!" % page.aslink()

def main():
    #page generator
    gen = None
    # If the user chooses to work on a single page, this temporary array is
    # used to read the words from the page title. The words will later be
    # joined with spaces to retrieve the full title.
    pageTitle = []
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()

    for arg in pywikibot.handleArgs():
        if not genFactory.handleArg(arg):
            pageTitle.append(arg)

    if pageTitle:
        # work on a single page
        page = pywikibot.Page(pywikibot.getSite(), ' '.join(pageTitle))
        gen = iter([page])
    if not gen:
        gen = genFactory.getCombinedGenerator()
    if not gen:
        pywikibot.showHelp('inline_images')
    else:
        preloadingGen = pagegenerators.PreloadingGenerator(gen)
        bot = SFRobot(preloadingGen)
        bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
# vim: set ts=4 sw=4 tw=0: 
