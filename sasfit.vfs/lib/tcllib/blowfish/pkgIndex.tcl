if {![package vsatisfies [package provide Tcl] 8.2]} {
    # PRAGMA: returnok
    return
}
package ifneeded blowfish 1.0.0 [list source [file join $dir blowfish.tcl]]
