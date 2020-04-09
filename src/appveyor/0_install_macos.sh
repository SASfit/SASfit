#!/bin/sh

echo
echo "## Installing build tools ..."
echo

# env | sort

set -x

# update Homebrew package DB and upgrade all packages
brew update; brew upgrade; brew update; brew upgrade
brew cleanup
# already installed: git, cmake
brew install gcc
brew cask install xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
