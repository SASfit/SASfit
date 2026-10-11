Uncertainty of the recovered distribution
===========================================

Analytic (Laplace) error band
-----------------------------

For the Bayesian-evidence solver the posterior is Gaussian, with
covariance equal to the inverse Hessian of :math:`\lambda S+\chi^2/2` at
the MAP point:

.. math::

   \mathrm{Cov}(\mathbf p) = \left(2\lambda\mathbf L^T\mathbf L
   + \mathbf A^T\mathbf W\mathbf A\right)^{-1},\qquad
   \sigma_{p_j} = \sqrt{\mathrm{Cov}_{jj}} .

It contains the noise propagation *and* the prior's uncertainty for a
fixed :math:`\lambda`, but not the uncertainty of :math:`\lambda` itself.
In B-spline mode the coefficient covariance is mapped to the display grid
with :math:`\mathbf B\,\mathrm{Cov}(\mathbf c)\,\mathbf B^T`. The band is
shown automatically ("MAP solution" with :math:`\pm1\sigma`).

Parametric bootstrap
--------------------

:func:`sasfit_inversion.uncertainty.bootstrap_uncertainty` works for every
solver (including the non-linear EM and MaxEnt solvers and the
automatic choice of the regularization parameter, which is repeated for
every replicate):

.. math::

   \mathbf b^{(k)} = \hat{\mathbf b} + \boldsymbol\varepsilon^{(k)},\qquad
   \varepsilon^{(k)}_i\sim\mathcal N(0,\sigma_i^2),\qquad k=1\dots N_{boot},

.. math::

   \bar{\mathbf x} = \frac1{N_{boot}}\sum_k\mathbf x^{(k)},\qquad
   \sigma_{x_j}^2 = \frac1{N_{boot}}\sum_k\left(x^{(k)}_j-\bar x_j\right)^2 .

Replicates whose solve fails are skipped and counted. The relative error
of :math:`\sigma_{x_j}` is about :math:`1/\sqrt{2N_{boot}}` (10 % for
:math:`N_{boot}=50`).

Centre of the replicates
    The replicates are generated around the **fitted** curve
    :math:`\hat{\mathbf b}=\mathbf A\mathbf x` of the single solve, not
    around the measured :math:`\mathbf b`. The measured data already
    contain one realization of noise; :math:`\mathbf b+\boldsymbol\varepsilon`
    has variance :math:`2\sigma_i^2` and every replicate a true
    :math:`\chi^2_r\approx2`. A solver forced to :math:`\chi^2_r=1` must then
    remove most of its regularization on such replicates: on test.dat
    centring on :math:`\mathbf b` drove :math:`\lambda` down by up to
    :math:`10^8` (std of :math:`p` up to 5.9 for a peak of 1), centring on
    the fit gave std :math:`\le0.014`. The floor
    :math:`\lambda\ge10^{-3}\lambda_{ev}` of the forced-:math:`\chi^2`
    search is a second safeguard.

The GUI plots the bootstrap mean with a :math:`\pm1\sigma` band and the
single solve as a dashed line; for non-linear solvers the two need not
coincide.
