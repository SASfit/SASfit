sasfit_inversion.solvers
==========================

The individual solver implementations. Most callers should go through
:mod:`sasfit_inversion.solver_registry` (:ref:`solver-registry`) instead of
these modules directly.

sasfit_inversion.solvers.base
---------------------------------

.. automodule:: sasfit_inversion.solvers.base
   :members:

sasfit_inversion.solvers.acceleration
------------------------------------------

Biggs-Andrews acceleration, applied by default (``accelerate=True``)
across the EM-family solvers below.

.. automodule:: sasfit_inversion.solvers.acceleration
   :members:

sasfit_inversion.solvers.em
---------------------------------

.. automodule:: sasfit_inversion.solvers.em
   :members:

sasfit_inversion.solvers.em_general
------------------------------------------

EM for signed kernels (Chae, Martin & Walker, 2018, Sec. 6) -- required
for the ``4*pi*j0(qr)`` PDDF kernel, since the ordinary EM update above is
only valid for a non-negative kernel.

.. automodule:: sasfit_inversion.solvers.em_general
   :members:

sasfit_inversion.solvers.maxent
-------------------------------------

.. automodule:: sasfit_inversion.solvers.maxent
   :members:

sasfit_inversion.solvers.hansen_maxent
---------------------------------------------

.. automodule:: sasfit_inversion.solvers.hansen_maxent
   :members:

sasfit_inversion.solvers.svd_methods
--------------------------------------------

Tikhonov/TSVD via SVD, plus GCV-based parameter selection.

.. automodule:: sasfit_inversion.solvers.svd_methods
   :members:

sasfit_inversion.solvers.general_tikhonov
--------------------------------------------------

.. automodule:: sasfit_inversion.solvers.general_tikhonov
   :members:

sasfit_inversion.solvers.bayesian_evidence
---------------------------------------------------

Bayesian evidence maximization (Vestergaard & Hansen, 2006) for choosing
the regularization weight -- generic over the regularization operator
``L``, including the two-hyperparameter block extension
(``log_evidence_blocks`` / ``evidence_search_blocks``) used by the
(ultimately unsuccessful, see :doc:`../theory`) r-segment adaptive-lambda
experiment.

.. automodule:: sasfit_inversion.solvers.bayesian_evidence
   :members:

sasfit_inversion.solvers.hansen_ift -- moved to ``_obsolete/``
------------------------------------------------------------------

Not imported by ``solver_registry.py`` or any test -- an orphaned leftover
from development, moved to
``_obsolete/src/sasfit_inversion/solvers/hansen_ift.py`` (see
``_obsolete/README.md``) rather than deleted outright. Hansen's
boundary-constrained operator itself is not gone -- it lives on in
:func:`sasfit_inversion.regularization.hansen_smoothness_cholesky`, used by
the ``bayesian_evidence_hansen`` and ``bayesian_evidence_hann_tapered``
solvers (see :doc:`../theory`).

sasfit_inversion.solvers.lambda_search
-----------------------------------------------

Discrepancy-principle and L-curve searches, used by the EM-family
solvers in ``solver_registry.py``.

.. automodule:: sasfit_inversion.solvers.lambda_search
   :members:

sasfit_inversion.solvers.arls
---------------------------------

.. automodule:: sasfit_inversion.solvers.arls
   :members:
