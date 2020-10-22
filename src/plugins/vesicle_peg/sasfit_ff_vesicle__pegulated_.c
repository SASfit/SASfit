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
#define RI   (R-T/2.)
#define RO   (R+T/2.)

scalar sasfit_ff_vesicle__pegulated_(scalar q, sasfit_param * param)
{
    scalar Phi, ni, no, N, Vho,Vhi,tmp, Ichain,Iii,Ioo,Iio,Ivi,Ivo;
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

	no = 4./3.*M_PI/V_TAIL*(gsl_pow_3(RO)-gsl_pow_3(R) );
	ni = 4./3.*M_PI/V_TAIL*(gsl_pow_3(R) -gsl_pow_3(RI));
	Vho = no*V_HEAD;
	Vhi = ni*V_HEAD;
	no=no*N_PEG;
	ni=ni*N_PEG;
	HO = -R - T/2. + 0.5*pow(gsl_pow_3(2*R+T) + (12*R*R*T+6*R*T*T+T*T*T)*V_HEAD/V_TAIL,1/3.);
	tmp = gsl_pow_3(2*R-T) - (12*R*R*T-6*R*T*T+T*T*T)*V_HEAD/V_TAIL;
	HI = GSL_SIGN(tmp) *(R - T/2. - 0.5*pow(fabs(tmp),1./3.));
	if (HI>=0 && R-T/2.-RG-HI>=0) {
       Phi =(ETA_HEAD-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(R+T/2.+HO)*sasfit_sphere_fc(Q,R+T/2.+HO)
                                -4./3.*M_PI*gsl_pow_3(R+T/2.)   *sasfit_sphere_fc(Q,R+T/2.) )
           +(ETA_TAIL-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(R+T/2.)   *sasfit_sphere_fc(Q,R+T/2.)
                                -4./3.*M_PI*gsl_pow_3(R-T/2)    *sasfit_sphere_fc(Q,R-T/2.) )
           +(ETA_HEAD-ETA_SOL)*( 4./3.*M_PI*gsl_pow_3(R-T/2.   )*sasfit_sphere_fc(Q,R-T/2.)
                                -4./3.*M_PI*gsl_pow_3(R-T/2.-HI)*sasfit_sphere_fc(Q,R-T/2.-HI) );
	} else {
	    return 0;
    }
	N = (ni+no);
	Ichain = N      *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG)*sasfit_gauss_fc(Q,RG);
	Ioo =  no*(no-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(R+T/2+HO+RG)/M_PI));
	Iii =  ni*(ni-1)*gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG) *gsl_sf_sinc(Q*(R-T/2-HI-RG)/M_PI));
	Iio = 2*ni*no   *gsl_pow_2((ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG))*gsl_sf_sinc(Q*(R+T/2+HO+RG)/M_PI)
	                                                                          *gsl_sf_sinc(Q*(R-T/2-HI-RG)/M_PI);
	Ivi = 2*ni*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(R-T/2-HI-RG)/M_PI);
	Ivo = 2*no*(ETA_PEG-ETA_SOL)*V_PEG*sasfit_rwbrush_w(Q,RG)*Phi*gsl_sf_sinc(Q*(R+T/2+HO+RG)/M_PI);
	return Phi*Phi+Ichain+Iii+Ioo+Iio+Ivi+Ivo;
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

