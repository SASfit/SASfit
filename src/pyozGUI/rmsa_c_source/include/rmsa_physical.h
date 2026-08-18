#ifndef RMSA_PHYSICAL_H
#define RMSA_PHYSICAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double radius;
    double lengthUnitInMeters;
    double macroionCharge;
    double volumeFraction;
    double molarity;
    double saltMolarity;
    double temperature;
    double dielectricConstant;
} RMSAPhysicalParameters;

typedef struct {
    double gamma;
    double screeningLength;
    double volumeFraction;
    double debyeScreeningParam;
    double ionicStrength;
} RMSAPhysicalConversion;

int rmsa_physical_to_dimensionless(const RMSAPhysicalParameters *physical, RMSAPhysicalConversion *result);

#ifdef __cplusplus
}
#endif
#endif /* RMSA_PHYSICAL_H */
