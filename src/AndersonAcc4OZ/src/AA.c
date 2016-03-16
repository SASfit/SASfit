/*
*-----------------------------------------------------------------------*
|                                                                       |
| Copyright (c) 2016 by Paul Scherrer Institute (http://www.psi.ch)     |
|                                                                       |
| Author: A. Studer, Scientific Computing, PSI                          |
| Contributor: J. Kohlbrecher, Neutron Laboratory, PSI                  |
*-----------------------------------------------------------------------*
GSL based implementation of the 
Anderson Acceleration Algorithm.
*/
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>


//compile: gcc -c AA.c
//with symbols (e.g. to use valgrind):  gcc -c -g AA.c
//link: (against GSL, BLAS): gcc AA.o -lgsl -lgslcblas -lm
//both in one line: gcc -c AA.c;gcc AA.o -lgsl -lgslcblas -lm
//Check for memory leaks:
//$ valgrind --tool=memcheck --leak-check=full ./a.out 



//helper functions for debugging
void printMatrix(const gsl_matrix* A){
  int i,j;
  for(i = 0; i < A->size1; i++){
    for(j = 0; j < A->size2; j++)
      printf("%f \t", gsl_matrix_get(A, i, j) );
    printf("\n");
  }
}

void printVector(const gsl_vector* x){
  int i;
  for (i = 0; i < x->size; i++)
    printf("%f ", gsl_vector_get(x, i) );
  printf("\n");
}

void fixPointOperator(gsl_vector* x, gsl_vector* x_n){
  //Dummy, but do not modify x here
  gsl_vector_memcpy(x_n, x);  
  gsl_vector_scale(x_n, 0.5);
  gsl_vector_add_constant(x_n, 2.0);
}

gsl_matrix* hilbertMatrix(int dim){
  gsl_matrix* H = gsl_matrix_alloc(dim, dim);
  int i,j;
  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
      gsl_matrix_set(H, i, j, 1.0 / (double)(i + j + 1));
    
  return H;
}

void fixPointOperator2(gsl_vector* x, gsl_vector* x_n, gsl_matrix* Hs){
  //Dummy, but do not modify x here
  gsl_vector_memcpy(x_n, x);
  gsl_blas_dgemv(CblasNoTrans, 1.0, Hs, x, 0.0, x_n);
  gsl_vector_add_constant(x_n, -2.0);
}
/*The above solution was checked against python:
from scipy.linalg import hilbert
import numpy as np
H=hilbert(10)
b = np.zeros(10)
b += 2
x = np.linalg.solve(H - np.eye(10), b)
*/
void fixPointOperator3(gsl_vector* x, gsl_vector* x_n){
  //Dummy, but do not modify x here
  gsl_vector_memcpy(x_n, x);
  int i;
  for(i = 0; i < x_n->size; i++){
    float c = cos(gsl_vector_get(x_n, i));
    //printf("cos %f\n", c);
    gsl_vector_set(x_n, i, c);
  }
}
/*Above checked against python:
import numpy as np
from scipy import optimize
def func(x):
  return np.cos(x)
optimize.fixed_point(func, [1.0,1.0])

Interestingly, the alg does not work if only
one component (0) of the vector is defined as
x[0] -> cos(x[0])
x[i] -> x[i];  i > 0
*/
void fixPointOperator4(gsl_vector* x, gsl_vector* x_n, gsl_matrix* Hs){
  //Dummy, but do not modify x here
  gsl_vector_memcpy(x_n, x);
  gsl_blas_dgemv(CblasNoTrans, 1.0, Hs, x, 0.0, x_n);
  gsl_vector_add_constant(x_n, -2.0);
  int i;
  for(i = 0; i < x_n->size; i++){
    float c = cos(gsl_vector_get(x_n, i));
    //printf("cos %f\n", c);
    gsl_vector_set(x_n, i, c);
  }
}
/*Above checked against python:
import numpy as np
from scipy import optimize
from scipy.linalg import hilbert
dimensionOfVectorSpace = 10
H=hilbert(dimensionOfVectorSpace)
def func(x):
  return np.cos(np.dot(H,x) -2.0)
x_0 = np.ones(dimensionOfVectorSpace)
optimize.fixed_point(func, x_0)

*/

//end helper functions

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


//A -> [A,c]
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
    printf("Cannot add column vector to matrix due to dimension problems \n");
    printf("Number of rows in matrix: %zu \n", numberOfRows);
    printf("Vector size %zu \n", vectorSize);
    printf("Exiting from addColumnToMatrixByExtension... \n");
    exit(-1);
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
void addColumnToMatrixByShifting(gsl_matrix* A, const gsl_vector* c){
  //Find new Matrix dimensions
  size_t numberOfRows = A->size1;
  size_t vectorSize = c->size;
  
  if (vectorSize != numberOfRows){
    printf("Cannot add column vector to matrix due to dimension problems \n");
    printf("Number of rows in matrix: %zu \n", numberOfRows);
    printf("Vector size %zu \n", vectorSize);
    printf("Exiting from addColumnToMatrixByShifting...\n");
    exit(-1);
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
  //Finally add the new column...
  es = gsl_matrix_set_col(A, numberOfColumns -1, c);
  //...and deallocate buffer
  gsl_vector_free(v);
}


int main(void){
  //The name of the variables and the implementation of the algorithm itself
  //follow closely the PhD of Peng Ni, Worcester Polytechnic Institute. Therein, AAA
  //is used in a DFT context. The 'Method of elimination' is chosen here, the reason
  //for this choice is good performance and a good condition number (see section
  //'method comparison' in PhD). However, from a theoretical point of view, I am not sure
  //on what extend the DFT results (where the fix point operator is the Hamiltonian
  //describing a Hartree-Fock step for a many-electron system, eq 3.2) 
  //can be transferred to OZ, but benchmarking this algorithm against others gives good results.
  //........................................................................................

  int maximalNumberOfIterations = 30;
  int iteration;
  //This is the number of elements in the vector space used
  //for anderson mixing. Both the residuals and the states
  //usd for mixing are in the Krylov subsapce of the problem,
  //hence the name
  int maximalDimensionOfKrylovSpace = 7;
  int isMaximalDimensionOfKrylovSpaceReached = 0;
  int dimensionOfKrylovSpace = 0;
  //This is the dimension of the vector space where 
  //the fiy point operator lives
  int dimensionOfVectorSpace = 10;
  //Parameter of the algorithm
  float b = 1.0;
  //current state
  gsl_vector* x;
  //new (updated state), x_n = G(x), where G is fix Point Operator
  gsl_vector* x_n;
  //vector to hold residuum
  gsl_vector* r;
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
  //negarive of last entry in D
  gsl_vector* d;
  
  //GSL internal
  int gslReturnValue = 0;
  
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
  //init x to zero
  gsl_vector_set_zero(x);
  //gsl_vector_add_constant(x, 1.0);
  gsl_vector_set_zero(x_n);
  gsl_vector_set_zero(x_A);

  //Next line is only for testing
  gsl_matrix* linearPartOfFixPointOperator = hilbertMatrix(dimensionOfVectorSpace);
  //Main Loop of algorithm. x must be initialized to x_0 at this stage
  for(iteration = 0; iteration < maximalNumberOfIterations; iteration++){
    printf("********************************************************\n");
    printf("Iteration %d\n", iteration);
    //Increment current dimension of Krylov space
    //(This equals the number of states used for AA mixing)
    if (dimensionOfKrylovSpace < maximalDimensionOfKrylovSpace)
      dimensionOfKrylovSpace++;
    else
      isMaximalDimensionOfKrylovSpaceReached = 1;
    printf("dimensionOfKrylovSpace %d\n", dimensionOfKrylovSpace);
    //Apply x_n = G(x)
    //fixPointOperator(x, x_n); //Variante 1 (affin, diagonal)
    //fixPointOperator2(x, x_n, linearPartOfFixPointOperator); //Variante 2, affine non-diagonal
    //fixPointOperator3(x, x_n); //Variante 3, diagonal, non-linear
    fixPointOperator4(x, x_n, linearPartOfFixPointOperator); //Variante 4, non-diagonal, non-linear
    //First we save the new value x_n in Matrix K (GSL sub will overwrite it)
    //As long as current dimensionOfKrylovSpace is smaller than what defined as maximum,
    //we extend the vector to the matrix...
    if (!isMaximalDimensionOfKrylovSpaceReached){
      K = addColumnToMatrixByExtension(K, x_n);
    }else{ //... we add by replacing the oldest
      addColumnToMatrixByShifting(K, x_n);
    }
    //printf("K: \n"); printMatrix(K);
    
    //Calculate residuum r = x_n - x, result is stored in x_n
    gslReturnValue = gsl_vector_sub(x_n, x); //printf("%d \n", gslReturnValue);
    r = x_n; //r ist just a pointer without allocated memory, just used for better naming
    r_norm = gsl_blas_dnrm2(r);
    if (r_norm < 0.001){
      printf("********************************************************\n");
      printf("Converged. x is \n");
      printVector(x);
      exit(0);
    }
    //save residuum r in Matrix D
    if (!isMaximalDimensionOfKrylovSpaceReached){
      D = addColumnToMatrixByExtension(D, r);
    }else{
      addColumnToMatrixByShifting(D, r);
    }
    //printf("D: \n"); printMatrix(D);
    
    if (dimensionOfKrylovSpace == 1){
      gsl_vector_memcpy (x, x_n);
      continue; //If D consists of one vector only, we skip the least square fit
                //(The constraint sum a_i =1 implies that there must be at least two vectors)
    }
    //Since we updated the Matrix D, we are now ready to solve the
    //constrained least square optimization problem
    if (!isMaximalDimensionOfKrylovSpaceReached)
      W = calculateW(dimensionOfKrylovSpace);
    //else W needs not be recalculated since it remains constant (in size)
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
    //printf("W:\n"); printMatrix(W);
    
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, D, W, 0.0, D_reduced);
    //Ready to solve min || D_reduced a_reduced + r || 
    //            a_reduced
    //QR decomposition
    gsl_vector_set_zero(tau);
    gslReturnValue = gsl_linalg_QR_decomp(D_reduced, tau);
    gsl_vector_set_zero(a_reduced);
    //d = -r
    gsl_vector_memcpy(d,r);
    //mind the sign in the lsf problem: D_reduced*a_reduced = -d
    gsl_vector_scale(d, -1.0);
    gslReturnValue = gsl_linalg_QR_lssolve(D_reduced, tau, d, a_reduced, res_opt);
    //printf("norm if least square fit difference vector: %f\n", gsl_blas_dnrm2(r));
    //Now a_reduced holds the solution of the unconstrained problem, next we want a
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
    gsl_blas_dgemv(CblasNoTrans, (b - 1.0), D, a, 0.0, summand_2);
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

  return 0;
}
