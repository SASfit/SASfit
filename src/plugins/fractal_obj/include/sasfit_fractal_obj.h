/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_FRACTAL_OBJ_H
#define SASFIT_PLUGIN_FRACTAL_OBJ_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_fractal_obj.h
 * Public available functions and descriptions of the fractal_obj plugin.
 */

/**
 * \def sasfit_fractal_obj_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_fractal_obj_EXPORTS)
	#ifdef sasfit_fractal_obj_DLLEXP
	#undef sasfit_fractal_obj_DLLEXP
	#endif
	#define sasfit_fractal_obj_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_fractal_obj_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_fractal_obj_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start sq_mass_fractal__exp__x_a__cut_off_ ################ */
/** 
 * \defgroup sq_mass_fractal__exp__x_a__cut_off_ Mass Fractal (Exp(-x^a) Cut-Off)
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of Mass Fractal (Exp(-x^a) Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r0</td>
 *       <td>r0: characteristic dimension of individual scatterers</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>xi: size of fractal aggregate</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>alpha: exponent in cut-off function exp(-x^alpha)</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_mass_fractal__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp__x_a__cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp__x_a__cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp__x_a__cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_mass_fractal__exp__x_a__cut_off_ ################ */

/* ################ start sq_mass_fractal__overapsph_cut_off_ ################ */
/** 
 * \defgroup sq_mass_fractal__overapsph_cut_off_ Mass Fractal (OverapSph Cut-Off)
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of Mass Fractal (OverapSph Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r0</td>
 *       <td>r0: characteristic dimension of individual scatterers</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>xi: size of fractal aggregate</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_mass_fractal__overapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__overapsph_cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__overapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__overapsph_cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__overapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__overapsph_cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_mass_fractal__overapsph_cut_off_ ################ */

/* ################ start sq_mass_fractal__gaussian_cut_off_ ################ */
/** 
 * \defgroup sq_mass_fractal__gaussian_cut_off_ Mass Fractal (Gaussian Cut-Off)
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of Mass Fractal (Gaussian Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r0</td>
 *       <td>r0: characteristic dimension of individual scatterers</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>xi: size of fractal aggregate</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_mass_fractal__gaussian_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__gaussian_cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__gaussian_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__gaussian_cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__gaussian_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__gaussian_cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_mass_fractal__gaussian_cut_off_ ################ */

/* ################ start sq_mass_fractal__exp_cut_off_ ################ */
/** 
 * \defgroup sq_mass_fractal__exp_cut_off_ Mass Fractal (Exp Cut-Off)
 * \ingroup sq_plugins_fractal_objects
 *
 * \brief \<some brief description of Mass Fractal (Exp Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b r0</td>
 *       <td>r0: characteristic dimension of individual scatterers</td>
 *      </tr><tr>
 *       <td>\b xi</td>
 *       <td>xi: size of fractal aggregate</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr></table>
 */

/**
 * \ingroup sq_mass_fractal__exp_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp_cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__exp_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp_cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup sq_mass_fractal__exp_cut_off_
 *
 * \sa sasfit_fractal_obj.h, sq_plugins_fractal_objects
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_sq_mass_fractal__exp_cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop sq_mass_fractal__exp_cut_off_ ################ */

/* ################ start ff_fisher_burford ################ */
/** 
 * \defgroup ff_fisher_burford Fisher-Burford
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of Fisher-Burford function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration (xi^2=Rg^2/3)</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fisher_burford
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_fisher_burford(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fisher_burford
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_fisher_burford_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fisher_burford
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_fisher_burford_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fisher_burford ################ */

/* ################ start ff_massfractexp ################ */
/** 
 * \defgroup ff_massfractexp MassFractExp
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of MassFractExp function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration (xi^2=2Rg^2/(D(D+1)))</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_massfractexp
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractexp(scalar q, sasfit_param * p);

/**
 * \ingroup ff_massfractexp
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractexp_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_massfractexp
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractexp_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_massfractexp ################ */

/* ################ start ff_massfractgauss ################ */
/** 
 * \defgroup ff_massfractgauss MassFractGauss
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of MassFractGauss function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration (xi^2=4Rg^2/D)</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_massfractgauss
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractgauss(scalar q, sasfit_param * p);

/**
 * \ingroup ff_massfractgauss
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractgauss_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_massfractgauss
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_massfractgauss_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_massfractgauss ################ */

/* ################ start ff_aggregate__exp__x_a__cut_off_ ################ */
/** 
 * \defgroup ff_aggregate__exp__x_a__cut_off_ Aggregate (Exp(-x^a) Cut-Off)
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of Aggregate (Exp(-x^a) Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b xi</td>
 *       <td>xi: size of fractal aggregate</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>alpha: exponent in cutoff function\nalpha=1 -> MassFractExp; alpa=2 -> MassFractGauss"</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_aggregate__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__exp__x_a__cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_aggregate__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__exp__x_a__cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_aggregate__exp__x_a__cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__exp__x_a__cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_aggregate__exp__x_a__cut_off_ ################ */

/* ################ start ff_aggregate__overlapsph_cut_off_ ################ */
/** 
 * \defgroup ff_aggregate__overlapsph_cut_off_ Aggregate (OverlapSph Cut-Off)
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of Aggregate (OverlapSph Cut-Off) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration (xi^2=Rg^2(D+2)(D+5)/(2D(D+1))</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_aggregate__overlapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__overlapsph_cut_off_(scalar q, sasfit_param * p);

/**
 * \ingroup ff_aggregate__overlapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__overlapsph_cut_off__f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_aggregate__overlapsph_cut_off_
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_aggregate__overlapsph_cut_off__v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_aggregate__overlapsph_cut_off_ ################ */

/* ################ start ff_dlcaggregation ################ */
/** 
 * \defgroup ff_dlcaggregation DLCAggregation
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of DLCAggregation function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dlcaggregation(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dlcaggregation_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dlcaggregation_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dlcaggregation ################ */

/* ################ start ff_rlcaggregation ################ */
/** 
 * \defgroup ff_rlcaggregation RLCAggregation
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of RLCAggregation function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg</td>
 *       <td>Rg: radius of gyration</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: fractal dimension</td>
 *      </tr><tr>
 *       <td>\b I0</td>
 *       <td>I0: forward scattering</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_rlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_rlcaggregation(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_rlcaggregation_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_rlcaggregation
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_rlcaggregation_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_rlcaggregation ################ */

/* ################ start ff_stackdiscs ################ */
/** 
 * \defgroup ff_stackdiscs StackDiscs
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of StackDiscs function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>R: radius of discs</td>
 *      </tr><tr>
 *       <td>\b t</td>
 *       <td>t: core thickness</td>
 *      </tr><tr>
 *       <td>\b h</td>
 *       <td>h: layer thickness</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>n: number of stacking</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>eta_c: scattering contrast of core</td>
 *      </tr><tr>
 *       <td>\b eta_l</td>
 *       <td>eta_l: scattering contrast of layer</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>sigma: Gaussian standard deviation of stacking parameter D</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>D: (2h+d) next neighbour center-to-center distance</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_stackdiscs
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_stackdiscs(scalar q, sasfit_param * p);

/**
 * \ingroup ff_stackdiscs
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_stackdiscs_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_stackdiscs
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_stackdiscs_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_stackdiscs ################ */

/* ################ start ff_dumbbellshell ################ */
/** 
 * \defgroup ff_dumbbellshell DumbbellShell
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of DumbbellShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>R: core radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>dR: shell thickness</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>L: surface to surface distance\n2(R+dR)+L: center to center distance</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>eta_c: scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>eta_sh: scattering length density of shell</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_dumbbellshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dumbbellshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dumbbellshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dumbbellshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_dumbbellshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_dumbbellshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_dumbbellshell ################ */

/* ################ start ff_two_attached_spheres ################ */
/** 
 * \defgroup ff_two_attached_spheres two_attached_spheres
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of two_attached_spheres function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1</td>
 *       <td>R1: radius of first sphere</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b R2</td>
 *       <td>R2: radius of second sphere</td>
 *      </tr><tr>
 *       <td>\b dummy</td>
 *       <td></td>
 *      </tr><tr>
 *       <td>\b eta_0</td>
 *       <td>eta0: SLD of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>eta1: SLD of first sphere</td>
 *      </tr><tr>
 *       <td>\b eta_2</td>
 *       <td>eta2: SLD of second sphere</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_two_attached_spheres
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres(scalar q, sasfit_param * p);

/**
 * \ingroup ff_two_attached_spheres
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_two_attached_spheres
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_two_attached_spheres ################ */


/* ################ start ff_two_attached_spheres_w_distr ################ */
/** 
 * \defgroup ff_two_attached_spheres_w_distr two_attached_spheres(distr)
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of two_attached_spheres(distr) function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1</td>
 *       <td>R1: radius of first sphere</td>
 *      </tr><tr>
 *       <td>\b sigma1</td>
 *       <td>width of lognormal distribution for R1</td>
 *      </tr><tr>
 *       <td>\b R2</td>
 *       <td>R2: radius of second sphere</td>
 *      </tr><tr>
 *       <td>\b sigma2</td>
 *       <td>width of lognormal distribution for R2</td>
 *      </tr><tr>
 *       <td>\b eta_0</td>
 *       <td>eta0: SLD of solvent</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>eta1: SLD of first sphere</td>
 *      </tr><tr>
 *       <td>\b eta_2</td>
 *       <td>eta2: SLD of second sphere</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_two_attached_spheres_w_distr
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres_w_distr(scalar q, sasfit_param * p);

/**
 * \ingroup ff_two_attached_spheres_w_distr
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres_w_distr_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_two_attached_spheres_w_distr
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_two_attached_spheres_w_distr_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_two_attached_spheres_w_distr ################ */


/* ################ start ff_doubleshellchain ################ */
/** 
 * \defgroup ff_doubleshellchain DoubleShellChain
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of DoubleShellChain function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_c</td>
 *       <td>R_c: core radius</td>
 *      </tr><tr>
 *       <td>\b DR1</td>
 *       <td>DR1: thickness of inner shell</td>
 *      </tr><tr>
 *       <td>\b DR2</td>
 *       <td>DR2: thickness of outer shell</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>L: surface to surface distance\n2(R_c+DR1+DR2)+L: center to center distance</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>eta_c: scattering length density core</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>eta_1: scattering length density of innet shell</td>
 *      </tr><tr>
 *       <td>\b eta_2</td>
 *       <td>eta_2: scattering length density of outer shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_sol_sh2</td>
 *       <td>x_sol_sh2: amount of solvent in shell 2</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>n: number of double shell spheres in the chain\nn>0</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_doubleshellchain
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_doubleshellchain(scalar q, sasfit_param * p);

/**
 * \ingroup ff_doubleshellchain
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_doubleshellchain_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_doubleshellchain
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_doubleshellchain_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_doubleshellchain ################ */

/* ################ start ff_tetrahedrondoubleshell ################ */
/** 
 * \defgroup ff_tetrahedrondoubleshell TetrahedronDoubleShell
 * \ingroup ff_clusters
 *
 * \brief \<some brief description of TetrahedronDoubleShell function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_c</td>
 *       <td>R_c: core radius</td>
 *      </tr><tr>
 *       <td>\b DR1</td>
 *       <td>DR1: thickness of inner shell</td>
 *      </tr><tr>
 *       <td>\b DR2</td>
 *       <td>DR2: thickness of outer shell</td>
 *      </tr><tr>
 *       <td>\b L</td>
 *       <td>L: surface to surface distance\n2(R_c+DR1+DR2)+L: center to center distance</td>
 *      </tr><tr>
 *       <td>\b eta_c</td>
 *       <td>eta_c: scattering length density core</td>
 *      </tr><tr>
 *       <td>\b eta_1</td>
 *       <td>eta_1: scattering length density of innet shell</td>
 *      </tr><tr>
 *       <td>\b eta_2</td>
 *       <td>eta_2: scattering length density of outer shell</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>eta_sol: scattering length density of solvent</td>
 *      </tr><tr>
 *       <td>\b x_sol_sh2</td>
 *       <td>x_sol_sh2: amount of solvent in shell 2</td>
 *      </tr><tr>
 *       <td>\b n</td>
 *       <td>n: number of double shell spheres in the chain\n 1<=n<=5</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_tetrahedrondoubleshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_tetrahedrondoubleshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedrondoubleshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_tetrahedrondoubleshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_tetrahedrondoubleshell
 *
 * \sa sasfit_fractal_obj.h, ff_clusters
 */
sasfit_fractal_obj_DLLEXP scalar sasfit_ff_tetrahedrondoubleshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_tetrahedrondoubleshell ################ */


#endif // this file

