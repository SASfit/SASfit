# part: sqlite3 {{{
part::create sqlite3 \
    -buildcommand {
        set tcldir [utils::wdrelative [part::srcdir tcl]]
        set sqlite3dir [file join [utils::wdrelative [part::srcdir sqlite3]] src]
        set destdir [utils::wdrelative [part::destdir sqlite3]]
        
        conf::norelnoacmainconfigure [part::srcdir sqlite3] --disable-shared --with-tcl=[utils::wdrelative [part::destdir tcl]] [conf::c_disen thread threadsafe] [conf::c_disen thread threads] [conf::c_disen debug symbols] \
            --enable-utf8 --enable-tempdb-in-ram
        if {($::tcl_platform(platform)=="unix")&&([conf::c_if thread 1 0])} {
            utils::withfile [file join [part::destdir sqlite3] Makefile] fc {
                if {[regexp THREADSAFE=0 $fc]} {
                    regsub -all "THREADSAFE=0" $fc "THREADSAFE=1" fc
                }  elseif {![regexp THREADSAFE=1 $fc]} {
                    regsub -all "(TCC = .*?) -D" $fc "\\1 -DTHREADSAFE=1 -D" fc
                }
            }
        }
        conf::make libsqlite3.la
        out::oexec [concat [list [conf::x gcc] -I$tcldir/generic -I$tcldir/[utils::platformdir] -I$sqlite3dir -I$destdir -DSQLITE_3_SUFFIX_ONLY=1 \
            -DBUILD_tcl -DSTATIC_BUILD=1 \
            -c -o $destdir/tclsqlite.o $sqlite3dir/tclsqlite.c]]
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Sqlite3\", Sqlite3_Init, Sqlite3_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Sqlite3_Init, Sqlite3_SafeInit;\n"
    } \
    -librarycommand {
        set rc [list]
        if {$::tcl_platform(os) == "SunOS"} {
            # from Makefile.linux-gcc:#TLIBS = -lrt    # fdatasync on Solaris 8
            lappend rc -lrt
        }
        lappend rc [part::destdir sqlite3]/.libs/libsqlite3.a [part::destdir sqlite3]/tclsqlite.o
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/sqlite@VER@} \

# }}}

