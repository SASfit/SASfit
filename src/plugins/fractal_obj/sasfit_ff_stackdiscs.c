/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define H	param->p[2]
#define N	param->p[3]
#define ETA_C	param->p[4]
#define ETA_L	param->p[5]
#define SIGMA	fabs(param->p[6])
#define D	param->p[7]
#define Q   param->p[MAXPAR-1]
#define NN  param->p[MAXPAR-2]


scalar StackDiscs_core(scalar theta, sasfit_param * param)
{
	scalar SQ, tn, V_t, V_c, f_t, f_c, A, A2, A4, An, AA, BB, AA2, AA3, AA4,
	       tn2,tn3, tn4,tn5,tn6,tn7,tn8, BB2, BB3, BB4, BB5, BB6, BB7, BB8;
	scalar bessarg, sinarg1, sinarg2, bess, tmp;
	int k;

    SASFIT_ASSERT_PTR(param);

	tn = NN;
	V_t = M_PI*R*R*(T+2*H);
	V_c = M_PI*R*R*T;

	bessarg = Q*R *sin(theta);
	sinarg1 = Q*(T/2.0+H)*cos(theta);
	sinarg2 = Q*T/2.0*cos(theta);

/*
	    BB = Q*D;
	    AA = gsl_pow_2(Q*SIGMA)/2.;
	    A = exp(AA*gsl_pow_2(cos(theta)));
	    A2 = A*A;
	    A4 = A2*A2;
	    An = gsl_pow_int(A,lround(tn));
	    sasfit_out("Q:%lg, theta:%lg, AA:%lg, BB:%lg, Z:%lg, N:%lg\n",Q,theta,AA,BB,(An*(4*A2 + (-1 + A4)*tn -
            2*A*(1 - tn + A2*(1 + tn))*cos(BB*cos(theta))) +
            2*A*(cos(BB*(-1 + tn)*cos(theta)) +
            A*(-2*cos(BB*tn*cos(theta)) + A*cos(BB*(1 + tn)*cos(theta))))),(An*tn*gsl_pow_2(1 + A2 - 2*A*cos(BB*cos(theta)))));
*/
    if (sasfit_eps_get_robertus_p()<0){
	if (fabs(theta-M_PI_2)<sasfit_eps_get_nriq()) {
        BB = Q*D;
        AA = gsl_pow_2(Q*SIGMA)/2.;
        AA2 = AA*AA;
        AA3 = AA2*AA;
        AA4 = AA2*AA2;
        BB2 = BB*BB;
        BB3 = BB2*BB;
        BB4 = BB3*BB;
        BB5 = BB4*BB;
        BB6 = BB5*BB;
        BB7 = BB6*BB;
        BB8 = BB6*BB;
        tn2 = tn*tn;
        tn3 = tn2*tn;
        tn4 = tn2*tn2;
        tn5 = tn4*tn;
        tn6 = tn5*tn;
        tn7 = tn6*tn;
        tn8 = tn7*tn;
       SQ =  (tn + (gsl_pow_2(-M_PI_2 + theta)*(80*BB2 + 80*BB*tn +
          16*AA2*BB*tn + 20*AA2*tn2 +
          2*AA4*tn2 - 20*BB2*tn2 -
          4*AA2*BB*tn3 - 3*AA4*tn4))/
      (60.*(4*BB + AA2*tn)))/
   (1 + (gsl_pow_2(-M_PI_2 + theta)*
        (80*BB - 24*AA2*BB + 20*AA2*tn - 3*AA4*tn +
          60*BB2*tn + 36*AA2*BB*tn2 +
          2*AA4*tn3))/(60.*(4*BB + AA2*tn)));
        SQ = (tn + (gsl_pow_2(-M_PI_2 + theta)*(2240*AA2 -
          672*AA2*BB2 + 896*AA*tn + 1008*AA3*tn -
          224*AA*BB2*tn - 132*AA*BB4*tn +
          1120*AA2*tn2 + 224*BB2*tn2 +
          588*AA2*BB2*tn2 -
          28*BB4*tn2 - 11*BB6*tn2 -
          672*AA3*tn3 + 896*AA*BB2*tn3 +
          260*AA*BB4*tn3 -
          420*AA2*BB2*tn4 -
          28*BB4*tn4 + 24*BB6*tn4 -
          188*AA*BB4*tn5 - 11*BB6*tn6))/
      (84.*(80*AA - 24*AA*BB2 + 60*AA2*tn +
          20*BB2*tn - 3*BB4*tn +
          36*AA*BB2*tn2 + 2*BB4*tn3)) +
     (gsl_pow_4(-M_PI_2 + theta)*(-26880*AA2 - 13440*AA4 -
          1632*AA2*BB4 - 13440*AA*BB2*tn -
          13440*AA3*BB2*tn - 208*AA*BB6*tn +
          26880*AA2*tn2 + 8400*AA4*tn2 -
          1680*BB4*tn2 +
          2688*AA2*BB4*tn2 -
          13*BB8*tn2 + 13440*AA*BB2*tn3 +
          8400*AA3*BB2*tn3 +
          672*AA*BB6*tn3 + 5040*AA4*tn4 +
          1680*BB4*tn4 -
          4368*AA2*BB4*tn4 +
          42*BB8*tn4 +
          5040*AA3*BB2*tn5 -
          812*AA*BB6*tn5 +
          3312*AA2*BB4*tn6 -
          42*BB8*tn6 + 348*AA*BB6*tn7 +
          13*BB8*tn8))/
      (5040.*(80*AA - 24*AA*BB2 + 60*AA2*tn +
          20*BB2*tn - 3*BB4*tn +
          36*AA*BB2*tn2 + 2*BB4*tn3)))/
   (1 + (gsl_pow_2(-M_PI_2 + theta)*
        (896*AA - 672*AA3 - 1344*AA*BB2 + 120*AA*BB4 +
          3360*AA2*tn + 224*BB2*tn -
          1512*AA2*BB2*tn - 168*BB4*tn +
          10*BB6*tn + 1008*AA3*tn2 +
          2016*AA*BB2*tn2 - 300*AA*BB4*tn2 +
          1008*AA2*BB2*tn3 +
          112*BB4*tn3 - 11*BB6*tn3 +
          120*AA*BB4*tn4 + 3*BB6*tn5))/
      (84.*(80*AA - 24*AA*BB2 + 60*AA2*tn +
          20*BB2*tn - 3*BB4*tn +
          36*AA*BB2*tn2 + 2*BB4*tn3)));
	} else {
	    BB = Q*D;
	    AA = gsl_pow_2(Q*SIGMA)/2.;
	    if (AA*tn > 100) {
            A = exp(-(1+tn)*AA*gsl_pow_2(cos(theta)));
            SQ = 1+2*A*cos(BB*(1+tn)*cos(theta))/tn;
        } else {
            A = exp(AA*gsl_pow_2(cos(theta)));
            A2 = A*A;
            A4 = A2*A2;
            An = gsl_pow_int(A,lround(tn));
            SQ = (An*(4*A2 + (-1 + A4)*tn -
                2*A*(1 - tn + A2*(1 + tn))*cos(BB*cos(theta))) +
                2*A*(cos(BB*(-1 + tn)*cos(theta)) +
                A*(-2*cos(BB*tn*cos(theta)) + A*cos(BB*(1 + tn)*cos(theta)))))/
                (An*tn*gsl_pow_2(1 + A2 - 2*A*cos(BB*cos(theta))));
        }
	}
    } else {
        SQ = 1.0;
        tmp = Q*cos(theta);
        for (k=1; k < tn ;k++)
        {
            SQ = SQ+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-k*gsl_pow_2(tmp*SIGMA)/2.0);
        }
    }
	if (bessarg != 0.0)
	{
		bess=2.0*sasfit_bessj1(bessarg)/bessarg;
	} else
	{
		bess =1.0;
	}

	if (sinarg1 != 0.0)
	{
		f_t = sin(sinarg1)/sinarg1*bess;
	} else
	{
		f_t = bess;
	}

	if (sinarg2 != 0.0)
	{
		f_c = sin(sinarg2)/sinarg2*bess;
	} else
	{
		f_c = bess;
	}

	return    (ETA_L * (V_t*f_t - V_c*f_c) + ETA_C * V_c*f_c)
		* (ETA_L * (V_t*f_t - V_c*f_c) + ETA_C * V_c*f_c)
		* SQ*sin(theta);
}

scalar StackDiscs_OOURA(scalar theta, void * pam)
{
    sasfit_param * param;
    param = (sasfit_param *) pam;
    return StackDiscs_core(theta,param);
}
int StackDiscs_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
    sasfit_param * param;
    param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0] = StackDiscs_core(x[0],param);
	return 0;
}
scalar sasfit_ff_stackdiscs(scalar q, sasfit_param * param)
{
    scalar w,ftmp1,ftmp2;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "h(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "n(%lg) < 1",N); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs

//	Q = 1;
//	NN = lround(floor(N));
//	return StackDiscs_core(q,param);
	// insert your code here
	Q = q;

    cubxmin[0]=0;
    cubxmax[0]=M_PI_2;
    switch(intstrategy) {
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
        aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
        sasfit_intccini(lenaw, aw);

        NN = lround(floor(N));
        w = N-floor(N);
        sasfit_intcc(&StackDiscs_OOURA,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &ftmp1, &err,param);
        NN=lround(floor(N))+1;
        if (w>0) {
            sasfit_intcc(&StackDiscs_OOURA,cubxmin[0], cubxmax[0], sasfit_eps_get_aniso(), lenaw, aw, &ftmp2, &err,param);
        } else {
            ftmp2=0;
        }
        free(aw);
        break;
        }
    case OOURA_DOUBLE_EXP_QUADRATURE: {
        aw = (scalar *)malloc((lenaw)*sizeof(scalar));
        sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_aniso(), aw);
        NN = lround(floor(N));
        w = N-floor(N);
        sasfit_intde(&StackDiscs_OOURA,cubxmin[0], cubxmax[0], aw, &ftmp1, &err, param);
        NN=lround(floor(N))+1;
        if (w>0) {
            sasfit_intde(&StackDiscs_OOURA,cubxmin[0], cubxmax[0], aw, &ftmp2, &err, param);
        } else {
            ftmp2=0;
        }
        free(aw);
        break;
        }
    case P_CUBATURE: {
        NN = lround(floor(N));
        w = N-floor(N);
        pcubature(1, &StackDiscs_cub,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
        ftmp1 = fval[0];
        NN=lround(floor(N))+1;
        if (w>0) {
            pcubature(1, &StackDiscs_cub,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
            ftmp2 = fval[0];
        } else {
            ftmp2=0;
        }
        break;
        }
    case H_CUBATURE: {
        NN = lround(floor(N));
        w = N-floor(N);
        pcubature(1, &StackDiscs_cub,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
        ftmp1 = fval[0];
        NN=lround(floor(N))+1;
        if (w>0) {
            pcubature(1, &StackDiscs_cub,param,1, cubxmin, cubxmax,
                        100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
                        fval, ferr);
            ftmp2 = fval[0];
        } else {
            ftmp2=0;
        }
        break;
        }
    default: {
        NN = lround(floor(N));
        w = N-floor(N);
        ftmp1=sasfit_integrate(0.0, M_PI/2.0, StackDiscs_core, param);
        NN=lround(floor(N))+1;
        if (w>0) {
            ftmp2=sasfit_integrate(0.0, M_PI/2.0, StackDiscs_core, param);
        } else {
            ftmp2=0;
        }
    }
    }
    return (1-w)*ftmp1+w*ftmp2;
}

scalar sasfit_ff_stackdiscs_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_stackdiscs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

