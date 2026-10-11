"""Data input (same conventions as pyOZgui's ``loadCurve``)."""

from __future__ import annotations

import numpy as np

__all__ = ["load_data"]


def load_data(filename, q_scale=1.0, skip_nonpositive_sigma=True, qmin=None, qmax=None):
    """Read columns q, I, sigma [, dq] from a text file.

    Conventions shared with pyOZgui (``polydisperse_fit.loadCurve``):

    * whitespace-, comma-, semicolon- or tab-separated columns; comment lines
      starting with ``#``, ``%`` or ``//`` and unparsable header lines are skipped;
    * rows with non-finite values or q <= 0 are dropped, the data are sorted by q;
    * a FOURTH column is the q resolution, read as the Gaussian SIGMA of the
      resolution function (not the FWHM; use ``Resolution(..., fwhm=True)`` if
      the reduction writes FWHM).

    ``q_scale`` converts units (e.g. 0.1 for 1/nm -> 1/Angstrom); it is applied to
    q and dq. Returns (q, I, sigma, dq_or_None).
    """
    rows = []
    with open(filename, "r", errors="replace") as fh:
        for line in fh:
            line = line.strip()
            if not line or line[0] in "#%" or line.startswith("//"):
                continue
            for sep in (None, ",", ";", "\t"):
                parts = line.split(sep) if sep else line.split()
                if len(parts) >= 3:
                    try:
                        vals = [float(x) for x in parts[:4]]
                    except ValueError:
                        continue
                    rows.append(vals)
                    break
    if not rows:
        raise ValueError(f"no numeric q, I, sigma rows in {filename}")
    ncol = min(len(r) for r in rows)
    a = np.array([r[:ncol] for r in rows])
    q, I, s = a[:, 0] * q_scale, a[:, 1], a[:, 2]
    dq = a[:, 3] * q_scale if ncol >= 4 else None
    m = np.isfinite(q) & np.isfinite(I) & np.isfinite(s) & (q > 0)
    if dq is not None:
        m &= np.isfinite(dq)
    if skip_nonpositive_sigma:
        m &= s > 0
    if qmin is not None:
        m &= q >= qmin
    if qmax is not None:
        m &= q <= qmax
    o = np.argsort(q[m])
    return q[m][o], I[m][o], s[m][o], (dq[m][o] if dq is not None else None)
