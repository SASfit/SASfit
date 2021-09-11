/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA_R	fabs(param->p[1])
#define T	param->p[2]
#define SIGMA_T	fabs(param->p[3])
#define RG	param->p[4]
#define N_PEG	param->p[5]
#define V_PEG	param->p[6]
#define ETA_PEG	param->p[7]
#define V_HEAD	param->p[8]
#define ETA_HEAD	param->p[9]
#define V_TAIL	param->p[10]
#define ETA_TAIL	param->p[11]
#define ETA_SOL	param->p[12]

#define Q   param->p[MAXPAR-1]
#define HO   param->p[MAXPAR-2]
#define HI   param->p[MAXPAR-3]
#define RINT param->p[MAXPAR-4]
#define TINT param->p[MAXPAR-5]
#define RI   (RINT-TINT/2.)
#define RO   (RINT+TINT/2.)

scalar vesicle_pegulated(const double* v, size_t n, void* p)
{
    scalar Phi, ni, no, N, Vho,Vhi,tmp, Ichain,Iii,Ioo,Iio,Ivi,Ivo;
    sasfit_param * param;
    param = (sasfit_param *) p;
    SASFIT_CHECK_COND1((n != 2), param, "n(%d) !=2 0",n);

    RINT = v[0];
    TINT = v[1];
	no = 4./3.*M_PI/V_TAIL*(gsl_pow_3(RO)   -gsl_pow_3(RINT) );
	ni = 4./3.*M_PI/V_TAIL*(gsl_pow_3(RINT) -gsl_pow_3(RI));
	Vho = no*V_HEAD;
	Vhi = ni*V_HEAD;
	no=no*N_PEG;
	ni=ni*N_PEG;
	HO = -RINT - TINT/2. + 0.5*pow(gsl_pow_3(2*RINT+TINT) + (12*RINT*RINT*TINT+6*RINT*TINT*TINT+TINT*TINT*TINT)*V_HEAD/V_TAIL,1/3.);
	tmp = gsl_pow_3(2*RINT-TINT) - (12*RINT*RINT*TINT-6*RINT*TINT*TINT+TINT*TINT*TINT)*V_HEAD/V_TAIL;
	HI = GSL_SIGN(tmp) *(RINT - TINT/2. - 0.5*pow(fabs(tmp),1./3.));
	if (HI>=0 && RINT-TINT/2.-RG-HI>=0) {
       Phi =(ETA_HEAD-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(RO+HO)*sasfit_sphere_fc(Q,RO+HO)
                                -4./3.*M_PI*gsl_pow_3(RO)   *sasfit_sphere_fc(Q,RO) )
           +(ETA_TAIL-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(RO)   *sasfit_sphere_fc(Q,RO)
                                -4./3.*M_PI*gsl_pow_3(RI)   *sasfit_sphere_fc(Q,RI) )
           +(ETA_HEAD-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(RI)   *sasfit_sphere_fc(Q,RI)
                                -4./3.*M_PI*gsl_pow_3(RI-HI)*sasfit_sphere_fc(Q,RI-HI) );
	} else {
	    return 0;
    }
	N = (ni+no);
	Ichain = N      *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG)*sasfit_gauss_fc(Q,RG);
	Ioo =  no*(no-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(RO+HO+RG)/M_PI));
	Iii =  ni*(ni-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(RI-HI-RG)/M_PI));
	Iio = 2*ni*no   *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG))*gsl_sf_sinc(Q*(RO+HO+RG)/M_PI)
	                                                                          *gsl_sf_sinc(Q*(RI-HI-RG)/M_PI);
	Ivi = 2*ni*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(RI-HI-RG)/M_PI);
	Ivo = 2*no*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(RO+HO+RG)/M_PI);
	return Phi*Phi+Ichain+Iii+Ioo+Iio+Ivi+Ivo;
}

scalar sasfit_ff_vesicle__pegulated_(scalar q, sasfit_param * param)
{
    double v[2],intstart[2],intend[2], res2D, err2D;
    scalar RStart,REnd, SigmaStart,SigmaEnd;
    int nintervals;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((N_PEG < 0.0), param, "n_PEG(%lg) < 0",N_PEG); // modify condition to your needs
	SASFIT_CHECK_COND1((N_PEG > 1.0), param, "n_PEG(%lg) > 1",N_PEG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_PEG < 0.0), param, "V_PEG(%lg) < 0",V_PEG); // modify condition to your needs
	SASFIT_CHECK_COND1((V_HEAD < 0.0), param, "V_head(%lg) < 0",V_HEAD); // modify condition to your needs
	SASFIT_CHECK_COND1((V_TAIL < 0.0), param, "V_tail(%lg) < 0",V_TAIL); // modify condition to your needs

	// insert your code here
	Q=q;
	v[0]=R;
	v[1]=T;
	if (SIGMA_R==0) {
        RStart = SIGMA_R;
        REnd=RStart;
	} else {
        find_LogNorm_int_range(4, R, SIGMA_R, &RStart, &REnd, param);
	}
	if (SIGMA_T==0) {
        SigmaStart = SIGMA_T;
        SigmaEnd=SigmaStart;
	} else {
        find_LogNorm_int_range(2, T, SIGMA_T, &SigmaStart, &SigmaEnd, param);
	}
	return vesicle_pegulated(v,2,param);
/*
    intstart[0]=RStart;
    intend[0]  =REnd;
    intstart[1]=SigmaStart;
    intend[1]=SigmaEnd;
	sasfit_cubature(2,
			intstart,
			intend,
			&vesicle_pegulated,
			param,
			sasfit_eps_get_nr(),
			&res2D,
			&err2D)
    return res2D;
*/
}

scalar sasfit_ff_vesicle__pegulated__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_vesicle__pegulated__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

