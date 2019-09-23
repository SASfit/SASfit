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

scalar sasfit_ff_tetrahedrondoubleshell(scalar q, sasfit_param * param)
{
	scalar A, R[5][5], R12, w, ftmp1, ftmp2, Detac, Deta1, Deta2;
	int i,j,NN;

	SASFIT_ASSERT_PTR(param);


	R12=2.0*(R_C+DR1+DR2)+L;
	for (i=0; i < 5 ;i++)
	{
		for (j=0; j < 5 ;j++)
		{
			R[i][j] = R12;
		}
	}
	for (i=0; i < 5 ;i++)
	{
		R[i][i] = 0.0;
	}
	R[3][4] = sqrt(6.0/3.0)*R12;
	R[4][3] = R[3][4];

	Deta2 = (1.0-X_SOL_SH2)*(ETA_2-ETA_SOL);
	Deta1 = (ETA_1-ETA_SOL);
	Detac = (ETA_C-ETA_SOL);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((fabs(X_SOL_SH2-0.5) > 0.5), param, "fabs(X_SOL_SH2-0.5)(%lg) > 0.5",fabs(X_SOL_SH2-0.5));
	SASFIT_CHECK_COND1((N > 5.0), param, "n(%lg) > 5",N);
	SASFIT_CHECK_COND1((N <= 0.0), param, "n(%lg) <= 0",N);
	SASFIT_CHECK_COND1((R12 <= 0.0), param, "R12(%lg) <= 0",R12);
	SASFIT_CHECK_COND1((fabs(Detac)+fabs(Deta1)+fabs(Deta2) == 0.0), param, "|Detac|+|Deta1|+|Deta2|(%lg) == 0",fabs(Detac)+fabs(Deta1)+fabs(Deta2));

	A  = sphere_f(q,R_C+DR1+DR2,Deta2);
	A += sphere_f(q, R_C+DR1,Deta1-Deta2);
	A += sphere_f(q, R_C, Detac-Deta1);

	ftmp1 = 0.0;
	ftmp2 = 0.0;
	A = A*A;

	NN = lround(floor(N));
	w = N-floor(N);


	for (i=0; i < 5 ;i++)
	{
		for (j=0; j < 5 ;j++)
		{
			if (q*R[i][j] <= 0)
			{
			    if (i<NN   && j < NN  ) ftmp1=ftmp1+A;
				if (i<NN+1 && j < NN+1) ftmp2=ftmp2+A;
			} else
			{
				if (i<NN   && j < NN  ) ftmp1 = ftmp1+A*sin(q*R[i][j])/(q*R[i][j]);
				if (i<NN+1 && j < NN+1) ftmp2 = ftmp2+A*sin(q*R[i][j])/(q*R[i][j]);
			}
		}
	}
	return (1-w)*ftmp1+w*ftmp2;
}

scalar sasfit_ff_tetrahedrondoubleshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_tetrahedrondoubleshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

