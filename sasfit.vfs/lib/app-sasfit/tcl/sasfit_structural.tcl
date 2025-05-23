# sasfit.vfs/lib/app-sasfit/tcl/sasfit_structural.tcl
#
# Copyright (c) 2008-2019, Paul Scherrer Institute (PSI)
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
#   Ingo Bre�ler (sasfit@ingobressler.net)

#------------------------------------------------------------------------------
#  calculates Guinier and Porod approximations and integral structural 
#  parameters of the scattering curves like Guinier radius, forward scattering,
#  Porod constant, Background, scattering invariant, Porod volume, specific 
#  surface, correlation length, intersection length
#
proc load_data_file { configarr filename
} {
    upvar $configarr arr
	set ::sasfit(filename) $filename
	if {![string equal [ReadFileCmd ::sasfit] no]} {
		tk_messageBox -parent . -icon error -message \
			"could not read data file $::sasfit(filename)"
		return
	}
	set ::sasfit(file,Q) {}
	set ::sasfit(file,I)  {}
	set ::sasfit(file,DI) {}
	lappend ::sasfit(file,Q)  $::sasfit(Q)
	lappend ::sasfit(file,I)  $::sasfit(I)
	lappend ::sasfit(file,DI) $::sasfit(DI)
	set ::sasfit(file,widcnt) 0
	set ::sasfit(file,n) 1
	set ::sasfit(file,name)  {}
	lappend ::sasfit(file,name) $::sasfit(filename)
	set ::sasfit(filename) ""
	set ::sasfit(file,widcnt)     0
#	set ::sasfit(file,divisor)   {1}
#	set ::sasfit(file,firstskip) {0}
#	set ::sasfit(file,lastskip)  {0}
	set ::sasfit(file,hide)      {no}
	if {[llength ::sasfit(file,name)] > 0} {
		set fileindex [expr [llength ::sasfit(file,name)]-1]
		set ::hide($fileindex) no
	}
	dr_default_op set ::sasfit "file,"
	set ::sasfit(file,widname)   {{.addfile.lay2 0}}
	eval $arr(seriesLoadCmd)
}

proc EmOptionsCmd {} {
	global EMOptions
	if { [winfo exists .emoptions] } {destroy .emoptions}
	toplevel .emoptions
	set wr .emoptions
	wm geometry $wr
	wm title $wr "internal parameters for EM algorithm"

	frame $wr.title -relief raised -bd 2
	frame $wr.general -relief flat -bd 2
	frame $wr.internal -relief flat -bd 2
	pack $wr.title $wr.general $wr.internal -fill x -pady 2m 
	label $wr.title.methodlabel -text "method:"
	ComboBox $wr.title.method -values {"MuCh" "EM (smoothing)" \
    		         "EM (ME constant prior)" "EM (ME adaptive prior)" \
    		         "SDM" "lin Reg" "MEM"}  \
				-width 18 \
				-textvariable ::EMOptions(method)  \
				-modifycmd {
						switch $::EMOptions(method) {
  						   "EM (smoothing)"         {.emoptions.notebook raise 1}
						   "EM (ME constant prior)" {.emoptions.notebook raise 2}
						   "EM (ME adaptive prior)" {.emoptions.notebook raise 3}
						   "lin Reg"                {.emoptions.notebook raise 4}
						   default                  {.emoptions.notebook raise 5}
						}
				}
	
#	grid  $wr.title.methodlabel -row 0 -column 0 -sticky e
#	grid  $wr.title.method -row 0 -column 1 -sticky w
	pack $wr.title.methodlabel $wr.title.method -side left
	set w [NoteBook $wr.notebook]
	pack $w
	set em_smooth_f   [$w insert 1 1  -text "EM (smoothing)"         -state normal ]
	set em_const_f    [$w insert 2 2  -text "EM (ME constant prior)" -state normal ]
	set em_adaptive_f [$w insert 3 3  -text "EM (ME adaptive prior)" -state normal ]
	set lin_reg_f     [$w insert 4 4  -text "lin Reg"                -state normal ]
	set missing_f     [$w insert 5 5  -text "missing"                -state normal ]
	switch $::EMOptions(method) {
		"EM (smoothing)"         {.emoptions.notebook raise 1}
		"EM (ME constant prior)" {.emoptions.notebook raise 2}
		"EM (ME adaptive prior)" {.emoptions.notebook raise 3}
		"lin Reg"                {.emoptions.notebook raise 4}
		default                  {.emoptions.notebook raise 5}
	}
	set entrywidth 8
	set w  $wr.general
	label $w.rmaxlabel -text "          Rmax ="
	entry $w.rmaxvalue -textvariable EMOptions(Rmax)  -width $entrywidth
	grid $w.rmaxlabel -row 0 -column 0 -sticky e
	grid $w.rmaxvalue -row 0 -column 1 -sticky w
	
	label $w.nRlabel -text "       nR ="
	entry $w.nRvalue -textvariable EMOptions(nR) -width $entrywidth
	grid $w.nRlabel -row 0 -column 2 -sticky e
	grid $w.nRvalue -row 0 -column 3 -sticky w
	
	label $w.spacing_label -text "            spacing:"
	ComboBox $w.spacing_value -values {"lin" "log"} \
				-width 10 \
				-textvariable ::EMOptions(spacing) 
	grid $w.spacing_label -row 1 -column 0 -sticky e
	grid $w.spacing_value -row 1 -column 1 -sticky w

	label $w.nLagrange_label -text "number of Lagrange values"
	entry $w.nLagrange_value -textvariable EMOptions(nLagrange) -width $entrywidth
				
	grid $w.nLagrange_label -row 2 -column 0 -sticky e
	grid $w.nLagrange_value -row 2 -column 1 -sticky w	

	label $w.maxslope_label -text "maximum slope"
	entry $w.maxslope_value -textvariable EMOptions(maxslope) -width $entrywidth
				
	grid $w.maxslope_label -row 2 -column 2 -sticky e
	grid $w.maxslope_value -row 2 -column 3 -sticky w	


	label $w.minLagrange_label -text "min Lagrange values"
	entry $w.minLagrange_value -textvariable EMOptions(minLagrange) -width $entrywidth
				
	grid $w.minLagrange_label -row 3 -column 0 -sticky e
	grid $w.minLagrange_value -row 3 -column 1 -sticky w	


	label $w.maxLagrange_label -text "       max Lagrange values"
	entry $w.maxLagrange_value -textvariable EMOptions(maxLagrange) -width $entrywidth
				
	grid $w.maxLagrange_label -row 3 -column 2 -sticky e
	grid $w.maxLagrange_value -row 3 -column 3 -sticky w	
	

	label $w.dimlabel -text "dim ="
	entry $w.dimvalue -textvariable EMOptions(dim) -width $entrywidth
	grid $w.dimlabel -row 5 -column 0 -sticky e
	grid $w.dimvalue -row 5 -column 1 -sticky w

    label $w.opt_Lagrange_label -text "opt. Lagrange param."
	ComboBox $w.opt_Lagrange_value -values {"L-corner (o)" "L-corner2" "L-corner (l)" "L-corner (w)" "L-corner (w+o)" "GCV" "red. chi2" "manual"
			}  \
				-width 10 \
				-textvariable ::EMOptions(optimumLagrange) 
				
	grid $w.opt_Lagrange_label -row 5 -column 2 -sticky e
	grid $w.opt_Lagrange_value -row 5 -column 3 -sticky w
	
	
	label $w.chi2label -text "chi2/N ="
	entry $w.chi2value -textvariable EMOptions(chi2) -width $entrywidth
	grid $w.chi2label -row 6 -column 0 -sticky e
	grid $w.chi2value -row 6 -column 1 -sticky w
	
	label $w.lamlabel -text "lambda ="
	entry $w.lamvalue -textvariable EMOptions(lambda) -width $entrywidth
	grid $w.lamlabel -row 6 -column 2 -sticky e
	grid $w.lamvalue -row 6 -column 3 -sticky w
		
	label $w.printProg_label -text "PrintProgress"
	ComboBox $w.printProg_value -values {0 1 2 3 4 5
			}  \
				-width $entrywidth \
				-textvariable ::EMOptions(PrintProgress) 
#	entry $w.printProg_value -textvariable EMOptions(PrintProgress) -width $entrywidth
				
	grid $w.printProg_label -row 7 -column 0 -sticky e
	grid $w.printProg_value -row 7 -column 1 -sticky w	
	set w $em_smooth_f
	label $w.epsSmoothlabel -text "smoothing value ="
	entry $w.epsSmoothvalue -textvariable EMOptions(smooth) -width $entrywidth
	grid  $w.epsSmoothlabel -row 1 -column 0 -sticky e
	grid  $w.epsSmoothvalue -row 1 -column 1 -sticky w
	
	label $w.seedlabel -text "seed :"
	ComboBox $w.seedvalue -values {"constant" "random"} \
				-width 10 \
				-textvariable EMOptions(seed) 
	grid $w.seedlabel -row 1 -column 2 -sticky e
	grid $w.seedvalue -row 1 -column 3 -sticky w
	
	label $w.maxitlabel -text "maxit ="
	entry $w.maxitvalue -textvariable EMOptions(maxit) -width $entrywidth
	grid $w.maxitlabel -row 3 -column 2 -sticky e
	grid $w.maxitvalue -row 3 -column 3 -sticky w
	
	label $w.intStrat_label -text "iteration scheme"
	ComboBox $w.intStrat_value -values {"Picard iteration" "Mann iteration" \
				 "Biggs_Andrews" "Anderson mixing" "KINSOL_FP"  
			}  \
				-width 16 \
				-textvariable ::EMOptions(IterationScheme) 

	grid $w.intStrat_label -row 4 -column 0 -sticky e
	grid $w.intStrat_value -row 4 -column 1 -sticky w
	
	label $w.epslabel -text "precision ="
	entry $w.epsvalue -textvariable EMOptions(eps) -width $entrywidth
	grid $w.epslabel -row 4 -column 2 -sticky e
	grid $w.epsvalue -row 4 -column 3 -sticky w

	label $w.smooth_type_label -text "smooth type"
	ComboBox $w.smooth_type_value -values {"single" "double"} \
				-width 10 \
				-textvariable ::EMOptions(smooth_type) 
	grid $w.smooth_type_label -row 5 -column 0 -sticky e
	grid $w.smooth_type_value -row 5 -column 1 -sticky w
	
	
	label $w.linRegu_label -text "regul. matrix"
	ComboBox $w.linRegu_value -values {"Idendity" "first deriv." "first deriv. (eps_b)" "first deriv. (eps_e)"
				"second deriv." "second deriv. (D-D)" "second deriv. (N-N)" "second deriv. (D-N)" "second deriv. (N-D)"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LMatrix) 
				
	grid $w.linRegu_label -row 7 -column 0 -sticky e
	grid $w.linRegu_value -row 7 -column 1 -sticky w
	
	
	label $w.ls_method_label -text "LS method:"
	ComboBox $w.ls_method_value -values {"LLS" "NNLLS"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LLSmethod) 
				
	grid $w.ls_method_label -row 8 -column 0 -sticky e
	grid $w.ls_method_value -row 8 -column 1 -sticky w
	
	label $w.em_weight_label -text "error weighted EM analysis"
	ComboBox $w.em_weight_value -values {"on" "off"} \
				-width 10 \
				-textvariable ::EMOptions(error_weight) 
	grid $w.em_weight_label -row 8 -column 2 -sticky e
	grid $w.em_weight_value -row 8 -column 3 -sticky w	
	
	label $w.overrelaxation_label -text "overrelaxation param.:"
	entry $w.overrelaxation_value -textvariable EMOptions(overrelaxation) -width $entrywidth 
						
	grid $w.overrelaxation_label -row 9 -column 0 -sticky e
	grid $w.overrelaxation_value -row 9 -column 1 -sticky w
	
	label $w.maxkrylov_label -text "max. Krylov space dim.:"
	entry $w.maxkrylov_value -textvariable EMOptions(maxKrylov) -width $entrywidth 
				
	grid $w.maxkrylov_label -row 9 -column 2 -sticky e
	grid $w.maxkrylov_value -row 9 -column 3 -sticky w

	
	


	set w $em_const_f
#	label $w.epsSmoothlabel -text "smoothing value ="
#	entry $w.epsSmoothvalue -textvariable EMOptions(smooth) -width $entrywidth
#	grid  $w.epsSmoothlabel -row 1 -column 0 -sticky e
#	grid  $w.epsSmoothvalue -row 1 -column 1 -sticky w
	
	label $w.seedlabel -text "seed :"
	ComboBox $w.seedvalue -values {"constant" "random"} \
				-width 10 \
				-textvariable EMOptions(seed) 
	grid $w.seedlabel -row 1 -column 2 -sticky e
	grid $w.seedvalue -row 1 -column 3 -sticky w
	
	label $w.maxitlabel -text "maxit ="
	entry $w.maxitvalue -textvariable EMOptions(maxit) -width $entrywidth
	grid $w.maxitlabel -row 3 -column 2 -sticky e
	grid $w.maxitvalue -row 3 -column 3 -sticky w
	
	label $w.intStrat_label -text "iteration scheme"
	ComboBox $w.intStrat_value -values {"Picard iteration" "Mann iteration" \
    		         "Biggs_Andrews" "Anderson mixing" "KINSOL_FP"  
			}  \
				-width 16 \
				-textvariable ::EMOptions(IterationScheme) 

	grid $w.intStrat_label -row 4 -column 0 -sticky e
	grid $w.intStrat_value -row 4 -column 1 -sticky w
	
	label $w.epslabel -text "precision ="
	entry $w.epsvalue -textvariable EMOptions(eps) -width $entrywidth
	grid $w.epslabel -row 4 -column 2 -sticky e
	grid $w.epsvalue -row 4 -column 3 -sticky w

	label $w.smooth_type_label -text "smooth type"
	ComboBox $w.smooth_type_value -values {"single" "double"} \
				-width 10 \
				-textvariable ::EMOptions(smooth_type) 
	grid $w.smooth_type_label -row 5 -column 0 -sticky e
	grid $w.smooth_type_value -row 5 -column 1 -sticky w
	
	
	label $w.linRegu_label -text "regul. matrix"
	ComboBox $w.linRegu_value -values {"Idendity" "first deriv." "first deriv. (eps_b)" "first deriv. (eps_e)"
				"second deriv." "second deriv. (D-D)" "second deriv. (N-N)" "second deriv. (D-N)" "second deriv. (N-D)"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LMatrix) 
				
	grid $w.linRegu_label -row 7 -column 0 -sticky e
	grid $w.linRegu_value -row 7 -column 1 -sticky w
	
	label $w.opt_Lagrange_label -text "opt. Lagrange param."
	ComboBox $w.opt_Lagrange_value -values {"L-corner (o)" "L-corner2" "L-corner (l)" "L-corner (w)" "L-corner (w+o)" "GCV" "red. chi2" "manual"
			}  \
				-width 10 \
				-textvariable ::EMOptions(optimumLagrange) 
				
	grid $w.opt_Lagrange_label -row 7 -column 2 -sticky e
	grid $w.opt_Lagrange_value -row 7 -column 3 -sticky w
	
	label $w.ls_method_label -text "LS method:"
	ComboBox $w.ls_method_value -values {"LLS" "NNLLS"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LLSmethod) 
				
	grid $w.ls_method_label -row 8 -column 0 -sticky e
	grid $w.ls_method_value -row 8 -column 1 -sticky w
	
	label $w.em_weight_label -text "error weighted EM analysis"
	ComboBox $w.em_weight_value -values {"on" "off"} \
				-width 10 \
				-textvariable ::EMOptions(error_weight) 
	grid $w.em_weight_label -row 8 -column 2 -sticky e
	grid $w.em_weight_value -row 8 -column 3 -sticky w	
	
	label $w.overrelaxation_label -text "overrelaxation param.:"
	entry $w.overrelaxation_value -textvariable EMOptions(overrelaxation) -width $entrywidth 
						
	grid $w.overrelaxation_label -row 9 -column 0 -sticky e
	grid $w.overrelaxation_value -row 9 -column 1 -sticky w
	
	label $w.maxkrylov_label -text "max. Krylov space dim.:"
	entry $w.maxkrylov_value -textvariable EMOptions(maxKrylov) -width $entrywidth 
				
	grid $w.maxkrylov_label -row 9 -column 2 -sticky e
	grid $w.maxkrylov_value -row 9 -column 3 -sticky w
	
	set w $lin_reg_f
#	label $w.epsSmoothlabel -text "smoothing value ="
#	entry $w.epsSmoothvalue -textvariable EMOptions(smooth) -width $entrywidth
#	grid  $w.epsSmoothlabel -row 1 -column 0 -sticky e
#	grid  $w.epsSmoothvalue -row 1 -column 1 -sticky w
	
	label $w.seedlabel -text "seed :"
	ComboBox $w.seedvalue -values {"constant" "random"} \
				-width 10 \
				-textvariable EMOptions(seed) 
	grid $w.seedlabel -row 1 -column 2 -sticky e
	grid $w.seedvalue -row 1 -column 3 -sticky w
	
	label $w.maxitlabel -text "maxit ="
	entry $w.maxitvalue -textvariable EMOptions(maxit) -width $entrywidth
	grid $w.maxitlabel -row 3 -column 2 -sticky e
	grid $w.maxitvalue -row 3 -column 3 -sticky w
	
	label $w.intStrat_label -text "iteration scheme"
	ComboBox $w.intStrat_value -values {"Picard iteration" "Mann iteration" \
    		         "Biggs_Andrews" "Anderson mixing" "KINSOL_FP"  
			}  \
				-width 16 \
				-textvariable ::EMOptions(IterationScheme) 

	grid $w.intStrat_label -row 4 -column 0 -sticky e
	grid $w.intStrat_value -row 4 -column 1 -sticky w
	
	label $w.epslabel -text "precision ="
	entry $w.epsvalue -textvariable EMOptions(eps) -width $entrywidth
	grid $w.epslabel -row 4 -column 2 -sticky e
	grid $w.epsvalue -row 4 -column 3 -sticky w

	label $w.smooth_type_label -text "smooth type"
	ComboBox $w.smooth_type_value -values {"single" "double"} \
				-width 10 \
				-textvariable ::EMOptions(smooth_type) 
	grid $w.smooth_type_label -row 5 -column 0 -sticky e
	grid $w.smooth_type_value -row 5 -column 1 -sticky w
	
	
	label $w.linRegu_label -text "regul. matrix"
	ComboBox $w.linRegu_value -values {"Idendity" "first deriv." "first deriv. (eps_b)" "first deriv. (eps_e)"
				"second deriv." "second deriv. (D-D)" "second deriv. (N-N)" "second deriv. (D-N)" "second deriv. (N-D)"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LMatrix) 
				
	grid $w.linRegu_label -row 7 -column 0 -sticky e
	grid $w.linRegu_value -row 7 -column 1 -sticky w
	
	label $w.opt_Lagrange_label -text "opt. Lagrange param."
	ComboBox $w.opt_Lagrange_value -values {"L-corner (o)" "L-corner2" "L-corner (l)" "L-corner (w)" "L-corner (w+o)" "GCV" "red. chi2" "manual"
			}  \
				-width 10 \
				-textvariable ::EMOptions(optimumLagrange) 
				
	grid $w.opt_Lagrange_label -row 7 -column 2 -sticky e
	grid $w.opt_Lagrange_value -row 7 -column 3 -sticky w
	
	label $w.ls_method_label -text "LS method:"
	ComboBox $w.ls_method_value -values {"LLS" "NNLLS"
			}  \
				-width 16 \
				-textvariable ::EMOptions(LLSmethod) 
				
	grid $w.ls_method_label -row 8 -column 0 -sticky e
	grid $w.ls_method_value -row 8 -column 1 -sticky w
	
	label $w.em_weight_label -text "error weighted EM analysis"
	ComboBox $w.em_weight_value -values {"on" "off"} \
				-width 10 \
				-textvariable ::EMOptions(error_weight) 
	grid $w.em_weight_label -row 8 -column 2 -sticky e
	grid $w.em_weight_value -row 8 -column 3 -sticky w	
	
	label $w.overrelaxation_label -text "overrelaxation param.:"
	entry $w.overrelaxation_value -textvariable EMOptions(overrelaxation) -width $entrywidth 
						
	grid $w.overrelaxation_label -row 9 -column 0 -sticky e
	grid $w.overrelaxation_value -row 9 -column 1 -sticky w
	
	label $w.maxkrylov_label -text "max. Krylov space dim.:"
	entry $w.maxkrylov_value -textvariable EMOptions(maxKrylov) -width $entrywidth 
				
	grid $w.maxkrylov_label -row 9 -column 2 -sticky e
	grid $w.maxkrylov_value -row 9 -column 3 -sticky w
	

}
proc structuralParFitCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global StructParData 
global IQGraph
global sasfit

proc menu_disable {} {
	.top.fit.menu entryconfigure 0 -state disabled
	.top.file.menu entryconfigure 0 -state disabled
	.top.file.menu entryconfigure 1 -state disabled
}

proc menu_enable {} {
	.top.fit.menu entryconfigure 0 -state normal
	.top.file.menu entryconfigure 0 -state normal
	.top.file.menu entryconfigure 1 -state normal
}

set ::StructParData(seriesLoadCmd) { RefreshStructParFit }
set ::StructParData(seriesSaveCmd) { structparSaveResult }

if {[winfo exists .structural]} {destroy .structural}
toplevel .structural
menu_disable
set taglist [bindtags .structural]
lappend taglist WindowCloseTag
bindtags .structural $taglist
bind WindowCloseTag <Destroy> {
	menu_enable
	if {[info exists ::StructParData(prev_file)]} {
#		load_data_file ::StructParData [lindex $::StructParData(prev_file) 0]
		merge_cmd_apply tmpsasfit 0
	}
}
if {[info exists ::sasfit(file,name)]} { 
    set ::StructParData(prev_file) $::sasfit(file,name)
}

set w .structural
wm geometry $w
wm title $w "integral structural parameters"
raise $w
focus $w
set StructParData(plottype) -1

#
# definition of the pull down menu
#
frame $w.menu -relief raised -bd 2
pack $w.menu -side top -fill x
menubutton $w.menu.file -text File -underline 0 \
           -menu $w.menu.file.menu
menu $w.menu.file.menu
$w.menu.file.menu add command -label "Quit" \
     -command { 
	 if {[winfo exists .structural]} {destroy .structural}
      }
menubutton $w.menu.options -text Options -underline 0 \
          -menu $w.menu.options.menu
   menu $w.menu.options.menu

   plot_axis_menu .structural ::IQGraph ::StructParData(plottype)

   $w.menu.options.menu add command -label "EM options" \
           -command "EmOptionsCmd"
		   
sasfit_menubar_build_help $w.menu.help
pack $w.menu.file $w.menu.options -side left
pack $w.menu.help -side right
#
#
#
frame $w.user
pack $w.user -side top -fill both
set w $w.user
#
# frames for guinier and porod fit ranges plus
# frames for guinier and porod fit results
#
frame $w.guinierrange
frame $w.porodrange
frame $w.guinierresult 
frame $w.porodresult   
frame $w.loadandsave

grid $w.guinierrange  -column 0 -row 0 -ipady 2m
grid $w.guinierresult -column 1 -row 0 -ipady 2m
grid $w.porodrange    -column 0 -row 1 -ipady 2m 
grid $w.porodresult   -column 1 -row 1 -ipady 2m
grid $w.loadandsave   -column 0 -row 2 -ipady 2m -columnspan 2 -sticky nsew

grid columnconfigure $w {0 1} -weight 1
grid columnconfigure $w {0 1} -weight 1

frame $w.guinierrange.lowQ
grid  $w.guinierrange.lowQ -columnspan 4
frame $w.guinierrange.lowQ.radio
frame $w.guinierrange.lowQ.radio2
pack $w.guinierrange.lowQ.radio -fill both -expand yes
pack $w.guinierrange.lowQ.radio2 -fill both -expand yes
radiobutton $w.guinierrange.lowQ.radio.guinier -text "Guinier" \
            -value "Guinier approx.:\nI(Q) = I0 exp(-Rg^2*Q^2/3)" \
            -variable StructParData(lowQText) \
            -command {
                global StructParData
                set StructParData(I0typestr) Guinier
                set StructParData(lowQFit) "Guinier fit results:"
                RefreshStructParFit
	    } \
            -highlightthickness 0 
radiobutton $w.guinierrange.lowQ.radio2.debye -text "Debye" \
            -value "Debye approx.: u=(Rg Q)^2\nI(Q) = 2 I0 (u-1+exp(-u))/u^2" \
            -variable StructParData(lowQText) \
            -command {
                global StructParData
                set StructParData(I0typestr) Debye
                set StructParData(lowQFit) "Debye fit results:"
                RefreshStructParFit
	    } \
            -highlightthickness 0
radiobutton $w.guinierrange.lowQ.radio.zimm -text "Zimm" \
            -value "Zimm approx.:\n1/I(Q) = 1/I0 (1+Rg^2*Q^2/3)" \
            -variable StructParData(lowQText) \
            -command {
                global StructParData
                set StructParData(I0typestr) Zimm
                set StructParData(lowQFit) "Zimm fit results:"
                RefreshStructParFit
	    } \
            -highlightthickness 0
			
button $w.guinierrange.lowQ.nr -text "calc." \
            -command {
                global StructParData IQGraph SDGraph EMOptions
                RefreshStructParFit
   		        sasfit_timer_start "\nStart apply"
				set ::SASfitinterrupt 0
				set EMOptions(Interrupt) 0
				switch $::EMOptions(method) {
					"MuCh" {
							set DR [sasfit_DR_MuCh StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"EM (smoothing)" { 
							set DR [sasfit_DR_EM_smoothing StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"SDM" { 
							set DR [sasfit_DR_SDM StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"lin Reg" { 
							set DR [sasfit_DR_linReg StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"MEM" { 
							set DR [sasfit_DR_MEM StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"EM (ME constant prior)" { 
							set DR [sasfit_DR_EM_ME_const StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
					"EM (ME adaptive prior)" { 
							set DR [sasfit_DR_EM_ME_adaptive StructParData [list $sasfit(Q) $sasfit(I) $sasfit(DI)]]
						 }
				}
				sasfit_timer_stop "Apply" "finished" ""
				
				clearGraph_el SDGraph
				Put_Graph_el SDGraph [lindex $DR 0] [lindex $DR 1]
				
				set SDGraph(e,symbol)     [lreplace $SDGraph(e,symbol) 0 0 none]
				set SDGraph(e,linehide)   [lreplace $SDGraph(e,linehide) 0 0 1]
				RefreshGraph SDGraph
				
				clearGraph_el IQGraph
				set indx1 -1
				Put_Graph_el IQGraph [lindex $DR 2] [lindex $DR 3]
				
				
	incr indx1
	set IQGraph(e,symbol)     [lreplace $IQGraph(e,symbol) $indx1 $indx1 none]
	set IQGraph(e,linehide)   [lreplace $IQGraph(e,linehide) $indx1 $indx1 1]
	set IQGraph(e,dashcolor)  [lreplace $IQGraph(e,dashcolor) $indx1 $indx1 red]
	set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
				       $indx1 $indx1 Fit]

	# draw the data (for fit mode)
		Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $sasfit(DI) $sasfit(res)
		incr indx1		
		RefreshGraph IQGraph
	    } \
            -highlightthickness 0
ComboBox $w.guinierrange.lowQ.method -values {"MuCh" "EM (smoothing)" \
    		         "EM (ME constant prior)" "EM (ME adaptive prior)" \
    		         "SDM" "lin Reg" "MEM"}  \
				-width 18 \
				-textvariable ::EMOptions(method) 
				
ComboBox $w.guinierrange.lowQ.objtype -values {"sphere" "local cyl." \
    		         "local planar"}  \
				-width 10 \
				-textvariable ::EMOptions(objtype) 	
				
pack $w.guinierrange.lowQ.radio.guinier \
     $w.guinierrange.lowQ.radio.zimm \
	 $w.guinierrange.lowQ.nr \
	 $w.guinierrange.lowQ.method \
	 $w.guinierrange.lowQ.objtype\
     -padx 2m  -pady 1m \
     -fill both -expand yes -side left -anchor w
#pack $w.guinierrange.lowQ.radio2.debye \
#     -padx 2m  -pady 1m \
#     -fill both -side left -anchor w
label $w.guinierrange.formula -justify left \
     -textvariable StructParData(lowQText)
grid $w.guinierrange.formula -columnspan 4 -row 1 -sticky w -padx 1m

radiobutton $w.guinierrange.bypointcount -text "point count" \
	-value 0 -variable ::StructParData(GuinierByQRange) \
	-highlightthickness 0 -command { configGuinierByPointCount }
radiobutton $w.guinierrange.byqrange -text "Q range" \
	-value 1 -variable ::StructParData(GuinierByQRange) \
	-highlightthickness 0 -command { configGuinierByQRange }
grid $w.guinierrange.bypointcount $w.guinierrange.byqrange - - \
	-row 2 -sticky w -padx 1m

proc configGuinierByPointCount {} {
	verifyUserInput
	set w .structural.user
	$w.guinierrange.first configure -text "1st pt. (asc.):"
	$w.guinierrange.nfirst configure \
		-textvariable ::StructParData(Guinierfirst)
	$w.guinierrange.up1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Guinierfirst) \
			< ( $::StructParData(npoints)\
			    - $::StructParData(Guiniernpoints)+1 )
		} {
			incr ::StructParData(Guinierfirst)
		}
		RefreshStructParFit
	}
	$w.guinierrange.down1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Guinierfirst) > 1 } { 
			incr ::StructParData(Guinierfirst) -1
		}
		RefreshStructParFit
	}
	$w.guinierrange.npoints configure -text "number of pts.:"
	$w.guinierrange.nnpoints configure \
		-textvariable ::StructParData(Guiniernpoints)
	$w.guinierrange.up2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Guiniernpoints) \
			< ( $::StructParData(npoints)\
			    - $::StructParData(Guinierfirst)+1 )
		} {
			incr ::StructParData(Guiniernpoints)
		}
		RefreshStructParFit
	}
	$w.guinierrange.down2 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Guiniernpoints) > 3 } { 
			incr ::StructParData(Guiniernpoints) -1
		}
		RefreshStructParFit
	}
}
proc configGuinierByQRange {} {
	verifyUserInput
	set w .structural.user
	$w.guinierrange.first configure -text "Q min:"
	$w.guinierrange.nfirst configure \
		-textvariable ::StructParData(GuinierQmin)
	$w.guinierrange.up1 configure -armcommand {
		verifyUserInput
		# same conditions as in 'ByPointCount' but different body
		if { $::StructParData(Guinierfirst) \
			< ( $::StructParData(npoints)\
			    - $::StructParData(Guiniernpoints)+1 )
		} {
			incr ::StructParData(Guinierfirst)
			incr ::StructParData(Guiniernpoints) -1
			setStructParQval GuinierQmin
		}
		RefreshStructParFit
	}
	$w.guinierrange.down1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Guinierfirst) > 1 } { 
			incr ::StructParData(Guinierfirst) -1
			incr ::StructParData(Guiniernpoints) 1
			setStructParQval GuinierQmin
		}
		RefreshStructParFit
	}
	$w.guinierrange.npoints configure -text "Q max:"
	$w.guinierrange.nnpoints configure \
		-textvariable ::StructParData(GuinierQmax)
	$w.guinierrange.up2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Guiniernpoints) \
			< ( $::StructParData(npoints)\
			    - $::StructParData(Guinierfirst)+1 )
		} {
			incr ::StructParData(Guiniernpoints)
			setStructParQval GuinierQmax
		}
		RefreshStructParFit
	}
	$w.guinierrange.down2 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Guiniernpoints) > 3 } { 
			incr ::StructParData(Guiniernpoints) -1
			setStructParQval GuinierQmax
		}
		RefreshStructParFit
	}
}

label $w.guinierrange.first -justify left 
entry $w.guinierrange.nfirst -relief sunken -width 6 -highlightthickness 0
bind  $w.guinierrange.nfirst <KeyPress-Return> { RefreshStructParFit}
ArrowButton $w.guinierrange.up1 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
ArrowButton $w.guinierrange.down1 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
grid $w.guinierrange.first $w.guinierrange.nfirst \
     $w.guinierrange.up1 $w.guinierrange.down1 \
     -row 3 -sticky w -padx 1m 

label $w.guinierrange.npoints -justify left
entry $w.guinierrange.nnpoints -relief sunken -width 6 -highlightthickness 0
bind  $w.guinierrange.nnpoints <KeyPress-Return> { RefreshStructParFit}
ArrowButton $w.guinierrange.up2 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
ArrowButton $w.guinierrange.down2 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
grid $w.guinierrange.npoints $w.guinierrange.nnpoints \
     $w.guinierrange.up2 $w.guinierrange.down2 \
     -row 4 -sticky w -padx 1m

checkbutton $w.guinierrange.showlin -variable StructParData(showlin) \
	-onvalue 1 -offvalue 0 \
	-text "show intermediate linear fit \n(green curve) and residuum" \
	-command \
	{
		verifyUserInput
		RefreshStructParFit
	}
grid $w.guinierrange.showlin -row 5 -columnspan 6 -sticky w -padx 1m
set StructParData(showlin) 0

label $w.porodrange.formula -justify left \
     -textvariable StructParData(largeQText)
grid $w.porodrange.formula -columnspan 4 -row 1 -sticky w -padx 1m

radiobutton $w.porodrange.bypointcount -text "point count" \
	-value 0 -variable ::StructParData(PorodByQRange) \
	-highlightthickness 0 -command { configPorodByPointCount }
radiobutton $w.porodrange.byqrange -text "Q range" \
	-value 1 -variable ::StructParData(PorodByQRange) \
	-highlightthickness 0 -command { configPorodByQRange }
grid $w.porodrange.bypointcount $w.porodrange.byqrange - - \
	-row 2 -sticky w -padx 1m

proc configPorodByPointCount {} {
	verifyUserInput
	set w .structural.user
	$w.porodrange.first configure -text "1st pt. (desc.):"
	$w.porodrange.nfirst configure \
		-textvariable ::StructParData(Porodfirst)
	$w.porodrange.up1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Porodfirst) \
			< ( $::StructParData(npoints)\
			   - $::StructParData(Porodnpoints)+1 )
		} {
			incr ::StructParData(Porodfirst)
		}
		RefreshStructParFit
	}
	$w.porodrange.down1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Porodfirst) > 1 } {
			incr ::StructParData(Porodfirst) -1
		}
		RefreshStructParFit
	}
	$w.porodrange.npoints configure -text "number of pts.:"
	$w.porodrange.nnpoints configure \
		-textvariable ::StructParData(Porodnpoints)
	$w.porodrange.up2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Porodnpoints) \
			< ( $::StructParData(npoints)\
			   - $::StructParData(Porodfirst)+1 )
		} {
			incr ::StructParData(Porodnpoints)
		}
		RefreshStructParFit
	}
	$w.porodrange.down2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Porodnpoints) > 3} {
			incr ::StructParData(Porodnpoints) -1
		}
		RefreshStructParFit
	}
}
proc configPorodByQRange {} {
	verifyUserInput
	set w .structural.user
	$w.porodrange.first configure -text "Q max:"
	$w.porodrange.nfirst configure \
		-textvariable ::StructParData(PorodQmax)
	$w.porodrange.up1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Porodfirst) > 1 } {
			incr ::StructParData(Porodfirst) -1
			incr ::StructParData(Porodnpoints) 1
			setStructParQval PorodQmax
		}
		RefreshStructParFit
	}
	$w.porodrange.down1 configure -armcommand {
		verifyUserInput
		if { $::StructParData(Porodfirst) \
			< ( $::StructParData(npoints)\
			   - $::StructParData(Porodnpoints)+1 )
		} {
			incr ::StructParData(Porodfirst)
			incr ::StructParData(Porodnpoints) -1
			setStructParQval PorodQmax
		}
		RefreshStructParFit
	}
	$w.porodrange.npoints configure -text "Q min:"
	$w.porodrange.nnpoints configure \
		-textvariable ::StructParData(PorodQmin)
	$w.porodrange.up2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Porodnpoints) > 3} {
			incr ::StructParData(Porodnpoints) -1
			setStructParQval PorodQmin
		}
		RefreshStructParFit
	}
	$w.porodrange.down2 configure -armcommand {
		verifyUserInput
		if {$::StructParData(Porodnpoints) \
			< ( $::StructParData(npoints)\
			   - $::StructParData(Porodfirst)+1 )
		} {
			incr ::StructParData(Porodnpoints)
			setStructParQval PorodQmin
		}
		RefreshStructParFit
	}
}

label $w.porodrange.first -justify left
entry $w.porodrange.nfirst -relief sunken -width 6 -highlightthickness 0
bind  $w.porodrange.nfirst <KeyPress-Return> { RefreshStructParFit}
ArrowButton $w.porodrange.up1 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 
ArrowButton $w.porodrange.down1 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
grid $w.porodrange.first $w.porodrange.nfirst \
     $w.porodrange.up1 $w.porodrange.down1 \
     -row 3 -sticky w -padx 1m

label $w.porodrange.npoints -justify left
entry $w.porodrange.nnpoints -relief sunken -width 6 -highlightthickness 0
bind  $w.porodrange.nnpoints <KeyPress-Return> { RefreshStructParFit}
ArrowButton $w.porodrange.up2 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
ArrowButton $w.porodrange.down2 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1
grid $w.porodrange.npoints $w.porodrange.nnpoints \
     $w.porodrange.up2 $w.porodrange.down2 \
     -row 4 -sticky w -padx 1m

set ::StructParData(GuinierByQRange) 0
set ::StructParData(PorodByQRange) 0
$w.guinierrange.bypointcount invoke
$w.porodrange.bypointcount invoke

eval "checkbutton $w.porodrange.porodDfit -offvalue no -onvalue yes \
     -variable ::StructParData(porodDfit) $::radio_check_button_prop \
     -highlightthickness 0 -text \"fit D:\" \
     -command { RefreshStructParFit }"
label $w.porodrange.porodD -justify left -text "D :"
entry $w.porodrange.nPorodD -textvariable StructParData(PorodD) \
      -relief sunken -width 4 -highlightthickness 0
bind  $w.porodrange.nPorodD <KeyPress-Return> { RefreshStructParFit}

ArrowButton $w.porodrange.up3 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {
		verifyUserInput
		if {$::StructParData(PorodD) <= (2.0-0.1)} {
			set ::StructParData(PorodD) \
				[expr $::StructParData(PorodD)+0.1]
		}
		RefreshStructParFit
	}
ArrowButton $w.porodrange.down3 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {
		verifyUserInput
		if {$::StructParData(PorodD) >= (-2.0+0.1)} {
			set ::StructParData(PorodD) \
				[expr $::StructParData(PorodD)-0.1]
		}
		RefreshStructParFit
	}
grid $w.porodrange.porodDfit $w.porodrange.nPorodD \
     $w.porodrange.up3 $w.porodrange.down3 \
     -row 5 -sticky w -padx 1m

eval "checkbutton $w.porodrange.c0fit -offvalue no -onvalue yes \
     -variable StructParData(c0fit) $::radio_check_button_prop \
     -text \"fit c0:\" -highlightthickness 0 \
     -command { RefreshStructParFit }"
label $w.porodrange.c0 -justify left \
     -text "c0 :"
entry $w.porodrange.nc0 -textvariable StructParData(c0) \
      -relief sunken -width 4 -highlightthickness 0
bind  $w.porodrange.nc0 <KeyPress-Return> { RefreshStructParFit}

ArrowButton $w.porodrange.up4 \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {
		verifyUserInput
		if {$::StructParData(c0) != 0.0} {
			set ::StructParData(c0) \
				[expr $::StructParData(c0)*1.1]
		}
		RefreshStructParFit
	}
ArrowButton $w.porodrange.down4 \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {
		verifyUserInput
		if {$::StructParData(c0) != 0.0} {
			set ::StructParData(c0) \
				[expr $::StructParData(c0)/1.1]
		}
		RefreshStructParFit
	}
grid $w.porodrange.c0fit $w.porodrange.nc0 \
     $w.porodrange.up4 $w.porodrange.down4 \
     -row 6 -sticky w -padx 1m

label $w.guinierresult.txt -justify left \
      -textvariable StructParData(lowQFit)
grid  $w.guinierresult.txt -columnspan 2 \
      -row 0 -sticky w -padx 1m
label $w.guinierresult.chisqlabel -justify right \
     -text "chisq ="
label $w.guinierresult.chisqval -justify left \
     -textvariable StructParData(I0chisq)
grid $w.guinierresult.chisqlabel -row 1 -column 0 -sticky e
grid $w.guinierresult.chisqval   -row 1 -column 1 -sticky w 
label $w.guinierresult.rglabel -justify right \
     -text "Rg ="
label $w.guinierresult.rgval -justify left \
     -textvariable StructParData(RG)

label $w.guinierresult.rgpm -justify right \
     -text "�"
label $w.guinierresult.drgval -justify left \
     -textvariable StructParData(DRG)

grid $w.guinierresult.rglabel -row 2 -column 0 -sticky e 
grid $w.guinierresult.rgval   -row 2 -column 1 -sticky w 
grid $w.guinierresult.rgpm    -row 2 -column 2 -sticky e 
grid $w.guinierresult.drgval  -row 2 -column 3 -sticky w 

label $w.guinierresult.i0label -justify right \
     -text "I0 ="
label $w.guinierresult.i0val -justify left \
     -textvariable StructParData(I0)
label $w.guinierresult.i0pm -justify right \
     -text "�"
label $w.guinierresult.di0val -justify left \
     -textvariable StructParData(DI0)

grid $w.guinierresult.i0label -row 3 -column 0 -sticky e 
grid $w.guinierresult.i0val   -row 3 -column 1 -sticky w 
grid $w.guinierresult.i0pm    -row 3 -column 2 -sticky e 
grid $w.guinierresult.di0val  -row 3 -column 3 -sticky w 

grid columnconfigure $w.guinierresult {0 1} -weight 1
grid rowconfigure    $w.guinierresult {0 1 2 3} -weight 1

label $w.porodresult.txt -justify left \
      -textvariable StructParData(largeQFit)
grid  $w.porodresult.txt -columnspan 2 \
      -row 0 -sticky w -padx 1m
label $w.porodresult.chisqlabel -justify right \
     -text "chisq ="
label $w.porodresult.chisqval -justify left \
     -textvariable StructParData(Porodchisq)
grid $w.porodresult.chisqlabel -row 1 -column 0 -sticky e
grid $w.porodresult.chisqval   -row 1 -column 1 -sticky w 
label $w.porodresult.c0label -justify right \
     -text "c0 ="
label $w.porodresult.c0val -justify left \
     -textvariable StructParData(c0)
label $w.porodresult.c0pm -justify right \
     -text "�"
label $w.porodresult.dc0val -justify left \
     -textvariable StructParData(Dc0)

grid $w.porodresult.c0label -row 2 -column 0 -sticky e 
grid $w.porodresult.c0val   -row 2 -column 1 -sticky w 
grid $w.porodresult.c0pm    -row 2 -column 2 -sticky e 
grid $w.porodresult.dc0val  -row 2 -column 3 -sticky w 


label $w.porodresult.c4label -justify right \
     -text "c4 ="
label $w.porodresult.c4val -justify left \
     -textvariable StructParData(c4)
label $w.porodresult.c4pm -justify right \
     -text "�"
label $w.porodresult.dc4val -justify left \
     -textvariable StructParData(Dc4)

grid $w.porodresult.c4label -row 3 -column 0 -sticky e 
grid $w.porodresult.c4val   -row 3 -column 1 -sticky w
grid $w.porodresult.c4pm    -row 3 -column 2 -sticky e 
grid $w.porodresult.dc4val  -row 3 -column 3 -sticky w

label $w.porodresult.porodDlabel -justify right \
     -text "D ="
label $w.porodresult.porodDval -justify left \
     -textvariable StructParData(PorodD)
label $w.porodresult.porodDpm -justify right \
     -text "�"
label $w.porodresult.dporodDval -justify left \
     -textvariable StructParData(DPorodD)

grid $w.porodresult.porodDlabel -row 4 -column 0 -sticky e 
grid $w.porodresult.porodDval   -row 4 -column 1 -sticky w 
grid $w.porodresult.porodDpm    -row 4 -column 2 -sticky e 
grid $w.porodresult.dporodDval  -row 4 -column 3 -sticky w 

grid columnconfigure $w.porodresult {0 1} -weight 1
grid rowconfigure    $w.porodresult {0 1 2 3 4} -weight 1


##################################################################
################ Fast Analysis of Data Series ####################
##################################################################

set outFile "isp.csv"
if {[info exists ::sasfit(lastProjectFile)]} {
	set fname [file rootname [file tail $::sasfit(lastProjectFile)]]
	set outFile "${fname}_$outFile"
}
set ::StructParData(series_outfile) $outFile
seriesInit ::StructParData $w.loadandsave
catch {RefreshStructParFit} {}



}

proc seriesInit { configarr widgetpath
} {
    upvar $configarr arr
    set arr(wls_wid) $widgetpath

proc seriesSetOutFile { configarr outFile
} {
	upvar $configarr arr
	if {[info exists arr(output_individual)]} {
		set fname [file tail $outFile]
		set dir [file dirname $outFile]
		set arr(series_outfile) [file join $dir "%s_$fname"]
	} else {
		set arr(series_outfile) $outFile
	}
}

    # init config
    if {![info exists arr(series_indir)] ||
        ![file isdirectory $arr(series_indir)]
    } {
        set arr(series_indir) "$::sasfit(datadir)"
    }
    if {![info exists arr(series_outfile)] ||
		[llength [file split $arr(series_outfile)]] <= 1
    } {
        set outFile [file join $arr(series_indir) $arr(series_outfile)]
		seriesSetOutFile $configarr $outFile
    }
    if {![info exists arr(series_ptrn)]
    } {
        set arr(series_ptrn) "*.*"
    }
    set arr(series_runnr) 0
    set arr(numCols) 8
    set arr(firstRow) 7

# helper functions
proc getIndir { structpar 
} {
	upvar $structpar spd
	set dir [tk_chooseDirectory -initialdir "$spd(series_indir)" \
		-parent $spd(wls_wid) -mustexist true \
		-title "Please select an input directory"]
    if {![file isdirectory $dir]} { return }
	set spd(series_indir) $dir
	$spd(wls_wid).ptrnNtr validate
	# update output file name
	set fname [file tail $spd(series_outfile)]
	set spd(series_outfile) [file join $dir $fname]
}
proc getOutFile { structpar 
} {
	upvar $structpar spd
	set types {{{Text Files (Semicolon Separated Values)} {.csv}} {{All Files} *}}
	set initialfile [string trim [file tail [format $spd(series_outfile) ""]]]
	set initialdir [file dirname $spd(series_outfile)]
	set windowtitle "Please select an output file"
	if {[info exists spd(output_individual)]} {
		set windowtitle "Please select an output file ending"
	}
	set outFile [tk_getSaveFile -initialdir "$initialdir" -parent $spd(wls_wid) \
		-filetypes $types -initialfile "$initialfile" -title $windowtitle]
    if {[string length $outFile] <= 0} { return }
	# separate output file for each data file
	seriesSetOutFile $structpar $outFile
}
proc processPattern { structpar ptrn
} {
	upvar $structpar spd
	set files [glob -directory "$spd(series_indir)" -nocomplain \
		-types {f r} -tails "$ptrn"]
	set files [lsort -dictionary $files]
	set spd(series_files) $files
	return 1
}
proc showFilePopup { w X Y x y
} {
	set sel [$w curselection]
	set pointedAt [lindex [$w index @$x,$y] 0]
	set isIn [lsearch -exact -integer $sel $pointedAt]
	if {$isIn < 0} {;# not found, select the new one
		$w selection clear 0 end
		$w selection set $pointedAt
	}
	tk_popup $w.popup $X $Y
}

proc removeFile { structpar } {
	upvar $structpar spd
	set lb $spd(wls_wid).fileBox
	set sel [$lb curselection]
	set newlist {}
	set first 0
	foreach idx [$lb curselection] {
		set last [expr $idx-1]
		if {$first <= $last} {
			set newlist [concat $newlist [lrange $spd(series_files) $first $last]]
		}
		set first [expr $idx+1]
	}
	set spd(series_files) [concat $newlist [lrange $spd(series_files) $first end]]
	$lb selection clear 0 end
}

proc updateFileSelection { configarr args } {
	upvar $configarr arr
	if {[llength $args] > 0} {
		set arr(series_runnr) [lindex $args 0]
	}
	$arr(wls_wid).fileBox selection clear 0 end
	$arr(wls_wid).fileBox selection set $arr(series_runnr)
}

proc seriesBuildWidgets { configarr wls
} {
    upvar $configarr arr
    button $wls.inDirBtn -text "select input directory:" -command "getIndir $configarr"
    grid   $wls.inDirBtn -row $arr(firstRow) -column 0 -sticky e -columnspan 2
    label  $wls.inDirLbl -textvariable [format "%s(series_indir)" $configarr]
    grid   $wls.inDirLbl -row $arr(firstRow) -column 2 -sticky w -columnspan $arr(numCols)

    label  $wls.ptrnLbl -text "enter a file selection pattern: "
    grid   $wls.ptrnLbl -row [expr $arr(firstRow)+1] -column 0 -sticky e -columnspan 2
    entry  $wls.ptrnNtr -textvariable [format "%s(series_ptrn)" $configarr] -width 10 \
        -justify right -validate key \
        -validatecommand "processPattern $configarr \"%P\""
    grid   $wls.ptrnNtr -row [expr $arr(firstRow)+1] -column 2 -sticky w
    $wls.ptrnNtr validate
    setTooltip $wls.ptrnNtr \
"The pattern may contain any of the following special characters:
?
    Matches any single character.
*
    Matches any sequence of zero or more characters.
\[chars\]
    Matches any single character in 'chars' (c,h,a,r and s independently).
    If 'chars' contains a sequence of the form 'a-b' then any character 
    between a and b (inclusive) will match. 
    '\[3-5\]' matches the digits 3,4 and 5
    '\[a-f\]' matches the characters a,b,c,d,e and f
\\x
    Matches the single character x. Use this to match special characters.
{ab,bcde,...}
    Matches any of the strings ab, bcde, etc. 

More Examples:
*
    Matches all files in the specified directory.
*.txt
    Matches files with extension 'txt'.
*{a,b,cde}*
    Matches all files whose name contains an 'a', a 'b' or 
    the sequence 'cde'."

    listbox $wls.fileBox -height 8 -yscrollcommand "$wls.fileScrY set" \
        -xscrollcommand "$wls.fileScrX set" \
        -selectmode extended \
        -listvariable [format "%s(series_files)" $configarr]
    scrollbar $wls.fileScrY -orient vertical -command "$wls.fileBox yview"
    grid $wls.fileBox -row [expr $arr(firstRow)+2] -sticky nsew -columnspan $arr(numCols)
    grid $wls.fileScrY -row [expr $arr(firstRow)+2] -column $arr(numCols) -sticky ns
    scrollbar $wls.fileScrX -orient horizontal -command "$wls.fileBox xview"
    grid $wls.fileScrX -row [expr $arr(firstRow)+3] -sticky ew -columnspan $arr(numCols)
    menu $wls.fileBox.popup -tearoff 0

proc popupCalculateCmd { configarr
} {
    upvar $configarr arr
    set arr(loadnextfail) 0
    set idx [lindex [$arr(wls_wid).fileBox curselection] 0]
    if {$idx < [llength $arr(series_files)]} { 
        set arr(series_runnr) $idx
    }
    $arr(wls_wid).loadnext invoke
}
    $wls.fileBox.popup add command -label "calculate" -command "popupCalculateCmd $configarr"
    $wls.fileBox.popup add command -label "remove" \
        -command "removeFile $configarr"
    bind $wls.fileBox <ButtonPress-3> "showFilePopup $wls.fileBox %X %Y %x %y"
    bind $wls.fileBox <Double-ButtonPress-1> "showFilePopup $wls.fileBox %X %Y %x %y"

    button $wls.storeFNLabel -text "select output file:" -command "getOutFile $configarr"
    grid   $wls.storeFNLabel -row [expr $arr(firstRow)+4] -column 0 -sticky e
    label  $wls.storeFNEntry -textvariable [format "%s(series_outfile)" $configarr]
    grid   $wls.storeFNEntry -row [expr $arr(firstRow)+4] -column 1 -columnspan $arr(numCols) -sticky w

    button $wls.loadnext -text "load next file" -command "seriesLoadNext $configarr"
    button $wls.storeISP -text "save fit result" -command "seriesSaveResult $configarr"
    button $wls.doall -text "Do all" -command "seriesDoAll $configarr"
    button $wls.resetISP -text "reset data file" -command "seriesResetResult $configarr"
    button $wls.interrupt -text "INTERRUPT" -command "seriesInterrupt $configarr" -bg red
	
    grid  $wls.loadnext -row [expr $arr(firstRow)+5] -column 0 -sticky e
	grid  $wls.storeISP -row [expr $arr(firstRow)+5] -column 1 -sticky e
	grid  $wls.resetISP -row [expr $arr(firstRow)+5] -column 2 -sticky e
	grid  $wls.doall -row [expr $arr(firstRow)+5] -column 3 -sticky e
	grid  $wls.interrupt -row [expr $arr(firstRow)+5] -column 4 -sticky e
#	grid  $wls.storeISP - $wls.resetISP - $wls.doall - $wls.interrupt \
#        -row [expr $arr(firstRow)+5] -sticky e
    grid columnconfigure $wls {0 1 2 3 4 5} -weight 1

    update

    set fboxHeight [winfo reqheight $wls.fileBox]
    set topParent [winfo toplevel $wls]
    set winHeight [winfo reqheight $topParent]
    set heightMargin [expr $winHeight - $fboxHeight ]
    set rowHeight [expr ($fboxHeight \
             - 2*[$wls.fileBox cget -borderwidth]) \
             / [$wls.fileBox cget -height]]

    set taglist [bindtags $topParent]
    lappend taglist ResizeTag
    bindtags $topParent $taglist
    bind ResizeTag <Configure> " resizeCmd $wls %h $heightMargin $rowHeight"
}

proc resizeCmd { widget height margin rowHeight } {
	if {$rowHeight < 1} { return }
	set avail [expr $height - $margin]
	set count [expr $avail / $rowHeight]
	if {$count < 1} { set count 1}
	$widget.fileBox configure -height $count
}

proc seriesLoadNext { configarr
} {
    upvar $configarr arr
    updateFileSelection $configarr
    set filename ""
    if {$arr(series_runnr) >= [llength $arr(series_files)]
    } {
        tk_messageBox -parent $arr(wls_wid) -icon info -message \
            "last file has been reached"
        #set filename [lindex $arr(prev_file) 0]
        return
    } else {
        set filename [file join $arr(series_indir) \
                [lindex $arr(series_files) \
                    $arr(series_runnr)]]
    }
    set arr(loadnextfail) 0
    if { ![info exists arr(seriesLoadCmd)]
    } {
        tk_messageBox -parent $arr(wls_wid) -icon error -message \
            "No load command defined!"
    }
    load_data_file $configarr $filename
    incr arr(series_runnr)
}

proc seriesInterrupt { configarr
} {
    upvar $configarr arr
	set arr(Interrupt) 1
	set ::EMOptions(Interrupt) 1
	$arr(wls_wid).doall configure -text "continue: Do all"
}

proc seriesSaveResult { configarr
} {
    upvar $configarr arr
    if { ![info exists arr(seriesSaveCmd)]
    } {
        tk_messageBox -parent $arr(wls_wid) -icon error -message \
            "No save command defined!"
        return
    }
    eval $arr(seriesSaveCmd) $configarr
}

proc seriesDoAll { configarr
} {
    upvar $configarr arr
    set arr(loadnextfail) 0
	set arr(Interrupt) 0
	set ::SASfitinterrupt 0
    while { ($arr(loadnextfail) == 0) && \
	        ($arr(Interrupt) == 0) && \
			($::SASfitinterrupt == 0) && \
            ($arr(series_runnr) < [llength $arr(series_files)])
    } {
        $arr(wls_wid).loadnext invoke
        if {$arr(loadnextfail) == 0} {
            $arr(wls_wid).storeISP invoke
        }
        update
    }
	if {($arr(series_runnr) == [llength $arr(series_files)])} {
		$arr(wls_wid).doall configure -text "finished: Do all"
	}
	if {$::SASfitinterrupt == 1} {
		$arr(wls_wid).doall configure -text "continue: Do all"
	}
}

proc seriesResetResult { configarr
} {
    upvar $configarr arr
    set fid [open $arr(series_outfile) w]
    close $fid
    updateFileSelection $configarr 0
	$arr(wls_wid).doall configure -text "Do all"
}

    seriesBuildWidgets $configarr $widgetpath
    return $widgetpath

}

proc structparSaveResult { configarr
} {
    upvar $configarr arr
    set fid [open $arr(series_outfile) a+]
    set c0 [structpar_get_data_descr $configarr]
    set c1 [structpar_get_data_value $configarr]
    set c2 [structpar_get_data_error $configarr]
    if {[file size $arr(series_outfile)] == 0} {
        set headerStr "runNr;filename"
        for {set i 0} {$i < [llength $c0]} {incr i
        } {
            append headerStr ";[lindex $c0 $i]"
            # if this value has an error
            if {[lindex [lindex $c1 $i] 1] >= 0} {
                append headerStr ";error"
            }
        }
        puts $fid $headerStr
    }
    set values [structpar_get_data_value $configarr]
    set errors [structpar_get_data_error $configarr]
    set dataStr [expr $arr(series_runnr)-1]
    set filename ""
    if {[info exists ::sasfit(file,name)]} { set filename [lindex $::sasfit(file,name) 0] }
    append dataStr ";$filename"
    foreach valpair $c1 {
        append dataStr ";[lindex $valpair 0]"
        set errIdx [lindex $valpair 1]
        if {$errIdx >= 0} {
            append dataStr ";[lindex $c2 $errIdx]"
        }
    }
    puts $fid $dataStr
    close $fid
}

proc RefreshStructParFit {} {
	global StructParData printer_vars
	global sasfit
	global IQGraph ResIQGraph SDGraph
	global I0extrapol Porod
	if { ! $sasfit(I_enable) } {
		tk_messageBox -parent .structural -icon warning -title Warning \
			-message "No data loaded"
		return -code return;	# required, otherwise BWidgets 
					# ArrowButton.armcommand throws an error
	}
	verifyUserInput
	set StructParData(order) descending
	set StructParData(typestr) Porod
	set StructParData(aname)   {c0 c1 c4 PorodD}
	set StructParData(active)  [list $StructParData(c0fit) no yes $StructParData(porodDfit)]
	set StructParData(a) [list $StructParData(c0) $StructParData(c1)     \
			      $StructParData(c4) $StructParData(PorodD) ]
	set ret [catch {sasfit_PorodFit StructParData \
		      [list $sasfit(Q) $sasfit(I) $sasfit(DI)]} Porod]
	if {$ret != 0} {
		tk_messageBox -parent .structural -icon error -title ERROR \
			-message "$Porod"
		return -code return
	}
	set StructParData(order) ascending
	set StructParData(typestr) Guinier
	set StructParData(aname)   {I0 RG2 "" ""}
	set StructParData(active)  {yes yes no no}
	set rg2 $StructParData(RG)
	if {![isnan $rg2]} {
		set rg2 [expr $StructParData(RG)*$StructParData(RG)]
	}
	set StructParData(a) [list $StructParData(I0) $rg2 "-1.0" "-1.0"]
	array unset StructParData guinier_lin*
	clearGraph_el ResIQGraph
	switch $StructParData(I0typestr) {
		"Guinier"  {
			set ret [catch {sasfit_GuinierFit StructParData \
				[list $sasfit(Q) $sasfit(I) $sasfit(DI)]} I0extrapol]
			if {$ret != 0} {
				tk_messageBox -parent .structural -icon error \ 
					-title ERROR -message "$I0extrapol"
				return -code return
			}
		}
		"Zimm"     {
			set ret [catch {sasfit_ZimmFit StructParData \
				[list $sasfit(Q) $sasfit(I) $sasfit(DI)]} I0extrapol]
			if {$ret != 0} {
				tk_messageBox -parent .structural -icon error \
					-title ERROR -message "$I0extrapol"
				return -code return
			}
		}
		"Debye"    {
			set ret [catch {sasfit_DebyeFit StructParData \
				[list $sasfit(Q) $sasfit(I) $sasfit(DI)]} I0extrapol]
			if {$ret != 0} {
				tk_messageBox -parent .structural -icon error \
					-title ERROR -message "$I0extrapol"
				return -code return
			}
		}
	}
	clearGraph_el IQGraph  
	
	Put_Graph_el IQGraph [lindex $Porod 0] [lindex $Porod 1]
	Put_Graph_el IQGraph [lindex $I0extrapol 0] [lindex $I0extrapol 1]
	Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $sasfit(DI)
	
	set IQGraph(e,symbol) [lreplace $IQGraph(e,symbol) 0 0 none]
	set IQGraph(e,symbol) [lreplace $IQGraph(e,symbol) 1 1 none]
	set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 0 0 1]
	set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 1 1 1]
	set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 2 2 0]
	
	set IQGraph(e,dashcolor) [lreplace $IQGraph(e,dashcolor) 0 0 magenta]
	set IQGraph(e,dashcolor) [lreplace $IQGraph(e,dashcolor) 1 1 red]


	set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) 2 2 \
				    [file tail $sasfit(filename)]]
	set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) 0 0 Porod Fit]
	set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) 1 1 \
				       "$StructParData(I0typestr) Fit"]

	# show intermediate linear approx if requested and available
	if {[llength [array names StructParData guinier_lin*]] &&
	    [info exists StructParData(showlin)] && $StructParData(showlin)
	} {
		clearGraph_el ResIQGraph
		Put_Graph_el ResIQGraph [lindex $StructParData(guinier_lin_data) 0] \
	                        [lindex $StructParData(guinier_lin_data) end]
		Put_Graph_el IQGraph [lindex $I0extrapol 0] [lindex $StructParData(guinier_lin_data) 2]
		set IQGraph(e,symbol) [lreplace $IQGraph(e,symbol) 3 3 none]
		set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 3 3 1]
		set IQGraph(e,dashcolor) [lreplace $IQGraph(e,dashcolor) 3 3 green]
	}

	set ret [catch {sasfit_StructParCalc StructParData \
	       [list $sasfit(Q) $sasfit(I) $sasfit(DI)]} msg]
	if {$ret != 0} {
		tk_messageBox -parent .structural -icon error \
			-title ERROR -message "$msg"
		return -code return
	}
	RefreshGraph IQGraph
	RefreshStructParDataTab
}

proc verifyUserInput {} {

	proc verifyQRange { name min max } {
		if {![info exists ::StructParData($name)] || 
		    ![string is double -strict $::StructParData($name)] ||
		    $::StructParData($name) < $min ||
		    $::StructParData($name) > $max
		} {
			setStructParQval $name
		}
	}
	proc verifyPointCount { name min max } {
		if {![info exists ::StructParData($name)] || 
		    ![string is digit -strict $::StructParData($name)] ||
		    $::StructParData($name) < $min ||
		    $::StructParData($name) > $max
		} {
			set ::StructParData($name) $min
		}
	}

	set ::StructParData(ndata) $::sasfit(npoints)
	set ::StructParData(npoints) $::sasfit(npoints)
	set len $::sasfit(npoints)
	set min [lindex $::sasfit(Q) 0]
	set max [lindex $::sasfit(Q) end]
	if { !$len } { 
		set min 0
		set max 0
	}

	# verify user input, guinier, by point count
	verifyPointCount Guinierfirst 1 [expr $len - 3]
	verifyPointCount Guiniernpoints 3 [expr $len - $::StructParData(Guinierfirst) + 1]

	# by q range
	verifyQRange GuinierQmin $min $max
	verifyQRange GuinierQmax $min $max

	# verify user input, porod
	verifyPointCount Porodfirst 1 [expr $len - 3]
	verifyPointCount Porodnpoints 3 [expr $len - $::StructParData(Porodfirst) + 1]

	# by q range
	verifyQRange PorodQmin $min $max
	verifyQRange PorodQmax $min $max

	if { $::StructParData(GuinierByQRange) } {
		set fidx 0
		set lidx 0
		foreach qval $::sasfit(Q) {
			if { $qval < $::StructParData(GuinierQmin) } {
				incr fidx
			}
			if { $qval < $::StructParData(GuinierQmax) } {
				incr lidx
			}
		}
		if {$fidx >= $len} { set fidx [expr $len - 1] }
		if {$lidx >= $len} { set lidx [expr $len - 1] }
		set ::StructParData(Guinierfirst) [expr $fidx + 1]
		set ::StructParData(Guiniernpoints) [expr $lidx - $fidx + 1]
	} else {
		setStructParQval GuinierQmin
		setStructParQval GuinierQmax
	}

	if { $::StructParData(PorodByQRange) } {
		set fidx 0
		set lidx 0
		foreach qval $::sasfit(Q) {
			if { $qval < $::StructParData(PorodQmin) } {
				incr fidx
			}
			if { $qval < $::StructParData(PorodQmax) } {
				incr lidx
			}
		}
		if {$fidx >= $len} { set fidx [expr $len - 1] }
		if {$lidx >= $len} { set lidx [expr $len - 1] }
		set ::StructParData(Porodfirst) [expr $len - $lidx]
		set ::StructParData(Porodnpoints) [expr $lidx - $fidx + 1]
	} else {
		setStructParQval PorodQmin
		setStructParQval PorodQmax
	}

	if {![info exists ::StructParData(PorodD)] || 
	    ![string is double -strict $::StructParData(PorodD)]
	} {
		set ::StructParData(PorodD) 0.0
	}
	if {![info exists ::StructParData(c0)] || 
	    ![string is double -strict $::StructParData(c0)]
	} {
		set ::StructParData(c0) 0.0
	}
	if { $::StructParData(PorodD) < -2.0} {set ::StructParData(PorodD) -2.0}
	if { $::StructParData(PorodD) >= 2.0} {set ::StructParData(PorodD) 1.99}
}
proc setStructParQval { name } {
	switch -- $name {
		GuinierQmax {
			set ::StructParData(GuinierQmax) \
				[lindex $::sasfit(Q) [expr \
				$::StructParData(Guinierfirst) + \
				$::StructParData(Guiniernpoints) - 2]]
		}
		GuinierQmin {
			set ::StructParData(GuinierQmin) \
				[lindex $::sasfit(Q) [expr \
				$::StructParData(Guinierfirst) - 1]]
		}
		PorodQmax {
			set ::StructParData(PorodQmax) \
				[lindex $::sasfit(Q) end-[expr \
				$::StructParData(Porodfirst) - 1]]
		}
		PorodQmin {
			set ::StructParData(PorodQmin) \
				[lindex $::sasfit(Q) end-[expr \
				($::StructParData(Porodfirst) + \
				 $::StructParData(Porodnpoints) - 2)]]
		}
	}
}

# vim: set ts=4 sw=4 tw=0:

