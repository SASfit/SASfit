# part: tcludp {{{
part::create tcludp \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tcludp] --disable-shared [conf::c_with tcl] [conf::c_disen debug symbols]
        conf::make binaries
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Udp\", Udp_Init, Udp_Init);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Udp_Init;\n"
    } \
    -definescommand {
    } \
    -librarycommand {
        upvar c c
        set dir [part::srcdir tcludp]
        set ver [part::srcversion $dir]
        return [list [part::destdir tcludp]/[conf::libname udp $ver end 0]]
    } \
    -kitpatterns {lib/tcludp@VER1@} \
    -depend {tcl} \
    -options {debug 0}
# }}}

