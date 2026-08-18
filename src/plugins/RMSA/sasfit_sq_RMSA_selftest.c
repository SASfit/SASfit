/*
 * sasfit_sq_RMSA_selftest.c
 *
 * Validates sasfit_sq_RMSA.c's *logic* -- parameter mapping, the
 * physical-to-dimensionless conversion, caching correctness, input
 * validation, and thread safety of the _Thread_local cache -- against
 * this project's own already-validated rmsa_compute() /
 * rmsa_physical_to_dimensionless() API.
 *
 * Standalone-compile note: this file includes <sasfit_common.h>
 * directly, so it resolves against whichever sasfit_common.h is on
 * the include path -- the real one in this source tree
 * (src/sasfit_common/include/sasfit_common.h) if built with that on
 * the include path, or a minimal test-only shim otherwise (see the
 * companion RMSA C project's sasfit_plugin/test_shims/sasfit_common.h
 * for such a shim, used to test this same file's logic in an
 * environment without the real SASfit source tree available).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sasfit_common.h>
#include "include/rmsa.h"
#include "include/rmsa_physical.h"

extern scalar sasfit_sq_RMSA(scalar q, sasfit_param *param);
extern scalar sasfit_sq_RMSA_f(scalar q, sasfit_param *param);
extern scalar sasfit_sq_RMSA_v(scalar q, sasfit_param *param, int dist);

static int gFailures = 0;

static void checkClose(const char *what, double got, double expected, double relTol)
{
    double reldiff = fabs(got - expected) / fmax(1e-300, fabs(expected));
    printf("  %-58s reldiff=%.3e (tol %.1e)  %s\n", what, reldiff, relTol, (reldiff <= relTol) ? "ok" : "FAIL");
    if (reldiff > relTol) gFailures++;
}

static double referenceSq(double radius, double charge, double volfraction, double temperature,
                           double saltMolarity, double dielectricConst, double q)
{
    RMSAPhysicalParameters phys = {0};
    phys.radius = radius;
    phys.lengthUnitInMeters = 1e-10; /* Angstrom -- must match sasfit_sq_RMSA.c's own fixed convention exactly */
    phys.macroionCharge = charge;
    phys.volumeFraction = volfraction;
    phys.saltMolarity = saltMolarity;
    phys.temperature = temperature;
    phys.dielectricConstant = dielectricConst;
    RMSAPhysicalConversion conv;
    if (rmsa_physical_to_dimensionless(&phys, &conv) != 0) return 1.0;
    double Sq;
    RMSASolution sol;
    if (rmsa_compute(radius, conv.screeningLength, conv.gamma, conv.volumeFraction, &q, &Sq, 1, &sol) < 0) return 1.0;
    return Sq;
}

#define NUM_PLUGIN_THREADS 8
typedef struct { long threadIndex; int failures; } PluginThreadArgs;

static void *pluginThreadWorker(void *argVoidPtr)
{
    PluginThreadArgs *args = (PluginThreadArgs *)argVoidPtr;
    double radius = 40.0 + 2.0 * (double)args->threadIndex;
    sasfit_param param;
    param.p[0] = radius; param.p[1] = 20.0; param.p[2] = 0.15;
    param.p[3] = 300.0; param.p[4] = 0.005; param.p[5] = 79.0;
    double qs[] = {0.005, 0.02, 0.05, 0.1, 0.2, 0.3};
    int failures = 0;
    for (int rep = 0; rep < 100; rep++) {
        for (int i = 0; i < 6; i++) {
            double got = sasfit_sq_RMSA(qs[i], &param);
            double expected = referenceSq(radius, 20.0, 0.15, 300.0, 0.005, 79.0, qs[i]);
            if (fabs(got - expected) / fmax(1e-300, fabs(expected)) > 1e-9) failures++;
        }
    }
    args->failures = failures;
    return NULL;
}

int main(void)
{
    printf("Parameter mapping and physical conversion, checked against rmsa_compute()\n"
           "and rmsa_physical_to_dimensionless() directly (the same reference this\n"
           "whole project's own test suite uses):\n");
    {
        struct { double radius, charge, volfraction, temperature, salt, dielectric; } cases[] = {
            {50.0, 15.0, 0.1, 298.15, 0.01, 78.5, },
            {30.0, 40.0, 0.2, 310.0, 0.001, 80.0, },
            {80.0, 5.0, 0.05, 293.0, 0.0, 78.0, },
        };
        double qs[] = {0.001, 0.01, 0.05, 0.1, 0.2};
        for (int c = 0; c < 3; c++) {
            for (int i = 0; i < 5; i++) {
                sasfit_param param;
                param.p[0] = cases[c].radius;
                param.p[1] = cases[c].charge;
                param.p[2] = cases[c].volfraction;
                param.p[3] = cases[c].temperature;
                param.p[4] = cases[c].salt;
                param.p[5] = cases[c].dielectric;
                double got = sasfit_sq_RMSA(qs[i], &param);
                double expected = referenceSq(cases[c].radius, cases[c].charge, cases[c].volfraction,
                                               cases[c].temperature, cases[c].salt, cases[c].dielectric, qs[i]);
                char buf[48];
                snprintf(buf, sizeof buf, "case %d q=%.3g", c, qs[i]);
                checkClose(buf, got, expected, 1e-12);
            }
        }
    }

    printf("\nCaching correctness: repeated calls with unchanged parameters (SASfit's\n"
           "own calling pattern -- once per Q, same parameters across the array) give\n"
           "results consistent with a fresh, uncached computation:\n");
    {
        sasfit_param param;
        param.p[0] = 50.0; param.p[1] = 15.0; param.p[2] = 0.1;
        param.p[3] = 298.15; param.p[4] = 0.01; param.p[5] = 78.5;
        double qs[] = {0.001, 0.01, 0.02, 0.05, 0.1, 0.15, 0.2, 0.3};
        int mismatches = 0;
        for (int i = 0; i < 8; i++) {
            double got = sasfit_sq_RMSA(qs[i], &param);
            double expected = referenceSq(50.0, 15.0, 0.1, 298.15, 0.01, 78.5, qs[i]);
            if (fabs(got - expected) / fmax(1e-300, fabs(expected)) > 1e-9) mismatches++;
        }
        printf("  %s (%d/8 mismatches across a simulated Q-array call sequence)\n",
               mismatches == 0 ? "PASS" : "FAIL", mismatches);
        gFailures += mismatches;
    }

    printf("\n_f/_v stubs (real SASfit precedent for structure factors -- see\n"
           "sasfit_sq_RMSA.c's own comment -- both must be exactly 0.0):\n");
    {
        sasfit_param param;
        param.p[0] = 50.0; param.p[1] = 15.0; param.p[2] = 0.1;
        param.p[3] = 298.15; param.p[4] = 0.01; param.p[5] = 78.5;
        double f = sasfit_sq_RMSA_f(0.1, &param);
        double v = sasfit_sq_RMSA_v(0.1, &param, 1);
        int ok = (f == 0.0) && (v == 0.0);
        printf("  %s (_f=%.6g, _v=%.6g)\n", ok ? "PASS" : "FAIL", f, v);
        if (!ok) gFailures++;
    }

    printf("\nInput validation control flow: SASFIT_CHECK_COND1 correctly fires and\n"
           "aborts before any RMSA computation runs, returning exactly 1.0 --\n"
           "confirmed by reading src/sasfit_common/include/sasfit_error_common.h and\n"
           "sasfit_error_sq.h directly from this same source tree:\n"
           "SASFIT_CHECK_COND1/SASFIT_ASSERT_PTR expand to\n"
           "`return SASFIT_RETURNVAL_ON_ERROR;`, and that's #defined to exactly 1.0\n"
           "for structure factors:\n");
    {
        struct { double radius, charge, volfraction, temperature, salt, dielectric; const char *label; } badCases[] = {
            {-1.0, 15.0, 0.1, 298.15, 0.01, 78.5, "negative radius"},
            {50.0, 15.0, -0.1, 298.15, 0.01, 78.5, "negative volume fraction"},
            {50.0, 15.0, 1.5, 298.15, 0.01, 78.5, "volume fraction >= 1"},
            {50.0, 15.0, 0.1, -298.15, 0.01, 78.5, "negative temperature"},
            {50.0, 15.0, 0.1, 298.15, -0.01, 78.5, "negative salt"},
            {50.0, 15.0, 0.1, 298.15, 0.01, -78.5, "negative dielectric constant"},
        };
        int failures = 0;
        for (int i = 0; i < 6; i++) {
            sasfit_param param;
            param.p[0] = badCases[i].radius; param.p[1] = badCases[i].charge; param.p[2] = badCases[i].volfraction;
            param.p[3] = badCases[i].temperature; param.p[4] = badCases[i].salt; param.p[5] = badCases[i].dielectric;
            double got = sasfit_sq_RMSA(0.05, &param);
            if (got != 1.0) { printf("  FAIL: %s: expected 1.0 (SASFIT_RETURNVAL_ON_ERROR), got %.6g\n", badCases[i].label, got); failures++; }
        }
        printf("  %s (%d/6 bad-input cases did not return exactly 1.0)\n", failures == 0 ? "PASS" : "FAIL", failures);
        gFailures += failures;
    }

    printf("\nInput validation *this file itself controls*: the physical-conversion\n"
           "failure path and the rmsa_compute_cached() negative-status path both\n"
           "explicitly `return 1.0;` in sasfit_sq_RMSA.c -- this IS this file's own\n"
           "choice (SASfit's own convention for \"no structural correlation\", matching\n"
           "the real sasfit_sq_clusteryukawa.c precedent), so it's fair to assert on\n"
           "directly. Triggered here via a parameter combination that hits RMSA's\n"
           "own documented \"no physical root\" edge case (near-zero charge + high\n"
           "volume fraction -- see the main RMSA project's README.md \"Known\n"
           "Limitations\"):\n");
    {
        RMSAPhysicalParameters physCheck = {0};
        physCheck.radius = 30.0; physCheck.lengthUnitInMeters = 1e-10;
        physCheck.macroionCharge = 1e-6; physCheck.volumeFraction = 0.25;
        physCheck.saltMolarity = 0.001; physCheck.temperature = 298.15; physCheck.dielectricConstant = 78.5;
        RMSAPhysicalConversion convCheck;
        rmsa_physical_to_dimensionless(&physCheck, &convCheck);
        double directStatus;
        {
            double q = 0.05, Sq; RMSASolution sol;
            directStatus = rmsa_compute(30.0, convCheck.screeningLength, convCheck.gamma, convCheck.volumeFraction, &q, &Sq, 1, &sol);
        }
        printf("  (confirmed directly: rmsa_compute() itself returns status=%.0f here)\n", directStatus);

        sasfit_param param;
        param.p[0] = 30.0; param.p[1] = 1e-6; param.p[2] = 0.25;
        param.p[3] = 298.15; param.p[4] = 0.001; param.p[5] = 78.5;
        double got = sasfit_sq_RMSA(0.05, &param);
        printf("  %s (got %.6g, expected exactly 1.0)\n", got == 1.0 ? "PASS" : "FAIL", got);
        if (got != 1.0) gFailures++;
    }

    printf("\nThread safety of the _Thread_local cache (8 threads, each running its own\n"
           "simulated SASfit fitting loop with distinct parameters, checked against a\n"
           "single-threaded reference -- same pattern as the RMSA project's own\n"
           "test_cache.c / test_warm_start.c):\n");
    {
        pthread_t threads[NUM_PLUGIN_THREADS];
        PluginThreadArgs args[NUM_PLUGIN_THREADS];
        for (int t = 0; t < NUM_PLUGIN_THREADS; t++) {
            args[t].threadIndex = t;
            args[t].failures = 0;
            pthread_create(&threads[t], NULL, pluginThreadWorker, &args[t]);
        }
        for (int t = 0; t < NUM_PLUGIN_THREADS; t++) pthread_join(threads[t], NULL);
        int total = 0;
        for (int t = 0; t < NUM_PLUGIN_THREADS; t++) total += args[t].failures;
        printf("  %s (%d mismatches across %d threads x 100 repetitions x 6 Q-values)\n",
               total == 0 ? "PASS" : "FAIL", total, NUM_PLUGIN_THREADS);
        gFailures += total;
    }

    printf("\n%s (%d check%s failed)\n", gFailures == 0 ? "PASS" : "FAIL", gFailures, gFailures == 1 ? "" : "s");
    return gFailures == 0 ? 0 : 1;
}
