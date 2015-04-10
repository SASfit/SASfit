# part: tclcompiler {{{
part::create tclcompiler \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tclcompiler] --disable-shared --enable-static [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tclcompiler\", Tclcompiler_Init, NULL);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tclcompiler_Init;\n"
    } \
    -librarycommand {
        set dir [part::srcdir tclcompiler]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tclcompiler] [conf::libname tclcompiler $ver]]
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/tclcompiler@VER1@} \

# }}}

