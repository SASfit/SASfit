#include <stdio.h>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

#define MAXPAR 40

typedef struct
{
	double             p[MAXPAR];      //!< Parameter of a function. 
//	sasfit_kernel_type kernelSelector; //!< Selects the kernel function to use, when used with gsl functions.
//	sasfit_kernel_type kernelSubSelector; //!< ??
//	char               errStr[STRLEN]; //!< An error message.
//	int                errLen;         //!< Length of the error message.
//	bool               errStatus;      //!< True, if an error occured. False otherwise.
} sasfit_param;

#define R_SPH		param->p[0]
#define T_SH		param->p[1]
#define THETA_1		param->p[2]
#define THETA_2		param->p[3]
#define ETA_SH		param->p[4]
#define ETA_D		param->p[5]
#define R_D1		param->p[6]
#define R_D2		param->p[7]
#define L			param->p[8]
#define Q			param->p[MAXPAR-1]
#define PSI			param->p[MAXPAR-2]
#define BIG_THETA_1	asin((R_SPH*sin(THETA_1))/(R_SPH+T_SH))
#define BIG_THETA_2	asin((R_SPH*sin(THETA_2))/(R_SPH+T_SH))
#define DELTA_1		( BIG_THETA_1 - asin( ((R_SPH+T_SH)*sin(2.0*M_PI - BIG_THETA_1))/R_D1 ) )
#define DELTA_2		( BIG_THETA_2 - asin( ((R_SPH+T_SH)*sin(2.0*M_PI - BIG_THETA_2))/R_D2 ) )
#define D_1			( sqrt( R_D1*R_D1 + (R_SPH+T_SH) - 2.0*R_D1*(R_SPH+T_SH)*cos(DELTA_1) ) )
#define D_2			( sqrt( R_D2*R_D2 + (R_SPH+T_SH) - 2.0*R_D2*(R_SPH+T_SH)*cos(DELTA_2) ) )


int main ()
{
     double q;
     q = 0.1;
     printf("V^2: %f\n",gsl_sf_bessel_J1(q));
	 
}

/* Note functions with ** in the description need to be integrated over z */

// (1a) Flat-capped sphere of radius R_SPH (r in formulae): real part  **

void inner_sphere_real ( double z, sasfit_param *param ) {

	double temp = 2.0*M_PI*ETA_SH*(R_SPH*R_SPH - z*z)*gsl_sf_bessel_J1(Q*sqrt(R_SPH*R_SPH - z*z)*sin(PSI))/(Q*sqrt(R_SPH*R_SPH-z*z)*sin(PSI));

	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
}
	

// (1b) Flat-capped sphere of radius R_SPH: imaginary part **

void inner_sphere_imag ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_SH*(R_SPH*R_SPH - z*z)*gsl_sf_bessel_J1(Q*sqrt(R_SPH*R_SPH - z*z)*sin(PSI))/(Q*sqrt(R_SPH*R_SPH-z*z)*sin(PSI));
	
	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (2a) Flat-capped sphere of radius R_SPH + T_SH (r+tsh in formulae): real part **

void outer_sphere_real ( double z, sasfit_param *param ) {

	double radius = R_SPH + T_SH;

	double temp = 2.0*M_PI*ETA_SH*(radius*radius - z*z)*gsl_sf_bessel_J1(Q*sqrt(radius*radius-z*z)*sin(PSI))/(Q*sqrt(radius*radius-z*z)*sin(PSI));

	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
}

// (2b) Flat-capped sphere of radius R_SPH + T_SH (r+tsh in formulae): imaginary part **

void outer_sphere_imag ( double z, sasfit_param *param ) {

	double radius = R_SPH + T_SH;
	
	double temp = 2.0*M_PI*ETA_SH*(radius*radius - z*z)*gsl_sf_bessel_J1(Q*sqrt(radius*radius-z*z)*sin(PSI))/(Q*sqrt(radius*radius-z*z)*sin(PSI));

	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (3a) Cylindrical cap (right): real part

void cap_right_real ( double q, sasfit_param *param ) {

	Q = q;

	double a = Q*R_SPH*cos(THETA_1)*cos(PSI);

	double b = Q*(R_SPH + T_SH)*cos(BIG_THETA_1)*cos(PSI);

	double result = (-1.0)*ETA_SH*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*sin(THETA_1))*(sin(PSI))*(sin(a) - sin(b)) / (Q*Q*sin(2.0*PSI));

	return;
}
	
// (3b) Cylindrical cap (right): imaginary part

void cap_right_imag ( double q, sasfit_param *param ) {

	Q = q;

	double a = Q*R_SPH*cos(THETA_1)*cos(PSI);

	double b = Q*(R_SPH + T_SH)*cos(BIG_THETA_1)*cos(PSI);

	double result = ETA_SH*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*sin(THETA_1))*(sin(PSI))*(cos(a) - cos(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
	
}

// (4a) Cylindrical cap (left): real part

void cap_left_real ( double q, sasfit_param *param ) {

	Q = q;
	
	double a = (-1.0)*Q*(R_SPH+T_SH)*cos(BIG_THETA_2)*cos(PSI);
	
	double b = -(1.0)*Q*R_SPH*cos(THETA_2)*cos(PSI);
	
	double result = (-1.0)*ETA_SH*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*sin(THETA_2))*(sin(PSI))*(sin(a) - sin(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (4b) Cylindrical cap (left): imaginary part

void cap_left_imag ( double q, sasfit_param *param ) {

	Q = q;

	double a = (-1.0)*Q*(R_SPH+T_SH)*cos(BIG_THETA_2)*cos(PSI);
	
	double b = -(1.0)*Q*R_SPH*cos(THETA_2)*cos(PSI);
	
	double result = ETA_SH*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*sin(THETA_2))*(sin(PSI))*(cos(a) - cos(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (5a) Domain cylindrical cap (right): real part

void dom_cap_right_real ( double q, sasfit_param *param ) {

	Q = q;

	double a = Q*((R_SPH+T_SH)*cos(BIG_THETA_1) - L)*cos(PSI);
	
	double b = Q*(R_SPH+T_SH)*cos(BIG_THETA_1)*cos(PSI);
	
	double result = (-1.0)*ETA_D*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*cos(THETA_1)*sin(PSI))*(sin(a) - sin(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (5b) Domain cylindrical cap (right): imaginary part

void dom_cap_right_imag ( double q, sasfit_param *param ) {

	Q = q;
	
	double a = Q*((R_SPH+T_SH)*cos(BIG_THETA_1) - L)*cos(PSI);
	
	double b = Q*(R_SPH+T_SH)*cos(BIG_THETA_1)*cos(PSI);
	
	double result = ETA_D*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*cos(THETA_1)*sin(PSI))*(cos(a) - cos(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (6a) Domain cylindrical cap (left): real part

void dom_cap_left_real ( double q, sasfit_param *param ) {

	Q = q;

	double a = (-1.0)*Q*(R_SPH*T_SH)*cos(BIG_THETA_2)*cos(PSI);
	
	double b = (-1.0)*Q*((R_SPH+T_SH)*cos(BIG_THETA_2)+L)*cos(PSI);
	
	double result = (-1.0)*ETA_D*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*cos(THETA_2)*sin(PSI))*(sin(a) - sin(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (6b) Domain cylindrical cap (left): imaginary part

void dom_cap_left_imag ( double q, sasfit_param *param ) {

	Q = q;

	double a = (-1.0)*Q*(R_SPH*T_SH)*cos(BIG_THETA_2)*cos(PSI);
	
	double b = (-1.0)*Q*((R_SPH+T_SH)*cos(BIG_THETA_2)+L)*cos(PSI);
	
	double result = ETA_D*4.0*M_PI*gsl_sf_bessel_J1(Q*R_SPH*cos(THETA_2)*sin(PSI))*(cos(a) - cos(b)) / (Q*Q*sin(2.0*PSI));
	
	return;
}

// (7a) Domain outer curved cap (right): real part **

void outer_curve_right_real ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D1*R_D1 - (z+D_1)*(z+D_1))*gsl_sf_bessel_J1(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI))/(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI));
	
	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (7b) Domain outer curved cap (right): imaginary part **

void outer_curve_right_imag ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D1*R_D1 - (z+D_1)*(z+D_1))*gsl_sf_bessel_J1(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI))/(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI));
	
	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
}

// (8a) Domain outer curved cap (left): real part **

void outer_curve_left_real ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D2*R_D2 - (z+D_2)*(z+D_2))*gsl_sf_bessel_J1(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI))/(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI));
	
	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
}

// (8b) Domain outer curved cap (left): imaginary part **

void outer_curve_left_imag ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D2*R_D2 - (z+D_2)*(z+D_2))*gsl_sf_bessel_J1(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI))/(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI));
	
	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
}

// (9a) Domain inner curved cap (right): real part **

void inner_curve_right_real ( double z, sasfit_param *param ) {
	
	double temp =  2.0*M_PI*ETA_D*(R_D1*R_D1 - (z+D_1)*(z+D_1))*gsl_sf_bessel_J1(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI))/(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI));
	
	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (9b) Domain inner curved cap (right): imaginary part

void inner_curve_right_imag ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D1*R_D1 - (z+D_1)*(z+D_1))*gsl_sf_bessel_J1(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI))/(Q*sqrt(R_D1*R_D1 - (z+D_1)*(z+D_1))*sin(PSI));
	
	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (10a) Domain inner curved cap (left): real part

void inner_curve_left_real ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D2*R_D2 - (z+D_2)*(z+D_2))*gsl_sf_bessel_J1(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI))/(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI));
	
	double result = cos(Q*z*cos(PSI))*temp;
	
	return;
	
}

// (10b) Domain inner curved cap (right): imaginary part

void inner_curve_left_imag ( double z, sasfit_param *param ) {

	double temp =  2.0*M_PI*ETA_D*(R_D2*R_D2 - (z+D_2)*(z+D_2))*gsl_sf_bessel_J1(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI))/(Q*sqrt(R_D2*R_D2 - (z+D_2)*(z+D_2))*sin(PSI));
	
	double result = sin(Q*z*cos(PSI))*temp;
	
	return;
	
}
