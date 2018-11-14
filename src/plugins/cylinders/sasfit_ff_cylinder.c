/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define R	param->p[0]

#define L	param->p[2]
#define ETA	param->p[3]
#define Q   param->p[MAXPAR-1]

scalar cylinder_kernel(scalar x, void *pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
    return gsl_pow_2(M_PI*R*R*L*ETA
                *sasfit_jinc(Q*R*sqrt(1.0-x*x))
                *2*gsl_sf_bessel_j0(Q*L*x/2.0));
}
scalar cylinder_kernel_int(scalar x, sasfit_param * param) {
	return cylinder_kernel(x,param);
}

int cylinder_kernel_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=cylinder_kernel(x[0],pam);
//	sasfit_out("::koyama_worm_kernel_cub: x[0]:%lf fval[0]:%lf\n",x[0],fval[0]);
	return 0;
}

scalar sasfit_ff_cylinder(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
    Q=q;
	cubxmin[0]=0;
	cubxmax[0]=1;

	intstrategy = sasfit_get_int_strategy();
    intstrategy=OOURA_CLENSHAW_CURTIS_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&cylinder_kernel,0,1, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&cylinder_kernel, 0,1, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &cylinder_kernel_cub,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &cylinder_kernel_cub,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("This is default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0,1.0, &cylinder_kernel_int, param);
            break;
            }
    }
    return sum;
}

scalar sasfit_ff_cylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

