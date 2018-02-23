#!/usr/bin/wish
#----------------------------------------------------------------------------
# BerSANS SANSDAni or SANSDRaw files to GIF format.
# Joachim Kohlbrecher, Februar 2018
#---------------------------------------------------------------------------
#package require Tk

global SCAData
if {![info exists erik]} {
	global erik
	set erik(startnumber) 1
	set erik(endnumber) 1
	set erik(number) 1

	set erik(outdirdata) $::sasfit_basedir

	set erik(outext) sca
	set erik(inext) sca

	set erik(input) "dummy.sca"
	set erik(indirdata) ""
	set erik(indirdataname) $erik(outdirdata)

	set erik(output) "dummy.sca"
	set erik(outdirdata) ""
	set erik(outdirdataname) $erik(outdirdata)
}
proc set_numberCmd { name1 name2 opt } {
   global erik
   set erik(number) $erik(startnumber)
}

proc set_outputCmd { name1 name2 opt } {
   global erik
#   set erik(output) "[format "%07d" $erik(number)].sca"
}

if {[winfo exists .werik]} {destroy .werik}
	toplevel .werik
	wm geometry .werik
	wm title .werik "Generate BerSANS *.sca file from dummy.sca for sequence of flipper on flipper off experiments"
	raise .werik
	focus .werik
	grab .werik


frame .werik.in 
pack .werik.in -side top -pady 4 -fill x

frame .werik.in.indirdata
frame .werik.in.startnumber
frame .werik.in.endnumber
frame .werik.in.input
frame .werik.in.output
frame .werik.in.line  -relief groove -bd 3 -height 3
frame .werik.in.convert



pack   .werik.in.startnumber .werik.in.endnumber \
       .werik.in.input .werik.in.output  \
       -side top -anchor w

pack   .werik.in.convert .werik.in.line -side bottom -anchor w -expand yes -fill x -pady 2



label .werik.in.indirdata.indirdatalabel  -width 12 -anchor w\
		-text "data dir.:"
label .werik.in.indirdata.indirdata  -width 46 -anchor w\
		-textvariable erik(indirdataname)
		
		
pack  .werik.in.startnumber .werik.in.endnumber \
      .werik.in.input  \
     -side top -anchor w 

pack   .werik.in.convert .werik.in.line -side bottom -anchor w -expand yes -fill x -pady 2


entry .werik.in.startnumber.entry -textvariable erik(startnumber) \
      -relief sunken -width 35 -highlightthickness 0 
label .werik.in.startnumber.label -text "start no:" \
      -width 18 -anchor e
pack .werik.in.startnumber.entry .werik.in.startnumber.label -side right 

entry .werik.in.endnumber.entry -textvariable erik(endnumber) \
      -relief sunken -width 35 -highlightthickness 0 
label .werik.in.endnumber.label -text "end no:" \
      -width 18 -anchor e
pack .werik.in.endnumber.entry .werik.in.endnumber.label -side right 


label .werik.in.output.lab2 -text "output file:" \
      -width 12 -anchor w
entry .werik.in.output.lab1 -textvariable erik(output) \
      -relief sunken -width 35 -highlightthickness 0 
pack .werik.in.output.lab1 .werik.in.output.lab2 -side right 


button .werik.in.input.lab2 -text "input file:" \
       -command { global erik 
                  set tmpstr [tk_getOpenFile -initialdir $erik(indirdata) \
									-filetypes {{{BerSANS sca} {*.sca}} {{All} {*}}} \
                                     -title "select input sca"]
                  if {[string length $tmpstr] > 0} {
                      set erik(input) $tmpstr
                  }
                  set erik(indirdata) [file dirname $erik(input)]
                  set erik(indirlen) [string length $erik(indirdata)]
                  if {$erik(indirlen) > 40} {
                     set erik(indirdataname) ...[string range $erik(indirdata) [expr $erik(indirlen)-36] [expr $erik(indirlen)-1]]
                  } else {
					set erik(indirdataname) $erik(indirdata)
				  }
                } 

label .werik.in.input.lab1 -textvariable erik(input) \
      -width 50 -anchor w
pack .werik.in.input.lab1 .werik.in.input.lab2 -side right 

button .werik.in.convert.action -text "convert" \
            -command { global erik SCAData
					   if {[file exist $erik(input)] } {
					        set fid [open $erik(indirdata)/$erik(output) w+] 
							puts "convert  $erik(input)  to \n[file dirname $erik(input)]/$erik(output)\nfile No. $erik(number)"   
							read_SCA  $erik(input) SCAData
							puts $fid "%File"
							puts $fid "FileName=$erik(output)"
							puts $fid "FileDate=[clock format [clock seconds] -format "%d-%b-%Y"]"
							puts $fid "FileTime=[clock format [clock seconds] -format "%H-%M-%S"]"
							puts $fid "Type=[SCAgetItem SCAData File Type]"
							puts $fid "Reduction=Flipper"
							puts $fid "Logging=[SCAgetItem SCAData File Logging]"
							puts $fid "%Default"
							puts $fid "Deadtime=[SCAgetItem SCAData Default Deadtime]"
							puts $fid "Normalizer=[SCAgetItem SCAData Default Normalizer]"
							puts $fid "%Commands"
							set item [SCAgetItem SCAData Commands Cadmium]
							if {[string compare $item "not used"]  == 0} {
								puts $fid "\n*Data type: \"Cadmium\" - not used."
							} else {
								puts $fid "\n\$DATASET"
								puts $fid "DataType=Cadmium"
								set NoExtName [getRunNo_SCA [SCAgetItem SCAData Commands Cadmium] Standard]
								puts $fid "Standard"
								puts $fid "*  Run    Ext-In   - SampleName"
								foreach meas $NoExtName {
									puts $fid "[format "%8s" [lindex $meas 0]] [format "%5s" [lindex $meas 1]]     - [lindex $meas 2]"
								}
#   88237     1     - S8
								puts $fid "End"
							}
							set item [SCAgetItem SCAData Commands Water]
							if {[string compare $item "not used"]  == 0} {
								puts $fid "\n*Data type: \"Water\" - not used."
							} else {
								puts $fid "\n\$DATASET"
								puts $fid "DataType=Water"
								puts $fid "Transmission=[SCAgetItem SCAData Commands Water Transmission]"
								puts $fid "Scaling=[SCAgetItem SCAData Commands Water Scaling]"
								set NoExtName [getRunNo_SCA [SCAgetItem SCAData Commands Water] Standard]
								puts $fid "Standard"
								puts $fid "*  Run    Ext-In   - SampleName"
								foreach meas $NoExtName {
									puts $fid "[format "%8s" [lindex $meas 0]] [format "%5s" [lindex $meas 1]]     - [lindex $meas 2]"
								}
#   88237     1     - S8
								puts $fid "End"
							}
							set item [SCAgetItem SCAData Commands WaterBackground]
							if {[string compare $item "not used"]  == 0} {
								puts $fid "\n*Data type: \"WaterBackground\" - not used."
							} else {
								puts $fid "\n\$DATASET"
								puts $fid "DataType=WaterBackground"
								puts $fid "Transmission=[SCAgetItem SCAData Commands WaterBackground Transmission]"
								set NoExtName [getRunNo_SCA [SCAgetItem SCAData Commands WaterBackground] Standard]
								puts $fid "Standard"
								puts $fid "*  Run    Ext-In   - SampleName"
								foreach meas $NoExtName {
									puts $fid "[format "%8s" [lindex $meas 0]] [format "%5s" [lindex $meas 1]]     - [lindex $meas 2]"
								}
#   88237     1     - S8
								puts $fid "End"
							}
							set item [SCAgetItem SCAData Commands SampleBackground]
							if {[string compare $item "not used"]  == 0} {
								puts $fid "\n*Data type: \"SampleBackground\" - not used."
							} else {
								puts $fid "\n\$DATASET"
								puts $fid "DataType=SampleBackground"
								puts $fid "Transmission=[SCAgetItem SCAData Commands SampleBackground Transmission]"
								set NoExtName [getRunNo_SCA [SCAgetItem SCAData Commands SampleBackground] Standard]
								puts $fid "Standard"
								puts $fid "*  Run    Ext-In   - SampleName"
								foreach meas $NoExtName {
									puts $fid "[format "%8s" [lindex $meas 0]] [format "%5s" [lindex $meas 1]]     - [lindex $meas 2]"
								}
#   88237     1     - S8
								puts $fid "End"
							}
							for {set erik(number) $erik(startnumber)} {
                                $erik(number) <= $erik(endnumber)} {
                                            incr erik(number) 2} {
								update idletasks
                        
								set item [SCAgetItem SCAData Commands Sample]
								if {[string compare $item "not used"]  == 0} {
									puts $fid "\n*Data type: \"Sample\" - not used."
									return 
								} else {
									puts $fid "\n\$DATASET"
									puts $fid "DataType=Sample"
									puts $fid "Transmission=[SCAgetItem SCAData Commands Sample 1 Transmission]"	
									puts $fid "Scaling=[SCAgetItem SCAData Commands Sample 1 Scaling]"
									puts $fid "Attenuation=[SCAgetItem SCAData Commands Sample 1 Attenuation]"
									puts $fid "Probability=[SCAgetItem SCAData Commands Sample 1 Probability]"
									puts $fid "ExtensionOut=[SCAgetItem SCAData Commands Sample 1 ExtensionOut]"
									puts $fid "FlipperOn"
									puts $fid "*  Run    Ext-In   - SampleName"
									puts $fid "[format "%8s" $erik(number)] [format "%5s" 1]     - unknown"
#   88237     1     - S8
									puts $fid "End"
									puts $fid "FlipperOff"
									puts $fid "*  Run    Ext-In   - SampleName"
									puts $fid "[format "%8s" [expr $erik(number)+1]] [format "%5s" 1]     - unknown"
#   88237     1     - S8
									puts $fid "End"
								}
								update idletasks
							}
							close $fid
							incr erik(number) -1
                       } else {
                           puts "file No. $erik(number):  $erik(input) does not exist"
                       }
                     }

button  .werik.in.convert.exit  -text "exit" -command {
	catch {destroy .werik}
}
pack  .werik.in.convert.action -side left -anchor w
pack  .werik.in.convert.exit -side right -anchor e

trace variable erik(startnumber) w set_numberCmd
trace variable erik(number) w set_outputCmd

trace variable erik(input)   w set_outputCmd
trace variable erik(outdirdata)  w set_outputCmd

 #.werik.in.outdirdata.outdirselect invoke
