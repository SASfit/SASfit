#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
Script to remove EDP images in non-article namespaces. This script is currently
used on the Chinese wikipedia.

Way:
* [[Image:logo.jpg]] --> [[:Image:logo.jpg]]
* [[:Image:logo.jpg]] pass
* Image:logo.jpg in gallery --> [[:Image:logo.jpg]] in gallery end
* logo.jpg(like used in template) --> hide(used <!--logo.jpg-->)
"""
#
# (C) Shizhao, 2008
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: deledpimage.py 9042 2011-03-13 10:14:47Z xqt $'
#

"""
commond:
python deledpimage.py

删除显示在用户页上的合理使用图像

"""
import re, time
import wikipedia as pywikibot
import catlib

site = pywikibot.getSite()

cat = {
    'ar': u'تصنيف:وسوم حقوق نسخ الصور غير الحرة',
    'en': u'Category:Non-free image copyright tags',
    'zh': u'Category:合理使用图像模板',
    }

content = {
    'ar': u'هذه الصورة غير الحرة غير مستخدمة في نطاق المقالات، انظر [[Wikipedia:Non-free content#Policy]]',
    'en': u'This Non-free image NOT used in non-article namespaces, see[[Wikipedia:Non-free content#Policy]]',
    'zh': u'不是使用在条目中的非自由版权图像，根据[[Wikipedia:合理使用]]，不能在非条目名字空间展示：\n',
    }

msg = {
    'ar': u'روبوت: إصلاح استخدام صورة EDP: [[%s]]',
    'en': u'Robot: Fix EDP image use: [[%s]]',
    'zh': u'Bot修正EDP图像用法：[[%s]]',
    }

lcontent=pywikibot.translate(site, content)
category=pywikibot.translate(site, cat)
putmsg=pywikibot.translate(site, msg)

#from non-free copyright tag category get all EDPtemplate
templatecat=catlib.Category(site, category)
templatelist = templatecat.articlesList()

#from References of EDP template get all non-free images
for tempalte in templatelist:
    images = [page for page in tempalte.getReferences() if page.isImage()]

    for image in images :
        imagetitle=image.title()
        imagepage=pywikibot.ImagePage(site,imagetitle)

#from imagepage get all usingPages of non-articles
        pimages=[puseimage for puseimage in imagepage.usingPages() if puseimage.namespace()<>0]
        for pimage in pimages:
            ns=pimage.namespace()
            pimagetitle=pimage.title()
            c = u'\nfond an used the image [[%s]] in [[%s]]: ' \
                % (imagetitle, pimagetitle)
            text=pimage.get()
            try:
                re.search('<!--(.*?)'+imagetitle+'(.*?)-->',text,re.I).group(0)
            except:
                try:
 #                   imagetext=re.search('\[\['+imagetitle+'(.*?)\]\]',text,re.I).group(0)
                    if imagetitle not in text:

# Not [[Image:]]  namespace
                        if imagetitle[6:] in text:
                            imagetext = re.search(imagetitle[6:]+'(.*?)(|)',text,re.I).group(0)
                            text = re.sub(imagetitle[6:]+'(.*?)(|)', '<!--'+lcontent+imagetext+' \n-->', text, re.I)
                            pywikibot.output(c+u'remove!!!\nSleep 10 s......')
                            pimage.put(text, putmsg % imagetitle)
                            time.sleep(10)

#used [[Image:wiki.png]]  image
                    else:
                        if '[['+imagetitle in text:

#Image in userpage, imagepage,and all talkpage , [[Image:wiki.png]] --> [[:Image:wiki.png]]
                                if ns==1 or ns==6 or ns==2 or ns==3 or ns==5 or ns==7 or ns==9 or ns==11 or ns==13 or ns==15 or ns==17 or ns==101:

                                    text = re.sub('\[\['+imagetitle+'(.*?)\]\]', '<!--'+lcontent+'\n-->'+'[['+':'+imagetitle+']]',text, re.I)
                                    pywikibot.output(c+u'FIX!\nSleep 10 s......')
                                    pimage.put(text, putmsg % imagetitle)
                                    time.sleep(10)

#Image in template, categorypage,  remove
                                elif ns==10 or ns==14:
                                    text = re.sub('\[\['+imagetitle+'(.*?)(|)\]\]', '<!--'+lcontent+imagetext+'\n-->',text, re.I)
                                    pywikibot.output(c+u'Remove!!!\nSleep 10 s......')
                                    pimage.put(text, putmsg % imagetitle)
                                    time.sleep(10)
#                elif '[[:'+imagetitle in text:
#                    pywikibot.output(c+u'EDP is OK!')

#Image in <gallery></gallery>
                        else:
#                            try:
#                            imagetext=re.search(imagetitle+'(.*?)\n',text,re.I).group(0)
                            text = re.sub(imagetitle+'(.*?)', '',text, re.I)
                            text=re.sub('</gallery>\n', '</gallery>\n'+'<!--'+lcontent+imagetext+'\n-->\n'+'[[:'+imagetitle+']]\n',text, re.I)
                            pywikibot.output(c+u'FIX <gallery>!\nSleep 10 s......')
                            pimage.put(text, putmsg % imagetitle)
                            time.sleep(10)
                except:
                    print 'Error'
                    pass

