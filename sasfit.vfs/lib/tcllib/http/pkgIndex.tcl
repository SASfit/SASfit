# pkgIndex.tcl for the tcllib http module.
#
# $Id: pkgIndex.tcl,v 1.1 2006/05/02 08:24:39 kohlbrecher Exp $

if {![package vsatisfies [package provide Tcl] 8.2]} {return}
package ifneeded autoproxy 1.2.1 [list source [file join $dir autoproxy.tcl]]
