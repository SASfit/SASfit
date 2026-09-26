/*
 * ry_polydisperse_core.h
 *
 * Multicomponent (polydisperse) hard-core Yukawa structure factor from a
 * numerical Ornstein-Zernike solve with the Rogers-Young closure.
 *
 * Method: B. D'Aguanno & R. Klein, Phys. Rev. A 46, 7652 (1992) and
 *         J. Chem. Soc. Faraday Trans. 87, 379 (1991).
 *         F. J. Rogers & D. A. Young, Phys. Rev. A 30, 999 (1984).
 *
 * This is a port of the validated Python implementation in src/pyozGUI
 * (setPolydisperseHardCoreYukawaPotential + the multicomponent Gamma
 * fixpoint operator), checked numerically against it: the two agree to
 * ~1e-8 relative on S^M(q), and to ~1e-11 on the plain Picard path.
 *
 * Deliberately free of SASfit types so it can be unit-tested standalone;
 * the SASfit plugin wrapper lives in sasfit_sq_RYPolydisperseYukawa.c.
 */
#ifndef RY_POLYDISPERSE_CORE_H
#define RY_POLYDISPERSE_CORE_H

#define RYP_AA_DEPTH 5       /* KINSetMAA: Anderson subspace size, matching
                              * the value in SASfit's own configure dialog */

/* Picard steps run before handing over to Anderson. Anderson diverges from a
 * cold start on this problem; a short contractive Picard run first fixes it.
 * Measured on the D'Aguanno-Klein test system: 0 diverges, 20 already works,
 * 40 is the time optimum. 50 is used as the default, slightly above the
 * minimum that works, for margin in other parameter regimes. */
#define RYP_PICARD_PRESTEPS 50

/* Newton-Krylov tuning, matching SASfit's own KIN_sasfit_configure()
 * and its "configure OZ solver" dialog defaults. */
#define RYP_MAX_NEWTON_STEP_FACTOR 100.0   /* KINSetMaxNewtonStep = 100*n */
#define RYP_GMRES_MAX_RESTARTS     10
#define RYP_FUNC_NORM_TOL          1e-10
#define RYP_SCALED_STEP_TOL        1e-13

#define RYP_MAXP 12          /* max number of size classes; p=3..5 is ample
                              * (D'Aguanno & Klein report p=3 indistinguishable
                              * from p=5 up to s_sigma=0.3, reproduced here) */

typedef enum {
    RYP_OK = 0,
    RYP_ERR_ALLOC       = -1,
    RYP_ERR_PARAM       = -2,
    RYP_ERR_GRID        = -3,   /* N+1 must be a power of two */
    RYP_ERR_NOCONVERGE  = -4,
    RYP_ERR_SINGULAR    = -5    /* 1 - C rho not invertible: unphysical input */
} ryp_status;

typedef struct {
    int    p;                    /* number of size classes */
    int    N;                    /* radial points; N+1 MUST be a power of two */
    double dr;                   /* grid spacing, units of the mean diameter */
    double dq;

    double sigma[RYP_MAXP];      /* component diameters */
    double x[RYP_MAXP];          /* molar fractions, sum = 1 */
    double rho[RYP_MAXP];        /* number densities rho_i = n * x_i */
    double Z[RYP_MAXP];          /* valence entering the pair amplitude */
    double Zscreen[RYP_MAXP];    /* valence entering kappa (usually == Z) */
    double A[RYP_MAXP];          /* factorized amplitude, see build_potential */

    double meanSigma;
    double nTotal;               /* total number density */
    double volumeFraction;
    double kappa;                /* inverse Debye length (small ions!) */
    double bjerrum;

    double alpha;                /* Rogers-Young mixing parameter */

    /* work arrays, all length p*p*N unless noted */
    double *EN;                  /* exp(-beta u_ij(r)) */
    double *betaU;
    double *gamma;
    double *cr;
    double *work;
    double *fmix;                /* length N: 1 - exp(-alpha r) */
    double *rgrid;               /* length N */
    double *qgrid;               /* length N */
    double *SM;                  /* length N: measured structure factor */
    double *fftbuf;              /* length 2*(N+1) + N */
    int     solved;
    int     hasGammaGuess;       /* gamma holds a usable starting guess */
    int     lastIterations;      /* iterations used by the last solve */
    double  lastMixing;          /* mixing that actually worked (ryp_solve_robust) */
    const char *lastMethod;      /* which solver tier succeeded */
    char    lastError[256];
} ryp_system;

/* lifecycle */
ryp_status ryp_alloc(ryp_system *sys, int p, int N, double pointsPerSigma);
void       ryp_free(ryp_system *sys);
/* force the next ryp_solve() to start from gamma = 0 instead of warm-starting */
void       ryp_reset(ryp_system *sys);

/* set-up: Schulz distribution reduced to p classes by moment matching
 * (Gauss-generalized-Laguerre; exact for the first 2p-1 moments) */
ryp_status ryp_set_schulz(ryp_system *sys, double relativeStdDev, double meanSigma);

/* charge scaling. chargeExponent: 2 = constant surface charge density
 * (D'Aguanno-Klein), 1 = linear in size (charge-renormalisation saturated),
 * 0 = size independent. Pass explicitValences != NULL (length p) to override
 * the power law entirely; screeningValences != NULL to let kappa use a
 * different charge than the amplitude does. */
ryp_status ryp_set_charges(ryp_system *sys, double referenceValence,
                           double chargeExponent, double bjerrumLength,
                           const double *explicitValences,
                           const double *screeningValences);

ryp_status ryp_set_volume_fraction(ryp_system *sys, double phi);
ryp_status ryp_build_potential(ryp_system *sys);

/* plain Picard with the given mixing (1.0 = undamped) */
ryp_status ryp_solve(ryp_system *sys, double alpha, int maxIterations,
                     double tolerance, double mixing);

/* Anderson-accelerated fixed-point iteration via SUNDIALS KINSOL (KIN_FP),
 * preceded by RYP_PICARD_PRESTEPS Picard steps; beta is currently unused
 * (SASfit's own configure routine applies no damping either). */
ryp_status ryp_solve_anderson(ryp_system *sys, double alpha, int maxIterations,
                              double tolerance, double beta);

/* Newton-Krylov (GMRES + linesearch) on the residual G(u)-u. A different
 * algorithm from the Anderson fixed-point solve above; the linesearch makes
 * it globally convergent, and it converged in every regime tried. */
ryp_status ryp_solve_newton_krylov(ryp_system *sys, double alpha, int maxIterations,
                                   double funcNormTol, double scaledStepTol);

/* Preferred entry point: Anderson -> Newton-Krylov -> Picard, fastest first,
 * each tier strictly more robust than the one before. */
ryp_status ryp_solve_robust(ryp_system *sys, double alpha, int maxIterations,
                            double tolerance);

/* measured structure factor S^M(q) at arbitrary q (interpolated) */
double     ryp_SM(const ryp_system *sys, double q);
/* partial structure factor S_ij on the internal q grid index k */
double     ryp_Sij(const ryp_system *sys, int i, int j, int k);

const char *ryp_strerror(ryp_status st);

#endif /* RY_POLYDISPERSE_CORE_H */
