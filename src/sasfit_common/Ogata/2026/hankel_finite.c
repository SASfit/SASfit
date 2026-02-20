#include <gsl/gsl_integration.h>
#include <gsl/gsl_sf_bessel.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include "hankel_adaptive_finite.h"
#include "hankel.h"

typedef struct {
    hankel_func_t f;
    double nu;
    double k;
    void *user;
} finite_params_t;

static double finite_integrand(double r, void *p_) {
    finite_params_t *p = p_;
    return p->f(r, p->user)*r*gsl_sf_bessel_Jnu(p->nu, p->k*r);
}

double hankel_finite_GL(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int n_gl
) {
    gsl_function F;
    finite_params_t p = {f, nu, k, user_data};
    F.function = &finite_integrand;
    F.params = &p;

    gsl_integration_glfixed_table *T =
        gsl_integration_glfixed_table_alloc(n_gl);

    double result = gsl_integration_glfixed(&F, 0.0, R, T);
    gsl_integration_glfixed_table_free(T);
    return result;
}

double hankel_finite_GL_adaptive(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int n_gl_start,
    double tol
) {
    /* Log invocation so we can see when adaptive GL is used */
    {
        FILE *lf = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log", "a");
        if (lf) {
            fprintf(lf, "GL_ADAPT nu=%g k=%g R=%g n_start=%d tol=%g\n", nu, k, R, n_gl_start, tol);
            fclose(lf);
        }
    }
    if (n_gl_start <= 0) n_gl_start = 16;
    if (tol <= 0.0) tol = 1e-8;

    double prev = 0.0;
    int first = 1;
    int n = n_gl_start;
    const int n_max = 65536;

    while (n <= n_max) {
        gsl_integration_glfixed_table *T = gsl_integration_glfixed_table_alloc(n);
        gsl_function F;
        finite_params_t p = {f, nu, k, user_data};
        F.function = &finite_integrand;
        F.params = &p;
        double res = gsl_integration_glfixed(&F, 0.0, R, T);
        gsl_integration_glfixed_table_free(T);

        if (!first) {
            double rel = (res != 0.0) ? fabs((res - prev)/res) : fabs(res - prev);
            if (rel < tol) return res;
        }
        prev = res;
        first = 0;
        n *= 2;
    }
    return prev;
}

void hankel_finite_GL_adaptive_vector(
    hankel_func_t f,
    void *user_data,
    double nu,
    const double *k_array,
    int nk,
    double R,
    int n_gl_start,
    double tol,
    double *H_out
) {
    if (!k_array || !H_out || nk <= 0) return;
    for (int i = 0; i < nk; ++i) {
        H_out[i] = hankel_finite_GL_adaptive(f, user_data, nu, k_array[i], R, n_gl_start, tol);
    }
}

void hankel_finite_GL_vector(
    hankel_func_t f,
    void *user_data,
    double nu,
    const double *k_array,
    int nk,
    double R,
    int n_gl,
    double *H_out
) {
    gsl_integration_glfixed_table *T =
        gsl_integration_glfixed_table_alloc(n_gl);

    /* #pragma omp parallel for */
    for (int i = 0; i < nk; ++i) {
        gsl_function F;
        finite_params_t p = {f, nu, k_array[i], user_data};
        F.function = &finite_integrand;
        F.params = &p;
        H_out[i] = gsl_integration_glfixed(&F, 0.0, R, T);
    }

    gsl_integration_glfixed_table_free(T);
}

/* ---------------- Composite Filon for [0,R] ---------------- */

/* Integrand for moment: t^q * J_n(t) */
typedef struct { int n; int q; } moment_ctx_t;
static double moment_integrand(double t, void *p_) {
    moment_ctx_t *p = (moment_ctx_t*)p_;
    int n = p->n; int q = p->q;
    double J = gsl_sf_bessel_Jn(n, t);
    double tq = 1.0;
    for (int i = 0; i < q; ++i) tq *= t;
    return tq * J;
}

static double compute_moment(int n, int q, double t0, double t1, double tol) {
    if (t1 <= t0) return 0.0;

    /* Analytic moments for common cases (improves accuracy and speed)
       For order n==1: M0 = \int J1 = -J0 |; M1 = t J2 |; M2 = t^2 J2 |
       For order n==0: M1 = t J1 |; M2 = t^2 J1 - t J2 |
    */
    if (n == 1) {
        switch (q) {
            case 0: {
                double J0_t1 = gsl_sf_bessel_J0(t1);
                double J0_t0 = gsl_sf_bessel_J0(t0);
                return (-J0_t1) - (-J0_t0); /* -J0(t1) + J0(t0) */
            }
            case 1: {
                double J2_t1 = gsl_sf_bessel_Jn(2, t1);
                double J2_t0 = gsl_sf_bessel_Jn(2, t0);
                return t1 * J2_t1 - t0 * J2_t0;
            }
            case 2: {
                double J2_t1 = gsl_sf_bessel_Jn(2, t1);
                double J2_t0 = gsl_sf_bessel_Jn(2, t0);
                return t1*t1 * J2_t1 - t0*t0 * J2_t0;
            }
            default: break;
        }
    }

    if (n == 0) {
        switch (q) {
            case 1: {
                double J1_t1 = gsl_sf_bessel_Jn(1, t1);
                double J1_t0 = gsl_sf_bessel_Jn(1, t0);
                return t1 * J1_t1 - t0 * J1_t0;
            }
            case 2: {
                double J1_t1 = gsl_sf_bessel_Jn(1, t1);
                double J1_t0 = gsl_sf_bessel_Jn(1, t0);
                double J2_t1 = gsl_sf_bessel_Jn(2, t1);
                double J2_t0 = gsl_sf_bessel_Jn(2, t0);
                return (t1*t1 * J1_t1 - t0*t0 * J1_t0) - (t1 * J2_t1 - t0 * J2_t0);
            }
            default: break;
        }
    }

    /* Fallback: numeric integration for general (n,q) */
    gsl_function F;
    moment_ctx_t ctx = {n, q};
    F.function = &moment_integrand;
    F.params = &ctx;
    gsl_integration_workspace *w = gsl_integration_workspace_alloc(5000);
    double result = 0.0, abserr = 0.0;
    gsl_integration_qag(&F, t0, t1, 0.0, tol, 5000, GSL_INTEG_GAUSS61, w, &result, &abserr);
    gsl_integration_workspace_free(w);
    return result;
}

/* Solve 3x3 Vandermonde for quadratic fit c0 + c1 r + c2 r^2 through three points */
static void fit_quadratic_from_three(const double *r, const double *g, double *c) {
    /* Solve V * c = g where V_{ij} = r_i^{j} with j=0..2 */
    double A[9] = {1.0, r[0], r[0]*r[0], 1.0, r[1], r[1]*r[1], 1.0, r[2], r[2]*r[2]};
    double b[3] = {g[0], g[1], g[2]};
    /* Gaussian elimination (3x3) */
    for (int i = 0; i < 3; ++i) {
        int piv = i;
        for (int j = i+1; j < 3; ++j) if (fabs(A[j*3+i]) > fabs(A[piv*3+i])) piv = j;
        if (piv != i) {
            for (int k = 0; k < 3; ++k) { double tmp = A[i*3+k]; A[i*3+k] = A[piv*3+k]; A[piv*3+k] = tmp; }
            double tmpb = b[i]; b[i] = b[piv]; b[piv] = tmpb;
        }
        double diag = A[i*3+i];
        if (fabs(diag) < 1e-16) { c[0]=c[1]=c[2]=0.0; return; }
        for (int k = i; k < 3; ++k) A[i*3+k] /= diag;
        b[i] /= diag;
        for (int j = i+1; j < 3; ++j) {
            double fac = A[j*3+i];
            for (int k = i; k < 3; ++k) A[j*3+k] -= fac * A[i*3+k];
            b[j] -= fac * b[i];
        }
    }
    double x[3];
    for (int i = 2; i >= 0; --i) {
        double s = b[i];
        for (int j = i+1; j < 3; ++j) s -= A[i*3+j]*x[j];
        x[i] = s;
    }
    c[0]=x[0]; c[1]=x[1]; c[2]=x[2];
}

double hankel_finite_filon(
    hankel_func_t f,
    void *user_data,
    double nu,
    double k,
    double R,
    int npanels,
    double tol
) {
    if (k <= 0.0 || R <= 0.0) return 0.0;
    if (tol <= 0.0) tol = 1e-8;

    int panels = 0;
    if (npanels > 0) {
        panels = npanels;
    } else {
        /* Increase panel density to improve Filon accuracy; default 40 panels per k*R */
        panels = (int)ceil(40.0 * k * R);
        if (panels < 64) panels = 64;
        if (panels > 200000) panels = 200000;
    }

    /* Trace and log invocation so we can confirm Filon is being exercised */
    printf("[FILON] k=%g R=%g panels=%d npanels=%d\n", k, R, panels, npanels);
    {
        FILE *lf = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log","a");
        if (lf) {
            fprintf(lf, "FILON_INVOKE k=%g R=%g panels=%d npanels=%d\n", k, R, panels, npanels);
            fclose(lf);
        }
    }

    double sum = 0.0;
    for (int p = 0; p < panels; ++p) {
        double r_start = R * (double)p / (double)panels;
        double r_end = R * (double)(p+1) / (double)panels;
        double r_points[3] = { r_start, 0.5*(r_start + r_end), r_end };
        double gvals[3];
        for (int j = 0; j < 3; ++j) gvals[j] = r_points[j] * f(r_points[j], user_data);
        double c[3]; fit_quadratic_from_three(r_points, gvals, c);

        double t0 = k * r_points[0];
        double t2 = k * r_points[2];
        double M0 = compute_moment((int)nu, 0, t0, t2, tol);
        double M1 = compute_moment((int)nu, 1, t0, t2, tol);
        double M2 = compute_moment((int)nu, 2, t0, t2, tol);

        double panel_sum = 0.0;
        double kpow1 = k; /* k^{1} */
        panel_sum += c[0] * (M0 / kpow1);
        panel_sum += c[1] * (M1 / (kpow1*k));
        panel_sum += c[2] * (M2 / (kpow1*k*k));

        sum += panel_sum;

        /* Instrumentation: compare Filon panel contribution to direct GL on this panel */
        {
            FILE *lf = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log","a");
            if (lf) {
                double direct = 0.0;
                int do_direct = 1;
                /* For very large panel counts compute direct only every so often to limit cost */
                if (panels > 4096) {
                    int stride = panels / 4096;
                    if ((p % stride) != 0) do_direct = 0;
                }
                if (do_direct) {
                    gsl_function F;
                    finite_params_t params = {f, (double)nu, k, user_data};
                    F.function = &finite_integrand;
                    F.params = &params;
                    gsl_integration_workspace *w = gsl_integration_workspace_alloc(500);
                    double abserr = 0.0;
                    gsl_integration_qag(&F, r_points[0], r_points[2], 0.0, tol, 500, GSL_INTEG_GAUSS61, w, &direct, &abserr);
                    gsl_integration_workspace_free(w);
                }
                fprintf(lf, "FILON_PANEL k=%g p=%d r0=%g r2=%g panel_sum=%g direct=%g\n", k, p, r_points[0], r_points[2], panel_sum, direct);
                fclose(lf);
            }
        }
    }
    return sum;
}
