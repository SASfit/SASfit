# pyozgui / ozLib

A solver for the multicomponent Ornstein–Zernike equations, with polydisperse
particle form factors, a graphical interface, and a programmatic API.

The GUI's Calculate button calls {func}`ozLib.solve`, so anything the
interface can compute is reachable from a script through the same function —
a figure made in the GUI can be reproduced in a batch job without
reimplementing anything.

```{toctree}
:maxdepth: 2
:caption: Contents

manual
gui
generic_polydisperse_tab
api
```

## Where to start

- **{doc}`manual`** — the quick reference, worked examples, and the traps that
  have actually caught people here: sign conventions, the real-space range,
  branch selection, grid sizing. Most of what will save you time is on that
  page rather than in the signatures.
- **{doc}`gui`** — the tabs, session files, how to read the parameter panel,
  and which solver and fitter to choose.
- **{doc}`generic_polydisperse_tab`** — the polydisperse tab in detail: size
  classes, the six approximation schemes, and what each costs.
- **{doc}`api`** — signatures and docstrings, generated from the source so
  they cannot drift from the code.

## A note on the two halves

The API reference is generated; the manual is written. Where they disagree,
the API reference is right and the manual needs correcting — the tables in
the manual list 21 potentials and 24 closures as text, and text does not
update itself when a potential is added.

## Citing

Results obtained with the two-Yukawa reference module require citation of
Liu, Chen & Chen, *J. Chem. Phys.* **122**, 044507 (2005). That is the
condition under which its authors granted redistribution, and it is not
optional.

## Indices

- {ref}`genindex`
- {ref}`modindex`
