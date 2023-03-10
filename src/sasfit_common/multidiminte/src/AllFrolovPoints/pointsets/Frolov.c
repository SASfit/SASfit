#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <unistd.h>
#include "sasfit.h"

double *Frolov_2_1_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (2*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_1_basic.inc"
	}
	return x;
}

double *Frolov_2_2_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (2*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_2_basic.inc"
	}
	return x;
}

double *Frolov_2_3_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 9;
	*w = 0.125;
	x = ( double * ) malloc (2*9 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_3_basic.inc"
	}
	return x;
}

double *Frolov_2_4_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 15;
	*w = 0.0625;
	x = ( double * ) malloc (2*15 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_4_basic.inc"
	}
	return x;
}

double *Frolov_2_5_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 31;
	*w = 0.03125;
	x = ( double * ) malloc (2*31 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_5_basic.inc"
	}
	return x;
}

double *Frolov_2_6_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 63;
	*w = 0.015625;
	x = ( double * ) malloc (2*63 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_6_basic.inc"
	}
	return x;
}

double *Frolov_2_7_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 127;
	*w = 0.0078125;
	x = ( double * ) malloc (2*127 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_7_basic.inc"
	}
	return x;
}

double *Frolov_2_8_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 257;
	*w = 0.00390625;
	x = ( double * ) malloc (2*257 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_8_basic.inc"
	}
	return x;
}

double *Frolov_2_9_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 511;
	*w = 0.001953125;
	x = ( double * ) malloc (2*511 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_9_basic.inc"
	}
	return x;
}

double *Frolov_2_10_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 1023;
	*w = 0.0009765625;
	x = ( double * ) malloc (2*1023 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_10_basic.inc"
	}
	return x;
}

double *Frolov_2_11_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 2049;
	*w = 0.00048828125;
	x = ( double * ) malloc (2*2049 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_11_basic.inc"
	}
	return x;
}

double *Frolov_2_12_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (2*4093 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_12_basic.inc"
	}
	return x;
}

double *Frolov_2_13_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 8195;
	*w = 0.0001220703125;
	x = ( double * ) malloc (2*8195 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_13_basic.inc"
	}
	return x;
}

double *Frolov_2_14_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 16387;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (2*16387 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_2_14_basic.inc"
	}
	return x;
}

double *Frolov_2_15_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 32771;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (2*32771 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_2_15_basic.inc"
	}
	return x;
}

double *Frolov_2_16_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65533;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (2*65533 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_2_16_basic.inc"
	}
	return x;
}

double *Frolov_3_1_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (3*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_1_basic.inc"
	}
	return x;
}

double *Frolov_3_2_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (3*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_2_basic.inc"
	}
	return x;
}

double *Frolov_3_3_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 9;
	*w = 0.125;
	x = ( double * ) malloc (3*9 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_3_basic.inc"
	}
	return x;
}

double *Frolov_3_4_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 17;
	*w = 0.0625;
	x = ( double * ) malloc (3*17 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_4_basic.inc"
	}
	return x;
}

double *Frolov_3_5_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 31;
	*w = 0.03125;
	x = ( double * ) malloc (3*31 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_5_basic.inc"
	}
	return x;
}

double *Frolov_3_6_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 67;
	*w = 0.015625;
	x = ( double * ) malloc (3*67 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_6_basic.inc"
	}
	return x;
}

double *Frolov_3_7_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 129;
	*w = 0.0078125;
	x = ( double * ) malloc (3*129 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_7_basic.inc"
	}
	return x;
}

double *Frolov_3_8_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 253;
	*w = 0.00390625;
	x = ( double * ) malloc (3*253 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_8_basic.inc"
	}
	return x;
}

double *Frolov_3_9_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 513;
	*w = 0.001953125;
	x = ( double * ) malloc (3*513 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_9_basic.inc"
	}
	return x;
}

double *Frolov_3_10_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 1021;
	*w = 0.0009765625;
	x = ( double * ) malloc (3*1021 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_10_basic.inc"
	}
	return x;
}

double *Frolov_3_11_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 2041;
	*w = 0.00048828125;
	x = ( double * ) malloc (3*2041 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_11_basic.inc"
	}
	return x;
}

double *Frolov_3_12_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (3*4093 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_12_basic.inc"
	}
	return x;
}

double *Frolov_3_13_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 8193;
	*w = 0.0001220703125;
	x = ( double * ) malloc (3*8193 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_13_basic.inc"
	}
	return x;
}

double *Frolov_3_14_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 16387;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (3*16387 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_3_14_basic.inc"
	}
	return x;
}

double *Frolov_3_15_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 32773;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (3*32773 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_3_15_basic.inc"
	}
	return x;
}

double *Frolov_3_16_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65537;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (3*65537 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_3_16_basic.inc"
	}
	return x;
}

double *Frolov_4_1_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (4*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_1_basic.inc"
	}
	return x;
}

double *Frolov_4_2_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 7;
	*w = 0.25;
	x = ( double * ) malloc (4*7 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_2_basic.inc"
	}
	return x;
}

double *Frolov_4_3_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 17;
	*w = 0.125;
	x = ( double * ) malloc (4*17 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_3_basic.inc"
	}
	return x;
}

double *Frolov_4_4_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 19;
	*w = 0.0625;
	x = ( double * ) malloc (4*19 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_4_basic.inc"
	}
	return x;
}

double *Frolov_4_5_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 39;
	*w = 0.03125;
	x = ( double * ) malloc (4*39 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_5_basic.inc"
	}
	return x;
}

double *Frolov_4_6_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65;
	*w = 0.015625;
	x = ( double * ) malloc (4*65 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_6_basic.inc"
	}
	return x;
}

double *Frolov_4_7_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 139;
	*w = 0.0078125;
	x = ( double * ) malloc (4*139 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_7_basic.inc"
	}
	return x;
}

double *Frolov_4_8_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 261;
	*w = 0.00390625;
	x = ( double * ) malloc (4*261 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_8_basic.inc"
	}
	return x;
}

double *Frolov_4_9_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 515;
	*w = 0.001953125;
	x = ( double * ) malloc (4*515 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_9_basic.inc"
	}
	return x;
}

double *Frolov_4_10_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 1023;
	*w = 0.0009765625;
	x = ( double * ) malloc (4*1023 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_10_basic.inc"
	}
	return x;
}

double *Frolov_4_11_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 2055;
	*w = 0.00048828125;
	x = ( double * ) malloc (4*2055 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_11_basic.inc"
	}
	return x;
}

double *Frolov_4_12_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 4103;
	*w = 0.000244140625;
	x = ( double * ) malloc (4*4103 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_12_basic.inc"
	}
	return x;
}

double *Frolov_4_13_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 8197;
	*w = 0.0001220703125;
	x = ( double * ) malloc (4*8197 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_13_basic.inc"
	}
	return x;
}

double *Frolov_4_14_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 16395;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (4*16395 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_4_14_basic.inc"
	}
	return x;
}

double *Frolov_4_15_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 32763;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (4*32763 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_4_15_basic.inc"
	}
	return x;
}

double *Frolov_4_16_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65551;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (4*65551 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_4_16_basic.inc"
	}
	return x;
}

double *Frolov_5_1_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (5*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_1_basic.inc"
	}
	return x;
}

double *Frolov_5_2_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (5*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_2_basic.inc"
	}
	return x;
}

double *Frolov_5_3_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 13;
	*w = 0.125;
	x = ( double * ) malloc (5*13 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_3_basic.inc"
	}
	return x;
}

double *Frolov_5_4_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 15;
	*w = 0.0625;
	x = ( double * ) malloc (5*15 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_4_basic.inc"
	}
	return x;
}

double *Frolov_5_5_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 35;
	*w = 0.03125;
	x = ( double * ) malloc (5*35 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_5_basic.inc"
	}
	return x;
}

double *Frolov_5_6_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65;
	*w = 0.015625;
	x = ( double * ) malloc (5*65 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_6_basic.inc"
	}
	return x;
}

double *Frolov_5_7_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 137;
	*w = 0.0078125;
	x = ( double * ) malloc (5*137 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_7_basic.inc"
	}
	return x;
}

double *Frolov_5_8_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 267;
	*w = 0.00390625;
	x = ( double * ) malloc (5*267 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_8_basic.inc"
	}
	return x;
}

double *Frolov_5_9_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 509;
	*w = 0.001953125;
	x = ( double * ) malloc (5*509 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_9_basic.inc"
	}
	return x;
}

double *Frolov_5_10_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 1021;
	*w = 0.0009765625;
	x = ( double * ) malloc (5*1021 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_10_basic.inc"
	}
	return x;
}

double *Frolov_5_11_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 2051;
	*w = 0.00048828125;
	x = ( double * ) malloc (5*2051 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_11_basic.inc"
	}
	return x;
}

double *Frolov_5_12_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 4093;
	*w = 0.000244140625;
	x = ( double * ) malloc (5*4093 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_12_basic.inc"
	}
	return x;
}

double *Frolov_5_13_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 8185;
	*w = 0.0001220703125;
	x = ( double * ) malloc (5*8185 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_13_basic.inc"
	}
	return x;
}

double *Frolov_5_14_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 16359;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (5*16359 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_5_14_basic.inc"
	}
	return x;
}

double *Frolov_5_15_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 32761;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (5*32761 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_5_15_basic.inc"
	}
	return x;
}

double *Frolov_5_16_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65533;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (5*65533 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_5_16_basic.inc"
	}
	return x;
}

double *Frolov_6_1_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.5;
	x = ( double * ) malloc (6*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_1_basic.inc"
	}
	return x;
}

double *Frolov_6_2_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 3;
	*w = 0.25;
	x = ( double * ) malloc (6*3 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_2_basic.inc"
	}
	return x;
}

double *Frolov_6_3_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 17;
	*w = 0.125;
	x = ( double * ) malloc (6*17 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_3_basic.inc"
	}
	return x;
}

double *Frolov_6_4_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 21;
	*w = 0.0625;
	x = ( double * ) malloc (6*21 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_4_basic.inc"
	}
	return x;
}

double *Frolov_6_5_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 27;
	*w = 0.03125;
	x = ( double * ) malloc (6*27 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_5_basic.inc"
	}
	return x;
}

double *Frolov_6_6_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 57;
	*w = 0.015625;
	x = ( double * ) malloc (6*57 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_6_basic.inc"
	}
	return x;
}

double *Frolov_6_7_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 149;
	*w = 0.0078125;
	x = ( double * ) malloc (6*149 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_7_basic.inc"
	}
	return x;
}

double *Frolov_6_8_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 275;
	*w = 0.00390625;
	x = ( double * ) malloc (6*275 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_8_basic.inc"
	}
	return x;
}

double *Frolov_6_9_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 517;
	*w = 0.001953125;
	x = ( double * ) malloc (6*517 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_9_basic.inc"
	}
	return x;
}

double *Frolov_6_10_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 1005;
	*w = 0.0009765625;
	x = ( double * ) malloc (6*1005 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_10_basic.inc"
	}
	return x;
}

double *Frolov_6_11_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 2051;
	*w = 0.00048828125;
	x = ( double * ) malloc (6*2051 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_11_basic.inc"
	}
	return x;
}

double *Frolov_6_12_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 4087;
	*w = 0.000244140625;
	x = ( double * ) malloc (6*4087 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_12_basic.inc"
	}
	return x;
}

double *Frolov_6_13_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 8207;
	*w = 0.0001220703125;
	x = ( double * ) malloc (6*8207 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_13_basic.inc"
	}
	return x;
}

double *Frolov_6_14_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 16401;
	*w = 6.103515625e-05;
	x = ( double * ) malloc (6*16401 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
        // #include "Frolov_6_14_basic.inc"
	}
	return x;
}

double *Frolov_6_15_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 32765;
	*w = 3.0517578125e-05;
	x = ( double * ) malloc (6*32765 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_6_15_basic.inc"
	}
	return x;
}

double *Frolov_6_16_basic ( double *w, int *npoints, int *status) {
    double *x;
	*npoints = 65513;
	*w = 1.52587890625e-05;
	x = ( double * ) malloc (6*65513 * sizeof (double) );
	if (x==NULL) {
        *status = 1;
        return NULL;
	} else {
	    *status = 0;
//        // #include "Frolov_6_16_basic.inc"
	}
	return x;
}

double *Frolov(int dim, int level, double *w, int *npoints, int *status) {
    char path[256];
    char filename[256];
    char line[1024];
    const char *res;
	gzFile gzfp;
	int i, j, ndim;
	double val, *data;
	char *poi = NULL;
	Tcl_Interp *interp;
	int nc;

	*status = 0;
	if (dim<2||dim>6||level<1||level>18) {
        *status = 2;
        return NULL;
	}
//	res = getcwd(path, sizeof(path));
	interp = sasfit_env_get_interp();
	res = Tcl_GetVar(interp,"::sasfit_basedir",TCL_GLOBAL_ONLY);
    sasfit_out("The current working directory is: %s\n", res);
    if (res==NULL) {
        *status = 3;
        return NULL;
    }
    sprintf(filename, "%s/plugins/FrolovPoints/Frolov_%d_%d_basic.dat.gz",res, dim, level);
    sasfit_out("The current filename is: %s\n", filename);
    gzfp = gzopen(filename, "r"); // open the gzipped file
    if (!gzfp) {
		sasfit_err("Error opening %s\n", filename);
        *status = 4;
		return NULL;
	}
	gzgets( gzfp, line, sizeof(line) ); // read first line, which is a comment
    sasfit_out("first line: %s\n", path);
    gzgets( gzfp, line, sizeof(line) );
    sscanf(line, "%d", &ndim); // getting dim
    gzgets( gzfp, line, sizeof(line) );
    sscanf(line, "%d", npoints); // getting number of nodes
    gzgets( gzfp, line, sizeof(line) );
    sscanf(line, "%lf", w); // getting weight
    sasfit_out("dim:%d, nodes:%d, weight:%lf\n",ndim,*npoints,*w);
	data = (double *)malloc(ndim* (*npoints) * sizeof(double)); // allocate memory for the data
    for (i = 0; i < *npoints; i++) { // read in the data
        gzgets( gzfp, line, sizeof(line) );
        poi = line;
        j=0;
        while(*poi && j<ndim){
            if (1 == sscanf(poi, "%lf%n", &val, &nc))
                poi += nc;
            else
                break;
            data[j+i*ndim] = val;
            j++;
        }
	}
    gzclose(gzfp);
	return data;

	if (dim==2 && level==1) {
		return Frolov_2_1_basic(w,npoints,status);
	} else 	if (dim==2 && level==2) {
		return Frolov_2_2_basic(w,npoints,status);
	} else 	if (dim==2 && level==3) {
		return Frolov_2_3_basic(w,npoints,status);
	} else 	if (dim==2 && level==4) {
		return Frolov_2_4_basic(w,npoints,status);
	} else 	if (dim==2 && level==5) {
		return Frolov_2_5_basic(w,npoints,status);
	} else 	if (dim==2 && level==6) {
		return Frolov_2_6_basic(w,npoints,status);
	} else 	if (dim==2 && level==7) {
		return Frolov_2_7_basic(w,npoints,status);
	} else 	if (dim==2 && level==8) {
		return Frolov_2_8_basic(w,npoints,status);
	} else 	if (dim==2 && level==9) {
		return Frolov_2_9_basic(w,npoints,status);
	} else 	if (dim==2 && level==10) {
		return Frolov_2_10_basic(w,npoints,status);
	} else 	if (dim==2 && level==11) {
		return Frolov_2_11_basic(w,npoints,status);
	} else 	if (dim==2 && level==12) {
		return Frolov_2_12_basic(w,npoints,status);
	} else 	if (dim==2 && level==13) {
		return Frolov_2_13_basic(w,npoints,status);
	} else 	if (dim==2 && level==14) {
		return Frolov_2_14_basic(w,npoints,status);
	} else 	if (dim==2 && level==15) {
		return Frolov_2_15_basic(w,npoints,status);
	} else 	if (dim==2 && level==16) {
		return Frolov_2_16_basic(w,npoints,status);
	} else 	if (dim==3 && level==1) {
		return Frolov_3_1_basic(w,npoints,status);
	} else 	if (dim==3 && level==2) {
		return Frolov_3_2_basic(w,npoints,status);
	} else 	if (dim==3 && level==3) {
		return Frolov_3_3_basic(w,npoints,status);
	} else 	if (dim==3 && level==4) {
		return Frolov_3_4_basic(w,npoints,status);
	} else 	if (dim==3 && level==5) {
		return Frolov_3_5_basic(w,npoints,status);
	} else 	if (dim==3 && level==6) {
		return Frolov_3_6_basic(w,npoints,status);
	} else 	if (dim==3 && level==7) {
		return Frolov_3_7_basic(w,npoints,status);
	} else 	if (dim==3 && level==8) {
		return Frolov_3_8_basic(w,npoints,status);
	} else 	if (dim==3 && level==9) {
		return Frolov_3_9_basic(w,npoints,status);
	} else 	if (dim==3 && level==10) {
		return Frolov_3_10_basic(w,npoints,status);
	} else 	if (dim==3 && level==11) {
		return Frolov_3_11_basic(w,npoints,status);
	} else 	if (dim==3 && level==12) {
		return Frolov_3_12_basic(w,npoints,status);
	} else 	if (dim==3 && level==13) {
		return Frolov_3_13_basic(w,npoints,status);
	} else 	if (dim==3 && level==14) {
		return Frolov_3_14_basic(w,npoints,status);
	} else 	if (dim==3 && level==15) {
		return Frolov_3_15_basic(w,npoints,status);
	} else 	if (dim==3 && level==16) {
		return Frolov_3_16_basic(w,npoints,status);
	} else 	if (dim==4 && level==1) {
		return Frolov_4_1_basic(w,npoints,status);
	} else 	if (dim==4 && level==2) {
		return Frolov_4_2_basic(w,npoints,status);
	} else 	if (dim==4 && level==3) {
		return Frolov_4_3_basic(w,npoints,status);
	} else 	if (dim==4 && level==4) {
		return Frolov_4_4_basic(w,npoints,status);
	} else 	if (dim==4 && level==5) {
		return Frolov_4_5_basic(w,npoints,status);
	} else 	if (dim==4 && level==6) {
		return Frolov_4_6_basic(w,npoints,status);
	} else 	if (dim==4 && level==7) {
		return Frolov_4_7_basic(w,npoints,status);
	} else 	if (dim==4 && level==8) {
		return Frolov_4_8_basic(w,npoints,status);
	} else 	if (dim==4 && level==9) {
		return Frolov_4_9_basic(w,npoints,status);
	} else 	if (dim==4 && level==10) {
		return Frolov_4_10_basic(w,npoints,status);
	} else 	if (dim==4 && level==11) {
		return Frolov_4_11_basic(w,npoints,status);
	} else 	if (dim==4 && level==12) {
		return Frolov_4_12_basic(w,npoints,status);
	} else 	if (dim==4 && level==13) {
		return Frolov_4_13_basic(w,npoints,status);
	} else 	if (dim==4 && level==14) {
		return Frolov_4_14_basic(w,npoints,status);
	} else 	if (dim==4 && level==15) {
		return Frolov_4_15_basic(w,npoints,status);
	} else 	if (dim==4 && level==16) {
		return Frolov_4_16_basic(w,npoints,status);
	} else 	if (dim==5 && level==1) {
		return Frolov_5_1_basic(w,npoints,status);
	} else 	if (dim==5 && level==2) {
		return Frolov_5_2_basic(w,npoints,status);
	} else 	if (dim==5 && level==3) {
		return Frolov_5_3_basic(w,npoints,status);
	} else 	if (dim==5 && level==4) {
		return Frolov_5_4_basic(w,npoints,status);
	} else 	if (dim==5 && level==5) {
		return Frolov_5_5_basic(w,npoints,status);
	} else 	if (dim==5 && level==6) {
		return Frolov_5_6_basic(w,npoints,status);
	} else 	if (dim==5 && level==7) {
		return Frolov_5_7_basic(w,npoints,status);
	} else 	if (dim==5 && level==8) {
		return Frolov_5_8_basic(w,npoints,status);
	} else 	if (dim==5 && level==9) {
		return Frolov_5_9_basic(w,npoints,status);
	} else 	if (dim==5 && level==10) {
		return Frolov_5_10_basic(w,npoints,status);
	} else 	if (dim==5 && level==11) {
		return Frolov_5_11_basic(w,npoints,status);
	} else 	if (dim==5 && level==12) {
		return Frolov_5_12_basic(w,npoints,status);
	} else 	if (dim==5 && level==13) {
		return Frolov_5_13_basic(w,npoints,status);
	} else 	if (dim==5 && level==14) {
		return Frolov_5_14_basic(w,npoints,status);
	} else 	if (dim==5 && level==15) {
		return Frolov_5_15_basic(w,npoints,status);
	} else 	if (dim==5 && level==16) {
		return Frolov_5_16_basic(w,npoints,status);
	} else 	if (dim==6 && level==1) {
		return Frolov_6_1_basic(w,npoints,status);
	} else 	if (dim==6 && level==2) {
		return Frolov_6_2_basic(w,npoints,status);
	} else 	if (dim==6 && level==3) {
		return Frolov_6_3_basic(w,npoints,status);
	} else 	if (dim==6 && level==4) {
		return Frolov_6_4_basic(w,npoints,status);
	} else 	if (dim==6 && level==5) {
		return Frolov_6_5_basic(w,npoints,status);
	} else 	if (dim==6 && level==6) {
		return Frolov_6_6_basic(w,npoints,status);
	} else 	if (dim==6 && level==7) {
		return Frolov_6_7_basic(w,npoints,status);
	} else 	if (dim==6 && level==8) {
		return Frolov_6_8_basic(w,npoints,status);
	} else 	if (dim==6 && level==9) {
		return Frolov_6_9_basic(w,npoints,status);
	} else 	if (dim==6 && level==10) {
		return Frolov_6_10_basic(w,npoints,status);
	} else 	if (dim==6 && level==11) {
		return Frolov_6_11_basic(w,npoints,status);
	} else 	if (dim==6 && level==12) {
		return Frolov_6_12_basic(w,npoints,status);
	} else 	if (dim==6 && level==13) {
		return Frolov_6_13_basic(w,npoints,status);
	} else 	if (dim==6 && level==14) {
		return Frolov_6_14_basic(w,npoints,status);
	} else 	if (dim==6 && level==15) {
		return Frolov_6_15_basic(w,npoints,status);
	} else 	if (dim==6 && level==16) {
		return Frolov_6_16_basic(w,npoints,status);
	}
	*status = 3;
	return NULL;
}

