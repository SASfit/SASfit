#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for userlib.py"""
__version__ = '$Id: test_userlib.py 9043 2011-03-13 10:25:08Z xqt $'

import unittest
import test_utils

import userlib
import wikipedia

class _GetAllUI(unittest.TestCase):
    def setUp(self):
        self.site = wikipedia.getSite('en', 'wikipedia')
        self.obj = userlib._GetAllUI(self.site,
                                     [userlib.User(self.site, "Example")],
                                     None,
                                     False)

    def testGetData(self):
        data = self.obj.getData()
        expecteddata = {
        u'Example': {
            u'editcount': 1,
            u'name': u'Example',
            u'gender': u'unknown',
            u'blockedby': u'AGK',
            u'blockreason': u"Example account. (Restoring [[User:CesarB|CesarB]]'s previous block: my test is complete.)",
            u'registration': u'2005-03-19T00:17:19Z'}
        }
        self.assertEqual(data, expecteddata);

if __name__ == "__main__":
    unittest.main()
