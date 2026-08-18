# -*- coding: utf-8 -*-
'''
OZ solver using SUNDIALS' own KINSOL Anderson-accelerated fixed-point
strategy (KIN_FP), via the official Python bindings (pip install
sundials4py -- https://pypi.org/project/sundials4py/). A separate
class from sundials4pyKinsolOZsolver.py (the Newton-Krylov one)
because KIN_FP genuinely needs a different sysfn convention, not just
a different KINSol() argument.

Matches sasfit_oz_solver.c's own "case KINSOLFP" block exactly (see
that file's OZ_step_kinsolFP() and KIN_sasfit_configure()):
  - sysfn must return G(x) (the fixed-point map) DIRECTLY, not the
    residual G(x)-x used everywhere else in this project's KINSOL-
    based solver -- found by testing (see the conversation this was
    built in) that using self.rootOperator here, matching the Newton-
    Krylov solver's own convention, either fails to converge or
    converges to a value that is not even a fixed point of the map;
    self.fixPointOperator directly is the correct one, confirmed by
    exact agreement (to 1e-12) against the existing, independently-
    validated Newton-Krylov solver.
  - KINSetMAA (Anderson subspace size) is set, matching the C code.
  - No damping (KINSetDampingAA) is set, ALSO matching the C code
    exactly -- SASfit's own KIN_sasfit_configure() never calls that
    function either, relying entirely on KINSOL's own default. Found
    by testing: an artificial, adversarial toy problem (Newton's
    iteration map for sqrt(2)) does genuinely diverge without damping,
    but this tool's own actual OZ fixed-point maps (tested on both
    HardSphere+PY and StickyHardSphere+MSA) do not -- so this
    divergence risk is a property of how ill-conditioned a given
    fixed-point map's residual history becomes near convergence, not
    a blanket requirement; matching SASfit's own choice (no damping)
    is the right default here, since the physical OZ maps this tool
    solves are evidently well-behaved enough for it, exactly as
    SASfit's own long production experience already demonstrates.
'''
import numpy as np
import sundials4py.core as core
import sundials4py.kinsol as kinsol

#Base class
from oZsolver import OZsolver

class Sundials4pyKinsolFPOZsolver(OZsolver):
    def __init__(self, port, **kwargs):
      OZsolver.__init__(self, port, **kwargs)
      #Anderson subspace size -- matches OZd->KINSetMAA's role in the
      #C code (a user-adjustable value there too, not a fixed constant).
      #5 here matches the value shown in SASfit's own "configure OZ
      #solver" dialog (shared directly by the user), not this file's
      #own earlier guess of 3.
      self.KINSetMAA = 5

    def solve(self):
      n = self.x_0.size
      err, sunctx = core.SUNContext_Create(core.SUN_COMM_NULL)

      def sysfn(uu, fval, user_data):
          x = core.N_VGetArrayPointer(uu)
          f = core.N_VGetArrayPointer(fval)
          if self.isInterrupted:
              #Known limitation, found by testing: unlike the Newton-
              #Krylov solver (sundials4pyKinsolOZsolver.py), KIN_FP does
              #not seem to treat a recoverable-error return (1) from
              #sysfn as a reason to actually stop -- the solve still
              #reports KIN_SUCCESS. Returning an unrecoverable-error
              #code (-1) instead, hoping that would stop it cleanly, was
              #tried and found to segfault. Left as 1 (safe, just not
              #fully effective) rather than -1 (crashes) -- interrupting
              #a running KIN_FP solve from the GUI's "interrupt" button
              #may therefore not take effect as promptly as it does for
              #the other solvers here. This does not affect the
              #correctness of a solve that runs to completion normally.
              return 1
          f[:] = self.fixPointOperator(x)   #G(x) directly -- see this file's own docstring for why
          return 0

      u = core.N_VNew_Serial(n, sunctx)
      scale = core.N_VNew_Serial(n, sunctx)
      core.N_VGetArrayPointer(u)[:] = self.x_0
      core.N_VConst(1.0, scale)

      #see sundials4pyKinsolOZsolver.py's own comment on why no
      #KINFree/N_VDestroy/SUNContext_Free calls appear here -- same
      #reasoning applies identically to this class.
      kin_mem_view = kinsol.KINCreate(sunctx)
      kin_mem = kin_mem_view.get()

      kinsol.KINInit(kin_mem, sysfn, u)
      kinsol.KINSetMAA(kin_mem, self.KINSetMAA)
      kinsol.KINSetNumMaxIters(kin_mem, self.numberOfIterations)
      kinsol.KINSetFuncNormTol(kin_mem, self.convergenceCriterion)
      kinsol.KINSetScaledStepTol(kin_mem, self.convergenceCriterion)

      #Matching sasfit_oz_solver.c's own KINSOLFP case, which attaches
      #a plain GMRES linear solver even for KIN_FP (via KINSpgmr(kin_mem,0)
      #immediately before KINSol(...,KIN_FP,...)) -- kept here for
      #parity with that reference, though KIN_FP's own Anderson mixing
      #does not itself require Newton-style linear solves the way
      #KIN_LINESEARCH/KIN_NONE do.
      LS = core.SUNLinSol_SPGMR(u, core.SUN_PREC_NONE, 0, sunctx)
      kinsol.KINSetLinearSolver(kin_mem, LS, None)

      flag = kinsol.KINSol(kin_mem, u, kinsol.KIN_FP, scale, scale)

      if flag not in (kinsol.KIN_SUCCESS, kinsol.KIN_INITIAL_GUESS_OK):
          print("sundials4py KINSOL (KIN_FP) did not converge, flag =", flag,
                "(", kinsol.KINGetReturnFlagName(flag), ")")
          return

      err, nfe = kinsol.KINGetNumFuncEvals(kin_mem)
      print("sundials4py KINSOL (KIN_FP) converged after", nfe, "function evaluations")

      x_fp = np.array(core.N_VGetArrayPointer(u), copy=True)
      self.derivePhysicalQuantitiesFromFixpoint(x_fp)
