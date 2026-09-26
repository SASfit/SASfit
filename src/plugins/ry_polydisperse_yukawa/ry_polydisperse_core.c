/*
 * ry_polydisperse_core.c -- see ry_polydisperse_core.h for the references.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_sf_bessel.h>

#include <stdlib.h>
#include <kinsol/kinsol.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_spgmr.h>
#include <sundials/sundials_types.h>
#include <sundials/sundials_config.h>

/* SUNDIALS 6 -> 7 API change: SUNContext_Create's first argument became a
 * typed SUNComm (with the SUN_COMM_NULL sentinel) instead of a plain void*.
 * SASfit ships SUNDIALS 7 (src/sundials7), and pyozGUI's Python binding
 * already uses the v7 form, core.SUNContext_Create(core.SUN_COMM_NULL);
 * this keeps the C source building against either. Everything else used
 * here (KINCreate/KINInit/KINSetMAA/KINSol/SUNLinSol_SPGMR and the N_Vector
 * serial API) is unchanged between the two versions. */
#if defined(SUNDIALS_VERSION_MAJOR) && SUNDIALS_VERSION_MAJOR >= 7
  #define RYP_SUNCONTEXT_CREATE(ctxp) SUNContext_Create(SUN_COMM_NULL, (ctxp))
#else
  #define RYP_SUNCONTEXT_CREATE(ctxp) SUNContext_Create(NULL, (ctxp))
#endif

#include "include/ry_polydisperse_core.h"

#define IDX3(sys,i,j,k)  (((size_t)(i)*(sys)->p + (j))*(size_t)(sys)->N + (k))

static int isPowerOfTwo(int v) { return v > 0 && (v & (v - 1)) == 0; }

void ryp_reset(ryp_system *sys)
{
    if (!sys) return;
    sys->solved = 0;
    sys->hasGammaGuess = 0;
    if (sys->gamma)
        memset(sys->gamma, 0, sizeof(double)*(size_t)sys->p*sys->p*sys->N);
}

const char *ryp_strerror(ryp_status st)
{
    switch (st) {
    case RYP_OK:              return "ok";
    case RYP_ERR_ALLOC:       return "out of memory";
    case RYP_ERR_PARAM:       return "invalid parameter";
    case RYP_ERR_GRID:        return "grid size invalid: N+1 must be a power of two";
    case RYP_ERR_NOCONVERGE:  return "Ornstein-Zernike iteration did not converge";
    case RYP_ERR_SINGULAR:    return "1 - C*rho is singular (unphysical parameters)";
    }
    return "unknown error";
}

/* ------------------------------------------------------------------ */
/* DST-I via a real radix-2 FFT of the odd extension.
 *
 * scipy's dst(x, type=1) is
 *     y[k] = 2 sum_{n=0}^{N-1} x[n] sin(pi (n+1)(k+1)/(N+1))
 * Building v of length M = 2(N+1) with
 *     v[0] = 0, v[1..N] = x, v[N+1] = 0, v[N+2..M-1] = -x reversed
 * makes v odd about 0 and M/2, so its DFT is purely imaginary and
 *     y[k] = -2 * Im( FFT(v)[k+1] ).
 * M = 2(N+1) is a power of two exactly when N+1 is -- which is why the
 * Python side defaults to N = 2^n - 1 (see its own __init__ comment); the
 * same restriction is enforced here in ryp_alloc().
 * ------------------------------------------------------------------ */
static void dst1(const double *in, double *out, int N, double *buf)
{
    const int M = 2 * (N + 1);
    int k;
    memset(buf, 0, sizeof(double) * (size_t)M);
    for (k = 0; k < N; k++) {
        buf[k + 1]         =  in[k];
        buf[M - 1 - k]     = -in[k];
    }
    gsl_fft_real_radix2_transform(buf, 1, (size_t)M);
    /* halfcomplex layout: buf[i] = Re, buf[M-i] = Im for 0<i<M/2 */
    for (k = 0; k < N; k++) {
        double im = buf[M - (k + 1)];
        out[k] = -im;   /* Im(V[k+1]) = -y[k]; the factor 2 is already in the DFT of the odd extension */
    }
}

/* forward radial transform, same normalisation as the Python
 * hankelTransform(): f_hat(q) = 4 pi / q * int r f(r) sin(qr) dr, evaluated
 * on the DST grid. Kept factor-for-factor identical so the two
 * implementations can be compared elementwise. */
static void radialForward(const ryp_system *sys, const double *f, double *fh, double *buf)
{
    int k;
    const int N = sys->N;
    double *tmp = buf + 2 * (N + 1);
    for (k = 0; k < N; k++) tmp[k] = f[k] * (double)(k + 1);
    dst1(tmp, fh, N, buf);
    for (k = 0; k < N; k++)
        fh[k] *= 2.0 * M_PI * sys->dr * sys->dr / sys->dq / (double)(k + 1);
}

static void radialBackward(const ryp_system *sys, const double *fh, double *f, double *buf)
{
    int k;
    const int N = sys->N;
    radialForward(sys, fh, f, buf);
    {
        const double s = sys->dq * sys->dq * sys->dq
                       / (8.0 * M_PI * M_PI * M_PI * sys->dr * sys->dr * sys->dr);
        for (k = 0; k < N; k++) f[k] *= s;
    }
}



/* Small square solve with a single right-hand side, used for the tiny
 * Anderson least-squares normal equations (m x m, m <= RYP_AA_DEPTH). */
static int solveSmallVec(int n, double *A, double *b)
{
    int i, j, k, piv;
    for (k = 0; k < n; k++) {
        double best = fabs(A[k*n + k]); piv = k;
        for (i = k + 1; i < n; i++) { const double v = fabs(A[i*n + k]);
            if (v > best) { best = v; piv = i; } }
        if (best < 1e-300) return -1;
        if (piv != k) {
            for (j = 0; j < n; j++) { double t = A[k*n+j]; A[k*n+j]=A[piv*n+j]; A[piv*n+j]=t; }
            { double t = b[k]; b[k] = b[piv]; b[piv] = t; }
        }
        for (i = k + 1; i < n; i++) {
            const double f = A[i*n + k] / A[k*n + k];
            if (f == 0.0) continue;
            for (j = k; j < n; j++) A[i*n + j] -= f * A[k*n + j];
            b[i] -= f * b[k];
        }
    }
    for (i = n - 1; i >= 0; i--) {
        double sum = b[i];
        for (k = i + 1; k < n; k++) sum -= A[i*n + k] * b[k];
        b[i] = sum / A[i*n + i];
    }
    return 0;
}

/* Small dense solve (I - C rho) H = C, done inline rather than through GSL.
 * This runs once per k-point per iteration -- for N=4095 and ~800 iterations
 * that is >3 million calls, where gsl_linalg_LU_decomp's generality and its
 * per-element accessor overhead dominate the actual arithmetic (measured:
 * the GSL version took 15 s against numpy's 3.3 s for the identical result).
 * Gaussian elimination with partial pivoting on stack arrays; p is small
 * (<= RYP_MAXP), so this is a handful of flops.
 * Returns 0 on success, -1 if the matrix is singular. */
static int solveSmall(int p, double *Amat, double *B)
{
    int i, j, k, piv;
    for (k = 0; k < p; k++) {
        double best = fabs(Amat[k*p + k]); piv = k;
        for (i = k + 1; i < p; i++) {
            const double v = fabs(Amat[i*p + k]);
            if (v > best) { best = v; piv = i; }
        }
        if (best < 1e-300) return -1;
        if (piv != k) {
            for (j = 0; j < p; j++) {
                double t = Amat[k*p + j]; Amat[k*p + j] = Amat[piv*p + j]; Amat[piv*p + j] = t;
                t = B[k*p + j];           B[k*p + j]    = B[piv*p + j];    B[piv*p + j]    = t;
            }
        }
        {
            const double d = Amat[k*p + k];
            for (i = k + 1; i < p; i++) {
                const double f = Amat[i*p + k] / d;
                if (f == 0.0) continue;
                for (j = k; j < p; j++) Amat[i*p + j] -= f * Amat[k*p + j];
                for (j = 0; j < p; j++) B[i*p + j]    -= f * B[k*p + j];
            }
        }
    }
    for (j = 0; j < p; j++)
        for (i = p - 1; i >= 0; i--) {
            double sum = B[i*p + j];
            for (k = i + 1; k < p; k++) sum -= Amat[i*p + k] * B[k*p + j];
            B[i*p + j] = sum / Amat[i*p + i];
        }
    return 0;
}

/* ------------------------------------------------------------------ */
ryp_status ryp_alloc(ryp_system *sys, int p, int N, double pointsPerSigma)
{
    size_t n3;
    if (!sys || p < 1 || p > RYP_MAXP || N < 16 || pointsPerSigma <= 0.0)
        return RYP_ERR_PARAM;
    if (!isPowerOfTwo(N + 1)) return RYP_ERR_GRID;

    memset(sys, 0, sizeof(*sys));
    sys->p = p;
    sys->N = N;
    sys->dr = 1.0 / pointsPerSigma;
    sys->dq = M_PI / (((double)N + 1.0) * sys->dr);

    n3 = (size_t)p * (size_t)p * (size_t)N;
    sys->EN     = (double *)calloc(n3, sizeof(double));
    sys->betaU  = (double *)calloc(n3, sizeof(double));
    sys->gamma  = (double *)calloc(n3, sizeof(double));
    sys->cr     = (double *)calloc(n3, sizeof(double));
    sys->work   = (double *)calloc(n3, sizeof(double));
    sys->fmix   = (double *)calloc((size_t)N, sizeof(double));
    sys->rgrid  = (double *)calloc((size_t)N, sizeof(double));
    sys->qgrid  = (double *)calloc((size_t)N, sizeof(double));
    sys->SM     = (double *)calloc((size_t)N, sizeof(double));
    sys->fftbuf = (double *)calloc((size_t)(2 * (N + 1) + N), sizeof(double));
    if (!sys->EN || !sys->betaU || !sys->gamma || !sys->cr || !sys->work ||
        !sys->fmix || !sys->rgrid || !sys->qgrid || !sys->SM || !sys->fftbuf) {
        ryp_free(sys);
        return RYP_ERR_ALLOC;
    }
    {
        int k;
        for (k = 0; k < N; k++) {
            sys->rgrid[k] = sys->dr * (double)(k + 1);
            sys->qgrid[k] = sys->dq * (double)(k + 1);
        }
    }
    sys->meanSigma = 1.0;
    return RYP_OK;
}

void ryp_free(ryp_system *sys)
{
    if (!sys) return;
    free(sys->EN);     free(sys->betaU); free(sys->gamma);
    free(sys->cr);     free(sys->work);  free(sys->fmix);
    free(sys->rgrid);  free(sys->qgrid); free(sys->SM);
    free(sys->fftbuf);
    memset(sys, 0, sizeof(*sys));
}

/* ------------------------------------------------------------------ */
/* Schulz -> p classes by moment matching. Schulz with relative width s is
 * Gamma(shape = t+1, scale = <sigma>/(t+1)) with t = 1/s^2 - 1, so the
 * moment-matching conditions sum_i x_i sigma_i^m = <sigma^m> for
 * m = 0..2p-1 are exactly Gauss-generalized-Laguerre quadrature with
 * weight x^t exp(-x). GSL provides those nodes/weights directly. */
ryp_status ryp_set_schulz(ryp_system *sys, double relativeStdDev, double meanSigma)
{
    int i;
    if (!sys || meanSigma <= 0.0) return RYP_ERR_PARAM;
    sys->meanSigma = meanSigma;

    if (relativeStdDev <= 0.0 || sys->p == 1) {
        sys->p = 1;
        sys->sigma[0] = meanSigma;
        sys->x[0] = 1.0;
        return RYP_OK;
    }
    {
        const double t = 1.0 / (relativeStdDev * relativeStdDev) - 1.0;
        double sum = 0.0;
        gsl_integration_fixed_workspace *w =
            gsl_integration_fixed_alloc(gsl_integration_fixed_laguerre,
                                        (size_t)sys->p, 0.0, 1.0, t, 0.0);
        if (!w) return RYP_ERR_ALLOC;
        for (i = 0; i < sys->p; i++) {
            sys->sigma[i] = w->x[i] * meanSigma / (t + 1.0);
            sys->x[i]     = w->weights[i] / gsl_sf_gamma(t + 1.0);
            sum += sys->x[i];
        }
        gsl_integration_fixed_free(w);
        if (!(sum > 0.0)) return RYP_ERR_PARAM;
        for (i = 0; i < sys->p; i++) sys->x[i] /= sum;
    }
    return RYP_OK;
}

ryp_status ryp_set_charges(ryp_system *sys, double referenceValence,
                           double chargeExponent, double bjerrumLength,
                           const double *explicitValences,
                           const double *screeningValences)
{
    int i;
    if (!sys || bjerrumLength <= 0.0) return RYP_ERR_PARAM;
    sys->bjerrum = bjerrumLength;
    for (i = 0; i < sys->p; i++) {
        sys->Z[i] = explicitValences
                  ? explicitValences[i]
                  : referenceValence * pow(sys->sigma[i] / sys->meanSigma, chargeExponent);
        sys->Zscreen[i] = screeningValences ? screeningValences[i] : sys->Z[i];
    }
    return RYP_OK;
}

ryp_status ryp_set_volume_fraction(ryp_system *sys, double phi)
{
    int i;
    double m3 = 0.0;
    if (!sys || phi <= 0.0 || phi >= 1.0) return RYP_ERR_PARAM;
    sys->volumeFraction = phi;
    for (i = 0; i < sys->p; i++) m3 += sys->x[i] * pow(sys->sigma[i], 3.0);
    /* phi = (pi/6) n <sigma^3>; reduces to the one-component relation for a
     * delta distribution. */
    sys->nTotal = phi / ((M_PI / 6.0) * m3);
    for (i = 0; i < sys->p; i++) sys->rho[i] = sys->nTotal * sys->x[i];
    return RYP_OK;
}

ryp_status ryp_build_potential(ryp_system *sys)
{
    int i, j, k;
    double sum = 0.0;
    if (!sys || sys->nTotal <= 0.0) return RYP_ERR_PARAM;

    /* kappa from the SMALL IONS: salt-free + monovalent counterions gives
     * n_counter = sum_i n_i Z_i by charge neutrality, hence the FIRST power
     * of Z. Using Z^2 gives a near-ideal-gas S(q). */
    for (i = 0; i < sys->p; i++) sum += sys->rho[i] * sys->Zscreen[i];
    if (sum <= 0.0) return RYP_ERR_PARAM;
    sys->kappa = sqrt(4.0 * M_PI * sys->bjerrum * sum);

    /* factorized amplitude, D'Aguanno & Klein PRA eq. (3): the DLVO size
     * correction and the exp(kappa sigma_ij) numerator both split per
     * species because sigma_ij = (sigma_i + sigma_j)/2. */
    for (i = 0; i < sys->p; i++)
        sys->A[i] = sys->Z[i] * sqrt(sys->bjerrum)
                  * exp(sys->kappa * sys->sigma[i] / 2.0)
                  / (1.0 + sys->kappa * sys->sigma[i] / 2.0);

    for (i = 0; i < sys->p; i++)
    for (j = 0; j < sys->p; j++) {
        const double sij = 0.5 * (sys->sigma[i] + sys->sigma[j]);
        const double aij = sys->A[i] * sys->A[j];
        for (k = 0; k < sys->N; k++) {
            const double r = sys->rgrid[k];
            const size_t o = IDX3(sys, i, j, k);
            if (r < sij) { sys->betaU[o] = 0.0; sys->EN[o] = 0.0; }
            else {
                const double u = aij * exp(-sys->kappa * r) / r;
                sys->betaU[o] = u;
                sys->EN[o]    = exp(-u);
            }
        }
    }
    sys->solved = 0;   /* results stale, but sys->hasGammaGuess is deliberately
                        * left alone: a slightly changed potential still leaves
                        * the previous gamma an excellent starting guess, which
                        * is exactly what makes warm starts pay off in a fit. */
    return RYP_OK;
}

/* ------------------------------------------------------------------ */
/* Rogers-Young closure, applied pairwise with a SINGLE SHARED mixing
 * function f(r) (no ij subscript in D'Aguanno & Klein eq. 37/38):
 *     c = EN * (1 + (exp(f*gamma) - 1)/f) - gamma - 1
 * The exponent is clipped: during transients exp() can overflow and poison
 * the whole vector with NaN. Clipping can only ever change values that
 * would otherwise be inf, so converged results are unaffected. */
static void ryClosure(const ryp_system *sys)
{
    const size_t n3 = (size_t)sys->p * sys->p * sys->N;
    size_t o;
    const int N = sys->N;
    for (o = 0; o < n3; o++) {
        const int k = (int)(o % (size_t)N);
        const double f = sys->fmix[k];
        const double g = sys->gamma[o];
        double xarg = f * g, term;
        if (xarg >  500.0) xarg =  500.0;
        if (xarg < -700.0) xarg = -700.0;
        term = (f < 1e-8) ? g * (1.0 + 0.5 * xarg) : expm1(xarg) / f;
        sys->cr[o] = sys->EN[o] * (1.0 + term) - g - 1.0;
    }
}

/* One application of the fixed-point map: gamma -> G(gamma).
 * Closure -> forward transform -> matrix OZ -> back transform. Reads
 * sys->gamma, writes the image into out. Shared by the Picard and Anderson
 * drivers so both are guaranteed to iterate exactly the same operator. */
static ryp_status fixedPointMap(ryp_system *sys, double *out,
                                double *chat, double *ghat)
{
    const int p = sys->p, N = sys->N;
    int i, j, k;

    ryClosure(sys);

    /* c_ij = c_ji, so only the upper triangle needs transforming. */
    for (i = 0; i < p; i++)
    for (j = i; j < p; j++) {
        radialForward(sys, sys->cr + IDX3(sys, i, j, 0),
                      chat + IDX3(sys, i, j, 0), sys->fftbuf);
        if (j != i)
            memcpy(chat + IDX3(sys, j, i, 0), chat + IDX3(sys, i, j, 0),
                   sizeof(double) * (size_t)N);
    }

    for (k = 0; k < N; k++) {
        double Amat[RYP_MAXP*RYP_MAXP], B[RYP_MAXP*RYP_MAXP];
        for (i = 0; i < p; i++)
        for (j = 0; j < p; j++) {
            const double cij = chat[IDX3(sys, i, j, k)];
            B[i*p + j]    = cij;
            Amat[i*p + j] = (i == j ? 1.0 : 0.0) - cij * sys->rho[j];
        }
        if (solveSmall(p, Amat, B) != 0) return RYP_ERR_SINGULAR;
        for (i = 0; i < p; i++)
        for (j = 0; j < p; j++)
            ghat[IDX3(sys, i, j, k)] = B[i*p + j] - chat[IDX3(sys, i, j, k)];
    }

    for (i = 0; i < p; i++)
    for (j = i; j < p; j++) {
        radialBackward(sys, ghat + IDX3(sys, i, j, 0),
                       out + IDX3(sys, i, j, 0), sys->fftbuf);
        if (j != i)
            memcpy(out + IDX3(sys, j, i, 0), out + IDX3(sys, i, j, 0),
                   sizeof(double) * (size_t)N);
    }
    return RYP_OK;
}

/* Fill sys->SM and the k-space h_ij used by ryp_Sij(), from a converged gamma. */
static void finalizeStructureFactors(ryp_system *sys, double *scratch)
{
    const int p = sys->p, N = sys->N;
    int i, j, k;
    ryClosure(sys);
    for (i = 0; i < p; i++)
    for (j = 0; j < p; j++) {
        for (k = 0; k < N; k++) {
            const size_t o = IDX3(sys, i, j, k);
            scratch[o] = sys->gamma[o] + sys->cr[o];      /* h_ij(r) */
        }
        radialForward(sys, scratch + IDX3(sys, i, j, 0),
                      sys->work + IDX3(sys, i, j, 0), sys->fftbuf);
    }
    for (k = 0; k < N; k++) {
        double num = 0.0, den = 0.0, b[RYP_MAXP];
        const double q = sys->qgrid[k];
        for (i = 0; i < p; i++) {
            const double sg = sys->sigma[i];
            b[i] = sg * sg * gsl_sf_bessel_j1(q * sg / 2.0) / q;
            den += sys->x[i] * b[i] * b[i];
        }
        for (i = 0; i < p; i++)
        for (j = 0; j < p; j++) {
            const double Sij = (i == j ? sys->x[i] : 0.0)
                + sys->nTotal * sys->x[i] * sys->x[j] * sys->work[IDX3(sys, i, j, k)];
            num += b[i] * b[j] * Sij;
        }
        sys->SM[k] = (den > 0.0) ? num / den : 1.0;
    }
    sys->solved = 1;
    sys->hasGammaGuess = 1;
}

ryp_status ryp_solve(ryp_system *sys, double alpha, int maxIterations,
                     double tolerance, double mixing)
{
    const int p = sys->p, N = sys->N;
    const size_t n3 = (size_t)p * p * N;
    double *chat = NULL, *ghat = NULL, *gnew = NULL;
    int it, k;
    ryp_status st = RYP_ERR_NOCONVERGE;

    if (!sys || maxIterations < 1 || mixing <= 0.0 || mixing > 1.0)
        return RYP_ERR_PARAM;
    sys->alpha = alpha;
    for (k = 0; k < N; k++) sys->fmix[k] = 1.0 - exp(-alpha * sys->rgrid[k]);

    chat = (double *)calloc(n3, sizeof(double));
    ghat = (double *)calloc(n3, sizeof(double));
    gnew = (double *)calloc(n3, sizeof(double));
    if (!chat || !ghat || !gnew) { st = RYP_ERR_ALLOC; goto done; }

    if (!sys->hasGammaGuess) memset(sys->gamma, 0, sizeof(double) * n3);

    st = RYP_ERR_NOCONVERGE;   /* only set to OK by the tolerance test below:
                                * fixedPointMap() returns RYP_OK every call, so
                                * reusing st as the loop status would report
                                * success for a run that simply ran out of
                                * iterations (observed: garbage S^M returned
                                * with lastIterations == 0). */
    for (it = 0; it < maxIterations; it++) {
        double maxdiff = 0.0;
        size_t o;
        ryp_status mst = fixedPointMap(sys, gnew, chat, ghat);
        if (mst != RYP_OK) { st = mst; goto done; }
        for (o = 0; o < n3; o++) {
            const double d = fabs(gnew[o] - sys->gamma[o]);
            if (!isfinite(gnew[o])) { st = RYP_ERR_NOCONVERGE; goto done; }
            if (d > maxdiff) maxdiff = d;
            sys->gamma[o] = (1.0 - mixing) * sys->gamma[o] + mixing * gnew[o];
        }
        if (maxdiff < tolerance) { st = RYP_OK; sys->lastIterations = it + 1; break; }
    }

    if (st == RYP_OK) finalizeStructureFactors(sys, gnew);

done:
    free(chat); free(ghat); free(gnew);
    if (st != RYP_OK) { sys->solved = 0; sys->hasGammaGuess = 0; }
    if (st != RYP_OK)
        snprintf(sys->lastError, sizeof(sys->lastError), "%s", ryp_strerror(st));
    return st;
}

/* ------------------------------------------------------------------ */
/* Anderson-accelerated fixed-point solve via SUNDIALS KINSOL (KIN_FP).
 *
 * A hand-rolled Anderson was tried first and abandoned: damped, it was
 * SLOWER than undamped Picard (851 vs 677 iterations) and it diverged for
 * any damping above ~0.1. The same instability showed up in the Python
 * prototype, where the hand-written Anderson solver ran away to ~1e200 while
 * Picard converged. KINSOL's implementation handles the least-squares
 * conditioning and safeguarding properly, and SASfit already links SUNDIALS
 * (see ${sundials_LIBRARIES} in src/CMakeLists.txt), so this costs no new
 * dependency.
 *
 * KIN_FP expects the FIXED-POINT map itself, G(u), not the residual. */
static ryp_system *g_kinsysForCallback = NULL;
static double *g_kinChat = NULL, *g_kinGhat = NULL;

static int kinFixedPoint(N_Vector u, N_Vector fval, void *user_data)
{
    ryp_system *sys = (ryp_system *)user_data;
    const size_t n3 = (size_t)sys->p * sys->p * sys->N;
    double *ud = N_VGetArrayPointer(u);
    double *fd = N_VGetArrayPointer(fval);
    ryp_status st;
    memcpy(sys->gamma, ud, sizeof(double) * n3);
    st = fixedPointMap(sys, fd, g_kinChat, g_kinGhat);
    /* 1 = recoverable. Returning -1 (unrecoverable) was found to segfault
     * in the Python binding too; see sundials4pyKinsolFPOZsolver.py. */
    return (st == RYP_OK) ? 0 : 1;
}

ryp_status ryp_solve_anderson(ryp_system *sys, double alpha, int maxIterations,
                              double tolerance, double beta)
{
    const int p = sys->p, N = sys->N;
    const size_t n3 = (size_t)p * p * N;
    SUNContext ctx = NULL;
    N_Vector u = NULL, scale = NULL;
    SUNLinearSolver LS = NULL;
    void *kmem = NULL;
    ryp_status st = RYP_ERR_NOCONVERGE;
    int flag, k;
    long int nni = 0;

    if (!sys || maxIterations < 1) return RYP_ERR_PARAM;
    sys->alpha = alpha;
    for (k = 0; k < N; k++) sys->fmix[k] = 1.0 - exp(-alpha * sys->rgrid[k]);

    g_kinChat = (double *)calloc(n3, sizeof(double));
    g_kinGhat = (double *)calloc(n3, sizeof(double));
    if (!g_kinChat || !g_kinGhat) { st = RYP_ERR_ALLOC; goto done; }

    if (RYP_SUNCONTEXT_CREATE(&ctx) != 0) { st = RYP_ERR_ALLOC; goto done; }
    u     = N_VNew_Serial((sunindextype)n3, ctx);
    scale = N_VNew_Serial((sunindextype)n3, ctx);
    if (!u || !scale) { st = RYP_ERR_ALLOC; goto done; }
    N_VConst(1.0, scale);

    if (!sys->hasGammaGuess) memset(sys->gamma, 0, sizeof(double) * n3);

    /* Picard pre-conditioning. Anderson extrapolation from a cold start
     * takes steps far outside the basin of attraction on this problem and
     * diverges (checked for both a hand-rolled Anderson and KINSOL, damped
     * and undamped). A short run of plain undamped Picard -- which IS
     * contractive here -- first gets close enough that the acceleration
     * then behaves. Set RYP_PREPICARD to override for experiments. */
    {
        const char *env = getenv("RYP_PREPICARD");
        int nPre = env ? atoi(env) : RYP_PICARD_PRESTEPS;
        if (nPre > 0) {
            double *pc = (double *)calloc(n3, sizeof(double));
            double *pg = (double *)calloc(n3, sizeof(double));
            double *pn = (double *)calloc(n3, sizeof(double));
            if (pc && pg && pn) {
                int q;
                for (q = 0; q < nPre; q++) {
                    if (fixedPointMap(sys, pn, pc, pg) != RYP_OK) break;
                    memcpy(sys->gamma, pn, sizeof(double) * n3);
                }
            }
            free(pc); free(pg); free(pn);
        }
    }
    memcpy(N_VGetArrayPointer(u), sys->gamma, sizeof(double) * n3);

    kmem = KINCreate(ctx);
    if (!kmem) { st = RYP_ERR_ALLOC; goto done; }

    /* Call order and options copied deliberately from pyozGUI's own
     * sundials4pyKinsolFPOZsolver.py, which in turn mirrors
     * sasfit_oz_solver.c's "case KINSOLFP" block -- that combination is what
     * SASfit has used in production. Three details matter and were each got
     * wrong on the first attempt here:
     *   - KINInit comes BEFORE KINSetMAA (my first version had them the
     *     other way round).
     *   - NO damping. KINSetDampingAA is never called by SASfit's own
     *     configure routine either; adding it made things worse here.
     *   - a plain GMRES linear solver is attached even though KIN_FP does
     *     not need Newton-style linear solves, again matching the C code.
     * KINSetMAA is the Anderson subspace size; 5 matches the value in
     * SASfit's own "configure OZ solver" dialog. */
    /* NOTE: pyozGUI calls KINInit first and KINSetMAA second; doing that in
     * C segfaults with SUNDIALS 6.4.1, because KINSetMAA after KINInit sets
     * the Anderson depth without allocating the corresponding arrays. The
     * documented requirement is KINSetMAA BEFORE KINInit, which is what is
     * done here. (In the Python binding the order is evidently tolerated --
     * quite possibly because the setting is silently ignored there, which
     * would mean that path is running plain fixed-point iteration rather
     * than the Anderson-accelerated one it appears to request.) */
    KINSetMAA(kmem, (long int)RYP_AA_DEPTH);
    if (KINInit(kmem, kinFixedPoint, u) != KIN_SUCCESS) { st = RYP_ERR_PARAM; goto done; }
    KINSetUserData(kmem, (void *)sys);
    KINSetNumMaxIters(kmem, (long int)maxIterations);
    KINSetFuncNormTol(kmem, tolerance);
    KINSetScaledStepTol(kmem, tolerance);
    LS = SUNLinSol_SPGMR(u, SUN_PREC_NONE, 0, ctx);
    if (LS) KINSetLinearSolver(kmem, LS, NULL);
    (void)beta;   /* deliberately unused: SASfit sets no damping, see above */

    flag = KINSol(kmem, u, KIN_FP, scale, scale);

    /* VERIFY. KINSol can return KIN_SUCCESS while the iteration has actually
     * diverged: once the RY closure's exponent clipping pins values at ~1e217
     * the internal stopping test is defeated and success is reported with
     * garbage (seen directly at p=5, s_sigma=0.3, where S^M came back as
     * ~1e222). Never trust the flag alone -- recompute the residual and
     * insist it really is small before accepting the result. */
    if (flag == KIN_SUCCESS || flag == KIN_INITIAL_GUESS_OK) {
        double *chk = (double *)calloc(n3, sizeof(double));
        double res = 0.0; size_t o;
        double *ud = N_VGetArrayPointer(u);
        if (!chk) { st = RYP_ERR_ALLOC; goto done; }
        memcpy(sys->gamma, ud, sizeof(double) * n3);
        if (fixedPointMap(sys, chk, g_kinChat, g_kinGhat) != RYP_OK) res = 1e300;
        else for (o = 0; o < n3; o++) {
            const double d = fabs(chk[o] - ud[o]);
            if (!isfinite(d)) { res = 1e300; break; }
            if (d > res) res = d;
        }
        free(chk);
        if (!(res < fmax(tolerance * 1e3, 1e-6))) flag = KIN_MEM_FAIL;  /* force failure */
    }
    if (getenv("RYP_DEBUG")) {
        double *ud = N_VGetArrayPointer(u);
        double *tmp = (double *)calloc(n3, sizeof(double));
        double res = 0.0, umax = 0.0; size_t o;
        memcpy(sys->gamma, ud, sizeof(double) * n3);
        fixedPointMap(sys, tmp, g_kinChat, g_kinGhat);
        for (o = 0; o < n3; o++) {
            const double d = fabs(tmp[o] - ud[o]);
            if (d > res) res = d;
            if (fabs(ud[o]) > umax) umax = fabs(ud[o]);
        }
        fprintf(stderr, "[ryp] KINSol flag=%d  ||G(u)-u||_inf=%.3e  ||u||_inf=%.3e\n",
                flag, res, umax);
        free(tmp);
    }
    if (flag == KIN_SUCCESS || flag == KIN_INITIAL_GUESS_OK) {
        KINGetNumNonlinSolvIters(kmem, &nni);
        sys->lastIterations = (int)nni;
        memcpy(sys->gamma, N_VGetArrayPointer(u), sizeof(double) * n3);
        st = RYP_OK;
    } else {
        st = RYP_ERR_NOCONVERGE;
    }

    if (st == RYP_OK) {
        double *scratch = (double *)calloc(n3, sizeof(double));
        if (!scratch) { st = RYP_ERR_ALLOC; goto done; }
        finalizeStructureFactors(sys, scratch);
        free(scratch);
    }

done:
    if (kmem) KINFree(&kmem);
    if (u)     N_VDestroy(u);
    if (scale) N_VDestroy(scale);
    if (LS)    SUNLinSolFree(LS);
    if (ctx)   SUNContext_Free(&ctx);
    free(g_kinChat); free(g_kinGhat);
    g_kinChat = g_kinGhat = NULL;
    (void)g_kinsysForCallback;
    if (st != RYP_OK) { sys->solved = 0; sys->hasGammaGuess = 0; }
    if (st != RYP_OK)
        snprintf(sys->lastError, sizeof(sys->lastError), "%s", ryp_strerror(st));
    return st;
}

double ryp_Sij(const ryp_system *sys, int i, int j, int k)
{
    if (!sys || !sys->solved || i < 0 || j < 0 || i >= sys->p || j >= sys->p
        || k < 0 || k >= sys->N) return 0.0;
    return (i == j ? sys->x[i] : 0.0)
         + sys->nTotal * sys->x[i] * sys->x[j] * sys->work[IDX3(sys, i, j, k)];
}

double ryp_SM(const ryp_system *sys, double q)
{
    int k;
    double t;
    if (!sys || !sys->solved) return 1.0;
    if (q <= sys->qgrid[0]) return sys->SM[0];
    if (q >= sys->qgrid[sys->N - 1]) return 1.0;   /* S -> 1 at large q */
    k = (int)floor(q / sys->dq) - 1;
    if (k < 0) k = 0;
    if (k > sys->N - 2) k = sys->N - 2;
    t = (q - sys->qgrid[k]) / (sys->qgrid[k + 1] - sys->qgrid[k]);
    return (1.0 - t) * sys->SM[k] + t * sys->SM[k + 1];
}


/* Robust solve: undamped Picard (mixing = 1) converges roughly 4x faster than
 * the damped mixing = 0.3 used during development (677 vs 2276 iterations on
 * the D'Aguanno-Klein test system, giving an identical S^M to six decimals),
 * but undamped iteration can diverge in other parameter regimes. So try the
 * fast setting first and fall back to progressively heavier damping only if
 * it fails -- the same "try fast, retry safe" pattern oZsolver.py uses in its
 * own _robustInlineSolve(). Each retry starts cold, since a diverged run
 * leaves gamma poisoned (ryp_solve() clears hasGammaGuess on failure). */

/* ------------------------------------------------------------------ */
/* Newton-Krylov with GMRES and linesearch (KIN_LINESEARCH), on the
 * RESIDUAL F(u) = G(u) - u.
 *
 * This is a genuinely different algorithm from KIN_FP above, not a variant:
 * KIN_FP does Anderson-accelerated fixed-point iteration on G, whereas this
 * does damped Newton steps on F with the Jacobian applied matrix-free through
 * GMRES. The linesearch is what makes it globally convergent, which is
 * precisely what Anderson lacks and why Anderson needed Picard
 * pre-conditioning to stay in the basin.
 *
 * Settings copied from pyozGUI's sundials4pyKinsolOZsolver.py, which in turn
 * matches SASfit's own KIN_sasfit_configure() and the values in its
 * "configure OZ solver" dialog. That file records that these specific values
 * mattered: a strongly charged DLVO+MSA case that failed with more
 * conservative settings converged cleanly once they matched SASfit's. Note
 * the two tolerances differ (FuncNormTol looser than ScaledStepTol) and
 * MaxNewtonStep is 100*n, not n. */
static int kinResidual(N_Vector u, N_Vector fval, void *user_data)
{
    ryp_system *sys = (ryp_system *)user_data;
    const size_t n3 = (size_t)sys->p * sys->p * sys->N;
    double *ud = N_VGetArrayPointer(u);
    double *fd = N_VGetArrayPointer(fval);
    size_t o;
    ryp_status st;
    memcpy(sys->gamma, ud, sizeof(double) * n3);
    st = fixedPointMap(sys, fd, g_kinChat, g_kinGhat);
    if (st != RYP_OK) return 1;             /* recoverable */
    for (o = 0; o < n3; o++) fd[o] -= ud[o];   /* F = G(u) - u */
    return 0;
}

ryp_status ryp_solve_newton_krylov(ryp_system *sys, double alpha, int maxIterations,
                                   double funcNormTol, double scaledStepTol)
{
    const int p = sys->p, N = sys->N;
    const size_t n3 = (size_t)p * p * N;
    SUNContext ctx = NULL;
    N_Vector u = NULL, scale = NULL;
    SUNLinearSolver LS = NULL;
    void *kmem = NULL;
    ryp_status st = RYP_ERR_NOCONVERGE;
    int flag, k;
    long int nni = 0;

    if (!sys || maxIterations < 1) return RYP_ERR_PARAM;
    sys->alpha = alpha;
    for (k = 0; k < N; k++) sys->fmix[k] = 1.0 - exp(-alpha * sys->rgrid[k]);

    g_kinChat = (double *)calloc(n3, sizeof(double));
    g_kinGhat = (double *)calloc(n3, sizeof(double));
    if (!g_kinChat || !g_kinGhat) { st = RYP_ERR_ALLOC; goto done; }

    if (RYP_SUNCONTEXT_CREATE(&ctx) != 0) { st = RYP_ERR_ALLOC; goto done; }
    u     = N_VNew_Serial((sunindextype)n3, ctx);
    scale = N_VNew_Serial((sunindextype)n3, ctx);
    if (!u || !scale) { st = RYP_ERR_ALLOC; goto done; }
    N_VConst(1.0, scale);

    if (!sys->hasGammaGuess) memset(sys->gamma, 0, sizeof(double) * n3);
    memcpy(N_VGetArrayPointer(u), sys->gamma, sizeof(double) * n3);

    kmem = KINCreate(ctx);
    if (!kmem) { st = RYP_ERR_ALLOC; goto done; }
    if (KINInit(kmem, kinResidual, u) != KIN_SUCCESS) { st = RYP_ERR_PARAM; goto done; }
    KINSetUserData(kmem, (void *)sys);
    KINSetNumMaxIters(kmem, (long int)maxIterations);
    KINSetFuncNormTol(kmem, funcNormTol);
    KINSetScaledStepTol(kmem, scaledStepTol);
    KINSetMaxNewtonStep(kmem, RYP_MAX_NEWTON_STEP_FACTOR * (double)n3);
    KINSetEtaForm(kmem, KIN_ETACHOICE1);

    LS = SUNLinSol_SPGMR(u, SUN_PREC_NONE, 0, ctx);
    if (!LS) { st = RYP_ERR_ALLOC; goto done; }
    if (KINSetLinearSolver(kmem, LS, NULL) != KIN_SUCCESS) { st = RYP_ERR_PARAM; goto done; }
    SUNLinSol_SPGMRSetMaxRestarts(LS, RYP_GMRES_MAX_RESTARTS);

    flag = KINSol(kmem, u, KIN_LINESEARCH, scale, scale);

    /* Same verification as the KIN_FP path: never trust the flag alone. */
    if (flag == KIN_SUCCESS || flag == KIN_INITIAL_GUESS_OK) {
        double *chk = (double *)calloc(n3, sizeof(double));
        double res = 0.0; size_t o;
        double *ud = N_VGetArrayPointer(u);
        if (!chk) { st = RYP_ERR_ALLOC; goto done; }
        memcpy(sys->gamma, ud, sizeof(double) * n3);
        if (fixedPointMap(sys, chk, g_kinChat, g_kinGhat) != RYP_OK) res = 1e300;
        else for (o = 0; o < n3; o++) {
            const double d = fabs(chk[o] - ud[o]);
            if (!isfinite(d)) { res = 1e300; break; }
            if (d > res) res = d;
        }
        free(chk);
        if (res < fmax(funcNormTol * 1e3, 1e-6)) {
            KINGetNumNonlinSolvIters(kmem, &nni);
            sys->lastIterations = (int)nni;
            memcpy(sys->gamma, ud, sizeof(double) * n3);
            st = RYP_OK;
        }
    }

    if (st == RYP_OK) {
        double *scratch = (double *)calloc(n3, sizeof(double));
        if (!scratch) { st = RYP_ERR_ALLOC; goto done; }
        finalizeStructureFactors(sys, scratch);
        free(scratch);
    }

done:
    if (kmem) KINFree(&kmem);
    if (LS)    SUNLinSolFree(LS);
    if (u)     N_VDestroy(u);
    if (scale) N_VDestroy(scale);
    if (ctx)   SUNContext_Free(&ctx);
    free(g_kinChat); free(g_kinGhat);
    g_kinChat = g_kinGhat = NULL;
    if (st != RYP_OK) { sys->solved = 0; sys->hasGammaGuess = 0; }
    if (st != RYP_OK)
        snprintf(sys->lastError, sizeof(sys->lastError), "%s", ryp_strerror(st));
    return st;
}

ryp_status ryp_solve_robust(ryp_system *sys, double alpha, int maxIterations,
                            double tolerance)
{
    static const double mixings[] = { 1.0, 0.7, 0.5, 0.3, 0.15 };
    const int nmix = (int)(sizeof(mixings)/sizeof(mixings[0]));
    ryp_status st;
    int i;

    /* Three-tier strategy, fastest first, each tier strictly more robust
     * than the one before. Measured across six parameter regimes (see
     * tests/nktest.c and tests/regtest.c):
     *
     *  1. Anderson (KINSOL KIN_FP) with Picard pre-conditioning -- fastest
     *     where it works (0.46 s at p=3), but genuinely fails in some
     *     regimes: at p=5, s_sigma=0.3 it diverges.
     *  2. Newton-Krylov (GMRES + KIN_LINESEARCH) on the residual -- a
     *     different algorithm, not a variant. It converged in EVERY regime
     *     tried, and in a strikingly constant 31-40 Newton iterations
     *     regardless of how hard the problem was, which is what global
     *     convergence via linesearch buys. The cost is wall-clock: each
     *     Newton step needs many GMRES inner iterations, each of which is a
     *     full function evaluation, so it runs 3-6x slower than Anderson
     *     where Anderson works at all.
     *  3. Plain Picard, undamped then progressively damped -- last resort.
     *
     * Both KINSOL tiers verify the residual themselves after KINSol returns
     * rather than trusting its flag, because KIN_FP was observed reporting
     * KIN_SUCCESS after diverging to ~1e217. */
    st = ryp_solve_anderson(sys, alpha, maxIterations, tolerance, 1.0);
    if (st == RYP_OK) { sys->lastMixing = 0.0; sys->lastMethod = "Anderson"; return RYP_OK; }

    st = ryp_solve_newton_krylov(sys, alpha, maxIterations,
                                 RYP_FUNC_NORM_TOL, RYP_SCALED_STEP_TOL);
    if (st == RYP_OK) { sys->lastMixing = 0.0; sys->lastMethod = "Newton-Krylov"; return RYP_OK; }

    for (i = 0; i < nmix; i++) {
        st = ryp_solve(sys, alpha, maxIterations, tolerance, mixings[i]);
        if (st == RYP_OK) { sys->lastMixing = mixings[i]; sys->lastMethod = "Picard"; return RYP_OK; }
        if (st != RYP_ERR_NOCONVERGE && st != RYP_ERR_SINGULAR) return st;
    }
    sys->lastMethod = "none";
    return st;
}
