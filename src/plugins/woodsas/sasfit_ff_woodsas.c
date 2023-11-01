/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A_SCALE	param->p[0]
#define RADIUS	param->p[1]
#define DR_RATIO	param->p[2]
#define A	    param->p[3]
#define DA_RATIO	param->p[4]
#define B_SCALE	param->p[5]
#define SIGMA	param->p[6]
#define C_SCALE	param->p[7]
#define ALPHA	param->p[8]
#define BCKGR	param->p[9]
#define Q       param->p[MAXPAR-1]
#define N       param->p[MAXPAR-2]
#define FMOD    param->p[MAXPAR-3]
#define F2MOD   param->p[MAXPAR-4]
#define SQ      param->p[MAXPAR-5]
#define DRADIUS (RADIUS*DR_RATIO)

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

	a1=1;
	a2=sigma;
	a3=x0;

	sasfit_init_param( &subParam );
	subParam.p[0] = a1;
	subParam.p[1] = a2;
	subParam.p[2] = a3;

    R_max = 0.5*a3+0.5*sqrt(a3*a3+4*n*a2*a2);
    R_n = R_max;
    n = 1;
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
    if (Q > q0) {
        return calc_Zq(Q, psi, param);
    } else {
        return calc_Zq(q0, psi, param);
    }
}

scalar calc_f( scalar R, sasfit_param * param ){
    scalar qR;
    qR = Q*R;
    return GaussPDF(R,param) * gsl_pow_int(M_PI*gsl_pow_2(R) * sasfit_jinc(qR),lround(N)); // divide( j1(qR), qR )
}

scalar Iperp(sasfit_param * param) {
    return F2MOD + gsl_pow_2(FMOD)*(SQ-1);
}

scalar sasfit_ff_woodsas(scalar q, sasfit_param * param)
{
    scalar Fmod, F2mod, Xstart, Xend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A_SCALE < 0.0), param, "A_scale(%lg) < 0",A_SCALE); // modify condition to your needs
	SASFIT_CHECK_COND1((RADIUS < 0.0), param, "radius(%lg) < 0",RADIUS); // modify condition to your needs
	SASFIT_CHECK_COND1((DRADIUS < 0.0), param, "Dradius(%lg) < 0",DRADIUS); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((DA_RATIO < 0.0), param, "da_ratio(%lg) < 0",DA_RATIO); // modify condition to your needs
	SASFIT_CHECK_COND1((B_SCALE < 0.0), param, "B_scale(%lg) < 0",B_SCALE); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((C_SCALE < 0.0), param, "C_scale(%lg) < 0",C_SCALE); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BCKGR < 0.0), param, "bckgr(%lg) < 0",BCKGR); // modify condition to your needs

	// insert your code here
	Q = q;
	if (DRADIUS == 0) {
        FMOD  = calc_f(RADIUS,param);
        F2MOD = FMOD*FMOD;
	} else {
	    if (RADIUS == 0) return 0;
        find_Gauss_int_range(4,RADIUS,DR_RATIO,&Xstart,&Xend,param);
        N=1;
        FMOD = sasfit_integrate(Xstart,Xend,&calc_f,param);
        N=2;
        F2MOD = sasfit_integrate(Xstart,Xend,&calc_f,param);
	}
	SQ = sasfit_integrate(0,M_PI,&ZZ,param)/(M_PI);
	// return SQ;
	return   A_SCALE*Iperp(param)/(M_PI*gsl_pow_4(RADIUS))
            +B_SCALE*exp(-0.5*gsl_pow_2(q*SIGMA))
            +C_SCALE*pow(q,-ALPHA)
            +BCKGR;
}

scalar sasfit_ff_woodsas_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_woodsas_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

