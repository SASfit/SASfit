# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*
'''
This code is to solve the OZ equation
for critically high densities. The strategy
is to start from moderate densities where
the algorithm is known to converge and then
use the current solution as start value for the next
iteration (which solves the OZ equation 
with increased density). The math background for
this to work is that the OZ solutions g(r; d) depend
in a continuous way from the density parameter d, i.e
a RDF solution g(r; d) for density d is supposed to be 
already 'close' (and hence a good start value) 
to the solution g(r; d + delta_d). But even for
a continuous g(r; d), there is no guarantee that the
initial search direction is correct. Experiments show
that a zero start value may be better than the previous.
'''
import numpy as np

#Solver
from scipyAndersonOZsolver import ScipyAndersonOZsolver

#plot
import matplotlib.pyplot as plt
import pylab as pl

if __name__ == '__main__':
  

  solver = ScipyAndersonOZsolver(port = 0)
  solver.setPotentialByName('HardSphere')
  #solver.setNumberOfIterations(10*solver.getNumberOfIterations())
  #Relaxing ConvergenceCriterion by a factor 1000
  solver.setConvergenceCriterion(1.0e3*solver.getConvergenceCriterion())
  #solver.setPotentialByName('LennardJones', 0.5)

  startDensity = 0.58
  targetDensity = 0.63
  totalDeltaDensity = targetDensity - startDensity
  
  density = startDensity
  deltaDensity = totalDeltaDensity
  
  decayFactor = 2.5
  deltaDensity = 0.01
  
  print "start approaching target density", targetDensity
  while density < targetDensity:
    #deltaDensity /= decayFactor
    density += deltaDensity
    print "current density", density
    solver.setVolumeDensity(density)
    solver.solve()
    g = solver.getRDF()
    solver.setStartValue(g)
   
   
  #Direct calculation
  print "Directly calculating RDF for target density", targetDensity
  solver.setStartValue(np.zeros(solver.getNumberOfRadialSamplingPoints()) )
  solver.setVolumeDensity(targetDensity)
  solver.solve()
  g_direct = solver.getRDF()
  
  #Plot results
  r = np.arange(solver.getNumberOfRadialSamplingPoints() ).astype('float') + 1.0
  r *= solver.getDelta_r()/solver.getHardSphereRadius()

  pl.plot(r, g, label = 'RDF obtained by iterating over densities' );
  pl.plot(r, g_direct, label = 'RDF obtained directly' );
  pl.xlabel('r/sigma'); pl.ylabel('g(r)')
  pl.legend(loc = 'upper right')
      
  pl.show()

