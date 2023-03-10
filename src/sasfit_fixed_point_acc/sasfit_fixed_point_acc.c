/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   29.4.2019
 *   Andersen Acceleration option has been implemented by Alain Studer
 */
#include <stdio.h>
#include <stdlib.h>
#include <sasfit_fixed_point_acc.h>
#include <bool.h>
#include "../sasfit_old/include/sasfit.h"
#include "../sasfit_old/include/sasfit.h"
#include "../sasfit_old/include/SASFIT_x_tcl.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <kinsol/kinsol.h>
// #include <nvector/nvector_openmp.h>
#include <nvector/nvector_serial.h>
//#include <kinsol/kinsol.h>
#include <kinsol/kinsol_spgmr.h>
#include <kinsol/kinsol_spfgmr.h>
#include <kinsol/kinsol_spbcgs.h>
#include <kinsol/kinsol_sptfqmr.h>

// #include "itlin.h"
#define ITLIN_OPT void

#define kb GSL_CONST_MKSA_BOLTZMANN
#define GET_TCL(val_type, target, src_name) (sasfit_tcl_get_ ## val_type(interp, target, fpname, src_name) == TCL_OK)


void  KINErrSASfit_fp(int error_code,
                               const char *module, const char *function,
                               char *msg, void *FPd_structure){

    sasfit_fp_data *FPd;
    FPd = (sasfit_fp_data*) FPd_structure;
    sasfit_out("optained error code %d from %s-%s:%s\n",error_code, module,function,msg);
};

void KINInfoSASfit_fp(const char *module, const char *function,
                                char *msg, void *FPd_structure){

    sasfit_fp_data *FPd;
    long int nfe, nnlsi;
    double fnorm;
    int flag;
    char sBuffer[256];
    FPd = (sasfit_fp_data*) FPd_structure;
    sasfit_out("Info message from %s-%s:%s\n",module,function,msg);
    if (FPd->KINSetPrintLevel == 5) {
        flag= KINGetNumFuncEvals(FPd->kin_mem,&nfe);
        flag = KINGetNumNonlinSolvIters(FPd->kin_mem,&nnlsi);
        flag = KINGetFuncNorm(FPd->kin_mem,&fnorm);
        sprintf(sBuffer,"storeOZstepinfo \"%d\t%le\t%d\t%d\t%le\"",FPd->it,FPd->gNorm, nfe, nnlsi,fnorm);
        Tcl_EvalEx(FPd->interp,sBuffer,-1,TCL_EVAL_DIRECT);
    }
};

/*
 * Check function return value...
 *    opt == 0 means SUNDIALS function allocates memory so check if
 *             returned NULL pointer
 *    opt == 1 means SUNDIALS function returns a flag so check if
 *             flag >= 0
 *    opt == 2 means function allocates memory so check if returned
 *             NULL pointer
 */
static int check_flag(void *flagvalue, char *funcname, int opt)
{
  int *errflag;

  /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
  if (opt == 0 && flagvalue == NULL) {
    fprintf(stderr,
            "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
	    funcname);
    return(1);
  }

  /* Check if flag < 0 */
  else if (opt == 1) {
    errflag = (int *) flagvalue;
    if (*errflag < 0) {
      fprintf(stderr,
              "\nSUNDIALS_ERROR: %s() failed with flag = %d\n\n",
	      funcname, *errflag);
      return(1);
    }
  }

  /* Check if function returned NULL pointer - no memory allocated */
  else if (opt == 2 && flagvalue == NULL) {
    fprintf(stderr,
            "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
	    funcname);
    return(1);
  }

  return(0);
}

void check_interrupt_fp(sasfit_fp_data *FPd) {
    int interupt_signal;
    Tcl_Interp *interp;
    int ival;

//    char * fpname = Tcl_GetStringFromObj(FPd->fp_obj, 0);

    interp = FPd->interp;
//    sasfit_out("testing existence of sasfit timer\n");
    if (FPd->tm == NULL) {
        FPd->tm = sasfit_timer_create();
        sasfit_timer_start(FPd->tm);
        sasfit_out("sasfit timer was not created\n");
    }
//    sasfit_out("check_interrupt_fp: time:%lg\tTcl_Array_Name:%s\n",sasfit_timer_measure(FPd->tm),FPd->Tcl_Array_Name);
    if (sasfit_timer_measure(FPd->tm) > MAXTM4UPDATE) {
//        sasfit_out("try update\n");
        Tcl_EvalEx(interp,"update",-1,TCL_EVAL_DIRECT);
//        sasfit_out("try update seemed to work\n");
//        sasfit_out("try to read PrintProgress\n");
        if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,"EMOptions","PrintProgress",0),
                             &ival) ) {
            sasfit_err("could not read PrintProgress, will set it to 0\n");
            FPd->PrintProgress = 0;
        } else {
            FPd->PrintProgress = ival;
//            sasfit_out("FPd->PrintProgress=%d\n",FPd->PrintProgress);
        }
//        sasfit_out("reading PrintProgress seemed to work\n");
//        sasfit_out("try to read Interrupt\n");
        if (TCL_ERROR == Tcl_GetInt(interp,
                             Tcl_GetVar2(interp,"EMOptions","Interrupt",0),
                             &ival) ) {
            sasfit_err("could not read PrintProgress, will set it to 0\n");
            FPd->interrupt = 0;
        } else {
            FPd->interrupt = ival;
//            sasfit_out("FPd->interrupt=%d\n",FPd->interrupt);
        }
        sasfit_timer_start(FPd->tm);
    }
}

int fp_cp_gsl_vector_to_array(const gsl_vector *src, double *target, int dimtarget) {
    int i;
    if (src->size != dimtarget) return GSL_FAILURE;
    for (i=0;i<dimtarget;i++) target[i] = gsl_vector_get(src,i);
    return GSL_SUCCESS;
}


int fp_cp_N_Vector_to_array(N_Vector src, double *target, int dimtarget) {
    int i;
    if (NV_LENGTH_S(src) != dimtarget) return GSL_FAILURE;
    for (i=0;i<dimtarget;i++) target[i] = NV_Ith_S(src,i);
    return GSL_SUCCESS;
}

int fp_cp_array_to_N_Vector( double * src, N_Vector target, int dimsrc) {
    int i;
    if (NV_LENGTH_S(target) != dimsrc) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++) NV_Ith_S(target,i) = src[i] ;
    return GSL_SUCCESS;
}

int fp_cp_array_diff_N_Vector_to_N_Vector(double *src1,  N_Vector src2, N_Vector target, int dimsrc) {
    int i;
    if (NV_LENGTH_S(target) != dimsrc) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  NV_Ith_S(target,i) = src1[i]-NV_Ith_S(src2,i);
    return GSL_SUCCESS;
}

int fp_cp_array_diff_to_gsl_vector(double *src1, const gsl_vector *src2, gsl_vector *target, int dimsrc) {
    int i;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-gsl_vector_get(src2,i));
    }
    return GSL_SUCCESS;
}

int fp_cp_array_diff_array_to_gsl_vector(double *src1, double *src2,  gsl_vector *target, int dimsrc) {
    int i;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src1[i]-src2[i]);
    }
    return GSL_SUCCESS;
}

int fp_cp_array_diff_array_to_array(double *src1, double *src2,  double *target, int dimsrc) {
    int i;
    for (i=0;i<dimsrc;i++)  {
            target[i]=src1[i]-src2[i];
    }
    return GSL_SUCCESS;
}

int fp_cp_array_to_gsl_vector(double *src, gsl_vector *target, int dimsrc) {
    int i;
    if (dimsrc != target->size) return GSL_FAILURE;
    for (i=0;i<dimsrc;i++)  {
            gsl_vector_set(target,i,src[i]);
    }
    return GSL_SUCCESS;
}

int fp_cp_array_to_array(double *src, double *target, int dim) {
    int i;
    for (i=0;i<dim;i++)  {
            target[i]=src[i];
    }
    return GSL_SUCCESS;
}

int FP_f(const gsl_vector * x, void *FPd, gsl_vector * fres) {
    double res;
    sasfit_fp_data *FPdata;
    FPdata = (sasfit_fp_data *) FPd;
    fp_cp_gsl_vector_to_array(x,FPdata->out,FPdata->Npoints);
    res = FP_step(FPdata);
    fp_cp_array_diff_to_gsl_vector(FPdata->out,x,fres,FPdata->Npoints);
    return GSL_SUCCESS;
}

double FP_fp(double * x, void *FPd, double * fres) {
    double Norm;
    int i;
    sasfit_fp_data *FPdata;
    FPdata = (sasfit_fp_data *) FPd;
    fp_cp_array_to_array(x,FPdata->out,FPdata->Npoints);
    Norm = FP_step(FPdata);
    for (i=0;i<FPdata->Npoints;i++) fres[i]=(FPdata->out[i]-x[i]);
    return Norm;
}

//*****************************************************************************
//Additional functions needed for Anderson Mixing
//*****************************************************************************
//Fixpoint operator:
double fixpointOperatorFP_GSL_API(gsl_vector * x, void *FPd, gsl_vector * fres) {
    double res;
    sasfit_fp_data *FPdata;
    FPdata = (sasfit_fp_data *) FPd;
    fp_cp_gsl_vector_to_array(x, FPdata->out, FPdata->Npoints);
    res = FP_step(FPdata);
    fp_cp_array_to_gsl_vector(FPdata->out, fres, FPdata->Npoints);
    return res;
}

//W is a matrix which helps to reduce the constrained
//k dimensional optimization problem to a unconstrained,
//k-1 dimensional optimization problem (constraint is 1D)
gsl_matrix* fp_calculateW(int k){
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
gsl_matrix* fp_addColumnToMatrixByExtension(gsl_matrix* A, const gsl_vector* c){
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
    sasfit_out("Exiting from fp_addColumnToMatrixByExtension... \n");
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
  //so is v..
  gsl_vector_free(v);
  return A_new;
}

//A = [a_0,...,a_n] -> [a_1,...a_n, c]
int fp_addColumnToMatrixByShifting(gsl_matrix* A, const gsl_vector* c){
  //Find new Matrix dimensions
  size_t numberOfRows = A->size1;
  size_t vectorSize = c->size;

  if (vectorSize != numberOfRows){
    sasfit_out("Cannot add column vector to matrix due to dimension problems \n");
    sasfit_out("Number of rows in matrix: %zu \n", numberOfRows);
    sasfit_out("Vector size %zu \n", vectorSize);
    sasfit_out("Exiting from fp_addColumnToMatrixByShifting...\n");
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
  //...and deallocate buffer
  gsl_vector_free(v);
  return es;
}

//*****************************************************************************
//FINISHED: Additional functions needed for Anderson Mixing
//*****************************************************************************


// Initialization of memory needed for computation
int FP_init(sasfit_fp_data *FPd) {
	FPd->it=0;
	FPd->tm = sasfit_timer_create();
    sasfit_timer_start(FPd->tm);
//   not used AT THE MOMENT
	return 1;
}


int KIN_sasfit_fp_configure(void *kin_mem,sasfit_fp_data *FPd) {
    int flag;
    flag += KINSetMaxNewtonStep(kin_mem, FPd->KINSetMaxNewtonStep);
    if (FPd->PrintProgress) sasfit_out("KINSetMaxNewtonStep(flag)=%d\n",flag);

    flag += KINSetInfoHandlerFn(kin_mem, &KINInfoSASfit_fp, FPd);
    flag += KINSetErrHandlerFn(kin_mem, &KINErrSASfit_fp, FPd);
    if  (FPd->KINSetPrintLevel > 3 ) {
        flag += KINSetPrintLevel(kin_mem,1);
    } else if ( FPd->KINSetPrintLevel < 0) {
        flag += KINSetPrintLevel(kin_mem,3);
    } else {
        flag += KINSetPrintLevel(kin_mem,FPd->KINSetPrintLevel);
    }
    if (FPd->PrintProgress) sasfit_out("KINSetPrintLevel(flag)=%d\n",flag);

    flag += KINSetEtaForm(kin_mem, FPd->KINSetEtaForm);
    if (FPd->PrintProgress) sasfit_out("KINSetEtaForm(flag)=%d\n",flag);


    flag += KINSetNumMaxIters(kin_mem,FPd->KINSetNumMaxIters);
    if (FPd->PrintProgress) sasfit_out("KINSetNumMaxIters(flag)=%d KINSetNumMaxIters:%d\n",flag,FPd->KINSetNumMaxIters);
    if (FPd->KINSetMAA < FPd->KINSetNumMaxIters) {
        flag += KINSetMAA(kin_mem, FPd->KINSetMAA);
    } else {
        flag += KINSetMAA(kin_mem, 0);
    }

    flag += KINSetScaledStepTol(kin_mem,FPd->KINSetScaledSteptol);
    if (FPd->PrintProgress) sasfit_out("KINSetScaledStepTol(flag)=%d\n",flag);

    flag += KINSetFuncNormTol(kin_mem, FPd->KINSetFuncNormTol);
    if (FPd->PrintProgress) sasfit_out("KINSetFuncNormTol(flag)=%d FuncNormTol: %lg\n",flag,FPd->KINSetFuncNormTol);
    flag += KINSetUserData(kin_mem,FPd);
    if (FPd->PrintProgress) sasfit_out("KINSetUserData(flag)=%d\n",flag);
    if (FPd->root_algorithm != KINSOLFP) flag += KINSetConstraints(kin_mem, FPd->KINConstraints);
    return flag;
}

int FP_solver_by_gsl_multroot(sasfit_fp_data *FPd,sasfit_oz_root_algorithms algorithm) {
    const gsl_multiroot_fsolver_type *Tgsl;
    gsl_multiroot_fsolver *sgsl;
    gsl_multiroot_function Fgsl;
    int status,j,stoppingflag;
    size_t iter;


    Fgsl.f = &FP_f;
    Fgsl.n = FPd->Npoints;
    Fgsl.params = FPd;
    switch (algorithm) {
        case dNewton:
                FPd->mixcoeff = 1.0;
                Tgsl = gsl_multiroot_fsolver_dnewton;
                break;
        case Hybrids:
                FPd->mixcoeff = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrids;
                break;
        case Hybrid:
                FPd->mixcoeff = 1.0;
                Tgsl = gsl_multiroot_fsolver_hybrid;
                break;
        case Broyden:
                FPd->mixcoeff = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
                break;
        default:
                FPd->mixcoeff = 1.0;
                Tgsl = gsl_multiroot_fsolver_broyden;
    }
    if (FPd->PrintProgress) sasfit_out("initialising root solver: %d\n",algorithm);
    sgsl = gsl_multiroot_fsolver_alloc (Tgsl, Fgsl.n);
    gsl_multiroot_fsolver_set (sgsl, &Fgsl, FPd->DR);
    gsl_set_error_handler_off ();
    iter = 0;
    FPd->PrintProgress=TRUE;
    do {
        if (FPd->PrintProgress) sasfit_out("starting %d-th iteration\n", iter+1);
        iter++;
        status = gsl_multiroot_fsolver_iterate (sgsl);
        if (FPd->PrintProgress) sasfit_out("iterations: %d, status = %s\n", iter, gsl_strerror (status));
        if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
        if (FPd->PrintProgress) sasfit_out("residual of D(R)-FP(D(R)):%g \n",gsl_blas_dasum(sgsl->f));

        stoppingflag = gsl_multiroot_test_residual (sgsl->f, FPd->relerror);

//        if (status)  { /* check if solver is stuck */
//            break;
//       }
    } while (FPd->it < FPd->maxsteps && stoppingflag == GSL_CONTINUE && FPd->interrupt == 0 && FPd->failed==0);
    gsl_multiroot_fsolver_free (sgsl);
    return TCL_OK;
}

int FP_solver_by_iteration(sasfit_fp_data *FPd, sasfit_oz_root_algorithms algorithm) {
    double err, errold;
    double alpha, beta, gama,phi_set, phi_actual;
    double *xn, *yn, *zn, *un, *Tx, *Ty, *Tz, *gn, *gn1, *gn2, *xn1, *xn2, *xp1, *yn1;
    double nsoliparam[5], tol[2];
    int i,j,n,iloop,ierr;
    int  rcode,error,ms;
    double xh, resnorm, xdnorm;
	long int N;
	N_Vector u, f, scale;
    void * kin_mem;
    int flag, maxlrst;

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
  int maximalDimensionOfKrylovSpace = FPd->KINSetMAA;
  //To check when to switch between extend and shift
  int isMaximalDimensionOfKrylovSpaceReached = 0;
  int dimensionOfKrylovSpace = 0;
  //This is the dimension of the vector space where the fix point operator lives
  int dimensionOfVectorSpace = FPd->Npoints;;
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
  //did fp_addColumnToMatrixByShifting succeed?:
  int matrixShiftReturnValue = 0;


  FPd->failed = 0;

//	sasfit_out("initialization finished and start loop, algorithm: %d\n",algorithm);
//    do {
        err=2*FPd->relerror;
#define NITSTEP 20
        switch (algorithm) {
        case Picard_iteration:
                n = 0;
 //               sasfit_out("FPd->it:%d\tFPd->maxsteps:%d\tFPd->interrupt:%d\n",FPd->it,FPd->maxsteps,FPd->interrupt,FPd->failed);
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    n++;
                    err = FP_step(FPd);
//                    sasfit_out("Picard_iteration FPd->it:%d\terr:%lg\n",FPd->it,err);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g\n",n,FPd->it,err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->in[j]=DBL_MIN;
                        }
                    }
                }
                break;
        case Krasnoselskij_iteration:
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    n++;
                    err = FP_step(FPd);
                    alpha=fabs(FPd->mixcoeff);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*xn[j]+alpha*FPd->out[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    FPd->gNorm=err;
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g\n",n,FPd->it,err);
                }
                free(xn);
                break;
        case Mann_iteration:
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop = 0;;
                alpha=fabs(FPd->mixcoeff);
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    n++;
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*xn[j]+alpha*Tx[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=fabs(FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of OZ_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                }
                free(xn);
                free(Tx);
                break;
        case PMH_iteration:
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop = 0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                 while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    n++;
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    for (j=0; j < FPd->Npoints; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    fp_cp_array_to_array(yn,FPd->out,FPd->Npoints);
                    err = FP_step(FPd);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case MannII_iteration:
                errold=err;
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop = 0;
                alpha=FPd->maxsteps;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    n++;
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    for (j=0; j < FPd->Npoints; j++) {
                            yn[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    fp_cp_array_to_array(yn,FPd->out,FPd->Npoints);
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Ty,FPd->Npoints);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(Tx);
                free(yn);
                free(Ty);
                break;
        case S_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));

                n=0;
                iloop = 0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    n++;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*xn[j]+beta*Tx[j];
                    }
                    err = FP_step(FPd);
                    err = 0;
//                    cp_array_to_array(G,Ty,NP);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*Tx[j]+alpha*FPd->out[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(Tx);
                free(Ty);
                break;
        case Ishikawa_iteration:
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop = 0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    n++;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1.0-beta)*xn[j]+beta*FPd->out[j];
                    }
                    err = FP_step(FPd);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*xn[j]+alpha*FPd->out[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                break;
        case SP_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tz = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                zn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop=0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    n++;
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tz,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*zn[j]+beta*Tz[j];
                            yn[j] = FPd->out[j];
                    }
                    err = FP_step(FPd);
                    err = 0;
                    fp_cp_array_to_array(FPd->out,Ty,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                break;
        case Noor_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tz = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                zn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop=0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    n++;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tz,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*xn[j]+beta*Tz[j];
                            yn[j] = FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Ty,FPd->Npoints);
                    err = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*xn[j]+alpha*Ty[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                break;
        case CR_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tz = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                zn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop=0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    errold=err;
                    n++;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tz,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = FPd->out[j];
                    }
                    err = FP_step(FPd);
                    err = 0;
                    fp_cp_array_to_array(FPd->out,Ty,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*yn[j]+alpha*Ty[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                break;
        case PicardS_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tz = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                zn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop=0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    n++;
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*xn[j]+beta*Tx[j];
                            zn[j]= FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tz,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*Tx[j]+alpha*Tz[j];
                            yn[j] = FPd->out[j];
                    }
                    err = FP_step(FPd);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                break;
        case Sstar_iteration:
                Tx = (double*)malloc((FPd->Npoints)*sizeof(double));
                Ty = (double*)malloc((FPd->Npoints)*sizeof(double));
                Tz = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                zn = (double*)malloc((FPd->Npoints)*sizeof(double));
                n=0;
                iloop=0;
                alpha=FPd->mixcoeff;
                beta=alpha;
                gama=alpha;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    n++;
                    errold=err;
                    fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                    err = FP_step(FPd);
                    if ((n % 1000)==0 && FPd->PrintProgress == 1)
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    fp_cp_array_to_array(FPd->out,Tx,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-gama)*xn[j]+gama*Tx[j];
                            zn[j]= FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Tz,FPd->Npoints);
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-beta)*Tx[j]+beta*Tz[j];
                            yn[j] = FPd->out[j];
                    }
                    err = FP_step(FPd);
                    fp_cp_array_to_array(FPd->out,Ty,FPd->Npoints);
                    err=0;
                    for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=(1-alpha)*Tx[j]+alpha*Ty[j];
                            err += gsl_pow_2(FPd->out[j]-xn[j]);
                    }
                    err=sqrt(err);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                    }
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                    }
                }
                free(xn);
                free(yn);
                free(zn);
                free(Tx);
                free(Ty);
                free(Tz);
                break;
        case BIGGS_ANDREWS:
                xn  = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn1 = (double*)malloc((FPd->Npoints)*sizeof(double));
                xn2 = (double*)malloc((FPd->Npoints)*sizeof(double));
                xp1 = (double*)malloc((FPd->Npoints)*sizeof(double));
                gn  = (double*)malloc((FPd->Npoints)*sizeof(double));
                gn1 = (double*)malloc((FPd->Npoints)*sizeof(double));
                gn2 = (double*)malloc((FPd->Npoints)*sizeof(double));
                yn = (double*)malloc((FPd->Npoints)*sizeof(double));
                iloop=0;
                alpha=0;
                beta=alpha;
                gama=alpha;
                fp_cp_array_to_array(FPd->out,xn,FPd->Npoints);
                err = FP_step(FPd);
                for (j=0; j < FPd->Npoints; j++) {
                    xp1[j] = FPd->out[j];
                    gn2[j] = gn1[j];
                    gn1[j] = xp1[j]-xn[j];
                    xn2[j] = xn1[j];
                    xn1[j] = xn[j];
                    xn[j]  = xp1[j];
                    yn[j]  = xn[j];
                }
                err = FP_step(FPd);
                for (j=0; j < FPd->Npoints; j++) {
                    xp1[j] = FPd->out[j];
                    gn2[j] = gn1[j];
                    gn1[j] = xp1[j]-yn[j];
                    xn2[j] = xn1[j];
                    xn1[j] = xn[j];
                    xn[j]  = xp1[j];
                    yn[j]  = xn[j];
                }
                n=2;
                while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
                    check_interrupt_fp(FPd);
                    n++;
                    errold=err;
                    beta = 0;
                    gama = 0;
                    for (j=0; j < FPd->Npoints; j++) {
                        beta = beta + gn1[j]*gn2[j];
                        gama = gama + gn2[j]*gn2[j];
                    }
                    if (gama==0) {
                        alpha = 1;
                    } else {
                        if (FPd->KINSetMAA > 0) {
                            alpha = beta/gama;
                        } else {
                            alpha = GSL_SIGN(beta/gama)*sqrt(fabs(beta/gama));
                        };
                        alpha = GSL_MAX(GSL_MIN(alpha,(n-1.)/(n+2.0)),-(n-1.)/(n+2.0));
                    }
                    switch (abs(FPd->KINSetMAA) * (alpha != 0)) {
                        case 0:
                            for (j=0; j < FPd->Npoints; j++) {
                                yn[j] = xn[j];
                                FPd->out[j] = yn[j];
                            }
                            break;
                        case 1:
                            for (j=0; j < FPd->Npoints; j++) {
                                yn[j] = xn[j]+alpha*(xn[j]-xn1[j]);
                                FPd->out[j] = yn[j];
                            }
                            break;
                        default:
                            for (j=0; j < FPd->Npoints; j++) {
                                yn[j] = xn[j]+alpha*(xn[j]-xn1[j])+alpha*alpha/2.0*(xn[j]-2*xn1[j]+xn2[j]);
                                FPd->out[j] = yn[j];
                            }
                    }
                    err = FP_step(FPd);
                    for (j=0; j < FPd->Npoints; j++) {
                        xp1[j] = FPd->out[j];
                        gn2[j] = gn1[j];
                        gn1[j] = xp1[j]-yn[j];
                        xn2[j] = xn1[j];
                        xn1[j] = xn[j];
                        xn[j]  = xp1[j];
                    }
 //                   if ((n % 500)==0 && FPd->PrintProgress == 1)
                    if ((n % 2000)==0 )
                        sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
                    if (err != err) {
                        sasfit_out("detected NAN for precision of FP solution: %g\n",err);
                        FPd->failed = 1;
                        for (j=0; j < FPd->Npoints; j++) {
                            FPd->out[j]=DBL_MIN;
                        }
                        break;
                    }
                }
                free(xn);
                free(xn1);
                free(xn2);
                free(xp1);
                free(yn);
                free(gn);
                free(gn1);;
                free(gn2);
                break;
        case KINSOLFP:
                maxlrst = FPd->maxsteps/10;
				scale = N_VNew_Serial(FPd->Npoints);
                N_VConst_Serial(1., scale);        /* no scaling */
                kin_mem=NULL;
				u = N_VNew_Serial(FPd->Npoints);
                fp_cp_array_to_N_Vector(FPd->out,u,FPd->Npoints);

				kin_mem = KINCreate();
                FPd->kin_mem=kin_mem;
                  /* Set number of prior residuals used in Anderson acceleration */
                KIN_sasfit_fp_configure(kin_mem,FPd);

   				flag = KINInit(kin_mem,FP_step_kinsolFP,u);
                if (FPd->PrintProgress) sasfit_out("KINInit(flag)=%d\n",flag);

				flag = KINSpfgmr(kin_mem, 0);
                if (FPd->PrintProgress) sasfit_out("KINSptfqmr(flag)=%d\n",flag);

				flag = KINSol(kin_mem,u,KIN_FP,scale, scale);
                if (flag !=  KIN_SUCCESS && flag != KIN_INITIAL_GUESS_OK && flag != KIN_LINESEARCH_NONCONV) FPd->failed = 1;
                if (FPd->PrintProgress) sasfit_out("KINSol(flag)=%d\n",flag);
                KINGetFuncNorm(kin_mem, &err);
                if (FPd->PrintProgress) sasfit_out("err: %lg\n",err);

				N_VDestroy_Serial(u);
                N_VDestroy_Serial(scale);
				KINFree(&kin_mem);
                if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
                break;
        case GMRES:
                maxlrst = FPd->maxsteps/10;
				scale = N_VNew_Serial(FPd->Npoints);
                N_VConst_Serial(1.0, scale);        /* no scaling */
                kin_mem=NULL;
				u = N_VNew_Serial(FPd->Npoints);
                fp_cp_array_to_N_Vector(FPd->out,u,FPd->Npoints);

				kin_mem = KINCreate();
                FPd->kin_mem=kin_mem;
                  /* Set number of prior residuals used in Anderson acceleration */
                KIN_sasfit_fp_configure(kin_mem,FPd);

   				flag = KINInit(kin_mem,FP_step_kinsol,u);
                if (FPd->PrintProgress) sasfit_out("KINInit(flag)=%d\n",flag);

				flag = KINSpgmr(kin_mem, 0);
                if (FPd->PrintProgress) sasfit_out("KINSpgmr(flag)=%d\n",flag);

				flag = KINSol(kin_mem,u,FPd->KINSolStrategy,scale, scale);
                if (flag !=  KIN_SUCCESS && flag != KIN_INITIAL_GUESS_OK && flag != KIN_LINESEARCH_NONCONV) FPd->failed = 1;
                if (FPd->PrintProgress) sasfit_out("KINSol(flag)=%d\n",flag);
                KINGetFuncNorm(kin_mem, &err);
                if (FPd->PrintProgress) sasfit_out("err: %lg\n:",err);

				N_VDestroy_Serial(u);
                N_VDestroy_Serial(scale);
				KINFree(&kin_mem);
                if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
                break;
        case FGMRES:
                maxlrst = FPd->maxsteps/10;
				scale = N_VNew_Serial(FPd->Npoints);
                N_VConst_Serial(1.0, scale);        /* no scaling */
                kin_mem=NULL;
				u = N_VNew_Serial(FPd->Npoints);
                fp_cp_array_to_N_Vector(FPd->out,u,FPd->Npoints);

				kin_mem = KINCreate();
                FPd->kin_mem=kin_mem;
                  /* Set number of prior residuals used in Anderson acceleration */
                KIN_sasfit_fp_configure(kin_mem,FPd);

   				flag = KINInit(kin_mem,FP_step_kinsol,u);
                if (FPd->PrintProgress) sasfit_out("KINInit(flag)=%d\n",flag);

				flag = KINSpfgmr(kin_mem, 0);
                if (FPd->PrintProgress) sasfit_out("KINSpfqmr(flag)=%d\n",flag);

				flag = KINSol(kin_mem,u,FPd->KINSolStrategy,scale, scale);
                if (flag !=  KIN_SUCCESS && flag != KIN_INITIAL_GUESS_OK && flag != KIN_LINESEARCH_NONCONV) FPd->failed = 1;
                if (FPd->PrintProgress) sasfit_out("KINSol(flag)=%d\n",flag);
                KINGetFuncNorm(kin_mem, &err);
                if (FPd->PrintProgress) sasfit_out("err: %lg\n:",err);

				N_VDestroy_Serial(u);
                N_VDestroy_Serial(scale);
				KINFree(&kin_mem);
                if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
                break;
        case BiCGSTAB:
                maxlrst = FPd->maxsteps/10;
				scale = N_VNew_Serial(FPd->Npoints);
                N_VConst_Serial(1.0, scale);        /* no scaling */
                kin_mem=NULL;
				u = N_VNew_Serial(FPd->Npoints);
                fp_cp_array_to_N_Vector(FPd->out,u,FPd->Npoints);

				kin_mem = KINCreate();
                FPd->kin_mem=kin_mem;
                  /* Set number of prior residuals used in Anderson acceleration */
                flag = KINSetMaxNewtonStep(kin_mem, FPd->Npoints*100.0);
                KIN_sasfit_fp_configure(kin_mem,FPd);

   				flag = KINInit(kin_mem,FP_step_kinsol,u);
                if (FPd->PrintProgress) sasfit_out("KINInit(flag)=%d\n",flag);

				flag = KINSpbcg(kin_mem, 0);
                if (FPd->PrintProgress) sasfit_out("KINSpbcg(flag)=%d\n",flag);

				flag = KINSol(kin_mem,u,FPd->KINSolStrategy,scale, scale);
                if (flag !=  KIN_SUCCESS && flag != KIN_INITIAL_GUESS_OK && flag != KIN_LINESEARCH_NONCONV) FPd->failed = 1;
                if (FPd->PrintProgress) sasfit_out("KINSol(flag)=%d\n",flag);
                KINGetFuncNorm(kin_mem, &err);
                if (FPd->PrintProgress) sasfit_out("err: %lg\n:",err);

				N_VDestroy_Serial(u);
                N_VDestroy_Serial(scale);
				KINFree(&kin_mem);
                if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
                break;
        case TFQMR:
                maxlrst = FPd->maxsteps/10;
				scale = N_VNew_Serial(FPd->Npoints);
                N_VConst_Serial(1.0, scale);        /* no scaling */
                kin_mem=NULL;
				u = N_VNew_Serial(FPd->Npoints);
                fp_cp_array_to_N_Vector(FPd->out,u,FPd->Npoints);

				kin_mem = KINCreate();
                FPd->kin_mem=kin_mem;
                  /* Set number of prior residuals used in Anderson acceleration */
                KIN_sasfit_fp_configure(kin_mem,FPd);

                flag = KINInit(kin_mem,FP_step_kinsol,u);
                if (FPd->PrintProgress)sasfit_out("KINInit(flag)=%d\n",flag);

				flag = KINSptfqmr(kin_mem, 0);
                if (FPd->PrintProgress) sasfit_out("KINSptfqmr(flag)=%d\n",flag);

				flag = KINSol(kin_mem,u,FPd->KINSolStrategy,scale, scale);
                if (flag !=  KIN_SUCCESS && flag != KIN_INITIAL_GUESS_OK && flag != KIN_LINESEARCH_NONCONV) FPd->failed = 1;
                if (FPd->PrintProgress) sasfit_out("KINSol(flag)=%d\n",flag);
                KINGetFuncNorm(kin_mem, &err);
                if (FPd->PrintProgress) sasfit_out("err: %lg\n:",err);

				N_VDestroy_Serial(u);
                N_VDestroy_Serial(scale);
				KINFree(&kin_mem);
                if (FPd->PrintProgress) sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
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
                //did fp_addColumnToMatrixByShifting succeed?:
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
                for (j=0; j < dimensionOfVectorSpace; j++) {
                    gsl_vector_set(x,j,FPd->out[j]);
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
			while (FPd->it < FPd->maxsteps && err > FPd->relerror && FPd->interrupt == 0 && FPd->failed==0) {
				n++;
				check_interrupt_fp(FPd);
                    switch (FPd->mixstrategy) {
                        case mix_const:
                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_err:
                                    alpha=(1-FPd->mixcoeff)*exp(-log10(err/FPd->relerror))+FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                        case mix_reward_penalty:
                                    if (err < errold) {
                                        iloop++;
                                        iloop = (((1.0*iloop)/(1.0*NITSTEP) >= 1)?NITSTEP:iloop);
                                    } else {
                                        iloop = (((iloop-1) >= 0) ? (iloop-1) : (0));
                                    }
                                    alpha=FPd->mixcoeff+(1.0*iloop)/(1.0*NITSTEP)*(1-FPd->mixcoeff);
                                    beta=alpha;
                                    gama=alpha;
                                    break;
                       default:

                                    alpha=FPd->mixcoeff;
                                    beta=alpha;
                                    gama=alpha;
                                    break;

                    }
    //Increment current dimension of Krylov space
    //(This equals the number of states used for AA mixing)
    if (dimensionOfKrylovSpace < maximalDimensionOfKrylovSpace)
      dimensionOfKrylovSpace++;
    else
      isMaximalDimensionOfKrylovSpaceReached = 1;
    //printf("dimensionOfKrylovSpace %d\n", dimensionOfKrylovSpace);
    //Apply x_n = G(x)
    err = fixpointOperatorFP_GSL_API(x, FPd, x_n);
    if (err != err) {
      sasfit_out("detected NAN for precision of FP solution: %g\n",err);
      FPd->failed = 1;
      for (j=0; j < FPd->Npoints; j++) {
        FPd->out[j]=1.0e-7;
        gsl_vector_set(x,j,1.0e-7);
      }
    }
    if ((n % 1000)==0 && FPd->PrintProgress == 1)
         sasfit_out("iterations: %d, calls of FP_step=%d, err=%g, alpha=%g\n",n,FPd->it,err,alpha);
    //First we save the new value x_n in Matrix K (GSL sub will overwrite it)
    //As long as current dimensionOfKrylovSpace is smaller than what defined as maximum,
    //we extend the vector to the matrix...
    if (!isMaximalDimensionOfKrylovSpaceReached){
      K = fp_addColumnToMatrixByExtension(K, x_n);
      if (!K){
        sasfit_out("Couldn't extend K matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }
    }else{ //... we add by replacing the oldest
      matrixShiftReturnValue = fp_addColumnToMatrixByShifting(K, x_n);
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
      D = fp_addColumnToMatrixByExtension(D, r_n);
      if (!D){
        sasfit_out("Couldn't extend D matrix, exiting AAA");
        break; //We exit the while Loop, not all memory is freed
      }

    }else{
      matrixShiftReturnValue = fp_addColumnToMatrixByShifting(D, r_n);
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
      W = fp_calculateW(dimensionOfKrylovSpace);
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
    fp_cp_gsl_vector_to_array(x,FPd->out,FPd->Npoints);

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

  //                  sasfit_out("up to now the number of FP_step calls are: %d\n",FPd->it);
                break;
    }
//        sasfit_out("it %d, interrupt: %d\n",FPd->it,FPd->interrupt);
//    } while (FPd->failed == 0 && FPd->it < FPd->maxsteps  && FPd->interrupt == 0);
//    sasfit_out("number of OZ_step calls are: %d\n",FPd->it);
   if (FPd->failed == 1) {
       sasfit_err("Failure in FPstep. FP algorithm does not converge.\n");
       for (j=0; j < FPd->Npoints; j++) {
            FPd->out[j]=1.0e-7;
       }
       return TCL_ERROR;
   }
   if (FPd->it >= FPd->maxsteps) {
       sasfit_err("FP algorithm reached maximum allowed number of steps\n");
       return TCL_ERROR;
   }
   if (FPd->interrupt == 1) {
       sasfit_err("FP algorithm was interrupted by user.\n");
       return TCL_ERROR;
   }
   return TCL_OK;
}


double FP_step(sasfit_fp_data *FPd) {
	scalar chi2,GNorm2, KLD, JSD, sumIn, sumOut, M, P, Q;
	EM_param_t *EMparam;
    char sBuffer[256];
	int j;
	FILE *fptr;
    FPd->it=FPd->it+1;
//	sasfit_out("123 %d\n",FPd->it);

//	sasfit_out("123456\n");
	FPd->Chi2Norm = FPd->FP_Op(FPd->FPstructure);

//	sasfit_out("123456789 %lg \n",FPd->Chi2Norm);

	sumIn=0;
	sumOut=0;
    for (j=0; j < FPd->Npoints; j++) {
		sumIn += FPd->in[j];
		sumOut += FPd->out[j];
    }

    GNorm2 = 0;
	KLD=0;
	JSD=0;
	FPd->Entropy=0;
	for (j=0; j < FPd->Npoints; j++) {
		P=FPd->in[j]/sumIn;
		Q=FPd->out[j]/sumOut;
		M=0.5*(P+Q);
		if (P>0 && Q>0) {
            KLD+=P*log(fabs(P/Q));
            JSD+=P*log(fabs(P/M))/2.+Q*log(fabs(Q/M))/2.;
            FPd->Entropy-= -FPd->out[j]*log(FPd->out[j]*FPd->Npoints/sumOut)+FPd->out[j]-sumOut/FPd->Npoints;
		}
        GNorm2=gsl_pow_2(FPd->out[j]-FPd->in[j])+GNorm2;

    }
    FPd->gNorm= sqrt(GNorm2);
    FPd->KLD= KLD;
    FPd->JSD= JSD;

    FPd->Sum1stDeriv=gsl_pow_2((FPd->out[1]-FPd->out[0]));
    FPd->Sum2ndDeriv=gsl_pow_2((2*FPd->out[1]-FPd->out[0]-FPd->out[2]));
	for (j=1; j < FPd->Npoints-1; j++) {
		FPd->Sum1stDeriv+=gsl_pow_2((FPd->out[j]-FPd->out[j-1]));
        FPd->Sum2ndDeriv+=gsl_pow_2((2*FPd->out[j]-FPd->out[j-1]-FPd->out[j+1]));
    }
    FPd->Sum1stDeriv+=gsl_pow_2((FPd->out[FPd->Npoints-1]-FPd->out[FPd->Npoints-2]));
    EMparam = (EM_param_t *) FPd->FPstructure;
/*
    fptr = fopen("c:/temp/SASfit.dat","a+");
    fprintf(fptr,"it.\t%d\teps\t%16.14lg\tchi2\t%16.14lg\tGtest\t%16.14lg\tchi2test\t%16.14lg\tJSDtest\t%16.14lg\tKLD\t%16.14lg\tJSD\t%16.14lg\tSum1st\t%16.14lg\tSum2nd\t%16.14lg\tEntropy\t%16.14lg\n",
                  FPd->it, GNorm2, FPd->Chi2Norm,EMparam->Gtest,EMparam->chi2test,EMparam->JSDtest, FPd->KLD, FPd->JSD,FPd->Sum1stDeriv, FPd->Sum2ndDeriv,FPd->Entropy);
    fclose(fptr);
*/
/*
    sasfit_out("it. %d KLD %lg JSD %lg\n",FPd->it, FPd->KLD, FPd->JSD);
*/
    if (FPd->KINSetPrintLevel == 4) {
        sprintf(sBuffer,"storeOZstepinfo \"it:%d\tgNorm:%lg\tKLD:%lg\tJSD%lg\tchi2:\"",FPd->it,FPd->gNorm, FPd->KLD, FPd->JSD,FPd->Chi2Norm);
        Tcl_EvalEx(FPd->interp,sBuffer,-1,TCL_EVAL_DIRECT);
    }

//    fp_cp_array_to_array(FPd->out, FPd->in, FPd->Npoints);
//    sasfit_out("FP_step: ");
//    for (j=0;j<10;j++) sasfit_out("%lg ",FPd->in[j]);
//    sasfit_out("\n");

    return FPd->gNorm;
}


static int FP_step_kinsol(N_Vector cc, N_Vector fval, void *FPdata) {
    sasfit_fp_data *FPd;
    int i;
    //This definition must be here (and not later), otherwise the macro G in next line is undefined
    FPd = (sasfit_fp_data*) FPdata;
    fp_cp_N_Vector_to_array(cc,FPd->out,FPd->Npoints);
    FP_step(FPd);
    fp_cp_array_diff_N_Vector_to_N_Vector(FPd->out,cc,fval,FPd->Npoints);
    check_interrupt_fp(FPd);
    if (FPd->gNorm != FPd->gNorm) {
//        for (i=0;i<NP;i++) G[i] = 0;
        return -1;
    }
    if (FPd->interrupt != 0){
        return -1;
    } else {
        return 0;
    }
}

static int FP_step_kinsolFP(N_Vector cc, N_Vector fval, void *FPd_structure) {
    sasfit_fp_data *FPd;
    int i;
    double Norm;
    //This definition must be here (and not later), otherwise the macro G in next line is undefined
    FPd = (sasfit_fp_data*) FPd_structure;
    fp_cp_N_Vector_to_array(cc,FPd->out,FPd->Npoints); //This line can in principle be omitted, but it is cleaner to have it....
                                   //....in case Kinsol and OZd get 'out of sync' (due to whatever reason)
    Norm=FP_step(FPd);
    fp_cp_array_to_N_Vector(FPd->out,fval,FPd->Npoints);
    check_interrupt_fp(FPd);
    if (FPd->gNorm != FPd->gNorm) {
        for (i=0;i<FPd->Npoints;i++) FPd->out[i] = 1e-7;
        return -1;
    }
    if (FPd->interrupt != 0){
        return -1;
    } else {
        return 0;
    }
}




//Release of memory used for computation
int FP_free (sasfit_fp_data *FPd) {
    sasfit_timer_destroy(&FPd->tm);
    return TCL_OK;
}

int FP_solver (sasfit_fp_data *FPd) {
    int i,status;
//    sasfit_out("algorithm: %d\n",FPd->root_algorithm);
    switch (FPd->root_algorithm) {
        case Picard_iteration:
                status = FP_solver_by_iteration(FPd,Picard_iteration);
                break;
        case Mann_iteration:
                status = FP_solver_by_iteration(FPd,Mann_iteration);
                break;
        case Ishikawa_iteration:
                status = FP_solver_by_iteration(FPd,Ishikawa_iteration);
                break;
        case Noor_iteration:
                status = FP_solver_by_iteration(FPd,Noor_iteration);
                break;
        case S_iteration:
                status = FP_solver_by_iteration(FPd,S_iteration);
                break;
        case SP_iteration:
                status = FP_solver_by_iteration(FPd,SP_iteration);
                break;
        case CR_iteration:
                status = FP_solver_by_iteration(FPd,CR_iteration);
                break;
        case PicardS_iteration:
               status =  FP_solver_by_iteration(FPd,PicardS_iteration);
                break;
        case PMH_iteration:
                status = FP_solver_by_iteration(FPd,PMH_iteration);
                break;
        case MannII_iteration:
                status = FP_solver_by_iteration(FPd,MannII_iteration);
                break;
        case Krasnoselskij_iteration:
                status = FP_solver_by_iteration(FPd,Krasnoselskij_iteration);
                break;
        case Sstar_iteration:
                status = FP_solver_by_iteration(FPd,Sstar_iteration);
                break;
        case Steffensen2_iteration:
                status = FP_solver_by_iteration(FPd,Steffensen2_iteration);
                break;
        case Steffensen4_iteration:
                status = FP_solver_by_iteration(FPd,Steffensen4_iteration);
                break;
        case AndersonAcc:
               status = FP_solver_by_iteration(FPd,AndersonAcc);
                break;
        case Broyden:
                status = FP_solver_by_gsl_multroot(FPd,Broyden);
                break;
        case dNewton:
                status = FP_solver_by_gsl_multroot(FPd,dNewton);
                break;
        case Hybrid:
                status = FP_solver_by_gsl_multroot(FPd,Hybrid);
                break;
        case Hybrids:
                status = FP_solver_by_gsl_multroot(FPd,Hybrids);
                break;
        case BIGGS_ANDREWS:
                status = FP_solver_by_iteration(FPd,BIGGS_ANDREWS);
                break;
        case KINSOLFP:
                status = FP_solver_by_iteration(FPd,KINSOLFP);
                break;
        case GMRES:
                status = FP_solver_by_iteration(FPd,GMRES);
                break;
        case FGMRES:
               status =  FP_solver_by_iteration(FPd,FGMRES);
                break;
        case TFQMR:
                status = FP_solver_by_iteration(FPd,TFQMR);
                break;
        case BiCGSTAB:
                status = FP_solver_by_iteration(FPd,BiCGSTAB);
                break;
        default:
                sasfit_err("this algorithm is planned to be implemented\n");
    }

    if (FPd->PrintProgress) sasfit_out("Needed %d calls to FP_step\n",FPd->it);

//    Tcl_EvalEx(OZd->interp,"set OZ(progressbar) 1",-1,TCL_EVAL_DIRECT);
    Tcl_EvalEx(FPd->interp,"update",-1,TCL_EVAL_DIRECT);

    return status;
}
