/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

 // doi: 10.1021/ma049950l
 // Burchard, Walther
 // Angular Dependence of Scattered Light from Hyperbranched Structures in a Good Solvent. A Fractal Approach
 // 2004-04
 // Macromolecules , Vol. 37, No. 10
 // American Chemical Society (ACS)
 // p. 3841-3849

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>


// define shortcuts for local parameters/variables
#define SIGMA	param->p[0]
#define F	    param->p[1]
#define ALPHA   param->p[2]
#define I0	    param->p[3]
#define APPROX	    param->p[4]

scalar Paf_Gauss(scalar x, sasfit_param * param) {
    return 0;
}

scalar Paf_exact(scalar x, sasfit_param * param) {
    return 0;
}

scalar sasfit_ff_f_funct_homo_polym(scalar q, sasfit_param * param)
{
    scalar X,u, FGauss, FStraight, PGm1, PSm1, Rg, S2z, phi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((SIGMA <= 0), param, "b(%lg) <= 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((F <= 1.0), param, "f(%lg) <= 1",F); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA > 1.0), param, "alpha(%lg) > 1",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND2(F >= (1.0+ALPHA)/ALPHA, param, "f(%lg) >= (1+alpha)/alpha, alpha(%lg)",F,ALPHA); // modify condition to your needs
    SASFIT_CHECK_COND2(ALPHA >= 1/(F-1), param, "alpha(%lg) >= 1/(f-1), f(%lg)",ALPHA,F); // modify condition to your needs

	// insert your code here
//	S2z = SIGMA*SIGMA * ((ALPHA*(F-1)/(1-ALPHA*(F-1))+ALPHA/(1+ALPHA))/2.);
    S2z = SIGMA*SIGMA/(2*(1-(F-1)*ALPHA));
	Rg=sqrt(S2z);
    X= q*Rg;
    phi = exp(-X*X/6.);
    switch (lround(APPROX)) {
        case 0: return I0/(1+X*X/3);
        case 1: return I0*ALPHA*F*phi/(1-ALPHA*(F-1)*phi) * (1-ALPHA*(F-1))/(ALPHA*F);
        case 2: return sasfit_integrate(0,GSL_POSINF,&Paf_Gauss,param);
        case 3: return sasfit_integrate(0,GSL_POSINF,&Paf_exact,param);
        default: sasfit_err("aproximarion: %d unknown!\n",lround(APPROX));
    }
}

scalar sasfit_ff_f_funct_homo_polym_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_f_funct_homo_polym_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

