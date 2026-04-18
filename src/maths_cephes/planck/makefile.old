CC = gcc
CFLAGS = -O2 -Wall

OBJS = planck.o polylog.o unity.o spence.o zetac.o \
sin.o exp.o log.o pow.o powi.o fac.o gamma.o \
polevl.o floor.o isnan.o mtherr.o const.o

rad: rad.o $(OBJS)
	$(CC) $(CFLAGS) -o rad rad.o $(OBJS)

clean:
	rm -f *.o
	rm rad
