# pyozgui

Ornstein-Zernike solver toolkit: closures, polydisperse structure factors and
small-angle scattering models, with a Tk GUI.

## Layout

```
src/pyozgui/     the library and the GUI (flat: modules import each other by
                 bare name, so the package is deliberately not sub-divided)
tests/           unit tests -- python -m pytest, or python -m unittest
docs/            documentation and the LaTeX report
examples/        runnable examples and small utilities
extern/          C sources for the compiled structure-factor libraries
archive/         one-off derivation and cross-check scripts, kept for the
                 record but not part of the package
```

## Install

```
pip install -e .            # core
pip install -e ".[full]"    # plus mpmath and sundials4py
```

## Run

```
pyozgui                     # or: python -m pyozgui.oZgui
```

## Tests

```
python -m pytest tests
```

Note that `tests/multicomponentUnitTest.py` contains one deliberately failing
test, documented in `docs/multicomponentUnitTest_results.md`: it records a
real robustness gap rather than hiding it.
