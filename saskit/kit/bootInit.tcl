global auto_path tcl_library tcl_libPath
global tcl_version tcl_rcFileName
  
set noe [info nameofexecutable]

set tcl_library [file join $noe lib tcl$tcl_version]
set tcl_libPath [list $tcl_library [file join $noe lib]]

# get rid of a build residue
unset -nocomplain ::tclDefaultLibrary

namespace eval ::vfs {}

namespace eval ::vfs { variable tclkit_version 200403 }
uplevel #0 [list source [file join $tcl_library init.tcl]]
set ::auto_path $::tcl_libPath

