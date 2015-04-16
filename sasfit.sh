#!/bin/bash
# http://stackoverflow.com/a/630387
PATH="`dirname \"$0\"`"
OS=$(/usr/bin/uname -s)
if [ "$OS" == "Darwin" ]; then
  DYLD_LIBRARY_PATH="$PATH:$DYLD_LIBRARY_PATH" "$PATH"/sasfit
else
  LD_LIBRARY_PATH="$PATH:$LD_LIBRARY_PATH" "$PATH"/sasfit
fi;
