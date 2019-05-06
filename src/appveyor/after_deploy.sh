#!/bin/sh
# some bintray tweaks after deployment

#echo
#echo "Environment variables:"
#echo
#env | sort

FN="$(find "$APPVEYOR_BUILD_FOLDER/src" -type f \
    -name "*$APPVEYOR_BUILD_VERSION*" -printf '%f')"
curl -usasfit:$BT_API -H Content-Type:application/json -H Accept:application/json \
     -X PUT -d '{ "list_in_downloads":true }' \
     https://api.bintray.com/file_metadata/sasfit/development/$FN

# vim: set ts=4 sw=4 sts=4 tw=0 et:
