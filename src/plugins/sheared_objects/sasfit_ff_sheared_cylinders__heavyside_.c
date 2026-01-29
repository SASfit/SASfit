/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables


scalar sasfit_ff_sheared_cylinders__heavyside_(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000, ierr;
	cubature_param cparam;
 // return pHeavysidePi(q,0,param);

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);


	Q = q;		// Q
	P = 2.0;	// pow
	if (SIGMA==0) {
		ndim=2;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim =3;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}

    if (KAPPA == 0 || fabs(KAPPA) <= 1./M_PI_2) {
        cubxmin[0]=0;
        cubxmax[0]=M_PI_2;
    } else if (KAPPA > 0 && KAPPA < 1./M_PI_2) {
        cubxmin[0]=0;
        cubxmax[0]=1./KAPPA;
    } else if (KAPPA < 0 && KAPPA > -1./M_PI_2){
        cubxmin[0]=-1./KAPPA;
        cubxmax[0]=M_PI_2;
    } else {
        cubxmin[0]=0;
        cubxmax[0]=M_PI_2;
    }
//	cubxmin[0]=0;
//	cubxmax[0]=M_PI_2;
	cubxmin[1]=0;
	cubxmax[1]=M_PI;
	cubxmin[2]=NUMIN;
	cubxmax[2]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &alignedCylShell;
	cparam.gam = &gamOthers;
	cparam.p1 = &pHeavysidePi;

    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&partly_aligned_sasfit_cubature,&cparam,sasfit_eps_get_aniso(), &sum,&err);
    return sum;
}

scalar sasfit_ff_sheared_cylinders__heavyside__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_sheared_cylinders__heavyside__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

