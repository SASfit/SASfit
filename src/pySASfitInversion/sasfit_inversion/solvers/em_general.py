"""
General-kernel EM for signed functions (Chae, Martin & Walker, 2018),
reconstructed from the `pysasem` notebook's "Known problems and To-Dos ->
General kernel tests" cell (a loaded script named
test_neg_pr_general_kernel.py, dated no later than 3 June 2019), NOT from
the Chae et al. 2018 paper directly -- I have not read that paper itself,
only the notebook's documented lessons learned from implementing it. If you
have the paper on hand, it's worth a direct check against this before
trusting it on real data.

The notebook's own words (quoted verbatim, since it's your own notebook,
not third-party material):

    Lessons learned about an algorithm in Chae et al which are not stated
    clearly in the literature.
    - Do not normalize the extended kernel.
    - Initial p(r) can be a constant with a positive value.
    - It's better to keep non-extended p(r). Extend it when needed
      preferably in a p(r) updating method/function.
    - In an iterating routine, you only duplicate p(r) along r axis with
      the reversed sign.
    - After update, only p(r) for the positive kernel is needed for the
      next iteration, which, surprisingly, means you have to discard the
      half of the result. This is because p(r) for the negative kernel is
      not guaranteed to be -p(r) of the positive kernel.
    - When calculating f(x):
      - subtract the offset value before duplicating p(r) for the negative
        kernel.
      - set the negative sign for the negative kernel as in the
        definition.

Working out the algebra behind that (my reconstruction, not from the
notebook): for a signed kernel K (M x N), split

    K_pos = where(K >= 0, K, 0)
    K_neg = where(K < 0, -K, 0)          # non-negative magnitude
    K_ext = hstack([K_pos, K_neg])        # M x 2N, entirely non-negative

Then for ANY p (signed or not), K_ext @ hstack([p, -p]) == K @ p exactly
(the two halves' contributions recombine to the true signed sum). That
identity is what lets you run the ordinary non-negative multiplicative EM
update (same em_step as em.py) on the extended, non-negative system. But
because K_pos and K_neg are different matrices, the EM correction factors
for the "p" half and the "-p" half diverge after one update -- hence you
can only trust one half (the notebook keeps the K_pos-associated half) and
must reconstruct the extension from scratch each iteration.

The "subtract the offset value before duplicating p(r) for the negative
kernel" and "set the negative sign for the negative kernel as in the
definition" notes (for computing f(x), i.e. the forward/fitted curve, not
the update itself) are NOT reconstructed here -- I don't have enough
context from the notebook fragment to be confident I'd get that right, so
`forward_model` below is a plain K @ p and should be checked against the
notebook's actual fx-calculation cells before relying on it.

STATUS: FAILS its own sanity check (tests/test_em_general_basic.py) --
diverges to huge magnitudes with ~0.09 correlation to the known truth, and
throws a divide-by-zero warning in em_step's column-sum normalization.
This matches the notebook's own warning ("probably due to lines filled
with only zero in extended kernels when separated into positive and
negative kernels") -- some K_neg (or K_pos) columns are entirely zero for
r values where the kernel never changes sign, making that column's sum
zero and the EM correction factor blow up. The notebook mentions
`prenorm=False` as a workaround for a related issue but I don't know
what `prenorm` actually does without the real source. Do not use this
module on real data as-is -- it needs either the actual
`pysasem.distribution_update` source or the Chae et al. (2018) paper
itself to fix properly, not another guess.
"""
from __future__ import annotations

import numpy as np

from .base import SolverResult, chi2_r
from .em import em_step


def split_kernel(K: np.ndarray) -> np.ndarray:
    """Build the non-negative extended kernel K_ext = [K_pos, K_neg]."""
    K_pos = np.where(K >= 0, K, 0.0)
    K_neg = np.where(K < 0, -K, 0.0)
    return np.hstack([K_pos, K_neg])


def forward_model(K: np.ndarray, p: np.ndarray) -> np.ndarray:
    """
    Plain forward model K @ p. NOTE: the notebook's fx-calculation for the
    general-kernel case has additional offset-subtraction / sign handling
    (see module docstring) that is not reproduced here -- verify against
    the notebook before trusting this for anything beyond a sanity check
    on the update rule itself.
    """
    return K @ p


def solve(
    K: np.ndarray,
    b: np.ndarray,
    db: np.ndarray,
    p0: np.ndarray | None = None,
    max_iterations: int = 10_000,
) -> SolverResult:
    """
    General-kernel EM for a signed p(r), per the reconstruction above.

    Parameters
    ----------
    K : (M, N) signed kernel matrix.
    b : (M,) data.
    db : (M,) uncertainty on b.
    p0 : initial p(r); per the notebook, "can be a constant with a positive
         value" -- defaults to a small positive constant.
    """
    n = K.shape[1]
    K_ext = split_kernel(K)  # (M, 2N), non-negative

    p = p0.copy() if p0 is not None else np.full(n, 1e-3)

    chi2_history = []
    roughness_history = []

    for _ in range(max_iterations):
        p_ext = np.hstack([p, -p])              # "duplicate along r with reversed sign"
        p_ext_updated = em_step(p_ext, K_ext, b)  # ordinary non-negative EM update
        p = p_ext_updated[:n]                     # "discard the half of the result"

        fitted_b = forward_model(K, p)
        chi2_history.append(chi2_r(b, fitted_b, db))
        roughness_history.append(float(np.sum(np.diff(p) ** 2)))

    fitted_b = forward_model(K, p)
    return SolverResult(
        x=p,
        fitted_b=fitted_b,
        n_iterations=max_iterations,
        chi2_r_history=chi2_history,
        roughness_history=roughness_history,
        converged=False,
        diagnostics={"note": "general-kernel EM, reconstructed from pysasem notebook; "
                              "verify against Chae et al. 2018 directly before trusting on real data"},
    )
