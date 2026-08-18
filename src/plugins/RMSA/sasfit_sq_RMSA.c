/*
 * Author(s) of this file:
 *   (see top-level README.md for the underlying RMSA project's own
 *   attribution: Hayter & Penfold, Mol. Phys. 42, 109 (1981); Hansen &
 *   Hayter, Mol. Phys. 46, 651 (1982))
 */
#include "include/private.h"
#include <sasfit_error_sq.h>

#define RADIUS            param->p[0]
#define CHARGE            param->p[1]
#define VOLFRACTION       param->p[2]
#define TEMPERATURE       param->p[3]
#define SALT_MOLARITY     param->p[4]
#define DIELECTRIC_CONST  param->p[5]

/* -------------------------------------------------------------------
 * Caching and thread safety
 *
 * SASfit calls this function once per Q value, both across the whole
 * Q-array for one parameter set and, during Levenberg-Marquardt
 * fitting, repeatedly with the same parameters perturbed slightly (for
 * the numerical Jacobian) -- exactly the two patterns
 * rmsa_compute_cached() targets (see the RMSA project's README.md
 * "Performance" section: ~1000-1900x for repeated-identical calls,
 * ~2x for small perturbations). Since this function's required
 * signature (matching every other SASfit sq_* model, confirmed against
 * real plugin sources) has no slot for an explicit cache handle, the
 * cache below is a file-scope variable instead, persisting across
 * calls implicitly.
 *
 * It is declared _Thread_local (C11) rather than a plain `static`,
 * specifically because this project could not confirm, from SASfit's
 * available plugin sources alone, whether structure-factor plugins are
 * ever invoked from more than one thread concurrently. A plain shared
 * `static RMSACache` would be a genuine data race if that ever
 * happens -- RMSACache is caller-owned state, safe for concurrent use
 * only if each thread has its own (see the RMSA project's README.md
 * "Thread safety" and "Performance" sections). _Thread_local costs
 * nothing if SASfit is in fact single-threaded here, and removes the
 * risk entirely if it is not.
 * ------------------------------------------------------------------- */
scalar sasfit_sq_RMSA(scalar q, sasfit_param * param)
{
    static _Thread_local RMSACache cache;
    static _Thread_local int cacheInitialized = 0;

    SASFIT_ASSERT_PTR(param);
    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0", q);
    SASFIT_CHECK_COND1((RADIUS <= 0.0), param, "RADIUS(%lg) <= 0", RADIUS);
    SASFIT_CHECK_COND1((VOLFRACTION <= 0.0), param, "VOLFRACTION(%lg) <= 0", VOLFRACTION);
    SASFIT_CHECK_COND1((VOLFRACTION >= 1.0), param, "VOLFRACTION(%lg) >= 1", VOLFRACTION);
    SASFIT_CHECK_COND1((TEMPERATURE <= 0.0), param, "TEMPERATURE(%lg) <= 0", TEMPERATURE);
    SASFIT_CHECK_COND1((SALT_MOLARITY < 0.0), param, "SALT_MOLARITY(%lg) < 0", SALT_MOLARITY);
    SASFIT_CHECK_COND1((DIELECTRIC_CONST <= 0.0), param, "DIELECTRIC_CONST(%lg) <= 0", DIELECTRIC_CONST);

    if (!cacheInitialized) {
        cache = (RMSACache){0};
        cacheInitialized = 1;
    }

    RMSAPhysicalParameters phys = {0};
    phys.radius = RADIUS;
    phys.lengthUnitInMeters = 1e-10; /* Angstrom -- this file's fixed
                                     * convention (see include/sasfit_RMSA.h's
                                     * parameter table); must match
                                     * whatever unit RADIUS is actually
                                     * given in, or gamma/screeningLength
                                     * come out describing a completely
                                     * different (wrong) physical size. */
    phys.macroionCharge = CHARGE;
    phys.volumeFraction = VOLFRACTION;
    phys.saltMolarity = SALT_MOLARITY;
    phys.temperature = TEMPERATURE;
    phys.dielectricConstant = DIELECTRIC_CONST;

    RMSAPhysicalConversion conv;
    if (rmsa_physical_to_dimensionless(&phys, &conv) != 0) {
        sasfit_err("sasfit_sq_RMSA: invalid physical parameters\n");
        return 1.0; /* SASfit's own convention for a degenerate/failed
                      * structure-factor evaluation -- "no structural
                      * correlation" rather than a numeric error code,
                      * since this function must return a scalar S(Q),
                      * not a status. */
    }

    double structureFactor;
    int status = rmsa_compute_cached(RADIUS, conv.screeningLength, conv.gamma, conv.volumeFraction,
                                      &q, &structureFactor, 1, &cache, NULL);
    if (status < 0) {
        sasfit_err("sasfit_sq_RMSA: %s\n", rmsa_error_string(status));
        return 1.0;
    }
    return structureFactor;
}

scalar sasfit_sq_RMSA_f(scalar q, sasfit_param * param)
{
    /* Confirmed directly against the real, independent
     * src/plugins/clusteryukawa/sasfit_sq_clusteryukawa.c in this same
     * source tree: a structure factor's "_f" (scattering amplitude)
     * variant is stubbed to exactly 0.0, with SASFIT_ASSERT_PTR(param)
     * still called first -- since "amplitude" is not a meaningful
     * concept for a structure factor, but the pointer check itself is
     * still done unconditionally. An earlier version of this file
     * omitted that call here. */
    (void)q;
    SASFIT_ASSERT_PTR(param);
    return 0.0; // insert your code here
}

scalar sasfit_sq_RMSA_v(scalar q, sasfit_param * param, int dist)
{
    /* Same reasoning and same real precedent as sasfit_sq_RMSA_f()
     * above: "particle volume" is not a meaningful concept for a
     * structure factor either. */
    (void)q; (void)dist;
    SASFIT_ASSERT_PTR(param);
    return 0.0; // insert your code here
}
