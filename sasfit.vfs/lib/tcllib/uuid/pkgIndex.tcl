# pkgIndex.tcl - 
#
# uuid package index file
#
# $Id: pkgIndex.tcl,v 1.1 2006/05/02 12:32:22 kohlbrecher Exp $

if {![package vsatisfies [package provide Tcl] 8.2]} {return}
package ifneeded uuid 1.0.1 [list source [file join $dir uuid.tcl]]
