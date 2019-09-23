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
);

int nnls_f2c(double *a, int *mda, int *m, int *n, double *b, double *x, double *rnorm, double *w, double *zz, int *index, int *mode);
