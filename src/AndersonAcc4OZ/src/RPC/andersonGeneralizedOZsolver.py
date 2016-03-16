# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*
'''
The generalized Anderson method
is generalized in so far that not
the oldest vector is replaced but the one
which has the largest alpha-contribution.
(which will mostly be the last one)
'''
#calculations
import numpy as np
#least square
from scipy import linalg

#Base class
from oZsolver import OZsolver
#This class inherits all that is needed to run the 
#iteration and to communicate with clients from OZsolver
class AndersonGeneralizedOZsolver(OZsolver):
    def __init__(self, port):
      #First run constructor of super class
      OZsolver.__init__(self, port)
  
    def calculateW(self, k):
      W = np.zeros((k, k - 1))
      for i in range(k):
          for j in range(k - 1):
              if i == j: 
                  W[i,j] = -1.0
              if i - 1 == j:
                  W[i,j] =  1.0
              
      return W
    
              
    #define the specific iteration schema for this class    
    def andersonAcceleration(self):
    
      maximalDimensionOfKrylovSpace = 5
      indexOfcolumnVectorToBeReplaced = -1 #-1 stands for default (none)
      
      isMaximalDimensionOfKrylovSpaceReached = False
      dimensionOfKrylovSpace = 0
      b = 1.0 #small b values are not good (and choosing argmin does not help either)
      
      #Start iteration
      G = self.x_0
      i = 0
      while  i < self.numberOfIterations and not self.isInterrupted:
          if dimensionOfKrylovSpace < maximalDimensionOfKrylovSpace:
              dimensionOfKrylovSpace += 1
          else:
              isMaximalDimensionOfKrylovSpaceReached = True
            
          G_old = G #save previuos state
          G_new = self.fixPointOperator(G)

          #np.newaxis makes 2D array which can be transposed. Mathematically,
          #this is like interpreting a vector in R^n as Matrix in R^{n x 1}
          G_new_AsColumnVector = G_new[np.newaxis].T 
                                                     
          #Save state G_new in Matrix K
          if not isMaximalDimensionOfKrylovSpaceReached:
              if dimensionOfKrylovSpace == 1:
                  #init K
                  K = G_new_AsColumnVector
              else:
                  #addColumnToMatrixByExtension
                  K = np.concatenate((K, G_new_AsColumnVector), axis=1)

          else:
              if indexOfcolumnVectorToBeReplaced < 0:
                  #addColumnToMatrixByShifting
                  K = np.roll(K, -1, axis =1)
                  K[:,-1] = G_new
              else:
                  K[:,indexOfcolumnVectorToBeReplaced] = G_new

          #save residuum r in Matrix D
          r = G_new - G_old
          r_AsColumnVector = r[np.newaxis].T #Is now 2D array 
          if not isMaximalDimensionOfKrylovSpaceReached:
              if dimensionOfKrylovSpace == 1:
                  #init D
                  D = r_AsColumnVector
              else:
                  #addColumnToMatrixByExtension
                  D = np.concatenate((D, r_AsColumnVector), axis=1)
          else:
              if indexOfcolumnVectorToBeReplaced < 0:
                  #addColumnToMatrixByShifting, replace oldest
                  D = np.roll(D, -1, axis =1)
                  D[:,-1] = r
              else:
                  D[:,indexOfcolumnVectorToBeReplaced] = r

          if dimensionOfKrylovSpace == 1:
              G = G_new
              continue
              
          #Only calculate W if needed   
          if not isMaximalDimensionOfKrylovSpaceReached:
              W = self.calculateW(dimensionOfKrylovSpace)

          #Start solving least square problem 
          D_reduced = np.dot(D, W) #D_reduced = DW
          #solve argmin(a) ||D_reduced a + r ||
          a_reduced, resid, rank, sigma = linalg.lstsq(D_reduced, -r)
          a_AsColumnVector = np.dot(W, a_reduced[np.newaxis].T)
          a_AsColumnVector[-1] += 1.0
          indexOfcolumnVectorToBeReplaced = np.argmax(np.abs(a_AsColumnVector))
          G_AsColumnVector = np.dot(K, a_AsColumnVector) - (1.0 - b)*np.dot(D, a_AsColumnVector)
          G = G_AsColumnVector[:,0]
          
          #We only check for convergence if K and D don't grow anymore
          if isMaximalDimensionOfKrylovSpaceReached:
              norm = np.linalg.norm(G_new)
              previousNorm = np.linalg.norm(G_old)
              relativeProgress = abs(previousNorm - norm)/(norm + np.finfo(float).eps)
              if relativeProgress < self.convergenceCriterion: #Defined in super class
                  #pass
                  print "Generalized Anderson converged after", i, "steps"
                  break

          i += 1
      #end while
      if i == self.numberOfIterations:
          print "Anderson did not converge after", self.numberOfIterations, "steps"
    
      self.derivePhysicalQuantitiesFromFixpoint(G)
    #implement the abstract method such that this class can be instantiated
    def solve(self):
      self.andersonAcceleration()
      
