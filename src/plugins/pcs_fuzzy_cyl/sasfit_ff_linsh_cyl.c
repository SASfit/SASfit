/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA_R	fabs(param->p[1])
#define DR	param->p[2]
#define DUMMY3	param->p[3]
#define DUMMY4	param->p[4]
#define ETAC	param->p[5]
#define ETASHI	param->p[6]
#define ETASHO	param->p[7]
#define ETASOL	param->p[8]

#define Q	param->p[MAXPAR-1]
#define P	param->p[MAXPAR-2]


scalar linsh_cyl(scalar x, sasfit_param *param) {
    scalar Fcs;
    scalar QR, QRDR;
    if (DR==0) {
        Fcs = 2 *(ETAC - ETASOL) *M_PI *x*x *sasfit_jinc(Q*x);
        goto LRET;
    }
	if (Q==0) {
        Fcs = (DR*DR*(ETASHI + 2*ETASHO - 3*ETASOL)*M_PI)/3. + DR*(ETASHI + ETASHO - 2*ETASOL)*M_PI*x + (ETAC - ETASOL)*M_PI*x*x;
        goto LRET;
	}

	QR=Q*x;
	QRDR=Q*(x+DR);
	Fcs = (M_PI*(2*DR*((ETAC - ETASHI)*QR*gsl_sf_bessel_J1(QR) + (ETASHO - ETASOL)*QRDR*gsl_sf_bessel_J1(QRDR)) +
       (ETASHI - ETASHO)*M_PI*(-(x*gsl_sf_bessel_J1(QR)*sasfit_struve_h0(&QR)) + (DR + x)*gsl_sf_bessel_J1(QRDR)*sasfit_struve_h0(&QRDR)
                               + x*gsl_sf_bessel_J0(QR)*sasfit_struve_h1(&QR)  - (DR + x)*gsl_sf_bessel_J0(QRDR)*sasfit_struve_h1(&QRDR))))/
   (DR*gsl_pow_2(Q));
LRET: return gsl_pow_int(Fcs,lround(P));
}

scalar linsh_cyl_core(scalar x, sasfit_param *param) {
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = linsh_cyl(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar sasfit_ff_linsh_cyl(scalar q, sasfit_param * param)
{
    scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "DR(%lg) < 0",DR); // modify condition to your needs

	// insert your code here
	Q=q;
	P=2;
    if (SIGMA_R == 0.0) {
		Pcs=linsh_cyl(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &linsh_cyl_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_linsh_cyl_f(scalar q, sasfit_param * param)
{
    scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q=q;
	P=1;
    if (SIGMA_R == 0.0) {
		Pcs=linsh_cyl(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &linsh_cyl_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_linsh_cyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}
