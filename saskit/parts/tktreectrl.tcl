part::create tktreectrl \
    -buildcommand {
        conf::norelmainconfigure [part::srcdir tktreectrl] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols] --enable-gcc
        if {$::tcl_platform(platform)=="windows"} {
            utils::withfile [file join [part::destdir tktreectrl] Makefile] fc {
    	        regsub -all -line "(EXTRA_CFLAGS.*?=.*?) -YX" $fc "\\1" fc
    	        regsub -all -line "CFLAGS_OPTIMIZE.*?=.*\$" $fc "CFLAGS_OPTIMIZE = -O2" fc
	        regsub -all -line "CFLAGS_DEBUG.*?=.*\$" $fc "CFLAGS_DEBUG = -g" fc
	        regsub -all -line "lib.*?-nologo" $fc "ar cr" fc
	        regsub -all -line "link.*?-nologo" $fc "ar cr" fc
	        regsub -all -line "\\.lib" $fc ".a" fc
            }
        }
	conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Treectrl\", Treectrl_Init, Treectrl_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Treectrl_Init, Treectrl_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tktreectrl]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tktreectrl] [conf::libname treectrl $ver 1 0]]
        return $rc
    } \
    -depend {tcl tk} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tktreectrl@VER1@} \


