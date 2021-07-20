#!/bin/sh

# Returns the latest version sorted command found in PATH, e.g. latest gcc, g++
# example for finding latest g++: './findCmdInPath.sh '/g\+\+(-(mp-)?[0-9]+)?$' 2> /dev/null'
latestInPATH()
{
    local pattern; pattern="$1"
    echo "Searching for pattern '$pattern':" >&2
#    pattern="$(echo "$PATH" \
#        | tr ':' '\n' | xargs -n 1 ls -1 2> /dev/null \
#	| egrep "$pattern" | sort -uV)"
    local sortedLst; sortedLst="$(echo "$PATH" \
        | eval find "$(awk 'BEGIN{RS=":";ORS=" "} {sub("\n$",""); print "\""$0"\""}')" 2> /dev/null \
        | egrep "$pattern" | sort -uV)"
    local selected; selected="$(printf "%s\n" "$sortedLst" | tail -n1)"
    printf "%s\n" "$sortedLst" | sed 's#\('$selected'\)\(\s\)*$#\1 (selected)\2#' >&2
    echo $selected
}

latestInPATH "$1"

# vim: set ts=4 sw=4 sts=4 tw=0 et:
