/* polyroots.c -- see polyroots.h */
#include "include/polyroots.h"
#include <pthread.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>

/* GSL's default error handler prints a message and calls abort() on
 * any internal error -- which, unlike a normal error return,
 * terminates the *entire process* (every thread), not just the one
 * that hit the error. Confirmed empirically: gsl_poly_complex_solve()
 * given a zero leading coefficient reliably aborts an unmodified GSL
 * build (exit code 134 / SIGABRT), even though it also returns an
 * ordinary GSL_EINVAL status code -- the abort happens first, inside
 * the call, so a caller-side `if (status != GSL_SUCCESS)` check can
 * never run. gsl_set_error_handler_off() makes GSL functions return
 * that status code instead of aborting, which is what actually makes
 * the error-handling code below reachable.
 *
 * This is process-wide, GSL-global configuration, not specific to
 * this library -- see the "Thread safety" section of README.md for
 * what that means for a host application that also uses GSL
 * elsewhere. It is installed exactly once, no matter how many threads
 * call quartic_roots() concurrently or how many times, via
 * pthread_once(): the alternative of calling
 * gsl_set_error_handler_off() unconditionally on every call would
 * itself be a data race (concurrent unsynchronized writes to GSL's
 * internal global handler pointer), even though every such write
 * would store the same value.                                        */
static pthread_once_t gGslErrorHandlerOnceGuard = PTHREAD_ONCE_INIT;

static void disableGslAbortingErrorHandler(void)
{
    gsl_set_error_handler_off();
}

int quartic_roots(double quarticCoeff4, double quarticCoeff3, double quarticCoeff2,
                   double quarticCoeff1, double quarticCoeff0, double complex rootsOut[4])
{
    pthread_once(&gGslErrorHandlerOnceGuard, disableGslAbortingErrorHandler);

    /* Still worth rejecting this case ourselves: it is the single most
     * likely way real (non-adversarial) input from rmsa.c could reach
     * GSL in an invalid state, and returning our own error code here
     * avoids relying on -- and matching error codes against -- GSL's
     * internal validation for what is a simple, common case.          */
    if (quarticCoeff4 == 0.0)
        return -1;

    double coefficients[5] = {quarticCoeff0, quarticCoeff1, quarticCoeff2, quarticCoeff3, quarticCoeff4};
    double rootsInterleaved[8]; /* re0,im0, re1,im1, re2,im2, re3,im3 */

    gsl_poly_complex_workspace *workspace = gsl_poly_complex_workspace_alloc(5);
    if (!workspace) return -2; /* e.g. out of memory; GSL_ENOMEM with the handler disabled */

    int status = gsl_poly_complex_solve(coefficients, 5, workspace, rootsInterleaved);
    gsl_poly_complex_workspace_free(workspace);
    if (status != GSL_SUCCESS) return -3; /* e.g. internal QR iteration failed to converge */

    for (int i = 0; i < 4; i++)
        rootsOut[i] = rootsInterleaved[2 * i] + rootsInterleaved[2 * i + 1] * I;
    return 0;
}

