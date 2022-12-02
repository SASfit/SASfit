/*
 * src/sasfit_ff/sasfit_ff_cyl_rwbrush_old.c
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

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

/*
float P_Cylinder_RWbrush(Tcl_Interp *interp, 
			float Q,
			float Rc,		// core radius
			float n_agg,		// aggregation number of chains per surfacearea
			float V_brush,	// volume of a single chain in brush
			float eta_core,	// scattering length density of non-swollen core
			float eta_brush,	// scattering length of a single chain in brush
			float eta_solv,	// scattering length density of core
			float xsolv_core,// amount of solvent in core
			float Rg,		// gyration radius of chain
			float d,			// correction factor should be chosen close to 1
			float H,			// height of cylinder
			bool *error)
*/



#define CYL_RWBRUSH_HEADER \
\
	CYL_RWBRUSH_BODY_VARS; \
	SASFIT_ASSERT_PTR(param); \
	V[Q] = q

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

#define CYL_RWBRUSH_COMMON_TESTS \
	SASFIT_CHECK_COND1((V[Q] < 0.0),           param, "q(%lg) < 0",V[Q]); \
	SASFIT_CHECK_COND1((V[RG] < 0.0),          param, "Rg(%lg) < 0",V[RG]); \
	SASFIT_CHECK_COND1((V[V_BRUSH] < 0.0),     param, "V_brush(%lg) < 0",V[V_BRUSH]); \
	SASFIT_CHECK_COND1((V[XSOLV_CORE] == 1.0), param, "xsolv_core(%lg) == 1",V[XSOLV_CORE])

#define V       param->p

typedef enum
{
	PARAM0,
	PARAM1,
	V_BRUSH,
	ETA_CORE,
	ETA_BRUSH,
	ETA_SOLV,
	XSOLV_CORE,
	RG,
	D,
	H,
	NAGG,
	N_AGG,
	V_CORE,
	R,
	Q
} variable_names;


scalar F_PSI(scalar Q, scalar R, scalar H, scalar alpha)
{
        scalar xR, xH, PSIres;

        xR = Q*R*sin(alpha);
        xH = Q*H*cos(alpha)/2.0;

        if (xR == 0.0) 
	{
                PSIres = 1.0;
        } else {
                PSIres = 2.0*gsl_sf_bessel_J1(xR)/xR;
        }

        if (xH == 0.0) 
	{
                PSIres = PSIres;
        } else {
                PSIres = PSIres * sin(xH)/xH;
        }

        return PSIres;
}

scalar XI(scalar Q, scalar R, scalar H, scalar alpha)
{
        scalar xR, xH, XIres;

        xR = Q*R*sin(alpha);
        xH = Q*H*cos(alpha)/2.0;

        if ((R+H) == 0.0) return 0.0;

        if (xR == 0.0)
	{
                XIres = R/(R+H) * cos(xH);
        } else {
                XIres = R/(R+H) * 2.0*gsl_sf_bessel_J1(xR)/xR * cos(xH);
        }

        if (xH == 0.0)
	{
                XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR);
        } else {
                XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR)*sin(xH)/xH;
        }

        return XIres;
}

scalar sasfit_ff_P18(scalar Q, scalar Rg)
{
        scalar u;
        u = Q*Q * Rg*Rg;
        if ( u == 0 ) return 1.0;

        return 2*(exp(-u)+u-1.0) / (u*u);
}

scalar sasfit_ff_cyl_rwbrush_fs_core(scalar alpha, sasfit_param *param)
{
	SASFIT_ASSERT_PTR(param);

	return pow( F_PSI(V[Q],V[R],V[H],alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_scc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(V[Q], V[RG]);

	return pow(w*XI(V[Q], V[R]+V[D]*V[RG], V[H]+2.0*V[D]*V[RG], alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_ssc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(V[Q], V[RG]);

	return w * F_PSI(V[Q], V[R], V[H], alpha) * XI(V[Q], V[R]+V[D]*V[RG], V[H]+2.0*V[D]*V[RG], alpha) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush(scalar q, sasfit_param * param)
{
	scalar s, vol;
	CYL_RWBRUSH_HEADER;

	V[R] = V[PARAM0];
	V[N_AGG] = V[PARAM1];

	CYL_RWBRUSH_COMMON_TESTS;
	SASFIT_CHECK_COND1((V[H] < 0.0),           param, "H(%lg) < 0",V[H]);

	s = 2.0*M_PI*V[R]*V[H] + M_PI*pow(V[R],2.0);
	V[NAGG] = V[N_AGG]*s;

	vol = M_PI * pow(V[R],2.)*V[H];
	if (V[NAGG] != 0) 
	{
		V[V_CORE] = vol*(1.0-V[XSOLV_CORE])/V[NAGG];
	} else {
		V[V_CORE] = 0.0;
	}

	CYL_RWBRUSH_BODY;
	return pcyl;
}

scalar sasfit_ff_cyl_rwbrush_nagg(scalar q, sasfit_param * param)
{
	CYL_RWBRUSH_HEADER;

	V[NAGG] = V[PARAM0];
	V[V_CORE] = V[PARAM1];

	CYL_RWBRUSH_COMMON_TESTS;
	SASFIT_CHECK_COND1((V[V_CORE] < 0.0), param, "V_core(%lg) < 0",V[V_CORE]);
	SASFIT_CHECK_COND((V[NAGG] == 0.0), param, "Nagg == 0");

	V[R] = sqrt( fabs(V[NAGG]*(V[V_CORE]/(1.0-V[XSOLV_CORE]))/M_PI/V[H]));

	CYL_RWBRUSH_BODY;
	return pcyl;
}


scalar sasfit_ff_cyl_rwbrush_rc(scalar q, sasfit_param * param)
{
	CYL_RWBRUSH_HEADER;

	V[R] = V[PARAM0];
	V[V_CORE] = V[PARAM1];

	CYL_RWBRUSH_COMMON_TESTS;
	SASFIT_CHECK_COND1((V[V_CORE] < 0.0), param, "V_core(%lg) < 0",V[V_CORE]);
	SASFIT_CHECK_COND1((V[H] < 0.0), param, "H(%lg) < 0",V[H]);

	V[NAGG] = M_PI * pow(V[R],2.)*V[H] * (1.-V[XSOLV_CORE])/V[V_CORE];

	CYL_RWBRUSH_BODY;
	return pcyl;
}


scalar sasfit_ff_cyl_rwbrush_old(scalar q, sasfit_param * param)
{
	scalar r_core, r_chain, Pcyl, R, S, vol, n_agg;
	scalar Rc, Nagg, V_core, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, d, H;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &d, &H);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND1((V_brush < 0.0), param, "V_brush(%lg) < 0",V_brush);
	SASFIT_CHECK_COND1((xsolv_core == 1.0), param, "xsolv_core(%lg) == 1",xsolv_core);

	switch( param->kernelSelector )
	{
		case CYL_RWBRUSH_NAGG:	
			Nagg		= param->p[0];
			V_core		= param->p[1];

			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			SASFIT_CHECK_COND((Nagg == 0.0), param, "Nagg == 0");

			R  = sqrt(fabs(Nagg*(V_core/(1.0-xsolv_core))/M_PI/H));
			break;
		case CYL_RWBRUSH_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];

			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H);

			R = Rc;
			Nagg = M_PI * pow(Rc,2.)*H * (1.-xsolv_core)/V_core;

			break;
		case CYL_RWBRUSH:
			Rc		= param->p[0];
			n_agg		= param->p[1];

			SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H);

			vol = M_PI * pow(Rc,2.)*H;
			S = 2.*M_PI*Rc*H + M_PI*pow(Rc,2.);
			R = Rc;

			Nagg = n_agg*S;
			if (Nagg != 0) 
			{
				V_core = vol*(1.-xsolv_core)/Nagg;
			} else {
				V_core = 0;
			}

			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	param->p[MAXPAR-1] = q;
	param->p[MAXPAR-2] = R;

        r_core  = Nagg*V_core  * (eta_core  - eta_solv);
        r_chain =      V_brush * (eta_brush - eta_solv);
/*
        Pcyl =        pow(r_core,2.0) * sasfit_Cyl(CYLFS, q, R, H);
        Pcyl = Pcyl + Nagg*r_chain*r_chain * sasfit_CylP18(q,Rg);
        Pcyl = Pcyl + Nagg*(Nagg-1.)*r_chain*r_chain * sasfit_Cyl(CYLSCC, q, R, H, Rg, d)*((Nagg < 1) ?  0 : 1);
        Pcyl = Pcyl + 2.*Nagg*r_core*r_chain * sasfit_Cyl(SSC, q, R, H, Rg, d);
*/
        Pcyl =        pow(r_core,2.0) * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_fs_core, param);
        Pcyl = Pcyl + Nagg*r_chain*r_chain * sasfit_ff_P18(q,Rg);
        Pcyl = Pcyl + Nagg*(Nagg-1.)*r_chain*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_scc_core, param)*((Nagg < 1) ?  0 : 1);
        Pcyl = Pcyl + 2.*Nagg*r_core*r_chain * sasfit_integrate(0.0, M_PI_2, sasfit_ff_cyl_rwbrush_ssc_core, param);

        return Pcyl;
}

