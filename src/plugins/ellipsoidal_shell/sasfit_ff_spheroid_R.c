/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_PRINCIPLE		param->p[0]
#define R_EQUATORIAL	param->p[1]
#define T	param->p[3]

scalar sasfit_ff_spheroid_R_core(sasfit_param * param) {
	A = R_PRINCIPLE;
	B = R_EQUATORIAL;
	TNU = 0;
	return LNDISTR*gsl_sf_pow_int(F_EllSh(param),lround(P));
}

scalar sasfit_ff_spheroid_R_core_MU(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	MU = x;
	LNDISTR = 1;
	NU = 1;
	res = sasfit_ff_spheroid_R_core(param);
	return res;
}

scalar sasfit_ff_spheroid_R(scalar q, sasfit_param * param)
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

	Q = q;		// Q
	P = 2.0;	// pow
	SIGMA = 0;
	T = 0;
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
	cparam.func = &sasfit_ff_spheroid_R_core;
	
	intstrategy = sasfit_get_int_strategy();
//	intstrategy=P_CUBATURE;
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
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_spheroid_R_core_MU, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_spheroid_R_f(scalar q, sasfit_param * param)
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

	Q = q;		// Q
	P = 1.0;	// pow
	SIGMA = 0;
	T = 0.0;
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
	cparam.func = &sasfit_ff_spheroid_R_core;
	
	intstrategy = sasfit_get_int_strategy();
//	intstrategy=P_CUBATURE;
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
            sum=sasfit_integrate(0.0, 1.0, sasfit_ff_spheroid_R_core_MU, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_spheroid_R_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./.3*M_PI*gsl_pow_2(R_EQUATORIAL)*R_PRINCIPLE;
}

