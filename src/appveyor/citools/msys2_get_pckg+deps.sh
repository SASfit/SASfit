#!/bin/sh
#
# MIT License
#
# Copyright (c) 2025 Ingo Breßler (dev <at> ingobressler.net)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software.

# For the MSYS2 environments on Windows, this script retrieves the given package
# (first cmdline arg) and all its dependencies, based on their package file
# timestamp. Works only if the package is still in the package repository.
#
# This targets specific (older) versions of packages (compilers like GCC, for example)
# which are superseeded by newer versions by default and which would have to be
# installed manually, including with all their (older) dependencies.
# Example:
#    sh msys2_get_pckg+deps.sh gcc-13
#    sh msys2_get_pckg+deps.sh cmake-3.29.2
#    pacman -U *.zst

set -e

desiredPckg="$1"
if [ -z "$desiredPckg" ]; then
	echo "No install candidate package given!"
	exit 1
fi
desiredPckg="${MINGW_PACKAGE_PREFIX}-$desiredPckg"
echo "Finding dependencies for '$desiredPckg'."

# get the exiting packages
installedfn="$(mktemp)"
pacman -Q > "$installedfn"
if grep -qF "^${desiredPckg}" "$installedfn"; then
	echo "Already installed: $desiredPckg!"
	exit 0
fi
rm -f "$installedfn"

CURL="curl -# -C -"
# get current package download links
baseurl="https://repo.msys2.org/mingw$MINGW_PREFIX"
packageidx=index.html
if [ ! -f "$packageidx" ]; then
	#packageidx="$(mktemp)"
	$CURL -o"$packageidx" -L "$baseurl"
fi
echo "Using package index at '$packageidx'."

get_package_by_date()
{
	local pckg="$1"
	[ -z "$pckg" ] && return 1
	local tsLatest="-1"
	[ -z "$2" ] || tsLatest="$2"
	local candidates="$(grep "${pckg}" "$packageidx" | \
		awk -v tsLatest=$tsLatest '/\.zst"/{
		ORS="\n";
		split($2, href, "\"");
		split($3, d, "-");
		split($4, t, ":");
		month = (d[2]=="Jan"?1:
			(d[2]=="Feb"?2:
			(d[2]=="Mar"?3:
			(d[2]=="Apr"?4:
			(d[2]=="May"?5:
			(d[2]=="Jun"?6:
			(d[2]=="Jul"?7:
			(d[2]=="Aug"?8:
			(d[2]=="Sep"?9:
			(d[2]=="Oct"?10:
			(d[2]=="Nov"?11:12)))))))))));
		ts = mktime(sprintf("%d %02d %02d %02d %02d 00", d[3], month, d[1], t[1], t[2]));
		if (tsLatest < 0 || ts <= tsLatest || true) print ts" "href[2]
	}' | sort -n)"
	echo "  Candidates up to $tsLatest:" 1>&2
	echo "$candidates" | awk '{print "    "$0}' 1>&2
	echo "$candidates" | tail -n1
}

queuefn=queue.txt #"$(mktemp)" # FIXME
[ -f "$queuefn" ] || touch "$queuefn"
queueAdd() {
    # append to file only if it doesn't exist yet
	grep -qF "$1" "$queuefn" || echo $1 >> "$queuefn"
}
# Remove and return the first line in the file
queuePop() {
	item="$(head -n1 "$queuefn")"
	sed -i '1d' "$queuefn"
	echo "$item"
}
queueIsEmpty() {
	[ -z "$(cat "$queuefn")" ]
}
getDeps() {  # process the first file an add its required packages
	fn="$(queuePop)"
	echo "-> Downloading: $fn"
	# Download the latest package up to timestamp
	$CURL -O "$baseurl/$fn"
	# get its dependencies
	for pckg in $(pacman -Qip $fn | awk -F':' '/Depends On/{print $2}');
	do
		[ "$pckg" = None ] && continue
		pckg="$(echo $pckg | awk -F'=' '{print $1}')"
		ls ${pckg}-* > /dev/null 2>&1 && continue  # file downloaded already
		echo "# Getting $pckg:"
		fn="$(get_package_by_date ${pckg}- $ts | cut -d' ' -f2)"
		echo "-> Adding $fn"
		queueAdd "$fn"
	done
}
# get root package
fn="$(get_package_by_date $desiredPckg)"
ts="$(echo $fn | cut -d' ' -f1)"
fn="$(echo $fn | cut -d' ' -f2)"
queueAdd "$fn"
while ! queueIsEmpty; do
	echo "Queue:"
	awk '{print "    "$0}' "$queuefn"
	getDeps
done

#pacman -U *.zst

# cleanup
rm -f "$queuefn"
#rm -f "$packageidx"
