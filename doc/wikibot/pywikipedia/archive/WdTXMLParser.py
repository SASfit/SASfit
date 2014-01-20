# -*- coding: iso-8859-1  -*-
"""
(C) 2003 Thomas R. Koll, <tomk32@tomk32.de>
 Distributed under the terms of the MIT license.
"""

__version__='$Id: WdTXMLParser.py,v 1.3 2005/12/21 17:51:26 wikipedian Exp $'

DEBUG = 0
import re
from xml.sax.handler import ContentHandler

class WdTXMLParser(ContentHandler):

        def __init__(self):
                self.rTitle = re.compile ('(.*): (.*)')
                self.rLink  = re.compile ('.*[\r\n]*(http://.*)')
                self.rCount = re.compile ('.*: (\d*)')
                self.inItem = 0
                self.inITitle = 0
                self.inILink = 0
                self.inIDescription = 0
                self.tmp = {}
                self.result = {}

        def startDocument(self):
                self.result = {}
                self.tmp = {}
        def endDocument(self):
                return self.result

        def startElement(self, name, attrs):
                if name == 'item':
                        self.inItem = 1
                if self.inItem == 1:
                        if name == 'title':
                                self.inTitle = 1
                        if name == 'link':
                                self.inLink = 1
                        if name == 'description':
                                self.inDescription = 1

        def characters(self, characters):
                if self.inItem:
                        if self.inTitle:
                                self.tmp['title'] = self.rTitle.match(characters).group(2)
                        if self.inLink:
                                self.tmp['link'] = self.rLink.match(characters).group(1)
                        if self.inDescription:
                                self.tmp['count'] = self.rCount.match(characters).group(1)
                                
        def endElement(self, name):
                if name == 'item':
                        self.inItem = 0
                        self.result[self.tmp['title']] = {
                                'link' : self.tmp['link'],
                                'count' : self.tmp['count']
                                }
                        self.tmp = {}
                if name == 'title':
                        self.inTitle = 0
                if name == 'link':
                        self.inLink = 0
                if name == 'description':
                        self.inDescription = 0
                
"""
if self.date and self.link and self.count:
                        self.results[self.title] = {
                                'date' : self.date,
                                'link' : self.link,
                                'count' : self.count
                                }
"""
