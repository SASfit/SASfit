#coding: utf-8
"""
Check pages on the English Wikipedia whether they are in the form
Something, State, and if so, create a redirect from Something, ST.

Arguments understood by various bots:
-lang: Choose your language - however, this bot probably is only
       to be used on en:
-putthrottle: Specify minimum number of seconds to wait between
              edits

Specific arguments:
-start:xxx Specify the place in the alphabet to start searching
-force: Don't ask whether to create pages, just create them.

"""
#
# (C) Andre Engels, 2004
# (C) Pywikipedia bot team, 2004-2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: us-states.py 8631 2010-10-09 21:01:00Z xqt $'
#

import re, sys
import wikipedia as pywikibot

def main():
    start = '0'
    force = False
    msg = {'en':'Creating state abbreviation redirect',
           'ar':u'إنشاء تحويلة اختصار الولاية',
           'fa':u'ایجاد تغییرمسیر برای نام اختصاری ایالت',
           'he':u'יוצר הפניה מראשי התיבות של המדינה',
           }

    abbrev = {
        'Alabama': 'AL',
        'Alaska': 'AK',
        'Arizona': 'AZ',
        'Arkansas': 'AR',
        'California': 'CA',
        'Colorado': 'CO',
        'Delaware': 'DE',
        'Florida': 'FL',
        'Georgia': 'GA',
        'Hawaii': 'HI',
        'Idaho': 'ID',
        'Illinois': 'IL',
        'Indiana': 'IN',
        'Iowa': 'IA',
        'Kansas': 'KS',
        'Kentucky': 'KY',
        'Louisiana': 'LA',
        'Maine': 'ME',
        'Maryland': 'MD',
        'Massachusetts': 'MA',
        'Michigan': 'MI',
        'Minnesota': 'MN',
        'Mississippi': 'MS',
        'Missouri': 'MO',
        'Montana': 'MT',
        'North Carolina': 'NC',
        'North Dakota': 'ND',
        'Nebraska': 'NE',
        'Nevada': 'NV',
        'New Hampshire': 'NH',
        'New Jersey': 'NJ',
        'New Mexico': 'NM',
        'New York': 'NY',
        'Ohio': 'OH',
        'Oklahoma': 'OK',
        'Oregon': 'OR',
        'Pennsylvania': 'PA',
        'Rhode Island': 'RI',
        'South Carolina': 'SC',
        'South Dakota': 'SD',
        'Tennessee': 'TN',
        'Texas': 'TX',
        'Utah': 'UT',
        'Vermont': 'VT',
        'Virginia': 'VA',
        'Washington': 'WA',
        'West Virginia': 'WV',
        'Wisconsin': 'WI',
        'Wyoming': 'WY'
    }

    for arg in pywikibot.handleArgs():
        if arg.startswith('-start:'):
            start = arg[7:]
        elif arg == '-force':
            force = True
        else:
            pywikibot.output(
                u'Warning: argument "%s" not understood; ignoring.' % arg)

    mysite = pywikibot.getSite()
    for p in mysite.allpages(start = start):
        for sn in abbrev:
            R=re.compile('[^[]]*' + '\%2C_' + sn)
            for res in R.findall(p.title()):
                pl=pywikibot.Page(mysite, p.title().replace(sn,abbrev[sn]))
                # A bit hacking here - the real work is done in the
                # 'except pywikibot.NoPage' part rather than the 'try'.
                try:
                    goal = pl.getRedirectTarget().title()
                    if pywikibot.Page(mysite, goal):
                        pywikibot.output(
                            u"Not creating %s - redirect already exists."
                            % goal)
                    else:
                        pywikibot.output(
                            u"WARNING!!! %s already exists but redirects elsewhere!"
                            % goal)
                except pywikibot.IsNotRedirectPage:
                    pywikibot.output(
                        u"WARNING!!! Page %s already exists and is not a redirect. Please check page!"
                        % goal)
                except pywikibot.NoPage:
                    change=''
                    if p.isRedirectPage():
                        p2 = p.getRedirectTarget()
                        wikipeda.ouput(
                            u'Note: goal page is redirect. Creating redirect to "%s" to avoid double redirect.'
                            % p2.title().replace("%2C",",").replace("_"," "))
                    else:
                        p2 = p
                    if force:
                        change='y'
                    else:
                        while not change in ['y','n']:
                            pywikibot.output(
                                u"Create redirect %s" %
                                pl.title().replace("%2C",",").replace("_"," "))
                            change = raw_input("(y/n)? ")
                    if change=='y':
                        text = '#REDIRECT [['+p2.title().replace("%2C",",").replace("_"," ")+']]'
                        pl.put(text, comment=pywikibot.translate(mysite, msg),
                               minorEdit = '0')

try:
    main()
finally:
    pywikibot.stopme()
