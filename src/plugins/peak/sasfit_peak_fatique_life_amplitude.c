/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_min.h>


// define shortcuts for local parameters/variables
#define AMPLITUDE	param->p[0]
#define X0	param->p[1]
#define DUMMY	param->p[2]
#define BETA	param->p[3]
#define GAMMA	param->p[4]
#define BACKGR	param->p[5]

scalar peak_fatique_life_area(scalar x, void *pam) {
    sasfit_param *param;
    param = (sasfit_param *) pam;
    return sasfit_peak_fatique_life_area(x,param);
}

scalar fatique_mode(sasfit_param * param) {
    scalar alpha,a1,a2,a3,a4,a5,a6, mode;
    scalar t1,t2,t6,t12,t16,t19,t25;

    sasfit_param tmpparam;
    scalar mean, variance, Xmin, Xmax;
    int status;
    int iter = 0, max_iter = 100;
    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    gsl_function F;
    alpha=GAMMA;

    t1 = gsl_pow_2(GAMMA);
    t2 = t1 * t1;
    t6 = t2 * t1;
    a1 = -0.27e2 * t6 + 0.276e3 * t2 - 0.192e3 * t1 + 0.192e3;
    if (a1 >= 0) {
        t12 = sqrt(a1);
        a2 = 0.12e2 * t12 * GAMMA + 0.48e2 * t1 + 0.84e2 * t2 - 0.8e1 * t6 + 0.64e2;
        if (a2>=0) {
            t16 = pow(a2, 0.1e1 / 0.3e1);
            t19 = t16 * t16;
            t25 = 0.1e1 / t16 * (-0.2e1 * t16 * t1 - 0.28e2 * t1 - 0.2e1 * t16 + t19 + 0.4e1 * t2 + 0.16e2) / 0.6e1;
            return t25;
        } else {
            sasfit_out("a2 <0: a1:%lg\n",a2);
        }
    } else {
        sasfit_out("a1 <0: a1:%lg\n",a1);
    }

    gsl_set_error_handler_off();
    F.function = &peak_fatique_life_area;

    tmpparam.p[0] = -1.0;
    tmpparam.p[1] = 0.0;
    tmpparam.p[2] = 0.0;
    tmpparam.p[3] = 1.0;
    tmpparam.p[4] = param->p[4];
    tmpparam.p[5] = 0.0;
    F.params = &tmpparam;

    gsl_set_error_handler_off();

    T = gsl_min_fminimizer_brent;
//    T = gsl_min_fminimizer_quad_golden;
    s = gsl_min_fminimizer_alloc (T);
    mean = BETA*(1+GAMMA*GAMMA/2.);
    variance = gsl_pow_2(BETA*GAMMA)*(1+2.5*GAMMA*GAMMA);
    Xmax=mean+X0;
    Xmin=X0;
    mode=(Xmax+Xmin)/2.;
    gsl_min_fminimizer_set (s, &F, mode, Xmin, Xmax);

    do {
      iter++;
      status = gsl_min_fminimizer_iterate (s);

      mode = gsl_min_fminimizer_x_minimum (s);
      Xmin = gsl_min_fminimizer_x_lower (s);
      Xmax = gsl_min_fminimizer_x_upper (s);

      status
        = gsl_min_test_interval (Xmin, Xmax, 1e-6*variance, 0.0);
//      sasfit_out("iter: %d mode:%lf\n",iter, mode);

    } while (status == GSL_CONTINUE && iter < max_iter);

    gsl_min_fminimizer_free (s);

    if (status == GSL_SUCCESS) return mode;
    sasfit_out("could not find mode for fartique life distribution\n");


 /*

    a3 = (4 - 4*pow(alpha,2) + (Complex(0,2)*(Complex(0,1) + Sqrt(3))*(4 + 7*Power(alpha,2) + Power(alpha,4)))/
         Power(-8 + 6*Power(alpha,2) - (21*Power(alpha,4))/2. - Power(alpha,6) +
           (3*Sqrt(3)*Sqrt(-(Power(alpha,2)*(64 + 64*Power(alpha,2) + 92*Power(alpha,4) + 9*Power(alpha,6)))))/2.,0.3333333333333333) -
        Power(2,0.6666666666666666)*(1 + Complex(0,1)*Sqrt(3))*Power(-16 + 12*Power(alpha,2) - 21*Power(alpha,4) - 2*Power(alpha,6) +
           3*Sqrt(3)*Sqrt(-(Power(alpha,2)*(64 + 64*Power(alpha,2) + 92*Power(alpha,4) + 9*Power(alpha,6)))),0.3333333333333333))/12.;
    if (mode <=0 && a3>0)
*/
}

scalar sasfit_peak_fatique_life_amplitude(scalar x, sasfit_param * param)
{
	scalar z, tmpAmpl, tmpBackgr,mode, res;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs

	// insert your code here
	z = (x-X0)/BETA;
	if (z<=0) return BACKGR;
    tmpBackgr=BACKGR;
    tmpAmpl = AMPLITUDE;
    BACKGR=0;
    AMPLITUDE = 1;
    mode=fatique_mode(param)*BETA+X0;
	res = tmpAmpl*sasfit_peak_fatique_life_area(x,param)/sasfit_peak_fatique_life_area(mode,param)+tmpBackgr;

    BACKGR=tmpBackgr;
    AMPLITUDE=tmpAmpl;
    return res;
}

scalar sasfit_peak_fatique_life_amplitude_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_peak_fatique_life_amplitude_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
