# part: res {{{
part::create res \
    -buildcommand {
        # this part is Windows-specific
        if {$::tcl_platform(platform)!="windows"} {return}

	# now we also include file information in non-Tk applications
        # only if Tk is compiled
        #if {[conf::c_if tk 0 1]} {return}

        if {[conf::c_if tk 0 1]} {
	    # no Tk
	    set dotk -nogui
	}  else  {
	    # with Tk
	    set dotk ""
	}

        set res $::conf::opt(resfile)
        if {$res!=""} {
            set tcldir [part::srcdir tcl]
            set tkdir [part::srcdir tk]
            set sdir [part::srcdir res]
            set ddir [part::destdir res]
            out::oexec [list windres -o [file join $ddir res$dotk.o] \
                --define STATIC_BUILD \
                --include $sdir \
                --include [file join $tcldir generic] \
                --include [file join $tkdir generic] \
                --include [file join $tkdir win rc] \
                [file join $sdir resource$dotk-$res.rc] \
                ]
        }
	if {![file exists [part::destdir dqkit]]} {
	    file mkdir [part::destdir dqkit]
	}
	file copy -force [part::srcdir res]/dqkit.exe.manifest [part::destdir dqkit]
    } \
    -librarycommand {
        # this part is Windows-specific
        if {$::tcl_platform(platform)!="windows"} {return {}}

	# now we also include file information in non-Tk applications
        # only if Tk is compiled
        #if {[conf::c_if tk 0 1]} {return {}}

        if {[conf::c_if tk 0 1]} {
	    # no Tk
	    set dotk -nogui
	}  else  {
	    # with Tk
	    set dotk ""
	}

        # if we don't want to include resources, then don't do that
        if {[conf::c_if resource 0 1]} {return {}}
        set rc [list]
        set res $::conf::opt(resfile)
        if {$res==""} {
            lappend rc [part::destdir tk]/wish.res.o 
        }  else  {
            lappend rc [part::destdir res]/res$dotk.o
        }
        return $rc
    } \
    -options {resfile "" tk 0 resource 1} \
    -core 1

# }}}

