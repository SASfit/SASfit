set target [pwd]
cd [file dirname [info script]]

set rcfile resource.rc
catch {set rcfile $env(BCrcfile)}
set cmd [list windres -o [file join $target res.o] --define STATIC_BUILD --include [pwd] --include $target --include ../tcl/generic --include ../tk/generic --include ../tk/win/rc --include ../tk/win/rc $rcfile >@stdout]
puts "CMD='$cmd'"
eval exec $cmd

