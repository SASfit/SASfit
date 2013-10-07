/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   27.9.2013
 *   Tcl Wrapper by Ingo Bressler (ingo.bressler@bam.de)
 *   29.09.2013
 */

#include <string.h>
#include <sasfit_oz.h>
#include <sasfit_analytpar.h>

#define OZMAXPAR 16
#define PUTS(format, ...) sasfit_out(format, __VA_ARGS__)

int
assign_closure(const char * token, sasfit_oz_data * OZD)
{
    const char * ClosureNames[12];
    int i,eq;
    if (!token || !OZD) return 0;

    ClosureNames[0] = "Percus-Yevick";
    ClosureNames[1] = "PY";
    ClosureNames[2] = "hypernetted-chain";
    ClosureNames[3] = "HNC";
    ClosureNames[4] = "Rogers-Young";
    ClosureNames[5] = "RY";
    ClosureNames[6] = "Verlet";
    ClosureNames[7] = "V";
    ClosureNames[8] = "Martynov-Sarkisov";
    ClosureNames[9] = "MS";
    ClosureNames[10] = "Ballone-Pastore-Galli-Gazzillo";
    ClosureNames[11] = "BPGG";

    i=0;
    eq=-1;
    while (i<12 && eq != 0) {
        eq = strcmp(token,ClosureNames[i]);
        i++;
    }
    PUTS("%s %d\n",token,i-1);
//    PY, HNC, RY, Verlet, MS, BPGG
    switch (i-1) {
        case 0 :
            OZD->cl=PY;
            break;
        case 1 :
            OZD->cl=PY;
            break;
        case 2 :
            OZD->cl=HNC;
            break;
        case 3 :
            OZD->cl=HNC;
            break;
        case 4 :
            OZD->cl=RY;
            break;
        case 5 :
            OZD->cl=RY;
            break;
        case 6 :
            OZD->cl=Verlet;
            break;
        case 7 :
            OZD->cl=Verlet;
            break;
        case 8 :
            OZD->cl=MS;
            break;
        case 9 :
            OZD->cl=MS;
            break;
        case 10 :
            OZD->cl=BPGG;
            break;
        case 11 :
            OZD->cl=BPGG;
            break;
        default :
            OZD->cl=PY;
            sasfit_err("Closure not found: %s\n", token);
            break;
    }
    return 1;
}

int
assign_pot(const char * token, sasfit_oz_data * OZD)
{
    const char * PotentialNames[16];
    int i,eq;
    if (!token || !OZD) return 0;
    PotentialNames[0] = "HardSphere";
    PotentialNames[1] = "HS";
    PotentialNames[2] = "StickyHardSphere";
    PotentialNames[3] = "SHS";
    PotentialNames[4] = "SoftSphere";
    PotentialNames[5] = "SS";
    PotentialNames[6] = "LennardJones";
    PotentialNames[7] = "LJ";
    PotentialNames[8] = "IonicMicrogel";
    PotentialNames[9] = "IM";
    PotentialNames[10] = "Depletion";
    PotentialNames[11] = "D";
    PotentialNames[12] = "LennardJones";
    PotentialNames[13] = "LJ";
    PotentialNames[13] = "DLVO";
    PotentialNames[14] = "PSM";
    PotentialNames[15] = "GGCM-n";

    i=0;
    eq=-1;
    while (i<16 && eq != 0) {
        eq = strcmp(token,PotentialNames[i]);
        i++;
    }
    PUTS("%s %d\n",token,i-1);
    switch (i-1) {
        case 0 :
            OZD->potential=&U_Hard_Sphere;
            break;
        case 1 :
            OZD->potential=&U_Hard_Sphere;
            break;
        case 2 :
            OZD->potential=&U_Sticky_Hard_Sphere;
            break;
        case 3 :
            OZD->potential=&U_Sticky_Hard_Sphere;
            break;
        case 4 :
            OZD->potential=&U_Soft_Sphere;
            break;
        case 5 :
            OZD->potential=&U_Soft_Sphere;
            break;
        case 6 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 7 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 8 :
            OZD->potential=&U_Ionic_Microgel;
            break;
        case 9 :
            OZD->potential=&U_Ionic_Microgel;
            break;
        case 10 :
            OZD->potential=&U_Depletion;
            break;
        case 11 :
            OZD->potential=&U_Depletion;
            break;
        case 12 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 13 :
            OZD->potential=&U_Lennard_Jones;
            break;
        case 14 :
            OZD->potential=&U_DLVO;
            break;
        case 15 :
            OZD->potential=&U_PSM;
            break;
        case 16 :
            OZD->potential=&U_GGCM_n;
            break;
        default :
            OZD->potential=&U_Hard_Sphere;
            break;
    }
    return 1;
}

#define GET_TCL(val_type, target, src_name) (sasfit_tcl_get_ ## val_type(interp, target, ozname, src_name) == TCL_OK)

int sasfit_oz_calc_cmd(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *CONST objv[])
{
    int i=0;
    char paramName[4];
    double tmp;
    double pPot[OZMAXPAR];
        // init target data structure
        sasfit_oz_data ozd;

        if ( objc < 2 ) return TCL_OK;
        Tcl_Obj * oz_obj = objv[1];
        const char * ozname = Tcl_GetStringFromObj(oz_obj, 0);
        ozd.pPot = pPot;

        // get input parameters from Tcl Interpreter

        ozd.Npoints = 4096;
        int grid, factor;
        if (GET_TCL(int, &factor, "1024") && GET_TCL(int, &grid, "mult"))
        {
                ozd.Npoints = factor * grid;
        }
        PUTS("Grid length has been set to %d.\n", ozd.Npoints);

        if (!GET_TCL(int, &ozd.maxsteps, "maxit")) {
                ozd.maxsteps = 1000;
        }
        PUTS("Maximum number of iterations has been set to %d.\n", ozd.maxsteps);

        if (!GET_TCL(double, &ozd.relerror, "releps")) {
                ozd.relerror = 1e-15;
        }
        PUTS("Relative iteration precision has been set to %g.\n", ozd.relerror);

        if (!GET_TCL(double, &ozd.dr_dsigma, "dr/dsigma")) {
                ozd.dr_dsigma = 0.005;
        }
        PUTS("Relative grid width will be set to %g.\n", ozd.dr_dsigma);

        if (!GET_TCL(double, &ozd.mixcoeff, "mix")) {
                ozd.mixcoeff = 1;
        }
        PUTS("The mixing coefficient alpha for weighting new and old c(r)\n",
             "will be set to %f.\n",
             "c_next(r) = alpha*c_new(r)+(1-alpha)c_old(r)\n",ozd.mixcoeff);

        assign_closure(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, ozname, "closure"), 0),
                       &ozd);
        assign_pot(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, ozname, "potential"), 0),
                   &ozd);

        for (i = 0; i < OZMAXPAR; i++) {
            sprintf(paramName, "p%d", i);
            if (!GET_TCL(double, &ozd.pPot[i], paramName)) {
                if (i == 0) {
                    ozd.pPot[i] = 1.0;
                } else {
                    ozd.pPot[i] = 0.0;
                }
            }
 //             ozd.mixcoeff = 1;
            PUTS("param %d: %f\n", i, ozd.pPot[i]);
        }
        ozd.dr = ozd.dr_dsigma * ozd.pPot[0];

        if (!GET_TCL(double, &ozd.T, "T")) {
                ozd.T = 300.;
        }
        PUTS("Temperature is set to %gK\n", ozd.T);

        if (!GET_TCL(double, &ozd.phi, "phi")) {
                ozd.phi = .3;
        }
        PUTS("Volume fraction is set to %g\n", ozd.phi);

        // calulate
        OZ_init(&ozd);
        OZ_calculation(&ozd);

        Tcl_Obj * sx = Tcl_NewListObj(0, 0);
        Tcl_Obj * sy = Tcl_NewListObj(0, 0);
        Tcl_Obj * gx = Tcl_NewListObj(0, 0);
        Tcl_Obj * gy = Tcl_NewListObj(0, 0);
        Tcl_Obj * cx = Tcl_NewListObj(0, 0);
        Tcl_Obj * cy = Tcl_NewListObj(0, 0);
        Tcl_Obj * ux = Tcl_NewListObj(0, 0);
        Tcl_Obj * uy = Tcl_NewListObj(0, 0);

        #define APPEND(obj, xval, yval) \
                Tcl_ListObjAppendElement(interp, obj ## x, Tcl_NewDoubleObj(xval)); \
                Tcl_ListObjAppendElement(interp, obj ## y, Tcl_NewDoubleObj(yval))

        APPEND(s, 0.0, ozd.Sq0);
        APPEND(g, 0.0, ozd.gr0);
        APPEND(c, 0.0, ozd.cr0);
        for (int i = 0; i < ozd.Npoints; i++) {
                APPEND(s, ozd.k[i], ozd.S[i]);
                APPEND(g, ozd.r[i], ozd.g[i]);
                APPEND(c, ozd.r[i], ozd.c[i]);
                APPEND(u, ozd.r[i], ozd.ubeta[i]);
        }

        #define SET(first, second, obj) \
                Tcl_ObjSetVar2(interp, \
                               Tcl_NewStringObj(first, -1), \
                               Tcl_NewStringObj(second, -1), \
                               obj, 0)

        SET(ozname, "res,s,x", sx);
        SET(ozname, "res,s,y", sy);
        SET(ozname, "res,g,x", gx);
        SET(ozname, "res,g,y", gy);
        SET(ozname, "res,c,x", cx);
        SET(ozname, "res,c,y", cy);
        SET(ozname, "res,u,x", ux);
        SET(ozname, "res,u,y", uy);

        OZ_free(&ozd);
        return TCL_OK;
}

