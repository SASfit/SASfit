/*
 * src/sasfit_ff/disc_rwbrush/include/public.h
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

#ifndef DISC_RWBRUSH_PUBLIC_H
#define DISC_RWBRUSH_PUBLIC_H


sasfit_ff_DLLEXP scalar sasfit_ff_disc_rwbrush(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_disc_rwbrush_lc(scalar q, sasfit_param * param);
sasfit_ff_DLLEXP scalar sasfit_ff_disc_rwbrush_nagg(scalar q, sasfit_param * param);

#endif

