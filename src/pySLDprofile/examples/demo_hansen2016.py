"""Reproduce the simulated examples of Hansen, J. Appl. Cryst. 49 (2016) 856-865.

    python examples/demo_hansen2016.py            # writes PNG + data files to examples/demo_out

Runs without installing the package: if ``pysldprofile`` is not importable,
the sibling ``../src`` directory is put on ``sys.path`` automatically.
"""

import os
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
try:
    import pysldprofile  # noqa: F401  (installed, e.g. pip install -e .)
except ImportError:
    sys.path.insert(0, str(HERE.parent / "src"))

import matplotlib

matplotlib.use("Agg")
import numpy as np

from pysldprofile import DensityProfileFit, SlitResolution, models

out = str(HERE / "demo_out")
os.makedirs(out, exist_ok=True)

# --- 1. homogeneous sphere, r = 7.5 nm, 5 % noise, slit smearing (paper Fig. 1)
res = SlitResolution(0.1)  # Gaussian beam-length profile, width in nm^-1
q = np.linspace(0.12, 1.0, 50)
q, I, s, It, sc = models.simulate(q, models.homogeneous, 7.5, rel_noise=0.05, abs_noise=0.0, resolution=res, seed=1)
fit = DensityProfileFit(q, I, s, geometry="sphere", n=80, resolution=res)
r = fit.fit(verbose=True)
print(r.summary())
r.save(f"{out}/sphere_homogeneous")
r.plot(data=(q, I, s), rho_true=(np.r_[0, 7.5, 7.5, 10], np.sqrt(sc) * np.r_[1, 1, 0, 0])).savefig(f"{out}/sphere_homogeneous.png", dpi=110)

# --- 2. inhomogeneous sphere, r = 5 nm, 1 % + 1e-4 noise (paper Figs. 5 and 7)
prof = lambda x: 1 - 0.6 * np.exp(-((np.asarray(x) - 3.0) ** 2) / 0.8**2)
q = np.linspace(0.15, 1.5, 50)
q, I, s, It, sc = models.simulate(q, prof, 5.0, rel_noise=0.01, abs_noise=1e-4, seed=2)
rt = np.linspace(0, 5, 200)
for reg in ("smooth", "maxent"):
    fit = DensityProfileFit(q, I, s, geometry="sphere", n=80, regularization=reg)
    r = fit.fit()
    print(r.summary())
    r.save(f"{out}/sphere_inhomogeneous_{reg}")
    r.plot(data=(q, I, s), rho_true=(rt, np.sqrt(sc) * prof(rt))).savefig(f"{out}/sphere_inhomogeneous_{reg}.png", dpi=110)
