## Synopsis

This python code solves the Ornstein Zernike equations numerically by finding a root or fixpoint
of a corresponding fixpoint operator. For a project description, see the PSI-internal Wiki page 
https://intranet.psi.ch/AIT/AAA4OZ

## Code Example

All you need to experiment with the code is an instance of the solver class, e.g:

solver = ScipyAndersonOZsolver(port = 0)
solver.setPotentialByName('HardSphere')
solver.setVolumeDensity(0.4)
solver.solve()
radialDistributionFunction = solver.getRDF()

## Motivation

The python code was developed as a rapid prototyping alternative to the SASfit C Code.

## Installation

If you use Anaconda, an environment containing numpy and scipy should be enough to run the code.

## API Reference

See setter/getter methods in RPC/oZfixpointOperator.py and RPC/oZsolver.py.
To solver class can be used locally or remotely via RPC API.

## Tests

In the RPC dir, there are various unit tests.

## Contributors

Alain Studer, Joachim Kohlbrecher; PSI


