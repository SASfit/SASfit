# scale.tcl - Copyright (C) 2004 Pat Thoyts <patthoyts@users.sourceforge.net>
#
# Bindings for the TScale widget
#
# scale.tcl,v 1.8 2004/08/25 23:58:11 hobbs Exp

namespace eval tile::scale {
    variable State
    array set State  {
	dragging 0
    }
}

bind TScale <ButtonPress-1>   { tile::scale::Press %W %x %y }
bind TScale <B1-Motion>       { tile::scale::Drag %W %x %y }
bind TScale <ButtonRelease-1> { tile::scale::Release %W %x %y }

proc tile::scale::Press {w x y} {
    variable State
    set State(dragging) 0

    switch -glob -- [$w identify $x $y] {
        *trough {
	    if {[$w get $x $y] <= [$w get]} {
		tile::Repeatedly Increment $w -1
	    } else {
		tile::Repeatedly Increment $w 1
	    }
        }
        *slider {
            set State(dragging) 1
            set State(initial) [$w get]
        }
    }
}

proc tile::scale::Drag {w x y} {
    variable State
    if {$State(dragging)} {
	$w set [$w get $x $y]
    }
}

proc tile::scale::Release {w x y} {
    variable State
    set State(dragging) 0
    tile::CancelRepeat
}

proc tile::scale::Increment {w delta} {
    if {![winfo exists $w]} return
    $w set [expr {[$w get] + $delta}]
}
