set thisdir [file dirname [info script]]

package ifneeded tkcon 2.4 [list source [file join $thisdir tkcon.tcl]]
