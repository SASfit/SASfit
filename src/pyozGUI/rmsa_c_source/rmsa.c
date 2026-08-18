#include "include/rmsa.h"
#include "include/polyroots.h"
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef enum {
    SQFUN_FULL_SOLVE       = 1,
    SQFUN_NEWTON_RESIDUAL  = 2,
    SQFUN_FINAL_RESCALED   = 4
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
    return fabs(K) < 0.02;
}

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

double rmsa_structure_factor_derivative(const RMSASolution *solution, double K)
{
    if (K == 0.0) return 0.0;

    double reducedWavevector = K / solution->rescalingFactor;
    double sinhScreening, coshScreening;
    if (solution->rescaledScreeningParam > 25.0) { sinhScreening = 0.0; coshScreening = 0.0; }
    else { sinhScreening = sinh(solution->rescaledScreeningParam); coshScreening = cosh(solution->rescaledScreeningParam); }

    double dSdReducedWavevector;
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
    return dSdReducedWavevector / solution->rescalingFactor;
}

#define GR_TEST_NUM_POINTS 8192
#define GR_TEST_RADIAL_STEP 0.05
#define GR_TEST_CONTACT_SINE_INDEX 19

typedef struct {
    double sinK[GR_TEST_NUM_POINTS];
    double cosK[GR_TEST_NUM_POINTS];
    double sinWeight[GR_TEST_NUM_POINTS];
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

static void ensureGRTestTrigTableInitialized(void)
{
    pthread_once(&gGRTestTrigTableOnceGuard, initGRTestTrigTable);
}

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
    return structureFactorFromCoefficients(K, solution->rescalingFactor, solution->rescaledVolumeFraction,
                                            solution->rescaledScreeningParam, solution->rescaledContactPotential,
                                            solution->closureConstantCoeff, solution->closureLinearCoeff,
                                            solution->closureSinhCoeff, solution->closureCoshCoeff);
}

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

typedef struct {
    double quarticCoeff4, quarticCoeff3, quarticCoeff2, quarticCoeff1, quarticCoeff0;
    double elimW16, elimW15, elimW14, elimW13, elimW12;
    double closureConstantBase, closureConstantCaCoeff, closureConstantFaCoeff;
    double closureLinearBase, closureLinearCaCoeff, closureLinearFaCoeff;
    double auxVBase, auxVCaCoeff, auxVFaCoeff;
    double contactGBase, contactGCaCoeff, contactGFaCoeff;
} QuarticSetup;

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

    double closureConstantBase = (twentyFourEta * contactPotential * (alpha1 + alpha2 + onePlusScreening * alpha3) - twoEtaPlusOneSq) * invOneMinusEtaQuad;
    double closureLinearBase = (1.5 * volumeFraction * etaPlusTwoSq - 12.0 * volumeFraction * contactPotential * (beta1 + beta2 + onePlusScreening * beta3)) * invOneMinusEtaQuad;
    double auxVBase = (twoEtaPlusOne * (etaSq - 2.0 * volumeFraction + 10.0) * 0.25 - contactPotential * (nu4 + nu5)) * invOneMinusEtaQuadFifth;
    double contactGBase = (contactPotential * (phi1Sq + phi2Sq - twicePhi1Phi2) - 0.5 * etaPlusTwo) * invOneMinusEtaSq;
    double constraintBase = tau3 + tau4 * closureConstantBase + tau5 * closureLinearBase;

    double closureConstantCaCoeff, closureConstantFaCoeff;
    double closureLinearCaCoeff, closureLinearFaCoeff;
    double auxVCaCoeff, auxVFaCoeff;
    double contactGCaCoeff, contactGFaCoeff;
    double constraintCaCoeff, constraintFaCoeff;
    double firstConstraintM1, firstConstraintM2, firstConstraintM3;
    double firstConstraintM4, firstConstraintM5, firstConstraintM6;

    if (rescaledScreeningParam > 15.0 && mode == SQFUN_FULL_SOLVE) {
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
        solution->numCandidateRoots = 0;
        solution->physicalRootResidual = NAN;
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

        double quarticCoeff4 = elimW16 * elimW16 - elimW13 * elimW36;
        double quarticCoeff3 = 2.0 * elimW16 * elimW15 - elimW13 * (elimW35 + elimW26) - elimW12 * elimW36;
        double quarticCoeff2 = elimW15 * elimW15 + 2.0 * elimW16 * elimW14 - elimW13 * (elimW34 + elimW25) - elimW12 * (elimW35 + elimW26);
        double quarticCoeff1 = 2.0 * elimW15 * elimW14 - elimW13 * elimW24 - elimW12 * (elimW34 + elimW25);
        double quarticCoeff0 = elimW14 * elimW14 - elimW12 * elimW24;

        double complex quarticRoots[4];
        if (quartic_roots(quarticCoeff4, quarticCoeff3, quarticCoeff2, quarticCoeff1, quarticCoeff0, quarticRoots) != 0) {
            solution->contactValue = -1.0;
            solution->numCandidateRoots = 0;
            solution->physicalRootResidual = NAN;
            return -4;
        }

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
            solution->contactValue = -1.0;
            solution->numCandidateRoots = 0;
            solution->physicalRootResidual = NAN;
            return -4;
        }

        double bestCoshCoeff = 0, bestSinhCoeff = 0, bestContactValue = 0, bestPairCorrelationNearContact = HUGE_VAL;
        double candidateResiduals[4];
        const double radialStep = GR_TEST_RADIAL_STEP;
        const int numSineTerms = GR_TEST_NUM_POINTS;
        const int contactSineIndex = GR_TEST_CONTACT_SINE_INDEX;
        ensureGRTestTrigTableInitialized();

        for (int i = 0; i < numRealCandidates; i++) {
            double coshCoeffTrial = realCandidates[i];
            double sinhCoeffTrial = -(elimW16 * coshCoeffTrial * coshCoeffTrial + elimW15 * coshCoeffTrial + elimW14)
                                     / (elimW13 * coshCoeffTrial + elimW12);
            double closureLinearTrial = closureLinearBase + closureLinearCaCoeff * sinhCoeffTrial + closureLinearFaCoeff * coshCoeffTrial;
            double closureConstantTrial = closureConstantBase + closureConstantCaCoeff * sinhCoeffTrial + closureConstantFaCoeff * coshCoeffTrial;

            double sineTransformSum = 0.0;
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

    double contactValue = solution->contactValue;
    if (contactValue >= 0.0) {
        solution->rescaleIterations = 0;
        if (solution->rescaledScreeningParam >= 10.0 && solution->rescaledScreeningParam <= 15.0)
            solution->warnings |= RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC;
        return 0;
    }

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
    solution->contactValue = finalResidual;

    if (solution->rescaledVolumeFraction > 0.64 || solution->rescaledVolumeFraction < volumeFraction) {
        solution->rescaleIterations = -3;
        return -3;
    }
    if (solution->rescaledScreeningParam >= 10.0 && solution->rescaledScreeningParam <= 15.0)
        solution->warnings |= RMSA_WARN_SCREENING_NEAR_ASYMPTOTIC;
    solution->rescaleIterations = iterationCount;
    return iterationCount;
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
    int sameAsLastTime = cache->hasSolved
        && cache->lastRadius == radius
        && cache->lastScreeningLength == screeningLength
        && cache->lastChargeParameter == chargeParameter
        && cache->lastVolumeFraction == volumeFraction;

    if (!sameAsLastTime) {
        int status = rmsa_compute(radius, screeningLength, chargeParameter, volumeFraction,
                                   q, structureFactorOut, numPoints, &cache->solution);
        cache->hasSolved = (status >= 0);
        cache->lastRadius = radius;
        cache->lastScreeningLength = screeningLength;
        cache->lastChargeParameter = chargeParameter;
        cache->lastVolumeFraction = volumeFraction;
        if (solution) *solution = cache->solution;
        return status;
    }

    double radiusAbs = fabs(radius);
    for (int i = 0; i < numPoints; i++) {
        double qSigma = 2.0 * radiusAbs * q[i];
        structureFactorOut[i] = rmsa_structure_factor(&cache->solution, qSigma);
    }
    if (solution) *solution = cache->solution;
    return cache->solution.rescaleIterations;
}
