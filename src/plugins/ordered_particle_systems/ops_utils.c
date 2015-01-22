/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include "include/cerf.h"
#include "include/defs.h"

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
    QMOD = 2.0*k*sin(VARTHETA/2.0);
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
    QMOD = 2.0*k*sin(VARTHETA/2.0);
}

void set_R_Nautic(ordered_particles_param *ospparam, sasfit_param *param)
{
    // http://upload.wikimedia.org/math/6/3/e/63e6b5b5426a3336d8da8eb60f3825d8.png
    // http://de.wikipedia.org/wiki/Eulersche_Winkel
    // http://en.wikipedia.org/wiki/Euler_angles
    MR[0][0] =cos(BETA)*cos(GAMMA);
    MR[1][0] =sin(ALPHA)*sin(BETA)*cos(GAMMA)-cos(ALPHA)*sin(GAMMA);
    MR[2][0] =cos(ALPHA)*sin(BETA)*cos(GAMMA)+sin(ALPHA)*sin(GAMMA);

    MR[0][1] =cos(BETA)*sin(GAMMA);
    MR[1][1] =sin(ALPHA)*sin(BETA)*sin(GAMMA)+cos(ALPHA)*cos(GAMMA);
    MR[2][1] =cos(ALPHA)*sin(BETA)*sin(GAMMA)-sin(ALPHA)*cos(GAMMA);

    MR[0][2] =-sin(BETA);
    MR[1][2] =sin(ALPHA)*cos(BETA);
    MR[2][2] =cos(ALPHA)*cos(BETA);
}

void set_R_Euler(ordered_particles_param *ospparam, sasfit_param *param)
{
    // http://mathworld.wolfram.com/images/equations/EulerAngles/Inline46.gif
    // http://mathworld.wolfram.com/EulerAngles.html
    // http://en.wikipedia.org/wiki/Euler_angles

    MR[0][0] =cos(PSI)*cos(PHI)-sin(PSI)*cos(THETA)*sin(PHI);
    MR[1][0] =cos(PSI)*sin(PHI)+sin(PSI)*cos(THETA)*cos(PHI);
    MR[2][0] =sin(PSI)*sin(THETA);

    MR[0][1] =-sin(PSI)*cos(PSI)-cos(THETA)*sin(PHI)*cos(PSI);
    MR[1][1] =-sin(PSI)*sin(PSI)+cos(THETA)*cos(PHI)*cos(PSI);
    MR[2][1] = cos(PSI)*sin(THETA);

    MR[0][2] = sin(THETA)*sin(PHI);
    MR[1][2] =-sin(THETA)*cos(PHI);
    MR[2][2] = cos(THETA);
}

void set_f_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param)
{
    switch (ospparam->order_type_Selector) {
        case SC  :  ospparam->f_hkl = 1;
                    break;
        case BCC :  ospparam->f_hkl = lround(1+cos(M_PI*(h+k+l)));
                    break;
        case FCC :  ospparam->f_hkl = lround(1+cos(M_PI*(h+k))+cos(M_PI*(h+l))+cos(M_PI*(k+l)));
                    break;
        case HCP :  ospparam->f_hkl = lround(1+2*cos(2.*M_PI*((h+2*k)/3.+l/4.)));
                    break;
        case BCT :  ospparam->f_hkl = lround(1+cos(M_PI*(h+k+l)));
                    break;
        case HEX :  ospparam->f_hkl = 1;
                    break;
        case SQ  :  ospparam->f_hkl = 1;
                    break;
        case CREC:  ospparam->f_hkl = 1;
                    break;
        case LAM :  ospparam->f_hkl = 1;
                    break;
        case la3d:  switch (100*h+10*k+l) {
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
        case lm3m:  switch (100*h+10*k+l) {
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
        case Pn3m:  switch (100*h+10*k+l) {
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
                    break;
    }
}

scalar qmod_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param)
{
    switch (ospparam->order_type_Selector) {
        case SC  :  return 2*M_PI*sqrt(h*h+k*k+l*l)/(ospparam->ad);
                    break;
        case HCPTWIN:
        case HCP :  return 2*M_PI*sqrt(4./3.*(h*h+k*h+k*k)+3./8.*l*l)/(ospparam->ad);;
                    break;
        case BCCTWIN:
        case BCC :  return 2*M_PI*sqrt(h*h+k*k+l*l)/(ospparam->ad);
                    break;
        case FCCTWIN:
        case FCC :  return 2*M_PI*sqrt(h*h+k*k+l*l)/(ospparam->ad);
                    break;
        case BCT :  return 2*M_PI*sqrt((h*h+k*k)/(ospparam->ad)+l*l/(ospparam->cd));
                    break;
        case SQ  :  return 2*M_PI*sqrt(h*h+k*k)/(ospparam->ad);
                    break;
        case CREC:  return 2*M_PI*sqrt(h*h/gsl_pow_2(ospparam->ad)+k*k/gsl_pow_2((ospparam->bd)));
                    break;
        case HEX :  return 4*M_PI*sqrt(h*h+h*k+k*k)/(sqrt(3.)*ospparam->ad);
                    break;
        case LAM :  return 2*M_PI*h/ospparam->ad;
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");

    }
    return 0.0;
}


void set_m_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param)
{
    ospparam->m_hkl = 0;
    switch (ospparam->order_type_Selector) {
        case SC  :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl=6;
                    } else if (h!=0 && k==h && l==0){
                        ospparam->m_hkl=12;
                    } else if (h!=0 && k==h && l==h){
                        ospparam->m_hkl=8;
                    } else if (h!=0 && k!=0 && l==0){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k==h && l!=h){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k!=0 && l!=0) {
                        ospparam->m_hkl=48;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case BCC :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl=6;
                    } else if (h!=0 && k==h && l==0){
                        ospparam->m_hkl=12;
                    } else if (h!=0 && k==h && l==h){
                        ospparam->m_hkl=8;
                    } else if (h!=0 && k!=0 && l==0){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k==h && l!=h){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k!=0 && l!=0) {
                        ospparam->m_hkl=48;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case FCC :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl=6;
                    } else if (h!=0 && k==h && l==0){
                        ospparam->m_hkl=12;
                    } else if (h!=0 && k==h && l==h){
                        ospparam->m_hkl=8;
                    } else if (h!=0 && k!=0 && l==0){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k==h && l!=h){
                        ospparam->m_hkl=24;
                    } else if (h!=0 && k!=0 && l!=0) {
                        ospparam->m_hkl=48;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case HCP :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl=6;
                    } else if (h==0 && k==0 && l!=0){
                        ospparam->m_hkl=2;
                    } else if (h!=0 && k==h && l==0){
                        ospparam->m_hkl=6;
                    } else if (h!=0 && k!=0 && l==0){
                        ospparam->m_hkl=12;
                    } else if (h==0 && k!=0 && l!=0){
                        ospparam->m_hkl=12;
                    } else if (h!=0 && k==h && l!=0){
                        ospparam->m_hkl=12;
                    } else if (h!=0 && k!=0 && l!=0) {
                        ospparam->m_hkl=24;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case BCT :
                    break;
        case HEX :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl = 6;
                    } else if (h!=0 && k==h && l==0) {
                        ospparam->m_hkl = 6;
                    } else if (h!=0 && k!=0 && l==0) {
                        ospparam->m_hkl = 12;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case SQ  :  if (h!=0 && k==0 && l==0) {
                        ospparam->m_hkl = 4;
                    } else if (h!=0 && k==h && l==0) {
                        ospparam->m_hkl = 4;
                    } else if (h!=0 && k!=0 && l==0) {
                        ospparam->m_hkl = 8;
                    } else {
                        sasfit_out("h=%d, k=%d, l=%d\n",h,k,l);
                        sasfit_err("undefined multiplicity m_hkl! Contact author of the software, if this happens.");
                    }
                    break;
        case CREC:
                    break;

        case LAM :  if (k==0 && l==0) {
                        ospparam->m_hkl = 1;
                    } else {
                        sasfit_err("Miller indices k & l should be zero! Contact author of the software, if this happens.");
                    }
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");
    }
}

void set_g_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    int i;
    switch (ospparam->order_type_Selector) {
        case la3d :
        case lm3m :
        case Pn3m :
        case SC  :
        case HCPTWIN:
        case HCP :
        case BCCTWIN:
        case BCC :
        case FCCTWIN:
        case FCC :
        case BCT :  for (i=0;i<3;i++) {
                        ospparam->ghkl[i]=h*ospparam->ast[i]+k*ospparam->bst[i]+l*ospparam->cst[i];
                    }
                    break;
        case SQ  :
        case CREC:
        case HEX :  for (i=0;i<3;i++) {
                        ospparam->ghkl[i]=h*ospparam->ast[i]+k*ospparam->bst[i];
                    }
                    break;
        case LAM :  for (i=0;i<3;i++) {
                        ospparam->ghkl[i]=h*ospparam->ast[i];
                    }
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");

    }
    ospparam->ghklmod = 0;
    for (i=0;i<3;i++) {
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
    ospparam->psi_hkl=acos(tmp/(ospparam->Qmod*ospparam->ghklmod));
    return;
}

void Cross(scalar V, scalar *x, scalar *y, scalar *res) {
    res[0] = (-x[2]*y[1] + x[1]*y[2])/V;
    res[1] = ( x[2]*y[0] - x[0]*y[2])/V;
    res[2] = (-x[1]*y[0] + x[0]*y[1])/V;
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
        case SC  : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->ad;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=2.0*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=2.0*M_PI/ospparam->ad;
                ospparam->Vd = gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 1;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/6.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
// BCC Body-centered cubic
        case BCC : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->ad;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=2.0*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=2.0*M_PI/ospparam->ad;
                ospparam->Vd = gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->abar = ospparam->ad*sqrt(3)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(3)/8.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
// BCCTWIN: twinned Body-centered cubic
        case BCCTWIN : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->ad;
                ospparam->ast[0]=4.0*M_PI/(3.*ospparam->ad);
                ospparam->ast[1]=2.0*M_PI/(3.*ospparam->ad);
                ospparam->ast[2]=-4.0*M_PI/(3.*ospparam->ad);
                ospparam->bst[0]=2.0*M_PI/(3.*ospparam->ad);
                ospparam->bst[1]=4.0*M_PI/(3.*ospparam->ad);
                ospparam->bst[2]=4.0*M_PI/(3.*ospparam->ad);
                ospparam->cst[0]=4.0*M_PI/(3.*ospparam->ad);
                ospparam->cst[1]=-4.0*M_PI/(3.*ospparam->ad);
                ospparam->cst[2]=2.0*M_PI/(3.*ospparam->ad);
                ospparam->Vd = gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->abar = ospparam->ad*sqrt(3)*0.5;
                ospparam->Omega = 4.0*M_PI;
                ospparam->fphimax = M_PI*sqrt(3)/8.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
// FCC Face-centered cubic (also called cubic close packed)
        case FCC : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->ad;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=2.0*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=2.0*M_PI/ospparam->ad;
                ospparam->Vd = gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 4;
                ospparam->abar = ospparam->ad*sqrt(2)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(2)/6.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
// FCCTWIN twinned Face-centered cubic (also called cubic close packed)
        case FCCTWIN : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->ad;

                ospparam->ast[0]=4.0/3.0*M_PI/ospparam->ad;
                ospparam->ast[1]=-2.0/3.0*M_PI/ospparam->ad;
                ospparam->ast[2]=4.0/3.0*M_PI/ospparam->ad;

                ospparam->bst[0]=4.0/3.0*M_PI/ospparam->ad;
                ospparam->bst[1]=4.0/3.0*M_PI/ospparam->ad;
                ospparam->bst[2]=-2.0/3.0*M_PI/ospparam->ad;

                ospparam->cst[0]=-2.0/3.0*M_PI/ospparam->ad;
                ospparam->cst[1]=4.0/3.0*M_PI/ospparam->ad;
                ospparam->cst[2]=4.0/3.0*M_PI/ospparam->ad;

                ospparam->Vd = gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 4;
                ospparam->abar = ospparam->ad*sqrt(2)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(2)/6.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
// HCP Hexagonal close-packed
        case HCP : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.5*ospparam->ad;
                ospparam->b[1]=sqrt(3.)/2.0*ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=sqrt(8./3.)*ospparam->ad;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=-2.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=4.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=sqrt(3./8.)*2.0*M_PI/ospparam->ad;
                ospparam->Vd = sqrt(2)*gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/(3*sqrt(2));
                MM[0][0]=ospparam->ad;      MM[0][1]=0.0;                       MM[0][2]=0.0;
                MM[1][0]=-ospparam->ad/2.;  MM[1][1]=sqrt(3)*ospparam->ad/2.;   MM[1][2]=0.0;
                MM[2][0]=0.0;               MM[2][1]=0.0;                       MM[2][2]=sqrt(8.0/3.0)*ospparam->ad;

                MN[0][0]=1/ospparam->ad;    MN[0][1]=1./(sqrt(3)*ospparam->ad); MN[0][2]=0.0;
                MN[1][0]=0.0;               MN[1][1]=2./(sqrt(3)*ospparam->ad); MN[1][2]=0.0;
                MN[2][0]=0.0;               MN[2][1]=0.0;                       MN[2][2]=sqrt(3./8.)/ospparam->ad;
                break;
              }
// HCPTWIN twinned Hexagonal close-packed
        case HCPTWIN : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.5*ospparam->ad;
                ospparam->b[1]=sqrt(3.)/2.0*ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=sqrt(8./3.)*ospparam->ad;

                ospparam->ast[0]=-2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;

                ospparam->bst[0]=-2.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->bst[1]= 4.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;

                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=sqrt(3./8.)*2.0*M_PI/ospparam->ad;

                ospparam->Vd = sqrt(2.0)*gsl_pow_3(ospparam->ad);
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 2;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI/(3*sqrt(2));
                MM[0][0]=ospparam->ad;      MM[0][1]=0.0;                       MM[0][2]=0.0;
                MM[1][0]=-ospparam->ad/2.;  MM[1][1]=sqrt(3)*ospparam->ad/2.;   MM[1][2]=0.0;
                MM[2][0]=0.0;               MM[2][1]=0.0;                       MM[2][2]=sqrt(8.0/3.0)*ospparam->ad;

                MN[0][0]=1/ospparam->ad;    MN[0][1]=1./(sqrt(3)*ospparam->ad); MN[0][2]=0.0;
                MN[1][0]=0.0;               MN[1][1]=2./(sqrt(3)*ospparam->ad); MN[1][2]=0.0;
                MN[2][0]=0.0;               MN[2][1]=0.0;                       MN[2][2]=sqrt(3./8.)/ospparam->ad;
                break;
              }
// BCT: body centered tetragonal
        case BCT : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->cd;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=2.0*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=2.0*M_PI/ospparam->cd;
                ospparam->Vd = gsl_pow_2(ospparam->ad)*ospparam->cd;
                ospparam->vd = ospparam->Vd;
                ospparam->dim = 3;
                ospparam->n = 4;
                ospparam->abar = ospparam->ad*sqrt(2)*0.5;
                ospparam->Omega = 4*M_PI;
                ospparam->fphimax = M_PI*sqrt(2)/6.0;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=ospparam->bd;  MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=ospparam->cd;

                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
        case HEX : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.5*ospparam->ad;
                ospparam->b[1]=0.5*sqrt(3)*ospparam->ad;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->cd;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=-2.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=4.0/sqrt(3.0)*M_PI/ospparam->ad;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=0.0;
                ospparam->Vd = sqrt(3.0)*gsl_pow_2(ospparam->ad)*ospparam->cd/2.0;
                ospparam->vd = sqrt(3.0)*gsl_pow_2(ospparam->ad)/2.0;
                ospparam->dim = 2;
                ospparam->n = 1;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 2*M_PI;
                ospparam->fphimax = M_PI/sqrt(3)/2.0;
                MM[0][0]=ospparam->ad;      MM[0][1]=0.0;                       MM[0][2]=0.0;
                MM[1][0]=-ospparam->ad/2.;  MM[1][1]=sqrt(3)*ospparam->ad/2.;   MM[1][2]=0.0;
                MM[2][0]=0.0;               MM[2][1]=0.0;                       MM[2][2]=sqrt(8.0/3.0)*ospparam->ad;

                MN[0][0]=1/ospparam->ad;    MN[0][1]=1./(sqrt(3)*ospparam->ad); MN[0][2]=0.0;
                MN[1][0]=0.0;               MN[1][1]=2./(sqrt(3)*ospparam->ad); MN[1][2]=0.0;
                MN[2][0]=0.0;               MN[2][1]=0.0;                       MN[2][2]=sqrt(3./8.)/ospparam->ad;
                break;
                }
        case SQ  : {
                break;
              }
        case CREC: {
                break;
              }
        case LAM : {
                ospparam->a[0]=ospparam->ad;
                ospparam->a[1]=0.0;
                ospparam->a[2]=0.0;
                ospparam->b[0]=0.0;
                ospparam->b[1]=ospparam->bd;
                ospparam->b[2]=0.0;
                ospparam->c[0]=0.0;
                ospparam->c[1]=0.0;
                ospparam->c[2]=ospparam->cd;
                ospparam->ast[0]=2.0*M_PI/ospparam->ad;
                ospparam->ast[1]=0.0;
                ospparam->ast[2]=0.0;
                ospparam->bst[0]=0.0;
                ospparam->bst[1]=0.0;
                ospparam->bst[2]=0.0;
                ospparam->cst[0]=0.0;
                ospparam->cst[1]=0.0;
                ospparam->cst[2]=0.0;
                ospparam->Vd = ospparam->ad*M_PI*1*1;
                ospparam->vd = ospparam->ad;
                ospparam->dim = 1;
                ospparam->n = 1;
                ospparam->abar = ospparam->ad;
                ospparam->Omega = 1;
                ospparam->fphimax = 1;
                MM[0][0]=ospparam->ad;  MM[0][1]=0.0;           MM[0][2]=0.0;
                MM[1][0]=0.0;           MM[1][1]=1/ospparam->bd;MM[1][2]=0.0;
                MM[2][0]=0.0;           MM[2][1]=0.0;           MM[2][2]=1/ospparam->cd;
                MN[0][0]=1/ospparam->ad;MN[0][1]=0.0;           MN[0][2]=0.0;
                MN[1][0]=0.0;           MN[1][1]=1/ospparam->bd;MN[1][2]=0.0;
                MN[2][0]=0.0;           MN[2][1]=0.0;           MN[2][2]=1/ospparam->cd;
                break;
              }
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
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   if (x > 0.1 ) {
                                Kval = 2*creal(cerfi(C(1.0/(2.0*sqrt(x)),0)));
                                Kval = Kval-creal(cerfi(C(1.0/(2.0*sqrt(x)),M_PI*sqrt(x))));
                                Kval = Kval+creal(cerfi(C(-1.0/(2.0*sqrt(x)),M_PI*sqrt(x))));
                                Kval = Kval*sqrt(M_PI/(16.0*x))*exp(-1.0/(4.0*x));
                                return Kval;
                            } else {
                                t1 = sqrt(M_PI);
                                t2 = sqrt(x);
                                t3 = 0.1e1 / t2;
                                t4 = t3 / 0.2e1;
                                t5 = cimag(w_of_z(C(-t4,0)));
                                t10 = t2 * M_PI;
                                t12 = cimag(w_of_z(C(-t4,-t10)));
                                t14 = M_PI * M_PI;
                                t16 = exp(x * t14);
                                t18 = t3 / t16;
                                t22 = cimag(w_of_z(C(-t4,t10)));
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
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
    return Kval;
}

scalar G(ordered_particles_param *ospparam, sasfit_param *param) {
    return exp(-gsl_pow_2(SIGMA_A*ABAR*QMOD));
}

scalar Lq_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar x, delta, gamma_nu, lnGnu_2, y, nu;
    int i;
    gsl_sf_result lnr, arg;

    gsl_set_error_handler_off();

    x = 0.0;
    for (i=0;i<3;i++) {
        x = x+gsl_pow_2(ospparam->q[i]-ospparam->ghkl[i]);
    }
    x=sqrt(x);
    x = ospparam->Qmod-ospparam->ghklmod;
    delta = ospparam->delta_q;
    nu = ospparam->nu;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   return 2.0/(M_PI*delta) * exp(-4.0*x*x/(gsl_pow_2(delta)*M_PI));
                            break;
    case LORENTZIANPEAK :   return 2.0/(M_PI*delta) /(1.0+4.0*x*x+gsl_pow_2(delta));
                            break;
    case BURGERMICHA    :   lnGnu_2 = gsl_sf_lngamma(0.5*nu);
                            gamma_nu = sqrt(M_PI)*exp(gsl_sf_lngamma((nu+1)/2.0)-lnGnu_2);
                            y = gsl_pow_2(2.0*x/(M_PI*delta));
                            gsl_sf_lngamma_complex_e(0.5*nu,gamma_nu*y,&lnr,&arg);
                            return 2.0/(M_PI*delta)*gsl_pow_2(exp(lnr.val-lnGnu_2));
                            break;
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
}

scalar Lpsi_hkl(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar cG, cL, x, bhkl, ahkl, deltapsi;
    cG=1.0;
    cL=1.0;
    deltapsi = ospparam->delta_psi;
    x = ospparam->Qmod*ospparam->psi_hkl;
    switch (ospparam->peak_shape_type_Selector) {
    case GAUSSIANPEAK   :   ahkl = 4*gsl_pow_2(ospparam->ghklmod/deltapsi)/M_PI;
                            cG = 1.0/(2.0*M_PI*gsl_pow_2(ospparam->ghklmod)*K(ahkl,ospparam,param));
//                            sasfit_out("Gauss: ghkl: %lg bhkl=%lg cL=%lg\n",ospparam->ghklmod, bhkl,cG);
                            return cG * exp(-4.*x*x/(gsl_pow_2(deltapsi)*M_PI));
                            break;
    case LORENTZIANPEAK :   bhkl = 4*gsl_pow_2(ospparam->ghklmod/deltapsi);
                            cL = 1.0/(2.0*M_PI*gsl_pow_2(ospparam->ghklmod)*K(bhkl,ospparam,param));
//                            sasfit_out("Lorentz: ghkl: %lg bhkl=%lg cL=%lg\n",ospparam->ghklmod, bhkl,cL);
                            return cL / (1.0+4.0*x*x+gsl_pow_2(deltapsi));
                            break;
    default :               sasfit_err("UNKNOWN peak shape\nInform author if this happens!\n");
                            return 0.0;
    }
}

scalar L_hkl_aniso(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
//    return Lpsi_hkl(h,k,l,ospparam,param);
    return  Lq_hkl(h,k,l,ospparam,param)*Lpsi_hkl(h,k,l,ospparam,param);
}

scalar L_hkl_iso(int h, int k, int l, ordered_particles_param *ospparam, sasfit_param *param) {
    scalar x, delta, gamma_nu, lnGnu_2, y, nu;
    gsl_sf_result lnr, arg;

    gsl_set_error_handler_off();

    x = ospparam->Qmod-qmod_hkl(h,k,l,ospparam,param);
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
    prefactorZ = 1+0*gsl_pow_3(2*M_PI)/(ospparam->n*ospparam->vd);
    init_osp(ospparam,param);
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
                            set_m_hkl(h,k,l,ospparam,param);
                            set_f_hkl(h,k,l,ospparam,param);
                            Z = Z+ospparam->m_hkl*gsl_pow_2(ospparam->f_hkl)*L_hkl_aniso(h,k,l,ospparam,param);
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case LAM :  k = 0;
                    l = 0;
                    for (h=-MAXHKL;h<=MAXHKL;h++) {
                        set_m_hkl(h,k,l,ospparam,param);
                        set_f_hkl(h,k,l,ospparam,param);
                        Z = Z+ospparam->m_hkl*gsl_pow_2(ospparam->f_hkl)*L_hkl_aniso(h,k,l,ospparam,param);
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
    prefactorZ = pow(2*M_PI,ospparam->dim-1)*ospparam->cL/(ospparam->n*ospparam->vd*ospparam->Omega*pow(ospparam->Qmod,ospparam->dim-1));
    switch (ospparam->order_type_Selector) {
        case SC  :
        case BCC :
        case FCC :
        case HCP :
        case BCT :  for (h=0;h<=MAXHKL;h++) {
                        for (k=0;k<=MAXHKL;k++) {
                            for (l=0;l<=MAXHKL;l++) {
                                set_m_hkl(h,k,l,ospparam,param);
                                set_f_hkl(h,k,l,ospparam,param);
                                if (h!=0 || k!=0 || l!=0) {
                                    Z = Z+ospparam->m_hkl*gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                                }
                            }
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case HEX :
        case SQ  :
        case CREC:  l = 0;
                    for (h=0;h<=MAXHKL;h++) {
                        for (k=0;k<=MAXHKL;k++) {
                            set_m_hkl(h,k,l,ospparam,param);
                            set_f_hkl(h,k,l,ospparam,param);
                            Z = Z+ospparam->m_hkl*gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                        }
                    }
                    Z=Z*prefactorZ;
                    break;
        case LAM :  k = 0;
                    l = 0;
                    for (h=0;h<=MAXHKL;h++) {
                        set_m_hkl(h,k,l,ospparam,param);
                        set_f_hkl(h,k,l,ospparam,param);
                        Z = Z+ospparam->m_hkl*gsl_pow_2(ospparam->f_hkl)*L_hkl_iso(h,k,l,ospparam,param);
                    }
                    Z=Z*prefactorZ;
                    break;
        default:    sasfit_err("Unknown type of ordered mesoscopic structure");
    }
    return Z;
}
