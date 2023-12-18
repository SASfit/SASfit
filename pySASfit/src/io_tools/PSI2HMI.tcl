#!/usr/bin/wish
#----------------------------------------------------------------------------
# Converts SANS NeXus files to BerSANS format.
# Joachim Kohlbrecher, May 2004
#       modified October 2023 to be used via tkinter within a python script
#---------------------------------------------------------------------------
package require Tk

set env(PATH) "$env(PATH)"
set home [file dirname [info script]]
set indirdata "/home/sans/data"
set indirdataname $indirdata
set outdirdata $indirdata 
set outdirdataname $outdirdata
set startnumber 0
set endnumber 0
set number 0
set year [clock format [clock seconds] -format %Y]
set ext  hdf
set output   ${outdirdata}/D0000000.000
set prename  ${indirdata}/sans${year}n
set instr    ${indirdata}/
set input    ${prename}00000.$ext
set inname   $input
set outname  $output
set continuously no
set stopconvert  no
set active1 0
set trans1 ""
set wl1 0.5
set active2 0
set trans2 ""
set wl2 1.0
set trans1data ""
set trans2data ""

global trans1, wl1, trans2, wl2, trans1data, trans2data active1 active2
global output input inname outname continuously stopconvert
global prename instr startnumber endnumber number year ext drive
global indirdata outdirdata indirdataname outdirdataname

proc set_numberCmd { name1 name2 opt } {
   global startnumber number
   set number $startnumber
}

proc set_outputCmd { name1 name2 opt } {
   global output input inname outname
   global prename instr startnumber endnumber number year ext drive
   global indirdata outdirdata

   set output ${outdirdata}/D[format "%07d" $number].001
   set kn [format "%03d" [expr int($number/1000)]]
   set instr   ${indirdata}/
   set prename ${instr}sans${year}n
   set input  "$prename[format "%06d" $number].$ext"
   set inname $input
   set inlen [string length $input]
   if {$inlen > 60} {
      set inname ...[string range $input [expr $inlen-56] [expr $inlen-1]]
   }
   set outname $output
   set outlen [string length $output]
   if {$outlen > 60} {
      set outname ...[string range $output [expr $outlen-56] [expr $outlen-1]]
   }
}

set_outputCmd {} {} {}

wm title . "SANS-I raw data to BerSANS format"

frame .top
pack .top -side top -fill x

frame .top.in 
pack .top.in -side top -pady 4 -fill x

#puts [get_pyvar Tfiles_tcl]
#puts [adddict q 124]
frame .top.in.indirdata
frame .top.in.outdirdata
frame .top.in.drive
frame .top.in.startnumber
frame .top.in.endnumber
frame .top.in.year
frame .top.in.ext
frame .top.in.output
frame .top.in.input
frame .top.in.wltrans
frame .top.in.line  -relief groove -bd 3 -height 3
frame .top.in.convert

pack   .top.in.drive .top.in.indirdata .top.in.outdirdata \
       .top.in.startnumber .top.in.endnumber \
       .top.in.year .top.in.ext .top.in.input .top.in.output \
       .top.in.wltrans \
       -side top -anchor w

pack   .top.in.startnumber .top.in.endnumber \
       .top.in.year .top.in.ext .top.in.input .top.in.output \
       -side top -anchor w
pack   .top.in.convert .top.in.line -side bottom -anchor w -expand yes -fill x -pady 2

label .top.in.outdirdata.outdirdatalabel  -width 12 -anchor w\
		-text "output data dir:"
label .top.in.outdirdata.outdirdata  -width 46 -anchor w\
		-textvariable outdirdataname
button .top.in.outdirdata.outdirselect -text "New..." \
       -command { global outdirdata outdirdataname
                  set tmpstr [tk_chooseDirectory -initialdir $outdirdata \
                                     -mustexist yes \
                                     -title "select output data directory"]
                  if {[string length $tmpstr] > 0} {
                      set outdirdata $tmpstr
                  }
                  set outdirdataname $outdirdata
                  set outdirlen [string length $outdirdataname]
                  if {$outdirlen > 40} {
                     set outdirdataname ...[string range $outdirdata [expr $outdirlen-36] [expr $outdirlen-1]]
                  }
                } 
pack .top.in.outdirdata.outdirdatalabel .top.in.outdirdata.outdirdata .top.in.outdirdata.outdirselect  -side left

label .top.in.indirdata.indirdatalabel  -width 12 -anchor w\
		-text "raw data dir:"
label .top.in.indirdata.indirdata  -width 46 -anchor w\
		-textvariable indirdataname
button .top.in.indirdata.indirselect -text "New..." \
       -command { global indirdata indirdataname instr year indirdataname
                  set tmpstr [tk_chooseDirectory -initialdir $indirdata \
                                     -mustexist yes \
                                     -title "select directory"]
                  if {[string length $tmpstr] > 0} {
                      set indirdata $tmpstr
                  }
                  set instr  ${indirdata}/
                  set indirdataname $indirdata
                  set indirlen [string length $indirdataname]
                  if {$indirlen > 40} {
                     set indirdataname ...[string range $indirdata [expr $indirlen-36] [expr $indirlen-1]]
                  }
                } 
pack .top.in.indirdata.indirdatalabel .top.in.indirdata.indirdata .top.in.indirdata.indirselect -side left


#entry .top.in.drive.entry -textvariable drive \
#       -relief sunken -width 25 -highlightthickness 0 
#label .top.in.drive.label -text "drive:" \
#       -width 12 -anchor w
#pack .top.in.drive.entry .top.in.drive.label -side right 

entry .top.in.startnumber.entry -textvariable startnumber \
      -relief sunken -width 25 -highlightthickness 0 
label .top.in.startnumber.label -text "start no:" \
      -width 12 -anchor w
pack .top.in.startnumber.entry .top.in.startnumber.label -side right 

entry .top.in.endnumber.entry -textvariable endnumber \
      -relief sunken -width 25 -highlightthickness 0 
label .top.in.endnumber.label -text "end no:" \
      -width 12 -anchor w
pack .top.in.endnumber.entry .top.in.endnumber.label -side right 

entry .top.in.year.entry -textvariable year \
      -relief sunken -width 25 -highlightthickness 0
label .top.in.year.label -text "year:" \
      -width 12 -anchor w
pack .top.in.year.entry .top.in.year.label -side right 

entry .top.in.ext.entry -textvariable ext \
      -relief sunken -width 25 -highlightthickness 0 
label .top.in.ext.label -text "extention:" \
      -width 12 -anchor w
pack .top.in.ext.entry .top.in.ext.label -side right 

label .top.in.output.lab2 -text "output file:" \
      -width 12 -anchor w
label .top.in.output.lab1 -textvariable outname \
      -width 48 -anchor w
pack .top.in.output.lab1 .top.in.output.lab2 -side right 

label .top.in.input.lab2 -text "input file:" \
      -width 12 -anchor w
label .top.in.input.lab1 -textvariable inname \
      -width 48 -anchor w
pack .top.in.input.lab1 .top.in.input.lab2 -side right 

entry .top.in.wltrans.wl1 -textvariable wl1 \
      -relief sunken -width 5 -highlightthickness 0 
label .top.in.wltrans.label1 -text "lambda1:" \
      -width 8 -anchor w
entry .top.in.wltrans.wl2 -textvariable wl2 \
      -relief sunken -width 5 -highlightthickness 0 

checkbutton .top.in.wltrans.trans1active -text ":use" -variable active1 \
               -command {
                  global active1 trans1data trans1dataname wl1
                  if {!$active1} {
                     set trans1data ""
                     set trans1dataname ""
                  }
               }
label .top.in.wltrans.trans1fn  -width 46 -anchor w\
		-textvariable trans1dataname
button .top.in.wltrans.trans1select -text "select..." \
       -command { global trans1data trans1dataname wl1 outdirdata active1
                  set tmpstr [tk_getOpenFile -initialdir $outdirdata \
                                             -title "select transmission file for lambda=$wl1"]
                  if {[string length $tmpstr] > 0} {
                      set trans1data $tmpstr
                  }
                  set trans1dataname $trans1data
                  set trans1len [string length $trans1dataname]
                  if {$trans1len > 40} {
                     set trans1dataname ...[string range $trans1data [expr $trans1len-36] [expr $trans1len-1]]
                  }
                  if {$trans1len > 0} {
                     set active1 1
                     puts "active1: $active1"
                  } else {
                     set active1 0
                  }
                  puts "wl1: $trans1data"
                } 
checkbutton .top.in.wltrans.trans2active -text ":use" -variable active2 \
               -command {
                  global active2 trans2data trans2dataname wl2
                  if {!$active2} {
                     set trans2data ""
                     set trans2dataname ""
                  }
                  puts "$active2"
               }
label .top.in.wltrans.trans2fn  -width 46 -anchor w\
		-textvariable trans2dataname
button .top.in.wltrans.trans2select -text "select..." \
       -command { global active2 trans2data trans2dataname wl2 outdirdata 
                  set tmpstr [tk_getOpenFile -initialdir $outdirdata \
                                             -title "select transmission file for lambda=$wl2"]
                  if {[string length $tmpstr] > 0} {
                      set trans2data $tmpstr
                  }
                  set trans2dataname $trans2data
                  set trans2len [string length $trans2dataname]
                  if {$trans2len > 40} {
                     set trans2dataname ...[string range $trans2data [expr $trans2len-36] [expr $trans2len-1]]
                  }
                  if {$trans2len > 0} {
                     set active2 1
                     puts "active2: $active2"
                  } else {
                     set active2 0
                  }
                  puts "wl2: $trans2data"
                } 
label .top.in.wltrans.label2 -text "lambda2:" \
      -width 8 -anchor w
grid .top.in.wltrans.label1 .top.in.wltrans.wl1 .top.in.wltrans.trans1active .top.in.wltrans.trans1select .top.in.wltrans.trans1fn
grid .top.in.wltrans.label2 .top.in.wltrans.wl2 .top.in.wltrans.trans2active .top.in.wltrans.trans2select .top.in.wltrans.trans2fn
button .top.in.convert.action -text "convert" \
            -command { global number startnumber endnumber output input continuously stopconvert
                       global trans1data trans1dataname wl1 outdirdata active1
                       global trans2data trans2dataname wl2 active2

                       for {set number $startnumber} {
                                $number <= $endnumber} {
                                            incr number} {
                          update
			  if {([string compare $continuously yes]==0) && ([string compare $stopconvert no]==0) } {
			      set tmpoutput $outname
			      while {![file exist $input]} {
				  if {[string compare $stopconvert yes] == 0} {
				      break
				  }
				  update
				  for {set i 0} {$i < 10} {incr i} {
				      if {[string compare $stopconvert yes] == 0} {
					  break
				      }
				      set outname "waiting for new data"
				      update
					after 100
				  }
			      }
			      set $outname $tmpoutput
			  }
			  if {[string compare $stopconvert yes] == 0} {
			      break
			  }
                          if {[file exist $input] } {
                              puts "convert  $input  to $output"
                              set outname [file tail $output]
                              # catch {eval exec "${home}/hdf2hmi ${home}/sansstore.dic $input $output $outname"} msg
                              # catch {eval exec "python ${home}/PSISANS1toHMI.py $input $output"} msg
                              set Trans_path "C:/Users/kohlbrecher/switchdrive/SANS/user/Genix/raw/"
                              set T8 ${Trans_path}trans8A.txt
                              adddict_Tfiles 0.8 $T8
                              catch {rawhdf2hmi_tcl $input $output} msg 
                              puts "msg from tcl call >rawhdf2hmi_tcl $input $output<"
                              #puts "python ${home}/PSISANS1toHMI.py $input $output"
                              #puts $msg
                          } else {
                              puts "file >$input< does not exists"
                          }
                          update idletasks
                       }
                       incr number -1
                     }  
checkbutton .top.in.convert.continuously \
           -text "continuously?" \
           -variable continuously \
           -onvalue yes -offvalue no
button .top.in.convert.stop  -text STOP -command {
    global output input inname outname continuously stopconvert
    set stopconvert yes
}
button  .top.in.convert.exit  -text "exit" -command {
      destroy .
}
pack  .top.in.convert.action \
      .top.in.convert.continuously \
      .top.in.convert.stop \
      -side left  -anchor w
pack  .top.in.convert.exit   -side right -anchor e

trace variable startnumber w set_numberCmd
trace variable number      w set_outputCmd
trace variable indirdata   w set_outputCmd
trace variable outdirdata  w set_outputCmd

 .top.in.outdirdata.outdirselect invoke
