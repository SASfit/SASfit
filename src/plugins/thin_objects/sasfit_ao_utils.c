/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@pssi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>


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

	if (dS<=0) {
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




scalar thinF1(scalar q, scalar r, scalar epsilon, scalar alpha)
{
    scalar u, Rtmp;

	Rtmp = r*gsl_hypot(sin(alpha),epsilon*cos(alpha));

	u = q * Rtmp;

    return gsl_sf_bessel_j0(u);
}


scalar thinEll_P_core(scalar alpha, sasfit_param * param)
{
	scalar S, ae, lEPSILON, lR,lQ;

	lEPSILON = param->p[1];
	lQ		 = param->p[MAXPAR-1];
	lR		 = param->p[MAXPAR-2];

	SASFIT_ASSERT_PTR(param);

	if (lEPSILON < 1.)
	{
		ae = acos(lEPSILON);
		S = 2.*M_PI*lR*lR*(1.+lEPSILON*gsl_atanh(sin(ae))/sin(ae));
	}
	else if (lEPSILON > 1.)
	{
		ae = acos(1./lEPSILON);
		S = 2.*M_PI*lR*lR*(1.+lEPSILON*ae/tan(ae));
	} else
	{
		S = 4.*M_PI*lR*lR;
	}
	return  pow(S * thinF1(lQ, lR, lEPSILON, alpha),2.0)*sin(alpha);
}

scalar thinEll_FtoP(sasfit_param * param)
{
	scalar S, ae, lEPSILON, lR,lQ;

	lEPSILON = EPSILON_SQ_ELLSH;
	lQ		 = Q;
	lR		 = R_SQ_ELLSH*NU1;

	SASFIT_ASSERT_PTR(param);

	if (lEPSILON < 1.)
	{
		ae = acos(lEPSILON);
		S = 2.*M_PI*lR*lR*(1.+lEPSILON*gsl_atanh(sin(ae))/sin(ae));
	}
	else if (lEPSILON > 1.)
	{
		ae = acos(1./lEPSILON);
		S = 2.*M_PI*lR*lR*(1.+lEPSILON*ae/tan(ae));
	} else
	{
		S = 4.*M_PI*lR*lR;
	}
	return  LNDISTR1*gsl_pow_int(S * thinF1(lQ, lR, lEPSILON, ALPHA),lround(P))*sin(ALPHA);
}

int thin_ellip_shell_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	if ((ndim == 1) && (SIGMA_R_SQ_ELLSH==0)) {
		LNDISTR1 = 1;
		NU1 = 1;
		ALPHA=x[0];
	} else if ((ndim == 1) && (EPSILON_SQ_ELLSH==1)) {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R_SQ_ELLSH;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU1=x[0];
	    ALPHA = M_PI_2;
		LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R_SQ_ELLSH;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		ALPHA = x[0];
		NU1 = x[1];
		LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA_R_SQ_ELLSH);
			return 1;
		}
	}
    fval[0] = (cparam->func)(param);
    return 0;
}

scalar thin_ellipsoidal_shell(scalar q, scalar R, scalar epsilon, scalar sigma, sasfit_param * param)
{
	scalar res,err,sum, Rmin, Rmax;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, ndim;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    EPSILON_SQ_ELLSH = epsilon;
    R_SQ_ELLSH = R;
    SIGMA_R_SQ_ELLSH = sigma;
	NU1 = 1;
	LNDISTR1=1.0;
	ndim  =2;
	if (SIGMA_R_SQ_ELLSH==0) {
		ndim=1;
		NU1MIN = 1;
		NU1MAX = 1;
	} else {
		ndim  =2;
		find_LogNorm_int_range(2*P,1.0,SIGMA_R_SQ_ELLSH,&Rmin, &Rmax, param);
		NU1MIN=Rmin;
		NU1MAX=Rmax;
	}
	if (EPSILON_SQ_ELLSH==1) {
        ndim=ndim-1;
        ALPHA = M_PI_2;
        cubxmin[0]=NU1MIN;
        cubxmax[0]=NU1MAX;
	} else {
	    cubxmin[0]=0;
        cubxmax[0]=M_PI_2;
        cubxmin[1]=NU1MIN;
        cubxmax[1]=NU1MAX;
	}
    if (ndim==0) {
        NU1 = 1;
        LNDISTR1=1.0;
        ALPHA = M_PI_2;
        return thinEll_FtoP(param);
    }
	cparam.ndim=ndim;
	cparam.func = &thinEll_FtoP;
    cparam.param = param;

	intstrategy = sasfit_get_int_strategy();
	intstrategy=P_CUBATURE;
	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &thin_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &thin_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            pcubature(1, &thin_ellip_shell_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    }
	return sum;
}

scalar thinEll_P(scalar x, sasfit_param * param)
{
	scalar lEPSILON;
	lEPSILON = param->p[1];

	if (x==0) return 0.0;
	param->p[MAXPAR-2]=x;

	SASFIT_ASSERT_PTR(param);
	if (lEPSILON == 1) {
		return thinEll_P_core(M_PI/2.0,param);
	} else {
		return sasfit_integrate(0.0, M_PI/2.0, &thinEll_P_core, param);
	}
}

scalar ThinEllShell_R_core(scalar x, sasfit_param * param)
{
	scalar IP, LNdistr, lR,lQ, lSIGMA_R, lEPSILON;
	sasfit_param subParam;

	if (x==0) return 0.0;

	lR = param->p[0];
	lEPSILON = param->p[1];
	lSIGMA_R = param->p[2];
	lQ = param->p[MAXPAR-1];

	SASFIT_ASSERT_PTR(param);

	IP = thinEll_P(x, param);

	if (lSIGMA_R != 0) {
		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = lSIGMA_R;
		subParam.p[2] = 1.0;
		subParam.p[3] = lR;
		LNdistr = sasfit_sd_LogNorm(x, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
	} else {
		LNdistr = 1;
	}

	return LNdistr*IP;
}


scalar r0(scalar epsilon, scalar s0)
{
	scalar r0, ae;
	if (epsilon < 1.)
	{
		ae = acos(epsilon);
		r0 = sqrt(s0/(1.+epsilon*gsl_atanh(sin(ae))/sin(ae)) / (2.*M_PI));
	}
	else if (epsilon > 1.)
	{
		ae = acos(1./epsilon);
		r0 = sqrt(s0/(1.+epsilon*ae/tan(ae)) / (2.*M_PI));
	} else
	{
		r0 = sqrt(s0 / 4./M_PI);
	}
	return r0;
}

scalar ThinEllShell_S_core(scalar x, sasfit_param * param)
{
	scalar IP, LNdistr, lS, lQ, lSIGMA_S, lEPSILON;
	sasfit_param subParam;

	lS = param->p[0];
	lEPSILON = param->p[1];
	lSIGMA_S = param->p[2];
	lQ = param->p[MAXPAR-1];

	SASFIT_ASSERT_PTR(param);

	param->p[MAXPAR-2] = r0(lEPSILON, lS);

	IP = thinEll_P(lQ, param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = lSIGMA_S;
	subParam.p[2] = 1.0;
	subParam.p[3] = lS;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*IP;
}

scalar PS3_EXP_SD_core(scalar x, void * pam) {
	sasfit_param *param;
	sasfit_param subParam;
	scalar NL;
	param = (sasfit_param *) pam;

	sasfit_init_param( &subParam );
	if (x > LB/3) {
        subParam.p[0] = LB;
        subParam.p[1] = x;
        subParam.p[2] = EXVOL;
        NL = exp(-x/param->p[1])/param->p[1];
        return NL * sasfit_sq_p__q___worm_ps3_(Q,&subParam);
	} else {
        subParam.p[0] = x;
        subParam.p[1] = x;
        subParam.p[2] = 0;
        NL = exp(-x/param->p[1])/param->p[1];
        return NL * sasfit_sq_p__q___rod(Q,&subParam);
	}
}

scalar PS3_EXP_SD(scalar q, sasfit_param * param) {
    scalar res,err, *aw;
    int lenaw=4000;
    Q=q;
    aw = (scalar *)malloc((lenaw)*sizeof(scalar));
    sasfit_intdeiini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
    sasfit_intdei(&PS3_EXP_SD_core, 0.0, aw, &res, &err, param);
    free(aw);
    return res;
}
