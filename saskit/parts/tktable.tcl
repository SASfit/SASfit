part::create tktable \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tktable] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
	if {$::tcl_platform(platform) == "windows"} {
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir tktable] Makefile] fc {
    	        regsub -all -line "(EXTRA_CFLAGS.*?=.*?) -YX" $fc "\\1" fc
    	        regsub -all -line "^CFLAGS_OPTIMIZE.*?=.*\$" $fc "CFLAGS_OPTIMIZE = -O2" fc
	        regsub -all -line "CFLAGS_DEBUG.*?=.*\$" $fc "CFLAGS_DEBUG = -g" fc
	        regsub -all -line "^\\*\\.lib" $fc "*.a" fc
	        regsub -all -line "(LIB_FILE.*?=.*?).lib" $fc "\\1.a" fc
	        regsub -all -line "^STLIB_LD.*?=.*?lib.*\$" $fc "STLIB_LD = ar cr" fc
            }
	}
	set dfh [open [file join [part::destdir tktable] tkTable.tcl.h] w]
	set sfh [open [file join [part::srcdir tktable] library tkTable.tcl] r]
	fconfigure $sfh -translation auto
	fconfigure $dfh -translation lf
	while {[gets $sfh line]>=0} {
	    if {[regexp "^#" $line]} {continue}
	    if {[regexp "^\$" $line]} {continue}
	    set line [string map [list \\ \\\\ \" \\\"] $line]
	    puts $dfh "\"$line\\n\""
	}
	close $dfh
	close $sfh
	conf::make binaries
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tktable\", Tktable_Init, Tktable_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tktable_Init, Tktable_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tktable]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tktable] [conf::libname Tktable $ver]]
        return $rc
    } \
    -depend {tcl tk} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tktable@VER1@} \


