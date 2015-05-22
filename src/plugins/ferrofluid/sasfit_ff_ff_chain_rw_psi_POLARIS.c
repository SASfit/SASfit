/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_ff_chain_rw_psi_POLARIS(scalar x, sasfit_param * param)
{
    scalar avgT,origyaw,q;
    scalar siga, da;
    int i;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R_CORE < 0.0), param, "R_core(%lg) < 0",R_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((SNAGG < 0.0), param, "nagg(%lg) < 0",SNAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((VBRUSH < 0.0), param, "Vbrush(%lg) < 0",VBRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(POL) > 1.0), param, "|pol|(%lg) > 1",POL); // modify condition to your needs

	// insert your code here

	RW_SAW = 1.0;
	RADAVG = 0.0;
	R_CORE = 0.0;

	if ((R_TOT-T_SHELL) > 0) {
		R_CORE=R_TOT-T_SHELL;
		T_SH = T_SHELL;
	} else {
		R_CORE = 0.0;
		T_SH = R_TOT;
	}
	if (PSIDEG >= 0) {
        q = x;
        PSI = sasfit_param_override_get_psi(PSIDEG*M_PI/180.);
	} else {
	    q = -PSIDEG;
        PSI = sasfit_param_override_get_psi(x*M_PI/180.);
	}

	if  (lround(PEP) == 0) {
        return	(1.0+POL)/2.0*(TPLUS *FFmicelle_pp(q,param)+TMINUS*FFmicelle_pm(q,param))
            +	(1.0-POL)/2.0*(TMINUS*FFmicelle_mm(q,param)+TPLUS *FFmicelle_mp(q,param));
	} else {
        avgT = 0;
        siga = SIGYAW*M_PI/180.;
        origyaw = YAWPEP*M_PI/180.;
        for (i=-6;i<=6;i++) {
            da = i*siga/(3.0);
            YAWPEP = origyaw+da;
            avgT = avgT+exp(-0.5*gsl_pow_2(da/siga))/(siga*sqrt(2.0*M_PI))
                        *siga/3.0*
                   (    (1.0+POL)/2.0*( TPLUS *FFmicelle_pp(q,param)
                                       +TMINUS*FFmicelle_pm(q,param))
                    +	(1.0-POL)/2.0*( TMINUS*FFmicelle_mm(q,param)
                                       +TPLUS *FFmicelle_mp(q,param))
                   );
        }
        YAWPEP = origyaw*180/M_PI;
        return avgT;
	}
}

scalar sasfit_ff_ff_chain_rw_psi_POLARIS_f(scalar x, sasfit_param * param)
{
    scalar avgT,origyaw,q;
    scalar siga, da;
    int i;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    RW_SAW = 1.0;
	RADAVG = 0.0;

	R_CORE = 0.0;
	if ((R_TOT-T_SHELL) > 0) {
		R_CORE=R_TOT-T_SHELL;
		T_SH = T_SHELL;
	} else {
		R_CORE = 0.0;
		T_SH = R_TOT;
	}

	if (PSIDEG >= 0) {
        q = x;
        PSI = sasfit_param_override_get_psi(PSIDEG*M_PI/180.);
	} else {
	    q = -PSIDEG;
        PSI = sasfit_param_override_get_psi(x*M_PI/180.);
	}
//	return	(1.0+POL)/2.0*(TPLUS *Amicelle_pp(q,param)+TMINUS*Amicelle_pm(q,param))
//		+	(1.0-POL)/2.0*(TMINUS*Amicelle_mm(q,param)+TPLUS *Amicelle_mp(q,param));

    if  (lround(PEP) == 0) {
        return	(1.0+POL)/2.0*(TPLUS *Amicelle_pp(q,param)+TMINUS*Amicelle_pm(q,param))
            +	(1.0-POL)/2.0*(TMINUS*Amicelle_mm(q,param)+TPLUS *Amicelle_mp(q,param));
	} else {
        avgT = 0;
        siga = SIGYAW*M_PI/180.;
        origyaw = YAWPEP*M_PI/180.;
        for (i=-6;i<=6;i++) {
            da = i*siga/(3.0);
            YAWPEP = origyaw+da;
            avgT = avgT+exp(-0.5*gsl_pow_2(da/siga))/(siga*sqrt(2.0*M_PI))
                        *siga/3.0*
                   (    (1.0+POL)/2.0*( TPLUS *Amicelle_pp(q,param)
                                       +TMINUS*Amicelle_pm(q,param))
                    +	(1.0-POL)/2.0*( TMINUS*Amicelle_mm(q,param)
                                       +TPLUS *Amicelle_mp(q,param))
                   );
        }
        YAWPEP = origyaw*180/M_PI;
        return avgT;
	}

}

scalar sasfit_ff_ff_chain_rw_psi_POLARIS_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return V(R_CORE+T_SH+2.*sqrt(5./3.)*RG,param);
}

