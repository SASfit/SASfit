# -*- coding: utf-8  -*-
__version__ = '$Id: southernapproach_family.py 9071 2011-03-13 16:50:07Z xqt $'

import family, config

# ZRHwiki, formerly known as SouthernApproachWiki, a wiki about Zürich Airport.

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = 'southernapproach'
        self.langs = {
            'de':'www.zrhwiki.ch',
        }
        # Most namespaces are inherited from family.Family.
        self.namespaces[4] = {
            '_default': [u'ZRHwiki', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'ZRHwiki Diskussion', self.namespaces[5]['_default']],
        }

    def version(self, code):
        return "1.17alpha"
