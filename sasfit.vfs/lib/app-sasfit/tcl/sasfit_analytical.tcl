# sasfit.vfs/lib/app-sasfit/tcl/sasfit_analytical.tcl
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

proc RefreshAPEntryConfigure {} {
	set alambda $::AnalytPar(alambda)
	set tmp $::actualAnalytPar(actual_SD)
	set ::actualAnalytPar(actual_SD) $::actualAnalytPar(old_actual_SD)
	save_AP ::tmpAnalytPar ::actualAnalytPar
	set ::actualAnalytPar(actual_SD) $tmp
	APindex ::tmpAnalytPar ::actualAnalytPar $::actualAnalytPar(actual_SD)
	cp_arr ::actualAnalytPar ::tmpactualAnalytPar
	set_limitsCmd
	set ::tmpAnalytPar(alambda) $alambda
	set ::AnalytPar(alambda) $alambda
	set ::actualAnalytPar(old_actual_SD) $::actualAnalytPar(actual_SD)
	select_current_functions ::actualAnalytPar
}

#------------------------------------------------------------------------------
# create the array holding values of fitparameter for the fit of a scattering
# curve with a given analytical size distribution and a given analytical form
# factor. 
proc create_AnalytPar {AnalytPar} {

	upvar $AnalytPar Par
	set Par(w) ""
	set Par(file) unknown.par
	set Par(w_AnalytPar) ""
	set Par(resolution) no
	set Par(geometrical/datafile) yes
	set Par(w_MomentsAnalytPar) ""
	set Par(GraphName)  AnalytPar
	
	init_analytpar Par 0; # not global
	add_analytpar Par 0; # not global
	
	set Par(actual_SD) 1     ;# No of size distribution of which its values 
	                         ;# are displayed
	set Par(old_actual_SD) 1 ;# 
	
}

#------------------------------------------------------------------------------
#
proc put_AP {analytPar} {

	upvar $analytPar Par
	
	add_analytpar Par 0; # not global
}

#------------------------------------------------------------------------------
# delete "index"-th size distribution out of the list actualAnalytPar
# possible values of index are 1 to max_SD
#
proc del_AP {AnalytPar index} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $AnalytPar Par

set FFl1       [lrange $Par(FF,l)       0      [expr $index - 2]       ]
set FFl2       [lrange $Par(FF,l)       $index [expr $Par(max_SD) - 1] ]
set FFerr1     [lrange $Par(FF,err)     0      [expr $index - 2]       ]
set FFerr2     [lrange $Par(FF,err)     $index [expr $Par(max_SD) - 1] ]
set FFmin1     [lrange $Par(FF,min)     0      [expr $index - 2]       ]
set FFmin2     [lrange $Par(FF,min)     $index [expr $Par(max_SD) - 1] ]
set FFmax1     [lrange $Par(FF,max)     0      [expr $index - 2]       ]
set FFmax2     [lrange $Par(FF,max)     $index [expr $Par(max_SD) - 1] ]
set FFlimits1  [lrange $Par(FF,limits)  0      [expr $index - 2]       ]
set FFlimits2  [lrange $Par(FF,limits)  $index [expr $Par(max_SD) - 1] ]
set FFactive1  [lrange $Par(FF,active)  0      [expr $index - 2]       ]
set FFactive2  [lrange $Par(FF,active)  $index [expr $Par(max_SD) - 1] ]
set FFdistr1   [lrange $Par(FF,distr)   0      [expr $index - 2]       ]
set FFdistr2   [lrange $Par(FF,distr)   $index [expr $Par(max_SD) - 1] ]
set FFtypestr1 [lrange $Par(FF,typestr) 0      [expr $index - 2]       ]
set FFtypestr2 [lrange $Par(FF,typestr) $index [expr $Par(max_SD) - 1] ]

set SDa1       [lrange $Par(SD,a)       0      [expr $index - 2]       ]
set SDa2       [lrange $Par(SD,a)       $index [expr $Par(max_SD) - 1] ]
set SDerr1     [lrange $Par(SD,err)     0      [expr $index - 2]       ]
set SDerr2     [lrange $Par(SD,err)     $index [expr $Par(max_SD) - 1] ]
set SDmin1     [lrange $Par(SD,min)     0      [expr $index - 2]       ]
set SDmin2     [lrange $Par(SD,min)     $index [expr $Par(max_SD) - 1] ]
set SDmax1     [lrange $Par(SD,max)     0      [expr $index - 2]       ]
set SDmax2     [lrange $Par(SD,max)     $index [expr $Par(max_SD) - 1] ]
set SDlimits1  [lrange $Par(SD,limits)  0      [expr $index - 2]       ]
set SDlimits2  [lrange $Par(SD,limits)  $index [expr $Par(max_SD) - 1] ]
set SDactive1  [lrange $Par(SD,active)  0      [expr $index - 2]       ]
set SDactive2  [lrange $Par(SD,active)  $index [expr $Par(max_SD) - 1] ]
set SDtypestr1 [lrange $Par(SD,typestr) 0      [expr $index - 2]       ]
set SDtypestr2 [lrange $Par(SD,typestr) $index [expr $Par(max_SD) - 1] ]

set SQs1       [lrange $Par(SQ,s)       0      [expr $index - 2]       ]
set SQs2       [lrange $Par(SQ,s)       $index [expr $Par(max_SD) - 1] ]
set SQerr1     [lrange $Par(SQ,err)     0      [expr $index - 2]       ]
set SQerr2     [lrange $Par(SQ,err)     $index [expr $Par(max_SD) - 1] ]
set SQmin1     [lrange $Par(SQ,min)     0      [expr $index - 2]       ]
set SQmin2     [lrange $Par(SQ,min)     $index [expr $Par(max_SD) - 1] ]
set SQmax1     [lrange $Par(SQ,max)     0      [expr $index - 2]       ]
set SQmax2     [lrange $Par(SQ,max)     $index [expr $Par(max_SD) - 1] ]
set SQlimits1  [lrange $Par(SQ,limits)  0      [expr $index - 2]       ]
set SQlimits2  [lrange $Par(SQ,limits)  $index [expr $Par(max_SD) - 1] ]
set SQactive1  [lrange $Par(SQ,active)  0      [expr $index - 2]       ]
set SQactive2  [lrange $Par(SQ,active)  $index [expr $Par(max_SD) - 1] ]
set SQtypestr1 [lrange $Par(SQ,typestr) 0      [expr $index - 2]       ]
set SQtypestr2 [lrange $Par(SQ,typestr) $index [expr $Par(max_SD) - 1] ]
set SQhow1     [lrange $Par(SQ,how)     0      [expr $index - 2]       ]
set SQhow2     [lrange $Par(SQ,how)     $index [expr $Par(max_SD) - 1] ]

set R0_1       [lrange $Par(<R^0>)      0      [expr $index - 2]       ]
set R0_2       [lrange $Par(<R^0>)      $index [expr $Par(max_SD) - 1] ]
set R1_1       [lrange $Par(<R^1>)      0      [expr $index - 2]       ]
set R1_2       [lrange $Par(<R^1>)      $index [expr $Par(max_SD) - 1] ]
set R2_1       [lrange $Par(<R^2>)      0      [expr $index - 2]       ]
set R2_2       [lrange $Par(<R^2>)      $index [expr $Par(max_SD) - 1] ]
set R3_1       [lrange $Par(<R^3>)      0      [expr $index - 2]       ]
set R3_2       [lrange $Par(<R^3>)      $index [expr $Par(max_SD) - 1] ]
set R4_1       [lrange $Par(<R^4>)      0      [expr $index - 2]       ]
set R4_2       [lrange $Par(<R^4>)      $index [expr $Par(max_SD) - 1] ]
set R5_1       [lrange $Par(<R^5>)      0      [expr $index - 2]       ]
set R5_2       [lrange $Par(<R^5>)      $index [expr $Par(max_SD) - 1] ]
set R6_1       [lrange $Par(<R^6>)      0      [expr $index - 2]       ]
set R6_2       [lrange $Par(<R^6>)      $index [expr $Par(max_SD) - 1] ]
set R7_1       [lrange $Par(<R^7>)      0      [expr $index - 2]       ]
set R7_2       [lrange $Par(<R^7>)      $index [expr $Par(max_SD) - 1] ]
set R8_1       [lrange $Par(<R^8>)      0      [expr $index - 2]       ]
set R8_2       [lrange $Par(<R^8>)      $index [expr $Par(max_SD) - 1] ]
set R_li_1     [lrange $Par(R_li)       0      [expr $index - 2]       ]
set R_li_2     [lrange $Par(R_li)       $index [expr $Par(max_SD) - 1] ]
set R_lc_1     [lrange $Par(R_lc)       0      [expr $index - 2]       ]
set R_lc_2     [lrange $Par(R_lc)       $index [expr $Par(max_SD) - 1] ]
set R_Ac_1     [lrange $Par(R_Ac)       0      [expr $index - 2]       ]
set R_Ac_2     [lrange $Par(R_Ac)       $index [expr $Par(max_SD) - 1] ]
set R_VP_1     [lrange $Par(R_VP)       0      [expr $index - 2]       ]
set R_VP_2     [lrange $Par(R_VP)       $index [expr $Par(max_SD) - 1] ]
set R_RG_1     [lrange $Par(R_RG)       0      [expr $index - 2]       ]
set R_RG_2     [lrange $Par(R_RG)       $index [expr $Par(max_SD) - 1] ]
set fp_1       [lrange $Par(fp)         0      [expr $index - 2]       ]
set fp_2       [lrange $Par(fp)         $index [expr $Par(max_SD) - 1] ]

set fitSDFF1    [lrange $Par(fitSDFF)    0      [expr $index - 2]       ]
set fitSDFF2    [lrange $Par(fitSDFF)    $index [expr $Par(max_SD) - 1] ]
set calcSDFF1   [lrange $Par(calcSDFF)   0      [expr $index - 2]       ]
set calcSDFF2   [lrange $Par(calcSDFF)   $index [expr $Par(max_SD) - 1] ]
set substrSDFF1 [lrange $Par(substrSDFF)   0      [expr $index - 2]       ]
set substrSDFF2 [lrange $Par(substrSDFF)   $index [expr $Par(max_SD) - 1] ]

if {$index == 1} {
   set Par(FF,l)       $FFl2
   set Par(FF,err)     $FFerr2
   set Par(FF,min)     $FFmin2
   set Par(FF,max)     $FFmax2
   set Par(FF,limits)  $FFlimits2
   set Par(FF,active)  $FFactive2
   set Par(FF,distr)   $FFdistr2
   set Par(FF,typestr) $FFtypestr2

   set Par(<R^0>)      $R0_2
   set Par(<R^1>)      $R1_2
   set Par(<R^2>)      $R2_2
   set Par(<R^3>)      $R3_2
   set Par(<R^4>)      $R4_2
   set Par(<R^5>)      $R5_2
   set Par(<R^6>)      $R6_2
   set Par(<R^7>)      $R7_2
   set Par(<R^8>)      $R8_2
   set Par(R_li)       $R_li_2
   set Par(R_lc)       $R_lc_2
   set Par(R_Ac)       $R_Ac_2
   set Par(R_VP)       $R_VP_2
   set Par(R_RG)       $R_RG_2
   set Par(fp)         $fp_2

   set Par(SD,a)       $SDa2
   set Par(SD,err)     $SDerr2
   set Par(SD,min)     $SDmin2
   set Par(SD,max)     $SDmax2
   set Par(SD,limits)  $SDlimits2
   set Par(SD,active)  $SDactive2
   set Par(SD,typestr) $SDtypestr2

   set Par(SQ,s)       $SQs2
   set Par(SQ,err)     $SQerr2
   set Par(SQ,min)     $SQmin2
   set Par(SQ,max)     $SQmax2
   set Par(SQ,limits)  $SQlimits2
   set Par(SQ,active)  $SQactive2
   set Par(SQ,typestr) $SQtypestr2
   set Par(SQ,how)     $SQhow2

   set Par(fitSDFF)    $fitSDFF2
   set Par(calcSDFF)   $calcSDFF2
   set Par(substrSDFF) $substrSDFF2
} elseif {$index == $Par(max_SD)} {
   set Par(FF,l)       $FFl1       
   set Par(FF,err)     $FFerr1     
   set Par(FF,min)     $FFmin1     
   set Par(FF,max)     $FFmax1     
   set Par(FF,limits)  $FFlimits1
   set Par(FF,active)  $FFactive1  
   set Par(FF,distr)   $FFdistr1   
   set Par(FF,typestr) $FFtypestr1 

   set Par(SD,a)       $SDa1       
   set Par(SD,err)     $SDerr1     
   set Par(SD,min)     $SDmin1     
   set Par(SD,max)     $SDmax1     
   set Par(SD,limits)  $SDlimits1
   set Par(SD,active)  $SDactive1  
   set Par(SD,typestr) $SDtypestr1

   set Par(SQ,s)       $SQs1       
   set Par(SQ,err)     $SQerr1     
   set Par(SQ,min)     $SQmin1     
   set Par(SQ,max)     $SQmax1     
   set Par(SQ,limits)  $SQlimits1
   set Par(SQ,active)  $SQactive1  
   set Par(SQ,typestr) $SQtypestr1
   set Par(SQ,how)     $SQhow1

   set Par(<R^0>)      $R0_1
   set Par(<R^1>)      $R1_1
   set Par(<R^2>)      $R2_1
   set Par(<R^3>)      $R3_1
   set Par(<R^4>)      $R4_1
   set Par(<R^5>)      $R5_1
   set Par(<R^6>)      $R6_1
   set Par(<R^7>)      $R7_1
   set Par(<R^8>)      $R8_1
   set Par(R_li)       $R_li_1
   set Par(R_lc)       $R_lc_1
   set Par(R_Ac)       $R_Ac_1
   set Par(R_VP)       $R_VP_1
   set Par(R_RG)       $R_RG_1
   set Par(fp)         $fp_1

   set Par(fitSDFF)    $fitSDFF1
   set Par(calcSDFF)   $calcSDFF1 
   set Par(substrSDFF) $substrSDFF1 
} else {
   set Par(FF,l)       [concat $FFl1       $FFl2       ]
   set Par(FF,err)     [concat $FFerr1     $FFerr2     ]
   set Par(FF,min)     [concat $FFmin1     $FFmin2     ]
   set Par(FF,max)     [concat $FFmax1     $FFmax2     ]
   set Par(FF,limits)  [concat $FFlimits1  $FFlimits2  ]
   set Par(FF,active)  [concat $FFactive1  $FFactive2  ]
   set Par(FF,distr)   [concat $FFdistr1   $FFdistr2   ]
   set Par(FF,typestr) [concat $FFtypestr1 $FFtypestr2 ]

   set Par(SD,a)       [concat $SDa1       $SDa2       ]
   set Par(SD,err)     [concat $SDerr1     $SDerr2     ]
   set Par(SD,min)     [concat $SDmin1     $SDmin2     ]
   set Par(SD,max)     [concat $SDmax1     $SDmax2     ]
   set Par(SD,limits)  [concat $SDlimits1  $SDlimits2  ]
   set Par(SD,active)  [concat $SDactive1  $SDactive2  ]
   set Par(SD,typestr) [concat $SDtypestr1 $SDtypestr2 ]

   set Par(SQ,s)       [concat $SQs1       $SQs2       ]
   set Par(SQ,err)     [concat $SQerr1     $SQerr2     ]
   set Par(SQ,min)     [concat $SQmin1     $SQmin2     ]
   set Par(SQ,max)     [concat $SQmax1     $SQmax2     ]
   set Par(SQ,limits)  [concat $SQlimits1  $SQlimits2  ]
   set Par(SQ,active)  [concat $SQactive1  $SQactive2  ]
   set Par(SQ,typestr) [concat $SQtypestr1 $SQtypestr2 ]
   set Par(SQ,how)     [concat $SQhow1     $SQhow2     ]

   set Par(<R^0>)      [concat $R0_1    $R0_2  ]
   set Par(<R^1>)      [concat $R1_1    $R1_2  ]
   set Par(<R^2>)      [concat $R2_1    $R2_2  ]
   set Par(<R^3>)      [concat $R3_1    $R3_2  ]
   set Par(<R^4>)      [concat $R4_1    $R4_2  ]
   set Par(<R^5>)      [concat $R5_1    $R5_2  ]
   set Par(<R^6>)      [concat $R6_1    $R6_2  ]
   set Par(<R^7>)      [concat $R7_1    $R7_2  ] 
   set Par(<R^8>)      [concat $R8_1    $R8_2  ]
   set Par(R_li)       [concat $R_li_1  $R_li_2]
   set Par(R_lc)       [concat $R_lc_1  $R_lc_2]
   set Par(R_Ac)       [concat $R_Ac_1  $R_Ac_2]
   set Par(R_VP)       [concat $R_VP_1  $R_VP_2]
   set Par(R_RG)       [concat $R_RG_1  $R_RG_2]
   set Par(fp)         [concat $fp_1    $fp_2  ]

   set Par(fitSDFF)    [concat $fitSDFF1    $fitSDFF2  ]
   set Par(calcSDFF)   [concat $calcSDFF1   $calcSDFF2 ]
   set Par(substrSDFF) [concat $substrSDFF1 $substrSDFF2 ]
}
if {$Par(actual_SD) == $Par(max_SD)} {
   incr Par(actual_SD) -1
} else {set Par(actual_SD) $index}
incr Par(max_SD) -1
}

#------------------------------------------------------------------------------
# save the actual values of the entries (stored in actualAnalytPar) into
# the array AnalytPar as the "$index-1"-th list element
#
proc save_AP {AnalytPar actualAnalytPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $AnalytPar Par
upvar $actualAnalytPar actualPar

set Par(w) $actualPar(w)
set Par(w_AnalytPar) $actualPar(w_AnalytPar)
set Par(w_MomentsAnalytPar) $actualPar(w_MomentsAnalytPar)

set Par(actual_SD) $actualPar(actual_SD)
set ti [expr $Par(actual_SD) - 1]

update_analytpar Par actualPar $ti

set Par(<R^0>)      [lreplace $Par(<R^0>)      $ti $ti $actualPar(<R^0>)]
set Par(<R^1>)      [lreplace $Par(<R^1>)      $ti $ti $actualPar(<R^1>)]
set Par(<R^2>)      [lreplace $Par(<R^2>)      $ti $ti $actualPar(<R^2>)]
set Par(<R^3>)      [lreplace $Par(<R^3>)      $ti $ti $actualPar(<R^3>)]
set Par(<R^4>)      [lreplace $Par(<R^4>)      $ti $ti $actualPar(<R^4>)]
set Par(<R^5>)      [lreplace $Par(<R^5>)      $ti $ti $actualPar(<R^5>)]
set Par(<R^6>)      [lreplace $Par(<R^6>)      $ti $ti $actualPar(<R^6>)]
set Par(<R^7>)      [lreplace $Par(<R^7>)      $ti $ti $actualPar(<R^7>)]
set Par(<R^8>)      [lreplace $Par(<R^8>)      $ti $ti $actualPar(<R^8>)]
set Par(R_li)       [lreplace $Par(R_li)       $ti $ti $actualPar(R_li) ]
set Par(R_lc)       [lreplace $Par(R_lc)       $ti $ti $actualPar(R_lc) ]
set Par(R_Ac)       [lreplace $Par(R_Ac)       $ti $ti $actualPar(R_Ac) ]
set Par(R_VP)       [lreplace $Par(R_VP)       $ti $ti $actualPar(R_VP) ]
set Par(R_RG)       [lreplace $Par(R_RG)       $ti $ti $actualPar(R_RG) ]
set Par(fp)         [lreplace $Par(fp)         $ti $ti $actualPar(fp)   ]

set Par(<sumR^0>)   $actualPar(<sumR^0>)
set Par(<sumR^1>)   $actualPar(<sumR^1>)
set Par(<sumR^2>)   $actualPar(<sumR^2>)
set Par(<sumR^3>)   $actualPar(<sumR^3>)
set Par(<sumR^4>)   $actualPar(<sumR^4>)
set Par(<sumR^5>)   $actualPar(<sumR^5>)
set Par(<sumR^6>)   $actualPar(<sumR^6>)
set Par(<sumR^7>)   $actualPar(<sumR^7>)
set Par(<sumR^8>)   $actualPar(<sumR^8>)
set Par(sumR_li)    $actualPar(sumR_li)
set Par(sumR_lc)    $actualPar(sumR_lc)
set Par(sumR_Ac)    $actualPar(sumR_Ac)
set Par(sumR_VP)    $actualPar(sumR_VP)
set Par(sumR_RG)    $actualPar(sumR_RG)
set Par(sumfp)      $actualPar(sumfp)

# is redundant (?)
#set Par(FF,typestr) [lreplace $Par(FF,typestr) $ti $ti $actualPar(FF,typestr)]
set Par(fitSDFF)    [lreplace $Par(fitSDFF)    $ti $ti $actualPar(fitSDFF)]
set Par(calcSDFF)   [lreplace $Par(calcSDFF)   $ti $ti $actualPar(calcSDFF)]
set Par(substrSDFF) [lreplace $Par(substrSDFF) $ti $ti $actualPar(substrSDFF)]
set Par(alambda) $actualPar(alambda)
set Par(error) $actualPar(error)

#	print_analytpar Par
}

#------------------------------------------------------------------------------
# extract out of the array list AnalytPar of parameters for fitting the 
# scattering intensity the $index-th element ($index-th size distribution) 
# and store them in the  array actualAnalytPar. Function return value is
# 1 for successfull extraction and 0 if it fails. "index" starts from 1.
#
proc APindex {AnalytPar actualAnalytPar index} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $AnalytPar Par
upvar $actualAnalytPar actualPar

#puts "### APindex"

set actualPar(w) $Par(w)
set actualPar(w_AnalytPar) $Par(w_AnalytPar)
set actualPar(w_MomentsAnalytPar) $Par(w_MomentsAnalytPar)

set ti [expr $index-1]

if {($index < 1) && ($index > $Par(max_SD))} { return 0 }

set actualPar(actual_SD) $index
set Par(actual_SD) $index
set Par(old_actual_SD)    $index
set actualPar(old_actual_SD) $Par(old_actual_SD)

set actualPar(max_SD) $Par(max_SD)

set actualPar(<sumR^0>)   $Par(<sumR^0>)
set actualPar(<sumR^1>)   $Par(<sumR^1>)
set actualPar(<sumR^2>)   $Par(<sumR^2>)
set actualPar(<sumR^3>)   $Par(<sumR^3>)
set actualPar(<sumR^4>)   $Par(<sumR^4>)
set actualPar(<sumR^5>)   $Par(<sumR^5>)
set actualPar(<sumR^6>)   $Par(<sumR^6>)
set actualPar(<sumR^7>)   $Par(<sumR^7>)
set actualPar(<sumR^8>)   $Par(<sumR^8>)
set actualPar(sumR_li)    $Par(sumR_li)
set actualPar(sumR_lc)    $Par(sumR_lc)
set actualPar(sumR_Ac)    $Par(sumR_Ac)
set actualPar(sumR_VP)    $Par(sumR_VP)
set actualPar(sumR_RG)    $Par(sumR_RG)
set actualPar(sumfp)      $Par(sumfp)

set actualPar(<R^0>)      [lindex $Par(<R^0>) $ti]
set actualPar(<R^1>)      [lindex $Par(<R^1>) $ti]
set actualPar(<R^2>)      [lindex $Par(<R^2>) $ti]
set actualPar(<R^3>)      [lindex $Par(<R^3>) $ti]
set actualPar(<R^4>)      [lindex $Par(<R^4>) $ti]
set actualPar(<R^5>)      [lindex $Par(<R^5>) $ti]
set actualPar(<R^6>)      [lindex $Par(<R^6>) $ti]
set actualPar(<R^7>)      [lindex $Par(<R^7>) $ti]
set actualPar(<R^8>)      [lindex $Par(<R^8>) $ti]
set actualPar(R_li)       [lindex $Par(R_li)  $ti]
set actualPar(R_lc)       [lindex $Par(R_lc)  $ti]
set actualPar(R_Ac)       [lindex $Par(R_Ac)  $ti]
set actualPar(R_VP)       [lindex $Par(R_VP)  $ti]
set actualPar(R_RG)       [lindex $Par(R_RG)  $ti]
set actualPar(fp)         [lindex $Par(fp)    $ti]

set actualPar(SQ,how)     [lindex  $Par(SQ,how)     $ti]
set actualPar(fitSDFF)    [lindex  $Par(fitSDFF)    $ti]
set actualPar(calcSDFF)   [lindex  $Par(calcSDFF)   $ti]
set actualPar(substrSDFF) [lindex  $Par(substrSDFF) $ti]

# set the current name to the typestr for old model functions,
# for plugins this is done in menu_command
foreach type {FF SD SQ} {
	set actualPar($type,typestr) [lindex  $Par($type,typestr) $ti]
}

array unset actualPar "SD,a*,label"
array unset actualPar "SQ,s*,label"
array unset actualPar "FF,l*,label"

switch $actualPar(FF,typestr) {
   "Background"         { set actualPar(FF,l1,label) "c_0 ="
                          set actualPar(FF,l2,label) "c_1 ="
                          set actualPar(FF,l3,label) "c_4 ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "p(r) -> 4 pi sin(qr)/(qr)" \
                        { set actualPar(FF,l1,label) "r ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "gamma(r) -> 4 pi r^2 sin(qr)/(qr)" \
                        { set actualPar(FF,l1,label) "r ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Guinier"            { set actualPar(FF,l1,label) "I0 ="
                          set actualPar(FF,l2,label) "a ="
                          set actualPar(FF,l3,label) "Ra ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Sphere"             { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "HardSphere"         { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "RHS ="
                          set actualPar(FF,l3,label) "fp ="
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "StickyHardSphere"   { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "RHS ="
                          set actualPar(FF,l3,label) "tau ="
                          set actualPar(FF,l4,label) "fp ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Spherical Shell i"  { set actualPar(FF,l1,label) "R1 ="
                          set actualPar(FF,l2,label) "R2 ="
                          set actualPar(FF,l3,label) "mu ="
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Spherical Shell ii" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "nu ="
                          set actualPar(FF,l3,label) "mu ="
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
  "Spherical Shell iii" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "eta1 ="
                          set actualPar(FF,l4,label) "eta2 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
 "MultiLamellarVesicle" { set actualPar(FF,l1,label) "R_c ="
                          set actualPar(FF,l2,label) "t_sh ="
                          set actualPar(FF,l3,label) "t_sol ="
                          set actualPar(FF,l4,label) "eta_sh ="
                          set actualPar(FF,l5,label) "eta_sol ="
                          set actualPar(FF,l6,label) "n ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
 "RNDMultiLamellarVesicle" { 
                          set actualPar(FF,l1,label) "R_c ="
                          set actualPar(FF,l2,label) "t_sh ="
                          set actualPar(FF,l3,label) "t_sol ="
                          set actualPar(FF,l4,label) "Dt_sol ="
                          set actualPar(FF,l5,label) "eta_sh ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "n ="
                          set actualPar(FF,l8,label) "sigma_Rc ="
                          set actualPar(FF,l9,label) "sigma_tsh ="
                          set actualPar(FF,l10,label) ""
                        }
 "RNDMultiLamellarVesicle2" { 
                          set actualPar(FF,l1,label) "t_sh ="
                          set actualPar(FF,l2,label) "sigma_tsh ="
                          set actualPar(FF,l3,label) "Rc ="
                          set actualPar(FF,l4,label) "sigma_Rc ="
                          set actualPar(FF,l5,label) "n ="
                          set actualPar(FF,l6,label) "s_n ="
                          set actualPar(FF,l7,label) "tsol ="
                          set actualPar(FF,l8,label) "sigma_tsol"
                          set actualPar(FF,l9,label) "Dtsol ="
                          set actualPar(FF,l10,label) "Delta_eta ="
                        }
 "MLVesicleFrielinghaus" { \
                          set actualPar(FF,l1,label) "n ="
                          set actualPar(FF,l2,label) "nw ="
                          set actualPar(FF,l3,label) "rho_sol ="
                          set actualPar(FF,l4,label) "rho_core ="
                          set actualPar(FF,l5,label) "rho_shell ="
                          set actualPar(FF,l6,label) "sig_core ="
                          set actualPar(FF,l7,label) "sig_shell ="
                          set actualPar(FF,l8,label) "Rmain ="
                          set actualPar(FF,l9,label) "zz ="
                          set actualPar(FF,l10,label) "Rshift ="
                        }
 "BiLayeredVesicle"     { set actualPar(FF,l1,label) "R_c ="
                          set actualPar(FF,l2,label) "t_h ="
                          set actualPar(FF,l3,label) "t_t ="
                          set actualPar(FF,l4,label) "eta_sol ="
                          set actualPar(FF,l5,label) "eta_h ="
                          set actualPar(FF,l6,label) "eta_t"
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
  "MagneticShellPsi"    { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "mag_sh ="
                          set actualPar(FF,l8,label) "mag_m ="
                          set actualPar(FF,l9,label) "psi ="
                          set actualPar(FF,l10,label) "pol ="
                        }
  "MagneticShellAniso"  { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "mag_sh ="
                          set actualPar(FF,l8,label) "mag_m ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) "pol ="
                        }
"MagneticShellCrossTerm" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "mag_sh ="
                          set actualPar(FF,l8,label) "mag_m ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) "pol ="
                        }
"SuperparamagneticFFpsi" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "R0 ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "psi ="
                          set actualPar(FF,l10,label) "pol ="
                        }
"SuperparamagneticFFCrossTerm" \
                        { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "R0 ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "psi ="
                          set actualPar(FF,l10,label) "pol ="
                        }
"SuperparamagneticFFAniso" \
                        { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "R0 ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) "pol ="
                        }
"SuperparamagneticFFIso" \
                        { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "nuc_c ="
                          set actualPar(FF,l4,label) "nuc_sh ="
                          set actualPar(FF,l5,label) "nuc_m ="
                          set actualPar(FF,l6,label) "mag_c ="
                          set actualPar(FF,l7,label) "R0 ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) "pol ="
                        }
  "LinShell"            { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "eta_c ="
                          set actualPar(FF,l4,label) "eta_sh =" 
                          set actualPar(FF,l5,label) "x_in ="
                          set actualPar(FF,l6,label) "x_out ="
                          set actualPar(FF,l7,label) "eta_s ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }  
  "LinShell2"           { set actualPar(FF,l1,label) "Rtot ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "eta_c ="
                          set actualPar(FF,l4,label) "eta_sh =" 
                          set actualPar(FF,l5,label) "x_in ="
                          set actualPar(FF,l6,label) "x_out ="
                          set actualPar(FF,l7,label) "eta_s ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }   
  "SphereShell_compr"   { set actualPar(FF,l1,label) "Rtot ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "eta_c ="
                          set actualPar(FF,l4,label) "eta_sh =" 
                          set actualPar(FF,l5,label) "x_sol ="
                          set actualPar(FF,l6,label) "eta_s ="
                          set actualPar(FF,l7,label) "kappa_c ="
                          set actualPar(FF,l8,label) "kappa_sh ="
                          set actualPar(FF,l9,label) "kappa_s ="
                          set actualPar(FF,l10,label) "P ="
                        }
  "ExpShell"            { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "eta_core ="
                          set actualPar(FF,l4,label) "eta_shell =" 
                          set actualPar(FF,l5,label) "x_in_solv ="
                          set actualPar(FF,l6,label) "x_out_solv ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "eta_solvent ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }   
  "DumbbellShell"       { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_c =" 
                          set actualPar(FF,l5,label) "eta_sh ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }   
"two_attached_spheres"  { set actualPar(FF,l1,label) "R1 ="
                          set actualPar(FF,l2,label) "R2 ="
                          set actualPar(FF,l3,label) "eta0 ="
                          set actualPar(FF,l4,label) "eta1 =" 
                          set actualPar(FF,l5,label) "eta2 ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }    
  "DoubleShellChain"    { set actualPar(FF,l1,label) "R_c ="
                          set actualPar(FF,l2,label) "DR1 ="
                          set actualPar(FF,l3,label) "DR2 ="
                          set actualPar(FF,l4,label) "L =" 
                          set actualPar(FF,l5,label) "eta_c ="
                          set actualPar(FF,l6,label) "eta_1 ="
                          set actualPar(FF,l7,label) "eta_2 ="
                          set actualPar(FF,l8,label) "eta_sol ="
                          set actualPar(FF,l9,label) "x_sol_sh2 ="
                          set actualPar(FF,l10,label) "n ="
                        }      
"TetrahedronDoubleShell" {set actualPar(FF,l1,label) "R_c ="
                          set actualPar(FF,l2,label) "DR1 ="
                          set actualPar(FF,l3,label) "DR2 ="
                          set actualPar(FF,l4,label) "L =" 
                          set actualPar(FF,l5,label) "eta_c ="
                          set actualPar(FF,l6,label) "eta_1 ="
                          set actualPar(FF,l7,label) "eta_2 ="
                          set actualPar(FF,l8,label) "eta_sol ="
                          set actualPar(FF,l9,label) "x_sol_sh2 ="
                          set actualPar(FF,l10,label) "n ="
                        }      
 "EllipsoidalCoreShell" { set actualPar(FF,l1,label) "a ="
                          set actualPar(FF,l2,label) "b ="
                          set actualPar(FF,l3,label) "t ="
                          set actualPar(FF,l4,label) "eta_c ="
                          set actualPar(FF,l5,label) "eta_sh ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }      
 "triaxEllShell1"       { set actualPar(FF,l1,label) "a ="
                          set actualPar(FF,l2,label) "b ="
                          set actualPar(FF,l3,label) "c ="
                          set actualPar(FF,l4,label) "t ="
                          set actualPar(FF,l5,label) "eta_c ="
                          set actualPar(FF,l6,label) "eta_sh ="
                          set actualPar(FF,l7,label) "eta_sol ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Ellipsoid i"        { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "V ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Ellipsoid ii"       { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "nu ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta"
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Cylinder"           { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "EllCylShell"  \
                        { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "epsilon ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "t ="
                          set actualPar(FF,l5,label) "eta_core =" 
                          set actualPar(FF,l6,label) "eta_shell ="
                          set actualPar(FF,l7,label) "eta_sol ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "FlatCylinder"       { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta"
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LongCylinder"       { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta"
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "PorodCylinder"      { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta"
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"LongCylShell"          { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"ellCylShell1"          { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "epsilon ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "t ="
                          set actualPar(FF,l5,label) "eta_core ="
                          set actualPar(FF,l6,label) "eta_shell ="
                          set actualPar(FF,l7,label) "eta_sol ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"ellCylShell2"          { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "epsilon ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "t ="
                          set actualPar(FF,l5,label) "eta_core ="
                          set actualPar(FF,l6,label) "eta_shell ="
                          set actualPar(FF,l7,label) "eta_sol ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"Pcs:homogeneousCyl"    { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "eta_core ="
                          set actualPar(FF,l3,label) "eta_solv ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"Pcs:ellCylSh"          { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "epsilon ="
                          set actualPar(FF,l3,label) "t ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"CylShell1"             { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"CylShell2"             { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"alignedCylShell"       { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "psi ="
                          set actualPar(FF,l8,label) "theta ="
                          set actualPar(FF,l9,label) "phi ="
                          set actualPar(FF,l10,label) ""
                        }
"partly aligned CylShell" \
	                { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "DR ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_shell ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "psi ="
                          set actualPar(FF,l8,label) "P2 ="
                          set actualPar(FF,l9,label) "P4 ="
                          set actualPar(FF,l10,label) "P6 ="
                        }
   "Torus"              { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "a ="
                          set actualPar(FF,l3,label) "Delta_a ="
                          set actualPar(FF,l4,label) "nu ="
                          set actualPar(FF,l5,label) "eta_c ="
                          set actualPar(FF,l6,label) "eta_sh ="
                          set actualPar(FF,l7,label) "eta_sol ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Disc"               { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "eta ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "TripleLayeredXS_ULV" \
                        { set actualPar(FF,l1,label) "nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_middle ="
                          set actualPar(FF,l4,label) "V_out ="
                          set actualPar(FF,l5,label) "eta_core ="
                          set actualPar(FF,l6,label) "eta_middle ="
                          set actualPar(FF,l7,label) "eta_out ="
                          set actualPar(FF,l8,label) "eta_solv ="
                          set actualPar(FF,l9,label) "xsolv_middle ="
                          set actualPar(FF,l10,label) "xsolv_out ="
                        }
   "homogenousXS"       { set actualPar(FF,l1,label) "L ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "eta ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "SphSh+SD+homoXS"    { set actualPar(FF,l1,label) "t ="
                          set actualPar(FF,l2,label) "sigma_t ="
                          set actualPar(FF,l3,label) "R0 ="
                          set actualPar(FF,l4,label) "sigma_R ="
                          set actualPar(FF,l5,label) "eta ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "EllSh+SD+homoXS"    { set actualPar(FF,l1,label) "t ="
                          set actualPar(FF,l2,label) "sigma_t ="
                          set actualPar(FF,l3,label) "R0 ="
                          set actualPar(FF,l4,label) "sigma_R ="
                          set actualPar(FF,l5,label) "epsilon ="
                          set actualPar(FF,l6,label) "eta ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "EllSh+SD+homoXS(S)" { set actualPar(FF,l1,label) "t ="
                          set actualPar(FF,l2,label) "sigma_t ="
                          set actualPar(FF,l3,label) "S0 ="
                          set actualPar(FF,l4,label) "sigma_S ="
                          set actualPar(FF,l5,label) "epsilon ="
                          set actualPar(FF,l6,label) "eta ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "CylSh+SD+homoXS"    { set actualPar(FF,l1,label) "t ="
                          set actualPar(FF,l2,label) "sigma_t ="
                          set actualPar(FF,l3,label) "R0 ="
                          set actualPar(FF,l4,label) "sigma_R ="
                          set actualPar(FF,l5,label) "L0 ="
                          set actualPar(FF,l6,label) "sigma_L ="
                          set actualPar(FF,l7,label) "eta ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Disc+homoXS"        { set actualPar(FF,l1,label) "t ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "eta ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"TwoInfinitelyThinPlates" \
                        { set actualPar(FF,l1,label) "L ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"LayeredCentroSymmetricXS" \
                        { set actualPar(FF,l1,label) "L_out ="
                          set actualPar(FF,l2,label) "L_core ="
                          set actualPar(FF,l3,label) "b_out ="
                          set actualPar(FF,l4,label) "b_core ="
                          set actualPar(FF,l5,label) "D ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"BiLayerGauss"          { set actualPar(FF,l1,label) "sigma_out ="
                          set actualPar(FF,l2,label) "b_out ="
                          set actualPar(FF,l3,label) "sigma_core ="
                          set actualPar(FF,l4,label) "b_core ="
                          set actualPar(FF,l5,label) "t ="
                          set actualPar(FF,l6,label) "D ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "OrnsteinZernike"    { set actualPar(FF,l1,label) "xi ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "BroadPeak"          { set actualPar(FF,l1,label) "I0 ="
                          set actualPar(FF,l2,label) "xi ="
                          set actualPar(FF,l3,label) "q0 ="
                          set actualPar(FF,l4,label) "m ="
                          set actualPar(FF,l5,label) "p ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "TeubnerStrey"       { set actualPar(FF,l1,label) "xi ="
                          set actualPar(FF,l2,label) "d ="
                          set actualPar(FF,l3,label) "eta ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "DAB"                { set actualPar(FF,l1,label) "xi ="
                          set actualPar(FF,l2,label) "I0 ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Spinodal"           { set actualPar(FF,l1,label) "Qmax ="
                          set actualPar(FF,l2,label) "gamma ="
                          set actualPar(FF,l3,label) "Imax ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gauss"              { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gauss2"             { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "b_p ="
                          set actualPar(FF,l3,label) "V ="
                          set actualPar(FF,l4,label) "eta_s ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gauss3"             { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "b_p ="
                          set actualPar(FF,l3,label) "Mw ="
                          set actualPar(FF,l4,label) "rho_p ="
                          set actualPar(FF,l5,label) "eta_s ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "GaussPoly"          { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "Mw ="
                          set actualPar(FF,l3,label) "Mn ="
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"generalized Gaussian coil" \
                        { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "nu ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"generalized Gaussian coil 2" \
                        { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "nu ="
                          set actualPar(FF,l3,label) "b_p ="
                          set actualPar(FF,l4,label) "V ="
                          set actualPar(FF,l5,label) "eta_s ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"generalized Gaussian coil 3" \
                        { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "nu ="
                          set actualPar(FF,l3,label) "b_p ="
                          set actualPar(FF,l4,label) "Mw ="
                          set actualPar(FF,l5,label) "rho_p ="
                          set actualPar(FF,l6,label) "eta_s ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
 "FlexibleRingPolymer"  { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) ""
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
 "mMemberedTwistedRing" { set actualPar(FF,l1,label) "R_G,1r ="
                          set actualPar(FF,l2,label) "m ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
       "DaisyLikeRing"  { set actualPar(FF,l1,label) "R_G,1r ="
                          set actualPar(FF,l2,label) "m ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "BenoitStar"         { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "f ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
		        }
   "PolydisperseStar"   { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "f ="
                          set actualPar(FF,l3,label) ""
                          set actualPar(FF,l4,label) "I0 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
		        }
   "JuelichMicelle"     { set actualPar(FF,l1,label) "mcore/rcore ="
                          set actualPar(FF,l2,label) "mbrsh/rbrsh ="
                          set actualPar(FF,l3,label) "bcore ="
                          set actualPar(FF,l4,label) "bbrush ="
                          set actualPar(FF,l5,label) "Nagg ="
                          set actualPar(FF,l6,label) "sigma1 ="
                          set actualPar(FF,l7,label) "sigma3 ="
                          set actualPar(FF,l8,label) "d3+ ="
                          set actualPar(FF,l9,label) "gamma ="
                          set actualPar(FF,l10,label) "rhosolv ="
		        }
   "Francois1"          { set actualPar(FF,l1,label) "Rc ="
                          set actualPar(FF,l2,label) "Rm ="
                          set actualPar(FF,l3,label) "nu ="
                          set actualPar(FF,l4,label) "L ="
                          set actualPar(FF,l5,label) "b ="
                          set actualPar(FF,l6,label) "Nagg ="
                          set actualPar(FF,l7,label) "Drho ="
                          set actualPar(FF,l8,label) "K ="
                          set actualPar(FF,l9,label) "exvol ="
                          set actualPar(FF,l10,label) ""
		        }
"CopolymerMicelleR^-a"  { set actualPar(FF,l1,label) "Rc ="
                          set actualPar(FF,l2,label) "Vc ="
                          set actualPar(FF,l3,label) "Vsh ="
                          set actualPar(FF,l4,label) "rho_c ="
                          set actualPar(FF,l5,label) "rho_sh ="
                          set actualPar(FF,l6,label) "rho_solv ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
		        }
 "CopolymerMicelleRod"  { set actualPar(FF,l1,label) "Vpolym_core ="
                          set actualPar(FF,l2,label) "Vpolym_brsh ="
                          set actualPar(FF,l3,label) "eta_core ="
                          set actualPar(FF,l4,label) "eta_brsh ="
                          set actualPar(FF,l5,label) "eta_solv ="
                          set actualPar(FF,l6,label) "xsolv_core ="
                          set actualPar(FF,l7,label) "Rg ="
                          set actualPar(FF,l8,label) "d ="
                          set actualPar(FF,l9,label) "Nl ="
                          set actualPar(FF,l10,label) "L ="
		        }
 "CopolymerMicelleEll"  { set actualPar(FF,l1,label) "Vpolym_core ="
                          set actualPar(FF,l2,label) "Vpolym_brsh ="
                          set actualPar(FF,l3,label) "eta_core ="
                          set actualPar(FF,l4,label) "eta_brsh ="
                          set actualPar(FF,l5,label) "eta_solv ="
                          set actualPar(FF,l6,label) "xsolv_core ="
                          set actualPar(FF,l7,label) "Rg ="
                          set actualPar(FF,l8,label) "d ="
                          set actualPar(FF,l9,label) "Nagg ="
                          set actualPar(FF,l10,label) "epsilon ="
		        }
 "CopolymerMicelleCyl"  { set actualPar(FF,l1,label) "Vpolym_core ="
                          set actualPar(FF,l2,label) "Vpolym_brsh ="
                          set actualPar(FF,l3,label) "eta_core ="
                          set actualPar(FF,l4,label) "eta_brsh ="
                          set actualPar(FF,l5,label) "eta_solv ="
                          set actualPar(FF,l6,label) "xsolv_core ="
                          set actualPar(FF,l7,label) "Rg ="
                          set actualPar(FF,l8,label) "d ="
                          set actualPar(FF,l9,label) "Nagg ="
                          set actualPar(FF,l10,label) "L ="
		        }
   "DozierStar"         { set actualPar(FF,l1,label) "I0 ="
                          set actualPar(FF,l2,label) "Rg ="
                          set actualPar(FF,l3,label) "alpha ="
                          set actualPar(FF,l4,label) "xi ="
                          set actualPar(FF,l5,label) "nu"
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "DozierStar2"        { set actualPar(FF,l1,label) "I0 ="
                          set actualPar(FF,l2,label) "Rg ="
                          set actualPar(FF,l3,label) "Nagg ="
                          set actualPar(FF,l4,label) "xi ="
                          set actualPar(FF,l5,label) "nu"
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "BeacaugeExpPowLaw2"  { set actualPar(FF,l1,label) "G ="
                          set actualPar(FF,l2,label) "B ="
                          set actualPar(FF,l3,label) "R_i ="
                          set actualPar(FF,l4,label) "R_i+1 ="
                          set actualPar(FF,l5,label) "k ="
                          set actualPar(FF,l6,label) "P ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "BeacaugeExpPowLaw" { set actualPar(FF,l1,label) "G ="
                          set actualPar(FF,l2,label) "B ="
                          set actualPar(FF,l3,label) "Gs ="
                          set actualPar(FF,l4,label) "Bs ="
                          set actualPar(FF,l5,label) "Rg ="
                          set actualPar(FF,l6,label) "Rsub ="
                          set actualPar(FF,l7,label) "Rs ="
                          set actualPar(FF,l8,label) "P ="
                          set actualPar(FF,l9,label) "Ps ="
                          set actualPar(FF,l10,label) ""
                        }
   "Aggregate (Exp(-x^a) Cut-Off)"\
                        { set actualPar(FF,l1,label) "xi ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Aggregate (OverlapSph Cut-Off)"\
                        { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Fisher-Burford"     { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "RLCAggregation"     { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "DLCAggregation"     { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "MassFractExp"       { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "MassFractGauss"     { set actualPar(FF,l1,label) "Rg ="
                          set actualPar(FF,l2,label) "D ="
                          set actualPar(FF,l3,label) "I0 ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LangevinMH"         { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "Ms ="
                          set actualPar(FF,l3,label) "T ="
                          set actualPar(FF,l4,label) "M8 ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "A(B)"               { set actualPar(FF,l1,label) "Knuc ="
                          set actualPar(FF,l2,label) "Kmag ="
			  set actualPar(FF,l3,label) {M\[Tesla\] =}
			  set actualPar(FF,l4,label) {R\[nm\] =}
			  set actualPar(FF,l5,label) {T\[K\] =}
			  set actualPar(FF,l6,label) {T_fluct\[K\] =}
			  set actualPar(FF,l7,label) {B_r\[Tesla\] =}
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "B(B)"               { set actualPar(FF,l1,label) ""
                          set actualPar(FF,l2,label) "Kmag ="
			  set actualPar(FF,l3,label) {M\[Tesla\] =}
			  set actualPar(FF,l4,label) {R\[nm\] =}
			  set actualPar(FF,l5,label) {T\[K\] =}
			  set actualPar(FF,l6,label) {T_fluct\[K\] =}
			  set actualPar(FF,l7,label) {B_r\[Tesla\] =}
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "P39"                { set actualPar(FF,l1,label) "Nc ="
                          set actualPar(FF,l2,label) "Rg ="
                          set actualPar(FF,l3,label) "Rc ="
                          set actualPar(FF,l4,label) "rhos ="
                          set actualPar(FF,l5,label) "rhoc ="
                          set actualPar(FF,l6,label) "L ="
                          set actualPar(FF,l7,label) "b ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "SuperParStroboPsi"  { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "R ="
                          set actualPar(FF,l4,label) "R_av ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
   "SuperParStroboPsi2" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "Dl_l ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboTISANE1" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "Dt ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) "psi ="
                        }
  "SuperParStroboPsiSQ" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQBt1" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQLx" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQL2x" \
                        { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQL2x_SANSPOL" \
                        { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "pol ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQL2x_SANSPOL_albr" \
                        { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "pol ="
                          set actualPar(FF,l9,label) "d_phi ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQL2x_SANSPOL_albr_stat" \
                        { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "pol ="
                          set actualPar(FF,l9,label) "d_phi ="
                          set actualPar(FF,l10,label) "psi ="
                        }
"SuperParStroboPsiSQL2modx_SANSPOL" \
                        { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "SQ ="
                          set actualPar(FF,l4,label) "B1 ="
                          set actualPar(FF,l5,label) "mu_kT ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "pol ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
  "hysteresesStroboPsi" { set actualPar(FF,l1,label) "phi0 ="
                          set actualPar(FF,l2,label) "Knuc_Kmag ="
                          set actualPar(FF,l3,label) "Dl_l ="
                          set actualPar(FF,l4,label) "Bc ="
                          set actualPar(FF,l5,label) "S ="
                          set actualPar(FF,l6,label) "f ="
                          set actualPar(FF,l7,label) "B0 ="
                          set actualPar(FF,l8,label) "lambda ="
                          set actualPar(FF,l9,label) "SD ="
                          set actualPar(FF,l10,label) "psi ="
                        }
   "MieSphere"          { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "lam_sol ="
                          set actualPar(FF,l3,label) "m_re ="
                          set actualPar(FF,l4,label) "m_im ="
                          set actualPar(FF,l5,label) "pol ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "MieShell"           { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "lam_sol ="
                          set actualPar(FF,l4,label) "mc_re ="
                          set actualPar(FF,l5,label) "mc_im ="
                          set actualPar(FF,l6,label) "ms_re ="
                          set actualPar(FF,l7,label) "ms_im ="
                          set actualPar(FF,l8,label) "pol ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Beta (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "xmin ="
                          set actualPar(FF,l3,label) "xmax ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) "beta ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Beta (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "xmin ="
                          set actualPar(FF,l3,label) "xmax ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) "beta ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Chi-squared (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Chi-squared (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Erfc (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Erfc (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Error (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Error (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "exponentially modified Gaussian (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "distortion ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "exponentially modified Gaussian (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "distortion ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Extreme Value (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Extreme Value (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "F-variance (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape1 ="
                          set actualPar(FF,l5,label) "shape2 ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "fatique life (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "beta ="
                          set actualPar(FF,l4,label) "gamma ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "F-variance (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape1 ="
                          set actualPar(FF,l5,label) "shape2 ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gamma (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gamma (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian-Lorentzian cross product (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian-Lorentzian cross product (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian-Lorentzian sum (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Gaussian-Lorentzian sum (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "generalized Gaussian 1 (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "generalized Gaussian 1 (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "generalized Gaussian 2 (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "generalized Gaussian 2 (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Giddings (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Giddings (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Haarhoff Van der Linde (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "distortion ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
 "half Gaussian modified Gaussian (Area)" \
                        { set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "distortion ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Inverted Gamma (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Inverted Gamma (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Kumaraswamy (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "xmin ="
                          set actualPar(FF,l3,label) "xmax ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) "beta ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Kumaraswamy (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "xmin ="
                          set actualPar(FF,l3,label) "xmax ="
                          set actualPar(FF,l4,label) "alpha ="
                          set actualPar(FF,l5,label) "beta ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Laplace (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Laplace (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Logistic (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "x0 ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Logistic (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "x0 ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogLogistic (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "x0 ="
                          set actualPar(FF,l3,label) "mu ="
                          set actualPar(FF,l4,label) "sigma ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogLogistic (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "x0 ="
                          set actualPar(FF,l3,label) "mu ="
                          set actualPar(FF,l4,label) "sigma ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogNormal, 4 parameters (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogNormal, 4 parameters (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogNormal (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "mu ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "x0 ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "LogNormal (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "mu ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "x0 ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Lorentzian (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Lorentzian (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Pearson IV (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape1 ="
                          set actualPar(FF,l5,label) "shape2 ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Pearson IV (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape1 ="
                          set actualPar(FF,l5,label) "shape2 ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Pearson VII (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Pearson VII (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "power LogNormal (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "lambda ="
                          set actualPar(FF,l5,label) "p ="
                          set actualPar(FF,l6,label) "backgr. ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "power Normal (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "location ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "p ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Pulsed Peak (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "backgr. ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse with 2nd width (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width1 ="
                          set actualPar(FF,l4,label) "width2 ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse with 2nd width (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width1 ="
                          set actualPar(FF,l4,label) "width2 ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse with power term (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "pulse with power term (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Student-t (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Student-t (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Voigt (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "gamma ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Voigt (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "gamma ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Weibull (Area)"   { \
	                  set actualPar(FF,l1,label) "area ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Weibull (Amplitude)"   { \
	                  set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) "center ="
                          set actualPar(FF,l3,label) "width ="
                          set actualPar(FF,l4,label) "shape ="
                          set actualPar(FF,l5,label) "backgr. ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"confinement with Gaussian potential" \
                        { set actualPar(FF,l1,label) "amplitude ="
                          set actualPar(FF,l2,label) {Q \[Ångstrøm\] =}
                          set actualPar(FF,l3,label) "Emean ="
                          set actualPar(FF,l4,label) {D \[m^2/s\] =}
                          set actualPar(FF,l5,label) {<u^2> \[m^2\] =}
                          set actualPar(FF,l6,label) "sigma_0 ="
                          set actualPar(FF,l7,label) "sigma_1 ="
                          set actualPar(FF,l8,label) "sigma_2 ="
                          set actualPar(FF,l9,label) "background ="
                          set actualPar(FF,l10,label) "N ="
                        }
   "DLS_Sphere_RDG"     { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "eta ="
                          set actualPar(FF,l3,label) "T ="
                          set actualPar(FF,l4,label) "Q ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Robertus2"          { set actualPar(FF,l1,label) "Rmean ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "tau ="
                          set actualPar(FF,l5,label) "xfp2 ="
                          set actualPar(FF,l6,label) "fp ="
                          set actualPar(FF,l7,label) "eta_core ="
                          set actualPar(FF,l8,label) "eta_shell ="
                          set actualPar(FF,l9,label) "x_solvent ="
                          set actualPar(FF,l10,label) "eta_solvent ="
                        }
   "Robertus1"          { set actualPar(FF,l1,label) "Rmean ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "sigma ="
                          set actualPar(FF,l4,label) "btau ="
                          set actualPar(FF,l5,label) "atau ="
                          set actualPar(FF,l6,label) "fp ="
                          set actualPar(FF,l7,label) "eta_core ="
                          set actualPar(FF,l8,label) "eta_shell ="
                          set actualPar(FF,l9,label) "x_solvent ="
                          set actualPar(FF,l10,label) "eta_solvent ="
                        }
   "WormLikeChainEXV"   { set actualPar(FF,l1,label) "S0 ="
                          set actualPar(FF,l2,label) "RL ="
                          set actualPar(FF,l3,label) "RRL ="
                          set actualPar(FF,l4,label) "R ="
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "KholodenkoWorm"     { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "l ="
                          set actualPar(FF,l3,label) "L ="
                          set actualPar(FF,l4,label) ""
                          set actualPar(FF,l5,label) ""
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"SphereWithGaussChains" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "Rg ="
                          set actualPar(FF,l3,label) "d ="
                          set actualPar(FF,l4,label) "Nagg ="
                          set actualPar(FF,l5,label) "rc ="
                          set actualPar(FF,l6,label) "rs ="
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
"BlockCopolymerMicelle" { set actualPar(FF,l1,label) "Vpolym,c ="
                          set actualPar(FF,l2,label) "xsolv_c ="
                          set actualPar(FF,l3,label) "Vpoly,sh ="
                          set actualPar(FF,l4,label) "eta_polym,c ="
                          set actualPar(FF,l5,label) "eta_polym,sh ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "Nagg ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) ""
                        }
"SPHERE+Chains(SAW)" \
                        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg"
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+Chains(SAW)_Rc" \
                        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+Chains(SAW)_Nagg" \
                        { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+Chains(RW)"     { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg"
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) ""
                        }
"SPHERE+Chains(RW)_Rc" \
                        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) ""
                        }
"SPHERE+Chains(RW)_Nagg" \
                        { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) ""
                        }
"DISC+Chains(RW)"     \
                        { set actualPar(FF,l1,label) "L_core ="
                          set actualPar(FF,l2,label) "n_agg"
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "D ="
                        }
"DISC+Chains(RW)_Lc" \
                        { set actualPar(FF,l1,label) "L_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "D ="
                        }
"DISC+Chains(RW)_nagg" \
                        { set actualPar(FF,l1,label) "nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "D ="
                        }
"ELL+Chains(RW)"        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg"
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "epsilon ="
                        }
"ELL+Chains(RW)_Rc"     { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "epsilon ="
                        }
"ELL+Chains(RW)_Nagg"   { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "epsilon ="
                        }
"CYL+Chains(RW)"        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg"
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"CYL+Chains(RW)_Rc"     { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"CYL+Chains(RW)_Nagg"   { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Chains(RW)"        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "nagg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Chains(RW)_Rc"     { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Chains(RW)_nagg"   { set actualPar(FF,l1,label) "n_agg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "d ="
                          set actualPar(FF,l10,label) "H ="
                        }
"WORM+Chains(RW)"       { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "nagg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "l ="
                          set actualPar(FF,l10,label) "L ="
                        }
"WORM+Chains(RW)_Rc"    { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "l ="
                          set actualPar(FF,l10,label) "L ="
                        }
"WORM+Chains(RW)_nagg"  { set actualPar(FF,l1,label) "n_agg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "Rg ="
                          set actualPar(FF,l9,label) "l ="
                          set actualPar(FF,l10,label) "L ="
                        }
"SPHERE+Exp_Rc"         { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "x_solv_c ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "t ="
                          set actualPar(FF,l10,label) "x_out ="
                        }
"SPHERE+R^-a"           { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+R^-a_Rc"        { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE_smooth_interface+R^-a_Rc" {
                          set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "Deta_core ="
                          set actualPar(FF,l5,label) "Deta_brush ="
                          set actualPar(FF,l6,label) "sigma ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+R^-a_Manuela"   { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "D_eta_core ="
                          set actualPar(FF,l5,label) "D_eta_brush ="
                          set actualPar(FF,l6,label) "r_SiOx / mol. ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "a ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+R^-a_Manuela2"  { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "D_eta_core ="
                          set actualPar(FF,l5,label) "D_eta_brush ="
                          set actualPar(FF,l6,label) "r_SiOx / mol. ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "a ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE+R^-a_Nagg"      { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_sol ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"SPHERE_smooth_interface+R^-a_Nagg" { 
                          set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "Deta_core ="
                          set actualPar(FF,l5,label) "Deta_brush ="
                          set actualPar(FF,l6,label) "sigma ="
                          set actualPar(FF,l7,label) "alpha ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "L ="
                          set actualPar(FF,l10,label) "b ="
                        }
"ROD+R^-a"              { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "t ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+R^-a_Rc"           { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "t ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+R^-a_nagg"         { set actualPar(FF,l1,label) "nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "alpha ="
                          set actualPar(FF,l9,label) "t ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Gauss"             { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "sigma ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Gauss_Rc"          { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "sigma ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Gauss_nagg"        { set actualPar(FF,l1,label) "nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "xsolv_core ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "sigma ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Exp"               { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "n_agg ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "ro ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "alpha ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Exp_Rc"            { set actualPar(FF,l1,label) "R_core ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "ro ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "alpha ="
                          set actualPar(FF,l10,label) "H ="
                        }
"ROD+Exp_nagg"          { set actualPar(FF,l1,label) "nagg ="
                          set actualPar(FF,l2,label) "V_core ="
                          set actualPar(FF,l3,label) "V_brush ="
                          set actualPar(FF,l4,label) "eta_core ="
                          set actualPar(FF,l5,label) "eta_brush ="
                          set actualPar(FF,l6,label) "eta_solv ="
                          set actualPar(FF,l7,label) "ro ="
                          set actualPar(FF,l8,label) "t ="
                          set actualPar(FF,l9,label) "alpha ="
                          set actualPar(FF,l10,label) "H ="
                        }
   "ShearedCylinder"    { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) "Gamma ="
                          set actualPar(FF,l4,label) "psi ="
                          set actualPar(FF,l5,label) "eta ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "ShearedCylGauss"    { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "L ="
                          set actualPar(FF,l3,label) "theta_b ="
                          set actualPar(FF,l4,label) "psi ="
                          set actualPar(FF,l5,label) "eta ="
                          set actualPar(FF,l6,label) ""
                          set actualPar(FF,l7,label) ""
                          set actualPar(FF,l8,label) ""
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Andrea1"            { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "dR ="
                          set actualPar(FF,l3,label) "c ="
                          set actualPar(FF,l4,label) "core =" 
                          set actualPar(FF,l5,label) "dR2 ="
                          set actualPar(FF,l6,label) "shell1 ="
                          set actualPar(FF,l7,label) "shell2 ="
                          set actualPar(FF,l8,label) "c2 ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "Simon1"             { set actualPar(FF,l1,label) "Nagg ="
                          set actualPar(FF,l2,label) "Rg ="
                          set actualPar(FF,l3,label) "d ="
                          set actualPar(FF,l4,label) "rho_solv =" 
                          set actualPar(FF,l5,label) "rho_core ="
                          set actualPar(FF,l6,label) "b_A_h ="
                          set actualPar(FF,l7,label) "V_A_h ="
                          set actualPar(FF,l8,label) "b_A_l ="
                          set actualPar(FF,l9,label) "V_A_l"
                          set actualPar(FF,l10,label) "rho_B"
                        }
   "DoubleShell_withSD" { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "s ="
                          set actualPar(FF,l3,label) "eta_c ="
                          set actualPar(FF,l4,label) "eta_1 =" 
                          set actualPar(FF,l5,label) "eta_2 ="
                          set actualPar(FF,l6,label) "eta_m ="
                          set actualPar(FF,l7,label) "d_1 ="
                          set actualPar(FF,l8,label) "d_2 ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   "StackDiscs"         { set actualPar(FF,l1,label) "R ="
                          set actualPar(FF,l2,label) "d ="
                          set actualPar(FF,l3,label) "h ="
                          set actualPar(FF,l4,label) "n =" 
                          set actualPar(FF,l5,label) "eta_c ="
                          set actualPar(FF,l6,label) "eta_l ="
                          set actualPar(FF,l7,label) "sigma ="
                          set actualPar(FF,l8,label) "D ="
                          set actualPar(FF,l9,label) ""
                          set actualPar(FF,l10,label) ""
                        }
   default              { ap_set_param_labels actualPar FF }
}
switch -exact $actualPar(SD,typestr) {
   "Monodisperse" { set actualPar(SD,a1,label) ""
                    set actualPar(SD,a2,label) ""
                    set actualPar(SD,a3,label) ""
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) "" 
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "A_div_B"      { set actualPar(SD,a1,label) "A ="
                    set actualPar(SD,a2,label) "B ="
                    set actualPar(SD,a3,label) ""
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) "" 
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Delta"        { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) ""
                    set actualPar(SD,a3,label) ""
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) "" 
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "LogNorm"      { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "s ="
                    set actualPar(SD,a3,label) "p ="
                    set actualPar(SD,a4,label) "mu =" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "BiLogNorm"    { set actualPar(SD,a1,label) "N_1 ="
                    set actualPar(SD,a2,label) "s_1 ="
                    set actualPar(SD,a3,label) "p_1 ="
                    set actualPar(SD,a4,label) "X0_1 =" 
                    set actualPar(SD,a5,label) "N_2 ="
                    set actualPar(SD,a6,label) "s_2 ="
                    set actualPar(SD,a7,label) "p_2 ="
                    set actualPar(SD,a8,label) "X0_2 ="  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "GEX"          { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "gamma ="
                    set actualPar(SD,a3,label) "beta ="
                    set actualPar(SD,a4,label) "lambda =" 
                    set actualPar(SD,a5,label) "X0 ="
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "GEV"          { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "xi ="
                    set actualPar(SD,a3,label) "s ="
                    set actualPar(SD,a4,label) "mu =" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "gammaSD"      { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "mode ="
                    set actualPar(SD,a3,label) "sigma ="
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Triangular"   { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "Xmin ="
                    set actualPar(SD,a3,label) "Xmode ="
                    set actualPar(SD,a4,label) "Xmax =" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "fractalSD"    { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "Xmin ="
                    set actualPar(SD,a3,label) "Xmax ="
                    set actualPar(SD,a4,label) "fD =" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Uniform"      { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "Xmin ="
                    set actualPar(SD,a3,label) "Xmax ="
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Beta"         { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "Xmin ="
                    set actualPar(SD,a3,label) "Xmax ="
                    set actualPar(SD,a4,label) "alpha =" 
                    set actualPar(SD,a5,label) "beta ="
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Maxwell"      { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "s ="
                    set actualPar(SD,a3,label) "X0"
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) "" 
                  }
   "Schultz-Zimm" { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "Xa ="
                    set actualPar(SD,a3,label) "sig ="
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) ""  
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) ""
                  }
   "Weibull"      { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "alpha ="
                    set actualPar(SD,a3,label) "lambda ="
                    set actualPar(SD,a4,label) "mu =" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) "" 
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) ""
                  }
   "Gaussian"     { set actualPar(SD,a1,label) "N ="
                    set actualPar(SD,a2,label) "s ="
                    set actualPar(SD,a3,label) "X0 ="
                    set actualPar(SD,a4,label) "" 
                    set actualPar(SD,a5,label) ""
                    set actualPar(SD,a6,label) ""
                    set actualPar(SD,a7,label) ""
                    set actualPar(SD,a8,label) "" 
                    set actualPar(SD,a9,label) ""
                    set actualPar(SD,a10,label) ""
                  }
   default        { ap_set_param_labels actualPar SD }
}
switch $actualPar(SQ,typestr) {
   "None"               { set actualPar(SQ,s1,label) ""
                          set actualPar(SQ,s2,label) ""
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Hard Sphere"        { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "fp ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Sticky Hard Sphere" { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "tau ="
                          set actualPar(SQ,s3,label) "fp ="
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
 "Sticky Hard Sphere 2" { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "tau ="
                          set actualPar(SQ,s3,label) "Delta ="
                          set actualPar(SQ,s4,label) "fp ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"Square Well Potential" { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "eta ="
                          set actualPar(SQ,s3,label) "epsi/kBT ="
                          set actualPar(SQ,s4,label) "lambda ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"Thin Square Well Potential" \
                        { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "phi ="
                          set actualPar(SQ,s3,label) "U/kT ="
                          set actualPar(SQ,s4,label) "epsilon ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"Square Well Potential 2" { set actualPar(SQ,s1,label) "RHS ="
                          set actualPar(SQ,s2,label) "eta ="
                          set actualPar(SQ,s3,label) "epsi/kB ="
                          set actualPar(SQ,s4,label) "Delta ="
                          set actualPar(SQ,s5,label) "T ="
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Mass Fractal (Exp Cut-Off)" \
                        { set actualPar(SQ,s1,label) "r0 ="
                          set actualPar(SQ,s2,label) "xi ="
                          set actualPar(SQ,s3,label) "D ="
                          set actualPar(SQ,s4,label) "" 
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Mass Fractal (Gaussian Cut-Off)"\
                        { set actualPar(SQ,s1,label) "r0 ="
                          set actualPar(SQ,s2,label) "xi ="
                          set actualPar(SQ,s3,label) "D ="
                          set actualPar(SQ,s4,label) "" 
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Mass Fractal (OverapSph Cut-Off)"\
                        { set actualPar(SQ,s1,label) "r0 ="
                          set actualPar(SQ,s2,label) "xi ="
                          set actualPar(SQ,s3,label) "D ="
                          set actualPar(SQ,s4,label) "" 
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Mass Fractal (Exp(-x^a) Cut-Off)"\
                        { set actualPar(SQ,s1,label) "r0 ="
                          set actualPar(SQ,s2,label) "xi ="
                          set actualPar(SQ,s3,label) "D ="
                          set actualPar(SQ,s4,label) "alpha=" 
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "Cylinder(PRISM)"    { set actualPar(SQ,s1,label) "R ="
                          set actualPar(SQ,s2,label) "L ="
                          set actualPar(SQ,s3,label) "nu ="
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "RPA"                { set actualPar(SQ,s1,label) "nu ="
                          set actualPar(SQ,s2,label) ""
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "ThermalDisorder"    { set actualPar(SQ,s1,label) "N ="
                          set actualPar(SQ,s2,label) "d ="
                          set actualPar(SQ,s3,label) "Delta ="
                          set actualPar(SQ,s4,label) "Nu ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
      "Paracrystalline" { set actualPar(SQ,s1,label) "N ="
                          set actualPar(SQ,s2,label) "d ="
                          set actualPar(SQ,s3,label) "Delta ="
                          set actualPar(SQ,s4,label) "Nu ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
      "ModifiedCaille"  { set actualPar(SQ,s1,label) "N ="
                          set actualPar(SQ,s2,label) "d ="
                          set actualPar(SQ,s3,label) "eta ="
                          set actualPar(SQ,s4,label) "Nu ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
 "GaussianAmplitude"    { set actualPar(SQ,s1,label) "bckgr ="
                          set actualPar(SQ,s2,label) "Ampl. ="
                          set actualPar(SQ,s3,label) "mean ="
                          set actualPar(SQ,s4,label) "width ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
 "LorentzianAmplitude"   { set actualPar(SQ,s1,label) "bckgr ="
                          set actualPar(SQ,s2,label) "Ampl. ="
                          set actualPar(SQ,s3,label) "mean ="
                          set actualPar(SQ,s4,label) "width ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
      "VoigtPeak"       { set actualPar(SQ,s1,label) "Area ="
                          set actualPar(SQ,s2,label) "mean ="
                          set actualPar(SQ,s3,label) "sigma ="
                          set actualPar(SQ,s4,label) "gamma ="
			  set actualPar(SQ,s5,label) "c0 ="
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "PearsonVIIAmplitude"  { set actualPar(SQ,s1,label) "Ampl. ="
                          set actualPar(SQ,s2,label) "mean ="
                          set actualPar(SQ,s3,label) "width1 ="
                          set actualPar(SQ,s4,label) "width2 =" 
			  set actualPar(SQ,s5,label) "bckgr ="
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
    "Correlation Hole"  { set actualPar(SQ,s1,label) "h ="
                          set actualPar(SQ,s2,label) "eta ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "Critical Scattering" { set actualPar(SQ,s1,label) "kappa ="
                          set actualPar(SQ,s2,label) "zeta ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "Macro Ion (HP)"      { set actualPar(SQ,s1,label) {T \[°C\] =}
                          set actualPar(SQ,s2,label) "Z_eff ="
                          set actualPar(SQ,s3,label) "RHS ="
                          set actualPar(SQ,s4,label) "ION ="
                          set actualPar(SQ,s5,label) "ETA ="
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "Hayter Penfold RMSA" { set actualPar(SQ,s1,label) {RHS \[nm\] =}
                          set actualPar(SQ,s2,label) "Z ="
                          set actualPar(SQ,s3,label) "eta ="
                          set actualPar(SQ,s4,label) {T \[K\] =}
                          set actualPar(SQ,s5,label) {salt \[M\] =}
                          set actualPar(SQ,s6,label) "eps_r ="
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "BabaAhmed" \
	                { set actualPar(SQ,s1,label) {Deff \[nm\] =}
		 	  set actualPar(SQ,s2,label) {Psi,0,eff \[Nm/C\] =}
			  set actualPar(SQ,s3,label) {epsilon \[C^2/(Nm^2)\] =}
                          set actualPar(SQ,s4,label) "eta ="
			  set actualPar(SQ,s5,label) {kD \[nm^-1\] =}
			  set actualPar(SQ,s6,label) {T \[K\] =}
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "BabaAhmed2" \
	                { set actualPar(SQ,s1,label) {R_eff \[nm\] =}
                          set actualPar(SQ,s2,label) "z_p ="
                          set actualPar(SQ,s3,label) "epsilon_r ="
		          set actualPar(SQ,s4,label) {mB \[mol/l\] =}
                          set actualPar(SQ,s5,label) "zB ="
                          set actualPar(SQ,s6,label) "eta ="
			  set actualPar(SQ,s7,label) {T \[K\] =}
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "LocalOrder"          { set actualPar(SQ,s1,label) "D ="
                          set actualPar(SQ,s2,label) "alpha ="
                          set actualPar(SQ,s3,label) "z ="
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
  "RandomDistribution"  { set actualPar(SQ,s1,label) "Rca ="
                          set actualPar(SQ,s2,label) "R ="
                          set actualPar(SQ,s3,label) "fp ="
                          set actualPar(SQ,s4,label) "epsilon ="
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "P'(Q):ThinDisc"     { set actualPar(SQ,s1,label) "R ="
                          set actualPar(SQ,s2,label) ""
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "P'(Q):ThinSphericalShell" \
                        { set actualPar(SQ,s1,label) "R ="
                          set actualPar(SQ,s2,label) ""
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"P'(Q):ThinEllipsoidalShell" \
                        { set actualPar(SQ,s1,label) "a ="
                          set actualPar(SQ,s2,label) "b"
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                         }
"P'(Q):ThinHollowCylinder" \
                        { set actualPar(SQ,s1,label) "R ="
                          set actualPar(SQ,s2,label) "L"
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   "P'(Q):Rod"          { set actualPar(SQ,s1,label) "L ="
                          set actualPar(SQ,s2,label) ""
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"P'(Q):Worm(PS1)" \
                        { set actualPar(SQ,s1,label) "lb ="
			  set actualPar(SQ,s2,label) "L ="
			  set actualPar(SQ,s3,label) "exvol ="
			  set actualPar(SQ,s4,label) ""
			  set actualPar(SQ,s5,label) ""
			  set actualPar(SQ,s6,label) ""
			  set actualPar(SQ,s7,label) ""
			  set actualPar(SQ,s8,label) ""
			  set actualPar(SQ,s9,label) ""
			  set actualPar(SQ,s10,label) ""
			}
"P'(Q):Worm(PS1)" \
                        { set actualPar(SQ,s1,label) "lb ="
			  set actualPar(SQ,s2,label) "L ="
			  set actualPar(SQ,s3,label) ""
			  set actualPar(SQ,s4,label) ""
			  set actualPar(SQ,s5,label) ""
			  set actualPar(SQ,s6,label) ""
			  set actualPar(SQ,s7,label) ""
			  set actualPar(SQ,s8,label) ""
			  set actualPar(SQ,s9,label) ""
			  set actualPar(SQ,s10,label) ""
			}
"P'(Q):Worm(PS3)" \
                        { set actualPar(SQ,s1,label) "lb ="
			  set actualPar(SQ,s2,label) "L ="
			  set actualPar(SQ,s3,label) "exvol ="
			  set actualPar(SQ,s4,label) ""
			  set actualPar(SQ,s5,label) ""
			  set actualPar(SQ,s6,label) ""
			  set actualPar(SQ,s7,label) ""
			  set actualPar(SQ,s8,label) ""
			  set actualPar(SQ,s9,label) ""
			  set actualPar(SQ,s10,label) ""
			}
"P'(Q):WormLikeChain_withEXV" \
                        { set actualPar(SQ,s1,label) "lb ="
                          set actualPar(SQ,s2,label) "L ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"P'(Q):WormLikeChain_withoutEXV" \
                        { set actualPar(SQ,s1,label) "lb ="
                          set actualPar(SQ,s2,label) "L ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
"P'(Q):KholodenkoWorm"  { set actualPar(SQ,s1,label) "lb ="
                          set actualPar(SQ,s2,label) "L ="
                          set actualPar(SQ,s3,label) ""
                          set actualPar(SQ,s4,label) ""
                          set actualPar(SQ,s5,label) ""
                          set actualPar(SQ,s6,label) ""
                          set actualPar(SQ,s7,label) ""
                          set actualPar(SQ,s8,label) ""
                          set actualPar(SQ,s9,label) ""
                          set actualPar(SQ,s10,label) ""
                        }
   default              { ap_set_param_labels actualPar SQ }
}
ap_update_param_count actualPar SD
ap_update_param_count actualPar SQ
ap_update_param_count actualPar FF

foreach scat_contr {FF SD SQ} varname {l a s} {
   set tmpvar [lindex $Par(${scat_contr},${varname}) $ti]
   set tmperr [lindex $Par(${scat_contr},err)        $ti]
   set tmpmin [lindex $Par(${scat_contr},min)        $ti]
   set tmpmax [lindex $Par(${scat_contr},max)        $ti]
   set tmplim [lindex $Par(${scat_contr},limits)     $ti]
   set tmpact [lindex $Par(${scat_contr},active)     $ti]
   if {[string compare $scat_contr FF] == 0} {
      set tmpdis [lindex $Par(${scat_contr},distr)      $ti]
   }
   for {set i 1} {$i <= $actualPar($scat_contr,param_count)} {incr i} {
       set actualPar(${scat_contr},${varname}${i},var)    [lindex $tmpvar [expr $i-1]]
       set actualPar(${scat_contr},${varname}${i},err)    [lindex $tmperr [expr $i-1]]
       set actualPar(${scat_contr},${varname}${i},min)    [lindex $tmpmin [expr $i-1]]
       set actualPar(${scat_contr},${varname}${i},max)    [lindex $tmpmax [expr $i-1]]
       set actualPar(${scat_contr},${varname}${i},limits) [lindex $tmplim [expr $i-1]]
       set actualPar(${scat_contr},${varname}${i},active) [lindex $tmpact [expr $i-1]]
       if {[string compare $scat_contr FF] == 0} {
          set actualPar(${scat_contr},${varname}${i},distr)  [lindex $tmpdis [expr $i-1]]
       }
   }
}

set actualPar(alambda) $Par(alambda);
set actualPar(error) $Par(error);

return 1
}

# check which entries and buttons are active, depending on the values of
# actualAnalytPar(SD,typestr) and actualAnalyt(FF,typestr)
# $type contains 'SD', 'SQ' or 'FF'
proc set_actualAP_labels {type AactualAnalytPar
} {
	upvar $AactualAnalytPar actualAnalytPar
	global distr

	set p [get_old_param_name $type]
	array unset actualAnalytPar "$type,${p}*,label"

	switch -exact $type {
		SD { switch $actualAnalytPar(SD,typestr) {
	   "Monodisperse" { set actualAnalytPar(SD,a1,label) ""
			    set actualAnalytPar(SD,a2,label) ""
			    set actualAnalytPar(SD,a3,label) ""
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "A_div_B"      { set actualAnalytPar(SD,a1,label) "A ="
			    set actualAnalytPar(SD,a2,label) "B ="
			    set actualAnalytPar(SD,a3,label) ""
			    set actualAnalytPar(SD,a4,label) ""
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "Delta"        { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) ""
			    set actualAnalytPar(SD,a3,label) ""
			    set actualAnalytPar(SD,a4,label) ""
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "LogNorm"      { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "s ="
			    set actualAnalytPar(SD,a3,label) "p ="
			    set actualAnalytPar(SD,a4,label) "mu =" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "BiLogNorm"    { set actualAnalytPar(SD,a1,label) "N_1 ="
			    set actualAnalytPar(SD,a2,label) "s_1 ="
			    set actualAnalytPar(SD,a3,label) "p_1 ="
			    set actualAnalytPar(SD,a4,label) "mu_1 =" 
			    set actualAnalytPar(SD,a5,label) "N_2 ="
			    set actualAnalytPar(SD,a6,label) "s_2 ="
			    set actualAnalytPar(SD,a7,label) "p_2 ="
			    set actualAnalytPar(SD,a8,label) "mu_2 ="
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "GEX"          { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "gamma ="
			    set actualAnalytPar(SD,a3,label) "beta ="
			    set actualAnalytPar(SD,a4,label) "lambda =" 
			    set actualAnalytPar(SD,a5,label) "X0 ="
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "GEV"          { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "xi ="
			    set actualAnalytPar(SD,a3,label) "s ="
			    set actualAnalytPar(SD,a4,label) "mu =" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "gammaSD"      { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "mode ="
			    set actualAnalytPar(SD,a3,label) "sigma ="
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "Triangular"   { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "Xmin ="
			    set actualAnalytPar(SD,a3,label) "X0 ="
			    set actualAnalytPar(SD,a4,label) "Xmax =" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			  }
	   "fractalSD"    { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "Xmin ="
			    set actualAnalytPar(SD,a3,label) "Xmax ="
			    set actualAnalytPar(SD,a4,label) "fD =" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			  }
	   "Uniform"      { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "Xmin ="
			    set actualAnalytPar(SD,a3,label) "Xmax ="
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			  }
	   "Beta"         { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "Xmin ="
			    set actualAnalytPar(SD,a3,label) "Xmax ="
			    set actualAnalytPar(SD,a4,label) "alpha =" 
			    set actualAnalytPar(SD,a5,label) "beta ="
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			  }
	   "Gaussian"     { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "s ="
			    set actualAnalytPar(SD,a3,label) "X0 ="
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "Maxwell"      { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "s ="
			    set actualAnalytPar(SD,a3,label) "X0"
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "Schultz-Zimm" { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "Ra ="
			    set actualAnalytPar(SD,a3,label) "sig ="
			    set actualAnalytPar(SD,a4,label) "" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   "Weibull"      { set actualAnalytPar(SD,a1,label) "N ="
			    set actualAnalytPar(SD,a2,label) "alpha ="
			    set actualAnalytPar(SD,a3,label) "lambda ="
			    set actualAnalytPar(SD,a4,label) "mu =" 
			    set actualAnalytPar(SD,a5,label) ""
			    set actualAnalytPar(SD,a6,label) ""
			    set actualAnalytPar(SD,a7,label) ""
			    set actualAnalytPar(SD,a8,label) ""
			    set actualAnalytPar(SD,a9,label) ""
			    set actualAnalytPar(SD,a10,label) ""
			  }
	   default        { ap_set_param_labels actualAnalytPar SD }
		} }
		FF { switch $actualAnalytPar(FF,typestr) {
	   "Background"         { set actualAnalytPar(FF,l1,label) "c_0 ="
				  set actualAnalytPar(FF,l2,label) "c_1 ="
				  set actualAnalytPar(FF,l3,label) "c_4 ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
 "p(r) -> 4 pi sin(qr)/(qr)"  \
                                { set actualAnalytPar(FF,l1,label) "r ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
 "gamma(r) -> 4 pi r^2 sin(qr)/(qr)"  \
                                { set actualAnalytPar(FF,l1,label) "r ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Guinier"            { set actualAnalytPar(FF,l1,label) "I0 ="
				  set actualAnalytPar(FF,l2,label) "a ="
				  set actualAnalytPar(FF,l3,label) "Ra ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Sphere"             { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "HardSphere"         { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "RHS ="
				  set actualAnalytPar(FF,l3,label) "fp ="
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "StickyHardSphere"   { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "RHS ="
				  set actualAnalytPar(FF,l3,label) "tau ="
				  set actualAnalytPar(FF,l4,label) "fp ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Spherical Shell i"  { set actualAnalytPar(FF,l1,label) "R1 ="
				  set actualAnalytPar(FF,l2,label) "R2 ="
				  set actualAnalytPar(FF,l3,label) "mu ="
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Spherical Shell ii" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "nu ="
				  set actualAnalytPar(FF,l3,label) "mu ="
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	  "Spherical Shell iii" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "eta1 ="
				  set actualAnalytPar(FF,l4,label) "eta2 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	 "MultiLamellarVesicle" { set actualAnalytPar(FF,l1,label) "R_c ="
				  set actualAnalytPar(FF,l2,label) "t_sh ="
				  set actualAnalytPar(FF,l3,label) "t_sol ="
				  set actualAnalytPar(FF,l4,label) "eta_sh ="
				  set actualAnalytPar(FF,l5,label) "eta_sol ="
				  set actualAnalytPar(FF,l6,label) "n ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	 "RNDMultiLamellarVesicle" { 
				  set actualAnalytPar(FF,l1,label) "R_c ="
				  set actualAnalytPar(FF,l2,label) "t_sh ="
				  set actualAnalytPar(FF,l3,label) "t_sol ="
				  set actualAnalytPar(FF,l4,label) "Dt_sol ="
				  set actualAnalytPar(FF,l5,label) "eta_sh ="
				  set actualAnalytPar(FF,l6,label) "eta_sol ="
				  set actualAnalytPar(FF,l7,label) "n ="
				  set actualAnalytPar(FF,l8,label) "sigms_Rc ="
				  set actualAnalytPar(FF,l9,label) "sigma_tsh ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	 "RNDMultiLamellarVesicle2" { 
				  set actualAnalytPar(FF,l1,label) "t_sh ="
				  set actualAnalytPar(FF,l2,label) "sigma_tsh ="
				  set actualAnalytPar(FF,l3,label) "Rc ="
				  set actualAnalytPar(FF,l4,label) "sigma_Rc ="
				  set actualAnalytPar(FF,l5,label) "n ="
				  set actualAnalytPar(FF,l6,label) "sigma_n ="
				  set actualAnalytPar(FF,l7,label) "tsol ="
				  set actualAnalytPar(FF,l8,label) "sigma_tsol"
				  set actualAnalytPar(FF,l9,label) "Dtsol ="
				  set actualAnalytPar(FF,l10,label) "Delta_eta ="
				}
	 "MLVesicleFrielinghaus" { 
				  set actualAnalytPar(FF,l1,label) "n ="
				  set actualAnalytPar(FF,l2,label) "nw ="
				  set actualAnalytPar(FF,l3,label) "rho_sol ="
				  set actualAnalytPar(FF,l4,label) "rho_core ="
				  set actualAnalytPar(FF,l5,label) "rho_shell ="
				  set actualAnalytPar(FF,l6,label) "sig_core ="
				  set actualAnalytPar(FF,l7,label) "sig_shell ="
				  set actualAnalytPar(FF,l8,label) "Rmain ="
				  set actualAnalytPar(FF,l9,label) "zz ="
				  set actualAnalytPar(FF,l10,label) "Rshift ="
				}
	 "BiLayeredVesicle"     { set actualAnalytPar(FF,l1,label) "R_c ="
				  set actualAnalytPar(FF,l2,label) "t_h ="
				  set actualAnalytPar(FF,l3,label) "t_t ="
				  set actualAnalytPar(FF,l4,label) "eta_sol ="
				  set actualAnalytPar(FF,l5,label) "eta_h ="
				  set actualAnalytPar(FF,l6,label) "eta_t ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	  "MagneticShellPsi"    { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "mag_sh ="
				  set actualAnalytPar(FF,l8,label) "mag_m ="
				  set actualAnalytPar(FF,l9,label) "psi ="
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	  "MagneticShellAniso"  { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "mag_sh ="
				  set actualAnalytPar(FF,l8,label) "mag_m ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	"MagneticShellCrossTerm"  { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "mag_sh ="
				  set actualAnalytPar(FF,l8,label) "mag_m ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	"SuperparamagneticFFpsi" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "R0 ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "psi ="
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	"SuperparamagneticFFCrossTerm" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "R0 ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "psi ="
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	"SuperparamagneticFFAniso" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "R0 ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	"SuperparamagneticFFIso" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "nuc_c ="
				  set actualAnalytPar(FF,l4,label) "nuc_sh ="
				  set actualAnalytPar(FF,l5,label) "nuc_m ="
				  set actualAnalytPar(FF,l6,label) "mag_c ="
				  set actualAnalytPar(FF,l7,label) "R0 ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) "pol ="
				}
	  "LinShell"            { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "eta_c ="
				  set actualAnalytPar(FF,l4,label) "eta_sh =" 
				  set actualAnalytPar(FF,l5,label) "x_in ="
				  set actualAnalytPar(FF,l6,label) "x_out ="
				  set actualAnalytPar(FF,l7,label) "eta_s ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}  
	  "LinShell2"           { set actualAnalytPar(FF,l1,label) "Rtot ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "eta_c ="
				  set actualAnalytPar(FF,l4,label) "eta_sh =" 
				  set actualAnalytPar(FF,l5,label) "x_in ="
				  set actualAnalytPar(FF,l6,label) "x_out ="
				  set actualAnalytPar(FF,l7,label) "eta_s ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}   
	  "SphereShell_compr"   { set actualAnalytPar(FF,l1,label) "Rtot ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "eta_c ="
				  set actualAnalytPar(FF,l4,label) "eta_sh =" 
				  set actualAnalytPar(FF,l5,label) "x_sol ="
				  set actualAnalytPar(FF,l6,label) "eta_s "
				  set actualAnalytPar(FF,l7,label) "kappa_c ="
				  set actualAnalytPar(FF,l8,label) "kappa_sh ="
				  set actualAnalytPar(FF,l9,label) "kappa_s ="
				  set actualAnalytPar(FF,l10,label) "P ="
				}
	  "ExpShell"            { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "eta_core ="
				  set actualAnalytPar(FF,l4,label) "eta_shell =" 
				  set actualAnalytPar(FF,l5,label) "x_in_solv ="
				  set actualAnalytPar(FF,l6,label) "x_out_solv ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "eta_solvent ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}   
	  "DumbbellShell"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_c =" 
				  set actualAnalytPar(FF,l5,label) "eta_sh ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				} 
	 "two_attached_spheres" { set actualAnalytPar(FF,l1,label) "R1 ="
				  set actualAnalytPar(FF,l2,label) "R2 ="
				  set actualAnalytPar(FF,l3,label) "eta0 ="
				  set actualAnalytPar(FF,l4,label) "eta1 =" 
				  set actualAnalytPar(FF,l5,label) "eta2 ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}        
	  "DoubleShellChain"    { set actualAnalytPar(FF,l1,label) "R_c ="
				  set actualAnalytPar(FF,l2,label) "DR1 ="
				  set actualAnalytPar(FF,l3,label) "DR2 ="
				  set actualAnalytPar(FF,l4,label) "L =" 
				  set actualAnalytPar(FF,l5,label) "eta_c ="
				  set actualAnalytPar(FF,l6,label) "eta_1 ="
				  set actualAnalytPar(FF,l7,label) "eta_2 ="
				  set actualAnalytPar(FF,l8,label) "eta_sol ="
				  set actualAnalytPar(FF,l9,label) "x_sol_sh2 ="
				  set actualAnalytPar(FF,l10,label) "n ="
				}        
	"TetrahedronDoubleShell" {set actualAnalytPar(FF,l1,label) "R_c ="
				  set actualAnalytPar(FF,l2,label) "DR1 ="
				  set actualAnalytPar(FF,l3,label) "DR2 ="
				  set actualAnalytPar(FF,l4,label) "L =" 
				  set actualAnalytPar(FF,l5,label) "eta_c ="
				  set actualAnalytPar(FF,l6,label) "eta_1 ="
				  set actualAnalytPar(FF,l7,label) "eta_2 ="
				  set actualAnalytPar(FF,l8,label) "eta_sol ="
				  set actualAnalytPar(FF,l9,label) "x_sol_sh2 ="
				  set actualAnalytPar(FF,l10,label) "n ="
				} 
	 "EllipsoidalCoreShell" { set actualAnalytPar(FF,l1,label) "a ="
				  set actualAnalytPar(FF,l2,label) "b ="
				  set actualAnalytPar(FF,l3,label) "t ="
				  set actualAnalytPar(FF,l4,label) "eta_c ="
				  set actualAnalytPar(FF,l5,label) "eta_sh ="
				  set actualAnalytPar(FF,l6,label) "eta_sol"
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	 "triaxEllShell1"       { set actualAnalytPar(FF,l1,label) "a ="
				  set actualAnalytPar(FF,l2,label) "b ="
				  set actualAnalytPar(FF,l3,label) "c ="
				  set actualAnalytPar(FF,l4,label) "t ="
				  set actualAnalytPar(FF,l5,label) "eta_c ="
				  set actualAnalytPar(FF,l6,label) "eta_sh ="
				  set actualAnalytPar(FF,l7,label) "eta_sol ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Ellipsoid i"        { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "V ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Ellipsoid ii"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "nu ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "EllCylShell"         \
				{ set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "epsilon ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "t ="
				  set actualAnalytPar(FF,l5,label) "eta_core ="
				  set actualAnalytPar(FF,l6,label) "eta_shell ="
				  set actualAnalytPar(FF,l7,label) "eta_sol ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Cylinder"           { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "FlatCylinder"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LongCylinder"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "PorodCylinder"      { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LongCylShell"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
           "ellCylShell1"       { set actualAnalytPar(FF,l1,label) "R ="
                                  set actualAnalytPar(FF,l2,label) "epsilon ="
                                  set actualAnalytPar(FF,l3,label) "L ="
                                  set actualAnalytPar(FF,l4,label) "t ="
                                  set actualAnalytPar(FF,l5,label) "eta_core ="
                                  set actualAnalytPar(FF,l6,label) "eta_shell ="
                                  set actualAnalytPar(FF,l7,label) "eta_sol ="
                                  set actualAnalytPar(FF,l8,label) ""
                                  set actualAnalytPar(FF,l9,label) ""
                                  set actualAnalytPar(FF,l10,label) ""
                                }
           "ellCylShell2"       { set actualAnalytPar(FF,l1,label) "R ="
                                  set actualAnalytPar(FF,l2,label) "epsilon ="
                                  set actualAnalytPar(FF,l3,label) "L ="
                                  set actualAnalytPar(FF,l4,label) "t ="
                                  set actualAnalytPar(FF,l5,label) "eta_core ="
                                  set actualAnalytPar(FF,l6,label) "eta_shell ="
                                  set actualAnalytPar(FF,l7,label) "eta_sol ="
                                  set actualAnalytPar(FF,l8,label) ""
                                  set actualAnalytPar(FF,l9,label) ""
                                  set actualAnalytPar(FF,l10,label) ""
                                }
	"Pcs:homogeneousCyl"    { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "eta_core ="
				  set actualAnalytPar(FF,l3,label) "eta_solv ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"Pcs:ellCylSh"          { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "epsilon ="
				  set actualAnalytPar(FF,l3,label) "t ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "CylShell1"          { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "CylShell2"          { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"alignedCylShell"       { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "psi ="
				  set actualAnalytPar(FF,l8,label) "theta ="
				  set actualAnalytPar(FF,l9,label) "phi ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"partly aligned CylShell" \
				{ set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "DR ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_shell ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "psi ="
				  set actualAnalytPar(FF,l8,label) "P2 ="
				  set actualAnalytPar(FF,l9,label) "P4 ="
				  set actualAnalytPar(FF,l10,label) "P6 ="
				}
	   "Disc"               { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "eta ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "TripleLayeredXS_ULV" \
				{ set actualAnalytPar(FF,l1,label) "nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_middle ="
				  set actualAnalytPar(FF,l4,label) "V_out ="
				  set actualAnalytPar(FF,l5,label) "eta_core ="
				  set actualAnalytPar(FF,l6,label) "eta_middle ="
				  set actualAnalytPar(FF,l7,label) "eta_out ="
				  set actualAnalytPar(FF,l8,label) "eta_solv ="
				  set actualAnalytPar(FF,l9,label) "xsolv_middle ="
				  set actualAnalytPar(FF,l10,label) "xsolv_out ="
				}
	   "homogenousXS"       { set actualAnalytPar(FF,l1,label) "L ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "eta ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "SphSh+SD+homoXS"    { set actualAnalytPar(FF,l1,label) "t ="
				  set actualAnalytPar(FF,l2,label) "sigma_t ="
				  set actualAnalytPar(FF,l3,label) "R0 ="
				  set actualAnalytPar(FF,l4,label) "sigma_R ="
				  set actualAnalytPar(FF,l5,label) "eta ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "EllSh+SD+homoXS"    { set actualAnalytPar(FF,l1,label) "t ="
				  set actualAnalytPar(FF,l2,label) "sigma_t ="
				  set actualAnalytPar(FF,l3,label) "R0 ="
				  set actualAnalytPar(FF,l4,label) "sigma_R ="
				  set actualAnalytPar(FF,l5,label) "epsilon ="
				  set actualAnalytPar(FF,l6,label) "eta ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "EllSh+SD+homoXS(S)" { set actualAnalytPar(FF,l1,label) "t ="
				  set actualAnalytPar(FF,l2,label) "sigma_t ="
				  set actualAnalytPar(FF,l3,label) "S0 ="
				  set actualAnalytPar(FF,l4,label) "s ="
				  set actualAnalytPar(FF,l5,label) "epsilon ="
				  set actualAnalytPar(FF,l6,label) "eta ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "CylSh+SD+homoXS"    { set actualAnalytPar(FF,l1,label) "t ="
				  set actualAnalytPar(FF,l2,label) "sigma_t ="
				  set actualAnalytPar(FF,l3,label) "R0 ="
				  set actualAnalytPar(FF,l4,label) "sigma_R ="
				  set actualAnalytPar(FF,l5,label) "L0 ="
				  set actualAnalytPar(FF,l6,label) "sigma_L ="
				  set actualAnalytPar(FF,l7,label) "eta ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Disc+homoXS"        { set actualAnalytPar(FF,l1,label) "t ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "eta ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"TwoInfinitelyThinPlates" \
				{ set actualAnalytPar(FF,l1,label) "L ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"LayeredCentroSymmetricXS" \
				{ set actualAnalytPar(FF,l1,label) "L_out ="
				  set actualAnalytPar(FF,l2,label) "L_core ="
				  set actualAnalytPar(FF,l3,label) "b_out ="
				  set actualAnalytPar(FF,l4,label) "b_core ="
				  set actualAnalytPar(FF,l5,label) "D ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"BiLayerGauss"          { set actualAnalytPar(FF,l1,label) "sigma_out ="
				  set actualAnalytPar(FF,l2,label) "b_out ="
				  set actualAnalytPar(FF,l3,label) "sigma_core ="
				  set actualAnalytPar(FF,l4,label) "b_core ="
				  set actualAnalytPar(FF,l5,label) "t ="
				  set actualAnalytPar(FF,l6,label) "D ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Torus"              { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "a ="
				  set actualAnalytPar(FF,l3,label) "Delta_a ="
				  set actualAnalytPar(FF,l4,label) "nu ="
				  set actualAnalytPar(FF,l5,label) "eta_c ="
				  set actualAnalytPar(FF,l6,label) "eta_sh ="
				  set actualAnalytPar(FF,l7,label) "eta_sol ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "OrnsteinZernike"    { set actualAnalytPar(FF,l1,label) "xi ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
           "BroadPeak"          { set actualAnalytPar(FF,l1,label) "I0 ="
                                  set actualAnalytPar(FF,l2,label) "xi ="
                                  set actualAnalytPar(FF,l3,label) "q0 ="
                                  set actualAnalytPar(FF,l4,label) "m ="
                                  set actualAnalytPar(FF,l5,label) "p ="
                                  set actualAnalytPar(FF,l6,label) ""
                                  set actualAnalytPar(FF,l7,label) ""
                                  set actualAnalytPar(FF,l8,label) ""
                                  set actualAnalytPar(FF,l9,label) ""
                                  set actualAnalytPar(FF,l10,label) ""
                                }
	   "TeubnerStrey"       { set actualAnalytPar(FF,l1,label) "xi ="
				  set actualAnalytPar(FF,l2,label) "d ="
				  set actualAnalytPar(FF,l3,label) "eta ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "DAB"                { set actualAnalytPar(FF,l1,label) "xi ="
				  set actualAnalytPar(FF,l2,label) "I0 ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Spinodal"           { set actualAnalytPar(FF,l1,label) "Qmax ="
				  set actualAnalytPar(FF,l2,label) "gamma ="
				  set actualAnalytPar(FF,l3,label) "Imax ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gauss"              { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gauss2"             { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "b_p ="
				  set actualAnalytPar(FF,l3,label) "V ="
				  set actualAnalytPar(FF,l4,label) "eta_s ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gauss3"             { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "b_p ="
				  set actualAnalytPar(FF,l3,label) "Mw ="
				  set actualAnalytPar(FF,l4,label) "rho_p ="
				  set actualAnalytPar(FF,l5,label) "eta_s ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "GaussPoly"          { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "Mw ="
				  set actualAnalytPar(FF,l3,label) "Mn ="
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"generalized Gaussian coil" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "nu ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"generalized Gaussian coil 2" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "nu ="
				  set actualAnalytPar(FF,l3,label) "b_p ="
				  set actualAnalytPar(FF,l4,label) "V ="
				  set actualAnalytPar(FF,l5,label) "eta_s ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"generalized Gaussian coil 3" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "nu ="
				  set actualAnalytPar(FF,l3,label) "b_p ="
				  set actualAnalytPar(FF,l4,label) "Mw ="
				  set actualAnalytPar(FF,l5,label) "rho_p ="
				  set actualAnalytPar(FF,l6,label) "eta_s ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"FlexibleRingPolymer"   { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) ""
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"mMemberedTwistedRing"  { set actualAnalytPar(FF,l1,label) "R_G,1r ="
				  set actualAnalytPar(FF,l2,label) "m ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	      "DaisyLikeRing"   { set actualAnalytPar(FF,l1,label) "R_G,1r ="
				  set actualAnalytPar(FF,l2,label) "m ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "BenoitStar"         { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "f ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "PolydisperseStar"   { set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "f ="
				  set actualAnalytPar(FF,l3,label) ""
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "JuelichMicelle"     { set actualAnalytPar(FF,l1,label) "mcore/rcore ="
				  set actualAnalytPar(FF,l2,label) "mbrsh/rbrsh ="
				  set actualAnalytPar(FF,l3,label) "bcore ="
				  set actualAnalytPar(FF,l4,label) "bbrush ="
				  set actualAnalytPar(FF,l5,label) "Nagg ="
				  set actualAnalytPar(FF,l6,label) "sigma1 ="
				  set actualAnalytPar(FF,l7,label) "sigma3 ="
				  set actualAnalytPar(FF,l8,label) "d3+ ="
				  set actualAnalytPar(FF,l9,label) "gamma ="
				  set actualAnalytPar(FF,l10,label) "rhosolv ="
				}
	   "Francois1"          { set actualAnalytPar(FF,l1,label) "Rc ="
				  set actualAnalytPar(FF,l2,label) "Rm ="
				  set actualAnalytPar(FF,l3,label) "nu ="
				  set actualAnalytPar(FF,l4,label) "L ="
				  set actualAnalytPar(FF,l5,label) "b ="
				  set actualAnalytPar(FF,l6,label) "Nagg ="
				  set actualAnalytPar(FF,l7,label) "Drho ="
				  set actualAnalytPar(FF,l8,label) "K ="
				  set actualAnalytPar(FF,l9,label) "exvol ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"CopolymerMicelleR^-a"  { set actualAnalytPar(FF,l1,label) "Rc ="
				  set actualAnalytPar(FF,l2,label) "Vc ="
				  set actualAnalytPar(FF,l3,label) "Vsh ="
				  set actualAnalytPar(FF,l4,label) "rho_c ="
				  set actualAnalytPar(FF,l5,label) "rho_sh ="
				  set actualAnalytPar(FF,l6,label) "rho_solv"
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	 "CopolymerMicelleRod"  { set actualAnalytPar(FF,l1,label) "Vpolym_core ="
				  set actualAnalytPar(FF,l2,label) "Vpolym_brsh ="
				  set actualAnalytPar(FF,l3,label) "eta_core ="
				  set actualAnalytPar(FF,l4,label) "eta_brsh ="
				  set actualAnalytPar(FF,l5,label) "eta_solv ="
				  set actualAnalytPar(FF,l6,label) "xsolv_core ="
				  set actualAnalytPar(FF,l7,label) "Rg ="
				  set actualAnalytPar(FF,l8,label) "d ="
				  set actualAnalytPar(FF,l9,label) "Nl ="
				  set actualAnalytPar(FF,l10,label) "L ="
				}
	 "CopolymerMicelleEll"  { set actualAnalytPar(FF,l1,label) "Vpolym_core ="
				  set actualAnalytPar(FF,l2,label) "Vpolym_brsh ="
				  set actualAnalytPar(FF,l3,label) "eta_core ="
				  set actualAnalytPar(FF,l4,label) "eta_brsh ="
				  set actualAnalytPar(FF,l5,label) "eta_solv ="
				  set actualAnalytPar(FF,l6,label) "xsolv_core ="
				  set actualAnalytPar(FF,l7,label) "Rg ="
				  set actualAnalytPar(FF,l8,label) "d ="
				  set actualAnalytPar(FF,l9,label) "Nagg ="
				  set actualAnalytPar(FF,l10,label) "epsilon ="
				}
	 "CopolymerMicelleCyl"  { set actualAnalytPar(FF,l1,label) "Vpolym_core ="
				  set actualAnalytPar(FF,l2,label) "Vpolym_brsh ="
				  set actualAnalytPar(FF,l3,label) "eta_core ="
				  set actualAnalytPar(FF,l4,label) "eta_brsh ="
				  set actualAnalytPar(FF,l5,label) "eta_solv ="
				  set actualAnalytPar(FF,l6,label) "xsolv_core ="
				  set actualAnalytPar(FF,l7,label) "Rg ="
				  set actualAnalytPar(FF,l8,label) "d ="
				  set actualAnalytPar(FF,l9,label) "Nagg ="
				  set actualAnalytPar(FF,l10,label) "L ="
				}
	   "DozierStar"         { set actualAnalytPar(FF,l1,label) "I0 ="
				  set actualAnalytPar(FF,l2,label) "Rg ="
				  set actualAnalytPar(FF,l3,label) "alpha ="
				  set actualAnalytPar(FF,l4,label) "xi ="
				  set actualAnalytPar(FF,l5,label) "nu ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "DozierStar2"        { set actualAnalytPar(FF,l1,label) "I0 ="
				  set actualAnalytPar(FF,l2,label) "Rg ="
				  set actualAnalytPar(FF,l3,label) "Nagg ="
				  set actualAnalytPar(FF,l4,label) "xi ="
				  set actualAnalytPar(FF,l5,label) "nu ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "BeacaugeExpPowLaw2" { set actualAnalytPar(FF,l1,label) "G ="
				  set actualAnalytPar(FF,l2,label) "B ="
				  set actualAnalytPar(FF,l3,label) "R_i ="
				  set actualAnalytPar(FF,l4,label) "R_i+1 ="
				  set actualAnalytPar(FF,l5,label) "k ="
				  set actualAnalytPar(FF,l6,label) "P ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	  "BeacaugeExpPowLaw"   { set actualAnalytPar(FF,l1,label) "G ="
				  set actualAnalytPar(FF,l2,label) "B ="
				  set actualAnalytPar(FF,l3,label) "Gs ="
				  set actualAnalytPar(FF,l4,label) "Bs ="
				  set actualAnalytPar(FF,l5,label) "Rg ="
				  set actualAnalytPar(FF,l6,label) "Rsub ="
				  set actualAnalytPar(FF,l7,label) "Rs ="
				  set actualAnalytPar(FF,l8,label) "P ="
				  set actualAnalytPar(FF,l9,label) "Ps ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Aggregate (Exp Cut-Off)"\
				{ set actualAnalytPar(FF,l1,label) "r0 ="
				  set actualAnalytPar(FF,l2,label) "xi ="
				  set actualAnalytPar(FF,l3,label) "D ="
				  set actualAnalytPar(FF,l4,label) "I0 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Aggregate (Exp(-x^a) Cut-Off)"\
				{ set actualAnalytPar(FF,l1,label) "xi ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Aggregate (OverlapSph Cut-Off)"\
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Fisher-Burford" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "RLCAggregation" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "DLCAggregation" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "MassFractExp" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "MassFractGauss" \
				{ set actualAnalytPar(FF,l1,label) "Rg ="
				  set actualAnalytPar(FF,l2,label) "D ="
				  set actualAnalytPar(FF,l3,label) "I0 ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LangevinMH"         { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "Ms ="
				  set actualAnalytPar(FF,l3,label) "T ="
				  set actualAnalytPar(FF,l4,label) "M8 ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "A(B)"               { set actualAnalytPar(FF,l1,label) "Knuc ="
				  set actualAnalytPar(FF,l2,label) "Kmag ="
				  set actualAnalytPar(FF,l3,label) {M\[Tesla\] =}
				  set actualAnalytPar(FF,l4,label) {R\[nm\] =}
				  set actualAnalytPar(FF,l5,label) {T\[K\] =}
				  set actualAnalytPar(FF,l6,label) {T_fluct\[K\] =}
				  set actualAnalytPar(FF,l7,label) {B_r\[Tesla\] =}
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "B(B)"               { set actualAnalytPar(FF,l1,label) ""
				  set actualAnalytPar(FF,l2,label) "Kmag ="
				  set actualAnalytPar(FF,l3,label) {M\[Tesla\] =}
				  set actualAnalytPar(FF,l4,label) {R\[nm\] =}
				  set actualAnalytPar(FF,l5,label) {T\[K\] =}
				  set actualAnalytPar(FF,l6,label) {T_fluct\[K\] =}
				  set actualAnalytPar(FF,l7,label) {B_r\[Tesla\] =}
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "P39"                { set actualAnalytPar(FF,l1,label) "Nc ="
				  set actualAnalytPar(FF,l2,label) "Rg"
				  set actualAnalytPar(FF,l3,label) "Rc ="
				  set actualAnalytPar(FF,l4,label) "rhos ="
				  set actualAnalytPar(FF,l5,label) "rhoc ="
				  set actualAnalytPar(FF,l6,label) "L ="
				  set actualAnalytPar(FF,l7,label) "b ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "SuperParStroboPsi"  { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "R ="
				  set actualAnalytPar(FF,l4,label) "R_av ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	   "SuperParStroboPsi2" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "Dl_l ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboTISANE1" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "Dt ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	  "SuperParStroboPsiSQ" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQBt1" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQLx" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQL2x" \
				{ set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQL2x_SANSPOL" \
				{ set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "pol ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQL2x_SANSPOL_albr" \
				{ set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "pol ="
				  set actualAnalytPar(FF,l9,label) "d_phi ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQL2x_SANSPOL_albr_stat" \
				{ set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "pol ="
				  set actualAnalytPar(FF,l9,label) "d_phi ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	"SuperParStroboPsiSQL2modx_SANSPOL" \
				{ set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "SQ ="
				  set actualAnalytPar(FF,l4,label) "B1 ="
				  set actualAnalytPar(FF,l5,label) "mu_kT ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "pol ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	  "hysteresesStroboPsi" { set actualAnalytPar(FF,l1,label) "phi0 ="
				  set actualAnalytPar(FF,l2,label) "Knuc_Kmag ="
				  set actualAnalytPar(FF,l3,label) "Dl_l ="
				  set actualAnalytPar(FF,l4,label) "Bc ="
				  set actualAnalytPar(FF,l5,label) "S ="
				  set actualAnalytPar(FF,l6,label) "f ="
				  set actualAnalytPar(FF,l7,label) "B0 ="
				  set actualAnalytPar(FF,l8,label) "Lambda ="
				  set actualAnalytPar(FF,l9,label) "SD ="
				  set actualAnalytPar(FF,l10,label) "psi ="
				}
	   "MieSphere"          { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "lam_sol ="
				  set actualAnalytPar(FF,l3,label) "m_re ="
				  set actualAnalytPar(FF,l4,label) "m_im ="
				  set actualAnalytPar(FF,l5,label) "pol ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "MieShell"           { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "lam_sol ="
				  set actualAnalytPar(FF,l4,label) "mc_re ="
				  set actualAnalytPar(FF,l5,label) "mc_im ="
				  set actualAnalytPar(FF,l6,label) "ms_re ="
				  set actualAnalytPar(FF,l7,label) "ms_im ="
				  set actualAnalytPar(FF,l8,label) "pol ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Beta (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "xmin ="
				  set actualAnalytPar(FF,l3,label) "xmax ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) "beta ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Beta (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "xmin ="
				  set actualAnalytPar(FF,l3,label) "xmax ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) "beta ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Chi-squared (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Chi-squared (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Erfc (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Erfc (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Error (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Error (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "exponentially modified Gaussian (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "distortion ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "exponentially modified Gaussian (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "distortion ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Extreme Value (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Extreme Value (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "fatique life (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "beta ="
				  set actualAnalytPar(FF,l4,label) "gamma ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "F-variance (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape1 ="
				  set actualAnalytPar(FF,l5,label) "shape2 ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "F-variance (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape1 ="
				  set actualAnalytPar(FF,l5,label) "shape2 ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gamma (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gamma (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian-Lorentzian cross product (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian-Lorentzian cross product (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian-Lorentzian sum (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Gaussian-Lorentzian sum (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "generalized Gaussian 1 (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "generalized Gaussian 1 (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "generalized Gaussian 2 (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "generalized Gaussian 2 (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Giddings (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Giddings (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Haarhoff Van der Linde (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "distortion ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
 "half Gaussian modified Gaussian (Area)" \
	                        { set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
                                  set actualAnalytPar(FF,l4,label) "distortion ="
                                  set actualAnalytPar(FF,l5,label) "backgr. ="
                                  set actualAnalytPar(FF,l6,label) ""
                                  set actualAnalytPar(FF,l7,label) ""
                                  set actualAnalytPar(FF,l8,label) ""
                                  set actualAnalytPar(FF,l9,label) ""
                                  set actualAnalytPar(FF,l10,label) ""
                                 }
	   "Inverted Gamma (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Inverted Gamma (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Kumaraswamy (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "xmin ="
				  set actualAnalytPar(FF,l3,label) "xmax ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) "beta ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Kumaraswamy (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "xmin ="
				  set actualAnalytPar(FF,l3,label) "xmax ="
				  set actualAnalytPar(FF,l4,label) "alpha ="
				  set actualAnalytPar(FF,l5,label) "beta ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Laplace (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Laplace (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Logistic (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "x0 ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Logistic (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "x0 ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogLogistic (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "x0 ="
				  set actualAnalytPar(FF,l4,label) "mu ="
				  set actualAnalytPar(FF,l5,label) "sigma ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogLogistic (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "x0 ="
				  set actualAnalytPar(FF,l3,label) "mu ="
				  set actualAnalytPar(FF,l4,label) "sigma ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogNormal, 4 parameters (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogNormal, 4 parameters (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogNormal (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "mu ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "x0 ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "LogNormal (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "mu ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "x0 ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Lorentzian (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Lorentzian (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Pearson IV (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape1 ="
				  set actualAnalytPar(FF,l5,label) "shape2 ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Pearson IV (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape1 ="
				  set actualAnalytPar(FF,l5,label) "shape2 ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Pearson VII (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Pearson VII (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "power LogNormal (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "p ="
				  set actualAnalytPar(FF,l6,label) "backgr. ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "power Normal (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "location ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "p ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "backgr. ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse with 2nd width (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width1 ="
				  set actualAnalytPar(FF,l4,label) "width2 ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse with 2nd width (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width1 ="
				  set actualAnalytPar(FF,l4,label) "width2 ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse with power term (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "pulse with power term (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Student-t (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Student-t (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Voigt (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "gamma ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Voigt (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "gamma ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Weibull (Amplitude)"   { \
				  set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Weibull (Area)"   { \
				  set actualAnalytPar(FF,l1,label) "area ="
				  set actualAnalytPar(FF,l2,label) "center ="
				  set actualAnalytPar(FF,l3,label) "width ="
				  set actualAnalytPar(FF,l4,label) "shape ="
				  set actualAnalytPar(FF,l5,label) "backgr. ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"confinement with Gaussian potential" \
				{ set actualAnalytPar(FF,l1,label) "amplitude ="
				  set actualAnalytPar(FF,l2,label) {Q \[Ångstrøm\] =}
				  set actualAnalytPar(FF,l3,label) "Emean ="
				  set actualAnalytPar(FF,l4,label) {D \[m^2/s\] =}
				  set actualAnalytPar(FF,l5,label) {<u^2> \[m^2\] =}
				  set actualAnalytPar(FF,l6,label) "sigma_0 ="
				  set actualAnalytPar(FF,l7,label) "sigma_1 ="
				  set actualAnalytPar(FF,l8,label) "sigma_2 ="
				  set actualAnalytPar(FF,l9,label) "background ="
				  set actualAnalytPar(FF,l10,label) "N ="
				}
	   "DLS_Sphere_RDG"     { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "eta ="
				  set actualAnalytPar(FF,l3,label) "T ="
				  set actualAnalytPar(FF,l4,label) "Q ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Robertus2"          { set actualAnalytPar(FF,l1,label) "Rmean ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "tau ="
				  set actualAnalytPar(FF,l5,label) "xfp2 ="
				  set actualAnalytPar(FF,l6,label) "fp ="
				  set actualAnalytPar(FF,l7,label) "eta_core ="
				  set actualAnalytPar(FF,l8,label) "eta_shell ="
				  set actualAnalytPar(FF,l9,label) "x_solvent ="
				  set actualAnalytPar(FF,l10,label) "eta_solvent ="
				}
	   "Robertus1"          { set actualAnalytPar(FF,l1,label) "Rmean ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "sigma ="
				  set actualAnalytPar(FF,l4,label) "btau ="
				  set actualAnalytPar(FF,l5,label) "atau ="
				  set actualAnalytPar(FF,l6,label) "fp ="
				  set actualAnalytPar(FF,l7,label) "eta_core ="
				  set actualAnalytPar(FF,l8,label) "eta_shell ="
				  set actualAnalytPar(FF,l9,label) "x_solvent ="
				  set actualAnalytPar(FF,l10,label) "eta_solvent ="
				}
	   "WormLikeChainEXV"   { set actualAnalytPar(FF,l1,label) "S0 ="
				  set actualAnalytPar(FF,l2,label) "RL ="
				  set actualAnalytPar(FF,l3,label) "RRL ="
				  set actualAnalytPar(FF,l4,label) "R ="
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "KholodenkoWorm"     { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "l ="
				  set actualAnalytPar(FF,l3,label) "L ="
				  set actualAnalytPar(FF,l4,label) ""
				  set actualAnalytPar(FF,l5,label) ""
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"SphereWithGaussChains" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "Rg ="
				  set actualAnalytPar(FF,l3,label) "d ="
				  set actualAnalytPar(FF,l4,label) "Nagg ="
				  set actualAnalytPar(FF,l5,label) "rc ="
				  set actualAnalytPar(FF,l6,label) "rs ="
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	"BlockCopolymerMicelle" { set actualAnalytPar(FF,l1,label) "Vpolym,c ="
				  set actualAnalytPar(FF,l2,label) "xsolv_c ="
				  set actualAnalytPar(FF,l3,label) "Vpoly,sh ="
				  set actualAnalytPar(FF,l4,label) "eta_polym,c ="
				  set actualAnalytPar(FF,l5,label) "eta_polym,sh ="
				  set actualAnalytPar(FF,l6,label) "eta_sol ="
				  set actualAnalytPar(FF,l7,label) "Nagg ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"SPHERE+Chains(SAW)"    { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+Chains(SAW)_Rc" {
				  set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_sol ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+Chains(SAW)_Nagg" {
				  set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_sol ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+Chains(RW)"     { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"SPHERE+Chains(RW)_Rc"  { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"SPHERE+Chains(RW)_Nagg" \
				{ set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) ""
				}
	"DISC+Chains(RW)"     \
				{ set actualAnalytPar(FF,l1,label) "L_core ="
				  set actualAnalytPar(FF,l2,label) "nagg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "D ="
				}
	"DISC+Chains(RW)_Lc"  \
				{ set actualAnalytPar(FF,l1,label) "L_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "D ="
				}
	"DISC+Chains(RW)_nagg" \
				{ set actualAnalytPar(FF,l1,label) "nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "D ="
				}
	"ELL+Chains(RW)"        { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "epsilon ="
				}
	"ELL+Chains(RW)_Rc"     { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "epsilon ="
				}
	"ELL+Chains(RW)_Nagg" \
				{ set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "epsilon ="
				}
	"CYL+Chains(RW)"        { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"CYL+Chains(RW)_Rc"     { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"CYL+Chains(RW)_Nagg" \
				{ set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Chains(RW)"        { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Chains(RW)_Rc"     { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Chains(RW)_nagg" \
				{ set actualAnalytPar(FF,l1,label) "n_agg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "d ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"WORM+Chains(RW)"       { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "l ="
				  set actualAnalytPar(FF,l10,label) "L ="
				}
	"WORM+Chains(RW)_Rc"    { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "l ="
				  set actualAnalytPar(FF,l10,label) "L ="
				}
	"WORM+Chains(RW)_nagg" \
				{ set actualAnalytPar(FF,l1,label) "n_agg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "Rg ="
				  set actualAnalytPar(FF,l9,label) "l ="
				  set actualAnalytPar(FF,l10,label) "L ="
				}
	"SPHERE+Exp_Rc"         { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "x_solv_c ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "t ="
				  set actualAnalytPar(FF,l10,label) "x_out ="
				}
	"SPHERE+R^-a"           { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+R^-a_Rc"        { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE_smooth_interface+R^-a_Rc" { 
				  set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "Deta_core ="
				  set actualAnalytPar(FF,l5,label) "Deta_brush ="
				  set actualAnalytPar(FF,l6,label) "sigma ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+R^-a_Manuela"   { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "D_eta_core ="
				  set actualAnalytPar(FF,l5,label) "D_eta_brush ="
				  set actualAnalytPar(FF,l6,label) "r_SiOx / mol. ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "a ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+R^-a_Manuela2"  { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "D_eta_core ="
				  set actualAnalytPar(FF,l5,label) "D_eta_brush ="
				  set actualAnalytPar(FF,l6,label) "r_SiOx / mol. ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "a ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"SPHERE+R^-a_Nagg"      { set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	"ROD+R^-a"              { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "t ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+R^-a_Rc"           { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "t ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+R^-a_nagg"         { set actualAnalytPar(FF,l1,label) "n_agg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "alpha ="
				  set actualAnalytPar(FF,l9,label) "t ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Gauss"             { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "sigma ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Gauss_Rc"          { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "sigma ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Gauss_nagg"        { set actualAnalytPar(FF,l1,label) "n_agg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "xsolv_core ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "sigma ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Exp"               { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "n_agg ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "ro ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "alpha ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Exp_Rc"            { set actualAnalytPar(FF,l1,label) "R_core ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "ro ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "alpha ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"ROD+Exp_nagg"          { set actualAnalytPar(FF,l1,label) "n_agg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "eta_core ="
				  set actualAnalytPar(FF,l5,label) "eta_brush ="
				  set actualAnalytPar(FF,l6,label) "eta_solv ="
				  set actualAnalytPar(FF,l7,label) "ro ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "alpha ="
				  set actualAnalytPar(FF,l10,label) "H ="
				}
	"SPHERE_smooth_interface+R^-a_Nagg" { 
				  set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "V_core ="
				  set actualAnalytPar(FF,l3,label) "V_brush ="
				  set actualAnalytPar(FF,l4,label) "Deta_core ="
				  set actualAnalytPar(FF,l5,label) "Deta_brush ="
				  set actualAnalytPar(FF,l6,label) "sigma ="
				  set actualAnalytPar(FF,l7,label) "alpha ="
				  set actualAnalytPar(FF,l8,label) "t ="
				  set actualAnalytPar(FF,l9,label) "L ="
				  set actualAnalytPar(FF,l10,label) "b ="
				}
	   "ShearedCylinder"    { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) "Gamma ="
				  set actualAnalytPar(FF,l4,label) "psi ="
				  set actualAnalytPar(FF,l5,label) "eta ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "ShearedCylinder"    { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) "Gamma ="
				  set actualAnalytPar(FF,l4,label) "psi ="
				  set actualAnalytPar(FF,l5,label) "eta ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "ShearedCylGauss"    { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "L ="
				  set actualAnalytPar(FF,l3,label) "theta_b ="
				  set actualAnalytPar(FF,l4,label) "psi ="
				  set actualAnalytPar(FF,l5,label) "eta ="
				  set actualAnalytPar(FF,l6,label) ""
				  set actualAnalytPar(FF,l7,label) ""
				  set actualAnalytPar(FF,l8,label) ""
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Andrea1"            { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "dR ="
				  set actualAnalytPar(FF,l3,label) "c ="
				  set actualAnalytPar(FF,l4,label) "core =" 
				  set actualAnalytPar(FF,l5,label) "dR2 ="
				  set actualAnalytPar(FF,l6,label) "shell1 ="
				  set actualAnalytPar(FF,l7,label) "shell2 ="
				  set actualAnalytPar(FF,l8,label) "c2 ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "Simon1"             { set actualAnalytPar(FF,l1,label) "Nagg ="
				  set actualAnalytPar(FF,l2,label) "Rg ="
				  set actualAnalytPar(FF,l3,label) "d ="
				  set actualAnalytPar(FF,l4,label) "rho_solv =" 
				  set actualAnalytPar(FF,l5,label) "rho_core ="
				  set actualAnalytPar(FF,l6,label) "b_A_h ="
				  set actualAnalytPar(FF,l7,label) "V_A_h ="
				  set actualAnalytPar(FF,l8,label) "b_A_l ="
				  set actualAnalytPar(FF,l9,label) "V_A_l"
				  set actualAnalytPar(FF,l10,label) "rho_B"
				}
	   "DoubleShell_withSD" { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "s ="
				  set actualAnalytPar(FF,l3,label) "eta_c ="
				  set actualAnalytPar(FF,l4,label) "eta_1 =" 
				  set actualAnalytPar(FF,l5,label) "eta_2 ="
				  set actualAnalytPar(FF,l6,label) "eta_m ="
				  set actualAnalytPar(FF,l7,label) "d_1 ="
				  set actualAnalytPar(FF,l8,label) "d_2 ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   "StackDiscs"         { set actualAnalytPar(FF,l1,label) "R ="
				  set actualAnalytPar(FF,l2,label) "d ="
				  set actualAnalytPar(FF,l3,label) "h ="
				  set actualAnalytPar(FF,l4,label) "n =" 
				  set actualAnalytPar(FF,l5,label) "eta_c ="
				  set actualAnalytPar(FF,l6,label) "eta_l ="
				  set actualAnalytPar(FF,l7,label) "sigma ="
				  set actualAnalytPar(FF,l8,label) "D ="
				  set actualAnalytPar(FF,l9,label) ""
				  set actualAnalytPar(FF,l10,label) ""
				}
	   default              { ap_set_param_labels actualAnalytPar FF }
		} }
		SQ { switch $actualAnalytPar(SQ,typestr) {
	   "None"               { set actualAnalytPar(SQ,s1,label) ""
				  set actualAnalytPar(SQ,s2,label) ""
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Hard Sphere"        { set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "fp ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Sticky Hard Sphere" {
				  set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "tau ="
				  set actualAnalytPar(SQ,s3,label) "fp ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	 "Sticky Hard Sphere 2" {
				  set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "tau ="
				  set actualAnalytPar(SQ,s3,label) "Delta ="
				  set actualAnalytPar(SQ,s4,label) "fp ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	 "Square Well Potential" {
				  set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "eta ="
				  set actualAnalytPar(SQ,s3,label) "epsi/kBT ="
				  set actualAnalytPar(SQ,s4,label) "lambda ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
    "Thin Square Well Potential" {
				  set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "phi ="
				  set actualAnalytPar(SQ,s3,label) "U/kT ="
				  set actualAnalytPar(SQ,s4,label) "epsilon ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	 "Square Well Potential 2" {
				  set actualAnalytPar(SQ,s1,label) "RHS ="
				  set actualAnalytPar(SQ,s2,label) "eta ="
				  set actualAnalytPar(SQ,s3,label) "epsi/kB ="
				  set actualAnalytPar(SQ,s4,label) "Delta ="
				  set actualAnalytPar(SQ,s5,label) "T ="
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Mass Fractal (Exp Cut-Off)"\
				{ set actualAnalytPar(SQ,s1,label) "r0 ="
				  set actualAnalytPar(SQ,s2,label) "xi ="
				  set actualAnalytPar(SQ,s3,label) "D ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Mass Fractal (Exp(-x^a) Cut-Off)"\
				{ set actualAnalytPar(SQ,s1,label) "r0 ="
				  set actualAnalytPar(SQ,s2,label) "xi ="
				  set actualAnalytPar(SQ,s3,label) "D ="
				  set actualAnalytPar(SQ,s4,label) "alpha ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Mass Fractal (Gaussian Cut-Off)" \
				{ set actualAnalytPar(SQ,s1,label) "r0 ="
				  set actualAnalytPar(SQ,s2,label) "xi ="
				  set actualAnalytPar(SQ,s3,label) "D ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Mass Fractal (OverlapSph Cut-Off)" \
				{ set actualAnalytPar(SQ,s1,label) "r0 ="
				  set actualAnalytPar(SQ,s2,label) "xi ="
				  set actualAnalytPar(SQ,s3,label) "D ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "Cylinder(PRISM)"    { set actualAnalytPar(SQ,s1,label) "R ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) "nu ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "RPA"                { set actualAnalytPar(SQ,s1,label) "nu ="
				  set actualAnalytPar(SQ,s2,label) ""
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   "ThermalDisorder"    { set actualAnalytPar(SQ,s1,label) "N ="
				  set actualAnalytPar(SQ,s2,label) "d ="
				  set actualAnalytPar(SQ,s3,label) "Delta ="
				  set actualAnalytPar(SQ,s4,label) "Nu ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	      "Paracrystalline" { set actualAnalytPar(SQ,s1,label) "N ="
				  set actualAnalytPar(SQ,s2,label) "d ="
				  set actualAnalytPar(SQ,s3,label) "Delta ="
				  set actualAnalytPar(SQ,s4,label) "Nu ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	      "ModifiedCaille"  { set actualAnalytPar(SQ,s1,label) "N ="
				  set actualAnalytPar(SQ,s2,label) "d ="
				  set actualAnalytPar(SQ,s3,label) "eta ="
				  set actualAnalytPar(SQ,s4,label) "Nu ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"GaussianAmplitude"     { set actualAnalytPar(SQ,s1,label) "Ampl. ="
				  set actualAnalytPar(SQ,s2,label) "mean ="
				  set actualAnalytPar(SQ,s3,label) "width ="
                                  set actualAnalytPar(SQ,s4,label) "bckgr ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"LorentzianAmplitude"    {set actualAnalytPar(SQ,s1,label) "Ampl. ="
				  set actualAnalytPar(SQ,s2,label) "mean ="
				  set actualAnalytPar(SQ,s3,label) "width =" 
				  set actualAnalytPar(SQ,s4,label) "bckgr ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	      "VoigtPeak"       { set actualAnalytPar(SQ,s1,label) "A ="
				  set actualAnalytPar(SQ,s2,label) "mean ="
				  set actualAnalytPar(SQ,s3,label) "sigma ="
				  set actualAnalytPar(SQ,s4,label) "gamma ="
				  set actualAnalytPar(SQ,s5,label) "c0 ="
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	 "PearsonVIIAmplitude"   { set actualAnalytPar(SQ,s1,label) "Ampl. ="
				  set actualAnalytPar(SQ,s2,label) "mean ="
				  set actualAnalytPar(SQ,s3,label) "width1 ="
				  set actualAnalytPar(SQ,s4,label) "width2 =" 
				  set actualAnalytPar(SQ,s5,label) "bckgr ="
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	    "Correlation Hole"  { set actualAnalytPar(SQ,s1,label) "h ="
				  set actualAnalytPar(SQ,s2,label) "eta ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "Critical Scattering" { set actualAnalytPar(SQ,s1,label) "kappa ="
				  set actualAnalytPar(SQ,s2,label) "zeta ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "Macro Ion (HP)"      { set actualAnalytPar(SQ,s1,label) {T \[°C\] =}
				  set actualAnalytPar(SQ,s2,label) "Z_eff ="
				  set actualAnalytPar(SQ,s3,label) "RHS ="
				  set actualAnalytPar(SQ,s4,label) "ION ="
				  set actualAnalytPar(SQ,s5,label) "ETA ="
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "Hayter Penfold RMSA" { set actualAnalytPar(SQ,s1,label) {RHS \[nm\] =}
				  set actualAnalytPar(SQ,s2,label) "Z ="
				  set actualAnalytPar(SQ,s3,label) "eta ="
				  set actualAnalytPar(SQ,s4,label) {T \[K\] =}
				  set actualAnalytPar(SQ,s5,label) {salt \[M\] =}
				  set actualAnalytPar(SQ,s6,label) "eps_r ="
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "BabaAhmed" \
				{ set actualAnalytPar(SQ,s1,label) {Deff \[nm\] =}
				  set actualAnalytPar(SQ,s2,label) {Psi,0,eff \[Nm/C\] =}
				  set actualAnalytPar(SQ,s3,label) {epsilon \[C^2/(Nm^2)\] =}
				  set actualAnalytPar(SQ,s4,label) "eta ="
				  set actualAnalytPar(SQ,s5,label) {kD \[nm^-1\] =}
				  set actualAnalytPar(SQ,s6,label) {T \[K\] =}
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "BabaAhmed2" \
				{ set actualAnalytPar(SQ,s1,label) {Reff \[nm\] =}
				  set actualAnalytPar(SQ,s2,label) "z_p ="
				  set actualAnalytPar(SQ,s3,label) "epsilon_r ="
				  set actualAnalytPar(SQ,s4,label) {mB \[mol/l\] =}
				  set actualAnalytPar(SQ,s5,label) "zB ="
				  set actualAnalytPar(SQ,s6,label) "eta ="
				  set actualAnalytPar(SQ,s7,label) {T \[K\] =}
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "LocalOrder"          { set actualAnalytPar(SQ,s1,label) "D ="
				  set actualAnalytPar(SQ,s2,label) "alpha ="
				  set actualAnalytPar(SQ,s3,label) "z ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	  "RandomDistribution"  { set actualAnalytPar(SQ,s1,label) "Rca ="
				  set actualAnalytPar(SQ,s2,label) "R ="
				  set actualAnalytPar(SQ,s3,label) "fp ="
				  set actualAnalytPar(SQ,s4,label) "epsilon ="
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
           "P'(Q):ThinDisc"     { set actualAnalytPar(SQ,s1,label) "R ="
                                  set actualAnalytPar(SQ,s2,label) ""
                                  set actualAnalytPar(SQ,s3,label) "sigma ="
                                  set actualAnalytPar(SQ,s4,label) ""
                                  set actualAnalytPar(SQ,s5,label) ""
                                  set actualAnalytPar(SQ,s6,label) ""
                                  set actualAnalytPar(SQ,s7,label) ""
                                  set actualAnalytPar(SQ,s8,label) ""
                                  set actualAnalytPar(SQ,s9,label) ""
                                  set actualAnalytPar(SQ,s10,label) ""
                                }
    "P'(Q):ThinSphericalShell" \
                                { set actualAnalytPar(SQ,s1,label) "R ="
                                  set actualAnalytPar(SQ,s2,label) ""
                                  set actualAnalytPar(SQ,s3,label) "sigma ="
                                  set actualAnalytPar(SQ,s4,label) ""
                                  set actualAnalytPar(SQ,s5,label) ""
                                  set actualAnalytPar(SQ,s6,label) ""
                                  set actualAnalytPar(SQ,s7,label) ""
                                  set actualAnalytPar(SQ,s8,label) ""
                                  set actualAnalytPar(SQ,s9,label) ""
                                  set actualAnalytPar(SQ,s10,label) ""
                                }
    "P'(Q):ThinEllipsoidalShell" \
                                { set actualAnalytPar(SQ,s1,label) "a ="
                                  set actualAnalytPar(SQ,s2,label) "b ="
                                  set actualAnalytPar(SQ,s3,label) "sigma ="
                                  set actualAnalytPar(SQ,s4,label) ""
                                  set actualAnalytPar(SQ,s5,label) ""
                                  set actualAnalytPar(SQ,s6,label) ""
                                  set actualAnalytPar(SQ,s7,label) ""
                                  set actualAnalytPar(SQ,s8,label) ""
                                  set actualAnalytPar(SQ,s9,label) ""
                                  set actualAnalytPar(SQ,s10,label) ""
                                }
     "P'(Q):ThinHollowCylinder" \
                                 { set actualAnalytPar(SQ,s1,label) "R ="
                                   set actualAnalytPar(SQ,s2,label) "L ="
                                   set actualAnalytPar(SQ,s3,label) "sigma ="
                                   set actualAnalytPar(SQ,s4,label) ""
                                   set actualAnalytPar(SQ,s5,label) ""
                                   set actualAnalytPar(SQ,s6,label) ""
                                   set actualAnalytPar(SQ,s7,label) ""
                                   set actualAnalytPar(SQ,s8,label) ""
                                   set actualAnalytPar(SQ,s9,label) ""
                                   set actualAnalytPar(SQ,s10,label) ""
                                }
	  "P'(Q):Rod"           { set actualAnalytPar(SQ,s1,label) "L ="
				  set actualAnalytPar(SQ,s2,label) ""
				  set actualAnalytPar(SQ,s3,label) "sigma ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):Worm(PS1)" \
                                { set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) "exvol ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):Worm(PS2)" \
                                { set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):Worm(PS3)" \
                                { set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) "exvol ="
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):WormLikeChain_withEXV" \
                                { set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):WormLikeChain_withoutEXV" \
                               {  set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	"P'(Q):KholodenkoWorm"  { set actualAnalytPar(SQ,s1,label) "lb ="
				  set actualAnalytPar(SQ,s2,label) "L ="
				  set actualAnalytPar(SQ,s3,label) ""
				  set actualAnalytPar(SQ,s4,label) ""
				  set actualAnalytPar(SQ,s5,label) ""
				  set actualAnalytPar(SQ,s6,label) ""
				  set actualAnalytPar(SQ,s7,label) ""
				  set actualAnalytPar(SQ,s8,label) ""
				  set actualAnalytPar(SQ,s9,label) ""
				  set actualAnalytPar(SQ,s10,label) ""
				}
	   default              { ap_set_param_labels actualAnalytPar SQ }
		} }
	}
	ap_update_param_count $AactualAnalytPar $type
}

proc set_actualAP_config {type AactualAnalytPar globalfit
} {
	upvar $AactualAnalytPar actualAnalytPar
	global distr

	set p [get_old_param_name $type]
	set ltype [string tolower $type]
	set path ".analytical.fitpar.$ltype.lay"
	set path_dropdownl "$path.wrl.which$ltype"
	set path_rangebutton "$path.wrl.rangebutton"
	set with_distr 0
	if {[string equal -length 2 $type FF]} {set with_distr 1}

	# set entries which decide the parameter of distribution
	if {[string equal -length 2 $type FF]} {
		set distr 0
		for {set j 1} {$j <= $actualAnalytPar($type,param_count)} {incr j} {
			if { $actualAnalytPar($type,$p$j,distr) == 1 } {
				set distr $j
				set actualAnalytPar($type,$p$j,active) 0
			}
		}
	}
	# set 'fit' checkbox values
	if {[string equal $actualAnalytPar(fitSDFF) yes]} {
		for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i} {
			eval [format %s%s $path.$p$indx_i fit ] configure -state normal
			eval [format %s%s $path.$p$indx_i fit ] configure $::radio_check_button_prop
		}
	}

	if {[string equal $actualAnalytPar(calcSDFF) yes]
	} {
		if {$globalfit} {enable_disable_Common actualAnalytPar}
		$path_rangebutton configure -state normal
		$path_dropdownl configure -state normal

		for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
		} {
			param_enable $path $p $indx_i $globalfit $with_distr
		}
	}

	# Check if actualAnalytPar(SD,typestr) is equal "Monodisperse".
	# In this case all buttons and entries for SD are disabled

	if {[string equal -length 2 $type SD]
	} {
		$path_dropdownl configure -fg Black
		$path_dropdownl configure -state normal

		if {[string equal $actualAnalytPar($type,typestr) "Monodisperse"] ||
		    [string equal $actualAnalytPar($type,typestr) "Delta"] ||
		    [string equal $actualAnalytPar($type,typestr) "A_div_B"]
		} {
			$path_rangebutton configure -state disabled
			# forces the usage of a specific size distribution for a given form factor
			if { [string equal $actualAnalytPar(FF,typestr) Background] ||
			     [string equal $actualAnalytPar(FF,typestr) Guinier]
			} {
				$path_dropdownl configure -fg #a3a3a3
				$path_dropdownl configure -state disabled
			} else {
				distr_disable distr actualAnalytPar FF .analytical.fitpar.ff.lay l $globalfit
			}
			for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
			} {
	      			param_disable $path $p $indx_i $globalfit $with_distr
#					set actualAnalytPar($type,$p$indx_i,active) 0
		   	}
			if { [string equal $actualAnalytPar($type,typestr) "Delta"] } {
				$path_rangebutton configure -state normal
#				set actualAnalytPar($type,${p}1,active) 1
	      			param_enable $path $p 1 $globalfit $with_distr
			}
			if { [string equal $actualAnalytPar($type,typestr) "A_div_B"] } {
				$path_rangebutton configure -state normal
	      			param_enable $path $p 1 $globalfit $with_distr
	      			param_enable $path $p 2 $globalfit $with_distr
			}
			set distr 0
		} else {
			for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i} {
				if { [string length $actualAnalytPar($type,$p$indx_i,label)] != 0 
				} {
					param_enable $path $p $indx_i $globalfit $with_distr
				} else {
					param_disable $path $p $indx_i $globalfit $with_distr
					set actualAnalytPar($type,$p$indx_i,active) 0
					if {$globalfit} {
						set actualAnalytPar($type,$p$indx_i,common) NONE
					}
				}
			}
			distr_enable distr actualAnalytPar FF .analytical.fitpar.ff.lay l $globalfit
		}
	}
	if {[string equal -length 2 $type FF]
	} {
		for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
		} {
			if { [string length $actualAnalytPar($type,$p$indx_i,label)] != 0 
			} {
				param_enable $path $p $indx_i $globalfit $with_distr
			} else {
				param_disable $path $p $indx_i $globalfit $with_distr
				if {$globalfit} {
					set actualAnalytPar($type,$p$indx_i,common) NONE
				}
				set actualAnalytPar($type,$p$indx_i,active) 0
				# for dummy labels
				if {$distr == $indx_i} {
					set distr 1
					set actualAnalytPar($type,${p}1,distr) 1
					set actualAnalytPar($type,${p}$indx_i,distr) 0
				}
			}
			if {$actualAnalytPar($type,$p$indx_i,distr) == 1} { set distr $indx_i }
		}
		if {[string equal $actualAnalytPar(SD,typestr) "Monodisperse"] ||
		    [string equal $actualAnalytPar(SD,typestr) "Delta"] ||
		    [string equal $actualAnalytPar(SD,typestr) "A_div_B"]
		} {
			distr_disable distr actualAnalytPar $type $path $p $globalfit
			if { ![string equal $actualAnalytPar($type,typestr) Background] &&
			     ![string equal $actualAnalytPar($type,typestr) Guinier]
			} {
				.analytical.fitpar.sd.lay.wrl.whichsd configure -fg Black 
				.analytical.fitpar.sd.lay.wrl.whichsd configure -state normal
			}
		} else {
			distr_enable distr actualAnalytPar $type $path $p $globalfit
		}
	}
	if {[string compare $actualAnalytPar(fitSDFF) no] == 0} {
		for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i} {
			[format %s%s $path.$p$indx_i fit ] configure -state disabled
			[format %s%s $path.$p$indx_i fit ] configure -selectcolor gray
		}
	}

	# Check if actualAnalytPar(SQ,typestr) is equal "None".
	# In this case all buttons and entries for SQ are disabled

	if {[string equal $type SQ]} {
		if {[string equal $actualAnalytPar($type,typestr) "None"]
		} {
			$path_rangebutton configure -state disabled
			for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
			} {
				param_disable $path $p $indx_i $globalfit $with_distr
			}
		} else { 
			$path_rangebutton configure -state normal
			for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
			} {
				if { [string length $actualAnalytPar($type,$p$indx_i,label)] != 0 } {
					param_enable $path $p $indx_i $globalfit $with_distr
				} else {
					param_disable $path $p $indx_i $globalfit $with_distr
					set actualAnalytPar($type,$p$indx_i,active) 0
					if {$globalfit} {
						set actualAnalytPar($type,$p$indx_i,common) NONE
					}
				}
			}
		}
	}
	if {[string equal $type SQ] && $actualAnalytPar($type,how) >= 2
	} {
		# Check if actualAnalytPar(SQ,how) is >= 2 
		# (local monodisperse approach, partial structure factors, scaling approach)
		# In this case first button and entry for SQ is disabled as it is the integration variable

		param_disable $path $p 1 $globalfit $with_distr
		set actualAnalytPar($type,${p}1,active) 0
	}
	if {[string equal $actualAnalytPar(calcSDFF) no]
	} {
		$path_rangebutton configure -state disabled
		$path_dropdownl configure -state disabled
	
		for {set indx_i 1} {$indx_i <= $actualAnalytPar($type,param_count) } {incr indx_i
		} {
			param_disable $path $p $indx_i $globalfit $with_distr
		}
	}
	# remove obsolete fields from previously selected model functions
	# avoids fit-selection of non-existent parameters ... (singular-matrix error)
	set fieldlist {active distr err label limits max min var}
	if {$globalfit} {
		lappend fieldlist common
		lappend fieldlist factor
	}
	foreach field $fieldlist {
		set entrylist [array names $AactualAnalytPar "$type,${p}*,$field"]
		set num [llength $entrylist]
		set c $actualAnalytPar($type,param_count)
		if {$num > $c} {
			for {set j [expr $c+1]} {$j <= $num} {incr j} {
				array unset $AactualAnalytPar "$type,${p}$j,$field"
			}
		}
	}
	.analytical.fitpar.notebook compute_size
}

proc distr_enable {distrib actualAnalytPar type path p globalfit} {
	upvar $distrib distr
	upvar $actualAnalytPar ap
	if {$ap($type,param_count) < 1 || ![winfo exists [format %s%s $path.${p}1 distr]]
	} { return }

	set distr 1
	for {set indx_i 1} {$indx_i <= $ap($type,param_count) } {incr indx_i
	} {
		if {[winfo exists [format %s%s $path.$p$indx_i distr]] 
		} {
			[format %s%s $path.$p$indx_i distr] configure -state normal
			if {$ap($type,$p$indx_i,distr) == 1} { set distr $indx_i }
		}
	}
	if {$distr > 0 && $distr <= $ap($type,param_count)} {
		set ap($type,$p$distr,active) 0
		set ap($type,$p$distr,distr) 1
		param_disable $path $p $distr $globalfit 1
	}
}

proc distr_disable {distrib actualAnalytPar type path p globalfit} {
	upvar $distrib distr
	upvar $actualAnalytPar ap
	if {$ap($type,param_count) < 1 || ![winfo exists [format %s%s $path.${p}1 distr]]
	} { return }

	if {$distr > 0} {
		param_enable $path $p $distr $globalfit 1
	}
	set distr 0
	for {set indx_i 1} {$indx_i <= $ap($type,param_count) } {incr indx_i
	} {
		if {[winfo exists [format %s%s $path.$p$indx_i distr]] 
		} {
			set ap($type,$p$indx_i,distr) 0
			[format %s%s $path.$p$indx_i distr] configure -state disabled
		}
	}
}

proc param_enable {path p idx globalfit with_distr
} {
	[format %s%s $path.$p$idx label  ] configure -foreground Black
	[format %s%s $path.$p$idx entry  ] configure -state normal
	[format %s%s $path.$p$idx entry  ] configure -foreground Black
	[format %s%s $path.$p$idx up     ] configure -state normal
	[format %s%s $path.$p$idx down   ] configure -state normal
	[format %s%s $path.$p$idx fit    ] configure -state normal 
	[format %s%s $path.$p$idx fit    ] configure -selectcolor $::check_button_color_en
	eval [format %s%s $path.$p$idx fit    ] configure $::radio_check_button_prop
	if {$globalfit} {
		[format %s%s $path.$p$idx factor ] configure -state normal
		[format %s%s $path.$p$idx factor ] configure -foreground Black
		[format %s%s $path.$p$idx list   ] configure -state normal
		[format %s%s $path.$p$idx list   ] configure -foreground black
	}
	if {$with_distr} {
		[format %s%s $path.$p$idx distr ] configure -state normal
		eval [format %s%s $path.$p$idx distr ] configure $::radio_check_button_prop
	}
}

proc param_disable {path p idx globalfit with_distr
} {
	[format %s%s $path.$p$idx label  ] configure -foreground $::check_button_color_dis
	[format %s%s $path.$p$idx entry  ] configure -state disabled
	[format %s%s $path.$p$idx entry  ] configure -foreground $::check_button_color_dis
	[format %s%s $path.$p$idx up     ] configure -state disabled
	[format %s%s $path.$p$idx down   ] configure -state disabled
	[format %s%s $path.$p$idx fit    ] configure -state disabled
	[format %s%s $path.$p$idx fit    ] configure -selectcolor $::check_button_color_dis
	if {$globalfit} {
		[format %s%s $path.$p$idx factor ] configure -state disabled
		[format %s%s $path.$p$idx factor ] configure -foreground $::check_button_color_dis
		[format %s%s $path.$p$idx list   ] configure -state disabled
		[format %s%s $path.$p$idx list   ] configure -foreground $::check_button_color_dis
	}
	if {$with_distr} {
		[format %s%s $path.$p$idx distr ] configure -state disabled
		[format %s%s $path.$p$idx distr ] configure -selectcolor $::check_button_color_dis
	}
}

proc set_actualAP_gen {type AactualAnalytPar globalfit } {

	set_actualAP_labels $type $AactualAnalytPar
	build_param_gui $type $AactualAnalytPar $globalfit
	set_actualAP_config $type $AactualAnalytPar $globalfit
	fiterror_update
}

proc set_actualAP {AactualAnalytPar {guiupdate yes}} {

# important: do each step first for _all_ then continue with the next step
	foreach type {SD SQ FF} { set_actualAP_labels $type $AactualAnalytPar }
	if ($guiupdate) {
	    foreach type {SD SQ FF} { build_param_gui $type $AactualAnalytPar 0 }
	}
	foreach type {SD SQ FF} { set_actualAP_config $type $AactualAnalytPar 0 }
	fiterror_update
}

proc set_alambdaCmd { name1 name2 op } {
	set ::AnalytPar(alambda) -1
	set ::tmpAnalytPar(alambda) -1
	set ::actualAnalytPar(alambda) -1
}


#------------------------------------------------------------------------------
# returns quick help message
proc get_helpline {type num actual_ap} {
	upvar $actual_ap ap
switch -exact $type {
   SD { switch $ap(SD,typestr) {
           LogNorm { return "LogNorm(x) = N x^(-p) exp(-(ln(x)-ln(mu))^2/(2s^2)) \nwith int(LogNorm(x),x=0,infinity) = N)" }
           BiLogNorm { return "BiLogNorm(x) = LogNorm(N1,s1,..x)+LogNorm(N2,s2,..R) \nwith int(BiLogNorm(x),x=0,infinity) = N1+N2" }
           GEX { return "GEX(x) = generalized exponential distribution \nwith int(GEX(x),x=0,infinity) = N)" }
           GEV { return "GEV(x) = generalized extreme value distribution \nwith int(GEV(x),x=0,infinity) = N)" }
           gammaSD { return "gammaSD(x) = gamma distribution = theta^k exp(-x/theta) x^(k-1) / GAMMA(k)\nwith int(gammaSD(x),x=0,infinity) = N), mode=(k-1)*theta, sigma^2 = k*theta^2" }
           Triangular { return "Triangular(x) = triangular distribution \nwith int(Triangular(x),x=0,infinity) = N)" }
           fractalSD { return "fractalSD(x) = fractal size distribution \nwith int(fractalSD(x),x=0,infinity) = N)" }
           Uniform { return "Uniform(x) = Uniform distribution \nwith int(Iniform(x),x=0,infinity) = N)" }
           Beta { return "Beta(x) = Beta distribution \nwith int(Iniform(x),x=0,infinity) = N)" }
           Gaussian { return "Gaussian(x) = N/c exp(-(x-x0)^2/(2s^2)) \nwith int(Gauss(x),x=0,infinity) = N)"}
           Maxwell { return "Maxwell(x) = N/c sqrt(2/Pi) s^3 (x-x0)^2 exp(-(x-x0)^2/(2s^2)) for x>x0\notherwise 0 with int(Maxwell(x),x=0,infinity) = N)"}
           Schultz-Zimm { return "Schultz-Zimm(x) = N (x/Ra)^(k-1)/Ra k^k/Gamma(k) exp(-k*x/Ra)\nwith k=1/sig^2 and int(Schultz-Zimm(x),x=0,infinity) = N)"}
           Weibull { return "Weibull(x) = N l/a*((x-mu)/a)^(l-1)*exp(-((x-mu)/a)^l)\nwith int(Weibull(x),x=0,infinity) = N)"}
           Monodisperse {return "delta(x)\n"}
           "A_div_B"    {return "A/B delta(x)\n"}
           Delta        {return "N delta(x)\n"}
	   default { return [get_descr_by_id $ap($type,typestr) $num] }
        }
      }
   SQ { switch $ap(SQ,typestr) {
           None { 
              switch $num {
                 0 {return "no structure factor\n"}
                 1 {return "no structure factor\n"}
                 2 {return "no structure factor\n"}
                 3 {return "no structure factor\n"}
                 4 {return "no structure factor\n"}
                 5 {return "no structure factor\n"}
                 6 {return "no structure factor\n"}
                 7 {return "no structure factor\n"}
                 8 {return "no structure factor\n"}
                 9 {return "no structure factor\n"}
                10 {return "no structure factor\n"}
              }
           }
           "Hard Sphere" { 
              switch $num {
                 0 {return "S(Q,RHS,fp)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "fp: volume fraction\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Sticky Hard Sphere" { 
              switch $num {
                 0 {return "S(Q,RHS,tau,fp)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "tau: stickiness\n"}
                 3 {return "fp: volume fraction\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Sticky Hard Sphere 2" { 
              switch $num {
                 0 {return "S(Q,RHS,tau,fp)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "tau: stickiness\n"}
                 3 {return "Delta: width of stickiness potential\n"}
                 4 {return "fp: volume fraction\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Square Well Potential" { 
              switch $num {
                 0 {return "S_SquareWell(Q,RHS,eta,epsi/kBT,lambda)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "eta: colloid volume fraction\n"}
                 3 {return "epsi/kBT: square well depth in terms of kBT\n"}
                 4 {return "lambda: relative square well width, lambda should be larger than 1\ntotal width Delta = 2 RHS (lambda-1)"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Thin Square Well Potential" { 
              switch $num {
                 0 {return "S_ThinSquareWell(Q,RHS,phi,U/kT,epsilon)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "phi: volume fraction\n"}
                 3 {return "U/kT: square well depth in terms of kT\n"}
                 4 {return "epsilon: relative square well width, epsilon should be between 0 and 0.05\nepsilon = DeltaR/(2RHS+DeltaR)"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Square Well Potential 2" { 
              switch $num {
                 0 {return "S_SquareWell2(Q,RHS,eta,epsi/kB,Delta,T)\n"}
                 1 {return "RHS: hard sphere repulsion radius\n"}
                 2 {return "eta: colloid volume fraction\n"}
                 3 {return "epsi/kB: square well depth in K\n"}
                 4 {return "Delta: square well width\n"}
                 5 {return "T: temperature in K\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Cylinder(PRISM)" {
               switch $num {
                 0 {return "S_Cyl(Q,R,L,nu) structure factor for cylinders\n"}
                 1 {return "R: radius of cylinder cross section \n"}
                 2 {return "L: length of cylinder\n"}
                 3 {return "nu: osmotic compressibility\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
	   }
           "RPA" {
               switch $num {
                 0 {return "RPA (random phase approximation) structure factor\nS(Q) = 1/(1+nu P(Q))"}
                 1 {return "nu: ?? \n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
	   }
           "ThermalDisorder" {
               switch $num {
                 0 {return "S_TD(Q,N,d,Delta,Nu) structure factor of weakly ordered membrane stacks\nthermal disorder caused by fluctuations of bilayers around mean layer positions of equal separation"}
                 1 {return "N: total number of layers within scattering domain \n"}
                 2 {return "d: stacking separation\n"}
                 3 {return "Delta: Debye-Waller temperature factor\nDelta = <(d_k-d)^2>"}
                 4 {return "Nu: scaling constant for the additional diffuse scattering term\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Paracrystalline" {
               switch $num {
                 0 {return "S_PT(Q,N,d,Delta,Nu) structure factor of weakly ordered membrane stacks\ndisorder accounting for small variation in bilayer separatios (stacking disorder)"}
                 1 {return "N: total number of layers within scattering domain \n"}
                 2 {return "d: stacking separation\n"}
                 3 {return "Delta: stacking disorder parameter\n"}
                 4 {return "Nu: scaling constant for the additional diffuse scattering term\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "ModifiedCaille" {
               switch $num {
                 0 {return "S_MCT(Q,N,d,Delta,Nu) structure factor of weakly ordered membrane stacks\ndisorder when bilayer bending fluctuations are considered"}
                 1 {return "N: total number of layers within scattering domain \n"}
                 2 {return "d: stacking separation\n"}
                 3 {return "eta: Caillé parameter, which is measure for bilayer bending fluctuations\n"}
                 4 {return "Nu: scaling constant for the additional diffuse scattering term\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "GaussianAmplitude" {
               switch $num {
                 0 {return "Peak with Gaussian Profile\n"}
                 1 {return "Ampl.: amplitude of peak\n"}
                 2 {return "mean: position of peak maximum\n"}
                 3 {return "width: width of Gaussian profile\n"}
                 4 {return "bckgr: background\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "LorentzianAmplitude" {
               switch $num {
                 0 {return "Peak with Lorentzian Profile\n"}
                 1 {return "Ampl.: amplitude of peak\n"}
                 2 {return "mean: position of peak maximum\n"}
                 3 {return "width: width of Lorentzian profile\n"}
                 4 {return "bckgr: background\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "VoigtPeak" {
               switch $num {
                 0 {return "Peak with Voigt Profile\n"}
                 1 {return "Area: area below peak\n"}
                 2 {return "mean: position of peak maximum\n"}
                 3 {return "sigma: width of Doppler profile\n"}
                 4 {return "gamma: width of Lorentzian profile\n"}
                 5 {return "backgr: background\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "PearsonVIIAmplitude" {
               switch $num {
                 0 {return "Peak with PearsonVII Profile: y=bckr+A/(1+4z^2(w^(1/width2)-1))^width2\nwith z=(x-mean)/width1"}
                 1 {return "Ampl.: amplitude of peak\n"}
                 2 {return "mean: position of peak maximum\n"}
                 3 {return "width1: first width profile\n"}
                 4 {return "width2: second wisth parameter\n"}
                 5 {return "bckgr: background\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Mass Fractal (Exp Cut-Off)" {
               switch $num {
                 0 {return "S(Q,xi,D)\n"}
                 1 {return "r0: characteristic dimension of individual scatterers\n"}
                 2 {return "xi: size of fractal aggregate\n"}
                 3 {return "D: fractal dimension\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Mass Fractal (Exp(-x^a) Cut-Off)" {
               switch $num {
                 0 {return "S(Q,xi,D)\n"}
                 1 {return "r0: characteristic dimension of individual scatterers\n"}
                 2 {return "xi: size of fractal aggregate\n"}
                 3 {return "D: fractal dimension\n"}
                 4 {return "alpha: exponent in cut-off function exp(-x^alpha)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Mass Fractal (Gaussian Cut-Off)" {
               switch $num {
                 0 {return "S(Q,xi,D)\n"}
                 1 {return "r0: characteristic dimension of individual scatterers\n"}
                 2 {return "xi: size of fractal aggregate\n"}
                 3 {return "D: fractal dimension\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Mass Fractal (OverlapSph Cut-Off)" {
               switch $num {
                 0 {return "S(Q,xi,D)\n"}
                 1 {return "r0: characteristic dimension of individual scatterers\n"}
                 2 {return "xi: size of fractal aggregate\n"}
                 3 {return "D: fractal dimension\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Correlation Hole" {
               switch $num {
                 0 {return "Correlation Hole     x=Qh\nS(Q)=1.0+3.0*eta*(x*cos(x)-sin(x))/pow(x,3.0)"}
                 1 {return "h: hole radius\n"}
                 2 {return "eta: volume fraction\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Critical Scattering" {
               switch $num {
                 0 {return "Critical Scattering\nS(Q,kappa,zeta)=1+kappa/(1+zeta^2*Q^2)"}
                 1 {return "kappa: scaling factor\n"}
                 2 {return "zeta: correlation length\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Macro Ion (HP)" {
               switch $num {
                 0 {return "macro ion structure factor \nRMSA approximation according to Hayter and Penfold"}
                 1 {return "T: temperature in Celsius\n"}
                 2 {return "Z_eff: effective particle charge in ???\n"}
                 3 {return "RHS: hard sphere radius\n"}
                 4 {return "ION: ionic strength in ???\n"}
                 5 {return "ETA: volume fraction\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Hayter Penfold RMSA" {
               switch $num {
                 0 {return "RMSA approximation according to Hayter and Penfold\nscattering vectors need to be in units of nm^-1"}
                 1 {return "RHS: hard sphere radius of particle\nneeds to be given in nm"}
                 2 {return "Z_eff: effective particle charge\n"}
                 3 {return "eta: volume fraction\n"}
                 4 {return "T: temperature in \[K\]\n"}
                 5 {return "salt: monovalent salt concentration \[M\]\n"}
                 6 {return "eps_r: dielectric constant of solvent \n(for H2O 71.08)"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "BabaAhmed" {
               switch $num {
                 0 {return "structure factor from charged colloidal suspensions"}
                 1 {return "Deff: effectife diameter in \[nm\]\n"}
                 2 {return "Psi,0,eff: effective surface potential in \[N*m/C\]\n"}
                 3 {return "epsilon: dielectric constant of solvent in \[C^2/(Nm^2\]\n"}
                 4 {return "eta: volume fraction\n"}
                 5 {return "kD: inverse Debye screening length in \[nm^-1\]\n"}
                 6 {return "T: \[K\]\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "BabaAhmed2" {
               switch $num {
                 0 {return "structure factor from charged colloidal suspensions"}
                 1 {return "R_eff: effective radius in nm\n"}
                 2 {return "z_p charge per colloid particle in units of e0\n"}
                 3 {return "epsilon_r: relative permittivity\n"}
                 4 {return "mB: molarity of ion in mol/l\n"}
                 5 {return "zB: charge number of ion in units of e0\n"}
                 6 {return "eta: volume fraction\n"}
                 7 {return "T: temperature in Kelvin\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "LocalOrder" {
               switch $num {
                 0 {return "\n"}
                 1 {return "D: \n"}
                 2 {return "alpha: ???\n"}
                 3 {return "z: \n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "RandomDistribution" {
               switch $num {
                 0 {return "\n"}
                 1 {return "Rca: \n"}
                 2 {return "R: ???\n"}
                 3 {return "fp: \n"}
                 4 {return "epsilon: \n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):ThinDisc" {
               switch $num {
                 0 {return "\n"}
                 1 {return "L: cylinder length\n"}
                 2 {return "\n"}
                 3 {return "sigma:width of lognormal distribution\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):ThinSphericalShell" {
               switch $num {
                 0 {return "\n"}
                 1 {return "L: cylinder length\n"}
                 2 {return "\n"}
                 3 {return "sigma:width of lognormal distribution\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):Rod" {
               switch $num {
                 0 {return "\n"}
                 1 {return "L: cylinder length\n"}
                 2 {return "\n"}
                 3 {return "sigma:width of lognormal distribution\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):Worm(PS1)" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "exvol: excluded volume parameter\nexvol < 1 without excluded volume,  exvol >= 1 with excluded volume"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):Worm(PS2)" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):Worm(PS3)" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "exvol: excluded volume parameter\nexvol < 1 without excluded volume,  exvol >= 1 with excluded volume"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):WormLikeChain_withEXV" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):WormLikeChain_withoutEXV" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "P'(Q):KholodenkoWorm" {
               switch $num {
                 0 {return "\n"}
                 1 {return "lb: Kuhn length\n"}
                 2 {return "L: contour length\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
	   default { return [get_descr_by_id $ap($type,typestr) $num] }
       }
      }
   FF { switch $ap(FF,typestr) {
           Sphere { 
              switch $num {
                 0 {return "F(Q,R) = 4/3 Pi R^3 eta 3 (sin(QR)-QRcos(QR))/(QR)^3\n"}
                 1 {return "R: radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           HardSphere { 
              switch $num {
                 0 {return "F(Q,R) = S(Q,RHS,fp) * K(Q,R,eta)^2\n"}
                 1 {return "R: radius\n"}
                 2 {return "RHS: hard sphere repulsion radius\n"}
                 3 {return "fp: volume fraction\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           StickyHardSphere { 
              switch $num {
                 0 {return "F(Q,R) = S(Q,RHS,tau,fp) * K(Q,R,eta)^2\n"}
                 1 {return "R: radius\n"}
                 2 {return "RHS: hard sphere repulsion radius\n"}
                 3 {return "tau: stickiness\n"}
                 4 {return "fp: volume fraction\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Spherical Shell i" {
               switch $num {
                 0 {return "F(Q,R1,R2,mu,eta) = K(Q,R1,eta) - K(Q,R2,eta*(1-mu))\nwith K(Q,R,eta) = 4/3 Pi R^3 eta 3 (sin(QR)-QRcos(QR))/(QR)^3"}
                 1 {return "R1: outer radius\n"}
                 2 {return "R2: inner radius\n"}
                 3 {return "mu*eta: scattering contrast of inner core\n"}
                 4 {return "eta: scattering contrast of shell\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Spherical Shell ii" {
               switch $num {
                 0 {return "F(Q,R,nu*R,mu,eta) = K(Q,R,eta) - K(Q,nu*R,eta*(1-mu))\nwith K(Q,R,eta) = 4/3 Pi R^3 eta 3 (sin(QR)-QRcos(QR))/(QR)^3"}
                 1 {return "R: outer radius\n"}
                 2 {return "nu*R: inner radius\n"}
                 3 {return "mu*eta: scattering contrast of inner core\n"}
                 4 {return "eta: scattering contrast of shell\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "Spherical Shell iii" {
               switch $num {
                 0 {return "F(Q,R,nu*R,mu,eta) = K(Q,R+dR,eta2) - K(Q,R,eta2-eta1)\nwith K(Q,R,eta) = 4/3 Pi R^3 eta 3 (sin(QR)-QRcos(QR))/(QR)^3"}
                 1 {return "R: inner radius\n"}
                 2 {return "R+dR: outer radius\n"}
                 3 {return "eta1: scattering contrast of inner core\n"}
                 4 {return "eta2: scattering contrast of shell\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "MultiLamellarVesicle" {
               switch $num {
                 0 {return " MLV(Q) = sum( 4/3*PI*(R_c+i*t_sh+i*t_sol)^3*K(interp,Q,R_c+i*t_sh+i*t_sol,eta_sol-eta_sh)+\n+4/3*PI*(R_c+(i+1)*t_sh+i*t_sol)^3*K(Q,R_c+(i+1)*t_sh+i*t_sol,eta_sol-eta_sh);i=0..n-1)"}
                 1 {return "R_c: core radius, core contains solvent\n"}
                 2 {return "t_sh: thickness of surfactant layer thickness\n"}
                 3 {return "t_sol: thickness of solvent layer\n"}
                 4 {return "eta_sh: scattering length density of surfactant shell\n"}
                 5 {return "eta_sol: scattering length density of solvent as well as core\n"}
                 6 {return "n: number of layers\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "RNDMultiLamellarVesicle" {
               switch $num {
                 0 {return " MLV(Q) = Multilamellar vesicle with non-centric shells\nrandom displacement up to Dt_sol between successive layers"}
                 1 {return "R_c: core radius, core contains solvent\n"}
                 2 {return "t_sh: average thickness of surfactant layer\n"}
                 3 {return "t_sol: average thickness of solvent layer\n"}
                 4 {return "Dt_sol: maximum random displacement between successive layers, valuesd  between (0:1)\n"}
                 5 {return "eta_sh: scattering length density of surfactant shell\n"}
                 6 {return "eta_sol: scattering length density of solvent as well as core\n"}
                 7 {return "n: number of layers\n"}
                 8 {return "sigma_Rc: width of size distribution of Rc (Gauss distr.)\n"}
                 9 {return "sigma_tsh: width of thickness distribution of tsh (Gauss distr.)\n"}
                10 {return "\n"}
               }
           }
           "RNDMultiLamellarVesicle2" {
               switch $num {
                 0 {return " MLV(Q) = Multilamellar vesicle with non-centric shells\nrandom displacement up to Dt_sol between successive layers"}
                 1 {return "t_sh: average thickness of surfactant layer\n"}
                 2 {return "sigma_tsh: width parameter of thickness distribution of tsh (Gauss distr.)\n"}
                 3 {return "R_c: core radius, core contains solvent\n"}
                 4 {return "sigma_Rc: width parameter of size distribution of Rc (LogNorm distr.)\n"}
                 5 {return "n: average number of layers\n"}
                 6 {return "sigma_n: width parameter of distribution of number of layers (assuming lognormal distribution)\n"}
                 7 {return "t_sol: average thickness of solvent layer\n"}
                 8 {return "sigma_t_sol: width parameter of thickness distribution  of solvent layer (Gaussian distrib.)\n"}
                 9 {return "Dt_sol: maximum random displacement between successive layers, valuesd  between (0:1)\n"}
                 10 {return "Delta_eta: scattering contrast between surfactant shell and solvent\n"}
               }
           }
           "MLVesicleFrielinghaus" {
               switch $num {
                 0 {return " MLVesicleFrielinghaus(Q) = Multilamellar vesicle with non-centric shells\nrandom displacement up to Rshift between successive layers"}
                 1 {return "\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "BiLayeredVesicle" {
               switch $num {
                 0 {return " BLV(Q) = \n"}
                 1 {return "R_c: core radius, core contains solvent\n"}
                 2 {return "t_h: thickness of bilayer on solvent side\n"}
                 3 {return "t_t: thickness of inner part of bilayer\n"}
                 4 {return "eta_sol: scattering length density of solvent\n"}
                 5 {return "eta_h: scattering length density bilayer on solvent side\n"}
                 6 {return "eta_t: scattering length density of inner part of bilayer\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "MagneticShellPsi" {
               switch $num {
                 0 {return "magnetic shell in saturation I=Fn^2+(Fm^2-2*FnFm)sin^2(psi)\nFn,Fm:nuclear and magnetic formfactor of a shell"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "mag_sh: magnetic scattering length density of shell\n"}
                 8 {return "mag_m: magnetic scattering length density of matrix\n"}
                 9 {return "psi: angle between B and Q\n"}
                10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
               }
           }
           "MagneticShellAniso" {
               switch $num {
                 0 {return "magnetic shell in saturation I=Fm^2-2*FnFm\nFn,Fm:nuclear and magnetic formfactor of a shell"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "mag_sh: magnetic scattering length density of shell\n"}
                 8 {return "mag_m: magnetic scattering length density of matrix\n"}
                 10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
                9 {return "\n"}
               }
           }
           "MagneticShellCrossTerm" {
               switch $num {
                 0 {return "magnetic shell in saturation (cross-term) I=4*FnFm\nFn,Fm:nuclear and magnetic formfactor of a shell"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "mag_sh: magnetic scattering length density of shell\n"}
                 8 {return "mag_m: magnetic scattering length density of matrix\n"}
                 10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
                9 {return "\n"}
               }
           }
           "SuperparamagneticFFpsi" {
               switch $num {
                 0 {return "Scattering intensiy of a superparamagnetic ferrofluid\nonly core is magnetic and particle has a nuclear shell structure"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "R0: radius for which alpha is supplied\n"}
                 8 {return "alpha: Langevin argument for R=R0\n"}
                 9 {return "psi: angle between B and Q\n"}
                10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
               }
           }
           "SuperparamagneticFFCrossTerm" {
               switch $num {
                 0 {return "Cross term I(p)-I(-p) for psi=90° of scattering intensiy of a superparamagnetic ferrofluid\nonly core is magnetic but particle has a nuclear shell structure"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "R0: radius for which alpha is supplied\n"}
                 8 {return "alpha: Langevin argument for R=R0\n"}
                 10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
                9 {return "\n"}
               }
           }
           "SuperparamagneticFFAniso" {
               switch $num {
                 0 {return "Ansisotropic term of scattering intensiy of a superparamagnetic ferrofluid\nonly core is magnetic but particle has a nuclear shell structure"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "R0: radius for which alpha is supplied\n"}
                 8 {return "alpha: Langevin argument for R=R0\n"}
                 10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
                9 {return "\n"}
               }
           }
           "SuperparamagneticFFIso" {
               switch $num {
                 0 {return "Isotropic term of scattering intensiy of a superparamagnetic ferrofluid\nonly core is magnetic but particle has a nuclear shell structure"}
                 1 {return "R: inner radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "nuc_c: nuclear scattering length density of core\n"}
                 4 {return "nuc_sh: nuclear scattering length density of shell\n"}
                 5 {return "nuc_m: nuclear scattering length density of matrix\n"}
                 6 {return "mag_c: magnetic scattering length density of core\n"}
                 7 {return "R0: radius for which alpha is supplied\n"}
                 8 {return "alpha: Langevin argument for R=R0\n"}
                 10 {return "pol: pol==0:unpolarized,\npol<0:negativ polarization, pol>0:positive polarization"}
                9 {return "\n"}
               }
           }
           "LinShell" {
               switch $num {
                 0 {return "LinShell = core shell form factor\nwith linear dependent contrast profile of shell\n"}
                 1 {return "R: core radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "eta_c: scattering length density of core\n"}
                 4 {return "eta_sh: scattering length density of non-swollen shell\n"}
                 5 {return "x_in: amount of solvent at R\n"}
                 6 {return "x_out: amount of solvent at R+dR\n"}
                 7 {return "eta_s: scattering length density of solvent\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "LinShell2" {
               switch $num {
                 0 {return "LinShell2 = core shell form factor\nwith linear dependent contrast profile of shell\n"}
                 1 {return "Rtot: overall radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "eta_c: scattering length density of core\n"}
                 4 {return "eta_sh: scattering length density of non-swollen shell\n"}
                 5 {return "x_in: amount of solvent at R\n"}
                 6 {return "x_out: amount of solvent at R+dR\n"}
                 7 {return "eta_s: scattering length density of solvent\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "SphereShell_compr" {
               switch $num {
                 0 {return "SphereShell_compr = spherical core shell form factor\nwhich is compressable\n"}
                 1 {return "Rtot: overall radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "eta_c: scattering length density of core\n"}
                 4 {return "eta_sh: scattering length density of non-swollen shell\n"}
                 5 {return "x_sol: amount of solvent in shell\n"}
                 6 {return "eta_s: scattering length density of solvent\n"}
                 7 {return "kappa_c: compressibility of core\n"}
                 8 {return "kappa_sh: compressibility of shell\n"}
                 9 {return "kappa_s: compressibility of solvent\n"}
                10 {return "P: applied pressure\n"}
               }
           }
           "ExpShell" {
               switch $num {
                 0 {return "ExpShell = core shell form factor\nwith expotential dependent contrast profile of shell\n"}
                 1 {return "R_core: core radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "eta_core: scattering length density of core\n"}
                 4 {return "eta_shell: scattering length density of non-swollen shell\n"}
                 5 {return "x_in_solv: amount of solvent at R\n"}
                 6 {return "x_out_solv: amount of solvent at R+dR\n"}
                 7 {return "alpha: factor for exponentail decay of scattering length density in shell\n"}
                 8 {return "eta_solv: scattering length density of solvent\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "DumbbellShell" {
               switch $num {
                 0 {return "DumbbellShell = core shell form factor\ntwo particles are in close contact"}
                 1 {return "R: core radius\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "L: surface to surface distance\n2(R+dR)+L: center to center distance"}
                 4 {return "eta_c: scattering length density of core\n"}
                 5 {return "eta_sh: scattering length density of shell\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "two_attached_spheres" {
               switch $num {
                 0 {return "two_attached_spheres = \n"}
                 1 {return "R1: radius of first sphere \n"}
                 2 {return "R2: radiuis of second sphere\n"}
                 3 {return "eta0: SLD of solvent\n"}
                 4 {return "eta1: SLD of first sphere\n"}
                 5 {return "eta2: SLD of second sphere\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
               }
           }
           "DoubleShellChain" {
               switch $num {
                 0 {return "DoubleShellChain = linear chain of spherical double shell particles\n"}
                 1 {return "R_c: core radius\n"}
                 2 {return "DR1: thickness of inner shell\n"}
                 3 {return "DR2: thickness of outer shell\n"}
                 4 {return "L: surface to surface distance\n2(R_c+DR1+DR2)+L: center to center distance"}
                 5 {return "eta_c: scattering length density core\n"}
                 6 {return "eta_1: scattering length density of innet shell\n"}
                 7 {return "eta_2: scattering length density of outer shell\n"}
                 8 {return "eta_sol: scattering length density of solvent\n"}
                 9 {return "x_sol_sh2: amount of solvent in shell 2\n"}
                10 {return "n: number of double shell spheres in the chain\nn>0"}
               }
           }
           "TetrahedronDoubleShell" {
               switch $num {
                 0 {return "TetrahedronDoubleShell = spherical double shell particles\nplaced in the edges of a tetrahedron"}
                 1 {return "R_c: core radius\n"}
                 2 {return "DR1: thickness of inner shell\n"}
                 3 {return "DR2: thickness of outer shell\n"}
                 4 {return "L: surface to surface distance\n2(R_c+DR1+DR2)+L: center to center distance"}
                 5 {return "eta_c: scattering length density core\n"}
                 6 {return "eta_1: scattering length density of innet shell\n"}
                 7 {return "eta_2: scattering length density of outer shell\n"}
                 8 {return "eta_sol: scattering length density of solvent\n"}
                 9 {return "x_sol_sh2: amount of solvent in shell 2\n"}
                10 {return "n: number of double shell spheres in the chain\n 1<=n<=5"}
               }
           }
           "EllipsoidalCoreShell" {
               switch $num {
                 0 {return "ellipsoidal core shell structure"}
                 1 {return "a: principle semi-axis\n"}
                 2 {return "b: equatorial semi-axes\n(rotational axis)"}
                 3 {return "t: shell thickness\n"}
                 4 {return "eta_c: core scattering length density\n"}
                 5 {return "eta_sh: shell scattering length density\n"}
                 6 {return "eta_solvent: solvent scattering length density\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
                }
	   }   
           "triaxEllShell1" {
               switch $num {
                 0 {return "triaxial ellipsoidal core shell structure"}
                 1 {return "a: semi-axis\n"}
                 2 {return "b: semi-axis\n"}
                 3 {return "c: semi-axis\n"}
                 4 {return "t: shell thickness\n"}
                 5 {return "eta_c: core scattering length density\n"}
                 6 {return "eta_sh: shell scattering length density\n"}
                 7 {return "eta_solvent: solvent scattering length density\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
                }
	   }
           "Ellipsoid i" {
               switch $num {
                 0 {return "F^2(Q,R,V,eta) = 1/2/Pi*V^2*eta^2*Int(sin(Theta)*\nK(Q,1/4*R*(9*V^2/Pi^2/R^6*cos(Theta)^2+16*sin(Theta)^2)^(1/2),eta)^2,Theta=0..Pi)"}
                 1 {return "R: equatorial semi-axes\n"}
                 2 {return "V: Volume\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
                }
	   }   
            "Ellipsoid ii" {
               switch $num {
                 0 {return "F^2(Q,R,nu,eta) = 1/2/Pi*V^2*eta^2*Int(sin(Theta)*\nK(Q,1/4*R*(9*V^2/Pi^2/R^6*cos(Theta)^2+16*sin(Theta)^2)^(1/2),eta)^2,Theta=0..Pi)"}
                 1 {return "R: equatorial semi-axes\n"}
                 2 {return "nu: nu*R principle semi-axis\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
                }
	   }  
           "EllCylShell" {
               switch $num {
                 0 {return "form factor of a cylinder with an elliptical cross-section\n"}
                 1 {return "R: radius, R is length of first semiaxis\n"}
                 2 {return "epsilon: eccentricity, epsilon*R is length of second semiaxes\n"}
                 3 {return "L: cylinder length\n"}
                 4 {return "t: shell thickness\n"}
                 5 {return "eta_core: scattering length density of cylinder core\n"}
                 6 {return "eta_shell: scattering length density of cylinder shell\n"}
                 7 {return "eta_sol: scattering length density of solvent\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
	     }
	   }      
           "Cylinder" {
               switch $num {
                 0 {return "F^2(Q,R,L,eta) = 4 int(J_1^2(QR sqrt(1-x^2)/(QR sqrt(1-x^2))^2 sinc^2(QLx/2,x=0..1)\n"}
                 1 {return "R: radius\n"}
                 2 {return "L: height\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
	     }
	   }   
           "FlatCylinder" {
               switch $num {
                 0 {return "F^2(Q,R,L,eta) = Porod's approximation of a flat cylinder\nwith L << R"}
                 1 {return "R: radius\n"}
                 2 {return "L: height\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
	     }
	   }   
           "LongCylinder" {
               switch $num {
                 0 {return "F^2(Q,R,L,eta) = Porod's approximation of a long cylinder\nwith L >> R"}
                 1 {return "R: radius\n"}
                 2 {return "L: height\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
	     }
	   }  
           "PorodCylinder" {
               switch $num {
                 0 {return "F^2(Q,R,L,eta) = Porod's approximation of a cylinder\nif L>2R: LongCylinder otherwise FlatCylinder"}
                 1 {return "R: radius\n"}
                 2 {return "L: height\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
	     }
	   }
           LongCylShell { 
              switch $num {
                 0 {return "P(Q,R,DR,L,...) = P_rod*Pcs\n"}
                 1 {return "R: core radius\n"}
                 2 {return "DR: shell thickness\n"}
                 3 {return "L: length of cylinder\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           ellCylShell1 { 
              switch $num {
                 0 {return "P(Q,R,epsilon,L,t,...) = ...\n"}
                 1 {return "R: core radius\n"}
                 2 {return "epsilon: eccentricity of ellipoid\n"}
                 3 {return "L: length of cylinder\n"}
                 3 {return "t: shell thickness\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           ellCylShell2 { 
              switch $num {
                 0 {return "P(Q,R,epsilon,L,t,...) = ...\n"}
                 1 {return "R: core radius\n"}
                 2 {return "epsilon: eccentricity of ellipoid\n"}
                 3 {return "L: length of cylinder\n"}
                 3 {return "t: shell thickness\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Pcs:homogeneousCyl { 
              switch $num {
                 0 {return "Pcs(Q,R,...) \n"}
                 1 {return "R: core radius\n"}
                 2 {return "eta_core: scattering length density of core\n"}
                 3 {return "eta_solv: scattering length density of solvent\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Pcs:ellCylSh { 
              switch $num {
                 0 {return "Pcs(Q,R,DR,L,...) \n"}
                 1 {return "R: core radius\n"}
                 2 {return "epsilon: eccentricity of elliptical cross section\n"}
                 3 {return "t: shell thickness\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           CylShell1 { 
              switch $num {
                 0 {return "random oriented cylindrical shell WITHOUT capped ends (exact)\n"}
                 1 {return "R: core radius\n"}
                 2 {return "DR: shell thickness\n"}
                 3 {return "L: length of cylinder\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           CylShell2 { 
              switch $num {
                 0 {return "random oriented cylindrical shell WITH capped ends (exact)\n"}
                 1 {return "R: core radius\n"}
                 2 {return "DR: shell thickness\n"}
                 3 {return "L: length of cylinder\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           alignedCylShell { 
              switch $num {
                 0 {return "P(Q,R,DR,L,...) = aligned cylindrical shell structure\n"}
                 1 {return "R: core radius\n"}
                 2 {return "DR: shell thickness\n"}
                 3 {return "L: length of cylinder\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "psi: direction of Q\[deg\]\n"}
                 8 {return "theta: orientation of cylinder axis towards vertical axis in plane of the detector \[deg\]\n"}
                 9 {return "phi: rotation of cylinder axis out of the detector plane \[deg\]\n"}
                10 {return "\n"}
              }
           }
           "partly aligned CylShell" { 
              switch $num {
                 0 {return "P(Q,R,DR,L,...) = partly aligned cylindrical shell structure\n"}
                 1 {return "R: core radius\n"}
                 2 {return "DR: shell thickness\n"}
                 3 {return "L: length of cylinder\n"}
                 4 {return "eta_core: scattering length density of core\n"}
                 5 {return "eta_shell: scattering length density of shell\n"}
                 6 {return "eta_solv: scattering length density of solvent\n"}
                 7 {return "psi: direction of Q\[deg\]\n"}
                 8 {return "P2: order parameter\n"}
                 9 {return "P4: order parameter\n"}
                10 {return "P6: order parameter\n"}
              }
           }
           Disc { 
              switch $num {
                 0 {return "F^2(Q,R,eta) = 2*Pi^2*R^4*eta^2/(Q*R)^2 * (1-1/(Q*R))*bessj1(2*Q*R)\n"}
                 1 {return "R: radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "eta: scattering contrast\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           TripleLayeredXS_ULV { 
              switch $num {
                 0 {return " = \n"}
                 1 {return "nagg: aggregation number per surface are\n"}
		 2 {return "V_core: \n"}
                 3 {return "V_middle: \n"}
                 4 {return "V_out:\n"}
                 5 {return "eta_core: scattering length density of the core\n"}
                 6 {return "eta_middle: scattering length density of the molecule part in the middle layer\n"}
                 7 {return "eta_out: scattering length density of the molecule part in the outer layer\n"}
                 8 {return "eta_solv: scattering length density of the solvent\n"}
                 9 {return "xsolv_middle: amount of solvent in the middle layer\n"}
                10 {return "xsolv_out: amount of solvent in the outer layer\n"}
              }
           }
           homogenousXS { 
              switch $num {
                 0 {return "P(Q,L,eta) = \n"}
                 1 {return "L: layer thickness\n"}
		 2 {return "D: diameter of planar object\n"}
                 3 {return "eta: scattering contrast\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           SphSh+SD+homoXS { 
              switch $num {
                 0 {return "P(Q,t,mean,sigmaeta) = \nSpherical shell with a LogNorm-distribution in its radius with R0 and sigma as defined for the LogNorm sizes distribution (p=1)"}
                 1 {return "t: most probable layer thickness\n"}
                 2 {return "sigma_t: width of thicknes size distribution, which is a LogNorm distribution\n"}
		 3 {return "R0: most probable radius of unilamellar spherical shell\n"}
                 4 {return "sigma_R: width of radius size distribution, which is a LogNorm distribution\n"}
                 5 {return "eta: scattering contrast\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           EllSh+SD+homoXS { 
              switch $num {
                 0 {return "P(Q,t,mean,sigmaeta) = \nellipsoidal shell with a LogNorm-distribution in its radius with R0 and sigma as defined for the LogNorm sizes distribution (p=1)"}
                 1 {return "t: most probable layer thickness\n"}
                 2 {return "sigma_t: width of thicknes size distribution, which is a LogNorm distribution\n"}
		 3 {return "R0: most probable radius of unilamellar spherical shell\n"}
                 4 {return "sigma_R: width of radius size distribution, which is a LogNorm distribution\n"}
                 5 {return "epsilon: eccentricity\n"}
                 6 {return "eta: scattering contrast\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           EllSh+SD+homoXS(S) { 
              switch $num {
                 0 {return "P(Q,t,mean,sigmaeta) = \nellipsoidal shell with a LogNorm-distribution in its surface area with S0 and sigma as defined for the LogNorm sizes distribution (p=1)"}
                 1 {return "t: most probable layer thickness\n"}
                 2 {return "sigma_t: width of thicknes size distribution, which is a LogNorm distribution\n"}
		 3 {return "S0: most probable surface are of unilamellar spherical shell\n"}
                 4 {return "sigma_S: width of surface area distribution, which is a LogNorm distribution\n"}
                 5 {return "epsilon: eccentricity\n"}
                 6 {return "eta: scattering contrast\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           CylSh+SD+homoXS { 
              switch $num {
                 0 {return "P(Q,t,mean,sigmaeta) = \ncylindrical shell with a LogNorm-distribution in its radius with R0 and sigma as defined for the LogNorm sizes distribution (p=1)"}
                 1 {return "t: most probable layer thickness\n"}
                 2 {return "sigma_t: width of thicknes size distribution, which is a LogNorm distribution\n"}
		 3 {return "R0: most probable radius of unilamellar spherical shell\n"}
                 4 {return "sigma_R: width of radius size distribution, which is a LogNorm distribution\n"}
                 5 {return "L0: most probable cylinder length\n"}
                 6 {return "sigma_L: width of length distribution, which is a LogNorm distribution\n"}
                 7 {return "eta: scattering contrast\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Disc+homoXS { 
              switch $num {
                 0 {return "P(Q,t,mean,sigmaeta) = \nDisc-shaped layer"}
                 1 {return "t: layer thickness\n"}
		 2 {return "D: diameter of unilamellar disc-like layer\n"}
                 3 {return "eta: scattering contrast\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           TwoInfinitelyThinPlates { 
              switch $num {
                 0 {return "P(Q,L,I0) =\n"}
                 1 {return "L: layer distance\n"}
                 2 {return "D: diameter of planar object\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           LayeredCentroSymmetricXS { 
              switch $num {
                 0 {return "P(Q,...) = \n"}
                 1 {return "L_out: \n"}
                 2 {return "L_core:\n"}
                 3 {return "b_out\n"}
                 4 {return "b_core:\n"}
                 5 {return "D: diameter of planar object\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           BiLayerGauss { 
              switch $num {
                 0 {return "P(Q,...) = \n"}
                 1 {return "sigma_out: \n"}
                 2 {return "b_out:\n"}
                 3 {return "sigma_core:\n"}
                 4 {return "b_core:\n"}
                 5 {return "t: distance between bilayer centres\n"}
                 6 {return "D: diameter of planar object\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Torus { 
              switch $num {
                 0 {return "Torus(Q,R,...) = calculates the scattering intensity of\na torus with an elliptical cross-section"}
                 1 {return "R: radius of torus\n"}
                 2 {return "a: inner radius of toris cross section\n"}
                 3 {return "Delta_a: shell thickness of torus cross section\n"}
                 4 {return "nu: if not equal one it describes scaling factor of\n cross sections radius incase elliptical cross section"}
                 5 {return "eta_c: scattering length density of core\n"}
                 6 {return "eta_sh: scattering length density of shell\n"}
                 7 {return "eta_sol: scattering length density of solvent\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           OrnsteinZernike { 
              switch $num {
                 0 {return "F^2(Q,Rg,I0) = I0/(1+Q^2*xi^2)\n"}
                 1 {return "xi: correlation length\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering I(Q=0)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           BroadPeak { 
              switch $num {
                 0 {return "F^2(q,I0,xi,m,q0) = I0/(1+(|q-q0|*xi)^m)^p\n"}
                 1 {return "I0: forward scattering\n"}
                 2 {return "xi: correlation length\n"}
                 3 {return "q0:peak position which is related to the d-spacing as q0 = 2pi/d\n"}
                 4 {return "m:\n"}
                 5 {return "p:\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           TeubnerStrey { 
              switch $num {
                 0 {return "F^2(Q,Rg,I0) = eta^2*8*pi/xi/(a^2-2bq^2+q^4)\nwith k=2pi/d, a=k^2+1/xi^2, b=k^2-1/xi^2"}
                 1 {return "xi: correlation length\n"}
                 2 {return "d: characteristic domain size (the periodicity)\n"}
                 3 {return "eta2: mean average contrast squared\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           DAB { 
              switch $num {
                 0 {return "F^2(Q,Rg,I0) = I0/(1+Q^2*xi^2)^2\n"}
                 1 {return "xi: correlation length\n"}
                 2 {return "I0: forward scattering I(Q=0)\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Spinodal { 
              switch $num {
                 0 {return "F^2(Q,gamma,Qmax,I0) = I0 (1+gamma/2)(Q/Qmax)^2/(gamma/2+(Q/Qmax)^(2+gamma))\n"}
                 1 {return "Qmax:\n"}
                 2 {return "gamma: the exponent gamma is equal to d+1 for off-critical concentration mixtures\nand 2d for critical concentration mixtures (d:dimension)"}
                 3 {return "Imax: scattering intensity at peak position I(Q=Qmax)\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Gauss { 
              switch $num {
                 0 {return "F^2(Q,Rg,I0) = I0*2*(u-1+exp(-u))/u^2 with u = Q^2*Rg^2\n"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering I(Q=0)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Gauss2 { 
              switch $num {
                 0 {return "F^2(Q,Rg,b_p,V,eta_s) = I0*2*(u-1+exp(-u))/u^2 with u = Q^2*Rg^2\n"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "b_p: scattering length of polymer in \[cm\]\n"}
                 3 {return "V: volume of the polymer Mw/(Na rho_p) in \[cm^3\]\n"}
                 4 {return "eta_s: scattering length density of solvent in \[cm^-2\]\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           Gauss3 { 
              switch $num {
                 0 {return "F^2(Q,Rg,eta) = I0*2*(u-1+exp(-u))/u^2 with u = Q^2*Rg^2\n"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "b_p: scattering length of polymer in \[cm\]\n"}
                 3 {return "Mw: molecular weight of polymer in \[g/mol\]\n"}
                 4 {return "rho_p: mass density of polymer in \[g/cm^3\]\n"}
                 5 {return "eta_s: scattering length density of solvent in \[cm^-2\]\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           GaussPoly { 
              switch $num {
		 0 {return "I(Q) = I0*2*((1+Ux)^(-1/U)+x-1)/((1+U)x^2)\n with U=Mw/Mn-1 and  x=Q^2*Rg^2/(1+2U)"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "Mw:weight averaged molecular weight of polymer\n"}
                 3 {return "Mn: number averaged molecular weight of polymer\n"}
                 4 {return "I0: forward scattering\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian coil" { 
              switch $num {
		 0 {return "I(Q) = I0(U^(1/(2nu)) Gamma(1/(2nu))-Gamma(1/nu)- U^(1/(2nu))Gamma(1/(2nu),U)+Gamma(1/nu,U))/(nu U^(1/nu))\nwith U = (2nu+1) (2nu+2) q^2 RG^2 / 6"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "nu: Flory exponent\ncollapsed coil nu=1/3, coil in theta-solvent nu=1/2, swollen coil nu=3/5"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian coil 2" { 
              switch $num {
		 0 {return "I(Q) = I0(U^(1/(2nu)) Gamma(1/(2nu))-Gamma(1/nu)- U^(1/(2nu))Gamma(1/(2nu),U)+Gamma(1/nu,U))/(nu U^(1/nu))\nwith U = (2nu+1) (2nu+2) q^2 RG^2 / 6"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "nu: Flory exponent\ncollapsed coil nu=1/3, coil in theta-solvent nu=1/2, swollen coil nu=3/5"}
                 3 {return "b_p: scattering length of polymer in \[cm\]\n"}
                 4 {return "V: volume of the polymer Mw/(Na rho_p) in \[cm^3\]\n"}
                 5 {return "eta_s: scattering length density of solvent in \[cm^-2\]\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian coil 3" { 
              switch $num {
		 0 {return "I(Q) = I0(U^(1/(2nu)) Gamma(1/(2nu))-Gamma(1/nu)- U^(1/(2nu))Gamma(1/(2nu),U)+Gamma(1/nu,U))/(nu U^(1/nu))\nwith U = (2nu+1) (2nu+2) q^2 RG^2 / 6"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "nu: Flory exponent\ncollapsed coil nu=1/3, coil in theta-solvent nu=1/2, swollen coil nu=3/5"}
                 3 {return "b_p: scattering length of polymer in \[cm\]\n"}
                 4 {return "Mw: molecular weight of polymer in \[g/mol\]\n"}
                 5 {return "rho_p: mass density of polymer in \[g/cm^3\]\n"}
                 6 {return "eta_s: scattering length density of solvent in \[cm^-2\]\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           FlexibleRingPolymer { 
              switch $num {
                 0 {return "I(Q,Rg,I0) = I0*sqrt(2/u^2)*dawson(sqrt(u*u/2)) with u = Q^2*Rg^2\n"}
                 1 {return "Rg: Guinier radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering I(Q=0)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           mMemberedTwistedRing { 
              switch $num {
                 0 {return "I(Q,Rg,I0) = I0*sqrt(2/u^2))*dawson(sqrt(u*u/2)) with u = Q^2*Rg^2\n"}
                 1 {return "R_G,1r: Guinier radius of one loop\n"}
                 2 {return "m: number of twists\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering I(Q=0)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           DaisyLikeRing { 
              switch $num {
                 0 {return "I(Q,Rg,I0) = I0*sqrt(2/u^2)*dawson(sqrt(u*u/2)) with u = Q^2*Rg^2\n"}
                 1 {return "R_G,1r: Guinier radius of one ring\n"}
                 2 {return "m: number of rings\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering I(Q=0)\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           BenoitStar { 
              switch $num {
                 0 {return "F^2(Q,Rg,f,eta) = I0*(2/(f*v*v)*(-1+v+exp(-v)+(f-1)*pow(1-exp(-v),2)/2))\nwith u=Rg*Rg*Q*Q and v=u*f/(3*f-2)"}
                 1 {return "Rg: radius of gyration\n"}
                 2 {return "f: number of branches\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           PolydisperseStar { 
              switch $num {
                 0 {return "F^2(Q,Rg,f,eta) = I0*(2/(f*v*v)*(-1+v+exp(-v)+(f-1)*pow(1-exp(-v),2)/2))\nwith u=Rg*Rg*Q*Q and v=u*f/(3*f-2)"}
                 1 {return "Rg: radius of gyration\n"}
                 2 {return "f: number of branches\n"}
                 3 {return "\n"}
                 4 {return "I0: forward scattering\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           JuelichMicelle { 
              switch $num {
                 0 {return "This form factor is at the moment under development. Furthermore it requires\nthat the parameter are given in the correct units, like Q - Angstrom^-1"}
                 1 {return "mcore/rcore: ratio of molecular weight and mass density \[g mol^-1 / g cm^-3\]\n"}
                 2 {return "mbrsh/rbrsh: ratio of molecular weight and mass density \[g mol^-1 / g cm^-3\]\n"}
                 3 {return "bcore: scattering length density in cm^-2\n"}
                 4 {return "bbrush: scattering length density in cm^-2\n"}
                 5 {return "Nagg: aggregation number\n"}
                 6 {return "sigma1: smearing parameter for core in Angstrom\n"}
                 7 {return "sigma3: smearing parameter for shell in Angstrom\n"}
                 8 {return "d3+: extra radius of shell3 (compared to compact) in Angstrom\n"}
                 9 {return "gamma: star-like exponent\nshould be 4/3 for star polymer, const=0"}
                10 {return "rhosolv: scattering length density of solvent cm^-2\n"}
              }
           }
           Francois1 { 
              switch $num {
                 0 {return "??\n"}
                 1 {return "Rc\n"}
                 2 {return "Rm\n"}
                 3 {return "nu:\n"}
                 4 {return "L: contour length\n"}
                 5 {return "b: Kuhn length\n"}
                 6 {return "Nagg: aggregation number\n"}
                 7 {return "Drho: contrast\n"}
                 8 {return "K: scaling parameter\n"}
                 9 {return "exvol=1 with excluded volume\nexvol=0 without excluded volume"}
                10 {return "\n"}
              }
           }
           "CopolymerMicelleR^-a" { 
              switch $num {
                 0 {return "??\n"}
                 1 {return "Rc\n"}
                 2 {return "Vc\n"}
                 3 {return "rho_c\n"}
                 4 {return "Vsh\n"}
                 5 {return "rho_sh\n"}
                 6 {return "t\n"}
                 7 {return "alpha\n"}
                 8 {return "L\n"}
                 9 {return "b\n"}
                10 {return "ro_solv\n"}
              }
           }
           CopolymerMicelleRod { 
              switch $num {
                 0 {return "Formfactor of very long cylindrical block copolymer micelles\n"}
                 1 {return "Vpolym_core: molecular volume of the block in the core \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 2 {return "Vpolym_brsh: molecular volume of the block bilding the shell \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 3 {return "eta_core: (bulk) scattering length density of block unit in core\n"}
                 4 {return "eta_brsh: (bulk) scattering length density of block unit in shell\n"}
                 5 {return "eta_solv: scattering length density of solvent\n"}
                 6 {return "xsolv_core: amount of solvent in the core\n"}
                 7 {return "Rg: radius of gyration of the chains in the shell\n"}
                 8 {return "d: should be around 1\n"}
                 9 {return "Nl: aggregation number per unit length\n"}
                10 {return "L: lenght of rodlike micelle\n"}
              }
           }
           CopolymerMicelleEll { 
              switch $num {
                 0 {return "Formfactor of an ellipsoidal block copolymer micelles\n"}
                 1 {return "Vpolym_core: molecular volume of the block in the core \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 2 {return "Vpolym_brsh: molecular volume of the block bilding the shell \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 3 {return "eta_core: (bulk) scattering length density of block unit in core\n"}
                 4 {return "eta_brsh: (bulk) scattering length density of block unit in shell\n"}
                 5 {return "eta_solv: scattering length density of solvent\n"}
                 6 {return "xsolv_core: amount of solvent in the core\n"}
                 7 {return "Rg: radius of gyration of the chains in the shell\n"}
                 8 {return "d: should be around 1\n"}
                 9 {return "Nagg: aggregation number\n"}
                10 {return "epsilon: eccentricity of ellipsoid\n"}
              }
           }
           CopolymerMicelleCyl { 
              switch $num {
                 0 {return "Formfactor of a cylindrical block copolymer micelles\n"}
                 1 {return "Vpolym_core: molecular volume of the block in the core \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 2 {return "Vpolym_brsh: molecular volume of the block bilding the shell \nin nm\^3 if Q in nm\^-1, in A\^3 if Q in A\^-1"}
                 3 {return "eta_core: (bulk) scattering length density of block unit in core\n"}
                 4 {return "eta_brsh: (bulk) scattering length density of block unit in shell\n"}
                 5 {return "eta_solv: scattering length density of solvent\n"}
                 6 {return "xsolv_core: amount of solvent in the core\n"}
                 7 {return "Rg: radius of gyration of the chains in the shell\n"}
                 8 {return "d: should be around 1\n"}
                 9 {return "Nagg: aggregation number\n"}
                10 {return "L: length of cylinder\n"}
              }
           }
           DozierStar { 
              switch $num {
                 0 {return "I(Q,I0,Rg,alpha,xi,nu): Star polymer according to Dozier\n "}
                 1 {return "I0: forward scattering\n"}
                 2 {return "Rg: Guinier radius of star\n"}
                 3 {return "alpha\n"}
                 4 {return "xi: correlation length inside star\nor equivalently the radius of the outmost blobs (xi=2Rg/sqrt(f))"}
                 5 {return "nu: Flory exponent g(r) = r^(1/nu-3)\n0.6 in good solvent, .5 in theta solvent"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           DozierStar2 { 
              switch $num {
                 0 {return "I(Q,I0,Rg,alpha,xi,nu): Star polymer according to Dozier\n "}
                 1 {return "I0: forward scattering\n"}
                 2 {return "Rg: Guinier radius of star\n"}
                 3 {return "Nagg: aggregation number\n"}
                 4 {return "xi: correlation length inside star\nor equivalently the radius of the outmost blobs (xi=2Rg/sqrt(f))"}
                 5 {return "nu: Flory exponent g(r) = r^(1/nu-3)\n0.6 in good solvent, .5 in theta solvent"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           BeacaugeExpPowLaw2 { 
              switch $num {
                 0 {return "\nUnified Exponential Power Law according to Beacauge"}
                 1 {return "G: Guinier prefactor\n"}
                 2 {return "B: prefactor specific to the type of power-law\n"}
                 3 {return "Rg_i: size of large structure\n"}
                 4 {return "Rg_i+1: size of small structure\n"}
                 5 {return "k: scaling parameter\nshould be close to 1"}
                 6 {return "4<P<3: suface fractal, P<3: mass fractal\nP>4 diffuse interface, P=2 for Gaussian polymer, P=1 for rod-like Kuhn steps"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           BeacaugeExpPowLaw { 
              switch $num {
                 0 {return "\nUnified Exponential Power Law according to Beacauge"}
                 1 {return "G: Guinier prefactor\n"}
                 2 {return "B: prefactor specific to the type of power-law\n"}
                 3 {return "Gs: Guinier prefactor\n"}
                 4 {return "Bs: prefactor specific to the type of power-law\n"}
                 5 {return "Rg: size of large-scale polymer coil\n"}
                 6 {return "Rsub: size of small-scale Kuhn steps\nsize of subunits of large-scale polymer"}
                 7 {return "Rs: size of small-scale Kuhn steps\nsize of subunits of large-scale polymer"}
                 8 {return "4<P<3: suface fractal, P<3: mass fractal\nP>4 diffuse interface, P=2 for Gaussian polymer"}
                 9 {return "Ps should be equal 1 for rod-like Kuhn steps\n"}
                10 {return "\n"}
              }
           }
           "Fisher-Burford" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = I0 pow(1+2/(3D)Q^2Rg^2,-D/2)\n"}
                 1 {return "Rg: radius of gyration (xi^2=Rg^2/3)\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Aggregate (Exp(-x^a) Cut-Off)" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0,alpha) = \n"}
                 1 {return "Rg: radius of gyration\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "alpha: exponent in cutoff function\nalpha=1 -> MassFractExp; alpa=2 -> MassFractGauss"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "DLCAggregation" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = I0 (sum\[C_i (Q^2Rg^2)^2i\])^-D/8\nC_1=0, C_1=8/(3D), C_2=2.5, C_3=-1.52, C_4=1.02"}
                 1 {return "Rg: radius of gyration\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "RLCAggregation" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = I0 (sum\[C_i (Q^2Rg^2)^2i\])^-D/8\nC_1=0, C_1=8/(3D), C_2=3.13, C_3=-2.58, C_4=0.95"}
                 1 {return "Rg: radius of gyration\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "MassFractExp" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = I0 sin((D-1)atan(Q*xi)) / ((D-1) Q xi (1+Q^2xi^2)^((D-1)/2))\nwith xi^2 = 2Rg^2/(D(D+1))"}
                 1 {return "Rg: radius of gyration (xi^2=2Rg^2/(D(D+1)))\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "MassFractGauss" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = eta^2 Rg^(2D) exp(-Q^2Rg^2/D) 1F1((3-D)/2,3/2,Q^2Rg^2/D)\n"}
                 1 {return "Rg: radius of gyration (xi^2=4Rg^2/D)\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Aggregate (OverlapSph Cut-Off)" { 
              switch $num {
                 0 {return "F^2(Q,Rg,D,I0) = eta^2 Rg^(2D) S_OverlapSph(Q)\n"}
                 1 {return "Rg: radius of gyration (xi^2=Rg^2(D+2)(D+5)/(2D(D+1))\n"}
                 2 {return "D: fractal dimension\n"}
                 3 {return "I0: forward scattering\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "LangevinMH" { 
              switch $num {
                 0 {return "F^2(B,R,Ms,T,M8) = M8 4/3 pi R^3 (1/tanh(alpha)-1/alpha)\nalpha = B*Ms* 4/3 pi R^3 /(kT), B=Gauss"}
                 1 {return "R: radius of spheres, nm\n"}
                 2 {return "Ms: particle magnetization, A/m\n"}
                 3 {return "T: temperature, K\n"}
                 4 {return "M8: saturation magnetization, emu\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "A(B)" { 
              switch $num {
                 0 {return "\n"}
                 1 {return "\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "B(B)" { 
              switch $num {
                 0 {return "\n"}
                 1 {return "\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "P39" { 
              switch $num {
                 0 {return "\n"}
                 1 {return "\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "SuperParStroboPsi" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "R: particle radius\n"}
                 4 {return "R_av: average particle radius for which mu_kT is given\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsi2" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "Dl_l: wavelength distribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboTISANE1" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "Dt: time resolution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQ" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQBt1" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQLx" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQL2x" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQL2x_SANSPOL" {
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\nThe wavelength has been fixed to 6 Angstrom"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "pol: incomming neutron polarization\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQL2x_SANSPOL_albr" {
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\nThe wavelength has been fixed to 6 Angstrom"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "pol: incomming neutron polarization\n"}
                 9 {return "d_phi: delta phi\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "SuperParStroboPsiSQL2x_SANSPOL_albr_stat" {
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\nThe wavelength has been fixed to 6 Angstrom"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "pol: incomming neutron polarization\n"}
                 9 {return "d_phi: delta phi\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
	   }
           "SuperParStroboPsiSQL2modx_SANSPOL" {
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\nThe wavelength has been fixed to 6 Angstrom"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "SQ: structure factor contribution\n"}
                 4 {return "B1: additional constant magnetic field\n"}
                 5 {return "mu_kT: ratio of magnetic moment of particle\nto thermal energy"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "pol: incomming neutron polarization\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "hysteresesStroboPsi" { 
              switch $num {
                 0 {return "superparamagnetic particles in an oscillating magnetic field\n"}
                 1 {return "phi0: phase shift, deg\n"}
                 2 {return "Knuc_Kmag: ratio of nuclear-to-magnetic contrast, 1\n"}
                 3 {return "Dl_l: wavelength distribution\n"}
                 4 {return "Bc: coercife field, Tesla\n"}
                 5 {return "S: shape parameter for hysteris loop\n"}
                 6 {return "f: frequency of oscillating magnetic field, Hz\n"}
                 7 {return "B0: amplitude of oscillating magnetic field, Tesla\n"}
                 8 {return "lambda: neutron wavelength, Angstrom\n"}
                 9 {return "SD: sample detector distance, m\n"}
                10 {return "Psi: angle between Q and B, deg\n"}
              }
           }
           "MieSphere" { 
              switch $num {
                 0 {return "MieSphere = scattering intensity of a homogenous sphere in Mie approximation\n"}
                 1 {return "R: radius of spheres, nm\n"}
                 2 {return "lam_sol: wavelength of light in solvent, nm\n"}
                 3 {return "m_re: real part of relative refraction index\n"}
                 4 {return "m_im: imagninary part of relative refraction index\n"}
                 5 {return "pol=0: unpolarized light\npol>0 (pol<0): parallel (perpendicular) to scattering plane polarized light"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "MieShell" { 
              switch $num {
                 0 {return "MieShell = scattering intensity of a spherical shell in Mie approximation\n"}
                 1 {return "R: radius of spheres, nm\n"}
                 2 {return "dR: thickness of shell, nm\n"}
                 3 {return "lam_sol: wavelength of light in solvent, nm\n"}
                 4 {return "mc_re: real part of relative refraction index of core\n"}
                 5 {return "mc_im: imagninary part of relative refraction index of core\n"}
                 6 {return "ms_re: real part of relative refraction index of shell\n"}
                 7 {return "ms_im: imagninary part of relative refraction index of shell\n"}
                 8 {return "pol=0: unpolarized light\npol>0 (pol<0): parallel (perpendicular) to scattering plane polarized light"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Beta (Amplitude)" { 
              switch $num {
                 0 {return "Beta (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "xmax: \n"}
                 3 {return "xmax: \n"}
                 4 {return "alpha: first shape parameter \n"}
                 5 {return "beta: second shape parameter\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Beta (Area)" { 
              switch $num {
                 0 {return "Beta (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "xmax: \n"}
                 3 {return "xmax: \n"}
                 4 {return "alpha: first shape parameter \n"}
                 5 {return "beta: second shape parameter\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Chi-squared (Amplitude)" { 
              switch $num {
                 0 {return "Chi-squared (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter (mode)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape>2)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Chi-squared (Area)" { 
              switch $num {
                 0 {return "Chi-squared (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (mode)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape>2)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Erfc (Amplitude)" { 
              switch $num {
                 0 {return "Erfc Peak (Amplitude)\ny(x)=amplitude*erfc(((x-center)/width)^2)"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Erfc (Area)" { 
              switch $num {
                 0 {return "Erfc Peak (Area)\ny(x)=amplitude*erfc(((x-center)/width)^2)"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Error (Amplitude)" { 
              switch $num {
                 0 {return "Error (Amplitude)\ny(x)=amplitude*exp(-1/2*(x-center)^(2/shape)/width)"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Error (Area)" { 
              switch $num {
		 0 {return "Erfc Peak (Area), y(x)=area/normconst*erfc(((x-center)/width)^2)\nwith area = int(y(x),x=-infinity..infinity))"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "exponentially modified Gaussian (Amplitude)" { 
              switch $num {
		 0 {return "exponentially modified Gaussian (Amplitude) EMG\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "exponentially modified Gaussian (Area)" { 
              switch $num {
                 0 {return "exponentially modified Gaussian (Area) EMG\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width>0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Extreme Value (Amplitude)" { 
              switch $num {
		 0 {return "Extreme Value (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Extreme Value (Area)" { 
              switch $num {
                 0 {return "Extreme Value (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width>0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "fatique life (Area)" { 
              switch $num {
                 0 {return "fatique life (Area)\n"}
                 1 {return "area: = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (mode)\n"}
                 3 {return "beta: scale parameter (width > 0)\n"}
                 4 {return "gamma: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "F-variance (Amplitude)" { 
              switch $num {
                 0 {return "F-variance (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter (mode)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape1: shape1 parameter (shape1 > 2)\n"}
                 5 {return "shape2: shape2 parameter (shape2 > 2)\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "F-variance (Area)" { 
              switch $num {
                 0 {return "F-variance (Area)\n"}
                 1 {return "area: = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (mode)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape1: shape1 parameter (shape1 > 2)\n"}
                 5 {return "shape2: shape2 parameter (shape2 > 2)\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gamma (Amplitude)" { 
              switch $num {
                 0 {return "Gamma (Amplitude)\ny(x)=amplitude*exp(-z)*pow((z+shape-1.)/(shape-1.),shape-1.) with z=(x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gamma (Area)" { 
              switch $num {
                 0 {return "Gamma (Amplitude)\ny(x)=A0*exp(-z)*pow((z+shape-1.)/(shape-1.),shape-1.) with z=(x-center)/width+shape-1 and A0=area/(width*GAMMA(shape))"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian (Amplitude)" { 
              switch $num {
                 0 {return "Gaussian (Amplitude)\ny(x)=amplitude*exp(-z^2/2.)+backgr with z=(x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian (Area)" { 
              switch $num {
                 0 {return "Gaussian (Area)\ny(x)=area/(sqrt(2*PI)*width)*exp(-z^2/2)+backgr with z=(x-center)/width"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian-Lorentzian cross product (Amplitude)" { 
              switch $num {
                 0 {return "Gaussian-Lorentzian cross product (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (0 <= shape <= 1)\nshape==0: Gaussian,  shape==1: Lorentzian"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian-Lorentzian cross product (Area)" { 
              switch $num {
                 0 {return "Gaussian-Lorentzian cross product (Area)\n"}
                 1 {return "area: maximum area\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (0 <= shape <= 1)\nshape==0: Gaussian,  shape==1: Lorentzian"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian-Lorentzian sum (Amplitude)" { 
              switch $num {
                 0 {return "Gaussian-Lorentzian sum (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (0 <= shape <= 1)\nshape==0: Gaussian,  shape==1: Lorentzian"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Gaussian-Lorentzian sum (Area)" { 
              switch $num {
                 0 {return "Gaussian-Lorentzian sum (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (0 <= shape <= 1)\nshape==0: Gaussian,  shape==1: Lorentzian"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian 1 (Amplitude)" { 
              switch $num {
                 0 {return "generalized Gaussian 1 (Amplitude)\ny(x)=amplitude*exp(-z^shape)+backgr with z=abs((x-center)/width)"}
                 1 {return "amplitude = y(center))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian 1 (Area)" { 
              switch $num {
                 0 {return "generalized Gaussian 1 (Area)\ny(x)=area/(2*width*Gamma(1/shape))*width)*exp(-z^shape)+backgr with z=abs((x-center)/width)"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian 2 (Amplitude)" { 
              switch $num {
                 0 {return "generalized Gaussian 2 (Amplitude)\ny(x)=amplitude scale*exp(-shape^2/2) phi(z)/{width-shape(x-location)}+backgr with phi(z)=exp(-z^2/2)/sqrt(2pi) z=-1/shapelog(1-shape(x-location)/width)"}
                 1 {return "amplitude = y(center))\n"}
                 2 {return "location: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "generalized Gaussian 2 (Area)" { 
              switch $num {
		  0 {return "generalized Gaussian 2 (Area)\ny(x)=area phi(z)/{width-shape(x-location)}+backgr with phi(z)=exp(-z^2/2)/sqrt(2pi) z=-1/shapelog(1-shape(x-location)/width)"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "location: location parameter \n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Giddings (Amplitude)" { 
              switch $num {
                 0 {return "Giddings (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Giddings (Area)" { 
              switch $num {
                 0 {return "Giddings (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Haarhoff Van der Linde (Amplitude)" { 
              switch $num {
                 0 {return "Haarhoff Van der Linde (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter (center > 0)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Haarhoff Van der Linde (Area)" { 
              switch $num {
                 0 {return "Haarhoff Van der Linde (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (center > 0)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "half Gaussian modified Gaussian (Amplitude)" { 
              switch $num {
                 0 {return "half Gaussian modified Gaussian (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "half Gaussian modified Gaussian (Area)" { 
              switch $num {
                 0 {return "half Gaussian modified Gaussian (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "distortion: distortion parameter (distortion != 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Inverted Gamma sum (Amplitude)" { 
              switch $num {
                 0 {return "Inverted Gamma (Amplitude)\nbckgr+ampl*width*exp(z*(shape+1)^2/u)*(u/width)^(-shape)/u with u = shape*x+x+width-center*shape-center, z = x-center"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Inverted Gamma (Area)" { 
              switch $num {
                 0 {return "Inverted Gamma (Area)\nbckgr+ampl*(shape+1)*exp(-width*(shape+1)/u)*(width*(shape+1)/u)^shape/u/GAMMA(shape) with u = shape*x+x+width-center*shape-center, z = x-center"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Kumaraswamy (Amplitude)" { 
              switch $num {
                 0 {return "Kumaraswamy (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "xmax: \n"}
                 3 {return "xmax: \n"}
                 4 {return "alpha: first shape parameter \n"}
                 5 {return "beta: second shape parameter\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Kumaraswamy (Area)" { 
              switch $num {
                 0 {return "Kumaraswamy (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "xmax: \n"}
                 3 {return "xmax: \n"}
                 4 {return "alpha: first shape parameter \n"}
                 5 {return "beta: second shape parameter\n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Laplace (Amplitude)" { 
              switch $num {
                 0 {return "Laplace (Amplitude)\ny(x)=bckgr+A0*exp(-z) with z = sqrt(2)*fabs(x-center)/width and A0=amplitude"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Laplace (Area)" { 
              switch $num {
                 0 {return "Laplace (Area)\ny(x)=bckgr+A0*exp(-z) with z = sqrt(2)*fabs(x-center)/width and A0=area/(sqrt(2)*width)"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Logistic (Amplitude)" { 
              switch $num {
                 0 {return "Logistic (Amplitude)\ny(x)=backgr+4*amplitude*exp(-z)/pow(1+exp(-z),2) with z = (x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "x0: location parameter\n"}
                 3 {return "sigma: scale parameter (sigma > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Logistic (Area)" { 
              switch $num {
                 0 {return "Logistic (Area)\ny(x)=backgr+area/width*exp(-z)/pow(1+exp(-z),2) with z = (x-center)/width"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "x0: location parameter\n"}
                 3 {return "sigma: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "LogLogistic (Amplitude)" { 
              switch $num {
                 0 {return "LogLogistic (Amplitude)\ny(x)=backgr+"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "x0: location parameter\n"}
                 3 {return "mu: scale parameter\n"}
                 4 {return "sigma: shape parameter (0<sigma<1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "LogLogistic (Area)" { 
              switch $num {
                 0 {return "LogLogistic (Area)\ny(x)=backgr+area"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "x0: location parameter\n"}
                 3 {return "mu: scale parameter\n"}
                 4 {return "sigma: shape parameter (0<sigma<1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "LogNormal, 4 parameters (Amplitude)" { 
              switch $num {
                 0 {return "LogNormal, 4 parameters (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "LogNormal, 4 parameters (Area)" { 
              switch $num {
                 0 {return "LogNormal, 4 parameters (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "LogNormal (Amplitude)" { 
              switch $num {
                 0 {return "LogNormal (Amplitude)\ny(x)=backgr + ampl*exp(-0.5*pow(log(x/center)/width,2))"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "mu: location parameter (mu > x0)\n"}
                 3 {return "sigma: width parameter (sigma > 0)\n"}
                 4 {return "x0: shift of the peak position\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "LogNormal (Area)" { 
              switch $num {
                 0 {return "LogNormal (Area)\ny(x)=backgr+area/(sqrt(2.*M_PI)*center*width*sqrt(exp(width*width)))*exp(-0.5*pow(log(x/center)/width,2))"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "mu: location parameter (mu > x0)\n"}
                 3 {return "sigma: width parameter (sigma > 0)\n"}
                 4 {return "x0: shift of the peak position\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Pearson IV (Amplitude)" { 
              switch $num {
                 0 {return "Pearson IV (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Pearson IV (Area)" { 
              switch $num {
                 0 {return "Pearson IV (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Pearson VII (Amplitude)" { 
              switch $num {
                 0 {return "Pearson VII (Amplitude)\ny(x)=bckgr+amplitude/pow(1+4*z*z*(pow(2,1/shape)-1),shape) with z = (x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Pearson VII (Area)" { 
              switch $num {
                 0 {return "Pearson VII (Area)\nny(x)=bckgr+A0/pow(1+4*z*z*(pow(2,1/shape)-1),shape) with z = (x-center)/width"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0.5)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "power LogNormal (Area)" { 
              switch $num {
                 0 {return "power LogNormal (Area)\ny(x)=bckgr+area*p/(z*sigm*shape)*phi(log(z)/shape)*Phi(-Log(z)/shape)^(p-1)\nwith z = (x-center)/width"}
                 1 {return "area:  int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width != 0)\n"}
                 4 {return "shape: shape parameter (shape != 0)\n"}
                 5 {return "p:power parameter (p>0) \n"}
                 6 {return "backgr.: constant offset\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "power Normal (Area)" { 
              switch $num {
                 0 {return "power Normal (Area)\ny(x)=bckgr+area*p/(sigma)*phi(z)*Phi(-z)^(p-1)\nwith z = (x-center)/width"}
                 1 {return "area:  int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "shape: shape parameter (shape != 0)\n"}
                 4 {return "p:power parameter (p>0) \n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "pulse (Amplitude)" { 
              switch $num {
                 0 {return "pulse (Amplitude)\ny(x)=bckgr+amplitude/pow(1+4*z*z*(pow(2,1/shape)-1),shape) with z = (x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "pulse (Area)" { 
              switch $num {
                 0 {return "pulse (Area)\ny(x)=bckgr+4*A0*exp(-z)*(1-exp(-z)) for z>0 else y(x)=backgr, z = (x-center)/width"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "backgr.: constant offset\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "pulse with 2nd width (Amplitude)" { 
              switch $num {
                 0 {return "pulse with 2nd width (Amplitude)\ny(x)=bckgr+ampl*pow(u,-width1/width2)/(1-u)*exp(-z2)*(1-exp(-z1)) with  u=width1/(width1+width2), z1=(x-center)/width1, z2=(x-center)/width2"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter (pulse initiation)\n"}
                 3 {return "width1: scale parameter (width1 > 0)\n"}
                 4 {return "width2: scale parameter (width2 > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "pulse with 2nd width (Area)" { 
              switch $num {
                 0 {return "pulse with 2nd width (Area)\ny(x)=bckgr+area*width2*width2/(width1+width2)*exp(-z2)*(1-exp(-z1)) with z1=(x-center)/width1 and z2=(x-center)/width2"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (pulse initiation)\n"}
                 3 {return "width1: scale parameter (width1 > 0)\n"}
                 4 {return "width2: scale parameter (width2 > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "pulse with power term (Amplitude)" { 
              switch $num {
                 0 {return "pulse with power term (Amplitude)\ny(x)=bckgr+A0*exp(-z)*pow(1-exp(-z),shape)with A0=ampl/(pow(shape,shape)*pow(shape+1,-(shape+1))) and z=(x-center)/width, y(x)=backr for z<0"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter (pulse initiation)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "pulse with power term (Area)" { 
              switch $num {
                 0 {return "pulse with power term (Area)\ny(x)=bckgr+A0*exp(-z)*pow(1-exp(-z),shape)with A0=area*(shape+1)/width and z=(x-center)/width, y(x)=backr for z<0"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter (pulse initiation)\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Student-t (Amplitude)" { 
              switch $num {
                 0 {return "Student-t (Amplitude)\ny(x)=bckgr+ampl*(1+z*z/shape)^(-(shape+1)/2) with z=(x-center)/width"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Student-t (Area)" { 
              switch $num {
                 0 {return "Student-t (Area)\ny(x)=bckgr+A0*(1+z*z/shape)^(-(shape+1)/2) with z=(x-center)/width z=(x-center)/width"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Voigt (Amplitude)" { 
              switch $num {
                 0 {return "Voigt (Amplitude)\n"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "sigma: width of Gaussian (Doppler) profile (sigma > 0)\n"}
                 4 {return "gamma: width of Lorentzian profile (gamma > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Voigt (Area)" { 
              switch $num {
                 0 {return "Voigt (Area)\n"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "sigma: width of Gaussian (Doppler) profile (sigma > 0)\n"}
                 4 {return "gamma: width of Lorentzian profile (gamma > 0)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Weibull (Amplitude)" { 
              switch $num {
                 0 {return "Weibull (Amplitude)\ny(x)=bckgr+ampl*u^(-u)*z^(shape-1)*exp(-z^shape+u) with u=(shape-1)/shape and z=(x-center)/width+u^(1/shape)"}
                 1 {return "amplitude: maximum amplitude\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           } 
           "Weibull (Area)" { 
              switch $num {
                 0 {return "Weibull (Area)\ny(x)=bckgr+area*shape/width^shape*(z*width)^(shape-1)*exp(-z^shape+u) with u=(shape-1)/shape and z=(x-center)/width+u^(1/shape)"}
                 1 {return "area = int(y(x),x=-infinity..infinity))\n"}
                 2 {return "center: location parameter\n"}
                 3 {return "width: scale parameter (width > 0)\n"}
                 4 {return "shape: shape parameter (shape > 1)\n"}
                 5 {return "backgr.: constant offset\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "DLS_Sphere_RDG" { 
              switch $num {
                 0 {return "DLS_Sphere_RDG(t,R,eta,T,Q) = \n"}
                 1 {return "R: radius of spheres, nm\n"}
                 2 {return "eta: viscosity of solvent, 1 cP = 0.001 N s m-2\n"}
                 3 {return "T: temperature, K\n"}
                 4 {return "Q: scattering vector, nm-1\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
	  }
          "Robertus2" { 
              switch $num {
                 0 {return "Robertus2 = StickyHardSphere Model for polydisperse spherical shell\n"}
                 1 {return "Rmean: mean particle size\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "sigma: width of Schultz distribution\n"}
                 4 {return "tau: stickiness parameter\n"}
                 5 {return "xfp2: relative volume fraction of the larger particle fraction with R = 2*Rmean\n"}
                 6 {return "fp: volume fraction\n"}
                 7 {return "eta_core: scattering length density of core\n"}
                 8 {return "eta_shell: scattering length density of shell\n"}
                 9 {return "x_solvent: amount of solvent in shell\n"}
                10 {return "eta_solvent: scattering length density of solvent\n"}
              }
	  }
          "Robertus1" { 
              switch $num {
                 0 {return "Robertus1 = StickyHardSphere Model for polydisperse spherical shell\nThe stickiness parameter tau=btau(1+2/pi atan(atau(ri-Rmean))) can be size dependent"}
                 1 {return "Rmean: mean particle size\n"}
                 2 {return "dR: shell thickness\n"}
                 3 {return "sigma: width of Schultz distribution\n"}
                 4 {return "btau: stickiness parameter\ntau=btau*(1+2/pi*atan(atau*(ri-rm)))"}
                 5 {return "atau: if tau is independent of radius set atau=0\ntau=btau*(1+2/pi*atan(atau*(ri-Rmean)))"}
                 6 {return "fp: volume fraction\n"}
                 7 {return "eta_core: scattering length density of core\n"}
                 8 {return "eta_shell: scattering length density of shell\n"}
                 9 {return "x_solvent: amount of solvent in shell\n"}
                10 {return "eta_solvent: scattering length density of solvent\n"}
              }
	  }
          "WormLikeChainEXV" { 
              switch $num {
                 0 {return "Worm like chain with excluded volume\n"}
                 1 {return "S0: forward scattering\n"}
                 2 {return "RL: contour length\n"}
                 3 {return "RRL: Kuhn length, K\n"}
                 4 {return "R: cross-section radius\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
          "KholodenkoWorm" { 
              switch $num {
                 0 {return "Worm like chain with uniform circular cross-section\n"}
                 1 {return "R: cross-section radius\n"}
                 2 {return "l: Kuhn length\n"}
                 3 {return "L: contour length\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
          "SphereWithGaussChains" { 
              switch $num {
                 0 {return "Form factor of spherical block copolymer micelles:\nA dense spherical core surrounded by a shell of dissolved polymer chains"}
                 1 {return "R: core radius\n"}
                 2 {return "Rg: gyration radius of polymer chain\n"}
                 3 {return "d: ?? set to 1\n"}
                 4 {return "Nagg: aggregation number\n"}
                 5 {return "rc : excess scattering length of the block in the chain\n"}
                 6 {return "rs : excess scattering length of the block in the core\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
          "BlockCopolymerMicelle" { 
              switch $num {
                 0 {return "Form factor of spherical block copolymer micelles:\nA dense spherical core surrounded by a shell of dissolved polymer chains"}
                 1 {return "Vpolym,c: volume of a block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 2 {return "xsolv_c: amount of solvent inside the core, value should be within \[0,1)\n"}
                 3 {return "Vpolym,sh:  volume of a block unit in the shell\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_polym,c: scattering length density block units in core\n"}
                 5 {return "eta_polym,sh: scattering length density of block units in shell\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "Nagg: aggregation number\n"}
		 8 {return "Rg: gyration radius of polymer chains in the shell\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "\n"}
              }
           }
          "SPHERE+Chains(RW)" {
              switch $num {
                 0 {return "sphere with Gaussian chains attached\nin contrast to the form factor *_Rc and *_Nagg this one does not necassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the block unit in the spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "\n"}
              }
           }
          "SPHERE+Chains(RW)_Rc" {
              switch $num {
                 0 {return "sphere with Gaussian chains attached, the micelle consists of block copolymers\nthe aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "\n"}
              }
           }
          "SPHERE+Chains(RW)_Nagg" {
              switch $num {
                 0 {return "sphere with Gaussian chains attached, the micelle consists of block copolymers\nthe core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "N_agg: aggregation number of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "\n"}
              }
           }
          "DISC+Chains(RW)" {
              switch $num {
                 0 {return "disc with Gaussian chains attached\nin contrast to the form factor *_Lc and *_nagg this one does not necassary consist of copolymers"}
                 1 {return "L_core: thickness of the disc core\n"}
                 2 {return "nagg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the block unit in the spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "D: disc diameter\n"}
              }
           }
          "DISC+Chains(RW)_Lc" {
              switch $num {
                 0 {return "disc with Gaussian chains attached, the micelle consists of block copolymers\nthe aggregation number Nagg is calculated from the core thickness L_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "L_core: thickness of the disc core\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "D: disc diameter\n"}
              }
           }
          "DISC+Chains(RW)_nagg" {
              switch $num {
                 0 {return "disc with Gaussian chains attached, the micelle consists of block copolymers\nthe core thickness L_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "nagg: specific aggregation number\nnumber of chains per surface area"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "D: disc diameter\n"}
              }
           }
          "ELL+Chains(RW)" {
              switch $num {
                 0 {return "ellipsoid with Gaussian chains attached\nin contrast to the form factor *_Rc and *_Nagg this one does not necassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "epsilon: eccentricity of the ellipsoid (R,R,epsilon*R)\n"}
              }
           }
          "ELL+Chains(RW)_Rc" {
              switch $num {
                 0 {return "ellipsoid with Gaussian chains attached, the micelle consists of block copolymers\nthe aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "epsilon: eccentricity of the ellipsoid (R,R,epsilon*R)\n"}
              }
           }
          "ELL+Chains(RW)_Nagg" {
              switch $num {
                 0 {return "ellipsoid with Gaussian chains attached, the micelle consists of block copolymers\nthe core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "N_agg: aggregation number of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "epsilon: eccentricity of the ellipsoid (R,R,epsilon*R)\n"}
              }
           }
          "CYL+Chains(RW)" {
              switch $num {
                 0 {return "cylinder with Gaussian chains attached\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: height of cylinder\n"}
              }
           }
          "CYL+Chains(RW)_Rc" {
              switch $num {
                 0 {return "cylinder with Gaussian chains attached, the micelle consists of block copolymers\nNagg is calculated from R_core, L, and the molecular volume V_core of the copolymer block unit in the core and "}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: height of cylinder\n"}
              }
           }
          "CYL+Chains(RW)_Nagg" {
              switch $num {
                 0 {return "cylinder with Gaussian chains attached, the micelle consists of block copolymers\nR_core is calculated from Nagg, the molecular volume V_core of the copolymer block unit in the core and L"}
                 1 {return "N_agg: aggregation number of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: height of cylinder\n"}
              }
           }
          "ROD+Chains(RW)" {
              switch $num {
                 0 {return "approximation for very long cylinder with Gaussian chains attached\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: height of cylinder\n"}
              }
           }
          "ROD+Chains(RW)_Rc" {
              switch $num {
                 0 {return "approximation for very long cylinder with Gaussian chains attached, the micelle consists of block copolymers\nn_agg is calculated from R_core, H, and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: height of cylinder\n"}
              }
           }
          "ROD+Chains(RW)_nagg" {
              switch $num {
                 0 {return "approximation for cylinder with Gaussian chains attached, the micelle consists of block copolymers\nR_core is calculated from n_agg, the molecular volume V_core of the copolymer block unit in the core and H"}
                 1 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "d: value should be around 1\nnon-penetration of the chains into the core is mimicked by d~1 for R_core>>Rg"}
                10 {return "H: heigth of cylinder\n"}
              }
           }
          "WORM+Chains(RW)" {
              switch $num {
                 0 {return "approximation for wormlike micelle with homogeneous core with Gaussian chains attached\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "l: Kuhn length of a segment (l=2a)\n"}
                10 {return "L: contour length\n"}
              }
           }
          "WORM+Chains(RW)_Rc" {
              switch $num {
                 0 {return "approximation for wormlike micelle with homogeneous core with Gaussian chains attached, the micelle consists of block copolymers\nn_agg is calculated from R_core, L, and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "l: Kuhn length of a segment (l=2a)\n"}
                10 {return "L: contour length\n"}
              }
           }
          "WORM+Chains(RW)_nagg" {
              switch $num {
                 0 {return "approximation for wormlike micelle with homogeneous core with Gaussian chains attached, the micelle consists of block copolymers\nR_core is calculated from n_agg, the molecular volume V_core of the copolymer block unit in the core and L"}
                 1 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "l: Kuhn length of a segment (l=2a)\n"}
                10 {return "L: contour length\n"}
              }
           }
          "SPHERE+Chains(SAW)" {
              switch $num {
                 0 {return "sphere with corona of semi-flexible interacting self-avoiding chains\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "L: contour length polymer chain in corona\n(Rg^2=L*b/6 for RW and Rg^2=Lb/6-(b/2)^2+b^3/(4L)(1+b/(2L) exp(-2L/b)-1) for OSF theory (SAW)"}
                10 {return "b: Kuhn length of polymer chain in corona\n"}
              }
           }
          "SPHERE+Chains(SAW)_Rc" {
              switch $num {
                 0 {return "sphere with corona of semi-flexible interacting self-avoiding chains\nthe aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "L: contour length polymer chain in corona\n(Rg^2=L*b/6 for RW and Rg^2=Lb/6-(b/2)^2+b^3/(4L)(1+b/(2L) exp(-2L/b)-1) for OSF theory (SAW)"}
                10 {return "b: Kuhn length of polymer chain in corona\n"}
              }
           }
          "SPHERE+Chains(SAW)_Nagg" {
              switch $num {
                 0 {return "sphere with corona of semi-flexible interacting self-avoiding chains\nthe core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "N_agg: aggregation number of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in core\n"}
		 8 {return "Rg: gyration radius of polymer chains in the corona\n"}
                 9 {return "L: contour length polymer chain in corona\n(Rg^2=L*b/6 for RW and Rg^2=Lb/6-(b/2)^2+b^3/(4L)(1+b/(2L) exp(-2L/b)-1) for OSF theory (SAW)"}
                10 {return "b: Kuhn length of polymer chain in corona\n"}
              }
           }
          "SPHERE+Exp_Rc" {
              switch $num {
                 0 {return "sphere with a radial profile of \nthe aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "\n"}
                 8 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell"}
		 9 {return "t: shell thickness\n"}
                 10 {return "x_out:\n"}
              }
           }
          "SPHERE+R^-a" {
              switch $num {
                 0 {return "sphere with a radial profile r^-alpha\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell"}
		 8 {return "t: shell thickness\n"}
                 9 {return "L: contour length of polymer in shell\n"}
                10 {return "b: Kuhn length of polymer in shell\n"}
              }
           }
          "SPHERE+R^-a_Rc" {
              switch $num {
                 0 {return "sphere with a radial profile of r^-alpha, the micelle consists of block copolymers\nthe aggregation number Nagg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell"}
		 8 {return "t: shell thickness\n"}
                 9 {return "L: contour length of polymer in shell\n"}
                10 {return "b: Kuhn length of polymer in shell\n"}
              }
           }
          "SPHERE+R^-a_Nagg" {
              switch $num {
                 0 {return "sphere with a radial profile r^-alpha, the micelle consists of block copolymers\nthe core radius R_core is calculated from the aggregation number Nagg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "N_agg: aggregation number of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of spherical core\n"}
                 5 {return "eta_brush: scattering length density of a Gaussian chain in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=2: polyelectrolyte, alpha=4/3: star-like polymer, alpha=0: compact shell"}
		 8 {return "t: shell thickness\n"}
                 9 {return "L: contour length of polymer in shell\n"}
                10 {return "b: Kuhn length of polymer in shell\n"}
              }
           }
          "ROD+R^-a" {
              switch $num {
                 0 {return "rod with a radial profile r^-alpha\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in the core\n"}
                 8 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=1: polyelectrolyte, alpha=0: compact shell"}
		 9 {return "t: shell thickness\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+R^-a_Rc" {
              switch $num {
                 0 {return "rod with a radial profile of r^-alpha, the micelle consists of block copolymers\nthe specific aggregation number n_agg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in the core\n"}
                 8 {return "alpha: exponent of radial profile (r^-alpha)\nalpha=1: polyelectrolyte, alpha=0: compact shell"}
		 9 {return "t: shell thickness\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Gauss" {
              switch $num {
                 0 {return "rod with a constant plus Gaussian radial profile\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in the core\n"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "sigma: width of gaussian part of shell\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Gauss_Rc" {
              switch $num {
                 0 {return "rod with a constant plus Gaussian radial profile, the micelle consists of block copolymers\nthe specific aggregation number n_agg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in the core\n"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "sigma: width of gaussian part of shell\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Gauss_nagg" {
              switch $num {
                 0 {return "rod with a constant plus Gaussian radial profile, the micelle consists of block copolymers\nthe core radius R_core is calculated from the specific aggregation number n_agg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "n_agg: aggregation number density of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "xsolv_core: amount of solvent in the core\n"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "sigma: width of gaussian part of shell\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Exp" {
              switch $num {
                 0 {return "rod with a Exp-type radial profile\nin contrast to the form factor *_Rc and *_Nagg this one does not neccassary consist of copolymers"}
                 1 {return "R_core: core radius\n"}
                 2 {return "n_agg: specific aggregation number\nnumber of chains per surface area"}
                 3 {return "V_brush: molecular volume of single chain in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "ro: relative offset of scattering length density profile\n ro = eta_(sh,out)/eta_(sh,in)"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "alpha: shape parameter for the Exp-type profile\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Exp_Rc" {
              switch $num {
                 0 {return "rod with a Exp-type radial profile, the micelle consists of block copolymers\nthe specific aggregation number n_agg is calculated from the core radius R_core and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "R_core: core radius\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "ro: relative offset of scattering length density profile\n ro = eta_(sh,out)/eta_(sh,in)"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "alpha: shape parameter for the Exp-type profile\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ROD+Exp_nagg" {
              switch $num {
                 0 {return "rod with a Exp-type radial profile, the micelle consists of block copolymers\nthe core radius R_core is calculated from the specific aggregation number n_agg and the molecular volume V_core of the copolymer block unit in the core"}
                 1 {return "n_agg: aggregation number density of copolymer molecules per micelle\n"}
                 2 {return "V_core: molecular volume of the copolymer block unit inside core\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 3 {return "V_brush: molecular volume of the copolymer block unit in corona\n in \[nm^3\] for Q in nm^-1 and in \[A^3\] for Q in A^-1"}
                 4 {return "eta_core: scattering length density of the core of the rod\n"}
                 5 {return "eta_brush: scattering length density of the chains in corona\n"}
                 6 {return "eta_sol: scattering length density of solvent\n"}
                 7 {return "ro: relative offset of scattering length density profile\n ro = eta_(sh,out)/eta_(sh,in)"}
		 8 {return "t: thickness of homogenous part of shell\n"}
                 9 {return "alpha: shape parameter for the Exp-type profile\n"}
                10 {return "H: height of rod\n"}
              }
           }
          "ShearedCylinder" { 
              switch $num {
                 0 {return "cylinder under shear\n"}
                 1 {return "R: cylinder radius\n"}
                 2 {return "L: cylinder length\n"}
                 3 {return "Gamma: G/Dr orientation parameter\n"}
                 4 {return "psi: scattering direction\n(0:parallel,pi/2:perpendicular"}
                 5 {return "eta: scattering contrast\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
          "ShearedCylGauss" { 
              switch $num {
                 0 {return "cylinder under shear\nwith Gaussian orientation distribution"}
                 1 {return "R: cylinder radius\n"}
                 2 {return "L: cylinder length\n"}
                 3 {return "theta_b: orientation parameter\n"}
                 4 {return "psi: scattering direction\n(0:parallel,pi/2:perpendicular"}
                 5 {return "eta: scattering contrast\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Andrea1" { 
              switch $num {
                 0 {return "F^2(Q,R,dR,c,core) = I_sphereshell(Q,R,dR,c,core)\n"}
                 1 {return "R: radius of spheres\n"}
                 2 {return "dR: thickness of first shell\n"}
                 3 {return "c: amount of H2O\n"}
                 4 {return "core: scattering lengths density of core\n"}
                 5 {return "dR2: thickness of outer shell\n"}
                 6 {return "shell1: scattering length density of shell 1\n"}
                 7 {return "shell2: scattering length density of shell 2\n"}
                 8 {return "c2: amount of solvent in shell 2\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Simon1" { 
              switch $num {
                 0 {return "spherical shell with Gaussian chains attached\nmass fraction ratio of Solutol:Capriol:Miglyol=60:20:20 "}
                 1 {return "Nagg: aggregation number of Solutol molecules per vesicle\n"}
                 2 {return "Rg: radius of gyration of hydrophilic part of Solutol\n"}
                 3 {return "d: correction factor should be close to 1\n"}
                 4 {return "rho_solv: scattering lengths density of solvent\n"}
                 5 {return "rho_core: scattering length density of Miglyol core\n"}
                 6 {return "b_A_h: scattering length of hydrophilic part of Solutol\n"}
                 7 {return "V_A_h: Volume of hydrophilic part of Solutol\n"}
                 8 {return "b_A_l: scattering length of lipophilic part of Solutol\n"}
                 9 {return "V_A_l: volume of lipophilic part of Solutol\n"}
                10 {return "rho_B: scattering length density of Capriol\n"}
              }
           }
           "DoubleShell_withSD" { 
              switch $num {
                 0 {return "F^2(Q,R,s,...) analytical expression for spherical double shells\nwith a size distribution"}
                 1 {return "R: mean radius of core\n"}
                 2 {return "s: width of size distribution\n"}
                 3 {return "eta_c: scattering length density of core\n"}
                 4 {return "eta_1: scattering length density of first shell\n"}
                 5 {return "eta_2: scattering length density of second shell\n"}
                 6 {return "eta_m: scattering length density of matrix\n"}
                 7 {return "d_1: thickness of first shell\n"}
                 8 {return "d_2: thickness of second shell\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "StackDiscs" { 
              switch $num {
                 0 {return "F^2(Q,R,dR,c,core) = I_stackdiscs(Q,R,d,h,n,eta_c,eta_l,sigma,D)\n"}
                 1 {return "R: radius of discs\n"}
                 2 {return "d: core thickness\n"}
                 3 {return "h: layer thickness\n"}
                 4 {return "n: number of stacking\n"}
                 5 {return "eta_c: scattering contrast of core\n"}
                 6 {return "eta_l: scattering contrast of layer\n"}
                 7 {return "sigma: Gaussian standard deviation of stacking parameter D\n"}
                 8 {return "D: (2h+d) next neighbour center-to-center distance\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "p(r) -> 4 pi sin(qr)/(qr)" { 
              switch $num {
                 0 {return "4 pi sin(qr)/(qr)\nneeded for calculating the pair distribution function"}
                 1 {return "r: radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "gamma(r) -> 4 pi r^2 sin(qr)/(qr)" { 
              switch $num {
                 0 {return "4 pi r^2 sin(qr)/(qr)\nneeded for calculating the correlation function"}
                 1 {return "r: radius\n"}
                 2 {return "\n"}
                 3 {return "\n"}
                 4 {return "\n"}
                 5 {return "\n"}
                 6 {return "\n"}
                 7 {return "\n"}
                 8 {return "\n"}
                 9 {return "\n"}
                10 {return "\n"}
              }
           }
           "Background" { return "c_0 + c_1 Q + c_4 Q^(-alpha)\n" }
           "Guinier" { return "if a==0 then I(Q) = I0 exp(-Ra^2Q^2/3)   a==0: sphere\nif a!=0 the I(Q) = a pi/Q^a I0 exp(-Ra^2Q^2/(3-a)))   a==2: lamellar,  a==1: rod" }
	   default { return [get_descr_by_id $ap($type,typestr) $num] }
      }
   }
   default { return "" }
}
}

# merge lists of menu indices
# called, if the selected entry is in the plugin menu, then the first list
# contains the length of the old-style functions menu which has to be added
# to the plugin-menu indices to find the plugin menu entries
proc dropdownl_concat_index_lists { list_not_found list_found } {
	upvar $list_not_found list1
	upvar $list_found list2
	set count [expr {[lindex $list1 0]+[lindex $list2 0]}]
	set first_index [expr {[lindex $list1 0]+[lindex $list2 1]}]
	set index_list [lrange $list2 2 end]
	return [concat $count $first_index $index_list]
}

# select SD/SQ/FF functions according to their 'typestr' settings in the AP
# we have to search the menu for it, that's bad but how to improve that ?
# menu indices are lists of variable length ...
proc select_current_functions { analytpar } {
	upvar $analytpar ap
	foreach type {SD SQ FF} {
		set typestr $ap($type,typestr)
		if { [string is integer -strict $typestr] } {
			set rec [get_record_by_id $typestr]
			if { [is_func_by_rec $rec] } {
				set typestr [lindex $rec 1]
			}
		}
		if {[info exists ::dropdownl_entries($typestr)]} {
			select_submenu_entry \
				[lindex $::dropdownl_entries($typestr) 0] \
				[lindex $::dropdownl_entries($typestr) 1]
		}
	}
}

proc select_submenu_entry { menu_path index } {
	$menu_path activate $index
	$menu_path invoke [$menu_path index active]
}

# build menu hierarchy structure and count menu entry indices
proc dropdownl_add_entries { menu_path act_ap tmp_ap upd_cmd root_btn root_btn_sd type children_list } {
	upvar $act_ap ap
	set selected -1
	set menui 0
	foreach idx $children_list {
		if { [is_func_by_idx $idx] } {
			set id_str [get_func_id_by_idx $idx]
			# check if it wants to set a default size distribution and forward it
			if { [has_default_sd_by_idx $idx] } {
				set id_str [list [get_func_id_by_idx $idx] $root_btn_sd [get_default_sd_name_by_idx $idx] [get_default_sd_title_by_idx $idx]]
			}
			dropdownl_add $menu_path $act_ap $tmp_ap $upd_cmd \
				$root_btn $type $id_str [get_title_by_idx $idx]
			if { [string compare [get_func_id_by_idx $idx] $ap($type,typestr)] == 0 } {
				set selected $menui
			}
			incr menui
		} elseif { [is_group_by_idx $idx] } {
			set submenu [dropdownl_add_submenu $menu_path [get_name_by_idx $idx] [get_title_by_idx $idx]]

			set sub_children [get_children_idx_by_idx $idx]
			if { [llength $sub_children] } {
				set temp_selected [dropdownl_add_entries $submenu $act_ap $tmp_ap $upd_cmd \
					$root_btn $root_btn_sd $type $sub_children]
				if { [lindex $temp_selected end] >= 0 && [lindex $selected end] < 0 } {
					set selected [concat $menui [lrange $temp_selected 1 end]]
				}
			}
			incr menui
		}
	}
	return [concat $menui $selected]
}

proc dropdownl_add_entries_old { menu_path act_ap tmp_ap upd_cmd root_btn root_btn_sd type entry_list } {
	upvar $act_ap ap
	set selected -1
	set menui 0
	foreach e $entry_list {
		if { [string compare [string trim $e "\}\{"] $ap($type,typestr)] == 0 } {
			set selected $menui
		}
#		puts "++entr ([llength $e]),($menui),($selected): '$e'"
		# single entry, no size distribution is selected
		if { [llength $e] == 1 } {
			dropdownl_add $menu_path $act_ap $tmp_ap $upd_cmd $root_btn $type $e [string trim $e "\}\{"]
			incr menui
		# single entry with default size distribution specified
		} elseif { [llength $e] == 2 } {
			# id_str for FF: { name_ff btn_path name_sd {descr_sd} }
			dropdownl_add $menu_path $act_ap $tmp_ap $upd_cmd $root_btn $type [list [lindex $e 0] \
									$root_btn_sd \
									[lindex $e 1] \
									[lindex $e 1]] [lindex $e 0]
			incr menui
		# submenu entry, group name, its title and a list of group members
		} elseif { [llength $e] == 3 } {
			set cur_menu [dropdownl_add_submenu $menu_path [lindex $e 0] [lindex $e 1]]

			set temp_selected [dropdownl_add_entries_old $cur_menu $act_ap $tmp_ap $upd_cmd \
							$root_btn $root_btn_sd \
							$type [lindex $e 2]]
			# check if entry found and adjust menu index list to return
			if { [lindex $temp_selected end] >= 0 && [lindex $selected end] < 0 } {
				set selected [concat $menui [lrange $temp_selected 1 end]]
			}
			incr menui
		# should not occur
		} else {
			if { [llength $e] >= 4 } { return -1 }
		}
	}
#	puts "*** list: '$selected'"
	return [concat $menui $selected]
}

proc update_datasets_single {type} {
	set_actualAP_gen $type ::actualAnalytPar 0
	cp_arr ::actualAnalytPar ::tmpactualAnalytPar
	set_limitsCmd
}

proc dropdownl_create { path_name } {
	menubutton $path_name -indicatoron 1 -relief raised \
		-bd 2 -highlightthickness 2 -anchor c -direction flush \
		-disabledforeground #a3a3a3 -activeforeground Black \
		-menu $path_name.menu
	menu $path_name.menu -tearoff 0
	return $path_name.menu
}
proc dropdownl_add { menu_path actual_ap tmp_ap update_cmd root_name type id_str func_lbl } {
	$menu_path add command -label $func_lbl \
		-command "menu_command $actual_ap $tmp_ap $update_cmd $root_name $type {$id_str} \"$func_lbl\""
	# save path and index for selection by entry name later
	set ::dropdownl_entries($func_lbl) [list $menu_path [$menu_path index last]]
}
proc dropdownl_add_submenu { menu_path grp_name title } {
	$menu_path add cascade -label $title -menu $menu_path.$grp_name
	return [menu $menu_path.$grp_name -tearoff 0]
}

# this command is executed on selection of any model function from FF,SD,SQ
# FF wants to set a default SD too, additional data goes into id_str:
# id_str for FF: { name_ff btn_path name_sd {title_sd} }
proc menu_command { actual_ap tmp_ap update_cmd root_name type id_str func_label } {
	upvar $actual_ap act_ap
	upvar $tmp_ap t_ap
#	puts "** mcmd: '$root_name' '$type' '$id_str'([llength $id_str]) '$func_label'"
	set p [get_old_param_name $type]
	$root_name configure -text $func_label
	# hope this is ok for _all_ 
	for {set i 1} {$i <= $act_ap($type,param_count)} {incr i} {
		if { [string length $act_ap($type,$p$i,label)] == 0 } {
			set act_ap($type,$p$i,active) 0
		}
	}
	if { ![string equal -length 2 $type FF] } { set t_ap(alambda) -1 }
	if { [string equal -length 2 $type FF] && 
	     [llength $id_str] > 3 && 
	     ![string equal $act_ap($type,typestr) [lindex $id_str 0]]
	} {
		set sdname [lindex $id_str 2]
		set sdmenu [[lindex $id_str 1] cget -menu]
		for {set i 0} {$i < [$sdmenu index end]} {incr i} {
			set sdlbl [$sdmenu entrycget $i -label]
			if {[string equal "[string tolower $sdlbl]" "[string tolower $sdname]"]} {
				lset id_str 2 "$sdlbl"
				lset id_str 3 "$sdlbl"
			}
		}
		set act_ap($type,typestr) [lindex $id_str 0]
		# issue 2nd cmd only if selected by hand for the first time
		menu_command $actual_ap $tmp_ap $update_cmd \
			[lindex $id_str 1] SD [lindex $id_str 2] [lindex $id_str 3]
	} else {
		# has to be set before setActualAP()
		set act_ap($type,typestr) [lindex $id_str 0]
	}
	$update_cmd $type
}

proc build_param_gui { type actual_ap globalfit } {
	upvar $actual_ap act_ap
        if {$::fitparamguiupdate == no} return
	set w [winfo parent [winfo parent [winfo parent $::sasfit($type,root_menu)]]]
	set p [get_old_param_name $type]

	foreach child [winfo children $w] {
		if {[string first "$w.$p" $child] >= 0} {
			destroy $child
		}
	}
	set count $act_ap($type,param_count)
	for {set i 1} {$i <= $count} {incr i} {
		# set elements to default values if not set (param count changed)
		if {![info exists act_ap($type,$p$i,distr)]} {  
			set act_ap($type,$p$i,distr) 0}
		if {![info exists act_ap($type,$p$i,err)]} {    
			set act_ap($type,$p$i,err) 0.0}
		if {![info exists act_ap($type,$p$i,var)]} {
			set act_ap($type,$p$i,var) 0.0}
		if {![info exists act_ap($type,$p$i,limits)]} {
			set act_ap($type,$p$i,limits) 0}
		if {![info exists act_ap($type,$p$i,max)]} {
			set act_ap($type,$p$i,max) 1e+200}
		if {![info exists act_ap($type,$p$i,min)]} {
			set act_ap($type,$p$i,min) -1e+200}
		if { $globalfit } {
			if {![info exists act_ap($type,$p$i,common)]} {
				set act_ap($type,$p$i,common) NONE}
			if {![info exists act_ap($type,$p$i,factor)]} {
				set act_ap($type,$p$i,factor) 1}
		}

		set labelpath "$w.$p${i}label"
		set entrypath "$w.$p${i}entry"
		set uppath "$w.$p${i}up"
		set downpath "$w.$p${i}down"
		set fitpath "$w.$p${i}fit"
		set distrpath "$w.$p${i}distr"
		set factorpath "$w.$p${i}factor"
		set listpath "$w.$p${i}list"

		if {[string equal "dummy =" "$act_ap($type,$p$i,label)"]} {
			set act_ap($type,$p$i,label) ""
		}

		# alternative way, but \[\] not allowed inside: -text {$act_ap($type,$p$i,label)}
		set labeloptions "-justify right -anchor e -text \"$act_ap($type,$p$i,label)\""
		if { ! [winfo exists $labelpath] } {
			eval label $labelpath $labeloptions -disabledforeground #a3a3a3 -activeforeground Black
		} else {
			eval $labelpath configure $labeloptions
		}

		set entryoptions "-textvariable [format %s%s $actual_ap ($type,$p$i,var)] \
		      -relief sunken -width 15 -highlightthickness 0"
		if { ! [winfo exists $entrypath] } {
			eval entry $entrypath $entryoptions -disabledforeground #a3a3a3
		} else {
			eval $entrypath configure $entryoptions
		}

		set arrow_cmd_up ""
		set arrow_cmd_down ""
		set fit_cmd ""
		if { $globalfit } {
			bind $entrypath <Deactivate> "bind_entry_deactivate_command $type $p $i"
			bind $entrypath <FocusOut> "bind_entry_deactivate_command $type $p $i"
			bind $entrypath <KeyRelease> "bind_entry_key_release_command $type $p $i"
			bind $entrypath <KeyRelease-Return> "bind_entry_key_return_command $type $p $i"
			set arrow_cmd_up "btn_cmd_global $type $p $i up"
			set arrow_cmd_down "btn_cmd_global $type $p $i down"
			set fit_cmd "fit_cmd_global $type $p $i"
		} else {
			bind  $entrypath <KeyPress-Return> { .analytical.adj.calc invoke }
			set arrow_cmd_up "btn_up_command $type $p $i"
			set arrow_cmd_down "btn_down_command $type $p $i"
			set fit_cmd "fit_command $type $p $i"
		}

		set upoptions "-dir top -repeatdelay 300 -repeatinterval 100 \
		       -highlightthickness 0 -width 22 -height 22 \
		       -ipadx 2 -ipady 2 -clean 1 \
		       -armcommand { $arrow_cmd_up }"
		if { ! [winfo exists $uppath] } {
			eval ArrowButton $uppath $upoptions
		} else {
			eval $uppath configure $upoptions
		}

		set downoptions "-dir bottom -repeatdelay 300 -repeatinterval 100 \
		       -highlightthickness 0 -width 22 -height 22 \
		       -ipadx 2 -ipady 2 -clean 1 \
		       -armcommand { $arrow_cmd_down }"
		if { ! [winfo exists $downpath] } {
			eval ArrowButton $downpath $downoptions
		} else {
			eval $downpath configure $downoptions
		}

		set fitoptions "-variable [format %s%s $actual_ap ($type,$p$i,active)]  \
			    $::radio_check_button_prop -highlightthickness 0 \
			    -command { $fit_cmd }"
		if { ! [winfo exists $fitpath] } {
			eval checkbutton $fitpath $fitoptions
		} else {
			eval $fitpath configure $fitoptions
		}

		if {[string equal $type FF]} { 
			set distr_options "-value $i -variable distr $::radio_check_button_prop \
						-highlightthickness 0 \
						-command { distr_command $actual_ap $type $p $i $globalfit }"
			if { ! [winfo exists $distrpath] } {
				eval radiobutton $distrpath $distr_options
			} else {
				eval $distrpath configure $distr_options
			}
		}

		if { $globalfit } {
			set factoroptions "-width 4 -textvariable ::actualGlobalAnalytPar($type,$p$i,factor)"
			if { ! [winfo exists $factorpath] } {
				eval entry $factorpath $factoroptions
			} else {
				eval $factorpath configure $factoroptions
			}
			bind $factorpath <KeyRelease> "bind_factor_key_release_command $type $p $i"

			set listoptions "-width 6 -values \"$::tmpGlobalAnalytPar(common_names)\" \
					-textvariable ::actualGlobalAnalytPar($type,$p$i,common) \
					-editable no \
					-modifycmd {CommonCmd $type $w $p$i}"
			if { ! [winfo exists $listpath] } {
				eval ComboBox $listpath $listoptions
			} else {
				eval $listpath configure $listoptions
			}
		}

		set all_paths [list $labelpath $entrypath $uppath $downpath]
		if { [winfo exists $distrpath] } { lappend all_paths $distrpath }
		lappend all_paths $fitpath
		if { [winfo exists $factorpath] } { lappend all_paths $factorpath }
		if { [winfo exists $listpath] } { lappend all_paths $listpath }
		
		eval grid $all_paths -padx 1m -pady 1m -row [expr $i + 1]
		grid configure $labelpath -sticky e

		bind $entrypath <Enter> "quick_message_command $type $i $actual_ap"
		bind $labelpath <Enter> "quick_message_command $type $i $actual_ap"
		bind $uppath <Enter> "quick_message_command $type $i $actual_ap"
		bind $downpath <Enter> "quick_message_command $type $i $actual_ap"
		bind $fitpath <Enter> "quick_message_command $type $i $actual_ap"
		if { [winfo exists $distrpath] } { 
			bind $distrpath <Enter> "quick_message_command $type $i $actual_ap"
		}
	}; # foreach parameter
}
proc quick_message_window {} { return .analytical.adj.msg }
proc quick_message_command { type param_idx actual_ap } {
	[quick_message_window] configure -text "[get_helpline $type $param_idx $actual_ap]"
}

proc btn_up_command { type var idx } {
	set ::actualAnalytPar($type,$var$idx,var) [expr $::actualAnalytPar($type,$var$idx,var) * $::AnalytPar(par_x_X)]
}
proc btn_down_command { type var idx } {
	set ::actualAnalytPar($type,$var$idx,var) [expr $::actualAnalytPar($type,$var$idx,var) / $::AnalytPar(par_x_X)]
}

proc btn_cmd_global { type var idx up_down } {
	set_GlobalalambdaCmd 1 1 1
	RefreshactualGlobalAnalytParVar $::actualGlobalAnalytPar($type,$var$idx,common) $type "$var$idx" $up_down
}

proc fit_command { type var idx } {
	if { ![string equal -length 2 $type FF] } { return }
	if {$::actualAnalytPar($type,$var$idx,distr) == 1} {
		set ::actualAnalytPar($type,$var$idx,active) 0
		bell
	}
}
proc fit_cmd_global { type var idx } {
	if { [string equal -length 2 $type FF] &&
	     $::actualGlobalAnalytPar($type,$var$idx,distr) == 1
	} {
		set ::actualGlobalAnalytPar($type,$var$idx,active) 0
		bell
	}
	set_GlobalalambdaCmd 1 1 1
	RefreshGlobalParCommonPsfit $::actualGlobalAnalytPar($type,$var$idx,common) $::actualGlobalAnalytPar($type,$var$idx,active)
}

proc distr_command { actual_ap type var idx globalfit } {
	upvar $actual_ap act_ap
	for {set i 1} {$i <= $act_ap($type,param_count)} {incr i} {
		set act_ap($type,$var$i,distr) 0
	}
	set act_ap($type,$var$idx,distr) 1
	set act_ap($type,$var$idx,active) 0
	if { $globalfit } {
		set_GlobalalambdaCmd 1 1 1
	}
	set_actualAP_gen FF $actual_ap $globalfit
}

proc bind_entry_deactivate_command { type var idx } {
	if { [string compare $::actualGlobalAnalytPar($type,$var$idx,common) NONE] != 0 
	} {
		set ::actualGlobalAnalytPar($::actualGlobalAnalytPar($type,$var$idx,common)) [expr 1.0*$::actualGlobalAnalytPar($type,$var$idx,var) / $::actualGlobalAnalytPar($type,$var$idx,factor)]
		RefreshactualGlobalParCommonPs
	}
}

proc bind_entry_key_release_command { type var idx } {
	set tmp $::actualGlobalAnalytPar($type,$var$idx,var)
	set_GlobalalambdaCmd 1 1 1
	if {   ([string is double $::actualGlobalAnalytPar($type,$var$idx,var)]) \
		&& ([string length $::actualGlobalAnalytPar($type,$var$idx,var)] > 0) \
		&& ([string compare $::actualGlobalAnalytPar($type,$var$idx,common) NONE] != 0)
	} {
		catch {set ::actualGlobalAnalytPar($::actualGlobalAnalytPar($type,$var$idx,common)) [expr 1.0*$::actualGlobalAnalytPar($type,$var$idx,var) / $::actualGlobalAnalytPar($type,$var$idx,factor)]}
		RefreshactualGlobalParCommonPs "$type,$var$idx,var"
		set ::actualGlobalAnalytPar($type,$var$idx,var) $tmp
	}
}

proc bind_entry_key_return_command { type var idx } {
	set_GlobalalambdaCmd 1 1 1
	if {   ([string is double $::actualGlobalAnalytPar($type,$var$idx,var)]) \
		&& ([string length $::actualGlobalAnalytPar($type,$var$idx,var)] > 0) \
		&& ([string compare $::actualGlobalAnalytPar($type,$var$idx,common) NONE] != 0)
	} {
		catch {set ::actualGlobalAnalytPar($::actualGlobalAnalytPar($type,$var$idx,common)) [expr 1.0*$::actualGlobalAnalytPar($type,$var$idx,var) / $::actualGlobalAnalytPar($type,$var$idx,factor)]}
		RefreshactualGlobalParCommonPs
	}
	.analytical.adj.calc invoke
}

proc bind_factor_key_release_command { type var idx } {
	set_GlobalalambdaCmd 1 1 1
	if { [string is double $::actualGlobalAnalytPar($type,$var$idx,factor)] && 
	     [string length $::actualGlobalAnalytPar($type,$var$idx,factor)] > 0 &&
	     [string compare $::actualGlobalAnalytPar($type,$var$idx,common) NONE] != 0 
	} {
		catch {set ::actualGlobalAnalytPar($type,$var$idx,var) [expr $::actualGlobalAnalytPar($::actualGlobalAnalytPar($type,$var$idx,common)) * $::actualGlobalAnalytPar($type,$var$idx,factor)]}
	}
}


#------------------------------------------------------------------------------
# user interface for the paramaters of the fit of a scattering curve with 
# analytical given size distributions (lognormal distribution, ...) and a 
# set of given form factors.
# 
proc analytical_menu_bar { simorfit analytPar tmpAnalytPar actualAnalytPar \
			tmpactualAnalytPar addsasfit iqgraph
} {
	upvar $analytPar ap
	upvar $tmpAnalytPar tmpap
	upvar $actualAnalytPar actap
	upvar $tmpactualAnalytPar tmpactap
	upvar $addsasfit sf

	if { [winfo exists .analytical] } {destroy .analytical}
	if { [string equal $simorfit fit] } {
		if { ($ap(isGlobal) && ($sf(Nth,n,nonhidden) < 1))
			|| !$sf(I_enable)
		} {
			tk_messageBox -icon error -title ERROR \
				-message "No data loaded"
			return 1
		}
		set ap(fit) yes
	} else {set ap(fit) no }
	set sf(simorfit) $simorfit
	set ::distr 1

	if { $ap(isGlobal) } {
		if { ![info exist ::GlobalAnalytPar(actual_SD)]} {
			create_GlobalAnalytPar ap
			create_GlobalAnalytPar tmpap
		}
	} else {
		if { ![info exist $analytPar]} { create_AnalytPar $analytPar }
	}

	cp_arr ap tmpap
	if {$ap(isGlobal)} { GlobalAPindex tmpap actap 1
	} else {             APindex tmpap actap $tmpap(actual_SD) }
	cp_arr actap tmpactap
	if {$ap(isGlobal)} { set_GloballimitsCmd
	} else {             set_limitsCmd }

	toplevel .analytical
	set w .analytical
	set_title_analytical $tmpap(file) $ap(isGlobal)
	raise $w
	focus $w
	set actap(plottype) -1

	#
	# Pull down menu for loading saving fit data sets
	#
	frame $w.menu  -relief raised -bd 2
	pack $w.menu -side top -fill x
	menubutton $w.menu.file -text File -underline 0 -menu $w.menu.file.menu
	menu $w.menu.file.menu
	$w.menu.file.menu add command -label "Load..." \
		-command "analytical_load_par_cmd $simorfit $analytPar $tmpAnalytPar $actualAnalytPar $tmpactualAnalytPar"

	$w.menu.file.menu add command -label "Save" -command \
		"analytical_save_par_cmd $tmpAnalytPar"

	$w.menu.file.menu add command -label "Save as..." -command \
		"cmd_analytpar_save_as $tmpAnalytPar"

#	$w.menu.file.menu add command -label "Quit" -command {
#		if {[winfo exists .analytical]} {destroy .analytical}
#	}
	$w.menu.file.menu add command -label "Exit" -command \
		"cp_arr $tmpAnalytPar $analytPar
		if { $ap(isGlobal) } { RefreshAnalytParDataTab $analytPar /Global
		} else {               RefreshAnalytParDataTab $analytPar }
		if {[winfo exists .analytical]} {destroy .analytical}"

	menubutton $w.menu.options -text Options -underline 0 \
		-menu $w.menu.options.menu
	menu $w.menu.options.menu
	$w.menu.options.menu add command -label "Customize ..." \
		-command "CustomizeCmd $analytPar $tmpAnalytPar"
	if { $ap(isGlobal) } {
		$w.menu.options.menu add command -label "Set Resolution ..." \
			-command { AddCmd }
	} else {
		$w.menu.options.menu add command -label "Set Resolution ..." \
			-command { MergeCmd }
	}
	$w.menu.options.menu add command \
		-label "show confidence intervall of fit parameters ..." \
		-command "FitErrorCmd $analytPar $actualAnalytPar $tmpactualAnalytPar"
	$w.menu.options.menu add checkbutton \
		-label "update menu during fit ?" \
		-variable ::sasfit(update_menu_during_fit)

	plot_axis_menu .analytical $iqgraph actap(plottype)

	sasfit_menubar_build_help $w.menu.help
	pack $w.menu.file $w.menu.options -side left
	pack $w.menu.help -side right

	# no error
	return 0
}

proc plot_axis_menu { w g var } {
	upvar $var v
   $w.menu.options.menu add cascade -label "Plot Axis" \
           -menu $w.menu.options.menu.plotaxis
   set w2 [menu $w.menu.options.menu.plotaxis -tearoff 0]
   $w2 add radio -label "Porod Plot" \
       -variable v -value 0 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,4)\" \"y*pow(x,4)\""
   $w2 add radio -label "Guinier Plot" \
       -variable v -value 1 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,2)\" \"log(abs(y))\""
   $w2 add radio -label "Guinier Plot (rods)" \
       -variable v -value 11 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,2)\" \"log(abs(y*x))\""
   $w2 add radio -label "Guinier Plot (sheets)" \
       -variable v -value 12 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,2)\" \"log(abs(y*pow(x,2)))\""
   $w2 add radio -label "Zimm Plot" \
       -variable v -value 2 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,2)\" \"1/y\""
   $w2 add radio -label "Kratky Plot" \
       -variable v -value 3 \
       -command "analytical_plotaxis_cmd $w $g \"x\" \"y*pow(x,2)\""
   $w2 add radio -label "Holtzer Plot" \
       -variable v -value 4 \
       -command "analytical_plotaxis_cmd $w $g \"x\" \"y*x\""
   $w2 add radio -label "Debye-Bueche Plot" \
       -variable v -value 5 \
       -command "analytical_plotaxis_cmd $w $g \"pow(x,2)\" \"sqrt(abs(y))\""
   $w2 add radio -label "LogLog Plot" \
       -variable v -value 6 \
       -command "analytical_plotaxis_cmd $w $g \"log10(x)\" \"log10(y)\""
   $w2 add radio -label "LinLin Plot" \
       -variable v -value 7 \
       -command "analytical_plotaxis_cmd $w $g \"x\" \"y\""
   $w2 add radio -label "LogLin Plot" \
       -variable v -value 8 \
       -command "analytical_plotaxis_cmd $w $g \"log10(x)\" \"y\""
   $w2 add radio -label "LinLog Plot" \
       -variable v -value 9 \
       -command "analytical_plotaxis_cmd $w $g \"x\" \"log10(y)\""
}
proc analytical_plotaxis_cmd { w iqgraph xtype ytype} {
	upvar $iqgraph graph
	set graph(x,type) "$xtype"
	set graph(y,type) "$ytype"
	RefreshGraph $iqgraph
	raise $w
	focus $w
}
proc analytical_load_par_cmd { simorfit analytPar tmpAnalytPar actualAnalytPar \
			tmpactualAnalytPar
} {
	upvar $analytPar ap
	upvar $tmpAnalytPar tmpap
	upvar $actualAnalytPar actap
	upvar $tmpactualAnalytPar tmpactap

	set loadFile [tk_getOpenFile -defaultextension ".par"  \
		-filetypes {{"Fit Parameter" ".par"} \
			{"All"          ".*"}}   \
		-initialdir "$::sasfit(datadir)" \
		-title "Load Fit Parameter"   \
		-parent .analytical]

	raise .analytical
	focus .analytical
	set loadFile [lindex $loadFile 0]
	if {![file exists $loadFile]} { return }

	set ::sasfit(datadir) "[file dirname $loadFile]"
	set tmpap(alambda) -1
	set ap(alambda) -1

	cp_arr tmpap tmpAP
	if { ![load_sasfit_inp_file tmpAP $loadFile] } {
		tk_messageBox -type ok -message "couldn't read parameter file" \
			-parent .analytical
		return
	}
	set_title_analytical $loadFile 0
	if { [info exists ::ResIQGraph] } {
		clearGraph_el ::ResIQGraph
		RefreshGraph  ::ResIQGraph
	}
	if { [info exists ::SDGraph] } {
		clearGraph_el ::SDGraph
		RefreshGraph  ::SDGraph
	}
	if { $ap(isGlobal) } {
		if { [info exists ::GlobalIQGraph] } {
			clearGraph_el ::GlobalIQGraph
		}
	} else {
		# for single fit only
		if { [info exists ::IQGraph] } {
			clearGraph_el ::IQGraph
		}
		if {[string equal $::sasfit(simorfit) fit]} {
			Put_Graph_el ::IQGraph \
				$::sasfit(Q) $::sasfit(I) $::sasfit(DI)
			set ::IQGraph(l,legendtext) [list \
				[file tail $::sasfit(filename)] ]
			RefreshGraph ::IQGraph
		}
	}
	#		
	# delete all menue entries in ::nomenu
	#
	for {set i $tmpap(max_SD)} {$i > 1} {incr i -1} {
		$::nomenu delete [expr $i-1]
	}
	$::nomenu activate 0
	if { $ap(isGlobal) } {
		GlobalAPindex tmpAP $actualAnalytPar 1
		cp_arr $actualAnalytPar $tmpactualAnalytPar
		set_GloballimitsCmd
		set_actualGlobalAP $actualAnalytPar
	} else {
		APindex tmpAP $actualAnalytPar 1
		cp_arr $actualAnalytPar $tmpactualAnalytPar
		set_limitsCmd
		set_actualAP $actualAnalytPar
	}
	#
	# add additional menue entries in ::nomenu
	#
	cp_arr tmpAP $tmpAnalytPar
	for {set i 2} {$i <= $tmpAP(max_SD)} {incr i} {
		$::nomenu add radiobutton \
			-variable "${actualAnalytPar}(actual_SD)" -label $i
		$::nomenu activate [expr $i-1]
		if { $ap(isGlobal) } {
			$::nomenu entryconfigure active -command {
				RefreshGlobalAPEntryConfigure }
		} else {
			$::nomenu entryconfigure active -command {
				RefreshAPEntryConfigure }
		}
	}
	$::nomenu invoke active
	if { $ap(isGlobal) } {
		GlobalAPindex $tmpAnalytPar $actualAnalytPar $tmpAP(max_SD)
		cp_arr $actualAnalytPar $tmpactualAnalytPar
		set_GloballimitsCmd
	} else {
		APindex $tmpAnalytPar $actualAnalytPar $tmpAP(max_SD)
		cp_arr $actualAnalytPar $tmpactualAnalytPar
		set_limitsCmd
	}
}
proc analytical_save_par_cmd { tmpAnalytPar } {
	upvar $tmpAnalytPar tmpap
	if { ! [file exists $tmpap(file)] } {
		cmd_analytpar_save_as tmpap
	} else {
		cmd_analytpar_save tmpap $tmpap(file)
	}
}

proc analyticalSDCmd {simorfit} {

if {[analytical_menu_bar $simorfit ::AnalytPar ::tmpAnalytPar \
	::actualAnalytPar ::tmpactualAnalytPar ::sasfit ::IQGraph] 
} { return }
set w .analytical

#
# buttons for choosing No of size distributions
#
frame $w.whichSD
pack $w.whichSD -fill x 

set ::nomenu [tk_optionMenu $w.whichSD.no ::actualAnalytPar(actual_SD) 1]
$::nomenu entryconfigure 0 -command { RefreshAPEntryConfigure }
set tmp $::actualAnalytPar(actual_SD)
for {set i 2} {$i <= $::actualAnalytPar(max_SD)} {incr i} {
  $::nomenu add radiobutton \
     -variable ::actualAnalytPar(actual_SD) \
     -label $i 
  $::nomenu entryconfigure [expr $i - 1] -command {
      RefreshAPEntryConfigure
  }
}
setTooltip $w.whichSD.no "display parameter of\ni-th scattering contribution"
$w.whichSD.no configure -highlightthickness 0
label $w.whichSD.label -text "contribution:"
pack $w.whichSD.label $w.whichSD.no -in $w.whichSD -side left -padx 1m -pady 1m
Button $w.whichSD.next -text Next -highlightthickness 0 \
-padx 1m -pady 1m \
-repeatinterval 100 \
-repeatdelay  2000 \
-command {
  if {$::actualAnalytPar(actual_SD) >= $::actualAnalytPar(max_SD)} {
     $::nomenu activate 0
     $::nomenu invoke 0
  } else {
     $::nomenu activate $::actualAnalytPar(actual_SD)
     $::nomenu invoke active 
  }
}
setTooltip $w.whichSD.next "display parameter of next\nscattering contribution"
Button $w.whichSD.previous -text Previous -highlightthickness 0 \
-padx 1m -pady 1m \
-repeatinterval 100 \
-repeatdelay  2000 \
-command {
  if {$::actualAnalytPar(actual_SD) <= 1} {
     $::nomenu activate last
     $::nomenu invoke active 
  } else { 
     $::nomenu activate [expr $::actualAnalytPar(actual_SD) - 2]
     $::nomenu invoke active
  }
}
setTooltip $w.whichSD.previous "display parameter of previous\nscattering contribution"
Button $w.whichSD.add -text Add -highlightthickness 0 \
-padx 1m -pady 1m \
-repeatinterval 100 \
-repeatdelay  2000 \
-command { 
  set ::tmpAnalytPar(alambda) -1
  cp_arr ::actualAnalytPar tt
  set ::actualAnalytPar(old_actual_SD) $::actualAnalytPar(actual_SD)
  save_AP ::tmpAnalytPar ::actualAnalytPar

  put_AP ::tmpAnalytPar
  set tt(actual_SD)  $::tmpAnalytPar(max_SD) 
  save_AP ::tmpAnalytPar tt
  APindex ::tmpAnalytPar ::actualAnalytPar $::tmpAnalytPar(max_SD)
  set ::actualAnalytPar(old_actual_SD) $::actualAnalytPar(actual_SD)
  cp_arr ::actualAnalytPar ::tmpactualAnalytPar
  set_limitsCmd
  $::nomenu add radiobutton \
     -variable ::actualAnalytPar(actual_SD) \
     -label $::actualAnalytPar(max_SD) 
  $::nomenu entryconfigure [expr $::actualAnalytPar(max_SD)-1] \
	  -command RefreshAPEntryConfigure
  $::nomenu activate [expr $::actualAnalytPar(max_SD)-1]
}
setTooltip $w.whichSD.add "add a set of parameter for\nanother scattering contribution"
Button $w.whichSD.delete -text Remove -highlightthickness 0 \
-padx 1m -pady 1m \
-repeatinterval 100 \
-repeatdelay  2000 \
-command {
  set ::tmpAnalytPar(alambda) -1
  if {$::actualAnalytPar(max_SD) > 1} {
     set max_SD $::actualAnalytPar(max_SD)
     del_AP ::tmpAnalytPar $::actualAnalytPar(actual_SD)
     if {$max_SD == $::actualAnalytPar(actual_SD)} {
	incr ::actualAnalytPar(actual_SD) -1
	$::nomenu delete $::actualAnalytPar(actual_SD) 
     } else {
	 for {set i $max_SD} \
	     {$i > $::actualAnalytPar(actual_SD)} \
	     {incr i -1 } {
		$::nomenu delete [expr $i - 1]
	 }
	for {set i  [expr $::actualAnalytPar(actual_SD)+1]} \
	    {$i < [expr $max_SD]} {incr i} {
	       $::nomenu add radiobutton \
		       -variable ::actualAnalytPar(actual_SD)  \
		       -label [expr $i]
	       $::nomenu entryconfigure $i \
		       -command RefreshAPEntryConfigure
	       }   
     }
     APindex ::tmpAnalytPar ::actualAnalytPar $::actualAnalytPar(actual_SD)
     cp_arr ::actualAnalytPar ::tmpactualAnalytPar
     set_limitsCmd
     $::nomenu activate [expr $::actualAnalytPar(actual_SD) - 1]
     $::nomenu invoke active
 } else { bell }
}
setTooltip $w.whichSD.delete "remove actual displayed\nscattering contribution"
frame $w.whichSD.fitcalc
eval checkbutton $w.whichSD.fitcalc.fit "-offvalue yes -onvalue no \
-variable ::actualAnalytPar(fitSDFF) \
-text \"fix\" \
-command {
	set_actualAP ::actualAnalytPar
      } -highlightthickness 0"
eval checkbutton $w.whichSD.fitcalc.calc "-offvalue no -onvalue yes \
-variable ::actualAnalytPar(calcSDFF) $::radio_check_button_prop \
-text \"apply\" \
-command {
	set_actualAP ::actualAnalytPar
      } -highlightthickness 0"
eval checkbutton $w.whichSD.fitcalc.substr "-offvalue no -onvalue yes \
-variable ::actualAnalytPar(substrSDFF) $::radio_check_button_prop \
-text \"subtract\" \
-command {
	set_actualAP ::actualAnalytPar
      } -highlightthickness 0"
grid configure $w.whichSD.fitcalc.fit    -column 0 -row 0 -sticky w
grid configure $w.whichSD.fitcalc.calc   -column 0 -row 1 -sticky w
grid configure $w.whichSD.fitcalc.substr -column 1 -row 1 -sticky w

pack $w.whichSD.previous $w.whichSD.next $w.whichSD.add $w.whichSD.delete \
-side left -in $w.whichSD -fill x -expand 1 -padx 1m -pady 1m
pack $w.whichSD.fitcalc -side right -in $w.whichSD -padx 1m -pady 1m

frame $w.dataset
pack $w.dataset

if {[string equal $::sasfit(simorfit) "fit"]
} {
	set wds $w.dataset
	label $wds.chisqrlabel_l -text "chisqr:" -anchor e
	label $wds.chisqrlabel_v \
	-textvariable ::AnalytPar(chisq) \
	-width 12 -anchor w

	if {![info exists ::AnalytPar(chisqr_rel)]} { 
		set ::AnalytPar(chisqr_rel) -1.0 }
	label $wds.chisqr_rel_lbl -text "R value:" -anchor e
	label $wds.chisqr_rel_val -width 12 -anchor w \
		-textvariable ::AnalytPar(chisqr_rel)

	if {![info exists ::AnalytPar(chisqr_relerr)]} { 
		set ::AnalytPar(chisqr_relerr) -1.0 }
	label $wds.chisqr_relerr_lbl -text "R value (incl. error, EXP.):" -anchor e
	label $wds.chisqr_relerr_val -width 12 -anchor w \
		-textvariable ::AnalytPar(chisqr_relerr)

	pack $wds.chisqrlabel_l $wds.chisqrlabel_v \
		$wds.chisqr_rel_lbl $wds.chisqr_rel_val \
		$wds.chisqr_relerr_lbl $wds.chisqr_relerr_val \
		-fill x -anchor w -padx 1m -pady 1 -side left
}

#
# frames of entry fields
#

set lst [create_analytical_parameter_frame $w $::AnalytPar(isGlobal)]
set wn       [lindex $lst 0]
set wsd      [lindex $lst 1]
set wff      [lindex $lst 2]
set wsq      [lindex $lst 3]
set wsqhow   [lindex $lst 4]
set wdet2D_a [lindex $lst 5]
set wdet2D_b [lindex $lst 6]

#
# input fields for parameters of calculating 2D detector intensity data
#

label $wdet2D_a.l_pix    -text "number of pixel:"
label $wdet2D_a.l_pixres -text "pixel size \[mm\]:"
label $wdet2D_a.l_SD     -text "detector distance \[m\]:"
label $wdet2D_a.l_lam    -text "wavelength \[nm\]"

grid $wdet2D_a.l_pix    -row 0 -column 0 -sticky e
grid $wdet2D_a.l_pixres -row 1 -column 0 -sticky e
grid $wdet2D_a.l_SD     -row 2 -column 0 -sticky e
grid $wdet2D_a.l_lam    -row 3 -column 0 -sticky e

entry    $wdet2D_a.e_SD     -textvariable Detector2DIQGraph(SD) -width 4
entry    $wdet2D_a.e_pixres -textvariable Detector2DIQGraph(pixres) -width 4
ComboBox $wdet2D_a.e_pix    -values "8 16 32 64 128 256 512" \
		    -textvariable Detector2DIQGraph(nPix) \
		    -width 4
entry    $wdet2D_a.e_lam    -textvariable Detector2DIQGraph(lambda) \
		    -width 4

grid $wdet2D_a.e_pix    -row 0 -column 1 -sticky w
grid $wdet2D_a.e_pixres -row 1 -column 1 -sticky w
grid $wdet2D_a.e_SD     -row 2 -column 1 -sticky w
grid $wdet2D_a.e_lam    -row 3 -column 1 -sticky w


ComboBox $wdet2D_b.c_scale -textvariable Detector2DIQGraph(scale) \
		   -values "y log(y) arcsinh(y) sqrt(y)" \
		   -width 9
#                           -label scale: 
grid $wdet2D_b.c_scale -row 1 -column 0 -sticky e

radiobutton    $wdet2D_b.r_manual -text manual -value manual -variable Detector2DIQGraph(auto)
radiobutton    $wdet2D_b.r_auto   -text auto   -value auto   -variable Detector2DIQGraph(auto)
LabelEntry     $wdet2D_b.e_min    \
       -label min:  \
       -width 12 \
       -textvariable Detector2DIQGraph(min)
LabelEntry     $wdet2D_b.e_max    \
       -label max:  \
       -width 12 \
       -textvariable Detector2DIQGraph(max)

ComboBox $wdet2D_b.c_ct -textvariable Detector2DIQGraph(ct) \
		-width 5 \
		-values "jet rainbow bw"
#                        -label "color table:"\

grid $wdet2D_b.r_manual -row 2 -column 0 -sticky w
grid $wdet2D_b.r_auto   -row 3 -column 0 -sticky w
grid $wdet2D_b.e_min    -row 2 -column 1 -sticky e
grid $wdet2D_b.e_max    -row 3 -column 1 -sticky e
grid $wdet2D_b.c_ct     -row 0 -column 0 -sticky e

#puts $wdet2D_b.do
button $wdet2D_b.do -text "Calculate\n2D detector pattern" \
-command {     
	    save_AP ::tmpAnalytPar ::actualAnalytPar 
	    cp_arr ::tmpAnalytPar ::AnalytPar
	    set Det2DRes [sasfit_2Diq ::AnalytPar {{1 1} {1 1} {1 1}} Detector2DIQGraph]
#                    puts $Det2DRes
	    destroy $Detector2DIQGraph(cw) 

	    if {$::sasfit(width) < $::sasfit(height)} {
	       set width2D $::sasfit(width)
	    } else {
	       set width2D $::sasfit(height)
	    }

	    canvas $Detector2DIQGraph(cw)  -background white \
					   -width $width2D \
					   -height $width2D
	    pack $Detector2DIQGraph(cw)
#   $Detector2DIQGraph(w) setwidget  $Detector2DIQGraph(cw) 
	    set s [::Plotchart::createIsometricPlot $Detector2DIQGraph(cw) \
		[list 0.0 $Detector2DIQGraph(nPix)] \
		[list 0.0 $Detector2DIQGraph(nPix)] noaxes] 
	    set Detector2DIQGraph(s) $s
	    for {set j 0} {$j < $Detector2DIQGraph(nPix)} {incr j} {
	       for {set i 0} {$i < $Detector2DIQGraph(nPix)} {incr i} {
		  set xll $j
		  set yll $i
		  set xur [expr $j+1]
		  set yur [expr $i+1]
		  set ctIndx [lindex [lindex $Det2DRes $j] $i]
		  $s plot filled-rectangle $xll $yll $xur $yur  \
		      [lindex $Detector2DIQGraph($Detector2DIQGraph(ct)) $ctIndx]
	       }
	    }

menu $Detector2DIQGraph(cw).popup -tearoff 0
$Detector2DIQGraph(cw).popup add command -label "copy to clipboard" -un 0 -command {
window_to_clipboard $Detector2DIQGraph(cw)
}
bind $Detector2DIQGraph(cw) <ButtonPress-3>        {tk_popup $Detector2DIQGraph(cw).popup %X %Y }
bind $Detector2DIQGraph(cw) <Double-ButtonPress-1> {tk_popup $Detector2DIQGraph(cw).popup %X %Y }

	  }
grid   $wdet2D_b.do    -row 4 -column 0 -columnspan 4 -sticky ew

#
# input fields for parameters of size distributions
#

frame $wsd.wrl
grid $wsd.wrl -columnspan 5 -row 0

set root_btn_sd $wsd.wrl.whichsd
set sdmenu [dropdownl_create $root_btn_sd]
set ::sasfit(SD,root_menu) $sdmenu

set menui 0
set selected_sd -1
foreach old_sd_type {Monodisperse "A_div_B" Delta LogNorm BiLogNorm GEX GEV \
		gammaSD Triangular fractalSD Uniform Beta Gaussian \
		Maxwell Schultz-Zimm Weibull
} {
	dropdownl_add $sdmenu ::actualAnalytPar ::tmpAnalytPar update_datasets_single \
		$root_btn_sd SD $old_sd_type $old_sd_type
	if { [string equal $old_sd_type $::actualAnalytPar(SD,typestr)]} {
		set selected_sd $menui
	}
	incr menui
}
set selected_sd [concat $menui $selected_sd]
set selected_plug [dropdownl_add_entries $sdmenu ::actualAnalytPar ::tmpAnalytPar update_datasets_single [winfo parent $sdmenu] [winfo parent $sdmenu] [get_old_func_name "size_distrib"] [get_children_idx_by_name "size_distrib"]]
if { [lindex $selected_sd end] < 0 } { 
	set selected_sd [dropdownl_concat_index_lists selected_sd selected_plug]
}


$wsd.wrl.whichsd configure -highlightthickness 0
pack $wsd.wrl.whichsd -side left -padx 1m -pady 1m -in $wsd.wrl

button $wsd.wrl.rangebutton -text "Parameter Range..." \
-highlightthickness 0 -padx 1m -pady 1m\
-command SDFitRangeCmd
setTooltip $wsd.wrl.rangebutton "define range interval\nfor fitting parameter"
pack $wsd.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wsd.wrl

label $wsd.text1 -text "parameter:"
label $wsd.text2 -text "fit "
grid $wsd.text1 -column 0 -columnspan 4 -row 1 -sticky w
grid $wsd.text2 -column 4 -row 1


#
# input fields for parameters of form factors
#
frame $wff.wrl
grid $wff.wrl -columnspan 6 -row 0

set root_btn_ff $wff.wrl.whichff
set ffmenu [dropdownl_create $root_btn_ff]
set ::sasfit(FF,root_menu) $ffmenu

set selected_ff [dropdownl_add_entries_old $ffmenu ::actualAnalytPar ::tmpAnalytPar \
				update_datasets_single $root_btn_ff $root_btn_sd FF { \ 
{Background Monodisperse} \ 
{aux "auxiliary transition functions" { \
				{{p(r) -> 4 pi sin(qr)/(qr)} Gauss} \
				{{gamma(r) -> 4 pi r^2 sin(qr)/(qr)} Gauss} 
}   }
{sphshell "Spheres & Shells" { Sphere \
		      {{Spherical Shell i}}\
		      {{Spherical Shell ii}} \
		      {{Spherical Shell iii}} \ 
		      {MultiLamellarVesicle Delta} \
		      {RNDMultiLamellarVesicle Delta} \
		      {RNDMultiLamellarVesicle2 Delta} \ 
		      {MLVesicleFrielinghaus Delta} \
		       BiLayeredVesicle \
		       LinShell \
		       LinShell2 \
		       SphereShell_compr \
		       ExpShell}} \ 
{ellipsoid "ellipsoidal obj." { \
		       {{Ellipsoid i}} \
		       {{Ellipsoid ii}} \
			 EllipsoidalCoreShell \
			{triaxEllShell1 Delta} \
		      }} \ 
{polymers_micelles "polymers & micelles" {{gauss "polymer chains" { \
							   {Gauss Delta} \
							   {Gauss2 Delta} \
							   {Gauss3 Delta} \
							   {GaussPoly Delta} \
							   {{generalized Gaussian coil} Delta} \
							   {{generalized Gaussian coil 2} Delta} \
							   {{generalized Gaussian coil 3} Delta} \
							  }} \ 
				  {stars "polymer stars" { \
							  {BenoitStar Delta} \
							  {PolydisperseStar Delta} \
							  {DozierStar Delta} \ 
							  {DozierStar2 Delta} }} \ 
				  {rings "polymer rings" { \
							  {FlexibleRingPolymer Delta} \ 
							  {mMemberedTwistedRing Delta} \ 
							  {DaisyLikeRing Delta} }} \ 
				  {spherical "spherical & ellipsoidal micelles" { \
							  {SPHERE+Chains(RW)_Nagg Delta} \ 
							  {SPHERE+Chains(RW)_Rc Delta} \
							  {SPHERE+Chains(RW) Delta} \ 
							  {SPHERE+Chains(SAW)_Nagg Delta} \
							  {SPHERE+Chains(SAW)_Rc Delta} \ 
							  {SPHERE+Chains(SAW) Delta} \
							  {SPHERE+Exp_Rc Delta} \
							  {SPHERE+R^-a_Nagg Delta} \ 
							  {SPHERE+R^-a_Rc Delta} \
							  {SPHERE+R^-a Delta} \ 
							  {SPHERE_smooth_interface+R^-a_Nagg Delta} \ 
							  {SPHERE_smooth_interface+R^-a_Rc Delta} \
							  {SPHERE+R^-a_Manuela Delta} \
							  {SPHERE+R^-a_Manuela2 Delta} \
							  {ELL+Chains(RW)_Nagg Delta} \ 
							  {ELL+Chains(RW)_Rc Delta} \
							  {ELL+Chains(RW) Delta} \ 
							  {SphereWithGaussChains Delta} \
							  {BlockCopolymerMicelle Delta} \
							  }} \ 
				  {cylindrical "cylindrical & rod-like micelles" { \
							  {CYL+Chains(RW)_Nagg Delta} \ 
							  {CYL+Chains(RW)_Rc Delta} \
							  {CYL+Chains(RW) Delta} \ 
							  {WORM+Chains(RW)_nagg Delta} \
							  {WORM+Chains(RW)_Rc Delta} \ 
							  {WORM+Chains(RW) Delta} \
							  {ROD+Chains(RW)_nagg Delta} \ 
							  {ROD+Chains(RW)_Rc Delta} \
							  {ROD+Chains(RW) Delta} \ 
							  {ROD+R^-a_nagg Delta} \
							  {ROD+R^-a_Rc Delta} {ROD+R^-a Delta} \ 
							  {ROD+Gauss_nagg Delta} \
							  {ROD+Gauss_Rc Delta} \
							  {ROD+Gauss Delta} \ 
							  {ROD+Exp_nagg Delta} \
							  {ROD+Exp_Rc Delta} \
							  {ROD+Exp Delta} \
							  }} \ 
				  {localplanar "local planar micelles (sheets, ULV)" { \
							  {DISC+Chains(RW)_nagg Delta} \ 
							  {DISC+Chains(RW)_Lc Delta} \
							  {DISC+Chains(RW) Delta} \ 
							  {SphULV+Chains(RW)_nagg Delta} \
							  {SphULV+Chains(RW)_tc Delta} \ 
							  {SphULV+Chains(RW) Delta} \
							  {EllULV+Chains(RW)_nagg Delta} \ 
							  {EllULV+Chains(RW)_tc Delta} \
							  {EllULV+Chains(RW) Delta} \ 
							  {CylULV+Chains(RW)_nagg Delta} \
							  {CylULV+Chains(RW)_tc Delta} \ 
							  {CylULV+Chains(RW) Delta} }} \ 
				  {worms "wormlike structures" { \
							  {WormLikeChainEXV Delta} \
							  {KholodenkoWorm Delta} \
							  }} }} \ 
{cluster "clusters" { \
		 {Fisher-Burford Monodisperse} \
		 {MassFractExp Monodisperse}\
		 {MassFractGauss Monodisperse}\
		 {{Aggregate (Exp(-x^a) Cut-Off)} Monodisperse} \ 
		 {{Aggregate (OverlapSph Cut-Off)} Monodisperse} \
		 {DLCAggregation Monodisperse}\
		 {RLCAggregation Monodisperse}\
		 StackDiscs \
		 DumbbellShell \ 
		 two_attached_spheres \
		 DoubleShellChain \ 
		 TetrahedronDoubleShell \
		}} \ 
{nonparticular "non-particular structures" {\
    {OrnsteinZernike Monodisperse} \ 
    {BroadPeak Monodisperse} \  
    {TeubnerStrey Monodisperse} \
    {DAB Monodisperse} \ 
    {Spinodal Monodisperse} \
    {BeacaugeExpPowLaw Monodisperse} \ 
    {BeacaugeExpPowLaw2 Monodisperse} \
    {Guinier Monodisperse} \
  }} \
{cyl "cylindrical obj." { \
		{Disc Delta} \
		 PorodCylinder \
		 LongCylinder \
		 FlatCylinder \
		 Cylinder \
		 LongCylShell \
		{ellCylShell1 Delta} \
		{ellCylShell2 Delta} \
		 CylShell1 \
		 CylShell2 \ 
		 alignedCylShell \
	       {{partly aligned CylShell} Delta} \
		 Torus \
		}} \ 
{anisotropic "anisotropic obj." {{planar "Pcs(Q) for planar obj." {{Pcs:homogenousXS Delta} \
							   {Pcs:TwoInfinitelyThinPlates Delta} \ 
							   {Pcs:LayeredCentroSymmetricXS Delta} \
							   {Pcs:BiLayerGauss Delta} \ 
							   {Pcs:Plate+Chains(RW) Delta}} } \ 
			 {cylindrical "Pcs(Q) for cylindrical obj." {{Pcs:homogeneousCyl Delta} \
								     {Pcs:ellCylSh Delta} \
								    }} \
			}} \ 
{planar "planar obj." {{homogenousXS Delta} \
	       {SphSh+SD+homoXS Delta} \
	       {EllSh+SD+homoXS Delta} \
	       {EllSh+SD+homoXS(S) Delta} \ 
	       {CylSh+SD+homoXS Delta} \
	       {Disc+homoXS Delta} \
	       {TwoInfinitelyThinPlates Delta} \
	       {LayeredCentroSymmetricXS Delta} \ 
	       {BiLayerGauss Delta}}} \ 
{shear "sheared objects" {ShearedCylinder \
		  ShearedCylGauss}} \ 
{magnetic "magnetic objects" {MagneticShellPsi \
		      MagneticShellAniso \
		      MagneticShellCrossTerm \
		      SuperparamagneticFFpsi \ 
		      SuperparamagneticFFAniso \
		      SuperparamagneticFFIso \
		      SuperparamagneticFFCrossTerm}} \ 
{mieFF "Mie FF for SLS" {MieSphere \
		 MieShell}} \
{qensFF "QENS" {{"confinement with Gaussian potential" Monodisperse} }} \
{peaks "Peaks" {{amplitude "Amplitude Functions" {{{Beta (Amplitude)} Monodisperse} \
					  {{Chi-squared (Amplitude)} Monodisperse} \
					  {{Erfc (Amplitude)} Monodisperse} \
					  {{Error (Amplitude)} Monodisperse} \
					  {{exponentially modified Gaussian (Amplitude)} Monodisperse} \
					  {{Extreme Value (Amplitude)} Monodisperse} \
					  {{F-variance (Amplitude)} Monodisperse} \
					  {{Gamma (Amplitude)} Monodisperse} \
					  {{Gaussian (Amplitude)} Monodisperse} \
					  {{Gaussian-Lorentzian cross product (Amplitude)} Monodisperse} \
					  {{Gaussian-Lorentzian sum (Amplitude)} Monodisperse} \
					  {{generalized Gaussian 1 (Amplitude)} Monodisperse} \
					  {{generalized Gaussian 2 (Amplitude)} Monodisperse} \
					  {{Giddings (Amplitude)} Monodisperse} \
					  {{Inverted Gamma (Amplitude)} Monodisperse} \
					  {{Kumaraswamy (Amplitude)} Monodisperse} \
					  {{Laplace (Amplitude)} Monodisperse} \
					  {{Logistic (Amplitude)} Monodisperse} \
					  {{LogLogistic (Amplitude)} Monodisperse} \
					  {{LogNormal, 4 parameters (Amplitude)} Monodisperse} \
					  {{LogNormal (Amplitude)} Monodisperse} \
					  {{Lorentzian (Amplitude)} Monodisperse} \
					  {{Pearson IV (Amplitude)} Monodisperse} \
					  {{Pearson VII (Amplitude)} Monodisperse} \
					  {{pulse (Amplitude)} Monodisperse} \
					  {{pulse with 2nd width (Amplitude)} Monodisperse} \
					  {{pulse with power term (Amplitude)} Monodisperse} \
					  {{Student-t (Amplitude)} Monodisperse} \
					  {{Voigt (Amplitude)} Monodisperse} \
					  {{Weibull (Amplitude)} Monodisperse} \
			  }   }
	{area "Area Functions" {          {{Beta (Area)} Monodisperse} \
					  {{Chi-squared (Area)} Monodisperse} \
					  {{Erfc (Area)} Monodisperse} \
					  {{Error (Area)} Monodisperse} \
					  {{exponentially modified Gaussian (Area)} Monodisperse} \
					  {{Extreme Value (Area)} Monodisperse} \
					  {{fatique life (Area)} Monodisperse} \
					  {{F-variance (Area)} Monodisperse} \
					  {{Gamma (Area)} Monodisperse} \
					  {{Gaussian (Area)} Monodisperse} \
					  {{Gaussian-Lorentzian cross product (Area)} Monodisperse} \
					  {{Gaussian-Lorentzian sum (Area)} Monodisperse} \
					  {{generalized Gaussian 1 (Area)} Monodisperse} \
					  {{generalized Gaussian 2 (Area)} Monodisperse} \
					  {{Giddings (Area)} Monodisperse} \
					  {{Haarhoff Van der Linde (Area)} Monodisperse} \
					  {{half Gaussian modified Gaussian (Area)} Monodisperse} \
					  {{Inverted Gamma (Area)} Monodisperse} \
					  {{Kumaraswamy (Area)} Monodisperse} \
					  {{Laplace (Area)} Monodisperse} \
					  {{Logistic (Area)} Monodisperse} \
					  {{LogLogistic (Area)} Monodisperse} \
					  {{LogNormal, 4 parameters (Area)} Monodisperse} \
					  {{LogNormal (Area)} Monodisperse} \
					  {{Lorentzian (Area)} Monodisperse} \
					  {{Pearson IV (Area)} Monodisperse} \
					  {{Pearson VII (Area)} Monodisperse} \
					  {{power LogNormal (Area)} Monodisperse} \
					  {{power Normal (Area)} Monodisperse}
					  {{pulse (Area)} Monodisperse} \
					  {{pulse with 2nd width (Area)} Monodisperse} \
					  {{pulse with power term (Area)} Monodisperse} \
					  {{Student-t (Area)} Monodisperse} \
					  {{Voigt (Area)} Monodisperse} \
					  {{Weibull (Area)} Monodisperse} \
			  }   }   }   } \
{others "other form factors" {   LangevinMH DoubleShell_withSD \
			{SuperParStroboPsi Delta} \
			{SuperParStroboPsi2 Delta} \ 
			{SuperParStroboTISANE1 Delta} \
			 SuperParStroboPsiSQ \
			 SuperParStroboPsiSQBt1 \
			 SuperParStroboPsiSQLx \ 
			 SuperParStroboPsiSQL2x \
			 SuperParStroboPsiSQL2x_SANSPOL \
			 SuperParStroboPsiSQL2x_SANSPOL_albr \ 
			 SuperParStroboPsiSQL2x_SANSPOL_albr_stat \
			 SuperParStroboPsiSQL2modx_SANSPOL \
			 hysteresesStroboPsi \ 
			 DLS_Sphere_RDG \
			{Robertus1 Delta} \
			{Robertus2 Delta} \
			{Guinier Monodisperse} \ 
			{JuelichMicelle Monodisperse} \
			{Francois1 Monodisperse} \
			{CopolymerMicelleR^-a Monodisperse} \ 
			{CopolymerMicelleRod Monodisperse} \
			{CopolymerMicelleEll Monodisperse} \ 
			{CopolymerMicelleCyl Monodisperse} \
			Andrea1 \
			Simon1 \
			{A(B) Monodisperse} \ 
			{B(B) Monodisperse} \
			{P39 Monodisperse} \
			{julia "Julia's form factors" {{TripleLayeredXS_ULV Monodisperse}}} } } \ 
}]

set selected_plug [dropdownl_add_entries $ffmenu ::actualAnalytPar ::tmpAnalytPar update_datasets_single [winfo parent $ffmenu] [winfo parent $sdmenu] [get_old_func_name "form_fac"] [get_children_idx_by_name "form_fac"]]
if { [lindex $selected_ff end] < 0 } { 
	set selected_ff [dropdownl_concat_index_lists selected_ff selected_plug]
}

$wff.wrl.whichff configure -highlightthickness 0
pack $wff.wrl.whichff -side left -padx 1m -pady 1m -in $wff.wrl
button $wff.wrl.rangebutton -text "Parameter Range..." \
-highlightthickness 0  -padx 1m -pady 1m \
-command FFFitRangeCmd
setTooltip $wff.wrl.rangebutton "define range interval\nfor fitting parameter"
pack $wff.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wff.wrl

label $wff.text1 -text "parameter:"
label $wff.text2 -text "distr"
label $wff.text3 -text "fit "
grid  $wff.text1 -column 0 -columnspan 4 -row 1 -sticky w
grid  $wff.text2 -column 4 -row 1
grid  $wff.text3 -column 5 -row 1


#
# input fields for parameters of structure factor
#

frame $wsq.wrl
grid $wsq.wrl -columnspan 7 -row 0 -sticky w
#set sqmenu [tk_optionMenu $wsq.wrl.whichsq ::actualAnalytPar(SQ,typestr) "None" ]

set root_btn_sq $wsq.wrl.whichsq
set sqmenu [dropdownl_create $root_btn_sq]
set ::sasfit(SQ,root_menu) $sqmenu

set selected_sq [dropdownl_add_entries_old $sqmenu ::actualAnalytPar ::tmpAnalytPar \
	update_datasets_single $root_btn_sq $root_btn_sd SQ { \ 
None \ 
{sticky "Hard & Sticky Hard Sphere" {{{Hard Sphere}} \
                                     {{Sticky Hard Sphere}} \
                                     {{Sticky Hard Sphere 2}} \ 
				     {{Square Well Potential}} \
				     {{Thin Square Well Potential}} \
                                     {{Square Well Potential 2}}}} \
{lamellar "Multi Lamellar Structures" {ThermalDisorder Paracrystalline ModifiedCaille}} \
{anisotropic "anisotropic obj." {{planar "P'(Q):local planar geometry" {{{P'(Q):ThinDisc}} {{P'(Q):ThinSphericalShell}} \
							{{P'(Q):ThinEllipsoidalShell}} {{P'(Q):ThinHollowCylinder}}}} \
				{cylindrical "P'(Q):local cylindrical geometry" { \
                                                                {{P'(Q):Rod}} \
                                                                {{P'(Q):Worm(PS1)}} \
                                                                {{P'(Q):Worm(PS2)}} \
                                                                {{P'(Q):Worm(PS3)}} \
                                                                {{P'(Q):WormLikeChain_withEXV}} \
								{{P'(Q):WormLikeChain_withoutEXV}} \
                                                                {{P'(Q):KholodenkoWorm}} }}}}
{fractal "fractal obj." {{{Mass Fractal (Exp Cut-Off)}} {{Mass Fractal (Exp(-x^a) Cut-Off)}} {{Mass Fractal (Gaussian Cut-Off)}} \
			 {{Mass Fractal (OverlapSph Cut-Off)}} }} \
{peaks "Peaks" {GaussianAmplitude LorentzianAmplitude VoigtPeak PearsonVIIAmplitude}
		} \
{other "other" {Cylinder(PRISM) RPA {{Correlation Hole}} {{Critical Scattering}} {{Macro Ion (HP)}} {{Hayter Penfold RMSA}} \
		BabaAhmed BabaAhmed2 LocalOrder RandomDistribution}} \
}]

set selected_plug [dropdownl_add_entries $sqmenu ::actualAnalytPar ::tmpAnalytPar update_datasets_single [winfo parent $sqmenu] [winfo parent $sdmenu] [get_old_func_name "struct_fac"] [get_children_idx_by_name "struct_fac"]]
if { [lindex $selected_sq end] < 0 } { 
	set selected_sq [dropdownl_concat_index_lists selected_sq selected_plug]
}


$wsq.wrl.whichsq configure -highlightthickness 0
pack $wsq.wrl.whichsq -side left -padx 1m -pady 1m -in $wsq.wrl
button $wsq.wrl.rangebutton -text "Parameter Range..." \
       -highlightthickness 0 -padx 1m -pady 1m\
       -command SQFitRangeCmd
setTooltip $wsq.wrl.rangebutton "define range interval\nfor fitting parameter"
pack $wsq.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wsq.wrl

label $wsq.text1 -text "parameter:"
label $wsq.text2 -text "fit "

grid  $wsq.text1 -column 1 -columnspan 3 -row 1 -sticky w
grid  $wsq.text2 -column 4 -row 1

#
# input fields for how to calculate the structure factor
#

radiobutton $wsqhow.sq0 -text "monodisperse approximation:\nI(Q) =  <F^2(Q,R)>  S(Q)" -value "0" \
                        -variable ::actualAnalytPar(SQ,how) -anchor w -justify left \
			-command {set_actualAP ::actualAnalytPar}
radiobutton $wsqhow.sq1 -text "decoupling approach:\nI(Q) =  <F^2(Q,R)> + <F(Q,R)>^2 \[S(Q)-1\]" -value "1" \
                        -variable ::actualAnalytPar(SQ,how) -anchor w -justify left\
			-command {set_actualAP ::actualAnalytPar}
radiobutton $wsqhow.sq2 -text "local monodisperse approximation:\nI(Q) = int(N(R) F^2(Q,R) S(Q,R),dR)" -value "2" \
                        -variable ::actualAnalytPar(SQ,how) -anchor w -justify left\
			-command {set_actualAP ::actualAnalytPar}
radiobutton $wsqhow.sq3 -text "partial structure factor:\nI(Q) = int(N(R) F^2(Q,R),dR)\n+  int(N(R1) F^2(Q,R1)N(R2) F^2(Q,R2) S(Q,R1,R2),dR1,dR2)" \
                        -value "3" -variable ::actualAnalytPar(SQ,how) -anchor w -justify left\
			-command {set_actualAP ::actualAnalytPar}
radiobutton $wsqhow.sq4 -text "scaling approximation of partial structure factor:\nI(Q) = int(N(R) F^2(Q,R),dR)+\n      int(N(R1) F^2(Q,R1)N(R2) ((R1+R2)/(<R>))^3F^2(Q,R2) S(Q,R1,R2),dR1,dR2)" \
                        -value "4" -variable ::actualAnalytPar(SQ,how) -anchor w -justify left\
			-command {set_actualAP ::actualAnalytPar}

set sqoriq [tk_optionMenu $wsqhow.sq5 FitPrecision(SQ_or_IQ,label) "calculate effective structure factor"]
$sqoriq entryconfigure 0 -command { 
        set ::FitPrecision(SQ_or_IQ) -1
        set ::FitPrecision(SQ_or_IQ,label) "calculate effective structure factor"
        }
$sqoriq add command -label "calculate scattering intensity" -command {
        set ::FitPrecision(SQ_or_IQ) 1
        set ::FitPrecision(SQ_or_IQ,label)  "calculate scattering intensity" 
        }

grid $wsqhow.sq0 -column 1 -row 1 -sticky w
grid $wsqhow.sq1 -column 1 -row 2 -sticky w
grid $wsqhow.sq2 -column 1 -row 3 -sticky w
grid $wsqhow.sq3 -column 1 -row 4 -sticky w
grid $wsqhow.sq4 -column 1 -row 5 -sticky w
grid $wsqhow.sq5 -column 1 -row 6 -sticky w

set simulate 1
if {[string equal "$simorfit" "fit"]} { set simulate 0 }

analytical_widgets_bottom $w $simulate $::AnalytPar(isGlobal) 

# ensures the bottom doesn't collapse on resize
pack $w.fitpar -fill both -expand 1

#
## button commands
#

if {!$simulate && [winfo exists $w.adj.calc]} {
	$w.adj.calc configure -command { 
              set ::fitparamguiupdate yes
              set sub no

              if {([string length $::sasfit(lower,Q)] > 0) && \
                  ([string length $::sasfit(upper,Q)] > 0) } {
	         set Q {}
                 set I {}
                 set DI {}
                 set res {}
                 foreach x $::sasfit(Q) y $::sasfit(I) e $::sasfit(DI) r $::sasfit(res) {
		    if {($x>=$::sasfit(lower,Q)) && ($x<=$::sasfit(upper,Q))} {
                       lappend Q   $x
                       lappend I   $y
                       lappend DI  $e
                       lappend res $r
                    }
                 } 
	      } else  {
                 set Q  $::sasfit(Q)
                 set I  $::sasfit(I)
                 set DI $::sasfit(DI)
                 set res $::sasfit(res)
              }
              save_AP ::tmpAnalytPar ::actualAnalytPar 
              cp_arr ::tmpAnalytPar ::AnalytPar
              if { $::sasfit(I_enable)  } {
              if {    (($::AnalytPar(error) == 0) && $::sasfit(DI_enable) )
                   || (($::AnalytPar(error) != 0) && !$::sasfit(DI_enable) ) } {
		 if {[string compare $::AnalytPar(resolution) yes] == 0} {
                    set IthIres [sasfit_iq ::AnalytPar  \
                                         [list $Q $I $DI $res] \
                                ]
		 } else {
                    set IthIres [sasfit_iq ::AnalytPar  \
                                         [list $Q $I $DI] \
                                ]
		 }

		 foreach subt $::AnalytPar(substrSDFF) calct $::AnalytPar(calcSDFF) {
		     if {$subt && $calct} {
			 set sub yes
		     }
		 }

#                 puts "theoretical scattering curves caluclated"
#                 puts $IthIres
                 set NR  [sasfit_nr ::AnalytPar \
                                    [list $Q $I $DI] \
                         ]
#                 puts "size distribution caluclated, plot it ..."
#                 puts $NR
                 NewFitDataCmd Q IthIres $sub NR
              } else { tk_messageBox -icon error -title ERROR \
                         -message "NO error values of data points are avaliable or assumed" 
              }
              } else { tk_messageBox -icon error \
                                     -title ERROR -message "NO data loaded" 
              }
              RefreshAnalytParDataTab ::AnalytPar
              set ::fitparamguiupdate yes
	}
}

if {[winfo exists $w.adj.step]} {
	$w.adj.step configure -command { 
              set ::fitparamguiupdate no
              set sub no
              if {([string length $::sasfit(lower,Q)] > 0) && \
                  ([string length $::sasfit(upper,Q)] > 0) } {
	         set Q {}
                 set I {}
                 set DI {}
                 set res {}
                 foreach x $::sasfit(Q) y $::sasfit(I) e $::sasfit(DI) r $::sasfit(res) {
		    if {($x>=$::sasfit(lower,Q)) && ($x<=$::sasfit(upper,Q))} {
                       lappend Q  $x
                       lappend I  $y
                       lappend DI $e
                       lappend res $r
                    }
                 } 
	      } else {
                 set Q  $::sasfit(Q)
                 set I  $::sasfit(I)
                 set DI $::sasfit(DI)
                 set res $::sasfit(res)
              }
              set alambda $::AnalytPar(alambda)
              set ::tmpAnalytPar(alambda) $alambda
              save_AP ::tmpAnalytPar ::actualAnalytPar 
              cp_arr ::tmpAnalytPar ::AnalytPar
              set ::AnalytPar(alambda) $alambda
              set ::tmpAnalytPar(alambda) $alambda
              if { $::sasfit(I_enable) } {
                 if {$alambda == -1} {
		       set ::stepfit(oalambda) ::AnalytPar(alambda)
		     if {[string compare $::AnalytPar(resolution) yes] == 0} {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI $res] ::stepfit \
                               ]
                    } else {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI] ::stepfit \
                               ]
		    }
                    set ::stepfit(itst) 0
                    set ::stepfit(k)    0
                    set ::stepfit(ochisq) $::stepfit(chisq)
                 } elseif {$alambda == 0} {
                    set alambda -1
                    set ::stepfit(oalambda) ::AnalytPar(alambda)
                    set IthIres [sasfit_iqfit ::AnalytPar  \
                                    [list $Q $I $DI] ::stepfit \
                            ]
                    set ::stepfit(itst) 0
                    set ::stepfit(k)    0
                    set ::stepfit(ochisq) $::stepfit(chisq)
                 } elseif {$::stepfit(itst) < 2} {
                    incr ::stepfit(k)
                    set ::stepfit(ochisq) $::stepfit(chisq)
                    set ::stepfit(oalambda) $::AnalytPar(alambda)
		    if {[string compare $::AnalytPar(resolution) yes] == 0} {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI $res] \
                                       ::stepfit \
                               ]
		    } else {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI] \
                                       ::stepfit \
                               ]
		    }
                    if {$::stepfit(chisq) > $::stepfit(ochisq)} {
                       set ::stepfit(itst) 0
                    } elseif {[expr abs($::stepfit(chisq)- \
                                        $::stepfit(ochisq))]<0.1} {
                      incr ::stepfit(itst)
                    }
                    if {$::stepfit(oalambda) > $::AnalytPar(alambda) } {
                       set ::stepfit(itst) 0
                    }
                 } else {
                    set alambda 0
                    set ::AnalytPar(alambda) 0
                    if {[string compare $::AnalytPar(resolution) yes] == 0} {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI $res] \
                                       ::stepfit \
                               ]
		    } else {
                       set IthIres [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI] \
                                       ::stepfit \
                               ]
		    }
                 }
                 set alambda $::AnalytPar(alambda)
                 puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
                 set tmp $::tmpAnalytPar(actual_SD)
                 cp_arr ::AnalytPar ::tmpAnalytPar
                 APindex ::tmpAnalytPar ::actualAnalytPar $tmp
                 cp_arr ::actualAnalytPar ::tmpactualAnalytPar
                 set_actualAP ::actualAnalytPar no
                 set NR  [sasfit_nr ::AnalytPar \
                                    [list $Q $I $DI] \
                         ]

		 foreach subt $::AnalytPar(substrSDFF) calct $::AnalytPar(calcSDFF) {
		     if {$subt && $calct} {
			 set sub yes
		     }
		 }

                 NewFitDataCmd Q IthIres $sub NR
#                 .analytical.adj.calc invoke
                 set ::AnalytPar(alambda) $alambda
                 set ::tmpAnalytPar(alambda) $alambda
              } else { tk_messageBox -icon error \
                                     -title ERROR -message "NO data loaded" 
              }
              RefreshAnalytParDataTab ::AnalytPar
              set ::fitparamguiupdate yes
	}
}

if {[winfo exists $w.adj.run]} {
	$w.adj.run configure -command {
              set ::fitparamguiupdate no
              $::nomenu activate [expr $::actualAnalytPar(actual_SD)-1]
              $::nomenu invoke [expr $::actualAnalytPar(actual_SD)-1]
              cp_arr ::tmpAnalytPar ::AnalytPar

              set sub no
              if {([string length $::sasfit(lower,Q)] > 0) && \
                  ([string length $::sasfit(upper,Q)] > 0) } {
	         set Q   {}
                 set I   {}
                 set DI  {}
                 set res {}
                 foreach x $::sasfit(Q) y $::sasfit(I) e $::sasfit(DI) r $::sasfit(res) {
		    if {($x>=$::sasfit(lower,Q)) && ($x<=$::sasfit(upper,Q))} {
                       lappend Q   $x
                       lappend I   $y
                       lappend DI  $e
                       lappend res $r
                    }
                 } 
	      } else {
                 set Q   $::sasfit(Q)
                 set I   $::sasfit(I)
                 set DI  $::sasfit(DI)
                 set res $::sasfit(res)
              }
              
              if {[winfo exists .fitruncontinue]} {destroy .fitruncontinue}
              toplevel .fitruncontinue
              set w .fitruncontinue
              wm geometry $w
              wm title $w "continue fit"
              set alambda  -1
              set ::stepfit(chisq) 0
              label $w.text -text "STOP FITTING ?" -font *-*-*-150-*
              set continue_fit "yes"
              button $w.button -text "Stop" -command {
                                                set continue_fit "no"
                                                set_alambdaCmd name1 name2 op
                                                destroy .fitruncontinue
                                             }
              frame $w.fitprogress
              label $w.fitprogress.chisqlabel -text "chisq ="
              label $w.fitprogress.chisqvalue -textvariable ::stepfit(chisq)
              label $w.fitprogress.alambdalabel -text "alambda ="
              label $w.fitprogress.alambdavalue -textvariable alambda
              pack $w.text -fill both -padx 2m -pady 2m
              pack $w.fitprogress -fill both -padx 2m -pady 2m -expand 1
              pack $w.fitprogress.alambdalabel $w.fitprogress.alambdavalue \
                   -side left
              pack $w.fitprogress.chisqlabel $w.fitprogress.chisqvalue \
                   -side left 
              pack $w.button -padx 2m -pady 2m

              set alambda $::AnalytPar(alambda)
              if {$alambda == 0} { set alambda  -1 }
              set ::tmpAnalytPar(alambda) $alambda
	      if { $::sasfit(update_menu_during_fit) } {
              	  save_AP ::tmpAnalytPar ::actualAnalytPar
              	  cp_arr ::tmpAnalytPar ::AnalytPar
                  RefreshAnalytParDataTab ::AnalytPar
	      }
              set ::AnalytPar(alambda) $alambda
              set ::tmpAnalytPar(alambda) $alambda
              update

              if { $::sasfit(I_enable) } {
                  set ::stepfit(oalambda) ::AnalytPar(alambda)
		  if {[catch {
                      sasfit_timer_start "\nstart 0. iteration"
		      if {[string compare $::AnalytPar(resolution) yes] == 0} {
			  set IthIres [sasfit_iqfit ::AnalytPar  \
				  [list $Q $I $DI $res] ::stepfit ]
		      } else {
			  set IthIres [sasfit_iqfit ::AnalytPar  \
				  [list $Q $I $DI] ::stepfit ]
		      }
                      sasfit_timer_stop "0. iteration" "iteration finished" "\nalambda = $::AnalytPar(alambda),  chisq = $::stepfit(chisq)"
	          } msg] } {
		     set_alambdaCmd 1 1 1
		     bgerror $msg
                     set ::fitparamguiupdate yes
		     return
		  }

                  set alambda $::AnalytPar(alambda)
                  puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
                  set tmp $::tmpAnalytPar(actual_SD)
	          if { $::sasfit(update_menu_during_fit) } {
                      cp_arr ::AnalytPar ::tmpAnalytPar
                      APindex ::tmpAnalytPar ::actualAnalytPar $tmp
		  }
		  set NR {}
		  foreach subt $::AnalytPar(substrSDFF) calct $::AnalytPar(calcSDFF) {
		      if {$subt && $calct} {
			  set sub yes
		      }
		  }
	          if { $::sasfit(update_menu_during_fit) } {
                      NewFitDataCmd Q IthIres $sub NR
		  }
                  set ::AnalytPar(alambda) $alambda
                  set ::tmpAnalytPar(alambda) $alambda
                  update 
                  if { $continue_fit == "no" } { 
                     destroy $w
                  }
                  set ::stepfit(itst) 0
                  set ::stepfit(k)    0
                  set ::stepfit(ochisq) $::stepfit(chisq)

                  while {($::stepfit(itst) < 2) && ($continue_fit == "yes")} {
                     incr ::stepfit(k)
                     set ::stepfit(ochisq) $::stepfit(chisq)
                     set ::stepfit(oalambda) $::AnalytPar(alambda)
		     if {[catch {
                         sasfit_timer_start "\nstart $::stepfit(k). iteration"
			 if {[string compare $::AnalytPar(resolution) yes] == 0} {
			     set IthIres [sasfit_iqfit ::AnalytPar  \
				     [list $Q $I $DI $res] \
				     ::stepfit \
				     ]
			 } else {
			     set IthIres [sasfit_iqfit ::AnalytPar  \
				     [list $Q $I $DI] \
				     ::stepfit \
                                     ]
			 }
                         sasfit_timer_stop "$::stepfit(k). iteration" "iteration finished" "\nalambda = $::AnalytPar(alambda),  chisq = $::stepfit(chisq)"
		     } msg] } {
			 set_alambdaCmd 1 1 1
			 bgerror $msg
                         set ::fitparamguiupdate yes
			 return
		     }

                     set alambda $::AnalytPar(alambda)
                     puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
                     set tmp $::tmpAnalytPar(actual_SD)
	             if { $::sasfit(update_menu_during_fit) } {
                         cp_arr ::AnalytPar ::tmpAnalytPar
                         APindex ::tmpAnalytPar ::actualAnalytPar $tmp
                         cp_arr ::actualAnalytPar ::tmpactualAnalytPar
                         set_actualAP ::tmpactualAnalytPar
		     }

   		     foreach subt $::AnalytPar(substrSDFF) calct $::AnalytPar(calcSDFF) {
			 if {$subt && $calct} {
			     set sub yes
			 }
		     }
		     set NR {}
	             if { $::sasfit(update_menu_during_fit) } {
                         NewFitDataCmd Q IthIres $sub NR
		     }
                     set ::AnalytPar(alambda) $alambda
                     set ::tmpAnalytPar(alambda) $alambda
                     update 
                     if {$::stepfit(chisq) > $::stepfit(ochisq)} {
                        set ::stepfit(itst) 0
                     } elseif {[expr abs($::stepfit(chisq)- \
                                         $::stepfit(ochisq))]<0.1} {
                       incr ::stepfit(itst)
                     }
                     if {$::stepfit(oalambda) > $::AnalytPar(alambda) } {
                        set ::stepfit(itst) 0
                     }
                  }
                 if { $continue_fit == "yes" } {
                    set alambda 0
                    set ::AnalytPar(alambda) 0
                    if {[string compare $::AnalytPar(resolution) yes] == 0} {
                       set Ith [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI $res] \
                                       ::stepfit \
                                   ]
		    } else {
                       set Ith [sasfit_iqfit ::AnalytPar  \
                                       [list $Q $I $DI] \
                                       ::stepfit \
                                   ]
		    }
                    set alambda $::AnalytPar(alambda)
                    puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
                    set tmp $::tmpAnalytPar(actual_SD)
	            if { $::sasfit(update_menu_during_fit) } {
                        cp_arr ::AnalytPar ::tmpAnalytPar
                        APindex ::tmpAnalytPar ::actualAnalytPar $tmp
                        cp_arr ::actualAnalytPar ::tmpactualAnalytPar
                        set_actualAP ::tmpactualAnalytPar
		    }

		    foreach subt $::AnalytPar(substrSDFF) calct $::AnalytPar(calcSDFF) {
			if {$subt && $calct} {
			    set sub yes
			}
		    }
		    set NR {}
	            if { $::sasfit(update_menu_during_fit) } {
                        NewFitDataCmd Q IthIres $sub NR
		    }
                    set ::AnalytPar(alambda) $alambda
                    set ::tmpAnalytPar(alambda) $alambda
                 }
		 set NR  [sasfit_nr ::AnalytPar [list $Q $I $DI]]
		 NewFitDataCmd Q IthIres $sub NR
                 cp_arr ::AnalytPar ::tmpAnalytPar
                 APindex ::tmpAnalytPar ::actualAnalytPar $tmp
                 cp_arr ::actualAnalytPar ::tmpactualAnalytPar
                 #set_actualAP ::tmpactualAnalytPar
                 set_actualAP ::actualAnalytPar
              } else { tk_messageBox -icon error \
                                     -title ERROR -message "NO data loaded"
              }
              catch { destroy $w }
              RefreshAnalytParDataTab ::AnalytPar
              set ::fitparamguiupdate yes
	}
}

if {$simulate && [winfo exists $w.adj.calc]} {
	$w.adj.calc configure -command {
	      sasfit_timer_start "\nStart simulation"

              set Q {}
              set I {}
              set DI {}
              if {$::sasfit(sim,Qmin) > 0} {
                 set logDQ [expr log10($::sasfit(sim,Qmax))-log10($::sasfit(sim,Qmin))]
                 set logDQ [expr $logDQ/$::sasfit(sim,N)]
                 for {set k 0} {$k<$::sasfit(sim,N)+1} {incr k} {
                     lappend Q [expr pow(10.0,log10($::sasfit(sim,Qmin))+$k*$logDQ)]
                     lappend I 1.0
                     lappend DI 1.0
                 }
	      } else {
                 set DQ [expr 1.0*($::sasfit(sim,Qmax))-($::sasfit(sim,Qmin))]
                 set DQ [expr $DQ/$::sasfit(sim,N)]
                 for {set k 0} {$k<$::sasfit(sim,N)+1} {incr k} {
                     lappend Q [expr $::sasfit(sim,Qmin)+$k*$DQ]
                     lappend I 1.0
                     lappend DI 1.0
                 }
	      }
              save_AP ::tmpAnalytPar ::actualAnalytPar 
              cp_arr ::tmpAnalytPar ::AnalytPar

	      set ::sasfit(Q) $Q

              set IthIres [sasfit_iq ::AnalytPar [list $Q $I $DI]]
	      set ::sasfit(I) [lindex $IthIres 2]
	      set ::sasfit(DI) [lindex $IthIres 4]

              set NR [sasfit_nr ::AnalytPar [list $Q $I $DI]]
	      set ::sasfit(NR) $NR
#puts "size distribution caluclated, plot it ..."
#puts $NR
              NewFitDataCmd Q IthIres no NR sim
              RefreshAnalytParDataTab ::AnalytPar
		     
                  sasfit_timer_stop "Simulation" "finished" ""
	}
}

	bind $wsd.wrl.whichsd <Enter> \
	     {[quick_message_window] configure -text "[get_helpline SD 0 ::actualAnalytPar]"}
	bind $wff.wrl.whichff <Enter> \
	     {[quick_message_window] configure -text "[get_helpline FF 0 ::actualAnalytPar]"}
	bind $wsq.wrl.whichsq <Enter> \
	     {[quick_message_window] configure -text "[get_helpline SQ 0 ::actualAnalytPar]"}
	analytical_scroll_binds [winfo toplevel $wsd]

	$::nomenu activate 0
	$::nomenu invoke 0
	$wn raise 1
	$wn compute_size

	if {[llength $::AnalytPar(FF,param_count)] != $::AnalytPar(max_SD)
	} {
		cycleThroughContributionsAndApply ::AnalytPar .analytical
	}
}

proc analytical_widgets_bottom { w simulate isGlobal
} {
	frame $w.adj -relief groove -borderwidth 1
	pack $w.adj -fill both -side bottom

	# online help in the 'status-bar'
	message [quick_message_window] -text "" \
		-justify left -width 350 -anchor w
	pack [quick_message_window] -side left -fill x

	if { ! $simulate
	} {
		button $w.adj.calc -text "Apply" -highlightthickness 0 \
			-padx 1m -pady 1m
		bind $w <KeyPress-Return> "$w.adj.calc invoke"
		setTooltip $w.adj.calc "calculate theoretical curve"
		button $w.adj.step -text "Step" -highlightthickness 0  \
			-padx 1m -pady 1m
		setTooltip $w.adj.step "do one step of\nfitting procedure"
		button $w.adj.run -text "Run fit" -highlightthickness -1 \
			-padx 1m -pady 1m
		label $w.adj.maxIterLabel -text "max. iter.:"
		entry $w.adj.maxIterEntry -width 4 \
			-textvariable ::addsasfit(maxIterations)
		pack $w.adj.run $w.adj.step $w.adj.calc \
			$w.adj.maxIterEntry $w.adj.maxIterLabel \
			-padx 1m -pady 1m -side right
	} else {
		.analytical.whichSD.fitcalc.fit configure -state disabled
		.analytical.whichSD.fitcalc.fit configure -selectcolor gray
		.analytical.whichSD.fitcalc.substr configure -state disabled
		.analytical.whichSD.fitcalc.substr configure -selectcolor gray
		button $w.adj.calc -text "Simulate" -highlightthickness 0 \
			-padx 1m -pady 1m
		bind $w <KeyPress-Return> "$w.adj.calc invoke"
		setTooltip $w.adj.calc "calculate theoretical curve"
		label $w.adj.qmintxt -text Qmin: -width 5
	        if {$isGlobal} {
		    entry $w.adj.qmin \
			-textvariable ::addsasfit(sim,Qmin) \
			-width 5
		} else {
		    entry $w.adj.qmin \
			-textvariable ::sasfit(sim,Qmin) \
			-width 5
		}
		label $w.adj.qmaxtxt -text Qmax: -width 5
		if {$isGlobal} {
		    entry $w.adj.qmax \
			-textvariable ::addsasfit(sim,Qmax) \
			-width 5
		} else {
		    entry $w.adj.qmax \
			-textvariable ::sasfit(sim,Qmax) \
			-width 5
		}
		label $w.adj.qNtxt -text N: -width 2
		if {$isGlobal} {
		    entry $w.adj.qN \
			-textvariable ::addsasfit(sim,N) \
			-width 5
		} else {
		    entry $w.adj.qN \
			-textvariable ::sasfit(sim,N) \
			-width 5
		}
		pack $w.adj.calc \
		     $w.adj.qmin $w.adj.qmintxt \
		     $w.adj.qmax $w.adj.qmaxtxt \
		     $w.adj.qN $w.adj.qNtxt \
		     -padx 1m -pady 1m -side right
	}
}

proc analytical_scroll_binds { w } {
	bind $w <Button-4> { analytical_scroll_cmd %W -10 }
	bind $w <Button-5> { analytical_scroll_cmd %W  10 }
	bind $w <MouseWheel> {
		set parent [winfo containing -displayof %W \
			[expr [winfo rootx %W] + %x] \
		       	[expr [winfo rooty %W] + %y] ]
		analytical_scroll_cmd $parent [expr %D/-10]
	}
}

proc analytical_scroll_cmd { w units } {
	set parent $w
	while {[winfo exists $parent] && 
		"[string range $parent end-3 end]" != ".lay"
	} {
		set parent [winfo parent $parent]
	}
	if {![string length $parent]} { return }
	if {[winfo exists $parent]} { set parent [winfo parent $parent] }
	if {![string length $parent]} { return }
	if {[winfo exists $parent] && "[winfo class $parent]" == "Canvas"} {
		$parent yview scroll $units units
	}
}

proc create_analytical_parameter_frame { w isGlobal } {

	frame $w.fitpar

	set wn [NoteBook $w.fitpar.notebook]
	pack $wn -fill both -expand 1

	set SDFF_f  [$wn insert 1 1 \
		-text "size distrib. and formfactor" \
		-state normal ]
	set SQ_f    [$wn insert 2 2 \
		-text "structure factor" \
		-state normal ]
	if {!$isGlobal} {
		set Det2D_f [$wn insert 3 3 \
			-text "2D detector calculation (under construction)" \
			-state normal ]
	}

	set canvasheight 300

	canvas $w.fitpar.sd -highlightthickness 0 -height $canvasheight \
		-yscrollcommand "$w.fitpar.sdscrb set"
	canvas $w.fitpar.ff -highlightthickness 0 -height $canvasheight \
		-yscrollcommand "$w.fitpar.ffscrb set"
	scrollbar $w.fitpar.sdscrb -command "$w.fitpar.sd yview"
	scrollbar $w.fitpar.ffscrb -command "$w.fitpar.ff yview"

	canvas $w.fitpar.sq -highlightthickness 0 -height $canvasheight \
		-yscrollcommand "$w.fitpar.sqscrb set"
	scrollbar $w.fitpar.sqscrb -command "$w.fitpar.sq yview"
	frame $w.fitpar.sqhow 
	frame $w.fitpar.sep2 -relief groove -borderwidth 3 -width 3

	pack $w.fitpar.sd     -side left -fill y -in $SDFF_f -expand 1 -anchor e
	pack $w.fitpar.sdscrb -side left -fill y -in $SDFF_f
	pack $w.fitpar.ff     -side left -fill y -in $SDFF_f -expand 1 -anchor e
	pack $w.fitpar.ffscrb -side left -fill y -in $SDFF_f

	pack $w.fitpar.sq -side left -fill both -in $SQ_f
	pack $w.fitpar.sqscrb -side left -fill y -in $SQ_f
	pack $w.fitpar.sep2 -side left -fill both -in $SQ_f -padx 12
	pack $w.fitpar.sqhow -side left -fill both -in $SQ_f

	if {!$isGlobal} {
		frame $w.fitpar.det2D
		pack $w.fitpar.det2D -side left -fill both -in $Det2D_f
	}

	set wsd $w.fitpar.sd.lay
	pack [frame $wsd] -in $w.fitpar.sd

	set wff $w.fitpar.ff.lay
	pack [frame $wff] -in $w.fitpar.ff

	set wsq $w.fitpar.sq.lay
	pack [frame $wsq] -in $w.fitpar.sq

	$w.fitpar.sd create window 0 0 -anchor nw -window $wsd
	bind $wsd <Configure> "scrollform_resize $w.fitpar.sd"
	$w.fitpar.ff create window 0 0 -anchor nw -window $wff
	bind $wff <Configure> "scrollform_resize $w.fitpar.ff"
	$w.fitpar.sq create window 0 0 -anchor nw -window $wsq
	bind $wsq <Configure> "scrollform_resize $w.fitpar.sq"

	set wsqhow $w.fitpar.sqhow.lay
	frame $wsqhow
	pack $wsqhow -in $w.fitpar.sqhow -fill y -expand 1

	set wlist [list $wn $wsd $wff $wsq $wsqhow]

	if {!$isGlobal} {
		set wdet2D_a $w.fitpar.det2D.lay_a
		frame $wdet2D_a
		pack $wdet2D_a -in $w.fitpar.det2D -fill y -expand 1 -side left
		set wdet2D_b $w.fitpar.det2D.lay_b
		frame $wdet2D_b
		pack $wdet2D_b -in $w.fitpar.det2D -fill y -expand 1 -side right
		lappend wlist $wdet2D_a
		lappend wlist $wdet2D_b
	}
	return $wlist
}

proc cycleThroughContributionsAndApply { AnalytPar wparent } {
	upvar $AnalytPar ap
	set count $ap(max_SD)
	set selincr 1
	if {$count < 10} { set selincr 2 }
	set w $wparent.verifystatus
	toplevel $w -takefocus 1
	wm title $w "Verifying project file"
	frame $w.frm -padx 20 -pady 20
	pack $w.frm
	label $w.frm.lbl -pady 10 \
		-text "Please wait, while verifying \nthe loaded project file data:"
	entry $w.frm.ntr -relief flat -width [expr $count*$selincr] \
       		-state readonly -highlightthickness 0 -selectborderwidth 0 \
		-exportselection 0 -font tkconfixed
	pack $w.frm.lbl
	pack $w.frm.ntr
	raise $w

	# position it central above the parent window
	set pgeo [split [wm geometry $wparent] {+x}]
	set cgeo [split [wm geometry $w] {+x}]
	# half the width + x-position
	set x [expr [lindex $pgeo 0]*0.5 + [lindex $pgeo 2]]
	set y [expr [lindex $pgeo 1]*0.5 + [lindex $pgeo 3]]
	# x-position - half the width of child
	set x [expr round($x - [lindex $cgeo 0]*0.5)]
	set y [expr round($y - [lindex $cgeo 1]*0.5)]
	if {$x < 0} {set x 0}
	if {$y < 0} {set y 0}
	wm geometry $w +$x+$y
	focus $w
	update

	$w.frm.ntr selection clear
	for {set i 0} {$i < $count} {incr i} {
		$wparent.whichSD.next invoke
		$w.frm.ntr configure -state normal
		for {set k 0} {$k < $selincr} {incr k} {
			$w.frm.ntr insert end "+"
		}
		# selection adjustment does not work on windows
		#$w.frm.ntr selection range 0 end
		$w.frm.ntr configure -state readonly
		update
	}
	destroy $w
#	$wparent.adj.calc invoke
	focus $wparent
}
