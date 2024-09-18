#!/bin/sh

echo
echo "## Installing build tools on:"
echo
lsb_release -d -c
echo

# env | sort

set -x

echo
echo "## Cleaning up unused package sources first"
echo
sudo ls -la /etc/apt/sources.list.d/
grep -q 'bintray.com' /etc/apt/sources.list && \
    sudo sed -i -e '/bintray.com/d' /etc/apt/sources.list

echo
echo "## Show all apt sources installed"
echo
find /etc/apt -type f -path '*source*' -exec grep -H -v '^\(\([# ]\+\)\|$\)' {} \;

echo
echo "## Update the package database"
echo
sudo apt-get update

echo
echo "## Install extra development packages"
echo
sudo apt-get -y install build-essential cmake zlib1g-dev libgsl-dev libfftw3-dev libx11-dev #gcc-14 g++-14 # for testing
