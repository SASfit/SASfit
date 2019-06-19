/// @file nnls.c
/// @author Vesa Oikonen, Kaisa Sederholm
/// @brief NNLS (non-negative least squares) and required subroutines.
///
///  This routine is based on the text and Fortran code in
///  C.L. Lawson and R.J. Hanson, Solving Least Squares Problems,
///  Prentice-Hall, Englewood Cliffs, New Jersey, 1974.
///
/*****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>
/*****************************************************************************/
/// @cond
/* Local function definitions */
int _lss_h12(
  int mode, int lpivot, int l1, int m, double *u, int iue,
  double *up, double *cm, int ice, int icv, int ncv
);
void _lss_g1(
  double a, double b, double *cterm, double *sterm, double *sig
);
/// @endcond
/*****************************************************************************/

/*****************************************************************************/
/** Algorithm NNLS (Non-negative least-squares)
  
    Given an m by n matrix A, and an m-vector B, computes an n-vector X,
    that solves the least squares problem
        A * X = B   , subject to X>=0
  
    Instead of pointers for working space, NULL can be given to let this
    function to allocate and free the required memory.
  
    @return Function returns 0 if successful, 1, if iteration count exceeded 3*N,
            or 2 in case of invalid problem dimensions or memory allocation error.
 */
int nnls(
  /** On entry, a[ 0... N ][ 0 ... M ] contains the M by N matrix A.
      On exit, a[][] contains the product matrix Q*A, where Q is an m by n
      orthogonal matrix generated implicitly by this function.*/
  double **a,
  /** Matrix dimension m */
  int m,
  /** Matrix dimension n */
  int n,
  /** On entry, b[] must contain the m-vector B.
      On exit, b[] contains Q*B */
  double *b,
  /** On exit, x[] will contain the solution vector */
  double *x,
  /** On exit, rnorm contains the Euclidean norm of the residual vector.
      If NULL is given, no rnorm is calculated */
  double *rnorm,
  /** An n-array of working space, wp[].
      On exit, wp[] will contain the dual solution vector.
      wp[i]=0.0 for all i in set p and wp[i]<=0.0 for all i in set z. 
      Can be NULL, which causes this algorithm to allocate memory for it. */
  double *wp,
  /** An m-array of working space, zz[]. 
      Can be NULL, which causes this algorithm to allocate memory for it. */
  double *zzp,
  /** An n-array of working space, index[]. 
      Can be NULL, which causes this algorithm to allocate memory for it. */
  int *indexp
) {
  int pfeas, ret=0, iz, jz, iz1, iz2, npp1, *index;
  double d1, d2, sm, up, ss, *w, *zz;
  int iter, k, j=0, l, itmax, izmax=0, nsetp, ii, jj=0, ip;
  double temp, wmax, t, alpha, asave, dummy, unorm, ztest, cc;


  /* Check the parameters and data */
  if(m<=0 || n<=0 || a==NULL || b==NULL || x==NULL) return(2);
  /* Allocate memory for working space, if required */
  if(wp!=NULL) w=wp; else w=(double*)calloc(n, sizeof(double));
  if(zzp!=NULL) zz=zzp; else zz=(double*)calloc(m, sizeof(double));
  if(indexp!=NULL) index=indexp; else index=(int*)calloc(n, sizeof(int));
  if(w==NULL || zz==NULL || index==NULL) return(2);

  /* Initialize the arrays INDEX[] and X[] */
  for(k=0; k<n; k++) {x[k]=0.; index[k]=k;}
  iz2=n-1; iz1=0; nsetp=0; npp1=0;

  /* Main loop; quit if all coeffs are already in the solution or */
  /* if M cols of A have been triangulated */
  iter=0; if(n<3) itmax=n*3; else itmax=n*n;
  while(iz1<=iz2 && nsetp<m) {
    /* Compute components of the dual (negative gradient) vector W[] */
    for(iz=iz1; iz<=iz2; iz++) {
      j=index[iz]; sm=0.; for(l=npp1; l<m; l++) sm+=a[j][l]*b[l];
      w[j]=sm;
    }

    while(1) {

      /* Find largest positive W[j] */
      for(wmax=0., iz=iz1; iz<=iz2; iz++) {
        j=index[iz]; if(w[j]>wmax) {wmax=w[j]; izmax=iz;}}

      /* Terminate if wmax<=0.; */
      /* it indicates satisfaction of the Kuhn-Tucker conditions */
      if(wmax<=0.0) break;
      iz=izmax; j=index[iz];

      /* The sign of W[j] is ok for j to be moved to set P. */
      /* Begin the transformation and check new diagonal element to avoid */
      /* near linear dependence. */
      asave=a[j][npp1]; up=0.0;
      _lss_h12(1, npp1, npp1+1, m, &a[j][0], 1, &up, &dummy, 1, 1, 0);
      unorm=0.;
      if(nsetp!=0) for(l=0; l<nsetp; l++) {d1=a[j][l]; unorm+=d1*d1;}
      unorm=sqrt(unorm);
      d2=unorm+fabs(a[j][npp1])*0.01;
      if((d2-unorm)>0.) {
        /* Col j is sufficiently independent. Copy B into ZZ, update ZZ */
        /* and solve for ztest ( = proposed new value for X[j] ) */
        for(l=0; l<m; l++) zz[l]=b[l];
        _lss_h12(2, npp1, npp1+1, m, &a[j][0], 1, &up, zz, 1, 1, 1);
        ztest=zz[npp1]/a[j][npp1];
        /* See if ztest is positive */
        if(ztest>0.) break;
      }

      /* Reject j as a candidate to be moved from set Z to set P. Restore */
      /* A[npp1,j], set W[j]=0., and loop back to test dual coeffs again */
      a[j][npp1]=asave; w[j]=0.;
    } /* while(1) */
    if(wmax<=0.0) break;

    /* Index j=INDEX[iz] has been selected to be moved from set Z to set P. */
    /* Update B and indices, apply householder transformations to cols in */
    /* new set Z, zero sub-diagonal elts in col j, set W[j]=0. */
    for(l=0; l<m; ++l) b[l]=zz[l];
    index[iz]=index[iz1]; index[iz1]=j; iz1++; nsetp=npp1+1; npp1++;
    if(iz1<=iz2) for(jz=iz1; jz<=iz2; jz++) {
      jj=index[jz];
      _lss_h12(2, nsetp-1, npp1, m, &a[j][0], 1, &up, &a[jj][0], 1, m, 1);
    }
    if(nsetp!=m) for(l=npp1; l<m; l++) a[j][l]=0.;
    w[j]=0.;
    
    /* Solve the triangular system; store the solution temporarily in Z[] */
    for(l=0; l<nsetp; l++) {
      ip=nsetp-(l+1);
      if(l!=0) for(ii=0; ii<=ip; ii++) zz[ii]-=a[jj][ii]*zz[ip+1];
      jj=index[ip]; zz[ip]/=a[jj][ip];
    }

    /* Secondary loop begins here */
    while(++iter<itmax) {
      /* See if all new constrained coeffs are feasible; if not, compute alpha */
      for(alpha=2.0, ip=0; ip<nsetp; ip++) {
        l=index[ip];
        if(zz[ip]<=0.) {t=-x[l]/(zz[ip]-x[l]); if(alpha>t) {alpha=t; jj=ip-1;}}
      }

      /* If all new constrained coeffs are feasible then still alpha==2. */
      /* If so, then exit from the secondary loop to main loop */
      if(alpha==2.0) break;

      /* Use alpha (0.<alpha<1.) to interpolate between old X and new ZZ */
      for(ip=0; ip<nsetp; ip++) {l=index[ip]; x[l]+=alpha*(zz[ip]-x[l]);}

      /* Modify A and B and the INDEX arrays to move coefficient i */
      /* from set P to set Z. */
      k=index[jj+1]; pfeas=1;
      do {
        x[k]=0.;
        if(jj!=(nsetp-1)) {
          jj++;
          for(j=jj+1; j<nsetp; j++) {
            ii=index[j]; index[j-1]=ii;
            _lss_g1(a[ii][j-1], a[ii][j], &cc, &ss, &a[ii][j-1]);
            for(a[ii][j]=0., l=0; l<n; l++) if(l!=ii) {
              /* Apply procedure G2 (CC,SS,A(J-1,L),A(J,L)) */
              temp=a[l][j-1];
              a[l][j-1]=cc*temp+ss*a[l][j];
              a[l][j]=-ss*temp+cc*a[l][j];
            }
            /* Apply procedure G2 (CC,SS,B(J-1),B(J)) */
            temp=b[j-1]; b[j-1]=cc*temp+ss*b[j]; b[j]=-ss*temp+cc*b[j];
          }
        }
        npp1=nsetp-1; nsetp--; iz1--; index[iz1]=k;

        /* See if the remaining coeffs in set P are feasible; they should */
        /* be because of the way alpha was determined. If any are */
        /* infeasible it is due to round-off error. Any that are */
        /* non-positive will be set to zero and moved from set P to set Z */
        for(jj=0, pfeas=1; jj<nsetp; jj++) {
          k=index[jj]; if(x[k]<=0.) {pfeas=0; break;}
        }
      } while(pfeas==0);

      /* Copy B[] into zz[], then solve again and loop back */
      for(k=0; k<m; k++) zz[k]=b[k];
      for(l=0; l<nsetp; l++) {
        ip=nsetp-(l+1);
        if(l!=0) for(ii=0; ii<=ip; ii++) zz[ii]-=a[jj][ii]*zz[ip+1];
        jj=index[ip]; zz[ip]/=a[jj][ip];
      }
    } /* end of secondary loop */

    if(iter>=itmax) {ret=1; break;}
    for(ip=0; ip<nsetp; ip++) {k=index[ip]; x[k]=zz[ip];}
  } /* end of main loop */
  /* Compute the norm of the final residual vector */
  sm=0.;
  
  if(rnorm != NULL) {
    if(npp1<m) for(k=npp1; k<m; k++) sm+=(b[k]*b[k]);
    else for(j=0; j<n; j++) w[j]=0.;
    *rnorm=sqrt(sm);
  }	
 
  /* Free working space, if it was allocated here */
  if(wp==NULL) free(w);
  if(zzp==NULL) free(zz);
  if(indexp==NULL) free(index);
  return(ret);
} /* nnls_ */
/*****************************************************************************/

/*****************************************************************************/
/** Algorithm for weighting the problem that is given to nnls-algorithm.
    Square roots of weights are used because in nnls the difference
    w*A-w*b is squared.
\return Algorithm returns zero if successful, 1 if arguments are inappropriate.
*/
int nnlsWght(
  /** NNLS dimension N (nr of parameters) */
  int N,
  /** NNLS dimension M (nr of samples) */
  int M,
  /** NNLS matrix A */
  double **A,
  /** NNLS vector B */
  double *b,
  /** Weights for each sample (array of length M) */
  double *weight
) {
  int n, m;
  double *w;

  /* Check the arguments */
  if(N<1 || M<1 || A==NULL || b==NULL || weight==NULL) return(1);

  /* Allocate memory */
  w=(double*)malloc(M*sizeof(double)); if(w==NULL) return(2);

  /* Check that weights are not zero and get the square roots of them to w[] */
  for(m=0; m<M; m++) {
    if(weight[m]<=1.0e-20) w[m]=0.0;
    else w[m]=sqrt(weight[m]);
  }
 
  /* Multiply rows of matrix A and elements of vector b with weights*/
  for(m=0; m<M; m++) {
    for(n=0; n<N; n++) {
      A[n][m]*=w[m];
    }
    b[m]*=w[m];
  }

  free(w);
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Algorithm for weighting the problem that is given to nnls-algorithm.
    Square roots of weights are used because in nnls the difference
    w*A-w*b is squared.
    Here user must give squared weights; this makes calculation faster, when
    this function needs to be called many times. 
\return Algorithm returns zero if successful, 1 if arguments are inappropriate.
*/
int nnlsWghtSquared(
  /** NNLS dimension N (nr of parameters) */
  int N,
  /** NNLS dimension M (nr of samples) */
  int M,
  /** NNLS matrix A */
  double **A,
  /** NNLS vector B */
  double *b,
  /** Squared weights for each sample (array of length M) */
  double *sweight
) {
  int n, m;

  /* Check the arguments */
  if(N<1 || M<1 || A==NULL || b==NULL || sweight==NULL) return(1);

  /* Multiply rows of matrix A and elements of vector b with weights*/
  for(m=0; m<M; m++) {
    for(n=0; n<N; n++) {
      A[n][m]*=sweight[m];
    }
    b[m]*=sweight[m];
  }

  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/// @cond 
/** Construction and/or application of a single Householder transformation:
    Q = I + U*(U**T)/B
\return Function returns 0 if successful, or >0 in case of erroneous parameters.
 */
int _lss_h12(
  /** mode=1 to construct and apply a Householder transformation, or
      mode=2 to apply a previously constructed transformation */
  int mode,
  /** Index of the pivot element, on pivot vector */
  int lpivot,
  /** Transformation is constructed to zero elements indexed from l1 to M */
  int l1,
  /** Transformation is constructed to zero elements indexed from l1 to M */
  int m,
  /** With mode=1: On entry, u[] must contain the pivot vector.
     On exit, u[] and up contain quantities defining the vector u[] of
     the Householder transformation.
     With mode=2: On entry, u[] and up should contain quantities previously
     computed with mode=1. These will not be modified. */
  double *u,
  /** u_dim1 is the storage increment between elements. */
  int u_dim1,
  /** with mode=1, here is stored an element defining housholder vector
      scalar, on mode=2 it's only used, and is not modified */
  double *up,
  /** On entry, cm[] must contain the matrix (set of vectors) to which the
     Householder transformation is to be applied. On exit, cm[] will contain
     the set of transformed vectors */
  double *cm,
  /** Storage increment between elements of vectors in cm[] */
  int ice,
  /** Storage increment between vectors in cm[] */
  int icv,
  /** Nr of vectors in cm[] to be transformed;
      if ncv<=0, then no operations will be done on cm[] */
  int ncv
) {
  double d1, b, clinv, cl, sm;
  int k, j;
  
  /* Check parameters */
  if(mode!=1 && mode!=2) return(1);
  if(m<1 || u==NULL || u_dim1<1 || cm==NULL) return(1);
  if(lpivot<0 || lpivot>=l1 || l1>m) return(1);

  /* Function Body */
  cl = fabs(u[lpivot*u_dim1]);

  if(mode==2) { /* Apply transformation I+U*(U**T)/B to cm[] */
    if(cl<=0.) return(0);
  } else {   /* Construct the transformation */
  
    /* trying to compensate overflow */
    for(j=l1; j<m; j++) {  // Computing MAX 
      cl = fmax(fabs(u[j*u_dim1]), cl);
    }
    // zero vector?   
    if(cl<=0.) return(0);

    clinv=1.0/cl;
       
    // Computing 2nd power 
    d1=u[lpivot*u_dim1]*clinv; 
    sm=d1*d1;
    for(j=l1; j<m; j++) {
      d1=u[j*u_dim1]*clinv;
      sm+=d1*d1;
    }
    cl*=sqrt(sm);
    // cl = sqrt( (u[pivot]*clinv)^2 + sigma(i=l1..m)( (u[i]*clinv)^2 ) )
    if(u[lpivot*u_dim1] > 0.) cl=-cl;
    *up = u[lpivot*u_dim1] - cl; 
    u[lpivot*u_dim1]=cl;
  }

  // no vectors where to apply? only change pivot vector!	
  b=(*up)*u[lpivot*u_dim1];
  
  /* b must be non-positive here; if b>=0., then return */
  if(b>=0.0) return(0); // was if(b==0) before 2013-06-22
  
  // ok, for all vectors we want to apply
  for(j=0; j<ncv; j++) {
    // take s = c[p,j]*h + sigma(i=l..m){ c[i,j] *v [ i ] }
    sm = cm[ lpivot*ice + j*icv ] * (*up);
    for(k=l1; k<m; k++) sm += cm[ k * ice + j*icv ] * u[ k*u_dim1 ]; 
    if(sm!=0.0) {
      sm *= (1.0/b); // was (1/b) before 2013-06-22
      // cm[lpivot, j] = ..
      cm[ lpivot * ice + j*icv] += sm*(*up);
      // for i = l1...m , set c[i,j] = c[i,j] + s*v[i]
      for(k=l1; k<m; k++) cm[ k*ice + j*icv] += u[k * u_dim1]*sm;
    }
  }
   
  return(0);
} /* _lss_h12 */
/*****************************************************************************/

/*****************************************************************************/
/** Compute orthogonal rotation matrix:
     (C, S) so that (C, S)(A) = (sqrt(A**2+B**2))
     (-S,C)         (-S,C)(B)   (   0          )
    Compute sig = sqrt(A**2+B**2):
      sig is computed last to allow for the possibility that sig may be in
      the same location as A or B.
 */
void _lss_g1(double a, double b, double *cterm, double *sterm, double *sig)
{
  double d1, xr, yr;

  if(fabs(a)>fabs(b)) {
    xr=b/a; d1=xr; yr=hypot(d1, 1.0); d1=1./yr;
    *cterm=copysign(d1, a);
    *sterm=(*cterm)*xr; *sig=fabs(a)*yr;
  } else if(b!=0.) {
    xr=a/b; d1=xr; yr=hypot(d1, 1.0); d1=1./yr;
    *sterm=copysign(d1, b);
    *cterm=(*sterm)*xr; *sig=fabs(b)*yr;
  } else {
    *sig=0.; *cterm=0.; *sterm=1.;
  }
} /* _lss_g1 */
/// @endcond
/*****************************************************************************/

/*****************************************************************************/




/* $Id: nnls.c,v 1.7 2000/11/07 16:29:30 tgkolda Exp $ */
/* $Source: /usr/local/cvsroot/appspack/apps/src/nnls.c,v $ */

/* Distributed with ASYNCHRONOUS PARALLEL PATTERN SEARCH (APPS) */

/* The routines in this file have been translated from Fortran to C by
   f2c. Additional modifications have been made to remove the
   dependencies on the f2c header file and library. The original
   Fortran 77 code accompanies the SIAM Publications printing of
   "Solving Least Squares Problems," by C. Lawson and R. Hanson and is
   freely available at www.netlib.org/lawson-hanson/all. */

/* nnls.F -- translated by f2c (version 19970805).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

/* The next line was removed after the f2c translation */
/* #include "f2c.h" */

/* The next lines were added after the f2c translation. Also swapped
   abs for nnls_abs and max for nnls_max to avoid confusion with some
   compilers. */
#define nnls_max(a,b) ((a) >= (b) ? (a) : (b))
#define nnls_abs(x) ((x) >= 0 ? (x) : -(x))
typedef int integer;
typedef double doublereal;

/* The following subroutine was added after the f2c translation */
double d_sign2(double *a, double *b)
{
  double x;
  x = (*a >= 0 ? *a : - *a);
  return( *b >= 0 ? x : -x);
}

/* Table of constant values */

static integer c__1 = 1;
static integer c__0 = 0;
static integer c__2 = 2;

/*     SUBROUTINE NNLS  (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,MODE) */

/*  Algorithm NNLS: NONNEGATIVE LEAST SQUARES */

/*  The original version of this code was developed by */
/*  Charles L. Lawson and Richard J. Hanson at Jet Propulsion Laboratory */
/*  1973 JUN 15, and published in the book */
/*  "SOLVING LEAST SQUARES PROBLEMS", Prentice-HalL, 1974. */
/*  Revised FEB 1995 to accompany reprinting of the book by SIAM. */

/*     GIVEN AN M BY N MATRIX, A, AND AN M-VECTOR, B,  COMPUTE AN */
/*     N-VECTOR, X, THAT SOLVES THE LEAST SQUARES PROBLEM */

/*                      A * X = B  SUBJECT TO X .GE. 0 */
/*     ------------------------------------------------------------------ */
/*                     Subroutine Arguments */

/*     A(),MDA,M,N     MDA IS THE FIRST DIMENSIONING PARAMETER FOR THE */
/*                     ARRAY, A().   ON ENTRY A() CONTAINS THE M BY N */
/*                     MATRIX, A.           ON EXIT A() CONTAINS */
/*                     THE PRODUCT MATRIX, Q*A , WHERE Q IS AN */
/*                     M BY M ORTHOGONAL MATRIX GENERATED IMPLICITLY BY */
/*                     THIS SUBROUTINE. */
/*     B()     ON ENTRY B() CONTAINS THE M-VECTOR, B.   ON EXIT B() CON- */
/*             TAINS Q*B. */
/*     X()     ON ENTRY X() NEED NOT BE INITIALIZED.  ON EXIT X() WILL */
/*             CONTAIN THE SOLUTION VECTOR. */
/*     RNORM   ON EXIT RNORM CONTAINS THE EUCLIDEAN NORM OF THE */
/*             RESIDUAL VECTOR. */
/*     W()     AN N-ARRAY OF WORKING SPACE.  ON EXIT W() WILL CONTAIN */
/*             THE DUAL SOLUTION VECTOR.   W WILL SATISFY W(I) = 0. */
/*             FOR ALL I IN SET P  AND W(I) .LE. 0. FOR ALL I IN SET Z */
/*     ZZ()     AN M-ARRAY OF WORKING SPACE. */
/*     INDEX()     AN INTEGER WORKING ARRAY OF LENGTH AT LEAST N. */
/*                 ON EXIT THE CONTENTS OF THIS ARRAY DEFINE THE SETS */
/*                 P AND Z AS FOLLOWS.. */

/*                 INDEX(1)   THRU INDEX(NSETP) = SET P. */
/*                 INDEX(IZ1) THRU INDEX(IZ2)   = SET Z. */
/*                 IZ1 = NSETP + 1 = NPP1 */
/*                 IZ2 = N */
/*     MODE    THIS IS A SUCCESS-FAILURE FLAG WITH THE FOLLOWING */
/*             MEANINGS. */
/*             1     THE SOLUTION HAS BEEN COMPUTED SUCCESSFULLY. */
/*             2     THE DIMENSIONS OF THE PROBLEM ARE BAD. */
/*                   EITHER M .LE. 0 OR N .LE. 0. */
/*             3    ITERATION COUNT EXCEEDED.  MORE THAN 3*N ITERATIONS. */

/*     ------------------------------------------------------------------ */
/* Subroutine */ int nnls_(a, mda, m, n, b, x, rnorm, w, zz, index, mode)
doublereal *a;
integer *mda, *m, *n;
doublereal *b, *x, *rnorm, *w, *zz;
integer *index, *mode;
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* The following lines were commented out after the f2c translation */
    /* double sqrt(); */
    /* integer s_wsfe(), do_fio(), e_wsfe(); */

    /* Local variables */
    extern doublereal diff_();
    static integer iter;
    static doublereal temp, wmax;
    static integer i__, j, l;
    static doublereal t, alpha, asave;
    static integer itmax, izmax, nsetp;
    extern /* Subroutine */ int g1_();
    static doublereal dummy, unorm, ztest, cc;
    extern /* Subroutine */ int h12_();
    static integer ii, jj, ip;
    static doublereal sm;
    static integer iz, jz;
    static doublereal up, ss;
    static integer rtnkey, iz1, iz2, npp1;

    /* Fortran I/O blocks */
    /* The following line was commented out after the f2c translation */
    /* static cilist io___22 = { 0, 6, 0, "(/a)", 0 }; */


/*     ------------------------------------------------------------------ 
*/
/*     integer INDEX(N) */
/*     double precision A(MDA,N), B(M), W(N), X(N), ZZ(M) */
/*     ------------------------------------------------------------------ 
*/
    /* Parameter adjustments */
    a_dim1 = *mda;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    --b;
    --x;
    --w;
    --zz;
    --index;

    /* Function Body */
    *mode = 1;
    if (*m <= 0 || *n <= 0) {
	*mode = 2;
	return 0;
    }
    iter = 0;
    itmax = *n * 3;

/*                    INITIALIZE THE ARRAYS INDEX() AND X(). */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = 0.;
/* L20: */
	index[i__] = i__;
    }

    iz2 = *n;
    iz1 = 1;
    nsetp = 0;
    npp1 = 1;
/*                             ******  MAIN LOOP BEGINS HERE  ****** */
L30:
/*                  QUIT IF ALL COEFFICIENTS ARE ALREADY IN THE SOLUTION. 
*/
/*                        OR IF M COLS OF A HAVE BEEN TRIANGULARIZED. */

    if (iz1 > iz2 || nsetp >= *m) {
	goto L350;
    }

/*         COMPUTE COMPONENTS OF THE DUAL (NEGATIVE GRADIENT) VECTOR W(). 
*/

    i__1 = iz2;
    for (iz = iz1; iz <= i__1; ++iz) {
	j = index[iz];
	sm = 0.;
	i__2 = *m;
	for (l = npp1; l <= i__2; ++l) {
/* L40: */
	    sm += a[l + j * a_dim1] * b[l];
	}
	w[j] = sm;
/* L50: */
    }
/*                                   FIND LARGEST POSITIVE W(J). */
L60:
    wmax = 0.;
    i__1 = iz2;
    for (iz = iz1; iz <= i__1; ++iz) {
	j = index[iz];
	if (w[j] > wmax) {
	    wmax = w[j];
	    izmax = iz;
	}
/* L70: */
    }

/*             IF WMAX .LE. 0. GO TO TERMINATION. */
/*             THIS INDICATES SATISFACTION OF THE KUHN-TUCKER CONDITIONS. 
*/

    if (wmax <= 0.) {
	goto L350;
    }
    iz = izmax;
    j = index[iz];

/*     THE SIGN OF W(J) IS OK FOR J TO BE MOVED TO SET P. */
/*     BEGIN THE TRANSFORMATION AND CHECK NEW DIAGONAL ELEMENT TO AVOID */
/*     NEAR LINEAR DEPENDENCE. */

    asave = a[npp1 + j * a_dim1];
    i__1 = npp1 + 1;
    h12_(&c__1, &npp1, &i__1, m, &a[j * a_dim1 + 1], &c__1, &up, &dummy, &
	    c__1, &c__1, &c__0);
    unorm = 0.;
    if (nsetp != 0) {
	i__1 = nsetp;
	for (l = 1; l <= i__1; ++l) {
/* L90: */
/* Computing 2nd power */
	    d__1 = a[l + j * a_dim1];
	    unorm += d__1 * d__1;
	}
    }
    unorm = sqrt(unorm);
    d__2 = unorm + (d__1 = a[npp1 + j * a_dim1], nnls_abs(d__1)) * .01;
    if (diff_(&d__2, &unorm) > 0.) {

/*        COL J IS SUFFICIENTLY INDEPENDENT.  COPY B INTO ZZ, UPDATE Z
Z */
/*        AND SOLVE FOR ZTEST ( = PROPOSED NEW VALUE FOR X(J) ). */

	i__1 = *m;
	for (l = 1; l <= i__1; ++l) {
/* L120: */
	    zz[l] = b[l];
	}
	i__1 = npp1 + 1;
	h12_(&c__2, &npp1, &i__1, m, &a[j * a_dim1 + 1], &c__1, &up, &zz[1], &
		c__1, &c__1, &c__1);
	ztest = zz[npp1] / a[npp1 + j * a_dim1];

/*                                     SEE IF ZTEST IS POSITIVE */

	if (ztest > 0.) {
	    goto L140;
	}
    }

/*     REJECT J AS A CANDIDATE TO BE MOVED FROM SET Z TO SET P. */
/*     RESTORE A(NPP1,J), SET W(J)=0., AND LOOP BACK TO TEST DUAL */
/*     COEFFS AGAIN. */

    a[npp1 + j * a_dim1] = asave;
    w[j] = 0.;
    goto L60;

/*     THE INDEX  J=INDEX(IZ)  HAS BEEN SELECTED TO BE MOVED FROM */
/*     SET Z TO SET P.    UPDATE B,  UPDATE INDICES,  APPLY HOUSEHOLDER */
/*     TRANSFORMATIONS TO COLS IN NEW SET Z,  ZERO SUBDIAGONAL ELTS IN */
/*     COL J,  SET W(J)=0. */

L140:
    i__1 = *m;
    for (l = 1; l <= i__1; ++l) {
/* L150: */
	b[l] = zz[l];
    }

    index[iz] = index[iz1];
    index[iz1] = j;
    ++iz1;
    nsetp = npp1;
    ++npp1;

    if (iz1 <= iz2) {
	i__1 = iz2;
	for (jz = iz1; jz <= i__1; ++jz) {
	    jj = index[jz];
	    h12_(&c__2, &nsetp, &npp1, m, &a[j * a_dim1 + 1], &c__1, &up, &a[
		    jj * a_dim1 + 1], &c__1, mda, &c__1);
/* L160: */
	}
    }

    if (nsetp != *m) {
	i__1 = *m;
	for (l = npp1; l <= i__1; ++l) {
/* L180: */
	    a[l + j * a_dim1] = 0.;
	}
    }

    w[j] = 0.;
/*                                SOLVE THE TRIANGULAR SYSTEM. */
/*                                STORE THE SOLUTION TEMPORARILY IN ZZ(). 
*/
    rtnkey = 1;
    goto L400;
L200:

/*                       ******  SECONDARY LOOP BEGINS HERE ****** */

/*                          ITERATION COUNTER. */

L210:
    ++iter;
    if (iter > itmax) {
	*mode = 3;
	/* The following lines were replaced after the f2c translation */
	/* s_wsfe(&io___22); */
	/* do_fio(&c__1, " NNLS quitting on iteration count.", 34L); */
	/* e_wsfe(); */
	fprintf(stdout, "\n NNLS quitting on iteration count.\n");
	fflush(stdout);
	goto L350;
    }

/*                    SEE IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE. */
/*                                  IF NOT COMPUTE ALPHA. */

    alpha = 2.;
    i__1 = nsetp;
    for (ip = 1; ip <= i__1; ++ip) {
	l = index[ip];
	if (zz[ip] <= 0.) {
	    t = -x[l] / (zz[ip] - x[l]);
	    if (alpha > t) {
		alpha = t;
		jj = ip;
	    }
	}
/* L240: */
    }

/*          IF ALL NEW CONSTRAINED COEFFS ARE FEASIBLE THEN ALPHA WILL */
/*          STILL = 2.    IF SO EXIT FROM SECONDARY LOOP TO MAIN LOOP. */

    if (alpha == 2.) {
	goto L330;
    }

/*          OTHERWISE USE ALPHA WHICH WILL BE BETWEEN 0. AND 1. TO */
/*          INTERPOLATE BETWEEN THE OLD X AND THE NEW ZZ. */

    i__1 = nsetp;
    for (ip = 1; ip <= i__1; ++ip) {
	l = index[ip];
	x[l] += alpha * (zz[ip] - x[l]);
/* L250: */
    }

/*        MODIFY A AND B AND THE INDEX ARRAYS TO MOVE COEFFICIENT I */
/*        FROM SET P TO SET Z. */

    i__ = index[jj];
L260:
    x[i__] = 0.;

    if (jj != nsetp) {
	++jj;
	i__1 = nsetp;
	for (j = jj; j <= i__1; ++j) {
	    ii = index[j];
	    index[j - 1] = ii;
	    g1_(&a[j - 1 + ii * a_dim1], &a[j + ii * a_dim1], &cc, &ss, &a[j 
		    - 1 + ii * a_dim1]);
	    a[j + ii * a_dim1] = 0.;
	    i__2 = *n;
	    for (l = 1; l <= i__2; ++l) {
		if (l != ii) {

/*                 Apply procedure G2 (CC,SS,A(J-1,L),A(J,
L)) */

		    temp = a[j - 1 + l * a_dim1];
		    a[j - 1 + l * a_dim1] = cc * temp + ss * a[j + l * a_dim1]
			    ;
		    a[j + l * a_dim1] = -ss * temp + cc * a[j + l * a_dim1];
		}
/* L270: */
	    }

/*                 Apply procedure G2 (CC,SS,B(J-1),B(J)) */

	    temp = b[j - 1];
	    b[j - 1] = cc * temp + ss * b[j];
	    b[j] = -ss * temp + cc * b[j];
/* L280: */
	}
    }

    npp1 = nsetp;
    --nsetp;
    --iz1;
    index[iz1] = i__;

/*        SEE IF THE REMAINING COEFFS IN SET P ARE FEASIBLE.  THEY SHOULD 
*/
/*        BE BECAUSE OF THE WAY ALPHA WAS DETERMINED. */
/*        IF ANY ARE INFEASIBLE IT IS DUE TO ROUND-OFF ERROR.  ANY */
/*        THAT ARE NONPOSITIVE WILL BE SET TO ZERO */
/*        AND MOVED FROM SET P TO SET Z. */

    i__1 = nsetp;
    for (jj = 1; jj <= i__1; ++jj) {
	i__ = index[jj];
	if (x[i__] <= 0.) {
	    goto L260;
	}
/* L300: */
    }

/*         COPY B( ) INTO ZZ( ).  THEN SOLVE AGAIN AND LOOP BACK. */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L310: */
	zz[i__] = b[i__];
    }
    rtnkey = 2;
    goto L400;
L320:
    goto L210;
/*                      ******  END OF SECONDARY LOOP  ****** */

L330:
    i__1 = nsetp;
    for (ip = 1; ip <= i__1; ++ip) {
	i__ = index[ip];
/* L340: */
	x[i__] = zz[ip];
    }
/*        ALL NEW COEFFS ARE POSITIVE.  LOOP BACK TO BEGINNING. */
    goto L30;

/*                        ******  END OF MAIN LOOP  ****** */

/*                        COME TO HERE FOR TERMINATION. */
/*                     COMPUTE THE NORM OF THE FINAL RESIDUAL VECTOR. */

L350:
    sm = 0.;
    if (npp1 <= *m) {
	i__1 = *m;
	for (i__ = npp1; i__ <= i__1; ++i__) {
/* L360: */
/* Computing 2nd power */
	    d__1 = b[i__];
	    sm += d__1 * d__1;
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* L380: */
	    w[j] = 0.;
	}
    }
    *rnorm = sqrt(sm);
    return 0;

/*     THE FOLLOWING BLOCK OF CODE IS USED AS AN INTERNAL SUBROUTINE */
/*     TO SOLVE THE TRIANGULAR SYSTEM, PUTTING THE SOLUTION IN ZZ(). */

L400:
    i__1 = nsetp;
    for (l = 1; l <= i__1; ++l) {
	ip = nsetp + 1 - l;
	if (l != 1) {
	    i__2 = ip;
	    for (ii = 1; ii <= i__2; ++ii) {
		zz[ii] -= a[ii + jj * a_dim1] * zz[ip + 1];
/* L410: */
	    }
	}
	jj = index[ip];
	zz[ip] /= a[ip + jj * a_dim1];
/* L430: */
    }
    switch ((int)rtnkey) {
	case 1:  goto L200;
	case 2:  goto L320;
    }

    /* The next line was added after the f2c translation to keep
       compilers from complaining about a void return from a non-void
       function. */
    return 0;

} /* nnls_f2c */

/* Subroutine */ int g1_(a, b, cterm, sterm, sig)
doublereal *a, *b, *cterm, *sterm, *sig;
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    /* The following line was commented out after the f2c translation */
    /* double sqrt(), d_sign(); */

    /* Local variables */
    static doublereal xr, yr;


/*     COMPUTE ORTHOGONAL ROTATION MATRIX.. */

/*  The original version of this code was developed by */
/*  Charles L. Lawson and Richard J. Hanson at Jet Propulsion Laboratory 
*/
/*  1973 JUN 12, and published in the book */
/*  "SOLVING LEAST SQUARES PROBLEMS", Prentice-HalL, 1974. */
/*  Revised FEB 1995 to accompany reprinting of the book by SIAM. */

/*     COMPUTE.. MATRIX   (C, S) SO THAT (C, S)(A) = (SQRT(A**2+B**2)) */
/*                        (-S,C)         (-S,C)(B)   (   0          ) */
/*     COMPUTE SIG = SQRT(A**2+B**2) */
/*        SIG IS COMPUTED LAST TO ALLOW FOR THE POSSIBILITY THAT */
/*        SIG MAY BE IN THE SAME LOCATION AS A OR B . */
/*     ------------------------------------------------------------------ 
*/
/*     ------------------------------------------------------------------ 
*/
    if (nnls_abs(*a) > nnls_abs(*b)) {
	xr = *b / *a;
/* Computing 2nd power */
	d__1 = xr;
	yr = sqrt(d__1 * d__1 + 1.);
	d__1 = 1. / yr;
	*cterm = d_sign2(&d__1, a);
	*sterm = *cterm * xr;
	*sig = nnls_abs(*a) * yr;
	return 0;
    }
    if (*b != 0.) {
	xr = *a / *b;
/* Computing 2nd power */
	d__1 = xr;
	yr = sqrt(d__1 * d__1 + 1.);
	d__1 = 1. / yr;
	*sterm = d_sign2(&d__1, b);
	*cterm = *sterm * xr;
	*sig = nnls_abs(*b) * yr;
	return 0;
    }
    *sig = 0.;
    *cterm = 0.;
    *sterm = 1.;
    return 0;
} /* g1_ */

/*     SUBROUTINE H12 (MODE,LPIVOT,L1,M,U,IUE,UP,C,ICE,ICV,NCV) */

/*  CONSTRUCTION AND/OR APPLICATION OF A SINGLE */
/*  HOUSEHOLDER TRANSFORMATION..     Q = I + U*(U**T)/B */

/*  The original version of this code was developed by */
/*  Charles L. Lawson and Richard J. Hanson at Jet Propulsion Laboratory */
/*  1973 JUN 12, and published in the book */
/*  "SOLVING LEAST SQUARES PROBLEMS", Prentice-HalL, 1974. */
/*  Revised FEB 1995 to accompany reprinting of the book by SIAM. */
/*     ------------------------------------------------------------------ */
/*                     Subroutine Arguments */

/*     MODE   = 1 OR 2   Selects Algorithm H1 to construct and apply a */
/*            Householder transformation, or Algorithm H2 to apply a */
/*            previously constructed transformation. */
/*     LPIVOT IS THE INDEX OF THE PIVOT ELEMENT. */
/*     L1,M   IF L1 .LE. M   THE TRANSFORMATION WILL BE CONSTRUCTED TO */
/*            ZERO ELEMENTS INDEXED FROM L1 THROUGH M.   IF L1 GT. M */
/*            THE SUBROUTINE DOES AN IDENTITY TRANSFORMATION. */
/*     U(),IUE,UP    On entry with MODE = 1, U() contains the pivot */
/*            vector.  IUE is the storage increment between elements. */
/*            On exit when MODE = 1, U() and UP contain quantities */
/*            defining the vector U of the Householder transformation. */
/*            on entry with MODE = 2, U() and UP should contain */
/*            quantities previously computed with MODE = 1.  These will */
/*            not be modified during the entry with MODE = 2. */
/*     C()    ON ENTRY with MODE = 1 or 2, C() CONTAINS A MATRIX WHICH */
/*            WILL BE REGARDED AS A SET OF VECTORS TO WHICH THE */
/*            HOUSEHOLDER TRANSFORMATION IS TO BE APPLIED. */
/*            ON EXIT C() CONTAINS THE SET OF TRANSFORMED VECTORS. */
/*     ICE    STORAGE INCREMENT BETWEEN ELEMENTS OF VECTORS IN C(). */
/*     ICV    STORAGE INCREMENT BETWEEN VECTORS IN C(). */
/*     NCV    NUMBER OF VECTORS IN C() TO BE TRANSFORMED. IF NCV .LE. 0 */
/*            NO OPERATIONS WILL BE DONE ON C(). */
/*     ------------------------------------------------------------------ */
/* Subroutine */ int h12_(mode, lpivot, l1, m, u, iue, up, c__, ice, icv, ncv)
integer *mode, *lpivot, *l1, *m;
doublereal *u;
integer *iue;
doublereal *up, *c__;
integer *ice, *icv, *ncv;
{
    /* System generated locals */
    integer u_dim1, u_offset, i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    /* The following line was commented out after the f2c translation */
    /* double sqrt(); */

    /* Local variables */
    static integer incr;
    static doublereal b;
    static integer i__, j;
    static doublereal clinv;
    static integer i2, i3, i4;
    static doublereal cl, sm;

/*     ------------------------------------------------------------------ 
*/
/*     double precision U(IUE,M) */
/*     ------------------------------------------------------------------ 
*/
    /* Parameter adjustments */
    u_dim1 = *iue;
    u_offset = u_dim1 + 1;
    u -= u_offset;
    --c__;

    /* Function Body */
    if (0 >= *lpivot || *lpivot >= *l1 || *l1 > *m) {
	return 0;
    }
    cl = (d__1 = u[*lpivot * u_dim1 + 1], nnls_abs(d__1));
    if (*mode == 2) {
	goto L60;
    }
/*                            ****** CONSTRUCT THE TRANSFORMATION. ****** 
*/
    i__1 = *m;
    for (j = *l1; j <= i__1; ++j) {
/* L10: */
/* Computing MAX */
	d__2 = (d__1 = u[j * u_dim1 + 1], nnls_abs(d__1));
	cl = nnls_max(d__2,cl);
    }
    if (cl <= 0.) {
	goto L130;
    } else {
	goto L20;
    }
L20:
    clinv = 1. / cl;
/* Computing 2nd power */
    d__1 = u[*lpivot * u_dim1 + 1] * clinv;
    sm = d__1 * d__1;
    i__1 = *m;
    for (j = *l1; j <= i__1; ++j) {
/* L30: */
/* Computing 2nd power */
	d__1 = u[j * u_dim1 + 1] * clinv;
	sm += d__1 * d__1;
    }
    cl *= sqrt(sm);
    if (u[*lpivot * u_dim1 + 1] <= 0.) {
	goto L50;
    } else {
	goto L40;
    }
L40:
    cl = -cl;
L50:
    *up = u[*lpivot * u_dim1 + 1] - cl;
    u[*lpivot * u_dim1 + 1] = cl;
    goto L70;
/*            ****** APPLY THE TRANSFORMATION  I+U*(U**T)/B  TO C. ****** 
*/

L60:
    if (cl <= 0.) {
	goto L130;
    } else {
	goto L70;
    }
L70:
    if (*ncv <= 0) {
	return 0;
    }
    b = *up * u[*lpivot * u_dim1 + 1];
/*                       B  MUST BE NONPOSITIVE HERE.  IF B = 0., RETURN. 
*/

    if (b >= 0.) {
	goto L130;
    } else {
	goto L80;
    }
L80:
    b = 1. / b;
    i2 = 1 - *icv + *ice * (*lpivot - 1);
    incr = *ice * (*l1 - *lpivot);
    i__1 = *ncv;
    for (j = 1; j <= i__1; ++j) {
	i2 += *icv;
	i3 = i2 + incr;
	i4 = i3;
	sm = c__[i2] * *up;
	i__2 = *m;
	for (i__ = *l1; i__ <= i__2; ++i__) {
	    sm += c__[i3] * u[i__ * u_dim1 + 1];
/* L90: */
	    i3 += *ice;
	}
	if (sm != 0.) {
	    goto L100;
	} else {
	    goto L120;
	}
L100:
	sm *= b;
	c__[i2] += sm * *up;
	i__2 = *m;
	for (i__ = *l1; i__ <= i__2; ++i__) {
	    c__[i4] += sm * u[i__ * u_dim1 + 1];
/* L110: */
	    i4 += *ice;
	}
L120:
	;
    }
L130:
    return 0;
} /* h12_ */

doublereal diff_(x, y)
doublereal *x, *y;
{
    /* System generated locals */
    doublereal ret_val;


/*  Function used in tests that depend on machine precision. */

/*  The original version of this code was developed by */
/*  Charles L. Lawson and Richard J. Hanson at Jet Propulsion Laboratory 
*/
/*  1973 JUN 7, and published in the book */
/*  "SOLVING LEAST SQUARES PROBLEMS", Prentice-HalL, 1974. */
/*  Revised FEB 1995 to accompany reprinting of the book by SIAM. */

    ret_val = *x - *y;
    return ret_val;
} /* diff_ */


/* The following subroutine was added after the f2c translation */
int nnls_f2c(double* a, const int* mda, const int* m, const int* n, double* b, 
	 double* x, double* rnorm, double* w, double* zz, int* index, 
	 int* mode)
{
  return (nnls_(a, mda, m, n, b, x, rnorm, w, zz, index, mode));
}


