#!/bin/bash

FILE=$1;
[ -f "$FILE" ] || exit 1;

#MAIN_FCT="fSQ";
MAIN_FCT="F2";

function getLines()
{
	[ -n "$1" -a -n "$2" -a -n "$3" ] || return;
	filename="$1";
	startPos="$2";
	endPos="$3";
	head -n $endPos $filename | tail -n $(($endPos-$startPos+1))
}

function getCutStart()
{
	[ -n "$1" -a -n "$2" ] || return;
	filename="$1";
	linepos="$2";
	echo $(head -n $linepos $filename | grep -n ^} | tail -n 1 | awk -F':' '{print $1}'); 
}

function getCutEnd()
{
	[ -n "$1" -a -n "$2" ] || return;
	filename="$1";
	linepos="$2";
        #echo "start line: $2"
	numLines=$(cat $filename | wc -l );
	#echo "numlines: "$numLines" last few line: "$(($numLines-$2))
	tail -n $(($numLines-$linepos)) $filename | grep -E -n '^}' | head -n 1 | awk -F':' '{print $1}'
	#echo $(tail -n $(($numLines-$2)) $FILE | grep -E -n '^}' | head -n 1 | awk -F':' '{print $1}'); 
}

function getNames()
{
#	grep -n '(Tcl_Interp' $FILE | awk '{print $2}' | awk -F'(' '{print $1}' | grep -v fSQ | awk '{print substr($1,3)}'
	numLines=$(cat $FILE | wc -l );
	tail -n $(($numLines - $(grep -n "$MAIN_FCT(Tcl_Interp" $FILE | awk -F':' '{print $1}'))) $FILE \
	| grep '(interp' \
	| awk -F'\(interp' '{print $1}' \
	| awk '{numfields=split($0, fArr, /[^A-Za-z0-9_]/); print fArr[numfields]}' \
	| grep -v ^$ 
#	| awk '{numfields=split($0, fArr, /[^A-Za-z0-9_]/); gsub(/^d[A-Za-z]*_d[A-Za-z0-9]*_/,"",fArr[numfields]); print fArr[numfields]}' \
}

function removeRedundancy()
{
	[ -n "$1" ] || return;
	AllNames="";
	for fctname in $($1 $2);
	do
		found="false";
		if [ "$fctname" == "SASFITnrerror" ]; then continue; fi;
		#if [ "${fctname:0:3}" == "d_d" ]; then echo "### $fctname"; fi;
		for name in $AllNames;
		do
			if [ "$fctname" == "$name" ];
			then
				found="true";
			fi;
		done;
		if [ "$found" == "false" ]; 
		then
			echo $fctname;
			AllNames=$(echo -e "$AllNames\n$fctname");
		fi;
		#getNames $FILE | awk -v name=$fctname '{if( name != $0 ) {print $0" "match(name,".*"$0".*")" "RLENGTH}}'
	done;
}

function getFctNames()
{
	removeRedundancy "getNames";
}

function removePrefix()
{
	[ -n "$1" ] || return;
	fctname="$1";
	if [ "${fctname:0:2}" == "S_" ]; then fctname="${fctname:2}"; fi;
	if [ "${fctname:0:3}" == "SQ_" ]; then fctname="${fctname:3}"; fi;
	echo "$fctname";
#	| awk '{numfields=split($0, fArr, /[^A-Za-z0-9_]/); gsub(/^d[A-Za-z]*_d[A-Za-z0-9]*_/,"",fArr[numfields]); print fArr[numfields]}' \
}

function getFctBody()
{
	[ -n "$1" ] || return;
	fctname="$1";
	result="$(egrep -r -n "[_a-zA-Z0-9]+( $fctname( )?\(Tcl)" ../* | grep -v '.svn' | grep -v 'include' | grep -v '/SQ/')";
#echo "$result"
#return
	num=$(echo $result | awk -F':' '{print $2}');
	filename=$(echo $result | awk -F':' '{print $1}');
	if [ -n "$filename" ];
	then
#		echo "'$fctname'";
#		echo "'$filename'";
		fctname=$(removePrefix "$fctname");
		for incLine in $(grep '#include' "$filename");
		do
			echo "//$incLine";
		done;
		echo -e "\nscalar sasfit_FF_$fctname(scalar x, sasfit_param* param)\n{\n";
		startPos=$(getCutStart $filename $num);
		startPos=$(($startPos+1));
		if [ -z $startPos ]; then startPos=$num; fi;
		endPos=$(getCutEnd $filename $num);
		endPos=$(($num+$endPos));
#		echo "start: '$startPos', end: '$endPos'";
		headerDone="";
		getLines $filename $startPos $endPos | 
			awk '	BEGIN { header=1 }

				# remove leading empty lines
				/^\r*$/			{ if ( header ) next }

				# remove redundant ## comments
				/^\/\*#+\*\/\r*$/ 	{ next }

				# convert old style comments into doxygen/javadoc comments
				/^\/\*+\r*$/		{ print "/**"; next }
				/^ *\*+\/\r*$/		{ print " */"; next }

				# tests passed, print this line
				{ 
					header=0; 
					gsub(/[\t| ]+\*\r*$/,"",$0);	# remove trailing stars (*)
					gsub("   ", "\t", $0);		# rough fix of indents
					print $0 
				}
			';
#		for line in $(getLines $filename $startPos $endPos);
#		do
#			# dont output leading empty lines (is compatible with \n vs. \r\n newline)
#			! echo "$line" | egrep -q '^[[:space:]]*$' || [ -n "$headerDone" ] || continue;
#
#			# the header is done when the function definition passed by
#			echo "$line" | grep -q "$fctname" && [ -z "$headerDone" ] && headerDone="1";
#
#			! echo "$line" | egrep -q '^\/\*#+\*\/' || continue;
#
#			echo "$line";
#		done;
	else
		echo "not found";
	fi;
}

function getInnerCalls()
{
	[ -n "$1" ] || return;
	fctname="$1";
	getFctBody $fctname | 
		egrep -v '^[[:space:]]*\/' | 
		egrep '[a-zA-Z0-9]+\(' | 
		awk '{	num=split($0,A,/\(+[^\(]*[^a-zA-Z0-9_\(]+/); 
#			print "###: "$0; 
			for (i in A)
			{ 
				n=split(A[i], fctArr, /[^a-zA-Z0-9_]/); 
				if( match(fctArr[n], /[a-zA-Z0-9_]+/) ) print fctArr[n];
			}
		}';
}

function getInnerFctCalls()
{
	[ -n "$1" ] || return;
	#getInnerCalls $1;
	removeRedundancy "getInnerCalls" "$1";
}

IFS=$'\n'

for curName in $(getFctNames);
do
	# retreive the base filename
#	basename="$(echo "$curName" | awk '{ gsub(/^d[A-Za-z]*_d[A-Za-z0-9]*_/,"",$0); gsub(/^[A-Za-z]_/,"",$0); print $0}')";
	#echo "$curName -> $basename"
#	newFilename="sasfit_FF_$basename.c";
#	| awk '{numfields=split($0, fArr, /[^A-Za-z0-9_]/); gsub(/^d[A-Za-z]*_d[A-Za-z0-9]*_/,"",fArr[numfields]); print fArr[numfields]}' \
#	echo "    $newFilename" >> CMakeLists.txt;
	echo "sasfit_ff_DLLEXP scalar sasfit_FF_$curName(scalar x, sasfit_param* param);";
#	echo "file name: '$newFilename'";
#	getFctBody "$curName" >> "$newFilename";
#	getFctBody "$curName";
#	flip -b -m "$newFilename";
#	getInnerFctCalls "$curName";
#	echo "====================================================================="
done;

