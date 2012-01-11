#!/bin/bash
#
# pyuic.sh
# 
# Copyright (c) 2010-2011 BAM
# Federal Institute for Materials Research and Testing
# Division I.3 Structure Analysis; Polymer Analysis
# Richard-Willstaetter-Strasse 11, 12489 Berlin, Germany
# 
# This file is part of cutesnake.
#
# cutesnake is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cutesnake is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cutesnake.  If not, see <http://www.gnu.org/licenses/>.
# 
# Author(s) of this file
# Ingo Bressler (cutesnake@ingobressler.net)

UIC="pyuic4 $1"
RCC="pyrcc4"
TMPFN=/tmp/"$((date +%s; echo $$) | md5sum | awk '{print $1}')"

filter() {
        # remove timestamps to avoid VCS changes on regenerate
	INFILE="$1"
        sed -r -e '/^# Created: /d' -e 's/^#\s+by: /# Created by: /g' -e 's/PyQt4 UI code generator ([0-9]\.)+[0-9]/PyQt4 UI code generator/g' -e 's/The Resource Compiler for PyQt \(Qt v(([0-9]\.)+[0-9])\)/The Resource Compiler for PyQt/g' "$INFILE" > "$TMPFN"
	mv "$TMPFN" "$INFILE"
}

genUI() {
	INFILE="$1"
	OUTFILE="$2"
	$UIC "$INFILE" > "$OUTFILE"
	filter "$OUTFILE"
}

genRCC() {
	INFILE="$1"
	OUTFILE="$2"
	$RCC "$INFILE" > "$OUTFILE"
	filter "$OUTFILE"
}

for uifile in *.ui; do
        [ -f "$uifile" ] || break
	OUTFILE="${uifile:0:${#uifile}-3}.py"
	genUI "$uifile" "$OUTFILE"
done
for resfile in *.qrc; do
        [ -f "$resfile" ] || break
	OUTFILE="${resfile:0:${#resfile}-4}_rc.py"
	genRCC "$resfile" "$OUTFILE"
done

# vim: set ts=8 sw=8 tw=0:
