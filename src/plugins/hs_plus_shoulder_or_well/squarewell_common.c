#include "include/squarewell_common.h"
#include <math.h>

const double SW_PHAT0[4] = {4.0*M_PI/3.0, M_PI, 4.0*M_PI/5.0, 2.0*M_PI/3.0};

double sw_c0_hat(double kstar, double phi)
{
    double k = kstar;
    double d = pow(1 - phi, 4);
    if (fabs(k) < 0.05) {
        double c0 = M_PI * (pow(phi,3) - 4*phi*phi + 2*phi - 8) / (6.0 * d);
        double c2 = M_PI * (4*phi*phi - 11*phi + 16) / (120.0 * d);
        double c4 = M_PI * (-7*pow(phi,3) - 12*phi*phi + 72*phi - 80) / (16800.0 * d);
        double c6 = M_PI * (6*pow(phi,3) + 4*phi*phi - 41*phi + 40) / (453600.0 * d);
        double k2 = k * k;
        return c0 + c2*k2 + c4*k2*k2 + c6*k2*k2*k2;
    }
    double p1 = phi, p2 = phi*phi, p3 = phi*phi*phi;
    double k2 = k*k, k3 = k2*k, k4 = k2*k2;
    double num =
          k4*p3*cos(k) - 3*k4*p1*cos(k) + 2*k4*cos(k)
        - 10*k3*p3*sin(k) + 12*k3*p1*sin(k) - 2*k3*sin(k)
        - 42*k2*p3*cos(k) - 6*k2*p3 - 24*k2*p2*cos(k) - 24*k2*p2
        + 12*k2*p1*cos(k) - 24*k2*p1
        + 96*k*p3*sin(k) + 96*k*p2*sin(k) + 24*k*p1*sin(k)
        + 96*p3*cos(k) - 96*p3 + 96*p2*cos(k) - 96*p2 + 24*p1*cos(k) - 24*p1;
    return 2.0 * M_PI * num / (pow(k, 6) * d);
}

double sw_fhat_mayer(double k, double x, double lam)
{
    if (fabs(k) < 1e-4) {
        double k2 = k*k;
        return 4*M_PI*x*((pow(lam,3)-1)/3.0 - k2*(pow(lam,5)-1)/30.0
                          + k2*k2*(pow(lam,7)-1)/840.0);
    }
    double f = (k*(cos(k) - lam*cos(lam*k)) + (sin(lam*k) - sin(k))) / pow(k,3);
    return 4*M_PI*x*f;
}

double sw_Phat(int j, double k)
{
    if (fabs(k) < 0.05) {
        double k2 = k*k, k4 = k2*k2, k6 = k4*k2;
        switch (j) {
            case 0: return 4*M_PI/3.0  - 2*M_PI/15.0*k2  + M_PI/210.0*k4   - M_PI/11340.0*k6;
            case 1: return M_PI        - M_PI/9.0*k2     + M_PI/240.0*k4  - M_PI/12600.0*k6;
            case 2: return 4*M_PI/5.0  - 2*M_PI/21.0*k2   + M_PI/270.0*k4  - M_PI/13860.0*k6;
            case 3: return 2*M_PI/3.0  - M_PI/12.0*k2     + M_PI/300.0*k4  - M_PI/15120.0*k6;
        }
    }
    double k2 = k*k, k3 = k2*k, k4 = k2*k2, k5 = k4*k, k6 = k4*k2;
    switch (j) {
        case 0: return 4*M_PI*(-k*cos(k) + sin(k)) / k3;
        case 1: return 4*M_PI*(-k2*cos(k) + 2*k*sin(k) + 2*cos(k) - 2) / k4;
        case 2: return 4*M_PI*(-k3*cos(k) + 3*k2*sin(k) + 6*k*cos(k) - 6*sin(k)) / k5;
        case 3: return 4*M_PI*(-k4*cos(k) + 4*k3*sin(k) + 12*k2*cos(k) - 24*k*sin(k) - 24*cos(k) + 24) / k6;
    }
    return 0.0; /* unreachable for j=0..3 */
}
