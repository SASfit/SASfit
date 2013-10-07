# -*- coding: utf-8 -*-
"""
Script to copy a table from one Wikipedia to another one, translating it
on-the-fly. 

Syntax:
  copy_table.py -type:abcd -from:xy Article_Name

Command line options:

-from:xy       Copy the table from the Wikipedia article in language xy
               Article must have interwiki link to xy

-debug         Show debug info, and don't send the results to the server
              
-type:abcd     Translates the table, using translations given below.
               When the -type argument is not used, the bot will simply
               copy the table as-is.

-file:XYZ      Reads article names from a file. XYZ is the name of the 
               file from which the list is taken. If XYZ is not given, the
               user is asked for a filename.
               Page titles should be saved one per line, without [[brackets]].
               The -pos parameter won't work if -file is used.                              

-image         Copy all images within the found table to the target Wikipedia.
               Make sure the bot is logged in before trying to upload images.

Article_Name:  Name of the article where a table should be inserted

"""
#
# (C) Daniel Herding, 2004
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: copy_table.py,v 1.31 2005/12/21 17:51:26 wikipedian Exp $'
#
import wikipedia, translator, lib_images
import re, sys, string

# Summary message
msg={
    "ar":u"روبوت: نسخ الجدول من ",
    "en":u"robot: copying table from ",
    "de":u"Bot: Kopiere Tabelle von ",
    "he":u"רובוט: מעתיק טבלה מתוך ",
    "pt":u"Bot: Copiando tabela de ",
    }

# Prints text on the screen only if in -debug mode.
# Argument text should be raw unicode.
def print_debug(text):
    if debug:
        wikipedia.output(text)
    

# this is a modified version of wikipedia.imagelinks(), it only looks in text, not in the whole page.
def imagelinks(site, text):
    image_ns = site.image_namespace()
    # regular expression which matches e.g. "Image" as well as "image" (for en:)
    im = '[' + image_ns[0].upper() + image_ns[0].lower() + ']' + image_ns[1:]
    w1=r'('+im+':[^\]\|]*)'
    w2=r'([^\]]*)'
    Rlink = re.compile(r'\[\['+w1+r'(\|'+w2+r')?\]\]')
    result = []
    for l in Rlink.findall(text):
        result.append(l[0])
    return result

# opens on a page, checks for an interwiki link, transfers and translates the first
# table, copies all images in that table.
def treat(to_pl, fromsite):
    try:
        to_text = to_pl.get()
        interwikis = to_pl.interwiki()
    except wikipedia.IsRedirectPage:
        print "Can't work on redirect page."
        return
    except wikipedia.NoPage:
        print "Page not found."
        return
    from_pl = None
    for interwiki in interwikis:
        if interwiki.site() == fromsite:
            from_pl = interwiki
    if from_pl is None:
        print "Interwiki link to %s not found." % repr(fromsite)
        return
    from_text = from_pl.get()
    wikipedia.setAction(wikipedia.translate(mysite.lang, msg) + from_pl.aslink())
    # search start of table
    table = get_table(from_text)
    if not table:
        wikipedia.output(u"No table found in %s" % (from_pl.aslink()))
        return

    print_debug(u"Copying images")
    if copy_images:
        # extract image links from original table
        images=imagelinks(fromsite, table)
        for image in images:
            # Copy the image to the current wikipedia, copy the image description page as well.
            # Prompt the user so that he can translate the filename.
            new_filename = lib_images.transfer_image(wikipedia.Page(fromsite, image), debug)
            # if the upload succeeded
            if new_filename:
                old_image_tag = wikipedia.Page(fromsite, image).title()
                new_image_tag = wikipedia.Page(mysite, mysite.image_namespace() + ":" + new_filename).title()
                print_debug(u"Replacing " + old_image_tag + " with " + new_image_tag)
                # We want to replace "Image:My pic.jpg" as well as "image:my_pic.jpg", so we need a regular expression.
                old_image_tag = old_image_tag.replace(" ", "[ \_]")
                old_image_tag = "[" + old_image_tag[0].upper() + old_image_tag[0].lower() + "]" + old_image_tag[1:]
                #todo: regex for first letter of filename, i.e. first letter after the colon
                rOld_image_tag = re.compile(old_image_tag)
                table = re.sub(old_image_tag, new_image_tag, table)


    translated_table = translator.translate(table, type, fromsite.lang, debug, mysite.lang)
    if not translated_table:
        print "Could not translate table."
        return

    print_debug(u"\n" + translated_table)
    # add table to top of the article, seperated by a blank lines
    to_text = translated_table + "\n\n" + to_text
    if not debug:
        # save changes on Wikipedia
        to_pl.put(to_text, minorEdit='0')

            
        

# Regular expression that will match both <table and {|
startR = re.compile(r"<table|\{\|")
# Regular expression that will match both </table> and |}
endR = re.compile(r"</table>|\|\}")

# Finds the first table inside a text, including cascaded inner tables.
def get_table(text):
    pos = 0
    # find first start tag
    first_start_tag = re.search(startR, text)
    if not first_start_tag:
        return
    else:
        print_debug(u"First start tag found at " + str(first_start_tag.start()))
        pos = first_start_tag.end()
        # number of start tags minus numer of end tags
        table_level = 1
        remaining_text = text
    # until an end tag has been found for each start tag:
    while table_level != 0:
        # continue search after the last found tag
        remaining_text = text[pos:]
        next_start_tag = re.search(startR, remaining_text, pos)
        next_end_tag = re.search(endR, remaining_text, pos)
        if not next_end_tag:
            print_debug(u"Error: missing end tag")
            pass
        # if another cascaded table is opened before the current one is closed    
        elif next_start_tag and next_start_tag.start() < next_end_tag.start():
            print_debug(u"Next start tag found at " + str(pos + next_start_tag.start()))
            pos += next_start_tag.end()
            table_level += 1
            print_debug(u"Table level is " + str(table_level))
        else:
            print_debug(u"Next end tag found at " + str(pos + next_end_tag.start()))
            pos += next_end_tag.end()
            table_level -= 1
            print_debug(u"Table level is " + str(table_level))
    print_debug(u"Table starts at " + str(first_start_tag.start()) + " and ends at " + str(pos) +"\n")
    print_debug(text[first_start_tag.start():pos])
    return text[first_start_tag.start():pos]

if __name__=="__main__":
    try:
        # if the -file argument is used, page titles are dumped in this array.
        # otherwise it will only contain one page.
        page_list = []
        # if -file is not used, this temporary array is used to read the page title.
        page_title = []
        from_lang = ""
        type = ""
        debug = False
        copy_images = False

        # read command line parameters
        for arg in sys.argv[1:]:
            arg = wikipedia.argHandler(arg, 'copy_table')
            if arg:
                if arg.startswith("-from"):
                    from_lang = arg[6:]
                elif arg.startswith("-type:"):
                    type = arg[6:]
                elif arg == "-debug":
                    debug = True
                elif arg == "-image":
                    copy_images = True
                elif arg.startswith('-file'):
                    if len(arg) == 5:
                        file = wikipedia.input(u'Please enter the list\'s filename: ')
                    else:
                        file = arg[6:]
                    # open file and read page titles out of it
                    f=open(file)
                    for line in f.readlines():
                        if line != '\n':           
                            page_list.append(line)
                    f.close()
                else:
                    page_title.append(arg)

        # if the page name is given as a command line argument,
        # connect the title's parts with spaces
        if page_title != []:
            page_title = ' '.join(page_title)
            page_list.append(page_title)

        mysite = wikipedia.getSite()
        fromsite = mysite.getSite(code=from_lang)
    
        for current_page_name in page_list:
            thispl = wikipedia.Page(mysite, current_page_name)
            treat(thispl, fromsite)
    except:
        wikipedia.stopme()
        raise
    wikipedia.stopme()

