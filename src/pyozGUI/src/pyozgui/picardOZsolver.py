# -*- coding: utf-8 -*-
#calculations
import numpy as np

#Base class
from oZsolver import OZsolver
#This class inherits all that is needed to run the 
#iteration and to communicate with clients from OZsolver
class PicardOZsolver(OZsolver):
    def __init__(self, port, **kwargs):
      #First run constructor of super class
      OZsolver.__init__(self, port, **kwargs)

    #define the specific iteration schema for this class    
    def picardIteration(self):
      #MANN / KRASNOSELSKII ITERATION.
      #
      #  x_{n+1} = (1 - alpha) x_n + alpha T(x_n)
      #
      #Plain Picard is the alpha = 1 case. The averaged form is due to Mann
      #(1953); with constant alpha it is usually called Krasnoselskii, and
      #the Krasnoselskii-Mann theorem is what gives convergence for
      #NONEXPANSIVE maps, where undamped Picard need not converge at all.
      #
      #alpha defaults to 1.0, so existing behaviour and every recorded
      #regression value are unchanged -- verified: HardSphere PY g_max
      #2.35611803 and LennardJones HNC 2.16359468, both exact. Set
      #self.mannAlpha to damp.
      #
      #This matters because Picard is the default VERIFIER in ozLib.solve(),
      #chosen for independence -- it is the only solver here with no
      #acceleration and no history vectors, so it cannot share an
      #acceleration failure mode with the primary. Undamped it is also the
      #most fragile: it returns NaN for Lennard-Jones at epsilon = 0.8,
      #phi = 0.3 under a type-4 transform, where alpha = 0.5 converges to the
      #correct 2.164596. A verifier that fails is nearly as unhelpful as one
      #that agrees wrongly.
      #
      #NOTE the manuscript's implementation section describes the final
      #fallback as "damped Picard iteration". Until now it was not damped.
      #With this the description becomes true, but only if an alpha < 1 is
      #actually chosen somewhere; the default is still undamped so that
      #nothing silently changes.
      alpha = float(getattr(self, 'mannAlpha', 1.0))
      if not (0.0 < alpha <= 1.0):
          raise ValueError(f"mannAlpha must be in (0, 1], got {alpha}")
      #NOT CONVERGED UNTIL PROVEN OTHERWISE.
      #
      #Exhausting the iteration limit used to print a warning and return the
      #last iterate regardless, with nothing a caller could test. That is how
      #BPGG alpha = 0.5, phi = 0.4 produced a "solution" whose residual was
      #|F| = 21.3. Callers should check `converged` (and, better, recompute
      #the residual themselves -- a flag is a claim, a residual is evidence).
      self.converged = False
      self.finalRelativeProgress = float('inf')
      #Start iteration
      x = self.x_0
      #Simple Picard iteration max number of steps = self.numberOfIterations
      #Converged if relative progress is less than self.convergenceCriterion
      i = 0; previousNorm = 0.0; xPrevious = None
      while  i < self.numberOfIterations and not self.isInterrupted:
          xPrevious = x
          xNew = self.fixPointOperator(x)
          x = xNew if alpha == 1.0 else (1.0 - alpha)*x + alpha*xNew
          norm = np.linalg.norm(x)
          #CONVERGENCE ON THE INCREMENT, not on successive NORMS.
          #
          #This previously read
          #    relativeProgress = abs(previousNorm - norm)/(norm + eps)
          #which compares ||x_n|| with ||x_{n-1}||. Two DIFFERENT vectors of
          #similar norm pass that test, so an iterate still moving but whose
          #norm has momentarily plateaued is reported as converged. That is
          #not hypothetical: at BPGG alpha = 0.5, phi = 0.4 it returned
          #g_max = 3.0039 as a converged solution when the residual there was
          #|F| = 19.4 -- not a fixed point at all, while the true root is
          #5.2528. The error was only found by evaluating |F| at the claimed
          #solution, which nothing in the pipeline was doing.
          #
          #||x - x_prev|| is the honest measure of progress. Better still
          #would be the residual ||T(x) - x||, which is what correctness
          #actually depends on; that is a larger change because the damped
          #update means the increment and the residual differ by a factor
          #alpha, so it is left as a deliberate next step rather than done
          #silently here.
          relativeProgress = (np.linalg.norm(x - xPrevious)
                              / (norm + np.finfo(float).eps))
          previousNorm = norm
          self.finalRelativeProgress = float(relativeProgress)
          if relativeProgress < self.convergenceCriterion: #Defined in super class
              #pass
              self.converged = True
              print("Picard converged after", i, "steps")
              break
          i += 1
          
      #endwhile
      if i == self.numberOfIterations:
          print("Picard did not converge after", self.numberOfIterations, "steps")

      self.derivePhysicalQuantitiesFromFixpoint(x)
    #implement the abstract method such that this class can be instantiated
    def solve(self):
      self.picardIteration()
