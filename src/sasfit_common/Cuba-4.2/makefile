CC = gcc
SUFFIX = 
CFLAGS = -O3 -fomit-frame-pointer -ffast-math -Wall -DHAVE_CONFIG_H -DREALSIZE=8 -I$(common) -I. -I$(srcdir)
MCFLAGS = 
FC = gfortran
FCFLAGS = -g -O2 -DREALSIZE=8
LIBS = -lm
INSTALL = /bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
RANLIB = ranlib
EXE = .exe

prefix = /usr/local
exec_prefix = ${prefix}
srcdir = .
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
datarootdir = ${prefix}/share
datadir = ${datarootdir}

MATH_DEFAULT = 
TOOLS_DEFAULT = 
DEMO_FORTRAN_DEFAULT = demo-fortran$(EXE)

MCC = CC="$(tools)/fcc" CXX="$(tools)/fcc -stdlib=libstdc++" \
  REALCC="$(CC)" PATH="$(PATH):$(tools)" mcc

TARDIR = Cuba-4.2
TARFILE = $(TARDIR).tar.gz


HEADERS = cuba$(SUFFIX).h
LIB = libcuba$(SUFFIX).a
MATH = Vegas$(SUFFIX)$(EXE) Suave$(SUFFIX)$(EXE) Divonne$(SUFFIX)$(EXE) Cuhre$(SUFFIX)$(EXE)
TOOLS = partview$(EXE)


all: lib demos $(MATH_DEFAULT) $(TOOLS_DEFAULT)

lib: $(LIB) $(HEADERS)

demos: demo-c$(EXE) $(DEMO_FORTRAN_DEFAULT)

math: $(MATH)

tools: $(TOOLS)


install: lib $(TOOLS_DEFAULT)
	$(INSTALL) -m 755 -d $(DESTDIR)$(libdir) \
	                     $(DESTDIR)$(includedir) \
	                     $(DESTDIR)$(bindir) \
	                     $(DESTDIR)$(datadir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(libdir)
	for f in $(HEADERS) ; do \
	  $(INSTALL_DATA) `test -f $$f || echo $(srcdir)/`$$f $(DESTDIR)$(includedir); \
	done
	for f in $(MATH) $(TOOLS) ; do \
	  test -f $$f && $(INSTALL_PROGRAM) $$f $(DESTDIR)$(bindir) || : ; \
	done
	$(INSTALL_DATA) $(srcdir)/cuba.pdf $(DESTDIR)$(datadir)


check: demo-c
	./demo-c | grep RESULT > demo-c.out
	@echo diff demo-c.out $(demo)/demo-c.out
	@diff demo-c.out $(demo)/demo-c.out || \
	echo "Note: This is serious only if the results deviate significantly."

.SUFFIXES:
.SUFFIXES: .c .f .tm

common = $(srcdir)/src/common
vegas = $(srcdir)/src/vegas
suave = $(srcdir)/src/suave
divonne = $(srcdir)/src/divonne
cuhre = $(srcdir)/src/cuhre
demo = $(srcdir)/demo
tools = $(srcdir)/tools


COMMON_DEPS = $(common)/stddecl.h $(common)/ChiSquare.c $(common)/Erf.c
COMMON_CDEPS = $(COMMON_DEPS) $(common)/CSample.c $(common)/Parallel.c $(common)/sock.h
COMMON_TMDEPS = $(COMMON_DEPS) $(common)/MSample.c

VEGAS_C = $(vegas)/Vegas.c
VEGAS_TM = $(vegas)/Vegas.tm
VEGAS_DEPS = $(common)/Random.c \
  $(vegas)/decl.h $(vegas)/common.c \
  $(vegas)/Grid.c $(vegas)/Integrate.c
VEGAS = $(VEGAS_C) $(VEGAS_TM) $(VEGAS_DEPS)

$(LIB)(Vegas.o): config.h $(VEGAS_C) $(VEGAS_DEPS) $(COMMON_CDEPS) 
	$(CC) $(CFLAGS) -I$(vegas) -DNOUNDERSCORE -c -o Vegas.o $(VEGAS_C)
	$(AR) $(ARFLAGS) $(LIB) Vegas.o
	$(RM) Vegas.o

$(LIB)(llVegas.o): config.h $(VEGAS_C) $(VEGAS_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(vegas) -DLONGLONGINT -DNOUNDERSCORE -c -o llVegas.o $(VEGAS_C)
	$(AR) $(ARFLAGS) $(LIB) llVegas.o
	$(RM) llVegas.o

$(LIB)(Vegas_.o): config.h $(VEGAS_C) $(VEGAS_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(vegas) -c -o Vegas_.o $(VEGAS_C)
	$(AR) $(ARFLAGS) $(LIB) Vegas_.o
	$(RM) Vegas_.o

$(LIB)(llVegas_.o): config.h $(VEGAS_C) $(VEGAS_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(vegas) -DLONGLONGINT -c -o llVegas_.o $(VEGAS_C)
	$(AR) $(ARFLAGS) $(LIB) llVegas_.o
	$(RM) llVegas_.o

Vegas$(EXE): config.h $(VEGAS_TM) $(VEGAS_DEPS) $(COMMON_TMDEPS)
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(vegas) -o Vegas$(EXE) $(VEGAS_TM)
	strip Vegas$(EXE)

Vegasl$(EXE): config.h $(VEGAS_TM) $(VEGAS_DEPS) $(COMMON_TMDEPS)
	sed '/:Begin:/,/:End:/ s/Real64/Real128/g' $(VEGAS_TM) > Vegasl.tm
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(vegas) -o Vegasl$(EXE) Vegasl.tm
	strip Vegasl$(EXE)
	$(RM) Vegasl.tm

Vegasq$(EXE):


SUAVE_C = $(suave)/Suave.c
SUAVE_TM = $(suave)/Suave.tm
SUAVE_DEPS = $(common)/Random.c \
  $(suave)/decl.h $(suave)/common.c \
  $(suave)/Grid.c $(suave)/Fluct.c $(suave)/Sample.c \
  $(suave)/Integrate.c
SUAVE = $(SUAVE_C) $(SUAVE_TM) $(SUAVE_DEPS)

$(LIB)(Suave.o): config.h $(SUAVE_C) $(SUAVE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(suave) -DNOUNDERSCORE -c -o Suave.o $(SUAVE_C)
	$(AR) $(ARFLAGS) $(LIB) Suave.o
	$(RM) Suave.o

$(LIB)(llSuave.o): config.h $(SUAVE_C) $(SUAVE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(suave) -DLONGLONGINT -DNOUNDERSCORE -c -o llSuave.o $(SUAVE_C)
	$(AR) $(ARFLAGS) $(LIB) llSuave.o
	$(RM) llSuave.o

$(LIB)(Suave_.o): config.h $(SUAVE_C) $(SUAVE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(suave) -c -o Suave_.o $(SUAVE_C)
	$(AR) $(ARFLAGS) $(LIB) Suave_.o
	$(RM) Suave_.o

$(LIB)(llSuave_.o): config.h $(SUAVE_C) $(SUAVE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(suave) -DLONGLONGINT -c -o llSuave_.o $(SUAVE_C)
	$(AR) $(ARFLAGS) $(LIB) llSuave_.o
	$(RM) llSuave_.o

Suave$(EXE): config.h $(SUAVE_TM) $(SUAVE_DEPS) $(COMMON_TMDEPS)
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(suave) -o Suave$(EXE) $(SUAVE_TM)
	strip Suave$(EXE)

Suavel$(EXE): config.h $(SUAVE_TM) $(SUAVE_DEPS) $(COMMON_TMDEPS)
	sed '/:Begin:/,/:End:/ s/Real64/Real128/g' $(SUAVE_TM) > Suavel.tm
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(suave) -o Suavel$(EXE) Suavel.tm
	strip Suavel$(EXE)
	$(RM) Suavel.tm

Suaveq$(EXE):


DIVONNE_C = $(divonne)/Divonne.c
DIVONNE_TM = $(divonne)/Divonne.tm
DIVONNE_DEPS = $(common)/Random.c \
  $(divonne)/decl.h $(divonne)/common.c \
  $(divonne)/KorobovCoeff.c $(divonne)/Rule.c $(divonne)/Sample.c \
  $(divonne)/FindMinimum.c $(divonne)/Iterate.c $(divonne)/Explore.c \
  $(divonne)/Split.c $(divonne)/Integrate.c
DIVONNE = $(DIVONNE_C) $(DIVONNE_TM) $(DIVONNE_DEPS)

$(LIB)(Divonne.o): config.h $(DIVONNE_C) $(DIVONNE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(divonne) -DNOUNDERSCORE -c -o Divonne.o $(DIVONNE_C)
	$(AR) $(ARFLAGS) $(LIB) Divonne.o
	$(RM) Divonne.o

$(LIB)(llDivonne.o): config.h $(DIVONNE_C) $(DIVONNE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(divonne) -DLONGLONGINT -DNOUNDERSCORE -c -o llDivonne.o $(DIVONNE_C)
	$(AR) $(ARFLAGS) $(LIB) llDivonne.o
	$(RM) llDivonne.o

$(LIB)(Divonne_.o): config.h $(DIVONNE_C) $(DIVONNE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(divonne) -c -o Divonne_.o $(DIVONNE_C)
	$(AR) $(ARFLAGS) $(LIB) Divonne_.o
	$(RM) Divonne_.o

$(LIB)(llDivonne_.o): config.h $(DIVONNE_C) $(DIVONNE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(divonne) -DLONGLONGINT -c -o llDivonne_.o $(DIVONNE_C)
	$(AR) $(ARFLAGS) $(LIB) llDivonne_.o
	$(RM) llDivonne_.o

Divonne$(EXE): config.h $(DIVONNE_TM) $(DIVONNE_DEPS) $(COMMON_TMDEPS)
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(divonne) -o Divonne$(EXE) $(DIVONNE_TM)
	strip Divonne$(EXE)

Divonnel$(EXE): config.h $(DIVONNE_TM) $(DIVONNE_DEPS) $(COMMON_TMDEPS)
	sed '/:Begin:/,/:End:/ s/Real64/Real128/g' $(DIVONNE_TM) > Divonnel.tm
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(divonne) -o Divonnel$(EXE) Divonnel.tm
	strip Divonnel$(EXE)
	$(RM) Divonnel.tm

Divonneq$(EXE):


CUHRE_C = $(cuhre)/Cuhre.c
CUHRE_TM = $(cuhre)/Cuhre.tm
CUHRE_DEPS = \
  $(cuhre)/decl.h $(cuhre)/common.c \
  $(cuhre)/Rule.c $(cuhre)/Integrate.c
CUHRE = $(CUHRE_C) $(CUHRE_TM) $(CUHRE_DEPS)

$(LIB)(Cuhre.o): config.h $(CUHRE_C) $(CUHRE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(cuhre) -DNOUNDERSCORE -c -o Cuhre.o $(CUHRE_C)
	$(AR) $(ARFLAGS) $(LIB) Cuhre.o
	$(RM) Cuhre.o

$(LIB)(llCuhre.o): config.h $(CUHRE_C) $(CUHRE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(cuhre) -DLONGLONGINT -DNOUNDERSCORE -c -o llCuhre.o $(CUHRE_C)
	$(AR) $(ARFLAGS) $(LIB) llCuhre.o
	$(RM) llCuhre.o

$(LIB)(Cuhre_.o): config.h $(CUHRE_C) $(CUHRE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(cuhre) -c -o Cuhre_.o $(CUHRE_C)
	$(AR) $(ARFLAGS) $(LIB) Cuhre_.o
	$(RM) Cuhre_.o

$(LIB)(llCuhre_.o): config.h $(CUHRE_C) $(CUHRE_DEPS) $(COMMON_CDEPS)
	$(CC) $(CFLAGS) -I$(cuhre) -DLONGLONGINT -c -o llCuhre_.o $(CUHRE_C)
	$(AR) $(ARFLAGS) $(LIB) llCuhre_.o
	$(RM) llCuhre_.o

Cuhre$(EXE): config.h $(CUHRE_TM) $(CUHRE_DEPS) $(COMMON_TMDEPS)
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(cuhre) -o Cuhre$(EXE) $(CUHRE_TM)
	strip Cuhre$(EXE)

Cuhrel$(EXE): config.h $(CUHRE_TM) $(CUHRE_DEPS) $(COMMON_TMDEPS)
	sed '/:Begin:/,/:End:/ s/Real64/Real128/g' $(CUHRE_TM) > Cuhrel.tm
	$(MCC) $(MCFLAGS) $(CFLAGS) -I$(cuhre) -o Cuhrel$(EXE) Cuhrel.tm
	strip Cuhrel$(EXE)
	$(RM) Cuhrel.tm

Cuhreq$(EXE):


$(LIB)(Fork.o): $(common)/Fork.c $(common)/stddecl.h $(common)/sock.h
	$(CC) $(CFLAGS) -DNOUNDERSCORE -c -o Fork.o $(common)/Fork.c
	$(AR) $(ARFLAGS) $(LIB) Fork.o
	$(RM) Fork.o

$(LIB)(Fork_.o): $(common)/Fork.c $(common)/stddecl.h $(common)/sock.h
	$(CC) $(CFLAGS) -c -o Fork_.o $(common)/Fork.c
	$(AR) $(ARFLAGS) $(LIB) Fork_.o
	$(RM) Fork_.o


$(LIB)(Global.o): $(common)/Global.c $(common)/stddecl.h
	$(CC) $(CFLAGS) -DNOUNDERSCORE -c -o Global.o $(common)/Global.c
	$(AR) $(ARFLAGS) $(LIB) Global.o
	$(RM) Global.o

$(LIB)(Global_.o): $(common)/Global.c $(common)/stddecl.h
	$(CC) $(CFLAGS) -c -o Global_.o $(common)/Global.c
	$(AR) $(ARFLAGS) $(LIB) Global_.o
	$(RM) Global_.o


$(LIB)(Data.o): $(common)/Data.c $(common)/stddecl.h
	$(CC) $(CFLAGS) -c -o Data.o $(common)/Data.c
	$(AR) $(ARFLAGS) $(LIB) Data.o
	$(RM) Data.o


$(LIB): $(LIB)(Vegas.o)     $(LIB)(Vegas_.o)     \
	$(LIB)(llVegas.o)   $(LIB)(llVegas_.o)   \
	$(LIB)(Suave.o)     $(LIB)(Suave_.o)     \
	$(LIB)(llSuave.o)   $(LIB)(llSuave_.o)   \
	$(LIB)(Divonne.o)   $(LIB)(Divonne_.o)   \
	$(LIB)(llDivonne.o) $(LIB)(llDivonne_.o) \
	$(LIB)(Cuhre.o)     $(LIB)(Cuhre_.o)     \
	$(LIB)(llCuhre.o)   $(LIB)(llCuhre_.o)   \
	$(LIB)(Fork.o)      $(LIB)(Fork_.o)      \
	$(LIB)(Global.o)    $(LIB)(Global_.o)    \
	$(LIB)(Data.o)
	-$(RANLIB) $(LIB)


cubal.h: cuba.h
	sed 's/double/long double/g' cuba.h > cubal.h

cubaq.h: cuba.h
	sed 's/double/__float128/g' cuba.h > cubaq.h


DEMO_F = $(demo)/demo-fortran.F
DEMO_C = $(demo)/demo-c.c
DEMO_TM = $(demo)/demo-math.m $(demo)/testsuite.m
DEMO = $(DEMO_F) $(DEMO_C) $(DEMO_TM) $(demo)/demo-c.out $(demo)/cuba.F

demo-fortran$(EXE): $(DEMO_F) $(LIB)
	$(FC) $(FCFLAGS) -o demo-fortran$(EXE) $(DEMO_F) $(LIB) $(LIBS)

demo-c$(EXE): $(DEMO_C) $(srcdir)/cuba.h $(LIB)
	$(CC) $(CFLAGS) -o demo-c$(EXE) $(DEMO_C) $(LIB) $(LIBS)


PARTVIEW = $(tools)/partview.cpp $(tools)/partview.pro \
  $(tools)/quit.xpm $(tools)/print.xpm

partview$(EXE): $(PARTVIEW)
	qmake -o makefile.partview $(tools)/partview.pro
	$(MAKE) -f makefile.partview
	$(RM) partview.o partview.moc makefile.partview
	-strip partview$(EXE)


TARCONTENTS = $(srcdir)/COPYING $(srcdir)/ChangeLog \
  $(srcdir)/configure $(srcdir)/configure.ac \
  $(srcdir)/config.sub $(srcdir)/config.guess $(srcdir)/install-sh \
  $(srcdir)/makefile.in $(srcdir)/config.h.in \
  $(srcdir)/cuba.pdf $(srcdir)/cuba.tex $(srcdir)/cuba.h \
  $(COMMON_CDEPS) $(COMMON_TMDEPS) \
  $(common)/Global.c $(common)/Fork.c $(common)/Data.c \
  $(VEGAS) $(SUAVE) $(DIVONNE) $(divonne)/KorobovCoeff.c-* $(CUHRE) \
  $(DEMO) $(PARTVIEW) $(tools)/partview.m \
  $(tools)/mcc $(tools)/fcc $(tools)/mkstatic $(tools)/mkdist.c

dist:
	mkdist cvfz $(TARFILE) $(TARDIR) \
	  $(patsubst $(srcdir)/%,%, $(sort $(TARCONTENTS)))

#	ln -s . $(TARDIR)
#	tar cvfz $(TARFILE) --owner=root --group=root \
#	  $(patsubst $(srcdir)/%,$(TARDIR)/%, $(sort $(TARCONTENTS)))
#	$(RM) $(TARDIR)

pub: dist
	mv -f $(TARFILE) web/
	./mkwebpage

clean:
	-$(RM) $(TARFILE) $(TARDIR) demo-c.out \
	  partview.moc partview.o makefile.partview \
	  cubal.h cubaq.h *.o */*.o */*.tm.c *.tm

distclean: clean
	-$(RM) $(LIB) demo-c demo-fortran $(MATH) $(TOOLS) \
	  config.h config.log config.status makefile

