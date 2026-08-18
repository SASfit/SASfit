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
Already-ported potentials (unchanged): HardSphere, LennardJones, Yukawa, Star.

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
      #neither keeps this class's original behaviour exactly (4096
      #points, 100 points per hard-sphere diameter).
      #
      #Named "diameter", not "radius": what this grid parameter
      #actually measures is sigma, the pairwise CONTACT distance
      #between two touching spheres -- i.e. the diameter, not a single
      #sphere's own geometric radius (confirmed directly: g(r)=0 for
      #r<=sigma, jumping to its characteristic nonzero contact-peak
      #value at r=sigma+, exactly the excluded-volume condition for two
      #spheres of DIAMETER sigma, not radius sigma).
      self.numberOfRadialSamplingPoints = (numberOfRadialSamplingPoints
                                            if numberOfRadialSamplingPoints is not None else 128*32)   #(2^7*2^5 = 2^12)
      #self.hardSphereDiameterInPoints = self.numberOfRadialSamplingPoints/2**3
      self.hardSphereDiameterInPoints = (hardSphereDiameterInPoints
                                          if hardSphereDiameterInPoints is not None else 100)
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
          return EN*(1.0 + (np.exp(f*G) - 1.0)/f) - G - 1.0

      elif ct == 'HMSA':
          #Same as RY but using Gstar = G - beta*U_attractive(r) in the
          #switching term, and the full EN in the exponential prefactor
          #(see J.K. docu section 5.4.16) -- algebraically identical to
          #exp(-U_repulsive)*(1+(exp(f*Gstar)-1)/f), used here instead
          #since self.repulsivePartOfP2Ppotential/attractivePartOfP2Ppotential
          #are already split out by the potential setters that support it.
          f = self.helperRYfunction
          Gstar = G - self.attractivePartOfP2Ppotential
          return np.exp(-self.repulsivePartOfP2Ppotential)*(1.0 + (np.exp(f*Gstar) - 1.0)/f) - G - 1.0

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
          #Kovalenko-Hirata closure -- ported directly from
          #sasfit_oz_solver.c's own "case KH:" block. Note (found by
          #reading that C code carefully, not assumed from the
          #textbook definition): its c(r)/g(r) formula there is
          #IDENTICALLY the same expression as HNC's own
          #(EN*exp(G)-G-1) -- the piecewise LINEARIZED bridge function
          #this closure is normally defined by in the literature
          #(g(r)=exp(gamma-betaU) where gamma-betaU<=0, else
          #g(r)=1+gamma-betaU, avoiding HNC's own divergence risk in
          #the strongly-repulsive/high-density regime) is computed in
          #that C code too, but only as a separate diagnostic BRIDGE[i]
          #output there -- it is never fed back into c[i] in that same
          #closure branch. Ported exactly as the C code actually
          #computes c(r)/g(r), not "corrected" to the textbook KH
          #definition, since the point of this port is to reproduce
          #SASfit's own numbers exactly, quirks included.
          return EN*np.exp(G) - G - 1.0

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
      numberOfSamplingPoints = f.shape[0]
      lr = np.arange(numberOfSamplingPoints).astype('float')
      lr += 1.0 #Division by zero (see over-next line, this is the reason why the grid starts at delta_r, not zero....)
      f_hat = dst(f*lr, type=1) #...but the formula is also correct (implemented as 5.18, 5.19 in SASfit docu, see also...)
      f_hat /= lr               # ..comment: it is important to mention that first elements equal delta_r, delta_q (and not at 0.0!)
      delta_q = np.pi/((numberOfSamplingPoints + 1.0)*delta_r)
      f_hat *= 2*np.pi*delta_r**2/delta_q
      return f_hat
      
    def inverseHankelTransform(self, f, delta_r):
      numberOfSamplingPoints = f.shape[0]
      delta_q = np.pi/((numberOfSamplingPoints + 1.0)*delta_r)
      return delta_q**3*self.hankelTransform(f, delta_r)/( (2*np.pi)**3*delta_r**3 )

    def isotropic3Dconvolution(self, f_1, f_2, delta_r):
      hat_f_1 = self.hankelTransform(f_1, delta_r)
      hat_f_2 = self.hankelTransform(f_2, delta_r)
      return self.inverseHankelTransform(hat_f_1*hat_f_2, delta_r)
      
    #FixPoint Operator (In 'Gamma' space). Note that also for hard spheres Gamma is continuous everywhere
    #(I.e also at sigma. x =[c,h] is not continuous at r = sigma, this may explain the much better 
    #numerical behaviour of the Gamma fixPointOperator in the root finding algorithms. Theory: Naegele, p.45)
    #*******************************************************************************************************
    def fixPointOperatorForGamma(self, G):
      c_new = self.update_c(G)
      c_hat = self.hankelTransform(c_new, self.Delta_r)
      #G_new = inverseHankelTransform( (rho*c_hat**2)/(1.0 - rho*c_hat) )
      G_hat = c_hat/(1.0 - self.particleDensity*c_hat) - c_hat
      #G_hat[np.isinf(G_hat)] = 0.0; G_hat[np.isneginf(G_hat)] = 0.0; G_hat[np.isnan(G_hat)] = 0.0
      G_new = self.inverseHankelTransform(G_hat, self.Delta_r)
      #G_new[np.isinf(G_new)] = 0.0; G_new[np.isneginf(G_new)] = 0.0; G_new[np.isnan(G_new)] = 0.0
      return (G_new, c_new)
      
      

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
    


    #Star Potential (Likos)
    def setStarPotential(self, NumberOfArms):
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
      numberOfArgumentsNeeded = len(inspect.getfullargspec(methodToCall)[0]) - 1 # minus self
      #print(numberOfArgumentsNeeded)
      if not args:
          if numberOfArgumentsNeeded != 0:
              print("number of arguments given for", potentialType, "is not correct")
              return
      else:
          if len(args) != numberOfArgumentsNeeded:
              print("number of arguments given for", potentialType, "is not correct")
              return
          
      #If everything is fine, we call the setter for this potential type with its parameters,
      #this would be e.g. setLennardJonesPotential(0.8)
      methodToCall(*args)
      self.activePotentialname = potentialType

      
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
