/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables


void calc_QFVG(sasfit_param *param)
{
    int i;
    scalar M[3][3];
    scalar Q[3];
    scalar ef[3],ev[3],eg[3],c[3],s[3];
    s[0]=sin(PHI);  c[0]=cos(PHI);
    s[1]=sin(THETA);c[1]=cos(THETA);
    s[2]=sin(PSI);  c[2]=cos(PSI);
    M[0][0]= c[0]*c[1]*c[2]-s[0]*s[2];
    M[0][1]=-c[0]*c[1]*s[2]-s[0]*c[2];
    M[0][2]=                c[0]*s[1];
    M[1][0]= s[0]*c[1]*c[2]+c[0]*s[2];
    M[1][1]=-s[0]*c[1]*s[2]+c[0]*c[2];
    M[1][2]=                s[0]*s[1];
    M[2][0]=               -s[1]*c[2];
    M[2][1]=                s[1]*s[2];
    M[2][2]=                c[1];
    ef[0] = M[0][0];
    ef[1] = M[1][0];
    ef[2] = M[2][0];
    ev[0] = M[0][1];
    ev[1] = M[1][1];
    ev[2] = M[2][1];
    eg[0] = M[0][2];
    eg[1] = M[1][2];
    eg[2] = M[2][2];
    Q[0] = QMOD*cos(VARPHI);
    Q[1] = QMOD*sin(VARPHI);
    Q[2] = 0;
    QF=0;QV=0;QG=0;
    for (i=0;i<3;i++) {
        QF = QF + Q[i]*ef[i];
        QV = QV + Q[i]*ev[i];
        QG = QG + Q[i]*eg[i];
    }

}
scalar sasfit_ff_shearflow_lin_core(sasfit_param *param)
{
    scalar b,c,expbnu;
    b= gsl_pow_2(RG*ALPHA*QF)+gsl_pow_2(RG*BETA *QV)+gsl_pow_2(RG*GAMMA *QG);
    c= gsl_pow_2(RG*A    *QF)+gsl_pow_2(RG*B    *QV)+gsl_pow_2(RG*C     *QG);
    expbnu = exp(-b*FP);

    return 2*(expbnu/c+(1-expbnu)*(1/b-1/(b*b))+expbnu/(c*c)*(exp(-c*(1-FP))-1)+expbnu*(1/b-1/c)*FP);

}

scalar sasfit_ff_shearflow_lin(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs

	// insert your code here
	QMOD = q;
	VARPHI = sasfit_param_override_get_psi(VARPHI_DEG*M_PI/180.);
    PHI = PHI_DEG*M_PI/180.;
    THETA = THETA_DEG*M_PI/180.;
    PSI = PSI_DEG*M_PI/180.;
	calc_QFVG(param);
	return I0*sasfit_ff_shearflow_lin_core(param);
}

scalar sasfit_ff_shearflow_lin_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_shearflow_lin_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

