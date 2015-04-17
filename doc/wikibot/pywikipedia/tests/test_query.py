#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for userlib.py"""
__version__ = '$Id: test_query.py 9197 2011-04-25 08:57:30Z xqt $'

import unittest
import tests.test_pywiki

import wikipedia as pywikibot
import catlib, userlib
import query


class PyWikiQueryTestCase(tests.test_pywiki.PyWikiTestCase):

    def assertEqualQueryResult(self, params, expectedresult):
        data = query.GetData(params, self.site)
        self.assertEqual(data[u'query'], expectedresult)

    def test_basic(self):
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['registration'],
            'ususers': u'Example',
        }
        expectedresult = {u'users': [
        {
            u'userid': 215131,
            u'name': u'Example',
            u'registration': u'2005-03-19T00:17:19Z'
        }
        ]}
        self.assertEqualQueryResult(params, expectedresult)

    def test_titles_multi(self):
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['registration'],
            'ususers': u'Example|Example2',
        }
        expectedresult = {u'users': [
        {
            u'userid': 215131,
            u'name': u'Example',
            u'registration': u'2005-03-19T00:17:19Z'
        },
        {
            u'userid': 5176706,
            u'name': u'Example2',
            u'registration': u'2007-08-26T02:13:33Z'
        },
        ]}
        self.assertEqualQueryResult(params, expectedresult)

    def test_titles_list(self):
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['registration'],
            'ususers': [u'Example', u'Example2'],
        }
        expectedresult = {u'users': [
        {
            u'userid': 215131,
            u'name': u'Example',
            u'registration': u'2005-03-19T00:17:19Z'
        },
        {
            u'userid': 5176706,
            u'name': u'Example2',
            u'registration': u'2007-08-26T02:13:33Z'
        },
        ]}
        self.assertEqualQueryResult(params, expectedresult)

    def test_titles_Page(self):
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['registration'],
            'ususers': [pywikibot.Page(self.site, u'Example'),
                        pywikibot.Page(self.site, u'Example2')],
        }
        expectedresult = {u'users': [
        {
            u'userid': 215131,
            u'name': u'Example',
            u'registration': u'2005-03-19T00:17:19Z'
        },
        {
            u'userid': 5176706,
            u'name': u'Example2',
            u'registration': u'2007-08-26T02:13:33Z'
        },
        ]}
        self.assertEqualQueryResult(params, expectedresult)

    def test_titles_User(self):
        params = {
            'action': 'query',
            'list': 'users',
            'usprop': ['registration'],
            'ususers': [userlib.User(self.site, u'Example'),
                        userlib.User(self.site, u'Example2')],
        }
        expectedresult = {u'users': [
        {
            u'userid': 215131,
            u'name': u'Example',
            u'registration': u'2005-03-19T00:17:19Z'
        },
        {
            u'userid': 5176706,
            u'name': u'Example2',
            u'registration': u'2007-08-26T02:13:33Z'
        },
        ]}
        self.assertEqualQueryResult(params, expectedresult)

    def test_titles_Category(self):
        params = {
            'action': 'query',
            'prop': 'revisions',
            'rvprop': ['ids', 'timestamp', 'user'],
            'rvdir': 'newer',
            'rvlimit': 1,
            'titles': [catlib.Category(self.site, u'Category:Categories')],
        }
        expectedresult = {u'pages': {u'794823':
        {
            u'ns': 14,
            u'pageid': 794823,
            u'revisions': [{
                u'revid': 4494485,
                u'user': u'SEWilco',
                u'timestamp': u'2004-07-07T18:45:50Z',
            }],
            u'title': u'Category:Categories',
        },
        }}
        self.assertEqualQueryResult(params, expectedresult)

if __name__ == "__main__":
    unittest.main()
