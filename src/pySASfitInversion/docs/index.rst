sasfit-inversion
=================

Standalone Python library (+ a Qt GUI) for solving Fredholm integrals of
the first kind arising in small-angle scattering: recovering a
**pair-distance distribution function (PDDF)** p(r) or a **size
distribution** from measured I(q).

Features: EM, EM + smoothing and EM + MaxEnt (SASfit), EM for signed
kernels with smoothing or MaxEnt, Bayesian-evidence IFT with Hansen's
penalties, Tikhonov/TSVD with GCV, ARLS, Hansen's MaxEnt; point grids
(uniform, quadratic, logarithmic) with linear or cubic interpolation, or a
cubic B-spline basis for any solver; analytic and bootstrap uncertainties.

.. toctree::
   :maxdepth: 2
   :caption: Contents

   installation
   usage
   gui
   theory
   references
   api/index

Indices and tables
-------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
