Discretization of the integral
================================

General scheme
--------------

Every discretization represents the unknown by a finite expansion

.. math::
   :label: expansion

   x(r) = \sum_{k=1}^{N} c_k\, \varphi_k(r)

and turns :eq:`fredholm` into the linear system
:math:`\mathbf{A}\mathbf{c}\approx\mathbf{b}` with

.. math::
   :label: amatrix

   A_{ik} = \int_0^{r_{max}} K(q_i,r)\,\varphi_k(r)\,\mathrm{d}r
   \qquad\text{(collocation in } q\text{)} .

The data are matched exactly at the measured :math:`q_i` (collocation);
Galerkin projections are not used because the data exist only at discrete
:math:`q`. Three choices of :math:`\varphi_k` are implemented: point values
with a simple quadrature, point values with interpolation and a fine
quadrature, and cubic B-splines. The table summarizes when each is used.

.. list-table::
   :header-rows: 1
   :widths: 26 26 48

   * - representation
     - unknowns :math:`c_k`
     - used when
   * - direct collocation
     - :math:`x(r_k)`
     - uniform grid with step :math:`\le 1.1\,\pi/q_{max}`, linear
       interpolation, B-splines off
   * - nodes + interpolation, fine quadrature
     - :math:`x(r_k)`
     - non-uniform grid, cubic interpolation, or uniform step
       :math:`>1.1\,\pi/q_{max}` (automatic)
   * - cubic B-splines
     - spline coefficients
     - "Represent p(r) by B-splines" checked

Direct collocation on a point grid
----------------------------------

:func:`sasfit_inversion.kernels.build_size_distribution_kernel` uses the
grid points themselves as quadrature nodes,

.. math::
   :label: collocation

   A_{ij} = \Delta r_j\; r_j^{-\alpha}\, K(q_i, r_j),\qquad
   \Delta r_j = \tfrac12 (r_{j+1}-r_{j-1}),

(``numpy.gradient``; one-sided at the ends). This is the rectangle/
trapezoid rule and is accurate only if the kernel varies little between
neighbouring points. For the :math:`j_0` kernel the fastest oscillation
in :math:`r` has period :math:`2\pi/q_{max}`, so a grid step
:math:`\Delta r>\pi/q_{max}` undersamples the kernel at high :math:`q` and
*no* coefficient vector can fit those data. On test.dat
(:math:`\pi/q_{max}=8.2` nm, :math:`D_{max}=782` nm) even unregularized
least squares gives :math:`\chi^2_r = 460`–:math:`840` for
:math:`N\le45` points.

Point values with interpolation and fine quadrature
---------------------------------------------------

To decouple the number of unknowns from the quadrature accuracy,
:func:`~sasfit_inversion.kernels.build_kernel_on_nodes` keeps the node
values :math:`c_k=x(r_k)` as unknowns, interpolates between the nodes
with fixed cardinal functions :math:`\varphi_k`
(:math:`\varphi_k(r_j)=\delta_{jk}`), and evaluates :eq:`amatrix` on a
fine uniform grid :math:`\rho_l`:

.. math::
   :label: nodekernel

   \mathbf{A} = \mathbf{A}^{fine}\,\boldsymbol{\Phi},\qquad
   A^{fine}_{il} = \Delta\rho\, \rho_l^{-\alpha} K(q_i,\rho_l),\qquad
   \Phi_{lk} = \varphi_k(\rho_l).

The size of the fine grid (:func:`~sasfit_inversion.kernels.quadrature_points`)
is

.. math::
   :label: nquad

   n_{q} = \max\!\left(4(N-1)+1,\;
   \left\lceil \frac{4\, r_{max}\, q_{max}}{\pi}\right\rceil + 1\right),

i.e. at least four points per Shannon interval :math:`\pi/q_{max}` and at
least four per node interval. The node kernel reproduces a reference
integral of a sphere PDDF to :math:`2.7\times10^{-4}` (relative) on a
60-point quadratic grid.

Linear interpolation (hat functions)
    .. math::

       \varphi_k(r) = \begin{cases}
       \dfrac{r-r_{k-1}}{r_k-r_{k-1}} & r_{k-1}\le r\le r_k\\[1ex]
       \dfrac{r_{k+1}-r}{r_{k+1}-r_k} & r_k\le r\le r_{k+1}\\[1ex]
       0 & \text{otherwise}\end{cases}

    :math:`\varphi_k\ge 0` and :math:`\sum_k\varphi_k=1`: non-negative node
    values give a non-negative :math:`x(r)` and a non-negative kernel stays
    non-negative. Hence linear interpolation is compatible with every
    solver.

Cubic spline interpolation (not-a-knot)
    :math:`\varphi_k` is the :math:`C^2` cubic interpolating spline through
    the unit vector :math:`\mathbf{e}_k` (:func:`~sasfit_inversion.kernels.cubic_basis`).
    On each interval :math:`x(r)` is a cubic; :math:`x`, :math:`x'` and
    :math:`x''` are continuous at the inner nodes. The two missing
    conditions are the *not-a-knot* conditions -- :math:`x'''` is also
    continuous at :math:`r_2` and :math:`r_{N-1}`, i.e. the first two and
    the last two intervals are each one cubic. The alternative *natural*
    condition :math:`x''(0)=x''(r_{max})=0` is wrong for a PDDF, which
    behaves like :math:`r^2` near the origin (test.dat, uniform 45 points:
    :math:`\chi^2_r` 2.42 natural vs 0.65 not-a-knot). The interpolation
    is exact for cubic polynomials and :math:`\sum_k\varphi_k = 1`.

    The cardinal cubic splines oscillate: they have negative lobes
    (minimum about :math:`-3.2` on a 60-point log grid, where adjacent
    intervals differ strongly in length). Consequently non-negative node
    values do **not** guarantee :math:`x(r)\ge0`, and a non-negative kernel
    becomes slightly signed (3 % negative entries for the sphere kernel).
    The GUI therefore uses linear interpolation for the solvers that need
    :math:`x\ge0` or a non-negative kernel (EM + smoothing, EM + MaxEnt,
    Hansen MaxEnt, ARLS :math:`\ge0`).

Effect of the representation (test.dat, :math:`j_0` kernel, Bayesian evidence, :math:`\chi^2_r`):

.. list-table::
   :header-rows: 1

   * - r grid
     - direct collocation
     - linear, fine quadrature
     - cubic, fine quadrature
   * - uniform, 95 points
     - 0.43
     - 0.47
     - 0.44
   * - uniform, 45 points
     - --
     - 13.9
     - 0.65
   * - uniform, 30 points
     - 580
     - 282
     - 1.18
   * - quadratic, 60 points
     - 20.4
     - 0.53
     - --
   * - quadratic, 30 points
     - 516
     - 1.41
     - 0.79
   * - log, 60 points
     - 96.2
     - 0.65
     - 0.60
   * - log, 30 points
     - --
     - 2.29
     - 0.81

Spacing of the grid points
--------------------------

:func:`~sasfit_inversion.kernels.make_r_grid` places :math:`N` points on
:math:`[0,r_{max}]` with :math:`u_k = (k-1)/(N-1)`:

.. math::

   \text{uniform: } r_k = r_{max}\,u_k ,\qquad
   \text{quadratic: } r_k = r_{max}\,u_k^2 ,

.. math::

   \text{log: } r_1 = 0,\quad
   r_k = r_{max}\left(\frac{1}{2N}\right)^{\frac{N-k}{N-2}},\ k=2\dots N ,

(a geometric series from :math:`r_{max}/(2N)` to :math:`r_{max}`). Dense
spacing at small :math:`r` suits a PDDF that rises steeply and has a long
smooth tail; size distributions with structure at large :math:`r` prefer
the uniform grid.

All smoothness penalties of the next chapter are built from differences of
*neighbouring unknowns*. On a non-uniform grid they therefore measure
smoothness in the grid index -- on a log grid, smoothness in
:math:`\ln r`, which gives the solution more freedom at small :math:`r`.

Cubic B-spline basis
--------------------

Following Glatter (1977) the unknown can be represented by
:math:`N_c\ll N_s` cubic B-splines
(:mod:`sasfit_inversion.spline_basis`):

.. math::
   :label: bspline

   x(r) = \sum_{k=1}^{N_c} c_k\, B_{k}(r/r_{max}) .

Knot vector
    Clamped, on :math:`t=r/r_{max}\in[0,1]`, degree :math:`d=3`:

    .. math::

       \boldsymbol\tau = (\underbrace{0,0,0,0}_{d+1},\ \tau_1,\dots,\tau_{K},\
       \underbrace{1,1,1,1}_{d+1}),\qquad K = N_c + 2 - d - 1 .

    The interior knots follow the same three rules as the grid points
    (:func:`~sasfit_inversion.spline_basis.interior_knots`): uniform
    :math:`\tau_j = j/(K+1)`, quadratic :math:`\tau_j=(j/(K+1))^2`, log
    geometric from :math:`1/(2N_c)` to 1.

Cox–de Boor recursion
    .. math::

       B_{j,0}(t) = \begin{cases}1 & \tau_j\le t<\tau_{j+1}\\ 0 & \text{else}\end{cases},
       \qquad
       B_{j,d}(t) = \frac{t-\tau_j}{\tau_{j+d}-\tau_j}B_{j,d-1}(t)
                  + \frac{\tau_{j+d+1}-t}{\tau_{j+d+1}-\tau_{j+1}}B_{j+1,d-1}(t).

    The clamped basis has :math:`N_c+2` functions; the first and the last
    (the only ones that are non-zero at :math:`t=0` and :math:`t=1`) are
    dropped, which builds :math:`x(0)=x(r_{max})=0` into the basis instead
    of imposing it by a penalty.

Properties
    :math:`B_k\ge0`, local support on four knot intervals, :math:`C^2`
    continuity. By the convex-hull property, :math:`\mathbf c\ge0`
    implies :math:`x(r)\ge0`; a non-negative kernel stays non-negative.
    Every solver -- including the EM and MaxEnt solvers -- can therefore be
    used unchanged on the coefficients.

Kernel and results
    With :math:`\boldsymbol\Phi_{lk}=B_k(\rho_l/r_{max})` on the fine grid
    :eq:`nquad` and :math:`\mathbf{B}_{jk} = B_k(r_j/r_{max})` on the display
    grid,

    .. math::

       \mathbf{A}_{spline} = \mathbf{A}^{fine}\boldsymbol\Phi
       \in\mathbb{R}^{M\times N_c},\qquad
       \mathbf{x} = \mathbf{B}\,\mathbf{c},\qquad
       \mathrm{Cov}(\mathbf{x}) = \mathbf{B}\,\mathrm{Cov}(\mathbf{c})\,\mathbf{B}^T .

    :func:`sasfit_inversion.solver_registry.run_solver` with
    ``spline=SplineOptions(n_coeffs, spacing, A_quad)`` runs any registered
    solver on :math:`\mathbf{A}_{spline}` and maps the result back. The
    solvers' smoothness operators then act on neighbouring coefficients.

Why it helps
    A cubic B-spline basis with knot spacing :math:`\Delta\tau\,r_{max}`
    cannot represent oscillations shorter than about
    :math:`2\Delta\tau\,r_{max}`. With :math:`N_c=20` the basis cannot
    follow individual high-:math:`q` points of log-spaced data, while a
    95-point grid can. On test.dat (:math:`j_0`, 20 coefficients, log
    knots) all Bayesian-evidence and Tikhonov variants reach
    :math:`\chi^2_r\approx1` with a high-:math:`q` :math:`\chi^2` of
    about 1.05, against 0.02–0.1 on the 95-point grid, and the Bayesian
    normalized evidence increases from :math:`\ln Z=-153.0` to
    :math:`-118.8`. With the sphere kernel, the spurious spike at
    :math:`r\approx3` nm of the volume-weighted point-grid solution
    disappears.

    The number of coefficients is the regularization knob of this
    representation (typically 15–30); knot spacing matters as well
    (uniform knots needed about 25 coefficients on test.dat, log/quadratic
    knots 20).

Alternatives in the literature
------------------------------

* **Sine series** (Moore 1980): :math:`p(r)=\sum_{n=1}^{N} a_n
  \sin(n\pi r/D_{max})`. The basis functions are the Shannon-optimal
  functions for a PDDF, vanish at both ends, and the integrals
  :eq:`amatrix` with :math:`j_0` have closed forms. It is linear and signed
  (no positivity). Not implemented.
* **Shape-preserving cubic interpolation** (PCHIP, Fritsch & Carlson
  1980) does not overshoot, but its slopes depend non-linearly on the node
  values, so it cannot be written as a fixed matrix
  :math:`\boldsymbol\Phi`; not usable in the linear framework.
* **Higher-order quadrature** (Simpson, Gauss) of the collocation
  integrals improves the accuracy of :eq:`collocation` but not the
  undersampling problem, which is solved by the fine quadrature above.
