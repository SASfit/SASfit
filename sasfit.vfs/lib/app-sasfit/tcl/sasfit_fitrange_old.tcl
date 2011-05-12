# sasfit.vfs/lib/app-sasfit/tcl/sasfit_fitrange_old.tcl
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

proc set_limitsCmd {} {
if {[winfo exists .analytical.sdfitrange]} {
   .analytical.sdfitrange.a1.limits invoke
   .analytical.sdfitrange.a1.limits invoke
   .analytical.sdfitrange.a2.limits invoke
   .analytical.sdfitrange.a2.limits invoke
   .analytical.sdfitrange.a3.limits invoke
   .analytical.sdfitrange.a3.limits invoke
   .analytical.sdfitrange.a4.limits invoke
   .analytical.sdfitrange.a4.limits invoke
} 
if {[winfo exists .analytical.fffitrange]} {
   .analytical.fffitrange.l1.limits invoke
   .analytical.fffitrange.l1.limits invoke
   .analytical.fffitrange.l2.limits invoke
   .analytical.fffitrange.l2.limits invoke
   .analytical.fffitrange.l3.limits invoke
   .analytical.fffitrange.l3.limits invoke
   .analytical.fffitrange.l4.limits invoke
   .analytical.fffitrange.l4.limits invoke
}
}

#------------------------------------------------------------------------------
# menu for defining fit range of the fit parameter of the size distribution
#
proc SDFitRangeCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^
   global sasfit
   global AnalytPar
   global tmpAnalytPar
   global actualAnalytPar
   global tmpactualAnalytPar

cp_arr actualAnalytPar tmpactualAnalytPar
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
frame $w.a1  
pack $w.a1 -in $w.layer -fill x -expand 1
label $w.a1.label -textvariable tmpactualAnalytPar(SD,a1,label)
entry $w.a1.min -textvariable tmpactualAnalytPar(SD,a1,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.a1.max -textvariable tmpactualAnalytPar(SD,a1,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.a1.limits "-variable tmpactualAnalytPar(SD,a1,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(SD,a1,limits) == 0} {
             .analytical.sdfitrange.a1.min configure -state disabled
             .analytical.sdfitrange.a1.min configure -foreground #a3a3a3
             .analytical.sdfitrange.a1.max configure -state disabled
             .analytical.sdfitrange.a1.max configure -foreground #a3a3a3
          } else {
             .analytical.sdfitrange.a1.min configure -state normal
             .analytical.sdfitrange.a1.min configure -foreground Black
             .analytical.sdfitrange.a1.max configure -state normal
             .analytical.sdfitrange.a1.max configure -foreground Black
          }
      }"
if {$tmpactualAnalytPar(SD,a1,limits) == 0} {
   .analytical.sdfitrange.a1.min configure -state disabled
   .analytical.sdfitrange.a1.min configure -foreground #a3a3a3
   .analytical.sdfitrange.a1.max configure -state disabled
   .analytical.sdfitrange.a1.max configure -foreground #a3a3a3
} else {
   .analytical.sdfitrange.a1.min configure -state normal
   .analytical.sdfitrange.a1.min configure -foreground Black
   .analytical.sdfitrange.a1.max configure -state normal
   .analytical.sdfitrange.a1.max configure -foreground Black
}
pack $w.a1.limits $w.a1.max $w.a1.min $w.a1.label \
     -side right -in $w.a1 -padx 1m -pady 1m 
frame $w.a2 
pack $w.a2 -in $w.layer -fill x -expand 1
label $w.a2.label -textvariable tmpactualAnalytPar(SD,a2,label)
entry $w.a2.min -textvariable tmpactualAnalytPar(SD,a2,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.a2.max -textvariable tmpactualAnalytPar(SD,a2,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.a2.limits "-variable tmpactualAnalytPar(SD,a2,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(SD,a2,limits) == 0} {
             .analytical.sdfitrange.a2.min configure -state disabled
             .analytical.sdfitrange.a2.min configure -foreground #a3a3a3
             .analytical.sdfitrange.a2.max configure -state disabled
             .analytical.sdfitrange.a2.max configure -foreground #a3a3a3
          } else {
             .analytical.sdfitrange.a2.min configure -state normal
             .analytical.sdfitrange.a2.min configure -foreground Black
             .analytical.sdfitrange.a2.max configure -state normal
             .analytical.sdfitrange.a2.max configure -foreground Black
          }
      }"
if {$tmpactualAnalytPar(SD,a2,limits) == 0} {
   .analytical.sdfitrange.a2.min configure -state disabled
   .analytical.sdfitrange.a2.min configure -foreground #a3a3a3
   .analytical.sdfitrange.a2.max configure -state disabled
   .analytical.sdfitrange.a2.max configure -foreground #a3a3a3
} else {
   .analytical.sdfitrange.a2.min configure -state normal
   .analytical.sdfitrange.a2.min configure -foreground Black
   .analytical.sdfitrange.a2.max configure -state normal
   .analytical.sdfitrange.a2.max configure -foreground Black
}
pack $w.a2.limits $w.a2.max $w.a2.min $w.a2.label \
     -side right -in $w.a2 -padx 1m -pady 1m
frame $w.a3 
pack $w.a3 -in $w.layer -fill x -expand 1
label $w.a3.label -textvariable tmpactualAnalytPar(SD,a3,label)
entry $w.a3.min -textvariable tmpactualAnalytPar(SD,a3,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.a3.max -textvariable tmpactualAnalytPar(SD,a3,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.a3.limits "-variable tmpactualAnalytPar(SD,a3,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(SD,a3,limits) == 0} {
             .analytical.sdfitrange.a3.min configure -state disabled
             .analytical.sdfitrange.a3.min configure -foreground #a3a3a3
             .analytical.sdfitrange.a3.max configure -state disabled
             .analytical.sdfitrange.a3.max configure -foreground #a3a3a3
          } else {
             .analytical.sdfitrange.a3.min configure -state normal
             .analytical.sdfitrange.a3.min configure -foreground Black
             .analytical.sdfitrange.a3.max configure -state normal
             .analytical.sdfitrange.a3.max configure -foreground Black
          }
      }"
if {$tmpactualAnalytPar(SD,a3,limits) == 0} {
   .analytical.sdfitrange.a3.min configure -state disabled
   .analytical.sdfitrange.a3.min configure -foreground #a3a3a3
   .analytical.sdfitrange.a3.max configure -state disabled
   .analytical.sdfitrange.a3.max configure -foreground #a3a3a3
} else {
   .analytical.sdfitrange.a3.min configure -state normal
   .analytical.sdfitrange.a3.min configure -foreground Black
   .analytical.sdfitrange.a3.max configure -state normal
   .analytical.sdfitrange.a3.max configure -foreground Black
}
pack $w.a3.limits $w.a3.max $w.a3.min $w.a3.label \
     -side right -in $w.a3 -padx 1m -pady 1m
frame $w.a4 
pack $w.a4 -in $w.layer -fill x -expand 1
label $w.a4.label -textvariable tmpactualAnalytPar(SD,a4,label)
entry $w.a4.min -textvariable tmpactualAnalytPar(SD,a4,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.a4.max -textvariable tmpactualAnalytPar(SD,a4,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.a4.limits "-variable tmpactualAnalytPar(SD,a4,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(SD,a4,limits) == 0} {
             .analytical.sdfitrange.a4.min configure -state disabled
             .analytical.sdfitrange.a4.min configure -foreground #a3a3a3
             .analytical.sdfitrange.a4.max configure -state disabled
             .analytical.sdfitrange.a4.max configure -foreground #a3a3a3
          } else {
             .analytical.sdfitrange.a4.min configure -state normal
             .analytical.sdfitrange.a4.min configure -foreground Black
             .analytical.sdfitrange.a4.max configure -state normal
             .analytical.sdfitrange.a4.max configure -foreground Black
          }
      }"
if {$tmpactualAnalytPar(SD,a4,limits) == 0} {
   .analytical.sdfitrange.a4.min configure -state disabled
   .analytical.sdfitrange.a4.min configure -foreground #a3a3a3
   .analytical.sdfitrange.a4.max configure -state disabled
   .analytical.sdfitrange.a4.max configure -foreground #a3a3a3
} else {
   .analytical.sdfitrange.a4.min configure -state normal
   .analytical.sdfitrange.a4.min configure -foreground Black
   .analytical.sdfitrange.a4.max configure -state normal
   .analytical.sdfitrange.a4.max configure -foreground Black
}
pack $w.a4.limits $w.a4.max $w.a4.min $w.a4.label \
      -side right -in $w.a4 -padx 1m -pady 1m 
#
# buttons OK, Apply and Cancel
#
frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command {set actualAnalytPar(SD,a1,limits) $tmpactualAnalytPar(SD,a1,limits)
             set actualAnalytPar(SD,a1,min) $tmpactualAnalytPar(SD,a1,min)
             set actualAnalytPar(SD,a1,max) $tmpactualAnalytPar(SD,a1,max)
             set actualAnalytPar(SD,a2,limits) $tmpactualAnalytPar(SD,a2,limits)
             set actualAnalytPar(SD,a2,min) $tmpactualAnalytPar(SD,a2,min)
             set actualAnalytPar(SD,a2,max) $tmpactualAnalytPar(SD,a2,max)
             set actualAnalytPar(SD,a3,limits) $tmpactualAnalytPar(SD,a3,limits)
             set actualAnalytPar(SD,a3,min) $tmpactualAnalytPar(SD,a3,min)
             set actualAnalytPar(SD,a3,max) $tmpactualAnalytPar(SD,a3,max)
             set actualAnalytPar(SD,a4,limits) $tmpactualAnalytPar(SD,a4,limits)
             set actualAnalytPar(SD,a4,min) $tmpactualAnalytPar(SD,a4,min)
             set actualAnalytPar(SD,a4,max) $tmpactualAnalytPar(SD,a4,max)
             destroy .analytical.sdfitrange
            }
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command {set actualAnalytPar(SD,a1,limits) $tmpactualAnalytPar(SD,a1,limits)
             set actualAnalytPar(SD,a1,min) $tmpactualAnalytPar(SD,a1,min)
             set actualAnalytPar(SD,a1,max) $tmpactualAnalytPar(SD,a1,max)
             set actualAnalytPar(SD,a2,limits) $tmpactualAnalytPar(SD,a2,limits)
             set actualAnalytPar(SD,a2,min) $tmpactualAnalytPar(SD,a2,min)
             set actualAnalytPar(SD,a2,max) $tmpactualAnalytPar(SD,a2,max)
             set actualAnalytPar(SD,a3,limits) $tmpactualAnalytPar(SD,a3,limits)
             set actualAnalytPar(SD,a3,min) $tmpactualAnalytPar(SD,a3,min)
             set actualAnalytPar(SD,a3,max) $tmpactualAnalytPar(SD,a3,max)
             set actualAnalytPar(SD,a4,limits) $tmpactualAnalytPar(SD,a4,limits)
             set actualAnalytPar(SD,a4,min) $tmpactualAnalytPar(SD,a4,min)
             set actualAnalytPar(SD,a4,max) $tmpactualAnalytPar(SD,a4,max)
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
proc FFFitRangeCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^
   global sasfit
   global AnalytPar
   global tmpAnalytPar
   global actualAnalytPar
   global tmpactualAnalytPar

cp_arr actualAnalytPar tmpactualAnalytPar
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
frame $w.l1
pack $w.l1 -in $w.layer -fill x -expand 1
label $w.l1.label -textvariable tmpactualAnalytPar(FF,l1,label)
entry $w.l1.min -textvariable tmpactualAnalytPar(FF,l1,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.l1.max -textvariable tmpactualAnalytPar(FF,l1,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.l1.limits "-variable tmpactualAnalytPar(FF,l1,limits) \
      $::radio_check_button_prop -highlightthickness 0  \
      -command {
          if {$tmpactualAnalytPar(FF,l1,limits) == 0} {
             .analytical.fffitrange.l1.min configure -state disabled
             .analytical.fffitrange.l1.min configure -foreground #a3a3a3
             .analytical.fffitrange.l1.max configure -state disabled
             .analytical.fffitrange.l1.max configure -foreground #a3a3a3
          } else {
             .analytical.fffitrange.l1.min configure -state normal
             .analytical.fffitrange.l1.min configure -foreground Black
             .analytical.fffitrange.l1.max configure -state normal
             .analytical.fffitrange.l1.max configure -foreground Black
          }
      }"
$w.l1.limits invoke
$w.l1.limits invoke
pack $w.l1.limits $w.l1.max $w.l1.min $w.l1.label \
     -side right -in $w.l1 -padx 1m -pady 1m
frame $w.l2
pack $w.l2 -in $w.layer -fill x -expand 1
label $w.l2.label -textvariable tmpactualAnalytPar(FF,l2,label)
entry $w.l2.min -textvariable tmpactualAnalytPar(FF,l2,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.l2.max -textvariable tmpactualAnalytPar(FF,l2,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.l2.limits "-variable tmpactualAnalytPar(FF,l2,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(FF,l2,limits) == 0} {
             .analytical.fffitrange.l2.min configure -state disabled
             .analytical.fffitrange.l2.min configure -foreground #a3a3a3
             .analytical.fffitrange.l2.max configure -state disabled
             .analytical.fffitrange.l2.max configure -foreground #a3a3a3
          } else {
             .analytical.fffitrange.l2.min configure -state normal
             .analytical.fffitrange.l2.min configure -foreground Black
             .analytical.fffitrange.l2.max configure -state normal
             .analytical.fffitrange.l2.max configure -foreground Black
          }
      }"
$w.l2.limits invoke
$w.l2.limits invoke
pack $w.l2.limits $w.l2.max $w.l2.min $w.l2.label \
     -side right -in $w.l2 -padx 1m -pady 1m
frame $w.l3
pack $w.l3 -in $w.layer -fill x -expand 1
label $w.l3.label -textvariable tmpactualAnalytPar(FF,l3,label)
entry $w.l3.min -textvariable tmpactualAnalytPar(FF,l3,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.l3.max -textvariable tmpactualAnalytPar(FF,l3,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.l3.limits "-variable tmpactualAnalytPar(FF,l3,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(FF,l3,limits) == 0} {
             .analytical.fffitrange.l3.min configure -state disabled
             .analytical.fffitrange.l3.min configure -foreground #a3a3a3
             .analytical.fffitrange.l3.max configure -state disabled
             .analytical.fffitrange.l3.max configure -foreground #a3a3a3
          } else {
             .analytical.fffitrange.l3.min configure -state normal
             .analytical.fffitrange.l3.min configure -foreground Black
             .analytical.fffitrange.l3.max configure -state normal
             .analytical.fffitrange.l3.max configure -foreground Black
          }
      }"
$w.l3.limits invoke
$w.l3.limits invoke
pack $w.l3.limits $w.l3.max $w.l3.min $w.l3.label \
     -side right -in $w.l3 -padx 1m -pady 1m
frame $w.l4
pack $w.l4 -in $w.layer -fill x -expand 1
label $w.l4.label -textvariable tmpactualAnalytPar(FF,l4,label)
entry $w.l4.min -textvariable tmpactualAnalytPar(FF,l4,min) \
      -relief sunken -width 17 -highlightthickness 0
entry $w.l4.max -textvariable tmpactualAnalytPar(FF,l4,max) \
      -relief sunken -width 17 -highlightthickness 0
eval checkbutton $w.l4.limits "-variable tmpactualAnalytPar(FF,l4,limits) \
      $::radio_check_button_prop -highlightthickness 0 \
      -command {
          if {$tmpactualAnalytPar(FF,l4,limits) == 0} {
             .analytical.fffitrange.l4.min configure -state disabled
             .analytical.fffitrange.l4.min configure -foreground #a3a3a3
             .analytical.fffitrange.l4.max configure -state disabled
             .analytical.fffitrange.l4.max configure -foreground #a3a3a3
          } else {
             .analytical.fffitrange.l4.min configure -state normal
             .analytical.fffitrange.l4.min configure -foreground Black
             .analytical.fffitrange.l4.max configure -state normal
             .analytical.fffitrange.l4.max configure -foreground Black
          }
      }"
$w.l4.limits invoke
$w.l4.limits invoke
pack $w.l4.limits $w.l4.max $w.l4.min $w.l4.label \
     -side right -in $w.l4 -padx 1m -pady 1m
#
# buttons OK, Apply and Cancel
#
frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command {set actualAnalytPar(FF,l1,limits) $tmpactualAnalytPar(FF,l1,limits)
             set actualAnalytPar(FF,l1,min) $tmpactualAnalytPar(FF,l1,min)
             set actualAnalytPar(FF,l1,max) $tmpactualAnalytPar(FF,l1,max)
             set actualAnalytPar(FF,l2,limits) $tmpactualAnalytPar(FF,l2,limits)
             set actualAnalytPar(FF,l2,min) $tmpactualAnalytPar(FF,l2,min)
             set actualAnalytPar(FF,l2,max) $tmpactualAnalytPar(FF,l2,max)
             set actualAnalytPar(FF,l3,limits) $tmpactualAnalytPar(FF,l3,limits)
             set actualAnalytPar(FF,l3,min) $tmpactualAnalytPar(FF,l3,min)
             set actualAnalytPar(FF,l3,max) $tmpactualAnalytPar(FF,l3,max)
             set actualAnalytPar(FF,l4,limits) $tmpactualAnalytPar(FF,l4,limits)
             set actualAnalytPar(FF,l4,min) $tmpactualAnalytPar(FF,l4,min)
             set actualAnalytPar(FF,l4,max) $tmpactualAnalytPar(FF,l4,max)
             destroy .analytical.fffitrange
            }
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command {set actualAnalytPar(FF,l1,limits) $tmpactualAnalytPar(FF,l1,limits)
             set actualAnalytPar(FF,l1,min) $tmpactualAnalytPar(FF,l1,min)
             set actualAnalytPar(FF,l1,max) $tmpactualAnalytPar(FF,l1,max)
             set actualAnalytPar(FF,l2,limits) $tmpactualAnalytPar(FF,l2,limits)
             set actualAnalytPar(FF,l2,min) $tmpactualAnalytPar(FF,l2,min)
             set actualAnalytPar(FF,l2,max) $tmpactualAnalytPar(FF,l2,max)
             set actualAnalytPar(FF,l3,limits) $tmpactualAnalytPar(FF,l3,limits)
             set actualAnalytPar(FF,l3,min) $tmpactualAnalytPar(FF,l3,min)
             set actualAnalytPar(FF,l3,max) $tmpactualAnalytPar(FF,l3,max)
             set actualAnalytPar(FF,l4,limits) $tmpactualAnalytPar(FF,l4,limits)
             set actualAnalytPar(FF,l4,min) $tmpactualAnalytPar(FF,l4,min)
             set actualAnalytPar(FF,l4,max) $tmpactualAnalytPar(FF,l4,max)
            }
button $w.buttons.cancel -text "Cancel" -highlightthickness 0 \
       -command { destroy .analytical.fffitrange }
pack $w.buttons.ok $w.buttons.apply $w.buttons.cancel \
     -in $w.buttons -padx 2m -pady 2m -side left
}
