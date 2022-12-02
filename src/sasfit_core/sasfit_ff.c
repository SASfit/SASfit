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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <string.h>
#include <sasfit_ff.h>
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
	if ( strcmp(ff->typestr, "BeaucageExpPowLaw2") == 0 )
	{
		ff->fct = sasfit_ff_UnifiedExponetialPowerLaw2;
	} else
	if ( strcmp(ff->typestr, "BeaucageExpPowLaw") == 0 )
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
	if ( strcmp(ff->typestr, "SPHERE+Exp_Rc") == 0 )
	{
		ff->fct = sasfit_ff_Sphere_Exp_Profile_Rc;
	} else
	if ( strcmp(ff->typestr, "Background") == 0 )
	{
		ff->fct = sasfit_ff_background;
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
	{
		// unknown form factor
		sasfit_param_set_err(&ff->params, "Unknown form factor!");
		return TCL_ERROR;
	}
	return TCL_OK;
}
