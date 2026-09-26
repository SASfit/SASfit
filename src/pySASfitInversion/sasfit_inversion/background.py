"""
Background determination: fit I(q) = backgr + c * q^(-alpha) over a
user-selectable q-range, then subtract it from the full curve before
handing the result to a Fredholm solver.

This is the piece flagged back in the sasfit-extrapolate-background work
as a prerequisite for inversion. Kept deliberately simple (a 3-parameter
power-law-plus-constant fit via scipy.optimize.curve_fit) rather than
porting sasfit_extrapol.c's full Porod/Guinier/Zimm/OZ extrapolation
machinery -- this is the specific model Joachim asked for here
(backgr + c*q^-alpha), not a general extrapolation-model library. If a
different background functional form is needed later, add a new fit
function alongside fit_background rather than generalizing this one
prematurely.

CAVEAT found during development, not theoretical: subtracting this model
far outside the q-range it was fit over is dangerous for a power law --
a background fit at high q and then subtracted a decade lower in q can
end up an order of magnitude LARGER than the actual signal there, because
c*q^-alpha amplifies whatever uncertainty alpha has enormously outside the
fit range. See BackgroundFitResult.extrapolation_risk() below, which the
GUI should check and warn on before subtracting over a much wider range
than the fit was done on.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np
from scipy.optimize import curve_fit


def background_model(q: np.ndarray, backgr: float, c: float, alpha: float) -> np.ndarray:
    """backgr + c * q^(-alpha)."""
    return backgr + c * np.asarray(q, dtype=float) ** (-alpha)


@dataclass
class BackgroundFitResult:
    backgr: float
    c: float
    alpha: float
    param_errors: np.ndarray  # 1-sigma errors from the covariance diagonal, same order as (backgr, c, alpha)
    q_min: float
    q_max: float

    def evaluate(self, q: np.ndarray) -> np.ndarray:
        """Evaluate the fitted background model over an arbitrary q array."""
        return background_model(q, self.backgr, self.c, self.alpha)

    def subtract(self, q: np.ndarray, I: np.ndarray) -> np.ndarray:
        """I - fitted background, evaluated at the same q as I."""
        return I - self.evaluate(q)

    def extrapolation_risk(self, q: np.ndarray) -> float:
        """
        A simple diagnostic for how far `q` extends outside the fit range,
        in log-q units relative to the fit range's own (log) width -- 0
        means q stays within [q_min, q_max], and values >> 1 mean the
        background model is being extrapolated well beyond where it was
        constrained by data, which for a power law can amplify small
        parameter uncertainties enormously (see module docstring). This is
        a real, not theoretical, failure mode -- caught during development
        when a c*q^-alpha background fit at high q, extrapolated a decade
        down to low q, ended up ~30x larger than the actual signal there.
        """
        q = np.asarray(q, dtype=float)
        log_span = np.log10(self.q_max) - np.log10(self.q_min)
        if log_span <= 0:
            return np.inf
        below = np.maximum(np.log10(self.q_min) - np.log10(q), 0)
        above = np.maximum(np.log10(q) - np.log10(self.q_max), 0)
        return float(np.max(below + above) / log_span)


def fit_background(
    q: np.ndarray,
    I: np.ndarray,
    q_min: float,
    q_max: float,
    dI: np.ndarray | None = None,
    p0: tuple[float, float, float] | None = None,
) -> BackgroundFitResult:
    """
    Fit backgr + c*q^-alpha to the data in [q_min, q_max].

    Parameters
    ----------
    q, I : full data arrays.
    q_min, q_max : the fit range (inclusive) -- e.g. from a GUI range
        selector.
    dI : optional uncertainties, used as curve_fit's sigma (absolute_sigma=True)
        if given.
    p0 : optional initial guess (backgr, c, alpha). If not given, a
        heuristic guess is constructed from the data in the fit range:
        backgr ~ min(I) in range, alpha ~ 4 (Porod-like default), c solved
        from the highest-q point in range given that alpha and backgr guess.
    """
    q = np.asarray(q, dtype=float)
    I = np.asarray(I, dtype=float)
    mask = (q >= q_min) & (q <= q_max)
    if mask.sum() < 3:
        raise ValueError(
            f"Need at least 3 data points in the fit range [{q_min}, {q_max}]; "
            f"found {mask.sum()}."
        )
    q_fit, I_fit = q[mask], I[mask]
    sigma = dI[mask] if dI is not None else None

    if p0 is None:
        backgr_guess = float(np.min(I_fit))
        alpha_guess = 4.0
        q_hi = float(q_fit[np.argmax(q_fit)])
        I_hi = float(I_fit[np.argmax(q_fit)])
        c_guess = max((I_hi - backgr_guess) * q_hi**alpha_guess, 1e-12)
        p0 = (backgr_guess, c_guess, alpha_guess)

    popt, pcov = curve_fit(
        background_model, q_fit, I_fit, p0=p0, sigma=sigma,
        absolute_sigma=sigma is not None, maxfev=10_000,
    )
    perr = np.sqrt(np.diag(pcov)) if pcov is not None and np.all(np.isfinite(pcov)) else np.full(3, np.nan)

    return BackgroundFitResult(
        backgr=float(popt[0]), c=float(popt[1]), alpha=float(popt[2]),
        param_errors=perr, q_min=q_min, q_max=q_max,
    )
