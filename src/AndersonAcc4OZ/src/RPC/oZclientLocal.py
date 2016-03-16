# -*- coding: utf-8 -*-
import numpy as np

#plot
import matplotlib.pyplot as plt
import pylab as pl

#performance measurement
import time

#Solver
from picardOZsolver import PicardOZsolver
from andersonOZsolver import AndersonOZsolver
from scipyAndersonOZsolver import ScipyAndersonOZsolver
from scipyNewtonKrylovOZsolver import ScipyNewtonKrylovOZsolver

if __name__ == '__main__':
  
  #Instantiate solver class (Picard iteration, etc.)
  #For star potential, ScipyAndersonOZsolver gives best result
  #s = PicardOZsolver(port = 0)
  #s = AndersonOZsolver(port = 0)
  s = ScipyAndersonOZsolver(port = 0)
  #s = ScipyNewtonKrylovOZsolver(port = 0)
  s.printPotentialSetterArguments()
  #s.setPotentialByName('HardSphere')
  #s.setPotentialByName('LennardJones', 0.2)
  s.setPotentialByName('Star', 20)
  
  #Define the density range to scan
  densityRange = np.arange(0.3, 0.4, 0.1)
  #densityRange = np.asarray([0.3])
  #densityRange = np.asarray([0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5])
  

  densityRDFdictionary = {}
  
  #Start loop over densities and measure wall clock time
  t_start = time.time()
  
  for d in densityRange:
      s.setVolumeDensity(float(d))
      s.solve()
      g = s.getRDF()
      densityRDFdictionary[d] = g

  t_stop = time.time()
  
  #Print out benchmark
  print "time used for", densityRange.size, "density parameters:", t_stop - t_start, " sec" 
  print "time used for one OZ run:", (t_stop - t_start)/float(densityRange.size), "seconds"
  
  #Plot results
  r = np.arange(s.getNumberOfRadialSamplingPoints() ).astype('float') + 1.0
  r *= s.getDelta_r()/s.getHardSphereRadius()

  for densityKey in densityRDFdictionary:
      pl.plot(r, densityRDFdictionary[densityKey], label = 'RDF for d=' + str(densityKey));
      pl.xlabel('r/sigma'); pl.ylabel('g(r)')
      pl.legend(loc = 'upper right')
      
  pl.show()
