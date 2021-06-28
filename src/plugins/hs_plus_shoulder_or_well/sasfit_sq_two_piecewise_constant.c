/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

#include <math.h>
#include <complex.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>

// define shortcuts for local parameters/variables
#define RHS	param->p[0]
#define DUMMY param->p[1]
#define ETA	param->p[2]
#define LAMBDA1	param->p[3]
#define E1_KBT	param->p[4]
#define LAMBDA2	param->p[5]
#define E2_KBT	param->p[6]

#define A0 exp(-E1_KBT)
#define A1 (exp(-E2_KBT)-exp(-E1_KBT))
#define A2 (1- exp(-E2_KBT))

double complex C1(double complex B1, double complex B2, sasfit_param * param) {
    int k=1;
    return A1*gsl_pow_int(LAMBDA1-1,k)-k*B1*1+
           A2*gsl_pow_int(LAMBDA2-1,k)-k*B2*1;
}
double complex C2(double complex B1, double complex B2, sasfit_param * param) {
    int k=2;
    return A1*gsl_pow_int(LAMBDA1-1,k)-k*B1*gsl_pow_int(LAMBDA1-1,k-1)+
           A2*gsl_pow_int(LAMBDA2-1,k)-k*B2*gsl_pow_int(LAMBDA2-1,k-1);
}
double complex C3(double complex B1, double complex B2, sasfit_param * param) {
    int k=3;
    return A1*gsl_pow_int(LAMBDA1-1,k)-k*B1*gsl_pow_int(LAMBDA1-1,k-1)+
           A2*gsl_pow_int(LAMBDA2-1,k)-k*B2*gsl_pow_int(LAMBDA2-1,k-1);
}
double complex C4(double complex B1, double complex B2, sasfit_param * param) {
    int k=4;
    return A1*gsl_pow_int(LAMBDA1-1,k)-k*B1*gsl_pow_int(LAMBDA1-1,k-1) +
           A2*gsl_pow_int(LAMBDA2-1,k)-k*B2*gsl_pow_int(LAMBDA2-1,k-1);
}

double complex B0(double complex B1, double complex B2, sasfit_param * param) {
    return C1(B1,B2,param)+0.5*ETA/(1+2*ETA)*(6*C2(B1,B2,param)+4*C3(B1,B2,param)+C4(B1,B2,param))+(1+0.5*ETA)/(1+2*ETA);
}

double complex S1(double complex B1, double complex B2, sasfit_param * param) {
    return -1+B0(B1,B2,param)-C1(B1,B2,param);
}
double complex S2(double complex B1, double complex B2, sasfit_param *param) {
    return 0.5-B0(B1,B2,param)+C1(B1,B2,param)+0.5*C2(B1,B2,param);
}
double complex S3(double complex B1, double complex B2, sasfit_param *param) {
    return -(1+2*ETA)/(12*ETA)+0.5*B0(B1,B2,param)-0.5*C1(B1,B2,param)-0.5*C2(B1,B2,param)-C3(B1,B2,param)/6.;
}

double complex rs1(double complex B1, double complex B2, sasfit_param *param) {

    return -S2(B1,B2,param)/(3.*S3(B1,B2,param)) - (pow(2,0.3333333333333333)*(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param)))/
    (3.*S3(B1,B2,param)*cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
        csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
          cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333)) +
   cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
      csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
        cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333)/
    (3.*pow(2,0.3333333333333333)*S3(B1,B2,param));
}

double complex rs2(double complex B1, double complex B2,sasfit_param *param) {
    return -S2(B1,B2,param)/(3.*S3(B1,B2,param)) + ((1 + I*sqrt(3))*(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param)))/
    (3.*pow(2,0.6666666666666666)*S3(B1,B2,param)*
      cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
        csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
          cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333)) -
   ((1 - I*sqrt(3))*cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
        csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
          cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333))/
    (6.*pow(2,0.3333333333333333)*S3(B1,B2,param));
}

double complex rs3(double complex B1, double complex B2, sasfit_param *param) {
    return -S2(B1,B2,param)/(3.*S3(B1,B2,param)) + ((1 - I*sqrt(3))*(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param)))/
    (3.*pow(2,0.6666666666666666)*S3(B1,B2,param)*
      cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
        csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
          cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333)) -
   ((1 + I*sqrt(3))*cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2) +
        csqrt(4*cpow(-cpow(S2(B1,B2,param),2) + 3*S1(B1,B2,param)*S3(B1,B2,param),3) +
          cpow(-2*cpow(S2(B1,B2,param),3) + 9*S1(B1,B2,param)*S2(B1,B2,param)*S3(B1,B2,param) - 27*cpow(S3(B1,B2,param),2),2)),0.3333333333333333))/
    (6.*pow(2,0.3333333333333333)*S3(B1,B2,param));
}

int
B12_f (const gsl_vector * x, void *pam,
              gsl_vector * f)
{
    sasfit_param * param;
    param = (sasfit_param *) pam;
    double complex B1, B2, yB1, yB2;

    B1 = gsl_vector_get (x, 0) + I*gsl_vector_get (x, 1);
    B2 = gsl_vector_get (x, 2) + I*gsl_vector_get (x, 3);
    yB1 = 0 + I*0;
    yB1 = yB1 + rs1(B1,B2,param)*cexp((LAMBDA1-1)*rs1(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs1(B1,B2,param)+3*S3(B1,B2,param)*rs1(B1,B2,param)*rs1(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs1(B1,B2,param));
    yB1 = yB1 + rs1(B1,B2,param)*cexp((LAMBDA1-1)*rs2(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs2(B1,B2,param)+3*S3(B1,B2,param)*rs2(B1,B2,param)*rs2(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs2(B1,B2,param));
    yB1 = yB1 + rs1(B1,B2,param)*cexp((LAMBDA1-1)*rs3(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs3(B1,B2,param)+3*S3(B1,B2,param)*rs3(B1,B2,param)*rs3(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs3(B1,B2,param));
    yB1 = yB1 * (exp(E1_KBT-E2_KBT)-1)*S3(B1,B2,param) - B1;

    yB2 = 0 + I*0;
    yB2 = yB2 + rs1(B1,B2,param)*cexp((LAMBDA2-1)*rs1(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs1(B1,B2,param)+3*S3(B1,B2,param)*rs1(B1,B2,param)*rs1(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs1(B1,B2,param)+(A1+B1*rs1(B1,B2,param))*cexp(-(LAMBDA1-1)*rs1(B1,B2,param)));
    yB2 = yB2 + rs1(B1,B2,param)*cexp((LAMBDA2-1)*rs2(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs2(B1,B2,param)+3*S3(B1,B2,param)*rs2(B1,B2,param)*rs2(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs2(B1,B2,param)+(A1+B1*rs2(B1,B2,param))*cexp(-(LAMBDA1-1)*rs2(B1,B2,param)));
    yB2 = yB2 + rs1(B1,B2,param)*cexp((LAMBDA2-1)*rs3(B1,B2,param))/(S1(B1,B2,param)+2*S2(B1,B2,param)*rs3(B1,B2,param)+3*S3(B1,B2,param)*rs3(B1,B2,param)*rs3(B1,B2,param))
              * (A0+B0(B1,B2,param)*rs3(B1,B2,param)+(A1+B1*rs3(B1,B2,param))*cexp(-(LAMBDA1-1)*rs3(B1,B2,param)));
    yB2 = yB2 * (exp(E2_KBT)-1)*S3(B1,B2,param) - B2;

    gsl_vector_set (f, 0, creal(yB1));
    gsl_vector_set (f, 1, cimag(yB1));
    gsl_vector_set (f, 2, creal(yB2));
    gsl_vector_set (f, 3, cimag(yB2));

  return GSL_SUCCESS;
}

int findB1B2(double complex *B1, double complex *B2,sasfit_param *param) {
  const gsl_multiroot_fsolver_type *T;
  gsl_multiroot_fsolver *s;

  int status;
  size_t i, iter = 0;

  const size_t n = 4;

  gsl_multiroot_function f = {&B12_f, n, param};

  static double x_init[4] = {100.0, 1, 200.0, 2};
  gsl_vector *x = gsl_vector_alloc (n);

  gsl_vector_set (x, 0, x_init[0]);
  gsl_vector_set (x, 1, x_init[1]);
  gsl_vector_set (x, 2, x_init[2]);
  gsl_vector_set (x, 3, x_init[3]);

  T = gsl_multiroot_fsolver_hybrids;
//  T = gsl_multiroot_fsolver_dnewton;
  s = gsl_multiroot_fsolver_alloc (T, 4);
  gsl_multiroot_fsolver_set (s, &f, x);

//  print_state (iter, s);

  do
    {
      iter++;
      status = gsl_multiroot_fsolver_iterate (s);

//      print_state (iter, s);

      if (status)   /* check if solver is stuck */
        break;

      status =
        gsl_multiroot_test_residual (s->f, 1e-8);
    }
  while (status == GSL_CONTINUE && iter < 2000);
//  sasfit_out("iter=%d\n",iter);
//  printf ("status = %s\n", gsl_strerror (status));
  x_init[0]=gsl_vector_get(s->x,0);
  x_init[1]=gsl_vector_get(s->x,1);
  x_init[2]=gsl_vector_get(s->x,2);
  x_init[3]=gsl_vector_get(s->x,3);
  *B1 = gsl_vector_get (s->x, 0) + I*gsl_vector_get (s->x, 1);
  *B2 = gsl_vector_get (s->x, 2) + I*gsl_vector_get (s->x, 3);
//  sasfit_out("rB1=%lg, iB1=%lg, rB2=%lg, iB2=%lg\n",gsl_vector_get (x, 0),gsl_vector_get (x, 1),gsl_vector_get (x, 2),gsl_vector_get (x, 3));

  gsl_multiroot_fsolver_free (s);
  gsl_vector_free (x);
  return status;
}

scalar sasfit_sq_square_shoulder(scalar q, sasfit_param * param)
{
    scalar rho;
    double complex cS,Gp, Gm, F, R0, R1, R2, s, B1, B2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA1 <= 1.0), param, "lambda1(%lg) <= 1",LAMBDA1); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA2 <= LAMBDA1), param, "lambda2(%lg) <= lambda1",LAMBDA2); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA > 1.0), param, "eta(%lg) > 1",ETA); // modify condition to your needs

	// insert your code here
	rho = ETA*6./M_PI / gsl_pow_3(2*RHS);
	findB1B2(&B1,&B2,param);

	s = I*q*2*RHS;
	R0 = -1/(12*ETA) *(A0+B0(B1,B2,param)*s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	R1 = -1/(12*ETA) *(A1+B1             *s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	R2 = -1/(12*ETA) *(A2+B2             *s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	F  = R0+R1*cexp(-(LAMBDA1-1)*s)+R2*cexp(-(LAMBDA2-1)*s);
	Gp = s*F*cexp(-s)/(1+12*ETA*F*cexp(-s));

	s = -I*q*2*RHS;
	R0 = -1/(12*ETA) *(A0+B0(B1,B2,param)*s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	R1 = -1/(12*ETA) *(A1+B1             *s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	R2 = -1/(12*ETA) *(A2+B2             *s)/(1+S1(B1,B2,param)*s+S2(B1,B2,param)*s*s+S3(B1,B2,param)*s*s*s);
	F  = R0+R1*cexp(-(LAMBDA1-1)*s)+R2*cexp(-(LAMBDA2-1)*s);
	Gm = s*F*cexp(-s)/(1+12*ETA*F*cexp(-s));

	cS = 1 - 2*M_PI*rho*(Gp-Gm)/(I*q*2*RHS);
	cS = 1 - 12*ETA*(Gp-Gm)/(I*q*2*RHS);
//	sasfit_out("rB1=%lg, iB1=%lg, rB2=%lg, iB2=%lg\n",creal(B1),cimag(B1),creal(B2),cimag(B2));
	return creal(cS);
}

scalar sasfit_sq_square_shoulder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_square_shoulder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

