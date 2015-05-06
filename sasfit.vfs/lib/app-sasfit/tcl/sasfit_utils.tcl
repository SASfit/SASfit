# sasfit.vfs/lib/app-sasfit/tcl/sasfit_utils.tcl
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

proc calc_e_by_fit_straight_line {x y} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
set sx 0
set sy 0
set st2 0
set b 0
set ndata [llength $x]
set ss [llength $x]
foreach xi $x yi $y {
    set sx [expr $sx + $xi]
    set sy [expr $sy + $yi]
}
set sxoss [expr $sx/$ss]
foreach xi $x yi $y {
    set t   [expr $xi-$sxoss]
    set st2 [expr $st2 + $t*$t]
    set b   [expr $b + $t*$yi]
}
set b [expr $b/$st2]
set a [expr ($sy-$sx*$b)/$ss]
set siga [expr sqrt((1.0+$sx*$sx/($ss*$st2))/$ss)]
set sigb [expr sqrt(1.0/$st2)]
set chi2 0.0
foreach xi $x yi $y {
    set chi2 [expr $chi2+pow($yi-$a-$b*$xi,2.0)]
}
set q 1
set sigdat [expr sqrt($chi2/($ndata-2))]
return $sigdat
#set sigdat [expr sqrt($chi2/$ndata)]
set siga [expr $siga*$sigdat]
set sigb [expr $sigb*$sigdat]
return [expr sqrt(pow($siga,2)+pow($b*$sigb,2.))]
}

proc invxaxistrans {xtrans x} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
switch $xtrans {
    "x"          {set xtrans "\$x"              }
    "pow(x,2)"   {set xtrans "pow(\$x,1.0/2.0)" } 
    "pow(x,3)"   {set xtrans "pow(\$x,1.0/3.0)" }
    "pow(x,4)"   {set xtrans "pow(\$x,1.0/4.0)" }
    "log10(x)"   {set xtrans "\$x"              }
    "log(x)"     {set xtrans "exp(\$x)"         }
    "sqrt(x)"    {set xtrans "pow(\$x,2.0)"     }
    "1/x"        {set xtrans "1.0/\$x"          }
    "1/sqrt(x)"  {set xtrans "1.0/pow(\$x,2.0)" }
    "arcsinh(x)" {set xtrans "sinh(\$x)"        }
}
if {[catch {set result [expr $xtrans]} msg]} {
    puts $msg
    set result 0
}
return  $result
}

#------------------------------------------------------------------------------
# puts_arr prints contents of array arr 
#
# usage:
#    putst_arr arr
# input parameter:
#    fid: channel ID
#    arr: array to be printed
# output parameter:
#    none
# returned value:
#    contents of array arr
#
proc puts_arr {fid arr} {
#^^^^^^^^^^^^^^^^^^^^^^^^
upvar $arr a1

set error [catch {set searchID [array startsearch a1]}]
if { $error==1 } { return 0}
set no_update_entries $::sasfit(no_update_entries)
for {set i 1} {$i <= [array size a1]} {incr i} {
   set nel [array nextelement a1 $searchID]
   if {[lsearch $no_update_entries $nel] >= 0} { continue }
#   puts $fid "set $arr\($nel\) \{$a1($nel)\}"
   if {[llength $a1($nel)] > 1} {
       puts $fid "set $arr\($nel\) \{$a1($nel)\}"
   } elseif {[llength [lindex $a1($nel) 0]] > 1} {
       puts $fid "set $arr\($nel\) \{$a1($nel)\}"
   } elseif { [string compare \" [string index [concat $a1($nel)] 0]] == 0} {
       puts $fid "set $arr\($nel\) $a1($nel)"
   } else {
       puts $fid "set $arr\($nel\) \"$a1($nel)\""
   }
}
array donesearch a1 $searchID
return 1
}

#------------------------------------------------------------------------------
# print_arr prints contents of array arr 
#
# usage:
#    print_arr arr
# input parameter:
#    arr: array to be printed
# output parameter:
#    none
# returned value:
#    contents of array arr
#
proc print_arr {arr} {

	upvar $arr a1
#set error [catch {set searchID [array startsearch a1]}]
#if { $error==1 } { return 0}
#for {set i 1} {$i <= [array size a1]} {incr i} {
#   set nel [array nextelement a1 $searchID]
#   puts "$nel: $a1($nel)"
#}
#array donesearch a1 $searchID
	foreach {name value} [array get a1] {
		puts "array($name): $value"
	}
	return 1
}

#------------------------------------------------------------------------------
# cp_arr copies contents of array arr1 into array arr2
#
# usage:
#    cp_arr arr1 arr2
# input parameter:
#    arr1: array to be copied
# output parameter:
#    arr2: new array with contents of arr1
#          if arr2 already exists, the array elements which are not
#          defined in arr1, stay defined
# returned value:
#    0 if copy was successful
#    1 if copy failed
#
proc cp_arr {arr1 arr2} {

	upvar $arr1 a1
	upvar $arr2 a2
	foreach {name value} [array get a1] {
		set a2($name) $value
	}
	return 1
}

#------------------------------------------------------------------------------
# lmindex returns a new list of the list x with subscripts stored in ind 
#
# usage:
#    lmindex $x $ind
# input parameters:
#    $x  : original list
#    $ind: list of subscript
# returned value:
#    new list of the list $x with subscripts stored in ind
#    an error occures if a subscipts points to an undefined index in list $x
#
proc lmindex {x ind} {
#^^^^^^^^^^^^^^^^^^^^^
   set rv {}
   if { [llength $ind] > 0 } {
      foreach i $ind {
         if { ($i < 0) || ($i >= [llength $x]) } {
            error "can't find subscript $i in list $x"
            return {}
         }
         set rv [linsert $rv end [lindex $x $i]]
      }
   }
   return $rv
}

#------------------------------------------------------------------------------
# lsort_index sorts the contens of a list
#
# usage:
#    lsort_index x
# input parameters:
#    x: the list to be sorted
# returned value:
#    a list of subscripts which allow access to the elements
#    of x in ascending order by the function lmindex 
#    if x is an empty list and empty list will be returned by lsort_index
# returns {} if sorting fails
# returns sorted list of y-values if sorting was successfull
#
proc helpsort {x1list x2list} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# function needed by lsort_index, it defines if a list element 
# is smaler, equal or larger than another
if { [lindex $x1list 0] < [lindex $x2list 0] } { return -1}
if { [lindex $x1list 0] == [lindex $x2list 0] } { return 0}
if { [lindex $x1list 0] > [lindex $x2list 0] } { return 1}
}
proc lsort_index {x} {
	#^^^^^^^^^^^^^^^^^^^^^
	#
	# return empty list if $x is an empty list
	#
	if {[llength $x] == 0} { return {} }
	#
	# create first a list of indices starting with 0
	#
	set c 0
	set data {}
	foreach i $x {
		set data [linsert $data end [list $i $c] ]
		incr c
	}	
	#
	# now sorting data
	#
	set data [lsort -command helpsort $data]
	#
	# getting list of indices of the sorted list $x
	#
	set x_ind {}
	foreach i $data {
		set x_ind [linsert $x_ind end [lindex $i 1] ]
	}
	return $x_ind
}


# Version of framebox that uses the placer.  Effects are only really
# OK when used with a static label of a suitable size of font.
proc framebox {w args} {
#^^^^^^^^^^^^^^^^^^^^^^^
frame $w
frame $w._border -relief ridge -bd 2
    uplevel label $w._label $args

    set dist [winfo reqheight $w._label]
    set d [expr $dist/2]

    place $w._label -x [expr $dist+2] -y 1
    pack $w._border -padx $d -pady $d -expand 1 -fill both
    pack [frame $w.c] -in $w._border -padx $d -pady $d -expand 1 -fill both
    return $w
}

# An alternate version using grid which should handle different
# heights of label much better.  It uses some of the more interesting
# things you can do with the gridder...
proc framebox_grid {w args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
frame $w
    set gap [winfo pixels $w 2m]
    grid [frame $w._border -bd 2 -relief ridge] \
	    -rowspan 3 -columnspan 3 -padx $gap -pady $gap -sticky nsew 
    incr gap $gap
    foreach rc {column row} {
	grid $rc . 0 -minsize $gap
	grid $rc . 1 -weight  1
	grid $rc . 2 -minsize $gap
    }
    grid [frame $w.c] -row 1 -column 1
    grid [uplevel label $w._label $args] -row 0 -column 1 -sticky w
    return $w
}
##example:
#pack [framebox .f -text "Command:"] -expand 1 -fill both
#
#pack [frame .f.c.1] -fill x
#pack [button .f.c.1.1 -text Read] -fill x -expand 1 -side left
#pack [button .f.c.1.2 -text Write] -fill x -expand 1 -side left
#pack [button .f.c.2 -text "Quit" -command exit] -fill x
#pack [button .f.c.3 -text "Long label - does nothing"] -fill x

proc StructParData2RTFclipboard {SPD} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SPD StructParData
clipboard clear

}

proc sasfit_timer_start { message_prefix 
} {
	puts stderr "$message_prefix at [clock format [clock scan now] -format %H:%M:%S]"
	set ::sasfit_timer_start [clock clicks -milliseconds]
}

proc sasfit_timer_stop { message_prefix message_postprefix message_postfix
} {
	set tend [clock clicks -milliseconds]
	set eti [expr $tend-$::sasfit_timer_start]
	set h [expr $eti/3600000]
	set eti [expr $eti-$h*3600000]
	set min [expr $eti/60000]
	set sec [expr ($eti-$min*60000)/1e3]

	puts stderr "$message_prefix needed (${h}h:${min}min:${sec}sec) ($message_postprefix at [clock format [clock scan now] -format %H:%M:%S])$message_postfix"
}

# tooltip code from:
# http://wiki.tcl.tk/1954
proc setTooltip {widget text
} {
	if { $text != "" } {
		# 2) Adjusted timings and added key and button bindings. These seem to
		# make artifacts tolerably rare.
		bind $widget <Any-Enter>    [list after 500 [list showTooltip %W $text]]
		bind $widget <Any-Leave>    [list after 500 [list destroy %W.tooltip]]
		bind $widget <Any-KeyPress> [list after 500 [list destroy %W.tooltip]]
		bind $widget <Any-Button>   [list after 500 [list destroy %W.tooltip]]
	}
}

proc showTooltip {widget text
} {
	global tcl_platform
	if { [string match $widget* [winfo containing  [winfo pointerx .] [winfo pointery .]] ] == 0  } {
		return
	}

	catch { destroy $widget.tooltip }

	set scrh [winfo screenheight $widget]    ; # 1) flashing window fix
	set scrw [winfo screenwidth $widget]     ; # 1) flashing window fix
	set tooltip [toplevel $widget.tooltip -bd 1 -bg black]
	wm geometry $tooltip +$scrh+$scrw        ; # 1) flashing window fix
	wm overrideredirect $tooltip 1

	if {$tcl_platform(platform) == {windows}} { ; # 3) wm attributes...
		wm attributes $tooltip -topmost 1   ; # 3) assumes...
	}                                           ; # 3) Windows
	pack [label $tooltip.label -bg lightyellow -fg black -text $text -justify left]

	set width [winfo reqwidth $tooltip.label]
	set height [winfo reqheight $tooltip.label]

	set positionX [winfo pointerx .]
	set positionY [expr [winfo pointery .] + 25]

	# a.) Ad-hockery: Set positionX so the entire tooltip widget will be displayed.
	if  {[expr $positionX + $width] > [winfo screenwidth .]} {
		set positionX [expr ($positionX - (($positionX + $width) - [winfo screenwidth .]))]
	}

	wm geometry $tooltip [join  "$width x $height + $positionX + $positionY" {}]
	raise $tooltip

	# 2) Kludge: defeat rare artifact by passing mouse over a tooltip to destroy it.
	bind $widget.tooltip <Any-Enter> {destroy %W}
	bind $widget.tooltip <Any-Leave> {destroy %W}
}

proc isnan {x} {
	if { ![string is double $x] || $x != $x } {
		return 1
	} else {
		return 0
	}
}

