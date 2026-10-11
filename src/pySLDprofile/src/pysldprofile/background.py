r"""Background models added to the particle scattering.

    I_model(q) = (smeared) c(q) F(q)^2  +  B(q; theta)

The background parameters theta can be linear (constant, amplitudes) or
nonlinear (power-law exponent, correlation length). They are not
regularised: they are extra columns of the Gauss-Newton Jacobian with a flat
prior and are integrated out in the Laplace approximation of the evidence,
exactly like the flat background of Hansen (2016).

Positive parameters (amplitudes, exponents, lengths) are fitted as
z = ln(theta), which keeps them positive and makes the Gaussian approximation
better behaved. The background is evaluated at the measured q (not smeared).

Built-in terms
--------------
``Constant``          c
``PowerLaw``          A q^(-n)                 (low-q upturn of large aggregates, Porod n = 4,
                                                mass fractals 1 < n < 3, surface fractals 3 < n < 4)
``OrnsteinZernike``   I_L / (1 + (q xi)^2)     (concentration fluctuations / network blobs)
``FunctionTerm``      any user function f(q, *theta) with a numerical Jacobian

Example::

    bg = Background([Constant(), PowerLaw(n=4.0, fit_n=False)])
    fit = DensityProfileFit(q, I, s, background=bg)
"""

from __future__ import annotations

import numpy as np

__all__ = ["BackgroundTerm", "Constant", "PowerLaw", "OrnsteinZernike", "FunctionTerm", "Background"]


class BackgroundTerm:
    """Base class. Subclasses define ``names``, ``positive``, ``f`` and (optionally) ``grad``."""

    names: tuple = ()
    positive: tuple = ()

    def __init__(self, values=None, free=None):
        k = len(self.names)
        self.values = [None] * k if values is None else list(values)
        self.free = [True] * k if free is None else list(free)

    def f(self, q, th):  # pragma: no cover - abstract
        raise NotImplementedError

    def grad(self, q, th):
        """d f / d theta (len(q) x k); default: central differences."""
        th = np.asarray(th, float)
        out = np.empty((q.size, th.size))
        for j in range(th.size):
            h = 1e-6 * max(abs(th[j]), 1e-8)
            tp, tm = th.copy(), th.copy()
            tp[j] += h
            tm[j] -= h
            out[:, j] = (self.f(q, tp) - self.f(q, tm)) / (2 * h)
        return out

    def guess(self, q, I):
        """Starting values for parameters given as None."""
        return [1.0] * len(self.names)


class Constant(BackgroundTerm):
    names = ("c",)
    positive = (False,)

    def __init__(self, c=None, fit=True):
        super().__init__([c], [fit])

    def f(self, q, th):
        return np.full(q.shape, th[0])

    def grad(self, q, th):
        return np.ones((q.size, 1))

    def guess(self, q, I):
        tail = np.sort(I[np.argsort(q)[-max(3, q.size // 20):]])
        return [max(0.5 * float(np.median(tail)), 0.0)]


class PowerLaw(BackgroundTerm):
    """A q^(-n)."""

    names = ("A", "n")
    positive = (True, True)

    def __init__(self, A=None, n=4.0, fit_A=True, fit_n=True):
        super().__init__([A, n], [fit_A, fit_n])

    def f(self, q, th):
        return th[0] * q ** (-th[1])

    def grad(self, q, th):
        v = q ** (-th[1])
        return np.c_[v, -th[0] * v * np.log(q)]

    def guess(self, q, I):
        n = self.values[1] if self.values[1] is not None else 4.0
        i = int(np.argmin(q))
        return [0.05 * max(I[i], 1e-30) * q[i] ** n, n]


class OrnsteinZernike(BackgroundTerm):
    """I_L / (1 + (q xi)^2)."""

    names = ("I_L", "xi")
    positive = (True, True)

    def __init__(self, I_L=None, xi=None, fit_I_L=True, fit_xi=True):
        super().__init__([I_L, xi], [fit_I_L, fit_xi])

    def f(self, q, th):
        return th[0] / (1.0 + (q * th[1]) ** 2)

    def grad(self, q, th):
        d = 1.0 + (q * th[1]) ** 2
        return np.c_[1.0 / d, -th[0] * 2.0 * q**2 * th[1] / d**2]

    def guess(self, q, I):
        return [0.05 * float(np.max(I)), 1.0 / np.median(q)]


class FunctionTerm(BackgroundTerm):
    """User-defined term ``func(q, *theta)``; Jacobian by finite differences."""

    def __init__(self, func, names, values, positive=None, free=None):
        self.func = func
        self.names = tuple(names)
        self.positive = tuple(positive) if positive is not None else (False,) * len(self.names)
        super().__init__(values, free)

    def f(self, q, th):
        return np.asarray(self.func(q, *th), float) * np.ones_like(q)

    def guess(self, q, I):
        return [v if v is not None else 1.0 for v in self.values]


class Background:
    """Sum of background terms with a common parameter vector.

    Internally the free parameters are packed into z (z = ln theta for positive
    ones); fixed parameters keep their values.
    """

    def __init__(self, terms=()):
        self.terms = list(terms)
        self._ready = False

    # -------------------------------------------------------------- set-up
    def setup(self, q, I):
        """Fill unspecified starting values from the data."""
        q = np.asarray(q, float)
        I = np.asarray(I, float)
        for t in self.terms:
            g = t.guess(q, I)
            t.values = [g[j] if v is None else v for j, v in enumerate(t.values)]
        self._ready = True
        return self

    def _flat(self):
        names, vals, free, pos, owner = [], [], [], [], []
        for it, t in enumerate(self.terms):
            for j, nm in enumerate(t.names):
                names.append(f"{type(t).__name__}.{nm}" if not isinstance(t, FunctionTerm) else nm)
                vals.append(float(t.values[j]))
                free.append(bool(t.free[j]))
                pos.append(bool(t.positive[j]))
                owner.append((it, j))
        return names, np.array(vals), np.array(free, bool), np.array(pos, bool), owner

    @property
    def names(self):
        return self._flat()[0]

    @property
    def free_names(self):
        n, _, fr, _, _ = self._flat()
        return [a for a, f in zip(n, fr) if f]

    @property
    def n_free(self):
        return int(self._flat()[2].sum())

    def z0(self):
        _, v, fr, pos, _ = self._flat()
        z = v[fr].copy()
        p = pos[fr]
        z[p] = np.log(np.maximum(z[p], 1e-300))
        return z

    def theta(self, z):
        """Full parameter vector for packed free parameters z."""
        _, v, fr, pos, _ = self._flat()
        th = v.copy()
        zf = np.asarray(z, float).copy()
        p = pos[fr]
        zf[p] = np.exp(np.clip(zf[p], -700, 700))
        th[fr] = zf
        return th

    # -------------------------------------------------------------- evaluation
    def value(self, q, z=None):
        z = self.z0() if z is None else z
        th = self.theta(z)
        out = np.zeros_like(np.asarray(q, float))
        i = 0
        for t in self.terms:
            k = len(t.names)
            out = out + t.f(q, th[i : i + k])
            i += k
        return out

    def jac(self, q, z=None):
        """d B / d z (len(q) x n_free)."""
        z = self.z0() if z is None else z
        th = self.theta(z)
        _, _, fr, pos, _ = self._flat()
        cols = []
        i = 0
        for t in self.terms:
            k = len(t.names)
            if fr[i : i + k].any():
                g = t.grad(q, th[i : i + k])
                for j in range(k):
                    if fr[i + j]:
                        col = g[:, j]
                        if pos[i + j]:
                            col = col * th[i + j]  # d/dz = theta d/dtheta
                        cols.append(col)
            i += k
        return np.array(cols).T if cols else np.zeros((np.asarray(q).size, 0))

    def constant(self, z=None):
        """Sum of the Constant terms (the 'flat background')."""
        th = self.theta(self.z0() if z is None else z)
        c, i = 0.0, 0
        for t in self.terms:
            k = len(t.names)
            if isinstance(t, Constant):
                c += th[i]
            i += k
        return c

    def describe(self, z=None):
        th = self.theta(self.z0() if z is None else z)
        names, _, fr, _, _ = self._flat()
        return {n: (float(v), bool(f)) for n, v, f in zip(names, th, fr)}

    def to_free_theta(self, z):
        """Free parameters in natural units (for error propagation)."""
        _, _, fr, _, _ = self._flat()
        return self.theta(z)[fr]
