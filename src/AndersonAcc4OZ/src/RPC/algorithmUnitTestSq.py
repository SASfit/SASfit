# -*- coding: utf-8 -*-
'''
This is a unit test which compares
the numerical result for the structure 
factor (obtained by solving the OZ equations
iteratively) with the formula obtained
analytically. The analytic result is taken from
the AMAS lectures notes of G. Naegele 
(The physics of colloidal soft matter)
and is valid for hard spheres and PY closure.
The precision is of the same order of 
magnitude as in the result obtained by the 
analogous unit test for the 
radial distribution function.
(See algorithmUnitTest.py)
Note however that this unit test only
checks *numerics* (how close to iterative
solution is to the analytic, 
mathematically 'true' solution)
It does not say anything about the
*physical* quality of the (analytical)
solution. (Which is mediocre, there are
e.g consistency requirements which
are not fullfilled.)
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
  algorithmSqDictionary = {}
  
  #Instantiate solver classes (port=0 means no server)
  solverClassDictionary['Picard'] = PicardOZsolver(port = 0)
  solverClassDictionary['Anderson'] = AndersonOZsolver(port = 0)
  solverClassDictionary['scipyAnderson'] = ScipyAndersonOZsolver(port = 0)
  solverClassDictionary['scipyNewtonKrylov'] = ScipyNewtonKrylovOZsolver(port = 0)
  
  #Basic params, use HardSphere together with analytical Solution (and PY, but this is default)
  density = 0.3
  potential = 'HardSphere'
  
  #Start loop over classes (calculate S(q) with all algorithms)
  for algorithmKey in solverClassDictionary:
      #Activate the next line to use a different FPO, here the investment in design pays off,
      #since the fixpoint operator can easily be exchanged without further modifications
      #solverClassDictionary[algorithmKey].doNotUseGammaFixPointOperator()
      solverClassDictionary[algorithmKey].setPotentialByName(potential)
      solverClassDictionary[algorithmKey].setVolumeDensity(density)
      solverClassDictionary[algorithmKey].solve()
      algorithmSqDictionary[algorithmKey] = solverClassDictionary[algorithmKey].getSq()

  #We can take any solver to get analytical solution (implemented in abstract class 'OZsolver') 
  sq_analytical = solverClassDictionary['Picard'].calculateAnalyticalStructureFactorArrayForHS()
  
  #calculate l2 difference (compare to analytical solution)
  for algorithmKey in algorithmSqDictionary:
      l2Difference = ((algorithmSqDictionary[algorithmKey] - sq_analytical)**2).sum()
      #Also print diff per pixel. Since Sq ~ 1, this is also (in good approx) the relative difference per pixel
      print "L2 difference Analytical -", algorithmKey.ljust(20), ":", l2Difference, \
      "\t (relative, averaged) per grid point:", np.round(l2Difference/float(solverClassDictionary['Picard'].getNumberOfRadialSamplingPoints() ), 10)

  delta_r = solverClassDictionary['Picard'].getDelta_r() 
  delta_q = np.pi/( (solverClassDictionary['Picard'].getNumberOfRadialSamplingPoints() +1.0) * delta_r )
  sigma = solverClassDictionary['Picard'].getHardSphereRadius()
  
  #calculate maximum norm (infinity norm) of difference (compared to analytical solution)
  for algorithmKey in algorithmSqDictionary:
      # S(q) > 0
      relativeMaxDifferenceArray = np.abs(algorithmSqDictionary[algorithmKey] - sq_analytical)/sq_analytical
      maxDifferenceArray = np.abs(algorithmSqDictionary[algorithmKey] - sq_analytical)
      worstGridPointRelative = np.argmax(relativeMaxDifferenceArray)
      worstGridPointAbsolute = np.argmax(maxDifferenceArray)
      lMaxDifferenceRelative = relativeMaxDifferenceArray[worstGridPointRelative]
      lMaxDifferenceAbsolute = maxDifferenceArray[worstGridPointAbsolute]
      worstArgumentRelative = np.arcsinh(delta_q*(worstGridPointRelative + 1.0)*sigma)
      worstArgumentAbsolute = np.arcsinh(delta_q*(worstGridPointAbsolute + 1.0)*sigma)
      print "absolute maximal deviation Analytical -", algorithmKey.ljust(20), ":", round(100*lMaxDifferenceAbsolute, 2) , '% \t @ position =', round(worstArgumentAbsolute, 3)
      print "relative maximal deviation Analytical -", algorithmKey.ljust(20), ":", round(100*lMaxDifferenceRelative, 2) , '% \t @ position =', round(worstArgumentRelative, 3)
      
  print 'maximal value of analytical structure factor is @', round(np.arcsinh(delta_q*(np.argmax(sq_analytical) + 1.0)*sigma), 3)
  print 'start value x-axis =', round(np.arcsinh(delta_q*(1.0)*sigma), 3)
  
  #Plot results; basic data (which is equal for all classes) is taken from Picard
  q = (delta_q/delta_r)*solverClassDictionary['Picard'].getrArray()
  for algorithmKey in algorithmSqDictionary:
      pl.plot(np.arcsinh(q*sigma), algorithmSqDictionary[algorithmKey], label = 'S(q) according ' + algorithmKey);
      pl.xlabel('arcsinh(q*sigma)'); pl.ylabel('S(q)')
      pl.legend(loc = 'upper right')
      
  #analytical solution
  pl.plot(np.arcsinh(q*sigma), sq_analytical, label = 'S(q) according analytical Solution');
  pl.xlabel('arcsinh(q*sigma)'); pl.ylabel('S(q)')

  pl.legend(loc = 'upper right')
  
      
  pl.show()
