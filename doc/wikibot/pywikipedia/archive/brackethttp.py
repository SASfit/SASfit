# -*- coding: utf-8 -*-
'''
Script to correct URLs like
(http://www.example.org) to [http://www.example.org example.org]
to have correct generation of links in Wikipedia
'''

__author__ = '(C) 2003 Thomas R. Koll, <tomk32@tomk32.de>'
__license__ = 'Distributed under the terms of the MIT license.'
__version__='$Id: brackethttp.py,v 1.13 2005/12/21 17:51:26 wikipedian Exp $'

import re, sys
import wikipedia

myComment = {'ar':u'بوت: URL تم إصلاحها',
             'en':u'Bot: URL fixed',
             'fa':u'ربات: URL اصلاح شد',
             'he':u'בוט: תוקנה כתובת URL',
             'pt':u'Bot: URL corrigido',
             'zh':u'機器人: 網址已修復',
             }

if __name__ == "__main__":
    try:
        for arg in sys.argv[1:]:
            if wikipedia.argHandler(arg, 'brackethttp'):
                pass
            else:
                pl = wikipedia.Page(wikipedia.getSite(), arg)
                text = pl.get()
        
                newText = re.sub("(http:\/\/([^ ]*[^\] ]))\)", "[\\1 \\2])", text)

                if newText != text:
                    wikipedia.showDiff(text, newText)
                    status, reason, data = pl.put(newText, wikipedia.translate(wikipedia.mylang,myComment))
                    print status, reason
                else:
                    print "No bad link found"
    except:
        wikipedia.stopme()
        raise
wikipedia.stopme()
