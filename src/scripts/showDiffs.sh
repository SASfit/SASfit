#!/bin/bash

if [ "$#" -lt "1" ];
then
	echo "USAGE: $0 <directory to compare the current one to>"
	exit 0;
fi;

CURBASE=$(pwd)
OTHERBASE=$CURBASE/$1

if ! [ -d "$OTHERBASE" ];
then
	echo "Provided directory '$OTHERBASE' does not exist."
	exit 1;
fi;

echo -n "enter filename to compare: "
read filename
diff -u "$CURBASE/$filename" "$OTHERBASE/$filename"

echo    "------------"
echo -n "Copy? [Y/n] "
read answer
if ! [ "$answer" == "n" ];
then
	cp "$OTHERBASE/$filename" "$CURBASE/$filename"
fi;

exit 0;
