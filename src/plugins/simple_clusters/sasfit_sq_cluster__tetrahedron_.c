/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define L_2	param->p[0]
#define N	param->p[1]

scalar sasfit_sq_cluster__tetrahedron_(scalar q, sasfit_param * param)
{
	int i,j,n;
	scalar R[5][5], ftmp;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L_2 <= 0.0), param, "t(%lg) <= 0",L_2); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "n(%lg) < 1",N); // modify condition to your needs
	SASFIT_CHECK_COND1((N > 5.0), param, "n(%lg) > 5",N); // modify condition to your needs

	
	n = (int) N;

	for (i=0; i < 5 ;i++) 
	{
		for (j=0; j < 5 ;j++) 
		{
			R[i][j] = L_2*2.0;
		}
	}
	for (i=0; i < 5 ;i++) 
	{
		R[i][i] = 0.0;
	}
	R[3][4] = sqrt(6.0/3.0)*L_2*2.0;
	R[4][3] = R[3][4];

	ftmp = 0.0;
	for (i=0; i < n ;i++) 
	{
		for (j=0; j < n ;j++) 
		{
			if (q*R[i][j] <= 0) 
			{
				ftmp = ftmp + 1.0;
			} else 
			{
				ftmp = ftmp+sin(q*R[i][j])/(q*R[i][j]);
			}
		}
	}
	return ftmp/n;
}

scalar sasfit_sq_cluster__tetrahedron__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_cluster__tetrahedron__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

