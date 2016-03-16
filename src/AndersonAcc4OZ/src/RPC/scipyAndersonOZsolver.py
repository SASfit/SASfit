# -*- coding: utf-8 -*-
#This class is an implementation
#of the abstract OZ solver class
import numpy as np
#scipy anderson
from scipy.optimize import anderson
#Base class
from oZsolver import OZsolver

#This class inherits all that is needed to run the 
#iteration and to communicate with clients from OZsolver
class ScipyAndersonOZsolver(OZsolver):
    def __init__(self, port):
      #member variable (iteration counter, incremented in call back function of optimizer)
      self.iterationStep = 0     
      #Run constructor of super class (start server, so we won't return, do not place
      #any other statements after the __init()__ of the constructor, or otherwise, start
      #RPC server as a daemon, see e.g here: https://gist.github.com/amitsaha/4969431)
      OZsolver.__init__(self, port)
      
      
    #Call back function for solver function, counts number of iteration steps and checks for interrupt
    def iterationCounter(self, x, f):
      self.iterationStep += 1
      if self.isInterrupted: 
          raise Exception('interrupted')

           
    #implement the abstract method such that this class can be instantiated
    def solve(self):
      try:
          x_fp = anderson(self.rootOperator, self.x_0, maxiter = self.numberOfIterations, verbose=0, callback=self.iterationCounter, x_rtol=self.convergenceCriterion )
      except Exception as inst:
          print type(inst) #Typically interrupt or non-convergence
          return
      print "scipy Anderson converged after" , self.iterationStep, "steps"
      #As a side effect, we calculate other intersting physical quantities based on the found fixpoint solution
      self.derivePhysicalQuantitiesFromFixpoint(x_fp)
      #Reset iteration counter in case solve is called various times (and class is not re-instantiated)
      self.iterationStep = 0
      
