# sasfit.vfs/lib/app-sasfit/tcl/sasfit_init.tcl
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
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
#   Ingo Bressler (ingo@cs.tu-berlin.de)

package provide app-sasfit 1.0
package require sasfit

# initialize sasfit

package require Tk
package require math
package require inifile

# check if we can find BLT easily otherwise 
# call a blt enabled executable directly
if { [catch {package require BLT}] } {
	if { $argc > 0 } {
		lappend auto_path [lindex $argv 0]
	}
}

proc _launchBrowser url {

      switch $::tcl_platform(os) {
        Darwin {
          set command [list open $url]
        }
        HP-UX -
        Linux  -
        SunOS {
          foreach executable {firefox mozilla netscape
                       chromium-browser chrome google-chrome
                       iexplorer opera lynx w3m links epiphany galeon konqueror
                       mosaic amaya browsex elinks} {
            set executable [auto_execok $executable]
            if [string length $executable] {
              # Do you want to mess with -remote? How about other browsers?
              set command [list $executable $url &]
              break
            }
          }
        }
        {Windows 95} -
        {Windows NT} {
	    set command "[auto_execok start] {} [list $url]"
        }
      }
      if [info exists command] {
        # Replace  eval exec "$command" by exec {*}$command if you want >= tcl 8.5 compatibility ([RA])
        if [catch {eval exec "$command"} err] {
          tk_messageBox -icon error -message "error '$err' with '$command'"
        }
      } else {
        tk_messageBox -icon error -message \
          "Please tell CL that ($::tcl_platform(os), $::tcl_platform(platform)) is not yet ready for browsing."
      }
}

set sasfit(webbrowser) _launchBrowser

set gdef_prefix		"groups"
set gdef_suffix		".def"
set sasfit(tcl)         [file dirname [info script]]
set sasfit(plugins)	[list [file normalize [file join $sasfit(tcl) .. plugins]]]

# set the base directory (where directories 'doc','data','plugins' are)
# when enabling multi-user this has to be rethought
if { ! [info exists ::sasfit_basedir]} {
	set ::sasfit_basedir [file normalize [file join $sasfit(tcl) .. .. .. .. ]]
}
set sasfit(basedir)	$::sasfit_basedir

if {[file isdirectory $::sasfit(basedir)]} {
	lappend sasfit(plugins) [file normalize [file join $::sasfit(basedir) plugins]]
}
set sasfit(group_file) {}
foreach plugins_path $sasfit(plugins) {
	foreach gfile [glob -nocomplain -directory $plugins_path $gdef_prefix*$gdef_suffix] {
		if {[file exists $gfile]} {
			lappend sasfit(group_file) $gfile
		}
	}
}

set sasfit(group_defs)	{}
set sasfit(datadir)	"[file normalize [file join $::sasfit(basedir) data]]"
set sasfit(homedir)	"[file normalize [file nativename ~/]]"
set sasfit(docdir)	"[file normalize [file join $::sasfit(basedir) doc]]"
set sasfit(cfgfile)	"[file normalize [file join $::sasfit(basedir) config.ini]]"

if [file exists $sasfit(tcl)/sasfit_svn_rev.tcl] {
	source $sasfit(tcl)/sasfit_svn_rev.tcl
}
set sasfit(no_update_entries) [list no_update_entries no_update_entries_graph \
webbrowser tcl plugins basedir homedir datadir docdir group_file group_defs \
filename cfgfile hide_about_win]
set sasfit(no_update_entries_graph) [list "fit,background"]

source $sasfit(tcl)/sasfit_main.tcl

