#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright 2021 Ingo Breßler (dev@ingobressler.net)
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.
#
# Uploading binary packages (artifacts) to cloudsmith.io repo

import sys, os
from pathlib import Path
from requests import put, post # for HTTP requests
from .helpers import makeRequest, assertEnvVarExists, sha256, UploadWithProgress

requiredEnvVars = ("CLOUDSMITH_ORG", "CLOUDSMITH_REPO", "CLOUDSMITH_KEY",
                   "PCKG_NAME", "PCKG_VERSION", "PCKG_DESCR")

def uploadFile(filepath):
    for vname in requiredEnvVars:
        assertEnvVarExists(vname)

    org = os.environ['CLOUDSMITH_ORG']
    repo = os.environ['CLOUDSMITH_REPO']
    apikey = os.environ['CLOUDSMITH_KEY']
    chksum = sha256(filepath)

    headers = {
        "Accept": "*/*",
        "Content-Type": "application/json",
        "X-Api-Key": apikey
    }

    prefix = "[cloudsmith]"
    print(prefix, f"Uploading {filepath}:")

    baseurl = "https://upload.cloudsmith.io"
    response, code = makeRequest(put, baseurl, f"/{org}/{repo}/{filepath.name}",
         data=UploadWithProgress(filepath),
         headers=dict({"Content-Sha256": chksum}, **headers))
    print("  Status:", code)
    identifier = response.get("identifier", None)
    if identifier is None:
        print("Failed to upload {filepath.name}!", code)
        print(" ", response)
        sys.exit()

    print(prefix, "Creating package ...")

    baseurl = "https://api.cloudsmith.io/v1"

    payload = {
        "name": os.environ['PCKG_NAME'],
        "version": os.environ['PCKG_VERSION'],
        "summary": os.environ['PCKG_DESCR'],
        "package_file": identifier,
        "republish": True,
    }
    response, code = makeRequest(post, baseurl, f"/packages/{org}/{repo}/upload/raw/", json=payload, headers=headers)
    #print(" ", response, "Status:", code)
    print("  Status:", code)
    try:
        print(f"  Available at {response['self_html_url']}")
    except (AttributeError, IndexError) as e:
        print(e)
        print(response)

    print(prefix, "Done.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("WARNING: No file for upload provided")
        sys.exit(1)
    uploadFile(Path(sys.argv[1]).resolve())

# vim: set ts=4 sw=4 sts=4 tw=0 et:
