#include <unistd.h>
#include <uutil.h>

static int execveCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

void uu_execv_setup(Tcl_Interp *interp)
{
    Tcl_CreateObjCommand(interp, "::uutil::execve", execveCmd, NULL, NULL);
}

static int execveCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj **aObj, **eObj;
    char **argp, **envp, *str,*vstr;
    int i,alen,elen,ok,ss,vss;
    unsigned long sz;

    if (objc!=4)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "fileName parameters env");
        return TCL_ERROR;
    }

    if (Tcl_ListObjGetElements(interp, objv[2], &alen, &aObj) != TCL_OK)
        return TCL_ERROR;
    
    if (Tcl_ListObjGetElements(interp, objv[3], &elen, &eObj) != TCL_OK)
        return TCL_ERROR;

    elen = elen / 2;

    sz = (alen + 2) * sizeof(Tcl_Obj *);
    if ((argp = (char **) Tcl_Alloc(sz)) != NULL)
    {
        memset(argp, 0, sz);
        ok = 1;
        argp[0] = Tcl_GetString(objv[1]);
        for (i=0; i<alen; i++)
        {
            str = Tcl_GetStringFromObj(aObj[i],&ss);
            if ((argp[i+1] = (char *) Tcl_Alloc((unsigned long) (ss + 1))) == NULL)
            {
                ok = 0;
                break;
            }
            strcpy(argp[i], str);
        }
        if (ok)
        {
            sz = (elen + 1) * sizeof(Tcl_Obj *);
            if ((envp = (char **) Tcl_Alloc(sz)) != NULL)
            {
                memset(envp, 0, sz);
                ok = 1;
                for (i=0; i<elen; i++)
                {
                    str = Tcl_GetStringFromObj(eObj[i*2],&ss);
                    vstr = Tcl_GetStringFromObj(eObj[i*2+1],&vss);
                    if ((envp[i] = (char *) Tcl_Alloc((unsigned long) (ss + vss + 1))) == NULL)
                    {
                        ok = 0;
                        break;
                    }
                    sprintf(envp[i],"%s=%s",str,vstr);
                }
                if (ok)
                {
                    execve(Tcl_GetString(objv[1]), argp, envp);
                }
                for (i=0; i<elen; i++)
                {
                    if (envp[i])
                        Tcl_Free((void *) envp[i]);
                }
                Tcl_Free((void *) envp);
            }
        }
        for (i=1; i<alen; i++)
        {
            if (argp[i])
                Tcl_Free((void *) argp[i]);
        }
        Tcl_Free((void *) argp);
    }
    
    return TCL_ERROR;
}

