# sasfit.vfs/lib/app-sasfit/tcl/sasfit_readData.tcl
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
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
#   Ingo Bressler (ingo@cs.tu-berlin.de)

# (c) Joachim Kohlbrecher, 1st Apr. 2003

#------------------------------------------------------------------------------
# writes the input file for running the program "sasfit" fitting the 
# scattering curve by an analytical given size distribution and analytical
# given form factor
#
proc write_sasfit_inp_file {AanalytPar filename} {

	upvar $AanalytPar AnalytPar
	if { [catch {set f [open $filename w]}] } { return 1 }
	puts $f "sasfit version 0.8"
	puts $f [format "%60s : max_SD" $AnalytPar(max_SD)]

	for {set i 0} {$i < $AnalytPar(max_SD)} {incr i} {
		puts $f [format "%60s : i-th SD" $i] 
#		puts "i-th SD: $i"
#if { [catch { puts "AnalytPar(dataset): $AnalytPar(dataset)" }] } { puts "AnalytPar(dataset) not defined" }
#if { [catch { puts "AnalytPar(datalabel): $AnalytPar(datalabel)" }] } { puts "AnalytPar(datalabel) not defined" }
#if { [catch { puts "AnalytPar(datasetlabel): $AnalytPar(datasetlabel)"}] } { puts "AnalytPar(datasetlabel) not defined" }
		if {[catch {puts $f [format "%60s : SD belongs to j-th data set" [lindex $AnalytPar(datasetlabel)] $i]} ]} {
			puts $f [format "%60s : SD belongs to j-th data set" 1]
		}
		puts $f [format "%s" [lindex $AnalytPar(calcSDFF)   $i]]
		puts $f [format "%s" [lindex $AnalytPar(fitSDFF)    $i]]
		puts $f [format "%s" [lindex $AnalytPar(substrSDFF)    $i]]
		puts $f [format "%60s : FF_l"       [lindex $AnalytPar(FF,l)       $i]]
		puts $f [format "%60s : FF_err"     [lindex $AnalytPar(FF,err)     $i]]
		puts $f [format "%60s : FF_min"     [lindex $AnalytPar(FF,min)     $i]]
		puts $f [format "%60s : FF_max"     [lindex $AnalytPar(FF,max)     $i]]
		puts $f [format "%60s : FF_limits"  [lindex $AnalytPar(FF,limits)  $i]]
		puts $f [format "%60s : FF_active"  [lindex $AnalytPar(FF,active)  $i]]
		puts $f [format "%60s : FF_distr"   [lindex $AnalytPar(FF,distr)   $i]]
		puts $f [format "%s" [lindex $AnalytPar(FF,typestr) $i]]
		puts $f [format "%60s : SD_a"       [lindex $AnalytPar(SD,a)       $i]]
		puts $f [format "%60s : SD_err"     [lindex $AnalytPar(SD,err)     $i]]
		puts $f [format "%60s : SD_min"     [lindex $AnalytPar(SD,min)     $i]]
		puts $f [format "%60s : SD_max"     [lindex $AnalytPar(SD,max)     $i]]
		puts $f [format "%60s : SD_limits"  [lindex $AnalytPar(SD,limits)  $i]]
		puts $f [format "%60s : SD_active"  [lindex $AnalytPar(SD,active)  $i]]
		puts $f [format "%s" [lindex $AnalytPar(SD,typestr) $i]]
		puts $f [format "%60s : SQ_s"       [lindex $AnalytPar(SQ,s)       $i]]
		puts $f [format "%60s : SQ_err"     [lindex $AnalytPar(SQ,err)     $i]]
		puts $f [format "%60s : SQ_min"     [lindex $AnalytPar(SQ,min)     $i]]
		puts $f [format "%60s : SQ_max"     [lindex $AnalytPar(SQ,max)     $i]]
		puts $f [format "%60s : SQ_limits"  [lindex $AnalytPar(SQ,limits)  $i]]
		puts $f [format "%60s : SQ_active"  [lindex $AnalytPar(SQ,active)  $i]]
		puts $f [format "%s" [lindex $AnalytPar(SQ,typestr) $i]]
		puts $f [format "%s : SQ_how" [lindex $AnalytPar(SQ,how) $i]]
	}
	close $f
	return 0
}



#------------------------------------------------------------------------------
# utility routine for "proc load_sasfit_inp_file
# simple check, if the elements in a list are of numeric type
#
proc check_list_numeric {list} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
foreach i  $list {
   if {[catch {expr $i+1}] } { return 0 }
}
return 1
#foreach i $list {
#   if {[string is double $i] != 1 } { return 0 }
#}
#return 1
}

#------------------------------------------------------------------------------
# utility routine for checking the version of the parameter file *.par
#
# return value of the prcedure:
#        "0.0" : zero version 
#        "0.3" : 0.3 version
#        "0.7" : 0.7 version
#
proc get_par_version {fname} {

	set par_ver 0.0
	set f [open $fname r]
	gets $f line
	close $f
	switch $line {
		"sasfit version 0.3" {return "0.3"}
		"sasfit version 0.7" {return "0.7"}
		default              {return "0.0"}
	}
}

#------------------------------------------------------------------------------
# utility routine for "proc load_sasfit_inp_file
#
# return values of procedure:
#      0 : a) because of eof $f
#          b) next line doesn't contain enough words, i.e. number of words 
#             in next line is smaller than (last-first+1)
#      1 : successful read of next line in file $f
# changed variables:
#    llv : contains the "$first" to the "$last" word of the next line in $f as
#          a list of words.
#
proc get_load_line {f first last llv} {

	upvar $llv lv
	if { [eof $f] } { return 0 }

	gets $f line
	set tmpsplitline [split $line "\t ,"]
	set splitline {}
	foreach i $tmpsplitline {
		if {[llength $i] != 0} {
			lappend splitline $i
		}
	}
	if {[llength $splitline] <= $last} { return 0 }

	set lv  [lrange $splitline $first $last]
	return 1
}

#------------------------------------------------------------------------------
# load the parameter for fitting the  scattering curve by an analytical 
# given size distribution and analytical given form factor
# This procedure reads input files of sasfit version 0.0
#
proc load_sasfit_inp_file_ver_0_0 {AanalyticPar filename} {

upvar $AanalyticPar AnalytPar
cp_arr AnalytPar tmpAP
set f [open $filename r]
#
# reading max_SD
#
if {[get_load_line $f 0 0 lv] && [check_list_numeric $lv]} {
   set tmpAP(max_SD) [lindex $lv 0]
} else {
   close $f
   return 0
}
#
# check if tmpAP(max_SD) is of integer typ; if not return 0
#
if { [expr $tmpAP(max_SD) / round($tmpAP(max_SD))] != 1.0 } {close $f; return 0}
#
# return false if tmpAP(max_SD) < 1
#
if { $tmpAP(max_SD) < 1 } { close $f; return 0 }
#
# reading parameter of i-th size distribution
#
for {set i 1} {$i <= $tmpAP(max_SD)} {incr i} {
#
# skip line "i-th SD"
#
   if {![eof $f]} {
      gets $f line 
   } else {
      close $f
      return 0
   }
#
# read FF_l
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,l) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(FF,l) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_err
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,err) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(FF,err) $lv

   } else {
      close $f
      return 0
   }
#
# read FF_min
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,min) {} }
      lappend lv -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200
      lappend tmpAP(FF,min) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_max
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,max) {} }
      lappend lv 1e+200 1e+200 1e+200 1e+200 1e+200 1e+200
      lappend tmpAP(FF,max) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_limits
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,limits) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_active
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,active) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,active) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_distr
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,distr) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,distr) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(FF,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(FF,typestr) $line
   } else {
      close $f
      return 0
   }
#
# read SD_a
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,a) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(SD,a) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_err
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,err) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(SD,err) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_min
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,min) {} }
      lappend lv -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200
      lappend tmpAP(SD,min) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_max
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,max) {} }
      lappend lv 1e+200 1e+200 1e+200 1e+200 1e+200 1e+200
      lappend tmpAP(SD,max) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_limits
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,limits) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(SD,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_active
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,active) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(SD,active) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(SD,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(SD,typestr) $line
   } else {
      close $f
      return 0
   }

   if {$i == 1} {
      set tmpAP(SQ,typestr) {} 
      set tmpAP(SQ,active)  {}
      set tmpAP(SQ,limits)  {}
      set tmpAP(SQ,max)     {}
      set tmpAP(SQ,min)     {}
      set tmpAP(SQ,err)     {}
      set tmpAP(SQ,s)       {}
      set tmpAP(SQ,how)     {}
   }
   lappend tmpAP(SQ,typestr)  None
   lappend tmpAP(SQ,how)      0
   lappend tmpAP(SQ,active)  { 0 0 0 0 0 0 0 0 0 0 }
   lappend tmpAP(SQ,limits)  { 0 0 0 0 0 0 0 0 0 0 }
   lappend tmpAP(SQ,min)     {-1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 }
   lappend tmpAP(SQ,max)     { 1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200 }
   lappend tmpAP(SQ,err)     { 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 }
   lappend tmpAP(SQ,s)       { 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 }


if {$i == 1} {
    set tmpAP(calcSDFF) {}
    set tmpAP(fitSDFF) {}
   set tmpAP(<R^0>) {}
   set tmpAP(<R^1>) {} 
   set tmpAP(<R^2>) {}
   set tmpAP(<R^3>) {}
   set tmpAP(<R^4>) {}
   set tmpAP(<R^5>) {}
   set tmpAP(<R^6>) {}
   set tmpAP(<R^7>) {}
   set tmpAP(<R^8>) {}
   set tmpAP(R_li)  {}
   set tmpAP(R_lc)  {}
   set tmpAP(R_Ac)  {}
   set tmpAP(R_VP)  {}
   set tmpAP(R_RG)  {}
   set tmpAP(fp)    {}
}
lappend tmpAP(<R^0>) -1.0
lappend tmpAP(<R^1>) -1.0
lappend tmpAP(<R^2>) -1.0
lappend tmpAP(<R^3>) -1.0
lappend tmpAP(<R^4>) -1.0
lappend tmpAP(<R^5>) -1.0
lappend tmpAP(<R^6>) -1.0
lappend tmpAP(<R^7>) -1.0
lappend tmpAP(<R^8>) -1.0
lappend tmpAP(R_li)  -1.0
lappend tmpAP(R_lc)  -1.0
lappend tmpAP(R_Ac)  -1.0
lappend tmpAP(R_VP)  -1.0
lappend tmpAP(R_RG)  -1.0
lappend tmpAP(fp)    -1.0

lappend tmpAP(calcSDFF) yes
lappend tmpAP(fitSDFF) yes

} ;# end of for loop
cp_arr tmpAP AnalytPar
return 1
}

#------------------------------------------------------------------------------
# load the parameter for fitting the  scattering curve by an analytical 
# given size distribution and analytical given form factor
# This procedure reads input files of sasfit version 0.0
#
proc load_sasfit_inp_file_ver_0_3 {AanalyticPar filename} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $AanalyticPar AnalytPar
cp_arr AnalytPar tmpAP
set f [open $filename r]
gets $f line
#
# reading max_SD
#
if {[get_load_line $f 0 0 lv] && [check_list_numeric $lv]} {
   set tmpAP(max_SD) [lindex $lv 0]
} else {
   close $f
   return 0
}
#
# check if tmpAP(max_SD) is of integer typ; if not return 0
#
if { [expr $tmpAP(max_SD) / round($tmpAP(max_SD))] != 1.0 } {close $f; return 0}
#
# return false if tmpAP(max_SD) < 1
#
if { $tmpAP(max_SD) < 1 } { close $f; return 0 }
#
# reading parameter of i-th size distribution
#
for {set i 1} {$i <= $tmpAP(max_SD)} {incr i} {
#
# skip line "i-th SD"
#
   if {![eof $f]} {
      gets $f line 
   } else {
      close $f
      return 0
   }
#
# read calcSDFF
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(calcSDFF) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(calcSDFF) $line
   } else {
      close $f
      return 0
   }
#
# read fitSDFF
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(fitSDFF) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(fitSDFF) $line
   } else {
      close $f
      return 0
   }

#
# read FF_l
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,l) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(FF,l) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_err
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,err) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(FF,err) $lv

   } else {
      close $f
      return 0
   }
#
# read FF_min
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,min) {} }
      lappend lv -1e+200 -1e+200 -1e+200 -1e+200  -1e+200 -1e+200
      lappend tmpAP(FF,min) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_max
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,max) {} }
      lappend lv 1e+200 1e+200 1e+200 1e+200 1e+200 1e+200
      lappend tmpAP(FF,max) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_limits
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,limits) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_active
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,active) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,active) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_distr
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,distr) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(FF,distr) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(FF,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(FF,typestr) $line
   } else {
      close $f
      return 0
   }
#
# read SD_a
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,a) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(SD,a) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_err
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,err) {} }
      lappend lv 0.0 0.0 0.0 0.0 0.0 0.0
      lappend tmpAP(SD,err) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_min
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,min) {} }
      lappend lv -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200
      lappend tmpAP(SD,min) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_max
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,max) {} }
      lappend lv 1e+200 1e+200 1e+200 1e+200 1e+200 1e+200
      lappend tmpAP(SD,max) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_limits
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,limits) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(SD,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_active
#
   if {[get_load_line $f 0 3 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,active) {} }
      lappend lv 0 0 0 0 0 0
      lappend tmpAP(SD,active) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(SD,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(SD,typestr) $line
   } else {
      close $f
      return 0
   }

   if {$i == 1} {
      set tmpAP(SQ,typestr) {} 
      set tmpAP(SQ,active)  {}
      set tmpAP(SQ,limits)  {}
      set tmpAP(SQ,max)     {}
      set tmpAP(SQ,min)     {}
      set tmpAP(SQ,err)     {}
      set tmpAP(SQ,s)       {}
      set tmpAP(SQ,how)     {}
   }
   lappend tmpAP(SQ,typestr)  None
   lappend tmpAP(SQ,active)  { 0 0 0 0 0 0 0 0 0 0 }
   lappend tmpAP(SQ,limits)  { 0 0 0 0 0 0 0 0 0 0 }
   lappend tmpAP(SQ,min)     {-1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 -1e+200 }
   lappend tmpAP(SQ,max)     { 1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200  1e+200 }
   lappend tmpAP(SQ,err)     { 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 }
   lappend tmpAP(SQ,s)       { 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 }
   lappend tmpAP(SQ,how) 0

if {$i == 1} {
   set tmpAP(<R^0>) {}
   set tmpAP(<R^1>) {} 
   set tmpAP(<R^2>) {}
   set tmpAP(<R^3>) {}
   set tmpAP(<R^4>) {}
   set tmpAP(<R^5>) {}
   set tmpAP(<R^6>) {}
   set tmpAP(<R^7>) {}
   set tmpAP(<R^8>) {}
   set tmpAP(R_li)  {}
   set tmpAP(R_lc)  {}
   set tmpAP(R_Ac)  {}
   set tmpAP(R_VP)  {}
   set tmpAP(R_RG)  {}
   set tmpAP(fp)    {}
}
lappend tmpAP(<R^0>) -1.0
lappend tmpAP(<R^1>) -1.0
lappend tmpAP(<R^2>) -1.0
lappend tmpAP(<R^3>) -1.0
lappend tmpAP(<R^4>) -1.0
lappend tmpAP(<R^5>) -1.0
lappend tmpAP(<R^6>) -1.0
lappend tmpAP(<R^7>) -1.0
lappend tmpAP(<R^8>) -1.0
lappend tmpAP(R_li)  -1.0
lappend tmpAP(R_lc)  -1.0
lappend tmpAP(R_Ac)  -1.0
lappend tmpAP(R_VP)  -1.0
lappend tmpAP(R_RG)  -1.0
lappend tmpAP(fp)    -1.0

} ;# end of for loop
cp_arr tmpAP AnalytPar
return 1
}


#------------------------------------------------------------------------------
# load the parameter for fitting the  scattering curve by an analytical 
# given size distribution and analytical given form factor
# This procedure reads input files of sasfit version 0.0
#
proc load_sasfit_inp_file_ver_0_7 {AanalyticPar filename} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $AanalyticPar AnalytPar
cp_arr AnalytPar tmpAP
set f [open $filename r]
gets $f line
#
# reading max_SD
#
if {[get_load_line $f 0 0 lv] && [check_list_numeric $lv]} {
   set tmpAP(max_SD) [lindex $lv 0]
} else {
   close $f
   return 0
}
#
# check if tmpAP(max_SD) is of integer typ; if not return 0
#
if { [expr $tmpAP(max_SD) / round($tmpAP(max_SD))] != 1.0 } {close $f; return 0}
#
# return false if tmpAP(max_SD) < 1
#
if { $tmpAP(max_SD) < 1 } { close $f; return 0 }
#
# reading parameter of i-th size distribution
#
for {set i 1} {$i <= $tmpAP(max_SD)} {incr i} {
#
# skip line "i-th SD"
#
   if {![eof $f]} {
      gets $f line 
   } else {
      close $f
      return 0
   }
#
# skip line "SD belongs to j-th data set"
#
   if {![eof $f]} {
      gets $f line 
   } else {
      close $f
      return 0
   }
#
# read calcSDFF
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(calcSDFF) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(calcSDFF) $line
   } else {
      close $f
      return 0
   }
#
# read fitSDFF
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(fitSDFF) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(fitSDFF) $line
   } else {
      close $f
      return 0
   }

#
# read substrSDFF
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(substrSDFF) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(substrSDFF) $line
   } else {
      close $f
      return 0
   }

#
# read FF_l
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,l) {} }
      lappend tmpAP(FF,l) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_err
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,err) {} }
      lappend tmpAP(FF,err) $lv

   } else {
      close $f
      return 0
   }
#
# read FF_min
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,min) {} }
      lappend tmpAP(FF,min) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_max
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,max) {} }
      lappend tmpAP(FF,max) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_limits
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,limits) {} }
      lappend tmpAP(FF,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_active
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,active) {} }
      lappend tmpAP(FF,active) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_distr
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(FF,distr) {} }
      lappend tmpAP(FF,distr) $lv
   } else {
      close $f
      return 0
   }
#
# read FF_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(FF,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(FF,typestr) $line
   } else {
      close $f
      return 0
   }
#
# read SD_a
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,a) {} }
      lappend tmpAP(SD,a) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_err
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,err) {} }
      lappend tmpAP(SD,err) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_min
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,min) {} }
      lappend tmpAP(SD,min) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_max
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,max) {} }
      lappend tmpAP(SD,max) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_limits
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,limits) {} }
      lappend tmpAP(SD,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_active
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SD,active) {} }
      lappend tmpAP(SD,active) $lv
   } else {
      close $f
      return 0
   }
#
# read SD_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(SD,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(SD,typestr) $line
   } else {
      close $f
      return 0
   }

#
# read SQ_s
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,s) {} }
      lappend tmpAP(SQ,s) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_err
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,err) {} }
      lappend tmpAP(SQ,err) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_min
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,min) {} }
      lappend tmpAP(SQ,min) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_max
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,max) {} }
      lappend tmpAP(SQ,max) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_limits
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,limits) {} }
      lappend tmpAP(SQ,limits) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_active
#
   if {[get_load_line $f 0 9 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,active) {} }
      lappend tmpAP(SQ,active) $lv
   } else {
      close $f
      return 0
   }
#
# read SQ_typestr
#
   if {![eof $f]} {
      if {$i == 1} {set tmpAP(SQ,typestr) {} }
      gets $f line
      set line [string trim $line]
      lappend tmpAP(SQ,typestr) $line
   } else {
      close $f
      return 0
   }
#
# read SQ_how
#
   if {[get_load_line $f 0 0 lv] && [check_list_numeric $lv]} {
      if {$i == 1} {set tmpAP(SQ,how) {} }
      lappend tmpAP(SQ,how) $lv
   } else {
      close $f
      return 0
   }

if {$i == 1} {
   set tmpAP(<R^0>) {}
   set tmpAP(<R^1>) {} 
   set tmpAP(<R^2>) {}
   set tmpAP(<R^3>) {}
   set tmpAP(<R^4>) {}
   set tmpAP(<R^5>) {}
   set tmpAP(<R^6>) {}
   set tmpAP(<R^7>) {}
   set tmpAP(<R^8>) {}
   set tmpAP(R_li)  {}
   set tmpAP(R_lc)  {}
   set tmpAP(R_Ac)  {}
   set tmpAP(R_VP)  {}
   set tmpAP(R_RG)  {}
   set tmpAP(fp)    {}
}
lappend tmpAP(<R^0>) -1.0
lappend tmpAP(<R^1>) -1.0
lappend tmpAP(<R^2>) -1.0
lappend tmpAP(<R^3>) -1.0
lappend tmpAP(<R^4>) -1.0
lappend tmpAP(<R^5>) -1.0
lappend tmpAP(<R^6>) -1.0
lappend tmpAP(<R^7>) -1.0
lappend tmpAP(<R^8>) -1.0
lappend tmpAP(R_li)  -1.0
lappend tmpAP(R_lc)  -1.0
lappend tmpAP(R_Ac)  -1.0
lappend tmpAP(R_VP)  -1.0
lappend tmpAP(R_RG)  -1.0
lappend tmpAP(fp)    -1.0

} ;# end of for loop
cp_arr tmpAP AnalytPar
return 1
}

# gets the file version out of an already opened file
# the first floating point number in the first line is the version number
proc get_param_file_version { fileid } {

	set line_val {}
	if { [get_param_record $fileid double line_val] } {
		return [lindex $line_val 0]
	}
	return 0.0
}

# reads an entry/record/line from a sasfit parameter file
proc get_param_record { fileid type target_var } {

	upvar $target_var result

	if { [eof $fileid] } { return 0 }

	set result {}
	set line [string trim [gets $fileid]]
	if { ! [string compare $type wordchar] } {
		set result $line
	} else {
		foreach item [split $line] {
			if { [string is $type -strict $item] } {
				lappend result $item
			}
		}
	}
	return [llength $result]
}

# set the value of a non-list entry in a parameter set (AP)
proc set_analytpar_entry { fileid analytpar name type i } {

	upvar $analytpar AP
	set line {}
	if { ! [get_param_record $fileid $type line] } { 
		puts stderr "Could not read '$name' for the $i. parameter set !"
		return 0
	}
#	puts "line: $line [llength $line]"
#	lset AP($name) $i [lindex $line 0]
	if { ! [string compare $type wordchar] } {
		lset AP($name) $i $line
	} else {
		for {set k 0} {$k < [llength $line] && $k < [llength [lindex $AP($name) $i]]} {incr k} {
			lset AP($name) $i $k [lindex $line $k]
		}
	}
	return 1
}

#------------------------------------------------------------------------------
# load the parameter for fitting the  scattering curve by an analytical 
# given size distribution and analytical given form factor
# at the moment, the input AP has to be properly initialized (correct number of values per list)
#
proc load_sasfit_inp_file {AnalyticPar filename} {

	upvar $AnalyticPar AP

	if { ! [file exists $filename] } {
		puts stderr "Could not open file '$filename' !"
		return
	}
	set f [open $filename RDONLY]
	set version [get_param_file_version $f]

#	puts "file version: $version"
	if { $version <= 0.0 } {
		puts stderr "Could not determine parameter file format version !"
		return 0
	}

	# for debugging/verification
#	foreach {name val} [array get AP] { puts "ar($name): $val ([llength AP($name)])" }

	set line {}
	
	#
	# reading max_SD
	#
	set max_SD 0
	if { [get_param_record $f integer line] } {
		set max_SD [lindex $line 0]
	}

	# break up if there are no APs to read in
	if { $max_SD < 1 } { 
		close $f
		return 0
	}

#	puts "number of aps: $max_SD"

	#
	# reading parameter of i-th size distribution
	#

	# let the final return know of this variable
	set result 0
	for {set i 0} {$i < $max_SD} {incr i} {

		# reset the result for each iteration
		set result 0

		# add new AP (i.e. list entries with default values) if it doesn't exist yet
		if { [expr $i+1] > $AP(max_SD) } {
			add_analytpar AP $AP(isGlobal)
		}

		# skip line "i-th SD"
		get_param_record $f integer line

		# skip line "SD belongs to j-th data set"
		get_param_record $f integer line

		# read calcSDFF
		if { ! [set_analytpar_entry $f AP "calcSDFF" alpha $i] } { break }

		# read fitSDFF
		if { ! [set_analytpar_entry $f AP "fitSDFF" alpha $i] } { break }

		if { $version > 0.7 } {
			# read substrSDFF
			if { ! [set_analytpar_entry $f AP "substrSDFF" alpha $i] } { break }
		}

		### FF ###

		# read FF,l
		if { ! [set_analytpar_entry $f AP "FF,l" double $i] } { break }

		# read FF,err
		if { ! [set_analytpar_entry $f AP "FF,err" double $i] } { break }

		# read FF,min
		if { ! [set_analytpar_entry $f AP "FF,min" double $i] } { break }

		# read FF,max
		if { ! [set_analytpar_entry $f AP "FF,max" double $i] } { break }

		# read FF,limits
		if { ! [set_analytpar_entry $f AP "FF,limits" integer $i] } { break }

		# read FF,active
		if { ! [set_analytpar_entry $f AP "FF,active" integer $i] } { break }

		# read FF,distr
		if { ! [set_analytpar_entry $f AP "FF,distr" integer $i] } { break }

		# read FF,typestr
		if { ! [set_analytpar_entry $f AP "FF,typestr" wordchar $i] } { break }

		### SD ###

		# read SD,a
		if { ! [set_analytpar_entry $f AP "SD,a" double $i] } { break }

		# read SD,err
		if { ! [set_analytpar_entry $f AP "SD,err" double $i] } { break }

		# read SD,min
		if { ! [set_analytpar_entry $f AP "SD,min" double $i] } { break }

		# read SD,max
		if { ! [set_analytpar_entry $f AP "SD,max" double $i] } { break }

		# read SD,limits
		if { ! [set_analytpar_entry $f AP "SD,limits" integer $i] } { break }

		# read SD,active
		if { ! [set_analytpar_entry $f AP "SD,active" integer $i] } { break }

		# read SD,typestr
		if { ! [set_analytpar_entry $f AP "SD,typestr" wordchar $i] } { break }

		### SQ ###

		# read SQ,s
		if { ! [set_analytpar_entry $f AP "SQ,s" double $i] } { break }

		# read SQ,err
		if { ! [set_analytpar_entry $f AP "SQ,err" double $i] } { break }

		# read SQ,min
		if { ! [set_analytpar_entry $f AP "SQ,min" double $i] } { break }

		# read SQ,max
		if { ! [set_analytpar_entry $f AP "SQ,max" double $i] } { break }

		# read SQ,limits
		if { ! [set_analytpar_entry $f AP "SQ,limits" integer $i] } { break }

		# read SQ,active
		if { ! [set_analytpar_entry $f AP "SQ,active" integer $i] } { break }

		# read SQ,typestr
		if { ! [set_analytpar_entry $f AP "SQ,typestr" wordchar $i] } { break }

		# read SQ,how
		if { ! [set_analytpar_entry $f AP "SQ,how" integer $i] } { break }

		set result 1
#		set ffmenu $::sasfit(FF,root_menu)
#dropdownl_select_entry_by_name $ffmenu $ffmenu $AP(FF,typestr)
	} ;# end of for loop

	# for debugging/verification
#	puts "------------------------------------------------------------------------"
#	foreach {name val} [array get AP] { puts "ar($name): $val ([llength $AP($name)])" }
#	puts "========================================================================"

	close $f
	return $result
}


#------------------------------------------------------------------------------
# load the parameter for fitting the  scattering curve by an analytical 
# given size distribution and analytical given form factor
#
proc load_sasfit_inp_file_old {AanalyticPar filename} {

	upvar $AanalyticPar AnalytPar
	cp_arr AnalytPar tmpAP
	switch [get_par_version $filename] {
	   0.0 {set retval [load_sasfit_inp_file_ver_0_0 AnalytPar $filename]}
	   0.3 {set retval [load_sasfit_inp_file_ver_0_3 AnalytPar $filename]}
	   0.7 {set retval [load_sasfit_inp_file_ver_0_7 AnalytPar $filename]}
	   default {set retval 0}
	}
	return $retval
}


#------------------------------------------------------------------------------
#                   Create a variable of Type SESANSData
#
proc create_SESANSData {SESANSData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SESANSData Data
set Data(InputFormat) SESANS
set Data(FileName) ""
set Data(Ext) "ses"
set Data(in_out) "nm->nm"
set Data(xscale) 1
set Data(unit) nm
set Data(npoints) 0
set Data(LineSkip) 0
set Data(Comment) {}
set Data(error) 0
set Data(SEL) {}
set Data(DeltaSEL) {}
set Data(Pz) {}
set Data(DeltaPz) {}
set Data(Lambda) {}
set Data(DeltaLambda) {}
set Data(Qmax_x) {}
set Data(Qmax_y) {}
set Data(nonneg) 0
set Data(Gz-G0) "Pz->Gz-G0" ;# possible values {"Pz->Pz" "Pz->Gz-G0" "Pz->Pz^(lmax^2/l^2)"} 
}

#------------------------------------------------------------------------------
#                   Create a variable of Type ASCIIData
#
proc create_ALVData {ALVData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ALVData Data
set Data(InputFormat) ALV-5000
set Data(FileName) ""
set Data(Ext) "ASC"
set Data(in_out) "ms->s"
set Data(xscale) 1e-3
set Data(unit) s
set Data(npoints) 0
set Data(LineSkip) 0
set Data(Comment) {}
set Data(error) 0
set Data(tau) {}
set Data(g2_1) {}
set Data(error) {}
set Data(res) {}
set Data(nonneg) 0
}

#------------------------------------------------------------------------------
#                   Create a variable of Type ASCIIData
#
proc create_ASCIIData {ASCIIData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ASCIIData Data
set Data(InputFormat) xye
set Data(FileName) ""
set Data(Ext) "dat"
set Data(in_out) "nm^-1->nm^-1"
set Data(xscale) 1
set Data(unit) nm
set Data(npoints) 0
set Data(LineSkip) 0
set Data(Comment) {}
set Data(error) 0
set Data(Q) {}
set Data(I) {}
set Data(DI) {}
set Data(res) {}
set Data(nonneg) 0
}

#------------------------------------------------------------------------------
#      Reads ascii file "filename" and stores the contents in ASCIIData
#      return value 1 if file "filename" exists and readable
#      return value 0 otherwise
#      if varable "ASCIIData" doesn't exists, it will be created
#      "ASCIIData" is called by reference
#         ASCIIData(FileName)    : file name
#         ASCIIData(InputFormat) : input format (e: error DI, x: Q-value,
#                                  y: intensity value I, r: resolution)
#         ASCIIData(npoints)     : number of data points
#         ASCIIData(LineSkip)    : number of lines to be skipped at the 
#                                  beginning of a file
#         ASCIIData(Comment)     : the first "LineSkip" lines and all lines
#                                  which couldn't succesfull read
#         ASCIIData(Q)           : Q-values
#         ASCIIData(I)           : intensity values
#         ASCIIData(DI)          : statistical error of intensity
#         ASCIIData(res)         : resolution value
#         ASCIIData(error)       : statistical data error available
#         ASCIIData(res_available) : resolution value available
#
proc read_Ascii {filename ASCIIData args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $ASCIIData Data
	if {![info exist Data]} {create_ASCIIData Data}
	set Data(unit) xxx
	set Data(xscale) 1
	if {[string equal $Data(in_out) "nm^-1->Ångström^-1" ] == 1} {
	   set Data(unit) Ångström^-1
	   set Data(xscale) 0.1
	}
	if {[string equal $Data(in_out) "Ångström^-1->nm^-1" ] == 1} {
	   set Data(unit) nm^-1
	   set Data(xscale) 10
	}
	if {[string equal $Data(in_out) "Ångström^-1->Ångström^-1" ] == 1} {
	   set Data(unit) Ångström^-1
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "nm^-1->nm^-1" ] == 1} {
	   set Data(unit) nm^-1
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "nm->Ångström" ] == 1} {
	   set Data(unit) Ångström
	   set Data(xscale) 10
	}
	if {[string equal $Data(in_out) "Ångström->nm" ] == 1} {
	   set Data(unit) nm
	   set Data(xscale) 0.1
	}
	if {[string equal $Data(in_out) "Ångström->Ångström" ] == 1} {
	   set Data(unit) Ångström
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "nm->nm" ] == 1} {
	   set Data(unit) nm
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "ms->s" ] == 1} {
	   set Data(unit) s
	   set Data(xscale) 1e-3
	}
	if {[string equal $Data(in_out) "s->s" ] == 1} {
	   set Data(unit) s
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "s->ms" ] == 1} {
	   set Data(unit) ms
	   set Data(xscale) 1000.0
	}
	if {[string equal $Data(in_out) "ms->ms" ] == 1} {
	   set Data(unit) ms
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "mus->mus" ] == 1} {
	   set Data(unit) mus
	   set Data(xscale) 1.0
	}
	if {[string equal $Data(in_out) "mus->s" ] == 1} {
	   set Data(unit) s
	   set Data(xscale) 1.0e-6
	}
	if {[string equal $Data(in_out) "s->mus" ] == 1} {
	   set Data(unit) mus
	   set Data(xscale) 1e+6
	}

	set Data(InputFormat) [string tolower $Data(InputFormat)]
	if { !([regexp x $Data(InputFormat)] && [regexp y $Data(InputFormat)]) } {
	   return 0
	}

	if { !([regexp e $Data(InputFormat)]) } {
	    set e -1.0
	    set Data(error) 0
	} else { 
	    set Data(error) 1 
	}

	# causes problems for files over network which are readable but
	# somehow restricted
	#if { ([file readable $filename]) }

	set Data(FileName) $filename
	set Data(npoints)  0
	set Data(Q)        {}
	set Data(I)        {}
	set Data(DI)       {}
	set Data(res)      {}
	set Data(Comment)  {}
	set f [open $filename r]
	set lineno 0

	for {set i 1} {$i <= $Data(LineSkip)} {incr i} {
	   gets $f line
	   if { [eof $f] } {
	      close $f
	      return 2
	   }
	   incr lineno
	   lappend Data(Comment) $line
	}
	set fieldseparator       "\t ;"
	set fieldseparator_comma "\t ;,"
	while {![eof $f]} {
	   gets $f line
	   if {![string length $line]} { continue }; # skip empty lines
	   incr lineno

	   set tmpsplitline {}
	   set firstcomma [string first "," $line]
	   set firstdot   [string first "." $line]
	   if { $firstcomma >= 0
		&& $firstdot >= 0
		&& ($firstdot < $firstcomma)
	   } {
		   # assume comma as field separator
		   set tmpsplitline [split $line $fieldseparator_comma]
	   } else {
		   set tmpsplitline [split $line $fieldseparator]
	   }
	   set splitline {}
	   foreach i $tmpsplitline {
	      set leni [string length [string trim $i]]
	      if {$leni != 0} {
		 #lappend splitline $i
		 lappend splitline [regsub "," $i "."]
	      }
	   }
	   set e_ok  1
	   set x_ok  1
	   set y_ok  1
	   set r_ok  1
	   set e    -1
	   set res  0.0
	   set x    0.0
	   set y    0.0

	   if { [string length $Data(InputFormat)] <= [llength $splitline]} {
	     for {set i 0} {$i < [string length $Data(InputFormat)]} {incr i} {
		switch [string index $Data(InputFormat) $i] {
		   e   { set e_ok [scan [lindex $splitline $i] "%f" e ] }
		   x   { set x_ok [scan [lindex $splitline $i] "%f" x ] }
		   y   { set y_ok [scan [lindex $splitline $i] "%f" y ] }
		   r   { set r_ok [scan [lindex $splitline $i] "%f" res ] }
		}
	     }
	     if {$y == 0 && [info exists Data(zero_int_chkb)] && $Data(zero_int_chkb)} {
		     continue
	     }
	     if {($e == 0) && ([llength $args] == 0)} {
		 set e_ok 0
	     }
	     if { [regexp r $Data(InputFormat)] } {
		set Data(res_available) 1 
	     } else { 
		set Data(res_available) 0
		set res 0.0
	     }
	     if {$Data(nonneg) && $y_ok} {
		 if {$y < 0.0} {
		     set nonneg 0
		 } else {
		     set nonneg 1
		 }
	     } else {
		 set nonneg 1
	     }
	     if {$e_ok && $x_ok && $y_ok && $r_ok && $nonneg} {
		lappend Data(Q)   [expr $x*$Data(xscale)]
		lappend Data(I)   $y
		lappend Data(DI)  $e
		lappend Data(res) [expr $res*$Data(xscale)]
		incr Data(npoints)
	     } else {
		lappend Data(Comment) $line
	     }
	   } else {
	      lappend Data(Comment) $line
	   }
	}; # while

	close $f
	if {$Data(error) == 0} {
		# generate artificial error data if none was provided
		if { [catch {set Data(DI) [sasfit_guess_err 3 2 $Data(Q) $Data(I)]} msg] } {
			error $msg
		}
		set Data(error) 1
	}
	return 1
}

#------------------------------------------------------------------------------
# Reads data file "filename" in ALV-format and stores the contents in ALVData
#
#
# read_ALV opens the file "filename" and stores all the lines 
# corresponding to the block "BlockName" in "ALVData($BlockName)" as a list.
# After successfully opening the data file the array ALVData first is unset.
# The neccessary array indeces corresponding to the different blocks in
# the data file are automatically defined.
#
proc read_ALV {filename ALVData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ALVData Data
if {![info exist Data]} {create_ALVData Data}
#catch {unset Data}
if { !([file readable $filename] && [file isfile $filename])  } {
   return 0
}
set BlockName WasteBasket
set f [open $filename r]

while {![eof $f]} {
   gets $f line
   if { [eof $f] } {
      close $f
      return 1
   }
   set linelength [string length $line]
   if { $linelength > 0 } {
      set ch1st [string index $line 0]
      if {[string compare $ch1st "-"] != 0} {
         switch $ch1st {
            \"      { 
                     set tBlockname [string range $line 1 [expr $linelength-1]]
                     set BNword [split $tBlockname]
                     if { [string length [lindex $BNword 0] ] > 0 } {
                        set BlockName $tBlockname 
#[lindex $BNword 0]
                     } else { set BlockName WasteBasket }
	            }
			A {
                     set tBlockname [string range $line 0 [expr $linelength-1]]
                     set BNword [split $tBlockname]
                     if { [string length [lindex $BNword 0] ] > 0 } {
					    if {[string first ALV-5000 [lindex $BNword 0]] == 0} {
							set BlockName "ALV-5000header"
							set Data(Type) "ALV-5000"
						} else { lappend Data($BlockName)  "$line" }
                     } else { set BlockName WasteBasket }
				}
            default { lappend Data($BlockName)  "$line" }
        }
     }
   }
}
}

#------------------------------------------------------------------------------
#           get item "ItemName" of type "ItemType" in block "BlockName" 
#
# ALVData:   array containing the different blocks of a HMI data file which can
#            be read with "read_HMI $filename HMIData"
# BlockName: string containing the name of teh block from which an item to
#            be read.
# ItemName:  string containing the name of the item to be read.
# ItemType:  item type are coded according to the following sheme
#            w - word format (string without spaces)
#            t - text format (string, spaces allowed)
#            i - integer format 
#            r - real format
#            d - date/time format (VMS date/time string without spaces)
#                date: dd-monthname-yy, time hh:mm:ss
#
# supported blocks and items:
#
# The default procedure is to look for the character "=" in each element of
# the list HMIData($BlockName). If the left the string on the left side of
# "=" (without leading and trailing white characters) is equal to $ItemName
# the  right side will be read with in the format given in $ItemType". 
#
# Example file begin:
# ALV-5000/E-WIN Data
# ALV-5000/EPP-WIN Data
# Date :	"04.12.2007"
# Time :	"17:05:13"
# Samplename : 	""
# SampMemo(0) : 	""
# SampMemo(1) : 	""
# SampMemo(2) : 	""
# SampMemo(3) : 	""
# SampMemo(4) : 	""
# SampMemo(5) : 	""
# SampMemo(6) : 	""
# SampMemo(7) : 	""
# SampMemo(8) : 	""
# SampMemo(9) : 	""
# Temperature [K] :	     298.16000
# Viscosity [cp]  :	       0.89000
# Refractive Index:	       1.33200
# Wavelength [nm] :	     632.80000
# Angle [°]       :	      15.00100
# Duration [s]    :	        60
# Runs            :	         1
# Mode            :	"SINGLE AUTO CH0"
# MeanCR0 [kHz]   :	     390.12515
# MeanCR1 [kHz]   :	       0.00000
# 
# "Correlation"
#   1.25000E-004	  7.11621E-001
#   2.50000E-004	  8.31231E-001
#   3.75000E-004	  8.33391E-001
#   5.00000E-004	  8.31041E-001
#   6.25000E-004	  8.30614E-001
# end example file
#
# after reading the file with read_ALV filename ALVData, the array ALVData(File)
# contains a list with two elements "ItemName: value" 
# the call of "ALVgetItem ALVData ALV-5000header $Temperature \[K\]$ r" returns the float value for the temperature.
#
# If $Blockname is not a valid array index an empty string
# will be returned. If the item could not be found in one of the list 
# elements of ALVData($BlockName) the procedure will return an empty string. 
# If the item has been found everything behind the substring {$ItemName}: 
# in the list element will be converted corresponding to the value $ItemType. 
# Only the first occurence of {$ItemName}: will be analysed.
#
# For the blocks "Correlation", "Count Rate" and "StandardDeviation" the item name defines what kind of 
# data are stored in the data file. If ItemName is not listed in the 
# following list, the item will be treated in the default way (see above)
# Supported items are:
#    1.) ItemName = ALV-5000 
#         for block name "Correlation":
#           Varying number of lines of 2 real numbers each, separated by whitespace characters.
#           Each line contains a data points with tau [ms], g2(tau)-1 
#			It also looks for the blockname "StandardDeviation" in the same way and merges 
#           the normalized correlation function with the standard deviation of it.
#    2.) other standard ALV-formats are not supported yet
#
proc ALVgetItem {ALVData BlockName ItemName ItemType} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ALVData Data
#
# check if $BlockName is a valid array name of ALVData
#
if {![string compare $BlockName [array names Data $BlockName]]==0} {
   puts "invalid BlockName"
   return {}
}

# 
# check for a non-default treatment, then perform special treatment
# and return the value otherwise perfom default treatment
#

#
# check for one dimensional scattering data Q IQ DIQ DQ
# columns can be separated by " ", "\t" or/and ","
#

# "Correlation", "Count Rate" and "StandardDeviation"
if {![string compare $BlockName "Correlation"] && \
    ![string compare $ItemName  ALV5000]} {
   set tau        {}
   set I          {}
   set dI         {}
   foreach lineC $Data($BlockName) lineE $Data(StandardDeviation) {
      set x_ok 1
      set x2_ok 1
      set y_ok 1
      set e_ok 1
      set tmpsplitlineC [split $lineC "\t ,"]
      set tmpsplitlineE [split $lineE "\t ,"]
      set splitlineC {}
      set splitlineE {}
      foreach i $tmpsplitlineC {
         if {[llength $i] != 0} {
            lappend splitlineC $i
         }
      }
      foreach j  $tmpsplitlineE {
         if {[llength $j] != 0} {
            lappend splitlineE $j
         }
      }
      set x_ok [scan [lindex $splitlineC 0] "%f" x ]
      set y_ok [scan [lindex $splitlineC 1] "%f" y ]
      set x2_ok [scan [lindex $splitlineE 0] "%f" x2 ]
      set e_ok [scan [lindex $splitlineE 1] "%f" sigma ]
#
# lines which cannot be converted are ignored (no error message)
#
      if {$x_ok && $y_ok && $x2_ok && $e_ok && $x == $x2} {
	      switch $Data(in_out) {
			"ms->s"  	{ 	set Data(xscale) 1e-3	}
			"mus->s"  	{	set Data(xscale) 1e-6	}
			"s->s"  	{	set Data(xscale) 1.0	}
			default  	{	set Data(xscale) 1.0	}
			
		  }
		  lappend tau   [expr $x*$Data(xscale)]
	      lappend I  	$y
	      lappend dI  	$sigma
	      lappend dtau  -1
      }
   }
   
   set CorrData {}
   lappend CorrData $tau $I $dI $dtau
   return $CorrData
}

#
# default treatment
#

foreach line $Data($BlockName) {
   set where_eq [string first : $line]
   if {$where_eq != -1} {
      set leftside  [string range $line 0 [expr $where_eq - 1]]
      set rightside [string range $line [expr $where_eq + 1]  \
                              [expr [string length $line] - 1] ]
      set leftside [string trim $leftside]
      set rightside [string trim $rightside]
      if {([string compare $leftside $ItemName] == 0) &&  \
          ([string length $rightside] > 0) } {
#
# evaluate right side of string "line"
#
         switch $ItemType {
            w { set split_right_side [split $rightside "\t "]
                return [lindex $split_right_side 0] }
            t { return $rightside }
            i { set i_ok [scan $rightside "%d" itemvalue]
                if {$i_ok} { return $itemvalue } else { return {} }
              }
            r { set r_ok [scan $rightside "%f" itemvalue]
                if {$r_ok} { return $itemvalue } else { return {} }
              }
            d {
#
# for the moment d will be treated as t, i.e. no check if $rightside is
# really of date/time format
#
                return $rightside }
            default { puts "%ALVgetItem: unknown ItemType, inform me about it"
                      return {} }
         }
      } 
   }
}
return {5}
} 

proc getRunNo_SCA {DATASET Type} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if {([string compare $Type Standard] == 0) || ([string compare $Type FlippeOn] == 0) || ([string compare $Type FlippeOff] == 0)} {
		set whereTypeStart [lsearch $DATASET $Type]
		if {$whereTypeStart < 0} {return "not used"}
		set whereTypeEnd [lsearch -start $whereTypeStart $DATASET End]
		set RunNoList [lrange $DATASET [expr $whereTypeStart+1] [expr $whereTypeEnd-1]]
#		puts "*  Run    Ext-In   - SampleName"
		set NoExtName {}
		foreach run $RunNoList {
			set where_eq [string first - $run]
			if {$where_eq != -1} {
				set leftside  [string range $run 0 [expr $where_eq - 1]]
				set rightside [string range $run [expr $where_eq + 1]  \
                              [expr [string length $run] - 1] ]
				set leftside [string trim $leftside]
				set rightside [string trim $rightside]
				set NoExt [split [regexp -all -inline {\S+} $leftside]]
				if {[llength $NoExt] == 2} {
#					puts "[format "%8s" [lindex $NoExt 0]] [format "%5s" [lindex $NoExt 1]]     - $rightside"
					lappend NoExtName [list [lindex $NoExt 0] [lindex $NoExt 1] "$rightside"]
				}
				
			}
		}
		return $NoExtName
	} else {
		return "not used"
	}
}

proc read_SCA {filename SCAData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SCAData Data

catch {unset Data}
#if {![info exist Data]} {create_SCAData Data}
if { !([file readable $filename] && [file isfile $filename])  } {
   puts "cannot read $filename"
   return 0
}
set BlockName WasteBasket
set SubBlockName Waste
set TypeBlockName Basket
set SampleIndx 0
set f [open $filename r]

while {![eof $f]} {
   gets $f line
   if { [eof $f] } {
      close $f
      return 1
   }
   set linelength [string length $line]
   if { $linelength > 0 } {
      set ch1st [string index $line 0]
         switch $ch1st {
            \%      { 
                     set tBlockname [string range $line 1 [expr $linelength-1]]
                     set BNword [split $tBlockname]
                     if { [string length [lindex $BNword 0] ] > 0 } {
                        set BlockName [lindex $BNword 0]
                     } else { set BlockName WasteBasket }
					 if { [string compare $BlockName File] == 0 } {
                        set SubBlockName Info
                        set TypeBlockName File
                     } elseif {[string compare $BlockName Default] == 0} { 
						set SubBlockName Info
						set TypeBlockName Default
					 } else { 
						set SubBlockName Waste
						set TypeBlockName Basket
					 }
					}
            \*      { lappend Data(WasteBasket) $line }
			"$"      {
					 set sBlockname [string range $line 1 [expr $linelength-1]]
					 set BNword [split $sBlockname]
					 puts "puts $BlockName"
                     if { [string compare $BlockName Commands] == 0 } {
                        set SubBlockName [lindex $BNword 0]
                     } else { 
						set SubBlockName Waste
						set TypeBlockName Basket
					 }
					}
            default { 
					set where_eq [string first = $line]
					if {$where_eq != -1} {
						set leftside  [string range $line 0 [expr $where_eq - 1]]
						set rightside [string range $line [expr $where_eq + 1]  \
											[expr [string length $line] - 1] ]
						set leftside [string trim $leftside]
						set rightside [string trim $rightside]
						if {([string compare $leftside DataType] == 0) &&  \
							([string length $rightside] > 0) } {
							if {[string compare $rightside Sample] == 0} {
								incr SampleIndx
								set SubBlockName $SampleIndx
								set TypeBlockName ${rightside}
							} else {						
								set SubBlockName 0
								set TypeBlockName ${rightside}
							}
						} else {
							lappend Data($BlockName,$SubBlockName,$TypeBlockName)  $line 
						}
					} else {
						lappend Data($BlockName,$SubBlockName,$TypeBlockName)  $line 
					}
					}
        }
   }
}
}

proc SCAgetItem {SCAData BlockName ItemName args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SCAData Data
#
# check if $BlockName is a valid array name of HMIData
#
switch $BlockName {
	"File" 		{ return [HMIgetItem  Data "File,Info,File" $ItemName t]
				}
	"Default" 	{ return [HMIgetItem  Data "Default,Info,Default" $ItemName t]
				}
	"Commands"	{ switch $ItemName {
					"Water"	{ 	
					          if {![string compare "$BlockName,0,$ItemName" [array names Data "$BlockName,0,$ItemName"]]==0} {
								return "not used"
							  }
							  if {[llength $args] == 0} {return $Data($BlockName,0,$ItemName)}
							  if {[string compare [lindex $args 1] Standard] == 0} {return $Data($BlockName,0,$ItemName)}
							  return [HMIgetItem  Data "$BlockName,0,$ItemName" [lindex $args 0] t]
							}
					"WaterBackground"	{ 	
					          if {![string compare "$BlockName,0,$ItemName" [array names Data "$BlockName,0,$ItemName"]]==0} {
								return "not used"
							  }
							  if {[llength $args] == 0} {return $Data($BlockName,0,$ItemName)}
							  return [HMIgetItem  Data "$BlockName,0,$ItemName" [lindex $args 0] t]
							}
					"Cadmium"	{ 	
					          if {![string compare "$BlockName,0,$ItemName" [array names Data "$BlockName,0,$ItemName"]]==0} {
								return "not used"
							  }
							  if {[llength $args] == 0} {return $Data($BlockName,0,$ItemName)}
							  return [HMIgetItem  Data "$BlockName,0,$ItemName" [lindex $args 0] t]
							}
					"SampleBackground"	{ 	
					          if {![string compare "$BlockName,0,$ItemName" [array names Data "$BlockName,0,$ItemName"]]==0} {
								return "not used"
							  }
							  if {[llength $args] == 0} {return $Data($BlockName,0,$ItemName)}
							  return [HMIgetItem  Data "$BlockName,0,$ItemName" [lindex $args 0] t]
							}
					"Sample"	{ 	if {[llength $args] < 1} {return "more arguments are required"}
					          if {![string compare "$BlockName,[lindex $args 0],$ItemName" [array names Data "$BlockName,[lindex $args 0],$ItemName"]]==0} {
								return "not used"
							  }
							  if {[llength $args] == 1} {return $Data($BlockName,[lindex $args 0],$ItemName)}
							  return [HMIgetItem  Data "$BlockName,[lindex $args 0],$ItemName" [lindex $args 1] t]
							}
					default { return UNKNOWN
							}
				  }
				}
	default		{ return UNKNOWN
				}	
}
}

#------------------------------------------------------------------------------
# Reads data file "filename" in HMI-format and stores the contents in HMIData
#
# for a detailed description of HMI-format see manual of the SANS program 
# package of Uwe Keiderling (Hahn Meitner Institut Berlin)
#
# read_HMI read opens the file "filename" and stores all the lines 
# corresponding to the block "BlockName" in "HMIData($BlockName)" as a list.
# After successfully opening the data file the array HMIData first is unset.
# The neccessary array indeces corresponding to the different blocks in
# the data file are automatically defined.
#
proc read_HMI {filename HMIData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $HMIData Data

catch {unset Data}
#if {![info exist Data]} {create_HMIData Data}
if { !([file readable $filename] && [file isfile $filename])  } {
   return 0
}
set BlockName WasteBasket
set f [open $filename r]

while {![eof $f]} {
   gets $f line
   if { [eof $f] } {
      close $f
      return 1
   }
   set linelength [string length $line]
   if { $linelength > 0 } {
      set ch1st [string index $line 0]
      if {[string compare $ch1st "-"] != 0} {
         switch $ch1st {
            \%      { 
                     set tBlockname [string range $line 1 [expr $linelength-1]]
                     set BNword [split $tBlockname]
                     if { [string length [lindex $BNword 0] ] > 0 } {
                        set BlockName [lindex $BNword 0]
                     } else { set BlockName WasteBasket }
	            }
            \*      { lappend Data(WasteBasket) $line }
            default { lappend Data($BlockName)  $line }
        }
     } else {lappend Data($BlockName)  $line}
   }
}
}

#------------------------------------------------------------------------------
#           get item "ItemName" of type "ItemType" in block "BlockName" 
#
# HMIData:   array containing the different blocks of a HMI data file which can
#            be read with "read_HMI $filename HMIData"
# BlockName: string containing the name of teh block from which an item to
#            be read.
# ItemName:  string containing the name of the item to be read.
# ItemType:  item type are coded according to the following sheme
#            w - word format (string without spaces)
#            t - text format (string, spaces allowed)
#            i - integer format 
#            r - real format
#            d - date/time format (VMS date/time string without spaces)
#                date: dd-monthname-yy, time hh:mm:ss
#
# supported blocks and items:
#
# The default procedure is to look for the character "=" in each element of
# the list HMIData($BlockName). If the left the string on the left side of
# "=" (without leading and trailing white characters) is equal to $ItemName
# the  right side will be read with in the format given in $ItemType". 
#
# Example file begin:
# %File
# FileName = Dnnnn.ee
# Type = SANSDIso
# end example file
#
# after reading the file with read_HMI filename HMIDate, the array HMIData(File)
# contains a list with two elements "FileName=Dnnnnnnn.ee" "Type=SANSDIso".
# the call of "HMIgetItem HMIData File FileName w" returns the string "Dnnnn.ee"
# and the call"HMIgetItem HMIData File type w" returns the string "SANSDIso".
#
# If $Blockname is not a valid array index an empty string
# will be returned. If the item could not be found in one of the list 
# elements of HMIData($BlockName) the procedure will return an empty string. 
# If the item has been found everything behind the substring {$ItemName}= 
# in the list element will be converted corresponding to the value $ItemType. 
# Only the first occurence of {$ItemName}= will be analysed.
#
# For the blocks "Errors" and "Counts" the item name defines what kind of 
# data are stored in the data file. If ItemName is not listed in the 
# following list, the item will be treated in the default way (see above)
# Supported items are:
#    1.) ItemName = SANSDIso 
#         for block name "Counts":
#           Varying number of lines of 4 real numbers each, separated by comma.
#           Each line contains a data points with Q [nm^-1], Intensity(Q) 
#           [cm^-1*sr^-1], and Error(Q) [cm^-1*sr^-1] and the q-resolution in [nm^-1].
#           The return value is a list of 3 elements, each containing
#           a list of Q, Intensity(Q) and Error(Q) values
#         for block name "Error"
#           default treatment descibed above
#    2.) other standard HMI-"Counts"-items like SANSDAni, SANSDRaw, SANSP, 
#        SANSCIso, and SANSCAni are not supported
#
proc HMIgetItem {HMIData BlockName ItemName ItemType} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $HMIData Data
#
# check if $BlockName is a valid array name of HMIData
#
if {![string compare $BlockName [array names Data $BlockName]]==0} { 
   return {}
}

# 
# check for a non-default treatment, then perform special treatment
# and return the value otherwise perfom default treatment
#

#
# check for one dimensional scattering data Q IQ DIQ DQ
# columns can be separated by " ", "\t" or/and ","
#
if {![string compare $BlockName Counts] && \
    ![string compare $ItemName  SANSDIso]} {
   set Q          {}
   set Intensity  {}
   set Error      {}
   set Resolution {}
   foreach line $Data($BlockName) {
      set x_ok 1
      set y_ok 1
      set e_ok 1
      set r_ok 1
      set e   -1
      set res 0.0
      set x   0.0
      set y   0.0
      set tmpsplitline [split $line "\t ,"]
      set splitline {}
      foreach i $tmpsplitline {
         if {[llength $i] != 0} {
            lappend splitline $i
         }
      }
      set x_ok [scan [lindex $splitline 0] "%f" x ]
      set y_ok [scan [lindex $splitline 1] "%f" y ]
      set e_ok [scan [lindex $splitline 2] "%f" e ]
      set r_ok [scan [lindex $splitline 3] "%f" res ]
#
# lines which cannot be converted are ignored (no error message)
#
      if {$e_ok && $x_ok && $y_ok} {
# lines with non-positive error bars are ignored
	  if {$e > 0} {
	      lappend Q          $x
	      lappend Intensity  $y
	      lappend Error      $e
	      lappend Resolution $res
	  }
      }
   }
   set ScattData {}
   lappend ScattData $Q $Intensity $Error $Resolution
   return $ScattData
}

if {![string compare $BlockName Mask] && \
    ![string compare $ItemName  SANSMAni]} {
	set maskdata {}
	for {set i 0} {$i < [HMIgetItem Data File DataSizeX i]} {incr i} {
		set maskline {}
		for {set j [expr [HMIgetItem Data File DataSizeY i]-1]} {$j >= 0} {incr j -1} {
			set maskji [string index [lindex $Data(Mask) $j] $i]
			if {[string compare # $maskji]==0} {
				lappend maskline 0
			} else {
				lappend maskline 1
			}
		}
		lappend maskdata $maskline
	}
	return $maskdata
}

if {![string compare $BlockName Counts] && \
    ![string compare $ItemName  SANSDAni]} {
	set SANSDAniData {}
	set nPix [expr round(sqrt([HMIgetItem Data File DataSize i]))]
	set nLines [expr round([HMIgetItem Data File DataSize i]/8)]
	puts "$nPix $nLines"
	set cPix 0
	set AData ""
	for {set nL 0} {$nL < $nLines} {incr nL} {
		incr cPix 8
		lappend AData [scan [lindex $Data(Counts) $nL] %10f%10f%10f%10f%10f%10f%10f%10f]
		set AData [join $AData]
		if {$cPix==$nPix} {
			lappend SANSDAniData $AData
			set cPix 0
			set AData ""
		}
	}
	set DAniData {}
	for {set i 0} {$i < $nPix} {incr i} {
		set AData {}
		for {set j 0} {$j < $nPix} {incr j} {
			set adataji [lindex [lindex $SANSDAniData $j] $i]
			lappend AData $adataji
		}
		lappend DAniData $AData
	}
	return $DAniData
}

if {![string compare $BlockName Errors] && \
    ![string compare $ItemName  SANSDAni]} {
	set SANSDAniData {}
	set nPix [expr round(sqrt([HMIgetItem Data File DataSize i]))]
	set nLines [expr round([HMIgetItem Data File DataSize i]/8)]
	puts "$nPix $nLines"
	set cPix 0
	set AData ""
	for {set nL 0} {$nL < $nLines} {incr nL} {
		incr cPix 8
		lappend AData [scan [lindex $Data(Counts) $nL] %10f%10f%10f%10f%10f%10f%10f%10f]
		set AData [join $AData]
		if {$cPix==$nPix} {
			lappend SANSDAniData $AData
			set cPix 0
			set AData ""
		}
	}
	set DAniData {}
	for {set i 0} {$i < $nPix} {incr i} {
		set AData {}
		for {set j 0} {$j < $nPix} {incr j} {
			set adataji [lindex [lindex $SANSDAniData $j] $i]
			lappend AData $adataji
		}
		lappend DAniData $AData
	}
	return $DAniData
}
#
# default treatment
#

foreach line $Data($BlockName) {
   set where_eq [string first = $line]
   if {$where_eq != -1} {
      set leftside  [string range $line 0 [expr $where_eq - 1]]
      set rightside [string range $line [expr $where_eq + 1]  \
                              [expr [string length $line] - 1] ]
      set leftside [string trim $leftside]
      set rightside [string trim $rightside]
      if {([string compare $leftside $ItemName] == 0) &&  \
          ([string length $rightside] > 0) } {
#
# evaluate right side of string "line"
#
         switch $ItemType {
            w { set split_right_side [split $rightside "\t "]
                return [lindex $split_right_side 0] }
            t { return $rightside }
            i { set i_ok [scan $rightside "%d" itemvalue]
                if {$i_ok} { return $itemvalue } else { return {} }
              }
            r { set r_ok [scan $rightside "%f" itemvalue]
                if {$r_ok} { return $itemvalue } else { return {} }
              }
            d {
#
# for the moment d will be treated as t, i.e. no check if $rightside is
# really of date/time format
#
                return $rightside }
            default { puts "%HMIgetItem: unknown ItemType, inform me about it"
                      return {} }
         }
      } 
   }
}
return {5}
} 

#------------------------------------------------------------------------------
# Reads data file "filename" in SESANS-format and stores the contents in SESANSData
#
#
# read_SESANS opens the file "filename" and stores all the lines 
# corresponding to the block "BlockName" in "SESANSData($BlockName)" as a list.
# After successfully opening the data file the array SESANSData first is unset.
# The neccessary array indeces corresponding to the different blocks in
# the data file are automatically defined.
#
proc read_SESANS {filename SESANSData} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SESANSData Data
if {![info exist Data]} {create_SESANSData Data}
#catch {unset Data}
if { !([file readable $filename] && [file isfile $filename])  } {
   return 0
}
set BlockName SESANSHeader
set f [open $filename r]

while {![eof $f]} {
   gets $f line
   if { [eof $f] } {
      close $f
      return 1
   }
   set linelength [string length $line]
   if { $linelength > 0 } {
      set linetrim [string trim $line]
	  set key [string range $linetrim 0 15]
      switch $key {
            "spin echo length"      { 
                     set BlockName SESANSData
	            }
            default { lappend Data($BlockName)  "$line" }
     }
   }
}
}

#------------------------------------------------------------------------------
#           get item "ItemName" of type "ItemType" in block "BlockName" 
#
# SESANSData: array containing the different blocks of a SESANS data file which can
#             be read with "read_SESANS $filename SESANSData"
# BlockName:  string containing the name of teh block from which an item to
#             be read.
# ItemName:   string containing the name of the item to be read.
# ItemType:   item type are coded according to the following sheme
#            w - word format (string without spaces)
#            t - text format (string, spaces allowed)
#            i - integer format 
#            r - real format
#            d - date/time format (VMS date/time string without spaces)
#                date: dd-monthname-yy, time hh:mm:ss
#
# supported blocks and items:
#
# The default procedure is to look for the character "\t" in each element of
# the list SESANSData($BlockName). If the left the string on the left side of
# "=" (without leading and trailing white characters) is equal to $ItemName
# the  right side will be read with in the format given in $ItemType". 
#
# Example file begin:
# #############################################################################################
# DataFileTitle 	Oil & Surfactants (Shell), Sine ++ only 
# Sample 	Sample 6 (2.0% NaCl) 
# Settings 	D1=D2=20x8 mm,Ds = 12x10 mm (WxH), GF1 =scanning, GF2 = 2.5 A.  
# Operator 	CPD 
# Date 	wo 1 apr 2015 15:52:53 
# ScanType 	sine one element scan 
# Thickness [cm] 	0.4 
# Q_zmax [\AA^-1] 	0.05 
# Q_ymax [\AA^-1] 	0.05 
#   
# spin echo length [nm] 	 error SEL 	 wavelength [nm] 	 error wavelength 	 polarisation 	 error pol 
# 28.77	1.4385	0.203	0.05	0.83661	0.0084561
# 34.587	1.7294	0.203	0.05	0.76921	0.0083706
# 40.468	2.0234	0.203	0.05	0.72961	0.0081288
# 46.406	2.3203	0.203	0.05	0.6771	0.0081332
# 52.398	2.6199	0.203	0.05	0.6257	0.0080548
# 58.579	2.9289	0.203	0.05	0.58638	0.0080726
# 64.764	3.2382	0.203	0.05	0.55493	0.0079709
# 71.127	3.5564	0.203	0.05	0.51375	0.0080619
# 77.541	3.877	0.203	0.05	0.488	0.0080518
# 84.115	4.2058	0.203	0.05	0.45708	0.0081632
# 90.819	4.5409	0.203	0.05	0.45625	0.0082216
# 97.607	4.8804	0.203	0.05	0.4605	0.0081209
# 104.54	5.2268	0.203	0.05	0.47003	0.0081647
# #############################################################################################
# end example file
#
# after reading the file with read_SESANS filename SESANSData, the array SESANSData(File)
# contains a list with two elements "ItemName: value" 
# the call of "SESANSgetItem SESANSData SESANSheader $Q_zmax \[\AA^-1\]$ r" returns the float value for Q_zmax.
#
# If $Blockname is not a valid array index an empty string
# will be returned. If the item could not be found in one of the list 
# elements of SESANSData($BlockName) the procedure will return an empty string. 
# If the item has been found everything behind the substring {$ItemName}: 
# in the list element will be converted corresponding to the value $ItemType. 
# Only the first occurence of {$ItemName}: will be analysed.
#
# For the block "SESANS" the item name defines what kind of 
# data are stored in the data file. If ItemName is not listed in the 
# following list, the item will be treated in the default way (see above)
# Supported items are:
#    1.) ItemName = SESANS 
#         for block name "spin echo":
#           Varying number of lines of 6 real numbers each, separated by whitespace characters.
#           Each line contains a data points with SEL DeltaSEL Lambda DeltaLambda P DeltaP
#    2.) other standard SESANS-formats are not supported yet
#
proc SESANSgetItem {SESANSData BlockName ItemName ItemType} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $SESANSData Data
#
# check if $BlockName is a valid array name of ALVData
#
if {![string compare $BlockName [array names Data $BlockName]]==0} {
   puts "invalid BlockName"
   return {}
}

# 
# check for a non-default treatment, then perform special treatment
# and return the value otherwise perfom default treatment
#

#
# check for one dimensional scattering data SEL DeltaSEL Lambda DeltaLambda P DeltaP
# columns can be separated by " ", "\t" or/and ","
#

if {![string compare $BlockName "SESANSData"] && \
    ![string compare $ItemName  SESANS]} {
   set SEL         {}
   set DeltaSEL    {}
   set Lambda      {}
   set DeltaLambda {}
   set P           {}
   set DeltaP      {}
   set Y           {}
   set DeltaY      {}
   set resY        {}
   foreach line $Data($BlockName) {
      set x_ok 1
      set xe_ok 1
      set y_ok 1
	  set ye_ok 1
      set z_ok 1
      set ze_ok 1
      set tmpsplitline [split $line "\t ,"]]
      set splitline {}
      foreach i $tmpsplitline {
         if {[llength $i] != 0} {
            lappend splitline $i
         }
      }
      set x_ok  [scan [lindex $splitline 0] "%f" x  ]
      set xe_ok [scan [lindex $splitline 1] "%f" xe ]
      set y_ok  [scan [lindex $splitline 2] "%f" y  ]
      set ye_ok [scan [lindex $splitline 3] "%f" ye ]
      set z_ok  [scan [lindex $splitline 4] "%f" z  ]
      set ze_ok [scan [lindex $splitline 5] "%f" ze ]
#
# lines which cannot be converted are ignored (no error message)
#
      if {$x_ok && $y_ok && $z_ok && $xe_ok && $ye_ok && $ze_ok && $z>0} {
	     lappend SEL $x
		 lappend DeltaSEL $xe
		 lappend Lambda $y
		 lappend DeltaLambda $ye
		 lappend P $z
		 lappend DeltaP $ze
		 lappend resY [expr 10*[SESANSgetItem Data SESANSHeader "Q_zmax \[\\AA\^-1\]" r]]
		 switch $Data(Gz-G0) {
			"Pz->Pz"  			{ 	
									lappend Y $z	
									lappend DeltaY $ze	
								}
			"Pz->Gz-G0"  		{	set M_PI [expr 4*atan(1)]
									set thickness [SESANSgetItem Data SESANSHeader "Thickness \[cm\]" r]
									set tmp [expr log($z)*(1.0)/($y*$y*$thickness)]
									lappend Y $tmp
									lappend DeltaY [expr sqrt(pow(abs((1.0)/($y*$y*$thickness)),2)*($ze/$z)*($ze/$z)+0*4*($tmp*$ye/$y)*($tmp*$ye/$y))]
								}
			"Pz->Pz^(lmax^2/l^2)"	{ 
									lappend Y $z	
									lappend DeltaY $ze
								}
		 }
      }
 }
 
if ([string compare $Data(Gz-G0) "Pz->Pz^(lmax^2/l^2)"]==0) {
	set lmax [lindex [lsort -real -decreasing $Lambda] 0]
	set nY {}
	set nDY {}
	set M_PI [expr 4*atan(1)]
	set MPI2 [expr $M_PI*$M_PI]
	foreach l $Lambda y $Y Dy $DeltaY {
		set cex [expr $lmax*$lmax/($l*$l)]
#		puts "lmax: $lmax lambda $l"
		lappend nY [expr pow($y,$cex)]
		lappend nDY [expr abs(pow($y,$cex)*$cex*$Dy/$y)]
	}
	set Y $nY
	set DeltaY $nDY
}
	set SESANSData {}
	lappend SESANSData $SEL $Y $DeltaY $resY
	return $SESANSData
}

#
# default treatment
#

foreach line $Data($BlockName) {
   set where_eq [string first \t $line]
   if {$where_eq != -1} {
      set leftside  [string range $line 0 [expr $where_eq - 1]]
      set rightside [string range $line [expr $where_eq + 1]  \
                              [expr [string length $line] - 1] ]
      set leftside [string trim $leftside]
      set rightside [string trim $rightside]
      if {([string compare $leftside $ItemName] == 0) &&  \
          ([string length $rightside] > 0) } {
#
# evaluate right side of string "line"
#
         switch $ItemType {
            w { set split_right_side [split $rightside "\t "]
                return [lindex $split_right_side 0] }
            t { return $rightside }
            i { set i_ok [scan $rightside "%d" itemvalue]
                if {$i_ok} { return $itemvalue } else { return {} }
              }
            r { set r_ok [scan $rightside "%f" itemvalue]
                if {$r_ok} { return $itemvalue } else { return {} }
              }
            d {
#
# for the moment d will be treated as t, i.e. no check if $rightside is
# really of date/time format
#
                return $rightside }
            default { puts "%SESANSgetItem: unknown ItemType, inform me about it"
                      return {} }
         }
      } 
   }
}
return {5}
} 

#------------------------------------------------------------------------------
#      Reads ascii file "filename" and stores the contents in ASCIIData
#      return value 1 if file "filename" exists and readable
#      return value 0 otherwise
#      if varable "ASCIIData" doesn't exists, it will be created
#      "ASCIIData" is called by reference
#         ASCIIData(FileName)    : file name
#         ASCIIData(InputFormat) : input format (e: error DI, x: Q-value,
#                                  y: intensity value I, r: resolution)
#         ASCIIData(npoints)     : number of data points
#         ASCIIData(LineSkip)    : number of lines to be skipped at the 
#                                  beginning of a file
#         ASCIIData(Comment)     : the first "LineSkip" lines and all lines
#                                  which couldn't succesfull read
#         ASCIIData(Q)           : Q-values
#         ASCIIData(I)           : intensity values
#         ASCIIData(DI)          : statistical error of intensity
#         ASCIIData(res)         : resolution value
#         ASCIIData(error)       : statistical data error available
#         ASCIIData(res_available) : resolution value available
#
proc read_Clipboard {ASCIIData args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ASCIIData Data
#puts $args
if {![info exist Data]} {create_ASCIIData Data}
if {[string equal $Data(in_out) "nm^-1->Ångström^-1" ] == 1} {
   set Data(unit) Ångström^-1
   set Data(xscale) 0.1
}
if {[string equal $Data(in_out) "Ångström^-1->nm^-1" ] == 1} {
   set Data(unit) nm^-1
   set Data(xscale) 10.0
}
if {[string equal $Data(in_out) "Ångström^-1->Ångström^-1" ] == 1} {
   set Data(unit) Ångström^-1
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "nm^-1->nm^-1" ] == 1} {
   set Data(unit) nm^-1
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "nm->Ångström" ] == 1} {
   set Data(unit) Ångström
   set Data(xscale) 10
}
if {[string equal $Data(in_out) "Ångström->nm" ] == 1} {
   set Data(unit) nm
   set Data(xscale) 0.1
}
if {[string equal $Data(in_out) "Ångström->Ångström" ] == 1} {
   set Data(unit) Ångström
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "nm->nm" ] == 1} {
   set Data(unit) nm
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "ms->s" ] == 1} {
   set Data(unit) s
   set Data(xscale) 1.0e-3
}
if {[string equal $Data(in_out) "s->s" ] == 1} {
   set Data(unit) s
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "s->ms" ] == 1} {
   set Data(unit) ms
   set Data(xscale) 1e3
}
if {[string equal $Data(in_out) "ms->ms" ] == 1} {
   set Data(unit) ms
   set Data(xscale) 1.0
}

set ClipboardData [selection get -selection CLIPBOARD]

set Data(InputFormat) [string tolower $Data(InputFormat)]
if { !([regexp x $Data(InputFormat)] && [regexp y $Data(InputFormat)]) } {
   return 0
}
if { [llength $ClipboardData] < [string length $Data(InputFormat)] } {
   return 0
}
set Data(FileName) "clipboard data"
set Data(npoints)  0
set Data(Q)        {}
set Data(I)        {}
set Data(DI)       {}
set Data(res)      {}
set Data(Comment)  {}
#set f [open $filename r]
set lineno 0

#for {set i 1} {$i <= $Data(LineSkip)} {incr i} {
#   gets $f line
#   if { [eof $f] } {
#      close $f
#      return 2
#   }
#   incr lineno
#   lappend Data(Comment) $line
#}

set ClipboardSplit [split $ClipboardData \n]
 
foreach line $ClipboardSplit {
   incr lineno
   set tmpsplitline [split $line "\t ,"]
   set splitline {}
   foreach i $tmpsplitline {
      if {[llength $i] != 0} {
         lappend splitline $i
      }
   }
   set e_ok  1
   set x_ok  1
   set y_ok  1
   set r_ok  1
   set e    -1
   set res  0.0
   set x    0.0
   set y    0.0
   if { [string length $Data(InputFormat)] <= [llength $splitline]} {
     for {set i 0} {$i < [string length $Data(InputFormat)]} {incr i} {
        switch [string index $Data(InputFormat) $i] {
           e   { set e_ok [scan [lindex $splitline $i] "%f" e ] }
           x   { set x_ok [scan [lindex $splitline $i] "%f" x ] }
           y   { set y_ok [scan [lindex $splitline $i] "%f" y ] }
           res { set r_ok [scan [lindex $splitline $i] "%f" r ] }
        }
     }
     if {($e == 0) && ([llength $args] == 0)} {
         set e_ok 0
     }
     if { !([regexp e $Data(InputFormat)]) } {
        set e -1.0
        set Data(error) 0
     } else { 
	 set Data(error) 1 
     }
     if { !([regexp r $Data(InputFormat)]) } {
        set res 0.0
        set Data(res_available) 0
     } else { set Data(res_available) 1 }
     if {$e_ok && $x_ok && $y_ok && $r_ok} {
        lappend Data(Q)   [expr $x*$Data(xscale)]
        lappend Data(I)   $y
        lappend Data(DI)  $e
        lappend Data(res) [expr $res*$Data(xscale)]
        incr Data(npoints)
     } else {
        lappend Data(Comment) $line
puts stderr "ERROR in line $lineno ??$line??"
     }
   } else {
      lappend Data(Comment) $line
   }
}

   if {$Data(error) == 0} {
       if { [catch {set Data(DI) [sasfit_guess_err 3 2 $Data(Q) $Data(I)]} msg] } {
	  error $msg
       }
       tk_messageBox -type ok -icon warning -message "as no error bar was supplied the error bar was estimated\nby looking on the local smoothness of the data"
       set Data(error) 1
   }
}

proc readANDaverage_Clipboard {ASCIIData args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ASCIIData Data
#puts $args
if {![info exist Data]} {create_ASCIIData Data}
if {[string equal $Data(in_out) "nm^-1->Ångström^-1" ] == 1} {
   set Data(unit) Ångström^-1
   set Data(xscale) 0.1
}
if {[string equal $Data(in_out) "Ångström^-1->nm^-1" ] == 1} {
   set Data(unit) nm^-1
   set Data(xscale) 10.0
}
if {[string equal $Data(in_out) "Ångström^-1->Ångström^-1" ] == 1} {
   set Data(unit) Ångström^-1
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "nm^-1->nm^-1" ] == 1} {
   set Data(unit) nm
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "ms->s" ] == 1} {
   set Data(unit) s
   set Data(xscale) 1000.0
}
if {[string equal $Data(in_out) "s->s" ] == 1} {
   set Data(unit) s
   set Data(xscale) 1.0
}
if {[string equal $Data(in_out) "s->ms" ] == 1} {
   set Data(unit) ms
   set Data(xscale) 1e-3
}
if {[string equal $Data(in_out) "ms->ms" ] == 1} {
   set Data(unit) ms
   set Data(xscale) 1000.0
}

set ClipboardData [selection get -selection CLIPBOARD]

if { [llength $ClipboardData] < 3 } {
   return 0
}
set Data(FileName) "clipboard data"
set Data(npoints)  0
set Data(Q)        {}
set Data(I)        {}
set Data(DI)       {}
set Data(res)      {}
set Data(Comment)  {}
set lineno 0

set ClipboardSplit [split $ClipboardData \n]
 
foreach line $ClipboardSplit {
   incr lineno
   set tmpsplitline [split $line "\t ,"]
   set splitline {}
   foreach i $tmpsplitline {
      if {[llength $i] != 0} {
         lappend splitline $i
      }
   }
   set e_ok 1
   set x_ok 1
   set y_ok 1
   set r_ok 1
   set x    0.0
   set res  0.0
   set y    {}
   set e    {}
   set Data(res_available) 0

   if {[llength $splitline] >= 3} {
     set x_ok [scan [lindex $splitline 0] "%f" x ]
     for {set i 1} {$i < [llength $splitline]} {incr i 2} {
        set y_ok [scan [lindex $splitline       $i   ] "%f" ty ]
        set e_ok [scan [lindex $splitline [expr $i+1]] "%f" te ]
	if {$x_ok && $y_ok && $e_ok} {
	    lappend y $ty
	    lappend e $te
	} 
	set y_ok 1
	set e_ok 1
     }
     if {([llength $y] > 0) && $x_ok} {
        lappend Data(Q)   [expr $x*$Data(xscale)]
        set e_NotZero 0
	set yav 0
	set eav 0
	foreach ty $y te $e {
	    if {$te > 0} {
		set wi [expr 1./($te*$te)]
		set yav [expr $yav+$ty*$wi]
		set eav [expr $eav+$wi]
		set e_NotZero 1
	    }
	}
	if {$e_NotZero} {
	    set yav [expr $yav/$eav]
	    set eav [expr sqrt(1./$eav)]
	}
        lappend Data(I)   $yav
        lappend Data(DI)  $eav
        lappend Data(res) [expr $res*$Data(xscale)]
        incr Data(npoints)
	set Data(error) 1
     } else {
        lappend Data(Comment) $line
puts stderr "ERROR in line $lineno ??$line??"
     }
   } else {
      lappend Data(Comment) $line
   }
}
}


#------------------------------------------------------------------------------
# export data into Ascii file, the Ascii file will have 4*n columns with
# with n equal to the number of curves stored in bltGraph
# n   column = x-values of curve n
# n+1 column = y-values of curve n
# n+2 column = dy-values of curve n
# n+3 column = res-value of curve n
proc export_ascii_data {filename bltGraph} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $bltGraph Graph
for {set i 0} {$i < $Graph(e,element)} {incr i} {
   set  xdata($i)  [lindex $Graph(e,xdata)   $i]
   set  ydata($i)  [lindex $Graph(e,ydata)   $i]
   set dydata($i)  [lindex $Graph(e,error)   $i]
   set resdata($i) [lindex $Graph(e,resdata) $i]
}
set f [open $filename w]
set max 0
for {set i 0} {$i < $Graph(e,element)} {incr i} {
  set tmax [llength $xdata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $ydata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $dydata($i)]
  if {$tmax > $max} { set max $tmax }
}
set columnwidth 16
# see http://www.tcl.tk/man/tcl8.4/TclCmd/format.htm#M19
set precision [expr $columnwidth - 9]
set floatfield "% ${columnwidth}.${precision}E"
set emptyfield [format "%${columnwidth}s" ""]
# output looks like this (columnwidth == 14):
#    7.81611E+00  7.47467E-132  -1.00000E+00   0.00000E+00

for {set k 0} {$k < $max} {incr k} {
   set row [list]
   for {set i 0} {$i < $Graph(e,element)} {incr i} {
      if {[llength [lindex $xdata($i) $k]] != 0 } {
         lappend row [format $floatfield [lindex $xdata($i) $k]]
         lappend row [format $floatfield [lindex $ydata($i) $k]]
         lappend row [format $floatfield [lindex $dydata($i) $k]]
         lappend row [format $floatfield [lindex $resdata($i) $k]]
      } else {
         for {set c 0} {$c < 4} {incr c} { lappend row $emptyfield }
      }
   }
   puts $f [join $row ""]
}
close  $f
}

#------------------------------------------------------------------------------
# export data into CSV file, the CSV file will have 4*n columns with
# with n equal to the number of curves stored in bltGraph
# n   column = x-values of curve n
# n+1 column = y-values of curve n
# n+2 column = dy-values of curve n
# n+3 column = res-value of curve n
proc export_csv_data {outpipe bltGraph {outtype file}} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $bltGraph Graph
for {set i 0} {$i < $Graph(e,element)} {incr i} {
   set  xdata($i)  [lindex $Graph(e,xdata)   $i]
   set  ydata($i)  [lindex $Graph(e,ydata)   $i]
   set dydata($i)  [lindex $Graph(e,error)   $i]
   set resdata($i) [lindex $Graph(e,resdata) $i]
}
set max 0
for {set i 0} {$i < $Graph(e,element)} {incr i} {
  set tmax [llength $xdata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $ydata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $dydata($i)]
  if {$tmax > $max} { set max $tmax }
}
set txt ""
for {set k 0} {$k < $max} {incr k} {
   for {set i 0} {$i < $Graph(e,element)} {incr i} {
      if {[llength [lindex $xdata($i) $k]] != 0 } {
         append txt [format "%g;" [lindex $xdata($i) $k]]
         append txt [format "%g;" [lindex $ydata($i) $k]]
         set dy  [lindex $dydata($i) $k]
         if {$dy > 0} {
            append txt [format "%g;" $dy]
         } else {
            append txt [format "%g;" -1]
         }
         append txt [format "%g;" [lindex $resdata($i) $k]]
      } else { append txt ";;;;" }
   }
   append txt "\n"
}
if {[string compare $outtype file] == 0 } {
	set f [open $outpipe w]
	puts $f $txt
	close  $f
} else {
	upvar $outpipe outtxt
	set outtxt $txt
}
}

#------------------------------------------------------------------------------
# export data into CSV file, the CSV file will have 4*n columns with
# with n equal to the number of curves stored in bltGraph
# n   column = x-values of curve n
# n+1 column = y-values of curve n
# n+2 column = dy-values of curve n
# n+3 column = res-value of curve n
proc export_clipboard_data {bltGraph} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $bltGraph Graph
for {set i 0} {$i < $Graph(e,element)} {incr i} {
   set  xdata($i)  [lindex $Graph(e,xdata)   $i]
   set  ydata($i)  [lindex $Graph(e,ydata)   $i]
   set dydata($i)  [lindex $Graph(e,error)   $i]
   set resdata($i) [lindex $Graph(e,resdata) $i]
}
clipboard clear
set max 0
for {set i 0} {$i < $Graph(e,element)} {incr i} {
  set tmax [llength $xdata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $ydata($i)]
  if {$tmax > $max} { set max $tmax }
  set tmax [llength $dydata($i)]
  if {$tmax > $max} { set max $tmax }
}
for {set k 0} {$k < $max} {incr k} {
   set line ""
   for {set i 0} {$i < $Graph(e,element)} {incr i} {
      if {[llength [lindex $xdata($i) $k]] != 0 } {
         append line [format "%g\t" [lindex $xdata($i) $k]]
         append line [format "%g\t" [lindex $ydata($i) $k]]
         set dy  [lindex $dydata($i) $k]
         if {$dy > 0} {
            append line [format "%g\t" $dy]
         } else {
            append line [format "%g\t" -1]
         }
         append line [format "%g\t" [lindex $resdata($i) $k]]
      } else { append line "\t\t\t\t" }
   }
   clipboard append "$line\n"
}
}



#------------------------------------------------------------------------------

#
proc read_ScattLen {filename CSV} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $CSV Data
#puts $args

if { !([file readable $filename] && [file isfile $filename])  } {
   return 0
}

set Data(FileName)  $filename
set Data(ColumnNames) {}
set Data(A)         {}
set Data(u)         {}
set Data(Symbol)    {}
set Data(Name)      {}
set Data(Z)         {}
set Data(abundance) {}
set Data(Re_bc_coh) {}
set Data(Im_bc_coh) {}
set Data(Re_bc_inc) {}
set Data(Im_bc_inc) {}
set Data(sigma_coh) {}
set Data(sigma_inc) {}
set Data(sigma_abs) {}
set Data(CrKafp,5989eV)   {}
set Data(CrKafpp,5989eV)  {}
set Data(FeKafp,7112eV)   {}
set Data(FeKafpp,7112eV)  {}
set Data(CuKafp,8979eV)   {}
set Data(CuKafpp,8979eV)  {}
set Data(MoKafp,20000eV)  {}
set Data(MoKafpp,20000eV) {}
set Data(AgKafp,25514eV)  {}
set Data(AgKafpp,25514eV) {}

set Data(PlusIsotopes,A)         {}
set Data(PlusIsotopes,u)         {}
set Data(PlusIsotopes,Symbol)    {}
set Data(PlusIsotopes,Name)      {}
set Data(PlusIsotopes,Z)         {}
set Data(PlusIsotopes,abundance) {}
set Data(PlusIsotopes,Re_bc_coh) {}
set Data(PlusIsotopes,Im_bc_coh) {}
set Data(PlusIsotopes,Re_bc_inc) {}
set Data(PlusIsotopes,Im_bc_inc) {}
set Data(PlusIsotopes,CrKafp,5989eV)   {}
set Data(PlusIsotopes,CrKafpp,5989eV)  {}
set Data(PlusIsotopes,FeKafp,7112eV)   {}
set Data(PlusIsotopes,FeKafpp,7112eV)  {}
set Data(PlusIsotopes,CuKafp,8979eV)   {}
set Data(PlusIsotopes,CuKafpp,8979eV)  {}
set Data(PlusIsotopes,MoKafp,20000eV)  {}
set Data(PlusIsotopes,MoKafpp,20000eV) {}
set Data(PlusIsotopes,AgKafp,25514eV)  {}
set Data(PlusIsotopes,AgKafpp,25514eV) {}

set f [open $filename r]
set lineno 0

gets $f line
gets $f line
set Data(n,ColumnNames) [split $line "\t ,"]
gets $f line
 
while {![eof $f]} {
   gets $f line
   if { [eof $f] } { 
      close $f
      return 1
   }
   incr lineno
   set tmpsplitline [split $line "\;"]
   set A           [lindex $tmpsplitline 0]
   set u           [lindex $tmpsplitline 1]
   set Symbol      [lindex $tmpsplitline 2]
   set Name        [lindex $tmpsplitline 3]
   set Z           [lindex $tmpsplitline 4]
   set abundance   [lindex $tmpsplitline 5]
   set Re_bc_coh   [lindex $tmpsplitline 6]
   set Im_bc_coh   [lindex $tmpsplitline 7]
   set Re_bc_inc   [lindex $tmpsplitline 8]
   set Im_bc_inc   [lindex $tmpsplitline 9]
   set sigma_coh   [lindex $tmpsplitline 10]
   set sigma_inc   [lindex $tmpsplitline 11]
   set sigma_scatt [lindex $tmpsplitline 12]
   set sigma_abs   [lindex $tmpsplitline 13]
   set CrKafp      [lindex $tmpsplitline 16]
   set CrKafpp     [lindex $tmpsplitline 17]
   set FeKafp      [lindex $tmpsplitline 18]
   set FeKafpp     [lindex $tmpsplitline 19]
   set CuKafp      [lindex $tmpsplitline 20]
   set CuKafpp     [lindex $tmpsplitline 21]
   set MoKafp      [lindex $tmpsplitline 22]
   set MoKafpp     [lindex $tmpsplitline 23]
   set AgKafp      [lindex $tmpsplitline 24]
   set AgKafpp     [lindex $tmpsplitline 25]

   if {![catch {set diff [expr $A-$u]}]} {
       if {$diff > 0.11} {
	   puts stderr "$Symbol $A $u"
       }
   }
   if {([string compare $A n] == 0) && ([llength $Re_bc_coh] != 0)} {
       lappend Data(A)         $A
       lappend Data(u)         $u
       lappend Data(Symbol)    $Symbol
       lappend Data(Name)      $Name
       lappend Data(Z)         $Z
       lappend Data(abundance) $abundance
       lappend Data(Re_bc_coh) $Re_bc_coh
       lappend Data(Im_bc_coh) $Im_bc_coh
       lappend Data(Re_bc_inc) $Re_bc_inc
       lappend Data(Im_bc_inc) $Im_bc_inc
       lappend Data(CrKafp,5989eV)   $CrKafp
       lappend Data(CrKafpp,5989eV)  $CrKafpp
       lappend Data(FeKafp,7112eV)   $FeKafp
       lappend Data(FeKafpp,7112eV)  $FeKafpp
       lappend Data(CuKafp,8979eV)   $CuKafp
       lappend Data(CuKafpp,8979eV)  $CuKafpp
       lappend Data(MoKafp,20000eV)  $MoKafp
       lappend Data(MoKafpp,20000eV) $MoKafpp
       lappend Data(AgKafp,25514eV)  $AgKafp
       lappend Data(AgKafpp,25514eV) $AgKafpp
   }  elseif {[llength $Re_bc_coh] != 0} {
       lappend Data(A)         $A
       lappend Data(u)         $u
       lappend Data(Symbol)    $Symbol
       lappend Data(Name)      $Name
       lappend Data(Z)         $Z
       lappend Data(abundance) $abundance
       lappend Data(Re_bc_coh) $Re_bc_coh
       lappend Data(Im_bc_coh) $Im_bc_coh
       lappend Data(Re_bc_inc) $Re_bc_inc
       lappend Data(Im_bc_inc) $Im_bc_inc
       lappend Data(CrKafp,5989eV)   $CrKafp
       lappend Data(CrKafpp,5989eV)  $CrKafpp
       lappend Data(FeKafp,7112eV)   $FeKafp
       lappend Data(FeKafpp,7112eV)  $FeKafpp
       lappend Data(CuKafp,8979eV)   $CuKafp
       lappend Data(CuKafpp,8979eV)  $CuKafpp
       lappend Data(MoKafp,20000eV)  $MoKafp
       lappend Data(MoKafpp,20000eV) $MoKafpp
       lappend Data(AgKafp,25514eV)  $AgKafp
       lappend Data(AgKafpp,25514eV) $AgKafpp
       lappend Data(PlusIsotopes,A)         $A
       lappend Data(PlusIsotopes,u)         $u
       lappend Data(PlusIsotopes,Symbol)    $Symbol
       lappend Data(PlusIsotopes,Name)      $Name
       lappend Data(PlusIsotopes,Z)         $Z
       lappend Data(PlusIsotopes,abundance) $abundance
       lappend Data(PlusIsotopes,Re_bc_coh) $Re_bc_coh
       lappend Data(PlusIsotopes,Im_bc_coh) $Im_bc_coh
       lappend Data(PlusIsotopes,Re_bc_inc) $Re_bc_inc
       lappend Data(PlusIsotopes,Im_bc_inc) $Im_bc_inc
   }
}
}

