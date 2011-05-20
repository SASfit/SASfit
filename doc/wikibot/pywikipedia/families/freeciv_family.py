# -*- coding: utf-8  -*-

__version__ = '$Id: freeciv_family.py 9052 2011-03-13 12:19:20Z xqt $'

import family

# The project wiki of Freeciv, an open source strategy game.

class Family(family.Family):

    def __init__(self):
        family.Family.__init__(self)
        self.name = 'freeciv'
        self.langs = {
            'ca': 'ca.freeciv.wikia.com',
            'da': 'da.freeciv.wikia.com',
            'de': 'de.freeciv.wikia.com',
            'en': 'freeciv.wikia.com',
            'es': 'es.freeciv.wikia.com',
            'fi': 'fi.freeciv.wikia.com',
            'fr': 'fr.freeciv.wikia.com',
            'ja': 'ja.freeciv.wikia.com',
            'ru': 'ru.freeciv.wikia.com',
        }

        self.namespaces[1]['fr'] = u'Discussion'

        self.namespaces[3]['fr'] = u'Discussion utilisateur'

        self.namespaces[4] = {
            '_default': u'Freeciv',
            'fi': u'FreeCiv wiki Suomalaisille',
            'ja': u'Freeciv.org ジャパン',
        }

        self.namespaces[5] = {
            '_default': u'Freeciv talk',
            'ca': u'Freeciv Discussió',
            'da': u'Freeciv-diskussion',
            'de': u'Freeciv Diskussion',
            'es': u'Freeciv Discusión',
            'fi': u'Keskustelu FreeCiv wiki Suomalaisillesta',
            'fr': u'Discussion Freeciv',
            'ja': u'Freeciv.org ジャパン‐ノート',
            'ru': u'Обсуждение Freeciv',
        }

        self.namespaces[6]['da'] = u'Fil'

        self.namespaces[7]['da'] = u'Fildiskussion'
        self.namespaces[7]['fr'] = u'Discussion fichier'

        self.namespaces[8]['fi'] = u'Järjestelmäviesti'

        self.namespaces[11]['fr'] = u'Discussion modèle'

        self.namespaces[13]['fr'] = u'Discussion aide'

        self.namespaces[15]['fr'] = u'Discussion catégorie'

        self.namespaces[110] = {
            '_default': u'Forum',
            'fi': u'Foorumi',
            'ru': u'Форум',
        }
        self.namespaces[111] = {
            '_default': u'Forum talk',
            'fi': u'Keskustelu foorumista',
            'ru': u'Обсуждение форума',
        }

        self.namespaces[400] = {
            '_default': u'Video',
            'ru': u'Видео',
        }
        self.namespaces[401] = {
            '_default': u'Video talk',
            'ru': u'Обсуждение видео',
        }

        self.namespaces[500] = {
            '_default': u'User blog',
            'de': u'Benutzer Blog',
            'ru': u'Блог участника',
        }
        self.namespaces[501] = {
            '_default': u'User blog comment',
            'de': u'Benutzer Blog Kommentare',
            'ru': u'Комментарий блога участника',
        }

        self.namespaces[502] = {
            '_default': u'Blog',
            'ru': u'Блог',
        }
        self.namespaces[503] = {
            '_default': u'Blog talk',
            'de': u'Blog Diskussion',
            'ru': u'Обсуждение блога',
        }


    def scriptpath(self, code):
        return ''

    def version(self, code):
        return "1.16.2"
