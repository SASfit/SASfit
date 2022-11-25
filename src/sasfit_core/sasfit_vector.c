/*
 * src/sasfit_core/sasfit_vector.c
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

#include "include/sasfit_core.h"

// what is nl and nh for ? i.e. their meaning ?

float * vector(int nl, int nh)
{
        float *v;
        int k;
	nl = 0;
        v=(float *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(float));
        if (!v) sasfit_err("allocation failure !");
        for (k=nl;k<=nh;k++) v[k] = 0.0;
        return v-nl;
}

int * ivector(int nl, int nh)
{
        int *v;
	nl = 0;
        v=(int *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(int));
        if (!v) sasfit_err("allocation failure in ivector()");
        return (v-nl);
}

double * dvector(int nl, int nh)
{
        double *v;
	int k;
	nl = 0;
        v=(double *) Tcl_Alloc((unsigned) (nh-nl+1)*sizeof(double));
        if (!v) sasfit_err("allocation failure in dvector()");
        for (k=nl;k<=nh;k++) v[k] = 0.0;
        return v-nl;
}



void free_vector(float * v, int nl, int nh)
{
	nl=0;
        Tcl_Free((char *) (v+nl));
}

void free_ivector(int * v, int nl, int nh)
{
	nl=0;
        Tcl_Free((char *) (v+nl));
}

void free_dvector(double * v, int nl, int nh)
{
	nl=0;
        Tcl_Free((char *) (v+nl));
}

