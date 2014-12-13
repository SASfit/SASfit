__version__ = '$Id: books.py 7128 2009-08-07 17:15:59Z siebrand $'

import re

class FrPhotographie(object):
    hook = 'before_replace'
    def __init__(self, CommonsDelinker):
        self.CommonsDelinker = CommonsDelinker
    def __call__(self, page, summary, image, replacement):
        site = page.site()
        if (site.lang, site.family.name) == ('fr', 'wikibooks') and replacement.get() is None:
            if page.title().startswith('Photographie/') or page.title().startswith('Tribologie/'):
                replacement.set('IMG.svg')
                self.CommonsDelinker.output(u'%s Replaced %s by IMG.svg on %s.' % \
                    (self, image.get(), replacement.get()))
