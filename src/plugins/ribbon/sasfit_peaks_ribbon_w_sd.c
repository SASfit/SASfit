/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_peaks.h>
#include <math.h>
#include <complex.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define SIGMA_A	param->p[1]
#define B	param->p[2]
#define SIGMA_B	param->p[3]
#define C	param->p[4]
#define DUMMY	param->p[5]
#define ETA	param->p[6]

scalar sasfit_peaks_ribbon_w_sd(scalar q, sasfit_param * param)
{
    scalar rod, xs;
    sasfit_param subParam;
    scalar Res3F2, pFq_r, pFq_i, ResA,RgA2, RgB2;
	scalar pr[3],pi[3],qr[2], qi[2], zi, zr;
	int ln_pFq, ix, nsigfig, ip, iq;
    double complex ctmp;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "A(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_A <= 0.0), param, "sigma_a(%lg) <= 0",SIGMA_A); // modify condition to your needs
	// SASFIT_CHECK_COND2((A <= sqrt(2)*SIGMA_A), param, "A<=sqrt(2)sigma_a(%lg)",A, SIGMA_A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "B(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_B < 0.0), param, "sigma_B(%lg) < 0",SIGMA_B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "C(%lg) < 0",C); // modify condition to your needs

	// insert your code here

	subParam.p[0] = C;
	subParam.p[3] = ETA;
	rod = sasfit_ff_rod(q,&subParam);

	ln_pFq = 0;
	ix = 0;
	nsigfig = 10;
	ip = 3;
	iq = 2;

	pi[0] = 0;
	pr[0] = 0.5;
	pi[1] = 0;
	pr[1] = 1+0.5*gsl_pow_2(B/SIGMA_B);
	pi[2] = 0;
	pr[2] = 1.5+0.5*gsl_pow_2(B/SIGMA_B);
	qi[0] = 0;
	qr[0] = 1.5;
	qi[1] = 0;
	qr[1] = 2;

	zi=0;
	zr=-gsl_pow_2(q*SIGMA_B*SIGMA_B/B);

	RgB2 = gsl_pow_4(B)+5*gsl_pow_2(B*SIGMA_B)+6*gsl_pow_4(SIGMA_B);
	RgB2 = RgB2/(8*B*B);
	RgA2 = gsl_pow_4(A)+5*gsl_pow_2(A*SIGMA_A)+6*gsl_pow_4(SIGMA_A);
	RgA2 = RgA2/(4*A*A);
	if (sqrt(q*q*RgB2) > 100) {
        Res3F2 = 2*B/(q*(B*B+SIGMA_B*SIGMA_B));
/*
        Res3F2 = ((2*B*gsl_pow_int(q,2))/(gsl_pow_int(B,2) + gsl_pow_int(SIGMA_B,2)) +
     (gsl_pow_int(B/q,gsl_pow_int(B,2)/gsl_pow_int(SIGMA_B,2))*
        ((2*B*gsl_sf_gamma(1 - gsl_pow_int(B,2)/(2.*gsl_pow_int(SIGMA_B,2)))*
             gsl_sf_gamma((1 + gsl_pow_int(B,2)/gsl_pow_int(SIGMA_B,2))/2.))/
           (gsl_pow_int(SIGMA_B,2)*(gsl_pow_int(B,2) + 2*gsl_pow_int(SIGMA_B,2))) +
          (q*gsl_sf_gamma(1 + gsl_pow_int(B,2)/(2.*gsl_pow_int(SIGMA_B,2)))*
             gsl_sf_gamma(-(gsl_pow_int(B,2) + gsl_pow_int(SIGMA_B,2))/(2.*gsl_pow_int(SIGMA_B,2))))/
           (gsl_pow_int(B,2) + gsl_pow_int(SIGMA_B,2)))*
        sin((gsl_pow_int(B,2)*M_PI)/gsl_pow_int(SIGMA_B,2)))/
      (gsl_pow_int(M_PI,1.5)*gsl_pow_int(SIGMA_B,(2*gsl_pow_int(B,2))/gsl_pow_int(SIGMA_B,2))))/
   gsl_pow_int(q,3);
*/
	} else if ((q*sqrt(RgB2)) < 1e-2) {
	    Res3F2 = 1  -q*q*(B*B+2*SIGMA_B*SIGMA_B)
                        *(B*B+3*SIGMA_B*SIGMA_B)/(24*gsl_pow_2(B))
                    +gsl_pow_4(q)*(B*B+2*SIGMA_B*SIGMA_B)
                                 *(B*B+3*SIGMA_B*SIGMA_B)
                                 *(B*B+4*SIGMA_B*SIGMA_B)
                                 *(B*B+5*SIGMA_B*SIGMA_B)/(960.0*gsl_pow_4(B));
	} else {
	    Res3F2 = sasfit_pfq(pr,pi,qr,qi,ip,iq,zr,zi,ln_pFq,ix,&pFq_r,&pFq_i,nsigfig,param);
	    if (param->errStatus == TRUE) {
            param->errStatus = FALSE;
            Res3F2 = 2*B/(q*(B*B+SIGMA_B*SIGMA_B));
	    }
	}
	if (q*sqrt(RgA2) < 1e-4) {
        ResA = (A*A+SIGMA_A*SIGMA_A);
    } else {
        ctmp = 1 + I*q*gsl_pow_2(SIGMA_A)/A;
        ctmp=cpow(ctmp,-gsl_pow_2(A/SIGMA_A));
        ResA=2*(1-creal(ctmp))/(q*q);
    }
	xs = Res3F2*ResA*(B*B+SIGMA_B*SIGMA_B);
	return rod*xs;
}

scalar sasfit_peaks_ribbon_w_sd_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peaks_ribbon_w_sd_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

