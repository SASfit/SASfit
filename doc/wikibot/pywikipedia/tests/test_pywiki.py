#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit test framework for pywiki"""
__version__ = '$Id: test_pywiki.py 9197 2011-04-25 08:57:30Z xqt $'

import unittest
import test_utils

import wikipedia as pywikibot


class PyWikiTestCase(unittest.TestCase):

    def setUp(self):
        self.site = pywikibot.getSite('en', 'wikipedia')

if __name__ == "__main__":
    unittest.main()
