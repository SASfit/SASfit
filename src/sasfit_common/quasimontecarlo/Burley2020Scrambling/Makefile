ALL = genpoints genpoints.so
CXXFLAGS += -O3 -std=c++11 -fPIC -I .

OBJS = genpoints.o faure05.o sobol.o

all : $(ALL)

clean :
	rm -f $(ALL) *.o

genpoints : $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

genpoints.so : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -shared -o $@


faure05.o: faure05.h radicalinverse.h
genpoints.o: genpoints.h sobol.h faure05.h radicalinverse.h owenhash.h pcg.h
main.o: genpoints.h
sobol.o: sobol.h
faure05.o: radicalinverse.h
