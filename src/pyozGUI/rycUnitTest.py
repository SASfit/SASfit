# -*- coding: utf-8 -*-
'''
This code is to check the limits of
the Rogers Young closure, which interpolates
between HNC and PY.
For alpha -> 0, PYC is recovered, 
for alpha -> inf, HNC closure is recovered
'''

import numpy as np
from scipyAndersonOZsolver import ScipyAndersonOZsolver

#plot
import matplotlib.pyplot as plt
import pylab as pl

if __name__ == '__main__':
  
  closureRDFdictionary = {}

  solver = ScipyAndersonOZsolver(port = 0)
  solver.setPotentialByName('HardSphere')
  solver.setVolumeDensity(0.4)
  
  #Default is PY
  print "PY closure.."
  solver.solve()
  closureRDFdictionary['PY'] = solver.getRDF()
  
  print "HNC closure.."
  solver.doHNCclosure()
  solver.solve()
  closureRDFdictionary['HNC'] = solver.getRDF()
  
  print "RY closure, alpha -> 0.."
  solver.doRYclosure(alpha=0.0001) #Not exactly zero...
  solver.solve()
  closureRDFdictionary['RY alpha -> 0'] = solver.getRDF()
  
  print "RY closure, alpha -> inf.."
  solver.doRYclosure(alpha=100000.0)
  solver.solve()
  closureRDFdictionary['RY alpha -> inf'] = solver.getRDF()
 
  print "RY closure, alpha = 3.0"
  #last but not least a alpha in between
  solver.doRYclosure(alpha=3.0)
  solver.solve()
  closureRDFdictionary['RY alpha=3.0'] = solver.getRDF()
 
 
  #Plot results
  r = np.arange(solver.getNumberOfRadialSamplingPoints() ).astype('float') + 1.0
  r *= solver.getDelta_r()/solver.getHardSphereRadius()

  for algorithmKey in closureRDFdictionary:
      pl.plot(r, closureRDFdictionary[algorithmKey], label = 'RDF according ' + str(algorithmKey));
      pl.xlabel('r/sigma'); pl.ylabel('g(r)')
      
  pl.legend(loc = 'upper right')

      
  pl.show()

