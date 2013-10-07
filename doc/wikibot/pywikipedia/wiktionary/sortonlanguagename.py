#!/usr/bin/python
# -*- coding: utf-8  -*-

# A big thanks to Rob Hooft for the following class:
# It may not seem like much, but it magically allows the translations to be sorted on
# the names of the languages. I would never have thought of doing it like this myself.

class sortonlanguagename:
    '''
    This class sorts translations alphabetically on the name of the language,
    instead of on the iso abbreviation that is used internally.
    '''
    def __init__(self, lang):
        self.lang = lang

    def __call__(self, one, two):
        return cmp(self.lang[one], self.lang[two])
