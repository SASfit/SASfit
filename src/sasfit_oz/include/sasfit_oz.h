/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */
/**
This package uses pair potential as an input to calculate such characteristics as:
 -structure factor S(q);
 -radial distribution function g(r)
 -direct correlation function c(r);
 -indirect correlation function gamma(r)
 -bridge function b(r)
 -Mayer-f function f(r)
 -cavity function y(r)

Calculations are done by iterative solution of Ornstein-Zernike equation with a chosen closure relation.

Structure OZdata is a principal element of this package which encapsulates input and output variables.
Input variables that describe physical system itself:
- OZD.phi —  particles volume fraction;
- OZD.pPot — array with parameters of interaction potental;
- OZD.potential - pointer to definition of pair potential;
- OZD.T - temperature;
- OZD.cl - specifies closure relation that will be used in calculation

Input variables that determine technical aspects of computation:
- OZD.maxstpes - maximum number of itterations to solve the Ornstein-Zernike equation;
- OZD.Npoints - number of points of a grid for the discrete sine transformation (inside OZ equation);
- OZD.dr - determines distance in r-space between two neighboring points;
- OZD.relerror - parameter that sets a convergence criterion for the solution of OZ equation.
                 Relative change of norm of structure factor between two iterations;
- OZD.mixcoeff - mixing parameter that determines in which proportion solution on step n will be mixed with the one on step n-1;
                 Used to improve convergence rate;

Output variables:
- OZD.Sq - structure factor S(q);
- OZD.gr - radial distribution function g(r);
- OZD.cr - direct correlation function c(r);
- OZD.Br - bridge function b(r)
- OZD.yr - cavity function y(r)
- OZD.fr - Mayer-f function f(r)
- OZD.G  - indirect correlation function gamma(r)
**/


#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_const_mksa.h>
#define PiCube 8*gsl_pow_3(M_PI)
#define kb GSL_CONST_MKSA_BOLTZMANN
#include <fftw3.h>

#include "sasfit_oz_tclcmd.h"
#include "sasfit_oz_potentials.h"

typedef enum {
        PY,     // Percus-Yevik
        HNC,    // Hypernetted Chain
        RHNC,   // Refernce Hypernetted Chain
        RY,     // Roger-Young
        Verlet, // Verlet
        BPGG,   // Ballone, Pastore, Galli, and Gazzillo Approximations.
        MSA,    // Mean Spherical Approximation
        mMSA,   // modified Mean Spherical Approximation
        SMSA,   // "Soft core" MSA (SMSA) Approximation
        HMSA,   // HNC-SMSA Approximation from Zerah and Hansen
        VM,     // Vompe-Martynov Approximation.
        CG,     // Choudhury-Gosh
        DH,     // Duh-Haymet Approximation
        MS,     // Martynov-Sarkisov Approximation
        CJVM,   // Chapentier-Jakse' semiempirical extention of the VM Approximation
        BB      // Bomont-Bretonnet Approximation
} sasfit_oz_closure;

typedef struct {
        Tcl_Interp *interp;
        double *r, *k, *En, *G,
               *G0, *g, *g0, *c,
               *cf, *cfold, *cfnew,
               *Gf,*f, *S,  *ud,
               *Br, *yr, *fr;
        double dr, dq, dr_dsigma;
        double Sq0, gr0, cr0;
        double T;
        int    it;
        double beta;
        int    Npoints;
        double mixcoeff;
        int    maxsteps;
        double relerror;
        double alpha;
        double sBPGG;
        double phi;
        double *pPot;
        double *ubeta;
        sasfit_oz_closure cl;
        OZ_func_one_t * potential;
        OZ_func_one_t * reference_pot;
        OZ_func_one_t * pertubation_pot;
        OZ_func_one_t * repulsive_pot;
        OZ_func_one_t * attractive_pot;
        OZ_func_one_t * shortrange_pot;
        OZ_func_one_t * longrange_pot;
        double *in, *out;
        fftw_plan pl;
} sasfit_oz_data;

void OZ_init (sasfit_oz_data *);
void OZ_calculation (sasfit_oz_data *);
void OZ_solver (sasfit_oz_data *);
void OZ_init (sasfit_oz_data *);
void OZ_free (sasfit_oz_data *);
double extrapolate (double x1, double x2, double x3, double y1, double y2, double y3);
void OZ_pot_der (sasfit_oz_data *);
double compressibility_calc (double alpha, void * params);
void root_finding (sasfit_oz_data *);


//Modified 23.10.2013
