# -*- coding: utf-8 -*-
"""Find DISTINCT solutions of the Ornstein-Zernike equations by deflation.

Implements the method of

    P. E. Farrell, A. Birkisson & S. W. Funke, "Deflation Techniques for
    Finding Distinct Solutions of Nonlinear Partial Differential Equations",
    SIAM J. Sci. Comput. 37(4), A2026-A2045 (2015).
    doi:10.1137/140984798, arXiv:1410.5620

WHY. The OZ closure equations admit several fixed points, and this package
has hit the consequences repeatedly. Continuation in density can identify
which root is connected to the dilute limit, but says nothing about HOW MANY
roots exist. Deflation answers that: it finds distinct solutions from the
SAME starting point, including branches disconnected from the one you began
on.

WHAT IT FOUND ON FIRST USE, which was not what was expected. At BPGG
alpha = 0.5, phi = 0.4 three solvers returned g_max = 5.2528 and Picard
returned 3.0039, both apparently physical. Deflation was applied to find out
whether more roots existed; instead it established that there was only ONE:

    Anderson's 5.252805   |F| = 3.79e-12   a genuine fixed point
    Picard's   3.003938   |F| = 1.94e+01   NOT a fixed point at all

Picard had reported success without converging (see the convergence-test
defect noted in picardOZsolver). By contrast, at Lennard-Jones
epsilon = 0.8, phi = 0.3 under HNC the same check confirms REAL multiple
roots: g_max = 2.1636 with min S(Q) = +0.21 and g_max = 1.4307 with
min S(Q) = -46.5 both have |F| ~ 1e-11.

So the single most useful thing here may not be finding extra roots but
CHECKING THE RESIDUAL AT EACH CLAIMED ONE. Nothing else in the pipeline was
doing that; a solver's own convergence flag is not evidence.

THE METHOD. Having found a root r, form the deflated residual

    G(x) = M(x; r) F(x),    M(x; r) = 1/||x - r||^p + alpha          (their 2.8)

and re-solve from the SAME initial guess. M is constructed so that any
sequence approaching r has ||G|| bounded away from zero, so a solver using
the residual norm as its merit function cannot converge there again. Other
roots are untouched, since M is linear and finite at them. Repeat,
accumulating one factor per known root, until the solve fails.

The alpha term is not cosmetic. With alpha = 0 the deflated residual tends
to zero as ||x - r|| -> infinity, and the solver can report convergence
having simply run away; the paper demonstrates this on a sigmoid where it
"converges" at x ~ -1.2e8. With alpha > 0 the deflated residual tends to
alpha*F(x) far from known roots, which is why alpha = 1 is their default.

TWO THINGS THIS NEEDS THAT OUR USUAL SOLVERS DO NOT PROVIDE.

1. A RESIDUAL-BASED SOLVER. Deflation acts on the residual, so it only
   influences an algorithm that uses ||residual|| as a merit function.
   Picard iterates x <- T(x) and would ignore deflation entirely: it never
   forms a residual to be deflated. Hence Newton-Krylov here, via
   scipy.optimize.newton_krylov.
2. GLOBALISATION. The paper needed damped/globalised Newton for several of
   its examples and reports that undamped iteration diverged for every
   (p, alpha) tried on the Painleve problem. newton_krylov's Armijo line
   search supplies this.

PRACTICAL NOTE. Newton-Krylov does not converge on the OZ problem from a
cold start at the states tried here, so supply the first root from a solver
that does (Anderson) and let deflation search from there. The paper is
explicit that the first solution may be found "by any means".

HONEST LIMITATIONS, stated by the authors:
  * deflation does NOT guarantee all solutions are found;
  * it is "highly sensitive to the choice of the shift parameter" -- on
    their Yamabe example, varying alpha over 10^0..10^-7 found between 1
    and 7 solutions -- and they give no a priori guidance, "reluctantly
    resorting to numerical experimentation". Expect to sweep (p, alpha).

USAGE

    from deflation import findDistinctSolutions
    def F(x):
        y = np.asarray(solver.fixPointOperatorForGamma(x), float)
        return (y[0] if y.ndim > 1 else y) - x     # note the [0]
    roots = findDistinctSolutions(F, np.zeros(N), knownRoots=[gammaFromAnderson])
"""
import numpy as np


def residualNorm(residual, x):
    """||F(x)||. Use this on ANY claimed solution before believing it.

    A solver's convergence flag is not evidence: at BPGG phi = 0.4 a claimed
    root had |F| = 19.4, and the only reason that was noticed is that
    someone computed this number.
    """
    return float(np.linalg.norm(residual(np.asarray(x, float))))


def deflationFactor(x, roots, p=2.0, alpha=1.0):
    """eta(x) = prod_i ( 1/||x - r_i||^p + alpha ), shifted deflation.

    Returns 1.0 when no roots are known, so the first solve is undeflated.
    """
    eta = 1.0
    for r in roots:
        d = float(np.linalg.norm(x - r))
        if d < 1e-14:                    # sitting exactly on a known root
            return np.inf
        eta *= 1.0/d**p + alpha
    return eta


def findDistinctSolutions(residual, x0, knownRoots=None, maxRoots=6,
                          p=2.0, alpha=1.0, tol=1e-8, maxiter=400,
                          distinct=1e-6, trueResidualTol=1e-4, verbose=True):
    """Peel off distinct roots of `residual`, all from the same guess `x0`.

    residual    : callable, x -> F(x), zero at a solution
    x0          : starting point, reused for EVERY solve. That reuse is the
                  point -- several roots from one guess is what distinguishes
                  deflation from multi-start.
    knownRoots  : roots already found by other means, deflated from the
                  start. Newton-Krylov often cannot find the first root on
                  this problem, so seeding it is the normal mode of use.
    distinct    : two roots closer than this in relative L2 are the same.

    Returns the list of NEWLY found roots (not including knownRoots).
    """
    from scipy.optimize import newton_krylov, NoConvergence

    roots = list(knownRoots) if knownRoots else []
    found = []
    for k in range(maxRoots):
        def deflated(x, roots=roots):
            return deflationFactor(x, roots, p, alpha)*residual(x)
        try:
            sol = newton_krylov(deflated, np.asarray(x0, float).copy(),
                                f_tol=tol, maxiter=maxiter,
                                line_search="armijo", verbose=False)
        except (NoConvergence, ValueError, ZeroDivisionError) as exc:
            if verbose:
                print(f"  deflation {k}: no further root "
                      f"({type(exc).__name__})")
            break
        except Exception as exc:
            if verbose:
                print(f"  deflation {k}: {type(exc).__name__}: "
                      f"{str(exc)[:50]}")
            break

        #Guard against the failure mode the shift is meant to prevent: a
        #solver that "converged" on the DEFLATED residual while the TRUE
        #residual is still large has run away rather than found anything.
        trueRes = residualNorm(residual, sol)
        if trueRes > trueResidualTol:
            if verbose:
                print(f"  deflation {k}: spurious -- deflated residual small "
                      f"but true residual {trueRes:.2e}")
            break

        if any(np.linalg.norm(sol - r)/max(np.linalg.norm(r), 1e-30) < distinct
               for r in roots):
            if verbose:
                print(f"  deflation {k}: returned a known root, stopping")
            break

        roots.append(sol)
        found.append(sol)
        if verbose:
            print(f"  deflation {k}: new root, |F| = {trueRes:.2e}, "
                  f"||x|| = {np.linalg.norm(sol):.4f}")
    return found
