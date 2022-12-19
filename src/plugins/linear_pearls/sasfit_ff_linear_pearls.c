/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.

	Author(s) of this file: Martin Schmiele (martin.schmiele@nbi.ku.dk)
*/

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R			param->p[0]
#define EDGE_SEP	param->p[1]
#define NUM_PEARLS	param->p[2]
#define ETA			param->p[3]
#define ETA_SOL		param->p[4]


// return normalized sphere form amplitude
scalar linear_pearls_sphere_f(scalar x)
{
	if (x != 0.0) { return 3.0 * (sin(x) - x*cos(x)) / gsl_pow_3(x) ; }
	else { return 1.0 ; }
}

// returns sinc function
scalar linear_pearls_sinc(scalar x)
{
	if (x != 0.0) { return sin(x) / x ; }
	else { return 1.0 ; }
}


// return form factor (includes the intrinsical structure factor) of linear pearls
scalar sasfit_ff_linear_pearls_kernel(scalar q, scalar NN, sasfit_param * param)
{
	scalar contrast_pearl, pearl_vol, tot_vol, separation, psi, structure_factor ;

	SASFIT_ASSERT_PTR(param) ; // assert pointer param is valid

	// contrast
	contrast_pearl = ETA - ETA_SOL ;

	// volume of one sphere/pearl
	pearl_vol = 4./3. * M_PI * gsl_pow_3(R) ;

	// total volume
	tot_vol = NN * pearl_vol ;
	if (NN < 1 || pearl_vol == 0) return 0;

	// center to center distance between the neighboring pearls
	separation = EDGE_SEP + 2.0 * R ;

	// scattering amplitude of one pearl
	psi = sasfit_sphere_fc(q , R) ;

	// interaction terms
	structure_factor = NN ; // self-correlations
	for( int num=1; num<NN; num++) // X-correlations
	{
		structure_factor += 2.0 * (NN - (scalar)num) * gsl_sf_sinc(q/M_PI * separation * (scalar)num) ;
	}

	// form factor
	return structure_factor * gsl_pow_2(contrast_pearl * pearl_vol * psi) / tot_vol ;
}



scalar sasfit_ff_linear_pearls(scalar q, sasfit_param * param)
{
	scalar w, ff_1, ff_2, NN ;

	SASFIT_ASSERT_PTR(param) ; // assert pointer param is valid

	// check ranges for geometrical and other params
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q) ;
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0", R) ;
	SASFIT_CHECK_COND1((EDGE_SEP < 0.0), param, "egde_sep(%lg) < 0", EDGE_SEP) ;
	SASFIT_CHECK_COND1((NUM_PEARLS < 0.0), param, "num_pearls(%lg) < 0", NUM_PEARLS) ;

	// deal with double numbers for NUM_PEARLS, calculate weighted average for pearls with NN=floor(NUM_PEARLS) and NN+1
	NN = floor(NUM_PEARLS) ;
	w = NUM_PEARLS - floor(NUM_PEARLS) ;
	ff_1 = sasfit_ff_linear_pearls_kernel( q, NN, param) ;

	if ( w > 0.0 ) { ff_2 = sasfit_ff_linear_pearls_kernel( q, NN+1.0, param) ; }
	else { ff_2 = 0.0 ; }

//	printf( "I=%f\n", (1.0-w) * ff_1 + w * ff_2) ;
	return (1.0-w) * ff_1 + w * ff_2 ;
}


// return form amplitude (includes the intrinsical structure factor) of linear pearls
// here: return a dummy value (this function is required in build step)
scalar sasfit_ff_linear_pearls_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
//	printf( "f=0.0\n" ) ;
	return 0.0 ;
}


// return volume of linear pearls
scalar sasfit_ff_linear_pearls_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
//	printf( "v=%f\n", NUM_PEARLS * 4./3. * M_PI * gsl_pow_3(R) ) ;
	return NUM_PEARLS * 4./3. * M_PI * gsl_pow_3(R) ; // (1.0-w) + w == 1.0
}

