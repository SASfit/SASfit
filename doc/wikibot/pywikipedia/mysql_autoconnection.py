#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
A small MySQL wrapper that catches dead MySQL connections, and tries to
reconnect them.
"""
#
# (C) Bryan Tong Minh, 2007
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: mysql_autoconnection.py 7588 2009-11-03 20:36:09Z btongminh $'

import MySQLdb, MySQLdb.cursors
import time

class Connection(object):
    """A wrapper to the MySQLdb database and cursor object.
    MySQL does not support cursors, so they can be safely wrapped
    into one object.
    """
    RECOVERABLE_ERRORS = (
        1040, # Too many connections
        1152, # Aborted connection
        2002, # Connection error
        2003, # Can't connect
        2006, # Server gone
        2013, # Server lost
        2014, # Commands out of sync
        )

    def __init__(self, retry_timeout = 60, max_retries = -1,
        callback = lambda *args: None, *conn_args, **conn_kwargs):

        self.retry_timeout = retry_timeout
        self.max_retries = max_retries
        self.current_retry = 0
        self.callback = callback
        self.database = None

        self.conn_args = conn_args
        self.conn_kwargs = conn_kwargs

        self.connected = False
        self.connect()

    def wait(self):
        if self.current_retry > self.max_retries and self.max_retries != -1:
            raise RuntimeError('Maximum retries exceeded')
        if self.current_retry:
            self.callback(self)
        time.sleep(self.current_retry * self.retry_timeout)
        self.current_retry += 1
    def __call(self, (object, function_name), *args, **kwargs):
        try:
            return getattr(object, function_name)(*args, **kwargs)
        except MySQLdb.Error, e:
            if e[0] in self.RECOVERABLE_ERRORS:
                self.error = e
                self.connect()
                return getattr(self, function_name)(*args, **kwargs)
            else:
                raise

    # Mimic database object
    def connect(self):
        self.close()

        while not self.connected:
            self.wait()
            try:
                self._connect()
            except MySQLdb.Error, e:
                self.error = e
        return True

    def _connect(self):
        self.database = MySQLdb.connect(*self.conn_args, **self.conn_kwargs)
        self.connected = True
        self.current_retry = 0
        self.__cursor = self.database.cursor()

    def close(self):
        self.current_retry = 0
        self.connected = False
        try:
            if self.database:
                self.database.close()
        except:
            pass

    def cursor(self, cursorclass = MySQLdb.cursors.Cursor):
        if type(cursorclass) is not type(self.__cursor):
            self.__cursor = self.database.cursor(cursorclass)
        return self

    # Mimic cursor object
    def __iter__(self):
        return self.__cursor.__iter__()
    def __getattr__(self, name, *args, **kwargs):
        if hasattr(self.database, name):
            obj = self.database
        else:
            obj = self.__cursor
        attr = getattr(obj, name)
        if hasattr(attr, '__call__'):
            return CallWrapper(self.__call, (obj, name))
        return attr


class CallWrapper(object):
    def __init__(self, executor, function):
        self.__executor = executor
        self.__function = function
    def __call__(self, *args, **kwargs):
        return self.__executor(self.__function,
            *args, **kwargs)
    def __getattr__(self, name):
        getattr(self.__function, name)

def connect(retry_timeout = 60, max_retries = -1,
    callback = lambda *args: None, *conn_args, **conn_kwargs):

    return Connection(retry_timeout = retry_timeout,
        max_retries = max_retries,
        callback = callback,
        *conn_args, **conn_kwargs)

if __name__ == '__main__':
    def callback(conn):
        print 'Waiting for', conn

    host = raw_input('Host: ')
    username = raw_input('Username: ')
    password = raw_input('Password: ')

    conn = connect(retry_timeout = 5, max_retries = 4, callback = callback,
        host = host, user = username, passwd = password, charset = 'utf8')
    cur = conn.cursor()
    print 'Connected!'
    conn.execute('SELECT 1')
    print 'Query ok, closing connection...'
    conn.close()
    conn.execute('SELECT 1')
    print 'Query ok, please kill the connection...'
    raw_input()
    conn.execute('SELECT 1')
    print 'Query ok!, please kill while connected...'
    raw_input()
    conn.execute('SELECT SLEEP(30), 1')

    print 'Now testing inserts...'
    conn.execute('USE test')
    conn.execute('CREATE TEMPORARY TABLE mysql_autoconnection (value INT)')
    for i in xrange(10):
        conn.execute('INSERT INTO mysql_autoconnection VALUES (%s)', (i, ))
    conn.commit()
    conn.execute('SELECT * FROM mysql_autoconnection')
    print conn.fetchall()
    print 'Query ok!'
    raw_input()



