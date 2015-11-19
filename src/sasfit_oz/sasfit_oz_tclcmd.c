/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   Tcl Wrapper by Ingo Bressler (ingo.bressler@bam.de)
 *   29.09.2013
 *   modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   02.07.2014
 */

#include <string.h>
#include <sasfit_oz.h>
#include <sasfit_analytpar.h>


#include "../sasfit_old/include/sasfit.h"
#include "../sasfit_old/include/SASFIT_nr.h"
#include "../sasfit_old/include/SASFIT_x_tcl.h"
#include "../sasfit_old/include/SASFIT_resolution.h"
#include <sasfit_common.h>
#include <sasfit_plugin.h>
#include <sasfit_plugin_backend.h>

#define OZMAXPAR 16
#define PUTS(format, ...) sasfit_out(format, __VA_ARGS__)

double U_ZERO(double d, double temp, double *p) {
return 0.0;
}
#define OZMAXCLOSURES 34
int
assign_closure(const char * token, sasfit_oz_data * OZD)
{
    const char * ClosureNames[OZMAXCLOSURES];
    int i,eq;
    if (!token || !OZD) return 0;

    ClosureNames[0] = "Percus-Yevick";
    ClosureNames[1] = "PY";
    ClosureNames[2] = "Hypernetted-Chain";
    ClosureNames[3] = "HNC";
    ClosureNames[4] = "Rogers-Young";
    ClosureNames[5] = "RY";
    ClosureNames[6] = "Verlet";
    ClosureNames[7] = "V";
    ClosureNames[8] = "Martynov-Sarkisov";
    ClosureNames[9] = "MS";
    ClosureNames[10] = "Ballone-Pastore-Galli-Gazzillo";
    ClosureNames[11] = "BPGG";
    ClosureNames[12] = "Mean-Spherical Approximation";
    ClosureNames[13] = "MSA";
    ClosureNames[14] = "modified Mean-Spherical Approximation";
    ClosureNames[15] = "mMSA";
    ClosureNames[16] = "Reference HNC";
    ClosureNames[17] = "RHNC";
    ClosureNames[18] = "Soft-Core MSA";
    ClosureNames[19] = "SMSA";
    ClosureNames[20] = "HNCSMSA";
    ClosureNames[21] = "HMSA";
    ClosureNames[22] = "Choudhury-Gosh";
    ClosureNames[23] = "CG";
    ClosureNames[24] = "Chapentier-Jakse";
    ClosureNames[25] = "CJVM";
    ClosureNames[26] = "Bomont-Brettomnet";
    ClosureNames[27] = "BB";
    ClosureNames[28] = "Vompe-Martynov";
    ClosureNames[29] = "VM";
    ClosureNames[30] = "Duh-Haymet";
    ClosureNames[31] = "DH";
    ClosureNames[32] = "rescaled Mean-Spherical Approximation";
    ClosureNames[33] = "RMSA";
    i=0;
    eq=-1;
    while (i<OZMAXCLOSURES && eq != 0) {
        eq = strcmp(token,ClosureNames[i]);
        i++;
    }
    PUTS("%s %d\n",token,i-1);
//    PY, HNC, RY, Verlet, MS, BPGG
    switch (i-1) {
        case 0 :
        case 1 :
            OZD->cl=PY;
            break;
        case 2 :
        case 3 :
            OZD->cl=HNC;
            break;
        case 4 :
        case 5 :
            OZD->cl=RY;
            break;
        case 6 :
        case 7 :
            OZD->cl=Verlet;
            break;
        case 8 :
        case 9 :
            OZD->cl=MS;
            break;
        case 10 :
        case 11 :
            OZD->cl=BPGG;
            break;
        case 12 :
        case 13 :
            OZD->cl=MSA;
            break;
        case 14 :
        case 15 :
            OZD->cl=mMSA;
            break;
        case 16 :
        case 17 :
            OZD->cl=RHNC;
            break;
        case 18 :
        case 19 :
            OZD->cl=SMSA;
            break;
        case 20 :
        case 21 :
            OZD->cl=HMSA;
            break;
        case 22 :
        case 23 :
            OZD->cl=CG;
            break;
        case 24 :
        case 25 :
            OZD->cl=CJVM;
            break;
        case 26 :
        case 27 :
            OZD->cl=BB;
            break;
        case 28 :
        case 29 :
            OZD->cl=VM;
            break;
        case 30 :
        case 31 :
            OZD->cl=DH;
            break;
        case 32 :
        case 33 :
            OZD->cl=RMSA;
            break;
        default :
            OZD->cl=PY;
            sasfit_err("Closure not found: %s\n", token);
            return 0;
            break;
    }
    return 1;
}

int
assign_root_Algorithm(const char * token, sasfit_oz_data * OZD)
{
#define MAXROOTALGORITHMS 23
    const char * RootAlgorithms[MAXROOTALGORITHMS];
    int i,eq;
    if (!token || !OZD) return 0;
    RootAlgorithms[0] = "Picard iteration";
    RootAlgorithms[1] = "Mann iteration";
    RootAlgorithms[2] = "Ishikawa iteration";
    RootAlgorithms[3] = "Noor iteration";
    RootAlgorithms[4] = "S iteration";
    RootAlgorithms[5] = "SP iteration";
    RootAlgorithms[6] = "CR iteration";
    RootAlgorithms[7] = "Picard-S iteration";
    RootAlgorithms[8] = "PMH iteration";
    RootAlgorithms[9] = "Mann II iteration";
    RootAlgorithms[10] = "Krasnoselskij iteration";
    RootAlgorithms[11] = "S* iteration";
    RootAlgorithms[12] = "Steffensen iteration";
    RootAlgorithms[13] = "Anderson mixing";
    RootAlgorithms[14] = "GMRES" ;
    RootAlgorithms[15] = "Bi-CGStab" ;
    RootAlgorithms[16] = "TFQMR" ;
    RootAlgorithms[17] = "FGMRES";
    RootAlgorithms[18] = "KINSOL_FP";
    RootAlgorithms[MAXROOTALGORITHMS-1] = "dNewton";
    RootAlgorithms[MAXROOTALGORITHMS-2] = "Hybrid";
    RootAlgorithms[MAXROOTALGORITHMS-3] = "Hybrids (int. sc.)";
    RootAlgorithms[MAXROOTALGORITHMS-4] = "Broyden";

    i=0;
    eq=-1;
    while (i<MAXROOTALGORITHMS && eq != 0) {
        eq = strcmp(token,RootAlgorithms[i]);
        i++;
    }
    PUTS("%s %d\n",token,i-1);

    switch (i-1) {
        case 0 :
            OZD->root_algorithm=Picard_iteration;
            break;
        case 1 :
            OZD->root_algorithm=Mann_iteration;
            break;
        case 2 :
            OZD->root_algorithm=Ishikawa_iteration;
            break;
        case 3 :
            OZD->root_algorithm=Noor_iteration;
            break;
        case 4 :
            OZD->root_algorithm=S_iteration;
            break;
        case 5 :
            OZD->root_algorithm=SP_iteration;
            break;
        case 6 :
            OZD->root_algorithm=CR_iteration;
            break;
        case 7 :
            OZD->root_algorithm=PicardS_iteration;
            break;
        case 8 :
            OZD->root_algorithm=PMH_iteration;
            break;
        case 9 :
            OZD->root_algorithm=MannII_iteration;
            break;
        case 10 :
            OZD->root_algorithm=Krasnoselskij_iteration;
            break;
        case 11 :
            OZD->root_algorithm=Sstar_iteration;
            break;
        case 12 :
            OZD->root_algorithm=Steffensen2_iteration;
            break;
        case 13 :
            OZD->root_algorithm=AndersonAcc;
            break;
        case 14 :
            OZD->root_algorithm=GMRES;
            break;
        case 15 :
            OZD->root_algorithm=BiCGSTAB;
            break;
        case 16 :
            OZD->root_algorithm=TFQMR;
            break;
        case 17 :
            OZD->root_algorithm=FGMRES;
            break;
        case 18 :
            OZD->root_algorithm=KINSOLFP;
            break;
        case MAXROOTALGORITHMS-1 :
            OZD->root_algorithm=dNewton;
            break;
        case MAXROOTALGORITHMS-2 :
            OZD->root_algorithm=Hybrid;
            break;
        case MAXROOTALGORITHMS-3 :
            OZD->root_algorithm=Hybrids;
            break;
        case MAXROOTALGORITHMS-4 :
            OZD->root_algorithm=Broyden;
            break;
        default :
            OZD->root_algorithm=S_iteration;
            sasfit_out("Root finding Algorithm not found: %s. Using S iteration instead.\n", token);
            sasfit_err("Root finding Algorithm not found: %s. Using S iteration instead.\n", token);
            break;
    }
    if (i<=MAXROOTALGORITHMS) return 1;
    return 0;
}

int
assign_pot(const char * token, sasfit_oz_data * OZD)
{
    #define MAXPOTENTIALS 28
    const char * PotentialNames[MAXPOTENTIALS];
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
    PotentialNames[10] = "DepletionOfSpheresBySpheres";
    PotentialNames[11] = "Depl-Sph-Sph";
    PotentialNames[12] = "DepletionOfSpheresByDiscs";
    PotentialNames[13] = "Depl-Sph-Discs";
    PotentialNames[14] = "DepletionOfSpheresByRods";
    PotentialNames[15] = "Depl-Sph-Rods";
    PotentialNames[16] = "DLVO";
    PotentialNames[17] = "PSM";
    PotentialNames[18] = "GGCM-n";
    PotentialNames[19] = "StarPolymer (f>10)";
    PotentialNames[20] = "StarPolymer (f<10)";
    PotentialNames[21] = "HS 3Yukawa";
    PotentialNames[22] = "SquareWell";
    PotentialNames[23] = "SW";
    PotentialNames[24] = "Fermi";
    PotentialNames[25] = "FDM";
    PotentialNames[26] = "PenetrableSphere";
    PotentialNames[27] = "PSM";


    i=0;
    eq=-1;
    while (i<MAXPOTENTIALS && eq != 0) {
        eq = strcmp(token,PotentialNames[i]);
        i++;
    }
    if (i== MAXPOTENTIALS) {
        PUTS("the potential >%s< is unknown\n",token,i-1);
        return 0;
    } else {
        PUTS("potential name:%s, index:%d\n",token,i-1);
    }

    switch (i-1) {
        case 0 :
        case 1 :
            OZD->potential=&U_Hard_Sphere;
            OZD->reference_pot=&U_Hard_Sphere;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_Hard_Sphere;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_Hard_Sphere;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 2 :
        case 3 :
            OZD->potential=&U_Sticky_Hard_Sphere;
            OZD->reference_pot=&U_Ref_Sticky_Hard_Sphere;
            OZD->pertubation_pot=&U_Pert_Sticky_Hard_Sphere;
            OZD->repulsive_pot=&U_R_Sticky_Hard_Sphere;
            OZD->attractive_pot=&U_A_Sticky_Hard_Sphere;
            OZD->shortrange_pot=&U_SR_Sticky_Hard_Sphere;
            OZD->longrange_pot=&U_LR_Sticky_Hard_Sphere;
            break;
        case 4 :
        case 5 :
            OZD->potential=&U_Soft_Sphere;
            OZD->reference_pot=&U_Soft_Sphere;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_Soft_Sphere;
            OZD->shortrange_pot=&U_Soft_Sphere;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 6 :
        case 7 :
            OZD->potential=&U_Lennard_Jones;
            OZD->reference_pot=&U_Lennard_Jones;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_SR_Lennard_Jones;
            OZD->attractive_pot=&U_LR_Lennard_Jones;
            OZD->shortrange_pot=&U_SR_Lennard_Jones;
            OZD->longrange_pot=&U_LR_Lennard_Jones;
            break;
        case 8 :
        case 9 :
            OZD->potential=&U_Ionic_Microgel;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 10 :
        case 11 :
            OZD->potential=&U_DepletionOfSpheresBySpheres;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 12 :
        case 13 :
            OZD->potential=&U_DepletionOfSpheresByDiscs;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 14 :
        case 15 :
            OZD->potential=&U_DepletionOfSpheresByRods;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 16 :
            OZD->potential=&U_DLVO;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 17 :
            OZD->potential=&U_PSM;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 18 :
            OZD->potential=&U_GGCM_n;
            OZD->reference_pot=&U_ZERO;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_ZERO;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_ZERO;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 19 :
            OZD->potential=&U_Star1;
            OZD->reference_pot=&U_Star1;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_Star1;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_Star1;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 20 :
            OZD->potential=&U_Star2;
            OZD->reference_pot=&U_Star2;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_Star2;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_Star2;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 21 :
            OZD->potential=&U_HS_3Yukawa;
            OZD->reference_pot=&U_HS_3Yukawa;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_HS_3Yukawa;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_HS_3Yukawa;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 22 :
        case 23 :
            OZD->potential=&U_Square_Well_Sphere;
            OZD->reference_pot=&U_Ref_Square_Well_Sphere;
            OZD->pertubation_pot=&U_Pert_Square_Well_Sphere;
            OZD->repulsive_pot=&U_R_Square_Well_Sphere;
            OZD->attractive_pot=&U_A_Square_Well_Sphere;
            OZD->shortrange_pot=&U_SR_Square_Well_Sphere;
            OZD->longrange_pot=&U_LR_Square_Well_Sphere;
            break;
        case 24 :
        case 25 :
            OZD->potential=&U_FDM;
            OZD->reference_pot=&U_FDM;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_FDM;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_FDM;
            OZD->longrange_pot=&U_ZERO;
            break;
        case 26 :
        case 27 :
            OZD->potential=&U_PSM;
            OZD->reference_pot=&U_PSM;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_PSM;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_PSM;
            OZD->longrange_pot=&U_ZERO;
            break;
        default :
            OZD->potential=&U_Hard_Sphere;
            OZD->reference_pot=&U_Hard_Sphere;
            OZD->pertubation_pot=&U_ZERO;
            OZD->repulsive_pot=&U_Hard_Sphere;
            OZD->attractive_pot=&U_ZERO;
            OZD->shortrange_pot=&U_Hard_Sphere;
            OZD->longrange_pot=&U_ZERO;
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
    int i,status;
    char paramName[4];
    double tmp;
    double pPot[OZMAXPAR];
    sasfit_oz_data ozd;
        // init target data structure
        i=0;

        ozd.interp = interp;
        ozd.interrupt = 0;

        if ( objc < 2 ) return TCL_OK;
        Tcl_Obj * oz_obj = objv[1];
        ozd.oz_obj = objv[1];
        const char * ozname = Tcl_GetStringFromObj(oz_obj, 0);
        ozd.pPot = pPot;

        // get input parameters from Tcl Interpreter

        ozd.Npoints = 4096;
        int grid, factor;

        if (!GET_TCL(int, &ozd.PrintProgress, "PrintProgress")) {
                ozd.PrintProgress = 0;
        }

        if (GET_TCL(int, &factor, "mindimOZ") && GET_TCL(int, &grid, "mult"))
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

        status = assign_closure(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, ozname, "closure"), 0),
                       &ozd);
        if (status == 0) {
                sasfit_err("Unknown closure\n");
                return TCL_ERROR;
        }

        status = assign_pot(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, ozname, "potential"), 0),
                   &ozd);
        if (status == 0) {
                sasfit_err("Unknown Potential\n");
                return TCL_ERROR;
        }

        status = assign_root_Algorithm(Tcl_GetStringFromObj(sasfit_tcl_get_obj(interp, ozname, "algorithm"), 0),
                   &ozd);
        if (status == 0) {
                sasfit_err("Unknown Root finding Algorithm\n");
                return TCL_ERROR;
        }

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
        }
        for (i = 0; i < 6; i++) PUTS("param %d: %f\n", i, ozd.pPot[i]);
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
        Tcl_Obj * hx = Tcl_NewListObj(0, 0);
        Tcl_Obj * hy = Tcl_NewListObj(0, 0);
        Tcl_Obj * ux = Tcl_NewListObj(0, 0);
        Tcl_Obj * uy = Tcl_NewListObj(0, 0);
        Tcl_Obj * bx = Tcl_NewListObj(0, 0);
        Tcl_Obj * by = Tcl_NewListObj(0, 0);
        Tcl_Obj * yx = Tcl_NewListObj(0, 0);
        Tcl_Obj * yy = Tcl_NewListObj(0, 0);
        Tcl_Obj * fx = Tcl_NewListObj(0, 0);
        Tcl_Obj * fy = Tcl_NewListObj(0, 0);
        Tcl_Obj * gammax = Tcl_NewListObj(0, 0);
        Tcl_Obj * gammay = Tcl_NewListObj(0, 0);

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
                APPEND(h, ozd.r[i], ozd.h[i]);
                APPEND(u, ozd.r[i], ozd.ubeta[i]);
                APPEND(b, ozd.r[i], ozd.Br[i]);
                APPEND(y, ozd.r[i], ozd.yr[i]);
                APPEND(gamma, ozd.r[i], ozd.G[i]);
                APPEND(f, ozd.r[i], ozd.fr[i]);
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
        SET(ozname, "res,h,x", hx);
        SET(ozname, "res,h,y", hy);
        SET(ozname, "res,u,x", ux);
        SET(ozname, "res,u,y", uy);
        SET(ozname, "res,B,x", bx);
        SET(ozname, "res,B,y", by);
        SET(ozname, "res,y,x", yx);
        SET(ozname, "res,y,y", yy);
        SET(ozname, "res,f,x", fx);
        SET(ozname, "res,f,y", fy);
        SET(ozname, "res,gamma,x", gammax);
        SET(ozname, "res,gamma,y", gammay);

        OZ_free(&ozd);
        return TCL_OK;
}

int sasfit_oz_assign_data_sq_cmd(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *CONST objv[])
{

        sasfit_oz_data ozd;
        int i;
        Tcl_Obj * oz_obj;
        Tcl_Obj **oz_objvPtr_q, **oz_objvPtr_Sq;
        const char * sqname;
        int lq, lSq;
        double tmp;
        static double *q=NULL, *Sq=NULL;
        int funcid;
        sasfit_param param;
        const sasfit_plugin_func_t *func_descr=NULL;
        const sasfit_plugin_func_t *curr_func=NULL;

        if (objc < 2) {
                PUTS("usage: sasfit_oz_assign_data <sqfunc> <{q*sigma}> <{S(q*sigma)}>",0);
                return TCL_ERROR;
        }
        if (objc < 4) {
                PUTS("usage: sasfit_oz_assign_data <sqfunc> <{q*sigma}> <{S(q*sigma)}>",0);
                return TCL_ERROR;
        }

        oz_obj = objv[1];
        sqname = Tcl_GetStringFromObj(oz_obj, 0);

        Tcl_ListObjGetElements(interp,objv[2],&lq,&oz_objvPtr_q);
        Tcl_ListObjGetElements(interp,objv[3],&lSq,&oz_objvPtr_Sq);
        if (lq != lSq) {
                PUTS("<q*sigma> and <S(q*sigma)> need to have the same length",0);
                return TCL_ERROR;
        }
        if (q==NULL) free(q);
        if (Sq==NULL) free(Sq);
        q = (double *)malloc(sizeof(double)*lq);
        Sq = (double *)malloc(sizeof(double)*lq);
        for (i=0; i<lq; i++) {
            if (Tcl_GetDoubleFromObj(interp,oz_objvPtr_q[i],&q[i])!=TCL_OK) {
                PUTS("could not read element %d of the list q",i);
                return TCL_ERROR;
            }
 //           PUTS("element q(%d):%f\n",i,q[i]);
        }

        for (i=0; i<lq; i++) {
            if (Tcl_GetDoubleFromObj(interp,oz_objvPtr_Sq[i],&Sq[i])!=TCL_OK) {
                PUTS("could not read element %d of the list Sq",i);
                return TCL_ERROR;
            }
 //           PUTS("element S(q(%d)):%f\n",i,Sq[i]);
        }

        sasfit_plugin_func_t * cur_func = NULL;
        i=0;
        do {
            curr_func = sasfit_plugin_db_get_by_id(i);
            i++;
            if (curr_func !=NULL) {
 //                   sasfit_out("%d: >%s<\n",i-1,curr_func->name);
                    if (strcmp(sqname,curr_func->name)==0) i = -(i-1);
            }
        } while (curr_func != NULL && i>0);

        PUTS("%d: >%s<:>%s<\n",-i,sqname,curr_func->name);

        if (curr_func != NULL ) {
			sasfit_init_param( &param );
			param.xarr=q;
			param.yarr=Sq;
            tmp=curr_func->func_v(1,&param,lq);
        } else {
            return TCL_ERROR;
        }
        return TCL_OK;
}

