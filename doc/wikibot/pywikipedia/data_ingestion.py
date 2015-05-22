#!/usr/bin/python
# -*- coding: utf-8  -*-
'''
A generic bot to do data ingestion (batch uploading) to Commons

'''
import sys, os.path, glob, re, hashlib, base64, StringIO
import xml.etree.ElementTree
import wikipedia as pywikibot
import config, query, upload
import csv, urllib
import flickrripper
import pagegenerators

class DataIngestionBot:
    def __init__(self, configurationPage):
        '''

        '''
        self.site = configurationPage.site()
        self.configuration = self.parseConfigurationPage(configurationPage)

    def parseConfigurationPage(self, configurationPage):
        '''
        Expects a pywikibot.page object "configurationPage" which contains the configuration
        '''
        configuration  = {}
        # Set a bunch of defaults
        configuration['csvDialect']=u'excel'
        configuration['csvDelimiter']=';'
        configuration['csvEncoding']=u'Windows-1252' #FIXME: Encoding hell
        
        templates = configurationPage.templatesWithParams()
        for (template, params) in templates:
            if template==u'Data ingestion':
                for param in params:
                    (field, sep, value) = param.partition(u'=')
                    
                    # Remove leading or trailing spaces
                    field = field.strip()
                    value = value.strip()
                    configuration[field] = value
        print configuration            
        return configuration
        

    def downloadPhoto(self, photoUrl = ''):
        '''
        Download the photo and store it in a StrinIO.StringIO object.

        TODO: Add exception handling
        '''
        imageFile=urllib.urlopen(photoUrl).read()
        return StringIO.StringIO(imageFile)

    def findDuplicateImages(self, photo = None, site = pywikibot.getSite(u'commons', u'commons')):
        '''
        Takes the photo, calculates the SHA1 hash and asks the mediawiki api for a list of duplicates.

        TODO: Add exception handling, fix site thing
        '''
        hashObject = hashlib.sha1()
        hashObject.update(photo.getvalue())
        return site.getFilesFromAnHash(base64.b16encode(hashObject.digest()))

    def getDescription(self, metadata):
        '''
        Generate a description for a file
        '''

        description = u'{{%(configurationTemplate)s}}\n' % self.configuration
        for key, value in metadata.iteritems():
            description = description + u'|' + key + u'=%(' + key + u')s\n'
        description = description + u'}}\n'
           
        return description % metadata
        
    def getTitle(self, metadata):
        '''
        Build a title.
        Have titleFormat to indicate how the title would look.
        We need to be able to strip off stuff if it's too long. configuration.get('maxTitleLength')
        '''

        #FIXME: Make this configurable.
        title = self.configuration.get('titleFormat') % metadata

        description = metadata.get(u'dc:title')
        identifier = metadata.get(u'dc:identifier')

        if len(description)>120:
            description = description[0 : 120]

        title = u'%s - %s.jpg' % (description, identifier)
        
        return flickrripper.cleanUpTitle(title)

    def cleanDate(self, field):
        '''
        A function to do date clean up.
        '''
        # Empty, make it really empty
        if field==u'-':
            return u''
        # TODO: Circa
        # TODO: Period

        return field

    def cleanEmptyField(self, field):
        return field

    def procesFile(self, metadata):
        # FIXME: Do some metadata enrichment
        #metadata = getEuropeanaMetadata(metadata)

        fileLocation = metadata.get(self.configuration.get('sourceFileField'))

        photo = self.downloadPhoto(fileLocation)
        duplicates = self.findDuplicateImages(photo)

        # We don't want to upload dupes
        if duplicates:
            pywikibot.output(u'Found duplicate image at %s' % duplicates.pop())
            # The file is at Commons so return True
            return True

        # FIXME: Do some checking to see if the title already exists

        title = self.getTitle(metadata)
        description = self.getDescription(metadata)


        pywikibot.output(u'Preparing upload for %s.' % title)    
        pywikibot.output(description)    
                            
        bot = upload.UploadRobot(url=fileLocation, description=description, useFilename=title, keepFilename=True, verifyDescription=False, targetSite = self.site)
        bot.run()

    def processCSV(self):
        database = {}

        reader = csv.DictReader(open(self.configuration.get('csvFile'), "rb"), dialect=self.configuration.get('csvDialect'), delimiter=self.configuration.csvDelimiter)
        # FIXME : Encoding problems http://docs.python.org/library/csv.html#csv-examples
        for row in reader:
            self.metadataCSV(row)
            self.processFile(metadata)
            
    def run(self):
        '''
        Do crap
        '''
        if not self.configuration.get('sourceFormat'):
            pywikibot.output(u'The field "sourceFormat" is not set')
            return False
        
        if self.configuration.get('sourceFormat')==u'csv':
            self.processCSV()
        else:
            pywikibot.output(u'%s is not a supported source format')
        
def main(args):
    generator = None;
    
    genFactory = pagegenerators.GeneratorFactory()

    for arg in pywikibot.handleArgs():
        genFactory.handleArg(arg)

    generator = genFactory.getCombinedGenerator()
    if not generator:
        return False

    for page in generator:
        bot  = DataIngestionBot(page)
        bot.run()
    
if __name__ == "__main__":
    try:
        main(sys.argv[1:])
    finally:
        print "All done!"
