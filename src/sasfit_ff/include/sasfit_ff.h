/*
 * src/sasfit_ff/include/sasfit_ff.h
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#ifndef SASFIT_FF_H
#define SASFIT_FF_H

#include <sasfit_common.h>
#include "sasfit_ff_shared_exports.h"

/**
 * \file sasfit_ff.h
 * \ingroup form_fac
 * Functions which are not (yet) available as plugins are declared here.
 */

/**
 * \defgroup spheres_shells sphere & spherical shells
 * \ingroup form_fac
 *
 * This group contains scattering functions for shperical objects
 *
 */

/**
 * \defgroup ellipsoidal_obj ellipsoidal obj.
 * \ingroup form_fac
 *
 * This group contains scattering functions for ellipsoidal objects
 *
 */

/**
 * \defgroup polymers_micelles polymers & micelles
 * \ingroup form_fac
 *
 * This group contains scattering functions for polymers and micelles objects
 *
 */

/**
 * \defgroup polymers_chains polymer chains
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for polymers chains
 *
 */

/**
 * \defgroup polymers_stars polymer stars
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for starlike structures
 *
 */

/**
 * \defgroup polymers_rings polymer rings
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for ringlike structures
 *
 */

/**
 * \defgroup polymers_sp_and_ell_micelles spherical & ellipsoidal micelles
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for spherical & ellipsoidal micelles
 *
 */

/**
 * \defgroup polymers_cyl_and_rod_micelles cylindrical & rod-like micelles
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for cylindrical & rod-like micelles
 *
 */

/**
 * \defgroup polymers_local_planar_micelles local planar micelles (sheets, ULV)
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for local planar micelles (sheets, ULV)
 *
 */

/**
 * \defgroup polymers_wormlike wormlike structures
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for wormlike structures
 *
 */


/**
 * \defgroup polymers_nonparticular non-particular structures
 * \ingroup polymers_micelles
 *
 * This group contains scattering functions for non-particular structures
 *
 */

/**
 * \defgroup cluster_obj clusters
 * \ingroup form_fac
 *
 * This group contains scattering functions for clusters
 *
 */

/**
 * \defgroup cylindrical_obj cylindrical obj.
 * \ingroup form_fac
 *
 * This group contains scattering functions for cylindrical objects
 *
 */

#include "../psi_a_b/include/public.h"

sasfit_ff_DLLEXP scalar sasfit_ff_alignedCylShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_partly_aligned_CylShell(scalar x, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Andrea1(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_BenoitStar(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_BiLayeredVesicle(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_BiLayeredVesicle_v(scalar q, sasfit_param * param, int distr);
sasfit_ff_DLLEXP scalar sasfit_ff_BiLayerGauss(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_BlockCopolymerMicelle(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_BroadPeak(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_CoPolymerMicelleSphericalProfile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Cylinder(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_cyl_mic(scalar q, sasfit_param * param);

#include "../cyl_rwbrush/include/public.h"

sasfit_ff_DLLEXP scalar sasfit_ff_DAB(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_DaisyLikeRing(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Disc_f(scalar q, sasfit_param * param);

#include "../disc_rwbrush/include/public.h"

sasfit_ff_DLLEXP scalar sasfit_ff_DLCAggregation(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_DLS_Sphere_RDG(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_DoubleShellChain(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_DoubleShell_withSD(scalar q, sasfit_param * param);

sasfit_ff_DLLEXP scalar sasfit_ff_dumbbell_shell(scalar q, sasfit_param * param);

#include "../ellip_rwbrush/include/public.h"
#include "../ellip_core_shell/include/public.h"

#include "../ellip/include/public.h"
sasfit_ff_DLLEXP scalar sasfit_ff_ExpShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_FlexibleRingPolymer(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_francois(scalar q, sasfit_param * param);

sasfit_ff_DLLEXP scalar sasfit_ff_HardSphere(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_d_deta_HardSphere(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_hysteresesFFpsiStrobo(scalar q, sasfit_param * param);

#include "../magnetic_shell/include/public.h"

sasfit_ff_DLLEXP scalar sasfit_ff_Isuperparamagnetic(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_IsuperparamagneticFFpsi(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_IsuperparamagneticFFpsiStrobo(scalar q, sasfit_param * param);

#include "../superparamagnetic_ff_psi/include/public.h"

sasfit_ff_DLLEXP scalar sasfit_ff_IsuperparamagneticFFpsiTISANE1(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_JuelichCoreShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_JuelichDoubleShell(scalar q, sasfit_param * param);

/**
 * \defgroup ff_sphere Sphere
 * \ingroup spheres_shells
 *
 * \brief Scattering function for a sphere
 *
 *
 * \image html form_factor/spherical/sphere.png "Sphere with diameter 2R"
 * \image latex form_factor/spherical/sphere.eps "Sphere with diameter 2R" width=10cm
 *
 * \f[
 *		I_\textbf{Sphere}(Q,R) = K^2(Q,R,\Delta\eta)
 * \f]
 * with
 * \f[
 *	 K(Q,R,\Delta\eta) = \frac{4}{3}\pi R^3 \Delta\eta \, 3 \frac{\sin QR - QR \cos QR}{(QR)^3}
 * \f]
 * The forward scattering for \f$Q=0\f$ is given by
 * \f[
 *	\displaystyle \lim_{Q=0}I_\textbf{Sphere}(Q,R) =\left( \frac{4}{3}\pi R^3 \Delta\eta \right)^2
 * \f]
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R </td>
 *       <td>radius of sphere</td>
 *      </tr><tr>
 *       <td>\b --- </td>
 *       <td> not used </td>
 *      </tr><tr>
 *       <td>\b --- </td>
 *       <td> not used </td>
 *      </tr><tr>
 *       <td>\b eta </td>
 *       <td> scattering length density contrast \f$\Delta\eta\f$</td>
 *      </tr></table>
 *
 * \note The parameters param.p[1] and param.p[2] are not used.
 *
 * \image html form_factor/spherical/sphere_P.png "Scattering function of spheres with radii 10 and 20nm. The scattering length density contrast is set to 1."
 * \image latex form_factor/spherical/sphere_P.eps "Scattering function of spheres with radii 10 and 20nm. The scattering length density contrast is set to 1." width=10cm
 */

/**
 * \ingroup ff_sphere
 *
 * \sa sasfit_ff_sphere.c, form_fac
 */
sasfit_ff_DLLEXP scalar sasfit_ff_sphere(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_sphere_f(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_sphere_v(scalar q, sasfit_param * param, int distr);

/**
 * \defgroup ff_spherical_shells Spherical Shells
 * \ingroup spheres_shells
 *
 * \brief Scattering function for spherical shells
 *
 * Three different parametrisations of spherical shells have been implemented
 * \ref shell_i "\c Spherical \c Shell \c i",
 * \ref shell_ii "\c Spherical \c Shell \c ii"
 * and \ref shell_iii "\c Spherical \c Shell \c iii".
 *
 * \section shell_i Spherical Shell i
 *
 * \image html form_factor/spherical/shell1.png "Spherical shell i"
 * \image latex form_factor/spherical/shell1.eps "Spherical shell i"
 *
 * This implementation of a spherical shell is parametrised with an inner radius \f$ R_2 \f$ and outer
 * radius \f$ R_1 \f$. The scattering contrast relative to the matrix of the core is \f$ \mu \Delta \eta \f$
 * and the one of the shell \f$\Delta\eta\f$.
 * \f[
 *		I_{Shell1}(Q,R_1,R_2,\Delta\eta,\mu)=
 *			\left[K(Q,R_1,\Delta\eta)-K(Q,R_2,\Delta\eta(1-\mu))\right]^2
 * \f]
 * with
 * \f[
 *	 K(Q,R,\Delta\eta) = \frac{4}{3}\pi R^3 \Delta\eta \, 3 \frac{\sin QR - QR \cos QR}{(QR)^3}
 * \f]
 * The forward scattering for \f$Q=0\f$ is given by
 * \f[
 *   \lim_{Q=0}I_{Shell1}(Q,R_1,R_2,\Delta\eta,\mu) =
 *   \left(\frac{4}{3}\pi \Delta\eta \left[ R_1^3 - R_2^3(1-\mu)\right]\right)^2
 * \f]
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R1 </td>
 *       <td>overall radius of spherical shell \f$R_1\f$</td>
 *      </tr><tr>
 *       <td>\b R2 </td>
 *       <td> radius of core \f$R_2\f$ </td>
 *      </tr><tr>
 *       <td>\b eta </td>
 *       <td> scattering length density difference between shell and matrix \f$\Delta\eta\f$ </td>
 *      </tr><tr>
 *       <td>\b mu </td>
 *       <td> scattering length density difference between core and matrix relative to the shell contrast \f$\mu\f$</td>
 *      </tr></table>
 *
 * \note This formfactor is selected by setting the variable \c param->kernelSelector  to \c SPH_SHELL1
 * in the argument of the calling function.
 *
 * \image html form_factor/spherical/shell_i_P.png "Scattering intensity of spherical shell with outer radius of R1=14nm and inner radius of R2=11.2nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2."
 * \image latex form_factor/spherical/shell_i_P.eps "Scattering intensity of spherical shell with outer radius of R1=14nm and inner radius of R2=11.2nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2." width=10cm
 *
 *
 * \section shell_ii Spherical Shell ii
 *
 * \image html form_factor/spherical/shell2.png "Spherical shell ii"
 * \image latex form_factor/spherical/shell2.eps "Spherical shell ii"
 *
 * This implementation of a spherical shell is parametrised with an outer radius \f$ R \f$ and an inner
 * radius \f$ \nu R \f$. The scattering contrast relative to the matrix of the core is \f$ \mu \Delta \eta \f$
 * and the one of the shell \f$\Delta\eta\f$.
 * \f[
 *		I_{Shell2}(Q,R,\nu,\Delta\eta,\mu)=
 *            \left(K(Q,R,\Delta\eta)-K(Q,\nu R,\Delta\eta(1-\mu))\right)^2
 * \f]
 * with
 * \f[
 *	 K(Q,R,\Delta\eta) = \frac{4}{3}\pi R^3 \Delta\eta \, 3 \frac{\sin QR - QR \cos QR}{(QR)^3}
 * \f]
 * The forward scattering for \f$Q=0\f$ is given by
 * \f[
 *   \lim_{Q=0}I_{Shell2}(Q,R,R,\Delta\eta,\mu) =
 *          \left(\frac{4}{3}\pi \Delta\eta \left[ R^3 - \nu^3
 *                 R^3(1-\mu)\right]\right)^2
 * \f]
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R </td>
 *       <td>overall radius of spherical shell \f$R\f$</td>
 *      </tr><tr>
 *       <td>\b nu </td>
 *       <td> the radius of the core is only the fraction \f$\nu\f$ of the overall radius  \f$R\f$ </td>
 *      </tr><tr>
 *       <td>\b eta </td>
 *       <td> scattering length density difference between shell and matrix \f$\Delta\eta\f$ </td>
 *      </tr><tr>
 *       <td>\b mu </td>
 *       <td> scattering length density difference between core and matrix relative to the shell contrast \f$\mu\f$</td>
 *      </tr></table>
 *
 * \note This formfactor is selected by setting the variable \c param->kernelSelector  to \c SPH_SHELL2
 * in the argument of the calling function.
 *
 * \image html form_factor/spherical/shell_ii_P.png "Scattering intensity of spherical shell with outer radius of R=14nm and inner radius of nu R=11.2nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2."
 * \image latex form_factor/spherical/shell_ii_P.eps "Scattering intensity of spherical shell with outer radius of R=14nm and inner radius of nu R=11.2nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2." width=10cm
 *
 *
 * \section shell_iii Spherical Shell iii
 *
 * \image html form_factor/spherical/shell3.png "Spherical shell iii"
 * \image latex form_factor/spherical/shell3.eps "Spherical shell iii"
 *
 * This implementation of a spherical shell is parametrised with an inner radius \f$R\f$ and a shell
 * thickness \f$\Delta R\f$. The scattering contrast relative to the matrix of the core is \f$ \Delta \eta_1\f$
 * and the one of the shell \f$\Delta\eta_2\f$.
 * \f[
 *	 I_{Shell3}(Q,R,\Delta R,\Delta\eta_1,\Delta\eta_2)=
 *       \left[K(Q,R+\Delta R,\Delta\eta_2)-K(Q,R,\Delta\eta_2-\Delta\eta_1)\right]^2
 * \f]
 * with
 * \f[
 *	 K(Q,R,\Delta\eta) = \frac{4}{3}\pi R^3 \Delta\eta \, 3 \frac{\sin QR - QR \cos QR}{(QR)^3}
 * \f]
 * The forward scattering for \f$Q=0\f$ is given by
 * \f[
 *  \lim_{Q=0}I_{Shell3}(Q,R,\Delta R,\Delta\eta_1,\Delta\eta_2)
 *    = \left(\frac{4}{3}\pi \left[(R+\Delta R)^3\Delta\eta_2
 *                                - R^3(\Delta\eta_2-\Delta\eta_1)\right]\right)^2
 * \f]
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b R </td>
 *       <td>overall radius of spherical shell \f$R\f$</td>
 *      </tr><tr>
 *       <td>\b dR </td>
 *       <td> tthickness of the shell \f$\Delta R\f$ </td>
 *      </tr><tr>
 *       <td>\b eta1 </td>
 *       <td> scattering length density difference between core and matrix \f$\Delta\eta_1\f$ </td>
 *      </tr><tr>
 *       <td>\b eta2 </td>
 *       <td> scattering length density difference between shell and matrix \f$\Delta\eta_2\f$</td>
 *      </tr></table>
 *
 * \note This formfactor is selected by setting the variable \c param->kernelSelector  to \c SPH_SHELL3
 * in the argument of the calling function.
 *
 * \image html form_factor/spherical/shell_iii_P.png "Scattering intensity of spherical shell with core radius of R=11.2nm and shell thickness of dR=2.8nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2."
 * \image latex form_factor/spherical/shell_iii_P.eps "Scattering intensity of spherical shell with core radius of R=11.2nm and shell thickness of dR=2.8nm. The scattering length density contrast the shell is set to 1 and the one of the core to -1, -0.5, and 2." width=10cm
 */

/**
 * \ingroup ff_spherical_shells
 *
 * \sa sasfit_ff_spherical_shell.c, form_fac
 */
sasfit_ff_DLLEXP scalar sasfit_ff_spherical_shell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_spherical_shell_v(scalar q, sasfit_param * param, int distr);


sasfit_ff_DLLEXP scalar sasfit_ff_KholodenkoWorm(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Ksh(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_KShellcompr(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_KshExp(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Kshlin(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_LangevinMH(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_LayeredCentroSymmetricXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_LinShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_long_cyl(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MassFractExp(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MassFractExp_mpow_x_a(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MassFractGauss(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MassFractOverlappingSpheres(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Mie(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MieShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MLVesicleFrielinghaus(scalar x, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_mMemberedTwistedRing(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_MultiLamellarVesicle(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_OrnsteinZernike(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_P39(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_PolydisperseStar(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_RLCAggregation(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_RNDMultiLamellarVesicle2(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_RNDMultiLamellarVesicle(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Robertus1(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Robertus2(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Robertus3(scalar q, sasfit_param * param);

//#include "../rod/include/sasfit_plugin_rod.h"

sasfit_ff_DLLEXP scalar sasfit_ff_Rod_Exp_Profile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Rod_Gauss_Profile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Rodlike_BlockCopolymerMicelle(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Rod_R_ma_Profile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Rod_R_ma_Profile_v(scalar q, sasfit_param * param, int distr);
sasfit_ff_DLLEXP scalar sasfit_ff_Rod_RWbrush(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_shearedCylinder(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Shellcompr(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_Exp1(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_Exp2(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_Exp(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_Exp_Profile_Rc(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_R_ma_Profile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_R_ma_Profile_v(scalar q, sasfit_param * param, int distr);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_R_ma_Profile_Manuela(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_old_Sphere_RWbrush(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Sphere_SAWbrush(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_SphereWithGaussChains(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Spinodal(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_StackDiscs(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_StickyHardSphere(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_TetrahedronDoubleShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_TeubnerStrey(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinCylShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinCylShell_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinDisc_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinEllShell_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinEllShell_S_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_ThinSphShell_homogenousXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Torus(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_triaxEllShell1(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_triaxEllShell1_v(scalar q, sasfit_param * param, int distr);
sasfit_ff_DLLEXP scalar sasfit_ff_TripleLayeredHomogeneousCentroSymmetricXS(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_two_attached_spheres(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_two_attached_spheres_f(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_TwoInfinitelyThinPlates(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_UnifiedExponetialPowerLaw2(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_UnifiedExponetialPowerLaw(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_VeryLongCylindricalShell(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_very_long_cyl_w_gauss_chains(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_WormLikeChainEXV(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Worm_R_ma_Profile(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Worm_RWbrush(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_worm_w_gauss_chains(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_background(scalar q, sasfit_param * param);

sasfit_ff_DLLEXP scalar sasfit_ff_Pcs_homogeneousPlanar(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Pcs_homogeneousCyl(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_Pcs_ellCylSh(scalar q, sasfit_param * param);


sasfit_ff_DLLEXP scalar sasfit_ff_aux_p_r(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_aux_gamma_r(scalar q, sasfit_param * param);

#endif

