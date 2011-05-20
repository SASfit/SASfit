# -*- coding: utf-8  -*-

import family

# CeltIKI - The Celtic Encyclopedia

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)

        self.name = 'celtic'

        self.langs = {
                'eml': 'celtic.gdr-online.eu',
        }
        self.namespaces[4] = {
            'eml': u'CeltIKI',
        }
        self.namespaces[5] = {
            'eml': u'Discussioni CeltIKI',
        }
        self.namespaces[6] = {
            'eml': u'Immagine',
        }
        self.namespaces[7] = {
            'eml': u'Discussioni immagine',
        }


    def version(self, code):
        return "1.11.0"
