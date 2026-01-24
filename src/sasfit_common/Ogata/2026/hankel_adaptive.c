#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "hankel_adaptive.h"
#include "hankel.h"         /* Ogata API: hankel_ogata_nu, hankel_ogata_table_* */
#include "levin_hankel.h"   /* Levin tail: hankel_tail_levin_int_order */

/* Fast method switch: for small k use finite Gauss-Legendre integration (faster
    and accurate for slowly oscillatory integrands); for larger k use Ogata+Levin. */
#define HANKEL_FAST_SWITCH_K 1.0
#define HANKEL_FINITE_R 6.0
#define HANKEL_FINITE_NGL 256

/* ---------- Ogata single-evaluation, with external table ---------- */

static double hankel_ogata_int_order_with_table(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol,
    hankel_ogata_table_t *tab,
    double h,
    int N_start,
    int N_step,
    int Nmax
)
{
    (void)tol; /* tol is handled by the N-increment logic inside hankel_ogata_nu, if any */

    return hankel_ogata_nu(
        f,
        user_data,
        (double)n,
        k,
        h,
        N_start,
        N_step,
        Nmax,
        tol,
        tab
    );
}

/* ---------- internal: adaptive h using fresh tables (scalar use) ---------- */

static double hankel_ogata_int_order_adaptive_h(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol,
    double *h_final_out,
    double *core_out
)
{
    /* try finer h values for higher accuracy */
    const double h_list[] = {0.10, 0.075, 0.05, 0.025, 0.0125, 0.00625, 0.003125, 0.0015625, 0.00078125, 0.000390625, 0.0001953125, 0.0001, 0.00005, 0.000025, 0.00001, 0.000001,  0.0000001, 0.00000001, 0.000000001};
    const int nh = (int)(sizeof(h_list)/sizeof(h_list[0]));

    /* avoid choosing h so small that x/k -> r becomes huge and f(r) underflows
       Estimate safe minimum h so that r = x/k ~ pi/(h*k) <= r_max. Solve for h:
       h >= pi/(k * r_max). Use r_max ~ 25 to avoid Gaussian underflow. Also
       enforce a global lower bound of 0.00001. */
    const double r_max = 25.0;
    double h_min = 0.00001;
    if (k > 0.0) {
        double h_from_r = M_PI / (k * r_max);
        if (h_from_r > h_min) h_min = h_from_r;
    }

    int N_start = 80; /* raise starting N for Ogata */
    int N_step  = 40;
    int Nmax    = 5000; /* allow many more nodes for harder cases */

    double prev = 0.0;
    int have_prev = 0;

    for (int i = 0; i < nh; ++i) {
        double h = h_list[i];
        if (h < h_min) {
            /* skip overly fine h which would map to enormous r values */
            continue;
        }

        hankel_ogata_table_t *tab = hankel_ogata_table_create(h, Nmax);
        double val = hankel_ogata_int_order_with_table(
            f, user_data, n, k, tol, tab, h, N_start, N_step, Nmax
        );
        hankel_ogata_table_destroy(tab);

        if (have_prev) {
            double diff = fabs(val - prev);
            if (diff < 0.5*tol) {
                if (h_final_out) *h_final_out = h;
                if (core_out)    *core_out    = val;
                return val;
            }
        }

        prev = val;
        have_prev = 1;
    }

    if (!have_prev) {
        /* All candidates were too fine; evaluate once at h_min */
        if (h_final_out) *h_final_out = h_min;
        hankel_ogata_table_t *tab = hankel_ogata_table_create(h_min, Nmax);
        double val = hankel_ogata_int_order_with_table(f, user_data, n, k, tol, tab, h_min, N_start, N_step, Nmax);
        hankel_ogata_table_destroy(tab);
        if (core_out) *core_out = val;
        return val;
    }

    if (h_final_out) *h_final_out = h_list[nh-1];
    if (core_out)    *core_out    = prev;
    return prev;
}

/* ---------- Levin tail helpers ---------- */

static int should_use_levin_tail(double k, double R0)
{
    if (k <= 0.0) return 0;
    return (k*R0 > 20.0) ? 1 : 0;
}

static double hankel_tail_adaptive(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol,
    double R0_guess,
    double *R0_out
)
{
    if (k <= 0.0) {
        if (R0_out) *R0_out = 0.0;
        return 0.0;
    }

    double R0;
    if (R0_guess > 0.0) {
        R0 = fmax(R0_guess, 15.0);
    } else {
        R0 = fmax(15.0, 30.0/k);
    }
    if (R0_out) *R0_out = R0;

    double R1 = R0 + 200.0;
    int    N_levin = 1024; /* more collocation points for better tail accuracy */

    double tail_prev = hankel_tail_levin_int_order(
        f, user_data, n, k, R0, R1, N_levin
    );

    for (int iter = 0; iter < 20; ++iter) {
        R1 += 200.0;
        double tail_new = hankel_tail_levin_int_order(
            f, user_data, n, k, R0, R1, N_levin
        );

        double diff = fabs(tail_new - tail_prev);
        if (diff < 0.05*tol) {
            return tail_new;
        }
        tail_prev = tail_new;
    }

    return tail_prev;
}

/* ---------- public: scalar adaptive Hankel (no table reuse) ---------- */

double hankel_adaptive_scalar(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol
)
{
    if (tol <= 0.0) tol = 1e-10;
    if (k <= 0.0)   return 0.0; /* or handle k=0 separately */

    /* Fast switch: use adaptive finite GL for small k to keep runtime low */
    if (n == 1) {
        if (k <= HANKEL_FAST_SWITCH_K) {
            return hankel_finite_GL_adaptive(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL, tol);
        }

        /* For larger k use Filon composite on [0,Rk] plus Levin tail */
        double Rk = fmax(HANKEL_FINITE_R, 20.0 + 10.0 * k);
        double H_core = hankel_finite_filon(f, user_data, n, k, Rk, 0, tol);

        double R0_tail = fmax(Rk, fmax(15.0, 30.0/k));
        int use_tail = should_use_levin_tail(k, R0_tail);
        if (!use_tail) return H_core;

        double H_tail = hankel_tail_adaptive(f, user_data, n, k, tol, R0_tail, &R0_tail);
        return H_core + H_tail;
    }

    /* Fast switch: use adaptive finite GL for small k to keep runtime low */
    if (k <= HANKEL_FAST_SWITCH_K) {
        return hankel_finite_GL_adaptive(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL, tol);
    }

    double h_used = 0.0;
    double core_val = 0.0;

    double H_core = hankel_ogata_int_order_adaptive_h(
        f, user_data, n, k, tol, &h_used, &core_val
    );

    /* Apply cheap Richardson extrapolation around h_used to improve accuracy */
    H_core = hankel_ogata_nu_extrapolate(f, user_data, (double)n, k, h_used, 80, 40, 5000, tol, NULL);

    /* Instrumentation: report core/tail breakdown for debugging accuracy */
    printf("[hankel_adaptive] k=%g h_used=%g core_val=%g H_core=%g\n", k, h_used, core_val, H_core);
    {
        FILE *f = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log", "a");
        if (f) { fprintf(f, "CORE k=%g h_used=%g core_val=%g H_core=%g\n", k, h_used, core_val, H_core); fclose(f); }
    }

    double R0_tail = fmax(15.0, 30.0/k);
    int use_tail = should_use_levin_tail(k, R0_tail);
    printf("[hankel_adaptive] k=%g R0_tail=%g use_tail=%d\n", k, R0_tail, use_tail);
    {
        FILE *f = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log", "a");
        if (f) { fprintf(f, "TAIL_CFG k=%g R0_tail=%g use_tail=%d\n", k, R0_tail, use_tail); fclose(f); }
    }

    if (!use_tail) {
        return H_core;
    }

    double H_tail = hankel_tail_adaptive(
        f, user_data, n, k, tol, R0_tail, &R0_tail
    );
    printf("[hankel_adaptive] k=%g H_tail=%g\n", k, H_tail);
    {
        FILE *f = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log", "a");
        if (f) { fprintf(f, "TAIL_VAL k=%g H_tail=%g\n", k, H_tail); fclose(f); }
    }

    return H_core + H_tail;
}

/* Debug variant: returns core and tail contributions and chosen h */
double hankel_adaptive_debug(
    hankel_func_t f,
    void *user_data,
    int n,
    double k,
    double tol,
    double *core_out,
    double *tail_out,
    double *h_used_out
)
{
    if (tol <= 0.0) tol = 1e-10;
    if (k <= 0.0) {
        if (core_out) *core_out = 0.0;
        if (tail_out) *tail_out = 0.0;
        if (h_used_out) *h_used_out = 1e-6;
        return 0.0;
    }

    if (n == 1) {
        if (k <= HANKEL_FAST_SWITCH_K) {
            double val = hankel_finite_GL_adaptive(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL, tol);
            if (core_out) *core_out = val;
            if (tail_out) *tail_out = 0.0;
            if (h_used_out) *h_used_out = 1e-6;
            return val;
        }

        double Rk = fmax(HANKEL_FINITE_R, 10.0 + 5.0 * k);
        double H_core = hankel_finite_filon(f, user_data, n, k, Rk, 0, tol);
        double R0_tail = fmax(Rk, fmax(15.0, 30.0/k));
        int use_tail = should_use_levin_tail(k, R0_tail);
        double H_tail = 0.0;
        if (use_tail) {
            H_tail = hankel_tail_adaptive(f, user_data, n, k, tol, R0_tail, &R0_tail);
        }
        if (core_out) *core_out = H_core;
        if (tail_out) *tail_out = H_tail;
        if (h_used_out) *h_used_out = 1e-6;
        return H_core + H_tail;
    }

    if (k <= HANKEL_FAST_SWITCH_K) {
        double val = hankel_finite_GL(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL);
        if (core_out) *core_out = val;
        if (tail_out) *tail_out = 0.0;
        if (h_used_out) *h_used_out = 1e-6;
        return val;
    }

    double h_used = 1e-6;
    double core_val = 0.0;

    double H_core = hankel_ogata_int_order_adaptive_h(
        f, user_data, n, k, tol, &h_used, &core_val
    );
    H_core = hankel_ogata_nu_extrapolate(f, user_data, (double)n, k, h_used, 80, 40, 5000, tol, NULL);

    double R0_tail = fmax(15.0, 30.0/k);
    int use_tail = should_use_levin_tail(k, R0_tail);

    double H_tail = 0.0;
    if (use_tail) {
        H_tail = hankel_tail_adaptive(f, user_data, n, k, tol, R0_tail, &R0_tail);
    }

    if (core_out) *core_out = H_core;
    if (tail_out) *tail_out = H_tail;
    if (h_used_out) *h_used_out = h_used;

    /* Write debug log to absolute path for later inspection */
    {
        FILE *f = fopen("C:/Users/kohlbrecher/switchdrive/SASfitGit/src/sasfit_common/Ogata/2026/hankel_instrument.log", "a");
        if (f) {
            fprintf(f, "DBG_CORE k=%g h_used=%g H_core=%g\n", k, h_used, H_core);
            fprintf(f, "DBG_TAIL k=%g R0=%g H_tail=%g use_tail=%d\n", k, R0_tail, H_tail, use_tail);
            fclose(f);
        }
    }

    return H_core + H_tail;
}

/* ---------- public: vector adaptive Hankel (no reuse, simple) ---------- */

void hankel_adaptive_vector(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double tol,
    double *H_out
)
{
    if (!k_array || !H_out || nk <= 0) return;

    for (int i = 0; i < nk; ++i) {
        double k = k_array[i];
        if (k <= 0.0) { H_out[i] = 0.0; continue; }
        if (k <= HANKEL_FAST_SWITCH_K) {
            if (n == 1) {
                double Rk = fmax(HANKEL_FINITE_R, 10.0 + 5.0 * k);
                H_out[i] = hankel_finite_filon(f, user_data, n, k, Rk, 0, tol);
            } else {
                H_out[i] = hankel_finite_GL_adaptive(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL, tol);
            }
        } else {
            H_out[i] = hankel_adaptive_scalar(f, user_data, n, k, tol);
        }
    }
}

/* ---------- internal: choose h once for a set of k's ---------- */

static double choose_h_for_k_set(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double tol
)
{
    double k_max = 0.0;
    int have_max = 0;

    for (int i = 0; i < nk; ++i) {
        double k = k_array[i];
        if (k <= 0.0) continue;
        if (!have_max || k > k_max) {
            k_max = k;
            have_max = 1;
        }
    }

    if (!have_max) {
        return 0.15; /* fallback */
    }

    double h_used = 0.0;
    double core_dummy = 0.0;

    hankel_ogata_int_order_adaptive_h(
        f, user_data, n, k_max, tol, &h_used, &core_dummy
    );

    /* Optionally clamp to [0.05, 0.20] */
    if (h_used < 0.05) h_used = 0.05;
    if (h_used > 0.20) h_used = 0.20;

    return h_used;
}


/* ---------- public: vector adaptive Hankel with table reuse ---------- */

void hankel_adaptive_vector_reuse(
    hankel_func_t f,
    void *user_data,
    int n,
    const double *k_array,
    int nk,
    double tol,
    double *H_out
)
{
    if (!k_array || !H_out || nk <= 0) return;
    if (tol <= 0.0) tol = 1e-10;

    /* Choose one h for the whole k set */
    double h = choose_h_for_k_set(f, user_data, n, k_array, nk, tol);

    /* Ogata parameters */
    int N_start = 20;
    int N_step  = 20;
    int Nmax    = 200;

    /* Build Ogata table once for this h */
    hankel_ogata_table_t *tab = hankel_ogata_table_create(h, Nmax);

    for (int i = 0; i < nk; ++i) {
        double k = k_array[i];

        if (k <= 0.0) {
            H_out[i] = 0.0; /* or special-case k=0 */
            continue;
        }

        if (k <= HANKEL_FAST_SWITCH_K) {
            if (n == 1) {
                double Rk = fmax(HANKEL_FINITE_R, 10.0 + 5.0 * k);
                H_out[i] = hankel_finite_filon(f, user_data, n, k, Rk, 0, tol);
            } else {
                H_out[i] = hankel_finite_GL(f, user_data, n, k, HANKEL_FINITE_R, HANKEL_FINITE_NGL);
            }
            continue;
        }

        /* Core with shared table */
        double H_core = hankel_ogata_int_order_with_table(
            f, user_data, n, k, tol, tab, h, N_start, N_step, Nmax
        );

        /* Improve with extrapolation (cheap two-point) */
        H_core = hankel_ogata_nu_extrapolate(f, user_data, (double)n, k, h, N_start, N_step, Nmax, tol, NULL);

        /* Decide if tail is needed */
        double R0_tail = fmax(10.0, 20.0/k);
        int use_tail = should_use_levin_tail(k, R0_tail);

        if (!use_tail) {
            H_out[i] = H_core;
            continue;
        }

        /* Tail via Levin */
        double H_tail = hankel_tail_adaptive(
            f, user_data, n, k, tol, R0_tail, &R0_tail
        );

        H_out[i] = H_core + H_tail;
    }

    hankel_ogata_table_destroy(tab);
}
