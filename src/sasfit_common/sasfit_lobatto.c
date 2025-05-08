#include "include/sasfit_common.h"
#include "include/sasfit_function.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "include/lobatto.h"

// Error-handling macro for exceeding evaluations
#define THROW_ERROR(msg) \
    { sasfit_err("%s\n", msg);  }

#define THROW_WARNING(msg) \
    { sasfit_out("%s\n", msg); }
// Constants used in the algorithm
#define ALPHA   0.816496580927726032732428024902 // 1.0 / sqrt(3.0 / 2.0)
#define BETA    0.447213595499957939281834733746 // 1.0 / sqrt(5.0)

/**
 * Perform a single step of the Gauss-Lobatto integration
 *
 * @param f       Function to integrate
 * @param a       Lower integration limit
 * @param b       Upper integration limit
 * @param fa      Function value at the lower limit
 * @param fb      Function value at the upper limit
 * @param neval   Pointer to count of function evaluations
 * @param maxeval Maximum allowed evaluations
 * @param acc     absolute required accuracy
 * @param racc    relative required accuracy
 *
 * @return The integral over [a, b]
 */
double GaussLobattoIntStep(sasfit_func_one_void f, double a, double b, double fa, double fb,
                           size_t* neval, size_t maxeval, double acc, double racc, double *error, int maxdepth, void *param) {
    // Check for max evaluations

    double h = (b - a) / 2.0;
    double m = (a + b) / 2.0;

    // Compute abscissa points
    double mll = m - ALPHA * h;
    double ml = m - BETA * h;
    double mr = m + BETA * h;
    double mrr = m + ALPHA * h;

    // Compute function values at these points
    double fmll = f(mll,param);
    double fml = f(ml,param);
    double fm = f(m,param);
    double fmr = f(mr,param);
    double fmrr = f(mrr,param);

    *neval += 5;

    // Compute 4-point and 7-point integrals
    volatile double integral2 = (h / 6.0) * (fa + fb + 5.0 * (fml + fmr));
    volatile double integral1 =   (h* 11/210.) * (fa + fb)
                                + (h* 72/245.) * (fmll + fmrr)
                                + (h*125/294.) * (fml + fmr)
                                + (h* 16/35. ) * fm;
    /*
                                (h / 1470.0) * (77.0 * (fa + fb)
                                       + 432.0 * (fmll + fmrr)
                                       + 625.0 * (fml + fmr)
                                       + 672.0 * fm);
*/
    volatile double estacc = fabs(integral1 - integral2);
    *error = estacc;
    volatile double dist  =  acc + estacc;
    if (acc==0 && racc !=0) dist=acc;
    volatile double rdist = racc + estacc/integral1;

    if (*neval >= maxeval) {
        THROW_ERROR("Maximum number of evaluations reached in GaussLobatto");
        return integral1;
    }
    // Check for convergence or invalid intervals
    //if ((dist == acc && rdist == racc) || mll <= a || b <= mrr) {
    if ((estacc <= gsl_max(acc,racc*fabs(gsl_min(integral1,integral2))) ) || mll <= a || b <= mrr) {
        if (!(m > a && b > m)) {
            THROW_WARNING("Integration reached an interval with no more machine numbers");
        }
        return integral1;
    } else {
        // limit for maximal recursive depth
        if (maxdepth > (log(maxeval)/log(6))) return integral1;
        // Recursive integration
        return GaussLobattoIntStep(f, a, mll, fa, fmll, neval, maxeval, acc, racc, error, maxdepth+1, param) +
               GaussLobattoIntStep(f, mll, ml, fmll, fml, neval, maxeval, acc, racc, error, maxdepth+1, param) +
               GaussLobattoIntStep(f, ml, m, fml, fm, neval, maxeval, acc, racc, error, maxdepth+1, param) +
               GaussLobattoIntStep(f, m, mr, fm, fmr, neval, maxeval, acc, racc, error, maxdepth+1, param) +
               GaussLobattoIntStep(f, mr, mrr, fmr, fmrr, neval, maxeval, acc, racc, error, maxdepth+1, param) +
               GaussLobattoIntStep(f, mrr, b, fmrr, fb, neval, maxeval, acc, racc, error, maxdepth+1, param);
    }
}

/**
 * Compute the Gauss-Lobatto integral
 *
 * @param f       Function to integrate
 * @param a       Lower integration limit
 * @param b       Upper integration limit
 * @param abstol  Absolute tolerance
 * @param maxeval Maximum allowed evaluations
 *
 * @return The integral over [a, b]
 */
double sasfit_GaussLobattoInt(sasfit_func_one_void f, double a, double b, double abstol, double reltol, size_t maxeval, double *error, void *param) {
    size_t neval = 0;
    double acc  = abstol / pow(2.0, -52); // Approximation of epsilon
    double racc = reltol / pow(2.0, -52); // Approximation of epsilon

//    return GaussLobattoIntStep(f, a, b, f(a,param), f(b,param), &neval, maxeval, acc, racc, error, param);
    return GaussLobattoIntStep(f, a, b, f(a,param), f(b,param), &neval, maxeval, abstol, reltol, error, 0, param);
}


// Infinite interval transformations
double infiniteLeft(double t, sasfit_func_one_void fcn, void *param) {
    return fcn(log(t),param) / t;
}
double infiniteRight(double t, sasfit_func_one_void fcn, void *param) {
    return fcn(-log(t),param) / t;
}
double infiniteBoth(double t, sasfit_func_one_void fcn, void *param) {
    return fcn(tan(t),param) / (cos(t) * cos(t));
}

// Function to evaluate adaptive Lobatto quadrature
double adaptiveLobatto(sasfit_func_one_void f, double a, double b, LobattoParams params, int *fcnEvals, int *iter, void *param) {
    // Constants for Lobatto integration
    const double alpha = sqrt(2.0 / 3.0);
    const double beta = sqrt(1.0 / 5.0);

    // Initialize parameters
    int m = params.parts;
    double Q = 0;
    double minH = (b - a) * DBL_EPSILON / 1024.0;

    *fcnEvals = 0;
    *iter = 0;

    // Grid initialization
    double *A = (double *)malloc(m * sizeof(double));
    double *B = (double *)malloc(m * sizeof(double));
    double *H = (double *)malloc(m * sizeof(double));

    // Initial grid points
    for (int i = 0; i < m; i++) {
        A[i] = a + (b - a) * i / m;
        B[i] = a + (b - a) * (i + 1) / m;
        H[i] = (B[i] - A[i]) / 2.0;
    }

    // Adaptive iteration loop
    while (1) {
        (*iter)++;

        // Quadrature values
        double integral1 = 0, integral2 = 0;
        for (int i = 0; i < m; i++) {
            double mid = (A[i] + B[i]) / 2.0;
            double fm = f(mid,param);
            double fml = f(mid - beta * H[i],param);
            double fmr = f(mid + beta * H[i],param);
            double fmll = f(mid - alpha * H[i],param);
            double fmrr = f(mid + alpha * H[i],param);

            // Four-point Lobatto formula
            double Q1 = (H[i] / 6.0) * (f(A[i],param) + 5 * (fml + fmr) + f(B[i],param));

            // Seven-point Lobatto-Kronrod formula
            double Q2 = (H[i] / 1470.0) * (77 * (f(A[i],param) + f(B[i],param))
                                           + 432 * (fmll + fmrr)
                                           + 625 * (fml + fmr) + 672 * fm);

            // Accumulate results
            integral1 += Q1;
            integral2 += Q2;

            // Error estimate
            double diff = fabs(Q2 - Q1);
            if (diff > params.atol+params.rtol*fabs(Q2)/2.) {
                // Subdivide interval
                double newH = H[i] / 2.0;
                H[i] = newH;
                A[i] = mid - newH;
                B[i] = mid + newH;
                if (newH < minH) THROW_ERROR("Reached minimum resolution without convergence");
            }
            (*fcnEvals) += 5;
            if (*fcnEvals > params.maxFcnEvals) THROW_ERROR("Exceeded maximum function evaluations");
        }

        // Convergence check
        if (fabs(integral2 - integral1) <= params.atol + params.rtol*fabs(integral2)/2.) {
            Q = integral2;
            break;
        }
        if (*iter > params.maxParts) THROW_ERROR("Exceeded maximum partitions allowed");
    }

    // Clean up
    free(A);
    free(B);
    free(H);
    return Q;
}


scalar GaussLobattoInt(sasfit_func_one_void f,
            scalar a, scalar b,
            scalar abstol,
            scalar reltol,
            size_t maxeval,
			scalar *error,
            void *param) {

    // return GaussLobattoInt(f,a,b,abstol,reltol,maxeval,error,param);

    int iter, fcnEvals;
    LobattoParams params;
    params.atol = abstol;
    params.atol = reltol;
    params.rtol = reltol;
    params.maxFcnEvals = maxeval;
    params.maxParts = maxeval/100;

    return adaptiveLobatto(f,a,b,params,&fcnEvals,&iter,param);
};
