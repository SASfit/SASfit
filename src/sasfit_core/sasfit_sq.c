/*
 * src/sasfit_core/sasfit_sq.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <string.h>
#include <sasfit_sq.h>
#include <sasfit_peaks.h>
#include "include/sasfit_core.h"

scalar sasfit_sq(scalar q, scalar * a, sasfit_function * sq, int * dparam, int * err)
{
	scalar res;
	int dpar = 0;
	/*
	float h = sasfit_eps_get_h();
	float param_neg, param_pos, param_len, param_orig;
	float* param;
	*/

	if( !sasfit_valid_fct(sq) )
	{
		sasfit_err("SQ is not valid !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}

   	if (SASFIT_EQUAL(a[0], 0.0)) a[0] = sasfit_eps_get_nriq(); 
	// the structure factors are often not defined for a[0]=0 (division by zero)
	// However, this limiting case is needed for scaling approx, 
	// local monodisperse approx., partical structure factors, etc..
	// As long as this limiting case is not treated in the struture factor functions 
	// itself this very poor work around above is required.

	// copy the supplied parameters
	sasfit_copy_param( &(sq->params), a);

	if( sq->params.errStatus != TCL_OK )
	{
		sasfit_err("Supplied parameters are erroneous !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
   
	if (dparam) dpar = dparam[SQfct];
//	sasfit_out("SQ: %g %g %g %g %d",sq->params.p[0],sq->params.p[1],sq->params.p[2],sq->params.p[3],dpar);

	res = sasfit_part_diff_fct(q, sq, dpar);
//	sasfit_out(" %lg\n", res);

	if ( sq->params.errStatus != TCL_OK ) 
	{
		sasfit_err("#structure factor: can not calculate \n"
		           "value of %s diff %d\n%s\n",sq->typestr, 
		           dpar, sq->params.errStr);
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
	return res;
}

int sasfit_sq_init(sasfit_analytpar * ap)
{
//	int i;

	sasfit_function * sq = 0;

	SASFIT_ASSERT_VAL(ap, TCL_ERROR);

	// assuming we get an already initialized (valid) sasfit_function
	// (initialized during init of according AP, earlier, 
	// see SASFIT_x_tcl.c and sasfit_init_fct() in sasfit_common)
	sq = &(ap->SQ);	// get size distrib function pointer

	// try to determine the function by specified plugin function ID
	if ( sasfit_function_select_id(sq) == TCL_OK )
	{
		return TCL_OK;
	}

	// select the functions
	if ( strcmp(sq->typestr, "RPA") == 0 )
	{
		sq->fct = sasfit_sq_None;
		sq->F_identifier = SQ_id_RPA;
	} else
	if ( strcmp(sq->typestr, "None") == 0 )
	{
		sq->fct = sasfit_sq_None;
		sq->F_identifier = SQ_id_None;
	} else
	if ( strcmp(sq->typestr, "Hard Sphere") == 0 )
	{
		sq->fct = sasfit_sq_HardSphere;
	} else
	if ( strcmp(sq->typestr, "Correlation Hole") == 0 )
	{
		sq->fct = sasfit_sq_CorrelationHole;
	} else
	if ( strcmp(sq->typestr, "Critical Scattering") == 0 )
	{
		sq->fct = sasfit_sq_CriticalScattering1;
	} else
	if ( strcmp(sq->typestr, "Cylinder(PRISM)") == 0 )
	{
		sq->fct = sasfit_sq_Cylinder_PRISM;
	} else
	if ( strcmp(sq->typestr, "Sticky Hard Sphere") == 0 )
	{
		sq->fct = sasfit_sq_StickyHardSphere;
	} else
	if ( strcmp(sq->typestr, "Sticky Hard Sphere 2") == 0 )
	{
		sq->fct = sasfit_sq_StickyHardSphere2;
	} else
	if ( strcmp(sq->typestr, "Mass Fractal (Exp Cut-Off)") == 0 )
	{
		sq->fct = sasfit_sq_MassFractal;
	} else
	if ( strcmp(sq->typestr, "Mass Fractal (Gaussian Cut-Off)") == 0 )
	{
		sq->fct = sasfit_sq_MassFractalGaussianCutOff;
	} else
	if ( strcmp(sq->typestr, "Mass Fractal (OverlapSph Cut-Off)") == 0 )
	{
		sq->fct = sasfit_sq_MassFractOverlapSph;
	} else
	if ( strcmp(sq->typestr, "Mass Fractal (Exp(-x^a) Cut-Off)") == 0 )
	{
		sq->fct = sasfit_sq_MassFractExp_mpow_x_a;
	} else
	if ( strcmp(sq->typestr, "Square Well Potential") == 0 )
	{
		sq->fct = sasfit_sq_SquareWell;
	} else
	if ( strcmp(sq->typestr, "Thin Square Well Potential") == 0 )
	{
		sq->fct = sasfit_sq_ThinSquareWell;
	} else
	if ( strcmp(sq->typestr, "Square Well Potential 2") == 0 )
	{
		sq->fct = sasfit_sq_SquareWell2;
	} else
	if ( strcmp(sq->typestr, "ThermalDisorder") == 0 )
	{
		sq->fct = sasfit_sq_TD;
	} else
	if ( strcmp(sq->typestr, "Paracrystalline") == 0 )
	{
		sq->fct = sasfit_sq_PT;
	} else
	if ( strcmp(sq->typestr, "ModifiedCaille") == 0 )
	{
		sq->fct = sasfit_sq_MCT;
	} else
	if ( strcmp(sq->typestr, "GaussianAmplitude") == 0 )
	{
		sq->fct = sasfit_peak_GaussianAmplitude;
	} else
	if ( strcmp(sq->typestr, "LorentzianAmplitude") == 0 )
	{
		sq->fct = sasfit_peak_LorentzianAmplitude;
	} else 
	if ( strcmp(sq->typestr, "QENS_ConfinementWithGaussianPotential") == 0 )
	{
		sq->fct = sasfit_peak_QENS_ConfinementWithGaussianPotential;
	} else
	if ( strcmp(sq->typestr, "VoigtPeak") == 0 )
	{
		sq->fct = sasfit_peak_VoigtPeakArea;
	} else
	if ( strcmp(sq->typestr, "PearsonVIIAmplitude") == 0 )
	{
		sq->fct = sasfit_peak_PearsonVIIAmplitude;
	} else
	if ( strcmp(sq->typestr, "ExpModifiedGaussian") == 0 )
	{
		sq->fct = sasfit_peak_ExponentiallyModifiedGaussianArea;
	} else
	if ( strcmp(sq->typestr, "HalfGaussianModifiedGaussian") == 0 )
	{
		sq->fct = sasfit_peak_HalfGaussianModifiedGaussianArea;
	} else
	if ( strcmp(sq->typestr, "Giddings") == 0 )
	{
		sq->fct = sasfit_peak_GiddingsArea;
	} else
	if ( strcmp(sq->typestr, "LogNormal4Parameters(Area)") == 0 )
	{
		sq->fct = sasfit_peak_LogNormal4ParameterArea;
	} else
	if ( strcmp(sq->typestr, "Haarhoff-Van der Linde") == 0 )
	{
		sq->fct = sasfit_peak_HaarhoffVanderLindeArea;
	} else
	if ( strcmp(sq->typestr, "Macro Ion (HP)") == 0 )
	{
		sq->fct = sasfit_sq_MacroIon;
	} else
	if ( strcmp(sq->typestr, "Hayter Penfold RMSA") == 0 )
	{
		sq->fct = sasfit_sq_HayterPenfoldMSA;
	} else
	if ( strcmp(sq->typestr, "BabaAhmed") == 0 )
	{
		sq->fct = sasfit_sq_BabaAhmed;
	} else
	if ( strcmp(sq->typestr, "BabaAhmed2") == 0 )
	{
		sq->fct = sasfit_sq_BabaAhmed2;
	} else
	if ( strcmp(sq->typestr, "LocalOrder") == 0 )
	{
		sq->fct = sasfit_sq_LocalOrderModel;
	} else
	if ( strcmp(sq->typestr, "RandomDistribution") == 0 )
	{
		sq->fct = sasfit_sq_RandomDistributionModel;
	} else
	if ( strcmp(sq->typestr, "P'(Q):ThinSphericalShell") == 0 )
	{
		sq->fct = sasfit_sq_ThinSphericalShell;
	} else
	if ( strcmp(sq->typestr, "P'(Q):Rod") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_rod;
	} else
	if ( strcmp(sq->typestr, "P'(Q):Worm(PS1)") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_WormPS1;
	} else
	if ( strcmp(sq->typestr, "P'(Q):Worm(PS2)") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_WormPS2;
	} else
	if ( strcmp(sq->typestr, "P'(Q):Worm(PS3)") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_WormPS3;
	} else
	if ( strcmp(sq->typestr, "P'(Q):KholodenkoWorm") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_KholodenkoWorm;
	} else
	if ( strcmp(sq->typestr, "P'(Q):WormLikeChain_withoutEXV") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_WormLikeChainGaussian;
	} else
	if ( strcmp(sq->typestr, "P'(Q):WormLikeChain_withEXV") == 0 )
	{
		sq->fct = sasfit_sq_Pprime_WormLikeChainEXV;
	} else
	{
		// unknown structure factor
		sasfit_param_set_err(&sq->params, "Unknown structure factor !");
		return TCL_ERROR;
	}
	return TCL_OK;
}

