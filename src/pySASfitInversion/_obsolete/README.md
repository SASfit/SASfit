# _obsolete

Retired files kept for reference rather than deleted outright, preserving
their original path under `src/` or `docs/`. Each entry below was checked
against the package's and tests' import graph before being moved.

## `src/sasfit_inversion/solvers/hansen_ift.py`

Never imported by `solver_registry.py`, `solvers/__init__.py`, or any test
(checked directly, not just by filename). Hansen's boundary-constrained
smoothness operator itself is **not** obsolete -- it lives on in
`sasfit_inversion.regularization.hansen_smoothness_cholesky`, used by the
`bayesian_evidence_hansen` and `bayesian_evidence_hann_tapered` solvers
(see `docs/theory.rst`). This specific module was an earlier, unused
implementation attempt.

## `docs/report.tex`

The first (hand-written) LaTeX version of the investigation report.
Superseded by `docs/report_build/`, which generates the same report
directly from `docs/installation.rst`, `docs/usage.rst`, and
`docs/theory.rst` via Sphinx's own LaTeX builder (`make latexpdf`), so
there is a single maintained source instead of two documents that can
drift apart. Kept here only as a historical snapshot.

## NOT here (corrected 2026-10-08): `src/sasfit_inversion/lambda_selection.py`

This was moved here on 2026-10-08 on the mistaken belief that it was
unused, based on checking only `solver_registry.py`'s own imports. That
check was wrong: `solvers/lambda_search.py` imports `LCurvePoint` and
`find_corner` from it directly (`from ..lambda_selection import
LCurvePoint, find_corner`), and `lambda_search.py` is used throughout
`solver_registry.py`. Moving it broke the import chain and crashed the
GUI (`ModuleNotFoundError: No module named 'sasfit_inversion.lambda_selection'`)
the next time it was actually run. It has been restored to
`src/sasfit_inversion/lambda_selection.py` and is a live, required module,
not obsolete. Noted here as a caution: "not imported by X" is not the same
check as "not imported by anything X depends on" -- the latter needs the
full dependency graph, not just the one entry-point module's own imports.
