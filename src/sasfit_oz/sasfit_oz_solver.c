/*
 * Author(s) of this file:
 *   Evgeniy Ponomarev (evgeniy.ponomarev@epfl.ch)
 *   Modified 13.09.2013
 *   extended and last modified by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   25.09.2015
 *   Andersen Acceleration option has been implemented by Alain Studer
 */
#include <stdio.h>
#include <stdlib.h>
#include <sasfit_oz.h>
#include <bool.h>
#include "../sasfit_old/include/sasfit.h"
#include "../sasfit_old/include/SASFIT_x_tcl.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <kinsol/kinsol.h>
// #include <nvector/nvector_openmp.h>
#include <nvector/nvector_serial.h>
#include <kinsol/kinsol_spgmr.h>
#include <kinsol/kinsol_spfgmr.h>
#include <kinsol/kinsol_spbcgs.h>
#include <kinsol/kinsol_sptfqmr.h>

// #include "itlin.h"
#define ITLIN_OPT void

#define kb GSL_CONST_MKSA_BOLTZMANN
#define GET_TCL(val_type, target, src_name) (sasfit_tcl_get_ ## val_type(interp, target, ozname, src_name) == TCL_OK)

void check_interrupt(sasfit_oz_data *OZd) {
    int interupt_signal;
    Tcl_Interp *interp;
    char * ozname = Tcl_GetStringFromObj(OZd->oz_obj, 0);

    interp = OZd->interp;
    if (!GET_TCL(int, &OZd->PrintProgress, "PrintProgress")) {
                OZd->PrintProgress = 0;
    }

    if (!GET_TCL(int, &OZd->interrupt, "interrupt")) {
                OZd->interrupt = 1;
    }
    if (OZd->interrupt == 1) {
            sasfit_out("interrupting OZ solver (%d)\n",OZd->interrupt);
    }
}

int cp_gsl_vector_to_array(const gsl_vector *src, double *target, int dimtarget) {
    int i;
    if (src->size != dimtarget) return GSL_FAILURE;
    for (i=0;i<dimtarget;i++) target[i] = gsl_vector_get(src,i);
    return GSL_SUCCESS;
}

int cp_array_diff_to_gsl_vector(double *src1, gsl_vector *src2, gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-gsl_vector_get(src2,i));
    }
    return GSL_SUCCESS;
}

int cp_array_diff_array_to_gsl_vector(double *src1, double *src2,  gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-src2[i]);
    }
    return GSL_SUCCESS;
}

int cp_array_diff_array_to_array(double *src1, double *src2,  double *target, int dimsrc) {
    int i,errno;
    errno=0;
    for (i=0;i<dimsrc;i++)  {
            target[i]=src1[i]-src2[i];
    }
    return GSL_SUCCESS;
}

int cp_array_to_gsl_vector(double *src, gsl_vector *target, int dimsrc) {
    int i,errno;
    errno=0;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src[i]);
    }
    return GSL_SUCCESS;
}

int cp_array_to_array(double *src, double *target, int dim) {
    int i;
    for (i=0;i<dim;i++)  {
            target[i]=src[i];
    }
    return GSL_SUCCESS;
}

int OZ_f(gsl_vector * x, void *OZd, gsl_vector * fres) {
    double res;
    sasfit_oz_data *OZdata;
    OZdata = (sasfit_oz_data *) OZd;
    cp_gsl_vector_to_array(x,OZdata->G,OZdata->Npoints);
    res = OZ_step(OZdata);
//    sasfit_out("OZ_f just has been called\n");
    cp_array_diff_to_gsl_vector(OZdata->G,x,fres,OZdata->Npoints);
    return GSL_SUCCESS;
}

double OZ_fp(double * x, void *OZd, double * fres) {
    double Norm;
    int i;
    sasfit_oz_data *OZdata;
    OZdata = (sasfit_oz_data *) OZd;
    cp_array_to_array(x,OZdata->G,OZdata->Npoints);
    Norm = OZ_step(OZdata);
//    sasfit_out("OZ_f just has been called\n");
    for (i=0;i<OZdata->Npoints;i++) fres[i]=(OZdata->G[i]-x[i]);
    return Norm;
}

//void matvec(int n, double *y, double *z, struct ITLIN_OPT *opt)
void matvec(int n, double *y, double *z, void *opt)
{
/*
    int i,Norm;
    cp_array_to_array(y,opt->OZd->G,n);
    Norm = OZ_step(opt->OZd);
    for (i=0;i<n;i++) z[i] = (opt->OZd->G[i] - y[i]);
    if ((opt->OZd->it % 50)==0 && opt->OZd->PrintProgress == 1) sasfit_out("matvec calls=%d\n",opt->OZd->it);
*/
}

//*****************************************************************************
//Additional functions needed for Anderson Mixing
//*****************************************************************************
//Fixpoint operator:
double fixpointOperatorOZ_GSL_API(gsl_vector * x, void *OZd, gsl_vector * fres) {
    double res;
    sasfit_oz_data *OZdata;
    OZdata = (sasfit_oz_data *) OZd;
    cp_gsl_vector_to_array(x, OZdata->G, OZdata->Npoints);
    res = OZ_step(OZdata);
//    sasfit_out("fixpointOperator_OZ_GSL_API just has been called\n");
    cp_array_to_gsl_vector(OZdata->G, fres, OZdata->Npoints);
    return res;
}

//W is a matrix which helps to reduce the constrained
//k dimensional optimization problem to a unconstrained,
//k-1 dimensional optimization problem (constraint is 1D)
gsl_matrix* calculateW(int k){
  gsl_matrix* W = gsl_matrix_alloc(k, k -1);
  gsl_matrix_set_zero(W);
  int i,j;
  for(i = 0; i < k; i++)
    for(j = 0; j < k-1; j++){
      if (i==j) //-1 on diagonal
        gsl_matrix_set(W, i, j, -1.0);
      if (i-1 == j) //1 on subdiagonal
        gsl_matrix_set(W, i, j, 1.0);
    }
  return W;
}


//A -> [A,c]. exception handling could still be better,
//but a problem here should be very rare..
gsl_matrix* addColumnToMatrixByExtension(gsl_matrix* A, const gsl_vector* c){
  int es; //To hold the exit status of the GSL methods

  size_t vectorSize = c->size;
  //A may be empty, this needs special treatment
  if (!A){
    //We allocate a Matrix containing c as single column
    gsl_matrix* A_new = gsl_matrix_alloc(vectorSize, 1);
    es = gsl_matrix_set_col(A_new, 0, c);
    return A_new;
  }
  //From here, Matrix must be already filled previously.
  //Find new Matrix dimensions: what is current dim?
  size_t numberOfRows = A->size1;
  
  if (vectorSize != numberOfRows){
    sasfit_out("Cannot add column vector to matrix due to dimension problems \n");
    sasfit_out("Number of rows in matrix: %zu \n", numberOfRows);
    sasfit_out("Vector size %zu \n", vectorSize);
    sasfit_out("Exiting from addColumnToMatrixByExtension... \n");
    return NULL;
  }
  
  
  int oldNumberOfColumns = A->size2;
  int newNumberOfColumns = oldNumberOfColumns +1;

  //Allocate new matrix
  gsl_matrix* A_new = gsl_matrix_alloc(numberOfRows, newNumberOfColumns);
  //Initialize new matrix to 0
  gsl_matrix_set_zero(A_new);
  //copy old values (i.e columns)
  //first we allocate a buffer column vector v
  gsl_vector* v = gsl_vector_alloc(numberOfRows);
  //Start loop over columns
  int j;
  for(j = 0; j < oldNumberOfColumns; j++){
    //copy the jth column of A to buffer vector v
    es = gsl_matrix_get_col(v, A, j);
    //Copy buffer to new matrix
    es = gsl_matrix_set_col(A_new, j, v);
  }
  //Finally add the new column
  es = gsl_matrix_set_col(A_new, newNumberOfColumns -1, c);
  //A is now unused, so we free its memory
  gsl_matrix_free(A);
  return A_new;
}  

//A = [a_0,...,a_n] -> [a_1,...a_n, c]
int addColumnToMatrixByShifting(gsl_matrix* A, const gsl_vector* c){
  //Find new Matrix dimensions
  size_t numberOfRows = A->size1;
  size_t vectorSize = c->size;
  
  if (vectorSize != numberOfRows){
    sasfit_out("Cannot add column vector to matrix due to dimension problems \n");
    sasfit_out("Number of rows in matrix: %zu \n", numberOfRows);
    sasfit_out("Vector size %zu \n", vectorSize);
    sasfit_out("Exiting from addColumnToMatrixByShifting...\n");
    return -1;
  }
  
  int es; //To hold the exit status of the GSL methods
  
  int numberOfColumns = A->size2;
  //copy old values (i.e columns)
  //first we allocate a buffer column vector v
  gsl_vector* v = gsl_vector_alloc(numberOfRows);
  //Start loop over columns
  int j;
  for(j = 0; j < numberOfColumns -1; j++){
    //copy the (j+1)th column of A to buffer vector v
    es = gsl_matrix_get_col(v, (const gsl_matrix*) A, j+1);
    //Copy buffer to column at the left
    es = gsl_matrix_set_col(A, j, v);
  }
  //Finally add the new column
  es = gsl_matrix_set_col(A, numberOfColumns -1, c);
  return es;
}

//*****************************************************************************
//FINISHED: Additional functions needed for Anderson Mixing
//*****************************************************************************

//COMMENT (AS): THE FOLLOWING define STATEMENTS ARE DANGEROUS,
//NEVER USE ANY OF THESE LETTERS/STRINGS TO DEFINE YOUR OWN VARIABLES IN THIS FILE!
//WORST CASE SCENARIO: YOU USE A NEW VARIABLE, SAY r, WHICH HAPPENS TO BE OF THE SAME TYPE AS
//OZd->r (double*), THEN THE COMPILER IS HAPPY, ESPECIALLY IF YOU FORGET TO DECLARE r, AND THIS
//WILL COMPLETELY MESS UP THE CODE.


#define MINDIMOZ 128
#define NITSTEP 20

#define NP  OZd->Npoints
#define EN  OZd->En
#define r   OZd->r
#define DR  OZd->dr
#define k   OZd->k
#define GF  OZd->Gf
#define G  OZd->G
#define G0  OZd->G0
#define g  OZd->g
#define HR  OZd->h
#define GAMMA_R  OZd->gamma_r
#define g0  OZd->g0
#define OZIN  OZd->in
#define OZOUT OZd->out
#define U   (*OZd->potential)
#define dU_dR OZd->ud
#define UBETA OZd->ubeta
#define PARAM OZd->pPot
#define BRIDGE OZd->Br
#define CAVITY OZd->yr
#define MAYER OZd->fr
#define dr  OZd->dr
#define c  OZd->c
#define CFOLD OZd->cfold
#define CFNEW OZd->cfnew
#define cf OZd->cf
#define ALPHA OZd->alpha
#define sBPGG OZd->alpha
#define aCJVM OZd->alpha
#define fBB OZd->alpha
#define PHI OZd->phi
#define CLOSURE OZd->cl
#define Fswitch OZd->f
#define S OZd->S
#define MIXCOEFF OZd->mixcoeff
#define MAXSTEPS OZd->maxsteps
#define RELERROR OZd->relerror
#define T OZd->T


// Initialization of memory needed for computation

void OZ_init(sasfit_oz_data *OZd) {
   double *tp;
   int i;
   OZd->beta=1.0/(kb*T);
   r      = (double*)malloc((NP)*sizeof(double));
   k      = (double*)malloc((NP)*sizeof(double));
   EN     = (double*)malloc((NP)*sizeof(double));
   G      = (double*)malloc((NP)*sizeof(double));
   HR     = (double*)malloc((NP)*sizeof(double));
   G0     = (double*)malloc((NP)*sizeof(double));
   g      = (double*)malloc((NP)*sizeof(double));
   g0     = (double*)malloc((NP)*sizeof(double));
   c      = (double*)malloc((NP)*sizeof(double));
   cf     = (double*)malloc((NP)*sizeof(double));
   CFOLD  = (double*)malloc((NP)*sizeof(double));
   CFNEW  = (double*)malloc((NP)*sizeof(double));
   GF     = (double*)malloc((NP)*sizeof(double));
   Fswitch= (double*)malloc((NP)*sizeof(double));
   S      = (double*)malloc((NP)*sizeof(double));
   UBETA  = (double*)malloc((NP)*sizeof(double));
   dU_dR  = (double*)malloc((NP)*sizeof(double));
   BRIDGE = (double*)malloc((NP)*sizeof(double));
   CAVITY = (double*)malloc((NP)*sizeof(double));
   MAYER  = (double*)malloc((NP)*sizeof(double));
   OZIN   = (double*)fftw_malloc(sizeof(double)*NP);
   OZOUT  = (double*)fftw_malloc(sizeof(double)*NP);

 //  GAMMA_R  = gsl_vector_view_array(G,NP);
   GAMMA_R  = gsl_vector_alloc(NP);
   for (i=0; i < NP; i++) {
        G[i]=0;
   }
   OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
   }

//Extrapolation routine
double extrapolate (double x1,double x2, double x3, double y1, double y2,double y3)
{  double Ex;
   Ex=-(-pow(x2,2)*x3*y1+x2*pow(x3,2)*y1+pow(x1,2)*x3*y2-x1*pow(x3,2)*y2-pow(x1,2)*x2*y3+x1*pow(x2,2)*y3)/((x1-x2)*(x1-x3)*(x2-x3));
   return Ex;
}


int OZ_first_order_divided_difference_matrix(sasfit_oz_data *OZd, double *x, double *y, gsl_matrix *DDF) {
    int i, j;
    int n;
    int kk;
    int l;
    int ms;
    double *u, *v, *Fu, *Fv;
    double Norm;

/*
    gsl_matrix *DDF;
    gsl_matrix *DDFinv;
    gsl_permutation *perm;
    DDF = gsl_matrix_calloc(NP,NP);
    DDFinv = gsl_matrix_calloc(NP,NP);
 */
 
    u = (double*)malloc((NP)*sizeof(double));
    v = (double*)malloc((NP)*sizeof(double));
    Fu = (double*)malloc((NP)*sizeof(double));
    Fv = (double*)malloc((NP)*sizeof(double));

    gsl_set_error_handler_off ();
    for (n=0;n<NP;n++) {
        for (kk=0; kk<=n; kk++) {
            u[kk] = x[kk];
            v[kk] = x[kk];
        }
        v[n] = y[n];
        for (kk=n; kk<NP; kk++) {
            u[kk] = y[kk];
            v[kk] = y[kk];
        }
        Norm=OZ_fp(u,OZd,Fu);
        Norm=OZ_fp(v,OZd,Fv);
        if ((x[n]-y[n])!=0) {
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,(Fu[l]-Fv[l])/(x[n]-y[n]));
            }
        } else {
            sasfit_out("problems in calculating divided difference operator due to division by 0\n");
            return FALSE;
        }

    }
    free(u);
    free(v);
    free(Fu);
    free(Fv);
    return TRUE;
}

int OZ_first_order_divided_difference (sasfit_oz_data *OZd, double *x, double *y, double *res) {
    int i, j;
    int n;
    int kk;
    int l;
    int ms;
    double *u, *v, *Fu, *Fv;
    double Norm;
    gsl_matrix *DDF;
    gsl_matrix *DDFinv;
    gsl_permutation *perm;
    gsl_vector *F_yn, *xn, *yn;

    DDF = gsl_matrix_calloc(NP,NP);
    DDFinv = gsl_matrix_calloc(NP,NP);
    F_yn = gsl_vector_calloc(NP);
    xn = gsl_vector_calloc(NP);
    yn = gsl_vector_calloc(NP);

    u = (double*)malloc((NP)*sizeof(double));
    v = (double*)malloc((NP)*sizeof(double));
    Fu = (double*)malloc((NP)*sizeof(double));
    Fv = (double*)malloc((NP)*sizeof(double));

    gsl_set_error_handler_off ();
    for (n=0;n<NP;n++) {
        for (kk=0; kk<=n; kk++) {
            u[kk] = x[kk];
            v[kk] = x[kk];
        }
        v[n] = y[n];
        for (kk=n; kk<NP; kk++) {
            u[kk] = y[kk];
            v[kk] = y[kk];
        }
        Norm=OZ_fp(u,OZd,Fu);
        Norm=OZ_fp(v,OZd,Fv);
//      DDF = [I - L(Fxn,xn)]
//      DDFinv = DDF^-1
/*
        if ((y[n]-x[n])!=0) {
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,-(Fu[l]-Fv[l])/(y[n]-x[n]));
            }
            gsl_matrix_set(DDF,n,n,1-(Fu[n]-Fv[n])/(y[n]-x[n]));
        } else {
            sasfit_out("problems in calculating divided difference operator due to division by 0\n");
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,-0);
            }
                gsl_matrix_set(DDF,n,n,1-0);
        }
*/
        if ((x[n]-y[n])!=0) {
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,(Fu[l]-Fv[l])/(x[n]-y[n]));
            }
        } else {
            sasfit_out("problems in calculating divided difference operator due to division by 0\n");
            for (l=0;l<NP;l++) {
                gsl_matrix_set(DDF,l,n,0);
            }
        }

    }
    perm=gsl_permutation_alloc(NP);
    gsl_linalg_LU_decomp(DDF, perm, &ms);
    gsl_linalg_LU_invert(DDF, perm, DDFinv);

    Norm=OZ_fp(y,OZd,res);
    cp_array_to_gsl_vector(y,yn,NP);
    cp_array_to_gsl_vector(res,F_yn,NP);

    gsl_blas_dgemv(CblasNoTrans,-1.0,DDFinv,F_yn,1.0,yn);
    cp_gsl_vector_to_array(yn,res,NP);

    gsl_matrix_free(DDF);
    gsl_matrix_free(DDFinv);
    gsl_permutation_free(perm);
    gsl_vector_free(F_yn);
    gsl_vector_free(xn);
    gsl_vector_free(yn);
    free(u);
    free(v);
    free(Fu);
    free(Fv);

}

int OZ_solver_by_iteration(sasfit_oz_data *OZd, sasfit_oz_root_algorithms algorithm) {
    double err, errold, Norm, Normold;
    double alpha, beta, gama,phi_set, phi_actual;
    double *xn, *yn, *zn, *un, *Tx, *Ty, *Tz;
    double nsoliparam[5], tol[2];
    int i,j,n,iloop,ierr;
    int  rcode,error,ms;
    double xh, resnorm, xdnorm;
	long int N;
    
 //   TERM_CHECK termcheck = CheckOnRestart;

 //Variable declaration:
  //The name of the variables and the implementation of the algorithm itself
  //follow closely the PhD of Peng Ni, Worcester Polytechnic Institute. Therein, AAA
  //is used in a DFT context. The 'Method of elimination' is chosen here, the reason
  //for this choice is good performance and a good condition number (see section
  //'method comparison' in PhD). However, from a theoretical point of view, I am not sure
  //on what extend the DFT results (where the fix point operator is the Hamiltonian
  //describing a Hartree-Fock step for a many-electron system, eq 3.2) 
  //can be transferred to OZ, but benchmarking this algorithm against others gives good results.
  //........................................................................................
  //maximalDimensionOfKrylovSpace is the number of elements in the vector space used
  //for Anderson mixing. Both the residuals and the states
  //used for mixing are in the Krylov subspace of the problem, hence the name
  int maximalDimensionOfKrylovSpace = 3;
  //To check when to switch between extend and shift
  int isMaximalDimensionOfKrylovSpaceReached = 0;
  int dimensionOfKrylovSpace = 0;
  //This is the dimension of the vector space where the fix point operator lives
  int dimensionOfVectorSpace = OZd->Npoints;;
  //Parameter of the algorithm
  //float b = 1.0; using alpha instead below. b is the notation in the above cited PhD.
  //Note that alpha here (contary to the other algorithms) does not describe the mixing coefficients.
  //These are stored in the vector 'a'. alpha here describes how the the mixing of the new state as a
  //weighted sum of the linear combination of past states plus the linear combination of past residuals 
  //is calculated. alpha = 1.0 means that we only take past states into account, alpha = 0.0 gives a
  //equal mixing. (General formula: x_new = Ka + (alpha - 1)Da, K: past states, D: past residuals)
  //alpha-values close to one seem to have better performance.
  //current state
  gsl_vector* x;
  //new (updated state), x_n = G(x), where G is fix Point Operator (OZ here)
  gsl_vector* x_n;
  //vector to hold residuum
  gsl_vector* r_n;
  //norm of the above
  double r_norm;
  //Anderson mixed state, i.e x_A = K a - (1- b) Da
  gsl_vector* x_A;
  //vector to hold mixing coefficients
  gsl_vector* a;
  //vector holding unconstrained lsf solution
  gsl_vector* a_reduced;
  //helper vector for QR decomposition
  gsl_vector* tau;
  //Matrix to hold previous states
  gsl_matrix* K = NULL;
  //Matrix to hold previous residuals
  gsl_matrix* D = NULL;
  //helper matrix to transform constrained optimization
  //problem to unconstrained optimization problem
  gsl_matrix* W;
  gsl_matrix* D_reduced;
  gsl_vector* res_opt;
  //negative of last entry in D
  gsl_vector* d;
  
  //GSL internal
  int gslReturnValue = 0;
  //did addColumnToMatrixByShifting succeed?:
  int matrixShiftReturnValue = 0;
  
  
    OZd->failed = 0;
    phi_actual = OZd->phi;
    phi_set = OZd->phi;
    do {
        Normold=1;
        err=2*RELERROR;
        OZd->phi=phi_actual ;
         if ( OZd->PrintProgress == 1)  sasfit_out("solving OZ for phi=%lf\n",OZd->phi);
        switch (algorithm) {
        case Picard_iteration:
                n = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    n++;
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g\n",n,OZd->it,err);
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                break;
        case Krasnoselskij_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                n=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g\n",n,OZd->it,err);
                    Normold=Norm;
                    alpha=fabs(MIXCOEFF);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*G[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                break;
        case Mann_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    Normold=Norm;
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*Tx[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                break;
        case PMH_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    cp_array_to_array(yn,G,NP);
                    Norm = OZ_step(OZd);
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case MannII_iteration:
                errold=err;
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    n++;
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    cp_array_to_array(yn,G,NP);
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            xn[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                    }
                    cp_array_to_array(xn,G,NP);

                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case S_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    Norm = OZ_step(OZd);
//                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(Tx);
                free(Ty);
                Norm = OZ_step(OZd);
                break;
        case Ishikawa_iteration:
                xn = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    for (j=0; j < NP; j++) {
                            G[j]=(1.0-beta)*xn[j]+beta*G[j];
                    }
                    Norm = OZ_step(OZd);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                Norm = OZ_step(OZd);
                free(xn);
                break;
        case SP_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*zn[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Noor_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*xn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case CR_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    errold=err;
                    n++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case PicardS_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));

                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        if (alpha>1) alpha =1.0;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*xn[j]+beta*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Sstar_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                Tz = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));
                n=0;
                iloop=0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    n++;
                    errold=err;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    err = fabs((Norm-Normold)/Norm);
                    if (MIXCOEFF > 0) {
                        alpha=1./(1+MIXCOEFF*n);
                        beta=1./(1.+MIXCOEFF*MIXCOEFF*n*n);
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                        beta=alpha;
                        gama=alpha;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                        beta=alpha;
                        gama=alpha;
                    }
                    if ((n % 100)==0 && OZd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
                    cp_array_to_array(G,Tx,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tz,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = G[j];
                    }
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < NP; j++) {
                            G[j]=(1-alpha)*Tx[j]+alpha*Ty[j];
                            xn[j] = G[j];
                    }
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                Norm = OZ_step(OZd);
                break;
        case Steffensen2_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    if (MIXCOEFF > 0) {
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                    }

                    iloop++;
                    cp_array_to_array(G,xn,NP);
                    Norm = OZ_step(OZd);
                    cp_array_to_array(G,Tx,NP);
                    for (i=0;i<NP;i++) yn[i] = (1-alpha)*xn[i]+alpha*G[i];
                    err = fabs((Norm-Normold)/Norm);

                    sasfit_out("loop: %d , error=%g\nnext step divided difference routine\n",iloop,err);
                    OZ_first_order_divided_difference(OZd,xn,yn,Tx);
                    sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);

                    cp_array_to_array(Tx,G,NP);

                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                break;
        case Steffensen4_iteration:
                Tx = (double*)malloc((NP)*sizeof(double));
                Ty = (double*)malloc((NP)*sizeof(double));
                xn = (double*)malloc((NP)*sizeof(double));
                yn = (double*)malloc((NP)*sizeof(double));
                zn = (double*)malloc((NP)*sizeof(double));
				un = (double*)malloc((NP)*sizeof(double));
				
				gsl_matrix *A, *B, *b0, *b2;
				gsl_matrix *Ainv, *Binv;
				gsl_permutation *perm;
				A = gsl_matrix_calloc(NP,NP);
				Ainv = gsl_matrix_calloc(NP,NP);
				B = gsl_matrix_calloc(NP,NP);
				Binv = gsl_matrix_calloc(NP,NP);
				b0 = gsl_matrix_calloc(NP,NP);
				b2 = gsl_matrix_calloc(NP,NP);
				
                iloop = 0;
                while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
                    check_interrupt(OZd);
                    if (MIXCOEFF > 0) {
                        alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
                    } else {
                        if (err < errold) {
                            iloop++;
                            iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                        } else {
                            iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                        }
                        alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
                    }

                    iloop++;
					
                    Norm = OZ_fp(xn,OZd,Tx);
					cp_array_to_array(G,zn,NP);
					
					sasfit_out("loop: %d , error=%g\nnext step divided difference routine\n",iloop,err);
                    error = OZ_first_order_divided_difference_matrix(OZd,xn,zn,A);
					sasfit_out("success divided differenzes: %d\n",error);
                    sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
					if (!error) break;
					perm=gsl_permutation_alloc(NP);
					gsl_linalg_LU_decomp(A, perm, &ms);
					gsl_linalg_LU_invert(A, perm, Ainv);

                    for (i=0;i<NP;i++) {
						un[i] = xn[i];
						for (j=0;j<NP;j++) un[i] = un[i]-gsl_matrix_get(Ainv,i,j)*Tx[j];
					}
/*
					error = OZ_first_order_divided_difference_matrix(OZd,un,xn,b0);
					sasfit_out("success divided differenzes: %d\n",error);
					if (!error) break;
					error = OZ_first_order_divided_difference_matrix(OZd,zn,un,b2);
					sasfit_out("success divided differenzes: %d\n",error);
					if (!error) break;
					
					for (i=0;i<NP;i++) {
						for (j=0;j<NP;j++) gsl_matrix_set(B,i,j,gsl_matrix_get(b0,i,j)-gsl_matrix_get(A,i,j)+gsl_matrix_get(b2,i,j));
					}
					gsl_linalg_LU_decomp(B, perm, &ms);
					gsl_linalg_LU_invert(B, perm, Binv);
					
					Norm = OZ_fp(un,OZd,Ty);
					Norm = 0;
                    for (i=0;i<NP;i++) {
						yn[i] = un[i];
						for (j=0;j<NP;j++) yn[i] = yn[i]-gsl_matrix_get(Binv,i,j)*Ty[j];
						Norm = Norm + gsl_pow_2(yn[i]);
					}
					Norm=sqrt(Norm);
                    err = fabs((Norm-Normold)/Norm);
                    cp_array_to_array(yn,G,NP);
*/
					cp_array_to_array(un,G,NP);
					for (i=0;i<NP;i++) {
						Norm = Norm + gsl_pow_2(un[i]);
					}
					Norm=sqrt(Norm);
                    err = fabs((Norm-Normold)/Norm);
                    Normold=Norm;
                    if (Norm != Norm) {
                        sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
                        OZd->failed = 1;
                        for (j=0; j < NP; j++) {
                            G[j]=0.0;
                        }
                    }
                }
                free(xn);
                free(yn);
                free(zn);
				free(un);
                free(Tx);
                free(Ty); 
				gsl_matrix_free(A);
				gsl_matrix_free(Ainv);
				gsl_matrix_free(B);
				gsl_matrix_free(Binv);
				gsl_matrix_free(b0);
				gsl_matrix_free(b2);
				gsl_permutation_free(perm);
                break;
        case NGMRES:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
/*
				u = N_VMake_Serial(N,G);
				NV_DATA S(u);
				kin_mem = KINCreate();
				flag = KINSpfgmr(kin_mem, 0);
				KINSetPrintLevel(kin_mem,0);
				KINSetUserData(kin_mem,OZd);
				KINSetSysFunc(kin_mem,OZ_step);
				KINSol(kin_mem,u,KIN_FP,u_scale, f_scale);
                cp_array_to_array(Tx,G,NP);
				N_VDestroy_Serial(u);
				KINFree(&kin mem);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
*/
                free(xn);
                free(Tx);
                break;
        case NBiCGSTAB:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
                nsoliparam[0]=240;
                nsoliparam[1]=480;
                nsoliparam[2]=-0.1;
                nsoliparam[3]=3;
                nsoliparam[4]=20;
                tol[0]=RELERROR;
                tol[1]=RELERROR;
                nsoli(xn,&OZ_step,OZd,tol,nsoliparam,Tx,&ierr);
                cp_array_to_array(Tx,G,NP);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                free(xn);
                free(Tx);
                break;
        case NTFQMR:
                xn = (double*)malloc((NP)*sizeof(double));
                Tx = (double*)malloc((NP)*sizeof(double));
                cp_array_to_array(G,xn,NP);
                nsoliparam[0]=20;
                nsoliparam[1]=40;
                nsoliparam[2]=-0.1;
                nsoliparam[3]=4;
                nsoliparam[4]=20;
                tol[0]=RELERROR;
                tol[1]=RELERROR;
                nsoli(xn,&OZ_step,OZd,tol,nsoliparam,Tx,&ierr);
                cp_array_to_array(Tx,G,NP);
                sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                free(xn);
                free(Tx);
                break;
        case AndersonAcc:
                //To check when to switch between extend and shift
                isMaximalDimensionOfKrylovSpaceReached = 0;
                dimensionOfKrylovSpace = 0;
                //Matrix to hold previous states
                K = NULL;
                //Matrix to hold previous residuals
                D = NULL;
  
                //GSL internal
                gslReturnValue = 0;
                //did addColumnToMatrixByShifting succeed?:
                matrixShiftReturnValue = 0;
 
 //Variable allocation
  //............................................................
  //allocate memory for gsl variables
                x = gsl_vector_alloc(dimensionOfVectorSpace);
                x_n = gsl_vector_alloc(dimensionOfVectorSpace);
                x_A = gsl_vector_alloc(dimensionOfVectorSpace);
  //res_opt is internal (how good was the optimization)
                res_opt = gsl_vector_alloc(dimensionOfVectorSpace);
                d = gsl_vector_alloc(dimensionOfVectorSpace);

  //helper (buffer) vectors
                gsl_vector* summand_1 = gsl_vector_alloc(dimensionOfVectorSpace);
                gsl_vector* summand_2 = gsl_vector_alloc(dimensionOfVectorSpace);

  //initialize gsl variables
  //init x to zero. Seems to be ok for OZ.
                for (j=0; j < NP; j++) {
                    gsl_vector_set(x,j,G[j]);
                }
//                gsl_vector_set_zero(x);
  //gsl_vector_add_constant(x, 1.0);
                gsl_vector_set_zero(x_n);
                gsl_vector_set_zero(x_A);

//
 
  //Main Loop of algorithm. x must be initialized to x_0 at this stage
  //...................................................................
  n=0;
  iloop=0;
  while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0) {
	n++;
    check_interrupt(OZd);
	if (MIXCOEFF > 0) {
		alpha=(1-MIXCOEFF)*exp(-log10(err/RELERROR))+MIXCOEFF;
    } else {
		if (err < errold) {
			iloop++;
			iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
        } else {
			iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
        }
		alpha=-MIXCOEFF+(1.0*iloop)/(1.0*NITSTEP)*(1+MIXCOEFF);
    }
    //Increment current dimension of Krylov space
    //(This equals the number of states used for AA mixing)
    if (dimensionOfKrylovSpace < maximalDimensionOfKrylovSpace)
      dimensionOfKrylovSpace++;
    else
      isMaximalDimensionOfKrylovSpaceReached = 1;
    //printf("dimensionOfKrylovSpace %d\n", dimensionOfKrylovSpace);
    //Apply x_n = G(x)
    Norm = fixpointOperatorOZ_GSL_API(x, OZd, x_n);
    if (Norm != Norm) {
      sasfit_out("detected NAN for precision of OZ solution: %g\n",Norm);
      OZd->failed = 1;
      for (j=0; j < NP; j++) {
        G[j]=0.0;
        gsl_vector_set(x,j,0.0);
      }
    }
    err = fabs((Norm-Normold)/Norm); //err must be defined since is loop criterion
    //sasfit_out("error %f\n", err);
    Normold=Norm;
    if ((n % 100)==0 && OZd->PrintProgress == 1)
         sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,OZd->it,err,alpha);
    //First we save the new value x_n in Matrix K (GSL sub will overwrite it)
    //As long as current dimensionOfKrylovSpace is smaller than what defined as maximum,
    //we extend the vector to the matrix...
    if (!isMaximalDimensionOfKrylovSpaceReached){
      K = addColumnToMatrixByExtension(K, x_n);
      if (!K){
        sasfit_out("Couldn't extend K matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }
    }else{ //... we add by replacing the oldest
      matrixShiftReturnValue = addColumnToMatrixByShifting(K, x_n);
      if (matrixShiftReturnValue < 0){
        sasfit_out("Couldn't shift K matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }
    }
    
    //Calculate residuum r = x_n - x, result is stored in x_n
    gslReturnValue = gsl_vector_sub(x_n, x); //printf("%d \n", gslReturnValue);
    r_n = x_n; //r_n ist just a pointer without allocated memory, just used for better naming

    //save residuum r_n in Matrix D
    if (!isMaximalDimensionOfKrylovSpaceReached){
      D = addColumnToMatrixByExtension(D, r_n);
      if (!D){
        sasfit_out("Couldn't extend D matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }
      
    }else{
      matrixShiftReturnValue = addColumnToMatrixByShifting(D, r_n);
      if (matrixShiftReturnValue < 0){
        sasfit_out("Couldn't shift D matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }
    }
    //printf("D: \n"); printMatrix(D);
    
    if (dimensionOfKrylovSpace == 1){
      gsl_vector_memcpy(x, x_n);
      continue; //If D consists of one vector only, we skip the least square fit
                //(The constraint sum a_i =1 implies that there must be at least two vectors)
    }
    //Since we updated the Matrix D, we are now ready to solve the
    //constrained least square optimization problem
    if (!isMaximalDimensionOfKrylovSpaceReached)
      W = calculateW(dimensionOfKrylovSpace);
    //else W needs not be recalculated since it remains constant (in size und ueberhaupt)
    //We allocate memory for the result matrix D_reduced := DW
    if (!isMaximalDimensionOfKrylovSpaceReached){
      D_reduced = gsl_matrix_alloc(dimensionOfVectorSpace, dimensionOfKrylovSpace -1);
      //tau is internal to store QR decomposition
      tau = gsl_vector_alloc(dimensionOfKrylovSpace -1);
      //Allocate memory for the solution of the reduced (unconstrained) optimization problem
      a_reduced = gsl_vector_alloc(dimensionOfKrylovSpace -1);
      a = gsl_vector_alloc(dimensionOfKrylovSpace);
    }
    gsl_matrix_set_zero(D_reduced);
    //We calculate D_reduced = DW    
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, D, W, 0.0, D_reduced);
    //Ready to solve min || D_reduced a_reduced + r_n || 
    //            a_reduced
    //QR decomposition
    gsl_vector_set_zero(tau);
    gslReturnValue = gsl_linalg_QR_decomp(D_reduced, tau);
    gsl_vector_set_zero(a_reduced);
    //d = -r
    gsl_vector_memcpy(d, r_n);
    //mind the sign in the lsf problem: D_reduced*a_reduced = -d
    gsl_vector_scale(d, -1.0);
    gslReturnValue = gsl_linalg_QR_lssolve(D_reduced, tau, d, a_reduced, res_opt);
    //printf("norm of least square fit difference vector: %f\n", gsl_blas_dnrm2(r));
    //Now a_reduced holds the solution of the unconstrained problem, next we want 'a'
    //zero a
    gsl_vector_set_zero(a);
    //a = W*a_reduced + (0,0,..,0,1)
    gsl_blas_dgemv(CblasNoTrans, 1.0, W, a_reduced, 0.0, a);
    gsl_vector_set(a, dimensionOfKrylovSpace -1, gsl_vector_get(a, dimensionOfKrylovSpace -1) + 1.0);
    //printf("a = "); printVector(a);
    //x = Ka - (1-b)Da = Ka + (b-1)Da. First x_A = Ka ....
    gsl_vector_set_zero(summand_1);
    gsl_blas_dgemv(CblasNoTrans, 1.0, K, a, 0.0, summand_1);
    //...then  (1-b)Da
    gsl_vector_set_zero(summand_2);
    gsl_blas_dgemv(CblasNoTrans, (alpha - 1.0), D, a, 0.0, summand_2); // using alpha here instead of b
    //Finally calculate the sum
    gsl_vector_add(summand_1, summand_2);
    gsl_vector_memcpy(x_A, summand_1);
    gsl_vector_memcpy(x, x_A);
    
    //Finally, free memory, if dimensionOfKrylovSpace == maximalDimensionOfKrylovSpace then
    //the memory will be reused, else we can free it.
    if (dimensionOfKrylovSpace < maximalDimensionOfKrylovSpace){
      gsl_matrix_free(W);
      gsl_matrix_free(D_reduced);
      gsl_vector_free(a);
      gsl_vector_free(a_reduced);
      gsl_vector_free(tau);
    }
  }//end main loop
  
  //Memory de-allocation
  //............................................................................
  //Free all memory explicitly, first the memory of size DimensionOfKrylovSpace
  //(or mixed with dimensionOfVectorSpace like D_reduced)...
  gsl_matrix_free(W);
  gsl_matrix_free(D_reduced);
  gsl_vector_free(a);
  gsl_vector_free(a_reduced);
  gsl_vector_free(tau);
  
  gsl_matrix_free(D);
  gsl_matrix_free(K);
  
  //..then the memory of size dimensionOfVectorSpace
  gsl_vector_free(x);
  gsl_vector_free(x_n);
  gsl_vector_free(x_A);
  gsl_vector_free(res_opt);
  gsl_vector_free(d);
  gsl_vector_free(summand_1);
  gsl_vector_free(summand_2);  
  
  //                  sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
                break;
    }
   
        if (OZd->failed == 1) { 
            if (phi_actual == phi_set) {
                    phi_actual = phi_set/ 2.0;
                    OZd->failed = 0;
                     sasfit_out("try first phi %lf\n",phi_actual);
            } 
        } else {
                if (phi_actual != phi_set) {
                    phi_actual =phi_actual+phi_set/ 20.0;
                    sasfit_out("try now phi %lf\n",phi_actual);
                }
        }
//        sasfit_out("it %d, interrupt: %d\n",OZd->it,OZd->interrupt);
    } while (OZd->failed == 0 && fabs(phi_actual - phi_set) > 1e-8 && OZd->it < MAXSTEPS  && OZd->interrupt == 0);
 //   sasfit_out("number of OZ_step calls are: %d\n",OZd->it);
}

int OZ_solver_by_gsl_multroot(sasfit_oz_data *OZd,sasfit_oz_root_algorithms algorithm) {
    const gsl_multiroot_fsolver_type *Tgsl;
    gsl_multiroot_fsolver *sgsl;
    gsl_multiroot_function Fgsl;
    int status,j;
    size_t iter;
    double Norm, Normold, err;


    Fgsl.f = &OZ_f;
    Fgsl.n = OZd->Npoints;
    Fgsl.params = OZd;
    switch (algorithm) {
        case dNewton:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_dnewton;
                break;
        case Hybrids:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrids;
                break;
        case Hybrid:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrid;
                break;
        case Broyden:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
                break;
        default:
                MIXCOEFF = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
    }
    sasfit_out("initialising root solver: %d\n",algorithm);
    sgsl = gsl_multiroot_fsolver_alloc (Tgsl, Fgsl.n);
    gsl_multiroot_fsolver_set (sgsl, &Fgsl, GAMMA_R);
    iter = 0;
    Normold=1;
    do {
        sasfit_out("starting %d-th iteration\n", iter+1);
        iter++;
        status = gsl_multiroot_fsolver_iterate (sgsl);
        Norm=0;
        for (j=0; j < NP; j++) {
            Norm=gsl_pow_2(G[j])+Norm;
        }
        Norm=sqrt(Norm);
        err = fabs((Norm-Normold)/Norm);
        Normold=Norm;
//        status = gsl_multiroot_test_residual (sgsl->f, RELERROR);
        sasfit_out("iterations: %d, status = %s\n", iter, gsl_strerror (status));
        sasfit_out("up to now the number of OZ_step calls are: %d\n",OZd->it);
        sasfit_out("residual of gamma(r)-OZ(gamma(r)):%g \n",gsl_blas_dasum(sgsl->f));
        sasfit_out("error:%g \n",err);
//        if (status)  { /* check if solver is stuck */
//            break;
//       }
    } while (OZd->it < MAXSTEPS && err > RELERROR && OZd->interrupt == 0);
    gsl_multiroot_fsolver_free (sgsl);
    return 0;
}

//Calling OZ_solver and generating output
void OZ_calculation (sasfit_oz_data *OZd) {
  int igneg;
  OZd->it=0;
  if (CLOSURE==RY || CLOSURE==BPGG || CLOSURE==HMSA || CLOSURE==CJVM || CLOSURE==BB) {
        sasfit_out("Root finding\n");
        root_finding(OZd);
  } else if (CLOSURE==RMSA) {
        CLOSURE=MSA;
        sasfit_out("Solving with MSA-closure first ...");
        OZ_solver(OZd);
        sasfit_out(" done\n");
        CLOSURE = RMSA;
        igneg = 0;
// finding contact point sigma+
        while (EN[igneg] == 0.0 && igneg < NP) {
            igneg++;
        }
// check where g(sigma+) >= 0, than nothing has to be done as RMSA=MSA which has been solved already above
        if (g[igneg] >= 0 || igneg == NP) {
            sasfit_out("g(sigma+)>=0 or g(r) > 0 for all r, i.e. no rescaling necessary\n");
            return;
        }
  } else {
        sasfit_out("Root finding not required\n");
  }
  OZ_solver(OZd);
  OZ_pot_der(OZd);
}


double OZ_step(sasfit_oz_data *OZd) {
    double  err, ro, dk, Sm, GNorm2, Norm, V, Gstar,residualG,tmpr;
    static double Normold=0;
    int i,j;
    double powarg, logarg,dtmp ;
    bool doneB;

    OZ_func_one_t * tmp_potential;
    ro=6*PHI/(M_PI*gsl_pow_3(PARAM[0]));
    dk = M_PI/((NP+1.0)*dr);

    doneB=FALSE;
    OZd->it++;


    if (((OZd->it*OZd->Npoints) % (100*MINDIMOZ))==0) {
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
   
    switch (CLOSURE) {
        case PY:
            for (i=0; i < NP; i++){
                BRIDGE[i] = log(1.0+G[i])-G[i];
                doneB=TRUE;
                c[i]=(1.+G[i])*(EN[i]-1.);
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case HNC:
            for (i=0; i < NP; i++){
                BRIDGE[i] = 0.0;
                doneB=TRUE;
                c[i]=-1-G[i]+EN[i]*exp(G[i]);
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case RHNC:
            for (i=0; i < NP; i++){
                c[i]=g0[i]*exp((G[i]-G0[i])-OZd->beta*OZd->pertubation_pot(r[i],T,PARAM))-G[i]-1;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case RY:
            for (i=0; i < NP; i++){
                if (ALPHA == 0) {
                    BRIDGE[i] = log(1.0+G[i])-G[i];
                    doneB=TRUE;
                    c[i]=(1.+G[i])*(EN[i]-1.);
                } else {
                    BRIDGE[i] = -G[i]+log(1.0+((exp(Fswitch[i]*G[i])-1)/Fswitch[i]));
                    doneB=TRUE;
                    c[i]=EN[i]*(1+(exp(Fswitch[i]*G[i])-1)/Fswitch[i])-G[i]-1;
                }
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case HMSA:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                if (ALPHA == 0) {
                    BRIDGE[i] = log(1.0+Gstar)-Gstar;
                    doneB=TRUE;
                    c[i]=(1.0+Gstar)*(EN[i]-1.0);
                } else {
                    BRIDGE[i] = -Gstar+log(1.0+((exp(Fswitch[i]*Gstar)-1)/Fswitch[i]));
                    doneB=TRUE;
                    c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                }
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case Verlet:
            for (i=0; i < NP; i++){
                BRIDGE[i] = -(gsl_pow_2(G[i])/(2.0*(1.+4.0/5.0*G[i])));
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case DH:
            for (i=0; i < NP; i++){
                if (OZd->potential == &U_Lennard_Jones) {
                    Gstar = G[i]+PARAM[1]*gsl_pow_6(PARAM[0]/r[i])
                                *exp(-PARAM[1]/OZd->beta*gsl_pow_6(PARAM[0]/r[i])/(PHI*M_PI/6.0));
                } else {
                    Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                }
                BRIDGE[i] = -gsl_pow_2(Gstar)/(2.0*(1.0+(5.0*Gstar+11.0)/(7.0*Gstar+9.0)*Gstar));
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case CG:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                if (Gstar > 0) {
                    BRIDGE[i] = -0.5*gsl_pow_2(Gstar)/(1.0+(1.0175-0.275*6*PHI/M_PI)*Gstar);
                } else {
                    BRIDGE[i] = -0.5*gsl_pow_2(Gstar);
                }
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case MS:
            for (i=0; i < NP; i++){
                powarg = 1.0+2.0*G[i];
                BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-G[i]-1.0;
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case BPGG:
            for (i=0; i < NP; i++){
                powarg = 1.0+sBPGG*G[i];
                BRIDGE[i] = GSL_SIGN(powarg)*pow(fabs(powarg),1.0/sBPGG)-G[i]-1.0;
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case VM:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                powarg = 1.0+2.0*Gstar;
                BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-Gstar-1.0;
//            BRIDGE[i] = sqrt(1.0+2.0*Gstar)-Gstar-1.0;
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case CJVM:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                powarg = (1.0+4.0*aCJVM*Gstar);
                BRIDGE[i] = 1.0/(2*aCJVM)*(GSL_SIGN(powarg)*sqrt(fabs(powarg))-1.0-2.0*aCJVM*Gstar);
//            BRIDGE[i] = 1.0/(2*aCJVM)*(sqrt((1.0+4.0*aCJVM*Gstar))-1.0-2.0*aCJVM*Gstar);
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case BB:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                powarg = 1.0+2.0*Gstar+fBB*Gstar*Gstar;
                BRIDGE[i] = GSL_SIGN(powarg)*sqrt(fabs(powarg))-Gstar-1.0;
//           BRIDGE[i] = sqrt(1.0+2.0*Gstar+fBB*Gstar*Gstar)-1.0-Gstar;
                doneB=TRUE;
                c[i]=EN[i]*exp(G[i]+BRIDGE[i])-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case SMSA:
            for (i=0; i < NP; i++){
                Gstar = G[i]-OZd->beta*OZd->attractive_pot(r[i],T,PARAM);
                BRIDGE[i] = log(1.0+Gstar)-Gstar;
                doneB=TRUE;
                c[i]=EN[i]*exp(OZd->beta*OZd->attractive_pot(r[i],T,PARAM))*(Gstar+1.0)-G[i]-1.0;
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
        case MSA:
            for (i=0; i < NP; i++){
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
        case mMSA:
            for (i=0; i < NP; i++){
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
        case RMSA:
            for (i=0; i < NP; i++){
                if (EN[i]==0.0) {
                    c[i] = -(G[i]+1.0);
                } else {
                    if (CLOSURE==MSA || CLOSURE==RMSA) {
                        c[i] = -UBETA[i];
                    } else if (CLOSURE==mMSA) {
                        c[i] = MAYER[i];
                    }
                }
                g[i]= c[i]+G[i]+1.0;
                OZIN[i]=(i+1)*c[i];
            }
            break;
    }
    
    cp_array_to_gsl_vector(G,GAMMA_R,NP);
 
 //   OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute_r2r(OZd->pl,OZIN,OZOUT);
    dtmp=4.0*M_PI*dr*dr/(2.0*dk);
    for (j=0; j < NP; j++) {
        cf[j]=dtmp*OZOUT[j]/(j+1.0);
        CFOLD[j]=cf[j];
        GF[j]=ro*cf[j]*cf[j]/(1.0-ro*cf[j]);
        OZIN[j]=GF[j]*(j+1.0);
    }
//    OZd->pl=fftw_plan_r2r_1d(NP, OZIN, OZOUT, FFTW_RODFT00, FFTW_ESTIMATE);
    fftw_execute_r2r(OZd->pl,OZIN,OZOUT);
    Sm=0;
    GNorm2 = 0;
    dtmp=4.*M_PI*dk*dk/(2.0*dr*gsl_pow_3(2.0*M_PI));
    for (j=0; j < NP; j++) {
        G[j]=dtmp*OZOUT[j]/(j+1.0);
        S[j]=1./(1.-ro*cf[j]);
        HR[j]=c[j]+G[j];
        Sm=S[j]*S[j]+Sm;
        GNorm2=G[j]*G[j]+GNorm2;
        if (!doneB) {
            if (g[j] > 0 && EN[j] != 0) {
                BRIDGE[j]=log(g[j])+UBETA[j]-G[j];
            } else {
                BRIDGE[j] = 0.0;
            }
        }
        CAVITY[j] = exp(BRIDGE[j]+G[j]);
    }
    return sqrt(GNorm2);
    return sqrt(Sm);
}

// Iterative solution of Ornstein-Zernike equation
void OZ_solver (sasfit_oz_data *OZd) {
    int i;
    double V, dk;
    OZ_func_one_t * tmp_potential;

    dk = M_PI/((NP+1.0)*dr);
    r[0]=dr;
    k[0]=dk;

    for (i=1; i < NP; i++) {
        r[i]=(r[i-1]+dr);
        k[i]=(k[i-1]+dk);
    }

    if (CLOSURE==RHNC) {
        tmp_potential =OZd->potential;
        OZd->potential=OZd->reference_pot;
        CLOSURE=PY;
        sasfit_out("Solving reference system (HS) with PY-closure first ...");
        OZ_solver(OZd);
        sasfit_out(" done\n");
        CLOSURE = RHNC;
        OZd->potential=tmp_potential;
        for (i=0; i < NP; i++){
            g0[i]=g[i];
            G0[i]=G[i];
        }
    }

    for (i=0; i < NP; i++) {
        Fswitch[i]=1-exp(-ALPHA*r[i]);
        V = U(r[i],T,PARAM);
        UBETA[i]=V*OZd->beta;
        if (CLOSURE==RMSA) {
            if (r[i]<PARAM[0] || V==GSL_POSINF) {
                EN[i]=0.0;
            } else {
                EN[i]=exp(-UBETA[i]);
            }
        } else {
            if (V == GSL_POSINF) {
                EN[i]=0.0;
            } else {
                EN[i]=exp(-UBETA[i]);
            }
        }
        MAYER[i] = EN[i]-1.;
        BRIDGE[i] = 0.0;
        CAVITY[i] = 0.0;
    }

    OZd->it=0;
    switch (OZd->root_algorithm) {
        case Picard_iteration:
                OZ_solver_by_iteration(OZd,Picard_iteration);
                break;
        case Mann_iteration:
                OZ_solver_by_iteration(OZd,Mann_iteration);
                break;
        case Ishikawa_iteration:
                OZ_solver_by_iteration(OZd,Ishikawa_iteration);
                break;
        case Noor_iteration:
                OZ_solver_by_iteration(OZd,Noor_iteration);
                break;
        case S_iteration:
                OZ_solver_by_iteration(OZd,S_iteration);
                break;
        case SP_iteration:
                OZ_solver_by_iteration(OZd,SP_iteration);
                break;
        case CR_iteration:
                OZ_solver_by_iteration(OZd,CR_iteration);
                break;
        case PicardS_iteration:
                OZ_solver_by_iteration(OZd,PicardS_iteration);
                break;
        case PMH_iteration:
                OZ_solver_by_iteration(OZd,PMH_iteration);
                break;
        case MannII_iteration:
                OZ_solver_by_iteration(OZd,MannII_iteration);
                break;
        case Krasnoselskij_iteration:
                OZ_solver_by_iteration(OZd,Krasnoselskij_iteration);
                break;
        case Sstar_iteration:
                OZ_solver_by_iteration(OZd,Sstar_iteration);
                break;
        case Steffensen2_iteration:
                OZ_solver_by_iteration(OZd,Steffensen2_iteration);
                break;
        case Steffensen4_iteration:
                OZ_solver_by_iteration(OZd,Steffensen4_iteration);
                break;
        case AndersonAcc:
                OZ_solver_by_iteration(OZd,AndersonAcc);
                break;
        case Broyden:
                OZ_solver_by_gsl_multroot(OZd,Broyden);
                break;
        case dNewton:
                OZ_solver_by_gsl_multroot(OZd,dNewton);
                break;
        case Hybrid:
                OZ_solver_by_gsl_multroot(OZd,Hybrid);
                break;
        case Hybrids:
                OZ_solver_by_gsl_multroot(OZd,Hybrids);
                break;
        case NGMRES:
                OZ_solver_by_iteration(OZd,NGMRES);
                break;
        case NTFQMR:
                OZ_solver_by_iteration(OZd,NTFQMR);
                break;
        case NewtonLibGMRES:
                OZ_solver_by_iteration(OZd,NewtonLibGMRES);
                break;
        default:
                sasfit_err("this algorithm is planned to be implemented\n");
    }


    sasfit_out("Needed %d calls to OZ_step\n",OZd->it);
 
    Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
    Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);


    OZd->Sq0=extrapolate(k[0],k[1],k[2],S[0],S[1],S[2]);
    OZd->gr0=extrapolate(r[0],r[1],r[2],g[0],g[1],g[2]);
    OZd->cr0=extrapolate(r[0],r[1],r[2],c[0],c[1],c[2]);
    OZd->dq=dk;
}

//Routine for calculation derivative from pair potential

void OZ_pot_der(sasfit_oz_data *OZd)
{  int n;

    if (U(r[0],T,PARAM)==GSL_POSINF && U(r[1],T,PARAM)==GSL_POSINF) {
        dU_dR[0]=0;
    } else if (U(r[0],T,PARAM)==GSL_POSINF && U(r[1],T,PARAM)!=GSL_POSINF) {
        dU_dR[0]=GSL_NEGINF;
    } else {
        dU_dR[0]=(U(r[1],T,PARAM)-U(r[0],T,PARAM))/(dr);
    }

    for (n=1; n < (NP-1); n++) {
        if (   (U(r[n  ],T,PARAM)==GSL_POSINF)
            && (U(r[n+1],T,PARAM)==GSL_POSINF)
            && (U(r[n-1],T,PARAM)==GSL_POSINF) ) {
            dU_dR[n]=0;
        } else if (   (U(r[n  ],T,PARAM)==GSL_POSINF)
                   && (U(r[n+1],T,PARAM)!=GSL_POSINF)) {
            dU_dR[n]=GSL_NEGINF;
        } else if (   (U(r[n  ],T,PARAM)!=GSL_POSINF)
                   && (U(r[n+1],T,PARAM)==GSL_POSINF)) {
            dU_dR[n]=GSL_POSINF;
        } else if (   (U(r[n-1],T,PARAM)==GSL_POSINF)
                   && (U(r[n  ],T,PARAM)!=GSL_POSINF)
                   && (U(r[n+1],T,PARAM)!=GSL_POSINF)) {
            dU_dR[n]=(U(r[n+1],T,PARAM)-U(r[n],T,PARAM))/(dr);
        } else{
            dU_dR[n]=(U(r[n+1],T,PARAM)-U(r[n-1],T,PARAM))/(2*dr);
        }
    }

     dU_dR[NP-1]=(U(r[NP-1],T,PARAM)-U(r[NP-2],T,PARAM))/(dr);
 }

//Routine for calculation the difference between compressibilities determined by virial and compressibility routes

double compressibility_calc(double scp, void *params)
{    sasfit_oz_data *OZd;
     int i;
     double iphi;
     double S0, chicp, chivir,Delta_chi, r1,r2,r3,P2,P3, sum2,sum3;
     sum2=0.0;
     sum3=0.0;

   OZd = (sasfit_oz_data *) params;
   ALPHA=scp;

   iphi = PHI;
   OZ_solver(OZd);
   S0=OZd->Sq0;
   r1=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   chicp=S0*OZd->beta/r1;

   OZ_pot_der(OZd);

   PHI=iphi-iphi*0.01;
   r2=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   OZ_solver(OZd);
   for (i=0; i<NP-1; i++) {
       if (dU_dR[i] == GSL_NEGINF) {
            if (i>0) {
                sum2=sum2-gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum2=sum2-gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else if (dU_dR[i] == GSL_POSINF) {
            if (i>0) {
                sum2=sum2+gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum2=sum2+gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else {
            sum2=sum2+(dU_dR[i])*g[i]*gsl_pow_3(r[i+1]);
       }
   }
   P2=(r2/OZd->beta)*(1.-(2.*M_PI*OZd->beta*r2/3.)*dr*sum2);

   PHI=iphi+iphi*0.01;
   r3=6.0*PHI/(M_PI*gsl_pow_3(PARAM[0]));
   OZ_solver(OZd);
   for (i=0; i<NP-1; i++) {
       if (dU_dR[i] == GSL_NEGINF) {
            if (i>0) {
                sum3=sum3-gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum3=sum3-gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else if (dU_dR[i] == GSL_POSINF) {
            if (i>0) {
                sum3=sum3+gsl_pow_3(r[i])*(g[i+1]-g[i-1])/OZd->beta/dr;
            } else {
                sum3=sum3+gsl_pow_3(r[i])*(g[i+1]-g[i])/OZd->beta/dr;
            }
       } else {
            sum3=sum3+(dU_dR[i])*g[i]*gsl_pow_3(r[i+1]);
       }
   }
   P3=(r3/OZd->beta)*(1.0-(2.0*M_PI*OZd->beta*r3/3.0)*dr*sum3);
   chivir=(r3-r2)/((P3-P2)*r1);
   PHI=iphi;
   Delta_chi = chicp-chivir;
   if (Delta_chi != Delta_chi) {
        sasfit_out("detected NAN for compressibility, alpha value: %g\n",ALPHA);
        OZd->failed = 1;
        for (i=0; i < NP; i++) {
            G[i]=0.0;
        }
    }
   return Delta_chi;
}
//Routine to minimize the difference between two compressibilities

void rescaleMSA (sasfit_oz_data *OZd) {
    int signchange,i,j;
    double refnew, refold, alpha_left, alpha_right, scp_inter;
    refold=0;
    signchange = 0;
    sasfit_out("\nrescale sigma so that g(sigma'+) becomes >=0: \n");
    sasfit_out("      sigma      |      sigma'+     |      g(sigma'+)     \n");
    sasfit_out("-----------------|------------------|---------------------\n");

    i = 0;
    while (signchange==0 && i<=28) {
        if (CLOSURE==RY || CLOSURE==HMSA){
            refnew=compressibility_calc(100/pow(2,i), OZd);
            alpha_left  = 100/pow(2,i);
            alpha_right = 100/pow(2,i-1);
        }
        if (CLOSURE==BPGG) {
            refnew=compressibility_calc(5.601-0.2*i, OZd);
            alpha_left  = 5.601-0.2*i;
            alpha_right = 5.601-0.2*(i-1);
        }
        if (CLOSURE==CJVM) {
            refnew=compressibility_calc(0.001+0.05*i, OZd);
            alpha_right  = 0.001+0.05*i;
            alpha_left = 0.001+0.05*(i-1);
        }
        if (CLOSURE==BB) {
            refnew=compressibility_calc(-0.5+0.1*i, OZd);
            alpha_right  = -0.5+0.1*i;
            alpha_left = -0.5+0.1*(i-1);
        }
        sasfit_out("          %15g  |   %15g \n", alpha_left , refnew/OZd->beta);

        if ((refnew*refold<0)) {
            signchange=1;
        } else {
            if (abs(refnew) < abs(refold)) {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_left;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_left;
            } else {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_right;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_right;
            }
            refold=refnew;
            if (i==28) {
                sasfit_out("No change of sign in %d steps \n", i);
                sasfit_out("choosen self-consistency parameter:%f\n",scp_inter);
                OZd->alpha=scp_inter;
            }
        }
        i++;
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    if (signchange==1) {
        int status;
        int iter;
        int max_iter;
        const gsl_root_fsolver_type *Tt;
        gsl_root_fsolver *s;
        double root = 0.0;
        double x_lo = alpha_left;
        double x_hi = alpha_right;
        gsl_function F;

        F.function = compressibility_calc;
        F.params = OZd;

        Tt = gsl_root_fsolver_brent;
        s = gsl_root_fsolver_alloc (Tt);
        gsl_root_fsolver_set (s, &F, x_lo, x_hi);

        sasfit_out("using %s method\n",gsl_root_fsolver_name (s));
        sasfit_out(" iter |  lower      upper  |    root    err(est)\n");
        iter = 0;
        max_iter = 100;
        do {
            iter++;
            status = gsl_root_fsolver_iterate (s);
            root = gsl_root_fsolver_root (s);
            x_lo = gsl_root_fsolver_x_lower (s);
            x_hi = gsl_root_fsolver_x_upper (s);
            status = gsl_root_test_interval (x_lo, x_hi, 0, 1e-5);
            if (status == GSL_SUCCESS) {
                sasfit_out("Converged:\n");
            }
            sasfit_out("%5d |%.7f, %.7f| %.7f %.7f\n", iter, x_lo, x_hi,  root,  x_hi - x_lo);

            Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
            check_interrupt(OZd);
        } while (status == GSL_CONTINUE && iter < max_iter && OZd->interrupt == 0);
        OZd->alpha = root;
        sasfit_out("consistency parameter after optimization: %g \n", root);
        gsl_root_fsolver_free (s);
    }
}
//Routine to minimize the difference between two compressibilities

void root_finding (sasfit_oz_data *OZd) {
    int signchange,i,j;
    double refnew, refold, alpha_left, alpha_right, scp_inter;
    refold=0;
    signchange = 0;
    sasfit_out("\nSearching for the interval where sign changes: \n");
    sasfit_out("%s |  %s \n","self-consistency parameter" , "(chicomp-chivir)/beta");
    sasfit_out("---------------------------|-------------------------\n");

    i = 0;
    while (signchange==0 && i<=28) {
        if (CLOSURE==RY || CLOSURE==HMSA){
            refnew=compressibility_calc(100/pow(2,i), OZd);
            alpha_left  = 100/pow(2,i);
            alpha_right = 100/pow(2,i-1);
        }
        if (CLOSURE==BPGG) {
            refnew=compressibility_calc(5.601-0.2*i, OZd);
            alpha_left  = 5.601-0.2*i;
            alpha_right = 5.601-0.2*(i-1);
        }
        if (CLOSURE==CJVM) {
            refnew=compressibility_calc(0.001+0.05*i, OZd);
            alpha_right  = 0.001+0.05*i;
            alpha_left = 0.001+0.05*(i-1);
        }
        if (CLOSURE==BB) {
            refnew=compressibility_calc(-0.5+0.1*i, OZd);
            alpha_right  = -0.5+0.1*i;
            alpha_left = -0.5+0.1*(i-1);
        }
        sasfit_out("          %15g  |   %15g \n", alpha_left , refnew/OZd->beta);

        if ((refnew*refold<0)) {
            signchange=1;
        } else {
            if (abs(refnew) < abs(refold)) {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_left;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_left;
            } else {
                if (    CLOSURE==RY
                    ||  CLOSURE==HMSA)   scp_inter=alpha_right;

                if (    CLOSURE==BPGG
                    ||  CLOSURE==CJVM
                    ||  CLOSURE==BB)     scp_inter=alpha_right;
            }
            refold=refnew;
            if (i==28) {
                sasfit_out("No change of sign in %d steps \n", i);
                sasfit_out("choosen self-consistency parameter:%f\n",scp_inter);
                OZd->alpha=scp_inter;
            }
        }
        i++;
        Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
        Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
    }
    if (signchange==1) {
        int status;
        int iter;
        int max_iter;
        const gsl_root_fsolver_type *Tt;
        gsl_root_fsolver *s;
        double root = 0.0;
        double x_lo = alpha_left;
        double x_hi = alpha_right;
        gsl_function F;

        F.function = compressibility_calc;
        F.params = OZd;

        Tt = gsl_root_fsolver_brent;
        s = gsl_root_fsolver_alloc (Tt);
        gsl_root_fsolver_set (s, &F, x_lo, x_hi);

        sasfit_out("using %s method\n",gsl_root_fsolver_name (s));
        sasfit_out(" iter |  lower      upper  |    root    err(est)\n");
        iter = 0;
        max_iter = 100;
        do {
            iter++;
            status = gsl_root_fsolver_iterate (s);
            root = gsl_root_fsolver_root (s);
            x_lo = gsl_root_fsolver_x_lower (s);
            x_hi = gsl_root_fsolver_x_upper (s);
            status = gsl_root_test_interval (x_lo, x_hi, 0, 1e-5);
            if (status == GSL_SUCCESS) {
                sasfit_out("Converged:\n");
            }
            sasfit_out("%5d |%.7f, %.7f| %.7f %.7f\n", iter, x_lo, x_hi,  root,  x_hi - x_lo);

            Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
            Tcl_EvalEx(OZd->interp,"update",-1,TCL_EVAL_DIRECT);
            check_interrupt(OZd);
        } while (status == GSL_CONTINUE && iter < max_iter && OZd->interrupt == 0);
        OZd->alpha = root;
        sasfit_out("consistency parameter after optimization: %g \n", root);
        gsl_root_fsolver_free (s);
    }
}

//Release of memory used for computation
void OZ_free (sasfit_oz_data *OZd) {
    free(r);
    free(k);
    free(EN);
    free(G);
    free(G0);
    free(g);
    free(HR);
    free(g0);
    free(dU_dR);
    free(UBETA);
    free(c);
    free(cf);
    free(CFNEW);
    free(CFOLD);
    free(GF);
    free(Fswitch);
    free(S);
    free(BRIDGE);
    free(CAVITY);
    free(MAYER);
    gsl_vector_free(GAMMA_R);

    fftw_destroy_plan(OZd->pl);
    fftw_free(OZIN);
    fftw_free(OZOUT);
}

