#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for header.py"""

import header
import unittest

class KnownValues(unittest.TestCase):
    knownValues = (
                    ('==English==', 'en', 2, 'lang'),
                    ('=={{en}}==', 'en', 2, 'lang'),
                    ('{{-en-}}', 'en', None, 'lang'),
                    ('===Noun===', 'noun', 3, 'pos'),
                    ('==={{noun}}===', 'noun', 3, 'pos'),
                    ('{{-noun-}}', 'noun', None, 'pos'),
                    ('===Verb===', 'verb', 3, 'pos'),
                    ('==={{verb}}===', 'verb', 3, 'pos'),
                    ('{{-verb-}}', 'verb', None, 'pos'),
                    ('====Translations====', 'trans', 4, 'other'),
                    ('===={{trans}}====', 'trans', 4, 'other'),
                    ('{{-trans-}}', 'trans', None, 'other'),
                  )

    def testHeaderInitKnownValuesContents(self):
        """Header parsing comparing known result with known input for contents"""
        for wikiline, contents, level, type in self.knownValues:
            result = header.Header(wikiline).contents
            self.assertEqual(contents, result)

    def testHeaderInitKnownValuesLevel(self):
        """Header parsing comparing known result with known input for level"""
        for wikiline, contents, level, type in self.knownValues:
            result = header.Header(wikiline).level
            self.assertEqual(level, result)

    def testHeaderInitKnownValuesType(self):
        """Header parsing comparing known result with known input for type"""
        for wikiline, contents, level, type in self.knownValues:
            result = header.Header(wikiline).type
            self.assertEqual(type, result)

    def testReprSanity(self):
        """Header __repr__, __eq__, __ne__ should give sane results"""
        for stuff in self.knownValues:
            wikiline=stuff[0]
            h=header.Header(wikiline)
            self.assertEqual(h, eval(repr(h)) )
            self.assertNotEqual(h,header.Header())

if __name__ == "__main__":
    unittest.main()
