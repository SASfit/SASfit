#!/bin/bash

if [ "$#" -lt "3" ]; then
	echo "This script adds a copyright notice to a source code file."
	echo "Supported languages: C, Tcl"
	echo "Usage: $0 <copyright notice file> <language> <directory of source files>"
	exit 0;
fi;

NOTICE_FILE="$1"
NOTICE_TEXT=""
LANG="$(echo $2 | tr A-Z a-z)"
TARGET="$3"
PREF=""
SUFF=""

case "$LANG" in
	"c"|"h")	COMMENT=" *"
			PREF="/*"
			SUFF=" */"
			;;
	"tcl"|"cmake")	COMMENT="#";;
	*)		COMMENT="";;
esac

if [ -z "$COMMENT" ]; then
	echo "Language '$LANG' not supported !"
	exit 0;
fi;

NOTICE_TEXT=$(awk -v comm="$COMMENT" -v pref="$PREF" -v suff="$SUFF" '
	BEGIN {
		empty=0; 
		if (pref) print pref
	} 
	/^$/ {empty=(empty+1)} 
	/^.+$/ {
		if (empty==1) {print comm} 
		if (empty==2) {
			if(length(suff)==0 && length(pref)==0) {print ""} 
			else {print suff"\n\n"pref}
		} 
		print comm" "$0; empty=0
	} 
	END {
		if (suff) {print suff}
	}' $NOTICE_FILE)

#echo "${NOTICE_TEXT//<FILENAME\/>/$TARGET}"

TEMPFILE="temp_source_code_file.tmp"
IFS=$'\n'

FILES=""
if [ -d "$TARGET" ]; then
	FILES="$(find $TARGET -type f -name "*.$LANG")";
else
	FILES="$*"
fi;

for fname in $FILES; do 
	if [ "$fname" == "$0" -o "$fname" == "$1" -o "$fname" == "$2" ];then continue; fi;
	echo -n "processing $fname ... "
	(echo "${NOTICE_TEXT//<FILENAME\/>/$fname}"; echo; cat $fname) > $TEMPFILE;
	flip -mb $TEMPFILE;
	mv $TEMPFILE $fname;
	echo "done."
done

exit 1;
