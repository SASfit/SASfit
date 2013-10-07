# -*- coding: utf-8  -*-

__version__ = '$Id: wikia_family.py 9052 2011-03-13 12:19:20Z xqt $'

import family

# The Wikia Search family
# user-config.py: usernames['wikia']['wikia'] = 'User name'

class Family(family.Family):
    def __init__(self):
        family.Family.__init__(self)
        self.name = u'wikia'

        self.langs = {
            u'wikia': None,
        }

        self.namespaces[4] = {
            '_default': [u'search', self.namespaces[4]['_default']],
        }
        self.namespaces[5] = {
            '_default': [u'search talk', self.namespaces[5]['_default']],
        }
        self.namespaces[100] = {
            '_default': u'Forum',
        }
        self.namespaces[101] = {
            '_default': u'Forum talk',
        }
        self.namespaces[112] = {
            '_default': u'Mini',
        }
        self.namespaces[113] = {
            '_default': u'Mini talk',
        }

    def hostname(self, code):
        return u'www.wikia.com'

    def version(self, code):
        return "1.16.2"

    def scriptpath(self, code):
        return ''

    def apipath(self, code):
        return '/api.php'
