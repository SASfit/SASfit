// bessel.cpp
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "FBT.h"
extern "C"
{
    #include "sasfit_constants.h"
	#include "sasfit_function.h"
    #include "sasfit_hankel.h"
}

FBT ogata = FBT(0.0, 0, 10, 1.0);  // nu_def:0.0, option_def:0 , N_def:1, Q_def:1.0

void sasfit_set_FBT(double nu, int option, int N, double Q) {
    static scalar old_nu=0.0, old_Q=1.0;
    static int old_option=0, old_N=10;
    if (old_nu == nu && old_Q==Q && old_N == N && old_option == option ) {
        return;
    } else {
        ogata  = FBT(nu, option, N, Q);
        old_nu = nu;
        old_option = option;
        old_N  = N;
        old_Q  = Q;
        return;
    }
}

scalar sasfit_FBT(scalar x, double (*intKern_fct)(double, void *), void * fparams){

	return ogata.fbt(std::bind(intKern_fct, std::placeholders::_1, fparams),x);
} // test function to transform data allows to send anything else to the function
