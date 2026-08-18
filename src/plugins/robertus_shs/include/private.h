/*
 * Author(s) of this file:
 *   (see top-level README.md for attribution: robertus_shs_core.c/.h
 *    implements Robertus, Philipse, Joosten & Levine, J. Chem. Phys.
 *    90(8), 4482 (1989))
 */

#ifndef ROBERTUS_SHS_PRIVATE_H
#define ROBERTUS_SHS_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself
 * (this model combines multicomponent structure-factor interaction
 * physics with a per-class particle form factor internally -- see
 * include/sasfit_robertus_shs.h for why it is categorised as ff_*).
 */

// optional, depends on structure factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;

#else


#endif

// adjust according to the plugins name
#include "sasfit_robertus_shs.h"

// the shared multicomponent Percus-Yevick sticky-hard-sphere engine
// (portable C99+GSL, no SASfit-specific types) used by every model
// function in this plugin -- see include/robertus_shs_core.h.
#include "robertus_shs_core.h"

#include <stdlib.h> // for getenv(), used by rshs_maybe_export_grid() below

//
// add local defines here:
// #define P0 param->p[0]
//

/* Diagnostic hook: if the SASFIT_ROBERTUS_EXPORT_GRID environment
 * variable is set (to a file path) before SASfit is launched, every
 * time the size-distribution grid is freshly (re)built -- i.e. on a
 * cache miss, not on every q-point of an already-cached scan -- it
 * gets dumped there as plain ASCII (see rshs_export_classes() in
 * robertus_shs_core.h/.c). Called twice per fresh build below: once
 * right after rshs_make_classes*() succeeds (so the grid is still
 * captured for debugging even if the solve that follows fails), and
 * again after rshs_solve() succeeds (overwriting with the richer
 * version that also includes each class's Baxter b[i]). Zero cost
 * when unset -- one getenv() call per parameter change, not per
 * q-point -- and purely opt-in, no GUI or Tcl changes needed to use
 * it.
 *
 * How to set the variable before launching SASfit (must be set in
 * the SAME shell/session that then starts sasfit -- setting it and
 * launching SASfit from a Desktop icon/Finder/Explorer double-click
 * afterward will NOT pick it up, since that doesn't inherit the
 * shell's environment):
 *
 *   Linux / macOS (bash or zsh):
 *     export SASFIT_ROBERTUS_EXPORT_GRID=/home/me/grid.txt
 *     ./sasfit                    (or however SASfit is normally started)
 *
 *   Windows, Command Prompt (cmd.exe):
 *     set SASFIT_ROBERTUS_EXPORT_GRID=C:\Users\me\grid.txt
 *     sasfit.exe
 *
 *   Windows, PowerShell:
 *     $env:SASFIT_ROBERTUS_EXPORT_GRID = "C:\Users\me\grid.txt"
 *     .\sasfit.exe
 *
 * To have it apply every time (not just the current shell session),
 * set it as a persistent user/system environment variable instead:
 * on Windows via System Properties -> Environment Variables (or
 * `setx SASFIT_ROBERTUS_EXPORT_GRID C:\Users\me\grid.txt`, which takes
 * effect in *new* shells/processes only, not the one that ran setx);
 * on Linux/macOS by adding the `export` line above to ~/.bashrc,
 * ~/.zshrc, or equivalent shell startup file.                         */
static void rshs_maybe_export_grid(const rshs_system *sys)
{
    const char *path = getenv("SASFIT_ROBERTUS_EXPORT_GRID");
    if (path && path[0]) rshs_export_classes(sys, path);
}

// Shared parameter-cache plumbing, used by all three model .c files.
// Not part of the confirmed real-plugin convention (the reference
// plugins examined don't need it, since none of them require an
// expensive shared solve step before evaluating q) -- added because
// re-solving the p(p+1)/2-dimensional nonlinear Percus-Yevick system
// on every single q-point would be prohibitively slow; solving once
// per parameter set and reusing across a q-scan is not optional here.
typedef struct {
    int    valid;
    double D, sigma_rel, phi, tau;
    int    dist, nclass;
    /* only meaningful when dist==RSHS_DIST_BILOGNORMAL, but always
     * included in the cache-key comparison below so a stale bimodal
     * solve can never be mistaken for a fresh unimodal one or vice
     * versa (or two different bimodal parameter sets for each other) */
    double D2, sigma_rel2, w1;
    rshs_system sys;
} rshs_plugin_cache;

static int rshs_plugin_cache_refresh(rshs_plugin_cache *c, double D, double sigma_rel,
                                      double phi, double tau, int dist, int nclass)
{
    if (c->valid && c->D == D && c->sigma_rel == sigma_rel && c->phi == phi &&
        c->tau == tau && c->dist == dist && c->nclass == nclass)
        return 0;

    if (rshs_make_classes(&c->sys, nclass, D, sigma_rel, phi, (rshs_dist_type)dist) != 0)
        return -1;
    rshs_maybe_export_grid(&c->sys);

    if (rshs_solve(&c->sys, tau) != 0)
        return -2;
    rshs_maybe_export_grid(&c->sys);

    c->D = D; c->sigma_rel = sigma_rel; c->phi = phi; c->tau = tau;
    c->dist = dist; c->nclass = nclass;
    c->valid = 1;
    return 0;
}

/* Bimodal-lognormal counterpart of rshs_plugin_cache_refresh() above,
 * for dist==RSHS_DIST_BILOGNORMAL. D/sigma_rel double as mode 1's
 * (Dmean1,srel1) so the same rshs_plugin_cache struct/fields serve
 * both paths; D2/sigma_rel2/w1 hold mode 2's parameters and the
 * mode-1 weight. moment/n_percent control the adaptive integration
 * range (see rshs_make_classes_bilognormal() in robertus_shs_core.h) --
 * pass moment=6.0 for the usual SAS r^6 forward-scattering weighting
 * and n_percent=0.0001 to match SASfit's own find_integration_range().*/
static int rshs_plugin_cache_refresh_bilognormal(rshs_plugin_cache *c,
                                      double Dmean1, double srel1,
                                      double Dmean2, double srel2, double w1,
                                      double phi, double tau, int nclass,
                                      double moment, double n_percent)
{
    const int dist = RSHS_DIST_BILOGNORMAL;

    if (c->valid && c->D == Dmean1 && c->sigma_rel == srel1 &&
        c->D2 == Dmean2 && c->sigma_rel2 == srel2 && c->w1 == w1 &&
        c->phi == phi && c->tau == tau && c->dist == dist && c->nclass == nclass)
        return 0;

    if (rshs_make_classes_bilognormal(&c->sys, nclass, Dmean1, srel1,
                                       Dmean2, srel2, w1, phi, moment, n_percent) != 0)
        return -1;
    rshs_maybe_export_grid(&c->sys);

    if (rshs_solve(&c->sys, tau) != 0)
        return -2;
    rshs_maybe_export_grid(&c->sys);

    c->D = Dmean1; c->sigma_rel = srel1; c->D2 = Dmean2; c->sigma_rel2 = srel2; c->w1 = w1;
    c->phi = phi; c->tau = tau; c->dist = dist; c->nclass = nclass;
    c->valid = 1;
    return 0;
}

#endif // end of file
