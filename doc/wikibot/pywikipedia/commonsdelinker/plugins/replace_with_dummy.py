__version__ = '$Id: replace_with_dummy.py 7129 2009-08-07 17:25:10Z siebrand $'

import re

class DummyReplacement(object):
    # only overrides delinks inside <gallery> tags.
    hook = 'gallery_replace'
    def __init__(self, CommonsDelinker):
        self.CommonsDelinker = CommonsDelinker
    def __call__(self, page, summary, image, replacement, match, groups):
        site = page.site()
        if (site.lang, site.family.name) == ('nl', 'wikipedia') and replacement.get() is None:
            # Do not delink inside galleries of pages in categories mentioned in [[User:CommonsDelinker/DummyReplacements]]
            # but replace the target file with another file (used in for example flag or coat of arms galleries).
            # Format of the configuration page (see example on http://nl.wikipedia.org/wiki/Gebruiker:CommonsDelinker/DummyReplacements):
            # <!--begin-dummyreplace Sin escudo.svg-->
            # Amerikaans zegel
            # <!--end-dummyreplace-->
            # The above will not delink files in pages in the category "Amerikaans zegel" but replace
            # them with the file "Sin escudo.svg"

            commands = self.CommonsDelinker.SummaryCache.get(site, 'DummyReplacements', default = '')

            dummyreplacements = re.findall(r'(?s)\<\!\-\-begin\-dummyreplace (.*?)\-\-\>(.*?)\<\!\-\-end\-dummyreplace\-\-\>', commands)
            text = page.get()

            namespace = site.namespace(14)
            r_namespace = r'(?:[Cc]ategory)|(?:[%s%s]%s)' % \
                (namespace[0], namespace[0].lower(), namespace[1:])

            for new_image, categories in dummyreplacements:
                for category in categories.split('\n'):
                    if category.strip() == '': continue

                    r_cat = r'\[\[\s*%s\s*\:\s*%s\s*(?:\|.*?)?\s*\]\]' % (r_namespace,
                        re.sub(r'\\[ _]', '[ _]', re.escape(category.strip())))
                    if re.search(r_cat, text):
                        self.CommonsDelinker.output(
                            u'%s %s replaced by %s in category %s' % \
                            (self, image, new_image, category))
                        replacement.set(new_image.replace(' ', '_'))

        if (site.lang, site.family.name) == ('de', 'wikipedia') and replacement.get() is None:
            commands = self.CommonsDelinker.SummaryCache.get(site, 'DummyReplacements', default = '')

            dummyreplacements = re.findall(r'(?s)\<\!\-\-begin\-dummyreplace (.*?)\-\-\>(.*?)\<\!\-\-end\-dummyreplace\-\-\>', commands)
            text = page.get()

            namespace = site.namespace(14)
            r_namespace = r'(?:[Cc]ategory)|(?:[%s%s]%s)' % \
                (namespace[0], namespace[0].lower(), namespace[1:])

            for new_image, categories in dummyreplacements:
                for category in categories.split('\n'):
                    if category.strip() == '': continue

                    r_cat = r'\[\[\s*%s\s*\:\s*%s\s*(?:\|.*?)?\s*\]\]' % (r_namespace,
                        re.sub(r'\\[ _]', '[ _]', re.escape(category.strip())))
                    if re.search(r_cat, text):
                        self.CommonsDelinker.output(
                            u'%s %s replaced by %s in category %s' % \
                            (self, image, new_image, category))
                        replacement.set(new_image.replace(' ', '_'))
