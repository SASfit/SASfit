#!/usr/bin/python
# -*- coding: utf-8  -*-

'''
'''

import meaning
import structs

class Entry:
    """ This class contains the entries that belong together on one page.
        On Wiktionaries that are still on first character capitalization, this means both [[Kind]] and [[kind]].
        Terms in different languages can be described. Usually there is one entry for each language.
    """

    def __init__(self,entrylang,meaning=""):
        """ Constructor
            Called with one parameter:
            - the language of this entry
        and can optionally be initialized with a first meaning
        """
        self.entrylang=entrylang
        self.meanings = {} # a dictionary containing the meanings for this term grouped by part of speech
    if meaning:
        self.addMeaning(meaning)
        self.posorder = [] # we don't want to shuffle the order of the parts of speech, so we keep a list to keep the order in which they were encountered

    def addMeaning(self,meaning):
        """ Lets you add another meaning to this entry """
        term = meaning.term # fetch the term, in order to be able to determine its part of speech in the next step

        self.meanings.setdefault( term.pos, [] ).append(meaning)
        if not term.pos in self.posorder:    # we only need each part of speech once in our list where we keep track of the order
            self.posorder.append(term.pos)

    def getMeanings(self):
        """ Returns a dictionary containing all the meaning objects for this entry
        """
        return self.meanings

    def wikiWrap(self,wikilang):
        """ Returns a string for this entry in a format ready for Wiktionary
        """
        entry = structs.wiktionaryformats[wikilang]['langheader'].replace('%%langname%%',langnames[wikilang][self.entrylang]).replace('%%ISOLangcode%%',self.entrylang) + '\n'

        for pos in self.posorder:
            meanings = self.meanings[pos]

            entry += structs.wiktionaryformats[wikilang]['posheader'][pos]
            entry +='\n'
            if wikilang=='en':
                entry = entry + meanings[0].term.wikiWrapAsExample(wikilang) + '\n\n'
                for meaning in meanings:
                    entry = entry + '#' + meaning.getLabel() + ' ' + meaning.definition + '\n'
                    entry = entry + meaning.wikiWrapExamples()
                entry +='\n'

            if wikilang=='nl':
                for meaning in meanings:
                    term=meaning.term
                    entry = entry + meaning.getLabel() + term.wikiWrapAsExample(wikilang) + '; ' + meaning.definition + '\n'
                    entry = entry + meaning.wikiWrapExamples()
                entry +='\n'

            if meaning.hasSynonyms():
                entry = entry + structs.wiktionaryformats[wikilang]['synonymsheader'] + '\n'
                for meaning in meanings:
                    entry = entry + '*' + meaning.getLabel() + "'''" + meaning.getConciseDef() + "''': " + meaning.wikiWrapSynonyms(wikilang)
                entry +='\n'

            if meaning.hasTranslations():
                entry = entry + structs.wiktionaryformats[wikilang]['translationsheader'] + '\n'
                for meaning in meanings:
                    entry = entry + meaning.getLabel() + "'''" + meaning.getConciseDef() + "'''" + '\n' + meaning.wikiWrapTranslations(wikilang,self.entrylang) + '\n\n'
                entry +='\n'
        return entry

    def showContents(self,indentation):
        """ Prints the contents of all the subobjects contained in this entry.
            Every subobject is indented a little further on the screen.
            The primary purpose is to help keep your sanity while debugging.
        """
        print ' ' * indentation + 'entrylang = %s'% self.entrylang

        print ' ' * indentation + 'posorder:' + repr(self.posorder)

        meaningkeys = self.meanings.keys()
        for meaningkey in meaningkeys:
            for meaning in self.meanings[meaningkey]:
                meaning.showContents(indentation+2)
