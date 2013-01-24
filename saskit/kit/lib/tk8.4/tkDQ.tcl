# tkDQ.tcl --
#
# Initializes Data Quest based patches.

package require -exact Tk 8.4
package require -exact Tcl 8.4

set pfontname "helvetica"
set pfontsize -12
set ffontname "courier"
set ffontsize -12
set tmpvar0 ""
set tmpvar1 ""

switch -- $::tcl_platform(platform) {
    windows {
	foreach {tmpvar0 tmpvar1} {Tahoma 8 {MS Sans Serif} 8} {
	    if {[lsearch -exact [font families] $tmpvar0] >= 0} {
		set pfontname $tmpvar0
		set pfontsize $tmpvar1
		break
	    }
	}

	foreach {tmpvar0 tmpvar1} {{Lucida Console} 10 {Courier New} 10} {
	    if {[lsearch -exact [font families] $tmpvar0] >= 0} {
		set ffontname $tmpvar0
		set ffontsize $tmpvar1
		break
	    }
	}
    }
}

font create defaultProportional -family $pfontname -size $pfontsize
font create defaultProportionalBold -family $pfontname -size $pfontsize -weight bold
font create defaultProportionalItalic -family $pfontname -size $pfontsize -slant italic
font create defaultProportionalBoldItalic -family $pfontname -size $pfontsize -weight bold -slant italic

font create defaultFixed -family $ffontname -size $ffontsize
font create defaultFixedBold -family $ffontname -size $ffontsize -weight bold
font create defaultFixedItalic -family $ffontname -size $ffontsize -slant italic
font create defaultFixedBoldItalic -family $ffontname -size $ffontsize -weight bold -slant italic

unset tmpvar0
unset tmpvar1
unset pfontname
unset pfontsize
unset ffontname
unset ffontsize
