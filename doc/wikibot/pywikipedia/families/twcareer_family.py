# -*- coding: utf-8  -*-

import family

class Family(family.Family):

    def __init__(self):

        family.Family.__init__(self)
        self.name = 'twcareer'

        self.langs = {
            'zh': None,
        }

        self.namespaces[4] = {
            '_default': u'TwCareer',
        }

        self.namespaces[5] = {
            '_default': u'TwCareer talk',
        }

        self.namespaces[6] = {
            '_default': u'Image',
        }

        self.namespaces[7] = {
            '_default': u'Image talk',
        }

    def hostname(self, code):
        return 'www.twcareer.com'

    def version(self, code):
        return "1.11.0"

    def scriptpath(self, code):
        return ''
