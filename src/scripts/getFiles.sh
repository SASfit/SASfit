#!/bin/bash
# script for extracting the source file names to build from M$ Visual Studio *.dsp files

#DSPFILE="../makefiles/sasfit.dsp";
#DSPFILE="../makefiles/sasfit_extrapol.dsp";
DSPFILE="$1";

for name in $(grep SOURCE $DSPFILE); 
do 
	file="${name##S*\\c\\}"; 	# strip 'SOURCE=....\c\'
	file=${file:0:${#file}-1};	# remove the trailing newline
	file=${file//\\/\/}; 		# replace \ by /
	file=${file//\"/}; 		# remove " left-overs
	if [ ! -f "$file" ]; then 
		echo "## does not exist: '$file'"; 
	else 
		echo "$file"; 
	fi; 
done | grep -v \.h$


exit 0;
