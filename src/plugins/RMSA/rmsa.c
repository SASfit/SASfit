/* ------------------------------------------------------------------
 * rmsa.c -- see rmsa.h. Ported from jscatter's
 * jscatter/structurefactor/fluid.py (_sqfun, _sqcoef, _SQMSA, RMSA),
 * which is itself derived from the original Hayter-Penfold Fortran
 * routine SQHPA (J.B.Hayter, I.L.L., 19-Aug-81), following the paper
 * equations in the Appendix of Hayter & Penfold, Mol. Phys. 42, 109
 * (1981).
 *
 * Identifiers here are named for what they represent rather than
 * mirroring the paper's/reference's single- or double-letter symbols
 * (eta, gek, ak, a, b, c, f, ..., al1, b1, n1, T1, M1, L1, W12, ...).
 * See README.md for a table mapping these names back to the paper's
 * notation and to jscatter's Python variable names, for anyone
 * cross-checking against either source.
 * ------------------------------------------------------------------ */
#include "include/rmsa.h"
#include "include/polyroots.h"
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* The three ways _sqfun (now sqfun) can be invoked, matching the
 * reference's ix=1,2,4 (ix=3 belongs only to the legacy Fortran-replica
 * path and is not needed by this port's root-selection algorithm). */
typedef enum {
    SQFUN_FULL_SOLVE       = 1, /* find all quartic roots, pick the physical one, fill *solution */
    SQFUN_NEWTON_RESIDUAL  = 2, /* evaluate the secant-search target function for the eta-rescaling loop */
    SQFUN_FINAL_RESCALED   = 4  /* evaluate final coefficients (and new g(1+)) at the converged rescaled eta */
} SqfunMode;

const char *rmsa_error_string(int errorCode)
{
    switch (errorCode) {
        case -1: return "NEWTON ITERATION NON-CONVERGENT while searching for the rescaled volume fraction";
        case -2: return "NEWTON ITERATION NON-CONVERGENT while solving for a single coefficient set";
        case -3: return "CANNOT RESCALE TO G(1+) > 0.";
        case -4: return "no physical root with G(r<1) close to 0 found";
        default: return "ok";
    }
}

int rmsa_is_small_k_risky(double K)
{
    /* Must match the actual crossover point in structureFactorFromCoefficients()
     * below (currently 0.02) -- this function is purely informational
     * (see its doc comment in rmsa.h) and would be actively misleading
     * if it disagreed with the real internal threshold.                */
    return fabs(K) < 0.02;
}

/* Hayter & Penfold equation (14), a.k.a. jscatter's _SQMSA: the
 * structure factor S(K) given the (rescaled) MSA closure coefficients. */
/* Taylor series expansion of the S(Q) formula above around K=0, used
 * for small reducedWavevector to avoid the catastrophic cancellation
 * the general formula suffers there (terms like 24/K^3 and 24/K^4
 * that must cancel to a finite result as K->0 -- see README.md's
 * "Known Limitations"). Four even terms (K^0, K^2, K^4, K^6); S(K) is an
 * even, analytic function of K near K=0, so odd powers don't appear.
 *
 * Derived and validated independently of any reference implementation:
 * symbolically expanded from the exact formula in generalStructureFactor()
 * below via SymPy, then checked against an arbitrary-precision (60-200
 * decimal digit, depending on how small K is) evaluation of that same
 * exact formula via mpmath. A fresh 150-case random sweep of physically
 * reasonable parameters (radius, screening length, contact potential,
 * volume fraction), checked at 8 points each from K=0.0002 to K=0.03
 * against that arbitrary-precision reference: worst relative error
 * found anywhere was 1.2e-4, with 0% of cases exceeding even 1% (let
 * alone the 5% fallback threshold below). See test_taylor_series.c.
 *
 * An earlier version of this function was transcribed from SasView's
 * hayter_msa.c (sqhcal(), a 2-term K^0+K^2 series, RKH Feb 2016 comment:
 * "Taylor series expansion at small qk ... with help from Mathematica").
 * Head-to-head comparison against the same arbitrary-precision reference
 * found that version to be catastrophically wrong (relative error of
 * order 1 -- not merely imprecise) for the most extreme coefficient set
 * in this project's validation suite (very small screeningParam, hence
 * very large closureCoshCoeff): the K^2 series' own internal
 * cancellation was fine, but the K^0 term's residual is only O(ak^4)
 * relative to intermediate terms of O(1), and that residual is combined
 * with (typically enormous) closureCoshCoeff. This function's added K^4
 * term is not what fixes that -- it's mostly still there for accuracy at
 * moderate K -- the K^0 and K^2 terms have simply been re-derived with a
 * different, apparently better-conditioned algebraic grouping. In the
 * previous 500-case sweep against SasView's formula, ~3% of cases
 * exceeded the 5% fallback threshold somewhere in K=[0.0002,0.03] (worst
 * ~1300% relative error before the fallback caught it); this function
 * found 0% of cases doing so in a fresh 150-case sweep. This function is
 * far more accurate in known extreme cases too (relative error ~1e-7 to
 * 1e-4 across K=0.0002-0.03 for the single worst case from that earlier
 * 500-case sweep, vs. SasView's ~100%-400%) but not perfectly immune to
 * this failure mode in principle -- see isPlausibleSmallKResult() below
 * for the safety net that remains for whatever extreme case turns up
 * next.
 *
 * Crossover threshold: originally 0.01, widened to 0.02 after a
 * dedicated check (not just the general 150-case sweep above): 199
 * cases sampled with extra weight on very small screeningParam (the
 * regime that produced the SasView-transcription failure above), each
 * compared directly against the arbitrary-precision exact formula
 * (not against S(0), which conflates genuine physical curvature with
 * actual series error and had previously made a perfectly-good
 * K=0.03 series result for the hardest known case look suspect purely
 * because the true S(K) itself already differs from S(0) by >5% there
 * -- see README.md). Worst relative error found: 4.4e-8 at K=0.01,
 * 2.8e-6 at K=0.02, 3.2e-5 at K=0.03 -- all comfortably better than
 * the accuracy this model is ever likely to be used at, so 0.02 was
 * chosen with real margin to spare rather than as a tight bound.      */
static double smallKStructureFactorTaylorSeries(double reducedWavevector, double volumeFraction,
                                                 double screeningParam, double contactPotential,
                                                 double closureConstantCoeff, double closureLinearCoeff,
                                                 double closureSinhCoeff, double closureCoshCoeff,
                                                 double sinhScreening, double coshScreening)
{
    double screeningSq = screeningParam * screeningParam;
    double screeningQuad = screeningSq * screeningSq;
    double screeningSixth = screeningQuad * screeningSq;
    double screeningEighth = screeningSixth * screeningSq;
    double kSq = reducedWavevector * reducedWavevector;
    double kQuad = kSq * kSq;
    double kSixth = kQuad * kSq;

    double coeffK0 =
          (closureConstantCoeff * screeningSq * (volumeFraction + 4.0) + 3.0 * closureLinearCoeff * screeningSq
           + 12.0 * closureSinhCoeff * (screeningParam * coshScreening - sinhScreening)
           + 6.0 * closureCoshCoeff * (2.0 - screeningSq - 2.0 * coshScreening + 2.0 * screeningParam * sinhScreening)
           - 12.0 * contactPotential * (screeningParam + 1.0))
        / (12.0 * screeningSq);

    double coeffK2 =
          (-closureConstantCoeff * screeningQuad * (15.0 * volumeFraction + 48.0) - 40.0 * closureLinearCoeff * screeningQuad
           - 240.0 * closureSinhCoeff * (screeningSq * screeningParam * coshScreening - 3.0 * screeningSq * sinhScreening
                                          + 6.0 * screeningParam * coshScreening - 6.0 * sinhScreening)
           + 60.0 * closureCoshCoeff * (screeningQuad - 4.0 * screeningSq * screeningParam * sinhScreening
                                        + 12.0 * screeningSq * coshScreening - 24.0 * screeningParam * sinhScreening
                                        + 24.0 * coshScreening - 24.0)
           + 240.0 * contactPotential * (screeningSq * screeningParam + 3.0 * screeningSq + 6.0 * screeningParam + 6.0))
        / (1440.0 * screeningQuad);

    double coeffK4 =
          (closureConstantCoeff * screeningSixth * (42.0 * volumeFraction + 120.0) + 105.0 * closureLinearCoeff * screeningSixth
           + 840.0 * closureSinhCoeff * (screeningQuad * screeningParam * coshScreening - 5.0 * screeningQuad * sinhScreening
                                          + 20.0 * screeningSq * screeningParam * coshScreening - 60.0 * screeningSq * sinhScreening
                                          + 120.0 * screeningParam * coshScreening - 120.0 * sinhScreening)
           - 140.0 * closureCoshCoeff * (screeningSixth - 6.0 * screeningQuad * screeningParam * sinhScreening
                                         + 30.0 * screeningQuad * coshScreening - 120.0 * screeningSq * screeningParam * sinhScreening
                                         + 360.0 * screeningSq * coshScreening - 720.0 * screeningParam * sinhScreening
                                         + 720.0 * coshScreening - 720.0)
           - 840.0 * contactPotential * (screeningQuad * screeningParam + 5.0 * screeningQuad + 20.0 * screeningSq * screeningParam
                                         + 60.0 * screeningSq + 120.0 * screeningParam + 120.0))
        / (100800.0 * screeningSixth);

    /* K^6 term: pushed a level further than the original 3-term (K^0,
     * K^2, K^4) series for extra safety margin, especially for the
     * very-small-screeningParam cases that are the current limiting
     * factor -- see README.md's "Known Limitations" for the empirical
     * comparison (roughly 5-30x better accuracy than the 3-term series
     * in the K=0.02-0.05 range for the hardest known case, though
     * diminishing and eventually *reversing* well beyond that, as is
     * typical for a truncated asymptotic series pushed past its useful
     * range -- this term does not by itself justify widening the
     * crossover threshold without also widening isPlausibleSmallKResult's
     * validation). Derived and verified the same way as the other three
     * (SymPy series expansion + mpmath cross-check + a from-scratch
     * hand-factoring verified symbolically equal to the raw SymPy
     * output before use here, to catch transcription slips like the one
     * that happened -- and was caught -- while writing this comment).  */
    double coeffK6 =
          (closureConstantCoeff * screeningEighth * (-15.0 * volumeFraction - 40.0) - 36.0 * closureLinearCoeff * screeningEighth
           - 360.0 * closureSinhCoeff * (screeningSixth * screeningParam * coshScreening - 7.0 * screeningSixth * sinhScreening
                                         + 42.0 * screeningQuad * screeningParam * coshScreening - 210.0 * screeningQuad * sinhScreening
                                         + 840.0 * screeningSq * screeningParam * coshScreening - 2520.0 * screeningSq * sinhScreening
                                         + 5040.0 * screeningParam * coshScreening - 5040.0 * sinhScreening)
           + 45.0 * closureCoshCoeff * (screeningEighth - 8.0 * screeningSixth * screeningParam * sinhScreening
                                        + 56.0 * screeningSixth * coshScreening - 336.0 * screeningQuad * screeningParam * sinhScreening
                                        + 1680.0 * screeningQuad * coshScreening - 6720.0 * screeningSq * screeningParam * sinhScreening
                                        + 20160.0 * screeningSq * coshScreening - 40320.0 * screeningParam * sinhScreening
                                        + 40320.0 * coshScreening - 40320.0)
           + 360.0 * contactPotential * (screeningSixth * screeningParam + 7.0 * screeningSixth
                                         + 42.0 * screeningQuad * screeningParam + 210.0 * screeningQuad
                                         + 840.0 * screeningSq * screeningParam + 2520.0 * screeningSq
                                         + 5040.0 * screeningParam + 5040.0))
        / (1814400.0 * screeningEighth);

    double directCorrelationIntegral = coeffK0 + coeffK2 * kSq + coeffK4 * kQuad + coeffK6 * kSixth;
    return 1.0 / (1.0 - 24.0 * volumeFraction * directCorrelationIntegral);
}

/* The general formula (Hayter & Penfold eq. 14), used for
 * reducedWavevector outside the small-K zone, and as the fallback
 * within it if the Taylor series' own sanity check fails (see
 * structureFactorFromCoefficients below).                              */
static double generalStructureFactor(double reducedWavevector, double volumeFraction, double screeningParam,

                                      double contactPotential, double closureConstantCoeff, double closureLinearCoeff,
                                      double closureSinhCoeff, double closureCoshCoeff,
                                      double sinhScreening, double coshScreening)
{
    double sinK = sin(reducedWavevector), cosK = cos(reducedWavevector);
    double K2 = reducedWavevector * reducedWavevector, K3 = K2 * reducedWavevector, K4 = K3 * reducedWavevector;
    double invKTimesDenom = 1.0 / reducedWavevector / (K2 + screeningParam * screeningParam);

    double directCorrelationIntegral =
          closureConstantCoeff * (sinK - reducedWavevector * cosK) / K3
        + closureLinearCoeff * ((2.0 / K2 - 1.0) * reducedWavevector * cosK + 2.0 * sinK - 2.0 / reducedWavevector) / K3
        + closureConstantCoeff * volumeFraction
            * (24.0 / K3 + 4.0 * (1.0 - 6.0 / K2) * sinK
               - (1.0 - 12.0 / K2 + 24.0 / K4) * reducedWavevector * cosK) / 2.0 / K3
        + closureSinhCoeff * (screeningParam * coshScreening * sinK - reducedWavevector * sinhScreening * cosK) * invKTimesDenom
        + closureCoshCoeff * (screeningParam * sinhScreening * sinK - reducedWavevector * (coshScreening * cosK - 1.0)) * invKTimesDenom
        + closureCoshCoeff * (cosK - 1.0) / K2
        - contactPotential * (screeningParam * sinK + reducedWavevector * cosK) * invKTimesDenom;

    return 1.0 / (1.0 - 24.0 * volumeFraction * directCorrelationIntegral);
}

/* d(directCorrelationIntegral)/d(reducedWavevector), for the general
 * formula above -- each term below is the derivative of the
 * correspondingly-named term in generalStructureFactor(), derived
 * symbolically (SymPy) and cross-checked against a central-difference
 * approximation before use (see test_derivative.c: matched to
 * O(h^2)-consistent precision, not just "close"). Combined into
 * dS/d(reducedWavevector) via the chain rule dS/dK = 24*eta*S(K)^2*da/dK
 * (from S=1/(1-24*eta*a), so dS/da=24*eta*S^2) by rmsa_structure_factor_derivative()
 * below, which also applies the further chain-rule factor for K itself
 * being qSigma/rescalingFactor, matching rmsa_structure_factor()'s own
 * K convention exactly.                                                */
static double generalDirectCorrelationIntegralDerivative(double reducedWavevector, double volumeFraction,
                                                           double screeningParam, double contactPotential,
                                                           double closureConstantCoeff, double closureLinearCoeff,
                                                           double closureSinhCoeff, double closureCoshCoeff,
                                                           double sinhScreening, double coshScreening)
{
    double sinK = sin(reducedWavevector), cosK = cos(reducedWavevector);
    double K = reducedWavevector;
    double K2 = K * K, K3 = K2 * K, K4 = K3 * K, K5 = K4 * K;
    double screeningSq = screeningParam * screeningParam;
    double sumSq = K2 + screeningSq;
    double sumSqSquared = sumSq * sumSq;

    double dTerm1 = (3.0 * K * cosK + K2 * sinK - 3.0 * sinK) / K4;
    double dTerm2 = (-8.0 * K * sinK + 4.0 * K2 * cosK + K3 * sinK - 8.0 * cosK + 8.0) / K5;
    double dTerm3 = 0.5 * (144.0 * K * sinK - 72.0 * K2 * cosK - 24.0 * K3 * sinK + 6.0 * K4 * cosK
                            + K5 * sinK + 144.0 * cosK - 144.0) / (K3 * K4);
    double dTerm4 = (K * sumSq * (K * sinK * sinhScreening + cosK * coshScreening * screeningParam - cosK * sinhScreening)
                      + 2.0 * K2 * (K * cosK * sinhScreening - coshScreening * screeningParam * sinK)
                      + sumSq * (K * cosK * sinhScreening - coshScreening * screeningParam * sinK))
                     / (K2 * sumSqSquared);
    double dTerm5 = (K * sumSq * (K * coshScreening * sinK - cosK * coshScreening + cosK * screeningParam * sinhScreening + 1.0)
                      + 2.0 * K2 * (K * (cosK * coshScreening - 1.0) - screeningParam * sinK * sinhScreening)
                      + sumSq * (K * (cosK * coshScreening - 1.0) - screeningParam * sinK * sinhScreening))
                     / (K2 * sumSqSquared);
    double dTerm6 = (-K * sinK - 2.0 * cosK + 2.0) / K3;
    double dTerm7 = (K * sumSq * (-K * sinK + cosK * screeningParam + cosK)
                      - 2.0 * K2 * (K * cosK + screeningParam * sinK)
                      - sumSq * (K * cosK + screeningParam * sinK))
                     / (K2 * sumSqSquared);

    return closureConstantCoeff * dTerm1 + closureLinearCoeff * dTerm2 + closureConstantCoeff * volumeFraction * dTerm3
         + closureSinhCoeff * dTerm4 + closureCoshCoeff * dTerm5 + closureCoshCoeff * dTerm6 - contactPotential * dTerm7;
}

static double generalStructureFactorDerivative(double reducedWavevector, double volumeFraction, double screeningParam,
                                                 double contactPotential, double closureConstantCoeff, double closureLinearCoeff,
                                                 double closureSinhCoeff, double closureCoshCoeff,
                                                 double sinhScreening, double coshScreening)
{
    double S = generalStructureFactor(reducedWavevector, volumeFraction, screeningParam, contactPotential,
                                       closureConstantCoeff, closureLinearCoeff, closureSinhCoeff, closureCoshCoeff,
                                       sinhScreening, coshScreening);
    double dA_dK = generalDirectCorrelationIntegralDerivative(reducedWavevector, volumeFraction, screeningParam,
                                                                contactPotential, closureConstantCoeff, closureLinearCoeff,
                                                                closureSinhCoeff, closureCoshCoeff, sinhScreening, coshScreening);
    return 24.0 * volumeFraction * S * S * dA_dK;
}

/* Same idea as generalStructureFactorDerivative() above, but built from
 * smallKStructureFactorTaylorSeries()'s own a(K) = coeffK0 + coeffK2*K^2
 * + coeffK4*K^4 + coeffK6*K^6 series instead of the general formula --
 * for the same reason rmsa_structure_factor() itself switches formulas
 * near K=0 (see that series' doc comment): the general formula's da/dK
 * above suffers the same catastrophic cancellation there that a(K)
 * itself does. da/dK = 2*coeffK2*K + 4*coeffK4*K^3 + 6*coeffK6*K^5 is
 * just this series differentiated term-by-term -- no new cancellation
 * risk of its own, since it's a sum of a few clean polynomial terms in
 * K, each with no subtraction of comparable large quantities. The
 * coefficient formulas themselves are copied verbatim from
 * smallKStructureFactorTaylorSeries() (not shared via a helper, to keep
 * that already-validated function completely untouched -- the same
 * choice made for computeQuarticSetup() in the warm-start code, for the
 * same reason); test_derivative.c checks the two independently-computed
 * copies stay in step with each other by comparing S(K) itself, not just
 * the derivative.                                                       */
static double smallKStructureFactorDerivativeTaylorSeries(double reducedWavevector, double volumeFraction,
                                                            double screeningParam, double contactPotential,
                                                            double closureConstantCoeff, double closureLinearCoeff,
                                                            double closureSinhCoeff, double closureCoshCoeff,
                                                            double sinhScreening, double coshScreening)
{
    double screeningSq = screeningParam * screeningParam;
    double screeningQuad = screeningSq * screeningSq;
    double screeningSixth = screeningQuad * screeningSq;
    double screeningEighth = screeningSixth * screeningSq;
    double K = reducedWavevector;
    double kSq = K * K, kQuad = kSq * kSq;

    double coeffK0 =
          (closureConstantCoeff * screeningSq * (volumeFraction + 4.0) + 3.0 * closureLinearCoeff * screeningSq
           + 12.0 * closureSinhCoeff * (screeningParam * coshScreening - sinhScreening)
           + 6.0 * closureCoshCoeff * (2.0 - screeningSq - 2.0 * coshScreening + 2.0 * screeningParam * sinhScreening)
           - 12.0 * contactPotential * (screeningParam + 1.0))
        / (12.0 * screeningSq);

    double coeffK2 =
          (-closureConstantCoeff * screeningQuad * (15.0 * volumeFraction + 48.0) - 40.0 * closureLinearCoeff * screeningQuad
           - 240.0 * closureSinhCoeff * (screeningSq * screeningParam * coshScreening - 3.0 * screeningSq * sinhScreening
                                          + 6.0 * screeningParam * coshScreening - 6.0 * sinhScreening)
           + 60.0 * closureCoshCoeff * (screeningQuad - 4.0 * screeningSq * screeningParam * sinhScreening
                                        + 12.0 * screeningSq * coshScreening - 24.0 * screeningParam * sinhScreening
                                        + 24.0 * coshScreening - 24.0)
           + 240.0 * contactPotential * (screeningSq * screeningParam + 3.0 * screeningSq + 6.0 * screeningParam + 6.0))
        / (1440.0 * screeningQuad);

    double coeffK4 =
          (closureConstantCoeff * screeningSixth * (42.0 * volumeFraction + 120.0) + 105.0 * closureLinearCoeff * screeningSixth
           + 840.0 * closureSinhCoeff * (screeningQuad * screeningParam * coshScreening - 5.0 * screeningQuad * sinhScreening
                                          + 20.0 * screeningSq * screeningParam * coshScreening - 60.0 * screeningSq * sinhScreening
                                          + 120.0 * screeningParam * coshScreening - 120.0 * sinhScreening)
           - 140.0 * closureCoshCoeff * (screeningSixth - 6.0 * screeningQuad * screeningParam * sinhScreening
                                         + 30.0 * screeningQuad * coshScreening - 120.0 * screeningSq * screeningParam * sinhScreening
                                         + 360.0 * screeningSq * coshScreening - 720.0 * screeningParam * sinhScreening
                                         + 720.0 * coshScreening - 720.0)
           - 840.0 * contactPotential * (screeningQuad * screeningParam + 5.0 * screeningQuad + 20.0 * screeningSq * screeningParam
                                         + 60.0 * screeningSq + 120.0 * screeningParam + 120.0))
        / (100800.0 * screeningSixth);

    double coeffK6 =
          (closureConstantCoeff * screeningEighth * (-15.0 * volumeFraction - 40.0) - 36.0 * closureLinearCoeff * screeningEighth
           - 360.0 * closureSinhCoeff * (screeningSixth * screeningParam * coshScreening - 7.0 * screeningSixth * sinhScreening
                                         + 42.0 * screeningQuad * screeningParam * coshScreening - 210.0 * screeningQuad * sinhScreening
                                         + 840.0 * screeningSq * screeningParam * coshScreening - 2520.0 * screeningSq * sinhScreening
                                         + 5040.0 * screeningParam * coshScreening - 5040.0 * sinhScreening)
           + 45.0 * closureCoshCoeff * (screeningEighth - 8.0 * screeningSixth * screeningParam * sinhScreening
                                        + 56.0 * screeningSixth * coshScreening - 336.0 * screeningQuad * screeningParam * sinhScreening
                                        + 1680.0 * screeningQuad * coshScreening - 6720.0 * screeningSq * screeningParam * sinhScreening
                                        + 20160.0 * screeningSq * coshScreening - 40320.0 * screeningParam * sinhScreening
                                        + 40320.0 * coshScreening - 40320.0)
           + 360.0 * contactPotential * (screeningSixth * screeningParam + 7.0 * screeningSixth
                                         + 42.0 * screeningQuad * screeningParam + 210.0 * screeningQuad
                                         + 840.0 * screeningSq * screeningParam + 2520.0 * screeningSq
                                         + 5040.0 * screeningParam + 5040.0))
        / (1814400.0 * screeningEighth);

    double directCorrelationIntegral = coeffK0 + coeffK2 * kSq + coeffK4 * kQuad + coeffK6 * kQuad * kSq;
    double S = 1.0 / (1.0 - 24.0 * volumeFraction * directCorrelationIntegral);
    double dA_dK = 2.0 * coeffK2 * K + 4.0 * coeffK4 * K * kSq + 6.0 * coeffK6 * K * kQuad;
    return 24.0 * volumeFraction * S * S * dA_dK;
}

/* dS/d(qSigma), where qSigma is exactly rmsa_structure_factor()'s own
 * "K" parameter (K plays the role of "qR2" in the reference; see that
 * function's doc comment) -- i.e. these two functions are meant to be
 * called with the same K convention, so e.g. a central-difference
 * check of this derivative against rmsa_structure_factor() itself uses
 * the same K on both sides. Exactly zero at K=0 (S(K) is even in K, by
 * the model's isotropy, so its derivative is odd and vanishes at the
 * origin) -- returned directly rather than through either formula
 * below, both of which divide by K.                                    */
double rmsa_structure_factor_derivative(const RMSASolution *solution, double K)
{
    if (K == 0.0) return 0.0;

    double reducedWavevector = K / solution->rescalingFactor;
    /* Same guard as structureFactorFromCoefficients()'s own dispatch
     * (see its screeningParam > 25.0 check below): missing this in an
     * earlier version of this function caused catastrophically wrong
     * results (values as absurd as 1e17, found by a 3000-case
     * randomized sweep, not the smaller hand-picked cases) for large
     * screeningParam, where sinh/cosh become huge enough that further
     * arithmetic with them is numerically unreliable well before
     * actually overflowing.                                           */
    double sinhScreening, coshScreening;
    if (solution->rescaledScreeningParam > 25.0) { sinhScreening = 0.0; coshScreening = 0.0; }
    else { sinhScreening = sinh(solution->rescaledScreeningParam); coshScreening = cosh(solution->rescaledScreeningParam); }

    double dSdReducedWavevector;
    /* Wider cutoff than rmsa_structure_factor()'s own 0.02 (see that
     * function's dispatch below): differentiating the general
     * formula's already-cancellation-prone terms makes the
     * cancellation *worse*, not better -- confirmed via arbitrary-
     * precision (mpmath) ground truth, the general formula's
     * derivative already shows ~1% error at reducedWavevector=0.03,
     * only dropping to negligible by roughly 0.08-0.1, a noticeably
     * wider danger zone than the value function has. The Taylor
     * series derivative itself has no such issue (a low-degree
     * polynomial in K, no subtraction of comparable large terms), and
     * remains accurate over the same range the value function's own
     * series is already established to be safe in (see "Known
     * Limitations" in README.md) -- so widening *only* this cutoff,
     * not rmsa_structure_factor()'s, is the safe fix. See
     * test_derivative.c for the sweep that both found this and
     * confirms 0.08 resolves it.                                       */
    if (fabs(reducedWavevector) <= 0.08) {
        dSdReducedWavevector = smallKStructureFactorDerivativeTaylorSeries(
            reducedWavevector, solution->rescaledVolumeFraction, solution->rescaledScreeningParam,
            solution->rescaledContactPotential, solution->closureConstantCoeff, solution->closureLinearCoeff,
            solution->closureSinhCoeff, solution->closureCoshCoeff, sinhScreening, coshScreening);
    } else {
        dSdReducedWavevector = generalStructureFactorDerivative(
            reducedWavevector, solution->rescaledVolumeFraction, solution->rescaledScreeningParam,
            solution->rescaledContactPotential, solution->closureConstantCoeff, solution->closureLinearCoeff,
            solution->closureSinhCoeff, solution->closureCoshCoeff, sinhScreening, coshScreening);
    }
    /* chain rule for K = qSigma = reducedWavevector*rescalingFactor */
    return dSdReducedWavevector / solution->rescalingFactor;
}

/* --------------------------------------------------------------------
 * Precomputed trig table for the g(r) physical-root-validation loops
 * below (both the cold-start candidate loop and
 * attemptWarmStartedSolve()'s single-candidate check). Both always
 * evaluate the exact same fixed sequence radius = n*radialStep for
 * n=0..8191 (rescalingFactor is always 1 there -- see this table's
 * user sites for why), regardless of which physical parameters are
 * being solved for -- so sin/cos at these points can be computed once
 * per process and reused forever, rather than 8192 times per g(r)
 * test. Measured contribution: see README.md's "Performance" section.
 *
 * Deliberately NOT used by generalStructureFactor()/
 * structureFactorFromCoefficients() themselves, which remain exactly
 * as they were and serve rmsa_structure_factor()'s public API on
 * arbitrary caller-supplied K values that do not follow this fixed
 * sequence -- generalStructureFactorPrecomputedTrig() below is a
 * separate, byte-for-byte-checked duplicate (see
 * test_gr_trig_table.c) used only internally by the two loops that
 * actually iterate over this exact grid, so this optimization has no
 * way to affect that unrelated, already-validated public path.        */
#define GR_TEST_NUM_POINTS 8192
#define GR_TEST_RADIAL_STEP 0.05
#define GR_TEST_CONTACT_SINE_INDEX 19

typedef struct {
    double sinK[GR_TEST_NUM_POINTS];
    double cosK[GR_TEST_NUM_POINTS];
    double sinWeight[GR_TEST_NUM_POINTS]; /* sin(GR_TEST_CONTACT_SINE_INDEX*GR_TEST_RADIAL_STEP*radius) */
} GRTestTrigTable;

static GRTestTrigTable gGRTestTrigTable;
static pthread_once_t gGRTestTrigTableOnceGuard = PTHREAD_ONCE_INIT;

static void initGRTestTrigTable(void)
{
    for (int n = 0; n < GR_TEST_NUM_POINTS; n++) {
        double radius = n * GR_TEST_RADIAL_STEP;
        gGRTestTrigTable.sinK[n] = sin(radius);
        gGRTestTrigTable.cosK[n] = cos(radius);
        gGRTestTrigTable.sinWeight[n] = sin(GR_TEST_CONTACT_SINE_INDEX * GR_TEST_RADIAL_STEP * radius);
    }
}

/* This function must be called before reading gGRTestTrigTable, and is
 * safe to call from multiple threads concurrently (pthread_once()
 * guarantees initGRTestTrigTable() itself runs exactly once, and the
 * table is read-only for the remaining lifetime of the process after
 * that -- the same reasoning, and the same mechanism, already used for
 * GSL's error handler in polyroots.c).                                */
static void ensureGRTestTrigTableInitialized(void)
{
    pthread_once(&gGRTestTrigTableOnceGuard, initGRTestTrigTable);
}

/* Byte-for-byte the same formula as generalStructureFactor() above,
 * except sinK/cosK are supplied directly (from gGRTestTrigTable)
 * instead of being computed internally via sin()/cos(). Deliberately a
 * separate function, not a parameterized version of
 * generalStructureFactor() itself -- see this table's own doc comment
 * for why keeping them fully independent, rather than sharing code,
 * was the safer choice given how validated the original already is.   */
static double generalStructureFactorPrecomputedTrig(double reducedWavevector, double sinK, double cosK,
                                                      double volumeFraction, double screeningParam,
                                                      double contactPotential, double closureConstantCoeff,
                                                      double closureLinearCoeff, double closureSinhCoeff,
                                                      double closureCoshCoeff, double sinhScreening, double coshScreening)
{
    double K2 = reducedWavevector * reducedWavevector, K3 = K2 * reducedWavevector, K4 = K3 * reducedWavevector;
    double invKTimesDenom = 1.0 / reducedWavevector / (K2 + screeningParam * screeningParam);

    double directCorrelationIntegral =
          closureConstantCoeff * (sinK - reducedWavevector * cosK) / K3
        + closureLinearCoeff * ((2.0 / K2 - 1.0) * reducedWavevector * cosK + 2.0 * sinK - 2.0 / reducedWavevector) / K3
        + closureConstantCoeff * volumeFraction
            * (24.0 / K3 + 4.0 * (1.0 - 6.0 / K2) * sinK
               - (1.0 - 12.0 / K2 + 24.0 / K4) * reducedWavevector * cosK) / 2.0 / K3
        + closureSinhCoeff * (screeningParam * coshScreening * sinK - reducedWavevector * sinhScreening * cosK) * invKTimesDenom
        + closureCoshCoeff * (screeningParam * sinhScreening * sinK - reducedWavevector * (coshScreening * cosK - 1.0)) * invKTimesDenom
        + closureCoshCoeff * (cosK - 1.0) / K2
        - contactPotential * (screeningParam * sinK + reducedWavevector * cosK) * invKTimesDenom;

    return 1.0 / (1.0 - 24.0 * volumeFraction * directCorrelationIntegral);
}

/* True for a physically plausible S(K) at a small reducedWavevector:
 * non-negative, finite, and not wildly different from the exact S(0)
 * -- which the true S(K) cannot be, for small enough K, since S(K) is
 * analytic and S(K)-S(0) is O(K^2). This is the sanity check that lets
 * structureFactorFromCoefficients notice when the Taylor series (or,
 * in the fallback, the general formula) has itself broken down rather
 * than silently returning a bad number -- see the doc comment there
 * for why both can, for sufficiently extreme (typically very small
 * screeningParam) coefficient sets, and README.md's "Known
 * Limitations" for how this was found (a 500-case, fine-K-resolution
 * sweep -- a coarser 2-point check missed it entirely).               */
static int isPlausibleSmallKResult(double candidate, double exactSAtZero)
{
    if (!isfinite(candidate) || candidate < 0.0) return 0;
    return fabs(candidate - exactSAtZero) <= 0.05 * fabs(exactSAtZero);
}

static double structureFactorFromCoefficients(double qSigma, double rescalingFactor, double volumeFraction,
                                               double screeningParam, double contactPotential,
                                               double closureConstantCoeff, double closureLinearCoeff,
                                               double closureSinhCoeff, double closureCoshCoeff)
{
    double exactSAtZero = -1.0 / closureConstantCoeff;
    if (qSigma == 0.0) return exactSAtZero;

    double reducedWavevector = qSigma / rescalingFactor;
    double sinhScreening, coshScreening;
    if (screeningParam > 25.0) { sinhScreening = 0.0; coshScreening = 0.0; }
    else                       { sinhScreening = sinh(screeningParam); coshScreening = cosh(screeningParam); }

    if (fabs(reducedWavevector) <= 0.02) {
        /* Try the Taylor series first (see its doc comment); if its own
         * arithmetic has broken down for this coefficient set, fall
         * back to the general formula; if that has too, the least-wrong
         * thing left is the exact K=0 value itself, whose error is
         * O(reducedWavevector^2) -- small, since we're already inside
         * the K<=0.02 zone.                                            */
        double taylorValue = smallKStructureFactorTaylorSeries(reducedWavevector, volumeFraction, screeningParam,
                                                                contactPotential, closureConstantCoeff, closureLinearCoeff,
                                                                closureSinhCoeff, closureCoshCoeff,
                                                                sinhScreening, coshScreening);
        if (isPlausibleSmallKResult(taylorValue, exactSAtZero)) return taylorValue;

        double generalValueAsFallback = generalStructureFactor(reducedWavevector, volumeFraction, screeningParam,
                                                                 contactPotential, closureConstantCoeff, closureLinearCoeff,
                                                                 closureSinhCoeff, closureCoshCoeff,
                                                                 sinhScreening, coshScreening);
        if (isPlausibleSmallKResult(generalValueAsFallback, exactSAtZero)) return generalValueAsFallback;

        return exactSAtZero;
    }

    return generalStructureFactor(reducedWavevector, volumeFraction, screeningParam, contactPotential,
                                   closureConstantCoeff, closureLinearCoeff, closureSinhCoeff, closureCoshCoeff,
                                   sinhScreening, coshScreening);
}

double rmsa_structure_factor(const RMSASolution *solution, double K)
{
    /* K plays the role of "qR2" in the reference (rescalingFactor divides
     * it back out inside structureFactorFromCoefficients).              */
    return structureFactorFromCoefficients(K, solution->rescalingFactor, solution->rescaledVolumeFraction,
                                            solution->rescaledScreeningParam, solution->rescaledContactPotential,
                                            solution->closureConstantCoeff, solution->closureLinearCoeff,
                                            solution->closureSinhCoeff, solution->closureCoshCoeff);
}

/* Treat a computed quartic root as "real" if its imaginary part is
 * negligible compared to its overall magnitude -- stands in for
 * numpy's exact imag==0 check, which relies on LAPACK's real-Schur-form
 * structure that our closed-form complex quartic solver does not
 * reproduce bit for bit (see README.md for discussion).                */
static int isNegligibleImaginaryPart(double complex z)
{
    double scale = fmax(1.0, cabs(z));
    return fabs(cimag(z)) < 1e-9 * scale;
}

static int compareDoubles(const void *pa, const void *pb)
{
    double valueA = *(const double *)pa, valueB = *(const double *)pb;
    return (valueA > valueB) - (valueA < valueB);
}

/* Computes the MSA closure coefficients (and, in SQFUN_FULL_SOLVE mode,
 * selects the physically correct quartic root) for one trial volume
 * fraction. This is the workhorse behind both the initial solve and the
 * eta-rescaling search in rmsa_solve().
 *
 * mode: which of the three things described by SqfunMode to do.
 * trialVolumeFraction: the eta to use in the coefficient formulas below
 *   (a rescaling candidate in SQFUN_NEWTON_RESIDUAL/SQFUN_FINAL_RESCALED,
 *   or simply the physical eta itself for the initial SQFUN_FULL_SOLVE).
 * physicalVolumeFraction, physicalContactPotential, physicalScreeningParam:
 *   the *original*, unrescaled eta/gek/ak -- fixed across the whole
 *   solve/rescale procedure; used (together with trialVolumeFraction)
 *   to compute the rescaling factor and the rescaled gek/ak below.
 *
 * Returns 0 on success (residualOut and *solution's coefficient fields
 * filled), or -4 if SQFUN_FULL_SOLVE finds no physical root. In
 * SQFUN_FULL_SOLVE mode, *residualOut is not physically meaningful --
 * the true g(1+) is left in solution->contactValue (see rmsa_solve).   */

/* Everything needed to (a) evaluate the MSA quartic p(fa)=0 and its
 * derivative at a candidate "fa", and (b) recover the corresponding
 * "ca" and the resulting contact value once a root fa is known --
 * i.e. everything sqfun()'s SQFUN_FULL_SOLVE branch computes *before*
 * calling quartic_roots(), for the (volumeFraction, screeningParam,
 * contactPotential) already given directly (no rescaling attempted:
 * this matches trialVolumeFraction==physicalVolumeFraction,
 * rescalingFactor=1). Restricted to the general branch
 * (screeningParam<=15); the large-screening asymptotic shortcut needs
 * none of this (see rmsa_solve_warm_started()'s doc comment).
 *
 * Deliberately a *separate* function from sqfun(), duplicating rather
 * than sharing its formulas: sqfun() is the original, extensively
 * validated (jscatter/SasView/Percus-Yevick/...) code path, used
 * unconditionally by every existing caller. Refactoring it to share
 * code with this new, less-tested warm-start path would risk
 * introducing a subtle bug into that already-correct path for the
 * sake of a purely additive optimization. Instead, this duplicate is
 * validated by a different means: rmsa_solve_warm_started() below
 * falls back to (and is checked against) the ordinary cold-start
 * sqfun()-based solve whenever there is any doubt -- see
 * test_warm_start.c, which checks many (parameter, small-perturbation)
 * pairs give results from this path bit-identical to the cold-start
 * path, and that deliberately-large perturbations correctly trigger
 * (and correctly use) the fallback rather than a wrong warm-started
 * answer.                                                              */
typedef struct {
    double quarticCoeff4, quarticCoeff3, quarticCoeff2, quarticCoeff1, quarticCoeff0;
    double elimW16, elimW15, elimW14, elimW13, elimW12; /* ca(fa) = -(elimW16*fa^2+elimW15*fa+elimW14)/(elimW13*fa+elimW12) */
    double closureConstantBase, closureConstantCaCoeff, closureConstantFaCoeff;
    double closureLinearBase, closureLinearCaCoeff, closureLinearFaCoeff;
    double auxVBase, auxVCaCoeff, auxVFaCoeff;
    double contactGBase, contactGCaCoeff, contactGFaCoeff;
} QuarticSetup;

static void computeQuarticSetup(double volumeFraction, double screeningParam, double contactPotential,
                                 QuarticSetup *setup)
{
    double etaSq = volumeFraction * volumeFraction;
    double etaCubed = etaSq * volumeFraction;
    double twelveEta = 12.0 * volumeFraction;
    double twentyFourEta = twelveEta + twelveEta;
    double screeningSq = screeningParam * screeningParam;
    double onePlusScreening = 1.0 + screeningParam;
    double invScreeningSq = 1.0 / screeningSq;
    double invScreeningQuad = invScreeningSq * invScreeningSq;
    double oneMinusEta = 1.0 - volumeFraction;
    double oneMinusEtaSq = oneMinusEta * oneMinusEta;
    double oneMinusEtaOverScreening = oneMinusEta / screeningParam;
    double invOneMinusEtaSq = 1.0 / oneMinusEtaSq;
    double invOneMinusEtaQuad = invOneMinusEtaSq * invOneMinusEtaSq;
    double invOneMinusEtaQuadFifth = invOneMinusEtaQuad * 2.0e-1;
    double threeEta = 3.0 * volumeFraction;
    double sixEta = threeEta + threeEta;
    double etaPlusTwo = volumeFraction + 2.0;
    double etaPlusTwoSq = etaPlusTwo * etaPlusTwo;
    double twoEtaPlusOne = 2.0 * volumeFraction + 1.0;
    double twoEtaPlusOneSq = twoEtaPlusOne * twoEtaPlusOne;

    double alpha1 = -twoEtaPlusOne * oneMinusEtaOverScreening;
    double alpha2 = (14.0 * etaSq - 4.0 * volumeFraction - 1.0) * invScreeningSq;
    double alpha3 = 36.0 * etaSq * invScreeningQuad;

    double beta1 = -(etaSq + 7.0 * volumeFraction + 1.0) * oneMinusEtaOverScreening;
    double beta2 = 9.0 * volumeFraction * (etaSq + 4.0 * volumeFraction - 2.0) * invScreeningSq;
    double beta3 = 12.0 * volumeFraction * (2.0 * etaSq + 8.0 * volumeFraction - 1.0) * invScreeningQuad;

    double nu1 = -(etaCubed + 3.0 * etaSq + 45.0 * volumeFraction + 5.0) * oneMinusEtaOverScreening;
    double nu2 = (2.0 * etaCubed + 3.0 * etaSq + 42.0 * volumeFraction - 20.0) * invScreeningSq;
    double nu3 = (2.0 * etaCubed + 30.0 * volumeFraction - 5.0) * invScreeningQuad;
    double nu4 = nu1 + 24.0 * volumeFraction * screeningParam * nu3;
    double nu5 = sixEta * (nu2 + 4.0 * nu3);

    double phi1 = sixEta / screeningParam;
    double phi2 = oneMinusEta - 12.0 * volumeFraction * invScreeningSq;
    double phi1Sq = phi1 * phi1;
    double phi2Sq = phi2 * phi2;

    double tau1 = (volumeFraction + 5.0) / (5.0 * screeningParam);
    double tau2 = etaPlusTwo * invScreeningSq;
    double tau3 = -12.0 * volumeFraction * contactPotential * (tau1 + tau2);
    double tau4 = threeEta * screeningSq * (tau1 * tau1 - tau2 * tau2);
    double tau5 = threeEta * (volumeFraction + 8.0) * 0.1 - 2.0 * twoEtaPlusOneSq * invScreeningSq;

    double closureConstantBase = (twentyFourEta * contactPotential * (alpha1 + alpha2 + onePlusScreening * alpha3) - twoEtaPlusOneSq) * invOneMinusEtaQuad;
    double closureLinearBase = (1.5 * volumeFraction * etaPlusTwoSq - 12.0 * volumeFraction * contactPotential * (beta1 + beta2 + onePlusScreening * beta3)) * invOneMinusEtaQuad;
    double auxVBase = (twoEtaPlusOne * (etaSq - 2.0 * volumeFraction + 10.0) * 0.25 - contactPotential * (nu4 + nu5)) * invOneMinusEtaQuadFifth;
    double contactGBase = (contactPotential * (phi1Sq + phi2Sq - 2.0 * phi1 * phi2) - 0.5 * etaPlusTwo) * invOneMinusEtaSq;
    double constraintBase = tau3 + tau4 * closureConstantBase + tau5 * closureLinearBase;

    double sinhScreening = sinh(screeningParam);
    double coshScreening = cosh(screeningParam);
    double coshMinusOneMinusScreeningSinh = coshScreening - 1.0 - screeningParam * sinhScreening;
    double sinhMinusScreeningCosh = sinhScreening - screeningParam * coshScreening;

    double closureConstantCaCoeff = twentyFourEta * (alpha3 * sinhMinusScreeningCosh + alpha2 * sinhScreening - alpha1 * coshScreening) * invOneMinusEtaQuad;
    double closureConstantFaCoeff = twentyFourEta * (twoEtaPlusOneSq * invScreeningSq - 0.5 * oneMinusEtaSq
                              + alpha3 * coshMinusOneMinusScreeningSinh - alpha1 * sinhScreening + alpha2 * coshScreening) * invOneMinusEtaQuad;
    double closureLinearCaCoeff = twelveEta * (-beta3 * sinhMinusScreeningCosh - beta2 * sinhScreening + beta1 * coshScreening) * invOneMinusEtaQuad;
    double closureLinearFaCoeff = twelveEta * (0.5 * oneMinusEtaSq * etaPlusTwo - threeEta * etaPlusTwoSq * invScreeningSq
                            - beta3 * coshMinusOneMinusScreeningSinh + beta1 * sinhScreening - beta2 * coshScreening) * invOneMinusEtaQuad;
    double auxVCaCoeff = (nu4 * coshScreening - nu5 * sinhScreening) * invOneMinusEtaQuadFifth;
    double auxVFaCoeff = ((etaCubed - 6.0 * etaSq + 5.0) * oneMinusEta
                   - sixEta * (2.0 * etaCubed - 3.0 * etaSq + 18.0 * volumeFraction + 10.0) * invScreeningSq
                   + twentyFourEta * nu3 + nu4 * sinhScreening - nu5 * coshScreening) * invOneMinusEtaQuadFifth;
    double contactGCaCoeff = ((phi1Sq + phi2Sq) * sinhScreening + 2.0 * phi1 * phi2 * coshScreening) * invOneMinusEtaSq;
    double contactGFaCoeff = ((phi1Sq + phi2Sq) * coshScreening + 2.0 * phi1 * phi2 * sinhScreening + phi1Sq - phi2Sq) * invOneMinusEtaSq;

    double constraintCaCoeff = tau4 * closureConstantCaCoeff + tau5 * closureLinearCaCoeff + twelveEta * (tau1 * coshScreening - tau2 * sinhScreening);
    double constraintFaCoeff = tau4 * closureConstantFaCoeff + tau5 * closureLinearFaCoeff
                         + twelveEta * (tau1 * sinhScreening - tau2 * (coshScreening - 1.0))
                         - 0.4 * volumeFraction * (volumeFraction + 10.0) - 1.0;

    double firstConstraintM1 = constraintCaCoeff * closureConstantCaCoeff - twelveEta * auxVCaCoeff * auxVCaCoeff;
    double firstConstraintM2 = constraintBase * closureConstantCaCoeff + constraintCaCoeff * closureConstantBase - twentyFourEta * auxVBase * auxVCaCoeff;
    double firstConstraintM3 = constraintCaCoeff * closureConstantFaCoeff + constraintFaCoeff * closureConstantCaCoeff - twentyFourEta * auxVCaCoeff * auxVFaCoeff;
    double firstConstraintM4 = constraintBase * closureConstantBase - twelveEta * auxVBase * auxVBase;
    double firstConstraintM5 = constraintBase * closureConstantFaCoeff + constraintFaCoeff * closureConstantBase - twentyFourEta * auxVBase * auxVFaCoeff;
    double firstConstraintM6 = constraintFaCoeff * closureConstantFaCoeff - twelveEta * auxVFaCoeff * auxVFaCoeff;

    double secondConstraintL1 = twelveEta * contactGCaCoeff * contactGCaCoeff;
    double secondConstraintL2 = twentyFourEta * contactGBase * contactGCaCoeff - 2.0 * closureLinearCaCoeff;
    double secondConstraintL3 = twentyFourEta * contactGCaCoeff * contactGFaCoeff;
    double secondConstraintL4 = twelveEta * contactGBase * contactGBase - 2.0 * closureLinearBase;
    double secondConstraintL5 = twentyFourEta * contactGBase * contactGFaCoeff - 2.0 * closureLinearFaCoeff - screeningSq;
    double secondConstraintL6 = twelveEta * contactGFaCoeff * contactGFaCoeff;

    double elimW16 = firstConstraintM1 * secondConstraintL6 - secondConstraintL1 * firstConstraintM6;
    double elimW15 = firstConstraintM1 * secondConstraintL5 - secondConstraintL1 * firstConstraintM5;
    double elimW14 = firstConstraintM1 * secondConstraintL4 - secondConstraintL1 * firstConstraintM4;
    double elimW13 = firstConstraintM1 * secondConstraintL3 - secondConstraintL1 * firstConstraintM3;
    double elimW12 = firstConstraintM1 * secondConstraintL2 - secondConstraintL1 * firstConstraintM2;
    double elimW26 = firstConstraintM2 * secondConstraintL6 - secondConstraintL2 * firstConstraintM6;
    double elimW25 = firstConstraintM2 * secondConstraintL5 - secondConstraintL2 * firstConstraintM5;
    double elimW24 = firstConstraintM2 * secondConstraintL4 - secondConstraintL2 * firstConstraintM4;
    double elimW36 = firstConstraintM3 * secondConstraintL6 - secondConstraintL3 * firstConstraintM6;
    double elimW35 = firstConstraintM3 * secondConstraintL5 - secondConstraintL3 * firstConstraintM5;
    double elimW34 = firstConstraintM3 * secondConstraintL4 - secondConstraintL3 * firstConstraintM4;

    setup->quarticCoeff4 = elimW16 * elimW16 - elimW13 * elimW36;
    setup->quarticCoeff3 = 2.0 * elimW16 * elimW15 - elimW13 * (elimW35 + elimW26) - elimW12 * elimW36;
    setup->quarticCoeff2 = elimW15 * elimW15 + 2.0 * elimW16 * elimW14 - elimW13 * (elimW34 + elimW25) - elimW12 * (elimW35 + elimW26);
    setup->quarticCoeff1 = 2.0 * elimW15 * elimW14 - elimW13 * elimW24 - elimW12 * (elimW34 + elimW25);
    setup->quarticCoeff0 = elimW14 * elimW14 - elimW12 * elimW24;
    setup->elimW16 = elimW16; setup->elimW15 = elimW15; setup->elimW14 = elimW14;
    setup->elimW13 = elimW13; setup->elimW12 = elimW12;
    setup->closureConstantBase = closureConstantBase;
    setup->closureConstantCaCoeff = closureConstantCaCoeff;
    setup->closureConstantFaCoeff = closureConstantFaCoeff;
    setup->closureLinearBase = closureLinearBase;
    setup->closureLinearCaCoeff = closureLinearCaCoeff;
    setup->closureLinearFaCoeff = closureLinearFaCoeff;
    setup->auxVBase = auxVBase; setup->auxVCaCoeff = auxVCaCoeff; setup->auxVFaCoeff = auxVFaCoeff;
    setup->contactGBase = contactGBase; setup->contactGCaCoeff = contactGCaCoeff; setup->contactGFaCoeff = contactGFaCoeff;
}

static int sqfun(SqfunMode mode, double trialVolumeFraction,
                  double physicalVolumeFraction, double physicalContactPotential, double physicalScreeningParam,
                  RMSASolution *solution, double *residualOut)
{
    double volumeFraction = trialVolumeFraction;
    double rescalingFactor = cbrt(physicalVolumeFraction / trialVolumeFraction);
    double rescaledScreeningParam = physicalScreeningParam / rescalingFactor;
    double nonzeroContactPotential = (fabs(physicalContactPotential) > 1e-9) ? physicalContactPotential : 1e-9;
    double rescaledContactPotential =
        nonzeroContactPotential * rescalingFactor * exp(physicalScreeningParam - rescaledScreeningParam);

    /* local shorthands used throughout the formulas below */
    double contactPotential = rescaledContactPotential;
    double screeningParam = rescaledScreeningParam;

    double etaSq = volumeFraction * volumeFraction;
    double etaCubed = etaSq * volumeFraction;
    double twelveEta = 12.0 * volumeFraction;
    double twentyFourEta = twelveEta + twelveEta;
    double screeningSq = screeningParam * screeningParam;
    double onePlusScreening = 1.0 + screeningParam;
    double invScreeningSq = 1.0 / screeningSq;
    double invScreeningQuad = invScreeningSq * invScreeningSq;
    double oneMinusEta = 1.0 - volumeFraction;
    double oneMinusEtaSq = oneMinusEta * oneMinusEta;
    double oneMinusEtaOverScreening = oneMinusEta / screeningParam;
    double invOneMinusEtaSq = 1.0 / oneMinusEtaSq;
    double invOneMinusEtaQuad = invOneMinusEtaSq * invOneMinusEtaSq;
    double invOneMinusEtaQuadFifth = invOneMinusEtaQuad * 2.0e-1;
    double threeEta = 3.0 * volumeFraction;
    double sixEta = threeEta + threeEta;
    double twoEtaCubed = etaCubed + etaCubed;
    double etaPlusTwo = volumeFraction + 2.0;
    double etaPlusTwoSq = etaPlusTwo * etaPlusTwo;
    double twoEtaPlusOne = 2.0 * volumeFraction + 1.0;
    double twoEtaPlusOneSq = twoEtaPlusOne * twoEtaPlusOne;

    /* coefficients from the Appendix of Hayter & Penfold (1981); named
     * alpha/beta/nu/tau after the paper's Greek-letter symbols there. */
    double alpha1 = -twoEtaPlusOne * oneMinusEtaOverScreening;
    double alpha2 = (14.0 * etaSq - 4.0 * volumeFraction - 1.0) * invScreeningSq;
    double alpha3 = 36.0 * etaSq * invScreeningQuad;

    double beta1 = -(etaSq + 7.0 * volumeFraction + 1.0) * oneMinusEtaOverScreening;
    double beta2 = 9.0 * volumeFraction * (etaSq + 4.0 * volumeFraction - 2.0) * invScreeningSq;
    double beta3 = 12.0 * volumeFraction * (2.0 * etaSq + 8.0 * volumeFraction - 1.0) * invScreeningQuad;

    double nu1 = -(etaCubed + 3.0 * etaSq + 45.0 * volumeFraction + 5.0) * oneMinusEtaOverScreening;
    double nu2 = (twoEtaCubed + 3.0 * etaSq + 42.0 * volumeFraction - 20.0) * invScreeningSq;
    double nu3 = (twoEtaCubed + 30.0 * volumeFraction - 5.0) * invScreeningQuad;
    double nu4 = nu1 + 24.0 * volumeFraction * screeningParam * nu3;
    double nu5 = sixEta * (nu2 + 4.0 * nu3);

    double phi1 = sixEta / screeningParam;
    double phi2 = oneMinusEta - 12.0 * volumeFraction * invScreeningSq;

    double phi1Sq = phi1 * phi1;
    double phi2Sq = phi2 * phi2;
    double phiSqSum = phi1Sq + phi2Sq;
    double twicePhi1Phi2 = 2.0 * phi1 * phi2;

    double tau1 = (volumeFraction + 5.0) / (5.0 * screeningParam);
    double tau2 = etaPlusTwo * invScreeningSq;
    double tau3 = -12.0 * volumeFraction * contactPotential * (tau1 + tau2);
    double tau4 = threeEta * screeningSq * (tau1 * tau1 - tau2 * tau2);
    double tau5 = threeEta * (volumeFraction + 8.0) * 0.1 - 2.0 * twoEtaPlusOneSq * invScreeningSq;

    /* "base" (candidate-root-independent) contributions to the closure
     * coefficients A,B,V,and to the contact-value expression P; see the
     * closureConstantCaCoeff/closureConstantFaCoeff etc. comments below
     * for how these combine with the eventual quartic root.            */
    double closureConstantBase = (twentyFourEta * contactPotential * (alpha1 + alpha2 + onePlusScreening * alpha3) - twoEtaPlusOneSq) * invOneMinusEtaQuad;
    double closureLinearBase = (1.5 * volumeFraction * etaPlusTwoSq - 12.0 * volumeFraction * contactPotential * (beta1 + beta2 + onePlusScreening * beta3)) * invOneMinusEtaQuad;
    double auxVBase = (twoEtaPlusOne * (etaSq - 2.0 * volumeFraction + 10.0) * 0.25 - contactPotential * (nu4 + nu5)) * invOneMinusEtaQuadFifth;
    double contactGBase = (contactPotential * (phi1Sq + phi2Sq - twicePhi1Phi2) - 0.5 * etaPlusTwo) * invOneMinusEtaSq;
    double constraintBase = tau3 + tau4 * closureConstantBase + tau5 * closureLinearBase;

    /* coefficients of the sinh-root ("ca") and cosh-root ("fa") terms in
     * the same linear/quadratic combinations; declared once so both the
     * large-screening shortcut below and the general sinh/cosh branch
     * that follows it can assign into them.                            */
    double closureConstantCaCoeff, closureConstantFaCoeff;
    double closureLinearCaCoeff, closureLinearFaCoeff;
    double auxVCaCoeff, auxVFaCoeff;
    double contactGCaCoeff, contactGFaCoeff;
    double constraintCaCoeff, constraintFaCoeff;
    double firstConstraintM1, firstConstraintM2, firstConstraintM3;
    double firstConstraintM4, firstConstraintM5, firstConstraintM6;

    if (rescaledScreeningParam > 15.0 && mode == SQFUN_FULL_SOLVE) {
        /* Large-screening asymptotic solution: for screeningParam this
         * large, sinh(k) ~ cosh(k), which cancels the exponentially
         * growing parts and lets fa (the "cosh" root) be obtained
         * directly, without any quartic root search.                   */
        closureConstantFaCoeff = twentyFourEta * (twoEtaPlusOneSq * invScreeningSq - 0.5 * oneMinusEtaSq - alpha3) * invOneMinusEtaQuad;
        closureLinearFaCoeff = twelveEta * (0.5 * oneMinusEtaSq * etaPlusTwo - threeEta * etaPlusTwoSq * invScreeningSq + beta3) * invOneMinusEtaQuad;
        auxVFaCoeff = ((etaCubed - 6.0 * etaSq + 5.0) * oneMinusEta
                       - sixEta * (2.0 * etaCubed - 3.0 * etaSq + 18.0 * volumeFraction + 10.0) * invScreeningSq
                       + twentyFourEta * nu3) * invOneMinusEtaQuadFifth;
        contactGFaCoeff = (phi1Sq - phi2Sq) * invOneMinusEtaSq;
        constraintFaCoeff = tau4 * closureConstantFaCoeff + tau5 * closureLinearFaCoeff + twelveEta * tau2
                             - 0.4 * volumeFraction * (volumeFraction + 10.0) - 1.0;

        double bigScreeningM6 = constraintFaCoeff * closureConstantFaCoeff - twelveEta * auxVFaCoeff * auxVFaCoeff;
        double bigScreeningM5 = constraintBase * closureConstantFaCoeff + closureConstantBase * constraintFaCoeff
                                 - twentyFourEta * auxVBase * auxVFaCoeff;
        double bigScreeningM4 = constraintBase * closureConstantBase - twelveEta * auxVBase * auxVBase;
        double bigScreeningL6 = twelveEta * contactGFaCoeff * contactGFaCoeff;
        double bigScreeningL5 = twentyFourEta * contactGBase * contactGFaCoeff - 2.0 * closureLinearFaCoeff - screeningSq;
        double bigScreeningL4 = twelveEta * contactGBase * contactGBase - 2.0 * closureLinearBase;
        /* NB: the reference literally has W56 = M5*L6 - L5*L6 (both terms
         * use L6, not L5 in the second product) -- reproduced exactly as
         * written in jscatter's _sqfun for this branch.                 */
        double bigScreeningElimW56 = bigScreeningM5 * bigScreeningL6 - bigScreeningL5 * bigScreeningL6;
        double bigScreeningElimW46 = bigScreeningM4 * bigScreeningL6 - bigScreeningL4 * bigScreeningM6;

        double coshCoeffSolved = -bigScreeningElimW46 / bigScreeningElimW56;
        double sinhCoeffSolved = -coshCoeffSolved;
        double contactValueSolved = -(contactGBase + contactGFaCoeff * coshCoeffSolved);

        solution->closureCoshCoeff = coshCoeffSolved;
        solution->closureSinhCoeff = sinhCoeffSolved;
        solution->closureLinearCoeff = closureLinearBase + closureLinearFaCoeff * coshCoeffSolved;
        solution->closureConstantCoeff = closureConstantBase + closureConstantFaCoeff * coshCoeffSolved;
        solution->laplaceHAtZero = auxVBase + auxVFaCoeff * coshCoeffSolved;
        solution->contactValue = contactValueSolved;
        solution->rescaledVolumeFraction = trialVolumeFraction;
        solution->rescaledContactPotential = rescaledContactPotential;
        solution->rescaledScreeningParam = rescaledScreeningParam;
        solution->rescalingFactor = rescalingFactor;
        solution->numCandidateRoots = 0;      /* this branch bypasses the quartic search entirely */
        solution->physicalRootResidual = NAN; /* -- not applicable */
        solution->laplaceHAtScreening =
            screeningParam * sinhCoeffSolved / twentyFourEta / contactPotential * exp(-screeningParam) - 1.0 / screeningSq;
        *residualOut = (contactValueSolved > 1e-3) ? contactValueSolved : 0.0;
        return 0;
    }

    double sinhScreening = sinh(screeningParam);
    double coshScreening = cosh(screeningParam);
    double coshMinusOneMinusScreeningSinh = coshScreening - 1.0 - screeningParam * sinhScreening;
    double sinhMinusScreeningCosh = sinhScreening - screeningParam * coshScreening;

    closureConstantCaCoeff = twentyFourEta * (alpha3 * sinhMinusScreeningCosh + alpha2 * sinhScreening - alpha1 * coshScreening) * invOneMinusEtaQuad;
    closureConstantFaCoeff = twentyFourEta * (twoEtaPlusOneSq * invScreeningSq - 0.5 * oneMinusEtaSq
                              + alpha3 * coshMinusOneMinusScreeningSinh - alpha1 * sinhScreening + alpha2 * coshScreening) * invOneMinusEtaQuad;

    closureLinearCaCoeff = twelveEta * (-beta3 * sinhMinusScreeningCosh - beta2 * sinhScreening + beta1 * coshScreening) * invOneMinusEtaQuad;
    closureLinearFaCoeff = twelveEta * (0.5 * oneMinusEtaSq * etaPlusTwo - threeEta * etaPlusTwoSq * invScreeningSq
                            - beta3 * coshMinusOneMinusScreeningSinh + beta1 * sinhScreening - beta2 * coshScreening) * invOneMinusEtaQuad;

    auxVCaCoeff = (nu4 * coshScreening - nu5 * sinhScreening) * invOneMinusEtaQuadFifth;
    auxVFaCoeff = ((etaCubed - 6.0 * etaSq + 5.0) * oneMinusEta
                   - sixEta * (2.0 * etaCubed - 3.0 * etaSq + 18.0 * volumeFraction + 10.0) * invScreeningSq
                   + twentyFourEta * nu3 + nu4 * sinhScreening - nu5 * coshScreening) * invOneMinusEtaQuadFifth;

    contactGCaCoeff = (phiSqSum * sinhScreening + twicePhi1Phi2 * coshScreening) * invOneMinusEtaSq;
    contactGFaCoeff = (phiSqSum * coshScreening + twicePhi1Phi2 * sinhScreening + phi1Sq - phi2Sq) * invOneMinusEtaSq;

    constraintCaCoeff = tau4 * closureConstantCaCoeff + tau5 * closureLinearCaCoeff + twelveEta * (tau1 * coshScreening - tau2 * sinhScreening);
    constraintFaCoeff = tau4 * closureConstantFaCoeff + tau5 * closureLinearFaCoeff
                         + twelveEta * (tau1 * sinhScreening - tau2 * (coshScreening - 1.0))
                         - 0.4 * volumeFraction * (volumeFraction + 10.0) - 1.0;

    firstConstraintM1 = constraintCaCoeff * closureConstantCaCoeff - twelveEta * auxVCaCoeff * auxVCaCoeff;
    firstConstraintM2 = constraintBase * closureConstantCaCoeff + constraintCaCoeff * closureConstantBase - twentyFourEta * auxVBase * auxVCaCoeff;
    firstConstraintM3 = constraintCaCoeff * closureConstantFaCoeff + constraintFaCoeff * closureConstantCaCoeff - twentyFourEta * auxVCaCoeff * auxVFaCoeff;
    firstConstraintM4 = constraintBase * closureConstantBase - twelveEta * auxVBase * auxVBase;
    firstConstraintM5 = constraintBase * closureConstantFaCoeff + constraintFaCoeff * closureConstantBase - twentyFourEta * auxVBase * auxVFaCoeff;
    firstConstraintM6 = constraintFaCoeff * closureConstantFaCoeff - twelveEta * auxVFaCoeff * auxVFaCoeff;

    if (mode == SQFUN_FULL_SOLVE) {
        /* second constraint (the contact/Gillan condition) as a
         * quadratic form in (ca, fa), analogous to the M's above.      */
        double secondConstraintL1 = twelveEta * contactGCaCoeff * contactGCaCoeff;
        double secondConstraintL2 = twentyFourEta * contactGBase * contactGCaCoeff - 2.0 * closureLinearCaCoeff;
        double secondConstraintL3 = twentyFourEta * contactGCaCoeff * contactGFaCoeff;
        double secondConstraintL4 = twelveEta * contactGBase * contactGBase - 2.0 * closureLinearBase;
        double secondConstraintL5 = twentyFourEta * contactGBase * contactGFaCoeff - 2.0 * closureLinearFaCoeff - screeningSq;
        double secondConstraintL6 = twelveEta * contactGFaCoeff * contactGFaCoeff;

        /* eliminate "ca" between the two quadratic constraints (classic
         * resultant-style cross terms) to get a quartic in "fa" alone. */
        double elimW16 = firstConstraintM1 * secondConstraintL6 - secondConstraintL1 * firstConstraintM6;
        double elimW15 = firstConstraintM1 * secondConstraintL5 - secondConstraintL1 * firstConstraintM5;
        double elimW14 = firstConstraintM1 * secondConstraintL4 - secondConstraintL1 * firstConstraintM4;
        double elimW13 = firstConstraintM1 * secondConstraintL3 - secondConstraintL1 * firstConstraintM3;
        double elimW12 = firstConstraintM1 * secondConstraintL2 - secondConstraintL1 * firstConstraintM2;
        double elimW26 = firstConstraintM2 * secondConstraintL6 - secondConstraintL2 * firstConstraintM6;
        double elimW25 = firstConstraintM2 * secondConstraintL5 - secondConstraintL2 * firstConstraintM5;
        double elimW24 = firstConstraintM2 * secondConstraintL4 - secondConstraintL2 * firstConstraintM4;
        double elimW36 = firstConstraintM3 * secondConstraintL6 - secondConstraintL3 * firstConstraintM6;
        double elimW35 = firstConstraintM3 * secondConstraintL5 - secondConstraintL3 * firstConstraintM5;
        double elimW34 = firstConstraintM3 * secondConstraintL4 - secondConstraintL3 * firstConstraintM4;

        double quarticCoeff4 = elimW16 * elimW16 - elimW13 * elimW36;
        double quarticCoeff3 = 2.0 * elimW16 * elimW15 - elimW13 * (elimW35 + elimW26) - elimW12 * elimW36;
        double quarticCoeff2 = elimW15 * elimW15 + 2.0 * elimW16 * elimW14 - elimW13 * (elimW34 + elimW25) - elimW12 * (elimW35 + elimW26);
        double quarticCoeff1 = 2.0 * elimW15 * elimW14 - elimW13 * elimW24 - elimW12 * (elimW34 + elimW25);
        double quarticCoeff0 = elimW14 * elimW14 - elimW12 * elimW24;

        double complex quarticRoots[4];
        if (quartic_roots(quarticCoeff4, quarticCoeff3, quarticCoeff2, quarticCoeff1, quarticCoeff0, quarticRoots) != 0) {
            /* Degenerate coefficients (see polyroots.c) -- treat the same
             * as "no physical root found" rather than propagating a
             * separate error code, since both mean the same thing to a
             * caller: this parameter combination could not be solved.   */
            solution->contactValue = -1.0;
            solution->numCandidateRoots = 0;
            solution->physicalRootResidual = NAN;
            return -4;
        }

        /* candidate real (or near-real) roots for "fa", following the
         * same fallback logic as the reference's numpy.roots+isreal:
         * if every root's imaginary part is small relative to its real
         * part, keep all four real parts as candidates; otherwise keep
         * only the (near-)genuinely-real ones.                         */
        int allRootsNearlyReal = 1;
        for (int i = 0; i < 4; i++) {
            double imagToRealRatio = cimag(quarticRoots[i]) / creal(quarticRoots[i]);
            if (!(imagToRealRatio < 1e-3)) { allRootsNearlyReal = 0; break; }
        }

        double realCandidates[4];
        int numRealCandidates = 0;
        if (allRootsNearlyReal) {
            for (int i = 0; i < 4; i++) realCandidates[numRealCandidates++] = creal(quarticRoots[i]);
        } else {
            for (int i = 0; i < 4; i++)
                if (isNegligibleImaginaryPart(quarticRoots[i])) realCandidates[numRealCandidates++] = creal(quarticRoots[i]);
        }
        qsort(realCandidates, numRealCandidates, sizeof(double), compareDoubles);

        if (numRealCandidates == 0) {
            solution->contactValue = -1.0; /* flag: no candidates at all */
            solution->numCandidateRoots = 0;
            solution->physicalRootResidual = NAN;
            return -4;
        }

        /* Physical-root selection: for each candidate fa, evaluate g(r)
         * at r/sigma = 1-radialStep via the direct sine-transform sum
         * of Hayter & Penfold eq. (12), and keep whichever candidate
         * gives a value closest to the required g(1+) = 0 (i.e. no
         * discontinuity/negative dip just inside the hard core).        */
        double bestCoshCoeff = 0, bestSinhCoeff = 0, bestContactValue = 0, bestPairCorrelationNearContact = HUGE_VAL;
        double candidateResiduals[4]; /* |g(r)| for every candidate, not just the winner -- used below to detect near-degenerate root selection */
        const double radialStep = GR_TEST_RADIAL_STEP;
        const int numSineTerms = GR_TEST_NUM_POINTS;
        const int contactSineIndex = GR_TEST_CONTACT_SINE_INDEX; /* == floor(1.0/radialStep) as evaluated in double arithmetic by the reference */
        ensureGRTestTrigTableInitialized();

        for (int i = 0; i < numRealCandidates; i++) {
            double coshCoeffTrial = realCandidates[i];
            double sinhCoeffTrial = -(elimW16 * coshCoeffTrial * coshCoeffTrial + elimW15 * coshCoeffTrial + elimW14)
                                     / (elimW13 * coshCoeffTrial + elimW12);
            double closureLinearTrial = closureLinearBase + closureLinearCaCoeff * sinhCoeffTrial + closureLinearFaCoeff * coshCoeffTrial;
            double closureConstantTrial = closureConstantBase + closureConstantCaCoeff * sinhCoeffTrial + closureConstantFaCoeff * coshCoeffTrial;

            /* n==0 (radius=0) is skipped here exactly as
             * structureFactorFromCoefficients() itself special-cases
             * qSigma==0 (dividing by K==0 otherwise); its contribution
             * to the n==0 term is -1/closureConstantTrial times
             * radius*sin(...)=0*0=0 regardless, i.e. exactly zero, so
             * skipping it changes nothing -- confirmed by
             * test_gr_trig_table.c's bit-for-bit comparison against the
             * original loop, not just this reasoning.
             *
             * OpenMP: each iteration only reads shared, read-only state
             * (gGRTestTrigTable, the *Trial scalars fixed for this
             * candidate) and writes to sineTransformSum via the
             * reduction clause -- no loop-carried dependency, so this
             * is safe to parallelize as-is. #ifdef _OPENMP guards the
             * pragma itself (not just omp_get_* calls) because an
             * unguarded #pragma omp is otherwise silently accepted but
             * triggers -Wunknown-pragmas under -Wall when built
             * without -fopenmp, which this project's Makefile treats
             * as a clean-build regression -- confirmed directly (see
             * README.md's "OpenMP" section). Only active at all when
             * SASfit's own SASFIT_WITH_OPENMP option is on (off by
             * default, matching current behavior exactly) -- see this
             * plugin's CMakeLists.txt.
             *
             * Floating-point reduction order is NOT the same as the
             * sequential loop's (OpenMP's reduction combines partial
             * per-thread sums in an unspecified, generally
             * thread-count-and-scheduling-dependent order, unlike
             * addition in strict left-to-right sequence) -- so results
             * with OpenMP enabled are consistent with, but not
             * bit-for-bit identical to, the sequential path. This
             * project's development sandbox has only a single CPU
             * core, so genuine multi-thread reduction non-determinism
             * could not be observed directly there; as an honest proxy,
             * summing the same ~8191 terms sequentially vs. split into
             * 8 chunks (each summed independently, then combined --
             * exactly what an 8-thread OpenMP reduction does
             * internally) gave a worst relative difference of 1.04e-14
             * across a 495-case randomized sweep (see
             * test_openmp_consistency.c) -- ordinary floating-point-
             * summation-order noise, many orders of magnitude below the
             * 0.1 acceptance threshold and the warning thresholds in
             * RMSAWarningFlags -- this can, in
             * principle, still flip a decision only in a case already
             * flagged RMSA_WARN_NEAR_DEGENERATE_ROOT (candidates within
             * noise of each other to begin with), the same caveat that
             * already exists for e.g. compiler/platform differences.  */
            double sineTransformSum = 0.0;
            #ifdef _OPENMP
            #pragma omp parallel for reduction(+:sineTransformSum) if(numSineTerms >= 2048)
            #endif
            for (int n = 1; n < numSineTerms; n++) {
                double radius = n * radialStep;
                double structureFactorMinusOne =
                    generalStructureFactorPrecomputedTrig(radius, gGRTestTrigTable.sinK[n], gGRTestTrigTable.cosK[n],
                                                           volumeFraction, screeningParam, contactPotential,
                                                           closureConstantTrial, closureLinearTrial,
                                                           sinhCoeffTrial, coshCoeffTrial, sinhScreening, coshScreening) - 1.0;
                sineTransformSum += structureFactorMinusOne * radius * gGRTestTrigTable.sinWeight[n];
            }
            sineTransformSum *= radialStep;
            double pairCorrelationNearContact = 1.0 + sineTransformSum / (12.0 * M_PI * volumeFraction * contactSineIndex * radialStep);
            candidateResiduals[i] = fabs(pairCorrelationNearContact);

            if (fabs(pairCorrelationNearContact) < fabs(bestPairCorrelationNearContact)) {
                bestPairCorrelationNearContact = pairCorrelationNearContact;
                bestCoshCoeff = coshCoeffTrial;
                bestSinhCoeff = sinhCoeffTrial;
                bestContactValue = -(contactGBase + contactGCaCoeff * sinhCoeffTrial + contactGFaCoeff * coshCoeffTrial);
            }
        }

        solution->numCandidateRoots = numRealCandidates;
        solution->physicalRootResidual = fabs(bestPairCorrelationNearContact);

        if (bestPairCorrelationNearContact > 0.1) {
            solution->contactValue = bestContactValue;
            return -4;
        }

        /* Near-degenerate root selection: more than one candidate was
         * independently below the same 0.1 acceptance threshold used to
         * decide "physical" above -- i.e. the choice between them was a
         * genuine judgment call, not a clear winner among also-rans.
         * (An earlier version of this check compared candidates by the
         * *ratio* of their residuals instead, which fired even on the
         * textbook Fig. 1 case from Hayter & Penfold's own paper: that
         * case's runner-up candidate turned out, on direct check, not
         * to be a competing solution at all -- perturbing the input by
         * 1e-9 relative moved the accepted solution by a correspondingly
         * infinitesimal amount, showing no real ambiguity was present.
         * Tying the check to the same threshold the algorithm itself
         * uses to accept a root avoids that false positive.)           */
        solution->warnings = 0;
        int numCandidatesBelowThreshold = 0;
        for (int i = 0; i < numRealCandidates; i++)
            if (candidateResiduals[i] <= 0.1) numCandidatesBelowThreshold++;
        if (numCandidatesBelowThreshold > 1)
            solution->warnings |= RMSA_WARN_NEAR_DEGENERATE_ROOT;
        if (solution->physicalRootResidual > 0.01)
            solution->warnings |= RMSA_WARN_MARGINAL_PHYSICAL_ROOT;

        double coshCoeffSolved = bestCoshCoeff, sinhCoeffSolved = bestSinhCoeff;
        double contactValueSolved = -(contactGBase + contactGCaCoeff * sinhCoeffSolved + contactGFaCoeff * coshCoeffSolved);
        double closureLinearSolved = closureLinearBase + closureLinearCaCoeff * sinhCoeffSolved + closureLinearFaCoeff * coshCoeffSolved;
        double closureConstantSolved = closureConstantBase + closureConstantCaCoeff * sinhCoeffSolved + closureConstantFaCoeff * coshCoeffSolved;
        double auxVSolved = (auxVBase + auxVCaCoeff * sinhCoeffSolved + auxVFaCoeff * coshCoeffSolved) / closureConstantSolved;

        solution->closureCoshCoeff = coshCoeffSolved;
        solution->closureSinhCoeff = sinhCoeffSolved;
        solution->closureLinearCoeff = closureLinearSolved;
        solution->closureConstantCoeff = closureConstantSolved;
        solution->laplaceHAtZero = auxVSolved;
        solution->contactValue = contactValueSolved;
        solution->rescaledVolumeFraction = trialVolumeFraction;
        *residualOut = (fabs(contactValueSolved) > 1e-3) ? contactValueSolved : 0.0;
    } else {
        /* SQFUN_NEWTON_RESIDUAL or SQFUN_FINAL_RESCALED: here ca and fa
         * are obtained directly (no root search) from the two linear
         * relations that follow from requiring the contact condition
         * P = 0 together with dP/d(ca) = 0 (i.e. assuming we're at, or
         * very near, the Gillan condition already).                    */
        double sinhCoeffTrial = screeningSq * contactGBase + 2.0 * (closureLinearFaCoeff * contactGBase - closureLinearBase * contactGFaCoeff);
        sinhCoeffTrial = -sinhCoeffTrial / (screeningSq * contactGCaCoeff + 2.0 * (closureLinearFaCoeff * contactGCaCoeff - closureLinearCaCoeff * contactGFaCoeff));
        double coshCoeffTrial = -(contactGBase + contactGCaCoeff * sinhCoeffTrial) / contactGFaCoeff;

        if (mode == SQFUN_NEWTON_RESIDUAL)
            *residualOut = firstConstraintM1 * sinhCoeffTrial * sinhCoeffTrial
                         + (firstConstraintM2 + firstConstraintM3 * coshCoeffTrial) * sinhCoeffTrial
                         + firstConstraintM4 + firstConstraintM5 * coshCoeffTrial + firstConstraintM6 * coshCoeffTrial * coshCoeffTrial;
        if (mode == SQFUN_FINAL_RESCALED)
            *residualOut = -(contactGBase + contactGCaCoeff * sinhCoeffTrial + contactGFaCoeff * coshCoeffTrial);

        solution->closureCoshCoeff = coshCoeffTrial;
        solution->closureSinhCoeff = sinhCoeffTrial;
        solution->closureLinearCoeff = closureLinearBase + closureLinearCaCoeff * sinhCoeffTrial + closureLinearFaCoeff * coshCoeffTrial;
        solution->closureConstantCoeff = closureConstantBase + closureConstantCaCoeff * sinhCoeffTrial + closureConstantFaCoeff * coshCoeffTrial;
        solution->laplaceHAtZero = (auxVBase + auxVCaCoeff * sinhCoeffTrial + auxVFaCoeff * coshCoeffTrial) / solution->closureConstantCoeff;
        /* rescaledVolumeFraction is intentionally left untouched here,
         * matching the reference (only SQFUN_FULL_SOLVE sets it); the
         * caller (rmsa_solve) tracks the rescaled eta itself across the
         * Newton/secant iterations.                                    */
    }

    solution->rescaledContactPotential = rescaledContactPotential;
    solution->rescaledScreeningParam = rescaledScreeningParam;
    solution->rescalingFactor = rescalingFactor;
    double screeningExpTerm = twentyFourEta * contactPotential * exp(screeningParam);
    solution->laplaceHAtScreening =
        (screeningSq * screeningParam * solution->closureSinhCoeff - screeningExpTerm) / (screeningSq * screeningExpTerm);
    return 0;
}

int rmsa_solve(double volumeFraction, double contactPotential, double screeningParam, RMSASolution *solution)
{
    const int maxRescaleIterations = 80;
    const double convergenceTolerance = 5e-6;

    solution->volumeFraction = volumeFraction;
    solution->contactPotential = contactPotential;
    solution->screeningParam = screeningParam;
    solution->couplingConstant = 2.0 * cbrt(volumeFraction) * contactPotential
                                  * exp(screeningParam - screeningParam / cbrt(volumeFraction));
    solution->warnings = RMSA_WARN_NONE;
    solution->numCandidateRoots = 0;
    solution->physicalRootResidual = NAN;

    double residual;
    int solveStatus = sqfun(SQFUN_FULL_SOLVE, volumeFraction, volumeFraction, contactPotential, screeningParam,
                             solution, &residual);
    if (solveStatus == -4) { solution->rescaleIterations = -4; return -4; }

    double contactValue = solution->contactValue; /* the *true* g(1+) from the full solve above */
    if (contactValue >= 0.0) {
        solution->rescaleIterations = 0;
        if (solution->rescaledScreeningParam >= 10.0 && solution->rescaledScreeningParam <= 15.0)
            solution->warnings |= RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC;
        return 0;
    }

    /* need to rescale: Newton/secant search for eta(Gillan) giving g(1+) == 0 */
    double trialRescaledVolumeFraction = fmin(volumeFraction, 0.2);
    int iterationCount = 0;
    double newTrialVolumeFraction = trialRescaledVolumeFraction;
    while (1) {
        iterationCount++;
        if (iterationCount > maxRescaleIterations) { solution->rescaleIterations = -1; return -1; }
        if (trialRescaledVolumeFraction <= 0.0) trialRescaledVolumeFraction = volumeFraction / iterationCount;
        if (trialRescaledVolumeFraction > 0.6) trialRescaledVolumeFraction = 0.35 / iterationCount;
        double currentTrialVolumeFraction = trialRescaledVolumeFraction;

        double residualAtCurrent;
        sqfun(SQFUN_NEWTON_RESIDUAL, currentTrialVolumeFraction, volumeFraction, contactPotential, screeningParam,
              solution, &residualAtCurrent);

        double perturbedTrialVolumeFraction = trialRescaledVolumeFraction * 1.01;
        double residualAtPerturbed;
        sqfun(SQFUN_NEWTON_RESIDUAL, perturbedTrialVolumeFraction, volumeFraction, contactPotential, screeningParam,
              solution, &residualAtPerturbed);

        newTrialVolumeFraction = currentTrialVolumeFraction
            - (perturbedTrialVolumeFraction - currentTrialVolumeFraction) * residualAtCurrent
              / (residualAtPerturbed - residualAtCurrent);
        trialRescaledVolumeFraction = newTrialVolumeFraction;
        double relativeChange = fabs((newTrialVolumeFraction - currentTrialVolumeFraction) / currentTrialVolumeFraction);
        if (relativeChange < convergenceTolerance) break;
    }

    double finalResidual;
    sqfun(SQFUN_FINAL_RESCALED, newTrialVolumeFraction, volumeFraction, contactPotential, screeningParam,
          solution, &finalResidual);
    solution->rescaledVolumeFraction = newTrialVolumeFraction;
    solution->contactValue = finalResidual; /* the new g(1+) at eta = eta(Gillan) */

    if (solution->rescaledVolumeFraction > 0.64 || solution->rescaledVolumeFraction < volumeFraction) {
        solution->rescaleIterations = -3;
        return -3;
    }
    if (solution->rescaledScreeningParam >= 10.0 && solution->rescaledScreeningParam <= 15.0)
        solution->warnings |= RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC;
    solution->rescaleIterations = iterationCount;
    return iterationCount;
}

/* Attempts a warm-started solve for (volumeFraction, contactPotential,
 * screeningParam), Newton-refining from a previous solve's
 * closureCoshCoeff ("fa") root instead of doing the full quartic-root
 * search and testing every candidate against the g(r) acceptance
 * criterion -- see rmsa_compute_cached()'s doc comment and README.md's
 * "Performance" section for why and how much this can save (roughly
 * 2-4x for the "small perturbation" pattern, since one still-necessary
 * g(r) test dominates either way -- see the derivation there for why
 * this is NOT expected to give the 1700x+ speedup a same-parameters
 * cache hit does).
 *
 * Deliberately conservative in scope, falling back (returning 0, i.e.
 * "not warm-started, caller should cold-start via rmsa_solve()")
 * whenever there is any doubt:
 *   - screeningParam > 15 (the asymptotic-shortcut branch; this
 *     function only handles the general quartic branch)
 *   - Newton fails to converge in 50 iterations
 *   - the refined root's g(r) residual exceeds the same 0.1 acceptance
 *     threshold sqfun()'s cold-start path uses
 *   - the resulting contactValue would be negative (needs rescaling;
 *     out of scope here -- the rescaling search's own trial-eta values
 *     are a different thing than a small physical-parameter
 *     perturbation, and reusing a cached root across them hasn't been
 *     validated, so cold-start handles rescaling unconditionally)
 * On success, fills *solution equivalently to sqfun()'s
 * SQFUN_FULL_SOLVE branch and rmsa_solve()'s subsequent field-setting,
 * and returns 1.                                                      */
static int attemptWarmStartedSolve(double volumeFraction, double contactPotential, double screeningParam,
                                    double previousCoshCoeff, RMSASolution *solution)
{
    if (screeningParam > 15.0) return 0;

    QuarticSetup setup;
    computeQuarticSetup(volumeFraction, screeningParam, contactPotential, &setup);

    /* Newton iteration on p(fa)=0 seeded from the previous solve's root.
     * Converges on RELATIVE STEP SIZE (checked *after* stepping), not
     * on a naively-scaled function value checked *before* stepping:
     * an earlier version checked |p(fa)| < tol*max(1,|quarticCoeff4*fa^4|)
     * *before* taking any step, which is unsound whenever |fa| is huge
     * (as happens for small screeningParam -- fa in the millions makes
     * fa^4 astronomically large, so almost any residual looks "small"
     * by comparison, falsely declaring convergence on iteration 0
     * without ever refining fa toward the new problem's actual root).
     * Found via a 500-case randomized sweep, not the smaller targeted
     * tests -- see test_warm_start.c and README.md's "Performance"
     * section for the full story.                                     */
    double fa = previousCoshCoeff;
    int converged = 0;
    for (int iter = 0; iter < 100; iter++) {
        double fa2 = fa * fa, fa3 = fa2 * fa, fa4 = fa3 * fa;
        double p = setup.quarticCoeff4 * fa4 + setup.quarticCoeff3 * fa3 + setup.quarticCoeff2 * fa2
                   + setup.quarticCoeff1 * fa + setup.quarticCoeff0;
        double dp = 4.0 * setup.quarticCoeff4 * fa3 + 3.0 * setup.quarticCoeff3 * fa2
                    + 2.0 * setup.quarticCoeff2 * fa + setup.quarticCoeff1;
        if (dp == 0.0) return 0;
        double step = p / dp;
        if (!isfinite(step)) return 0;
        double newFa = fa - step;
        if (!isfinite(newFa)) return 0;
        double relativeStepSize = fabs(step) / fmax(1.0, fabs(newFa));
        fa = newFa;
        if (relativeStepSize < 1e-13) { converged = 1; break; }
    }
    if (!converged) return 0;

    /* Reject an otherwise-"converged" root if |fa| is huge: this
     * signals the small-screeningParam regime already documented
     * elsewhere in this project (README.md's "Known Limitations",
     * test_taylor_series.c) as producing extremely large MSA
     * coefficients -- and, found via a 2000-case randomized sweep
     * here, severely ill-conditioned quartics (evaluating the quartic
     * at its own root can involve ~10^11-fold cancellation between the
     * leading w4*fa^4 term and the rest, leaving only a handful of
     * significant digits of double-precision accuracy in fa itself).
     * This is a *pre-existing* limitation of the underlying double-
     * precision quartic solve, affecting the ordinary cold-start path
     * (via GSL) exactly as much as this Newton refinement -- neither
     * is "more correct" than the other there, they simply land on
     * slightly different, similarly-imprecise points near the true
     * root. Since warm-starting can offer no reliable advantage in a
     * regime where its own validation (the g(r) test) can't be
     * trusted to that precision either, the safe choice is simply not
     * to attempt it there: falling back to cold-start preserves
     * today's already-established (if imprecise, in this rare regime)
     * behavior rather than risking a *different* imprecise answer.
     * 1e6 was chosen from a clean, sharp empirical cutoff: below it,
     * 2000 random cases agreed with cold-start to 3.8e-7 worst-case;
     * above it, some disagreed by as much as 1.1e-4. See
     * test_warm_start.c's broad sweep for the full story.             */
    if (fabs(fa) > 1e6) return 0;

    double ca = -(setup.elimW16 * fa * fa + setup.elimW15 * fa + setup.elimW14) / (setup.elimW13 * fa + setup.elimW12);
    if (!isfinite(ca)) return 0;
    double closureLinearTrial = setup.closureLinearBase + setup.closureLinearCaCoeff * ca + setup.closureLinearFaCoeff * fa;
    double closureConstantTrial = setup.closureConstantBase + setup.closureConstantCaCoeff * ca + setup.closureConstantFaCoeff * fa;

    const double radialStep = GR_TEST_RADIAL_STEP;
    const int numSineTerms = GR_TEST_NUM_POINTS;
    const int contactSineIndex = GR_TEST_CONTACT_SINE_INDEX;
    ensureGRTestTrigTableInitialized();
    double sinhScreening = sinh(screeningParam), coshScreening = cosh(screeningParam); /* loop-invariant, hoisted */
    double sineTransformSum = 0.0;
    /* OpenMP: same reasoning as the identical loop in sqfun()'s
     * candidate-selection branch above -- see that comment for the
     * full explanation (embarrassingly parallel, guard needed to avoid
     * -Wunknown-pragmas without -fopenmp, and the floating-point
     * reduction-order caveat). Only active when SASfit's
     * SASFIT_WITH_OPENMP option is on.                                */
    #ifdef _OPENMP
    #pragma omp parallel for reduction(+:sineTransformSum) if(numSineTerms >= 2048)
    #endif
    for (int n = 1; n < numSineTerms; n++) { /* n==0 skipped -- see the identical comment in sqfun()'s candidate loop */
        double radius = n * radialStep;
        double structureFactorMinusOne =
            generalStructureFactorPrecomputedTrig(radius, gGRTestTrigTable.sinK[n], gGRTestTrigTable.cosK[n],
                                                   volumeFraction, screeningParam, contactPotential,
                                                   closureConstantTrial, closureLinearTrial, ca, fa,
                                                   sinhScreening, coshScreening) - 1.0;
        sineTransformSum += structureFactorMinusOne * radius * gGRTestTrigTable.sinWeight[n];
    }
    sineTransformSum *= radialStep;
    double pairCorrelationNearContact = 1.0 + sineTransformSum / (12.0 * M_PI * volumeFraction * contactSineIndex * radialStep);
    if (fabs(pairCorrelationNearContact) > 0.1) return 0;

    double contactValueSolved = -(setup.contactGBase + setup.contactGCaCoeff * ca + setup.contactGFaCoeff * fa);
    if (contactValueSolved < 0.0) return 0; /* would need rescaling -- out of scope, see doc comment */

    double auxVSolved = (setup.auxVBase + setup.auxVCaCoeff * ca + setup.auxVFaCoeff * fa) / closureConstantTrial;

    solution->volumeFraction = volumeFraction;
    solution->contactPotential = contactPotential;
    solution->screeningParam = screeningParam;
    solution->couplingConstant = 2.0 * cbrt(volumeFraction) * contactPotential
                                  * exp(screeningParam - screeningParam / cbrt(volumeFraction));
    solution->closureCoshCoeff = fa;
    solution->closureSinhCoeff = ca;
    solution->closureLinearCoeff = closureLinearTrial;
    solution->closureConstantCoeff = closureConstantTrial;
    solution->laplaceHAtZero = auxVSolved;
    solution->contactValue = contactValueSolved;
    solution->rescaledVolumeFraction = volumeFraction;
    solution->rescaledContactPotential = contactPotential;
    solution->rescaledScreeningParam = screeningParam;
    solution->rescalingFactor = 1.0;
    solution->laplaceHAtScreening =
        screeningParam * ca / (24.0 * volumeFraction) / contactPotential * exp(-screeningParam)
        - 1.0 / (screeningParam * screeningParam);
    solution->numCandidateRoots = 1; /* only the warm-started root was checked, not all quartic roots */
    solution->physicalRootResidual = fabs(pairCorrelationNearContact);
    solution->warnings = RMSA_WARN_NONE; /* near-degenerate-root diagnostic is N/A: only one candidate tested */
    if (solution->physicalRootResidual > 0.01) solution->warnings |= RMSA_WARN_MARGINAL_PHYSICAL_ROOT;
    if (screeningParam >= 10.0) solution->warnings |= RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC;
    solution->rescaleIterations = 0;
    return 1;
}

int rmsa_compute(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                  const double *q, double *structureFactorOut, int numPoints, RMSASolution *solution)
{
    radius = fabs(radius);
    if (volumeFraction <= 0.0) volumeFraction = 1e-10;

    double screeningParam;
    if (screeningLength <= 0.0) screeningParam = 1e20;
    else screeningParam = 2.0 * radius / screeningLength;
    if (screeningParam > 200.0) screeningParam = 200.0;

    double contactPotential = chargeParameter * exp(-screeningParam);

    int solveStatus = rmsa_solve(volumeFraction, contactPotential, screeningParam, solution);
    if (solveStatus < 0) return solveStatus;

    for (int i = 0; i < numPoints; i++) {
        double qSigma = 2.0 * radius * q[i];
        structureFactorOut[i] = rmsa_structure_factor(solution, qSigma);
    }
    return solveStatus;
}

int rmsa_compute_cached(double radius, double screeningLength, double chargeParameter, double volumeFraction,
                         const double *q, double *structureFactorOut, int numPoints,
                         RMSACache *cache, RMSASolution *solution)
{
    /* NaN-safe: NaN != NaN would otherwise always miss the cache and
     * silently re-solve every time rather than failing loudly, which
     * would defeat the point without ever being noticed.              */
    int sameAsLastTime = cache->hasSolved
        && cache->lastRadius == radius
        && cache->lastScreeningLength == screeningLength
        && cache->lastChargeParameter == chargeParameter
        && cache->lastVolumeFraction == volumeFraction;

    if (!sameAsLastTime) {
        /* Try a warm-started solve first if we have a usable previous
         * one to Newton-refine from: cheap to attempt (a handful of
         * scalar Newton iterations plus computeQuarticSetup(), all
         * O(1)), and attemptWarmStartedSolve() itself falls back
         * (returns 0) at the first sign of trouble -- see its doc
         * comment for the exact conditions. Restricted to a previous
         * solve that didn't need rescaling (rescaleIterations==0):
         * a rescaled solve's closureCoshCoeff comes from the *final*
         * rescaled state, not the initial quartic search, and isn't
         * validated as a warm-start seed for a *different* physical
         * parameter perturbation -- see attemptWarmStartedSolve()'s
         * doc comment.                                                */
        int warmStarted = 0;
        if (cache->hasSolved && cache->solution.rescaleIterations == 0) {
            double radiusAbs = fabs(radius);
            double newVolumeFraction = (volumeFraction <= 0.0) ? 1e-10 : volumeFraction;
            double newScreeningParam = (screeningLength <= 0.0) ? 1e20 : 2.0 * radiusAbs / screeningLength;
            if (newScreeningParam > 200.0) newScreeningParam = 200.0;
            double newContactPotential = chargeParameter * exp(-newScreeningParam);

            warmStarted = attemptWarmStartedSolve(newVolumeFraction, newContactPotential, newScreeningParam,
                                                   cache->solution.closureCoshCoeff, &cache->solution);
            if (warmStarted) {
                for (int i = 0; i < numPoints; i++) {
                    double qSigma = 2.0 * radiusAbs * q[i];
                    structureFactorOut[i] = rmsa_structure_factor(&cache->solution, qSigma);
                }
            }
        }

        int status;
        if (warmStarted) {
            status = cache->solution.rescaleIterations;
        } else {
            status = rmsa_compute(radius, screeningLength, chargeParameter, volumeFraction,
                                   q, structureFactorOut, numPoints, &cache->solution);
        }
        cache->hasSolved = (status >= 0);
        cache->lastRadius = radius;
        cache->lastScreeningLength = screeningLength;
        cache->lastChargeParameter = chargeParameter;
        cache->lastVolumeFraction = volumeFraction;
        if (solution) *solution = cache->solution;
        return status;
    }

    /* cache hit: skip straight to evaluating q against the solution
     * already sitting in cache->solution from the previous call.      */
    double radiusAbs = fabs(radius);
    for (int i = 0; i < numPoints; i++) {
        double qSigma = 2.0 * radiusAbs * q[i];
        structureFactorOut[i] = rmsa_structure_factor(&cache->solution, qSigma);
    }
    if (solution) *solution = cache->solution;
    return cache->solution.rescaleIterations;
}
