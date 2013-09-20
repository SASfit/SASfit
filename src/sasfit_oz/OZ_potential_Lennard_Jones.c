/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Modified 17.09.2013
 */
#include "OZ_solver.h"
#include "OZ_potentials.h"

//Depletion potential:
#define sigma p[0] //Diameter of spheres in nm
#define epsilon p[1] //  is the strength of the interaction in units of kb*T

double U_Lennard_Jones(double r, double T, double *p) {
    if (r == 0) {
        return GSL_POSINF;
    } else {
        return 4*kb*T*epsilon*(pow(sigma/r,12)-pow(sigma/r,6));
    }
}
