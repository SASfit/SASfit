#!/bin/bash
# start SASfit and point it to the shipped dynamic libraries
SCRIPTDIR="`dirname \"$0\"`"
# fix possible file permission problems
chmod -R u+w,g+w "$SCRIPTDIR"
LIBDIR="$SCRIPTDIR/lib"
UNAME="/bin/uname"
[ -f "$UNAME" ] || UNAME="/usr/bin/uname"
OS="$($UNAME -s)"
if [ "$OS" == "Darwin" ]; then
  # try to remove quarantine flag from package binaries
  xattr -rc "$SCRIPTDIR"
  # start SASfit with adjusted library search path
  DYLD_LIBRARY_PATH="$LIBDIR:$DYLD_LIBRARY_PATH" "$SCRIPTDIR"/sasfit
else
  # start SASfit with adjusted library search path
  # not used for releases, replaced by AppImage
  LD_LIBRARY_PATH="$LIBDIR" "$SCRIPTDIR"/sasfit
fi;
