/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	    param->p[0]
#define SIGMA_R param->p[1]
#define H	    param->p[2]
#define SIGMA_H param->p[3]
#define L	    param->p[4]
#define SIGMA_L param->p[5]
#define ETA	    param->p[6]

#define Q       param->p[MAXPAR-1]
#define RCAP    param->p[MAXPAR-2]
#define P       param->p[MAXPAR-3]
#define ALPHA   param->p[MAXPAR-4]
#define RR      param->p[MAXPAR-5]
#define LL      param->p[MAXPAR-6]

scalar AcaptQ0(scalar t, sasfit_param *param) {
    return (1-t*t)/2;
}

scalar Acapt(scalar t, sasfit_param *param) {
    return cos(Q*cos(ALPHA)*(RCAP*t+H+LL/2.))*(1-t*t)*sasfit_jinc(Q*RCAP*sin(ALPHA)*sqrt(1-t*t));
}

scalar Acapped(const double *x, sasfit_param *param) {
    scalar Acyl, Acap, Vcyl;
    Vcyl=M_PI*RR*RR*LL;
    if (Vcyl==0 && RCAP==0) return 0.0;
    Acyl=Vcyl*gsl_sf_bessel_j0(Q*LL/2.*cos(ALPHA))*2*sasfit_jinc(Q*RR*sin(ALPHA));
    if (RCAP==0) {
        Acap = 0;
    } else if (-H/RCAP==1) {
        Acap = 0;
    } else {
        Acap=4*M_PI*RCAP*RCAP*RCAP*sasfit_integrate(-H/RCAP,1,&Acapt,param);
    }
    return gsl_pow_int(ETA*(Acyl+Acap),lround(P))*sin(ALPHA);
}

scalar Acap_cubature(const double *x, size_t ndim,  void *pam) {
    scalar LNR, LNL;
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	param = (sasfit_param *) pam;

    ALPHA=x[0];

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	RR=R*x[1];
	LNR = sasfit_sd_LogNorm(x[1], &subParam);

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_L;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	LL=L*x[2];
	LNL = sasfit_sd_LogNorm(x[2], &subParam);

	return LNR*LNL*Acapped(x,param);
}

scalar sasfit_ff_capped_cylinder(scalar q, sasfit_param * param)
{
    int ndim;
    scalar Rcap, res, err;
    scalar cubxmin[3], cubxmax[3];

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R);                     // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_R <= 0.0), param, "sigma_r(%lg) <= 0",SIGMA_R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_L <= 0.0), param, "sigma_L(%lg) <= 0",SIGMA_L); // modify condition to your needs

	// insert your code here
	RCAP = gsl_hypot(H,R);
	Q=q;
	P=2;
	ALPHA=0;
	ndim=3;
	cubxmin[0]=0; cubxmax[0]=M_PI_2;
	find_LogNorm_int_range(4,1,SIGMA_R,&cubxmin[1], &cubxmax[1], param);
	find_LogNorm_int_range(2,1,SIGMA_L,&cubxmin[2], &cubxmax[2], param);
	sasfit_cubature(3,cubxmin,cubxmax,&Acap_cubature,param,sasfit_eps_get_nriq(),&res, &err);
	return res;
//	return sasfit_integrate(0,M_PI_2,&Acapped, param);
}

scalar sasfit_ff_capped_cylinder_f(scalar q, sasfit_param * param)
{
    int ndim;
	scalar Rcap, res, err;
    scalar cubxmin[3], cubxmax[3];
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
	RCAP = gsl_hypot(H,R);
	Q=q;
	P=1;
	ALPHA=0;
	ndim=3;
	cubxmin[0]=0; cubxmax[0]=M_PI_2;
	find_LogNorm_int_range(4,1,SIGMA_R,&cubxmin[1], &cubxmax[1], param);
	find_LogNorm_int_range(2,1,SIGMA_L,&cubxmin[2], &cubxmax[2], param);
	sasfit_cubature(3,cubxmin,cubxmax,&Acap_cubature,param,sasfit_eps_get_nriq(),&res, &err);
	return res;
}

scalar sasfit_ff_capped_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*R*R*L+4*M_PI*gsl_pow_3(RCAP)*(1./3.-gsl_pow_3(H/RCAP)/6.+H/RCAP/2.);
}

