/*
 * src/sasfit_ff/gauss/include/public.h
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef GAUSS_PUBLIC_H
#define GAUSS_PUBLIC_H


/** 
 * \defgroup ff_gauss1 Gauss
 * \ingroup polymers_chains
 *
 * \brief Flexible polymer chains obeying Gaussian statistics 
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * Flexible polymer chains which are not selfavoiding
 * and obey Gaussian statistics. Debye (1947) has calculated the 
 * form factor of such chains:
 * \f[
 *		I_\textbf{Gauss}(q) = I_0 2\frac{\exp(-u)+u-1}{u^2}
 * \f]
 * with \f$u = q^2R_g^2\f$
 * The forward scattering for \f$Q=0\f$ is \f$I_0\f$
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b I0 </td>
 *       <td> forward scattering for \f$Q=0\f$</td>
 *      </tr></table>
 *
 * \note None.
 *
 * \image html form_factor/gaussian_coils/gaussian_coils.png "Scattering function of Gaussian coils plotted for several radii of gyration."
 * \image latex form_factor/gaussian_coils/gaussian_coils.eps "Scattering function of Gaussian coils plotted for several radii of gyration." width=10cm
 */

/**
 * \ingroup ff_gauss1
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_1(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_gauss2 Gauss2
 * \ingroup polymers_chains
 *
 * \brief Flexible polymer chains obeying Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * Flexible polymer chains which are not selfavoiding
 * and obey Gaussian statistics. Debye (1947) has calculated the 
 * form factor of such chains:
 * \f[
 *		I_\textbf{Gauss}(q) = I_0 2\frac{\exp(-u)+u-1}{u^2}
 * \f]
 * with \f$u = q^2R_g^2\f$
 * The forward scattering for \f$Q=0\f$ of this scattering function has been 
 * parametrized by \f$I_0=(b_p-V\eta_{s})^2\f$
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b b_p </td>
 *       <td> scattering length of polymer \f$b_p\f$ in [cm]</td>
 *      </tr><tr>
 *       <td>\b V </td>
 *       <td> volume of a single polymer molecule\f$V\f$ in [cm<SUP>3</SUP>]</td>
 *      </tr><tr>
 *       <td>\b eta_s </td>
 *       <td> scattering length density of solvent \f$\eta_s\f$ in [cm<SUP>-2</SUP>]</td>
 *      </tr></table>
 *
 * \note None.
 *
 * \image html form_factor/gaussian_coils/gaussian_coils.png "Scattering function of Gaussian coils plotted for several radii of gyration."
 * \image latex form_factor/gaussian_coils/gaussian_coils.eps "Scattering function of Gaussian coils plotted for several radii of gyration." width=10cm
 */

/**
 * \ingroup ff_gauss2
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_2(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_gauss3 Gauss3
 * \ingroup polymers_chains
 *
 * \brief Flexible polymer chains obeying Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * Flexible polymer chains which are not selfavoiding
 * and obey Gaussian statistics. Debye (1947) has calculated the 
 * form factor of such chains:
 * \f[
 *		I_\textbf{Gauss}(q) = I_0 2\frac{\exp(-u)+u-1}{u^2}
 * \f]
 * with \f$u = q^2R_g^2\f$
 * The forward scattering for \f$Q=0\f$ of this scattering function has been 
 * parametrized by \f$I_0=\left(b_p-\frac{M_w}{N_a\rho_p}\eta_{s}\right)^2\f$
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b b_p </td>
 *       <td> scattering length of polymer \f$b_p\f$ in [cm]</td>
 *      </tr><tr>
 *       <td>\b Mw </td>
 *       <td> molecular weight of a polymer molecule \f$M_w\f$ in [g/mol]</td>
 *      </tr><tr>
 *       <td>\b rho_p </td>
 *       <td> mass density of the polymer \f$\rho_p\f$ in [g/cm<SUP>3</SUP>]</td>
 *      </tr><tr>
 *       <td>\b eta_s </td>
 *       <td> scattering length density of solvent \f$\eta_s\f$ in [cm<SUP>-2</SUP>]</td>
 *      </tr></table>
 *
 * \note None.
 *
 * \image html form_factor/gaussian_coils/gaussian_coils.png "Scattering function of Gaussian coils plotted for several radii of gyration."
 * \image latex form_factor/gaussian_coils/gaussian_coils.eps "Scattering function of Gaussian coils plotted for several radii of gyration." width=10cm
 */

/**
 * \ingroup ff_gauss3
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_3(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_gauss_poly GaussPoly
 * \ingroup polymers_chains
 *
 * \brief Polydisperse flexible polymers with Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * Flexible polymer chains which are not selfavoiding
 * and obey Gaussian statistics. 
 * Polydispersity has been included in terms of a Schulz-Zimm mass distribution by
 * Zimm (1948) and Greschner (1973).
 * \f[
 *		I_{GaussPoly}(q) = I_0 2 \frac{\left( 1+U x\right)^{-1/U}+x-1}{(1+U)x^2}
 * \f]
 * with \f$x = q^2R_g^2/(1+2U)\f$ and \f$U = \frac{M_w}{M_n} -1 \f$.
 * The forward scattering for \f$Q=0\f$ of this scattering function is \f$I_0\f$
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b Mw </td>
 *       <td> weight averaged molecular weight of a polymer molecule \f$M_w\f$</td>
 *      </tr><tr>
 *       <td>\b Mn </td>
 *       <td> number averaged molecular weight of a polymer molecule \f$M_n\f$ </td>
 *      </tr><tr>
 *       <td>\b I0 </td>
 *       <td> forward scattering \f$I_0\f$</td>
 *      </tr></table>
 *
 * \note None.
 *
 * \image html form_factor/gaussian_coils/gauss_poly.png "Scattering function of polydisperse Gaussian coil plotted for several ratios of Mw/Mn."
 * \image latex form_factor/gaussian_coils/gauss_poly.eps "Scattering function of polydisperse Gaussian coil plotted for several ratios of Mw/Mn." width=10cm
 */

/**
 * \ingroup ff_gauss_poly
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_poly(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_generalized_gauss1 generalized Gaussian coil
 * \ingroup polymers_chains
 *
 * \brief Polydisperse flexible polymers with Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * The scattering function for the generalized Gaussian coil is given by 
 *
 * \f[
 *  I_\textbf{general. Gauss}(q) = I_0\frac{U^{\frac{1}{2 \nu}} \Gamma\left(\frac{1}{2 \nu}\right)-
 *   \Gamma\left(\frac{1}{\nu}\right)-U^{\frac{1}{2\nu}}
 *   \Gamma\left(\frac{1}{2 \nu},U\right)+\Gamma\left(\frac{1}{\nu},U\right)}{\nu U^{1/\nu}}
 * \f]
 * with the modified variable
 * \f[
 *   U= \left(2\nu+1\right)\left(2\nu+2\right)\frac{q^2R_G^2}{6}
 * \f]
 * and the unnormalized incomplete Gamma Function 
 * \f$\Gamma(a,x) = \int_x^\infty \mathrm{d}t \; t^{a-1} \exp(-t)\f$
 * and the Gamma function \f$\Gamma(a)=\Gamma(a,0)=\int_0^\infty \mathrm{d}t\;  t^{a-1} \exp(-t)\f$.
 * \f$\nu\f$ is the excluded volume parameter from the Flory mean field theory and
 * typical values for them are
 *  - \f$\nu=1/3\f$: partially precipitate in poor solvents
 *  - \f$\nu=1/2\f$: thermally relaxed in "theta"-solvents
 *  - \f$\nu=3/5\f$: swollen in good solvents
 *  .
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu </td>
 *       <td>excluded volume parameter \f$\nu\in[1/2;1]\f$</td>
 *      </tr><tr>
 *       <td>\b I0 </td>
 *       <td> forward scattering \f$I_0\f$</td>
 *      </tr></table>
 *
 * \note The exclude volume parameter \f$\nu\f$ should be choosen between \f$\nu \in [1/2, 1]\f$.
 *
 * \image html form_factor/gaussian_coils/generalized_gaussian_coils.png "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters."
 * \image latex form_factor/gaussian_coils/generalized_gaussian_coils.eps "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters." width=10cm
 */

/**
 * \ingroup ff_generalized_gauss1
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_generalized_1(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_generalized_gauss2 generalized Gaussian coil 2
 * \ingroup polymers_chains
 *
 * \brief Polydisperse flexible polymers with Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * The scattering function for the generalized Gaussian coil is given by 
 *
 * \f[
 *  I_\textbf{general. Gauss}(q) = I_0\frac{U^{\frac{1}{2 \nu}} \Gamma\left(\frac{1}{2 \nu}\right)-
 *   \Gamma\left(\frac{1}{\nu}\right)-U^{\frac{1}{2\nu}}
 *   \Gamma\left(\frac{1}{2 \nu},U\right)+\Gamma\left(\frac{1}{\nu},U\right)}{\nu U^{1/\nu}}
 * \f]
 * with the modified variable
 * \f[
 *   U= \left(2\nu+1\right)\left(2\nu+2\right)\frac{q^2R_G^2}{6}
 * \f]
 * \f$\nu\f$ is the excluded volume parameter from the Flory mean field theory and
 * typical values for them are
 *  - \f$\nu=1/3\f$: partially precipitate in poor solvents
 *  - \f$\nu=1/2\f$: thermally relaxed in "theta"-solvents
 *  - \f$\nu=3/5\f$: swollen in good solvents
 *  .
 *
 * This scattering function only differs from "generalized Gaussian coil" only by the 
 * parametrization of the forward scattering which has been 
 * parametrized by \f$I_0=\left(b_p-V\eta_{s}\right)^2\f$

 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu </td>
 *       <td>excluded volume parameter \f$\nu\in[1/2;1]\f$</td>
 *      </tr><tr>
 *       <td>\b b_p </td>
 *       <td> scattering length of polymer \f$b_p\f$ in [cm]</td>
 *      </tr><tr>
 *       <td>\b V </td>
 *       <td> volume of a single polymer molecule\f$V\f$ in [cm<SUP>3</SUP>]</td>
 *      </tr><tr>
 *       <td>\b eta_s </td>
 *       <td> scattering length density of solvent \f$\eta_s\f$ in [cm<SUP>-2</SUP>]</td>
 *      </tr></table>
 *
 * \note The exclude volume parameter \f$\nu\f$ should be choosen between \f$\nu \in [1/2, 1]\f$.
 *
 * \image html form_factor/gaussian_coils/generalized_gaussian_coils.png "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters."
 * \image latex form_factor/gaussian_coils/generalized_gaussian_coils.eps "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters." width=10cm
 */

/**
 * \ingroup ff_generalized_gauss2
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */


sasfit_ff_DLLEXP scalar sasfit_ff_gauss_generalized_2(scalar q, sasfit_param * param);

/** 
 * \defgroup ff_generalized_gauss3 generalized Gaussian coil 3
 * \ingroup polymers_chains
 *
 * \brief Polydisperse flexible polymers with Gaussian statistics
 *
 *
 * \image html form_factor/gaussian_coils/random_walk.png "random walk"
 * \image latex form_factor/gaussian_coils/random_walk.png "random walk"
 *
 * The scattering function for the generalized Gaussian coil is given by 
 *
 * \f[
 *  I_\textbf{general. Gauss}(q) = I_0\frac{U^{\frac{1}{2 \nu}} \Gamma\left(\frac{1}{2 \nu}\right)-
 *   \Gamma\left(\frac{1}{\nu}\right)-U^{\frac{1}{2\nu}}
 *   \Gamma\left(\frac{1}{2 \nu},U\right)+\Gamma\left(\frac{1}{\nu},U\right)}{\nu U^{1/\nu}}
 * \f]
 * with the modified variable
 * \f[
 *   U= \left(2\nu+1\right)\left(2\nu+2\right)\frac{q^2R_G^2}{6}
 * \f]
 * \f$\nu\f$ is the excluded volume parameter from the Flory mean field theory and
 * typical values for them are
 *  - \f$\nu=1/3\f$: partially precipitate in poor solvents
 *  - \f$\nu=1/2\f$: thermally relaxed in "theta"-solvents
 *  - \f$\nu=3/5\f$: swollen in good solvents
 *  .
 *
 * This scattering function only differs from "generalized Gaussian coil" only by the 
 * parametrization of the forward scattering which has been 
 * parametrized by \f$I_0=\left(b_p-\frac{M_w}{N_a\rho_p}\eta_{s}\right)^2\f$.
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b Rg </td>
 *       <td>radius of gyration</td>
 *      </tr><tr>
 *       <td>\b nu </td>
 *       <td>excluded volume parameter \f$\nu\in[1/2;1]\f$</td>
 *      </tr><tr>
 *       <td>\b b_p </td>
 *       <td> scattering length of polymer \f$b_p\f$ in [cm]</td>
 *      </tr><tr>
 *       <td>\b Mw </td>
 *       <td> molecular weight of a polymer molecule \f$M_w\f$ in [g/mol]</td>
 *      </tr><tr>
 *       <td>\b rho_p </td>
 *       <td> mass density of the polymer \f$\rho_p\f$ in [g/cm<SUP>3</SUP>]</td>
 *      </tr><tr>
 *       <td>\b eta_s </td>
 *       <td> scattering length density of solvent \f$\eta_s\f$ in [cm<SUP>-2</SUP>]</td>
 *      </tr></table>
 *
 * \note The exclude volume parameter \f$\nu\f$ should be choosen between \f$\nu \in [1/2, 1]\f$.
 *
 * \image html form_factor/gaussian_coils/generalized_gaussian_coils.png "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters."
 * \image latex form_factor/gaussian_coils/generalized_gaussian_coils.eps "Scattering function of the generalized Gaussian coil plotted for several excluded volume parameters." width=10cm
 */

/**
 * \ingroup ff_generalized_gauss3
 *
 * \sa ff_gauss1, ff_gauss2, ff_gauss3, ff_gauss_poly, ff_generalized_gauss1, ff_generalized_gauss2, ff_generalized_gauss3, form_fac
 */

sasfit_ff_DLLEXP scalar sasfit_ff_gauss_generalized_3(scalar q, sasfit_param * param);

#endif

