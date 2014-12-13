part::create tile \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tile] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
	if {$::tcl_platform(platform) == "windows"} {
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir tile] Makefile] fc {
    	        regsub -all -line "(EXTRA_CFLAGS.*?=.*?) -YX" $fc "\\1" fc
    	        regsub -all -line "^CFLAGS_OPTIMIZE.*?=.*\$" $fc "CFLAGS_OPTIMIZE = -O2" fc
	        regsub -all -line "^CFLAGS_DEBUG.*?=.*\$" $fc "CFLAGS_DEBUG = -MTd" fc
	        regsub -all -line "^\\*\\.lib" $fc "*.a" fc
	        regsub -all -line "(LIB_FILE.*?=.*?).lib" $fc "\\1.a" fc
	        regsub -all -line "^STLIB_LD.*?=.*?lib.*\$" $fc "STLIB_LD = ar cr" fc
            }
	}
	conf::make all
	conf::make install
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tile\", Tile_Init, NULL);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tile_Init;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tile]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tile] [conf::libname tile $ver end 0]]
        return $rc
    } \
    -depend {tcl tk} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tile@VER@} \


