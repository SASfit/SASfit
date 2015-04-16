# part: tk {{{
proc part::_tk_appinit {} {
    if {[utils::platformdir]=="win"} {
        return [part::srcdir tk]/win/winMain.c
    }
    return {}
}

part::create tk \
    -buildcommand {
        set tclsrcdir [part::srcdir tcl]
        set tclversion [part::srcversion $tclsrcdir 1]
        set cmd [list conf::norelnoacplatformconfigure [part::srcdir tk] --disable-shared [conf::c_with tcl] [conf::c_disen thread threads] [conf::c_disen debug symbols]]
        if {([utils::platformdir] == "unix") && ([package vcompare $tclversion 8.5] >= 0)} {
            lappend cmd [conf::c_if xft --enable-xft --disable-xft]
        }
        if { $::tcl_platform(os) == "Darwin" } {
            lappend cmd "--enable-aqua"
            lappend cmd "--without-x"
        }
        eval $cmd
        conf::make binaries
        conf::make install
    } \
    -librarycommand {
        upvar c c
        set tclsrcdir [part::srcdir tcl]
        set tclversion [part::srcversion $tclsrcdir 1]
        set rc [list]
        if {([utils::platformdir] == "unix") && ([package vcompare $tclversion 8.5] >= 0) && [conf::c_if xft 1 0]} {
            lappend rc -lXft -lfontconfig -lfreetype -lexpat -lXrender
        }
        set rc [concat $rc [list "[part::destdir tk]/$c(TK_LIB_FILE)"] $c(TK_LIBS)]
        if {[conf::c_if threadedx11 1 0] && [conf::c_if thread 0 1]} {
	    set rc [linsert $rc 0 -lpthread]
	}
        if {$::tcl_platform(platform)=="windows"} {
            lappend rc [conf::c_if console -mconsole -mwindows]
        }
        return $rc
    } \
    -definescommand {
        upvar c c
        set tksrc [part::srcdir tk]
        set rc [concat $c(TK_DEFS) [list -DKIT_INCLUDES_TK] [list -I$tksrc/[utils::platformdir] -I$tksrc/generic]]
        if {$::tcl_platform(platform)=="windows"} {
            lappend rc -DTK_LOCAL_APPINIT=TclKit_AppInit -DTCLKIT_SOURCE_MAINTCL
        }
        return $rc
    } \
    -depend {tcl} \
    -options {debug 0 thread 0 console 0 threadedx11 0 xft 0} \
    -config tkConfig.sh \
    -core 0 \
    -appinit {[part::_tk_appinit]} \
    -kitpatterns {lib/tk@VER1@} \

# }}}

