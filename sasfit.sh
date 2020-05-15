#!/bin/bash
# start SASfit and point it to the shipped dynamic libraries
SCRIPTDIR="`dirname \"$0\"`"
LIBDIR="$SCRIPTDIR/lib"
UNAME="/bin/uname"
[ -f "$UNAME" ] || UNAME="/usr/bin/uname"
OS="$($UNAME -s)"
if [ "$OS" == "Darwin" ]; then
  DYLD_LIBRARY_PATH="$LIBDIR:$DYLD_LIBRARY_PATH" "$SCRIPTDIR"/sasfit
else
  # use own library loader on Linux
  LD_LIBRARY_PATH="$LIBDIR" "$SCRIPTDIR"/sasfit
fi;

# vim: set ts=2 sts=2 sw=2 tw=0:
