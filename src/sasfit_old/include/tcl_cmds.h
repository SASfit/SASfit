/*
 * src/sasfit_old/include/tcl_cmds.h
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

#ifndef _SASFIT_TCL_CMDS_H
#define _SASFIT_TCL_CMDS_H

// sasfit main commands

int Sasfit_guess_errCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, char **argv));
int Sasfit_iqCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, const char **argv));
int Sasfit_2DiqCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, const char **argv));
int Sasfit_global_iqCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, char **argv));
int Sasfit_nrCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, const char **argv));
int Sasfit_iqfitCmd _ANSI_ARGS_((struct sasfit_CData *SASFIT_CData,
                         Tcl_Interp *interp, int argc, const char **argv));
int Sasfit_iqglobalfitCmd _ANSI_ARGS_((struct sasfit_CData *SASGlobalFIT_CData,
                         Tcl_Interp *interp, int argc, char **argv));
int Sasfit_resCmd _ANSI_ARGS_((ClientData clientData,
                         Tcl_Interp *interp, int argc, char **argv));

// sasfit DLS commands

int DLS_CumulantFitCmd      _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

int DLS_ExponentialFitCmd   _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

int DLS_StretchedExpFitCmd  _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

int DLS_CumulantCalcCmd    _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

int DLS_ExponentialCalcCmd  _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

int DLS_StretchedExpCalcCmd _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp, int argc, char **argv));

// sasfit extrapol commands

int Sasfit_PorodFitCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_GuinierFitCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_StructParCalcCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_ZimmFitCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DebyeFitCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_OrnsteinZernickeFitCmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_FredholmIntegrals_RegularizationCmd _ANSI_ARGS_((struct sasfit_CData *SASFIT_CData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_EM_smoothing_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_EM_ME_const_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_EM_ME_adaptive_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_MEM1_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_MuCh_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));
int Sasfit_DR_SDM_Cmd _ANSI_ARGS_((ClientData clientData,
                                Tcl_Interp *interp, int argc, char **argv));

#endif

