#!/usr/bin/python
# -*- coding: utf-8  -*-

import term
import structs
import re

class Meaning:
    """ This class contains one meaning for a word or an expression.
    """
    def __init__(self,term,definition='',etymology='',synonyms={'remark': '', 'synonyms': [{'remark': '', 'synonym': ''}]},translations={},label='',concisedef='',examples=[]):
        """ Constructor
            Generally called with one parameter:
            - The Term object we are describing

            - definition (string) for this term is optional
            - etymology (string) is optional
            - synonyms (optional)
            - translations (dictionary of Term objects, ISO639 is the key) is optional
        """
        self.term=term
        self.definition=definition
        self.concisedef=concisedef
        self.etymology=etymology
        self.synonyms=synonyms
        # A structure, possibly containing the following items:
        # {'remark' : 'this remark concerns all the synonyms for this meaning',
        #  'synonyms' : [
        #                {'remark': 'this remark concerns this particular synonym',
        #                 'synonym': Term object containing the synonym
        #                },
        #               ]
        self.examples=examples
        self.label=label

        if translations: # Why this has to be done explicitly is beyond me, but it doesn't work correctly otherwise
            self.translations=translations
        else:
            self.translations={} # a dictionary containing lists with translations to the different languages. Each translation is again a dictionary as follows: {'remark': '', 'trans': Term object}
            self.translationsremark='' # a remark applying to all the translations for this meaning
            self.translationsremarks={} # a dictionary containing remarks applying to a specific language
        self.label=label

    def setDefinition(self,definition):
        """ Provide a definition  """
        self.definition=definition

    def getDefinition(self):
        """ Returns the definition  """
        return self.definition

    def setEtymology(self,etymology):
        """ Provide the etymology  """
        self.etymology=etymology

    def getEtymology(self):
        """ Returns the etymology  """
        return self.etymology

    def setSynonyms(self,synonyms):
        """ Provide the synonyms  """
        self.synonyms=synonyms

    def getSynonyms(self):
        """ Returns the list of synonym Term objects  """
        return self.synonyms

    def parseSynonyms(self,synonymswikiline):
        synsremark = ''
        synonyms = []
        openparenthesis=synonymswikiline.lower().find('(see')
        if openparenthesis!=-1:
            closeparenthesis=synonymswikiline.find(')',openparenthesis)
            synsremark=synonymswikiline[openparenthesis:closeparenthesis+1]
            synonymswikiline=synonymswikiline[:openparenthesis-1] +  synonymswikiline[closeparenthesis+1:]
        for synonym in synonymswikiline.split(','):
            synremark = ''
            openparenthesis=synonym.lower().find('(')
            if openparenthesis!=-1:
                closeparenthesis=synonym.find(')',openparenthesis)
                synremark=synonym[openparenthesis:closeparenthesis+1]
                synonym=synonym[:openparenthesis-1] +  synonym[closeparenthesis+2:]
            synonym=synonym.replace(',','').replace("[",'').replace(']','').strip()
            synonyms.append({'synonym': synonym, 'remark': synremark})
        self.synonyms={'remark': synsremark, 'synonyms': synonyms}

    def parseTranslations(self,translationswikiline):
        '''
        This function will parse one line in wiki format
        Typically this is the translation towards one language.
        '''
        # There can be many translations for a language, each one can have remark
        # a gender and a number.
        # There can also be a remark for the group of translations for a given language
        # And there can be a remark applying to all the translations (That has to be detected and stored on a higher level though.
        # It is also possible that the translation for a given language is not parseable
        # In that case the entire line should go into the remark.
        translationsremark = translationremark = ''
        translations = [] # a list of translations for a given language
        colon=translationswikiline.find(':')
        if colon!=-1:
            # Split in lang and the rest of the line which should be a list of translations
            lang = translationswikiline[:colon].replace('*','').replace('[','').replace(']','').replace('{','').replace('}','').strip().lower()
            trans = translationswikiline[colon+1:]
            # Look up lang and convert to an ISO abbreviation
            isolang=''
            if lang in structs.langnames:
                isolang=lang
            elif lang in structs.invertedlangnames:
                isolang=structs.invertedlangnames[lang]

            # We need to prepare the line a bit to make it more easily parseable
            # All the commas found between '' '' are converted to simple spaces
            # Also }}, {{ has to be converted to }} {{

            trans="''".join([ [i[1],re.sub(',',' ',i[1])][i[0]%2==1] for i in enumerate(trans.split("''")) ])

            trans=re.sub(r"(}}.*),(.*{{)",'}} {{',trans)

            # Now split up the translations (we got rid of extraneous commas)
            for translation in trans.split(','):
                translation=translation.strip()
                # Find what is contained inside parentheses
                m= re.search(r'(\(.*\))',translation)
                if m:
                    # Only when the parentheses don't occur
                    # between [[ ]]
                    if translation[m.end(1)+1:m.end(1)+2]!=']':
                        translationremark = m.group(1).replace('(','').replace(')','')
                        translation=translation.replace(m.group(1),'')
                number = 1
                masculine = feminine = neutral = common = diminutive = False
                partconsumed = False
                for part in translation.split(' '):
                    part=part.strip()
                    colon=part.find(':')
                    if colon!=-1:
                        colon2=part.find(':',colon+1)
                        pipe=part.find('|')
                        if colon2!=-1 and pipe!=-1:
                            # We found a link to another language Wiktionary
                            # This contains no interesting information to store
                            # If the target Wiktionary uses them, we'll create them upon output
                            pass
                        else:
                            translationremark = part.replace("'",'').replace('(','').replace(')','').replace(':','')
                        partconsumed = True
                    cleanpart=part.replace("'",'').lower()
                    delim=''
                    # XXX The following 3 tests look wrong:
                    # find() returns either -1 if the substring is not found,
                    # or the position of the substring in the string.
                    # since bool(-1) = True, cleanpart.find(',') will always
                    # be False, unless cleanpart[0] is ','
                    #
                    # the test "',' in cleanpart" might be the one to use.
                    if cleanpart.find(','):
                        delim=','
                    if cleanpart.find(';'):
                        delim=';'
                    if cleanpart.find('/'):
                        delim='/'
                    if 0 <= part.find("'") <= 2 or '{' in part:
                        if delim=='':
                            delim='|'
                            cleanpart=cleanpart+'|'
                        for maybegender in cleanpart.split(delim):
                            maybegender=maybegender.strip()
                            if maybegender=='m' or maybegender=='{{m}}':
                                masculine=True
                                partconsumed = True
                            if maybegender=='f' or maybegender=='{{f}}':
                                feminine=True
                                partconsumed = True
                            if maybegender=='n' or maybegender=='{{n}}':
                                neutral=True
                                partconsumed = True
                            if maybegender=='c' or maybegender=='{{c}}':
                                common=True
                                partconsumed = True
                            if maybegender=='p' or maybegender=='pl' or maybegender=='plural' or maybegender=='{{p}}':
                                number=2
                                partconsumed = True
                            if maybegender[:3]=='dim' or maybegender=='{{dim}}':
                                diminutive=True
                                partconsumed = True
 #                   print 'consumed: ', partconsumed
                    if not partconsumed:
                        # This must be our term
                        termweareworkingon=part.replace("[",'').replace("]",'').lower()
                        if '#' in termweareworkingon and '|' in termweareworkingon:
                            termweareworkingon=termweareworkingon.split('#')[0]
                # Now we have enough information to create a term
                # object for this translation and add it to our list
                addedflag=False
                if masculine:
                    thistrans = {'remark': translationremark, 'trans': term.Term(isolang,termweareworkingon,gender='m',number=number,diminutive=diminutive,wikiline=translation)}
                    translations.append(thistrans)
                    addedflag=True
                if feminine:
                    thistrans = {'remark': translationremark, 'trans': term.Term(isolang,termweareworkingon,gender='f',number=number,diminutive=diminutive,wikiline=translation)}
                    translations.append(thistrans)
                    addedflag=True
                if neutral:
                    thistrans = {'remark': translationremark, 'trans': term.Term(isolang,termweareworkingon,gender='n',number=number,diminutive=diminutive,wikiline=translation)}
                    translations.append(thistrans)
                    addedflag=True
                if common:
                    thistrans = {'remark': translationremark, 'trans': term.Term(isolang,termweareworkingon,gender='c',number=number,diminutive=diminutive,wikiline=translation)}
                    translations.append(thistrans)
                    addedflag=True
                # if it wasn't added by now, it's a term which has no gender indication
                if not addedflag:
                    thistrans = {'remark': translationremark, 'trans': term.Term(isolang,termweareworkingon,number=number,diminutive=diminutive)}
                    translations.append(thistrans)

            if not isolang:
                print "Houston, we have a problem. This line doesn't seem to contain an indication of the language:",translationswikiline
            self.translations[isolang] = {'remark':   translationsremark,
                                          'alltrans': translations }

    def hasSynonyms(self):
        """ Returns True if there are synonyms
            Returns False if there are no synonyms
        """
        if self.synonyms == []:
            return False
        else:
            return True

    def setTranslations(self,translations):
        """ Provide the translations  """
        self.translations=translations

    def getTranslations(self):
        """ Returns the translations dictionary containing translation
            Term objects for this meaning
        """
        return self.translations

    def addTranslation(self,translation):
        """ Add a translation Term object to the dictionary for this meaning
            The lang property of the Term object will be used as the key of the dictionary
        """
        self.translations.setdefault( translation.lang, [] ).append( translation )

    def addTranslations(self,*translations):
        """ This method calls addTranslation as often as necessary to add
            all the translations it receives
        """
        for translation in translations:
            self.addTranslation(translation)

    def hasTranslations(self):
        """ Returns True if there are translations
            Returns False if there are no translations
        """
        if self.translations == {}:
            return 0
        else:
            return 1

    def setLabel(self,label):
        self.label=label.replace('<!--','').replace('-->','')

    def getLabel(self):
        if self.label:
            return u'<!--' + self.label + u'-->'

    def setConciseDef(self,concisedef):
        self.concisedef=concisedef

    def getConciseDef(self):
        if self.concisedef:
            return self.concisedef

    def getExamples(self):
        """ Returns the list of example strings for this meaning
        """
        return self.examples

    def addExample(self,example):
        """ Add a translation Term object to the dictionary for this meaning
            The lang property of the Term object will be used as the key of the dictionary
        """
        self.examples.append(example)

    def addExamples(self,*examples):
        """ This method calls addExample as often as necessary to add
            all the examples it receives
        """
        for example in examples:
            self.addExample(example)

    def hasExamples(self):
        """ Returns True if there are examples
            Returns False if there are no examples
        """
        if self.examples == []:
            return 0
        else:
            return 1

    def wikiWrapSynonyms(self,wikilang):
        """ Returns a string with all the synonyms in a format ready for Wiktionary
        """
        first = 1
        wrappedsynonyms = ''
        for synonym in self.synonyms:
            if first==0:
                wrappedsynonyms += ', '
            else:
                first = 0
            wrappedsynonyms = wrappedsynonyms + synonym.wikiWrapForList(wikilang)
        return wrappedsynonyms + '\n'

    def wikiWrapTranslations(self,wikilang,entrylang):
        """ Returns a string with all the translations in a format
            ready for Wiktionary
            The behavior changes with the circumstances.
            For an entry in the same language as the Wiktionary the full list of translations is contained in the output, excluding the local
        language itself
            - This list of translations has to end up in a table with two columns
            - The first column of this table contains languages with names from A to M, the second contains N to Z
            - If a column in this list remains empty a html comment is put in that column
            For an entry in a foreign language only the translation towards the local language is output.
        """
        if wikilang == entrylang:
            # When treating an entry of the same lang as the Wiktionary, we want to output the translations in such a way that they end up sorted alphabetically on the language name in the language of the current Wiktionary
            alllanguages=self.translations.keys()
            alllanguages.sort(sortonname(langnames[wikilang]))
            wrappedtranslations = structs.wiktionaryformats[wikilang]['transbefore'] + '\n'
            alreadydone = 0
            for language in alllanguages:
                if language == wikilang: continue # don't output translation for the wikilang itself
                # split translations into two column table
                if not alreadydone and langnames[wikilang][language][0:1].upper() > 'M':
                    wrappedtranslations = wrappedtranslations + structs.wiktionaryformats[wikilang]['transinbetween'] + '\n'
                    alreadydone = 1
                # Indicating the language according to the wikiformats dictionary
                wrappedtranslations = wrappedtranslations + structs.wiktionaryformats[wikilang]['translang'].replace('%%langname%%',langnames[wikilang][language]).replace('%%ISOLangcode%%',language) + ': '
                first = 1
                for translation in self.translations[language]:
                    termweareworkingon=translation.term
                    if first==0:
                        wrappedtranslations += ', '
                    else:
                        first = 0
                    wrappedtranslations = wrappedtranslations + translation.wikiWrapAsTranslation(wikilang)
                wrappedtranslations += '\n'
            if not alreadydone:
                wrappedtranslations = wrappedtranslations + structs.wiktionaryformats[wikilang]['transinbetween'] + '\n' + structs.wiktionaryformats[wikilang]['transnoNtoZ'] + '\n'
                alreadydone = 1
            wrappedtranslations = wrappedtranslations + structs.wiktionaryformats[wikilang]['transafter'] + '\n'
        else:
            # For the other entries we want to output the translation in the language of the Wiktionary
            wrappedtranslations = structs.wiktionaryformats[wikilang]['translang'].replace('%%langname%%',langnames[wikilang][wikilang]).replace('%%ISOLangcode%%',wikilang) + ': '
            first = True
            for translation in self.translations[wikilang]:
                termweareworkingon=translation.term
                if first==False:
                    wrappedtranslations += ', '
                else:
                    first = False
                wrappedtranslations = wrappedtranslations + translation.wikiWrapAsTranslation(wikilang)
        return wrappedtranslations

    def showContents(self,indentation):
        """ Prints the contents of this meaning.
            Every subobject is indented a little further on the screen.
            The primary purpose is to help keep one's sanity while debugging.
        """
        print ' ' * indentation + 'term: '
        self.term.showContents(indentation+2)
        print ' ' * indentation + 'definition = %s'% self.definition
        print ' ' * indentation + 'etymology = %s'% self.etymology

        print ' ' * indentation + 'Synonyms:'
        for synonym in self.synonyms:
            synonym.showContents(indentation+2)

        print ' ' * indentation + 'Translations:'
        translationkeys = self.translations.keys()
        for translationkey in translationkeys:
            for translation in self.translations[translationkey]:
                translation.showContents(indentation+2)

    def wikiWrapExamples(self):
        """ Returns a string with all the examples in a format ready for Wiktionary
        """
        wrappedexamples = ''
        for example in self.examples:
            wrappedexamples = wrappedexamples + "#:'''" + example + "'''\n"
        return wrappedexamples
