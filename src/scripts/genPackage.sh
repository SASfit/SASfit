#!/bin/bash
# this script generates a sasfit package for distribution out of a supplied file
# containing all the (package-)relative path names to the required files
# it creates a new directory in the current one (where this script is executed) where
# the required files are copied to
# first parameter is the current work dir to copy the files from
# (absolute path names required, because of possible symlinks)
# second parameter is the file containing a list of all files to include

echo "### BE SURE THAT THE _LINUX_ BINARIES ARE 32 BIT !!! ###"

CURDIR=$1
[ -d "$CURDIR" ] || exit 1;
LISTFILE=$2
[ -f "$LISTFILE" ] || exit 1;

echo "current work dir: $CURDIR"
echo "current list file used: $LISTFILE"

NEWDIRNAME="sasfit_$(date +%Y-%m-%d_%H-%M-%S)"
NEWDIR="$(pwd)/$NEWDIRNAME"
echo "new package dir: $NEWDIR"

mkdir -p "$NEWDIR";

OLDDIR="$(pwd)"
cd "$CURDIR"

for filename in $(cat "$LISTFILE");
do
	if [ ! -f "$filename" ];
	then
		echo "Could not copy: $filename";
	else
		cp --parents "$filename" "$NEWDIR/";
	fi;
done;

cd "$OLDDIR";
pwd

zip -r "$NEWDIRNAME" "$NEWDIRNAME"

exit 0;

