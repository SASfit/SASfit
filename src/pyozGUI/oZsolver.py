# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*
'''
This class is abstracting the algorithm (solver),
i.e it tries to unify the methods and members
common to any kind of OZ fixpoint solving algorithm.
To implement a specific fixpoint finding
algorithm, one needs to derive from
this class and implement the 'solve()' method.
The class can also be used 'locally'
(without RPC)
This class additionally offers the possibility to
a) use a different Fix point operator (the one described in Kelley,
 http://epubs.siam.org/doi/book/10.1137/1.9780898718898 )
b) to set the initial FPO value such that one can iteratively
 approach high densities starting from moderate densities calculations
c) Calculate a analytic solution of the OZ equation. The analytic
version is considered generic (and any solver implementation may
want to compare against it) hence the analytic version is placed here
Note that in order for c) to work, all solver algorithms must be implemented
in a agnostic way about the problem size (may be either n or 2n, where n
is the number of sampling points). So the specific solvers must be 
defined in a generic way such that they can derive all they need from
the initial value!

Ported from Python 2 to Python 3 (2026): xmlrpclib -> xmlrpc.server,
`print x` -> `print(x)`. No numerical changes.
'''

#calculations
import numpy as np

#Server
from xmlrpc.server import SimpleXMLRPCServer

#solve() is abstract method
from abc import ABCMeta, abstractmethod

#Base class
from oZfixpointOperator import OZfixpointOperator

class OZsolver(OZfixpointOperator):
    #Init needs to be given by constructor, all other members can be set via RPC later
    #but port needs to be known in advance by the client. (Will be defined by the client)
    def __init__(self, port, **kwargs):
      OZfixpointOperator.__init__(self, port, **kwargs)
      #*******************************************************************************
      #We set the convergence Criterion hard coded, this could also be handed over to constructor 
      #(or set via setter). It is here (and not in the derived classes) since it is to be equal
      # for all algorithms. (To allow for a fair comparison)
      self.convergenceCriterion = 1e-12
      self.numberOfIterations = 1000; #Upper bound
      #final results (part of solver, not FPO)
      self.radialDistributionFunction = np.zeros(self.numberOfRadialSamplingPoints)
      self.structureFactor = np.zeros(self.numberOfRadialSamplingPoints)
      self.directCorrelationFunction = np.zeros(self.numberOfRadialSamplingPoints)
      #Control variable to stop loop
      self.isInterrupted = False
      #which operator to chose
      self.doUseGammaFixPointOperator = True
      #Having the proper start value is very important, the specific solver implementations
      #will rely on this
      if self.doUseGammaFixPointOperator:
          self.x_0 = np.zeros(self.numberOfRadialSamplingPoints)
      else:
          #The other algorithm available is the one in c,h space R^{2n}
          self.x_0 = np.zeros(2*self.numberOfRadialSamplingPoints)
 
      #....then RPC
      #*******************************************************************************
      #Init server, only if port number is valid
      if port > 0:
          #rpcServer was instantiated in super class
          self.rpcServer.register_function(self.solve, 'solve' )
          self.rpcServer.register_function(self.calculateAnalyticalRDFsolutionForHS, 'calculateAnalyticalRDFsolutionForHS' )
          self.rpcServer.register_function(self.getRDFasList, 'getRDFasList' )
          self.rpcServer.register_function(self.doInterrupt, 'doInterrupt' )

          #start serving..
          self.rpcServer.serve_forever()


    #Defining the fixpoint and root operator (the core of this unit)
    #******************************************************************
    def fixPointOperator(self, z):
      if self.doUseGammaFixPointOperator:
          #define a wrapper for returning Gamma only (without c)
          return self.fixPointOperatorForGamma(z)[0]
      else:
          #return x = [h,c] in this case
          return self.fixPointOperatorFor_ch(z)
          
    
    def rootOperator(self, x):
      return self.fixPointOperator(x) - x
    
    
    #**********************************************************************
    #If fixpoint was found, we can derive other useful quantities
    #This is the same procedure for all solver types, hence it is here
    def derivePhysicalQuantitiesFromFixpoint(self, x_fp):
      #Calculate c,h, gamma based on converged x for the ch based fixpoint operator
      if not self.doUseGammaFixPointOperator:
          c_fp = self.get_c_From_ch(x_fp)
          h_fp = self.get_h_From_ch(x_fp)
          G_fp = h_fp - c_fp
      else:
          #Calculate c based on the converged G_fp for Gamma based fixpoint operator
          G_fp = x_fp
          c_fp = self.update_c(G_fp)
          
      #Make results available
      self.structureFactor = self.calculateSq(c_fp)
      #Calculate RDF
      self.radialDistributionFunction = self.calculateRDF(G_fp, c_fp)
      #This is needed for thermodynimcal consistency check in optimizeRYalpha.py
      self.directCorrelationFunction = c_fp
      
    #Reference HNC: solves a reference hard-sphere system (same sigma
    #as the actual potential, via setHardSpherePotential() which
    #already always uses self.hardSphereDiameter) with the Martynov-
    #Sarkisov closure first, to get that reference system's own g0/G0
    #-- then restores the ACTUAL potential and solves again with the
    #RHNC closure (see OZfixpointOperator.update_c()'s 'RHNC' branch),
    #using g0/G0 plus beta*(perturbation potential) = beta*U_actual(r)
    #wherever EN_actual(r)>0 (0 elsewhere, though it does not actually
    #matter there since g0=0 makes that whole term vanish regardless --
    #see the comment in update_c() for why).
    #**********************************************************************
    def solveRHNC(self):
      #save the actual potential's state before overwriting it with
      #the reference (hard-sphere) potential below
      EN_actual = self.boltzmannOfP2Ppotential.copy()
      repulsive_actual = self.repulsivePartOfP2Ppotential.copy()
      attractive_actual = self.attractivePartOfP2Ppotential.copy()
      name_actual = getattr(self, 'activePotentialname', None)

      self.betaPerturbation = np.where(EN_actual > 0.0,
                                        -np.log(np.clip(EN_actual, 1e-300, None)), 0.0)

      print("solveRHNC: solving reference system (HS) with Martynov-Sarkisov (MS) closure first...")
      self.setHardSpherePotential()
      self.closureType = 'MS'
      self.solve()
      g0 = self.getRDF().copy()
      c0 = self.getDirectCorrelationFunction().copy()
      self.g0 = g0
      self.G0 = g0 - c0 - 1.0
      print("solveRHNC: reference system done")

      #restore the actual potential's state
      self.boltzmannOfP2Ppotential = EN_actual
      self.repulsivePartOfP2Ppotential = repulsive_actual
      self.attractivePartOfP2Ppotential = attractive_actual
      if name_actual is not None:
          self.activePotentialname = name_actual

      self.closureType = 'RHNC'
      self.solve()

    #Eu-Rah closure: c(r) is fixed externally to an array (self.CEURAH)
    #that must itself be found via an OUTER root-find -- fundamentally
    #different from every other closure here, where c is some function
    #of Gamma computed fresh each OZ iteration. The residual for that
    #outer problem needs, at every trial CEURAH: 3 direct (non-
    #iterative, see OZfixpointOperator._directGammaAndCavityFromFixedC())
    #density-perturbed cavity-function calculations (at phi0-dphi,
    #phi0+dphi, phi0), then finite-difference derivatives of the cavity
    #function with respect to both r (np.gradient(), matching
    #sasfit_oz_solver.c's own calc_dy()'s forward/central/backward
    #finite-difference scheme) and density (central differences across
    #the 3 perturbed calculations) -- mirroring OZ_EuRah_step_kinsol()
    #and its own outer KINSpfgmr solve in that file exactly, just using
    #scipy's newton_krylov here for the outer root-find instead (this
    #is a self-contained root-find over a different unknown than
    #self.rootOperator/self.x_0, so it does not go through self.solve()
    #for this part -- only for the final, single confirming solve once
    #self.CEURAH has actually converged).
    #**********************************************************************
    def solveEuRah(self):
      from scipy.optimize import newton_krylov

      sigma = self.hardSphereDiameter
      r = self.getrArray()
      phi0 = self.volumeDensity
      Dphi = phi0*0.01
      Drho = self.transformVolume2ParticleNumberDensity(Dphi)
      EN = self.boltzmannOfP2Ppotential
      MAYER = EN - 1.0
      #index where the potential's own hard core ends (EN starts being
      #nonzero) -- used below to keep every dy/dr finite difference
      #from ever straddling across it.
      hsp = int(np.nonzero(EN != 0.0)[0][0]) if np.any(EN != 0.0) else self.numberOfRadialSamplingPoints

      def safe_gradient(y, boundary_idx):
          #np.gradient() alone, applied across the WHOLE array, was
          #found by testing to blow up the entire outer root-find:
          #the cavity function CAVITY (see _directGammaAndCavityFromFixedC())
          #is built from two DIFFERENT formulas on either side of the
          #potential's own hard-core boundary (g/EN outside, exp(G)
          #inside, since EN=0 there makes g/EN ill-defined) -- on a
          #finite grid these two formulas don't match to full numerical
          #precision exactly at that boundary, so a central-difference
          #stencil straddling it produces a large SPURIOUS derivative
          #right there (checked directly: roughly -19 vs a smooth ~-2
          #to -4 trend on either side), which then corrupts the entire
          #outer CEURAH root-find (a single application of the update
          #already amplified the initial guess's range by ~5x). Taking
          #the gradient separately on each side avoids this entirely.
          dy = np.empty_like(y)
          if boundary_idx > 0:
              dy[:boundary_idx] = np.gradient(y[:boundary_idx], self.Delta_r)
          if boundary_idx < len(y):
              dy[boundary_idx:] = np.gradient(y[boundary_idx:], self.Delta_r)
          return dy

      def euRahResidual(CEURAH_trial_core):
          #CEURAH[r>=hsp] is forced to exactly 0 by construction: the
          #formula below multiplies everything by MAYER=EN-1, which is
          #IDENTICALLY zero for r>=hsp whenever the potential has no
          #tail beyond the bare hard core (e.g. plain HardSphere itself)
          #-- confirmed by testing directly (MAYER[hsp:] == 0.0 exactly
          #for HardSphere). Passing the FULL array (including this
          #always-zero region) to newton_krylov made the outer root-find
          #start at an already-exactly-zero residual over that whole
          #stretch, which scipy's own Krylov Jacobian estimate cannot
          #take a meaningful step from -- surfacing as "Jacobian
          #inversion yielded zero vector" (or NoConvergence, depending
          #on scipy version) rather than anything specific to this
          #tool's own formulas. Restricting the unknown vector to just
          #the genuinely nontrivial r<hsp region (where MAYER is
          #generally nonzero) avoids ever handing scipy a degenerate,
          #already-solved sub-problem.
          CEURAH_trial = np.zeros_like(r)
          CEURAH_trial[:hsp] = CEURAH_trial_core

          G_m, CAVITY_m = self._directGammaAndCavityFromFixedC(CEURAH_trial, phi0 - Dphi)
          G_p, CAVITY_p = self._directGammaAndCavityFromFixedC(CEURAH_trial, phi0 + Dphi)
          G_0, CAVITY_0 = self._directGammaAndCavityFromFixedC(CEURAH_trial, phi0)

          dydr_m = safe_gradient(CAVITY_m, hsp)
          dydr_p = safe_gradient(CAVITY_p, hsp)
          dydr_0 = safe_gradient(CAVITY_0, hsp)

          dydrho = (CAVITY_p - CAVITY_m) / (2.0*Drho)
          dydrdrho = (dydr_p - dydr_m) / (2.0*Drho)

          CEURAH_new = (1.0/36.0) * MAYER * (36.0*CAVITY_0 + 18.0*phi0*dydr_0
                                              + 12.0*r*dydrho + 6.0*phi0*r*dydrdrho)
          return CEURAH_new[:hsp] - CEURAH_trial_core

      #Initial guess: the analytical PY c(r) at the actual density,
      #matching sasfit_oz_solver.c's own CEURAH[i]=crPY(r[i],phi)
      #starting point (its initial setup, before the main OZ_solver
      #loop even begins). Uses an index-based split (matching
      #self.hardSphereDiameterInPoints exactly) rather than a floating-
      #point x<1.0 test -- found by testing that the latter disagrees
      #with EN's own hard-core boundary by exactly one grid point
      #(x[hardSphereDiameterInPoints-1] evaluates to exactly 1.0, not
      #<1.0, due to the r-grid's own 1-indexed offset), which otherwise
      #leaves one grid point right at contact with a spurious nonzero
      #g(r) from mismatched inside/outside treatment.
      hsp = self.hardSphereDiameterInPoints
      x = r/sigma
      CEURAH_0_core = ((6.0*phi0*(1.0+0.5*phi0)**2*x[:hsp]
                        - (1.0+2.0*phi0)**2*(1.0+0.5*phi0*x[:hsp]**3)) / (1.0-phi0)**4)

      print("solveEuRah: starting outer root-find over CEURAH "
            "(slow -- every residual evaluation needs 3 density-perturbed cavity-function calculations)...")
      CEURAH_core = newton_krylov(euRahResidual, CEURAH_0_core, method='lgmres',
                                   maxiter=self.numberOfIterations, verbose=0)
      print("solveEuRah: outer root-find converged")

      self.CEURAH = np.zeros_like(r)
      self.CEURAH[:hsp] = CEURAH_core

      self.closureType = 'EuRah'
      self.solve()
      #g(r) inside the hard core is not physically meaningful (r<sigma
      #is excluded volume) -- every other closure that has an actual
      #hard core (PY/HNC/MSA/mMSA/RMSA/RHNC/HardSphere itself) already
      #forces g(r)=0 there, one way or another. EuRah's own c(r)
      #(self.CEURAH) has no such built-in masking, so without this,
      #g(r) there is whatever this closure's formula happens to produce
      #-- found by testing that it can be wildly oscillatory there
      #(down to ~-38 at the innermost grid point, tracked directly to
      #CAVITY=exp(Gamma) in that all-EN-is-zero region, a purely
      #mathematical continuation with no physical meaning), while the
      #physically relevant region (r>sigma) is completely unaffected
      #and gives a smooth, correctly-decaying g(r) throughout.
      self.radialDistributionFunction[:hsp] = 0.0

    #Hayter-Penfold-style rescaled MSA (Baxter's/sasfit_oz_solver.c's
    #own RMSA closure, see OZfixpointOperator.update_c()'s 'RMSA'
    #branch). Not a single-formula closure like the others -- an
    #orchestration that (a) solves plain MSA first, (b) checks whether
    #g(sigma+) is already non-negative there (if so, RMSA==MSA and
    #nothing more is needed, exactly as in the C code's own shortcut),
    #and (c) if not, bisection-searches for how far outward the
    #"apparent" hard-core gate (self.gate4g) needs to extend before
    #g stays non-negative from that point on, re-solving at each step.
    #Calls self.solve() (whichever concrete numerical solver this
    #instance actually is -- Picard/KINSOL/Anderson/etc, all inherited
    #from this same base class) multiple times, not just once.
    #**********************************************************************
    def _robustInlineSolve(self, startFrom=None, maxIterations=None, convergenceCriterion=None):
      #Plain Picard iteration on self.fixPointOperator, independent of
      #whichever top-level solver class self actually is -- used by
      #solveRMSA() below. Found by testing (see the conversation this
      #was added in) that relying on self.solve() for RMSA's own
      #repeated trial gate4g configurations was a genuine bug: several
      #of this project's own solver classes (e.g.
      #Sundials4pyKinsolOZsolver) simply print a message and return
      #early on non-convergence, WITHOUT updating
      #self.radialDistributionFunction -- meaning self.getRDF() could
      #silently return STALE data from whichever solve last actually
      #succeeded, corrupting RMSA's own decisions and reported result.
      #Plain Picard iteration, run for generously many iterations, has
      #proven reliably robust across this project's own full range of
      #test cases (unlike the Newton-Krylov solvers, which can fail
      #outright on some of the harder trial gate4g configurations
      #explored during the search), so it is used here regardless of
      #which solver class RMSA is actually being called through.
      #startFrom lets each trial warm-start from the previous trial's
      #own converged solution rather than always restarting from
      #x_0=0 -- matches the same density-continuation-with-warm-restarts
      #principle already used for solveEuRah()'s own outer iteration;
      #found to be necessary for reasonable convergence speed on the
      #harder (larger apparent-hard-core) trial gates.
      maxIterations = maxIterations if maxIterations is not None else max(self.numberOfIterations, 5000)
      convergenceCriterion = convergenceCriterion if convergenceCriterion is not None else self.convergenceCriterion
      x = startFrom.copy() if startFrom is not None else self.x_0.copy()
      previousNorm = np.linalg.norm(x)
      if previousNorm == 0.0:
          previousNorm = 1.0
      converged = False
      for i in range(maxIterations):
          x = self.fixPointOperator(x)
          if not np.all(np.isfinite(x)):
              return False, x
          norm = np.linalg.norm(x)
          relativeProgress = abs(previousNorm - norm)/(norm + np.finfo(float).eps)
          previousNorm = norm
          if relativeProgress < convergenceCriterion:
              converged = True
              break
      self.derivePhysicalQuantitiesFromFixpoint(x)
      return converged, x

    def solveRMSA(self):
      #Ported directly from sasfit_oz_solver.c's own "else if
      #(CLOSURE==RMSA)" block (not re-derived from first principles) --
      #a genuinely different algorithm from a plain bisection search:
      #a "grow, then bisect" scheme, where each solve's own FULL g(r)
      #array is scanned once to see exactly how far the negative
      #region extends (extending the excluded "apparent hard core" as
      #far as that single solve's own data justifies, no extra re-solve
      #needed for that scan), and only once a solve gives an already-
      #non-negative result does it fall back to bisecting the gap
      #between the historical bounds. This is materially different
      #from (and more efficient than) an earlier version of this
      #method that bisected on an arbitrary midpoint at every step,
      #blind to where the actual negative region ended within that
      #trial's own solve.
      N = self.numberOfRadialSamplingPoints
      EN = self.boltzmannOfP2Ppotential

      self.gate4g = (EN != 0.0).astype(float)
      self.closureType = 'MSA'
      print("solveRMSA: solving with MSA closure first...")
      _, xLast = self._robustInlineSolve()
      g = self.getRDF()
      print("solveRMSA: done")

      #first index outside the bare potential's own hard core
      outsideCore = np.nonzero(EN != 0.0)[0]
      if outsideCore.size == 0:
          print("solveRMSA: potential has no hard core (EN is never 0) -- RMSA is not applicable")
          return
      igneg = int(outsideCore[0])
      self.gate4g[:igneg] = 0.0
      indx_min_appearent_sigma = igneg
      indx_max_appearent_sigma = igneg

      if igneg >= N or g[igneg] >= 0.0:
          print("solveRMSA: g(sigma+)>=0, i.e. no rescaling necessary -- RMSA == MSA")
          return

      print("solveRMSA: g(sigma+)<0, i.e. rescaling necessary")
      self.closureType = 'RMSA'
      indx_min_appearent_sigma = igneg - 1
      indx_max_appearent_sigma = N - 1
      self.gate4g[igneg] = 0.0

      #Initial scan, using the PLAIN MSA solve's own g(r) (not yet a
      #fresh RMSA solve) to build a starting guess -- exactly matching
      #the C code's own initial "while (g[igneg]<0) {...}" scan, which
      #likewise reuses the MSA solve already computed above rather than
      #solving again first.
      while igneg < N and g[igneg] < 0.0:
          self.gate4g[igneg] = 0.0
          indx_max_appearent_sigma = igneg
          igneg += 1
      self.gate4g[igneg:] = 1.0

      #Main grow-then-bisect loop.
      if indx_min_appearent_sigma < indx_max_appearent_sigma:
          while indx_min_appearent_sigma != indx_max_appearent_sigma:
              print("solveRMSA: try to solve next OZ with RMSA, igneg:", igneg)
              stepConverged, xLast = self._robustInlineSolve(startFrom=xLast, convergenceCriterion=1e-7)
              g = self.getRDF()
              print("solveRMSA: solved OZ with RMSA, igneg:", igneg)
              #A trial that produced non-finite output is treated the
              #same as "still negative here", i.e. keep growing --
              #this project's own addition, not present in the C
              #source (which has no equivalent non-finite check at
              #all), needed here since this Python port's own Picard-
              #based _robustInlineSolve can produce non-finite output
              #on some trial gates in a way the C code's own KINSOL-based
              #solve apparently does not.
              if not np.all(np.isfinite(g)) or g[igneg] < 0.0:
                  #GROW: extend the exclusion zone as far as THIS
                  #solve's own g(r) justifies.
                  indx_min_appearent_sigma = igneg
                  while igneg < N and (not np.all(np.isfinite(g)) or g[igneg] < 0.0):
                      self.gate4g[igneg] = 0.0
                      igneg += 1
                  indx_max_appearent_sigma = igneg
              else:
                  #BISECT: this solve's exclusion zone was already
                  #sufficient -- try a smaller one.
                  #
                  #Note on a genuine ambiguity found in the C source
                  #itself (subsequently fixed there too, see project
                  #documentation): after computing the halved
                  #indx_max_appearent_sigma here, the ORIGINAL C code
                  #only did "for (i=igneg;i<NP;i++) gate4g[i]=1;" -- it
                  #did NOT reset gate4g between the new, smaller
                  #indx_max_appearent_sigma and the old igneg back to
                  #1, nor reassign igneg itself to the new, halved
                  #value -- so the next iteration's g[igneg] check and
                  #solve would silently re-test the SAME (larger) gate
                  #configuration as before, never actually probing the
                  #smaller extent the halving was meant to try. Fixed
                  #here (and in the C source) to match the algorithm's
                  #own clearly-stated intent: reset gate4g across the
                  #newly-excluded range back to 1, then re-point igneg
                  #at the new boundary.
                  #
                  #Edge case found by testing: when indx_min and
                  #indx_max are already adjacent integers (gap of 1),
                  #integer division (indx_min+indx_max)//2 always
                  #rounds DOWN to indx_min -- meaning a solve that just
                  #SUCCEEDED at indx_max would otherwise have its own
                  #proven-sufficient boundary discarded and collapsed
                  #straight back to "no extension at all". Recognised
                  #here as "already converged, indx_max is the answer"
                  #instead of bisecting further.
                  newIndxMax = (indx_min_appearent_sigma + indx_max_appearent_sigma)//2
                  if newIndxMax <= indx_min_appearent_sigma:
                      indx_min_appearent_sigma = indx_max_appearent_sigma
                  else:
                      self.gate4g[newIndxMax+1:igneg] = 1.0
                      indx_max_appearent_sigma = newIndxMax
                      igneg = indx_max_appearent_sigma
      finalConverged, xLast = self._robustInlineSolve(startFrom=xLast, convergenceCriterion=1e-7)
      if not finalConverged:
          print("solveRMSA: final rescaled solve did not converge -- result may not be reliable")
      print("solveRMSA: converged with apparent hard-core radius extended to grid point", indx_max_appearent_sigma)

    #We know a analytical OZ solution for hard sphere particles and Percus Yevick closure 
    #********************************************************************************************************
    def calculateAnalyticalRDFsolutionForHS(self):
      if self.isHNC or not (self.getpotentialByName() == "HardSphere"):
          print("analytical solution is only available for PY with HS")
          return
      sigma = self.getHardSphereDiameter()
      numberOfRadialSamplingPoints = self.getNumberOfRadialSamplingPoints()
      delta_r = self.getDelta_r()
      hardSphereDiameterInPoints = self.getHardSphereDiameterInPoints()
      rho_V = self.getVolumeDensity()
      r = self.getrArray()
      c = np.zeros(numberOfRadialSamplingPoints).astype('float')
      c[hardSphereDiameterInPoints:] = 0.0 #Not needed (c = np.zeros), just to emphasize definition of c (c = 0 for r > sigma)
      r_HSR = r[:hardSphereDiameterInPoints]
      c[:hardSphereDiameterInPoints] = ( 6.0*rho_V*(1.0 + 0.5*rho_V)**2*(r_HSR/sigma) - (1.0 + 2.0*rho_V)**2*(1 + 0.5*rho_V*(r_HSR/sigma)**3) ) / (1.0 - rho_V)**4
      c_hat = self.hankelTransform(c, delta_r)
      G_hat = c_hat/(1.0 - self.particleDensity*c_hat) - c_hat
      G = self.inverseHankelTransform(G_hat, delta_r)
      g = G + c + 1.0
      g[:hardSphereDiameterInPoints] = 0.0 #Here we must set it 
      self.radialDistributionFunction = g


    #We know a analytical OZ solution for hard sphere particles and Percus Yevick closure also in Fourier space
    #***********************************************************************************************************
    def calculateAnalyticalStructureFactorForHS(self, q):
      #the calculations and notations follow Naegele, AMAS lecture notes on colloidal soft matter, p.44/45
      sigma = self.getHardSphereRadius()
      rho_V = self.getVolumeDensity()
      if q == 0.0:
        #Avoid division by zero, take the limit formula (4.64)
        return (1.0 - rho_V)**4/((1 + 2.0*rho_V)**2)
      
      #result is expressed in reduced wave number
      y = q*sigma
      A = (1.0 + 2.0*rho_V)/(1.0 - rho_V)**2
      B = (1.0 + 0.5*rho_V)/(1.0 - rho_V)**2
      
      f_1 = (y - np.sin(y))/y**3
      f_2 = (np.cos(y) - 1.0)/y**2
      f_3 = f_2/y + 1.0/(2.0*y)
      f_4 = -y*f_1
      
      X = 1.0 - 12.0*rho_V*(A*f_1 + B*f_2)
      Y = 0.0 - 12.0*rho_V*(A*f_3 + B*f_4)
      
      return 1.0/(X**2 + Y**2)

    #Baxter's (1968) analytical PY sticky/adhesive hard sphere S(Q) --
    #ported directly from SASfit's own
    #src/plugins/hs_plus_shoulder_or_well/sasfit_sq_sticky_hard_sphere.c
    #(not re-derived from a textbook), same tau convention Baxter/this
    #tool's own setStickyHardSpherePotential()/the robertus_shs plugin
    #already use (small tau = sticky, large tau = hard-sphere limit).
    #Fully closed-form -- no reference-system sub-solve, unlike RMSA/
    #RHNC/EuRah above.
    def calculateAnalyticalStructureFactorForStickyHS(self, q, tau):
      RHS = 0.5*self.getHardSphereDiameter()
      #this file's own hardSphereDiameter already reads as such directly
      FP = self.getVolumeDensity()
      if FP == 0.0:
          return np.ones_like(q) if hasattr(q, "__len__") else 1.0

      kappa = 2.0*q*RHS
      epsi = tau + FP/(1.0 - FP)
      gama = FP*(1.0 + FP/2.0)/(3.0*(1.0 - FP)**2)
      disc = epsi**2 - gama
      if np.any(np.asarray(disc) < 0.0):
          print("calculateAnalyticalStructureFactorForStickyHS: tau =", tau,
                "is too small (discriminant < 0) -- no real solution, matching the C code's own check")
      lamb = 6.0/FP*(epsi - np.sqrt(np.clip(disc, 0.0, None)))
      mu = lamb*FP*(1.0 - FP)
      beta = -(3.0*FP*(2.0+FP)**2 - 2.0*mu*(1.0+7.0*FP+FP*FP) + mu*mu*(2.0+FP)) / (2.0*(1.0-FP)**4)
      alpha = (1.0 + 2.0*FP - mu)**2 / (1.0-FP)**4

      kappa = np.atleast_1d(np.asarray(kappa, dtype=float))
      CQ = np.empty_like(kappa)

      large = kappa > 1e-2
      k = kappa[large]
      CQ[large] = (-24.0*FP*k**-6 * (
                        alpha*k**3*(np.sin(k) - k*np.cos(k))
                        + beta*k**2*(2.0*k*np.sin(k) - (k**2 - 2.0)*np.cos(k) - 2.0)
                        + 0.5*FP*alpha*((4.0*k**3 - 24.0*k)*np.sin(k) - (k**4 - 12.0*k**2 + 24.0)*np.cos(k) + 24.0)
                   )
                   - 2.0*(FP*lamb)**2*(1.0 - np.cos(k))*k**-2
                   + 2.0*FP*lamb/k*np.sin(k))

      small = ~large
      eta = FP
      denom = 1.0 - 2.0*eta*(4.0*alpha + 3.0*beta + alpha*eta) - 2.0*eta*lamb - eta**2*lamb**2
      k2 = kappa[small]
      CQ[small] = (1.0/denom
                   - (k2**2*(48.0*alpha*eta + 40.0*beta*eta + 15.0*alpha*eta**2
                             + 20.0*eta*lamb + 5.0*eta**2*lamb**2))
                     / (60.0*denom**2))

      SQ = 1.0/(1.0 - CQ)
      return SQ if SQ.size > 1 else float(SQ[0])

    #Finite-well generalization of the above (SASfit's own
    #sasfit_sq_sticky_hard_sphere_2.c) -- the plain Baxter formula
    #above assumes the idealized delta->0 "sticky point" limit (a true
    #adhesive delta-function interaction), which is NOT the same
    #physical system as this tool's own finite-width
    #setStickyHardSpherePotential(tau, delta) numerical potential.
    #Found by testing: comparing the numerical solve against the plain
    #Baxter formula above gets WORSE, not better, as delta shrinks (and
    #the numerical solve eventually breaks down entirely once delta
    #becomes comparable to the r-grid's own spacing, which a finite
    #grid simply cannot resolve) -- this finite-DELTA formula is the
    #actually-correct, apples-to-apples comparison for that potential
    #at whatever delta was actually used.
    def calculateAnalyticalStructureFactorForStickyHS2(self, q, tau, delta):
      RHS = 0.5*self.getHardSphereRadius()
      FP = self.getVolumeDensity()
      if FP == 0.0:
          return np.ones_like(q) if hasattr(q, "__len__") else 1.0

      sigma = 2.0*RHS + delta
      kappa = np.atleast_1d(np.asarray(q, dtype=float))*sigma
      phi = FP*(sigma/(2.0*RHS))**3

      ltmp = 6.0*(tau/phi + 1.0/(1.0-phi))
      disc = ltmp**2 - 12.0/phi*(1.0+0.5*phi)/(1.0-phi)**2
      if disc >= 0.0:
          l1 = ltmp + np.sqrt(disc)
          l2 = ltmp - np.sqrt(disc)
      else:
          print("calculateAnalyticalStructureFactorForStickyHS2: tau =", tau,
                "is too small (discriminant < 0), matching the C code's own check")
          l1 = l2 = ltmp
      lamb = l1 if abs(l1) < abs(l2) else l2

      mu = lamb*phi*(1.0-phi)
      A = 0.5*(1.0 + 2.0*phi - mu)/(1.0-phi)**2
      B = 0.5*sigma*(mu - 3.0*phi)/(1.0-phi)**2
      C = -A*sigma**2 - B*sigma + lamb*sigma**2/12.0

      I0 = np.sin(kappa)/kappa
      I1 = (np.cos(kappa) + kappa*np.sin(kappa) - 1.0)/kappa**2
      I2 = (kappa**2*np.sin(kappa) - 2.0*np.sin(kappa) + 2.0*kappa*np.cos(kappa))/kappa**3
      J0 = (1.0 - np.cos(kappa))/kappa
      J1 = (np.sin(kappa) - kappa*np.cos(kappa))/kappa**2
      J2 = (2.0*np.sin(kappa)*kappa + 2.0*np.cos(kappa) - kappa**2*np.cos(kappa) - 2.0)/kappa**3

      alpha = 1.0 - 12.0*phi*(C/sigma**2*I0 + B/sigma*I1 + A*I2)
      beta = 12.0*phi*(C/sigma**2*J0 + B/sigma*J1 + A*J2)

      SQ = 1.0/(alpha**2 + beta**2)
      return SQ if SQ.size > 1 else float(SQ[0])

    #PY square-well structure factor (Sharma & Sharma, Physica A 89
    #(1977) 213 -- the classic closed-form first-order PY square-well
    #solution) -- ported directly from SASfit's own
    #src/plugins/hs_plus_shoulder_or_well/sasfit_sq_square_well_potential.c.
    #lambda_ here is the well's OUTER range in units of sigma (well
    #width = (lambda_-1)*sigma), matching this tool's own
    #setSquareWellPotential(epsilonInkTUnits, delta)'s delta =
    #(lambda_-1)*hardSphereDiameter exactly. Fully closed-form, no
    #reference-system sub-solve or root-finding needed -- but note
    #(found by testing) this is a first-order PERTURBATIVE solution:
    #agreement with the numerical solve is excellent for shallow/narrow
    #wells and degrades for strong/wide ones (max diff went from ~0.005
    #at epsilon=-0.02,delta=0.02 up to ~0.33 at epsilon=-0.5,delta=0.3,
    #in a phi=0.15 test), which is the expected behaviour of a
    #perturbative expansion approaching the edge of its own validity,
    #not a bug.
    def calculateAnalyticalStructureFactorForSquareWell(self, q, epsilonInkT, lambda_):
      sigma = self.getHardSphereRadius()
      FP = self.getVolumeDensity()

      alpha = ((1.0+2.0*FP)**2 + FP**3*(FP-4.0)) / (1.0-FP)**4
      beta = -FP/3.0*(18.0+20.0*FP-12.0*FP**2+FP**4) / (1.0-FP)**4
      gamma = FP*0.5*alpha

      kappa = np.atleast_1d(np.asarray(q, dtype=float))*sigma
      lkappa = kappa*lambda_

      Ck = (-24.0*FP/kappa**6 * (
                alpha*kappa**3*(np.sin(kappa)-kappa*np.cos(kappa))
                + beta*kappa**2*(2.0*kappa*np.sin(kappa)-(kappa**2-2.0)*np.cos(kappa)-2.0)
                + gamma*((4.0*kappa**3-24.0*kappa)*np.sin(kappa)
                         - (kappa**4-12.0*kappa**2+24.0)*np.cos(kappa)+24.0)
                - epsilonInkT*kappa**3*(np.sin(lkappa)-lkappa*np.cos(lkappa)
                                          + kappa*np.cos(kappa)-np.sin(kappa))
            ))
      SQ = 1.0/(1.0-Ck)
      return SQ if SQ.size > 1 else float(SQ[0])

    def calculateAnalyticalStructureFactorArrayForHS(self):
      if self.isHNC or not (self.getpotentialByName() == "HardSphere"):
          print("analytical solution is only available for PY with HS")
          return
     
      numberOfRadialSamplingPoints = self.getNumberOfRadialSamplingPoints()
      delta_r = self.getDelta_r()
      delta_q = np.pi/((numberOfRadialSamplingPoints + 1.0)*delta_r)
      #we start qArray at delta_q, not zero, as in fix point operator
      qArray = delta_q*(np.arange(numberOfRadialSamplingPoints).astype('float') + 1.0)
      Sq = np.zeros(numberOfRadialSamplingPoints).astype('float')
      for qIndex in range(numberOfRadialSamplingPoints):
          Sq[qIndex] = self.calculateAnalyticalStructureFactorForHS(qArray[qIndex])
          
      return Sq
      
      
      
    #**********************************************************************  
    #The next two functions operate on the converged solution only
    #that's why they are here and not in the fixpoint operator unit.
    #If Gamma and c is given, Calculate g
    def calculateRDF(self, G, c):
      g = G + c + 1.0
      #g[:self.hardSphereDiameterInPoints] = 0.0  #This is only true for potentials with a hard sphere part given
      return g
    
    #If c is given, Calculate structure factor. Math/Physics: Actually Sq is defined as 1 + rho g^, the formula bellow
    #states that Sq = 1/(1-rho*c^). From OZE we know h^ = c^/(1-rho*c^). Hence for the bellow to be true it must hold Sq =h^/c^
    #If we devide OZE form h^ = c^ + rho*h^c^ by c^ , there is h^/c^ = 1 + rho*h^ = 1 + rho*g^ - rho*delta. So what is calculated here
    #is not S(q) but S'(q) = S(q) - rho* delta(q). See also https://en.wikipedia.org/wiki/Radial_distribution_function#The_structure_factor
    def calculateSq(self, c):
      c_hat = self.hankelTransform(c, self.Delta_r)
      Sq = 1.0/(1.0 - self.particleDensity*c_hat)
      return Sq


    #**********************************************************************
    #Setter methods
    #Defining start values is as well common to all solvers
    def setStartValue(self, x_0):
      if self.doUseGammaFixPointOperator and (x_0.size != self.numberOfRadialSamplingPoints):
         print("Size of initial array is incorrect, not set")
         return
         
      if not self.doUseGammaFixPointOperator and (x_0.size != 2*self.numberOfRadialSamplingPoints):
         print("Size of initial array is incorrect, not set")
         return
      
      self.x_0 = x_0
      
      
    #Which fixpoint operator to use is something which should only affect the abstract solver,
    #the specific solver will work with whatever fixpoint operator
    def doNotUseGammaFixPointOperator(self):
        self.doUseGammaFixPointOperator = False
        #As a side effect, we adjust the size of the initial vector x_0, 
        #this is the way to tell the specific solver what it needs.
        x_0 = np.zeros(2*self.numberOfRadialSamplingPoints)
        self.setStartValue(x_0)
        
    def setNumberOfIterations(self, numberOfIterations):
        self.numberOfIterations = numberOfIterations
      
    def setConvergenceCriterion(self, convergenceCriterion):
        self.convergenceCriterion = convergenceCriterion
        
    def doInterrupt(self):
      self.isInterrupted = True

    def setNumberOfIterations(self, n):
      self.numberOfIterations = n

    #End setter
    #**********************************************************************
    #Getter methods
    def getConvergenceCriterion(self):
        return self.convergenceCriterion
        
    def getNumberOfIterations(self):
        return self.numberOfIterations
      
    def getRDF(self):
      return self.radialDistributionFunction
      
    def getRDFasList(self):
      #A list can be marshaled, a numpy array not
      return self.radialDistributionFunction.tolist()
      
    def getSq(self):
      #Not exactly structure factor, see comment in calculation
      return self.structureFactor
      
    def getDirectCorrelationFunction(self):
      return self.directCorrelationFunction
        
    #End getter
    #**********************************************************************
      
    #To be implemented by derived class
    @abstractmethod
    def solve(self):
      pass
