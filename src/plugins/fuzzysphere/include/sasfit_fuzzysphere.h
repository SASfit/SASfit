/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_PLUGIN_FUZZYSPHERE_H
#define SASFIT_PLUGIN_FUZZYSPHERE_H

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_fuzzysphere.h
 * Public available functions and descriptions of the fuzzysphere plugin.
 */

/**
 * \def sasfit_fuzzysphere_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_fuzzysphere_EXPORTS)
	#ifdef sasfit_fuzzysphere_DLLEXP
	#undef sasfit_fuzzysphere_DLLEXP
	#endif
	#define sasfit_fuzzysphere_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_fuzzysphere_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_fuzzysphere_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style

/* ################ start ff_fuzzysphere ################ */
/** 
 * \defgroup ff_fuzzysphere FuzzySphere
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some scattering from spherical particles with a "fuzzy" interface function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of the fuzzy sphere</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>thickness of the fuzzy shell</td>
 *      </tr><tr>
 *       <td>\b eta_sph</td>
 *       <td>scattering length density of sphere</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of the solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_fuzzysphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_fuzzysphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_fuzzysphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_fuzzysphere ################ */

/* ################ start ff_radial_profile_of_fuzzysphere ################ */
/** 
 * \defgroup ff_radial_profile_of_fuzzysphere radial profile of FuzzySphere
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of Radial Profile from a spherical particles with a "fuzzy" interface\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R</td>
 *       <td>radius of the fuzzy sphere</td>
 *      </tr><tr>
 *       <td>\b sigma</td>
 *       <td>thickness of the fuzzy shell</td>
 *      </tr><tr>
 *       <td>\b eta_sph</td>
 *       <td>scattering length density of sphere</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_radial_profile_of_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_fuzzysphere(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_fuzzysphere_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_fuzzysphere
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_fuzzysphere_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_radial_profile_of_fuzzysphere ################ */

/* ################ start ff_coreshellmicrogel ################ */
/** 
 * \defgroup ff_coreshellmicrogel CoreShellMicrogel
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some scattering from spherical particles with a "fuzzy" interface function\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b W_core</td>
 *       <td>radius of center parts of core  with homogeneous scattering length density</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>interface half width of the core</td>
 *      </tr><tr>
 *       <td>\b W_sh</td>
 *       <td>width of center parts of shell  with homogeneous scattering length density</td>
 *      </tr><tr>
 *       <td>\b sigma_sh,in</td>
 *       <td>half width of the inner interface of shell</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>distance between interface of core and in interface of shell</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>half width of the outer surface profile</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of homogeneous core part</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of homogeneous shell part</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_coreshellmicrogel(scalar q, sasfit_param * p);

/**
 * \ingroup ff_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_coreshellmicrogel_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_coreshellmicrogel_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_coreshellmicrogel ################ */

/* ################ start ff_radial_profile_of_coreshellmicrogel ################ */
/** 
 * \defgroup ff_radial_profile_of_coreshellmicrogel radial profile of CoreShellMicrogel
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief \<some brief description of Radial Profile from a spherical particles with a "fuzzy" interface\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b W_core</td>
 *       <td>radius of center parts of core  with homogeneous scattering length density</td>
 *      </tr><tr>
 *       <td>\b sigma_core</td>
 *       <td>interface half width of the core</td>
 *      </tr><tr>
 *       <td>\b W_shell</td>
 *       <td>width of center parts of shell  with homogeneous scattering length density</td>
 *      </tr><tr>
 *       <td>\b sigma_sh,in</td>
 *       <td>half width of the inner interface of shell</td>
 *      </tr><tr>
 *       <td>\b D</td>
 *       <td>distance between interface of core and in interface of shell</td>
 *      </tr><tr>
 *       <td>\b sigma_out</td>
 *       <td>half width of the outer surface profile</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of homogeneous core part</td>
 *      </tr><tr>
 *       <td>\b eta_shell</td>
 *       <td>scattering length density of homogeneous shell part</td>
 *      </tr><tr>
 *       <td>\b eta_sol</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_radial_profile_of_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_coreshellmicrogel(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_coreshellmicrogel_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_coreshellmicrogel
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_coreshellmicrogel_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_radial_profile_of_coreshellmicrogel ################ */


#endif // this file



/* ################ start ff_expshell ################ */
/** 
 * \defgroup ff_expshell ExpShell
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief ExpShell is a core shell form factor with an expotential dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>factor for exponentail decay of scattering length density in shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_expshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_expshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_expshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_expshell ################ */


/* ################ start ff_radial_profile_of_expshell ################ */
/** 
 * \defgroup ff_radial_profile_of_expshell radial profile of ExpShell
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief radial profile of the from factor ExpShell, which is a core shell form factor with an expotential dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b alpha</td>
 *       <td>factor for exponentail decay of scattering length density in shell</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_radial_profile_of_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_expshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_expshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_expshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_expshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_radial_profile_of_expshell ################ */


/* ################ start ff_linshell ################ */
/** 
 * \defgroup ff_linshell LinShell
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief LinShell is a core shell form factor with a linear dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_linshell ################ */


/* ################ start ff_radial_profile_of_linshell ################ */
/** 
 * \defgroup ff_radial_profile_of_linshell radial profile of LinShell
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief radial profile of LinShell which is a core shell form factor with a linear dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_core</td>
 *       <td>core radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_radial_profile_of_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_linshell
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_radial_profile_of_linshell ################ */


/* ################ start ff_linshell2 ################ */
/** 
 * \defgroup ff_linshell2 LinShell2
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief LinShell2 is a core shell form factor with a linear dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_tot</td>
 *       <td>overall radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_linshell2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_linshell2 ################ */

/* ################ start ff_radial_profile_of_linshell2 ################ */
/** 
 * \defgroup ff_radial_profile_of_linshell2 radial profile of LinShell2
 * \ingroup ff_plugins_fuzzy_sph
 *
 * \brief radial radial profile of LinShell2 which is a core shell form factor with a linear dependent contrast profile of the shell
 *
 * <more detailed documentation, see 'doxygen' docs>
 *
 * \note Default (Size) Distribution: \ref delta
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R_tot</td>
 *       <td>overall radius</td>
 *      </tr><tr>
 *       <td>\b dR</td>
 *       <td>shell thickness</td>
 *      </tr><tr>
 *       <td>\b eta_core</td>
 *       <td>scattering length density of core</td>
 *      </tr><tr>
 *       <td>\b eta_sh</td>
 *       <td>scattering length density of non-swollen shell</td>
 *      </tr><tr>
 *       <td>\b x_in_solv</td>
 *       <td>amount of solvent at a distance r=R_core from the center</td>
 *      </tr><tr>
 *       <td>\b x_out_solv</td>
 *       <td>amount of solvent at a distance r=R_core+dR from the center</td>
 *      </tr><tr>
 *       <td>\b eta_solv</td>
 *       <td>scattering length density of solvent</td>
 *      </tr></table>
 */

/**
 * \ingroup ff_radial_profile_of_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell2(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell2_f(scalar q, sasfit_param * p);

/**
 * \ingroup ff_radial_profile_of_linshell2
 *
 * \sa sasfit_fuzzysphere.h, ff_plugins_fuzzy_sph
 */
sasfit_fuzzysphere_DLLEXP scalar sasfit_ff_radial_profile_of_linshell2_v(scalar q, sasfit_param * p, int dist);
/* ################ stop ff_radial_profile_of_linshell2 ################ */
