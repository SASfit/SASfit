EM, maximum entropy and signed kernels
========================================

Expectation–maximization (Richardson–Lucy)
------------------------------------------

For a non-negative kernel :math:`A_{ij}\ge0` and non-negative unknowns,
the EM algorithm maximizes the Poisson log-likelihood

.. math::

   \ell(\mathbf x) = \sum_i \left[ b_i\ln(\mathbf A\mathbf x)_i - (\mathbf A\mathbf x)_i\right],

equivalently it minimizes the Kullback–Leibler divergence
:math:`\mathrm{KL}(\mathbf b\,\|\,\mathbf A\mathbf x)`. The stationarity
condition :math:`\partial\ell/\partial x_j = 0` for :math:`x_j>0`,

.. math::

   \sum_i A_{ij}\frac{b_i}{(\mathbf A\mathbf x)_i} = \sum_i A_{ij},

is turned into the multiplicative fixed-point iteration (Kohlbrecher &
Bressler 2022, eq. 36–37)

.. math::
   :label: em

   x_j^{(k+1)} = x_j^{(k)}\;
   \frac{\displaystyle\sum_i A_{ij}\, b_i/(\mathbf A\mathbf x^{(k)})_i}
        {\displaystyle\sum_i A_{ij}} .

Each step keeps :math:`\mathbf x\ge0` and does not decrease
:math:`\ell`. Columns with :math:`\sum_iA_{ij}=0` (e.g. :math:`r=0` with
:math:`\alpha>0`) are left unchanged. The iteration converges slowly and,
run to convergence without regularization, ends in a noisy maximum
likelihood solution; early stopping acts as an uncontrolled regularizer
("semi-convergence"). All EM solvers here therefore run to convergence
and regularize explicitly.

Stopping criterion: :math:`\|\mathbf x^{(k+1)}-\mathbf x^{(k)}\|_2\le`
``tol`` (SASfit's ``gNorm``), for the accelerated iteration the relative
change of :math:`\|\mathbf x\|`.

EM + smoothing (``em_discrepancy``)
-----------------------------------

After every EM step the solution is smoothed with the tridiagonal operator
(eq. 41–46)

.. math::

   \mathbf S^{(h)} = \begin{pmatrix}
   1-h & h \\
   h & 1-2h & h\\
   & \ddots & \ddots & \ddots\\
   && h & 1-2h & h\\
   &&& h & 1-h\end{pmatrix},\qquad
   \mathbf x^{(k+1)} = \mathbf S^{(h)}\,\mathrm{EM}(\mathbf x^{(k)}),

:math:`0<h\lesssim0.3`; rows sum to one, so the smoothing conserves
:math:`\sum_jx_j` and positivity. (The optional *double smoothing* also
smooths :math:`\ln\mathbf x` before the step,
:math:`\mathbf x\leftarrow\exp(\mathbf S^{(h)}\ln\mathbf x)`.) The fixed
point balances the EM correction against the diffusion-like smoothing;
:math:`h` plays the role of :math:`\lambda`. It is chosen by the
discrepancy principle (start :math:`h=0.3`, floor :math:`10^{-9}`) or the
L-curve corner.

Biggs–Andrews acceleration
--------------------------

All EM-type iterations are accelerated by vector extrapolation (Biggs &
Andrews 1997; :func:`sasfit_inversion.solvers.acceleration.biggs_andrews`,
ported from SASfit's fixed-point solver). With the fixed-point map
:math:`\mathcal T` and the last updates
:math:`\mathbf g_{n} = \mathcal T(\mathbf y_{n})-\mathbf y_{n}`:

.. math::

   \alpha_n = \frac{\mathbf g_{n-1}\cdot\mathbf g_{n-2}}{\mathbf g_{n-2}\cdot\mathbf g_{n-2}},
   \qquad |\alpha_n|\le\frac{n-1}{n+2},

.. math::

   \mathbf y_n = \mathbf x_n + \alpha_n(\mathbf x_n-\mathbf x_{n-1})
   + \tfrac12\alpha_n^2(\mathbf x_n-2\mathbf x_{n-1}+\mathbf x_{n-2}),
   \qquad \mathbf x_{n+1} = \mathcal T(\mathbf y_n),

(second order, ``kin_set_maa=2``; order 1 drops the last term, order 0 is
plain Picard iteration). For non-negative problems :math:`\mathbf y_n` is
clipped to :math:`\ge0` before :math:`\mathcal T` is applied. The fixed
point is unchanged; on the JAC 2022 benchmark the iteration count drops
from more than 200 000 to about 2 800.

EM + MaxEnt (``maxent_em``)
---------------------------

SASfit's DR_EM_ME (Kohlbrecher & Bressler 2022, eq. 47–55) adds an
entropy correction to each EM step,

.. math::

   S(\mathbf x,\mathbf m) = \sum_j\left[-x_j\ln\frac{x_j}{m_j} + x_j - m_j\right],
   \qquad
   \mathbf x^{(k+1)} = \mathbf x^{(k)} + \Delta\mathbf x^{(k)}_{EM} + \Delta\mathbf s^{(k)} .

Constant prior
    :math:`m_j=\bar b/N` fixed and

    .. math::

       \Delta s_j = \lambda\,x_j\left[-\ln\frac{x_j}{m_j}
       + \frac{\sum_l x_l\ln(x_l/m_l)}{\sum_l x_l}\right],

    the gradient of :math:`S` in the EM metric (multiplied by :math:`x_j`),
    projected such that :math:`\sum_j\Delta s_j=0`.

Adaptive prior
    The prior is the current solution smoothed with a Gaussian point
    spread function (Horne 1985),

    .. math::

       m_i = \sum_j\Pi_{ij}x_j,\qquad
       \Pi_{ij} = \frac{1}{c_i}\exp\!\left[-\frac{(i-j)^2}{2\sigma^2}\right],
       \quad\sum_j\Pi_{ij}=1,

    (:math:`\sigma^2=1` grid step squared) and

    .. math::

       \Delta s_j = \lambda\,x_j\left[\frac{\sum_l x_l\ln(x_l/m_l)}{\sum_l x_l}
       - \ln\frac{x_j}{m_j} - 1 + \sum_i\Pi_{ij}\frac{x_i}{m_i}\right].

    Entropy relative to a smoothed version of itself acts as a
    smoothness prior.

:math:`\lambda` is chosen from 24 values between :math:`10^{-9}` and 1 as
the one with :math:`\chi^2_r` closest to 1, every trial run to convergence.

Maximum entropy, Hansen's functional (``hansen_maxent``)
--------------------------------------------------------

Following Hansen & Elfving (``maxent.m`` of Regularization Tools) but with
statistical weights and a flat default model :math:`m`:

.. math::
   :label: hmaxent

   F(\mathbf x) = \sum_i\frac{\left[(\mathbf A\mathbf x)_i-b_i\right]^2}{\sigma_i^2}
   + \lambda^2\sum_j x_j\ln\frac{x_j}{m},\qquad x_j>0 .

.. math::

   \nabla F = 2\tilde{\mathbf A}^T(\tilde{\mathbf A}\mathbf x-\tilde{\mathbf b})
   + \lambda^2\left(1+\ln\frac{\mathbf x}{m}\right),\qquad
   \nabla^2F = 2\tilde{\mathbf A}^T\tilde{\mathbf A}
   + \lambda^2\,\mathrm{diag}(1/x_j).

* Default model: the constant that best fits the data,
  :math:`m = \frac{\mathbf a^T\tilde{\mathbf b}}{\mathbf a^T\mathbf a}`,
  :math:`a_i = \sum_j\tilde A_{ij}`.
* Minimization: bound-constrained L-BFGS-B
  (:math:`x_j\ge10^{-30}m`) in variables scaled by
  :math:`s_j=\sqrt{(\nabla^2F)_{jj}}` at :math:`\mathbf x=m`, followed by a
  Newton polish on the free components (fraction-to-boundary 0.99,
  Armijo backtracking). Without the polish L-BFGS-B stops in the flat
  valley of the ill-conditioned :math:`j_0` problem with :math:`\chi^2_r`
  off by several percent and non-monotonic in :math:`\lambda`.
* :math:`\lambda^2` by the bidirectional discrepancy principle, warm-
  starting each trial from the previous solution.

The original non-linear CG transcription of ``maxent.m`` (kept as
``hansen_maxent.solve``) caps each step where the first component reaches
zero and has no active-set handling; on test.dat it stopped after 2–18
iterations at :math:`F=71\,049` where the minimum of the same functional is
:math:`F=165`.

EM for signed kernels (``em_general_signed_kernel``)
----------------------------------------------------

The :math:`j_0` kernel and signed :math:`p(r)` violate both assumptions of
:eq:`em`. Chae, Martin & Walker (2019, Sec. 6) reduce the general problem
to a non-negative one in two steps.

Shift (signed solution, their eq. 10)
    With a constant :math:`t>\max|p|`,

    .. math::

       \tilde p = p + t \ge 0,\qquad
       \tilde b_i = b_i + t\sum_j |A_{ij}| ,

Kernel split (signed kernel, their eq. 12)
    :math:`\mathbf A = \mathbf A^+ - \mathbf A^-` with
    :math:`\mathbf A^\pm\ge0` (pointwise positive and negative parts),
    extended kernel and unknown

    .. math::

       \mathbf K = \left[\mathbf A^+\ \ \mathbf A^-\right]\in\mathbb R^{M\times2N}_{\ge0},
       \qquad
       \mathbf u = \begin{pmatrix}\mathbf p + t\\ -\mathbf p + t\end{pmatrix}\ge0,
       \qquad \mathbf K\mathbf u = \tilde{\mathbf b} .

    EM :eq:`em` is applied to :math:`(\mathbf K,\tilde{\mathbf b})`. The
    extended problem has more solutions than those of structure
    :math:`[\mathbf p,-\mathbf p]`, so after each step the structure is
    restored from the first half, :math:`\mathbf p\leftarrow\mathbf u_{1:N}-t`
    (an antisymmetrized variant is available). :math:`|p_j|` is clipped to
    :math:`0.999\,t`. Default :math:`t = 10\max_j|p^{ARLS}_j|` from a quick
    ARLS estimate; results are nearly independent of :math:`t` once it
    exceeds :math:`\max|p|`.

:math:`\chi^2` row weighting (2026-10-10)
    EM minimizes :math:`\mathrm{KL}(\tilde{\mathbf b}\|\mathbf K\mathbf u)
    \approx\sum_i(\mathbf K\mathbf u-\tilde{\mathbf b})_i^2/(2\tilde b_i)`:
    the implicit weights are :math:`1/\tilde b_i`. After the shift
    :math:`\tilde b_i\approx t\sum_j|A_{ij}|` is dominated by the shift; for
    :math:`j_0` the row sums do not decay with :math:`q`, and the real signal
    at high :math:`q` is only :math:`10^{-5}`–:math:`10^{-4}` of
    :math:`\tilde b_i` -- EM is blind to these data (test.dat:
    :math:`\chi^2_r\ge5200` without smoothing, for any :math:`t`). Scaling
    each row by

    .. math::

       w_i = \frac{\tilde b_i}{\sigma_i^2}:\qquad
       \mathbf K\to\mathrm{diag}(w)\mathbf K,\quad
       \tilde{\mathbf b}\to\mathrm{diag}(w)\tilde{\mathbf b},

    keeps the kernel non-negative and changes the misfit to

    .. math::

       \sum_i\frac{w_i^2(\mathbf K\mathbf u-\tilde{\mathbf b})_i^2}{2w_i\tilde b_i}
       = \sum_i\frac{(\mathbf K\mathbf u-\tilde{\mathbf b})_i^2}{2\sigma_i^2}
       = \tfrac12\chi^2(\mathbf p),

    because :math:`\mathbf K\mathbf u-\tilde{\mathbf b}=\mathbf A\mathbf p-\mathbf b`.
    The fixed point then fits the data within their errors.

Acceleration safeguard
    With the first-half rebuild the iteration is no longer a monotone EM
    map, and Biggs–Andrews extrapolation can drift (13 spline
    coefficients: :math:`\chi^2_r` 1.68 to 260). If the accelerated run ends
    unconverged with :math:`\chi^2_r>1.05\times` the best iterate, plain
    Picard iteration continues from the best iterate
    (``acceleration_fallback`` in the diagnostics).

Regularization options
    *smoothing* -- :math:`\mathbf p\leftarrow\mathbf S^{(h)}\mathbf p` after
    each step, :math:`h` by the discrepancy principle (floor
    :math:`10^{-7}`).

    *MaxEnt* -- the entropy of a signed :math:`p` is defined through the
    two-channel representation :math:`p = h_+ - h_-`, :math:`h_\pm\ge0`,
    with default model :math:`m` for both channels (Skilling & Gull);
    eliminating the channels gives

    .. math::

       S_\pm(\mathbf p) = \sum_j\left[\psi_j - 2m - p_j\,\mathrm{asinh}\frac{p_j}{2m}\right],
       \qquad \psi_j=\sqrt{p_j^2+4m^2},
       \qquad \frac{\partial S_\pm}{\partial p_j} = -\mathrm{asinh}\frac{p_j}{2m}.

    In analogy to SASfit's :math:`\Delta s = \lambda\,x\,\partial S/\partial x`
    the metric :math:`\psi_j` is used, and each EM step is followed by

    .. math::

       p_j \leftarrow p_j + \lambda\,\psi_j\left[\tau_j - \mathrm{asinh}\frac{p_j}{2m}\right],
       \qquad
       \tau_j = \begin{cases} 0 & \text{constant prior}\\
       \mathrm{asinh}\dfrac{(\boldsymbol\Pi\mathbf p)_j}{2m} & \text{adaptive prior}\end{cases}

    For :math:`|p|\ll m` this is a ridge pull :math:`-\lambda p` (towards 0
    or towards :math:`\boldsymbol\Pi\mathbf p`), for :math:`|p|\gg m` the
    usual MaxEnt behaviour :math:`-\lambda|p|\ln(|p|/m)`. Default
    :math:`m=0.1\max|p^{ARLS}|`; :math:`\lambda` by the bidirectional
    discrepancy principle (start :math:`10^{-5}`, range
    :math:`10^{-12}`–1). A first attempt applying SASfit's correction to
    the shifted :math:`\mathbf u` with prior :math:`t` failed: the entropy
    then degenerates to a ridge with scale :math:`t`, and the adaptive
    variant diverged.

Results (test.dat, :math:`j_0`, correlation with the Bayesian-evidence :math:`p(r)`):

.. list-table::
   :header-rows: 1

   * - setup
     - regularization
     - :math:`\chi^2_r`
     - correlation
   * - 95-point grid
     - smoothing
     - 1.01
     - 0.994
   * - 95-point grid
     - MaxEnt, adaptive
     - 1.00
     - 0.994
   * - 95-point grid
     - MaxEnt, constant
     - not converged
     - :math:`\approx` 0.93
   * - 20 log B-splines
     - smoothing
     - 1.00
     - 0.9998
   * - 20 log B-splines
     - MaxEnt, adaptive
     - 1.00
     - 0.9998
   * - 20 log B-splines
     - MaxEnt, constant
     - 0.99
     - 0.9996
   * - 13 log B-splines (n = 15)
     - smoothing
     - 1.87
     - 0.995

The constant prior pulls towards zero, which does not damp the
oscillating components that the data do not determine (slow null-space
modes), and converges slowly on point grids. With the sphere kernel the
signed EM fits (:math:`\chi^2_r=1`) but gives a clearly negative
:math:`N(R)` (minimum :math:`\approx-0.6\times` maximum); for size
distributions the positive EM solvers are the right choice.
