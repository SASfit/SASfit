"""
sasfit_inversion.solvers
=========================

The individual Fredholm-inversion solver implementations (EM, EM+MaxEnt,
signed-kernel EM, Tikhonov/TSVD via SVD, Hansen's boundary-constrained
Bayesian-evidence IFT, Hann-tapered-boundary variant, etc.) plus their
shared support modules (acceleration, base result types, lambda search).

Most callers should go through ``sasfit_inversion.solver_registry`` rather
than importing modules from here directly -- the registry wraps each
solver with its own validated auto-tuning (discrepancy principle, GCV,
Bayesian evidence, ...) and exposes a uniform ``run(A, b, db) -> SolverResult``
interface suitable for a GUI dropdown. Import directly from here only when
you need a specific solver's raw interface (e.g. for a custom lambda scan,
as the tests/diagnose_*.py scripts do).
"""
