#include <uutil.h>

int Uutil_Init(Tcl_Interp *interp)
{
    static char initCmd[] = "namespace eval ::uutil {}";
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.2", 0) == NULL)
        return TCL_ERROR;
#endif
    Tcl_EvalEx(interp, initCmd, -1, TCL_EVAL_GLOBAL);
    uu_mount_setup(interp);
    uu_execv_setup(interp);
    uu_loop_setup(interp);
    Tcl_PkgProvide(interp, "uutil", "0.1");
    return TCL_OK;
}

int Uutil_SafeInit(Tcl_Interp *interp)
{
    return TCL_OK;
}

