# part: openssl {{{
part::create openssl \
    -buildcommand {
        utils::sync [part::srcdir openssl] [part::destdir openssl] $::utils::defaultOmitFiles $::utils::defaultOmitDirs
        if {$::tcl_platform(platform)=="windows"} {
            set fh [open msmkmf.bat w]
            puts $fh "perl util\\mkfiles.pl >MINFO"
            puts $fh "perl util\\mk1mf.pl Mingw32 >ms\\mingw32.mak"
            puts $fh "perl util\\mk1mf.pl Mingw32-files >ms\\mingw32f.mak"
            puts $fh "perl util\\mkdef.pl 32 libeay >ms\\libeay32.def"
            puts $fh "perl util\\mkdef.pl 32 ssleay >ms\\ssleay32.def"
            close $fh
            out::oexec msmkmf.bat
            utils::withfile ms/mingw32.mak fc {
                regsub "RM=rem" $fc "RM=rm -f" fc
                regsub "MKDIR=gmkdir" $fc "MKDIR=mkdir" fc
                regsub "INC=" $fc "INC=-I. " fc
                regsub "INC=-I\\. -I\\." $fc "INC=-I. " fc
            }
            conf::make -f ms/mingw32.mak
	    foreach f [glob -nocomplain -directory [file join [part::destdir openssl] outinc openssl] *.h] {
		set destfile [file join [part::destdir openssl] include openssl [file tail $f]]
		if {![file exists $destfile]} {
		    file copy $f $destfile
		}
	    }
        }  else  {
            out::oexec [list [conf::x sh] ./config]
            conf::make
        }
    } \
    -librarycommand {
        upvar c c
        set rc [list]
        set dir [part::destdir openssl]
        if {$::tcl_platform(platform)=="windows"} {
            set dir [file join $dir out]
        }
        lappend rc [file join $dir libcrypto.a]
        lappend rc [file join $dir libssl.a]
        if {$::tcl_platform(platform)=="windows"} {
            lappend rc -lwsock32 -lgdi32
        }
        return $rc
    } \

# }}}
# part: tls {{{
part::create tls \
    -buildcommand {
        conf::norelnoacmainconfigure [part::srcdir tls] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols] \
            --enable-gcc \
            --with-ssl-dir=[part::destdir openssl]
        if {$::tcl_platform(platform)=="windows"} {
            set dir [part::srcdir tls]
            set ver [part::srcversion $dir]
            set files {tls tlsBIO tlsIO tlsX509}
            set ofiles [list]
            foreach f $files {
                lappend ofiles $f.o
            }
            eval [concat [list conf::make] $ofiles]
            out::oexec [concat [list ar cr [conf::libname tls $ver]] $ofiles]
        }  else  {
            conf::make
        }
    } \
    -librarycommand {
        upvar c c
        set dir [part::srcdir tls]
        set ver [part::srcversion $dir]
        set rc [list]
        lappend rc [file join [part::destdir tls] [conf::libname tls $ver]]
        return $rc
    } \
    -initcommand {
        return "Tcl_StaticPackage(0, \"Tls\", Tls_Init, Tls_SafeInit);\n"
    } \
    -externcommand {
        return "Tcl_AppInitProc Tls_Init, Tls_SafeInit;\n"
    } \
    -definescommand {
    } \
    -depend {tcl openssl} \
    -options {debug 0 thread 0} \
    -core 0 \
    -kitpatterns {lib/tls@VER@} \

# }}}

