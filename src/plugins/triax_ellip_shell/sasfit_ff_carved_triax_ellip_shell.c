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

#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define R0	param->p[3]
#define ALPHA param->p[4]
#define	BETA	param->p[MAXPAR-3] // param from outer integration
#define	GAMA	param->p[MAXPAR-4] // param from second outermost integration
#define PHI	param->p[MAXPAR-5] // param from innermost integration
#define R	param->p[MAXPAR-6] // param from second innermost integration
#define EE  param->p[MAXPAR-7]
#define SINPHI  param->p[MAXPAR-8]
#define COSPHI  param->p[MAXPAR-9]
#define H1  param->p[MAXPAR-10]
#define H1w  param->p[MAXPAR-11]
#define H2  param->p[MAXPAR-12]
#define H2w  param->p[MAXPAR-13]

scalar sasfit_ff_carved_triax_ellip_shell_core(sasfit_param * param)
{
    return gsl_sf_bessel_J0(H1*cos(PHI))*cos(H2*sin(PHI))*cos(PHI)*R*R *A*B*C*(ETA_SH-ETA_SOL);
}

scalar Kernel_carvedP_OOURA4(scalar x, void * pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;


	R = x;

	return  (cparam->func)(param);
}

scalar Kernel_carvedP_OOURA3(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam2;
	cparam2 = (cubature_param *) pam;
	param = cparam2->param;
	scalar sum, err, *aw;
	int lenaw;

	lenaw=10000;
	PHI = x;

    EE= sqrt(gsl_pow_2(A*cos(GAMA))+gsl_pow_2(B*sin(GAMA))+C*C);
    SINPHI = sqrt(gsl_pow_2(A*cos(GAMA))+gsl_pow_2(B*sin(GAMA)))/EE;
    COSPHI = C/EE;
    H1w = EE*Q*R*SINPHI*sin(BETA);
    H1 = H1w*R;
    H2w = EE*Q*R*COSPHI*cos(BETA);
    H2 = H2w*R;
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_carvedP_OOURA4, cparam2->cubxmin[1], cparam2->cubxmax[1], aw, &sum, &err,pam);
			free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_carvedP_OOURA4, cparam2->cubxmin[1], cparam2->cubxmax[1], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,pam);
			free(aw);
            break;
            }
	}
	return sum;
}

scalar Kernel_carvedP_OOURA2(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	scalar sum, err, *aw;
	int lenaw;
    scalar cubxmin[2], cubxmax[2];
    int intstrategy, ndim2;
	cubature_param cparam2;
	ndim2=2;

	cubxmin[0]=0;
	cubxmax[0]=ALPHA;
	cubxmin[1]=R0;
	cubxmax[1]=1;
	cparam2.param = param;
	cparam2.cubxmin=cubxmin;
	cparam2.cubxmax=cubxmax;
	cparam2.ndim=ndim2;
	cparam2.func = cparam->func;

	lenaw=10000;
	BETA = x;

	intstrategy = sasfit_get_int_strategy();
//	intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_carvedP_OOURA3, cparam2.cubxmin[0], cparam2.cubxmax[0], aw, &sum, &err,&cparam2);
			free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_carvedP_OOURA3, cparam2.cubxmin[0], cparam2.cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,&cparam2);
			free(aw);
            break;
            }
	}
	return gsl_pow_int(sum,lround(P))*sin(BETA);
}

scalar Kernel_carvedP_OOURA1(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	scalar sum, err, *aw;
	int lenaw;
    int intstrategy;

	lenaw=10000;
	GAMA = x;
	intstrategy = sasfit_get_int_strategy();
//	intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_carvedP_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], aw, &sum, &err,pam);
			free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_carvedP_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,pam);
			free(aw);
            break;
            }
	}
	return sum;
}

int carved_triax_ellip_shell_F_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 4) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}

    BETA = x[0];
    GAMA = x[1];
    PHI = x[2];
    R = x[3];

    fval[0] = (cparam->func)(param)*sin(BETA);
    return 0;
}

int carved_triax_ellip_shell_I_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}


    PHI = x[0];
    R = x[1];

    fval[0] = (cparam->func)(param);
    return 0;
}

int carved_triax_ellip_shell_Iavg_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval2[1], ferr2[1];
    size_t neval;
	int lenaw;
    int intstrategy, ndim2;
	cubature_param cparam2;

	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	lenaw=10000;

	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}

    BETA = x[0];
    GAMA = x[1];

    EE= sqrt(gsl_pow_2(A*cos(GAMA))+gsl_pow_2(B*sin(GAMA))+C*C);
    SINPHI = sqrt(gsl_pow_2(A*cos(GAMA))+gsl_pow_2(B*sin(GAMA)))/EE;
    COSPHI = C/EE;
    H1w = EE*Q*SINPHI*sin(BETA);
    H1 = H1w*R;
    H2w = EE*Q*COSPHI*cos(BETA);
    H2 = H2w*R;

	ndim2=2;
	cubxmin[0]=0;
	cubxmax[0]=ALPHA;
	cubxmin[1]=R0;
	cubxmax[1]=1;
	cparam2.param = param;
	cparam2.cubxmin=cubxmin;
	cparam2.cubxmax=cubxmax;
	cparam2.ndim=ndim2;
	cparam2.func = cparam->func;

	intstrategy = sasfit_get_int_strategy();
//	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_carvedP_OOURA3, cparam2.cubxmin[0], cparam2.cubxmax[0], aw, &sum, &err,&cparam2);
			free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_carvedP_OOURA3, cparam2.cubxmin[0], cparam2.cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,&cparam2);
			free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &carved_triax_ellip_shell_I_cubature,&cparam2,ndim2, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval2, ferr2);
			sum = fval2[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &carved_triax_ellip_shell_I_cubature,&cparam2,ndim2, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval2, ferr2);
			sum = fval2[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
			hcubature(1, &carved_triax_ellip_shell_I_cubature,&cparam2,ndim2, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval2, ferr2);
			sum = fval2[0];
            break;
            }
    }
	fval[0] = gsl_pow_int(sum,lround(P))*sin(BETA);
    return 0;
}


scalar sasfit_ff_carved_triax_ellip_shell(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A);
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B);
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C);
	SASFIT_CHECK_COND1((R0 < 0.0), param, "r0(%lg) < 0",R0);
	SASFIT_CHECK_COND1((R0 >= 1.0), param, "r0(%lg) >= 1",R0);
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "ALPHA(%lg) <= 0",ALPHA);
	SASFIT_CHECK_COND1((ALPHA > M_PI_2), param, "ALPHA(%lg) > Pi/2",ALPHA);


	Q = q;		// Q
	P = 2.0;	// pow
	ndim=2;
	cubxmin[0]=0;
	cubxmax[0]=M_PI;
	cubxmin[1]=0;
	cubxmax[1]=2*M_PI;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_carved_triax_ellip_shell_core;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=H_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_carvedP_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_carvedP_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &carved_triax_ellip_shell_Iavg_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &carved_triax_ellip_shell_Iavg_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
			hcubature(1, &carved_triax_ellip_shell_Iavg_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_carved_triax_ellip_shell_f(scalar q, sasfit_param * param)
{
	return 0;
}

scalar sasfit_ff_carved_triax_ellip_shell_v(scalar x, sasfit_param * param, int dist)
{
    scalar V;
	switch ( dist )
	{
		case 0:
			V = (x) * (B) * (C);
			break;
		case 1:
			V = (A) * (x) * (C);
			break;
		case 2:
			V = (A) * (B) * (x);
			break;
		default:
			V = (A) * (B) * (C);
			break;
	}
	return 4.0/3.0 * M_PI * V;
}

