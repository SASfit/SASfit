#!/usr/bin/python
# -*- coding: utf-8  -*-

'''
This module contains code to store Wiktionary content in Python objects.
The objects can output the content again in Wiktionary format by means of the wikiWrap methods

I'm currently working on a parser that can read the textual version in the various Wiktionary formats and store what it finds in the Python objects.

The data dictionaries will be moved to a separate file, later on. Right now it's practical to have everything together. They also still need to be expanded to contain more languages and more Wiktionary formats. Right now I like to keep everything together to keep my sanity.

The code is still very much alpha level and the scope of what it can do is still rather limited, only 3 parts of speech, only 2 different Wiktionary output formats, only langnames matrix for about 8 languages. On of the things on the todo list is to harvest the content of this matrix dictionary from the various Wiktionary projects. GerardM put them all in templates already.
'''

__version__='$Id: wiktionary.py 6744 2009-04-28 05:35:32Z nicdumz $'

#from editarticle import EditArticle
#import wikipedia
import copy

isolangs = ['af','sq','ar','an','hy','ast','tay','ay','az','bam','eu','bn','my','bi','bs','br','bg','sro','ca','zh','chp','rmr','co','dgd','da','de','eml','en','eo','et','fo','fi','fr','cpf','fy','fur','gl','ka','el','gu','hat','haw','he','hi','hu','io','ga','is','gil','id','ia','it','ja','jv','ku','kok','ko','hr','lad','la','lv','ln','li','lt','lb','src','ma','ms','mg','mt','mnc','mi','mr','mh','mas','myn','mn','nah','nap','na','nds','no','ny','oc','uk','oen','grc','pau','pap','pzh','fa','pl','pt','pa','qu','rap','roh','ra','ro','ja-ro','ru','smi','sm','sa','sc','sco','sr','sn','si','sk','sl','so','sov','es','scn','su','sw','tl','tt','th','ti','tox','cs','che','tn','tum','tpn','tr','ts','tvl','ur','vi','vo','wa','cy','be','wo','xh','zu','sv']

wiktionaryformats = {
    'nl': {
        'langheader': u'{{-%%ISOLangcode%%-}}',
        'translang': u':*{{%%ISOLangcode%%}}',
        'beforeexampleterm': u"'''",
        'afterexampleterm': u"'''",
        'gender': u"{{%%gender%%}}",
        'posheader': {
                'noun': u'{{-noun-}}',
                'adjective': u'{{-adj-}}',
                'verb': u'{{-verb-}}',
                },
        'translationsheader': u"{{-trans-}}",
        'transbefore': u'{{top}}',
        'transinbetween': u'{{mid}}',
        'transafter': u'{{after}}',
        'transnoAtoM': u'<!-- Vertalingen van A tot M komen hier-->',
        'transnoNtoZ': u'<!-- Vertalingen van N tot Z komen hier-->',
        'synonymsheader': u"{{-syn-}}",
        'relatedheader': u'{{-rel-}}',
        },
    'en': {
        'langheader': u'==%%langname%%==',
        'translang': u'*%%langname%%',
        'beforeexampleterm': u"'''",
        'afterexampleterm': u"'''",
        'gender': u"''%%gender%%''",
        'posheader': {
                'noun': u'===Noun===',
                'adjective': u'===Adjective===',
                'verb': u'===Verb===',
                },
        'translationsheader': u"====Translations====",
        'transbefore': u'{{top}}',
        'transinbetween': u'{{mid}}',
        'transafter': u'{{after}}',
        'transnoAtoM': u'<!-- Translations from A tot M go here-->',
        'transnoNtoZ': u'<!-- Translations from N tot Z go here-->',
        'synonymsheader': u"====Synonyms====",
        'relatedheader': u'===Related words===',
        }
}

pos = {
    u'noun': u'noun',
    u'adjective': u'adjective',
    u'verb': u'verb',
}

otherheaders = {
    u'see also': u'seealso',
    u'see': u'seealso',
    u'translations': u'trans',
    u'trans': u'trans',
    u'synonyms': u'syn',
    u'syn': u'syn',
    u'antonyms': u'ant',
    u'ant': u'ant',
    u'pronunciation': u'pron',
    u'pron': u'pron',
    u'related terms': u'rel',
    u'rel': u'rel',
    u'acronym': u'acr',
    u'acr': u'acr',
    u'etymology': u'etym',
    u'etym': u'etym',
}

langnames = {
    'nl':    {
        'translingual' : u'Taalonafhankelijk',
        'nl' : u'Nederlands',
        'en' : u'Engels',
        'de' : u'Duits',
        'fr' : u'Frans',
        'it' : u'Italiaans',
        'eo' : u'Esperanto',
        'es' : u'Spaans',
        },
     'de':    {
        'translingual' : u'???',
        'nl' : u'Niederländisch',
        'en' : u'Englisch',
        'de' : u'Deutsch',
        'fr' : u'Französisch',
        'it' : u'Italienisch',
        'eo' : u'Esperanto',
        'es' : u'Spanisch',
        },
     'en':    {
        'translingual' : u'Translingual',
        'nl' : u'Dutch',
        'en' : u'English',
        'de' : u'German',
        'fr' : u'French',
        'it' : u'Italian',
        'eo' : u'Esperanto',
        'es' : u'Spanish',
        },
    'eo':    {
        'translingual' : u'???',
        'nl' : u'Nederlanda',
        'en' : u'Angla',
        'de' : u'Germana',
        'fr' : u'Franca',
        'it' : u'Italiana',
        'eo' : u'Esperanto',
        'es' : u'Hispana',
        },
     'ia':    {
        'translingual' : u'translingual',
        'nl' : u'nederlandese',
        'en' : u'anglese',
        'de' : u'germano',
        'fr' : u'francese',
        'it' : u'italiano',
        'eo' : u'esperanto',
        'es' : u'espaniol',
        },
    'it':    {
        'translingual' : u'???',
        'nl' : u'olandese',
        'en' : u'inglese',
        'de' : u'tedesco',
        'fr' : u'francese',
        'it' : u'italiano',
        'eo' : u'esperanto',
        'es' : u'spagnuolo',
        },
    'fr':    {
        'translingual' : u'???',
        'nl' : u'néerlandais',
        'en' : u'anglais',
        'de' : u'allemand',
        'fr' : u'français',
        'it' : u'italien',
        'eo' : u'espéranto',
        'es' : u'espagnol',
        },
    'es':    {
        'translingual' : u'???',
        'nl' : u'olandés',
        'en' : u'inglés',
        'de' : u'alemán',
        'fr' : u'francés',
        'it' : u'italiano',
        'eo' : u'esperanto',
        'es' : u'español',
        },
    'sr':    {
        'translingual' : u'Вишејезички',
        'nl' : u'холандски',
        'en' : u'енглески',
        'de' : u'немачки',
        'fr' : u'француски',
        'it' : u'италијански',
        'eo' : u'есперанто',
        'es' : u'шпански',
        },
}

def invertlangnames():
    '''
    On the English Wiktionary it is customary to use full language names. For
    parsing we need a dictionary to efficiently convert these back to iso
    abbreviations.
    '''
    invertedlangnames = {}
    for ISOKey in langnames.keys():
        for ISOKey2 in langnames[ISOKey].keys():
            lowercaselangname=langnames[ISOKey][ISOKey2].lower()
            #Put in the names of the languages so we can easily do a reverse lookup lang name -> iso abbreviation
            invertedlangnames.setdefault(lowercaselangname, ISOKey2)
            # Now all the correct forms are in, but we also want to be able to find them when there are typos in them
            for index in range(1,len(lowercaselangname)):
                # So first we create all the possibilities with one letter gone
                invertedlangnames.setdefault(lowercaselangname[:index]+lowercaselangname[index+1:], ISOKey2)
                # Then we switch two consecutive letters
                invertedlangnames.setdefault(lowercaselangname[:index-1]+lowercaselangname[index]+lowercaselangname[index-1]+lowercaselangname[index+1:], ISOKey2)
                # There are of course other typos possible, but this caters for a lot of possibilities already
                # TODO One other treatment that would make sense is to filter out the accents.
    return invertedlangnames

def createPOSlookupDict():
    for key in pos.keys():
        lowercasekey=key.lower()
        value=pos[key]
        for index in range(1,len(lowercasekey)):
            # So first we create all the possibilities with one letter gone
            pos.setdefault(lowercasekey[:index]+lowercasekey[index+1:], value)
            # Then we switch two consecutive letters
            pos.setdefault(lowercasekey[:index-1]+lowercasekey[index]+lowercasekey[index-1]+lowercasekey[index+1:], value)
            # There are of course other typos possible, but this caters for a lot of possibilities already
    return pos

def createOtherHeaderslookupDict():
    for key in otherheaders.keys():
        lowercasekey=key.lower()
        value=otherheaders[key]
        for index in range(1,len(lowercasekey)):
            # So first we create all the possibilities with one letter gone
            otherheaders.setdefault(lowercasekey[:index]+lowercasekey[index+1:], value)
            # Then we switch two consecutive letters
            otherheaders.setdefault(lowercasekey[:index-1]+lowercasekey[index]+lowercasekey[index-1]+lowercasekey[index+1:], value)
            # There are of course other typos possible, but this caters for a lot of possibilities already
    return otherheaders

# A big thanks to Rob Hooft for the following class:
# It may not seem like much, but it magically allows the translations to be sorted on
# the names of the languages. I would never have thought of doing it like this myself.
class sortonname:
    '''
    This class sorts translations alphabetically on the name of the language,
    instead of on the iso abbreviation that is used internally.
    '''
    def __init__(self, lang):
        self.lang = lang

    def __call__(self, one, two):
        return cmp(self.lang[one], self.lang[two])

class WiktionaryPage:
    """ This class contains all that can appear on one Wiktionary page """

    def __init__(self,wikilang,term):    # wikilang here refers to the language of the Wiktionary this page belongs to
        """ Constructor
            Called with two parameters:
            - the language of the Wiktionary the page belongs to
            - the term that is described on this page
        """
        self.wikilang=wikilang
        self.term=term
        self.entries = {}        # entries is a dictionary of entry objects indexed by entrylang
        self.sortedentries = []
        self.interwikilinks = []
        self.categories = []

    def setWikilang(self,wikilang):
        """ This method allows to switch the language on the fly """
        self.wikilang=wikilang

    def addEntry(self,entry):
        """ Add an entry object to this page object """
#        self.entries.setdefault(entry.entrylang, []).append(entry)
        self.entries[entry.entrylang]=entry

    def listEntries(self):
        """ Returns a dictionary of entry objects for this entry """
        return self.entries

    def sortEntries(self):
        """ Sorts the sortedentries list containing the keys of the entry
            objects dictionary for this entry
        """

        if not self.entries == {}:
            self.sortedentries = self.entries.keys()
            self.sortedentries.sort(sortonname(langnames[self.wikilang]))

            try:
                samelangentrypos=self.sortedentries.index(self.wikilang)
            except (ValueError):
                # wikilang isn't in the list, do nothing
                pass
            else:
                samelangentry=self.sortedentries[samelangentrypos]
                self.sortedentries.remove(self.wikilang)
                self.sortedentries.insert(0,samelangentry)

            try:
                translingualentrypos=self.sortedentries.index(u'translingual')
            except (ValueError):
                # translingual isn't in the list, do nothing
                pass
            else:
                translingualentry=self.sortedentries[translingualentrypos]
                self.sortedentries.remove(u'translingual')
                self.sortedentries.insert(0,translingualentry)

    def addLink(self,link):
        """ Add a link to another wikimedia project """
        link=link.replace('[','').replace(']','')
        pos=link.find(':')
        if pos!=1:
            link=link[:pos]
        self.interwikilinks.append(link)
 #       print self.interwikilinks

    def addCategory(self,category):
        """ Add a link to another wikimedia project """
        self.categories.append(category)

    def parseWikiPage(self,content):
        '''This function will parse the content of a Wiktionary page
           and read it into our object structure.
           It returns a list of dictionaries. Each dictionary contains a header object
           and the textual content found under that header. Only relevant content is stored.
           Empty lines and lines to create tables for presentation to the user are taken out.'''

        templist = []
        context = {}

        splitcontent=[]
        content=content.split('\n')
        for line in content:
 #           print line
            # Let's get rid of line breaks and extraneous white space
            line=line.replace('\n','').strip()
            lower = line.lower()
            # Let's start by looking for general stuff, that provides information which is
            # interesting to store at the page level
            if '{wikipedia}' in lower:
                self.addLink('wikipedia')
                continue
            if '[[category:' in lower:
                category=line.split(':')[1].replace(']','')
                self.addCategory(category)
#                print 'category: ', category
                continue
            if '|' not in line:
                bracketspos=line.find('[[')
                colonpos=line.find(':')
                if bracketspos!=-1 and colonpos!=-1 and bracketspos < colonpos:
                    # This seems to be an interwikilink
                    # If there is a pipe in it, it's not a simple interwikilink
                    linkparts=line.replace(']','').replace('[','').split(':')
                    lang=linkparts[0]
                    linkto=linkparts[1]
                    if len(lang)>1 and len(lang)<4:
                        self.addLink(lang+':'+linkto)
                    continue
            # store empty lines literally, this necessary for the blocks we don't parse
            # and will return literally
            if len(line) <2:
                templist.append(line)
                continue
#        print 'line0:',line[0], 'line-2:',line[-2],'|','stripped line-2',line.rstrip()[-2]
            if line.strip()[0]=='='and line.rstrip()[-2]=='=' or '{{-' in line and '-}}' in line:
                # When a new header is encountered, it is necessary to store the information
                # encountered under the previous header.
                if templist:
                    tempdictstructure={'text': templist,
                                       'header': header,
                                       'context': copy.copy(context),
                                      }
                    templist=[]
                    splitcontent.append(tempdictstructure)
#                print "splitcontent: ",splitcontent,"\n\n"
                header=Header(line)
#                print "Header parsed:",header.level, header.header, header.type, header.contents
                if header.type==u'lang':
                    context['lang']=header.contents
                if header.type==u'pos':
                    if 'lang' not in context:
                        # This entry lacks a language indicator,
                        # so we assume it is the same language as the Wiktionary we're working on
                        context['lang']=self.wikilang
                    context['pos']=header.contents

            else:
                # It's not a header line, so we add it to a temporary list
                # containing content lines
                if header.contents==u'trans':
                    # Under the translations header there is quite a bit of stuff
                    # that's only needed for formatting, we can just skip that
                    # and go on processing the next line
                    lower = line.lower()
                    if '{top}' in lower: continue
                    if '{mid}' in lower: continue
                    if '{bottom}' in lower: continue
                    if '|-' in line: continue
                    if '{|' in line: continue
                    if '|}' in line: continue
                    if 'here-->' in lower: continue
                    if 'width=' in lower: continue
                    if '<!--left column' in lower: continue
                    if '<!--right column' in lower: continue

                templist.append(line)

            # Let's not forget the last block that was encountered
            if templist:
                tempdictstructure={'text': templist,
                                   'header': header,
                                   'context': copy.copy(context),
                                  }
                splitcontent.append(tempdictstructure)


        # make sure variables are defined
        gender = sample = plural = diminutive = label = definition = ''
        examples = []
        for contentblock in splitcontent:
#            print "contentblock:",contentblock
#            print contentblock['header']
            # Now we parse the text blocks.
            # Let's start by describing what to do with content found under the POS header
            if contentblock['header'].type==u'pos':
                flag=False
                for line in contentblock['text']:
#                    print line
                    if line[:3] == "'''":
                        # This seems to be an ''inflection line''
                        # It can be built up like this: '''sample'''
                        # Or more elaborately like this: '''staal''' ''n'' (Plural: [[stalen]],     diminutive: [[staaltje]])
                        # Or like this: {{en-infl-reg-other-e|ic|e}}
                        # Let's first get rid of parentheses and brackets:
                        line=line.replace('(','').replace(')','').replace('[','').replace(']','')
                        # Then we can split it on the spaces
                        for part in line.split(' '):
#                            print part[:3], "Flag:", flag
                            if flag==False and part[:3] == "'''":
                                sample=part.replace("'",'').strip()
#                                print 'Sample:', sample
                                # OK, so this should be an example of the term we are describing
                                # maybe it is necessary to compare it to the title of the page
                            if sample:
                                maybegender=part.replace("'",'').replace("}",'').replace("{",'').lower()
                                if maybegender=='m':
                                   gender='m'
                                if maybegender=='f':
                                   gender='f'
                                if maybegender=='n':
                                    gender='n'
                                if maybegender=='c':
                                    gender='c'
#                            print 'Gender: ',gender
                            if part.replace("'",'')[:2].lower()=='pl':
                                flag='plural'
                            if part.replace("'",'')[:3].lower()=='dim':
                                flag='diminutive'
                            if flag=='plural':
                                plural=part.replace(',','').replace("'",'').strip()
#                                print 'Plural: ',plural
                            if flag=='diminutive':
                                diminutive=part.replace(',','').replace("'",'').strip()
#                                print 'Diminutive: ',diminutive
                    if line[:2] == "{{":
                        # Let's get rid of accolades:
                        line=line.replace('{','').replace('}','')
                        # Then we can split it on the dashes
                        parts=line.split('-')
                        lang=parts[0]
                        what=parts[1]
                        mode=parts[2]
                        other=parts[3]
                        infl=parts[4].split('|')
                    if sample:
                        # We can create a Term object
                        # TODO which term object depends on the POS
#                        print "contentblock['context'].['lang']", contentblock['context']['lang']
                        if contentblock['header'].contents=='noun':
                            theterm=Noun(lang=contentblock['context']['lang'], term=sample, gender=gender)
                        if contentblock['header'].contents=='verb':
                            theterm=Verb(lang=contentblock['context']['lang'], term=sample)
                        sample=''
#                        raw_input("")
                    if line[:1].isdigit():
                        # Somebody didn't like automatic numbering and added static numbers
                        # of their own. Let's get rid of them
                        while line[:1].isdigit():
                            line=line[1:]
                        # and replace them with a hash, so the following if block picks it up
                        line = '#' + line
                    if line[:1] == "#":
                        # This probably is a definition
                        # If we already had a definition we need to store that one's data
                        # in a Meaning object and make that Meaning object part of the Page object
                        if definition:
                            ameaning = Meaning(term=theterm,definition=definition, label=label, examples=examples)

                            # sample
                            # plural and diminutive belong with the Noun object
                            # comparative and superlative belong with the Adjective object
                            # conjugations belong with the Verb object

                            # Reset everything for the next round
                            sample = plural = diminutive = label = definition = ''
                            examples = []

                            if contentblock['context']['lang'] not in self.entries:
                                # If no entry for this language has been foreseen yet
                                # let's create one
                                anentry = Entry(contentblock['context']['lang'])
                                # and add it to our page object
                                self.addEntry(anentry)
                            # Then we can easily add this meaning to it.
                            anentry.addMeaning(ameaning)

                        pos=line.find('<!--')
                        if pos < 4:
                            # A html comment at the beginning of the line means this entry already has disambiguation labels, great
                            pos2=line.find('-->')
                            label=line[pos+4:pos2]
                            definition=line[pos2+1:]
#                            print 'label:',label
                        else:
                            definition=line[1:]
#                        print "Definition: ", definition
                    if line[:2] == "#:":
                        # This is an example for the preceding definition
                        example=line[2:]
#                        print "Example:", example
                        examples.add(example)
            # Make sure we store the last definition
            if definition:
                ameaning = Meaning(term=theterm, definition=definition, label=label, examples=examples)
                if contentblock['context']['lang'] not in self.entries:
                    # If no entry for this language has been foreseen yet
                    # let's create one
                    anentry = Entry(contentblock['context']['lang'])
                    # and add it to our page object
                    self.addEntry(anentry)
                    # Then we can easily add this meaning to it.
                    anentry.addMeaning(ameaning)
 #            raw_input("")

    def wikiWrap(self):
        """ Returns a string that is ready to be submitted to Wiktionary for
            this page
        """
        page = ''
        self.sortEntries()
        # print "sorted: %s",self.sortedentries
        first = True
        print "SortedEntries:", self.sortedentries, len(self.sortedentries)
        for index in self.sortedentries:
            print "Entries:", self.entries[index]
            entry = self.entries[index]
            print entry
            if first == False:
                page = page + '\n----\n'
            else:
                first = False
            page = page + entry.wikiWrap(self.wikilang)
        # Add interwiktionary links at bottom of page
        for link in self.interwikilinks:
            page = page + '[' + link + ':' + self.term + ']\n'

        return page

    def showContents(self):
        """ Prints the contents of all the subobjects contained in this page.
            Every subobject is indented a little further on the screen.
            The primary purpose is to help keep one's sanity while debugging.
        """
        indentation = 0
        print ' ' * indentation + 'wikilang = %s' % self.wikilang

        print ' ' * indentation + 'term = %s' % self.term

        entrieskeys = self.entries.keys()
        for entrieskey in entrieskeys:
            for entry in self.entries[entrieskey]:
                entry.showContents(indentation+2)

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
        entry = wiktionaryformats[wikilang]['langheader'].replace('%%langname%%',langnames[wikilang][self.entrylang]).replace('%%ISOLangcode%%',self.entrylang) + '\n'

        for pos in self.posorder:
            meanings = self.meanings[pos]

            entry += wiktionaryformats[wikilang]['posheader'][pos]
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
                entry = entry + wiktionaryformats[wikilang]['synonymsheader'] + '\n'
                for meaning in meanings:
                    entry = entry + '*' + meaning.getLabel() + "'''" + meaning.getConciseDef() + "''': " + meaning.wikiWrapSynonyms(wikilang)
                entry +='\n'

            if meaning.hasTranslations():
                entry = entry + wiktionaryformats[wikilang]['translationsheader'] + '\n'
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

class Meaning:
    """ This class contains one meaning for a word or an expression.
    """
    def __init__(self,term,definition='',etymology='',synonyms=[],translations={},label='',concisedef='',examples=[]):
        """ Constructor
            Generally called with one parameter:
            - The Term object we are describing

            - definition (string) for this term is optional
            - etymology (string) is optional
            - synonyms (list of Term objects) is optional
            - translations (dictionary of Term objects, ISO639 is the key) is optional
        """
        self.term=term
        self.definition=definition
        self.concisedef=concisedef
        self.etymology=etymology
        self.synonyms=synonyms
        self.examples=examples
        self.label=label

        if translations: # Why this has to be done explicitly is beyond me, but it doesn't work correctly otherwise
            self.translations=translations
        else:
            self.translations={}
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
            wrappedtranslations = wiktionaryformats[wikilang]['transbefore'] + '\n'
            alreadydone = 0
            for language in alllanguages:
                if language == wikilang: continue # don't output translation for the wikilang itself
                # split translations into two column table
                if not alreadydone and langnames[wikilang][language][0:1].upper() > 'M':
                    wrappedtranslations = wrappedtranslations + wiktionaryformats[wikilang]['transinbetween'] + '\n'
                    alreadydone = 1
                # Indicating the language according to the wikiformats dictionary
                wrappedtranslations = wrappedtranslations + wiktionaryformats[wikilang]['translang'].replace('%%langname%%',langnames[wikilang][language]).replace('%%ISOLangcode%%',language) + ': '
                first = 1
                for translation in self.translations[language]:
                    term=translation.term
                    if first==0:
                        wrappedtranslations += ', '
                    else:
                        first = 0
                    wrappedtranslations = wrappedtranslations + translation.wikiWrapAsTranslation(wikilang)
                wrappedtranslations += '\n'
            if not alreadydone:
                wrappedtranslations = wrappedtranslations + wiktionaryformats[wikilang]['transinbetween'] + '\n' + wiktionaryformats[wikilang]['transnoNtoZ'] + '\n'
                alreadydone = 1
            wrappedtranslations = wrappedtranslations + wiktionaryformats[wikilang]['transafter'] + '\n'
        else:
            # For the other entries we want to output the translation in the language of the Wiktionary
            wrappedtranslations = wiktionaryformats[wikilang]['translang'].replace('%%langname%%',langnames[wikilang][wikilang]).replace('%%ISOLangcode%%',wikilang) + ': '
            first = True
            for translation in self.translations[wikilang]:
                term=translation.term
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


class Term:
    """ This is a superclass for terms.  """
    def __init__(self,lang,term,relatedwords=[]): # ,label=''):
        """ Constructor
            Generally called with two parameters:
            - The language of the term
            - The term (string)

            - relatedwords (list of Term objects) is optional
        """
        self.lang=lang
        self.term=term
        self.relatedwords=relatedwords
#        self.label=label

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

#    def setLabel(self,label):
#        self.label=label.replace('<!--','').replace('-->','')

#    def getLabel(self):
#        if self.label:
#            return '<!--' + self.label + '-->'

    def wikiWrapGender(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it is applicable
        """
        if self.gender:
            return ' ' + wiktionaryformats[wikilang]['gender'].replace('%%gender%%',self.gender)
        else:
            return ''

    def wikiWrapAsExample(self,wikilang):
        """ Returns a string with the gender in a format ready for Wiktionary, if it exists
        """
        return wiktionaryformats[wikilang]['beforeexampleterm'] + self.term + wiktionaryformats[wikilang]['afterexampleterm'] + self.wikiWrapGender(wikilang)

    def wikiWrapForList(self,wikilang):
        """ Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return '[[' + self.term + ']]' + self.wikiWrapGender(wikilang)

    def wikiWrapAsTranslation(self,wikilang):
        """    Returns a string with this term as a link followed by the gender in a format ready for Wiktionary
        """
        return '[[' + self.term + ']]' + self.wikiWrapGender(wikilang)

    def showContents(self,indentation):
        """ Prints the contents of this Term.
            Every subobject is indented a little further on the screen.
            The primary purpose is to help keep your sanity while debugging.
        """
        print ' ' * indentation + 'lang = %s'% self.lang
        print ' ' * indentation + 'pos = %s'% self.pos
        print ' ' * indentation + 'term = %s'% self.term
        print ' ' * indentation + 'relatedwords = %s'% self.relatedwords

class Noun(Term):
    """ This class inherits from Term.
        It adds properties and methods specific to nouns
    """
    def __init__(self,lang,term,gender=''):
        """ Constructor
            Generally called with two parameters:
            - The language of the term
            - The term (string)

            - gender is optional
        """
        self.pos='noun'        # part of speech
        self.gender=gender
        Term.__init__(self,lang,term)

    def setGender(self,gender):
        self.gender=gender

    def getGender(self):
        return(self.gender)

    def showContents(self,indentation):
        Term.showContents(self,indentation)
        print ' ' * indentation + 'gender = %s'% self.gender

class Adjective(Term):
    def __init__(self,lang,term,gender=''):
        self.pos='adjective'        # part of speech
        self.gender=gender
        Term.__init__(self,lang,term)

    def setGender(self,gender):
        self.gender=gender

    def getGender(self):
        return(self.gender)

    def showContents(self,indentation):
        Term.showContents(self,indentation)
        print ' ' * indentation + 'gender = %s'% self.gender

class Header:
    def __init__(self,line):
        """ Constructor
            Generally called with one parameter:
            - The line read from a Wiktonary page
              after determining it's probably a header
        """
        self.type=''        # The type of header, i.e. lang, pos, other
        self.contents=''    # If lang, which lang? If pos, which pos?

        self.level=None
        self.header = ''

        if line.count('=')>1:
            self.level = line.count('=') // 2 # integer floor division without fractional part
            self.header = line.replace('=','')
        elif '{{' in line:
            self.header = line.replace('{{-','').replace('-}}','')

        self.header = self.header.replace('{{','').replace('}}','').strip().lower()

        # Now we know the content of the header, let's try to find out what it means:
        if self.header in pos:
            self.type=u'pos'
            self.contents=pos[self.header]
        if self.header in langnames:
            self.type=u'lang'
            self.contents=self.header
        if self.header in invertedlangnames:
            self.type=u'lang'
            self.contents=invertedlangnames[self.header]
        if self.header in otherheaders:
            self.type=u'other'
            self.contents=otherheaders[self.header]

    def __repr__(self):
        return self.__module__+".Header("+\
            "contents='"+self.contents+\
            "', header='"+self.header+\
            "', level="+str(self.level)+\
            ", type='"+self.type+\
            "')"


def temp():
    """
    apage = WiktionaryPage('nl',u'iemand')
#    print 'Wiktionary language: %s'%apage.wikilang
#    print 'Wiktionary apage: %s'%apage.term
#    print
    aword = Noun('nl',u'iemand')
#    print 'Noun: %s'%aword.term
    aword.setGender('m')
#    print 'Gender: %s'%aword.gender
    frtrans = Noun('fr',u"quelqu'un")
    frtrans.setGender('m')
    entrans1 = Noun('en',u'somebody')
    entrans2 = Noun('en',u'someone')
#    print 'frtrans: %s'%frtrans

    ameaning = Meaning(aword, definition=u'een persoon')
    ameaning.addTranslation(frtrans)
#    print ameaning.translations
    ameaning.addTranslation(entrans1)
#    print ameaning.translations
    ameaning.addTranslation(entrans2)
#    print ameaning.translations
    ameaning.addTranslation(aword) # This is for testing whether the order of the translations is correct

    anentry = Entry('en')
    anentry.addMeaning(ameaning)

    apage.addEntry(anentry)

    print
    t=apage.wikiWrap()
    print t
    apage.wikilang = 'en'
    print
    t=apage.wikiWrap()
    print t
    """
    apage = WiktionaryPage('nl',u'Italiaanse')
    aword = Noun('nl',u'Italiaanse','f')
    FemalePersonFromItalymeaning = Meaning(aword,definition = u'vrouwelijke persoon die uit [[Italië]] komt', label=u'NFemalePersonFromItaly', concisedef=u'vrouwelijke persoon uit Italië',examples=['Die vrouw is een Italiaanse'])

#    {{-rel-}}
#    * [[Italiaan]]
    detrans = Noun('de',u'Italienerin','f')
    entrans = Noun('en',u'Italian')
    frtrans = Noun('fr',u'Italienne','f')
    ittrans = Noun('it',u'italiana','f')

    FemalePersonFromItalymeaning.addTranslations(detrans, entrans, frtrans, ittrans)

    Italiaanseentry = Entry('nl')
    Italiaanseentry.addMeaning(FemalePersonFromItalymeaning)

    apage.addEntry(Italiaanseentry)


    aword = Adjective('nl',u'Italiaanse')
    asynonym = Adjective('nl',u'Italiaans')
    FromItalymeaning = Meaning(aword, definition = u'uit Italië afkomstig', synonyms=[asynonym], label=u'AdjFromItaly', concisedef=u'uit/van Italië',examples=['De Italiaanse mode'])
    RelatedToItalianLanguagemeaning = Meaning(aword, definition = u'gerelateerd aan de Italiaanse taal', synonyms=[asynonym], label=u'AdjRelatedToItalianLanguage', concisedef=u'm.b.t. de Italiaanse taal',examples=['De Italiaanse werkwoorden','De Italiaanse vervoeging'])

    detrans = Adjective('de',u'italienisches','n')
    detrans2 = Adjective('de',u'italienischer','m')
    detrans3 = Adjective('de',u'italienische','f')
    entrans = Adjective('en',u'Italian')
    frtrans = Adjective('fr',u'italien','m')
    frtrans2 = Adjective('fr',u'italienne','f')
    ittrans = Adjective('it',u'italiano','m')
    ittrans2 = Adjective('it',u'italiana','f')

    FromItalymeaning.addTranslations(detrans, detrans2, detrans3, entrans)
    FromItalymeaning.addTranslations(frtrans2, frtrans, ittrans, ittrans2)

    RelatedToItalianLanguagemeaning.addTranslations(detrans, detrans2, detrans3, entrans)
    RelatedToItalianLanguagemeaning.addTranslations(frtrans2, frtrans, ittrans, ittrans2)

    Italiaanseentry.addMeaning(FromItalymeaning)
    Italiaanseentry.addMeaning(RelatedToItalianLanguagemeaning)

    apage.addEntry(Italiaanseentry)

    print
    u=apage.wikiWrap()
    print repr(u)
    raw_input()

    apage.setWikilang('en')
    print repr(apage.wikiWrap())
    raw_input()


#    {{-nl-}}
#    {{-noun-}}
#    '''Italiaanse''' {{f}}; vrouwelijke persoon die uit [[Italië]] komt

#    {{-rel-}}
#    * [[Italiaan]]

#    {{-trans-}}
#    *{{de}}: [[Italienierin]] {{f}}
#    *{{en}}: [[Italian]]
#    *{{fr}}: [[Italienne]] {{f}}
#    *{{it}}: [[italiana]] {{f}}

#    {{-adj-}}
#    #'''Italiaanse'''; uit Italië afkomstig
#    #'''Italiaanse'''; gerelateerd aan de Italiaanse taal

#    {{-syn-}}
#    * [[Italiaans]]

def run():
    ea = EditArticle(['-p', 'Andorra', '-e', 'bluefish'])
    ea.initialise_data()
    try:
        ofn, old = ea.fetchpage()

        parseWikiPage(ofn)
        new = ea.edit(ofn)
    except wikipedia.LockedPage:
        sys.exit("You do not have permission to edit %s" % self.pagelink.sectionFreeTitle())
    if old != new:
        new = ea.repair(new)
        ea.showdiff(old, new)
        comment = ea.getcomment()
        try:
            ea.pagelink.put(new, comment=comment, minorEdit=False, watchArticle=ea.options.watch, anon=ea.options.anonymous)
        except wikipedia.EditConflict:
            ea.handle_edit_conflict()
    else:
        wikipedia.output(u"Nothing changed")

# this is setup
invertedlangnames=invertlangnames()
createPOSlookupDict()
createOtherHeaderslookupDict()

if __name__ == '__main__':

    temp()


    ofn = 'wiktionaryentry.txt'
    content = open(ofn).readlines()

    apage = WiktionaryPage(wikilang,pagetopic)
    apage.parseWikiPage(content)
