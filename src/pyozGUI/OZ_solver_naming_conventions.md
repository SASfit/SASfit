# Naming conventions: SASfit's Tcl OZ solver GUI vs. the Python OZ solver GUI

Compares `sasfit.vfs/lib/app-sasfit/tcl/sasfit_OZ_solver.tcl` (SASfit's built-in
Tcl/Tk "Ornstein Zernike solver" window) against `src/pyozGUI/` (`oZgui.py` +
`ozLib.py`), the Python equivalent. The Python GUI's own docstring states it is
a direct port of this same Tcl window, and several of its comments already
document specific mapping decisions — those are called out below where
relevant, rather than re-derived from scratch.

All Tcl values are read directly from `sasfit_OZ_solver.tcl` (the global `OZ`
array); all Python values are read directly from `oZgui.py`/`ozLib.py`.

## 1. Potentials (dropdown values)

The Tcl dropdown (`$w.param.potvalue`) and Python's potential list (populated
by introspecting `oZfixpointOperator.py`'s `set<Name>Potential()` methods) use
**identical display strings** for every potential both sides support:

| Tcl `OZ(potential)` value | Python potential name | Notes |
|---|---|---|
| `HardSphere` | `HardSphere` | identical |
| `StickyHardSphere` | `StickyHardSphere` | identical |
| `SquareWell` | `SquareWell` | identical |
| `PiecewiseConstant` | `PiecewiseConstant` | identical |
| `SoftSphere` | `SoftSphere` | identical |
| `Fermi` | `Fermi` | identical |
| `LennardJones` | `LennardJones` | identical |
| `Depl-Sph-Sph` | `Depl-Sph-Sph` | identical |
| `Depl-Sph-Discs` | `Depl-Sph-Discs` | identical |
| `Depl-Sph-Rods` | `Depl-Sph-Rods` | identical |
| `IonicMicrogel` | `IonicMicrogel` | identical |
| `PenetrableSphere` | `PenetrableSphere` | identical |
| `DLVO` | `DLVO` | identical |
| `DLVO Hydra` | `DLVO Hydra` | identical |
| `GGCM-n` | `GGCM-n` | identical |
| `StarPolymer (f>10)` | `StarPolymer (f>10)` | identical |
| `StarPolymer (f<10)` | `StarPolymer (f<10)` | identical |
| `HS 3Yukawa` | `HS 3Yukawa` | identical |

Python's list is generated dynamically via `getAvailablePotentialNames()`
(reflection over `oZfixpointOperator.py`), so it will automatically include
any potential added there in the future — the Tcl list above is a fixed,
hand-written `-values {...}` string that must be edited by hand to add one.

## 2. Potential parameters (per-potential argument names)

Tcl hardcodes each potential's parameter display names in the `oz_input_names`
proc, filling generic slots `OZ(p0,name)` … `OZ(p6,name)` (backed by generic
entry fields `OZ(p0)` … `OZ(p6)`, and further unused slots up to `OZ(p15)` for
result-history storage only). Python instead introspects each
`set<Name>Potential(self, name1, name2, ...)` method's actual Python argument
names via `inspect.getfullargspec` — so the labels shown are whatever the
Python method signature calls its parameters, not a separately-maintained
display string.

| Potential | Tcl slot → display name | Python argument name (from `oZfixpointOperator.py`) |
|---|---|---|
| HardSphere | p0 → `diameter` | (see that potential's `setHardSpherePotential` signature) |
| StickyHardSphere | p0 → `diameter`, p1 → `tau`, p2 → `delta` | same 3 args, in that order |
| SquareWell | p0 → `diameter`, p1 → `epsilon`, p2 → `delta` | same 3 args |
| PiecewiseConstant | p0 → `diameter`, p1 → `epsilon1`, p2 → `delta1`, p3 → `epsilon2`, p4 → `delta2`, p5 → `epsilon3`, p6 → `delta3` | same 7 args |
| SoftSphere | p0 → `diameter`, p1 → `epsilon`, p2 → `"stiffness n"` | Tcl's p2 label has an embedded space + word "stiffness"; confirm the actual Python arg is literally `n` or something more descriptive |
| Fermi | p0 → `diameter`, p1 → `epsilon`, p2 → `xi` | same 3 args |
| LennardJones | p0 → `diameter`, p1 → `epsilon` | same 2 args |
| Depl-Sph-Sph | p0 → `"diam. (large)"`, p1 → `"diam. (small)"`, p2 → `"n (numb. dens.)"` | Tcl labels are abbreviated/descriptive prose, not bare identifiers — Python's introspected names are presumably plainer (e.g. `diameterLarge`/`diameterSmall`/`numberDensity`-style); worth confirming directly against that one method's signature |
| Depl-Sph-Discs | p0 → `"diam. (sphere)"`, p1 → `"diam. (disc)"`, p2 → `"n (numb. dens.)"` | same caveat as above |
| Depl-Sph-Rods | p0 → `"diam. (sphere)"`, p1 → `"length (rods)"`, p2 → `"n (numb. dens.)"` | same caveat |
| IonicMicrogel | p0 → `diameter`, p1 → `Z`, p2 → `ed`, p3 → `kpi`, p4 → `epsilon` | same 5 args |
| PenetrableSphere | p0 → `diameter`, p1 → `epsilon` | same 2 args |
| DLVO | p0 → `diameter`, p1 → `kappa`, p2 → `Z`, p3 → `LB`, p4 → `A` | same 5 args |
| DLVO Hydra | p0 → `diameter`, p1 → `kappa`, p2 → `Z`, p3 → `LB`, p4 → `A`, p5 → `gHy`, p6 → `DH` | same 7 args |
| GGCM-n | p0 → `diameter`, p1 → `epsilon`, p2 → `n`, p3 → `alpha` | same 4 args |
| StarPolymer (f>10) / (f<10) | p0 → `diameter`, p1 → `functionality` | same 2 args |
| HS 3Yukawa | p0 → `diameter`, p1 → `K1`, p2 → `lambda1`, p3 → `K2`, p4 → `lambda2`, p5 → `K3`, p6 → `lambda3` | same 6 args |

**Not independently re-verified from `oZfixpointOperator.py`'s actual source
in this pass** (that file is ~62 KB; only `oZgui.py`/`ozLib.py`/the Tcl file
were read in full) — the three rows flagged above (SoftSphere, Depl-Sph-Sph,
Depl-Sph-Discs) are exactly where Tcl's display string is descriptive prose
rather than a bare identifier, so they're the most likely spots for the
actual Python argument name to differ in spelling even though the *physical
quantity* is the same. Worth a direct check against that file if exact
argument-name fidelity matters.

## 3. Closures (dropdown values)

This is the biggest naming divergence between the two GUIs — Python's list is
both **renamed** (fuller, less abbreviated names) and **larger** (has closures
Tcl's dropdown doesn't offer, and omits one Tcl offers that was deliberately
dropped).

| Tcl `OZ(closure)` value | Python closure name | Relationship |
|---|---|---|
| `Percus-Yevick` | `Percus-Yevick` | identical |
| `Hypernetted-Chain` | `Hypernetted-Chain` | identical |
| `Reference HNC` | `Reference HNC` | identical (both "special": own orchestration method) |
| `modified HNC` | `Modified HNC` | capitalization differs ("modified" vs "Modified") |
| `PLHNC` | *(not offered)* | present in Tcl only |
| `MSA` | `MSA` | identical |
| `RMSA` | `Rescaled MSA` | **renamed** — Tcl abbreviates, Python spells it out (both "special": own orchestration method `solveRMSA()`) |
| `mMSA` | `Modified MSA` | **renamed** — Tcl abbreviates, Python spells out |
| `SMSA` | `Symmetric MSA` | **renamed** — Tcl abbreviates, Python spells out |
| `HMSA` | `HMSA` | identical |
| `Rogers-Young` | `Rogers-Young` | identical |
| `Verlet` | `Verlet` | identical |
| `MS` | `Martynov-Sarkisov` | **renamed** — Tcl abbreviates to initials, Python spells out |
| `DH` | `Duh-Haymet` | **renamed** — Tcl abbreviates to initials, Python spells out |
| `Vompe-Martynov` | `Vompe-Martynov` | identical |
| `BB` | `BB` | identical (kept as initials on both sides) |
| `BPGG` | `BPGG` | identical (kept as initials on both sides) |
| `CJVM` | `CJVM` | identical (kept as initials on both sides) |
| `Choudhury-Gosh` | `Choudhury-Ghosh` | **spelling differs** — Tcl: "Gosh", Python: "Ghosh" (the latter matches the standard transliteration of the author's name) |
| *(not offered)* | `Kovalenko-Hirata` | Python only — added after checking SASfit's own `sasfit_oz.h` enum directly (per `ozLib.py`'s own comment) |
| *(not offered)* | `ZSEP` | Python only — Lee (1995) hard-sphere zero-separation closure; "special": self-fits its own 3 parameters via `fitZSEPparameters()` |
| *(not offered — deliberately excluded)* | *(not offered — deliberately excluded)* | **Euler-Rahman/EuRah**: exists as a method on both sides' underlying solver classes, but is deliberately excluded from *both* GUIs' dropdowns — Tcl doesn't list it either, and Python's own `ozLib.py` documents an extensive, specific reason (a genuine exponential positive-feedback instability that defeated every solver strategy tried) |

## 4. Closure parameters

| Concept | Tcl name | Python name |
|---|---|---|
| Closure's extra scalar parameter (Rogers-Young's α, Modified HNC's η, etc.) | shown via combined label `"α / η:"` is actually the **Python** GUI's own label (see `oZgui.py`) — Tcl doesn't appear to expose a labelled α/η entry in the excerpt read here | `self.closureParamVar`, labelled `"α / η:"` |
| Automatic thermodynamic-consistency search for that parameter | *(not found in the Tcl file read here — may not exist in the Tcl GUI)* | `self.findConsistentVar` checkbox, "find thermodynamically consistent value", driving `findConsistentParameter=` in `ozLib.solve()` |
| Which closures support that search | *(n/a)* | `CONSISTENT_PARAMETER_CLOSURES = {"Rogers-Young": "RY", "HMSA": "HMSA", "Modified HNC": "MHNC", "BPGG": "BPGG", "CJVM": "CJVM", "BB": "BB"}` — note the dict *values* here are yet a **third** naming style (short codes distinct from both the Tcl abbreviations and Python's own spelled-out closure names), matching `OZsolver.findThermodynamicallyConsistentParameter()`'s own argument convention |

## 5. Solver / algorithm names

Tcl's algorithm list (`configOZalgorithm`'s `$w.algorithmvalue` combobox) is
long and includes many fixed-point iteration variants; Python's
`SOLVER_CLASSES` dict is shorter, using different naming entirely (grouped by
underlying library rather than by mathematical iteration scheme name), and
none of the two lists' strings match verbatim:

| Tcl `OZ(algorithm)` value | Closest Python `solver=` equivalent | Notes |
|---|---|---|
| `Picard iteration` | `Picard iteration` | identical string — the one exact match |
| `Mann iteration` | *(not offered)* | Tcl only |
| `Ishikawa iteration` | *(not offered)* | Tcl only |
| `Noor iteration` | *(not offered)* | Tcl only |
| `SP iteration` | *(not offered)* | Tcl only |
| `S iteration` | *(not offered)* | Tcl only |
| `CR iteration` | *(not offered)* | Tcl only |
| `Picard-S iteration` | *(not offered)* | Tcl only |
| `PMH iteration` | *(not offered)* | Tcl only |
| `Mann II iteration` | *(not offered)* | Tcl only |
| `Krasnoselskij iteration` | *(not offered)* | Tcl only |
| `S* iteration` | *(not offered)* | Tcl only |
| `dNewton` | *(not offered directly — closest concept)* `scipy Newton-Krylov` | not a naming match, just the nearest equivalent solver family |
| `Hybrid` | *(no direct equivalent found)* | |
| `Hybrids (int. sc.)` | *(no direct equivalent found)* | |
| `Broyden` | *(no direct equivalent found)* | |
| `Biggs_Andrews` | `Biggs-Andrews` | **punctuation differs**: underscore vs hyphen |
| `Anderson mixing` | `Anderson acceleration` / `scipy Anderson` | **renamed** — Tcl's "mixing" vs Python's "acceleration"; Python additionally splits this into a hand-written version and a scipy-backed version |
| `KINSOL_FP` | `sundials4py: Fixed-Point (Anderson)` | **renamed and restructured** — Python's SUNDIALS-backed solver names are all prefixed `sundials4py:` and are conditionally available (only if that optional package is installed) |
| `GMRES` | `sundials4py: Newton-Krylov (GMRES)` | Tcl names the linear-solver method alone; Python names it as a Newton-Krylov variant parameterized by which linear solver, prefixed `sundials4py:` |
| `Bi-CGStab` | *(not offered)* | Tcl only |
| `TFQMR` | `sundials4py: Newton-Krylov (TFQMR)` | same restructuring as GMRES above |
| `FGMRES` | `sundials4py: Newton-Krylov (FGMRES)` | same restructuring |
| *(not offered)* | `scipy Newton-Krylov` | Python only |

Python's default solver (`sundials4py: Fixed-Point (Anderson)`, falling back
to `scipy Anderson` if that optional dependency isn't installed) has no
equivalent "this is the pre-selected default" concept visible in the Tcl
excerpt read here.

## 6. KINSOL tuning parameters

Tcl exposes SUNDIALS' KINSOL tuning knobs directly by their **literal C API
names** (`configOZalgorithm`'s sub-dialog) — these are the only names in
either file that are effectively identical to the underlying C library's own
API, not a GUI-specific relabeling:

| Tcl `OZ(...)` name | Meaning | Python equivalent |
|---|---|---|
| `KINSetMAA` | Anderson acceleration subspace size | *(not directly exposed in `oZgui.py`'s own widget set read here — likely set internally by `Sundials4pyKinsolOZsolver`/`Sundials4pyKinsolFPOZsolver`, not user-facing in the Python GUI)* |
| `KINSetFuncNormTol` | function-norm convergence tolerance | *(not directly exposed — Python exposes `self.convergenceCriterion` per `oZgui.py`'s own docstring, at a simpler/higher level)* |
| `KINSetScaledSteptol` | scaled-step convergence tolerance | *(not directly exposed)* |
| `KINSetNumMaxIters` | max iterations | Python: `self.maxIterVar` ("Max iterations:"), passed as `maxIterations=` |
| `KINSetPrintLevel` | KINSOL's own verbosity level | *(not directly exposed — Python routes all diagnostic output through its Log tab instead, see `oZgui.py`'s own `_TkTextRedirector`)* |
| `KINSetEtaForm` | forcing-term strategy (`KIN_ETACHOICE1`/`2`/`KIN_ETACONSTANT`) | *(not directly exposed)* |
| `KINSpilsSetMaxRestarts` | GMRES restart count | *(not directly exposed)* |
| `KINSolStrategy` | `KIN_NONE`/`KIN_LINESEARCH` | *(not directly exposed)* |
| `KINSetMaxNewtonStep` | max Newton step size | *(not directly exposed)* |

`oZgui.py`'s own docstring explicitly documents this gap as an intentional
simplification, not an oversight: *"the detailed KINSOL tuning sub-dialog
(KINSetMAA, KINSetEtaForm, etc.) — the solver classes here mostly use each
library's own sensible defaults; `self.numberOfIterations`/
`self.convergenceCriterion` are exposed instead, covering the same role at a
simpler level."*

## 7. General solver / grid parameters

| Concept | Tcl name | Python name |
|---|---|---|
| Volume fraction | `OZ(phi)`, label "volume fraction:" | `self.phiVar`, label "Volume fraction φ:" |
| Temperature | `OZ(T)`, label "temperature [K]:" | *(not found as a separate widget in `oZgui.py`'s excerpt — temperature may be folded into specific potentials' own parameters, e.g. DLVO's Bjerrum length `LB`, rather than a standalone global field)* |
| Grid size multiplier | `OZ(mult)`, label "gridsize (n x 128), n:" | closest Python equivalent: `self.gridPointsVar` ("Grid points (N):") → `numberOfRadialSamplingPoints` — **different convention**: Tcl's is a multiplier of 128-point blocks, Python's is the total point count directly |
| Points per hard-sphere diameter | *(not found in the Tcl excerpt read here as a separate control)* | `self.pointsPerSigmaVar` ("Points per σ:") → `hardSphereDiameterInPoints` |
| Mixing strategy | `OZ(mixstrategy)`, values `"mixing parameter (const)"` / `"mixing parameter (err)"` / `"mixing parameter (reward/penalty)"` | *(no equivalent found — Python's solver choice itself, e.g. plain Picard vs. Anderson-accelerated, replaces this concept rather than offering a separate mixing-strategy selector)* |
| Mixing parameter value | `OZ(mix)`, label "mixing parameter (const):" | *(no directly corresponding widget found)* |
| Max iterations | `OZ(maxit)`, label "max iterations:" | `self.maxIterVar`, label "Max iterations:" — same concept, same effective label wording |
| Iteration precision | `OZ(releps)`, label "iteration precision:" | *(no directly corresponding widget found — likely absorbed into each solver class's own internal convergence handling)* |
| Relative grid step width | `OZ(dr/dsigma)`, label "rel. grid step width:" | same underlying concept as `hardSphereDiameterInPoints` above, inverted (points-per-diameter vs. step-width-per-diameter) |
| Run/curve-set label | `OZ(label)`, label "label:" | `self.labelVar`, label "Label:" — same concept, same wording |
| Minimum OZ grid dimension | `OZ(mindimOZ)` | *(no equivalent found)* |
| Grid length (internal, stored per result) | `OZ(gridlength)` | *(no directly corresponding named field — Python's grid size is fully determined by `numberOfRadialSamplingPoints`/`hardSphereDiameterInPoints` instead)* |

## 8. Output curves / plot tabs

Both GUIs show the same 9 physical curves, but structure the names
differently: Tcl uses one flat, abbreviated variable per curve; Python uses a
short internal dict key plus a separate, longer human-readable tab label.

| Physical quantity | Tcl variable(s) | Tcl tab label | Python internal key | Python tab label |
|---|---|---|---|---|
| Structure factor | `OZ(res,s,x)`/`OZ(res,s,y)`, result arrays `OZ(result,q)`/`OZ(result,Sq)` | "structure\nfactor\nS(Q)" | `Sq` | `S(Qsigma)` |
| Radial distribution function | `OZ(res,g,x)`/`OZ(res,g,y)`, `OZ(result,r)`/`OZ(result,gr)` | "radial\ndistribution\nfunction g(r)" | `gr` | `g(r)` |
| Direct correlation function | `OZ(res,c,x)`/`OZ(res,c,y)`, `OZ(result,cr)` | "direct\ncorrelation\nfunction c(r)" | `cr` | `c(r)` |
| Indirect correlation function Γ(r) | `OZ(res,gamma,x)`/`OZ(res,gamma,y)`, `OZ(result,gammar)` | "indirect\ncorrelation\nfunct. gamma(r)" | `gamma` | `Gamma(r)` |
| Total correlation function | `OZ(res,h,x)`/`OZ(res,h,y)`, `OZ(result,hr)` | "total\ncorrelation\nfunct. h(r)" | `hr` | `h(r)` |
| Interaction potential / kT | `OZ(res,u,x)`/`OZ(res,u,y)`, `OZ(result,Ur)` | "interaction\npotential\nU(r)/(kB\*T)" | `Ur` | `U(r)/kT` |
| Bridge function | `OZ(res,B,x)`/`OZ(res,B,y)`, `OZ(result,Br)` | "bridge\nfunction\nB(r)" | `Br` | `B(r)` |
| Cavity function | `OZ(res,y,x)`/`OZ(res,y,y)`, `OZ(result,yr)` | "cavity\nfunction\ny(r)" | `yr` | `y(r)` |
| Mayer-f function | `OZ(res,f,x)`/`OZ(res,f,y)`, `OZ(result,fr)` | "Mayer-f\nfunction\nf(r)" | `fr` | `f(r)` |

Notes:
- Python's per-curve internal keys (`Sq`, `gr`, `cr`, `gamma`, `hr`, `Ur`,
  `Br`, `yr`, `fr`) are, character-for-character, the same short strings Tcl
  uses as the *suffix* of its own `OZ(result,<suffix>)` names (`Sq`, `gr`,
  `cr`, `gammar`→`gamma` is the one exception, `hr`, `Ur`, `Br`, `yr`, `fr`) —
  strongly suggesting Python's key choice was deliberately kept close to
  Tcl's existing `OZ(result,...)` suffixes for this specific naming layer,
  even though the *tab labels* shown to the user were rewritten.
- Tcl additionally keeps `gr2`/`gr3`/`gr4` (`OZ(result,gr2)` etc.) — used for
  a coordination-number / cluster-size-fluctuation readout on mouse-hover
  over the g(r) graph (see `StartOZsolver`'s and the g(r) tab's own
  `<Motion>` binding) — with no equivalent found in `oZgui.py` (its own
  docstring lists "crosshair coordinate readout" as explicitly out of scope
  for this port, covered instead by matplotlib's built-in toolbar).

## 9. General naming-convention patterns observed

- **Tcl**: a single global associative array `OZ(...)`, with comma-joined
  compound keys emulating a namespace (`OZ(result,Sq)`, `OZ(res,s,x)`,
  `OZ(p0,name)`) — idiomatic Tcl, but means every piece of state shares one
  flat global namespace.
- **Python**: an `OZgui` instance's own attributes (`self.phiVar`,
  `self.solverVar`, ...) for widget-bound state, plus a separate `OZResult`
  object (from `ozLib.py`) carrying the actual computed data
  (`result.curves["gr"]`, `result.r`, `result.q`, ...) — ordinary
  object-oriented encapsulation instead of one flat global array.
- **Abbreviation style**: Tcl consistently prefers short, sometimes
  cryptic abbreviations matching the underlying SUNDIALS/mathematical
  literature exactly (`RMSA`, `mMSA`, `MS`, `DH`, `KINSetMAA`); Python
  consistently prefers spelled-out, more self-explanatory names for anything
  user-facing (`Rescaled MSA`, `Modified MSA`, `Martynov-Sarkisov`,
  `Duh-Haymet`), while still using terse internal dict keys (`gr`, `cr`,
  `Sq`, ...) at the data-storage layer — i.e. the *renaming* happens
  specifically at the human-facing label layer, not throughout.
- **Widget label wording**: where a label exists on both sides for the same
  concept, the wording is usually close paraphrase rather than identical
  text (Tcl "volume fraction:" vs. Python "Volume fraction φ:"; Tcl "max
  iterations:" vs. Python "Max iterations:" — this one pair is an exact
  match).

## What this table does not cover

- `oZfixpointOperator.py` (the file that actually defines every
  `set<Name>Potential()` method's real Python argument names) was not read
  in full during this pass — Section 2's Python-side argument names are
  taken from the Tcl display strings and the Python GUI's own introspection
  *mechanism*, not confirmed character-for-character against that file's
  source for every potential. The three rows flagged there (SoftSphere,
  Depl-Sph-Sph, Depl-Sph-Discs) are the ones most likely to need a direct
  check.
- Only `oZgui.py` and `ozLib.py` were read on the Python side; the
  individual solver classes (`oZsolver.py`, `oZfixpointOperator.py`,
  `andersonOZsolver.py`, etc., all substantially larger files) may contain
  additional internal naming worth comparing if a more exhaustive
  cross-reference is wanted later.
- The Tcl file's `configOZalgorithm` sub-dialog and its widget internals
  (beyond the tuning-parameter names in Section 6) were not otherwise
  cross-examined against Python's corresponding solver classes' own
  constructor/attribute names.
