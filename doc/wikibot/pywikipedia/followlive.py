#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
Script to follow new articles on a wikipedia and flag them
with a template or eventually blank them.

There must be A LOT of bugs ! Use with caution and verify what
it is doing !
"""

__version__='$Id: followlive.py 8524 2010-09-11 16:11:14Z xqt $'

import sys, datetime, time, traceback
import wikipedia as pywikibot
import editarticle

__metaclass__ = type

# The question asked
question = u"""(multiple numbers delimited with ',')

b) blank page
e) edit page
d) delete page (need sysop right)

q) quit cleaningbot
Enter) OK
What is it? """

# templates that can be used followed by the message used as comment
# templates contains list of languages code
#   languages code contains list of templates to be used
#       templates contains a message and its position
templates = {
    'ar': {
        '{{شطب}}' :{
            'msg' : 'وسم مساعد بالبوت: هذا المقال ينبغي حذفه',
            'pos': 'top'},

        '{{تنظيف}}' :{
            'msg' : 'وسم مساعد بالبوت: هذا المقال يحتاج للتنظيف',
            'pos': 'top'},

        '{{بذرة}}':{
            'msg' : 'وسم مساعد بالبوت: هذا المقال بذرة',
            'pos': 'bottom'},

        '{{ويكي}}' :{
            'msg' : 'وسم مساعد بالبوت: هذا المقال يحتاج إلى التنسيق بضيغة الويكي حسب [[ويكيبيديا:دليل الأسلوب|دليل الأسلوب]]',
            'pos' : 'top'},
    },
    'en': {
        '{{db-reason}}' :{
            'msg' : 'Robot-assisted tagging: this article should be deleted',
            'pos': 'top'},

        '{{cleanup}}' :{
            'msg' : 'Robot-assisted tagging: this article need cleanup',
            'pos': 'top'},

        '{{stub}}':{
            'msg' : 'Robot-assisted tagging: this article is a stub',
            'pos': 'bottom'},

        '{{uncategorized}}' :{
            'msg' : 'Robot-assisted tagging: This article needs to be [[Wikipedia:Categorization|categorized]]',
            'pos' : 'top'},

        '{{notability}}':{
            'msg' : 'Robot-assisted tagging: the [[Wikipedia:Notability|notability]] of this article is unclear.',
            'pos': 'top'},

        '{{not verified}}':{
            'msg' : 'Robot-assisted tagging: this article needs to be checked for factuality.',
            'pos': 'top'},

        '{{copyedit}}':{
            'msg' : 'Robot-assisted tagging: the writing of this article needs to be [[Wikipedia:How to copy-edit|copyedited]] and improved.',
            'pos' : 'top'},

        '{{unreferenced}}':{
            'msg' : 'Robot-assisted tagging: this article needs [[Wikipedia:Citing sources|references]] so it can be verified.',
            'pos': 'bottom'},

        '{{wikify}}' :{
            'msg' : 'Robot-assisted tagging: this article needs to be wikified per the [[Wikipedia:Manual of Style|Manual of Style]]',
            'pos' : 'top'},
    },
    'ia':{
        '{{Eliminar}}' :{
            'msg' : 'Assistite per robot: iste articulo debe esser eliminate',
            'pos': 'top'},

        '{{Revision}}' :{
            'msg' : 'Assistite per robot: iste articulo require revision',
            'pos': 'top'},

        '{{Stub}}' :{
            'msg' : 'Assistite per robot: iste articulo es in stato embryonic',
            'pos': 'bottom'},
    },
    'fr':{
        u'{{suppression}}': {
            'msg' : u'à l\'aide du robot: cet article devrait être supprimé',
            'pos': 'top'},

        u'{{à vérifier}}' : {
            'msg': u'à l\'aide du robot: cet article est à vérifier',
            'pos': 'top'},

        u'{{ébauche}}' : {
            'msg': u'à l\'aide du robot: cet article est une ébauche',
            'pos': 'top'},
    },
    'he':{
        u'{{מחק}}': {
            'msg' : u'יש למחוק ערך זה',
            'pos': 'top'
        },
        u'{{לשכתב}}': {
            'msg' : u'ערך זה דורש שכתוב',
            'pos': 'top'
        },
        u'{{קצרמר}}': {
            'msg' : u'ערך זה הוא קצרמר',
            'pos': 'bottom'
        },
        u'{{הבהרת חשיבות}}':{
            'msg' : u'חשיבותו של ערך זה אינה ברורה.',
            'pos': 'top'
        },
        u'{{עריכה}}': {
            'msg' : u'ערך זה דורש עריכה',
            'pos': 'top'},
    },
    'ia':{
        u'{{Eliminar}}': {
            'msg' : u'Iste articulo debe esser eliminate',
            'pos': 'top'
        },
        u'{{Revision}}': {
            'msg' : u'Iste articulo require revision',
            'pos': 'top'
        },
        u'{{Stub}}': {
            'msg' : u'Iste articulo es in stato embryonic',
            'pos': 'bottom'
        },
    },
   'nl':{
        u'{{weg}}': {
            'msg' : '{weg}',
            'pos' : 'top'
        },
        u'{{nuweg}}': {
            'msg' : '{nuweg}',
            'pos' : 'top'
        },
        u'{{wiu}}': {
            'msg' : '{wiu}',
            'pos' : 'top'
        },
        u'{{beg}}': {
            'msg' : '{beg}',
            'pos' : 'bottom'
        },
        u'{{wikify}}': {
            'msg' : '{wikify}',
            'pos' : 'top'
        },
        u'{{wb}}': {
            'msg' : '{wb}',
            'pos' : 'top'
        },
    },
    'pl':{
        u'{{ek}}': {
            'msg' : u'[[Kategoria:Ekspresowe kasowanko|ek]]',
            'pos':'top'
        },
        u'{{dopracować}}' : {
            'msg' : u'Dopracować',
            'pos':'top'
        },
        u'{{linki}}'      : {
            'msg' : u'Linki wewnętrzne do dodania',
            'pos':'top'
        },
        u'{{źródła}}'     : {
            'msg' : u'W artykule brakuje źródeł',
            'pos':'top'
        },
        u'{{stub}}'       : {
            'msg' : u'stub (zalążek)',
            'pos':'bottom'
        },
    },
    'pt': {
        u'{{wikificar}}' : {
            'msg': 'Assistida por bot: {{wikificar}}',
            'pos':'top'},

        u'{{reciclar}}'  : {
            'msg': 'Assistida por bot: {{reciclar}}',
            'pos':'top'},

        u'{{lixo|~~~~}}' : {
            'msg': 'Assistida por bot: {{lixo}}',
            'pos':'top'},

        u'{{revisão}}' : {
            'msg': 'Assistida por bot: {{revisão}}',
            'pos':'top'},

        u'{{impróprio}}' : {
            'msg': 'Assistida por bot: {{impróprio}}',
            'pos':'top'},

        u'{{apagar vaidade}}' : {
            'msg': 'Assistida por bot: {{apagar vaidade}}',
            'pos':'top'},
    },
    'sv': {
        u'{{radera}}' :{
            'msg' : u'Robotkoll: Artikeln bör raderas',
            'pos': 'top'},

        u'{{städa}}' :{
            'msg' : u'Robotkoll: Artikeln bör städas',
            'pos': 'top'},

        u'{{stub}}':{
            'msg' : u'Robotkoll: Artikeln är en stubbe',
            'pos': 'bottom'},

        u'{{subst:relevanskontroll}}':{
            'msg' : u'Robotkoll: Artikeln bör kollas mot [[WP:REL|Wikipedias relevanskriterier]].',
            'pos': 'top'},

        u'{{verifieras}}':{
            'msg' : u'Robotkoll: Artikeln bör verifieras',
            'pos': 'top'},

        u'{{språkvård}}':{
            'msg' : u'Robotkoll: Artikeln bör språkvårdas',
            'pos' : 'top'},

        u'{{Källor}}':{
            'msg' : u'Robotkoll: Artikeln behöver källor',
            'pos': 'bottom'},

        u'{{wikify}}' :{
            'msg' : u'Robotkoll: Artikeln behöver wikifieras',
            'pos' : 'top'},
    },
    'zh': {
        u'{{Delete}}' :{
            'msg' : u'機器人掛上模板: 本文應被刪除。',
            'pos': 'top'},

        u'{{subst:Cleanup/auto}}' :{
            'msg' : u'機器人掛上模板: 本文需清理',
            'pos': 'top'},

        u'{{subst:Uncategorized/auto}}' :{
            'msg' : u'機器人掛上模板:  本頁需要適當的頁面分類',
            'pos' : u'bottom'},

        u'{{subst:Notability/auto}}':{
            'msg' : u'機器人掛上模板:  本條目主題未突顯其知名度或顯著性',
            'pos': 'top'},

        u'{{subst:refimprove/auto}}':{
            'msg' : u'機器人掛上模板:  本條目参考文献不足',
            'pos': 'top'},

        u'{{copyedit}}':{
            'msg' : u'機器人掛上模板:  本條目或段落需要校對',
            'pos' : 'top'},

        u'{{subst:Unreferenced/auto}}':{
            'msg' : u'機器人掛上模板:  本條目沒有列出任何參考或來源',
            'pos': 'top'},

        u'{{subst:wikify/auto}}' :{
            'msg' : u'機器人掛上模板:  本條目需要維基化',
            'pos' : 'top'},

        u'{{subst:Notchinese/auto}}':{
            'msg' : u'機器人掛上模板: 本条目没有翻译',
            'pos' : 'top'},

        u'{{subst:Substub/auto}}' :{
            'msg' : u'機器人掛上模板:  小小作品',
            'pos' : 'top'},

        u'{{stub}}':{
            'msg' : u'機器人掛上模板: 本文是小作品',
            'pos': 'bottom'},
        u'{{notchinesetitle}}':{
            'msg' : u'機器人掛上模板: 本条目名称需要翻译成中文',
            'pos': 'top'},
        u'{{subst:Translating/auto}}':{
            'msg' : u'機器人掛上模板: 本条目没有翻译完成',
            'pos': 'top'},
        u'{{fansite}}':{
            'msg' : u'機器人掛上模板: 本条目內容類似愛好者網站',
            'pos': 'top'},

    },
}

# Message used when blanking an article
blanking = {
    'ar': u'تم الإفراغ، المحتوى كان "%s"',
    'en': u'blanked, content was "%s"',
    'fr': u'blanchit, le contenu était "%s"',
    'he': u'רוקן, תוכן היה "%s"',
    'pl': u'wyczyszczony - zawartością było "%s"',
    'pt': u'em branco - conteúdo é "%s"',
    'sv': u'Robot tar bort innehåll på grund av "%s"',
    'zh': u'清空，之前的內容是 "%s"',
}

# do nothing if this is in it
done = {
    'ar':(u'{{شطب}}', u'{{حذف}}', u'{{خرق}}'),
    'en':('{{VfD}}', '{{AfD}}', '{{AfD1}}', '{{cleanup}}', '{{nonsense}}',
          '{{deletedpage}}', '{{db-reason}}', '{{notability}}',
          '{{not verified}}', '{{unreferenced}}', '{{db-empty}}',
          '{{db-nocontext}}', '{{db-foreign}}', '{{db-notenglish}}',
          '{{db-nocontent}}', '{{db-blankcsd}}', '{{db-transwiki}}',
          '{{db-attack}}', '{{db-band}}', '{{db-club}}', '{{db-bio}}',
          '{{db-bio-notenglish}}', '{{db-inc}}', '{{db-bio-photo}}',
          '{{db-catempty}}', '{{db-c2}}', '{{db-catfd}}', '{{badname}}',
          '{{db-pagemove}}', '{{db-nonsense}}', '{{db-spam}}', '{{db-copyvio}}',
          '{{db-test}}', '{{db-vandalism}}', '{{db-repost}}', '{{db-banned}}',
          '{{db-histmerge}}', '{{db-move}}', '{{db-g6}}', '{{db-afd}}',
          '{{db-disambig}}', '{{db-authora}}', '{{db-author}}',
          '{{db-blanked}}', '{{csd:g7}}', '{{db-talk}}', '{{db-botnomain}}',
          '{{db-redundantimage}}', '{{db-noimage}}', '{{db-noncom}}',
          '{{db-ccnoncom}}', '{{db-unksource}}', '{{db-norat}}',
          '{{db-badfairuse}}', '{{duplicate}}', '{{db-meta}}',
          '{{db-emptyportal}}', '{{db-redirnone}}', '{{db-rediruser}}',
          '{{db-redirtypo}}', '{{csd-c3}}', '{{cc-by-nc-sa}}', '{{cc-nd-nc}}',
          '{{cc-nc}}', '{{cc-by-nc-2.0}}', '{{cc-by-nc-sa-2.0}}',
          '{{cc-by-nd-nc-2.0}}', '{{cc-by-2.0-nc-nd}}', '{{cc-by-nc-nd-2.0}}',
          '{{db-contact}}', '{{db-i2}}', '{{db-i1}}', '{{communityuseonly}}',
          '{{db-disparage}}', '{{db-web}}', '{{db-userreq}}', '{{db-nouser}}',
          '{{db-u3}}', '{{db-unfree}}'),
    'fr':(u'{{suppression}}', u'{{à vérifier}}', u'{{ébauche}}'),
    'ia':(u'{{Eliminar}}', u'{{Revision}}', u'{{Stub}}'),
    'he':(u'{{מחק}}', u'{{פירושונים}}', u'{{הצבעת מחיקה}}'),
    'nl':('{{nuweg}}', '{{weg}}', '{{wb}}', '{{wiu}}', '{{nocat}}'),
    'pl':('{{ek}}', u'{{dopracować}}', '{{linki}}', u'{{źródła}}', u'{{stub}}'),
    'pt':('{{wikificar}}', '{{reciclar}}', '{{lixo}}', u'{{revisão}}',
          u'{{impróprio}}', u'{{apagar vaidade}}'),
    'sv':(u'{{radera', u'{{Radera', u'{{städa}}', u'{{stub}}',
          u'{{verifieras}}', u'{{språkvård}}', u'{{Källor', u'{{källor',
          u'{{wikify}}', u'{{Ickewiki}}', u'{{ickewiki}}', u'{{Wikify}}'),
    'zh':(u'{{VfD}}', u'{{AfD}}', u'{{unreferenced}}', u'{{db-reason}}',
          u'{{cleanup}}', u'{{stub}}', u'{{uncategorized}}', u'{{notability}}',
          u'{{copyedit}}', u'{{unreferenced}}', u'{{wikify}}',
          u'{{Translating}}',u'{{copyvio}}',u'{{Notchinese}}'),
    }

# TODO: merge 'done' with 'templates' above

class PageHandler:
    # Initialization stuff
    def __init__(self, page, date, length, loggedIn, user, comment):
        self.page = page
        self.date = date
        self.length = length
        self.loggedIn = loggedIn
        self.user = user
        self.comment = comment

    # Display informations about an article
    def showpageinfo(self):
        pywikibot.output(u'[[%s]] %s ' % (self.page.title(), self.date))
        print 'Length: %i bytes' % self.length
        pywikibot.output(u'User  : %s' % self.user)

    def couldbebad(self):
        return self.length < 250 or not self.loggedIn

    def handlebadpage(self):
        try:
            self.content = self.page.get()
        except pywikibot.IsRedirectPage:
            pywikibot.output(u'Already redirected, skipping.')
            return
        except pywikibot.NoPage:
            pywikibot.output(u'Already deleted')
            return

        for d in pywikibot.translate(pywikibot.getSite(), done):
            if d in self.content:
                pywikibot.output(
                    u'Found: "%s" in content, nothing necessary' % d)
                return
        print "---- Start content ----------------"
        pywikibot.output(u"%s" % self.content)
        print "---- End of content ---------------"

        # Loop other user answer
        answered = False
        while not answered:
            answer = pywikibot.input(question)

            if answer == 'q':
                sys.exit("Exiting")
            if answer == 'd':
                pywikibot.output(u'Trying to delete page [[%s]].'
                                 % self.page.title())
                self.page.delete()
                return
            if answer == 'e':
                oldText = self.page.get()
                text = oldText
                editor = editarticle.TextEditor()
                text = editor.edit(self.page.get())
                if oldText != text:
                    pywikibot.showDiff(oldText, text)
                    msg = pywikibot.input(u'Summary message:')
                    self.page.put(text, msg)
                return
            if answer == 'b':
                pywikibot.output(u'Blanking page [[%s]].' % self.page.title())
                try:
                    self.page.put('',
                                  comment=pywikibot.translate(
                                      pywikibot.getSite(), blanking)
                                  % self.content)
                except EditConflict:
                    print "An edit conflict occured ! Automatically retrying"
                    handlebadpage(self)
                return
            if answer == '':
                print 'Page correct ! Proceeding with next pages.'
                return
            # Check user input:
            if answer[0] == 'u':
                # Answer entered as an utf8 string
                try:
                    choices=answer[1:].split(',')
                except ValueError:
                    # User entered wrong value
                    pywikibot.output(u'ERROR: "%s" is not valid' % answer)
                    continue
            else:
                try:
                    choices=answer.split(',')
                except ValueError:
                    # User entered wrong value
                    pywikibot.output(u'ERROR: "%s" is not valid' % answer)
                    continue
            #test input
            for choice in choices:
                try:
                    x=int(choice)
                except ValueError:
                    break
                else:
                    answered=x in range(1,len(questionlist)+1)
            if not answered:
                pywikibot.output(u'ERROR: "%s" is not valid' % answer)
                continue
        summary = u''
        for choice in choices:
            answer = int(choice)
            # grab the template parameters
            tpl = pywikibot.translate(pywikibot.getSite(), templates) \
                  [questionlist[answer]]
            if tpl['pos'] == 'top':
                pywikibot.output(u'prepending %s...' % questionlist[answer])
                self.content = questionlist[answer] + '\n' + self.content
            elif tpl['pos'] == 'bottom':
                pywikibot.output(u'appending %s...' % questionlist[answer])
                self.content += '\n' + questionlist[answer]
            else:
                pywikibot.output(
                    u'ERROR: "pos" should be "top" or "bottom" for template %s. Contact a developer.'
                    % questionlist[answer])
                sys.exit("Exiting")
            summary += tpl['msg']+' '
            pywikibot.output(u'Probably added %s' % questionlist[answer])
#        pywikibot.output(newcontent) bug #2986247
        self.page.put(self.content, comment = summary)
        pywikibot.output(u'with comment %s\n' % summary)

    def run(self):
        self.showpageinfo()
        if self.couldbebad():
            print 'Integrity of page doubtful...'
            try:
                self.handlebadpage()
            except pywikibot.NoPage:
                print 'seems already gone'
        print '----- Current time:', datetime.datetime.now()


class CleaningBot:
    def __init__(self, site=None):
        if site is None:
            site = pywikibot.getSite()
        self.site = site

    def run(self):
        for (page, date, length, loggedIn, username, comment) in \
                self.site.newpages(100, repeat=True):
            handler = PageHandler(page, date, length, loggedIn, username,
                                  comment)
            handler.run()

# Generate the question text
i = 0
questions = '\n'
questionlist = {}
for t in pywikibot.translate(pywikibot.getSite(), templates):
    i+=1
    questions += ( u'%s) %s\n' % (i,t) )
    questionlist[i] = t
question = questions + question

# MAIN
if __name__ == "__main__":
    try:
        for arg in pywikibot.handleArgs():
            pywikibot.output(
                u'Warning: argument "%s" not understood; ignoring.' % arg)
        bot = CleaningBot()
        bot.run()
    except:
        pywikibot.stopme()
        raise
