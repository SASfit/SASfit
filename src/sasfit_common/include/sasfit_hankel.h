/*
 * src/sasfit_common/include/sasfit_hankel.h
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

void sasfit_set_FBT(double nu, int option, int N, double Q) ;
scalar sasfit_FBT(scalar x, double (*intKern_fct)(double, void *), void *) ;
scalar sasfit_qwe(double nu, double (*f)(double, void *), double x, void *fparams, int nIntervalsMax, scalar rtol, scalar atol);
scalar sasfit_HankelChave(double nu, double (*f)(double, void *), double x,  void *fparams, int nIntervalsMax, scalar rtol, scalar atol);
