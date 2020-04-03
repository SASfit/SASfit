#!/bin/sh
# some bintray tweaks after deployment

CURL="curl -s -usasfit:$BT_API -H Content-Type:application/json -H Accept:application/json"

env | sort

echo "Wait for uploaded file being published:"
#sleep 30
# publish first?
# POST /content/:subject/:repo/:package/:version/publish
# https://bintray.com/docs/api/
out=$($CURL -X POST -d '{ "publish_wait_for_secs": -1 }' \
      "https://api.bintray.com/content/sasfit/development/SASfit/$APPVEYOR_BUILD_VERSION/publish")
echo "    $out"
# stop here if the artifact was not uploaded
(echo "$out" | grep -q 'not found') && exit 1

echo "Add uploaded file to download list:"
FN="$(find "$APPVEYOR_BUILD_FOLDER/src" -maxdepth 1 -type f \
    -name "*$APPVEYOR_BUILD_VERSION*" -printf '%f\n' | head -n1)"
FN="${FN##*/}"
echo "FN: '$FN'"
$CURL -X PUT -d '{ "list_in_downloads":true }' \
     "https://api.bintray.com/file_metadata/sasfit/development/$FN"
echo

# vim: set ts=4 sw=4 sts=4 tw=0 et:
