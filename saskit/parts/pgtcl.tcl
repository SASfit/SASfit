# part: pgtcl {{{
part::create pgtcl \
    -buildcommand {
    	conf::norelnoacmainconfigure [part::srcdir pgtcl] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]
	conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Pgtcl\", Pgtcl_Init, Pgtcl_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Pgtcl_Init, Pgtcl_SafeInit;\n"
    } \
    -librarycommand {
    	set pglibdir [exec pg_config --libdir]
	set pglibfile [lindex [lsort -dictionary [glob -directory $pglibdir libpq*.a]] end]
        set dir [part::srcdir pgtcl]
        set ver [part::srcversion $dir]
        set rc [list]

	# handle standard parameters
        lappend rc $pglibfile [part::destdir pgtcl]/[conf::x lib]pgtcl[part::dotversion $ver][conf::c_if debug [conf::gsuffix] ""].a
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/pgtcl@VER@} \

# }}}

