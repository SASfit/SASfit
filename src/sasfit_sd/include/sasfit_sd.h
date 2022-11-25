/*
 * src/sasfit_sd/include/sasfit_sd.h
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

#ifndef SASFIT_SD_H
#define SASFIT_SD_H

#include <sasfit_common.h>
#include "sasfit_sd_shared_exports.h"

/**
 * \file sasfit_sd.h
 * \ingroup size_distrib
 * Functions which are not (yet) available as plugins are declared here.
 */

typedef enum
{
	SD_id_Delta,
	SD_id_Monodisperse,
	SD_id_LogNorm,
	SD_id_AdivB
} sasfit_SD_id;

sasfit_sd_DLLEXP scalar sasfit_sd_LogNorm(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_LogNorm_fp(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_BiLogNorm(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_GaussDistribution(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Maxwell(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Schulz_Zimm(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Weibull(scalar x, sasfit_param * param);

/**
 * \defgroup uniform Uniform distribution
 * \ingroup size_distrib
 *
 * \brief calculates a Uniform distribution function
 *
 * The continuous uniform distribution is a family of probability distributions
 * such that for each member of the family, all intervals of the same length on
 * the distribution's support are equally probable. The support is defined by the
 * two parameters, \f$A\f$ and \f$B\f$, which are its minimum and maximum values.
 * \f[
 *		n(x) =  \Bigg\{
 *		\begin{array}{ll}
 *			\frac{N}{B-A} & \mbox{for } A \leq x \leq Bc\\
 *			0		& \mbox{otherwise}
 *		\end{array}
 * \f]
 *
 * \image html sizedistributions/Uniform.png "Uniform distribution"
 *
 */

/**
 * \ingroup uniform
 *
 * \sa sasfit_sd_Uniform.c, size_distrib
 */
sasfit_sd_DLLEXP scalar sasfit_sd_Uniform(scalar x, sasfit_param * param);

sasfit_sd_DLLEXP scalar sasfit_sd_Beta(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_fractalSD(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Rn_Triangular(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Triangular(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_GEX(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_GEV(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_gammaSD(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_Monodisperse(scalar x, sasfit_param * param);

/**
 * \defgroup delta Delta distribution
 * \ingroup size_distrib
 *
 * \brief Scales the form factor.
 *
 * This size distribution simply scales the form factor with a constant value
 * \f$N\f$ which is provided as parameter by the user.
 */

/**
 * \ingroup delta
 *
 * \sa sasfit_sd_Delta.c, size_distrib
 */
sasfit_sd_DLLEXP scalar sasfit_sd_Delta(scalar x, sasfit_param * param);
sasfit_sd_DLLEXP scalar sasfit_sd_AdivB(scalar x, sasfit_param * param);

#endif

