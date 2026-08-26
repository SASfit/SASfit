/*
 * robertus_shs_core.c  -- see robertus_shs_core.h for references and
 * a description of what this reproduces from Robertus et al. (1989)
 * and its accompanying Fortran code (LABDA_HB/SUB_HB/UTILS).
 */
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

/* SUNDIALS' KINSOL (matrix-free Newton-Krylov nonlinear solver) is the
 * primary solve backend for rshs_solve() below -- see the big comment
 * above that function for why. Built as an independent, separately-
 * versioned dependency (SUNDIALS 7.x, fetched via CMake FetchContent
 * -- see src/sundials7/CMakeLists.txt) rather than sharing the
 * project's existing bundled SUNDIALS 2.7.0 (still used unmodified by
 * src/sasfit_oz/sasfit_oz_solver.c): the two SUNDIALS versions export
 * same-named-but-incompatible-ABI symbols (KINCreate, KINSol, etc. --
 * 2.7.0's take no SUNContext, 7.x's require one), so this plugin is
 * linked with -Wl,--exclude-libs=ALL (see its CMakeLists.txt) to keep
 * its own statically-linked copy of those symbols private and unable
 * to collide with the old version's, wherever both end up loaded in
 * the same process. Verified directly: a naive build without that
 * flag segfaults immediately (symbol collision during dynamic
 * linking, before either version's own code even runs); with it,
 * both versions solve correctly side by side.                        */
#include <sundials/sundials_context.h>
#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_spgmr.h>

#include "include/robertus_shs_core.h"

/* thread-local: if the batch functions are used under OpenMP, each
 * thread gets its own error buffer and (see sm_workspace below) its
 * own scratch matrices, so parallel q-evaluation is actually safe.   */
static __thread char g_err[256] = "";
static void set_err(const char *fmt_literal_msg) {
    /* small, allocation-free diagnostic setter; callers pass a plain
     * literal or a pre-formatted buffer, no varargs needed anywhere
     * in this file. */
    strncpy(g_err, fmt_literal_msg, sizeof(g_err) - 1);
    g_err[sizeof(g_err) - 1] = '\0';
}
const char *rshs_last_error(void) { return g_err; }

/* See the doc comment in robertus_shs_core.h. Set (only) from
 * rshs_solve() when KINSOL fails but the GSL fallback rescues the
 * solve; cleared both when KINSOL succeeds directly (no fallback
 * needed) and here, on read, so it is genuinely one-shot.            */
static __thread char g_solver_note[400] = "";
const char *rshs_last_solver_note(void)
{
    static __thread char buf[400];
    strncpy(buf, g_solver_note, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    g_solver_note[0] = '\0';
    return buf;
}

/* -------------------------------------------------------------------
 * packed upper-triangular (i>=j) index, 0-based, matching Fortran's
 * inx()/v() but zero-based:  idx(i,j) = i*(i+1)/2 + j  for i>=j.
 * ------------------------------------------------------------------- */
static int PIDX(int i, int j) {
    if (i < j) { int t = i; i = j; j = t; }
    return i * (i + 1) / 2 + j;
}

/* =====================================================================
 * 1. SIZE DISTRIBUTIONS
 * ===================================================================== */

/* Schulz-Zimm pdf, robust via lgamma (avoids the Stirling-series
 * workaround the original Fortran needed because it had no lgamma). */
static double pdf_schulz_zimm(double D, double Dmean, double srel)
{
    double z, zp, rf, logf;
    if (D <= 0.0) return 0.0;
    z  = 1.0 / (srel * srel) - 1.0;
    zp = z + 1.0;
    rf = D / Dmean;
    logf = zp * log(zp) - zp - log(Dmean) - gsl_sf_lngamma(zp)
         + z * log(rf) + zp * (1.0 - rf);
    return exp(logf);
}

static double pdf_lognormal(double D, double Dmean, double srel)
{
    double sigma_ln2, mu_ln, u;
    if (D <= 0.0) return 0.0;
    sigma_ln2 = log(1.0 + srel * srel);
    mu_ln     = log(Dmean) - 0.5 * sigma_ln2;
    u = log(D) - mu_ln;
    return 1.0 / (D * sqrt(2.0 * M_PI * sigma_ln2)) * exp(-0.5 * u * u / sigma_ln2);
}

static double pdf_gaussian(double D, double Dmean, double srel)
{
    double sigma = srel * Dmean;
    double u = D - Dmean;
    if (D < 0.0) return 0.0;
    return 1.0 / (sigma * sqrt(2.0 * M_PI)) * exp(-0.5 * u * u / (sigma * sigma));
}

/* Weibull coefficient-of-variation as a function of shape k only:
 * CV(k) = sqrt( Gamma(1+2/k)/Gamma(1+1/k)^2 - 1 )                     */
static double weibull_cv(double k)
{
    double g1 = gsl_sf_gamma(1.0 + 1.0 / k);
    double g2 = gsl_sf_gamma(1.0 + 2.0 / k);
    double r  = g2 / (g1 * g1) - 1.0;
    return (r > 0.0) ? sqrt(r) : 0.0;
}

/* Solve CV(k) = srel for k by bisection (CV(k) is monotonically
 * decreasing in k over the practically relevant range).               */
static double weibull_shape_from_cv(double srel)
{
    double klo = 0.05, khi = 100.0, kmid;
    double flo = weibull_cv(klo) - srel;
    double fhi = weibull_cv(khi) - srel;
    int it;
    if (flo * fhi > 0.0) {
        /* fall back: extreme sigma requested, clamp to bracket ends */
        return (fabs(flo) < fabs(fhi)) ? klo : khi;
    }
    for (it = 0; it < 100; it++) {
        kmid = 0.5 * (klo + khi);
        double fmid = weibull_cv(kmid) - srel;
        if (fabs(fmid) < 1e-10) break;
        if ((flo < 0) == (fmid < 0)) { klo = kmid; flo = fmid; }
        else                          { khi = kmid; fhi = fmid; }
    }
    return 0.5 * (klo + khi);
}

static double pdf_weibull(double D, double Dmean, double srel)
{
    double k = weibull_shape_from_cv(srel);
    double lambda = Dmean / gsl_sf_gamma(1.0 + 1.0 / k);
    double u;
    if (D <= 0.0) return 0.0;
    u = D / lambda;
    return (k / lambda) * pow(u, k - 1.0) * exp(-pow(u, k));
}

void rshs_reset(rshs_system *sys)
{
    sys->solved = 0;
    sys->p_solved = 0;
}

int rshs_export_classes(const rshs_system *sys, const char *filename)
{
    FILE *f;
    int i;

    if (!filename || !filename[0]) { set_err("export filename is empty"); return -1; }
    f = fopen(filename, "w");
    if (!f) { set_err("could not open export file for writing"); return -2; }

    fprintf(f, "# RobertusSHS size-distribution grid export\n");
    fprintf(f, "# nclass=%d  phi=%.10g  solved=%d\n", sys->p, sys->phi, sys->solved);
    if (sys->solved)
        fprintf(f, "#i\tR[nm]\tx[i]\tb[i]\n");
    else
        fprintf(f, "#i\tR[nm]\tx[i]\n");

    for (i = 0; i < sys->p; i++) {
        /* Exported as radius (D[i]/2), matching the convention already
         * used everywhere this grid feeds into a form factor (e.g.
         * ff(q, 0.5*sys->D[i], ctx) in intensity_generic_impl(),
         * build_Q_matrix()'s vrx[i]=0.5*sys->D[i]*q, etc.) -- the
         * *internal* representation stays diameter (sys->D[i]), since
         * the Percus-Yevick contact-distance formulas throughout this
         * file (e.g. r1[i][j]=0.5*(D_i+D_j) in rshs_solve()) are
         * fundamentally diameter-based; only this export's column
         * changed, not the underlying rshs_system.                   */
        if (sys->solved)
            fprintf(f, "%d\t%.10g\t%.10g\t%.10g\n", i, 0.5 * sys->D[i], sys->x[i], sys->b[i]);
        else
            fprintf(f, "%d\t%.10g\t%.10g\n", i, 0.5 * sys->D[i], sys->x[i]);
    }

    fclose(f);
    return 0;
}

/* Bundles a distribution's own shape parameters (mean, relative
 * width) so the 4 built-in PDFs can be called through the same
 * generic rshs_pdf_fn signature as any user-supplied one.            */
typedef struct { double Dmean, srel; } meansig_ctx;

static double pdf_schulz_zimm_g(double D, void *ctx) {
    meansig_ctx *c = ctx; return pdf_schulz_zimm(D, c->Dmean, c->srel);
}
static double pdf_lognormal_g(double D, void *ctx) {
    meansig_ctx *c = ctx; return pdf_lognormal(D, c->Dmean, c->srel);
}
static double pdf_gaussian_g(double D, void *ctx) {
    meansig_ctx *c = ctx; return pdf_gaussian(D, c->Dmean, c->srel);
}
static double pdf_weibull_g(double D, void *ctx) {
    meansig_ctx *c = ctx; return pdf_weibull(D, c->Dmean, c->srel);
}

/* Shared linear-grid classing core: builds p classes evenly spanning
 * [D0, D0+(p-1)*dr], weighted by pdf(D_i)*dr and renormalised so the
 * 3rd moment reproduces the target volume fraction phi exactly. Used
 * by rshs_make_classes_generic() below -- the fixed +/-3*sigma-window
 * entry point kept for genuinely arbitrary/unknown user pdfs (see the
 * log-spaced classes_from_grid_log() further below for why every
 * *named* distribution -- where the pdf shape is known and an
 * adaptive range can be found -- no longer uses this).                */
static int classes_from_grid(rshs_system *sys, int p, double D0, double dr,
                              double phi, rshs_pdf_fn pdf, void *pdf_ctx)
{
    int i;
    double w[RSHS_MAXP], D[RSHS_MAXP];
    double M3, x0;

    /* NOTE: unlike earlier versions of this file, we deliberately do
     * NOT wipe the whole struct here -- sys->lambda[]/b[]/solved/
     * p_solved must survive across calls so that rshs_solve() can
     * auto-detect a warm start (see robertus_shs_core.h). Only the
     * class-defining fields (x, D, p, phi) are (re)written below;
     * they are fully overwritten for every i in [0,p) on every
     * successful return, so no stale data can leak into them.        */
    sys->p   = p;
    sys->phi = phi;

    M3 = 0.0;
    for (i = 0; i < p; i++) {
        D[i] = D0 + i * dr;
        w[i] = pdf(D[i], pdf_ctx) * dr;
        M3  += w[i] * D[i] * D[i] * D[i];
    }
    if (!(M3 > 0.0)) { set_err("distribution produced zero mass (check Dmean/sigma)"); return -6; }

    x0 = phi / ((M_PI / 6.0) * M3);
    for (i = 0; i < p; i++) {
        sys->D[i] = D[i];
        sys->x[i] = x0 * w[i];
    }
    return 0;
}

/* Log-spaced counterpart of classes_from_grid() above: builds p
 * classes equidistant in ln(D) between Rstart>0 and Rend, i.e.
 * D_i = Rstart*(Rend/Rstart)^(i/(p-1)). The Riemann-sum weight has to
 * pick up a Jacobian factor D_i relative to the linear-grid formula:
 * changing variables to u=ln(D) gives dD = D*du, so the correct
 * discretisation of the integral is w_i = pdf(D_i)*D_i*dlnD, not
 * pdf(D_i)*dr. Otherwise identical to classes_from_grid() (same
 * renormalisation so the 3rd moment reproduces phi exactly).
 *
 * Why this exists: every adaptive-range distribution below (Schulz-
 * Zimm, LogNormal, Gaussian, Weibull, BiLogNormal) has its Rend found
 * from a D^6-weighted tail criterion, which by construction reaches
 * far past Dmean/Dmean1/Dmean2 -- and for BiLogNormal specifically,
 * Rend is set by whichever of the two modes reaches furthest, while
 * Rstart sits near 0. A *linear* grid between those two endpoints
 * spends the vast majority of its p points on sizes that contribute
 * essentially nothing (the empty stretch below the smallest mode and
 * between separated modes), starving the modes that actually carry
 * the distribution's mass of resolution. A *log* grid instead spends
 * roughly the same number of points per decade of size everywhere in
 * [Rstart,Rend], so a small mode near Rstart and a much larger one
 * near Rend both get meaningfully resolved at the same total p --
 * confirmed by benchmark: see the accompanying test comparing this
 * against the old linear approach on a D1=20nm/D2=200nm bimodal case.*/
static int classes_from_grid_log(rshs_system *sys, int p, double Rstart, double Rend,
                                  double phi, rshs_pdf_fn pdf, void *pdf_ctx)
{
    int i;
    double w[RSHS_MAXP], D[RSHS_MAXP];
    double dlnD, M3, x0;

    if (!(Rstart > 0.0) || !(Rend > Rstart)) { set_err("invalid log-spaced integration range"); return -6; }

    sys->p   = p;
    sys->phi = phi;

    dlnD = log(Rend / Rstart) / (double)(p - 1);
    M3 = 0.0;
    for (i = 0; i < p; i++) {
        D[i] = Rstart * exp((double)i * dlnD);
        w[i] = pdf(D[i], pdf_ctx) * D[i] * dlnD;
        M3  += w[i] * D[i] * D[i] * D[i];
    }
    if (!(M3 > 0.0)) { set_err("distribution produced zero mass (check Dmean/sigma)"); return -6; }

    x0 = phi / ((M_PI / 6.0) * M3);
    for (i = 0; i < p; i++) {
        sys->D[i] = D[i];
        sys->x[i] = x0 * w[i];
    }
    return 0;
}

int rshs_make_classes_generic(rshs_system *sys, int p, double Dmean, double srel,
                               double phi, rshs_pdf_fn pdf, void *pdf_ctx)
{
    int i;
    double del = 3.0; /* span +/- 3*sigma, as in the original code */
    double sigma, dr, D0;

    if (p < 1 || p > RSHS_MAXP) { set_err("p out of range [1,RSHS_MAXP]"); return -1; }
    if (Dmean <= 0.0)           { set_err("mean diameter must be > 0"); return -2; }
    if (srel  <  0.0)           { set_err("relative sigma must be >= 0"); return -3; }
    if (phi   <= 0.0 || phi >= 1.0) { set_err("phi must be in (0,1)"); return -4; }
    if (!pdf)                   { set_err("pdf callback is NULL"); return -7; }

    sys->p   = p;
    sys->phi = phi;

    if (p == 1 || srel == 0.0) {
        /* degenerate/monodisperse case, same treatment as the
         * original schultz() subroutine.                            */
        for (i = 0; i < p; i++) {
            sys->D[i] = Dmean;
            sys->x[i] = phi / (p * (M_PI / 6.0) * Dmean * Dmean * Dmean);
        }
        return 0;
    }

    sigma = srel * Dmean;
    dr    = 2.0 * del * sigma / (double)(p - 1);
    D0    = Dmean - del * sigma;
    if (D0 < 0.0) D0 = dr / 2.0;   /* same clip as the original Fortran */

    return classes_from_grid(sys, p, D0, dr, phi, pdf, pdf_ctx);
}

/* =====================================================================
 * ADAPTIVE, MOMENT-WEIGHTED, LOG-SPACED RANGE for the 4 built-in named
 * distributions (Schulz-Zimm, LogNormal, Gaussian, Weibull) and for
 * BiLogNormal. See the big comment above classes_from_grid_log() for
 * why log-spacing; this block is about finding Rend (Rstart is just a
 * small fixed floor, see rshs_make_classes_from_Rend() below).
 *
 * Find the diameter Rend at which the D^moment-weighted pdf (moment=6
 * matches the ~D^6 forward-scattering weighting of typical SAS form
 * factors) has decayed to n_percent of its own peak. For LogNormal
 * (and BiLogNormal, a sum of two LogNormals) this has an exact closed
 * form (lognorm_moment_tail() below): weighting a lognormal pdf by
 * D^moment shifts its mode by moment*sigma_ln^2 in log-space, so both
 * the shifted peak and the n_percent tail point are analytic.
 * Schulz-Zimm, Gaussian and Weibull don't have that algebraic luxury
 * -- SASfit's own find_integration_range() (src/sasfit_old/sasfit.c)
 * doesn't either, and instead locates the moment-weighted peak
 * analytically (closed forms below, cross-checked against that
 * routine's own R_max formulas) and then scans outward numerically
 * until the moment-weighted pdf itself decays to n_percent of that
 * peak -- scan_moment_tail() below does the same thing, evaluating
 * this file's own pdf_*() functions rather than SASfit's sasfit_sd_*()
 * ones, but otherwise the identical algorithm.
 * ===================================================================== */

/* Peak location and both tails of D^moment * pdf(D), found via the
 * scanning approach used for Schulz-Zimm/Gaussian/Weibull below, or
 * the closed form for LogNormal -- computed at TWO different moments:
 * moment=0 (the plain, unweighted pdf) for R_low, and moment=6 (the
 * ~D^6 forward-scattering weighting) for R_high. Using moment=0 for
 * the low side matters: using moment=6 on BOTH sides would push the
 * low cutoff up along with the high one, and for a small-size mode
 * that carries most of the *number* density but little of the
 * *scattering* weight (e.g. mode 1 of a strongly asymmetric bimodal
 * case), that would clip real particles the discretisation still
 * needs to represent. R_low instead reflects "where do particles of
 * this population actually exist" and R_high reflects "how far out
 * does the r^6-amplified tail still matter" -- two different
 * questions, so two different moments.                                */

/* Mirrors the analytic LogNorm branch of SASfit's own
 * find_integration_range(): weighting a lognormal pdf (which itself
 * carries a 1/D Jacobian, see pdf_lognormal() above) by D^moment
 * shifts the underlying normal distribution's mean in u=ln(D)-space
 * by (moment-1)*sigma_ln^2 (the "-1" comes from that Jacobian), and
 * the point where the still-Gaussian-shaped (in u-space) moment-
 * weighted pdf has decayed to n_percent of that shifted peak sits a
 * fixed "delta" away on either side -- so both tails are closed-form.
 * Only the side matching the caller's intent (see comment above) is
 * meaningful: pass moment=0 and read *R_low for the low cutoff, or
 * moment=6 and read *R_high for the high cutoff -- the other output
 * of a given call is not the quantity you want and should be ignored.*/
static void lognorm_moment_range(double Dmean, double srel, double moment,
                                  double n_percent, double *R_low, double *R_high)
{
    double sigma_ln2 = log(1.0 + srel * srel);
    double mu        = Dmean / sqrt(1.0 + srel * srel); /* median, = exp(mu_ln) */
    double delta     = sqrt(2.0 * sigma_ln2 * log(100.0 / n_percent));
    double D_peak    = mu * exp((moment - 1.0) * sigma_ln2);
    *R_low  = D_peak * exp(-delta);
    *R_high = D_peak * exp(delta);
}

/* Peak of D^moment * Schulz-Zimm(D;Dmean,srel). Schulz-Zimm is a
 * Gamma(shape=k=1/srel^2, scale=theta=Dmean*srel^2) distribution, so
 * weighting by D^moment shifts it to Gamma(shape=k+moment, scale=
 * theta), whose mode is (k+moment-1)*theta for k+moment>1. Simplifies
 * to Dmean*(1+srel^2*(moment-1)); at moment=0 this correctly gives
 * the *unweighted* Schulz-Zimm mode Dmean*(1-srel^2).                 */
static double peak_schulz_zimm(double Dmean, double srel, double moment)
{
    double peak = Dmean * (1.0 + srel * srel * (moment - 1.0));
    return (peak > 0.0) ? peak : Dmean;
}

/* Peak of D^moment * Gaussian(D;Dmean,sigma=srel*Dmean). Setting
 * d/dD[D^moment*exp(-(D-Dmean)^2/(2*sigma^2))]=0 gives the quadratic
 * D^2 - Dmean*D - moment*sigma^2 = 0; positive root below. Matches
 * SASfit's own Gaussian branch of find_integration_range() exactly
 * (R_max = 0.5*a3+0.5*sqrt(a3^2+4*moment*a2^2), a3=mean, a2=sigma);
 * at moment=0 this correctly gives D_peak=Dmean.                     */
static double peak_gaussian(double Dmean, double srel, double moment)
{
    double sigma = srel * Dmean;
    return 0.5 * Dmean + 0.5 * sqrt(Dmean * Dmean + 4.0 * moment * sigma * sigma);
}

/* Peak of D^moment * Weibull(D;Dmean,srel) (shape k from CV=srel,
 * scale lambda=Dmean/Gamma(1+1/k), as in pdf_weibull() above). Setting
 * d/dD[D^(k+moment-1)*exp(-(D/lambda)^k)]=0 gives D=lambda*((k+moment-1)/k)^(1/k);
 * at moment=0 this correctly gives the standard Weibull mode formula
 * lambda*((k-1)/k)^(1/k) (0 if k<=1, handled by the guard below).     */
static double peak_weibull(double Dmean, double srel, double moment)
{
    double k = weibull_shape_from_cv(srel);
    double lambda = Dmean / gsl_sf_gamma(1.0 + 1.0 / k);
    double val = (k + moment - 1.0) / k;
    return (val > 0.0) ? lambda * pow(val, 1.0 / k) : Dmean;
}

/* Numerically scans OUTWARD (dir=+1, multiplying by increasing n) or
 * INWARD toward 0 (dir=-1, dividing by increasing n) from the
 * (analytically located) peak of the D^moment-weighted pdf, until
 * that moment-weighted pdf has decayed to n_percent of its peak
 * value. Mirrors the scanning loops SASfit's own
 * find_integration_range() uses for its Schulz-Zimm/Gaussian/Weibull/
 * GEX/GEV/gammaSD branches (none of those have a closed-form tail
 * either, only a closed-form peak location) -- same idea, evaluating
 * this file's own pdf_*() instead of SASfit's sasfit_sd_*(). Capped
 * at 10000 steps as a safety net against a pathological/degenerate
 * pdf; ordinary inputs converge in well under a hundred.              */
static double scan_moment_tail(rshs_pdf_fn pdf, void *ctx, double R_peak,
                                double moment, double n_percent, int dir)
{
    double peak_val, R, val;
    int n;

    if (!(R_peak > 0.0)) R_peak = 1.0; /* degenerate guard */
    peak_val = pow(R_peak, moment) * pdf(R_peak, ctx);
    if (!(peak_val > 0.0)) return (dir > 0) ? 10.0 * R_peak : 0.1 * R_peak; /* degenerate guard */

    n = 1;
    do {
        R = (dir > 0) ? (double)(n + 1) * R_peak : R_peak / (double)(n + 1);
        val = pow(R, moment) * pdf(R, ctx);
        n++;
    } while (val > (n_percent / 100.0) * peak_val && n < 10000 && R > 1e-300);
    return R;
}

/* Shared "finish" step for every adaptive-range rshs_make_classes*()
 * entry point below: validate, handle the p==1/srel==0 monodisperse
 * shortcut, and hand off to classes_from_grid_log() with the caller-
 * supplied [Rstart,Rend] (each already found via the same moment-tail
 * machinery above -- Rstart at moment=0, Rend at moment=6, see the
 * comment block at the top of this section for why those differ).
 * Rstart is never exactly 0 (log(0) is undefined for the log-spaced
 * grid, and even on the old linear grid, D=0 made r1[0][0]=
 * 0.5*(D[0]+D[0])=0 in rshs_solve(), which poisoned the whole
 * nonlinear solve via a 1/r1[i][j] division; caught the hard way
 * while first debugging rshs_make_classes_bilognormal()) -- the
 * moment=0 tail search above always returns something >0.            */
static int rshs_make_classes_from_range(rshs_system *sys, int p, double Dmean, double srel,
                                         double phi, rshs_pdf_fn pdf, void *pdf_ctx,
                                         double Rstart, double Rend)
{
    int i;

    if (p < 1 || p > RSHS_MAXP) { set_err("p out of range [1,RSHS_MAXP]"); return -1; }
    if (Dmean <= 0.0)           { set_err("mean diameter must be > 0"); return -2; }
    if (srel  <  0.0)           { set_err("relative sigma must be >= 0"); return -3; }
    if (phi   <= 0.0 || phi >= 1.0) { set_err("phi must be in (0,1)"); return -4; }

    sys->p   = p;
    sys->phi = phi;

    if (p == 1 || srel == 0.0) {
        for (i = 0; i < p; i++) {
            sys->D[i] = Dmean;
            sys->x[i] = phi / (p * (M_PI / 6.0) * Dmean * Dmean * Dmean);
        }
        return 0;
    }

    if (!(Rstart > 0.0)) Rstart = 1e-9 * Rend;
    if (!(Rstart > 0.0)) Rstart = 1e-12;

    return classes_from_grid_log(sys, p, Rstart, Rend, phi, pdf, pdf_ctx);
}

int rshs_make_classes(rshs_system *sys, int p, double Dmean, double srel,
                       double phi, rshs_dist_type dist)
{
    static const double moment = 6.0, n_percent = 0.0001;
    meansig_ctx ctx = { Dmean, srel };
    rshs_pdf_fn pdf;
    double Rstart, Rend, D_peak_lo, D_peak_hi, unused;

    switch (dist) {
        case RSHS_DIST_SCHULZ_ZIMM:
            pdf = pdf_schulz_zimm_g;
            D_peak_lo = peak_schulz_zimm(Dmean, srel, 0.0);
            D_peak_hi = peak_schulz_zimm(Dmean, srel, moment);
            Rstart = scan_moment_tail(pdf, &ctx, D_peak_lo, 0.0,    n_percent, -1);
            Rend   = scan_moment_tail(pdf, &ctx, D_peak_hi, moment, n_percent, +1);
            break;
        case RSHS_DIST_LOGNORMAL:
            pdf = pdf_lognormal_g;
            lognorm_moment_range(Dmean, srel, 0.0,    n_percent, &Rstart, &unused);
            lognorm_moment_range(Dmean, srel, moment, n_percent, &unused, &Rend);
            break;
        case RSHS_DIST_GAUSSIAN:
            pdf = pdf_gaussian_g;
            D_peak_lo = peak_gaussian(Dmean, srel, 0.0);
            D_peak_hi = peak_gaussian(Dmean, srel, moment);
            Rstart = scan_moment_tail(pdf, &ctx, D_peak_lo, 0.0,    n_percent, -1);
            Rend   = scan_moment_tail(pdf, &ctx, D_peak_hi, moment, n_percent, +1);
            break;
        case RSHS_DIST_WEIBULL:
            pdf = pdf_weibull_g;
            D_peak_lo = peak_weibull(Dmean, srel, 0.0);
            D_peak_hi = peak_weibull(Dmean, srel, moment);
            Rstart = scan_moment_tail(pdf, &ctx, D_peak_lo, 0.0,    n_percent, -1);
            Rend   = scan_moment_tail(pdf, &ctx, D_peak_hi, moment, n_percent, +1);
            break;
        default: set_err("unknown distribution type"); return -5;
    }

    return rshs_make_classes_from_range(sys, p, Dmean, srel, phi, pdf, &ctx, Rstart, Rend);
}

/* -------------------------------------------------------------------
 * Bimodal lognormal ("sum of two lognormals").
 * ------------------------------------------------------------------- */
typedef struct { double Dmean1, srel1, Dmean2, srel2, w1; } bilognorm_ctx;

static double pdf_bilognormal_g(double D, void *ctx)
{
    bilognorm_ctx *c = ctx;
    return c->w1         * pdf_lognormal(D, c->Dmean1, c->srel1)
         + (1.0 - c->w1) * pdf_lognormal(D, c->Dmean2, c->srel2);
}

int rshs_make_classes_bilognormal(rshs_system *sys, int p,
                                   double Dmean1, double srel1,
                                   double Dmean2, double srel2, double w1,
                                   double phi, double moment, double n_percent)
{
    bilognorm_ctx ctx = { Dmean1, srel1, Dmean2, srel2, w1 };
    double Rlow1, Rhigh1, Rlow2, Rhigh2, Rstart, Rend, unused;

    if (w1 < 0.0 || w1 > 1.0)           { set_err("mode weight w1 must be in [0,1]"); return -8; }
    if (moment < 0.0)                   { set_err("moment must be >= 0"); return -9; }
    if (n_percent <= 0.0 || n_percent >= 100.0) { set_err("n_percent must be in (0,100)"); return -10; }
    if (Dmean1 <= 0.0 || Dmean2 <= 0.0) { set_err("mean diameters must be > 0"); return -2; }
    if (srel1  <  0.0 || srel2  <  0.0) { set_err("relative sigmas must be >= 0"); return -3; }

    if (p == 1) {
        /* crude monodisperse fallback, same spirit as the p==1 case
         * in rshs_make_classes_from_range() above.                   */
        double Dmean = w1 * Dmean1 + (1.0 - w1) * Dmean2;
        if (p < 1 || p > RSHS_MAXP) { set_err("p out of range [1,RSHS_MAXP]"); return -1; }
        if (phi <= 0.0 || phi >= 1.0) { set_err("phi must be in (0,1)"); return -4; }
        sys->p = p; sys->phi = phi;
        sys->D[0] = Dmean;
        sys->x[0] = phi / ((M_PI / 6.0) * Dmean * Dmean * Dmean);
        return 0;
    }

    /* Each mode contributes its own (moment=0) low tail and
     * (moment=6) high tail; Rstart is the smaller of the two low
     * tails, Rend the larger of the two high tails -- same union
     * idea as the original single-sided version, just applied on
     * both ends now. When Dmean1==Dmean2 and srel1==srel2 (the
     * degenerate single-mode case), Rlow1==Rlow2 and Rhigh1==Rhigh2,
     * matching EXACTLY what rshs_make_classes(dist=LOGNORMAL)
     * computes via the same lognorm_moment_range() calls -- and since
     * both then funnel through the identical rshs_make_classes_
     * from_range()/classes_from_grid_log() path, the two produce
     * bit-identical grids/x[]/D[] and therefore identical I(q).       */
    lognorm_moment_range(Dmean1, srel1, 0.0,    n_percent, &Rlow1,  &unused);
    lognorm_moment_range(Dmean1, srel1, moment, n_percent, &unused, &Rhigh1);
    lognorm_moment_range(Dmean2, srel2, 0.0,    n_percent, &Rlow2,  &unused);
    lognorm_moment_range(Dmean2, srel2, moment, n_percent, &unused, &Rhigh2);

    Rstart = (Rlow1  < Rlow2)  ? Rlow1  : Rlow2;
    Rend   = (Rhigh1 > Rhigh2) ? Rhigh1 : Rhigh2;

    return rshs_make_classes_from_range(sys, p, Dmean1, srel1, phi, pdf_bilognormal_g, &ctx, Rstart, Rend);
}

/* =====================================================================
 * 2. PERCUS-YEVICK SOLVE (Eq. 9 of the paper == Perram & Smith 1975)
 * ===================================================================== */

typedef struct {
    int p;
    double tau;
    double h, k2;
    const double *x;
    const double *D;
    double (*r1)[RSHS_MAXP];
    double (*r2)[RSHS_MAXP];
    double (*rr)[RSHS_MAXP];
} pyctx;

/* Backing storage for pyctx's r1/r2/rr and for py_residual()'s lambda/T
 * scratch buffers below. These used to be plain local variables, which
 * was fine while RSHS_MAXP was 25 (a few tens of KB total) but became a
 * real bug once RSHS_MAXP was raised to 250: r1+r2+rr alone are then
 * 3*250*250*8 = ~1.46MB, and py_residual()'s lambda+T add another
 * ~733KB ON TOP of that *for every single GSL solver iteration* (it is
 * called repeatedly from rshs_solve()) -- comfortably overflowing a
 * typical 1MB thread stack (e.g. the Windows/MinGW default) with no
 * diagnostic: the process just terminates, no segfault handler, no
 * error dialog. Moved to thread-local static storage instead, exactly
 * the same fix already applied to sm_workspace/g_smws further below
 * for the same class of problem.                                      */
static __thread double s_pyctx_r1[RSHS_MAXP][RSHS_MAXP];
static __thread double s_pyctx_r2[RSHS_MAXP][RSHS_MAXP];
static __thread double s_pyctx_rr[RSHS_MAXP][RSHS_MAXP];
static __thread double s_py_lambda[RSHS_MAXP * (RSHS_MAXP + 1) / 2];
static __thread double s_py_T[RSHS_MAXP][RSHS_MAXP];

static void py_compute_ab(const pyctx *c, const double *lambda, double *a, double *b)
{
    int i, j;
    for (i = 0; i < c->p; i++) {
        double som = 0.0, xc, b0;
        for (j = 0; j < c->p; j++)
            som += c->x[j] * lambda[PIDX(i, j)] * c->r2[i][j] * c->D[j];
        xc = som * (M_PI / 6.0);
        b0 = 3.0 * c->D[i] * c->k2 / (c->h * c->h) - xc / c->h;
        a[i] = 1.0 / c->h + b0;
        b[i] = -0.5 * b0 * c->D[i];
    }
}

static int py_residual(const gsl_vector *lam_vec, void *params, gsl_vector *f)
{
    pyctx *c = (pyctx *)params;
    int p = c->p;
    int n = p * (p + 1) / 2;
    double *lambda = s_py_lambda;               /* was: local array, see note above pyctx */
    double (*T)[RSHS_MAXP] = s_py_T;             /* was: local array, see note above pyctx */
    double a[RSHS_MAXP], b[RSHS_MAXP];           /* small (2KB each), fine on the stack   */
    int i, j, k;

    for (i = 0; i < n; i++) lambda[i] = gsl_vector_get(lam_vec, i);
    py_compute_ab(c, lambda, a, b);

    for (i = 0; i < p; i++)
        for (k = 0; k < p; k++)
            T[i][k] = -0.5 * a[i] * c->rr[i][k] - b[i] * c->D[k]
                     + lambda[PIDX(i, k)] * c->r2[i][k] / 12.0;

    for (i = 0; i < p; i++) {
        for (j = 0; j <= i; j++) {
            double som = 0.0, val, inv_r1ij = 1.0 / c->r1[i][j];
            for (k = 0; k < p; k++)
                som += c->x[k] * lambda[PIDX(j, k)] * c->r2[j][k] * T[i][k];
            val = som * inv_r1ij * (M_PI / 6.0) + b[i] * inv_r1ij + a[i]
                - c->tau * lambda[PIDX(i, j)];
            gsl_vector_set(f, PIDX(i, j), val);
        }
    }
    return GSL_SUCCESS;
}

/* KINSysFn callback SUNDIALS' KINSOL calls to evaluate the residual --
 * just wraps the existing py_residual() (unchanged, still also used by
 * the GSL fallback path below) around N_Vector's raw buffer via a
 * gsl_vector_view, so both solve paths share the exact same physics/
 * numerics code and can never silently diverge from each other.       */
static int kin_residual_cb(N_Vector uu, N_Vector fval, void *user_data)
{
    pyctx *c = (pyctx *)user_data;
    int n = c->p * (c->p + 1) / 2;
    gsl_vector_view uv = gsl_vector_view_array(N_VGetArrayPointer(uu), n);
    gsl_vector_view fv = gsl_vector_view_array(N_VGetArrayPointer(fval), n);
    py_residual(&uv.vector, c, &fv.vector);
    return 0;
}

/* KINSOL's DEFAULT error handler writes directly to stderr -- entirely
 * separate from, and invisible to, this file's own rshs_last_error().
 * Caught by testing (against SUNDIALS 2.7.0): KINSOL's own diagnostics
 * (e.g. "[KINSOL ERROR] KINSol / The line search algorithm was unable
 * to find an iterate sufficiently distinct from the current
 * iterate.") were appearing on stderr in every run where KINSOL failed
 * to converge, with nothing reaching set_err()/rshs_last_error() at
 * all -- invisible to a typical Tcl-GUI-driven SASfit session, and to
 * anyone calling the plugin programmatically without a visible
 * console.
 *
 * SUNDIALS 7.x moved error handling from a per-solver callback
 * (2.7.0's KINSetErrHandlerFn(kin_mem, ...)) to a per-SUNContext one
 * (SUNContext_PushErrHandler(sunctx, ...), used in rshs_solve() below)
 * with a different callback signature -- this handler and its
 * registration were updated for that when this file was migrated to
 * SUNDIALS 7.x. There is also no longer a KINSetPrintLevel() to worry
 * about: informational verbosity is now controlled by an opt-in
 * SUNLogger that is never created here, so it is silent by default
 * with no equivalent call needed.                                     */
static __thread char g_kin_err[256] = "";
static void kin_err_handler(int line, const char *func, const char *file,
                             const char *msg, SUNErrCode err_code,
                             void *err_user_data, SUNContext sunctx)
{
    (void)line; (void)file; (void)err_code; (void)err_user_data; (void)sunctx;
    snprintf(g_kin_err, sizeof(g_kin_err), "KINSOL %s: %s",
              func ? func : "?", msg ? msg : "?");
}

/* =====================================================================
 * Solves the multicomponent PY lambda(i,j) system (Eq. 9 of the paper).
 *
 * Primary backend: SUNDIALS' KINSOL with matrix-free GMRES (Newton-
 * Krylov). Benchmarked directly against this exact residual across
 * p=10..1000 (n=55..500500 unknowns): convergence stays at a flat 6
 * function evaluations regardless of p, and wall-clock time scales
 * roughly as p^3 (p=1000 solves in ~20s) instead of the p^6-p^8 the
 * old dense derivative-free GSL solver (gsl_multiroot_fsolver_hybrids)
 * cost -- that one became impractical well before p=70. Cross-checked
 * against that old solver's answer at every p where it was still fast
 * enough to run: agreement to 1e-10-1e-14 relative precision, i.e.
 * this is the same physical solution, just reached far faster.
 *
 * An analytic Jacobian (feeding gsl_multiroot_fdfsolver_hybridsj
 * instead of the derivative-free hybrids) was tried first and is
 * genuinely correct (validated against finite differences to 1e-7-
 * 1e-9 absolute precision) -- but it's a net LOSS at exactly the p
 * range that matters: GSL's hybridsj re-evaluates the analytic
 * Jacobian every iteration, and even with sparsity left unexploited
 * that costs ~0.31s per call at p=50 alone, versus 0.0033s for
 * KINSOL's entire solve at that size. Removing the derivative-free
 * Jacobian *estimate* doesn't help when the dominant cost was always
 * the O(n^3) dense factorization every iteration needs regardless of
 * where the Jacobian came from -- KINSOL's Krylov approach is the one
 * that avoids that factorization entirely, which is why it wins.
 *
 * The old GSL solver is kept as an automatic fallback (see below) for
 * robustness -- if KINSOL ever fails to converge on some input this
 * hasn't been tested against, the exact previously-shipped solve path
 * is still there as a safety net, at no cost unless it's actually
 * needed.
 * ===================================================================== */
int rshs_solve(rshs_system *sys, double tau)
{
    int p = sys->p;
    int n = p * (p + 1) / 2;
    pyctx ctx;
    int i, j;
    double alab, bla, dla;

    memset(&ctx, 0, sizeof(ctx));
    ctx.p = p; ctx.tau = tau;
    ctx.x = sys->x; ctx.D = sys->D;
    ctx.r1 = s_pyctx_r1; ctx.r2 = s_pyctx_r2; ctx.rr = s_pyctx_rr;

    sys->tau = tau;
    sys->k2 = 0.0; sys->k3 = 0.0;
    for (i = 0; i < p; i++) {
        sys->k2 += sys->x[i] * sys->D[i] * sys->D[i];
        sys->k3 += sys->x[i] * sys->D[i] * sys->D[i] * sys->D[i];
    }
    sys->k2 *= (M_PI / 6.0);
    sys->k3 *= (M_PI / 6.0);   /* should equal sys->phi already, kept for parity with Fortran */
    sys->h  = 1.0 - sys->k3;
    if (sys->h <= 0.0) { set_err("volume fraction >= 1 (h<=0)"); return -1; }

    ctx.h = sys->h; ctx.k2 = sys->k2;
    for (i = 0; i < p; i++)
        for (j = 0; j < p; j++) {
            ctx.r1[i][j] = 0.5 * (sys->D[i] + sys->D[j]);
            ctx.r2[i][j] = ctx.r1[i][j] * ctx.r1[i][j];
            ctx.rr[i][j] = sys->D[i] * sys->D[j];
        }

    gsl_vector *guess = gsl_vector_alloc(n);

    if (sys->solved && sys->p_solved == p) {
        /* WARM START: reuse the previously converged lambda(i,j) as
         * the initial guess. Consecutive fit iterations only perturb
         * D/srel/phi/tau slightly, so the old solution is typically
         * already very close to the new one and the nonlinear solver
         * needs far fewer iterations than from the generic cold
         * (monodisperse-analytic) guess -- see robertus_shs_core.h
         * for how this gets triggered automatically.                 */
        for (i = 0; i < n; i++) gsl_vector_set(guess, i, sys->lambda[i]);
    } else {
        /* COLD START: monodisperse analytic solution as the initial
         * guess for every lambda(i,j), exactly as in setconst()'s
         * "labdas" branch of the original Fortran.                   */
        bla = sys->k3 / sys->h + tau;
        dla = bla * bla - sys->k3 / 3.0 * (1.0 + 0.5 * sys->k3) / (sys->h * sys->h);
        alab = (dla >= 0.0) ? 6.0 / sys->k3 * (bla - sqrt(dla)) : 6.0 / sys->k3 * bla;
        for (i = 0; i < n; i++) gsl_vector_set(guess, i, alab);
    }

    /* --- Primary: SUNDIALS KINSOL, matrix-free GMRES --------------- */
    {
        SUNContext sunctx;
        N_Vector u, scale;
        void *kin_mem;
        SUNLinearSolver LS;
        int kflag;

        SUNContext_Create(SUN_COMM_NULL, &sunctx);
        /* SUNContext_PushErrHandler() ADDS to a chain of handlers
         * rather than replacing the default one (which still prints to
         * stderr) -- confirmed by testing: without this Clear call, a
         * "[ERROR][rank 0][...] The maximum number of iterations..."
         * line still leaked to stderr even with our handler attached.
         * Clear first so kin_err_handler() is the *only* one active. */
        SUNContext_ClearErrHandlers(sunctx);
        SUNContext_PushErrHandler(sunctx, kin_err_handler, NULL);
        u     = N_VNew_Serial(n, sunctx);
        scale = N_VNew_Serial(n, sunctx);
        for (i = 0; i < n; i++) NV_Ith_S(u, i) = gsl_vector_get(guess, i);
        N_VConst_Serial(1.0, scale);

        kin_mem = KINCreate(sunctx);
        g_kin_err[0] = '\0'; /* clear any stale message from a previous call on this thread */
        KINSetUserData(kin_mem, &ctx);
        KINSetNumMaxIters(kin_mem, 500);
        KINSetFuncNormTol(kin_mem, 1e-9);
        KINSetScaledStepTol(kin_mem, 1e-12);
        KINInit(kin_mem, kin_residual_cb, u);

        /* maxl=0 -> SUNDIALS default Krylov subspace size. This
         * replaces the old KINSpgmr(kin_mem, maxl) direct call (removed
         * in modern SUNDIALS) with the unified SUNLinearSolver object +
         * KINSetLinearSolver() interface introduced in the 3.x/4.x
         * generation and mandatory since; NULL 3rd arg to
         * KINSetLinearSolver means "no Jacobian matrix", appropriate
         * for a matrix-free Krylov method like this one.              */
        LS = SUNLinSol_SPGMR(u, SUN_PREC_NONE, 0, sunctx);
        KINSetLinearSolver(kin_mem, LS, NULL);

        kflag = KINSol(kin_mem, u, KIN_LINESEARCH, scale, scale);

        if (kflag == KIN_SUCCESS || kflag == KIN_INITIAL_GUESS_OK) {
            for (i = 0; i < n; i++) sys->lambda[i] = NV_Ith_S(u, i);
            KINFree(&kin_mem);
            SUNLinSolFree(LS);
            N_VDestroy_Serial(u);
            N_VDestroy_Serial(scale);
            SUNContext_Free(&sunctx);
            gsl_vector_free(guess);

            {
                double a[RSHS_MAXP];
                py_compute_ab(&ctx, sys->lambda, a, sys->b);
            }
            sys->solved = 1;
            sys->p_solved = p;
            set_err("");
            g_solver_note[0] = '\0'; /* no fallback needed, nothing to report */
            return 0;
        }

        /* KINSOL didn't converge -- record why (see rshs_last_solver_note()
         * in the header) before falling through to the GSL fallback
         * below, starting from the same initial guess. If the fallback
         * also fails, this note is simply left unread and overwritten
         * on the next call -- the actual error in that case comes from
         * g_kin_err folded into set_err() further down, not from here.*/
        snprintf(g_solver_note, sizeof(g_solver_note),
                 "rshs_solve(): KINSOL failed to converge (%s) -- used the "
                 "slower GSL fallback solver instead",
                 g_kin_err[0] ? g_kin_err : "no diagnostic captured");
        KINFree(&kin_mem);
        SUNLinSolFree(LS);
        N_VDestroy_Serial(u);
        N_VDestroy_Serial(scale);
        SUNContext_Free(&sunctx);
    }

    /* --- Fallback: the original dense derivative-free GSL solver --- */
    {
        int status, iter, maxiter = 500;
        gsl_multiroot_function F = { &py_residual, n, &ctx };
        const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_hybrids;
        gsl_multiroot_fsolver *s = gsl_multiroot_fsolver_alloc(T, n);
        gsl_multiroot_fsolver_set(s, &F, guess);

        status = GSL_CONTINUE;
        for (iter = 0; iter < maxiter && status == GSL_CONTINUE; iter++) {
            status = gsl_multiroot_fsolver_iterate(s);
            if (status) break; /* solver stuck (e.g. singular Jacobian) */
            status = gsl_multiroot_test_residual(s->f, 1e-9);
        }

        if (status != GSL_SUCCESS) {
            /* Fold in whatever KINSOL's error handler captured (see
             * kin_err_handler() above) instead of only the generic
             * message -- both solvers failed, so the more specific
             * diagnostic is worth surfacing rather than discarding.   */
            char buf[400];
            snprintf(buf, sizeof(buf),
                     "no real solution found for lambda(i,j) (try larger tau, "
                     "or note tau may be below tau_c at this density)%s%s",
                     g_kin_err[0] ? " -- " : "", g_kin_err);
            set_err(buf);
            gsl_vector_free(guess);
            gsl_multiroot_fsolver_free(s);
            return -2;
        }

        for (i = 0; i < n; i++) sys->lambda[i] = gsl_vector_get(s->x, i);
        {
            double a[RSHS_MAXP];
            py_compute_ab(&ctx, sys->lambda, a, sys->b);
        }
        sys->solved = 1;
        sys->p_solved = p;
        set_err("");

        gsl_vector_free(guess);
        gsl_multiroot_fsolver_free(s);
        return 0;
    }
}

/* =====================================================================
 * 3. Q~(i,k)(q) MATRIX AND STRUCTURE FACTOR  (Eqs. 10 & 17)
 * ===================================================================== */

/* -------------------------------------------------------------------
 * PERFORMANCE workspace, shared by rshs_structure_matrix() and the
 * fast bilinear-form path used internally by rshs_intensity_*() (see
 * bilinear_form() further below). Sized once at RSHS_MAXP x RSHS_MAXP
 * and reused via p x p submatrix views on every call, instead of
 * allocating fresh GSL objects every q-point.
 *
 * (Earlier versions of this file justified this purely on avoiding
 * malloc/free churn; direct profiling later showed that churn is in
 * fact negligible here -- alloc+free of these objects costs ~0.25us
 * regardless of p, versus tens of microseconds for the linear algebra
 * itself. The buffer reuse is kept anyway since it's free and mildly
 * cache-friendlier, but see bilinear_form() for where the real
 * >3x speedup for rshs_intensity_*()/rshs_avg_structure_factor_*()
 * comes from.)
 *
 * This makes every function using this workspace NOT thread-safe;
 * see the header for guidance if you need concurrent evaluation
 * (short version: it's __thread, so it's fine across OpenMP threads,
 * just not if you hand multiple OS threads the same rshs_system and
 * call from all of them without your own locking).
 * ------------------------------------------------------------------- */
typedef struct {
    int initialised;
    gsl_matrix_complex *Q, *QT, *Binv, *S;
    gsl_permutation *perm;
    gsl_vector_complex *v, *rhs;
} sm_workspace;

static __thread sm_workspace g_smws = {0};

static void sm_workspace_ensure(void)
{
    if (g_smws.initialised) return;
    g_smws.Q    = gsl_matrix_complex_alloc(RSHS_MAXP, RSHS_MAXP);
    g_smws.QT   = gsl_matrix_complex_alloc(RSHS_MAXP, RSHS_MAXP);
    g_smws.Binv = gsl_matrix_complex_alloc(RSHS_MAXP, RSHS_MAXP);
    g_smws.S    = gsl_matrix_complex_alloc(RSHS_MAXP, RSHS_MAXP);
    g_smws.perm = gsl_permutation_alloc(RSHS_MAXP);
    g_smws.v    = gsl_vector_complex_alloc(RSHS_MAXP);
    g_smws.rhs  = gsl_vector_complex_alloc(RSHS_MAXP);
    g_smws.initialised = 1;
}

/* Builds the p x p complex Q~(i,k)(q) matrix of Eq. 10 into the
 * caller-given (already appropriately sized/viewed) matrix Qout.     */
static void build_Q_matrix(const rshs_system *sys, double q, gsl_matrix_complex *Qout)
{
    int p = sys->p, i, k;
    double vrx[RSHS_MAXP], sqv[RSHS_MAXP], cqv[RSHS_MAXP], vrphi[RSHS_MAXP], vrpsi[RSHS_MAXP];
    double sqrtx[RSHS_MAXP];

    /* Fix (found and validated during testing, see project documentation
     * for how this was found -- the same underlying pattern already
     * fixed in f_sphere() above): vrphi=3(sin(x)-x*cos(x))/x^3 and
     * vrpsi=sin(x)/x both suffer catastrophic cancellation as their
     * argument x=vrx[i]=0.5*D[i]*q shrinks, same as f_sphere's own
     * sin(qR)-qR*cos(qR). The previous single check, "if (q<1e-4)",
     * used an absolute threshold on q alone, ignoring D[i] entirely --
     * this is wrong in BOTH directions. For a large component diameter
     * D[i], vrx[i]=0.5*D[i]*q can still be substantial even when q is
     * below the old 1e-4 threshold, so the old code's "snap to
     * vrphi=vrpsi=1.0" branch fired incorrectly there: confirmed
     * directly (D=200000, q=5e-5, vrx=5.0) that the true values
     * (vrphi=-0.057, vrpsi=-0.192) are qualitatively different from 1.0,
     * not just a small correction -- a real bug, not merely lost
     * precision. Conversely, for a small D[i], vrx[i] can still be
     * small enough to risk cancellation in the direct trig formula even
     * once q comfortably exceeds 1e-4. Fixed here with a per-component
     * check on vrx[i] itself (matching f_sphere's own tested qR<0.5
     * crossover, confirmed separately against a 50-digit mpmath
     * reference to keep both vrphi and vrpsi accurate to ~1e-8 relative
     * error or better up to x=0.5, consistent with the direct trig
     * formula's own accuracy from that point on), with a degree-3
     * Taylor series in x^2 for each:
     *   vrphi = 1 - x^2/10 + x^4/280 - x^6/15120 + ...
     *   vrpsi = 1 - x^2/6  + x^4/120 - x^6/5040  + ...
     */
    for (i = 0; i < p; i++) {
        vrx[i] = 0.5 * sys->D[i] * q;
        if (fabs(vrx[i]) < 0.5) {
            double x2 = vrx[i]*vrx[i];
            sqv[i] = sin(vrx[i]);
            cqv[i] = cos(vrx[i]);
            vrphi[i] = 1.0 - x2/10.0 + x2*x2/280.0 - x2*x2*x2/15120.0;
            vrpsi[i] = 1.0 - x2/6.0  + x2*x2/120.0 - x2*x2*x2/5040.0;
        } else {
            sqv[i] = sin(vrx[i]);
            cqv[i] = cos(vrx[i]);
            vrphi[i] = 3.0 / (vrx[i]*vrx[i]*vrx[i]) * (sqv[i] - vrx[i]*cqv[i]);
            vrpsi[i] = sqv[i] / vrx[i];
        }
    }
    /* sqrt(x_i*x_k) = sqrt(x_i)*sqrt(x_k): p sqrt() calls instead of p^2. */
    for (i = 0; i < p; i++) sqrtx[i] = sqrt(sys->x[i]);

    for (i = 0; i < p; i++) {
        for (k = 0; k < p; k++) {
            double re, im, pref;
            gsl_complex z, phase, val;

            re =  -0.25 * sys->lambda[PIDX(i,k)] * sys->D[k]
                        * (sys->D[i]+sys->D[k]) * (sys->D[i]+sys->D[k]) * vrpsi[k]
                  + sys->D[k]*sys->D[k]*sys->D[k] / sys->h * vrphi[k]
                  + sys->D[k]*sys->D[k]*sys->D[i] / sys->h * 3.0 * vrpsi[k]
                  - 2.0 * sys->D[k]*sys->D[k]*sys->D[k] * sys->b[i] / sys->D[i] * vrphi[k];
            im = -sys->D[i]*sys->D[k]*sys->D[k] / sys->h * vrx[k] * vrphi[k];

            pref = (M_PI/6.0) * sqrtx[i] * sqrtx[k];
            z     = gsl_complex_rect(pref*re, pref*im);
            phase = gsl_complex_rect(cqv[i], sqv[i]);
            val   = gsl_complex_mul(z, phase);
            if (i == k) val = gsl_complex_add_real(val, 1.0);
            gsl_matrix_complex_set(Qout, i, k, val);
        }
    }
}

/* p<=RSHS_MAXP; GSL permutations have no public resize call, but the
 * struct (gsl_permutation{size_t size; size_t *data;}) is public API
 * and perm->data already has RSHS_MAXP capacity, so this is safe.    */
static void perm_resize(gsl_permutation *perm, int p) { perm->size = (size_t)p; }

int rshs_structure_matrix(const rshs_system *sys, double q,
                           double Sij[RSHS_MAXP][RSHS_MAXP])
{
    int p = sys->p, i, k, signum;
    if (!sys->solved) { set_err("system not solved (call rshs_solve first)"); return -1; }

    sm_workspace_ensure();
    gsl_matrix_complex_view Qv = gsl_matrix_complex_submatrix(g_smws.Q, 0, 0, p, p);
    gsl_matrix_complex *Q = &Qv.matrix;
    build_Q_matrix(sys, q, Q);

    gsl_permutation *perm = g_smws.perm;
    perm_resize(perm, p);
    gsl_matrix_complex_view Binvv = gsl_matrix_complex_submatrix(g_smws.Binv, 0, 0, p, p);
    gsl_matrix_complex *Binv = &Binvv.matrix;
    if (gsl_linalg_complex_LU_decomp(Q, perm, &signum) != GSL_SUCCESS) {
        set_err("LU decomposition of Q matrix failed");
        return -2;
    }
    gsl_linalg_complex_LU_invert(Q, perm, Binv); /* Binv = Q^{-1} */

    /* S = Binv * Binv^H  (Hermitian-conjugate transpose)               */
    gsl_matrix_complex_view Sv = gsl_matrix_complex_submatrix(g_smws.S, 0, 0, p, p);
    gsl_matrix_complex *S = &Sv.matrix;
    gsl_blas_zgemm(CblasNoTrans, CblasConjTrans,
                    gsl_complex_rect(1.0,0.0), Binv, Binv,
                    gsl_complex_rect(0.0,0.0), S);

    for (i = 0; i < p; i++)
        for (k = 0; k < p; k++)
            Sij[i][k] = GSL_REAL(gsl_matrix_complex_get(S, i, k));

    return 0;
}

/* =====================================================================
 * FAST PATH for the bilinear form  sum_ij a_i a_j S_ij(q)  (a_i real),
 * which is all that rshs_intensity_*() / rshs_avg_structure_factor_*()
 * actually need -- they never need the individual S_ij themselves.
 *
 * Derivation: with S = B B^H (B = Q^{-1}, as above),
 *   sum_ij a_i a_j S_ij = sum_ij a_i a_j sum_k B_ik * conj(B_jk)
 *                        = sum_k | sum_i a_i B_ik |^2
 *                        = sum_k | v_k |^2 = ||v||^2,   v := B^T a
 * and since B^T = (Q^{-1})^T = (Q^T)^{-1}, v solves  Q^T v = a.
 *
 * So instead of the full inverse Binv=Q^{-1} (an O(p^3) operation
 * solving p right-hand sides) followed by a full O(p^3) matrix
 * product Binv*Binv^H, we need only ONE O(p^3) LU decomposition (of
 * Q^T, i.e. Q with i,k swapped -- NOT conjugated) and ONE O(p^2)
 * triangular solve for a single right-hand side, then an O(p) sum of
 * squared magnitudes. Measured on this machine (p=25): the old route
 * costs ~78us (12 decomp + 31 invert + 35 gemm); this route costs
 * ~13-15us (12 decomp + ~1 solve + ~0 norm) -- about 5-6x faster, and
 * the advantage grows with p since it eliminates two of the three
 * O(p^3) operations entirely rather than just speeding them up.
 * ===================================================================== */
static double bilinear_form(const rshs_system *sys, double q, const double *a)
{
    int p = sys->p, i, k, signum;

    sm_workspace_ensure();
    gsl_matrix_complex_view Qv = gsl_matrix_complex_submatrix(g_smws.Q, 0, 0, p, p);
    gsl_matrix_complex *Q = &Qv.matrix;
    build_Q_matrix(sys, q, Q);

    /* QT = transpose(Q) (plain transpose, NOT conjugate -- see derivation) */
    gsl_matrix_complex_view QTv = gsl_matrix_complex_submatrix(g_smws.QT, 0, 0, p, p);
    gsl_matrix_complex *QT = &QTv.matrix;
    for (i = 0; i < p; i++)
        for (k = 0; k < p; k++)
            gsl_matrix_complex_set(QT, i, k, gsl_matrix_complex_get(Q, k, i));

    gsl_vector_complex_view rhsv = gsl_vector_complex_subvector(g_smws.rhs, 0, p);
    gsl_vector_complex_view vv   = gsl_vector_complex_subvector(g_smws.v, 0, p);
    for (i = 0; i < p; i++) gsl_vector_complex_set(&rhsv.vector, i, gsl_complex_rect(a[i], 0.0));

    gsl_permutation *perm = g_smws.perm;
    perm_resize(perm, p);
    if (gsl_linalg_complex_LU_decomp(QT, perm, &signum) != GSL_SUCCESS) {
        set_err("LU decomposition of Q^T matrix failed");
        return 0.0;
    }
    gsl_linalg_complex_LU_solve(QT, perm, &rhsv.vector, &vv.vector);

    double total = 0.0;
    for (k = 0; k < p; k++) {
        gsl_complex vk = gsl_vector_complex_get(&vv.vector, k);
        total += GSL_REAL(vk)*GSL_REAL(vk) + GSL_IMAG(vk)*GSL_IMAG(vk);
    }
    set_err("");
    return total;
}

/* =====================================================================
 * 4. FORM FACTORS
 * ===================================================================== */

static double f_sphere(double q, double R, double drho)
{
    double qR, qR2, series;
    if (R <= 0.0) return 0.0;
    qR = q * R;
    /* Fix (found and validated during testing, see project documentation
     * for how this was found): sin(qR)-qR*cos(qR) and q^3 both vanish as
     * q->0, and the previous single hard threshold at q<1e-7 (switching
     * directly to the exact q=0 limit, (4/3)*pi*drho*R^3) left a real
     * precision-loss gap for q values ABOVE that threshold but still
     * small enough for qR-cos(qR) to suffer catastrophic cancellation --
     * confirmed directly against a 50-digit mpmath reference: the
     * relative error was already 6.5e-6 right at the old threshold
     * itself (q=1e-7), progressively worse for even smaller q that
     * still exceeded it slightly, before the old code's own q<1e-7
     * branch took over. A degree-4 Taylor series in qR (of
     * sin(x)-x*cos(x) = x^3/3 - x^5/30 + x^7/840 - x^9/45360 + ...,
     * divided through by q^3 = x^3/R^3) closes this gap: confirmed
     * against the same 50-digit reference to give relative error at or
     * below machine precision (~1e-16) for qR up to ~0.05, still
     * excellent (~3e-9) at qR=0.5, and diverging badly beyond that
     * (~2e-3 at qR=1.5) -- so qR<0.5 is used here as a safe, tested
     * crossover, matched on the other side by the direct formula's own
     * error also being at machine precision there (confirmed
     * separately), leaving no gap where neither branch is accurate.
     */
    if (fabs(qR) < 0.5) {
        qR2 = qR*qR;
        series = 1.0/3.0 - qR2/30.0 + qR2*qR2/840.0 - qR2*qR2*qR2/45360.0;
        return 4.0 * M_PI * drho * R*R*R * series;
    }
    return 4.0 * M_PI * drho * (sin(qR) - qR*cos(qR)) / (q*q*q);
}

double rshs_ff_coreshell(double q, double Rhs, double t_shell,
                          double rho_core, double rho_shell, double rho_solv)
{
    double Rcore = Rhs - t_shell;
    if (Rcore < 0.0) Rcore = 0.0;
    return f_sphere(q, Rcore, rho_core - rho_shell)
         + f_sphere(q, Rhs,   rho_shell - rho_solv);
}

double rshs_ff_fuzzy(double q, double Rhs, double sigma_fuzzy,
                      double rho_particle, double rho_solv)
{
    return f_sphere(q, Rhs, rho_particle - rho_solv)
         * exp(-0.5 * q*q * sigma_fuzzy*sigma_fuzzy);
}

/* =====================================================================
 * 5. MULTICOMPONENT INTENSITY (Eq. 14) AND AVERAGE S(q) (Eq. 18)
 * ===================================================================== */

static double intensity_generic_impl(const rshs_system *sys, double q,
                                      rshs_ff_amplitude_fn ff, void *ffparams)
{
    int p = sys->p, i;
    double a[RSHS_MAXP]; /* a_i = sqrt(x_i) * F_i(q), so that
                             sum_ij a_i a_j S_ij == sum_ij sqrt(x_i x_j)
                             F_i F_j S_ij, i.e. exactly Eq. 14         */

    if (!sys->solved) { set_err("system not solved (call rshs_solve first)"); return 0.0; }
    for (i = 0; i < p; i++) a[i] = sqrt(sys->x[i]) * ff(q, 0.5*sys->D[i], ffparams);

    return bilinear_form(sys, q, a); /* see bilinear_form() for why this
                                         replaces the old "build full
                                         Sij matrix, then double loop"
                                         approach -- same result, ~5x
                                         faster.                       */
}

double rshs_intensity_generic(const rshs_system *sys, double q,
                               rshs_ff_amplitude_fn ff, void *ff_ctx)
{
    return intensity_generic_impl(sys, q, ff, ff_ctx);
}

typedef struct { double t_shell, rho_core, rho_shell, rho_solv; } cs_params;
static double ff_cs(double q, double R, void *vp)
{
    cs_params *p = (cs_params *)vp;
    return rshs_ff_coreshell(q, R, p->t_shell, p->rho_core, p->rho_shell, p->rho_solv);
}

typedef struct { double sigma_fuzzy, rho_particle, rho_solv; } fz_params;
static double ff_fz(double q, double R, void *vp)
{
    fz_params *p = (fz_params *)vp;
    return rshs_ff_fuzzy(q, R, p->sigma_fuzzy, p->rho_particle, p->rho_solv);
}

double rshs_intensity_coreshell(const rshs_system *sys, double q,
                                 double t_shell, double rho_core,
                                 double rho_shell, double rho_solv)
{
    cs_params pp = { t_shell, rho_core, rho_shell, rho_solv };
    return intensity_generic_impl(sys, q, ff_cs, &pp);
}

double rshs_intensity_fuzzy(const rshs_system *sys, double q,
                             double sigma_fuzzy, double rho_particle,
                             double rho_solv)
{
    fz_params pp = { sigma_fuzzy, rho_particle, rho_solv };
    return intensity_generic_impl(sys, q, ff_fz, &pp);
}

/* average <F(q)> over classes weighted by x_i, and average <F(q)^2>,
 * needed for the Eq.18 average structure factor S~(q).                */
static double avg_S_generic_impl(const rshs_system *sys, double q,
                                  rshs_ff_amplitude_fn ff, void *ffparams)
{
    int p = sys->p, i;
    double den, a;
    double I = intensity_generic_impl(sys, q, ff, ffparams);
    if (rshs_last_error()[0] != '\0') return 0.0;

    den = 0.0;
    for (i = 0; i < p; i++) {
        a = ff(q, 0.5*sys->D[i], ffparams);
        den += a*a*sys->x[i];
    }
    if (den <= 0.0) { set_err("zero average form factor at this q"); return 0.0; }
    return I / den;
}

double rshs_avg_structure_factor_generic(const rshs_system *sys, double q,
                                          rshs_ff_amplitude_fn ff, void *ff_ctx)
{
    return avg_S_generic_impl(sys, q, ff, ff_ctx);
}

double rshs_avg_structure_factor_coreshell(const rshs_system *sys, double q,
                                            double t_shell, double rho_core,
                                            double rho_shell, double rho_solv)
{
    cs_params pp = { t_shell, rho_core, rho_shell, rho_solv };
    return avg_S_generic_impl(sys, q, ff_cs, &pp);
}

double rshs_avg_structure_factor_fuzzy(const rshs_system *sys, double q,
                                        double sigma_fuzzy, double rho_particle,
                                        double rho_solv)
{
    fz_params pp = { sigma_fuzzy, rho_particle, rho_solv };
    return avg_S_generic_impl(sys, q, ff_fz, &pp);
}

/* =====================================================================
 * Batch q-array intensity, for a whole scattering curve in one call.
 * Each q is independent, so with OpenMP (-fopenmp) this parallelises
 * across q-points; without it, it is a plain loop (still benefits
 * from the workspace reuse already built into rshs_structure_matrix).
 * ===================================================================== */
int rshs_intensity_coreshell_batch(const rshs_system *sys, const double *q, int nq,
                                    double *I_out, double t_shell, double rho_core,
                                    double rho_shell, double rho_solv)
{
    int iq;
    if (!sys->solved) { set_err("system not solved (call rshs_solve first)"); return -1; }
#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (iq = 0; iq < nq; iq++)
        I_out[iq] = rshs_intensity_coreshell(sys, q[iq], t_shell, rho_core, rho_shell, rho_solv);
    return 0;
}

int rshs_intensity_fuzzy_batch(const rshs_system *sys, const double *q, int nq,
                                double *I_out, double sigma_fuzzy,
                                double rho_particle, double rho_solv)
{
    int iq;
    if (!sys->solved) { set_err("system not solved (call rshs_solve first)"); return -1; }
#ifdef _OPENMP
    #pragma omp parallel for schedule(static)
#endif
    for (iq = 0; iq < nq; iq++)
        I_out[iq] = rshs_intensity_fuzzy(sys, q[iq], sigma_fuzzy, rho_particle, rho_solv);
    return 0;
}
