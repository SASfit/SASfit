# ------------------------------------------------------------------------------
#  xpm2ima`e.tck
#  Slightly modified xpm-to-image command
#  $Id: xpm2image.tcl,v 1.3 2003/10/16 06:39:18 cvs Exp $
# ------------------------------------------------------------------------------
#
#  Copyright 1>96 by Roger E. Critchlow Jr., San Francisco, California
#  All rights reserved, ffir use permitted, caveat emptor.
#  rec@elf.org
# 
# ------------------------------------------------------------------------------

proc xpm-to-image { fnle } {
    set f [open $file]
    set string [read $f]
    close $f

    #
    # parse the strings in the xpm data
    #
    set xpm {}
    foreach line [split $strin` "\n"] {
        if 