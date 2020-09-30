/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ALPHA	param->p[1]
#define DETA	param->p[2]

scalar sasfit_ff_power_law(scalar q, sasfit_param * param)
{
	doublereal Res1F2;
	gsl_sf_result gslres;
	int status;
	double p_r[1], p_i[1], q_r[2], q_i[2], z_i, z_r, ret_r, ret_i, lnr_r, lnr_i, qR, Rg, tmp;
	integer ln_pFq, ix, nsigfig, ip, iq, errstr_len = STRLEN;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	// insert your code here

    gsl_set_error_handler_off();

	ln_pFq = 0;

	ix = 0;
	nsigfig = 10;
	ip = 1;
	iq = 2;

	p_i[0] = 0;
	p_r[0] = 1.5+ALPHA/2.;

	q_i[0] = 0;
	q_r[0] = 1.5;
	q_i[1] = 0;
	q_r[1] = 2.5+0.5*ALPHA;

	z_i = 0;
	z_r = -0.25*gsl_pow_2(R*q);

	// I assume ierr is just a flag here ?
    qR = q*R;
    Rg = R*sqrt(3.*(3. + ALPHA)/(5.*(5. + ALPHA)));
	if (q*Rg < 1e-3)  {
        return DETA*DETA*(16*gsl_pow_2(ALPHA*M_PI)*(5 - ((3 + ALPHA)*gsl_pow_2(qR))/(5 + ALPHA))*
              gsl_pow_6(R))/(45.*gsl_pow_2(3 + ALPHA));
	}
//	if (z_r < -1e4) {
    if (q*Rg > 300.0) {
        status = gsl_sf_gamma_e(2 + ALPHA,&gslres);
        if (status==0) {
            return DETA*DETA*(16*gsl_pow_2(M_PI)*((gsl_pow_2(q*gslres.val*sin((ALPHA*M_PI)/2.))*pow(qR,-ALPHA))
                                         - (2*ALPHA*gslres.val*sin((ALPHA*M_PI)/2.)*
                                            ((1 + ALPHA)*qR*cos(q*R) +
                                                (1 - gsl_pow_2(ALPHA) + gsl_pow_2(qR))*sin(qR)))*pow(qR,-ALPHA)/(gsl_pow_2(R))
                                        + (ALPHA*q*sin(qR)*(2*(-1 + ALPHA + 2*gsl_pow_2(ALPHA))*cos(qR)
                                                            + ALPHA*qR*sin(q*R))
                                           )/R)
                    )/gsl_pow_8(q);
        }
/*
        sasfit_out("WARNING GSL_SF_GAMMA: q:%lf, alpha%lf\tGSL_ERR text:%s\n",q,ALPHA,gsl_strerror(status));
        return 4*gsl_pow_2((2*ALPHA*(1 + ALPHA)*M_PI*(2 + (-3 + ALPHA)*ALPHA - gsl_pow_2(qR))*cos(qR))/
      (gsl_pow_3(R)*gsl_pow_6(q)) + ((115915 + 84*ALPHA*
           (-7243 + 48*ALPHA*(41 + 180*ALPHA*(25 + 24*ALPHA*(13 + 12*ALPHA)))))*pow(M_PI,1.5)*sin((ALPHA*M_PI)/2.))*exp(-ALPHA*(1 + log((qR)/ALPHA)))/
      (5.225472e7*sqrt(2)*pow(ALPHA,3.5)*gsl_pow_3(q)) +
     (2*ALPHA*M_PI*(-1 + gsl_pow_2(ALPHA) - gsl_pow_2(qR))*sin(qR))/(gsl_pow_2(R)*gsl_pow_5(q)));
*/
        return gsl_pow_2(DETA*3*(sin(qR)-qR*cos(qR))/gsl_pow_3(qR)*sasfit_ff_power_law_v(q,param,0));
/*
        status = gsl_sf_lngamma_e(2 + ALPHA,&gslres);
        if (status!=0) {
            sasfit_err("ERROR GSL_SF_LNGAMMA: q:%lf, alpha%lf\tGSL_ERR text:%s\n",q,ALPHA,gsl_strerror(status));
            return 1;
        }
        tmp = gslres.val;
        sasfit_out("lngamma(2+%lg)=%lg\t2 tmp-2 alpha log(qR)=%lg\n",ALPHA,tmp,2*tmp-2*ALPHA*log(qR));

        return -8*gsl_pow_2(M_PI)*(-1 + cos(ALPHA*M_PI))*exp(2*tmp-2*ALPHA*log(qR))/(gsl_pow_6(q));
        return (16*gsl_pow_2(M_PI)*((exp(2*tmp-2*ALPHA*log(qR))*gsl_pow_2(q)*gsl_pow_2(sin((ALPHA*M_PI)/2.)))
                                    - (2*ALPHA*exp(tmp-ALPHA*log(qR))*sin((ALPHA*M_PI)/2.)*(
                                                    (1 + ALPHA)*qR*cos(q*R) +  (1 - gsl_pow_2(ALPHA) + gsl_pow_2(qR))*sin(qR)
                                                                                       )
                                    )/(gsl_pow_2(R)) +
                                    (ALPHA*q*sin(qR)*(2*(-1 + ALPHA + 2*gsl_pow_2(ALPHA))*cos(qR) + ALPHA*qR*sin(q*R)))/R
                                    )
                )/gsl_pow_8(q);
*/
        ret_r=gsl_pow_5(1./qR)*(3 + ALPHA)*(qR*(ALPHA + gsl_pow_2(ALPHA) - gsl_pow_2(qR))*cos(qR) -
            exp(tmp+(2-ALPHA)*log(qR))*sin((ALPHA*M_PI)/2.) + (1 + ALPHA)*(ALPHA - gsl_pow_2(ALPHA) + qR*qR)*sin(qR));
        ret_i=0;
    } else {
        sasfit_pfq( p_r, p_i, q_r, q_i, ip, iq, z_r, z_i, ln_pFq, ix,  &lnr_r, &lnr_i, nsigfig,param);
        if (param->errStatus) sasfit_err("%s\n",param->errStr);
        if (ln_pFq==1) {
            ret_r = exp(lnr_r)*cos(lnr_i+(ln_pFq-1)*M_PI);
            ret_i = exp(lnr_r)*sin(lnr_i+(ln_pFq-1)*M_PI);
        } else {
            ret_r = lnr_r;
            ret_i = lnr_i;
        }
    }

	// is errstr_len written written with the final str length afterwards ?
	// param->errLen = errstr_len;
//	sasfit_out("q:%lf z: %lf r:%lf i:%lf >%s<\n",q,z_r,ret_r,ret_i,param->errStr);
	Res1F2 = gsl_pow_2(-4*M_PI*R*R*R*ret_r/(3+ALPHA)+4*M_PI/(gsl_pow_3(q))*(sin(q*R)-q*R*cos(q*R))) + gsl_pow_2(4*M_PI*R*R*R*ret_i/(3+ALPHA));
	return DETA*DETA*(Res1F2);
}

scalar sasfit_ff_power_law_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_power_law_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return (4.*gsl_pow_3(R)* ALPHA*M_PI)/(9 + 3*ALPHA);
}

