/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>


void set_ki(ordered_particles_param *ospparam, sasfit_param *param)
{
    scalar k;

    k=2.0*M_PI/LAMBDA;

    KI[0]= 0.0;
    KI[1]= 0.0;
    KI[2]=-k;
}

void set_ks(ordered_particles_param *ospparam, sasfit_param *param)
{
    scalar k;

    k=2.0*M_PI/LAMBDA;

    KS[0]=k*cos(VARPHI)*sin(VARTHETA);
    KS[1]=k*sin(VARPHI)*sin(VARTHETA);
    KS[2]=-k*cos(VARTHETA);
}

void set_q(ordered_particles_param *ospparam, sasfit_param *param)
{
    scalar k;

    k=2.0*M_PI/LAMBDA;

    Q[0]=k*cos(VARPHI)*sin(VARTHETA);
    Q[1]=k*sin(VARPHI)*sin(VARTHETA);
    Q[2]=k*(1-cos(VARTHETA));   // 1-cos(theta) = 2 sin^2(theta/2)

    if (fabs(QMOD - 2.0*k*sin(VARTHETA/2.0)) > 1e-8) sasfit_out("Qmod might by not precise enough\n");;

 //   QMOD = 2.0*k*sin(VARTHETA/2.0);

}

void set_l(ordered_particles_param *ospparam, sasfit_param *param)
{
    L[0]=D*cos(VARPHI)*tan(VARTHETA);
    L[1]=D*sin(VARPHI)*tan(VARTHETA);
    L[2]=0;
    LMOD = D*tan(VARTHETA);
}

scalar set_q_via_l(ordered_particles_param *ospparam, sasfit_param *param)
{
    scalar k;

    k=2.0*M_PI/LAMBDA;

    Q[0]=k*L[0]/D*cos(VARTHETA);
    Q[1]=k*L[1]/D*cos(VARTHETA);
    Q[2]=k*(1-cos(VARTHETA));
    if (fabs(QMOD - 2.0*k*sin(VARTHETA/2.0)) > 1e-8) sasfit_out("Qmod might by not precise enough\n");;
}

void ops_setConvention (ordered_particles_param *opsparam, ops_Euler_convention_t convention)
{
    ops_rot *opsd;
    opsd= &opsparam->EulerR;
    opsd->convention = convention;
}

void ops_setEulerAngles(ordered_particles_param *opsparam, double alpha, double beta,double gamma)
{
    ops_rot *opsd;
    opsd= &opsparam->EulerR;
    opsd->EulerAngles[2] = alpha;  // [0, 2 pi]
    opsd->EulerAngles[1] = beta;   // [0, pi]
    opsd->EulerAngles[0] = gamma;  // [0, 2 pi]
}

void ops_setRotationMatrix(ordered_particles_param *opsparam) {
    double s[3], c[3];
    int i,j;
    ops_rot *opsd;
    opsd= &opsparam->EulerR;

    for (i=0;i<3;i++) {
        s[i] = sin(opsd->EulerAngles[i]);
        c[i] = cos(opsd->EulerAngles[i]);
    }
    switch (opsd->convention) {
// proper Euler Angles:
        case X1_Z2_X3 : {
                        opsd->RotationMatrix[0][0] = c[1];
                        opsd->RotationMatrix[1][0] = c[0]*s[1];
                        opsd->RotationMatrix[2][0] = s[0]*s[1];

                        opsd->RotationMatrix[0][1] = -c[2]*s[1];
                        opsd->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opsd->RotationMatrix[2][1] =  c[0]*s[2]     +c[1]*c[2]*s[0];

                        opsd->RotationMatrix[0][2] =  s[1]*s[2];
                        opsd->RotationMatrix[1][2] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opsd->RotationMatrix[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case X1_Y2_X3 : {
                        opsd->RotationMatrix[0][0] =  c[1];
                        opsd->RotationMatrix[1][0] =  s[0]*s[1];
                        opsd->RotationMatrix[2][0] = -c[0]*s[1];

                        opsd->RotationMatrix[0][1] =  s[1]*s[2];
                        opsd->RotationMatrix[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opsd->RotationMatrix[2][1] =  c[2]*s[0]+c[0]*c[1]*s[2];

                        opsd->RotationMatrix[0][2] =  c[2]*s[1];
                        opsd->RotationMatrix[1][2] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opsd->RotationMatrix[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_X2_Y3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opsd->RotationMatrix[1][0] =  s[1]*s[2];
                        opsd->RotationMatrix[2][0] = -c[2]*s[0]-c[0]*c[1]*s[2];

                        opsd->RotationMatrix[0][1] =  s[0]*s[1];
                        opsd->RotationMatrix[1][1] =  c[1];
                        opsd->RotationMatrix[2][1] =  c[0]*s[1];

                        opsd->RotationMatrix[0][2] =  c[0]*s[2]+c[1]*c[2]*s[0];
                        opsd->RotationMatrix[1][2] = -c[2]*s[1];
                        opsd->RotationMatrix[2][2] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        break;
                        }
        case Y1_Z2_Y3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opsd->RotationMatrix[1][0] =  c[2]*s[1];
                        opsd->RotationMatrix[2][0] = -c[0]*s[2]-c[1]*c[2]*s[0];

                        opsd->RotationMatrix[0][1] = -c[0]*s[1];
                        opsd->RotationMatrix[1][1] =  c[1];
                        opsd->RotationMatrix[2][1] =  s[0]*s[1];

                        opsd->RotationMatrix[0][2] =  c[2]*s[0]+c[0]*c[1]*s[2];
                        opsd->RotationMatrix[1][2] =  s[1]*s[2];
                        opsd->RotationMatrix[2][2] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        break;
                        }
        case Z1_X2_Z3 : { // x-convention
                        opsd->RotationMatrix[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opsd->RotationMatrix[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opsd->RotationMatrix[2][0] = s[1]*s[2];

                        opsd->RotationMatrix[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opsd->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opsd->RotationMatrix[2][1] =  c[2]*s[1];

                        opsd->RotationMatrix[0][2] =  s[0]*s[1];
                        opsd->RotationMatrix[1][2] = -c[0]*s[1];
                        opsd->RotationMatrix[2][2] =  c[2];
                        break;
                        }
        case Z1_Y2_Z3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opsd->RotationMatrix[1][0] =  c[0]*s[2]     +c[1]*c[2]*s[0];
                        opsd->RotationMatrix[2][0] = -c[2]*s[1];

                        opsd->RotationMatrix[0][1] = -c[2]*s[0]-c[0]*c[1]*s[2];
                        opsd->RotationMatrix[1][1] =  c[0]*c[2]-c[1]*s[0]*s[2];
                        opsd->RotationMatrix[2][1] =  s[1]*s[2];

                        opsd->RotationMatrix[0][2] = c[0]*s[1];
                        opsd->RotationMatrix[1][2] = s[0]*s[1];
                        opsd->RotationMatrix[2][2] = c[1];
                        break;
                        }
// Tait-Bryan angles:
        case X1_Z2_Y3 : {
                        opsd->RotationMatrix[0][0] =  c[1]*c[2];
                        opsd->RotationMatrix[1][0] =  s[0]*s[2]     +c[0]*c[2]*s[1];
                        opsd->RotationMatrix[2][0] =  c[2]*s[0]*s[1]-c[0]*s[2];

                        opsd->RotationMatrix[0][1] = -s[1];
                        opsd->RotationMatrix[1][1] =  c[0]*c[1];
                        opsd->RotationMatrix[2][1] =  c[1]*s[0];

                        opsd->RotationMatrix[0][2] = c[1]*s[2];
                        opsd->RotationMatrix[1][2] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opsd->RotationMatrix[2][2] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        break;
                        }
        case X1_Y2_Z3 : {
                        opsd->RotationMatrix[0][0] =  c[1]*c[2];
                        opsd->RotationMatrix[1][0] =  c[0]*s[2]+c[2]*s[0]*s[1];
                        opsd->RotationMatrix[2][0] =  s[0]*s[2]-c[0]*c[2]*s[1];

                        opsd->RotationMatrix[0][1] = -c[1]*s[2];
                        opsd->RotationMatrix[1][1] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opsd->RotationMatrix[2][1] =  c[2]*s[0]+c[0]*s[1]*s[2];

                        opsd->RotationMatrix[0][2] =  s[1];
                        opsd->RotationMatrix[1][2] = -c[1]*s[0];
                        opsd->RotationMatrix[2][2] =  c[0]*c[1];
                        break;
                        }
        case Y1_X2_Z3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[2]     +s[0]*s[1]*s[2];
                        opsd->RotationMatrix[1][0] =  c[1]*s[2];
                        opsd->RotationMatrix[2][0] =  c[0]*s[1]*s[2]-c[2]*s[1];

                        opsd->RotationMatrix[0][1] =  c[2]*s[0]*s[1]-c[0]*s[2];
                        opsd->RotationMatrix[1][1] =  c[1]*c[2];
                        opsd->RotationMatrix[2][1] =  c[0]*c[2]*s[1]+s[0]*s[2];

                        opsd->RotationMatrix[0][2] =  c[1]*s[0];
                        opsd->RotationMatrix[1][2] = -s[1];
                        opsd->RotationMatrix[2][2] =  c[0]*c[1];
                        break;
                        }
        case Y1_Z2_X3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[1];
                        opsd->RotationMatrix[1][0] =  s[1];
                        opsd->RotationMatrix[2][0] = -c[1]*s[0];

                        opsd->RotationMatrix[0][1] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opsd->RotationMatrix[1][1] = c[1]*c[2];
                        opsd->RotationMatrix[2][1] = c[0]*s[2]+c[2]*s[0]*s[1];

                        opsd->RotationMatrix[0][2] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opsd->RotationMatrix[1][2] = -c[1]*s[2];
                        opsd->RotationMatrix[2][2] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        break;
                        }
        case Z1_Y2_X3 : {
                        opsd->RotationMatrix[0][0] = c[0]*c[1];
                        opsd->RotationMatrix[1][0] = c[1]*s[0];
                        opsd->RotationMatrix[2][0] = -s[1];

                        opsd->RotationMatrix[0][1] = c[0]*s[1]*s[2]-c[2]*s[0];
                        opsd->RotationMatrix[1][1] = c[0]*c[2]     +s[0]*s[1]*s[2];
                        opsd->RotationMatrix[2][1] = c[1]*s[2];

                        opsd->RotationMatrix[0][2] = s[0]*s[2]     +c[0]*c[2]*s[1];
                        opsd->RotationMatrix[1][2] = c[2]*s[0]*s[1]-c[0]*s[2];
                        opsd->RotationMatrix[2][2] = c[1]*c[2];
                        break;
                        }
        case Z1_X2_Y3 : {
                        opsd->RotationMatrix[0][0] =  c[0]*c[2]-s[0]*s[1]*s[2];
                        opsd->RotationMatrix[1][0] =  c[2]*s[0]+c[0]*s[1]*s[2];
                        opsd->RotationMatrix[2][0] = -c[1]*s[2];

                        opsd->RotationMatrix[0][1] = -c[1]*s[0];
                        opsd->RotationMatrix[1][1] =  c[0]*c[1];
                        opsd->RotationMatrix[2][1] =  s[1];

                        opsd->RotationMatrix[0][2] = c[0]*s[2]+c[2]*s[0]*s[1];
                        opsd->RotationMatrix[1][2] = s[0]*s[2]-c[0]*c[2]*s[1];
                        opsd->RotationMatrix[2][2] = c[1]*c[2];
                        break;
                        }
        default:        {// x-convention
                        opsd->RotationMatrix[0][0] = c[0]*c[2]-c[1]*s[0]*s[2];
                        opsd->RotationMatrix[1][0] = c[2]*s[0]+c[0]*c[1]*s[2];
                        opsd->RotationMatrix[2][0] = s[1]*s[2];

                        opsd->RotationMatrix[0][1] = -c[0]*s[2]     -c[1]*c[2]*s[0];
                        opsd->RotationMatrix[1][1] =  c[0]*c[1]*c[2]-s[0]*s[2];
                        opsd->RotationMatrix[2][1] =  c[2]*s[1];

                        opsd->RotationMatrix[0][2] =  s[0]*s[1];
                        opsd->RotationMatrix[1][2] = -c[0]*s[1];
                        opsd->RotationMatrix[2][2] =  c[2];
                        }
    }
    for (i=0;i<3;i++) {
        for (j=0;j<3;j++) {
            opsd->RotationMatrixT[i][j] = opsd->RotationMatrix[j][i];
        }
    }
}

scalar CalcModFhkl(int h, int k, int l, ordered_particles_param *opsparam, sasfit_param *param)
{
    scalar A, B, tmp;
    int i;
    A=0;
    B=0;
    for (i=0;i<opsparam->n;i++) {
        tmp = 2*M_PI*(h*opsparam->positions[i][0]+k*opsparam->positions[i][1]+l*opsparam->positions[i][2]);
        A=A+cos(tmp);
        B=B+sin(tmp);
    }
    return sqrt(A*A+B*B);
}

void set_f_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param)
{
    switch (ospparam->order_type_Selector) {
        case SC  :
        case BCT :
        case BCC :
        case FCC :
        case HCP :
        case HEX :
        case SQ  :
        case CREC :
        case LAM :
                ospparam->f_hkl=CalcModFhkl(h, k, l, ospparam,param);
                break;
        case la3d:  switch (100*abs(h)+10*abs(k)+abs(l)) {
                        case 211:
                        case 220:
                        case 321:
                        case 332:
                        case 400:
                        case 420:
                        case 422:
                        case 431:
                        case 440:
                        case 444:
                        case 521:
                        case 532:
                        case 541:
                        case 543:
                        case 552:   ospparam->f_hkl = 1;
                                    break;
                        default:    ospparam->f_hkl = 0;
                                    break;
                    }
                    break;
        case lm3m:  switch (100*abs(h)+10*abs(k)+abs(l)) {
                        case 110:
                        case 200:
                        case 211:
                        case 220:
                        case 222:
                        case 310:
                        case 321:
                        case 330:
                        case 332:
                        case 400:
                        case 411:
                        case 420:
                        case 422:
                        case 431:
                        case 433:   ospparam->f_hkl = 1;
                                    break;
                        default:    ospparam->f_hkl = 0;
                                    break;
                    }
                    break;
        case Pn3m:  switch (100*abs(h)+10*abs(k)+abs(l)) {
                        case 110:
                        case 111:
                        case 200:
                        case 211:
                        case 220:
                        case 221:
                        case 222:
                        case 310:
                        case 311:
                        case 321:
                        case 322:
                        case 330:
                        case 331:
                        case 332:
                        case 333:   ospparam->f_hkl = 1;
                                    break;
                        default:    ospparam->f_hkl = 0;
                                    break;
        }
        default:
                    sasfit_err("Unknown type of ordered mesoscopic structure\n");
                    ospparam->f_hkl = 0;
                    break;
    }
}

void set_g_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    int i;
    ospparam->ghklmod = 0;
    for (i=0;i<3;i++) {
        ospparam->ghkl[i]=h*ospparam->ast[i]+k*ospparam->bst[i]+l*ospparam->cst[i];
        ospparam->ghklmod=ospparam->ghklmod+gsl_pow_2(ospparam->ghkl[i]);
    }
    ospparam->ghklmod = sqrt(ospparam->ghklmod);
    return;
}

void set_psi_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param)
{
    scalar tmp;
    int i;
    tmp = 0;
    for (i=0;i<3;i++) {
        tmp = tmp+ospparam->q[i]*ospparam->ghkl[i];
    }
    ospparam->psi_hkl=acos(tmp/fabs(ospparam->Qmod*ospparam->ghklmod));
    return;
}

void CrossProduct( scalar *x, scalar *y, scalar *res) {
    res[0] = (-x[2]*y[1] + x[1]*y[2]);
    res[1] = ( x[2]*y[0] - x[0]*y[2]);
    res[2] = (-x[1]*y[0] + x[0]*y[1]);
}

scalar DotProduct(scalar *x, scalar *y) {
    return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

scalar SpatProduct(scalar *a, scalar *b, scalar *c) {
    scalar CP[3];
    CrossProduct(b,c,CP);
    return DotProduct(a,CP);
}

scalar CalcReciprocalLatticeVectors(scalar *av, scalar *bv, scalar *cv, scalar *astar, scalar *bstar, scalar *cstar) {
        scalar Volume;
        scalar tv[3];
        int i;
        Volume = SpatProduct(av,bv,cv);
        CrossProduct(bv,cv,tv);
        for (i=0;i<3;i++) astar[i]=2*M_PI/Volume * tv[i];
        CrossProduct(cv,av,tv);
        for (i=0;i<3;i++) bstar[i]=2*M_PI/Volume * tv[i];
        CrossProduct(av,bv,tv);
        for (i=0;i<3;i++) cstar[i]=2*M_PI/Volume * tv[i];
        return Volume;
}

scalar CalcDirectReciprocalLatticeVectors(ordered_particles_param *opsparam) {
        scalar Volume;
        scalar tv[3];
        int i;
        switch (opsparam->dim) {
            case 3:
                opsparam->a[0] = opsparam->ad;
                opsparam->a[1] = 0;
                opsparam->a[2] = 0;
                opsparam->b[0] = opsparam->bd*cos(opsparam->lgamma);
                opsparam->b[1] = opsparam->bd*sin(opsparam->lgamma);
                opsparam->b[2] = 0;
                opsparam->c[0] = cos(opsparam->lbeta);
                opsparam->c[1] = (cos(opsparam->lalpha)-cos(opsparam->lbeta)*cos(opsparam->lgamma))/sin(opsparam->lgamma);
                opsparam->c[2] = sqrt(1-gsl_pow_2(opsparam->c[0])-gsl_pow_2(opsparam->c[1]));
                for (i=0;i<3;i++) opsparam->c[i]=opsparam->cd*opsparam->c[i];
                Volume = SpatProduct(opsparam->a,opsparam->b,opsparam->c);
                CrossProduct(opsparam->b,opsparam->c,tv);
                for (i=0;i<3;i++) opsparam->ast[i]=2*M_PI/Volume * tv[i];
                CrossProduct(opsparam->c,opsparam->a,tv);
                for (i=0;i<3;i++) opsparam->bst[i]=2*M_PI/Volume * tv[i];
                CrossProduct(opsparam->a,opsparam->b,tv);
                for (i=0;i<3;i++) opsparam->cst[i]=2*M_PI/Volume * tv[i];
                break;
            case 2:
                opsparam->a[0] = opsparam->ad;
                opsparam->a[1] = 0;
                opsparam->a[2] = 0;
                opsparam->b[0] = opsparam->bd*cos(opsparam->lgamma);
                opsparam->b[1] = opsparam->bd*sin(opsparam->lgamma);
                opsparam->b[2] = 0;
                opsparam->c[0] = 0;
                opsparam->c[1] = 0;
                opsparam->c[2] = 0;

                opsparam->ast[0] = 2*M_PI/opsparam->ad;
                opsparam->ast[1] = -2*M_PI/opsparam->ad*cos(opsparam->lgamma)/sin(opsparam->lgamma);
                opsparam->ast[2] = 0;
                opsparam->bst[0] = 0;
                opsparam->bst[1] = 2*M_PI/opsparam->bd/sin(opsparam->lgamma);
                opsparam->bst[2] = 0;
                opsparam->cst[0] = 0;
                opsparam->cst[1] = 0;
                opsparam->cst[2] = 0;
                Volume=opsparam->ad*opsparam->bd*sin(opsparam->lgamma);
                break;
            case 1:
                opsparam->a[0] = opsparam->ad;
                opsparam->a[1] = 0;
                opsparam->a[2] = 0;
                opsparam->b[0] = 0;
                opsparam->b[1] = 0;
                opsparam->b[2] = 0;
                opsparam->c[0] = 0;
                opsparam->c[1] = 0;
                opsparam->c[2] = 0;

                opsparam->ast[0] = 2*M_PI/opsparam->ad;
                opsparam->ast[1] = 0;
                opsparam->ast[2] = 0;
                opsparam->bst[0] = 0;
                opsparam->bst[1] = 0;
                opsparam->bst[2] = 0;
                opsparam->cst[0] = 0;
                opsparam->cst[1] = 0;
                opsparam->cst[2] = 0;
                Volume=opsparam->ad;
                break;
            default:
                sasfit_out("Can't handle such a dimensionality\n");
                Volume = 0;
                break;
        }
        return Volume;
}

void R_mult_r(scalar R[3][3],scalar r[3]) {
    scalar t[3];
    t[0]=r[0];
    t[1]=r[1];
    t[2]=r[2];
    r[0] = R[0][0]*t[0]+R[0][1]*t[1]+R[0][2]*t[2];
    r[1] = R[1][0]*t[0]+R[1][1]*t[1]+R[1][2]*t[2];
    r[2] = R[2][0]*t[0]+R[2][1]*t[1]+R[2][2]*t[2];
}

void init_osp(ordered_particles_param *ospparam, sasfit_param *param)
{
    ops_setRotationMatrix(ospparam);
    ospparam->ex[0] = 1.0;
    ospparam->ex[1] = 0.0;
    ospparam->ex[2] = 0.0;

    ospparam->ey[0] = 0.0;
    ospparam->ey[1] = 1.0;
    ospparam->ey[2] = 0.0;

    ospparam->ez[0] = 0.0;
    ospparam->ez[1] = 0.0;
    ospparam->ez[2] = 1.0;

    switch (ospparam->order_type_Selector) {
// SC simple cubic
        case Pm3m:
        case SC  :
                ospparam->bd=ospparam->ad;
                ospparam->cd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 1;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/6.0;
                break;
// BCC Body-centered cubic
        case BCC :
                ospparam->bd=ospparam->ad;
                ospparam->cd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.5;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.5;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad*sqrt(3)*0.5;
                ospparam->Omega = 4*M_PI;
                break;
// BCCTWIN: twinned Body-centered cubic
        case BCCTWIN :
                ospparam->bd=ospparam->ad;
                ospparam->cd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.5;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.5;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad*sqrt(3)*0.5;
                ospparam->Omega = 4.0*M_PI;
                ospparam->fphimax = M_PI*sqrt(3)/8.0;
                break;
// FCC Face-centered cubic (also called cubic close packed)
        case Fm3m:
        case FCC :
                ospparam->bd=ospparam->ad;
                ospparam->cd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 4;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.0;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.5;
                ospparam->positions[2][0]=0.5;
                ospparam->positions[2][1]=0.0;
                ospparam->positions[2][2]=0.5;
                ospparam->positions[3][0]=0.5;
                ospparam->positions[3][1]=0.5;
                ospparam->positions[3][2]=0.0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad*sqrt(2)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(2)/6.0;
                break;
// FCCTWIN twinned Face-centered cubic (also called cubic close packed)
        case FCCTWIN :
                ospparam->bd=ospparam->ad;
                ospparam->cd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 4;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.0;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.5;
                ospparam->positions[2][0]=0.5;
                ospparam->positions[2][1]=0.0;
                ospparam->positions[2][2]=0.5;
                ospparam->positions[3][0]=0.5;
                ospparam->positions[3][1]=0.5;
                ospparam->positions[3][2]=0.0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad*sqrt(2)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(2)/6.0;
                break;
// HCP Hexagonal close-packed
        case P6_mmc:
        case HCP :
                ospparam->bd=ospparam->ad;
                ospparam->cd=sqrt(8./3.)*ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 120*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=2./3.;
                ospparam->positions[1][1]=1./3.;
                ospparam->positions[1][2]=0.5;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/(3*sqrt(2));
                break;
// HCPTWIN twinned Hexagonal close-packed
        case HCPTWIN :
                ospparam->bd=ospparam->ad;
                ospparam->cd=sqrt(8./3.)*ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 120*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=2./3.;
                ospparam->positions[1][1]=1./3.;
                ospparam->positions[1][2]=0.5;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/(3*sqrt(2));
                break;
// BCT: body centered tetragonal
        case  I4_mm:
        case BCT :
                ospparam->bd=ospparam->ad;
                ospparam->lalpha = 90*M_PI/180.;
                ospparam->lbeta = 90*M_PI/180.;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.5;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.5;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad*sqrt(3)*0.5;
                ospparam->Omega = 4*M_PI;
                break;
        case P6_mm:
        case HEX :
                ospparam->bd=ospparam->ad;
                ospparam->cd=0.0;
                ospparam->lalpha = 0;
                ospparam->lbeta = 0;
                ospparam->lgamma = 120*M_PI/180.;
                ospparam->dim = 2;
                ospparam->n = 1;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 2*M_PI;
                break;
        case P4_mm:
        case SQ  :
                ospparam->bd=ospparam->ad;
                ospparam->cd=0.0;
                ospparam->lalpha = 0;
                ospparam->lbeta = 0;
                ospparam->lgamma = 90*M_PI/180.;
                ospparam->dim = 2;
                ospparam->n = 1;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 2*M_PI;
                break;
        case cmm2:
        case CREC:
                ospparam->cd=0.0;
                ospparam->lalpha = 0;
                ospparam->lbeta = 0;
                ospparam->lgamma = 90.0*M_PI/180.;
                ospparam->dim = 2;
                ospparam->n = 2;
/*
                ospparam->lgamma = atan(PBD/PAD);
                ospparam->n = 1;
                ospparam->bd=sqrt(gsl_pow_2(PAD)+gsl_pow_2(PBD));
*/
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->positions[1][0]=0.5;
                ospparam->positions[1][1]=0.5;
                ospparam->positions[1][2]=0.0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = sqrt(gsl_pow_2(ospparam->ad)+gsl_pow_2(ospparam->bd))/2.0;
                ospparam->abar=PBD;
                if (ospparam->abar > ospparam->ad) ospparam->abar=ospparam->ad;
                if (ospparam->abar > ospparam->bd) ospparam->abar=ospparam->bd;
                ospparam->Omega = 2*M_PI;
                break;
        case LAM :
                ospparam->bd=0.0;
                ospparam->cd=0.0;
                ospparam->lalpha = 0;
                ospparam->lbeta = 0;
                ospparam->lgamma = 0;
                ospparam->dim = 1;
                ospparam->n = 1;
                ospparam->positions[0][0]=0;
                ospparam->positions[0][1]=0;
                ospparam->positions[0][2]=0;
                ospparam->Vd = CalcDirectReciprocalLatticeVectors(ospparam);
                ospparam->vd = ospparam->Vd;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 1;
                ospparam->fphimax = 1;
                break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");
    }

    R_mult_r(MR,ospparam->a);
    R_mult_r(MR,ospparam->b);
    R_mult_r(MR,ospparam->c);
    R_mult_r(MR,ospparam->ast);
    R_mult_r(MR,ospparam->bst);
    R_mult_r(MR,ospparam->cst);

}

scalar K(scalar x,ordered_particles_param *ospparam, sasfit_param *param) {
    scalar Kval, t1, t2 ,t3 ,t4, t5, t10, t12, t14, t16, t18, t22, t26;
    scalar t8, t9, t11, t13, t15, t17, t19, t24, t25, t35, t38, t39, t42, t43, t56, t58, t61, t65, t71, t73, t81, t83, t84,t89, t102, t104, t112, t136, t139, t150;
    scalar t28,t46, t54, t55,t57, t64, t70, t78, t94,t95,t100,t27,t31,t32,t47,t62;
    scalar t23, t29, t30,t36,t37,t40,t41,t66,t67,t107,t108,t111,t113,t114,t119,t124,t129,t140,t141,t152,t168,t169,t170,t171,t172,t173,t174,t176,t177,t178,t180,t183,t184,t193,t195,t197;
    scalar t206,t224,t213,t267,t274,t288,t291,t239,t277,t286,t328,t359,t378;
    scalar t6,t34,t48,t68,t7,t51,t20,t49,t53,t75,t98;
    scalar t33,t59,t69,t103,t106;
    scalar t88,t45,t116,t63,t21,t52,t76,t87,t85,t121,t60;
    scalar c, m;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   if (x > 0.1 ) {
                                Kval = 2*re_cerfi_z(1.0/(2.0*sqrt(x)),0);
                                Kval = Kval-re_cerfi_z(1.0/(2.0*sqrt(x)),M_PI*sqrt(x));
                                Kval = Kval+re_cerfi_z(-1.0/(2.0*sqrt(x)),M_PI*sqrt(x));
                                Kval = Kval*sqrt(M_PI/(16.0*x))*exp(-1.0/(4.0*x));
                                return Kval;
                            } else {
                                t1 = sqrt(M_PI);
                                t2 = sqrt(x);
                                t3 = 0.1e1 / t2;
                                t4 = t3 / 0.2e1;
                                t5 = im_w_of_z(-t4,0);
                                t10 = t2 * M_PI;
                                t12 = im_w_of_z(-t4,-t10);
                                t14 = M_PI * M_PI;
                                t16 = exp(x * t14);
                                t18 = t3 / t16;
                                t22 = im_w_of_z(-t4,t10);
                                t26 = -t1 * t5 * t3 / 0.2e1 - t1 * t12 * t18 / 0.4e1 - t1 * t22 * t18 / 0.4e1;
                                return t26;
                                Kval = 2-(M_PI*M_PI-4)*x+(48-12*M_PI+gsl_pow_4(M_PI))*x*x/2.0-(-1440+360*M_PI*M_PI-30*gsl_pow_4(M_PI)+gsl_pow_6(M_PI))*x*x*x/6.;
                            }
                            break;
    case LORENTZIANPEAK :   if (x > 0.017503651419485603162948751901763484536441467315369) {
                                Kval = (x*gsl_pow_2(M_PI)*(-840*(1 + 272*x*(1 +
                                        210*x*(1 + 156*x*(1 + 110*x*(1 + 72*x*(1 + 42*x*(1 + 20*x))))))) +
                                        420*x*(1 + 272*x*(1 + 210*x*(1 + 156*x*(1 + 110*x*(1 + 72*x*(1 + 42*x))))))*
                                        gsl_pow_2(M_PI) - 280*gsl_pow_2(x)*
                                        (1 + 272*x*(1 + 210*x*(1 + 156*x*(1 + 110*x*(1 + 72*x)))))*gsl_pow_4(M_PI) +
                                        210*gsl_pow_3(x)*(1 + 272*x*(1 + 210*x*(1 + 156*x*(1 + 110*x))))*gsl_pow_6(M_PI) -
                                        168*gsl_pow_4(x)*(1 + 272*x*(1 + 210*x*(1 + 156*x)))*gsl_pow_8(M_PI) +
                                        140*gsl_pow_5(x)*(1 + 272*x*(1 + 210*x))*gsl_pow_int(M_PI,10) -
                                        120*gsl_pow_6(x)*(1 + 272*x)*gsl_pow_int(M_PI,12) + 105*gsl_pow_7(x)*gsl_pow_int(M_PI,14)) +
                                        840*(1 + 272*x*(1 + 210*x*(1 + 156*x*
                                        (1 + 110*x*(1 + 72*x*(1 + 42*x*(1 + 20*x*(1 + 6*x))))))))*
                                        log(1 + x*gsl_pow_2(M_PI)))/(5.9755487920128e17*gsl_pow_9(x));
                            } else {
                                Kval = 2 + x*(4 - gsl_pow_2(M_PI) + x*(48*(1 + 30*x*
                                        (1 + 56*x*(1 + 90*x*(1 + 132*x*(1 + 182*x*(1 + 240*x)))))) -
                                        12*(1 + 30*x*(1 + 56*x*(1 + 90*x*(1 + 132*x*(1 + 182*x*(1 + 240*x))))))*
                                        gsl_pow_2(M_PI) + (1 + 30*x*(1 + 56*x*(1 + 90*x*(1 + 132*x*(1 + 182*x*(1 + 240*x))))))*
                                        gsl_pow_4(M_PI) - x*(1 + 56*x*(1 + 90*x*(1 + 132*x*(1 + 182*x*(1 + 240*x)))))*
                                        gsl_pow_6(M_PI) + gsl_pow_2(x)*(1 + 90*x*(1 + 132*x*(1 + 182*x*(1 + 240*x))))*
                                        gsl_pow_8(M_PI) - gsl_pow_3(x)*(1 + 132*x*(1 + 182*x*(1 + 240*x)))*gsl_pow_int(M_PI,10) +
                                        gsl_pow_4(x)*(1 + 182*x*(1 + 240*x))*gsl_pow_int(M_PI,12) -
                                        gsl_pow_5(x)*(1 + 240*x)*gsl_pow_int(M_PI,14) + gsl_pow_6(x)*gsl_pow_int(M_PI,16)));
                            }
                            return Kval;
                            break;
    case PEARSON :
                            c=x;
                            m=ospparam->nu;
                            if (x<1.1e-3) {
                                t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * t1;
t3 = c * c;
t4 = t3 * t3;
t5 = t4 * t2;
t6 = m * m;
t7 = t6 * t6;
t10 = t2 * t1;
t11 = t3 * c;
t12 = t11 * t10;
t15 = t6 * m;
t18 = t4 * t10;
t25 = t4 * t1;
t28 = t11 * t2;
t35 = t11 * t1;
t40 = t3 * t1;
t51 = t3 * t2;
t56 = 0.2e1 + (double) (70 * t7 * t5) - t6 * t12 / 0.2e1 + (double) (420 * t15 * t5) - 0.14e2 * m * t18 + 0.770e3 * t6 * (double) t5 + 0.420e3 * m * (double) t5 - (double) (5040 * t15 * t25) + 0.10e2 * m * t28 - 0.9240e4 * t6 * (double) t25 - 0.5040e4 * m * (double) t25 - 0.180e3 * t6 * t35 - 0.120e3 * m * t35 - 0.6e1 * m * t40 - m * t12 / 0.3e1 + 0.5e1 * (double) t15 * t28 - (double) (840 * t7 * t25) + 0.15e2 * t6 * t28 + t6 * t51 / 0.2e1 - 0.60e2 * (double) t15 * t35;
t59 = t2 * t2;
t60 = t4 * t59;
t102 = -m * c * t1 + 0.4e1 * c * m + 0.480e3 * m * t11 + 0.24e2 * m * t3 + 0.20160e5 * t4 * m + 0.240e3 * t15 * t11 + 0.720e3 * t6 * t11 + 0.20160e5 * t15 * t4 + 0.24e2 * t6 * t3 + 0.36960e5 * t6 * t4 + 0.3360e4 * t7 * t4;
t104 = t56 + m * t51 / 0.2e1 + t7 * t60 / 0.24e2 + t15 * t60 / 0.4e1 + 0.11e2 / 0.24e2 * t6 * t60 - 0.7e1 / 0.3e1 * t7 * t18 + m * t60 / 0.4e1 - 0.14e2 * t15 * t18 - 0.77e2 / 0.3e1 * t6 * t18 - t15 * t12 / 0.6e1 - 0.6e1 * t6 * t40 + t102;
Kval=t104;
return Kval;
                            }
                            if (m==1) {
                                t1 = log(c);
t3 = 0.3141592654e1 * 0.3141592654e1;
t4 = c * c;
t5 = t4 * t4;
t6 = t5 * c;
t9 = t3 * t3;
t10 = t9 * t3;
t15 = t4 * c;
t32 = c * t3;
t34 = -20. * t15 * t10 - 2200. * t5 * t10 - 158400. * t6 * t10 - 475200. * t15 * t3 + 3300. * t15 * t9 - 6600. * t4 * t3 - 19958400. * t5 * t3 - 399168000. * t6 * t3 + 30 * t4 * t9 + 237600. * t5 * t9 + 9979200. * t6 * t9 + 60 * t1 - 60 * t32;
t35 = t9 * t9;
t48 = log(0.1e1 / c *  (t32 + 1));
t68 = - (12 * t6 * t35 * t3) + 0.6600e4 * t1 * c + 0.6600e4 * t48 * c + 0.19958400e8 * t15 * t1 + 0.475200e6 * t4 * t1 + 0.399168000e9 * t5 * t1 + 0.2395008000e10 *  t6 * t1 + 0.19958400e8 * t48 * t15 + 0.15e2 * t5 *  t35 +  (1650 * t6 * t35) + 0.475200e6 * t48 * t4 + 0.399168000e9 * t48 * t5 + 0.2395008000e10 * t48 *  t6 + 0.60e2 * t48;
t73 = 0.1e1 / t5 / t4 * (t34 + t68) / 0.4790016000e10;
Kval=t73;
                                return Kval;
                            }
                            if (m==2) {
                                t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * c;
t4 = c * c;
t5 = t4 * t4;
t6 = t5 * c;
t7 = t6 * t1;
t9 = t1 * t1;
t10 = t9 * t1;
t15 = t4 * c;
t26 = t5 * t1;
t28 = t15 * t1;
t30 = t4 * t1;
t32 = t9 * t9;
t40 = log(c);
t46 = log(0.1e1 / c * (t2 + 1));
t51 = - (3 * t6 * t32 * t1) - 0.5280e4 * t40 * c - 0.5280e4 * t46 * c - (10 * t15 * t10) - (880 * t5 * t10) - (47520 * t6 * t10) - 0.7983360e7 * t15 * t40 + (2640 * t15 * t9) + (5 * t5 * t32) + (440 * t6 * t32) + (30 * t4 * t9) + (142560. * t5 * t9) + (3991680. * t6 * t9) + (60 * t2) + (7983360. * t26) + (285120 * t28) + (5280 * t30) + (79833600. * t7);
t58 = t5 * t4;
t65 = t40 * t1;
t89 = - (7983360 * t5 * t46 * t1) - 0.60e2 * c * t65 + (479001600. * t58 * t1) - (7983360. * t46 * t15) - 0.285120e6 * t15 * t65 - (60 * t46 * t2) - (7983360. * t40 * t26) - (285120. * t46 * t28) - (5280. * t46 * t30) - (285120. * t4 * t40) - (285120. * t46 * t4) - 0.5280e4 * t4 * t65 - (79833600. * t5 * t40) - (79833600. * t46 * t5) - (79833600. * t46 * t7) - 0.79833600e8 * t6 * t65 - (60 * t40) - (60 * t46);
t95 = 0.1e1 / (t5 * t15 * t1 + t58) * (t51 + t89) / 0.958003200e9;
Kval=t95;
                                return Kval;
                            }
                            if (m==3) {
t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * c;
t6 = log(0.1e1 / c *  (t2 + 1));
t8 = c * c;
t9 = t8 * t8;
t12 = t9 * t1;
t13 = log(c);
t16 = t1 * t1;
t17 = t13 * t16;
t20 = t9 * t16;
t23 = t9 * c;
t26 = t23 * t16;
t31 = t8 * t16;
t35 = t16 * t1;
t40 = t8 * c;
t49 = t40 * t1;
t51 = t8 * t1;
t53 = t9 * t40;
t56 = 3991680 * t9 * t6 * t1 + 3991680 * t13 * t12 - 5940 * t40 * t16 + 119750400 * t53 * t16 + 1995840 * t23 * t17 + 60 * t8 * t17 + 142560 * t9 * t17 + 142560 * t6 * t20 - 47520 * t23 * t35 + 1995840 * t6 * t26 + 60 * t6 * t31 - 20 * t40 * t35 - 1320 * t9 * t35 - 1995840 * t12 - 60 * t2 - 213840 * t20 - 2993760 * t26 - 90 * t31 - 142560 * t49 - 3960 * t51;
t57 = t16 * t16;
t75 = t13 * t1;
t98 = t9 * t8;
t103 = 3960 * t40 * t6 * t16 + 239500800 * t98 * t1 - 19958400 * t98 * t16 + 3960 * t40 * t17 + 285120 * t40 * t75 + 285120 * t6 * t49 + 7920 * t6 * t51 + 142560 * t8 * t6 + 7920 * t8 * t75 + 60 * t13 + 60 * t6;
t106 = t9 * t9;
t114 = 0.1e1 / (958003200 * t53 * t1 + 479001600 * t106 * t16 + 479001600 * t98) * (- (2 * t23 * t57 * t1) + 0.3960e4 * t13 * c + 0.3960e4 * t6 * c + 0.120e3 * c * t75 + 0.1995840e7 * t40 * t13 + 0.142560e6 * t13 * t8 + 0.120e3 * t6 * t2 + (330 * t23 * t57) + 0.1995840e7 * t6 * t40 +  (5 * t9 * t57) + t103 + t56);
Kval=t114;
                                return Kval;
                            }
                            if (m==4) {
                                t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * t1;
t3 = t2 * t1;
t4 = c * c;
t5 = t4 * t4;
t6 = t5 * t4;
t9 = t5 * c;
t10 = t9 * t3;
t12 = t5 * t3;
t14 = t4 * c;
t15 = t14 * t3;
t17 = t5 * t2;
t21 = t4 * t2;
t23 = t14 * t1;
t25 = t4 * t1;
t27 = t1 * c;
t30 = t5 * t14;
t35 = t2 * t2;
t43 = t5 * t5;
t46 = log(c);
t52 = log(0.1e1 / c * (t27 + 1));
t61 = - (3 * t9 * t35 * t1) - 0.2640e4 * t46 * c - 0.2640e4 * t52 * c + (239500800 * t30 * t2) - (19958400 * t6 * t2) - (6652800 * t30 * t3) + (79833600 * t43 * t3) + (15 * t5 * t35) + (660 * t9 * t35) - 0.47520e5 * t46 * t4 - 0.47520e5 * t52 * t4;
t67 = t46 * t1;
t76 = t46 * t2;
t87 = t46 * t3;
t108 = -0.180e3 * c * t67 - (47520 * t52 * t10) - (2640 * t52 * t12) - (60 * t14 * t87) - (60 * t52 * t15) - (142560 * t52 * t17) - (180 * t52 * t21) - (180 * t52 * t27) - (180 * t4 * t76) - (2640 * t5 * t87) - (47520 * t9 * t87);
t121 = 0.1e1 / (0.479001600e9 * t43 * c * t3 + (1437004800. * t30 * t1) + 0.1437004800e10 * t43 * t2 + (479001600 * t6)) * (-0.7920e4 * t14 * t2 * t52 + (239500800. * t6 * t1) + 0.6600e4 * t14 * t2 - 0.142560e6 * t14 * t67 - 0.7920e4 * t14 * t76 - 0.142560e6 * t52 * t23 - 0.7920e4 * t52 * t25 + 0.665280e6 * t6 * t3 - 0.7920e4 * t4 * t67 - 0.142560e6 * t5 * t76 + (87120 * t10) + t108 + (4840 * t12) + (110 * t15) + (118800 * t17) + (150 * t21) + 0.47520e5 * t23 + 0.2640e4 * t25 + (60 * t27) - (60 * t46) - 0.60e2 * t52 + t61);
Kval=t121;
                                return Kval;
                            }
                            if (m==5) {
                                t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * c;
t6 = log(0.1e1 / c * (t2 + 1));
t7 = t1 * t1;
t8 = t7 * t7;
t9 = t8 * t6;
t10 = c * c;
t11 = t10 * t10;
t12 = t11 * c;
t17 = t8 * t11;
t18 = log(c);
t21 = t7 * t1;
t22 = t18 * t21;
t25 = t11 * t21;
t28 = t10 * c;
t31 = t28 * t21;
t34 = t18 * t7;
t37 = t10 * t7;
t40 = t18 * t1;
t47 = t10 * t1;
t59 = t11 * t10;
t63 = (1320 * t12 * t8 * t18) + (7920 * t28 * t7 * t6) + 0.240e3 * t40 * c + (360 * t10 * t34) + 0.5280e4 * t10 * t40 + (5280 * t11 * t22) + (60 * t11 * t9) + (1320 * t12 * t9) + (60 * t18 * t17) + (240 * t6 * t2) + (1330560 * t59 * t21) + (240 * t28 * t22) + (5280 * t6 * t25) + (7920 * t28 * t34) + (240 * t6 * t31) + (360 * t6 * t37) + (5280 * t47 * t6) - (60 * t2) - (5720 * t25);
t69 = t11 * t28;
t84 = t11 * t11;
t85 = t84 * c;
t102 = - (12 * t12 * t8 * t1) + 0.1320e4 * t18 * c + 0.1320e4 * t6 * c + (479001600. * t59 * t1) - (2750. * t12 * t8) - (26611200. * t69 * t21) + (479001600. * t84 * t21) - (4620 * t28 * t7) - (39916800. * t59 * t7) - (23760. * t59 * t8) + (718502400. * t7 * t69) + (332640 * t8 * t69) - (6652800 * t84 * t8) + (119750400 * t85 * t8) - (125 * t17) + 0.60e2 * t18 - (260 * t31) - (210 * t37) - (1320 * t47) + 0.60e2 * t6;
t116 = 1 / (958003200. * t84 * t10 * t8 + 3832012800. * t69 * t1 + 3832012800. * t85 * t21 + 5748019200. * t84 * t7 + 958003200. * t59) * (t63 + t102);
Kval=t116;
                                return Kval;
                            }
                            if (m==6) {
                                t1 = 0.3141592654e1 * 0.3141592654e1;
t2 = t1 * t1;
t3 = t2 * t2;
t4 = t3 * t1;
t5 = c * c;
t6 = t5 * t5;
t7 = t6 * t6;
t8 = t7 * t5;
t11 = t7 * c;
t16 = t5 * c;
t17 = t6 * t16;
t22 = t6 * t5;
t25 = t6 * c;
t26 = t25 * t4;
t32 = t2 * t1;
t35 = t6 * t3;
t41 = t16 * t32;
t43 = t5 * t2;
t45 = t1 * c;
t47 = 2395008000. * t11 * t3 - 19958400 * t11 * t4 + 4790016000. * t17 * t2 + 3326400. * t17 * t3 - 199584000. * t17 * t32 - 23760 * t17 * t4 - 118800. * t22 * t3 + 6652800. * t22 * t32 + 1320 * t4 * t22 + 665280. * t7 * t4 + 479001600. * t8 * t4 + 137 * t26 + 385 * t35 + 470 * t41 + 270 * t43 + 60 * t45;
t56 = log(c);
t63 = log(0.1e1 / c * (t45 + 1));
t88 = -0.300e3 * c * t56 * t1 - 0.600e3 * t16 * t56 * t32 - 0.600e3 * t5 * t56 * t2 - 0.60e2 * t25 * t56 * t4 - (300 * t6 * t63 * t3) + 0.2395008000e10 * t22 * t1 - 0.199584000e9 * t22 * t2 - (60 * t63 * t26) - (99792000. * t7 * t3) + 0.4790016000e10 * t7 * t32 - 0.300e3 * t56 * t35 - (600 * t63 * t41) - (600 * t63 * t43) - (300 * t63 * t45) - 0.60e2 * t56 - (60 * t63);
t104 = 1 / (4790016000. * t7 * t16 * t4 + 23950080000. * t17 * t1 + 47900160000. * t11 * t32 + 47900160000. * t7 * t2 + 23950080000. * t8 * t3 + 4790016000. * t22) * (t47 + t88);
Kval=t104;
                                return Kval;
                            }
t2 = 0.3141592654e1 * 0.3141592654e1;
t3 = 0.5e1 - m;
t4 = pow(c, t3);
t5 = t4 * t2;
t10 = pow(0.1e1 / c * (c * t2 + 0.1e1), -m);
t11 = m * m;
t12 = t11 * m;
t13 = t12 * t10;
t16 = t11 * t10;
t19 = m * t10;
t22 = t2 * t2;
t23 = t22 * t2;
t24 = t4 * t23;
t25 = t11 * t11;
t26 = t25 * t10;
t29 = t22 * t22;
t30 = t4 * t29;
t35 = t4 * t22;
t36 = t25 * m;
t37 = t36 * t10;
t40 = 0.4e1 - m;
t41 = pow(c, t40);
t58 = - (665280. * t26 * t41 * t2) +  (23760. * t26 * t41 * t22) +  (514800. * t13 * t24) - (29604960. * t13 * t35) + (791683200. * t13 * t5) - (887040. * t16 * t24) -  (2275257600. * t16 * t5) +  (475200. * t19 * t24) -  (3960. * t19 * t30) +  (2395008000. * t19 * t5) -  (110880. * t26 * t24) +  (5322240. * t26 * t35) -  (332640. * t37 * t35) +  (6652800. * t37 * t5) - 0.15840e5 * c - 0.120e3;
t65 = 0.6e1 - m;
t66 = pow(c, t65);
t67 = t10 * t66;
t107 = t10 * t2;
t108 = t11 * t41;
t111 = -0.41673139200e11 * m * t2 * t67 + 0.4670265600e10 * m * t22 * t67 - 0.168981120e9 * m * t23 * t67 + 0.23151744000e11 * t11 * t2 * t67 - 0.3066940800e10 * t11 * t22 * t67 + 0.123742080e9 * t11 * t23 * t67 - 0.6187104000e10 * t12 * t2 * t67 + 0.911433600e9 * t12 * t22 * t67 - 0.40249440e8 * t12 * t23 * t67 + 0.798336000e9 * t25 * t2 * t67 - 0.39916800e8 * t36 * t2 * t67 - 0.126403200e9 * t25 * t22 * t67 + 0.6652800e7 * t36 * t22 * t67 -  (49230720. * t108 * t107) +  (64532160. * t16 * t35) -  (39916800. * t19 * t35) -  (119750400. * t26 * t5);
t113 = t10 * t22;
t114 = t12 * t41;
t119 = m * t41;
t124 = t10 * t29;
t129 = t10 * t23;
t140 = t29 * t2;
t141 = t4 * t140;
t152 = 5 * t26 * t41 * t29 + 9979200. * t114 * t107 + 79833600. * t119 * t107 + 974160. * t108 * t113 + 55. * t108 * t124 - 8800. * t108 * t129 - 285120. * t114 * t113 - 712800. * t119 * t113 - 30. * t114 * t124 + 3960. * t114 * t129 - 30. * t119 * t124 + 5280. * t119 * t129 + 35 * t13 * t141 - 10 * t26 * t141 + t37 * t141 - 110 * t37 * t30;
t168 = 0.1e1 - m;
t169 = pow(c, t168);
t170 = t10 * t169;
t172 = 0.2e1 - m;
t173 = pow(c, t172);
t174 = t10 * t173;
t176 = 0.3e1 - m;
t177 = pow(c, t176);
t178 = t10 * t177;
t180 = pow(c, -m);
t183 = c * c;
t184 = t183 * t183;
t193 = t10 * t41;
t195 = t10 * t4;
t197 = - (440 * t26 * t41 * t23) + 0.2275257600e10 * t184 * m +  (120 * t10 * t180) - 0.791683200e9 * t11 * t184 + 0.119750400e9 * t12 * t184 -  (5170 * t13 * t30) -  (50 * t16 * t141) +  (24 * t19 * t141) +  (7920 * t16 * t30) - 0.6652800e7 * t25 * t184 +  (7920 * t37 * t24) +  (1320. * t26 * t30) +  (15840. * t170) +  (1425600. * t174) +  (79833600. * t178) +  (2395008000. * t193) +  (28740096000. * t195);
t206 = t183 * c;
t213 = t184 * c;
t224 = t29 * t22;
t239 = - (225 * t11 * t224 * t67) +  (85 * t12 * t224 * t67) -  (110 * t36 * t140 * t67) -  (15 * t25 * t224 * t67) +  (t36 * t224 * t67) + 0.2640e4 * c * m + 0.522720e6 * m * t183 + 0.49230720e8 * m * t206 + 0.41673139200e11 * t213 * m - 0.47520e5 *  t11 * t183 - 0.9979200e7 *  t11 * t206 - 0.23151744000e11 *  t11 * t213 + 0.665280e6 *  t12 * t206 + 0.6187104000e10 *  t12 * t213 - 0.798336000e9 *  t25 * t213 + 0.39916800e8 *  t36 * t213;
t267 = t177 * t23;
t274 = t177 * t22;
t277 = t173 * t22;
t286 = -0.35640e5 * m * t140 * t67 + 0.274e3 * m * t224 * t67 + 0.28600e5 * t11 * t140 * t67 - 0.10450e5 * t12 * t140 * t67 + 0.847440e6 * t12 * t29 * t67 + 0.1760e4 * t25 * t140 * t67 - 0.332640e6 * t36 * t23 * t67 - 0.134640e6 * t25 * t29 * t67 + 0.7920e4 * t36 * t29 * t67 +  (20 * t13 * t267) -  (1320 * t13 * t274) -  (60 * t16 * t267) +  (9240 * t16 * t274) +  (60 * t16 * t277) +  (40 * t19 * t267) -  (7920 * t19 * t274) -  (60 * t19 * t277);
t288 = t177 * t2;
t291 = t173 * t2;
t328 = 0.3136320e7 * m * t29 * t67 - 0.2431440e7 * t11 * t29 * t67 +  (120 * t19 * t169 * t2) + 0.5987520e7 * t25 * t23 * t67 +  (47520 * t13 * t288) + 0.15840e5 * t140 * t67 -  (522720 * t16 * t288) -  (2640 * t16 * t291) +  (1425600 * t19 * t288) +  (15840 * t19 * t291) + 0.6652800e7 * t25 * t193 -  (39916800 * t36 * t195) + 0.28740096000e11 *  t2 * t67 - 0.2395008000e10 * t22 * t67 - 0.120e3 * t224 * t67 + 0.79833600e8 * t23 * t67 - 0.1425600e7 * t29 * t67;
t359 = -0.2640e4 * m * t170 - 0.522720e6 * m * t174 - 0.49230720e8 * m * t178 - 0.2275257600e10 * m * t193 - 0.41673139200e11 * m * t195 + 0.47520e5 * t11 * t174 + 0.9979200e7 * t11 * t178 + 0.791683200e9 * t11 * t193 + 0.23151744000e11 * t11 * t195 - 0.665280e6 * t12 * t178 - 0.119750400e9 * t12 * t193 - 0.6187104000e10 * t12 * t195 + 0.798336000e9 * t25 * t195 -  (1425600. * t183) -  (2395008000. * t184) -  (79833600.* t206) -  (28740096000. * t213);
t378 = 0.1e1 / t65 / t3 / t40 / t176 / t172 / t168 / t184 / t183 * (t58 + t111 + t152 + t197 + t239 + t286 + t328 + t359) / 0.79833600e8;
                            Kval = t378;
                            return Kval;
                            break;
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
    return Kval;
}

scalar G(ordered_particles_param *ospparam, sasfit_param *param) {
    return exp(-gsl_pow_2(SIGMA_A*ABAR*QMOD));
}

scalar Lq_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar x2,delta, gamma_nu, lnGnu_2, y, nu,x;
    int i;
    gsl_sf_result lnr, arg;

    gsl_set_error_handler_off();

    x2 = 0.0;
// doi: 10.1016.j.cis.2010.12.003
// S. Foerster et al./ Calculation of scattering-patterns of ordered nano- and mesoscale materials
// Advances in Colloid and Interface Science 163 (2011) 53-83
//
//    for (i=0;i<3;i++) {
//        x2 = x2+gsl_pow_2(ospparam->q[i]-ospparam->ghkl[i]);
//    }
// even though  the paper shows in eq. 82 the square of the vector difference I assume
// the sqare of the difference of the moduli of the two vectors needs to be used. here.
    x = ospparam->Qmod-ospparam->ghklmod;
    x2 = x*x;
    delta = ospparam->delta_q;
    nu = ospparam->nu;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   return 2.0/(M_PI*delta) * exp(-4.0*x2/(gsl_pow_2(delta)*M_PI));
                            break;
    case LORENTZIANPEAK :   return 2.0/(M_PI*delta) /(1.0+4.0*x2/gsl_pow_2(delta));
                            break;
    case BURGERMICHA    :   lnGnu_2 = gsl_sf_lngamma(0.5*nu);
                            gamma_nu = sqrt(M_PI)*exp(gsl_sf_lngamma((nu+1)/2.0)-lnGnu_2);
                            y = x2*gsl_pow_2(2.0/(M_PI*delta));
                            gsl_sf_lngamma_complex_e(0.5*nu,gamma_nu*y,&lnr,&arg);
                            return 2.0/(M_PI*delta)*gsl_pow_2(exp(lnr.val-lnGnu_2));
                            break;
    case PSEUDOVOIGT    :
    case PEARSON             :
                            return pow(1+gsl_pow_2(gsl_sf_beta(nu-0.5,0.5)*x/delta),-nu)/delta;
                            break;
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
}

scalar Lpsi_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar cG, cL,cP, x, bhkl, ahkl, chkl, deltapsi, mq,nu;
    int i;
    cG=1.0;
    cL=1.0;
    cP = 1.0;
    deltapsi = ospparam->delta_psi;
    nu = ospparam->nu;
    x = ospparam->Qmod*ospparam->psi_hkl;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :
                            ahkl = 4*gsl_pow_2(ospparam->ghklmod/deltapsi)/M_PI;
                            cG = 1.0/(2.0*M_PI*gsl_pow_2(ospparam->ghklmod)*K(ahkl,ospparam,param));
                            return cG * exp(-gsl_pow_2(2*x/deltapsi)/M_PI);
                            break;
    case LORENTZIANPEAK :
                            bhkl = 4*gsl_pow_2(ospparam->ghklmod/deltapsi);
                            cL = 1.0/(2.0*M_PI*gsl_pow_2(ospparam->ghklmod)*K(bhkl,ospparam,param));
                            return cL / (1.0+gsl_pow_2(2.0*x/deltapsi));
                            break;
    case PEARSON:
                            chkl = gsl_pow_2(gsl_sf_beta(nu-0.5,0.5)/deltapsi)*gsl_pow_2(ospparam->ghklmod/deltapsi);
                            cP = 1.0/(2.0*M_PI*gsl_pow_2(ospparam->ghklmod)*K(chkl,ospparam,param));
                            return cP*pow(1+gsl_pow_2(gsl_sf_beta(nu-0.5,0.5)*x/deltapsi),-nu);
                            break;

    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
}

scalar L_hkl_aniso(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    return  Lq_hkl(h,k,l,ospparam,param)*Lpsi_hkl(h,k,l,ospparam,param);
}

scalar L_hkl_iso(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar x, delta, gamma_nu, lnGnu_2, y, nu;
    gsl_sf_result lnr, arg;

    gsl_set_error_handler_off();

    x = ospparam->Qmod-ospparam->ghklmod;
    delta = ospparam->delta;
    nu = ospparam->nu;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   return 2/(M_PI*delta) * exp(-4.*x*x/(gsl_pow_2(delta)*M_PI));
                            break;
    case LORENTZIANPEAK :   return delta/(2*M_PI) / (x*x+gsl_pow_2(0.5*delta));
                            break;
    case BURGERMICHA    :   lnGnu_2 = gsl_sf_lngamma(0.5*nu);
                            gamma_nu = sqrt(M_PI)*exp(gsl_sf_lngamma((nu+1)/2.0)-lnGnu_2);
                            y = gsl_pow_2(2*x/(M_PI*delta));
                            gsl_sf_lngamma_complex_e(0.5*nu,gamma_nu*y,&lnr,&arg);
                            return 2.0/(M_PI*delta)*gsl_pow_2(exp(lnr.val-lnGnu_2));
                            break;
    case PEARSON:
                            return pow(1+gsl_pow_2(gsl_sf_beta(nu-0.5,0.5)*x/delta),-nu)/delta;
                            break;
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
}

scalar Lattice_Factor_aniso(ordered_particles_param *ospparam, sasfit_param *param)
{
    int h,k,l;
    scalar Z,prefactorZ;
    Z = 0.0;
    h=0;
    k=0;
    l=0;
    prefactorZ = gsl_pow_3(2*M_PI)/(ospparam->n*ospparam->vd);
// init_osp(ospparam,param);
    switch (ospparam->order_type_Selector) {
        case SC  :
        case BCC :
        case FCC :
        case HCP :
        case BCT :  for (h=-MAXHKL;h<=MAXHKL;h++) {
                        for (k=-MAXHKL;k<=MAXHKL;k++) {
                            for (l=-MAXHKL;l<=MAXHKL;l++) {
                                set_f_hkl(h,k,l,ospparam,param);
                                set_g_hkl(h,k,l,ospparam,param);
                                set_psi_hkl(h,k,l,ospparam,param);
                                if (h!=0 || k!=0 || l!=0) {
                                    Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_aniso(h,k,l,ospparam,param);
                                }
 //                               sasfit_out("%d %d %d %d %lf\n",h,k,l,ospparam->f_hkl,Z);
                            }
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case HEX :
        case SQ  :
        case CREC:  l = 0;
                    for (h=-MAXHKL;h<=MAXHKL;h++) {
                        for (k=-MAXHKL;k<=MAXHKL;k++) {
                                set_f_hkl(h,k,l,ospparam,param);
                                set_g_hkl(h,k,l,ospparam,param);
                                set_psi_hkl(h,k,l,ospparam,param);
                                if (h!=0 || k!=0 || l!=0) {
                                    Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_aniso(h,k,l,ospparam,param);
                                }
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case LAM :  k = 0;
                    l = 0;
                    for (h=-MAXHKL;h<=MAXHKL;h++) {
                                set_f_hkl(h,k,l,ospparam,param);
                                set_g_hkl(h,k,l,ospparam,param);
                                set_psi_hkl(h,k,l,ospparam,param);
                                if (h!=0 || k!=0 || l!=0) {
                                    Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_aniso(h,k,l,ospparam,param);
                                }
                    }
                    Z=Z*prefactorZ;
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");
    }
    return Z;
}

scalar Lattice_Factor_iso(ordered_particles_param *ospparam, sasfit_param *param)
{
    int h,k,l;
    scalar Z,prefactorZ;
    Z = 0.0;
    h=0;
    k=0;
    l=0;
    init_osp(ospparam,param);
    prefactorZ = pow(2*M_PI/ospparam->Qmod,ospparam->dim-1)*ospparam->cL/ (ospparam->n*ospparam->vd*ospparam->Omega);
    switch (ospparam->order_type_Selector) {
        case SC  :
        case BCC :
        case FCC :
        case HCP :
        case BCT :
                for (h=-MAXHKL;h<=MAXHKL;h++) {
                        for (k=-MAXHKL;k<=MAXHKL;k++) {
                            for (l=-MAXHKL;l<=MAXHKL;l++) {
                                set_f_hkl(h,k,l,ospparam,param);
                                set_g_hkl(h,k,l,ospparam,param);
                                if (h!=0 || k!=0 || l!=0) {
                                    Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                                }
                            }
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case HEX :
        case SQ  :
        case CREC:
                for (h=-MAXHKL;h<=MAXHKL;h++) {
                        for (k=-MAXHKL;k<=MAXHKL;k++) {
                             set_f_hkl(h,k,l,ospparam,param);
                             set_g_hkl(h,k,l,ospparam,param);
                             if (h!=0 || k!=0 || l!=0) {
                                Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                             }
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case LAM :
                    for (h=1;h<=MAXHKL;h++) {
                        set_f_hkl(h,k,l,ospparam,param);
                        set_g_hkl(h,k,l,ospparam,param);
                        if (h!=0 || k!=0 || l!=0) {
                            Z = Z+gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");
    }
    return Z;
}
