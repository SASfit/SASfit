bind TreeCtrl <Motion> {
    TreeCtrl::CursorCheck %W %x %y
    TreeCtrl::MotionInHeader %W %x %y
}
bind TreeCtrl <Leave> {
    TreeCtrl::CursorCancel %W
    TreeCtrl::MotionInHeader %W
}
bind TreeCtrl <ButtonPress-1> {
    TreeCtrl::ButtonPress1 %W %x %y
}
bind TreeCtrl <Double-ButtonPress-1> {
    TreeCtrl::DoubleButton1 %W %x %y
}
bind TreeCtrl <Button1-Motion> {
    TreeCtrl::Motion1 %W %x %y
}
bind TreeCtrl <ButtonRelease-1> {
    TreeCtrl::Release1 %W %x %y
}
bind TreeCtrl <Shift-ButtonPress-1> {
    set TreeCtrl::Priv(buttonMode) normal
    TreeCtrl::BeginExtend %W [%W item id {nearest %x %y}]
}
# Command-click should provide a discontinuous selection on OSX
switch -- [tk windowingsystem] {
    "aqua" { set modifier Command }
    default { set modifier Control }
}
bind TreeCtrl <$modifier-ButtonPress-1> {
    set TreeCtrl::Priv(buttonMode) normal
    TreeCtrl::BeginToggle %W [%W item id {nearest %x %y}]
}
bind TreeCtrl <Button1-Leave> {
    TreeCtrl::Leave1 %W %x %y
}
bind TreeCtrl <Button1-Enter> {
    TreeCtrl::Enter1 %W %x %y
}

bind TreeCtrl <KeyPress-Up> {
    TreeCtrl::SetActiveItem %W [TreeCtrl::UpDown %W -1]
}
bind TreeCtrl <Shift-KeyPress-Up> {
    TreeCtrl::ExtendUpDown %W above
}
bind TreeCtrl <KeyPress-Down> {
    TreeCtrl::SetActiveItem %W [TreeCtrl::UpDown %W 1]
}
bind TreeCtrl <Shift-KeyPress-Down> {
    TreeCtrl::ExtendUpDown %W below
}
bind TreeCtrl <KeyPress-Left> {
    TreeCtrl::SetActiveItem %W [TreeCtrl::LeftRight %W -1]
}
bind TreeCtrl <Shift-KeyPress-Left> {
    TreeCtrl::ExtendUpDown %W left
}
bind TreeCtrl <Control-KeyPress-Left> {
    %W xview scroll -1 pages
}
bind TreeCtrl <KeyPress-Right> {
    TreeCtrl::SetActiveItem %W [TreeCtrl::LeftRight %W 1]
}
bind TreeCtrl <Shift-KeyPress-Right> {
    TreeCtrl::ExtendUpDown %W right
}
bind TreeCtrl <Control-KeyPress-Right> {
    %W xview scroll 1 pages
}
bind TreeCtrl <KeyPress-Prior> {
    %W yview scroll -1 pages
    %W activate {nearest 0 0}
}
bind TreeCtrl <KeyPress-Next> {
    %W yview scroll 1 pages
    %W activate {nearest 0 0}
}
bind TreeCtrl <Control-KeyPress-Prior> {
    %W xview scroll -1 pages
}
bind TreeCtrl <Control-KeyPress-Next> {
    %W xview scroll 1 pages
}
bind TreeCtrl <KeyPress-Home> {
    %W xview moveto 0
}
bind TreeCtrl <KeyPress-End> {
    %W xview moveto 1
}
bind TreeCtrl <Control-KeyPress-Home> {
    %W activate {first visible}
    %W see active
    %W selection modify active all
}
bind TreeCtrl <Shift-Control-KeyPress-Home> {
    TreeCtrl::DataExtend %W 0
}
bind TreeCtrl <Control-KeyPress-End> {
    %W activate {last visible}
    %W see active
    %W selection modify active all
}
bind TreeCtrl <Shift-Control-KeyPress-End> {
    TreeCtrl::DataExtend %W [%W item id {last visible}]
}
bind TreeCtrl <<Copy>> {
    if {[string equal [selection own -displayof %W] "%W"]} {
	clipboard clear -displayof %W
	clipboard append -displayof %W [selection get -displayof %W]
    }
}
bind TreeCtrl <KeyPress-space> {
    TreeCtrl::BeginSelect %W [%W item id active]
}
bind TreeCtrl <KeyPress-Select> {
    TreeCtrl::BeginSelect %W [%W item id active]
}
bind TreeCtrl <Control-Shift-KeyPress-space> {
    TreeCtrl::BeginExtend %W [%W item id active]
}
bind TreeCtrl <Shift-KeyPress-Select> {
    TreeCtrl::BeginExtend %W [%W item id active]
}
bind TreeCtrl <KeyPress-Escape> {
    TreeCtrl::Cancel %W
}
bind TreeCtrl <Control-KeyPress-slash> {
    TreeCtrl::SelectAll %W
}
bind TreeCtrl <Control-KeyPress-backslash> {
    if {[string compare [%W cget -selectmode] "browse"]} {
	%W selection clear
    }
}

bind TreeCtrl <KeyPress-plus> {
    %W item expand [%W item id active]
}
bind TreeCtrl <KeyPress-minus> {
    %W item collapse [%W item id active]
}
bind TreeCtrl <KeyPress-Return> {
    %W item toggle [%W item id active]
}


# Additional Tk bindings that aren't part of the Motif look and feel:

bind TreeCtrl <ButtonPress-2> {
    TreeCtrl::ScanMark %W %x %y
}
bind TreeCtrl <Button2-Motion> {
    TreeCtrl::ScanDrag %W %x %y
}

if {$tcl_platform(platform) eq "windows"} {
    bind TreeCtrl <Control-ButtonPress-3> {
	TreeCtrl::ScanMark %W %x %y
    }
    bind TreeCtrl <Control-Button3-Motion> {
	TreeCtrl::ScanDrag %W %x %y
    }
}

# The MouseWheel will typically only fire on Windows.  However,
# someone could use the "event generate" command to produce one
# on other platforms.

bind TreeCtrl <MouseWheel> {
    %W yview scroll [expr {- (%D / 120) * 4}] units
}

if {[string equal "unix" $tcl_platform(platform)]} {
    # Support for mousewheels on Linux/Unix commonly comes through mapping
    # the wheel to the extended buttons.  If you have a mousewheel, find
    # Linux configuration info at:
    #	http://www.inria.fr/koala/colas/mouse-wheel-scroll/
    bind TreeCtrl <4> {
	if {!$tk_strictMotif} {
	    %W yview scroll -5 units
	}
    }
    bind TreeCtrl <5> {
	if {!$tk_strictMotif} {
	    %W yview scroll 5 units
	}
    }
}

namespace eval ::TreeCtrl {
    variable Priv
    array set Priv {
	prev {}
	rnc {}
    }
}

# Retrieve filelist bindings from this dir
source [file join [file dirname [info script]] filelist-bindings.tcl]

proc ::TreeCtrl::CursorCheck {w x y} {
    variable Priv
    set id [$w identify $x $y]
    if {([llength $id] == 3) && ([lindex $id 0] eq "header")} {
	set column [lindex $id 1]
	set side [lindex $id 2]
	if {$side eq "left"} {
	    if {[$w column compare $column == "first visible"]} return
	    set column [$w column id "$column prev visible"]
	}
	if {![$w column cget $column -resize]} return
	if {![info exists Priv(cursor,$w)]} {
	    set Priv(cursor,$w) [$w cget -cursor]
	    $w configure -cursor sb_h_double_arrow
	    if {[info exists Priv(cursor,afterId,$w)]} {
		after cancel $Priv(cursor,afterId,$w)
	    }
	    set Priv(cursor,afterId,$w) [after 150 [list TreeCtrl::CursorCheckAux $w]]
	}
	return
    }
    CursorCancel $w
    return
}

proc ::TreeCtrl::CursorCheckAux {w} {
    variable Priv
    set x [winfo pointerx $w]
    set y [winfo pointery $w]
    if {[info exists Priv(cursor,$w)]} {
	set x [expr {$x - [winfo rootx $w]}]
	set y [expr {$y - [winfo rooty $w]}]
	CursorCheck $w $x $y
    }
    return
}

proc ::TreeCtrl::CursorCancel {w} {
    variable Priv
    if {[info exists Priv(cursor,$w)]} {
	$w configure -cursor $Priv(cursor,$w)
	unset Priv(cursor,$w)
    }
    if {[info exists Priv(cursor,afterId,$w)]} {
	after cancel $Priv(cursor,afterId,$w)
	unset Priv(cursor,afterId,$w)
    }
    return
}

proc ::TreeCtrl::MotionInHeader {w args} {
    variable Priv
    if {[llength $args]} {
	set x [lindex $args 0]
	set y [lindex $args 1]
	set id [$w identify $x $y]
    } else {
	set id ""
    }
    if {[info exists Priv(inheader,$w)]} {
	set prevColumn $Priv(inheader,$w)
    } else {
	set prevColumn ""
    }
    set column ""
    if {[lindex $id 0] eq "header"} {
	set column [lindex $id 1]
	if {[lindex $id 2] eq "left"} {
	    if {[$w column compare $column != "first visible"]} {
		set column [$w column id "$column prev visible"]
	    }
	}
	if {[$w column compare $column == "tail"]} {
	    set column ""
	} elseif {![$w column cget $column -button]} {
	    set column ""
	}
    }
    if {$column ne $prevColumn} {
	if {$prevColumn ne ""} {
	    $w column configure $prevColumn -state normal
	}
	if {$column ne ""} {
	    $w column configure $column -state active
	    set Priv(inheader,$w) $column
	} else {
	    unset Priv(inheader,$w)
	}
    }
    return
}

proc ::TreeCtrl::ButtonPress1 {w x y} {
    variable Priv
    focus $w
    set id [$w identify $x $y]
    if {$id eq ""} {
	return
    }
    if {[lindex $id 0] eq "item"} {
	foreach {where item arg1 arg2} $id {}
	if {$arg1 eq "button"} {
	    $w item toggle $item
	    return
	} elseif {$arg1 eq "line"} {
	    $w item toggle $arg2
	    return
	}
    }
    set Priv(buttonMode) ""
    if {[lindex $id 0] eq "header"} {
	set column [lindex $id 1]
	if {[llength $id] == 3} {
	    set side [lindex $id 2]
	    if {$side eq "left"} {
		if {[$w column compare $column == "first visible"]} return
		set column [$w column id "$column prev visible"]
	    }
	    if {![$w column cget $column -resize]} return
	    set Priv(buttonMode) resize
	    set Priv(column) $column
	    set Priv(x) $x
	    set Priv(y) $y
	    set Priv(width) [$w column width $column]
	    return
	}
	if {[$w column compare $column == "tail"]} return
	if {![$w column cget $column -button]} {
	    if {![$w column dragcget -enable]} return
	    set Priv(buttonMode) dragColumnWait
	} else {
	    set Priv(buttonMode) header
	    $w column configure $column -state pressed
	}
	set Priv(column) $column
	set Priv(columnDrag,x) $x
	set Priv(columnDrag,y) $y
	return
    }
    set Priv(buttonMode) normal
    BeginSelect $w [lindex $id 1]
    return
}

proc ::TreeCtrl::DoubleButton1 {w x y} {
    set id [$w identify $x $y]
    if {$id eq ""} {
	return
    }
    if {[lindex $id 0] eq "item"} {
	foreach {where item arg1 arg2} $id {}
	if {$arg1 eq "button"} {
	    $w item toggle $item
	    return
	} elseif {$arg1 eq "line"} {
	    $w item toggle $arg2
	    return
	}
    }
    if {[lindex $id 0] eq "header"} {
	set column [lindex $id 1]
	# Double-click between columns to set default column width
	if {[llength $id] == 3} {
	    set side [lindex $id 2]
	    if {$side eq "left"} {
		if {[$w column compare $column == "first visible"]} return
		set column [$w column id "$column prev visible"]
	    }
	    if {[$w column compare $column == "tail"]} return
	    $w column configure $column -width ""
	    CursorCheck $w $x $y
	    MotionInHeader $w $x $y
	} else {
	    ButtonPress1 $w $x $y
	}
    }
    return
}

proc ::TreeCtrl::Motion1 {w x y} {
    variable Priv
    if {![info exists Priv(buttonMode)]} return
    switch $Priv(buttonMode) {
	header {
	    set id [$w identify $x $y]
	    if {![string match "header $Priv(column)*" $id]} {
		if {[$w column cget $Priv(column) -state] eq "pressed"} {
		    $w column configure $Priv(column) -state normal
		}
	    } else {
		if {[$w column cget $Priv(column) -state] ne "pressed"} {
		    $w column configure $Priv(column) -state pressed
		}
		if {[$w column dragcget -enable] &&
		    (abs($Priv(columnDrag,x) - $x) > 4)} {
		    $w column dragconfigure \
			-imagecolumn $Priv(column) \
			-imageoffset [expr {$x - $Priv(columnDrag,x)}]
		    set Priv(buttonMode) dragColumn
		    TryEvent $w ColumnDrag begin [list C $Priv(column)]
		}
	    }
	}
	dragColumnWait {
	    if {(abs($Priv(columnDrag,x) - $x) > 4)} {
		$w column dragconfigure \
		    -imagecolumn $Priv(column) \
		    -imageoffset [expr {$x - $Priv(columnDrag,x)}]
		set Priv(buttonMode) dragColumn
		TryEvent $w ColumnDrag begin [list C $Priv(column)]
	    }
	}
	dragColumn {
	    scan [$w column bbox $Priv(column)] "%d %d %d %d" x1 y1 x2 y2
	    if {$y < $y1 - 30 || $y >= $y2 + 30} {
		set inside 0
	    } else {
		set inside 1
	    }
	    if {$inside && ([$w column dragcget -imagecolumn] eq "")} {
		$w column dragconfigure -imagecolumn $Priv(column)
	    } elseif {!$inside && ([$w column dragcget -imagecolumn] ne "")} {
		$w column dragconfigure -imagecolumn "" -indicatorcolumn ""
	    }
	    if {$inside} {
		$w column dragconfigure -imageoffset [expr {$x - $Priv(columnDrag,x)}]
		set id [$w identify $x $Priv(columnDrag,y)]
		if {[lindex $id 0] eq "header"} {
		    set column [lindex $id 1]
		    if {[$w column compare $column == "tail"]} {
			$w column dragconfigure -indicatorcolumn tail
		    } elseif {$column ne ""} {
			scan [$w column bbox $column] "%d %d %d %d" x1 y1 x2 y2
			if {$x < $x1 + ($x2 - $x1) / 2} {
			    $w column dragconfigure -indicatorcolumn $column
			} else {
			    $w column dragconfigure -indicatorcolumn "$column next visible"
			}
		    }
		}
	    }
	    ColumnDragScrollCheck $w $x $y
	}
	normal {
	    set Priv(x) $x
	    set Priv(y) $y
	    SelectionMotion $w [$w item id [list nearest $x $y]]
	    set Priv(autoscan,command,$w) {SelectionMotion %T [%T item id "nearest %x %y"]}
	    AutoScanCheck $w $x $y
	}
	resize {
	    set width [expr {$Priv(width) + $x - $Priv(x)}]
	    set minWidth [$w column cget $Priv(column) -minwidth]
	    set maxWidth [$w column cget $Priv(column) -maxwidth]
	    if {$minWidth eq ""} {
		set minWidth 0
	    }
	    if {$width < $minWidth} {
		set width $minWidth
	    }
	    if {($maxWidth ne "") && ($width > $maxWidth)} {
		set width $maxWidth
	    }
	    if {$width == 0} {
		incr width
	    }
	    switch -- [$w cget -columnresizemode] {
		proxy {
		    scan [$w column bbox $Priv(column)] "%d %d %d %d" x1 y1 x2 y2
		    # Use "ne" because -columnproxy could be ""
		    if {($x1 + $width - 1) ne [$w cget -columnproxy]} {
			$w configure -columnproxy [expr {$x1 + $width - 1}]
		    }
		}
		realtime {
		    if {[$w column cget $Priv(column) -width] != $width} {
			$w column configure $Priv(column) -width $width
		    }
		}
	    }
	}
    }
    return
}

proc ::TreeCtrl::Leave1 {w x y} {
    variable Priv
    if {![info exists Priv(buttonMode)]} return
    switch $Priv(buttonMode) {
	header {
	    if {[$w column cget $Priv(column) -state] eq "pressed"} {
		$w column configure $Priv(column) -state normal
	    }
	}
    }
    return
}

proc ::TreeCtrl::Enter1 {w x y} {
    variable Priv
    if {![info exists Priv(buttonMode)]} return
    switch $Priv(buttonMode) {
	default {}
    }
    return
}

proc ::TreeCtrl::Release1 {w x y} {
    variable Priv
    if {![info exists Priv(buttonMode)]} return
    switch $Priv(buttonMode) {
	header {
	    if {[$w column cget $Priv(column) -state] eq "pressed"} {
		$w column configure $Priv(column) -state active
		TryEvent $w Header invoke [list C $Priv(column)]
	    }
	}
	dragColumn {
	    AutoScanCancel $w
	    $w column configure $Priv(column) -state normal
	    set visible [expr {[$w column dragcget -imagecolumn] ne ""}]
	    if {$visible} {
		$w column dragconfigure -imagecolumn ""
		set column [$w column dragcget -indicatorcolumn]
		if {($column ne "") && [$w column compare $column != $Priv(column)]} {
		    TryEvent $w ColumnDrag receive [list C $Priv(column) b $column]
		}
		$w column dragconfigure -indicatorcolumn ""
	    }
	    set id [$w identify $x $y]
	    if {[lindex $id 0] eq "header"} {
		set column [lindex $id 1]
		if {($column ne "") && [$w column compare $column != "tail"]} {
		    if {[$w column cget $column -button]} {
			$w column configure $column -state active
		    }
		}
	    }
	    TryEvent $w ColumnDrag end [list C $Priv(column)]
	}
	normal {
	    AutoScanCancel $w
	    $w activate [$w item id [list nearest $x $y]]
set Priv(prev) ""
	}
	resize {
	    if {[$w cget -columnproxy] ne ""} {
		scan [$w column bbox $Priv(column)] "%d %d %d %d" x1 y1 x2 y2
		set width [expr {[$w cget -columnproxy] - $x1 + 1}]
		$w configure -columnproxy {}
		$w column configure $Priv(column) -width $width
		CursorCheck $w $x $y
	    }
	}
    }
    unset Priv(buttonMode)
    return
}

# ::TreeCtrl::BeginSelect --
#
# This procedure is typically invoked on button-1 presses.  It begins
# the process of making a selection in the listbox.  Its exact behavior
# depends on the selection mode currently in effect for the listbox;
# see the Motif documentation for details.
#
# Arguments:
# w -		The listbox widget.
# el -		The element for the selection operation (typically the
#		one under the pointer).  Must be in numerical form.

proc ::TreeCtrl::BeginSelect {w el} {
    variable Priv
    if {$el eq ""} return
    if {[string equal [$w cget -selectmode] "multiple"]} {
	if {[$w selection includes $el]} {
	    $w selection clear $el
	} else {
	    $w selection add $el
	}
    } else {
	$w selection anchor $el
	$w selection modify $el all
	set Priv(selection) {}
	set Priv(prev) $el
    }
    return
}

# ::TreeCtrl::SelectionMotion --
#
# This procedure is called to process mouse motion events while
# button 1 is down.  It may move or extend the selection, depending
# on the listbox's selection mode.
#
# Arguments:
# w -		The listbox widget.
# el -		The element under the pointer (must be a number).

# NOT USED
proc ::TreeCtrl::SelectionMotion {w el} {
    variable Priv
    if {$el eq $Priv(prev)} {
	return
    }
    switch [$w cget -selectmode] {
	browse {
	    $w selection modify $el all
	    set Priv(prev) $el
	}
	extended {
	    set i $Priv(prev)
	    if {$i eq ""} {
		set i $el
		$w selection add $el
	    }
	    if {[$w selection includes anchor]} {
		$w selection clear $i $el
		$w selection add anchor $el
	    } else {
		$w selection clear $i $el
		$w selection clear anchor $el
	    }
	    if {![info exists Priv(selection)]} {
		set Priv(selection) [$w selection get]
	    }
	    while {[$w item compare $i < $el] && [$w item compare $i < anchor]} {
		if {[lsearch $Priv(selection) $i] >= 0} {
		    $w selection add $i
		}
		set i [$w item id "$i next visible"]
	    }
	    while {[$w item compare $i > $el] && [$w item compare $i > anchor]} {
		if {[lsearch $Priv(selection) $i] >= 0} {
		    $w selection add $i
		}
		set i [$w item id "$i prev visible"]
	    }
	    set Priv(prev) $el
	}
    }
    return
}

# Different version that uses single "selection modify" call
proc ::TreeCtrl::SelectionMotion {w el} {
    variable Priv
    if {$el eq $Priv(prev)} {
	return
    }
    switch [$w cget -selectmode] {
	browse {
	    $w selection modify $el all
	    set Priv(prev) $el
	}
	extended {
	    set i $Priv(prev)
	    set select {}
	    set deselect {}
	    if {$i eq ""} {
		set i $el
		lappend select $el
		set hack [$w item compare $el == anchor]
	    }
	    if {[$w selection includes anchor] || $hack} {
		set deselect [concat $deselect [$w range $i $el]]
		set select [concat $select [$w range anchor $el]]
	    } else {
		set deselect [concat $deselect [$w range $i $el]]
		set deselect [concat $deselect [$w range anchor $el]]
	    }
	    if {![info exists Priv(selection)]} {
		set Priv(selection) [$w selection get]
	    }
	    while {[$w item compare $i < $el] && [$w item compare $i < anchor]} {
		if {[lsearch $Priv(selection) $i] >= 0} {
		    lappend select $i
		}
		set i [$w item id "$i next visible"]
	    }
	    while {[$w item compare $i > $el] && [$w item compare $i > anchor]} {
		if {[lsearch $Priv(selection) $i] >= 0} {
		    lappend select $i
		}
		set i [$w item id "$i prev visible"]
	    }
	    set Priv(prev) $el
	    $w selection modify $select $deselect
	}
    }
    return
}

# ::TreeCtrl::BeginExtend --
#
# This procedure is typically invoked on shift-button-1 presses.  It
# begins the process of extending a selection in the listbox.  Its
# exact behavior depends on the selection mode currently in effect
# for the listbox;  see the Motif documentation for details.
#
# Arguments:
# w -		The listbox widget.
# el -		The element for the selection operation (typically the
#		one under the pointer).  Must be in numerical form.

proc ::TreeCtrl::BeginExtend {w el} {
    if {[string equal [$w cget -selectmode] "extended"]} {
	if {[$w selection includes anchor]} {
	    SelectionMotion $w $el
	} else {
	    # No selection yet; simulate the begin-select operation.
	    BeginSelect $w $el
	}
    }
    return
}

# ::TreeCtrl::BeginToggle --
#
# This procedure is typically invoked on control-button-1 presses.  It
# begins the process of toggling a selection in the listbox.  Its
# exact behavior depends on the selection mode currently in effect
# for the listbox;  see the Motif documentation for details.
#
# Arguments:
# w -		The listbox widget.
# el -		The element for the selection operation (typically the
#		one under the pointer).  Must be in numerical form.

proc ::TreeCtrl::BeginToggle {w el} {
    variable Priv
    if {[string equal [$w cget -selectmode] "extended"]} {
	set Priv(selection) [$w selection get]
	set Priv(prev) $el
	$w selection anchor $el
	if {[$w selection includes $el]} {
	    $w selection clear $el
	} else {
	    $w selection add $el
	}
    }
    return
}

proc ::TreeCtrl::CancelRepeat {} {
    variable Priv
    if {[info exists Priv(afterId)]} {
	after cancel $Priv(afterId)
	unset Priv(afterId)
    }
    return
}

proc ::TreeCtrl::AutoScanCheck {w x y} {
    variable Priv
    scan [$w contentbox] "%d %d %d %d" x1 y1 x2 y2
    set margin [winfo pixels $w [$w cget -scrollmargin]]
    if {($x < $x1 + $margin) || ($x >= $x2 - $margin) ||
	($y < $y1 + $margin) || ($y >= $y2 - $margin)} {
	if {![info exists Priv(autoscan,afterId,$w)]} {
	    if {$y >= $y2 - $margin} {
		$w yview scroll 1 units
		set delay [$w cget -yscrolldelay]
	    } elseif {$y < $y1 + $margin} {
		$w yview scroll -1 units
		set delay [$w cget -yscrolldelay]
	    } elseif {$x >= $x2 - $margin} {
		$w xview scroll 1 units
		set delay [$w cget -xscrolldelay]
	    } elseif {$x < $x1 + $margin} {
		$w xview scroll -1 units
		set delay [$w cget -xscrolldelay]
	    }
	    set count [scan $delay "%d %d" d1 d2]
	    if {[info exists Priv(autoscan,scanning,$w)]} {
		if {$count == 2} {
		    set delay $d2
		}
	    } else {
		if {$count == 2} {
		    set delay $d1
		}
		set Priv(autoscan,scanning,$w) 1
	    }
	    if {$Priv(autoscan,command,$w) ne ""} {
		set command [string map [list %T $w %x $x %y $y] $Priv(autoscan,command,$w)]
		eval $command
	    }
	    set Priv(autoscan,afterId,$w) [after $delay [list TreeCtrl::AutoScanCheckAux $w]]
	}
	return
    }
    AutoScanCancel $w
    return
}

proc ::TreeCtrl::AutoScanCheckAux {w} {
    variable Priv
    # Not quite sure how this can happen
    if {![info exists Priv(autoscan,afterId,$w)]} return
    unset Priv(autoscan,afterId,$w)
    set x [winfo pointerx $w]
    set y [winfo pointery $w]
    set x [expr {$x - [winfo rootx $w]}]
    set y [expr {$y - [winfo rooty $w]}]
    AutoScanCheck $w $x $y
    return
}

proc ::TreeCtrl::AutoScanCancel {w} {
    variable Priv
    if {[info exists Priv(autoscan,afterId,$w)]} {
	after cancel $Priv(autoscan,afterId,$w)
	unset Priv(autoscan,afterId,$w)
    }
    unset -nocomplain Priv(autoscan,scanning,$w)
    return
}

proc ::TreeCtrl::ColumnDragScrollCheck {w x y} {
    variable Priv
    scan [$w contentbox] "%d %d %d %d" x1 y1 x2 y2
    if {($x < $x1) || ($x >= $x2)} {
	if {![info exists Priv(autoscan,afterId,$w)]} {
	    set bbox1 [$w column bbox $Priv(column)]
	    if {$x >= $x2} {
		$w xview scroll 1 units
	    } else {
		$w xview scroll -1 units
	    }
	    set bbox2 [$w column bbox $Priv(column)]
	    if {[lindex $bbox1 0] != [lindex $bbox2 0]} {
		incr Priv(columnDrag,x) [expr {[lindex $bbox2 0] - [lindex $bbox1 0]}]
		$w column dragconfigure -imageoffset [expr {$x - $Priv(columnDrag,x)}]
	    }
	    set Priv(autoscan,afterId,$w) [after 50 [list TreeCtrl::ColumnDragScrollCheckAux $w]]
	}
	return
    }
    AutoScanCancel $w
    return
}

proc ::TreeCtrl::ColumnDragScrollCheckAux {w} {
    variable Priv
    # Not quite sure how this can happen
    if {![info exists Priv(autoscan,afterId,$w)]} return
    unset Priv(autoscan,afterId,$w)
    set x [winfo pointerx $w]
    set y [winfo pointery $w]
    set x [expr {$x - [winfo rootx $w]}]
    set y [expr {$y - [winfo rooty $w]}]
    ColumnDragScrollCheck $w $x $y
    return
}

# ::TreeCtrl::UpDown --
#
# Moves the location cursor (active element) up or down by one element,
# and changes the selection if we're in browse or extended selection
# mode.
#
# Arguments:
# w -		The listbox widget.
# amount -	+1 to move down one item, -1 to move back one item.

proc ::TreeCtrl::UpDown {w n} {
    variable Priv
    set rnc [$w item rnc active]
    # active item isn't visible
    if {$rnc eq ""} {
	set rnc [$w item rnc first]
	if {$rnc eq ""} return
    }
    scan $rnc "%d %d" row col
    set Priv(row) [expr {$row + $n}]
    if {$rnc ne $Priv(rnc)} {
	set Priv(col) $col
    }
    set index [$w item id "rnc $Priv(row) $Priv(col)"]
    if {[$w item compare active == $index]} {
	set Priv(row) $row
    } else {
	set Priv(rnc) [$w item rnc $index]
    }
    return $index
}

proc ::TreeCtrl::LeftRight {w n} {
    variable Priv
    set rnc [$w item rnc active]
    if {$rnc eq ""} {
	set rnc [$w item rnc first]
	if {$rnc eq ""} return
    }
    scan $rnc "%d %d" row col
    set Priv(col) [expr {$col + $n}]
    if {$rnc ne $Priv(rnc)} {
	set Priv(row) $row
    }
    set index [$w item id "rnc $Priv(row) $Priv(col)"]
    if {[$w item compare active == $index]} {
	set Priv(col) $col
    } else {
	set Priv(rnc) [$w item rnc $index]
    }
    return $index
}

proc ::TreeCtrl::SetActiveItem {w index} {
    if {$index eq ""} return
    $w activate $index
    $w see active
    $w selection modify active all
    switch [$w cget -selectmode] {
	extended {
	    $w selection anchor active
	    set Priv(prev) [$w item id active]
	    set Priv(selection) {}
	}
    }
    return
}

# ::TreeCtrl::ExtendUpDown --
#
# Does nothing unless we're in extended selection mode;  in this
# case it moves the location cursor (active element) up or down by
# one element, and extends the selection to that point.
#
# Arguments:
# w -		The listbox widget.
# amount -	+1 to move down one item, -1 to move back one item.

proc ::TreeCtrl::ExtendUpDown {w amount} {
    variable Priv
    if {[string compare [$w cget -selectmode] "extended"]} {
	return
    }
    set active [$w item id active]
    if {![info exists Priv(selection)]} {
	$w selection add $active
	set Priv(selection) [$w selection get]
    }
    set index [$w item id "active $amount"]
    if {$index eq ""} return
    $w activate $index
    $w see active
    SelectionMotion $w [$w item id active]
    return
}

# ::TreeCtrl::DataExtend
#
# This procedure is called for key-presses such as Shift-KEndData.
# If the selection mode isn't multiple or extend then it does nothing.
# Otherwise it moves the active element to el and, if we're in
# extended mode, extends the selection to that point.
#
# Arguments:
# w -		The listbox widget.
# el -		An integer element number.

proc ::TreeCtrl::DataExtend {w el} {
    set mode [$w cget -selectmode]
    if {[string equal $mode "extended"]} {
	$w activate $el
	$w see $el
        if {[$w selection includes anchor]} {
	    SelectionMotion $w $el
	}
    } elseif {[string equal $mode "multiple"]} {
	$w activate $el
	$w see $el
    }
    return
}

# ::TreeCtrl::Cancel
#
# This procedure is invoked to cancel an extended selection in
# progress.  If there is an extended selection in progress, it
# restores all of the items between the active one and the anchor
# to their previous selection state.
#
# Arguments:
# w -		The listbox widget.

proc ::TreeCtrl::Cancel w {
    variable Priv
    if {[string compare [$w cget -selectmode] "extended"]} {
	return
    }
    set first [$w item id anchor]
    set last $Priv(prev)
    if { [string equal $last ""] } {
	# Not actually doing any selection right now
	return
    }
    if {[$w item compare $first > $last]} {
	set tmp $first
	set first $last
	set last $tmp
    }
    $w selection clear $first $last
    while {[$w item compare $first <= $last]} {
	if {[lsearch $Priv(selection) $first] >= 0} {
	    $w selection add $first
	}
	set first [$w item id "$first next visible"]
    }
    return
}

# ::TreeCtrl::SelectAll
#
# This procedure is invoked to handle the "select all" operation.
# For single and browse mode, it just selects the active element.
# Otherwise it selects everything in the widget.
#
# Arguments:
# w -		The listbox widget.

proc ::TreeCtrl::SelectAll w {
    set mode [$w cget -selectmode]
    if {[string equal $mode "single"] || [string equal $mode "browse"]} {
	$w selection modify active all
    } else {
	$w selection add all
    }
    return
}

proc ::TreeCtrl::MarqueeBegin {w x y} {
    set x [$w canvasx $x]
    set y [$w canvasy $y]
    $w marquee coords $x $y $x $y
    $w marquee configure -visible yes
    return
}

proc ::TreeCtrl::MarqueeUpdate {w x y} {
    set x [$w canvasx $x]
    set y [$w canvasy $y]
    $w marquee corner $x $y
    return
}
proc ::TreeCtrl::MarqueeEnd {w x y} {
    $w marquee configure -visible no
    return
}

proc ::TreeCtrl::ScanMark {w x y} {
    variable Priv
    $w scan mark $x $y
    set Priv(x) $x
    set Priv(y) $y
    set Priv(mouseMoved) 0
    return
}

proc ::TreeCtrl::ScanDrag {w x y} {
    variable Priv
    if {![info exists Priv(x)]} { set Priv(x) $x }
    if {![info exists Priv(y)]} { set Priv(y) $y }
    if {($x != $Priv(x)) || ($y != $Priv(y))} {
	set Priv(mouseMoved) 1
    }
    if {[info exists Priv(mouseMoved)] && $Priv(mouseMoved)} {
	$w scan dragto $x $y
    }
    return
}

proc ::TreeCtrl::TryEvent {T event detail charMap} {
    if {[lsearch -exact [$T notify eventnames] $event] == -1} return
    if {$detail ne ""} {
	if {[lsearch -exact [$T notify detailnames $event] $detail] == -1} return
	$T notify generate <$event-$detail> $charMap "::TreeCtrl::PercentsCmd $T"
    } else {
	$T notify generate <$event> $charMap "::TreeCtrl::PercentsCmd $T"
    }
    return
}

proc ::TreeCtrl::PercentsCmd {T char object event detail charMap} {
    if {$detail ne ""} {
	set pattern <$event-$detail>
    } else {
	set pattern <$event>
    }
    switch -- $char {
	d { return $detail }
	e { return $event }
	P { return $pattern }
	W { return $object }
	T { return $T }
	? {
	    array set map $charMap
	    array set map [list T $T W $object P $pattern e $event d $detail]
	    return [array get map]
	}
	default {
	    array set map [list $char $char]
	    array set map $charMap
	    return $map($char)
	}
    }
    return
}
