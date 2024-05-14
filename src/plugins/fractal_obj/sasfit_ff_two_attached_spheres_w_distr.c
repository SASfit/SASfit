/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	    param->p[0]
#define SIGMA1  fabs(param->p[1])
#define R2	    param->p[2]
#define SIGMA2  fabs(param->p[3])
#define ETA0	param->p[4]
#define ETA1	param->p[5]
#define ETA2	param->p[6]
#define R1_MU   param->p[MAXPAR-1]
#define R2_MU   param->p[MAXPAR-2]
#define Q       param->p[MAXPAR-3]

scalar two_attached_spheres_v(const double *x, size_t ndim, void *pam) {
	sasfit_param * param;
	sasfit_param subParam;
	scalar LNDISTR1, LNDISTR2;
	sasfit_init_param( &subParam );
	param = (sasfit_param *) pam;
	R1 = x[0];
	R2 = x[1];
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA1;
	subParam.p[2] = 1;
	subParam.p[3] = R1_MU;
	LNDISTR1 = sasfit_sd_LogNorm(x[0], &subParam);
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA2;
	subParam.p[2] = 1;
	subParam.p[3] = R1_MU;
	LNDISTR2 = sasfit_sd_LogNorm(x[1], &subParam);
	return LNDISTR1*LNDISTR2*sasfit_ff_two_attached_spheres_v(Q,param,0);
}

scalar two_attached_spheres_f(const double *x, size_t ndim, void *pam) {
	sasfit_param * param;
	sasfit_param subParam;
	scalar LNDISTR1, LNDISTR2;
	sasfit_init_param( &subParam );
	param = (sasfit_param *) pam;
	R1 = x[0];
	R2 = x[1];
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA1;
	subParam.p[2] = 1;
	subParam.p[3] = R1_MU;
	LNDISTR1 = sasfit_sd_LogNorm(x[0], &subParam);
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA2;
	subParam.p[2] = 1;
	subParam.p[3] = R1_MU;
	LNDISTR2 = sasfit_sd_LogNorm(x[1], &subParam);
	return LNDISTR1*LNDISTR2*sasfit_ff_two_attached_spheres_f(Q,param);
}

scalar two_attached_spheres(const double *x, size_t ndim, void *pam) {
	sasfit_param * param;
	sasfit_param subParam;
	scalar LNDISTR1, LNDISTR2;
	sasfit_init_param( &subParam );
	param = (sasfit_param *) pam;
	R1 = x[0];
	R2 = x[1];
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA1;
	subParam.p[2] = 1;
	subParam.p[3] = R1_MU;
	LNDISTR1 = sasfit_sd_LogNorm(x[0], &subParam);
	subParam.p[0] = 1;
	subParam.p[1] = SIGMA2;
	subParam.p[2] = 1;
	subParam.p[3] = R2_MU;
	LNDISTR2 = sasfit_sd_LogNorm(x[1], &subParam);
	return LNDISTR1*LNDISTR2*sasfit_ff_two_attached_spheres(Q,param);
}

scalar sasfit_ff_two_attached_spheres_w_distr(scalar q, sasfit_param * param)
{
    scalar cubmin[2], cubmax[2], res, err;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "R1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "R2(%lg) < 0",R2); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA1 == 0.0), param, "sigma1(%lg) == 0",SIGMA1); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA2 == 0.0), param, "sigma2(%lg) == 0",SIGMA2); // modify condition to your needs

	R1_MU = R1;
	R2_MU = R2;
	Q = q;
	find_LogNorm_int_range(6,R1,SIGMA1,&cubmin[0],&cubmax[0],param);
	find_LogNorm_int_range(6,R2,SIGMA2,&cubmin[1],&cubmax[1],param);
    sasfit_cubature(2,cubmin,cubmax,&two_attached_spheres,param,sasfit_eps_get_nriq(), &res, &err);
    R1 = R1_MU;
    R2 = R2_MU;
    return res;
}

scalar sasfit_ff_two_attached_spheres_w_distr_f(scalar q, sasfit_param * param)
{
    scalar cubmin[2], cubmax[2], res, err;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	R1_MU = R1;
	R2_MU = R2;
	Q = q;
	find_LogNorm_int_range(6,R1,SIGMA1,&cubmin[0],&cubmax[0],param);
	find_LogNorm_int_range(6,R2,SIGMA2,&cubmin[1],&cubmax[1],param);
    sasfit_cubature(2,cubmin,cubmax,&two_attached_spheres_f,param,sasfit_eps_get_nriq(), &res, &err);
    R1 = R1_MU;
    R2 = R2_MU;
    return res;
}

scalar sasfit_ff_two_attached_spheres_w_distr_v(scalar q, sasfit_param * param, int dist)
{
    scalar cubmin[2], cubmax[2], res, err;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    R1_MU = R1;
	R2_MU = R2;

	find_LogNorm_int_range(6,R1,SIGMA1,&cubmin[0],&cubmax[0],param);
	find_LogNorm_int_range(6,R2,SIGMA2,&cubmin[1],&cubmax[1],param);

    sasfit_cubature(2,cubmin,cubmax,&two_attached_spheres_v,param,sasfit_eps_get_nriq(), &res, &err);

    R1 = R1_MU;
    R2 = R2_MU;
    return res;
}

