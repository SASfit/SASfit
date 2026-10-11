Theory
======

This part of the documentation describes the mathematics behind every
step of an inversion with *sasfit-inversion*: the forward model, how the
integral is turned into a linear system, how that system is regularized,
how the regularization strength is chosen, and how the uncertainty of the
result is estimated. The last chapter records the investigations that led
to the present set of methods (what was tried, what failed and why).

.. toctree::
   :maxdepth: 2

   theory_forward_model
   theory_discretization
   theory_regularization
   theory_em_maxent
   theory_uncertainty
   theory_investigation

Notation
--------

.. list-table::
   :widths: 22 78
   :header-rows: 0

   * - :math:`q_i,\ i=1\dots M`
     - scattering vector magnitudes of the :math:`M` data points
   * - :math:`b_i,\ \sigma_i`
     - background-subtracted intensity :math:`I(q_i)-I_{bg}` and its
       standard uncertainty :math:`\Delta I(q_i)`
   * - :math:`r`, :math:`D_{max}`, :math:`r_{max}`
     - real-space variable (pair distance or radius), maximum particle
       dimension, upper end of the :math:`r` grid
   * - :math:`p(r)`, :math:`x(r)`
     - unknown function: pair-distance distribution (PDDF) or weighted
       size distribution :math:`x(R)=N(R)\,R^{\alpha}`
   * - :math:`K(q,r)`
     - kernel of the Fredholm integral
   * - :math:`\mathbf{A}\in\mathbb{R}^{M\times N}`, :math:`\mathbf{x}`
     - discretized kernel and vector of unknowns (point values or basis
       coefficients)
   * - :math:`\mathbf{W}=\mathrm{diag}(1/\sigma_i^2)`
     - statistical weights
   * - :math:`\chi^2`, :math:`\chi^2_r`
     - :math:`\chi^2=\sum_i\left[(\mathbf{A}\mathbf{x})_i-b_i\right]^2/\sigma_i^2`;
       in this package :math:`\chi^2_r=\chi^2/M` (no subtraction of the
       number of parameters, as in SASfit's C code)
   * - :math:`\mathbf{L}`, :math:`\lambda`
     - regularization (penalty) operator and its weight

Main references
---------------

The key papers are Glatter (1977) for the B-spline IFT, Moore (1980) for
the information content, Hansen (2000) and Vestergaard & Hansen (2006)
for the Bayesian IFT, Kohlbrecher & Breßler (2022) for SASfit's EM
solvers, Chae, Martin & Walker (2019) for EM with signed kernels,
Silverman *et al.* (1990) for EM with smoothing, and Hansen's
Regularization Tools. The complete, topic-ordered list with notes on where
each work is used is in :doc:`references`.
