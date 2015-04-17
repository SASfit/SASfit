#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This module provides a way for users of the Wikimedia toolserver to check the
use of images from Commons on other Wikimedia wikis. It supports both running
checkusage against the database and against the live wikis. It is very
efficient as it only creates one HTTP connection and one MySQL connection
during its life time. It is not suitable for multithreading!

The CheckUsage class' constructor accept as parameters the maximum number of
wikis that should be checked, an option to use it only live and the parameters
to connect to the MySQL database. The top wikis in size will be checked. The
class provides multiple methods:

get_usage(image)
This method will return a generator object that generates the usage of the
image, returned as the following tuple: (page_namespace, page_title,
full_title). page_namespace is the numeric namespace, page_title the page title
without namespace, full_title the page title including localized namespace.

get_usage_db(dbname, image), get_usage_live(domain, image)
Those methods allow querying a specific wiki, respectively against the database
and against the live wiki. They accept respectively the database name and the
domain name. The return a generator which generates the same results as
get_usage().

get_usage_multi(images)
Calls get_usage for each image and returns a dictionary with usages.

get_replag(dbname)
Returns the time in seconds since the latest known edit of dbname.
"""
#
# (C) Bryan Tong Minh, 2007
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: checkusage.py 9053 2011-03-13 12:24:49Z xqt $'
#

import httplib, urlparse, socket, time
from urllib import urlencode
import simplejson

import wikipedia, family

try:
    import MySQLdb
except ImportError:
    pass
try:
    import mysql_autoconnection
except ImportError:
    pass
__ver__ = '0.4c'

def strip_ns(title):
    title = title.replace(' ', '_')
    if title.find(':') != -1:
        return title[title.find(':') + 1:]
    return title
def strip_image(title):
    if title.startswith('Image:'):
        return strip_ns(title)
    return title

def family(domain):
    if domain is None:
        raise RuntimeError('None is not a valid family')

    wiki = domain.split('.')
    # Standard family
    if wiki[1] in ('wikipedia', 'wiktionary', 'wikibooks',
        'wikiquote', 'wikisource', 'wikinews', 'wikiversity'):
        return wiki[0], wiki[1]
    # Family on own domain
    if wiki[0] == 'www':
        return wiki[1], wiki[1]
    # Special Wikimedia wiki
    if wiki[1] == 'wikimedia':
        return wiki[0], wiki[0]
    # Multilingual wikisource
    if domain == 'wikisource.org':
        return '-', 'wikisource'
    raise RuntimeError('Unknown family ' + domain)

# Not sure whether this belongs here
class HTTP(object):
    def __init__(self, host):
        self.host = host
        self._conn = httplib.HTTPConnection(host)
        #self._conn.set_debuglevel(100)
        self._conn.connect()

    def request(self, method, path, headers, data):
        if not headers: headers = {}
        if not data: data = ''
        headers['Connection'] = 'Keep-Alive'
        headers['User-Agent'] = 'MwClient/' + __ver__

        try:
            self._conn.request(method, path, data, headers)
        except socket.error, e:
            self._conn.close()
            raise

        try:
            res = self._conn.getresponse()
        except httplib.BadStatusLine:
            self._conn.close()
            self._conn.connect()
            self._conn.request(method, path, data, headers)
            res = self._conn.getresponse()

        if res.status >= 500:
            self._conn.request(method, path, data, headers)
            res = self._conn.getresponse()

        if res.status != 200:
            raise RuntimeError, (res.status, res)

        return res

    def query_api(self, host, path, **kwargs):
        data = urlencode([(k, v.encode('utf-8')) for k, v in kwargs.iteritems()])
        if path.endswith('query.php'):
            query_string = '%s?format=json&%s' % (path, data)
            method = 'GET'
            data = ''
        elif path.endswith('api.php'):
            query_string = '%s?format=json' % path
            method = 'POST'
        else:
            raise ValueError('Unknown api %s' % repr(api))

        try:
            res = self.request(method, query_string,
                {'Host': host, 'Content-Type': 'application/x-www-form-urlencoded'}, data)
        except httplib.ImproperConnectionState:
            self._conn.close()
            self.__init__(self.host)
        try:
            data = simplejson.load(res)
        finally:
            res.close()

        if 'error' in data:
            if data['error']['code'] == u'internal_api_error_DBConnectionError':
                return self.query_api(host, path, **kwargs)
            raise wikipedia.Error(data['error']['code'],
                data['error']['info'])

        return data
    def close(self):
        self._conn.close()

class HTTPPool(list):
    def __init__(self, retry_timeout = 10, max_retries = -1,
        callback = lambda *args: None):

        self.retry_timeout = retry_timeout
        self.max_retries = -1
        self.callback = callback
        self.current_retry = 0

        list.__init__(self, ())

    def query_api(self, host, path, **kwargs):
        conn = self.find_conn(host)
        while True:
            try:
                res = conn.query_api(host, path, **kwargs)
                self.current_retry = 0
                return res
            except RuntimeError:
                self.wait()
            except socket.error:
                conn.close()
                self.remove(conn)
                self.wait()
                conn = self.find_conn(host)


    def find_conn(self, host):
        for conn in self:
            if host in conn.hosts:
                return conn
        for conn in self:
            while True:
                try:
                    conn.request('HEAD', '/w/api.php', {}, '').read()
                except RuntimeError, e:
                    if e[0] < 500:
                        break
                else:
                    conn.hosts.append(host)
                    return conn
        conn = HTTP(host)
        conn.hosts = []
        self.append(conn)
        return self

    def wait(self):
        if self.current_retry > self.max_retries and self.max_retries != -1:
            raise RuntimeError('Maximum retries exceeded')
        if self.current_retry:
            self.callback(self)
        time.sleep(self.current_retry * self.retry_timeout)
        self.current_retry += 1

    def close(self):
        for conn in self:
            conn.close()
        del self[:]


class CheckUsage(object):
    def __init__(self, limit = 100,
            mysql_default_server = 3, mysql_host_prefix = 'sql-s', mysql_host_suffix = '',
            mysql_kwargs = {}, no_db = False, use_autoconn = False,

            http_retry_timeout = 30, http_max_retries = -1,
            http_callback = lambda *args: None,

            mysql_retry_timeout = 60,
            mysql_max_retries = -1, mysql_callback = lambda *args: None):

        self.http = None
        self.http_retry_timeout = http_retry_timeout
        self.http_max_retries = http_max_retries
        self.http_callback = http_callback

        if no_db: return

        self.mysql_host_prefix = mysql_host_prefix
        self.mysql_kwargs = mysql_kwargs.copy() # To be safe
        if 'host' in self.mysql_kwargs: del self.mysql_kwargs['host']
        self.use_autoconn = use_autoconn
        self.mysql_retry_timeout = mysql_retry_timeout
        self.mysql_max_retries = mysql_max_retries
        self.mysql_callback = mysql_callback

        self.connections = []

        # Mapping database name -> mysql connection
        self.databases = {}
        # Mapping server id -> mysql connection
        self.servers = {}
        # Mapping database name -> (lang, family)
        self.sites = {}

        self.domains = {}

        self.unknown_families = []
        # Mapping family name -> family object
        self.known_families = {}

        database, cursor = self.connect_mysql(mysql_host_prefix + str(mysql_default_server))
        self.servers[mysql_default_server] = (database, cursor)

        # Find where the databases are located
        cursor.execute('SELECT dbname, domain, server FROM toolserver.wiki ORDER BY size DESC LIMIT %s', (limit, ))
        for dbname, domain, server in cursor.fetchall():
            if server not in self.servers:
                self.servers[server] = self.connect_mysql(mysql_host_prefix + str(server) + mysql_host_suffix)

            # FIXME: wikimediafoundation!
            # TODO: This is one big mess
            try:
                lang, fam = family(domain)
                if fam not in self.known_families:
                    self.known_families[fam] = wikipedia.Family(fam, fatal = False)
            except (RuntimeError, ValueError, SyntaxError):
                self.unknown_families.append(domain)
            else:
                self.sites[dbname] = (lang, fam)
                self.databases[dbname] = self.servers[server]

            self.domains[dbname] = domain


    def connect_mysql(self, host):
        # A bug in MySQLdb 1.2.1_p will force you to set
        # all your connections to use_unicode = False.
        # Please upgrade to MySQLdb 1.2.2 or higher.
        if self.use_autoconn:
            database = mysql_autoconnection.connect(
                use_unicode = False, host = host,
                retry_timeout = self.mysql_retry_timeout,
                max_retries = self.mysql_max_retries,
                callback = self.mysql_callback,
                **self.mysql_kwargs)
        else:
            database = MySQLdb.connect(use_unicode = False,
                host = host, **self.mysql_kwargs)
        cursor = database.cursor()
        self.connections.append((database, cursor))
        return database, cursor
    def connect_http(self):
        if not self.http:
            self.http = HTTPPool(retry_timeout = self.http_retry_timeout,
                max_retries = self.http_max_retries, callback = self.http_callback)

    def get_usage(self, image):
        for dbname in self.databases:
            usage = self.get_usage_db(dbname, image, True)
            for link in usage:
                yield self.sites[dbname], link

    def get_usage_db(self, dbname, image, shared = False):
        #image = strip_image(image)
        lang, family_name = self.sites[dbname]
        family = self.known_families[family_name]

        if family.shared_image_repository(lang) != (lang, family_name) and shared:
            left_join = 'LEFT JOIN %s.image ON (il_to = img_name) WHERE img_name IS NULL AND' % dbname
        else:
            left_join = 'WHERE';
        query = """SELECT page_namespace, page_title FROM %s.page, %s.imagelinks
    %s page_id = il_from AND il_to = %%s"""
        self.databases[dbname][1].execute(query % (dbname, dbname, left_join),
            (image.encode('utf-8', 'ignore'), ))
        for page_namespace, page_title in self.databases[dbname][1]:
            stripped_title = page_title.decode('utf-8', 'ignore')
            if page_namespace != 0:
                title = family.namespace(lang, page_namespace) + u':' + stripped_title
            else:
                title = stripped_title
            yield page_namespace, stripped_title, title

    def get_usage_live(self, site, image, shared = False):
        self.connect_http()

        if type(site) is str:
            hostname = site
            apipath = '/w/api.php'
        else:
            hostname = site.hostname()
            apipath = site.apipath()

        # FIXME: Use continue
        kwargs = {'action': 'query', 'iutitle': u'Image:' + image,
            'titles': u'Image:' + image, 'prop': 'info'}
        kwargs['list'] = 'imageusage'
        kwargs['iulimit'] = '500'

        res = self.http.query_api(hostname, apipath,
            **kwargs)
        if '-1' not in res['query']['pages'] and shared:
            return

        usages = res['query'].get('imageusage')
        if not usages: return

        # Apparently this someday changed from dict to list?
        if type(usages) is dict:
            usages = usages.values()

        for usage in usages:
            title = usage['title'].replace(' ', '_')
            namespace = usage['ns']
            if namespace != 0:
                stripped_title = strip_ns(title)
            else:
                stripped_title = title
            yield namespace, stripped_title, title


    def exists(self, site, image):
        self.connect_http()
        # Check whether the image still is deleted on Commons.
        # BUG: This also returns true for images with a page, but
        # without the image itself. Can be fixed by querying query.php
        # instead of api.php.
        # BUG: This is ugly.
        return '-1' not in self.http.query_api(site.hostname(), site.apipath(),
            action = 'query', titles = 'Image:' + image)['query']['pages']


    def close(self):
        if getattr(self, 'http'):
            self.http.close()
        if not hasattr(self, 'databases'): return
        for connection, cursor in self.databases.itervalues():
            try:
                connection.close()
            except:
                pass

