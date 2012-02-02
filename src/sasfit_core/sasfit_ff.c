/*
 * src/sasfit_core/sasfit_ff.c
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
#include <sasfit_ff.h>
#include <sasfit_peaks.h>
#include "include/sasfit_core.h"

scalar sasfit_ff(scalar q, scalar * a, sasfit_function * ff, int * dparam, int * err)
{
	scalar res;
	int dpar = 0;

	if( !sasfit_valid_fct(ff) )
	{
		sasfit_err("FF is not valid !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}

	// copy the supplied parameters
	sasfit_copy_param( &(ff->params), a);

	if( ff->params.errStatus != TCL_OK )
	{
		sasfit_err("Supplied parameters are erroneous !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
   
	if (dparam) dpar = dparam[FFfct];
//	printf("FF: %g %g %g %g %d",ff->params.p[0],ff->params.p[1],ff->params.p[2],ff->params.p[3],dpar);

	res = sasfit_part_diff_fct(q, ff, dpar);
//	printf(", res: %lg\n", res);

	if ( ff->params.errStatus != TCL_OK ) 
	{
		sasfit_err("#form factor: can not calculate \n"
		           "value of %s diff %d\n%s\n",ff->typestr, 
		           dpar, ff->params.errStr);
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
	return res;
}

scalar sasfit_volume(scalar x, scalar * l, sasfit_function * ff, int distr, int * err)
{
	scalar res;

	if( !sasfit_valid_fct(ff) )
	{
		sasfit_err("FF is not valid !\n");
		if(err) *err = TCL_ERROR;
		return 0.0;
	}

	// copy the supplied parameters
	sasfit_copy_param( &(ff->params), l);

	// no volume function defined
	if ( !(ff->fct_v) ) return 0.0;

	res = (ff->fct_v)(x, &(ff->params), distr);

	if ( ff->params.errStatus != TCL_OK ) 
	{
		sasfit_err("#volume function: Could not calculate >%s<\n",ff->typestr);
		if(err) *err = TCL_ERROR;
		return 0.0;
	}
	return res;
}

int sasfit_ff_init(sasfit_analytpar * ap)
{
//	int i;

	sasfit_function* ff = 0;

	SASFIT_ASSERT_VAL(ap, FALSE);

	// assuming we get an already initialized (valid) sasfit_function
	// (initialized during init of according AP, earlier, 
	// see SASFIT_x_tcl.c and sasfit_init_fct() in sasfit_common)
	ff = &(ap->FF);	// get size distrib function pointer

	// try to determine the function by specified plugin function ID
	if ( sasfit_function_select_id(ff) == TCL_OK )
	{
		return TCL_OK;
	}

	// select the functions

	if ( strcmp(ff->typestr, "MultiLamellarVesicle") == 0 )
	{
		ff->fct = sasfit_ff_MultiLamellarVesicle;
	} else
	if ( strcmp(ff->typestr, "RNDMultiLamellarVesicle") == 0 )
	{
		ff->fct = sasfit_ff_RNDMultiLamellarVesicle;
	} else
	if ( strcmp(ff->typestr, "RNDMultiLamellarVesicle2") == 0 )
	{
		ff->fct = sasfit_ff_RNDMultiLamellarVesicle2;
	} else
	if ( strcmp(ff->typestr, "MLVesicleFrielinghaus") == 0 )
	{
		ff->fct = sasfit_ff_MLVesicleFrielinghaus;
	} else
	if ( strcmp(ff->typestr, "TripleLayeredXS_ULV") == 0 )
	{
		ff->fct = sasfit_ff_TripleLayeredHomogeneousCentroSymmetricXS;
	} else
	if ( strcmp(ff->typestr, "SphereWithGaussChains") == 0 )
	{
		ff->fct = sasfit_ff_SphereWithGaussChains;
	} else
	if ( strcmp(ff->typestr, "Torus") == 0 )
	{
		ff->fct = sasfit_ff_Torus;
	} else
	if ( strcmp(ff->typestr, "BiLayeredVesicle") == 0 )
	{
		ff->fct = sasfit_ff_BiLayeredVesicle;
		ff->fct_v = sasfit_ff_BiLayeredVesicle_v;
	} else
	if ( strcmp(ff->typestr, "Robertus1") == 0 )
	{
		ff->fct = sasfit_ff_Robertus1;
	} else
	if ( strcmp(ff->typestr, "Robertus2") == 0 )
	{
		ff->fct = sasfit_ff_Robertus2;
	} else
	if ( strcmp(ff->typestr, "Robertus3") == 0 )
	{
		ff->fct = sasfit_ff_Robertus3;
	} else
	if ( strcmp(ff->typestr, "MagneticShellAniso") == 0 )
	{
		ff->fct = sasfit_ff_magnetic_shell_aniso;
	} else
	if ( strcmp(ff->typestr, "MagneticShellCrossTerm") == 0 )
	{
		ff->fct = sasfit_ff_magnetic_shell_cross;
	} else
	if ( strcmp(ff->typestr, "MagneticShellPsi") == 0 )
	{
		ff->fct = sasfit_ff_magnetic_shell_psi;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboTISANE1") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_tisane;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsi") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsi2") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo2;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQ") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQBt1") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_bt1;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQLx") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_lx;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQL2x") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_l2x;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQL2x_SANSPOL") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_l2x_pol;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQL2x_SANSPOL_albr") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_l2x_pol_albr;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQL2x_SANSPOL_albr_stat") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_l2x_pol_albr_stat;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQL2modx_SANSPOL") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_sq_l2modx_pol;
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQp_90") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_ppsi;
		ff->params.p[10] = 90.0; // psi == 90
	} else
	if ( strcmp(ff->typestr, "SuperParStroboPsiSQp_0") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_strobo_ppsi;
		ff->params.p[10] = 0.0; // psi == 0
	} else
	if ( strcmp(ff->typestr, "hysteresesStroboPsi") == 0 )
	{
		ff->fct = sasfit_ff_hysteresesFFpsiStrobo;
	} else
	if ( strcmp(ff->typestr, "SuperparamagneticFFpsi") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi;
	} else
	if ( strcmp(ff->typestr, "SuperparamagneticFFAniso") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_aniso;
	} else
	if ( strcmp(ff->typestr, "SuperparamagneticFFCrossTerm") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_cross;
	} else
	if ( strcmp(ff->typestr, "SuperparamagneticFFIso") == 0 )
	{
		ff->fct = sasfit_ff_superparamagnetic_ff_psi_iso;
	} else
	if ( strcmp(ff->typestr, "Andrea1") == 0 )
	{
		ff->fct = sasfit_ff_Andrea1;
	} else
	if ( strcmp(ff->typestr, "DoubleShell_withSD") == 0 )
	{
		ff->fct = sasfit_ff_DoubleShell_withSD;
	} else
	if ( strcmp(ff->typestr, "StackDiscs") == 0 )
	{
		ff->fct = sasfit_ff_StackDiscs;
	} else
	if ( strcmp(ff->typestr, "Ellipsoid i") == 0 )
	{
		ff->fct = sasfit_ff_ellip_1;
		ff->fct_v = sasfit_ff_ellip_v;
		ff->params.kernelSelector = ELLIP1;
	} else
	if ( strcmp(ff->typestr, "Ellipsoid ii") == 0 )
	{
		ff->fct = sasfit_ff_ellip_2;
		ff->fct_v = sasfit_ff_ellip_v;
		ff->params.kernelSelector = ELLIP2;
	} else
	if ( strcmp(ff->typestr, "ellCylShell1") == 0 )
	{
		ff->fct = sasfit_ff_ellCylShell1;
		ff->fct_v = sasfit_ff_ellCylShell1_v;
		ff->fct_f = sasfit_ff_ellCylShell1_f;
	} else
	if ( strcmp(ff->typestr, "ellCylShell2") == 0 )
	{
		ff->fct = sasfit_ff_ellCylShell2;
		ff->fct_v = sasfit_ff_ellCylShell2_v;
		ff->fct_f = sasfit_ff_ellCylShell2_f;
	} else
	if ( strcmp(ff->typestr, "Cylinder") == 0 )
	{
		ff->fct = sasfit_ff_Cylinder;
		ff->fct_v = sasfit_ff_porod_cyl_v;
	} else
	if ( strcmp(ff->typestr, "PorodCylinder") == 0 )
	{
		ff->fct = sasfit_ff_porod_cyl;
		ff->fct_v = sasfit_ff_porod_cyl_v;
	} else
	if ( strcmp(ff->typestr, "LongCylinder") == 0 )
	{
		ff->fct = sasfit_ff_long_cyl;
		ff->fct_v = sasfit_ff_porod_cyl_v;
	} else
	if ( strcmp(ff->typestr, "FlatCylinder") == 0 )
	{
		ff->fct = sasfit_ff_flat_cyl;
		ff->fct_v = sasfit_ff_porod_cyl_v;
	} else
	if ( strcmp(ff->typestr, "HardSphere") == 0 )
	{
		ff->fct 	= sasfit_ff_HardSphere;
		ff->fct_v 	= sasfit_ff_sphere_v;
	} else
	if ( strcmp(ff->typestr, "StickyHardSphere") == 0 )
	{
		ff->fct 	= sasfit_ff_StickyHardSphere;
		ff->fct_v 	= sasfit_ff_sphere_v;
	} else
	if ( strcmp(ff->typestr, "Mass Fractal (Exp Cut-Off)") == 0 )
	{
		ff->fct = sasfit_ff_Fractal;
	} else
	if ( strcmp(ff->typestr, "Aggregate (Exp(-x^a) Cut-Off)") == 0 )
	{
		ff->fct = sasfit_ff_MassFractExp_mpow_x_a;
	} else
	if ( strcmp(ff->typestr, "Fisher-Burford") == 0 )
	{
		ff->fct = sasfit_ff_FisherBurford;
	} else
	if ( strcmp(ff->typestr, "DLCAggregation") == 0 )
	{
		ff->fct = sasfit_ff_DLCAggregation;
	} else
	if ( strcmp(ff->typestr, "RLCAggregation") == 0 )
	{
		ff->fct = sasfit_ff_RLCAggregation;
	} else
	if ( strcmp(ff->typestr, "MassFractExp") == 0 )
	{
		ff->fct = sasfit_ff_MassFractExp;
	} else
	if ( strcmp(ff->typestr, "MassFractGauss") == 0 )
	{
		ff->fct = sasfit_ff_MassFractGauss;
	} else
	if ( strcmp(ff->typestr, "Aggregate (OverlapSph Cut-Off)") == 0 )
	{
		ff->fct = sasfit_ff_MassFractOverlappingSpheres;
	} else
	if ( strcmp(ff->typestr, "WormLikeChainEXV") == 0 )
	{
		ff->fct = sasfit_ff_WormLikeChainEXV;
	} else
	if ( strcmp(ff->typestr, "KholodenkoWorm") == 0 )
	{
		ff->fct = sasfit_ff_KholodenkoWorm;
	} else
	if ( strcmp(ff->typestr, "LangevinMH") == 0 )
	{
		ff->fct = sasfit_ff_LangevinMH;
		ff->params.kernelSelector = LANGEVIN1;
	} else
	if ( strcmp(ff->typestr, "LangevinMH2") == 0 )
	{
		ff->fct = sasfit_ff_LangevinMH;
		ff->params.kernelSelector = LANGEVIN2;
	} else
	if ( strcmp(ff->typestr, "DLS_Sphere_RDG") == 0 )
	{
		ff->fct = sasfit_ff_DLS_Sphere_RDG;
	} else
	if ( strcmp(ff->typestr, "MieSphere") == 0 )
	{
		ff->fct = sasfit_ff_Mie;
		ff->fct_v = sasfit_ff_sphere_v;
	} else
	if ( strcmp(ff->typestr, "MieShell") == 0 )
	{
		ff->fct = sasfit_ff_MieShell;
		ff->fct_v = sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = SPH_SHELL3; // only for volume function
	} else
	if ( strcmp(ff->typestr, "confinement with Gaussian potential") == 0 )
	{
		ff->fct = sasfit_peak_QENS_ConfinementWithGaussianPotential;
	} else
	if ( strcmp(ff->typestr, "LinShell") == 0 )
	{
		ff->fct 	= sasfit_ff_LinShell;
		ff->fct_f 	= sasfit_ff_Kshlin;
		ff->fct_v 	= sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = KSHLIN1;
	} else
	if ( strcmp(ff->typestr, "LinShell2") == 0 )
	{
		ff->fct 	= sasfit_ff_LinShell;
		ff->fct_f 	= sasfit_ff_Kshlin;
		ff->fct_v 	= sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = KSHLIN2;
	} else
	if ( strcmp(ff->typestr, "P39") == 0 )
	{
		ff->fct = sasfit_ff_P39;
	} else
	if ( strcmp(ff->typestr, "SphereShell_compr") == 0 )
	{
		ff->fct 	= sasfit_ff_Shellcompr;
		ff->fct_f 	= sasfit_ff_KShellcompr;
		ff->fct_v 	= sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = KSHLIN1; // for volume function
	} else
	if ( strcmp(ff->typestr, "DumbbellShell") == 0 )
	{
		ff->fct = sasfit_ff_dumbbell_shell;
	} else
	if ( strcmp(ff->typestr, "two_attached_spheres") == 0 )
	{
		ff->fct 	= sasfit_ff_two_attached_spheres;
		ff->fct_f 	= sasfit_ff_two_attached_spheres_f;
	} else
	if ( strcmp(ff->typestr, "DoubleShellChain") == 0 )
	{
		ff->fct = sasfit_ff_DoubleShellChain;
	} else
	if ( strcmp(ff->typestr, "TetrahedronDoubleShell") == 0 )
	{
		ff->fct = sasfit_ff_TetrahedronDoubleShell;
	} else
	if ( strcmp(ff->typestr, "ShearedCylinder") == 0 )
	{
		ff->fct = sasfit_ff_shearedCylinder;
		ff->params.kernelSelector = CYL_SHRD;
	} else
	if ( strcmp(ff->typestr, "ShearedCylGauss") == 0 )
	{
		ff->fct = sasfit_ff_shearedCylinder;
		ff->params.kernelSelector = CYL_SHRD_GAUSS;
	} else
	if ( strcmp(ff->typestr, "BenoitStar") == 0 )
	{
		ff->fct = sasfit_ff_BenoitStar;
	} else
	if ( strcmp(ff->typestr, "PolydisperseStar") == 0 )
	{
		ff->fct = sasfit_ff_PolydisperseStar;
	} else
	if ( strcmp(ff->typestr, "DozierStar") == 0 )
	{
		ff->fct = sasfit_ff_dozier_star;
	} else
	if ( strcmp(ff->typestr, "DozierStar2") == 0 )
	{
		ff->fct = sasfit_ff_dozier_star_2;
	} else
	if ( strcmp(ff->typestr, "BeacaugeExpPowLaw2") == 0 )
	{
		ff->fct = sasfit_ff_UnifiedExponetialPowerLaw2;
	} else
	if ( strcmp(ff->typestr, "BeacaugeExpPowLaw") == 0 )
	{
		ff->fct = sasfit_ff_UnifiedExponetialPowerLaw;
	} else
	if ( strcmp(ff->typestr, "JuelichMicelle") == 0 )
	{
		ff->fct = sasfit_ff_JuelichDoubleShell;
	} else
	if ( strcmp(ff->typestr, "BlockCopolymerMicelle") == 0 )
	{
		ff->fct = sasfit_ff_BlockCopolymerMicelle;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_RWbrush;
		ff->params.kernelSelector = SPHERE_RWBRUSH;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(RW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_RWbrush;
		ff->params.kernelSelector = SPHERE_RWBRUSH_RC;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(RW)_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_RWbrush;
		ff->params.kernelSelector = SPHERE_RWBRUSH_NAGG;
	} else
	if (strcmp(ff->typestr, "CopolymerMicelleRod") == 0 )
	{
		ff->fct = sasfit_ff_Rodlike_BlockCopolymerMicelle;
	} else
	if ( strcmp(ff->typestr, "CopolymerMicelleEll") == 0 )
	{
		ff->fct = sasfit_ff_ellip_rwbrush_nagg;
	} else
	if ( strcmp(ff->typestr, "ELL+Chains(RW)_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_ellip_rwbrush_nagg;
	} else
	if ( strcmp(ff->typestr, "ELL+Chains(RW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_ellip_rwbrush_rc;
	} else
	if ( strcmp(ff->typestr, "ELL+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_ellip_rwbrush;
	} else
	if ( strcmp(ff->typestr, "CYL+Chains(RW)_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_cyl_rwbrush_nagg;
	} else
	if ( strcmp(ff->typestr, "CYL+Chains(RW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_cyl_rwbrush_rc;
	} else
	if ( strcmp(ff->typestr, "CYL+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_cyl_rwbrush;
	} else
	if ( strcmp(ff->typestr, "DISC+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_disc_rwbrush;
	} else
	if ( strcmp(ff->typestr, "DISC+Chains(RW)_Lc") == 0 )
	{
		ff->fct = sasfit_ff_disc_rwbrush_lc;
	} else
	if ( strcmp(ff->typestr, "DISC+Chains(RW)_nagg") == 0 )
	{
		ff->fct = sasfit_ff_disc_rwbrush_nagg;
	} else
	if ( strcmp(ff->typestr, "ROD+Chains(RW)_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Rod_RWbrush;
		ff->params.kernelSelector = ROD_RWBRUSH_NAGG;
	} else
	if ( strcmp(ff->typestr, "ROD+Chains(RW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Rod_RWbrush;
		ff->params.kernelSelector = ROD_RWBRUSH_RC;
	} else
	if ( strcmp(ff->typestr, "ROD+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_Rod_RWbrush;
		ff->params.kernelSelector = ROD_RWBRUSH;
	} else
	if ( strcmp(ff->typestr, "WORM+Chains(RW)_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Worm_RWbrush;
		ff->params.kernelSelector = WORM_RWBRUSH_NAGG;
	} else
	if ( strcmp(ff->typestr, "WORM+Chains(RW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Worm_RWbrush;
		ff->params.kernelSelector = WORM_RWBRUSH_RC;
	} else
	if ( strcmp(ff->typestr, "WORM+Chains(RW)") == 0 )
	{
		ff->fct = sasfit_ff_Worm_RWbrush;
		ff->params.kernelSelector = WORM_RWBRUSH;
	} else
	if ( strcmp(ff->typestr, "WORM+R^-a_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Worm_R_ma_Profile;
		ff->params.kernelSelector = WORM_RMA_NAGG;
	} else
	if ( strcmp(ff->typestr, "WORM+R^-a_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Worm_R_ma_Profile;
		ff->params.kernelSelector = WORM_RMA_RC;
	} else
	if ( strcmp(ff->typestr, "WORM+R^-a") == 0 )
	{
		ff->fct = sasfit_ff_Worm_R_ma_Profile;
		ff->params.kernelSelector = WORM_RMA;
	} else
	if ( strcmp(ff->typestr, "ROD+R^-a_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Rod_R_ma_Profile;
		ff->fct_v = sasfit_ff_Rod_R_ma_Profile_v;
		ff->params.kernelSelector = ROD_RMA;
	} else
	if ( strcmp(ff->typestr, "ROD+R^-a_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Rod_R_ma_Profile;
		ff->fct_v = sasfit_ff_Rod_R_ma_Profile_v;
		ff->params.kernelSelector = ROD_RMA_RC;
	} else
	if ( strcmp(ff->typestr, "ROD+R^-a") == 0 )
	{
		ff->fct = sasfit_ff_Rod_R_ma_Profile;
		ff->fct_v = sasfit_ff_Rod_R_ma_Profile_v;
		ff->params.kernelSelector = ROD_RMA;
	} else
	if ( strcmp(ff->typestr, "ROD+Gauss_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Gauss_Profile;
		ff->params.kernelSelector = ROD_GAUSS_NAGG;
	} else
	if ( strcmp(ff->typestr, "ROD+Gauss_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Gauss_Profile;
		ff->params.kernelSelector = ROD_GAUSS_RC;
	} else
	if ( strcmp(ff->typestr, "ROD+Gauss") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Gauss_Profile;
		ff->params.kernelSelector = ROD_GAUSS;
	} else
	if ( strcmp(ff->typestr, "ROD+Exp_nagg") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Exp_Profile;
		ff->params.kernelSelector = ROD_EXP_NAGG;
	} else
	if ( strcmp(ff->typestr, "ROD+Exp_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Exp_Profile;
		ff->params.kernelSelector = ROD_EXP_RC;
	} else
	if ( strcmp(ff->typestr, "ROD+Exp") == 0 )
	{
		ff->fct = sasfit_ff_Rod_Exp_Profile;
		ff->params.kernelSelector = ROD_EXP;
	} else
	if ( strcmp(ff->typestr, "CopolymerMicelleCyl") == 0 )
	{
		ff->fct = sasfit_ff_cyl_mic;
	} else
	if ( strcmp(ff->typestr, "Francois1") == 0 )
	{
		ff->fct = sasfit_ff_francois;
	} else
	if ( strcmp(ff->typestr, "SPHERE+R^-a") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile;
		ff->fct_v = sasfit_ff_Sphere_R_ma_Profile_v;
		ff->params.kernelSelector = SPHERE_RMA;
	} else
	if ( strcmp(ff->typestr, "SPHERE+R^-a_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile;
		ff->fct_v = sasfit_ff_Sphere_R_ma_Profile_v;
		ff->params.kernelSelector = SPHERE_RMA_RC;
	} else
	if ( strcmp(ff->typestr, "SPHERE_smooth_interface+R^-a_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile;
		ff->params.kernelSelector = SPHERE_RMA_RC_SMOOTH;
	} else
	if ( strcmp(ff->typestr, "SPHERE_smooth_interface+R^-a_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile;
		ff->params.kernelSelector = SPHERE_RMA_NAGG_SMOOTH;
	} else
	if ( strcmp(ff->typestr, "SPHERE+R^-a_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile;
		ff->fct_v = sasfit_ff_Sphere_R_ma_Profile_v;
		ff->params.kernelSelector = SPHERE_RMA_NAGG;
	} else
	if ( strcmp(ff->typestr, "CopolymerMicelleR^-a") == 0 )
	{
		ff->fct = sasfit_ff_CoPolymerMicelleSphericalProfile;
	} else
	if ( strcmp(ff->typestr, "Simon1") == 0 )
	{
		ff->fct = sasfit_ff_SphellWithGaussChainsSimon1;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(SAW)") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_SAWbrush;
		ff->params.kernelSelector = SPHERE_SAW;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(SAW)_Nagg") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_SAWbrush;
		ff->params.kernelSelector = SPHERE_SAW_NAGG;
	} else
	if ( strcmp(ff->typestr, "SPHERE+Chains(SAW)_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_SAWbrush;
		ff->params.kernelSelector = SPHERE_SAW_RC;
	} else
	if ( strcmp(ff->typestr, "SPHERE+R^-a_Manuela") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile_Manuela;
		ff->params.kernelSelector = SPHERE_RMA_MANU1;
	} else
	if ( strcmp(ff->typestr, "SPHERE+R^-a_Manuela2") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_R_ma_Profile_Manuela;
		ff->params.kernelSelector = SPHERE_RMA_MANU2;
	} else
	/*
	if ( strcmp(ff->typestr, "Rod") == 0 )
	{
		ff->fct = sasfit_ff_rod;
	} else
	*/
	if ( strcmp(ff->typestr, "CylShell1") == 0 )
	{
		ff->fct = sasfit_ff_cyl_shell_1;
		ff->fct_f = sasfit_ff_cyl_shell_1_f;
		ff->fct_v = sasfit_ff_cyl_shell_v;
	} else
	if ( strcmp(ff->typestr, "CylShell2") == 0 )
	{
		ff->fct = sasfit_ff_cyl_shell_2;
		ff->fct_f = sasfit_ff_cyl_shell_2_f;
		ff->fct_v = sasfit_ff_cyl_shell_v;
	} else
	if ( strcmp(ff->typestr, "LongCylShell") == 0 )
	{
		ff->fct = sasfit_ff_VeryLongCylindricalShell;
		ff->fct_v = sasfit_ff_cyl_shell_v;
	} else
	if ( strcmp(ff->typestr, "alignedCylShell") == 0 )
	{
		ff->fct = sasfit_ff_alignedCylShell;
		ff->fct_v = sasfit_ff_cyl_shell_v;
	} else
	if ( strcmp(ff->typestr, "partly aligned CylShell") == 0 )
	{
		ff->fct = sasfit_ff_partly_aligned_CylShell;
		ff->fct_v = sasfit_ff_cyl_shell_v;
	} else
	if ( strcmp(ff->typestr, "Disc") == 0 )
	{
		ff->fct = sasfit_ff_Disc;
	} else
	if ( strcmp(ff->typestr, "homogenousXS") == 0 )
	{
		ff->fct = sasfit_ff_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "TwoInfinitelyThinPlates") == 0 )
	{
		ff->fct = sasfit_ff_TwoInfinitelyThinPlates;
	} else
	if ( strcmp(ff->typestr, "LayeredCentroSymmetricXS") == 0 )
	{
		ff->fct = sasfit_ff_LayeredCentroSymmetricXS;
	} else
	if ( strcmp(ff->typestr, "BiLayerGauss") == 0 )
	{
		ff->fct = sasfit_ff_BiLayerGauss;
	} else
	if ( strcmp(ff->typestr, "SphSh+SD+homoXS") == 0 )
	{
		ff->fct = sasfit_ff_ThinSphShell_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "EllSh+SD+homoXS") == 0 )
	{
		ff->fct = sasfit_ff_ThinEllShell_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "EllSh+SD+homoXS(S)") == 0 )
	{
		ff->fct = sasfit_ff_ThinEllShell_S_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "CylSh+SD+homoXS") == 0 )
	{
		ff->fct = sasfit_ff_ThinCylShell_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "Disc+homoXS") == 0 )
	{
		ff->fct = sasfit_ff_ThinDisc_homogenousXS;
	} else
	if ( strcmp(ff->typestr, "OrnsteinZernike") == 0 )
	{
		ff->fct = sasfit_ff_OrnsteinZernike;
	} else
	if ( strcmp(ff->typestr, "BroadPeak") == 0 )
	{
		ff->fct = sasfit_ff_BroadPeak;
	} else
	if ( strcmp(ff->typestr, "DAB") == 0 )
	{
		ff->fct = sasfit_ff_DAB;
	} else
	if ( strcmp(ff->typestr, "TeubnerStrey") == 0 )
	{
		ff->fct = sasfit_ff_TeubnerStrey;
	} else
	if ( strcmp(ff->typestr, "Spinodal") == 0 )
	{
		ff->fct = sasfit_ff_Spinodal;
	} else
	if ( strcmp(ff->typestr, "Gauss") == 0 )
	{
		ff->fct = sasfit_ff_gauss_1;
	} else
	if ( strcmp(ff->typestr, "Gauss2") == 0 )
	{
		ff->fct = sasfit_ff_gauss_2;
	} else
	if ( strcmp(ff->typestr, "Gauss3") == 0 )
	{
		ff->fct = sasfit_ff_gauss_3;
	} else
	if ( strcmp(ff->typestr, "GaussPoly") == 0 )
	{
		ff->fct = sasfit_ff_gauss_poly;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian coil") == 0 )
	{
		ff->fct = sasfit_ff_gauss_generalized_1;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian coil 2") == 0 )
	{
		ff->fct = sasfit_ff_gauss_generalized_2;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian coil 3") == 0 )
	{
		ff->fct = sasfit_ff_gauss_generalized_3;
	} else
	if ( strcmp(ff->typestr, "FlexibleRingPolymer") == 0 )
	{
		ff->fct = sasfit_ff_FlexibleRingPolymer;
	} else
	if ( strcmp(ff->typestr, "mMemberedTwistedRing") == 0 )
	{
		ff->fct = sasfit_ff_mMemberedTwistedRing;
	} else
	if ( strcmp(ff->typestr, "DaisyLikeRing") == 0 )
	{
		ff->fct = sasfit_ff_DaisyLikeRing;
	} else
	if ( strcmp(ff->typestr, "A(B)") == 0 )
	{
		ff->fct = sasfit_ff_a;
	} else
	if ( strcmp(ff->typestr, "B(B)") == 0 )
	{
		ff->fct = sasfit_ff_b;
	} else
	if ( strcmp(ff->typestr, "EllipsoidalCoreShell") == 0 )
	{
		ff->fct 	= sasfit_ff_ellip_core_shell;
		ff->fct_f 	= sasfit_ff_ellip_core_shell_f;
		ff->fct_v 	= sasfit_ff_ellip_core_shell_v;
	} else
	if ( strcmp(ff->typestr, "triaxEllShell1") == 0 )
	{
		ff->fct = sasfit_ff_triaxEllShell1;
		ff->fct_v = sasfit_ff_triaxEllShell1_v;
	} else
	if ( strcmp(ff->typestr, "ExpShell") == 0 )
	{
		ff->fct 	= sasfit_ff_ExpShell;
		ff->fct_f 	= sasfit_ff_KshExp;
		ff->fct_v 	= sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = KSHLIN1; // for volume function
	} else
	if ( strcmp(ff->typestr, "SPHERE+Exp_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_Exp_Profile_Rc;
	} else
	if ( strcmp(ff->typestr, "Background") == 0 )
	{
		ff->fct = sasfit_ff_background;
	} else
	if ( strcmp(ff->typestr, "Guinier") == 0 )
	{
		ff->fct = sasfit_ff_guinier;
	} else
	if ( strcmp(ff->typestr, "Sphere") == 0 )
	{
		ff->fct 	= sasfit_ff_sphere;
		ff->fct_f 	= sasfit_ff_sphere_f;
		ff->fct_v 	= sasfit_ff_sphere_v;
	} else
	if ( strcmp(ff->typestr, "Spherical Shell i") == 0 )
	{
		ff->fct = sasfit_ff_spherical_shell;
		ff->fct_v = sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = SPH_SHELL1;
	} else
	if ( strcmp(ff->typestr, "Spherical Shell ii") == 0 )
	{
		ff->fct = sasfit_ff_spherical_shell;
		ff->fct_v = sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = SPH_SHELL2;
	} else
	if ( strcmp(ff->typestr, "Spherical Shell iii") == 0 )
	{
		ff->fct = sasfit_ff_spherical_shell;
		ff->fct_v = sasfit_ff_spherical_shell_v;
		ff->params.kernelSelector = SPH_SHELL3;
	} else
	if ( strcmp(ff->typestr, "Beta (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_BetaAmplitude;
	} else
	if ( strcmp(ff->typestr, "Beta (Area)") == 0 )
	{
		ff->fct = sasfit_peak_BetaArea;
	} else
	if ( strcmp(ff->typestr, "Chi-squared (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_ChiSquaredAmplitude;
	} else
	if ( strcmp(ff->typestr, "Chi-squared (Area)") == 0 )
	{
		ff->fct = sasfit_peak_ChiSquaredArea;
	} else
	if ( strcmp(ff->typestr, "Erfc (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_ErfcPeakAmplitude;
	} else
	if ( strcmp(ff->typestr, "Erfc (Area)") == 0 )
	{
		ff->fct = sasfit_peak_ErfcPeakArea;
	} else
	if ( strcmp(ff->typestr, "Error (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_ErrorAmplitude;
	} else
	if ( strcmp(ff->typestr, "Error (Area)") == 0 )
	{
		ff->fct = sasfit_peak_ErrorArea;
	} else
	if ( strcmp(ff->typestr, "exponentially modified Gaussian (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_ExponentiallyModifiedGaussianAmplitude;
	} else
	if ( strcmp(ff->typestr, "exponentially modified Gaussian (Area)") == 0 )
	{
		ff->fct = sasfit_peak_ExponentiallyModifiedGaussianArea;
	} else
	if ( strcmp(ff->typestr, "Extreme Value (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_ExtremeValueAmplitude;
	} else
	if ( strcmp(ff->typestr, "Extreme Value (Area)") == 0 )
	{
		ff->fct = sasfit_peak_ExtremeValueArea;
	} else
	if ( strcmp(ff->typestr, "fatique life (Area)") == 0 )
	{
		ff->fct = sasfit_peak_fatique_lifeArea;
	} else
	if ( strcmp(ff->typestr, "F-variance (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_FVarianceAmplitude;
	} else
	if ( strcmp(ff->typestr, "F-variance (Area)") == 0 )
	{
		ff->fct = sasfit_peak_FVarianceArea;
	} else
	if ( strcmp(ff->typestr, "Gamma (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GammaAmplitude;
	} else
	if ( strcmp(ff->typestr, "Gamma (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GammaArea;
	} else
	if ( strcmp(ff->typestr, "Gaussian (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianAmplitude;
	} else
	if ( strcmp(ff->typestr, "Gaussian (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianArea;
	} else
	if ( strcmp(ff->typestr, "Gaussian-Lorentzian cross product (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianLorentzianCrossProductAmplitude;
	} else
	if ( strcmp(ff->typestr, "Gaussian-Lorentzian cross product (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianLorentzianCrossProductArea;
	} else
	if ( strcmp(ff->typestr, "Gaussian-Lorentzian sum (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianLorentzianSumAmplitude;
	} else
	if ( strcmp(ff->typestr, "Gaussian-Lorentzian sum (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GaussianLorentzianSumArea;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian 1 (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GeneralizedGaussian1Amplitude;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian 1 (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GeneralizedGaussian1Area;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian 2 (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GeneralizedGaussian2Amplitude;
	} else
	if ( strcmp(ff->typestr, "generalized Gaussian 2 (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GeneralizedGaussian2Area;
	} else
	if ( strcmp(ff->typestr, "Giddings (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_GiddingsAmplitude;
	} else
	if ( strcmp(ff->typestr, "Giddings (Area)") == 0 )
	{
		ff->fct = sasfit_peak_GiddingsArea;
	} else
	if ( strcmp(ff->typestr, "Haarhoff Van der Linde (Area)") == 0 )
	{
		ff->fct = sasfit_peak_HaarhoffVanderLindeArea;
	} else
	if ( strcmp(ff->typestr, "half Gaussian modified Gaussian (Area)") == 0 )
	{
		ff->fct = sasfit_peak_HalfGaussianModifiedGaussianArea;
	} else
	if ( strcmp(ff->typestr, "Inverted Gamma (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_InvertedGammaAmplitude;
	} else
	if ( strcmp(ff->typestr, "Inverted Gamma (Area)") == 0 )
	{
		ff->fct = sasfit_peak_InvertedGammaArea;
	} else
	if ( strcmp(ff->typestr, "Kumaraswamy (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_KumaraswamyAmplitude;
	} else
	if ( strcmp(ff->typestr, "Kumaraswamy (Area)") == 0 )
	{
		ff->fct = sasfit_peak_KumaraswamyArea;
	} else
	if ( strcmp(ff->typestr, "Laplace (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LaplaceAmplitude;
	} else
	if ( strcmp(ff->typestr, "Laplace (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LaplaceArea;
	} else
	if ( strcmp(ff->typestr, "Logistic (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LogisticAmplitude;
	} else
	if ( strcmp(ff->typestr, "Logistic (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LogisticArea;
	} else
	if ( strcmp(ff->typestr, "LogLogistic (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LogLogisticAmplitude;
	} else
	if ( strcmp(ff->typestr, "LogLogistic (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LogLogisticArea;
	} else
	if ( strcmp(ff->typestr, "LogNormal, 4 parameters (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LogNormal4ParameterAmplitude;
	} else
	if ( strcmp(ff->typestr, "LogNormal, 4 parameters (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LogNormal4ParameterArea;
	} else
	if ( strcmp(ff->typestr, "LogNormal (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LogNormalAmplitude;
	} else
	if ( strcmp(ff->typestr, "LogNormal (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LogNormalArea;
	} else
	if ( strcmp(ff->typestr, "Lorentzian (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_LorentzianAmplitude;
	} else
	if ( strcmp(ff->typestr, "Lorentzian (Area)") == 0 )
	{
		ff->fct = sasfit_peak_LorentzianArea;
	} else
	if ( strcmp(ff->typestr, "Maxwell (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_MaxwellAmplitude;
	} else
	if ( strcmp(ff->typestr, "Maxwell (Area)") == 0 )
	{
		ff->fct = sasfit_peak_MaxwellArea;
	} else
	if ( strcmp(ff->typestr, "generalized Maxwell (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_generalizedMaxwellAmplitude;
	} else
	if ( strcmp(ff->typestr, "generalized Maxwell (Area)") == 0 )
	{
		ff->fct = sasfit_peak_generalizedMaxwellArea;
	} else
	if ( strcmp(ff->typestr, "Pearson IV (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_PearsonIVAmplitude;
	} else
	if ( strcmp(ff->typestr, "Pearson IV (Area)") == 0 )
	{
		ff->fct = sasfit_peak_PearsonIVArea;
	} else
	if ( strcmp(ff->typestr, "Pearson VII (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_PearsonVIIAmplitude;
	} else
	if ( strcmp(ff->typestr, "Pearson VII (Area)") == 0 )
	{
		ff->fct = sasfit_peak_PearsonVIIArea;
	} else
	if ( strcmp(ff->typestr, "power LogNormal (Area)") == 0 )
	{
		ff->fct = sasfit_peak_powerlognormalArea;
	} else
	if ( strcmp(ff->typestr, "power Normal (Area)") == 0 )
	{
		ff->fct = sasfit_peak_powernormalArea;
	} else
	if ( strcmp(ff->typestr, "pulse (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakAmplitude;
	} else
	if ( strcmp(ff->typestr, "pulse (Area)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakArea;
	} else
	if ( strcmp(ff->typestr, "pulse with 2nd width (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakwith2ndWidthTermAmplitude;
	} else
	if ( strcmp(ff->typestr, "pulse with 2nd width (Area)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakwith2ndWidthTermArea;
	} else
	if ( strcmp(ff->typestr, "pulse with power term (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakwithPowerTermAmplitude;
	} else
	if ( strcmp(ff->typestr, "pulse with power term (Area)") == 0 )
	{
		ff->fct = sasfit_peak_PulsePeakwithPowerTermArea;
	} else
	if ( strcmp(ff->typestr, "Student-t (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_Student_tAmplitude;
	} else
	if ( strcmp(ff->typestr, "Student-t (Area)") == 0 )
	{
		ff->fct = sasfit_peak_Student_tArea;
	} else
	if ( strcmp(ff->typestr, "Voigt (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_VoigtPeakAmplitude;
	} else
	if ( strcmp(ff->typestr, "Voigt (Area)") == 0 )
	{
		ff->fct = sasfit_peak_VoigtPeakArea;
	} else
	if ( strcmp(ff->typestr, "Weibull (Amplitude)") == 0 )
	{
		ff->fct = sasfit_peak_WeibullAmplitude;
	} else
	if ( strcmp(ff->typestr, "Weibull (Area)") == 0 )
	{
		ff->fct = sasfit_peak_WeibullArea;
	} else
	if ( strcmp(ff->typestr, "Pcs:homogeneous") == 0 )
	{
		ff->fct = sasfit_ff_Pcs_homogeneousPlanar;
	} else
	if ( strcmp(ff->typestr, "Pcs:homogeneousCyl") == 0 )
	{
		ff->fct = sasfit_ff_Pcs_homogeneousCyl;
	} else
	if ( strcmp(ff->typestr, "Pcs:ellCylSh") == 0 )
	{
		ff->fct = sasfit_ff_Pcs_ellCylSh;
	} else
	if ( strcmp(ff->typestr, "p(r) -> 4 pi sin(qr)/(qr)") == 0 )
	{
		ff->fct = sasfit_ff_aux_p_r;
	} else
	if ( strcmp(ff->typestr, "gamma(r) -> 4 pi r^2 sin(qr)/(qr)") == 0 )
	{
		ff->fct = sasfit_ff_aux_gamma_r;
	} else
	{
		// unknown form factor
		sasfit_param_set_err(&ff->params, "Unknown form factor!");
		return TCL_ERROR;
	}
	return TCL_OK;
}
