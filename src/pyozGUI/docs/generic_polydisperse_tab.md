# The generic polydisperse tab

`Polydisperse (any potential)` — any potential x any closure x any form
factor, with the exact I(Q) shown against all six of SASfit's approximate
schemes.

The three older polydisperse tabs are special cases of it:

| tab | potential | closure |
|---|---|---|
| Polydisperse Yukawa | one-Yukawa, analytic | MSA / RMSA only |
| Robertus SHS | adhesive spheres, analytic | PY only |
| RY Polydisperse Yukawa | charge-coupled Yukawa, numerical | RY only |
| **Polydisperse (any potential)** | **16 potentials** | **19 closures** |

They are kept because each is faster or better validated in its own niche.

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

## 2. Size classes: the rule depends on the distribution

All four distributions have **analytic moments** — that is never the problem.
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

**The scattering radius equals the hard-core radius**, `R_i = sigma_i/2`, for
every class:

```python
self.R = self.ff.outer_radius(self.sigma / 2.0)     # outer_radius(R) == R
```

For **core-shell** the model is polydisperse in the OUTER radius, with the
core at `ratio*R`, so the shell's outer surface coincides with the hard-sphere
contact radius.

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

### Measured alternatives

On the synthetic problem above, with three parameters:

| method | evaluations | time | result |
|---|---|---|---|
| `scipy.least_squares` (default) | 36 | 24 s | -- |
| DFO-LS | 94 | 71 s | identical to 4 d.p. |

Derivative-free loses here because with only three parameters a
finite-difference Jacobian is cheap and yields a genuine Gauss-Newton step.
It should win once the parameter count grows, or where solves fail.

## 8. Not done

1. **Distribution selector** in the tab — the engine supports Schulz,
   Gaussian, log-normal and Weibull, but the tab always passes Schulz.
2. The tab does not subsume the three older polydisperse tabs; it sits
   alongside them.
3. No decoupling of scattering radius from hard-core radius (see section 4).
5. No uncertainty estimate on the fitted parameters. Given the phi/scale
   correlation above, a posterior would be more honest than a point estimate;
   Bumps' DREAM, reachable through FitBenchmarking, is the natural route.
6. No global search. The fit is local, so the workflow depends on the user
   getting close by eye first. GOFit (global, least-squares aware, already
   shipped with Mantid) is the obvious candidate.
