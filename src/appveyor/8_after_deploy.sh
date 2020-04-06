#!/bin/sh
# Using the BinTray REST API for some tweaks after deployment.
# Make sure all uploaded files appear in the direct downloads section

CURL="curl -s -u sasfit:$BT_API -H Content-Type:application/json -H Accept:application/json"
API_URL="https://api.bintray.com"
PKG_PATH="sasfit/development/SASfit"

# latest version via BinTray REST API is outdated, using what it's supposed to be
latest_ver=$SASFIT_VERSION
if [ -z "$latest_version" ]; then
    # get the latest version, previously created one, probably
    latest_ver=$($CURL -X GET "$API_URL/packages/$PKG_PATH" \
        | grep -oE 'latest_version"?:"?[^"]+' | grep -oE '[^"]+$')
fi
echo "Latest version queried on BinTray: '$latest_ver'."

# Asynchronously publishes all unpublished content for a user’s package version.
# https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-Publish/DiscardUploadedContent
printf "Publish all unpublished content: "
$CURL -X POST -d '{ "publish_wait_for_secs": -1 }' -w " HTTP Status: %{http_code}\n" \
      "$API_URL/content/$PKG_PATH/$latest_ver/publish"
sleep 1 # wait a moment, BinTray does not seem to publish immediately

# Get available files on BinTray
# https://www.jfrog.com/confluence/display/BT/Bintray+REST+API#BintrayRESTAPI-GetPackageFiles
# for debugging:
echo "Files on BinTray and 'published' status:"
$CURL -X GET "$API_URL/packages/$PKG_PATH/versions/$latest_ver/files?include_unpublished=1" | python3 -c 'import sys, json; [print(" ", elem["name"], elem["published"]) for elem in json.loads(sys.stdin.readline())]'

# Add uploaded files to download list, only possible for 'published' files
for fn in $($CURL -X GET "$API_URL/packages/$PKG_PATH/versions/$latest_ver/files?include_unpublished=1" | python3 -c 'import sys, json; [print(elem["name"]) for elem in json.loads(sys.stdin.readline())]'); do
    printf "Add uploaded $fn to direct download list:\n  "
    output=400
    while [ "$output" = 400 ]; do
        output="$($CURL -X PUT -d '{ "list_in_downloads":true }' -w " HTTP Status: %{http_code}\n" \
             "$API_URL/file_metadata/sasfit/development/$fn")"
        echo "$output"
        output="$(echo "$output" | grep -oE '[0-9]+$')"
        [ "$output" = 400 ] && sleep 5 # not published yet
    done
done

# vim: set ts=4 sw=4 sts=4 tw=0 et:
