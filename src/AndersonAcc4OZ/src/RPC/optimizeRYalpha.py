# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*
'''
This script tries to find the optimal
Rogers Young or HMSA closure parameter alpha,
where alpha is a positive number defining
how close we are either to the HNC closure
or the PY closure. (See also rycUnitTest.py)
Since with alpha a new 'degree of freedom'
is introduced into the system of OZ equations,
we need yet another equation fixing alpha.
This equation is given by a thermodynamic
consistency, i.e. it is possible to 
calculate the same thermodynamic quantity
(the isothermal compressibility)
via two different routes. 
(See J.K. SASfit docu or A.S. docu
or Naegele AMAS LN).
That calculating kappa in two different ways
must yield the same result defines an
implicit (functional) equation for alpha.
This implicit equation is solved numerically
in the following piece of code.
Design: Here it pays off to have a solver class,
the loss function (or root finding function)
can be written in a compact and clear manner,
all computational details are hidden,
i.e. encapsulated within class; note that all
data, methods and functions needed to calculate
the zero problem function comes from the solver.
(searchTripGraph and doUseMinimizer is 'meta-data' )
'''
import numpy as np
from scipy import optimize
from scipyAndersonOZsolver import ScipyAndersonOZsolver
from scipyNewtonKrylovOZsolver import ScipyNewtonKrylovOZsolver


#plot
#import matplotlib.pyplot as plt
import pylab as pl
import collections

def calculate_I_g_terms(integrand, solver):
  #to calculate derivative of integral with repsect to rho
  deltaVolumeDensity = 0.01 #numerically subtle for LJ
  #First calculate g(rho + delta_rho)
  #store (fix) volume density
  originalVolumeDensity = solver.getVolumeDensity()
  rho = solver.getParticleNumberDensity()
  solver.setVolumeDensity(originalVolumeDensity + deltaVolumeDensity)
  delta_rho = solver.getParticleNumberDensity() - rho
  solver.solve()
  g = solver.getRDF()
  integrand_g_rho_plus_delta_rho = integrand*g
  #we have what we need to calculate I(rho + delta_rho) rho, 
  #so reset original volume density to calculate I(rho)
  solver.setVolumeDensity(originalVolumeDensity)
  #Calculate g(rho)
  solver.solve()
  g = solver.getRDF()  
  integrand_g_rho = integrand*g
  
  #calculate I_g integrals according integrands
  delta_r = solver.getDelta_r()
  #First calculate I(rho)
  I_rho = integrand_g_rho.sum()*delta_r
  #Then calculate I(rho + delta_rho)
  I_rho_plus_delta_rho = integrand_g_rho_plus_delta_rho.sum()*delta_r
  #Finally dI/ d rho
  derivativeOf_I_rho = (I_rho_plus_delta_rho - I_rho)/delta_rho
  return I_rho, derivativeOf_I_rho
    

#This is a scalar function: alpha in [0, inf) -> R, either loss- or root finding function
def finderFunctionForAlpha(alpha, solver, searchTripGraph, doUseMinimizer):
  #Get the necessary constant data (to calculate integrals' difference) from solver
  r = solver.getrArray()
  delta_r = solver.getDelta_r()
  sigma = solver.getHardSphereRadius()
  sigmaInPoints = solver.getHardSphereRadiusInPoints()
  #Start update step
  #print 'Potential:', solver.getpotentialByName()
  #Integrand obtained via g-route
  #LJ (analytically)
  if solver.getpotentialByName() == 'LennardJones':
      #Inform the solver about the new alpha
      solver.doHMSAclosure(alpha)
      #evaluate u'
      #f = 6.0*(sigma/r)**6 - 12.0*(sigma/r)**12  should only be calculated once since it won't change. but its cleaner to be here.
      #Calculate analytically obtained powers in sigma/r ratio, this is better for numerics. One r of r^3 is absorbed in u' for numerical stability
      integrand_g = 4.0*(6.0*(sigma/r)**6 - 12.0*(sigma/r)**12)*(r**2) #units: distance squared
      energyInKTunits = solver.getEpsilonInkTUnits()
      
  #Yukawa (analytically)
  if solver.getpotentialByName() == 'Yukawa':
      #Inform the solver about the new alpha
      solver.doRYclosure(alpha)
      #evaluate u'
      lamda = solver.getShieldingConstantInSigmaUnits()
      e = np.exp(-r/lamda)
      integrand_g = -sigma*e/(r*lamda) - e/(r**2) #Units: 1/distance
      if solver.getIsHardSphereAdded():
          #Shift u' in total, this is smooth, contrary to bellow
          integrand_g = r**3*np.roll(integrand_g, sigmaInPoints) #overwritten bellow
          #Only shift exp part in u', not 1/r part
          e = np.roll(e, sigmaInPoints) # = np.exp(-(r - sigma)/lamda )
          #Activate next line for d/dr exp( (r - sigma)/lamda) / (r/sigma), corresponding to line 313 in fixPointOp
          integrand_g = - sigma*e*( (r**2)/lamda + r ) #units: distance squared. (integration -> Volume)
          integrand_g[:sigmaInPoints] = -0.0 #u' is zero since u = const = infty for r < sigma
          
      energyInKTunits = solver.getInteractionStrengthInKTunits()
      
  #plot
  #pl.plot(r, integrand_g, label = 'integrand_g ~du/dr * r^3 ' ); pl.legend(loc = 'upper right'); pl.show()
  I_rho, derivativeOf_I_rho = calculate_I_g_terms(integrand_g, solver) #Units: (Volume, Volume_Squared)
  rho = solver.getParticleNumberDensity()
  I_g = (4.0*np.pi/3.0)*energyInKTunits*I_rho + (2.0*np.pi/3.0)*energyInKTunits*rho*derivativeOf_I_rho #Units: Volume
  #solver is in original state, hence c = c (rho) (not c (rho + delta_rho)
  c = solver.getDirectCorrelationFunction()
  #Integrand obtained via c-route
  integrand_c = c*(r**2) #units: distance squared (integration -> Volume)
  #Integral obtained via c-route
  I_c = 4.0*np.pi*integrand_c.sum()*delta_r  #I_2 = I_2(alpha), units: Volume
  #numpy provides trapezoidal rule integration (maybe better..)
  #I_c = 4.0*np.pi*np.trapz(integrand_c, dx = delta_r)
  
  #Difference obtained (using different routes)
  delta_I = (I_c - I_g)/rho       #Units: None
      
  if doUseMinimizer: #return 'convexitized' (squared) difference, minimize.
    #we record the search trip to plot it later..
    searchTripGraph[alpha[0]] = delta_I**2 #in minimizer, scalar alpha is treated as 1D array, not float
    return delta_I**2
  else: #return plain difference, find root.
    #we record the search trip to plot it later..
    searchTripGraph[alpha] = delta_I
    return delta_I
    
#Only needed to plot the loss function (with the found minumum)
def plotDifferentRouteFuction(graphAsDict, alpha_min, f_alpha_min):
  #we must sort the dict since the search trip was a hopping one..
  finderFunctionGraph = collections.OrderedDict(sorted(graphAsDict.items()))
  pl.plot(finderFunctionGraph.keys(), finderFunctionGraph.values(), label = 'Route 1 - Route 2' );
  pl.plot(alpha_min, f_alpha_min, 'o', color='red')
  pl.show()
    

if __name__ == '__main__':
  
  solver = ScipyAndersonOZsolver(port = 0)
  #solver = ScipyNewtonKrylovOZsolver(port = 0)
  #We only work with LJ or Yukawa potential since we want to avoid the trouble
  #related to the 'jump' (discontinuity) at r = sigma
  
  doLennardJonesExample = False
  
  if doLennardJonesExample:
      #LJ possible but with HMSA
      solver.setPotentialByName('LennardJones', 0.1) #epsilon/kT = 0.1 is default in SASfit
      print "Energy scale of Lennard Jones Potential in kT units:", solver.getEpsilonInkTUnits()
      #Initialize HMSA with some (random) start alpha
      solver.doHMSAclosure(1.0)
  
  else:
      #Yukawa possible with RY: With negative K (attractive besides HS) convergence may be reached for exp shift only (real HS-Yukawa)
      #positive (repulsive as HS) K converges for fully shifted potential (1/r included)
      solver.setPotentialByName('Yukawa', 1.0, -1.0, True) #lamda in SASfit is different; K = 0.1 is default in SASfit
      print "Energy scale of Yukawa HS Potential in kT units:", solver.getInteractionStrengthInKTunits()
      print "Length scale of Yukawa HS Potential in sigma units:", solver.getShieldingConstantInSigmaUnits()
      #Initialize RY with some (random) start alpha
      solver.doRYclosure(1.0)

  
  solver.setVolumeDensity(0.3)
  print "Volume density of liquid:", solver.getVolumeDensity()
  
  print "start solving for optimal closure alpha...."
  
  #Should we minimize cost functional or find a root?
  doUseMinimizer = False
  doSearchIndependentPlot = False
  #Save iterated points in alpha space as dict to plot later
  finderFunctionGraph = {}
  
  if doUseMinimizer:
      #Ein Minimum macht noch keine Nullstelle, minimierungs algorithmen
      #kommen aber (auch) mit doppelten (mehrfachen) Nullstellen klar. *Falls* eine Nullstelle vorliegt,
      #ist sie in jedem Fall auch ein Minimum der quadrierten funktion (bei mehreren Nullstellen kein globales Minimum)
      alpha_opt = optimize.minimize(finderFunctionForAlpha, alpha_initial, args=(solver, finderFunctionGraph, doUseMinimizer,))
      print "optimal alpha according minimizer:", alpha_opt.x[0]
      print "function value @ optimal alpha :", alpha_opt.fun
      plotDifferentRouteFuction(finderFunctionGraph, alpha_opt.x[0], alpha_opt.fun)
  else:
      #Falls die Nullstelle eine doppelte ist, sind die meisten (bisektionsartigen) algorithmen ohne Loesung.
      #Im Beispiel (LJ + RY) gibt es gar keine Nullstelle, also ist 'minimieren' (oben) eine Kompromiss-Strategie.
      #newton gives (temporarily) negative alpha and domain interval cannot be defined..
      #alpha_opt = optimize.newton(finderFunctionForAlpha, alpha_initial, args=(solver, finderFunctionGraph, doUseMinimizer,))
      #brentq raises 'f(a) and f(b) must have different signs'
      try: 
          alpha_opt = optimize.brentq(finderFunctionForAlpha, 0.1, 10.0, args=(solver, finderFunctionGraph, doUseMinimizer,))
          print "optimal alpha according root finder:", alpha_opt
      except:
          print "Root finding not successful, plotting delta function"
          doSearchIndependentPlot = True
 
  #plot independently of search algorithm in case no result was found
  if doSearchIndependentPlot:
      alphaRange = np.arange(0.2, 10.0, 0.1)
      for alpha in alphaRange:
          print alpha, finderFunctionForAlpha(alpha, solver, finderFunctionGraph, doUseMinimizer)
      plotDifferentRouteFuction(finderFunctionGraph, 0.0, 0.0)
      
      