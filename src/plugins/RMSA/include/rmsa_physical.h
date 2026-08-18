/* ------------------------------------------------------------------
 * rmsa_physical.h -- convert typical physical solution properties
 * (macroion charge, salt concentration, temperature, dielectric
 * constant) into the dimensionless (gamma, screeningLength,
 * volumeFraction) triple that rmsa_compute()/rmsa_solve() expect.
 *
 * rmsa.c/rmsa.h deliberately do *not* do this conversion themselves:
 * as README.md's "Physical parameter conversion" section explains,
 * there is more than one convention in the literature for mapping a
 * macroion's charge and a solution's ionic content onto the
 * dimensionless (gamma, kappa*sigma) the Hayter-Penfold model actually
 * solves in, and baking one particular choice into the core solver
 * would make it harder to use with a different convention later. This
 * file provides one specific, standard, and validated choice --
 * linearized Poisson-Boltzmann (Debye-Hueckel) screening from the
 * total ionic strength (counterions from the macroions themselves,
 * plus any added monovalent salt), and the contact potential of
 * Hayter & Penfold's eq. 2 -- as a convenience for the common case,
 * not as the only correct way to do this.
 *
 * FORMULAS (SI internally; see the per-field comments below for the
 * exact unit conventions each input/output actually uses -- lengths in
 * particular are in a caller-chosen unit, not meters). With
 * sigma = 2*radius (diameter), beta = 1/(kB*T), e = elementary charge,
 * eps0 = vacuum permittivity, eps = dielectricConstant, N_A = Avogadro's
 * number:
 *
 *   number density:      n = volumeFraction / ((4/3)*pi*radius^3)
 *                         (or, from molarity c [mol/L]: n = c*N_A*1e3
 *                         [1/m^3], with volumeFraction derived back as
 *                         n*(4/3)*pi*radius^3 for consistency)
 *
 *   ionic strength:       I = 0.5*(|macroionCharge|*n + 2*n_salt)
 *                         (mol/L; the standard I = 0.5*sum(n_i*z_i^2)
 *                         definition, with the macroion's counterions
 *                         --  |macroionCharge| of them per macroion,
 *                         each monovalent -- and a fully-dissociated
 *                         1:1 salt contributing n_salt cations + n_salt
 *                         anions; sanity check: pure 1:1 salt at
 *                         concentration c gives I=c exactly, a standard
 *                         identity, since (0 + 2*c)/2 = c)
 *
 *   screening (Hayter &   kappa = sqrt(beta*e^2*(|macroionCharge|*n
 *   Penfold eq. 17,                    + 2*n_salt) / (eps*eps0))
 *   generalized to        screeningLength = 1/kappa
 *   include added salt):  debyeScreeningParam = kappa*sigma
 *
 *   contact potential      gamma = beta*(macroionCharge*e)^2
 *   (eqs. 2-3a combined):          / (pi*eps*eps0*sigma*(2+kappa*sigma)^2)
 *
 * This is the same physics (and, for its charge/salt/screening
 * formulas, the same specific convention) as SasView's hayter_msa.c,
 * against which it is cross-checked in test_physical_conversion.c;
 * see that file and README.md for the validation story (including a
 * factor-of-2 error in an earlier version of the ionicStrength output
 * above, caught by the "pure salt gives I=c" identity, and where this
 * convention's own approximations -- linearized PB theory -- break
 * down).
 * ------------------------------------------------------------------ */
#ifndef RMSA_PHYSICAL_H
#define RMSA_PHYSICAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double radius;             /* macroion radius, in whatever length unit you intend
                                 * to pass to rmsa_compute() (e.g. nm) */
    double lengthUnitInMeters; /* how many meters is one unit of `radius`?
                                 * e.g. 1e-9 for nm, 1e-10 for Angstrom, 1e-6 for micron.
                                 * Verified length-unit-invariant: the same physical
                                 * system described in any of these gives identical
                                 * gamma/debyeScreeningParam/ionicStrength, with
                                 * screeningLength scaled to match whichever unit
                                 * `radius` was given in. */
    double macroionCharge;     /* effective macroion surface charge Z* (a.k.a. Z_m),
                                 * in elementary-charge units (dimensionless number,
                                 * e.g. 20.0 for 20 e) -- see README.md's "Physical
                                 * parameter conversion" for why this is usually an
                                 * *effective* charge fitted to data, not the bare
                                 * structural charge, and why that matters. Sign
                                 * doesn't matter (only |macroionCharge| and its
                                 * square appear in the formulas above).             */
    double volumeFraction;     /* macroion volume fraction eta (dimensionless,
                                 * 0 < eta < 1); set to 0 (or negative) and give
                                 * molarity below instead if you have a concentration
                                 * rather than a volume fraction */
    double molarity;           /* macroion molar concentration, mol/L -- only used if
                                 * volumeFraction above is <= 0 */
    double saltMolarity;       /* additional monovalent (1:1, e.g. NaCl) salt
                                 * concentration, mol/L (0 for salt-free solution) */
    double temperature;        /* solution temperature, kelvin (e.g. 298.15 for 25 C --
                                 * this is kelvin, not Celsius: passing 25.0 here would
                                 * be room temperature colder than absolute zero) */
    double dielectricConstant; /* solvent *relative* permittivity eps (dimensionless;
                                 * ~78.5 for water at 25 C, ~87.9 at 0 C -- see e.g. the
                                 * Malmberg-Maryott tables for other temperatures). Not
                                 * the absolute permittivity eps*eps0 -- eps0 is applied
                                 * internally. */
} RMSAPhysicalParameters;

typedef struct {
    double gamma;               /* -> rmsa_compute()'s chargeParameter (dimensionless) */
    double screeningLength;     /* -> rmsa_compute()'s screeningLength; same length
                                  * unit as physical->radius (i.e. NOT meters unless
                                  * physical->radius was itself given in meters) */
    double volumeFraction;      /* -> rmsa_compute()'s volumeFraction (dimensionless;
                                  * echoes the input if given directly, otherwise
                                  * derived from molarity) */
    double debyeScreeningParam; /* kappa*sigma = 2*radius/screeningLength (dimensionless),
                                  * for inspection -- same quantity as
                                  * RMSASolution.screeningParam once solved */
    double ionicStrength;       /* total ionic strength I = 0.5*sum(n_i*z_i^2)
                                  * (standard definition; macroion counterions + added
                                  * salt), in mol/L, for inspection. Sanity-checked
                                  * against the "pure 1:1 salt at concentration c gives
                                  * I=c" identity in test_physical_conversion.c.       */
} RMSAPhysicalConversion;

/* Returns 0 on success, filling in *result. Returns -1 for invalid
 * input (radius, temperature, or dielectricConstant <= 0; or both
 * volumeFraction and molarity <= 0).                                   */
int rmsa_physical_to_dimensionless(const RMSAPhysicalParameters *physical, RMSAPhysicalConversion *result);

#ifdef __cplusplus
}
#endif
#endif /* RMSA_PHYSICAL_H */
