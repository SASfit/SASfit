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
    def __init__(self, port, **kwargs):
      #member variable (iteration counter, incremented in call back function of optimizer)
      self.iterationStep = 0     
      #Run constructor of super class (start server, so we won't return, do not place
      #any other statements after the __init()__ of the constructor, or otherwise, start
      #RPC server as a daemon, see e.g here: https://gist.github.com/amitsaha/4969431)
      OZsolver.__init__(self, port, **kwargs)
      
      
    #Call back function for solver function, counts number of iteration steps and checks for interrupt
    def iterationCounter(self, x, f):
      self.iterationStep += 1
      if self.isInterrupted: 
          raise Exception('interrupted')

           
    #implement the abstract method such that this class can be instantiated
    def solve(self):
      try:
          #alpha SETS THE INITIAL INVERSE-JACOBIAN APPROXIMATION, as -1/alpha.
          #Leaving it at scipy's default (None) makes this solver 15 to 66
          #times SLOWER on this problem -- it was the slowest of the five
          #available solvers, and with alpha = 1.0 it is the fastest.
          #Measured, five classes, 160 Q points, default vs alpha = 1.0:
          #
          #  HardSphere phi=0.20 s=0.20    30.59 s ->  0.50 s   61.6x
          #  HardSphere phi=0.40 s=0.10    96.34 s ->  1.46 s   66.2x
          #  HardSphere phi=0.30 s=0.30    34.22 s ->  1.64 s   20.8x
          #  SquareWell phi=0.20 s=0.20    36.53 s ->  0.63 s   57.5x
          #  Yukawa     phi=0.20 s=0.20     8.22 s ->  0.53 s   15.4x
          #
          #The fixpoint is UNCHANGED -- I_max agrees to 1e-9 or better in
          #every case -- so this is purely a convergence rate, not accuracy.
          #
          #The sign matters and is instructive: NEGATIVE alpha is worse than
          #the default (alpha = -0.5 gave 14.0 s, -0.2 gave 4.8 s) while
          #positive alpha is much better. That is consistent with the OZ
          #fixpoint map being contractive here, so a simple positive scaling
          #is already a good Jacobian guess.
          #
          #NOTE the damping-like parameter is alpha, NOT beta -- beta is
          #silently swallowed by anderson()'s **kw and then raises.
          x_fp = anderson(self.rootOperator, self.x_0, alpha = 1.0,
                          maxiter = self.numberOfIterations, verbose=0,
                          callback=self.iterationCounter,
                          x_rtol=self.convergenceCriterion )
      except Exception as inst:
          print(type(inst)) #Typically interrupt or non-convergence
          return
      print("scipy Anderson converged after" , self.iterationStep, "steps")
      #As a side effect, we calculate other intersting physical quantities based on the found fixpoint solution
      self.derivePhysicalQuantitiesFromFixpoint(x_fp)
      #Reset iteration counter in case solve is called various times (and class is not re-instantiated)
      self.iterationStep = 0
