/*
 * src/sasfit_core/include/sasfit_2d.h
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

#ifndef SASFIT_2D_H
#define SASFIT_2D_H

/** Scale types for 2D calculation. */
typedef enum
{
	LOG, SQRT, ARCSINH
} sasfit_2d_scale_t;

/** Operation modes of 2D calculation. */
typedef enum
{
	AUTO, MANUAL
} sasfit_2d_mode_t;

typedef struct 
{
//	bool              calc2d;
	int               num_pix;
//	scalar            angle;
	scalar            pixelsize;
	scalar            lambda;
	scalar            dist;
	scalar            min;
	scalar            max;
	sasfit_2d_scale_t scale;
	sasfit_2d_mode_t  mode;
//        char ct[STRLEN];
} sasfit_2d_param_t;

// sasfit_core_DLLEXP sasfit_2d_param_t	Det2DPar;

/** Initializes the data structure at startup. */
void sasfit_2d_init (void);

/**
 * Gets user configuration for 2D mode from the GUI (TCL code).
 * \returns TCL_OK on success, TCL_ERROR if an error occured.
 */
int sasfit_2d_set(Tcl_Interp * interp, const char * argv[]);

/**
 * This function implements the Tcl "sasfit_2Diq" command.
 * \returns A standard Tcl result.
 */
int Sasfit_2DiqCmd(ClientData    clientData, 
                   Tcl_Interp *  interp, 
                   int           argc, 
                   const char ** argv);

#endif // SASFIT_2D_H
