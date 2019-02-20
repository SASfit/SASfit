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
    scalar norm,u,p,theta,intfrac;
    theta=abs(modf(x/M_PI,&intfrac)*M_PI);
    if (KAPPA == 0) return 0.25/M_PI;
    if (KAPPA < 0) {
		u = sqrt(-KAPPA);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	} else {
		u = sqrt(KAPPA);
		norm = 2.0*exp(KAPPA)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(P2*cos(THETA)^2)*sin(THETA), theta = 0 .. Pi), dawson)], [kappa > 0])
	}
    p = exp(KAPPA*gsl_pow_2(cos(theta)))/norm;
    return p/(2*M_PI);
}

scalar pGauss(scalar x,scalar y, sasfit_param * param) {
    scalar c,p,theta,k,intfrac;
    theta = abs(modf(x/M_PI,&intfrac)*M_PI);
    if (KAPPA == 0) return 1./(4.*M_PI);
    if (KAPPA > 0) {
        c =  2*gsl_sf_dawson(1./(2.*KAPPA))/KAPPA
           + sqrt(M_PI)/KAPPA*exp(-1./gsl_pow_2(2*KAPPA))*re_cerfi_z(-0.5/KAPPA,M_PI_2*KAPPA);
        if (fabs(theta) <= M_PI_2) {
            p = exp(-gsl_pow_2(fabs(theta)*KAPPA))/c;
        } else {
            p = exp(-gsl_pow_2((M_PI-fabs(theta))*KAPPA))/c;
        }
    } else {
        k = fabs(KAPPA);
        c = sqrt(M_PI)/k*exp(-1./gsl_pow_2(2*k))*im_cerfi_z(-0.5/k,M_PI_2*k);
        p = exp(-gsl_pow_2((M_PI_2-fabs(theta))*k))/c;
    }
    return p/(2*M_PI);
}

scalar pBoltzmann(scalar x,scalar y, sasfit_param * param) {
    scalar c,p,theta,intfrac;
    theta = fabs(modf(x/M_PI,&intfrac)*M_PI);
    if (KAPPA == 0) return 0.25/M_PI;
    c = -(4*M_PI)*(KAPPA*exp(-M_PI_2*KAPPA)-1.0)/(KAPPA*KAPPA+1.0);
    if (theta<M_PI_2) {
        p = exp(-fabs(theta)*KAPPA)/c;
    } else {
        p = exp(-(M_PI-fabs(theta))*KAPPA)/c;
    }
    return p;
}

scalar pOnsager(scalar x,scalar y, sasfit_param * param) {
    scalar c,p,Lv1,theta,intfrac;
    theta=abs(modf(x/M_PI,&intfrac)*M_PI);
    if (KAPPA == 0) return 0.25/M_PI;
    if (KAPPA > 10 ) {
        p = 0.5*KAPPA*exp(gsl_sf_lncosh(KAPPA*cos(x))-gsl_sf_lnsinh(KAPPA));
    } else if (KAPPA > 0 && KAPPA <= 10){
        p = 0.5*KAPPA*cosh(KAPPA*cos(theta))/sinh(KAPPA);
    } else if (KAPPA < 0) {
        STVLV(-1, -KAPPA, &Lv1);
        p = cosh(KAPPA*sin(theta))/(M_PI*Lv1);
    }
    return p/(2*M_PI);
}

scalar pHayterPenfold(scalar x,scalar y, sasfit_param * param) {
    scalar phi,theta,twophi0,phi0;
    phi	= y;
	theta 	= x;

    if (KAPPA==0) {
        return 0.25/M_PI;
    } else {
        twophi0 = atan(8.0/KAPPA);
    }
    phi0 = twophi0/2.0;
    return   (1.0-cos(twophi0))*sqrt(gsl_pow_3(1.0+gsl_pow_2(sin(theta))*cos(twophi0)))
                /  (4.0*M_PI*gsl_pow_2(1.0-gsl_pow_2(sin(theta))*cos(twophi0)*cos(2.0*(phi-phi0))));
}

scalar pHeavysidePi(scalar x,scalar y, sasfit_param * param) {
    scalar c,k,theta,intfrac;
    theta=abs(modf(x/M_PI,&intfrac)*M_PI);

    if (fabs(KAPPA)<0.1) return 0.25/M_PI;

    if (KAPPA > 0) {
        k=1./KAPPA;
        if (k <= M_PI_2) {
            c = 4.*M_PI*(1.-cos(k));
        } else {
            c = 4.*M_PI;
        }
        if (fabs(theta)<k) return 1./c;
        if (fabs(M_PI-theta)<k) return 1./c;
        return 0;
    } else {
        k=1./fabs(KAPPA);
        if (k<= M_PI_2) {
            c = 4.*M_PI*sin(k);
        } else {
            c = 4.*M_PI;
        }
        if (fabs(theta-M_PI_2)<k) return 1./c;
        return 0;
    }
    return GSL_NAN;
}

scalar pLegendre(scalar x,scalar y, sasfit_param * param) {
    scalar pdistr, Sl[7], Pl[7], P2, P4, P6,theta,intfrac;
	int l;

    theta=abs(modf(x/M_PI,&intfrac)*M_PI);

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

	pdistr=0.5;
    gsl_sf_legendre_Pl_array(7,cos(theta),Pl);
	for (l=1;l<=6;l++) {
		pdistr = pdistr+(l+0.5)*Sl[l]*gsl_sf_legendre_Pl(l,cos(theta));
	}
	return pdistr/(2*M_PI);

}

scalar gamHPplus(scalar psi, scalar theta, scalar phi) {
    return acos(  cos(psi) * sin(theta) * cos(phi) + sin(psi) * cos(theta) );
}
scalar gamHPminus(scalar psi, scalar theta, scalar phi) {
    return acos( -cos(psi) * sin(theta) * cos(phi) + sin(psi) * cos(theta) );
}

scalar gamOthers(scalar psi, scalar theta, scalar phi) {
    return acos(  cos(psi) * cos(theta) + sin(psi) * sin(theta) * cos(phi));
    return acos(  cos(psi) * cos(theta) + sin(psi) * sin(theta) * sin(phi));
}

scalar gamTheta(scalar psi, scalar theta, scalar phi) {
    return theta;
}


scalar f_sph(scalar x) {
	if (fabs(x) < 1e-6)	{
		return 1-gsl_pow_2(x)/10.+gsl_pow_4(x)/280.0-gsl_pow_6(x)/15120.+gsl_pow_8(x)/1330560.;
	} else {
		return 3*gsl_sf_bessel_j1(x)/x;
	}
}

scalar alignedEllSh(sasfit_cubature_g *gam, sasfit_param * param) {
	scalar xc, xt, Vc, Vt;

	scalar gama, psi, cosgama, singama;

	SASFIT_ASSERT_PTR(param);

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);

    gama = (*gam)(psi, THETA, PHI);

	xc = Q*sqrt(gsl_pow_2(A*cos(gama))+ gsl_pow_2(B)*(1-cos(gama)*cos(gama)));
	xt = Q*sqrt(gsl_pow_2((A+T)*cos(gama))+ gsl_pow_2(B+T)*(1-cos(gama)*cos(gama)));
	Vc = 4./3.*M_PI*A*gsl_pow_2(B);
	Vt = 4./3.*M_PI*(A+T)*gsl_pow_2(B+T);
	return 	 gsl_sf_pow_int( (ETA_CORE-ETA_SHELL)*Vc*f_sph(xc)+
			(ETA_SHELL-ETA_SOLV)*Vt*f_sph(xt),lround(P));
}

scalar alignedCylShell(sasfit_cubature_g *gam, sasfit_param * param)
{
	scalar gama, psi, cosgama, singama;

	SASFIT_ASSERT_PTR(param);

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);

    gama = (*gam)(psi, THETA, PHI);
    return gsl_sf_pow_int(  (ETA_CORE-ETA_SHELL)*M_PI*gsl_pow_2(R)  *L
                                                *gsl_sf_hyperg_0F1(2,-0.25*gsl_pow_2(Q* R   *sin(gama)))
                                                *2*gsl_sf_bessel_j0(Q*L*cos(gama)/2.)+
                            (ETA_SHELL-ETA_SOLV)*M_PI*gsl_pow_2(R+T)*L
                                                *gsl_sf_hyperg_0F1(2,-0.25*gsl_pow_2(Q*(R+T)*sin(gama)))
                                                *2*gsl_sf_bessel_j0(Q*L*cos(gama)/2.) ,lround(P));
}

int partly_aligned_cubature(unsigned ndim, const double *x, void *pam,
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
	THETA = x[0];
	PHI = x[1];
	NR=NU;
	NT=NU;
	NL=NU;
    fval[0] = sin(THETA)* LNDISTR * (cparam->p1)(THETA,PHI,param) * (cparam->func)(cparam->gam,param);
    return 0;
}

int partly_aligned_cubature_u_phi(unsigned ndim, const double *x, void *pam,
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
	THETA = acos(x[0]);
	PHI = x[1];
	NR=NU;
	NT=NU;
	NL=NU;
    fval[0] = LNDISTR * (cparam->p1)(THETA,PHI,param) * (cparam->func)(cparam->gam,param);
    return 0;
}

int random_oriented_cubature(unsigned ndim, const double *x, void *pam,
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
	if ((ndim < 2) || (SIGMA==0)) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU=x[1];
		LNDISTR = sasfit_sd_LogNorm(NU, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	THETA = x[0];
	PHI = 0;
	NR=NU;
	NT=NU;
	NL=NU;
    fval[0] = sin(THETA)* LNDISTR * (cparam->func)(cparam->gam,param);
    return 0;
}
