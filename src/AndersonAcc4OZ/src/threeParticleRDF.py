# -*- coding: utf-8 -*-
'''
This code calculates the
radial distribution function
for a 3 hard-sphere particle system
The solution is analytical
and exact up to global scaling.

Author: A. Studer, Scientific Computing, PSI
'''
import numpy as np

import matplotlib.pyplot as plt
import pylab as pl

#global constants
numberOfRadialSamplingPoints = 2**8



#ideal Gas
#**********
boltzmannOfP2Ppotential = np.ones(numberOfRadialSamplingPoints)

#Hard Spehere
#***********
hardSphereRadius = numberOfRadialSamplingPoints/2**4
#beta = 1.0 #not needed for hard spheres
boltzmannOfP2Ppotential[:hardSphereRadius] = 0.0

r = np.arange(numberOfRadialSamplingPoints).astype('float')

#Plot exp(-beta*U)
#pl.plot(boltzmannOfP2Ppotential, label = 'Boltzmann of Potential'); 
#plt.ylim((0,1.2));pl.legend(loc = 'lower right'); pl.show()

sphereVolume = (4*np.pi/3.0)*hardSphereRadius**3
boxVolume = 2*3*sphereVolume
g = np.zeros(numberOfRadialSamplingPoints)
sphereIntersectionVolume = (np.pi/12.0)*(4*hardSphereRadius + r)*(2*hardSphereRadius -r)**2
g[0:2*hardSphereRadius] = boxVolume - (2*sphereVolume - sphereIntersectionVolume[0:2*hardSphereRadius])
g[2*hardSphereRadius:] = boxVolume - 2*sphereVolume
g *= boltzmannOfP2Ppotential
g /= g[-1]
pl.plot(g, label = 'Radial Distribution function for 3 particles'); 
pl.legend(loc = 'lower right'); pl.show()

