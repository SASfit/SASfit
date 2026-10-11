"""
Error models for data without (or with doubtful) uncertainties, and
goodness-of-fit statistics beyond chi2_r (2026-10-10).

Error models (``estimate_errors``)
----------------------------------
Every solver needs per-point standard uncertainties sigma_i (weights,
chi2_r, discrepancy principle). If the file has no dI column, one of these
models supplies them (formulas in the documentation, chapter "Forward
model, data and information content"):

* ``"scatter"``: sigma estimated from the point-to-point scatter of the
  data. ln I_i is compared with the interpolation (in ln q) of its two
  neighbours; the running median (15 points) of the normalized deviations,
  times 1.4826, is the relative error. Robust against form-factor minima
  and outliers, follows a q-dependent noise level. test.dat: median ratio
  to the file's dI 1.18, all points within a factor 0.54-2.35.
* ``"poly"``: local polynomial fits (default: cubic through the point and
  two neighbours on each side, in ln I vs ln q); sigma^2 = RSS/(n - p) of
  the fit, optionally combined over a window of points. Less biased by
  curvature than "scatter", noisier per point (1 degree of freedom).
* ``"poisson"``: sigma_i^2 = I_i, for I in counts.
* ``"relative"``: sigma_i = r * abs(I_i).

Fit statistics (``fit_statistics``)
-----------------------------------
For measured and fitted total intensities (background included) the
reduced chi2 (needs sigma), Pearson's chi2/M (sigma^2 = F), the Poisson
deviance (G test)/M and the KL divergence of the normalized curves.
Pearson and deviance are ~1 for a correct model only if I is in counts
and scale with the unit of I; KL is scale-free (SASfit's G test = 2 KL)
but has no target value, so it can monitor fits but not replace
chi2_r = 1 in the discrepancy principle.
"""
from __future__ import annotations

import numpy as np

ERROR_MODELS = ("file", "scatter", "poly", "poisson", "relative")


def _running_median(x: np.ndarray, half: int) -> np.ndarray:
    n = len(x)
    out = np.empty(n)
    for i in range(n):
        lo, hi = max(0, i - half), min(n, i + half + 1)
        out[i] = np.median(x[lo:hi])
    return out


def relative_scatter(q: np.ndarray, I: np.ndarray, window: int = 15) -> np.ndarray:
    """Relative error eps_i estimated from the point-to-point scatter of
    ln I versus ln q (see module docstring). Points with I <= 0 are
    bridged; window = number of points of the running median."""
    q = np.asarray(q, float)
    I = np.asarray(I, float)
    n = len(q)
    if n < 5:
        raise ValueError("need at least 5 points to estimate the scatter")
    ok = (I > 0) & (q > 0)
    lq = np.log(np.where(ok, q, np.nan))
    lI = np.log(np.where(ok, I, np.nan))
    idx = np.flatnonzero(ok)
    eps = np.full(n, np.nan)
    if len(idx) < 5:
        raise ValueError("need at least 5 positive points to estimate the scatter")
    li, lqq = lI[idx], lq[idx]
    w = (lqq[2:] - lqq[1:-1]) / (lqq[2:] - lqq[:-2])
    d = li[1:-1] - (w * li[:-2] + (1 - w) * li[2:])
    e = np.abs(d) / np.sqrt(1 + w**2 + (1 - w) ** 2)
    e = np.concatenate([[e[0]], e, [e[-1]]])
    e = 1.4826 * _running_median(e, max(window // 2, 2))
    eps[idx] = e
    # bridge non-positive points with the nearest estimate
    if not ok.all():
        eps = np.interp(np.arange(n), idx, eps[idx])
    return np.maximum(eps, 1e-6)


def local_polynomial_scatter(
    q: np.ndarray, I: np.ndarray, degree: int = 3, half_width: int = 2,
    window: int = 1, log: bool = True, combine: str = "median",
) -> np.ndarray:
    """Error estimate from local polynomial fits (Joachim's method,
    2026-10-10): for every point a polynomial of `degree` is fitted to the
    point and its `half_width` neighbours on each side (default: cubic
    through 5 points); the scatter of the points about that fit is the
    error estimate for the central point.

    With n = 2*half_width + 1 points and p = degree + 1 parameters the
    residual sum of squares has nu = n - p degrees of freedom, so

        s_i^2 = RSS_i / nu

    is an unbiased estimate of sigma^2 (the plain mean square deviation
    RSS/n would be too small by nu/n, i.e. by a factor sqrt(1/5) = 0.45 in
    sigma for a cubic through 5 points). For the cubic through 5 points
    nu = 1: each s_i is abs(z) of a single Gaussian variable z (for equidistant
    points z is proportional to the 4th difference y_{i-2} - 4y_{i-1} +
    6y_i - 4y_{i+1} + y_{i+2}), so single-point estimates scatter
    strongly (chi-square with 1 degree of freedom: 0.03 sigma - 2.2 sigma
    at the 5/95 % levels).

    window > 1 combines the s_i of `window` neighbouring points:
    combine="median" (robust): median(s) / sqrt(median(chi2_nu)/nu),
    i.e. x 1.4826 for nu = 1; combine="rms": sqrt(mean(s^2)) (unbiased for
    sigma^2, sensitive to outliers). window = 1 is the plain per-point
    estimate. Points near the ends use the first/last n points.

    log=True fits ln I versus ln q and returns RELATIVE errors (sigma/I);
    log=False fits I versus q and returns absolute errors. The cubic
    removes local curvature up to third order, so it is less biased than
    the 3-point linear comparison of relative_scatter near steep slopes and
    form-factor minima, at the price of fewer degrees of freedom per point.
    """
    from scipy.stats import chi2 as _chi2
    q = np.asarray(q, float)
    I = np.asarray(I, float)
    n_win = 2 * half_width + 1
    nu = n_win - (degree + 1)
    if nu < 1:
        raise ValueError("need 2*half_width + 1 > degree + 1 points per fit")
    ok = (I > 0) & (q > 0) if log else np.isfinite(I)
    idx = np.flatnonzero(ok)
    if len(idx) < n_win:
        raise ValueError(f"need at least {n_win} usable points")
    x = np.log(q[idx]) if log else q[idx]
    y = np.log(I[idx]) if log else I[idx]
    m = len(idx)
    s = np.empty(m)
    for i in range(m):
        lo = min(max(i - half_width, 0), m - n_win)
        xs, ys = x[lo:lo + n_win], y[lo:lo + n_win]
        xc = (xs - xs.mean()) / (np.ptp(xs) or 1.0)       # conditioning
        coef = np.polyfit(xc, ys, degree)
        r = ys - np.polyval(coef, xc)
        s[i] = np.sqrt(np.sum(r**2) / nu)
    if window > 1:
        half = window // 2
        if combine == "median":
            s = _running_median(s, half) / np.sqrt(_chi2.ppf(0.5, nu) / nu)
        elif combine == "rms":
            s = np.sqrt(np.convolve(s**2, np.ones(2 * half + 1), "same")
                        / np.convolve(np.ones(m), np.ones(2 * half + 1), "same"))
        else:
            raise ValueError("combine must be 'median' or 'rms'")
    out = np.full(len(q), np.nan)
    out[idx] = s
    if not ok.all():
        out = np.interp(np.arange(len(q)), idx, out[idx])
    return np.maximum(out, 1e-6 if log else 1e-300)


def estimate_errors(
    q: np.ndarray, I: np.ndarray, model: str = "scatter",
    relative: float = 0.03, window: int = 15,
    degree: int = 3, half_width: int = 2, combine: str = "median",
) -> np.ndarray:
    """Per-point standard uncertainties for data without dI (see module
    docstring for the models). "poly": local polynomial fits
    (local_polynomial_scatter, default cubic through 5 points, in ln I vs
    ln q); window = 1 gives the plain per-point estimate."""
    I = np.asarray(I, float)
    if model == "scatter":
        return relative_scatter(q, I, window) * np.maximum(np.abs(I), 1e-300)
    if model == "poly":
        return local_polynomial_scatter(q, I, degree, half_width, window,
                                        combine=combine) * np.maximum(np.abs(I), 1e-300)
    if model == "poisson":
        pos = I[I > 0]
        floor = pos.min() if pos.size else 1.0
        return np.sqrt(np.maximum(I, floor))
    if model == "relative":
        if relative <= 0:
            raise ValueError("relative error must be > 0")
        pos = np.abs(I[I != 0])
        floor = relative * (pos.min() if pos.size else 1.0)
        return np.maximum(relative * np.abs(I), floor)
    raise ValueError(f"unknown error model {model!r}; use 'scatter', 'poisson' or 'relative'")


def fit_statistics(I: np.ndarray, F: np.ndarray, sigma: np.ndarray | None = None) -> dict:
    """chi2_r (if sigma given), Pearson chi2/M, Poisson deviance/M and the
    KL divergence of the normalized curves (see module docstring). I and F
    should be TOTAL intensities (background included). Points with
    non-positive I or F are left out of the Poisson statistics and of kl."""
    I = np.asarray(I, float)
    F = np.asarray(F, float)
    out: dict[str, float] = {"n_points": float(len(I))}
    if sigma is not None:
        out["chi2_r"] = float(np.mean(((I - F) / sigma) ** 2))
    pos_f = F > 0
    out["pearson_r"] = float(np.sum((I[pos_f] - F[pos_f]) ** 2 / F[pos_f]) / max(pos_f.sum(), 1))
    m = pos_f & (I >= 0)
    with np.errstate(divide="ignore", invalid="ignore"):
        t = np.where(I[m] > 0, I[m] * np.log(I[m] / F[m]), 0.0) - (I[m] - F[m])
    out["deviance_r"] = float(2 * np.sum(t) / max(m.sum(), 1))
    m2 = pos_f & (I > 0)
    p = I[m2] / I[m2].sum()
    ph = F[m2] / F[m2].sum()
    out["kl"] = float(np.sum(p * np.log(p / ph)))
    out["n_excluded"] = float(len(I) - m2.sum())
    return out
