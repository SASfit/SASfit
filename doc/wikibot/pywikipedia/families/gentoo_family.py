# -*- coding: utf-8  -*-

import urllib
import family, config

__version__ = '$Id: gentoo_family.py 7008 2009-07-01 06:14:26Z alexsh $'

# An inofficial Gentoo wiki project.
# Ask for permission at http://gentoo-wiki.com/Help:Bots before running a bot.
# Be very careful, and set a long throttle: "until we see it is good one edit
# ever minute and one page fetch every 30 seconds, maybe a *bit* faster later".

class Family(family.Family):

    def __init__(self):
        family.Family.__init__(self)
        self.name = 'gentoo'

        self.languages_by_size = ['en', 'de', 'es', 'fr', 'cs', 'nl', 'tr', 'ru', 'fi']
        for l in self.languages_by_size:
            self.langs[l] = '%s.gentoo-wiki.com' % l

        # TODO: sort


        # he: also uses the default 'Media'

        self.namespaces[4] = {
            '_default': u'Gentoo Linux Wiki',
        }
        self.namespaces[5] = {
            '_default': u'Gentoo Linux Wiki talk',
            'cs': u'Gentoo Linux Wiki diskuse',
            'de': u'Gentoo Linux Wiki Diskussion',
            'es': u'Gentoo Linux Wiki Discusión',
            'fi': u'Keskustelu Gentoo Linux Wikistä',
            'fr': u'Discussion Gentoo Linux Wiki',
            'nl': u'Overleg Gentoo Linux Wiki',
            'ru': u'Обсуждение Gentoo Linux Wiki',
            'tr': u'Gentoo Linux Wiki tartışma',
        }
        self.namespaces[90] = {
            '_default': u'Thread',
        }
        self.namespaces[91] = {
            '_default': u'Thread talk',
        }
        self.namespaces[92] = {
            '_default': u'Summary',
        }
        self.namespaces[93] = {
            '_default': u'Summary talk',
        }
        self.namespaces[100] = {
            '_default': u'Index',
            'tr': u'Icerik',
        }
        self.namespaces[101] = {
            '_default': u'Index Talk',
            'tr': u'Icerik Talk',
        }
        self.namespaces[102] = {
            '_default': u'Ebuild',
        }
        self.namespaces[103] = {
            '_default': u'Ebuild Talk',
        }
        self.namespaces[104] = {
            '_default': u'News',
            'tr': u'Haberler',
        }
        self.namespaces[105] = {
            '_default': u'News Talk',
            'tr': u'Haberler Talk',
        }
        self.namespaces[106] = {
            '_default': u'Man',
        }
        self.namespaces[107] = {
            '_default': u'Man Talk',
        }
        self.namespaces[110] = {
            '_default': u'Ucpt',
        }
        self.namespaces[111] = {
            '_default': u'Ucpt talk',
        }

        self.known_families.pop('gentoo-wiki')

    def version(self, code):
        return "1.16alpha"
