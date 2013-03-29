# sasfit.vfs/lib/app-sasfit/tcl/sasfit_fitrange.tcl
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
proc set_limitsCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if {[winfo exists .analytical.sdfitrange]} {
		for {set i 1} {$i <= $::actualAnalytPar(SD,param_count) } {incr i} {
			.analytical.sdfitrange.a$i.limits invoke
			.analytical.sdfitrange.a$i.limits invoke
		}
	} 
	if {[winfo exists .analytical.fffitrange]} {
		for {set i 1} {$i <= $::actualAnalytPar(FF,param_count) } {incr i} {
			.analytical.fffitrange.l$i.limits invoke
			.analytical.fffitrange.l$i.limits invoke
		}
	}
	if {[winfo exists .analytical.sqfitrange]} {
		for {set i 1} {$i <= $::actualAnalytPar(SQ,param_count) } {incr i} {
			.analytical.sqfitrange.s$i.limits invoke
			.analytical.sqfitrange.s$i.limits invoke
		}
	}
}

# menu for defining fit range of the fit parameter of the size distribution

proc fitRangeCmd { type } {
cp_arr ::actualAnalytPar ::tmpactualAnalytPar
set lowerType [string tolower $type]
set w .analytical.${lowerType}fitrange
if {[winfo exists $w]} {destroy $w}
toplevel $w
wm geometry $w
set detailText ""
set p ""
switch $type {
        SD {
                set p "a"
                set detailText "size distribution"
        }
        FF {
                set p "l"
                set detailText "form factor"
        }
        SQ {
                set p "s"
                set detailText "structure factor"
        }
        default {
                tk_messageBox -parent $w -icon error \
                    -message "fitRangeCmd() called with wrong type '$type'!"\
                    -title "Runtime Error"
                return
        }
}
wm title $w [format "fit range of %s parameters" $detailText]
bind $w <Escape> "destroy $w"
#
# label of columns
#
frame $w.layer
frame $w.headerFrame
frame $w.tl
pack $w.layer -fill y -expand 1 -padx 2m -pady 2m
pack $w.headerFrame -in $w.layer -fill x -expand 1
pack $w.tl -in $w.layer -fill x -expand 1
label $w.tl.text1 -text "minimum" -width 17
label $w.tl.text2 -text "maximum" -width 17
label $w.tl.text3 -text "on/off"
label $w.headerFrame.text -justify left
pack $w.headerFrame.text -side left -in $w.headerFrame
pack $w.tl.text3 $w.tl.text2 $w.tl.text1 -side right -fill x -in $w.tl

# entry fields for fit 

for {set i 1} {$i <= $::tmpactualAnalytPar($type,param_count) } {incr i} {
	frame $w.$p$i
	pack $w.$p$i -in $w.layer -fill x -expand 1
	label $w.$p$i.label -textvariable ::tmpactualAnalytPar($type,$p$i,label)
	entry $w.$p$i.min -textvariable ::tmpactualAnalytPar($type,$p$i,min) \
	      -relief sunken -width 17 -highlightthickness 0
        bind $w.$p$i.min <FocusOut> "verifyValue ::tmpactualAnalytPar $w $type $p$i minimum"
        bind $w.$p$i.min <Tab> "focus $w.$p$i.max\nbreak"
        bind $w.$p$i.min <Shift-Tab> "focusWin $w.$p[expr $i-1].limits\nbreak"
	entry $w.$p$i.max -textvariable ::tmpactualAnalytPar($type,$p$i,max) \
	      -relief sunken -width 17 -highlightthickness 0
        bind $w.$p$i.max <FocusOut> "verifyValue ::tmpactualAnalytPar $w $type $p$i maximum"
        bind $w.$p$i.max <Tab> "focus $w.$p$i.limits\nbreak"
        bind $w.$p$i.max <Shift-Tab> "focus $w.$p$i.min\nbreak"
	eval checkbutton $w.$p$i.limits "-variable ::tmpactualAnalytPar($type,$p$i,limits) \
	      $::radio_check_button_prop -highlightthickness 0 \
	      -command { cb_cmd ::tmpactualAnalytPar $type $p $i }"
        bind $w.$p$i.limits <Tab> "focusWin $w.$p[expr $i+1].min\nbreak"
        bind $w.$p$i.limits <Shift-Tab> "focus $w.$p$i.max\nbreak"
        cb_cmd ::tmpactualAnalytPar $type $p $i
	pack $w.$p$i.limits $w.$p$i.max $w.$p$i.min $w.$p$i.label \
	     -side right -in $w.$p$i -padx 1m -pady 1m 
}

# buttons OK, Apply and Cancel

frame $w.buttons -relief groove -borderwidth 1
pack $w.buttons -fill x
button $w.buttons.ok -text "OK" -highlightthickness 0 \
   -command "setLimits ::actualAnalytPar ::tmpactualAnalytPar $type $p true"
button $w.buttons.apply -text "Apply" -highlightthickness 0 \
   -command "setLimits ::actualAnalytPar ::tmpactualAnalytPar $type $p false"
button $w.buttons.cancel -text "Cancel" -highlightthickness 0 \
       -command "destroy $w"
button $w.buttons.help -text "Help" -highlightthickness 0 \
       -command { }
pack $w.buttons.ok $w.buttons.apply $w.buttons.cancel \
     -in $w.buttons -padx 2m -pady 2m -side left
pack $w.buttons.help -in $w.buttons -padx 1m -pady 1 -side right

# set informative header text
update
$w.headerFrame.text configure -wraplength [winfo width $w] -text [headerText]
bind $w <Return> "$w.buttons.ok invoke"
}

proc focusWin { window } {
        # changes focus only if the window exists
        if { ! [winfo exists $window] } {
                return
        }
        focus $window
}

proc headerText { } {
        return [join [list \
                "Please set the range limits to physical reasonable values! " \
                "The range (max-min) should be much smaller than 1e16. " \
                "\nThe Parameter range is implemented by projecting into " \
                "\[0, 1\] which gets numerically unstable for ranges near " \
                "inversed machine precision EPS ~1e-16."]]
}

proc setLimits { actap tmpap type p closeWindow } {
	upvar $actap aap
	upvar $tmpap tap
        set lt [string tolower $type]
        set window .analytical.${lt}fitrange
	for {set i 1} {$i <= $tap($type,param_count) } {incr i} {
                if { ! [verifyRange $tmpap $window $type $p$i] } {
                        return
                }
		set aap($type,$p$i,limits) $tap($type,$p$i,limits)
		set aap($type,$p$i,min)    $tap($type,$p$i,min)
		set aap($type,$p$i,max)    $tap($type,$p$i,max)
	}
        set closeWindow [string tolower $closeWindow]
        if { $closeWindow == "true" } {
        	destroy $window
        }
}

proc rangeError { w msg } {
        tk_messageBox -parent $w -icon error \
                -message $msg -title "Range Limit Error"
}

proc verifyValue { tmpap window type par which } {
        upvar $tmpap tap
        set wh [string range $which 0 2]
        set value $tap($type,$par,$wh)
        set name [string trimright $tap($type,$par,label) "= "]
        if { ! [string is double -strict $value] } {
                rangeError $window [join [list \
                        "Please enter a decimal number for the " \
                        "<$which> of <$name>!"]]
                focus $window.$par.$wh
                return 0
        }
        return 1
}

proc verifyRange { tmpap window type par } {
        upvar $tmpap tap
        if { "[winfo class [focus]]" == "Entry" } {
                if { ! ([verifyValue $tmpap $window $type $par minimum] &&
                        [verifyValue $tmpap $window $type $par maximum]) } {
                        return 0
                }
        }
        set minValue $tap($type,$par,min)
        set maxValue $tap($type,$par,max)
        set width [expr $maxValue - $minValue]
        puts "range verify: $minValue $maxValue $width"
        if { $tap($type,$par,limits) == 1 && [expr $width > 1e16] } {
                set name [string trimright $tap($type,$par,label) "= "]
                rangeError $window [format \
                        "Parameter <%s> range %g:\n%s"\
                        $name $width [headerText]]
                return 0
        }
        return 1
}

proc cb_cmd { tmpap type p i } {
	upvar $tmpap ap
	set lt [string tolower $type]
	if {$ap($type,$p$i,limits) == 0} {
	     .analytical.${lt}fitrange.$p$i.min configure -state disabled
	     .analytical.${lt}fitrange.$p$i.min configure -foreground #a3a3a3
	     .analytical.${lt}fitrange.$p$i.max configure -state disabled
	     .analytical.${lt}fitrange.$p$i.max configure -foreground #a3a3a3
	} else {
	     .analytical.${lt}fitrange.$p$i.min configure -state normal
	     .analytical.${lt}fitrange.$p$i.min configure -foreground Black
	     .analytical.${lt}fitrange.$p$i.max configure -state normal
	     .analytical.${lt}fitrange.$p$i.max configure -foreground Black
	}
}

proc verifyFloat { tmpap idx newValue } {
	upvar $tmpap ap
        if { ! [string is double $newValue] } {
                puts "string not float '$newValue'"
                return false
        }
        return true
}
