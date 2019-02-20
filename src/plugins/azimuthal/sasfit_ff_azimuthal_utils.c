/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
 void STVLV_2nd(double V, double X, double *SLV) {
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

// define shortcuts for local parameters/variables

scalar OnsagerODF(scalar x, scalar kappa) {
// Onsager orientation distribution
    scalar c,p,Lv1,theta,intfrac;
    theta=abs(modf(x/M_PI,&intfrac)*M_PI);
    if (kappa == 0) return 0.25/M_PI;
    if (kappa > 10 ) {
        p = 0.5*kappa*exp(gsl_sf_lncosh(kappa*cos(x))-gsl_sf_lnsinh(kappa));
    } else if (kappa > 0 && kappa <= 10){
        p = 0.5*kappa*cosh(kappa*cos(theta))/sinh(kappa);
    } else if (kappa < 0) {
        STVLV_2nd(-1, -kappa, &Lv1);
        p = cosh(kappa*sin(theta))/(M_PI*Lv1);
    }
    return p/(2*M_PI);
}

scalar MaierSaupeODF(scalar psi, scalar kappa) {
// Maier-Saupe orientation distribution
scalar u, norm;
	if (kappa < 0) {
		u = sqrt(-kappa);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	}
	else if (kappa == 0.0) {
		norm = 2;
	}
	else {
		u = sqrt(kappa);
		norm = 2.0*exp(kappa)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(ksppa*cos(psi)^2)*sin(psi), psi = 0 .. Pi), dawson)], [kappa > 0])
	}
    return exp(kappa*gsl_pow_2(cos(psi)))/norm;
}

scalar LNKernel(scalar theta, scalar psi) {
    return sin(theta)/(gsl_pow_2(cos(psi))*sqrt(gsl_pow_2(tan(theta))-gsl_pow_2(tan(psi))));
}

scalar KratkyKernel(scalar theta, scalar psi) {
//    return sin(theta)/ sqrt(gsl_pow_2(sin(theta))-gsl_pow_2(sin(psi)));
    return tan(theta)/(cos(psi)         *sqrt(gsl_pow_2(tan(theta))-gsl_pow_2(tan(psi))));
}

scalar psi_quarter_rad(scalar psi) {
    scalar tp,tf, ti;
    tf = modf(fabs(psi/M_PI),&ti)*M_PI;
    ti = ti*M_PI;
    if (tf <= M_PI_2) {
        return tf;
    } else {
        return M_PI-tf;
    }
}

scalar psi_quarter_deg(scalar psi) {
    scalar tp,tf, ti;
    tf = modf(fabs(psi/180.),&ti)*180.;
    ti = ti*180.;
    if (tf <= 90.) {
        return tf;
    } else {
        return 180.0-tf;
    }
}
