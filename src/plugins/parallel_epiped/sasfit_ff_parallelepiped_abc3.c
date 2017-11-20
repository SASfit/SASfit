/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */
 
#include "../../sasfit_common/cubature/cubature.h"
#include <sasfit.h>

#include <sasfit_common.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_parallelepiped_abc3(scalar q, sasfit_param * param)
{
	scalar sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    int intstrategy;
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	Q=q;
	NUDIM = 3;
	// insert your code here
    intstrategy = sasfit_get_int_strategy();

	switch(intstrategy) {
		// ERROR_INDIVIDUAL, ERROR_PAIRED, ERROR_L2
    case H_CUBATURE: {
			cubxmin[0]=0;
			cubxmax[0]=M_PI/2.0;
			cubxmin[1]=0;
			cubxmax[1]=M_PI/2.0;
			find_LogNorm_int_range(6,1,SIGMA,&cubxmin[2], &cubxmax[2], &subParam);
			hcubature(1, &K_cubature,param,3, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_L2, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			cubxmin[0]=0;
			cubxmax[0]=M_PI/2.0;
			cubxmin[1]=0;
			cubxmax[1]=M_PI/2.0;
			find_LogNorm_int_range(6,1,SIGMA,&cubxmin[2], &cubxmax[2], &subParam);
			pcubature(1, &K_cubature,param,3, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_L2, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
		    sasfit_err("no valid sasfit_integrate routine\n");
//            sum=sasfit_integrate(0.0,M_PI/2.0,&Psi_kernel,param);;
            break;
            }
    }
	return gsl_pow_2(ETA)	*sum;
}

scalar sasfit_ff_parallelepiped_abc3_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_parallelepiped_abc3_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A*B*C;
}

