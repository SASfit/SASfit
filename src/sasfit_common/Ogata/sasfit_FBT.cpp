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
    ogata = FBT(nu, option, N, Q);
}

scalar sasfit_FBT(scalar x, sasfit_func_one_void intKern_fct, void *param){

	return ogata.fbt(std::bind(intKern_fct, std::placeholders::_1, param),x);
} // test function to transform data allows to send anything else to the function
