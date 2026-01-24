#include <math.h>
#include <gsl/gsl_sf_bessel.h>
#include "hankel.h"

/* Helper: compute x_m, w_m either from table or on the fly */
static inline void get_xw(
    int m, double h,
    const hankel_ogata_table_t *tab,
    double *x_out, double *w_out
) {
    if (tab) {
        int idx = m + tab->Nmax;
        *x_out = tab->x[idx];
        *w_out = tab->w[idx];
    } else {
        double t = m * h;
        double sh = sinh(t);
        double u  = (M_PI/2.0)*sh;
        double ps = tanh(u);
        double ch = cosh(t);
        double cu = cosh(u);
        double dps = (M_PI/2.0)*ch/(cu*cu);
        *x_out = (M_PI/h)*ps;
        *w_out = dps;
    }
}

double hankel_ogata_nu(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double h,
    int N_start,
    int N_step,
    int Nmax,
    double tol,
    const hankel_ogata_table_t *tab
) {
    double sum = 0.0;

    /* Initial block */
    for (int m = -N_start; m <= N_start; ++m) {
        double x, w;
        get_xw(m, h, tab, &x, &w);
        if (x <= 0.0) continue;
        double r = x/k;
        sum += f(r, user_data)*x*gsl_sf_bessel_Jnu(nu, x)*w;
    }

    double H_prev = (h/(k*k))*sum;
    int N_old = N_start;

    /* Incremental adaptive loop */
    for (int N_new = N_start + N_step; N_new <= Nmax; N_new += N_step) {

        /* positive m */
        for (int m = N_old+1; m <= N_new; ++m) {
            double x, w;
            get_xw(m, h, tab, &x, &w);
                if (x > 0.0) {
                double r = x/k;
                sum += f(r, user_data)*x*gsl_sf_bessel_Jnu(nu, x)*w;
            }
        }

        /* negative m */
        for (int m = -(N_old+1); m >= -N_new; --m) {
            double x, w;
            get_xw(m, h, tab, &x, &w);
                if (x > 0.0) {
                double r = x/k;
                sum += f(r, user_data)*x*gsl_sf_bessel_Jnu(nu, x)*w;
            }
        }

        double H_curr = (h/(k*k))*sum;
        if (fabs(H_curr - H_prev) < tol)
            return H_curr;

        H_prev = H_curr;
        N_old = N_new;
    }

    return H_prev;
}

double hankel_ogata_nu_extrapolate(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double h,
    int N_start,
    int N_step,
    int Nmax,
    double tol,
    const hankel_ogata_table_t *tab
) {
    /* Use three evaluations (h, h/2, h/4) to estimate convergence order p
       and perform Richardson extrapolation using the two finest values. */
    double Hh  = hankel_ogata_nu(f, user_data, nu, k, h,       N_start,     N_step,     Nmax,     tol, tab);
    double Hh2 = hankel_ogata_nu(f, user_data, nu, k, h*0.5,   N_start*2,   N_step*2,   Nmax*2,   fmax(tol*0.25, 1e-16), tab);
    double Hh4 = hankel_ogata_nu(f, user_data, nu, k, h*0.25,  N_start*4,   N_step*4,   Nmax*4,   fmax(tol*0.0625, 1e-20), tab);

    double dh1 = Hh  - Hh2; /* approx C*h^p - C*(h/2)^p = C*h^p*(1 - 2^{-p}) */
    double dh2 = Hh2 - Hh4;

    double p = 2.0; /* default */
    if (dh1 != 0.0 && dh2 != 0.0) {
        double ratio = dh1 / dh2;
        if (ratio > 0.0) {
            p = log(ratio) / log(2.0);
            if (!isfinite(p) || p <= 0.0) p = 2.0;
            if (p > 12.0) p = 12.0;
        }
    }

    double denom = pow(2.0, p) - 1.0;
    if (denom == 0.0 || !isfinite(denom)) {
        return Hh4; /* can't extrapolate reliably */
    }

    /* Extrapolate using finest two: H ≈ H(h/4) + (H(h/4)-H(h/2))/(2^p - 1) */
    double H_ex = Hh4 + (Hh4 - Hh2) / denom;
    return H_ex;
}
