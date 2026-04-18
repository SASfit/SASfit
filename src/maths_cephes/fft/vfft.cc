// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		Verify the Fast Fourier Transform Package
 *
 * $Id: vfft.cc,v 1.5 1998/12/22 20:50:44 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "fft.h"
#include <iostream.h>
#include <float.h>

/*
 *------------------------------------------------------------------------
 *		     Timing the program execution
 */

#include <time.h>

static clock_t clock_acc;

static void start_timing(void)
{
  clock_acc = clock();
}

static void print_timing(const char * header)
{
  register clock_t old_tick = clock_acc;
  register float timing = (clock_acc=clock(),clock_acc - old_tick )
  			  /(float)CLOCKS_PER_SEC; 	// In secs  
  printf("\nIt took %.2f sec to perform %s\n",timing,header);
}

/*
 *-----------------------------------------------------------------------
 */

				// Simplified printing a vector
static void print_seq(const char * header, const Vector& v)		
{
  LAStreamIn vs(v);
  printf("\n%s\t",header);
  while( !vs.eof() )
    printf("%7.4f ",vs.get());
  printf("\n");
}

/*
 *-----------------------------------------------------------------------
 *	      Check FFT of the Arithmetical Progression sequence
 *				x[j] = j
 * The analytical transform is
 *	SUM{ j*W^(kj) } = N/(W^k - 1), k > 0,
 *		          N*(N-1)/2,   k = 0
 */

static void test_ap_series(const int N)
{
  cout << "\n\nVerify the computed FFT of the AP series x[j]=j\n";
  cout << "j = 0.." << N-1 << endl;

  Vector xre(0,N-1);
  Vector xim = zero(xre);

  for(register int j=0; j<N; j++)
    xre(j) = j;


  Vector xfe_re(xre), xfe_im(xre), xfe_abs(xre); // Exact transform
  xfe_re(0) = xfe_abs(0) = N*(N-1)/2;
  xfe_im(0) = 0;
  for(register int k=1; k<N; k++)
  {
    Complex arg(0,-2*M_PI/N * k);
    Complex t = ((double)N) / ( exp(arg) - 1.0);
    xfe_re(k)  = real(t);
    xfe_im(k)  = imag(t);
    xfe_abs(k) = abs(t);
  }

  FFT fft(N);
  Vector xf_re(xre), xf_im(xre), xf_abs(xre);

  cout << "\nPerforming Complex FFT of AP series (IM part being set to 0)"
       << endl;
  start_timing();
  fft.input(xre,xim);
  fft.real(xf_re);
  fft.imag(xf_im);
  print_timing("Complex Fourier transform");
  fft.abs(xf_abs);

  cout << "Verifying the Re part of the transform ..." << endl;
  verify_matrix_identity(xfe_re,xf_re);
  cout << "Verifying the Im part of the transform ..." << endl;
  verify_matrix_identity(xfe_im,xf_im);
  cout << "Verifying the power spectrum ..." << endl;
  verify_matrix_identity(xfe_abs,xf_abs);

  Vector xfr_re(xre), xfr_im(xre), xfr_abs(xre);
  cout << "\nPerforming FFT of a REAL AP sequence" << endl;
  start_timing();
  fft.input(xre);
  fft.real(xfr_re);
  fft.imag(xfr_im);
  print_timing("\"Real\" Fourier transform");
  fft.abs(xfr_abs);

  cout << "Check out that \"Real\" and Complex FFT give identical results"
       << endl;
  verify_matrix_identity(xfr_re,xf_re);
  verify_matrix_identity(xfr_im,xf_im);
  verify_matrix_identity(xfr_abs,xf_abs);

  cout << "\nDone\n";
}

/*
 *-----------------------------------------------------------------------
 *	      Check out the orthogonality of FFT's basis functions
 *			
 * x[j] = W^(-l*j)
 * SUM{ x[j] * W^(kj) } = 0,   k <> l
 *		          N,   k = l
 */

static void test_orth(const int N, const int l)
{
  cout << "\n\nVerify the computed FFT for x[j] = W^(-l*j)\n";
  cout << "j = 0.." << N-1 << ", l=" << l << endl;

  Vector xre(0,N-1);
  Vector xim(xre);

  register int j;
  for(j=0; j<N; j++)
  {
    Complex arg(0, 2*M_PI/N * l * j);
    Complex t = exp(arg);
    xre(j) = real(t), xim(j) = imag(t);
  }

  Vector xfe_re(xre), xfe_im(xre);		// Exact transform
  xfe_re.clear(); xfe_im.clear(); xfe_re(l) = N;

  FFT fft(N);
  Vector xf_re(xre), xf_im(xre), xf_abs(xre);

  cout << "\nPerforming Complex FFT" << endl;
  start_timing();
  fft.input(xre,xim);
  fft.real(xf_re);
  fft.imag(xf_im);
  print_timing("Complex Fourier transform");
  fft.abs(xf_abs);

  cout << "Verifying the Re part of the transform ..." << endl;
  verify_matrix_identity(xfe_re,xf_re);
  cout << "Verifying the Im part of the transform ..." << endl;
  verify_matrix_identity(xfe_im,xf_im);
  cout << "Verifying the power spectrum ..." << endl;
  verify_matrix_identity(xfe_re,xf_abs);

  cout << "\nDone\n";
}


/*
 *-----------------------------------------------------------------------
 *      Check FFT of the truncated Arithmetical Progression sequence
 *			     x[j] = j, j=0..N/2
 * Analytical transform is
 *	SUM{ j*W^(kj) } = N/2 (W^k - 1), k > 0 and even
 *			  2*W^k/(W^k - 1)^2 - N/2 * 1/(W^k - 1), k being odd
 *		          N/2 * (N/2-1)/2,   k = 0
 */

static void test_ap_series_pad0(const int N)
{
  cout << "\n\nVerify the computed FFT of the truncated AP sequence x[j]=j\n";
  cout << "j = 0.." << N/2-1 << ", with N=" << N << endl;

  Vector xre(0,N/2-1);
  Vector xim(xre);

  for(register int j=0; j<N/2; j++)
    xre(j) = j;
  xim = 0;


  Vector xfe_re(0,N-1), xfe_im(0,N-1), xfe_abs(0,N-1); // Exact transform
  xfe_re(0) = xfe_abs(0) = N/2 * (N/2 - 1)/2;
  xfe_im(0) = 0;
  for(register int k=1; k<N; k++)
  {
    const Complex wk = exp(Complex(0,-2*M_PI/N * k));
    Complex t = 1.0/ ( wk - 1.0 );
    if( k & 1 )
      t = 2.0*wk*t*t - ((double)(N/2)) * t;			// for k odd
    else
      t *= N/2;					// for k even
    xfe_re(k)  = real(t);
    xfe_im(k)  = imag(t);
    xfe_abs(k) = abs(t);
  }

  FFT fft(N);
  Vector xf_re(0,N-1), xf_im(0,N-1), xf_abs(0,N-1);

  cout << "\nPerforming Complex FFT (with IM part being set to 0)"
       << endl;
  start_timing();
  fft.input_pad0(xre,xim);
  fft.real(xf_re);
  fft.imag(xf_im);
  print_timing("Complex Fourier transform");
  fft.abs(xf_abs);

  if( N <= 16 )
  {
    print_seq("Source Vector         ",xre);
    print_seq("Computed cos transform",xf_re);
    print_seq("Computed sin transform",xf_im);
  }

  cout << "Verifying the Re part of the transform ..." << endl;
  verify_matrix_identity(xfe_re,xf_re);
  cout << "Verifying the Im part of the transform ..." << endl;
  verify_matrix_identity(xfe_im,xf_im);
  cout << "Verifying the power spectrum ..." << endl;
  verify_matrix_identity(xfe_abs,xf_abs);

  Vector xfr_re(xf_re), xfr_im(xf_re), xfr_abs(xf_re);
  cout << "\nPerforming FFT of a REAL AP sequence" << endl;
  start_timing();
  fft.input_pad0(xre);
  fft.real(xfr_re);
  fft.imag(xfr_im);
  print_timing("\"Real\" Fourier transform");
  fft.abs(xfr_abs);

  cout << "Check out that \"Real\" and Complex FFT give identical results"
       << endl;
  verify_matrix_identity(xfr_re,xf_re);
  verify_matrix_identity(xfr_im,xf_im);
  verify_matrix_identity(xfr_abs,xf_abs);

  Vector xfh_re(xre), xfh_im(xre), xfh_abs(xre);
  cout << "Check out the functions returning the half of the transform"
       << endl;
  fft.real_half(xfh_re);
  fft.imag_half(xfh_im);
  fft.abs_half(xfh_abs);
  xfr_re.resize_to(xfh_re);
  xfr_im.resize_to(xfh_re);
  xfr_abs.resize_to(xfh_re);
  verify_matrix_identity(xfr_re,xfh_re);
  verify_matrix_identity(xfr_im,xfh_im);
  verify_matrix_identity(xfr_abs,xfh_abs);

  cout << "\nDone\n";
}


/*
 *-----------------------------------------------------------------------
 *		     Verify the sin/cos Fourier transform
 *
 *	r*exp( -r/a )	<=== sin-transform ===> 2a^3 k / (1 + (ak)^2)^2
 *	r*exp( -r/a )	<=== cos-transform ===> a^2 (1 - (ak)^2)/(1 + (ak)^2)^2
 */

static void test_lorentzian(void)
{
  const double R = 20;			// Cutoff distance
  const int n = 512;			// No. of grids
  const double a = 4;			// Constant a, see above
  const double dr = R/n, 		// Grid meshes
	       dk = M_PI/R;	

  cout << "\n\nVerify the sin/cos transform for the following example\n";
  cout << "\tr*exp( -r/a )\t<=== sin-transform ===>\t2a^3 k/(1 + (ak)^2)^2\n";
  cout << "\tr*exp( -r/a )\t<=== cos-transform ===>\t"
          "a^2 (1-(ak)^2)/(1 + (ak)^2)^2" << endl;

  printf("\nParameter a is %2f",a);
  printf("\nNo. of grids   %d",n);
  printf("\nGrid mesh in the r-space dr = %.3f",dr);
  printf("\nGrid mesh in the k-space dk = %.3f\n",dk);

  FFT fft(2*n,dr);

  cout << "\nCheck out the inquires to FFT package about N, dr, dk, cutoffs"
       << endl;
  assert( fft.q_N() == 2*n );
  assert( fft.q_dr() == dr );
  assert( fft.q_dk() == dk );
  assert( fft.q_r_cutoff() == R );
  assert( fft.q_k_cutoff() == dk*n );


  Vector xr(0,n-1);			// Tabulate the source function
  register int j;
  for(j=0; j<n; j++)
  {
    double r = j*dr;
    xr(j) = r * exp( -r/a );
  }

  Vector xs(xr), xc(xr);
  start_timing();
  fft.sin_transform(xs,xr);
  print_timing("Sine transform");
  start_timing();
  fft.cos_transform(xc,xr);
  print_timing("Cosine transform");

  Vector xs_ex(xs), xc_ex(xc);		// Compute exact transforms
  for(j=0; j<n; j++)
  {
    double k = j * dk;
    Complex t = 1.0/Complex(1/a,-k);
    t = t*t;
    xc_ex(j)  = real(t);
    xs_ex(j)  = imag(t);
  }

  compare(xs,xs_ex,"Computed and Exact sin-transform");
  compare(xc,xc_ex,"Computed and Exact cos-transform");

  Vector xt(xc); xt = xc; xt -= xc(xc.q_upb());
  compare(xt,xc_ex,"Computed cos-transform with DC component removed, and "
	  "exact result");

  Vector xs_inv(xr), xc_inv(xr);	// Compute Inverse transforms
  start_timing();
  fft.sin_inv_transform(xs_inv,xs);
  print_timing("Inverse sine transform");
  start_timing();
  fft.cos_inv_transform(xc_inv,xc);
  print_timing("Inverse cosine transform");

  compare(xs_inv,xr,"Computed inverse sin-transform vs the original function");
  compare(xc_inv,xr,"Computed inverse cos-transform vs the original function");

  xt = xc_inv; xt -= xc_inv(xc_inv.q_upb());
  compare(xt,xr,"Computed inverse cos-transform with DC component removed,\n"
	  "and the original function");

  cout << "\nDone\n";
}

/*
 *-----------------------------------------------------------------------
 *		     Verify the cos Fourier transform
 *
 *	exp( -r^2/4a )	<=== cos-transform ===> sqrt(a*pi) exp(-a*k^2)
 */

static void test_gaussian(void)
{
  const double R = 20;			// Cutoff distance
  const int n = 512;			// No. of grids
  const double a = 4;			// Constant a, see above
  const double dr = R/n, 		// Grid meshes
	       dk = M_PI/R;	

  cout << "\n\nVerify the sin/cos transform for the following example\n";
  cout << "\texp( -r^2/4a )\t<=== cos-transform ===> sqrt(a*pi) exp(-a*k^2)\n";

  printf("\nParameter a is %.2f",a);
  printf("\nNo. of grids   %d",n);
  printf("\nGrid mesh in the r-space dr = %.3f",dr);
  printf("\nGrid mesh in the k-space dk = %.3f\n",dk);

  FFT fft(2*n,dr);

  cout << "\nCheck out the inquires to FFT package about N, dr, dk, cutoffs"
       << endl;
  assert( fft.q_N() == 2*n );
  assert( fft.q_dr() == dr );
  assert( fft.q_dk() == dk );
  assert( fft.q_r_cutoff() == R );
  assert( fft.q_k_cutoff() == dk*n );


  Vector xr(0,n-1);			// Tabulate the source function
  register int j;
  for(j=0; j<n; j++)
  {
    double r = j*dr;
    xr(j) = exp(-r*r/4/a );
  }

  Vector xc(xr);
  start_timing();
  fft.cos_transform(xc,xr);
  print_timing("Cosine transform");

  Vector xc_ex(xr);			// Compute exact transforms
  for(j=0; j<n; j++)
  {
    double k = j * dk;
    xc_ex(j)  = sqrt(M_PI*a) * exp( -a*k*k );
  }

  compare(xc,xc_ex,"Computed and Exact cos-transform");
  xc -= xc(xc.q_upb());
  compare(xc,xc_ex,"Computed with DC removed, and Exact cos-transform");

  Vector xc_inv(xr);			// Compute Inverse transforms
  start_timing();
  fft.cos_inv_transform(xc_inv,xc);
  print_timing("Inverse cosine transform");

  compare(xc_inv,xr,"Computed inverse cos-transform vs the original function");
  xc_inv -= xc_inv(xc_inv.q_upb());
  compare(xc_inv,xr,"Computed inverse with DC removed vs the original");

  cout << "\nDone\n";
}

/*
 *-----------------------------------------------------------------------
 *	      Check speed of different kinds of transforms
 */

static void check_speed(const int N, const int ntimes)
{
  cout << "\n\ncheck speed of different kind of transforms\n";
  cout << "j = 0.." << N-1 << " for " << ntimes << " times" << endl;

  Vector xre(0,N-1);
  Vector xim(xre);

  register int j;
  for(j=0; j<N; j++)
    xre(j) = j;
  xim.clear();

  FFT fft(N);
  Vector xf_re(xre), xf_im(xre);

  cout << "\tPerforming Complex FFT " << endl;
  start_timing();
  for(j=0; j<ntimes; j++)
  {
    fft.input(xre,xim);
    fft.real(xf_re);
    fft.imag(xf_im);
  }
  print_timing("Complex Fourier transform");
  						// Make sure the transform was alright
  assert( abs(xf_im(0)) < FLT_EPSILON && abs(xf_re(0) - N*(N-1)/2) < FLT_EPSILON );

  cout << "\tPerforming FFT of a REAL sequence" << endl;
  start_timing();
  for(j=0; j<ntimes; j++)
  {
    fft.input(xre);
    fft.real(xf_re);
    fft.imag(xf_im);
  }
  print_timing("\"Real\" Fourier transform");
  						// Make sure the transform was alright
  assert( abs(xf_im(0)) < FLT_EPSILON && abs(xf_re(0) - N*(N-1)/2) < FLT_EPSILON );

					// Now check padding
  xre.resize_to(0,N/2-1), xim.resize_to(xre);

  cout << "\tPerforming Complex FFT of a half sequence padded by zeros" << endl;
  start_timing();
  for(j=0; j<ntimes; j++)
  {
    fft.input_pad0(xre,xim);
    fft.real(xf_re);
    fft.imag(xf_im);
  }
  print_timing("Complex Fourier transform");

  cout << "\tPerforming FFT of a REAL half-sequence padded by zeros" << endl;
  start_timing();
  for(j=0; j<ntimes; j++)
  {
    fft.input_pad0(xre);
    fft.real(xf_re);
    fft.imag(xf_im);
  }
  print_timing("\"Real\" Fourier transform");

  xf_re.resize_to(xre), xf_im.resize_to(xre);
  cout << "\tPerforming FFT of a REAL padded half-sequence and getting half" << endl;
  start_timing();
  for(j=0; j<ntimes; j++)
  {
    fft.input_pad0(xre);
    fft.real_half(xf_re);
    fft.imag_half(xf_im);
  }
  print_timing("\"Real\" Fourier transform");
  
  cout << "\nDone\n";
}


/*
 *------------------------------------------------------------------------
 *				Root module
 */

main(void)
{
  cout << "\n\n\t\tVerify Fast Fourier Transform Package\n" << endl;

  test_ap_series(8);
  test_ap_series(1024);

  test_orth(1024,1);

  test_ap_series_pad0(16);
  test_ap_series_pad0(1024);

  test_lorentzian();
  test_gaussian();
  
  check_speed(2048,100);
}

