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

proc setSphAvgStrategy2int {} {
	puts $::FitPrecision(SphAvgStrategy)
	switch $::FitPrecision(SphAvgStrategy) {
		"GSL_2D_GAUSSLEGENDRE" {set ::FitPrecision(SphAvgStrategy_int) 0}
		"Lebedev"  {set ::FitPrecision(SphAvgStrategy_int) 1}
		"FIBONACCI" {set ::FitPrecision(SphAvgStrategy_int) 2}
		"H_CUBATURE" {set ::FitPrecision(SphAvgStrategy_int) 3}
		"P_CUBATURE" {set ::FitPrecision(SphAvgStrategy_int) 4}
		"MC_MISER" {set ::FitPrecision(SphAvgStrategy_int) 5}
		"MC_VEGAS" {set ::FitPrecision(SphAvgStrategy_int) 6}
		"MC_PLAIN" {set ::FitPrecision(SphAvgStrategy_int) 7}
		"spherical_t_design"  {set ::FitPrecision(SphAvgStrategy_int) 8}
		"OOURA_DE" {set ::FitPrecision(SphAvgStrategy_int) 9}
		"OOURA_CC" {set ::FitPrecision(SphAvgStrategy_int) 10}
		"GSL_CQUAD" {set ::FitPrecision(SphAvgStrategy_int) 11}
		"TANHSINH_1" {set ::FitPrecision(SphAvgStrategy_int) 12}
		"TANHSINH_2" {set ::FitPrecision(SphAvgStrategy_int) 13}
		"QMC_NIEDERREITER_2" {set ::FitPrecision(SphAvgStrategy_int) 14}
		"QMC_SOBOL" {set ::FitPrecision(SphAvgStrategy_int) 15}
		"QMC_HALTON" {set ::FitPrecision(SphAvgStrategy_int) 16}
		"QMC_REVERSEHALTON" {set ::FitPrecision(SphAvgStrategy_int) 17}
		"RQMC_SOBOL_RDS" {set ::FitPrecision(SphAvgStrategy_int) 18}
		"RQMC_SOBOL_OWEN" {set ::FitPrecision(SphAvgStrategy_int) 19}
		"RQMC_FAURE05_OWEN" {set ::FitPrecision(SphAvgStrategy_int) 20}
		"RQMC_LAINE_KARRAS" {set ::FitPrecision(SphAvgStrategy_int) 21}
		default {set ::FitPrecision(SphAvgStrategy_int) 0}
	}
}
proc setCubatureStrategy2int {} {
	puts $::FitPrecision(CubatureStrategy)
	switch $::FitPrecision(CubatureStrategy) {
		"H_CUBATURE" {set ::FitPrecision(SphAvgStrategy_int) 0}
		"P_CUBATURE" {set ::FitPrecision(SphAvgStrategy_int) 1}
		"MC_MISER" {set ::FitPrecision(SphAvgStrategy_int) 2}
		"MC_VEGAS" {set ::FitPrecision(SphAvgStrategy_int) 3}
		"MC_PLAIN" {set ::FitPrecision(SphAvgStrategy_int) 4}
		default {set ::FitPrecision(SphAvgStrategy_int) 0}
	}
}

proc setIntStrategy2int {} {
	switch $::FitPrecision(IntStrategy) {
		"OOURA_DE" {set ::FitPrecision(IntStrategy_int) 0}
		"OOURA_CC" {set ::FitPrecision(IntStrategy_int) 1}
		"GSL_CQUAD" {set ::FitPrecision(IntStrategy_int) 2}
		"GSL_QAG" {set ::FitPrecision(IntStrategy_int) 3}
		"H_CUBATURE" {set ::FitPrecision(IntStrategy_int) 4}
		"P_CUBATURE" {set ::FitPrecision(IntStrategy_int) 5}
		"NR_QROMB" {set ::FitPrecision(IntStrategy_int) 6}
		"GSL_QNG" {set ::FitPrecision(IntStrategy_int) 10}
		"GSL_GAUSSLEGENDRE" {set ::FitPrecision(IntStrategy_int) 12}
		"GSL_CHEBYSHEV1" {set ::FitPrecision(IntStrategy_int) 13}
		"GSL_CHEBYSHEV2" {set ::FitPrecision(IntStrategy_int) 14}
		"GSL_GEGENBAUER" {set ::FitPrecision(IntStrategy_int) 15}
		"GSL_EXPONENTIAL" {set ::FitPrecision(IntStrategy_int) 16}
		"GSL_LAGUERRE" {set ::FitPrecision(IntStrategy_int) 17}
		"GSL_JACOBI" {set ::FitPrecision(IntStrategy_int) 18}
		"MC_MISER" {set ::FitPrecision(IntStrategy_int) 19}
		"MC_VEGAS" {set ::FitPrecision(IntStrategy_int) 20}
		"MC_PLAIN" {set ::FitPrecision(IntStrategy_int) 21}
		"TANHSINH_1" {set ::FitPrecision(IntStrategy_int) 22}
		"TANHSINH_2" {set ::FitPrecision(IntStrategy_int) 23}
		"QMC_NIEDERREITER_2" {set ::FitPrecision(IntStrategy_int) 24}
		"QMC_SOBOL" {set ::FitPrecision(IntStrategy_int) 25}
		"QMC_HALTON" {set ::FitPrecision(IntStrategy_int) 26}
		"QMC_REVERSEHALTON" {set ::FitPrecision(IntStrategy_int) 27}
		"RQMC_SOBOL_RDS" {set ::FitPrecision(IntStrategy_int) 28}
		"RQMC_SOBOL_OWEN" {set ::FitPrecision(IntStrategy_int) 29}
		"RQMC_FAURE05_OWEN" {set ::FitPrecision(IntStrategy_int) 30}
		"RQMC_LAINE_KARRAS" {set ::FitPrecision(IntStrategy_int) 31}
		default {set ::FitPrecision(IntStrategy_int) 0}
	}
}


proc setHankelStrategy2int {} {
	puts $::FitPrecision(HankelStrategy)
	switch $::FitPrecision(HankelStrategy) {
		"OOURA_DEO" 	{set ::FitPrecision(HankelStrategy_int) 0}
		"OGATA_2005"	{set ::FitPrecision(HankelStrategy_int) 1}
		"FBT0" 			{set ::FitPrecision(HankelStrategy_int) 2}
		"FBT1" 			{set ::FitPrecision(HankelStrategy_int) 3}
		"FBT2" 			{set ::FitPrecision(HankelStrategy_int) 4}
		"GSL_QAWF"		{set ::FitPrecision(HankelStrategy_int) 5}
		"GUPTASARMA_97"			{set ::FitPrecision(HankelStrategy_int) 6}
		"GUPTASARMA_97_FAST"	{set ::FitPrecision(HankelStrategy_int) 7}
		"KEY_51"		{set ::FitPrecision(HankelStrategy_int) 8}
		"KEY_101"		{set ::FitPrecision(HankelStrategy_int) 9}
		"KEY_201"		{set ::FitPrecision(HankelStrategy_int) 10}
		"ANDERSON_801"	{set ::FitPrecision(HankelStrategy_int) 11}
		"QWE"			{set ::FitPrecision(HankelStrategy_int) 12}
		"CHAVE"			{set ::FitPrecision(HankelStrategy_int) 13}
		default 		{set ::FitPrecision(HankelStrategy_int) 0}
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
	grid $w.adjust_entry -row 7 -column 0 -columnspan 4 -sticky ew
	$w.adjust_entry set [expr log10(100*$ap(par_x_X)-100)]
	
	label $w.intStrat_label -text "integration strategy"
	ComboBox $w.intStrat_value -values {"OOURA_DE" "OOURA_CC" "TANHSINH_1" "TANHSINH_2" \
										"GSL_CQUAD" "GSL_QAG" "GSL_QNG"\
										"H_CUBATURE" "P_CUBATURE" "NR_QROMB" \
										"GSL_GAUSSLEGENDRE" "GSL_CHEBYSHEV1" "GSL_CHEBYSHEV2"\
										"GSL_GEGENBAUER" "GSL_EXPONENTIAL" "GSL_JACOBI"\
										"MC_MISER" "MC_VEGAS" "MC_PLAIN" "QMC_NIEDERREITER_2" \
										"QMC_SOBOL" "QMC_HALTON" "QMC_REVERSEHALTON" "RQMC_SOBOL_RDS" \
										"RQMC_SOBOL_OWEN" "RQMC_FAURE05_OWEN" "RQMC_LAINE_KARRAS"} \
				-width 15 \
				-textvariable ::FitPrecision(IntStrategy) \
				-modifycmd setIntStrategy2int
	grid $w.intStrat_label -row 5 -column 0 -sticky e
	grid $w.intStrat_value -row 5 -column 1 -sticky w
    label $w.sphavgStrat_label -text "spherical average strategy"
	ComboBox $w.sphavgStrat_value -values {"GSL_2D_GAUSSLEGENDRE" "Lebedev" "FIBONACCI"\
										"H_CUBATURE" "P_CUBATURE" "MC_MISER" "MC_VEGAS" "MC_PLAIN" \
										"spherical_t_design" "OOURA_DE" "OOURA_CC" "GSL_CQUAD" \
										"TANHSINH_1" "TANHSINH_2" "QMC_NIEDERREITER_2" \
										"QMC_SOBOL" "QMC_HALTON" "QMC_REVERSEHALTON" "RQMC_SOBOL_RDS" \
										"RQMC_SOBOL_OWEN" "RQMC_FAURE05_OWEN" "RQMC_LAINE_KARRAS"} \
				-width 15 \
				-textvariable ::FitPrecision(SphAvgStrategy) \
				-modifycmd setSphAvgStrategy2int
	grid $w.sphavgStrat_label -row 5 -column 2 -sticky e
	grid $w.sphavgStrat_value -row 5 -column 3 -sticky w
	
	label $w.hankelStrat_label -text "Hankel transform strategy"
	ComboBox $w.hankelStrat_value -values {"OOURA_DEO" "OGATA_2005" "FBT0" "FBT1" "FBT2" "GSL_QAWF" "GUPTASARMA_97_FAST" "GUPTASARMA_97" "KEY_51" "KEY_101" "KEY_201" "ANDERSON_801" "QWE"  "CHAVE"} \
				-width 15 \
				-textvariable ::FitPrecision(HankelStrategy) \
				-modifycmd setHankelStrategy2int
	grid $w.hankelStrat_label -row 6 -column 0 -sticky e
	grid $w.hankelStrat_value -row 6 -column 1 -sticky w
	
	label $w.gsl_GL_label -text "GSL_GAUSSLEGENDRE points:"
	entry $w.gsl_GL_value -textvariable FitPrecision(GSL_GAUSSLEGENDRE) -width $entrywidth
	grid $w.gsl_GL_label -row 8 -column 0 -sticky e
	grid $w.gsl_GL_value -row 8 -column 1 -sticky w
	label $w.gsl_C1_label -text "GSL_CHEBYSHEV1 points:"
	entry $w.gsl_C1_value -textvariable FitPrecision(GSL_CHEBYSHEV1) -width $entrywidth
	grid $w.gsl_C1_label -row 9 -column 0 -sticky e
	grid $w.gsl_C1_value -row 9 -column 1 -sticky w
	label $w.gsl_C2_label -text "GSL_CHEBYSHEV2 points:"
	entry $w.gsl_C2_value -textvariable FitPrecision(GSL_CHEBYSHEV2) -width $entrywidth
	grid $w.gsl_C2_label -row 10 -column 0 -sticky e
	grid $w.gsl_C2_value -row 10 -column 1 -sticky w
	label $w.gsl_GEGB_label -text "GSL_GEGENBAUER points:"
	entry $w.gsl_GEGB_value -textvariable FitPrecision(GSL_GEGENBAUER) -width $entrywidth
	grid $w.gsl_GEGB_label -row 8 -column 2 -sticky e
	grid $w.gsl_GEGB_value -row 8 -column 3 -sticky w
	label $w.gsl_EXP_label -text "GSL_EXPONENTIAL points:"
	entry $w.gsl_EXP_value -textvariable FitPrecision(GSL_EXPONENTIAL) -width $entrywidth
	grid $w.gsl_EXP_label -row 9 -column 2 -sticky e
	grid $w.gsl_EXP_value -row 9 -column 3 -sticky w
#	label $w.gsl_LAG_label -text "GSL_LAGUERRE points:"
#	entry $w.gsl_LAG_value -textvariable FitPrecision(GSL_LAGUERRE) -width $entrywidth
#	grid $w.gsl_LAG_label -row 10 -column 2 -sticky e
#	grid $w.gsl_LAG_value -row 10 -column 3 -sticky w
	label $w.gsl_JAC_label -text "GSL_JACOBI points:"
	entry $w.gsl_JAC_value -textvariable FitPrecision(GSL_JACOBI) -width $entrywidth
	grid $w.gsl_JAC_label -row 10 -column 2 -sticky e
	grid $w.gsl_JAC_value -row 10 -column 3 -sticky w
	label $w.gsl_ALPHA_label -text "GSL_ALPHA parameter:"
	entry $w.gsl_ALPHA_value -textvariable FitPrecision(GSL_ALPHA) -width $entrywidth
	grid $w.gsl_ALPHA_label -row 11 -column 0 -sticky e
	grid $w.gsl_ALPHA_value -row 11 -column 1 -sticky w
	label $w.gsl_BETA_label -text "GSL_BETA parameter:"
	entry $w.gsl_BETA_value -textvariable FitPrecision(GSL_BETA) -width $entrywidth
	grid $w.gsl_BETA_label -row 11 -column 2 -sticky e
	grid $w.gsl_BETA_value -row 11 -column 3 -sticky w
	label $w.lebedev_label -text "Lebedev order \[1,65\]:"
	entry $w.lebedev_value -textvariable FitPrecision(Lebedev) -width $entrywidth
	grid $w.lebedev_label -row 12 -column 0 -sticky e
	grid $w.lebedev_value -row 12 -column 1 -sticky w
	label $w.finonacci_label -text "FIBONACCI points:"
	entry $w.gibonacci_value -textvariable FitPrecision(FIBONACCI) -width $entrywidth
	grid $w.finonacci_label -row 12 -column 2 -sticky e
	grid $w.gibonacci_value -row 12 -column 3 -sticky w
	label $w.sphericaltdesign_label -text "spherical-t design, order \[1,136\]:"
	entry $w.sphericaltdesign_value -textvariable FitPrecision(spherical_t_design) -width $entrywidth
	grid $w.sphericaltdesign_label -row 13 -column 0 -sticky e
	grid $w.sphericaltdesign_value -row 13 -column 1 -sticky w
	
	label $w.ogata_2005_N_label -text "N_Ogata >= 2 \[default: 50\]"
	entry $w.ogata_2005_N_value -textvariable FitPrecision(N_Ogata) -width $entrywidth
	grid $w.ogata_2005_N_label -row 14 -column 0 -sticky e
	grid $w.ogata_2005_N_value -row 14 -column 1 -sticky w
	
	label $w.ogata_2005_h_label -text "h_Ogata \[default: 0.01\]:"
	entry $w.ogata_2005_h_value -textvariable FitPrecision(h_Ogata) -width $entrywidth
	grid $w.ogata_2005_h_label -row 14 -column 2 -sticky e
	grid $w.ogata_2005_h_value -row 14 -column 3 -sticky w
}

proc update_parameter_increment { analytpar tanalytpar value } {
	upvar $analytpar ap
	upvar $tanalytpar tap
	set ap(par_x_X)  [expr (100+pow(10,$value))/100]
	set tap(par_x_X) $ap(par_x_X)
}

