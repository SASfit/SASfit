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

The regularization operators, including Hansen's boundary-constrained
smoothness operator and the Hann-tapered-boundary extension -- see
:doc:`../theory` for the derivations and the investigation that produced
these.

.. automodule:: sasfit_inversion.regularization
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

sasfit_inversion.solver_registry
------------------------------------

.. _solver-registry:

The GUI-facing, auto-tuned solver presets -- every entry in
``SOLVER_REGISTRY`` is a self-contained ``callable(A, b, db) -> SolverResult``
and automatically appears in the GUI's solver dropdown. See :doc:`../theory`
for the reasoning behind the two Bayesian-evidence IFT solvers
(``bayesian_evidence_hansen`` and ``bayesian_evidence_hann_tapered``)
specifically.

.. automodule:: sasfit_inversion.solver_registry
   :members:
