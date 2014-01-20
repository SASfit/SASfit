#include <uutil.h>
#include <sys/mount.h>

static int mountCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);
static int umountCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

void uu_mount_setup(Tcl_Interp *interp)
{
    Tcl_CreateObjCommand(interp, "::uutil::mount", mountCmd, NULL, NULL);
    Tcl_CreateObjCommand(interp, "::uutil::umount", umountCmd, NULL, NULL);
}

static int mountCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    const char *switches[] = {
        "-readonly",
        "-nosuid",
        "-nodev",
        "-noexec",
        NULL
    };
    int sw;
    unsigned long flags = 0xC0ED0000;
    enum {
        SW_READONLY,
        SW_NOSUID,
        SW_NODEV,
        SW_NOEXEC,
        SW_DUMMY
    };
    if (objc < 4)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? device mountpoint filesystem");
        return TCL_ERROR;
    }
    while (objc > 4)
    {
        if (Tcl_GetIndexFromObj(interp, objv[1], switches, "switch", 0, &sw) != TCL_OK)
            return TCL_ERROR;
        objc--;
        objv++;
        switch (sw)
        {
            case SW_READONLY:
                flags |= MS_RDONLY;
                break;
        }
    }
    if (mount(Tcl_GetString(objv[1]), Tcl_GetString(objv[2]), Tcl_GetString(objv[3]), flags, NULL) == -1)
    {
        return TCL_ERROR;
    }
    return TCL_OK;
}

static int umountCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    if (objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "mountpoint");
        return TCL_ERROR;
    }
    if (umount(Tcl_GetString(objv[1])) == -1)
    {
        return TCL_ERROR;
    }
    return TCL_OK;
}

