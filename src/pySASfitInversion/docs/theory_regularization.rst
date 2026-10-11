Regularization and choice of the regularization strength
==========================================================

Weighted least squares
----------------------

All linear methods work on the statistically weighted system

.. math::

   \tilde{\mathbf A} = \mathbf{W}^{1/2}\mathbf{A},\qquad
   \tilde{\mathbf b} = \mathbf{W}^{1/2}\mathbf{b},\qquad
   \|\tilde{\mathbf A}\mathbf x-\tilde{\mathbf b}\|^2 = \chi^2 ,

(``solver_registry._weighted``). Without the weights the few
high-intensity low-:math:`q` points dominate: on test.dat :math:`\sigma`
spans four decades (0.01 to 90).

Let :math:`\tilde{\mathbf A} = \mathbf U\boldsymbol\Sigma\mathbf V^T`
with singular values :math:`s_1\ge s_2\ge\dots`. The naive solution
:math:`\mathbf x = \sum_i (\mathbf u_i^T\tilde{\mathbf b}/s_i)\,\mathbf v_i`
divides the noise component of :math:`\mathbf u_i^T\tilde{\mathbf b}` by
singular values that decay to :math:`10^{-15}` (condition number
:math:`\sim10^{16}` for the :math:`j_0` kernel on test.dat). Regularization
replaces :math:`1/s_i` by filtered values.

Tikhonov regularization
-----------------------

General form

.. math::
   :label: tikhonov

   \mathbf x_\lambda = \arg\min_{\mathbf x}\;
   \chi^2(\mathbf x) + \lambda^2\|\mathbf L\mathbf x\|^2
   = \left(\tilde{\mathbf A}^T\tilde{\mathbf A} + \lambda^2\mathbf L^T\mathbf L\right)^{-1}\tilde{\mathbf A}^T\tilde{\mathbf b}.

Standard form (``tikhonov_standard_gcv``)
    :math:`\mathbf L=\mathbf I`; in the SVD basis the filter factors are

    .. math::

       f_i = \frac{s_i^2}{s_i^2+\lambda^2},\qquad
       \mathbf x_\lambda = \sum_i f_i\,\frac{\mathbf u_i^T\tilde{\mathbf b}}{s_i}\,\mathbf v_i .

Truncated SVD (``tsvd_gcv``)
    :math:`f_i = 1` for :math:`i\le k`, 0 otherwise.

General form (``general_tikhonov_gcv``)
    :math:`\mathbf L` is the second-difference operator below. A
    rank-deficient :math:`\mathbf L\in\mathbb R^{p\times N}` is handled by
    Hansen's QR-based transformation to standard form (``std_form``,
    method 1 of Regularization Tools): with :math:`\mathbf L^T = \mathbf K\mathbf R`
    (:math:`\mathbf K=[\mathbf K_p,\mathbf K_0]`, :math:`\mathbf K_0` spanning the
    null space of :math:`\mathbf L`) and :math:`\tilde{\mathbf A}\mathbf K_0=\mathbf H\mathbf T`,

    .. math::

       \mathbf L_p^\dagger = \mathbf K_p\mathbf R^{-T},\quad
       \bar{\mathbf A} = \mathbf H_2^T\tilde{\mathbf A}\mathbf L_p^\dagger,\quad
       \bar{\mathbf b} = \mathbf H_2^T\tilde{\mathbf b},

    the standard-form problem in :math:`\bar{\mathbf x}` is solved with
    filter factors and mapped back by
    :math:`\mathbf x = \mathbf L_p^\dagger\bar{\mathbf x} + \mathbf K_0\mathbf T^{-1}\mathbf H_1^T(\tilde{\mathbf b}-\tilde{\mathbf A}\mathbf L_p^\dagger\bar{\mathbf x})`.
    The result agrees with a direct solve of the normal equations to
    :math:`10^{-11}`.

Smoothness penalties
--------------------

Let :math:`p_1\dots p_N` be the unknowns (grid values or spline
coefficients).

Second difference, free ends (``second_derivative``)
    .. math::

       S_2(\mathbf p) = \sum_{j=2}^{N-1}\left(p_{j-1}-2p_j+p_{j+1}\right)^2
       = \|\mathbf L_2\mathbf p\|^2,\qquad
       \mathbf L_2\in\mathbb R^{(N-2)\times N}.

    The null space of :math:`\mathbf L_2` contains constants and linear
    ramps: these components are not penalized at all. Together with the
    ill-conditioned :math:`j_0` kernel this gave :math:`\chi^2_r` in the
    hundreds on real data with GCV or the L-curve.

Hansen (2000), matrix as printed (``hansen_printed``)
    The text after Hansen's eq. 19 gives the tridiagonal matrix
    :math:`a_{ii}=1`, :math:`a_{i,i\pm1}=-\tfrac12`, which is the Hessian of

    .. math::

       S_{pr}(\mathbf p) = \tfrac12\sum_{j=1}^{N-1}(p_j-p_{j+1})^2
       + \tfrac12 p_1^2 + \tfrac12 p_N^2 ,

    a *first-difference* penalty plus the assumption that :math:`p`
    vanishes just outside :math:`[0,D_{max}]`. It has full rank with
    :math:`\det = (N+1)/2^N` (checked at construction).

Hansen (2000) eq. 19 (``hansen_eq19``, default)
    .. math::
       :label: eq19

       S_{19}(\mathbf p) = \sum_{j=2}^{N-1}\left[p_j-\tfrac12(p_{j-1}+p_{j+1})\right]^2
       + \tfrac12 p_1^2 + \tfrac12 p_N^2 ,

    a *curvature* penalty with the same boundary terms; its Hessian
    :math:`\mathbf D^T\mathbf D + \tfrac12(\mathbf e_1\mathbf e_1^T+\mathbf e_N\mathbf e_N^T)`
    (:math:`\mathbf D` with rows :math:`(-\tfrac12,1,-\tfrac12)`) is
    five-diagonal and positive definite. The printed matrix and eq. 19 are
    therefore *not* the same penalty; both are offered. The normalized
    evidence (below) prefers eq. 19 on test.dat (:math:`\ln Z=-153.0` vs
    :math:`-236.2` on the 95-point grid, :math:`-118.8` vs :math:`-134.1`
    with 20 B-splines), which made it the default.

In all three cases :math:`\mathbf L` is a factor with
:math:`\mathbf L^T\mathbf L` = Hessian of :math:`S`
(Cholesky factor for the Hansen penalties).

Bayesian evidence (``bayesian_evidence``)
-----------------------------------------

Following Hansen (2000) and Vestergaard & Hansen (2006) the
regularization is read as a Gaussian prior and the likelihood as Gaussian
noise:

.. math::

   P(\mathbf p\mid\lambda) \propto \exp\!\left(-\lambda S(\mathbf p)\right),
   \qquad
   P(\mathbf d\mid\mathbf p) \propto \exp\!\left(-\tfrac12\chi^2(\mathbf p)\right),
   \qquad S(\mathbf p) = \mathbf p^T\mathbf L^T\mathbf L\,\mathbf p .

Posterior maximum (MAP)
    .. math::
       :label: map

       \mathbf p_\lambda = \arg\min\ \lambda S + \tfrac12\chi^2
       = \left(2\lambda\mathbf L^T\mathbf L + \mathbf B\right)^{-1}\mathbf A^T\mathbf W\mathbf b ,
       \qquad \mathbf B = \mathbf A^T\mathbf W\mathbf A .

    (:math:`\mathbf B` is the Hessian of :math:`\chi^2/2`,
    :math:`\mathbf{A}_S = 2\mathbf L^T\mathbf L` that of :math:`S`.)

Evidence
    Integrating the posterior over :math:`\mathbf p` (exact for this
    linear-Gaussian model) gives the probability of the data for a given
    :math:`\lambda`:

    .. math::
       :label: evidence

       \ln P(\mathbf d\mid\lambda) =
       \tfrac12\ln\det\!\left(\lambda\mathbf A_S\right)
       - \tfrac12\ln\det\!\left(\lambda\mathbf A_S+\mathbf B\right)
       - \lambda S(\mathbf p_\lambda) - \tfrac12\chi^2(\mathbf p_\lambda)
       + \text{const}.

    For the :math:`\lambda` search the first term is replaced by
    :math:`\tfrac{N-1}{2}\ln\lambda` (Vestergaard & Hansen eq. 6; the
    remaining :math:`\lambda`-independent part of
    :math:`\det\mathbf A_S` does not move the maximum). :math:`\lambda` is
    the maximizer on a log grid of 60 values spanning
    :math:`10^{\pm6}\,\mathrm{median}(s_i)^2` (:math:`s_i` singular values of :math:`\mathbf A`).

Normalized evidence (penalty comparison)
    To compare *different* penalties, the full prior normalization must be
    kept:

    .. math::

       \ln Z = \tfrac12\ln\det(2\lambda\mathbf L^T\mathbf L)
       - \tfrac12\ln\det(2\lambda\mathbf L^T\mathbf L+\mathbf B)
       - \lambda S - \tfrac12\chi^2 .

    It is reported in the result line. For the second difference
    :math:`\mathbf L^T\mathbf L` is singular (improper prior) and
    :math:`\ln Z` is not defined.

Unknown error scale (``noise_scale="estimate"``)
    If the errors are known only up to a common factor,
    :math:`\sigma_i\to s\,\sigma_i`, the likelihood normalization adds
    :math:`-M\ln s` to :eq:`evidence` and :math:`s` is a second
    hyperparameter. :func:`sasfit_inversion.solver_registry.estimate_noise_scale`
    maximizes

    .. math::

       E(s) = \max_\lambda \ln P(\mathbf d\mid\lambda, s\boldsymbol\sigma) - M\ln s

    over :math:`\ln s\in[\ln 0.01,\ln 100]` (bounded scalar search, the inner
    maximum on an 80-point :math:`\lambda` grid over 16 decades) and then
    solves with :math:`s\boldsymbol\sigma`. This is the Bayesian analogue
    of estimating the noise level from the residuals corrected for the
    number of fitted parameters (MacKay: at the optimum
    :math:`\chi^2(s\boldsymbol\sigma)=M-\gamma`, :math:`\gamma` the
    effective number of parameters determined by the data). test.dat: with
    :math:`3\Delta I` or :math:`\Delta I/3` the estimate is
    :math:`s=0.323` and :math:`2.91` (product 0.97), and :math:`p(r)` is
    identical to the one with the file errors (correlation 1.0000); with
    errors from the local cubic fits :math:`s=0.91`. :math:`\chi^2_r` is
    reported for the unscaled :math:`\sigma`.

Number of good parameters
    .. math::

       N_g = \sum_j \frac{\mu_j}{\lambda+\mu_j},

    with :math:`\mu_j` the eigenvalues of :math:`\mathbf B` (the
    :math:`\mathbf A_S=\mathbf I` form of Vestergaard & Hansen's eq. 8),
    an estimate of the number of parameters determined by the data;
    compare with :math:`N_s` of :eq:`shannon`.

Posterior covariance
    The Laplace approximation, exact here, gives
    :math:`\mathrm{Cov}(\mathbf p) = (\lambda\mathbf A_S+\mathbf B)^{-1}`,
    shown as the analytic error band (:doc:`theory_uncertainty`).

Discrepancy principle
---------------------

Choose the regularization parameter :math:`\theta` (:math:`\lambda`,
:math:`\lambda^2`, the EM smoothing weight :math:`h` or the entropy weight)
such that

.. math::

   \chi^2_r(\theta) = 1 ,

i.e. the residuals are as large as the stated errors. Two searches are
implemented (:mod:`sasfit_inversion.solvers.lambda_search`):

* ``discrepancy_principle_search`` starts over-smoothed and walks
  :math:`\theta` down geometrically until :math:`\chi^2_r-1` changes sign,
  then refines by Brent's method (EM + smoothing).
* ``discrepancy_principle_search_bidirectional`` decides the direction
  from the sign of :math:`\chi^2_r-1` at the start value, walks
  geometrically until the root is bracketed and refines by Brent. If the
  target is not reachable (plateau of :math:`\chi^2_r` for four steps, or
  the search range is exhausted) the closest achievable solution is
  returned with ``reachable=False`` and a warning in the result line,
  stating which side failed: a too-small basis (cannot fit) or
  overestimated errors (always fits better than the target).

For the Bayesian-evidence solver "Force :math:`\chi^2_r=1`" starts at the
evidence-maximizing :math:`\lambda_{ev}` and allows :math:`\lambda` to
decrease to at most :math:`10^{-3}\lambda_{ev}`. Reaching
:math:`\chi^2_r=1` only by almost removing the regularization means that
the target is wrong for these data (underestimated :math:`\sigma`, or
bootstrap replicates with inflated noise) and would give an oscillating,
unregularized :math:`p(r)`.

When the discrepancy principle fails for EM + smoothing on a point grid,
the L-curve corner is used instead; in B-spline mode :math:`h=0` is used
(the basis regularizes already, and the L-curve picks far too much extra
smoothing there).

L-curve
-------

For a decreasing sequence :math:`\theta_k = \theta_0/1.2^k` the points
:math:`P_k = (\ln\rho_k, \ln\eta_k)` with residual measure
:math:`\rho=\chi^2_r` and roughness :math:`\eta=\sum_j(x_{j+1}-x_j)^2`
are computed, and the corner is the point of maximum Menger curvature of
consecutive triples,

.. math::

   \kappa(P_{k-1},P_k,P_{k+1}) =
   \frac{4\,\mathcal A(P_{k-1},P_k,P_{k+1})}
        {|P_{k-1}P_k|\,|P_kP_{k+1}|\,|P_{k-1}P_{k+1}|},

:math:`\mathcal A` the triangle area. This mirrors the ``Lcorner`` branch
of SASfit's ``DR_EM_smoothing``.

Generalized cross-validation
----------------------------

GCV chooses :math:`\lambda` (or the TSVD rank :math:`k`) by minimizing

.. math::

   G(\lambda) = \frac{\|\tilde{\mathbf A}\mathbf x_\lambda-\tilde{\mathbf b}\|^2}
                     {\left(M - \sum_i f_i\right)^2},

the leave-one-out prediction error in rotation-invariant form (for the
general form evaluated in the standard-form variables, for TSVD with
:math:`\sum_i f_i = k`). Grid: 100 values from
:math:`10^{-3}s_{min}` to :math:`10\,s_{max}`; TSVD :math:`k\le40`.

ARLS (``arlsnn``)
-----------------

Jones' automatically regularized least squares uses the discrete Picard
condition: the Tikhonov coefficients
:math:`g_i(\lambda) = \beta_i s_i/(s_i^2+\lambda^2)`,
:math:`\boldsymbol\beta=\mathbf U^T\tilde{\mathbf b}`, should decay. The
smallest :math:`\lambda` for which a line and a parabola fitted to
:math:`\log_{10}|g_i|` both have non-positive slope is taken, then
:math:`\lambda` is increased as long as the residual stays below twice its
value. Non-negativity: columns with negative coefficients are removed
repeatedly. No parameter has to be chosen; it serves as a quick sanity
check.
