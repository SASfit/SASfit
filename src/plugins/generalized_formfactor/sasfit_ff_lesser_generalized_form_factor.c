/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define A	param->p[0]
#define SIGMA_A	fabs(param->p[1])
#define DUMMY2  param->p[2]
#define DIM     param->p[3]
#define S	    param->p[4]
#define P	    param->p[5]
#define ETA	    param->p[6]

#define Q   	param->p[MAXPAR-1]
#define ALPHA	param->p[MAXPAR-2]
#define BETA	param->p[MAXPAR-3]
#define GAMA    param->p[MAXPAR-4]
#define NU  	param->p[MAXPAR-5]
#define F     	param->p[MAXPAR-6]

scalar lesser_gFF(scalar x, sasfit_param *param) {
    scalar m,ff2;
    ff2 = lround(F);
    if(x==0) {
        return 0.0;
    } else {
        m= ETA*pow(x,ALPHA);
    }
    return gsl_pow_int(pow(Q,-GAMA/2.)*m*pow(2,-NU)*gsl_sf_hyperg_0F1(NU+1,-gsl_pow_2(Q*x)/4.)/gsl_sf_gamma(NU+1),ff2);
}

scalar lesser_gFF_core(scalar x, sasfit_param *param) {
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = lesser_gFF(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_A;
	subParam.p[2] = 1.0;
	subParam.p[3] = A;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar sasfit_ff_lesser_generalized_form_factor(scalar q, sasfit_param * param)
{
    scalar gFF, Astart, Aend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((S < 0.0), param, "s(%lg) < 0",S); // modify condition to your needs
	SASFIT_CHECK_COND1((S > 3.0), param, "alpha(%lg) < 0",3-S); // modify condition to your needs
	// insert your code here
	Q = q;
	GAMA=S;
	BETA = P-S-1;
	ALPHA=DIM-S;
//    alpha=ALPHA;
	NU=BETA/2;
	F=2;

    if (SIGMA_A==0) {
        gFF=lesser_gFF(A,param);
    } else {
        find_LogNorm_int_range(2*DIM,A,SIGMA_A,&Astart,&Aend,param);
		gFF = sasfit_integrate(Astart, Aend, &lesser_gFF_core, param);
    }

	return gFF;
}

scalar sasfit_ff_lesser_generalized_form_factor_f(scalar q, sasfit_param * param)
{
    scalar gFF, Astart, Aend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    Q = q;
	GAMA=S;
	BETA = P-S-1;
	ALPHA=DIM-S;
//    alpha=ALPHA;
	NU=BETA/2;
	F=1;

    if (SIGMA_A==0) {
        gFF=lesser_gFF(A,param);
    } else {
        find_LogNorm_int_range(DIM,A,SIGMA_A,&Astart,&Aend,param);
		gFF = sasfit_integrate(Astart, Aend, &lesser_gFF_core, param);
    }

	return gFF;
}

scalar sasfit_ff_lesser_generalized_form_factor_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	ALPHA=DIM-S;
	return pow(A,ALPHA);
}

