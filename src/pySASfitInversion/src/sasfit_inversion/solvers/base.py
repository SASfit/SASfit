from __future__ import annotations

from dataclasses import dataclass, field

import numpy as np


@dataclass
class SolverResult:
    x: np.ndarray                    # recovered solution vector
    fitted_b: np.ndarray              # A @ x, for comparison against the
                                       # background-subtracted input data
    n_iterations: int
    chi2_r_history: list[float] = field(default_factory=list)
    roughness_history: list[float] = field(default_factory=list)
    converged: bool = False
    diagnostics: dict = field(default_factory=dict)


def chi2_r(b: np.ndarray, fitted_b: np.ndarray, db: np.ndarray) -> float:
    """Weighted sum of squared residuals, eq. 38 of the JAC 2022 paper."""
    return float(np.mean(((fitted_b - b) / db) ** 2))


def g_test(b: np.ndarray, fitted_b: np.ndarray) -> float:
    """G-test goodness-of-fit, eq. 40 of the JAC 2022 paper."""
    b_norm = b / b.sum()
    fitted_norm = fitted_b / fitted_b.sum()
    # guard against zeros in b_norm (would make ln(.) blow up); the C code
    # presumably has an equivalent guard -- flagged for cross-checking.
    mask = b_norm > 0
    return float(2 * np.sum(b_norm[mask] * np.log(b_norm[mask] / fitted_norm[mask])))
