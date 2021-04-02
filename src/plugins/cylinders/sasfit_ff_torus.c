/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define A	param->p[1]
#define DA	param->p[2]
#define NU	param->p[3]
#define ETAC	param->p[4]
#define ETASH	param->p[5]
#define ETASOL	param->p[6]
#define Q		param->p[MAXPAR-1]
#define THETA	param->p[MAXPAR-2]
#define	ETA		param->p[MAXPAR-3]
#define ATOT	param->p[MAXPAR-4]
#define P       param->p[MAXPAR-5]

scalar sasfit_Torus_core_int_r(scalar r, void *pam)
{
    sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar gamma, KTcore, s,Q_2;

	SASFIT_ASSERT_PTR(param);

	s = Q/(2.0*M_PI);
	Q_2=Q/2.0;
	gamma = NU*sqrt(ATOT*ATOT - gsl_pow_2(r-R));

	if (THETA == 0.0)
	{
		return ETA*4.*r*sin(Q_2*gamma)*cos(Q_2*gamma)/s;
	}
	if (THETA == M_PI_2)
	{
		return ETA*4.*gsl_sf_bessel_J0(Q*r)*M_PI*r*gamma;
	}
	KTcore = ETA*4.*M_PI*r
		* gsl_sf_bessel_J0(Q*r*sin(THETA))
		* sin(Q*gamma*cos(THETA))
		/ (Q*cos(THETA));

	return KTcore;
}
scalar Torus_kernel_int_r(scalar x, sasfit_param * param) {
	return sasfit_Torus_core_int_r(x,param);
}

int Torus_kernel_cub_r(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=sasfit_Torus_core_int_r(x[0],pam);
	return 0;
}


scalar sasfit_Torus_core_int_theta(scalar theta,  void *pam)
{
    sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar *aw, res,err,sum;
	scalar TorusShell, int_r, int_r_delta;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param);

	THETA = theta;
	ETA = 1.0; // eta

	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
    int_r_delta	= 0;
    int_r=0;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            ATOT = A;
            sasfit_intde(&sasfit_Torus_core_int_r,R-ATOT,R+ATOT, aw, &res, &err, param);
            int_r = res;
            if (!(DA == 0.0 || ETASH == ETASOL)){
                ATOT = A+DA;
                sasfit_intde(&sasfit_Torus_core_int_r,R-ATOT, R+ATOT,aw, &res, &err, param);
                int_r_delta	= res;
            }
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            ATOT = A;
            sasfit_intcc(&sasfit_Torus_core_int_r,R-ATOT,R+ATOT, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
            int_r = res;
            if (!(DA == 0.0 || ETASH == ETASOL)){
                ATOT = A+DA;
                sasfit_intcc(&sasfit_Torus_core_int_r,R-ATOT, R+ATOT,sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
                int_r_delta	= res;
            }
            free(aw);
            break;
            }
    case H_CUBATURE: {
            ATOT = A;
            cubxmin[0]=R-ATOT;
            cubxmax[0]=R+ATOT;
            hcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            int_r	= fval[0];
            if (!(DA == 0.0 || ETASH == ETASOL)){
                ATOT = A+DA;
                cubxmin[0]=R-ATOT;
                cubxmax[0]=R+ATOT;
                hcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
                    100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
                    fval, ferr);
                int_r_delta	= fval[0];
            }
            break;
            }
    case P_CUBATURE: {
            ATOT = A;
            cubxmin[0]=R-ATOT;
            cubxmax[0]=R+ATOT;
            pcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            int_r	= fval[0];
            if (!(DA == 0.0 || ETASH == ETASOL)){
                ATOT = A+DA;
                cubxmin[0]=R-ATOT;
                cubxmax[0]=R+ATOT;
                pcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
                    100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
                    fval, ferr);
                int_r_delta	= fval[0];
            }
            break;
            }
    default: {
            ATOT = A;
            int_r	= sasfit_integrate(R-ATOT, R+ATOT, Torus_kernel_int_r, param);
            if (!(DA == 0.0 || ETASH == ETASOL)) {
                ATOT = A+DA;
                int_r_delta	= sasfit_integrate(R-ATOT, R+ATOT, Torus_kernel_int_r, param);
            }
            break;
            }
    }

	if (DA == 0.0 || ETASH == ETASOL)	{
		return gsl_pow_int((ETAC-ETASOL)*int_r,lround(P)) * sin(theta);
	}
	if (ETASH == ETAC)
	{
		return gsl_pow_int((ETASH-ETASOL)*int_r_delta,lround(P)) * sin(theta);
	}

	TorusShell = (ETASH-ETASOL)*int_r_delta + (ETAC-ETASH)*int_r;

	return gsl_pow_int(TorusShell,lround(P)) * sin(theta);
}

scalar Torus_kernel_int_theta(scalar x, sasfit_param * param) {
	return sasfit_Torus_core_int_theta(x,param);
}

int Torus_kernel_cub_theta(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=sasfit_Torus_core_int_theta(x[0],pam);
	return 0;
}


scalar sasfit_ff_torus(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((DA < 0.0), param, "Delta_a(%lg) < 0",DA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
	Q=q;
	P=2;
    intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&sasfit_Torus_core_int_theta,0.0, M_PI_2, aw, &res, &err, param);
            sum = res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&sasfit_Torus_core_int_theta,0.0, M_PI_2, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
            sum = res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            cubxmin[0]=0;
            cubxmax[0]=M_PI_2;
            hcubature(1, &Torus_kernel_cub_theta,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    case P_CUBATURE: {
            cubxmin[0]=0;
            cubxmax[0]=M_PI_2;
            pcubature(1, &Torus_kernel_cub_theta,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    default: {
            sum=sasfit_integrate(0.0, M_PI_2, Torus_kernel_int_theta, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_torus_f(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	Q=q;
	P=1;
    intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_DOUBLE_EXP_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&sasfit_Torus_core_int_theta,0.0, M_PI_2, aw, &res, &err, param);
            sum = res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&sasfit_Torus_core_int_theta,0.0, M_PI_2, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
            sum = res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
            cubxmin[0]=0;
            cubxmax[0]=M_PI_2;
            hcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    case P_CUBATURE: {
            cubxmin[0]=0;
            cubxmax[0]=M_PI_2;
            pcubature(1, &Torus_kernel_cub_r,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
            sum	= fval[0];
            break;
            }
    default: {
            sum=sasfit_integrate(0.0, M_PI_2, Torus_kernel_int_theta, param);
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_torus_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

