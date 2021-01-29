/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables


scalar Lambda(scalar qb, sasfit_param *param) {
    scalar Lbda;
	if (fabs(qb<1e-6)) {
		Lbda = 1 - gsl_pow_2(qb)/18. + gsl_pow_4(qb)/600. - gsl_pow_6(qb)/35280. + gsl_pow_8(qb)/3265920.;
	} else {
		Lbda = gsl_sf_Si(qb)/qb;
	}
	return Lbda;
}

scalar LambdaExt(scalar qR, scalar qL, sasfit_param *param) {
    scalar Lbda;
	if (fabs(qL<1e-6)) {
		if (fabs(qR<1e-6)) {
//			Lbda = 1 - gsl_pow_2(qL)/18. - (qL*qR)/6. + ((-10 + gsl_pow_2(qL))*gsl_pow_2(qR))/60.;
			Lbda = (180*gsl_pow_3(qL)*qR*(42 - 5*gsl_pow_2(qR)) + 15120*qL*qR*(-10 + gsl_pow_2(qR)) +
                        7560*(120 - 20*gsl_pow_2(qR) + gsl_pow_4(qR)) -
                        180*gsl_pow_2(qL)*(280 - 84*gsl_pow_2(qR) + 5*gsl_pow_4(qR)) +
                        gsl_pow_4(qL)*(1512 - 540*gsl_pow_2(qR) + 35*gsl_pow_4(qR)))/907200.;
		} else {
//			Lbda = (qL*qR*(-2*qL + 3*qR)*cos(qR) + (-3*qL*qR + 6*gsl_pow_2(qR) - gsl_pow_2(qL)*(-2 + gsl_pow_2(qR)))*sin(qR))/(6.*gsl_pow_3(qR));
			Lbda = (qL*qR*(-40*qL*gsl_pow_2(qR) + 60*gsl_pow_3(qR) + 4*gsl_pow_3(qL)*(-6 + gsl_pow_2(qR)) -
                        5*gsl_pow_2(qL)*qR*(-6 + gsl_pow_2(qR)))*cos(qR) +
                        (-60*qL*gsl_pow_3(qR) + 120*gsl_pow_4(qR) + 15*gsl_pow_3(qL)*qR*(-2 + gsl_pow_2(qR)) -
                        20*gsl_pow_2(qL)*gsl_pow_2(qR)*(-2 + gsl_pow_2(qR)) +
                        gsl_pow_4(qL)*(24 - 12*gsl_pow_2(qR) + gsl_pow_4(qR)))*sin(qR))/(120.*gsl_pow_5(qR));
		}
	} else {
		Lbda = (gsl_sf_Si(qL+qR)-gsl_sf_Si(qR))/qL;
	}
	return Lbda;
}

scalar Psi(scalar x) {
    if (fabs(x)<=1e-6) {
        return 1.-x*x/10.+gsl_pow_4(x)/240.-gsl_pow_6(x)/15120.;
	} else {
        return 3.*(sin(x)-x*cos(x))/gsl_pow_3(x);
	}
}
