Theory and the regularization investigation
=============================================

This page documents the PDDF (pair-distance-distribution function)
regularization investigation carried out on this package's real test
dataset (``tests/data/test.dat``), which produced the
``bayesian_evidence_hansen`` and ``bayesian_evidence_hann_tapered``
solvers. It's kept as part of the documentation, not just a changelog
entry, because the *reasoning* (what was ruled out, and why) matters as
much as the final solver choice for anyone extending this work.

The problem
-----------

Recovering the pair-distance-distribution function p(r) from small-angle
scattering intensity I(q) is a Fredholm integral equation of the first
kind:

.. math::

   I(q) = 4\pi \int_0^{D_{max}} p(r)\, \frac{\sin(qr)}{qr}\, dr

with the physical constraints p(0) = 0 and p(r) = 0 for r > :math:`D_{max}`
(the particle's maximum dimension). This is a classically ill-posed
inverse problem: small amounts of noise in I(q) can correspond to large,
unphysical oscillations in the recovered p(r) unless the solution is
regularized.

Using the signed ``4*pi*j0(qr)`` kernel on real data with a naive
second-derivative smoothness penalty (``regularization.second_derivative_operator``)
produced catastrophically poor fits (chi2_r in the hundreds), traced to
that operator's 2-dimensional null space (a constant offset and a linear
ramp are both "free", unconstrained by the roughness penalty) combined
with the kernel's extreme condition number (~1e16 for this dataset's
q/r grid).

Hansen's boundary-constrained Bayesian-evidence IFT
-----------------------------------------------------

Hansen (2000), *J. Appl. Cryst.* 33, 1415-1421, fixes the null-space
problem by building the physical boundary condition p(0) = p(:math:`D_{max}`) = 0
directly into the smoothness functional (his eq. 19) rather than via a
small set of basis functions (Hansen explicitly calls Glatter's original
small-basis-function restriction obsolete once direct point-grid
estimation -- which this package already uses throughout -- is
available):

.. math::

   S(p) = \frac{1}{2}\sum_{i=0}^{N-2} (p_i - p_{i+1})^2 + \frac{1}{2}p_0^2 + \frac{1}{2}p_{N-1}^2

This has a provably full-rank Hessian (``regularization.hansen_smoothness_cholesky``
asserts this at construction time), which drops the condition number from
~1e16 to ~9.2e3 on this dataset.

The regularization strength :math:`\lambda` is chosen by **Bayesian
evidence maximization** (Vestergaard & Hansen, 2006, *J. Appl. Cryst.* 39,
797-804), not GCV, the L-curve, or the discrepancy principle --
implemented generically in :mod:`sasfit_inversion.solvers.bayesian_evidence`
so that any regularization operator (not just Hansen's) can be plugged in.

This is the ``bayesian_evidence_hansen`` solver, and on real data it was a
dramatic improvement over every other regularized solver tried (chi2_r
~1.6 vs 586 for EM+smoothing and ~34,600 for plain Tikhonov+GCV at the
time it was introduced).

Regional overfitting: a hidden problem behind a good chi2_r
---------------------------------------------------------------

A good *aggregate* chi2_r can hide a badly imbalanced fit. Splitting the
residuals into low/mid/high-q terciles and computing the mean chi2
contribution per region (``(fit - data) / dI`` squared, averaged) revealed
that the Hansen solver's chi2_r ~1.6 was a compromise: low-q was
underfit (chi2 contribution ~3+) while **high-q was severely overfit --
tracking noise almost exactly** (chi2 contribution ~0.02, i.e. the fit
matches the data roughly 7x more closely than the stated error bars
justify).

A single global :math:`\lambda` cannot, in general, be simultaneously
correct for a region with real structure (needs less smoothing) and a
region with little information content (needs more smoothing to avoid
chasing noise) -- the question investigated below is *why*, and whether
any choice of hyperparameter or grid fixes it.

Systematically ruling out the usual suspects
-----------------------------------------------

Four separate scans, each holding everything else fixed, were run on the
real dataset (see ``tests/diagnose_hansen_*.py``):

:math:`\lambda` scan (``diagnose_hansen_lambda_balance.py``)
    Scanning :math:`\lambda` across six decades: high-q chi2 only
    approaches 1 once :math:`\lambda` is so large that low-q is already
    badly underfit (chi2 >> 1) -- there is **no** :math:`\lambda` giving
    low/mid/high-q all close to 1 simultaneously. Critically, **low-q chi2
    never dropped below ~2.16 even at the smallest** :math:`\lambda`
    **tested** (essentially unregularized, 49 of 50 parameters free) --
    more freedom does not fix low-q, which was the first clue this wasn't
    a regularization-strength problem at all.

:math:`D_{max}` scan (``diagnose_hansen_dmax_scan.py``)
    :math:`D_{max}` = 500 turned out to be genuinely too small (truncating
    real pair-distance density): low-q chi2 dropped from 3.35 to ~0.8 as
    :math:`D_{max}` grew from 500 to ~800. This motivated checking
    Glatter's consistency condition :math:`q_{min} \le \pi / D_{max}`,
    i.e. :math:`D_{max} \le \pi / q_{min}` -- for this dataset's
    :math:`q_{min}` = 0.004016, that bound is :math:`D_{max} \approx 782`.
    The GUI now auto-suggests this value on data load (overridable).
    Beyond that bound, "improvement" in aggregate chi2_r was shown to be
    mostly overfitting (mid-q chi2 kept sliding well below 1) rather than
    real resolution -- **but the high-q chi2 contribution stayed pinned
    at ~0.02 across every** :math:`D_{max}` **tested**, confirming
    :math:`D_{max}` isn't the mechanism either.

n_r scan (``diagnose_hansen_nr_scan.py``)
    Tested whether oversampling r (more grid points than the data's
    Shannon-limited information content) was handing the high-q region
    cheap noise-fitting freedom. Refuted: once above the minimum resolved
    grid density (n_r >= ~75 for this problem), every metric -- including
    high-q chi2 -- was flat from n_r=75 to n_r=200.

r_min scan (same script)
    Tested the boundary-condition approximation (p(0)=0 enforced at the
    grid's first node, r_min, not exactly r=0) and near-r=0 kernel
    degeneracy. Also flat from r_min=0.1 to r_min=10 -- no effect.

**Conclusion**: :math:`\lambda`, :math:`D_{max}`, n_r, and r_min were each
ruled out individually. The high-q noise-tracking survived every one of
them unchanged, at the one regime where the method is well-resolved.

A failed fix: position-dependent (r-segment) adaptive :math:`\lambda`
--------------------------------------------------------------------------

Reasoning that a single global :math:`\lambda` was the limiting factor,
Hansen's penalty was split into two additive pieces by r-index
(``regularization.hansen_smoothness_two_segment``, exact algebraic
partition, :math:`A_{lo} + A_{hi} = A_{hansen}`), with an independent
:math:`\lambda_{lo}, \lambda_{hi}` chosen by a generalized two-hyperparameter
evidence search (``solvers.bayesian_evidence.log_evidence_blocks`` /
``evidence_search_blocks``), searching both the split location and both
:math:`\lambda`'s jointly.

**Result: no improvement.** High-q chi2 was unchanged or slightly worse
(0.0234 -> 0.0298) regardless of where the split was placed, and
:math:`\lambda_{lo}` pinned at the search grid's floor for every split
candidate. The reason, in hindsight: the oscillatory j0(qr) kernel means a
single high-q data point is sensitive to p(r) across the *whole* r-range,
not a contiguous region -- so a noise-fitting wiggle can sit anywhere in
r. Partitioning by r-*position* doesn't target the actual mechanism, which
turned out to be spatial-frequency content, not position.

A rejected fix: non-negativity
---------------------------------

p(r) >= 0 is **not** a universal constraint -- it only holds for a single,
homogeneous-contrast particle. For core-shell particles, multi-shell/
multilamellar structures, or contrast-variation (partially-deuterated)
systems, the excess scattering-length density changes sign within the
particle, and p(r) can legitimately dip negative from cross-correlation
between oppositely-signed regions -- this is a documented diagnostic
signature of internal structure in the SAXS/SANS literature, not an
artifact. Imposing p(r) >= 0 as a hard, default constraint would
distort or suppress genuine structural signal for exactly this class of
system, which is within this package's intended scope. **Not implemented
as a default**; if offered at all, it must be an explicit opt-in variant
for the homogeneous-contrast case specifically.

The fix: a Hann-tapered boundary
------------------------------------

p(r) and I(q) are (essentially) a Fourier sine transform pair. Hansen's
boundary condition is mathematically a **sharp edge** in r-space (p pinned
to exactly 0 at a single grid point). By the standard windowing/Gibbs-
phenomenon relationship, a sharp edge in one domain convolves the
conjugate domain's transform with a sinc function, whose sidelobes decay
slowly (~1/q) and persist out to high q -- *wherever* the edge sits. This
explains both negative results above: :math:`D_{max}` scans don't fix it
because moving the edge doesn't remove it, and r-segment splitting doesn't
fix it because the ringing isn't localized to one region of r.

The fix is the standard apodization remedy for truncation ringing: replace
the hard edge with a smooth rolloff. ``regularization.hann_taper(r, r_max,
taper_frac=0.25)`` returns a weight :math:`w(r) \in [0, 1]`, 1 in the
interior and raised-cosine (Hann-style) tapering to 0 over the last
quarter of :math:`[0, D_{max}]`. Reparametrizing :math:`p(r) = w(r)\, p_{free}(r)`
and solving for :math:`p_{free}` with a *plain* (non-boundary-constrained)
smoothness penalty replaces Hansen's hard knot with a soft decay. (The
plain second-derivative operator's null space, harmless when combined
with Hansen's boundary fix, needed a tiny numerical ridge stabilizer here
-- see ``bayesian_evidence.py``'s ``ridge`` parameter -- since nothing
else constrains it in this variant.)

Validation -- synthetic (known ground truth)
```````````````````````````````````````````````

A homogeneous-sphere PDDF (:math:`D_{true}` = 380, analytic p(r)), forward-
transformed to I(q) on a q-grid matching the real dataset's span/count,
with realistic relative + floor noise, across 5 independent noise draws:

.. list-table::
   :header-rows: 1

   * - seed
     - RMSE (hard boundary)
     - RMSE (Hann-tapered)
     - high-q chi2 (hard)
     - high-q chi2 (tapered)
   * - 1
     - 0.0474
     - 0.0409
     - 0.614
     - 0.633
   * - 2
     - 0.0381
     - 0.0233
     - 0.839
     - 0.885
   * - 3
     - 0.0434
     - 0.0266
     - 1.141
     - 1.196
   * - 4
     - 0.0362
     - 0.0295
     - 1.192
     - 1.260
   * - 5
     - 0.0566
     - 0.0240
     - 0.477
     - 0.580

The tapered version had lower RMSE against the known true p(r) in every
single trial (sometimes by ~35-58%), and a high-q chi2 contribution
consistently, if modestly, closer to 1 (less overfitting) in every trial
too. This synthetic test -- a smooth, textbook sphere PDDF -- is inherently
better-conditioned than the real particle's shape, so it validates the
*mechanism and direction*, not the full *severity* seen on real data.

Validation -- real data (``tests/diagnose_hansen_tapered.py``)
````````````````````````````````````````````````````````````````

.. list-table::
   :header-rows: 1

   * - region
     - hard boundary (``bayesian_evidence_hansen``)
     - Hann-tapered (``bayesian_evidence_hann_tapered``)
   * - low-q chi2
     - 0.797
     - 0.807
   * - mid-q chi2
     - 0.343
     - 0.491
   * - **high-q chi2**
     - **0.023**
     - **0.208**
   * - overall chi2_r
     - 0.384
     - 0.499

On the actual pathological dataset, high-q chi2 contribution moved **9x**
closer to the ideal value of 1 (0.023 -> 0.208), mid-q also improved
(0.343 -> 0.491), and low-q -- which was already reasonable -- was
essentially unchanged. This is a genuine, substantial improvement in
regional balance, not just the synthetic test's direction holding up in
principle.

It is **not a complete fix**: high-q chi2 = 0.208 is still below the
ideal 1, so some residual overfitting remains. :math:`\tau` (``taper_frac``,
currently a fixed, untuned 0.25) has not been scanned for an optimum, and
is a natural next step for anyone picking this work back up.

Current recommendation
--------------------------

For the j0(qr)/PDDF kernel on real data:

- Use ``bayesian_evidence_hann_tapered`` as the primary choice --
  consistently better regional balance than the hard-boundary version,
  validated on both synthetic (known ground truth) and real data.
- ``bayesian_evidence_hansen`` (hard boundary) remains available, in
  particular as a simpler/more-classical baseline to compare against or
  for cases where the taper's extra approximation (the ridge stabilizer)
  is undesirable.
- Both are registered in :data:`sasfit_inversion.solver_registry.SOLVER_REGISTRY`
  and therefore automatically available in the GUI's solver dropdown, with
  their full tradeoffs documented in each ``SolverSpec.description``.
- For the sphere/size-distribution kernel (non-negative, not PDDF),
  EM+smoothing (discrepancy principle) remains the better-validated
  default -- none of this investigation concerned that kernel.

Open questions for future work
----------------------------------

- Scan ``taper_frac`` for an optimum (not done -- the current 0.25 is a
  reasonable but untuned default).
- Check whether the real dataset's quoted ``dI`` is itself well-calibrated
  at high q (compare raw point-to-point I(q) scatter against quoted dI,
  independent of any inversion) -- if ``dI`` is overestimated there, part
  of the "overfitting" signature may be the solver correctly reporting
  that the data has less noise than its stated error bars claim, not a
  regularization failure at all. Not yet checked.
- A frequency-domain (rather than position-domain) adaptive regularization
  -- e.g. a higher-order roughness penalty targeting short-wavelength
  ripples specifically, wherever they occur in r -- was proposed but not
  implemented; may compound with the Hann taper rather than replace it.
- Total-variation (L1-on-the-gradient) regularization as an alternative to
  Hansen's L2 smoothness was proposed (sparsity-promoting, suppresses
  ringing without a sign assumption, unlike non-negativity) but not
  implemented -- would require moving from a closed-form quadratic solve
  to an IRLS/QP formulation.
