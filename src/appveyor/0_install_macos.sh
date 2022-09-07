#!/bin/sh

echo
echo "## Installing build tools ..."
echo

# env | sort

set -x

# update Homebrew package DB and upgrade all packages
#brew update; brew upgrade; brew update; brew upgrade
#brew cleanup
# FIXME: remove this when fixed at appveyor.com
#export HOMEBREW_NO_AUTO_UPDATE=1
# due to Bintray being out of service, homebrew migrated all packages to github
# -> requiring explicit update: https://github.com/Homebrew/discussions/discussions/691
brew update
# already installed: git, cmake
brew install gcc # for xquartz libstc++ compat.
brew install --cask xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
