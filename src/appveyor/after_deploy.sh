#!/bin/sh
# some bintray tweaks after deployment

#echo "Find package:"
#find "$APPVEYOR_BUILD_FOLDER/src" -type f -name "*$APPVEYOR_BUILD_VERSION*"

echo "Wait for uploaded file being published?"
sleep 30
# publish first?
# POST /content/:subject/:repo/:package/:version/publish
# https://bintray.com/docs/api/
#curl -s -usasfit:$BT_API -H Content-Type:application/json -H Accept:application/json \
#    -d '{ "publish_wait_for_secs": -1 }' \
#    -X POST "https://api.bintray.com/content/sasfit/development/SASfit/$APPVEYOR_BUILD_VERSION/publish"

echo "Add uploaded file to download list:"
FN="$(find "$APPVEYOR_BUILD_FOLDER/src" -maxdepth 1 -type f \
    -name "*$APPVEYOR_BUILD_VERSION*" -printf '%f\n' | head -n1)"
curl -s -usasfit:$BT_API -H Content-Type:application/json -H Accept:application/json \
     -X PUT -d '{ "list_in_downloads":true }' \
     "https://api.bintray.com/file_metadata/sasfit/development/$FN"

# vim: set ts=4 sw=4 sts=4 tw=0 et:
