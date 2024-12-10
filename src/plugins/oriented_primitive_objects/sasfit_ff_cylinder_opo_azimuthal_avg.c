/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
opo_data cyl_a_opod;

scalar sasfit_ff_cylinder_kernel_f(scalar psi, sasfit_param * param) {
    PSI_DEG = psi;
    return sasfit_ff_cylinder_opo_f(cyl_a_opod.Qmod,param);
}

scalar sasfit_ff_cylinder_kernel(scalar psi, sasfit_param * param) {
    PSI_DEG = psi;
    return sasfit_ff_cylinder_opo(cyl_a_opod.Qmod,param);
}

scalar sasfit_ff_cylinder_opo_azimuthal_avg(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
    cyl_a_opod.Qmod = q;
    return sasfit_integrate(-90,90,&sasfit_ff_cylinder_kernel,param)/180.;
}

scalar sasfit_ff_cylinder_opo_azimuthal_avg_f(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	cyl_a_opod.Qmod = q;
    return sasfit_integrate(-90,90,&sasfit_ff_cylinder_kernel_f,param)/180;
}

scalar sasfit_ff_cylinder_opo_azimuthal_avg_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_ff_cylinder_opo_v(q, param, dist);
}

