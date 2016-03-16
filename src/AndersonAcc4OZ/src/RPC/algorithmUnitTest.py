# -*- coding: utf-8 -*-
'''
This script compares the
numerically obtained RDF 
with its analytical counterpart,
the differences are both plotted
and written to console.
(As l2 difference and relative 
deviation per pixel)
'''
import numpy as np

#plot
import matplotlib.pyplot as plt
import pylab as pl


#Solver
from picardOZsolver import PicardOZsolver
from andersonOZsolver import AndersonOZsolver
from scipyAndersonOZsolver import ScipyAndersonOZsolver
from scipyNewtonKrylovOZsolver import ScipyNewtonKrylovOZsolver

if __name__ == '__main__':  

  #Dict to contain the various algorithms (classes)
  solverClassDictionary = {}
  #Dict to contain the various results (algorithm specific)
  algorithmRDFdictionary = {}
  
  #Instantiate solver classes (port=0 means no server)
  solverClassDictionary['Picard'] = PicardOZsolver(port = 0)
  solverClassDictionary['Anderson'] = AndersonOZsolver(port = 0)
  solverClassDictionary['scipyAnderson'] = ScipyAndersonOZsolver(port = 0)
  solverClassDictionary['scipyNewtonKrylov'] = ScipyNewtonKrylovOZsolver(port = 0)
  
  #Basic params, use HardSphere together with analytical Solution
  density = 0.3
  potential = 'HardSphere'
  
  #Start loop over classes (calculate RDF with all algorithms)
  for algorithmKey in solverClassDictionary:
      #Activate the next line to use a different FPO, here the investment in design pays off,
      #since the fixpoint operator can easily be exchanged without further modifications
      #solverClassDictionary[algorithmKey].doNotUseGammaFixPointOperator()
      solverClassDictionary[algorithmKey].setPotentialByName(potential)
      solverClassDictionary[algorithmKey].setVolumeDensity(density)
      solverClassDictionary[algorithmKey].solve()
      g = solverClassDictionary[algorithmKey].getRDF()
      algorithmRDFdictionary[algorithmKey] = g

  #We can take any solver (implemented in abstract class) to get analytical solution
  solverClassDictionary['Picard'].calculateAnalyticalRDFsolutionForHS()
  g_analytical = solverClassDictionary['Picard'].getRDF()
  
  #calculate l2 difference (compared to analytical solution)
  for algorithmKey in algorithmRDFdictionary:
      l2Difference = ((algorithmRDFdictionary[algorithmKey] - g_analytical)**2).sum()
      #Also print diff per pixel. Since g = O(1), this is also (in good approx) the relative difference per pixel
      print "L2 difference Analytical -", algorithmKey.ljust(20), ":", l2Difference, \
      "\t (relative) per grid point:", np.round(l2Difference/float(solverClassDictionary['Picard'].getNumberOfRadialSamplingPoints() ), 10)
  
  #calculate maximum norm (infinity norm) of difference (compared to analytical solution)
  for algorithmKey in algorithmRDFdictionary:
      lMaxDifference = np.abs(algorithmRDFdictionary[algorithmKey] - g_analytical).max()
      worstGridPoint = np.argmax(np.abs(algorithmRDFdictionary[algorithmKey] - g_analytical))
      relativelMaxDifference = lMaxDifference/g_analytical[worstGridPoint]
      worstRadialPosition = float(worstGridPoint)/solverClassDictionary['Picard'].getHardSphereRadiusInPoints()
      print "relative maximal deviation Analytical -", algorithmKey.ljust(20), ":", 100*relativelMaxDifference , '% \t @ position r/sigma =', worstRadialPosition
      
  #Plot results; basic data (which is equal for all classes) is taken from Picard
  r = solverClassDictionary['Picard'].getrArray()
  r /= solverClassDictionary['Picard'].getHardSphereRadius()

  for algorithmKey in algorithmRDFdictionary:
      pl.plot(r, algorithmRDFdictionary[algorithmKey], label = 'RDF according ' + str(algorithmKey));
      pl.xlabel('r/sigma'); pl.ylabel('g(r)')
      pl.legend(loc = 'upper right')
      
  #analytical solution
  pl.plot(r, g_analytical, label = 'RDF according analytical Solution');
  pl.xlabel('r/sigma'); pl.ylabel('g(r)')
  pl.legend(loc = 'upper right')
  
      
  pl.show()
