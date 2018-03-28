/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_PRINCIPLE	param->p[0]*NU
#define R_EQUATORIAL	param->p[1]
#define T	param->p[3]
#define RP param->p[0]
#define RE param->p[1]
#define TT param->p[3]


scalar sasfit_ff_ellip_shell_rp_core(sasfit_param * param) {
	A = R_PRINCIPLE;
	B = R_EQUATORIAL;
	TNU = T;
	return LNDISTR*gsl_sf_pow_int(F_EllSh(param),lround(P));
}

scalar sasfit_ff_ellip_shell_rp_core_NU(scalar x, sasfit_param * param)
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
	return sasfit_ff_ellip_shell_rp_core(param);
}


scalar sasfit_ff_ellip_shell_rp_core_MU(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	MU = x;

	res = sasfit_integrate(NUMIN, NUMAX, sasfit_ff_ellip_shell_rp_core_NU, param);

	return res;
}

scalar sasfit_ff_ellipsoidal_shell_rp(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_PRINCIPLE < 0.0), param, "R_PRINCIPLE(%lg) < 0",R_PRINCIPLE);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_EQUATORIAL(%lg) < 0",R_EQUATORIAL);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "SIGMA(%lg) < 0",SIGMA);

	Q = q;		// Q
	P = 2.0;	// pow
	if (SIGMA==0) {
		ndim=1;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =2;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=NUMIN;
	cubxmax[1]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_ellip_shell_rp_core;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, cubxmin[0],cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_P_OOURA1, cubxmin[0],cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_ellip_shell_rp_core_MU, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_ellipsoidal_shell_rp_f(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_PRINCIPLE < 0.0), param, "R_PRINCIPLE(%lg) < 0",R_PRINCIPLE);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_MINOR(%lg) < 0",R_EQUATORIAL);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "SIGMA(%lg) < 0",SIGMA);

	Q = q;		// Q
	P = 1.0;	// pow
	if (SIGMA==0) {
		ndim=1;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =2;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=NUMIN;
	cubxmax[1]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_ellip_shell_rp_core;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, cubxmin[0],cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_P_OOURA1, cubxmin[0],cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_ellip_shell_rp_core_MU, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_ellipsoidal_shell_rp_v(scalar q, sasfit_param * param, int dist)
{
	scalar V,nu1,nu2,nu3,T3,T2,RE2;
	SASFIT_ASSERT_PTR(param);
	nu1=exp(0.5*SIGMA*SIGMA*1*1);
	nu2=exp(0.5*SIGMA*SIGMA*2*2);
	nu3=exp(0.5*SIGMA*SIGMA*3*3);
	T3 = gsl_pow_3(TT);
	T2 = gsl_pow_2(TT);
	RE2 = gsl_pow_2(RE);

	V = (RP*T2+2*RE*RP*TT+RE2*RP)*nu1+T3+2*RE*T2+RE2*TT;
	// insert your code here
	return 4./3.*M_PI*V;
}

