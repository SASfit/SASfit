# entry.tcl --
#
# DERIVED FROM: tk/library/entry.tcl r1.22
#
# Copyright (c) 1992-1994 The Regents of the University of California.
# Copyright (c) 1994-1997 Sun Microsystems, Inc.
# Copyright (c) 2004, Joe English
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

namespace eval tile {
    namespace eval entry {
	variable State

	set State(x) 0
	set State(selectMode) char
	set State(anchor) 0
	set State(scanX) 0
	set State(scanIndex) 0
	set State(scanMoved) 0

	# Button-2 scan speed is (scanNum/scanDen) characters
	# per pixel of mouse movement.
	# The standard Tk entry widget uses the equivalent of
	# scanNum = 10, scanDen = average character width.
	# I don't know why that was chosen.
	#
	set State(scanNum) 1
	set State(scanDen) 1
	set State(deadband) 3	;# #pixels for mouse-moved deadband.
    }
}

### Bindings.
#
# Removed the following standard Tk bindings:
#
# <Control-Key-space>, <Control-Shift-Key-space>,
# <Key-Select>,  <Shift-Key-Select>:
#	Tile entry widget doesn't use selection anchor.
# <Key-Insert>:
#	Inserts PRIMARY selection (on non-Windows platforms).
#	This is inconsistent with typical platform bindings.
# <Double-Shift-ButtonPress-1>, <Triple-Shift-ButtonPress-1>:
#	These don't do the right thing to start with.
# <Meta-Key-b>, <Meta-Key-d>, <Meta-Key-f>,
# <Meta-Key-BackSpace>, <Meta-Key-Delete>:
#	Judgment call.  If <Meta> happens to be assigned to the Alt key,
#	these could conflict with application accelerators.
#	(Plus, who has a Meta key these days?)
# <Control-Key-t>:
#	Another judgment call.  If anyone misses this, let me know
#	and I'll put it back.
#

## Clipboard events:
#
bind TEntry <<Cut>> 			{ tile::entry::Cut %W }
bind TEntry <<Copy>> 			{ tile::entry::Copy %W }
bind TEntry <<Paste>> 			{ tile::entry::Paste %W }
bind TEntry <<Clear>> 			{ tile::entry::Clear %W }

## Button1 bindings:
#	Used for selection and navigation.
#
bind TEntry <ButtonPress-1> 		{ tile::entry::Press %W %x }
bind TEntry <Shift-ButtonPress-1>	{ tile::entry::Shift-Press %W %x }
bind TEntry <Double-ButtonPress-1> 	{ tile::entry::Select %W %x word }
bind TEntry <Triple-ButtonPress-1> 	{ tile::entry::Select %W %x line }
bind TEntry <B1-Motion>			{ tile::entry::Drag %W %x }

bind TEntry <B1-Leave> 		{ tile::Repeatedly tile::entry::AutoScroll %W }
bind TEntry <B1-Enter>		{ tile::CancelRepeat }
bind TEntry <ButtonRelease-1>	{ tile::CancelRepeat }

bind TEntry <Control-ButtonPress-1> {
    %W instate {!readonly !disabled} { %W icursor @%x ; focus %W }
}

## Button2 bindings:
#	Used for scanning and primary transfer.
#	Note: ButtonRelease-2 is mapped to <<PasteSelection>> in tk.tcl.
#
bind TEntry <ButtonPress-2> 		{ tile::entry::ScanMark %W %x }
bind TEntry <B2-Motion> 		{ tile::entry::ScanDrag %W %x }
bind TEntry <ButtonRelease-2>		{ tile::entry::ScanRelease %W %x }
bind TEntry <<PasteSelection>>		{ tile::entry::ScanRelease %W %x }

## Keyboard navigation bindings:
#
bind TEntry <Key-Left> 			{ tile::entry::Move %W prevchar }
bind TEntry <Key-Right> 		{ tile::entry::Move %W nextchar }
bind TEntry <Control-Key-Left>		{ tile::entry::Move %W prevword }
bind TEntry <Control-Key-Right>		{ tile::entry::Move %W nextword }
bind TEntry <Key-Home>			{ tile::entry::Move %W home }
bind TEntry <Key-End>			{ tile::entry::Move %W end }

bind TEntry <Shift-Key-Left> 		{ tile::entry::Extend %W prevchar }
bind TEntry <Shift-Key-Right>		{ tile::entry::Extend %W nextchar }
bind TEntry <Shift-Control-Key-Left>	{ tile::entry::Extend %W prevword }
bind TEntry <Shift-Control-Key-Right>	{ tile::entry::Extend %W nextword }
bind TEntry <Shift-Key-Home>		{ tile::entry::Extend %W home }
bind TEntry <Shift-Key-End>		{ tile::entry::Extend %W end }

bind TEntry <Control-Key-slash> 	{ %W selection range 0 end }
bind TEntry <Control-Key-backslash> 	{ %W selection clear }

bind TEntry <<TraverseIn>> 	{ %W selection range 0 end; %W icursor end }

## Edit bindings:
#
bind TEntry <KeyPress> 			{ tile::entry::Insert %W %A }
bind TEntry <Key-Delete>		{ tile::entry::Delete %W }
bind TEntry <Key-BackSpace> 		{ tile::entry::Backspace %W }

# Ignore all Alt, Meta, and Control keypresses unless explicitly bound.
# Otherwise, the <KeyPress> class binding will fire and insert the character.
# Ditto for Escape, Return, and Tab.
#
bind TEntry <Alt-KeyPress>		{# nothing}
bind TEntry <Meta-KeyPress>		{# nothing}
bind TEntry <Control-KeyPress> 		{# nothing}
bind TEntry <Key-Escape> 		{# nothing}
bind TEntry <Key-Return> 		{# nothing}
bind TEntry <Key-KP_Enter> 		{# nothing}
bind TEntry <Key-Tab> 			{# nothing}

# Argh.  Apparently on Windows, the NumLock modifier is interpreted
# as a Command modifier.
if {[tk windowingsystem] eq "aqua"} {
    bind TEntry <Command-KeyPress>	{# nothing}
}

## Additional emacs-like bindings:
#
bind TEntry <Control-Key-a>		{ tile::entry::Move %W home }
bind TEntry <Control-Key-b>		{ tile::entry::Move %W prevchar }
bind TEntry <Control-Key-d> 		{ tile::entry::Delete %W }
bind TEntry <Control-Key-e> 		{ tile::entry::Move %W end }
bind TEntry <Control-Key-f> 		{ tile::entry::Move %W nextchar }
bind TEntry <Control-Key-h>		{ tile::entry::Backspace %W }
bind TEntry <Control-Key-k>		{ %W delete insert end }

### Clipboard procedures.
#

## EntrySelection -- Return the selected text of the entry.
#	Raises an error if there is no selection.
#
proc tile::entry::EntrySelection {w} {
    set entryString [string range [$w get] [$w index sel.first] \
	    [expr {[$w index sel.last] - 1}]]
    if {[$w cget -show] ne ""} {
	return [string repeat [string index [$w cget -show] 0] \
		[string length $entryString]]
    }
    return $entryString
}

## Paste -- Insert clipboard contents at current insert point.
#
proc tile::entry::Paste {w} {
    catch {
	set clipboard [::tk::GetSelection $w CLIPBOARD]
	PendingDelete $w
	$w insert insert $clipboard
	See $w insert
    }
}

## Copy -- Copy selection to clipboard.
#
proc tile::entry::Copy {w} {
    if {![catch {EntrySelection $w} selection]} {
	clipboard clear -displayof $w
	clipboard append -displayof $w $selection
    }
}

## Clear -- Delete the selection.
#
proc tile::entry::Clear {w} {
    catch { $w delete sel.first sel.last }
}

## Cut -- Copy selection to clipboard then delete it.
#
proc tile::entry::Cut {w} {
    Copy $w; Clear $w
}

### Navigation procedures.
#

## ClosestGap -- Find closest boundary between characters.
# 	Returns the index of the character just after the boundary.
#
proc tile::entry::ClosestGap {w x} {
    set pos [$w index @$x]
    set bbox [$w bbox $pos]
    if {$x - [lindex $bbox 0] > [lindex $bbox 2]/2} {
	incr pos
    }
    return $pos
}

## See $index -- Make sure that the character at $index is visible.
#
proc tile::entry::See {w {index insert}} {
    set c [$w index $index]
    # @@@ OR: check [$w index left] / [$w index right]
    if {$c < [$w index @0] || $c >= [$w index @[winfo width $w]]} {
	$w xview $c
    }
}

## NextWord -- Find the next word position.
#	Note: The "next word position" follows platform conventions:
#	either the next end-of-word position, or the start-of-word
#	position following the next end-of-word position.
#
set ::tile::entry::State(startNext) \
	[string equal $tcl_platform(platform) "windows"]

proc tile::entry::NextWord {w start} {
    variable State
    set pos [tcl_endOfWord [$w get] [$w index $start]]
    if {$pos >= 0 && $State(startNext)} {
	set pos [tcl_startOfNextWord [$w get] $pos]
    }
    if {$pos < 0} {
	return end
    }
    return $pos
}

## PrevWord -- Find the previous word position.
#
proc tile::entry::PrevWord {w start} {
    set pos [tcl_startOfPreviousWord [$w get] [$w index $start]]
    if {$pos < 0} {
	return 0
    }
    return $pos
}

## RelIndex -- Compute character/word/line-relative index.
#
proc tile::entry::RelIndex {w where {index insert}} {
    switch -- $where {
	prevchar	{ expr {[$w index $index] - 1} }
    	nextchar	{ expr {[$w index $index] + 1} }
	prevword	{ PrevWord $w $index }
	nextword	{ NextWord $w $index }
	home		{ return 0 }
	end		{ $w index end }
	default		{ error "Bad relative index $index" }
    }
}

## Move -- Move insert cursor to relative location.
#	Also clears the selection, if any, and makes sure
#	that the insert cursor is visible.
#
proc tile::entry::Move {w where} {
    $w icursor [RelIndex $w $where]
    $w selection clear
    See $w insert
}

### Selection procedures.
#

## ExtendTo -- Extend the selection to the specified index.
#
# The other end of the selection (the anchor) is determined as follows:
#
# (1) if there is no selection, the anchor is the insert cursor;
# (2) if the index is outside the selection, grow the selection;
# (3) if the insert cursor is at one end of the selection, anchor the other end
# (4) otherwise anchor the start of the selection
#
# The insert cursor is placed at the new end of the selection.
#
# Returns: selection anchor.
#
proc tile::entry::ExtendTo {w index} {
    set index [$w index $index]
    set insert [$w index insert]

    # Figure out selection anchor:
    if {![$w selection present]} {
    	set anchor $insert
    } else {
    	set selfirst [$w index sel.first]
	set sellast  [$w index sel.last]

	if {   ($index < $selfirst)
	    || ($insert == $selfirst && $index <= $sellast)
	} {
	    set anchor $sellast
	} else {
	    set anchor $selfirst
	}
    }

    # Extend selection:
    if {$anchor < $index} {
	$w selection range $anchor $index
    } else {
    	$w selection range $index $anchor
    }

    $w icursor $index
    return $anchor
}

## Extend -- Extend the selection to a relative position, show insert cursor
#
proc tile::entry::Extend {w where} {
    ExtendTo $w [RelIndex $w $where]
    See $w
}

### Button 1 binding procedures.
#
# Double-clicking followed by a drag enters "word-select" mode.
# Triple-clicking enters "line-select" mode.
#

## Press -- ButtonPress-1 binding.
#	Set the insertion cursor, claim the input focus, set up for
#	future drag operations.
#
proc tile::entry::Press {w x} {
    variable State

    $w icursor [ClosestGap $w $x]
    $w selection clear
    $w instate {!readonly !disabled} { focus $w }

    # Set up for future drag, double-click, or triple-click.
    set State(x) $x
    set State(selectMode) char
    set State(anchor) [$w index insert]
}

## Shift-Press -- Shift-ButtonPress-1 binding.
#	Extends the selection, sets anchor for future drag operations.
#
proc tile::entry::Shift-Press {w x} {
    variable State

    focus $w
    set anchor [ExtendTo $w @$x]

    set State(x) $x
    set State(selectMode) char
    set State(anchor) $anchor
}

## Select $w $x $mode -- Binding for double- and triple- clicks.
#	Selects a word or line (according to mode),
#	and sets the selection mode for subsequent drag operations.
#
proc tile::entry::Select {w x mode} {
    variable State
    set cur [ClosestGap $w $x]

    switch -- $mode {
    	word	{ WordSelect $w $cur $cur }
    	line	{ LineSelect $w $cur $cur }
	char	{ # no-op }
    }

    set State(anchor) $cur
    set State(selectMode) $mode
}

## Drag -- Button1 motion binding.
#
proc tile::entry::Drag {w x} {
    variable State
    set State(x) $x
    DragTo $w $x
}

## DragTo $w $x -- Extend selection to $x based on current selection mode.
#
proc tile::entry::DragTo {w x} {
    variable State

    set cur [ClosestGap $w $x]
    switch $State(selectMode) {
	char { CharSelect $w $State(anchor) $cur }
	word { WordSelect $w $State(anchor) $cur }
	line { LineSelect $w $State(anchor) $cur }
    }
}

## AutoScroll
#	Called repeatedly when the mouse is outside an entry window
#	with Button 1 down.  Scroll the window left or right,
#	depending on where the mouse is, and extend the selection
#	according to the current selection mode.
#
# TODO: AutoScroll should repeat faster (50ms) than normal autorepeat.
# TODO: Need a way for tile::Repeat scripts to cancel themselves.
#
proc tile::entry::AutoScroll {w} {
    variable State
    if {![winfo exists $w]} return
    set x $State(x)
    if {$x > [winfo width $w]} {
	$w xview scroll 2 units
	DragTo $w $x
    } elseif {$x < 0} {
	$w xview scroll -2 units
	DragTo $w $x
    }
}

## CharSelect -- select characters between index $from and $to
#
proc tile::entry::CharSelect {w from to} {
    if {$to <= $from} {
	$w selection range $to $from
    } else {
	$w selection range $from $to
    }
    $w icursor $to
}

## WordSelect -- Select whole words between index $from and $to
#
proc tile::entry::WordSelect {w from to} {
    if {$to < $from} {
	set first [WordBack [$w get] $to]
	set last [WordForward [$w get] $from]
	$w icursor $first
    } else {
	set first [WordBack [$w get] $from]
	set last [WordForward [$w get] $to]
	$w icursor $last
    }
    $w selection range $first $last
}

## WordBack, WordForward -- helper routines for WordSelect.
#
proc WordBack {text index} {
    if {[set pos [tcl_wordBreakBefore $text $index]] < 0} { return 0 }
    return $pos
}
proc WordForward {text index} {
    if {[set pos [tcl_wordBreakAfter $text $index]] < 0} { return end }
    return $pos
}

## LineSelect -- Select the entire line.
#
proc tile::entry::LineSelect {w _ _} {
    variable State
    $w selection range 0 end
    $w icursor end
}

### Button 2 binding procedures.
#

## ScanMark -- ButtonPress-2 binding.
#	Marks the start of a scan or primary transfer operation.
#
proc tile::entry::ScanMark {w x} {
    variable State
    set State(scanX) $x
    set State(scanIndex) [$w index @0]
    set State(scanMoved) 0
}

## ScanDrag -- Button2 motion binding.
#
proc tile::entry::ScanDrag {w x} {
    variable State

    set dx [expr {$State(scanX) - $x}]
    if {abs($dx) > $State(deadband)} {
	set State(scanMoved) 1
    }
    set left [expr {$State(scanIndex) + ($dx*$State(scanNum))/$State(scanDen)}]
    $w xview $left

    if {$left != [set newLeft [$w index @0]]} {
    	# We've scanned past one end of the entry;
	# reset the mark so that the text will start dragging again
	# as soon as the mouse reverses direction.
	#
	set State(scanX) $x
	set State(scanIndex) $newLeft
    }
}

## ScanRelease -- Button2 release binding.
#	Do a primary transfer if the mouse has not moved since the button press.
#
proc tile::entry::ScanRelease {w x} {
    variable State
    if {!$State(scanMoved)} {
	$w instate {!disabled !readonly} {
	    $w icursor [ClosestGap $w $x]
	    catch {$w insert insert [::tk::GetSelection $w PRIMARY]}
	}
    }
}

### Insertion and deletion procedures.
#

## PendingDelete -- Delete selection prior to insert.
#	If the entry currently has a selection, delete it and
#	set the insert position to where the selection was.
#	Returns: 1 if pending delete occurred, 0 if nothing was selected.
#
proc tile::entry::PendingDelete {w} {
    if {[$w selection present]} {
	$w icursor sel.first
	$w delete sel.first sel.last
	return 1
    }
    return 0
}

## Insert -- Insert text into the entry widget.
#	If a selection is present, the new text replaces it.
#	Otherwise, the new text is inserted at the insert cursor.
#
proc tile::entry::Insert {w s} {
    if {$s eq ""} { return }
    PendingDelete $w
    $w insert insert $s
    See $w insert
}

## Backspace -- Backspace over the character just before the insert cursor.
#	If there is a selection, delete that instead.
#	If the new insert position is offscreen to the left,
#	scroll to place the cursor at about the middle of the window.
#
proc tile::entry::Backspace {w} {
    if {[PendingDelete $w]} {
    	See $w
	return
    }
    set x [expr {[$w index insert] - 1}]
    if {$x < 0} { return }

    $w delete $x

    if {[$w index @0] >= [$w index insert]} {
	set range [$w xview]
	set left [lindex $range 0]
	set right [lindex $range 1]
	$w xview moveto [expr {$left - ($right - $left)/2.0}]
    }
}

## Delete -- Delete the character after the insert cursor.
#	If there is a selection, delete that instead.
#
proc tile::entry::Delete {w} {
    if {![PendingDelete $w]} {
	$w delete insert
    }
}

### Compatibility procedures:
#
# The Tile entry bindings don't use the built-in selection anchor, 
# and scanning is implemented in Tcl instead of C, so the 
# [selection from], [selection to], and [selection adjust]
# widget commands aren't needed.
#
# However, other code (notably BWidget Spin and Combo boxes) still use 
# those commands (typically because it's trying to use Entry bindings on 
# a TEntry widget), so we implement them for compatibility.
# 
# Note that the selection commands don't have exactly the same behavior,
# but it's close enough.
#
proc tile::entry::compat {w cmd subcmd args} {
    switch -glob -- $cmd-$subcmd {
	sc*-m* { ScanMark $w [lindex $args 0]	;# $w scan mark $x }
	sc*-d* { ScanDrag $w [lindex $args 0]	;# $w scan dragto $x }
	se*-t* { 
	    # $w selection to $index
	    CharSelect $w [$w index anchor] [$w index [lindex $args 0]]
	}
	se*-a* { 
	    # $w selection adjust $index
	    ExtendTo $w [lindex $args 0]
	}
    }
}

#*EOF*
