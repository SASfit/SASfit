#!/usr/bin/python
# -*- coding: utf-8  -*-

"""Unit tests for Wiktionarypage.py"""

import wiktionarypage
import entry as entrymodule
import unittest


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
            page = wiktionarypage.WiktionaryPage(example[0][0], example[0][1])
            for lang in example[1]:
                entry = entrymodule.Entry(lang)
                page.addEntry(entry)
            page.sortEntries()
            self.assertEqual(page.sortedentries, example[2])

class TestKnownValuesInParser(unittest.TestCase):
    """This class will check various aspects of parsing Wiktionary entries into our object model"""
    knownvalues=(
{'wikilang': 'en',
 'term': 'nut',
 'wikiformat': u"""==English==
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
       [{'definition': u'A hard-shelled seed.', 'concisedef': u'seed',
         'trans': {'remark': '',
                   'alltrans': {
                             'nl': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"noot", 'f', 1)}
                                                    ]
                                   },
#                            'fr': u"""''no generic translation exists''; [[noix]] ''f'' ''is often used, but this actually means "[[walnut]]"''""",
                             'de': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"Nuss", 'f', 1)}
                                                    ]
                                   },
                             'it': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"noce", 'f', 1)}
                                                    ]
                                   },
                             'la': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"nux", '', 1)}
                                                    ]
                                   },
                               }
                  }
        },
        {'definition': u"A piece of metal, often [[hexagonal]], with a hole through it with internal threading intended to fit on to a bolt.", 'concisedef': u'that fits on a bolt',
         'trans': {'remark': '',
                   'alltrans': {
                             'nl': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"moer", 'f', 1)}
                                                    ]
                                   },
                             'fr': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"Ã©crou", 'm', 1)}
                                                    ]
                                   },
                             'de': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"Mutter", 'f', 1)}
                                                    ]
                                   },
                             'it': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"dado", 'm', 1)}
                                                    ]
                                   }
                                }
                    }
        },
        {'definition': u"(''informal'') An insane person.", 'concisedef': u"informal: insane person",
         'syns': {'remark': '',
                  'synonyms': [{'remark': '',
                                'synonym': u"loony"},
                               {'remark': '',
                                'synonym': u"nutcase"},
                               {'remark': '',
                                'synonym': u"nutter"}
                              ]
                  },
         'trans': {'remark': '',
                   'alltrans': {
                             'nl': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"gek", 'm', 1)},
                                                     {'remark': '',
                                                      'translation': (u"gekkin", 'f', 1)},
                                                     {'remark': '',
                                                      'translation': (u"zot", 'm', 1)},
                                                     {'remark': '',
                                                      'translation': (u"zottin", 'f', 1)}
                                                    ]
                                    },
                             'fr': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': ("fou", 'm', 1)},
                                                     {'remark': '',
                                                      'translation': ("folle", 'f', 1)}
                                                    ]
                                   },
                             'de': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': ("Irre", 'mf', 1)},
                                                     {'remark': '',
                                                      'translation': ("Irrer", 'm indef.', 1)}
                                                    ]
                                   }
                               }
                    }
        },
        {'definition': u"(''slang'') The head.", 'concisedef': u"slang: the head",
         'syns': {'remark': '(See further synonyms under [[head]])',
                  'synonyms': [{'remark': '',
                                'synonym': u"bonce"},
                               {'remark': '',
                                'synonym': u"noddle"}]},
         'trans': {'remark': '',
                   'alltrans': {
                            'de': {'remark': '',
                                   'translations': [{'remark': '',
                                                     'translation': (u"Birne", 'f', 1)},
                                                    {'remark': '',
                                                     'translation': ("RÃ¼be", 'f', 1)},
                                                    {'remark': '',
                                                     'translation': ("DÃ¶tz", 'm', 1)}
                                                   ]
                                   }
                                }
                   }
        },
        {'definition': u"(''slang; rarely used in the singular'') A testicle.", 'concisedef': u"slang: testicle",
         'syns': {'remark': '',
                  'synonyms': [{'remark': '',
                                'synonym': u"ball"},
                               {'remark': "(''taboo slang'')",
                                'synonym': u"bollock"},
                               {'remark': '',
                                'synonym': u"nad"}]},
         'trans': {'remark': '',
                   'alltrans': {'nl': {'remark': '<!--Never heard this before-->',
                                    'translations': [{'remark': '',
                                                      'translation': (u"noten", 'm', 2)},
                                                     {'remark': '',
                                                      'translation': ("bal", 'm', 1)},
                                                     {'remark': '',
                                                      'translation': ("teelbal", 'm', 1)}
                                                    ]
                                    },
                              'fr': {'remark': '',
                                     'translations': [{'remark': '',
                                                       'translation': (u"couille", 'f', 1)}
                                                     ]
                                    },
                              'de': {'remark': '',
                                     'translations': [{'remark': '',
                                                       'translation': (u"Ei", 'n', 1)},
                                                      {'remark': u"''lately:''",
                                                       'translation': (u"Nuss", 'f', 1)}
                                                     ]
                                    },
                              'es': {'remark': '',
                                     'translations': [{'remark': '',
                                                       'translation': (u"cojone", '', 1)},
                                                      {'remark': '',
                                                       'translation': (u"huevo", '', 1)}
                                                     ]
                                    }
                            }
                  },
        }
       ],
      ],
       u'nl':
      [u'nut', 'n', None,
       [{'definition': u'[[use]], [[benefit]]', 'concisedef': u''}]
      ],
     }
    )
   },
{'wikilang': 'nl',
 'term': 'dummy',
 'wikiformat': u"""
{{-nl-}}
{{-noun-}}
'''dummy''' {{m}}
""",
   'internalrep':
    (
     [u''],
     [u''],
     {u'nl':
      [u'dummy', 'm', u"dummy's",
       [{'definition': u'', 'concisedef': u'',
         'trans': {'remark': '',
                   'alltrans': {
                             'nl': {'remark': '',
                                    'translations': [{'remark': '',
                                                      'translation': (u"", '', 1)}
                                                    ]
                                   },
                               }
                  }
         }
       ],
      ],
     }
    )
   }
  )
#     def testWhetherCategoriesAreParsedProperly(self):
#         """Test whether Categories are parsed properly"""
#         for value in self.knownvalues:
#             internalrepresentation=value['internalrep']
#             apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
#             apage.parseWikiPage(value['wikiformat'])
#
#             self.assertEqual(apage.categories, internalrepresentation[0])
#
#     def testWhetherLinksAreParsedProperly(self):
#         """Test whether Links are parsed properly"""
#         for value in self.knownvalues:
#             internalrepresentation=value['internalrep']
#             apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
#             apage.parseWikiPage(value['wikiformat'])
#
#             self.assertEqual(apage.interwikilinks, internalrepresentation[1])
#
#     def testWhetherDefsAreParsedProperly(self):
#         """Test whether definitions are parsed properly"""
#         for value in self.knownvalues:
#             internalrepresentation=value['internalrep'][2]
#             apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
#             apage.parseWikiPage(value['wikiformat'])
#             for entrylang in internalrepresentation.keys():
# #                term=internalrepresentation[entrylang][0]
# #                gender=internalrepresentation[entrylang][1]
# #                plural=internalrepresentation[entrylang][2]
#                 definitions=internalrepresentation[entrylang][3]
#                 refdefs=[]
#                 for definition in definitions:
#                     refdefs.append(definition['definition'])
#
#                 resultmeanings=[]
#                 for key in apage.entries[entrylang].meanings.keys():
#                     for resultmeaning in apage.entries[entrylang].meanings[key]:
#                         resultmeanings.append(resultmeaning.definition)
#
#                 self.assertEqual(resultmeanings.sort(), refdefs.sort())
#
#     def testWhetherDefsAndConciseDefsAreMatchedProperly(self):
#         """Test whether definitions and concisedefs are matched properly"""
#         for value in self.knownvalues:
#             internalrepresentation=value['internalrep'][2]
#             apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
#             apage.parseWikiPage(value['wikiformat'])
#             for entrylang in internalrepresentation.keys():
#                 definitions=internalrepresentation[entrylang][3]
#                 refdefs={}
#                 for definition in definitions:
#                     if definition['concisedef']!='':
#                         refdefs[definition['concisedef']] = definition['definition']
#
#                 resultmeanings={}
#                 for key in apage.entries[entrylang].meanings.keys():
#                     for resultmeaning in apage.entries[entrylang].meanings[key]:
#                         resultmeanings[resultmeaning.concisedef] = resultmeaning.definition
#
#                 for concisedef in resultmeanings.keys():
#                     if concisedef!='' and refdefs.has_key(concisedef) and resultmeanings.has_key(concisedef):
#                         self.assertEqual(resultmeanings[concisedef], refdefs[concisedef])
#
#     def testWhetherSynonymsAreParsedProperly(self):
#         """Test whether synonyms are parsed properly"""
#         for value in self.knownvalues:
#             internalrepresentation=value['internalrep'][2]
#             apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
#             apage.parseWikiPage(value['wikiformat'])
#             for entrylang in internalrepresentation.keys():
#                 definitions=internalrepresentation[entrylang][3]
#                 refsyns={}
#                 for definition in definitions:
#                     if definition.has_key('syns') and definition['syns']!='':
#                         refsyns[definition['concisedef']] = definition['syns']
#
#                 resultsyns={}
#                 for key in apage.entries[entrylang].meanings.keys():
#                     for resultmeaning in apage.entries[entrylang].meanings[key]:
#                         resultsyns[resultmeaning.concisedef] = resultmeaning.synonyms
#
#                 for concisedef in resultsyns.keys():
#                     if concisedef!='' and refsyns.has_key(concisedef) and resultsyns.has_key(concisedef):
#                         self.assertEqual(resultsyns[concisedef], refsyns[concisedef])
#
    def testWhetherTranslationsAreParsedProperly(self):
        """Test whether translations are parsed properly"""
        for value in self.knownvalues:
            internalrepresentation=value['internalrep'][2]
            apage = wiktionarypage.WiktionaryPage(value['wikilang'],value['term'])
            apage.parseWikiPage(value['wikiformat'])
            for entrylang in internalrepresentation.keys():
                definitions=internalrepresentation[entrylang][3]
                reftrans={}
                for definition in definitions:
                    if 'trans' in definition and definition['trans']!='':
                        reftrans[definition['concisedef']] = definition['trans']

                resulttrans={}
                for key in apage.entries[entrylang].meanings.keys():
                    print key
                    for resultmeaning in apage.entries[entrylang].meanings[key]:
                        print resultmeaning.concisedef
                        print 'Translations: ',resultmeaning.getTranslations()
                        resulttrans[resultmeaning.concisedef] = resultmeaning.getTranslations()

                for concisedef in resulttrans.keys():
                    if concisedef != '' and concisedef in reftrans and concisedef in resulttrans:
                        print concisedef
                        print resulttrans[concisedef]
#                        raw_input()
                        print reftrans[concisedef]
                        raw_input()
#                         self.assertEqual(resulttrans[concisedef]['remark'], reftrans[concisedef]['remark'])
#                         for translatedlang in resulttrans[concisedef]['alltrans']:
#                             self.assertEqual(resulttrans[concisedef]['alltrans'][translatedlang]['remark'], reftrans[concisedef]['alltrans'][translatedlang]['remark'])
#                             for translation in resulttrans[concisedef]['alltrans'][translatedlang][translations]:
#                                 self.assertEqual(resulttrans[concisedef]['alltrans'][translatedlang][translation]['remark'], reftrans[concisedef]['alltrans'][translatedlang][translation]['remark'])
#                                 refterm,refgender,refnumber=reftrans[concisedef]['alltrans'][translatedlang][translation]
#                                 resultterm,resultgender,resultnumber=reftrans[concisedef]['alltrans'][translatedlang][translation]
#                                 self.assertEqual(refterm,resultterm)
#                                 self.assertEqual(refgender,resultgender)
#                                 self.assertEqual(refnumber,resultnumber)

if __name__ == "__main__":
    unittest.main()
