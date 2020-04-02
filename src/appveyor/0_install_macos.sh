#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

set -x
echo "CMD: '$0' argv: '$@'"
uname -s
which cmake
cmake --version
which gcc
echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^gcc'

which brew
brew -h
brew update
brew cleanup
# already installed: git, cmake
#brew install cmake gcc
brew cask install xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
