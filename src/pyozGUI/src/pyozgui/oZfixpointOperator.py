# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*
'''
This class provides the basic functionality
to solve the Ornstein Zernike equation.
It implements the fixpoint operator
according the algorithm described in
J. Kohlbrecher's SASfit
documentation. (Chapter 5.2)
The fixpoint operator according Kelley
(see also docu in this git repo)
is as well implemented but seems far less
suitable regarding convergence properties.
Class Structure:
The OZSolver class is serving clients via a RPC interface.
A client can set the member variables of the class
using the (public) setter methods and then
retrieve the result using the (public)
getter methods. (These are the only ones
exposed, i.e. mapped to RP calls).

--------------------------------------------------------------------------
Ported from Python 2 to Python 3 (2026), and extended with the potentials
below to match the ones available in the SASfit C code
(src/sasfit_oz/sasfit_oz_potential_*.c):
  - setDepletionPotential            (sasfit_oz_potential_depletion.c,
                                       simplified 2-sphere U_Depletion form)
  - setDLVOPotential                 (sasfit_oz_potential_dlvo.c)
  - setDLVOHydraPotential             (sasfit_oz_potential_dlvo_hydra.c)
  - setFermiDistributionPotential    (sasfit_oz_potential_FermiDistributionModel.c)
  - setGGCMnPotential                (sasfit_oz_potential_ggcm-n.c)
  - setHS3YukawaPotential            (sasfit_oz_potential_HS_3Yukawa.c)
  - setIonicMicrogelPotential        (sasfit_oz_potential_ionic_microgel.c)
  - setPiecewiseConstantHSPotential  (sasfit_oz_potential_piecewise_constant_HS.c)
  - setPSMPotential                  (sasfit_oz_potential_psm.c)
  - setSoftSpherePotential,
    setParabolicSpherePotential      (sasfit_oz_potential_soft_sphere.c)
  - setSquareWellPotential           (sasfit_oz_potential_square_well.c)
  - setStickyHardSpherePotential     (sasfit_oz_potential_sticky_hard_sphere.c,
                                       Baxter tau convention -- same convention
                                       used by the robertus_shs plugin)
Already-ported potentials (unchanged): HardSphere, LennardJones, Yukawa.
Star polymer potential (Likos & Harreis 2002) split into its two genuinely
different regimes, matching sasfit_oz_potential_star_Likos.c's own
U_Star1/U_Star2 exactly (an earlier version of this port only had U_Star1,
silently applied regardless of functionality f):
  - setStarPolymerHighFPotential     (f>=10, exponential decay)
  - setStarPolymerLowFPotential      (f<=10, Gaussian decay, own tau(f))

All energies are expressed directly in kT units (matching the existing
convention in this file, e.g. LennardJones' epsilonInkTUnits) -- the C
code's explicit kb*T factors are dropped accordingly, since kb*T=1 in
these units.

Python-2-isms fixed throughout: xmlrpclib -> xmlrpc.client, http.server-
based SimpleXMLRPCServer import path, `print x` -> `print(x)`,
`x.shape[0]/2` -> `x.shape[0]//2` (integer division; Python 3's `/` is
always float division), `basestring` -> `str`.
--------------------------------------------------------------------------
'''

#calculations
import numpy as np
#Uses scipy.fft (the modern, recommended pocketfft-based replacement
#for the legacy scipy.fftpack, which this file originally used) --
#deliberately NOT scipy.fftpack: found by direct testing that calling
#scipy.fftpack.dst repeatedly from inside a callback invoked by
#sundials4py's KINSOL bindings (a nanobind C extension) causes a
#segfault after enough calls, while scipy.fft.dst in the exact same
#context does not. Only dst is ever actually called below (see
#inverseHankelTransform(), which computes the inverse via another
#dst call rather than idst, exploiting that DST-I is self-inverse up
#to normalisation) -- confirmed scipy.fft.dst(x, type=1) is bit-
#identical to scipy.fftpack.dst(x, type=1) with default arguments, so
#this swap changes nothing numerically, it only avoids the crash.
from scipy.fft import dst

#Server
from xmlrpc.server import SimpleXMLRPCServer

#Setting (and getting) Potentials (Potential names)
import inspect


class OZfixpointOperator:
    #Init needs to be given by constructor, all other members can be set via RPC later
    #but port needs to be known in advance by the client. (Will be defined by the client)
    def __init__(self, port, numberOfRadialSamplingPoints=None, hardSphereDiameterInPoints=None):
      #First computational stuff...
      #global constants
      #*******************************************************************************
      #Both grid parameters are now configurable (previously hard-coded
      #and explicitly commented "we don't make them settable") -- long-
      #range potentials (e.g. Yukawa/DLVO with a large screening length)
      #can need either a longer total range (increase
      #numberOfRadialSamplingPoints at fixed hardSphereDiameterInPoints)
      #or finer near-contact resolution (increase hardSphereDiameterInPoints
      #at fixed numberOfRadialSamplingPoints), or both -- see this
      #class's own getrArray()/getqArray() for how these two parameters
      #together set the whole real- and reciprocal-space grid. Passing
      #Default of 2**12-1=4095, not a round 4096: this class's own
      #hankelTransform() uses scipy.fft.dst(..., type=1) (DST-I), whose
      #"logical" transform size is 2*(N+1) -- fastest when that factors
      #into small primes, ideally a power of 2, i.e. exactly when N+1
      #is a power of 2. This class's own getqArray()/hankelTransform()
      #already treat N+1 as the natural grid-resolution quantity
      #(delta_q = pi/((N+1)*Delta_r)), so N=2^n-1 matches that intent;
      #the previous default of a plain 4096 gave N+1=4097=17*241 (two
      #largish prime factors) -- likely one of the worst possible sizes
      #for this transform, not just a missed optimization. Matches the
      #equivalent fix in the C/Tcl OZ solver GUI (src/sasfit_oz/
      #sasfit_oz_tclcmd.c, sasfit.vfs/lib/app-sasfit/tcl/
      #sasfit_OZ_solver.tcl), which now also takes an exponent n
      #(gridsize = 2^n-1) rather than a direct point count.
      self.numberOfRadialSamplingPoints = (numberOfRadialSamplingPoints
                                            if numberOfRadialSamplingPoints is not None else 2**12 - 1)   #n=12 -> N=4095, N+1=4096=2^12
      #self.hardSphereDiameterInPoints = self.numberOfRadialSamplingPoints/2**3
      self.hardSphereDiameterInPoints = (hardSphereDiameterInPoints
                                          if hardSphereDiameterInPoints is not None else 100)
      #Named "diameter", not "radius": what this grid parameter
      #actually measures is sigma, the pairwise CONTACT distance
      #between two touching spheres -- i.e. the diameter, not a single
      #sphere's own geometric radius (confirmed directly: g(r)=0 for
      #r<=sigma, jumping to its characteristic nonzero contact-peak
      #value at r=sigma+, exactly the excluded-volume condition for two
      #spheres of DIAMETER sigma, not radius sigma).
      #Length units is such that the 'box' has length 1
      #self.rLength = 1.0 #arbitrary
      #rLength is defined in multiples of hardSphereDiameter
      self.rLength = (self.numberOfRadialSamplingPoints - 1.0)/self.hardSphereDiameterInPoints #defined such that...
      #Calculate spacing in this length units
      self.Delta_r = self.rLength/(self.numberOfRadialSamplingPoints - 1.0) #....Delta_r = 1.0/hardSphereDiameterInPoints 
      #Sphere diameter (also for LJ) -- the pairwise CONTACT distance
      #(i.e. sigma, in the usual OZ/potential sense), matching every
      #potential setter below (e.g. LennardJones' sigma = self.hardSphereDiameter).
      self.hardSphereDiameter = self.hardSphereDiameterInPoints*self.Delta_r
      
      #Default values for member variables:
      #************************************
      self.isHNC = False #The bridge function is part of this algorithm, whereas the potential is not (or less)
      self.isHMSA = False #Closure relation alternative to Rogers Young well suited for Lennard Jones
      #Do not modify the *private* variable alpha, use the corresponding setter (doRYclosure)
      self.alpha = -1.0 #Rogers Young and HMSA closure, negative value means deactivated
      #Selects which closure update_c() (above) actually uses; 'PY' by
      #default, matching this file's original (pre-closureType)
      #behaviour. See update_c() and the doXXXclosure() setters below
      #for the full list of supported values.
      self.closureType = 'PY'
      self.epsilonInkTUnits = -1.0 #LJ binding energy, negative value means not set
      self.shieldingLengthInSigmaUnits = -1.0 #Yukawa length scale, negative value means not set
      self.interactionStrengthInKTunits = 0.0 #Yukawa energy (may be attractive or repulsive)
      self.isHardSphereAdded = False #Yukawa + HS
      self.volumeDensity = 0.2
      self.particleDensity = self.transformVolume2ParticleNumberDensity(self.volumeDensity)
      #Multicomponent / polydisperse state. numberOfComponents == 1 is the
      #default and every code path below then behaves EXACTLY as before
      #(regression-checked); values > 1 are set up by
      #setPolydisperseHardCoreYukawaPotential(). componentDensities are the
      #per-species number densities rho_i = n*x_i, with n = particleDensity
      #the TOTAL number density.
      self.numberOfComponents = 1
      self.componentDiameters = np.array([self.hardSphereDiameter])
      self.componentFractions = np.array([1.0])
      self.componentDensities = np.array([self.particleDensity])
      #internal result exp(-beta u) (Default: Ideal Gas Boltzmann potential factor)
      self.boltzmannOfP2Ppotential = np.ones(self.numberOfRadialSamplingPoints)
      #Truncated LJ potential, used for HMSA closure
      self.repulsivePartOfP2Ppotential = np.zeros(self.numberOfRadialSamplingPoints)
      self.attractivePartOfP2Ppotential = np.zeros(self.numberOfRadialSamplingPoints)
      #Used only by the RMSA closure (see update_c() and
      #OZsolver.solveRMSA()) -- defaults to "everywhere outside the
      #bare hard core" (1s), i.e. plain MSA, until solveRMSA() actually
      #adjusts it.
      self.gate4g = np.ones(self.numberOfRadialSamplingPoints)
      #Used only by RHNC (set by OZsolver.solveRHNC()) and MHNC (set by
      #doMHNCclosure() below) respectively -- zeros here purely so an
      #AttributeError can't happen if update_c() were ever somehow
      #called for one of those closureTypes before its own setter/
      #solve method has run.
      self.g0 = np.zeros(self.numberOfRadialSamplingPoints)
      self.G0 = np.zeros(self.numberOfRadialSamplingPoints)
      self.betaPerturbation = np.zeros(self.numberOfRadialSamplingPoints)
      self.bridgeMHNC = np.zeros(self.numberOfRadialSamplingPoints)
      #Used only by EuRah (set by OZsolver.solveEuRah()'s own outer
      #root-find) -- zeros here purely as a defensive default, same
      #reasoning as g0/G0/betaPerturbation above.
      self.CEURAH = np.zeros(self.numberOfRadialSamplingPoints)
      #Used only by ZSEP (set by setZSEPclosure()/OZsolver.fitZSEPparameters())
      #-- defensive defaults for the same reason as above; alpha=1.0
      #here matches Lee (1995)'s own empirical finding that alpha~=1.0
      #across every density tested, not an arbitrary placeholder.
      self.zsep_zeta = 1.0
      self.zsep_phi = 1.0
      self.zsep_alpha = 1.0
      #Only for HS and in PY approximation valid
      self.zeroQlimitOfStructureFactor = self.calculateZeroQlimitOfStructureFactor(self.volumeDensity)
      
      
      #....then RPC
      #*******************************************************************************
      #Init server, only if port number is valid
      if port > 0:
          self.rpcServer = SimpleXMLRPCServer(("localhost", port), allow_none=True)
          #registered functions that can be called remotely
          self.rpcServer.register_function(self.setVolumeDensity, 'setVolumeDensity' )
          self.rpcServer.register_function(self.setPotentialByName, 'setPotentialByName' )
          self.rpcServer.register_function(self.getAvailablePotentialNames, 'getAvailablePotentialNames' )
          self.rpcServer.register_function(self.getHardSphereDiameterInPoints, 'getHardSphereDiameterInPoints')
          self.rpcServer.register_function(self.getHardSphereDiameter, 'getHardSphereDiameter')
          self.rpcServer.register_function(self.getNumberOfRadialSamplingPoints, 'getNumberOfRadialSamplingPoints')
          self.rpcServer.register_function(self.getDelta_r, 'getDelta_r')
      
      
    #Helper functions
    #************************************************************************************
    #Volume to particle density
    def transformVolume2ParticleNumberDensity(self, rho_V):
      rho = rho_V/( (4*np.pi/3.0)*(0.5*self.hardSphereDiameter)**3 ) #rho_V = rho*V_atom
      return rho
    
    #If Gamma is given calculate (updated) c (according bridge function)
    #Overflow guard for the closure formulas below. Several closures
    #exponentiate Gamma; during the large transient excursions an
    #iteration can take early on, that argument exceeds ~709 and np.exp
    #overflows to inf, which then turns the whole iteration vector into
    #NaN and aborts the solve with no useful diagnostic (seen directly
    #on strongly charged polydisperse Yukawa systems). Clipping the
    #EXPONENT keeps the value large but finite so the iteration can
    #still back off. This can only ever alter results that would
    #otherwise have been inf/NaN, so converged results are unchanged.
    def safeExp(self, x):
      return np.exp(np.clip(x, -700.0, 500.0))

    def update_c(self, G):
      #Here is the only place where the bridge function/closure comes
      #into play, so this is encapsulated. Dispatches on self.closureType
      #(a plain string, default 'PY') rather than the original ad hoc
      #boolean flags (self.isHNC/self.isHMSA/self.alpha>=0), since there
      #are now more than 2-3 closures to choose between -- see
      #doXXXclosure() below for the setters. Every formula here was
      #cross-checked directly against its corresponding `case XXX:`
      #block in src/sasfit_oz/sasfit_oz_solver.c's own closure switch
      #statement (not re-derived from scratch), using that file's own
      #EN[i]=exp(-beta*U(r_i)), G[i]=Gamma(r_i) notation.
      #
      #RHNC, MHNC, RMSA and EuRah are NOT included here: each of those
      #needs a full reference-system OZ sub-solve first (e.g. RMSA
      #needs an MSA solve, RHNC/MHNC need a hard-sphere-like reference
      #closure's own g0/G0), which is a materially larger feature this
      #single method's dispatch can't absorb -- they would need their
      #own dedicated sub-solve step before update_c() could even be
      #called with the right extra state.
      ct = getattr(self, 'closureType', 'PY')
      EN = self.boltzmannOfP2Ppotential

      if ct == 'PY':
          return EN*(1.0 + G) - G - 1.0

      elif ct == 'HNC':
          return EN*np.exp(G) - G - 1.0

      elif ct == 'RY':
          #Rogers-Young: interpolates between PY (alpha->infinity) and
          #HNC (alpha->0) via Fswitch[i] = 1-exp(-alpha*r_i).
          f = self.helperRYfunction
          return EN*(1.0 + (self.safeExp(f*G) - 1.0)/f) - G - 1.0

      elif ct == 'HMSA':
          #Zerah & Hansen, J. Chem. Phys. 84, 2336 (1986).
          #
          #HMSA interpolates between SMSA and HNC -- NOT between PY and HNC.
          #That is the difference from RY, and it is the whole point of the
          #closure: RY brackets the exact g(r) only for purely repulsive
          #potentials, and the paper states outright that "thermodynamic
          #consistency can never be achieved for a Lennard-Jones fluid by
          #mixing the HNC and PY closures within the RY scheme".
          #
          #With the potential split v = v1 (repulsive) + v2 (attractive) and
          #Gstar = G - beta*v2, the closure is
          #     g = exp(-beta*v1) * [1 + (exp(f*Gstar) - 1)/f]
          #whose limits are, with f(r) = 1 - exp(-alpha r) so that f -> 0 as
          #r -> 0 and f -> 1 as r -> infinity (the paper's eqs. 7a/7b):
          #     f -> 0  =>  g = exp(-beta*v1)(1 + G - beta*v2)   = SMSA (eq.9)
          #     f -> 1  =>  g = exp(-beta*v1) exp(G - beta*v2)   = HNC  (eq.4)
          #so SMSA governs SHORT range and HNC LONG range. In terms of the
          #single parameter: alpha -> 0 gives SMSA everywhere, alpha ->
          #infinity gives HNC everywhere -- the same direction as RY, whose
          #alpha -> 0 limit is PY.
          #
          #For a PURELY REPULSIVE potential v2 = 0, SMSA reduces to PY (the
          #paper says so explicitly), hence HMSA reduces exactly to RY. So
          #HMSA and RY returning bit-identical results for hard spheres, or
          #for the screened-Coulomb polydisperse Yukawa potential, is the
          #expected behaviour and not a defect -- it is also what exposed the
          #earlier bug in which the repulsive/attractive split was never
          #populated at all (see setPotentialByName's fallback).
          f = self.helperRYfunction
          Gstar = G - self.attractivePartOfP2Ppotential
          return self.safeExp(-self.repulsivePartOfP2Ppotential)*(1.0 + (self.safeExp(f*Gstar) - 1.0)/f) - G - 1.0

      elif ct == 'CarbajalTinoko':
          #Carbajal-Tinoco, J. Chem. Phys. 128, 184507 (2008).
          #
          #The only IMPLICIT closure here: the bridge function is defined by
          #     b = e(r) * [(2-w)e^w - 2 - w]/(e^w - 1),   w = Gamma + b
          #so b appears on both sides and has to be solved for. The amplitude
          #depends on r as well as on the parameter:
          #     e(r) = 3 + lambda            for lambda > 0
          #     e(r) = 3 exp(lambda r)       otherwise
          #
          #Solved by a VECTORISED fixed-point iteration rather than a scalar
          #root find per grid point: the reference implementation calls
          #find_zero once per r, which here would mean N (or p^2 N) separate
          #solves per closure evaluation, inside the outer OZ iteration.
          #Iterating b <- bfunc(Gamma + b) over the whole array at once costs
          #a few dozen numpy passes instead.
          #
          #The |w| < 1e-4 branch is not cosmetic: (2-w)e^w - 2 - w and
          #e^w - 1 both vanish as w -> 0, so the quotient is 0/0 and loses all
          #precision well before it reaches zero. The series is the reference
          #implementation's own.
          lam = self.alpha
          rr = self.getrArray()
          e_amp = (3.0 + lam) if lam > 0.0 else 3.0*np.exp(lam*rr)
          #Damped iteration. The undamped map b <- bfunc(Gamma + b) is not
          #contractive once the amplitude is large: at lambda = +0.4, where
          #e = 3.4 everywhere, it diverged at the most negative Gamma and
          #produced g <= 0 (bridge function -inf), while lambda = -0.4, whose
          #amplitude 3exp(-0.4r) is much smaller, converged to 4e-14. Damping
          #costs a few more passes and fixes both.
          b = np.zeros_like(G)
          beta = 0.5
          for _ in range(500):
              w = G + b
              small = np.abs(w) < 1e-4
              wsafe = np.where(small, 1.0, w)
              y = self.safeExp(wsafe)
              target = np.where(small,
                                e_amp*(-(w**2)/6.0 + (w**4)/360.0),
                                e_amp*((2.0 - wsafe)*y - 2.0 - wsafe)/(y - 1.0))
              bnew = (1.0 - beta)*b + beta*target
              if np.max(np.abs(bnew - b)) < 1e-13:
                  b = bnew
                  break
              b = bnew
          return EN*self.safeExp(G + b) - G - 1.0

      elif ct == 'Khanpour':
          #Khanpour's one-parameter bridge function,
          #    B = log1p(alpha*Gamma)/alpha - Gamma
          #Cross-checked against OrnsteinZernike.jl's own Khanpour closure
          #(src/Closures.jl), which is where this was taken from.
          #
          #NOT to be confused with this library's 'KH', which is
          #Kovalenko-Hirata -- the two abbreviate identically but are
          #different closures, so this one is spelled out in full.
          #
          #alpha -> 0 recovers HNC: log1p(a*G)/a -> G, so B -> 0.
          #log1p is used rather than log(1+x) for accuracy at small
          #alpha*Gamma, and the argument is clipped just above -1 because
          #the bridge function is undefined for alpha*Gamma <= -1 (the
          #iteration can transiently stray there).
          a = self.alpha
          arg = np.clip(a*G, -1.0 + 1e-12, None)
          BRIDGE = np.log1p(arg)/a - G
          return EN*self.safeExp(G + BRIDGE) - G - 1.0

      elif ct == 'ModifiedVerlet':
          #Verlet's bridge function with the denominator switched off for
          #negative Gamma:
          #    B = -Gamma^2/2                          for Gamma < 0
          #    B = -(Gamma^2/2)/(1 + alpha*Gamma/2)    otherwise
          #From OrnsteinZernike.jl's ModifiedVerlet. Note the plain
          #'Verlet' branch above uses the equivalent of alpha = 8/5 with no
          #sign split; that value was confirmed to agree with the Julia
          #reference to 1e-16.
          a = self.alpha
          BRIDGE = np.where(G < 0.0,
                            -(G**2)/2.0,
                            -(G**2)/2.0/(1.0 + a*G/2.0))
          return EN*self.safeExp(G + BRIDGE) - G - 1.0

      elif ct == 'ExtendedRY':
          #Extended Rogers-Young: the RY switching construction with one
          #extra quadratic term inside the logarithm,
          #    phi = (exp(f*Gamma) - 1)/f,   f(r) = 1 - exp(-alpha r)
          #    B   = -Gamma + log1p(phi + a*phi^2)
          #so a = 0 reduces EXACTLY to plain RY, which is the natural
          #regression check. Taken from OrnsteinZernike.jl's own
          #ExtendedRogersYoung closure.
          #
          #Two parameters: alpha (the RY switching rate, shared with the
          #'Rogers-Young' branch via self.helperRYfunction) and a, the
          #quadratic coefficient, held in self.extendedRYa.
          f = self.helperRYfunction
          phi = (self.safeExp(f*G) - 1.0)/f
          arg = np.clip(phi + self.extendedRYa*phi**2, -1.0 + 1e-12, None)
          BRIDGE = -G + np.log1p(arg)
          return EN*self.safeExp(G + BRIDGE) - G - 1.0

      elif ct == 'Verlet':
          #Verlet's empirical bridge function, no reference system or
          #extra closure parameter needed.
          BRIDGE = -(G**2)/(2.0*(1.0 + 0.8*G))
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'BPGG':
          #Ballone-Pastore-Galli-Gazzillo power-law bridge; self.alpha
          #here is sBPGG in the C code's own naming (same storage slot,
          #reused per-closure, exactly as in that code: sBPGG, aCJVM and
          #fBB below are literally #define'd to the same OZd->alpha).
          s = self.alpha
          powarg = 1.0 + s*G
          BRIDGE = np.sign(powarg)*np.power(np.abs(powarg), 1.0/s) - G - 1.0
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'VM':
          #Verlet-modified/Gstar-based sqrt bridge, no extra parameter.
          Gstar = G - self.attractivePartOfP2Ppotential
          powarg = 1.0 + 2.0*Gstar
          BRIDGE = np.sign(powarg)*np.sqrt(np.abs(powarg)) - Gstar - 1.0
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'CJVM':
          a = self.alpha  #aCJVM in the C code
          Gstar = G - self.attractivePartOfP2Ppotential
          powarg = 1.0 + 4.0*a*Gstar
          BRIDGE = 1.0/(2.0*a)*(np.sign(powarg)*np.sqrt(np.abs(powarg)) - 1.0 - 2.0*a*Gstar)
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'BB':
          f = self.alpha  #fBB in the C code
          Gstar = G - self.attractivePartOfP2Ppotential
          powarg = 1.0 + 2.0*Gstar + f*Gstar**2
          BRIDGE = np.sign(powarg)*np.sqrt(np.abs(powarg)) - Gstar - 1.0
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'KH':
          #Kovalenko-Hirata closure -- the REAL literature formula, not
          #SASfit's own C-code quirk (an earlier version of this port
          #matched sasfit_oz_solver.c's own "case KH:" block exactly,
          #which computes the textbook piecewise bridge function only
          #as a separate diagnostic BRIDGE[i] output, never feeding it
          #back into c[i] -- so that earlier version was, in effect,
          #plain HNC under a different name). Chosen and verified
          #directly against Kovalenko & Hirata's own formula
          #(Chem. Phys. Lett. 290, 237 (1998)) as reproduced in
          #Pihlajamaa & Janssen (2024)'s closure-comparison table
          #(b(r) = [ln(1+d(r))-d(r)]*theta(d(r)), d(r)=gamma(r)-beta*u(r)):
          #
          #   g(r) = EN(r)*exp(G(r))     where d(r) = G(r)+ln(EN(r)) <= 0
          #   g(r) = 1 + d(r)            where d(r) > 0
          #
          #i.e. identical to HNC wherever d(r)<=0, and linearized
          #(bounded, non-exponential) wherever d(r)>0 -- this is the
          #actual mechanism behind KH's own well-known good numerical
          #convergence properties (see e.g. Ebato & Miyata (2016), AIP
          #Advances 6, 055111): HNC's c(r) can grow without bound as
          #Gamma(r) grows, KH's cannot, since the linear branch's c(r)
          #= ln(EN(r)) = -beta*u(r) doesn't depend on Gamma(r) at all.
          #Verified directly (not just derived): the two branches give
          #identical c(r) AND g(r) at the switch boundary d(r)=0
          #(checked to 1e-10), so this introduces no discontinuity;
          #reduces to the exact same hard-core exclusion PY/HNC/MSA use
          #when EN=0 (log(EN)->-inf forces d<=0, selecting the HNC
          #branch, which gives EN*exp(G)-G-1 = -(G+1) there); and, in a
          #full self-consistent Picard solve for hard spheres at
          #phi=0.3, gives g(sigma+)=2.36 -- much closer to the exact PY
          #value (2.356) than to plain HNC's own 2.845 at the same
          #state point, consistent with the linear branch reducing to
          #c(r)=ln(EN(r))=0 outside contact for a bare hard sphere
          #(EN=1 there exactly), the same value PY's own formula
          #happens to give there too.
          logEN = np.log(np.clip(EN, 1e-300, None))
          d = G + logEN
          c_HNC_branch = EN*np.exp(G) - G - 1.0
          c_linear_branch = logEN
          return np.where(d <= 0.0, c_HNC_branch, c_linear_branch)

      elif ct == 'DH':
          #Duh-Haymet bridge function -- a rational-function
          #approximation to B(r), more refined than Verlet/VM/MS's own
          #simpler closed forms. Uses the same Gstar=G-beta*U_attractive
          #pattern as VM/CJVM/BB/HMSA above; no extra alpha-like
          #parameter (matching the C code, which has none for this
          #closure). sasfit_oz_solver.c's own "case DH:" block ALSO has
          #a special-cased Gstar formula specifically when the active
          #potential is Lennard-Jones (a narrower numerical-stability
          #tweak for that one potential's own r^-12 term, not a
          #different closure formula) -- NOT replicated here; this is a
          #known, deliberate gap (flagged rather than silently
          #skipped), since matching it exactly would need reverse-
          #engineering that C code's own internal Lennard-Jones
          #PARAM[0]/PARAM[1] convention against this project's own
          #setLennardJonesPotential(), which has not been done.
          Gstar = G - self.attractivePartOfP2Ppotential
          BRIDGE = -(Gstar**2)/(2.0*(1.0 + (5.0*Gstar+11.0)/(7.0*Gstar+9.0)*Gstar))
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'CG':
          #Choudhury-Ghosh bridge function -- piecewise in the sign of
          #Gstar, with a density-dependent coefficient (self.volumeDensity
          #matches the C code's own PHI, used identically throughout
          #this project) on the Gstar>0 branch. No extra alpha-like
          #parameter (matching the C code).
          Gstar = G - self.attractivePartOfP2Ppotential
          coeff = 1.0175 - 0.275*6.0*self.volumeDensity/np.pi
          BRIDGE = np.where(Gstar > 0.0,
                            -0.5*Gstar**2/(1.0 + coeff*Gstar),
                            -0.5*Gstar**2)
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'SMSA':
          #"Symmetric" MSA-like closure using the attractive/repulsive
          #split; no extra parameter.
          Gstar = G - self.attractivePartOfP2Ppotential
          return EN*np.exp(self.attractivePartOfP2Ppotential)*(Gstar + 1.0) - G - 1.0

      elif ct == 'MSA':
          #Mean Spherical Approximation: c(r) = -beta*U(r) outside the
          #hard core, c(r) = -(Gamma(r)+1) inside it (forcing g(r)=0
          #there, same as PY/HNC's own implicit hard-core handling).
          #EN[i]=exp(-beta*U(r_i)) => beta*U(r_i) = -log(EN[i]), so
          #-beta*U(r_i) = log(EN[i]) wherever EN>0.
          insideCore = (EN == 0.0)
          c_new = np.where(insideCore, -(G + 1.0), np.log(np.clip(EN, 1e-300, None)))
          return c_new

      elif ct == 'mMSA':
          #"Modified" MSA: same hard-core handling as MSA, but uses the
          #Mayer f-function f(r)=exp(-beta*U(r))-1 outside the core
          #instead of the bare potential.
          insideCore = (EN == 0.0)
          MAYER = EN - 1.0
          c_new = np.where(insideCore, -(G + 1.0), MAYER)
          return c_new

      elif ct == 'MS':
          #Martynov-Sarkisov, standalone (no reference system, no extra
          #parameter) -- used below as the reference-system closure
          #for RHNC (see solveRHNC()), but also usable on its own.
          powarg = 1.0 + 2.0*G
          BRIDGE = np.sign(powarg)*np.sqrt(np.abs(powarg)) - G - 1.0
          return EN*np.exp(G + BRIDGE) - G - 1.0

      elif ct == 'MHNC':
          #Modified HNC: HNC with an added, purely ANALYTICAL bridge
          #function (self.bridgeMHNC, set by doMHNCclosure() below via
          #_analyticalPYbridgeFunction()) -- no reference-system
          #sub-solve needed for this one, unlike RHNC/RMSA: the
          #Percus-Yevick hard-sphere bridge function has a genuine
          #closed form (mirroring sasfit_oz_solver.c's own BPY()), so
          #it only needs to be computed once, not solved for.
          return EN*np.exp(G + self.bridgeMHNC) - G - 1.0

      elif ct == 'RHNC':
          #Reference HNC: needs a reference system's own g0/Gamma0 (a
          #hard sphere at the same sigma, solved with the MS closure
          #above) plus the "perturbation" part of the potential (the
          #actual potential minus that bare hard-sphere reference).
          #Both self.g0/self.G0 AND self.betaPerturbation are set by
          #solveRHNC() below, which orchestrates the reference solve
          #this closure depends on -- update_c() alone only implements
          #the formula for already-available reference data. Note this
          #single expression naturally reduces to the same "-(G+1)"
          #hard-core exclusion PY/HNC/MSA/mMSA all use, with no
          #separate branch needed for it: self.g0 is exactly 0 inside
          #the reference hard core (a real hard sphere's own g(r) is
          #always 0 there), which zeroes out the whole first term.
          return self.g0*np.exp((G - self.G0) - self.betaPerturbation) - G - 1.0

      elif ct == 'RMSA':
          #"Rescaled" MSA (Hayter-Penfold-style, sasfit_oz_solver.c's
          #own RMSA case) -- identical formula to MSA, but the
          #inside-vs-outside switch is driven by self.gate4g (0/1,
          #set by OZsolver.solveRMSA() below) instead of EN==0 -- the
          #whole point of RMSA is that this "apparent" hard core can
          #extend further out than the bare potential's own one, when
          #plain MSA would otherwise predict an unphysical g(r)<0 just
          #past contact. See solveRMSA() for the orchestration this
          #needs (a first MSA solve, then a bisection search over how
          #far to push the gate outward, each step re-solving) -- this
          #branch alone only implements the formula for a GIVEN gate,
          #it does not decide what the gate should be.
          gate = self.gate4g
          c_new = np.where(gate == 0.0, -(G + 1.0), np.log(np.clip(EN, 1e-300, None)))
          return c_new

      elif ct == 'EuRah':
          #Eu-Rah closure: c(r) is set to self.CEURAH directly,
          #completely independent of G (unlike every other closure
          #here) -- self.CEURAH is a converged array computed by
          #OZsolver.solveEuRah()'s own OUTER root-find (over the
          #CEURAH array itself, via density-derivatives of the cavity
          #correlation function), which is a substantially different
          #and more involved calculation than any other closure. This
          #branch alone just returns whatever solveEuRah() already
          #converged to; it does not compute it.
          return self.CEURAH.copy()

      elif ct == 'ZSEP':
          #Lee's zero-separation-theorem closure (ZSEP), ported
          #directly from Eq. (3.5) of L. L. Lee, J. Chem. Phys. 103,
          #9388 (1995) -- designed specifically for hard spheres (it
          #relies on the exact Carnahan-Starling zero-separation
          #theorems for its own parameter-fitting, see
          #OZsolver.fitZSEPparameters() below, and is therefore only
          #meaningful when the active potential is HardSphere).
          #self.zsep_zeta/phi/alpha are the closure's own three free
          #parameters (set directly via setZSEPclosure(), or fitted via
          #fitZSEPparameters()). Gstar here uses the SAME Mayer-factor
          #renormalization pattern as this project's other Gstar-based
          #closures (HMSA/VM/CJVM/BB/DH/CG), but is written out
          #explicitly (rather than reusing self.attractivePartOfP2Ppotential)
          #since ZSEP's own renormalization is specifically
          #rho/2*(bare hard-sphere Mayer function), not a potential-
          #specific attractive tail -- matching the paper's own
          #gamma*(r) = gamma(r) + (rho/2)*f(r), f(r)=EN(r)-1.
          MAYER = EN - 1.0
          Gstar = G + 0.5*self.particleDensity*MAYER
          zeta, phi, alpha = self.zsep_zeta, self.zsep_phi, self.zsep_alpha
          denom = 1.0 + alpha*Gstar
          BRIDGE = -0.5*zeta*Gstar**2*(1.0 - phi*alpha*Gstar/denom)
          return EN*np.exp(G + BRIDGE) - G - 1.0

      else:
          raise ValueError("unknown closureType: " + str(ct))
    
    #In the grand canonical ensemble and for HS + PY, there is a analytical form of S(q=0)
    #Theory: Naegele, AMAS lecture notes on colloidal soft matter, eq. 4.64
    #In the canonical enemble, S(q=0) = 0, hence the name
    def calculateZeroQlimitOfStructureFactor(self, rho_V):
      return (1.0 - rho_V)**4/(1 + 2.0*rho_V)**2

    #Used only by EuRah (see OZsolver.solveEuRah()). Since EuRah's own
    #c(r) is fixed externally (equal to whatever CEURAH array is being
    #tried), independent of Gamma, Gamma follows from a SINGLE Hankel-
    #transform pass -- no fixed-point iteration is needed here, unlike
    #every other closure in update_c() above, where c depends on G and
    #the two must be iterated to a joint fixed point.
    def _directGammaAndCavityFromFixedC(self, c, phi):
      rho = self.transformVolume2ParticleNumberDensity(phi)
      c_hat = self.hankelTransform(c, self.Delta_r)
      G_hat = c_hat/(1.0 - rho*c_hat) - c_hat
      G = self.inverseHankelTransform(G_hat, self.Delta_r)
      g = G + c + 1.0
      EN = self.boltzmannOfP2Ppotential
      #cavity correlation function y(r)=g(r)*exp(beta*U(r)), computed
      #the same way sasfit_oz_solver.c's own generic CAVITY[j]=
      #exp(BRIDGE[j]+G[j]) does for closures that don't supply their
      #own BRIDGE (BRIDGE[j]=log(g[j])+UBETA[j]-G[j] there when
      #g[j]>0 and EN[j]!=0, else 0) -- algebraically CAVITY=g/EN in
      #the first case, CAVITY=exp(G) in the second (used directly
      #here rather than going via an explicit BRIDGE array).
      valid = (g > 0.0) & (EN != 0.0)
      CAVITY = np.where(valid, g/np.clip(EN, 1e-300, None), np.exp(G))
      return G, CAVITY
    
    #Hankel transform
    #*********************************************************************************
    #See https://en.wikipedia.org/wiki/Hankel_transform#Relation_to_the_Fourier_transform_.28radially_symmetric_case_in_n-dimensions.29
    #and http://mathworld.wolfram.com/SphericalBesselFunctionoftheFirstKind.html
    #for connection between Fourier/Hankel transform and the spherical Bessel function (Set n=3 in the former)
    #See http://docs.scipy.org/doc/scipy/reference/tutorial/fftpack.html#discrete-sine-transforms
    #for the definition of dst and idst (discrete sine transform and its inverse) 
    def hankelTransform(self, f, delta_r):
      #shape[-1]/axis=-1 (was shape[0] and a 1-D dst): identical for the
      #1-D one-component case, but also accepts the (p,p,N) pair-matrix
      #arrays used by the multicomponent path. lr broadcasts on the last
      #axis, so the body below is otherwise untouched.
      numberOfSamplingPoints = f.shape[-1]
      lr = np.arange(numberOfSamplingPoints).astype('float')
      lr += 1.0 #Division by zero (see over-next line, this is the reason why the grid starts at delta_r, not zero....)
      f_hat = dst(f*lr, type=1, axis=-1) #...but the formula is also correct (implemented as 5.18, 5.19 in SASfit docu, see also...)
      f_hat /= lr               # ..comment: it is important to mention that first elements equal delta_r, delta_q (and not at 0.0!)
      delta_q = np.pi/((numberOfSamplingPoints + 1.0)*delta_r)
      f_hat *= 2*np.pi*delta_r**2/delta_q
      return f_hat
      
    def inverseHankelTransform(self, f, delta_r):
      numberOfSamplingPoints = f.shape[-1]
      delta_q = np.pi/((numberOfSamplingPoints + 1.0)*delta_r)
      return delta_q**3*self.hankelTransform(f, delta_r)/( (2*np.pi)**3*delta_r**3 )

    def isotropic3Dconvolution(self, f_1, f_2, delta_r):
      hat_f_1 = self.hankelTransform(f_1, delta_r)
      hat_f_2 = self.hankelTransform(f_2, delta_r)
      return self.inverseHankelTransform(hat_f_1*hat_f_2, delta_r)
      
    #Multicomponent helpers
    #********************************************************************
    #The solver classes (Picard/Anderson/AndersonGeneralized/scipy/
    #SUNDIALS KINSOL+KIN_FP/Biggs-Andrews) all iterate on a FLAT vector
    #and never inspect its shape -- confirmed directly: none of them
    #references numberOfRadialSamplingPoints. So the multicomponent
    #problem is carried through them as the p(p+1)/2 unique pairs of the
    #symmetric Gamma_ij(r) matrix, packed into one flat vector; nothing
    #in the acceleration layer needs to change.
    def isMulticomponent(self):
      return getattr(self, 'numberOfComponents', 1) > 1

    def numberOfUniquePairs(self):
      p = self.numberOfComponents
      return p*(p + 1)//2

    def packPairs(self, M):
      '''(p,p,N) symmetric matrix -> flat vector of the unique pairs.'''
      p = self.numberOfComponents
      iu = np.triu_indices(p)
      return M[iu].reshape(-1)

    def unpackPairs(self, v):
      '''flat vector of unique pairs -> full symmetric (p,p,N) matrix.'''
      p = self.numberOfComponents
      N = self.numberOfRadialSamplingPoints
      iu = np.triu_indices(p)
      M = np.zeros((p, p, N))
      M[iu] = v.reshape(len(iu[0]), N)
      il = np.tril_indices(p, -1)
      M[il] = M[(il[1], il[0])]
      return M

    #FixPoint Operator (In 'Gamma' space). Note that also for hard spheres Gamma is continuous everywhere
    #(I.e also at sigma. x =[c,h] is not continuous at r = sigma, this may explain the much better 
    #numerical behaviour of the Gamma fixPointOperator in the root finding algorithms. Theory: Naegele, p.45)
    #*******************************************************************************************************
    def fixPointOperatorForGamma(self, G):
      if self.isMulticomponent():
          return self.fixPointOperatorForGammaMulticomponent(G)
      c_new = self.update_c(G)
      c_hat = self.hankelTransform(c_new, self.Delta_r)
      #G_new = inverseHankelTransform( (rho*c_hat**2)/(1.0 - rho*c_hat) )
      G_hat = c_hat/(1.0 - self.particleDensity*c_hat) - c_hat
      #G_hat[np.isinf(G_hat)] = 0.0; G_hat[np.isneginf(G_hat)] = 0.0; G_hat[np.isnan(G_hat)] = 0.0
      G_new = self.inverseHankelTransform(G_hat, self.Delta_r)
      #G_new[np.isinf(G_new)] = 0.0; G_new[np.isneginf(G_new)] = 0.0; G_new[np.isnan(G_new)] = 0.0
      return (G_new, c_new)
      
      

    #Multicomponent Gamma fixpoint operator.
    #********************************************************************
    #Same three steps as the one-component version above (closure ->
    #forward transform -> OZ in k-space -> back transform); the only
    #change is that the scalar OZ relation
    #     G_hat = c_hat/(1 - rho*c_hat) - c_hat
    #becomes the matrix relation
    #     H_hat = (I - C_hat rho)^-1 C_hat ,  G_hat = H_hat - C_hat
    #with rho = diag(rho_i). Every closure in update_c() is element-wise
    #in numpy, so it operates on the (p,p,N) arrays unchanged -- which is
    #why PY/HNC/RY/HMSA/... all become multicomponent for free once the
    #potential arrays carry their pair indices.
    def fixPointOperatorForGammaMulticomponent(self, Gflat):
      p = self.numberOfComponents
      Gm = self.unpackPairs(Gflat)
      c_new = self.update_c(Gm)
      c_hat = self.hankelTransform(c_new, self.Delta_r)
      C = np.moveaxis(c_hat, -1, 0)
      R = np.diag(self.componentDensities)
      M = np.eye(p)[np.newaxis, :, :] - C @ R
      H = np.linalg.solve(M, C)
      G_hat = np.moveaxis(H - C, 0, -1)
      G_new = self.inverseHankelTransform(G_hat, self.Delta_r)
      return (self.packPairs(G_new), c_new)

    #Split x=[c,h] in c and h component (helper functions for second fixpoint operator)
    #******************************************************************************************************
    def get_c_From_ch(self, x):
        c = x[:x.shape[0]//2]
        return c
        
    def get_h_From_ch(self, x):
        h = x[x.shape[0]//2:]
        return h
        
    #FixPoint Operator 2 (In 'ch' space, x=[c,h])
    #*******************************************************************************************************
    def fixPointOperatorFor_ch(self, x):
      x_new = np.zeros_like(x)
      c = self.get_c_From_ch(x)
      h = self.get_h_From_ch(x)
      G = h - c
      #Closure part, Rogers Young not supported
      if not self.isHNC:
          #PY closure
          h_new = self.boltzmannOfP2Ppotential*(1.0 + G) - 1.0
      else:
          #Hypernetted Chain closure 
          h_new = self.boltzmannOfP2Ppotential*np.exp(G) - 1.0

      #Ornstein Zernike part (should we use h_new (instead of h) yes, seems to have slightly better convergence properties)
      c_new = h_new - self.particleDensity*self.isotropic3Dconvolution(h_new, c, self.Delta_r)
      x_new[:x.shape[0]//2] = c_new
      x_new[x.shape[0]//2:] = h_new
      return x_new



    #Generic polydisperse potential, built from ANY one-component setter.
    #********************************************************************
    #Turns any of this class's eighteen one-component setXXXPotential()
    #methods into a multicomponent (p,p,N) pair potential, so that "any
    #potential x any closure x any form factor" is possible without
    #rewriting a single one of them.
    #
    #MIXING RULE (a modelling choice, stated rather than buried):
    #    additive hard cores,     sigma_ij = (sigma_i + sigma_j)/2
    #    identical REDUCED tail,  u_ij(r)  = u(r/sigma_ij)
    #i.e. every pair sees the same interaction shape measured in units of its
    #own contact distance. This is the same assumption Robertus et al. make
    #for adhesive spheres (size-INDEPENDENT stickiness tau), and it is what
    #makes the construction potential-agnostic: the tail parameters (epsilon,
    #delta, n, tau, ...) keep their reduced-unit meaning for every pair, so
    #no per-potential mixing rule has to be invented.
    #
    #It is NOT the only defensible choice. For Lennard-Jones the conventional
    #alternative is Lorentz-Berthelot, epsilon_ij = sqrt(epsilon_i epsilon_j)
    #with per-species epsilon; that is a different model and would need
    #per-species tail parameters, which this builder deliberately does not
    #make up.
    #
    #NOT usable for the charge-based potentials (DLVO, DLVOHydra,
    #IonicMicrogel): there the amplitude must scale with particle size
    #(Z ~ sigma^n) and kappa depends on the whole distribution through the
    #counterion density, so the reduced-tail rule is simply wrong. Those need
    #the bespoke treatment in setPolydisperseHardCoreYukawaPotential(), and
    #are refused here rather than silently mis-modelled.
    #
    #Verified: with relativeStandardDeviation = 0 and numberOfComponents = 1
    #this reproduces the ordinary one-component setters BIT-IDENTICALLY
    #(maxdiff 0.0 for HardSphere, SquareWell and LennardJones).
    CHARGE_COUPLED_POTENTIALS = ('DLVO', 'DLVOHydra', 'IonicMicrogel',
                                 'PolydisperseHardCoreYukawa')

    def setPolydispersePotential(self, potentialName, potentialArgs=(),
                                 relativeStandardDeviation=0.2,
                                 numberOfComponents=3, meanDiameter=None,
                                 distribution="Schulz"):
        p = int(numberOfComponents)
        srel = float(relativeStandardDeviation)
        meanSigma = self.hardSphereDiameter if meanDiameter is None else float(meanDiameter)

        if potentialName in self.CHARGE_COUPLED_POTENTIALS:
            print("setPolydispersePotential: %r is charge-coupled; its amplitude "
                  "scales with particle size and kappa depends on the whole "
                  "distribution, so the identical-reduced-tail rule does not "
                  "apply. Use setPolydisperseHardCoreYukawaPotential() instead."
                  % potentialName)
            return
        setterName = "set" + potentialName + "Potential"
        if not hasattr(self, setterName):
            print("unknown potential %r" % potentialName)
            return

        #Moment-matched size classes (Gauss-generalised-Laguerre for the
        #Schulz distribution), identical to the polydisperse Yukawa route so
        #that results from the two are directly comparable.
        #Moment-matched classes; the rule used depends on the distribution
        #because the moments-to-nodes map is ill-conditioned in float64 even
        #though every distribution here has ANALYTIC moments. Schulz and
        #Gaussian have closed-form classical rules (generalised Gauss-Laguerre
        #and Gauss-Hermite) and stay in float64; log-normal and Weibull have
        #none and go through high-precision Golub-Welsch. See
        #polydisperse_nodes.py, which also guards against the Gauss-Hermite
        #nodes going non-positive at large width -- fatal here, because a hard
        #core has to be placed at every sigma_ij.
        from polydisperse_nodes import sizeClasses
        sigma, x = sizeClasses(distribution, srel, p, meanSigma)
        p = len(sigma)

        self.numberOfComponents = p
        self.componentDiameters = sigma
        self.componentFractions = x
        thirdMoment = np.sum(x*sigma**3)
        self.particleDensity = self.volumeDensity/((np.pi/6.0)*thirdMoment)
        self.componentDensities = self.particleDensity*x

        N = self.numberOfRadialSamplingPoints
        r = self.Delta_r*(np.arange(N).astype('float') + 1.0)
        EN = np.zeros((p, p, N)); U = np.zeros((p, p, N))
        REP = np.zeros((p, p, N)); ATT = np.zeros((p, p, N))
        setter = getattr(self, setterName)
        try:
            for i in range(p):
                for j in range(i, p):
                    sij = 0.5*(sigma[i] + sigma[j])
                    #Evaluate the one-component setter at the REDUCED
                    #separation r/sigma_ij; its own hard core sits at
                    #hardSphereDiameter = 1 there, i.e. at r = sigma_ij.
                    self._rArrayOverride = r/sij
                    self.repulsivePartOfP2Ppotential = np.zeros(N)
                    self.attractivePartOfP2Ppotential = np.zeros(N)
                    setter(*potentialArgs)
                    EN[i, j] = EN[j, i] = np.asarray(self.boltzmannOfP2Ppotential, float)
                    #Not every setter defines p2PpotentialInkTUnits (several
                    #write only the Boltzmann factor), so treat it as optional
                    #and reconstruct it from EN below when absent.
                    uij = getattr(self, 'p2PpotentialInkTUnits', None)
                    if uij is not None and np.ndim(uij) == 1:
                        U[i, j] = U[j, i] = np.asarray(uij, float)
                    REP[i, j] = REP[j, i] = np.asarray(self.repulsivePartOfP2Ppotential, float)
                    ATT[i, j] = ATT[j, i] = np.asarray(self.attractivePartOfP2Ppotential, float)
        finally:
            #Always restore the real grid, even if a setter raised.
            self._rArrayOverride = None

        #If the chosen setter never populated the repulsive/attractive split
        #(most do not), fall back to "everything repulsive", exactly as
        #setPotentialByName() does for the one-component case. Without this,
        #HMSA and the other split-reading closures would see an all-zero
        #potential and c(r) = 0 would become an exact fixed point -- the
        #"S(q) = 1 everywhere" bug already met once in this project.
        if not (np.any(REP != 0.0) or np.any(ATT != 0.0)):
            REP = -np.log(np.clip(EN, 1e-300, None))
            ATT = np.zeros_like(EN)

        self.boltzmannOfP2Ppotential = EN
        #Where no setter supplied beta*u directly, recover it from the
        #Boltzmann factor; infinite inside a hard core, matching what the
        #setters that do define it store.
        if not np.any(U != 0.0):
            with np.errstate(divide='ignore'):
                U = np.where(EN > 0.0, -np.log(np.clip(EN, 1e-300, None)), np.inf)
        self.p2PpotentialInkTUnits = U
        self.repulsivePartOfP2Ppotential = REP
        self.attractivePartOfP2Ppotential = ATT

        if p == 1:
            #A single class is genuinely a ONE-component problem, so store
            #plain 1-D arrays and leave numberOfComponents at 1. Keeping the
            #(1,1,N) shape instead looks harmless -- isMulticomponent() is
            #False either way, so the scalar code path runs -- but that path
            #then operates on 3-D input and every derived quantity comes back
            #shaped (1,1,N). getRDF().max() still gives the right number,
            #which is why the bit-identical p=1 check passed, but anything
            #expecting a curve (np.interp, the six approximation schemes)
            #fails with "object too deep for desired array".
            self.boltzmannOfP2Ppotential = EN[0, 0]
            self.p2PpotentialInkTUnits = U[0, 0]
            self.repulsivePartOfP2Ppotential = REP[0, 0]
            self.attractivePartOfP2Ppotential = ATT[0, 0]
            self.setStartValue(np.zeros(N))
        else:
            self.setStartValue(np.zeros(self.numberOfUniquePairs()*N))
        self.activePotentialname = 'Polydisperse' + potentialName

    #Polydisperse hard-core Yukawa (charged colloids), multicomponent.
    #********************************************************************
    #Follows B. D'Aguanno & R. Klein, Phys. Rev. A 46, 7652 (1992) and
    #J. Chem. Soc. Faraday Trans. 87, 379 (1991):
    #  * the continuous Schulz size distribution is reduced to p
    #    components by MOMENT MATCHING, sum_i x_i sigma_i^m = <sigma^m>
    #    for m = 0..2p-1. Schulz is a gamma distribution, so this is
    #    exactly Gauss-generalized-Laguerre quadrature. p=3 already gives
    #    results indistinguishable from p=5 up to s_sigma = 0.3.
    #  * charge scales with particle size, see the block below for the
    #    three ways to control that.
    #  * potential (their eq. 3), with factorizable amplitudes:
    #        beta phi_ij(r) = A_i A_j exp(-kappa r)/r,  r > sigma_ij
    #        A_i = Z_i sqrt(L_B) exp(kappa sigma_i/2)/(1 + kappa sigma_i/2)
    #    and a hard core at sigma_ij = (sigma_i + sigma_j)/2.
    #
    #relativeStandardDeviation is s_sigma = sqrt(<sigma^2>-<sigma>^2)/<sigma>.
    #meanDiameter defaults to this grid's own hardSphereDiameter so that
    #lengths stay in the usual sigma units.
    def setPolydisperseHardCoreYukawaPotential(self, relativeStandardDeviation,
                                                numberOfComponents, referenceValence,
                                                bjerrumLengthInSigmaUnits,
                                                meanDiameter=None, chargeExponent=2.0,
                                                *,
                                                componentValences=None,
                                                screeningValences=None):
      from scipy.special import roots_genlaguerre
      from scipy.special import gamma as gammafunction
      p = int(numberOfComponents)
      s = float(relativeStandardDeviation)
      meanSigma = self.hardSphereDiameter if meanDiameter is None else float(meanDiameter)

      if p < 1:
          print("numberOfComponents must be >= 1")
          return
      if s <= 0.0 or p == 1:
          sigma = np.array([meanSigma]); x = np.array([1.0]); p = 1
      else:
          t = 1.0/s**2 - 1.0
          nodes, weights = roots_genlaguerre(p, t)
          sigma = nodes*meanSigma/(t + 1.0)
          #The Gauss-Laguerre weights carry a 1/Gamma(t+1) normalisation, but
          #gammafunction(t+1) OVERFLOWS to inf for narrow distributions --
          #t = 1/srel^2 - 1, so srel=0.01 gives Gamma(10000) = inf and every
          #fraction came out 0, then NaN once normalised. The factor is common
          #to all classes and cancels in the normalisation below, so it is
          #simply left out rather than computed and divided away.
          x = weights/np.sum(weights)

          #roots_genlaguerre ITSELF also degrades for very large t (srel=0.005
          #gives t ~ 4e4) and returns non-finite nodes/weights, which no
          #amount of renormalising can repair. Moment matching is only worth
          #having while it is numerically sound, so fall back to plain
          #equally-spaced classes over +-3 sigma with the Schulz weight
          #evaluated in LOG space (no gamma function, hence no overflow).
          #This loses the exact-moment property but is perfectly adequate
          #precisely where it triggers: a distribution that narrow is already
          #close to monodisperse.
          if (not np.all(np.isfinite(sigma))) or (not np.all(np.isfinite(x))) \
                  or np.sum(x) <= 0.0:
              width = s*meanSigma
              lo = max(meanSigma - 3.0*width, 1e-6*meanSigma)
              sigma = np.linspace(lo, meanSigma + 3.0*width, p)
              z = t
              #Schulz-Zimm log-pdf, dropping additive constants that cancel
              #in the normalisation: (z+1)ln(D) - (z+1)D/<D>
              logw = (z + 1.0)*np.log(sigma/meanSigma) \
                     - (z + 1.0)*sigma/meanSigma
              logw -= logw.max()
              x = np.exp(logw)
              x = x/np.sum(x)

      self.numberOfComponents = p
      self.componentDiameters = sigma
      self.componentFractions = x
      #particleDensity is the TOTAL number density; phi = (pi/6) n <sigma^3>,
      #which reduces to the one-component expression for a delta distribution.
      thirdMoment = np.sum(x*sigma**3)
      self.particleDensity = self.volumeDensity/((np.pi/6.0)*thirdMoment)
      self.componentDensities = self.particleDensity*x

      L_B = float(bjerrumLengthInSigmaUnits)

      #--- how the valence scales with particle size -------------------
      #  1. componentValences -- explicit array of length p, overrides
      #     everything else. Use for externally computed renormalised
      #     charges, measured titration data, or a saturating law. Charge
      #     renormalisation is deliberately NOT implemented here: how to
      #     define an effective charge for a POLYDISPERSE system is
      #     unsettled (even for one component the Alexander et al.
      #     edge-linearisation Z_eff and the extrapolated-point-charge
      #     Q_eff disagree), and burying one choice in a fitting routine
      #     would hide a contested assumption.
      #  2. chargeExponent -- power law Z_i = Z_ref (sigma_i/<sigma>)^n:
      #       n = 2 (default) constant SURFACE CHARGE DENSITY, what
      #           D'Aguanno & Klein used and explicitly flagged as an
      #           assumption with "little precise experimental
      #           information about the scaling".
      #       n = 1 charge LINEAR in size: what charge renormalisation
      #           predicts once the bare charge saturates (Z_eff ~ a/L_B)
      #           and what electrophoresis measures for highly charged
      #           low-salt spheres. Arguably the better choice for
      #           strongly charged colloids; n=2 is the default only so
      #           published D'Aguanno-Klein setups reproduce.
      #       n = 0 size-independent charge.
      #     A single power law cannot be right across a broad
      #     distribution if some size classes are renormalisation-
      #     saturated and others are not; use route 1 in that case.
      if componentValences is not None:
          Z = np.asarray(componentValences, dtype=float).reshape(-1)
          if Z.size != p:
              print("componentValences must have exactly numberOfComponents entries")
              return
      else:
          Z = referenceValence*(sigma/meanSigma)**chargeExponent

      #--- which charge screens? ---------------------------------------
      #kappa is the inverse Debye length due to the SMALL ions. Salt-free
      #with monovalent counterions gives n_counter = sum_i n_i Z_i by
      #charge neutrality, hence kappa^2 = 4 pi L_B sum_i n_i Z_i, the
      #FIRST power of Z. Using Z^2 yields a near-ideal-gas S(q)
      #(g_max ~ 1.006) -- a symptom resembling the separate
      #"S(q) == 1 everywhere" bug fixed in setPotentialByName().
      #
      #screeningValences lets the charge entering kappa differ from the
      #one entering the amplitude; defaults to using the same Z for both
      #(D'Aguanno-Klein behaviour). The distinction matters with charge
      #renormalisation: the amplitude needs the effective charge, whereas
      #ALL counterions are physically present and screen. Note that
      #changing chargeExponent therefore moves BOTH the amplitude and the
      #screening length unless they are decoupled here.
      if screeningValences is not None:
          Zscreen = np.asarray(screeningValences, dtype=float).reshape(-1)
          if Zscreen.size != p:
              print("screeningValences must have exactly numberOfComponents entries")
              return
      else:
          Zscreen = Z

      self.kappaInverseDebyeLength = np.sqrt(4*np.pi*L_B*np.sum(self.componentDensities*Zscreen))
      kappa = self.kappaInverseDebyeLength
      #Factorizable amplitude: sigma_ij = (sigma_i+sigma_j)/2 makes
      #exp(kappa sigma_ij) = exp(kappa sigma_i/2) exp(kappa sigma_j/2),
      #so beta phi_ij = A_i A_j exp(-kappa r)/r.
      A = Z*np.sqrt(L_B)*np.exp(kappa*sigma/2.0)/(1.0 + kappa*sigma/2.0)
      self.componentValences = Z
      self.componentScreeningValences = Zscreen
      self.componentAmplitudes = A

      r = self.getrArray()
      sigma_ij = 0.5*(sigma[:, None] + sigma[None, :])
      amplitude_ij = A[:, None]*A[None, :]
      isCore = r[None, None, :] < sigma_ij[:, :, None]
      with np.errstate(over='ignore', invalid='ignore'):
          betaU = amplitude_ij[:, :, None]*np.exp(-kappa*r)/r
      betaU = np.where(isCore, 0.0, betaU)
      self.p2PpotentialInkTUnits = np.where(isCore, np.inf, betaU)
      self.boltzmannOfP2Ppotential = np.where(isCore, 0.0, np.exp(-betaU))
      self.derivativeOfP2Ppotential = np.where(
          isCore, 0.0,
          -amplitude_ij[:, :, None]*np.exp(-kappa*r)*(1.0/r**2 + kappa/r))

      #Repulsive/attractive split, as (p,p,N) pair matrices. Set explicitly
      #here rather than left to setPotentialByName()'s generic fallback, so
      #that the physics is stated rather than inferred.
      #
      #This potential is a screened COULOMB interaction between like-signed
      #charges: beta*U_ij = A_i A_j exp(-kappa r)/r with A_i A_j > 0, so it is
      #purely REPULSIVE at every separation and the attractive part is
      #identically zero. That is not a placeholder -- it is the correct split
      #for this model.
      #
      #Consequence worth knowing: HMSA interpolates between a
      #soft-core/MSA-like treatment of the repulsive part and HNC for the
      #attractive one, so with no attractive tail HMSA reduces EXACTLY to
      #Rogers-Young. Both closures returning identical numbers here is the
      #expected result, not a bug -- the same identity was checked earlier for
      #plain hard spheres.
      #
      #Supplying these makes the closures that read the split
      #(HMSA, VM, CJVM, BB, DH, CG, SMSA) usable for polydisperse systems.
      self.repulsivePartOfP2Ppotential = np.where(isCore, 700.0, betaU)
      self.attractivePartOfP2Ppotential = np.zeros_like(betaU)

      #The fixpoint vector now carries p(p+1)/2 pairs, so the start value
      #has to be resized (same pattern the ch-fixpoint operator uses when
      #it switches the vector length).
      self.setStartValue(np.zeros(self.numberOfUniquePairs()*self.numberOfRadialSamplingPoints))
      self.activePotentialname = 'PolydisperseHardCoreYukawa'

    #Definition of the pair interaction potential
    #********************************************************************************************************
    
    
    #Return a list of the characteristic potential names
    def getAvailablePotentialNames(self):
      potentialNameList = []
      methodTupelList = inspect.getmembers(self, predicate=inspect.ismethod)
      for methodTupel in methodTupelList:
          methodName = methodTupel[0]
          if methodName.startswith('set') and methodName.endswith('Potential'):
              potentialName = methodName[len('set'):-len('Potential')]
              potentialNameList.append(potentialName)
              #print(methodName[len('set'):-len('Potential')])
      #print('***************************************************')
      return potentialNameList
    
    #Print out a list of the characteristic potential names
    def printPotentialSetterArguments(self):
      methodTupelList = inspect.getmembers(self, predicate=inspect.ismethod)
      print("***************************************************")
      print("Available Potentials with corresponding Parameters:")
      print("***************************************************")
      for methodTupel in methodTupelList:
          methodName = methodTupel[0]
          methodToCall = getattr(self, methodName)
          if methodName.startswith('set') and methodName.endswith('Potential'):
              print("Potential Name:", methodName[len('set'):-len('Potential')], "; Arguments:", inspect.getfullargspec(methodToCall)[0][1:])
      print("************************************************************************************************************************")

    
    #Implementation of the specific potential setters, these could be called directly, but it is
    #preferred to call them indirectly by the common API 'setPotentialByName'.
    #Hard Sphere
    def setHardSpherePotential(self):
      self.boltzmannOfP2Ppotential[:] = 1.0
      self.boltzmannOfP2Ppotential[:self.hardSphereDiameterInPoints] = 0.0
   
    #Lennard Jones
    def setLennardJonesPotential(self, epsilonInkTUnits):
      #epsilonInkTUnits may as well be a string
      try:
          epsilonInkTUnits = float(epsilonInkTUnits)
      except ValueError:
          print("epsilonInkTUnits can not be converted to float")
          return
      #Side effect: We record the value in a member variable (was initialized with a negative number)
      self.epsilonInkTUnits = epsilonInkTUnits
      #calculate potential
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential_LJ = (sigma/r)**12 - (sigma/r)**6
      #Since Potential so far is unitless, we also want this for the energy
      #hence we express epsilon (energy scale of potential) in kT unit
      #binding energy. For e.g. liquid Argon: e/k ~ 120 K, e/(kT) ~ 1.2
      #(http://www.sklogwiki.org/SklogWiki/index.php/Argon)
      potential_LJ *= 4.0*epsilonInkTUnits
      #pl.plot(potential_LJ[hardSphereDiameter -2:]); pl.show()
      self.boltzmannOfP2Ppotential = np.exp(-potential_LJ)
      #calculate also repulsive part , see J.K docu, page 5.102
      minmumPositionInPoints = int(2**(1.0/6.0)*self.getHardSphereDiameterInPoints())
      # - epsilonInkTUnits = u(2^{1/6)}) = u(r_min)
      self.repulsivePartOfP2Ppotential[:minmumPositionInPoints] = potential_LJ[:minmumPositionInPoints]
      self.attractivePartOfP2Ppotential = potential_LJ - self.repulsivePartOfP2Ppotential
     
    #Yukawa 
    def setYukawaPotential(self, shieldingLengthInSigmaUnits, interactionStrengthInKTunits, doAddHardSphere):
      #Check if shieldingLengthInSigmaUnits, interactionStrengthInKTunits is reasonable float value (> 0)
      try:
          shieldingLengthInSigmaUnits = float(shieldingLengthInSigmaUnits)
          interactionStrengthInKTunits = float(interactionStrengthInKTunits)
      except ValueError:
          print("shieldingLengthInSigmaUnits or interactionStrengthInKTunits can not be converted to float")
          return
      #Boolean value doAddHardSphere may also be string, if so, we convert to Boolean type
      if isinstance(doAddHardSphere, str):
          if doAddHardSphere == 'True':
              doAddHardSphere = True
          else: #any other string evaluates to false
              doAddHardSphere = False
      #Side effect: We record the value in a member variable (was initialized with a negative number)
      self.shieldingLengthInSigmaUnits = shieldingLengthInSigmaUnits
      self.interactionStrengthInKTunits = interactionStrengthInKTunits
      self.isHardSphereAdded = doAddHardSphere
      #calculate potential
      r = self.getrArray()
      sigma = self.getHardSphereDiameter()
      potential_Y = np.exp(-r/shieldingLengthInSigmaUnits)/(r/sigma ) #so far dimension less
      self.boltzmannOfP2Ppotential = np.exp(-interactionStrengthInKTunits*potential_Y)
      if doAddHardSphere:
          sigmaInPoints = self.getHardSphereDiameterInPoints()
          #shift exp(-potential) by sigma (also 1/r -> 1/(r - sigma), de-activated (overwritten bellow)
          self.boltzmannOfP2Ppotential = np.roll(self.boltzmannOfP2Ppotential, sigmaInPoints)
          #Only shift exp(-r/lamda)......
          potential_Y = np.exp(-r/shieldingLengthInSigmaUnits)
          potential_Y = np.roll(potential_Y, sigmaInPoints) # = np.exp(-(r - sigma)/shieldingLengthInSigmaUnits)
          #.......Don't shift 1/r part
          potential_Y *= sigma/r
          #Activate next line for exp( (r - sigma)/lamda) / (r/sigma)
          self.boltzmannOfP2Ppotential = np.exp(-interactionStrengthInKTunits*potential_Y)
          #HS definition
          self.boltzmannOfP2Ppotential[:sigmaInPoints] = 0.0
          
      #calculate also repulsive part
      self.repulsivePartOfP2Ppotential = potential_Y
      #attractive part is zero (as initialized)
    


    #Star Polymer potential (Likos & Harreis 2002), high-functionality
    #branch (U_Star1 in sasfit_oz_potential_star_Likos.c), valid for
    #f>=10. Renamed from setStarPotential() to make explicit that this
    #is one of TWO distinct formulas the C source actually has (see
    #setStarPolymerLowFPotential() immediately below for the f<=10
    #branch, U_Star2) -- an earlier version of this port only had this
    #one, silently using it for every functionality regardless of
    #whether f was above or below the paper's own crossover point.
    def setStarPolymerHighFPotential(self, NumberOfArms):
      try:
          NumberOfArms = int(NumberOfArms)
      except ValueError:
          print("NumberOfArms can not be converted to int")
          return

      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential_Star =   5.0/18.0*np.power(NumberOfArms,3.0/2.0)* 1.0/(1.0+np.sqrt(NumberOfArms)/2.0)* sigma/r* np.exp(-np.sqrt(NumberOfArms)*(r-sigma)/(2.0*sigma))
      potential_Star[:self.hardSphereDiameterInPoints] =   5.0/18.0*np.power(NumberOfArms,3.0/2.0)* (-np.log(r[:self.hardSphereDiameterInPoints]/sigma)+1.0/(1.0+np.sqrt(NumberOfArms)/2.0))
      #if r<=sigma :
          #potential_Star[:self.hardSphereDiameterInPoints] =   5.0/18.0*pow(NumberOfArms,3.0/2.0)* (-log(r/sigma)+1.0/(1.0+sqrt(NumberOfArms)/2.0))
      #else:
          #potential_Star =   5.0/18.0*pow(NumberOfArms,3.0/2.0)* 1.0/(1.0+sqrt(NumberOfArms)/2.0)* sigma/r* exp(-sqrt(NumberOfArms)*(r-sigma)/(2.0*sigma))
      #pl.plot(potential_Star[hardSphereDiameter -2:]); pl.show()
      self.boltzmannOfP2Ppotential = np.exp(-potential_Star)

    #Star Polymer potential (Likos & Harreis 2002), low-functionality
    #branch (U_Star2 in sasfit_oz_potential_star_Likos.c), valid for
    #f<=10 -- a genuinely DIFFERENT formula from the f>=10 branch above
    #(Gaussian decay outside the core, with its own tau(f) interpolation
    #between tau(f=2)=1.03/sigma and tau(f=5)=1.12/sigma), not just the
    #same formula evaluated at a smaller f. This was previously missing
    #entirely from this port -- confirmed directly against
    #sasfit_oz_potential_star_Likos.c's own U_Star2(), found while
    #cross-checking the C source specifically for this potential.
    #The two branches do NOT meet smoothly at f=10 (checked directly:
    #~28% relative difference in U(r=sigma+), about 30x apart by
    #r=2*sigma) -- this is a genuine property of Likos & Harreis's own
    #two-regime fit, not a bug in either branch; f=10 is where the
    #paper recommends switching approximations, not a point the two
    #forms were designed to agree at exactly.
    def setStarPolymerLowFPotential(self, NumberOfArms):
      try:
          NumberOfArms = int(NumberOfArms)
      except ValueError:
          print("NumberOfArms can not be converted to int")
          return

      sigma = self.hardSphereDiameter
      r = self.getrArray()
      f = NumberOfArms
      tau2 = 1.03/sigma
      tau5 = 1.12/sigma
      tau = (tau5-tau2)/3.0*f+tau2
      potential_Star = 5.0/18.0*np.power(f,3.0/2.0)*1.0/(2.0*(tau*sigma)**2)*np.exp(-tau*tau*(r*r-sigma*sigma))
      potential_Star[:self.hardSphereDiameterInPoints] = 5.0/18.0*np.power(f,3.0/2.0)*(-np.log(r[:self.hardSphereDiameterInPoints]/sigma)+1.0/(2.0*(tau*sigma)**2))
      self.boltzmannOfP2Ppotential = np.exp(-potential_Star)

    # ---------------------------------------------------------------
    # NEW potentials, ported from src/sasfit_oz/sasfit_oz_potential_*.c
    # (main U(r) form only; the R/A/SR/LR/Ref/Pert reference-vs-
    # perturbation split variants in the C files are for a different
    # purpose there -- RY/HMSA-style closures that need that split use
    # self.repulsivePartOfP2Ppotential/self.attractivePartOfP2Ppotential
    # here instead, same mechanism as the existing LennardJones/Yukawa
    # setters above.) All energies in kT units, matching this file's
    # existing convention -- the C code's explicit kb*T factors are
    # dropped accordingly (kb*T=1 in these units).
    # ---------------------------------------------------------------

    #Depletion potential (Oversteegen & Lekkerkerker 2004), 2-sphere form
    #(sasfit_oz_potential_depletion.c, U_Depletion)
    def setDepletionPotential(self, sigmaRatio, phi2):
      try:
          sigmaRatio = float(sigmaRatio); phi2 = float(phi2)
      except ValueError:
          print("sigmaRatio or phi2 can not be converted to float")
          return
      sigma1 = self.hardSphereDiameter            #diameter of the (big) spheres this OZ solve is actually for
      sigma2 = sigmaRatio*sigma1                #diameter of the (small) depletant
      r = self.getrArray()
      rt = sigma2/sigma1
      rho2 = 6.0*phi2/(np.pi*sigma2**3)
      potential_Dep = np.zeros_like(r)
      mask = (r >= sigma1) & (r < sigma1 + sigma2)
      potential_Dep[mask] = (-rho2*np.pi*sigma1**3*(1.0+rt)**3/6.0)*(
          1.0 - (3.0*r[mask]/(2.0*(1.0+rt)*sigma1))
          + (r[mask]**3/(2.0*(1.0+rt)**3*sigma1**3)))
      self.boltzmannOfP2Ppotential = np.exp(-potential_Dep)
      self.boltzmannOfP2Ppotential[r < sigma1] = 0.0

    #Derjaguin-Landau-Verwey-Overbeek (DLVO) potential
    #(sasfit_oz_potential_dlvo.c, U_DLVO). kappa is the (dimensionful)
    #inverse Debye-Hueckel screening length, LB the Bjerrum length,
    #A the effective Hamaker constant -- all in the same length unit as
    #this file's r-grid, matching the C code's parameter convention.
    def setDLVOPotential(self, kappa, Z, LB, A):
      try:
          kappa = float(kappa); Z = float(Z); LB = float(LB); A = float(A)
      except ValueError:
          print("kappa, Z, LB or A can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      a = 0.5*sigma
      Uel  = LB*Z**2/(1.0+kappa*a)**2 * np.exp(-kappa*(r-sigma))/r
      #the van der Waals term has a genuine (physical) divergence at
      #r=2a=sigma. Found by testing that this isn't just a "starts just
      #past sigma" theoretical concern: the grid's own construction
      #(hardSphereDiameter = hardSphereDiameterInPoints*Delta_r exactly)
      #puts a real grid point at r==sigma to the last bit of floating-
      #point precision, causing a genuine 0/0 (not just "very large")
      #at that one point if UvdW is evaluated on the full r array first
      #and masked afterward. Excluded from the computation itself here,
      #rather than only masked afterward, to avoid both the resulting
      #RuntimeWarning and the transient NaN it produces.
      outside = r > sigma
      UvdW = np.zeros_like(r)
      ro = r[outside]
      UvdW[outside] = -A/6.0*(2.0*a*a/(ro*ro-4.0*a*a) + 2.0*(a/ro)**2 + np.log(1.0-4.0*(a/ro)**2))
      potential_DLVO = Uel + UvdW
      self.boltzmannOfP2Ppotential = np.exp(-potential_DLVO)
      self.boltzmannOfP2Ppotential[~outside] = 0.0
      #repulsive/attractive split, for RY/HMSA-style closures
      self.repulsivePartOfP2Ppotential = Uel
      self.attractivePartOfP2Ppotential = UvdW

    #DLVO plus an extra short-range hydration repulsion term
    #(sasfit_oz_potential_dlvo_hydra.c, U_DLVO_Hydra)
    def setDLVOHydraPotential(self, kappa, Z, LB, A, GHY, DH):
      try:
          GHY = float(GHY); DH = float(DH)
      except ValueError:
          print("GHY or DH can not be converted to float")
          return
      #Build the plain DLVO potential first (also sets repulsive/attractive parts)
      self.setDLVOPotential(kappa, Z, LB, A)
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      Hydra = 2.0*GHY*np.exp(-(r-sigma)/DH)
      potential_DLVO = -np.log(np.clip(self.boltzmannOfP2Ppotential, 1e-300, None))
      potential_DLVO[r > sigma] -= Hydra[r > sigma]
      self.boltzmannOfP2Ppotential = np.exp(-potential_DLVO)
      self.boltzmannOfP2Ppotential[r <= sigma] = 0.0
      #NOTE (checked directly against the C source, not left as an
      #assumption): self.repulsivePartOfP2Ppotential/attractivePartOfP2Ppotential
      #are deliberately NOT updated to include the Hydra term above --
      #this was flagged as a possible staleness bug during review, but
      #sasfit_oz_potential_dlvo_hydra.c's own U_R_DLVO_Hydra/
      #U_A_DLVO_Hydra are BYTE-FOR-BYTE IDENTICAL to plain DLVO's own
      #U_R_DLVO/U_A_DLVO -- the original C code itself deliberately
      #excludes the Hydra correction from this split, only the full
      #U_DLVO_Hydra includes it. This Python port already matches that
      #exactly (setDLVOPotential() sets the split, and is intentionally
      #never revisited here), so no fix was needed after all; this note
      #exists purely so a future reader doesn't mistake this for the
      #same kind of oversight setPotentialByName()'s own fallback split
      #(see its own comment above) was written to catch.

    #Fermi-Dirac distribution potential (C.N. Likos)
    #(sasfit_oz_potential_FermiDistributionModel.c, U_FDM)
    #xi=0 reduces to the penetrable sphere model (PSM), same as in the C code.
    def setFermiDistributionPotential(self, epsilonInkTUnits, xi):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits); xi = float(xi)
      except ValueError:
          print("epsilonInkTUnits or xi can not be converted to float")
          return
      if xi == 0.0:
          self.setPSMPotential(epsilonInkTUnits)
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential_FDM = epsilonInkTUnits*(1.0+np.exp(-sigma/xi))/(1.0+np.exp((r-sigma)/xi))
      self.boltzmannOfP2Ppotential = np.exp(-potential_FDM)

    #Generalized Gaussian core model GGCM-n
    #(sasfit_oz_potential_ggcm-n.c, U_GGCM_n)
    def setGGCMnPotential(self, epsilonInkTUnits, n, alpha):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits); n = float(n); alpha = float(alpha)
      except ValueError:
          print("epsilonInkTUnits, n or alpha can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential_GGCM = epsilonInkTUnits*np.exp(-alpha*np.power(r/sigma, n))
      self.boltzmannOfP2Ppotential = np.exp(-potential_GGCM)

    #Hard sphere with 3 additive Yukawa tails
    #(sasfit_oz_potential_HS_3Yukawa.c, U_HS_3Yukawa)
    def setHS3YukawaPotential(self, K1, lambda1, K2, lambda2, K3, lambda3):
      try:
          K1, lambda1, K2, lambda2, K3, lambda3 = (float(v) for v in
              (K1, lambda1, K2, lambda2, K3, lambda3))
      except ValueError:
          print("K1/lambda1/K2/lambda2/K3/lambda3 can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      Y1 = -K1*np.exp(-(r-sigma)/sigma/lambda1)
      Y2 = -K2*np.exp(-(r-sigma)/sigma/lambda2)
      Y3 = -K3*np.exp(-(r-sigma)/sigma/lambda3)
      potential = sigma/r*(Y1+Y2+Y3)
      self.boltzmannOfP2Ppotential = np.exp(-potential)
      self.boltzmannOfP2Ppotential[r < sigma] = 0.0

    #Ionic microgel (Likos et al., "Structure and Thermodynamics of Ionic
    #Microgels", 2011). el = elementary charge in statcoulombs (cgs-esu),
    #matching the C code's own hard-coded constant exactly.
    #(sasfit_oz_potential_ionic_microgel.c, U_Ionic_Microgel)
    def setIonicMicrogelPotential(self, Z, ED, KPi, EPSILON):
      try:
          Z = float(Z); ED = float(ED); KPi = float(KPi); EPSILON = float(EPSILON)
      except ValueError:
          print("Z, ED, KPi or EPSILON can not be converted to float")
          return
      el = 4.80320425e-10  #elementary charge in statcoulombs
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      kp = KPi/sigma
      potential = np.zeros_like(r)

      inside = r <= sigma
      ri = r[inside]
      p1 = (1.0-np.exp(-kp*ri)+0.5*(kp*ri)**2+(kp*ri)**4/24.0)*(1.0-4.0/(kp*sigma)**2)
      p2 = 4.0*np.exp(-kp*sigma)*np.sinh(kp*ri)/(kp*sigma)
      p3 = (np.exp(-kp*sigma)*np.sinh(kp*ri)
            + kp**2*sigma*ri
            + kp**4*(sigma**3*ri + ri**3*sigma)/6.0) * (1.0+4.0/(kp*sigma)**2)
      p4 = (4.0*ri/sigma)*(1.0+0.5*(kp*sigma)**2+(kp*sigma)**4/30.0)
      p5 = (8.0*(ri/sigma)**3/3.0)*(0.25*(kp*sigma)**2+(kp*sigma)**4/12.0)
      p6 = kp**4*ri**6/(180.0*sigma**2)
      phiind = p1+p2+p3-p4-p5-p6
      potential[inside] = (EPSILON*np.power(np.clip(1.0-(ri/sigma), 0, None), 2.5)
          + (2.0*(Z*el)**2)/(ED*sigma)*(1.2-2.0*(ri/sigma)**2+1.5*(ri/sigma)**3-0.2*(ri/sigma)**5)
          - phiind*(72.0*(Z*el)**2/(ED*(kp*sigma)**4*ri)))

      outside = ~inside
      ro = r[outside]
      potential[outside] = ((144.0*(Z*el)**2/(ED*(kp*sigma)**4))
          * (np.cosh(0.5*kp*sigma)-(2.0*np.sinh(0.5*kp*sigma)/(kp*sigma)))**2
          * np.exp(-kp*ro)/ro)

      self.boltzmannOfP2Ppotential = np.exp(-potential)

    #3-segment piecewise-constant hard-sphere shoulder/well potential
    #(sasfit_oz_potential_piecewise_constant_HS.c, U_Piecewise_Constant_HS)
    def setPiecewiseConstantHSPotential(self, epsilon1, delta1, epsilon2, delta2, epsilon3, delta3):
      try:
          epsilon1, delta1, epsilon2, delta2, epsilon3, delta3 = (float(v) for v in
              (epsilon1, delta1, epsilon2, delta2, epsilon3, delta3))
      except ValueError:
          print("epsilon1/delta1/epsilon2/delta2/epsilon3/delta3 can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      d1, d2, d3 = abs(delta1), abs(delta2), abs(delta3)
      potential = np.zeros_like(r)
      m1 = (r >= sigma) & (r < sigma+d1)
      m2 = (r >= sigma+d1) & (r < sigma+d1+d2)
      m3 = (r >= sigma+d1+d2) & (r < sigma+d1+d2+d3)
      potential[m1] = epsilon1
      potential[m2] = epsilon2
      potential[m3] = epsilon3
      self.boltzmannOfP2Ppotential = np.exp(-potential)
      self.boltzmannOfP2Ppotential[r < sigma] = 0.0

    #Penetrable sphere model (Likos, Watzlawek, Loewen, PRE 58 (1998) 3135)
    #(sasfit_oz_potential_psm.c, U_PSM)
    def setPSMPotential(self, epsilonInkTUnits):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits)
      except ValueError:
          print("epsilonInkTUnits can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential = np.where(r <= sigma, epsilonInkTUnits, 0.0)
      self.boltzmannOfP2Ppotential = np.exp(-potential)

    #Soft sphere (inverse power law)
    #(sasfit_oz_potential_soft_sphere.c, U_Soft_Sphere)
    def setSoftSpherePotential(self, epsilonInkTUnits, n):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits); n = float(n)
      except ValueError:
          print("epsilonInkTUnits or n can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential = epsilonInkTUnits*np.power(sigma/r, n)
      self.boltzmannOfP2Ppotential = np.exp(-potential)

    #Parabolic/Hertzian sphere
    #(sasfit_oz_potential_soft_sphere.c, U_Parabolic_Sphere)
    def setParabolicSpherePotential(self, epsilonInkTUnits):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits)
      except ValueError:
          print("epsilonInkTUnits can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      potential = np.where(r <= sigma,
                            epsilonInkTUnits*np.power(np.clip(1.0-(r/sigma), 0, None), 2.5),
                            0.0)
      self.boltzmannOfP2Ppotential = np.exp(-potential)

    #Square well
    #(sasfit_oz_potential_square_well.c, U_Square_Well_Sphere)
    def setSquareWellPotential(self, epsilonInkTUnits, delta):
      try:
          epsilonInkTUnits = float(epsilonInkTUnits); delta = float(delta)
      except ValueError:
          print("epsilonInkTUnits or delta can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      d = abs(delta)
      potential = np.zeros_like(r)
      m = (r >= sigma) & (r <= sigma+d)
      potential[m] = epsilonInkTUnits
      self.boltzmannOfP2Ppotential = np.exp(-potential)
      self.boltzmannOfP2Ppotential[r < sigma] = 0.0

    #Sticky hard sphere, Baxter's tau convention -- same tau convention
    #used by the robertus_shs SASfit plugin (small tau = sticky, large
    #tau = hard sphere), giving a natural point of comparison between
    #this (monodisperse) OZ-based solve and that (polydisperse, PY-only)
    #multicomponent solve.
    #(sasfit_oz_potential_sticky_hard_sphere.c, U_Sticky_Hard_Sphere)
    def setStickyHardSpherePotential(self, tau, delta):
      try:
          tau = float(tau); delta = float(delta)
      except ValueError:
          print("tau or delta can not be converted to float")
          return
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      d = abs(delta)
      energy = np.log(12.0*tau*d/(sigma+d))
      potential = np.zeros_like(r)
      m = (r >= sigma) & (r <= sigma+d)
      potential[m] = energy
      self.boltzmannOfP2Ppotential = np.exp(-potential)
      self.boltzmannOfP2Ppotential[r < sigma] = 0.0

    #****************************************************************************************************
    #Getter and Setter plus actual solver method. All other members (variables and methods) are private
    #But the public/private difference is solved by what is provided by the RPC 'API'.
    
    #The definition of the potential is solved by Object introspection in order to avoid tedious code 
    #like: "if potential == lennardJones : set LennardJonesPotential(x); if "if potential ==    .....
    #First, we can set the potential by its characteristic name in the setter of the name schema set<PotentialName>Potential()
    def setPotentialByName(self, potentialType, *args):
      #First we check if a name was given which can be mapped
      #to a method Name
      if potentialType not in self.getAvailablePotentialNames():
          print("unrecognized potential")
          return
      methodName = 'set' + potentialType + 'Potential'
      methodToCall = getattr(self, methodName)
      #Then we check if the correct number of arguments was given for this potential type
      #Arguments WITH defaults are OPTIONAL: only the leading positional ones
      #are required. Counting every parameter (as this did before) made any
      #setter carrying default arguments unreachable through this entry point:
      #setPolydisperseHardCoreYukawaPotential could not be selected at all, and
      #the rejection was SILENT -- it printed and returned, leaving the
      #potential untouched, so the solver ran on its default state and every
      #closure then produced the trivial S(q) = 1. The GUI builds its input
      #fields from this same argspec, so optional arguments simply appear as
      #extra fields the user may leave at their defaults.
      spec = inspect.getfullargspec(methodToCall)
      numberOfArgumentsNeeded = len(spec[0]) - 1 # minus self
      numberOfDefaults = len(spec.defaults) if spec.defaults else 0
      numberOfArgumentsRequired = numberOfArgumentsNeeded - numberOfDefaults
      #print(numberOfArgumentsNeeded)
      if not args:
          if numberOfArgumentsRequired != 0:
              print("number of arguments given for", potentialType, "is not correct:",
                    numberOfArgumentsRequired, "required, none given")
              return
      else:
          if not (numberOfArgumentsRequired <= len(args) <= numberOfArgumentsNeeded):
              print("number of arguments given for", potentialType, "is not correct:",
                    len(args), "given, expected between", numberOfArgumentsRequired,
                    "and", numberOfArgumentsNeeded)
              return
          
      #If everything is fine, we call the setter for this potential type with its parameters,
      #this would be e.g. setLennardJonesPotential(0.8)
      methodToCall(*args)
      self.activePotentialname = potentialType

      #Fallback default repulsive/attractive split, for closures that
      #need it (HMSA/VM/CJVM/BB/DH/CG/SMSA -- see update_c() above).
      #Only LennardJones/Yukawa/DLVO(Hydra) actually populate
      #self.repulsivePartOfP2Ppotential/self.attractivePartOfP2Ppotential
      #themselves; every other potential setter leaves both at their
      #__init__ zeros-everywhere default. That silently broke every one
      #of those closures for every OTHER potential (confirmed directly
      #for HardSphere+HMSA: with both arrays exactly zero, HMSA's own
      #formula reduces to exp(-0)*(1+(exp(f*G)-1)/f)-G-1, which
      #evaluates to EXACTLY 0 at G=0 -- a genuine, self-consistent fixed
      #point with no hard-core exclusion at all, so the solver
      #'converges' instantly to G=0, c(r)=0 everywhere, i.e. S(q)=1
      #everywhere -- not a slow-convergence issue, a wrong-equations
      #issue). Detected here (rather than fixed inside every affected
      #setter individually) via "both arrays are still all-exactly-zero"
      #-- a reliable test, since __init__'s own np.zeros(...) leaves
      #every element exactly 0.0, and no real potential's own genuine
      #split legitimately does that. Where it does trigger, the whole
      #potential is treated as purely repulsive (attractive part left at
      #zero) -- confirmed by direct calculation that this makes HMSA's
      #formula reduce to become algebraically IDENTICAL to RY's own
      #formula for a potential with no attractive tail (e.g. plain
      #HardSphere), which is exactly the physically expected behaviour.
      if not (np.any(self.repulsivePartOfP2Ppotential != 0.0) or
              np.any(self.attractivePartOfP2Ppotential != 0.0)):
          EN = self.boltzmannOfP2Ppotential
          self.repulsivePartOfP2Ppotential = -np.log(np.clip(EN, 1e-300, None))
          self.attractivePartOfP2Ppotential = np.zeros_like(EN)

      
    #Standard setters (defining members directly)
    def setVolumeDensity(self, rho_V):
      #Make check
      self.volumeDensity = rho_V
      #Side effects
      self.particleDensity = self.transformVolume2ParticleNumberDensity(self.volumeDensity)
      self.zeroQlimitOfStructureFactor = self.calculateZeroQlimitOfStructureFactor(self.volumeDensity)

    def doPYclosure(self):
      self.closureType = 'PY'
      self.isHNC = False
      self.alpha = -1.0

    def doHNCclosure(self):
      self.closureType = 'HNC'
      self.isHNC = True
      #Switch off RY via the implicit (negative alpha) definition
      self.alpha = -1.0
      
    def doRYclosure(self, alpha):
      if not alpha >= 0.0:
          print("alpha must be positive real number")
          return
      self.closureType = 'RY'
      self.isHMSA = False
      self.alpha = float(alpha)
      #side effect: define function which interpolates between PY and HNC
      r = self.getrArray()
      self.helperRYfunction = 1.0 - np.exp(-self.alpha*r)
      
    def doHMSAclosure(self, alpha):
      if not alpha >= 0.0:
          print("alpha must be positive real number")
          return   
      self.closureType = 'HMSA'
      self.isHMSA = True
      self.alpha = float(alpha)
      #side effect: define function which interpolates between PY and HNC
      r = self.getrArray()
      self.helperRYfunction = 1.0 - np.exp(-self.alpha*r)

    # ---------------------------------------------------------------
    # NEW closures, ported from the `case XXX:` blocks of
    # src/sasfit_oz/sasfit_oz_solver.c's own closure switch statement.
    # Each is standalone (needs no reference-system sub-solve) -- see
    # update_c() above for the formulas and why RHNC/MHNC/RMSA/EuRah
    # are NOT included here.
    # ---------------------------------------------------------------
    def doCarbajalTinokoClosure(self, lam):
      #Carbajal-Tinoco: an implicit bridge function, solved by a vectorised
      #fixed-point iteration in update_c(). lambda controls the amplitude,
      #which is r-dependent for lambda <= 0.
      self.closureType = 'CarbajalTinoko'
      self.alpha = float(lam)

    def doKhanpourClosure(self, alpha):
      #Khanpour bridge function; alpha -> 0 recovers HNC. Named in full
      #because 'KH' in this library is Kovalenko-Hirata, a different
      #closure with the same natural abbreviation.
      if alpha == 0.0:
          print("alpha must be non-zero (division by alpha in the Khanpour bridge function)")
          return
      self.closureType = 'Khanpour'
      self.alpha = float(alpha)

    def doModifiedVerletClosure(self, alpha):
      if alpha == 0.0:
          print("alpha must be non-zero (division by 1 + alpha*Gamma/2)")
          return
      self.closureType = 'ModifiedVerlet'
      self.alpha = float(alpha)

    def doExtendedRYclosure(self, alpha, a=0.0):
      #Extended Rogers-Young. alpha is the RY switching rate; a is the
      #extra quadratic coefficient, and a = 0 reduces exactly to RY.
      if alpha <= 0.0:
          print("alpha must be > 0 for the Extended Rogers-Young switching function")
          return
      self.closureType = 'ExtendedRY'
      self.alpha = float(alpha)
      self.extendedRYa = float(a)
      self.helperRYfunction = 1.0 - np.exp(-self.alpha*self.getrArray())

    def doVerletClosure(self):
      self.closureType = 'Verlet'

    def doBPGGclosure(self, alpha):
      if alpha == 0.0:
          print("alpha must be non-zero (division by alpha in the BPGG bridge function)")
          return
      self.closureType = 'BPGG'
      self.alpha = float(alpha)

    def doVMclosure(self):
      self.closureType = 'VM'

    def doKHclosure(self):
      self.closureType = 'KH'

    def doDHclosure(self):
      self.closureType = 'DH'

    def doCGclosure(self):
      self.closureType = 'CG'

    def doCJVMclosure(self, alpha):
      if alpha == 0.0:
          print("alpha must be non-zero (division by alpha in the CJVM bridge function)")
          return
      self.closureType = 'CJVM'
      self.alpha = float(alpha)

    def doBBclosure(self, alpha):
      self.closureType = 'BB'
      self.alpha = float(alpha)

    def doSMSAclosure(self):
      self.closureType = 'SMSA'

    def doMSAclosure(self):
      self.closureType = 'MSA'

    def doMMSAclosure(self):
      self.closureType = 'mMSA'

    def doMSclosure(self):
      self.closureType = 'MS'

    def _analyticalPYbridgeFunction(self, eta):
      #Analytical Percus-Yevick hard-sphere bridge function B(r), at
      #packing fraction eta, evaluated on this object's own r-grid --
      #mirrors sasfit_oz_solver.c's own BPY(r/sigma, eta), reusing the
      #same closed-form c(r) calculateAnalyticalRDFsolutionForHS()
      #already uses (generalized here to an arbitrary eta, since MHNC
      #uses this as a tunable parameter, not necessarily the actual
      #system's own volume fraction). g(r) outside contact has no
      #simple closed form, so it is obtained the same way that method
      #already does: Hankel-transforming the analytical c(r) exactly,
      #not by any further approximation.
      sigma = self.hardSphereDiameter
      r = self.getrArray()
      x = r/sigma
      inside = x < 1.0

      c_ana = np.zeros_like(r)
      xin = x[inside]
      c_ana[inside] = (6.0*eta*(1.0+0.5*eta)**2*xin
                        - (1.0+2.0*eta)**2*(1.0+0.5*eta*xin**3)) / (1.0-eta)**4

      rho = self.transformVolume2ParticleNumberDensity(eta)
      c_hat = self.hankelTransform(c_ana, self.Delta_r)
      G_hat = c_hat/(1.0 - rho*c_hat) - c_hat
      G_ana = self.inverseHankelTransform(G_hat, self.Delta_r)
      g_ana = G_ana + c_ana + 1.0
      g_ana[inside] = 0.0

      #BPY()'s own br, then negated on return -- see sasfit_oz_solver.c:
      #br = -cr-1-log(-cr) for r<sigma; br = gr-1-log(gr) for r>sigma;
      #BPY returns -br.
      BRIDGE = np.zeros_like(r)
      BRIDGE[inside] = c_ana[inside] + 1.0 + np.log(np.clip(-c_ana[inside], 1e-300, None))
      outside = ~inside
      BRIDGE[outside] = -g_ana[outside] + 1.0 + np.log(np.clip(g_ana[outside], 1e-300, None))
      return BRIDGE

    def doMHNCclosure(self, eta):
      #Modified HNC. eta here is an independently tunable "effective"
      #hard-sphere packing fraction fed into the analytical PY bridge
      #function (see _analyticalPYbridgeFunction() above) -- matching
      #sasfit_oz_solver.c's own convention of reusing the ALPHA/self.alpha
      #storage slot for whatever the active closure's own single free
      #parameter is (same slot BPGG/CJVM/BB use, just a different role
      #here). eta<=0.02 there switches the bridge off entirely (BRIDGE=0,
      #i.e. MHNC degenerates to plain HNC) -- mirrored here exactly.
      self.closureType = 'MHNC'
      self.alpha = float(eta)
      if eta > 0.02:
          self.bridgeMHNC = self._analyticalPYbridgeFunction(eta)
      else:
          self.bridgeMHNC = np.zeros(self.numberOfRadialSamplingPoints)

    def setZSEPclosure(self, zeta, phi, alpha):
      #Lee's zero-separation-theorem closure -- see update_c()'s own
      #'ZSEP' branch for the formula. Unlike the single-parameter
      #closures above (which reuse self.alpha as a shared storage
      #slot), ZSEP genuinely needs three independent parameters at
      #once, so it gets its own three attributes rather than
      #overloading self.alpha. Usually not called directly -- see
      #OZsolver.fitZSEPparameters() for fitting these three parameters
      #to the exact conditions from Lee (1995).
      self.closureType = 'ZSEP'
      self.zsep_zeta = float(zeta)
      self.zsep_phi = float(phi)
      self.zsep_alpha = float(alpha)

      
    #Standard getters (returning members)
    def getNumberOfRadialSamplingPoints(self):
      return self.numberOfRadialSamplingPoints
    
    def getDelta_r(self):
      return self.Delta_r
    
    def getHardSphereDiameter(self):
      return self.hardSphereDiameter

    def getHardSphereDiameterInPoints(self):
      return self.hardSphereDiameterInPoints

    def getrArray(self):
      #We start at Delta_r, not 0, due to numerical reasons.
      #
      #_rArrayOverride exists so that a ONE-component potential setter can be
      #re-used to build one pair of a MULTICOMPONENT potential:
      #setPolydispersePotential() points this at r/sigma_ij and calls the
      #ordinary setter, which then evaluates its tail at the reduced
      #separation and places its hard core at sigma_ij (every setter measures
      #the core against self.hardSphereDiameter, which is 1 in those reduced
      #units). That reuses all eighteen already-validated one-component
      #setters instead of rewriting each of them for pair matrices.
      if getattr(self, '_rArrayOverride', None) is not None:
          return self._rArrayOverride
      return self.Delta_r*(np.arange(self.numberOfRadialSamplingPoints).astype('float') + 1.0)

    def getqArray(self):
      #S(Q) (getSq(), on OZsolver) lives on this reciprocal-space grid,
      #NOT on getrArray()'s real-space one -- same construction
      #(starting at delta_q, not 0, for the same numerical reason as
      #getrArray()), matching hankelTransform()'s/inverseHankelTransform()'s
      #own delta_q. Found by testing/inspection that a GUI built on top
      #of this file was plotting/exporting S(Q) against the WRONG
      #(real-space) x-axis before this existed, since there was no
      #general q-array getter to use instead -- index i here and in
      #getrArray() correspond to the same Hankel-transform grid point,
      #just expressed in q vs r, so a CSV/ASCII export can safely put
      #both as separate columns of the same table.
      delta_q = np.pi/((self.numberOfRadialSamplingPoints + 1.0)*self.Delta_r)
      return delta_q*(np.arange(self.numberOfRadialSamplingPoints).astype('float') + 1.0)

    def getVolumeDensity(self):
      return self.volumeDensity
      
    def getParticleNumberDensity(self):
      return self.particleDensity
    
    def getpotentialByName(self):
      return self.activePotentialname
    
    #LJ
    def getEpsilonInkTUnits(self):
      return self.epsilonInkTUnits
    
    #Yukawa
    def getShieldingConstantInSigmaUnits(self):
      return self.shieldingLengthInSigmaUnits
    
    def getInteractionStrengthInKTunits(self):
      return self.interactionStrengthInKTunits

    def getIsHardSphereAdded(self):
      return self.isHardSphereAdded
    
#Finished Fix point operator class
