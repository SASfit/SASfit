if {[package vcompare [package provide Tcl] 8.5] != 0} { return }
package ifneeded Tk 8.5 [list load {} Tk]
