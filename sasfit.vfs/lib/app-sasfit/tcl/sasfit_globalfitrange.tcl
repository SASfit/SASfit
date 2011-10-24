# sasfit.vfs/lib/app-sasfit/tcl/sasfit_globalfitrange.tcl
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

#-------------------------------------------------------------------------------
#
#
proc set_GloballimitsCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if {[winfo exists .analytical.sdfitrange]} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar(SD,param_count) } {incr i} {
			.analytical.sdfitrange.a$i.limits invoke
			.analytical.sdfitrange.a$i.limits invoke
		}
	} 
	if {[winfo exists .analytical.fffitrange]} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar(FF,param_count) } {incr i} {
			.analytical.fffitrange.l$i.limits invoke
			.analytical.fffitrange.l$i.limits invoke
		}
	}
	if {[winfo exists .analytical.sqfitrange]} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar(SQ,param_count) } {incr i} {
			.analytical.sqfitrange.s$i.limits invoke
			.analytical.sqfitrange.s$i.limits invoke
		}
	}
}

#------------------------------------------------------------------------------
# menu for defining fit range of the fit parameter of the size distribution
#
proc SDGlobalFitRangeCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   global sasfit
   global GlobalAnalytPar
   global tmpGlobalAnalytPar
   global actualGlobalAnalytPar
   global tmpactualGlobalAnalytPar

tk_messageBox -message "this option is not ready for global fitting" -type ok
return

cp_arr actualGlobalAnalytPar tmpactualGlobalAnalytPar
if {[winfo exists .analytical.sdfitrange]} {destroy .analytical.sdfitrange}
toplevel .analytical.sdfitrange
set w .analytical.sdfitrange
wm geometry $w
wm title $w "fit range of size distribution parameters"
#
# label of columns
#
frame $w.layer
frame $w.tl
pack $w.layer -fill y -expand 1 -padx 2m -pady 2m
pack $w.tl -in $w.layer -fill x -expand 1
label $w.tl.text1 -text "minimum" -width 17
label $w.tl.text2 -text "maximum" -width 17
label $w.tl.text3 -text "on/off"
pack $w.tl.text3 $w.tl.text2 $w.tl.text1 -side right -fill x -in $w.tl
#
# entry fields for fit 
#
for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SD,param_count) } {incr i} {
	frame $w.a$i  
	pack $w.a$i -in $w.layer -fill x -expand 1
	label $w.a$i.label -textvariable tmpactualGlobalAnalytPar(SD,a$i,label)
	entry $w.a$i.min -textvariable tmpactualGlobalAnalytPar(SD,a$i,min) \
	      -relief sunken -width 17 -highlightthickness 0
	entry $w.a$i.max -textvariable tmpactualGlobalAnalytPar(SD,a$i,max) \
	      -relief sunken -width 17 -highlightthickness 0
	eval checkbutton $w.a$i.limits "-variable tmpactualGlobalAnalytPar(SD,a$i,limits) \
	      $::radio_check_button_prop -highlightthickness 0 \
	      -command { cb_cmd tmpactualGlobalAnalytPar SD a $i }"
	if {$tmpactualGlobalAnalytPar(SD,a$i,limits) == 0} {
	   .analytical.sdfitrange.a$i.min configure -state disabled
	   .analytical.sdfitrange.a$i.min configure -foreground #a3a3a3
	   .analytical.sdfitrange.a$i.max configure -state disabled
	   .analytical.sdfitrange.a$i.max configure -foreground #a3a3a3
	} else {
	   .analytical.sdfitrange.a$i.min configure -state normal
	   .analytical.sdfitrange.a$i.min configure -foreground Black
	   .analytical.sdfitrange.a$i.max configure -state normal
	   .analytical.sdfitrange.a$i.max configure -foreground Black
	}
	pack $w.a$i.limits $w.a$i.max $w.a$i.min $w.a$i.label \
	     -side right -in $w.a$i -padx 1m -pady 1m 
}

#
# buttons OK, Apply and Cancel
#
frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SD,param_count) } {incr i} {
		set actualGlobalAnalytPar(SD,a$i,limits) $tmpactualGlobalAnalytPar(SD,a$i,limits)
		set actualGlobalAnalytPar(SD,a$i,min)    $tmpactualGlobalAnalytPar(SD,a$i,min)
		set actualGlobalAnalytPar(SD,a$i,max)    $tmpactualGlobalAnalytPar(SD,a$i,max)
	}
	destroy .analytical.sdfitrange
   }
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SD,param_count) } {incr i} {
		set actualGlobalAnalytPar(SD,a$i,limits) $tmpactualGlobalAnalytPar(SD,a$i,limits)
		set actualGlobalAnalytPar(SD,a$i,min)    $tmpactualGlobalAnalytPar(SD,a$i,min)
		set actualGlobalAnalytPar(SD,a$i,max)    $tmpactualGlobalAnalytPar(SD,a$i,max)
	}
   }
button $w.buttons.cancel -text "Cancel" -highlightthickness 0 \
       -command { destroy .analytical.sdfitrange }
button $w.buttons.help -text "Help" -highlightthickness 0 \
       -command { }
pack $w.buttons.ok $w.buttons.apply $w.buttons.cancel \
     -in $w.buttons -padx 2m -pady 2m -side left
pack $w.buttons.help -in $w.buttons -padx 1m -pady 1 -side right
}

#------------------------------------------------------------------------------
# menu for defining fit range of the fit parameter of the form factor
#
proc FFGlobalFitRangeCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^
   global sasfit
   global GlobalAnalytPar
   global tmpGlobalAnalytPar
   global actualGlobalAnalytPar
   global tmpactualGlobalAnalytPar

tk_messageBox -message "this option is not ready for global fitting" -type ok
return

cp_arr actualGlobalAnalytPar tmpactualGlobalAnalytPar
if {[winfo exists .analytical.fffitrange]} {destroy .analytical.fffitrange}
toplevel .analytical.fffitrange
set w .analytical.fffitrange
wm geometry $w
wm title $w "fit range of form factor parameters"
#
# label of columns
#
frame $w.layer
frame $w.tl
pack $w.layer -fill y -expand 1 -padx 2m -pady 2m
pack $w.tl -in $w.layer -fill x -expand 1
label $w.tl.text1 -text "minimum" -width 17
label $w.tl.text2 -text "maximum" -width 17
label $w.tl.text3 -text "on/off"
pack $w.tl.text3 $w.tl.text2 $w.tl.text1 -side right -fill x -in $w.tl
#
# entry fields for fit
#
for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(FF,param_count) } {incr i} {
	frame $w.l$i
	pack $w.l$i -in $w.layer -fill x -expand 1
	label $w.l$i.label -textvariable tmpactualGlobalAnalytPar(FF,l$i,label)
	entry $w.l$i.min -textvariable tmpactualGlobalAnalytPar(FF,l$i,min) \
	      -relief sunken -width 17 -highlightthickness 0
	entry $w.l$i.max -textvariable tmpactualGlobalAnalytPar(FF,l$i,max) \
	      -relief sunken -width 17 -highlightthickness 0
	eval checkbutton $w.l$i.limits "-variable tmpactualGlobalAnalytPar(FF,l$i,limits) \
	      $::radio_check_button_prop -highlightthickness 0  \
	      -command { cb_cmd tmpactualGlobalAnalytPar FF l $i }"
	$w.l$i.limits invoke
	$w.l$i.limits invoke
	pack $w.l$i.limits $w.l$i.max $w.l$i.min $w.l$i.label \
	     -side right -in $w.l$i -padx 1m -pady 1m
}

#
# buttons OK, Apply and Cancel
#
frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(FF,param_count) } {incr i} {
		set actualGlobalAnalytPar(FF,l$i,limits) $tmpactualGlobalAnalytPar(FF,l$i,limits)
		set actualGlobalAnalytPar(FF,l$i,min)    $tmpactualGlobalAnalytPar(FF,l$i,min)
		set actualGlobalAnalytPar(FF,l$i,max)    $tmpactualGlobalAnalytPar(FF,l$i,max)
	}
	destroy .analytical.fffitrange
   }
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(FF,param_count) } {incr i} {
		set actualGlobalAnalytPar(FF,l$i,limits) $tmpactualGlobalAnalytPar(FF,l$i,limits)
		set actualGlobalAnalytPar(FF,l$i,min)    $tmpactualGlobalAnalytPar(FF,l$i,min)
		set actualGlobalAnalytPar(FF,l$i,max)    $tmpactualGlobalAnalytPar(FF,l$i,max)
	}
   }
button $w.buttons.cancel -text "Cancel" -highlightthickness 0 \
       -command { destroy .analytical.fffitrange }
pack $w.buttons.ok $w.buttons.apply $w.buttons.cancel \
     -in $w.buttons -padx 2m -pady 2m -side left
}


#------------------------------------------------------------------------------
# menu for defining fit range of the fit parameter of the form factor
#
proc SQGlobalFitRangeCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^
   global sasfit
   global GlobalAnalytPar
   global tmpGlobalAnalytPar
   global actualGlobalAnalytPar
   global tmpactualGlobalAnalytPar

tk_messageBox -message "this option is not ready for global fitting" -type ok
return

cp_arr actualGlobalAnalytPar tmpactualGlobalAnalytPar
if {[winfo exists .analytical.sqfitrange]} {destroy .analytical.sqfitrange}
toplevel .analytical.sqfitrange
set w .analytical.sqfitrange
wm geometry $w
wm title $w "fit range of structure factor parameters"
#
# label of columns
#
frame $w.layer
frame $w.tl
pack $w.layer -fill y -expand 1 -padx 2m -pady 2m
pack $w.tl -in $w.layer -fill x -expand 1
label $w.tl.text1 -text "minimum" -width 17
label $w.tl.text2 -text "maximum" -width 17
label $w.tl.text3 -text "on/off"
pack $w.tl.text3 $w.tl.text2 $w.tl.text1 -side right -fill x -in $w.tl
#
# entry fields for fit
#
for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SQ,param_count) } {incr i} {
	frame $w.s$i
	pack $w.s$i -in $w.layer -fill x -expand 1
	label $w.s$i.label -textvariable tmpactualGlobalAnalytPar(SQ,s$i,label)
	entry $w.s$i.min -textvariable tmpactualGlobalAnalytPar(SQ,s$i,min) \
	      -relief sunken -width 17 -highlightthickness 0
	entry $w.s$i.max -textvariable tmpactualGlobalAnalytPar(SQ,s$i,max) \
	      -relief sunken -width 17 -highlightthickness 0
	eval checkbutton $w.s$i.limits "-variable tmpactualGlobalAnalytPar(SQ,s$i,limits) \
	      $::radio_check_button_prop -highlightthickness 0  \
	      -command { cb_cmd tmpactualGlobalAnalytPar SQ s $i }"
	$w.s$i.limits invoke
	$w.s$i.limits invoke
	pack $w.s$i.limits $w.s$i.max $w.s$i.min $w.s$i.label \
	     -side right -in $w.s$i -padx 1m -pady 1m
}

#
# buttons OK, Apply and Cancel
#
frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SQ,param_count) } {incr i} {
		set actualGlobalAnalytPar(SQ,s$i,limits) $tmpactualGlobalAnalytPar(SQ,s$i,limits)
		set actualGlobalAnalytPar(SQ,s$i,min)    $tmpactualGlobalAnalytPar(SQ,s$i,min)
		set actualGlobalAnalytPar(SQ,s$i,max)    $tmpactualGlobalAnalytPar(SQ,s$i,max)
	}
	destroy .analytical.sqfitrange
   }
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command {
	for {set i 1} {$i <= $::tmpactualGlobalAnalytPar(SQ,param_count) } {incr i} {
		set actualGlobalAnalytPar(SQ,s$i,limits) $tmpactualGlobalAnalytPar(SQ,s$i,limits)
		set actualGlobalAnalytPar(SQ,s$i,min)    $tmpactualGlobalAnalytPar(SQ,s$i,min)
		set actualGlobalAnalytPar(SQ,s$i,max)    $tmpactualGlobalAnalytPar(SQ,s$i,max)
	}
   }
button $w.buttons.cancel -text "Cancel" -highlightthickness 0 \
       -command { destroy .analytical.sqfitrange }
pack $w.buttons.ok $w.buttons.apply $w.buttons.cancel \
     -in $w.buttons -padx 2m -pady 2m -side left
}
