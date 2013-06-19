#!/bin/bash

FILE=$1;
[ -f "$FILE" ] || exit 1;

function removePrefix()
{
	[ -n "$1" ] || return;
	fctname="$1";
	if [ "${fctname:0:2}" == "S_" ]; then fctname="${fctname:2}"; fi;
	if [ "${fctname:0:3}" == "SQ_" ]; then fctname="${fctname:3}"; fi;
	echo "$fctname";
}

IFS=$'\n'; 

for line in $( cat "$FILE" | 
		grep -vE '(^ ?\/\*|^ ?\/\/|^ ?\*)' | 
		grep -v '/ d' | 
		grep -A 1 strcmp); 
do
	if [ "$line" == "--" ]; 
	then 
		continue; 
	fi; 
	
	echo $line | awk -F'\(' '{
						gsub(/\) \{/,")",$0); 
						gsub(/FF_typestr,/,"FF->typestr, ",$0);
						if ( match($0,"return") ) 
						{
							str = $2;
							if ( NF == 2 )
							{
								num = split($1,arr,/return /);
								str = arr[2];
							}
							gsub("P_","",str);
							gsub("I_","",str);
							print "{\n\tFF->fct = sasfit_ff_"str";\n} else" 
						} else { 
							print substr($0,4) 
						} 
					}' ; 
done

