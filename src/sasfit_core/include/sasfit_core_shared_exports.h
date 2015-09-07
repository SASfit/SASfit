/*
 * src/sasfit_core/include/sasfit_core_shared_exports.h
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef SASFIT_CORE_SHARED_EXPORTS_H 
#define SASFIT_CORE_SHARED_EXPORTS_H 

#include <sasfit_common_shared_exports.h>

/**
 * \file sasfit_core_shared_exports.h
 * \copydoc sasfit_common_shared_exports.h
 */

/**
 * \def sasfit_core_DLLEXP
 * \copydoc sasfit_common_DLLEXP
 */

#if defined(sasfit_core_shrd_EXPORTS) || defined(sasfit_core_stat_EXPORTS) || defined(sasfit_EXPORTS)
	#define sasfit_core_DLLEXP SASFIT_LIB_EXPORT
#else
	#define sasfit_core_DLLEXP SASFIT_LIB_IMPORT
#endif

#endif // this file
