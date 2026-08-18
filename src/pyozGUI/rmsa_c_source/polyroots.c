#include "include/polyroots.h"
#include <pthread.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>

static pthread_once_t gGslErrorHandlerOnceGuard = PTHREAD_ONCE_INIT;

static void disableGslAbortingErrorHandler(void)
{
    gsl_set_error_handler_off();
}

int quartic_roots(double quarticCoeff4, double quarticCoeff3, double quarticCoeff2,
                   double quarticCoeff1, double quarticCoeff0, double complex rootsOut[4])
{
    pthread_once(&gGslErrorHandlerOnceGuard, disableGslAbortingErrorHandler);

    if (quarticCoeff4 == 0.0)
        return -1;

    double coefficients[5] = {quarticCoeff0, quarticCoeff1, quarticCoeff2, quarticCoeff3, quarticCoeff4};
    double rootsInterleaved[8];

    gsl_poly_complex_workspace *workspace = gsl_poly_complex_workspace_alloc(5);
    if (!workspace) return -2;

    int status = gsl_poly_complex_solve(coefficients, 5, workspace, rootsInterleaved);
    gsl_poly_complex_workspace_free(workspace);
    if (status != GSL_SUCCESS) return -3;

    for (int i = 0; i < 4; i++)
        rootsOut[i] = rootsInterleaved[2 * i] + rootsInterleaved[2 * i + 1] * I;
    return 0;
}
