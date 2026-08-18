/*
 * sasfit_sq_RFA_selftest.c
 *
 * Validates all four model files' LOGIC (parameter mapping, caching,
 * warm-starting, model/step-count isolation, error handling, the
 * physicality guard, thread safety) against this project's own
 * already-validated direct API (rfa1_solve()/rfa2_solve()/
 * rfa_structure_factor()), compiled against the test-only shims in
 * ../test_shims/ (a sibling of this RFA/ directory, matching the
 * confirmed real SASfit layout -- see that directory's own
 * sasfit_common.h header comment for exactly what's confirmed vs.
 * inferred, and from which real SASfit plugin sources) since the real
 * SASfit headers and build system are not available here -- see this
 * delivery's README.md for exactly what that does and does not confirm.
 *
 * Build (run from inside RFA/):
 *   gcc -std=c11 -O2 -Wall -Wextra -I. -Iinclude -I../test_shims \
 *       sasfit_sq_RFA_selftest.c sasfit_sq_RFA1.c sasfit_sq_RFA2.c \
 *       sasfit_sq_RFA1_3step.c sasfit_sq_RFA2_3step.c rfa_piecewise.c \
 *       -o sasfit_sq_RFA_selftest -lgsl -lgslcblas -lm -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sasfit_common.h>
#include "rfa_piecewise.h"

scalar sasfit_sq_RFA1(scalar q, sasfit_param *param);
scalar sasfit_sq_RFA2(scalar q, sasfit_param *param);
void sasfit_sq_RFA1_release_thread_cache(void);
void sasfit_sq_RFA2_release_thread_cache(void);
scalar sasfit_sq_RFA1_3step(scalar q, sasfit_param *param);
scalar sasfit_sq_RFA2_3step(scalar q, sasfit_param *param);
void sasfit_sq_RFA1_3step_release_thread_cache(void);
void sasfit_sq_RFA2_3step_release_thread_cache(void);
void sasfit_sq_RFA1_3step_release_thread_cache(void);
void sasfit_sq_RFA2_3step_release_thread_cache(void);

static int gFailures = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); gFailures++; } \
} while (0)

static sasfit_param make_param(double radius, double lambda, double eps, double eta)
{
    sasfit_param p = {{0}};
    p.p[0] = radius; p.p[1] = lambda; p.p[2] = eps; p.p[3] = eta;
    return p;
}

static sasfit_param make_param3(double radius, double l1, double l2, double l3,
                                 double e1, double e2, double e3, double eta)
{
    sasfit_param p = {{0}};
    p.p[0] = radius; p.p[1] = l1; p.p[2] = l2; p.p[3] = l3;
    p.p[4] = e1; p.p[5] = e2; p.p[6] = e3; p.p[7] = eta;
    return p;
}

/* ------------------------------------------------------------------
 * 1. Parameter mapping and radius/q scaling: does the plugin's S(q)
 *    match a direct rfa2_solve()+rfa_structure_factor() call?
 * ------------------------------------------------------------------ */
static void test_parameter_mapping(void)
{
    printf("=== 1. Parameter mapping (RFA2) vs. direct API ===\n");
    double radius = 50.0, lambda = 1.3, eps = -1.5, eta = 0.25;
    sasfit_param param = make_param(radius, lambda, eps, eta);

    double lambda_steps[1] = {lambda};
    double beps_steps[1]   = {eps};
    PWPotential pot;
    rfa_potential_init(&pot, 1, eta, lambda_steps, beps_steps);
    RFASolution sol;
    int conv = rfa2_solve(&pot, &sol);
    CHECK(conv, "direct rfa2_solve() should converge for this ordinary state point");

    double sigma = 2.0*radius;
    int nmismatch = 0;
    for (double Q = 0.001; Q < 0.05; Q += 0.001) {
        double direct  = rfa_structure_factor(&sol, Q*sigma);
        double plugin  = sasfit_sq_RFA2(Q, &param);
        if (fabs(direct - plugin) > 1e-9*fmax(1.0, fabs(direct))) nmismatch++;
    }
    CHECK(nmismatch == 0, "plugin S(Q) should match direct API across a Q-array (%d/50 mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");

    rfa_solution_free(&sol);
    rfa_potential_free(&pot);
}

/* ------------------------------------------------------------------
 * 2. Exact cache hit: repeated calls at the SAME parameters across a
 *    simulated Q-array should give identical results to solving fresh
 *    every time (this is the primary performance claim being tested,
 *    not just correctness -- see main-README "Performance").
 * ------------------------------------------------------------------ */
static void test_exact_cache(void)
{
    printf("\n=== 2. Exact-parameter cache hit across a simulated Q-array (RFA1) ===\n");
    sasfit_param param = make_param(40.0, 1.4, -1.0, 0.30);

    double lambda_steps[1] = {1.4};
    double beps_steps[1]   = {-1.0};
    PWPotential pot;
    rfa_potential_init(&pot, 1, 0.30, lambda_steps, beps_steps);
    RFASolution ref;
    rfa1_solve(&pot, &ref);

    int nmismatch = 0;
    double sigma = 80.0;
    for (double Q = 0.002; Q < 0.08; Q += 0.0015) {
        double want = rfa_structure_factor(&ref, Q*sigma);
        double got  = sasfit_sq_RFA1(Q, &param);
        if (fabs(want - got) > 1e-9*fmax(1.0, fabs(want))) nmismatch++;
    }
    CHECK(nmismatch == 0, "cached-hit S(Q) should match a fresh reference solve (%d mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");

    rfa_solution_free(&ref);
    rfa_potential_free(&pot);
}

/* ------------------------------------------------------------------
 * 3. Warm-starting across slightly-perturbed parameters (the fitting
 *    Jacobian pattern): a cache MISS every call, but each solve should
 *    still land on the correct (matching an independent fresh solve)
 *    answer, not a stale/wrong one from a bad warm start.
 * ------------------------------------------------------------------ */
static void test_perturbed_sequence(void)
{
    printf("\n=== 3. Perturbed-parameter sequence (RFA2, simulating a fitting Jacobian) ===\n");
    double etas[6] = {0.250, 0.2501, 0.2499, 0.2502, 0.2498, 0.30};
    int nmismatch = 0;
    for (int i = 0; i < 6; i++) {
        sasfit_param param = make_param(45.0, 1.35, -1.2, etas[i]);
        double plugin = sasfit_sq_RFA2(0.01, &param);

        double lambda_steps[1] = {1.35};
        double beps_steps[1]   = {-1.2};
        PWPotential pot;
        rfa_potential_init(&pot, 1, etas[i], lambda_steps, beps_steps);
        RFASolution ref;
        int conv = rfa2_solve(&pot, &ref);
        double direct = conv ? rfa_structure_factor(&ref, 0.01*90.0) : NAN;
        if (conv) {
            if (fabs(direct - plugin) > 1e-6*fmax(1.0, fabs(direct))) {
                nmismatch++;
                printf("    eta=%.4f: plugin=%.8f direct=%.8f MISMATCH\n", etas[i], plugin, direct);
            }
        }
        rfa_solution_free(&ref);  /* unconditional: rfa2_solve() always allocates, regardless of convergence --
                                    * a real leak here (confirmed via LeakSanitizer on the sibling n=3 test
                                    * and thread_worker(), which DO hit non-convergent points in practice)
                                    * before this fix guarded it behind `if (conv)`. */
        rfa_potential_free(&pot);
    }
    CHECK(nmismatch == 0, "every perturbed call should match an independent fresh solve (%d mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");
}

/* ------------------------------------------------------------------
 * 4. RFA1 and RFA2 caches must not cross-contaminate: calling one
 *    must not affect the other's cached state or results.
 * ------------------------------------------------------------------ */
static void test_model_isolation(void)
{
    printf("\n=== 4. RFA1/RFA2 cache isolation ===\n");
    sasfit_param param = make_param(50.0, 1.25, -1.0, 0.20);   /* verified: both models converge here */
    double s1_before = sasfit_sq_RFA1(0.01, &param);
    double s2 = sasfit_sq_RFA2(0.01, &param);
    double s1_after = sasfit_sq_RFA1(0.01, &param);
    CHECK(s1_before == s1_after, "calling sasfit_sq_RFA2 must not perturb sasfit_sq_RFA1's cached result");
    CHECK(s1_before != 1.0 && s2 != 1.0, "both models should actually converge for this state point (got RFA1=%.6g, RFA2=%.6g -- if either is exactly 1.0, the test's premise no longer holds and needs a new parameter choice)", s1_before, s2);
    CHECK(fabs(s1_before - s2) > 1e-6, "RFA1 and RFA2 should generally give DIFFERENT S(q) (sanity: they're different models)");
    printf("  RFA1=%.6f  RFA2=%.6f  %s\n", s1_before, s2, (s1_before==s1_after) ? "PASS" : "see above");
}

/* ------------------------------------------------------------------
 * 5. Error handling: invalid parameters must not crash and must not
 *    silently return a plausible-looking value computed from garbage.
 * ------------------------------------------------------------------ */
static void test_invalid_parameters(void)
{
    printf("\n=== 5. Invalid-parameter handling (RFA1) ===\n");
    struct { double radius, lambda, eps, eta; const char *label; } bad[] = {
        {-1.0, 1.3, -1.0, 0.25, "negative RADIUS"},
        {50.0, 0.9, -1.0, 0.25, "LAMBDA <= 1"},
        {50.0, 1.3, -1.0, -0.1, "negative VOLFRACTION"},
        {50.0, 1.3, -1.0, 1.5,  "VOLFRACTION >= 1"},
    };
    int failures = 0;
    for (int i = 0; i < 4; i++) {
        sasfit_param param = make_param(bad[i].radius, bad[i].lambda, bad[i].eps, bad[i].eta);
        double got = sasfit_sq_RFA1(0.01, &param);
        /* SASFIT_CHECK_COND1 does NOT return early in the real,
         * confirmed test shim (see ../test_shims/sasfit_common.h's
         * header comment) -- an earlier version of this plugin
         * incorrectly modelled it as doing so, and this test's
         * expectation (0.0) dates from that incorrect model. The
         * ACTUAL early-exit for invalid RADIUS happens inside
         * rfa_common_sq_n() directly; for invalid LAMBDA/VOLFRACTION
         * it happens via rfa_potential_init()'s own validation -- both
         * paths return 1.0 (SASfit's "no structural correlation"
         * convention), consistently with the non-convergence and
         * unphysical-result paths tested elsewhere in this file. */
        if (got != 1.0) { printf("  FAIL: %s: expected 1.0 (no structural correlation), got %.6g\n", bad[i].label, got); failures++; }
    }
    printf("  %s (%d/4 bad-input cases did not return 1.0 as expected)\n",
           failures == 0 ? "PASS" : "FAIL", failures);
    gFailures += failures;
}

/* ------------------------------------------------------------------
 * 6. Non-convergence: a real "no solution" region (see main project's
 *    scan_sw_ss.c) must return 1.0 (SASfit's "no correlation"
 *    convention), not crash and not silently invent a value.
 * ------------------------------------------------------------------ */
static void test_nonconvergence(void)
{
    printf("\n=== 6. Non-convergence handling (RFA1, a known 'no solution' region) ===\n");
    /* From the main project's scan_sw_ss.c data: SW, wide+deep well,
     * high density -- RFA1 has no solution here. */
    sasfit_param param = make_param(50.0, 1.9, -5.0, 0.45);
    double got = sasfit_sq_RFA1(0.01, &param);
    CHECK(got == 1.0, "non-convergent state should return 1.0 (SASfit's 'no correlation' convention), got %.6g", got);
    printf("  %s\n", got == 1.0 ? "PASS" : "see above");
}

/* ------------------------------------------------------------------
 * 7. Physicality guard: a converged-but-unphysical state (see main
 *    project's plot_scan.py / crit_breakdown figure) must ALSO return
 *    1.0, not the (numerically "successful" but unphysical) S(q).
 * ------------------------------------------------------------------ */
static void test_unphysical_guard(void)
{
    printf("\n=== 7. Unphysical-result guard (RFA1, a known converged-but-unphysical case) ===\n");
    /* From the main project's critical_states.c "breakdown" scenario:
     * SS lambda=1.75, T*=5.0 (beps=+0.2), eta=0.40 -- RFA1 converges
     * but g(r)<0 somewhere in the first shell. Verify directly first,
     * so this test doesn't silently stop meaning anything if the
     * underlying model's behaviour ever changes. */
    double lambda_steps[1] = {1.75};
    double beps_steps[1]   = {0.2};
    PWPotential pot;
    rfa_potential_init(&pot, 1, 0.40, lambda_steps, beps_steps);
    RFASolution ref;
    int conv = rfa1_solve(&pot, &ref);
    double mingr = 1e300;
    for (double r = 1.0; r < 2.0; r += 0.002) {
        double g = rfa_rdf_first_shell(&ref, r);
        if (g < mingr) mingr = g;
    }
    printf("  direct check: converged=%d, min g(r) in first shell = %.4f\n", conv, mingr);
    CHECK(conv && mingr < 0.0, "this test's premise (converged but unphysical) no longer holds -- pick a new case");

    sasfit_param param = make_param(50.0, 1.75, 0.2, 0.40);
    double got = sasfit_sq_RFA1(0.01, &param);
    CHECK(got == 1.0, "unphysical-but-converged state should return 1.0, got %.6g (i.e. the guard did not fire)", got);
    printf("  %s\n", got == 1.0 ? "PASS" : "see above");

    rfa_solution_free(&ref);  /* unconditional: rfa1_solve() always allocates, regardless of convergence */
    rfa_potential_free(&pot);
}

/* ------------------------------------------------------------------
 * 9-15. The n=3 plugins: the same checks as 1-7 above, generalized.
 * Parameter values below were verified directly against rfa1_solve()/
 * rfa2_solve() (see /tmp/find_n3_cases.c in the session log) before
 * being hardcoded here, exactly as tests 4 and 6/7's n=1 parameters
 * were -- picking an untested "should converge"/"should fail" state
 * point is exactly the mistake test 4 originally made for n=1 (see
 * this file's own git-free history: it first used parameters where
 * RFA1 silently didn't converge, making the test pass for the wrong
 * reason, until checked directly and fixed).
 * ------------------------------------------------------------------ */

/* known-good baseline, also used throughout the main project's
 * demo_3step.c/bench_warmstart.c/etc. -- both RFA1_3step and
 * RFA2_3step converge here. */
#define N3_GOOD_L1 1.20
#define N3_GOOD_L2 1.40
#define N3_GOOD_L3 1.60
#define N3_GOOD_E1 1.00
#define N3_GOOD_E2 -1.50
#define N3_GOOD_E3 0.50
#define N3_GOOD_ETA 0.30

/* verified non-convergent for RFA1_3step (and RFA2_3step): a wide,
 * fairly deep middle well at moderate density. */
#define N3_BAD_L1 1.3
#define N3_BAD_L2 1.6
#define N3_BAD_L3 1.9
#define N3_BAD_E1 0.5
#define N3_BAD_E2 -2.0
#define N3_BAD_E3 0.3
#define N3_BAD_ETA 0.30

/* verified converged-but-UNPHYSICAL for RFA1_3step: same shape, a
 * shallower well but higher density. */
#define N3_UNPHYS_E2  -1.0
#define N3_UNPHYS_ETA  0.45

static void test_3step_parameter_mapping(void)
{
    printf("\n=== 9. Parameter mapping, n=3 (RFA2_3step) vs. direct API ===\n");
    double radius = 50.0;
    sasfit_param param = make_param3(radius, N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3,
                                      N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3, N3_GOOD_ETA);

    double lambda_steps[3] = {N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3};
    double beps_steps[3]   = {N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3};
    PWPotential pot;
    rfa_potential_init(&pot, 3, N3_GOOD_ETA, lambda_steps, beps_steps);
    RFASolution sol;
    int conv = rfa2_solve(&pot, &sol);
    CHECK(conv, "direct rfa2_solve() should converge for this verified state point");

    double sigma = 2.0*radius;
    int nmismatch = 0;
    for (double Q = 0.001; Q < 0.05; Q += 0.001) {
        double direct = rfa_structure_factor(&sol, Q*sigma);
        double plugin = sasfit_sq_RFA2_3step(Q, &param);
        if (fabs(direct - plugin) > 1e-9*fmax(1.0, fabs(direct))) nmismatch++;
    }
    CHECK(nmismatch == 0, "plugin S(Q) should match direct API across a Q-array (%d/50 mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");

    rfa_solution_free(&sol);
    rfa_potential_free(&pot);
}

static void test_3step_exact_cache(void)
{
    printf("\n=== 10. Exact-parameter cache hit, n=3 (RFA1_3step) ===\n");
    double radius = 40.0;
    sasfit_param param = make_param3(radius, N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3,
                                      N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3, N3_GOOD_ETA);

    double lambda_steps[3] = {N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3};
    double beps_steps[3]   = {N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3};
    PWPotential pot;
    rfa_potential_init(&pot, 3, N3_GOOD_ETA, lambda_steps, beps_steps);
    RFASolution ref;
    rfa1_solve(&pot, &ref);

    int nmismatch = 0;
    double sigma = 2.0*radius;
    for (double Q = 0.002; Q < 0.08; Q += 0.0015) {
        double want = rfa_structure_factor(&ref, Q*sigma);
        double got  = sasfit_sq_RFA1_3step(Q, &param);
        if (fabs(want - got) > 1e-9*fmax(1.0, fabs(want))) nmismatch++;
    }
    CHECK(nmismatch == 0, "cached-hit S(Q) should match a fresh reference solve (%d mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");

    rfa_solution_free(&ref);
    rfa_potential_free(&pot);
}

static void test_3step_perturbed_sequence(void)
{
    printf("\n=== 11. Perturbed-parameter sequence, n=3 (RFA2_3step) ===\n");
    double etas[6] = {N3_GOOD_ETA, N3_GOOD_ETA+0.0001, N3_GOOD_ETA-0.0001,
                       N3_GOOD_ETA+0.0002, N3_GOOD_ETA-0.0002, 0.25};
    int nmismatch = 0;
    for (int i = 0; i < 6; i++) {
        sasfit_param param = make_param3(45.0, N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3,
                                          N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3, etas[i]);
        double plugin = sasfit_sq_RFA2_3step(0.01, &param);

        double lambda_steps[3] = {N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3};
        double beps_steps[3]   = {N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3};
        PWPotential pot;
        rfa_potential_init(&pot, 3, etas[i], lambda_steps, beps_steps);
        RFASolution ref;
        int conv = rfa2_solve(&pot, &ref);
        if (conv) {
            double direct = rfa_structure_factor(&ref, 0.01*90.0);
            if (fabs(direct - plugin) > 1e-6*fmax(1.0, fabs(direct))) {
                nmismatch++;
                printf("    eta=%.4f: plugin=%.8f direct=%.8f MISMATCH\n", etas[i], plugin, direct);
            }
        }
        rfa_solution_free(&ref);  /* unconditional -- see the matching fix/comment in test_perturbed_sequence() */
        rfa_potential_free(&pot);
    }
    CHECK(nmismatch == 0, "every perturbed call should match an independent fresh solve (%d mismatched)", nmismatch);
    printf("  %s\n", nmismatch == 0 ? "PASS" : "see above");
}

static void test_3step_model_isolation(void)
{
    printf("\n=== 12. Cache isolation: RFA1_3step / RFA2_3step / n=1 siblings ===\n");
    sasfit_param param3 = make_param3(50.0, N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3,
                                       N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3, N3_GOOD_ETA);
    sasfit_param param1 = make_param(50.0, 1.25, -1.0, 0.20);   /* verified n=1 state point, from test 4 */

    double r1_before = sasfit_sq_RFA1_3step(0.01, &param3);
    double r2         = sasfit_sq_RFA2_3step(0.01, &param3);
    double n1a        = sasfit_sq_RFA1(0.01, &param1);          /* n=1 sibling, must be unaffected */
    double r1_after   = sasfit_sq_RFA1_3step(0.01, &param3);
    double n1b        = sasfit_sq_RFA1(0.01, &param1);

    CHECK(r1_before == r1_after, "calling RFA2_3step/RFA1(n=1) must not perturb RFA1_3step's cached result");
    CHECK(n1a == n1b, "calling the n=3 plugins must not perturb the n=1 sibling's cached result");
    CHECK(r1_before != 1.0 && r2 != 1.0 && n1a != 1.0, "all three should actually converge here (got %.6g, %.6g, %.6g)", r1_before, r2, n1a);
    CHECK(fabs(r1_before - r2) > 1e-6, "RFA1_3step and RFA2_3step should generally give DIFFERENT S(q)");
    printf("  RFA1_3step=%.6f  RFA2_3step=%.6f  RFA1(n=1)=%.6f  %s\n",
           r1_before, r2, n1a, (r1_before==r1_after && n1a==n1b) ? "PASS" : "see above");
}

static void test_3step_invalid_parameters(void)
{
    printf("\n=== 13. Invalid-parameter handling, n=3 (RFA1_3step) ===\n");
    struct { double l1,l2,l3,e1,e2,e3,eta; const char *label; } bad[] = {
        {0.9, 1.4, 1.6,  1.0,-1.5,0.5, 0.30, "LAMBDA1 <= 1"},
        {1.4, 1.3, 1.6,  1.0,-1.5,0.5, 0.30, "LAMBDA2 <= LAMBDA1 (out of order)"},
        {1.2, 1.4, 1.35, 1.0,-1.5,0.5, 0.30, "LAMBDA3 <= LAMBDA2 (out of order)"},
        {1.2, 1.4, 1.6,  1.0,-1.5,0.5, -0.1, "negative VOLFRACTION"},
        {1.2, 1.4, 1.6,  1.0,-1.5,0.5, 1.5,  "VOLFRACTION >= 1"},
    };
    int failures = 0;
    for (int i = 0; i < 5; i++) {
        sasfit_param param = make_param3(50.0, bad[i].l1, bad[i].l2, bad[i].l3,
                                          bad[i].e1, bad[i].e2, bad[i].e3, bad[i].eta);
        double got = sasfit_sq_RFA1_3step(0.01, &param);
        /* As test_invalid_parameters() above: expect 1.0 (SASfit's "no
         * structural correlation" convention, via rfa_potential_init()'s
         * own validation), not an early-exit sentinel from
         * SASFIT_CHECK_COND1, which doesn't return early in the real
         * confirmed test shim. */
        if (got != 1.0) { printf("  FAIL: %s: expected 1.0 (no structural correlation), got %.6g\n", bad[i].label, got); failures++; }
    }
    printf("  %s (%d/5 bad-input cases did not return 1.0 as expected)\n",
           failures == 0 ? "PASS" : "FAIL", failures);
    gFailures += failures;
}

static void test_3step_nonconvergence(void)
{
    printf("\n=== 14. Non-convergence handling, n=3 (RFA1_3step, verified 'no solution' point) ===\n");
    sasfit_param param = make_param3(50.0, N3_BAD_L1, N3_BAD_L2, N3_BAD_L3,
                                      N3_BAD_E1, N3_BAD_E2, N3_BAD_E3, N3_BAD_ETA);
    double got = sasfit_sq_RFA1_3step(0.01, &param);
    CHECK(got == 1.0, "non-convergent state should return 1.0, got %.6g", got);
    printf("  %s\n", got == 1.0 ? "PASS" : "see above");
}

static void test_3step_unphysical_guard(void)
{
    printf("\n=== 15. Unphysical-result guard, n=3 (RFA1_3step, verified converged-but-unphysical point) ===\n");
    double lambda_steps[3] = {N3_BAD_L1, N3_BAD_L2, N3_BAD_L3};
    double beps_steps[3]   = {N3_BAD_E1, N3_UNPHYS_E2, N3_BAD_E3};
    PWPotential pot;
    rfa_potential_init(&pot, 3, N3_UNPHYS_ETA, lambda_steps, beps_steps);
    RFASolution ref;
    int conv = rfa1_solve(&pot, &ref);
    double mingr = 1e300;
    for (double r = 1.0; r < 2.0; r += 0.002) {
        double g = rfa_rdf_first_shell(&ref, r);
        if (g < mingr) mingr = g;
    }
    printf("  direct check: converged=%d, min g(r) in first shell = %.4f\n", conv, mingr);
    CHECK(conv && mingr < 0.0, "this test's premise (converged but unphysical) no longer holds -- pick a new case");

    sasfit_param param = make_param3(50.0, N3_BAD_L1, N3_BAD_L2, N3_BAD_L3,
                                      N3_BAD_E1, N3_UNPHYS_E2, N3_BAD_E3, N3_UNPHYS_ETA);
    double got = sasfit_sq_RFA1_3step(0.01, &param);
    CHECK(got == 1.0, "unphysical-but-converged state should return 1.0, got %.6g (guard did not fire)", got);
    printf("  %s\n", got == 1.0 ? "PASS" : "see above");

    rfa_solution_free(&ref);  /* unconditional: rfa1_solve() always allocates, regardless of convergence */
    rfa_potential_free(&pot);
}

/* ------------------------------------------------------------------
 * 8/16. Thread safety: both n=1 (test 8, unchanged) and n=3 plugins.
 * ------------------------------------------------------------------ */
typedef struct { int id; int ok; } ThreadArg;

static void *thread_worker(void *arg)
{
    ThreadArg *ta = (ThreadArg*)arg;
    int ok = 1;
    for (int i = 0; i < 200; i++) {
        double eta = 0.20 + 0.001*(ta->id) + 0.0001*i;
        sasfit_param param = make_param(50.0, 1.3, -1.2, eta);
        double got1 = sasfit_sq_RFA1(0.01, &param);
        double got2 = sasfit_sq_RFA2(0.01, &param);

        double lambda_steps[1] = {1.3};
        double beps_steps[1]   = {-1.2};
        PWPotential pot;
        rfa_potential_init(&pot, 1, eta, lambda_steps, beps_steps);
        RFASolution r1, r2;
        int c1 = rfa1_solve(&pot, &r1);
        int c2 = rfa2_solve(&pot, &r2);
        if (c1) {
            double want = rfa_structure_factor(&r1, 0.01*100.0);
            if (fabs(want - got1) > 1e-6*fmax(1.0,fabs(want))) ok = 0;
        }
        if (c2) {
            double want = rfa_structure_factor(&r2, 0.01*100.0);
            if (fabs(want - got2) > 1e-6*fmax(1.0,fabs(want))) ok = 0;
        }
        /* unconditional: rfa1_solve()/rfa2_solve() always allocate,
         * regardless of convergence -- this eta range genuinely
         * crosses a non-convergence boundary for some (thread,
         * iteration) combinations (confirmed directly), so freeing
         * only inside `if (c1)`/`if (c2)` leaked real memory here,
         * caught by LeakSanitizer, not just a theoretical risk.       */
        rfa_solution_free(&r1);
        rfa_solution_free(&r2);
        rfa_potential_free(&pot);

        /* same pattern, n=3, using the verified known-good state point
         * perturbed slightly per thread/iteration -- exercises the
         * n=3 plugins' caches concurrently alongside the n=1 ones.    */
        double eta3 = N3_GOOD_ETA + 0.001*(ta->id) + 0.0001*i;
        sasfit_param param3 = make_param3(50.0, N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3,
                                           N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3, eta3);
        double got3a = sasfit_sq_RFA1_3step(0.01, &param3);
        double got3b = sasfit_sq_RFA2_3step(0.01, &param3);

        double lambda_steps3[3] = {N3_GOOD_L1, N3_GOOD_L2, N3_GOOD_L3};
        double beps_steps3[3]   = {N3_GOOD_E1, N3_GOOD_E2, N3_GOOD_E3};
        PWPotential pot3;
        rfa_potential_init(&pot3, 3, eta3, lambda_steps3, beps_steps3);
        RFASolution r3a, r3b;
        int c3a = rfa1_solve(&pot3, &r3a);
        int c3b = rfa2_solve(&pot3, &r3b);
        if (c3a) {
            double want = rfa_structure_factor(&r3a, 0.01*100.0);
            if (fabs(want - got3a) > 1e-6*fmax(1.0,fabs(want))) ok = 0;
        }
        if (c3b) {
            double want = rfa_structure_factor(&r3b, 0.01*100.0);
            if (fabs(want - got3b) > 1e-6*fmax(1.0,fabs(want))) ok = 0;
        }
        /* unconditional -- see the matching fix/comment above. This is
         * the specific pair that LeakSanitizer's backtrace pointed at
         * (128 leaked objects, tracing into rfa2_solve_warm): this
         * eta3 range crosses a real non-convergence boundary at
         * higher thread ids/iteration counts (e.g. id=3..4, i~150,
         * eta3~0.318-0.319), confirmed directly, not merely suspected. */
        rfa_solution_free(&r3a);
        rfa_solution_free(&r3b);
        rfa_potential_free(&pot3);
    }
    ta->ok = ok;
    /* Demonstrates the intended cleanup pattern for a thread that is
     * about to exit -- see rfa_common_release_cache()'s header
     * comment in sasfit_sq_RFA_common.h. Without this, LeakSanitizer
     * correctly reports one cached solution's worth of memory held per
     * thread that has called each plugin (confirmed directly for the
     * n=1 pair before this call existed: exactly 8 solutions leaked,
     * one per thread). All four plugins used above get released. */
    sasfit_sq_RFA1_release_thread_cache();
    sasfit_sq_RFA2_release_thread_cache();
    sasfit_sq_RFA1_3step_release_thread_cache();
    sasfit_sq_RFA2_3step_release_thread_cache();
    return NULL;
}

static void test_thread_safety(void)
{
    printf("\n=== 8/16. Thread safety: 8 threads x 200 calls each, all four plugins ===\n");
    const int N = 8;
    pthread_t th[8];
    ThreadArg args[8];
    for (int i = 0; i < N; i++) { args[i].id = i; args[i].ok = -1; pthread_create(&th[i], NULL, thread_worker, &args[i]); }
    int all_ok = 1;
    for (int i = 0; i < N; i++) { pthread_join(th[i], NULL); all_ok &= args[i].ok; }
    CHECK(all_ok, "all threads should match their own serial reference computation");
    printf("  %s\n", all_ok ? "PASS" : "see above");
}

int main(void)
{
    test_parameter_mapping();
    test_exact_cache();
    test_perturbed_sequence();
    test_model_isolation();
    test_invalid_parameters();
    test_nonconvergence();
    test_unphysical_guard();
    test_3step_parameter_mapping();
    test_3step_exact_cache();
    test_3step_perturbed_sequence();
    test_3step_model_isolation();
    test_3step_invalid_parameters();
    test_3step_nonconvergence();
    test_3step_unphysical_guard();
    test_thread_safety();

    /* release the main thread's own caches too, for a fully leak-free
     * self-test -- same reasoning as thread_worker()'s cleanup above. */
    sasfit_sq_RFA1_release_thread_cache();
    sasfit_sq_RFA2_release_thread_cache();
    sasfit_sq_RFA1_3step_release_thread_cache();
    sasfit_sq_RFA2_3step_release_thread_cache();

    printf("\n%s (%d failure%s)\n", gFailures == 0 ? "ALL CHECKS PASSED" : "SOME CHECKS FAILED",
           gFailures, gFailures == 1 ? "" : "s");
    return gFailures == 0 ? 0 : 1;
}
