#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This robot checks copyright text in Google, Yahoo! and Live Search.

Google search requires to install the pyGoogle module from
http://pygoogle.sf.net and get a Google API license key from
http://code.google.com/apis/soapsearch/ (but since December 2006 Google is
no longer issuing new SOAP API keys).

Yahoo! search requires pYsearch module from http://pysearch.sourceforge.net
and a Yahoo AppID from http://developer.yahoo.com.

Windows Live Search requires to get an AppID from http://search.msn.com/developer
and to download/install the SOAPpy module from http://pywebsvcs.sf.net or using
SVN with the following command:

svn co http://pywebsvcs.svn.sourceforge.net/svnroot/pywebsvcs/trunk/SOAPpy/SOAPpy SOAPpy

Unlike SOAPpy version 0.12, current SVN version has no problem with Python 2.5.


You can run the bot with the following commandline parameters:

-g           - Use Google search engine
-ng          - Do not use Google
-y           - Use Yahoo! search engine
-ny          - Do not use Yahoo!
-l           - Use Windows Live Search engine
-nl          - Do not use Windows Live Search
-maxquery    - Stop after a specified number of queries for page (default: 25)
-skipquery   - Skip a number specified of queries
-output      - Append results to a specified file (default:
               'copyright/output.txt')

-text:input_text - Work on a specified text

-file        - Work on all pages given in a local text file.
               Will read any [[wiki link]] and use these articles.
               Argument can also be given as "-file:filename".
-new         - Work on the 60 newest pages. If given as -new:x, will work
               on the x newest pages.
-cat         - Work on all pages which are in a specific category.
               Argument can also be given as "-cat:categoryname".
-subcat      - When the pages to work on have been chosen by -cat, pages in
               subcategories of the selected category are also included.
               When -cat has not been selected, this has no effect.
-page        - Only check a specific page.
               Argument can also be given as "-page:pagetitle". You can give
               this parameter multiple times to check multiple pages.
-ref         - Work on all pages that link to a certain page.
               Argument can also be given as "-ref:referredpagetitle".
-filelinks   - Works on all pages that link to a certain image.
               Argument can also be given as "-filelinks:ImageName".
-links       - Work on all pages that are linked to from a certain page.
               Argument can also be given as "-links:linkingpagetitle".
-start       - Work on all pages in the wiki, starting at a given page.
-namespace:n - Number or name of namespace to process. The parameter can be used
               multiple times.

Examples:

If you want to check first 50 new articles then use this command:

    python copyright.py -new:50

If you want to check a category with no limit for number of queries to
request, use this:

    python copyright.py -cat:"Wikipedia featured articles" -maxquery:0

You can include also the text to examine directly on the command line:

    python copyright.py -text:"
    ...text...
    "
"""

#
# (C) Francesco Cosoleto, 2006
#
# Distributed under the terms of the MIT license.
#

from __future__ import generators
import re, codecs, os, time, urllib, urllib2, httplib
import wikipedia as pywikibot
import pagegenerators, config

__version__='$Id: copyright.py 9042 2011-03-13 10:14:47Z xqt $'

# Search keywords added to all the queries.
no_result_with_those_words = '-Wikipedia'

# Performing a search engine query if string length is greater than the given
# value.
min_query_string_len = 120

# Split the text into strings of a specified number of words.
number_of_words = 22

# Try to skip quoted text.
exclude_quote = True

# Enable DOTALL regular expression flag in remove_wikicode() function.
remove_wikicode_dotall = True

# If ratio between query length and number of commas is greater or equal
# to 'comma_ratio' then the script identify a comma separated list and
# don't send data to search engine.
comma_ratio = 5

# No checks if the page is a disambiguation page.
skip_disambig = True

# Parameter used in Live Search query.
# (http://msdn2.microsoft.com/en-us/library/bb266177.aspx)
region_code = 'en-US'

enable_color = True

warn_color = 'lightyellow'
error_color = 'lightred'

appdir = "copyright"
output_file = pywikibot.config.datafilepath(appdir, "output.txt")

pages_for_exclusion_database = [
    ('it', 'Wikipedia:Sospette violazioni di copyright/Lista di esclusione',
           'exclusion_list.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Abc', 'Abc.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Def', 'Def.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Ghi', 'Ghi.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Jkl', 'Jkl.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Mno', 'Mno.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Pqr', 'Pqr.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Stu', 'Stu.txt'),
    ('en', 'Wikipedia:Mirrors_and_forks/Vwxyz', 'Vwxyz.txt'),
    ('es', 'Wikipedia:Espejos de Wikipedia/Espejos_que_cumplen_la_GFDL_y_CC-BY-SA', 'Espejos.txt'),
    #('de', 'Wikipedia:Weiternutzung', 'Weiternutzung.txt'),
    ('it', 'Wikipedia:Cloni', 'Cloni.txt'),
    #('pl', 'Wikipedia:Mirrory_i_forki_polskiej_Wikipedii', 'Mirrory_i_forki_polskiej_Wikipedii.txt'),
    #('pt', 'Wikipedia:Clones_da_Wikipédia', 'Clones_da_Wikipédia.txt'),
    #('sv', 'Wikipedia:Spegelsidor', 'Spegelsidor.txt'),
]

reports_cat = {
    'it': u'Segnalazioni automatiche sospetti problemi di copyright',
    'es': u'Wikipedia:Páginas para revisar en busca de posible violación de copyright',
}

wikipedia_names = {
    '--': u'Wikipedia',
    'am': u'ዊኪፔድያ',
    'an': u'Biquipedia',
    'ang': u'Wicipǣdia',
    'ar': u'ويكيبيديا',
    'arc': u'ܘܝܟܝܦܕܝܐ',
    'ast': u'Uiquipedia',
    'az': u'Vikipediya',
    'bat-smg': u'Vikipedėjė',
    'be': u'Вікіпэдыя',
    'be-x-old': u'Вікіпэдыя',
    'bg': u'Уикипедия',
    'bn': u'উইকিপিডিয়া',
    'bpy': u'উইকিপিডিয়া',
    'ca': u'Viquipèdia',
    'ceb': u'Wikipedya',
    'chr': u'ᏫᎩᏇᏗᏯ',
    'cr': u'ᐎᑭᐱᑎᔭ',
    'cs': u'Wikipedie',
    'csb': u'Wikipedijô',
    'cu': u'Википедї',
    'cv': u'Википеди',
    'cy': u'Wicipedia',
    'diq': u'Wikipediya',
    'dv': u'ވިކިޕީޑިއާ',
    'el': u'Βικιπαίδεια',
    'eo': u'Vikipedio',
    'et': u'Vikipeedia',
    'fa': u'ویکی‌پدیا',
    'fiu-vro': u'Vikipeediä',
    'fr': u'Wikipédia',
    'frp': u'Vuiquipèdia',
    'fur': u'Vichipedie',
    'fy': u'Wikipedy',
    'ga': u'Vicipéid',
    'gu': u'વિકિપીડિયા',
    'he': u'ויקיפדיה',
    'hi': u'विकिपीडिया',
    'hr': u'Wikipedija',
    'hsb': u'Wikipedija',
    'hu': u'Wikipédia',
    'hy': u'Վիքիփեդիա',
    'io': u'Wikipedio',
    'iu': u'ᐅᐃᑭᐱᑎᐊ/oikipitia',
    'ja': u'ウィキペディア',
    'jbo': u'uikipedias',
    'ka': u'ვიკიპედია',
    'kk': u'Уикипедия',
    'kn': u'ವಿಕಿಪೀಡಿಯ',
    'ko': u'위키백과',
    'ksh': u'Wikkipedija',
    'la': u'Vicipaedia',
    'lad': u'ויקיפידיה',
    'lt': u'Vikipedija',
    'lv': u'Vikipēdija',
    'mk': u'Википедија',
    'ml': u'വിക്കിപീഡിയ',
    'mo': u'Википедия',
    'mr': u'विकिपिडीया',
    'mt': u'Wikipedija',
    'nah': u'Huiquipedia',
    'ne': u'विकिपीडिया',
    'nrm': u'Viqùipédie',
    'oc': u'Wikipèdia',
    'os': u'Википеди',
    'pa': u'ਵਿਕਿਪੀਡਿਆ',
    'pt': u'Wikipédia',
    'qu': u'Wikipidiya',
    'rmy': u'Vikipidiya',
    'ru': u'Википедия',
    'sco': u'Wikipaedia',
    'si': u'විකිපීඩියා',
    'sk': u'Wikipédia',
    'sl': u'Wikipedija',
    'sr': u'Википедија',
    'su': u'Wikipédia',
    'ta': u'விக்கிபீடியா',
    'tg': u'Википедиа',
    'th': u'วิกิพีเดีย',
    'tr': u'Vikipedi',
    'uk': u'Вікіпедія',
    'uz': u'Vikipediya',
    'yi': u'‫װיקיפעדיע',
    'zh': u'维基百科',
    'zh-classical': u'維基大典',
    'zh-yue': u'維基百科',
}

editsection_names = {
    'ar': u'\[عدل\]',
    'en': u'\[edit\]',
    'fa': u'\[ویرایش\]',
    'fr': u'\[modifier\]',
    'de': u'\[Bearbeiten\]',
    'es,pt': u'\[editar\]',
    'it': u'\[modifica\]',
    'ja': u'\[編集\]',
    'zh': u'\[编辑\]',
}

sections_to_skip = {
    'ar': [u'مراجع', u'قراءة أخرى', u'ملاحظات', u'وصلات خارجية'],
    'en': [u'References', u'Further reading', u'Citations', u'External links'],
    'fa': [u'منابع', u'منابع برای مطالعه بیشتر', u'یادکردها',
           u'پیوند به بیرون'],
    'es': [u'Referencias', u'Ver también', u'Bibliografía', u'Enlaces externos',
           u'Notas'],
    'fr': [u'Liens externes'],
    'it': [u'Bibliografia', u'Discografia', u'Opere bibliografiche',
           u'Riferimenti bibliografici', u'Collegamenti esterni',
           u'Pubblicazioni', u'Pubblicazioni principali',
           u'Bibliografia parziale'],
    'ja': [u'脚注', u'脚注欄', u'脚注・出典', u'出典', u'注釈'],
    'zh': [u'參考文獻', u'参考文献', u'參考資料', u'参考资料', u'資料來源', u'资料来源',
           u'參見', u'参见', u'參閱', u'参阅'],
}

if enable_color:
    warn_color = '\03{%s}' % warn_color
    error_color = '\03{%s}' % error_color
    default_color = '\03{default}'
else:
    warn_color = '' ; error_color = '' ; default_color = ''

def _output(text, prefix = None, color = ''):
    if prefix:
        pywikibot.output('%s%s: %s%s' % (color, prefix, default_color, text))
    else:
        pywikibot.output('%s%s' % (color, text))

def warn(text, prefix = None):
    _output(text, prefix = prefix, color = warn_color)

def error(text ,prefix = None):
    _output(text, prefix = prefix, color = error_color)

def skip_section(text):
    sect_titles = '|'.join(sections_to_skip[pywikibot.getSite().lang])
    sectC = re.compile('(?mi)^==\s*(' + sect_titles + ')\s*==')
    while True:
        newtext = cut_section(text, sectC)
        if newtext == text:
            break
        text = newtext
    return text

def cut_section(text, sectC):
    sectendC = re.compile('(?m)^==[^=]')
    start = sectC.search(text)
    if start:
        end = sectendC.search(text, start.end())
        if end:
            return text[:start.start()]+text[end.start():]
        else:
            return text[:start.start()]
    return text

class URLExclusion:
    def __init__(self):
        self.URLlist = set()
        self.scan()

    def pages_list(self):
        for i in pages_for_exclusion_database:
            path = pywikibot.config.datafilepath(appdir, i[0], i[2])
            pywikibot.config.makepath(path)
            page = pywikibot.Page(pywikibot.getSite(i[0]), i[1])
            yield page, path

    def download(self, force_update = False):
        for page, path in self.pages_list():
            download = force_update
            try:
                if not os.path.exists(path):
                    print 'Creating file \'%s\' (%s)' % (pywikibot.config.shortpath(path),
                                                         page.aslink())
                    download = True
                else:
                    file_age = time.time() - os.path.getmtime(path)
                    if download or file_age > 24 * 60 * 60:
                        print 'Updating file \'%s\' (%s)' % (
                        pywikibot.config.shortpath(path), page.aslink())
                        download = True
            except OSError:
                raise

            if download:
                data = None
                try:
                    data = page.get()
                except KeyboardInterrupt:
                    raise
                except pywikibot.IsRedirectPage:
                    data = page.getRedirectTarget().get()
                except:
                    error('Getting page failed')

                if data:
                    f = codecs.open(path, 'w', 'utf-8')
                    f.write(data)
                    f.close()

    def update(self):
        self.download(force_update = True)
        self.scan()

    def check(self, url, verbose = False):
        for entry in self.URLlist:
           if entry in url:
               if verbose > 1:
                   warn('URL Excluded: %s\nReason: %s' % (url, entry))
               elif verbose:
                   warn('URL Excluded: %s' % url)
               return True
        return False

    def scan(self):
        prelist = [] ; result_list = []
        self.download()

        for page, path in self.pages_list():
            if 'exclusion_list.txt' in path:
                result_list += re.sub("</?pre>","",
                                      read_file(path,
                                                cut_comment=True,
                                                cut_newlines=True)
                                      ).splitlines()
            else:
                data = read_file(path)
                # wikipedia:en:Wikipedia:Mirrors and forks
                prelist += re.findall("(?i)url\s*=\s*<nowiki>(?:http://)?(.*)</nowiki>", data)
                prelist += re.findall("(?i)\*\s*Site:\s*\[?(?:http://)?(.*)\]?", data)
                # wikipedia:it:Wikipedia:Cloni
                if 'it/Cloni.txt' in path:
                    prelist += re.findall('(?mi)^==(?!=)\s*\[?\s*(?:<nowiki>)?\s*(?:http://)?(.*?)(?:</nowiki>)?\s*\]?\s*==', data)
        list1 = []
        for entry in prelist:
            list1 += entry.split(", ")
        list2 = []
        for entry in list1:
            list2 += entry.split("and ")
        for entry in list2:
            # Remove unnecessary part of URL
            entry = re.sub("(http://|www\.)", "", entry)
            entry = re.sub("</?nowiki>", "", entry)
            if entry:
                if '/' in entry:
                    entry = entry[:entry.rfind('/')]

                entry = re.sub("\s.*", "", entry)

                if len(entry) > 4:
                    result_list.append(entry)

        result_list += read_file(
                            pywikibot.config.datafilepath(appdir, 'exclusion_list.txt'),
                            cut_comment = True, cut_newlines = True
                       ).splitlines()

        for item in result_list:
            cleaned = item.strip()
            if cleaned:
                self.URLlist.add(cleaned)

    def sanity_check(self):
        print "Exclusion list sanity check..."
        for entry in self.URLlist:
            if (not '.' in entry and not '/' in entry) or len(entry) < 5:
                print "** " + entry

    def dump(self):
        f = open(pywikibot.config.datafilepath(appdir, 'exclusion_list.dump'), 'w')
        f.write('\n'.join(self.URLlist))
        f.close()
        print "Exclusion list dump saved."


def read_file(filename, cut_comment = False, cut_newlines = False):
    text = u""

    f = codecs.open(filename, 'r','utf-8')
    text = f.read()
    f.close()

    if cut_comment:
        text = re.sub(" ?#.*", "", text)

    if cut_newlines:
        text = re.sub("(?m)^\r?\n", "", text)

    return text

def write_log(text, filename = output_file):
    f = codecs.open(filename, 'a', 'utf-8')
    f.write(text)
    f.close()

#
# Ignore text that contents comma separated list, only numbers,
# punctuation...

def economize_query(text):
    # Comma separated list
    c = text.count(', ')
    if c > 4:
        l = len(text)
        r = 100 * float(c) / l

        #if r >= 4 and r < 7:
        #    write_log("%d/%d/%d: %s\n" % (l,c,r,text), "copyright/skip_%s.txt" % ("%0.1f" % r))

        if r >= comma_ratio:
            return True

    # Numbers
    if re.search('[^0-9\'*/,. +?:;-]{5}', text):
        return False
    return True

#
# Set regex used in remove_wikicode() to remove [[Image:]] tags
# and regex used in check_in_source() to reject pages with
# 'Wikipedia'.

def join_family_data(reString, namespace):
    for s in pywikibot.Family().namespaces[namespace].itervalues():
        if type (s) == list:
            for e in s:
                reString += '|' + e
        else:
            reString += '|' + s
    return '\s*(' + reString + ')\s*'

reImageC = re.compile('\[\[' + join_family_data('Image', 6) + ':.*?\]\]', re.I)
reWikipediaC = re.compile('(' + '|'.join(wikipedia_names.values()) + ')', re.I)
reSectionNamesC = re.compile('(' + '|'.join(editsection_names.values()) + ')')

def remove_wikicode(text, re_dotall = False, remove_quote = exclude_quote, debug = False):
    if not text:
        return ""

    if debug:
        write_log(text+'\n', "copyright/wikicode.txt")

    text = re.sub('(?i)</?(p|u|i|b|em|div|span|font|small|big|code|tt).*?>', '', text)
    text = re.sub('(?i)<(/\s*)?br(\s*/)?>', '', text)
    text = re.sub('<!--.*?-->', '', text)

    text = text.replace('&lt;', '<')
    text = text.replace('&gt;', '>')

    # remove URL
    text = re.sub('(ftp|https?)://[\w/.,;:@&=%#\\\?_!~*\'|()\"+-]+', ' ', text)

    # remove Image tags
    text = reImageC.sub("", text)

    # replace piped wikilink
    text = re.sub("\[\[[^\]]*?\|(.*?)\]\]", "\\1", text)

    # remove unicode and polytonic template
    text = re.sub("(?i){{(unicode|polytonic)\|(.*?)}}", "\\1", text)

    if re_dotall:
       flags = "(?xsim)"
       # exclude wikitable
       text = re.sub('(?s){\|.*?^\|}', '', text)
    else:
       flags = "(?xim)"

    text = re.sub("""
    %s
    (
        <ref[^>]*?\s*/\s*>     | # exclude <ref name = '' / > tags
        <ref.*?>.*?</ref>      | # exclude <ref> notes
        ^[\ \t]*({\||[|!]).*?$ | # exclude wikitable
        </*nowiki>             | # remove <nowiki> tags
        {{.*?}}                | # remove (not nested) template
        <math>.*?</math>       | # remove LaTeX staff
        [\[\]]                 | # remove [, ]
        ^[*:;]+                | # remove *, :, ; in begin of line
        <!--                   |
        -->                    |
    )
    """ % flags, "", text)

    if remove_quote:
        # '' text ''
        # '' text ''.
        # '' text '' (text)
        # « text »
        # ...
        #

        italic_quoteC = re.compile("(?m)^[:*]?\s*(''.*?'')\.?\s*(\(.*?\))?\r?$")

        index = 0
        try:
            import pywikiparser
        except ImportError:
            pywikiparser = False

        while pywikiparser:
            m = italic_quoteC.search(text, index)
            if not m:
                break

            s = pywikiparser.Parser(m.group(1))

            try:
                xmldata = s.parse().toxml()
                if '<wikipage><p><i>' in xmldata and '</i></p></wikipage>' in xmldata:
                    if xmldata.count('<i>') == 1:
                        text = text[:m.start()] + text[m.end():]
            except:
                pass

            index = m.start() + 1

        text = re.sub('(?m)^[:*]*\s*["][^"]+["]\.?\s*(\(.*?\))?\r?$', "", text)
        text = re.sub('(?m)^[:*]*\s*[«][^»]+[»]\.?\s*(\(.*?\))?\r?$', "", text)
        text = re.sub('(?m)^[:*]*\s*[“][^”]+[”]\.?\s*(\(.*?\))?\r?$', "", text)

    # remove useless spaces
    text = re.sub("(?m)(^[ \t]+|[ \t]+\r?$)", "", text)

    if debug:
        write_log(text+'\n', "copyright/wikicode_removed.txt")

    return text

def n_index(text, n, sep):
    pos = 0
    while n>0:
        try:
            pos = text.index(sep, pos + 1)
            n -= 1
        except ValueError:
            return 0
    return pos

def mysplit(text, dim, sep):
    if not sep in text:
        return [text]
    t = text
    l = list()
    while t:
        if sep in t:
            n = n_index(t, dim, sep)
            if n>0:
                l.append(t[:n])
                t = t[n+1:]
                continue
        l.append(t)
        break
    return l

class SearchEngine:

    num_google_queries = num_yahoo_queries = num_msn_queries = 0

    def __init__(self):
        self.URLexcl = URLExclusion()

    def __del__(self):
        self.print_stats()

    def query(self, lines = [], max_query_len = 1300, wikicode = True):
        # Google max_query_len = 1480?
        # - '-Wikipedia ""' = 1467

        # Google limit queries to 32 words.

        n_query = 0
        output = unicode()
        previous_group_url = 'null'

        for line in lines:
            if wikicode:
                line = remove_wikicode(line)
            for search_words in mysplit(line, number_of_words, " "):
                if len(search_words) > min_query_string_len:
                    if config.copyright_economize_query:
                        if economize_query(search_words):
                            warn(search_words, prefix = 'Text excluded')
                            consecutive = False
                            continue
                    n_query += 1
                    #pywikibot.output(search_words)
                    if config.copyright_max_query_for_page and n_query > config.copyright_max_query_for_page:
                        warn(u"Max query limit for page reached")
                        return output
                    if config.copyright_skip_query > n_query:
                        continue
                    if len(search_words) > max_query_len:
                        search_words = search_words[:max_query_len]
                        consecutive = False
                        if " " in search_words:
                             search_words = search_words[:search_words.rindex(" ")]

                    results = self.get_results(search_words)

                    group_url = '' ; cmp_group_url = ''

                    for url, engine, comment in results:
                        if comment:
                            group_url += '\n*%s - %s (%s)' % (engine, url, "; ".join(comment))
                        else:
                            group_url += '\n*%s - %s' % (engine, url)
                        cmp_group_url += '\n*%s - %s' % (engine, url)
                    if results:
                        group_url_list = group_url.splitlines()
                        cmp_group_url_list = cmp_group_url.splitlines()
                        group_url_list.sort()
                        cmp_group_url_list.sort()
                        group_url = '\n'.join(group_url_list)
                        cmp_group_url = '\n'.join(cmp_group_url_list)
                        if previous_group_url == cmp_group_url:
                            if consecutive:
                                output += ' ' + search_words
                            else:
                                output += '\n**' + search_words
                        else:
                            output += group_url + '\n**' + search_words

                        previous_group_url = cmp_group_url
                        consecutive = True
                    else:
                        consecutive = False
                else:
                   consecutive = False

        return output

    def add_in_urllist(self, url, add_item, engine, cache_url = None):
        if (engine == 'google' and config.copyright_check_in_source_google) or \
        (engine == 'yahoo' and config.copyright_check_in_source_yahoo) or \
        (engine == 'msn' and config.copyright_check_in_source_msn):
            check_in_source = True
        else:
            check_in_source = False

        if check_in_source or config.copyright_show_date or config.copyright_show_length:
            s = None
            cache = False

            # list to store date, length, cache URL
            comment = list()

            try:
                s = WebPage(add_item, self.URLexcl)
            except URL_exclusion:
                pass
            except NoWebPage:
                cache = True

            if s:
                # Before of add url in result list, perform the check in source
                if check_in_source:
                    if s.check_in_source():
                        return

                if config.copyright_show_date:
                    date = s.lastmodified()
                    if date:
                        if date[:3] != time.localtime()[:3]:
                            comment.append("%s/%s/%s" % (date[2], date[1], date[0]))

                unit = 'bytes'

                if config.copyright_show_length:
                    length = s.length()
                    if length > 1024:
                        # convert in kilobyte
                        length /= 1024
                        unit = 'KB'
                        if length > 1024:
                            # convert in megabyte
                            length /= 1024
                            unit = 'MB'
                    if length > 0:
                        comment.append("%d %s" % (length, unit))

            if cache:
                if cache_url:
                    if engine == 'google':
                        comment.append('[http://www.google.com/search?sourceid=navclient&q=cache:%s Google cache]' % urllib.quote(short_url(add_item)))
                    elif engine == 'yahoo':
                        #cache = False
                        #comment.append('[%s Yahoo cache]' % re.sub('&appid=[^&]*','', urllib2.unquote(cache_url)))
                        comment.append("''Yahoo cache''")
                    elif engine == 'msn':
                        comment.append('[%s Live cache]' % re.sub('&lang=[^&]*','', cache_url))
                else:
                    comment.append('[http://web.archive.org/*/%s archive.org]' % short_url(add_item))

        for i in range(len(url)):
            if add_item in url[i]:
                if engine not in url[i][1]:
                    if url[i][2]:
                        comment = url[i][2]
                    url[i] = (add_item, url[i][1] + ', ' + engine, comment)
                return
        url.append((add_item, engine, comment))
        return

    def soap(self, engine, query, url, numresults = 10):
        print "  %s query..." % engine.capitalize()
        search_request_retry = config.copyright_connection_tries
        query_success = False

        while search_request_retry:
            try:
                if engine == 'google':
                    import google
                    google.LICENSE_KEY = config.google_key
                    data = google.doGoogleSearch('%s "%s"' % (no_result_with_those_words, query))
                    for entry in data.results:
                       self.add_in_urllist(url, entry.URL, 'google', entry.cachedSize)

                    self.num_google_queries += 1

                elif engine == 'yahoo':
                    import yahoo.search.web
                    data = yahoo.search.web.WebSearch(config.yahoo_appid, query='"%s" %s' % (
                                                      query.encode('utf_8'),
                                                      no_result_with_those_words
                                                     ), results = numresults)
                    for entry in data.parse_results():
                        cacheurl = None
                        if entry.Cache:
                            cacheurl = entry.Cache.Url
                        self.add_in_urllist(url, entry.Url, 'yahoo', cacheurl)

                    self.num_yahoo_queries += 1

                elif engine == 'msn':
                    #max_query_len = 150?
                    from SOAPpy import WSDL

                    try:
                        server = WSDL.Proxy('http://soap.search.msn.com/webservices.asmx?wsdl')
                    except Exception, err:
                        error("Live Search Error: %s" % err)
                        raise

                    params = {'AppID': config.msn_appid, 'Query': '%s "%s"' % (no_result_with_those_words, query),
                             'CultureInfo': region_code, 'SafeSearch': 'Off', 'Requests': {
                             'SourceRequest':{'Source': 'Web', 'Offset': 0, 'Count': 10, 'ResultFields': 'All',}}}

                    results = ''

                    server_results = server.Search(Request = params)
                    if server_results.Responses[0].Results:
                        results = server_results.Responses[0].Results[0]
                    if results:
                        # list or instance?
                        if type(results) == list:
                            for entry in results:
                                cacheurl = None
                                if hasattr(entry, 'CacheUrl'):
                                    cacheurl = entry.CacheUrl
                                self.add_in_urllist(url, entry.Url, 'msn', cacheurl)
                        else:
                            cacheurl = None
                            if hasattr(results, 'CacheUrl'):
                                cacheurl = results.CacheUrl
                            self.add_in_urllist(url, results.Url, 'msn', cacheurl)

                    self.num_msn_queries += 1

                search_request_retry = 0
                query_success = True
            except KeyboardInterrupt:
                raise
            except Exception, err:
                # Something is going wrong...
                if 'Daily limit' in str(err) or 'Insufficient quota for key' in str(err):
                    exceeded_in_queries('google')
                elif 'limit exceeded' in str(err):
                    exceeded_in_queries('yahoo')
                elif 'Invalid value for AppID in request' in str(err):
                    exceeded_in_queries('msn')
                else:
                    error(err, "Got an error")

                if search_request_retry:
                    search_request_retry -= 1

        if not query_success:
            error('No response for: %s' % query, "Error (%s)" % engine)

    def get_results(self, query, numresults = 10):
        result_list = list()
        query = re.sub("[()\"<>]", "", query)
        # pywikibot.output(query)
        if config.copyright_google:
            self.soap('google', query, result_list)
        if config.copyright_yahoo:
            self.soap('yahoo', query, result_list, numresults = numresults)
        if config.copyright_msn:
            self.soap('msn', query, result_list)

        offset = 0
        for i in range(len(result_list)):
            if self.URLexcl.check(result_list[i + offset][0], verbose = True):
                result_list.pop(i + offset)
                offset += -1
        return result_list

    def print_stats(self):
        pywikibot.output('\n'
                         'Search engine | number of queries\n'
                         '---------------------------------\n'
                         'Google        | %s\n'
                         'Yahoo!        | %s\n'
                         'Live Search   | %s\n'
                         % (self.num_google_queries, self.num_yahoo_queries,
                            self.num_msn_queries))

source_seen = set()
positive_source_seen = set()

class NoWebPage(Exception):
    """Web page does not exist (404)"""

class URL_exclusion(Exception):
    """URL in exclusion list"""


class WebPage(object):
    """
    """

    def __init__(self, url, URLExcl):
        global source_seen
        self.URLexcludedlist = URLExcl.URLlist

        if url in source_seen or URLExcl.check(url):
            raise URL_exclusion

        self._url = url

        try:
            self._urldata = urllib2.urlopen(urllib2.Request(self._url, None, { 'User-Agent': pywikibot.useragent }))
        #except httplib.BadStatusLine, line:
        #    print 'URL: %s\nBad status line: %s' % (url, line)
        except urllib2.HTTPError, err:
            error("HTTP error: %d / %s (%s)" % (err.code, err.msg, url))
            if err.code >= 400:
                source_seen.add(self._url)
                raise NoWebPage
            return None
        except urllib2.URLError, arg:
            error("URL error: %s / %s" % (url, arg))
            return None
        except Exception, err:
            error("ERROR: %s" % (err))

        self._lastmodified = self._urldata.info().getdate('Last-Modified')
        self._length = self._urldata.info().getheader('Content-Length')
        self._content_type = self._urldata.info().getheader('Content-Type')

    def length(self):
         if hasattr(self, '_length'):
             if self._length:
                 return int(self._length)
         if hasattr(self, '_contents'):
             return len(self._contents)

         # print "No length for " + self._url

         return None

    def lastmodified(self):
         if hasattr(self, '_lastmodified'):
             return self._lastmodified
         return None

    def get(self, force = False):
        # Exclude URL with listed file extension.
        if self._url[-4:] in [".pdf", ".doc", ".ppt"]:
            raise URL_exclusion

        # Make sure we did try to get the contents once
        if not hasattr(self, '_contents'):
            self._contents = self._urldata.read()

        return self._contents

    def check_regexp(self, reC, text, filename = None):
        m = reC.search(text)
        if m:
            global positive_source_seen
            self.URLexcludedlist.add(self._url)
            positive_source_seen.add(self._url)
            if filename:
                write_log("%s (%s)\n" % (self._url, m.group()), filename)
            return True

    def check_in_source(self):
        """ Sources may be different from search engine database and include
        mentions of Wikipedia. This function avoid also errors in search results
        that can occurs either with Google and Yahoo! service.

        """
        global source_seen

        if not hasattr(self, '_urldata'):
            return False

        if self._url in positive_source_seen:
            return True

        if self._url in source_seen:
            return False

        try:
            text = self.get()
        except URL_exclusion:
            return False

        # Character encoding conversion if 'Content-Type' field has
        # charset attribute set to UTF-8.

        if text:
            if 'utf-8' in self._content_type.lower():
                text = text.decode("utf-8", 'replace')
            else:
                # <META> declaration with "http-equiv" set to "Content-Type" in HTML document.
                if 'text/html' in self._content_type and (re.search("(?is)<meta\s.*?charset\s*=\s*[\"\']*\s*UTF-8.*?>", text) or re.search("(?is)<\?.*?encoding\s*=\s*[\"\']*\s*UTF-8.*?\?>", text)):
                    text = text.decode("utf-8", 'replace')

            if config.copyright_check_in_source_section_names:
                if self.check_regexp(reSectionNamesC, text, "copyright/sites_with_'[edit]'.txt"):
                    return True

            if self.check_regexp(reWikipediaC, text, "copyright/sites_with_'wikipedia'.txt"):
                return True

        source_seen.add(self._url)
        return False


def exceeded_in_queries(engine):
    """Behavior if an exceeded error occur."""

    # Disable search engine
    if config.copyright_exceeded_in_queries == 1:
        exec('config.copyright_' + engine + ' = False')
    # Sleeping
    if config.copyright_exceeded_in_queries == 2:
        error("Got a queries exceeded error from %s. Sleeping for %d hours..." % (engine.capitalize(), config.copyright_exceeded_in_queries_sleep_hours))
        time.sleep(config.copyright_exceeded_in_queries_sleep_hours * 60 * 60)
    # Stop execution
    if config.copyright_exceeded_in_queries == 3:
        raise 'Got a queries exceeded error.'

def get_by_id(title, id):
    return pywikibot.getSite().getUrl("/w/index.php?title=%s&oldid=%s&action=raw" % (title, id))

def checks_by_ids(ids):
    for title, id in ids:
        original_text = get_by_id(title, id)
        if original_text:
            pywikibot.output(original_text)
            output = query(lines=original_text.splitlines())
            if output:
                write_log(
                    "=== [[" + title + "]] ===\n{{botbox|%s|prev|%s|%s|00}}"
                        % (title.replace(" ", "_").replace("\"", "%22"),
                           id, "author")
                        + output,
                    pywikibot.config.datafilepath(appdir, "ID_output.txt"))


class CheckRobot:
    def __init__(self, generator):
        self.generator = generator
        self.SearchEngine = SearchEngine()

    def run(self):
        for page in self.generator:
            try:
                original_text = page.get()
            except pywikibot.NoPage:
                pywikibot.output(u'Page %s not found' % page.title())
                continue
            except pywikibot.IsRedirectPage:
                newpage = page.getRedirectTarget()
                pywikibot.output(u'Page %s redirects to \'%s\'' % (page.aslink(), newpage.title()))
                bot = CheckRobot(iter([newpage,]))
                bot.run()
                continue
            except pywikibot.SectionError:
                error("Page %s has no section %s" % (page.title(), page.section()))
                continue

            if skip_disambig:
                if page.isDisambig():
                    pywikibot.output(u'Page %s is a disambiguation page' % page.aslink())
                    continue

            pywikibot.output(page.title())

            if original_text:
                text = skip_section(original_text)

                if remove_wikicode_dotall:
                    text = remove_wikicode(text, re_dotall = True)

                output = self.SearchEngine.query(lines = text.splitlines(), wikicode = not remove_wikicode_dotall)
                if output:
                   write_log('=== [[' + page.title() + ']] ===' + output + '\n',
                             filename = output_file)


def short_url(url):
    return url[url.index('://')+3:]

def put(page, text, comment):
    while True:
        try:
            page.put(text, comment = comment)
            break
        except pywikibot.SpamfilterError, url:
            warn(url, prefix = "Spam filter")
            text = re.sub(url[0], '<blacklist>' + short_url(url[0]), text)
        except pywikibot.EditConflict:
            warn("Edit conflict")
            raise pywikibot.EditConflict

def check_config(var, license_id, license_name):
    if var:
        if not license_id:
            warn(u"You don't have set a " + license_name + ", search engine is disabled.",
                 prefix = "WARNING")
            return False
    return var

def setSavepath(path):
    global output_file
    output_file = path

def main():
    gen = None
    # pages which will be processed when the -page parameter is used
    PageTitles = []
    # IDs which will be processed when the -ids parameter is used
    ids = None
    # Which namespaces should be processed?
    # default to [] which means all namespaces will be processed
    namespaces = []
    #
    repeat = False
    #
    text = None
    # Number of pages to load at a time by Preload generator
    pageNumber = 40
    # Default number of pages for NewPages generator
    number = 60

    # This factory is responsible for processing command line arguments
    # that are also used by other scripts and that determine on which pages
    # to work on.
    genFactory = pagegenerators.GeneratorFactory()

    # Read commandline parameters.
    for arg in pywikibot.handleArgs():
        if arg == '-y':
            config.copyright_yahoo = True
        elif arg == '-g':
            config.copyright_google = True
        elif arg == '-l':
            config.copyright_msn = True
        elif arg == '-ny':
            config.copyright_yahoo = False
        elif arg == '-ng':
            config.copyright_google = False
        elif arg == '-nl':
            config.copyright_msn = False
        elif arg.startswith('-output'):
            if len(arg) >= 8:
                setSavepath(arg[8:])
        elif arg.startswith('-maxquery'):
            if len(arg) >= 10:
                config.copyright_max_query_for_page = int(arg[10:])
        elif arg.startswith('-skipquery'):
            if len(arg) >= 11:
                config.copyright_skip_query = int(arg[11:])
        elif arg.startswith('-nwords'):
            if len(arg) >= 8:
                number_of_words = int(arg[8:])
        elif arg.startswith('-text'):
            if len(arg) >= 6:
              text = arg[6:]
        elif arg.startswith('-page'):
            if len(arg) == 5:
                PageTitles.append(pywikibot.input(u'Which page do you want to change?'))
            else:
                PageTitles.append(arg[6:])
        elif arg.startswith('-namespace:'):
            try:
                namespaces.append(int(arg[11:]))
            except ValueError:
                namespaces.append(arg[11:])
        elif arg.startswith('-forceupdate'):
            URLExclusion().update()
        elif arg == '-repeat':
            repeat = True
        elif arg.startswith('-new'):
            if len(arg) >=5:
              number = int(arg[5:])
            gen = pagegenerators.NewpagesPageGenerator(number = number, repeat = repeat)
            # Preload generator work better if 'pageNumber' is not major than 'number',
            # this avoid unnecessary delay.
            if number < pageNumber:
                pageNumber = number
        else:
            genFactory.handleArg(arg)

    if PageTitles:
        pages = [pywikibot.Page(pywikibot.getSite(), PageTitle) for PageTitle in PageTitles]
        gen = iter(pages)

    config.copyright_yahoo = check_config(config.copyright_yahoo, config.yahoo_appid, "Yahoo AppID")
    config.copyright_google = check_config(config.copyright_google, config.google_key, "Google Web API license key")
    config.copyright_msn = check_config(config.copyright_msn, config.msn_appid, "Live Search AppID")

    if ids:
        checks_by_ids(ids)

    if not gen:
        gen = genFactory.getCombinedGenerator()
    if not gen and not ids and not text:
        # syntax error, show help text from the top of this file
        pywikibot.output(__doc__, 'utf-8')

    if text:
        output = SearchEngine().query(lines = text.splitlines())
        if output:
            pywikibot.output(output)

    if not gen:
        return
    if namespaces != []:
        gen =  pagegenerators.NamespaceFilterPageGenerator(gen, namespaces)
    preloadingGen = pagegenerators.PreloadingGenerator(gen, pageNumber = pageNumber)
    bot = CheckRobot(preloadingGen)
    bot.run()

if number_of_words > 22 and config.copyright_msn:
        warn("Live Search requires a lower value for 'number_of_words' variable "
             "(current value is %d, a good value may be 22)." % (number_of_words), prefix = 'Warning')

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
