"""
Named, pluggable solver presets for interactive selection (e.g. a GUI
dropdown), matching the kernel_registry.py pattern: each entry is a
self-contained callable(A, b, db) -> SolverResult, so the GUI never needs
to know about smoothing parameters, lambda searches, or solver-specific
kwargs -- it just picks a key and calls .run(A, b, db).

Only the solvers validated as reliable general-purpose choices in this
package are exposed here (EM+smoothing with automatic parameter selection,
and ARLS's automatic non-negative least squares). The others built in this
package (Tikhonov/TSVD/GCV, Bayesian evidence, Hansen's CG MaxEnt, the
signed-kernel EM) are NOT included -- either because they're consistently
weaker for this problem class (general_tikhonov, svd_methods -- see their
own docstrings' PERFORMANCE FINDING notes), not robust enough yet
(hansen_maxent), or outright broken (em_general). Add them here once/if
they're trusted for general use; a half-working option in a GUI dropdown
is worse than no option.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import numpy as np

from .solvers.base import SolverResult, chi2_r
from .solvers import em, lambda_search, arls, maxent as maxent_em


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
    lambda_search.py and this package's own test findings)."""
    def solve_fn(h):
        return em.solve(A, b, db, max_iterations=5000, smoothing_h=h)

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
    (Menger curvature). Doesn't assume chi2_r=1 is achievable."""
    def solve_fn(h):
        return em.solve(A, b, db, max_iterations=3000, smoothing_h=h)

    lc = lambda_search.l_curve_search(solve_fn, param_start=0.3, n_points=25)
    result = lc.result
    result.diagnostics["lambda_selection"] = f"L-curve corner, h={lc.param:.4g}"
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


SOLVER_REGISTRY: dict[str, SolverSpec] = {
    "em_discrepancy": SolverSpec(
        label="EM + smoothing (auto: discrepancy principle)",
        run=_run_em_discrepancy,
        description="Expectation-Maximization with a smoothing operator; the "
                     "smoothing strength is auto-tuned to hit chi2_r=1. Falls "
                     "back to the L-curve corner if that target is unreachable.",
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
                     "multi-modal distributions (see arls.py's own findings) "
                     "but useful as a zero-configuration sanity check.",
    ),
}


def list_solvers() -> list[tuple[str, str]]:
    """(key, label) pairs, e.g. for populating a GUI dropdown."""
    return [(key, spec.label) for key, spec in SOLVER_REGISTRY.items()]
