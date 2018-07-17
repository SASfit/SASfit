# sasfit.vfs/lib/app-sasfit/tcl/sasfit_Webtk.tcl
#
# Copyright (c) 2008-2018, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)

#!/bin/sh
# webtk - HTML editor
#
# Copyright (c) 1996 Sun Microsystems, Inc.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# Start a the HTML editor/browser
#
# This is the main script for webtk when used on Macintosh and Windows platforms
#

if {$tk_version < 4.1} {
    error "WebEdit requires Tk 4.1, this is only $tk_version"
}
if [catch {file join x y}] {
    error "WebTk requires the non-beta version of Tcl 7.5/Tk 4.1"
}


#CONFIGURATION
set WebTk(version) {1.0beta5 12/4/96}
set maintainer Brent.Welch@eng.sun.com
set WebTk(proxy) {}
set WebTk(port) {}
#END CONFIGURATION

# Bootstrap our location from the name of the script.

if {$tcl_platform(platform) == "macintosh"} {
    # On the mac, the bundled executable is one level above the
    # library and html subdirectories.  This script is included in
    # the binary as the "webtk.tcl" resource.
    set WebTk(home) [file join [pwd] [file dirname [info nameofexecutable]]]
} else {
    # On windows, the webtk.tcl script is in the bin subdirectory
    set WebTk(home) [file join [pwd] [file dirname [file dirname [info script]]]]
}
set WebTk(home) $sasfit(tcl)/WEBTK/v1/
set WebTk(library) [file join $WebTk(home) lib]
set WebTk(html) [file join $WebTk(home) html]
set WebTk(bin) [file join $WebTk(home) bin]
set WebTk(images) [file join $WebTk(home) images]
set WebTk(userfiles) [file join $WebTk(home) user]
set WebTk(cache) [file join $WebTk(home) cache]

lappend auto_path $WebTk(library)

# WebTk is defined in main.tcl



