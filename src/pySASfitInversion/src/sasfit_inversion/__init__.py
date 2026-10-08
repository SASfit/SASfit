"""
sasfit_inversion
=================

Standalone Python reimplementation of the Fredholm-integral-of-the-first-kind
inversion solvers from SASfit (originally in src/sasfit_old/sasfit_extrapol.c),
extended with the signed/general-kernel EM variant (Chae, Martin & Walker,
2018) that was present in the earlier `pysasem` prototype but not carried
into the C code.

Design goals
------------
- No GUI code in here. This library must be fully usable headless/scriptable,
  so a thin Qt (PySide6) layer can be built on top without duplicating any
  numerics, and so a cross-validation harness (see `validate.py`, to come)
  can run without importing Qt at all.
- Background subtraction (Porod/Guinier/Zimm/OZ tail extrapolation) is a
  separate upstream concern (see the sasfit-extrapolate-background work) and
  is NOT part of this package. Solvers here expect background-subtracted
  intensities as input. The convention carried over from the C code is that
  the flat Porod background constant is added back only when reconstructing
  a fitted I(Q) curve for display -- never fed into the solve itself.
"""

__version__ = "0.0.1"
