/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define SIGMA_R0	fabs(param->p[1])
#define EPSILON	param->p[2]
#define T0	param->p[3]
#define SIGMA_T0	fabs(param->p[4])
#define DUMMY2	param->p[5]
#define DUMMY3	param->p[6]
#define ETA_CORE	param->p[7]
#define ETA_SHELL	param->p[8]
#define ETA_SOLV	param->p[9]


scalar ell_r(scalar radius, scalar epsilo, scalar t, scalar theta)
{
	return sqrt(gsl_pow_2((radius+t)*sin(theta))+gsl_pow_2((radius*epsilo+t)*cos(theta)));
}

scalar pcs_ellcylsh(sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,b1,b2, R,T;
	R=R0*NU1;
	T=T0*NU2;
	u1 = Q*ell_r(R,EPSILON,0,ALPHA);
	u2 = Q*ell_r(R,EPSILON,T,ALPHA);
	b1 = M_PI*R*R*EPSILON*(ETA_CORE-ETA_SHELL);
	b2 = M_PI*(R+T)*(R*EPSILON+T)*(ETA_SHELL-ETA_SOLV);

	if (u1==0) {
		Ain = 1.0;
	} else {
		Ain = 2.0*gsl_sf_bessel_J1(u1)/u1;
	}

	if (u2==0) {
		Aout = 1.0;
	} else {
		Aout = 2.0*gsl_sf_bessel_J1(u2)/u2;
	}

	A = b1*Ain+b2*Aout;
	return gsl_pow_int(A,lround(P));
}

int pcs_ellcylsh_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	int ndimtest;

	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0] = 0;
	ndimtest=0;

    if (EPSILON != 1) {
        ALPHA=x[ndimtest];
        ndimtest++;
        if (ndimtest > ndim) {
            sasfit_out("false dimensions ndim:%d\n",ndim);
            return 1;
        }
	} else {
	    ALPHA = M_PI_2;
	}

	if (SIGMA_R0!=0) {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R0;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU1=x[ndimtest];
		LNDISTR1 = sasfit_sd_LogNorm(x[ndimtest], &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R0);
			return 1;
		}
        ndimtest++;
        if (ndimtest > ndim) {
            sasfit_out("false dimensions ndim:%d\n",ndim);
            return 1;
        }
	} else {
		NU1=1;
		LNDISTR1=1;
	}

	if (SIGMA_T0!=0) {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_T0;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU2=x[ndimtest];
		LNDISTR2 = sasfit_sd_LogNorm(x[ndimtest], &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_T0);
			return 1;
		}
        ndimtest++;
        if (ndimtest > ndim) {
            sasfit_out("false dimensions ndim:%d\n",ndim);
            return 1;
        }
	} else {
		NU2=1;
		LNDISTR2=1;
	}
    if (ndimtest != ndim) {
        sasfit_out("false dimensions ndim:%d\n",ndim);
        return 1;
    }
    fval[0] = LNDISTR1*LNDISTR2*(cparam->func)(param);
    return 0;
}
scalar sasfit_ff_pcs_ellcylsh(scalar q, sasfit_param * param)
{
    scalar err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    int intstrategy, ndim;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 < 0.0), param, "R(%lg) < 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((T0 < 0.0), param, "t(%lg) < 0",T0); // modify condition to your needs

	// insert your code here
	Q=q;
	P=2;

	// insert your code here
	//return ThinCylShell(q,param);
    ndim = 0;
	if (SIGMA_R0==0 && SIGMA_T0==0 && EPSILON == 1) {
        NU1=1;
        NU2=1;
        ALPHA=M_PI_2;
        return pcs_ellcylsh(param);
	}
	if (EPSILON != 1) {
        cubxmin[ndim]=0;
        cubxmax[ndim]=M_PI_2;
        ndim++;
	}
	if (SIGMA_R0!=0) {
        find_LogNorm_int_range(4,1,SIGMA_R0,&cubxmin[ndim],&cubxmax[ndim],param);
        ndim++;
	}
	if (SIGMA_T0!=0) {
        find_LogNorm_int_range(4,1,SIGMA_T0,&cubxmin[ndim],&cubxmax[ndim],param);
        ndim++;
	}

    P=2;
    Q=q;
	cparam.ndim=ndim;
	cparam.func = &pcs_ellcylsh;
    cparam.param = param;
    intstrategy = sasfit_get_int_strategy();

    if (ndim>=3) {
        intstrategy=H_CUBATURE;
    } else {
        intstrategy=P_CUBATURE;
    }

    intstrategy = sasfit_get_int_strategy();

	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &pcs_ellcylsh_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &pcs_ellcylsh_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
			pcubature(1, &pcs_ellcylsh_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    }
    return sum;
}

scalar sasfit_ff_pcs_ellcylsh_f(scalar q, sasfit_param * param)
{
	scalar Pcs,Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q=q;
    P=1;

	return 0;
}

scalar sasfit_ff_pcs_ellcylsh_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

