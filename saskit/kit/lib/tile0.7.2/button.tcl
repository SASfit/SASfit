#
# button.tcl,v 1.9 2005/06/26 22:07:39 jenglish Exp
#
# Bindings for Buttons, Checkbuttons, and Radiobuttons.
#
# Notes: <Button1-Leave>, <Button1-Enter> only control the "pressed"
# state; widgets remain "active" if the pointer is dragged out.
# This doesn't seem to be conventional, but it's a nice way
# to provide extra feedback while the grab is active.
# (If the button is released off the widget, the grab deactivates and
# we get a <Leave> event then, which turns off the "active" state)
#
# Normally, <ButtonRelease> and <ButtonN-Enter/Leave> events are 
# delivered to the widget which received the initial <ButtonPress>
# event.  However, Tk [grab]s (#1223103) and menu interactions
# (#1222605) can interfere with this.  To guard against spurious
# <Button1-Enter> events, the <Button1-Enter> binding only sets
# the pressed state if the button is currently active.
#

namespace eval tile { namespace eval button { } }

bind TButton <Enter>		{ %W state active }
bind TButton <Leave>		{ %W state !active }
bind TButton <Key-space>	{ tile::button::activate %W }
bind TButton <<Invoke>> 	{ tile::button::activate %W }

bind TButton <ButtonPress-1> \
    { %W instate !disabled { tile::clickToFocus %W; %W state pressed } }
bind TButton <ButtonRelease-1> \
    { %W instate {pressed !disabled} { %W state !pressed; %W invoke } }
bind TButton <Button1-Leave> \
    { %W state !pressed }
bind TButton <Button1-Enter> \
    { %W instate {active !disabled} { %W state pressed } }

# Checkbuttons and Radiobuttons have the same bindings as Buttons:
#
tile::CopyBindings TButton TCheckbutton
tile::CopyBindings TButton TRadiobutton

# ...plus a few more:

bind TRadiobutton <KeyPress-Up> 	{ tile::button::RadioTraverse %W -1 }
bind TRadiobutton <KeyPress-Down> 	{ tile::button::RadioTraverse %W +1 }

# bind TCheckbutton <KeyPress-plus> { %W select }
# bind TCheckbutton <KeyPress-minus> { %W deselect }

# activate --
#	Simulate a button press: temporarily set the state to 'pressed',
#	then invoke the button.
#
proc tile::button::activate {w} {
    set oldState [$w state pressed]
    update idletasks; after 100
    $w state $oldState
    $w invoke
}

# RadioTraverse -- up/down keyboard traversal for radiobutton groups.
# 	Set focus to previous/next radiobutton in a group.
#	A radiobutton group consists of all the radiobuttons with
#	the same parent and -variable; this is a pretty good heuristic
#	that works most of the time.
#
proc tile::button::RadioTraverse {w dir} {
    set group [list]
    foreach sibling [winfo children [winfo parent $w]] {
    	if {   [winfo class $sibling] eq "TRadiobutton"
	    && [$sibling cget -variable] eq [$w cget -variable]
	    && ![$sibling instate disabled]
	} {
	   lappend group $sibling
	}
    }

    if {![llength $group]} {	 # Shouldn't happen, but can.
    	return
    }

    set pos [expr {([lsearch -exact $group $w] + $dir) % [llength $group]}]
    keynav::traverseTo [lindex $group $pos]
}
