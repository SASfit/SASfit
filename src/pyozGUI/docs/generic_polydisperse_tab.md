# The generic polydisperse tab

`Polydisperse (any potential)` — any potential x any closure x any form
factor, with the exact I(Q) shown against all six of SASfit's approximate
schemes.

The tabs it replaced are special cases of it:

| tab | potential | closure |
|---|---|---|
| Polydisperse Yukawa *(retired)* | one-Yukawa, analytic | MSA / RMSA only |
| Robertus SHS *(retired)* | adhesive spheres, analytic | PY only |
| RY Polydisperse Yukawa | charge-coupled Yukawa, numerical | RY only |
| **Polydisperse (any potential)** | **21 potentials** | **24 closures** |

The first two are no longer registered in `oZgui.EXTRA_TABS`, though their
modules remain importable and restoring either is one line. RY Polydisperse
Yukawa is kept because it takes per-distribution parameters, which this tab
does not: everything here is pinned to (mean, srel), which is what makes the
five distributions mutually comparable but rules out an asymmetric Gamma or a
general Beta.

The counts above come from `ozLib.CLOSURE_SETTERS` and the solver's own
`setXXXPotential` methods. They are a snapshot: inspect those rather than
trust this table if it matters.

## Files

```
polydisperse_nodes.py          size classes: which quadrature rule, and why
generic_polydisperse_sas.py    solves + exposes I_exact and the six schemes
generic_polydisperse_tab.py    the GUI
oZfixpointOperator.py          setPolydispersePotential() (the pair potential)
```

Registered in `oZgui.EXTRA_TABS` as
`("generic_polydisperse_tab", "GenericPolydisperseTab", "Polydisperse (any potential)")`.

---

## 1. The pair potential: 18 setters reused, none rewritten
`setPolydispersePotential(potentialName, potentialArgs, srel, nbins,
meanDiameter, distribution)` turns any one-component `setXXXPotential()` into
a multicomponent `(p,p,N)` pair potential.

The trick is a single hook. `getrArray()` honours an `_rArrayOverride`; the
builder points it at `r/sigma_ij` and calls the ordinary setter. Because every
setter measures its hard core against `hardSphereDiameter = 1`, that core
lands exactly at `sigma_ij` while the tail is evaluated at the reduced
separation. No setter was modified.

**Mixing rule** (a modelling choice, stated not buried):

```
additive cores      sigma_ij = (sigma_i + sigma_j)/2
identical reduced   u_ij(r)  = u(r/sigma_ij)
tail
```

Every pair sees the same interaction shape in units of its own contact
distance — the assumption Robertus et al. make for size-independent
stickiness. It is what makes the construction potential-agnostic: epsilon,
delta, n, tau keep their reduced-unit meaning for every pair, so no
per-potential mixing rule has to be invented.

It is not the only defensible choice. For Lennard-Jones the conventional
alternative is Lorentz-Berthelot, `epsilon_ij = sqrt(epsilon_i epsilon_j)`
with per-species epsilon; that is a different model and needs per-species tail
parameters, which the builder deliberately does not make up.

**Charge-coupled potentials are refused, not mis-modelled.** DLVO, DLVOHydra,
IonicMicrogel and the dedicated polydisperse Yukawa have amplitudes that scale
with particle size and a kappa set by the whole distribution through the
counterion density, so the reduced-tail rule is simply wrong for them. They
have their own tabs.

**Verified**: with `srel = 0, nbins = 1` the generic path reproduces the
ordinary one-component setters **bit-identically** (maxdiff `0.00e+00` for
HardSphere, SquareWell and LennardJones; the first and third match the
recorded literature values 2.3561180274 and 2.1635946756).

---

### Yukawa potentials: sign and range conventions

Both Yukawa methods use the SAME sign convention, with each tail's amplitude
independently signed:

    K_i > 0  ->  ATTRACTIVE tail  (beta u < 0)
    K_i < 0  ->  REPULSIVE tail   (beta u > 0)

They differ only in how the range is parameterised:

| method | range parameter | tails |
|---|---|---|
| `HS3Yukawa(K1, lambda1, ...)` | decay length lambda | 3 |
| `HardSphereDoubleYukawa(K1, z1, K2, z2)` | screening z = 1/lambda | 2 |

so, with no sign changes anywhere,

    HardSphereDoubleYukawa(K1, z1, K2, z2)
      ==  HS3Yukawa(K1, 1/z1, K2, 1/z2, 0, 1)

verified to 1e-16 across all four sign combinations. The double-Yukawa entry
exists only because published parameters are usually quoted as screening
parameters z; inverting them by hand is a slip that still produces a
plausible-looking curve. Neither argument position is reserved for the
attractive or the repulsive term -- two attractions, two repulsions, or one
of each are all valid.

The repulsive/attractive split is assigned **per term by the sign of its
amplitude**, so the closures that read it (HMSA, SMSA, CG, Carbajal-Tinoco)
see the real decomposition for any sign combination.

**Convergence limit.** Strong attraction defeats Picard: with z1 = 1.8,
K2 = -0.5, z2 = 6, it diverges past K1 ~ 1.5 at every volume fraction tried,
while g_max rises smoothly (1.368, 1.444, 1.562, 2.060) below that. Use
SUNDIALS KIN_FP or Newton-Krylov for attractive systems. If no solver
converges, the state may genuinely be near the spinodal.

## 2. Size classes: the rule depends on the distribution

All five distributions have **analytic moments** — that is never the problem.
The problem is the map from moments to nodes (Golub-Welsch), which is
classically ill-conditioned even with exact input. Measured condition numbers
of the log-normal Hankel matrix:

|  | s=0.3 | s=0.4 | s=0.5 |
|---|---|---|---|
| N=6 | 1.6e7 | 1.7e7 | 1.2e8 |
| N=8 | 2.1e10 | 1.9e11 | 4.7e13 |
| N=10 | 7.0e13 | 1.7e16 | 5.4e19 |
| N=12 | 6.4e17 | 1.2e21 | 2.3e27 |

float64 carries ~1e16, and Cholesky does **not** raise past that — it silently
returns nonsense. So a closed-form rule is used wherever one exists:

| distribution | rule | precision |
|---|---|---|
| Schulz / gamma | generalised Gauss-Laguerre | float64 |
| Gaussian | Gauss-Hermite | float64 |
| log-normal | Golub-Welsch | mpmath |
| Weibull | Golub-Welsch | mpmath |

All four are moment-exact to ~1e-16 (`polydisperse_nodes.momentError()`).

**Gaussian guard.** Gauss-Hermite puts its outermost node at u ~ +/-2.86, so
sigma goes non-positive around s ~ 0.35. A form factor tolerates a vanishing
bin; a structure factor does not, because a hard core must be placed at every
`sigma_ij`. Nodes below `1e-3*<sigma>` are dropped and the weights
renormalised — sacrificing exactness rather than physicality. The test is
deliberately RELATIVE: at s = 0.35 the node sits at 1e-4, positive but still a
zero-diameter particle, so a bare `> 0` check is not enough.

---

## 3. Two class counts, and why

**Classes (S)** and **Classes (form f.)** are separate controls, because the
two averages behave differently in sigma:

* `S_ij(Q)` varies **smoothly** with size — 3-5 moment-matched classes suffice,
  and the OZ solve costs O(p^2) pair transforms, so p should stay small
* `<|F|^2>` **oscillates** — at high Q the phase `Q*R` spans about
  `Q*<sigma>*s` radians across the distribution

Rule of thumb: **nFF >~ Qmax * sigma * s**.

### The bug this fixed

Reported as: *"for a size distribution with sigma 0.3 no oscillations should
be visible at larger q, but they are"*. Correct — and the cause was not
resolution but a convention error.

`S^AL_ij = delta_ij + sqrt(rho_i rho_j) h_ij`. Interpolating the **whole**
matrix onto the fine grid smears the Kronecker delta into a band, which turns
the incoherent `sum_i |F_i|^2` into the coherent `|sum_i F_i|^2` — and
coherent addition of form factors is exactly what oscillates. Only `h_ij` is
interpolated now; the delta is re-imposed exactly on the fine grid.

Validated against brute-force integration of `<|F|^2>` (4000 points):

| | ripple at high Q |
|---|---|
| brute-force reference, s = 0.3 | 0.038 |
| 5 classes (before) | 1.91 |
| nFF = 40 | **0.041** |
| nFF = 160 | 0.041 |

Two earlier hypotheses were wrong and are recorded because they look
plausible: "too few classes" (disproved — the ripple did not converge away by
30 classes) and "Gauss nodes are unsuited to oscillatory integrands" (a true
statement, and the dense non-Gauss grid was the right change, but not the
cause — the ripple got *worse*, 1.91 -> 3.14). The brute-force reference is
what localised it, by showing `I_dilute` correct at 0.040 while `I_exact` gave
2.79 at phi = 1e-6, where S must be the identity.

The fine grid is also constrained to lie **inside the coarse hull**, since
S_ij is only known at the coarse classes and clamping a wide interval to one
S value reintroduces ringing.

---

## 4. Length scale, radius, and the Q axis

**Mean radius** sets the length scale. Q is then a genuine inverse length.

The OZ equations are **always solved in reduced units** (mean diameter = 1):
the solver's radial grid spans only about 41 diameters, so handing it a
physical sigma of 100 would put every hard core off the end of the grid. The
scale is applied *after* the solve, to the diameters, the number densities
(`n ~ 1/L^3`, which keeps phi invariant) and the q axis (`q_reduced = Q*L`).
The tail parameters are already reduced, so they are untouched.

**The scattering radius is the CORE radius**, `R_i = sigma_i/2`, for every
class, and the interaction radius is whatever that form factor declares:

```python
self.R = self.ff.outer_radius(self.sigma / 2.0)
```

For a plain **Sphere** the two coincide, `outer_radius(R) == R`.

For **core-shell** the tab builds `CoreShellFixedShell`, which puts the
polydispersity on the CORE and gives every particle the same shell thickness,
so `outer_radius(R) = R + dR` and the hard core sits at the shell's outer
surface. That is what makes the `delta = c * dR` link coherent: dR is a real
thickness, so c = 1 means the attraction range IS the layer and c = 2 means
two layers overlap at contact.

> An earlier version of this section said the model is polydisperse in the
> OUTER radius with the core at `ratio*R`. That describes the older
> `CoreShell` class, which the tab no longer uses. Both classes remain in
> `polydisperse_yukawa_sas.py`; only `CoreShellFixedShell` is reachable from
> the interface.

`outer_radius()` is the hook for any other arrangement. A hydrated sphere, a
polymer corona or a charged particle whose effective hard core exceeds its
scattering radius is a NEW FORM FACTOR overriding that one method -- not a
GUI parameter. No interface could expose the radius conventions of every form
factor without becoming unreadable, and the ones that matter differ in their
scattering too, so a new class is the honest unit of extension.

Verified at meanRadius = 50 (sigma = 100, R = 50): the S(Q) peak moves to
Q = 0.06608 while `Q*sigma = 6.608` (hard spheres expect ~2pi = 6.28) and
`Q*R = 4.496` at the first I(Q) minimum (exact sphere zero 4.493) — both
unchanged from the dimensionless case, i.e. a pure change of units.

**Known limitation**: scattering size and interaction size are perfectly
correlated by construction — no independent width, no offset. That is right
for charged colloids or bare silica, where the particle *is* the hard core. It
is wrong for sterically stabilised particles (the brush is often
contrast-matched, so R_scatter < sigma/2), for charged colloids at low salt
(the effective core can exceed the physical particle), and for solvated or
partially matched shells. A single scale factor `R = f*sigma/2` would cover
the first two; decoupling the *widths* is a larger change, because it breaks
the one-to-one class correspondence that lets S_ij and F share an index.

This assumption is inherited from `polydisperse_yukawa_sas`, so the Yukawa and
Robertus tabs make it too.

---

## 5. Closures

Offered: `ozLib.multicomponentCapableClosures()` — 19 of 23. The four excluded
each need something that only exists for a single component:

| excluded | reason |
|---|---|
| Reference HNC | needs a one-component hard-sphere reference solve (g0/G0) |
| Modified HNC | needs an analytic one-component PY bridge at one packing fraction; the mixture version is the Lado variational problem |
| Rescaled MSA | a one-component diameter-rescaling procedure; the polydisperse counterpart is the separate analytic `polydisperse_rmsa.py` |
| EuRah | uses a precomputed one-component HS/PY array |

MS, VM and CJVM are **not** excluded. They are structurally fine and do run
multicomponent; they fail only because their square-root bridge overflows
`exp(G+B)` for strongly coupled charged systems — a closure-domain limit that
applies equally in one component. Hiding them would misattribute that to the
polydispersity machinery.

Extended Rogers-Young carries a second parameter `a` (a = 0 reduces exactly to
RY); it appears automatically, declared in `ozLib.SECOND_CLOSURE_PARAM`.

---

## 6. Thermodynamic consistency: solving for alpha

Tick **solve alpha by thermodynamic consistency** and alpha is fixed by
requiring the compressibility and virial routes to the pressure to agree,
rather than typed in. The entry is disabled while it is ticked, so the field
and the solved value cannot silently disagree. The checkbox is only enabled
for closures that carry a free parameter and are listed in
`ozLib.CONSISTENT_PARAMETER_CLOSURES`.

```
chi^-1_comp = 1 - n sum_ij x_i x_j chat_ij(0)          (quadratic extrap. to q=0)
chi^-1_vir  = d(betaP)/dn
betaP/n     = 1 + (2pi/3) n sum_ij x_i x_j sigma_ij^3 g_ij(sigma_ij+)
                - (2pi/3) n sum_ij x_i x_j int r^3 g_ij (beta U_ij)' dr
```

The **contact term** is kept. D'Aguanno & Klein omit it legitimately — their
macroions are so strongly charged that `g(sigma+) ~ 0` — but that is not safe
for a general potential with a reachable core, where the term is first order
in the pressure. `(beta U)'` is differentiated numerically, because the
builder reuses arbitrary setters and cannot know their analytic derivative.

The reduced-tail potential is density-independent, so perturbing phi leaves it
untouched. That is not automatic: in the charge-coupled route kappa depends on
the counterion density, and rebuilding at `n +/- dn` silently differentiates a
density-dependent potential.

**Validated against analytic PY hard spheres at eta = 0.3:**

| quantity | analytic | measured |
|---|---|---|
| chi^-1 | 10.6622 | 11.0678 (3.8%, grid) |
| betaP/rho | 3.8163 | 3.8896 (1.9%, grid) |
| PY residual | +1.2482 | +1.3284 (6%) |

and the RY residual crosses zero between alpha = 0.1 and 0.5, converging on
the HNC limit at large alpha (-4.1495 vs HNC -4.1497). A live run returns
`alpha = 0.2441 [consistent: residual -6.94e-05, relative 6.7e-06]`.

**Consistency is judged RELATIVE to chi^-1**, which is of order 10 for a dense
fluid. An absolute threshold rejected a perfectly good root (alpha = 0.2443,
residual -1.2e-3, i.e. a relative 1e-4) as "no consistent value found".

**A consistent alpha need not exist.** The residual is often monotone and
already nonzero in the alpha -> 0 limit, meaning the base closure is itself
inconsistent for that state and no mixing repairs it. That is reported, not
hidden behind a fallback presented as a fit.

**Caution.** First tested at `srel = 0.2, nbins = 3, phi = 0.3`, where the
residual saturated near +2.11 with almost no alpha dependence, and this was
wrongly read as a broken virial route. With three moment-matched classes the
largest diameter reaches sigma ~ 2.48, so at phi = 0.3 the biggest spheres are
at or past their own close packing: the state was unreachable, not the code
wrong. Test consistency at phi ~ 0.15 for s = 0.2, or use fewer classes.

Cost: **three OZ solves per trial alpha**, so it runs in the worker thread
with progress messages.

---

## 7. Fitting to measured data

`Load data...` reads a two- or three-column ASCII/CSV curve (Q, I, optionally
dI). Whitespace, comma, semicolon and tab separators are accepted, `#`, `%`
and `//` comments and text headers are skipped, and rows that will not parse
as numbers are dropped rather than raising -- so a stray trailing line does
not lose the file. SASfit exports load directly.

Workflow:

1. **Compute** once to get the curve roughly right by eye. This is not
   politeness: every fit evaluation is a full Ornstein-Zernike solve, so the
   starting guess is worth a minute of your time.
2. **Load data...**
3. Tick the parameters to **vary**. Mean radius, relative width and volume
   fraction are offered by default; the closure parameter and the potential's
   own arguments appear when they exist, and the set is rebuilt whenever you
   change the potential or the closure.
4. **Fit**. Progress goes to the status line; the fitted values are written
   back into the input fields, so a following Compute reproduces the fit.

### Scale and background are not fitted nonlinearly

A measured curve is `I_obs = scale * I_model(p) + background`, and both enter
LINEARLY. They are therefore obtained exactly at every iteration by a
two-parameter weighted linear least squares against the current model shape,
rather than being handed to the optimiser. This removes the two most strongly
correlated parameters from the nonlinear problem -- scale trades against
volume fraction and contrast, background against everything at high Q --
costs nothing, since the model shape is already computed, and stops the
optimiser spending an OZ solve on a scale factor. It is why a three-parameter
fit converges in about 30 evaluations rather than a few hundred.

### Verified

Synthetic hard-sphere data (R = 50, s = 0.22, phi = 0.18, scale 1.7e-3,
background 0.012, 3 % noise), started deliberately wrong at R = 40, s = 0.10,
phi = 0.30:

| parameter | fitted | true | error |
|---|---|---|---|
| meanRadius | 49.52 | 50.0 | 1.0 % |
| srel | 0.2187 | 0.22 | 0.6 % |
| phi | 0.1657 | 0.18 | 7.9 % |
| scale | 1.82e-3 | 1.7e-3 | -- |
| background | 0.0128 | 0.012 | -- |

chi2_red = 0.903 in 31 evaluations, no failed solves. The residual error on
phi is the visible consequence of its correlation with the scale factor: it
is the parameter such data constrains least, and a point estimate is arguably
the wrong output for it (see "Not done").

### Interrupt

**Interrupt** stops a running fit as well as a running single solve. The flag
is checked BETWEEN model evaluations rather than inside the OZ solve, so the
response time is one solve -- measured at 1.5 s on a fit that would have run
84 s. Interrupting mid-solve was deliberately not done: it would leave the
solver partially updated with no converged Gamma to fall back on.

An interrupted fit returns the **best point reached**, not the last one
tried, since the optimiser may well have been probing a poor direction when
the interrupt arrived. The summary is headed "FIT (INTERRUPTED -- best point
so far)" and the status line says so, so a partial fit cannot be mistaken for
a converged one.

### Cost, and what to do about it

Each evaluation is one OZ solve; the numerical Jacobian adds one solve per
parameter per iteration. Keep **Classes (S)** at 3 while fitting and raise it
for the final Compute -- **Classes (form f.)** can stay high, since it does
not enter the OZ solve.

A failed solve is not an error during a fit: the optimiser explores
unphysical corners, and those return a large residual so it walks away.
Watch `failedEvaluations` in the summary. A nonzero count means the fit is
straddling a region where solves fail, and a derivative-free method is then
the better tool -- DFO-LS is a one-line swap on the same `_residuals`
callable.

### Resolution smearing

SANS resolution dQ/Q is of order 10 % and smears form-factor oscillations in
exactly the way polydispersity does. Fitting without it lets the model absorb
the instrument into `srel`. Measured on smeared synthetic data with a true
`srel` of 0.15:

| | chi2_red | R | srel | phi |
|---|---|---|---|---|
| without smearing | 1.073 | 47.82 | **0.1726 (15.1 % high)** | 0.1541 |
| with smearing | 0.750 | 49.96 | **0.1497 (0.2 % high)** | 0.1490 |

Note the chi-squared: without smearing the fit does not look bad, it is
simply wrong. That is bias, not imprecision, and more data does not fix it.

To use it, supply a FOURTH column of dQ and read it with
`loadCurve(path, withResolution=True)`, then pass
`PolydisperseFit(..., resolution=Resolution(Q, dQ))`.

**Convention:** dQ is read as the Gaussian SIGMA, not the FWHM. The two
differ by 2.355, and using the wrong one silently rescales the fitted
polydispersity -- precisely the bias smearing exists to remove. Pass
`Resolution(Q, dQ, fwhm=True)` if the reduction writes FWHM.

**Why it is cheap here.** Smearing needs the model on an extended, denser Q
grid, which for most models multiplies the cost several-fold. Here it is
nearly free, because the expensive part -- the Ornstein-Zernike solve -- does
not depend on the Q grid at all. It also leaves the scale/background
elimination intact, since smearing is linear and the background is not
smeared.

Verified: dQ -> 0 reproduces the unsmeared curve to 1.1e-16, kernel rows
normalise to 1.000000, and the extended grid reaches below Q_min.

### Parameter uncertainties and correlations

`run()` returns `uncertainty`, `correlation` and `parameterOrder` alongside
the fitted values, computed from the Jacobian `least_squares` has already
produced -- no extra model evaluations. On the synthetic test:

    meanRadius  49.5228 +/- 0.2262
    srel         0.2187 +/- 0.0020
    phi          0.1657 +/- 0.0060

                  meanRadius   srel    phi
    meanRadius        1.000  -0.224   0.751
    srel             -0.224   1.000   0.389
    phi               0.751   0.389   1.000

The correlation matrix is the useful part. **phi and R are correlated at
0.75**: the fit can compensate a slightly small radius with a slightly small
volume fraction, since both act on the position and height of the
structure-factor peak. That, not noise, is why phi is the least
well-determined parameter in the table further up. Note that `scale` does not
appear -- it is eliminated analytically and never enters the Jacobian.

With trustworthy dI the covariance is `(J^T J)^-1`; without it, it is scaled
by chi2_red, since the absolute residual scale is then arbitrary.

**Caveat.** This is a linearised estimate, so it assumes a quadratic cost
surface near the minimum and understates the error for bounded or strongly
correlated problems -- on the test above two parameters sit 2.1 and 2.4 sigma
from truth on 3 % noise, which is a little far. Good for relative precision
and for spotting correlation; for a published error bar, sample the posterior
with `fitWithBumps(..., method="dream")`.

### Measured alternatives

On the synthetic problem above, with three parameters:

| method | evaluations | time | result |
|---|---|---|---|
| `scipy.least_squares` (default) | 36 | 24 s | -- |
| DFO-LS | 94 | 71 s | identical to 4 d.p. |
| bumps `amoeba` | 158 | 80 s | identical to 4 d.p. |

Derivative-free loses here because with only three parameters a
finite-difference Jacobian is cheap and yields a genuine Gauss-Newton step.
It should win once the parameter count grows, or where solves fail.

### Global search and uncertainties: the bumps back end

`polydisperse_fit.fitWithBumps(fitter, method=...)` exposes the optimisers of
[bumps](https://bumps.readthedocs.io) -- from the DANSE project, and the
fitting engine behind Refl1D -- namely `dream`, `de`, `amoeba`, `newton`,
`lm` and `pt`. It reuses the same residual machinery, including the exact
linear elimination of scale and background, so both back ends optimise
exactly the same objective and their results are directly comparable.

Two of them do things `least_squares` cannot:

* **`de`** (differential evolution) is a GLOBAL search, so it does not depend
  on the user getting close by eye first. That matters less for a hard sphere
  than for a charged Yukawa with a free closure parameter.
* **`dream`** samples a POSTERIOR rather than returning a point, and adds
  `uncertainty` and `correlation` keys to the result. This is the honest
  answer to the 7.9 % error on `phi` in the table above: `phi` correlates
  strongly with the scale factor, so with 3 % noise the data does not
  constrain it well, and a correlation matrix says so where a chi-squared
  value cannot.

**Cost.** Every evaluation is one Ornstein-Zernike solve. `de` needs hundreds
and `dream` thousands, so these are overnight tools, not interactive ones. The
sensible pattern is a global method to find the basin, then
`PolydisperseFit.run()` to polish -- about 30 evaluations.

**State of testing, stated plainly.** `amoeba` has been run end to end and
reproduces `least_squares` exactly. `de` and `dream` use the identical code
path -- only the fitter string differs -- but have NOT been run to completion,
so the `uncertainty` and `correlation` keys are untested plumbing rather than
a demonstrated feature. `bumps` is an optional dependency: nothing else in
the package imports it, and the default fit path does not need it.

**A trap worth knowing.** bumps orders the fitted parameters alphabetically,
not in the order they are declared, so `result.x` must be matched by label.
An earlier version zipped it against the declared order and produced a
plausible-looking fit (chi2_red 95 instead of 0.90) in which two parameters
held each other's values while the third was correct. Nothing raised; it
simply looked like a poor optimiser.

## 8. Open items

0. **The three documents are not yet consistent with each other.** A check
   across them found tonight's findings landing unevenly:

   | finding | manuscript | supplement | report | Sphinx |
   |---|---|---|---|---|
   | S(0): type 1 low by 5.27 % at phi=0.4, type 4 by 0.036 % | yes | yes | no | manual only |
   | Rician vs Gaussian, 44 % against 34 % below Qmin | yes | yes | no | no |
   | lstsq conditioning at 1e20 | yes | yes | no | no |
   | Carbajal-Tinoco is the 2022 paper, not 2008 | yes | no | yes | no |
   | Carbajal-Tinoco domain, Gamma_min = -0.509 | yes | no | yes | this file |
   | RMSA exact only where rescaling does not engage | yes | no | no | no |
   | two-Yukawa 0.21 %, locally reproducible | yes | yes | no | manual |

   The transform finding is the one that matters most and is the one most
   unevenly spread: it is a systematic bias in the compressibility, so a fit
   absorbs it by moving phi, and a reader of the report alone would not know.

   THE UNDERLYING CAUSE is that the report and the manuscript maintain
   SEPARATE BIBLIOGRAPHIES for the same papers -- `daguanno1991` against
   `DAguannoKlein1991`, and so on across 37 `\bibitem` entries. A correction
   to one does not reach the other, which is exactly how the
   Carbajal-Tinoco misattribution survived in two places at once.

   AND IT HAS ALREADY LEAKED INTO `ozgui.bib` ITSELF, which carries NINE
   DUPLICATE PAIRS -- the same paper under both conventions:

       Baxter1968 / baxter1968          BlumHoye1978 / blumhoye1978
       DAguannoKlein1991 / daguanno1991 DAguannoKlein1992 / daguanno1992
       HayterPenfold1981 / hayter1981   PihlajamaaJanssen2024 / pihlajamaa2024
       Robertus1989 / robertus1989      RogersYoung1984 / rogersyoung1984
       ZerahHansen1986 / zerahhansen1986

   THE TRANSCRIPTION IS ALREADY DONE, which was not obvious until a header
   comment in `ozgui.bib` turned up: the lowercase keys were generated by
   `tools/bibitem_to_bibtex.py` from the report's own `\bibitem` entries in
   an earlier session. They are not stray duplicates but the report's
   bibliography already in BibTeX form, sitting beside the manuscript's.
   The nine pairs are simply the papers both documents cite.

   So the remaining work is smaller than it looks:
     1. deduplicate the nine pairs, checking volume, pages and year agree
        before discarding either member;
     2. rewrite the report's `\cite` keys to the survivors;
     3. replace `thebibliography` in the report with `\bibliography{ozgui}`
        and add `bibtex` to `build_report.sh` if it is not already there.

   MATCH ON (VOLUME, FIRST PAGE, YEAR), not on titles and never on
   surnames. That triple is effectively unique and is present on both sides,
   where DOIs are not -- the report's 37 `\bibitem`s carry none at all, and
   only 33 of the 73 bib entries do. It resolves 31 of the 37 outright,
   including `lee1995` against `lee1999`, two papers by the same author in
   different years which a surname match conflates and which is the error
   being repaired. Six need a human decision.

   Success criterion: zero undefined citations from `build_report.sh` AND
   from the manuscript build, with 73 entries reduced to 64.

1. Radius conventions are per FORM FACTOR, by design, and adding one is the
   way to get a different one. `sigma/2` is the core radius; the interaction
   radius is whatever the form factor's `outer_radius()` returns, which is
   `R` for a sphere and `R + dR` for the core-shell the tab builds (see §4).

   A hydrated sphere, a polymer corona, or a charged particle whose
   effective hard core exceeds its scattering radius therefore needs a new
   form factor class overriding that one method -- not a new GUI control. No
   interface could carry the radius conventions of every form factor without
   becoming unreadable, and those cases differ in their SCATTERING too, so a
   class is the honest unit of extension. Recorded here because "decouple
   the scattering radius from the hard-core radius" stood as an open item
   for some time and is better read as a description of how extension works.
2. `Carbajal-Tinoco`: **the bridge equation has a bounded domain**, and
   outside it there is no solution to find. This is a property of the
   closure, not a defect in the solver.

   THE STRUCTURE. The closure is implicit, b = T(Gamma + b) with
   T(w) = e[(2-w)e^w - 2 - w]/(e^w - 1). Substituting w = Gamma + b turns it
   into an explicit map

       Gamma(w) = w - T(w)

   whose RANGE is the set of Gamma for which a solution exists. That range
   is bounded below: Gamma(w) has a single minimum, and below it the
   equation has no root, above it exactly two.

   | e | Gamma_min | at w |
   |---|---|---|
   | 1.0 | -2.000 | -- |
   | 2.0 | -0.781 | -1.63 |
   | 3.0 (lambda = 0) | **-0.509** | -1.04 |
   | 3.4 (lambda = +0.4) | **-0.447** | -0.91 |

   Three things follow. Gamma_min RISES towards zero as e grows, so lambda >
   0 shrinks the admissible region -- which is the real reason that case
   fails, not the repulsive fixed point diagnosed earlier. The two roots
   above the fold are the usual physical/unphysical pair, and nothing in the
   code chooses between them. And Gamma ~ -0.5 is not an exotic value: at
   contact it is routinely of that order at moderate density, so this is
   reachable in ordinary use rather than in a corner.

   Gamma_min is computable rather than scanned: it is where dGamma/dw = 0,
   i.e. T'(w) = 1, a scalar root-find per e. The code could then say "this
   state point is outside the closure's domain" precisely, instead of
   iterating 500 times and returning the last iterate whether or not it
   converged -- which is what it does now, silently, including for the
   lambda <= 0 that is validated to 2e-13.

   THE LITERATURE SAYS THIS IS NORMAL. A closure having a no-solution domain
   is a known and accepted phenomenon: Amokrane, Ayadim & Malherbe
   (J. Chem. Phys. 123, 174508, 2005) describe "the major limitation of the
   RHNC closure in the case of highly asymmetric mixtures -- the wide domain
   of packing fractions in which it has no solution", and propose a modified
   closure specifically to shrink that domain. So the finding is not that
   something is broken but that this closure's domain has never been mapped
   here, and the code does not report when it leaves it.

   ALSO: THE CITATION WAS WRONG, and is now fixed. The closure is
   Carbajal-Tinoco, J. Chem. Phys. 157, 204502 (2022), Eqs. 15-16 -- the
   "local approximation" bridge function B_LA(r). The code cited the same
   author's 2008 paper, JCP 128, 184507, which is a DIFFERENT closure:
   Extended Rogers-Young, implemented separately here. The error came from
   OrnsteinZernike.jl's source docstring, which contradicts Table I of the
   paper accompanying that package -- and this implementation inherited it
   along with the code, down to the `Tinoko` spelling. Reading the 2008
   paper while looking for this formula finds Eq. 13 and no trace of it,
   which is how it surfaced. Both papers are now in `ozgui.bib`, and the
   upstream package's docstring is worth a bug report.

   USEFUL DETAIL FROM THE SOURCE: lambda interpolates between known
   closures. lambda = 0 recovers Martynov-Sarkisov, lambda -> infinity
   recovers HNC. So the shrinking domain as lambda grows is the domain
   shrinking as the closure moves from MS towards HNC.

   NEITHER PAPER MENTIONS THE DOMAIN. The 2022 source notes only that
   Eq. 15 is "a transcendental equation for B(r) as a function of gamma"
   and says nothing about when it has no root. Pihlajamaa & Janssen
   (Phys. Rev. E 110, 044608, 2024) repeatedly exclude closures from their
   figures "due to convergence issues at this state point" and because
   they "converged to nonphysical results", without diagnosing which
   closures fail where or why. So the phenomenon is known and accepted --
   as Amokrane et al. also show for RHNC -- but the location of the
   boundary for THIS closure appears not to be published.

   WHAT THE REFERENCE IMPLEMENTATION DOES: `find_zero(f, gamma)` from
   Roots.jl, with no domain check, so outside the domain it raises. That is
   the better behaviour and the one to match: returning the 500th iterate,
   as this code does, is not a milder failure but an undetected one. The
   obstacle is that a check on every closure evaluation fires during the
   iteration path from Gamma = 0 even when the converged solution is
   perfectly good -- tried, and it broke lambda <= 0 as well. A check
   applied to the CONVERGED solution rather than to each evaluation would
   give the reference's semantics without the false alarms.
3. Uncertainty estimation is available two ways, and the plumbing of both is
   now verified. The linearised Jacobian covariance is computed on every fit
   and reported with the correlation matrix; it is reliable for spotting
   correlation. The posterior route `fitWithBumps(..., method="dream")` has
   now been run to completion for the first time -- `tools/numerics_test.py`,
   `testDreamPlumbing` -- and all of it resolves: 3697 evaluations, none
   failed, `uncertainty` populated from `result.dx`, and `correlation` a
   (2, 2) matrix with unit diagonal and a symmetric off-diagonal. The bare
   `except Exception: pass` around the state lookup was NOT hiding a broken
   attribute path, which was the live worry.

   WHAT REMAINS. The test fits noiseless synthetic data, so the likelihood is
   nearly flat: chi2 comes out at 4e-7, the returned uncertainties are
   essentially the prior widths (0.097 on a phi bounded [0.10, 0.40]) and the
   correlation is 0.06 where phi and srel would be expected to correlate
   strongly. Those are the right outputs for a zero-noise fit and say nothing
   about whether the posterior is MEANINGFUL. One run on real data with real
   error bars is still owed -- at 2.5 s per evaluation and some ten thousand
   evaluations that is an overnight job, so reduce the size classes to 3.
4. Resolution smearing **is** exposed in the tab: `Load data...` reads an
   optional 4th dQ column and enables an "apply Q resolution (dQ column)"
   checkbox, ticked by default when the column is present. Verified live:
   loading a 4-column file gave dQ/Q 0.080..0.080, built a 320-point extended
   grid, and fitted R, srel and phi to 0.1 %, 0.2 % and 0.7 % with
   chi2_red = 0.750. dQ is read as the kernel's SIGMA; use
   `Resolution(Q, dQ, fwhm=True)` if the reduction writes FWHM.

   The kernel is now RICIAN rather than Gaussian. A small-angle instrument
   broadens Q in two dimensions in the detector plane and the data are
   radially averaged, so the distribution of the MAGNITUDE about a true Q0 is
   Rician; a Gaussian in Q is its large-Q0/sigma limit. At the dQ/Q = 0.08 of
   the verification above the two agree closely and those figures stand. At
   dQ/Q = 0.55, which real spherical-shell data reach at their lowest point,
   they do not: a Gaussian places 44 % of that point's weight below Q_min and
   some of it at NEGATIVE Q, against 34 % for the Rician form, whose leading
   factor of Q excludes the origin by construction. Both reproduce the
   unsmeared curve to 2e-16 as dQ -> 0, which is the check that catches a
   mis-normalised kernel. Pass `kernel="gaussian"` to compare.
5. `S_partials` below the solver's own q grid: the clamp is GONE, replaced
   by a Taylor extrapolation with a divergence guard. One check remains.

   S(q) for a liquid is even and analytic at the origin, so A + B q^2 is the
   leading behaviour rather than a convenient fit; A and B come from the
   solver's own two lowest grid points, so it bridges only the gap between
   q_0 and zero. Verified continuous across the edge (3.7e-7), exact at q_0,
   and varying below it where the clamp held flat.

   A divergence is REFUSED rather than extrapolated. S ~ c q^-alpha means the
   compressibility is running away, at or near a spinodal, and there is no
   finite S(0) to extrapolate TO -- a parabola through it returns a number
   that means nothing and the fit would use it without complaint. The
   discriminant is the log-log slope of the trace over the lowest few grid
   points: hard spheres at phi = 0.10, 0.30 and 0.45 give +0.0009, -0.0000
   and -0.0008; a synthetic S proportional to 1/q gives -1.000 exactly; the
   threshold is -0.1, three orders of headroom. The message says what to do,
   including that extending the q grid removes the need to extrapolate.

   STILL OWED: comparison against the ANALYTIC S(0). Percus-Yevick hard
   spheres have a closed form for the compressibility route, so extrapolating
   to q = 0 and comparing would show the q^2 form is RIGHT rather than merely
   continuous -- which is all the above establishes. A case in
   `tools/numerics_test.py` at two or three volume fractions closes it.

   Separately: the mixture validation tab at phi = 0.4 shows a solve that is
   a fixed point to 7e-12, machine precision, yet differs from the analytic
   references by about 1e-3 and barely shrinks under fourfold refinement with
   the real-space range held constant. That was suspected to be this same
   question seen from the other side. With the extrapolation now in place,
   RE-RUNNING THAT COMPARISON is five minutes and the highest-value check
   available.
