/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_p__q___worm_ps1_(scalar q, sasfit_param * param)
{

// J. S. Pedersen and P. Schurtenberger,
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects,
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 1 of the above paper is implemented


	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB < 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	double AEXP1, AEXP2;
	double Schain, Srod, SDebye, Swc, chi, xi, Rg2_0, nb, WGAMMA, q_mem;
	double a1[6][3], a2[6][3], b1[3][3], b2[3][3];
	int i,j;
	double AA[6]={0,0,0,0,0};
	double BB[3]={0,0,0};
	double na1[6][3] = {
	{ 0.0,		 0.0,		 0.0	}, // a1[0][*]
	{ 0.0,		 0.0,		 0.0	}, // a1[1][*]
	{ 0.3054,	 0.2316,	-22.779	}, // a1[2][*]
//	{ 0.3054,	 0.2316,	-22.25	}, // a1[2][*]
	{ 0.05777,	 2.6531,	 23.2457}, // a1[3][*]
//	{ 0.05777,	 2.6531,	 23.25}, // a1[3][*]
	{-0.00604,	 0.3706,	 8.1092	}, // a1[4][*]
	{-0.03902,	-1.0081,	-3.3603	}  // a1[5][*]
};
double na2[6][3] = {
	{ 0.0,	 0.0,		 0.0	}, // a2[0][*]
	{ 0.0,	 0.0,		 0.0	}, // a2[1][*]
	{ 0.0,	-0.4963,	-0.4678	}, // a2[2][*]
	{ 0.0,	 0.03688,	 0.3365	}, // a2[3][*]
	{ 0.0,	 0.30570,	 0.4290	}, // a2[4][*]
	{ 0.0,	 0.39013,	 0.3737	}  // a2[5][*]
};
double nb1[3][3] = {
	{-0.0162,	-0.3565,	-0.3078	},
	{ 0.09046,	 0.1909,	 0.05176},
	{ 0.1213,	 0.15634,	 0.01568}
};
double nb2[3][3] = {
	{ 0.0,	-0.3946,	 1.1361	},
	{ 0.0,	-0.2231,	-0.01615},
	{ 0.0,	-0.2546,	-0.07606}
};

double xa1[6][3] = {
	{ 0.0,		 0.0,		 0.0	}, // xa1[0][*]
	{ 0.0,		 0.0,		 0.0	}, // xa1[1][*]
	{-0.1222,	 1.761,		-26.04	}, // xa1[2][*]
	{ 0.3051,	 2.252,		 20.00	}, // xa1[3][*]
	{-0.0711,	-1.291,		 4.382	}, // xa1[4][*]
	{ 0.0584,	 0.6994,	 1.594	}  // xa1[5][*]
};
double xa2[6][3] = {
	{ 0.0,	 0.0,		 0.0	}, // a2[0][*]
	{ 0.0,	 0.0,		 0.0	}, // a2[1][*]
	{ 0.0,	 0.1212,	 0.0170	}, // a2[2][*]
	{ 0.0,	-0.4169,	-0.4731	}, // a2[3][*]
	{ 0.0,	 0.1988,	 0.1869	}, // a2[4][*]
	{ 0.0,	 0.3435,	 0.3350	}  // a2[5][*]
};
double xb1[3][3] = {
	{-0.0699,	 0.1342,	-0.2020},
	{-0.0900,	 0.0138,	-0.0114},
	{ 0.2677,	 0.1898,	 0.0123}
};
double xb2[3][3] = {
	{ 0.0,	-0.5171,	 0.6950	},
	{ 0.0,	-0.2028,	-0.3238},
	{ 0.0,	-0.3112,	-0.5403}
};

SASFIT_ASSERT_PTR(param);

if (EXVOL >= 1) {
	for (i=0;i<6;i++){
		for (j=0;j<3;j++){
			a1[i][j] = xa1[i][j];
			a2[i][j] = xa2[i][j];
		}
	}
	for (i=0;i<3;i++){
		for (j=0;j<3;j++){
			b1[i][j] = xb1[i][j];
			b2[i][j] = xb2[i][j];
		}
	}
} else {
	for (i=0;i<6;i++){
		for (j=0;j<3;j++){
			a1[i][j] = na1[i][j];
			a2[i][j] = na2[i][j];
		}
	}
	for (i=0;i<3;i++){
		for (j=0;j<3;j++){
			b1[i][j] = nb1[i][j];
			b2[i][j] = nb2[i][j];
		}
	}
}

nb = L/B;

q_mem = q;
if (q_mem*B > 10.) {
	q = 10./B;
}


if (EXVOL < 1) {
	Rg2_0 = fRg2(L,B);
	xi = q*M_PI/(2.0*fabs(L))*Rg2_0;
	if (gsl_pow_5(xi) < 0.01) {
		chi = 0.0;
	} else {
		chi = exp(-1.0/gsl_pow_5(xi));
	}
} else {
	Rg2_0 = fRg2(L,B)*falpha2(L,B);
	xi = q*B*pow(M_PI*B/(1.103*fabs(L)),3./2.) * pow(Rg2_0/(B*B),1.282);
	if (gsl_pow_5(xi) < 0.01) {
		chi = 0;
	} else {
		chi = exp(-1.0/gsl_pow_5(xi));
	}
}

// CORRECTION FUNCTION FGAMMA
// CALCULATE A'S

AEXP1 = 10./nb;
if (AEXP1 > 100) {
	AEXP1 = 0;
} else {
	AEXP1 = exp(-AEXP1);
}
AEXP2 = 2.*nb;
if (AEXP2 > 100) {
	AEXP2 = 0;
} else {
	AEXP2 = exp(-AEXP2);
}

for (i=2;i<=5;i++) {
	AA[i] = 0.0;
	for (j=0;j<=2;j++) {
		if (j==0) {
			AA[i] =   AA[i]
			        + a1[i][j]*pow(nb,-j)*AEXP1;
		} else {
			AA[i] =   AA[i]
			        + a1[i][j]*pow(nb,-j)*AEXP1
				    + a2[i][j]*pow(nb,j)*AEXP2;
		}
	}
}


// CALCULATE B'S

for (i=0;i<=2;i++) {
	BB[i] = 0.0;
	for (j=0;j<=2;j++) {
		if (j==0) {
			BB[i] =   BB[i]
			        + b1[i][j]*pow(nb,-j);
		} else {
			BB[i] =   BB[i]
			        + b1[i][j]*pow(nb,-j)
				    + b2[i][j]*pow(nb,j)*AEXP2;
		}
	}
}

// CALCULATE WGAMMA

WGAMMA = 1.0;
for (i=2;i<=5;i++) {
	WGAMMA = WGAMMA+(1.0-chi)*AA[i]*pow(xi,i);
}
for (i=0;i<=2;i++) {
	WGAMMA = WGAMMA+chi*BB[i]*pow(xi,-i);
}
// interpolated function
if (EXVOL < 1) {
	SDebye = IDebye(q,sqrt(Rg2_0));
	Schain = SDebye;
} else {
	CLB = 1;
	Schain = S_EXV(q,sqrt(Rg2_0),param);
}
Srod = IRod(q,L);

// The large argument expansion for q*B>10 has been done as in lsqres.for. However, this expansion
// is not described in the original publication.
// Without this expansion S(q) does not properly converges to the proper potential behaviour.
if (q_mem*B<=10) {
	Swc = ((1.0-chi)*Schain + chi*Srod)*WGAMMA;
} else {
	Swc = 100.*(((1.0-chi)*Schain + chi*Srod)*WGAMMA-M_PI/(10.*nb));
	Swc = M_PI/(q_mem*L)+Swc/(q_mem*q_mem*B*B);
}
return Swc*L*L;
}

scalar sasfit_sq_p__q___worm_ps1__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___worm_ps1__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

