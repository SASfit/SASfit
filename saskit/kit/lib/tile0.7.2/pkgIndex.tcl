if {[catch {package require Tcl 8.4}]} return
package ifneeded tile 0.7.2  [list load {} tile]
