/* rmsa_physical.c -- see rmsa_physical.h */
#include "include/rmsa_physical.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Physical constants (same values used by SasView's hayter_msa.c, so
 * that test_physical_conversion.c's cross-check against it isn't also
 * fighting a difference in which vintage of CODATA constants was used;
 * the discrepancy against current CODATA values is far below the
 * accuracy of the linearized-Poisson-Boltzmann physics itself).       */
static const double ELEMENTARY_CHARGE = 1.602189e-19;      /* C */
static const double BOLTZMANN_CONSTANT = 1.380662e-23;     /* J/K */
static const double VACUUM_PERMITTIVITY = 8.85418782e-12;  /* C^2/(N m^2) */
static const double AVOGADRO_NUMBER = 6.022e23;             /* 1/mol */

int rmsa_physical_to_dimensionless(const RMSAPhysicalParameters *physical, RMSAPhysicalConversion *result)
{
    if (physical->radius <= 0.0 || physical->lengthUnitInMeters <= 0.0
        || physical->temperature <= 0.0 || physical->dielectricConstant <= 0.0)
        return -1;
    if (physical->volumeFraction <= 0.0 && physical->molarity <= 0.0)
        return -1;

    double diameterMeters = 2.0 * physical->radius * physical->lengthUnitInMeters;
    double particleVolumeMeters3 = (4.0 / 3.0) * M_PI * (diameterMeters / 2.0) * (diameterMeters / 2.0) * (diameterMeters / 2.0);

    double beta = 1.0 / (BOLTZMANN_CONSTANT * physical->temperature);
    double permittivity = physical->dielectricConstant * VACUUM_PERMITTIVITY;
    double chargeCoulomb = physical->macroionCharge * ELEMENTARY_CHARGE;

    /* Resolve macroion number density and volume fraction, whichever
     * was given directly, so both are available and mutually
     * consistent regardless of which the caller provided.             */
    double numberDensityPerM3, volumeFraction;
    if (physical->volumeFraction > 0.0) {
        volumeFraction = physical->volumeFraction;
        numberDensityPerM3 = volumeFraction / particleVolumeMeters3;
    } else {
        numberDensityPerM3 = physical->molarity * AVOGADRO_NUMBER * 1.0e3; /* mol/L -> 1/m^3 */
        volumeFraction = numberDensityPerM3 * particleVolumeMeters3;
    }

    /* Total ionic strength: macroion counterions (|macroionCharge|
     * monovalent counterions released per macroion, each contributing
     * 1^2 to the charge-squared sum) plus any added 1:1 salt (both the
     * cation and anion contribute, hence the factor of 2). This is the
     * same combination as Hansen & Hayter's kappa_m^2 (eq. 17 of
     * Hayter & Penfold 1981) generalized to include added salt.        */
    double saltNumberDensityPerM3 = physical->saltMolarity * AVOGADRO_NUMBER * 1.0e3;
    double ionicStrengthTerm = 0.5 * ELEMENTARY_CHARGE * ELEMENTARY_CHARGE
        * (fabs(physical->macroionCharge) * numberDensityPerM3 + 2.0 * saltNumberDensityPerM3);

    double kappaPerMeter = sqrt(2.0 * beta * ionicStrengthTerm / permittivity);
    double debyeScreeningParam = kappaPerMeter * diameterMeters;

    /* Hayter & Penfold eq. 2-3a: gamma*exp(-k) is the contact
     * potential in kT; gamma itself (what rmsa_compute() wants) is
     * this expression before the exp(-k) is applied by rmsa_compute()
     * internally.                                                     */
    double gamma = beta * chargeCoulomb * chargeCoulomb
        / (M_PI * permittivity * diameterMeters * (2.0 + debyeScreeningParam) * (2.0 + debyeScreeningParam));

    result->gamma = gamma;
    result->screeningLength = (1.0 / kappaPerMeter) / physical->lengthUnitInMeters;
    result->volumeFraction = volumeFraction;
    result->debyeScreeningParam = debyeScreeningParam;
    /* Standard ionic strength I = 0.5*sum(n_i*z_i^2) (e.g. Atkins'
     * Physical Chemistry, or any electrolyte-solution reference),
     * reported back out in mol/L. Sanity check: pure 1:1 salt at
     * concentration c gives I=c exactly (a well-known identity) --
     * (|0|*0 + 2*c)/2 = c confirms the 0.5 here is required; an
     * earlier version of this code omitted it and reported 2x the
     * standard value, caught by exactly that check.                   */
    result->ionicStrength = 0.5 * (fabs(physical->macroionCharge) * numberDensityPerM3 + 2.0 * saltNumberDensityPerM3)
        / (AVOGADRO_NUMBER * 1.0e3);
    return 0;
}
