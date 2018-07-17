# sasfit.vfs/lib/app-sasfit/tcl/sasfit_customize.tcl
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

proc setIntStrategy2int {} {
	switch $::FitPrecision(IntStrategy) {
		"OOURA_DE" {set ::FitPrecision(IntStrategy_int) 0}
		"OOURA_CC" {set ::FitPrecision(IntStrategy_int) 1}
		"GSL_CQUAD" {set ::FitPrecision(IntStrategy_int) 2}
		"GSL_QAG" {set ::FitPrecision(IntStrategy_int) 3}
		"H_CUBATURE" {set ::FitPrecision(IntStrategy_int) 4}
		"P_CUBATURE" {set ::FitPrecision(IntStrategy_int) 5}
		"NR_QROMB" {set ::FitPrecision(IntStrategy_int) 6}
		default {set ::FitPrecision(IntStrategy_int) 0}
	}
}

#-------------------------------------------------------------------------
#   setting some parameters for the fitting routine
#
proc CustomizeCmd { analytpar tanalytpar } {

	global FitPrecision
	upvar $analytpar ap
	if { [winfo exists .customize] } {destroy .customize}
	toplevel .customize
	set w .customize
	wm geometry $w
	wm title $w "setting some values for the fitting routines"

	set entrywidth 8

	label $w.epsIQlabel -text "eps \[int N(R) F(Q,R)^2 S(Q) dR\] ="
	entry $w.epsIQvalue -textvariable FitPrecision(epsNRIQ) -width $entrywidth
	grid $w.epsIQlabel -row 0 -column 0 -sticky e
	grid $w.epsIQvalue -row 0 -column 1 -sticky w
	label $w.jmaxIQlabel -text "JMAX \[int N(R) F(Q,R)^2 S(Q) dR\] ="
	entry $w.jmaxIQvalue -textvariable FitPrecision(JMAXNRIQ) -width $entrywidth
	grid $w.jmaxIQlabel -row 0 -column 2 -sticky e
	grid $w.jmaxIQvalue -row 0 -column 3 -sticky w
	label $w.epsreslabel -text "eps \[int R_av(Q) I(Q) dQ\] ="
	entry $w.epsresvalue -textvariable FitPrecision(epsresolution) -width $entrywidth
	grid $w.epsreslabel -row 1 -column 0 -sticky e
	grid $w.epsresvalue -row 1 -column 1 -sticky w
	label $w.jmaxreslabel -text "JMAX \[int R_av(Q) I(Q) dQ\] ="
	entry $w.jmaxresvalue -textvariable FitPrecision(JMAXresolution) -width $entrywidth
	grid $w.jmaxreslabel -row 1 -column 2 -sticky e
	grid $w.jmaxresvalue -row 1 -column 3 -sticky w
	label $w.epsanisolabel -text "eps \[int p(x) I_aniso(Q) dx\] ="
	entry $w.epsanisovalue -textvariable FitPrecision(epsanisotropic) -width $entrywidth
	grid $w.epsanisolabel -row 2 -column 0 -sticky e
	grid $w.epsanisovalue -row 2 -column 1 -sticky w
	label $w.jmaxanisolabel -text "JMAX \[int p(x) I_aniso(Q) dx\] ="
	entry $w.jmaxanisovalue -textvariable FitPrecision(JMAXanisotropic) -width $entrywidth
	grid $w.jmaxanisolabel -row 2 -column 2 -sticky e
	grid $w.jmaxanisovalue -row 2 -column 3 -sticky w
	label $w.sqoriqlabel -text "if value < 0 calculate S~(Q) otherwise I(Q):"
	entry $w.sqoriqvalue -textvariable FitPrecision(SQ_or_IQ) -width $entrywidth
	grid $w.sqoriqlabel -row 3 -column 0 -sticky e
	grid $w.sqoriqvalue -row 3 -column 1 -sticky w
	label $w.robertus_plabel -text "number of classes for\nintegration of Robertus1\n value should be in \[3:15\]"
	entry $w.robertus_pvalue -textvariable FitPrecision(Robertus_p) -width $entrywidth
	grid $w.robertus_plabel -row 3 -column 2 -sticky e
	grid $w.robertus_pvalue -row 3 -column 3 -sticky w
	
	label $w.mc_plabel -text "number of iterations for\n Monte Carlo simulation"
	entry $w.mc_pvalue -textvariable FitPrecision(iter_4_MC) -width $entrywidth
	grid $w.mc_plabel -row 4 -column 0 -sticky e
	grid $w.mc_pvalue -row 4 -column 1 -sticky w

	label $w.adjust_text -text "parameter input increment: "
	entry $w.adjust_value -width $entrywidth \
		-textvariable [format %s%s $analytpar (par_x_X)]
	grid $w.adjust_text  -row 4 -column 2 -sticky e
	grid $w.adjust_value -row 4 -column 3 -sticky w
	scale $w.adjust_entry -showvalue no -highlightthickness 0 \
		-from -1 -to 2.9542425096 -resolution 0.000000001 \
		-orient horizontal \
		-command "update_parameter_increment $analytpar $tanalytpar"
	grid $w.adjust_entry -row 5 -column 2 -columnspan 2 -sticky ew
	$w.adjust_entry set [expr log10(100*$ap(par_x_X)-100)]
	
	label $w.intStrat_label -text "integration strategy"
	ComboBox $w.intStrat_value -values {"OOURA_DE" "OOURA_CC" "GSL_CQUAD" "GSL_QAG" \
										"H_CUBATURE" "P_CUBATURE" "NR_QROMB"} \
				-width 15 \
				-textvariable ::FitPrecision(IntStrategy) \
				-modifycmd setIntStrategy2int
	grid $w.intStrat_label -row 5 -column 0 -sticky e
	grid $w.intStrat_value -row 5 -column 1 -sticky w

}

proc update_parameter_increment { analytpar tanalytpar value } {
	upvar $analytpar ap
	upvar $tanalytpar tap
	set ap(par_x_X)  [expr (100+pow(10,$value))/100]
	set tap(par_x_X) $ap(par_x_X)
}

