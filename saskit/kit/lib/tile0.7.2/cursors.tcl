#
# cursors.tcl,v 1.1 2005/03/14 01:23:57 jenglish Exp
#
# Tile package: Symbolic cursor names.
#
# @@@ TODO: Figure out appropriate platform-specific cursors
#	for the various functions.
#

namespace eval tile {

    variable Cursors;

    # These are good for X11:
    #
    array set Cursors {
	hresize 	sb_h_double_arrow
	vresize 	sb_v_double_arrow
    }
}

#*EOF*
