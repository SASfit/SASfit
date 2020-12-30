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
export HOMEBREW_NO_AUTO_UPDATE=1
# already installed: git, cmake
brew install gcc
brew install --cask xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
