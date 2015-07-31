/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <gsl/gsl_roots.h>

// define shortcuts for local parameters/variables


scalar sinc(scalar x)
{
	if (fabs(x) <= 1e-4) {
		return 1.0-gsl_pow_2(x)/6.0
		          +gsl_pow_4(x)/120.0
		          -gsl_pow_6(x)/5040.0
		          +gsl_pow_8(x)/362880;
	} else {
		return sin(x)/x;
	}
}

double Arg(double x, double y)
{
	if (x > 0) {
		return atan(y/x);
	} else if (x<0 && y>=0) {
		return M_PI+atan(y/x);
	} else if (x<0 && y<0) {
		return -M_PI+atan(y/x);
	}
	return 0.0;
}
double IntPhi(double x, sasfit_param * param)
{	scalar retval,K;
	K=1;

	if (TAU < 0.0) {
		ZMAX = sqrt((2*x+3*gsl_pow_2(TAU))/(2*C));
		retval = -(ZMAX*(2*(36*pow(R_TOT,2)*TAU*pow(x,1.5) - 8*R_TOT*
            (4*TAU*TAU*TAU*sqrt(x) - 3*TAU*pow(x,1.5) + 2*sqrt(2)*TAU*TAU*sqrt(x*(2*TAU*TAU + x)) +
              sqrt(2)*x*sqrt(x*(2*TAU*TAU + x)))*ZMAX + TAU*sqrt(x)*(-6*TAU*TAU + 11*x)*pow(ZMAX,2)) +
        3*sqrt(2)*(2*TAU*TAU + x)*(4*pow(R_TOT,2)*x + (2*TAU*TAU + x)*pow(ZMAX,2))*
         Arg((-(sqrt(2)*TAU)),-sqrt(x)) ))/(24.*pow(x,1.5))-I/K;
	} else if (TAU > 0.0){
		ZMAX = sqrt(x/C);
		retval = -(ZMAX*(2*(12*gsl_pow_2(R_TOT)*TAU*pow(x,1.5) - 8*R_TOT*
            (-4*gsl_pow_3(TAU)*sqrt(x) - 3*TAU*pow(x,1.5) +
			2*sqrt(2)*gsl_pow_2(TAU)*sqrt(x*(2*gsl_pow_2(TAU) + x)) +
              sqrt(2)*x*sqrt(x*(2*gsl_pow_2(TAU) + x)))*ZMAX +
			  TAU*sqrt(x)*(6*gsl_pow_2(TAU) + 5*x)*gsl_pow_2(ZMAX)) +
        3*sqrt(2)*(2*gsl_pow_2(TAU) + x)*(4*gsl_pow_2(R_TOT)*x
		+ (2*gsl_pow_2(TAU) + x)*gsl_pow_2(ZMAX))*
         Arg((sqrt(2)*TAU),-sqrt(x)) ))/(24.*pow(x,1.5)) - I/K;
	} else {
		ZMAX = sqrt(x/C);
		retval = (sqrt(x)*ZMAX*(32*R_TOT*ZMAX + 3*M_PI*(4*gsl_pow_2(R_TOT)
			+ gsl_pow_2(ZMAX))))/(24.*sqrt(2))-I/K;
	}
	return retval;
	if (fabs(retval) < 1e-15) {
		return 0.0;
	} else {
		return retval;
	}
}

void myhandler (const char * reason,
              const char * file,
              int line,
              int gsl_errno)
{
	sasfit_out("reason: %s\nfile:%s(line:%d)\ngsl_errno:%d\n",reason,file,line,gsl_errno);
}

scalar find_LAMBDA(sasfit_param * param)
{
	int status;
	int iter, max_iter;
	const gsl_root_fsolver_type *T;
	gsl_root_fsolver *s;
	gsl_function F;
	double x_lo, x_hi, sx_lo, sx_hi;
	double root;
	double eps=1e-6;

	if (TAU < 0) {
		sx_lo = 1e-6 - 0.0*3*gsl_pow_2(TAU);
	} else {
		sx_lo = 1e-6;
	}
	sx_hi = 1e3;

	x_lo = sx_lo;
	x_hi = sx_hi;

	sasfit_out("f_low(%lg):%lg f_high(%lg):%lg R:%lg\n",x_lo,IntPhi(x_lo,param),x_hi, IntPhi(x_hi,param),R_TOT);

	F.function = (double (*) (double, void*))&IntPhi;
	F.params = param;

	T = gsl_root_fsolver_brent;
	T = gsl_root_fsolver_bisection;
	T = gsl_root_fsolver_falsepos;
	s = gsl_root_fsolver_alloc (T);
	iter = 0;
	max_iter = 1000;

	gsl_set_error_handler_off();
	gsl_set_error_handler (&myhandler);
	status = gsl_root_fsolver_set (s, &F, x_lo, x_hi);
	if (status != GSL_SUCCESS) {
		sasfit_err("could not initialize root finding algorithm gsl_root_fsolver_set");
		return 0.0;
	    gsl_root_fsolver_free (s);
	}

	do {
		iter++;
		status = gsl_root_fsolver_iterate (s);
		root = gsl_root_fsolver_root (s);
		x_lo = gsl_root_fsolver_x_lower (s);
		x_hi = gsl_root_fsolver_x_upper (s);

		status = gsl_root_test_interval (x_lo, x_hi, eps, eps);

	} while (status == GSL_CONTINUE && iter < max_iter);

	SASFIT_CHECK_COND2((iter >=max_iter), param, "exceeded maximum iteration for getting Langrange parameter: iter(%d) >= max_iter(%d)",iter,max_iter);
	SASFIT_CHECK_COND2(fabs(root -sx_lo)<=eps, param, "root(%f) at lower limit(%f)",root,sx_lo);
	SASFIT_CHECK_COND2(fabs(root -sx_hi)<=eps, param, "root(%f) at upper limit(%f)",root,sx_hi);
    gsl_root_fsolver_free (s);


	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	return root;
}

scalar phi(scalar r, sasfit_param * param)
{
	scalar z, zmax;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	z=r-R_TOT;
	if (z < 0) return 0.0;
	if (TAU<0) {
		zmax = sqrt((2*LAMBDA+3*gsl_pow_2(TAU))/(2*C));
	} else {
		zmax = sqrt(LAMBDA/C);
	}
	if (z>zmax) return 0.0;
	return -2*TAU+sqrt((2*LAMBDA)*(1.+2.*gsl_pow_2(TAU)/LAMBDA-gsl_pow_2(z/zmax)));
}

scalar phi_kernel(scalar x, sasfit_param *param)
{
	if (fabs(Q*x)<=1.e-5) {
		return 4*M_PI*gsl_pow_2(x) * phi(x,param) * (1.-gsl_pow_2(Q*x)/6.+gsl_pow_4(Q*x)/120.);
	} else {
		return 4*M_PI*gsl_pow_2(x) * sinc(Q*x) * phi(x,param);
	}
}
scalar FFDebye(scalar x)
{
	if (x<=1.0e-5) {
		return 1.0-x/3.+gsl_pow_2(x)/12.-gsl_pow_3(x)/60.+gsl_pow_4(x)/360.-gsl_pow_5(x)/2520.;
	} else {
		return 2.0*(x-1.0+exp(-x))/(x*x);
	}
}

scalar S1a(sasfit_param *param)
{
	scalar xi;
	if (ALPHA == 0.0 || R_AV == 0) {
		xi=0;
	} else {
		xi = ALPHA*V(R_CORE,param)/V(R_AV,param);
	}

	if ( fabs(xi) < 1.0e-5 ) return 1./3.*xi-1./45.*gsl_pow_3(xi)+2./945.*gsl_pow_5(xi);
	return 1.0/tanh(xi) - 1./xi;

}

scalar S2a(sasfit_param *param)
{
	scalar xi;
	if (ALPHA == 0.0 || R_AV == 0) {
		xi=0;
	} else {
		xi = ALPHA*V(R_CORE,param)/V(R_AV,param);
	}
	if ( fabs(xi) < 1.0e-5 ) return gsl_pow_2(xi)/15.-2./315.*gsl_pow_4(xi); // 1.0-3*(1./3.-1./45.*gsl_pow_2(xi));
	return 1.-3.*S1a(param)/xi;

}


scalar FFDaniels(scalar x, scalar n)
{
	if (x == 0.0) {
		return 1.0;
	} else {
		return FFDebye(x)+1.0/(15.0*n)*(4.+7./x-(11.+7./x)*exp(-x));
	}
}

scalar ecorr(scalar n)
{
	return 1.-1.5/n+1.5/(n*n)-0.75/(n*n*n)*(1.-exp(-2.*n));
}

scalar nu(scalar X)
{
	if (fabs(X)<1e-3) return 1.4*pow(fabs(X),1.04);
//	return 1.4*pow(X,1.04);
	return 1./8.*(9.*X-2.+2*log(1.+X)/X)
		  *exp(1./4.*(1./X+(1.-1./(X*X))*log(1.+X)));
}
scalar sigma(scalar Rg, scalar Rc, scalar Nagg)
{
	return Nagg*Rg*Rg/gsl_pow_2(2.0*(Rc+Rg));
}
scalar FFRPA(scalar q, scalar Rg, scalar Rc, scalar n, scalar Nagg)
{
	return FFDaniels(q*q*Rg*Rg/ecorr(n),n)/(1.+nu(sigma(Rg,Rc,Nagg))*FFDebye(q*q*Rg*Rg));
}

scalar V(scalar R, sasfit_param *param)
{
    if (lround(PEP) == 0) {
        return 4./3.*M_PI*gsl_pow_3(R);
    } else {
        return gsl_pow_3(2*R);
    }
}

scalar Vg(scalar R, scalar s)
{
	return 2.*M_PI*s*(4.*R*s+sqrt(2.*M_PI)*(R*R+s*s));
}

scalar Sg(scalar q, scalar r, scalar s)
{
	if ((q*r*(4.*r*s+sqrt(2.*M_PI)*(r*r+s*s)))  == 0.0) return 1;
	return (  2.*r*s*sin(q*r)
			+ sqrt(2.*M_PI)*exp(-0.5*gsl_pow_2(q*s))*(q*r*s*s*cos(q*r)+r*r*sin(q*r))
			+ 2.*sqrt(2.0)*gsl_sf_dawson(q*s/sqrt(2.0))*(r*r*cos(q*r)-q*r*s*s*sin(q*r))
			) / (q*r*(4.*r*s+sqrt(2.*M_PI)*(r*r+s*s)));
}

scalar FFphi(scalar x, sasfit_param *param)
{
    int parallelepiped,i;
    scalar da,gam,dpsi,sum,siga;

    parallelepiped = lround(PEP);
    switch (parallelepiped) {
	case 0: if (x == 0.0) {
                return 1.0;
            } else {
                return 3.0*(sin(x)-x*cos(x))/gsl_pow_3(x);
            }
            break;
    case 1: sum=0;
            da = YAWPEP-PSI;
            return   sinc(Q*R_CORE*sin(da))
                    *sinc(Q*R_CORE*cos(da));
            break;
    case 2: sum=0;
            da = YAWPEP-PSI;
            gam = acos(cos(PSI)*cos(da)+sin(PSI)*sin(da)*cos(DTHETA));
            return sinc(Q*R_CORE*sin(da)*sin(gam))
                        *sinc(Q*R_CORE*cos(da)*sin(gam))
                        *sinc(Q*R_CORE*sin(da)*cos(gam))
                        * exp(-0.5*gsl_pow_2(dpsi/siga))/(siga*sqrt(2.0*M_PI))
                        *siga/3.0;
            return sum;
            break;
    }
}

scalar FFAcor(scalar q, scalar s, scalar Rco, scalar Rch,sasfit_param *param)
{
	scalar w,sinc_x,d,Fnorm;
	int rw_saw;

	rw_saw = lround(RW_SAW);
	Q=q;

	switch (rw_saw) {
	case 0: {
				if (((Vg(Rch,s)+V(Rch,param)-V(Rco,param)) == 0) || (q==0)) {
					return 1.0;
				} else {
					return (Vg(Rch,s)*Sg(q,Rch,s)+V(Rch,param)*FFphi(q*Rch,param)-V(Rco,param)*FFphi(q*Rco,param))
						  /(Vg(Rch,s)+V(Rch,param)-V(Rco,param));
				}
				break;
			}
	case 1: {
				w = sasfit_rwbrush_w(q, RG);
				d=1;
				return w*sinc(q * (R_CORE + d * RG)) ;
				break ;
			}
	case 2: {
				if (TAU == 0.0) {
					Fnorm = (sqrt(LAMBDA)*M_PI*ZMAX*(32*R_TOT*ZMAX +
								3*M_PI*(4*gsl_pow_2(R_TOT) + gsl_pow_2(ZMAX))))/(6.*sqrt(2));
				} else {
					Fnorm = (M_PI*ZMAX*(-16*(-(sqrt(2)*R_TOT*
							(2*gsl_pow_2(TAU)*sqrt(LAMBDA*(LAMBDA + 2*gsl_pow_2(TAU))) +
							sqrt(gsl_pow_3(LAMBDA)*(LAMBDA + 2*gsl_pow_2(TAU))))*ZMAX) +
							pow(LAMBDA,1.5)*TAU*
							(3*gsl_pow_2(R_TOT) + 3*R_TOT*ZMAX + gsl_pow_2(ZMAX))) +
							2*sqrt(LAMBDA)*(-2*gsl_pow_2(TAU)*ZMAX*(16*R_TOT + 3*ZMAX) +
							3*LAMBDA*(4*gsl_pow_2(R_TOT) + gsl_pow_2(ZMAX)))*fabs(TAU) +
							3*sqrt(2)*(LAMBDA + 2*gsl_pow_2(TAU))*
							(4*LAMBDA*gsl_pow_2(R_TOT) +
							(LAMBDA + 2*gsl_pow_2(TAU))*gsl_pow_2(ZMAX))*
							asin(sqrt(LAMBDA/(LAMBDA + 2*gsl_pow_2(TAU))))))/
							(6.*pow(LAMBDA,1.5));
				}
				if (Q==0) {
					return 1.0;
				} else {
					return sasfit_integrate(R_TOT,R_TOT+ZMAX,&phi_kernel,param)/Fnorm;
				}
				break;
			}
	default: break;
	}
	return 0.0;
}

scalar FFmicelle(scalar q, scalar bs1, scalar bs2, scalar bc, sasfit_param * param)
{
	scalar ffacor;
	ffacor = FFAcor(q,SIGMA_BRUSH_GAUSSIAN,R_CORE+T_SH,R_CORE+T_SH+T_BRUSH_CONST,param);
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	if (RW_SAW == 0.0) {
		return	  gsl_pow_2((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+ (NAGG < 1.0?0:(NAGG-1.0))
				* NAGG*gsl_pow_2(bc*ffacor)
				+ 2.0*NAGG*bc*ffacor
				      *((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+ NAGG*bc*bc*FFRPA(q,RG,R_CORE+T_SH,L_B,NAGG);
	} else {
		return	  gsl_pow_2((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+ (NAGG < 1.0?0:(NAGG-1.0))
				* NAGG*gsl_pow_2(bc*ffacor)
				+ 2.0*NAGG*bc*ffacor
				      *((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+ NAGG*bc*bc*FFDebye(q*q*RG*RG);
	}
}

scalar FFmicelle_mm(scalar q, sasfit_param * param)
{
	scalar bc, bs1,bs2, bm1, bm2, S1, S2,sin2,sin4, FM,FN;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
	}
	S1=S1a(param);
	S2=S2a(param);
	bs1 = (ETA_CORE-ETA_SOLV);
	bs2 = (ETA_SHELL-ETA_SOLV);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	bc = (ETA_BRUSH-ETA_SOLV)*VBRUSH;
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	FN = ((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
			           +bc*NAGG*FFAcor(q,SIGMA_BRUSH_GAUSSIAN,R_CORE+T_SH,R_CORE+T_SH+T_BRUSH_CONST,param);

	return	  FFmicelle(q,bs1,bs2,bc,param)
			+ gsl_pow_2(FM)
                    *(S2*sin4+(1.0-S2)/3.0*sin2)
			+ 2.0*FM*FN*S1*sin2;
}

scalar FFmicelle_pp(scalar q, sasfit_param * param)
{
	scalar bc, bs1,bs2, bm1, bm2, S1, S2,sin2,sin4, FM,FN;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
	}
	S1=S1a(param);
	S2=S2a(param);
	bs1 = (ETA_CORE-ETA_SOLV);
	bs2 = (ETA_SHELL-ETA_SOLV);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	bc = (ETA_BRUSH-ETA_SOLV)*VBRUSH;
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	FN = ((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
			           +bc*NAGG*FFAcor(q,SIGMA_BRUSH_GAUSSIAN,R_CORE+T_SH,R_CORE+T_SH+T_BRUSH_CONST,param);

	return	  FFmicelle(q,bs1,bs2,bc,param)
			+ gsl_pow_2(FM)
			        *(S2*sin4+(1.0-S2)/3.0*sin2)
			- 2.0*FM*FN*S1*sin2;
}

scalar FFmicelle_pm(scalar q, sasfit_param * param)
{
	scalar bm1, bm2, S1, S2,sin2,sin4, FM;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
	}
	S1=S1a(param);
	S2=S2a(param);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	return gsl_pow_2(FM)
				*	(  2.0/3.0*(1.0-S2)
                      - S2*sin4
					  +(4.0*S2-1.0)/3.0*sin2
		             );
}

scalar FFmicelle_mp(scalar q, sasfit_param * param)
{
	return FFmicelle_pm(q,param);
}


scalar Amicelle(scalar q, scalar bs1, scalar bs2, scalar bc, sasfit_param * param)
{
	scalar ffacor;
	ffacor = FFAcor(q,SIGMA_BRUSH_GAUSSIAN,R_CORE+T_SH,R_CORE+T_SH+T_BRUSH_CONST,param);
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	if (RW_SAW == 0.0) {
		return	  ((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+  NAGG*(bc*ffacor);
	} else {
		return	  ((bs1-bs2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bs2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param))
				+ NAGG*(bc*ffacor);
	}
}

scalar Amicelle_mm(scalar q, sasfit_param * param)
{
	scalar bc, bs1,bs2, bm1, bm2, S1, S2,sin2,sin4,FM;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
	}
	S1=S1a(param);
	S2=S2a(param);
	bs1 = (ETA_CORE-ETA_SOLV);
	bs2 = (ETA_SHELL-ETA_SOLV);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	bc = (ETA_BRUSH-ETA_SOLV)*VBRUSH;
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	return	  Amicelle(q,bs1,bs2,bc,param)
			+ FM*(S1*sin2);
}

scalar Amicelle_pp(scalar q, sasfit_param * param)
{
	scalar bc, bs1,bs2, bm1, bm2, S1, S2,sin2,sin4,FM;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
	}
	S1=S1a(param);
	S2=S2a(param);
	bs1 = (ETA_CORE-ETA_SOLV);
	bs2 = (ETA_SHELL-ETA_SOLV);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	bc = (ETA_BRUSH-ETA_SOLV)*VBRUSH;
	NAGG = SNAGG*4.*M_PI*gsl_pow_2(R_CORE+T_SH);
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	return	  Amicelle(q,bs1,bs2,bc,param)
			- FM*(S1*sin2);
}

scalar Amicelle_pm(scalar q, sasfit_param * param)
{
	scalar bm1, bm2, S1, S2,sin2,sin4,sincos,FM;
	if (RADAVG == 1.0) {
		sin2 = 1./2.;
		sin4 = 3./8.;
		sincos=0;
	} else {
		sin2 = gsl_pow_2(sin(PSI));
		sin4 = gsl_pow_2(sin2);
		sincos = sin(PSI)*cos(PSI);
	}
	S1=S1a(param);
	S2=S2a(param);
	bm1 = ETA_MAG_CORE;
	bm2 = ETA_MAG_SHELL;
	FM = (bm1-bm2)*V(R_CORE,param)*FFphi(q*R_CORE,param)+bm2*V(R_CORE+T_SH,param)*FFphi(q*(R_CORE+T_SH),param);
	return FM*S1*sincos;
}

scalar Amicelle_mp(scalar q, sasfit_param * param)
{
	return Amicelle_pm(q,param);
}
