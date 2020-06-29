/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define SIGMA	fabs(param->p[1])
#define NN	param->p[2]
#define THETA_DEG param->p[3]
#define PHI_DEG param->p[4]
#define PSI_DEG param->p[5]
#define TN param->p[MAXPAR-3]
#define GAMMA param->p[MAXPAR-2]
#define Q param->p[MAXPAR-1]

scalar sasfit_sq_paracrystal_1d_random_tinySigma(scalar theta, void *pam) {
    scalar A, AA, BB;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	if (D*Q*cos(theta)<1e-4) {
        return TN/720.*(720-60*(TN*TN-1)*gsl_pow_2(D*Q*cos(theta))+gsl_pow_4(D*Q*cos(theta))*(3-5*TN*TN+2*gsl_pow_4(TN)));
	}
	return (cos(D*TN*Q*cos(theta))-1) / (TN*(cos(D*Q*cos(theta))-1));
}

scalar sasfit_sq_paracrystal_1d_random_tinySigma_sasfit(scalar theta, sasfit_param *param)
{
    return sasfit_sq_paracrystal_1d_random_tinySigma(theta, param);
}

scalar sasfit_sq_paracrystal_1d_random(scalar q, sasfit_param * param)
{
	scalar a, b, tmp, ferf,SQ, SQN[2], w, rw1, iw1, rw2, iw2, a2b;
	int i,imax,k,tn;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((NN < 1.0), param, "N(%lg) < 1",NN); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs

	// insert your code here

    w = NN - floor(NN);
    if (w==0) {
        imax = 0;
    } else {
        imax = 1;
    }
    TN = lround(NN);
    tn = TN;

    if (SIGMA < 0) {
        Q = q;
        SQ = sasfit_integrate(0,M_PI_2,&sasfit_sq_paracrystal_1d_random_tinySigma_sasfit,param);
        TN++;
        return SQ*(1-w)+w*sasfit_integrate(0,M_PI_2,&sasfit_sq_paracrystal_1d_random_tinySigma_sasfit,param);
    }
    for (i=0;i<=imax;i++) {
        TN=TN+i;
        SQ = 1.0;
        tmp = gsl_pow_2(q*SIGMA)/2.;

        for (k=1; k < tn ;k++) {
            b = k*tmp;
            a = k*D*q;
            ferf=-(tn-k)*exp(-a*a/(4*b))*sqrt(M_PI)*
		          (im_cerfi_z(a/(2*sqrt(b)),-sqrt(b))
				  -im_cerfi_z(a/(2*sqrt(b)), sqrt(b))
				  )/(4*sqrt(b));
            if (!gsl_finite(ferf)) {
                a2b = a/(2*sqrt(b));
                w_of_z_e(-a2b,-sqrt(b), &rw1, &iw1);
                w_of_z_e(-a2b, sqrt(b), &rw2, &iw2);
                ferf = (tn-k)*exp(-b)*sqrt(M_PI)/(4*sqrt(b))*(-(cos(a)*rw1+sin(a)*iw1)+(cos(a)*rw2-sin(a)*iw2));
            }
//        sasfit_out("a:%lg, b:%lg, a/2sqrt(b),:%lf Q:%lg\n",a,b,a2b, q);
            if (!gsl_finite(ferf)) {
//            sasfit_out("errrrrrr: a:%lg, b:%lg, a/2sqrt(b):%lf Q:%lg, k:%ld\n",a,b,a2b, q,k);
/********** Below is an approximation for SIGMA << 1 ******************************************************/
                if (fabs(SIGMA) < 1e-2) {
                    ferf = ((tn - k)*(2*k*D*q*gsl_pow_2(SIGMA)*cos(k*D*q) +
                            (-2*gsl_pow_2(SIGMA) +
                            k*gsl_pow_2(D)*(-2 + k*gsl_pow_2(q*SIGMA)))
                            *sin(k*D*q)))/(2.*k*k*gsl_pow_3(D)*q) ;
/********** Above is an approximation for SIGMA << 1 ******************************************************/
/********** Below is an approximation for a/(2*sqrt(b)) -> Infinity ***************************************/
                } else if (a2b > 1e8) {
                    ferf = (tn - k)* (2*exp(-a2b*a2b)*sqrt(M_PI)+2*exp(-b)*(a2b*sin(a)-sqrt(b)*cos(a))/gsl_pow_2(a2b))/(4*sqrt(b));
/********** Above is an approximation for a/(2*sqrt(b)) -> Infinity ***************************************/
/********** Below is an approximation for a/(2*sqrt(b)) -> 0 **********************************************/
                } else if (a2b<1e-2) {
                    ferf = (tn - k)*((2-2*a2b*a2b+gsl_pow_4(a2b))*sqrt(M_PI)*gsl_sf_erf(sqrt(b))/(4*sqrt(b))
                            -(a2b*a2b/6.*(a2b*a2b*(3+2*b)*a2b*a2b-6)*exp(-b)));
/********** Above is an approximation for a/(2*sqrt(b)) -> 0 **********************************************/
/********** Below is an approximation for a -> Infinity ***************************************************/
                } else  if (a>1e8) {
                    ferf = (tn - k)*(-exp(-a2b*a2b)*sqrt(M_PI)/(2*sqrt(b)+exp(-b)/(gsl_pow_4(a))*(a*(a*a+2*(1-2*b)*b)*sin(a)-2*b*(a*a+2*(3-2*b)*b)*cos(a))));
/********** Above is an approximation for a -> Infinity ***************************************************/
                } else {
/********** Below is an approximation for b -> Infinity ********************************************/
                    ferf = (tn - k)/(768*gsl_pow_4(b))*(sqrt(b*M_PI)*(12*gsl_pow_4(a)*b-gsl_pow_6(a)-96*gsl_pow_2(a*b)+384*gsl_pow_3(b))
                                                +exp(-b)*48*( cos(a)*(15+2*a*a*(b-3)-6*b+4*b*b-8*gsl_pow_3(b))
                                                             -sin(a)*a*(a*a-15+6*b-4*b*b)));
/********** Above is an approximation for b -> Infinity ********************************************/
                }

            }
            SQ = SQ+2.0/tn * ferf;
//		SQ = SQ+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-a*gsl_pow_2(tmp*SIGMA)/2.0);
        }
        SQN[i] = SQ;
    }
	return (1-w)*SQN[0]+w*SQN[1];
}

scalar sasfit_sq_paracrystal_1d_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_paracrystal_1d_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

