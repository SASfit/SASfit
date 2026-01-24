#include <math.h>
#include <stddef.h>
#include <gsl/gsl_sf_bessel.h>
#include "hankel.h"
#include "hankel_adaptive.h"
#include "hankel_adaptive_finite.h"

/* ================================================================
   1. Bessel J_n helper (you can replace jn() with your own)
   ================================================================ */

/* Asymptotic J_n(z) for large z */
static inline double bessel_jn_asymptotic(int n, double z)
{
    double phase = z - 0.5 * n * M_PI - 0.25 * M_PI;
    return sqrt(2.0 / (M_PI * z)) * cos(phase);
}

/* Evaluate J_n(kr) and d/dr J_n(kr), with asymptotic switching.
   NOTE: jn() is the POSIX Bessel function (math.h). If you don't have it,
   replace calls to jn() with gsl_sf_bessel_Jn() or your own implementation. */
static void bessel_jn_and_deriv(
    int n,
    double k,
    double r,
    double *Jn_out,
    double *dJn_out
)
{
    double z = k * r;
    double Jn, dJn;

    if (z > 50.0) {
        /* Asymptotic region */
        double phase = z - 0.5 * n * M_PI - 0.25 * M_PI;
        double amp   = sqrt(2.0 / (M_PI * z));
        Jn  = amp * cos(phase);

        /* d/dr J_n(kr) = d/dz J_n(z) * k
           Use derivative of asymptotic form */
        double dJdz = -amp * sin(phase) - 0.5 * Jn / z;
        dJn = dJdz * k;
    } else {
        /* Exact region – use GSL Bessel Jn */
        Jn = gsl_sf_bessel_Jn(n, z);
        /* d/dz J_n(z) = 0.5 * (J_{n-1}(z) - J_{n+1}(z)) */
        double Jn_minus = gsl_sf_bessel_Jn(n - 1, z);
        double Jn_plus  = gsl_sf_bessel_Jn(n + 1, z);
        double dJdz     = 0.5 * (Jn_minus - Jn_plus);
        dJn = dJdz * k;
    }

    if (Jn_out)  *Jn_out  = Jn;
    if (dJn_out) *dJn_out = dJn;
}


/* ================================================================
   3. Levin collocation solver on [a, b] (Chebyshev basis)
   ================================================================ */

/* Levin collocation for integral:
     ∫_a^b f(r) J_n(k r) r dr
   using Chebyshev-Lobatto nodes and a polynomial approximation
   to u(r) satisfying u'(r) = f(r) r.
*/
static double hankel_levin_interval(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double a,
    double b,
    int degree
)
{
    int N = degree + 1;
    if (N < 3)
        N = 3;

    /* Simple stack-allocated arrays for small N (<= 16 is typical) */
    double x[32];     /* Chebyshev nodes in [-1,1]           */
    double r[32];     /* Mapped nodes in [a,b]               */
    double fr[32];    /* f(r_i) * r_i                        */
    double Jn[32];    /* J_n(k r_i)                          */
    double dJn[32];   /* d/dr J_n(k r_i)                     */
    double u[32];     /* solution u(r_i)                     */
    double D[32][32]; /* differentiation matrix              */
    double A[32][32]; /* system matrix                       */
    double rhs[32];   /* right-hand side                     */

    /* 1. Chebyshev-Lobatto nodes in [-1,1], then map to [a,b] */
    for (int i = 0; i < N; ++i) {
        x[i] = cos(M_PI * i / (N - 1));
        r[i] = 0.5 * (b - a) * (x[i] + 1.0) + a;
    }

    /* 2. Evaluate f(r)*r, J_n(kr), d/dr J_n(kr) */
    for (int i = 0; i < N; ++i) {
        double ri = r[i];
        fr[i] = f(ri, user_data) * ri;
        bessel_jn_and_deriv(n, k, ri, &Jn[i], &dJn[i]);
    }

    /* 3. Build Chebyshev differentiation matrix on [-1,1] */
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                if (i == 0)
                    D[i][j] = (2.0 * (N - 1) * (N - 1) + 1.0) / 6.0;
                else if (i == N - 1)
                    D[i][j] = -(2.0 * (N - 1) * (N - 1) + 1.0) / 6.0;
                else
                    D[i][j] = -x[i] / (2.0 * (1.0 - x[i] * x[i]));
            } else {
                double ci = (i == 0 || i == N - 1) ? 2.0 : 1.0;
                double cj = (j == 0 || j == N - 1) ? 2.0 : 1.0;
                D[i][j] = ci / cj * pow(-1.0, i + j) / (x[i] - x[j]);
            }
        }
    }

    /* Scale from [-1,1] to [a,b]: dr/dx = (b - a)/2 → du/dr = du/dx * dx/dr */
    double scale = 2.0 / (b - a);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            D[i][j] *= scale;

    /* 4. Solve D u = fr for u (Gaussian elimination, N small) */
    for (int i = 0; i < N; ++i) {
        rhs[i] = fr[i];
        for (int j = 0; j < N; ++j)
            A[i][j] = D[i][j];
    }

    for (int i = 0; i < N; ++i) {
        double piv = A[i][i];
        /* very basic pivoting; you can improve if needed */
        if (fabs(piv) < 1e-30)
            piv = (piv >= 0.0 ? 1e-30 : -1e-30);

        for (int j = i; j < N; ++j)
            A[i][j] /= piv;
        rhs[i] /= piv;

        for (int k2 = 0; k2 < N; ++k2) {
            if (k2 == i) continue;
            double factor = A[k2][i];
            for (int j = i; j < N; ++j)
                A[k2][j] -= factor * A[i][j];
            rhs[k2] -= factor * rhs[i];
        }
    }

    for (int i = 0; i < N; ++i)
        u[i] = rhs[i];

    /* 5. Levin formula:
       ∫_a^b f(r) J_n(kr) r dr ≈ u(b)J_n(kb) - u(a)J_n(ka) - ∫_a^b u(r) dJ_n/dr dr
       We approximate the last integral via a simple weighted sum. */
    double term_end = u[N - 1] * Jn[N - 1];
    double term_start = u[0] * Jn[0];

    double I2 = 0.0;
    /* crude composite trapezoid / rectangular rule over nodes */
    for (int i = 0; i < N; ++i)
        I2 += u[i] * dJn[i];

    I2 *= (b - a) / (N - 1);

    return term_end - term_start - I2;
}

/* ================================================================
   4. Asymptotic tail solver on [R1, R]
   ================================================================ */

static double hankel_asymptotic_tail(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double R1,
    double R,
    double tol
)
{
    if (R <= R1)
        return 0.0;

    /* Leading approximation using value at R1 */
    double fR1 = f(R1, user_data);
    double z1  = k * R1;
    double amp1 = sqrt(2.0 / (M_PI * z1));
    double phase1 = z1 - 0.5 * n * M_PI - 0.25 * M_PI;

    double leading = fR1 * R1 * amp1 * sin(phase1) / k;

    /* Rough error estimate */
    double err = fabs(leading) / (k * sqrt(R1));
    if (err < tol)
        return leading;

    /* Two-point refinement using midpoint */
    double mid  = 0.5 * (R1 + R);
    double fmid = f(mid, user_data);
    double zmid = k * mid;
    double ampmid = sqrt(2.0 / (M_PI * zmid));
    double phasemid = zmid - 0.5 * n * M_PI - 0.25 * M_PI;

    double mid_contrib = fmid * mid * ampmid * sin(phasemid) / k;

    double refined = 0.5 * (leading + mid_contrib);
    return refined;
}


/* --------------------------------------------------------------
   Ogata DE on a finite interval [a, b]
   -------------------------------------------------------------- */

static double hankel_ogata_interval(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double a,
    double b,
    double tol
)
{
    /* If the interval is tiny, fall back to a simple midpoint rule */
    if (b - a < 1e-12) {
        double mid = 0.5 * (a + b);
        return f(mid, user_data) * mid * gsl_sf_bessel_Jn(n, k * mid) * (b - a);
    }

    /* Case 1: a = 0 → just use Ogata on [0, b] and subtract tail */
    if (a <= 0.0) {
        /* Ogata on [0, ∞) */
        double H_full = hankel_adaptive_scalar(f, user_data, n, k, tol);

        /* Tail from b to ∞ using Levin or asymptotic */
        double R0_tail = b;
        double H_tail = hankel_asymptotic_tail(f, user_data, n, k, R0_tail, INFINITY, tol);

        return H_full - H_tail;
    }

    /* Case 2: general [a, b] interval:
       Compute Ogata on [0, b] minus Ogata on [0, a] */

    double H0b = hankel_adaptive_scalar(f, user_data, n, k, tol);

    /* Tail from b to ∞ */
    double H_tail_b = hankel_asymptotic_tail(f, user_data, n, k, b, INFINITY, tol);

    double Hb = H0b - H_tail_b;

    /* Now compute [0, a] */
    double H0a = hankel_adaptive_scalar(f, user_data, n, k, tol);

    /* Tail from a to ∞ */
    double H_tail_a = hankel_asymptotic_tail(f, user_data, n, k, a, INFINITY, tol);

    double Ha = H0a - H_tail_a;

    /* Return difference: ∫_a^b */
    return Hb - Ha;
}

/* ================================================================
   5. Hybrid finite-range controller: scalar
   ================================================================ */

double hankel_adaptive_finite(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double R,
    double tol
)
{
    if (k <= 0.0 || R <= 0.0)
        return 0.0;

    if (tol <= 0.0)
        tol = 1e-10;

    /* Region boundaries in r-space */
    double R0 = 50.0 / k;        /* Ogata region ends when kr ≈ 50 */
    if (R0 > R) R0 = R;

    double R1 = R0 + 200.0 / k;  /* Levin region length ~ 200/k */
    if (R1 > R) R1 = R;

    /* Region 1: Ogata on [0, R0] */
    double H1 = 0.0;
    if (R0 > 0.0)
        H1 = hankel_ogata_interval(f, user_data, n, k, 0.0, R0, tol);

    /* Region 2: Levin on [R0, R1] */
    double H2 = 0.0;
    if (R1 > R0)
        H2 = hankel_levin_interval(f, user_data, n, k, R0, R1, 12);

    /* Region 3: Asymptotic tail on [R1, R] */
    double H3 = 0.0;
    if (R > R1)
        H3 = hankel_asymptotic_tail(f, user_data, n, k, R1, R, tol);

    return H1 + H2 + H3;
}

/* ================================================================
   6. Hybrid finite-range controller: vectorized
   ================================================================ */

void hankel_adaptive_finite_vector(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double R,
    double tol,
    double *H_out
)
{
    if (!k_array || !H_out || nk <= 0)
        return;

    if (tol <= 0.0)
        tol = 1e-10;

    for (int i = 0; i < nk; ++i) {
        double k = k_array[i];

        if (k <= 0.0 || R <= 0.0) {
            H_out[i] = 0.0;
            continue;
        }

        double R0 = 50.0 / k;
        if (R0 > R) R0 = R;

        double R1 = R0 + 200.0 / k;
        if (R1 > R) R1 = R;

        double H1 = 0.0;
        if (R0 > 0.0)
            H1 = hankel_ogata_interval(f, user_data, n, k, 0.0, R0, tol);

        double H2 = 0.0;
        if (R1 > R0)
            H2 = hankel_levin_interval(f, user_data, n, k, R0, R1, 12);

        double H3 = 0.0;
        if (R > R1)
            H3 = hankel_asymptotic_tail(f, user_data, n, k, R1, R, tol);

        H_out[i] = H1 + H2 + H3;
    }
}

