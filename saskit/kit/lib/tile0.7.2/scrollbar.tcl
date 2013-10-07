#
# scrollbar.tcl,v 1.15 2005/07/18 14:53:26 jenglish Exp
#
# Bindings for TScrollbar widget
#

namespace eval tile::scrollbar {
    variable State
    # State(xPress)	--
    # State(yPress)	-- initial position of mouse at start of drag.
    # State(first)	-- value of -first at start of drag.
}

bind TScrollbar <ButtonPress-1> 	{ tile::scrollbar::Press %W %x %y }
bind TScrollbar <B1-Motion>		{ tile::scrollbar::Drag %W %x %y }
bind TScrollbar <ButtonRelease-1>	{ tile::scrollbar::Release %W %x %y }

bind TScrollbar <ButtonPress-2> 	{ tile::scrollbar::Jump %W %x %y }
bind TScrollbar <B2-Motion>		{ tile::scrollbar::Drag %W %x %y }
bind TScrollbar <ButtonRelease-2>	{ tile::scrollbar::Release %W %x %y }

proc tile::scrollbar::Scroll {w n units} {
    set cmd [$w cget -command]
    if {$cmd ne ""} {
	uplevel #0 $cmd scroll $n $units
    }
}

proc tile::scrollbar::Moveto {w fraction} {
    set cmd [$w cget -command]
    if {$cmd ne ""} {
	uplevel #0 $cmd moveto $fraction
    }
}

proc tile::scrollbar::Press {w x y} {
    variable State

    set State(xPress) $x
    set State(yPress) $y

    switch -glob -- [$w identify $x $y] {
    	*uparrow -
	*leftarrow {
	    tile::Repeatedly Scroll $w -1 units
	}
	*downarrow -
	*rightarrow {
	    tile::Repeatedly Scroll $w  1 units
	}
	*thumb {
	    set State(first) [lindex [$w get] 0]
	}
	*trough {
	    set f [$w fraction $x $y]
	    if {$f < [lindex [$w get] 0]} {
		# Clicked in upper/left trough
		tile::Repeatedly Scroll $w -1 pages
	    } elseif {$f > [lindex [$w get] 1]} {
		# Clicked in lower/right trough
		tile::Repeatedly Scroll $w 1 pages
	    } else {
		# Clicked on thumb (???)
		set State(first) [lindex [$w get] 0]
	    }
	}
    }
}

proc tile::scrollbar::Drag {w x y} {
    variable State
    if {![info exists State(first)]} {
    	# Initial buttonpress was not on the thumb, 
	# or something screwy has happened.  In either case, ignore:
	return;
    }
    set xDelta [expr {$x - $State(xPress)}]
    set yDelta [expr {$y - $State(yPress)}]
    Moveto $w [expr {$State(first) + [$w delta $xDelta $yDelta]}]
}

proc tile::scrollbar::Release {w x y} {
    variable State
    unset -nocomplain State(xPress) State(yPress) State(first)
    tile::CancelRepeat
}

# scrollbar::Jump -- ButtonPress-2 binding for scrollbars.
# 	Behaves exactly like scrollbar::Press, except that
#	clicking in the trough jumps to the the selected position.
#
proc tile::scrollbar::Jump {w x y} {
    variable State

    switch -glob -- [$w identify $x $y] {
	*thumb -
	*trough {
	    set State(first) [$w fraction $x $y]
	    Moveto $w $State(first)
	    set State(xPress) $x
	    set State(yPress) $y
	}
	default {
	    Press $w $x $y
	}
    }
}
