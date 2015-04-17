#!/usr/bin/python
# -*- coding: utf-8  -*-
"""
This module allow you to use the API in a simple and easy way.


-- Example --

    params = {
        'action'    :'query',
        'prop'      :'revisions',
        'titles'    :'Test',
        'rvlimit'   :'2',
        'rvprop'    :'user|timestamp|content',
        }

    print query.GetData(params, encodeTitle = False)

"""
#
# (C) Yuri Astrakhan, 2006
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: query.py 9214 2011-05-04 20:40:36Z xqt $'
#

import wikipedia, time
try:
    #For Python 2.6 newer
    import json
    if not hasattr(json, 'loads'):
        # 'json' can also be the name in for
        # http://pypi.python.org/pypi/python-json
        raise ImportError
except ImportError:
    import simplejson as json


def GetData(params, site = None, useAPI = True, retryCount = 5, encodeTitle = True, sysop = False, back_response = False):
    """Get data from the query api, and convert it into a data object
    """
    if not site:
        site = wikipedia.getSite()
    data = {}
    titlecount = 0

    for k,v in params.iteritems():
        if k == u'file':
            data[k] = v
        elif type(v) == list:
            if k in [u'titles', u'pageids', u'revids', u'ususers'] and len(v) > 10:
                # Titles param might be long, case convert it to post request
                titlecount = len(params[k])
                data[k] = unicode(ListToParam(v))
            else:
                params[k] = unicode(ListToParam(v))

        elif not isinstance(v,basestring):
            params[k] = unicode(v)
        elif type(v) == unicode:
            params[k] = ToUtf8(v)

    if 'format' not in params or params['format'] != u'json':
        params['format'] = u'json'

    if not useAPI:
        params['noprofile'] = ''

    if data:
        for k in data:
            del params[k]

    if wikipedia.verbose: #dump params info.
        wikipedia.output(u"==== API action:%s ====" % params[u'action'])
        if data and 'file' not in data:
            wikipedia.output(u"%s: (%d items)" % (data.keys()[0], titlecount ) )

        for k, v in params.iteritems():
            if k not in ['action', 'format', 'file', 'xml', 'text']:
                if k == 'lgpassword' and wikipedia.verbose == 1:
                    v = u'XXXXX'
                elif not isinstance(v, unicode):
                    v = v.decode('utf-8')
                wikipedia.output(u"%s: %s" % (k, v) )
        wikipedia.output(u'-' * 16 )


    postAC = [
        'edit', 'login', 'purge', 'rollback', 'delete', 'undelete', 'protect', 'parse',
        'block', 'unblock', 'move', 'emailuser','import', 'userrights', 'upload', 'patrol'
    ]
    if useAPI:
        if params['action'] in postAC:
            path = site.api_address()
            cont = ''
        else:
            path = site.api_address() + site.urlEncode(params.items())

    else:
        path = site.query_address() + site.urlEncode(params.items())

    if wikipedia.verbose:
        if titlecount > 1:
            wikipedia.output(u"Requesting %d %s from %s" % (titlecount, data.keys()[0], site))
        else:
            wikipedia.output(u"Requesting API query from %s" % site)

    lastError = None
    retry_idle_time = 1

    while retryCount >= 0:
        try:
            jsontext = "Nothing received"
            if params['action'] == 'upload' and ('file' in data):
                import upload
                res, jsontext = upload.post_multipart(site, path, params.items(),
                  (('file', params['filename'].encode(site.encoding()), data['file']),),
                  site.cookies(sysop=sysop)
                  )
            elif params['action'] in postAC:
                res, jsontext = site.postForm(path, params, sysop, site.cookies(sysop = sysop) )
            else:
                if back_response:
                    res, jsontext = site.getUrl( path, retry=True, data=data, sysop=sysop, back_response=True)
                else:
                    jsontext = site.getUrl( path, retry=True, sysop=sysop, data=data)

            # This will also work, but all unicode strings will need to be converted from \u notation
            # decodedObj = eval( jsontext )

            jsontext = json.loads( jsontext )

            if "error" in jsontext:
                errorDetails = jsontext["error"]
                if errorDetails["code"] == 'badtoken':
                    wikipedia.output('Received a bad login token error from the server.  Attempting to refresh.')
                    params['token'] = site.getToken(sysop = sysop, getagain = True)
                    continue

            if back_response:
                return res, jsontext
            else:
                return jsontext

        except ValueError, error:
            if "<title>Wiki does not exist</title>" in jsontext:
                raise wikipedia.NoSuchSite(u'Wiki %s does not exist yet' % site)

            if 'Wikimedia Error' in jsontext: #wikimedia server error
                raise wikipedia.ServerError

            retryCount -= 1
            wikipedia.output(u"Error downloading data: %s" % error)
            wikipedia.output(u"Request %s:%s" % (site.lang, path))
            lastError = error
            if retryCount >= 0:
                wikipedia.output(u"Retrying in %i minutes..." % retry_idle_time)
                time.sleep(retry_idle_time*60)
                # Next time wait longer, but not longer than half an hour
                retry_idle_time *= 2
                if retry_idle_time > 30:
                    retry_idle_time = 30
            else:
                wikipedia.debugDump('ApiGetDataParse', site, str(error) + '\n%s\n%s' % (site.hostname(), path), jsontext)



    raise lastError

def GetInterwikies(site, titles, extraParams = None ):
    """ Usage example: data = GetInterwikies('ru','user:yurik')
    titles may be either ane title (as a string), or a list of strings
    extraParams if given must be a dict() as taken by GetData()
    """

    params = {
        'action': 'query',
        'prop': 'langlinks',
        'titles': ListToParam(titles),
        'redirects': 1,
    }
    params = CombineParams( params, extraParams )
    return GetData(params, site)

def GetLinks(site, titles, extraParams = None ):
    """ Get list of templates for the given titles
    """
    params = {
        'action': 'query',
        'prop': 'links',
        'titles': ListToParam(titles),
        'redirects': 1,
    }
    params = CombineParams( params, extraParams )
    return GetData(params, site)

#
#
# Helper utilities
#
#

def CleanParams( params ):
    """Params may be either a tuple, a list of tuples or a dictionary.
    This method will convert it into a dictionary
    """
    if params is None:
        return {}
    pt = type( params )
    if pt == dict:
        return params
    elif pt == typle:
        if len( params ) != 2: raise "Tuple size must be 2"
        return {params[0]:params[1]}
    elif pt == list:
        for p in params:
            if p != tuple or len( p ) != 2: raise "Every list element must be a 2 item tuple"
        return dict( params )
    else:
        raise "Unknown param type %s" % pt

def CombineParams( params1, params2 ):
    """Merge two dictionaries. If they have the same keys, their values will
    be appended one after another separated by the '|' symbol.
    """

    params1 = CleanParams( params1 )
    if params2 is None:
        return params1
    params2 = CleanParams( params2 )

    for k, v2 in params2.iteritems():
        if k in params1:
            v1 = params1[k]
            if len( v1 ) == 0:
                params1[k] = v2
            elif len( v2 ) > 0:
                if str in [type(v1), type(v2)]:
                    raise "Both merged values must be of type 'str'"
                params1[k] = v1 + '|' + v2
            # else ignore
        else:
            params1[k] = v2
    return params1

def ConvToList( item ):
    """Ensure the output is a list
    """
    if item is None:
        return []
    elif isinstance(item,basestring):
        return [item]
    else:
        return item

def ListToParam( list ):
    """Convert a list of unicode strings into a UTF8 string separated by the '|'
    symbols

    """
    list = ConvToList( list )
    if len(list) == 0:
        return ''

    encList = ''
    # items may not have one symbol - '|'
    for item in list:
        if isinstance(item, basestring):
            if u'|' in item:
                raise wikipedia.Error(u"item '%s' contains '|' symbol" % item)
            encList += ToUtf8(item) + u'|'
        elif type(item) == int:
            encList += ToUtf8(item) + u'|'
        elif isinstance(item, wikipedia.Page):
            encList += ToUtf8(item.title()) + u'|'
        elif item.__class__.__name__ == 'User':
            # delay loading this until it is needed
            import userlib
            encList += ToUtf8(item.name()) + u'|'
        else:
            raise wikipedia.Error(u'unknown item class %s'
                                  % item.__class__.__name__)

    # strip trailing '|' before returning
    return encList[:-1]

def ToUtf8(s):
    if type(s) != unicode:
        try:
            s = unicode(s)
        except UnicodeDecodeError:
            s = s.decode(wikipedia.config.console_encoding)
    return s

if __name__ == '__main__':
    """
    Testing code for this module
    """
    wikipedia.output("""
    This module is not for direct usage from the command prompt.
    """)
    # unit tests
    import tests.test_query
    import unittest
    unittest.main(tests.test_query)
