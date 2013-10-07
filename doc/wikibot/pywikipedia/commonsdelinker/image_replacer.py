#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Please refer to delinker.txt for full documentation.
"""
#
#
# (C) Bryan Tong Minh, 2007
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: image_replacer.py 9053 2011-03-13 12:24:49Z xqt $'
import config, wikipedia, simplejson
import re, time
import threadpool
import sys, os, signal, traceback

from delinker import wait_callback, output, connect_database
from checkusage import family

def mw_timestamp(ts):
    return '%s%s%s%s-%s%s-%s%sT%s%s:%s%s:%s%sZ' % tuple(ts)
DB_TS = re.compile('[^0-9]')
def db_timestamp(ts):
    return DB_TS.sub('', ts)
IMG_NS = re.compile(r'(?i)^\s*File\:')
def strip_image(img):
    img = IMG_NS.sub('', img)
    img = img.replace(' ', '_')
    img = img[0].upper() + img[1:]
    return img.strip()

def site_prefix(site):
    if site.lang == site.family.name:
        return site.lang
    if (site.lang, site.family.name) == ('-', 'wikisource'):
        return 'oldwikisource'
    return '%s:%s' % (site.family.name, site.lang)

class Replacer(object):
    def __init__(self):
        self.config = config.CommonsDelinker
        self.config.update(getattr(config, 'Replacer', ()))
        self.template = re.compile(r'\{\{%s\|([^|]*?)\|([^|]*?)(?:(?:\|reason\=(.*?))?)\}\}' % \
                self.config['replace_template'])
        self.disallowed_replacements = [(re.compile(i[0], re.I), re.compile(i[1], re.I))
            for i in self.config.get('disallowed_replacements', ())]

        self.site = wikipedia.getSite(persistent_http = True)
        self.site.forceLogin()

        self.database = connect_database()
        self.cursor = self.database.cursor()

        self.first_revision = 0
        if self.config.get('replacer_report_replacements', False):
            self.reporters = threadpool.ThreadPool(Reporter, 1, self.site, self.config)
            self.reporters.start()


    def read_replace_log(self):
        """ The actual worker method """

        # FIXME: Make sqlite3 compatible
        insert = """INSERT INTO %s (timestamp, old_image, new_image,
            status, user, comment) VALUES (%%s, %%s, %%s,
            'pending', %%s, %%s)""" % self.config['replacer_table']

        page = wikipedia.Page(self.site, self.config['command_page'])

        # Get last revision date
        if self.cursor.execute("""SELECT timestamp FROM %s
                ORDER BY timestamp DESC LIMIT 1""" % \
                self.config['replacer_table']):
            since = mw_timestamp(self.cursor.fetchone()[0])
        else:
            since = None

        if self.config.get('clean_list', False):
            username = config.sysopnames[self.site.family.name][self.site.lang]
        else:
            username = None

        try:
            # Fetch revision history
            revisions = self.get_history(page.title(), since, username)
            # Fetch the page any way, to prevent editconflicts
            old_text = text = page.get()
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception, e:
            # Network error, not critical
            output(u'Warning! Unable to read replacement log.', False)
            output('%s: %s' % (e.__class__.__name__, str(e)), False)
            #self.site.conn.close()
            #self.site.conn.connect()
            return time.sleep(self.config['timeout'])

        # We're being killed
        if '{{stop}}' in text.lower():
            output(u'Found {{stop}} on command page. Not replacing anything.')
            return time.sleep(self.config['timeout'])

        # Sort oldest first
        revisions.sort(key = lambda rev: rev['timestamp'])

        # Find all commands
        replacements = self.template.finditer(text)

        remove_from_list = []
        count = 0
        for replacement in replacements:
            if count == self.config.get('replacer_rate_limit', -1): break
            # Find out who's to blame
            res = self.examine_revision_history(
                revisions, replacement, username)
            if res and self.allowed_replacement(replacement) and \
                    replacement.group(1) != replacement.group(2):
                # Insert replace command into database
                self.cursor.execute(insert, res)
                # Tag line for removal
                remove_from_list.append(replacement.group(0))
                output('Replacing %s by %s: %s' % replacement.groups())
            count += 1

        # Save all replaces to database
        self.database.commit()

        if remove_from_list and self.config.get('clean_list', False):
            # Cleanup the command page
            while True:
                try:
                    for remove in remove_from_list:
                        text = text.replace(remove, u'')
                    # Kill the freaky CommonsDupes
                    text = text.replace('== Dummy section, heading can be deleted (using [http://tools.wikimedia.de/~magnus/commons_dupes.php CommonsDupes]) ==', '')
                    # Kill the freaky whitespace
                    text = text.replace('\r', '')
                    while '\n\n\n' in text:
                        text = text.replace('\n\n\n', '\n')
                    # Save the page
                    page.put(text.strip(), comment = 'Removing images being processed')
                    return
                except wikipedia.EditConflict:
                    # Try again
                    text = page.get()

    def get_history(self, title, since, username):
        """ Fetch the last 50 revisions using the API """

        address = self.site.api_address()
        predata = [
            ('action', 'query'),
            ('prop', 'revisions'),
            ('titles', title.encode('utf-8')),
            ('rvprop', 'timestamp|user|comment|content'),
            ('rvlimit', '50'),
            ('format', 'json'),
        ]
        if username:
            predata.append(('rvexcludeuser', username.encode('utf-8')))
        if since:
            predata.append(('rvend', since))
        response, data = self.site.postForm(address, predata)
        data = simplejson.loads(data)
        if 'error' in data:
            raise RuntimeError(data['error'])

        page = data['query']['pages'].values()[0]
        if 'missing' in page:
            raise Exception('Missing page!')
        return page.get('revisions', [])

    def examine_revision_history(self, revisions, replacement, username):
        """ Find out who is to blame for a replacement """

        for revision in revisions:
            if replacement.group(0) in revision['*']:
                db_time = db_timestamp(revision['timestamp'])
                if db_time < self.first_revision or not self.first_revision:
                    self.first_revision = int(db_time)
                return (db_time, strip_image(replacement.group(1)),
                    strip_image(replacement.group(2)),
                    revision['user'], replacement.group(3))

        output('Warning! Could not find out who did %s' % \
                repr(replacement.group(0)), False)
        return

    def read_finished_replacements(self):
        """ Find out which replacements have been completed and add them to
            the reporters queue. """

        self.cursor.execute('START TRANSACTION WITH CONSISTENT SNAPSHOT')
        self.cursor.execute("""SELECT old_image, new_image, user, comment FROM
            %s WHERE status = 'done' AND timestamp >= %i""" % \
            (self.config['replacer_table'], self.first_revision))
        finished_images = list(self.cursor)
        self.cursor.execute("""UPDATE %s SET status = 'reported'
            WHERE status = 'done' AND timestamp >= %i""" % \
            (self.config['replacer_table'], self.first_revision))
        self.cursor.commit()

        for old_image, new_image, user, comment in finished_images:
            self.cursor.execute("""SELECT wiki, namespace, page_title
                FROM %s WHERE img = %%s AND status <> 'ok'""" %
                self.config['log_table'], (old_image, ))
            not_ok = [(wiki, namespace, page_title.decode('utf-8', 'ignore'))
                for wiki, namespace, page_title in self.cursor]

            if not comment: comment = ''

            self.reporters.append((old_image.decode('utf-8', 'ignore'),
                new_image.decode('utf-8', 'ignore'),
                user.decode('utf-8', 'ignore'),
                comment.decode('utf-8', 'ignore'), not_ok))


    def start(self):
        while True:
            self.read_replace_log()
            if self.config.get('replacer_report_replacements', False):
                self.read_finished_replacements()

            # Replacer should not loop as often as delinker
            time.sleep(self.config['timeout'] * 2)

    def allowed_replacement(self, replacement):
        """ Method to prevent World War III """

        for source, target in self.disallowed_replacements:
            if source.search(replacement.group(1)) and \
                    target.search(replacement.group(2)):
                return False
        return True

class Reporter(threadpool.Thread):
    """ Asynchronous worker to report finished replacements to file pages. """

    def __init__(self, pool, site, config):
        self.site = wikipedia.getSite(site.lang, site.family,
            site.user, True)
        self.config = config

        threadpool.Thread.__init__(self, pool)

    def do(self, args):
        try:
            self.report(args)
        except Exception, e:
            output(u'A critical error during reporting has occured!', False)
            output('%s: %s' % (e.__class__.__name__, str(e)), False)
            traceback.print_exc(file = sys.stderr)
            sys.stderr.flush()
            self.exit()
            os.kill(0, signal.SIGTERM)

    def report(self, (old_image, new_image, user, comment, not_ok)):
        not_ok_items = []
        for wiki, namespace, page_title in not_ok:
            # Threadsafety?
            site = wikipedia.getSite(*family(wiki))
            namespace_name = site.namespace(namespace)
            if namespace_name:
                namespace_name = namespace_name + u':'
            else:
                namespace_name = u''

            if unicode(site) == unicode(self.site):
                if (namespace, page_title) != (6, old_image):
                    not_ok_items.append(u'[[:%s%s]]' % \
                        (namespace_name, page_title))
            else:
                not_ok_items.append(u'[[:%s:%s%s]]' % (site_prefix(site),
                    namespace_name, page_title))

        template = u'{{%s|new_image=%s|user=%s|comment=%s|not_ok=%s}}' % \
            (self.config['replacer_report_template'],
            new_image, user, comment,
            self.config.get('replacer_report_seperator', u', ').join(not_ok_items))
        page = wikipedia.Page(self.site, u'Image:' + old_image)

        try:
            text = page.get()
        except wikipedia.NoPage:
            output(u'Warning! Unable to report replacement to %s. Page does not exist!' % old_image)
            return
        except wikipedia.IsRedirectPage:
            output(u'Warning! %s is a redirect; not reporting replacement!' % old_image)
            return
        try:
            page.put(u'%s\n%s' % (template, text),
                comment = u'This image has been replaced by ' + new_image)
        except wikipedia.PageNotSaved, e:
            output(u'Warning! Unable to report replacement to %s.' % old_image, False)
            output('%s: %s' % (e.__class__.__name__, str(e)), False)
        except wikipedia.ServerError, e:
            output(u'Warning! Server error while reporting replacement to %s.' % old_image, False)
            output('%s: %s' % (e.__class__.__name__, str(e)), False)
            return self.report((old_image, new_image, user, comment, not_ok))
        else:
            output(u'Reporting replacement of %s by %s.' % \
                (old_image, new_image))


def main():
    global R

    import sys, traceback
    wikipedia.handleArgs()
    output(u'Running ' + __version__)

    try:
        try:
            # FIXME: Add support for single-process replacer.
            R = Replacer()
            output(u'This bot runs from: ' + str(R.site))
            R.start()
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception, e:
            output('A critical error has occured! Aborting!')
            traceback.print_exc(file = sys.stderr)
    finally:
        try:
            R.reporters.exit()
        except:
            pass
        wikipedia.stopme()

if __name__ == '__main__': main()
