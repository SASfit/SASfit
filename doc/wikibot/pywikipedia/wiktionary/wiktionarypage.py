#!/usr/bin/python
# -*- coding: utf-8  -*-

'''
This module contains code to store Wiktionary content in Python objects.
The objects can output the content again in Wiktionary format by means of the wikiWrap methods

I'm currently working on a parser that can read the textual version in the various Wiktionary formats and store what it finds in the Python objects.

The code is still very much alpha level and the scope of what it can do is still rather limited, only 3 parts of speech, only 2 different Wiktionary output formats, only langnames matrix for about 8 languages. One of the things on the todo list is to harvest the content of this matrix dictionary from the various Wiktionary projects. GerardM put them all on line in templates already.
'''

import entry
import sortonlanguagename
import structs
import header
import copy
import meaning
import term

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
            self.sortedentries.sort(sortonlanguagename.sortonlanguagename(structs.langnames[self.wikilang]))

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
        aheader = ''
        splitcontent=[]
        content=content.split('\n')
        for line in content:
 #           print line
            # Let's get rid of line breaks and extraneous white space
            line=line.replace('\n','').strip()
            # Let's start by looking for general stuff, that provides information which is
            # interesting to store at the page level
            if '{wikipedia}' in line.lower():
                self.addLink('wikipedia')
                continue
            if '[[category:' in line.lower():
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
            # store empty lines literally, this is necessary for the blocks we don't parse
            # and will return literally
            if len(line) <2:
                templist.append(line)
                continue
#        print 'line0:',line[0], 'line-2:',line[-2],'|','stripped line-2',line.rstrip()[-2]
            if line.strip()[0]=='='and line.rstrip()[-2]=='=' or '{{-' in line and '-}}' in line:
                # When a new header is encountered, it is necessary to store the information
                # encountered under the previous header.
                if templist and aheader:
                    tempdictstructure={'text': templist,
                                       'header': aheader,
                                       'context': copy.copy(context),
                                      }
                    templist=[]
                    splitcontent.append(tempdictstructure)
#                print "splitcontent: ",splitcontent,"\n\n"
                aheader=header.Header(line)
#                print "Header parsed:",aheader.level, aheader.header, aheader.type, aheader.contents
                if aheader.type==u'lang':
                    context['lang']=aheader.contents
                if aheader.type==u'pos':
                    if not 'lang' in context:
                        # This entry lacks a language indicator,
                        # so we assume it is the same language as the Wiktionary we're working on
                        context['lang']=self.wikilang
                    context['pos']=aheader.contents

            else:
                # It's not a header line, so we add it to a temporary list
                # containing content lines
                if aheader.contents==u'trans':
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
                                   'header': aheader,
                                   'context': copy.copy(context),
                                  }
                splitcontent.append(tempdictstructure)


        # make sure variables are defined before they are used
        gender = sample = plural = diminutive = label = definition = ''
        number = 1
        diminutive = False
        examples = []
        for contentblock in splitcontent:
            headercontent=contentblock['header'].contents

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
                                for subpart in line.split(' '):
                                    maybegender=part.replace("'",'').replace("}",'').replace("{",'').lower()
                                    if maybegender=='m':
                                        gender='m'
                                    if maybegender=='f':
                                        gender='f'
                                    if maybegender=='n':
                                        gender='n'
                                    if maybegender=='c':
                                        gender='c'
                                    if maybegender[:1]=='p':
                                        number=2
                                    if maybegender[:3]=='dim':
                                        diminutive=True
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
                        if headercontent=='noun':
                            theterm=term.Noun(lang=contentblock['context']['lang'], term=sample, gender=gender, number=number, diminutive=diminutive)
                        if headercontent=='verb':
                            theterm=term.Verb(lang=contentblock['context']['lang'], term=sample)
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
                            ameaning = meaning.Meaning(term=theterm,definition=definition, label=label, examples=examples)

                            # sample
                            # plural and diminutive belong with the Noun object
                            # comparative and superlative belong with the Adjective object
                            # conjugations belong with the Verb object

                            # Reset everything for the next round
                            sample = plural = diminutive = label = definition = ''
                            examples = []

                            if not contentblock['context']['lang'] in self.entries:
                                # If no entry for this language has been foreseen yet
                                # let's create one
                                anentry = entry.Entry(contentblock['context']['lang'])
                                # and add it to our page object
                                self.addEntry(anentry)
                            # Then we can easily add this meaning to it.
                            anentry.addMeaning(ameaning)

                        pos=line.find('<!--')
                        if pos!=-1 and pos < 4:
                            # A html comment at the beginning of the line means this entry already has disambiguation labels, great
                            pos2=line.find('-->')
                            label=line[pos+4:pos2]
                            definition=line[pos2+1:]
#                            print 'label:',label
                        else:
                            definition=line[1:].strip()
#                        print "Definition: ", definition
                    if line[:2] == "#:":
                        # This is an example for the preceding definition
                        example=line[2:]
#                        print "Example:", example
                        examples.add(example)
            # Make sure we store the last definition
            if definition:
                ameaning = meaning.Meaning(term=theterm, definition=definition, label=label, examples=examples)
                if not contentblock['context']['lang'] in self.entries:
                    # If no entry for this language has been foreseen yet
                    # let's create one
                    anentry = entry.Entry(contentblock['context']['lang'])
                    # and add it to our page object
                    self.addEntry(anentry)
                    # Then we can easily add this meaning to it.
                    anentry.addMeaning(ameaning)

            winner = False # This is going to contain the Meaning object which has the Definition which matches the Concisedef of the entry we are working on right now
            if headercontent=='trans' or headercontent=='syn' or headercontent=='ant':
                # On the English Wiktionary we will find concisedefs here to link definitions to the content of these sections, but only if there is more than one definition.
                print "number of meanings:",len(anentry.meanings.keys())
                concisedefclean=''
                for line in contentblock['text']:
                    if line[:3] == "'''":
                        # This seems to be a line containing a concisedef
                        concisedef=line.replace("'''",'').strip()
                        concisedefclean=concisedef.replace("(",'').replace(")",'').replace("'",'').replace(":",'').replace(".",'').lower()
                    if line[:2] == "*(":
                        # This seems to be a line containing a concisedef
                        pos=line.find(')')
                        concisedef=line[2:pos].strip()
                        concisedefclean=concisedef.replace("(",'').replace(")",'').replace("'",'').replace(":",'').replace(".",'').lower()
                        restofline=line[pos+2:].strip()
                    # Now we have this concisedef, it's worthless if it can't
                    # be matched to a definition in order to know to what
                    # meaning the following content belongs to

                    # Let's start by creating a list of meanings for the entry
                    # we're working on
                    if concisedefclean:
                        highest=0
                        winner=anentry.meanings[contentblock['context']['pos']][0]
                        for anothermeaning in anentry.meanings[contentblock['context']['pos']]:
                            score=0
                            for word in concisedefclean.split():
                                definition=anothermeaning.definition.replace("(",'').replace(")",'').replace("'",'').replace(":",'').replace(".",'').replace("#",'').lower()
                                if len(word)>1 and ' '+word+' ' in definition:
                                    score+=1
                                if len(word)>2 and word in definition:
                                    score+=1
                            if score>highest:
                                highest=score
                                winner=anothermeaning
#                        print 'winner:',winner.definition, 'score:',highest
                        winner.setConciseDef(concisedef)
                        if headercontent=='trans':
                            """
                            We have to find a way to read the rest of the lines until the next ConciseDef into a structure, that can be processed later on. In contrast to a list of synonyms where the synonyms are on the rest of the lines, translations are on the following lines.
                            It's also possible that there is no concisedef and that the translation's block simpy starts... or that there are numbers instead of concisedefs.
                            """

                    if headercontent=='syn':
#                        print 'syn',restofline
                        winner.parseSynonyms(restofline)
                    if headercontent=='trans':
#                        print 'trans',restofline
                        winner.parseTranslations(line)

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

if __name__ == '__main__':

    temp()


    ofn = 'wiktionaryentry.txt'
    content = open(ofn).readlines()

    apage = WiktionaryPage(wikilang,pagetopic)
    apage.parseWikiPage(content)
