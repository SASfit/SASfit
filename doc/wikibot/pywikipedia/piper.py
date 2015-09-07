#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This is a bot that uses external filtering programs to munge the
article text, for example:

    python piper.py -filter:'tr A-Z a-z' Wikipedia:Sandbox

Would lower case the article with tr(1).

Muliple -filter commands can be specified:

    python piper.py -filter:cat -filter:'tr A-Z a-z' -filter:'tr a-z A-Z' Wikipedia:Sandbox


Would pipe the article text through cat(1) (NOOP) and then lower case
it with tr(1) and upper case it again with tr(1)

The following parameters are supported:

&params;

    -dry           If given, doesn't do any real changes, but only shows
                   what would have been changed.

    -always        Always commit changes without asking you to accept them

    -filter:       Filter the article text through this program, can be
                   given multiple times to filter through multiple programs in
                   the order which they are given

In addition all command-line parameters are passed to
genFactory.handleArg() which means pagegenerators.py arguments are
supported.

"""
#
# (C) Pywikipedia bot team, 2008-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: piper.py 8629 2010-10-09 16:11:46Z xqt $'
#

import os
import pipes
import tempfile
import wikipedia as pywikibot
import pagegenerators

# This is required for the text that is shown when you run this script
# with the parameter -help.
docuReplacements = {
    '&params;': pagegenerators.parameterHelp
}

class PiperBot:
    # Edit summary message that should be used.
    # NOTE: Put a good description here, and add translations, if possible!
    msg = {
        'ar': u'روبوت: استبدال نص المقال من خلال %s',
        'en': u'Robot: Piping the article text through %s',
        'fa': u'ربات: رد کردن متن مقاله از درون %s',
        'is': u'Vélmenni: Pípa texta síðunnar í gegnum %s',
        'nl': u'Bot: paginatekst door %s geleid'
    }

    def __init__(self, generator, debug, filters, always):
        """
        Constructor. Parameters:
            * generator - The page generator that determines on which pages
                          to work on.
            * debug     - If True, doesn't do any real changes, but only shows
                          what would have been changed.
            * always    - If True, don't prompt for changes
        """
        self.generator = generator
        self.dry = debug
        self.always = always
        self.filters = filters

    def run(self):
        # Set the edit summary message
        pipes = ', '.join(self.filters)
        pywikibot.setAction(pywikibot.translate(pywikibot.getSite(), self.msg)
                            % pipes)
        for page in self.generator:
            self.treat(page)

    def pipe(self, program, text):
        """
        Pipes a given text through a given program and returns it
        """

        text = text.encode('utf-8')

        pipe = pipes.Template()
        pipe.append(program.encode("ascii"), '--')

        # Create a temporary filename to save the piped stuff to
        tempFilename = '%s.%s' % (tempfile.mktemp(), 'txt')
        file = pipe.open(tempFilename, 'w')
        file.write(text)
        file.close()

        # Now retrieve the munged text
        mungedText = open(tempFilename, 'r').read()
        # clean up
        os.unlink(tempFilename)

        unicode_text = unicode(mungedText, 'utf-8')

        return unicode_text

    def treat(self, page):
        """
        Loads the given page, does some changes, and saves it.
        """
        try:
            # Load the page
            text = page.get()
        except pywikibot.NoPage:
            pywikibot.output(u"Page %s does not exist; skipping."
                             % page.title(asLink=True))
            return
        except pywikibot.IsRedirectPage:
            pywikibot.output(u"Page %s is a redirect; skipping."
                             % page.title(asLink=True))
            return

        # Munge!
        for program in self.filters:
            text = self.pipe(program, text);

        # only save if something was changed
        if text != page.get():
            # Show the title of the page we're working on.
            # Highlight the title in purple.
            pywikibot.output(u"\n\n>>> %s <<<" % page.title())
            # show what was changed
            pywikibot.showDiff(page.get(), text)
            if not self.dry:
                if not self.always:
                    choice = pywikibot.inputChoice(
                        u'Do you want to accept these changes?',
                        ['Yes', 'No'], ['y', 'N'], 'N')
                else:
                    choice = 'y'
                if choice == 'y':
                    try:
                        # Save the page
                        page.put(text)
                    except pywikibot.LockedPage:
                        pywikibot.output(u"Page %s is locked; skipping."
                                         % page.title(asLink=True))
                    except pywikibot.EditConflict:
                        pywikibot.output(
                            u'Skipping %s because of edit conflict'
                            % (page.title()))
                    except pywikibot.SpamfilterError, error:
                        pywikibot.output(
                            u'Cannot change %s because of spam blacklist entry %s'
                            % (page.title(), error.url))


def main():
    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()
    # The generator gives the pages that should be worked upon.
    gen = None
    # This temporary array is used to read the page title if one single
    # page to work on is specified by the arguments.
    pageTitleParts = []
    # If dry is True, doesn't do any real changes, but only show
    # what would have been changed.
    dry = False
    # will become True when the user uses the -always flag.
    always = False
    # The program to pipe stuff through
    filters = []

    # Parse command line arguments
    for arg in pywikibot.handleArgs():
        if arg.startswith("-dry"):
            dry = True
        elif arg.startswith("-filter:"):
            prog = arg[8:]
            filters.append(prog)
        elif arg.startswith("-always"):
            always = True
        else:
            # check if a standard argument like
            # -start:XYZ or -ref:Asdf was given.
            if not genFactory.handleArg(arg):
                pageTitleParts.append(arg)

    if pageTitleParts != []:
        # We will only work on a single page.
        pageTitle = ' '.join(pageTitleParts)
        page = pywikibot.Page(pywikibot.getSite(), pageTitle)
        gen = iter([page])

    if not gen:
        gen = genFactory.getCombinedGenerator()
    if gen:
        # The preloading generator is responsible for downloading multiple
        # pages from the wiki simultaneously.
        gen = pagegenerators.PreloadingGenerator(gen)
        bot = PiperBot(gen, dry, filters, always)
        bot.run()
    else:
        pywikibot.showHelp()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
