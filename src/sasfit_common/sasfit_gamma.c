/*
 * src/sasfit_common/sasfit_gamma.c
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

#include <math.h>
#include "include/sasfit_common.h"

scalar sasfit_gammaln(scalar xx)
{
	scalar x,tmp,ser;
	static scalar cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

	x=xx-1.0;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.0;
	for (j=0;j<=5;j++) {
		x += 1.0;
		ser += cof[j]/x;
	}
	return -tmp+log(2.50662827465*ser);
}

scalar sasfit_gamma(scalar x) 
{ 
	return exp(sasfit_gammaln(x)); 
}

scalar sasfit_dgammaln_dx(scalar x)
{
	scalar tmp,ser1,ser2;
	static scalar cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

        tmp = -1.0+log(x+4.5) + (x-0.5)/(x+4.5);
	ser1 = 1.0;
	ser2 = 0.0;
	for (j=0;j<=5;j++) {
		ser1 += cof[j]/(x+j);
		ser2 -= cof[j]/((x+j)*(x+j));
	}
        return tmp + ser2/ser1;
}

scalar sasfit_dgamma_dx(scalar x) 
{ 
	return sasfit_dgammaln_dx(x)*exp(sasfit_gammaln(x)); 
}
