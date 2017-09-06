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
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define ETA	param->p[3]
#define Q	param->p[MAXPAR-1]
#define ALPHA	param->p[MAXPAR-2]
#define BETA	param->p[MAXPAR-3]

scalar sinc(scalar x)
{
	if (fabs(x) <= 1e-4) {
		return 1.0-gsl_pow_2(x)/6.0
		          +gsl_pow_4(x)/120.0
		          -gsl_pow_6(x)/5040.0
		          +gsl_pow_8(x)/362880;
	} else {
		return sin(x)/x;
	}
}

scalar Psi_P_kernel(scalar y, sasfit_param * param)
{
	param->p[MAXPAR-2] = y;
	return 2./M_PI * gsl_pow_2(sinc(Q*A/2.0*sin(param->p[MAXPAR-2])*cos(param->p[MAXPAR-3])) *
				               sinc(Q*B/2.0*sin(param->p[MAXPAR-2])*sin(param->p[MAXPAR-3])) *
							   sinc(Q*C/2.0*cos(param->p[MAXPAR-2]))) * sin(param->p[MAXPAR-2]);
}

scalar Psi_kernel(scalar x, sasfit_param * param)
{
	param->p[MAXPAR-3] = x;
	return sasfit_integrate(0.0,M_PI/2.0,&Psi_P_kernel,param);
}

scalar sasfit_ff_parallelepiped_abc(scalar q, sasfit_param * param)
{
	scalar *aw, res,err;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    gsl_integration_workspace * w;
    gsl_integration_cquad_workspace * wcquad;
    gsl_integration_glfixed_table * wglfixed;
    gsl_function F;
    size_t neval;
    int lenaw=4000;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	Q=q;
	// insert your code here
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
 //           sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
 //           sasfit_intde(&IQ_IntdLen, Len_start,Len_end, aw, &res, &err,&param4int);
            free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
 //           sasfit_intccini(lenaw, aw);
//            sasfit_intcc(&IQ_IntdLen, Len_start,Len_end, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&param4int);
            free(aw);
            break;
            }
    case GSL_CQUAD: {
//            wcquad = gsl_integration_cquad_workspace_alloc(lenaw);
//            F.function=&IQ_IntdLen;
//            F.params = &param4int;
//            gsl_integration_cquad (&F, Len_start, Len_end, 0, sasfit_eps_get_nriq(), wcquad, &res, &err,&neval);
//            gsl_integration_cquad_workspace_free(wcquad);
            break;
            }
    case GSL_QAG: {
//            w = gsl_integration_workspace_alloc(lenaw);
//            F.function=&IQ_IntdLen;
//            F.params = &param4int;
//            gsl_integration_qag (&F, Len_start, Len_end, 0, sasfit_eps_get_nriq(), lenaw, 3,
//                        w, &res, &err);
//            gsl_integration_workspace_free (w);
            break;
            }
    case H_CUBATURE: {
//            param4int.function=&IQ_IntdLen;
//            cubxmin[0]=Len_start;
//            cubxmax[0]=Len_end;
//            hcubature(1, &f1D_cubature,&param4int,1, cubxmin, cubxmax, 
//              10000, 0.0, sasfit_eps_get_nriq(), 
//              ERROR_INDIVIDUAL, fval, ferr);
//            res = fval[0];
            break;
            }
    case P_CUBATURE: {
//            param4int.function=&IQ_IntdLen;
//            cubxmin[0]=Len_start;
//            cubxmax[0]=Len_end;
//            pcubature(1, &f1D_cubature, &param4int,1, cubxmin, cubxmax, 
//              10000,0.0, sasfit_eps_get_nriq(), 
//              ERROR_INDIVIDUAL, fval, ferr);
//            res = fval[0];
            break;
            }
    case NR_QROMB: {
//            res = SASFITqrombIQdR_old(interp,dF_dpar,l,s,Q,a,
//		                    SD,FF,SQ,
//							distr,Len_start, Len_end,error);
            break;
            }
    default: {
            sasfit_err("Unknown integration strategy\n");
            break;
            }
    }
	return gsl_pow_2(ETA*sasfit_ff_parallelepiped_abc_v(q,param,1))
			*sasfit_integrate(0.0,M_PI/2.0,&Psi_kernel,param);
}

scalar sasfit_ff_parallelepiped_abc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_parallelepiped_abc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A*B*C;
}

