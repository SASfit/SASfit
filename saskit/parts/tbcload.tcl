# part: tbcload {{{
part::create tbcload \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tbcload] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tbcload\", Tbcload_Init, Tbcload_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tbcload_Init, Tbcload_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tbcload]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tbcload] [conf::libname tbcload $ver]]
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tbcload@VER1@} \

# }}}

