# -*- coding: utf-8 -*-
'''
OZ solver using SUNDIALS' own KINSOL, via its official Python bindings
(pip install sundials4py -- https://pypi.org/project/sundials4py/),
rather than scipy.optimize.newton_krylov (see
scipyNewtonKrylovOZsolver.py for that alternative).

Exposes the choice of Newton-Krylov linear solver as a plain instance
attribute (self.linearSolver), matching the original Tcl GUI's own
"algorithm" dropdown offering GMRES/FGMRES/Bi-CGStab/TFQMR -- e.g.:
    s = Sundials4pyKinsolOZsolver(port=0)
    s.linearSolver = 'SUNLinSol_SPFGMR'
    s.solve()
(not a constructor argument, to avoid changing __init__(self, port)'s
signature, which the GUI and every other caller in this project relies
on). Defaults to 'SUNLinSol_SPGMR' (plain GMRES), matching this class's
original, single-linear-solver behaviour.

Every combination below was verified directly (see the conversation
this was built in) both on a trivial scalar root-find and against this
tool's own analytical PY+HardSphere reference before being offered
here. One linear solver was deliberately left OUT after that testing:
  - SUNLinSol_SPBCGS (Bi-CGSTAB): segfaults unconditionally in this
    sundials4py version (confirmed reproducible across several maxl
    values, in total isolation from any OZ-specific code at all) --
    a real bug in this beta package, not a usage error on this file's
    part. Excluded rather than offered with a crash warning.

KINSOL's Anderson-accelerated fixed-point strategy (KIN_FP) is NOT
offered here -- it needs a genuinely different sysfn convention
(G(x) directly, not the residual G(x)-x this file uses), so it lives
in its own class, sundials4pyKinsolFPOZsolver.py. KIN_PICARD is not
offered at all: checking sasfit_oz_solver.c directly (rather than
assuming) showed that what SASfit's own GUI calls "Picard" is a
hand-written iteration scheme (matching this project's own
picardOZsolver.py), not KINSOL's KIN_PICARD strategy -- SASfit's C
code never actually calls KIN_PICARD anywhere, so there is no
reference behaviour to match it against, and it showed genuine
non-convergence in this file's own testing besides.

Tuning parameters below (KINSetMaxNewtonStep, funcNormTol/scaledStepTol,
maxRestarts, eta form) were updated to match SASfit's own C
configuration exactly (KIN_sasfit_configure() in sasfit_oz_solver.c
and its own GUI's "configure OZ solver" dialog defaults), after the
user shared that dialog's real values directly. This made a genuine,
confirmed difference: a strongly-charged DLVO+MSA case that failed to
converge (KIN_MAXITER_REACHED) under this file's own earlier, more
conservative settings (KINSetMaxNewtonStep=n, funcNormTol=scaledStepTol
=1e-12, default maxRestarts/eta form) converges cleanly under these --
confirming the earlier convergence difficulty was this file's own
under-tuned defaults, not a fundamental limit of the problem or of
sundials4py itself.
'''
import numpy as np
import sundials4py.core as core
import sundials4py.kinsol as kinsol

#Base class
from oZsolver import OZsolver

#Linear solvers confirmed working in this sundials4py version (see this
#file's own docstring for what was excluded, and why).
AVAILABLE_LINEAR_SOLVERS = ['SUNLinSol_SPGMR', 'SUNLinSol_SPFGMR', 'SUNLinSol_SPTFQMR']

#GMRES-family (restartable) solvers only -- TFQMR has no equivalent
#SetMaxRestarts function in SUNDIALS at all (Bi-CGSTAB/TFQMR are
#short-recurrence Krylov methods, not restarted the way GMRES is).
_RESTARTABLE_LINEAR_SOLVERS = {'SUNLinSol_SPGMR': core.SUNLinSol_SPGMRSetMaxRestarts,
                                'SUNLinSol_SPFGMR': core.SUNLinSol_SPFGMRSetMaxRestarts}

class Sundials4pyKinsolOZsolver(OZsolver):
    def __init__(self, port, **kwargs):
      OZsolver.__init__(self, port, **kwargs)
      #GMRES by default -- matches this class's own original,
      #single-linear-solver behaviour exactly, so existing code that
      #never touches this attribute keeps working unchanged.
      self.linearSolver = 'SUNLinSol_SPGMR'
      #SASfit's own "configure OZ solver" dialog defaults (shared by
      #the user directly from a real screenshot of that dialog) --
      #kept as separate, overridable attributes rather than reusing
      #self.convergenceCriterion (which the OTHER solver classes in
      #this project use for their own, differently-scaled convergence
      #checks), since SASfit itself uses two DIFFERENT values here
      #(FuncNormTol looser than ScaledStepTol), not one shared value.
      self.KINSetFuncNormTol = 1e-10
      self.KINSetScaledStepTol = 1e-13
      self.KINSetMaxNewtonStepFactor = 100.0   #multiplies vector size n, matching SASfit's 100*NP
      self.KINSetMaxRestarts = 10
      self.KINSetEtaForm = 'KIN_ETACHOICE1'

    def solve(self):
      if self.linearSolver not in AVAILABLE_LINEAR_SOLVERS:
          print("Sundials4pyKinsolOZsolver: unknown linearSolver '" + str(self.linearSolver)
                + "', must be one of", AVAILABLE_LINEAR_SOLVERS)
          return

      n = self.x_0.size
      err, sunctx = core.SUNContext_Create(core.SUN_COMM_NULL)

      def sysfn(uu, fval, user_data):
          x = core.N_VGetArrayPointer(uu)
          f = core.N_VGetArrayPointer(fval)
          if self.isInterrupted:
              return 1  #nonzero = unrecoverable error, stops KINSol
          f[:] = self.rootOperator(x)
          return 0

      u = core.N_VNew_Serial(n, sunctx)
      scale = core.N_VNew_Serial(n, sunctx)
      core.N_VGetArrayPointer(u)[:] = self.x_0
      core.N_VConst(1.0, scale)

      #sundials4py has no explicit KINFree/N_VDestroy/SUNContext_Free --
      #confirmed directly (none of those names exist in the module at
      #all). Object lifetimes are managed entirely by Python's own
      #garbage collector via the C++ objects' destructors (RAII), so
      #every object that must stay alive for the whole solve -- the
      #KINView itself included, not just the raw handle from .get() --
      #is kept as a local variable for the full duration of this
      #method, and none are freed explicitly.
      kin_mem_view = kinsol.KINCreate(sunctx)
      kin_mem = kin_mem_view.get()

      kinsol.KINInit(kin_mem, sysfn, u)
      kinsol.KINSetNumMaxIters(kin_mem, self.numberOfIterations)
      kinsol.KINSetFuncNormTol(kin_mem, self.KINSetFuncNormTol)
      kinsol.KINSetScaledStepTol(kin_mem, self.KINSetScaledStepTol)
      #Matching SASfit's own 100*NP exactly (its "configure OZ solver"
      #dialog showed KINSetMaxNewtonStep=204800 for NP=2048, i.e.
      #100*NP) -- this file's own earlier, more conservative choice of
      #plain n was found (see this file's own docstring) to be
      #needlessly restrictive for genuinely hard (e.g. strongly
      #charged) problems.
      kinsol.KINSetMaxNewtonStep(kin_mem, self.KINSetMaxNewtonStepFactor*n)
      kinsol.KINSetEtaForm(kin_mem, getattr(kinsol, self.KINSetEtaForm))

      linsolfunc = getattr(core, self.linearSolver)
      LS = linsolfunc(u, core.SUN_PREC_NONE, 0, sunctx)
      kinsol.KINSetLinearSolver(kin_mem, LS, None)
      if self.linearSolver in _RESTARTABLE_LINEAR_SOLVERS:
          _RESTARTABLE_LINEAR_SOLVERS[self.linearSolver](LS, self.KINSetMaxRestarts)

      flag = kinsol.KINSol(kin_mem, u, kinsol.KIN_LINESEARCH, scale, scale)

      if flag not in (kinsol.KIN_SUCCESS, kinsol.KIN_INITIAL_GUESS_OK):
          print("sundials4py KINSOL (" + self.linearSolver + ") did not converge, flag =", flag,
                "(", kinsol.KINGetReturnFlagName(flag), ")")
          return

      err, nfe = kinsol.KINGetNumFuncEvals(kin_mem)
      print("sundials4py KINSOL (" + self.linearSolver + ") converged after", nfe, "function evaluations")

      x_fp = np.array(core.N_VGetArrayPointer(u), copy=True)
      self.derivePhysicalQuantitiesFromFixpoint(x_fp)
