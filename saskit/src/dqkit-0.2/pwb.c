/* Written by Matt Newman and Jean-Claude Wippler, as part of Tclkit.
 * March 2003 - placed in the public domain by the authors.
 *
 * Expose TclSetLibraryPath to script layer
 */

#include <tcl.h>
#include <tclInt.h> /* TclGetLibraryPath */

/*
 * Exported Routines
 */

DLLEXPORT Tcl_AppInitProc Pwb_Init;

/*
 * Internal Routines
 */

static Tcl_ObjCmdProc	LibraryPathObjCmd;

/* Support for encodings, from Vince Darley <vince.darley@eurobios.com> */
int
LibraryPathObjCmd(dummy, interp, objc, objv)
     ClientData dummy;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
     if (objc == 1) {
	Tcl_SetObjResult(interp, TclGetLibraryPath());
     }  else if (objc == 2) {
	Tcl_Obj *path=Tcl_DuplicateObj(objv[1]);
	TclSetLibraryPath(Tcl_NewListObj(1,&path));
	TclpSetInitialEncodings();
	Tcl_FindExecutable(Tcl_GetVar(interp, "argv0", TCL_GLOBAL_ONLY));
     } else {
        /* fix for 8.5 and [info nameofexe] */
        char *noe = Tcl_GetString(objv[2]);
	Tcl_Obj *path=Tcl_DuplicateObj(objv[1]);
	TclSetLibraryPath(Tcl_NewListObj(1,&path));
	TclpSetInitialEncodings();
	Tcl_FindExecutable(noe);
     }
     return TCL_OK;
}

/*
 * Public Entrypoint
 */

DLLEXPORT int Pwb_Init(Tcl_Interp *interp)
{
    Tcl_CreateObjCommand(interp, "pwblibrarypath", LibraryPathObjCmd, 0, 0);
    return Tcl_PkgProvide( interp, "dqkitpwb", "1.1");
}
