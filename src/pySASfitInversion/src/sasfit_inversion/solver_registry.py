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

from dataclasses import dataclass, replace
from typing import Callable

import numpy as np

from .solvers.base import SolverResult, chi2_r
from .solvers import (
    em, lambda_search, arls, maxent as maxent_em, svd_methods,
    general_tikhonov, bayesian_evidence, hansen_maxent, em_general,
)
from .regularization import (
    second_derivative_operator, hansen_smoothness_cholesky, hann_taper,
    hansen_smoothness_two_segment,
)
from . import spline_basis


@dataclass
class SolverSpec:
    label: str
    run: Callable[[np.ndarray, np.ndarray, np.ndarray], SolverResult]
    description: str
    # extra kwargs passed to `run` when the solver is used in B-spline
    # coefficient space via run_solver(..., spline=...)
    spline_kwargs: dict | None = None
    # True for solvers that build their own spline basis (they receive
    # n_coeffs/spacing/A_quad directly instead of being wrapped)
    native_spline: bool = False
    # selectable variants of one solver: {kwarg name: [(value, label), ...]};
    # the first entry is the default. The GUI shows one dropdown per option.
    options: dict | None = None
    # option labels for the GUI, {kwarg name: "Label:"}
    option_labels: dict | None = None


@dataclass
class SplineOptions:
    """Represent p(r) (or x(r)) by cubic B-splines for ANY solver:
    p = B c, the solver works on A_spline = A_quad @ B_quad and returns
    the coefficients c. See spline_basis.py for the basis itself.

    n_coeffs: number of coefficients (None -> _spline_n_coeffs_default).
    spacing: knot placement in r ("uniform", "quadratic", "log").
    A_quad: optional kernel on a finer equidistant grid over the same
        [0, r_max] (same kernel and alpha) used to integrate the basis;
        None integrates on A's own grid.
    """
    n_coeffs: int | None = None
    spacing: str = "uniform"
    A_quad: np.ndarray | None = None


def _run_em_discrepancy(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, fallback: str = "lcurve",
) -> SolverResult:
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
        return _em_fallback(solve_fn, fallback)


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


def _em_fallback(solve_fn, fallback: str) -> SolverResult:
    """What the EM-family discrepancy-principle solvers do when chi2_r=1
    cannot be reached. "lcurve" (point-grid default): L-curve corner.
    "least_smoothed" (used in B-spline mode): h=0, i.e. the closest
    achievable fit -- in coefficient space the basis already regularizes
    and the L-curve picks far too much extra smoothing (test.dat, sphere
    kernel, 40 coefficients: chi2_r 37.6 (em_lcurve) / 705 (signed EM)
    vs ~1.0 with h=0)."""
    if fallback == "least_smoothed":
        result = solve_fn(0.0)
        result.diagnostics["lambda_selection"] = (
            "chi2_r=1 unreachable; h=0 (basis-only regularization, closest "
            f"achievable chi2_r={result.chi2_r_history[-1]:.4g})"
        )
        return result
    lc = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=25)
    result = lc.result
    result.diagnostics["lambda_selection"] = (
        f"L-curve fallback (discrepancy principle unreachable), h={lc.param:.4g}"
    )
    return result


def _run_em_general(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, fallback: str = "lcurve",
) -> SolverResult:
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
        return _em_fallback(solve_fn, fallback)


def _weighted(A: np.ndarray, b: np.ndarray, db: np.ndarray):
    """Rows scaled by 1/dI, so plain least-squares solvers minimize chi2
    instead of the unweighted residual. Before 2026-10-10 arlsnn,
    tikhonov_standard_gcv, tsvd_gcv and general_tikhonov_gcv ignored dI
    entirely (dI spans 0.01..90 on test.dat, so the high-intensity low-q
    points dominated and those solvers looked far worse than they are)."""
    w = 1.0 / np.asarray(db, dtype=float)
    return A * w[:, None], b * w


def _run_arlsnn(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """ARLS: fully automatic, non-negative, no tuning parameter at all.
    Applied to the 1/dI-weighted system."""
    Aw, bw = _weighted(A, b, db)
    x = arls.arlsnn(Aw, bw)
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
    Aw, bw = _weighted(A, b, db)
    svd = svd_methods.compute_svd(Aw)
    lam_grid = np.geomspace(svd.s.min() * 1e-3 + 1e-300, svd.s.max() * 10, 100)
    lam_opt, _ = svd_methods.gcv_select_tikhonov(svd, bw, lam_grid)
    x = svd_methods.tikhonov(svd, bw, lam_opt)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": f"GCV, lambda={lam_opt:.4g}"},
    )


def _run_tsvd_gcv(A: np.ndarray, b: np.ndarray, db: np.ndarray) -> SolverResult:
    """Truncated SVD, truncation k chosen via GCV."""
    Aw, bw = _weighted(A, b, db)
    svd = svd_methods.compute_svd(Aw)
    k_max = max(2, min(40, len(svd.s) - 1))
    k_opt, _ = svd_methods.gcv_select_tsvd(svd, bw, k_max)
    x = svd_methods.tsvd(svd, bw, k_opt)
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
    Aw, bw = _weighted(A, b, db)
    transform = general_tikhonov.std_form(Aw, L, bw)
    svd_s = svd_methods.compute_svd(transform.A_s)
    lam_grid = np.geomspace(svd_s.s.min() * 1e-3 + 1e-300, svd_s.s.max() * 10, 100)
    lam_opt, _ = general_tikhonov.general_tikhonov_gcv(Aw, L, bw, lam_grid)
    x = general_tikhonov.general_tikhonov(Aw, L, bw, lam_opt)
    fitted_b = A @ x
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True, diagnostics={"lambda_selection": f"GCV, lambda={lam_opt:.4g}"},
    )


# The optional chi2_r target may RAISE lambda freely above the evidence
# value, but may lower it by at most this factor. Reaching chi2_r=1 only by
# (almost) removing the regularization means the target is wrong for these
# data (e.g. noise-inflated bootstrap replicates, underestimated dI), and
# the result would be an unregularized, oscillating p(r). Below the floor
# the closest admissible fit is returned with a "not reachable" warning.
_MAX_LAMBDA_REDUCTION = 1e-3


def _unreachable_note(target_chi2_r: float, search) -> str:
    """Diagnostic suffix when the discrepancy-principle target could not be
    reached: the closest achievable fit is returned instead of an error."""
    c2 = search.result.chi2_r_history[-1]
    why = ("even the least-regularized solution fits worse than the target "
           "(model has too few degrees of freedom, e.g. too few spline "
           "coefficients)" if c2 > target_chi2_r else
           "even the most-regularized solution fits better than the target "
           "(error bars may be overestimated)")
    return (f" -- WARNING: target chi2_r={target_chi2_r:.3g} not reachable; "
            f"closest achievable chi2_r={c2:.4g} returned ({why})")


def _bayesian_evidence_pick_lambda(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, L: np.ndarray,
    lam_grid: np.ndarray, target_chi2_r: float | None, selection_label: str,
    ridge: float = 0.0,
):
    """Shared logic for all three Bayesian-evidence solver variants below:
    find the evidence-maximizing lambda (always -- used as the discrepancy-
    principle search's starting point even when target_chi2_r is given, and
    its own chi2_r/Ng are always reported in the diagnostics so you can see
    what evidence-maximization alone would have chosen), then OPTIONALLY
    (target_chi2_r is not None) refine it to hit a target chi2_r exactly via
    discrepancy_principle_search_bidirectional instead -- see that
    function's docstring for why this needs its own, direction-agnostic
    search rather than reusing the EM solvers' discrepancy_principle_search.

    Returns (p_map, lam_used, selection_note).
    """
    best, _ = bayesian_evidence.evidence_search(A, b, db, L, lam_grid, ridge=ridge)
    if target_chi2_r is None:
        return best.p_map, best.lam, (
            f"{selection_label}, lambda={best.lam:.4g}, Ng={best.n_good_params:.1f}"
        )

    def solve_fn(lam: float) -> SolverResult:
        p_map = bayesian_evidence._map_solution(A, b, db, L, lam, ridge=ridge)
        fitted = A @ p_map
        return SolverResult(
            x=p_map, fitted_b=fitted, n_iterations=0,
            chi2_r_history=[chi2_r(b, fitted, db)], roughness_history=[], converged=True,
        )

    search = lambda_search.discrepancy_principle_search_bidirectional(
        solve_fn, target_chi2_r=target_chi2_r, param_start=best.lam,
        param_floor=best.lam * _MAX_LAMBDA_REDUCTION,
    )
    c2_evidence = chi2_r(b, A @ best.p_map, db)
    note = (
        f"{selection_label}; discrepancy principle (target chi2_r={target_chi2_r:.3g}), "
        f"lambda={search.param:.4g} (evidence-maximizing lambda was "
        f"{best.lam:.4g}, Ng={best.n_good_params:.1f}, chi2_r={c2_evidence:.4g})"
    )
    if not search.reachable:
        note += _unreachable_note(target_chi2_r, search)
    return search.result.x, search.param, note


def _run_bayesian_evidence(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, target_chi2_r: float | None = None,
) -> SolverResult:
    """General-form Tikhonov with lambda chosen by Bayesian evidence
    maximization (Vestergaard & Hansen, 2006) instead of GCV/L-curve. The
    lambda search grid here is a heuristic based on A's singular value
    scale, not independently validated the way the evidence formula
    itself was (see bayesian_evidence.py).

    target_chi2_r: optional (default None, i.e. pure evidence maximization,
    unchanged behavior). When given, the evidence-maximizing lambda found
    above is used only as a starting point, then refined so chi2_r hits
    this target exactly (discrepancy principle) -- see
    lambda_search.discrepancy_principle_search_bidirectional's docstring
    and _bayesian_evidence_pick_lambda above for why this is opt-in rather
    than the default: evidence maximization and chi2_r=1 are two different
    criteria, and on at least this package's own real test.dat data,
    evidence maximization alone was found to land significantly under-
    regularized (chi2_r~0.38), which is consistent with -- and a more
    direct explanation for -- the high-q p(r) "ringing" this package spent
    considerable effort chasing via boundary/taper changes (Joachim,
    2026-10-09): a fit that tracks noise point-by-point (chi2_r well below
    1) will show spurious oscillation in the recovered p(r) regardless of
    what happens at the r=0/Dmax boundary specifically."""
    n = A.shape[1]
    L = second_derivative_operator(n).toarray()
    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
    p_map, lam_used, selection_note = _bayesian_evidence_pick_lambda(
        A, b, db, L, lam_grid, target_chi2_r, "Bayesian evidence",
    )
    fitted_b = A @ p_map
    cov = bayesian_evidence.posterior_covariance(A, db, L, lam_used)
    x_sigma = np.sqrt(np.clip(np.diag(cov), 0, None))
    return SolverResult(
        x=p_map, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": selection_note, "lambda": lam_used, "x_sigma": x_sigma, "x_cov": cov},
    )


def _run_bayesian_evidence_hansen(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, target_chi2_r: float | None = None,
) -> SolverResult:
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
    expected to vanish (e.g. a deliberately truncated r-range).

    target_chi2_r: optional (default None, pure evidence maximization,
    unchanged behavior) -- see _run_bayesian_evidence's docstring and
    _bayesian_evidence_pick_lambda above for what this does and why it's
    opt-in. Applies equally here: evidence maximization choosing a
    significantly-under-1 chi2_r is exactly the kind of overfitting that
    shows up as spurious high-q p(r) oscillation regardless of which
    boundary treatment (hard Hansen vs. Hann-tapered) is used."""
    n = A.shape[1]
    L = hansen_smoothness_cholesky(n)
    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
    p_map, lam_used, selection_note = _bayesian_evidence_pick_lambda(
        A, b, db, L, lam_grid, target_chi2_r, "Bayesian evidence (Hansen boundary-constrained)",
    )
    fitted_b = A @ p_map
    cov = bayesian_evidence.posterior_covariance(A, db, L, lam_used)
    x_sigma = np.sqrt(np.clip(np.diag(cov), 0, None))
    return SolverResult(
        x=p_map, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": selection_note, "lambda": lam_used, "x_sigma": x_sigma, "x_cov": cov},
    )


def _run_bayesian_evidence_hann_tapered(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, taper_frac: float = 0.25,
    target_chi2_r: float | None = None,
) -> SolverResult:
    """General-form Tikhonov, lambda chosen by Bayesian evidence (Vestergaard
    & Hansen, 2006), but with Hansen (2000)'s HARD p(0)=p(Dmax)=0 boundary
    condition replaced by a soft Hann-window taper (regularization.py's
    hann_taper): p(r) = w(r) * p_free(r), with w(r) smoothly rolling off
    to 0 over the last quarter of the r-range instead of being pinned to
    exactly 0 at a single hard edge.

    ADDED 2026-10-07, as an alternative to 'bayesian_evidence_hansen' above
    -- not a replacement, since the two make different tradeoffs (see
    below). Motivation: p(r) and I(q) are related by (essentially) a
    Fourier sine transform, and a sharp edge in one domain is expected to
    leak oscillatory "ringing" into the conjugate domain with slowly
    decaying (~1/q) sidelobes -- a plausible mechanism for the high-q
    noise-tracking found on this package's own real test.dat data, which
    a systematic empirical scan (diagnose_hansen_lambda_balance.py,
    diagnose_hansen_dmax_scan.py, diagnose_hansen_nr_scan.py) showed was
    essentially unchanged across global lambda, Dmax, n_r, and r_min
    individually -- i.e. not fixable by any of Hansen's own hard-boundary
    hyperparameters.

    Validated on a synthetic sphere test with known ground truth (5
    independent noise draws): the tapered version had LOWER RMSE against
    the true p(r) than the hard boundary in every single trial, and its
    roughness (sum-of-squared-second-differences, a ringing proxy) matched
    the true curve's own smoothness far more closely. High-q chi2
    contribution was also consistently, if modestly, closer to 1 (less
    overfit) under tapering. CAVEAT: that synthetic test did not reproduce
    the SEVERITY of the real test.dat high-q pathology (chi2 contribution
    there sits around 0.02 under the hard boundary; the synthetic test's
    worst case was ~0.48), so this is validated as a real, reproducible
    improvement in the right direction, not a guarantee that it closes
    the real-data gap to the same degree -- see
    tests/diagnose_hansen_tapered.py for the direct real-data comparison,
    which should be checked before treating this as the new default.

    Implementation note: without Hansen's boundary fix, the plain
    second-derivative operator used here for p_free's smoothness penalty
    has a 2-dimensional null space (constant offset, linear ramp -- see
    second_derivative_operator's own docstring), which on this package's
    kernels can leave the evidence Hessian singular for the whole lambda
    grid. A tiny ridge stabilizer (bayesian_evidence.py's new `ridge`
    parameter, added specifically for this solver) removes the exact
    singularity without materially changing the result -- see that
    module's docstring for why this is an approximation, not exact.

    target_chi2_r: optional (default None, pure evidence maximization,
    unchanged behavior) -- see _run_bayesian_evidence's docstring for what
    this does and why it's opt-in."""
    n = A.shape[1]
    # r is not directly available here (only the kernel matrix A), so the
    # taper is built in INDEX space (0..n-1) rather than physical r -- fine
    # since hann_taper only needs a monotonic coordinate and a fraction of
    # its range, and the GUI/caller always builds A from a linear r grid.
    idx = np.arange(n, dtype=float)
    w = hann_taper(idx, idx[-1], taper_frac=taper_frac)
    A_win = A * w[None, :]

    L_plain = second_derivative_operator(n).toarray()
    svd_Awin = svd_methods.compute_svd(A_win)
    scale = float(np.percentile(svd_Awin.s, 75)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
    ridge_eps = 1e-8 * scale  # relative to A_win's own singular-value scale, not an absolute constant

    p_free, lam_used, selection_note = _bayesian_evidence_pick_lambda(
        A_win, b, db, L_plain, lam_grid, target_chi2_r,
        f"Bayesian evidence (Hann-tapered boundary, taper_frac={taper_frac:.3g})",
        ridge=ridge_eps,
    )
    x = w * p_free
    fitted_b = A_win @ p_free  # == A @ x
    # Cov(x) = diag(w) @ Cov(p_free) @ diag(w) exactly, since x = w*p_free
    # elementwise is a diagonal linear map -- so the diagonal (pointwise
    # variance) is just w_j^2 * Var(p_free)_j, no need to form the full
    # (n,n) product.
    cov_free = bayesian_evidence.posterior_covariance(A_win, db, L_plain, lam_used, ridge=ridge_eps)
    cov = w[:, None] * cov_free * w[None, :]
    x_sigma = np.sqrt(np.clip(np.diag(cov), 0, None))
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": selection_note, "x_sigma": x_sigma, "x_cov": cov},
    )


def _run_bayesian_evidence_two_segment(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, split_frac: float = 0.5,
    target_chi2_r: float | None = None,
) -> SolverResult:
    """Region-adaptive extension of 'bayesian_evidence_hansen': instead of
    one global lambda for Hansen (2000)'s boundary-constrained smoothness
    penalty, this splits it into two additive, non-overlapping pieces by
    r-index (regularization.hansen_smoothness_two_segment) -- lambda_lo for
    the low-r segment, lambda_hi for the high-r segment -- and finds the
    evidence-maximizing PAIR via a 2D grid search
    (bayesian_evidence.evidence_search_blocks/log_evidence_blocks).

    MOTIVATION (Joachim, 2026-10-09/10): a single global lambda, whether
    evidence-chosen or forced to hit chi2_r=1 via the discrepancy principle,
    was found to be structurally unable to properly regularize both the
    low/mid-q-dominated bulk of p(r) and the high-q region simultaneously
    (forcing global chi2_r=1 only moved the region-specific high-q chi2_r
    from 0.029 to 0.088 -- still ~10x below the ideal 1, not fixed). This is
    the direct fix for that finding: let the high-r segment (which, via the
    kernel's Fourier-type relationship, controls the solution's response at
    high q) carry its own, independently-chosen regularization strength
    rather than being forced to share the bulk's lambda.

    split_frac: where to place the lo/hi split, as a fraction of the
    r-index range (0.5 = midpoint). Not independently tuned -- a sensible
    default, not a validated optimum; expose as a GUI parameter if a
    dataset needs a different split point.

    target_chi2_r: optional (default None, pure evidence maximization on
    the lo/hi pair, unchanged default behavior otherwise). When given, the
    evidence-maximizing (lam_lo, lam_hi) pair found above is used as a
    starting point, and both lambdas are then scaled by a single common
    multiplier (found via discrepancy_principle_search_bidirectional) until
    the GLOBAL chi2_r hits the target -- this preserves the lo/hi lambda
    RATIO the evidence search found (i.e. keeps the region-adaptive shape)
    rather than re-optimizing lo and hi independently against a chi2_r
    target, which has no unique solution (many (lo,hi) pairs can give the
    same global chi2_r).

    Cost: the grid search is O(m^2) log-evidence evaluations for an m-point
    per-block grid (each itself an O(n^3) dense solve+logdet) -- kept to
    m=14 (196 evaluations) to stay responsive; coarser than the single-
    block solvers' m=60 1D grids.
    """
    n = A.shape[1]
    split_idx = int(np.clip(round(n * split_frac), 1, n - 2))
    A_lo, A_hi, n_lo_terms, n_hi_terms = hansen_smoothness_two_segment(n, split_idx)

    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-5, 1e-300), scale * 1e5, 14)

    best, _ = bayesian_evidence.evidence_search_blocks(
        A, b, db, (A_lo, A_hi), (n_lo_terms, n_hi_terms), (lam_grid, lam_grid)
    )
    lam_lo, lam_hi = best.lams

    if target_chi2_r is None:
        p_map = best.p_map
        lams_used = best.lams
        selection_note = (
            f"Bayesian evidence, two-segment (split at r-index {split_idx}/{n}), "
            f"lambda_lo={lam_lo:.4g}, lambda_hi={lam_hi:.4g}, Ng={best.n_good_params:.1f}"
        )
    else:
        def solve_fn(s: float) -> SolverResult:
            res = bayesian_evidence.log_evidence_blocks(
                A, b, db, (A_lo, A_hi), (n_lo_terms, n_hi_terms), (lam_lo * s, lam_hi * s)
            )
            fitted = A @ res.p_map
            return SolverResult(
                x=res.p_map, fitted_b=fitted, n_iterations=0,
                chi2_r_history=[chi2_r(b, fitted, db)], roughness_history=[], converged=True,
            )
        # search directly in the common SCALE multiplier applied to both
        # evidence-chosen lambdas (not their log), starting at scale=1 (the
        # evidence-chosen pair itself) and expanding geometrically --
        # mirrors the single-lambda solvers' use of
        # discrepancy_principle_search_bidirectional on lambda itself
        # (param_start there is the evidence-chosen lambda; here it's the
        # scale relative to the evidence-chosen pair, i.e. 1.0).
        search = lambda_search.discrepancy_principle_search_bidirectional(
            solve_fn, target_chi2_r=target_chi2_r, param_start=1.0, factor=2.0,
            param_floor=_MAX_LAMBDA_REDUCTION,
        )
        s_used = float(search.param)
        lams_used = (lam_lo * s_used, lam_hi * s_used)
        p_map = search.result.x
        c2_evidence = chi2_r(b, A @ best.p_map, db)
        selection_note = (
            f"discrepancy principle (target chi2_r={target_chi2_r:.3g}), two-segment "
            f"(split at r-index {split_idx}/{n}), scale={s_used:.4g} applied to evidence-chosen "
            f"lambda_lo={lam_lo:.4g}/lambda_hi={lam_hi:.4g} (ratio preserved; evidence-only "
            f"chi2_r={c2_evidence:.4g}, Ng={best.n_good_params:.1f})"
        )
        if not search.reachable:
            selection_note += _unreachable_note(target_chi2_r, search)

    fitted_b = A @ p_map
    cov = bayesian_evidence.posterior_covariance_blocks(A, db, (A_lo, A_hi), lams_used)
    x_sigma = np.sqrt(np.clip(np.diag(cov), 0, None))
    return SolverResult(
        x=p_map, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": selection_note, "x_sigma": x_sigma, "x_cov": cov},
    )


def _spline_n_coeffs_default(n: int) -> int:
    """Heuristic default coefficient count for the Glatter-style B-spline
    basis: Glatter's own papers and GNOM typically use ~15-30 coefficients
    regardless of the point-grid's n; clip to that literature range while
    staying comfortably below n so the basis matrix isn't rank-deficient on
    coarser grids.

    GUARD: spline_basis.build_bspline_basis requires n_coeffs in
    [degree+1, n-2] (degree=3 here, so >=4). The GUI's r-grid-points
    spinbox allows n as low as 10, where the naive literature-range lower
    bound of 10 would ask for n_coeffs=10 on an n=10 grid (n-2=8 < 10) and
    raise -- clip the lower bound down to whatever the grid can actually
    support instead of failing on small grids."""
    upper = min(30, n - 2)
    lower = max(min(10, upper), 4)
    if lower > upper:
        raise ValueError(
            f"grid of n={n} r-points is too small for a cubic B-spline basis "
            f"(need at least {4 + 2} points)"
        )
    return int(np.clip(n // 4, lower, upper))


def _spline_system(
    A: np.ndarray, n_coeffs: int | None, spacing: str, A_quad: np.ndarray | None
):
    """Shared setup for the B-spline solvers.

    Returns (B_disp, A_spline, desc): B_disp maps coefficients to p(r) on
    the display grid (A's n columns, equidistant 0..r_max), A_spline is the
    (M, n_coeffs) forward operator in coefficient space.

    A_quad: optional kernel on a FINER equidistant grid over the same
    [0, r_max] (e.g. 4x denser, built by the caller with the same kernel and
    alpha). The spline columns are then integrated on that grid instead of
    the display grid, which matters when knot spans are shorter than the
    display spacing (quadratic/log knots near r=0: on test.dat with 95
    points some basis functions fall entirely between grid points).
    """
    n = A.shape[1]
    n_quad = n if A_quad is None else A_quad.shape[1]
    if n_coeffs is None:
        n_coeffs = _spline_n_coeffs_default(n)
    if n_coeffs + 2 > n_quad:
        raise ValueError(
            f"n_coeffs={n_coeffs} too large for {n_quad} integration points "
            f"(need n_coeffs <= {n_quad - 2})"
        )
    B_disp = spline_basis.bspline_basis_at(np.linspace(0.0, 1.0, n), n_coeffs, 3, spacing)
    if A_quad is None:
        A_spline = A @ B_disp
        quad = ""
    else:
        B_quad = spline_basis.bspline_basis_at(np.linspace(0.0, 1.0, n_quad), n_coeffs, 3, spacing)
        A_spline = A_quad @ B_quad
        quad = f", integrated on {n_quad} points"
    return B_disp, A_spline, f"{n_coeffs} coefficients, {spacing} knots{quad}"


def _run_bayesian_evidence_spline(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, n_coeffs: int | None = None,
    target_chi2_r: float | None = None, spacing: str = "uniform",
    A_quad: np.ndarray | None = None,
) -> SolverResult:
    """Glatter-style IFT: p(r) represented as a small number of cubic
    B-spline coefficients (spline_basis.build_bspline_basis) rather than the
    full n-point grid, with lambda (a mild smoothness penalty on the
    coefficients themselves) chosen by Bayesian evidence maximization,
    exactly as in 'bayesian_evidence_hansen' above but operating on the much
    smaller/better-conditioned coefficient-space problem A_spline = A @ B.

    WHY THIS IS A DIFFERENT FIX FOR THE SAME PROBLEM as
    'bayesian_evidence_two_segment': that solver keeps the full n-point
    grid and lets regularization STRENGTH vary by region; this solver
    instead reduces the number of free parameters so that high-frequency,
    near-Nyquist oscillation (the specific shape of the observed high-q
    'ringing' in p(r)) isn't representable at all, at any lambda -- the
    classic Glatter (1977)/GNOM (Svergun, 1992) strategy, cited in this
    package's own hansen_smoothness_cholesky docstring as the approach
    Hansen (2000) considered 'obsolete' once direct point-grid estimation
    became computationally practical -- offered here as a selectable
    alternative, not a replacement, since which assumption (smooth basis
    vs. region-adaptive point-grid smoothing) fits a given dataset better
    is an empirical question, not something to decide in the library.

    p(0)=p(Dmax)=0 is enforced structurally by the basis itself (see
    spline_basis.py's module docstring), not via a penalty term, so the
    coefficient-space smoothness operator L here can be the PLAIN
    second-derivative operator (no Hansen boundary term needed -- there is
    no boundary null-space problem in coefficient space the way there was
    in point-grid space, because m << n coefficients are already heavily
    overdetermined by the n data-fitting equations).

    n_coeffs: number of B-spline coefficients (default: see
    _spline_n_coeffs_default -- ~15-30, following Glatter/GNOM convention).
    target_chi2_r: optional, same meaning as the other Bayesian-evidence
    solvers (see _run_bayesian_evidence's docstring) -- opt-in, default
    None leaves pure evidence maximization unchanged.
    """
    B, A_spline, desc = _spline_system(A, n_coeffs, spacing, A_quad)
    m = B.shape[1]
    L = second_derivative_operator(m).toarray()

    svd_spline = svd_methods.compute_svd(A_spline)
    scale = float(np.median(svd_spline.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)

    c_map, lam_used, selection_note = _bayesian_evidence_pick_lambda(
        A_spline, b, db, L, lam_grid, target_chi2_r,
        f"Bayesian evidence (Glatter-style B-spline basis, {desc})",
    )
    x = B @ c_map
    fitted_b = A_spline @ c_map  # == A @ x on the integration grid
    cov_c = bayesian_evidence.posterior_covariance(A_spline, db, L, lam_used)
    cov = B @ cov_c @ B.T
    x_sigma = np.sqrt(np.clip(np.diag(cov), 0, None))
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=0,
        chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
        converged=True,
        diagnostics={"lambda_selection": selection_note, "x_sigma": x_sigma, "x_cov": cov},
    )


def _run_em_discrepancy_spline(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, n_coeffs: int | None = None,
    spacing: str = "uniform", A_quad: np.ndarray | None = None,
) -> SolverResult:
    """EM + smoothing (see _run_em_discrepancy), but solved in Glatter-style
    B-spline coefficient space instead of on the full point grid, then
    mapped back via x = B @ c.

    ADDED to directly test Joachim's own stated assumption (2026-10-10)
    that 'the EM strategies might not be extended easily': for the
    B-SPLINE strategy specifically, that assumption turns out to be wrong,
    in the easy direction. em.solve() is fully generic in what its 'A'
    argument physically represents -- it never assumes A is the raw
    point-grid kernel, only that it's a matrix to fit b against with a
    non-negative x. Since B-spline basis functions are themselves
    non-negative (a standard property of the B-spline construction) and
    appear in A_spline = A @ B only as a non-negative linear recombination
    of A's own columns, A_spline has exactly the same sign structure as A,
    and any non-negative coefficient vector c maps to a non-negative
    x = B @ c automatically. So this function is almost entirely a
    thin wrapper: build A_spline once, call the EXISTING, UNCHANGED
    em.solve/discrepancy_principle_search machinery on it, then one extra
    matrix-vector product to map the result back to r-space -- no changes
    needed anywhere in em.py or lambda_search.py.

    This is NOT the case for the region-adaptive (two-segment/block-lambda)
    strategy: that one relies on EM's smoothing strength being tunable via
    a closed-form Hessian/evidence formula split into independent additive
    blocks (bayesian_evidence.log_evidence_blocks), and EM's own smoothing
    parameter h has no equivalent evidence/Hessian machinery at all (its h
    is chosen purely by the discrepancy principle or an L-curve corner on
    the FULL residual, not an analytic marginal-likelihood score) -- a
    multi-region generalization for EM would need a genuinely new, more ad
    hoc 2D (h_lo, h_hi) search with no single well-defined scalar objective
    to split the way log_evidence splits additively, rather than a
    relatively mechanical reuse of existing machinery. So Joachim's
    stated caution about EM extensibility is right for that strategy, and
    backwards for this one.
    """
    B, A_spline, desc = _spline_system(A, n_coeffs, spacing, A_quad)

    def solve_fn(h):
        return em.solve(A_spline, b, db, max_iterations=10000, smoothing_h=h, accelerate=True)

    try:
        search = lambda_search.discrepancy_principle_search(
            solve_fn, target_chi2_r=1.0, param_start=0.3, param_floor=1e-9
        )
        em_result = search.result
        selection_note = f"discrepancy principle, h={search.param:.4g}"
    except RuntimeError:
        # chi2_r=1 unreachable: in coefficient space the spline basis itself
        # is the main regularizer, and the non-negative coefficient fit has a
        # chi2_r floor > 1 (e.g. 1.13 on test.dat with the sphere kernel and
        # 30 coefficients). The point-grid L-curve fallback picks far too
        # much extra smoothing here (chi2_r ~48), so take the least-smoothed
        # (h=0) solution instead, i.e. the closest achievable fit.
        em_result = solve_fn(0.0)
        selection_note = (
            "chi2_r=1 unreachable with this basis (non-negative fit floor "
            f"chi2_r={em_result.chi2_r_history[-1]:.4g}); h=0, basis-only "
            "regularization -- increase the number of B-spline coefficients "
            "to fit closer"
        )

    c = em_result.x
    x = B @ c
    fitted_b = A_spline @ c  # == A @ x
    diagnostics = dict(em_result.diagnostics)
    diagnostics["lambda_selection"] = (
        f"EM + smoothing, Glatter-style B-spline basis ({desc}), {selection_note}"
    )
    return SolverResult(
        x=x, fitted_b=fitted_b, n_iterations=em_result.n_iterations,
        chi2_r_history=em_result.chi2_r_history, roughness_history=em_result.roughness_history,
        converged=em_result.converged, diagnostics=diagnostics,
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


def _run_hansen_maxent(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, target_chi2_r: float = 1.0,
) -> SolverResult:
    """Maximum entropy with Hansen/Elfving's functional (regu.zip maxent.m),
    reformulated 2026-10-10 (see hansen_maxent.py DIAGNOSIS): 1/dI-weighted
    residual, entropy relative to a flat default model m, bound-constrained
    L-BFGS-B instead of the original nonlinear CG (which jammed at the
    positivity bound after a few iterations), and lambda^2 chosen by the
    discrepancy principle (chi2_r = target) with a bracketed search instead
    of a fixed lambda grid. If the target is unreachable the closest
    achievable fit is returned with a warning. x > 0 always: suitable for
    size distributions and positive p(r), not for sign-changing p(r)."""
    m = hansen_maxent.flat_prior_level(A, b, db)
    state = {"x": None}

    def solve_fn(lam2: float) -> SolverResult:
        res = hansen_maxent.solve_lbfgsb(A, b, db, lam2=lam2, m=m, x0=state["x"])
        state["x"] = res.x   # warm start along the search path
        return res

    search = lambda_search.discrepancy_principle_search_bidirectional(
        solve_fn, target_chi2_r=target_chi2_r, param_start=1.0, factor=4.0,
    )
    result = search.result
    note = (f"discrepancy principle (target chi2_r={target_chi2_r:.3g}), "
            f"lambda^2={search.param:.4g}, default model m={m:.3g}")
    if not search.reachable:
        note += _unreachable_note(target_chi2_r, search)
    result.diagnostics["lambda_selection"] = note
    return result


# ------------------------------------------------ merged solvers (2026-10-10)
# Several registry entries differed in a single setting; they are now one
# solver each with an option (SolverSpec.options). Old keys remain usable
# through SOLVER_ALIASES.

def _hansen_eq19_factor(n: int) -> np.ndarray:
    """Upper-triangular R with R^T R = A_eq19, the Hessian of Hansen (2000)
    eq. 19 as printed: S = sum_{j=2}^{N-1} [f_j - (f_{j-1}+f_{j+1})/2]^2
    + f_1^2/2 + f_N^2/2 (curvature form, five-diagonal). Note that the
    matrix stated in the paper's text (a_ii=1, a_i,i+-1=-1/2) is instead the
    Hessian of a first-difference penalty; that one is "hansen_printed"
    (regularization.hansen_smoothness_cholesky)."""
    D = np.zeros((n - 2, n))
    for j in range(n - 2):
        D[j, j:j + 3] = (-0.5, 1.0, -0.5)
    A19 = D.T @ D
    A19[0, 0] += 0.5
    A19[-1, -1] += 0.5
    return np.linalg.cholesky(A19).T


# first entry = default (GUI and function default). hansen_eq19 made the
# default 2026-10-10 (Joachim): the normalized evidence prefers it on
# test.dat (ln Z -153.0 vs -236.2 point grid, -118.8 vs -134.1 B-splines).
_EVIDENCE_PENALTIES = {
    "hansen_eq19": "Hansen (2000) eq. 19: curvature, p(0)=p(Dmax)=0",
    "hansen_printed": "Hansen (2000), matrix as printed: 1st difference, p(0)=p(Dmax)=0",
    "second_derivative": "2nd derivative, free ends",
}


def _normalized_log_evidence(A, b, db, L, lam):
    """log Z = 1/2 logdet(2 lam L^T L) - 1/2 logdet(2 lam L^T L + B)
    - lam |L p|^2 - chi2/2 (up to penalty-independent constants), i.e.
    including the prior normalization that log_evidence() omits. Only this
    form can be compared between penalties. Returns nan when L^T L is
    singular (2nd-derivative operator: improper prior)."""
    if L.shape[0] < L.shape[1]:
        return float("nan")   # rank-deficient penalty: improper prior
    W = 1.0 / np.asarray(db, float) ** 2
    Bh = A.T @ (W[:, None] * A)
    P = L.T @ L
    s1, ld1 = np.linalg.slogdet(2 * lam * P)
    H = 2 * lam * P + Bh
    p = np.linalg.solve(H, A.T @ (W * b))
    s2, ld2 = np.linalg.slogdet(H)
    if s1 <= 0 or s2 <= 0:
        return float("nan")
    c2 = float(np.sum(((A @ p - b) / db) ** 2))
    return 0.5 * ld1 - 0.5 * ld2 - lam * float(p @ P @ p) - c2 / 2


def _run_bayesian_evidence_merged(
    A: np.ndarray, b: np.ndarray, db: np.ndarray,
    penalty: str = "hansen_eq19", target_chi2_r: float | None = None,
) -> SolverResult:
    """Tikhonov/IFT with lambda by Bayesian evidence (Vestergaard & Hansen
    2006), smoothness penalty selectable:
      hansen_eq19       -- DEFAULT. Hansen (2000) eq. 19 (curvature, soft
                           p(0)=p(Dmax)=0); test.dat: favoured by the
                           normalized evidence by ~80 in ln Z
      hansen_printed    -- Hansen (2000) tridiagonal matrix as printed (1st
                           difference, same boundary terms); former
                           bayesian_evidence_hansen
      second_derivative -- plain 2nd-difference operator, free ends; former
                           bayesian_evidence (improper prior: no normalized
                           evidence)
    The diagnostics report the normalized log-evidence so penalties can be
    compared on the same data."""
    n = A.shape[1]
    if penalty == "hansen_printed":
        return _with_logz(_run_bayesian_evidence_hansen(A, b, db, target_chi2_r=target_chi2_r),
                          A, b, db, hansen_smoothness_cholesky(n), penalty)
    if penalty == "second_derivative":
        return _with_logz(_run_bayesian_evidence(A, b, db, target_chi2_r=target_chi2_r),
                          A, b, db, second_derivative_operator(n).toarray(), penalty)
    if penalty == "hansen_eq19":
        L = _hansen_eq19_factor(n)
        svd_A = svd_methods.compute_svd(A)
        scale = float(np.median(svd_A.s)) ** 2
        lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)
        p_map, lam_used, note = _bayesian_evidence_pick_lambda(
            A, b, db, L, lam_grid, target_chi2_r, "Bayesian evidence")
        fitted_b = A @ p_map
        cov = bayesian_evidence.posterior_covariance(A, db, L, lam_used)
        res = SolverResult(
            x=p_map, fitted_b=fitted_b, n_iterations=0,
            chi2_r_history=[chi2_r(b, fitted_b, db)], roughness_history=[],
            converged=True,
            diagnostics={"lambda_selection": note, "lambda": lam_used,
                         "x_sigma": np.sqrt(np.clip(np.diag(cov), 0, None)), "x_cov": cov},
        )
        return _with_logz(res, A, b, db, L, penalty)
    raise ValueError(f"unknown penalty {penalty!r}; use one of {list(_EVIDENCE_PENALTIES)}")


def _with_logz(res, A, b, db, L, penalty):
    """Attach penalty name and normalized log-evidence (at the lambda
    actually used) to the diagnostics."""
    lam = res.diagnostics.get("lambda")
    if lam is not None:
        lz = _normalized_log_evidence(A, b, db, L, lam)
        res.diagnostics["log_evidence_normalized"] = lz
        tail = f", ln Z={lz:.1f}" if np.isfinite(lz) else ", ln Z n/a (improper prior)"
    else:
        tail = ""
    res.diagnostics["penalty"] = penalty
    res.diagnostics["lambda_selection"] = (
        f"[{_EVIDENCE_PENALTIES[penalty]}] {res.diagnostics.get('lambda_selection', '')}{tail}")
    return res


def _run_em_merged(
    A: np.ndarray, b: np.ndarray, db: np.ndarray,
    selection: str = "discrepancy", fallback: str = "lcurve",
) -> SolverResult:
    """EM + smoothing; smoothing strength chosen by the discrepancy
    principle (chi2_r=1, falls back if unreachable) or the L-curve corner.
    Former em_discrepancy / em_lcurve."""
    if selection == "discrepancy":
        return _run_em_discrepancy(A, b, db, fallback=fallback)
    if selection == "lcurve":
        return _run_em_lcurve(A, b, db)
    raise ValueError(f"unknown selection {selection!r}")


def _run_maxent_em_merged(
    A: np.ndarray, b: np.ndarray, db: np.ndarray, prior: str = "adaptive",
) -> SolverResult:
    """EM + MaxEnt (SASfit DR_EM_ME, JAC 2022 eq. 47-55) with constant or
    adaptive prior. Former maxent_constant_prior / maxent_adaptive_prior."""
    if prior == "constant":
        return _run_maxent_constant_prior(A, b, db)
    if prior == "adaptive":
        return _run_maxent_adaptive_prior(A, b, db)
    raise ValueError(f"unknown prior {prior!r}")


_LEGACY_SPECS: dict[str, SolverSpec] = {
    "em_discrepancy": SolverSpec(
        label="EM + smoothing (auto: discrepancy principle)",
        run=_run_em_discrepancy,
        description="Recommended default. Expectation-Maximization with a "
                     "smoothing operator; smoothing strength auto-tuned to hit "
                     "chi2_r=1, falling back to the L-curve corner if that "
                     "target is unreachable. Best-performing solver in this "
                     "package's own validation (chi2_r=1.0000 on the JAC 2022 "
                     "benchmark, correct 60/180nm bimodal peaks).",
        spline_kwargs={"fallback": "least_smoothed"},
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
    "bayesian_evidence_hann_tapered": SolverSpec(
        label="IFT, Bayesian evidence (Hann-tapered boundary) -- alternative to Hansen boundary-constrained",
        run=_run_bayesian_evidence_hann_tapered,
        description="Same Bayesian-evidence-chosen general-form Tikhonov as "
                     "'IFT, Bayesian evidence (Hansen boundary-constrained)', but "
                     "replaces Hansen's HARD p(0)=p(Dmax)=0 boundary condition with "
                     "a soft Hann-window taper over the last quarter of the r-range. "
                     "Motivation: p(r) and I(q) are related by a Fourier sine "
                     "transform, so a sharp r-space edge is expected to leak "
                     "oscillatory ringing into I(q) with slowly-decaying sidelobes "
                     "reaching high q -- a plausible mechanism for the high-q "
                     "noise-tracking this package's own empirical scans found "
                     "unchanged across global lambda, Dmax, n_r, and r_min "
                     "individually. Validated on a synthetic sphere test (known "
                     "ground truth): consistently lower RMSE and a high-q fit size "
                     "closer to the expected chi2 contribution of 1 than the hard "
                     "boundary, across every noise realization tried -- but that "
                     "synthetic test did not reproduce the severity of the real "
                     "test.dat high-q pathology, so check "
                     "tests/diagnose_hansen_tapered.py's real-data comparison before "
                     "treating this as better than the hard-boundary version here.",
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
        label="MaxEnt, Hansen functional (L-BFGS-B, auto: discrepancy principle)",
        run=_run_hansen_maxent,
        description="Maximum entropy regularization with Hansen & Elfving's "
                     "functional ||(Ax-b)/dI||^2 + lambda^2 x log(x/m) (flat "
                     "default model m), solved by bound-constrained L-BFGS-B; "
                     "lambda chosen so that chi2_r = 1. Replaces the original "
                     "nonlinear-CG port of maxent.m, which stalled at the "
                     "positivity bound. Positive solutions only."
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
        spline_kwargs={"fallback": "least_smoothed"},
    ),
    "bayesian_evidence_two_segment": SolverSpec(
        label="IFT, Bayesian evidence (region-adaptive, two-segment lambda)",
        run=_run_bayesian_evidence_two_segment,
        description="Extends the Hansen boundary-constrained solver with TWO "
                     "independent regularization strengths instead of one global "
                     "lambda -- a low-r segment and a high-r segment, each chosen "
                     "by a 2D Bayesian-evidence grid search. Added because a single "
                     "global lambda (evidence-chosen or forced to chi2_r=1) was "
                     "found unable to properly regularize both the low/mid-q bulk "
                     "and the high-q region of real test.dat at once (forcing "
                     "global chi2_r=1 only moved the high-q-specific chi2_r from "
                     "0.029 to 0.088, still ~10x below ideal). Region-adaptive "
                     "regularization in this spirit is an established idea in the "
                     "wider inverse-problems literature (e.g. wavelet shrinkage); "
                     "this is a direct, exact-for-this-penalty generalization of "
                     "Vestergaard & Hansen (2006)'s evidence formula via additive "
                     "regularization blocks, not an ad hoc heuristic. Coarser "
                     "lambda grid than the single-block solvers (14x14 vs 60 "
                     "points) to stay responsive -- a 2D grid search is inherently "
                     "more expensive.",
    ),
    "bayesian_evidence_spline": SolverSpec(
        label="IFT, Bayesian evidence (Glatter-style B-spline basis)",
        run=_run_bayesian_evidence_spline,
        description="Classic Glatter (1977)/GNOM (Svergun, 1992) strategy: p(r) "
                     "represented as ~15-30 cubic B-spline coefficients instead of "
                     "the full point grid, with p(0)=p(Dmax)=0 built into the basis "
                     "itself (not a penalty term) and lambda (a mild coefficient-"
                     "space smoothness penalty) chosen by Bayesian evidence "
                     "maximization. A structurally different fix for the same "
                     "high-q 'ringing' problem that motivated the Hansen/Hann-"
                     "tapered/two-segment solvers: here, near-Nyquist oscillation "
                     "simply isn't representable in the reduced basis at any "
                     "lambda, rather than being suppressed by a penalty. Offered "
                     "as a selectable alternative, not a replacement -- which "
                     "assumption (smooth low-dimensional basis vs. region-adaptive "
                     "point-grid smoothing) fits a given dataset better is an "
                     "empirical question.",
        native_spline=True,
    ),
    "em_discrepancy_spline": SolverSpec(
        label="EM + smoothing, Glatter-style B-spline basis (auto: discrepancy principle)",
        run=_run_em_discrepancy_spline,
        description="Same EM+smoothing solver as 'EM + smoothing (auto: "
                     "discrepancy principle)', but solved in Glatter-style B-spline "
                     "coefficient space (see 'IFT, Bayesian evidence (Glatter-style "
                     "B-spline basis)') instead of on the full point grid. "
                     "Demonstrates that the B-spline strategy, unlike the region-"
                     "adaptive two-segment-lambda strategy, extends to the EM "
                     "family essentially for free: EM's update rule doesn't care "
                     "what its kernel matrix represents, and B-spline basis "
                     "functions are themselves non-negative, so positivity is "
                     "preserved automatically through the basis -- no changes "
                     "needed to em.py itself.",
        native_spline=True,
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


# ------------------------------------------------------------ registry
# Reduced 2026-10-10 (Joachim): duplicates removed now that the B-spline
# representation and the chi2_r=1 target are generic options, and solvers
# differing in one setting merged into one entry with an option.
class _Registry(dict):
    """dict of the offered solvers. Indexing with an old key (alias of a
    merged solver, or a retired solver) still works, so existing scripts
    keep running; iteration/membership only see the offered solvers."""

    def __missing__(self, key):
        spec, fixed = get_solver(key)
        if fixed:
            run = spec.run
            return replace(spec, run=lambda A, b, db, _r=run, _f=fixed, **kw: _r(A, b, db, **{**_f, **kw}))
        return spec


SOLVER_REGISTRY: dict[str, SolverSpec] = _Registry({
    "em_discrepancy": SolverSpec(
        label="EM + smoothing",
        run=_run_em_merged,
        description=(
            "Expectation-Maximization (Lucy-Richardson) with a smoothing "
            "operator, Biggs-Andrews accelerated. Smoothing strength by the "
            "discrepancy principle (chi2_r=1; falls back to the L-curve, or "
            "to no extra smoothing in B-spline mode, if unreachable) or by "
            "the L-curve corner. Needs a non-negative kernel. Validated on "
            "the JAC 2022 benchmark (sphere kernel)."),
        spline_kwargs={"fallback": "least_smoothed"},
        options={"selection": [("discrepancy", "discrepancy principle (chi2_r = 1)"),
                               ("lcurve", "L-curve corner")]},
        option_labels={"selection": "Smoothing choice:"},
    ),
    "maxent_em": SolverSpec(
        label="EM + MaxEnt (SASfit)",
        run=_run_maxent_em_merged,
        description=(
            "EM with an additive entropy term (SASfit DR_EM_ME, JAC 2022 "
            "eq. 47-55), constant or adaptive prior; lambda by a grid search "
            "targeting chi2_r = 1. Needs a non-negative kernel."),
        options={"prior": [("adaptive", "adaptive prior"), ("constant", "constant prior")]},
        option_labels={"prior": "Prior:"},
    ),
    "hansen_maxent": replace(_LEGACY_SPECS["hansen_maxent"],
                             label="MaxEnt, Hansen functional (L-BFGS-B)"),
    "bayesian_evidence": SolverSpec(
        label="IFT / Tikhonov, Bayesian evidence",
        run=_run_bayesian_evidence_merged,
        description=(
            "Tikhonov regularization with lambda chosen by Bayesian evidence "
            "maximization (Vestergaard & Hansen 2006); optional chi2_r = 1. "
            "Smoothness penalty: Hansen (2000) eq. 19 (curvature, "
            "p(0)=p(Dmax)=0; default), Hansen's matrix as printed (1st "
            "difference, same boundary terms), or plain 2nd derivative. The result line "
            "reports the normalized log-evidence ln Z for comparing "
            "penalties (higher is better; not available for the 2nd "
            "derivative, whose prior is improper). Works with signed "
            "kernels; recommended for j0/PDDF."),
        options={"penalty": [(k, v) for k, v in _EVIDENCE_PENALTIES.items()]},
        option_labels={"penalty": "Smoothness penalty:"},
    ),
    "general_tikhonov_gcv": _LEGACY_SPECS["general_tikhonov_gcv"],
    "tikhonov_standard_gcv": _LEGACY_SPECS["tikhonov_standard_gcv"],
    "tsvd_gcv": _LEGACY_SPECS["tsvd_gcv"],
    "arlsnn": _LEGACY_SPECS["arlsnn"],
    "em_general_signed_kernel": replace(_LEGACY_SPECS["em_general_signed_kernel"],
                                        label="EM for signed kernels (Chae et al. 2018)"),
})

# No longer offered in the GUI; kept so that old scripts/keys still run.
#   bayesian_evidence_spline / em_discrepancy_spline: identical to
#     bayesian_evidence(penalty="second_derivative") / em_discrepancy with
#     the generic B-spline option (verified: identical p(r)).
#   bayesian_evidence_two_segment: within 0.3-0.6 % of the Hansen solver,
#     never improved the high-q fit.
#   bayesian_evidence_hann_tapered: superseded by the B-spline option.
RETIRED_SOLVERS: dict[str, SolverSpec] = {
    k: _LEGACY_SPECS[k] for k in (
        "bayesian_evidence_spline", "em_discrepancy_spline",
        "bayesian_evidence_two_segment", "bayesian_evidence_hann_tapered",
    )
}

# Old keys of merged solvers -> (new key, fixed options)
SOLVER_ALIASES: dict[str, tuple[str, dict]] = {
    "em_lcurve": ("em_discrepancy", {"selection": "lcurve"}),
    "maxent_constant_prior": ("maxent_em", {"prior": "constant"}),
    "maxent_adaptive_prior": ("maxent_em", {"prior": "adaptive"}),
    "bayesian_evidence_hansen": ("bayesian_evidence", {"penalty": "hansen_printed"}),
}


def get_solver(key: str) -> tuple[SolverSpec, dict]:
    """Resolve a solver key (current, alias or retired) to (spec, fixed kwargs)."""
    if key in SOLVER_REGISTRY:
        return SOLVER_REGISTRY[key], {}
    if key in SOLVER_ALIASES:
        new, kw = SOLVER_ALIASES[key]
        return SOLVER_REGISTRY[new], dict(kw)
    if key == "bayesian_evidence_plain":
        return SOLVER_REGISTRY["bayesian_evidence"], {"penalty": "second_derivative"}
    if key in RETIRED_SOLVERS:
        return RETIRED_SOLVERS[key], {}
    raise KeyError(key)


def run_solver(
    key: str, A: np.ndarray, b: np.ndarray, db: np.ndarray,
    spline: SplineOptions | None = None, **kwargs,
) -> SolverResult:
    """Run a registered solver, optionally in B-spline coefficient space.

    With spline=None this is SOLVER_REGISTRY[key].run(A, b, db, **kwargs).
    With spline given, the solver is applied unchanged to the (M, n_coeffs)
    matrix A_spline and its result mapped back to the display grid:
    x = B_disp @ c, fitted_b = A_spline @ c, and the Laplace error band
    (if the solver provides "x_cov") as diag(B Cov_c B^T). Solvers that
    only report x_sigma lose it in spline mode (use the bootstrap).

    Works for every solver because B-spline basis functions are
    non-negative: c >= 0 gives p >= 0 (EM/MaxEnt/ARLS-nn keep their
    positivity), and signed solvers simply get signed coefficients.
    The solvers' own smoothness operators then act on neighbouring
    coefficients instead of neighbouring grid points.

    test.dat, 2026-10-10: every Bayesian-evidence variant goes from
    high-q chi2 ~0.02-0.1 (point grid, j0 kernel) to ~1.05 (20 coeffs,
    log knots), and with the sphere kernel the spurious spike at r~3 nm in
    the volume-weighted distribution disappears (peak at 77 nm, like EM).
    """
    spec, fixed = get_solver(key)
    kwargs = {**fixed, **kwargs}
    if spline is None:
        return spec.run(A, b, db, **kwargs)
    if spec.native_spline:
        return spec.run(A, b, db, n_coeffs=spline.n_coeffs, spacing=spline.spacing,
                        A_quad=spline.A_quad, **kwargs)
    B, A_spline, desc = _spline_system(A, spline.n_coeffs, spline.spacing, spline.A_quad)
    res = spec.run(A_spline, b, db, **{**(spec.spline_kwargs or {}), **kwargs})
    c = np.asarray(res.x, dtype=float)
    fitted_b = A_spline @ c
    diag = dict(res.diagnostics)
    cov_c = diag.pop("x_cov", None)
    diag.pop("x_sigma", None)
    if cov_c is not None:
        cov = B @ cov_c @ B.T
        diag["x_cov"] = cov
        diag["x_sigma"] = np.sqrt(np.clip(np.diag(cov), 0, None))
    diag["spline_coefficients"] = c
    diag["lambda_selection"] = f"B-spline basis ({desc}); {diag.get('lambda_selection', '')}"
    hist = list(res.chi2_r_history) if res.chi2_r_history else []
    hist.append(chi2_r(b, fitted_b, db))
    return SolverResult(
        x=B @ c, fitted_b=fitted_b, n_iterations=res.n_iterations,
        chi2_r_history=hist, roughness_history=res.roughness_history,
        converged=res.converged, diagnostics=diag,
    )


def list_solvers() -> list[tuple[str, str]]:
    """(key, label) pairs, e.g. for populating a GUI dropdown."""
    return [(key, spec.label) for key, spec in SOLVER_REGISTRY.items()]
