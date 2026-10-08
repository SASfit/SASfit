Usage
=====

Library (headless)
-------------------

.. code-block:: python

   import numpy as np
   from sasfit_inversion.io_utils import load_sas_data
   from sasfit_inversion.background import fit_background
   from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
   from sasfit_inversion.kernels import build_size_distribution_kernel
   from sasfit_inversion.solver_registry import SOLVER_REGISTRY

   data = load_sas_data("tests/data/test.dat")

   # Background: a flat constant, or a fitted backgr + c*q^-alpha power law
   # (note: only the flat `backgr` term is actually subtracted -- see
   # background.py's BackgroundFitResult.evaluate()).
   bg = fit_background(data.q, data.I, q_min=0.094, q_max=0.385, dI=data.dI)
   b = data.I - bg.evaluate(data.q)

   # Dmax: Glatter's consistency condition q_min <= pi/Dmax gives the
   # largest Dmax the data's low-q reach can actually constrain.
   dmax = np.pi / data.q.min()
   r = np.linspace(1.0, dmax, 150)

   kernel_spec = KERNEL_REGISTRY["sinc_4pi"]   # 4*pi*j0(qr), for PDDF/p(r)
   A = build_size_distribution_kernel(data.q, r, kernel_spec.func, alpha=0.0)

   result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, data.dI)
   print("chi2_r =", result.chi2_r_history[-1])
   print(result.diagnostics["lambda_selection"])
   p_r = result.x   # the recovered p(r), same length as r

See :doc:`theory` for which solver to pick and why, and :ref:`solver-registry`
for the full list of available solvers (``SOLVER_REGISTRY``) with their
individual tradeoffs documented in each one's ``description``.

Choosing a kernel
-----------------

``KERNEL_REGISTRY`` (see :doc:`api/sasfit_inversion.kernel_registry`)
holds the available forward-model kernels -- a sphere/size-distribution
form factor family, and the signed ``4*pi*j0(qr)`` kernel for PDDF/p(r)
recovery. Pick the PDDF kernel (``"sinc_4pi"``) for pair-distance-
distribution work; only the **signed-kernel** solvers
(``em_general_signed_kernel``, ``bayesian_evidence_hansen``,
``bayesian_evidence_hann_tapered``) are valid for it, since it goes
negative and the ordinary (non-negative-kernel) EM solvers silently give
garbage otherwise.

GUI
---

.. code-block:: bash

   python -m gui.app
   # or, after an editable install:
   sasfit-inversion-gui

The GUI (:class:`gui.app.MainWindow`) is a thin layer: it loads data, fits
or sets a background, builds the kernel matrix, and calls
``SOLVER_REGISTRY[key].run(A, b, dI)`` -- every solver registered in
``solver_registry.py`` automatically appears in its solver dropdown, so
adding a new solver there is enough to make it available in the GUI too
(no separate GUI wiring needed).

On loading data, the GUI auto-suggests ``r max`` (Dmax) as
``pi / q_min`` (overridable) -- see :doc:`theory` for why.

Running the tests and diagnostics
-----------------------------------

.. code-block:: bash

   pytest tests/ -q

``tests/diagnose_*.py`` are standalone scripts (not pytest-collected) used
during the regularization investigation in :doc:`theory` -- run any of
them directly, e.g.:

.. code-block:: bash

   python tests/diagnose_hansen_tapered.py
