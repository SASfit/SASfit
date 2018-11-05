#!/usr/bin/tclsh
#
# sasfit.vfs/main.tcl
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
#   Ingo Bressler (ingo@cs.tu-berlin.de)

if {[catch {package require starkit} result]
} {
	set thisdir [file dirname [info script]]
	lappend auto_path [file join $thisdir lib]
} {
	starkit::startup
}

package require tkcon
tkcon show
tkcon title "SASfit console"
tkcon buffer 8192
tkcon hide
set tkcon_visible 0

# package tcllib
package require struct::matrix
package require csv
package require control
# package tklib
package require Plotchart

package require -exact BWidget 1.2.1
# some strange early tcl package
# removed that package as it only used the balloonhelp command from it
# all other commands are already part of tklib
#package require widget

# set basedir to something else if available
# (useful if executable is not in same dir with data&doc&plugins dir)
if { $argc > 0 } {
	set ::sasfit_basedir [lindex $argv 0]
}

# final sasfit application
package require math::interpolate
package require math::statistics
package require math

package require app-sasfit

