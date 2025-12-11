#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright 2020-2021 Ingo Breßler (dev@ingobressler.net)
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.
#
# Using the AppVeyor REST API to purge old builds.

import sys, os
import argparse
from requests import get, delete # for HTTP requests
from dateutil.parser import parse as dateutil_parse
from operator import itemgetter
from helpers import makeRequest, assertEnvVarExists, jsonPrettyPrint

parser = argparse.ArgumentParser(description='Remove old AppVeyor builds.')
parser.add_argument('buildsToKeep', metavar='N', type=int, default=20,
                    help='The number of builds to keep.')
parser.add_argument('--verbose', const=True, default=False, action='store_const',
                    help='Give more output for debugging')
args = parser.parse_args()

requiredEnvVars = ('APPVEYOR_ACCOUNT_NAME', 'APPVEYOR_PROJECT_SLUG', 'APPVEYOR_TOKEN')

for vname in requiredEnvVars:
    assertEnvVarExists(vname)

baseurl = f"https://ci.appveyor.com/api"
accountName = os.environ['APPVEYOR_ACCOUNT_NAME']
projectSlug = os.environ['APPVEYOR_PROJECT_SLUG']
authHead = {"Authorization": "Bearer " + os.environ['APPVEYOR_TOKEN']}

response, code = makeRequest(get, baseurl, f"/projects/{accountName}/{projectSlug}/history",
                             headers=authHead, verbose=args.verbose,
                             params=dict(recordsNumber=100))
if code != 200:
    print("Could not get build history! Stopping.")
    sys.exit(1)

#jsonPrettyPrint(response) # for debugging
builds = [(build['buildId'], dateutil_parse(build['finished']))
            for build in response['builds'] if 'finished' in build]
builds.sort(key=itemgetter(-1))
print(f"Found {len(builds)} builds.")
for buildId, finished in builds[:-args.buildsToKeep]: # keep the newest <buildsToKeep>
    print(f"Deleting buildId {buildId} finished at {finished.isoformat()}:", end=" ")
    response, code = makeRequest(delete, baseurl, f"/account/{accountName}/builds/{buildId}",
                                 headers=authHead, verbose=args.verbose)
    print("OK" if code == 204 else f"{code} != 204!")

# vim: set ts=4 sw=4 sts=4 tw=0 et:
