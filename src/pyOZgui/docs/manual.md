# ozLib — manual and quick reference

`ozLib` is the programmatic entry point to the Ornstein–Zernike solver. The
GUI's Calculate button calls `ozLib.solve()`; anything the GUI can compute is
reachable from a script through the same function, which means a figure made
in the GUI can be reproduced in a batch job without reimplementing anything.

---

## Quick reference

```python
import ozLib

res = ozLib.solve("HardSphere", phi=0.3)          # simplest possible call
q, S = res.q, res.Sq                              # both plain numpy arrays
```

| what | how |
|---|---|
| one solve | `ozLib.solve(potential, phi, ...)` → `OZResult` |
| agreement of several solvers | `ozLib.solveWithConsensus(...)` |
| which closures work for mixtures | `ozLib.multicomponentCapableClosures()` |
| is this potential a mixture one | `ozLib.isMulticomponentPotential(name)` |
| does this closure take a 2nd parameter | `ozLib.secondClosureParam(name)` |
| names of everything | `ozLib.CLOSURE_SETTERS`, `SOLVER_CLASSES`, `CURVE_NAMES` |

### `OZResult` attributes

Curves are plain arrays on the solver's own grid, **not** functions:

| real space (vs `res.r`) | reciprocal (vs `res.q`) |
|---|---|
| `gr` g(r), `cr` c(r), `hr` h(r) | `Sq` S(Q) |
| `gamma` γ(r), `Br` B(r), `yr` y(r) | |
| `Ur` βu(r), `fr` Mayer f(r) | |

Plus `potential`, `potentialArgs`, `phi`, `closure`, `closureParam`,
`solverName`, and `solverInstance` for the underlying solver object.

**Beware the three spellings.** The same two quantities are named differently
at each layer, and writing a call from memory is how an afternoon gets lost:

| layer | grid size | resolution | q and S(Q) |
|---|---|---|---|
| `ozLib.solve` | `numberOfRadialSamplingPoints` | `hardSphereDiameterInPoints` | returns `OZResult` |
| `OZResult` | — | — | `.q`, `.Sq` (arrays) |
| solver object | — | — | `getqArray()`, `getSq()` (methods) |
| `GenericPolydisperseSAS` | `gridN` | `pointsPerSigma` | `I_exact(Q)` |

---

## `solve()` in full

```python
ozLib.solve(potential, phi,
            potentialArgs=(),
            closure="Percus-Yevick",
            closureParam=None, closureParam2=None,
            findConsistentParameter=False,
            solver="scipy Anderson",
            maxIterations=1000,
            numberOfRadialSamplingPoints=None,
            hardSphereDiameterInPoints=None,
            onSolverCreated=None,
            verify=True,
            verifyWith=("Picard iteration", "Biggs-Andrews"),
            verifyTolerance=1e-3)
```

**`potentialArgs` is positional and its meaning is per-potential.** There is
no keyword form. `(tau, delta)` for StickyHardSphere, `(K1, z1, K2, z2)` for
HardSphereDoubleYukawa — see the table below, or call
`getAvailablePotentialNames()` on a solver instance for the live list.

**`verify=True` is the default and should stay that way.** It re-solves with
two other solvers and raises if they land on different fixed points. That is
not paranoia: the closure equations genuinely admit several solutions, and
a solver reporting success is not saying it found the *physical* one. At a
Lennard-Jones state tested during development, every Newton–Krylov variant
converged to residuals below 1e-11 on branches with min S(Q) ≈ −38 — real
roots, entirely unphysical. Turn verification off for speed only once you
know the state point is well-behaved.

**`findConsistentParameter=True`** solves the closure's own α by
thermodynamic consistency (compressibility route against virial) instead of
taking `closureParam`. It costs three OZ solves per trial value, so a single
Calculate becomes dozens of solves.

---

## Potentials (21)

All lengths are in units of the hard-core diameter σ; all energies in k_BT.

| name | arguments |
|---|---|
| `HardSphere` | — |
| `SquareWell` | `(epsilon, delta)` |
| `StickyHardSphere` | `(tau, delta)` |
| `Yukawa` | `(shieldingLength, strength, doAddHardSphere)` |
| `HardSphereDoubleYukawa` | `(K1, z1, K2, z2)` |
| `HS3Yukawa` | `(K1, l1, K2, l2, K3, l3)` |
| `LennardJones` | `(epsilon)` |
| `SoftSphere` | `(epsilon, n)` |
| `DLVO` | `(kappa, Z, LB, A)` |
| `DLVOHydra` | `(kappa, Z, LB, A, GHY, DH)` |
| `Depletion` | `(sigmaRatio, phi2)` |
| `FermiDistribution` | `(epsilon, xi)` |
| `GGCMn` | `(epsilon, n, alpha)` |
| `IonicMicrogel` | `(Z, ED, KPi, EPSILON)` |
| `PSM` | `(epsilon)` |
| `ParabolicSphere` | `(epsilon)` |
| `PiecewiseConstantHS` | `(eps1, d1, eps2, d2, eps3, d3)` |
| `StarPolymerHighF` | `(NumberOfArms)` |
| `StarPolymerLowF` | `(NumberOfArms)` |
| `Polydisperse` | `(potentialName, potentialArgs, srel, nComponents, ...)` |
| `PolydisperseHardCoreYukawa` | `(srel, nComponents, valence, bjerrum, ...)` |

**Sign convention, verified rather than assumed.** The implementation sets
u(r) = ε inside the well and forms exp(−u), so for `SquareWell` **a positive
ε is REPULSIVE** — a square shoulder. Attraction needs ε < 0. This was
checked directly (ε = +1 gives exp(−u) = 0.368 < 1) after a survey had been
run and written up describing a shoulder as a well.

**`SquareWell` and `StickyHardSphere` take δ as an absolute length**, not as
a fraction of σ, so it is directly comparable with a shell thickness.

---

## Closures (24)

`ozLib.CLOSURE_SETTERS` maps each name to its setter and whether it takes a
parameter. `ozLib.multicomponentCapableClosures()` filters to those valid for
mixtures.

**Parameter-free**: Percus-Yevick, Hypernetted-Chain, MSA, Modified MSA,
Symmetric MSA, Rescaled MSA, Verlet, Martynov-Sarkisov, Duh-Haymet,
Kovalenko-Hirata, Choudhury-Ghosh, Vompe-Martynov, Reference HNC, ZSEP.

**Take an α, settable directly or by consistency** (`ozLib.CONSISTENT_
PARAMETER_CLOSURES`): Rogers-Young, Extended Rogers-Young, HMSA, Modified
HNC, Modified Verlet, BPGG, CJVM, BB, Khanpour, Carbajal-Tinoco.

**Takes a second parameter**: Extended Rogers-Young, `(a, 0.0, "quadratic
coefficient; a = 0 gives plain RY")` — see `ozLib.secondClosureParam`.

---

## Solvers

Nine with SUNDIALS installed, five without. Timings are from
`tools/benchmark.py` on a 4095-point grid; the ranking is what
`ozLib.SOLVER_CLASSES` lists, fastest first.

| name | family | seconds | note |
|---|---|---|---|
| `sundials4py: Fixed-Point (Anderson)` | fixed point | **0.0049** | the default when built |
| `scipy Anderson` | fixed point | 0.0077 | the default without SUNDIALS |
| `MDIIS` | fixed point | 0.0096 | see below |
| `sundials4py: Newton-Krylov (FGMRES)` | Newton–Krylov | 0.0100 | |
| `sundials4py: Newton-Krylov (GMRES)` | Newton–Krylov | 0.0108 | |
| `sundials4py: Newton-Krylov (TFQMR)` | Newton–Krylov | 0.0187 | |
| `scipy Newton-Krylov` | Newton–Krylov | 0.0232 | **see the warning below** |
| `Biggs-Andrews` | fixed point | 0.0402 | |
| `Picard iteration` | fixed point | 0.0493 | plainest; **diverges above φ ≈ 0.42** |

**Picard's limit is 0.42, not the 0.45 this table used to say** — measured on
a 1023-point grid, where the iteration count grows geometrically (41, 49, 60,
… 946 in steps of Δφ = 0.02) and then fails outright with a residual of 1.1.
And **a density ramp does not rescue it**: the contraction factor exceeds 1
there, so the basin has not moved but ceased to exist, and no starting point
inside it helps. Continuation methods fail for the same reason.

**`Anderson acceleration` was removed.** It duplicated `scipy Anderson` —
the same algorithm, hand-written rather than from scipy — and two entries for
one method cost a choice nobody has a basis to make. The module remains
importable; it is simply not registered.

**`MDIIS` is a fallback, not a recommendation.** Third of nine here, and it
fails at φ = 0.58 where KIN_FP and scipy Anderson both converge. Its use is
on installations without SUNDIALS, where scipy Anderson's iteration count
swings erratically with density (28, 116, 40, 179) while MDIIS rises
smoothly — at φ = 0.52 it is the better of the two. Try it when scipy
Anderson stalls.

**Prefer a fixed-point method.** Near a fold the Jacobian is singular, and
the Newton–Krylov family is drawn onto negative-compressibility branches
exactly where the fold lies. Measured at one Lennard-Jones state: the
fixed-point family found g_max = 2.16 with min S(Q) = +0.21; the
Newton–Krylov family found 1.13 and 1.22 with min S(Q) ≈ −38. All had
residuals below 1e-11. They are all correct answers to the question asked;
only one is physical.

**Picard damping (Mann iteration).** `PicardOZsolver` honours a `mannAlpha`
attribute implementing x ← (1−a)x + a·T(x). a = 1 is plain Picard; below 1
it is Mann's iteration, slower but convergent where undamped Picard
diverges — it rescued a Lennard-Jones case at a = 0.5. Set it on the solver
instance, which `onSolverCreated` exists to let you do.

**Every solver exposes `converged`**, declared in `OZsolver.__init__` and
defaulting to `False` — so a solver that forgets to set it reports failure
rather than a success it never established. Until recently only Picard set
it, and the rest returned `None`, which a caller cannot tell apart from
"ran and did not converge".

**But it is not a correctness check**, and the distinction matters more here
than in most numerical work. On one Lennard-Jones state point all four
Newton–Krylov solvers report `converged = True`, are genuine fixed points,
and reach residuals of 10⁻¹³ — at **min S(Q) ≈ −38**. Every question of the
form "did it converge?" answers yes; only the `min S(Q) ≥ 0` screen
separates them from the correct g_max = 2.1636.

So check both, and recompute the residual yourself if it matters: one
operator evaluation, and `tools/residual_check.py` shows it is the only thing
distinguishing a converged solve from a stalled one when a solver's own flag
is optimistic.

---

## Grids and transforms

**N and the resolution are not independent.** The real-space range is

    r_max = N · σ / pointsPerSigma

so raising the resolution at fixed N *shrinks the box*. Refining that way
makes the result better near contact and worse at low Q, and a convergence
test done that way measures nothing. Scale both together. This mistake has
been made twice in this project and caught both times only by a plot.

**Every precision claim in this documentation was checked against that rule.**
The transform timings and accuracies, the first-order convergence of type 1,
the stability of the low-q extrapolation: all compare grids at
r_max = 40.95–40.96 σ, varying only the density. The one comparison that does
vary the range — 41 σ to 655 σ at fixed resolution, which changed S(0) in the
sixth decimal — is described as such, and it is what established that the
type-1 error is a grid-alignment effect rather than truncation.

When writing a new comparison, pair `bestGridSize(N, transformType)` with a
proportionally scaled `pointsPerSigma` and the range stays fixed without
anyone having to remember it. Print `N/pointsPerSigma` if in doubt: it takes
a second and it is the check that would have caught both earlier mistakes.

**Grid size depends on the transform type**, and the difference is large:

| N | DST-I (type 1) | DST-IV (type 4) |
|---|---|---|
| 16383 = 2¹⁴−1 | **0.263 ms** | 0.308 ms |
| 16384 = 2¹⁴ | 0.599 ms | **0.099 ms** |
| 16385 | 2.497 ms | 0.303 ms |

A DST-I goes through an FFT of length 2(N+1), so it wants N = 2^k − 1; a
DST-IV is a length-N transform and wants N = 2^k. Use
`oZfixpointOperator.bestGridSize(n, transformType)` rather than choosing by
hand. Sizing both for type 1 made type 4 benchmark *slower* than type 1 for
some time, which is how the rule was found.

**Type 4 is second order** — measured 139× to 1092× more accurate than type 1
at the same grid, and faster on a power-of-two grid. It is **correct only for
one size class**: it requires every pair core to fall between grid points,
which cannot be arranged for a mixture because the σ_ij are irrational.

**How much that costs, measured absolutely.** Percus–Yevick hard spheres have
a closed form for the compressibility, S(0) = (1−φ)⁴/(1+2φ)², which is the
only exact answer available anywhere in this package. Against it:

| φ | type 1, pps=100 | type 1, pps=400 | type 4, pps=100 |
|---|---|---|---|
| 0.30 | 3.67% | 0.92% | **0.015%** |
| 0.40 | 5.27% | 1.33% | **0.036%** |

The cause is where the discontinuity lands. A DST-I places nodes at (n+1)Δr,
so the hard core at r = σ falls **on** a node and is carried by a point that
is neither inside nor outside; the excluded volume is then wrong at first
order. A DST-IV places them at (n+½)Δr, the core falls **between** points,
and the step is resolved to second order.

Two things make this worth knowing rather than filing under "type 4 is
better". The error is **independent of the real-space range** — extending
r_max from 41σ to 655σ changed S(0) in the sixth decimal — so the usual
instinct of giving the solver more room does nothing. And refining the
resolution *does* reduce it, by a clean factor of 3.96 per fourfold step, so
a convergence study sees orderly first-order behaviour and concludes the
method is working. It is, slowly, towards an answer whose leading error is
set by grid alignment.

S(0) is the isothermal compressibility, so this is a systematic bias in
exactly the quantity the volume fraction controls: **a fit absorbs it by
moving φ**, and the denser the sample the more it moves. For one component,
use type 4. For a mixture there is no such option, and a dense polydisperse
fit should use the finest affordable resolution with its fitted volume
fraction read accordingly.

`tools/numerics_test.py` asserts all of this, and
`tools/validation_table_test.py` reports it per transform and per resolution.

---

## `solveWithConsensus()`

```python
ozLib.solveWithConsensus(potential, potentialArgs=(),
                         closure="doPYclosure", closureParam=None,
                         volumeDensity=0.3,
                         solvers=("scipy Anderson", "Biggs-Andrews",
                                  "Picard iteration"),
                         tolerance=1e-3,
                         gridN=4095, pointsPerSigma=100,
                         maxIterations=8000, transformType=1,
                         quantity="Sq")
```

Runs several solvers and reports whether they agree. Use it when a state
point is suspect, or when a result matters enough to want more than one
route to it. Note it spells its arguments `gridN`/`pointsPerSigma`, unlike
`solve()`.

---

## Worked examples

### A single curve

```python
import ozLib
res = ozLib.solve("StickyHardSphere", phi=0.3, potentialArgs=(0.2, 0.02))
q, S = res.q, res.Sq
```

### An attractive square well, at a chosen resolution

```python
from oZfixpointOperator import bestGridSize
res = ozLib.solve("SquareWell", phi=0.3,
                  potentialArgs=(-1.0, 0.1),      # NEGATIVE = attractive
                  closure="Hypernetted-Chain",
                  numberOfRadialSamplingPoints=bestGridSize(16383, 1),
                  hardSphereDiameterInPoints=400)
```

### Solving α by thermodynamic consistency

```python
res = ozLib.solve("HardSphere", phi=0.4, closure="Rogers-Young",
                  findConsistentParameter=True)
print(res.closureParam)          # the α that made the two routes agree
```

### Checking convergence properly

```python
from oZfixpointOperator import bestGridSize
for pps, k in ((100, 12), (400, 14)):        # r_max held CONSTANT
    res = ozLib.solve("HardSphere", phi=0.3,
                      numberOfRadialSamplingPoints=bestGridSize(2**k - 1, 1),
                      hardSphereDiameterInPoints=pps)
```

A fourfold refinement should reduce a first-order error fourfold. A ratio
near 1 means the discrepancy is not discretisation, and something else is
wrong — that is how a defect giving every pair in a mixture the same hard
core was eventually found.

### Damped Picard for a hard state point

```python
def damp(sol):
    sol.mannAlpha = 0.5
res = ozLib.solve("LennardJones", phi=0.4, potentialArgs=(1.0,),
                  solver="Picard iteration", onSolverCreated=damp)
```

---

## Things that will bite

| symptom | cause |
|---|---|
| a converged fit with min S(Q) < 0 | a real root on an unphysical branch — screen on S(Q) ≥ 0 |
| `TypeError` on unexpected keyword | `gridN` vs `numberOfRadialSamplingPoints`; the layers differ |
| a "square well" that repels | positive ε is a shoulder; attraction needs ε < 0 |
| error grows under refinement | N held fixed while resolution rose; the box shrank |
| type 4 slower than type 1 | grid sized 2^k−1 instead of 2^k |
| solver reports success, answer is wrong | KINSOL can report success after diverging; keep `verify=True` |

---

## Where to look next

- `tools/two_yukawa_test.py` — a worked validation against a published
  reference, showing the convergence-ratio pattern
- `tools/gui_path_test.py` — exercises the GUI's own compute and fit paths
- `docs/ozGUI_ozLib_report.tex` — the full report, including the derivations
- `oZfixpointOperator.py` — the transform, the grid rules, `bestGridSize`
