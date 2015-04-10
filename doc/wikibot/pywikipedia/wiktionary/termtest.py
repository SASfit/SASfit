#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for term.py"""

import term
import unittest

class KnownValues(unittest.TestCase):
    knownValues = (
                    ('en','noun','en','example','', "'''example'''", '[[example]]'),
                    ('en','noun','nl','voorbeeld','n', "'''voorbeeld''' ''n''", "[[voorbeeld]] ''n''"),
                    ('nl','noun','nl','voorbeeld','n', "'''voorbeeld''' {{n}}", "[[voorbeeld]] {{n}}"),
                    ('en','verb','en','to show','', "'''to show'''", 'to [[show]]'),
                    ('en','verb','nl','tonen','', "'''tonen'''", "[[tonen]]"),
                    ('nl','verb','nl','tonen','', "'''tonen'''", "[[tonen]]"),
                  )

    def testTermKnownValuesWikiWrapAsExample(self):
        """WikiWrap output correct for a term used as an example"""
        for wikilang, pos, termlang, thisterm, termgender, asexample, forlist in self.knownValues:
            if pos=='noun':
                aterm = term.Noun(termlang, thisterm, gender=termgender)
            if pos=='verb':
                aterm = term.Verb(termlang, thisterm)
            result = aterm.wikiWrapAsExample(wikilang)
            self.assertEqual(asexample, result)

    def testTermKnownValuesWikiWrapForList(self):
        """WikiWrap output correct for a term when used in a list"""
        for wikilang, pos, termlang, thisterm, termgender, asexample, forlist in self.knownValues:
            if pos=='noun':
                aterm = term.Noun(termlang, thisterm, gender=termgender)
            if pos=='verb':
                aterm = term.Verb(termlang, thisterm)
            result = aterm.wikiWrapForList(wikilang)
            self.assertEqual(forlist, result)

    def testTermKnownValuesWikiWrapAsTranslation(self):
        """WikiWrap output correct for a term when used as a translation"""
        for wikilang, pos, termlang, thisterm, termgender, asexample, forlist in self.knownValues:
            if pos=='noun':
                aterm = term.Noun(termlang, thisterm, gender=termgender)
            if pos=='verb':
                aterm = term.Verb(termlang, thisterm)
            result = aterm.wikiWrapAsTranslation(wikilang)
            self.assertEqual(forlist, result)

    knownParserValues = (
                    ("[[example]] ",'en','example','',1),
                    ("[[voorbeeld]] ''n''",'nl','voorbeeld','n',1),
                    ("[[voorbeeld]] {{n}}",'nl','voorbeeld','n',1),
                    ("[[voorbeelden]] ''n, pl''",'nl','voorbeelden','n',2),
                    ("[[voorbeelden]] {{n}},{{p}}",'nl','voorbeelden','n',2),
#                    ("to [[show]]",'en','to show','',1),
                    ("[[tonen]]",'nl','tonen','',1),
                  )

    def testParser(self):
        '''self.term, self.gender and self.number parsed correctly from Wiki format'''
        for wikiline, termlang, thisterm, termgender, termnumber in self.knownParserValues:
            aterm = term.Term(termlang, '', wikiline=wikiline)
            self.assertEqual(aterm.getTerm(), thisterm)
            self.assertEqual(aterm.getGender(), termgender)
            self.assertEqual(aterm.getNumber(), termnumber)

if __name__ == "__main__":
    unittest.main()

