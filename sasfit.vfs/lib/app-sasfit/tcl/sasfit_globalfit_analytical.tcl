# sasfit.vfs/lib/app-sasfit/tcl/sasfit_globalfit_analytical.tcl
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
#   Ingo Bressler (ingo@cs.tu-berlin.de)

proc RefreshGlobalAPEntryConfigure {
} {
	set alambda $::GlobalAnalytPar(alambda) 
	set tmp $::actualGlobalAnalytPar(actual_SD)
	set ::actualGlobalAnalytPar(actual_SD) $::actualGlobalAnalytPar(old_actual_SD)
	save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar
	set ::actualGlobalAnalytPar(actual_SD) $tmp
	GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $::actualGlobalAnalytPar(actual_SD)
	cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
	set_GloballimitsCmd
	set ::tmpGlobalAnalytPar(alambda) $alambda
	set ::GlobalAnalytPar(alambda) $alambda 
	set ::actualGlobalAnalytPar(old_actual_SD) $::actualGlobalAnalytPar(actual_SD)
	RefreshactualGlobalParCommonPs
	select_current_functions ::actualGlobalAnalytPar
}

proc set_dataset_labels_sim {
} {
	set ::actualGlobalAnalytPar(datasetlabel) {}
	for {set i 1} {$i <= $::ask4dataset_var(N_datasets)} {incr i
	} {
		lappend ::actualGlobalAnalytPar(datasetlabel) $i
		if {![info exists ::actualGlobalAnalytPar(datalabel,$i)]} { 
			set ::actualGlobalAnalytPar(datalabel,$i) "I_$i"
		}
	}
}

proc DefNoOfCurvesToSim {
} {
	if {[winfo exists .defnoofcurvestosim]} {destroy .defnoofcurvestosim}
	toplevel .defnoofcurvestosim
	set w .defnoofcurvestosim
	wm geometry $w
	wm title $w "No. of data sets to simulate"

	frame $w.question
	label $w.question.text -text "number of data sets to simulate:" -anchor w
	spinbox $w.question.totno -width 5 -from 1 -to 1000 -command {
			set num "%s"
			if { [string is integer -strict $num] && $num > 0 
			} {
				set ::ask4dataset_var(N_datasets) $num
				set ::actualGlobalAnalytPar(datasetlabel) {}
				set entry_parent "[winfo parent [winfo parent %W]].dataset"

				foreach ch [winfo children $entry_parent] { destroy $ch }

				set_dataset_labels_sim
				foreach idx $::actualGlobalAnalytPar(datasetlabel) {
					set entryname [join [list "$entry_parent" ".entry$idx"] {}]
					entry $entryname -textvariable ::actualGlobalAnalytPar(datalabel,$idx)
				}
				eval pack [winfo children $entry_parent]
			}
		}

	frame $w.dataset

	pack $w.question.text  $w.question.totno -fill both -expand yes -side left

	frame $w.button 
	pack $w.question $w.dataset $w.button -fill both -expand no -pady 4

	button $w.button.ok \
		-text "OK"  \
		-command {
		  .analytical.dataset.datasetNo configure -values $::actualGlobalAnalytPar(datasetlabel)
		  destroy .defnoofcurvestosim
		}
	button $w.button.cancel -text "Cancel" -command {
		destroy .defnoofcurvestosim
	}

	pack $w.button.ok $w.button.cancel -side left -fill x -expand yes
	raise $w
	focus $w

	$w.question.totno set [expr $::ask4dataset_var(N_datasets)-1]
	$w.question.totno invoke buttonup
}


#------------------------------------------------------------------------------
# actualize all fit (active) parameters of actualGlobalPar anf tmpGlobalAnalytPar
# is selected
#

proc RefreshGlobalParCommonPsfit {CommonP active_state
} {
	foreach type {SD FF SQ} p {a l s} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar($type,param_count)} {incr i} {
			if { [string compare $::actualGlobalAnalytPar($type,$p$i,common) $CommonP] == 0 && 
					[string compare $CommonP NONE] != 0 
			} {
				set ::actualGlobalAnalytPar($type,$p$i,active) $active_state
				if { [string compare $type FF] == 0 
				} {
					if { $::actualGlobalAnalytPar($type,$p$i,distr) == 1 
					} {
						set ::actualGlobalAnalytPar($type,$p$i,active) 0
					}
				}
			}
		}
	}

	set cnt 0
	foreach SDactive $::tmpGlobalAnalytPar(SD,active) SDname $::tmpGlobalAnalytPar(SD,common) {
	   set cnter 0
	   foreach SDact $SDactive SDnam $SDname {
	      if {([string compare $SDnam $CommonP] == 0) && ([string compare $CommonP NONE] != 0)} {
		 set ltmp [lindex $::tmpGlobalAnalytPar(SD,active) $cnt]
		 set ltmp [lreplace $ltmp $cnter $cnter $active_state]
		 set ::tmpGlobalAnalytPar(SD,active) [lreplace $::tmpGlobalAnalytPar(SD,active) $cnt $cnt $ltmp]
	      }
	      incr cnter
	   }
	   incr cnt
	}

	set cnt 0
	foreach FFactive $::tmpGlobalAnalytPar(FF,active) FFname $::tmpGlobalAnalytPar(FF,common) {
	   set cnter 0
	   foreach FFact $FFactive FFnam $FFname {
	      if {([string compare $CommonP NONE] != 0) && ([string compare $FFnam $CommonP] == 0) && ($::actualGlobalAnalytPar(FF,l[expr $cnter+1],distr) == 0)} {
		 set ltmp [lindex $::tmpGlobalAnalytPar(FF,active) $cnt]
		 set ltmp [lreplace $ltmp $cnter $cnter $active_state]
		 set ::tmpGlobalAnalytPar(FF,active) [lreplace $::tmpGlobalAnalytPar(FF,active) $cnt $cnt $ltmp]
	      }
	      incr cnter
	   }
	   incr cnt
	}


	set cnt 0
	foreach SQactive $::tmpGlobalAnalytPar(SQ,active) SQname $::tmpGlobalAnalytPar(SQ,common) {
	   set cnter 0
	   foreach SQact $SQactive SQnam $SQname {
	      if {([string compare $SQnam $CommonP] == 0) && ([string compare $CommonP NONE] != 0)} {
		 set ltmp [lindex $::tmpGlobalAnalytPar(SQ,active) $cnt]
		 set ltmp [lreplace $ltmp $cnter $cnter $active_state]
		 set ::tmpGlobalAnalytPar(SQ,active) [lreplace $::tmpGlobalAnalytPar(SQ,active) $cnt $cnt $ltmp]
	      }
	      incr cnter
	   }
	   incr cnt
	}
}


#------------------------------------------------------------------------------
# actualize all a_i's an l_i's of actualGlobalPar for which an global parameter
# is selected
#

proc RefreshactualGlobalParCommonPs {args
} {
	foreach type {SD FF SQ} p {a l s} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar($type,param_count)} {incr i} {
			foreach CommonP $::actualGlobalAnalytPar(common_names) {
				if { [string compare $::actualGlobalAnalytPar($type,$p$i,common) $CommonP] == 0 &&
				     [string compare $CommonP NONE] != 0 &&
				     [string compare [lindex $args 0] "$type,$p$i,var"] != 0
				} {
					catch { 
						set ::actualGlobalAnalytPar($type,$p$i,var) [expr $::actualGlobalAnalytPar($type,$p$i,factor) * $::actualGlobalAnalytPar($CommonP)] 
					}
				}
			}
		}
	}
}


#----------------------------------------------------------------------------
# parameter a_i and l_i of actualGlobalPar are set in case a global parameter 
# is selected
#
proc RefreshactualGlobalAnalytParVar {CommonP Rtyp Rvarname Rdirection
} {
	if {[string compare $Rdirection up] == 0} {
	   set tmpfac $::GlobalAnalytPar(par_x_X)
	} else {
	   set tmpfac 1.0/$::GlobalAnalytPar(par_x_X)
	}
	if {[string compare $CommonP NONE] != 0} {
	   set tmpfloat [expr 1.0*$::actualGlobalAnalytPar($CommonP)*$tmpfac]
	   set ::actualGlobalAnalytPar($CommonP) $tmpfloat
	} else {
	   set ::actualGlobalAnalytPar($Rtyp,$Rvarname,var) [expr 1.0*$::actualGlobalAnalytPar($Rtyp,$Rvarname,var)*$tmpfac]
	   return
	}
	foreach type {SD FF SQ} p {a l s} {
		for {set i 1} {$i <= $::actualGlobalAnalytPar($type,param_count)} {incr i} {
			if { [string compare $::actualGlobalAnalytPar($type,$p$i,common) $CommonP] == 0
			} {
				set ::actualGlobalAnalytPar($type,$p$i,var) [format "%g" [expr 1.0*$::actualGlobalAnalytPar($type,$p$i,factor)*$tmpfloat]]
			}
		}
	}
}


#------------------------------------------------------------------------------
# actualize the list of global parameters if a NEW one is selected or enables 
# or disables if NONE or a global parameter P_i is selected
#
proc CommonCmd {typ wid varname
} {
	set ind [format %s%s%s%s $typ "," $varname ",common" ]
	if {[string compare $::actualGlobalAnalytPar($ind) NEW] == 0} {
	    incr ::actualGlobalAnalytPar(common_i) 
	    lappend ::actualGlobalAnalytPar(common_names) "P$::actualGlobalAnalytPar(common_i)"
	    for {set i 1} {$i <= $::actualGlobalAnalytPar(SD,param_count)} {incr i} {
	       set tmpstr [format %s%s%s .analytical.fitpar.sd.lay.a $i list]
	       $tmpstr configure -values $::actualGlobalAnalytPar(common_names)
	    }
	    for {set i 1} {$i <= $::actualGlobalAnalytPar(FF,param_count)} {incr i} {
	       set tmpstr [format %s%s%s .analytical.fitpar.ff.lay.l $i list]
	       $tmpstr configure -values $::actualGlobalAnalytPar(common_names)
	    }
	    for {set i 1} {$i <= $::actualGlobalAnalytPar(SQ,param_count)} {incr i} {
	       set tmpstr [format %s%s%s .analytical.fitpar.sq.lay.s $i list]
	       $tmpstr configure -values $::actualGlobalAnalytPar(common_names)
	    }
	    set ::actualGlobalAnalytPar($ind) P$::actualGlobalAnalytPar(common_i)
	    set tmpexprstr "1.0*$::actualGlobalAnalytPar($typ,$varname,var)/$::actualGlobalAnalytPar($typ,$varname,factor)"
	    set ::actualGlobalAnalytPar(P$::actualGlobalAnalytPar(common_i)) [expr $tmpexprstr]
	}
	set entryname  [format %s%s $wid.$varname entry]
	set factorname [format %s%s $wid.$varname factor]
	if {![string equal -length 4 $::actualGlobalAnalytPar($ind) NONE]
	} { 
	    set ::actualGlobalAnalytPar($typ,$varname,var) [expr $::actualGlobalAnalytPar($::actualGlobalAnalytPar($ind))*$::actualGlobalAnalytPar($typ,$varname,factor)]
	}
}

#------------------------------------------------------------------------------
# create the array holding values of fitparameter for the fit of a scattering
# curve with a given analytical size distribution and a given analytical form
# factor. 
proc create_GlobalAnalytPar {AnalytPar
} {
	upvar $AnalytPar Par

	set Par(w) ""
	set Par(file) unknown.par
	set Par(w_AnalytPar) ""
	set Par(resolution) no
	set Par(geometrical/datafile) yes
	set Par(w_MomentsAnalytPar) ""
	set Par(GraphName)  AnalytPar

	set Par(chisq) 0
	set Par(reducedchisq) 0
	set Par(Q) 0
	set Par(R) 0
	set Par(wR) 0
        set Par(varianceOFfit) 0
	set Par(common_i) 0
	set Par(common_names) {NONE NEW}

	init_analytpar Par 1; # global
	add_analytpar Par 1; # global

	set Par(actual_SD) 1   ;# No of size distribution of which its values 
			       ;# are displayed
	set Par(old_actual_SD) 1

}

#------------------------------------------------------------------------------
#
proc put_GlobalAP {analytPar
} {
	upvar $analytPar Par

	add_analytpar Par 1; # global

	#print_analytpar Par
}

#------------------------------------------------------------------------------
# delete "index"-th size distribution out of the list actualAnalytPar
# possible values of index are 1 to max_SD
#
proc del_GlobalAP {AnalytPar index
} {
	upvar $AnalytPar Par

	set DS1        [lrange $Par(dataset)   0      [expr $index - 2]       ]
	set DS2        [lrange $Par(dataset)   $index [expr $Par(max_SD) - 1] ]
	set DL1        [lrange $Par(datalabel) 0      [expr $index - 2]       ]
	set DL2        [lrange $Par(datalabel) $index [expr $Par(max_SD) - 1] ]

	set FFl1       [lrange $Par(FF,l)       0      [expr $index - 2]       ]
	set FFl2       [lrange $Par(FF,l)       $index [expr $Par(max_SD) - 1] ]
	set FFerr1     [lrange $Par(FF,err)     0      [expr $index - 2]       ]
	set FFerr2     [lrange $Par(FF,err)     $index [expr $Par(max_SD) - 1] ]
	set FFcommon1  [lrange $Par(FF,common)  0      [expr $index - 2]       ]
	set FFcommon2  [lrange $Par(FF,common)  $index [expr $Par(max_SD) - 1] ]
	set FFfactor1  [lrange $Par(FF,factor)  0      [expr $index - 2]       ]
	set FFfactor2  [lrange $Par(FF,factor)  $index [expr $Par(max_SD) - 1] ]
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
	set SDcommon1  [lrange $Par(SD,common)  0      [expr $index - 2]       ]
	set SDcommon2  [lrange $Par(SD,common)  $index [expr $Par(max_SD) - 1] ]
	set SDfactor1  [lrange $Par(SD,factor)  0      [expr $index - 2]       ]
	set SDfactor2  [lrange $Par(SD,factor)  $index [expr $Par(max_SD) - 1] ]
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
	set SQcommon1  [lrange $Par(SQ,common)  0      [expr $index - 2]       ]
	set SQcommon2  [lrange $Par(SQ,common)  $index [expr $Par(max_SD) - 1] ]
	set SQfactor1  [lrange $Par(SQ,factor)  0      [expr $index - 2]       ]
	set SQfactor2  [lrange $Par(SQ,factor)  $index [expr $Par(max_SD) - 1] ]
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

	set substrSDFF1 [lrange $Par(substrSDFF) 0      [expr $index - 2]       ]
	set substrSDFF2 [lrange $Par(substrSDFF) $index [expr $Par(max_SD) - 1] ]
	set fitSDFF1    [lrange $Par(fitSDFF)    0      [expr $index - 2]       ]
	set fitSDFF2    [lrange $Par(fitSDFF)    $index [expr $Par(max_SD) - 1] ]
	set calcSDFF1   [lrange $Par(calcSDFF)   0      [expr $index - 2]       ]
	set calcSDFF2   [lrange $Par(calcSDFF)   $index [expr $Par(max_SD) - 1] ]

	if {$index == 1} {

	   set Par(dataset)   $DS2
	   set Par(datalabel) $DL2

	   set Par(FF,l)       $FFl2
	   set Par(FF,err)     $FFerr2
	   set Par(FF,common)  $FFcommon2
	   set Par(FF,factor)  $FFfactor2
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
	   set Par(SD,common)  $SDcommon2
	   set Par(SD,factor)  $SDfactor2
	   set Par(SD,min)     $SDmin2
	   set Par(SD,max)     $SDmax2
	   set Par(SD,limits)  $SDlimits2
	   set Par(SD,active)  $SDactive2
	   set Par(SD,typestr) $SDtypestr2

	   set Par(SQ,s)       $SQs2
	   set Par(SQ,err)     $SQerr2
	   set Par(SQ,common)  $SQcommon2
	   set Par(SQ,factor)  $SQfactor2
	   set Par(SQ,min)     $SQmin2
	   set Par(SQ,max)     $SQmax2
	   set Par(SQ,limits)  $SQlimits2
	   set Par(SQ,active)  $SQactive2
	   set Par(SQ,typestr) $SQtypestr2
	   set Par(SQ,how)     $SQhow2

	   set Par(substrSDFF) $substrSDFF2
	   set Par(fitSDFF)    $fitSDFF2
	   set Par(calcSDFF)   $calcSDFF2

	} elseif {$index == $Par(max_SD)} {

	   set Par(dataset)   $DS1
	   set Par(datalabel) $DL1

	   set Par(FF,l)       $FFl1       
	   set Par(FF,err)     $FFerr1  
	   set Par(FF,common)  $FFcommon1
	   set Par(FF,factor)  $FFfactor1   
	   set Par(FF,min)     $FFmin1     
	   set Par(FF,max)     $FFmax1     
	   set Par(FF,limits)  $FFlimits1
	   set Par(FF,active)  $FFactive1  
	   set Par(FF,distr)   $FFdistr1   
	   set Par(FF,typestr) $FFtypestr1 

	   set Par(SD,a)       $SDa1       
	   set Par(SD,err)     $SDerr1
	   set Par(SD,common)  $SDcommon1
	   set Par(SD,factor)  $SDfactor1     
	   set Par(SD,min)     $SDmin1     
	   set Par(SD,max)     $SDmax1     
	   set Par(SD,limits)  $SDlimits1
	   set Par(SD,active)  $SDactive1  
	   set Par(SD,typestr) $SDtypestr1

	   set Par(SQ,s)       $SQs1       
	   set Par(SQ,err)     $SQerr1
	   set Par(SQ,common)  $SQcommon1
	   set Par(SQ,factor)  $SQfactor1     
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

	   set Par(substrSDFF) $substrSDFF1
	   set Par(fitSDFF)    $fitSDFF1
	   set Par(calcSDFF)   $calcSDFF1 

	} else {

	   set Par(dataset)   [concat $DS1     $DS2        ]
	   set Par(datalabel) [concat $DL1     $DL2        ]

	   set Par(FF,l)       [concat $FFl1       $FFl2       ]
	   set Par(FF,err)     [concat $FFerr1     $FFerr2     ]
	   set Par(FF,common)  [concat $FFcommon1  $FFcommon2  ]
	   set Par(FF,factor)  [concat $FFfactor1  $FFfactor2  ]
	   set Par(FF,min)     [concat $FFmin1     $FFmin2     ]
	   set Par(FF,max)     [concat $FFmax1     $FFmax2     ]
	   set Par(FF,limits)  [concat $FFlimits1  $FFlimits2  ]
	   set Par(FF,active)  [concat $FFactive1  $FFactive2  ]
	   set Par(FF,distr)   [concat $FFdistr1   $FFdistr2   ]
	   set Par(FF,typestr) [concat $FFtypestr1 $FFtypestr2 ]

	   set Par(SD,a)       [concat $SDa1       $SDa2       ]
	   set Par(SD,err)     [concat $SDerr1     $SDerr2     ]
	   set Par(SD,common)  [concat $SDcommon1  $SDcommon2  ]
	   set Par(SD,factor)  [concat $SDfactor1  $SDfactor2  ]
	   set Par(SD,min)     [concat $SDmin1     $SDmin2     ]
	   set Par(SD,max)     [concat $SDmax1     $SDmax2     ]
	   set Par(SD,limits)  [concat $SDlimits1  $SDlimits2  ]
	   set Par(SD,active)  [concat $SDactive1  $SDactive2  ]
	   set Par(SD,typestr) [concat $SDtypestr1 $SDtypestr2 ]

	   set Par(SQ,s)       [concat $SQs1       $SQs2       ]
	   set Par(SQ,err)     [concat $SQerr1     $SQerr2     ]
	   set Par(SQ,common)  [concat $SQcommon1  $SQcommon2  ]
	   set Par(SQ,factor)  [concat $SQfactor1  $SQfactor2  ]
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

	   set Par(substrSDFF) [concat $substrSDFF1 $substrSDFF2  ]
	   set Par(fitSDFF)    [concat $fitSDFF1    $fitSDFF2  ]
	   set Par(calcSDFF)   [concat $calcSDFF1   $calcSDFF2 ]
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
proc save_GlobalAP {AnalytPar actualAnalytPar
} {
	upvar $AnalytPar Par
	upvar $actualAnalytPar actualPar

	set Par(common_i) $actualPar(common_i)
	set Par(common_names) $actualPar(common_names)
	for {set i 1} {$i <= $actualPar(common_i)} {incr i} {
	   set Par(P$i) $actualPar(P$i)
	}
	 
	set Par(w) $actualPar(w)
	set Par(w_AnalytPar) $actualPar(w_AnalytPar)
	set Par(w_MomentsAnalytPar) $actualPar(w_MomentsAnalytPar)

	set Par(actual_SD) $actualPar(actual_SD)
	set ti [expr $Par(actual_SD) - 1]

	update_analytpar Par actualPar $ti

#	print_analytpar Par

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

	set Par(dataset)   [lreplace $Par(dataset)   $ti $ti $actualPar(dataset)  ]
	set Par(datalabel) [lreplace $Par(datalabel) $ti $ti $actualPar(datalabel)]

	set Par(substrSDFF) [lreplace $Par(substrSDFF) $ti $ti $actualPar(substrSDFF)]
	set Par(fitSDFF)    [lreplace $Par(fitSDFF)    $ti $ti $actualPar(fitSDFF)]
	set Par(calcSDFF)   [lreplace $Par(calcSDFF)   $ti $ti $actualPar(calcSDFF)]
	set Par(alambda) $actualPar(alambda)
	set Par(error) $actualPar(error)
	assert_dataset_list Par
}

#------------------------------------------------------------------------------
# extract out of the array list AnalytPar of parameters for fitting the 
# scattering intensity the $index-th element ($index-th size distribution) 
# and store them in the  array actualAnalytPar. Function return value is
# 1 for successfull extraction and 0 if it fails. "index" starts from 1.
#
proc GlobalAPindex {AnalytPar actualAnalytPar index
} {
	upvar $AnalytPar Par
	upvar $actualAnalytPar actualPar

	set actualPar(w) $Par(w)
	set actualPar(w_AnalytPar) $Par(w_AnalytPar)
	set actualPar(w_MomentsAnalytPar) $Par(w_MomentsAnalytPar)

	set ti [expr $index-1]

	if {($index < 1) && ($index > $Par(max_SD))} { return 0 }

	set actualPar(chisq)         $Par(chisq)
	set actualPar(reducedchisq)  $Par(reducedchisq)
	set actualPar(Q)             $Par(Q)
	set actualPar(R)             $Par(R)
	set actualPar(wR)            $Par(wR)
        set actualPar(varianceOFfit) $Par(varianceOFfit)

	set actualPar(common_i)     $Par(common_i)
	set actualPar(common_names) $Par(common_names)
	for {set i 1} {$i <= $actualPar(common_i)} {incr i} {
	   set actualPar(P$i) $Par(P$i)
	}

	set actualPar(actual_SD)  $index
	set Par(actual_SD)        $index
	set Par(old_actual_SD)    $index
	set actualPar(old_actual_SD) $Par(old_actual_SD)

	set actualPar(max_SD)     $Par(max_SD)
	set actualPar(dataset)   [lindex $Par(dataset)   $ti]
	set actualPar(datalabel) [lindex $Par(datalabel) $ti]

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
	set actualPar(substrSDFF) [lindex  $Par(substrSDFF) $ti]
	set actualPar(fitSDFF)    [lindex  $Par(fitSDFF)    $ti]
	set actualPar(calcSDFF)   [lindex  $Par(calcSDFF)   $ti]

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
				  set actualPar(FF,l6,label) "n"
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
				  set actualPar(FF,l6,label) "sigma_n ="
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
	"SuperparamagneticFFAniso" { set actualPar(FF,l1,label) "R ="
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
	"SuperparamagneticFFIso" { set actualPar(FF,l1,label) "R ="
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
	"SuperparamagneticFFCrossTerm" { set actualPar(FF,l1,label) "R ="
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
				  set actualPar(FF,l5,label) "L ="
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
	"LayeredCentroSymmetricXS" { set actualPar(FF,l1,label) "L_out ="
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
	  "BeaucageExpPowLaw2"  { set actualPar(FF,l1,label) "G ="
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
	   "BeaucageExpPowLaw"  { set actualPar(FF,l1,label) "G ="
				  set actualPar(FF,l2,label) "B ="
				  set actualPar(FF,l3,label) "Gs ="
				  set actualPar(FF,l4,label) "Bs ="
				  set actualPar(FF,l5,label) "Rg ="
				  set actualPar(FF,l6,label) "Rs ="
				  set actualPar(FF,l7,label) "k ="
				  set actualPar(FF,l8,label) "ks ="
				  set actualPar(FF,l9,label) "P ="
				  set actualPar(FF,l10,label) "Ps ="
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
				  set actualPar(FF,l3,label) "Dl ="
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
	"SuperParStroboPsiSQL2x" { set actualPar(FF,l1,label) "phi0 ="
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
	"SuperParStroboPsiSQp_0" { set actualPar(FF,l1,label) "phi0 ="
				  set actualPar(FF,l2,label) "phi0SQ ="
				  set actualPar(FF,l3,label) "f ="
				  set actualPar(FF,l4,label) "B0 ="
				  set actualPar(FF,l5,label) "B_SQ ="
				  set actualPar(FF,l6,label) "SQ ="
				  set actualPar(FF,l7,label) "bS ="
				  set actualPar(FF,l8,label) "Knuc_Kmag ="
				  set actualPar(FF,l9,label) "mu_kT ="
				  set actualPar(FF,l10,label) "SD ="
				}
	"SuperParStroboPsiSQp_90" { set actualPar(FF,l1,label) "phi0 ="
				  set actualPar(FF,l2,label) "phi0SQ ="
				  set actualPar(FF,l3,label) "f ="
				  set actualPar(FF,l4,label) "B0 ="
				  set actualPar(FF,l5,label) "B_SQ ="
				  set actualPar(FF,l6,label) "SQ ="
				  set actualPar(FF,l7,label) "bS ="
				  set actualPar(FF,l8,label) "Knuc_Kmag ="
				  set actualPar(FF,l9,label) "mu_kT ="
				  set actualPar(FF,l10,label) "SD ="
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
	   "Robertus4"          { set actualPar(FF,l1,label) "Rmean ="
				  set actualPar(FF,l2,label) "dR ="
				  set actualPar(FF,l3,label) "sigma ="
				  set actualPar(FF,l4,label) "b ="
				  set actualPar(FF,l5,label) "P ="
				  set actualPar(FF,l6,label) "fp ="
				  set actualPar(FF,l7,label) "P0 ="
				  set actualPar(FF,l8,label) "T ="
				  set actualPar(FF,l9,label) "tauB0 ="
				  set actualPar(FF,l10,label) "eta_solvent ="
				}
	   "Robertus3"          { set actualPar(FF,l1,label) "Rmean ="
				  set actualPar(FF,l2,label) "Delta ="
				  set actualPar(FF,l3,label) "sigma ="
				  set actualPar(FF,l4,label) "dTheta_dP ="
				  set actualPar(FF,l5,label) "P ="
				  set actualPar(FF,l6,label) "fp ="
				  set actualPar(FF,l7,label) "L ="
				  set actualPar(FF,l8,label) "T ="
				  set actualPar(FF,l9,label) "Theta0 ="
				  set actualPar(FF,l10,label) "eta_solvent ="
				}
	   "Robertus2"          { set actualPar(FF,l1,label) "Rmean ="
				  set actualPar(FF,l2,label) "dR ="
				  set actualPar(FF,l3,label) "sigma ="
				  set actualPar(FF,l4,label) "tau ="
				  set actualPar(FF,l5,label) "P ="
				  set actualPar(FF,l6,label) "fp ="
				  set actualPar(FF,l7,label) "eta_core ="
				  set actualPar(FF,l8,label) "T ="
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
				  set actualPar(FF,l2,label) "n_agg ="
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
   "Thin Square Well Potential" { set actualPar(SQ,s1,label) "RHS ="
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
	   "Mass Fractal (OverlapSph Cut-Off)"\
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
	      "VoigtPeak"       { set actualPar(SQ,s1,label) "A ="
				  set actualPar(SQ,s2,label) "mean ="
				  set actualPar(SQ,s3,label) "sigma ="
				  set actualPar(SQ,s4,label) "gamma ="
				  set actualPar(SQ,s5,label) "c0 ="
				  set actualPar(SQ,s6,label) ""
				  set actualPar(SQ,s7,label) ""
				  set actualPar(SQ,s8,label) ""
				  set actualPar(SQ,s9,label) ""
				  set actualPar(SQ,s10,label) ""
				} #
#	    "Correlation Hole"  { set actualPar(SQ,s1,label) "h ="
#				  set actualPar(SQ,s2,label) "eta ="
#				  set actualPar(SQ,s3,label) ""
#				  set actualPar(SQ,s4,label) ""
#				  set actualPar(SQ,s5,label) ""
#				  set actualPar(SQ,s6,label) ""
#				  set actualPar(SQ,s7,label) ""
#				  set actualPar(SQ,s8,label) ""
#				  set actualPar(SQ,s9,label) ""
#				  set actualPar(SQ,s10,label) ""
#				}
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
	  "Macro Ion (HP)" \
				{ set actualPar(SQ,s1,label) {T \[°C\] =}
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
	  "Hayter Penfold RMSA" \
				{ set actualPar(SQ,s1,label) {RHS \[nm\] =}
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
	   default              { ap_set_param_labels actualPar SQ }
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
			    set actualPar(SD,a4,label) "R0_1 ="
			    set actualPar(SD,a5,label) "N_2 ="
			    set actualPar(SD,a6,label) "s_2 ="
			    set actualPar(SD,a7,label) "p_2 ="
			    set actualPar(SD,a8,label) "R0_2"
			    set actualPar(SD,a9,label) ""
			    set actualPar(SD,a10,label) ""
			  }
	   "GEX"          { set actualPar(SD,a1,label) "N ="
			    set actualPar(SD,a2,label) "gamma ="
			    set actualPar(SD,a3,label) "beta ="
			    set actualPar(SD,a4,label) "lambda =" 
			    set actualPar(SD,a5,label) "R0 ="
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
			    set actualPar(SD,a3,label) "X0 ="
			    set actualPar(SD,a4,label) "Xmax ="
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
			    set actualPar(SD,a3,label) "R0 ="
			    set actualPar(SD,a4,label) ""
			    set actualPar(SD,a5,label) ""
			    set actualPar(SD,a6,label) ""
			    set actualPar(SD,a7,label) ""
			    set actualPar(SD,a8,label) ""
			    set actualPar(SD,a9,label) ""
			    set actualPar(SD,a10,label) ""
			  }
	   "Schulz-Zimm" { set actualPar(SD,a1,label) "N ="
			    set actualPar(SD,a2,label) "Ra ="
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

	ap_update_param_count actualPar SD
	ap_update_param_count actualPar SQ
	ap_update_param_count actualPar FF

	foreach scat_contr {FF SQ SD} varname {l s a} {
	   set tmpvar [lindex $Par(${scat_contr},${varname}) $ti]
	   set tmperr [lindex $Par(${scat_contr},err)        $ti]
	   set tmpmin [lindex $Par(${scat_contr},min)        $ti]
	   set tmpmax [lindex $Par(${scat_contr},max)        $ti]
	   set tmplim [lindex $Par(${scat_contr},limits)     $ti]
	   set tmpact [lindex $Par(${scat_contr},active)     $ti]
	   if {[string compare $scat_contr FF] == 0} {
	      set tmpdis [lindex $Par(${scat_contr},distr)      $ti]
	   }
	   set tmpfac [lindex $Par(${scat_contr},factor)     $ti]
	   set tmpcom [lindex $Par(${scat_contr},common)     $ti]

	   for {set i 1} {$i <= $actualPar($scat_contr,param_count)} {incr i} {
	       if {[llength $tmpvar] == 0} {error "tmpvar ($tmpvar) is empty"}
	       set actualPar(${scat_contr},${varname}${i},var)    [lindex $tmpvar [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},err)    [lindex $tmperr [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},min)    [lindex $tmpmin [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},max)    [lindex $tmpmax [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},limits) [lindex $tmplim [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},active) [lindex $tmpact [expr $i-1]]
	       if {[string compare $scat_contr FF] == 0} {
		  set actualPar(${scat_contr},${varname}${i},distr)  [lindex $tmpdis [expr $i-1]]
	       }
	       set actualPar(${scat_contr},${varname}${i},factor) [lindex $tmpfac [expr $i-1]]
	       set actualPar(${scat_contr},${varname}${i},common) [lindex $tmpcom [expr $i-1]]
	   }
	}

	set actualPar(alambda) $Par(alambda);
	set actualPar(error) $Par(error);
	RefreshactualGlobalParCommonPs
	return 1
}
#------------------------------------------------------------------------------
# Check if common parameter is equal "NONE" or "P1", "P2" etc.
# In this case enable/disable entry and factor
proc enable_disable_Common {AactualAnalytPar
} {
	upvar $AactualAnalytPar actualAnalytPar
	foreach type_l {SD FF SQ} type_s {sd ff sq} p {a l s} {
		for {set i 1} {$i <= $actualAnalytPar($type_l,param_count)} {incr i} {
			CommonCmd "$type_l" ".analytical.fitpar.$type_s.lay" $p$i
		}
	}
}

proc set_actualGlobalAP {AactualAnalytPar {guiupdata yes}} {
	foreach type {SD SQ FF} { set_actualAP_labels $type $AactualAnalytPar }
	if ($guiupdata) {
	    foreach type {SD SQ FF} { build_param_gui $type $AactualAnalytPar 1 }
	}
	foreach type {SD SQ FF} { set_actualAP_config $type $AactualAnalytPar 1 }
	fiterror_update
}


#------------------------------------------------------------------------------
#

proc set_GlobalalambdaCmd { name1 name2 op } {
	set ::GlobalAnalytPar(alambda) -1
	set ::tmpGlobalAnalytPar(alambda) -1
	set ::actualGlobalAnalytPar(alambda) -1
}

proc ask4dataset {args
} {
	if {[winfo exists .ask4dataset]} {destroy .ask4dataset}
	toplevel .ask4dataset
	set w .ask4dataset
	wm geometry $w
	wm title .ask4dataset "chose data set nr"

	set simorfit simulate
	if {[llength $args] > 0} {set simorfit [lindex $args 0]}
	set ::ask4dataset_var(simorfit) $simorfit

	frame $w.question
	label $w.question.text -text "Select the data set to which the scattering contribution will be assigned:" -anchor w
	pack $w.question.text  -fill both -expand yes
	frame $w.dataset
	frame $w.button 
	pack $w.question $w.dataset $w.button -fill both -expand yes -pady 4

	set ::ask4dataset_var(datasetlabel) {}
	if {[string compare $simorfit simulate] == 0} {
	    set Ndatasets  $::ask4dataset_var(N_datasets)
	} else {
	    set Ndatasets $::addsasfit(Nth,n)
	}
	for {set i 1} {$i <= $Ndatasets} {incr i} {
	   lappend ::ask4dataset_var(datasetlabel) $i
	}
	set wds $w.dataset
	SpinBox $wds.datasetNo \
		 -width 4  \
		 -highlightthickness 0 \
		 -labelwidth 12 \
		 -textvariable ::ask4dataset_var(dataset) \
		 -label "Data set Nr:" -labelanchor e \
		 -values $::ask4dataset_var(datasetlabel) \
		 -modifycmd {
		       global ::addsasfit
		       global ::ask4dataset_var
		       if {[string compare $::addsasfit(simorfit) fit] == 0} {
			   set ::ask4dataset_var(datalabel) [lindex $::addsasfit(Nth,filelabel) [expr $::ask4dataset_var(dataset)-1]]
		       } else {
			   set ::actualGlobalAnalytPar(datalabel) "$::actualGlobalAnalytPar(datalabel,$::ask4dataset_var(dataset))"
		       }
		   }
	pack $wds.datasetNo  -fill x -anchor w -padx 1m -pady 1 -side left
	label $wds.datasetlabel_l -text "data set label:" -anchor e
	if {[string compare $::addsasfit(simorfit) fit] == 0} {
		label $wds.datasetlabel_v -relief sunken \
			-textvariable ::ask4dataset_var(datalabel) \
			-width 32 -anchor w
	} else {
		label $wds.datasetlabel_v -relief sunken \
			-textvariable ::actualGlobalAnalytPar(datalabel) \
			-width 32
	}

	pack $wds.datasetlabel_l $wds.datasetlabel_v -fill x -anchor w -padx 1m -pady 1 -side left

	button $w.button.ok \
	   -text "Select"  \
	   -command {
	       global ::ask4dataset_var
	       set ::ask4dataset_var(go_on) 1
	       destroy .ask4dataset
	   }
	button $w.button.cancel -text "Cancel" -command {
	    global ::ask4dataset_var
	    set ::ask4dataset_var(go_on) 0
	    destroy .ask4dataset
	}

	pack $w.button.ok $w.button.cancel -side left -fill x -expand yes
	raise $w
	focus $w
}

proc update_datasets_multi {type} {
	set_actualAP_gen $type ::actualGlobalAnalytPar 1
	cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
        set_GloballimitsCmd
}

#------------------------------------------------------------------------------
# user interface for the paramaters of the fit of a scattering curve with 
# analytical given size distributions (lognormal distribution, ...) and a 
# set of given form factors.
# 
proc analyticalGlobalSDCmd {simorfit
} {

	if {[analytical_menu_bar $simorfit ::GlobalAnalytPar ::tmpGlobalAnalytPar \
	        ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar ::addsasfit \
		::GlobalFitIQGraph]
	} { return }
	set w .analytical
#
# buttons for choosing No of size distributions
#
	frame $w.whichSD  -borderwidth 1
	pack $w.whichSD -pady 1m -fill x

	set ::nomenu [tk_optionMenu $w.whichSD.no ::actualGlobalAnalytPar(actual_SD) 1]
	$::nomenu entryconfigure 0 -command {
		  RefreshGlobalAPEntryConfigure
		}
        set tmp $::actualGlobalAnalytPar(actual_SD)
	for {set i 2} {$i <= $::actualGlobalAnalytPar(max_SD)} {incr i} {
		  $::nomenu add radiobutton \
		     -variable ::actualGlobalAnalytPar(actual_SD) \
		     -label $i
		  $::nomenu entryconfigure [expr $i - 1] -command {
		      RefreshGlobalAPEntryConfigure
		  }
	}

	setTooltip $w.whichSD.no "display parameter of\ni-th scattering contribution"
	$w.whichSD.no configure -highlightthickness 0
	label $w.whichSD.label -text "contribution:"
	pack $w.whichSD.label $w.whichSD.no -in $w.whichSD -side left -padx 1m -pady 1m
	Button $w.whichSD.next -text Next -highlightthickness 0 \
	       -padx 1m -pady 1m -takefocus 1 \
	       -repeatinterval 100 \
	       -repeatdelay  2000 \
	       -command {
		  if {$::actualGlobalAnalytPar(actual_SD) >= $::actualGlobalAnalytPar(max_SD)} {
		      $::nomenu activate 0
		      $::nomenu invoke active
		  } else { 
		      $::nomenu activate $::actualGlobalAnalytPar(actual_SD)
		      $::nomenu invoke active
		  }
	       }

	setTooltip $w.whichSD.next "display parameter of next\nscattering contribution"
	Button $w.whichSD.previous -text Previous -highlightthickness 0 \
	       -padx 1m -pady 1m -takefocus 1\
	       -repeatinterval 100 \
	       -repeatdelay  2000 \
	       -command {
		  if {$::actualGlobalAnalytPar(actual_SD) <= 1} {
		      $::nomenu activate last
		      $::nomenu invoke active 
		  } else { 
		      $::nomenu activate [expr $::actualGlobalAnalytPar(actual_SD)-2]
		      $::nomenu invoke active
		  }
	       }

	setTooltip $w.whichSD.previous "display parameter of previous\nscattering contribution"
	Button $w.whichSD.add -text Add -highlightthickness 0 \
	       -padx 1m -pady 1m -takefocus 1\
	       -repeatinterval 100 \
	       -repeatdelay  2000 \
	       -command { 
		  RefreshactualGlobalParCommonPs
		  set ::tmpGlobalAnalytPar(alambda) -1
		  cp_arr ::actualGlobalAnalytPar tt
		  set ::actualGlobalAnalytPar(old_actual_SD) $::actualGlobalAnalytPar(actual_SD)
		  save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar

		  ask4dataset $::addsasfit(simorfit)
		  tkwait window .ask4dataset

		  if {$::ask4dataset_var(go_on)} {
		     put_GlobalAP ::tmpGlobalAnalytPar
		     set tt(actual_SD)  $::tmpGlobalAnalytPar(max_SD) 
		     save_AP ::tmpGlobalAnalytPar tt
		     GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $::tmpGlobalAnalytPar(max_SD)
		     set ::actualGlobalAnalytPar(old_actual_SD) $::actualGlobalAnalytPar(actual_SD)
		     cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
		     set_GloballimitsCmd
		     $::nomenu add radiobutton \
			-variable ::actualGlobalAnalytPar(actual_SD) \
			-label $::actualGlobalAnalytPar(max_SD) 
		     $::nomenu entryconfigure [expr $::actualGlobalAnalytPar(max_SD)-1] \
			     -command {
				RefreshGlobalAPEntryConfigure
			      }
		     $::nomenu activate [expr $::actualGlobalAnalytPar(max_SD)-1]
		     RefreshactualGlobalParCommonPs
		  }
	       }
	setTooltip $w.whichSD.add "add a set of parameter for\nanother scattering contribution"

	Button $w.whichSD.delete -text Remove -highlightthickness 0 \
	       -padx 1m -pady 1m -takefocus 1\
	       -repeatinterval 100 \
	       -repeatdelay  2000 \
	       -command {
		  RefreshactualGlobalParCommonPs
		  set ::tmpGlobalAnalytPar(alambda) -1
		  if {$::actualGlobalAnalytPar(max_SD) > 1} {
		     set max_SD $::actualGlobalAnalytPar(max_SD)
		     del_GlobalAP ::tmpGlobalAnalytPar $::actualGlobalAnalytPar(actual_SD)
		     if {$max_SD == $::actualGlobalAnalytPar(actual_SD)} {
			incr ::actualGlobalAnalytPar(actual_SD) -1
			$::nomenu delete $::actualGlobalAnalytPar(actual_SD) 
		     } else {
			 for {set i $max_SD} \
			     {$i > $::actualGlobalAnalytPar(actual_SD)} \
			     {incr i -1 } {
				$::nomenu delete [expr $i - 1]
			 }
			for {set i  [expr $::actualGlobalAnalytPar(actual_SD)+1]} \
			    {$i < [expr $max_SD ]} {incr i} {
			       $::nomenu add radiobutton \
				       -variable ::actualGlobalAnalytPar(actual_SD)  \
				       -label [expr $i]
			       $::nomenu entryconfigure $i \
				  -command {
				      RefreshGlobalAPEntryConfigure
				  }
			}
		     }
		     GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $::actualGlobalAnalytPar(actual_SD)
		     cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
		     set_GloballimitsCmd
		     $::nomenu activate [expr $::actualGlobalAnalytPar(actual_SD) - 1]
		     $::nomenu invoke active
		  } else { bell }
	       }
	frame $w.whichSD.fitcalc
	eval checkbutton $w.whichSD.fitcalc.fit "-offvalue yes -onvalue no -takefocus 1\
	     -variable ::actualGlobalAnalytPar(fitSDFF) $::radio_check_button_prop \
	     -text \"fix\" \
	     -command {
			set_actualGlobalAP ::actualGlobalAnalytPar
		      } -highlightthickness 0"
	eval checkbutton $w.whichSD.fitcalc.substr "-offvalue no -onvalue yes -takefocus 1\
	     -variable ::actualGlobalAnalytPar(substrSDFF) $::radio_check_button_prop \
	     -text \"subtract\" \
	     -command {
			set_actualGlobalAP ::actualGlobalAnalytPar
		      } -highlightthickness 0"
	eval checkbutton $w.whichSD.fitcalc.calc "-offvalue no -onvalue yes -takefocus 1 \
	     -variable ::actualGlobalAnalytPar(calcSDFF) $::radio_check_button_prop \
	     -text \"apply\" \
	     -command {
			set_actualGlobalAP ::actualGlobalAnalytPar
		      } -highlightthickness 0"

	grid configure $w.whichSD.fitcalc.fit    -column 0 -row 0 -sticky w
	grid configure $w.whichSD.fitcalc.calc   -column 0 -row 1 -sticky w
	grid configure $w.whichSD.fitcalc.substr -column 1 -row 1 -sticky w

	pack $w.whichSD.previous $w.whichSD.next $w.whichSD.add $w.whichSD.delete \
	     -side left -in $w.whichSD -fill x -expand yes -padx 1m -pady 1m
	pack $w.whichSD.fitcalc \
	     -side right -in $w.whichSD  -padx 1m -pady 1m


#
# frame for chisq and other goodness-of-fit parameters
#
        frame $w.goodnessoffit
        pack $w.goodnessoffit -anchor w -fill x 
	set wg $w.goodnessoffit

	label  $wg.chisq_l -text "chisq:" -anchor e
	label  $wg.chisq_v \
	      -textvariable ::GlobalAnalytPar(chisq) \
	      -width 16 -anchor w
	grid configure $wg.chisq_l -column 0 -row 0 -sticky w
	grid configure $wg.chisq_v -column 1 -row 0 -sticky e
	
	label  $wg.redchisq_l -text "red. chisq:" -anchor e
	label  $wg.redchisq_v \
	      -textvariable ::GlobalAnalytPar(reducedchisq) \
	      -width 16 -anchor w
	grid configure $wg.redchisq_l -column 2 -row 0 -sticky w
	grid configure $wg.redchisq_v -column 3 -row 0 -sticky e
	
	label  $wg.ndata_l -text "data points:" -anchor e
	label  $wg.ndata_v \
	      -textvariable ::GlobalAnalytPar(ndata) \
	      -width 16 -anchor w
	grid configure $wg.ndata_l -column 4 -row 0 -sticky w
	grid configure $wg.ndata_v -column 5 -row 0 -sticky e
	
	label  $wg.mfit_l -text "fit parameters:" -anchor e
	label  $wg.mfit_v \
	      -textvariable ::GlobalAnalytPar(mfit) \
	      -width 16 -anchor w
	grid configure $wg.mfit_l -column 6 -row 0 -sticky w
	grid configure $wg.mfit_v -column 7 -row 0 -sticky e

	label  $wg.r_l -text "R value:" -anchor e
	label  $wg.r_v \
	      -textvariable ::GlobalAnalytPar(R) \
	      -width 16 -anchor w
	grid configure $wg.r_l -column 0 -row 1 -sticky w
	grid configure $wg.r_v -column 1 -row 1 -sticky e
	
	label  $wg.wr_l -text "w. R:" -anchor e
	label  $wg.wr_v \
	      -textvariable ::GlobalAnalytPar(wR) \
	      -width 16 -anchor w
	grid configure $wg.wr_l -column 2 -row 1 -sticky w
	grid configure $wg.wr_v -column 3 -row 1 -sticky e
	
	label  $wg.q_l -text "Q:" -anchor e
	label  $wg.q_v \
	      -textvariable ::GlobalAnalytPar(Q) \
	      -width 16 -anchor w
	grid configure $wg.q_l -column 4 -row 1 -sticky w
	grid configure $wg.q_v -column 5 -row 1 -sticky e
		
	label  $wg.s_l -text "variance of fit:" -anchor e
	label  $wg.s_v \
	      -textvariable ::GlobalAnalytPar(varianceOFfit) \
	      -width 16 -anchor w
	grid configure $wg.s_l -column 6 -row 1 -sticky w
	grid configure $wg.s_v -column 7 -row 1 -sticky e

#	label $wg.empty -text " "
#	grid configure $wg.empty -column 1 -row 2 -sticky e


#
# frames for selecting data set
#
	set ::actualGlobalAnalytPar(datasetlabel) {} 
	for {set i 1} {$i <= $::addsasfit(Nth,n)} {incr i} {
	   lappend ::actualGlobalAnalytPar(datasetlabel) $i
	}
	set ::actualGlobalAnalytPar(dataset) 1
	set ::actualGlobalAnalytPar(datalabel) [lindex $::addsasfit(Nth,filelabel) [expr $::actualGlobalAnalytPar(dataset)-1]]

	frame $w.dataset
	pack $w.dataset -anchor w

	if {[string compare $::addsasfit(simorfit) fit] != 0
	} {
		set_dataset_labels_sim
		frame $w.datasetdef 
		pack $w.datasetdef -fill both 

		label $w.datasetdef.howmany -text "number of scattering curves to simulate"
		button $w.datasetdef.number -textvariable ::ask4dataset_var(N_datasets) \
					-command {DefNoOfCurvesToSim}
		pack $w.datasetdef.howmany $w.datasetdef.number -side left
		set ::actualGlobalAnalytPar(datalabel) "$::actualGlobalAnalytPar(datalabel,$::actualGlobalAnalytPar(dataset))"
	}
	set wds $w.dataset

	SpinBox $wds.datasetNo \
		 -width 5  \
		 -highlightthickness 0 \
		 -labelwidth 12 \
		 -textvariable ::actualGlobalAnalytPar(dataset) \
		 -label "Data set No:" -labelanchor e \
		 -values $::actualGlobalAnalytPar(datasetlabel) \
		 -modifycmd {
		       RefreshactualGlobalParCommonPs
		       if {[string compare $::addsasfit(simorfit) fit] == 0} {
			   set ::actualGlobalAnalytPar(datalabel) [lindex $::addsasfit(Nth,filelabel) [expr $::actualGlobalAnalytPar(dataset)-1]]
		       } else {
			   set ::actualGlobalAnalytPar(datalabel) $::actualGlobalAnalytPar(datalabel,$::actualGlobalAnalytPar(dataset))
		       }
		 }

	pack $wds.datasetNo  -fill x -anchor w -padx 1m -pady 1 -side left
	label $wds.datasetlabel_l -text "data set label:" -anchor e
	label $wds.datasetlabel_v -relief sunken \
	      -textvariable ::actualGlobalAnalytPar(datalabel) \
	      -width 50 -anchor w

	pack $wds.datasetlabel_l $wds.datasetlabel_v \
		-fill x -anchor w -padx 1m -pady 1 -side left
#
# frames of entry fields
#

	set lst [create_analytical_parameter_frame $w $::GlobalAnalytPar(isGlobal)]
	set wn       [lindex $lst 0]
	set wsd      [lindex $lst 1]
	set wff      [lindex $lst 2]
	set wsq      [lindex $lst 3]
	set wsqhow   [lindex $lst 4]

#
# input fields for parameters of size distributions
#

	frame $wsd.wrl
	grid $wsd.wrl -columnspan 7 -row 0 -sticky w

	set root_btn_sd $wsd.wrl.whichsd
	set sdmenu [dropdownl_create $root_btn_sd]
	set ::sasfit(SD,root_menu) $sdmenu

	set menui 0
	set selected_sd -1
	foreach old_sd_type {Monodisperse "A_div_B" Delta LogNorm BiLogNorm GEX GEV \
				gammaSD Triangular fractalSD Uniform Beta Gaussian \
				Maxwell Schulz-Zimm Weibull} {

		dropdownl_add $sdmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar update_datasets_multi \
			$root_btn_sd SD $old_sd_type $old_sd_type
		if { [string compare $old_sd_type $::actualGlobalAnalytPar(SD,typestr)] == 0 } {
			set selected_sd $menui
		}
		incr menui
	}
	set selected_sd [concat $menui $selected_sd]
	set selected_plug [dropdownl_add_entries $sdmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar update_datasets_multi [winfo parent $sdmenu] [winfo parent $sdmenu] [get_old_func_name "size_distrib"] [get_children_idx_by_name "size_distrib"]]
	if { [lindex $selected_sd end] < 0 } {
		set selected_sd [dropdownl_concat_index_lists selected_sd selected_plug]
	}

	$wsd.wrl.whichsd configure -highlightthickness 0
	pack $wsd.wrl.whichsd -side left -padx 1m -pady 1m -in $wsd.wrl

	button $wsd.wrl.rangebutton -text "Parameter Range..." \
	       -highlightthickness 0 -padx 1m -pady 1m\
	       -command SDGlobalFitRangeCmd
	setTooltip $wsd.wrl.rangebutton "define range interval\nfor fitting parameter\nnot available for multiple fit"
	pack $wsd.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wsd.wrl

	label $wsd.text1 -text "\nparameter:"
	label $wsd.text2 -text "\nfit "
	label $wsd.text3 -text "global\nparam.:" -anchor w
	label $wsd.text4 -text "pre-\nfactor:"
	grid $wsd.text1 -column 1 -columnspan 3 -row 1 -sticky w
	grid $wsd.text2 -column 4 -row 1
	grid $wsd.text4 -column 5 -row 1
	grid $wsd.text3 -column 6 -row 1

#
# input fields for parameters of form factors
#
	frame $wff.wrl
	grid $wff.wrl -columnspan 7 -row 0 -sticky w

	set root_btn_ff $wff.wrl.whichff
	set ffmenu [dropdownl_create $root_btn_ff]
	set ::sasfit(FF,root_menu) $ffmenu

	set selected_ff [dropdownl_add_entries_old $ffmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar \
							   update_datasets_multi $root_btn_ff $root_btn_sd FF { \
	{Background Monodisperse} \ 
	{sphshell "Spheres & Shells" { Sphere \
				      {{Spherical Shell i}}\
				      {{Spherical Shell ii}} \
				      {{Spherical Shell iii}} \ 
				      {MultiLamellarVesicle Delta} \
				      {RNDMultiLamellarVesicle Delta} \
				      {RNDMultiLamellarVesicle2 Delta} \ 
				      {MLVesicleFrielinghaus Delta} \
				       BiLayeredVesicle \
				       SphereShell_compr}} \ 
	{polymers_micelles "polymers & micelles" { \ 
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
									  {CylULV+Chains(RW) Delta} }} }} \
	{planar "planar obj." {{homogenousXS Delta} \
			       {SphSh+SD+homoXS Delta} \
			       {EllSh+SD+homoXS Delta} \
			       {EllSh+SD+homoXS(S) Delta} \ 
			       {CylSh+SD+homoXS Delta} \
			       {Disc+homoXS Delta} \
			       {TwoInfinitelyThinPlates Delta} \
			       {LayeredCentroSymmetricXS Delta} \ 
			       {BiLayerGauss Delta}}} \ 
	{magnetic "magnetic objects" {MagneticShellPsi \
				      MagneticShellAniso \
				      MagneticShellCrossTerm \
				      SuperparamagneticFFpsi \ 
				      SuperparamagneticFFAniso \
				      SuperparamagneticFFIso \
				      SuperparamagneticFFCrossTerm}} \ 
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
					{JuelichMicelle Monodisperse} \
					{Francois1 Monodisperse} \
					{CopolymerMicelleR^-a Monodisperse} \ 
					{CopolymerMicelleRod Monodisperse} \
					{CopolymerMicelleEll Monodisperse} \ 
					{CopolymerMicelleCyl Monodisperse} \
					Andrea1 \
					{A(B) Monodisperse} \ 
					{B(B) Monodisperse} \
					{P39 Monodisperse} \
					{julia "Julia's form factors" {{TripleLayeredXS_ULV Monodisperse}}} } } \ 
	}]

	set selected_plug [dropdownl_add_entries $ffmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar update_datasets_multi [winfo parent $ffmenu] [winfo parent $ffmenu] [get_old_func_name "form_fac"] [get_children_idx_by_name "form_fac"]]
	if { [lindex $selected_ff end] < 0 } { 
		set selected_ff [dropdownl_concat_index_lists selected_ff selected_plug]
	}

	$wff.wrl.whichff configure -highlightthickness 0
	pack $wff.wrl.whichff -side left -padx 1m -pady 1m -in $wff.wrl
	button $wff.wrl.rangebutton -text "Parameter Range..." \
	       -highlightthickness 0  -padx 1m -pady 1m \
	       -command FFGlobalFitRangeCmd
	setTooltip $wff.wrl.rangebutton "define range interval\nfor fitting parameter\nnot available for multiple fit"
	pack $wff.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wff.wrl

	label $wff.text1 -text "\nparameter:"
	label $wff.text2 -text "\ndistr"
	label $wff.text3 -text "\nfit "
	label $wff.text4 -text "global\nparam.:" -anchor w
	label $wff.text5 -text "pre-\nfactor: "
	grid  $wff.text1 -column 1 -columnspan 3 -row 1 -sticky w
	grid  $wff.text2 -column 4 -row 1
	grid  $wff.text3 -column 5 -row 1
	grid  $wff.text5 -column 6 -row 1 
	grid  $wff.text4 -column 7 -row 1 

#
# input fields for parameters of structure factor
#

	frame $wsq.wrl
	grid $wsq.wrl -columnspan 7 -row 0 -sticky w

	set root_btn_sq $wsq.wrl.whichsq
	set sqmenu [dropdownl_create $root_btn_sq]
	set ::sasfit(SQ,root_menu) $sqmenu

	set selected_sq [dropdownl_add_entries_old $sqmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar \
		    update_datasets_multi $root_btn_sq $root_btn_sd SQ { \
	None \
	{sticky "Hard & Sticky Hard Sphere" {{{Hard Sphere}} 
                                             {{Sticky Hard Sphere}} \
                                             {{Sticky Hard Sphere 2}} \
					     {{Square Well Potential}} \
                                             {{Thin Square Well Potential}} \
                                             {{Square Well Potential 2}} }} \
	{lamellar "Multi Lamellar Structures" {ThermalDisorder Paracrystalline ModifiedCaille}} \
	{fractal "fractal obj." {{{Mass Fractal (Exp Cut-Off)}} {{Mass Fractal (Exp(-x^a) Cut-Off)}} {{Mass Fractal (Gaussian Cut-Off)}} \
				 {{Mass Fractal (OverlapSph Cut-Off)}} }} \
	{other "other" {Cylinder(PRISM) RPA {{Critical Scattering}} {{Macro Ion (HP)}} {{Hayter Penfold RMSA}} \
			BabaAhmed BabaAhmed2}} \
	}]

	set selected_plug [dropdownl_add_entries $sqmenu ::actualGlobalAnalytPar ::tmpGlobalAnalytPar update_datasets_multi [winfo parent $sqmenu] [winfo parent $sqmenu] [get_old_func_name "struct_fac"] [get_children_idx_by_name "struct_fac"]]
	if { [lindex $selected_sq end] < 0 } {
		set selected_sq [dropdownl_concat_index_lists selected_sq selected_plug]
	}

	$wsq.wrl.whichsq configure -highlightthickness 0
	pack $wsq.wrl.whichsq -side left -padx 1m -pady 1m -in $wsq.wrl
	button $wsq.wrl.rangebutton -text "Parameter Range..." \
	       -highlightthickness 0 -padx 1m -pady 1m\
	       -command SQGlobalFitRangeCmd
	setTooltip $wsq.wrl.rangebutton "define range interval\nfor fitting parameter\nnot available for multiple fit"
	pack $wsq.wrl.rangebutton -side right -fill x -padx 1m -pady 1m -in $wsq.wrl

	label $wsq.text1 -text "\nparameter:"
	label $wsq.text2 -text "\nfit "
	label $wsq.text3 -text "global\nparam.:" -anchor w
	label $wsq.text4 -text "pre-\nfactor: "
	grid  $wsq.text1 -column 1 -columnspan 3 -row 1 -sticky w
	grid  $wsq.text2 -column 4 -row 1
	grid  $wsq.text4 -column 5 -row 1
	grid  $wsq.text3 -column 6 -row 1

#
# input fields for how to calculate the structure factor
#
	radiobutton $wsqhow.sq0 -text "monodisperse approach:\nI(Q) =  <F^2(Q,R)> S(Q)" -value "0" \
				-variable ::actualGlobalAnalytPar(SQ,how) -anchor w -justify left \
				-command {set_GlobalalambdaCmd 1 1 1
					  set_actualGlobalAP ::actualGlobalAnalytPar}
	radiobutton $wsqhow.sq1 -text "decoupling approach:\nI(Q) =  <F^2(Q,R)> + <F(Q,R)>^2 \[S(Q)-1\]" -value "1" \
				-variable ::actualGlobalAnalytPar(SQ,how) -anchor w -justify left \
				-command {set_GlobalalambdaCmd 1 1 1
					  set_actualGlobalAP ::actualGlobalAnalytPar}
	radiobutton $wsqhow.sq2 -text "local monodisperse approximation:\nI(Q) = int(N(R) F^2(Q,R) S(Q,R),dR)" -value "2" \
				-variable ::actualGlobalAnalytPar(SQ,how) -anchor w -justify left \
				-command {set_GlobalalambdaCmd 1 1 1
					  set_actualGlobalAP ::actualGlobalAnalytPar}
	radiobutton $wsqhow.sq3 -text "partial structure factor:\nI(Q) = int(N(R) F^2(Q,R),dR)+\n      int(N(R1) F^2(Q,R1)N(R2) F^2(Q,R2) S(Q,R1,R2),dR1,dR2)" \
				-value "3" -variable ::actualGlobalAnalytPar(SQ,how) -anchor w -justify left \
				-command {set_GlobalalambdaCmd 1 1 1
					  set_actualGlobalAP ::actualGlobalAnalytPar}
	radiobutton $wsqhow.sq4 -text "scaling approximation of partial structure factor:\nI(Q) = int(N(R) F^2(Q,R),dR)+\n      int(N(R1) F^2(Q,R1)N(R2) ((R1+R2)/(<R>))^3F^2(Q,R2) S(Q,R1,R2),dR1,dR2)" \
				-value "4" -variable ::actualGlobalAnalytPar(SQ,how) -anchor w -justify left \
				-command {set_GlobalalambdaCmd 1 1 1
					  set_actualGlobalAP ::actualGlobalAnalytPar}

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

	analytical_widgets_bottom $w $simulate $::GlobalAnalytPar(isGlobal)

	# ensures the bottom doesn't collapse on resize
	pack $w.fitpar -fill both -expand 1

	proc assert_dataset_list { analytpar } {
		upvar $analytpar ap
		if {[llength $ap(dataset)] > $ap(max_SD)
		} {
			set ap(dataset) [lrange \
				$ap(dataset) 0 \
				[expr $ap(max_SD)-1]]
		}
		if {[llength $ap(datalabel)] > $ap(max_SD)
		} {
			set ap(datalabel) [lrange \
				$ap(datalabel) 0 [expr $ap(max_SD)-1]]
		}
	}

	if {!$simulate && [winfo exists $w.adj.calc]} {
		$w.adj.calc configure -command {
			if {$::sasfit(busy)} {
				puts "SASfit is busy"
			} else {
	   	      set ::SASfitprogressbar 0
	      	      set ::SASfitinterrupt 0
                      set ::fitparamguiupdate no
		      sasfit_timer_start "\nStart apply"

#              RefreshactualGlobalParCommonPs
		      if {([string length $::addsasfit(lower,Q)] > 0) && \
			  ([string length $::addsasfit(upper,Q)] > 0) } {
			 set Q {}
			 set I {}
			 set DI {}
			 set res {}
			 foreach xx $::addsasfit(Nth,Q)   \
				 yy $::addsasfit(Nth,I)   \
				 ee $::addsasfit(Nth,DI)  \
				 rr $::addsasfit(Nth,res)    {

			    set Qx   {}
			    set Iy   {}
			    set DIe  {}
			    set resr {}
			    foreach x $xx y $yy e $ee r $rr {
			       if {($x>=$::addsasfit(lower,Q)) && ($x<=$::addsasfit(upper,Q))} {
				  lappend Qx   $x
				  lappend Iy   $y
				  lappend DIe  $e
				  lappend resr $r
			       }
			    }
			    lappend Q   $Qx
			    lappend I   $Iy
			    lappend DI  $DIe
			    lappend res $resr
			 }
		      } else  {
			 set Q   $::addsasfit(Nth,Q)
			 set I   $::addsasfit(Nth,I)
			 set DI  $::addsasfit(Nth,DI)
			 set res $::addsasfit(Nth,res)
		      }
		      $::nomenu activate [expr $::actualGlobalAnalytPar(actual_SD)-1]
		      $::nomenu invoke [expr $::actualGlobalAnalytPar(actual_SD)-1]
		      save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar 
		      cp_arr ::tmpGlobalAnalytPar ::GlobalAnalytPar
		      assert_dataset_list ::GlobalAnalytPar
		      if { $::addsasfit(I_enable)  } {
		      if {    (($::GlobalAnalytPar(error) == 0) && $::addsasfit(DI_enable) )
			   || (($::GlobalAnalytPar(error) != 0) && !$::addsasfit(DI_enable) ) } {
				if {[catch {
					if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
						set IthIres [sasfit_global_iq ::GlobalAnalytPar  \
								[list $Q $I $DI $res]  $::addsasfit(Nth,hide)\
									]
					} else {
						set IthIres [sasfit_global_iq ::GlobalAnalytPar  \
								[list $Q $I $DI] $::addsasfit(Nth,hide)\
									]
					}
				} msg] } {
					bgerror $msg
					set ::sasfit(busy) false
					return
				}
		     sasfit_timer_stop "Apply" "finished" ""
		     sasfit_timer_start "\nStart plotting"
			 set ::addsasfit(Nth,Ith)     [lindex $IthIres 0] 
			 set ::addsasfit(Nth,sub,Ith) [lindex $IthIres 1] 
			 set ::addsasfit(Nth,sub,Ih)  [lindex $IthIres 2]
			 set ::addsasfit(Nth,DIh)     [lindex $IthIres 3]
			 set ::addsasfit(Nth,sub,res) [lindex $IthIres 4]
			 set ::addsasfit(Nth,Qth)     $Q

			 set sub {}
			 foreach tmpQ $Q {
			     lappend sub no
			 }
			 foreach datset $::GlobalAnalytPar(dataset)  minus $::GlobalAnalytPar(substrSDFF) {
			     if {$minus} {
				 set sub [lreplace $sub [expr $datset-1] [expr $datset-1] $minus]
			     }
			 }
			 NewGlobalFitDataCmd ::addsasfit $sub plustheory
		      } else { tk_messageBox -icon error -title ERROR \
				 -message "NO error values of data points are avaliable or assumed" 
		      }
		      } else { tk_messageBox -icon error \
					     -title ERROR -message "NO data loaded" 
		      }
		      RefreshAnalytParDataTab ::GlobalAnalytPar /Global
		      sasfit_timer_stop "Plotting" "finished" ""
                      set ::fitparamguiupdate yes
		    }
			}
	}

	if {[winfo exists $w.adj.step]} {
		$w.adj.step configure -command {
			if {$::sasfit(busy)} {
				puts "SASfit is busy"
			} else {
                      set ::fitparamguiupdate no
                      set ::SASfitprogressbar 0
	       	      set ::SASfitinterrupt 0
		      RefreshactualGlobalParCommonPs
		      if {([string length $::addsasfit(lower,Q)] > 0) && \
			  ([string length $::addsasfit(upper,Q)] > 0) } {
			 set Q {}
			 set I {}
			 set DI {}
			 set res {}
			 foreach xx $::addsasfit(Nth,Q)   \
				 yy $::addsasfit(Nth,I)   \
				 ee $::addsasfit(Nth,DI)  \
				 rr $::addsasfit(Nth,res)    {

			    set Qx   {}
			    set Iy   {}
			    set DIe  {}
			    set resr {}
			    foreach x $xx y $yy e $ee r $rr {
			       if {($x>=$::addsasfit(lower,Q)) && ($x<=$::addsasfit(upper,Q))} {
				  lappend Qx   $x
				  lappend Iy   $y
				  lappend DIe  $e
				  lappend resr $r
			       }
			    }
			    lappend Q   $Qx
			    lappend I   $Iy
			    lappend DI  $DIe
			    lappend res $resr
			 }
		      } else  {
			 set Q   $::addsasfit(Nth,Q)
			 set I   $::addsasfit(Nth,I)
			 set DI  $::addsasfit(Nth,DI)
			 set res $::addsasfit(Nth,res)
		      }

		      set alambda $::GlobalAnalytPar(alambda)
		      set ::tmpGlobalAnalytPar(alambda) $alambda
		      save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar 
		      cp_arr ::tmpGlobalAnalytPar ::GlobalAnalytPar
		      set ::GlobalAnalytPar(alambda) $alambda
		      set ::tmpGlobalAnalytPar(alambda) $alambda
		      if { $::addsasfit(I_enable) } {
			 if {$alambda == -1} {
			       set ::stepfit(oalambda) ::GlobalAnalytPar(alambda)
			     if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI $res] $::addsasfit(Nth,hide) ::stepfit \
				       ]
			    } else {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI] $::addsasfit(Nth,hide) ::stepfit \
				       ]
			    }

			    set ::addsasfit(Nth,Ith)     [lindex $Ith 0] 
			    set ::addsasfit(Nth,sub,Ith) [lindex $Ith 1] 
			    set ::addsasfit(Nth,sub,Ih)  [lindex $Ith 2]
			    set ::addsasfit(Nth,DIh)     [lindex $Ith 3]
			    set ::addsasfit(Nth,sub,res) [lindex $Ith 4]
			    set ::addsasfit(Nth,Qth)     $Q
			    set ::stepfit(itst) 0
			    set ::stepfit(k)    0
			    set ::stepfit(ochisq) $::stepfit(chisq)
			 } elseif {$alambda == 0} {
			    set alambda -1
			    set ::stepfit(oalambda) ::GlobalAnalytPar(alambda)
	 #                   set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
	 #                                   [list $Q $I $DI]  $::addsasfit(Nth,hide) ::stepfit \
	 #                           ]
			    if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
			       set IthIres [sasfit_global_iq ::GlobalAnalytPar  \
						    [list $Q $I $DI $res]  $::addsasfit(Nth,hide) ::stepfit\
					   ]
			    } else {
			       set IthIres [sasfit_global_iq ::GlobalAnalytPar  \
						    [list $Q $I $DI] $::addsasfit(Nth,hide) ::stepfit \
					   ]
			    }
#                    set ::addsasfit(Nth,Ith) [lindex $IthIres 0] 
#                    set ::addsasfit(Nth,Qth) $Q
			    set ::stepfit(itst) 0
			    set ::stepfit(k)    0
			    set ::stepfit(chisq) $::GlobalAnalytPar(chisq)
			    set ::stepfit(ochisq) $::stepfit(chisq)
			 } elseif {$::stepfit(itst) < 2} {
			    incr ::stepfit(k)
			    set ::stepfit(chisq) $::GlobalAnalytPar(chisq)
			    set ::stepfit(ochisq) $::stepfit(chisq)
			    set ::stepfit(oalambda) $::GlobalAnalytPar(alambda)
			    if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI $res]  $::addsasfit(Nth,hide)\
					       ::stepfit \
				       ]
			    } else {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI]  $::addsasfit(Nth,hide)\
					       ::stepfit \
				       ]
			    }
			    set ::addsasfit(Nth,Ith)     [lindex $IthIres 0] 
			    set ::addsasfit(Nth,sub,Ith) [lindex $IthIres 1] 
			    set ::addsasfit(Nth,sub,Ih)  [lindex $IthIres 2]
			    set ::addsasfit(Nth,DIh)     [lindex $IthIres 3]
			    set ::addsasfit(Nth,sub,res) [lindex $IthIres 4]
			    set ::addsasfit(Nth,Qth)     $Q
			    set ::stepfit(chisq) $::GlobalAnalytPar(chisq)
			    if {$::stepfit(chisq) > $::stepfit(ochisq)} {
			       set ::stepfit(itst) 0
			    } elseif {[expr abs($::stepfit(chisq)- \
						$::stepfit(ochisq))]<0.1} {
			      incr ::stepfit(itst)
			    }
			    if {$::stepfit(oalambda) > $::GlobalAnalytPar(alambda) } {
			       set ::stepfit(itst) 0
			    }
			 } else {
			    set alambda 0
			    set ::GlobalAnalytPar(alambda) 0
			    if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI $res]  $::addsasfit(Nth,hide)\
					       ::stepfit \
				       ]
			    } else {
			       set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					       [list $Q $I $DI] $::addsasfit(Nth,hide)\
					       ::stepfit \
				       ]
			    }
			 }
			 set alambda $::GlobalAnalytPar(alambda)
			 puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
			 set tmp $::tmpGlobalAnalytPar(actual_SD)
			 cp_arr ::GlobalAnalytPar ::tmpGlobalAnalytPar
			 GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $tmp
			 cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
			 set_actualGlobalAP ::actualGlobalAnalytPar
			 set sub {}
			 foreach tmpQ $Q {
			     lappend sub no
			 }
			 foreach datset $::GlobalAnalytPar(dataset)  minus $::GlobalAnalytPar(substrSDFF) {
			     if {$minus} {
				 set sub [lreplace $sub [expr $datset-1] [expr $datset-1] $minus]
			     }
			 }
			 NewGlobalFitDataCmd ::addsasfit $sub plustheory
			 set ::GlobalAnalytPar(alambda) $alambda
			 set ::tmpGlobalAnalytPar(alambda) $alambda
		      } else { tk_messageBox -icon error \
					     -title ERROR -message "NO data loaded" 
		      }
		      RefreshAnalytParDataTab ::GlobalAnalytPar /Global
                      set ::fitparamguiupdate yes
		    }
			}
	}

	if {[winfo exists $w.adj.run]} {
		$w.adj.run configure -command {
			if {$::sasfit(busy)} {
				puts "SASfit is busy"
			} else {
			set ::fitparamguiupdate no
	                set ::SASfitprogressbar 0
	                set ::SASfitinterrupt 0
		      if { $::sasfit(update_menu_during_fit) } {
			  $::nomenu activate [expr $::actualGlobalAnalytPar(actual_SD)-1]
			  $::nomenu invoke [expr $::actualGlobalAnalytPar(actual_SD)-1]
		      }
		      cp_arr ::tmpGlobalAnalytPar ::GlobalAnalytPar

		      if {([string length $::addsasfit(lower,Q)] > 0) && \
			  ([string length $::addsasfit(upper,Q)] > 0) } {
			 set Q {}
			 set I {}
			 set DI {}
			 set res {}
			 foreach xx $::addsasfit(Nth,Q)   \
				 yy $::addsasfit(Nth,I)   \
				 ee $::addsasfit(Nth,DI)  \
				 rr $::addsasfit(Nth,res)    {

			    set Qx   {}
			    set Iy   {}
			    set DIe  {}
			    set resr {}
			    foreach x $xx y $yy e $ee r $rr {
			       if {($x>=$::addsasfit(lower,Q)) && ($x<=$::addsasfit(upper,Q))} {
				  lappend Qx   $x
				  lappend Iy   $y
				  lappend DIe  $e
				  lappend resr $r
			       }
			    }
			    lappend Q   $Qx
			    lappend I   $Iy
			    lappend DI  $DIe
			    lappend res $resr
			 }
		      } else  {
			 set Q   $::addsasfit(Nth,Q)
			 set I   $::addsasfit(Nth,I)
			 set DI  $::addsasfit(Nth,DI)
			 set res $::addsasfit(Nth,res)
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
							set_GlobalalambdaCmd name1 name2 op
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

		      set alambda $::GlobalAnalytPar(alambda)
		      if {$alambda == 0} { set alambda  -1 }
		      set ::tmpGlobalAnalytPar(alambda) $alambda
		      RefreshAnalytParDataTab ::GlobalAnalytPar /Global
		      set ::GlobalAnalytPar(alambda) $alambda
		      set ::tmpGlobalAnalytPar(alambda) $alambda
		      update
		      if { $::addsasfit(I_enable) } {
			  set ::stepfit(oalambda) ::GlobalAnalytPar(alambda)
			  if {[catch {
			     sasfit_timer_start "\nstart 0. iteration"
			     if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
				set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
						[list $Q $I $DI $res] $::addsasfit(Nth,hide) \
						::stepfit \
					]
			     } else {
				set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
						[list $Q $I $DI]  $::addsasfit(Nth,hide)\
						::stepfit \
					]
			     }
			     sasfit_timer_stop "0. iteration" "iteration finished" "\nalambda = $::GlobalAnalytPar(alambda),  chisq = $::stepfit(chisq)"
			 } msg] } {
			     set_GlobalalambdaCmd 1 1 1
			     bgerror $msg
			     set ::fitparamguiupdate yes
				 set ::sasfit(busy) false
			     return
			 }
			  set alambda $::GlobalAnalytPar(alambda)
			  puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
			  set tmp $::tmpGlobalAnalytPar(actual_SD)
			  if { $::sasfit(update_menu_during_fit) } {
			      cp_arr ::GlobalAnalytPar ::tmpGlobalAnalytPar
			      GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $tmp
			  }
			  set ::addsasfit(Nth,Ith)     [lindex $Ith 0] 
			  set ::addsasfit(Nth,sub,Ith) [lindex $Ith 1] 
			  set ::addsasfit(Nth,sub,Ih)  [lindex $Ith 2]
			  set ::addsasfit(Nth,DIh)     [lindex $Ith 3]
			  set ::addsasfit(Nth,sub,res) [lindex $Ith 4]
			  set ::addsasfit(Nth,Qth)     $Q
			  set sub {}
			  foreach tmpQ $Q {
			     lappend sub no
			  }
			  foreach datset $::GlobalAnalytPar(dataset)  minus $::GlobalAnalytPar(substrSDFF) {
			     if {$minus} {
				 set sub [lreplace $sub [expr $datset-1] [expr $datset-1] $minus]
			     }
			  }
			  if { $::sasfit(update_menu_during_fit) } {
			      NewGlobalFitDataCmd ::addsasfit $sub plustheory
			      RefreshGraph ::ResIQGraph
			  }
			  set ::GlobalAnalytPar(alambda) $alambda
			  set ::tmpGlobalAnalytPar(alambda) $alambda
			  update 
			  if { $continue_fit == "no" } { 
			     destroy $w
			  }
			  set ::stepfit(itst) 0
			  set ::stepfit(k)    1
			  set ::stepfit(ochisq) $::stepfit(chisq)

			  while {($::stepfit(itst) < 2) && ($continue_fit == "yes") && ($::addsasfit(maxIterations) >= $::stepfit(k))} {
			     set ::stepfit(ochisq) $::stepfit(chisq)
			     set ::stepfit(oalambda) $::GlobalAnalytPar(alambda)
			     if {[catch { 
				 sasfit_timer_start "\nstart $::stepfit(k). iteration"
				 if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
				     set Ith [sasfit_iqglobalfit ::GlobalAnalytPar \
					     [list $Q $I $DI $res] $::addsasfit(Nth,hide) \
					     ::stepfit \
					     ]
				 } else {
				     set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					     [list $Q $I $DI] $::addsasfit(Nth,hide) \
					     ::stepfit \
					     ]
				 }
				 sasfit_timer_stop "$::stepfit(k). iteration" "iteration finished" "\nalambda = $::GlobalAnalytPar(alambda),  chisq = $::stepfit(chisq)"
			     } msg] } {
				 set_GlobalalambdaCmd 1 1 1
				 bgerror $msg 
				 set ::fitparamguiupdate yes
				 set ::sasfit(busy) false
				 return 
			     }
			     set alambda $::GlobalAnalytPar(alambda)
			     incr ::stepfit(k)
			     set tmp $::tmpGlobalAnalytPar(actual_SD)
			     if { $::sasfit(update_menu_during_fit) } {
				 cp_arr ::GlobalAnalytPar ::tmpGlobalAnalytPar
				 GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $tmp
				 cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
				 set_actualGlobalAP ::tmpactualGlobalAnalytPar
			     }
			     set ::addsasfit(Nth,Ith)     [lindex $Ith 0] 
			     set ::addsasfit(Nth,sub,Ith) [lindex $Ith 1] 
			     set ::addsasfit(Nth,sub,Ih)  [lindex $Ith 2]
			     set ::addsasfit(Nth,DIh)     [lindex $Ith 3]
			     set ::addsasfit(Nth,sub,res) [lindex $Ith 4]
			     set ::addsasfit(Nth,Qth)     $Q
			     set sub {}
			     foreach tmpQ $Q {
				 lappend sub no
			     }
			     foreach datset $::GlobalAnalytPar(dataset)  minus $::GlobalAnalytPar(substrSDFF) {
				 if {$minus} {
				     set sub [lreplace $sub [expr $datset-1] [expr $datset-1] $minus]
				 }
			     }
			     if { $::sasfit(update_menu_during_fit) } {
				 NewGlobalFitDataCmd ::addsasfit $sub plustheory
				 RefreshGraph ::ResIQGraph
			     }
			     set ::GlobalAnalytPar(alambda) $alambda
			     if { $::sasfit(update_menu_during_fit) } {
				 $::nomenu activate [expr $::actualGlobalAnalytPar(actual_SD)-1]
				 $::nomenu invoke [expr $::actualGlobalAnalytPar(actual_SD)-1]
				 save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar
				 cp_arr ::tmpGlobalAnalytPar ::GlobalAnalytPar
			     }
			     set ::tmpGlobalAnalytPar(alambda) $alambda
			     update 
			     if {$::stepfit(chisq) > $::stepfit(ochisq)} {
				set ::stepfit(itst) 0
			     } elseif {[expr abs($::stepfit(chisq)- \
						 $::stepfit(ochisq))]<0.01} {
			       incr ::stepfit(itst)
			     }
			     if {$::stepfit(oalambda) > $::GlobalAnalytPar(alambda) } {
				set ::stepfit(itst) 0
			     }
			  }
			 if {($::stepfit(itst) >= 2) &&  ($continue_fit == "yes") } {
			    set alambda 0
			    set ::GlobalAnalytPar(alambda) 0
			    if {[catch {
				if {[string compare $::GlobalAnalytPar(resolution) yes] == 0} {
				    set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					    [list $Q $I $DI $res]  $::addsasfit(Nth,hide)\
					    ::stepfit \
					    ]
				} else {
				    set Ith [sasfit_iqglobalfit ::GlobalAnalytPar  \
					    [list $Q $I $DI]  $::addsasfit(Nth,hide)\
					    ::stepfit \
					    ]
				}
			    } msg] } {
				set_GlobalalambdaCmd 1 1 1
				bgerror $msg 
				set ::fitparamguiupdate yes
				set ::sasfit(busy) false
				return
			    }
			    set alambda $::GlobalAnalytPar(alambda)
			    puts "alambda = $alambda,  chisq = $::stepfit(chisq)"
			    set tmp $::tmpGlobalAnalytPar(actual_SD)
			    if { $::sasfit(update_menu_during_fit) } {
				cp_arr ::GlobalAnalytPar ::tmpGlobalAnalytPar
				GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $tmp
				cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
				set_actualGlobalAP ::tmpactualGlobalAnalytPar
			    }
			    set ::addsasfit(Nth,Ith)     [lindex $Ith 0] 
			    set ::addsasfit(Nth,sub,Ith) [lindex $Ith 1] 
			    set ::addsasfit(Nth,sub,Ih)  [lindex $Ith 2]
			    set ::addsasfit(Nth,DIh)     [lindex $Ith 3]
			    set ::addsasfit(Nth,sub,res) [lindex $Ith 4]
			    set ::addsasfit(Nth,Qth)     $Q
			    set sub {}
			    foreach tmpQ $Q {
				lappend sub no
			    }
			    foreach datset $::GlobalAnalytPar(dataset)  minus $::GlobalAnalytPar(substrSDFF) {
				if {$minus} {
				    set sub [lreplace $sub [expr $datset-1] [expr $datset-1] $minus]
				}
			    }
			    if { $::sasfit(update_menu_during_fit) } {
				NewGlobalFitDataCmd ::addsasfit $sub plustheory
				RefreshGraph ::ResIQGraph
			    }
			    set GloblAnalytPar(alambda) $alambda
			    set ::tmpGlobalAnalytPar(alambda) $alambda
			    update
			 }
			 cp_arr ::GlobalAnalytPar ::tmpGlobalAnalytPar
			 GlobalAPindex ::tmpGlobalAnalytPar ::actualGlobalAnalytPar $tmp
			 cp_arr ::actualGlobalAnalytPar ::tmpactualGlobalAnalytPar
			 #set_actualGlobalAP ::tmpactualGlobalAnalytPar
			 set_actualGlobalAP ::actualGlobalAnalytPar
		      } else { tk_messageBox -icon error \
					     -title ERROR -message "NO data loaded"
		      }
		      catch { destroy $w }
		      RefreshAnalytParDataTab ::GlobalAnalytPar /Global
		      .analytical.adj.calc invoke 
		      set ::fitparamguiupdate yes
		    }
			}
	}

	if {$simulate && [winfo exists $w.adj.calc]} {
		$w.adj.calc configure -command {
			if {$::sasfit(busy)} {
				puts "SASfit is busy"
			} else {
		      set ::fitparamguiupdate no
	      	      set ::SASfitprogressbar 0
		      set ::SASfitinterrupt 0
		      sasfit_timer_start "\nStart simulation"
		      set ::addsasfit(filename) unknown
		      set Q {}
		      set I {}
		      set DI {}
		      if {($::addsasfit(sim,Qmin) > 0) && (![string compare "log(x)" $::GlobalFitIQGraph(x,type)] || ![string compare "log10(x)" $::GlobalFitIQGraph(x,type)])} {
			 set logDQ [expr log10($::addsasfit(sim,Qmax))-log10($::addsasfit(sim,Qmin))]
			 set logDQ [expr $logDQ/$::addsasfit(sim,N)]
			 for {set k 0} {$k<$::addsasfit(sim,N)+1} {incr k} {
			     lappend Q [expr pow(10.0,log10($::addsasfit(sim,Qmin))+$k*$logDQ)]
			     lappend I 1.0
			     lappend DI 1.0
			 }
		      } else {
			 set DQ [expr 1.0*($::addsasfit(sim,Qmax))-($::addsasfit(sim,Qmin))]
			 set DQ [expr $DQ/$::addsasfit(sim,N)]
			 for {set k 0} {$k<$::addsasfit(sim,N)+1} {incr k} {
			     lappend Q [expr $::addsasfit(sim,Qmin)+$k*$DQ]
			     lappend I 1.0
			     lappend DI 1.0
			 }
		      }
		      set allQ {}
		      set allI {}
		      set allDI {}
		      set allhide {}
		      for {set i 1} {$i <= $::ask4dataset_var(N_datasets)} {incr i} {
			  lappend allQ $Q
			  lappend allI $I
			  lappend allDI $DI
			  lappend allhide no
		      }
		      save_GlobalAP ::tmpGlobalAnalytPar ::actualGlobalAnalytPar 
		      cp_arr ::tmpGlobalAnalytPar ::GlobalAnalytPar
			  if {[catch {
				set IthIres [sasfit_global_iq ::GlobalAnalytPar  \
							[list $allQ $allI $allDI] $allhide\
							]
			  } msg] } {
				bgerror $msg
				set ::sasfit(busy) false
				return
			  }
		      cp_arr ::addsasfit t_addsasfit
		      set ::addsasfit(Nth,Ith)     [lindex $IthIres 0] 
		      set ::addsasfit(Nth,sub,Ith) [lindex $IthIres 1] 
		      set ::addsasfit(Nth,sub,Ih)  [lindex $IthIres 2]
		      set ::addsasfit(Nth,DIh)     [lindex $IthIres 3]
		      set ::addsasfit(Nth,sub,res) [lindex $IthIres 4]
		      set ::addsasfit(Nth,Qth)     $allQ
		      set ::addsasfit(Nth,n)       $::ask4dataset_var(N_datasets)
		      set ::addsasfit(Nth,hide)    $allhide
		      foreach tmpQ $allQ {
			  lappend sub no
		      }
		      NewGlobalFitDataCmd ::addsasfit $sub simulate
		      RefreshAnalytParDataTab ::GlobalAnalytPar /Global
		      cp_arr t_addsasfit ::addsasfit
		      sasfit_timer_stop "Simulation" "finished" ""
		      set ::fitparamguiupdate yes
		    }
			}
	}
	 
	bind $wsd.wrl.whichsd <Enter> \
	     {[quick_message_window] configure -text "[get_helpline SD 0 ::actualGlobalAnalytPar]"}
	bind $wff.wrl.whichff <Enter> \
	     {[quick_message_window] configure -text "[get_helpline FF 0 ::actualGlobalAnalytPar]"}
	bind $wsq.wrl.whichsq <Enter> \
	     {[quick_message_window] configure -text "[get_helpline SQ 0 ::actualGlobalAnalytPar]"}
	analytical_scroll_binds [winfo toplevel $wsd]

	$::nomenu activate 0
	$::nomenu invoke 0
	$wn raise 1
	$wn compute_size

	if {[string compare $::addsasfit(simorfit) fit] == 0} {
	    set ::actualGlobalAnalytPar(datalabel) [lindex $::addsasfit(Nth,filelabel) [expr $::actualGlobalAnalytPar(dataset)-1]]
	} else {
	    set ::actualGlobalAnalytPar(datalabel) "$::actualGlobalAnalytPar(datalabel,$::actualGlobalAnalytPar(dataset))"
	}

	if {[llength $::GlobalAnalytPar(FF,param_count)] != $::GlobalAnalytPar(max_SD)
	} {
		cycleThroughContributionsAndApply ::GlobalAnalytPar .analytical
	}
}

