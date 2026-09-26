# API reference

Generated from the source, so the signatures and docstrings here are what the
code actually does. Where this disagrees with the {doc}`manual`, this page is
right and the manual needs fixing.

## ozLib

The programmatic entry point. `solve()` is what the GUI's Calculate button
calls.

```{eval-rst}
.. automodule:: ozLib
   :members: solve, solveWithConsensus, multicomponentCapableClosures,
             isMulticomponentPotential, secondClosureParam
   :undoc-members:
   :no-index:
```

### Result object

Curves are plain arrays on the solver's own grid, not callables: `res.Sq`
against `res.q`, `res.gr` against `res.r`.

```{eval-rst}
.. autoclass:: ozLib.OZResult
   :members:
```

### Name tables

These are the authority for what is available. The tables in the manual are a
convenience and will lag behind them.

Inspect them directly rather than trusting a transcription:

```python
import ozLib
print(sorted(ozLib.CLOSURE_SETTERS))              # 24 closures
print(sorted(ozLib.SOLVER_CLASSES))               # 5 solvers
print(ozLib.CURVE_NAMES)                          # 9 curves
print(sorted(ozLib.CONSISTENT_PARAMETER_CLOSURES))
print(ozLib.SECOND_CLOSURE_PARAM)
```

They are deliberately NOT rendered by `autodata` here. Each is preceded in
the source by a block of explanatory comments written for a programmer
reading the file, and `docutils` reads those as reStructuredText -- indented
continuations become block quotes, quoted words become malformed emphasis,
and a dozen warnings result. The prose is worth more where it is than
reformatted to satisfy a parser it was never written for, and the values are
better inspected live in any case: a printed list cannot go stale.

## The transform and the grid

`bestGridSize` matters more than its size suggests: a DST-I goes through an
FFT of length 2(N+1) and wants N = 2^k − 1, while a DST-IV is a length-N
transform and wants N = 2^k. Choosing by hand made the second-order transform
benchmark *slower* than the first-order one for some time.

```{eval-rst}
.. automodule:: oZfixpointOperator
   :members: bestGridSize
```

## Polydisperse size classes

The quadrature nodes are moment-matched, which is why three classes suffice
where naive binning needs dozens.

`sizeClasses` and `quantileClasses` are **not** interchangeable: the first is
the generalised Gauss–Laguerre rule and reproduces the first 2p−1 moments
exactly, the second is quantile-based and reproduces them only
approximately. The solver uses the second; anything claiming exact moments
must use the first.

```{eval-rst}
.. automodule:: polydisperse_nodes
   :members: sizeClasses, quantileClasses, crossCheckMoments, analyticMoments,
             DISTRIBUTIONS
```

## The polydisperse model

`GenericPolydisperseSAS` solves the multicomponent Ornstein–Zernike equations
for a size distribution and forms the scattered intensity from them. Its
constructor docstring carries the two settings most easily got wrong: the
grid and the resolution must be **scaled together** when refining, since
r_max = N·σ/pointsPerSigma, and the choice of transform type is worth more
than it looks — type 1 places grid points on the hard core and is first
order, type 4 places them between and is second, a factor of 148 in S(0) at
φ = 0.40.

```{eval-rst}
.. autoclass:: generic_polydisperse_sas.GenericPolydisperseSAS
   :members: I_exact, S_partials, I_monodisperse, I_decoupling, I_lma,
             I_partial_sf, I_scaling, I_vdw1
   :special-members: __init__
```

## Fitting measured data

```{eval-rst}
.. autoclass:: polydisperse_fit.Resolution
   :members:

.. autoclass:: polydisperse_fit.PolydisperseFit
   :members: run, runMultiStart
   :special-members: __init__

.. autofunction:: polydisperse_fit.fitWithBumps

.. autofunction:: polydisperse_fit.fitWithNLopt
```

The linear coefficients — scale, background and the power-law amplitude —
are solved exactly by weighted least squares at every iteration rather than
handed to the optimiser. That solve equilibrates its columns first, which is
not a refinement: at a condition number of 10²⁰, which SLDs in cm⁻² produce,
an unequilibrated `lstsq` returns two of the three coefficients as exactly
zero and reports success.

```{eval-rst}
.. autofunction:: polydisperse_fit._linearScaleAndBackground
```
