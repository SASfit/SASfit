/*
 * src/sasfit_common/include/sasfit_error_sd.h
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

#ifndef SASFIT_ERROR_SD_H
#define SASFIT_ERROR_SD_H

#ifdef SASFIT_RETURNVAL_ON_ERROR
  #undef SASFIT_RETURNVAL_ON_ERROR
#endif
#define SASFIT_RETURNVAL_ON_ERROR	0.0

#ifdef SASFIT_ERR_PREFIX
  #undef SASFIT_ERR_PREFIX
#endif
#define SASFIT_ERR_PREFIX "Can not calculate size distribution because "

#endif

