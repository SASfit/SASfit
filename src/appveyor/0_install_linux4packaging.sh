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
# https://appimage-builder.readthedocs.io/en/latest/intro/install.html#debian-ubuntu
sudo apt-get -y install binutils coreutils pipx python3-pip python3-venv python3-setuptools python3-packaging patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace fuse appstream squashfs-tools util-linux zsync wget
# install docker possibly
(dpkg -l | grep -q docker) || sudo apt-get -y install docker.io
pipx install 'appimage-builder<1.1'
echo "local bin: '$HOME/.local/bin'"
ls -la $HOME/.local/bin
sudo wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /usr/local/bin/appimagetool
sudo chmod +x /usr/local/bin/appimagetool
# fix docker error about missing snd device:
sudo ln /dev/null /dev/snd
# pull docker img now, prevents errors later
#docker pull appimagecrafters/tests-env:ubuntu-bionic
#docker pull appimagecrafters/tests-env:centos-7
for img in $(awk '/^[^#]*appimagecrafters/{print $2}' "$scriptdir/../AppImageBuilder.template.yml");
do
    docker pull "$img";
done

# vim: set ts=4 sw=4 sts=4 tw=0 et:
