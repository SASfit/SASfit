#!/usr/bin/python
# -*- coding: utf-8  -*-

from structs import *

class Header(object):
    def __init__(self,line=None,contents=None,header=None,level=None,type=None):
        """ Constructor
            Generally called with one parameter:
            - The line read from a Wiktonary page
              after determining it's probably a header
        """

        # sane defaults for self
        self.contents=None
        self.header=None
        self.level=None
        self.type=None

        # settings for self
        if line!=None: self.parseLine(line)
        if contents!=None: self.contents=contents
        if header!=None: self.header=header
        if level!=None: self.level=level
        if type!=None: self.type=type

    def __eq__(x,y):
        """x.__eq__(y) <==> x==y"""

        return hasattr(x,"__dict__") and hasattr(y,"__dict__") and x.__dict__==y.__dict__

    def __ne__(x,y):
        """x.__ne__(y) <==> x!=y"""

        return (not hasattr(x,"__eq__")) and (not x.__eq__(y))

    def parseLine(self,line):
        self.level=None
        self.type=''        # The type of header, i.e. lang, pos, other
        self.contents=''    # If lang, which lang? If pos, which pos?

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
