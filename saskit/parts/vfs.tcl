# part: vfs {{{
part::create vfs \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir vfs] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads]
        # TODO: TCL_WIDE_INT_TYPE=long\\\\ long on Windows
        if {$::tcl_platform(platform)=="windows"} {
            utils::withfile [file join [part::destdir vfs] Makefile] fc {
                regsub -all " -DTCL_WIDE_INT_TYPE=long\\\\ long" $fc "" fc
            }
        }
        conf::make
    } -librarycommand {
        upvar c c
        set dir [part::srcdir vfs]
        set ver [part::srcversion $dir]
        return [list [part::destdir vfs]/[conf::x lib]vfs[part::dotversion $ver].a]
    } \
    -depend {tcl} \
    -options {debug 0 thread 0} \
    -core 1 \
    -kitpatterns {lib/vfs} \

# }}}

