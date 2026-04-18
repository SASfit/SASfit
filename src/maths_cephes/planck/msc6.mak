# Microsoft make file for Planck program
CC = cl

planck.obj: planck.c
 $(CC) /c planck.c

polylog.obj: polylog.c
 $(CC) /c polylog.c

unity.obj: unity.c
 $(CC) /c unity.c

spence.obj: spence.c
 $(CC) /c spence.c

zetac.obj: zetac.c
 $(CC) /c zetac.c

sin.obj: sin.c
 $(CC) /c sin.c

exp.obj: exp.c
 $(CC) /c exp.c

log.obj: log.c
 $(CC) /c log.c

pow.obj: pow.c
 $(CC) /c pow.c

powi.obj: powi.c
 $(CC) /c powi.c

fac.obj: fac.c
 $(CC) /c fac.c

gamma.obj: gamma.c
 $(CC) /c gamma.c

polevl.obj: polevl.c
 $(CC) /c polevl.c

floor.obj: floor.c
 $(CC) /c floor.c

isnan.obj: isnan.c
 $(CC) /c isnan.c

mtherr.obj: mtherr.c
 $(CC) /c mtherr.c

const.obj: const.c
 $(CC) /c const.c

rad.obj: rad.c
 $(CC) /c rad.c

rad.exe: planck.obj polylog.obj unity.obj spence.obj zetac.obj \
sin.obj exp.obj log.obj pow.obj powi.obj fac.obj gamma.obj \
polevl.obj floor.obj isnan.obj mtherr.obj const.obj rad.obj
	link rad planck polylog unity spence zetac sin exp log pow powi fac gamma polevl floor isnan mtherr const;
