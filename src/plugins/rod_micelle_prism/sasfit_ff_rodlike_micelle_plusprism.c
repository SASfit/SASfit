/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (<joachim.kohlbrecher@psi.ch>)
 *   17.5.2009
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R			param->p[0]
#define EPSILON		param->p[1]
#define L			param->p[2]
#define T			param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SH		param->p[5]
#define ETA_SOL		param->p[6]
#define PHI			param->p[7]
#define PAR			param->p[8]
#define HOW			param->p[9]
#define Q			param->p[MAXPAR-3]
#define ALPHA		param->p[MAXPAR-2]
#define THETA		param->p[MAXPAR-1]

scalar beta_cq(scalar q, sasfit_param *param)
{
	scalar cq,beta,u,S0, B,C,D,n,r;
	scalar B2, B3,  Vp, Ap, Rp;

	r=sqrt((R+T)*(R*EPSILON+T));
	u=q*2.0*r;
	cq=3*(sin(u)-u*cos(u))/gsl_pow_3(u);
	n = PHI/sasfit_ff_rodlike_micelle_plusprism_v(q,param,0);

	B = M_PI*r*r*(L+2.0*T)*n;
	C = 4./3.*M_PI*r*r*r*n;
	D = 0.5 * M_PI*r*(L+2.0*T)*(L+2.0*T)*n;
	S0 = -1;
	if ( (int) (fabs(fmod(HOW,10))+0.5) == 1) {
		S0 = gsl_pow_4(1.0-B-C)/(gsl_pow_2(1.0+2.0*(B+C))+2.0*D*(1.0+B+5./4.*C));
	}
	if ( (int) (fabs(fmod(HOW,10))+0.5) == 2) {
		S0 = gsl_pow_4(1.0-B-C)/((1.0+2.0*gsl_pow_2(B+C))+2.0*D*(1.0+B+5./4.*C));
	}

	if ( (int) (fmod(HOW,10)+0.5) == 3) {
		Rp = M_PI*r/4.0+(L+2*T)/4.0;
		Vp = sasfit_ff_rodlike_micelle_plusprism_v(q,param,0);
		if (EPSILON == 1) {
			Ap = 2.0*M_PI*R;
		} else if (EPSILON < 1) {
			Ap = 4.0*R*gsl_sf_ellint_Ecomp(EPSILON,0);
		} else if (EPSILON > 1) {
			Ap = 4.0*R*EPSILON*gsl_sf_ellint_Ecomp(1./EPSILON,0);
		}
		B2 = (Vp+Ap*Rp)/Vp;
		B3 = (gsl_pow_2(Vp)+2.0*Rp*Ap*Vp+1.0/3.0*gsl_pow_2(Rp*Ap))/gsl_pow_2(Vp);
		S0 = gsl_pow_4(1.0-PHI)/(1.0+2.0*(B2-2.0)*PHI+(3.0*B3-8.0*B2+6.0)*gsl_pow_2(PHI));
	}
	if ( (int) (fabs(fmod(HOW,10))+0.5) == 4) {
		Rp = M_PI*r/4.0+(L+2*T)/4.0;
		Vp = sasfit_ff_rodlike_micelle_plusprism_v(q,param,0);
		if (EPSILON == 1) {
			Ap = 2.0*M_PI*R;
		} else if (EPSILON < 1) {
			Ap = 4.0*R*gsl_sf_ellint_Ecomp(EPSILON,0);
		} else if (EPSILON > 1) {
			Ap = 4.0*R*EPSILON*gsl_sf_ellint_Ecomp(1./EPSILON,0);
		}
		B3 = (gsl_pow_2(Vp)+2.0*Rp*Ap*Vp+1.0/(12.0*M_PI)*gsl_pow_3(Ap))/gsl_pow_2(Vp);
		S0 = gsl_pow_4(1.0-PHI)/(1.0+2.0*(B2-2.0)*PHI+(3.0*B3-8.0*B2+6.0)*gsl_pow_2(PHI));
	}

	if ( (int) (fabs(fmod(HOW,10))+0.5) == 5) {
		S0 = 1.0/(1.0+2.0*PAR*PHI);
	}

	SASFIT_CHECK_COND1((S0 < 0.0), param, "S0(%lg) < 0",S0);

	beta = (1.0-S0)/S0;
	return beta*cq;
}

scalar P_Rod(scalar q, scalar length)
{
	scalar u,u2;
	u = fabs(q*length);
	u2 = u/2.0;
//	if (u < 1.0e-2) return exp(-u*u/12.0/3.0);
	return 2.0*gsl_sf_Si(u)/u-gsl_pow_2(sin(u2)/u2);
}

scalar P_L(scalar q, sasfit_param *param)
{
	return L*L*P_Rod(q,L);
}

scalar r(scalar radius, scalar epsilo, scalar theta)
{
	return radius*sqrt(sin(theta)*sin(theta)+epsilo*epsilo*cos(theta)*cos(theta));
}

scalar AcylSHell(scalar x, sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,v1,v2,b1,b2;
	u1 = Q*r(R,EPSILON,THETA)*sin(ALPHA);
	v1 = Q*L*0.5*cos(ALPHA);
	u2 = Q*(r(R,EPSILON,THETA)+T)*sin(ALPHA);
	v2 = Q*(L*0.5+T)*cos(ALPHA);
	b1 = M_PI*R*R*EPSILON*L*(ETA_CORE-ETA_SH);
	b2 = M_PI*(R+T)*(R+T)*EPSILON*(L+2.0*T)*(ETA_SH-ETA_SOL);

	if (v1==0) {
		Ain = 1;
	} else {
		Ain = sin(v1)/v1;
	}
	if (u1!=0) {
		Ain = Ain*2.0*gsl_sf_bessel_J1(u1)/u1;
	}

	if (v2==0) {
		Aout = 1;
	} else {
		Aout = sin(v2)/v2;
	}
	if (u2!=0) {
		Aout = Aout*2.0*gsl_sf_bessel_J1(u2)/u2;
	}

	A = b1*Ain+b2*Aout;
	return A;
}

scalar PcylSHell(sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,v1,v2,b1,b2, J1;

	u1 = Q*r(R,EPSILON,THETA)*sin(ALPHA);
	v1 = Q*L*0.5*cos(ALPHA);
	u2 = Q*(r(R,EPSILON,THETA)+T)*sin(ALPHA);
	v2 = Q*(L*0.5+T)*cos(ALPHA);
	b1 = M_PI*R*R*EPSILON*L*(ETA_CORE-ETA_SH);
	b2 = M_PI*(R+T)*(R+T)*EPSILON*(L+2*T)*(ETA_SH-ETA_SOL);

	Ain  = gsl_sf_bessel_j0(v1)*2*sasfit_jinc(u1);
	Aout = gsl_sf_bessel_j0(v2)*2*sasfit_jinc(u2);

	A = b1*Ain+b2*Aout;
	return A*A;
}

scalar PcylSHellCS(scalar x,sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,b1,b2, J1;

	u1 = Q*r(R,EPSILON,x);
	u2 = Q*(r(R,EPSILON,x)+T);

	b1 = M_PI*R*R*EPSILON*(ETA_CORE-ETA_SH);
	b2 = M_PI*(R+T)*(R+T)*EPSILON*(ETA_SH-ETA_SOL);

	Ain  = 2*sasfit_jinc(u1);
	Aout = 2*sasfit_jinc(u2);

	A = b1*Ain+b2*Aout;
	return A*A;
}
scalar P_cs(scalar q, sasfit_param *param)
{
	Q = q;
	if (EPSILON == 1.0) {
		return PcylSHellCS(1.0,param);
	} else {
		return 2.0/M_PI*sasfit_integrate(0.0,M_PI/2.0,&PcylSHellCS,param);
	}
}
scalar RM_PRISM_theta(scalar x,sasfit_param * param)
{
	THETA = x;
	return PcylSHell(param);
}

scalar RM_PRISM_alpha(scalar x,sasfit_param * param)
{
	ALPHA = x;

	if (EPSILON == 1.0) {
		return RM_PRISM_theta(1.0,param)*sin(ALPHA);
	} else {
		return sasfit_integrate(0.0,M_PI/2.0,&RM_PRISM_theta,param)*2.0/M_PI*sin(ALPHA);
	}
}

scalar sasfit_ff_rodlike_micelle_plusprism(scalar q, sasfit_param * param)
{
	scalar P_RODSH,ProdSQ,  SQ;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_CORE < 0.0), param, "eta_core(%lg) < 0",ETA_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SH < 0.0), param, "eta_shell(%lg) < 0",ETA_SH); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOL < 0.0), param, "eta_sol(%lg) < 0",ETA_SOL); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI); // modify condition to your needs

	Q = q;
	if (sasfit_eps_get_iter_4_mc() <= 0.0) {
		P_RODSH = 1.0;
	} else {
		if (((int) floor(HOW/10)) == 1) {
			P_RODSH = P_L(q,param)*P_cs(q,param);
		} else {
			P_RODSH = sasfit_integrate(0.0,M_PI/2.0,&RM_PRISM_alpha,param);
		}
	}
	ProdSQ = P_Rod(q,L-2.0*R);

	SQ=1.0/(1.0+beta_cq(q,param)*ProdSQ);
/*
    Bapp =   2.0/3.0*M_PI*gsl_pow_3(DEFF)/sasfit_ff_rodlike_micelle_plusprism_v(q,param,0)
			*(1.0-gsl_pow_2(DEFF*q)/10.0);
	if (Bapp < 0) {
		SQ = 1;
	} else {
		SQ=1.0/(1.0+2.0*Bapp*PHI);
	}
*/
	return P_RODSH*SQ;
}

scalar sasfit_ff_rodlike_micelle_plusprism_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_rodlike_micelle_plusprism_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return M_PI*(R+T)*(R*EPSILON+T)*L;
}

