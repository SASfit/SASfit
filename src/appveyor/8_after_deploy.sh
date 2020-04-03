#!/bin/sh
# some bintray tweaks after deployment

CURL="curl -s -usasfit:$BT_API -H Content-Type:application/json -H Accept:application/json"

echo "Wait for uploaded file being published:"
#sleep 30
# publish first?
# POST /content/:subject/:repo/:package/:version/publish
# https://bintray.com/docs/api/
out=$($CURL -X POST -d '{ "publish_wait_for_secs": -1 }' \
      "https://api.bintray.com/content/sasfit/development/SASfit/$SASFIT_VERSION/publish")
echo "    $out"
# stop here if the artifact was not uploaded
(echo "$out" | grep -q 'not found') && exit 1

echo "Add uploaded file to download list:"
FN="$(find "$APPVEYOR_BUILD_FOLDER/src" -maxdepth 1 -type f \
    -name "*$SASFIT_VERSION*" | head -n1)"
FN="${FN##*/}"
echo "FN: '$FN'"
if [ -z "$FN" ];
then
    echo "No package file found for listing in download section of BinTray."
else
    $CURL -X PUT -d '{ "list_in_downloads":true }' \
         "https://api.bintray.com/file_metadata/sasfit/development/$FN"
    echo
fi

# vim: set ts=4 sw=4 sts=4 tw=0 et:
