# `multicomponentUnitTest.py` — coverage and current results

Consolidates the ad-hoc verification scripts written while building the
multicomponent / polydisperse extension into one `unittest` suite, following
this directory's existing convention (`algorithmUnitTest.py`,
`rycUnitTest.py`, `andersonGeneralizedOZsolverUT.py`).

```
python -m unittest multicomponentUnitTest -v               # everything
python -m unittest multicomponentUnitTest.MomentMatching   # a fast subset
```

**7 classes, 23 tests.** Every test corresponds to a defect that was actually
found during development, and nearly all of those defects returned *plausible
numbers* rather than crashing — which is precisely why the suite is worth
having. None would have been caught by "does it run without raising".

## Results

| class | tests | result | time |
|---|---|---|---|
| `RegressionOneComponent` | 2 | **pass** | 1.1 s |
| `MomentMatching` | 2 | **pass** | <0.1 s |
| `ChargeScaling` | 3 | **pass** | 0.3 s |
| `PotentialByName` | 2 | **pass** | 0.1 s |
| `RobertusSHS` | 5 | **pass** | 0.3 s |
| `Multicomponent` | 5 | **pass** | 180 s |
| `RYWrapper` | 4 | **3 pass, 1 error** | 196 s |

The 14 fast tests run in **1.45 s**; the two slow classes need full OZ solves.

## The one failure is a real, unfixed bug

`RYWrapper.test_dilute_limit_and_symmetry` fails at phi = 1e-5:

```
RuntimeError: RY solve failed with ScipyAndersonOZsolver at phi=1e-05,
z=2, K=0, alpha=0.5: the solver produced no fixpoint
```

The physics is fine — **Picard and Anderson both solve this state**
(residuals 0.0 and 6.8e-21). It is the *default* solver, scipy Anderson, that
fails on this near-trivial case, apparently because the solution is so close
to gamma = 0 that its relative convergence criterion is never satisfied.

Two things were fixed as a result, and one was not:

* **Fixed:** the wrapper used to crash with a bare `AttributeError` on
  `sol.gammaMatrixMulticomponent` when a solver failed before producing a
  fixpoint. It now raises a `RuntimeError` naming the solver and state point.
* **Not fixed:** an automatic fallback to the next candidate solver was added
  (`_solverClasses()[1:]`) but **does not engage**, and the reason has not been
  identified. The code path is present and its condition looks correct, yet
  the final error still reports the first solver. Until this is resolved, pass
  `solverClass=PicardOZsolver` explicitly for very dilute states.

The test is deliberately left failing rather than weakened or marked
`expectedFailure`: it documents a genuine robustness gap that should be fixed,
not tolerated.

## What each class protects

**`RegressionOneComponent`** — eight potential/closure combinations must stay
*bit-identical* (`places=9`) to values recorded before any multicomponent
work. Exactness is by design: p = 1 takes the original scalar code path, so
any difference means that path was disturbed. Also asserts HMSA == RY for hard
spheres, the check that originally exposed HMSA returning S(q) = 1 everywhere
(the repulsive/attractive split was never populated, so exp(-0) = 1 removed
the hard core and c(r) = 0 became an exact fixed point).

**`MomentMatching`** — Gauss-generalised-Laguerre classes reproduce the first
2p-1 Schulz moments to 10 decimals. This is what lets p = 3 classes stand in
for a continuous distribution.

**`Multicomponent`** — the reference state (g_NN 1.7368, S^M 1.7478,
independently reproduced with a separately written implementation); class
convergence stated as a *two-sided* claim (p=1->3 must change a lot, p=3->5
almost nothing — a one-sided test would also pass if the classes were being
ignored entirely); the 13-closure sweep; HMSA == RY with no attractive tail;
and that the potential split really is a (p,p,N) pair matrix.

**`ChargeScaling`** — the kappa identities that follow exactly from
kappa^2 = 4 pi L_B sum_i n_i Z_i (the *first* power of Z — screening is by the
small ions; using Z^2 gives a near-ideal gas). Exponents 0 and 1 must share a
kappa because sum_i x_i sigma_i = <sigma> = 1, and exponent 2 must raise it by
exactly sqrt(<sigma^2>) = sqrt(1.04). Also checks that explicit valences
reproduce the equivalent power law bit-for-bit, and that `screeningValences`
changes kappa while leaving the amplitude untouched.

**`PotentialByName`** — optional setter arguments must be accepted.
`setPotentialByName` used to count arguments *with defaults* as mandatory, so
`PolydisperseHardCoreYukawa` was unreachable — and the rejection was silent
(print + return), leaving the potential untouched so every closure produced
the trivial S(q) = 1. Also checks that genuinely too-few arguments are still
rejected.

**`RobertusSHS`** — guards against the placeholder engine that shipped
previously (its `solve()` was a bare `pass`; `S_matrix` filled every element
from a single-component formula, so classes were never coupled). The decisive
test is the dilute limit -> identity: the placeholder gave diagonal ~ 1 *and
off-diagonal ~ 1*. Also: lambda residual < 1e-9; I_exact independent of class
count (the placeholder inflated it by (sum_i sqrt(w_i))^2 — 8.84 at 12
classes, 18.2 at 24); and that S(q) actually varies monotonically with
distribution width, which was the reported symptom.

**`RYWrapper`** — dilute limit and symmetry; the sign convention (MSA sets
c = -beta U so K > 0 is attractive and must *raise* low-Q S); the alpha
consistency search returning 0.2166; and that an unphysical solution is
rejected. That last one matters most: a small residual is **not sufficient**,
because the closure has several fixed points and different solvers converge
cleanly onto different ones — at phi = 0.1, K = 1 two solvers gave S = 17.03
and 9.71, both with residual ~1e-12 and both with min S_NN(q) < 0. A structure
factor is a variance and cannot be negative.

## A bug found in the tests themselves

`_yukawa_solver` computed <sigma^3> via `1/srel**2`, which raised
`ZeroDivisionError` for the monodisperse case srel = 0. Fixed by
special-casing the delta-function limit — worth recording because it was a
defect in the test, not the code, and briefly looked like a failure of the
p-convergence claim.
