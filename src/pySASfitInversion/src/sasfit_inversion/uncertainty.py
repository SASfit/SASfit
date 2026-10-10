"""
Parametric bootstrap for pointwise uncertainty on a recovered distribution
x(r) (or N(s)), applicable to ANY solver in solver_registry.py -- unlike
bayesian_evidence.posterior_covariance (a Laplace/Hessian approximation
specific to the Bayesian-evidence solver family), this makes no
linearity/positivity assumptions about the solver at all, so it also
covers the EM/MaxEnt/ARLS/SVD solvers, at the cost of actually re-solving
many times instead of a single cheap matrix inverse.

Method: perturb the FITTED intensity (A @ x of the single solve; the
measured b only if no fit is given) by the stated per-point uncertainty
db (assuming independent Gaussian noise
-- the same assumption chi2_r itself already makes throughout this
package), re-run the FULL solve (including its own internal lambda/h
search) on each noisy replicate, and summarize the resulting ensemble of
recovered x(r) curves by their pointwise mean and standard deviation.

Re-running the complete solve (not just the final linear step at a fixed
hyperparameter) on every replicate is deliberate: it propagates
uncertainty in the regularization-strength CHOICE itself, not just the
uncertainty of a linear solve conditioned on one particular lambda/h --
more honest, at the cost of this being exactly as slow as N full solves
(including whatever lambda search each solver does internally).

A consequence worth knowing before reading the output: the bootstrap MEAN
of a nonlinear, positivity-constrained solver (EM, MaxEnt, ARLS) is not
generally equal to a single solve on the unperturbed data -- the ensemble
average can be smoother/biased relative to the single "best" solve,
particularly wherever a replicate's noise pushes x toward (and gets
clipped at) its positivity floor. That's not a bug in this module; it's
the real behavior of the underlying nonlinear solver under their own
positivity constraint, and precisely the kind of thing a bootstrap
reveals and a single point estimate hides.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Callable

import numpy as np

from .solvers.base import SolverResult


@dataclass
class BootstrapResult:
    x_mean: np.ndarray       # (n,) pointwise mean over successful replicates
    x_std: np.ndarray        # (n,) pointwise std over successful replicates
    x_samples: np.ndarray    # (n_ok, n) every successful replicate's x
    n_boot: int               # successful replicates actually used
    n_failed: int              # replicates skipped (solver raised, or produced non-finite x)


def bootstrap_uncertainty(
    solve_fn: Callable[[np.ndarray], SolverResult],
    b: np.ndarray,
    db: np.ndarray,
    n_boot: int = 100,
    rng: np.random.Generator | None = None,
    center: np.ndarray | None = None,
) -> BootstrapResult:
    """
    Parameters
    ----------
    solve_fn : callable(b_replicate) -> SolverResult
        Everything else (kernel matrix A, dI used for weighting/chi2, the
        chosen solver and any of its own fixed kwargs like taper_frac)
        should already be bound in via e.g. functools.partial or a small
        lambda -- see gui/app.py's usage for the exact pattern.
    b : (M,) the real, background-subtracted measured intensity. Used as
        the bootstrap center only if `center` is None (legacy behavior).
    db : (M,) per-point uncertainty on b, used both as the noise
        generating sigma for the parametric bootstrap and (inside
        solve_fn, as always) for chi2 weighting.
    n_boot : number of replicates to attempt. A handful can fail (solver
        non-convergence, a lambda search that can't bracket a root on a
        particular noisy draw, ...) -- those are silently skipped and
        counted in n_failed rather than aborting the whole run, since on
        a reasonably-behaved problem a small failure rate is expected and
        shouldn't block getting an answer from the rest.
    rng : optional numpy Generator for reproducibility; a fresh default
        generator is used otherwise.
    center : (M,) noise-free curve the replicates are generated around --
        pass the single solve's fitted_b. STRONGLY recommended: the
        measured b already contains one realization of noise, so b + noise
        has variance ~2*db^2. Every replicate then has a true chi2_r of ~2,
        and any solver forced to chi2_r=1 (discrepancy principle) must
        overfit it. Found 2026-10-10 on test.dat with
        bayesian_evidence_two_segment + target_chi2_r=1: centering on b
        drove lambda down by up to 1e8 on some replicates (std(p) up to
        5.9, p peak ~1); centering on fitted_b gave std(p) <= 0.014.

    Raises
    ------
    RuntimeError if every single replicate failed (nothing to summarize).
    """
    if rng is None:
        rng = np.random.default_rng()

    base = np.asarray(b if center is None else center, dtype=float)
    samples: list[np.ndarray] = []
    n_failed = 0
    for _ in range(n_boot):
        b_rep = base + rng.normal(0.0, db)
        try:
            result = solve_fn(b_rep)
            x = np.asarray(result.x, dtype=float)
            if x.size == 0 or not np.all(np.isfinite(x)):
                raise ValueError("solver returned an empty or non-finite result")
        except Exception:
            n_failed += 1
            continue
        samples.append(x)

    if not samples:
        raise RuntimeError(
            f"All {n_boot} bootstrap replicates failed -- cannot estimate "
            "uncertainty this way for this solver/data combination."
        )

    X = np.vstack(samples)
    return BootstrapResult(
        x_mean=X.mean(axis=0),
        x_std=X.std(axis=0),
        x_samples=X,
        n_boot=X.shape[0],
        n_failed=n_failed,
    )
