/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

// define shortcuts for local parameters/variables
typedef struct
{
	sasfit_param *param;
	scalar RG;
	scalar q0_B;
} Sdiff_param;

double f (double x, void *Sparam) {
    Sdiff_param *Spam;
    sasfit_param *param;
    scalar u, Rg, res;
    Spam = (Sdiff_param *) Sparam;
	param = Spam->param;
	Rg = Spam->RG;
	u = gsl_pow_2(Rg*x);
    res = L*L*(S_EXV(x,Rg,param) + CLB*(4./15.+7./(15*u)-(11./15.+7./(15.*u))*exp(-u))*LB/L);
    return res;
}

void S_fdf(scalar *S, scalar *Sp, Sdiff_param *Sparam) {
    gsl_function F;
    double abserr;

  F.function = &f;
  F.params = Sparam;
  *S = f(Sparam->q0_B, Sparam);
  gsl_deriv_central (&F, Sparam->q0_B, 1e-8, Sp, &abserr);
 }


scalar sasfit_sq_p__q___worm_ps3_(scalar q, sasfit_param * param)
{
    Sdiff_param Sparam;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB < 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here

// J. S. Pedersen and P. Schurtenberger,
// Scattering Functions of Semiflexible Polymers with and without Excluded Volume Effects,
// Macromolecules 1996, 29, 7602-7612
// DOI: 10.1021/ma9607630
// here method 3 of the above paper is implemented.

	scalar a1,a2,a3,a4,p1,p2,p3, q0, u, RG, Sf, Sdf;
	scalar MapleGenVar1, MapleGenVar2, MapleGenVar3, MapleGenVar4, MapleGenVar5, MapleGenVar6;
	scalar C1, C2, C3, C4, C5, epsilon, nu;

    if (fabs(L*q)<=1e-16) return gsl_pow_2(L);

	C1 = 1.2220;
	C2 = 0.4288;
	C3 = -1.651;
	epsilon = 0.170;
	nu = (1.0+epsilon)/2.0;
	C4 = 1.523;
	C5 = 0.1477;

	CLB = 1;
	a4 = 3.06;
	p3 = 0.44;
	a3 = 2.02;

	if (L>4.0*B) {
		q0 = 3.1;
	} else {
		if (EXVOL < 1) {
			q0=a3*B/sqrt(fRg2(L,B));
			if (q0<4.0) q0=4.0;
		} else {
			q0=a3*B/sqrt(fRg2(L,B)*falpha2(L,B));
			if (q0<3.0) q0=3.0;
		}
	}

	if (EXVOL < 1) {
		if (L>4*B) {
			if (q*B<q0) {
				return L*L*S_SB(q,sqrt(L*B/6.0),param);
			} else {
				p1 = 4.95;
				p2 = 5.29;
				RG = sqrt(fRg2(L,B));
				RG = sqrt(L*B/6.0);
				a1 = -pow(q0,1.0*p1-4.0)*(-60.0*q0*q0*RG*RG*B*B*B*L+30.0*B*B*B*B*B*L*exp(
-q0*q0/(B*B)*RG*RG)*p2-30.0*B*B*B*B*B*L*p2-60.0*q0*q0*RG*RG*B*B*B*L*exp(-q0*q0/
(B*B)*RG*RG)+14.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B+22.0*CLB*q0*
q0*q0*q0*q0*q0*RG*RG*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+14.0*CLB*q0*q0*q0*q0*
RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B+15.0*B*B*q0*q0*q0*RG*RG*RG*RG*
0.3141592653589793E1-14.0*CLB*q0*q0*RG*RG*B*B*B*B+120.0*B*B*B*B*B*L-120.0*B*B*B
*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+30.0*q0*q0*RG*RG*B*B*B*L*p2-15.0*B*B*q0*q0*q0*RG
*RG*RG*RG*0.3141592653589793E1*p2-11.0*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(
B*B)*RG*RG)*B*B*p2+4.0*B*B*CLB*q0*q0*q0*q0*RG*RG*RG*RG*p2+7.0*CLB*q0*q0*RG*RG*B
*B*B*B*p2-7.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B*B*B*p2)/L/(RG*RG*RG*
RG)/B/(-p2+p1)/15.0;
				a2 = pow(q0,1.0*p2-4.0)*(-60.0*q0*q0*RG*RG*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG
)-60.0*q0*q0*RG*RG*B*B*B*L-120.0*B*B*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+120.0*B*B*
B*B*B*L-14.0*CLB*q0*q0*RG*RG*B*B*B*B+14.0*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*
RG)*B*B*B*B+22.0*CLB*q0*q0*q0*q0*q0*q0*RG*RG*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG
)+14.0*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)*B*B-15.0*B*B*p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+15.0*B*B*q0*q0*q0*RG*RG*RG*RG*
0.3141592653589793E1+30.0*B*B*B*p1*L*q0*q0*RG*RG-30.0*L*p1*B*B*B*B*B+4.0*B*B*p1
*CLB*q0*q0*q0*q0*RG*RG*RG*RG+30.0*B*B*B*B*B*p1*L*exp(-q0*q0/(B*B)*RG*RG)-11.0*B
*B*p1*CLB*q0*q0*q0*q0*RG*RG*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+7.0*B*B*B*B*p1*CLB*q0
*q0*RG*RG-7.0*B*B*B*B*p1*CLB*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG))/B/(RG*RG*RG*
RG)/L/(-p2+p1)/15.0;
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		} else {
			if (q*B<q0) {
				return L*L*IDebye(q,sqrt(fRg2(L,B)));
			} else {
				RG = sqrt(fRg2(L,B));
				p1 = 5.13;
				p2 = 7.47;
				CLB = 1;
				a1 = B*pow(q0,1.0*p1-4.0)*(-2.0*B*L*q0*q0*RG*RG*p2+4.0*B*L*q0*q0*RG*RG*
exp(-q0*q0/(B*B)*RG*RG)+4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)
-8.0*B*B*B*L-0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+0.3141592653589793E1*q0*
q0*q0*RG*RG*RG*RG*p2-2.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)*p2+2.0*B*B*B*L*p2)/L/(
RG*RG*RG*RG)/(p1-p2);
				a2 = -B*pow(q0,1.0*p2-4.0)*(4.0*B*L*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+
4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)-8.0*B*B*B*L+p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG-0.3141592653589793E1*q0*q0*q0*RG*RG*
RG*RG-2.0*p1*B*L*q0*q0*RG*RG-2.0*p1*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+2.0*p1*B*B*
B*L)/(RG*RG*RG*RG)/L/(p1-p2);
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		}
	} else { // with excluded volume effects: EXVOL >= 1
		if (L>4*B) {
//			RG = sqrt(falpha2(L,B)*L*B/6.0);
			RG = sqrt(falpha2(L,B)*fRg2(L,B));
			if (L>10.0*B) {
				CLB = a4/pow(L/B,p3);
			} else {
				CLB = 1;
			}
			if (q*B<=q0) {
				u = gsl_pow_2(RG*q);
				return L*L*(S_EXV(q,RG,param) + CLB*(4./15.+7./(15*u)-(11./15.+7./(15.*u))*exp(-u))*B/L);
			} else {
				p1 = 4.12;
				p2 = 4.42;
				Sparam.q0_B=q0/LB;
				Sparam.RG=RG;
				Sparam.param=param;
				S_fdf(&Sf, &Sdf, &Sparam);
//				a2 = ((Sf+Sdf*q0/p1)-M_PI*LB/(q0*L)*(1-1/p1))
//                    *(p1*pow(q0,p2))/(p1-p2);
//				a1= Sf*pow(q0,p1)-a2*pow(q0,p1-p2)+M_PI*LB*pow(q0,p1-1)/L;
				a1= -pow(q0,1.0*p1-1.0)*(-B*B*L*p2*M_PI+B*B*L*M_PI+B*Sf*p2*q0+Sdf*q0*q0)/(L*L)/B/(p1-p2);
				a2 = pow(q0,1.0*p2-1.0)*(-B*B*L*p1*M_PI+B*B*L*M_PI+B*Sf*p1*q0+Sdf*q0*q0)/(L*L)/B/(p1-p2);
                return L*L*(a1/pow(q*LB,p1)+a2/pow(q*LB,p2)+M_PI/(q*L));
			}
		} else {
			RG = sqrt(fRg2(L,B)*falpha2(L,B));
			q0=a3*B/sqrt(fRg2(L,B));
			if (q0<4.0) {
				q0=4.0;
			}
			if (q*B<=q0) {
				return L*L*IDebye(q,RG);
			} else {
				p1 = 5.36;
				p2 = 5.62;
				a1 = B*pow(q0,1.0*p1-4.0)*(-2.0*B*L*q0*q0*RG*RG*p2+4.0*B*L*q0*q0*RG*RG*
exp(-q0*q0/(B*B)*RG*RG)+4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)
-8.0*B*B*B*L-0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG+0.3141592653589793E1*q0*
q0*q0*RG*RG*RG*RG*p2-2.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)*p2+2.0*B*B*B*L*p2)/L/(
RG*RG*RG*RG)/(p1-p2);
				a2 = -B*pow(q0,1.0*p2-4.0)*(4.0*B*L*q0*q0*RG*RG*exp(-q0*q0/(B*B)*RG*RG)+
4.0*B*L*q0*q0*RG*RG+8.0*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)-8.0*B*B*B*L+p1*
0.3141592653589793E1*q0*q0*q0*RG*RG*RG*RG-0.3141592653589793E1*q0*q0*q0*RG*RG*
RG*RG-2.0*p1*B*L*q0*q0*RG*RG-2.0*p1*B*B*B*L*exp(-q0*q0/(B*B)*RG*RG)+2.0*p1*B*B*
B*L)/(RG*RG*RG*RG)/L/(p1-p2);
				return L*L*(a1/pow(q*B,p1)+a2/pow(q*B,p2)+M_PI/(q*L));
			}
		}
	}
	return 9999999;
}

scalar sasfit_sq_p__q___worm_ps3__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___worm_ps3__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
