# -*- coding: utf-8 -*-
#parser
from optparse import OptionParser
import sys

from picardOZsolver import PicardOZsolver
from andersonOZsolver import AndersonOZsolver
from scipyAndersonOZsolver import ScipyAndersonOZsolver
from scipyNewtonKrylovOZsolver import ScipyNewtonKrylovOZsolver

#Definition of the parser
#**********************************************************************************************************
def readOptions():
    parser = OptionParser(usage="Usage: python " + sys.argv[0] + " <Options>")
    parser.add_option("-p", "--portNumber", dest="portNumber", default=0, help="port number to listen[default: %default]") 
    parser.add_option("-t", "--typeOfIteration", dest="typeOfIteration", default='Picard', help="define iteration schema [default: %default]")
   
    options, args = parser.parse_args()
    return options, args


#Start here
#*******************************************************************************************************************************************************************
if __name__ == '__main__':
  options, args = readOptions()
  portNumber = int(options.portNumber)
  typeOfIteration = options.typeOfIteration
  
  if typeOfIteration == 'Picard':
    PicardOZsolver(portNumber)
    
  elif typeOfIteration == 'Anderson':
    AndersonOZsolver(portNumber)
    
  elif typeOfIteration == 'ScipyAnderson':
    ScipyAndersonOZsolver(portNumber)
    
  elif typeOfIteration == 'ScipyNewtonKrylov':
    ScipyNewtonKrylovOZsolver(portNumber)
    
  else:
    print "Unknown iteration schema, exiting"
    sys.exit()
    
  
  
