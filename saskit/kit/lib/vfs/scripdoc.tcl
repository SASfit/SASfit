# Only useful for TclKit
# (this file is include in tclvfs so this entire package can be
# use in tclkit if desired).
#
# Scripted document support
#
# 2000/03/12 jcw 0.1 initial version
# 2000/09/30 jcw 0.2 added extendPath
# 2001/10/18 jcw 0.3 
# 2002/03/24 jcw 1.0 added listall and sync
#
# Copyright (C) 2000 Jean-Claude Wippler <jcw@equi4.com>

package require vfs
package provide scripdoc 1.0

namespace eval scripdoc {
  variable self   ;# the scripted document file
  variable script ;# the script which is started up

  namespace export init extendPath listall sync

  proc init {version driver args} {
    variable self
    variable script
    global errorInfo tk_library

    set self [info script]
    set root [file tail [file rootname $self]]

    if {$root == ""} {
      error "scripdoc::init can only be called from a script file"
    }

    if {[catch {
      if {$version != 1.0} {
	error "Unsupported scripdoc format (need $version, have 1.0)"
      }

      array set opts {m -nocommit}
      array set opts $args

      package require ${driver}vfs
      ::vfs::${driver}::Mount $self $self $opts(m)

      extendPath $self

      foreach name [list $root main help] {
	set script [file join $self bin $name.tcl]
	if {[file exists $script]} break
      }

      if {![file exists $script]} {
	error "don't know how to run $root for $self"
      }

      uplevel [list source $script]
    } msg]} {
      if {[info exists tk_library]} {
	wm withdraw .
	tk_messageBox -icon error -message $msg -title "Fatal error"
      } elseif {"[info commands eventlog][info procs eventlog]" != ""} {
	eventlog error $errorInfo
      } else {
	puts stderr $errorInfo
      }
      exit
    }
  }

# Extend auto_path with a set of directories, if they exist.
#
# The following paths may be added (but in the opposite order):
#   $base/lib
#   $base/lib/arch/$tcl_platform(machine)
#   $base/lib/arch/$tcl_platform(platform)
#   $base/lib/arch/$tcl_platform(os)
#   $base/lib/arch/$tcl_platform(os)/$tcl_platform(osVersion)
#
# The last two entries are actually expanded even further, splitting
# $tcl_platform(os) on spaces and $tcl_platform(osVersion) on ".".
#
# So on NT, "Windows" and "Windows/NT" would also be considered, and on
# Linux 2.2.14, all of the following: Linux/2, Linux/2/2, Linux/2/2/14
#
# Only paths for which the dir exist are added (once) to auto_path.

  proc extendPath {base {verbose 0}} {
    global auto_path
    upvar #0 tcl_platform pf

    set path [file join $base lib]
    if {[file isdirectory $path]} {
      set pos [lsearch $auto_path $path]
      if {$pos < 0} {
	set pos [llength $auto_path]
	lappend auto_path $path
      }
      
      if {$verbose} {
	set tmp [join [concat {{}} $auto_path] "\n    "]
	tclLog "scripDoc::extendPath $base -> auto_path is: $tmp"
      }

      foreach suffix [list $pf(machine) \
			   $pf(platform) \
			   [list $pf(os) $pf(osVersion)] \
			   [concat [split $pf(os) " "] \
				   [split $pf(osVersion) .]]] {

	set tmp [file join $path arch]
	foreach x $suffix {
	  set tmp [file join $tmp $x]
	  if {$verbose} {tclLog "  checking $tmp"}
	  if {![file isdirectory $tmp]} break
	  if {[lsearch $auto_path $tmp] < 0} {
	    if {$verbose} {tclLog "    inserted in auto_path."}
	    set auto_path [linsert $auto_path $pos $tmp]
	  }
	}
      }
    }
  }

# recursive directory listing, a bit like "ls -lR"
  proc listall {dirs} {
    while {[llength $dirs] > 0} {
      set dir [lindex $dirs 0]
      set dirs [lrange $dirs 1 end]
      puts "\n$dir:"
      set entries [glob -nocomplain [file join $dir *]]
      #if {[llength $entries] > 0} { puts "" }
      foreach path [lsort $entries] {
	if {[file isdir $path]} {
	  set len "         "
	  set tim "           dir"
	  set suf "/"
	  lappend dirs $path
	} else {
	  set len [format %9d [file size $path]]
	  set tim [clock format [file mtime $path] -format {%y/%m/%d %H:%M}]
	  set suf ""
	}
	puts " $len  $tim  [file tail $path]$suf"
      }
    }
    puts ""
  }

# recursively copy subdirs and files, if modification times differ
  proc sync {from to} {
    foreach path [glob -nocomplain [file join $from *]] {
      set tail [file tail $path]
      set dest [file join $to $tail]
      if {[file isdir $path]} {
	file mkdir $dest
	sync $path $dest
      } else {
	set mod [file mtime $path]
	if {![file exists $dest] || $mod != [file mtime $dest]} {
	  file delete $dest
	  file copy $path $dest
	  file mtime $dest $mod
	}
      }
    }
  }
}
