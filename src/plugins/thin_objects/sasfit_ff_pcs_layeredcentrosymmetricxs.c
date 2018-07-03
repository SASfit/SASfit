/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define LC	param->p[0]
#define SIGMA_LC	fabs(param->p[1])
#define LSH	param->p[2]
#define SIGMA_LSH	fabs(param->p[3])
#define ETA_C	param->p[4]
#define ETA_SH	param->p[5]
#define ETA_SOL	param->p[6]

scalar layeredcentrosymmetric_FF(sasfit_param *param)
{
	scalar Fc, Fsh, Pcs, u, v, SH, LL;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);
    LL=NU1*LC;
    SH=NU2*LSH;
	u = LL*Q/2.0;
	v = (LL+2*SH)*Q/2.0;

	Fc = (ETA_SH-ETA_SOL)*(LL+2.0*SH)*gsl_sf_bessel_j0(v);

	Fsh = (ETA_SH-ETA_C)*LL*gsl_sf_bessel_j0(u);;

	Pcs = gsl_pow_int(Fc-Fsh,lround(P));

	return Pcs;

}
scalar layeredcentrosymmetric_core(scalar x, sasfit_param *param)
{
	scalar Fc, Fsh, Pcs, LNdistr, u, v;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	NU2 = x/LSH;


	Pcs = layeredcentrosymmetric_FF(param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_LSH;
	subParam.p[2] = 1.0;
	subParam.p[3] = LSH;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar layeredcentrosymmetric_SH(scalar x, sasfit_param * param)
{
	scalar SHstart = 0.0, SHend = 0.0;
	scalar Pcs;

	scalar LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	if (SIGMA_LC == 0) {
		NU1=1;
		LNdistr = 1;
	} else {
		NU1 = x/LC;
		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LC;
		subParam.p[2] = 1.0;
		subParam.p[3] = LC;
		LNdistr = sasfit_sd_LogNorm(x, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
	}

	if (SIGMA_LSH == 0.0) {
		NU2=1;
		Pcs = layeredcentrosymmetric_FF(param);
	} else {
		find_LogNorm_int_range(2,LSH,SIGMA_LSH,&SHstart,&SHend,param);
		Pcs = sasfit_integrate(SHstart, SHend, &layeredcentrosymmetric_core, param);
	}


	return LNdistr*Pcs;
	return Pcs;
}

scalar layeredcentrosymmetric_L(scalar q, sasfit_param * param)
{
	scalar Lstart = 0.0, Lend = 0.0;
	scalar Pcs;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if (SIGMA_LC == 0.0) {
		NU1=1;
		Pcs = layeredcentrosymmetric_SH(q,param);
	} else {
		find_LogNorm_int_range(2,LC,SIGMA_LC,&Lstart,&Lend,param);
		Pcs = sasfit_integrate(Lstart, Lend, &layeredcentrosymmetric_SH, param);
	}

	return Pcs;
}

int layeredcentrosymmetricxs_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	NU1 = 1;
	LNDISTR1=1.0;
	NU2 = 1;
	LNDISTR2=1.0;
	if ((ndim == 1) && (SIGMA_LC==0)) {
		LNDISTR1 = 1;
		NU1 = 1;
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LSH;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU2=x[0];
		LNDISTR2 = sasfit_sd_LogNorm(NU2, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
	} else if ((ndim == 1) && (SIGMA_LSH==0)) {
        NU2 = 1;
        LNDISTR2=1.0;
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LC;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU1=x[0];
		LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LC;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU1 = x[0];
		LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LSH;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU2=x[1];
		LNDISTR2 = sasfit_sd_LogNorm(NU2, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
	}
    fval[0] = LNDISTR1*LNDISTR2*(cparam->func)(param);
    return 0;
}

scalar sasfit_ff_pcs_layeredcentrosymmetricxs(scalar q, sasfit_param * param)
{

	scalar res,err,sum, Lmin, Lmax;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, ndim;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LC < 0.0), param, "t(%lg) < 0",LC); // modify condition to your needs
	SASFIT_CHECK_COND1((LSH < 0.0), param, "t(%lg) < 0",LSH); // modify condition to your needs

	// insert your code here
	P=2;
	Q=q;

	NU1 = 1;
	LNDISTR1=1.0;
	NU2 = 1;
	LNDISTR2=1.0;
	ndim  =2;
	if (SIGMA_LC==0 && SIGMA_LSH==0) {
		ndim=0;
		NU1 = 1;
		LNDISTR1 = 1;
		NU2 = 1;
		LNDISTR2 = 1;
		return layeredcentrosymmetric_FF(param);
	}
	if (SIGMA_LC!=0 && SIGMA_LSH!=0) {
		ndim=2;
		find_LogNorm_int_range(2,1,SIGMA_LC, &cubxmin[0],&cubxmax[0],param);
		find_LogNorm_int_range(2,1,SIGMA_LSH,&cubxmin[1],&cubxmax[1],param);
	} else if (SIGMA_LC!=0 && SIGMA_LSH==0) {
		ndim=1;
		find_LogNorm_int_range(2,1,SIGMA_LC,&cubxmin[0],&cubxmax[0],param);
	} else if (SIGMA_LC==0 && SIGMA_LSH!=0) {
		ndim=1;
		find_LogNorm_int_range(2,1,SIGMA_LSH,&cubxmin[0],&cubxmax[0],param);
	}



	cparam.ndim=ndim;
	cparam.func = &layeredcentrosymmetric_FF;
    cparam.param = param;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &layeredcentrosymmetricxs_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &layeredcentrosymmetricxs_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
            sum=layeredcentrosymmetric_L(q,param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_pcs_layeredcentrosymmetricxs_f(scalar q, sasfit_param * param)
{

	scalar res,err,sum, Lmin, Lmax;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, ndim;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	P=1;
	Q=q;

	NU1 = 1;
	LNDISTR1=1.0;
	NU2 = 1;
	LNDISTR2=1.0;
	ndim  =2;
	if (SIGMA_LC==0 && SIGMA_LSH==0) {
		ndim=0;
		NU1 = 1;
		LNDISTR1 = 1;
		NU2 = 1;
		LNDISTR2 = 1;
		return layeredcentrosymmetric_FF(param);
	}
	if (SIGMA_LC!=0 && SIGMA_LSH!=0) {
		ndim=2;
		find_LogNorm_int_range(2,1,SIGMA_LC, &cubxmin[0],&cubxmax[0],param);
		find_LogNorm_int_range(2,1,SIGMA_LSH,&cubxmin[1],&cubxmax[1],param);
	} else if (SIGMA_LC!=0 && SIGMA_LSH==0) {
		ndim=1;
		find_LogNorm_int_range(2,1,SIGMA_LC,&cubxmin[0],&cubxmax[0],param);
	} else if (SIGMA_LC==0 && SIGMA_LSH!=0) {
		ndim=1;
		find_LogNorm_int_range(2,1,SIGMA_LSH,&cubxmin[0],&cubxmax[0],param);
	}



	cparam.ndim=ndim;
	cparam.func = &layeredcentrosymmetric_FF;
    cparam.param = param;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &layeredcentrosymmetricxs_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &layeredcentrosymmetricxs_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
            sum=layeredcentrosymmetric_L(q,param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_pcs_layeredcentrosymmetricxs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

