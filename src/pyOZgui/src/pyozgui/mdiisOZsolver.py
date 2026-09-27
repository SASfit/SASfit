# -*- coding: utf-8 -*-
"""MDIIS: Modified Direct Inversion in the Iterative Subspace.

Kovalenko, Ten-no & Hirata's adaptation of Pulay's DIIS, the standard
accelerator in the 3D-RISM literature where it is often the difference
between converging and not.

WHAT IT DOES. Keep the last m iterates and their residuals, solve a small
(m+1)x(m+1) system for the coefficients that minimise the residual norm over
their span, and step from that optimal combination. The residual here is
r = T(x) - x for the gamma fixpoint map, exactly as for Picard.

THE "MODIFIED" PART is a diagonal regularisation added to the residual
overlap matrix. Plain DIIS goes singular once the stored iterates become
nearly parallel -- which happens precisely as it converges -- and the
regularisation keeps the system solvable, biasing the step back toward
simple mixing when the history is degenerate.

RELATION TO ANDERSON ACCELERATION, which this package also offers: they are
the same algorithm. Anderson (1965) and Pulay (1980) were derived
independently and are algebraically equivalent for this problem, as the
report notes when it observes Anderson is in turn equivalent to GMRES. So
MDIIS is not a new class of method; what it adds is the explicit
regularisation and history-depth control, which matter where Anderson
stalls.

MEASURED, and the honest summary is that this is a FALLBACK, not a
recommendation.

On the real grid (N = 4095, SUNDIALS installed) it is fifth of nine at
0.0117 s, against SUNDIALS KIN_FP at 0.0053 and scipy Anderson at 0.0089.
An earlier measurement at N = 1023 in an environment WITHOUT SUNDIALS put it
ahead of scipy Anderson; that was wrong, and wrong in the flattering
direction, because the small grid hid the cost of its Python-level
(m+1)x(m+1) solve per step and the fastest solver was absent from the
comparison entirely.

Iterations to converge, hard spheres under PY, N = 4095, 100 points per
diameter:

    phi     KIN_FP   scipy Anderson   MDIIS
    0.45      28           28           54
    0.52      38          116           83
    0.55      43           40           97
    0.58      60          179        FAILS

So where SUNDIALS is present there is no case for it: KIN_FP is faster at
every density and converges at 0.58 where this does not.

WHERE IT EARNS ITS PLACE is an installation WITHOUT SUNDIALS, which is the
fallback configuration ozLib drops into when _HAVE_SUNDIALS4PY is false --
anyone installing from PyPI without building SUNDIALS. There the choice is
between this and scipy Anderson, and scipy Anderson's iteration count swings
erratically with density (28, 116, 40, 179) where this one rises smoothly.
At phi = 0.52 it is the better of the two. Offer it as the alternative to
try when scipy Anderson stalls, and nothing more than that.

Against plain Picard it looks transformative -- 30 iterations against 336 at
phi = 0.30, and convergent at every density where Picard diverges above
0.42. That comparison is not worth much: Picard is the weakest solver here.

THE PARAMETERS MATTER ENORMOUSLY and a bad choice makes the method look
broken rather than mistuned. A first attempt with delta = 0.3 and damp = 0.5
failed at every state point above phi = 0.42 and was nearly written off:
delta that large over-regularises, collapsing MDIIS toward plain mixing, and
damp = 0.5 halves a step that is already optimal. With m = 5, delta = 0.01
and damp = 1.0 it converges everywhere Picard does not. Those are the
defaults below.

DENSITY RAMPING DOES NOT RESCUE PICARD, which is worth recording next to
this since it is the obvious alternative. Walking phi up in steps of 0.02
from 0.10, warm-starting each solve from the last, the iteration count grows
geometrically -- 41, 49, 60, 74, ... 946 -- and then fails outright at
phi = 0.42 with a residual of 1.1, not a slowly decreasing one. Picard's
contraction factor exceeds 1 there; the basin has not moved, it has ceased
to exist, so no starting point inside it helps. Continuation methods fail
for the same reason.
"""
import numpy as np

from oZsolver import OZsolver


class MDIISOZsolver(OZsolver):

    #Defaults chosen by measurement, not taken from the literature -- see the
    #module docstring for what the alternatives did.
    HISTORY = 5
    DELTA = 0.01
    DAMPING = 1.0

    def __init__(self, port, **kwargs):
        self.iterationStep = 0
        self.mdiisHistory = int(kwargs.pop("mdiisHistory", self.HISTORY))
        self.mdiisDelta = float(kwargs.pop("mdiisDelta", self.DELTA))
        self.mdiisDamping = float(kwargs.pop("mdiisDamping", self.DAMPING))
        OZsolver.__init__(self, port, **kwargs)

    def solve(self):
        try:
            m = max(1, self.mdiisHistory)
            delta = self.mdiisDelta
            damp = self.mdiisDamping
            #ATTRIBUTES, not accessors. OZsolver exposes convergenceCriterion
            #and numberOfIterations directly, and
            #derivePhysicalQuantitiesFromFixpoint TAKES the fixpoint as an
            #argument rather than reading it off self -- checked against
            #scipyAndersonOZsolver rather than assumed.
            tol = self.convergenceCriterion
            maxit = self.numberOfIterations

            x = np.array(self.x_0, float)
            xs, rs = [], []
            converged = False
            for step in range(maxit):
                self.iterationStep = step + 1
                #rootOperator is r = T(x) - x, the same residual Picard and
                #the Anderson solvers use, so the tolerance means the same
                #thing across all of them.
                r = np.asarray(self.rootOperator(x), float)
                res = float(np.max(np.abs(r)))
                if not np.isfinite(res):
                    break
                if res < tol:
                    converged = True
                    break
                xs.append(x.copy())
                rs.append(r.copy())
                if len(xs) > m:
                    xs.pop(0)
                    rs.pop(0)
                n = len(xs)
                #Bordered overlap matrix: B[i,j] = <r_i, r_j>, with the last
                #row and column imposing sum(coefficients) = 1.
                B = np.empty((n + 1, n + 1))
                for i in range(n):
                    for j in range(i, n):
                        B[i, j] = B[j, i] = float(rs[i] @ rs[j])
                #THE MODIFICATION. Scaled by the overlap's own magnitude so
                #that delta is dimensionless in effect and does not depend on
                #how large the residuals happen to be.
                scale = np.trace(B[:n, :n])/float(n)
                B[:n, :n] += delta*scale*np.eye(n)
                B[n, :n] = -1.0
                B[:n, n] = -1.0
                B[n, n] = 0.0
                rhs = np.zeros(n + 1)
                rhs[n] = -1.0
                try:
                    coef = np.linalg.solve(B, rhs)[:n]
                except np.linalg.LinAlgError:
                    #Degenerate even after regularisation: fall back to a
                    #plain step from the newest iterate rather than failing.
                    coef = np.zeros(n)
                    coef[-1] = 1.0
                xopt = sum(c*xi for c, xi in zip(coef, xs))
                ropt = sum(c*ri for c, ri in zip(coef, rs))
                x = xopt + damp*ropt
                if not np.all(np.isfinite(x)):
                    break

            if converged:
                print(f"MDIIS converged after {self.iterationStep} steps")
            else:
                print(f"MDIIS did not converge after {self.iterationStep} "
                      f"steps")
            #SET THE FLAG, do not merely print it. picardOZsolver establishes
            #the convention -- callers check `converged` -- and an earlier
            #version of this class computed the value and then dropped it, so
            #a failed solve was indistinguishable from a good one to anything
            #but a human reading stdout.
            #
            #Not hypothetical: at phi = 0.58 on a 4095-point grid this solver
            #runs out of iterations and returns min S(Q) = 0.00600 against the
            #correct 0.00603 -- a plausible curve, three digits in, from a
            #solve that failed. A fit would have absorbed it without
            #complaint.
            #
            #The quantities are still derived, matching every other solver
            #here: a non-converged iterate is worth inspecting, and refusing
            #to return it would break callers that handle the flag properly.
            #What must not happen is returning it SILENTLY.
            self.converged = converged
            self.derivePhysicalQuantitiesFromFixpoint(x)
            self.iterationStep = 0
        except Exception as exc:
            print(f"MDIIS solver failed: {type(exc).__name__}: {exc}")
            raise
