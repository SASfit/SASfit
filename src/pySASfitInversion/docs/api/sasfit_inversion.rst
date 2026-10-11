sasfit_inversion
=================

The library's top-level modules. Most of the day-to-day interface is
:data:`sasfit_inversion.solver_registry.SOLVER_REGISTRY` and
:data:`sasfit_inversion.kernel_registry.KERNEL_REGISTRY` -- see
:doc:`../usage`.

sasfit_inversion
-----------------

.. automodule:: sasfit_inversion
   :members:

sasfit_inversion.io_utils
----------------------------

.. automodule:: sasfit_inversion.io_utils
   :members:

sasfit_inversion.background
-------------------------------

.. automodule:: sasfit_inversion.background
   :members:

sasfit_inversion.kernels
----------------------------

.. automodule:: sasfit_inversion.kernels
   :members:

sasfit_inversion.kernel_registry
------------------------------------

.. automodule:: sasfit_inversion.kernel_registry
   :members:

sasfit_inversion.regularization
-----------------------------------

Regularization operators: identity, first and second difference,
Hansen's (2000) boundary-constrained matrix (``hansen_smoothness_cholesky``,
the "printed" variant; eq. 19 itself is built in
``solver_registry._hansen_eq19_factor``), and the retired Hann taper and
two-segment split -- see :doc:`../theory_regularization`.

.. automodule:: sasfit_inversion.regularization
   :members:

sasfit_inversion.spline_basis
---------------------------------

Clamped cubic B-spline basis with :math:`B(0)=B(r_{max})=0` and uniform,
quadratic or logarithmic knots -- see :doc:`../theory_discretization`.

.. automodule:: sasfit_inversion.spline_basis
   :members:

sasfit_inversion.goodness_of_fit
------------------------------------

Error models for data without uncertainties and alternative fit
statistics -- see :doc:`../theory_forward_model`.

.. automodule:: sasfit_inversion.goodness_of_fit
   :members:

sasfit_inversion.uncertainty
--------------------------------

Parametric bootstrap -- see :doc:`../theory_uncertainty`.

.. automodule:: sasfit_inversion.uncertainty
   :members:

sasfit_inversion.lambda_selection
-------------------------------------

Standalone L-curve (Menger-curvature) regularization-parameter selection.
Not imported by ``solver_registry.py`` directly, but it IS imported by
:mod:`sasfit_inversion.solvers.lambda_search` (``find_corner``,
``LCurvePoint``), which ``solver_registry.py`` uses throughout -- so this
module is a required dependency, not an unused leftover (an earlier pass
of this documentation incorrectly flagged it as obsolete and moved it
out, which broke the import chain; it has been restored).

.. automodule:: sasfit_inversion.lambda_selection
   :members:

.. _solver-registry:

sasfit_inversion.solver_registry
------------------------------------

The GUI-facing, auto-tuned solvers. ``run_solver(key, A, b, dI,
spline=None, **options)`` runs any of them, optionally on a B-spline basis
(``SplineOptions``). Every entry of ``SOLVER_REGISTRY`` appears in the GUI;
``SolverSpec.options`` become drop-downs. ``SOLVER_ALIASES`` and
``RETIRED_SOLVERS`` keep old keys working. See :doc:`../usage` for the list
of solvers and options.

.. automodule:: sasfit_inversion.solver_registry
   :members:
