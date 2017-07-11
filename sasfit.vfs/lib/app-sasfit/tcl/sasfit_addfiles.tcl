# sasfit.vfs/lib/app-sasfit/tcl/sasfit_addfiles.tcl
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

#------------------------------------------------------------------------------
#         Plot New Fit Data together with residuum and size distribution
proc NewFitDataCmd {l_Q l_IthIres sub l_NR args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# args eventually set to sim
#---------

	upvar $l_IthIres IthIres
	upvar $l_NR NR
	upvar $l_Q Q
	global SDGraph ResIQGraph IQGraph 
	global sasfit

	# calculates an alternative fit-quality indicator (across datasets)
	proc updateChisqr { Q I } {
		if {![llength $Q] || ![llength $I]} {
			return
		}
		set exp_q  $::sasfit(Q)
		set exp_i  $::sasfit(I)
		set exp_di $::sasfit(DI)
		set exp_res $::sasfit(res)
		# take the selected q-range into account 
		if {$::sasfit(lower,Q) > 0 && 
		    $::sasfit(upper,Q) > $::sasfit(lower,Q)
		} {
			set exp_q   {}
			set exp_i   {}
			set exp_di  {}
			set exp_res {}
			foreach x $::sasfit(Q) y $::sasfit(I) e $::sasfit(DI) r $::sasfit(res) {
				if { $x >= $::sasfit(lower,Q) &&
				     $x <= $::sasfit(upper,Q) 
				} {
					lappend exp_q $x
					lappend exp_i $y
					lappend exp_di $e
					lappend exp_res $r
				}
			}
		}
		set val 0.0
		set valerr 0.0
		# calculate sum of intensities
		set sum_id 0.0
		foreach id $exp_i { set sum_id [expr $sum_id + abs($id)] }
		# c = calculated, d = experimental data
		foreach qc $Q qd $exp_q ic $I id $exp_i did $exp_di {
			if {$qc != $qd} {
				error "updateChisqr: Q sample points differ !"
			}
			# set val [expr $val + (abs($ic - $id) / abs($id)) ]
			set val [expr $val + abs($ic - $id) ]
			set divisor [expr abs($id) - abs($did)]
			if {$divisor <= 0.0} {
				set valerr inf
			} elseif {![string equal $valerr "inf"]} {
				set valerr [expr $valerr + (abs($ic - $id) / $divisor) ]
			}
		}
#		puts stderr "updateChisqr: val: '$val', valerr: '$valerr'"
		# set ::AnalytPar(chisqr_rel) [expr $val / [llength $Q]]
		# sum(|F_exp - F_calc|)/sum(|F_exp|)
		catch {set ::AnalytPar(chisqr_rel) [expr 100.0 * $val / $sum_id]}
		if {![string equal $valerr "inf"]} {
			set ::AnalytPar(chisqr_relerr) [expr 100.0 * $valerr / [llength $Q]]
		} else {
			set ::AnalytPar(chisqr_relerr) $valerr
		}
	}

	set Ih   [lindex $IthIres 1]
	set Ith  [lindex $IthIres 2]
	set Ires [lindex $IthIres 3]
	set DI   [lindex $IthIres 4]

	if {[string equal $::sasfit(simorfit) "fit"]} {
#		updateChisqr $Q $Ith
	}

	clearGraph_el IQGraph

	set indx1 -1

	# draw and configure the calculated graph
	Put_Graph_el IQGraph $Q $Ith 
	incr indx1
	if {[llength $args] == 0} {
		set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
							0 0 \
							[file tail [lindex $sasfit(filename) 0]]]
	}
	set IQGraph(e,symbol)     [lreplace $IQGraph(e,symbol) $indx1 $indx1 none]
	set IQGraph(e,linehide)   [lreplace $IQGraph(e,linehide) $indx1 $indx1 1]
	set IQGraph(e,dashcolor)  [lreplace $IQGraph(e,dashcolor) $indx1 $indx1 red]
	set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
				       $indx1 $indx1 Fit]

	# draw the data (for fit mode)
	if {[llength $args] == 0} {
		if {$sub} {
			Put_Graph_el IQGraph $Q $Ih $DI $Ires
			incr indx1
			lset IQGraph(e,fill) $indx1 {}; # show empty symbols
		}
		Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $sasfit(DI) $sasfit(res)
		incr indx1
	}
	
	# update the graph
	RefreshGraph IQGraph
	clearGraph_el ResIQGraph
	Put_Graph_el ResIQGraph $Q $Ires
	set ResIQGraph(e,linehide) [lreplace $ResIQGraph(e,linewidth)\
				       0 0 0]
	set ResIQGraph(l,legendtext) \
				[lreplace ResIQGraph(l,legendtext) 0 0 \
				[file tail [lindex $sasfit(filename) 0]]]
	set ResIQGraph(l,hide) yes
	RefreshGraph ResIQGraph
	clearGraph_el SDGraph
	if {[llength $NR] != 0} {
		for {set i 0} {$i<=[expr [llength $NR]-1]} {incr i} {
#			puts "plotting $i-th size distribution ([llength \
#                                            [lindex [lindex $NR $i] 0]] points)"
			Put_Graph_el SDGraph [lindex [lindex $NR $i] 0] \
						[lindex [lindex $NR $i] 1]
			set SDGraph(x,logscale) 0
			set SDGraph(x,title) R
			set SDGraph(y,logscale) 0
			set SDGraph(y,title) N(R)
			set SDGraph(e,symbol)   [lreplace $SDGraph(e,symbol)   \
				[expr $i] [expr $i] none]
			set SDGraph(e,linehide)   [lreplace $SDGraph(e,linehide)   \
				[expr $i] [expr $i] 1]
		}
		RefreshGraph SDGraph
	}
}


#------------------------------------------------------------------------------
#                            Plot New Global Fit Data
proc NewGlobalFitDataCmd {sasfitarray args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $sasfitarray ssasfit
global SDGraph GlobalFitIQGraph ResIQGraph
global addsasfit


   clearGraph_el SDGraph
   clearGraph_el ResIQGraph
   clearGraph_el GlobalFitIQGraph

   set GlobalFitIQGraph(e,symbol)       {}
   set GlobalFitIQGraph(e,fill)         {}
   set GlobalFitIQGraph(e,outline)      {}
   set GlobalFitIQGraph(l,hide) no

   set simorfit rawdata
   set sub {}
   foreach tmpQ $ssasfit(Nth,Q) {
       lappend sub no
   }
   if {[llength $args] == 1} {
       set simorfit [lindex $args 0]
   } elseif {[llength $args] == 2} {
       set sub [lindex $args 0]
       set simorfit [lindex $args 1]
   } else {
       set simorfit rawdata
   }

   set color_i 0
   set symbol_i 0
   set count_i 0
   set count_j 0
   set count_k 0

   for {set i 0} {$i < $ssasfit(Nth,n)} {incr i} {
      if {$color_i >= [llength $GlobalFitIQGraph(colorselection)]} {
          set color_i 0
          incr symbol_i
          if {$symbol_i >= [llength $GlobalFitIQGraph(symbolselection)]} {set symbol_i 0}
      }
      set  color_n  [lindex $GlobalFitIQGraph(colorselection) $color_i]
      incr color_i
      if {[string compare [lindex $ssasfit(Nth,hide) $i] no] == 0} {	 
	  set symbol_n [lindex $GlobalFitIQGraph(symbolselection) $symbol_i] 
	  if {[string compare $simorfit plustheory] == 0} {
	     Put_Graph_el GlobalFitIQGraph \
		     [lindex $ssasfit(Nth,Qth)     $count_i] \
		     [lindex $ssasfit(Nth,sub,Ith) $count_i]  
	     set GlobalFitIQGraph(e,symbol)     [lreplace $GlobalFitIQGraph(e,symbol)     $count_k $count_k  none     ]
	     set GlobalFitIQGraph(e,linehide)   [lreplace $GlobalFitIQGraph(e,linehide)   $count_k $count_k  1        ]
	     set GlobalFitIQGraph(e,dashcolor)  [lreplace $GlobalFitIQGraph(e,dashcolor)  $count_k $count_k  $color_n ] 
	     set GlobalFitIQGraph(e,outline)    [lreplace $GlobalFitIQGraph(e,outline)    $count_k $count_k  $color_n ] 
	     set GlobalFitIQGraph(l,legendtext) [lreplace $GlobalFitIQGraph(l,legendtext) $count_k $count_k  \
		     "Fit: [lindex $ssasfit(Nth,filelabel) $count_i]"]
	     Put_Graph_el ResIQGraph \
		     [lindex $ssasfit(Nth,Qth)     $count_i] \
		     [lindex $ssasfit(Nth,sub,res) $count_i] 
	     set ResIQGraph(e,symbol)     [lreplace $ResIQGraph(e,symbol)     $count_j $count_j  $symbol_n ]
	     set ResIQGraph(e,fill)       [lreplace $ResIQGraph(e,fill)       $count_j $count_j  $color_n  ]
	     set ResIQGraph(e,outline)    [lreplace $ResIQGraph(e,outline)    $count_j $count_j  $color_n  ] 
	     set ResIQGraph(l,legendtext) [lreplace $ResIQGraph(l,legendtext) $count_j $count_j  \
		     "[lindex $ssasfit(Nth,filelabel) $count_i]"]
	     incr count_k
	  }
	  if {[lindex $sub $i] && ([string compare $simorfit simulate]!= 0)} {
               Put_Graph_el GlobalFitIQGraph \
                       [lindex $ssasfit(Nth,Qth)     $count_j] \
		       [lindex $ssasfit(Nth,sub,Ih)  $count_j] \
		       [lindex $ssasfit(Nth,DIh) $count_j]
	       incr ssasfit(Nth,n,nonhidden)
	       set symbol_n [lindex $GlobalFitIQGraph(symbolselection) $symbol_i] 
               set GlobalFitIQGraph(e,symbol)     [lreplace $GlobalFitIQGraph(e,symbol)     $count_k $count_k  $symbol_n]
	       set GlobalFitIQGraph(e,fill)       [lreplace $GlobalFitIQGraph(e,fill)       $count_k $count_k  {}       ]
	       set GlobalFitIQGraph(e,dashcolor)  [lreplace $GlobalFitIQGraph(e,dashcolor)  $count_k $count_k  $color_n ] 
	       set GlobalFitIQGraph(e,outline)    [lreplace $GlobalFitIQGraph(e,outline)    $count_k $count_k  $color_n ] 
               set GlobalFitIQGraph(l,legendtext) [lreplace $GlobalFitIQGraph(l,legendtext) $count_k $count_k  \
		       "Subtr.: [lindex $ssasfit(Nth,filelabel)   $count_i]" ]
	       incr count_k
	  }
	  if {[string compare $simorfit simulate] == 0} {
	     global ask4dataset_var
	     Put_Graph_el GlobalFitIQGraph [lindex $ssasfit(Nth,Qth) $count_i] \
		     [lindex $ssasfit(Nth,sub,Ith) $count_i] 
	     set symbol_n [lindex $GlobalFitIQGraph(symbolselection) $symbol_i] 
	     set GlobalFitIQGraph(e,symbol)     [lreplace $GlobalFitIQGraph(e,symbol)     $count_k $count_k  none     ]
	     set GlobalFitIQGraph(e,linehide)   [lreplace $GlobalFitIQGraph(e,linehide)   $count_k $count_k  1        ]
	     set GlobalFitIQGraph(e,dashcolor)  [lreplace $GlobalFitIQGraph(e,dashcolor)  $count_k $count_k  $color_n ] 
	     set GlobalFitIQGraph(l,legendtext) [lreplace $GlobalFitIQGraph(l,legendtext) $count_k $count_k  \
		     "Sim.: $::actualGlobalAnalytPar(datalabel,[expr $count_k+1])" ]
	     incr count_k
	  }
	  if {[string compare $simorfit simulate] != 0} {
	      Put_Graph_el GlobalFitIQGraph \
		      [lindex $ssasfit(Nth,Q)  $count_i] \
		      [lindex $ssasfit(Nth,I)  $count_i] \
		      [lindex $ssasfit(Nth,DI) $count_i]
	      incr ssasfit(Nth,n,nonhidden)
	      set GlobalFitIQGraph(e,symbol)     [lreplace $GlobalFitIQGraph(e,symbol)     $count_k $count_k  $symbol_n]
	      set GlobalFitIQGraph(e,fill)       [lreplace $GlobalFitIQGraph(e,fill)       $count_k $count_k  $color_n ]
	      set GlobalFitIQGraph(e,outline)    [lreplace $GlobalFitIQGraph(e,outline)    $count_k $count_k  $color_n ] 
	      set GlobalFitIQGraph(l,legendtext) [lreplace $GlobalFitIQGraph(l,legendtext) $count_k $count_k  \
		      [lindex $ssasfit(Nth,filelabel) $count_i]]
	      incr count_k
	  }
	  incr count_j
     }
     incr count_i
     set GlobalFitIQGraph(title) $ssasfit(filename)
   }
   RefreshGraph GlobalFitIQGraph
   RefreshGraph SDGraph
   set ResIQGraph(l,hide) no
   RefreshGraph ResIQGraph
}

#------------------------------------------------------------------------------
#                     menu for reading data files
#
proc SelectNewDataFileCmd {args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global addsasfit tmpsasfit
if {[winfo exists .openfile]} {destroy .openfile}
set tmpsasfit(SelectNewDataFileCmd_args) $args
toplevel .openfile
wm geometry .openfile
wm title .openfile "New File"
raise .openfile
focus .openfile
grab  .openfile

set datatype $tmpsasfit(actualdatatype)
frame .openfile.layout1
frame .openfile.layout2
frame .openfile.layout3

set format [tk_optionMenu .openfile.layout1.format tmpsasfit(actualdatatype) \
                           Ascii BerSANS SESANS ALV5000]
.openfile.layout1.format configure -highlightthickness 0
label .openfile.layout1.label -text "File Format:" -highlightthickness 0
pack .openfile.layout1.label .openfile.layout1.format -side left -fill x

label .openfile.layout2.label -text "  File Name:"  -highlightthickness 0
entry .openfile.layout2.entry -width 40 -relief sunken -highlightthickness 0 \
                              -textvariable tmpsasfit(filename)
button .openfile.layout2.browse -text "Browse..." -highlightthickness 0 \
       -command {BrowseCmd .openfile tmpsasfit(filename) \
                           tmpsasfit(actualdatatype) ReadData }
pack .openfile.layout2.label .openfile.layout2.entry \
     .openfile.layout2.browse  -side left -padx 2

button .openfile.layout3.read \
       -text OK -command {
	global tmpsasfit
	set Nth [lindex $tmpsasfit(SelectNewDataFileCmd_args) 0]
	if {$Nth < 1} {set Nth 1}
	set_Nth_addsasfit tmpsasfit $Nth
	set tmpsasfit(error) no                            
	catch {destroy .openfile}
	}\
       -highlightthickness 0
button .openfile.layout3.option -text "Options..." -command ReadOptionsCmd \
       -highlightthickness 0
button .openfile.layout3.dismiss -text Cancel \
       -highlightthickness 0 -command {
                                        if {[winfo exist .asciioptions]} {
                                           destroy .asciioptions
                                        }
                                        set tmpsasfit(error) yes
					set tmpsasfit(filename) ""
                                        destroy .openfile
                                      }
pack .openfile.layout3.read .openfile.layout3.option  \
     .openfile.layout3.dismiss \
     -side left -padx 6 -fill x -expand yes 

pack .openfile.layout1 .openfile.layout2 .openfile.layout3 \
     -fill x -pady 2 -pady 6
}

#------------------------------------------------------------------------------
#                     menu for reading data files
#
proc SelectNewDataFromClipboardCmd {average args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global addsasfit tmpsasfit
catch {destroy .openfile}
catch {destroy .addfile}
set tmpsasfit(SelectNewDataFileCmd_args) $args

if {[catch {set  ReadSuccess [ReadClipboardCmd tmpsasfit $average addfiles]}]} {
   tk_messageBox -icon error \
                 -message "no valid ASCII Data in Clipboard"
} else {
    if {[string compare $ReadSuccess no] == 0} {
	if {[llength $tmpsasfit(SelectNewDataFileCmd_args)] == 0} {
	    set Nth 1
	} else {
	    set Nth [lindex $tmpsasfit(SelectNewDataFileCmd_args) 0]
	}
	set tmpsasfit(file,name)  {}
	lappend tmpsasfit(file,name) "data from clipboard"
	set tmpsasfit(filename) "data from clipboard"
	set_Nth_addsasfit tmpsasfit $Nth 
	set tmpsasfit(error) no                            
	catch {destroy .openfile}
    } else {
	tk_messageBox -icon error \
		-message \
		"no valid ASCII Data in Clipboard"
    } 
}
}

proc PasteClipboardData2GlobalIQGraph {average} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global addsasfit tmpsasfit ASCIIData
catch {destroy .addfile}
cp_arr addsasfit tmpsasfit
#set tmpsasfit(file,name) "data from clipboard"
if {$tmpsasfit(file,n) < 1} {
#    set tmpsasfit(Nth,n) 0
    SelectNewDataFromClipboardCmd $average 1
} else {
    SelectNewDataFromClipboardCmd $average [expr $tmpsasfit(Nth,n)+1]
}
set GlobalFitIQGraph(x,title) "Q / $ASCIIData(unit)\^-1"
# tmpsasfit
NewGlobalFitDataCmd tmpsasfit
cp_arr tmpsasfit addsasfit
}

#------------------------------------------------------------------------------
#                     help routine for reading Nth data set
#
proc set_Nth_addsasfit {globalsasfit Nth} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $globalsasfit tmpsasfit
global resolution
set tmpsasfit(file,Q)   {}
set tmpsasfit(file,I)   {}
set tmpsasfit(file,DI)  {}
set tmpsasfit(file,res) {}
set tmpsasfit(file,res,file) {}
set tmpsasfit(file,res,calc) {}
set tmpsasfit(file,widcnt) 0
set tmpsasfit(file,n)       0
set tmpsasfit(filelabel)    [file tail [lindex $tmpsasfit(filename) 0]]
set tmpsasfit(file,name)  {}
set tmpsasfit(file,widcnt)     0
set tmpsasfit(file,divisor)   {1}
set tmpsasfit(file,firstskip) {0}
set tmpsasfit(file,lastskip)  {0}
set tmpsasfit(file,hide)      {no}
dr_default_op set tmpsasfit "file,"
set tmpsasfit(file,r1)        {}
set tmpsasfit(file,r2)        {}
set tmpsasfit(file,l1)        {}
set tmpsasfit(file,l2)        {}
set tmpsasfit(file,lambda)    {}
set tmpsasfit(file,Dlambda)   {}
set tmpsasfit(file,d)         {}
set tmpsasfit(file,Dd)        {}
set tmpsasfit(file,widname)   {{.addfile.lay2 0}}
set tmpsasfit(file,widname)   {}
replace_or_append_Nth_globalfit_data tmpsasfit $Nth
}


#------------------------------------------------------------------------------
#                     setting Nth entry in globalsasfit
#
proc get_Nth_globalfit_data {globalsasfit Nth} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $globalsasfit addsasfit
if {$addsasfit(Nth,n) < $Nth} {
   error "proc get_Nth_globalfit_data: addsasfit(Nth,n) < Nth"
}

set addsasfit(Nth,actual) $Nth
sasfit_arr_op set addsasfit addsasfit "file," "Nth,file," [expr $Nth-1] \
	{n name divisor firstskip lastskip hide widname widcnt \
	Q I DI res "res,file" "res,calc" r1 r2 lambda Dlambda l1 l2 Dd d \
	dr_by_count dr_percent dr_loglogdist dr_mindist}
sasfit_arr_op set addsasfit addsasfit "" "Nth," [expr $Nth-1] \
	{filelabel hide Q I DI res "res,file" "res,calc"}
}


#------------------------------------------------------------------------------
#                     help routine for reading Nth data set
#
#proc save_actual_globalfit_data {globalsasfit} {
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#	upvar $globalsasfit tmpsasfit
#	global fn hide fskip lskip divisor widname 
#
#	set tmpsasfit(file,hide)      {}
#	set tmpsasfit(file,firstskip) {}
#	set tmpsasfit(file,lastskip)  {}
#	set tmpsasfit(file,divisor)   {}
#	set tmpsasfit(file,name)      {}
#
#	set n_no_hide 0
#	for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
#	   set tmpwidname [lindex $tmpsasfit(file,widname) $i]
#	   set j [lindex $tmpwidname 1]
#	   lappend tmpsasfit(file,hide)      $hide($j)
#	   if {[string compare $hide($j) no] == 0} {
#	      incr n_no_hide
#	   }
#	   lappend tmpsasfit(file,firstskip) $fskip($j)
#	   lappend tmpsasfit(file,lastskip)  $lskip($j)
#	   lappend tmpsasfit(file,divisor)   $divisor($j)
#	   lappend tmpsasfit(file,name)      $fn($j)
#	}
#	replace_or_append_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)
#	if {$n_no_hide > 0} {
#	   FileMergeCmd tmpsasfit
#	} else {
#	   blt::beep
#	   error "at least one file should not be hidden"
#	}
#	replace_or_append_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)
#	if {$n_no_hide > 0} {
#	   FileMergeCmd tmpsasfit
#	} else {
#	   blt::beep
#	   error "at least one file should not be hidden"
#	}
#}


#------------------------------------------------------------------------------
#                     help routine for reading Nth data set
#
proc replace_or_append_Nth_globalfit_data {globalsasfit Nth} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $globalsasfit addsasfit
	if {[expr $addsasfit(Nth,n)+1] < $Nth} {
	   error "proc replace_or_append_Nth_globalfit_data: globalsasfit(Nth,n)+1 < Nth"
	}
	set addsasfit(Nth,actual) $Nth
	if {$addsasfit(Nth,n) < $Nth} {
	   incr addsasfit(Nth,n)
	   sasfit_arr_op lappend addsasfit addsasfit "Nth,file," "file," -1 \
	      {n name divisor firstskip lastskip hide widname widcnt Q I DI res \
	      "res,file" "res,calc" r1 r2 lambda Dlambda l1 l2 Dd d \
	      dr_by_count dr_percent dr_loglogdist dr_mindist}
	   sasfit_arr_op lappend addsasfit addsasfit "Nth," "" -1 \
	      {filelabel hide Q I DI res "res,file" "res,calc"}
	} else {
	   sasfit_arr_op lset addsasfit addsasfit "Nth,file," "file," [expr $Nth-1] \
	      {n name divisor firstskip lastskip hide widname widcnt Q I DI res "res,file" "res,calc" \
	      r1 r2 lambda Dlambda l1 l2 Dd d \
	      dr_by_count dr_percent dr_loglogdist dr_mindist}
	   sasfit_arr_op lset addsasfit addsasfit "Nth," "" [expr $Nth-1] \
	      {filelabel hide Q I DI res "res,file" "res,calc"}
	}
}



#------------------------------------------------------------------------------
#                     help routine for removing Nth data set
#
proc remove_Nth_globalfit_data {globalsasfit Nth} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $globalsasfit addsasfit
	if {([expr $addsasfit(Nth,n)+1] < $Nth) ||($Nth < 1) } {
		error "proc remove_Nth_globalfit_data: globalsasfit(Nth,n)+1 < Nth || Nth<1"
	}
	set addsasfit(Nth,actual) $Nth
	sasfit_arr_op delete addsasfit addsasfit "Nth,file," "Nth,file," [expr $Nth-1] \
		{n name divisor firstskip lastskip hide widname widcnt Q I DI res \
		"res,calc" "res,file" r1 r2 lambda Dlambda l1 l2 Dd d \
		dr_by_count dr_percent dr_loglogdist dr_mindist}
	sasfit_arr_op delete addsasfit addsasfit "Nth," "Nth," [expr $Nth-1] \
		{filelabel hide Q I DI res "res,file" "res,calc"}

	if {$Nth == $addsasfit(Nth,n)} {
		incr addsasfit(Nth,actual) -1
	} else {
		set addsasfit(Nth,actual) $Nth
	}
	incr addsasfit(Nth,n) -1
}

#------------------------------------------------------------------------------
#                     subroutine for Add data files
#
proc AddCmd {args} {
#^^^^^^^^^^^^^^^^^^^
global sasfit tmpsasfit AnalytPar actualAnalytPar tmpAnalytPar
global fn hide fskip lskip divisor widname r1 r2 l1 l2 lambda Dlambda d Dd
global addsasfit
global nomenu_add
cp_arr addsasfit tmpsasfit
if {$tmpsasfit(file,n) < 1 || $::show_new_file_dialog} {
   SelectNewDataFileCmd 1
   tkwait window .openfile
   if {![string length [string trim $tmpsasfit(filename)]]} {return};# 'cancel' pressed
   # clear structures
   clear_sasfit_file_config addsasfit "file,"
   set ::show_new_file_dialog 0
}
if {[winfo exists .addfile]} {destroy .addfile}

toplevel .addfile
wm geometry .addfile
wm title .addfile "Merge Files"
raise .addfile
focus .addfile
#grab  .addfile

set datatype $tmpsasfit(actualdatatype)
frame .addfile.lay0 -relief ridge -borderwidth 2
frame .addfile.lay1 -relief ridge -borderwidth 2
frame .addfile.lay2 -relief ridge -borderwidth 2
pack .addfile.lay0 .addfile.lay1 .addfile.lay2 -fill x -expand yes

frame .addfile.lay0.layout1
frame .addfile.lay0.layout2
set w .addfile.lay0
pack $w.layout1 $w.layout2 -pady 1m -fill x -expand yes

label $w.layout1.label -text "data set:" \
      -width 9 -highlightthickness 0
set nomenu_add [tk_optionMenu $w.layout1.no tmpsasfit(Nth,actual_optionMenu) 1]
$nomenu_add entryconfigure 0 -command {
           global addsasfit tmpsasfit
           merge_cmd_apply tmpsasfit 1
           set tmpsasfit(Nth,actual) $tmpsasfit(Nth,actual_optionMenu)
           foreach twidname $tmpsasfit(file,widname) {
              set w2 .addfile.lay2
              set widcnt [lindex $twidname 1]
              destroyMergeFile $widcnt
           }
           get_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)
           for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
              MergeFileCmd tmpsasfit $i
           }
        }
for {set i 2} {$i <= $tmpsasfit(Nth,n)} {incr i} {
   $nomenu_add add radiobutton \
      -variable tmpsasfit(Nth,actual_optionMenu) \
      -label $i
   $nomenu_add entryconfigure [expr $i-1] -command {
           global addsasfit tmpsasfit
           merge_cmd_apply tmpsasfit 1
           set tmpsasfit(Nth,actual) $tmpsasfit(Nth,actual_optionMenu)
           foreach twidname $tmpsasfit(file,widname) {
              set w2 .addfile.lay2
              set widcnt [lindex $twidname 1]
              destroyMergeFile $widcnt
           }
           get_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)           
           for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
              MergeFileCmd tmpsasfit $i
           }
       }
}

setTooltip $w.layout1.no "display files of\ni-th data set"

$w.layout1.no configure -highlightthickness 0
pack $w.layout1.label $w.layout1.no -side left -padx 1m -pady 1m
Button $w.layout1.next -text Next -highlightthickness 0 \
       -padx 1m -pady 1m \
       -repeatinterval 100 \
       -repeatdelay  2000 \
       -command {
           global addsasfit tmpsasfit nomenu_add
           merge_cmd_apply tmpsasfit 1
           if {$tmpsasfit(Nth,actual_optionMenu)>=$tmpsasfit(Nth,n)} {
              $nomenu_add activate 0
              $nomenu_add invoke 0
           } else {
              $nomenu_add activate [expr $tmpsasfit(Nth,actual_optionMenu)]
              $nomenu_add invoke [expr $tmpsasfit(Nth,actual_optionMenu)]
	   }
       }
setTooltip $w.layout1.next "display next data set"
Button $w.layout1.previous -text Previous -highlightthickness 0 \
       -padx 1m -pady 1m \
       -repeatinterval 100 \
       -repeatdelay  2000 \
       -command {
           global addsasfit tmpsasfit nomenu_add
           merge_cmd_apply tmpsasfit 1
           if {$tmpsasfit(Nth,actual_optionMenu)<=1} {
              $nomenu_add activate last
              $nomenu_add invoke last
           } else {
              $nomenu_add activate [expr $tmpsasfit(Nth,actual_optionMenu)-2]
              $nomenu_add invoke [expr $tmpsasfit(Nth,actual_optionMenu)-2]
	   }
       }
setTooltip $w.layout1.previous "display the next data set"
Button $w.layout1.add -text Add -highlightthickness 0 \
       -padx 1m -pady 1m \
       -repeatinterval 100 \
       -repeatdelay  2000 \
       -command { 
           global addsasfit tmpsasfit
           global nomenu_add
           merge_cmd_apply tmpsasfit 1
           foreach twidname $tmpsasfit(file,widname) {
              set w2 .addfile.lay2
              set widcnt [lindex $twidname 1]
              destroyMergeFile $widcnt
           }
           SelectNewDataFileCmd [expr $tmpsasfit(Nth,n)+1]
           tkwait window .openfile
           if {$tmpsasfit(error)} {
              blt::beep
           } else {
		if {[llength $tmpsasfit(filename)] > 0} {
			.addfile.lay1.layout1.read invoke
		} else { return }
              $nomenu_add add radiobutton \
                 -variable  tmpsasfit(Nth,actual_optionMenu) \
                 -label $tmpsasfit(Nth,n)
              $nomenu_add activate [expr $tmpsasfit(Nth,n)-1]
              $nomenu_add entryconfigure active -command {
                      global addsasfit tmpsasfit
                      merge_cmd_apply tmpsasfit 1
                      set tmpsasfit(Nth,actual) $tmpsasfit(Nth,actual_optionMenu)
                      foreach twidname $tmpsasfit(file,widname) {
                         set w2 .addfile.lay2
                         set widcnt [lindex $twidname 1]
                         destroyMergeFile $widcnt
                      }
                      get_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)
                      for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
                         MergeFileCmd tmpsasfit $i
                      }
              }
              $nomenu_add invoke active
	  }
       }
setTooltip $w.layout1.add "add another data set"
Button $w.layout1.delete -text Remove -highlightthickness 0 \
       -padx 1m -pady 1m \
       -repeatinterval 100 \
       -repeatdelay  2000 \
       -command { 
           global addsasfit tmpsasfit
           global nomenu_add
           if {$tmpsasfit(Nth,n) > 1} {
              set max_Nth $tmpsasfit(Nth,n)
              foreach twidname $tmpsasfit(file,widname) {
                 set w2 .addfile.lay2
                 set widcnt [lindex $twidname 1]
                 destroyMergeFile $widcnt
              }
              remove_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual) 

              if {$max_Nth == $tmpsasfit(Nth,actual)} {
                 incr tmpsasfit(Nth,actual) -1
                 $nomenu_add delete $tmpsasfit(Nth,actual) 
              } else {
		 for {set i $max_Nth} \
                     {$i > $tmpsasfit(Nth,actual)} \
	             {incr i -1 } {
                        $nomenu_add delete [expr $i - 1]
	         }
                 for {set i  $tmpsasfit(Nth,actual)} \
                    {$i < [expr $max_Nth - 1]} {incr i} {
                       $nomenu_add entryconfigure $i \
                          -label [expr $i + 1] \
                          -variable tmpsasfit(Nth,actual_optionMenu) \
                          -command {
                                   global addsasfit tmpsasfit
                                   merge_cmd_apply tmpsasfit 1
                                   set tmpsasfit(Nth,actual) $tmpsasfit(Nth,actual_optionMenu)
                                   foreach twidname $tmpsasfit(file,widname) {
                                      set w2 .addfile.lay2
                                      set widcnt [lindex $twidname 1]
                                      destroyMergeFile $widcnt
                                   }
                                   get_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)           
                                   for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
                                      MergeFileCmd tmpsasfit $i
                                  }
			      }
		 }
	       }
               get_Nth_globalfit_data tmpsasfit $tmpsasfit(Nth,actual)
               $nomenu_add activate [expr $tmpsasfit(Nth,actual) - 1]
               $nomenu_add invoke [expr $tmpsasfit(Nth,actual) - 1]
	} else { bell }
	}
setTooltip $w.layout1.delete "remove actual displayed data set"
eval checkbutton $w.layout1.hide "-offvalue no -onvalue yes \
     -variable tmpsasfit(hide) $::radio_check_button_prop \
     -text \"hide data set\" \
     -command {
              } -highlightthickness 0"
pack $w.layout1.previous $w.layout1.next $w.layout1.add $w.layout1.delete $w.layout1.hide \
     -side left -fill x -expand yes -padx 1m -pady 1m


label $w.layout2.legendtext  -text "data label:" \
      -width 11 -highlightthickness 0 

entry $w.layout2.entry -width 30 -relief sunken -highlightthickness 0 \
                              -textvariable tmpsasfit(filelabel)
pack  $w.layout2.legendtext $w.layout2.entry \
     -side left -padx 1m -pady 1m


frame .addfile.lay1.layout1
frame .addfile.lay1.layout2
set w .addfile.lay1


frame .addfile.layout3

set format [tk_optionMenu $w.layout1.format tmpsasfit(actualdatatype) \
                           Ascii BerSANS SESANS ALV5000]
$w.layout1.format configure -highlightthickness 0
label $w.layout1.label -text "File Format:" \
      -width 12 -highlightthickness 0
button $w.layout1.option -text "Options..." -command ReadOptionsCmd \
       -highlightthickness 0 -pady 1m
button $w.layout1.read -text "Read file" \
       -command {
				global tmpsasfit
				set tmpfnlist $tmpsasfit(filename)
				set errornessFiles {}
				foreach fin $tmpfnlist {
					set tmpsasfit(filename) $fin
					set tmpval [ReadFileCmd tmpsasfit /norefreshdata]
					if {[string equal "$tmpval" "no"]} {
						lappend tmpsasfit(file,Q)   $tmpsasfit(Q)
						lappend tmpsasfit(file,I)   $tmpsasfit(I)
						lappend tmpsasfit(file,DI)  $tmpsasfit(DI)
						lappend tmpsasfit(file,res) $tmpsasfit(res)
						set tmpsasfit(res,calc) [sasfit_res $resolution(lambda)  \
													$resolution(Dlambda) \
													$resolution(r1)      \
													$resolution(l1)      \
													$resolution(r2)      \
													$resolution(l2)      \
													$resolution(d)       \
													$resolution(Dd)      \
													$tmpsasfit(Q)       \
												]
						lappend tmpsasfit(file,res,file) $tmpsasfit(res,file)
						lappend tmpsasfit(file,res,calc) $tmpsasfit(res,calc)
						if {$tmpAnalytPar(geometrical/datafile)} {
							lappend tmpsasfit(file,res) $tmpsasfit(res,calc)
						} else {
							lappend tmpsasfit(file,res) $tmpsasfit(res,file)
						}
						incr tmpsasfit(file,n)
						MergeFileCmd tmpsasfit
					} else {
						lappend errornessFiles  $tmpsasfit(filename)
					}
				}
				if {[llength $errornessFiles] > 0} {
					set msgtxt ""
					set indx 1
					foreach fin  $errornessFiles {
						set msgtxt "$msgtxt\n$indx\. $fin"
						incr indx
					}
					tk_messageBox -icon error \
                        -message "Could not read data files:\n$msgtxt\n\nPlease review the input format options !"
					if {[winfo exists .addfile]} {destroy .addfile}
					if {$tmpsasfit(file,n) == 0} {
						set addsasfit(filename) $tmpfnlist
						set ::show_new_file_dialog 1 
					}
					AddCmd
				}
			} \
       -highlightthickness 0 -pady 1m
pack $w.layout1.label $w.layout1.format \
     -side left -fill x
pack $w.layout1.option -side left  -fill x -padx 3m
pack $w.layout1.read   -side right -fill x -padx 2m

label $w.layout2.label -text "File Name:" \
      -width 12 -highlightthickness 0
entry $w.layout2.entry -width 30 -relief sunken -highlightthickness 0 \
                              -textvariable tmpsasfit(filename)
button $w.layout2.browse -text "Browse..." -highlightthickness 0 \
       -command {BrowseCmd .addfile tmpsasfit(filename) \
                           tmpsasfit(actualdatatype) ReadData }
pack $w.layout2.label $w.layout2.entry -side left
pack $w.layout2.browse  -side left -padx 2m

button $w.layout2.readclipboard -text "Read from Clipboard" \
       -command {
               global tmpsasfit
               if {[string compare \
                      [ReadClipboardCmd tmpsasfit /norefreshdata] no] == 0} {
                  lappend tmpsasfit(file,Q)   $tmpsasfit(Q)
                  lappend tmpsasfit(file,I)   $tmpsasfit(I)
                  lappend tmpsasfit(file,DI)  $tmpsasfit(DI)
                  lappend tmpsasfit(file,res) $tmpsasfit(res)
                  lappend tmpsasfit(file,res,file) $tmpsasfit(res,file)
                  lappend tmpsasfit(file,res,calc) $tmpsasfit(res,calc)
                  incr tmpsasfit(file,n)
                  MergeFileCmd tmpsasfit
	       } else {
                  tk_messageBox -icon error \
                     -message "could not read data file $tmpsasfit(filename)"
	       }
       } \
       -highlightthickness 0 -pady 1m
pack $w.layout2.readclipboard   -side right -fill x -padx 2m

set w2 .addfile.lay2
label $w2.file -text "file name:" \
      -highlightthickness 0 -justify left -anchor w
grid  $w2.file -column 0 -row 0 -sticky w 
label $w2.divisor -text "divisor" \
      -highlightthickness 0 -justify left -anchor w
grid  $w2.divisor -column 1 -row 0 -sticky w
label $w2.skipfirst -text "skip first\nn points" \
      -highlightthickness 0 -justify left -anchor w
grid  $w2.skipfirst -column 2 -row 0 -sticky w
label $w2.skiplast -text "skip last\npoints" \
      -highlightthickness 0 -justify left -anchor w
grid  $w2.skiplast -column 3 -row 0 -sticky w
label $w2.hide -text "hide" \
      -highlightthickness 0 -justify left -anchor w
grid  $w2.hide -column 4 -row 0 -sticky w
frame $w2.resframe
grid  $w2.resframe -column 6 -row 0 -sticky w
checkbutton $w2.resframe.geom -text "calc resolution\nfrom geometrical values (on)\ntake values from data file (off)" \
      -variable tmpAnalytPar(geometrical/datafile) \
      -onvalue yes -offvalue no \
      -justify left
grid $w2.resframe.geom  -column 0 -row 1 -sticky w



set canvasheight 4c

scrollbar $w2.sbar -command "hsetyview"
grid   $w2.sbar -column 7 -row 1 -sticky ns
grid rowconfigure $w2 1 -weight 2 -minsize $canvasheight

canvas $w2.vportfile -width 1  -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportfile -column 0 -row 1 
frame  $w2.vportfile.frame 
$w2.vportfile create window 0 0 -anchor nw -window $w2.vportfile.frame
bind $w2.vportfile.frame <Configure> "scrollform_resize $w2.vportfile"

canvas $w2.vportdivisor -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportdivisor -column 1 -row 1
frame  $w2.vportdivisor.frame
$w2.vportdivisor create window 0 0 -anchor nw -window $w2.vportdivisor.frame
bind $w2.vportdivisor.frame <Configure> "scrollform_resize $w2.vportdivisor"

canvas $w2.vportsfirst -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportsfirst -column 2 -row 1
frame  $w2.vportsfirst.frame
$w2.vportsfirst create window 0 0 -anchor nw -window $w2.vportsfirst.frame
bind $w2.vportsfirst.frame <Configure> "scrollform_resize $w2.vportsfirst"

canvas $w2.vportslast -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportslast -column 3 -row 1
frame  $w2.vportslast.frame
$w2.vportslast create window 0 0 -anchor nw -window $w2.vportslast.frame
bind $w2.vportslast.frame <Configure> "scrollform_resize $w2.vportslast"

canvas $w2.vporthide -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vporthide -column 4 -row 1
frame  $w2.vporthide.frame
$w2.vporthide create window 0 0 -anchor nw -window $w2.vporthide.frame
bind $w2.vporthide.frame <Configure> "scrollform_resize $w2.vporthide"

canvas $w2.vportdel -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportdel -column 5 -row 1
frame  $w2.vportdel.frame
$w2.vportdel create window 0 0 -anchor nw -window $w2.vportdel.frame
bind $w2.vportdel.frame <Configure> "scrollform_resize $w2.vportdel"

canvas $w2.vportres -width 1 -height $canvasheight \
       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
grid   $w2.vportres -column 6 -row 1
frame  $w2.vportres.frame
$w2.vportres create window 0 0 -anchor nw -window $w2.vportres.frame
bind $w2.vportres.frame <Configure> "scrollform_resize $w2.vportres"

if {[llength $tmpsasfit(filename)] > 0} {
	$w.layout1.read invoke
} else {
	for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
		MergeFileCmd tmpsasfit $i
	}
}
if {![winfo exists .addfile] || [winfo exists .addfile.layout3.read]
} {return}
button .addfile.layout3.read \
       -text OK -command {
                          global tmpsasfit
			  destroy_ascii_options
                          merge_cmd_apply tmpsasfit 1
                          catch {destroy .addfile}
                         }\
       -highlightthickness 0
button .addfile.layout3.apply \
       -text Apply -command {
                             global tmpsasfit
			     destroy_ascii_options
                             merge_cmd_apply tmpsasfit 1
                            }\
       -highlightthickness 0
button .addfile.layout3.dismiss -text Cancel \
       -highlightthickness 0 -command {
					destroy_ascii_options
                                        destroy .addfile
                                      }

button .addfile.layout3.new -text "New..." \
       -highlightthickness 0 -command {
           zero_sasfit addsasfit
	   set ::show_new_file_dialog 1
           AddCmd
}

checkbutton .addfile.layout3.dores -text "use resolution" \
	-onvalue yes -offvalue no \
	-variable ::tmpGlobalAnalytPar(resolution)

pack .addfile.layout3.read .addfile.layout3.apply  \
     .addfile.layout3.dismiss .addfile.layout3.new \
     .addfile.layout3.dores \
     -side left -padx 6 -fill x 

pack $w.layout1 $w.layout2 .addfile.layout3 \
     -fill x -pady 2 -pady 6
$nomenu_add activate 0
$nomenu_add invoke 0
set ::show_new_file_dialog 0
}

