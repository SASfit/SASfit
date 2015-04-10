import difflib
__version__ = '$Id: debug.py 7128 2009-08-07 17:15:59Z siebrand $'

class Diff(object):
    hook = 'before_save'
    def __init__(self, CommonsDelinker):
        self.CommonsDelinker = CommonsDelinker
    def __call__(self, page, text, new_text, summary):
        diff = difflib.context_diff(
            text.encode('utf-8').splitlines(True),
            new_text.get().encode('utf-8').splitlines(True))

        f = open((u'diff/%s-%s-%s.txt' % (page.urlname().replace('/', '-'),
            page.site().dbName(), page.editTime())).encode('utf-8', 'ignore'), 'w')

        f.writelines(diff)
        f.close()
