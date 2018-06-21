/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define H0	param->p[1]
#define SIGMA_R	fabs(param->p[2])
#define SIGMA_H	fabs(param->p[3])


scalar XI_CylShell(scalar q, sasfit_param * param)
{
	scalar qR, qH, XIres,R,H;

	SASFIT_ASSERT_PTR(param);
    R=R0*NU1;
    H=H0*NU2;
	qR = q*R*sin(ALPHA);
	qH = q*H*cos(ALPHA)/2.;

	if ((R+H) == 0) return 0;

/*
	if (xR == 0)
	{
		XIres = R/(R+H) * cos(xH);
	} else
	{
		XIres = R/(R+H) * 2.*gsl_sf_bessel_J1(xR)/xR * cos(xH);
	}
*/
    XIres = R/(R+H) * gsl_sf_hyperg_0F1(2,-0.25*gsl_pow_2(qR))* cos(qH);
	XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(qR)*gsl_sf_bessel_j0(qH);

	return (2.* M_PI*R*R + 2.0*M_PI*R*H)*XIres;
}


scalar thinCyl_P_core(scalar alpha, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	ALPHA = alpha;

	return  gsl_pow_2(XI_CylShell(Q, param)) * sin(ALPHA);
}

scalar thinCyl_P(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return sasfit_integrate(0.0, M_PI/2.0, thinCyl_P_core, param);
}



scalar ThinCylShell_H_core(scalar xH, sasfit_param * param)
{
	scalar IP, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	NU2 = xH/H0;

	IP = thinCyl_P(Q,param);

	sasfit_init_param( &subParam );

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_H;
	subParam.p[2] = 1.0;
	subParam.p[3] = H0;

	LNdistr = sasfit_sd_LogNorm(xH, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*IP;
}

scalar ThinCylShell_H(scalar q,sasfit_param * param)
{
	scalar Lstart = 0.0, Lend = 0.0;
	scalar Pprime;

	SASFIT_ASSERT_PTR(param);

    if (SIGMA_H == 0.0)
    {
        NU2 = 1;
        Pprime = thinCyl_P(Q, param);
    } else
    {
        find_LogNorm_int_range(2,H0,SIGMA_H,&Lstart,&Lend,param);
        Pprime 	= sasfit_integrate(Lstart, Lend, &ThinCylShell_H_core, param);
    }
	return Pprime;
}

scalar ThinCylShell_core(scalar xR, sasfit_param * param)
{
	scalar IP, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	NU1 = xR/R0;
	IP = ThinCylShell_H(Q,param);

	sasfit_init_param( &subParam );

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R0;

	LNdistr = sasfit_sd_LogNorm(xR, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*IP;
}

scalar ThinCylShell(scalar q, sasfit_param * param)
{
	scalar xstart = 0.0, xend = 0.0;
	scalar Pprime;

	SASFIT_ASSERT_PTR(param);

	Q = q;

    if (SIGMA_R == 0.0)
    {
        NU1 = 1;
        NU2 = 1;
        Pprime = ThinCylShell_H(Q,param);;
    } else
    {
        find_LogNorm_int_range(4,R0,SIGMA_R,&xstart,&xend,param);
        Pprime 	= sasfit_integrate(xstart, xend, ThinCylShell_core, param);
    }

	return Pprime;
}

scalar thinCyl_FtoP(sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return  LNDISTR1*LNDISTR2*gsl_pow_int(XI_CylShell(Q, param),lround(P)) * sin(ALPHA);
}

int thin_hollow_cylinder_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	if ((ndim == 1)) {
		LNDISTR1 = 1;
		NU1 = 1;
		LNDISTR2 = 1;
		NU2 = 1;
	} else if ((ndim == 2)) {
	    if (SIGMA_R==0) {
            LNDISTR1 = 1;
            NU1 = 1;
            subParam.p[0] = 1.0;
            subParam.p[1] = SIGMA_H;
            subParam.p[2] = 1.0;
            subParam.p[3] = 1.0;
            NU2=x[1];
            LNDISTR2 = sasfit_sd_LogNorm(NU2, &subParam);
            SASFIT_CHECK_SUB_ERR(param, subParam);
            if ( subParam.errStatus != FALSE ) {
                sasfit_out("LogNormError: SIGMA_H:%lf\n",SIGMA_H);
                return 1;
            }
        } else {
            LNDISTR2 = 1;
            NU2 = 1;
            subParam.p[0] = 1.0;
            subParam.p[1] = SIGMA_R;
            subParam.p[2] = 1.0;
            subParam.p[3] = 1.0;
            NU1=x[1];
            LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
            SASFIT_CHECK_SUB_ERR(param, subParam);
            if ( subParam.errStatus != FALSE ) {
                sasfit_out("LogNormError: SIGMA_R:%lf\n",SIGMA_R);
                return 1;
            }
        }
	} else if ((ndim == 3)) {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU1=x[1];
		LNDISTR1 = sasfit_sd_LogNorm(NU1, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA_R:%lf\n",SIGMA_R);
			return 1;
		}
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_H;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU2=x[2];
		LNDISTR2 = sasfit_sd_LogNorm(NU2, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA_H:%lf\n",SIGMA_H);
			return 1;
		}
	}
	ALPHA = x[0];
    fval[0] = (cparam->func)(param);
    return 0;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder(scalar q, sasfit_param * param)
{
	scalar err,sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    int intstrategy, ndim;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 < 0.0), param, "R(%lg) < 0",R0); // modify condition to your needs
	SASFIT_CHECK_COND1((H0 < 0.0), param, "H(%lg) < 0",H0); // modify condition to your needs

	// insert your code here
	//return ThinCylShell(q,param);

	cubxmin[0]=0;
	cubxmax[0]=M_PI_2;
	if (SIGMA_R==0 && SIGMA_H==0) {
        ndim=1;
	} else if (SIGMA_R==0 && SIGMA_H!=0) {
	    ndim=2;
        find_LogNorm_int_range(2,1,SIGMA_H,&cubxmin[1],&cubxmax[1],param);
	} else if (SIGMA_R!=0 && SIGMA_H==0) {
	    ndim=2;
        find_LogNorm_int_range(4,1,SIGMA_R,&cubxmin[1],&cubxmax[1],param);
	} else {
	    ndim = 3;
        find_LogNorm_int_range(4,1,SIGMA_R,&cubxmin[1],&cubxmax[1],param);
	    find_LogNorm_int_range(2,1,SIGMA_H,&cubxmin[2],&cubxmax[2],param);
	}
    P=2;
    Q=q;
	cparam.ndim=ndim;
	cparam.func = &thinCyl_FtoP;
    cparam.param = param;
    intstrategy = sasfit_get_int_strategy();

    if (ndim>=3) {
        intstrategy=H_CUBATURE;
    } else {
        intstrategy=P_CUBATURE;
    }

	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &thin_hollow_cylinder_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &thin_hollow_cylinder_cubature,&cparam,ndim, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
            sum=ThinCylShell(q,param);
            break;
            }
    }
    return sum;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

