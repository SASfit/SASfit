/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include "include/private.h"
#include <sasfit_error_ff.h>
#include <sasfit_utils_f2c.h>

// define shortcuts for local parameters/variables
#define RSH0	param->p[0]
#define THETA1	param->p[1]*M_PI/180.0
#define THETA2	param->p[2]*M_PI/180.0
#define T_SH	param->p[3]
#define T_C		param->p[4]
#define DETA_SH	param->p[5]
#define DETA_CE	param->p[6]
#define SIGMA   param->p[7]
#define PSI_DEG	param->p[8]
#define KAPPA	param->p[9]
#define Q		param->p[MAXPAR-1]
#define RKERNEL param->p[MAXPAR-2]
#define GAMA	param->p[MAXPAR-3]
#define _THETA	param->p[MAXPAR-4]
#define PHI		param->p[MAXPAR-5]
#define PSI		param->p[MAXPAR-6]
#define RSH		param->p[MAXPAR-7]


#define Power pow
#define Cos cos
#define Sin sin
#define Pi M_PI
#define Sec 1./cos
#define Csc 1./sin


scalar mKernelreal(scalar z, sasfit_param *param) 
{
	scalar res, tmp;
	res = cos(Q*z*cos(GAMA))*Pi*(RKERNEL*RKERNEL-z*z);
	tmp =Q*sqrt(RKERNEL*RKERNEL-z*z)*sin(GAMA);
	if (fabs(tmp) > 0.0) {
		return res*2.0*gsl_sf_bessel_J1(tmp)/tmp;
	} else {
		return res;
	}
}

scalar mKernelimag(scalar z, sasfit_param *param) 
{
	scalar res, tmp;
	res = sin(Q*z*cos(GAMA))*Pi*(RKERNEL*RKERNEL-z*z);
	tmp =Q*sqrt(RKERNEL*RKERNEL-z*z)*sin(GAMA);
	if (fabs(tmp) > 0.0) {
		return res*2.0*gsl_sf_bessel_J1(tmp)/tmp;
	} else {
		return res;
	}
}

void sasfit_cappedSphere(scalar q,
						 sasfit_param *param,
						 scalar R,
						 scalar theta1, 
						 scalar theta2,
						 scalar *Freal,
						 scalar *Fimag)
{
			RKERNEL = R;
			Q = q;
			if (R == 0) {
				*Freal = 0.0;
				*Fimag = 0.0;
			} else {
				*Freal = sasfit_integrate(-RKERNEL*cos(theta2), RKERNEL*cos(theta1), mKernelreal, param);
				if (theta1 == theta2) {
					*Fimag = 0.0;
				} else {
					*Fimag = sasfit_integrate(-RKERNEL*cos(theta2), RKERNEL*cos(theta1), mKernelimag, param);
				}
			}
			return;
}

void sasfit_Cylinder4cappedSphere(scalar q,
								  scalar R,
								  scalar a,
								  scalar b,
								  scalar alpha,
								  scalar *Freal,
								  scalar *Fimag)
{
	if (q==0.0) {
		*Freal = (b-a)*Pi*R*R;
		*Fimag = 0.0;
	} else if (alpha == 0.0) {
		*Freal = -((Pi*Power(R,2)*(Sin(a*q) - Sin(b*q)))/q);
		*Fimag =   (Pi*Power(R,2)*(Cos(a*q) - Cos(b*q)))/q;
	} else {
		*Freal = (-8*Pi*R*gsl_sf_bessel_J1(q*R*Sin(alpha))
			       * Cos(((a + b)*q*Cos(alpha))/2.)
				   * Csc(2*alpha)
				   * Sin(((a - b)*q*Cos(alpha))/2.)
				 )/Power(q,2);
		*Fimag = ( 4*Pi*R*gsl_sf_bessel_J1(q*R*Sin(alpha))
			       *(Cos(a*q*Cos(alpha)) - Cos(b*q*Cos(alpha)))
				   * Csc(2*alpha)
				 )/Power(q,2);
	}
	return;
}

scalar sasfit_ff_magneticfieldalignedvesicle_util_psi(scalar q, sasfit_param * param)
{	
	scalar R,t, 
		   lc1, lc2,tdisc1, tdisc2,
		   Rc1, Rc2, 
		   theta1P, theta2P,
		   Deta_sh, Deta_c;
	scalar Fsphreal, Fsphimag;
	scalar Freal , Fimag;
	scalar tmpFreal , tmpFimag;
	scalar FD1real, FD1imag;
	scalar FD2real, FD2imag, IQ;
	int i,imax;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RSH < 0.0), param, "Rsh(%lg) < 0",RSH); 
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "t_sh(%lg) < 0",T_SH); 
	SASFIT_CHECK_COND1((T_C < 0.0), param, "t_c(%lg) < 0",T_C);  

	// insert your code here
	R = RSH;
	t = T_SH;
	if (R*10 < t) {
		return gsl_pow_2(DETA_SH*4.0*M_PI*(sin(Q*t)-Q*t*cos(Q*t))/(Q*Q*Q));
	}
	
	Deta_sh = DETA_SH;
	Deta_c  = DETA_CE;

	lc1 = RSH*cos(THETA1);
	lc2 = RSH*cos(THETA2);
	Rc1 = RSH*sin(THETA1);
	Rc2 = RSH*sin(THETA2);

	if (fabs(RSH+T_SH)>0) {
		theta1P = asin(Rc1/(RSH+T_SH));
		theta2P = asin(Rc2/(RSH+T_SH));
	} else {
		theta1P = 0.0;
		theta2P = 0.0;
	}

	tdisc1 = sqrt((R+t)*(R+t)-Rc1*Rc1)-sqrt(R*R-Rc1*Rc1);
	tdisc2 = sqrt((R+t)*(R+t)-Rc2*Rc2)-sqrt(R*R-Rc2*Rc2);

	IQ = 0.0;
	imax = 0;
	for (i=1;i<=1;i++) {
		imax++;
		if (fabs(q) > 0) {
			sasfit_cappedSphere(q * (1+0.0*gsl_pow_int(-1,i+1)),param,R+T_SH,theta1P,theta2P,&tmpFreal,&tmpFimag);
			sasfit_cappedSphere(q * (1+0.0*gsl_pow_int(-1,i+1)),param,R     ,THETA1 ,THETA2 ,&Fsphreal,&Fsphimag);
			
			Fsphreal = Deta_sh*(tmpFreal-Fsphreal);
			Fsphimag = Deta_sh*(tmpFimag-Fsphimag);
			
			sasfit_Cylinder4cappedSphere(q,Rc1,lc1,lc1+tdisc1,GAMA,&tmpFreal,&tmpFimag);
			FD1real = - Deta_sh * tmpFreal;
			FD1imag = - Deta_sh * tmpFimag;

			sasfit_Cylinder4cappedSphere(q,Rc2,-(lc2+tdisc2),-lc2,GAMA,&tmpFreal,&tmpFimag);
			FD2real = - Deta_sh * tmpFreal;
			FD2imag = - Deta_sh * tmpFimag;

			sasfit_Cylinder4cappedSphere(q,Rc1,lc1,lc1+T_C,GAMA,&tmpFreal,&tmpFimag);
			FD1real = FD1real + Deta_c * tmpFreal;
			FD1imag = FD1imag + Deta_c * tmpFimag;

			sasfit_Cylinder4cappedSphere(q,Rc2,-(lc2+T_C),-lc2,GAMA,&tmpFreal,&tmpFimag);
			FD2real =  FD2real + Deta_c * tmpFreal;
			FD2imag =  FD2imag + Deta_c * tmpFimag;
		} else {
			Fsphreal = Deta_sh*
						(Pi*Power(R,3)*(- 9*Cos(THETA1) 
					                    +   Cos(3*THETA1) 
									    - 9*Cos(THETA2) 
									    +   Cos(3*THETA2)
									  )
						)/12.;
		
			FD1real  = -Deta_sh*Pi*pow(Rc1,2)*tdisc1;
			FD2real  = -Deta_sh*Pi*pow(Rc2,2)*tdisc2;

			FD1real = FD1real + Deta_c*Pi*pow(Rc1,2)*T_C;
			FD2real = FD2real + Deta_c*Pi*pow(Rc2,2)*T_C;
			
			Fsphimag = 0.0;
			FD1imag = 0.0;
			FD2imag = 0.0;
		}


		Freal = FD1real + FD2real + Fsphreal;
		Fimag = FD1imag + FD2imag + Fsphimag;
		IQ = IQ + Freal*Freal+Fimag*Fimag;
	}
	return IQ/imax;
}
