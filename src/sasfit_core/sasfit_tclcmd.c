/*
 * src/sasfit_core/sasfit_tclcmd.c
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

#include "include/sasfit_core.h"

int sasfit_get_maxpar_cmd(ClientData clientData,
				Tcl_Interp *interp,
				int objc,
				Tcl_Obj *CONST objv[])
{
	Tcl_SetObjResult(interp, Tcl_NewIntObj(sasfit_get_maxpar()));
	return TCL_OK;
}

int sasfit_get_lib_prefix_cmd(ClientData clientData,
				Tcl_Interp *interp,
				int objc,
				Tcl_Obj *CONST objv[])
{
	Tcl_SetObjResult(interp, Tcl_NewStringObj(sasfit_get_lib_prefix(), -1));
	return TCL_OK;
}

int sasfit_get_lib_suffix_cmd(ClientData clientData,
				Tcl_Interp *interp,
				int objc,
				Tcl_Obj *CONST objv[])
{
	Tcl_SetObjResult(interp, Tcl_NewStringObj(sasfit_get_lib_suffix(), -1));
	return TCL_OK;
}

int sasfit_covar_usage()
{
	sasfit_err("wrong # args, should be: sasfit_covar ?list of indices for each contribution: SD FF SQ (in this order)?\n");
	return TCL_ERROR;
}

int sasfit_covar_array_index(int model_count, int contrib, int model, int param)
{
	if (model_count < 0 || contrib < 0 || model < 0 || param < 0)
		return -1;
	return contrib*model_count*MAXPAR + model*MAXPAR + param;
}

// Take model relative parameter indices from a hierarchical list.
// Transform them to covar matrix absolute column/row indices.
// Store them in a hierarchical list.
Tcl_Obj * sasfit_covar_abs_idx(Tcl_Interp * interp,
                               Tcl_Obj *CONST active,
                               int modelcount)
{
	int clen, mlen, plen, c, m, p, idx; // Contribution, Model, Parameter
	Tcl_Obj * modellist = NULL;
	Tcl_Obj * paramlist = NULL;
	Tcl_Obj * index = NULL;
//	Tcl_Obj * new_modellist = NULL;
//	Tcl_Obj * new_paramlist = NULL;
	Tcl_Obj * arrIdx = NULL;
	int err = TCL_OK;

	if (!active) return NULL;

	err = Tcl_ListObjLength(interp, active, &clen);
	if (!err) arrIdx = Tcl_NewListObj(0, NULL);

	for(c=0; !err && c < clen; c++) // cycle through index lists per contribution
	{
		err = Tcl_ListObjIndex(interp, active, c, &modellist);
		if (err || !modellist) continue;
		err = Tcl_ListObjLength(interp, modellist, &mlen);

		// always create the (empty) list to maintain list structure
//		new_modellist = Tcl_NewListObj(0, NULL);
//		if (!new_modellist) err = TCL_ERROR;

		for(m=0; !err && m < mlen; m++)
		{
			err = Tcl_ListObjIndex(interp, modellist, m, &paramlist);
			if (err || !paramlist) continue;
			err = Tcl_ListObjLength(interp, paramlist, &plen);

			// always create the (empty) list to maintain list structure
//			new_paramlist = Tcl_NewListObj(0, NULL);
//			if (!new_paramlist) err = TCL_ERROR;

			for(p=0; !err && p < plen; p++)
			{
				err = Tcl_ListObjIndex(interp, paramlist, p, &index);
				if (err || !index) continue;

				// relative index
				err = Tcl_GetIntFromObj(interp, index, &idx);
				if (err || idx < 0) continue;
				// calculate absolute array index
				idx = sasfit_covar_array_index(modelcount, c, m, idx);
				// save for data extraction later
//				err = Tcl_ListObjAppendElement(interp, new_paramlist, Tcl_NewIntObj(idx));
				err = Tcl_ListObjAppendElement(interp, arrIdx, Tcl_NewIntObj(idx));
			}
//			err = Tcl_ListObjAppendElement(interp, new_modellist, new_paramlist);
		}
//		err = Tcl_ListObjAppendElement(interp, arrIdx, new_modellist);
	}
	if (!err) return arrIdx;
	return NULL;
}

Tcl_Obj * sasfit_covar_get_data(Tcl_Interp * interp,
                                Tcl_Obj * arrIdx,
                                scalar ** covar,
                                int covarsize)
{
	int len, i, p, px, py;
	scalar val;
	Tcl_Obj * array = NULL;
	Tcl_Obj * row = NULL;
	Tcl_Obj * index = NULL;
	int err = TCL_OK;

	if (!arrIdx || !covar) return NULL;

	err  = Tcl_ListObjLength(interp, arrIdx, &len);
	array = Tcl_NewListObj(0, NULL);

	for (i=0; !err && i < len; i++) // per parameter
	{
		// open a new row
		row = Tcl_NewListObj(0, NULL);
		err = Tcl_ListObjIndex(interp, arrIdx, i, &index);
		if (err) continue;
		err = Tcl_GetIntFromObj(interp, index, &py);

		for (p=i; !err && p < len; p++) // per parameter
		{
			err = Tcl_ListObjIndex(interp, arrIdx, p, &index);
			if (err) continue;
			err = Tcl_GetIntFromObj(interp, index, &px);
			if (err || px >= covarsize || py >= covarsize) continue;
			// add a new value
			if (px == py) {
			    val = sqrt(fabs(covar[px][py]));
			} else {
			    val = -9999;
			    if ((sqrt(fabs(covar[py][py]))*sqrt(fabs(covar[px][px]))) != 0)
                        val = covar[px][py]/(sqrt(fabs(covar[px][px]))*sqrt(fabs(covar[py][py])));
			}
			err = Tcl_ListObjAppendElement(interp, row, Tcl_NewDoubleObj(val));
		}
		if (err) continue;
		err = Tcl_ListObjAppendElement(interp, array, row);
	}
	if (err) return NULL;
	return array;
}

// active parameter means: a paramter which was fitted
int sasfit_covar_cmd(sasfit_cdata * sf_data,
                     Tcl_Interp   * interp,
                     int            objc,
                     Tcl_Obj *CONST objv[])
{
	const int model_count = 3; // FF, SD, SQ
	Tcl_Obj * array = NULL; // contains resulting covar matrix diagonal
	Tcl_Obj * arrIdx = NULL;
	int err = TCL_OK;

	if (!sf_data || !interp || !objv || !sf_data->covar) return TCL_ERROR;
	if (sf_data->ma == 0) return TCL_OK; // no elements yet

	// return model and contribution count
	if (objc == 1) {
		Tcl_Obj * val[2] = { Tcl_NewIntObj(model_count),
		                     Tcl_NewIntObj(sf_data->max_SD) };
		Tcl_SetObjResult(interp, Tcl_NewListObj(2, val));
		return TCL_OK;
	}

	// invalid input data, show help
	if (objc != 2 || !objv[1] || !objv[2]) {
		return sasfit_covar_usage();
	}

	// get&verify input data
	// transform relative indices to absolute array indices

	arrIdx = sasfit_covar_abs_idx(interp, objv[1], model_count);
	if (!arrIdx) err = TCL_ERROR;

	// retrieve and format output data
	if (!err) array = sasfit_covar_get_data(interp, arrIdx, sf_data->covar, sf_data->ma);

//	Tcl_DecrRefCount(arrIdx); // is this necessary ?

	if (!array) {
		sasfit_err("Could not get covar matrix data!\n");
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, array);
	return TCL_OK;
}

