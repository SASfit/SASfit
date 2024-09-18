#!/bin/sh

echo
echo "## Installing build tools on:"
echo
lsb_release -d -c
echo
echo "## Cleaning up unused package sources first"
echo
set -x
sudo ls -la /etc/apt/sources.list.d/
grep -q 'bintray.com' /etc/apt/sources.list && \
    sudo sed -i -e '/bintray.com/d' /etc/apt/sources.list
set +x
echo
echo "## Show all apt sources installed"
echo
set -x
for fn in $(find /etc/apt -type f -path '*source*' ); do echo "## $fn:"; grep -v '^\(\([# ]\+\)\|$\)' $fn; done
set +x
echo
echo "## Update the package database"
echo
set -x
sudo apt-get update
# show source of a package
apt-cache showpkg gcc-13
set +x
echo
echo "## Install extra development packages"
echo
set -x
sudo apt-get -y install build-essential cmake zlib1g-dev libgsl-dev libfftw3-dev libx11-dev #gcc-14 g++-14 # for testing
