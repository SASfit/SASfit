/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_metalog_log_profile(scalar x, sasfit_param * param)
{
	metalog_param mp;
    gsl_function F;
    gsl_function_fdf FDF;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	if (x<0) return N;
	if (x>=BU) return 0;

	param->p[1]=0;
    param->moreparam=&mp;

    F.params=param;
    F.function = &root_metalog_Log_f;
    FDF.params=param;
    FDF.f  = &root_metalog_Log_f;
    FDF.df  = &root_metalog_Log_df;
    FDF.fdf = &root_metalog_Log_fdf;
    param->moreparam=&mp;
    assign_metalog_par(x, &mp,param);
    if (sasfit_get_rootalg()>3) {
        return N*(1-find_root_fdf_metalog(&FDF));
    } else {
        return N*(1-find_root_f_metalog(&F));
    }
}

scalar sasfit_ff_metalog_log_profile_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	return 0;
}

scalar sasfit_ff_metalog_log_profile_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
