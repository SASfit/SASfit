# -*- coding: utf-8  -*-
"""
"""
__version__ = '$Id: copyright_put.py 8805 2010-12-26 16:43:08Z xqt $'

#
# (C) Francesco Cosoleto, 2006
#
# Distributed under the terms of the MIT license.
#

import sys, re, codecs, os, time, shutil
import wikipedia as pywikibot
import config, date

from copyright import put, join_family_data, appdir, reports_cat

#
# Month + Year save method (e.g. User:BotName/Report_December_2007)
append_date_to_wiki_save_path = True

#
# Append day of month to wiki save path
# e.g. User:BotName/Report_25_December_2007)
append_day_to_wiki_save_path = False

#
# Add pubblication date to entries (template:botdate)
append_date_to_entries = False

msg_table = {
    'ar': {'_default': [u'مدخلات جديدة', u'مدخلات جديدة']},
    'en': {'_default': [u'New entries', u'New entries']},
    'es': {'_default': [u'Entradas nuevas', u'Entradas nuevas']},
    'fa': {'_default': [u'محتویات جدید', u'محتویات جدید']},
    'it': {'_default': [u'Pagine nuove', u'Nuove voci'],
           'feed': [u'Aggiunte a voci esistenti', u'Testo aggiunto in']},
    'ru': {'_default': [u'Новые записи', u'Новые записи']},
    'uk': {'_default': [u'Нові записи', u'Нові записи']},
}

template_cat = {
  '_default': [u'This template is used by copyright.py, a script part of [[:m:Using the python wikipediabot|PyWikipediaBot]].',
               u''],
  'it': [u'Questo template è usato dallo script copyright.py del [[:m:Using the python wikipediabot|PyWikipediaBot]].',
         u'Template usati da bot'],
}

stat_msg = {
    'ar': [u'إحصاءات', u'صفحة', u'مدخلات', u'حجم', u'إجمالي', 'تحديث'],
    'en': [u'Statistics', u'Page', u'Entries', u'Size', u'Total', 'Update'],
    'es': [u'Estadísticas', u'Página', u'Entradas', u'Tamaño', u'Total',
           u'Actualizacion'],
    'fa': [u'آمار', u'صفحه', u'محتویات', u'اندازه', u'مجموع', 'بروزرسانی'],
    'it': [u'Statistiche', u'Pagina', u'Segnalazioni', u'Lunghezza', u'Totale',
           u'Ultimo aggiornamento'],
    'ru': [u'Статистика', u'Страница', u'Записи', u'Размер', u'Всего',
           u'Изменено'],
    'uk': [u'Статистика', u'Сторінка', u'Записи', u'Розмір', u'Разом',
           u'Змінено'],
}

separatorC = re.compile('(?m)^== +')

def get_wiki_save_page(stat_page = False):

    site = pywikibot.getSite()

    wiki_save_path = {
        '_default': u'User:%s/Report' % config.usernames[site.family.name][site.lang],
        'es': u'Usuario:%s/Reporte' % config.usernames[site.family.name][site.lang],
        'it': u'Utente:RevertBot/Report'
    }

    save_path = pywikibot.translate(site, wiki_save_path)

    if stat_page:
        return pywikibot.Page(site,
                              '%s/%s' % (save_path,
                                         pywikibot.translate(site, stat_msg)[0]))

    if append_date_to_wiki_save_path:
        t = time.localtime()
        day = ''
        if append_day_to_wiki_save_path:
            day = '_' + str(t[2])

        save_path += day + '_' + date.monthName(site.language(), t[1]) + '_' + str(t[0])

    return pywikibot.Page(site, save_path)

def set_template(name = None):

    site = pywikibot.getSite()

    tcat = pywikibot.translate(site, template_cat)

    url = "%s://%s%s" % (site.protocol(), site.hostname(), site.path())

    botdate = u"""
<div style="text-align:right">{{{1}}}</div><noinclude>%s\n[[%s:%s]]</noinclude>
""" % (tcat[0], site.namespace(14), tcat[1])

    botbox = """
<div class=plainlinks style="text-align:right">[%s?title={{{1}}}&diff={{{2}}}&oldid={{{3}}} diff] - [%s?title={{{1}}}&action=history cron] - [%s?title=Special:Log&page={{{1}}} log]</div><noinclude>%s\n[[%s:%s]]</noinclude>
""" % (url, url, url, tcat[0], site.namespace(14), tcat[1])

    if name == 'botdate':
        p = pywikibot.Page(site, 'Template:botdate')
        if not p.exists():
            p.put(botdate, comment = 'Init.')

    if name == 'botbox':
        p = pywikibot.Page(site, 'Template:botbox')
        if not p.exists():
            p.put(botbox, comment = 'Init.')

def stat_sum(engine, text):
    return len(re.findall('(?im)^\*.*?' + engine + '.*?- ', text))

def get_stats():
    import catlib, pagegenerators

    msg = pywikibot.translate(pywikibot.getSite(), stat_msg)
    cat = catlib.Category(pywikibot.getSite(),
                          'Category:%s'
                          % pywikibot.translate(pywikibot.getSite(),
                                                reports_cat))
    gen = pagegenerators.CategorizedPageGenerator(cat, recurse = True)
    output = u"""{| {{prettytable|width=|align=|text-align=left}}
! %s
! %s
! %s
! %s
! %s
! %s
|-
""" % ( msg[1], msg[2], msg[3], 'Google', 'Yahoo', 'Live Search' )
    gnt = 0 ; ynt = 0 ; mnt = 0 ; ent = 0 ; sn = 0 ; snt = 0
    for page in gen:
        data = page.get()
        gn = stat_sum('google', data)
        yn = stat_sum('yahoo', data)
        mn = stat_sum('(msn|live)', data)
        en = len(re.findall('=== \[\[', data))
        sn = len(data)
        gnt += gn ; ynt += yn ; mnt += mn ; ent += en ; snt += sn
        if en > 0:
            output += u"|%s||%s||%s KB||%s||%s||%s\n|-\n" % (page.aslink(), en, sn / 1024, gn, yn, mn)
    output += u"""|&nbsp;||||||||
|-
|'''%s'''||%s||%s KB||%s||%s||%s
|-
|colspan="6" align=right style="background-color:#eeeeee;"|<small>''%s: %s''</small>
|}
""" % (msg[4], ent, snt / 1024, gnt, ynt, mnt, msg[5], time.strftime("%d " + "%s" % (date.monthName(pywikibot.getSite().language(), time.localtime()[1])) + " %Y"))
    return output

def put_stats():
    page = get_wiki_save_page(stat_page = True)
    page.put(get_stats(), comment = pywikibot.translate(pywikibot.getSite(),
                                                        stat_msg)[0])

def output_files_gen():
    for f in os.listdir(appdir):
        if 'output' in f and not '_pending' in f:
            m = re.search('output_(.*?)\.txt', f)
            if m:
                tag = m.group(1)
            else:
                tag = '_default'
            section_name_and_summary = pywikibot.translate(pywikibot.getSite(),
                                                           msg_table)[tag]
            section = section_name_and_summary[0]
            summary = section_name_and_summary[1]
            yield os.path.join(appdir, f), section, summary

def read_output_file(filename):
    if os.path.isfile(filename + '_pending'):
        shutil.move(filename, filename + '_temp')
        ap = codecs.open(filename + '_pending', 'a', 'utf-8')
        ot = codecs.open(filename + '_temp', 'r', 'utf-8')
        ap.write(ot.read())
        ap.close()
        ot.close()
        os.remove(filename + '_temp')
    else:
        shutil.move(filename, filename + '_pending')
    f = codecs.open(filename + '_pending', 'r', 'utf-8')
    data = f.read()
    f.close()
    return data

def run(send_stats = False):
    page = get_wiki_save_page()
    try:
        wikitext = page.get()
    except pywikibot.NoPage:
        pywikibot.output("%s not found." % page.aslink())
        wikitext = '[[%s:%s]]\n' % (pywikibot.getSite().namespace(14),
                                    pywikibot.translate(pywikibot.getSite(),
                                                        reports_cat))
    final_summary = u''
    output_files = list()
    for f, section, summary in output_files_gen():
        pywikibot.output('File: \'%s\'\nSection: %s\n' % (f, section))
        output_data = read_output_file(f)
        output_files.append(f)
        entries = re.findall('=== (.*?) ===', output_data)
        if not entries:
            continue
        if append_date_to_entries:
            dt = time.strftime('%d-%m-%Y %H:%M', time.localtime())
            output_data = re.sub("(?m)^(=== \[\[.*?\]\] ===\n)", r"\1{{botdate|%s}}\n" % dt, output_data)
        m = re.search('(?m)^==\s*%s\s*==' % section, wikitext)
        if m:
            m_end = re.search(separatorC, wikitext[m.end():])
            if m_end:
                wikitext = wikitext[:m_end.start() + m.end()] + output_data + wikitext[m_end.start() + m.end():]
            else:
                wikitext += '\n' + output_data
        else:
            wikitext += '\n' + output_data
        if final_summary:
            final_summary += ' '
        final_summary += u'%s: %s' % (summary, ', '.join(entries))

    if final_summary:
        pywikibot.output(final_summary + '\n')

        # if a page in 'Image' or 'Category' namespace is checked then fix
        # title section by adding ':' in order to avoid wiki code effects.
        wikitext = re.sub(u'(?i)=== \[\[%s:' % join_family_data('Image', 6), ur'=== [[:\1:', wikitext)
        wikitext = re.sub(u'(?i)=== \[\[%s:' % join_family_data('Category', 14), ur'=== [[:\1:', wikitext)

        # TODO:
        # List of frequent rejected address to improve upload process.
        wikitext = re.sub('http://(.*?)((forumcommunity|forumfree).net)',r'<blacklist>\1\2', wikitext)

        if len(final_summary)>=200:
            final_summary = final_summary[:200]
            final_summary = final_summary[:final_summary.rindex("[")-3] + "..."

        try:
            put(page, wikitext, comment = final_summary)
            for f in output_files:
                os.remove(f + '_pending')
                pywikibot.output("\'%s\' deleted." % f)
        except pywikibot.PageNotSaved:
            raise

        if append_date_to_entries:
            set_template(name =  'botdate')
        if '{{botbox' in wikitext:
            set_template(name =  'botbox')

    if send_stats:
        put_stats()

def main():
    #
    # Send statistics
    send_stats = False
    for arg in pywikibot.handleArgs():
        if arg == "-stats":
            send_stats = True
    run(send_stats = send_stats)

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
