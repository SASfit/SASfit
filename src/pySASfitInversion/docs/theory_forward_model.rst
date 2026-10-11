Forward model, data and information content
=============================================

The Fredholm equation
---------------------

All inversions in this package solve a Fredholm integral equation of the
first kind

.. math::
   :label: fredholm

   I(q) - I_{bg} \;=\; \int_0^{r_{max}} K(q,r)\, x(r)\, \mathrm{d}r ,

for the unknown function :math:`x(r)`, given :math:`M` noisy samples
:math:`b_i = I(q_i)-I_{bg}` with uncertainties :math:`\sigma_i`. The
kernel :math:`K` is smooth in :math:`r`, so the integral operator damps
high-frequency components of :math:`x` strongly; inverting it amplifies
noise without bound. The problem is *ill-posed* and every method below is
a way of adding prior information (smoothness, positivity, entropy, a
reduced basis) to obtain a stable, physically meaningful solution.

Kernels
-------

Two kernels are registered in
:data:`sasfit_inversion.kernel_registry.KERNEL_REGISTRY`.

Pair-distance distribution (``sinc_4pi``)
    For a particle with excess scattering length density
    :math:`\Delta\rho(\mathbf r)` the orientationally averaged intensity is
    the Debye/Fourier-sine relation

    .. math::
       :label: pddf

       I(q) = 4\pi \int_0^{D_{max}} p(r)\, j_0(qr)\, \mathrm{d}r ,
       \qquad j_0(x) = \frac{\sin x}{x},

    with :math:`p(r) = r^2\,\tilde\gamma(r)`, :math:`\tilde\gamma` the
    autocorrelation of :math:`\Delta\rho`. Hence
    :math:`K(q,r)=4\pi j_0(qr)`; the factor :math:`r^2` is part of the
    unknown :math:`p(r)`. Properties used throughout:

    * :math:`p(0)=0` (it vanishes like :math:`r^2`) and :math:`p(r)=0`
      for :math:`r \ge D_{max}`; the :math:`r` grid therefore always starts
      at :math:`r=0`.
    * The kernel is **signed**: :math:`j_0(qr)<0` for
      :math:`\pi<qr<2\pi`, etc. Methods that require a non-negative kernel
      (plain EM, EM + MaxEnt) are not applicable.
    * :math:`p(r)` may itself be negative for particles with an internal
      contrast that changes sign (core–shell, contrast variation). No
      positivity is imposed by default.
    * There is no size weighting (``uses_alpha=False``).

Sphere size distribution (``sphere_rg``)
    For a dilute polydisperse system of homogeneous spheres of radius
    :math:`R` with number distribution :math:`N(R)`

    .. math::
       :label: sphere

       I(q) = \int_0^{\infty} N(R)\, R^6\,
       \left[ \frac{3\left(\sin qR - qR\cos qR\right)}{(qR)^3}\right]^2
       \mathrm{d}R ,

    up to the constant :math:`(4\pi/3)^2\Delta\rho^2`. The factor
    :math:`R^6\propto V^2` must be kept: it sets the *relative* weight of
    small and large particles. The kernel is non-negative. The variable
    :math:`r` of the grid is the **radius**, so the suggested
    :math:`r_{max}` is :math:`D_{max}/2`.

Size weighting
    Following Kohlbrecher & Bressler (2022, eq. 32–34) the unknown is
    rescaled as

    .. math::
       :label: alpha

       x(R) = N(R)\,R^{\alpha}, \qquad
       K_\alpha(q,R) = R^{-\alpha} K(q,R),

    which leaves :eq:`fredholm` unchanged. :math:`\alpha=0, 3, 6` give
    number, volume and volume-squared (intensity) weighted distributions.
    For the sphere kernel :math:`\alpha=6` divides the :math:`R^6` out of
    the kernel; with :math:`\alpha=0` the columns of :math:`\mathbf{A}`
    span about 15 decades and the EM iteration fails. At :math:`R=0` the
    column is set to zero for :math:`\alpha>0`.
    :func:`sasfit_inversion.kernels.recover_N_from_x` undoes the
    substitution.

Background
----------

:func:`sasfit_inversion.background.fit_background` fits

.. math::

   I(q) \approx I_{bg} + c\,q^{-\alpha_{bg}}

by weighted non-linear least squares in a chosen :math:`q` range. **Only
the constant** :math:`I_{bg}` is subtracted (``BackgroundFitResult.evaluate``
deliberately evaluates the power law with :math:`c=0`); the power law only
serves to separate the flat background from the decaying particle
scattering in the fit. ``extrapolation_risk`` measures in units of the fit
range's own log width how far the data extend outside the fit range,
because a power law extrapolated by a decade can exceed the signal. A
fixed, user-given constant can be used instead.

Information content and the choice of the grid
----------------------------------------------

Maximum dimension
    Glatter's consistency condition :math:`q_{min}\le\pi/D_{max}` gives
    the largest dimension the low-:math:`q` end of the data can constrain.
    The GUI suggests

    .. math::

       D_{max} \approx \frac{\pi}{q_{min}},\qquad
       r_{max} = \begin{cases} D_{max} & \text{PDDF}\\
                               D_{max}/2 & \text{sphere radius}\end{cases}

    For ``tests/data/test.dat`` (:math:`q_{min}=0.004016\ \mathrm{nm^{-1}}`)
    this is :math:`D_{max}\approx 782` nm. Larger values let the solver
    "improve" :math:`\chi^2` by fitting noise at mid :math:`q`.

Shannon sampling
    :math:`p(r)` on :math:`[0,D_{max}]` and :math:`I(q)` are a Fourier pair.
    The sampling theorem says that :math:`I(q)` is fully determined by its
    values at the Shannon points :math:`q_k = k\pi/D_{max}`, and
    :math:`p(r)` measured up to :math:`q_{max}` is resolved on a length
    scale

    .. math::

       \Delta r_{Shannon} = \frac{\pi}{q_{max}} .

    The number of independent parameters (Shannon channels) in the data is

    .. math::
       :label: shannon

       N_s = \frac{(q_{max}-q_{min})\,D_{max}}{\pi},

    which the GUI suggests as the number of :math:`r` grid points
    (test.dat: :math:`\pi/q_{max}=8.2` nm, :math:`N_s\approx 95` for the
    PDDF, 47 for the sphere radius).

Log-spaced data
    Most SAS data are equidistant in :math:`\log q`. The number of data
    points per Shannon channel is then

    .. math::

       \frac{\mathrm{d}n}{\mathrm{d}k}
       = \frac{\pi}{D_{max}}\,\frac{\mathrm{d}n}{\mathrm{d}q}
       = \frac{\pi}{D_{max}}\,\frac{1}{q\,\Delta\ln q},

    i.e. it decreases like :math:`1/q`. On test.dat it drops to about 0.45
    points per channel at high :math:`q`. A model with one free parameter
    per channel (a point grid with :math:`N\approx N_s`) can then follow
    every single high-:math:`q` point, including its noise -- this was the
    "high-:math:`q` overfitting" analysed in :doc:`theory_investigation`,
    and it is the main reason for the reduced B-spline basis
    (:doc:`theory_discretization`).

Goodness of fit
---------------

.. math::

   \chi^2_r = \frac{1}{M}\sum_{i=1}^{M}
   \frac{\left[(\mathbf{A}\mathbf{x})_i - b_i\right]^2}{\sigma_i^2}

is used as the fit criterion (target value 1 in the discrepancy principle).
For diagnosing *where* a fit is too good or too bad, the residuals are
split into the lower, middle and upper third of the :math:`q` points and
the mean of :math:`[(\mathbf{A}\mathbf{x})_i-b_i]^2/\sigma_i^2` is
reported per third ("low/mid/high-:math:`q` :math:`\chi^2`"); a value far
below 1 in one region means that the model follows the noise there.
Data without error bars
-----------------------

Every solver needs :math:`\sigma_i` (weights, :math:`\chi^2_r`, discrepancy
principle). For files without a :math:`\Delta I` column -- or to check the
given one -- :func:`sasfit_inversion.goodness_of_fit.estimate_errors`
provides three error models.

Local cubic fits (default without :math:`\Delta I`)
    For every point a cubic in :math:`\ln I` versus :math:`\ln q` is fitted
    by least squares to the point and its two neighbours on each side
    (:func:`~sasfit_inversion.goodness_of_fit.local_polynomial_scatter`).
    With :math:`n=5` points and :math:`p=4` parameters the residual sum of
    squares has :math:`\nu=n-p=1` degree of freedom, and

    .. math::

       s_i^2 = \frac{\mathrm{RSS}_i}{\nu} = \frac{1}{n-p}\sum_{k=i-2}^{i+2}
       \left[\ln I_k - P_i(\ln q_k)\right]^2

    is an unbiased estimate of the relative variance
    :math:`\varepsilon_i^2`. (The plain mean square deviation
    :math:`\mathrm{RSS}/n` is too small by :math:`\nu/n=1/5`.) The cubic
    removes the local shape of the curve up to third order. For equidistant
    points the single residual degree of freedom is proportional to the
    fourth difference, :math:`s_i\propto|y_{i-2}-4y_{i-1}+6y_i-4y_{i+1}+y_{i+2}|/\sqrt{70}`.

    A single :math:`s_i` is :math:`|z|` of one Gaussian variable
    (:math:`s_i^2\sim\varepsilon^2\chi^2_1`): unbiased in the variance, but
    its 5–95 % range is :math:`0.06`–:math:`2\,\varepsilon`, and weights
    :math:`1/\sigma_i^2` built from it can be wrong by orders of magnitude.
    The estimates are therefore combined over a window of :math:`N` points
    (default 15) by a running median, scaled to be unbiased for Gaussian
    noise,

    .. math::

       \hat\varepsilon_i = \frac{\mathrm{median}_{j\in\mathcal W_i}\, s_j}
       {\sqrt{\mathrm{median}(\chi^2_\nu)/\nu}}
       \quad(=1.4826\,\mathrm{median}\,s_j \text{ for } \nu=1),\qquad
       \sigma_i = \hat\varepsilon_i\,|I_i| .

    :math:`N=1` is the plain per-point estimate. Near the ends the first or
    last five points are used.

Neighbour interpolation
    Each point is compared with the line through its two neighbours in
    :math:`\ln I` versus :math:`\ln q`,

    .. math::

       d_i = \ln I_i - \left[w_i\ln I_{i-1} + (1-w_i)\ln I_{i+1}\right],\qquad
       w_i = \frac{\ln q_{i+1}-\ln q_i}{\ln q_{i+1}-\ln q_{i-1}},

    :math:`\mathrm{Var}(d_i)=\varepsilon^2(1+w_i^2+(1-w_i)^2)`, combined the
    same way:
    :math:`\hat\varepsilon_i = 1.4826\,\mathrm{median}_{\mathcal W_i}|d_j|/\sqrt{1+w_j^2+(1-w_j)^2}`.
    Only linear trends are removed, so curvature (steep slopes, form-factor
    minima) inflates the estimate.

Comparison (median of :math:`\hat\sigma/\sigma_{true}`, window 15; synthetic
monodisperse spheres with known noise on 91 or 200 log-spaced points, 40
noise realizations; test.dat against its file errors):

.. list-table::
   :header-rows: 1

   * - data
     - 5-point cubic
     - 3-point neighbour
   * - spheres R = 15, 5 %, 91 points
     - 1.06
     - 1.10
   * - spheres R = 30, 3 %, 200 points
     - 1.09
     - 1.16
   * - spheres R = 30, 3 %, 91 points
     - 1.22
     - 1.28
   * - spheres R = 60, 1 %, 91 points
     - 1.60
     - 1.91
   * - test.dat vs file :math:`\Delta I`
     - 1.20
     - 1.18

The remaining overestimate comes from the sharp minima of monodisperse
spheres (few points, low noise); with a smooth curve and 3 % noise the
cubic gives :math:`\langle s^2\rangle/\sigma^2 = 1.015` per point and a
median ratio of 1.03 combined.

Poisson
    :math:`\sigma_i^2 = I_i`, valid only for intensities in counts.

Relative error
    :math:`\sigma_i = r\,|I_i|` with a given :math:`r`.

If the *shape* of :math:`\sigma_i` is right but its overall level is not
(estimated or guessed errors), the Bayesian-evidence solver can estimate
an overall factor (:doc:`theory_regularization`, "Unknown error scale").

Alternative fit statistics
--------------------------

:func:`~sasfit_inversion.goodness_of_fit.fit_statistics` reports, for the
measured and fitted **total** intensities :math:`I_i`, :math:`F_i`
(background included, so that both are positive):

.. math::

   \chi^2_r = \frac1M\sum_i\frac{(I_i-F_i)^2}{\sigma_i^2},\qquad
   \frac{\chi^2_P}{M} = \frac1M\sum_i\frac{(I_i-F_i)^2}{F_i},

.. math::

   \frac{G}{M} = \frac2M\sum_i\left[I_i\ln\frac{I_i}{F_i}-(I_i-F_i)\right],\qquad
   D_{KL} = \sum_i p_i\ln\frac{p_i}{\hat p_i},\quad
   p_i=\frac{I_i}{\sum_jI_j},\ \hat p_i=\frac{F_i}{\sum_jF_j} .

* Pearson's :math:`\chi^2_P` and the Poisson deviance (G-test)
  :math:`G` are the :math:`\chi^2` and the likelihood-ratio statistic for
  Poisson noise, :math:`\sigma_i^2=F_i`. Both are :math:`\approx M` for a
  correct model **only if** :math:`I` is in counts. For intensities in
  other units they scale with the unit (:math:`I\to cI` multiplies both by
  :math:`c`), so they cannot replace :math:`\chi^2_r=1` as a target.
* :math:`D_{KL}` of the normalized curves is scale-free (SASfit's G-test
  is :math:`2D_{KL}`) and needs no errors, but it has no target value: it
  can monitor or compare fits, not choose a regularization parameter.
* Without :math:`\sigma_i` a stopping target therefore needs a noise model
  (above), or a parameter choice that does not use the absolute error
  level: GCV, the L-curve, or the Bayesian evidence with an estimated error
  scale.

All four statistics are shown in the GUI's result panel. The EM solvers
also store SASfit's G-test in ``diagnostics["g_test_final"]``.
