#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Using the BinTray REST API for some tweaks after deployment.
# Make sure all uploaded files appear in the direct downloads section

import sys, os, time
import requests # for HTTP requests
import json
from datetime import datetime
from dateutil import parser as dateparser

baseurl = "https://api.bintray.com"
subject, repo, package = "sasfit", "development", "SASfit"

if 'BT_API' not in os.environ:
    print("No BinTray authentication token ('BT_API' environment variable) provided!")
    sys.exit(1)

if 'SASFIT_VERSION' not in os.environ:
    print("No 'SASFIT_VERSION' environment variable provided!")
    sys.exit(1)

def prettyPrint(parsed, **kwargs):
    print(json.dumps(parsed, indent=2, sort_keys=True), **kwargs)

def secsToDays(ts):
    return ts/(60*60*24)

def getVersionTimestamp(version):
    dt = dateparser.isoparse(version)
    #print(version, "->", dt)
    return dt.timestamp()

def makeRequest(path, method, verbose=False, **kwargs):
    """Sends a web API request with the given path and method from python requests module."""
    url = baseurl + path
    if verbose:
        print("url:", url)
    response = method(url, auth=('sasfit', os.environ['BT_API']), **kwargs)
    if verbose:
        print(" ", "Status:", response.status_code)
    return response.json(), response.status_code

def deleteVersion(version):
    return makeRequest("/packages/{}/{}/{}/versions/{}".format(
                       subject, repo, package, version), requests.delete)

def deleteVersionsOlderThanDays(days):
    if days < 0.:
        return # nothing to do
    print("Deleting versions older than {} days:".format(days))
    # get all versions
    response, _ = makeRequest("/packages/{}/{}/{}".format(subject, repo, package),
                              requests.get)
    versions = response['versions']
    for version in versions:
        print(" ", version)
        entry, _ = makeRequest("/packages/{}/{}/{}/versions/{}".format(
                                    subject, repo, package, version), requests.get)
        #prettyPrint(ver)
        ts = getVersionTimestamp(entry['created'])
        deltaDays = secsToDays(datetime.now().timestamp()-ts)
        print("   ", "Created on {} ({:.2f} days ago):".format(entry['created'], deltaDays),
              end=" ")
        if deltaDays > days:
            print("delete.")
            deleteVersion(version)
        else:
            print("keep.")

def keepNewestVersions(count):
    if count < 1:
        return # nothing to do
    print("Deleting old versions, keeping the newest {}:".format(count))
    # get all versions
    response, _ = makeRequest("/packages/{}/{}/{}".format(subject, repo, package),
                              requests.get)
    versions = response['versions']
    print(" ", "Found {} versions.".format(len(versions)))
    count = min(count, len(versions))
    if count == len(versions):
        print(" ", "Nothing to do, keep'em all.")
        return
    print(" ", "Keeping:")
    [print("   ", v) for v in versions[:count]]
    print(" ", "Deleting:")
    for version in versions[count:]: # delete the oldest ones, assuming chrono sort
        print("   ", version)
        deleteVersion(version)

# Determine the current version
# latest version on BinTray is outdated, instead use what it's supposed to be
latestVersion = os.environ['SASFIT_VERSION']
print("Using version '{}'.".format(latestVersion))

# Asynchronously publishes all unpublished content for a user’s package version.
# https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-Publish/DiscardUploadedContent
print("Publish all unpublished content:")
response, code = makeRequest("/content/{}/{}/{}/{}/publish".format(subject, repo, package, latestVersion),
                             requests.post, json={'publish_wait_for_secs': -1})
print(" ", response, "Status:", code)

# Get available files on BinTray
# https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-GetPackageFiles
response, code = makeRequest("/packages/{}/{}/{}/versions/{}/files".format(
                                subject, repo, package, latestVersion), requests.get,
                             params={'include_unpublished': 1})
names = []
if response is not None:
    names = [entry['name'] for entry in response]

# Add uploaded files to download list, only possible for 'published' files, wait until published
for filename in names:
    print("Add uploaded", filename, "to direct download list:")
    tryAgain = True
    while tryAgain:
        response, code = makeRequest("/file_metadata/{}/{}/{}".format(subject, repo, filename),
                                     requests.put, json={'list_in_downloads': True})
        print(" ", response, "Status:", code)
        tryAgain = (code != 200) and ('unpublished' in response['message'])
        if tryAgain: # wait a moment before the next try
            delay = 5
            print(" ", "-> Trying again after {} seconds:".format(delay))
            time.sleep(delay)

#
## Cleanup on BinTray, limit number of files
#

# Get number of Package Files
files, _ = makeRequest("/packages/{}/{}/{}/files".format(subject, repo, package),
                       requests.get, verbose=False)
if files is not None:
    print("Number of files on BinTray:", len(files))

#deleteVersionsOlderThanDays(320)
keepNewestVersions(15)

# vim: set ts=4 sw=4 sts=4 tw=0 et:
