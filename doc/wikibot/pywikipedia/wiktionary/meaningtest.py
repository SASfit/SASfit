#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for meaning.py"""

import meaning
import unittest

class KnownValues(unittest.TestCase):

    knownParserValues = (
                ("*German: [[wichtig]]",
                    [('de','wichtig','',1,False,'')]
                ),
                ("*[[Esperanto]]: [[grava]]",
                    [('eo','grava','',1,False,'')]
                ),
                ("*{{fr}}: [[importante]] {{f}}",
                    [('fr','importante','f',1,False,'')]
                ),
                ("*Dutch: [[voorbeelden]] ''n, pl'', [[instructies]] {{f}}, {{p}}",
                    [('nl','voorbeelden','n',2,False,''),
                     ('nl','instructies', 'f',2,False,'')]
                ),
                ("*Russian: [[шесток]] ''m'' (shestok)",
                    [('ru','шесток','m',1,False,'shestok')]
                ),
                ("*Kazakh: сәлем, салам, сәлеметсіздер(respectable)",
                    [('ka','сәлем','',1,False,''),
                     ('ka','салам','',1,False,''),
                     ('ka','сәлеметсіздер','',1,False,'respectable')]
                ),
                ("*Chinese(Mandarin):[[你好]](ni3 hao3), [[您好]](''formal'' nin2 hao3)",
                    [('zh','你好','',1,False,'ni3 hao3'),
                     ('zh','您好','',1,False,"''formal'' nin2 hao3")]
                ),
                ("*German: [[Lamm]] ''n'' [[:de:Lamm|(de)]]",
                    [('de','Lamm','n',1,False,'')]
                ),
                ("*Italian: [[pronto#Italian|pronto]]",
                    [('it','pronto','',1,False,'')]
                ),
                         )

    def testParser(self):
        '''self.term, self.gender, self.number, self.diminutive and remark parsed correctly from Wiki format'''
        for wikiline, results in self.knownParserValues:
            ameaning = meaning.Meaning('en', 'dummy')
            ameaning.parseTranslations(wikiline)
            i=0
            for termlang, thisterm, termgender, termnumber, termisadiminutive, remark in results:
                resultterm = ameaning.translations[termlang]['alltrans'][i]['trans']
                self.assertEqual(resultterm.getTerm(), thisterm)
                self.assertEqual(resultterm.getGender(), termgender)
                self.assertEqual(resultterm.getNumber(), termnumber)
#                self.assertEqual(resultterm.getIsDiminutive(), termisadiminutive)
                self.assertEqual(ameaning.translations[termlang]['alltrans'][i]['remark'], remark)
                i+=1

if __name__ == "__main__":
    unittest.main()

