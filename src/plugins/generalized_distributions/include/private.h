/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef GENERALIZED_DISTRIBUTIONS_PRIVATE_H
#define GENERALIZED_DISTRIBUTIONS_PRIVATE_H

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
#include "sasfit_generalized_distributions.h"

//
// add local defines here:
// #define P0 param->p[0]
//
int i4_abs ( int i );
int i4_mach ( int i );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
int i4_pow ( int i, int j );
float r4_abs ( float x );
float r4_acos ( float x );
float r4_acosh ( float x );
void r4_admp ( float x, float *ampl, float *phi );
float r4_ai ( float x );
float r4_aid ( float x );
float r4_aide ( float x );
float r4_aie ( float x );
void r4_aimp ( float x, float *ampl, float *theta );
float r4_aint ( float x );
float r4_asin ( float x );
float r4_asinh ( float x );
float r4_atan ( float x );
float r4_atan2 ( float sn, float cs );
float r4_atanh ( float x );
float r4_besi0 ( float x );
float r4_besi0e ( float x );
float r4_besi1 ( float x );
float r4_besi1e ( float x );
float r4_besj0 ( float x );
float r4_besj1 ( float x );
float r4_besk ( float nu, float x );
float r4_besk0 ( float x );
float r4_besk0e ( float x );
float r4_besk1 ( float x );
float r4_besk1e ( float x );
float *r4_beskes ( float xnu, float x, int nin );
float *r4_besks ( float xnu, float x, int nin );
float r4_besy0 ( float x );
float r4_besy1 ( float x );
float r4_beta ( float a, float b );
float r4_betai ( float x, float pin, float qin );
float r4_betai ( float x, float pin, float qin );
float r4_bi ( float x );
float r4_bid ( float x );
float r4_bide ( float x );
float r4_bie ( float x );
float r4_binom ( int n, int m );
float r4_cbrt ( float x );
float r4_chi ( float x );
float r4_chu ( float a, float b, float x );
float r4_chu_scaled ( float a, float b, float z );
float r4_ci ( float x );
float r4_cin ( float x );
float r4_cinh ( float x );
float r4_cos ( float x );
float r4_cos_deg ( float x );
float r4_cosh ( float x );
float r4_cot ( float x );
float r4_csevl ( float x, float a[], int n );
float r4_dawson ( float x );
float r4_e1 ( float x );
float r4_ei ( float x );
float r4_erf ( float x );
float r4_erfc ( float x );
float r4_exp ( float x );
float r4_exprel ( float x );
float r4_fac ( int n );
float r4_gami ( float a, float x );
float r4_gamic ( float a, float x );
float r4_gamit ( float a, float x );
void r4_gaml ( float *xmin, float *xmax );
float r4_gamma ( float x );
float r4_gamr ( float x );
float r4_gmic ( float a, float x, float alx );
float r4_gmit ( float a, float x, float algap1, float sgngam, float alx );
int r4_inits ( float dos[], int nos, float eta );
float r4_int ( float x );
void r4_knus ( float xnu, float x, float *bknu, float *bknu1, int *iswtch );
float r4_lbeta ( float a, float b );
void r4_lgams ( float x, float *algam, float *sgngam );
float r4_lgic ( float a, float x, float alx );
float r4_lgit ( float a, float x, float algap1 );
float r4_lgmc ( float x );
float r4_li ( float x );
float r4_lngam ( float x );
float r4_lnrel ( float x );
float r4_log ( float x );
float r4_log10 ( float x );
float r4_mach ( int i );
void r4_machar ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
  long int *machep, long int *negep, long int *iexp, long int *minexp,
  long int *maxexp, float *eps, float *epsneg, float *xmin, float *xmax );
float r4_max ( float x, float y );
float r4_min ( float x, float y );
float r4_mod ( float x, float y );
float r4_mop ( int i );
float r4_pak ( float y, int n );
float r4_poch ( float a, float x );
float r4_poch1 ( float a, float x );
float r4_power ( float a, float b );
float r4_psi ( float x );
float r4_rand ( float r );
float r4_randgs ( float xmean, float sd, int *seed );
float r4_random ( int n, float t[] );
float r4_ren ( int *seed );
float r4_shi ( float x );
float r4_sign ( float x );
float r4_si ( float x );
void r4_sifg ( float x, float *f, float *g );
float r4_sin ( float x );
float r4_sin_deg ( float x );
float r4_sinh ( float x );
float r4_spence ( float x );
float r4_sqrt ( float x );
float r4_tan ( float x );
float r4_tanh ( float x );
void r4_upak ( float x, float *y, int *n );
double r8_abs ( double x );
double r8_acos ( double x );
double r8_acosh ( double x );
void r8_admp ( double x, double *ampl, double *phi );
double r8_ai ( double x );
double r8_aid ( double x );
double r8_aide ( double x );
double r8_aie ( double x );
void r8_aimp ( double x, double *ampl, double *theta );
double r8_aint ( double x );
double r8_asin ( double x );
double r8_asinh ( double x );
double r8_atan ( double x );
double r8_atan2 ( double sn, double cs );
double r8_atanh ( double x );
void r8_b0mp ( double x, double *ampl, double *theta );
void r8_b1mp ( double x, double *ampl, double *theta );
double r8_besi0 ( double x );
double r8_besi0e ( double x );
double r8_besi1 ( double x );
double r8_besi1e ( double x );
double r8_besj0 ( double x );
double r8_besj1 ( double x );
double r8_besk ( double nu, double x );
double r8_besk0 ( double x );
double r8_besk0e ( double x );
double r8_besk1 ( double x );
double r8_besk1e ( double x );
double *r8_beskes ( double xnu, double x, int nin );
double *r8_besks ( double xnu, double x, int nin );
double r8_besy0 ( double x );
double r8_besy1 ( double x );
double r8_beta ( double a, double b );
double r8_betai ( double x, double pin, double qin );
double r8_bi ( double x );
double r8_bid ( double x );
double r8_bide ( double x );
double r8_bie ( double x );
double r8_binom ( int n, int m );
double r8_cbrt ( double x );
double r8_chi ( double x );
double r8_chu ( double a, double b, double x );
double r8_chu_scaled ( double a, double b, double z );
double r8_ci ( double x );
double r8_cin ( double x );
double r8_cinh ( double x );
double r8_cos ( double x );
double r8_cos_deg ( double x );
double r8_cosh ( double x );
double r8_cot ( double x );
double r8_csevl ( double x, double a[], int n );
double r8_dawson ( double x );
double r8_e1 ( double x );
double r8_ei ( double x );
double r8_erf ( double x );
double r8_erfc ( double x );
double r8_exp ( double x );
double r8_exprel ( double x );
double r8_fac ( int n );
double r8_gami ( double a, double x );
double r8_gamic ( double a, double x );
double r8_gamit ( double a, double x );
void r8_gaml ( double *xmin, double *xmax );
double r8_gamma ( double x );
double r8_gamr ( double x );
double r8_gmic ( double a, double x, double alx );
double r8_gmit ( double a, double x, double algap1, double sgngam, double alx );
int r8_inits ( double dos[], int nos, double eta );
double r8_int ( double x );
void r8_knus ( double xnu, double x, double *bknu, double *bknu1, int *iswtch );
double r8_lbeta ( double a, double b );
void r8_lgams ( double x, double *algam, double *sgngam );
double r8_lgic ( double a, double x, double alx );
double r8_lgit ( double a, double x, double algap1 );
double r8_lgmc ( double x );
double r8_li ( double x );
double r8_lngam ( double x );
double r8_lnrel ( double x );
double r8_log ( double x );
double r8_log10 ( double x );
double r8_mach ( int i );
void r8_machar ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
  long int *machep, long int *negep, long int *iexp, long int *minexp,
  long int *maxexp, double *eps, double *epsneg, double *xmin, double *xmax );
double r8_max ( double x, double y );
double r8_min ( double x, double y );
double r8_mod ( double x, double y );
double r8_mop ( int i );
double r8_pak ( double y, int n );
double r8_poch ( double a, double x );
double r8_poch1 ( double a, double x );
double r8_power ( double a, double b );
double r8_psi ( double x );
double r8_rand ( double r, int *ix0, int *ix1 );
double r8_randgs ( double xmean, double sd, int *seed );
double r8_random ( int n, double t[], int *ix0, int *ix1 );
void r8_random_init ( int n, double t[], int *ix0, int *ix1 );
double r8_ren ( int *seed );
double r8_shi ( double x );
double r8_si ( double x );
void r8_sifg ( double x, double *f, double *g );
double r8_sign ( double x );
double r8_sin ( double x );
double r8_sin_deg ( double x );
double r8_sinh ( double x );
double r8_spence ( double x );
double r8_sqrt ( double x );
double r8_tan ( double x );
double r8_tanh ( double x );
void r8_upak ( double x, double *y, int *n );
#endif // end of file

