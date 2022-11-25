/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef CLUSTERYUKAWA_PRIVATE_H
#define CLUSTERYUKAWA_PRIVATE_H

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

  // use lookup table for sq_cluster__linear_chain_(q, param)
  #define sasfit_sq_cluster__linear_chain_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func((q),(p))
  #define sasfit_sq_cluster__linear_chain__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[0].func_f((q),(p))
  #define sasfit_sq_cluster__linear_chain__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[0].func_v((q),(p),(d))

  // use lookup table for sq_cluster__tetrahedron_(q, param)
  #define sasfit_sq_cluster__tetrahedron_(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func((q),(p))
  #define sasfit_sq_cluster__tetrahedron__f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[1].func_f((q),(p))
  #define sasfit_sq_cluster__tetrahedron__v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[1].func_v((q),(p),(d))

  // use lookup table for sq_dlca_incl_nn_pertubation(q, param)
  #define sasfit_sq_dlca_incl_nn_pertubation(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func((q),(p))
  #define sasfit_sq_dlca_incl_nn_pertubation_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[2].func_f((q),(p))
  #define sasfit_sq_dlca_incl_nn_pertubation_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[2].func_v((q),(p),(d))

  // use lookup table for sq_freely_joined_chain_of_rods(q, param)
  #define sasfit_sq_freely_joined_chain_of_rods(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func((q),(p))
  #define sasfit_sq_freely_joined_chain_of_rods_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[3].func_f((q),(p))
  #define sasfit_sq_freely_joined_chain_of_rods_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[3].func_v((q),(p),(d))

  // use lookup table for sq_hcp_iso_gaussian(q, param)
  #define sasfit_sq_hcp_iso_gaussian(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func((q),(p))
  #define sasfit_sq_hcp_iso_gaussian_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[4].func_f((q),(p))
  #define sasfit_sq_hcp_iso_gaussian_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[4].func_v((q),(p),(d))

  // use lookup table for sq_hcp_iso_lorentzian(q, param)
  #define sasfit_sq_hcp_iso_lorentzian(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func((q),(p))
  #define sasfit_sq_hcp_iso_lorentzian_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[5].func_f((q),(p))
  #define sasfit_sq_hcp_iso_lorentzian_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[5].func_v((q),(p),(d))

  // use lookup table for sq_hcp_iso_pearson(q, param)
  #define sasfit_sq_hcp_iso_pearson(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func((q),(p))
  #define sasfit_sq_hcp_iso_pearson_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[6].func_f((q),(p))
  #define sasfit_sq_hcp_iso_pearson_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[6].func_v((q),(p),(d))

  // use lookup table for sq_macro_ion_rmsa_1(q, param)
  #define sasfit_sq_macro_ion_rmsa_1(q,p) SASFIT_PLUGIN_IMPORTS()->functions[7].func((q),(p))
  #define sasfit_sq_macro_ion_rmsa_1_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[7].func_f((q),(p))
  #define sasfit_sq_macro_ion_rmsa_1_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[7].func_v((q),(p),(d))

  // use lookup table for sq_macro_ion_rmsa_2(q, param)
  #define sasfit_sq_macro_ion_rmsa_2(q,p) SASFIT_PLUGIN_IMPORTS()->functions[8].func((q),(p))
  #define sasfit_sq_macro_ion_rmsa_2_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[8].func_f((q),(p))
  #define sasfit_sq_macro_ion_rmsa_2_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[8].func_v((q),(p),(d))

  // use lookup table for sq_macro_ion_rmsa_3(q, param)
  #define sasfit_sq_macro_ion_rmsa_3(q,p) SASFIT_PLUGIN_IMPORTS()->functions[9].func((q),(p))
  #define sasfit_sq_macro_ion_rmsa_3_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[9].func_f((q),(p))
  #define sasfit_sq_macro_ion_rmsa_3_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[9].func_v((q),(p),(d))

  // use lookup table for sq_one_d_hard_spheres_random_orient(q, param)
  #define sasfit_sq_one_d_hard_spheres_random_orient(q,p) SASFIT_PLUGIN_IMPORTS()->functions[10].func((q),(p))
  #define sasfit_sq_one_d_hard_spheres_random_orient_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[10].func_f((q),(p))
  #define sasfit_sq_one_d_hard_spheres_random_orient_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[10].func_v((q),(p),(d))

  // use lookup table for sq_paracrystal_1d_random(q, param)
  #define sasfit_sq_paracrystal_1d_random(q,p) SASFIT_PLUGIN_IMPORTS()->functions[11].func((q),(p))
  #define sasfit_sq_paracrystal_1d_random_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[11].func_f((q),(p))
  #define sasfit_sq_paracrystal_1d_random_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[11].func_v((q),(p),(d))

  // use lookup table for sq_paracrystal_orig_1d_random(q, param)
  #define sasfit_sq_paracrystal_orig_1d_random(q,p) SASFIT_PLUGIN_IMPORTS()->functions[12].func((q),(p))
  #define sasfit_sq_paracrystal_orig_1d_random_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[12].func_f((q),(p))
  #define sasfit_sq_paracrystal_orig_1d_random_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[12].func_v((q),(p),(d))

  // use lookup table for sq_pc_random_flight(q, param)
  #define sasfit_sq_pc_random_flight(q,p) SASFIT_PLUGIN_IMPORTS()->functions[13].func((q),(p))
  #define sasfit_sq_pc_random_flight_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[13].func_f((q),(p))
  #define sasfit_sq_pc_random_flight_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[13].func_v((q),(p),(d))

  // use lookup table for sq_simple_sq_power_law(q, param)
  #define sasfit_sq_simple_sq_power_law(q,p) SASFIT_PLUGIN_IMPORTS()->functions[14].func((q),(p))
  #define sasfit_sq_simple_sq_power_law_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[14].func_f((q),(p))
  #define sasfit_sq_simple_sq_power_law_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[14].func_v((q),(p),(d))

  // use lookup table for sq_two_yukawa(q, param)
  #define sasfit_sq_two_yukawa(q,p) SASFIT_PLUGIN_IMPORTS()->functions[15].func((q),(p))
  #define sasfit_sq_two_yukawa_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[15].func_f((q),(p))
  #define sasfit_sq_two_yukawa_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[15].func_v((q),(p),(d))

  // use lookup table for sq_one_yukawa(q, param)
  #define sasfit_sq_one_yukawa(q,p) SASFIT_PLUGIN_IMPORTS()->functions[16].func((q),(p))
  #define sasfit_sq_one_yukawa_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions[16].func_f((q),(p))
  #define sasfit_sq_one_yukawa_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions[16].func_v((q),(p),(d))

#else

  #include <sasfit_simple_clusters.h>
  #include <sasfit_mass_fractal_w_nn_pertub.h>
  #include <sasfit_thin_objects.h>
  #include <sasfit_ordered_particle_systems.h>
  #include <sasfit_macro_ion.h>
  #include <sasfit_hard_spheres_1d.h>
  #include <sasfit_sq_1d.h>
  #include <sasfit_random_flight.h>
  #include <sasfit_powerlawsq.h>
  #include <sasfit_twoyukawa.h>

#endif

// adjust according to the plugins name
#include "sasfit_clusteryukawa.h"

//
// add local defines here:
// #define P0 param->p[0]
//

#endif // end of file

