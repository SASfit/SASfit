#!/bin/sh

echo
echo "## Installing packaging tools:"
echo

set -x

# path of this script
scriptdir="$(cd "$(dirname "$0")" && pwd -P)"
# install ssh deploy key
[ -z "${SF_DEPLOY_KEY}" ] || echo "${SF_DEPLOY_KEY}" | tr " " "\n" >> $HOME/.ssh/id_rsa
# Install AppImage-Builder and requirements
# https://appimage-builder.readthedocs.io/en/latest/intro/install.html
sudo apt-get -y install libfuse2
# latest version: https://github.com/AppImageCrafters/appimage-builder/releases/download/Continuous/appimage-builder-1.1.1.dev32+g2709a3b-x86_64.AppImage
# does not work yet, does not copy all specified files to local AppDir in the same way as before ...
sudo wget https://github.com/AppImageCrafters/appimage-builder/releases/download/v1.0.3/appimage-builder-1.0.3-x86_64.AppImage -O /usr/local/bin/appimage-builder
sudo chmod +x /usr/local/bin/appimage-builder
# install docker possibly
(dpkg -l | grep -q docker) || sudo apt-get -y install docker.io
# give the current user access to docker socket (requires log out+in to activate)
groups | grep -q docker || sudo adduser $(id -un) docker
# fix docker error about missing snd device:
[ -e /dev/snd ] || sudo ln /dev/null /dev/snd
# pull docker img now, prevents errors later
#docker pull appimagecrafters/tests-env:ubuntu-bionic
#docker pull appimagecrafters/tests-env:centos-7
for img in $(awk '/^[^#]*appimagecrafters/{print $2}' "$scriptdir/../AppImageBuilder.template.yml");
do
    docker pull "$img";
done
