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
_BASE_SOLVER_CLASSES = {
    "Picard iteration": (PicardOZsolver, None),
    "Anderson acceleration": (AndersonOZsolver, None),
    "scipy Anderson": (ScipyAndersonOZsolver, None),
    "scipy Newton-Krylov": (ScipyNewtonKrylovOZsolver, None),
    "Biggs-Andrews": (BiggsAndrewsOZsolver, None),
}
if _HAVE_SUNDIALS4PY:
    _BASE_SOLVER_CLASSES["sundials4py: Newton-Krylov (GMRES)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPGMR")
    _BASE_SOLVER_CLASSES["sundials4py: Newton-Krylov (FGMRES)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPFGMR")
    _BASE_SOLVER_CLASSES["sundials4py: Newton-Krylov (TFQMR)"] = (Sundials4pyKinsolOZsolver, "SUNLinSol_SPTFQMR")
    _BASE_SOLVER_CLASSES["sundials4py: Fixed-Point (Anderson)"] = (Sundials4pyKinsolFPOZsolver, None)

_defaultSolverName = ("sundials4py: Fixed-Point (Anderson)" if _HAVE_SUNDIALS4PY
                      else "scipy Anderson")
# Reordered so the FIRST key is this project's recommended DEFAULT
# solver -- oZgui.py's own dropdown uses next(iter(SOLVER_CLASSES)) as
# its initial pre-selected value. SUNDIALS' own Anderson-accelerated
# fixed-point strategy (KIN_FP) when sundials4py is installed --
# SUNDIALS' production-grade implementation of the same acceleration
# idea as this project's own hand-written "Anderson acceleration"/
# "scipy Anderson" entries, generally more robust and faster to
# converge for the harder closures (HMSA/RY/BPGG/CJVM/BB) than plain
# unaccelerated Picard iteration, which is why it replaces Picard as
# the default here. Falls back to scipy's own Anderson acceleration
# (scipy.optimize.anderson-based, no extra dependency) when
# sundials4py isn't installed, rather than falling all the way back to
# plain Picard -- Picard is still fully present in the dict, just no
# longer pre-selected. Every other key keeps its original relative
# order from _BASE_SOLVER_CLASSES above; only the chosen default moves
# to the front. (solve()'s own solver= keyword argument default below
# is set to this same _defaultSolverName, not left hardcoded to
# "Picard iteration" -- so a script calling ozLib.solve() directly,
# without going through the GUI at all, gets the same improved default
# rather than silently falling back to unaccelerated Picard.)
SOLVER_CLASSES = {_defaultSolverName: _BASE_SOLVER_CLASSES.pop(_defaultSolverName)}
SOLVER_CLASSES.update(_BASE_SOLVER_CLASSES)

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
    #Added after cross-validating against OrnsteinZernike.jl (Closures.jl).
    #All three are standalone bridge functions of Gamma, so they inherit the
    #multicomponent path, every solver and this dropdown automatically.
    #"Khanpour" is spelled out because "KH" above is Kovalenko-Hirata --
    #different closure, same natural abbreviation.
    "Khanpour":              ("doKhanpourClosure", True),
    #Implicit bridge function (solved by fixed-point iteration inside
    #update_c); lambda sets the amplitude and is r-dependent for lambda <= 0.
    "Carbajal-Tinoco":       ("doCarbajalTinokoClosure", True),
    "Modified Verlet":       ("doModifiedVerletClosure", True),
    #Extended RY takes TWO parameters (alpha and the quadratic coefficient a);
    #the second is set on the solver directly, so only alpha comes through the
    #single-parameter GUI field. a = 0 reduces exactly to Rogers-Young.
    "Extended Rogers-Young": ("doExtendedRYclosure", True),
    # ZSEP (Lee 1995, hard spheres only) is a "special" entry, like
    # Reference HNC/Rescaled MSA above: it needs its own orchestration
    # method (fitZSEPparameters()) rather than a plain doXXXclosure()+
    # solve() call, since it self-fits all three of its own parameters
    # (zeta, phi, alpha) against Lee's exact zero-separation-theorem
    # conditions instead of taking any of them from the caller. See
    # solve() below for exactly how it's dispatched, and
    # oZsolver.py's own fitZSEPparameters() docstring for the physics
    # and its documented ill-conditioning caveat.
    "ZSEP":                  ("fitZSEPparameters", False),
}

# Closures with a single free parameter (needsExtraScalarParam==True
# above) that ALSO have a published thermodynamic-consistency
# condition determining that parameter automatically -- see
# OZsolver.findThermodynamicallyConsistentParameter() (already fully
# generic across potentials, not limited to any specific one; ported
# directly from sasfit_oz_solver.c's own root_finding(), NOT the
# same as the older, abandoned, LJ/Yukawa-only optimizeRYalpha.py
# prototype elsewhere in this project). Maps this module's own
# closure name to that method's own closureName argument. Used by
# solve()'s new findConsistentParameter= option below -- Modified HNC
# is included here too (it already appears above with
# needsExtraScalarParam=True for its own eta parameter, which this
# same consistency condition also determines).
# Closures needing a SECOND scalar beyond closureParam, as
#   dropdown label -> (argument name, default, short description)
#
# Only Extended Rogers-Young needs this so far: its bridge function is the
# ordinary RY construction plus one extra quadratic term,
#     phi = (exp(f*Gamma)-1)/f,  B = -Gamma + log1p(phi + a*phi^2)
# so it carries the RY switching rate alpha AND the coefficient a. a = 0
# reduces exactly to RY, which is why that is the default.
#
# Kept as its own table rather than widening CLOSURE_SETTERS to 3-tuples,
# so every existing consumer of CLOSURE_SETTERS keeps working unchanged.
SECOND_CLOSURE_PARAM = {
    "Extended Rogers-Young": ("a", 0.0,
                              "quadratic coefficient; a = 0 gives plain RY"),
}


def secondClosureParam(closureName):
    """(name, default, description) or None if the closure needs only one."""
    return SECOND_CLOSURE_PARAM.get(closureName)


# Which closures can be used with a MULTICOMPONENT (polydisperse) potential.
#
# The distinction is structural, not a matter of taste. Every closure whose
# bridge function is a plain elementwise expression in Gamma (and optionally
# the repulsive/attractive potential split) operates on the (p,p,N) pair
# matrices unchanged, so it is multicomponent for free. The excluded ones each
# need something that only exists for a single component:
#
#   Reference HNC   needs a one-component hard-sphere reference solve (g0/G0)
#   Modified HNC    needs an analytic one-component PY hard-sphere bridge
#                   function at a single packing fraction; the mixture
#                   equivalent is the Lado-type variational problem, i.e. a
#                   research question rather than a port
#   Rescaled MSA    is a one-component diameter-rescaling procedure; the
#                   polydisperse counterpart is the separate analytic module
#                   polydisperse_rmsa.py
#   EuRah           uses a precomputed one-component HS/PY array (CEURAH)
#   ZSEP            fixes its three parameters from the Carnahan-Starling
#                   zero-separation theorems, which are hard-sphere specific
#                   (fitZSEPparameters() already refuses anything else)
#
# Martynov-Sarkisov, Vompe-Martynov and CJVM are NOT excluded here: they are
# structurally fine and do run, but all three build a square-root bridge
# function and exp(G + B) overflows for strongly coupled charged systems --
# a closure-domain limitation that applies equally in one component, so it is
# reported as a failed solve rather than hidden as an unavailable option.
MULTICOMPONENT_INCAPABLE_CLOSURES = {
    "Reference HNC", "Modified HNC", "Rescaled MSA", "EuRah", "ZSEP",
}


def multicomponentCapableClosures():
    """Dropdown labels usable with a polydisperse potential."""
    return [name for name in CLOSURE_SETTERS
            if name not in MULTICOMPONENT_INCAPABLE_CLOSURES]


def isMulticomponentPotential(potentialName):
    """True for potentials that set up more than one component.

    A name test rather than a probe of the solver, so the GUI can filter the
    closure list the moment a potential is chosen, before anything is built.
    """
    return potentialName.startswith("Polydisperse")


CONSISTENT_PARAMETER_CLOSURES = {
    "Rogers-Young": "RY", "HMSA": "HMSA", "Modified HNC": "MHNC",
    "BPGG": "BPGG", "CJVM": "CJVM", "BB": "BB",
    #The three closures added from OrnsteinZernike.jl each carry a single
    #free parameter, so the same compressibility-vs-virial search applies.
    "Khanpour": "Khanpour", "Modified Verlet": "ModifiedVerlet",
    "Carbajal-Tinoco": "CarbajalTinoko",
    "Extended Rogers-Young": "ExtendedRY",
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
          closureParam2=None,
          findConsistentParameter=False,
          solver=_defaultSolverName, maxIterations=1000,
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
        HNC, Rogers-Young, HMSA, BPGG, CJVM, BB) UNLESS
        findConsistentParameter=True is given instead (see below).
        Not used for ZSEP (it always self-fits all three of its own
        parameters; see fitZSEPparameters() on OZsolver directly if
        you want to fix one of them rather than fit all three).
    findConsistentParameter: if True, instead of taking closureParam
        from the caller, automatically searches for the value that
        makes the compressibility-route and virial-route isothermal
        compressibility agree (Rogers & Young's own thermodynamic-
        consistency idea) -- see
        OZsolver.findThermodynamicallyConsistentParameter() for the
        physics and its own documented caveats (a located root can
        occasionally be a numerical artefact rather than a genuine
        one; that method already screens for this and falls back to
        a closest-approach value with a printed warning when it
        cannot find a clean one). Only valid when `closure` is one of
        CONSISTENT_PARAMETER_CLOSURES (Rogers-Young, HMSA, Modified
        HNC, BPGG, CJVM, BB); raises ValueError otherwise. When True,
        closureParam is ignored (the found value is used instead).
    solver: solver name, see SOLVER_CLASSES above. Defaults to
        SUNDIALS' Anderson-accelerated fixed-point strategy
        ("sundials4py: Fixed-Point (Anderson)") when sundials4py is
        installed, otherwise "scipy Anderson" -- see SOLVER_CLASSES'
        own comment above for why. Plain (unaccelerated) Picard
        iteration is still available (solver="Picard iteration"), just
        no longer the default.
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

    Raises ValueError for an unrecognised closure/solver name, a
    closure that needs closureParam when none was given and
    findConsistentParameter is not requested, ZSEP requested for any
    potential other than HardSphere (see below), or
    findConsistentParameter=True for a closure that doesn't support it
    -- these are checked here (before touching the solver) so a
    mistake is reported immediately rather than surfacing later as a
    confusing internal AttributeError.
    '''
    if closure not in CLOSURE_SETTERS:
        raise ValueError(f"unknown closure {closure!r}, must be one of {list(CLOSURE_SETTERS)}")
    if solver not in SOLVER_CLASSES:
        raise ValueError(f"unknown solver {solver!r}, must be one of {list(SOLVER_CLASSES)}")
    if closure == "ZSEP" and potential != "HardSphere":
        # OZsolver.fitZSEPparameters() ALSO checks this itself (see that
        # method's own docstring) -- but only via a print()ed warning
        # and an early `return None`, not an exception. Without this
        # check here too, that would leave solverInstance completely
        # unsolved (radialDistributionFunction etc. still at their
        # __init__ all-zeros defaults) and this function would still
        # happily package that up into a normal-looking OZResult full of
        # zeros/S(Q)=1 everywhere, with no exception and nothing wrong
        # visible except a Log-tab message a GUI user could easily miss.
        # ZSEP's three fitting conditions (Lee 1995) are derived from the
        # exact Carnahan-Starling hard-sphere equation of state
        # specifically -- there is no meaningful way to "fit" it against
        # any other potential's own equation of state.
        raise ValueError("closure='ZSEP' is only meaningful for potential='HardSphere' "
                          "(Lee (1995)'s zero-separation conditions are derived from the exact "
                          f"Carnahan-Starling hard-sphere equation of state), not {potential!r}")

    setterName, needsParam = CLOSURE_SETTERS[closure]
    if findConsistentParameter and closure not in CONSISTENT_PARAMETER_CLOSURES:
        raise ValueError(f"findConsistentParameter=True is only supported for "
                          f"{list(CONSISTENT_PARAMETER_CLOSURES)}, not {closure!r}")
    if needsParam and closureParam is None and not findConsistentParameter:
        raise ValueError(f"closure {closure!r} needs a closureParam value "
                          f"(or findConsistentParameter=True)")

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
    elif setterName == "fitZSEPparameters":
        solverInstance.fitZSEPparameters()
    elif findConsistentParameter:
        solverInstance.findThermodynamicallyConsistentParameter(CONSISTENT_PARAMETER_CLOSURES[closure])
    else:
        if needsParam:
            #Closures listed in SECOND_CLOSURE_PARAM take a second scalar;
            #pass it through when the caller supplied one, otherwise let the
            #setter apply its own default (a = 0 for Extended RY, which
            #reduces it exactly to Rogers-Young).
            extra = SECOND_CLOSURE_PARAM.get(closure)
            if extra is not None and closureParam2 is not None:
                getattr(solverInstance, setterName)(closureParam, closureParam2)
            else:
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
