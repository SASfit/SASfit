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
'''

#calculations
import numpy as np
from scipy.fftpack import dst, idst

#Server
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer

#Setting (and getting) Potentials (Potential names)
import inspect


class OZfixpointOperator:
    #Init needs to be given by constructor, all other members can be set via RPC later
    #but port needs to be known in advance by the client. (Will be defined by the client)
    def __init__(self, port):
      #First computational stuff...
      #global constants (We don't make them 'settable')
      #*******************************************************************************
      self.numberOfRadialSamplingPoints = 128*32   #(2^7*2^5 = 2^12)
      #self.hardSphereRadiusInPoints = self.numberOfRadialSamplingPoints/2**3
      self.hardSphereRadiusInPoints = 100
      #Length units is such that the 'box' has length 1
      #self.rLength = 1.0 #arbitrary
      #rLength is defined in multiples of hardSphereRadius
      self.rLength = (self.numberOfRadialSamplingPoints - 1.0)/self.hardSphereRadiusInPoints #defined such that...
      #Calculate spacing in this length units
      self.Delta_r = self.rLength/(self.numberOfRadialSamplingPoints - 1.0) #....Delta_r = 1.0/hardSphereRadiusInPoints 
      #Sphere radius (also for LJ)
      self.hardSphereRadius = self.hardSphereRadiusInPoints*self.Delta_r
      
      #Default values for member variables:
      #************************************
      self.isHNC = False #The bridge function is part of this algorithm, whereas the potential is not (or less)
      self.isHMSA = False #Closure relation alternative to Rogers Young well suited for Lennard Jones
      #Do not modify the *private* variable alpha, use the corresponding setter (doRYclosure)
      self.alpha = -1.0 #Rogers Young and HMSA closure, negative value means deactivated
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
          self.rpcServer.register_function(self.getHardSphereRadiusInPoints, 'getHardSphereRadiusInPoints')
          self.rpcServer.register_function(self.getHardSphereRadius, 'getHardSphereRadius')
          self.rpcServer.register_function(self.getNumberOfRadialSamplingPoints, 'getNumberOfRadialSamplingPoints')
          self.rpcServer.register_function(self.getDelta_r, 'getDelta_r')
      
      
    #Helper functions
    #************************************************************************************
    #Volume to particle density
    def transformVolume2ParticleNumberDensity(self, rho_V):
      rho = rho_V/( (4*np.pi/3.0)*(0.5*self.hardSphereRadius)**3 ) #rho_V = rho*V_atom
      return rho
    
    #If Gamma is given calculate (updated) c (according bridge function)
    def update_c(self, G):
      #Here is the only place where the bridge function /closure comes into play,
      #so this is encapsulated. (Only here self.isHNC is used, so it might be preferred to
      #hand over the member variable to the function as a argument). TODO:
      #It would be cleaner to define an abstract class, e.g Closure(OZfixpointoperator) with (abstract) class
      #method 'update_c'. A specific closure relation would then be a derived class of 'Closure' with the specific
      #update_c method overwritten/implemented. An instance 'closure' of the specific closure class (e.g PYclosure) 
      #would then be a member variable of this class, such that this method would be replaced by self.closure.update_c(G).
      if self.alpha >= 0.0:
          f = self.helperRYfunction
          if self.isHMSA: #See J.K docu, section 5.4.16
              #G_star = G - self.repulsivePartOfP2Ppotential 
              #c_new = self.boltzmannOfP2Ppotential*(1.0 + (np.exp(f*G_star) - 1.0)/f) - G_star - 1.0
             c_new = np.exp(-self.repulsivePartOfP2Ppotential)*(1.0 + (np.exp(f*(G - self.attractivePartOfP2Ppotential)) - 1.0)/f) - G - 1.0
          else: #RY
              c_new = self.boltzmannOfP2Ppotential*(1.0 + (np.exp(f*G) - 1.0)/f) - G - 1.0
          return c_new
      #Hence not RY closure, so either HNC or PY is left
      if not self.isHNC:
          c_new = self.boltzmannOfP2Ppotential*(1.0 + G) - G - 1.0
      else:
          c_new = self.boltzmannOfP2Ppotential*np.exp(G) - G - 1.0
      return c_new
    
    #In the grand canonical ensemble and for HS + PY, there is a analytical form of S(q=0)
    #Theory: Naegele, AMAS lecture notes on colloidal soft matter, eq. 4.64
    #In the canonical enemble, S(q=0) = 0, hence the name
    def calculateZeroQlimitOfStructureFactor(self, rho_V):
      return (1.0 - rho_V)**4/(1 + 2.0*rho_V)**2
    
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
        c = x[:x.shape[0]/2]
        return c
        
    def get_h_From_ch(self, x):
        h = x[x.shape[0]/2:]
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
      x_new[:x.shape[0]/2] = c_new
      x_new[x.shape[0]/2:] = h_new
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
              #print methodName[len('set'):-len('Potential')]
      #print '***************************************************'
      return potentialNameList
    
    #Print out a list of the characteristic potential names
    def printPotentialSetterArguments(self):
      methodTupelList = inspect.getmembers(self, predicate=inspect.ismethod)
      print "***************************************************"
      print "Available Potentials with corresponding Parameters:"
      print "***************************************************"
      for methodTupel in methodTupelList:
          methodName = methodTupel[0]
          methodToCall = getattr(self, methodName)
          if methodName.startswith('set') and methodName.endswith('Potential'):
              print "Potential Name:", methodName[len('set'):-len('Potential')], "; Arguments:", inspect.getargspec(methodToCall)[0][1:]
      print "************************************************************************************************************************"

    
    #Implementation of the specific potential setters, these could be called directly, but it is
    #preferred to call them indirectly by the common API 'setPotentialByName'.
    #Hard Sphere
    def setHardSpherePotential(self):
      self.boltzmannOfP2Ppotential[:] = 1.0
      self.boltzmannOfP2Ppotential[:self.hardSphereRadiusInPoints] = 0.0
   
    #Lennard Jones
    def setLennardJonesPotential(self, epsilonInkTUnits):
      #epsilonInkTUnits may as well be a string
      try:
          epsilonInkTUnits = float(epsilonInkTUnits)
      except ValueError:
          print "epsilonInkTUnits can not be converted to float"
          return
      #Side effect: We record the value in a member variable (was initialized with a negative number)
      self.epsilonInkTUnits = epsilonInkTUnits
      #calculate potential
      sigma = self.hardSphereRadius
      r = self.getrArray()
      potential_LJ = (sigma/r)**12 - (sigma/r)**6
      #Since Potential so far is unitless, we also want this for the energy
      #hence we express epsilon (energy scale of potential) in kT unit
      #binding energy. For e.g. liquid Argon: e/k ~ 120 K, e/(kT) ~ 1.2
      #(http://www.sklogwiki.org/SklogWiki/index.php/Argon)
      potential_LJ *= 4.0*epsilonInkTUnits
      #pl.plot(potential_LJ[hardSphereRadius -2:]); pl.show()
      self.boltzmannOfP2Ppotential = np.exp(-potential_LJ)
      #calculate also repulsive part , see J.K docu, page 5.102
      minmumPositionInPoints = int(2**(1.0/6.0)*self.getHardSphereRadiusInPoints())
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
          print "shieldingLengthInSigmaUnits or interactionStrengthInKTunits can not be converted to float"
          return
      #Boolean value doAddHardSphere may also be string, if so, we convert to Boolean type
      if isinstance(doAddHardSphere, basestring):
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
      sigma = self.getHardSphereRadius()
      potential_Y = np.exp(-r/shieldingLengthInSigmaUnits)/(r/sigma ) #so far dimension less
      self.boltzmannOfP2Ppotential = np.exp(-interactionStrengthInKTunits*potential_Y)
      if doAddHardSphere:
          sigmaInPoints = self.getHardSphereRadiusInPoints()
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
          print "NumberOfArms can not be converted to int"
          return

      sigma = self.hardSphereRadius
      r = self.getrArray()
      potential_Star =   5.0/18.0*np.power(NumberOfArms,3.0/2.0)* 1.0/(1.0+np.sqrt(NumberOfArms)/2.0)* sigma/r* np.exp(-np.sqrt(NumberOfArms)*(r-sigma)/(2.0*sigma))
      potential_Star[:self.hardSphereRadiusInPoints] =   5.0/18.0*np.power(NumberOfArms,3.0/2.0)* (-np.log(r[:self.hardSphereRadiusInPoints]/sigma)+1.0/(1.0+np.sqrt(NumberOfArms)/2.0))
      #if r<=sigma :
          #potential_Star[:self.hardSphereRadiusInPoints] =   5.0/18.0*pow(NumberOfArms,3.0/2.0)* (-log(r/sigma)+1.0/(1.0+sqrt(NumberOfArms)/2.0))
      #else:
          #potential_Star =   5.0/18.0*pow(NumberOfArms,3.0/2.0)* 1.0/(1.0+sqrt(NumberOfArms)/2.0)* sigma/r* exp(-sqrt(NumberOfArms)*(r-sigma)/(2.0*sigma))
      #pl.plot(potential_Star[hardSphereRadius -2:]); pl.show()
      self.boltzmannOfP2Ppotential = np.exp(-potential_Star)

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
          print "unrecognized potential"
          return
      methodName = 'set' + potentialType + 'Potential'
      methodToCall = getattr(self, methodName)
      #Then we check if the correct number of arguments was given for this potential type
      numberOfArgumentsNeeded = len(inspect.getargspec(methodToCall)[0]) - 1 # minus self
      #print numberOfArgumentsNeeded 
      if not args:
          if numberOfArgumentsNeeded != 0:
              print "number of arguments given for", potentialType, "is not correct"
              return
      else:
          if len(args) != numberOfArgumentsNeeded:
              print "number of arguments given for", potentialType, "is not correct"
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

    def doHNCclosure(self):
      self.isHNC = True
      #Switch off RY via the implicit (negative alpha) definition
      self.alpha = -1.0
      
    def doRYclosure(self, alpha):
      if not alpha >= 0.0:
          print "alpha must be positive real number"
          return
      self.isHMSA = False
      self.alpha = float(alpha)
      #side effect: define function which interpolates between PY and HNC
      r = self.getrArray()
      self.helperRYfunction = 1.0 - np.exp(-self.alpha*r)
      
    def doHMSAclosure(self, alpha):
      if not alpha >= 0.0:
          print "alpha must be positive real number"
          return   
      self.isHMSA = True
      self.alpha = float(alpha)
      #side effect: define function which interpolates between PY and HNC
      r = self.getrArray()
      self.helperRYfunction = 1.0 - np.exp(-self.alpha*r)

      
    #Standard getters (returning members)
    def getNumberOfRadialSamplingPoints(self):
      return self.numberOfRadialSamplingPoints
    
    def getDelta_r(self):
      return self.Delta_r
    
    def getHardSphereRadius(self):
      return self.hardSphereRadius
      
    def getHardSphereRadiusInPoints(self):
      return self.hardSphereRadiusInPoints
      
    def getrArray(self):
      #We start at Delta_r, not 0, due to numerical reasons.
      return self.Delta_r*(np.arange(self.numberOfRadialSamplingPoints).astype('float') + 1.0)
        
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


