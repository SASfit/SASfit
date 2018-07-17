# sasfit.vfs/lib/app-sasfit/tcl/sasfit_batch.tcl
#
# Copyright (c) 2008-2018, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
#   Ingo Breßler (sasfit@ingobressler.net)

proc menuBatch {} {

    set btnApply .analytical.adj.calc
    set btnFit   .analytical.adj.run

    if {![winfo exists $btnApply] ||
        ![winfo exists $btnFit]
    } {
        tk_messageBox -parent . -icon warning -message [concat \
            "Please open the fit window first:\n" \
            "<menubar> -> 'Calc' -> 'Single Data Set' -> 'fit'"]
        return
    }

    set w .batch

    if {[winfo exists $w]} {destroy $w}
    toplevel $w
    wm geometry $w
    wm title $w "batch processing"
    raise $w
    focus $w

    frame $w.series
    pack  $w.series
    #grid $w.series   -column 0 -row 2 -ipady 2m -columnspan 2 -sticky nsew

    array set ::batchConfig {}
    # reload parameter file before each run/file?
    set ::batchConfig(seriesLoadCmd) "$btnFit invoke"
    set ::batchConfig(seriesSaveCmd) "saveBatchResult"
    set ::batchConfig(output_individual) true
    set ::batchConfig(series_outfile) "fitresult.csv"
    seriesInit ::batchConfig $w.series
}

proc saveBatchResult { configarr } {
	global IQGraph SDGraph
	upvar "::AnalytPar" ap
	upvar $configarr ca

	# create output filename
	set basename [join [lindex $::sasfit(file,name) end] " "]
	set basename [join [lrange [split "$basename" {.}] 0 end-1] .]

	set result {}
	lappend result $basename
	set suffix "_text_export"
	foreach key [array names ap *$suffix] {
		set prefix [string range $key 0 [expr [string first $suffix $key]-1]]
		set title [join [split "$ap(${prefix}_descr)" {_}] " "]
		lappend result $title
		lappend result ""
		lappend result $ap(${key})
		lappend result ""
	}

	set fname "fitresult.csv"
	if {[string length $basename] > 0} {
		set fname "${basename}_fitresult.csv"
	}
	if {[info exists ca(series_outfile)] &&
		[string first "%s" $ca(series_outfile)] >= 0
	} {
		set fname [format $ca(series_outfile) [file tail $basename]]
	}
	set textdata [join $result "\n"]
	set fh [open "$fname" w+]
	puts $fh $textdata
	puts $fh ""
	puts $fh "quality of fit:"
	puts $fh ""
	puts $fh "chisq;red.chisq.;data points;fit parameters"
	puts $fh "$ap(chisq);$ap(reducedchisq);$ap(ndata);$ap(mfit)"
	puts $fh "R value;w.R value;Q factor;variance of fit"
	puts $fh "$ap(R);$ap(wR);$ap(Q);$ap(varianceOFfit)"
	puts $fh ""
	puts $fh "scattering intensity:"
	puts $fh ""
	puts $fh "Q;I(Q);Delta I(Q);resolution;..."
	export_csv_data outtxt IQGraph txt
	puts $fh $outtxt
    puts $fh ""
	puts $fh "size distributions:"
	puts $fh ""
	puts $fh "R;N(R);not used;not used;..."
	export_csv_data outtxt SDGraph txt
	puts $fh $outtxt
	close $fh
}

## ********************************************************
## tail.tcl version 1.0
##
## provides facilities like the UNIX tail function, in Tcl.
## ********************************************************

;#barecode
;#package provide tail 1.0
;#end

## ********************************************************
##
## Name: tail
##
## Description:
## A combination of tail -f and egrep.
## Loops forever on a file NAME.
## If the file doesn't exist it will wait for it to appear
## and then work on it.
##
## Parameters:
## file - a filename
## rx - a regular expression pattern to filter the lines on
##
## Usage:
##       tail filename [ regex pattern ] [ delay in ms ]
##
## If the file disappears or is replaced by a new file with the
## same name it is handled transparently.
##
## The behaviour of this function is based upon tail in the gnu
## fileutils alpha release 4.0 package.
##
## Comments:
## Only the filename argument is required.  If a regex pattern is
## given as the second argument, it will be used to filter the
## lines.
## auto-cancel if $var is deleted
proc tail { file { rx .+ } { delay 8000 } { var "" } { stats "" } { fid "" } } {

    set inode {}
    set size  {}
    set mtime {}
    foreach { inode size mtime } $stats { break }

    if { [ string length $var ] > 0 } {
        if { ! [ info exist $var ] } {
            catch { unset ${var}_tailId } err
            return
        }
    } else {
        ;## default var name
        set var ::tail_$file
    }
    if { ! [ regexp {^::} $var ] } {
        set var "::$var"
    }
    set varcntl "::${var}_cntl"
    set ${varcntl} 0
    ;## if the file exists at this iteration, tail it
    if { [ file exists $file ] } {
        file stat $file fstat
        set _inode $fstat(ino)
        set _size  $fstat(size)
        set _mtime $fstat(mtime)

        ;## if the inode has changed since the last iteration,
        ;## reopen the file.  this is from tail v4.0 in the
        ;## gnu fileutils package.
        if { $_inode != $inode } {
            catch { close $fid; set fid {} }
        } else {
            if { $_size < $size } {
                catch { seek $fid 0 }
                set ${varcntl} 0
                catch {del_jobfile}
            }
            if { $_size == $size && $_mtime != $mtime } {
                catch { seek $fid 0 }
                set ${varcntl} 0
                catch {del_jobfile}
            }
        }

        ;## if the file is not open, open it!
        if { ! [ string length $fid ] } {
            set fid [ open $file r ]
            fconfigure $fid -blocking off
            fconfigure $fid -buffering line
        }

        set inode $_inode
        set size  $_size
        set mtime $_mtime

        ;## set a variable with the content of the
        ;## regex filtered line.
        ;## use a temp var to store variable
        ;## until all lines are read
        ;## then set the global var
        ;## so trace function is not called for every line
        set temp {}

        while { [ gets $fid line ] >= 0 } {
            if { [ regexp -- $rx $line match ] } {
                ;## put a trace on variable to
                ;## read the tail output.
                append temp "$line\n"
                incr $varcntl
            }
        }

        ;## setting this will invoke trace function
        if { [ string length $temp ] } {
            set $var $temp
        }
    ;## if the file doesn't exist, make sure we aren't
    ;## creating an NFS orphan.
    } else {
        ;## maybe the file got nuked? Handle it!
        if { [ string length $fid ] } {
            catch { close $fid; set fid {} }
        }
    }

    ;## lather, rinse, repeat.  This is NOT recursion!
    set stats \{[ list $inode $size $mtime ]\}
puts "tail is alive"
    set ${var}_tailId [ after $delay \
        [ subst { tail ${file} $rx $delay ${var} $stats $fid } ] ]
}
## ********************************************************

## ********************************************************
##
## Name: cancelTail
##
## Description:
## Removes the trace associated with am invocation of "tail"
## Parameters:
## file - filename being tailed
## cmd - command handling the trace
##
## Usage:
##  cancelTail filename [ command ]
##
## Comments:
## see the man page for "trace"
proc cancelTail { file { var "" } { cmd "" } } {

    if { [ catch {
        if { ! [ string length $var ] } {
            set var ::tail_$file
            set aftervar ${var}_tailId
        } else {
            set aftervar ${var}_tailId
        }
        catch { after cancel [ set $aftervar ] }
        if { ! [ string length $cmd ] } {
            catch { set cmd [ eval lindex [ trace vinfo $var ] 1 ] }
        }
        catch { trace vdelete $var w $cmd }
    } err ] } {
        return -code error $err
    }
}
## ********************************************************

proc do_analysis_step {cntl line} {
    set maxlines [expr int([lindex [$::sasfit(batch,jobcontents) dump -mark 0.0 end] 5]-1)]
    $::sasfit(batch,jobprogress) configure -state normal
    $::sasfit(batch,jobprogress) insert end  "$cntl/$maxlines: $line\n"
    $::sasfit(batch,jobprogress) configure -state disabled
    $::sasfit(batch,jobprogress) yview moveto 1.0
    update idletasks
}

 ## auto-cancel if $var is deleted
proc analyse_loop { file  { delay 2000 } { var "" } } {
    if { [ string length $var ] > 0 } {
        if { ! [ info exist $var ] } {
            catch { unset ${var}_analyse_loopId } err
	    return
        }
    } else {
        ;## default var name
        set var ::analyse_loop_$file
        set $var 0
        set varcntl ::${var}_cntl
        set ${varcntl} 0
    }
    set varcntl ::${var}_cntl

    set lineNo [set $varcntl]
    set maxlines [expr int([lindex [$::sasfit(batch,jobcontents) dump -mark 0.0 end] 5]-1)]
    if {$maxlines > $lineNo} {
        set lineNo [incr $varcntl]
        do_analysis_step $lineNo [$::sasfit(batch,jobcontents) get  $lineNo.0 $lineNo.end]
        set delay_only_in_case_of_waiting_for_further_data 0 
    } elseif {$maxlines < $lineNo} {
        stop_analysis
        tk_messageBox -type ok -icon warning -title "analyse loop" -message "canceled the analysing loop"
        return
    } else {
        set delay_only_in_case_of_waiting_for_further_data $delay
#       puts "analyse_loop waits for further input"
    }
    ;## lather, rinse, repeat.  This is NOT recursion!
    set ${var}_analyse_loopId [ after $delay_only_in_case_of_waiting_for_further_data \
       [ subst { analyse_loop ${file} $delay ${var} }] ]
}
## ********************************************************

proc cancelAnalyseLoop { file { var "" } } {

     if  { [ catch {
         if  { ! [ string length $var ] } {
             set var ::analyse_loop_$file
             set aftervar ${var}_analyse_loopId
         } else {
	     set aftervar ${var}_analyse_loopId
         }
         catch { after cancel [ set $aftervar ] }
     } err ] } {
         return -code error $err
     }
}
## ********************************************************

proc start_jobfile_trace {} { 
    trace vdelete ::tail_$::sasfit(batch,jobfile) w jobfile_trace_cmd
    set ::tail_$::sasfit(batch,jobfile) ""
    trace variable ::tail_$::sasfit(batch,jobfile) w jobfile_trace_cmd
    tail  $::sasfit(batch,jobfile)
}

proc jobfile_trace_cmd {name1 name2 op} {
    set newcontents $name1
    $::sasfit(batch,jobcontents) configure -state normal
    $::sasfit(batch,jobcontents) insert end  [set $newcontents]
    $::sasfit(batch,jobcontents) configure -state disabled
    $::sasfit(batch,jobcontents) yview moveto 1.0
}

proc stop_jobfile_trace {} {
    cancelTail  $::sasfit(batch,jobfile)
}

proc stop_analysis {} {
    cancelTail  $::sasfit(batch,jobfile)
    cancelAnalyseLoop $::sasfit(batch,jobfile)
}

proc del_jobfile {} {
    $::sasfit(batch,jobcontents) configure -state normal
    $::sasfit(batch,jobcontents) delete 0.0 end
}

proc del_jobresult {} {
    $::sasfit(batch,jobprogress) configure -state normal
    $::sasfit(batch,jobprogress) delete 0.0 end
}

proc SingleDataSetBatchFitMenu {} {
catch {destroy .singleDatasetBatchFit}
toplevel .singleDatasetBatchFit
set w .singleDatasetBatchFit
wm geometry $w
wm title $w "SASfit batch mode"
raise $w
bind $w <Destroy> {
    stop_jobfile_trace
}

frame $w.selectbatchfile 
frame $w.continuous
frame $w.batchfilecontent
frame $w.batchprogress
frame $w.fitresult
frame $w.actionbuttons 

pack $w.selectbatchfile \
     $w.continuous \
     -padx 2mm -pady 2mm 
pack $w.batchfilecontent \
     $w.batchprogress \
     $w.fitresult \
     $w.actionbuttons \
     -expand yes -fill both \
     -padx 2mm -pady 2mm
pack $w.fitresult \
     $w.actionbuttons \
     -expand no \
     -padx 2mm -pady 2mm

set sbf $w.selectbatchfile
label $sbf.label -text "batch job file:"
label $sbf.entry -background white -relief sunken -anchor w -width 55 -textvariable ::sasfit(batch,jobfile)
button $sbf.browse -text "Select batch job file:" \
	-command  {
             set datatype All
             set filename ""
             BrowseCmd .singleDatasetBatchFit.selectbatchfile.browse filename datatype ReadData
             if {[file exists $filename]} {
		 stop_jobfile_trace 
		 set ::sasfit(batch,jobfile) $filename
	     }
         }

pack $sbf.browse\
     $sbf.entry  -side left -padx 1mm

set bfc $w.batchfilecontent
set ::sasfit(batch,jobcontents) [text $bfc.box -setgrid 1 -yscrollcommand "$bfc.sb set"]
$bfc.box configure  -wrap none -width 74 -height 10
scrollbar $bfc.sb -orient vertical -command "$bfc.box yview"
pack $bfc.box $bfc.sb -fill y -side left


set bp $w.batchprogress
set ::sasfit(batch,jobprogress) [text $bp.box -setgrid 1 -yscrollcommand "$bp.sb set"]
$bp.box configure -wrap none -width 74 -height 5
scrollbar $bp.sb -orient vertical -command "$bp.box yview"
pack $bp.box $bp.sb -fill y -side left

set fr $w.fitresult
text $fr.box -setgrid 1 -yscrollcommand "$fr.sb set"
$fr.box configure -wrap none -width 74 -height 5
scrollbar $fr.sb -orient vertical -command "$fr.box yview"
pack $fr.box $fr.sb -fill y -side left

set abf $w.actionbuttons
button $abf.readbatch -text "Read Batch" \
	-command { stop_jobfile_trace
                   del_jobfile
                   del_jobresult
		   start_jobfile_trace
                 }
button $abf.do_next -text "Analyse Next" \
	-command { analyse_loop $::sasfit(batch,jobfile) }

button $abf.do_continously -text "Analyse Contoniously" \
	-command { analyse_loop $::sasfit(batch,jobfile) }

button $abf.showHistory -text "Plot" \
	-command {destroy .singleDatasetBatchFit}

button $abf.stopall -text "Stop All" \
	-command {stop_analysis}

button $abf.exit -text Exit \
	-command {
                  stop_analysis
                  destroy .singleDatasetBatchFit
                 }

pack $abf.readbatch \
     $abf.do_next \
     $abf.do_continously \
     $abf.showHistory \
     $abf.stopall \
     $abf.exit \
     -side left -expand yes -fill x -padx 1mm
}

# vim: set ts=4 sw=4 tw=0: 
