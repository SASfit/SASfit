#include "include/rmsa_physical.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double ELEMENTARY_CHARGE = 1.602189e-19;
static const double BOLTZMANN_CONSTANT = 1.380662e-23;
static const double VACUUM_PERMITTIVITY = 8.85418782e-12;
static const double AVOGADRO_NUMBER = 6.022e23;

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

    double numberDensityPerM3, volumeFraction;
    if (physical->volumeFraction > 0.0) {
        volumeFraction = physical->volumeFraction;
        numberDensityPerM3 = volumeFraction / particleVolumeMeters3;
    } else {
        numberDensityPerM3 = physical->molarity * AVOGADRO_NUMBER * 1.0e3;
        volumeFraction = numberDensityPerM3 * particleVolumeMeters3;
    }

    double saltNumberDensityPerM3 = physical->saltMolarity * AVOGADRO_NUMBER * 1.0e3;
    double ionicStrengthTerm = 0.5 * ELEMENTARY_CHARGE * ELEMENTARY_CHARGE
        * (fabs(physical->macroionCharge) * numberDensityPerM3 + 2.0 * saltNumberDensityPerM3);

    double kappaPerMeter = sqrt(2.0 * beta * ionicStrengthTerm / permittivity);
    double debyeScreeningParam = kappaPerMeter * diameterMeters;

    double gamma = beta * chargeCoulomb * chargeCoulomb
        / (M_PI * permittivity * diameterMeters * (2.0 + debyeScreeningParam) * (2.0 + debyeScreeningParam));

    result->gamma = gamma;
    result->screeningLength = (1.0 / kappaPerMeter) / physical->lengthUnitInMeters;
    result->volumeFraction = volumeFraction;
    result->debyeScreeningParam = debyeScreeningParam;
    result->ionicStrength = 0.5 * (fabs(physical->macroionCharge) * numberDensityPerM3 + 2.0 * saltNumberDensityPerM3)
        / (AVOGADRO_NUMBER * 1.0e3);
    return 0;
}
