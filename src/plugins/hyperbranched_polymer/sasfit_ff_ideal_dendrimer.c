/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

// doi: 10.1063/5.0160446.
// M. Jura and M. Bishop,
// “The scattering functions of ideal tri-functional comb and dendrimer polymers,”
// The Journal of Chemical Physics,
// vol. 159, no. 3, Jul. 2023,

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>


// define shortcuts for local parameters/variables
#define RG	    param->p[0]
#define DUMMY	param->p[1]
#define G       param->p[2]
#define I0	    param->p[3]


scalar sasfit_ff_ideal_dendrimer(scalar q, sasfit_param * param)
{
    scalar X,X2,X4,Dendrimer[10],u,v,w,F,res;
    int gl;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG <= 0.0), param, "Rg(%lg) <= 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((G < 0.0), param, "g(%lg) < 0",G); // modify condition to your needs
	SASFIT_CHECK_COND1((G > 9.0), param, "g(%lg) > 9",G); // modify condition to your needs

	// insert your code here

    X= q*RG;
    u=X*X;
    if (u<=1e-4) return I0;
    res = 42;
    Dendrimer[0] = 2/(u*u) *(u+expm1(-u));

    F=3;
    v = u*F/(3.0*F-2.0);
	Dendrimer[1] = (2.0/(F*v*v)*(-1.0+v+exp(-v)+(F-1.0)*pow(1.0-exp(-v),2.0)/2.0)); // generation 1, 3 branches
	Dendrimer[1] = 2.0/(u*u)*(u-3*(1-exp(-u/3))+3*gsl_pow_2(1-exp(-u/3))); // generation 1, 3 branches

	Dendrimer[2] = 2./(u*u)* (3 + u - 3*exp(-u/9.) - 12*exp(-u/3.) + 12*exp(-4*u/9)); // generation 2, 9 branches

	Dendrimer[3] = 2./(u*u)* (9 + u - 3*exp(-u/21) + 6*exp(-2*u/21) - 12*exp(-u/7)
                                - 48*exp(-5*u/21) + 48*exp(-2*u/7));  // generation 3, 45 branches

	Dendrimer[4] = 2./(u*u)* (45 + u - 3*exp(-u/93) + 42*exp(-2*u/93) - 12*exp(-u/31)
                                + 72*exp(-4*u/93) - 48*exp(-5*u/93) + 96*exp(-2*u/31)
                                - 192*exp(-7*u/93) - 768*exp(-3*u/31)
                                + 768*exp(-10*u/93));  // generation 4, 93 branched

    Dendrimer[5] = 2./(u*u)* (93 + u - 3*exp(-u/189) + 90*exp(-2*u/189) - 12*exp(-u/63)
                                 + 168*exp(-4*u/189) - 48*exp(-5*u/189) + 288*exp(-2*u/63)
                                 - 192*exp(-u/27) + 384*exp(-8*u/189) - 768*exp(-u/21)
                                 - 3072*exp(-11*u/189) + 3072*exp(-4*u/63)); // generation 5, 189 branched

    Dendrimer[6] = 2./(u*u)*
                             (  (u + 189)*exp(14*u/381-14*u/381)
                                + 672*exp(8*u/381-14*u/381)
                                + 360*exp(10*u/381-14*u/381)
                                - 12*exp(11*u/381-14*u/381)
                                - 3*exp(13*u/381-14*u/381)
                                - 3072*exp(u/127-14*u/381)
                                - 12288*exp(u/381-14*u/381)
                                + 1152*exp(2*u/127-14*u/381)
                                - 48*exp(3*u/127-14*u/381)
                                + 186*exp(4*u/127-14*u/381)
                                + 1536*exp(4*u/381-14*u/381)
                                - 768*exp(5*u/381-14*u/381)
                                - 192*exp(7*u/381-14*u/381)
                                + 12288*exp(-14*u/381)); // generation 6, 381 branched

    Dendrimer[7] = 2./(u*u)*
                             (  (u + 381)*exp(16*u/765-16*u/765)
                               + 6144*exp(4*u/765-16*u/765)
                               - 768*exp(7*u/765-16*u/765)
                               + 2688*exp(8*u/765-16*u/765)
                               - 48*exp(11*u/765-16*u/765)
                               - 12*exp(13*u/765-16*u/765)
                               + 378*exp(14*u/765-16*u/765)
                               - 3*exp(u/51-16*u/765)
                               - 192*exp(u/85-16*u/765)
                               - 3072*exp(u/153-16*u/765)
                               - 12288*exp(u/255-16*u/765)
                               - 49152*exp(u/765-16*u/765)
                               + 1440*exp(2*u/153-16*u/765)
                               + 4608*exp(2*u/255-16*u/765)
                               + 744*exp(4*u/255-16*u/765)
                               + 49152*exp(-16*u/765) ); // generation 7, 765 branched

    Dendrimer[8] = 2./(u*u)*
                             (  (u + 765)*exp(6*u/511-6*u/511)
                                - 196608*exp(u/1533-6*u/511)
                                + 24576*exp(4*u/1533-6*u/511)
                                - 12288*exp(5*u/1533-6*u/511)
                                + 10752*exp(8*u/1533-6*u/511)
                                + 5760*exp(10*u/1533-6*u/511)
                                - 192*exp(11*u/1533-6*u/511)
                                - 48*exp(13*u/1533-6*u/511)
                                + 762*exp(16*u/1533-6*u/511)
                                - 3*exp(17*u/1533-6*u/511)
                                - 3072*exp(u/219-6*u/511)
                                - 49152*exp(u/511-6*u/511)
                                + 1512*exp(2*u/219-6*u/511)
                                + 18432*exp(2*u/511-6*u/511)
                                - 768*exp(3*u/511-6*u/511)
                                + 2976*exp(4*u/511-6*u/511)
                                - 12*exp(5*u/511-6*u/511)
                                + 196608*exp(-6*u/511) ); // generation 8, 1533 branched

    Dendrimer[9] = 2./(u*u)*
                             (  (u + 1533)*exp(20*u/3069-20*u/3069)
                               - 48*exp(5*u/1023-20*u/3069)
                               - 49152*exp(5*u/3069-20*u/3069)
                               - 12288*exp(7*u/3069-20*u/3069)
                               + 43008*exp(8*u/3069-20*u/3069)
                               + 23040*exp(10*u/3069-20*u/3069)
                               - 192*exp(13*u/3069-20*u/3069)
                               + 6048*exp(14*u/3069-20*u/3069)
                               + 3048*exp(16*u/3069-20*u/3069)
                               - 12*exp(17*u/3069-20*u/3069)
                               - 768*exp(u/279-20*u/3069)
                               - 3*exp(19*u/3069-20*u/3069)
                               - 3072*exp(u/341-20*u/3069)
                               + 1530*exp(2*u/341-20*u/3069)
                               - 196608*exp(u/1023-20*u/3069)
                               - 786432*exp(u/3069-20*u/3069)
                               + 73728*exp(2*u/1023-20*u/3069)
                               + 11904*exp(4*u/1023-20*u/3069)
                               + 98304*exp(4*u/3069-20*u/3069)
                               + 786432*exp(-20*u/3069) ); // generation 9, 3069 branched

    gl = rint(G);
    w = G-gl;
    res = (1-w) * Dendrimer[gl] + w*Dendrimer[gl+1];
    return I0*res;
}

scalar sasfit_ff_ideal_dendrimer_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ideal_dendrimer_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

