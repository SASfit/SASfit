part::create tcl \
    -buildcommand {
        upvar c c

        set tcl_version [part::srcversion [part::srcdir tcl] 1]

        conf::norelnoacplatformconfigure [part::srcdir tcl] --disable-shared [conf::c_disen thread threads] [conf::c_disen debug symbols]
        conf::make binaries
        conf::make libraries
        conf::make install-binaries
        conf::make install-libraries
        if {$::tcl_platform(platform)=="windows"} {
            utils::withfile [file join [part::destdir tcl] tclConfig.sh] fc {
                if {[regexp "\nTCL_VERSION='(.*?)'" $fc {} tclver]} {
                    regsub "\nTCL_VERSION" $fc "\nNODOT_VERSION='[string map [list . {}] $tclver]'\nTCL_VERSION" fc
                }
            }
        }
        # copy private headers
        foreach g [glob -nocomplain -directory [part::srcdir tcl] generic/tcl*.h] {
            set d [file join $::conf::installdir include [file tail $g]]
            file copy -force $g $d
        }
    } \
    -definescommand {
        upvar c c
        return [list -DKIT_INCLUDES_TCL]
    } \
    -librarycommand {
        upvar c c
        set rc [concat [list "[part::destdir tcl]/$c(TCL_LIB_FILE)"] $c(TCL_LIBS)]
        if {$::tcl_platform(platform)=="windows"} {
            lappend rc [lindex [glob -directory [part::destdir tcl] tcldde*.a] 0]
            lappend rc [lindex [glob -directory [part::destdir tcl] tclreg*.a] 0]
        }
        return $rc
    } \
    -options {debug 0 thread 0} \
    -config tclConfig.sh \
    -appinit {[part::srcdir tcl]/[utils::platformdir]/tclAppInit.c} \
    -core 1 \
    -kitpatterns {lib/tcl@VER1@} \

