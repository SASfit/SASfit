# -*- coding: utf-8 -*-
'''
Conventional Python library API for this project's whole OZ solver
ecosystem -- the same calculation workflow oZgui.py's own "calculate"
button runs, factored out here so it can be called directly from your
own scripts, without needing the Tkinter GUI at all:

    from ozLib import solve
    result = solve(potential='HardSphere', phi=0.3,
                    closure='Percus-Yevick',
                    solver='sundials4py: Newton-Krylov (GMRES)')
    print(result.gr, result.Sq, result.cr)

oZgui.py itself now imports SOLVER_CLASSES/CLOSURE_SETTERS and calls
solve() from this module too (see its own work() function), rather
than duplicating this logic -- so the GUI and this library always
produce identical results by construction, not by two independently-
maintained copies of the same formulas happening to agree.

See SOLVER_CLASSES and CLOSURE_SETTERS below for the exact valid
`solver=`/`closure=` argument strings. Both dicts are the project's
own canonical list (moved here from oZgui.py, which now imports them
back) -- print(list(SOLVER_CLASSES)) / print(list(CLOSURE_SETTERS))
to see the current options from a script.

Grid configurability (numberOfRadialSamplingPoints/hardSphereDiameterInPoints,
see oZfixpointOperator.py's own __init__ for the full rationale --
important for long-range potentials, where the default grid can
truncate a slowly-decaying tail before it has actually decayed to
~0) is exposed here as plain optional solve() arguments, passed
straight through to whichever solver class gets instantiated.
'''
import numpy as np

from oZsolver import OZsolver
from picardOZsolver import PicardOZsolver
from andersonOZsolver import AndersonOZsolver
from scipyAndersonOZsolver import ScipyAndersonOZsolver
from scipyNewtonKrylovOZsolver import ScipyNewtonKrylovOZsolver
from biggsAndrewsOZsolver import BiggsAndrewsOZsolver
try:
    from sundials4pyKinsolOZsolver import Sundials4pyKinsolOZsolver, AVAILABLE_LINEAR_SOLVERS
    from sundials4pyKinsolFPOZsolver import Sundials4pyKinsolFPOZsolver
    _HAVE_SUNDIALS4PY = True
except ImportError:
    _HAVE_SUNDIALS4PY = False


# Each value is (solverClass, linearSolverNameOrNone) -- the second
# element, when not None, is assigned to solverInstance.linearSolver
# right after construction (see Sundials4pyKinsolOZsolver.py's own
# AVAILABLE_LINEAR_SOLVERS for why this is a post-construction
# attribute, not a constructor argument).
SOLVER_CLASSES = {
    "Picard iteration": (PicardOZsolver, None),
    "Anderson acceleration": (AndersonOZsolver, None),
    "scipy Anderson": (ScipyAndersonOZsolver, None),
    "scipy Newton-Krylov": (ScipyNewtonKrylovOZsolver, None),
    # Hand-written (non-SUNDIALS) acceleration scheme, ported directly
    # from sasfit_oz_solver.c's own "case BIGGS_ANDREWS:" block -- see
    # biggsAndrewsOZsolver.py's own docstring for the full algorithm
    # and how its KINSetMAA attribute (extrapolation order/formula
    # choice) differs in meaning from the KINSOL solvers' own use of
    # that same name.
    "Biggs-Andrews": (BiggsAndrewsOZsolver, None),
}
if _HAVE_SUNDIALS4PY:
    # One entry per confirmed-working linear solver (matching the
    # original Tcl GUI's own "algorithm" dropdown, which offered
    # GMRES/FGMRES/Bi-CGStab/TFQMR as separate choices) -- see
    # sundials4pyKinsolOZsolver.py's own AVAILABLE_LINEAR_SOLVERS and
    # its docstring for exactly which combinations were tested and
    # which were excluded (SUNLinSol_SPBCGS segfaults unconditionally
    # in this sundials4py version; Bi-CGStab is therefore not offered
    # here either).
    SOLVER_CLASSES["sundials4py: Newton-Krylov (GMRES)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPGMR")
    SOLVER_CLASSES["sundials4py: Newton-Krylov (FGMRES)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPFGMR")
    SOLVER_CLASSES["sundials4py: Newton-Krylov (TFQMR)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPTFQMR")
    # Anderson-accelerated fixed point (KIN_FP) -- a genuinely separate
    # class (sundials4pyKinsolFPOZsolver.py), not just another
    # linearSolver value, since KIN_FP needs a different sysfn
    # convention entirely (see that file's own docstring for the full
    # story). None here (second tuple element) since this class has no
    # linearSolver attribute to set.
    SOLVER_CLASSES["sundials4py: Fixed-Point (Anderson)"] = (Sundials4pyKinsolFPOZsolver, None)

# Each value is (closure-setter method name, needsExtraScalarParam).
# Closures that take one extra scalar parameter share self.alpha's
# reused role across RY/HMSA/BPGG/CJVM/BB, and eta for MHNC -- see
# oZfixpointOperator.py's doXXXclosure() setters. Three entries are
# "special" (marked below): each needs its own orchestration method on
# OZsolver (solveRHNC()/solveRMSA()/solveEuRah()) instead of a plain
# doXXXclosure() + solve() call, since each needs a reference-system
# sub-solve first -- see oZsolver.py for exactly what each does.
CLOSURE_SETTERS = {
    "Percus-Yevick":        ("doPYclosure",    False),
    "Hypernetted-Chain":    ("doHNCclosure",   False),
    "Reference HNC":        ("doRHNCclosure",  False),   # special: solveRHNC()
    "Modified HNC":         ("doMHNCclosure",  True),
    "MSA":                  ("doMSAclosure",   False),
    "Rescaled MSA":         ("doRMSAclosure",  False),   # special: solveRMSA()
    "Modified MSA":         ("doMMSAclosure",  False),
    "Symmetric MSA":        ("doSMSAclosure",  False),
    "Rogers-Young":         ("doRYclosure",    True),
    "HMSA":                 ("doHMSAclosure",  True),
    "Verlet":                ("doVerletClosure", False),
    "Martynov-Sarkisov":    ("doMSclosure",    False),
    "Vompe-Martynov":       ("doVMclosure",    False),
    "BPGG":                  ("doBPGGclosure",  True),
    "CJVM":                  ("doCJVMclosure",  True),
    "BB":                    ("doBBclosure",    True),
    # Euler-Rahman ("Euler-Rahman"/EuRah, ported from sasfit_oz_solver.c's
    # own CEURAH machinery) is DELIBERATELY NOT offered here. Extensive
    # testing (see the conversation this decision was made in) found a
    # genuine exponential positive-feedback instability intrinsic to the
    # closure's own mathematical structure (c(r) inside the hard core
    # feeds into an exp(Gamma) term that dominates the very next c(r)
    # estimate), which defeated every numerical strategy tried --
    # Newton-Krylov, damped Picard, density continuation, dense-Jacobian
    # Newton, Levenberg-Marquardt, Broyden, df-sane, and SUNDIALS' own
    # KINSOL with this project's best-tuned settings. The underlying
    # solveEuRah()/doEuRahClosure() methods still exist on OZsolver for
    # anyone who wants to experiment with it directly, but it is
    # excluded from this dict (and therefore from both the GUI dropdown
    # and ozLib.solve()'s own closure= validation) since it does not
    # reliably converge for ordinary use.
    # Three closures found missing when checking SASfit's own
    # src/sasfit_oz/include/sasfit_oz.h enum against this list directly
    # (rather than assuming this project's existing 17 were complete) --
    # see oZfixpointOperator.py's own doKHclosure()/doDHclosure()/
    # doCGclosure() and their update_c() branches for the full story on
    # each, including a documented quirk (KH's own c(r) formula in
    # SASfit's C code is identical to HNC's, and a known, deliberate gap
    # (DH's Lennard-Jones-specific special case is not replicated).
    "Kovalenko-Hirata":      ("doKHclosure",    False),
    "Duh-Haymet":            ("doDHclosure",    False),
    "Choudhury-Ghosh":       ("doCGclosure",    False),
}

# The 9 curves every solve derives, same set oZgui.py's own plot tabs
# show (S(Q), g(r), c(r), Gamma(r), h(r), U(r)/kT, B(r), y(r), f(r)).
CURVE_NAMES = ["Sq", "gr", "cr", "gamma", "hr", "Ur", "Br", "yr", "fr"]


class OZResult:
    '''
    Everything one solve() call produced: the r/q grids, every derived
    curve, and the parameters that produced them. Curves are available
    both via .curves['gr'] and the shorthand properties below
    (.gr, .cr, .Sq, .gamma, .hr, .Ur, .Br, .yr, .fr).
    '''
    def __init__(self, r, q, potential, potentialArgs, closure, closureParam,
                 phi, solverName, curves, solverInstance):
        self.r = r
        self.q = q
        self.potential = potential
        self.potentialArgs = potentialArgs
        self.closure = closure
        self.closureParam = closureParam
        self.phi = phi
        self.solverName = solverName
        self.curves = curves
        #kept for anyone who wants lower-level access (e.g.
        #solverInstance.getHardSphereRadius(), .boltzmannOfP2Ppotential,
        #or to re-solve at a nearby state using this one as a warm start)
        #-- not required for normal use, everything routine is already
        #on this object directly.
        self.solverInstance = solverInstance

    @property
    def gr(self): return self.curves["gr"]
    @property
    def cr(self): return self.curves["cr"]
    @property
    def Sq(self): return self.curves["Sq"]
    @property
    def gamma(self): return self.curves["gamma"]
    @property
    def hr(self): return self.curves["hr"]
    @property
    def Ur(self): return self.curves["Ur"]
    @property
    def Br(self): return self.curves["Br"]
    @property
    def yr(self): return self.curves["yr"]
    @property
    def fr(self): return self.curves["fr"]

    def __repr__(self):
        return (f"OZResult(potential={self.potential!r}, potentialArgs={self.potentialArgs!r}, "
                f"closure={self.closure!r}, phi={self.phi!r}, solver={self.solverName!r})")


def solve(potential, phi, potentialArgs=(), closure="Percus-Yevick", closureParam=None,
          solver="Picard iteration", maxIterations=1000,
          numberOfRadialSamplingPoints=None, hardSphereDiameterInPoints=None,
          onSolverCreated=None):
    '''
    Run one full OZ solve and return an OZResult with every derived
    curve. This is the exact same workflow oZgui.py's own "calculate"
    button runs (that button calls this function too) -- see
    SOLVER_CLASSES/CLOSURE_SETTERS in this module for the exact list
    of valid `solver=`/`closure=` names.

    potential: potential name, e.g. 'HardSphere', 'StickyHardSphere',
        'DLVO' -- see oZfixpointOperator.py's own setXXXPotential()
        methods, or call getAvailablePotentialNames() on any solver
        instance, for the full current list.
    phi: volume fraction.
    potentialArgs: positional arguments for that potential's own
        setXXXPotential(), e.g. (tau, delta) for StickyHardSphere.
    closure: closure name, see CLOSURE_SETTERS above.
    closureParam: the closure's own alpha/eta value, required for
        closures where CLOSURE_SETTERS[closure][1] is True (Modified
        HNC, Rogers-Young, HMSA, BPGG, CJVM, BB).
    solver: solver name, see SOLVER_CLASSES above.
    maxIterations: upper bound on iterations/function evaluations.
    numberOfRadialSamplingPoints, hardSphereDiameterInPoints: optional
        grid overrides (see oZfixpointOperator.py's own __init__) --
        leave as None for this project's original grid (4096 points,
        100 points per hard-sphere diameter); increase
        numberOfRadialSamplingPoints (at fixed hardSphereDiameterInPoints)
        for a longer real-space range without losing resolution, e.g.
        for long-range/slowly-decaying potentials.
    onSolverCreated: optional callback, called with the just-constructed
        solver instance right after it is set up (potential/closure
        applied) but BEFORE solve() actually runs -- lets a caller
        (e.g. oZgui.py's own "interrupt" button) reach the live
        instance while a long solve is still in progress, since
        solve() itself only returns once the whole computation is
        already finished. Not needed for normal scripted use.

    Raises ValueError for an unrecognised closure/solver name, or a
    closure that needs closureParam when none was given -- these are
    checked here (before touching the solver) so a mistake is reported
    immediately rather than surfacing later as a confusing internal
    AttributeError.
    '''
    if closure not in CLOSURE_SETTERS:
        raise ValueError(f"unknown closure {closure!r}, must be one of {list(CLOSURE_SETTERS)}")
    if solver not in SOLVER_CLASSES:
        raise ValueError(f"unknown solver {solver!r}, must be one of {list(SOLVER_CLASSES)}")

    setterName, needsParam = CLOSURE_SETTERS[closure]
    if needsParam and closureParam is None:
        raise ValueError(f"closure {closure!r} needs a closureParam value")

    solverClass, solverLinearSolver = SOLVER_CLASSES[solver]

    gridKwargs = {}
    if numberOfRadialSamplingPoints is not None:
        gridKwargs["numberOfRadialSamplingPoints"] = numberOfRadialSamplingPoints
    if hardSphereDiameterInPoints is not None:
        gridKwargs["hardSphereDiameterInPoints"] = hardSphereDiameterInPoints

    solverInstance = solverClass(port=0, **gridKwargs)
    if solverLinearSolver is not None:
        solverInstance.linearSolver = solverLinearSolver
    solverInstance.setNumberOfIterations(maxIterations)
    solverInstance.setVolumeDensity(phi)
    solverInstance.setPotentialByName(potential, *potentialArgs)

    if onSolverCreated is not None:
        onSolverCreated(solverInstance)

    if setterName == "doRHNCclosure":
        solverInstance.solveRHNC()
    elif setterName == "doRMSAclosure":
        solverInstance.solveRMSA()
    else:
        if needsParam:
            getattr(solverInstance, setterName)(closureParam)
        else:
            getattr(solverInstance, setterName)()
        solverInstance.solve()

    curves = {}
    curves["gr"] = solverInstance.getRDF()
    curves["cr"] = solverInstance.getDirectCorrelationFunction()
    curves["Sq"] = np.real(solverInstance.getSq())
    g = curves["gr"]; c = curves["cr"]
    curves["gamma"] = g - c - 1.0
    curves["hr"] = g - 1.0
    U = -np.log(np.clip(solverInstance.boltzmannOfP2Ppotential, 1e-300, None))
    curves["Ur"] = U
    EN = solverInstance.boltzmannOfP2Ppotential
    curves["fr"] = EN - 1.0
    # cavity y(r)=g/EN outside the hard core, and bridge
    # B(r)=log(y(r))-Gamma(r) wherever y(r)>0 -- matches the original
    # Tcl GUI's own B(r)/y(r) tabs, derived the same way (from
    # whatever g/c/Gamma the chosen closure and solver actually
    # converged to, not re-solved).
    valid = EN != 0.0
    y = np.where(valid, g / np.clip(EN, 1e-300, None), np.nan)
    curves["yr"] = y
    with np.errstate(invalid="ignore", divide="ignore"):
        curves["Br"] = np.where(y > 0, np.log(y) - curves["gamma"], np.nan)

    return OZResult(solverInstance.getrArray(), solverInstance.getqArray(),
                     potential, tuple(potentialArgs), closure, closureParam,
                     phi, solver, curves, solverInstance)
