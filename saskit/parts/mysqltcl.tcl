# part: mysqltcl {{{
part::create mysqltcl \
    -buildcommand {
        conf::loadconf [part::configfile tcl] cc cc
        set dir [part::srcdir mysqltcl]
        out::oexec [concat [list gcc -DUSE_TCL_STUBS -c -o mysqltcl.a -I$dir -I[file join [part::srcdir tcl] generic] -I/usr/include/mysql [file join [part::srcdir mysqltcl] mysqltcl.c]]]
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"mysqltcl\", Mysqltcl_Init, Mysqltcl_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Mysqltcl_Init, Mysqltcl_SafeInit;\n"
    } \
    -librarycommand {
        set dir [part::srcdir mysqltcl]
        set ver [part::srcversion $dir]
        set rc [list]
        #lappend rc [file join [part::destdir mysqltcl] [conf::libname mysqltcl $ver]]
        lappend rc [part::destdir zlib]/libz.a /usr/lib/libmysqlclient.a
        lappend rc [file join [part::destdir mysqltcl] mysqltcl.a]
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/mysqltcl} \

# }}}

