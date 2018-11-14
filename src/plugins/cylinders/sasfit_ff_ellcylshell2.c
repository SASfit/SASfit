/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define EPSILON	param->p[2]
#define L	param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SHELL	param->p[5]
#define ETA_SOLV	param->p[6]

#define Q   param->p[MAXPAR-1]
#define TYPE_SHELL  param->p[MAXPAR-2]
#define THETA param->p[MAXPAR-3]
#define ALPHA param->p[MAXPAR-4]
#define P param->p[MAXPAR-5]

scalar sasfit_ff_ellcylshell2(scalar q, sasfit_param * param)
{   scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000,ndim;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
    Q=q;
    P=2;
    TYPE_SHELL=1;
	cubxmin[0]=0;
	cubxmax[0]=M_PI_2;
	cubxmin[1]=0;
	cubxmax[1]=M_PI_2;

	intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_CLENSHAW_CURTIS_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&A_alpha,0,M_PI_2, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&A_alpha, 0,M_PI_2, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            ndim=2;
            if (EPSILON == 1.0) ndim=1;
			hcubature(1, &A_cub2,param,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
            ndim=2;
            if (EPSILON == 1.0) ndim=1;
			pcubature(1, &A_cub2,param,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("This is default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0,M_PI_2, &A_alpha_sasfit, param);
            break;
            }
    }
    return sum;
}

scalar sasfit_ff_ellcylshell2_f(scalar q, sasfit_param * param)
{   scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000,ndim;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    Q=q;
    P=1;
    TYPE_SHELL=1;
	cubxmin[0]=0;
	cubxmax[0]=M_PI_2;
	cubxmin[1]=0;
	cubxmax[1]=M_PI_2;

	intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_CLENSHAW_CURTIS_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&A_alpha,0,M_PI_2, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&A_alpha, 0,M_PI_2, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            ndim=2;
            if (EPSILON == 1.0) ndim=1;
			hcubature(1, &A_cub2,param,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
            ndim=2;
            if (EPSILON == 1.0) ndim=1;
			pcubature(1, &A_cub2,param,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("This is default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0,M_PI_2, &A_alpha_sasfit, param);
            break;
            }
    }
    return sum;
}

scalar sasfit_ff_ellcylshell2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*(R+T)*(R*EPSILON+T)*(L+T);;
}

