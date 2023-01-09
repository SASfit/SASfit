/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <float.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

typedef struct
{
	sasfit_param *param;
	scalar *cubxmin;
	scalar *cubxmax;
	scalar s;
	scalar rn2;
	scalar rn4;
	scalar K;
	scalar X;
	scalar max_negargExp;
} koyama_param;

// define shortcuts for local parameters/variables


scalar xg(scalar x, void *pam) {
	scalar rn2,rn4,K;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return sqrt((2.*rn2/gsl_pow_2(LB))*sqrt(10.*fabs(1.0-3./5.*K)));
}

scalar xf(scalar x, void *pam) {
	scalar rn2,rn4,K;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return (2*rn2/gsl_pow_2(LB)) - gsl_pow_2(xg(x,pam))/2.0;
}

scalar phi(scalar x, void *pam) {
	scalar s,t,res;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	s=cparam->s;

	t=x*LB/2.;

    if (fabs(x)< 1e-4) {
        cparam->rn2=gsl_pow_2(LB)/2.*(0.5*gsl_pow_2(x)-gsl_pow_3(x)/6.0+gsl_pow_4(x)/24.-gsl_pow_5(x)/120.+gsl_pow_6(x)/720.);
        cparam->rn4=gsl_pow_4(LB)/4.*(1./4.0*gsl_pow_4(x)-1./6.0*gsl_pow_5(x)+29./360.0*gsl_pow_6(x)-1./30.0*gsl_pow_7(x)+247./20160.0*gsl_pow_8(x));
    } else {
        cparam->rn2=gsl_pow_2(LB)/2.*(x-(1-exp(-x)));
        cparam->rn4=gsl_pow_4(LB)/4.*(5/3.*x*x - 52.0/9.*x-2./27.*(1.0-exp(-3.0*x))
	           +8*(1.0-exp(-x))-2*x*exp(-x));
    }
    if (fabs(cparam->rn2/gsl_pow_2(LB))<sqrt(GSL_DBL_MIN*100)) {
        cparam->K=1;
    } else {
        cparam->K=cparam->rn4/gsl_pow_2(cparam->rn2);
    }
	res = exp(-s*s/3.0*xf(x,pam)) * gsl_sf_bessel_j0(fabs(s*xg(x,pam)));
	if (!gsl_finite(res))
            sasfit_out("::phi: x: %lg, rn2=%lg, rn4=%lg, K=%lg phi:%lg xf:%lg, xg:%lg\n",x, cparam->rn2,cparam->rn4,cparam->K,res,xf(x,pam),xg(x,pam));
	return res;
}

scalar koyama_worm_kernel(scalar x, void *pam) {
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	return 2*phi(x,pam)*(cparam->X - x)/gsl_pow_2(cparam->X);
}

scalar koyama_worm_kernel_cub(const double *x, size_t ndim, void *pam) {
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	if (ndim < 1) {
		sasfit_err("false dimensions ndim:%d\n",ndim);
		return 0;
	}
	return koyama_worm_kernel(x[0],pam);
}

scalar x_func(scalar x) {
    if (fabs(x)<0.1) {
        return gsl_pow_int(x,4)/60.
                - gsl_pow_int(x,5)/90.
                + (89*gsl_pow_int(x,6))/18900.
                - (37*gsl_pow_int(x,7))/22680.
                + (6961*gsl_pow_int(x,8))/1.3608e7
                - (4327*gsl_pow_int(x,9))/2.80665e7
                + (438839*gsl_pow_int(x,10))/9.430344e9
                - (216599*gsl_pow_int(x,11))/1.5011568e10
                + (1024331149*gsl_pow_int(x,12))/2.206700496e14;
    } else {

        return (gsl_expm1(-x) + x - sqrt(2.5)*(gsl_expm1(-x) + x)*
                    sqrt(1 - (3*((-2*(-gsl_expm1(-3*x)))/27. + 8*(-gsl_expm1(-x)) - (52*x)/9. - (2*x)*exp(-x) + (5*gsl_pow_int(x,2))/3.))/
                    (5.*gsl_pow_int(gsl_expm1(-x) + x,2))));
        return (-1 + exp(-x) + x - sqrt(2.5)*(-1 + exp(-x) + x)*
                    sqrt(1 - (3*((-2*(1 - exp(-3*x)))/27. + 8*(1 - exp(-x)) - (52*x)/9. - (2*x)*exp(-x) + (5*gsl_pow_int(x,2))/3.))/
                    (5.*gsl_pow_int(-1 + exp(-x) + x,2))));
    }
}

double root_x (double x, void *params) {
    koyama_param *cparam;
    cparam = (koyama_param *) params;
//    sasfit_out("root_x:%lg max:%lg maxexp:%lg scaling:%lg\n",x_func(x)-cparam->max_negargExp,cparam->max_negargExp,-log(GSL_DBL_MIN*1.2),cparam->s*cparam->s/3.0);
    return x_func(x)-cparam->max_negargExp;
}

scalar which_x(koyama_param *cparam) {
    int status;
    sasfit_param *param;
    int iter = 0, max_iter = 100;
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;
    double r = 0;
    double x_lo, x_hi;
    gsl_function F;
    scalar MAXEXP, scaling;
    param = cparam->param;
    MAXEXP = -log(GSL_DBL_MIN*1e10);
    scaling = cparam->s*cparam->s/3.0;
    if (!gsl_finite(MAXEXP/scaling)) return cparam->X;
    if (x_func(cparam->X) <= MAXEXP/scaling) return cparam->X;
    cparam->max_negargExp=MAXEXP/scaling;
    x_lo=0;
    x_hi=cparam->X;
//    x_hi=MAXEXP/scaling;
    F.function = &root_x;
    F.params = cparam;

//    sasfit_out("max:%lg maxexp:%lg scaling:%lg\n",cparam->max_negargExp,-log(GSL_DBL_MIN*1e10),cparam->s*cparam->s/3.0);

    T = gsl_root_fsolver_brent;
    s = gsl_root_fsolver_alloc (T);
    gsl_root_fsolver_set (s, &F, x_lo, x_hi);

/*
    sasfit_out ("using %s method\n",
          gsl_root_fsolver_name (s));

    sasfit_out ("%5s [ %9s, %9s ] %9s %9s \n",
          "iter", "lower", "upper", "root", "err(est)");
*/
  do
    {
      iter++;
      status = gsl_root_fsolver_iterate (s);
      r = gsl_root_fsolver_root (s);
      x_lo = gsl_root_fsolver_x_lower (s);
      x_hi = gsl_root_fsolver_x_upper (s);
      status = gsl_root_test_interval (x_lo, x_hi,
                                       0, 0.001);
/*
      if (status == GSL_SUCCESS)
        sasfit_out ("Converged after %d steps at %lf:\n", iter,r);

      sasfit_out ("%5d [ %.7lf, %.7lf ] %.7lf %.7lf \n",
              iter, x_lo, x_hi,
              r,
              x_hi - x_lo);
*/
    }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_root_fsolver_free (s);

  return r;
}
scalar sasfit_sq_koyama_worms(scalar q, sasfit_param * param)
{
    int ierr;
    scalar res,err, X;
    scalar cubxmin[1], cubxmax[1];
	koyama_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs

	// insert your code here

	Q=q;
	cparam.param = param;
	cparam.s=(q*LB/2.);
	cparam.X = 2*L/LB;
	cubxmin[0]=0;
	cubxmax[0]=which_x(&cparam);
//	if (cubxmax[0]!=cparam.X) sasfit_out("upper limit:%lg (%lg)\n",cubxmax[0],cparam.X);

	ierr = sasfit_cubature(1,cubxmin, cubxmax,&koyama_worm_kernel_cub,&cparam , sasfit_eps_get_nriq(),&res,&err);

	return res*L*L;
}

scalar sasfit_sq_koyama_worms_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_koyama_worms_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

