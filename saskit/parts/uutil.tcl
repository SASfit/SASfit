# part: uutil {{{
part::create uutil \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir uutil] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Uutil\", Uutil_Init, Uutil_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Uutil_Init, Uutil_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir uutil]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir uutil] [conf::libname uutil $ver]]
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/uutil@VER1@} \

# }}}

