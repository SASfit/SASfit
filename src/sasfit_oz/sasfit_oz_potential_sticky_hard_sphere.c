/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   16.09.2013
 */
#include <sasfit_oz.h>

//Sticky hard sphere:
#define SIGMA p[0] // Diameter of spheres in nm
#define TAU   p[1] // stickiness parameter
#define DELTA p[2] // width of the square well
 double U_Sticky_Hard_Sphere(double d, double temp, double *p) {
    if (d<SIGMA) {
        return GSL_POSINF;
    } else if (d>=SIGMA&&d<SIGMA+fabs(DELTA)) {
 //       printf("%lf war hier\n",d);
        return kb*temp*log(12.0*TAU*fabs(DELTA)/(SIGMA+fabs(DELTA)));
    } else {
        return 0.0;
    }
}

