# -*- coding: utf-8 -*-
# *-----------------------------------------------------------------------*
# |                                                                       |
# | Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
# |                                                                       |
# | Author: A. Studer, Scientific Computing, PSI                          |
# | Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
# *-----------------------------------------------------------------------*

#List to numpy array conversion
import numpy as np

#Remote procedure calls
import xmlrpclib

#Start serving processes
import subprocess

#plot
import matplotlib.pyplot as plt
import pylab as pl

#parser
from optparse import OptionParser
import sys

#We don't want to wait for I/O
import threading
import Queue

#performance measurement
import time


#thread function
def calculateRDF(potentialName, densityRange, serverHostname, portNumberAsString):
      print "Connecting to Server", serverHostname, "on port", portNumberAsString
      solverProxy = xmlrpclib.ServerProxy("http://" + serverHostname + ":" + portNumberAsString + "/")
      solverProxyDictionary[portNumberAsString] = solverProxy
      solverProxy.setPotentialByName(potentialName)
      for d in densityRange:
          solverProxy.setVolumeDensity(float(d)) #d is a numpy array (with 1 entry)
          solverProxy.solve()
          g = np.array(solverProxy.getRDFasList() ) #A list of numbers is received
          densityRDFdictionary[d] = g
      threadQueue.task_done() #Kind of semaphore, here we decrease value by one

#Definition of the parser
#**********************************************************************************************************
def readOptions():
    parser = OptionParser(usage="Usage: python " + sys.argv[0] + " <Options>")
    parser.add_option("-b", "--basePortNumber", dest="basePortNumber", default=20000, help="define base port number [default: %default]") 
    parser.add_option("-s", "--serverHostname", dest="serverHostname", default='localhost', help="define server host where OZsolver runs [default: %default]")
    parser.add_option("-n", "--numberOfServerProcesses", dest="numberOfServerProcesses", default=1, help="number of OZsolver to run. (E.g number of cores on server machine [default: %default]")
    parser.add_option("-t", "--typeOfIteration", dest="typeOfIteration", default='Picard', help="define iteration schema [default: %default]")
    parser.add_option("-p", "--potentialName", dest="potentialName", default='HardSphere', help="define pair interaction potential [default: %default]")

    options, args = parser.parse_args()
    return options, args

      
      
#Start here
#*******************************************************************************************************************************************************************
if __name__ == '__main__':
  options, args = readOptions()
  basePortNumber = int(options.basePortNumber)
  serverHostname = options.serverHostname
  numberOfServerProcesses = int(options.numberOfServerProcesses)
  typeOfIteration = options.typeOfIteration
  potentialName = options.potentialName

  #global vars (filled by threads or book-keeping for threads)
  densityRDFdictionary = {}
  threadQueue = Queue.Queue() # See https://docs.python.org/2/library/queue.html
  solverProxyDictionary = {}
  
  #First start all server processes
  for p in range(numberOfServerProcesses):
      portNumberAsString = str(basePortNumber + p)
      print "Starting Server", serverHostname, "on port", portNumberAsString
      #Activate next line if server processes are not to run on local machine. (where this script is launched)
      #serverProcess = subprocess.Popen(["ssh", "studer_a1@" + serverHostname, "python runOZserver.py -p " + portNumberAsString + " -t " + typeOfIteration]);
      #The only None-TCP/IP thing we communicate over commandline is the type of the solver, since this is needed to instantiate the class
      #and cannot be (re-) set later. The other attributes of the class are communicated via RPC (after the specific solver class was instantiated)
      serverProcess = subprocess.Popen("python runOZserver.py -p " + portNumberAsString + " -t " + typeOfIteration, shell=True)
  time.sleep(5) # delays for 5 seconds
  
  #We scan a range of density values. (Alternatively, this could be a range of energy values for the LJ potential)
  densityRangeDictionary = {}
  numberOfDensityParametersPerProcess = 20
  maxDensity = 0.5
  for p in range(numberOfServerProcesses):
      #The densities must be distributed randomly to the server processes to achieve equal load since the higher
      #the density the longer it takes for the algorithm to converge. An alternative would be to implement the thread pool pattern.
      #(If the densities would be defined in a systematic/non-random way and the corresponding tasks would subsequently asssigned to the workers)
      rangeForProcess = maxDensity*np.random.rand(numberOfDensityParametersPerProcess)
      densityRangeDictionary[p] = rangeForProcess
  #Then assign them work via threads. (We remain on single core/process, but do not want to wait for results to arrive)
  #Discussion e.g here: http://stackoverflow.com/questions/2846653/python-multithreading-for-dummies
  #(Here we just receive data, this is not CPU intensive but I/O blocking. Hence we can use threads here, contrary
  #to the server processes, these are CPU intensive, hence we start separate processes for these (or we would need the multiprocessing module there) )
  t_start = time.time()
  for p in range(numberOfServerProcesses):
      portNumberAsString = str(basePortNumber + p)
      t = threading.Thread(target=calculateRDF, args = (potentialName, densityRangeDictionary[p], serverHostname, portNumberAsString))
      t.daemon = True
      t.start()
      threadQueue.put(t) #Kind of semaphore, here we increase value by one, see thread function for decrease..
  
  threadQueue.join()  # block until all tasks are done, i.e all threads are finished
  t_stop = time.time()
  numberOfDensityParameters = numberOfDensityParametersPerProcess*numberOfServerProcesses #Equal load for all servers
  print "time used for", numberOfDensityParameters, "density parameters:", t_stop - t_start, " sec" 
  print "time used for one OZ run:", (t_stop - t_start)/float(numberOfDensityParameters), "seconds"
  
  #Plot
  metaDatasolverProxy = solverProxyDictionary[str(basePortNumber)]
  numberOfRadialSamplingPoints = metaDatasolverProxy.getNumberOfRadialSamplingPoints() #In SASfit gridsize n=8 (2^7*2^3 = 2^10)
  hardSphereRadius = metaDatasolverProxy.getHardSphereRadius()
  Delta_r = metaDatasolverProxy.getDelta_r()
  r = Delta_r*(np.arange(numberOfRadialSamplingPoints).astype('float') + 1.0)/hardSphereRadius
  
  for densityKey in densityRDFdictionary:
      pl.plot(r, densityRDFdictionary[densityKey], label = 'RDF for d='+str(densityKey));
      pl.xlabel('r/sigma'); pl.ylabel('g(r)')
      pl.legend(loc = 'upper right')
  pl.show()
  #serverProcess.kill()
      
