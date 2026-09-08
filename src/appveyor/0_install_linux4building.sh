#!/bin/sh

set -e

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
for fn in $(find /etc/apt -type f -path '*source*' ); do echo "## $fn:"; grep -v '^\(\([# ]\+\)\|$\)' $fn; done || true
echo
echo "## Update the package database"
echo
sudo apt-get update
# show source of a package
# apt-cache showpkg gcc-13
echo
echo "## Install extra development packages"
echo
set -x
sudo apt-get -y install build-essential cmake zlib1g-dev libgsl-dev libfftw3-dev libx11-dev libboost-all-dev libomp-dev
set +x

echo
echo "## Install a newer GCC than jammy's default (gcc-11), best-effort"
echo
# Ubuntu 22.04's own repos only carry gcc-9..gcc-12, so this project's
# CMake config -- and the exact compiler warnings/behaviour it was last
# checked against -- assumed gcc-14 from either an image that already
# bundles it, or (as attempted here) the ubuntu-toolchain-r/test PPA. This
# whole block is deliberately non-fatal: if the PPA can't be reached or
# gcc-14 isn't packaged for this release after all, the build continues
# with whatever gcc is already available (4_build.sh picks the newest
# installed gcc-N automatically, it does not hardcode this version).
(
    set -x
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    sudo apt-get update && \
    sudo apt-get -y install gcc-14 g++-14
) || echo "Could not install gcc-14/g++-14, continuing with whatever gcc is already available."
