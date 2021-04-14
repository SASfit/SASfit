#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Upload the current build to the cloudsmith repo

import os, sys
from pathlib import Path
from citools.helpers import assertEnvVarExists
from citools.cloudsmith import uploadFile

requiredEnvVars = ("CLOUDSMITH_KEY",)

for vname in requiredEnvVars:
    assertEnvVarExists(vname)

for item in os.environ.items():
    print(item)
sys.exit()

os.environ["CLOUDSMITH_ORG"] = "sasfit"
os.environ["CLOUDSMITH_REPO"] = "build"
os.environ["PCKG_NAME"] = os.environ["CACHEID"]
os.environ["PCKG_VERSION"] = os.environ["SASFIT_VERSION"]
os.environ["PCKG_DESCR"] = "SASfit development build"

uploadFile(Path(sys.argv[1]).resolve())

# vim: set ts=4 sw=4 sts=4 tw=0 et:
