#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for pywikibot/textlib.py"""
__version__ = '$Id: test_textlib.py 9201 2011-04-26 01:12:57Z jayvdb $'

import unittest
from tests.test_pywiki import PyWikiTestCase

import wikipedia as pywikibot
import pywikibot.textlib as textlib
import catlib


class PyWikiTextLibTestCase(PyWikiTestCase):

    end_of_line = '\r\n'
    blank_line = '\r\n\r\n'
    foo_page_start = 'Foo' + blank_line
    iwresult1 = '[[de:German]]\r\n[[fr:French]]\r\n'
    catresult1 = '[[Category:Cat1]]\r\n[[Category:Cat2]]\r\n'
    result1 = foo_page_start + \
              catresult1.strip() + \
              blank_line + \
              iwresult1.strip()

    def setUp(self):
        self.site = pywikibot.getSite('en', 'wikipedia')
        self.data = [catlib.Category(self.site, 'Category:Cat1'),
                     catlib.Category(self.site, 'Category:Cat2')]

        self.site_de = pywikibot.getSite('de', 'wikipedia')
        self.site_fr = pywikibot.getSite('fr', 'wikipedia')

    def test_interwikiFormat(self):
        interwikis = {
           'de':pywikibot.Page(self.site_de, 'German'),
           'fr':pywikibot.Page(self.site_fr, 'French')
        }
        self.assertEqual(self.iwresult1,
                         textlib.interwikiFormat(interwikis, self.site))

    def assertRoundtripInterwiki(self, text, count):
        old_interwikis = textlib.getLanguageLinks(text, self.site)
        new_text = textlib.replaceLanguageLinks(text, old_interwikis, site = self.site)

        self.assertEqual(len(old_interwikis), count)
        self.assertEqual(text, new_text)

    def assertFailedRoundtripInterwiki(self, text):
        old_interwikis = textlib.getLanguageLinks(text, self.site)
        new_text = textlib.replaceLanguageLinks(text, old_interwikis, site = self.site)

        self.assertNotEqual(text, new_text)

    def test_replaceLanguageLinks(self):
        # This case demonstrates that eol isnt stripped
        self.assertFailedRoundtripInterwiki(self.result1)
        self.assertRoundtripInterwiki(self.result1 + self.end_of_line, 2)

    def test_replaceLanguageLinks1(self):
        # This case demonstrates that eol isnt stripped
        result = self.foo_page_start + self.iwresult1
        self.assertFailedRoundtripInterwiki(self.iwresult1)
        self.assertRoundtripInterwiki(result, 2)

    def test_categoryFormat_raw(self):
        self.assertEqual(self.catresult1,
                         textlib.categoryFormat(['[[Category:Cat1]]',
                                                 '[[Category:Cat2]]'],
                                                self.site))

    def test_categoryFormat_bare(self):
        self.assertEqual(self.catresult1,
                         textlib.categoryFormat(['Cat1', 'Cat2'], self.site))

    def test_categoryFormat_Category(self):
        self.assertEqual(self.catresult1,
                         textlib.categoryFormat(self.data, self.site))

    def test_categoryFormat_Page(self):
        data = [pywikibot.Page(self.site, 'Category:Cat1'),
                pywikibot.Page(self.site, 'Category:Cat2')]
        self.assertEqual(self.catresult1, textlib.categoryFormat(self.data,
                                                              self.site))

    def assertRoundtripCategory(self, text, catcount):
        cats = textlib.getCategoryLinks(text)
        self.assertEqual(len(cats), catcount)
        self.assertEqual(text, textlib.replaceCategoryLinks(text,
                                                            cats,
                                                            site = self.site))

    def test_replaceCategoryLinks(self):
        self.assertRoundtripCategory(self.result1,2)

    def test_replaceCategoryLinks1(self):
        result = 'Blah\r\n\r\n[[Category:Cat1]]\r\n[[Category:Cat2]]'
        self.assertRoundtripCategory(result,2)

    def test_replaceCategoryLinks2(self):
        result = 'Blah\r\n\r\n[[Category:Cat1]]\r\n[[Category:Cat2]]\r\n\r\n[[fr:Test]]'
        self.assertRoundtripCategory(result,2)

if __name__ == "__main__":
    unittest.main()
