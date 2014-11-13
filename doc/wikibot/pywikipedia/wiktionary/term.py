#!/usr/bin/python
# -*- coding: utf-8  -*-

import structs

class Term:
    """ This is a superclass for terms.  """
    def __init__(self,lang,term,relatedwords=[],gender='',number=1,diminutive=False,wikiline=u''):
        """ Constructor
            Generally called with two parameters:
            - The language of the term
            - The term (string)

            - relatedwords (list of Term objects) is optional
        """
        self.lang=lang
        self.term=term
        self.relatedwords=relatedwords
        self.gender=gender # m: masculine, f: feminine, n: neutral, c: common
        self.number=number # 1: singular, 2: plural
        self.diminutive=diminutive # True: diminutive, False: not a diminutive

        if wikiline:
            pos=wikiline.find("''")
            if pos==-1:
                pos=wikiline.find("{{")
            if pos==-1:
                pos=len(wikiline)
            maybegender=wikiline[pos:].replace("'",'').replace('{','').replace('}','').strip()
            self.term=wikiline[:pos].replace("[",'').replace(']','').strip()
            if 'm' in maybegender:
                self.gender='m'
            if 'f' in maybegender:
                self.gender='f'
            if 'n' in maybegender:
                self.gender='n'
            if 'c' in maybegender:
                self.gender='c'
            if 'p' in maybegender:
                self.number=2
            if 'dim' in maybegender:
                self.diminutive=True

    def __getitem__(self):
        """ Documenting as an afterthought is a bad idea
            I don't know anymore why I added this, but I'm pretty sure it was in response to an error message
        """
        return self

    def setTerm(self,term):
        self.term=term

    def getTerm(self):
        return self.term

    def setLang(self,lang):
        self.lang=lang

    def getLang(self):
        return self.lang

    def setGender(self,gender):
        self.gender=gender

    def getGender(self):
        return(self.gender)

    def setNumber(self,number):
        self.number=number

    def getNumber(self):
        return(self.number)

#    def setLabel(self,label):
#        self.label=label.replace('<!--','').replace('-->','')

#    def getLabel(self):
#        if self.label:
#            return '<!--' + self.label + '-->'

    def wikiWrapGender(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it is applicable
        """
        if self.gender:
            return ' ' + structs.wiktionaryformats[wikilang]['gender'].replace('%%gender%%',self.gender)
        else:
            return ''

    def wikiWrapAsExample(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it exists
        """
        return structs.wiktionaryformats[wikilang]['beforeexampleterm'] + self.term + structs.wiktionaryformats[wikilang]['afterexampleterm']

    def wikiWrapForList(self,wikilang):
        """ Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return '[[' + self.term + ']]'

    def wikiWrapAsTranslation(self,wikilang):
        """    Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return '[[' + self.term + ']]'

    def showContents(self,indentation):
        """ Prints the contents of this Term.
            Every subobject is indented a little further on the screen.
            The primary purpose is to help keep one's sanity while debugging.
        """
        print ' ' * indentation + 'lang = %s'% self.lang
        print ' ' * indentation + 'pos = %s'% self.pos
        print ' ' * indentation + 'term = %s'% self.term
        print ' ' * indentation + 'relatedwords = %s'% self.relatedwords

class Noun(Term):
    """ This class inherits from Term.
        It adds properties and methods specific to nouns
    """
    def __init__(self,lang,term,gender='',number=1,diminutive=False):
        """ Constructor
            Generally called with two parameters:
            - The language of the term
            - The term (string)

            - gender is optional
        """
        self.pos='noun'        # part of speech
        Term.__init__(self,lang,term,gender=gender,number=number,diminutive=diminutive)

    def showContents(self,indentation):
        Term.showContents(self,indentation)
        print ' ' * indentation + 'gender = %s'% self.gender

    def wikiWrapAsExample(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it exists
        """
        return Term.wikiWrapAsExample(self, wikilang) + Term.wikiWrapGender(self,wikilang)

    def wikiWrapForList(self,wikilang):
        """ Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return Term.wikiWrapForList(self, wikilang) + Term.wikiWrapGender(self, wikilang)

    def wikiWrapAsTranslation(self,wikilang):
        """    Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return Term.wikiWrapAsTranslation(self, wikilang) + Term.wikiWrapGender(self, wikilang)

class Adjective(Term):
    def __init__(self,lang,term,gender='',number=1):
        self.pos='adjective'        # part of speech
        Term.__init__(self,lang,term,gender=gender,number=number)

    def wikiWrapAsExample(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it exists
        """
        return Term.wikiWrapAsExample(self, wikilang) + Term.wikiWrapGender(self,wikilang)

    def wikiWrapForList(self,wikilang):
        """ Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return Term.wikiWrapForList(self, wikilang) + Term.wikiWrapGender(self, wikilang)

    def wikiWrapAsTranslation(self,wikilang):
        """    Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return Term.wikiWrapAsTranslation(self, wikilang) + Term.wikiWrapGender(self, wikilang)

class Verb(Term):
    def __init__(self,lang,term):
        self.pos='verb'        # part of speech
        Term.__init__(self,lang,term)

    def showContents(self,indentation):
        Term.showContents(self,indentation)

    def wikiWrapForList(self,wikilang):
        """ Returns a string with this term as a link in a format ready for Wiktionary
        """
        if wikilang=='en':
            if self.term.lower().startswith('to '):
                return 'to [[' + self.term[3:] + ']]'
        return Term.wikiWrapForList(self, wikilang)

    def wikiWrapAsTranslation(self,wikilang):
        """ Returns a string with this term as a link in a format ready for Wiktionary
        """
        return Verb.wikiWrapForList(self, wikilang)
