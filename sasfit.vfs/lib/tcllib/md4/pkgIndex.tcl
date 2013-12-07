# pkgIndex.tcl - 
#
# md4 package index file
#
# This package has been tested with tcl 8.2.3 and above.
#
# $Id: pkgIndex.tcl,v 1.1 2006/05/02 08:24:41 kohlbrecher Exp $

if {![package vsatisfies [package provide Tcl] 8.2]} {return}
package ifneeded md4 1.0.3 [list source [file join $dir md4.tcl]]
