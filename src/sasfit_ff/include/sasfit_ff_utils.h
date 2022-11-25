/*
 * src/sasfit_ff/include/sasfit_ff_utils.h
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

#ifndef SASFIT_FF_UTILS_H
#define SASFIT_FF_UTILS_H

#include <sasfit_common.h>
#include <sasfit_error_ff.h>
#include "sasfit_ff.h"

scalar sasfit_ff_P18 (scalar Q, scalar Rg);
scalar sasfit_Pthirtynine(scalar q, sasfit_param * param);
scalar sasfit_planar_gauss_chains(scalar q, sasfit_param * param);
scalar sasfit_ff_Pcs_ellCylSh(scalar q, sasfit_param * param);
scalar sasfit_ff_Cylinder_core(scalar q, sasfit_param * param);
scalar sasfit_ff_cyl_rwbrush_fs_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_cyl_rwbrush_scc_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_cyl_rwbrush_ssc_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_CylShell_core(scalar x, sasfit_param * param);
scalar sasfit_ff_ellip_rwbrush_ssc_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_ellip_rwbrush_scc_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_ellip_rwbrush_fs_core(scalar alpha, sasfit_param *param);
scalar sasfit_ff_EllipsoidalCoreShell_core(scalar mu, sasfit_param * param);
scalar sasfit_ff_EllipsoidalCoreShell_f_core(scalar mu, sasfit_param * param);
scalar sasfit_ff_Ellipsoid1_core(scalar x, sasfit_param * param);
scalar sasfit_ff_Ellipsoid2_core(scalar x, sasfit_param * param);
scalar sasfit_ff_IsuperparamagneticFFpsiTISANE1_core(scalar x, sasfit_param * param);
scalar sasfit_ff_KholodenkoWorm_core(scalar x, sasfit_param * param);
scalar sasfit_ff_MassFractExp_mpow_x_a_core(scalar x, sasfit_param *param);
scalar sasfit_ff_MassFractOverlappingSpheres_core(scalar x, sasfit_param * param);
scalar sasfit_ff_Rod_R_ma_ave_core(scalar x, sasfit_param * param);
scalar sasfit_ff_shearedCylinder_i_core(scalar x, sasfit_param * param);
scalar sasfit_ff_shearedCylinder_p_core(scalar x, sasfit_param * param);
scalar sasfit_ff_shearedCylinderGauss_p_core(scalar x, sasfit_param * param);
scalar sasfit_ff_shearedCylinder_core(scalar x, sasfit_param * param);
scalar sasfit_ff_shearedCylinderGauss_core(scalar x, sasfit_param * param);
scalar sasfit_ff_Sphere_R_ma_Profile_ave_core(scalar x, sasfit_param * param);
scalar sasfit_ff_Sphere_R_ma_x_beta_ave_core(scalar x, sasfit_param * param);
scalar sasfit_ff_Sphere_R_ma_x_beta_V_core(scalar x, sasfit_param * param);
scalar sasfit_ff_StackDiscs_core(scalar x, sasfit_param * param);
scalar sasfit_ff_ThinCylShell_core(scalar x, sasfit_param * param);
scalar sasfit_ff_ThinEllShell_core(scalar x, sasfit_param * param);
scalar sasfit_ff_ThinSphShell_core(scalar x, sasfit_param * param);
scalar sasfit_ff_triaxEllShell1_core(scalar y, sasfit_param * param);
scalar sasfit_ff_triaxEllShell1_core_x(scalar x, sasfit_param * param);

#endif

