Development history and findings
==================================

This chapter keeps the reasoning behind the present methods -- including
what was tried and discarded -- for anyone extending the package. All
numbers refer to ``tests/data/test.dat`` (bilognormal spheres with modes
near 60 and 180 nm, :math:`q` in nm\ :sup:`-1`, 91 log-spaced points,
background :math:`\approx0.1`) unless stated otherwise.

The null space of the second-difference penalty
-----------------------------------------------

With the :math:`j_0` kernel and the plain second-difference penalty, GCV,
the L-curve and the evidence all gave :math:`\chi^2_r` in the hundreds
(EM + smoothing 586, Tikhonov + GCV 34 600). Constants and linear ramps
are not penalized, and with a kernel condition number of
:math:`\sim10^{16}` these free directions are enough to make every
criterion choose an oscillating solution. Hansen's (2000) boundary terms
remove the null space (condition number of :math:`\mathbf L^T\mathbf L`
:math:`\sim9\times10^3`) and gave a clean evidence maximum.

Regional overfitting at high :math:`q`
--------------------------------------

A good overall :math:`\chi^2_r` hid an unbalanced fit: with the Hansen
penalty on the 95-point grid the high-:math:`q` third of the data had
:math:`\chi^2\approx0.02` (the fit followed the noise), the low-:math:`q`
third about 0.8. Scans that each held everything else fixed ruled out the
obvious causes:

* :math:`\lambda`: no value gave all three regions near 1 at once.
* :math:`D_{max}`: 500 nm was too small (low-:math:`q` :math:`\chi^2`
  3.35 to 0.8 up to :math:`D_{max}\approx800`), which led to the
  suggestion :math:`D_{max}=\pi/q_{min}`; the high-:math:`q` value stayed
  at 0.02 for every :math:`D_{max}`.
* number of grid points above :math:`\approx75` and :math:`r_{min}`:
  no effect.

Two attempted fixes, now retired:

* **Two-segment** :math:`\lambda` (independent :math:`\lambda` for low
  and high :math:`r`, chosen by a two-hyperparameter evidence search):
  within 0.3–0.6 % of the single-:math:`\lambda` result, never improved
  the high-:math:`q` fit. The :math:`j_0` kernel couples every data
  point to the whole :math:`r` range, so the noise-following component is
  not localized in :math:`r`.
* **Hann-tapered boundary** (:math:`p=w(r)\,p_{free}` with a raised-cosine
  roll-off over the last quarter of :math:`[0,D_{max}]`): moved the
  high-:math:`q` :math:`\chi^2` from 0.023 to 0.21, but needed a ridge
  stabilizer, and was superseded by the B-spline representation.

The explanation is Shannon undersampling of log-spaced data
(:doc:`theory_forward_model`): at high :math:`q` there are only 0.45 data
points per Shannon channel, so a model with one free parameter per channel
can follow every point. Reducing the number of free parameters with a
B-spline basis (:doc:`theory_discretization`) brings the high-:math:`q`
:math:`\chi^2` to about 1 for every solver and increases the normalized
evidence (:math:`\ln Z` from :math:`-153.0` to :math:`-118.8`). The retired solvers remain
callable by their old keys (``RETIRED_SOLVERS``) but are no longer
offered.

Bootstrap with forced :math:`\chi^2_r=1`
----------------------------------------

The bootstrap exploded (std of :math:`p` up to 400) when combined with
forced :math:`\chi^2_r=1`, because the replicates were centred on the
noisy data. Fixed by centring on the fit and by the :math:`\lambda` floor
(:doc:`theory_uncertainty`).

Hansen MaxEnt
-------------

The CG transcription of ``maxent.m`` jammed at the positivity bound and
the registry wrapper ignored :math:`\sigma_i`; replaced by weighted
L-BFGS-B with a Newton polish and the discrepancy principle
(:doc:`theory_em_maxent`).

Signed EM
---------

Three problems were found and fixed on 2026-10-10: the shifted KL misfit
was blind to the high-:math:`q` data (fixed by :math:`\chi^2` row
weighting), Biggs–Andrews extrapolation could drift away from the fixed
point (safeguard), and too coarse quadrature with few grid points
(:eq:`nquad`). MaxEnt regularization with a positive/negative entropy was
added as an alternative to smoothing.

Few grid points
---------------

A uniform point grid with step :math:`>\pi/q_{max}` undersamples the
:math:`j_0` kernel (:math:`\chi^2_r` 460–840 for :math:`N\le45`); this
affects every solver. The fine quadrature of the node and spline
representations removes the limitation; with cubic interpolation 30
uniform points suffice (:math:`\chi^2_r` 1.18).

Reduced solver list
-------------------

Solvers that differed in a single setting were merged and given options
(e.g. EM + smoothing with discrepancy principle or L-curve, Bayesian
evidence with three penalties); the B-spline solvers were replaced by the
generic B-spline option. Old keys are mapped by ``SOLVER_ALIASES``.

Non-negativity is not a default
-------------------------------

:math:`p(r)\ge0` holds only for homogeneous contrast. For core–shell,
multilayer or contrast-variation systems :math:`p(r)` has negative parts
that carry structural information. Non-negativity is therefore only used
where it is part of the method (EM, MaxEnt, ARLS :math:`\ge0`) and
never imposed on the signed solvers.

Open questions
--------------

* Automatic choice of the number of B-spline coefficients and of the knot
  spacing (e.g. by the normalized evidence).
* Penalties scaled by the node spacing (smoothness in :math:`r` rather
  than in the index) for non-uniform point grids.
* The default level :math:`m` of the signed MaxEnt is a heuristic.
* Moore's sine-series basis for the PDDF.
* Whether the quoted :math:`\sigma_i` of test.dat are well calibrated at
  high :math:`q`.
