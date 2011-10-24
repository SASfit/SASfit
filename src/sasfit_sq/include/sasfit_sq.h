/*
 * src/sasfit_sq/include/sasfit_sq.h
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

#ifndef SASFIT_SQ_H
#define SASFIT_SQ_H

#include <sasfit_common.h>
#include "sasfit_sq_shared_exports.h"

/**
 * \file sasfit_sq.h
 * \ingroup struct_fac
 * Functions which are not (yet) available as plugins are declared here.
 */

typedef enum
{
	SQ_id_RPA,
	SQ_id_None
} sasfit_SQ_id;

sasfit_sq_DLLEXP scalar sasfit_sq_BabaAhmed2(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_BabaAhmed(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_CorrelationHole(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_CriticalScattering1(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Cylinder_PRISM(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_HardSphere(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_HayterPenfoldMSA(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_LocalOrderModel(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MacroIon(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MassFractal(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MassFractalGaussianCutOff(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MassFractExp_mpow_x_a(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MassFractOverlapSph(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_MCT(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_PT(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_RandomDistributionModel(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_SquareWell2(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_SquareWell(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_StickyHardSphere2(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_StickyHardSphere(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_TD(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_ThinSphericalShell(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_ThinSquareWell(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_None(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_rod(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_KholodenkoWorm(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_WormPS1(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_WormPS2(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_WormPS3(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_WormLikeChainEXV(scalar x, sasfit_param * param);
sasfit_sq_DLLEXP scalar sasfit_sq_Pprime_WormLikeChainGaussian(scalar x, sasfit_param * param);

#endif
