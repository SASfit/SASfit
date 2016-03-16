# -*- coding: utf-8 -*-
#calculations
import numpy as np

#Base class
from oZsolver import OZsolver
#This class inherits all that is needed to run the 
#iteration and to communicate with clients from OZsolver
class PicardOZsolver(OZsolver):
    def __init__(self, port):
      #First run constructor of super class
      OZsolver.__init__(self, port)

    #define the specific iteration schema for this class    
    def picardIteration(self):
      #Start iteration
      x = self.x_0
      #Simple Picard iteration max number of steps = self.numberOfIterations
      #Converged if relative progress is less than self.convergenceCriterion
      i = 0; previousNorm = 0.0
      while  i < self.numberOfIterations and not self.isInterrupted:
          x = self.fixPointOperator(x)
          norm = np.linalg.norm(x)
          relativeProgress = abs(previousNorm - norm)/(norm + np.finfo(float).eps)
          previousNorm = norm
          if relativeProgress < self.convergenceCriterion: #Defined in super class
              #pass
              print "Picard converged after", i, "steps"
              break
          i += 1
          
      #endwhile
      if i == self.numberOfIterations:
          print "Picard did not converge after", self.numberOfIterations, "steps"

      self.derivePhysicalQuantitiesFromFixpoint(x)
    #implement the abstract method such that this class can be instantiated
    def solve(self):
      self.picardIteration()
      
