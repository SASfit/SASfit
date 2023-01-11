#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Upload the current build to Zenodo

import os, sys, tempfile, json, hashlib, re
from copy import deepcopy
from pathlib import Path
from pprint import pprint, pformat
from time import sleep
import yaml
from citools.helpers import assertEnvVarExists, downloadFile
from requests import put, post, get, delete # for HTTP requests
from citools.helpers import makeRequest, sha256, UploadWithProgress

if os.environ.get('APPVEYOR_REPO_TAG', '').lower() != 'true':
    print("Only tagged versions get uploaded to Zenodo! Giving up.")
    sys.exit()

requiredEnvVars = ("SASFIT_PACKAGE_FILE", "SASFIT_VERSION", "SASFIT_RELEASE_DATE",
                   "ZENODO_TOKEN",)

for vname in requiredEnvVars:
    assertEnvVarExists(vname)

#verbose = bool(int(os.environ["ZENODO_DEBUG"]))
verbose = True
packagefile = Path(os.environ["SASFIT_PACKAGE_FILE"]).resolve()
packagever  = os.environ["SASFIT_VERSION"]
packagedate = os.environ["SASFIT_RELEASE_DATE"]

token = os.environ["ZENODO_TOKEN"]
params = {
    "access_token": os.environ["ZENODO_TOKEN"]
}

baseurl = "https://sandbox.zenodo.org/api" # FIXME
prefix = "[zenodo]"

def out(*args, **kwargs):
    print(prefix, *[(pformat(obj) if isinstance(obj, dict) else obj) for obj in args])

out(f"Running for package version {packagever} at {packagedate} in path {os.getcwd()}.")

# get some initial meta data from CITATION.cff, such as GitHub project URL
cff = "CITATION.cff"
with open(cff, 'r') as fh:
    cffdata = yaml.safe_load(fh)
    ghUrl = cffdata['repository-code']
    ghUserProject = '/'.join(ghUrl.split('/')[-2:])
if verbose:
    print(cff)
    pprint(cffdata)

def getRecord(state='draft', verbose=False):
    reclst, code = makeRequest(get, baseurl, f"/deposit/depositions",
                               params=dict(access_token=token, status=state),
                               json={})
    if verbose:
        out("Get the current draft: Status", code)
        out(reclst)
    return reclst[0] if len(reclst) else None

def delAllFilesInDraft():
    out("Removing files from draft record …")
    record = getRecord(state='draft')
    out("Remove files from:", record)
    if 'files' not in record:
        return
    for file in record['files']:
        response, code = makeRequest(delete, file['links']['self'], f"",
                                     params=dict(access_token=token),
                                     json={})
        out(response)

def updatedMetaData(oldmeta):
    newmeta = deepcopy(oldmeta)
    newmeta['publication_date'] = packagedate
    newmeta['version'] = packagever
    newmeta['license'] = cffdata['license']
    newmeta['related_identifiers'] = [{
            'identifier': ghUrl +'/tree/'+ packagever,
            'relation': 'isSupplementTo', 'scheme': 'url'}]
    newmeta['upload_type'] = cffdata['type']
    newmeta['access_right'] = 'open'
    return newmeta

def calcChecksum(filepath, method):
    with open(filepath, 'rb') as fh:
        filehash = getattr(hashlib, method)()
        chunk = fh.read(8192)
        while chunk:
            filehash.update(chunk)
            chunk = fh.read(8192)
        return filehash.hexdigest()

def uploadFile(record, filepath, verbose=False):
    if 'files' in record and filepath.name in [file['filename'] for file in record['files']]:
        out(f"{filepath.name} exists already in the target record of {record['links']['latest_draft_html']}, not uploading.")
        return
    assert filepath.is_file(), "Given package file does not exist!"
    out(f"Uploading '{filepath}' …")
    response, code = makeRequest(put, record['links']['bucket'], '/'+filepath.name,
         data=UploadWithProgress(filepath), params=params, verbose=verbose)
    out("Status:", code)
    out("Upload response:", response)
    method, checksum = response['checksum'].split(':')
    checksumCalcd = calcChecksum(filepath, method)
    out(f"Checksum: {checksum} with {method} "
       f"{'is OK' if checksumCalcd == checksum else 'DOES NO MATCH'}!")

def updateMeta(record, meta, verbose=False):
    """Updating meta data also updates the file listing, old api vs. new (bucket) api."""
    record, _ = makeRequest(put, record['links']['self'], '', data=json.dumps({'metadata':meta}),
                            headers={'Content-Type':'application/json'}, params=params, verbose=verbose)
    if verbose:
        out("Updated metadata:", record)
    return record

# Check for an existing draft first
draft = getRecord()
if verbose:
    out("Existing record:", draft)

newmeta = None
if draft is not None:
    # Update meta data for the new version
    newmeta = updatedMetaData(draft['metadata'])
    if draft['metadata']['version'] != newmeta['version']:
        # delete existing draft since it does not match current version
        response, code = makeRequest(delete, draft['links']['self'], "",
                                     params=dict(access_token=token),
                                     json={})
        out("Deleted existing draft: Status", code)
        if code == 204:
            draft = None
            sleep(1)

if draft is None:
    out("Creating a new version since no draft exists")
    parent = getRecord(state='published')
    if verbose:
        out("Parent record:", parent)
    if parent is not None and 'links' in parent:
        response, code = makeRequest(post, parent['links']['self'], "/actions/newversion",
                                     params=dict(access_token=token),
                                     json={}) # returns the parent record
        # Update the draft
        draft = getRecord()
    else: # if there is none yet, create one with metadata
        draft, _ = makeRequest(post, baseurl, f"/deposit/depositions",
                               params=dict(access_token=token), json={})
        # get initial meta data from cff file
        from cffconvert.cli.create_citation import create_citation
        metadata = json.loads(create_citation(cff, None).as_zenodo())
        print(f"Meta data from '{cff}':")
        pprint(metadata)
        # update the meta data with current version and more details
        newmeta = updatedMetaData(metadata)
        draft = updateMeta(draft, newmeta, verbose=verbose)

if verbose:
    out("Current draft:", draft)

if draft is None: # still!
    out("Could not get or create a draft record! Aborting.")
    sys.exit()

newmeta = updatedMetaData(draft['metadata'])

# work around empty files listing in record (although they are in the bucket)
# and missing bucket link
if ('files' not in draft
    or not len(draft['files'])): # no files listed yet
    # Upload a temp file to get the bucket link (new file upload api)
    _, tmpfn = tempfile.mkstemp()
    response, code = makeRequest(post, draft['links']['files'], f"",
                                 params=dict(access_token=token),
                                 data=dict(name="tempfile"),
                                 files=dict(file=open(tmpfn, 'rb')),
                                 json={})
    if verbose:
        out("Uploaded temp file:", response) # the uploaded file info
    try:
        os.remove(tmpfn) # may fail on Windows: file in use
    except PermissionError:
        pass

draft = updateMeta(draft, newmeta, verbose=verbose)

# try to upload the source tarball, e.g.:
# wget -c -O sasfit_0.94.11.zip https://api.github.com/repos/SASfit/SASfit/zipball/0.94.11
# figure out the base filename for the source package to download and store
basename = packagefile.name[:packagefile.name.find(packagever)+len(packagever)]
zipballfn = Path(basename + '-source.zip')
if ('files' not in draft
    or zipballfn.name not in [file['filename'] for file in draft['files']]):
    if not zipballfn.is_file():
        downloadFile("https://api.github.com/repos/"+ghUserProject+"/zipball/"+packagever, zipballfn)
    uploadFile(draft, zipballfn, verbose=verbose)

# Upload file if it does not exist there yet
# - The file uploaded to the bucket is not shown until files are modified with the old API
#   (see tempfile above), therefore record is updated after deleting tempfile at the end
# - but it is listed on the 'latest_draft_html' rendering page
uploadFile(draft, packagefile, verbose=verbose)

draft = updateMeta(draft, newmeta, verbose=verbose)
# Remove all files not matching the current version
for file in draft['files']:
    if re.search(r'[_-]'+packagever.replace('.',r'\.')+r'[_.-]', file['filename']) is None:
        response, code = makeRequest(delete, file['links']['self'], f"",
                                     params=dict(access_token=token), json={})
        out(f"Deleted '{file['filename']}':", code)

draft = updateMeta(draft, newmeta, verbose=verbose)
if len(draft['files']) == 4: # perhaps, check with AppVeyor API to check for current number of jobs done
    # finalize this record
    published, code = makeRequest(post, draft['links']['self'], '/actions/publish',
                                  params=dict(access_token=token),
                                  json={}) # returns the parent record
    out("Published record:", published['id'] if 'id' in published else published)

# vim: set ts=4 sw=4 sts=4 tw=0 et:
