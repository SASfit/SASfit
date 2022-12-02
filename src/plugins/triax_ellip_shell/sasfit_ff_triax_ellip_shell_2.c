/*
 * src/plugins/triax_ellip_shell/sasfit_ff_triax_ellip_shell.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]*NU
#define B	param->p[1]*NU
#define C	param->p[2]
#define T	param->p[3]
#define AA	param->p[0]
#define BB	param->p[1]
#define CC	param->p[2]
#define TT	param->p[3]


scalar sasfit_ff_triax_ellip_shell_2_core(sasfit_param * param)
{
	scalar u_c, u_sh;
	scalar f_c, f_sh;

	SASFIT_ASSERT_PTR(param);

	if (fabs(A*B*C) + fabs(T) == 0.0) return 0.0;

	u_c = Q*sqrt(( pow(A*cos(X*M_PI/2.),2.) + pow(B*sin(X*M_PI/2.),2.) )*(1.-Y*Y) + pow(C*Y,2.));
	u_sh = Q*sqrt(( pow((A+T)*cos(X*M_PI/2.),2.) + pow((B+T)*sin(X*M_PI/2.),2.) ) * (1.-Y*Y) + pow((C+T)*Y,2.));

	f_c  = 4./3.*M_PI*A*B*C*(ETA_C-ETA_SH);
	if (u_c != 0.0)
	{
		f_c  = f_c * 3*(sin(u_c) -u_c *cos(u_c) )/pow(u_c ,3);
	}

	f_sh  = 4./3.*M_PI*(A+T)*(B+T)*(C+T)*(ETA_SH-ETA_SOL);
	if (u_sh != 0.0) 
	{
		f_sh  = f_sh * 3*(sin(u_sh)-u_sh*cos(u_sh))/pow(u_sh,3);
	}

	return LNDISTR*gsl_sf_pow_int(f_sh+f_c, lround(P));
}


scalar sasfit_ff_triax_ellip_shell_2_core_NU(scalar x, sasfit_param * param)
{
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	if (SIGMA==0) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		LNDISTR = sasfit_sd_LogNorm(x, &subParam);
		NU=x;
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	return sasfit_ff_triax_ellip_shell_2_core(param);
}

scalar sasfit_ff_triax_ellip_shell_2_core_y(scalar y, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	Y = y;
	if (SIGMA == 0) {
		NU = 1;
		LNDISTR = 1;
		res = sasfit_ff_triax_ellip_shell_2_core(param);
	} else {
		res = sasfit_integrate(NUMIN, NUMAX, sasfit_ff_triax_ellip_shell_2_core_NU, param);
	}

	return res;
}

scalar sasfit_ff_triax_ellip_shell_2_core_x(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	X = x;

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_2_core_y, param);

	return res;
}

scalar sasfit_ff_triax_ellip_shell_2(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	
	NU = 1;
	LNDISTR=1.0;
	
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A);
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B);
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "SIGMA(%lg) < 0",SIGMA);

	Q = q;		// Q
	P = 2.0;	// pow
	if (SIGMA==0) {
		ndim=2;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =3;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=0;
	cubxmax[1]=1;
	cubxmin[2]=NUMIN;
	cubxmax[2]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_triax_ellip_shell_2_core;
	
	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intde(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &triax_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &triax_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_2_core_x, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_triax_ellip_shell_2_f(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	
	NU = 1;
	LNDISTR=1.0;
	
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A);
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B);
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "SIGMA(%lg) < 0",SIGMA);

	Q = q;		// Q
	P = 1.0;	// pow
	if (SIGMA==0) {
		ndim=2;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =3;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=0;
	cubxmax[1]=1;
	cubxmin[2]=NUMIN;
	cubxmax[2]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_triax_ellip_shell_2_core;
	
	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intde(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &triax_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &triax_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_2_core_x, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_triax_ellip_shell_2_v(scalar x, sasfit_param * param, int dist)
{
	scalar V,nu1,nu2,nu3;
	SASFIT_ASSERT_PTR(param);
	nu1=exp(0.5*SIGMA*SIGMA*1*1);
	nu2=exp(0.5*SIGMA*SIGMA*2*2);
	nu3=exp(0.5*SIGMA*SIGMA*3*3);
	
	V = (A*B*T+A*B*C)*nu2+(B*T*T+A*T*T+B*C*T+A*C*T)*nu1+T*T*T+C*T*T;
	V=V*4./3.*M_PI;
	return V;
	
	switch ( dist )
	{
		case 0:
			V = (x + T) * (B + T) * (C + T);
			break;
		case 1:
			V = (A + T) * (x + T) * (C + T);
			break;
		case 2:
			V = (A + T) * (B + T) * (x + T);
			break;
		case 3:
			V = (A + x) * (B + x) * (C + x);
			break;
		default:
			V = (A + T) * (B + T) * (C + T);
			break;
	}
	return 4.0/3.0 * M_PI * x;
}

