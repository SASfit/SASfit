CC = gcc
PACKAGE_DIR = sasfit_220408180523_macos64
PLATFORM := $(shell uname)

all: build run

build: example_sasfit_ff_fuzzysphere.c
	$(CC) -I../../$(PACKAGE_DIR)/plugins -I../src/sasfit_common/include -I../src/sasfit_core/include -I../saskit/src/tcl-8.4.20/generic/ -I../src/gsl/macos64/include/ -I../src/f2c/ -L../../$(PACKAGE_DIR)/lib -lsasfit -L../../$(PACKAGE_DIR)/plugins  -lsasfit_fuzzysphere  -o example_sasfit_ff_fuzzysphere example_sasfit_ff_fuzzysphere.c

ifneq (,$(findstring Darwin,$(PLATFORM)))

# Running on macOS
run: example_sasfit_ff_fuzzysphere
	DYLD_LIBRARY_PATH=../../$(PACKAGE_DIR)/plugins/:../../$(PACKAGE_DIR)/lib/ ./example_sasfit_ff_fuzzysphere

else ifneq (,$(findstring MINGW64,$(PLATFORM)))

# Running on Windows/MinGW64
run: example_sasfit_ff_fuzzysphere.exe
	cp ../../$(PACKAGE_DIR)/plugins/libsasfit_fuzzysphere.dll .
	cp ../../$(PACKAGE_DIR)/lib/libsasfit.dll .
	./example_sasfit_ff_fuzzysphere.exe

endif


