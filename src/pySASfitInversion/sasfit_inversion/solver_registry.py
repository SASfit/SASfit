"""
Named, pluggable solver presets for interactive selection (e.g. a GUI
dropdown), matching the kernel_registry.py pattern: each entry is a
self-contained callable(A, b, db) -> SolverResult, so the GUI never needs
to know about smoothing parameters, lambda searches, or solver-specific
kwargs -- it just picks a key and calls .run(A, b, db).

Every solver built in this package is exposed here (2026-09-26, per
Joachim's request -- "not all solvers supplied in gui are available").
Each SolverSpec's description carries forward that solver's own
documented caveats (see the individual solvers/*.py docstrings for the
full story) so the choice of which to trust is yours, not made silently
on your behalf. Where a solver needs a parameter it doesn't choose for
itself (Tikhonov/TSVD's lambda/k, MaxEnt's lambda, Hansen's CG MaxEnt's
lambda+x0), this module adds a small auto-tuning wrapper (GCV, or a log
grid search targeting chi2_r near 1) -- these wrappers are new here, not
inherited from the solvers' own modules, and are simple heuristics, not
independently validated the way the underlying solvers' own docstrings'
findings are.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import numpy as np

from .solvers.base import SolverResult, chi2_r
from .solvers import (
    em, lambda_search, arls, maxent as maxent_em, svd_methods,
    general_tikhonov, bayesian_evidence, hansen_maxent, em_general,
)
from .regularization import second_derivative_operator, hansen_smoothness_cholesky


@dataclass
class SolverSpec:
    label: str
    run: Callable[[np.ndarray, np.ndarray, np.ndarray], SolverResult]
    description: str


def _run_em_discrepancy(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """EM + smoothing, smoothing parameter chosen via the discrepancy principle
    (target chi2_r=1). Falls back to the L-curve corner if the discrepancy
    principle can't be bracketed (this happens on real data with residual
    model mismatch, where chi2_r=1 may be genuinely unreachable -- see
    lambda_search.py and this package's own test findings). Uses
    Biggs-Andrews acceleration (acceleration.py) -- on the JAC 2022
    benchmark, plain Picard iteration needed 200000+ iterations and still
    hadn't converged where the accelerated version converges properly in
    ~2000-3000, ~4x faster wall-clock for the whole discrepancy-principle
    search including all its trial smoothing values."""
    def solve_fn(h):
        return em.solve(A, b, db, max_iterations=10000, smoothing_h=h, accelerate=True)

    try:
        search = lambda_search.discrepancy_principle_search(
            solve_fn, target_chi2_r=1.0, param_start=0.3, param_floor=1e-9
        )
        result = search.result
        result.diagnostics["lambda_selection"] = f"discrepancy principle, h={search.param:.4g}"
        return result
    except RuntimeError:
        lc = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=25)
        result = lc.result
        result.diagnostics["lambda_selection"] = (
            f"L-curve fallback (discrepancy principle unreachable), h={lc.param:.4g}"
        )
        return result


def _run_em_lcurve(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """EM + smoothing, smoothing parameter chosen via the L-curve corner
    (Menger curvature). Doesn't assume chi2_r=1 is achievable. Also uses
    Biggs-Andrews acceleration -- see _run_em_discrepancy's docstring."""
    def solve_fn(h):
        return em.solve(A, b, db, max_iterations=10000, smoothing_h=h, accelerate=True)

    lc = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=25)
    result = lc.result
    result.diagnostics["lambda_selection"] = f"L-curve corner, h={lc.param:.4g}"
    return result


def _run_em_general(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """EM for signed kernels/solutions (Chae, Martin & Walker 2018, Sec. 6).
    FIXED 2026-09-26 -- the earlier version omitted the paper's positivity
    shift (its eq. 10) and diverged; now reproduces the paper's own Fig. 7
    benchmark to corr=1.00000, and recovers a sphere correlation function
    from this library's signed j0(qr) kernel with chi2_r~0.6-0.7 and
    corr>0.99 (see em_general.py and test_em_general_basic.py). Needed for
    any kernel that goes negative (e.g. "4pi r^2 j0(qr)") -- the ordinary
    EM solvers above assume a non-negative kernel and silently give
    garbage otherwise. Smoothing strength auto-tuned via the discrepancy
    principle, same as the default EM solver.

    ACCELERATED 2026-10-03: em_general.solve is now Biggs-Andrews-accelerated
    by default (accelerate=True, passed explicitly below for clarity) -- a
    real stopping tolerance (tol=1e-8) is used instead of the old tol=0
    (which forced every discrepancy-principle trial to run the full
    max_iterations with no early exit at all, now unnecessary and wasteful
    since acceleration converges each trial quickly)."""
    def solve_fn(h):
        return em_general.solve(A, b, db, max_iterations=10000, smoothing_h=h, tol=1e-8, accelerate=True)

    try:
        search = lambda_search.discrepancy_principle_search(
            solve_fn, target_chi2_r=1.0, param_start=0.3, param_floor=1e-7
        )
        result = search.result
        result.diagnostics["lambda_selection"] = f"discrepancy principle, h={search.param:.4g}"
        return result
    except RuntimeError:
        lc = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=25)
        result = lc.result
        result.diagnostics["lambda_selection"] = (
            f"L-curve fallback (discrepancy principle unreachable), h={lc.param:.4g}"
        )
        return result


def _run_arlsnn(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """ARLS: fully automatic, non-negative, no tuning parameter at all."""
    x = arls.arlsnn(A, b)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": "automatic (ARLS, no tuning)"},
    )


# --------------------------------------------------------- SVD/Tikhonov family

def _run_tikhonov_standard_gcv(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """Standard-form (L=identity) Tikhonov, lambda chosen via GCV. No
    smoothness penalty -- see svd_methods.py's PERFORMANCE FINDING: this
    is consistently weaker than EM+smoothing for sharply-peaked or
    multi-modal distributions."""
    svd = svd_methods.compute_svd(A)
    lam_grid = np.geomspace(svd.s.min() * 1e-3 + 1e-300, svd.s.max() * 10, 100)
    lam_opt, _ = svd_methods.gcv_select_tikhonov(svd, b, lam_grid)
    x = svd_methods.tikhonov(svd, b, lam_opt)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": f"GCV, lambda={lam_opt:.4g}"},
    )


def _run_tsvd_gcv(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """Truncated SVD, truncation k chosen via GCV."""
    svd = svd_methods.compute_svd(A)
    k_max = max(2, min(40, len(svd.s) - 1))
    k_opt, _ = svd_methods.gcv_select_tsvd(svd, b, k_max)
    x = svd_methods.tsvd(svd, b, k_opt)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": f"GCV, k={k_opt}"},
    )


def _run_general_tikhonov_gcv(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """General-form (2nd-derivative-penalized) Tikhonov, lambda chosen via
    GCV. Mathematically verified exact (matches direct normal-equations
    solve), but see general_tikhonov.py's PERFORMANCE FINDING: still
    consistently weaker than EM+smoothing on sharply-peaked/multi-modal
    distributions in this package's own tests, because it has no
    positivity constraint."""
    n = A.shape[1]
    L = second_derivative_operator(n).toarray()
    transform = general_tikhonov.std_form(A, L, b)
    svd_s = svd_methods.compute_svd(transform.A_s)
    lam_grid = np.geomspace(svd_s.s.min() * 1e-3 + 1e-300, svd_s.s.max() * 10, 100)
    lam_opt, _ = general_tikhonov.general_tikhonov_gcv(A, L, b, lam_grid)
    x = general_tikhonov.general_tikhonov(A, L, b, lam_opt)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": f"GCV, lambda={lam_opt:.4g}"},
    )


def _run_bayesian_evidence(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """General-form Tikhonov with lambda chosen by Bayesian evidence
    maximization (Vestergaard & Hansen, 2006) instead of GCV/L-curve. The
    lambda search grid here is a heuristic based on A's singular value
    scale, not independently validated the way the evidence formula
    itself was (see bayesian_evidence.py)."""
    n = A.shape[1]
    L = second_derivative_operator(n).toarray()
    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
    best, _ = bayesian_evidence.evidence_search(A, b, db, L, lam_grid)
    fitted_b = A @ best.p_map
    return SolverResult(
        x=best.p_map, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={
            "lambda_selection": f"Bayesian evidence, lambda={best.lam:.4g}, Ng={best.n_good_params:.1f}",
        },
    )


def _run_bayesian_evidence_hansen(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """General-form Tikhonov, lambda chosen by Bayesian evidence
    (Vestergaard & Hansen, 2006), using Hansen (2000)'s own
    boundary-constrained smoothness operator (regularization.py's
    hansen_smoothness_cholesky) instead of the plain second-derivative
    operator used by _run_bayesian_evidence above.

    ADDED 2026-10-05, specifically to fix a real problem found on this
    package's own real-data test (test.dat) with the signed j0(qr)/PDDF
    kernel: the plain second_derivative_operator has a 2-dimensional null
    space (constant offset, linear ramp -- nothing in the roughness
    penalty constrains them), and combined with that kernel's extreme
    ill-conditioning (~1e16 for a 91-point/150-r-point real-data grid),
    GCV/L-curve/evidence search using that operator could not get chi2_r
    anywhere near 1 no matter how the smoothing strength was tuned (best
    found: ~586 via EM+smoothing, ~34600 via plain Tikhonov+GCV).

    Hansen (2000), J. Appl. Cryst. 33, 1415-1421, eq. 19 fixes exactly this
    by building the assumption p(0)=p(Dmax)=0 directly into the smoothness
    functional (not via basis functions -- Hansen explicitly calls the
    small-basis-function restriction of Glatter's original method
    obsolete once direct point-grid estimation is used, which is what this
    whole package already does). That removes the 2D null space (Hansen's
    own regularization matrix is provably full rank, det != 0) and fixes
    the conditioning: on the same test.dat signed-kernel problem, L^T L's
    condition number drops from ~1e16 to ~9.2e3, and the evidence search
    finds a clean interior maximum (not stuck at a grid edge) at
    chi2_r~1.6 -- a dramatic improvement over every other regularized
    solver tried on this specific problem (see
    tests/diagnose_hansen_evidence.py for the full lambda scan).

    This is a real, physically motivated constraint (the pair-distance/
    size distribution genuinely must vanish at r=0 and at the true maximum
    dimension) rather than an ad hoc numerical fix, so it's offered here
    as its own solver choice rather than silently replacing
    _run_bayesian_evidence's operator -- the plain second-derivative
    version remains available for cases where the endpoints are not
    expected to vanish (e.g. a deliberately truncated r-range)."""
    n = A.shape[1]
    L = hansen_smoothness_cholesky(n)
    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
    best, _ = bayesian_evidence.evidence_search(A, b, db, L, lam_grid)
    fitted_b = A @ best.p_map
    return SolverResult(
        x=best.p_map, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={
            "lambda_selection": f"Bayesian evidence (Hansen boundary-constrained), "
                                 f"lambda={best.lam:.4g}, Ng={best.n_good_params:.1f}",
        },
    )


# ------------------------------------------------------------- MaxEnt family

def _run_maxent_constant_prior(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """EM + MaxEnt, fixed uniform prior (JAC 2022 eq. 47-50). lambda is
    auto-selected here via a log-grid search targeting chi2_r near 1 --
    this search is a new heuristic added for the GUI, not part of the
    validated maxent.py module itself.

    ACCELERATED 2026-10-03 + SEMICONVERGENCE FIX: maxent.py's solvers are
    now Biggs-Andrews-accelerated by default and actually run to
    convergence (see maxent.py's own IMPORTANT FINDING note) instead of
    silently stopping at a fixed, small iteration count. That matters here:
    the previous version of this grid search ran every lambda trial for
    exactly 10000 *unaccelerated* iterations, so "best lambda" was
    implicitly tuned against that specific (unconverged) iteration count,
    not against lambda alone -- a form of accidental extra regularization.
    Each trial below now runs accelerate=True with a real tol, so every
    lambda in the grid is compared at its own true fixed point. On the
    JAC 2022 benchmark dataset, this and the adaptive-prior variant still
    land at the grid's lambda floor with near-identical chi2_r (~1.06) --
    unchanged from before the fix, so the entropy penalty genuinely adds
    little for this dataset; it isn't an artifact of the old iteration cap."""
    n = A.shape[1]
    prior = np.full(n, max(float(np.mean(b)) / n, 1e-6))
    x0 = np.full(n, 1e-6)
    best_result, best_diff = None, np.inf
    for lam in np.geomspace(1e-9, 1.0, 24):
        result = maxent_em.solve_constant_prior(
            A, b, db, prior=prior, lam=lam, x0=x0,
            max_iterations=50_000, tol=1e-8, accelerate=True,
        )
        if not result.chi2_r_history:
            continue
        diff = abs(result.chi2_r_history[-1] - 1.0)
        if diff < best_diff:
            best_diff, best_result, best_lam = diff, result, lam
    if best_result is None:
        raise RuntimeError("maxent constant-prior solver produced no usable result at any lambda tried")
    best_result.diagnostics["lambda_selection"] = f"grid search targeting chi2_r=1, lambda={best_lam:.4g}"
    return best_result


def _run_maxent_adaptive_prior(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """EM + MaxEnt, adaptive (self-constructed) prior (JAC 2022 eq. 51-55).
    lambda auto-selected the same way as the constant-prior variant above
    -- see that function's docstring for the semiconvergence fix and the
    near-identical-result finding on the JAC 2022 benchmark. This variant's
    fixed point can take noticeably more iterations to reach than the
    constant-prior case (confirmed up to ~800k plain-Picard iterations in
    testing), which is exactly why running every trial to real convergence
    here -- rather than a fixed small cap -- matters more for this solver."""
    n = A.shape[1]
    x0 = np.full(n, 1e-6)
    best_result, best_diff = None, np.inf
    for lam in np.geomspace(1e-9, 1.0, 24):
        result = maxent_em.solve_adaptive_prior(
            A, b, db, lam=lam, sigma2=1.0, x0=x0,
            max_iterations=50_000, tol=1e-8, accelerate=True,
        )
        if not result.chi2_r_history:
            continue
        diff = abs(result.chi2_r_history[-1] - 1.0)
        if diff < best_diff:
            best_diff, best_result, best_lam = diff, result, lam
    if best_result is None:
        raise RuntimeError("maxent adaptive-prior solver produced no usable result at any lambda tried")
    best_result.diagnostics["lambda_selection"] = f"grid search targeting chi2_r=1, lambda={best_lam:.4g}"
    return best_result


def _run_hansen_maxent(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """Hansen/Elfving's nonlinear-CG MaxEnt (regu.zip's maxent.m). Gradient
    verified correct against finite differences, but see
    hansen_maxent.py's PERFORMANCE FINDING: converges poorly on
    ill-conditioned kernels (this package's own tests saw a ~0.65
    correlation ceiling on a case where EM+smoothing reached ~0.996).
    Uses x0=0.01 (not the module's literal ones(n) default, which
    diverges badly here) and a small lambda grid search."""
    n = A.shape[1]
    x0 = np.full(n, 0.01)
    best_result, best_diff = None, np.inf
    for lam in np.geomspace(1e-3, 1.0, 15):
        result = hansen_maxent.solve(A, b, db, lam=lam, x0=x0, max_iterations=15000)
        if not result.chi2_r_history:
            continue
        diff = abs(result.chi2_r_history[-1] - 1.0)
        if diff < best_diff:
            best_diff, best_result, best_lam = diff, result, lam
    if best_result is None:
        raise RuntimeError("Hansen CG MaxEnt produced no usable result at any lambda tried")
    best_result.diagnostics["lambda_selection"] = f"grid search targeting chi2_r=1, lambda={best_lam:.4g}"
    return best_result


SOLVER_REGISTRY: dict[str, SolverSpec] = {
    "em_discrepancy": SolverSpec(
        label="EM + smoothing (auto: discrepancy principle)",
        run=_run_em_discrepancy,
        description="Recommended default. Expectation-Maximization with a "
                     "smoothing operator; smoothing strength auto-tuned to hit "
                     "chi2_r=1, falling back to the L-curve corner if that "
                     "target is unreachable. Best-performing solver in this "
                     "package's own validation (chi2_r=1.0000 on the JAC 2022 "
                     "benchmark, correct 60/180nm bimodal peaks).",
    ),
    "em_lcurve": SolverSpec(
        label="EM + smoothing (auto: L-curve corner)",
        run=_run_em_lcurve,
        description="Same EM+smoothing solver, but the smoothing strength is "
                     "chosen via the L-curve corner instead -- doesn't assume "
                     "chi2_r=1 is achievable, a safer default for real data "
                     "with model mismatch.",
    ),
    "arlsnn": SolverSpec(
        label="ARLS (automatic, non-negative, no tuning)",
        run=_run_arlsnn,
        description="Fully automatic Tikhonov-family solver (Jones, 2023) -- "
                     "no smoothing parameter to choose at all. Generally "
                     "weaker than EM+smoothing for sharply-peaked or "
                     "multi-modal distributions but useful as a "
                     "zero-configuration sanity check.",
    ),
    "tikhonov_standard_gcv": SolverSpec(
        label="Tikhonov, standard form L=I (auto: GCV)",
        run=_run_tikhonov_standard_gcv,
        description="Classic Tikhonov regularization with no smoothness "
                     "penalty, lambda chosen by generalized cross-validation. "
                     "Consistently weaker than EM+smoothing in this package's "
                     "tests (~0.85-0.89 correlation vs ~0.996 on synthetic "
                     "benchmarks) -- no positivity constraint.",
    ),
    "tsvd_gcv": SolverSpec(
        label="Truncated SVD (auto: GCV)",
        run=_run_tsvd_gcv,
        description="Classic TSVD, truncation rank chosen by GCV. Same "
                     "positivity-constraint limitation as standard Tikhonov.",
    ),
    "general_tikhonov_gcv": SolverSpec(
        label="Tikhonov, 2nd-derivative penalty (auto: GCV)",
        run=_run_general_tikhonov_gcv,
        description="General-form Tikhonov with a smoothness penalty, "
                     "mathematically verified exact against a direct solve. "
                     "Still weaker than EM+smoothing in this package's tests "
                     "(~0.86/~0.68 correlation on single/bimodal synthetic "
                     "cases vs EM's ~0.996/~0.995) -- again, no positivity "
                     "constraint, which matters for sharply-peaked or "
                     "multi-modal distributions.",
    ),
    "bayesian_evidence": SolverSpec(
        label="Tikhonov, 2nd-derivative penalty (auto: Bayesian evidence)",
        run=_run_bayesian_evidence,
        description="Same general-form Tikhonov, but lambda is chosen by "
                     "maximizing the Bayesian evidence (Vestergaard & Hansen, "
                     "2006) instead of GCV -- also reports Ng, the effective "
                     "number of resolved parameters. Has an unresolved "
                     "factor-of-2 ambiguity noted in bayesian_evidence.py "
                     "between the paper's eq. 3 and eq. 6.",
    ),
    "maxent_constant_prior": SolverSpec(
        label="EM + MaxEnt, constant prior (auto lambda)",
        run=_run_maxent_constant_prior,
        description="EM with an additive maximum-entropy penalty against a "
                     "fixed uniform prior (JAC 2022 eq. 47-50). lambda "
                     "auto-selected by a grid search targeting chi2_r=1 -- "
                     "this search is new for the GUI, not independently "
                     "validated the way the underlying update rule is.",
    ),
    "maxent_adaptive_prior": SolverSpec(
        label="EM + MaxEnt, adaptive prior (auto lambda)",
        run=_run_maxent_adaptive_prior,
        description="Same EM+MaxEnt framework, but the prior is rebuilt each "
                     "iteration from a smoothed version of the current "
                     "solution (JAC 2022 eq. 51-55) -- in this package's own "
                     "tests, did notably better than the constant-prior "
                     "version even at an untuned lambda.",
    ),
    "hansen_maxent": SolverSpec(
        label="Hansen CG MaxEnt (auto lambda) -- weak on ill-conditioned kernels",
        run=_run_hansen_maxent,
        description="Nonlinear conjugate-gradient MaxEnt (Hansen & Elfving, "
                     "regu.zip). Gradient verified correct, but converges "
                     "poorly on the ill-conditioned kernels typical here -- "
                     "expect noticeably worse results than EM+smoothing.",
    ),
    "em_general_signed_kernel": SolverSpec(
        label="Signed-kernel EM (required for j0/sinc kernel)",
        run=_run_em_general,
        description="Use this instead of the plain EM solvers above whenever "
                     "the kernel can go negative -- e.g. the '4\u03c0r\u00b2 "
                     "j0(qr)' kernel, where j0(x)=sin(x)/x is negative for "
                     "x>\u03c0. The ordinary EM update is only valid for a "
                     "non-negative kernel and silently gives garbage "
                     "otherwise. Implements Chae, Martin & Walker (2018) "
                     "Sec. 6; reproduces their own published benchmark "
                     "exactly (corr=1.00000) and recovers a sphere "
                     "correlation function from this library's own signed "
                     "kernel with chi2_r~1, corr>0.99. On real, noisy data with a "
                     "genuinely ill-conditioned kernel (e.g. this package's "
                     "own test.dat with the j0 kernel), this solver's chi2_r "
                     "can plateau far above 1 regardless of smoothing "
                     "strength -- try 'IFT, Bayesian evidence (Hansen "
                     "boundary-constrained)' below instead in that case.",
    ),
    "bayesian_evidence_hansen": SolverSpec(
        label="IFT, Bayesian evidence (Hansen boundary-constrained) -- recommended for j0/PDDF kernel",
        run=_run_bayesian_evidence_hansen,
        description="General-form Tikhonov with the smoothing strength chosen "
                     "automatically by Bayesian evidence maximization (Vestergaard "
                     "& Hansen, 2006), using Hansen (2000)'s own smoothness "
                     "operator, which builds the assumption p(0)=p(Dmax)=0 "
                     "directly into the regularization (not via basis functions). "
                     "Added specifically because the other regularized solvers "
                     "(plain Tikhonov+GCV, EM+smoothing, signed-kernel EM) could "
                     "not get anywhere near chi2_r=1 on this package's own "
                     "real-data test with the signed j0(qr)/PDDF kernel, due to "
                     "the ~1e16 condition number that comes from the ordinary "
                     "roughness penalty leaving a constant-offset/linear-ramp "
                     "null space unconstrained. With Hansen's operator that "
                     "condition number drops to ~9e3 and the evidence search "
                     "finds a clean maximum at chi2_r~1.6 on that same test "
                     "(see tests/diagnose_hansen_evidence.py). This is the "
                     "recommended starting point for pair-distance-distribution "
                     "(j0/PDDF kernel) recovery specifically; for the sphere "
                     "size-distribution kernel, EM+smoothing (discrepancy "
                     "principle) remains the better-validated default.",
    ),
}


def list_solvers() -> list[tuple[str, str]]:
    """(key, label) pairs, e.g. for populating a GUI dropdown."""
    return [(key, spec.label) for key, spec in SOLVER_REGISTRY.items()]
