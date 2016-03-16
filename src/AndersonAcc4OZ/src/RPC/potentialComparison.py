# -*- coding: utf-8 -*-
import numpy as np

#plot
import matplotlib.pyplot as plt
import pylab as pl


#Solver
from andersonOZsolver import AndersonOZsolver
from scipyAndersonOZsolver import ScipyAndersonOZsolver

if __name__ == '__main__':  

  #Dict to contain the various exp(-potentials)
  boltzmannOfPotentialDictionary = {}
  #Dict to contain the various results (potential specific)
  RDFdictionary = {}
  
  #Instantiate solver class (port=0 means no server)
  solver =  AndersonOZsolver(port = 0)
  
  #List of potential we check (together with '_' separated arguments
  listOfPotentials = ['HardSphere', 'LennardJones_0.3', 'Yukawa_1.2_0.1_False', 'Yukawa_1.2_0.1_True', 'Yukawa_1.2_-1.0_True', 'Star_20']

  #zero density yields g(r) = exp(-beta U)
  solver.setVolumeDensity(0.0)
  
  #Start loop over classes (calculate RDF with all algorithms)
  for potentialType in listOfPotentials:
      #print potentialType.split('_')
      #* is the (slpat= operator which converts a list to a set of arguements
      solver.setPotentialByName( *(potentialType.split('_')) ) 
      solver.solve()
      g = solver.getRDF() #Here g equals exp(-beta*u)
      boltzmannOfPotentialDictionary[potentialType] = g

  #Plot results
  r = solver.getrArray()
  for potentialType in boltzmannOfPotentialDictionary:
      pl.plot(r, boltzmannOfPotentialDictionary[potentialType], label = 'exp(-beta*' + str(potentialType) + ')');
      pl.xlabel('r/sigma'); pl.ylabel('exp(-beta*u)')
      pl.legend(loc = 'upper right')
      
  pl.show()
  
  #We  just repeat everything from above with a density > 0 to get a RDF fifferent from the Boltzmann of mutual potential
  density = 0.3
  #We switch to ScipyAndersonOZsolver since it can better handle Star potential (?)
  solver = ScipyAndersonOZsolver(port = 0)
  solver.setVolumeDensity(density)
  for potentialType in listOfPotentials:
      solver.setPotentialByName( *(potentialType.split('_')) ) 
      solver.solve()
      g = solver.getRDF()
      RDFdictionary[potentialType] = g

  #Plot results
  r = solver.getrArray()
  for potentialType in RDFdictionary:
      pl.plot(r, RDFdictionary[potentialType], label = 'RDF with' + str(potentialType));
      pl.xlabel('r/sigma'); pl.ylabel('g')
      pl.legend(loc = 'upper right')
      
  pl.show()
