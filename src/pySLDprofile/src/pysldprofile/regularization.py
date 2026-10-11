r"""Regularisation functionals of Hansen (2016), eqs. (14) and (15).

Smoothness (eq. 14)::

    S = sum_j (f_j - (f_{j-1} + f_{j+1})/2)^2

End conditions for the density profile: at r = 0 the profile is continued
symmetrically (f_{-1} = f_1, rho is an even function through the centre);
at r = r_max the end point is either free (no row, Hansen's "free end
points") or forced towards zero (f_N = 0).

Maximum entropy (eq. 15)::

    S = sum_j f_j ln(f_j / m_j) - f_j + m_j

with prior m (requires f > 0, i.e. a positive contrast profile).
"""

from __future__ import annotations

import numpy as np

__all__ = ["smoothness_operator", "Smoothness", "MaxEnt"]


def smoothness_operator(n: int, outer: str = "free", grid=None) -> np.ndarray:
    """Operator D with S = |D f|^2 (eq. 14).  ``outer`` in {'free', 'zero'}.

    Uniform grid (``grid=None``): exactly Hansen's rows f_j - (f_{j-1}+f_{j+1})/2.
    Non-uniform grid (normalised edges ``grid``): the rows are the second
    derivative on the actual node positions x_j (shell centres), scaled by
    -h^2/2 with the mean spacing h and weighted by sqrt(dx_j / h), so that
    S approximates (h^4/4) int f''(x)^2 dx, i.e. smoothness in r, independent
    of how the nodes are distributed. For a uniform grid both are identical.
    """
    if n < 3:
        raise ValueError("need at least 3 points")
    if outer not in ("free", "zero"):
        raise ValueError("outer must be 'free' or 'zero'")
    if grid is None:
        rows = []
        r0 = np.zeros(n)
        r0[0], r0[1] = 1.0, -1.0  # f_0 - (f_1 + f_1)/2
        rows.append(r0)
        for j in range(1, n - 1):
            r = np.zeros(n)
            r[j - 1], r[j], r[j + 1] = -0.5, 1.0, -0.5
            rows.append(r)
        if outer == "zero":
            r = np.zeros(n)
            r[-2], r[-1] = -0.5, 1.0
            rows.append(r)
        return np.array(rows)
    t = np.asarray(grid, float)
    x = 0.5 * (t[1:] + t[:-1])
    dx = np.diff(t)
    h = 1.0 / n
    rows = []
    # centre: rho even in r -> first derivative 0 at r = 0; row ~ (f_0 - f_1) h/(x_1 - x_0)
    r0 = np.zeros(n)
    r0[0], r0[1] = h / (x[1] - x[0]), -h / (x[1] - x[0])
    rows.append(r0 * np.sqrt(dx[0] / h))

    def d2row(j, xm, xp):
        hm, hp = x[j] - xm, xp - x[j]
        w = np.array([2 / (hm * (hm + hp)), -2 / (hm * hp), 2 / (hp * (hm + hp))])
        return -0.5 * h * h * w * np.sqrt(dx[j] / h)

    for j in range(1, n - 1):
        r = np.zeros(n)
        r[j - 1 : j + 2] = d2row(j, x[j - 1], x[j + 1])
        rows.append(r)
    if outer == "zero":
        r = np.zeros(n)
        w = d2row(n - 1, x[n - 2], x[n - 1] + dx[n - 1])  # ghost node with f = 0
        r[-2], r[-1] = w[0], w[1]
        rows.append(r)
    return np.array(rows)


class Smoothness:
    """Quadratic smoothness prior. A small ridge makes A non-singular for free ends."""

    name = "smooth"

    def __init__(self, n: int, outer: str = "free", ridge: float = 1e-6, grid=None):
        self.D = smoothness_operator(n, outer, grid)
        L = self.D.T @ self.D
        self.L = L + ridge * np.trace(L) / n * np.eye(n)
        self._logdetL = np.linalg.slogdet(self.L)[1]

    def value(self, f):
        return float(f @ self.L @ f)

    def grad(self, f):
        return 2.0 * self.L @ f

    def hess(self, f):
        return 2.0 * self.L

    def logdet_hess(self, f):
        n = self.L.shape[0]
        return n * np.log(2.0) + self._logdetL

    def feasible(self, f):
        return True


class MaxEnt:
    """Entropy relative to prior ``m`` (eq. 15)."""

    name = "maxent"

    def __init__(self, m):
        m = np.asarray(m, float)
        if np.any(m <= 0):
            raise ValueError("maximum-entropy prior must be strictly positive")
        self.m = m

    def value(self, f):
        return float(np.sum(f * np.log(f / self.m) - f + self.m))

    def grad(self, f):
        return np.log(f / self.m)

    def hess(self, f):
        return np.diag(1.0 / f)

    def logdet_hess(self, f):
        return float(-np.sum(np.log(f)))

    def feasible(self, f):
        return bool(np.all(f > 0))
