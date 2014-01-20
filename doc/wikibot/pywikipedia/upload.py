# -*- coding: utf-8 -*-
"""
Script to upload images to wikipedia.

Arguments:

  -keep         Keep the filename as is
  -filename     Target filename
  -noverify     Do not ask for verification of the upload description if one
                is given

If any other arguments are given, the first is the URL or filename to upload,
and the rest is a proposed description to go with the upload. If none of these
are given, the user is asked for the file or URL to upload. The bot will then
upload the image to the wiki.

The script will ask for the location of an image, if not given as a parameter,
and for a description.
"""
#
# (C) Rob W.W. Hooft, Andre Engels 2003-2004
# (C) Pywikipedia bot team, 2003-2010
#
# Distributed under the terms of the MIT license.
#
__version__='$Id: upload.py 9042 2011-03-13 10:14:47Z xqt $'
#

import os, sys, time
import urllib
import mimetypes
import wikipedia as pywikibot
import config, query

def post_multipart(site, address, fields, files, cookies):
    """
    Post fields and files to an http host as multipart/form-data.
    fields is a sequence of (name, value) elements for regular form fields.
    files is a sequence of (name, filename, value) elements for data to be uploaded as files
    Return the server's response page.
    """
    contentType, body = encode_multipart_formdata(fields, files)
    return site.postData(address, body, contentType = contentType, cookies = cookies)

def encode_multipart_formdata(fields, files):
    """
    fields is a sequence of (name, value) elements for regular form fields.
    files is a sequence of (name, filename, value) elements for data to be uploaded as files
    Return (content_type, body) ready for httplib.HTTP instance
    """
    boundary = '----------ThIs_Is_tHe_bouNdaRY_$'
    lines = []
    for (key, value) in fields:
        lines.append('--' + boundary)
        lines.append('Content-Disposition: form-data; name="%s"' % str(key))
        lines.append('')
        try:
            lines.append(str(value))
        except UnicodeEncodeError:
            lines.append(value.encode('utf-8'))
    for (key, filename, value) in files:
        lines.append('--' + boundary)
        lines.append('Content-Disposition: form-data; name="%s"; filename="%s"' % (key, filename))
        lines.append('Content-Type: %s' % get_content_type(filename))
        lines.append('')
        lines.append(value)
    lines.append('--' + boundary + '--')
    lines.append('')
    body = '\r\n'.join(lines)
    content_type = 'multipart/form-data; boundary=%s' % boundary
    return content_type, body

def get_content_type(filename):
    return mimetypes.guess_type(filename)[0] or 'application/octet-stream'


class UploadRobot:
    def __init__(self, url, urlEncoding = None, description = u'', useFilename = None, keepFilename = False,
                 verifyDescription = True, ignoreWarning = False, targetSite = None, uploadByUrl = False):
        """
        ignoreWarning - Set this to True if you want to upload even if another
                        file would be overwritten or another mistake would be
                        risked.

        """
        self._retrieved = False
        self.url = url
        self.urlEncoding = urlEncoding
        self.description = description
        self.useFilename = useFilename
        self.keepFilename = keepFilename
        self.verifyDescription = verifyDescription
        self.ignoreWarning = ignoreWarning
        if config.upload_to_commons:
            self.targetSite = targetSite or pywikibot.getSite('commons', 'commons')
        else:
            self.targetSite = targetSite or pywikibot.getSite()
        self.targetSite.forceLogin()
        self.uploadByUrl = uploadByUrl

    def urlOK(self):
        """Return true if self.url looks like an URL or an existing local file.

        """
        return "://" in self.url or os.path.exists(self.url)

    def read_file_content(self):
        """Return name of temp file in which remote file is saved."""
        if not self._retrieved or self.uploadByUrl:
            # Get file contents
            pywikibot.output(u'Reading file %s' % self.url)
            if '://' in self.url:
                resume = False
                dt = 15

                while not self._retrieved:
                    uo = pywikibot.MyURLopener
                    headers = [('User-agent', pywikibot.useragent)]

                    if resume:
                        pywikibot.output(u"Resume download...")
                        headers.append(('Range', 'bytes=%s-' % rlen))
                    uo.addheaders = headers

                    file = uo.open(self.url)

                    if 'text/html' in file.info().getheader('Content-Type'):
                        print "Couldn't download the image: the requested URL was not found on this server."
                        return

                    content_len = file.info().getheader('Content-Length')
                    accept_ranges = file.info().getheader('Accept-Ranges') == 'bytes'

                    if resume:
                        self._contents += file.read()
                    else:
                        self._contents = file.read()

                    file.close()
                    self._retrieved = True

                    if content_len:
                        rlen = len(self._contents)
                        content_len = int(content_len)
                        if rlen < content_len:
                            self._retrieved = False
                            pywikibot.output(u"Connection closed at byte %s (%s left)" % (rlen, content_len))
                            if accept_ranges and rlen > 0:
                                resume = True
                            pywikibot.output(u"Sleeping for %d seconds..." % dt)
                            time.sleep(dt)
                            if dt <= 60:
                                dt += 15
                            elif dt < 360:
                                dt += 60
                    else:
                        if pywikibot.verbose:
                            pywikibot.output(u"WARNING: No check length to retrieved data is possible.")
            else:
                # Opening local files with MyURLopener would be possible, but we
                # don't do it because it only accepts ASCII characters in the
                # filename.
                file = open(self.url,"rb")
                self._contents = file.read()
                file.close()

    def process_filename(self):
        """Return base filename portion of self.url"""
        # Isolate the pure name
        filename = self.url
        # Filename may be either a local file path or a URL
        if '/' in filename:
            filename = filename.split('/')[-1]

        if '\\' in filename:
            filename = filename.split('\\')[-1]

        if self.urlEncoding:
            filename = urllib.unquote(filename.decode(self.urlEncoding))

        if self.useFilename:
            filename = self.useFilename
        if not self.keepFilename:
            pywikibot.output(
                u"The filename on the target wiki will default to: %s"
                % filename)
            ok = False
            # FIXME: these 2 belong somewhere else, presumably in family
            forbidden = '/' # to be extended
            allowed_formats = (u'gif', u'jpg', u'jpeg', u'mid', u'midi',
                               u'ogg', u'png', u'svg', u'xcf', u'djvu')
            # ask until it's valid
            while not ok:
                ok = True
                newfn = pywikibot.input(
                            u'Enter a better name, or press enter to accept:')
                if newfn == "":
                    newfn = filename
                ext = os.path.splitext(newfn)[1].lower().strip('.')
                for c in forbidden:
                    if c in newfn:
                        print "Invalid character: %s. Please try again" % c
                        ok = False
                if ext not in allowed_formats and ok:
                    choice = pywikibot.inputChoice(u"File format is not one of [%s], but %s. Continue?" % (u' '.join(allowed_formats), ext), ['yes', 'no'], ['y', 'N'], 'N')
                    if choice == 'n':
                        ok = False
            if newfn != '':
                filename = newfn
        # MediaWiki doesn't allow spaces in the file name.
        # Replace them here to avoid an extra confirmation form
        filename = filename.replace(' ', '_')
        # A proper description for the submission.
        pywikibot.output(u"The suggested description is:")
        pywikibot.output(self.description)
        if self.verifyDescription:
            newDescription = u''
            choice = pywikibot.inputChoice(
                u'Do you want to change this description?',
                ['Yes', 'No'], ['y', 'N'], 'n')
            if choice == 'y':
                import editarticle
                editor = editarticle.TextEditor()
                newDescription = editor.edit(self.description)
            # if user saved / didn't press Cancel
            if newDescription:
                self.description = newDescription
        return filename

    def upload_image(self, debug=False, sessionKey=0):
        """Upload the image at self.url to the target wiki.

        Return the filename that was used to upload the image.
        If the upload fails, ask the user whether to try again or not.
        If the user chooses not to retry, return null.

        """
        if not self.targetSite.has_api() or self.targetSite.versionnumber() < 16:
            return self._uploadImageOld(debug)

        if not hasattr(self,'_contents'):
            self.read_file_content()

        filename = self.process_filename()

        params = {
            'action': 'upload',
            'token': self.targetSite.getToken(),
            'comment': self.description,
            'filename': filename,
            #'': '',
        }
        if sessionKey:
            params['sessionkey'] = sessionKey
        if self.uploadByUrl:
            params['url'] = self.url
        elif not self.uploadByUrl and not sessionKey:
            params['file'] = self._contents

        if self.ignoreWarning:
            params['ignorewarnings'] = 1

        pywikibot.output(u'Uploading file to %s via API....' % self.targetSite)

        data = query.GetData(params, self.targetSite)

        if pywikibot.verbose:
            pywikibot.output("%s" % data)

        if 'error' in data: # error occured
            errCode = data['error']['code']
            pywikibot.output("%s" % data)
        else:
            data = data['upload']
            if data['result'] == u'Warning': #upload success but return warning.
                pywikibot.output("Got warning message:")
                for k,v in data['warnings'].iteritems():
                    if k == 'duplicate-archive':
                        pywikibot.output("\tThe file is duplicate a deleted file %s." % v)
                    elif k == 'was-deleted':
                        pywikibot.output("\tThis file was deleted for %s." % v)
                    elif k == 'emptyfile':
                        pywikibot.output("\tFile %s is an empty file." % v)
                    elif k == 'exists':
                        pywikibot.output("\tFile %s is exists." % v)
                    elif k == 'duplicate':
                        pywikibot.output("\tUploaded file is duplicate with %s." % v)
                    elif k == 'badfilename':
                        pywikibot.output("\tTarget filename is invalid.")
                    elif k == 'filetype-unwanted-type':
                        pywikibot.output("\tFile %s type is unwatched type." % v)
                answer = pywikibot.inputChoice(u"Do you want to ignore?", ['Yes', 'No'], ['y', 'N'], 'N')
                if answer == "y":
                    self.ignoreWarning = 1
                    self.keepFilename = True
                    return self.upload_image(debug, sessionKey = data['sessionkey'])
                else:
                    pywikibot.output("Upload aborted.")
                    return

            elif data['result'] == u'Success': #No any warning, upload and online complete.
                pywikibot.output(u"Upload successful.")
                return filename #data['filename']


    def _uploadImageOld(self, debug=False):
        if not hasattr(self,'_contents'):
            self.read_file_content()

        filename = self.process_filename()
        # Convert the filename (currently Unicode) to the encoding used on the
        # target wiki
        encodedFilename = filename.encode(self.targetSite.encoding())


        formdata = {
            'wpUploadDescription': self.description,
            'wpUploadAffirm': '1',
            'wpUpload': 'upload bestand',
            'wpEditToken': self.targetSite.getToken(), # Get an edit token so we can do the upload
            'wpDestFile': filename, # Set the new filename
        }
        # This somehow doesn't work.
        if self.ignoreWarning:
            formdata["wpIgnoreWarning"] = "1"

        if self.uploadByUrl:
            formdata["wpUploadFileURL"]  = self.url
            formdata["wpSourceType"] = 'Url'

        # try to encode the strings to the encoding used by the target site.
        # if that's not possible (e.g. because there are non-Latin-1 characters and
        # the home Wikipedia uses Latin-1), convert all non-ASCII characters to
        # HTML entities.
        for key in formdata:
            assert isinstance(key, basestring), "ERROR: %s is not a string but %s" % (key, type(key))
            try:
                formdata[key] = formdata[key].encode(self.targetSite.encoding())
            except (UnicodeEncodeError, UnicodeDecodeError):
                formdata[key] = pywikibot.UnicodeToAsciiHtml(formdata[key]).encode(self.targetSite.encoding())

        # don't upload if we're in debug mode
        if not debug:
            pywikibot.output(u'Uploading file to %s...' % self.targetSite)

            if self.uploadByUrl:
                # Just do a post with all the fields filled out
                response, returned_html = self.targetSite.postForm(self.targetSite.upload_address(), formdata.items(), cookies = self.targetSite.cookies())
            else:
                response, returned_html = post_multipart(self.targetSite, self.targetSite.upload_address(),
                                  formdata.items(), (('wpUploadFile', encodedFilename, self._contents),),
                                  cookies = self.targetSite.cookies())
            # There are 2 ways MediaWiki can react on success: either it gives
            # a 200 with a success message, or it gives a 302 (redirection).
            # Do we know how the "success!" HTML page should look like?
            # ATTENTION: if you changed your Wikimedia Commons account not to show
            # an English interface, this detection will fail!
            success_msg = self.targetSite.mediawiki_message('successfulupload')
            if success_msg in returned_html or response.code == 302:
                 pywikibot.output(u"Upload successful.")
            # The following is not a good idea, because the server also gives a 200 when
            # something went wrong.
            #if response.code in [200, 302]:
            #    pywikibot.output(u"Upload successful.")

            elif response.code == 301:
                pywikibot.output(u"Following redirect...")
                address = response.getheader('Location')
                pywikibot.output(u"Changed upload address to %s. Please update %s.py" % (address, self.targetSite.family.__module__))
                exec('self.targetSite.upload_address = lambda: %r' % address, locals(), globals())
                return self.upload_image(debug)
            else:
                try:
                    # Try to find the error message within the HTML page.
                    # If we can't find it, we just dump the entire HTML page.
                    returned_html = returned_html[returned_html.index('<!-- start content -->') + 22: returned_html.index('<!-- end content -->')]
                except:
                    pass
                pywikibot.output(u'%s\n\n' % returned_html)
                pywikibot.output(u'%i %s' % (response.code, response.msg))

                if self.targetSite.mediawiki_message('uploadwarning') in returned_html:
                    answer = pywikibot.inputChoice(u"You have recevied an upload warning message. Ignore?", ['Yes', 'No'], ['y', 'N'], 'N')
                    if answer == "y":
                        self.ignoreWarning = 1
                        self.keepFilename = True
                        return self._uploadImageOld(debug)
                else:
                    answer = pywikibot.inputChoice(u'Upload of %s probably failed. Above you see the HTML page which was returned by MediaWiki. Try again?' % filename, ['Yes', 'No'], ['y', 'N'], 'N')
                    if answer == "y":
                        return self._uploadImageOld(debug)
                    else:
                        return
        return filename

    def run(self):
        while not self.urlOK():
            if not self.url:
                pywikibot.output(u'No input filename given')
            else:
                pywikibot.output(u'Invalid input filename given. Try again.')
            self.url = pywikibot.input(u'File or URL where image is now:')
        return self.upload_image()


def main(*args):
    url = u''
    description = []
    keepFilename = False
    useFilename = None
    verifyDescription = True

    # process all global bot args
    # returns a list of non-global args, i.e. args for upload.py
    for arg in pywikibot.handleArgs(*args):
        if arg:
            if arg.startswith('-keep'):
                keepFilename = True
            elif arg.startswith('-filename:'):
                useFilename = arg[10:]
            elif arg.startswith('-noverify'):
                verifyDescription = False
            elif url == u'':
                url = arg
            else:
                description.append(arg)
    description = u' '.join(description)
    bot = UploadRobot(url, description=description, useFilename=useFilename,
                      keepFilename=keepFilename,
                      verifyDescription=verifyDescription)
    bot.run()

if __name__ == "__main__":
    try:
        main()
    finally:
        pywikibot.stopme()
