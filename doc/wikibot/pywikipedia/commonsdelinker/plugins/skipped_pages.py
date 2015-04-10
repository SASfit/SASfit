__version__ = '$Id: skipped_pages.py 7128 2009-08-07 17:15:59Z siebrand $'

class SkipPages(object):
    hook = 'before_save'
    def __init__(self, CommonsDelinker):
        self.CommonsDelinker = CommonsDelinker
    def __call__(self, page, text, new_text, summary):
        site = page.site()
        if (site.lang, site.family.name) == ('en', 'wikipedia'):
            if page.title == 'Wikipedia:Sound/list':
                return False
        return True
