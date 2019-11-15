# part: itcl {{{
part::create itcl \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir itcl]/itcl --enable-64bit --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        if {$::tcl_platform(platform)=="windows"} {
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir itcl] Makefile] fc {
    	        regsub -all -line "(EXTRA_CFLAGS.*?=.*?) -YX" $fc "\\1" fc
    	        regsub -all -line "CFLAGS_OPTIMIZE.*?=.*\$" $fc "CFLAGS_OPTIMIZE = -O2" fc
	        regsub -all -line "CFLAGS_DEBUG.*?=.*\$" $fc "CFLAGS_DEBUG = -g" fc
# interferes on MinGW64/MSYS2
#	        regsub -all -line "^\\*\\.lib" $fc "*.a" fc
#	        regsub -all -line "(LIB_FILE.*?=.*?).lib" $fc "\\1.a" fc
                regsub -all -line "(LIB_FILE\\s*=\\s*)lib" $fc "\\1" fc
#	        regsub -all -line "^STLIB_LD.*?=.*?lib.*\$" $fc "STLIB_LD = ar cr" fc
            }
        }
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Itcl\", Itcl_Init, Itcl_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Itcl_Init, Itcl_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir itcl]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [part::destdir itcl]/[conf::x lib]itcl$ver[conf::c_if debug [conf::gsuffix] ""].a
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/itcl@VER1@} \

# }}}
# part: itk {{{
part::create itk \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir itcl]/itk --enable-64bit --disable-shared [conf::c_with tcl] [conf::c_with tk] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        if {$::tcl_platform(platform)=="windows"} {
	    out::printcommand "Converting Makefile"
            utils::withfile [file join [part::destdir itk] Makefile] fc {
    	        regsub -all -line "(EXTRA_CFLAGS.*?=.*?) -YX" $fc "\\1" fc
    	        regsub -all -line "CFLAGS_OPTIMIZE.*?=.*\$" $fc "CFLAGS_OPTIMIZE = -O2" fc
	        regsub -all -line "CFLAGS_DEBUG.*?=.*\$" $fc "CFLAGS_DEBUG = -MTd" fc
	        regsub -all -line "^\\*\\.lib" $fc "*.a" fc
	        regsub -all -line "(LIB_FILE.*?=.*?).lib" $fc "\\1.a" fc
	        regsub -all -line "^STLIB_LD.*?=.*?lib.*\$" $fc "STLIB_LD = ar cr" fc
            }
        }
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Itk\", Itk_Init, Itk_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Itk_Init, Itk_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir itcl]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [part::destdir itk]/[conf::x lib]itk[part::dotversion $ver][conf::c_if debug [conf::gsuffix] ""].a
        return $rc
    } \
    -depend {tcl tk itcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/itk@VER1@} \

# }}}

