/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// define shortcuts for local parameters/variables

void STVLV(double V, double X, double *SLV) {
/*      ======================================================
!       Purpose:  Compute modified Struve function Lv(x) with
!                 an arbitrary order v
!       Input :   v   --- Order of Lv(x)  ( |v| ó 20 )
!                 x   --- Argument of Lv(x) ( x ò 0 )
!       Output:   SLV --- Lv(x)
!       Routine called: GAMMA to compute the gamma function
!       ====================================================== */
        double BIV, BIV0, GA, GB, PI, R, R1, R2, S, S0, SA, U, U0, V0, VA, VB, VT;
		double BF, BF0, BF1;
		int K, L, N;
	    PI=3.141592653589793;
        if (X == 0.0) {
           if (V > -1.0 || ((int)V)-V == 0.5)
              *SLV=0.0;
           else if (V < -1.0)
              *SLV=gsl_pow_int(-1, (int)(0.5-V)-1)*1.0e+300;
           else if (V == -1.0)
              *SLV=2.0/PI;
		   return;
        }
        if (X <= 40.0) {
           V0=V+1.5;
           GA = gsl_sf_gamma(V0);
           S=2.0/(sqrt(PI)*GA);
           R1=1.0;
           for (K=1; K<=100; K++) {
              VA=K+1.5;
              GA = gsl_sf_gamma(VA);
              VB=V+K+1.5;
              GB = gsl_sf_gamma(VB);
              R1=R1*(0.5*X)*(0.5*X);
              R2=R1/(GA*GB);
              S=S+R2;
              if (fabs(R2/S) < 1.0e-12) goto e15;
	   }
e15:       *SLV=pow(0.5*X,V+1.0)*S;
        }
        else {
           SA=-1.0/PI*pow(0.5*X, V-1.0);
           V0=V+0.5;
           GA = gsl_sf_gamma(V0);
           S=-sqrt(PI)/GA;
           R1=-1.0;
           for (K=1; K<13; K++) {
              VA=K+0.5;
              GA = gsl_sf_gamma(VA);
              VB=-K+V+0.5;
              GB = gsl_sf_gamma(VB);
              R1=-R1/pow(0.5*X,2);
              S += R1*GA/GB;
		   }
           S0=SA*S;
           U=fabs(V);
           N=(int)U;
           U0=U-N;
           for (L=0; L<2; L++) {
              VT=U0+L;
              R=1.0;
              BIV=1.0;
              for (K=1; K<17; K++) {
                 R=-0.125*R*(4.0*VT*VT-pow(2.0*K-1.0,2))/(K*X);
                 BIV += R;
                 if (fabs(R/BIV) < 1.0e-12) goto e30;
	      }
e30:          if (L == 0)  BIV0=BIV;
	   }
           BF0=BIV0;
           BF1=BIV;
           for (K=2; K<=N; K++) {
              BF=-2.0*(K-1.0+U0)/X*BF1+BF0;
              BF0=BF1;
              BF1=BF;
           }
           if (N == 0) BIV=BIV0;
           if (N > 1)  BIV=BF;
           *SLV=exp(X)/sqrt(2.0*PI*X)*BIV+S0;
        }
}

#include "include/private.h"
#include <sasfit_error_ff.h>


scalar pMaierSaupe(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,norm,u,p;
    kappa = param->p[8];
    if (kappa < 0) {
		u = sqrt(-kappa);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	} else if (kappa == 0.0) {
		norm = 2;
	} else {
		u = sqrt(kappa);
		norm = 2.0*exp(kappa)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(P2*cos(THETA)^2)*sin(THETA), theta = 0 .. Pi), dawson)], [kappa > 0])
	}
    p = exp(kappa*gsl_pow_2(cos(x)))/norm;
    return p/(2*M_PI);
}

scalar pGauss(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,c,p,k;
    kappa = param->p[8];
    if (kappa > 0) {
        c =  gsl_sf_dawson(1./(2.*kappa))/kappa
           +0.5* sqrt(M_PI)/kappa*exp(-1./gsl_pow_2(2*kappa))*re_cerfi_z(-0.5/kappa,M_PI_2*kappa);
        if (x <= M_PI_2) {
            p = exp(-gsl_pow_2(x*kappa))/c;
        } else {
            p = exp(-gsl_pow_2((M_PI-x)*kappa))/c;
        }
    } else {
        k = fabs(kappa);
        c = sqrt(M_PI)/k*exp(-1./gsl_pow_2(2*k))*im_cerfi_z(-0.5/k,M_PI_2*k);
        p = exp(-gsl_pow_2((M_PI_2-x)*k))/c;
    }
    return p/(2*M_PI);
}

scalar pBoltzmann(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,c,p,k;
    kappa = param->p[8];
    k = (kappa);
    c = -(4*M_PI)*(k*exp(-0.5*M_PI*k)-1.0)/(k*k+1.0);
    if (x<M_PI_2) {
        p = exp(-x*kappa)/c;
    } else {
        p = exp(-(M_PI-x)*kappa)/c;
    }
    return p;

    c=2*kappa*kappa*(1+exp(-M_PI/kappa))/(1+kappa*kappa);
    return (exp(-x*kappa)+exp(-(M_PI-x)*kappa))/c;
}

scalar pOnsager(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,c,p,Lv1;
    kappa = param->p[8];
    if (kappa > 10 ) {
        p = 0.5*kappa*exp(gsl_sf_lncosh(kappa*cos(x))-gsl_sf_lnsinh(kappa));
    } else if (kappa >= 0 && kappa <= 10){
        p = 0.5*kappa*cosh(kappa*cos(x))/sinh(kappa);
    } else if (kappa < 0) {
        STVLV(-1, -kappa, &Lv1);
        p = cosh(kappa*sin(x))/(M_PI*Lv1);
    }
    return p/(2*M_PI);
}

scalar pHayterPenfold(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,phi,theta,twophi0,phi0;
    phi	= y;
	theta 	= x;
    kappa = param->p[8];

    twophi0 = atan(8.0/kappa);
    phi0 = twophi0/2.0;
    return   (1-cos(twophi0))*pow(1.0+pow(sin(theta),2.0)*cos(twophi0),1.5)
                /  (4.0*M_PI*pow(1.0-pow(sin(theta),2.0)*cos(twophi0)*cos(2.0*(phi-phi0)),2.0));
}

scalar pHeavysidePi(scalar x,scalar y, sasfit_param * param) {
    scalar kappa,c;
    kappa = fabs(param->p[8]);
    if (kappa <= M_PI/2) {
        c = 2*(1-cos(kappa));
    } else {
        c = 2;
    }

    if (kappa > 0) {
        if (fabs(x)<kappa) return 1./c;
        if (fabs(M_PI-x)<kappa) return 1./c;
        return 0;
    }
    return GSL_NAN;
}

scalar pLegendre(scalar x,scalar y, sasfit_param * param) {
    scalar pdistr, Sl[7], Pl[7], P2, P4, P6;
	int l;

    P2 = fabs(param->p[8]);
    P4 = fabs(param->p[9]);
    P6 = fabs(param->p[10]);
// size distribution simulated up to the 6th even order parameter
	Sl[0]=1.0;
	Sl[1]=0.0;
	Sl[2]=P2;
	Sl[3]=0.0;
	Sl[4]=P4;
	Sl[5]=0.0;
	Sl[6]=P6;

	pdistr=0.0;
    gsl_sf_legendre_Pl_array(7,cos(x),Pl);
	for (l=0;l<=6;l++) {
		pdistr = pdistr+(l+0.5)*Sl[l]*Pl[l];
	}
	return pdistr;

}
scalar alignedCylShell(sasfit_param * param)
{
	scalar gama, psi, theta, phi;

	SASFIT_ASSERT_PTR(param);

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
//	theta = THETA_DEG * M_PI/180.;
//	phi   = PHI_DEG * M_PI/180.;
    theta = X;
    phi = Y;

//	gama = acos( sin(theta) * cos(phi) * cos(psi) + cos(theta) * sin(psi) );
	gama = acos( sin(theta) * cos(phi) * sin(psi) + cos(theta) * cos(psi) );

	if ( Q == 0.0 )
	{
		return pow( (ETA_CORE - ETA_SHELL)*R*R*L*M_PI + (ETA_SHELL - ETA_SOLV)*(R+T)*(R+T)*L*M_PI, P);
	}
	if (L == 0.0)
	{
		return 0.0;
	}
	if (R+T == 0.0)
	{
		return 0.0;
	}

	if (gama == M_PI/2.0)
	{
		return pow(2.*gsl_sf_bessel_J1(Q*R    )/Q *L*  R  *M_PI*(ETA_CORE-ETA_SHELL) +
		  	       2.*gsl_sf_bessel_J1(Q*(R+T))/Q *L*(R+T)*M_PI*(ETA_SHELL-ETA_SOLV)  ,P);
	}
	else if (gama == 0.0)
	{
		return pow(2./Q*R    *R    *sin(Q*L*0.5)*(ETA_CORE-ETA_SHELL)*M_PI+
			       2./Q*(R+T)*(R+T)*sin(Q*L*0.5)*(ETA_SHELL-ETA_SOLV)*M_PI  ,P);
	}
	else
	{
		return pow(4.*(ETA_CORE-ETA_SHELL)*M_PI* R   *gsl_sf_bessel_J1(Q* R   *sin(gama))*sin(Q*L*cos(gama)/2.)*pow(Q,-2.)/sin(gama)/cos(gama)+
			       4.*(ETA_SHELL-ETA_SOLV)*M_PI*(R+T)*gsl_sf_bessel_J1(Q*(R+T)*sin(gama))*sin(Q*L*cos(gama)/2.)*pow(Q,-2.)/sin(gama)/cos(gama) ,P);
	}
}

int partly_aligned_cylinders_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	if ((ndim < 3) || (SIGMA==0)) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU=x[2];
		LNDISTR = sasfit_sd_LogNorm(NU, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	X = x[0];
	Y = x[1];
	NR=NU;
	NT=NU;
	NL=NU;
    fval[0] = LNDISTR * (cparam->p1)(X,Y,param) * (cparam->func)(param);
    return 0;
}
