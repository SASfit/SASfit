/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sasfit_ff_cylinder_opo_random(scalar q, sasfit_param * param)
{
    scalar Iavg;
    int available, order, n, rule_max=65, i;
    double *w, *x, *y, *z;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B <= 0.0), param, "b(%lg) <= 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) <= 0",C); // modify condition to your needs

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	opod.a = A;
	opod.b = B;
	opod.c = C;
    opod.Rotation.convention = yaw_pitch_roll;
    opo_setEulerAngles(&opod,0,0,0);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be not linear independent");

    opod.Qmod = q;
    order = sasfit_order_table ( rule_max );
    for ( n = lround(sasfit_eps_get_robertus_p()); n <= rule_max; n++ ) {
        available = sasfit_available_table ( n );
        if ( available ) {
            order = sasfit_order_table ( n );
            break;
        }
    }

    w = ( double * ) malloc ( order * sizeof ( double ) );
    x = ( double * ) malloc ( order * sizeof ( double ) );
    y = ( double * ) malloc ( order * sizeof ( double ) );
    z = ( double * ) malloc ( order * sizeof ( double ) );
    sasfit_ld_by_order ( order, x, y, z, w );
    Iavg = 0;
    for (i=0;i<order;i++) {
        opod.Q[0] = q*x[i];
        opod.Q[1] = q*y[i];
        opod.Q[2] = q*z[i];
        opo_setQhat(&opod);
        Iavg = Iavg+w[i]*gsl_pow_2((ETA_P-ETA_M) *opo_Fc(&opod));
    }
    free ( x );
    free ( y );
    free ( z );
    free ( w );
    return Iavg;
}

scalar sasfit_ff_cylinder_opo_random_f(scalar q, sasfit_param * param)
{
    scalar psi, Iavg;
    int available, order, n, rule_max=65, i;
    double *w, *x, *y, *z;
	opo_data opod;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ea,EA_X,EA_Y,EA_Z),0.0),param,"vector [EA_X,EA_Y,EA_Z] must have a norm != 0");
	SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.eb,EB_X,EB_Y,EB_Z),0.0),param,"vector [EB_X,EB_Y,EB_Z] must have a norm != 0");
    SASFIT_CHECK_COND(SASFIT_EQUAL(opo_set_e(opod.ec,EC_X,EC_Y,EC_Z),0.0),param,"vector [EC_X,EC_Y,EC_Z] must have a norm != 0");

	opod.a = A;
	opod.b = B;
	opod.c = C;
    opo_setConvention(&opod,yaw_pitch_roll);
 //   opod.Rotation.convention =Z1_X2_Y3;
    opo_setEulerAngles(&opod,0,0,0);
    opo_init(&opod);

    SASFIT_CHECK_COND(SASFIT_EQUAL(opod.detDinv,0.0),param,"vectors ea, eb, ec seem to be linear dependent");

    opod.Qmod = q;
    order = sasfit_order_table ( rule_max );
    for ( n = lround(sasfit_eps_get_robertus_p()); n <= rule_max; n++ ) {
        available = sasfit_available_table ( n );
        if ( available ) {
            order = sasfit_order_table ( n );
            break;
        }
    }

    w = ( double * ) malloc ( order * sizeof ( double ) );
    x = ( double * ) malloc ( order * sizeof ( double ) );
    y = ( double * ) malloc ( order * sizeof ( double ) );
    z = ( double * ) malloc ( order * sizeof ( double ) );
    sasfit_ld_by_order ( order, x, y, z, w );
    Iavg = 0;
    for (i=0;i<order;i++) {
        opod.Q[0] = q*x[i];
        opod.Q[1] = q*y[i];
        opod.Q[2] = q*z[i];
        opo_setQhat(&opod);
        Iavg = Iavg+w[i]*(ETA_P-ETA_M) *opo_Fc(&opod);
    }
    free ( x );
    free ( y );
    free ( z );
    free ( w );
    return Iavg;
	return (ETA_P-ETA_M) *opo_Fc(&opod);
}

scalar sasfit_ff_cylinder_opo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

