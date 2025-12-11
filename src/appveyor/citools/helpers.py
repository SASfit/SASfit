# -*- coding: utf-8 -*-
#
# Copyright 2020-2021 Ingo Breßler (dev@ingobressler.net)
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.

import sys, os
from pathlib import Path
import requests # for HTTP requests
import json
try:
    from simplejson.errors import JSONDecodeError
except ImportError:
    from json.decoder import JSONDecodeError

def assertEnvVarExists(varname, purpose=None):
    if varname in os.environ:
        return True
    item = f"'{varname}' environment variable"
    if purpose is not None:
        print(f"No {purpose} provided ({item})!")
    else:
        print(f"No {item} provided!")
    sys.exit(1)

def makeRequest(method, baseurl, path, verbose=False, **kwargs):
    """Sends a web API request with the given path and method from python requests module."""
    url = baseurl + path
    if verbose:
        print(method, url, kwargs, type(url))
    response = method(url, **kwargs)
    if verbose:
        print(" ", "Status:", response.status_code)
    try:
        return response.json(), response.status_code
    except JSONDecodeError:
        return response, response.status_code

def jsonPrettyPrint(parsed, **kwargs):
    print(json.dumps(parsed, indent=2, sort_keys=True), **kwargs)

import hashlib
def sha256(filename):
    algo = hashlib.sha256()
    blkSize = 100*1024 # 100 KiB
    with open(filename, 'rb') as fd:
        # Read and update hash block wise
        for blk in iter(lambda: fd.read(blkSize), b""):
            algo.update(blk)
    return algo.hexdigest()

def downloadFile(url, targetFilename, progressstepsMB=15):
    print(f"Downloading {url} to '{targetFilename}':")
    with requests.get(url, stream=True) as response:
        response.raise_for_status()
        dlcount = 0
        chunksize = 8192
        progressintval = (progressstepsMB * 1000*1000 // chunksize) * chunksize
        with open(targetFilename, 'wb') as fh:
            for chunk in response.iter_content(chunk_size=chunksize):
                # If you have chunk encoded response uncomment if
                # and set chunk_size parameter to None.
                #if chunk:
                fh.write(chunk)
                dlcount += chunksize
                if dlcount % progressintval == 0:
                    print(f"{dlcount//(1000*1000)} MB")
    if Path(targetFilename).is_file():
        print(f"Downloaded '{targetFilename}'.")

class UploadWithProgress:
    """Provides an iterator for uploading files in chunks to allow progress tracking.
    For use with the *data* argument of a request."""

    @staticmethod
    def printProgress(ratio):
        if ratio is not None:
            print(f"{ratio*100:7.1f} %")

    def __init__(self, filename, chunksize=5*1024*1024,
                 callback=printProgress.__func__):
        self.filename = filename
        self.chunksize = chunksize
        self.totalsize = filename.stat().st_size # assuming pathlib.Path
        self.callback = callback
        self.readsofar = 0

    def __iter__(self):
        with open(self.filename, 'rb') as fd:
            while True:
                data = fd.read(self.chunksize)
                if not data:
                    if callable(self.callback):
                        self.callback(None)
                    break
                self.readsofar += len(data)
                if callable(self.callback):
                    self.callback(self.readsofar / self.totalsize)
                yield data

    def __len__(self):
        return self.totalsize

# vim: set ts=4 sw=4 sts=4 tw=0 et:
