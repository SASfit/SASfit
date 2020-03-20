# part: blt {{{
proc part::_blt_libname {} {
    if {$::tcl_platform(platform)=="windows"} {
        set dir [part::srcdir blt]
        set ver [part::srcversion $dir]
        return libBLT[part::dotversion $ver].a
    }
    return libBLT.a
}
part::create blt \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir blt] --disable-shared [conf::c_norelwith tcl] \
	       [conf::c_norelwith tk] [conf::c_disen thread threads] \
	       [conf::c_if debug "--with-cflags=-g $::env(CFLAGS)" "--with-cflags=$::env(CFLAGS)"]
        utils::withfile [file join [part::destdir blt] src Makefile] fc {
            if {$::tcl_platform(platform)=="windows"} {
	        regsub -all -line "(DEFINES.*?=.*)\$" $fc "\\1 -DSTATIC_BUILD=1" fc
            }
	    set fc [string map [list "../../../noarch" "../../../../noarch"] $fc]
	    set fc [string map [list "../../../install" "../../../../install"] $fc]
	    set fc [string map [list "../../../../../noarch" "../../../../noarch"] $fc]
	    set fc [string map [list "../../../../../install" "../../../../install"] $fc]
        }
        conf::make -C src [_blt_libname]
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Blt\", Blt_Init, Blt_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Blt_Init, Blt_SafeInit;\n"
    } \
    -librarycommand {
        set rc [list]
        lappend rc [file join [part::destdir blt] src [_blt_libname]]
        if {$::tcl_platform(platform)=="windows"} {
            lappend rc -lwinspool -lgdi32 -lcomdlg32 -limm32 -lcomctl32 -lshell32 -lm
        }  else  {
            lappend rc -lX11
        }
        return $rc
    } \
    -depend {tcl tk itcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/blt@VER1@} \

# }}}

