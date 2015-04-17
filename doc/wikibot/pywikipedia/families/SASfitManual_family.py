# -*- coding: utf-8  -*-              # REQUIRED
import config, family, urllib         # REQUIRED

class Family(family.Family):          # REQUIRED
    def __init__(self):               # REQUIRED
        family.Family.__init__(self)  # REQUIRED
        self.name = 'SASfitManual'    # REQUIRED; replace with actual name

        self.langs = {                # REQUIRED
            'en': 'sasfit.ingobressler.net'  # Include one line for each wiki in family
        }

        self.namespaces[4]['en'] = u'SASfitManual'
        self.namespaces[5]['en'] = u'SASfitManual talk'

    def protocol(self, code):
        """
        Can be overridden to return 'https'. Other protocols are not supported.
        """
        return 'http'

    def scriptpath(self, code):
        """The prefix used to locate scripts on this wiki.

        This is the value displayed when you enter {{SCRIPTPATH}} on a
        wiki page (often displayed at [[Help:Variables]] if the wiki has
        copied the master help page correctly).

        The default value is the one used on Wikimedia Foundation wikis,
        but needs to be overridden in the family file for any wiki that
        uses a different value.

        """
        return '/w'

    # IMPORTANT: if your wiki does not support the api.php interface,
    # you must uncomment the second line of this method:
    def apipath(self, code):
        # raise NotImplementedError, "%s wiki family does not support api.php" % self.name
        return '%s/api.php' % self.scriptpath(code)

    # Which version of MediaWiki is used? REQUIRED
    def version(self, code):
        # Replace with the actual version being run on your wiki
        return '1.16.4'

    def code2encoding(self, code):
        """Return the encoding for a specific language wiki"""
        # Most wikis nowadays use UTF-8, but change this if yours uses
        # a different encoding
        return 'utf-8'
