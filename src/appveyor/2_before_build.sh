#!/bin/sh

echo
echo "## Listing environment variables ..."
echo
env | sort
echo
echo "Checking appveyor command line tool:"
set -x
appveyor UpdateBuild -h
appveyor -h

# vim: set ts=4 sw=4 sts=4 tw=0 et:
