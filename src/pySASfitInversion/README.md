# sasfit-inversion

Standalone Python library (+ a thin Qt GUI) for solving Fredholm integrals
of the first kind arising in small-angle scattering analysis:

- **Size-distribution recovery** from SAS data (the `DR_EM`, `DR_MEM`, ...
  solvers from SASfit's `sasfit_extrapol.c`, reimplemented natively in
  Python rather than wrapped).
- **Pair-distance-distribution function (PDDF) recovery**, p(r), via the
  signed `4*pi*j0(qr)` kernel, including a boundary-constrained Bayesian-
  evidence indirect Fourier transform (IFT) following Hansen (2000) and
  Vestergaard & Hansen (2006), plus a Hann-tapered-boundary variant
  developed in this project to address a high-q overfitting artifact in
  the hard-boundary version (see `docs/theory.rst` / the Sphinx docs for
  the full derivation and validation).

The library has no GUI dependency of its own and is fully usable headless
or scripted; `src/gui/` is a separate, thin PySide6 layer built on top of
it (see `src/sasfit_inversion/__init__.py`'s design-goals docstring).

## Installation

From the project root (editable install, recommended for development):

```bash
pip install -e ".[gui,dev]"
```

Install just the library (no Qt/GUI dependencies) with:

```bash
pip install -e .
```

## Quick start

```python
import numpy as np
from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.background import fit_background
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY

data = load_sas_data("tests/data/test.dat")
bg = fit_background(data.q, data.I, q_min=0.094, q_max=0.385, dI=data.dI)
b = data.I - bg.evaluate(data.q)

r = np.linspace(1.0, np.pi / data.q.min(), 150)  # Dmax from Glatter's q_min <= pi/Dmax
A = build_size_distribution_kernel(data.q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)

result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, data.dI)
print(result.chi2_r_history[-1], result.diagnostics["lambda_selection"])
```

## GUI

```bash
python -m gui.app
# or, after an editable install:
sasfit-inversion-gui
```

## Running the tests

```bash
pytest tests/ -q
# or run an individual script directly, e.g.:
python tests/test_hansen_bayesian_evidence.py
```

`tests/diagnose_*.py` are exploratory diagnostic scripts (not pass/fail
tests) written during the regularization investigation documented in
`docs/theory.rst` -- kept alongside the tests because they're the
reproducible evidence behind several solver design decisions, not because
they belong in a CI suite.

## Documentation

Full documentation (API reference, theory/background on the inversion
methods, and the regularization investigation this project's `bayesian_evidence_hansen`
and `bayesian_evidence_hann_tapered` solvers came out of) is built with
Sphinx from `docs/`:

```bash
pip install -e ".[docs]"
cd docs
make html   # or: make.bat html  on Windows
```

Open `docs/_build/html/index.html` afterwards.

## Project layout

```
pyproject.toml
src/
    sasfit_inversion/      the library (no GUI dependency)
        solvers/           individual solver implementations
        kernel_registry.py, kernels.py       forward-model kernels
        regularization.py                    regularization operators (incl. Hansen's and the Hann taper)
        solver_registry.py                   the GUI-facing, auto-tuned solver dropdown entries
        background.py, io_utils.py           data loading / background subtraction
    gui/
        app.py             thin PySide6 GUI (MainWindow), no numerics of its own
tests/
    test_*.py              pass/fail tests
    diagnose_*.py           exploratory diagnostic scripts (see docs/theory.rst)
    logs/                   saved output of past diagnostic runs
docs/                       Sphinx documentation source
```
