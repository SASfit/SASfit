# _obsolete

Retired files kept for reference rather than deleted outright, preserving
their original path under `src/` or `docs/`. Nothing here is imported by
the package, the GUI, or any test -- each entry below was verified unused
before being moved.

## `src/sasfit_inversion/lambda_selection.py`

Standalone L-curve (Menger-curvature) regularization-parameter selection.
Never imported by `solver_registry.py` (which uses
`sasfit_inversion.solvers.lambda_search` instead) or by any test -- an
orphaned leftover from development, not a maintained alternative
implementation.

## `src/sasfit_inversion/solvers/hansen_ift.py`

Never imported by `solver_registry.py` or any test. Hansen's
boundary-constrained smoothness operator itself is **not** obsolete --
it lives on in `sasfit_inversion.regularization.hansen_smoothness_cholesky`,
used by the `bayesian_evidence_hansen` and `bayesian_evidence_hann_tapered`
solvers (see `docs/theory.rst`). This specific module was an earlier,
unused implementation attempt.

## `docs/report.tex`

The first (hand-written) LaTeX version of the investigation report.
Superseded by `docs/report_build/`, which generates the same report
directly from `docs/installation.rst`, `docs/usage.rst`, and
`docs/theory.rst` via Sphinx's own LaTeX builder (`make latexpdf`), so
there is a single maintained source instead of two documents that can
drift apart. Kept here only as a historical snapshot.
