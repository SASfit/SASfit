#!/bin/bash

TEXFILE=sasfit.tex
BBLFILE=sasfit.bbl
INTERMEDIATEFILE=latex2wiki.tex

IFS=$'\n'

cat_tex_recursive () {
	TEXFILE=$1
	if [ ! -f "$TEXFILE" ]; then
		TEXFILE=$TEXFILE.tex
		[ -f "$TEXFILE" ] || return
	fi;
	for line in $(cat $TEXFILE); do
		if echo $line | grep -nqF '\input';
		then
			FN=$(echo $line | awk -F'input{' '{print $2}' | awk -F'}' '{print $1}')
			cat_tex_recursive $FN
		else
			echo $line
		fi;
	done;
}

cat_tex_recursive $TEXFILE | awk -F'\' '

# we support _one-line_ /def and /newcommand statements only !
BEGIN {
	debug = 0
	# init the array with a list of commands we want to replace
	command[0]="SASfit;BAL;EAL;BE;EE;BD;ED;BEA;EEA;BDA;EDA;T;M;S;U;V;DS;BM;abs"
	split(command[0], allowed_commands, ";")

	# custom&manual replacements
	command["oldsqrt"] = "\\sqrt"
	command["B"] = "{\\underline{#1}}"
	command["nonumber"] = ""
}
# ignore commented latex code
/^%/ { next }
#/(\\\<(re)?newcommand\>|\\\<def\>)/ {
#	print
#}
# \newcommand starts at the beginning of a line in my TeX code
/^\\\<(re)?newcommand\>/ {
	# get the command name, search its end
	found = match($3,/^.+[ }]/)
	if (!found) next
	cmd_name = substr($3, RSTART, RLENGTH-1)
	# set the rest of the line for later processing
	tail = substr($0, length($1)+1+length($2)+1+RSTART+RLENGTH)

	# get the parameter count
	found = match($3,/\[[0-9]\]/)
	param_count = 0
	if (found) { param_count = substr($3, RSTART+1, RLENGTH-2) }
	# we dont support multiple parameters yet
	if (param_count > 1) next
	
	# get the command sequence for the current command name
	found = match(tail, /{.+}/)
	if (!found) next
	cmd_value = substr(tail, RSTART+1, RLENGTH-2)

	# add the retrieved data to the global bucket
	addcmd(cmd_name, cmd_value)
#	print "  |"cmd_name"|  |"cmd_value"|"
}
# there are multiple \def on a line in my TeX code
/\\\<def\>/ {
	split($0,a,/\\\<def\>/)
	for (num in a) {
		# get the command name
		found = match(a[num],/^\\[[:alnum:]]+[[:space:]]*[{#]/)
		if (!found) continue
		cmd_name = substr(a[num], RSTART+1, RLENGTH-2)
		gsub(/[[:space:]]/,"",cmd_name)

		# get the command sequence
		cmd_value = substr(a[num], RSTART+RLENGTH-1)
		# howto test for balanced parenthesis ? {[^}{]+} ?
		found = match(cmd_value, /{.+}/)
		if (!found) continue # unbalanced !
		cmd_value = substr(cmd_value, RSTART+1, RLENGTH-2)

		# add the retrieved data to the global bucket
		addcmd(cmd_name, cmd_value)
#		print "  |"cmd_name"|  |"cmd_value"|"
	}
}
! /(\\\<(re)?newcommand\>|\\\<def\>)/ {
#	print ":: "$0
	for (k in command) {
		# replace all ocurrences of the current command
		while(replace_cmd($0, k)) {}
	}
	print $0
}
function cmd2regex(cmd) { return "\\\\\\<"k"\\>" }
function addcmd(name, value) {
	if (!is_allowed(name)) return
	command[name] = value
}
function is_allowed(name) {
	allowed = 0
	for (i in allowed_commands) {
		if (name == allowed_commands[i]) {
			allowed = 1
			break
		}
	}
	return allowed
}
function replace_cmd(str, k) {
	if (k == "0") return 0
	idx = match(str, cmd2regex(k)"({([^}{]+)})?", a)
	if (idx == 0) return 0
#	printf("%d: ", idx);
#	for (idx in a) { printf("%s: %s ",idx,a[idx]) }
	head = substr(str, 0, a[0, "start"]-1)
	tail = substr(str, a[0, "start"]+a[0, "length"])
	mid = command[k]
	gsub(/#1/," "a[2],mid)
	$0 = sprintf("%s%s%s",head,mid,tail)
	return 1
}
END {
	if (!debug) exit 0
	for (k in command) {
		print k": "command[k]
	}
	for (k in command) printf("%s ", k)
	print "\n"
}' > $INTERMEDIATEFILE

cat $INTERMEDIATEFILE $BBLFILE | perl latex2wiki.pl -

