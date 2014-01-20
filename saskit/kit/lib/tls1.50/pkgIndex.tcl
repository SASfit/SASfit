
# pkgIndex.tcl - 
#
#    A new manually generated "pkgIndex.tcl" file for tls to
#    replace the original which didn't include the commands from "tls.tcl".
#

set tlslibfile tls[info sharedlibextension]

package ifneeded tls 1.50 "[list load {} Tls] ; [list source [file join $dir tls.tcl]]"

