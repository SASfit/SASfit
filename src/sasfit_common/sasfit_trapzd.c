/*
 * src/sasfit_common/sasfit_trapzd.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_math.h>
#include "include/sasfit_common.h"

void sasfit_polint(scalar xa[], scalar ya[], int n, scalar x, scalar * y, scalar * dy)
{
	int i,m,ns=1;
	scalar den,dif,dift,ho,hp,w;
	scalar *c = 0, *d = 0;

	dif = fabs(x-xa[1]);
	sasfit_init_vector(&c, n+1);
	sasfit_init_vector(&d, n+1);
	for (i=1; i <= n ;i++) 
	{
		if ( (dift=fabs(x-xa[i])) < dif) 
		{
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	*y = ya[ns--];
	for (m=1; m < n ;m++) 
	{
		for (i=1; i <= n-m ;i++) 
		{
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if ( (den = ho-hp) == 0.0) 
			{
				sasfit_err("ho-hp == 0.0\n");
				sasfit_free_vector(&c);
				sasfit_free_vector(&d);
				return;
			}
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	sasfit_free_vector(&c);
	sasfit_free_vector(&d);
}

scalar sasfit_trapzd(scalar a, scalar b, scalar n, sasfit_func_one_t * trapzd_fct, sasfit_param * param)
{
	scalar x,tnm,sum,del;
	scalar t1, t2;
	static scalar s;
	static int it;
	int j;

        SASFIT_ASSERT_PTR(trapzd_fct);
        SASFIT_ASSERT_PTR(param);

	if (n == 1) 
	{
		it=1;
		t1 = trapzd_fct(a, param);
		t2 = trapzd_fct(b, param);
		return (s=0.5*(b-a)*(t1+t2));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0, j=1; j <= it ;j++, x+=del) 
		{
			t1 = trapzd_fct(x, param);
			sum += t1;
		}
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
		return s;
	}
}

#define SASFIT_JMAX	22
#define SASFIT_JMAXP 	SASFIT_JMAX+1
#define K 		5

scalar sasfit_qromb(scalar a, scalar b, sasfit_func_one_t * trapzd_fct, sasfit_param * param)
{
	scalar ss,dss;
	scalar s[SASFIT_JMAXP+1], h[SASFIT_JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j <= SASFIT_JMAX;j++) 
	{
		s[j] = sasfit_trapzd(a, b, j, trapzd_fct, param);
		if (j >= K) 
		{
			sasfit_polint(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
			if (fabs(dss) <= sasfit_eps_get_aniso()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	sasfit_err("Too many steps !");
	sasfit_err("\ndss: %lg, FitPrecis.epsanisotropic: %lg, ss: %lg\n",dss,sasfit_eps_get_aniso(),ss);
	return 0.0;
}


