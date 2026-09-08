#!/bin/sh

echo
echo "## Installing build tools on:"
echo
sw_vers
echo

# env | sort

set -x

export HOMEBREW_NO_INSTALL_CLEANUP=1
# Deliberately NOT calling a bare `brew update` here: on this AppVeyor macOS
# image, `brew update` alone (no other brew command involved) triggers
# AppVeyor's own baked-in reconciliation of its pre-provisioned tools
# (PowerShell, dotnet, openssl@3, ...) against a curated Brewfile -- a
# known AppVeyor macOS quirk, unrelated to anything this project installs.
# That reconciliation can itself hit a bottle-less formula and fall back to
# compiling from source (this time openssl@3, previously it was gcc
# itself), stalling the whole job long before it ever reaches this
# script's own installs. HOMEBREW_NO_AUTO_UPDATE=1 additionally prevents
# the brew install/link calls below from implicitly triggering the same
# thing on their own.
export HOMEBREW_NO_AUTO_UPDATE=1
# Pinned to gcc@15 rather than the rolling 'gcc' formula: as of this writing
# the unversioned 'gcc' formula resolves to 16.2.0, whose patch definition
# has a known Homebrew bug ("Patch file must be within the formula
# repository" / unverifiable checksum on the 16.2.0 diff -- see
# https://github.com/orgs/Homebrew/discussions/6910). That causes the
# bottle to fail and Homebrew to fall back to compiling all of GCC from
# source (45-90+ minutes), which is what was timing out/stalling this build
# before ever reaching SASfit's own sources. gcc@15 has a normal published
# bottle for sonoma (both Apple Silicon and Intel) and needs no changes to
# 4_build.sh's compiler discovery, since that already finds any version-
# suffixed gcc-N/g++-N binary generically -- which is the only kind Homebrew
# ever installs for its gcc formulas anyway (rolling or pinned alike).
brew install --overwrite cmake gcc@15 gsl fftw zlib boost libomp  # GCC for xquartz libstc++ compat.
brew link --force libomp
brew install --cask xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
