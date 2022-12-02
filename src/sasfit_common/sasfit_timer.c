/*
 * src/sasfit_common/sasfit_timer.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/sasfit_timer.h"

sasfit_timer * sasfit_timer_create()
{
	sasfit_timer * sf_t = 0;
	sf_t = (sasfit_timer*)malloc( sizeof(sasfit_timer) );

	if ( !sf_t ) return 0;

#ifdef WIN32
	// get time frequency
	QueryPerformanceFrequency( &(sf_t->freq) );
#else
	// do nothing special
#endif
	sasfit_timer_start(sf_t);
	return sf_t;
}

void sasfit_timer_destroy(sasfit_timer ** sf_timer)
{
	if ( !sf_timer ) return;	// nothing to do
	if ( !*sf_timer ) return;	// nothing to do

	free(*sf_timer);
	*sf_timer = 0;
}

void sasfit_timer_start(sasfit_timer * sf_timer)
{
	if ( !sf_timer ) return;

	// get current time
#ifdef WIN32
	QueryPerformanceCounter( &(sf_timer->tstart) );
#else
	gettimeofday( &(sf_timer->tstart), &(sf_timer->tz) );
#endif
}

double sasfit_timer_measure(sasfit_timer * sf_timer)
{
	SASFIT_TIMEUNIT tend;

	if ( !sf_timer ) return -1.0;

	// measure elapsed time
#ifdef WIN32
	QueryPerformanceCounter( &tend );
        return ((double)tend.QuadPart - (double)((sf_timer->tstart).QuadPart))/((double)((sf_timer->freq).QuadPart));
#else
	gettimeofday( &tend, &(sf_timer->tz) );
	double t1, t2;
	t1 =  (double)(sf_timer->tstart).tv_sec + (double)(sf_timer->tstart).tv_usec/(double)(1000*1000);
        t2 =  (double)tend.tv_sec + (double)tend.tv_usec/(double)(1000*1000);
        return (t2-t1);
#endif
}
