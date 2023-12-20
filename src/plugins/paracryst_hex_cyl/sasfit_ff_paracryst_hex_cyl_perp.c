/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RADIUS      param->p[0]
#define DR_RATIO	param->p[1]

#define A	        param->p[4]
#define DA_RATIO	param->p[5]
#define VERSION_SQ	param->p[6]
#define Q       param->p[MAXPAR-1]
#define N       param->p[MAXPAR-2]
#define FMOD    param->p[MAXPAR-3]
#define F2MOD   param->p[MAXPAR-4]
#define SQ      param->p[MAXPAR-5]
#define DRADIUS (RADIUS*DR_RATIO)

// [1] Penttilä, P. A., Rautkari, L., Österberg, M., & Schweins, R.
//     J. Appl. Cryst. (2019). 52, 369–377 https://doi.org/10.1107/S1600576719002012
// [2] Hashimoto, T., Kawamura, T., Harada, M., & Tanaka, H. .
//     Macromolecules (1994), 27, 3063-3072. https://doi.org/10.1021/ma00089a025

scalar GaussPDF(scalar x, sasfit_param * param)
{
	scalar NN, s, R0;
	scalar c;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &NN, &s, &R0);

    if (s == 0 ) return 1;

	SASFIT_CHECK_COND1((s == 0.0), param, "s(%lg) == 0", s);

	c = 0.5*sqrt(2.0*M_PI)*fabs(s) + 0.5*sasfit_erf_pure(0.5/fabs(s)*R0*sqrt(2.0))*sqrt(2.0*M_PI)*fabs(s);

	return NN/c*exp(-gsl_pow_2(x-R0)/(2.0*gsl_pow_2(s)));
}

scalar find_Gauss_int_range(scalar dim, scalar x0, scalar sigma, scalar *Xstart, scalar *Xend, sasfit_param *param)
{
	scalar R_n, R_50,R_0,R_max, tmp;
	scalar R2_n, R2_50,R2_0, R2start, R2end;
	int   n;
	scalar a1,a2,a3,a4,a5,a6;
	int funcid;
	sasfit_param subParam;
	scalar X_0, X_50, X_n, nperc;
	nperc = 0.1;

	n=dim;
	a1=1;
	a2=sigma;
	a3=x0;

	sasfit_init_param( &subParam );
	subParam.p[0] = a1;
	subParam.p[1] = a2;
	subParam.p[2] = a3;

    R_max = 0.5*a3+0.5*sqrt(a3*a3+4*n*a2*a2);
    R_n = R_max;
    tmp = 1.0;
    if (a3 > 0.0)  tmp=R_max;
    while ( (pow(n*R_n+tmp,dim) * GaussPDF(n*R_n+tmp, &subParam) ) >
        (pow(R_max,dim)     * nperc / 100.0
                    * GaussPDF(R_max, &subParam)) ) {
        n++;
    }
    R_n = n*R_n;
    *Xstart = 0.0;
    *Xend   = R_n+tmp;
    if ((R_n+tmp) < R_max)  *Xstart=(R_max-(R_n+tmp));
    return 0;
}

scalar calc_Zq(scalar q, scalar psi, sasfit_param * param){
    scalar qa1, qa2, F_abs, Z1, Z2;
    // Eqs. 11, 12 in Penttila et al. (2019):
    qa1 = -A*q*cos(psi-M_PI/6.);
    qa2 =  A*q*sin( psi );
    // Eq. 10 in Penttila et al. (2019):
    F_abs = exp( -0.5 * gsl_pow_2(DA_RATIO) * ( gsl_pow_2( qa1 ) + gsl_pow_2( qa2 ) ));
    // Eq. 9 in Penttila et al. (2019):
    Z1 = ( 1 - gsl_pow_2(F_abs) )/( 1 - 2*F_abs*cos(qa1) + gsl_pow_2(F_abs) );
    Z2 = ( 1 - gsl_pow_2(F_abs) )/( 1 - 2*F_abs*cos(qa2) + gsl_pow_2(F_abs) );
    return (Z1*Z2);
}
scalar ZZ(scalar psi, sasfit_param * param){
    scalar q0;
    q0 = 7.061e-5*A*A - 0.007413*A + 0.2465;
    if (Q <= q0 && VERSION_SQ <= 0) {
        return calc_Zq(q0, psi, param);
    }
    return calc_Zq(Q, psi, param);
}

scalar calc_f( scalar R, sasfit_param * param ){
    scalar qR;
	sasfit_param subParam;
    qR = Q*R;
	subParam.p[0] = 1;
	subParam.p[1] = DRADIUS;
	subParam.p[2] = RADIUS;
    return GaussPDF(R,&subParam) * gsl_pow_int(M_PI*gsl_pow_2(R) * sasfit_jinc(qR),lround(N)); // divide( j1(qR), qR )
}

scalar Iperp(sasfit_param * param) {
    return F2MOD + gsl_pow_2(FMOD)*(SQ-1);
}

scalar sasfit_ff_paracryst_hex_cyl_perp(scalar q, sasfit_param * param)
{
    scalar Fmod, F2mod, Xstart, Xend;
    sasfit_param subParam;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RADIUS < 0.0), param, "R(%lg) < 0",RADIUS); // modify condition to your needs
	SASFIT_CHECK_COND1((DR_RATIO < 0.0), param, "dR_ratio(%lg) < 0",DR_RATIO); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((DA_RATIO < 0.0), param, "da_ratio(%lg) < 0",DA_RATIO); // modify condition to your needs

	// insert your code here

	Q = q;
	if (DRADIUS == 0) {
        N=1;
        FMOD  = calc_f(RADIUS,param);
        N=2;
        F2MOD = FMOD*FMOD;
	} else {
	    if (RADIUS == 0) return 0;
        N=1;
        find_Gauss_int_range(2*N,RADIUS,DRADIUS,&Xstart,&Xend,param);
        FMOD = sasfit_integrate(Xstart,Xend,&calc_f,param);
        N=2;
        find_Gauss_int_range(2*N,RADIUS,DRADIUS,&Xstart,&Xend,param);
        F2MOD = sasfit_integrate(Xstart,Xend,&calc_f,param);
	}

	SQ = sasfit_integrate(0,M_PI,&ZZ,param)/(M_PI);
    return   Iperp(param);
}

scalar sasfit_ff_paracryst_hex_cyl_perp_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_paracryst_hex_cyl_perp_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

