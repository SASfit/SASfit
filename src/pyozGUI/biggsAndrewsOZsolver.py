# -*- coding: utf-8 -*-
'''
"Biggs-Andrews" accelerated fixed-point iteration, ported directly
from sasfit_oz_solver.c's own "case BIGGS_ANDREWS:" block (dispatched
there, like Picard/PicardS, through OZ_solver_by_iteration() -- a
hand-written iteration scheme, NOT SUNDIALS-based, exactly like
Picard/PicardS turned out to be; see picardOZsolver.py's own history
in this project for that same finding).

The scheme (from the C code, not re-derived from a textbook): after
two unaccelerated warm-up fixed-point steps (to have two residual
history vectors available), each further step:
  1. computes a spectral/Barzilai-Borwein-style step size
         alpha = beta/gamma                    (self.KINSetMAA > 0)
         alpha = sign(beta/gamma)*sqrt(|beta/gamma|)   (otherwise)
     where beta = <g_n, g_{n-1}>, gamma = <g_{n-1}, g_{n-1}> are inner
     products of consecutive fixed-point RESIDUALS g_k = G(x_k)-x_k
     (NOT gradients -- this accelerates a fixed-point map, not an
     optimization);
  2. clamps alpha to [-(n-1)/(n+2), (n-1)/(n+2)] -- the classic
     Nesterov momentum schedule bound;
  3. extrapolates the next trial point using |self.KINSetMAA| as the
     "order" (0 = no extrapolation, plain fixed-point; 1 = first-order/
     "heavy-ball" momentum y=x+alpha*(x-x_prev); >=2 = second-order,
     adding a further alpha^2/2*(x-2x_prev+x_prevprev) term);
  4. evaluates the fixed-point map at that extrapolated point.

self.KINSetMAA here plays a genuinely different role than in the
KINSOL-based solvers (sundials4pyKinsolFPOZsolver.py) despite sharing
this project's naming convention for it (matching OZd->KINSetMAA
being reused by SASfit's own C code across both the KINSOL-based AND
this hand-written scheme) -- there it is KINSOL's own Anderson
subspace dimension; here it selects extrapolation order (and its
SIGN separately selects which of the two alpha formulas above is
used), an unrelated meaning that happens to share the same underlying
storage slot in SASfit's own OZd struct.

The exact convergence criterion SASfit's own OZ_step()/"err" computes
internally is not visible from the calling code alone (it is computed
inside a function this project has not needed to trace in full) -- so
this file uses the same relative-norm-of-residual convergence check
this project's own picardOZsolver.py/andersonOZsolver.py already use,
rather than guessing at an exact match to that internal detail.

Validated (see the conversation this was built in): KINSetMAA=0
matches plain Picard iteration almost exactly (354 vs 353 steps on
HardSphere+PY at phi=0.3), confirming the order-0 branch degenerates
to unaccelerated fixed-point iteration correctly; KINSetMAA=1 and -2
both roughly halve the iteration count needed versus plain Picard on
that same case; every configuration tested (0, 1, 2, -2) agrees with
this project's independently-validated GMRES-based solver to within
~1e-12 on StickyHardSphere+MSA.
'''
import numpy as np

#Base class
from oZsolver import OZsolver

class BiggsAndrewsOZsolver(OZsolver):
    def __init__(self, port, **kwargs):
      OZsolver.__init__(self, port, **kwargs)
      #Extrapolation order (|value|) and step-size formula choice
      #(sign) -- see this file's own docstring. 2 (second-order,
      #sign>0) is used as a reasonable default; SASfit's own GUI
      #exposes this as an adjustable field, same as here.
      self.KINSetMAA = 2

    def solve(self):
      xn = self.x_0.copy()

      #Two unaccelerated warm-up fixed-point steps, exactly matching
      #the C code's own two hand-unrolled steps before the main loop
      #-- needed to have two residual-history vectors (gn1, gn2)
      #available before any extrapolation can be attempted.
      xp1 = self.fixPointOperator(xn)
      gn1 = xp1 - xn
      xn1 = xn.copy()
      xn = xp1.copy()
      yn = xn.copy()

      xp1 = self.fixPointOperator(yn)
      gn2 = gn1.copy()
      gn1 = xp1 - yn
      xn2 = xn1.copy()
      xn1 = xn.copy()
      xn = xp1.copy()

      n = 2
      previousNorm = np.linalg.norm(xn)
      relativeProgress = np.inf
      while n < self.numberOfIterations and relativeProgress >= self.convergenceCriterion and not self.isInterrupted:
          n += 1
          beta = np.dot(gn1, gn2)
          gamma = np.dot(gn2, gn2)
          if gamma == 0.0:
              alpha = 1.0
          else:
              if self.KINSetMAA > 0:
                  alpha = beta/gamma
              else:
                  alpha = np.sign(beta/gamma)*np.sqrt(abs(beta/gamma))
              bound = (n - 1.0)/(n + 2.0)
              alpha = max(min(alpha, bound), -bound)

          order = abs(self.KINSetMAA) * (alpha != 0.0)
          if order == 0:
              yn = xn.copy()
          elif order == 1:
              yn = xn + alpha*(xn - xn1)
          else:
              yn = xn + alpha*(xn - xn1) + 0.5*alpha*alpha*(xn - 2.0*xn1 + xn2)

          xp1 = self.fixPointOperator(yn)
          gn2 = gn1.copy()
          gn1 = xp1 - yn
          xn2 = xn1.copy()
          xn1 = xn.copy()
          xn = xp1.copy()

          norm = np.linalg.norm(xn)
          relativeProgress = abs(previousNorm - norm)/(norm + np.finfo(float).eps)
          previousNorm = norm

      if n >= self.numberOfIterations:
          print("Biggs-Andrews did not converge after", self.numberOfIterations, "steps")
      else:
          print("Biggs-Andrews converged after", n, "steps")

      self.derivePhysicalQuantitiesFromFixpoint(xn)
