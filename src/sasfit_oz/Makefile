SHELL = /bin/sh
#
# Makefile for oz-solver 
# Joachim Kohlbrecher
# 22.10.2013
#

CFLAGS = -I"/usr/local/include" 
LFLAGS = -static -Wall -lm -lc -L"./"
LIBS   = libfftw3.a  -lgsl -lgslcblas 
CC = gcc 

EXE = main
OUTPUT = sasfit_oz

OBJECTS = 	main.o sasfit_oz_solver.o	\
		sasfit_oz_potential_Depletion.o \
		sasfit_oz_potential_Hard_Sphere.o \
		sasfit_oz_potential_Ionic_Microgel.o \
		sasfit_oz_potential_Lennard_Jones.o \
		sasfit_oz_potential_Soft_Sphere.o \
		sasfit_oz_potential_Sticky_Hard_Sphere.o \
		sasfit_oz_potential_DLVO.o \
		sasfit_oz_potential_PSM.o \
		sasfit_oz_potential_GGCM-n.o
    
$(EXE): $(OBJECTS)
	$(CC) -g -static -fexpensive-optimizations -O3 -o $(OUTPUT) $(OBJECTS) $(LIBS) $(LFLAG)
    
.c.o:
	$(CC) $(CFLAGS) -g -static -fexpensive-optimizations -O3 -c $*.c
	
clean:
	rm -f *.o
	rm $(OUTPUT).exe

realclean:
	make clean
	rm -f *.so
