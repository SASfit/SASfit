int Frolov_2_1_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (2*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_1_basic.inc"
	return 0;
}

int Frolov_2_2_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (2*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_2_basic.inc"
	return 0;
}

int Frolov_2_3_basic ( double *x, double *w, int *npoints) {
	*npoints = 9;
	*w = 0.125;
	x = ( double * ) malloc (2*9 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_3_basic.inc"
	return 0;
}

int Frolov_2_4_basic ( double *x, double *w, int *npoints) {
	*npoints = 15;
	*w = 0.0625;
	x = ( double * ) malloc (2*15 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_4_basic.inc"
	return 0;
}

int Frolov_2_5_basic ( double *x, double *w, int *npoints) {
	*npoints = 31;
	*w = 0.03125;
	x = ( double * ) malloc (2*31 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_5_basic.inc"
	return 0;
}

int Frolov_2_6_basic ( double *x, double *w, int *npoints) {
	*npoints = 63;
	*w = 0.015625;
	x = ( double * ) malloc (2*63 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_6_basic.inc"
	return 0;
}

int Frolov_2_7_basic ( double *x, double *w, int *npoints) {
	*npoints = 127;
	*w = 0.0078125;
	x = ( double * ) malloc (2*127 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_7_basic.inc"
	return 0;
}

int Frolov_2_8_basic ( double *x, double *w, int *npoints) {
	*npoints = 257;
	*w = 0.00390625;
	x = ( double * ) malloc (2*257 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_8_basic.inc"
	return 0;
}

int Frolov_2_9_basic ( double *x, double *w, int *npoints) {
	*npoints = 511;
	*w = 0.001953125;
	x = ( double * ) malloc (2*511 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_9_basic.inc"
	return 0;
}

int Frolov_2_10_basic ( double *x, double *w, int *npoints) {
	*npoints = 1023;
	*w = 0.0009765625;
	x = ( double * ) malloc (2*1023 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_10_basic.inc"
	return 0;
}

int Frolov_2_11_basic ( double *x, double *w, int *npoints) {
	*npoints = 2049;
	*w = 0.00048828125;
	x = ( double * ) malloc (2*2049 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_11_basic.inc"
	return 0;
}

int Frolov_2_12_basic ( double *x, double *w, int *npoints) {
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (2*4093 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_12_basic.inc"
	return 0;
}

int Frolov_2_13_basic ( double *x, double *w, int *npoints) {
	*npoints = 8195;
	*w = 0.0001220703125;
	x = ( double * ) malloc (2*8195 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_13_basic.inc"
	return 0;
}

int Frolov_2_14_basic ( double *x, double *w, int *npoints) {
	*npoints = 16387;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (2*16387 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_14_basic.inc"
	return 0;
}

int Frolov_2_15_basic ( double *x, double *w, int *npoints) {
	*npoints = 32771;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (2*32771 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_15_basic.inc"
	return 0;
}

int Frolov_2_16_basic ( double *x, double *w, int *npoints) {
	*npoints = 65533;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (2*65533 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_2_16_basic.inc"
	return 0;
}

int Frolov_3_1_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (3*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_1_basic.inc"
	return 0;
}

int Frolov_3_2_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (3*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_2_basic.inc"
	return 0;
}

int Frolov_3_3_basic ( double *x, double *w, int *npoints) {
	*npoints = 9;
	*w = 0.125;
	x = ( double * ) malloc (3*9 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_3_basic.inc"
	return 0;
}

int Frolov_3_4_basic ( double *x, double *w, int *npoints) {
	*npoints = 17;
	*w = 0.0625;
	x = ( double * ) malloc (3*17 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_4_basic.inc"
	return 0;
}

int Frolov_3_5_basic ( double *x, double *w, int *npoints) {
	*npoints = 31;
	*w = 0.03125;
	x = ( double * ) malloc (3*31 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_5_basic.inc"
	return 0;
}

int Frolov_3_6_basic ( double *x, double *w, int *npoints) {
	*npoints = 67;
	*w = 0.015625;
	x = ( double * ) malloc (3*67 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_6_basic.inc"
	return 0;
}

int Frolov_3_7_basic ( double *x, double *w, int *npoints) {
	*npoints = 129;
	*w = 0.0078125;
	x = ( double * ) malloc (3*129 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_7_basic.inc"
	return 0;
}

int Frolov_3_8_basic ( double *x, double *w, int *npoints) {
	*npoints = 253;
	*w = 0.00390625;
	x = ( double * ) malloc (3*253 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_8_basic.inc"
	return 0;
}

int Frolov_3_9_basic ( double *x, double *w, int *npoints) {
	*npoints = 513;
	*w = 0.001953125;
	x = ( double * ) malloc (3*513 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_9_basic.inc"
	return 0;
}

int Frolov_3_10_basic ( double *x, double *w, int *npoints) {
	*npoints = 1021;
	*w = 0.0009765625;
	x = ( double * ) malloc (3*1021 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_10_basic.inc"
	return 0;
}

int Frolov_3_11_basic ( double *x, double *w, int *npoints) {
	*npoints = 2041;
	*w = 0.00048828125;
	x = ( double * ) malloc (3*2041 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_11_basic.inc"
	return 0;
}

int Frolov_3_12_basic ( double *x, double *w, int *npoints) {
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (3*4093 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_12_basic.inc"
	return 0;
}

int Frolov_3_13_basic ( double *x, double *w, int *npoints) {
	*npoints = 8193;
	*w = 0.0001220703125;
	x = ( double * ) malloc (3*8193 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_13_basic.inc"
	return 0;
}

int Frolov_3_14_basic ( double *x, double *w, int *npoints) {
	*npoints = 16387;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (3*16387 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_14_basic.inc"
	return 0;
}

int Frolov_3_15_basic ( double *x, double *w, int *npoints) {
	*npoints = 32773;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (3*32773 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_15_basic.inc"
	return 0;
}

int Frolov_3_16_basic ( double *x, double *w, int *npoints) {
	*npoints = 65537;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (3*65537 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_3_16_basic.inc"
	return 0;
}

int Frolov_4_1_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (4*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_1_basic.inc"
	return 0;
}

int Frolov_4_2_basic ( double *x, double *w, int *npoints) {
	*npoints = 7;
	*w = 0.25;
	x = ( double * ) malloc (4*7 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_2_basic.inc"
	return 0;
}

int Frolov_4_3_basic ( double *x, double *w, int *npoints) {
	*npoints = 17;
	*w = 0.125;
	x = ( double * ) malloc (4*17 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_3_basic.inc"
	return 0;
}

int Frolov_4_4_basic ( double *x, double *w, int *npoints) {
	*npoints = 19;
	*w = 0.0625;
	x = ( double * ) malloc (4*19 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_4_basic.inc"
	return 0;
}

int Frolov_4_5_basic ( double *x, double *w, int *npoints) {
	*npoints = 39;
	*w = 0.03125;
	x = ( double * ) malloc (4*39 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_5_basic.inc"
	return 0;
}

int Frolov_4_6_basic ( double *x, double *w, int *npoints) {
	*npoints = 65;
	*w = 0.015625;
	x = ( double * ) malloc (4*65 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_6_basic.inc"
	return 0;
}

int Frolov_4_7_basic ( double *x, double *w, int *npoints) {
	*npoints = 139;
	*w = 0.0078125;
	x = ( double * ) malloc (4*139 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_7_basic.inc"
	return 0;
}

int Frolov_4_8_basic ( double *x, double *w, int *npoints) {
	*npoints = 261;
	*w = 0.00390625;
	x = ( double * ) malloc (4*261 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_8_basic.inc"
	return 0;
}

int Frolov_4_9_basic ( double *x, double *w, int *npoints) {
	*npoints = 515;
	*w = 0.001953125;
	x = ( double * ) malloc (4*515 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_9_basic.inc"
	return 0;
}

int Frolov_4_10_basic ( double *x, double *w, int *npoints) {
	*npoints = 1023;
	*w = 0.0009765625;
	x = ( double * ) malloc (4*1023 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_10_basic.inc"
	return 0;
}

int Frolov_4_11_basic ( double *x, double *w, int *npoints) {
	*npoints = 2055;
	*w = 0.00048828125;
	x = ( double * ) malloc (4*2055 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_11_basic.inc"
	return 0;
}

int Frolov_4_12_basic ( double *x, double *w, int *npoints) {
	*npoints = 4103;
	*w = 0.000244140625;
	x = ( double * ) malloc (4*4103 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_12_basic.inc"
	return 0;
}

int Frolov_4_13_basic ( double *x, double *w, int *npoints) {
	*npoints = 8197;
	*w = 0.0001220703125;
	x = ( double * ) malloc (4*8197 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_13_basic.inc"
	return 0;
}

int Frolov_4_14_basic ( double *x, double *w, int *npoints) {
	*npoints = 16395;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (4*16395 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_14_basic.inc"
	return 0;
}

int Frolov_4_15_basic ( double *x, double *w, int *npoints) {
	*npoints = 32763;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (4*32763 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_15_basic.inc"
	return 0;
}

int Frolov_4_16_basic ( double *x, double *w, int *npoints) {
	*npoints = 65551;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (4*65551 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_4_16_basic.inc"
	return 0;
}

int Frolov_5_1_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (5*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_1_basic.inc"
	return 0;
}

int Frolov_5_2_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (5*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_2_basic.inc"
	return 0;
}

int Frolov_5_3_basic ( double *x, double *w, int *npoints) {
	*npoints = 13;
	*w = 0.125;
	x = ( double * ) malloc (5*13 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_3_basic.inc"
	return 0;
}

int Frolov_5_4_basic ( double *x, double *w, int *npoints) {
	*npoints = 15;
	*w = 0.0625;
	x = ( double * ) malloc (5*15 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_4_basic.inc"
	return 0;
}

int Frolov_5_5_basic ( double *x, double *w, int *npoints) {
	*npoints = 35;
	*w = 0.03125;
	x = ( double * ) malloc (5*35 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_5_basic.inc"
	return 0;
}

int Frolov_5_6_basic ( double *x, double *w, int *npoints) {
	*npoints = 65;
	*w = 0.015625;
	x = ( double * ) malloc (5*65 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_6_basic.inc"
	return 0;
}

int Frolov_5_7_basic ( double *x, double *w, int *npoints) {
	*npoints = 137;
	*w = 0.0078125;
	x = ( double * ) malloc (5*137 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_7_basic.inc"
	return 0;
}

int Frolov_5_8_basic ( double *x, double *w, int *npoints) {
	*npoints = 267;
	*w = 0.00390625;
	x = ( double * ) malloc (5*267 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_8_basic.inc"
	return 0;
}

int Frolov_5_9_basic ( double *x, double *w, int *npoints) {
	*npoints = 509;
	*w = 0.001953125;
	x = ( double * ) malloc (5*509 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_9_basic.inc"
	return 0;
}

int Frolov_5_10_basic ( double *x, double *w, int *npoints) {
	*npoints = 1021;
	*w = 0.0009765625;
	x = ( double * ) malloc (5*1021 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_10_basic.inc"
	return 0;
}

int Frolov_5_11_basic ( double *x, double *w, int *npoints) {
	*npoints = 2051;
	*w = 0.00048828125;
	x = ( double * ) malloc (5*2051 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_11_basic.inc"
	return 0;
}

int Frolov_5_12_basic ( double *x, double *w, int *npoints) {
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (5*4093 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_12_basic.inc"
	return 0;
}

int Frolov_5_13_basic ( double *x, double *w, int *npoints) {
	*npoints = 8185;
	*w = 0.0001220703125;
	x = ( double * ) malloc (5*8185 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_13_basic.inc"
	return 0;
}

int Frolov_5_14_basic ( double *x, double *w, int *npoints) {
	*npoints = 16359;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (5*16359 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_14_basic.inc"
	return 0;
}

int Frolov_5_15_basic ( double *x, double *w, int *npoints) {
	*npoints = 32761;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (5*32761 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_15_basic.inc"
	return 0;
}

int Frolov_5_16_basic ( double *x, double *w, int *npoints) {
	*npoints = 65533;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (5*65533 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_5_16_basic.inc"
	return 0;
}

int Frolov_6_1_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (6*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_1_basic.inc"
	return 0;
}

int Frolov_6_2_basic ( double *x, double *w, int *npoints) {
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (6*3 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_2_basic.inc"
	return 0;
}

int Frolov_6_3_basic ( double *x, double *w, int *npoints) {
	*npoints = 17;
	*w = 0.125;
	x = ( double * ) malloc (6*17 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_3_basic.inc"
	return 0;
}

int Frolov_6_4_basic ( double *x, double *w, int *npoints) {
	*npoints = 21;
	*w = 0.0625;
	x = ( double * ) malloc (6*21 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_4_basic.inc"
	return 0;
}

int Frolov_6_5_basic ( double *x, double *w, int *npoints) {
	*npoints = 27;
	*w = 0.03125;
	x = ( double * ) malloc (6*27 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_5_basic.inc"
	return 0;
}

int Frolov_6_6_basic ( double *x, double *w, int *npoints) {
	*npoints = 57;
	*w = 0.015625;
	x = ( double * ) malloc (6*57 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_6_basic.inc"
	return 0;
}

int Frolov_6_7_basic ( double *x, double *w, int *npoints) {
	*npoints = 149;
	*w = 0.0078125;
	x = ( double * ) malloc (6*149 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_7_basic.inc"
	return 0;
}

int Frolov_6_8_basic ( double *x, double *w, int *npoints) {
	*npoints = 275;
	*w = 0.00390625;
	x = ( double * ) malloc (6*275 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_8_basic.inc"
	return 0;
}

int Frolov_6_9_basic ( double *x, double *w, int *npoints) {
	*npoints = 517;
	*w = 0.001953125;
	x = ( double * ) malloc (6*517 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_9_basic.inc"
	return 0;
}

int Frolov_6_10_basic ( double *x, double *w, int *npoints) {
	*npoints = 1005;
	*w = 0.0009765625;
	x = ( double * ) malloc (6*1005 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_10_basic.inc"
	return 0;
}

int Frolov_6_11_basic ( double *x, double *w, int *npoints) {
	*npoints = 2051;
	*w = 0.00048828125;
	x = ( double * ) malloc (6*2051 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_11_basic.inc"
	return 0;
}

int Frolov_6_12_basic ( double *x, double *w, int *npoints) {
	*npoints = 4087;
	*w = 0.000244140625;
	x = ( double * ) malloc (6*4087 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_12_basic.inc"
	return 0;
}

int Frolov_6_13_basic ( double *x, double *w, int *npoints) {
	*npoints = 8207;
	*w = 0.0001220703125;
	x = ( double * ) malloc (6*8207 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_13_basic.inc"
	return 0;
}

int Frolov_6_14_basic ( double *x, double *w, int *npoints) {
	*npoints = 16401;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (6*16401 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_14_basic.inc"
	return 0;
}

int Frolov_6_15_basic ( double *x, double *w, int *npoints) {
	*npoints = 32765;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (6*32765 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_15_basic.inc"
	return 0;
}

int Frolov_6_16_basic ( double *x, double *w, int *npoints) {
	*npoints = 65513;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (6*65513 * sizeof (double) );
	if (x==NULL) return 1;
	#include "Frolov_6_16_basic.inc"
	return 0;
}

int Frolov(int dim, int level, double *x, double *w, int *npoints) {
	if (dim<2||dim>6||level<1||level>16) return 2;
	if (dim==2 && level==1) {
		return Frolov_2_1_basis (x,w,npoints);
	} else 	if (dim==2 && level==2) {
		return Frolov_2_2_basis (x,w,npoints);
	} else 	if (dim==2 && level==3) {
		return Frolov_2_3_basis (x,w,npoints);
	} else 	if (dim==2 && level==4) {
		return Frolov_2_4_basis (x,w,npoints);
	} else 	if (dim==2 && level==5) {
		return Frolov_2_5_basis (x,w,npoints);
	} else 	if (dim==2 && level==6) {
		return Frolov_2_6_basis (x,w,npoints);
	} else 	if (dim==2 && level==7) {
		return Frolov_2_7_basis (x,w,npoints);
	} else 	if (dim==2 && level==8) {
		return Frolov_2_8_basis (x,w,npoints);
	} else 	if (dim==2 && level==9) {
		return Frolov_2_9_basis (x,w,npoints);
	} else 	if (dim==2 && level==10) {
		return Frolov_2_10_basis (x,w,npoints);
	} else 	if (dim==2 && level==11) {
		return Frolov_2_11_basis (x,w,npoints);
	} else 	if (dim==2 && level==12) {
		return Frolov_2_12_basis (x,w,npoints);
	} else 	if (dim==2 && level==13) {
		return Frolov_2_13_basis (x,w,npoints);
	} else 	if (dim==2 && level==14) {
		return Frolov_2_14_basis (x,w,npoints);
	} else 	if (dim==2 && level==15) {
		return Frolov_2_15_basis (x,w,npoints);
	} else 	if (dim==2 && level==16) {
		return Frolov_2_16_basis (x,w,npoints);
	} else 	if (dim==3 && level==1) {
		return Frolov_3_1_basis (x,w,npoints);
	} else 	if (dim==3 && level==2) {
		return Frolov_3_2_basis (x,w,npoints);
	} else 	if (dim==3 && level==3) {
		return Frolov_3_3_basis (x,w,npoints);
	} else 	if (dim==3 && level==4) {
		return Frolov_3_4_basis (x,w,npoints);
	} else 	if (dim==3 && level==5) {
		return Frolov_3_5_basis (x,w,npoints);
	} else 	if (dim==3 && level==6) {
		return Frolov_3_6_basis (x,w,npoints);
	} else 	if (dim==3 && level==7) {
		return Frolov_3_7_basis (x,w,npoints);
	} else 	if (dim==3 && level==8) {
		return Frolov_3_8_basis (x,w,npoints);
	} else 	if (dim==3 && level==9) {
		return Frolov_3_9_basis (x,w,npoints);
	} else 	if (dim==3 && level==10) {
		return Frolov_3_10_basis (x,w,npoints);
	} else 	if (dim==3 && level==11) {
		return Frolov_3_11_basis (x,w,npoints);
	} else 	if (dim==3 && level==12) {
		return Frolov_3_12_basis (x,w,npoints);
	} else 	if (dim==3 && level==13) {
		return Frolov_3_13_basis (x,w,npoints);
	} else 	if (dim==3 && level==14) {
		return Frolov_3_14_basis (x,w,npoints);
	} else 	if (dim==3 && level==15) {
		return Frolov_3_15_basis (x,w,npoints);
	} else 	if (dim==3 && level==16) {
		return Frolov_3_16_basis (x,w,npoints);
	} else 	if (dim==4 && level==1) {
		return Frolov_4_1_basis (x,w,npoints);
	} else 	if (dim==4 && level==2) {
		return Frolov_4_2_basis (x,w,npoints);
	} else 	if (dim==4 && level==3) {
		return Frolov_4_3_basis (x,w,npoints);
	} else 	if (dim==4 && level==4) {
		return Frolov_4_4_basis (x,w,npoints);
	} else 	if (dim==4 && level==5) {
		return Frolov_4_5_basis (x,w,npoints);
	} else 	if (dim==4 && level==6) {
		return Frolov_4_6_basis (x,w,npoints);
	} else 	if (dim==4 && level==7) {
		return Frolov_4_7_basis (x,w,npoints);
	} else 	if (dim==4 && level==8) {
		return Frolov_4_8_basis (x,w,npoints);
	} else 	if (dim==4 && level==9) {
		return Frolov_4_9_basis (x,w,npoints);
	} else 	if (dim==4 && level==10) {
		return Frolov_4_10_basis (x,w,npoints);
	} else 	if (dim==4 && level==11) {
		return Frolov_4_11_basis (x,w,npoints);
	} else 	if (dim==4 && level==12) {
		return Frolov_4_12_basis (x,w,npoints);
	} else 	if (dim==4 && level==13) {
		return Frolov_4_13_basis (x,w,npoints);
	} else 	if (dim==4 && level==14) {
		return Frolov_4_14_basis (x,w,npoints);
	} else 	if (dim==4 && level==15) {
		return Frolov_4_15_basis (x,w,npoints);
	} else 	if (dim==4 && level==16) {
		return Frolov_4_16_basis (x,w,npoints);
	} else 	if (dim==5 && level==1) {
		return Frolov_5_1_basis (x,w,npoints);
	} else 	if (dim==5 && level==2) {
		return Frolov_5_2_basis (x,w,npoints);
	} else 	if (dim==5 && level==3) {
		return Frolov_5_3_basis (x,w,npoints);
	} else 	if (dim==5 && level==4) {
		return Frolov_5_4_basis (x,w,npoints);
	} else 	if (dim==5 && level==5) {
		return Frolov_5_5_basis (x,w,npoints);
	} else 	if (dim==5 && level==6) {
		return Frolov_5_6_basis (x,w,npoints);
	} else 	if (dim==5 && level==7) {
		return Frolov_5_7_basis (x,w,npoints);
	} else 	if (dim==5 && level==8) {
		return Frolov_5_8_basis (x,w,npoints);
	} else 	if (dim==5 && level==9) {
		return Frolov_5_9_basis (x,w,npoints);
	} else 	if (dim==5 && level==10) {
		return Frolov_5_10_basis (x,w,npoints);
	} else 	if (dim==5 && level==11) {
		return Frolov_5_11_basis (x,w,npoints);
	} else 	if (dim==5 && level==12) {
		return Frolov_5_12_basis (x,w,npoints);
	} else 	if (dim==5 && level==13) {
		return Frolov_5_13_basis (x,w,npoints);
	} else 	if (dim==5 && level==14) {
		return Frolov_5_14_basis (x,w,npoints);
	} else 	if (dim==5 && level==15) {
		return Frolov_5_15_basis (x,w,npoints);
	} else 	if (dim==5 && level==16) {
		return Frolov_5_16_basis (x,w,npoints);
	} else 	if (dim==6 && level==1) {
		return Frolov_6_1_basis (x,w,npoints);
	} else 	if (dim==6 && level==2) {
		return Frolov_6_2_basis (x,w,npoints);
	} else 	if (dim==6 && level==3) {
		return Frolov_6_3_basis (x,w,npoints);
	} else 	if (dim==6 && level==4) {
		return Frolov_6_4_basis (x,w,npoints);
	} else 	if (dim==6 && level==5) {
		return Frolov_6_5_basis (x,w,npoints);
	} else 	if (dim==6 && level==6) {
		return Frolov_6_6_basis (x,w,npoints);
	} else 	if (dim==6 && level==7) {
		return Frolov_6_7_basis (x,w,npoints);
	} else 	if (dim==6 && level==8) {
		return Frolov_6_8_basis (x,w,npoints);
	} else 	if (dim==6 && level==9) {
		return Frolov_6_9_basis (x,w,npoints);
	} else 	if (dim==6 && level==10) {
		return Frolov_6_10_basis (x,w,npoints);
	} else 	if (dim==6 && level==11) {
		return Frolov_6_11_basis (x,w,npoints);
	} else 	if (dim==6 && level==12) {
		return Frolov_6_12_basis (x,w,npoints);
	} else 	if (dim==6 && level==13) {
		return Frolov_6_13_basis (x,w,npoints);
	} else 	if (dim==6 && level==14) {
		return Frolov_6_14_basis (x,w,npoints);
	} else 	if (dim==6 && level==15) {
		return Frolov_6_15_basis (x,w,npoints);
	} else 	if (dim==6 && level==16) {
		return Frolov_6_16_basis (x,w,npoints);
	}
	return 3;
}

