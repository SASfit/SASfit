#
# combobox.tcl,v 1.28 2005/09/29 02:32:06 jenglish Exp
#
# Tile widget set: combobox bindings.
#
# Each combobox $cb has a child $cb.popdown, which contains
# a listbox $cb.popdown.l and a scrollbar.  The listbox -listvariable
# is set to a namespace variable, which is used to synchronize the
# combobox values with the listbox values.
#

namespace eval tile::combobox {
    variable Values	;# Values($cb) is -listvariable of listbox widget

    variable State
    set State(entryPress) 0
}

### Combobox bindings.
#
# Duplicate the Entry bindings, override if needed:
#

tile::CopyBindings TEntry TCombobox

bind TCombobox <KeyPress-Down> 		{ tile::combobox::Post %W }
bind TCombobox <KeyPress-Escape> 	{ tile::combobox::Unpost %W }

bind TCombobox <ButtonPress-1> 		{ tile::combobox::Press "" %W %x %y }
bind TCombobox <Shift-ButtonPress-1>	{ tile::combobox::Press "s" %W %x %y }
bind TCombobox <Double-ButtonPress-1> 	{ tile::combobox::Press "2" %W %x %y }
bind TCombobox <Triple-ButtonPress-1> 	{ tile::combobox::Press "3" %W %x %y }
bind TCombobox <B1-Motion>		{ tile::combobox::Drag %W %x }

bind TCombobox <MouseWheel> 	{ tile::combobox::Scroll %W [expr {%D/-120}] }
if {[tk windowingsystem] eq "x11"} {
    bind TCombobox <ButtonPress-4>	{ tile::combobox::Scroll %W -1 }
    bind TCombobox <ButtonPress-5>	{ tile::combobox::Scroll %W  1 }
}

bind TCombobox <<TraverseIn>> 		{ tile::combobox::TraverseIn %W }

### Combobox listbox bindings.
#
bind ComboboxListbox <ButtonPress-1> 	{ focus %W ; continue }
bind ComboboxListbox <ButtonRelease-1>	{ tile::combobox::LBSelected %W }
bind ComboboxListbox <KeyPress-Return>	{ tile::combobox::LBSelected %W }
bind ComboboxListbox <KeyPress-Escape>  { tile::combobox::LBCancel %W }
bind ComboboxListbox <KeyPress-Tab>	{ tile::combobox::LBTab %W next }
bind ComboboxListbox <<PrevWindow>>	{ tile::combobox::LBTab %W prev }
bind ComboboxListbox <Destroy>		{ tile::combobox::LBCleanup %W }
# Default behavior is to follow selection on mouseover
bind ComboboxListbox <Motion> {
    %W selection clear 0 end
    %W activate @%x,%y
    %W selection set @%x,%y
}

# The combobox has a global grab active when the listbox is posted,
# but on Windows that doesn't prevent the user from interacting
# with other applications. The listbox gets a <FocusOut> event
# when this happens.  Don't know how reliable this is:
#
bind ComboboxListbox <FocusOut>		{ tile::combobox::LBCancel %W }

### Option database settings.
#

if {[tk windowingsystem] eq "x11"} {
    option add *TCombobox*Listbox.background white
}

# The following ensures that the popdown listbox uses the same font 
# as the combobox entry field (at least for the standard Tile themes).
#
option add *TCombobox*Listbox.font TkTextFont

### Binding procedures.
#

## combobox::Press $mode $x $y --
#	ButtonPress binding for comboboxes.
#	Either post/unpost the listbox, or perform Entry widget binding,
#	depending on widget state and location of button press.
#
proc tile::combobox::Press {mode w x y} {
    variable State
    set State(entryPress) [expr {
	   [$w instate {!readonly !disabled}]
	&& [string match *textarea [$w identify $x $y]]
    }]

    if {$State(entryPress)} {
	focus $w
	switch -- $mode {
	    s 	{ tile::entry::Shift-Press $w $x 	; # Shift }
	    2	{ tile::entry::Select $w $x word 	; # Double click}
	    3	{ tile::entry::Select $w $x line 	; # Triple click }
	    ""	-
	    default { tile::entry::Press $w $x }
	}
    } else {
	TogglePost $w
    }
}

## combobox::Drag --
#	B1-Motion binding for comboboxes.
#	If the initial ButtonPress event was handled by Entry binding,
#	perform Entry widget drag binding; otherwise nothing.
#
proc tile::combobox::Drag {w x}  {
    variable State
    if {$State(entryPress)} {
	tile::entry::Drag $w $x
    }
}

## TraverseIn -- receive focus due to keyboard navigation
#	For editable comboboxes, set the selection and insert cursor.
#
proc tile::combobox::TraverseIn {w} {
    $w instate {!readonly !disabled} { 
	$w selection range 0 end
	$w icursor end
    }
}

## SelectEntry $cb $index -- 
#	Set the combobox selection in response to a user action.
#
proc tile::combobox::SelectEntry {cb index} {
    $cb current $index
    $cb selection range 0 end
    $cb icursor end
    event generate $cb <<ComboboxSelected>>
}

## Scroll -- Mousewheel binding
#
proc tile::combobox::Scroll {cb dir} {
    $cb instate disabled { return }
    set max [llength [$cb cget -values]]
    set current [$cb current]
    incr current $dir
    if {$max != 0 && $current == $current % $max} {
	SelectEntry $cb $current
    }
}

## LBSelected $lb -- Activation binding for listbox
#	Set the combobox value to the currently-selected listbox value
#	and unpost the listbox.
#
proc tile::combobox::LBSelected {lb} {
    set cb [LBMaster $lb]
    set selection [$lb curselection]
    Unpost $cb
    focus $cb
    if {[llength $selection] == 1} {
	SelectEntry $cb [lindex $selection 0]
    }
}

## LBCancel --
#	Unpost the listbox.
#
proc tile::combobox::LBCancel {lb} {
    Unpost [LBMaster $lb]
}

## LBTab --
#	Tab key binding for combobox listbox:  
#	Set the selection, and navigate to next/prev widget.
#
proc tile::combobox::LBTab {lb dir} {
    set cb [LBMaster $lb]
    switch -- $dir {
	next	{ set newFocus [tk_focusNext $cb] }
	prev	{ set newFocus [tk_focusPrev $cb] }
    }

    if {$newFocus ne ""} {
	LBSelected $lb
	# The [grab release] call in [Unpost] queues events that later 
	# re-set the focus.  [update] to make sure these get processed first:
	update
	keynav::traverseTo $newFocus
    }
}

## PopdownShell --
#	Returns the popdown shell widget associated with a combobox,
#	creating it if necessary.
#
proc tile::combobox::PopdownShell {cb} {
    if {![winfo exists $cb.popdown]} {
	set popdown [toplevel $cb.popdown -relief solid -bd 1]
	wm withdraw $popdown
	wm overrideredirect $popdown 1
	wm transient $popdown [winfo toplevel $cb]

	# XXX Until we have a proper native scrollbar on Aqua, use
	# XXX the regular Tk one
	if {[tk windowingsystem] eq "aqua"} {
	    scrollbar $popdown.sb -orient vertical \
		-command [list $popdown.l yview]
	} else {
	    ttk::scrollbar $popdown.sb -orient vertical \
		-command [list $popdown.l yview]
	}
	listbox $popdown.l \
	    -listvariable tile::combobox::Values($cb) \
	    -yscrollcommand [list $popdown.sb set] \
	    -exportselection false \
	    -selectmode browse \
	    -borderwidth 2 -relief flat \
	    -highlightthickness 0 \
	    -activestyle none \
	    ;

	bindtags $popdown.l \
	    [list $popdown.l ComboboxListbox Listbox $popdown all]

	grid $popdown.l $popdown.sb -sticky news
	grid columnconfigure $popdown 0 -weight 1
	grid rowconfigure $popdown 0 -weight 1
    }
    return $cb.popdown
}

## combobox::Post $cb --
#	Pop down the associated listbox.
#
proc tile::combobox::Post {cb} {
    variable State
    variable Values

    # Don't do anything if disabled:
    #
    $cb instate disabled { return }

    # Run -postcommand callback:
    #
    uplevel #0 [$cb cget -postcommand]

    # Combobox is in 'pressed' state while listbox posted:
    #
    $cb state pressed

    set popdown [PopdownShell $cb]
    set values [$cb cget -values]
    set current [$cb current]
    if {$current < 0} {
	set current 0 		;# no current entry, highlight first one
    }
    set Values($cb) $values
    $popdown.l selection clear 0 end
    $popdown.l selection set $current
    $popdown.l activate $current
    $popdown.l see $current
    # Should allow user to control listbox height
    set height [llength $values]
    if {$height > 10} {
	set height 10
    }
    $popdown.l configure -height $height
    update idletasks

    # Position listbox (@@@ factor with menubutton::PostPosition
    #
    set x [winfo rootx $cb]
    set y [winfo rooty $cb]
    set w [winfo width $cb]
    set h [winfo height $cb]
    if {[tk windowingsystem] eq "aqua"} {
	# Adjust for platform-specific bordering to ensure the box is
	# directly under actual 'entry square'
	set xoff 3
	set yoff 2
	incr x $xoff
	set w [expr {$w - $xoff*2}]
    } else {
	set yoff 0
    }

    set H [winfo reqheight $popdown]
    if {$y + $h + $H > [winfo screenheight $popdown]} {
	set Y [expr {$y - $H - $yoff}]
    } else {
	set Y [expr {$y + $h - $yoff}]
    }
    wm geometry $popdown ${w}x${H}+${x}+${Y}

    # Post the listbox:
    #
    wm deiconify $popdown
    raise $popdown
    # @@@ Workaround for TrackElementState bug:
    event generate $cb <ButtonRelease-1>
    # /@@@
    tile::globalGrab $cb
    focus $popdown.l
}

## combobox::Unpost $cb --
#	Unpost the listbox, restore focus to combobox widget.
#
proc tile::combobox::Unpost {cb} {
    $cb state !pressed
    tile::releaseGrab $cb
    if {[winfo exists $cb.popdown]} {
	wm withdraw $cb.popdown
    }
    focus $cb
}

## combobox::TogglePost $cb --
#	Post the listbox if unposted, unpost otherwise.
#
proc tile::combobox::TogglePost {cb} {
    if {[$cb instate pressed]} { Unpost $cb } { Post $cb }
}

## LBMaster $lb --
#	Return the combobox main widget that owns the listbox.
#
proc tile::combobox::LBMaster {lb} {
    winfo parent [winfo parent $lb]
}

## LBCleanup $lb --
#	<Destroy> binding for combobox listboxes.
#	Cleans up by unsetting the linked textvariable.
#
#	Note: we can't just use { unset [%W cget -listvariable] }
#	because the widget command is already gone when this binding fires).
#	[winfo parent] still works, fortunately.
#

proc tile::combobox::LBCleanup {lb} {
    variable Values
    unset Values([LBMaster $lb])
}

#*EOF*
