# part: tclx {{{
part::create tclx \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tclx] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        # TODO: TCL_WIDE_INT_TYPE=long\\\\ long on Windows
	if {$::tcl_platform(os) == "SunOS"} {
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir tclx] Makefile] fc {
                regsub -all -line -- "-DHAVE_NET_ERRNO_H=1" $fc "" fc
            }
        }
        conf::make
    } -librarycommand {
        upvar c c
        set dir [part::srcdir tclx]
        set ver [part::srcversion $dir]
        set g [conf::c_if debug [conf::gsuffix] ""]
        set rc [list [part::destdir tclx]/[conf::x lib]tclx[part::dotversion $ver 1]$g.a]
	if {$::tcl_platform(platform) == "windows"} {
	    lappend rc -lwsock32
	}
	return $rc
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tclx\", Tclx_Init, Tclx_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tclx_Init, Tclx_SafeInit;\n"
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -core 0 \
    -kitpatterns {lib/tclx@VER1@} \

# }}}

