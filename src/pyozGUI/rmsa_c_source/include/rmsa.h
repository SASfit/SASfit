/* ------------------------------------------------------------------
 * rmsa.h -- Rescaled Mean Spherical Approximation (RMSA) structure
 * factor for charged colloids / macroion solutions.
 *
 * This is a standalone copy of SASfitGit's own
 * src/plugins/RMSA/{rmsa.c,polyroots.c,rmsa_physical.c} (simplified:
 * the warm-start/caching optimization in rmsa_compute_cached() was
 * dropped in favour of a plain re-solve on every call -- that
 * optimization only ever affects performance, never correctness, and
 * the original always falls back to a full solve whenever there is
 * any doubt anyway). No SASfit-specific dependencies -- builds as a
 * standalone shared library, wrapped from Python via ctypes
 * (../rmsaWrapper.py) rather than reimplemented, to reuse this
 * project's own already-validated quartic-root-selection/rescaling
 * algorithm directly.
 *
 * C port of the algorithm in:
 *   J. B. Hayter and J. Penfold, Mol. Phys. 42, 109 (1981)      [MSA]
 *   J.-P. Hansen and J. B. Hayter, Mol. Phys. 46, 651 (1982)    [rescaling]
 * ------------------------------------------------------------------ */
#ifndef RMSA_H
#define RMSA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int    rescaleIterations;
    double volumeFraction;
    double contactPotential;
    double screeningParam;
    double couplingConstant;
    double closureConstantCoeff;
    double closureLinearCoeff;
    double closureSinhCoeff;
    double closureCoshCoeff;
    double laplaceHAtScreening;
    double laplaceHAtZero;
    double rescaledVolumeFraction;
    double rescaledContactPotential;
    double rescaledScreeningParam;
    double rescalingFactor;
    double contactValue;
    double physicalRootResidual;
    int numCandidateRoots;
    unsigned warnings;
} RMSASolution;

enum RMSAWarningFlags {
    RMSA_WARN_NONE                     = 0,
    RMSA_WARN_NEAR_DEGENERATE_ROOT      = 1 << 0,
    RMSA_WARN_MARGINAL_PHYSICAL_ROOT    = 1 << 1,
    RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC = 1 << 2
};

int rmsa_solve(double volumeFraction, double contactPotential, double screeningParam,
               RMSASolution *solution);
const char *rmsa_error_string(int errorCode);
int rmsa_is_small_k_risky(double K);
double rmsa_structure_factor(const RMSASolution *solution, double K);
double rmsa_structure_factor_derivative(const RMSASolution *solution, double K);
int rmsa_compute(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                  const double *q, double *structureFactorOut, int numPoints, RMSASolution *solution);

typedef struct {
    int hasSolved;
    double lastRadius;
    double lastScreeningLength;
    double lastChargeParameter;
    double lastVolumeFraction;
    RMSASolution solution;
} RMSACache;

int rmsa_compute_cached(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                         const double *q, double *structureFactorOut, int numPoints,
                         RMSACache *cache, RMSASolution *solution);

#ifdef __cplusplus
}
#endif
#endif /* RMSA_H */
