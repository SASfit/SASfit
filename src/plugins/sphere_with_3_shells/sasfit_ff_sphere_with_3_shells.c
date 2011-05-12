/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_CORE	param->p[0]
#define T_SH1	param->p[1]
#define T_SH2	param->p[2]
#define T_SH3	param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SH1	param->p[5]
#define ETA_SH2	param->p[6]
#define ETA_SH3	param->p[7]
#define ETA_SOL	param->p[8]

scalar sasfit_ff_sphere_with_3_shells(scalar q, sasfit_param * param)
{
	scalar F;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_CORE < 0.0), param, "R_core(%lg) < 0",R_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((T_SH1 < 0.0), param, "t_sh1(%lg) < 0",T_SH1); // modify condition to your needs
	SASFIT_CHECK_COND1((T_SH2 < 0.0), param, "t_sh2(%lg) < 0",T_SH2); // modify condition to your needs
	SASFIT_CHECK_COND1((T_SH3 < 0.0), param, "t_sh3(%lg) < 0",T_SH3); // modify condition to your needs

	// insert your code here
	F=sasfit_ff_sphere_with_3_shells_f(q,param);
	return gsl_pow_2(F);
}

scalar SF(scalar q,scalar r) {
	scalar qr;
	qr=q*r;
	if (qr == 0.0) {
		return 4./3. * M_PI * gsl_pow_3(r);
	} else {
		return 4./3. * M_PI * gsl_pow_3(r) * 3.0*(sin(q*r)-qr*cos(q*r))/gsl_pow_3(q*r);
	}
}

scalar sasfit_ff_sphere_with_3_shells_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	return   (ETA_CORE-ETA_SOL)* SF(q,R_CORE)
		   + (ETA_SH1-ETA_SOL) *(SF(q,R_CORE+T_SH1)            -SF(q,R_CORE))
		   + (ETA_SH2-ETA_SOL) *(SF(q,R_CORE+T_SH1+T_SH2)      -SF(q,R_CORE+T_SH1))
		   + (ETA_SH3-ETA_SOL) *(SF(q,R_CORE+T_SH1+T_SH2+T_SH3)-SF(q,R_CORE+T_SH1+T_SH2));
}

scalar sasfit_ff_sphere_with_3_shells_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./3. * M_PI * gsl_pow_3(R_CORE+T_SH1+T_SH2+T_SH3);
}

