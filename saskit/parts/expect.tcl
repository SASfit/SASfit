# part: expect {{{
part::create expect \
    -buildcommand {
        set clk [clock seconds]

        incr clk -1
        file mtime [part::srcdir expect]/configure.in $clk
        file mtime [part::srcdir expect]/Makefile.in $clk
        incr clk 1
        file mtime [part::srcdir expect]/configure $clk

        conf::norelnoacmainconfigure [part::srcdir expect] --disable-shared [conf::c_with tcl] --with-tclinclude=[part::srcdir tcl]/generic [conf::c_disen debug symbols]
        utils::withfile [file join [part::destdir expect] Makefile] fc {
            regsub -all -line -- {-DSTTY_BIN=\\".*?/stty\\"} $fc {-DSTTY_BIN=\\"stty\\"} fc
            # this is needed for 8.5
            if {[string equal [part::srcversion [part::srcdir tcl] 1] "8.5"]} {
                regsub -all -line -- {^(TCLHDIRDASHI +)=(.*)$} $fc "\\1=\\2 -I[part::srcdir tcl]/[utils::platformdir]" fc
            }
        }
        conf::make
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Expect\", Expect_Init, NULL);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Expect_Init;\n"
    } \
    -librarycommand {
        set dir [part::srcdir expect]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir expect] [conf::libname expect $ver]]
        switch -- $::tcl_platform(os) {
            Linux {
                lappend rc -lutil
            }
        }
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -kitpatterns {lib/expect@VER1@} \

# }}}

