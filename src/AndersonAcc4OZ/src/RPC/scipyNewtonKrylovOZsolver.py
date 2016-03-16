# -*- coding: utf-8 -*-
#calculations
import numpy as np
#scipy newton_krylovn
from scipy.optimize import newton_krylov
#Base class
from oZsolver import OZsolver

#This class inherits all that is needed to run the 
#iteration and to communicate with clients from OZsolver
class ScipyNewtonKrylovOZsolver(OZsolver):
    def __init__(self, port):
      #member variable (iteration counter, incremented in call back function of optimizer)
      self.iterationStep = 0     
      #Run constructor of super class (start server, so we won't return, do not place
      #any other statements after the __init()__ of the constructor, or otherwise, start
      #RPC server as a daemon, see e.g here: https://gist.github.com/amitsaha/4969431)
      OZsolver.__init__(self, port)
      
      
    #Call back function for solver function, only counts number of iteration steps
    def iterationCounter(self, x, f):
      self.iterationStep += 1

    #implement the abstract method such that this class can be instantiated
    def solve(self):
      x_fp = newton_krylov(self.rootOperator, self.x_0, maxiter = self.numberOfIterations, verbose=0, x_rtol=self.convergenceCriterion, method='lgmres', callback=self.iterationCounter)
      print "scipy Newton Krylov converged after" , self.iterationStep, "steps"
      #As a side effect, we calculate other intersting physical quantities based on the found fixpoint solution
      self.derivePhysicalQuantitiesFromFixpoint(x_fp)
      #Reset iteration counter in case solve is called various times (and class is not re-instantiated)
      self.iterationStep = 0     
      
