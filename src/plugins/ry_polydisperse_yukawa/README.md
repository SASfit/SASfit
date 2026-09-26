# SASfit plugin: RY polydisperse hard-core Yukawa

Drop into `src/plugins/ry_polydisperse_yukawa/`. Provides
`sasfit_ff_RYPolydisperseYukawa`, the measured structure factor S^M(q) of a
size- and charge-polydisperse charged-colloid dispersion, from a numerical
multicomponent Ornstein-Zernike solve with the Rogers-Young closure.

It is registered as a FORM FACTOR (`ff_plugins_user1`) even though what it
returns is a structure factor. That is deliberate; see "Why ff_ and not sq_"
below, and in particular the warning about attaching a size distribution.

Unlike MSA/RMSA, RY has no closed-form solution, so the coupled multicomponent
OZ equations are solved numerically. The payoff: against Monte-Carlo data
D'Aguanno & Klein find RY reproduces the excess energy to within 1% and the
excess pressure to within 3%, while RMSA "disagrees substantially" with the
same simulation data.

## Why `ff_` and not `sq_`

The function returns `ryp_SM()`, the MEASURABLE structure factor

    S^M(q) = 1 + [ sum_ij sqrt(x_i x_j) F_i(q) F_j(q) (S_ij(q) - delta_ij) ]
                 / <F^2>(q)

which already contains the form-factor weighting and is normalised by
`<F^2>`. Offered as an `sq_`, SASfit forms `I = FF(q) * SQ(q)` and multiplies
a SECOND, independently parametrised form factor onto it. That is
arithmetically right only if the two use the same size distribution and the
same width -- and nothing checks it.

Set the relative width to 0.15 in one and 0.25 in the other and the result is
a plausible curve that is silently wrong: no error, no warning, and a fit
that converges happily to parameters that mean nothing.

In the form-factor slot SASfit multiplies nothing onto it, so the mismatch is
unrepresentable rather than merely discouraged. **Leave SASfit's own
structure factor at 1.**

S^M is dimensionless and of order 1, so what comes out is a shape rather than
an absolute intensity; scale and background are supplied as usual.

### Use it alone; `_f` and `_v` return 0

SASfit uses the amplitude `_f` and the volume `_v` to build the SIMPLIFIED
polydisperse structure factors -- decoupling, local monodisperse and the
rest, the same family of approximations `ozLib` computes and compares. Those
schemes assume the size average and the interaction can be separated.

This model does the exact calculation instead: the polydispersity is coupled
to the interaction through the multicomponent OZ equations, and the average
is performed inside over the `nclass` size classes of width `s_rel`. The
approximations therefore do not apply to it, and there is no amplitude to
offer them -- an interacting polydisperse system has no single F(q).

Both functions return 0, SASfit's own signal that the quantity is
unavailable. The model is meant to be used on its own: leave SASfit's
structure factor at 1 and do not attach a size distribution, since the one
it would apply is already applied.

## Relationship to the robertus_shs plugin

The two are the same physics: polydisperse interacting particles whose size
distribution is coupled to the interaction, delivered as form factors for the
same reason. It is natural to conclude they differ only by potential and
should share a solver. They should not.

Adhesive hard spheres under Percus-Yevick have Baxter's analytic
factorisation -- a closed form, no iteration. Rogers-Young with a Yukawa tail
has none, which is why this plugin carries a numerical multicomponent OZ
solve, a cache, warm starting and a 20 000-iteration budget. The difference
is not the potential but the CLOSURE, and merging the two would throw away
the analytic path, making the fast case as slow as the slow one. That speed
is what makes robertus_shs usable.

Separate plugins also keep the dependency boundary where the linking is: this
one needs GSL and SUNDIALS KINSOL, and a build without them should lose one
model rather than two. An end user sees neither directory; only a developer
does, so there is nothing to gain by co-locating them.

What the two SHOULD share is the entry-point convention -- both `ff_`, both
refusing an attached size distribution, both saying why. That is interface
rather than implementation, and it is where consistency helps.

## Files

```
ry_polydisperse_yukawa/
├── CMakeLists.txt                           links GSL + SUNDIALS KINSOL
├── interface.c                              SASFIT_PLUGIN_EXP_* registration
├── ry_polydisperse_core.c                   the solver (no SASfit types)
├── sasfit_ff_RYPolydisperseYukawa.c         model function + cache
└── include/
    ├── private.h
    ├── ry_polydisperse_core.h
    └── sasfit_ry_polydisperse_yukawa.h      Doxygen GUI metadata
```

`ry_polydisperse_core.c/.h` is free of SASfit types so it unit-tests
standalone (see `tests/`), exactly as `robertus_shs_core` is.

## Parameters

| # | name | meaning |
|---|---|---|
| p[0] | `sigma` | mean hard-sphere diameter [nm] |
| p[1] | `s_rel` | relative width of the Schulz distribution; 0 = monodisperse |
| p[2] | `phi` | volume fraction |
| p[3] | `Z` | effective valence of the particle of diameter `<sigma>` |
| p[4] | `L_B` | Bjerrum length, same units as `sigma` |
| p[5] | `alpha` | Rogers-Young mixing parameter |
| p[6] | `nclass` | number of size classes (3 is normally enough, max 12) |
| p[7] | `chargeExp` | valence scaling: 2 = constant surface charge density, 1 = linear in size, 0 = size independent |

## Method

* Schulz distribution reduced to `nclass` components by **moment matching**
  (Gauss-generalized-Laguerre via `gsl_integration_fixed_laguerre`), exact for
  the first `2*nclass-1` moments. `nclass=3` was independently confirmed
  indistinguishable from 5 up to 30% polydispersity.
* Single shared RY mixing function `f(r) = 1 - exp(-alpha r)` for all pairs.
* Matrix OZ `H = (I - C rho)^-1 C` at every q-point.
* Radial transform is a DST-I built from a **radix-2 real FFT**, so `N+1` must
  be a power of two. `N = 2^12-1` at 100 points/sigma matches the Python OZ
  solver's default grid exactly.
* Solve strategy: **three tiers, fastest first, each strictly more robust
  than the last** -- Anderson (KINSOL KIN_FP) -> Newton-Krylov (GMRES +
  linesearch) -> Picard.

## Solver tiers

| tier | robustness | speed (SUNDIALS 7) |
|---|---|---|
| 1. Anderson (KIN_FP) + Picard pre-conditioning | fails at p=5, s=0.3 | fastest: 0.25 s at p=3 |
| 2. Newton-Krylov (GMRES + `KIN_LINESEARCH`) on the residual | **converged in every regime tried** | 3-6x slower than Anderson |
| 3. Picard, undamped then damped | last resort | slow |

Measured across seven regimes on v7 (`tests/regtest.c`):

| regime | time | tier used |
|---|---|---|
| p=3 s=0.2 | 0.25 s | Anderson |
| p=1 monodisperse | 0.03 s | Anderson |
| p=5 s=0.3 | 6.85 s | Newton-Krylov |
| p=3 s=0.4 | 0.32 s | Anderson |
| p=3 s=0.2, 5x density | 0.29 s | Anderson |
| p=3 s=0.2, 0.2x density | 0.25 s | Anderson |
| p=7 s=0.25 | 4.11 s | Anderson |

Newton-Krylov is a different algorithm, not a variant: damped Newton steps on
F(u) = G(u) - u, with the Jacobian applied matrix-free through GMRES. It
converged in a strikingly constant **31-40 Newton iterations** regardless of
how hard the problem was -- that flat iteration count is what global
convergence via linesearch buys, and it is the answer to the reasonable
objection that SUNDIALS' advanced methods ought to be more stable than
Picard. They are. The cost is wall-clock: each Newton step needs many GMRES
inner iterations, each a full function evaluation.

Newton-Krylov tuning follows `KIN_sasfit_configure()` and SASfit's own
"configure OZ solver" dialog: `MaxNewtonStep = 100*n` (not `n`),
`FuncNormTol = 1e-10` and `ScaledStepTol = 1e-13` (deliberately different
from each other), `MaxRestarts = 10`, `ETACHOICE1`. pyozGUI records that
these specific values were decisive -- a strongly charged DLVO+MSA case that
failed under more conservative settings converged cleanly once they matched
SASfit's.

## SUNDIALS version

Builds and runs against **both SUNDIALS 7 and 6**, verified by actually
compiling and running the plugin against 7.1.1 (built from source) and 6.4.1,
which give identical results. SASfit ships v7 (`src/sundials7`), and
pyozGUI's binding already uses the v7 form
`core.SUNContext_Create(core.SUN_COMM_NULL)`.

The one API break that matters here is `SUNContext_Create`: v6 takes a plain
`void*` communicator, v7 takes a typed `SUNComm` with a `SUN_COMM_NULL`
sentinel (and returns `SUNErrCode`). That is handled by the
`RYP_SUNCONTEXT_CREATE` macro, switched on `SUNDIALS_VERSION_MAJOR`.
Everything else used here -- `KINCreate`/`KINInit`/`KINSetMAA`/`KINSol`,
`SUNLinSol_SPGMR` and the serial `N_Vector` API -- is unchanged between the
two versions.

Note v7 also needs `-lsundials_core` at link time, which does not exist in
v6.

Timings below are on v7, which came out roughly 30% faster than v6 for the
identical calculation.

## Validation

| check | result |
|---|---|
| DST-I vs `scipy.fft.dst(type=1)` | exact |
| full S^M(q) curve vs the validated Python implementation | **1.3e-8 relative** (the solver tolerance; the Picard path agrees to 7e-11) |
| discretisation, total density, kappa vs Python | identical to all printed digits |
| 7 parameter regimes (p=1..7, s=0..0.4, density x0.2..x5) | all converge to correct values; `tests/regtest.c` reports which tier won |
| Newton-Krylov vs Anderson, same 6 regimes | identical S^M wherever both converge (`tests/nktest.c`) |
| caching | 2000 q-points after the first cost ~0 s |
| error path | bad `phi` returns `SASFIT_RETURNVAL_ON_ERROR` (= 1.0) with a `sasfit_param_set_err` message |

Build the standalone tests:

```
gcc -O2 -std=gnu99 -I. -Iinclude -Itests/shims -o plugtest \
    tests/plugtest.c sasfit_sq_RYPolydisperseYukawa.c ry_polydisperse_core.c \
    -lgsl -lgslcblas -lsundials_kinsol -lsundials_nvecserial -lsundials_core -lm
```

(drop `-lsundials_core` for SUNDIALS 6)

## Performance

One solve, p=3, N=4095: **0.26 s cold, 0.21 s warm** on SUNDIALS 7 (0.36 / 0.27 s on v6). All further q-points are
free (cached). Four measured steps, all kept in `tests/`:

| change | time |
|---|---|
| initial (GSL LU per q-point, damped Picard) | 15.1 s |
| inline small-matrix solve + upper-triangle symmetry | 8.3 s |
| undamped Picard | 2.3 s |
| Anderson (KIN_FP) + Picard pre-conditioning | **0.45 s** |

The first version was *slower than numpy*, because it did 3.4 million GSL 3x3
`LU_decomp` calls where GSL's generality dominates the arithmetic.

## Three traps worth knowing

All three produced plausible-looking wrong answers rather than obvious
failures.

1. **Anderson diverges from a cold start on this problem.** A hand-rolled
   Anderson was *slower* than undamped Picard when damped enough to be stable
   (851 vs 677 iterations) and diverged otherwise; KINSOL's KIN_FP diverged
   too, damped or not. The fix is `RYP_PICARD_PRESTEPS` Picard steps first --
   Picard *is* contractive here -- and only then Anderson. Measured: 0
   pre-steps diverges, 20 works, 40 is the time optimum; 50 is the default for
   margin. The same instability appeared in the Python prototype, where the
   hand-written Anderson solver ran away to ~1e200 while Picard converged.

2. **KINSOL can return `KIN_SUCCESS` after diverging.** Once the RY closure's
   exponent clipping pins values at ~1e217 the internal stopping test is
   defeated and success is reported with garbage -- observed at p=5,
   s_sigma=0.3, where S^M came back as ~1e222. Both KINSOL paths therefore
   **recompute the residual after KINSol returns** and reject the result
   unless it really is small. Never trust the flag alone.

3. **`KINSetMAA` must be called BEFORE `KINInit`.** pyozGUI's
   `sundials4pyKinsolFPOZsolver.py` calls them the other way round; doing that
   in C segfaults with SUNDIALS 6.4.1, because the Anderson depth gets set
   without the corresponding arrays being allocated. That the Python binding
   tolerates it suggests the setting may simply be ignored there -- in which
   case that path is running plain fixed-point iteration rather than the
   Anderson-accelerated one it appears to request. **Worth checking on the
   Python side.**

Everything else follows pyozGUI/`sasfit_oz_solver.c`: for KIN_FP no damping
(`KINSetDampingAA` is never called by SASfit's own configure routine either),
a GMRES linear solver attached even for KIN_FP, `KINSetMAA` = 5 matching
SASfit's configure dialog, and the callback returning `G(u)` directly rather
than the residual; for Newton-Krylov the callback returns `G(u) - u`.

## Known limitations

1. **`alpha` is not determined automatically.** RY fixes it by requiring the
   compressibility and virial routes to agree, costing three further OZ solves
   per trial -- too slow inside a fit. Determine it once with the "find
   thermodynamically consistent value" option in the Python OZ solver GUI and
   keep it fixed; it is a slowly varying function of density, the same
   reasoning D'Aguanno & Klein use when holding it constant across their own
   density derivative.
2. **`ryp_SM()` interpolates linearly** between q-grid points; scanning across
   the main peak underestimates the maximum by about 0.05%. Use a spline if
   that matters.
3. **The cache is a single static, so not thread-safe.** SASfit evaluates a
   model serially over q, which matches how this is driven; make it
   `__thread` if that changes.
4. Some regimes (e.g. p=5, s=0.3) fall through to Newton-Krylov and take
   ~7 s on v7. Automatic and correct, just slower.
5. Charges are **effective** (renormalised), not bare -- see the `chargeExp`
   discussion in the header.
6. `\ingroup sq_plugins_user1` copied from `baba_ahmed`; check
   `src/plugins/groups.struct_fac.def` if a more specific category fits.

## References

B. D'Aguanno & R. Klein, J. Chem. Soc. Faraday Trans. **87**, 379 (1991)
B. D'Aguanno & R. Klein, Phys. Rev. A **46**, 7652 (1992)
F. J. Rogers & D. A. Young, Phys. Rev. A **30**, 999 (1984)
