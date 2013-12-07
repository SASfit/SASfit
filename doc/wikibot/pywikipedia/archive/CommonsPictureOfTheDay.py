# -*- coding: utf-8 -*-
'''
Put "Picture of the day" in your desktop wallpaper from Wikimedia Commons.

For Windows system, do you need:
* Python 2.5
* Pywin32 for Python 2.5
* PIL for Python 2.5

For Linux system, do you need:
* Python and PIL

'''

from wikipedia import Site, Page, ImagePage
from PIL import Image, ImageDraw, ImageFont
import httplib, time, sys, os

if sys.platform == 'win32':
    import ctypes, win32con
    from _winreg import *
else:
    import gconf

def get_commons_image(image):
    headers = {"Accept": "image/jpg",
               "Accept": "image/gif",
               "Accept": "image/png",
               "Accept": "image/svg",
               }
    conn = httplib.HTTPConnection('upload.wikimedia.org')
    conn.request("GET", image, None, headers)
    r = conn.getresponse()
    data = r.read()
    if sys.platform == 'win32':
        arq = open("Picture_of_the_day.bmp","wb") # convert image "on the fly" to Windows Bitmap
    else:
        arq = open("Picture_of_the_day.png","wb")
    arq.write(data)
    arq.close()
    conn.close()

def write_gray(filename, text, outfilename):
    img = Image.open(filename).convert("RGB")
    write = Image.new("RGB", (img.size[0], img.size[1]))
    draw = ImageDraw.ImageDraw(img)
    size = 0
    while True:
        size +=1
        try:
            FONT = "C:\WINDOWS\Fonts\Verdana.ttf"
        except IndexError:
            FONT = "/usr/share/fonts/truetype/ttf-bitstream-vera/Verdana.ttf" # ubuntu
        except IndexError:
            FONT = "/usr/share/fonts/bitstream-vera/Vera.ttf" # fedora
        except IndexError:
            print "Please, report this problem to leogregianin@gmail.com"
            sys.exit()
        nextfont = ImageFont.truetype(FONT, size)
        nexttextwidth, nexttextheight = nextfont.getsize(text)
        if nexttextwidth+nexttextheight/3 > write.size[0]: break
        font = nextfont
        textwidth, textheight = nexttextwidth, nexttextheight
    draw.setfont(font)
    draw.text(((write.size[0]-textwidth)/55, (write.size[0]-textheight)/55), text, fill=(120,120,120))
    img.save(outfilename)
    
def set_wallpaper():
    if sys.platform == 'win32':
        SPI_SETDESKWALLPAPER = 20
        ctypes.windll.user32.SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, "Picture_of_the_day.bmp", 0)
    else:
        gconf.client_get_default().get_string('/desktop/gnome/background/picture_options', 'scaled') 
        gconf.client_get_default().get_string('/desktop/gnome/background/picture_filename', 'Picture_of_the_day.png')

if __name__ == '__main__':
    commons = Site('commons', 'commons')
    date_today = time.strftime('%Y-%m-%d', time.localtime())
    template = 'Template:Potd/%s' % date_today
    templatePage = Page(commons, template)
    image_today = templatePage.get()
    image_name = 'Image:%s'% image_today
    imageURL = ImagePage(commons, image_name)
    featuredImage = imageURL.fileUrl()
    image = featuredImage[27:]

    if sys.platform == 'win32':
        if image.endswith('.svg'):
            sys.exit() # Windows background don't accept svg files

        ### Install CommonsPictureOfTheDay in registry
        Reg = ConnectRegistry(None, HKEY_LOCAL_MACHINE)
        Key = OpenKey(Reg, r"SOFTWARE\Microsoft\Windows\CurrentVersion\Run", 0, KEY_WRITE)
        # entry your correct pywikipediabot patch
        SetValueEx(Key,"CommonsPictureOfTheDay", 0, REG_SZ, r"C:\pywikipediabot\pywikipedia\CommonsPictureOfTheDay.py")
        CloseKey(Key)
        CloseKey(Reg)
    
        get_commons_image(image)

        write_gray('Picture_of_the_day.bmp',
                   'http://commons.wikimedia.org/wiki/Commons:Picture_of_the_day',
                   'Picture_of_the_day.bmp')

        set_wallpaper()
    
    else:
        get_commons_image(image)
        write_gray('Picture_of_the_day.png',
                   'http://commons.wikimedia.org/wiki/Commons:Picture_of_the_day',
                   'Picture_of_the_day.png')
        set_wallpaper()
