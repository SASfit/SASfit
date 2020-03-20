/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define LC	param->p[0]
#define N_AGG param->p[1]
#define V_BRUSH param->p[2]
#define ETA_CORE param->p[3]
#define ETA_BRUSH param->p[4]
#define ETA_SOLV param->p[5]
#define XSOLV_CORE param->p[6]
#define RG param->p[7]
#define D param->p[8]

#define SURF M_PI*gsl_pow_2(100*((LC < 1.) ?  1 : LC)) // surface area is not really known for this cross section form factor and is fixed to be the one of a disc with a radius 100 times larger than the thickjness of the core layer
#define PCHAIN param->p[MAXPAR-3]
#define NAGG param->p[MAXPAR-2]
#define V_CORE param->p[MAXPAR-1]

scalar sasfit_ff_pcs_plate_chains_rw_(scalar q, sasfit_param * param)
{
	scalar Fc, Fs, Fcs, Scc, Ssc, w, I, rc, rs;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LC < 0.0), param, "L_core(%lg) < 0",LC); // modify condition to your needs
	SASFIT_CHECK_COND1((N_AGG <= 0.0), param, "nagg(%lg) <= 0",N_AGG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_BRUSH < 0.0), param, "V_brush(%lg) < 0",V_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((XSOLV_CORE < 0.0), param, "xsolv_core(%lg) < 0",XSOLV_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((XSOLV_CORE >= 1), param, "xsolv_core(%lg) >= 1.0",XSOLV_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RG <= 0.0), param, "Rg(%lg) <= 0",RG); // modify condition to your needs

	// insert your code here

	V_CORE = LC * (1.0-XSOLV_CORE)/N_AGG;
	NAGG = SURF * N_AGG;

	rs = V_CORE * (ETA_CORE - ETA_SOLV);
	rc = V_BRUSH * (ETA_BRUSH - ETA_SOLV);

	Fc = sasfit_gauss_fc(q, RG);
	w = sasfit_rwbrush_w(q, RG);
	Fs = gsl_sf_sinc(q*LC*0.5/M_PI);
	Fcs = Fs*Fs;
	Scc = gsl_pow_2(w*cos(q*(LC/2.+D*RG)));
	Ssc = w * Fs *cos(q*(LC/2.+D*RG));

    PCHAIN = NAGG*rc*rc*Fc;
	I =   NAGG*NAGG*rs*rs*Fcs
		+ 0*NAGG*rc*rc*Fc
		+ NAGG*(NAGG-1.)*((NAGG < 1.) ?  0 : 1)*rc*rc*Scc
		+ 2.*NAGG*NAGG*rs*rc*Ssc;
	return I/gsl_pow_2(SURF);
}

scalar sasfit_ff_pcs_plate_chains_rw__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_plate_chains_rw__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

