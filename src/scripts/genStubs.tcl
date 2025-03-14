# modified for use with SASfit by Ingo Bressler (ingo.bressler@bam.de)
# 2008-12-13
#
# genStubs.tcl --
#
#	This script generates a set of stub files for a given
#	interface.  
#	
#
# Copyright (c) 1998-1999 by Scriptics Corporation.
#
# "license.terms" file contents:
#
# This software is copyrighted by the Regents of the University of
# California, Sun Microsystems, Inc., Scriptics Corporation, ActiveState
# Corporation and other parties.  The following terms apply to all files
# associated with the software unless explicitly disclaimed in
# individual files.
# 
# The authors hereby grant permission to use, copy, modify, distribute,
# and license this software and its documentation for any purpose, provided
# that existing copyright notices are retained in all copies and that this
# notice is included verbatim in any distributions. No written agreement,
# license, or royalty fee is required for any of the authorized uses.
# Modifications to this software may be copyrighted by their authors
# and need not follow the licensing terms described here, provided that
# the new terms are clearly indicated on the first page of each file where
# they apply.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
# DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
# IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
# NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
# MODIFICATIONS.
# 
# GOVERNMENT USE: If you are acquiring this software on behalf of the
# U.S. government, the Government shall have only "Restricted Rights"
# in the software and related documentation as defined in the Federal 
# Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
# are acquiring the software on behalf of the Department of Defense, the
# software shall be classified as "Commercial Computer Software" and the
# Government shall have only "Restricted Rights" as defined in Clause
# 252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
# authors grant the U.S. Government and others acting in its behalf
# permission to use and distribute the software in accordance with the
# terms specified in this license. 
#

package require Tcl 8

namespace eval genStubs {
    # libraryName --
    #
    #	The name of the entire library.  This value is used to compute
    #	the USE_*_STUB_PROCS macro and the name of the init file.

    variable libraryName "UNKNOWN"

    # interfaces --
    #
    #	An array indexed by interface name that is used to maintain
    #   the set of valid interfaces.  The value is empty.

    array set interfaces {}

    # curName --
    #
    #	The name of the interface currently being defined.

    variable curName "UNKNOWN"

    # hooks --
    #
    #	An array indexed by interface name that contains the set of
    #	subinterfaces that should be defined for a given interface.

    array set hooks {}

    # stubs --
    #
    #	This three dimensional array is indexed first by interface name,
    #	second by platform name, and third by a numeric offset or the
    #	constant "lastNum".  The lastNum entry contains the largest
    #	numeric offset used for a given interface/platform combo.  Each
    #	numeric offset contains the C function specification that
    #	should be used for the given entry in the stub table.  The spec
    #	consists of a list in the form returned by parseDecl.

    array set stubs {}

    # outDir --
    #
    #	The directory where the generated files should be placed.

    variable outDir .
}

# genStubs::library --
#
#	This function is used in the declarations file to set the name
#	of the library that the interfaces are associated with (e.g. "tcl").
#	This value will be used to define the inline conditional macro.
#
# Arguments:
#	name	The library name.
#
# Results:
#	None.

proc genStubs::library {name} {
    variable libraryName $name
}

# genStubs::interface --
#
#	This function is used in the declarations file to set the name
#	of the interface currently being defined.
#
# Arguments:
#	name	The name of the interface.
#
# Results:
#	None.

proc genStubs::interface {name} {
    variable curName $name
    variable interfaces

    set interfaces($name) {}
    return
}

# genStubs::hooks --
#
#	This function defines the subinterface hooks for the current
#	interface.
#
# Arguments:
#	names	The ordered list of interfaces that are reachable through the
#		hook vector.
#
# Results:
#	None.

proc genStubs::hooks {names} {
    variable curName
    variable hooks

    set hooks($curName) $names
    return
}

# genStubs::declare --
#
#	This function is used in the declarations file to declare a new
#	interface entry.
#
# Arguments:
#	index		The index number of the interface.
#	platform	The platform the interface belongs to.  Should be one
#			of generic, win, unix, or mac, or macosx or aqua or x11.
#	decl		The C function declaration, or {} for an undefined
#			entry.
#
# Results:
#	None.

proc genStubs::declare {args} {
    variable stubs
    variable curName

    if {[llength $args] != 3} {
	puts stderr "wrong # args: declare $args"
    }
    lassign $args index platformList decl

    # Check for duplicate declarations, then add the declaration and
    # bump the lastNum counter if necessary.

    foreach platform $platformList {
	if {[info exists stubs($curName,$platform,$index)]} {
	    puts stderr "Duplicate entry: declare $args"
	}
    }
    regsub -all "\[ \t\n\]+" [string trim $decl] " " decl
    set decl [parseDecl $decl]

    foreach platform $platformList {
	if {$decl != ""} {
	    set stubs($curName,$platform,$index) $decl
	    if {![info exists stubs($curName,$platform,lastNum)] \
		    || ($index > $stubs($curName,$platform,lastNum))} {
		set stubs($curName,$platform,lastNum) $index
	    }
	}
    }
    return
}

# genStubs::rewriteFile --
#
#	This function replaces the machine generated portion of the
#	specified file with new contents.  It looks for the !BEGIN! and
#	!END! comments to determine where to place the new text.
#
# Arguments:
#	file	The name of the file to modify.
#	text	The new text to place in the file.
#
# Results:
#	None.

proc genStubs::rewriteFile {file text} {
    if {![file exists $file]} {
	puts stderr "Cannot find file: $file"
	return
    }
    set in [open ${file} r]
    set out [open ${file}.new w]

    while {![eof $in]} {
	set line [gets $in]
	if {[regexp {!BEGIN!} $line]} {
	    break
	}
	puts $out $line
    }
    puts $out "/* !BEGIN!: Do not edit below this line, see ${genStubs::libraryName}.decls for modifications. */"
    puts $out $text
    while {![eof $in]} {
	set line [gets $in]
	if {[regexp {!END!} $line]} {
	    break
	}
    }
    puts $out "/* !END!: Do not edit above this line, see ${genStubs::libraryName}.decls for modifications. */"
    puts -nonewline $out [read $in]
    close $in
    close $out
    file rename -force ${file}.new ${file}
    return
}

# genStubs::addPlatformGuard --
#
#	Wrap a string inside a platform #ifdef.
#
# Arguments:
#	plat	Platform to test.
#
# Results:
#	Returns the original text inside an appropriate #ifdef.

proc genStubs::addPlatformGuard {plat text} {
    switch $plat {
	win {
	    return "#ifdef __WIN32__\n${text}#endif /* __WIN32__ */\n"
	}
	unix {
	    return "#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */\n${text}#endif /* UNIX */\n"
	}		    
	mac {
	    return "#ifdef MAC_TCL\n${text}#endif /* MAC_TCL */\n"
	}
	macosx {
	    return "#ifdef MAC_OSX_TCL\n${text}#endif /* MAC_OSX_TCL */\n"
	}
	aqua {
	    return "#ifdef MAC_OSX_TK\n${text}#endif /* MAC_OSX_TK */\n"
	}
	x11 {
	    return "#if !(defined(__WIN32__) || defined(MAC_TCL) || defined(MAC_OSX_TK)) /* X11 */\n${text}#endif /* X11 */\n"
	}
    }
    return "$text"
}

# genStubs::emitSlots --
#
#	Generate the stub table slots for the given interface.  If there
#	are no generic slots, then one table is generated for each
#	platform, otherwise one table is generated for all platforms.
#
# Arguments:
#	name	The name of the interface being emitted.
#	textVar	The variable to use for output.
#
# Results:
#	None.

proc genStubs::emitSlots {name textVar} {
    variable stubs
    upvar $textVar text

    forAllStubs $name makeSlot 1 text {"\tvoid *reserved$i;\n"}
    return
}

# genStubs::parseDecl --
#
#	Parse a C function declaration into its component parts.
#
# Arguments:
#	decl	The function declaration.
#
# Results:
#	Returns a list of the form {returnType name args}.  The args
#	element consists of a list of type/name pairs, or a single
#	element "void".  If the function declaration is malformed
#	then an error is displayed and the return value is {}.

proc genStubs::parseDecl {decl} {
    if {![regexp {^(.*)\((.*)\)$} $decl all prefix args]} {
	puts stderr "Malformed declaration: $decl"
	return
    }
    set prefix [string trim $prefix]
    if {![regexp {^(.+[ ][*]*)([^ *]+)$} $prefix all rtype fname]} {
	puts stderr "Bad return type: $decl"
	return
    }
    set rtype [string trim $rtype]
    set argList {}
    foreach arg [split $args ,] {
	lappend argList [string trim $arg]
    }
#    if {![string compare [lindex $argList end] "..."]} {
#	if {[llength $argList] != 2} {
#	    puts stderr "Only one argument is allowed in varargs form: $decl"
#	}
#	set arg [parseArg [lindex $argList 0]]
#	if {$arg == "" || ([llength $arg] != 2)} {
#	    puts stderr "Bad argument: '[lindex $argList 0]' in '$decl'"
#	    return
#	}
#	set args [list TCL_VARARGS $arg]
#    } else {
	set args {}
	foreach arg $argList {
	    set argInfo [parseArg $arg]
	    if {![string compare $argInfo "void"] || ![string compare $argInfo "..."]} {
		lappend args $argInfo
		break
	    } elseif {[llength $argInfo] == 2 || [llength $argInfo] == 3} {
		lappend args $argInfo
	    } else {
		puts stderr "Bad argument: '$arg' in '$decl'"
		return
	    }
	}
 #   }
    return [list $rtype $fname $args]
}

# genStubs::parseArg --
#
#	This function parses a function argument into a type and name.
#
# Arguments:
#	arg	The argument to parse.
#
# Results:
#	Returns a list of type and name with an optional third array
#	indicator.  If the argument is malformed, returns "".

proc genStubs::parseArg {arg} {
    if {![regexp {^(.+[ ][*]*)([^][ *]+)(\[\])?$} $arg all type name array]} {
	if {$arg == "void"} {
	    return $arg
        } elseif {![string compare [string trim $arg] "..."]} {
            return [string trim $arg]
	} else {
	    return
	}
    }
    set result [list [string trim $type] $name]
    if {$array != ""} {
	lappend result $array
    }
    return $result
}

# genStubs::makeDecl --
#
#	Generate the prototype for a function.
#
# Arguments:
#	name	The interface name.
#	decl	The function declaration.
#	index	The slot index for this function.
#
# Results:
#	Returns the formatted declaration string.

proc genStubs::makeDecl {name decl index} {
    lassign $decl rtype fname args

    append text "/* $index */\n"
    set line "EXTERN $rtype"
    set count [expr {2 - ([string length $line] / 8)}]
    append line [string range "\t\t\t" 0 $count]
    set pad [expr {24 - [string length $line]}]
    if {$pad <= 0} {
	append line " "
	set pad 0
    }
    append line "$fname _ANSI_ARGS_("

    set arg1 [lindex $args 0]
    switch -exact $arg1 {
	void {
	    append line "(void)"
	}
	TCL_VARARGS {
	    set arg [lindex $args 1]
	    append line "TCL_VARARGS([lindex $arg 0],[lindex $arg 1])"
	}
	default {
	    set sep "("
	    foreach arg $args {
		append line $sep
		set next {}
		append next [lindex $arg 0] " " [lindex $arg 1] \
			[lindex $arg 2]
		if {[string length $line] + [string length $next] \
			+ $pad > 76} {
		    append text $line \n
		    set line "\t\t\t\t"
		    set pad 28
		}
		append line $next
		set sep ", "
	    }
	    append line ")"
	}
    }
    append text $line
    
    append text ");\n"
    return $text
}

# genStubs::makeMacro --
#
#	Generate the inline macro for a function.
#
# Arguments:
#	name	The interface name.
#	decl	The function declaration.
#	index	The slot index for this function.
#
# Results:
#	Returns the formatted macro definition.

proc genStubs::makeMacro {name decl index} {
    lassign $decl rtype fname args

    set lfname [string tolower [string index $fname 0]]
    append lfname [string range $fname 1 end]

    set text "#ifndef $fname\n#define $fname"
    set arg1 [lindex $args 0]
    set argList ""
    switch -exact $arg1 {
	void {
	    set argList "()"
	}
	TCL_VARARGS {
	}
	default {
	    set sep "("
	    foreach arg $args {
		append argList $sep [lindex $arg 1]
		set sep ", "
	    }
	    append argList ")"
	}
    }
    append text " \\\n\t([string toupper ${name}_stubs]()->$lfname)"
    append text " /* $index */\n#endif\n"
    return $text
}

# genStubs::makeStub --
#
#	Emits a stub function definition.
#
# Arguments:
#	name	The interface name.
#	decl	The function declaration.
#	index	The slot index for this function.
#
# Results:
#	Returns the formatted stub function definition.

proc genStubs::makeStub {name decl index} {
    lassign $decl rtype fname args

    set lfname [string tolower [string index $fname 0]]
    append lfname [string range $fname 1 end]

    append text "/* Slot $index */\n" $rtype "\n" $fname

    set arg1 [lindex $args 0]

    if {![string compare $arg1 "TCL_VARARGS"]} {
	lassign [lindex $args 1] type argName 
	append text " TCL_VARARGS_DEF($type,$argName)\n\{\n"
	append text "    " $type " var;\n    va_list argList;\n"
	if {[string compare $rtype "void"]} {
	    append text "    " $rtype " resultValue;\n"
	}
	append text "\n    var = (" $type ") TCL_VARARGS_START(" \
		$type "," $argName ",argList);\n\n    "
	if {[string compare $rtype "void"]} {
	    append text "resultValue = "
	}
	append text "(" $name "_stubs_ptr->" $lfname "VA)(var, argList);\n"
	append text "    va_end(argList);\n"
	if {[string compare $rtype "void"]} {
	    append text "return resultValue;\n"
	}
	append text "\}\n\n"
	return $text
    }

    if {![string compare $arg1 "void"]} {
	set argList "()"
	set argDecls ""
    } else {
	set argList ""
	set sep "("
	foreach arg $args {
	    append argList $sep [lindex $arg 1]
	    append argDecls "    " [lindex $arg 0] " " \
		    [lindex $arg 1] [lindex $arg 2] ";\n"
	    set sep ", "
	}
	append argList ")"
    }
    append text $argList "\n" $argDecls "{\n    "
    if {[string compare $rtype "void"]} {
	append text "return "
    }
    append text "(" $name "_stubs_ptr->" $lfname ")" $argList ";\n}\n\n"
    return $text
}

# genStubs::makeSlot --
#
#	Generate the stub table entry for a function.
#
# Arguments:
#	name	The interface name.
#	decl	The function declaration.
#	index	The slot index for this function.
#
# Results:
#	Returns the formatted table entry.

proc genStubs::makeSlot {name decl index} {
    lassign $decl rtype fname args

    set lfname [string tolower [string index $fname 0]]
    append lfname [string range $fname 1 end]

    set text "\t"
#    append text $rtype " (*" $lfname ") _ANSI_ARGS_("
    append text $rtype " (*" $lfname ") "

    set arg1 [lindex $args 0]
    switch -exact $arg1 {
	void {
	    append text "(void)"
	}
	TCL_VARARGS {
	    set arg [lindex $args 1]
	    append text "TCL_VARARGS([lindex $arg 0],[lindex $arg 1])"
	}
	default {
	    set sep "("
	    foreach arg $args {
		append text $sep [lindex $arg 0] " " [lindex $arg 1] \
			[lindex $arg 2]
		set sep ", "
	    }
	    append text ")"
	}
    }
    
#    append text "); /* $index */\n"
    append text "; /* $index */\n"
    return $text
}

# genStubs::makeInit --
#
#	Generate the prototype for a function.
#
# Arguments:
#	name	The interface name.
#	decl	The function declaration.
#	index	The slot index for this function.
#
# Results:
#	Returns the formatted declaration string.

proc genStubs::makeInit {name decl index} {
    append text "\t" [lindex $decl 1] ", /* " $index " */\n"
    return $text
}

# genStubs::forAllStubs --
#
#	This function iterates over all of the platforms and invokes
#	a callback for each slot.  The result of the callback is then
#	placed inside appropriate platform guards.
#
# Arguments:
#	name		The interface name.
#	slotProc	The proc to invoke to handle the slot.  It will
#			have the interface name, the declaration,  and
#			the index appended.
#	onAll		If 1, emit the skip string even if there are
#			definitions for one or more platforms.
#	textVar		The variable to use for output.
#	skipString	The string to emit if a slot is skipped.  This
#			string will be subst'ed in the loop so "$i" can
#			be used to substitute the index value.
#
# Results:
#	None.

proc genStubs::forAllStubs {name slotProc onAll textVar \
	{skipString {"/* Slot $i is reserved */\n"}}} {
    variable stubs
    upvar $textVar text

    set plats [array names stubs $name,*,lastNum]
    if {[info exists stubs($name,generic,lastNum)]} {
	# Emit integrated stubs block
	set lastNum -1
	foreach plat [array names stubs $name,*,lastNum] {
	    if {$stubs($plat) > $lastNum} {
		set lastNum $stubs($plat)
	    }
	}
	for {set i 0} {$i <= $lastNum} {incr i} {
	    set slots [array names stubs $name,*,$i]
	    set emit 0
	    if {[info exists stubs($name,generic,$i)]} {
		if {[llength $slots] > 1} {
		    puts stderr "platform entry duplicates generic entry: $i"
		}
		append text [$slotProc $name $stubs($name,generic,$i) $i]
		set emit 1
	    } elseif {[llength $slots] > 0} {
		foreach plat {unix win mac} {
		    if {[info exists stubs($name,$plat,$i)]} {
			append text [addPlatformGuard $plat \
				[$slotProc $name $stubs($name,$plat,$i) $i]]
			set emit 1
		    } elseif {$onAll} {
			append text [eval {addPlatformGuard $plat} $skipString]
			set emit 1
		    }
		}
                #
                # "aqua" and "macosx" and "x11" are special cases, 
                # since "macosx" always implies "unix" and "aqua", 
                # "macosx", so we need to be careful not to 
                # emit duplicate stubs entries for the two.
                #
		if {[info exists stubs($name,aqua,$i)]
                        && ![info exists stubs($name,macosx,$i)]} {
		    append text [addPlatformGuard aqua \
			    [$slotProc $name $stubs($name,aqua,$i) $i]]
		    set emit 1
		}
		if {[info exists stubs($name,macosx,$i)]
                        && ![info exists stubs($name,unix,$i)]} {
		    append text [addPlatformGuard macosx \
			    [$slotProc $name $stubs($name,macosx,$i) $i]]
		    set emit 1
		}
		if {[info exists stubs($name,x11,$i)]
                        && ![info exists stubs($name,unix,$i)]} {
		    append text [addPlatformGuard x11 \
			    [$slotProc $name $stubs($name,x11,$i) $i]]
		    set emit 1
		}
	    }
	    if {$emit == 0} {
		eval {append text} $skipString
	    }
	}
	
    } else {
	# Emit separate stubs blocks per platform
	foreach plat {unix win mac} {
	    if {[info exists stubs($name,$plat,lastNum)]} {
		set lastNum $stubs($name,$plat,lastNum)
		set temp {}
		for {set i 0} {$i <= $lastNum} {incr i} {
		    if {![info exists stubs($name,$plat,$i)]} {
			eval {append temp} $skipString
		    } else {
			append temp [$slotProc $name $stubs($name,$plat,$i) $i]
		    }
		}
		append text [addPlatformGuard $plat $temp]
	    }
	}
        # Again, make sure you don't duplicate entries for macosx & aqua.
	if {[info exists stubs($name,aqua,lastNum)]
                && ![info exists stubs($name,macosx,lastNum)]} {
	    set lastNum $stubs($name,aqua,lastNum)
	    set temp {}
	    for {set i 0} {$i <= $lastNum} {incr i} {
		if {![info exists stubs($name,aqua,$i)]} {
		    eval {append temp} $skipString
		} else {
			append temp [$slotProc $name $stubs($name,aqua,$i) $i]
		    }
		}
		append text [addPlatformGuard aqua $temp]
	    }
        # Again, make sure you don't duplicate entries for macosx & unix.
	if {[info exists stubs($name,macosx,lastNum)]
                && ![info exists stubs($name,unix,lastNum)]} {
	    set lastNum $stubs($name,macosx,lastNum)
	    set temp {}
	    for {set i 0} {$i <= $lastNum} {incr i} {
		if {![info exists stubs($name,macosx,$i)]} {
		    eval {append temp} $skipString
		} else {
			append temp [$slotProc $name $stubs($name,macosx,$i) $i]
		    }
		}
		append text [addPlatformGuard macosx $temp]
	    }
        # Again, make sure you don't duplicate entries for x11 & unix.
	if {[info exists stubs($name,x11,lastNum)]
                && ![info exists stubs($name,unix,lastNum)]} {
	    set lastNum $stubs($name,x11,lastNum)
	    set temp {}
	    for {set i 0} {$i <= $lastNum} {incr i} {
		if {![info exists stubs($name,x11,$i)]} {
		    eval {append temp} $skipString
		} else {
			append temp [$slotProc $name $stubs($name,x11,$i) $i]
		    }
		}
		append text [addPlatformGuard x11 $temp]
	    }
    }
}

# genStubs::emitDeclarations --
#
#	This function emits the function declarations for this interface.
#
# Arguments:
#	name	The interface name.
#	textVar	The variable to use for output.
#
# Results:
#	None.

proc genStubs::emitDeclarations {name textVar} {
    variable stubs
    upvar $textVar text

#    append text "\n/*\n * Exported function declarations:\n */\n\n"
#    forAllStubs $name makeDecl 0 text
    return
}

# genStubs::emitMacros --
#
#	This function emits the inline macros for an interface.
#
# Arguments:
#	name	The name of the interface being emitted.
#	textVar	The variable to use for output.
#
# Results:
#	None.

proc genStubs::emitMacros {name textVar} {
    variable stubs
    variable libraryName
    upvar $textVar text

    set upName [string toupper $libraryName]
#    append text "\n#if defined(USE_${upName}_STUBS) && !defined(USE_${upName}_STUB_PROCS)\n"
    append text "\n#if defined(MAKE_SASFIT_PLUGIN)\n"
    append text "\n/*\n * Inline function declarations:\n */\n\n"
    
    forAllStubs $name makeMacro 0 text

#    append text "\n#endif /* defined(USE_${upName}_STUBS) && !defined(USE_${upName}_STUB_PROCS) */\n"
    append text "\n#endif /* defined(MAKE_SASFIT_PLUGIN) */\n"
    return
}

# genStubs::emitHeader --
#
#	This function emits the body of the <name>Decls.h file for
#	the specified interface.
#
# Arguments:
#	name	The name of the interface being emitted.
#
# Results:
#	None.

proc genStubs::emitHeader {name} {
    variable outDir
    variable hooks

#    set capName [string toupper [string index $name 0]]
#    append capName [string range $name 1 end]
    set capName $name

    emitDeclarations $name text

    if {[info exists hooks($name)]} {
	append text "\ntypedef struct ${capName}_stub_hooks {\n"
	foreach hook $hooks($name) {
	    set capHook [string toupper [string index $hook 0]]
	    append capHook [string range $hook 1 end]
	    append text "    struct ${capHook}_stubs *${hook}_stubs;\n"
	}
	append text "} ${capName}_stub_hooks;\n"
    }
    append text "\ntypedef struct\n{\n"
#    append text "    int magic;\n"
#    append text "    struct ${capName}StubHooks *hooks;\n\n"

    emitSlots $name text

    append text "} ${capName}_stubs_t;\n"

#    append text "\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n"
#    append text "extern ${capName}Stubs *${name}StubsPtr;\n"
#    append text "#ifdef __cplusplus\n}\n#endif\n"

    emitMacros $name text

    rewriteFile [file join $outDir include ${name}_stubs.h] $text
    return
}

# genStubs::emitStubs --
#
#	This function emits the body of the <name>_stubs.c file for
#	the specified interface.
#
# Arguments:
#	name	The name of the interface being emitted.
#
# Results:
#	None.

proc genStubs::emitStubs {name} {
    variable outDir

    append text "\n/*\n * Exported stub functions:\n */\n\n"
    forAllStubs $name makeStub 0 text

    rewriteFile [file join $outDir ${name}.c] $text
    return    
}

# genStubs::emitInit --
#
#	Generate the table initializers for an interface.
#
# Arguments:
#	name		The name of the interface to initialize.
#	textVar		The variable to use for output.
#
# Results:
#	Returns the formatted output.

proc genStubs::emitInit {name textVar} {
    variable stubs
    variable hooks
    upvar $textVar text

#    set capName [string toupper [string index $name 0]]
#    append capName [string range $name 1 end]
    set capName $name

    if {[info exists hooks($name)]} {
 	append text "\nstatic ${capName}StubHooks ${name}StubHooks = \{\n"
	set sep "    "
	foreach sub $hooks($name) {
	    append text $sep "&${sub}Stubs"
	    set sep ",\n    "
	}
	append text "\n\};\n"
    }
    append text "\n${capName}_stubs_t ${name}_stubs = \{\n"
#    append text "    TCL_STUB_MAGIC,\n"
#    if {[info exists hooks($name)]} {
#	append text "    &${name}StubHooks,\n"
#    } else {
#	append text "    NULL,\n"
#    }
    
    forAllStubs $name makeInit 1 text {"\tNULL, /* $i */\n"}

    append text "\};\n"
    return
}

# genStubs::emitInits --
#
#	This function emits the body of the <library_name>.c file for
#	the specified interface.
#
# Arguments:
#	name	The name of the interface being emitted.
#
# Results:
#	None.

proc genStubs::emitInits {} {
    variable hooks
    variable outDir
    variable libraryName
    variable interfaces

    # Assuming that dependencies only go one level deep, we need to emit
    # all of the leaves first to avoid needing forward declarations.

    set leaves {}
    set roots {}
    foreach name [lsort [array names interfaces]] {
	if {[info exists hooks($name)]} {
	    lappend roots $name
	} else {
	    lappend leaves $name
	}
    }
    foreach name $leaves {
	emitInit $name text
    }
    foreach name $roots {
	emitInit $name text
    }

    rewriteFile [file join $outDir ${libraryName}.c] $text
}

# genStubs::init --
#
#	This is the main entry point.
#
# Arguments:
#	None.
#
# Results:
#	None.

proc genStubs::init {} {
    global argv argv0
    variable outDir
    variable interfaces

    if {[llength $argv] < 2} {
	puts stderr "usage: $argv0 outDir declFile ?declFile...?"
	exit 1
    }

    set outDir [lindex $argv 0]

    foreach file [lrange $argv 1 end] {
	source $file
    }

    foreach name [lsort [array names interfaces]] {
	puts "Emitting $name"
	emitHeader $name
    }

    emitInits
}

# lassign --
#
#	This function emulates the TclX lassign command.
#
# Arguments:
#	valueList	A list containing the values to be assigned.
#	args		The list of variables to be assigned.
#
# Results:
#	Returns any values that were not assigned to variables.

proc lassign {valueList args} {
  if {[llength $args] == 0} {
      error "wrong # args: lassign list varname ?varname..?"
  }

  uplevel [list foreach $args $valueList {break}]
  return [lrange $valueList [llength $args] end]
}

genStubs::init

