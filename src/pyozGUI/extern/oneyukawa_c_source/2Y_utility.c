#include "include/2Y_utility.h"
double chop( double x )
{
	if ( fabs(x) < 1E-6 )
		return 0;
	else
		return x;
}
