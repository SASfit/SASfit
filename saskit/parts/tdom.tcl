# part: tdom {{{
part::create tdom \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tdom] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
	if {$::tcl_platform(platform) == "windows"} {
            set ver1 [part::srcversion [part::srcdir tdom] end]
            set ver0 [string map [list . ""] $ver1]
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir tdom] Makefile] fc {
		regsub -all -line -- "^(\\s*VERSION\\s*)=(\\s*)${ver0}(\\s*)\$" $fc "\\1=\\2${ver1}\\3" fc
		regsub -all -line -- "(-DVERSION=\\\\\")${ver0}(\\\\\")" $fc "\\1${ver1}\\2" fc
	    }
	}
	conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"tdom\", Tdom_Init, Tdom_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tdom_Init, Tdom_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tdom]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tdom] [conf::libname tdom $ver]]
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tdom@VER@} \

# }}}

