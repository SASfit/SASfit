#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for Wiktionary.py"""
__version__ = '$Id: test_wiktionary.py 9043 2011-03-13 10:25:08Z xqt $'

import unittest
import test_utils

import wiktionary

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
            result = wiktionary.Header(wikiline).contents
            self.assertEqual(contents, result)

    def testHeaderInitKnownValuesLevel(self):
        """Header parsing comparing known result with known input for level"""
        for wikiline, contents, level, type in self.knownValues:
            result = wiktionary.Header(wikiline).level
            self.assertEqual(level, result)

    def testHeaderInitKnownValuesType(self):
        """Header parsing comparing known result with known input for type"""
        for wikiline, contents, level, type in self.knownValues:
            result = wiktionary.Header(wikiline).type
            self.assertEqual(type, result)


class SortEntriesCheckSortOrder(unittest.TestCase):
    """Entries should be sorted as follows on a page: Translingual first, Wikilang next, then the others alphabetically on the language name in the Wiktionary's language """
    def testHeaderInitKnownValuesType(self):
        """Sorting order of Entries on a page"""
        examples=((('en','C'),('eo', 'en', 'de', 'nl', 'es', 'translingual', 'fr'),
                              ['translingual', 'en', 'nl', 'eo', 'fr', 'de', 'es']),
                  (('nl','C'),('eo', 'en', 'de', 'nl', 'es', 'translingual', 'fr'),
                              ['translingual', 'nl', 'de', 'en', 'eo', 'fr', 'es']),
                  (('fr','C'),('eo', 'en', 'de', 'nl', 'es', 'translingual', 'fr'),
                              ['translingual', 'fr', 'de', 'en', 'es', 'eo', 'nl']),
                  (('de','C'),('eo', 'en', 'de', 'nl', 'es', 'translingual', 'fr'),
                              ['translingual', 'de', 'en', 'eo', 'fr', 'nl', 'es']),
                 )
        for example in examples:
            page = wiktionary.WiktionaryPage(example[0][0], example[0][1])
            for lang in example[1]:
                entry = wiktionary.Entry(lang)
                page.addEntry(entry)
            page.sortEntries()
            self.assertEqual(page.sortedentries, example[2])

class TestKnownValuesInParser(unittest.TestCase):
    """This class will check various aspects of parsing Wiktionary entries into our object model"""
    knownvalues=({'wikilang': 'en', 'term': 'nut', 'wikiformat': u"""==English==
===Etymology===
From Middle English [[nute]], from Old English [[hnutu]]. <!-- Is Latin [[nux]], nuc- a cognate? -->
===Pronunciation===
*[[w:AHD|AHD]]: nÅ­t
*[[w:IPA|IPA]]: /nÊt/
*[[w:SAMPA|SAMPA]]: /nVt/
===Noun===
'''nut''' (''plural'' '''[[nuts]]''')

#A hard-shelled seed.
#A piece of metal, often [[hexagonal]], with a hole through it with internal threading intended to fit on to a bolt.
#(''informal'') An insane person.
#(''slang'') The head.
#(''slang; rarely used in the singular'') A testicle.

====Synonyms====
*(''insane person''): [[loony]], [[nutcase]], [[nutter]]
*(''the head''): [[bonce]], [[noddle]] (See further synonyms under [[head]])
*(''a testicle''): [[ball]], [[bollock]] (''taboo slang''), [[nad]]

====Translations====

'''seed'''
{{top}}
<!--Put translations for languages from A to I here-->
*Dutch: [[noot]] ''f''
*French: ''no generic translation exists''; [[noix]] ''f'' ''is often used, but this actually means "[[walnut]]"''
*German: [[Nuss]] ''f''
*German: [[Nuss]] ''f''
*Italian: [[noce]] {{f}}
{{mid}}
<!--Put translations for languages from J to Z here-->
*Latin: [[nux]]
{{bottom}}

'''that fits on a bolt'''
{{top}}
<!--Put translations for languages from A to I here-->
*Dutch: [[moer]] ''f''
*French: [[Ã©crou]] ''m''
*German: [[Mutter]] ''f''
*Italian: [[dado]] {{m}}
{{mid}}
<!--Put translations for languages from J to Z here-->
{{bottom}}

'''informal: insane person'''
{{top}}
<!--Put translations for languages from A to I here-->
*Dutch: [[gek]] ''m'', [[gekkin]] ''f'', [[zot]] ''m'', [[zottin]] ''f''
*French: [[fou]] ''m'', [[folle]] ''f''
*German: [[Irre]] ''m/f'', [[Irrer]] ''m indef.''
{{mid}}
<!--Put translations for languages from J to Z here-->
{{bottom}}

'''slang: the head'''
{{top}}
<!--Put translations for languages from A to I here-->
*German: [[Birne]] ''f'', [[RÃ¼be]] ''f'', [[DÃ¶tz]] ''m''
{{mid}}
<!--Put translations for languages from J to Z here-->
{{bottom}}

'''slang: testicle'''
{{top}}
<!--Put translations for languages from A to I here-->
*Dutch: [[noten]] ''m (plural)'' <!--Never heard this before-->, [[bal]] ''m'', [[teelbal]] ''m''
*French: [[couille]] ''f''
*German: [[Ei]] ''n'', ''lately:'' [[Nuss]] ''f''
{{mid}}
<!--Put translations for languages from J to Z here-->
*Spanish: [[cojone]], [[huevo]]
{{bottom}}

=====Translations to be checked=====
<!--Remove this section once all of the translations below have been moved into the tables above.-->
{{checktrans}}
The translations below need to be checked by native speakers and inserted into the appropriate table(s) above, removing any numbers.  Any numbering associating translations with definitions is unreliable.

*[[Anglo-Saxon]]: [[hnutu]] ''f''
*Breton: [[kraoÃ±]] ''collective noun'' [[kraoÃ±enn]] ''singular f'' (1), [[kraouenn]] ''f'' -oÃ¹ ''pl'' (2), [[brizh-sod]] (3), [[kell]] ''f'' divgell ''pl'' (4)
*Finnish: [[pÃ¤hkinÃ¤]] (1), [[mutteri]] (2), [[hullu]] (3), [[pÃ¶pi]] (3), [[muna#Finnish|muna]] (4), [[palli]] (4), [[kaali]] (5)
*Interlingua: [[nuce]] (1); [[matre vite]] (2); [[folle]] (3); [[teste]] (4), [[testiculo]] (4)
*Italian: [[noce]] ''f''
*Latvian: [[rieksts]] ''m'' (1), [[uzgrieznis]] ''m'' (2), [[trakais]] ''m'' (3), [[jucis]] ''m'' (3), [[pauts]] ''m'' (usually ''pl. - pauti'') (4)
*Polish: [[orzech]] ''m'' (1), [[nakrÄtka]] ''f'' (2), [[Åwir]] ''m'' (3)
*Portuguese: [[noz]] ''f'' (1); [[porca]] ''f'' (2); [[louco]] ''m'' (3), [[doido]] ''m'' (3), [[maluco]] ''m'' (3); [[bago]] ''m'' (4), [[ovo]] ''m'' (4)
*Romanian: [[nucÄ]] ''f'' (1), [[piuliÅ£Ä]] ''f'' (2), [[nebun]] ''m'' (3) [[Å£icnit]] ''m'' (3)
*Russian: [[Ð¾ÑÐµÑ]] ''m'' (1), [[Ð³Ð°Ð¹ÐºÐ°]] (gaika/gajka) ''f'' (2), [[ÑÑÐ¼Ð°ÑÑÐµÐ´ÑÐ¸Ð¹]] (sumasshedshij) ''m'' / [[ÑÑÐ¼Ð°ÑÑÐµÐ´ÑÐ°Ñ]] (sumasshedshaya) ''f'' (3), [[ÑÐ¹ÑÐ¾]] (yaitso) ''n'' (4)
*Spanish: [[nuez]] (1), [[tuerca]] (2), [[chiflado]] (3), [[chalado]] (3)
*[[Tok Pisin]]: [[nat]] (2), [[longlongman]] (3), [[kiau]] (4), [[het]] (5)

====Related terms====
*[[coconut]]
*[[groundnut]]
*[[hazelnut]]
*[[peanut]]
*[[walnut]]
*[[nutbeam]]
*[[nutcase]]
*[[nutmeg]]
*[[NutRageous]]&reg;
*[[nutshell]]

===Transitive verb===
'''to nut''' ('''nutting''', '''nutted''')

#(''slang'') To hit deliberately with the head; to [[headbutt]].

===Intransitive verb===
'''to nut''' ('''nutting''', '''nutted''')

#(''slang'') To [[ejaculate]] (''semen'').

----
==Dutch==
===Noun===
'''nut''' ''n''

# [[use]], [[benefit]]

[[io:nut]]
[[la:nut]]

[[Category:1000 English basic words]]
[[Category:Colors]]
[[Category:Browns]]
[[Category:Trees]]
[[category:Foods]]
""",
   'internalrep':
    (
     [u'1000 English basic words',u'Colors',u'Browns',u'Trees',u'Foods'],
     [u'io','la'],
     {u'en':
      [u'nut', None, u'nuts',
       [{'definition': u'A hard-shelled seed', 'concisedef': u'seed',
         'trans': {'nl': u"[[noot]] ''f''", 'fr': u"""''no generic translation exists''; [[noix]] ''f'' ''is often used, but this actually means "[[walnut]]"''""", 'de': u"[[Nuss]] ''f''", 'it': u"[[noce]] {{f}}", 'la': u"[[nux]]"}},
        {'definition': u"A piece of metal, often [[hexagonal]], with a hole through it with internal threading intended to fit on to a bolt.", 'concisedef': u'that fits on a bolt',
         'trans': {'nl': u"[[moer]] ''f''", 'fr': u"[[Ã©crou]] ''m''", 'de': u"[[Mutter]] ''f''", 'it': u"[[dado]] {{m}}"}},
        {'definition': u"(''informal'') An insane person.", 'concisedef': u"'''informal: insane person'''",
         'syns': u"[[loony]], [[nutcase]], [[nutter]]",
         'trans': {'nl': u"[[gek]] ''m'', [[gekkin]] ''f'', [[zot]] ''m'', [[zottin]] ''f''", 'fr': "[[fou]] ''m'', [[folle]] ''f''", 'de': "[[Irre]] ''m/f'', [[Irrer]] ''m indef.''"}},
        {'definition': u"(''slang'') The head.", 'concisedef': u"'''slang: the head'''",
         'syns': u"[[bonce]], [[noddle]] (See further synonyms under [[head]])",
         'trans': {'de': u"[[Birne]] ''f'', [[RÃ¼be]] ''f'', [[DÃ¶tz]] ''m''"}},
        {'definition': u"(''slang; rarely used in the singular'') A testicle.", 'concisedef': u"'''slang: testicle'''",
         'syns': u"[[ball]], [[bollock]] (''taboo slang''), [[nad]]",
         'trans': {'nl': u"[[noten]] ''m (plural)'' <!--Never heard this before-->, [[bal]] ''m'', [[teelbal]] ''m''", 'fr': u"[[couille]] ''f''", 'de': u"[[Ei]] ''n'', ''lately:'' [[Nuss]] ''f''", 'es': u"[[cojone]], [[huevo]]"}},
       ],
      ],
       u'nl':
      [u'nut', 'n', None,
       [{'definition': u'[[use]], [[benefit]]'}]
      ],
     }
     )
    },{'wikilang': 'en', 'term': 'nut', 'wikiformat': u"""[[category:Foods]]
[[category:Drinks]]""", 'internalrep': ([u'Foods', u'Drinks'],[],{})})

    def testWhetherCategoriesAreParsedProperly(self):
        """Test whether Categories are parsed properly"""
        for value in self.knownvalues:
            internalrepresentation=value['internalrep']
            apage = wiktionary.WiktionaryPage(value['wikilang'],value['term'])
            apage.parseWikiPage(value['wikiformat'])

            self.assertEqual(apage.categories, internalrepresentation[0])

    def testWhetherLinksAreParsedProperly(self):
        """Test whether Links are parsed properly"""
        for value in self.knownvalues:
            internalrepresentation=value['internalrep']
            apage = wiktionary.WiktionaryPage(value['wikilang'],value['term'])
            apage.parseWikiPage(value['wikiformat'])

            self.assertEqual(apage.interwikilinks, internalrepresentation[1])

    def testWhetherDefsAreParsedProperly(self):
        """Test whether Definitions are parsed properly"""
        for value in self.knownvalues:
            internalrepresentation=value['internalrep'][2]
            apage = wiktionary.WiktionaryPage(value['wikilang'],value['term'])
            apage.parseWikiPage(value['wikiformat'])
            for entrylang in internalrepresentation.keys():
                term=internalrepresentation[entrylang][0]
                gender=internalrepresentation[entrylang][1]
                plural=internalrepresentation[entrylang][2]
                definitions=internalrepresentation[entrylang][3]
                refdefs=[]
                for definition in definitions:
                    refdefs.append(definition['definition'])

                resultmeanings=[]
                for key in apage.entries[entrylang].meanings.keys():
                    for resultmeaning in apage.entries[entrylang].meanings[key]:
                        resultmeanings.append(resultmeaning.definition)

                self.assertEqual(resultmeanings.sort(), refdefs.sort())

'''
class ToRomanBadInput(unittest.TestCase):
    def testTooLarge(self):
        """toRoman should fail with large input"""
        self.assertRaises(roman.OutOfRangeError, roman.toRoman, 4000)
'''

if __name__ == "__main__":
    unittest.main()
