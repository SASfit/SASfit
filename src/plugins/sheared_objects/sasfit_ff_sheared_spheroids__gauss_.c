/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_sheared_spheroids__gauss_(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000;
	cubature_param cparam;
 // return pGauss(q,0,param);

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);


	Q = q;		// Q
	P = 2.0;	// pow
	if (SIGMA==0) {
		ndim=2;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim =3;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	cubxmin[0]=0;
	cubxmax[0]=M_PI;
	cubxmin[1]=0;
	cubxmax[1]=2*M_PI;
	cubxmin[2]=NUMIN;
	cubxmax[2]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &alignedEllSh;
	cparam.gam = &gamOthers;
	cparam.p1 = &pGauss;

	intstrategy = sasfit_get_int_strategy();
//	intstrategy=P_CUBATURE;
	switch(intstrategy) {
/*
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_P_OOURA1, cparam.cubxmin[0], cparam.cubxmax[0], sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&cparam);
			sum=res;
            free(aw);
            break;
            }
*/
    case H_CUBATURE: {
			hcubature(1, &partly_aligned_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_L2,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
            cubxmin[0]=0;
            cubxmax[0]=M_PI_2;
			pcubature(1, &partly_aligned_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_L2,
				fval, ferr);
			sum = 2*fval[0];
            break;
            }
    default: {
            cubxmin[0]=0;
            cubxmax[0]=1;
			pcubature(1, &partly_aligned_cubature_u_phi,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_L2,
				fval, ferr);
			sum = 2*fval[0];
            break;
            }
    }
	return sum;
}

scalar sasfit_ff_sheared_spheroids__gauss__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_sheared_spheroids__gauss__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

