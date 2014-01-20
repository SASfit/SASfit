/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef FERROFLUID_PRIVATE_H
#define FERROFLUID_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_ferrofluid.h"

scalar FFmicelle_pp(scalar q, sasfit_param * param);
scalar FFmicelle_mm(scalar q, sasfit_param * param);
scalar FFmicelle_mp(scalar q, sasfit_param * param);
scalar FFmicelle_pm(scalar q, sasfit_param * param);
scalar FFmicelle(scalar q, scalar bs1, scalar bs2, scalar bc, sasfit_param * param);
scalar V(scalar x);
scalar Vg(scalar x, scalar sigma);
scalar FFphi(scalar x);
scalar phi(scalar x, sasfit_param * param);
scalar find_LAMBDA(sasfit_param * param);
scalar FFAcor(scalar q, scalar s, scalar Rco, scalar Rch,sasfit_param *param);
//
// add local defines here:
// #define P0 param->p[0]
//

#define R_TOT					param->p[0]
#define T_SHELL					param->p[1]
#define SNAGG					param->p[2]
#define SIGMA					1.0/SNAGG
#define VBRUSH					param->p[3]
#define ETA_CORE				param->p[4]
#define ETA_SHELL				param->p[5]
#define ETA_BRUSH				param->p[6]
#define ETA_SOLV				param->p[7]
#define ETA_MAG_CORE			param->p[8]
#define ETA_MAG_SHELL			param->p[9]
#define R_AV					param->p[10]
#define ALPHA					param->p[11]
#define RG						param->p[12]
#define PSIDEG					param->p[13]
#define POL						param->p[14]

#define P						param->p[15]
#define A						param->p[16]
#define TAU						param->p[17]

#define T_BRUSH_CONST			param->p[15]
#define SIGMA_BRUSH_GAUSSIAN	param->p[16]
#define L_B						param->p[17]

#define PSI						param->p[MAXPAR-1]
#define NAGG					param->p[MAXPAR-2]
#define RW_SAW					param->p[MAXPAR-3]
#define RADAVG					param->p[MAXPAR-4]
#define R_CORE					param->p[MAXPAR-5]
#define T_SH					param->p[MAXPAR-6]

#define I						param->p[MAXPAR-7]
#define C						param->p[MAXPAR-8]
#define LAMBDA					param->p[MAXPAR-9]
#define R02						param->p[MAXPAR-10]
#define ALPHA_SH				param->p[MAXPAR-11]
#define ZMAX					param->p[MAXPAR-12]
#define N						VBRUSH/gsl_pow_3(A)

#define Q						param->p[MAXPAR-13]
#endif // end of file

