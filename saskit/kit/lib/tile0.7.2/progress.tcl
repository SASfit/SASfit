#
# progress.tcl,v 1.3 2005/05/11 07:04:06 muonics Exp
#
# Tile widget set: progress bar utilities.
#

namespace eval tile::progressbar {
    variable Timers	;# Map: widget name -> after ID
}

# Autoincrement --
#	Periodic callback procedure for autoincrement mode
#
proc tile::progressbar::Autoincrement {pb steptime stepsize} {
    variable Timers

    if {![winfo exists $pb]} {
    	# widget has been destroyed -- cancel timer
	unset -nocomplain Timers($pb)
	return
    }

    $pb step $stepsize

    set Timers($pb) [after $steptime \
    	[list tile::progressbar::Autoincrement $pb $steptime $stepsize] ]
}

# tile::progressbar::start --
#	Start autoincrement mode.
#
proc tile::progressbar::start {pb {steptime 50} {stepsize 1}} {
    variable Timers
    if {![info exists Timers($pb)]} {
	Autoincrement $pb $steptime $stepsize
    }
}

# tile::progressbar::stop --
#	Cancel autoincrement mode
#
proc tile::progressbar::stop {pb} {
    variable Timers
    if {[info exists Timers($pb)]} {
	after cancel $Timers($pb)
	unset Timers($pb)
    }
    $pb configure -value 0
}

