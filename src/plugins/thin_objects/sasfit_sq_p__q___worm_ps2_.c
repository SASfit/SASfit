/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_p__q___worm_ps2_(scalar q, sasfit_param * param)
{

// J. S. Pedersen and P. Schurtenberger, 
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects, 
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 2 of the above paper is implemented. This method is only 
// without excluded volume effects

	double nb, S, SDebye, SSB, Slocal, q1, q2, p1, p2, a1, a2, Rg2;
	
    
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB < 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	nb = L/B;

	if (nb > 2) {
		Rg2 = L*B/6.0;
		q1 = 5.53;
		p1 = 5.33;
		a1 = 1.0;
		Slocal = a1/(L*B*q*q)+M_PI/(L*q);
		SSB = S_SB(q,sqrt(Rg2),param);
		S =   SSB    *     exp(-pow(q*B/q1,p1))
			+ Slocal * (1.-exp(-pow(q*B/q1,p1)));
	} else {
		a1 = 0.0625;
		a2 = 11.7;
		p2 = 3.95;
		q2 = a2/nb;
		Rg2 = fRg2(L,B);
		SDebye = IDebye(q,sqrt(Rg2));
		Slocal = a1/(L*B*q*q)+M_PI/(L*q);
		S =   SDebye *     exp(-pow(q*B/q2,p2))
			+ Slocal * (1.-exp(-pow(q*B/q2,p2)));
	}
	return L*L*S;
}


scalar sasfit_sq_p__q___worm_ps2__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___worm_ps2__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

