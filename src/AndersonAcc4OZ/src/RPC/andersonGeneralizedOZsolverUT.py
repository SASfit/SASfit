# -*- coding: utf-8 -*-
'''
This script compares the
original Anderson algorithm versus 
the generalized
'''
import numpy as np

#plot
import matplotlib.pyplot as plt
import pylab as pl


#Solvers
from andersonOZsolver import AndersonOZsolver
from andersonGeneralizedOZsolver import AndersonGeneralizedOZsolver

if __name__ == '__main__':  

  #Dict to contain the various algorithms (classes)
  solverClassDictionary = {}
  #Dict to contain the various results (algorithm specific)
  algorithmRDFdictionary = {}
  
  #Instantiate solver classes (port=0 means no server)
  solverClassDictionary['Anderson'] = AndersonOZsolver(port = 0)
  solverClassDictionary['AndersonGeneralized'] = AndersonGeneralizedOZsolver(port = 0)
  
  #Basic params
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

  
      
  #Plot results; basic data (which is equal for all classes) is taken from Anderson
  r = solverClassDictionary['Anderson'].getrArray()
  r /= solverClassDictionary['Anderson'].getHardSphereRadius()

  for algorithmKey in algorithmRDFdictionary:
      pl.plot(r, algorithmRDFdictionary[algorithmKey], label = 'RDF according ' + str(algorithmKey));
      pl.xlabel('r/sigma'); pl.ylabel('g(r)')
      pl.legend(loc = 'upper right')
        
      
  pl.show()
