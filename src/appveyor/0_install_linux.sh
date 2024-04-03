#!/bin/sh

echo
echo "## Installing build tools on:"
echo
lsb_release -d -c
echo

# env | sort

set -x

# install ssh deploy key
echo "${SF_DEPLOY_KEY}" | tr " " "\n" >> $HOME/.ssh/id_rsa
# cleaning up unused package sources first
sudo rm -f /etc/apt/sources.list.d/*.*
sudo ls -la /etc/apt/sources.list.d/
sudo sed -i -e '/bintray.com/d' /etc/apt/sources.list
sudo apt-get update
# Install AppImage-Builder and requirements
# https://appimage-builder.readthedocs.io/en/latest/intro/install.html#debian-ubuntu
sudo apt-get -y install binutils coreutils python3-pip python3-venv python3-setuptools python3-packaging patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace fuse appstream squashfs-tools util-linux zsync libgsl-dev libfftw3-dev
sudo python3 -m pip install 'appimage-builder<1.1'
sudo wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /usr/local/bin/appimagetool
sudo chmod +x /usr/local/bin/appimagetool
# fix docker error about missing snd device:
sudo ln /dev/null /dev/snd
# pull docker img now, prevents errors later
#docker pull appimagecrafters/tests-env:ubuntu-bionic
#docker pull appimagecrafters/tests-env:centos-7
for img in $(awk '/^[^#]*appimagecrafters/{print $2}' src/AppImageBuilder.template.yml);
do
    docker pull "$img";
done

# vim: set ts=4 sw=4 sts=4 tw=0 et:
