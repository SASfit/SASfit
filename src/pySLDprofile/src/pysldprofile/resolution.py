r"""Instrumental smearing.

Because the model intensity is quadratic in rho, smearing cannot be applied
to the amplitude.  A resolution object instead provides a fine q-grid q' and
a matrix R such that

    I_smeared(q_i) = sum_m R_im I(q'_m).

Then C_i = sum_m R_im c(q'_m) a(q'_m) a(q'_m)^T in Hansen's notation.
"""

from __future__ import annotations

import numpy as np

__all__ = ["NoResolution", "Resolution", "RicianResolution", "GaussianResolution", "SlitResolution", "MatrixResolution"]


class NoResolution:
    """Point-like resolution (R = identity)."""

    def setup(self, q):
        q = np.asarray(q, float)
        return q, None


class Resolution:
    r"""Pinhole Q-resolution as a fixed linear operator -- the same operator as
    ``pyozgui.polydisperse_fit.Resolution`` (pyOZgui), so both programs smear identically.

    ``kernel="rician"`` (default) or ``"gaussian"``.

    Rician is the physically correct kernel: the instrument broadens Q
    isotropically in the two-dimensional detector plane and the data are then
    radially averaged, so the distribution of the magnitude |Q| about the
    nominal Q0 is

        P(Q|Q0) = (Q/s^2) exp(-(Q^2 + Q0^2)/2s^2) I0(Q Q0/s^2),

    evaluated as (Q/s^2) exp(-(Q-Q0)^2/2s^2) i0e(Q Q0/s^2) to avoid overflow.
    A Gaussian in Q is its Q0/s >> 1 limit; it is adequate for small dQ/Q but
    puts weight at Q < 0 for large dQ/Q (lowest-Q points). The factor Q makes
    the Rician weight vanish at the origin.

    Conventions (identical to pyOZgui):

    * ``dq`` is the kernel's SIGMA; ``fwhm=True`` converts a FWHM (/2.3548).
    * The model is evaluated on an extended grid Qext = linspace(lo, hi,
      oversample*M) united with the measured Q, lo = max(min(Q - nSigma dQ),
      1e-3 Q_min), hi = max(Q + nSigma dQ); including the measured Q makes
      dQ -> 0 reproduce the unsmeared curve exactly.
    * Trapezoid weights on Qext; every row normalised to 1; a row that falls
      between grid points falls back to the nearest node.
    * The background is NOT smeared (it is added after smearing, in the model).

    ``dq`` may be a scalar or an array with one value per data point.
    """

    def __init__(self, dq, kernel: str = "rician", fwhm: bool = False, nSigma: float = 3.0, oversample: int = 3):
        if kernel not in ("rician", "gaussian"):
            raise ValueError(f"kernel must be 'rician' or 'gaussian', got {kernel!r}")
        self.dq = dq
        self.kernel = kernel
        self.fwhm = bool(fwhm)
        self.nSigma = float(nSigma)
        self.oversample = int(oversample)

    def setup(self, q):
        Q = np.asarray(q, float)
        dQ = np.broadcast_to(np.asarray(self.dq, float), Q.shape).astype(float)
        if self.fwhm:
            dQ = dQ / 2.3548200450309493
        lo = max(float(np.min(Q - self.nSigma * dQ)), float(Q.min()) * 1e-3)
        hi = float(np.max(Q + self.nSigma * dQ))
        n = max(int(self.oversample * Q.size), Q.size + 2)
        Qext = np.unique(np.concatenate([np.linspace(lo, hi, n), Q]))
        dq = np.gradient(Qext)
        with np.errstate(divide="ignore", invalid="ignore", over="ignore"):
            if self.kernel == "gaussian":
                z = (Qext[None, :] - Q[:, None]) / dQ[:, None]
                W = np.exp(-0.5 * z * z) * dq[None, :]
            else:
                from scipy.special import i0e

                s2 = (dQ[:, None]) ** 2
                Qe = Qext[None, :]
                Q0 = Q[:, None]
                x = Qe * Q0 / s2
                W = (Qe / s2) * np.exp(-0.5 * ((Qe - Q0) ** 2) / s2) * i0e(x) * dq[None, :]
                W = np.where(Qe > 0.0, W, 0.0)
        W = np.where(np.isfinite(W), W, 0.0)
        norm = W.sum(axis=1, keepdims=True)
        bad = (norm[:, 0] <= 0) | ~np.isfinite(norm[:, 0])
        if np.any(bad):
            for i in np.where(bad)[0]:
                W[i] = 0.0
                W[i, int(np.argmin(np.abs(Qext - Q[i])))] = 1.0
            norm = W.sum(axis=1, keepdims=True)
        self.Q, self.Qext, self.W = Q, Qext, W / norm
        return Qext, self.W

    def __repr__(self):
        return f"Resolution(kernel={self.kernel!r}, fwhm={self.fwhm}, nSigma={self.nSigma}, oversample={self.oversample})"


class RicianResolution(Resolution):
    """Rician pinhole resolution (default kernel); ``dq`` = sigma (or FWHM with ``fwhm=True``)."""

    def __init__(self, dq, fwhm: bool = False, nSigma: float = 3.0, oversample: int = 3):
        super().__init__(dq, "rician", fwhm, nSigma, oversample)


class GaussianResolution(Resolution):
    """Gaussian pinhole resolution (large-Q/dQ limit of the Rician), same operator as pyOZgui."""

    def __init__(self, dq, fwhm: bool = False, nSigma: float = 3.0, oversample: int = 3, n_nodes=None):
        super().__init__(dq, "gaussian", fwhm, nSigma, oversample)


class SlitResolution:
    r"""Slit smearing with a Gaussian beam-length profile (Glatter 1977; Hansen 2016, sect. 3.1).

        I_m(q) = 2 \int_0^\infty P(t) I(sqrt(q^2 + t^2)) dt,
        P(t)   = exp(-t^2 / c^2) / (c sqrt(pi))      (2 \int_0^\infty P = 1)

    ``c`` is the profile width in q units.  The t-integral is done with
    Gauss-Hermite (half-range) quadrature.
    """

    def __init__(self, c: float, n_nodes: int = 16):
        self.c = float(c)
        self.n_nodes = int(n_nodes)

    def setup(self, q):
        q = np.asarray(q, float)
        x, w = np.polynomial.hermite.hermgauss(2 * self.n_nodes)
        sel = x > 0
        t = self.c * x[sel]
        wt = 2 * w[sel] / np.sqrt(np.pi)  # 2 * int_0^inf exp(-x^2)/sqrt(pi) f = sum wt f
        wt = wt / wt.sum()
        qf = np.sqrt(q[:, None] ** 2 + t[None, :] ** 2).ravel()
        R = np.zeros((q.size, qf.size))
        k = t.size
        for i in range(q.size):
            R[i, i * k : (i + 1) * k] = wt
        return qf, R


class MatrixResolution:
    """User supplied fine grid ``q_fine`` and smearing matrix ``R`` (len(q) x len(q_fine))."""

    def __init__(self, q_fine, R):
        self.q_fine = np.asarray(q_fine, float)
        self.R = np.asarray(R, float)

    def setup(self, q):
        if self.R.shape != (len(q), self.q_fine.size):
            raise ValueError("R has wrong shape")
        return self.q_fine, self.R
