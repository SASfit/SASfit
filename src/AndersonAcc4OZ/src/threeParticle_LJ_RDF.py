# -*- coding: utf-8 -*-
import numpy as np

import matplotlib.pyplot as plt
import pylab as pl

#global constants
numberOfRadialSamplingPoints = 2**8



#Ideal Gas
#**********
boltzmannOfP2Ppotential = np.ones(numberOfRadialSamplingPoints)

#Hard Sphere
#***********
hardSphereRadius = numberOfRadialSamplingPoints/2**3
#beta = 1.0 #not needed for hard spheres
boltzmannOfP2Ppotential[:hardSphereRadius] = 0.0


#Lennard Jones
#*************
sigma = 2*float(hardSphereRadius)
epsilon = 1.0e-1
r = np.arange(numberOfRadialSamplingPoints).astype('float')
r[0] = 1.0e-10
potential_LJ = (sigma/r)**12 - (sigma/r)**6
potential_LJ *= 4.0*epsilon
#pl.plot(potential_LJ[hardSphereRadius-2:]); pl.show()
beta = 1.0e+1
boltzmannOfP2Ppotential = np.exp(-beta*potential_LJ)

#Plot exp(-beta*U)
pl.plot(boltzmannOfP2Ppotential, label = 'Boltzmann of Potential'); 
#plt.ylim((0,1.2));pl.legend(loc = 'lower right'); #pl.show()

#Convolution by MC integration
boxVolumeSize = int( (1.0/np.sqrt(3.0))*numberOfRadialSamplingPoints)
g = np.zeros(numberOfRadialSamplingPoints)
#vec r = (x,0,0), hence x = |vec r|
for x in range (numberOfRadialSamplingPoints):
  numberOfMCsamplesDrawn = 0.0
  integralValue = 0.0
  for i in range(10000):
    x_prime = np.random.randint(-boxVolumeSize, 2*boxVolumeSize) #Convolution corresponds to right shift,..
    y_prime = np.random.randint(-boxVolumeSize, boxVolumeSize)   #...so we make space in this direction larger
    z_prime = np.random.randint(-boxVolumeSize, boxVolumeSize)

    r_prime = int(np.round(np.sqrt(x_prime**2 + y_prime**2 + z_prime**2)))
    r_minus_r_prime = int(np.round(np.sqrt((x_prime - x)**2 + y_prime**2 + z_prime**2)))
    if (r_prime >= numberOfRadialSamplingPoints and r_minus_r_prime >= numberOfRadialSamplingPoints):
      contribution = 1.0
    elif (r_minus_r_prime >= numberOfRadialSamplingPoints and r_prime < numberOfRadialSamplingPoints):
      contribution = boltzmannOfP2Ppotential[r_prime]
    elif (r_prime >= numberOfRadialSamplingPoints and r_minus_r_prime < numberOfRadialSamplingPoints):
      contribution = boltzmannOfP2Ppotential[r_minus_r_prime]
    else:
      contribution = boltzmannOfP2Ppotential[r_minus_r_prime]*boltzmannOfP2Ppotential[r_prime]
    integralValue += contribution
    #print x, contribution
    numberOfMCsamplesDrawn += 1.0
  #end for MC sampling point for this r, x rspectively
  #print numberOfMCsamplesDrawn
  g[x] = integralValue/float(numberOfMCsamplesDrawn)
#Finished convolution
  
g *= boltzmannOfP2Ppotential
#g /= g[-1]
pl.plot(g, label = 'Radial Distribution function for 3 particles'); 
pl.legend(loc = 'lower right'); pl.show()

