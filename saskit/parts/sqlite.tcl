# part: sqlite {{{
part::create sqlite \
    -buildcommand {
        set tcldir [utils::wdrelative [part::srcdir tcl]]
        set sqlitedir [file join [utils::wdrelative [part::srcdir sqlite]] src]
        set destdir [utils::wdrelative [part::destdir sqlite]]
        
        conf::norelnoacmainconfigure [part::srcdir sqlite] --disable-shared --with-tcl=[utils::wdrelative [part::srcdir tcl]] [conf::c_disen thread threads] [conf::c_disen debug symbols] \
            --enable-utf8 --enable-tempdb-in-ram
        if {($::tcl_platform(platform)=="unix")&&([conf::c_if thread 1 0])} {
            utils::withfile [file join [part::destdir sqlite] Makefile] fc {
                if {![regexp THREADSAFE $fc]} {
                    regsub -all "(TCC = .*?) -D" $fc "\\1 -DTHREADSAFE=1 -D" fc
                }
            }
        }
        conf::make libsqlite.la
        out::oexec [concat [list [conf::x gcc] -I$tcldir/generic -I$tcldir/[utils::platformdir] -I$sqlitedir -I$destdir \
            -DBUILD_tcl -DSTATIC_BUILD=1 \
            -c -o $destdir/tclsqlite.o $sqlitedir/tclsqlite.c]]
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Sqlite\", Sqlite_Init, Sqlite_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Sqlite_Init, Sqlite_SafeInit;\n"
    } \
    -librarycommand {
        set rc [list]
        lappend rc [part::destdir sqlite]/.libs/libsqlite.a [part::destdir sqlite]/tclsqlite.o
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/sqlite@VER@} \

# }}}

