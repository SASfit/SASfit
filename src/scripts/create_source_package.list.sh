#!/bin/bash
# This script creates a file list for a sasfit source distribution.
# This list is based on the _svn_ repository which allows to not synchronize
# the file list with changes in the svn repository
#
# The subversion command line client has to be in PATH (see below)
#
# Call this script from the command line in this way:
# src/scripts/create_source_package.list.sh

LIST_FILE=src/scripts/create_source_package.list
SVN=svn

# space separated list of directories
TARGETS="
README_source.txt
src/Doxyfile 
src/doc/images
src/doc/groups_def.h
src/doc/mainpage.h
src/CopySharedLibs.cmake 
src/FindGSL.cmake 
src/FindF2C.cmake 
src/Findsasfit-common.cmake 
src/Findsasfit-sd.cmake 
src/Findsasfit-peaks.cmake 
src/Findsasfit-sq.cmake 
src/Findsasfit-ff.cmake 
src/Findsasfit-oz.cmake 
src/f2c 
src/sasfit-common 
src/sasfit-sd 
src/sasfit-sq 
src/sasfit-peaks 
src/sasfit-ff
src/sasfit-oz
src/sasfit-core"

if [ ! -f $LIST_FILE ];then
	echo "List file '$LIST_FILE' doesn't exist !"
	exit 1;
fi;

rm -f $LIST_FILE

for TARGET in $TARGETS; do
	if [ ! -e $TARGET ]; then
		echo "The file/directory '$TARGET' doesn't exist in current path !"
		continue;
	fi;
	if [ -f "$TARGET" ]; then
		echo "$TARGET" >> $LIST_FILE
	else
		for FILE in $($SVN list -R $TARGET); do
			if [ -f "$TARGET/$FILE" ]; then
				echo "$TARGET/$FILE" >> $LIST_FILE
			fi;
		done;
	fi;
done

exit 0;

