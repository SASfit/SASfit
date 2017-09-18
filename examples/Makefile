example_sasfit_ff_fuzzysphere: example_sasfit_ff_fuzzysphere_stat.c example_sasfit_ff_fuzzysphere_so.c
	gcc -o example_sasfit_ff_fuzzysphere_stat example_sasfit_ff_fuzzysphere_stat.c -static -DSTATIC_ENABLED \
			-I../src/sasfit_common/include -I../src/f2c -I../plugins -I../src/gsl/windows_amd64/include \
			-L../src/plugins/fuzzysphere/lib -L../src/lib -L../src/gsl/windows_amd64/lib \
			-lsasfit_fuzzysphere_stat -lsasfit.dll -lsasfit_common_stat -lgsl -lgslcblas -lm
	gcc -o example_sasfit_ff_fuzzysphere_so example_sasfit_ff_fuzzysphere_so.c \
			-I../src/sasfit_common/include -I../src/f2c -I../plugins -I../src/gsl/windows_amd64/include \
			-L../src/plugins/fuzzysphere/lib -L../src/lib -L../src/gsl/windows_amd64/lib \
			-Wl,-Bdynamic  -lsasfit -lsasfit_fuzzysphere \
			-Wl,-Bstatic   -lsasfit_common_stat  -lgsl -lgslcblas -lm  \