/*
 * src/sasfit_core/include/sasfit_vector.h
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

#ifndef SASFIT_VECTOR_H
#define SASFIT_VECTOR_H

#include "sasfit_core_shared_exports.h"

sasfit_core_DLLEXP float * vector(int nl, int nh);
sasfit_core_DLLEXP double * dvector(int nl, int nh);
sasfit_core_DLLEXP int * ivector(int nl, int nh);

sasfit_core_DLLEXP void free_vector(float * v, int nl, int nh);
sasfit_core_DLLEXP void free_dvector(double * v, int nl, int nh);
sasfit_core_DLLEXP void free_ivector(int * v, int nl, int nh);

#endif

