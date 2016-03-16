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
'''

#calculations
import numpy as np

#Server
import xmlrpclib
from SimpleXMLRPCServer import SimpleXMLRPCServer

#solve() is abstract method
from abc import ABCMeta, abstractmethod

#Base class
from  oZfixpointOperator import OZfixpointOperator

class OZsolver(OZfixpointOperator):
    #Init needs to be given by constructor, all other members can be set via RPC later
    #but port needs to be known in advance by the client. (Will be defined by the client)
    def __init__(self, port):
      OZfixpointOperator.__init__(self, port)
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
      
    #We know a analytical OZ solution for hard sphere particles and Percus Yevick closure 
    #********************************************************************************************************
    def calculateAnalyticalRDFsolutionForHS(self):
      if self.isHNC or not (self.getpotentialByName() == "HardSphere"):
          print "analytical solution is only available for PY with HS"
          return
      sigma = self.getHardSphereRadius()
      numberOfRadialSamplingPoints = self.getNumberOfRadialSamplingPoints()
      delta_r = self.getDelta_r()
      hardSphereRadiusInPoints = self.getHardSphereRadiusInPoints()
      rho_V = self.getVolumeDensity()
      r = self.getrArray()
      c = np.zeros(numberOfRadialSamplingPoints).astype('float')
      c[hardSphereRadiusInPoints:] = 0.0 #Not needed (c = np.zeros), just to emphasize definition of c (c = 0 for r > sigma)
      r_HSR = r[:hardSphereRadiusInPoints]
      c[:hardSphereRadiusInPoints] = ( 6.0*rho_V*(1.0 + 0.5*rho_V)**2*(r_HSR/sigma) - (1.0 + 2.0*rho_V)**2*(1 + 0.5*rho_V*(r_HSR/sigma)**3) ) / (1.0 - rho_V)**4
      c_hat = self.hankelTransform(c, delta_r)
      G_hat = c_hat/(1.0 - self.particleDensity*c_hat) - c_hat
      G = self.inverseHankelTransform(G_hat, delta_r)
      g = G + c + 1.0
      g[:hardSphereRadiusInPoints] = 0.0 #Here we must set it 
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
      
    def calculateAnalyticalStructureFactorArrayForHS(self):
      if self.isHNC or not (self.getpotentialByName() == "HardSphere"):
          print "analytical solution is only available for PY with HS"
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
      #g[:self.hardSphereRadiusInPoints] = 0.0  #This is only true for potentials with a hard sphere part given
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
         print "Size of initial array is incorrect, not set"
         return
         
      if not self.doUseGammaFixPointOperator and (x_0.size != 2*self.numberOfRadialSamplingPoints):
         print "Size of initial array is incorrect, not set"
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
 
