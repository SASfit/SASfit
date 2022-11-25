# -*- tcl -*- $Id: config_peg.tcl,v 1.1 2006/05/02 12:32:12 kohlbrecher Exp $

package provide page::config::peg 0.1

proc page_cdefinition {} {
    return {
	--reset
	--append
	--reader    peg
	--transform reachable
	--transform realizable
	--writer    me
    }
}
