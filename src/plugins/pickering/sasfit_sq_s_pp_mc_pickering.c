/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include <gsl/gsl_randist.h>
#include "include/private.h"
#include <sasfit_error_sq.h>
#include <time.h>

// define shortcuts for local parameters/variables
#define R_O	param->p[0]
#define R_P	param->p[1]
#define DELTA	param->p[2]
#define N	param->p[3]
#define EXVOL	param->p[4]

#define NRND 20
#define MAXN 1000
#define MAXTRY 1000

scalar sasfit_sq_s_pp_mc_pickering(scalar q, sasfit_param * param)
{
    scalar result,d;
    time_t current_time;
    longint ctime;
	static gsl_rng *r1, *r2;
	static const gsl_rng_type *T1, *T2;
	scalar x1,x2,xx;
	static int idum=-1;
	int i,j,m,k,overlap,try;
	static scalar phi,theta,r_i[MAXN][3][10*MAXN],sum[10*MAXN];
	static scalar oR_O=-1, oR_P=-1, oDELTA=-1,oEXVOL=-1;
	static int oNav=-1, Nav=NRND;

	if (idum < 0) {
		idum = -idum;
		gsl_rng_env_setup();
		T1 = gsl_rng_default;
		r1 = gsl_rng_alloc(T1);
		current_time = time(NULL);
		if (current_time == ((time_t)-1)) {
                ctime =1;
		} else {
		    ctime = (longint) current_time;
		}
		gsl_rng_set (r1, ctime);
		T2 = gsl_rng_default;
		r2 = gsl_rng_alloc(T1);
		gsl_rng_set (r1, ctime+3);
	}

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_O < 0.0), param, "R_o(%lg) < 0",R_O); // modify condition to your needs
	SASFIT_CHECK_COND1((R_P < 0.0), param, "R_p(%lg) < 0",R_P); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA < 0.0), param, "delta(%lg) < 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "n(%lg) < 1",N); // modify condition to your needs
	SASFIT_CHECK_COND1((N > MAXN), param, "n(%lg) > MAXN",N); // modify condition to your needs

//	sasfit_out("Nav:%d, iter_4_mc:%d\n",Nav,lround(sasfit_eps_get_iter_4_mc()));

    if (Nav < sasfit_eps_get_iter_4_mc()) {
            Nav = sasfit_eps_get_iter_4_mc();
    }
    if (Nav > 10*MAXN) {
            Nav = 10*MAXN;
    }
    if ( (oR_O != R_O) ||
         (oR_P != R_P) ||
         (oDELTA != DELTA) ||
         (oNav != Nav) ||
         (oEXVOL != EXVOL)) {
        R_O = R_O;
        oR_P = R_P;
        oDELTA = DELTA;
        oNav = Nav;
        oEXVOL = EXVOL;

        for (k=0;k<Nav;k++){
        for (m=0;m<lround(N);m++) {
            try = 0;
            do {
 /*
                phi = gsl_rng_uniform(r1)*2.*M_PI;
                theta = acos(2.*gsl_rng_uniform(r1)-1);
                r_i[m][0][l] = (R_O+DELTA*R_P)*cos(phi)*sin(theta);
                r_i[m][1][l] = (R_O+DELTA*R_P)*sin(phi)*sin(theta);
                r_i[m][2][l] = (R_O+DELTA*R_P)*         cos(theta);
 */
// /*
                do {
                    x1 = gsl_rng_uniform_pos(r1)*2.-1.;
                    x2 = gsl_rng_uniform_pos(r2)*2.-1.;
                    xx=x1*x1+x2*x2;
                } while (xx >= 1.0);
                r_i[m][0][k] = (R_O+DELTA*R_P)*2.*x1*sqrt(1.-xx);
                r_i[m][1][k] = (R_O+DELTA*R_P)*2.*x2*sqrt(1.-xx);
                r_i[m][2][k] = (R_O+DELTA*R_P)*(1.-2.*xx);
// */
                overlap = 0;
                if (EXVOL != 0) {
                    for (i=0;i<m;i++) {
                        d=gsl_hypot3(   r_i[i][0][k]-r_i[m][0][k],
                                        r_i[i][1][k]-r_i[m][1][k],
                                        r_i[i][2][k]-r_i[m][2][k]);
                        if (d < 2.*R_P) overlap=1;
                    }
                }
                try++;
            } while (overlap==1 && try < MAXTRY);
            if (try==MAXTRY) {
                sasfit_err("Reached maximum number of tries to place a new micelle on the surface of the particle\n");
                return 0;
            }
        }
        }
    }

    result=0;
    for (k=0;k<Nav;k++) {
        sum[k]=0;
        for (i=0;i<lround(N);i++) {
            for (j=0;j<i;j++) {
                sum[k] = sum[k]+ gsl_sf_bessel_j0(q*gsl_hypot3( r_i[i][0][k]-r_i[j][0][k],
                                                                r_i[i][1][k]-r_i[j][1][k],
                                                                r_i[i][2][k]-r_i[j][2][k]));
            }
        }
        result=result+2*sum[k];
    }
	// insert your code here
	return 1+result/(Nav*lround(N));
}

scalar sasfit_sq_s_pp_mc_pickering_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_s_pp_mc_pickering_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

