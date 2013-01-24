This are the sources of the 'dqkit' package from dqsoftware
http://sourceforge.net/projects/dqsoftware/

It is a heavily modified and extended fork of the 'tclkit' program to create
and run 'starkit' packages (standalone tcl-environment). 
The original 'tclkit' can be found at: http://www.equi4.com/tclkit/



This 'dqkit' was modified to consist of the minimal required tcl runtime
environment to run 'SASfit' on several platforms as standalone executable
without the need to install further software on the target system.

To build it on the current platform, run 'gen/common.sh'
To remove any binaries built, run 'gen/buildkit cleanupall'

For more available commands, see the functions part of the ::main:: namespace
within the 'gen/buildkit' file.



The modifications applied are of minor nature, just to build it smoothly with
the desired configuration. Fixed some linker issues.

2008-11-26, Ingo Bressler, ingo@wollsau.de

