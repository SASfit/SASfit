/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define L_2	param->p[0]
#define N	param->p[1]

scalar sasfit_sq_cluster__linear_chain_(scalar q, sasfit_param * param)
{
	int i,j,n;
	scalar ftmp,R12;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L_2 < 0.0), param, "L_2(%lg) < 0",L_2); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 0.0), param, "n(%lg) < 0",N); // modify condition to your needs

	// insert your code here

	ftmp = 0.0;
	n = (int) N;
	for (i=0; i < n ;i++)
	{
		for (j=0; j < n ;j++)
		{
			R12 = fabs(j-i)*L_2*2.0;
			if (q*R12 <= 0)
			{
				ftmp = ftmp + 1.0;
			} else {
				ftmp = ftmp+sin(q*R12)/(q*R12);
			}
		}
	}
	return ftmp/n;
}

scalar sasfit_sq_cluster__linear_chain__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_cluster__linear_chain__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

