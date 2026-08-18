/*
 * robertus_shs_core.h
 *
 * Multicomponent Percus-Yevick adhesive (sticky) hard-sphere engine,
 * after:
 *   C. Robertus, W.H. Philipse, J.G.H. Joosten, Y.K. Levine,
 *   "Solution of the Percus-Yevick approximation of the multicomponent
 *    adhesive spheres system applied to the small angle x-ray scattering
 *    from microemulsions", J. Chem. Phys. 90(8), 4482 (1989).
 *
 * This file re-implements, in portable C + GSL, the numerical core of
 * the original Fortran-77 program (MAIN_HB/LABDA_HB/SUB_HB/UTILS,
 * C. Robertus) that was distributed with the paper:
 *   - LABDA_HB : discretised size distribution + solution of the
 *                quadratic Percus-Yevick equations for lambda(i,j)
 *                (originally via IMSL ZSPOW == MINPACK hybrd; here via
 *                gsl_multiroot_fsolver_hybrids, its GSL equivalent).
 *   - SUB_HB   : the Q~(i,k)(q) matrix (Eq. 10 of the paper), its
 *                complex inversion (originally IMSL D_LEQT1C; here GSL
 *                complex LU), and the resulting partial structure
 *                factors S_nm(q) and scattered intensity I(q) (Eqs.
 *                14-18 of the paper).
 *
 * Extensions relative to the original code:
 *   - four selectable size distributions (Schulz-Zimm, LogNormal,
 *     Gaussian, Weibull), all parameterised uniformly by a mean
 *     diameter and a *relative* width sigma/D (same convention as
 *     sigma/D used throughout the paper), instead of only Schulz;
 *   - two selectable particle form factors: a sharp core-shell sphere
 *     and a sphere with a diffuse ("fuzzy") interface, instead of the
 *     IFORM=1..10 zoo in SUB_HB;
 *   - a fully GENERIC mode (rshs_make_classes_generic(),
 *     rshs_intensity_generic(), rshs_avg_structure_factor_generic()):
 *     the size-distribution PDF and the per-class scattering-amplitude
 *     function are plain callbacks, so the four distributions and two
 *     form factors above are just the built-in special cases -- any
 *     other size distribution or (centrosymmetric) particle form
 *     factor can be wired in without touching the Percus-Yevick solver
 *     or the structure-factor machinery at all. This is what lets the
 *     SASfit plugin wrapper (sasfit_pl_robertus_shs.c) adapt SASfit's
 *     own sasfit_sd_*(x,param) / sasfit_ff_*_f(q,param) plugin
 *     functions into this engine -- see the "GENERIC / composability"
 *     section below and the worked example in the wrapper source.
 *
 * As in the paper, the stickiness tau is taken independent of particle
 * size (tau_nm = tau for all pairs) -- this is the simplifying
 * assumption the paper itself adopts for all numerical results.
 */
#ifndef ROBERTUS_SHS_CORE_H
#define ROBERTUS_SHS_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of size classes the fixed-size arrays below support.
 * The paper notes p>=9 is needed for numerical convergence and that
 * increasing p beyond ~11-19 buys little extra accuracy but costs
 * compute time (the lambda-system has p(p+1)/2 unknowns) -- but see
 * below, that cost concern is now largely moot.
 *
 * History: originally 25, raised to 80 when rshs_solve() still used
 * GSL's dense derivative-free hybrid solver (gsl_multiroot_fsolver_
 * hybrids), whose cost grows roughly as p^6-p^8 (a numerically-
 * estimated dense Jacobian, O(n^3) QR factorization/update every
 * iteration, n=p(p+1)/2) -- p=70 (2485 unknowns) already didn't finish
 * in 90s under that solver.
 *
 * rshs_solve() now uses SUNDIALS' KINSOL with matrix-free GMRES
 * instead (see the big comment above rshs_solve() in robertus_shs_
 * core.c for why). Benchmarked directly against this exact residual:
 * convergence stays at a flat 6 function evaluations from p=10 all
 * the way to p=1000 (500500 unknowns), with wall-clock time scaling
 * roughly as p^3 instead of p^6-p^8, verified to agree with the old
 * GSL solver's answer to 1e-10-1e-14 relative precision at every p
 * where the old solver was still fast enough to cross-check.
 *
 * p=1000 itself solves in ~20s -- correct, but too long to wait for
 * in practice, so the ceiling is set to 300 instead (solves in well
 * under 1s) rather than the largest value that was actually tested
 * working. Raise this again later if a genuine use case needs more
 * and the wait is acceptable; the solver itself has no problem with
 * larger p, this is purely a "how long are you willing to wait" knob.
 *
 * Memory note: several buffers below are still sized RSHS_MAXP x
 * RSHS_MAXP (thread-local, see s_pyctx_r1/r2/rr and s_py_T in
 * robertus_shs_core.c), so this is not entirely free even at 300: on
 * the order of a few MB per thread for the PY solve's own scratch
 * space, plus more (lazily allocated, also per thread) the first time
 * rshs_intensity_*()/rshs_structure_matrix() is called at a large p.
 * Fine for ordinary use; worth knowing if this is ever raised further
 * or used heavily under many OpenMP threads at once.                  */
#define RSHS_MAXP 300

typedef enum {
    RSHS_DIST_SCHULZ_ZIMM  = 1,
    RSHS_DIST_LOGNORMAL    = 2,
    RSHS_DIST_GAUSSIAN     = 3,
    RSHS_DIST_WEIBULL      = 4,
    RSHS_DIST_BILOGNORMAL  = 5   /* sum of two lognormals, see
                                  * rshs_make_classes_bilognormal() below */
} rshs_dist_type;

/* IMPORTANT: rshs_system must be zero-initialised before its first
 * use (e.g. `rshs_system sys = {0};`, or by putting it in static/
 * global storage, or via calloc) so that the warm-start bookkeeping
 * fields (solved, p_solved) start from a well-defined "nothing solved
 * yet" state. rshs_make_classes() intentionally leaves lambda[]/b[]/
 * solved/p_solved untouched across calls (that is what makes the
 * warm start in rshs_solve() possible) so it cannot establish this
 * initial zero state on your behalf.                                 */
typedef struct {
    int    p;                                   /* number of size classes actually used   */
    double x[RSHS_MAXP];                         /* number density of class i (sums to give phi via 3rd moment) */
    double D[RSHS_MAXP];                         /* hard-sphere (interaction) diameter of class i */
    double lambda[RSHS_MAXP*(RSHS_MAXP+1)/2];    /* solved PY lambda(i,j), packed i>=j      */
    double b[RSHS_MAXP];                         /* Baxter/Perram-Smith b(i) coefficients   */
    double tau;                                  /* stickiness (same for all pairs)         */
    double phi;                                  /* hard-sphere volume fraction (xi3)       */
    double h;                                    /* 1 - phi                                 */
    double k2;                                   /* xi2 = (pi/6) sum x_i D_i^2               */
    double k3;                                   /* xi3 = phi                                */
    int    solved;                               /* 1 once rshs_solve() has succeeded        */
    int    p_solved;                             /* p at the time lambda[] was last validly
                                                    solved; used to auto-detect a safe warm
                                                    start (see rshs_solve() below).           */
} rshs_system;

/* --- error reporting ---------------------------------------------- */
/* Returns a short human readable string for the last error that
 * occurred in this translation unit (not thread safe, diagnostic use
 * only -- mirrors the way the original Fortran wrote messages to
 * stdout on failure to converge / non pos.def. matrices, etc).       */
const char *rshs_last_error(void);

/* Returns a non-fatal diagnostic note about the *last successful*
 * rshs_solve() call, or an empty string if there's nothing to report.
 * Currently the only thing this reports: rshs_solve()'s primary
 * backend (SUNDIALS KINSOL) failed to converge and the automatic GSL
 * fallback was used instead to still produce a correct answer -- see
 * the big comment above rshs_solve() in robertus_shs_core.c. That is
 * NOT an error (the result is still correct), so it does not go
 * through rshs_last_error()/sasfit_err() -- but a caller that wants to
 * know when the (much slower) fallback path is being taken can check
 * here. Plugin wrapper functions should surface a non-empty result via
 * sasfit_out() (the informational channel), not sasfit_err().
 *
 * One-shot: reading this clears it, so polling it after every q-point
 * of a cached/warm-started evaluation only reports the fallback once,
 * right after the actual solve that needed it -- not on every
 * subsequent cache hit at the same parameters.                        */
const char *rshs_last_solver_note(void);

/* Writes the current p-class size-distribution discretisation (the
 * D[i], x[i] pairs produced by rshs_make_classes()/rshs_make_classes_
 * bilognormal(), i.e. exactly the grid rshs_solve() then works with)
 * to a plain-text ASCII file, one line per class, tab-separated.
 * Mainly a diagnostic: e.g. to directly inspect whether the adaptive
 * log-spaced grid (see classes_from_grid_log() in robertus_shs_core.c)
 * actually straddles both modes of a bimodal distribution at a given
 * nclass, rather than inferring it indirectly from I(q).
 *
 * Exported as RADIUS (D[i]/2), not diameter, matching the convention
 * already used wherever this grid feeds into a form factor elsewhere
 * in this file (e.g. ff(q, 0.5*sys->D[i], ctx)). Internally sys->D[i]
 * itself remains a diameter -- rshs_solve()'s Percus-Yevick contact-
 * distance formulas (e.g. r1[i][j]=0.5*(D_i+D_j)) are fundamentally
 * diameter-based, so only this export's column changed, not the
 * underlying rshs_system representation.
 *
 * If sys->solved is true (i.e. called after rshs_solve()), the file
 * also includes each class's Baxter b[i] coefficient as a 4th column.
 * Overwrites filename if it already exists.
 *
 * Returns 0 on success, or a negative code (see rshs_last_error()) if
 * the file could not be opened for writing. Pure C, no SASfit
 * dependency -- see also the private.h SASFIT_ROBERTUS_EXPORT_GRID
 * environment-variable hook, which calls this automatically from the
 * plugin's cache-refresh path whenever the grid is freshly rebuilt.  */
int rshs_export_classes(const rshs_system *sys, const char *filename);

/* --- 1. size distribution ------------------------------------------
 *
 * Build the p-class discretisation of the chosen size distribution.
 * Dmean  : number-weighted mean hard-sphere diameter
 * srel   : relative width, i.e. sigma/Dmean (the "sigma/D" of the paper)
 * phi    : target hard-sphere volume fraction (xi3)
 * dist   : which distribution to discretise
 *
 * The distribution is sampled at p equally spaced diameters spanning
 * Dmean +/- 3*sigma (same range as the original Schulz-only code),
 * weighted by w_i = pdf(D_i)*dr, and then renormalised numerically so
 * that (pi/6) * sum_i x_i * D_i^3 == phi exactly (this generalises the
 * paper's analytic Schulz-only normalisation to any distribution).
 *
 * Returns 0 on success, <0 on invalid input (see rshs_last_error()).
 */
int rshs_make_classes(rshs_system *sys, int p, double Dmean, double srel,
                       double phi, rshs_dist_type dist);

/* --- 1b. GENERIC size distribution -----------------------------------
 *
 * Same discretisation-and-renormalisation scheme as rshs_make_classes()
 * above, but the probability density itself is a callback instead of
 * one of the 4 built-ins. This is what rshs_make_classes() is actually
 * implemented in terms of.
 *
 * pdf(x, ctx) must return the (not necessarily normalised -- it gets
 * renormalised numerically anyway) probability density at diameter x.
 * ctx is passed through unchanged; its meaning is entirely up to the
 * caller (e.g. it might be a small {Dmean,srel} struct for a
 * standalone C caller, or a SASfit sasfit_param* when called from the
 * plugin wrapper -- see rshs_pdf_fn below and the wrapper source for
 * a worked example).
 */
typedef double (*rshs_pdf_fn)(double x, void *ctx);

int rshs_make_classes_generic(rshs_system *sys, int p, double Dmean, double srel,
                               double phi, rshs_pdf_fn pdf, void *pdf_ctx);

/* --- 1d. Bimodal lognormal ("sum of two lognormals") ------------------
 *
 * pdf(D) = w1*LogNormal(D;Dmean1,srel1) + (1-w1)*LogNormal(D;Dmean2,srel2)
 *
 * Unlike rshs_make_classes()/rshs_make_classes_generic() above, which
 * always span a *fixed* window Dmean +/- 3*sigma regardless of how the
 * scattering weights the tail, this variant sizes the class grid
 * *adaptively*: it finds, for each of the two lognormal modes, the
 * diameter at which the D^moment-weighted pdf (moment=6 matches the
 * ~D^6 forward-scattering weighting of typical SAS form factors) has
 * decayed to n_percent of its (shifted) peak, and grids [0, Rend] with
 * Rend = the larger of the two modes' tails. This mirrors the analytic
 * approach SASfit's own find_integration_range() (src/sasfit_old/
 * sasfit.c) uses for its LogNorm/BiLogNorm size distributions, so that
 * a small-weight, large-size second mode is not clipped just because
 * it sits far from Dmean1 in absolute terms -- exactly the situation
 * where a fixed +/-3*sigma window around a single mean would fail.
 *
 * n_percent : percentage of the moment-weighted peak at which the
 *             tail is considered negligible; SASfit's own
 *             find_integration_range() hardcodes 0.0001 -- pass the
 *             same value unless you have a specific reason not to.
 *
 * Returns 0 on success, <0 on invalid input (see rshs_last_error()),
 * on the same convention as rshs_make_classes_generic() above.
 */
int rshs_make_classes_bilognormal(rshs_system *sys, int p,
                                   double Dmean1, double srel1,
                                   double Dmean2, double srel2, double w1,
                                   double phi, double moment, double n_percent);

/* --- 1c. GENERIC per-class scattering amplitude ----------------------
 *
 * Real, centrosymmetric-density-profile scattering amplitude F(q) for
 * a particle of "size" D (whatever quantity rshs_make_classes[_generic]
 * distributed over classes -- for a sphere this is the radius, for a
 * core-shell sphere the outer/hard-sphere radius, etc; must match the
 * convention rshs_ff_coreshell()/rshs_ff_fuzzy() already use: they are
 * called as ff(q, 0.5*sys->D[i], ctx) internally, i.e. D is a RADIUS,
 * half of the distributed hard-sphere diameter). ctx carries whatever
 * the callback needs (shell thickness, densities, interface width,
 * or -- from the plugin wrapper -- a sasfit_param* plus which of its
 * slots to overwrite with D before calling a SASfit *_f amplitude
 * function; see the wrapper source for the worked example).
 */
typedef double (*rshs_ff_amplitude_fn)(double q, double R, void *ctx);

/* --- 2. Percus-Yevick solve -----------------------------------------
 *
 * Solve the multicomponent quadratic PY equations (Eq. 9 of the paper)
 * for lambda(i,j), given the classes already set by rshs_make_classes()
 * and a stickiness tau (tau -> 0 is infinitely sticky/adhesive,
 * tau -> infinity is the pure hard-sphere limit, matching Baxter's and
 * the paper's convention).
 *
 * Returns 0 on success, <0 if no physically-real solution was found
 * (this genuinely happens for tau below a critical tau_c at
 * intermediate densities, exactly as discussed in the paper around
 * Fig. 1/2 -- it is not necessarily a bug).
 *
 * PERFORMANCE NOTE (warm start): if `sys` already holds a valid
 * solution for the SAME number of classes p (i.e. sys->solved==1 and
 * sys->p_solved==sys->p, which is exactly the situation when the same
 * rshs_system object is reused across successive calls of
 * rshs_make_classes()+rshs_solve() with only slightly different
 * Dmean/srel/phi/tau -- precisely what happens once per iteration of
 * a least-squares fit), rshs_solve() automatically seeds the
 * nonlinear solver from the *previous* lambda(i,j) instead of the
 * generic monodisperse analytic guess. Because consecutive fit
 * iterations only perturb the parameters slightly, this typically
 * needs far fewer solver iterations to converge than a cold start.
 * rshs_make_classes() deliberately does not clear lambda[]/solved/
 * p_solved so this can happen automatically; call rshs_reset(sys) to
 * force a cold start (e.g. after deliberately jumping to a very
 * different parameter set).
 */
int rshs_solve(rshs_system *sys, double tau);

/* Force the next rshs_solve() on this system to use a cold (generic
 * monodisperse-analytic) initial guess instead of any warm-started
 * one -- useful when you know the next parameter set is unrelated to
 * the previous solve (e.g. a fit restarted from a new initial guess).*/
void rshs_reset(rshs_system *sys);

/* --- 3. structure factor matrix -------------------------------------
 *
 * Computes the real part of the partial structure factor matrix
 * S_nm(q) (Eq. 17 of the paper) at scattering vector q, into the
 * caller-supplied p x p block of Sij (only [0..p-1][0..p-1] is
 * written; Sij must be declared as double Sij[RSHS_MAXP][RSHS_MAXP]
 * by the caller). Returns 0 on success, <0 on failure (e.g. singular
 * Q matrix).
 */
int rshs_structure_matrix(const rshs_system *sys, double q,
                           double Sij[RSHS_MAXP][RSHS_MAXP]);

/* PERFORMANCE NOTE: internally this reuses a lazily-allocated, static
 * GSL workspace (sized RSHS_MAXP x RSHS_MAXP) across calls instead of
 * allocating/freeing the Q/inverse/product matrices every time, which
 * is by far the dominant cost of a q-scan otherwise (profiling showed
 * >80% of a typical fit-iteration's time was malloc/free churn here,
 * not the underlying O(p^3) linear algebra). This makes the function
 * NOT thread-safe -- if you need to call it concurrently from
 * multiple threads, give each thread its own rshs_system and protect
 * this function with a lock, or ask for a workspace-per-call variant.
 *
 * rshs_intensity_batch()/rshs_avg_structure_factor_batch() below do
 * the same thing for a whole q-array in one call, which additionally
 * saves per-call function-call overhead and, if compiled with
 * OpenMP (-fopenmp), parallelises the loop over q (each q is fully
 * independent). Without OpenMP they are equivalent to calling the
 * single-q versions in a loop, just marginally faster.               */
int rshs_intensity_coreshell_batch(const rshs_system *sys, const double *q, int nq,
                                    double *I_out, double t_shell, double rho_core,
                                    double rho_shell, double rho_solv);
int rshs_intensity_fuzzy_batch(const rshs_system *sys, const double *q, int nq,
                                double *I_out, double sigma_fuzzy,
                                double rho_particle, double rho_solv);

/* --- 4. form factors --------------------------------------------------
 *
 * Both return the (real) single-particle scattering amplitude F(q)
 * for a centrosymmetric electron/scattering-length density profile,
 * normalised the same way as Baxter/Guinier-Fournet, i.e.
 * F(q->0) = (4*pi/3) * R^3 * drho for a homogeneous sphere.
 *
 * rshs_ff_coreshell: sharp two-shell sphere. Rhs is the *hard-sphere*
 *   (interaction) radius of this size class; t_shell is the thickness
 *   of the outer shell (same for all classes, as in the original
 *   AOT-droplet analysis of the paper); the core radius is therefore
 *   Rcore = Rhs - t_shell. rho_core/rho_shell/rho_solv are the
 *   electron (or scattering-length) densities of core, shell and
 *   solvent respectively.
 *
 * rshs_ff_fuzzy: homogeneous sphere of hard-sphere radius Rhs (nominal
 *   radius) and density rho_particle, with a diffuse interface of rms
 *   width sigma_fuzzy smeared into the solvent rho_solv, using the
 *   standard Bartlett-Ottewill fuzzy-sphere approximation
 *   F(q) = F_sphere(q,Rhs,rho_particle-rho_solv) * exp(-(q*sigma_fuzzy)^2/2).
 */
double rshs_ff_coreshell(double q, double Rhs, double t_shell,
                          double rho_core, double rho_shell, double rho_solv);

double rshs_ff_fuzzy(double q, double Rhs, double sigma_fuzzy,
                      double rho_particle, double rho_solv);

/* --- 5. full multicomponent intensity --------------------------------
 *
 * Evaluates Eq. 14 of the paper,
 *   I(q)/V = sum_{n,m} sqrt(x_n x_m) F_n(q) F_m(q) S_nm(q)
 * using the per-class form factor for either supported particle model.
 * sys must already be solved via rshs_solve(). Returns the intensity
 * (arbitrary/absolute units consistent with the densities supplied),
 * or a negative number is impossible for this quantity so errors are
 * instead reported through rshs_last_error() and a return of 0 with a
 * message set; check rshs_last_error()[0]!='\0' after the call if you
 * need to distinguish a genuine zero from an error.
 */
double rshs_intensity_coreshell(const rshs_system *sys, double q,
                                 double t_shell, double rho_core,
                                 double rho_shell, double rho_solv);

double rshs_intensity_fuzzy(const rshs_system *sys, double q,
                             double sigma_fuzzy, double rho_particle,
                             double rho_solv);

/* Average (measurable) structure factor S~(q), Eq. 18 of the paper,
 * i.e. the intensity normalised by the average single-particle form
 * factor -- this is what is directly comparable to S(q) of a
 * monodisperse hard-sphere/adhesive-sphere fit, and what most SAS
 * fitting programs (incl. SASfit) expect a "structure factor" plugin
 * to return.                                                          */
double rshs_avg_structure_factor_coreshell(const rshs_system *sys, double q,
                                            double t_shell, double rho_core,
                                            double rho_shell, double rho_solv);

double rshs_avg_structure_factor_fuzzy(const rshs_system *sys, double q,
                                        double sigma_fuzzy, double rho_particle,
                                        double rho_solv);

/* --- 5b. GENERIC intensity / average structure factor ----------------
 *
 * What rshs_intensity_coreshell/fuzzy() and
 * rshs_avg_structure_factor_coreshell/fuzzy() are actually implemented
 * in terms of: Eq. 14 / Eq. 18 of the paper, using whatever per-class
 * scattering-amplitude callback you supply instead of one of the two
 * built-in particle models. sys must already be solved via
 * rshs_solve(). ff_ctx is passed through to ff unchanged.
 *
 * This is the hook that lets an arbitrary (centrosymmetric) particle
 * form factor be combined with the full multicomponent sticky-hard-
 * sphere interaction physics without writing any new solver code --
 * see sasfit_pl_robertus_shs.c for a worked example adapting SASfit's
 * own sasfit_ff_*_f(q,param)-style plugin functions this way.
 */
double rshs_intensity_generic(const rshs_system *sys, double q,
                               rshs_ff_amplitude_fn ff, void *ff_ctx);

double rshs_avg_structure_factor_generic(const rshs_system *sys, double q,
                                          rshs_ff_amplitude_fn ff, void *ff_ctx);

#ifdef __cplusplus
}
#endif

#endif /* ROBERTUS_SHS_CORE_H */
