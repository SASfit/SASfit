/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

#define R_C	param->p[0]

scalar sasfit_ff_mlv_frielinghaus(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_C < 0.0), param, "R_c(%lg) < 0",R_C); // modify condition to your needs

	// insert your code here

	static integer npar;
	static char thnam[8];
	static char parnam[8*20];
	static integer ini;
	static real mlv;
	real Q = (real)q;

	SASFIT_ASSERT_PTR(param);

	param->p[MAXPAR-2] = 1.0e20;	// phi = 1
	param->p[MAXPAR-1] = 0.0;	// background

	npar = 12;
	ini = 0;
	mlv = (real)sasfit_th36(&Q, param, thnam, parnam, &npar, &ini, 8L, 8L);
	ini = 1;
	npar = 12;
/*
		  parnam(1) = 'phi'	! Concentration Polymer
		  parnam(2) = 'n'	  ! Number of layers
		  parnam(3) = 'nw'	 ! width of n-distribution
		  parnam(4) = 'rhoW'  ! Scattering length density water
		  parnam(5) = 'rho1'  ! Scattering length density 'core'
		  parnam(6) = 'rho2'  ! Scattering length density 'shell'
		  parnam(7) = 'sig1'  ! Thickness 'core'
		  parnam(8) = 'sig2'  ! Thickness 'shell'
		  parnam(9) = 'Rmain' ! Main Radius (distance between layers)
		  parnam(10)= 'zz'	 ! Schultz Z
		  parnam(11)= 'Rshift'! Maximum possible shift
		  parnam(12)= 'bckgr' ! background
 */
	mlv = (real)sasfit_th36(&Q, param, thnam, parnam, &npar, &ini, 8L, 8L);

	return mlv;
}

scalar sasfit_ff_mlv_frielinghaus_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_mlv_frielinghaus_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

