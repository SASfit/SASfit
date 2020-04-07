#!/usr/bin/env python3
# Using the BinTray REST API for some tweaks after deployment.
# Make sure all uploaded files appear in the direct downloads section

import sys, os, time
import requests # for HTTP requests

baseurl = "https://api.bintray.com"
subject, repo, package = "sasfit", "development", "SASfit"

if 'BT_API' not in os.environ:
    print("No BinTray authentication token ('BT_API' environment variable) provided!")
    sys.exit(1)

if 'SASFIT_VERSION' not in os.environ:
    print("No 'SASFIT_VERSION' environment variable provided!")
    sys.exit(1)

# latest version on BinTray is outdated, instead use what it's supposed to be
latestVersion = os.environ['SASFIT_VERSION']
print("Using version '{}'.".format(latestVersion))

# Asynchronously publishes all unpublished content for a user’s package version.
# https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-Publish/DiscardUploadedContent
print("Publish all unpublished content:")
url = "{}/content/{}/{}/{}/{}/publish".format(
        baseurl, subject, repo, package, latestVersion)
#print("url:", url)
response = requests.post(url, auth=('sasfit', os.environ['BT_API']),
                         json={'publish_wait_for_secs': -1})
print(" ", response.json(), "Status:", response.status_code)

## Get available files on BinTray
## https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-GetPackageFiles
url = "{}/packages/{}/{}/{}/versions/{}/files".format(
        baseurl, subject, repo, package, latestVersion)
#print("url:", url)
response = requests.get(url, auth=('sasfit', os.environ['BT_API']),
                        params={'include_unpublished': 1})
names = []
if not response.ok:
    print(" ", response.json(), "Status:", response.status_code)
else:
    names = [entry['name'] for entry in response.json()]

# Add uploaded files to download list, only possible for 'published' files, wait until published
for filename in names:
    print("Add uploaded", filename, "to direct download list:")
    url = "{}/file_metadata/{}/{}/{}".format(baseurl, subject, repo, filename)
    #print("url:", url)
    tryAgain = True
    while tryAgain:
        response = requests.put(url, auth=('sasfit', os.environ['BT_API']),
                                json={'list_in_downloads': True})
        print(" ", response.json(), "Status:", response.status_code)
        tryAgain = not response.ok and 'unpublished' in response.json()['message']
        if tryAgain: # wait a moment before the next try
            time.sleep(5)
            print(" ", "Trying again:")

# vim: set ts=4 sw=4 sts=4 tw=0 et:
