/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 */
#include "OZ_solver.h"
#include "OZ_potentials.h"

//Depletion potential:
#define sigma p[0] //Diameter of spheres in nm

double U_Hard_Sphere(double r, double T, double *p) {
    if (r<sigma) return GSL_POSINF;
    else  return 0;
}
