#!/bin/sh
# the next line restarts using wish \
exec wish "$0" ${1+"$@"}

#==============================================================================
# Demonstrates some ways of improving the look & feel of a tablelist widget.
#
# Copyright (c) 2002-2005  Csaba Nemethi (E-mail: csaba.nemethi@t-online.de)
#==============================================================================

package require Tablelist

wm title . "Tablelist Styles"

#
# Get the current windowing system ("x11", "win32", "classic",
# or "aqua") and add some entries to the Tk option database
#
if {[catch {tk windowingsystem} winSys] != 0} {
    switch $::tcl_platform(platform) {
	unix      { set winSys x11 }
	windows   { set winSys win32 }
	macintosh { set winSys classic }
    }
}
switch $winSys {
    x11 {
	option add *Font		"Helvetica -12"
	option add *selectBackground	#447bcd
	option add *selectForeground	white
    }
    classic {
	option add *background		#dedede
    }
}

#
# Create, configure, and populate 8 tablelist widgets
#
frame .f
for {set n 0} { $n < 8} {incr n} {
    set tbl .f.tbl$n
    tablelist::tablelist $tbl \
    	-columns {0 "Label 0"  0 "Label 1"  0 "Label 2"  0 "Label 3"} \
	-background gray98 -height 4 -width 40 -stretch all

    switch $n {
	1 {
	    $tbl configure -showseparators yes
	}
	2 {
	    $tbl configure -stripebackground #e0e8f0
	}
	3 {
	    $tbl configure -stripebackground #e0e8f0 -showseparators yes
	}
	4 {
	    foreach col {1 3} {
		$tbl columnconfigure $col -background linen
	    }
	}
	5 {
	    $tbl configure -showseparators yes
	    foreach col {1 3} {
		$tbl columnconfigure $col -background linen
	    }
	}
	6 {
	    $tbl configure -stripebackground #e0e8f0
	    foreach col {1 3} {
		$tbl columnconfigure $col -background linen
	    }
	}
	7 {
	    $tbl configure -stripebackground #e0e8f0 -showseparators yes
	    foreach col {1 3} {
		$tbl columnconfigure $col -background linen
	    }
	}
    }

    foreach row {0 1 2 3} {
	$tbl insert end \
	     [list "Cell $row,0" "Cell $row,1" "Cell $row,2" "Cell $row,3"]
    }
}

button .close -text "Close" -command exit
frame .bottom -height 10

#
# Manage the widgets
#
grid .f.tbl0 .f.tbl1 -sticky news -padx 5 -pady 5
grid .f.tbl2 .f.tbl3 -sticky news -padx 5 -pady 5
grid .f.tbl4 .f.tbl5 -sticky news -padx 5 -pady 5
grid .f.tbl6 .f.tbl7 -sticky news -padx 5 -pady 5
grid rowconfigure    .f {0 1 2 3} -weight 1
grid columnconfigure .f {0 1}     -weight 1
pack .bottom .close -side bottom
pack .f -side top -expand yes -fill both -padx 5 -pady 5
