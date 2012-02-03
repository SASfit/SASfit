/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@pssi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar find_LogNorm_int_range(scalar dim, scalar x0, scalar sigma, scalar *Xstart, scalar *Xend, sasfit_param *param) 
{
	scalar a1,a2,a3,a4, X_0, X_50, X_n, nperc;
	nperc = 0.1;
	a3 = 1.0;
	a4 = fabs(x0);
	a2 = fabs(sigma);
	X_0  = a4*exp(-a2*a2*(a3-2.0*dim));
	X_n  = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/nperc)));
	X_50 = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/50.0)));

	SASFIT_CHECK_COND((X_50 == 0.0), param, "Can't guess good integration interval");

	*Xstart = 0.0;
	*Xend = X_n;

	if ((X_n-X_0) < X_0)
	{
		*Xend = X_n;
		*Xstart = 2.0*X_0-X_n;
	}
	return 0;
}


scalar fRg2(scalar cl,scalar lb)
// the radius of gyration of ideal semiflexible chains.
{
	scalar nb;
	nb = cl/lb;
	return cl*lb/6.0*(1.-1.5/nb
		               +1.5/gsl_pow_2(nb)
					   -0.75/gsl_pow_3(nb)
					   *(1.-exp(-2*nb)));
}

scalar falpha2(scalar cl, scalar lb)
// 
// alpha^2(x) is the expansion coefficient originating from excluded-volume interactions
//
{
	scalar epsilon = 0.170;
	scalar nb;
	nb = cl/lb;
	return pow( 1.0
	              + gsl_pow_2(nb/3.12)
				  + gsl_pow_3(nb/8.67)
				 ,epsilon/3.0);
}

scalar IDebye(scalar q, scalar Rg)
// Scattering function (Debye function) for Gaussian Coils with radius of gyration Rg
{
	scalar u;
	u = gsl_pow_2(q*Rg);
	if (u < 0.01) {
		return 1.-1./3.*u;
	} else { 
		return 2.0*(exp(-u)+u-1.0)/gsl_pow_2(u);
	}
}

scalar S_SB(scalar q, scalar Rg, sasfit_param *param) 
// The scattering function calculated for the
// Daniels approximation by Sharp and Bloomfield
{
	scalar u, nb;
	nb = L/B;
	u = gsl_pow_2(q*Rg);
	return IDebye(q,Rg)+(4./15.+7./(15*u)-(11./15.+7./(15.*u))*exp(-u))/nb;
}

scalar S_EXV(scalar q, scalar Rg, sasfit_param *param)
/*
The expression given for the scattering function for worm like chains with excluded volume effects 
by Utiyama et al.21 consists of a low-q and a large-q expansion. In order to get good agreement at
intermediate q, a large number of terms is included in the high-q expansion and this makes the 
numerical calculations rather slow. An approximate expression has therefore been constructed using
this approximation:
*/
{
	scalar SDebye, C1,C2,C3,w,x,nu,epsilon;
    SDebye = IDebye(q,Rg);
	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	x = Rg*q;
	w = (1.+tanh((x-1.523)/0.1477))/2.0;
	return  (1-w)* SDebye
		  + (  w)* (  C1*pow(x,-1.0/nu)
			         +C2*pow(x,-2.0/nu)
			         +C3*pow(x,-3.0/nu))*fcorr(q,Rg,param);
}


scalar fcorr(scalar q, scalar RG, sasfit_param *param)
{
/*	
	Correction function as described in the paper
	Wei-Ren Chen, Paul D. Butler, and Linda J. Magid
	Incorporating Intermicellar Interactions in the Fitting of SANS Data
	from Cationic Wormlike Micelles
	Langmuir 2006, 22, 6539-6548
	DOI: 10.1021/la0530440
	f_corr(Q) = 0 for dS(q)/dq > 0
	f_corr(Q) = 1 for dS(q)/dq <= 0
*/
	scalar dS;	
	scalar C1, C2, C3, C4, C5, epsilon, nu;

	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	C4 = 1.523;
	C5 = 0.1477;

	dS = -(1.0-pow(tanh((q*RG-C4)/C5),2.0))/(RG*RG*RG)/C5*(2.0*exp(-q*q*RG*RG
)+2.0*q*q*RG*RG-2.0)/(q*q*q*q)/2.0+(1.0/2.0-tanh((q*RG-C4)/C5)/2.0)*(-4.0*q*RG*
RG*exp(-q*q*RG*RG)+4.0*q*RG*RG)/(q*q*q*q)/(RG*RG*RG*RG)-4.0*(1.0/2.0-tanh((q*RG
-C4)/C5)/2.0)*(2.0*exp(-q*q*RG*RG)+2.0*q*q*RG*RG-2.0)/(q*q*q*q*q)/(RG*RG*RG*RG)
+(1.0-pow(tanh((q*RG-C4)/C5),2.0))*RG/C5*(C1*pow(q*RG,-1.0/nu)+C2*pow(q*RG,-2.0
/nu)+C3*pow(q*RG,-3.0/nu))/2.0+(1.0/2.0+tanh((q*RG-C4)/C5)/2.0)*(-C1*pow(q*RG,
-1.0/nu)/nu/q-2.0*C2*pow(q*RG,-2.0/nu)/nu/q-3.0*C3*pow(q*RG,-3.0/nu)/nu/q)+CLB*
(-14.0/15.0/(q*q*q)/(RG*RG)+14.0/15.0/(q*q*q)/(RG*RG)*exp(-q*q*RG*RG)+2.0*(11.0
/15.0+7.0/15.0/(q*q)/(RG*RG))*q*RG*RG*exp(-q*q*RG*RG))*B/L;

	if (dS<0) {
		return 1;
	} else {
		return 0;
	}
}

scalar IRod(scalar q, scalar H)
//  ROD SCATTERING FUNCTION
{
	return 2.0*gsl_sf_Si(q*H)/(q*H)-gsl_pow_2(sin(q*H/2.)/(q*H/2.));
}