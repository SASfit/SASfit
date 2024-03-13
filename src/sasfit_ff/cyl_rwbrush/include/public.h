/*
 * src/sasfit_ff/cyl_rwbrush/include/public.h
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

#ifndef CYL_RWBRUSH_PUBLIC_H
#define CYL_RWBRUSH_PUBLIC_H

#define CYL_RWBRUSH_BODY_VARS \
	scalar r_core, r_chain, pcyl


#define CYL_RWBRUSH_BODY \
	do { \
		r_core  = V[NAGG]*V[V_CORE]  * (V[ETA_CORE] - V[ETA_SOLV]); \
		r_chain =         V[V_BRUSH] * (V[ETA_BRUSH]- V[ETA_SOLV]); \
	\
		pcyl =        pow(r_core,2.0) * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_fs_core, param); \
		pcyl = pcyl + V[NAGG]*r_chain*r_chain * sasfit_ff_P18(q, V[RG]); \
		pcyl = pcyl + V[NAGG]*(V[NAGG]-1.)*r_chain*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_scc_core, param)*((V[NAGG] < 1) ?  0 : 1); \
		pcyl = pcyl + 2.*V[NAGG]*r_core*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_ssc_core, param); \
	} while(0)


scalar F_PSI(scalar Q, scalar R, scalar H, scalar alpha);

scalar XI(scalar Q, scalar R, scalar H, scalar alpha);

scalar sasfit_ff_P18(scalar Q, scalar Rg);

scalar sasfit_ff_cyl_rwbrush_fs_core(scalar alpha, sasfit_param *param);

scalar sasfit_ff_cyl_rwbrush_scc_core(scalar alpha, sasfit_param *param);

scalar sasfit_ff_cyl_rwbrush_ssc_core(scalar alpha, sasfit_param *param);


sasfit_ff_DLLEXP scalar sasfit_ff_cyl_rwbrush(scalar q, sasfit_param * param);

sasfit_ff_DLLEXP scalar sasfit_ff_cyl_rwbrush_nagg(scalar q, sasfit_param * param);

sasfit_ff_DLLEXP scalar sasfit_ff_cyl_rwbrush_rc(scalar q, sasfit_param * param);

#endif

