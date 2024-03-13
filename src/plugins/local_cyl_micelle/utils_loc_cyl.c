/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar F_PSI(scalar fQ, scalar fR, scalar fH, scalar alpha)
{
        scalar xR, xH, PSIres;

        xR = fQ*fR*sin(alpha);
        xH = fQ*fH*cos(alpha)/2.0;

        PSIres = 2.0*sasfit_jinc(xR)*gsl_sf_bessel_j0(xH);


        return PSIres;
}

scalar XI(scalar fQ, scalar fR, scalar fH, scalar alpha)
{
        scalar xR, xH, XIres;

        xR = fQ*fR*sin(alpha);
        xH = fQ*fH*cos(alpha)/2.0;

        if ((fR+fH) == 0.0) return 0.0;

        XIres = fR/(fR+fH) * 2.0*sasfit_jinc(xR) * cos(xH);

        XIres = XIres + fH/(fR+fH) * gsl_sf_bessel_J0(xR)*gsl_sf_bessel_j0(xH);

        return XIres;
}

scalar sasfit_ff_P18(scalar fQ, scalar fRG)
{
        scalar u;
        u = fQ*fQ * fRG*fRG;
        if ( u == 0 ) return 1.0;

        return 2*(exp(-u)+u-1.0) / (u*u);
}

scalar sasfit_ff_cyl_rwbrush_fs_core(scalar alpha, sasfit_param *param)
{
	SASFIT_ASSERT_PTR(param);

	return pow( F_PSI(Q,R,H,alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_scc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(Q, RG);

	return pow(w*XI(Q, R+D*RG, H+2.0*D*RG, alpha), 2.0) * sin(alpha);
}

scalar sasfit_ff_cyl_rwbrush_ssc_core(scalar alpha, sasfit_param *param)
{
	scalar w;

	SASFIT_ASSERT_PTR(param);

	w	= sasfit_rwbrush_w(Q, RG);

	return w * F_PSI(Q, R, H, alpha) * XI(Q, R+D*RG, H+2.0*D*RG, alpha) * sin(alpha);
}

