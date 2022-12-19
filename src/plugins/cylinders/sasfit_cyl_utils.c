/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define EPSILON	param->p[2]
#define L	param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SHELL	param->p[5]
#define ETA_SOLV	param->p[6]

#define Q   param->p[MAXPAR-1]
#define TYPE_SHELL  param->p[MAXPAR-2]
#define THETA param->p[MAXPAR-3]
#define ALPHA param->p[MAXPAR-4]
#define P param->p[MAXPAR-5]

scalar ell_rad(scalar radius, scalar epsilo, scalar t, scalar theta)
{
	return gsl_hypot((radius+t)*sin(theta),(epsilo*radius+t)*cos(theta));
}

scalar AcylSHell(sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,v1,v2,b1,b2, J1;

	u1 = Q*ell_rad(R,EPSILON,0,THETA)*sin(ALPHA);
	v1 = Q*L*0.5*cos(ALPHA);
	u2 = Q*ell_rad(R,EPSILON,T,THETA)*sin(ALPHA);
	v2 = Q*(L*0.5+T*TYPE_SHELL)*cos(ALPHA);
	b1 = M_PI*R*R*EPSILON*L*(ETA_CORE-ETA_SHELL);
	b2 = M_PI*(R+T)*(EPSILON*R+T)*(L+2.0*T*TYPE_SHELL)*(ETA_SHELL-ETA_SOLV);
	Ain = gsl_sf_bessel_j0(v1);
	Ain = Ain*2*sasfit_jinc(u1);
	Aout = gsl_sf_bessel_j0(v2);
	Aout = Aout*2*sasfit_jinc(u2);


	A = b1*Ain+b2*Aout;
	return gsl_pow_int(A,lround(P));
}

scalar A_ellcyl(const double *x, size_t ndim, void *pam) {
	sasfit_param * param;
	scalar ftmp;
	param = (sasfit_param *) pam;
	if ((ndim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 0;
	}
	ALPHA=x[0];
    if (ndim == 1) {
        THETA=0;
        ftmp=1;
    } else {
        THETA=x[1];
        ftmp=1.0/(4*M_PI);
    }
	return AcylSHell(param)*sin(ALPHA)*ftmp;
}
