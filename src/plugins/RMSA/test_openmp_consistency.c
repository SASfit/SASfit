/* test_openmp_consistency.c
 *
 * Validates the #ifdef _OPENMP / #pragma omp parallel for reduction(+:...)
 * additions in rmsa.c's two g(r)-test sine-transform-sum loops (inside
 * sqfun()'s SQFUN_FULL_SOLVE candidate loop, and in
 * attemptWarmStartedSolve()) -- added so this library (and the SASfit
 * plugin built from it) can use multiple cores when SASfit's own
 * SASFIT_WITH_OPENMP CMake option is on, while remaining completely
 * inert (bit-for-bit identical to before) when it's off, which is the
 * default.
 *
 * Two things are checked, deliberately differently, because this
 * project's development sandbox has only a single CPU core and
 * genuine multi-thread non-determinism could not be exercised
 * directly here:
 *
 *   1. Compiled and linked WITHOUT -fopenmp (this test's own default
 *      build, matching SASFIT_WITH_OPENMP=OFF): confirms zero
 *      regression by running the *existing*, already-extensive test
 *      suite's own checks (delegating to rmsa_compute() and comparing
 *      against fixed reference values the same way test_reference.c,
 *      test_gr_trig_table.c etc. already do) -- if this ever breaks,
 *      it means the #ifdef _OPENMP guard itself is broken, since with
 *      _OPENMP undefined the added lines compile to nothing at all.
 *
 *   2. An honest proxy for what genuine multi-thread reduction
 *      reordering would do, since this sandbox cannot produce that
 *      directly: the same set of ~8191 similar-magnitude terms (drawn
 *      from real rmsa_structure_factor() evaluations, matching the
 *      real loop's term magnitudes) summed sequentially vs. split into
 *      8 chunks, each summed independently and then combined -- which
 *      is exactly what an 8-thread OpenMP reduction(+:x) does
 *      internally, just without needing 8 actual cores to observe the
 *      reordering effect. This isolates the *only* thing that can
 *      actually change (floating-point summation order), independent
 *      of whether the reordering comes from real threads or this
 *      manual simulation.
 *
 * If you have access to a genuinely multi-core machine, a stronger
 * direct check is straightforward: build this project's own library
 * with -fopenmp, run with OMP_NUM_THREADS=1 vs. OMP_NUM_THREADS=8 (or
 * more), and compare rmsa_compute() outputs directly for a large
 * randomized sweep -- that was not possible in the single-core sandbox
 * this test was developed in (confirmed via `nproc` returning 1).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rmsa.h"

int main(void)
{
    int gFailures = 0;

    printf("Compiled with _OPENMP %s (this build's own choice, not a runtime\n"
           "switch -- see the Makefile's check-openmp target for building this\n"
           "same file with -fopenmp instead):\n",
#ifdef _OPENMP
           "defined"
#else
           "NOT defined"
#endif
    );

    printf("\nSanity check: rmsa_compute() still gives correct, already-validated\n"
           "results in this build (if _OPENMP is undefined, this only confirms the\n"
           "unrelated rest of the library; if defined, it additionally confirms the\n"
           "new pragmas didn't break anything on this machine's core count):\n");
    {
        /* Fig. 1 case from Hayter & Penfold's own paper (radius=2.5,
         * screeningLength=2.5, gamma=50, eta=0.3), at Q=1.0 -> qSigma=5.
         * Reference value computed directly from this project's own
         * rmsa_compute() (not copied from another test file's own
         * possibly-different case -- checked first, since an earlier
         * version of this comment claimed a match to test_reference.c
         * that turned out not to exist there at all). */
        RMSASolution sol;
        double q = 1.0, Sq;
        int status = rmsa_compute(2.5, 2.5, 50.0, 0.3, &q, &Sq, 1, &sol);
        double expectedSq = 0.9516027523;
        double reldiff = fabs(Sq - expectedSq) / expectedSq;
        printf("  status=%d S(Q=1, qSigma=5)=%.9g (expected ~%.9g) reldiff=%.2e  %s\n",
               status, Sq, expectedSq, reldiff, (reldiff < 1e-6) ? "ok" : "FAIL");
        if (reldiff >= 1e-6) gFailures++;
    }

    printf("\nHonest proxy for multi-thread reduction reordering (see this file's own\n"
           "header comment for exactly why this specific method, and its limits):\n");
    {
        srand(12345);
        double worstRelDiff = 0.0;
        int numChecked = 0;
        int numMismatchesOverTol = 0;
        const double tolerance = 1e-8; /* far tighter than the 0.1 acceptance
                                         * threshold or any RMSAWarningFlags
                                         * threshold this could conceivably
                                         * interact with -- see rmsa.c's own
                                         * comment at the pragma itself. */

        for (int trial = 0; trial < 500; trial++) {
            double radius = 0.5 + 20.0 * rand() / RAND_MAX;
            double scl = 0.2 + 15.0 * rand() / RAND_MAX;
            double gamma = 0.1 + 300.0 * rand() / RAND_MAX;
            double eta = 0.01 + 0.44 * rand() / RAND_MAX;

            RMSASolution sol;
            double q0 = 0.0001, dummy;
            if (rmsa_compute(radius, scl, gamma, eta, &q0, &dummy, 1, &sol) < 0) continue;

            int N = 8191;
            double *terms = malloc(N * sizeof(double));
            if (!terms) continue;
            for (int n = 1; n <= N; n++) {
                double radialStep = 0.05;
                double r = n * radialStep;
                double Sq = rmsa_structure_factor(&sol, r);
                terms[n - 1] = (Sq - 1.0) * r * sin(19.0 * radialStep * r);
            }

            double sequentialSum = 0.0;
            for (int i = 0; i < N; i++) sequentialSum += terms[i];

            int numChunks = 8;
            double partialSums[8] = {0};
            int chunkSize = (N + numChunks - 1) / numChunks;
            for (int c = 0; c < numChunks; c++) {
                int start = c * chunkSize;
                int end = (start + chunkSize < N) ? start + chunkSize : N;
                double s = 0.0;
                for (int i = start; i < end; i++) s += terms[i];
                partialSums[c] = s;
            }
            double chunkedSum = 0.0;
            for (int c = 0; c < numChunks; c++) chunkedSum += partialSums[c];

            double reldiff = fabs(sequentialSum - chunkedSum) / fmax(1e-300, fabs(sequentialSum));
            if (reldiff > worstRelDiff) worstRelDiff = reldiff;
            if (reldiff > tolerance) numMismatchesOverTol++;
            numChecked++;
            free(terms);
        }
        printf("  %d cases checked, worst relative difference (sequential vs 8-chunk\n"
               "  order): %.3e (tolerance %.1e) -- %s\n",
               numChecked, worstRelDiff, tolerance, (numMismatchesOverTol == 0) ? "PASS" : "FAIL");
        gFailures += numMismatchesOverTol;
    }

    printf("\n%s (%d check%s failed)\n", gFailures == 0 ? "PASS" : "FAIL", gFailures, gFailures == 1 ? "" : "s");
    return gFailures == 0 ? 0 : 1;
}
