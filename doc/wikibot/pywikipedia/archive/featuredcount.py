#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
##################################################
This script all function have merged to featured.py. please use:

  featured.py -fromall -count

shizhao 2009-04-18
##################################################


This script only counts how many featured articles all wikipedias have.

usage: featuredcount.py

"""
__version__ = '$Id: featuredcount.py 6336 2009-02-08 04:14:37Z purodha $'

#
# Distributed under the terms of the MIT license.
#

import sys
import wikipedia, catlib
from featured import featured_name

def featuredArticles(site):
    method=featured_name[site.lang][0]
    name=featured_name[site.lang][1]
    args=featured_name[site.lang][2:]
    raw=method(site, name, *args)
    arts=[]
    for p in raw:
        if p.namespace()==0:
            arts.append(p)
        elif p.namespace()==1:
            arts.append(wikipedia.Page(p.site(), p.titleWithoutNamespace()))
    wikipedia.output('\03{lightred}** wikipedia:%s has %i featured articles\03{default}' % (site.lang, len(arts)))

if __name__=="__main__":
    mysite = wikipedia.getSite()
    fromlang = featured_name.keys()
    fromlang.sort()
    try:
        for ll in fromlang:
            fromsite = wikipedia.getSite(ll)
            if fromsite != mysite:
                arts = featuredArticles(fromsite)
        arts_mysite = featuredArticles(mysite)
    finally:
        wikipedia.stopme()
