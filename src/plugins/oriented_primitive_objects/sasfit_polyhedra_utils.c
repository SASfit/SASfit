/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

#define EPS_OH 1e-4
scalar OH_A(scalar Q1, scalar Q2) {
    scalar Q12, Q22, Q23,Q24;
    Q12=Q1*Q1;
    Q22=Q2*Q2;
    Q23=Q2*Q22;
    Q24=Q22*Q22;
    if        (fabs(Q1-Q2) <= EPS_OH && fabs(Q1+Q2)>EPS_OH && fabs(Q2)>EPS_OH) {
        return (Q2*(3*Q22 - 2*Q24 + 4*Q1*Q2*(3 + Q22) - Q12*(3 + 2*Q22))*cos(Q2) +
                3*(Q12 - 4*Q1*Q2 - (-7 + Q12)*Q22 + Q24)*sin(Q2))/(3.*Q23);
    } else if (fabs(Q1-Q2) > EPS_OH && fabs(Q1+Q2)<=EPS_OH && fabs(Q2)>EPS_OH) {
        return (-(Q2*(4*Q1*Q2*(3 + Q22) + Q22*(-3 + 2*Q22) + Q12*(3 + 2*Q22))*cos(Q2)) +
                3*(Q12 + 4*Q1*Q2 - (-7 + Q12)*Q22 + Q24)*sin(Q2))/(3.*Q23);
    } else if (fabs(Q1-Q2) <= EPS_OH && fabs(Q1+Q2)<= EPS_OH) {
        return 4*(6 - Q12)/3. + (-20 + 7*Q12)*Q22/15.;
    } else {
        return 8*(Q1*sin(Q1)-Q2*sin(Q2))/(Q12-Q22);
    }
}

scalar opo_FOH(opo_data *opod) {
    scalar Q1,Q12, Q2,Q22,Q23,Q24,Q26,Q28, Q3,Q32,Q33,Q34,Q35,Q36,Q37;
    Q1 = opod->Qhat[0];
    Q2 = opod->Qhat[1];
    Q3 = opod->Qhat[2];
    Q12=Q1*Q1;
    Q22=Q2*Q2;
    Q23=Q2*Q22;
    Q24=Q22*Q22;
    Q26=Q22*Q24;
    Q28=Q24*Q24;
    Q32=Q3*Q3;
    Q33=Q32*Q3;
    Q34=Q32*Q32;
    Q35=Q34*Q3;
    Q36=Q32*Q34;
    Q37=Q32*Q35;
//    if (fabs(Q12-Q22)<EPS_OH || fabs(Q22-Q32)<EPS_OH|| fabs(Q12-Q22)<EPS_OH) {
//        sasfit_out("q:%lf Q1:%lf Q2:%lf Q3:%lf\n",opod->Qmod,Q1,Q2,Q3);
//    }
goto label10;
label101:
    if (fabs(Q1)<=EPS_OH&&fabs(Q2)<=EPS_OH&&fabs(Q3)>EPS_OH) {
        return (120*Q22*Q33 - 20*(-6 + Q22)*Q35 +
                Q12*Q3*(120*Q22 - 20*(-6 + Q22)*Q32 + (-20 + Q22)*Q34) -
                120*(Q12 + Q32)*(Q22 + Q32)*sin(Q3))/(15.*Q37);
    } else if (fabs(Q12-Q22)<=EPS_OH && fabs(Q2)>EPS_OH && fabs(Q32-Q22)>EPS_OH) {
        return (2*(Q2*(Q2 - Q3)*(Q2 + Q3)*((3*Q1 - 5*Q2)*Q22 + (Q1 + Q2)*Q32)*cos(Q2) +
                (Q24*(5*Q2 - Q23 + Q1*(-3 + Q22)) - 2*(Q1 - Q2)*Q22*(3 + Q22)*Q32 +
                (Q1*(1 + Q22) - Q2*(3 + Q22))*Q34)*sin(Q2) +
                4*Q22*Q3*(2*Q1*Q2 - 3*Q22 + Q32)*sin(Q3)))/(Q22*gsl_pow_3(Q22 - Q32));
    } else if (fabs(Q12-Q22)<=EPS_OH && fabs(Q32-Q22)<=EPS_OH && fabs(Q3)>EPS_OH) {
        return  -(Q3*(33*Q1*Q2 + 45*(Q1 + Q2)*Q3 + (69 + 2*Q1*Q2)*Q32 + 6*(Q1 + Q2)*Q33 + 10*Q34)*cos(Q3) +
                3*(-11*Q1*Q2 - 15*(Q1 + Q2)*Q3 + (-23 + 3*Q1*Q2)*Q32 + 3*(Q1 + Q2)*Q33 + (-1 + Q1*Q2)*Q34 +
                (Q1 + Q2)*Q35 + Q36)*sin(Q3))/(12.*Q35);
    } else if (fabs(Q1)<=EPS_OH && fabs(Q2)<=EPS_OH && fabs(Q3)<=EPS_OH) {
        return 4./3. - Q12/15. + ((-42 + Q12)*Q32)/630. -
                    (Q22*(Q12*(-72 + Q32) - 72*(-42 + Q32)))/45360.;
    } else  {
        return (OH_A(Q2,Q3)-OH_A(Q1,Q3))/(Q12-Q22);
    }

label10:
    if (fabs(Q1)<=EPS_OH&&fabs(Q2)<=EPS_OH&&fabs(Q3)>EPS_OH) {
        return (120*Q22*Q33 - 20*(-6 + Q22)*Q35 +
                Q12*Q3*(120*Q22 - 20*(-6 + Q22)*Q32 + (-20 + Q22)*Q34) -
                120*(Q12 + Q32)*(Q22 + Q32)*sin(Q3))/(15.*Q37);
    } else if (fabs(Q1-Q2)<=EPS_OH && fabs(Q1+Q2)>EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)>EPS_OH&& fabs(Q2+Q3)>EPS_OH) {
        return (2*(Q2*(Q2 - Q3)*(Q2 + Q3)*((3*Q1 - 5*Q2)*Q22 + (Q1 + Q2)*Q32)*cos(Q2) +
                (Q24*(5*Q2 - Q23 + Q1*(-3 + Q22)) - 2*(Q1 - Q2)*Q22*(3 + Q22)*Q32 +
                (Q1*(1 + Q22) - Q2*(3 + Q22))*Q34)*sin(Q2) +
                4*Q22*Q3*(2*Q1*Q2 - 3*Q22 + Q32)*sin(Q3)))/(Q22*gsl_pow_3(Q22 - Q32));
    } else if (fabs(Q1-Q2)>EPS_OH && fabs(Q1+Q2)<=EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)>EPS_OH&& fabs(Q2+Q3)>EPS_OH) {
        return (-2*Q2*(Q2 - Q3)*(Q2 + Q3)*(Q22*(3*Q1 + 5*Q2) + (Q1 - Q2)*Q32)*cos(Q2) -
                2*(Q24*(-5*Q2 + Q23 + Q1*(-3 + Q22)) - 2*Q22*(Q1 + Q2)*(3 + Q22)*Q32 +
                (Q1 + 3*Q2 + Q1*Q22 + Q23)*Q34)*sin(Q2) +
                8*Q22*Q3*(-2*Q1*Q2 - 3*Q22 + Q32)*sin(Q3))/(Q22*gsl_pow_3(Q22 - Q32));
    } else if (fabs(Q1-Q2)<=EPS_OH && fabs(Q1+Q2)>EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)>EPS_OH&& fabs(Q2+Q3)<=EPS_OH && fabs(Q3)>EPS_OH) {
        return  -(Q3*(33*Q1*Q2 + 45*(Q1 + Q2)*Q3 + (69 + 2*Q1*Q2)*Q32 + 6*(Q1 + Q2)*Q33 + 10*Q34)*cos(Q3) +
                3*(-11*Q1*Q2 - 15*(Q1 + Q2)*Q3 + (-23 + 3*Q1*Q2)*Q32 + 3*(Q1 + Q2)*Q33 + (-1 + Q1*Q2)*Q34 +
                (Q1 + Q2)*Q35 + Q36)*sin(Q3))/(12.*Q35);
    } else if (fabs(Q1-Q2)<=EPS_OH && fabs(Q1+Q2)>EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)<=EPS_OH&& fabs(Q2+Q3)>EPS_OH && fabs(Q3)>EPS_OH) {
        return (Q3*(-33*Q1*Q2 + 45*(Q1 + Q2)*Q3 - (69 + 2*Q1*Q2)*Q32 + 6*(Q1 + Q2)*Q33 - 10*Q34)*cos(Q3) +
                3*(11*Q1*Q2 - 15*(Q1 + Q2)*Q3 + (23 - 3*Q1*Q2)*Q32 + 3*(Q1 + Q2)*Q33 + (1 - Q1*Q2)*Q34 +
                (Q1 + Q2)*Q35 - Q36)*sin(Q3))/(12.*Q35);
    } else if (fabs(Q1-Q2)>EPS_OH && fabs(Q1+Q2)<=EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)>EPS_OH&& fabs(Q2+Q3)<=EPS_OH && fabs(Q3)>EPS_OH) {
        return (Q3*(33*Q1*Q2 + 45*(Q1 - Q2)*Q3 + (-69 + 2*Q1*Q2)*Q32 + 6*(Q1 - Q2)*Q33 - 10*Q34)*cos(Q3) +
                3*(23*Q32 + Q34 - Q36 + Q1*Q3*(-15 + 3*Q32 + Q34) -
                Q2*Q3*(-15 + 3*Q32 + Q34) + Q1*Q2*(-11 + 3*Q32 + Q34))*sin(Q3))/(12.*Q35);
    } else if (fabs(Q1-Q2)>EPS_OH && fabs(Q1+Q2)<=EPS_OH && fabs(Q2)>EPS_OH && fabs(Q3-Q2)<=EPS_OH&& fabs(Q2+Q3)>EPS_OH && fabs(Q3)>EPS_OH) {
        return (Q3*(33*Q1*Q2 + 45*(-Q1 + Q2)*Q3 + (-69 + 2*Q1*Q2)*Q32 + 6*(-Q1 + Q2)*Q33 - 10*Q34)*cos(Q3) +
                3*(Q3*(23*Q3 + Q33 - Q35 + Q2*(-15 + 3*Q32 + Q34)) +
                Q1*(15*Q3 - Q33*(3 + Q32) + Q2*(-11 + 3*Q32 + Q34)))*sin(Q3))/(12.*Q35);
    } else if (fabs(Q1)<=EPS_OH && fabs(Q2)<=EPS_OH && fabs(Q3)<=EPS_OH) {
        return 4./3. - Q12/15. + ((-42 + Q12)*Q32)/630. -
                    (Q22*(Q12*(-72 + Q32) - 72*(-42 + Q32)))/45360.;
    } else  {
        return (OH_A(Q2,Q3)-OH_A(Q1,Q3))/(Q12-Q22);
    }
label1011:
    if (fabs(Q12-Q22) < EPS_OH && fabs(Q22) >= EPS_OH && fabs(Q32) >= EPS_OH) {
        return -(12*gsl_pow_2(Q23 - Q2*Q32)*(Q2*(Q22 - Q32)*cos(Q2) - (Q22 + Q32)*sin(Q2) +
                2*Q2*Q3*sin(Q3)) - 6*(Q1 - Q2)*Q2*(Q2 - Q3)*(Q2 + Q3)*
                (Q2*(Q2 - Q3)*(Q2 + Q3)*(3*Q22 + Q32)*cos(Q2) +
                (Q26 + Q34 + Q22*Q32*(-6 + Q32) - Q24*(3 + 2*Q32))*sin(Q2) +
                8*Q23*Q3*sin(Q3)) + gsl_pow_2(Q1 - Q2)*(-(Q2*(Q2 - Q3)*(Q2 + Q3)*
                (2*Q26 + 3*Q34 + 2*Q22*Q32*(-15 + Q32) - Q24*(21 + 4*Q32))*cos(Q2))\
                + 3*(3*Q28 - Q36 + Q24*Q32*(-27 + Q32) + Q22*Q34*(3 + Q32) -
                Q26*(7 + 5*Q32))*sin(Q2) + 24*Q23*Q3*(3*Q22 + Q32)*sin(Q3)))/
                (3.*Q23*gsl_pow_4(Q2 - Q3)*gsl_pow_4(Q2 + Q3));
    } else if (fabs(Q12-Q22) < EPS_OH && fabs(Q22) < EPS_OH && fabs(Q32) >= EPS_OH) {
        return (120*Q22*Q33 - 20*(-6 + Q22)*Q35 +
                Q12*(-20*Q33*(-6 + Q32) + 7*Q22*Q3*(120 - 20*Q33 + Q34)) -
                120*(7*Q12*Q22 + (Q12 + Q22)*Q32 + Q34)*sin(Q3))/(15.*Q37);
    } else if (fabs(Q12-Q22) < EPS_OH && fabs(Q22) < EPS_OH && fabs(Q32) < EPS_OH) {
        return (-6*(-20 + Q22) + Q12*(-6 + Q22))/90. +
                ((Q12*(72 - 7*Q22) + 72*(-42 + Q22))*Q32)/45360.;
    } else {
        return (OH_A(Q2,Q3)-OH_A(Q1,Q3))/(Q12-Q22);
    }
}
