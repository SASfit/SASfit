# -*- coding: utf-8 -*-
'''
Simple example how to
start proxy solver
and use the RPC API to
trigger calculation
and get result back.
For a more advanced example,
which shows how one can send
work to a cluster using RPC,
see oZclient.py
'''

#List to numpy array conversion
import numpy as np

#Remote procedure calls
import xmlrpclib

#Solver
from andersonOZsolver import AndersonOZsolver

#Solver runs in thread
import threading

#Sleep
import time

#plot
import pylab as pl


if __name__ == '__main__':
  portNumber = 4444
  print "Starting Server on port", str(portNumber)
  t = threading.Thread(target=AndersonOZsolver, args=(portNumber,))
  t.daemon = True; t.start(); time.sleep(2)
  print "Connecting to Server.."
  solverProxy = xmlrpclib.ServerProxy("http://localhost:" + str(portNumber) + "/")
  #Now the solver proxy can be used alike the solver (same/similar API)
  solverProxy.setPotentialByName('HardSphere')
  solverProxy.setVolumeDensity(0.5)
  solverProxy.solve()
  #Even though the Solver class runs in a separate thread, solve() blocks..
  #(Remember you are talking to the proxy, not calling solve() directly on AndersonOZsolver)
  g = np.array(solverProxy.getRDFasList() ) #..such that g is ready.
  
  #Plot; first get meta-data to calculate r range
  numberOfRadialSamplingPoints = solverProxy.getNumberOfRadialSamplingPoints()
  hardSphereRadius = solverProxy.getHardSphereRadius()
  Delta_r = solverProxy.getDelta_r()
  r = Delta_r*(np.arange(numberOfRadialSamplingPoints).astype('float') + 1.0)/hardSphereRadius
  #Then plot g versus r
  pl.plot(r, g, label = 'RDF');
  pl.xlabel('r/sigma'); pl.ylabel('g(r)')
  pl.legend(loc = 'upper right')
  pl.show()