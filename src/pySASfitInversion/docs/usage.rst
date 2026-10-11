Usage (library)
===============

Minimal example
---------------

.. code-block:: python

   import numpy as np
   from sasfit_inversion.io_utils import load_sas_data
   from sasfit_inversion.background import fit_background
   from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
   from sasfit_inversion.kernels import build_size_distribution_kernel
   from sasfit_inversion.solver_registry import run_solver

   data = load_sas_data("tests/data/test.dat")

   # flat background from a fit of backgr + c*q^-alpha (only backgr is
   # subtracted, see the theory chapter)
   bg = fit_background(data.q, data.I, q_min=0.094, q_max=0.385, dI=data.dI)
   b = data.I - bg.evaluate(data.q)

   # grid: Dmax = pi/q_min, N = Shannon number
   dmax = np.pi / data.q.min()
   n = int(round((data.q.max() - data.q.min()) * dmax / np.pi))
   r = np.linspace(0.0, dmax, n)

   f = KERNEL_REGISTRY["sinc_4pi"].func            # 4*pi*j0(qr), PDDF
   A = build_size_distribution_kernel(data.q, r, f, alpha=0.0)

   res = run_solver("bayesian_evidence", A, b, data.dI)
   print(res.chi2_r_history[-1], res.diagnostics["lambda_selection"])
   p_r, p_sigma = res.x, res.diagnostics["x_sigma"]

``run_solver(key, A, b, dI, spline=None, **options)`` is the single entry
point for all solvers. It returns a
:class:`~sasfit_inversion.solvers.base.SolverResult` with ``x`` (the
distribution on the grid), ``fitted_b`` (:math:`\mathbf A\mathbf x`),
``chi2_r_history``, ``converged``, ``n_iterations`` and ``diagnostics``
(``lambda_selection`` text, and depending on the solver ``x_sigma``,
``x_cov``, ``lambda``, ``log_evidence_normalized``, ...).

Solvers and options
-------------------

.. list-table::
   :header-rows: 1
   :widths: 26 30 44

   * - key
     - options (first = default)
     - notes
   * - ``em_discrepancy``
     - ``selection``: ``"discrepancy"`` | ``"lcurve"``
     - EM + smoothing; non-negative kernel
   * - ``maxent_em``
     - ``prior``: ``"adaptive"`` | ``"constant"``
     - SASfit EM + MaxEnt; non-negative kernel
   * - ``hansen_maxent``
     - ``target_chi2_r`` (1.0)
     - Hansen functional, :math:`x>0`
   * - ``bayesian_evidence``
     - ``penalty``: ``"hansen_eq19"`` | ``"hansen_printed"`` |
       ``"second_derivative"``; ``noise_scale``: ``"given"`` |
       ``"estimate"``; ``target_chi2_r`` (None)
     - recommended for the PDDF; Laplace error band, :math:`\ln Z`
   * - ``general_tikhonov_gcv``
     -
     - 2nd difference, GCV
   * - ``tikhonov_standard_gcv``
     -
     - :math:`\mathbf L=\mathbf I`, GCV
   * - ``tsvd_gcv``
     -
     - truncated SVD, GCV
   * - ``arlsnn``
     -
     - ARLS with :math:`x\ge0`, no parameter
   * - ``em_general_signed_kernel``
     - ``regularization``: ``"smoothing"`` | ``"maxent_adaptive"`` |
       ``"maxent_constant"``
     - EM for signed kernels (PDDF)

Example: ``run_solver("bayesian_evidence", A, b, dI, penalty="hansen_printed",
target_chi2_r=1.0)``.

Old keys still work: ``SOLVER_ALIASES`` maps e.g. ``em_lcurve`` to
``("em_discrepancy", {"selection": "lcurve"})`` and
``bayesian_evidence_hansen`` to
``("bayesian_evidence", {"penalty": "hansen_printed"})``; the retired
solvers (``bayesian_evidence_spline``, ``em_discrepancy_spline``,
``bayesian_evidence_two_segment``, ``bayesian_evidence_hann_tapered``) are
kept in ``RETIRED_SOLVERS``. ``get_solver(key)`` resolves any key.

Kernel and solver compatibility
-------------------------------

* ``sinc_4pi`` (signed): ``bayesian_evidence``, the Tikhonov/TSVD solvers,
  ``arlsnn`` (only if :math:`p\ge0` is acceptable) and
  ``em_general_signed_kernel``. ``em_discrepancy`` and ``maxent_em``
  diverge with a signed kernel.
* ``sphere_rg`` (non-negative): every solver; ``em_discrepancy`` is the
  best validated. Use :math:`\alpha=6`.

B-spline representation
-----------------------

Any solver can solve for B-spline coefficients instead of point values:

.. code-block:: python

   from sasfit_inversion.kernels import quadrature_points
   from sasfit_inversion.solver_registry import SplineOptions

   nq = quadrature_points(dmax, data.q.max(), n)          # fine grid
   A_quad = build_size_distribution_kernel(data.q, np.linspace(0, dmax, nq), f)
   opt = SplineOptions(n_coeffs=20, spacing="log", A_quad=A_quad)
   res = run_solver("em_general_signed_kernel", A, b, data.dI, spline=opt,
                    regularization="maxent_adaptive")
   # res.x is p(r) on the grid r; coefficients in res.diagnostics["spline_coefficients"]

``A`` (the kernel on the display grid ``r``) only defines the display
grid; the solver works on ``A_quad @ B``.

Non-uniform grids and interpolation
-----------------------------------

.. code-block:: python

   from sasfit_inversion.kernels import make_r_grid, build_kernel_on_nodes

   r = make_r_grid(dmax, 30, "log")                       # uniform | quadratic | log
   A = build_kernel_on_nodes(data.q, r, f, interpolation="cubic")   # or "linear"
   res = run_solver("bayesian_evidence", A, b, data.dI)

The unknowns are the values at ``r``; between the nodes :math:`p(r)` is
linear or a not-a-knot cubic spline and the kernel is integrated on a fine
grid. Use ``"linear"`` with solvers that need :math:`x\ge0`.

Uncertainty
-----------

.. code-block:: python

   from sasfit_inversion.uncertainty import bootstrap_uncertainty

   solve = lambda b_rep: run_solver("bayesian_evidence", A, b_rep, data.dI)
   boot = bootstrap_uncertainty(solve, b, data.dI, n_boot=50, center=res.fitted_b)
   boot.x_mean, boot.x_std, boot.n_failed

Always pass ``center=res.fitted_b`` (see :doc:`theory_uncertainty`).

Data without error bars
-----------------------

.. code-block:: python

   from sasfit_inversion.goodness_of_fit import estimate_errors, fit_statistics

   d = load_sas_data("tests/data/test_2col.dat")          # q, I only: d.dI is None
   sigma = estimate_errors(d.q, d.I, "poly")             # 5-point cubic fits;
                                                          # or "scatter", "poisson", "relative"
   res = run_solver("bayesian_evidence", A, d.I - 0.1, sigma,
                    noise_scale="estimate")               # overall level by the evidence
   res.diagnostics["noise_scale"]                         # s, errors used = s*sigma
   fit_statistics(d.I, res.fitted_b + 0.1, sigma)         # chi2_r, Pearson, deviance, KL

Size distributions
------------------

.. code-block:: python

   from sasfit_inversion.kernels import recover_N_from_x

   R = np.linspace(0.0, dmax / 2, 150)
   A = build_size_distribution_kernel(data.q, R, KERNEL_REGISTRY["sphere_rg"].func, alpha=6.0)
   res = run_solver("em_discrepancy", A, b, data.dI)
   N = recover_N_from_x(res.x, R, alpha=6.0)      # number distribution
   V = res.x * R**-3.0                             # volume-weighted (x = N R^6)

Tests
-----

The tests are plain scripts (run from the project directory):

.. code-block:: bash

   python tests/test_em_general_basic.py
   for f in tests/test_*.py; do python "$f"; done
   QT_QPA_PLATFORM=offscreen python tests/test_gui_smoke.py
