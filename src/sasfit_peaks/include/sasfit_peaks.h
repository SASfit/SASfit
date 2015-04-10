/*
 * src/sasfit_peaks/include/sasfit_peaks.h
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
 */

#ifndef SASFIT_PEAKS_H
#define SASFIT_PEAKS_H

#include <sasfit_common.h>
#include "sasfit_peaks_shared_exports.h"

/**
 * \file sasfit_peaks.h
 * \ingroup peaks
 * Functions which are not (yet) available as plugins are declared here.
 */

/** 
 * \defgroup gamma_amp Gamma Peak (Amplitude) 
 * \ingroup gamma
 * \brief Gamma peak (Amplitude): 
 *	\f$ y(x) = c_0 + a \exp(-z) \left(\frac{z+k-1}{k-1}\right)^{k-1}\f$ with \f$z=\frac{x-x_c}{\theta}\f$
 *
 * In probability theory and statistics, the gamma distribution is a 
 * two-parameter family of continuous probability distributions. 
 * It has a scale parameter \f$\theta\f$ and a shape parameter \f$k\f$. If \f$k\f$ 
 * is an integer then the distribution represents the sum of \f$k\f$ independent 
 * exponentially distributed random variables, each of which has a mean of 
 * \f$\theta\f$ (which is equivalent to a rate parameter of \f$\theta^{-1}\f$).
 * The parameter \f$x_c\f$ has been added to enable variable \f$x\f$ positioning,
 * whereas the \f$+\theta(k-1)\f$ adjusts \f$x_c\f$ so that it represents the mode.
 * \f$c_0\f$ is the offset value.
 * The function returns the offset \f$c_0\f$ for those \f$x\f$ where it is undefined
 *
 * \f[
 *	y(x) = \Bigg\{
 *	\begin{array}{ll}
 *		c_0 + a \exp(-z) \left(\frac{z+k-1}{k-1}\right)^{k-1} & \mbox{for } (z+k-1) > 0 \\
 *		c_0 &  \mbox{otherwise}
 *	\end{array}
 * \f]
 * with
 * \f$ z=\frac{x-x_c}{\theta} \f$
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b ampl. </td>
 *       <td>amplitude \f$a\f$ of the Gamma peak</td>
 *      </tr><tr>
 *       <td>\b center </td>
 *       <td> location parameter (mode) \f$x_c\f$</td>
 *      </tr><tr>
 *       <td>\b width </td>
 *       <td> scaling parameter \f$\theta>0\f$</td>
 *      </tr><tr>
 *       <td>\b shape </td>
 *       <td> shape parameter \f$1<k\f$</td>
 *      </tr><tr>
 *       <td>\b backgr </td>
 *       <td> offset \f$c_0\f$</td>
 *      </tr></table>
 *
 * \note The shape parameter needs to be larger than 1 \f$k>0\f$.
 * \note The scale parameter needs to be larger than zero \f$\theta>0\f$
 * \note Default (Size) Distribution: delta
 *
 * \image html peaks/Gamma/GammaAmplitude.png "plot of Gamma distribution"
 * \image latex peaks/Gamma/GammaAmplitude.eps "plot of Gamma distribution" width=10cm
 *
 * \sa sasfit_peak_GammaAmplitude.c, sasfit_peak_GammaArea.c, peaks
 */
sasfit_peaks_DLLEXP scalar sasfit_peak_GammaAmplitude(scalar x, sasfit_param * param);

/** 
 * \defgroup gamma_area Gamma Peak (Area) 
 * \ingroup gamma
 * \brief Gamma peak (Amplitude): 
 *			\f$ y(x) = c_0 + \frac{a}{\theta\Gamma(k)} \exp(-z) z^{k-1}\f$ with \f$z=\frac{x-x_c}{\theta}+k-1\f$
 *
 * In probability theory and statistics, the gamma distribution is a 
 * two-parameter family of continuous probability distributions. 
 * It has a scale parameter \f$\theta\f$ and a shape parameter \f$k\f$. If \f$k\f$ 
 * is an integer then the distribution represents the sum of \f$k\f$ independent 
 * exponentially distributed random variables, each of which has a mean of 
 * \f$\theta\f$ (which is equivalent to a rate parameter of \f$\theta^{-1}\f$).
 * The parameter \f$x_c\f$ has been added to enable variable \f$x\f$ positioning,
 * whereas the \f$+\theta(k-1)\f$ adjusts \f$x_c\f$ so that it represents the mode.
 * \f$c_0\f$ is the offset value.
 * The function returns the offset \f$c_0\f$ for those \f$x\f$ where it is undefined
 *
 * \f[
 *		y(x) = \Bigg\{
 *		\begin{array}{ll}
 *			c_0 + \frac{a}{\theta\Gamma(k)} \exp(-z) z^{k-1} & \mbox{for } z > 0 \\
 *			c_0 &  \mbox{otherwise}
 *		\end{array}
 * \f]
 * with
 * \f$
 *		z=\frac{x-x_c}{\theta} +k-1
 * \f$
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b area </td>
 *       <td>area \f$a\f$ of the Gamma peak</td>
 *      </tr><tr>
 *       <td>\b center </td>
 *       <td> location parameter (mode) \f$x_c\f$</td>
 *      </tr><tr>
 *       <td>\b width </td>
 *       <td> scaling parameter \f$\theta>0\f$</td>
 *      </tr><tr>
 *       <td>\b shape </td>
 *       <td> shape parameter \f$1<k\f$</td>
 *      </tr><tr>
 *       <td>\b backgr </td>
 *       <td> offset \f$c_0\f$</td>
 *      </tr></table>
 *
 * \note The shape parameter needs to be larger than 1 \f$k>0\f$.
 * \note The scale parameter needs to be larger than zero \f$\theta>0\f$
 * \note Default (Size) Distribution: delta
 *
 * \image html peaks/Gamma/GammaArea.png "plot of Gamma distribution"
 * \image latex peaks/Gamma/GammaArea.eps "plot of Gamma distribution" width=10cm
 *
 */

/** 
 * \ingroup gamma_area
 *
 * \sa sasfit_peak_GammaAmplitude.c, sasfit_peak_GammaArea.c, peaks
 */
sasfit_peaks_DLLEXP scalar sasfit_peak_GammaArea(scalar x, sasfit_param * param);

/** 
 * \defgroup gauss_area Gaussian Peak (Area)
 * \ingroup gaussian
 * \brief Gaussian peak (Area}: 
 *			\f$ y(x) = c_0+\frac{a}{\sqrt{2\pi}\sigma} \exp\left(-z^2/2\right) \f$
 *
 * 
 * The Gaussian is also known as the normal distribution function. 
 * It is a symmetric function whose mode \f$x_c\f$ is equal the location parameter \c center.
 * Its standard deviation \f$\sigma\f$ is equal to the width parameter \c width.
 * The area \f$a\f$ below the peak (without the offset) is equal to \c area and the constant offset 
 * \f$c_0\f$ to \c backgr.
 * \f[
 *		y(x) = c_0+\frac{a}{\sqrt{2\pi}\sigma} \exp\left(-\frac{z^2}{2}\right)
 * \f]
 * with
 * \f[
 *		z ~=~ \frac{x-x_c}{\sigma} 
 * \f]
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b area </td>
 *       <td> area \f$a\f$ below the Gaussian peak</td>
 *      </tr><tr>
 *       <td>\b center </td>
 *       <td> location parameter \f$x_c\f$</td>
 *      </tr><tr>
 *       <td>\b width </td>
 *       <td> scaling parameter \f$\sigma>0\f$</td>
 *      </tr><tr>
 *       <td>\b backgr </td>
 *       <td> offset \f$c_0\f$</td>
 *      </tr></table>
 *
 * \note The width parameter needs to be larger than zero \f$\sigma>0\f$
 * \note Default (Size) Distribution: \ref delta
 *
 * \image html peaks/Gaussian/GaussianArea.png "plot of Gaussian or normal distribution"
 * \image latex peaks/Gaussian/GaussianArea.eps "plot of Gaussian or normal distribution" width=10cm
 *
 */

/**
 * \ingroup gauss_area
 *
 * \sa sasfit_peak_GaussianAmplitude.c, sasfit_peak_GaussianArea.c, peaks
 */
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianArea(scalar x, sasfit_param * param);

/** 
 * \defgroup gauss_amp Gaussian Peak (Amplitude) 
 * \ingroup gaussian
 *
 * \brief Gaussian peak (Amplitude): 
 *			\f$ y(x) = c_0+a \exp\left(-z^2/2\right) \f$
 *
 * The Gaussian is also known as the normal distribution function. 
 * It is a symmetric function whose mode \f$x_c\f$ is equal the location parameter \c center.
 * Its standard deviation \f$\sigma\f$ is equal to the width parameter \c width.
 * The amplitude \f$a\f$ of the peak is equal to \c ampl and the constant offset 
 * \f$c_0\f$ to \c backgr.
 * \f[
 *		y(x) = c_0+a \exp\left(-\frac{z^2}{2}\right)
 * \f]
 * with
 * \f[
 *		z ~=~ \frac{x-x_c}{\sigma} 
 * \f]
 *
 *
 * \par Required parameters:
 *      <table border="0"><tr>
 *       <td>\b ampl. </td>
 *       <td>amplitude \f$a\f$ of the Gaussian peak</td>
 *      </tr><tr>
 *       <td>\b center </td>
 *       <td> location parameter \f$x_c\f$</td>
 *      </tr><tr>
 *       <td>\b width </td>
 *       <td> scaling parameter \f$\sigma>0\f$</td>
 *      </tr><tr>
 *       <td>\b backgr </td>
 *       <td> offset \f$c_0\f$</td>
 *      </tr></table>
 *
 * \note The width parameter needs to be larger than zero \f$\sigma>0\f$
 * \note Default (Size) Distribution: \ref delta
 *
 * \image html peaks/Gaussian/GaussianAmplitude.png "plot of Gaussian or normal distribution"
 * \image latex peaks/Gaussian/GaussianAmplitude.eps "plot of Gaussian or normal distribution" width=10cm
 *
 */

/** 
 * \ingroup gauss_amp
 *
 * \sa sasfit_peak_GaussianAmplitude.c, sasfit_peak_GaussianArea.c, peaks
 */
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianAmplitude(scalar x, sasfit_param * param);

sasfit_peaks_DLLEXP scalar sasfit_peak_BetaAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_BetaArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ChiSquaredAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ChiSquaredArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ErfcPeakAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ErfcPeakArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ErrorAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ErrorArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ExponentiallyModifiedGaussianAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ExponentiallyModifiedGaussianArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ExtremeValueAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_ExtremeValueArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_fatique_lifeArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_FVarianceAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_FVarianceArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianLorentzianCrossProductAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianLorentzianCrossProductArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianLorentzianSumAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GaussianLorentzianSumArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GeneralizedGaussian1Amplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GeneralizedGaussian1Area(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GeneralizedGaussian2Amplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GeneralizedGaussian2Area(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GiddingsAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_GiddingsArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_HaarhoffVanderLindeArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_HalfGaussianModifiedGaussianArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_InvertedGammaAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_InvertedGammaArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_KumaraswamyAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_KumaraswamyArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogisticAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogisticArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogLogisticAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogLogisticArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogNormalAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogNormalArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogNormal4ParameterAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LogNormal4ParameterArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LorentzianAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LorentzianArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LaplaceAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_LaplaceArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_MaxwellAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_MaxwellArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_generalizedMaxwellAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_generalizedMaxwellArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PearsonIVAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PearsonIVArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PearsonVIIAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PearsonVIIArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_powerlognormalArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_powernormalArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakwithPowerTermAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakwithPowerTermArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakwith2ndWidthTermAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_PulsePeakwith2ndWidthTermArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_Student_tAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_Student_tArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_VoigtPeakAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_VoigtPeakArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_WeibullAmplitude(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_WeibullArea(scalar x, sasfit_param * param);
sasfit_peaks_DLLEXP scalar sasfit_peak_QENS_ConfinementWithGaussianPotential(scalar x, sasfit_param * param);

#endif

