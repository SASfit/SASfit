/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_C	param->p[0]
#define DR1	param->p[1]
#define DR2	param->p[2]
#define L	param->p[3]
#define ETA_C	param->p[4]
#define ETA_1	param->p[5]
#define ETA_2	param->p[6]
#define ETA_SOL	param->p[7]
#define X_SOL_SH2	param->p[8]
#define N	param->p[9]





scalar sasfit_ff_doubleshellchain(scalar q, sasfit_param * param)
{
	scalar A, w, R12, ftmp1, ftmp2, Detac, Deta1, Deta2, Kval1, Kval2, Kval3;
	int i,j,NN;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Deta2 = (1.0-X_SOL_SH2)*(ETA_2-ETA_SOL);
	Deta1 = (ETA_1-ETA_SOL);
	Detac = (ETA_C-ETA_SOL);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N <= 0.0), param, "n(%lg) <= 0",N);
	SASFIT_CHECK_COND1((fabs(X_SOL_SH2-0.5) > 0.5), param, "|X_SOL_SH2 - 0.5|(%lg) > 0.5",fabs(X_SOL_SH2-0.5));
	SASFIT_CHECK_COND1(((2*(R_C+DR1+DR2)+L) <= 0.0), param, "(2*(Rc+DR1+DR2)+L)(%lg) <= 0",(2*(R_C+DR1+DR2)+L));
	SASFIT_CHECK_COND1(((fabs(Detac)+fabs(Deta1)+fabs(Deta2)) <= 0.0), param, "(|Detac|+|Deta1|+|Deta2|)(%lg) <= 0",(fabs(Detac)+fabs(Deta1)+fabs(Deta2)));

	sasfit_init_param( &subParam );

	Kval1 = sphere_f(q,R_C+DR1+DR2,Deta2);
	Kval2 = sphere_f(q,R_C+DR1,Deta1-Deta2);
	Kval3 = sphere_f(q,R_C, Detac-Deta1);

	A = Kval1 + Kval2 + Kval3;

	NN = lround(floor(N));
	w = N-floor(N);

	ftmp1 = 0.0;
	for (i=0; i < NN ;i++)
	{
		for (j=0; j < NN ;j++)
		{
			R12 = fabs(j-i)*(2.0*(R_C+DR1+DR2)+L);
			if (q*R12 <= 0)
			{
				ftmp1 = ftmp1 + A*A;
			} else {
				ftmp1 = ftmp1+A*A*sin(q*R12)/(q*R12);
			}
		}
	}
	ftmp2 = ftmp1;
	if (w!=0) {
        for (i=NN; i < NN+1 ;i++)
        {
            for (j=0; j < NN ;j++)
            {
                R12 = fabs(j-i)*(2.0*(R_C+DR1+DR2)+L);
                if (q*R12 <= 0)
                {
                    ftmp2 = ftmp2 + A*A;
                } else {
                    ftmp2 = ftmp2+A*A*sin(q*R12)/(q*R12);
                }
            }
        }
        for (i=0; i < NN+1 ;i++)
        {
            for (j=NN; j < NN+1 ;j++)
            {
                R12 = fabs(j-i)*(2.0*(R_C+DR1+DR2)+L);
                if (q*R12 <= 0)
                {
                    ftmp2 = ftmp2 + A*A;
                } else {
                    ftmp2 = ftmp2+A*A*sin(q*R12)/(q*R12);
                }
            }
        }
	}
	return (1-w)*ftmp1+w*ftmp2;
}

scalar sasfit_ff_doubleshellchain_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_doubleshellchain_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

