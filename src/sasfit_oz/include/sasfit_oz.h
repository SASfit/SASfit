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
- OZD.phi   particles volume fraction;
- OZD.pPot  array with parameters of interaction potental;
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
- OZD.hr - total correlation function h(r)
- OZD.G  - indirect correlation function gamma(r)
**/

#ifndef SASFIT_OZ
#define SASFIT_OZ

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_const_mksa.h>
#include <gsl/gsl_spline.h>
#define PiCube 8*gsl_pow_3(M_PI)

#include <fftw3.h>

#include "sasfit_oz_tclcmd.h"
#include "sasfit_oz_potentials.h"
#include <nvector/nvector_serial.h>

typedef enum {
    mix_const,
    mix_err,
    mix_reward_penalty
} sasfit_oz_mixstrategy;

typedef enum {
        PY,     // Percus-Yevik
        HNC,    // Hypernetted Chain
        RHNC,   // Refernce Hypernetted Chain
        MHNC,   // modified HNC
        RY,     // Roger-Young
        Verlet, // Verlet
        BPGG,   // Ballone, Pastore, Galli, and Gazzillo Approximations.
        MSA,    // Mean Spherical Approximation
        RMSA,   // rescaled Mean Spherical Approximation
        mMSA,   // modified Mean Spherical Approximation
        SMSA,   // "Soft core" MSA (SMSA) Approximation
        HMSA,   // HNC-SMSA Approximation from Zerah and Hansen
        VM,     // Vompe-Martynov Approximation.
        CG,     // Choudhury-Gosh
        DH,     // Duh-Haymet Approximation
        MS,     // Martynov-Sarkisov Approximation
        CJVM,   // Chapentier-Jakse' semiempirical extention of the VM Approximation
        BB,     // Bomont-Bretonnet Approximation
        KH,     // Kovalenko-Hirata closure
        EuRah   // EuRah closure
} sasfit_oz_closure;

typedef enum {
        dNewton,  // uses  gsl_multiroot_fsolver_dnewtonThe discrete Newton algorithm is the simplest method of solving a multidimensional system. It uses the Newton iteration
                  //
                  //  x -> x - J^{-1} f(x)
                  //
                  //  where the Jacobian matrix J is approximated by taking finite
                  //  differences of the function f. The approximation scheme used
                  //  by this implementation is,
                  //
                  //  J_{ij} = (f_i(x + \delta_j) - f_i(x)) /  \delta_j
                  //
                  // where \delta_j is a step of size \sqrt\epsilon |x_j| with
                  // \epsilon being the machine precision
                  // (\epsilon \approx 2.22 \times 10^-16).
                  // The order of convergence of Newtons algorithm is quadratic,
                  // but the finite differences require n^2 function evaluations
                  // on each iteration. The algorithm may become unstable if the
                  // finite differences are not a good approximation to the true
                  // derivatives.
                  //
        Hybrid,   // This is a finite difference version of the Hybrid algorithm without internal scaling.
        Hybrids,  // uses gsl_multiroot_fsolver_hybrids
                  //      This is a version of the Hybrid algorithm which
                  //      replaces calls to the Jacobian function by its finite
                  //      difference approximation. The finite difference
                  //      approximation is computed using gsl_multiroots_fdjac
                  //      with a relative step size of GSL_SQRT_DBL_EPSILON.
                  //      Note that this step size will not be suitable for all
                  //      problems.
                  //
        Broyden,  //      The Broyden algorithm is a version of the discrete Newton algorithm which
                  //      attempts to avoids the expensive update of the Jacobian matrix on each iteration.
                  //      The changes to the Jacobian are also approximated, using a rank-1 update,
                  //
                  //        J^{-1} \to J^{-1} - (J^{-1} df - dx) dx^T J^{-1} / dx^T J^{-1} df
                  //
                  //      where the vectors dx and df are the changes in x and f. On the first
                  //      iteration the inverse Jacobian is estimated using finite differences,
                  //      as in the discrete Newton algorithm.
                  //      This approximation gives a fast update but is unreliable if the changes
                  //      are not small, and the estimate of the inverse Jacobian becomes worse
                  //      as time passes. The algorithm has a tendency to become unstable unless
                  //      it starts close to the root. The Jacobian is refreshed if this instability
                  //      is detected (consult the source for details).
        Picard_iteration,   // fixed point iteration of the form: x_n+1 = OZ(xn)
        Mann_iteration,     //
        Ishikawa_iteration, //
        Noor_iteration,     //
        S_iteration,        //
        SP_iteration,       //
        CR_iteration,       //
        PicardS_iteration,  //
        PMH_iteration,      //
        MannII_iteration,   //
        Krasnoselskij_iteration, //
        Sstar_iteration,    //
        Steffensen2_iteration, //
        Steffensen4_iteration, //
        AndersonAcc,
        GMRES,  // routine from sundials/konsol library
        BiCGSTAB, // routine from sundials/konsol library
        TFQMR, // routine from sundials/konsol library
        FGMRES, // routine from sundials/konsol library
        KINSOLFP,
        BIGGS_ANDREWS
} sasfit_oz_root_algorithms;

typedef struct {
        Tcl_Interp *interp;
        double *r, *k, *En, *G, *Gprevious,
               *G0, *g, *g2, *g3, *g4, *g0, *c, *c_EuRah, *h,
               *cf, *cfold, *cfnew,
               *Gf,*f, *S, *S0pSq, *Q0pQ, *F2,  *ud,
               *Br, *yr, *fr;
        int *gate4g, indx_min_appearent_sigma, indx_max_appearent_sigma;
        gsl_vector *gamma_r;
        double dr, dq, dr_dsigma;
        double Sq0, gr0, cr0, F2xS_Inv, F2_Inv;
        double T;
        int    it;
        double beta;
        int    Npoints;
        double mixcoeff;
        int    maxsteps;
        double relerror;
        double alpha;
        double phi;
        double *pPot;
        double *ubeta;
        double GNorm;
        double SNorm;
        double chi_virial_route;
        double chi_compressibility_route;
        sasfit_oz_closure cl;
        sasfit_oz_root_algorithms root_algorithm;
        sasfit_oz_mixstrategy mixstrategy;
        OZ_func_one_t * potential;
        OZ_func_one_t * reference_pot;
        OZ_func_one_t * pertubation_pot;
        OZ_func_one_t * repulsive_pot;
        OZ_func_one_t * attractive_pot;
        OZ_func_one_t * shortrange_pot;
        OZ_func_one_t * longrange_pot;
        double *in, *out;
        fftw_plan pl;
        int interrupt;
        int failed;
        int PrintProgress;
        Tcl_Obj *oz_obj;
        int KINSetMAA;
        void  *kin_mem;
        double  KINSetFuncNormTol;
        double KINSetScaledSteptol;
        int KINSetNumMaxIters;
        int KINSetPrintLevel;
        int KINSetEtaForm;
        double KINSetEtaConstValue;
        int KINSpilsSetMaxRestarts;
        int KINSolStrategy;
        double KINSetMaxNewtonStep;
        gsl_interp_accel *acc_splineOZSQ;
        gsl_spline * OZSQakima_T;
        gsl_spline * OZSQcspline_T;
} sasfit_oz_data;

int OZ_init (sasfit_oz_data *);
int OZ_calculation (sasfit_oz_data *);
int OZ_solver (sasfit_oz_data *);
int OZ_free (sasfit_oz_data *);
static int OZ_step_kinsol(N_Vector, N_Vector, void *);
static int OZ_step_kinsolFP(N_Vector, N_Vector, void *);
static int OZ_EuRah_step_kinsol(N_Vector, N_Vector, void *);

double OZ_step(sasfit_oz_data *);
double extrapolate (double x1, double x2, double x3, double y1, double y2, double y3);
int OZ_pot_der (sasfit_oz_data *);
double compressibility_calc (double alpha, void * params);
void root_finding (sasfit_oz_data *);

void nsoli(scalar *,void *,sasfit_oz_data *,scalar *,scalar *,scalar *,int *);
//Modified 7.5.2014

#endif // SASFIT_OZ
