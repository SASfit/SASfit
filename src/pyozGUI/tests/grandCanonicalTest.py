# -*- coding: utf-8 -*-
'''
This script enforces a boundary
condition provided by the analytic,
Grand Canonical Ensemble solution 
of the HS + PY OZ system,
for which it holds that:
S_c(q→0) =(1 − phi)^4/(1 + 2*phi)^2  (1)
where S_c := 1 + rho*FFT(h)
and phi is the volume density of the fluid.
Since in the canonical ensemble it holds
S_c(0) = 1 + rho*int h d^3r = 0,    (2)
we call the equation (1) the
*grand* canonical ensemble version.
(As a side mark, in the GCE, there
are particle fluctuations, since 
the system is also open regarding volume,
and hence particle conservation
(as expressed in (2) ) does not hold
strictly (only on average).
So far for the naming)
The script calculates the OZ solution
of the HS + PY system with and without
the S_c(0) result.
'''
import numpy as np
from scipyAndersonOZsolver import ScipyAndersonOZsolver


#plot
import matplotlib.pyplot as plt

if __name__ == '__main__':
  solver = ScipyAndersonOZsolver(port = 0)
  solver.setPotentialByName('HardSphere')
  volumeDensity = 0.5
  solver.setVolumeDensity(volumeDensity)
  solver.setConvergenceCriterion(1.0e-4*solver.getConvergenceCriterion())
  #PY is default, so no explicit settings are needed in this regard
  #so solver is in desired state.
  solver.solve()
  #Get reference structure factor
  Sq_default = solver.getSq()
  #now GCE limit version
  solver.doEnforceGrandCanonicalZeroQlimit()
  #solver is in desired state.
  solver.solve()
  #Get GCE structure factor
  Sq_GCE = solver.getSq()
  print "reference Sq(0)", Sq_default[0]
  print "GCE Sq(0)", Sq_GCE[0]
  print "Analytical Sq(0)", (1.0 - volumeDensity)**4/(1.0 + 2.0*volumeDensity)**2
  
  #Even though the enforcement only affects one point in Fourier space,
  #the impact will be global, hence we plot the functions. (But real analyis must be done numerically)
  
  #Plot results
  Delta_r = solver.getDelta_r()
  numberOfRadialSamplingPoints = solver.getNumberOfRadialSamplingPoints()
  sigma = solver.getHardSphereRadius()
  Delta_q = np.pi/((numberOfRadialSamplingPoints + 1.0)*Delta_r)
  q = Delta_q*((np.arange(numberOfRadialSamplingPoints) + 1.0).astype('float'))
  
  plt.plot(np.arcsinh(q*sigma), Sq_default, label = 'Sq_default(q)');
  plt.xlabel('arcsinh(q*sigma)'); plt.ylabel('Sq_default(q)')

  plt.plot(np.arcsinh(q*sigma), Sq_GCE, label = 'Sq_GCE(q)');
  plt.xlabel('arcsinh(q*sigma)'); plt.ylabel('Sq_GCE(q)')

  plt.legend(loc = 'upper right')
      
  plt.show()

