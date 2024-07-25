#!/bin/sh

echo
echo "## Installing build tools on:"
echo
lsb_release -d -c
echo

# env | sort

set -x

# cleaning up unused package sources first
sudo ls -la /etc/apt/sources.list.d/
grep -q 'bintray.com' /etc/apt/sources.list && \
    sudo sed -i -e '/bintray.com/d' /etc/apt/sources.list
sudo apt-get update
# extra compiler
sudo apt-get -y install build-essential cmake zlib1g-dev libgsl-dev libfftw3-dev libx11-dev #gcc-14 g++-14 # for testing

