""" This tool sets an email address on all bot accounts and email confirms them."""

import sys, os, getpass
sys.path.append('..')

import poplib
import wikipedia, config
import preferences
import re
import urllib2

def confirm(link):
    req = urllib2.Request(link, headers = {'User-Agent': wikipedia.useragent})
    u = urllib2.urlopen(req)
    u.close()

if __name__ == '__main__':
    r_mail = re.compile(ur'(http\:\/\/\S*Confirmemail\S*)')

    email = wikipedia.input('Email?')
    host = wikipedia.input('Host?')
    port = wikipedia.input('Port (default: 110; ssl: 995)?')
    try:
        port = int(port)
    except ValueError:
        port = 0
    if not port:
        port = 110
    ssl = wikipedia.inputChoice('SSL? ', ['no', 'yes'],
        ['n', 'y'], (port == 995) and 'y' or 'n') == 'y'
    username = wikipedia.input('User?')
    password = wikipedia.input('Password?', True)
    do_delete = wikipedia.inputChoice('Delete confirmed mails?', ['yes', 'no'], ['y', 'n'], 'y') == 'y'

    if email:
        preferences.set_all(['wpUserEmail', 'wpEmailFlag', 'wpOpenotifusertalkpages'],
            [email, True, False], verbose = True)

    if ssl:
        pop = poplib.POP3_SSL(host, port)
    else:
        pop = poplib.POP3(host, port)

    pop.user(username)
    pop.pass_(password)
    wikipedia.output(unicode(pop.getwelcome()))
    messages = [i.split(' ', 1)[0] for i in pop.list()[1]]
    for i in messages:
        msg = pop.retr(i)
        confirmed = False
        for line in msg[1]:
            if r_mail.search(line):
                confirmed = True
                link = r_mail.search(line).group(1)
                wikipedia.output(u'Confirming %s.' % link)
                confirm(link)

        if not confirmed:
            wikipedia.output(u'Unconfirmed mail!')
        elif do_delete:
            pop.dele(i)
    pop.quit()

