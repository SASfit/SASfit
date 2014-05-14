# -*- coding: utf-8  -*-

import family

class Family(family.Family):

    def __init__(self):

        family.Family.__init__(self)
        self.name = 'ubuntutw' #Set the family name; this should be the same as in the filename.

        self.langs = {
            'zh': None,
        }

        self.namespaces[-2] = {
            '_default': u'媒體',
        }

        self.namespaces[-1] = {
            '_default': u'特殊',
        }

        self.namespaces[1] = {
            '_default': u'討論',
            'zh': u'使用者討論',
        }

        self.namespaces[2] = {
            '_default': u'使用者',
        }

        self.namespaces[3] = {
            '_default': u'討論',
            'zh': u'使用者討論',
        }

        self.namespaces[4] = {
            '_default': u'Ubuntu 正體中文 Wiki',
        }

        self.namespaces[5] = {
            '_default': u'Ubuntu 正體中文 Wiki對話',
        }

        self.namespaces[6] = {
            '_default': u'圖片',
        }

        self.namespaces[7] = {
            '_default': u'圖片討論',
        }

        self.namespaces[10] = {
            '_default': u'模板',
        }

        self.namespaces[11] = {
            '_default': u'模板討論',
        }

        self.namespaces[12] = {
            '_default': u'使用說明',
        }

        self.namespaces[13] = {
            '_default': u'使用說明討論',
        }

        self.namespaces[14] = {
            '_default': u'分類',
        }

        self.namespaces[15] = {
            '_default': u'分類討論',
        }

    def hostname(self, code):
        return 'wiki.ubuntu-tw.org'

    def version(self, code):
        return "1.12.0"

    def scriptpath(self, code):
        return ''
