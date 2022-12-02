/*
 * src/sasfit_ff/ellip/utils.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/private.h"

scalar sasfit_ff_ellip_1_core(scalar theta, sasfit_param * param)
{
        scalar t1,t2,t3,t5,t7,t9,t10,t13,t14,t18,t20,t21,t23,t24,t25,t30,t31,t32,t36,t41;

        SASFIT_ASSERT_PTR(param);

	if (SASFIT_EQUAL(V[R], 0.0) || SASFIT_EQUAL(V[VOL], 0.0))
	{
		return 0.0;
	}
	else if (SASFIT_EQUAL(V[Q], 0.0))
	{
		return V[VOL]*V[VOL];
	} else
	{
		t1 = V[VOL]*V[VOL];
		t2 = V[R]*V[R];
		t3 = t2*t2;
		t5 = 1/t3/t2;
		t7 = sin(theta);
		t9 = V[Q]*V[R];
		t10 = 0.3141592653589793E1*0.3141592653589793E1;
		t13 = cos(theta);
		t14 = t13*t13;
		t18 = t7*t7;
		t20 = 9.0*t1/t10*t5*t14+16.0*t18;
		t21 = sqrt(t20);
		t23 = t9*t21/4.0;
		t24 = sin(t23);
		t25 = cos(t23);
		t30 = pow(t24-t9*t21*t25/4.0,2.0);
		t31 = V[Q]*V[Q];
		t32 = t31*t31;
		t36 = t20*t20;
		t41 = 36864.0*t1*t5*t7*t30/t32/t31/t36/t20;
		return t41;
	}
}

scalar sasfit_ff_ellip_2_core(scalar theta, sasfit_param * param)
{
        scalar t1,t2,t4,t6,t7,t8,t10,t11,t12,t14,t15,t17,t21,t23,t26,t27,t33;

        SASFIT_ASSERT_PTR(param);

	if (SASFIT_EQUAL(V[R], 0.0) || SASFIT_EQUAL(V[NU], 0.0))
	{
		return 0.0;
	}
	else if (SASFIT_EQUAL(V[Q], 0.0))
	{
		return pow(4.0/3.0 * M_PI * V[R]*V[R]*V[R] * V[NU] ,2.0);
	} else {
		t1 = 0.3141592653589793E1*0.3141592653589793E1;
		t2 = V[NU]*V[NU];
		t4 = 1.0;
		t6 = sin(theta);
		t7 = cos(theta);
		t8 = t7*t7;
		t10 = t6*t6;
		t11 = t2*t8+t10;
		t12 = sqrt(t11);
		t14 = V[R]*t12*V[Q];
		t15 = sin(t14);
		t17 = cos(t14);
		t21 = pow(t15-V[Q]*V[R]*t12*t17, 2.0);
		t23 = t11*t11;
		t26 = V[Q]*V[Q];
		t27 = t26*t26;
		t33 = 16.0*t1*t2*t4*t6*t21/t23/t11/t27/t26;
		return t33;
	}
}
scalar sasfit_ff_porod_cyl_v(scalar R, sasfit_param * param, int distr)
{
	scalar v = R;

	SASFIT_ASSERT_PTR(param);

	if ( distr != 0 ) v = param->p[0];
	if ( distr != 1 ) R = param->p[1];

	return M_PI * R * pow(v, 2.0);
}

scalar sasfit_ff_ellip_v(scalar q, sasfit_param * param, int distr)
{
	scalar v = 0.0;

	SASFIT_ASSERT_PTR(param);

	if ( param->kernelSelector == ELLIP2 )
	{
		v = 4.0/3.0 * sasfit_ff_porod_cyl_v(q, param, distr);
	} else
	if ( param->kernelSelector == ELLIP1 )
	{
		if ( distr != 1 ) v = V[PARAM1];
	} else
	{
		SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
	}
	return v;
}

