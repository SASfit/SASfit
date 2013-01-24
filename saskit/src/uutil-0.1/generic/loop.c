#include <sys/types.h>
#include <tcl.h>
#include <unistd.h>
#include <string.h>
#include <loop.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static int loopCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int l_set(char *device, char *file, int offset, int mode);
static int l_unset(char *device);

void uu_loop_setup(Tcl_Interp *interp)
{
    Tcl_CreateObjCommand(interp, "::uutil::loop", loopCmd, NULL, NULL);
}


static int loopCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    enum {
        C_SET, C_CLEAR, _DUMMY
    };
    const static char *commands[] = {
        "set",
        "clear",
    NULL };
    int idx;
    
    if (objc<2)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "command ?args?");
        return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], commands, "command", 0, &idx) != TCL_OK)
        return TCL_ERROR;
            
    switch (idx)
    {
        case C_SET:
            {
                int mode = O_RDWR, offset = 0;
                while (objc>4)
                {
                    enum {
                        SW_READONLY,
                        SW_READWRITE,
                        SW_OFFSET,
                        _DUMMY
                    };
                    const static char *switches[] = {
                        "-readonly",
                        "-readwrite",
                        "-offset",
                    NULL };
                    if (Tcl_GetIndexFromObj(interp, objv[2], switches, "switch", 0, &idx)!=TCL_OK)
                        return TCL_ERROR;
                    switch (idx)
                    {
                        case SW_READONLY:
                            mode = O_RDONLY;
                            break;
                        case SW_READWRITE:
                            mode = O_RDWR;
                            break;
                        case SW_OFFSET:
                            objv++;
                            objc--;
                            if (Tcl_GetIntFromObj(interp, objv[2], &offset) != TCL_OK)
                                return TCL_ERROR;
                            break;
                    }
                    objv++;
                    objc--;
                }
                if (objc!=4)
                {
                    Tcl_WrongNumArgs(interp, 2, objv, "?-readonly? ?-readwrite? device filename");
                    return TCL_ERROR;
                }

                printf("%s -> %s (%d,%d)\n",Tcl_GetString(objv[2]), Tcl_GetString(objv[3]), offset, mode);
                if (l_set(Tcl_GetString(objv[2]), Tcl_GetString(objv[3]), 0, mode))
                {
                    return TCL_OK;
                }  else  {
                    Tcl_SetResult(interp, "Unable to complete", NULL);
                    return TCL_ERROR;
                }
            }
            break;
        case C_CLEAR:
            {
                if (objc!=3)
                {
                    Tcl_WrongNumArgs(interp, 2, objv, "device");
                    return TCL_ERROR;
                }
                if (l_unset(Tcl_GetString(objv[2])))
                {
                    return TCL_OK;
                }  else  {
                    Tcl_SetResult(interp, "Unable to complete", NULL);
                    return TCL_ERROR;
                }
            }
            break;
    }
    
    return TCL_OK;
}

static int l_set(char *device, char *file, int offset, int mode)
{
    int dfd, ffd, rc = 0;
    struct loop_info li;
    
    memset(&li,0,sizeof(li));
    
    strncpy(li.lo_name, file, LO_NAME_SIZE);
    li.lo_offset = offset;

    li.lo_encrypt_key_size = 0;

    if ((dfd = open(device, mode)) >= 0)
    {
        if ((ffd = open(file, mode)) >= 0)
        {
            if (ioctl (dfd, LOOP_SET_FD, ffd) >= 0) {
                if (ioctl (dfd, LOOP_SET_STATUS, &li) >= 0) {
                    rc = 1;
                }  else  {
                    ioctl (dfd, LOOP_CLR_FD, 0);
                }
            }
            close(ffd);
        }
        close(dfd);
    }
    return rc;
}

static int l_unset(char *device)
{
    int dfd, rc = 0;
    if ((dfd = open(device, O_RDONLY)) >= 0)
    {
        if (ioctl (dfd, LOOP_CLR_FD, 0) >= 0)
            rc = 1;
        close(dfd);
    }
    return rc;
}

