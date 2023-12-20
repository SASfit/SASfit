/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RADIUS      param->p[0]
#define DR_RATIO	param->p[1]

#define A	        param->p[4]
#define DA_RATIO	param->p[5]
#define VERSION_SQ	param->p[6]
#define LL	        param->p[7]
#define SIGMA_L	    param->p[8]
#define Q       param->p[MAXPAR-1]
#define N       param->p[MAXPAR-2]
#define FMOD    param->p[MAXPAR-3]
#define F2MOD   param->p[MAXPAR-4]
#define SQ      param->p[MAXPAR-5]
#define DRADIUS (RADIUS*DR_RATIO)

// [1] Penttilä, P. A., Rautkari, L., Österberg, M., & Schweins, R.
//     J. Appl. Cryst. (2019). 52, 369–377 https://doi.org/10.1107/S1600576719002012
// [2] Hashimoto, T., Kawamura, T., Harada, M., & Tanaka, H. .
//     Macromolecules (1994), 27, 3063-3072. https://doi.org/10.1021/ma00089a025

scalar sasfit_ff_paracryst_hex_cyl_rnd(scalar q, sasfit_param * param)
{
    scalar Fmod, F2mod, Xstart, Xend, Pprime;
    sasfit_param subParam;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RADIUS < 0.0), param, "R(%lg) < 0",RADIUS); // modify condition to your needs
	SASFIT_CHECK_COND1((DR_RATIO < 0.0), param, "dR_ratio(%lg) < 0",DR_RATIO); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((DA_RATIO < 0.0), param, "da_ratio(%lg) < 0",DA_RATIO); // modify condition to your needs
	SASFIT_CHECK_COND1((LL < 0.0), param, "L(%lg) < 0",LL); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_L < 0.0), param, "sigma_L(%lg) < 0",SIGMA_L); // modify condition to your needs

	// insert your code here

	Q = q;

	subParam.p[0] = LL;
	subParam.p[2] = SIGMA_L;
	Pprime = sasfit_sq_p__q___rod(q,&subParam);
    return   sasfit_ff_paracryst_hex_cyl_perp(q,param)*Pprime;
}

scalar sasfit_ff_paracryst_hex_cyl_rnd_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_paracryst_hex_cyl_rnd_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

