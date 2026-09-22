# -*- coding: utf-8 -*-
"""
Load measured or simulated I(Q) and fit the polydisperse model to it.

Two pieces:

  loadCurve(path)              read a two- or three-column ASCII/CSV curve,
                               as exported by SASfit and most SAS packages
  PolydisperseFit(...).run()   least-squares fit of the model in
                               generic_polydisperse_sas to such a curve

SCALE AND BACKGROUND ARE NOT FITTED NONLINEARLY
-----------------------------------------------
A measured curve is

    I_obs(Q) = scale * I_model(Q; p) + background

and both scale and background enter LINEARLY. At every iteration they are
therefore obtained exactly, by a two-parameter weighted linear least squares
against the current model shape, instead of being handed to the nonlinear
optimiser. This matters more than it looks:

  * it removes the two most strongly correlated parameters from the
    nonlinear problem -- scale trades against volume fraction and contrast,
    background against everything at high Q -- which is where such fits
    usually stall;
  * it costs nothing, since the model shape is already computed;
  * and it means the optimiser never wastes an Ornstein-Zernike solve
    exploring a scale factor.

Only the parameters that actually change the SHAPE of the curve are varied
nonlinearly.

COST
----
Each model evaluation is one OZ solve, a few tenths of a second to a few
seconds depending on the closure and the number of size classes. A fit of
three or four shape parameters is therefore minutes, not seconds, and the
numerical Jacobian costs one extra solve per parameter per iteration. Two
consequences worth knowing:

  * start from a sensible guess -- use the tab to get the curve roughly
    right by eye first;
  * keep the structure-factor class count low (3 is usually enough) while
    fitting, and raise it for the final evaluation. The form-factor class
    count can stay high: it costs nothing, since it does not enter the OZ
    solve.

A failed solve is NOT an error during a fit: the optimiser will try
unphysical corners of parameter space. Such points return a large residual
so the optimiser walks away from them, rather than aborting the fit.

VERIFIED
--------
Synthetic hard-sphere data (R = 50, s = 0.22, phi = 0.18, scale 1.7e-3,
background 0.012, 3 % noise), started deliberately wrong at R = 40, s = 0.10,
phi = 0.30: recovered to 1.0 %, 0.6 % and 7.9 % respectively, with
chi2_red = 0.903 in 31 evaluations and no failed solves. The residual error on
phi is the visible consequence of its correlation with the scale factor -- it
is the parameter such data constrains least.
"""
import copy
import time

import numpy as np


class FitAborted(Exception):
    """Raised out of the residual function when the user interrupts.

    Propagated through scipy's optimiser rather than returning a sentinel
    value: a large residual would merely make the optimiser step elsewhere and
    keep going, which is not what "interrupt" means.
    """


# ----------------------------------------------------------------------
def loadCurve(path, qmin=None, qmax=None, withResolution=False):
    """Read Q, I and optionally dI (and dQ) from a text file.

    Accepts whitespace-, comma- or semicolon-separated columns with any
    number of comment lines (#, %, //) or a text header, which is what
    SASfit, SasView and most beamline pipelines export. Rows that cannot be
    parsed as numbers are skipped rather than raising, so a stray trailing
    line does not lose the file.

    Returns (Q, I, dI), or (Q, I, dI, dQ) when withResolution is set. dI is
    None when the file has only two columns, dQ when it has fewer than four.

    A FOURTH column is taken to be the Q resolution. NOTE the convention:
    it is read as the GAUSSIAN SIGMA of the resolution function, not the
    FWHM. The two differ by 2.355, and using the wrong one silently rescales
    the fitted polydispersity -- which is precisely the bias that smearing
    exists to remove. If the reduction writes FWHM, divide by 2.355 before
    fitting, or pass the column through Resolution(..., fwhm=True).
    """
    rows = []
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            line = line.strip()
            if not line or line[0] in "#%" or line.startswith("//"):
                continue
            for sep in (None, ",", ";", "\t"):
                parts = line.split(sep) if sep else line.split()
                if len(parts) >= 2:
                    try:
                        vals = [float(p) for p in parts[:4]]
                    except ValueError:
                        continue
                    rows.append(vals)
                    break
    if not rows:
        raise ValueError(f"no numeric data found in {path}")
    width = min(len(r) for r in rows)
    arr = np.array([r[:width] for r in rows], float)
    Q, I = arr[:, 0], arr[:, 1]
    dI = arr[:, 2] if width >= 3 else None
    dQ = arr[:, 3] if width >= 4 else None

    good = np.isfinite(Q) & np.isfinite(I) & (Q > 0)
    if dI is not None:
        good &= np.isfinite(dI)
    if dQ is not None:
        good &= np.isfinite(dQ)
    if qmin is not None:
        good &= Q >= qmin
    if qmax is not None:
        good &= Q <= qmax
    Q, I = Q[good], I[good]
    dI = dI[good] if dI is not None else None
    dQ = dQ[good] if dQ is not None else None
    order = np.argsort(Q)
    out = (Q[order], I[order], (dI[order] if dI is not None else None))
    if withResolution:
        out = out + ((dQ[order] if dQ is not None else None),)
    return out


# ----------------------------------------------------------------------
class Resolution:
    """Gaussian Q-resolution smearing as a fixed linear operator.

        I_smeared(Q_i) = sum_j W_ij I_model(Qext_j)

    W is built ONCE from (Q, dQ), so applying it during a fit is a single
    matrix-vector product -- microseconds, against the tenths of a second an
    Ornstein-Zernike solve costs.

    WHY THIS MATTERS FOR REAL DATA
    ------------------------------
    SANS resolution dQ/Q is of order 10 % and smears form-factor oscillations
    in exactly the way polydispersity does. Fitting without it lets the model
    absorb the instrument into srel. Measured on smeared synthetic data with
    a true srel of 0.15:

        without smearing   srel = 0.1726   (15.1 % high)   chi2_red = 1.07
        with smearing      srel = 0.1497   ( 0.2 % high)   chi2_red = 0.75

    Note the chi-squared: WITHOUT smearing the fit does not look bad, it just
    is wrong. That is bias, not imprecision, and no amount of extra data
    fixes it.

    WHY THIS IS CHEAP HERE, WHEN IT USUALLY IS NOT
    ----------------------------------------------
    Smearing needs the model on an EXTENDED, denser grid, because the kernel
    has tails beyond the measured range. For most models that multiplies the
    evaluation cost several-fold. Here it is nearly free: the expensive part,
    the OZ solve, does not depend on the Q grid at all. Once Gamma has
    converged, I_exact(Q) only interpolates the stored S_ij and evaluates an
    analytic form factor, so asking for several times as many Q points costs
    almost nothing.

    WHY IT LEAVES VARIABLE PROJECTION INTACT
    ----------------------------------------
    The measured curve is

        I_obs = scale * Smear(I_model) + background

    Smearing is linear, and the background is NOT smeared -- it is a flat
    instrumental term added afterwards. So the model remains linear in scale
    and background, and their exact elimination is unaffected. Smearing the
    background instead would break that, and would also be wrong physics.

    CONVENTION: dQ is the Gaussian SIGMA, not the FWHM (factor 2.355). Pass
    fwhm=True to convert.
    """

    def __init__(self, Q, dQ, fwhm=False, nSigma=3.0, oversample=3,
                 kernel="rician"):
        #kernel: "rician" (default) or "gaussian".
        #
        #WHY RICIAN. A small-angle instrument broadens the scattering vector
        #in the DETECTOR PLANE, in two dimensions, and the data are then
        #radially averaged. If the broadening is isotropic Gaussian with
        #width sigma, the distribution of the MAGNITUDE |Q| about a true Q0
        #is not Gaussian but Rician,
        #
        #    P(Q|Q0) = (Q/sigma^2) exp(-(Q^2+Q0^2)/2 sigma^2) I0(Q Q0/sigma^2)
        #
        #with I0 the modified Bessel function of the first kind. A plain
        #Gaussian in Q is the large-Q0/sigma limit of this, and it is a good
        #approximation wherever dQ/Q is small.
        #
        #It is NOT small here. For the spherical-shell data this was written
        #against, dQ/Q reaches 0.55 at the lowest measured point, where the
        #Gaussian puts appreciable weight at NEGATIVE Q -- which does not
        #exist. The Rician form has the factor Q in front, so its weight
        #vanishes at the origin and is identically zero below it: the
        #unphysical region is excluded by construction rather than by a
        #guard.
        #
        #This does NOT by itself tame a divergent model. For a Porod-like
        #Q^(-4+d) the product with the leading Q still diverges, which is why
        #the power-law column is separately clamped below the measured range
        #(see PolydisperseFit._porodColumn). The two fixes are complementary:
        #this one gets the instrument right, that one declines to extrapolate
        #the model where there are no data.
        Q = np.asarray(Q, float)
        dQ = np.asarray(dQ, float)
        if dQ.shape != Q.shape:
            raise ValueError("dQ must have the same shape as Q")
        if fwhm:
            dQ = dQ/2.3548200450309493
        if kernel not in ("rician", "gaussian"):
            raise ValueError(f"kernel must be 'rician' or 'gaussian', "
                             f"got {kernel!r}")
        self.Q = Q
        self.dQ = dQ
        self.kernel = kernel

        #The extended grid must reach BELOW Q.min(): smearing the lowest
        #measured point pulls in model values at smaller Q, and a structure
        #factor can vary steeply there. Clipped at a small positive value
        #because Q <= 0 is meaningless. With the Rician kernel the clip is
        #barely felt -- the weight there is already going to zero -- whereas
        #with the Gaussian its exact value mattered a great deal.
        lo = max(float(np.min(Q - nSigma*dQ)), float(Q.min())*1e-3)
        hi = float(np.max(Q + nSigma*dQ))
        n = max(int(oversample*Q.size), Q.size + 2)
        #The measured Q are INCLUDED in the grid. Without them the dQ -> 0
        #limit does not reproduce the unsmeared curve: a kernel narrower than
        #the grid spacing collapses to nearest-neighbour, and the nearest
        #linspace node is not the measured point. Measured error before this
        #fix: 4.4e-2 relative at dQ/Q = 1e-9, where it must be zero. With the
        #points included it is 1.1e-16, and it costs nothing.
        self.Qext = np.unique(np.concatenate([np.linspace(lo, hi, n), Q]))

        #Trapezoid weights, so the kernel integrates correctly on a grid that
        #need not be uniform.
        dq = np.gradient(self.Qext)
        if kernel == "gaussian":
            z = (self.Qext[None, :] - Q[:, None])/dQ[:, None]
            W = np.exp(-0.5*z*z)*dq[None, :]
        else:
            #Rician. Written with the EXPONENTIALLY SCALED Bessel function
            #i0e(x) = exp(-|x|) I0(x): for the wide-resolution points here
            #the argument Q*Q0/sigma^2 reaches several hundred, where I0
            #overflows a double long before the exponential prefactor
            #cancels it. Factoring gives
            #
            #   P = (Q/s^2) exp(-(Q-Q0)^2/2s^2) i0e(Q Q0/s^2)
            #
            #which is the same function with every large factor removed.
            from scipy.special import i0e
            s2 = (dQ[:, None])**2
            Qe = self.Qext[None, :]
            Q0 = Q[:, None]
            x = Qe*Q0/s2
            W = (Qe/s2)*np.exp(-0.5*((Qe - Q0)**2)/s2)*i0e(x)*dq[None, :]
            #Q <= 0 has no weight at all: the factor Qe already vanishes
            #there, and this makes it exact rather than merely small.
            W = np.where(Qe > 0.0, W, 0.0)
        W = np.where(np.isfinite(W), W, 0.0)
        norm = W.sum(axis=1, keepdims=True)
        #A dQ so small that the kernel falls between grid points would give a
        #zero row; fall back to nearest-neighbour rather than dividing by
        #zero. Exact, because Q is in Qext.
        bad = (norm[:, 0] <= 0) | ~np.isfinite(norm[:, 0])
        if np.any(bad):
            for i in np.where(bad)[0]:
                W[i] = 0.0
                W[i, int(np.argmin(np.abs(self.Qext - Q[i])))] = 1.0
            norm = W.sum(axis=1, keepdims=True)
        self.W = W/norm

    def __call__(self, Iext):
        """Smear a model already evaluated on self.Qext."""
        return self.W @ np.asarray(Iext, float)

    def __repr__(self):
        return (f"Resolution({self.Q.size} points, dQ/Q "
                f"{np.min(self.dQ/self.Q):.3g}..{np.max(self.dQ/self.Q):.3g}, "
                f"{self.Qext.size}-point model grid)")


# ----------------------------------------------------------------------
def _linearScaleAndBackground(model, obs, weight, fixedScale=None,
                              fixedBackground=None, extraColumns=None,
                              fixedExtras=None, nonNegativeExtras=False):
    """Exact weighted least squares for the LINEAR parts of the model.

        I(Q) = a*model(Q) + b + sum_k c_k * column_k(Q)

    Returns (a, b, c) with c a list, one entry per extra column.

    Minimises sum w^2 (fit - obs)^2 over whichever coefficients are free.
    Any of them may be held at a given value; the rest are still solved
    exactly, because the problem stays linear -- a held term is simply moved
    to the right-hand side.

    WHY EVERY LINEAR TERM BELONGS HERE. Each enters the model linearly, so a
    weighted linear solve gives its exact optimum at no cost, while handing
    it to the nonlinear optimiser would add a strongly correlated parameter
    that has to be searched for. Scale trades against volume fraction;
    background against everything at high Q; a power-law term against both.
    Searching that subspace is wasted effort and makes the covariance of the
    parameters that matter worse.

    `extraColumns` is the caller's business: for an incoherent background
    plus a Porod-like tail the columns are a constant (already covered by b)
    and Q**(-4+d), with the EXPONENT nonlinear and therefore fitted in the
    ordinary way while its amplitude is solved here.
    """
    #nonNegativeExtras: require the extra-column coefficients to be >= 0.
    #
    #A Porod-like amplitude cannot be negative -- it is a surface term, and a
    #negative one would mean scattering removed rather than added. The
    #unconstrained solve does not know that and returns whatever fits best,
    #which has been observed at -4.5e-19: harmless in magnitude but
    #meaningless as a physical quantity, and misleading when reported.
    #
    #For a SINGLE bound the constrained optimum is EXACT rather than
    #approximate. The objective is convex and quadratic, so either the
    #unconstrained optimum already satisfies the bound, or the constrained
    #optimum lies ON the boundary -- in which case fixing that coefficient at
    #zero and re-solving for the rest gives the exact answer. No iteration,
    #no clipping.
    w2 = weight**2
    cols, fixedCols = [], []
    #model and constant first, then any extras, so the returned order is
    #predictable.
    cols.append(np.asarray(model, float))
    fixedCols.append(fixedScale)
    cols.append(np.ones_like(np.asarray(obs, float)))
    fixedCols.append(fixedBackground)
    for i, c in enumerate(extraColumns or ()):
        cols.append(np.asarray(c, float))
        fixedCols.append(None if fixedExtras is None else fixedExtras[i])

    rhs = np.asarray(obs, float).copy()
    freeIdx = []
    for i, (col, fx) in enumerate(zip(cols, fixedCols)):
        if fx is None:
            freeIdx.append(i)
        else:
            rhs = rhs - float(fx)*col          # move the held term across

    coeff = [0.0]*len(cols)
    for i, fx in enumerate(fixedCols):
        if fx is not None:
            coeff[i] = float(fx)

    if freeIdx:
        A = np.stack([cols[i] for i in freeIdx], axis=1)
        Aw = A*weight[:, None]
        bw = rhs*weight
        #COLUMN EQUILIBRATION, and it is not optional.
        #
        #The columns can differ in magnitude by twenty orders. A model built
        #from scattering length densities in 1/cm^2 carries contrast^2 and so
        #has magnitude ~1e20, while the constant background column is 1.
        #np.linalg.lstsq with rcond=None discards singular values below
        #max(M,N)*eps ~ 1e-14 relative, so with a condition number of 1e20
        #the background and power-law coefficients are truncated to EXACTLY
        #zero -- not small, zero -- and the solve reports success.
        #
        #That was not hypothetical: it produced fits returning background = 0
        #and A = 0 that a hand-chosen background of 0.5 and amplitude of
        #2e-3 beat by 30 % in chi-squared, which looked like a broken
        #optimiser and was in fact a discarded singular value.
        #
        #Scaling each column to unit norm brings the condition number down to
        #something the SVD can resolve; the solution is scaled back
        #afterwards, so the answer is unchanged where the old code was
        #already right. Measured on the failing case: (3.5e-20, 0, 0) before,
        #(2.3e-20, 0.5, 2.0e-3) after, against a truth of (2.3e-20, 0.5,
        #2e-3).
        norms = np.linalg.norm(Aw, axis=0)
        norms = np.where((norms > 0) & np.isfinite(norms), norms, 1.0)
        try:
            sol, *_ = np.linalg.lstsq(Aw/norms, bw, rcond=None)
            sol = sol/norms
            if np.all(np.isfinite(sol)):
                for i, v in zip(freeIdx, sol):
                    coeff[i] = float(v)
            else:
                raise np.linalg.LinAlgError("non-finite solution")
        except np.linalg.LinAlgError:
            #Degenerate: a flat or collinear design carries no information.
            #Leave the scale at 1 and put everything into the constant, which
            #is the least-wrong fallback and matches the previous behaviour.
            if 0 in freeIdx:
                coeff[0] = 1.0
            if 1 in freeIdx:
                s = float(np.sum(w2))
                coeff[1] = float(np.sum(w2*rhs)/s) if s > 0 else 0.0

    a, b, extras = coeff[0], coeff[1], coeff[2:]
    #Apply the non-negativity bound, exactly. Only if the coefficient was
    #FREE: a value the caller fixed is theirs to choose, even if negative.
    if nonNegativeExtras and extras:
        needed = [i for i, c in enumerate(extras)
                  if c < 0.0 and (fixedExtras is None
                                  or fixedExtras[i] is None)]
        if needed:
            forced = list(fixedExtras) if fixedExtras is not None \
                else [None]*len(extras)
            for i in needed:
                forced[i] = 0.0
            return _linearScaleAndBackground(
                model, obs, weight, fixedScale, fixedBackground,
                extraColumns, forced, nonNegativeExtras=False)
    return a, b, extras


class PolydisperseFit:
    """Least-squares fit of the polydisperse model to a measured curve.

    parameters : {name: (initial, lower, upper)}
        Names understood are

          meanRadius, srel, phi, closureParam, closureParam2
          pot0, pot1, ...      the potential's own arguments, in order

        Anything not listed is held fixed at the value given in `fixed`.
    """

    #"shell" is here so a shell thickness can be FITTED. It is not consumed
    #by GenericPolydisperseSAS -- it reaches the model only through
    #formfactorFactory, which receives every shape key as a keyword. Any
    #future form-factor parameter should be added the same way.
    SHAPE_KEYS = ("meanRadius", "srel", "phi", "closureParam",
                  "closureParam2", "shell", "rhoCore", "rhoShell",
                  #rhoRatio = rhoShell/rhoCore. The FITTED contrast
                  #parameter: only the ratio is determined by the data, the
                  #magnitude being perfectly correlated with the scale. It
                  #reaches the model through formfactorFactory, which builds
                  #the shell SLD as rhoRatio*rhoCore.
                  "rhoRatio",
                  "linkC",
                  #porodD: the exponent is -4+d, so d = 0 is Porod's law for
                  #a smooth sharp interface and larger d is shallower. Only
                  #the exponent is nonlinear; the amplitude is solved with
                  #the other linear terms.
                  "porodD")

    def __init__(self, Q, I, dI=None, potential="HardSphere",
                 potentialArgs=(), closure="Percus-Yevick",
                 parameters=None, fixed=None, nbins=3, nFF=60,
                 distribution="Schulz", logResiduals=True, shouldStop=None,
                 resolution=None, formfactorFactory=None,
                 fixedScale=None, fixedBackground=None, solverClass=None,
                 linkedArg=None, usePorod=False, fixedPorodAmplitude=None,
                 porodQmin=None, onProgress=None, progressInterval=1.0,
                 warmStart=False, mannAlpha=None):
        #mannAlpha: damping for the Picard/Mann iteration, passed through to
        #every solve. None leaves the solver's default (undamped Picard).
        self.mannAlpha = mannAlpha
        #warmStart: seed each OZ solve with the converged gamma of the
        #previous one instead of starting from zero.
        #
        #WHY IT HELPS. The optimiser moves in small steps, so the previous
        #solution is an excellent guess for the next. Measured on eight
        #nearby volume fractions with Anderson on one component: 1.29x
        #faster. The gain grows with the cost of a single solve -- at seven
        #size classes a solve is 2.5 s rather than 0.015 s -- and grows again
        #for a population fitter making thousands of nearby evaluations.
        #
        #WHY IT MAY ALSO BE MORE CORRECT. The closure equations admit several
        #solutions. A cold start can land on a different branch between
        #adjacent iterations, which makes the residual DISCONTINUOUS in the
        #parameters -- poor food for an optimiser that assumes smoothness.
        #Seeding from the previous gamma tracks one branch continuously; it
        #is the same continuation argument that settled which BPGG root is
        #physical.
        #
        #WHY IT IS NOT THE DEFAULT. Warm starting can also carry a fit ONTO a
        #wrong branch and hold it there, and it is precisely what makes an
        #independent verifier useless. Here the risk is bounded, because the
        #residual check and the min S(Q) >= 0 screen run on every solve. Left
        #off by default so the behaviour only changes when asked for.
        self.warmStart = bool(warmStart)
        self._lastGamma = None
        #onProgress: callable(dict) invoked at most every `progressInterval`
        #seconds with the BEST point found so far, so a caller can show the
        #fit converging instead of a frozen window for twenty seconds.
        #
        #BEST-SO-FAR rather than the current point, deliberately. The current
        #point wanders -- a line search or a Jacobian probe can be far worse
        #than where it started -- and watching it jump about is alarming
        #without being informative. The best point improves monotonically, so
        #what the user sees is progress. It costs nothing extra: bestX and
        #bestCost are already tracked for the interrupt path.
        #
        #THROTTLED because each callback crosses to the GUI thread and
        #triggers a redraw. Unthrottled, a fit of thirty evaluations would
        #redraw thirty times and measurably slow the thing it is reporting
        #on.
        self.onProgress = onProgress
        self.progressInterval = float(progressInterval)
        self._lastProgress = 0.0
        #porodQmin: the Q below which the power law is held constant instead
        #of being extrapolated. None means "the lowest measured Q".
        self.porodQmin = porodQmin
        #usePorod: add a power-law term to the background,
        #
        #    I_bg(Q) = background + A * Q**(-4 + d)
        #
        #Real backgrounds are often not flat. Porod's law gives Q**-4 for a
        #smooth sharp interface; a rough or fractal one is shallower, so the
        #exponent is written -4+d and d is fitted (`porodD`, a SHAPE_KEY).
        #
        #The AMPLITUDE is linear in the model and therefore solved exactly
        #alongside the scale and the constant, never handed to the nonlinear
        #optimiser. Only the exponent is nonlinear, so switching this on
        #costs ONE extra fitted parameter, not two.
        self.usePorod = bool(usePorod)
        self.fixedExtras = (None if fixedPorodAmplitude is None
                            else [fixedPorodAmplitude])
        #linkedArg: (index, c, sourceKey) or None. When given, the potential
        #argument at `index` is not independent -- it is recomputed at every
        #iteration as c times the current value of the shape parameter
        #`sourceKey`.
        #
        #The case this exists for is a square-well or sticky-sphere `delta`,
        #the well WIDTH as an absolute length, tied to a core-shell particle's
        #shell thickness: if the attraction comes from the surface layer then
        #the interaction range and the layer are the same physical quantity,
        #and fitting them independently invites them to trade against each
        #other while both drift.
        #
        #It MUST be recomputed per iteration rather than set once, or delta
        #would be frozen at the starting shell thickness while the shell
        #itself refined away from it -- leaving the potential inconsistent
        #with the form factor and no sign that anything was wrong.
        self.linkedArg = linkedArg
        #solverClass: the OZ solver to use, or None for the package default
        #(Picard). Previously there was NO such parameter, so the fitter
        #always used the default however the interface was set: the GUI read
        #the user's choice, passed it to the calculate path, and silently
        #dropped it here. The symptom was a solver that appeared to fall back
        #to Picard and never recover.
        #
        #Prefer a FIXED-POINT method here. A fit wanders through parameter
        #space, including regions near folds, and the Newton-Krylov family
        #converges to negative-compressibility branches there -- genuine
        #roots, but unphysical ones. A wrong branch during a fit is worse
        #than a slow one, because the optimiser will happily follow it.
        self.solverClass = solverClass
        #fixedScale / fixedBackground: hold either linear parameter at a
        #given value instead of solving for it. Both default to None, i.e.
        #solved exactly as before, so existing behaviour is unchanged.
        #
        #These stay OUT of the nonlinear optimiser deliberately. They enter
        #the model linearly, so a weighted linear solve is exact and free,
        #whereas giving them to the optimiser would add the two most strongly
        #correlated parameters -- scale against volume fraction, background
        #against everything at high Q -- for no gain.
        self.fixedScale = fixedScale
        self.fixedBackground = fixedBackground
        #formfactorFactory: callable taking the current shape parameters as
        #keyword arguments and returning a form-factor object, or None for a
        #plain sphere. A FACTORY rather than an instance, because the form
        #factor may depend on fitted parameters -- a shell thickness, say --
        #and must therefore be rebuilt at every iteration.
        #
        #Before this existed the GUI constructed a CoreShell instance and
        #then never passed it: PolydisperseFit had no formfactor argument at
        #all, so every fit silently used a solid sphere no matter what the
        #interface displayed. It raised nothing, converged, and reported a
        #chi-squared. That is the failure mode this whole package is built to
        #guard against, and it survived here in the fitter itself.
        self.formfactorFactory = formfactorFactory
        self.Q = np.asarray(Q, float)
        self.I = np.asarray(I, float)
        # Weights. With uncertainties, standard chi-squared weighting; without
        # them, uniform weighting on the LOG of the intensity, which is what
        # makes a fit follow a curve spanning decades instead of being
        # dominated by the low-Q points.
        if dI is not None:
            self.dI = np.asarray(dI, float)
            self.weight = 1.0/np.where(self.dI > 0, self.dI, np.inf)
        else:
            self.dI = None
            self.weight = np.ones_like(self.I)
        self.logResiduals = bool(logResiduals) and dI is None
        self.potential = potential
        self.potentialArgs = list(potentialArgs)
        self.closure = closure
        self.distribution = distribution
        self.nbins = int(nbins)
        self.nFF = int(nFF)
        self.parameters = dict(parameters or {})
        self.fixed = dict(fixed or {})
        self.nEvaluations = 0
        self.failures = 0
        #Called before every model evaluation; returning True aborts. Checked
        #BETWEEN evaluations rather than inside the Ornstein-Zernike solve, so
        #the response time is one solve (a few tenths of a second to a few
        #seconds) rather than instant. Interrupting mid-solve would leave the
        #solver in a partially updated state with no converged Gamma to fall
        #back on. Measured: 4.1 s on a fit that would have run 84 s.
        #Optional Resolution operator. When present the model is evaluated on
        #its extended grid and smeared; when absent, on Q directly. Nothing
        #else in the class changes, because smearing is linear and leaves the
        #scale/background elimination intact.
        self.resolution = resolution
        self.shouldStop = shouldStop
        #Best point seen so far, so that an aborted fit still returns
        #something useful rather than discarding the work.
        self.bestCost = np.inf
        self.bestX = None

    # ------------------------------------------------------------------
    def _value(self, name, vector):
        names = list(self.parameters)
        if name in self.parameters:
            return vector[names.index(name)]
        if name in self.fixed:
            return self.fixed[name]
        return None

    def _modelShape(self, vector):
        """I_model(Q) for the current parameter vector, or None on failure."""
        from generic_polydisperse_sas import GenericPolydisperseSAS
        args = list(self.potentialArgs)
        for i in range(len(args)):
            v = self._value(f"pot{i}", vector)
            if v is not None:
                args[i] = v
        kw = {}
        for key in self.SHAPE_KEYS:
            v = self._value(key, vector)
            if v is not None:
                kw[key] = v
        #Apply the link AFTER both the potential arguments and the shape
        #parameters have been read, since it depends on one to set the other.
        if self.linkedArg is not None:
            idx, c, sourceKey = self.linkedArg
            #c itself may be a fitted parameter. Prefer the value from the
            #current vector, falling back to the constant supplied at
            #construction.
            cNow = kw.get("linkC", self.fixed.get("linkC", c))
            source = kw.get(sourceKey, self.fixed.get(sourceKey))
            if source is not None and 0 <= idx < len(args):
                args[idx] = float(cNow)*float(source)
        try:
            #Rebuild the form factor from the CURRENT parameter vector. It
            #cannot be built once outside the loop: a fitted shell thickness
            #would then be frozen at its starting value and would appear to
            #be refined while doing nothing.
            ff = None
            if self.formfactorFactory is not None:
                ffKw = dict(self.fixed)
                ffKw.update(kw)
                ff = self.formfactorFactory(**ffKw)
            #Warm start, through the onSolverCreated hook. It has to be a
            #hook: GenericPolydisperseSAS SOLVES inside __init__, so by the
            #time the constructor returns, sas.solver has already run and
            #seeding it is too late -- which is exactly why an earlier
            #post-construction attempt silently did nothing.
            def onSolver(sol):
                g = self._lastGamma
                if g is not None and hasattr(sol, "setStartValue"):
                    try:
                        #SHAPE. setStartValue wants a FLAT array of
                        #N * numberOfUniquePairs -- p(p+1)/2 pairs, not p^2.
                        #The captured gamma is the full (p, p, N) matrix, so
                        #handing it over directly is rejected with "Size of
                        #initial array is incorrect, not set": a printed
                        #message, not an exception, so the seed was silently
                        #dropped and every solve still ran cold. Take the
                        #upper triangle in the solver's own pair order.
                        a = np.asarray(g, float)
                        if a.ndim == 3:
                            p = a.shape[0]
                            a = np.concatenate([a[i, j] for i in range(p)
                                                for j in range(i, p)])
                        sol.setStartValue(np.ravel(a))
                    except Exception:
                        pass                 # fall back to a cold start
                original = sol.derivePhysicalQuantitiesFromFixpoint

                def capture(x, *a, _o=original, **k):
                    self._lastGamma = np.asarray(x, float).copy()
                    return _o(x, *a, **k)

                sol.derivePhysicalQuantitiesFromFixpoint = capture

            sas = GenericPolydisperseSAS(
                self.potential, tuple(args),
                phi=kw.get("phi", self.fixed.get("phi", 0.1)),
                srel=kw.get("srel", self.fixed.get("srel", 0.2)),
                nbins=self.nbins, nFF=self.nFF,
                closure=self.closure,
                closureParam=kw.get("closureParam"),
                closureParam2=kw.get("closureParam2"),
                meanRadius=kw.get("meanRadius"),
                formfactor=ff,
                solverClass=self.solverClass,
                mannAlpha=self.mannAlpha,
                onSolverCreated=onSolver if self.warmStart else None,
                distribution=self.distribution)
            self.nEvaluations += 1
            if self.resolution is not None:
                return self.resolution(sas.I_exact(self.resolution.Qext))
            return sas.I_exact(self.Q)
        except Exception:
            # Unphysical corner, or a closure with no solution there. Not an
            # error: the optimiser explores such points and must simply be
            # told they are bad.
            self.failures += 1
            return None

    def _extraColumns(self, vector):
        """Linear basis functions beyond the model and the constant.

        Currently one: Q**(-4+d) for a sloping background. Returns None when
        switched off, so the solve reduces to exactly what it was before.

        The column is built on the SMEARING grid and smeared like everything
        else -- it is part of the observable, not a correction bolted on
        afterwards. Smearing the model but not the background would put the
        two on different footings at low Q, where the resolution is widest
        and the power law largest.
        """
        if not self.usePorod:
            return None
        d = self._value("porodD", vector)
        if d is None:
            d = self.fixed.get("porodD", 0.0)
        Q = self.resolution.Qext if self.resolution is not None else self.Q
        col = self._porodColumn(np.asarray(Q, float), float(d))
        if self.resolution is not None:
            col = self.resolution(col)
        return (col,)

    def _porodColumn(self, Q, d):
        """A * Q**(-4+d), CLAMPED below the measured range.

        Q**(-4+d) diverges as Q -> 0, and the smearing grid reaches well
        below the lowest measured point: for a dataset with dQ/Q = 0.55 at
        Q_min the grid would run to negative Q but for a guard clamping it
        at Q_min/1000. Evaluating the power law there is indefensible, and
        measurably so -- the smeared value at the lowest measured point came
        out 6.6e5 times the unsmeared one, and moving that arbitrary guard
        from Q_min/1000 to Q_min/10 changed the answer by five orders of
        magnitude. The result was being set by a constant chosen to avoid a
        division by zero.

        The power law is an EMPIRICAL description of the measured range, not
        a statement about Q -> 0 where any real curve turns over. So it is
        held constant below the lowest measured Q rather than extrapolated:
        the smearing integral stays finite, the answer stops depending on the
        guard, and nothing is claimed about a region with no data.

        The floor is `porodQmin`, defaulting to the lowest measured Q.
        Setting it far below the data restores the divergence this guards
        against, so the effect is worth checking rather than assuming: the
        smeared value at the first measured point is dominated by whatever
        happens at the floor.
        """
        qFloor = (float(self.porodQmin) if self.porodQmin is not None
                  else float(np.min(self.Q)))
        qFloor = max(qFloor, 1e-30)      # never zero: Q**negative overflows
        Qc = np.maximum(Q, qFloor)
        with np.errstate(divide="ignore", invalid="ignore"):
            col = np.power(Qc, -4.0 + d)
        return np.where(np.isfinite(col), col, 0.0)

    def _residuals(self, vector):
        if self.shouldStop is not None and self.shouldStop():
            raise FitAborted(f"interrupted after {self.nEvaluations} evaluations")
        model = self._modelShape(vector)
        if model is None or not np.all(np.isfinite(model)):
            return np.full(self.Q.size, 1e3)
        cols = self._extraColumns(vector)
        a, b, extras = _linearScaleAndBackground(
            model, self.I, self.weight, self.fixedScale, self.fixedBackground,
            cols, self.fixedExtras, nonNegativeExtras=True)
        fit = a*model + b
        for col, c in zip(cols or (), extras):
            fit = fit + c*col
        r = self._logResid(fit) if self.logResiduals else self.weight*(fit - self.I)
        cost = float(np.dot(r, r))
        if np.isfinite(cost) and cost < self.bestCost:
            #Remember the best point, so an aborted fit still returns
            #something useful rather than whatever was being probed when the
            #interrupt arrived.
            self.bestCost, self.bestX = cost, np.array(vector, float)
            #Report it, throttled. Only on an IMPROVEMENT: there is nothing
            #to show otherwise, and it keeps the callback off the hot path
            #for most evaluations.
            if self.onProgress is not None:
                now = time.monotonic()
                if now - self._lastProgress >= self.progressInterval:
                    self._lastProgress = now
                    try:
                        self.onProgress({
                            "Q": self.Q,
                            "fit": np.asarray(fit, float).copy(),
                            "cost": cost,
                            #list(self.parameters), NOT self.names -- `names`
                            #is a local of run(), and referring to it here
                            #raised an AttributeError that the except below
                            #swallowed, so no progress was ever reported and
                            #nothing said why.
                            "parameters": dict(zip(
                                list(self.parameters),
                                np.asarray(vector, float))),
                            "scale": a, "background": b,
                            "nEvaluations": self.nEvaluations})
                    except Exception as exc:
                        #A failing progress callback must never break a fit,
                        #but a SILENT failure is how the above went unnoticed.
                        #Report once and then stay quiet.
                        if not getattr(self, "_progressWarned", False):
                            self._progressWarned = True
                            print(f"[PolydisperseFit] progress callback "
                                  f"failed, disabling it: "
                                  f"{type(exc).__name__}: {exc}")
                        self.onProgress = None
        return r

    def _logResid(self, fit):
        """Residuals on log I -- what makes a fit follow a curve spanning
        decades instead of being dominated by the low-Q points."""
        good = (fit > 0) & (self.I > 0)
        r = np.zeros_like(self.I)
        r[good] = np.log(fit[good]) - np.log(self.I[good])
        r[~good] = 1e3
        return r

    # ------------------------------------------------------------------
    def runMultiStart(self, nStarts=10, maxNfev=200, seed=0, spread=None,
                      verbose=False, onProgress=None):
        """Run the local fit from several scattered starting points.

        Returns the best result, with three extra keys:

            nStarts       how many were attempted
            startResults  [{start, chi2_reduced, parameters, success}, ...]
                          in the order tried, best first after sorting
            distinctMinima  how many distinct chi-squared values were found,
                          within a relative tolerance of 1e-3

        WHY THIS RATHER THAN A GLOBAL OPTIMISER. A genetic algorithm or
        differential evolution needs thousands of evaluations, and here one
        evaluation is a full OZ solve -- 2.5 s at seven size classes, so a
        modest population run is several hours. Ten local fits cost about
        300 evaluations, roughly fifteen minutes, and answer the question
        that actually matters: is the minimum we found the only one?

        That question is not academic for this model. A square-well epsilon
        is repulsive when positive and attractive when negative, and a local
        method cannot cross between those branches: it will report a
        converged fit on whichever side it started. The same applies to the
        sign of a shell contrast. Scattering the starts is the cheapest way
        to find out whether a better basin exists.

        `spread` controls how far starts are scattered, as a fraction of
        each parameter's bounded range; the default samples the full range.
        The FIRST start is always the user's own values, so this can only
        improve on a plain run.
        """
        rng = np.random.default_rng(seed)
        names = list(self.parameters)
        lo = np.array([self.parameters[n][1] for n in names], float)
        hi = np.array([self.parameters[n][2] for n in names], float)
        x0 = np.array([self.parameters[n][0] for n in names], float)

        #Sample in LOG space for parameters spanning orders of magnitude and
        #strictly positive; linearly otherwise. A radius bounded
        #(1e-3, 1e5) sampled linearly would put almost every start above
        #1000, which is useless.
        useLog = (lo > 0) & (hi/np.maximum(lo, 1e-300) > 100.0)

        results, best = [], None
        for k in range(int(nStarts)):
            if k == 0:
                start = x0.copy()            # the user's own values first
            else:
                u = rng.random(len(names))
                if spread is None:
                    start = np.where(
                        useLog,
                        np.exp(np.log(np.maximum(lo, 1e-300))
                               + u*(np.log(np.maximum(hi, 1e-300))
                                    - np.log(np.maximum(lo, 1e-300)))),
                        lo + u*(hi - lo))
                else:
                    #Scatter about the user's point instead of over the whole
                    #range, for a local exploration.
                    span = float(spread)*(hi - lo)
                    start = np.clip(x0 + (2*u - 1)*span, lo, hi)

            sub = copy.copy(self)
            sub.parameters = {n: (float(s), float(a), float(b))
                              for n, s, a, b in zip(names, start, lo, hi)}
            sub.nEvaluations = 0
            sub.bestCost = np.inf
            sub.bestX = None
            sub._lastGamma = None            # each start solves cold
            sub.onProgress = None            # one progress stream, not ten
            try:
                out = sub.run(maxNfev=maxNfev, verbose=verbose)
            except Exception as exc:
                results.append(dict(start=dict(zip(names, start)),
                                    chi2_reduced=float("inf"),
                                    parameters=None, success=False,
                                    error=f"{type(exc).__name__}: {exc}"))
                continue
            results.append(dict(start=dict(zip(names, start)),
                                chi2_reduced=out["chi2_reduced"],
                                parameters=dict(out["parameters"]),
                                success=bool(out.get("success"))))
            if best is None or out["chi2_reduced"] < best["chi2_reduced"]:
                best = out
            if onProgress is not None:
                try:
                    onProgress({"start": k + 1, "of": int(nStarts),
                                "chi2_reduced": out["chi2_reduced"],
                                "best": best["chi2_reduced"]})
                except Exception:
                    pass
            if self.shouldStop is not None and self.shouldStop():
                break

        if best is None:
            raise RuntimeError("every start failed; see startResults")

        #Count DISTINCT minima. Several starts landing on the same
        #chi-squared is evidence the minimum is unique; a spread of values
        #means the landscape is multimodal and the reported answer depends
        #on where the search began -- which is the whole point of doing this.
        found = sorted(r["chi2_reduced"] for r in results
                       if np.isfinite(r["chi2_reduced"]))
        distinct = 0
        last = None
        for c in found:
            if last is None or abs(c - last) > 1e-3*max(abs(last), 1e-30):
                distinct += 1
                last = c
        best = dict(best)
        best["nStarts"] = len(results)
        best["startResults"] = sorted(
            results, key=lambda r: r["chi2_reduced"])
        best["distinctMinima"] = distinct
        return best

    def run(self, maxNfev=200, verbose=False):
        """Fit. Returns a dict with the fitted values and diagnostics."""
        from scipy.optimize import least_squares
        names = list(self.parameters)
        if not names:
            raise ValueError("no free parameters given")
        x0 = np.array([self.parameters[n][0] for n in names], float)
        lo = np.array([self.parameters[n][1] for n in names], float)
        hi = np.array([self.parameters[n][2] for n in names], float)

        aborted = False
        try:
            res = least_squares(self._residuals, x0, bounds=(lo, hi),
                                method="trf", max_nfev=maxNfev,
                                verbose=2 if verbose else 0,
                                diff_step=1e-3)
            xbest = res.x
            success, message = bool(res.success), res.message
        except FitAborted as exc:
            #Report the best point reached, not the last one tried: the
            #optimiser may well have been probing a poor direction when the
            #interrupt arrived. NOTE res is undefined on this path -- every
            #field below must come from xbest, which is what an earlier
            #version got wrong (UnboundLocalError, and only when the abort
            #actually fired).
            aborted = True
            xbest = self.bestX if self.bestX is not None else x0
            success, message = False, f"interrupted by user ({exc})"

        return self._finalise(xbest, names, success=success,
                              message=message, aborted=aborted,
                              #`res` is unbound if the interrupt arrived
                              #before least_squares returned.
                              jac=getattr(locals().get("res"), "jac", None))

    def _finalise(self, xbest, names, success=True, message="",
                  aborted=False, jac=None):
        """Build the result dict from a converged parameter vector.

        Split out of run() so alternative drivers -- fitWithNLopt, and any
        future back end -- return exactly the same dict rather than a
        near-copy that drifts. Everything downstream (the plot, the
        write-back into the entries, the session file) depends on these keys.

        `jac` is the Jacobian at the solution when the driver has one.
        least_squares does; NLopt's derivative-free methods do not, and pass
        None -- uncertainties are then omitted rather than invented.
        """
        model = self._modelShape(xbest)
        cols = self._extraColumns(xbest)
        a, b, extras = _linearScaleAndBackground(
            model, self.I, self.weight, self.fixedScale, self.fixedBackground,
            cols, self.fixedExtras, nonNegativeExtras=True)
        fit = a*model + b
        for col, c in zip(cols or (), extras):
            fit = fit + c*col
        porodAmplitude = float(extras[0]) if extras else None
        ndof = max(self.Q.size - len(names) - 2, 1)
        if self.dI is not None:
            chi2 = float(np.sum(((fit - self.I)/self.dI)**2)/ndof)
        else:
            chi2 = float(np.sum((np.log(fit) - np.log(self.I))**2)/ndof)

        #Parameter uncertainties from the Jacobian least_squares has ALREADY
        #computed -- no further model evaluations, so this is free.
        #
        #    cov = (J^T J)^-1        when dI is trustworthy
        #    cov = chi2_red (J^T J)^-1   otherwise
        #
        #The distinction matters: with real counting statistics the absolute
        #scale of the residuals is meaningful and must not be rescaled; with
        #the log-residual fallback it is arbitrary, so the fit's own scatter
        #has to supply it.
        #
        #This is a LINEARISED estimate. It assumes the cost surface is
        #quadratic near the minimum, so it understates the error for strongly
        #curved or bounded problems, and says nothing about a second minimum.
        #For a defensible error bar on a published number, sample the
        #posterior instead (fitWithBumps(..., method="dream")). What it is
        #very good at is revealing CORRELATION: the synthetic hard-sphere test
        #recovered the radius to 1 % but the volume fraction only to 8 %, and
        #the reason is that phi trades against the scale factor. A correlation
        #matrix says that immediately; a chi-squared value cannot.
        uncertainty, correlation = None, None
        jac = None if aborted else jac
        if jac is not None and np.all(np.isfinite(jac)):
            try:
                JTJ = jac.T @ jac
                #pinv, not inv: a parameter the data does not constrain makes
                #J^T J singular, and that is a result worth reporting rather
                #than an exception.
                cov = np.linalg.pinv(JTJ)
                if self.dI is None:
                    cov = cov*chi2
                sd = np.sqrt(np.clip(np.diag(cov), 0.0, None))
                uncertainty = dict(zip(names, sd))
                outer = np.outer(sd, sd)
                with np.errstate(invalid="ignore", divide="ignore"):
                    correlation = np.where(outer > 0, cov/outer, np.nan)
            except np.linalg.LinAlgError:
                pass

        return {
            "parameters": dict(zip(names, xbest)),
            "scale": a, "background": b,
            #Solved exactly alongside the scale and constant, so it is
            #reported rather than fitted. None when the term is off.
            "porodAmplitude": porodAmplitude,
            "chi2_reduced": chi2,
            "fit": fit, "Q": self.Q,
            "success": success, "message": message, "aborted": aborted,
            "uncertainty": uncertainty, "correlation": correlation,
            "parameterOrder": names,
            "nEvaluations": self.nEvaluations, "failedEvaluations": self.failures,
        }


# ----------------------------------------------------------------------
# Optional back end: bumps
# ----------------------------------------------------------------------
#scipy's least_squares is a LOCAL, derivative-based optimiser. It is the right
#default -- on a three-parameter test it converged in 36 evaluations against
#94 for a derivative-free method, to the same answer -- but it cannot do two
#things this problem needs:
#
#  * GLOBAL search. The workflow otherwise relies on the user getting close by
#    eye first, which is fine for a hard sphere and not fine for a charged
#    Yukawa with a closure parameter.
#  * UNCERTAINTIES. The volume fraction correlates strongly with the scale
#    factor, so a point estimate is arguably the wrong output for it: on
#    synthetic data with 3 % noise it came back 7.9 % off while the radius was
#    within 1 %. That is not a fit failure, it is the data not constraining
#    phi, and only a posterior says so.
#
#bumps (from the DANSE project, and the fitting engine behind Refl1D) supplies
#both: 'de' for differential evolution and 'dream' for DiffeRential Evolution
#Adaptive Metropolis, which returns a posterior rather than a point.
#
#COST. Each evaluation is one Ornstein-Zernike solve. Differential evolution
#and DREAM need hundreds to thousands, so these are overnight tools, not
#interactive ones. The sensible pattern is: global method to find the basin,
#then PolydisperseFit.run() to polish, which takes about 30 evaluations.
#
#STATE OF TESTING. 'amoeba' has been run end to end and reproduces
#least_squares exactly (chi2_red 0.9031, same parameters, 158 evaluations
#against 36). 'de' and 'dream' use the IDENTICAL code path -- only the fitter
#string differs -- but have not been run to completion here, so the
#'uncertainty' and 'correlation' keys below are untested. Treat them as
#plumbing until you have seen one return.
BUMPS_METHODS = ("dream", "de", "amoeba", "newton", "lm", "pt")


# --------------------------------------------------------------------------
# Optional back end: NLopt, for CONSTRAINED fits
# --------------------------------------------------------------------------
#Box bounds are handled by least_squares, and `A >= 0` exactly in the linear
#solve. What neither can express is a constraint RELATING two parameters, and
#this model has a real one: the shell must fit inside the core.
#
#Not quite dR < R, either. `meanRadius` is the MEAN core radius and the
#distribution has width `srel`, so the smallest class has radius about
#R(1 - n*srel) with n the quadrature window. The shell has to fit inside THAT,
#or the small classes acquire negative inner radii long before the mean does.
#
#NLopt is used ONLY for this. Its algorithm portfolio is not the attraction --
#bumps, multi-start and deflation already answer the global-search question --
#and at 2.5 s per evaluation its population methods are out of reach anyway.
#AUGLAG is the right wrapper: it adds an augmented-Lagrangian layer around an
#inner optimiser, so an evaluation-frugal derivative-free method can be kept.

NLOPT_CONSTRAINTS = {
    #name: (description, f(values) <= 0 when satisfied)
    "shellInsideSmallestCore": (
        "shell fits inside the smallest core class: dR < R*(1 - n*srel)",
        lambda v, nSigma=2.5: (
            float(v.get("shell", 0.0))
            - float(v.get("meanRadius", 1e30))
            * (1.0 - nSigma*float(v.get("srel", 0.0))))),
    "phiBelowClosePacking": (
        "volume fraction below random close packing, which RISES with "
        "polydispersity",
        #0.64 is random close packing for MONODISPERSE hard spheres. It is
        #the WRONG limit here: polydispersity raises phi_RCP, because small
        #particles fill the gaps between large ones. Bidisperse mixtures
        #reach well above 0.8, and even modest continuous polydispersity
        #pushes the limit up measurably. Using 0.64 for a polydisperse system
        #would clamp the fit below a volume fraction the sample can actually
        #have -- precisely the systems this code exists for.
        #
        #The linear increase below is a DELIBERATELY CRUDE placeholder,
        #chosen to be approximately right near srel = 0 and to move in the
        #correct direction, not to be quantitative. Desmond & Weeks,
        #Phys. Rev. E 90, 022204 (2014) give a calibrated expression for
        #phi_RCP against polydispersity and skewness; NOBODY HAS CHECKED
        #THEIR COEFFICIENTS AGAINST THIS IMPLEMENTATION, so treat the number
        #below as a soft guard rather than a physical boundary.
        #
        #Note also that this constraint is loose in practice: a fitted phi
        #approaching ANY packing limit is already telling you the model is
        #wrong, and clamping it hides that. It is offered, not applied by
        #default.
        lambda v: (float(v.get("phi", 0.0))
                   - (0.64 + 0.5*float(v.get("srel", 0.0))))),
}


def fitWithNLopt(fitter, constraints=("shellInsideSmallestCore",),
                 method="LN_COBYLA", maxNfev=400, ftol=1e-10):
    """Fit with NLopt, honouring constraints that relate several parameters.

    Returns the same dict as PolydisperseFit.run() -- both go through
    _finalise -- plus

        constraintsApplied : the names used
        constraintValues   : {name: f(x) at the solution}; <= 0 means met

    Check those values. A constraint reported as slightly positive means the
    augmented-Lagrangian tolerance was not reached, not that it was ignored.

    `method` is the INNER optimiser. LN_COBYLA and LN_BOBYQA are
    derivative-free and frugal, which suits a model whose every evaluation is
    an OZ solve. Gradient methods are refused rather than silently stalled:
    there is no analytic derivative here, and finite-differencing would cost
    one evaluation per parameter per step.
    """
    import nlopt

    names = list(fitter.parameters)
    x0 = np.array([fitter.parameters[n][0] for n in names], float)
    lo = np.array([fitter.parameters[n][1] for n in names], float)
    hi = np.array([fitter.parameters[n][2] for n in names], float)

    unknown = [c for c in constraints if c not in NLOPT_CONSTRAINTS]
    if unknown:
        raise KeyError(f"unknown constraint(s) {unknown}; "
                       f"available: {sorted(NLOPT_CONSTRAINTS)}")
    chosen = list(constraints)

    def values(x):
        """What a constraint sees: fitted values laid over the fixed ones."""
        v = dict(fitter.fixed)
        v.update(dict(zip(names, np.asarray(x, float))))
        return v

    def objective(x, grad):
        if grad.size:
            raise RuntimeError(
                "gradient-based NLopt methods need a derivative, which this "
                "model does not provide; use LN_COBYLA or LN_BOBYQA")
        r = fitter._residuals(np.asarray(x, float))
        return float(np.dot(r, r))

    inner = nlopt.opt(getattr(nlopt, method), len(names))
    inner.set_lower_bounds(lo)
    inner.set_upper_bounds(hi)
    inner.set_maxeval(int(maxNfev))
    inner.set_ftol_rel(float(ftol))

    if chosen:
        opt = nlopt.opt(nlopt.AUGLAG, len(names))
        opt.set_local_optimizer(inner)
        opt.set_lower_bounds(lo)
        opt.set_upper_bounds(hi)
        opt.set_maxeval(int(maxNfev))
        opt.set_ftol_rel(float(ftol))
        for cname in chosen:
            _desc, f = NLOPT_CONSTRAINTS[cname]
            opt.add_inequality_constraint(
                lambda x, grad, _f=f: _f(values(x)), 1e-10)
    else:
        opt = inner
    opt.set_min_objective(objective)

    try:
        xbest = np.asarray(opt.optimize(x0), float)
        code = opt.last_optimize_result()
        success, message = code > 0, f"nlopt result code {code}"
    except Exception as exc:
        #A failed run still leaves the best point the residual actually saw.
        if fitter.bestX is None:
            raise
        xbest = np.asarray(fitter.bestX, float)
        success, message = False, f"{type(exc).__name__}: {exc}"

    out = fitter._finalise(xbest, names, success=success, message=message)
    v = values(xbest)
    out["constraintsApplied"] = chosen
    out["constraintValues"] = {c: NLOPT_CONSTRAINTS[c][1](v) for c in chosen}
    return out


def fitWithBumps(fitter, method="de", steps=None, pop=10, burn=100,
                 samples=10000, **options):
    """Fit `fitter` (a PolydisperseFit) with a bumps optimiser.

    Returns the same dict as PolydisperseFit.run(), plus 'bumpsOrder', and --
    when the method samples a posterior ('dream') -- two more keys:

        uncertainty : {name: standard deviation}
        correlation : parameter correlation matrix, in bumpsOrder

    The correlation matrix is the point of using dream here: it shows
    DIRECTLY whether phi and the scale factor are trading against each other,
    which a chi-squared value cannot.
    """
    from bumps.curve import Curve
    from bumps.fitproblem import FitProblem
    import bumps.fitters as bf

    names = list(fitter.parameters)
    if not names:
        raise ValueError("no free parameters given")

    def _core(vector):
        shape = fitter._modelShape(np.asarray(vector, float))
        if shape is None or not np.all(np.isfinite(shape)):
            #A large but FINITE curve: bumps must be able to compare it, and
            #inf or nan would poison the population rather than merely making
            #this point unattractive.
            return np.full_like(fitter.I, np.max(fitter.I)*1e3)
        a, b, _extras = _linearScaleAndBackground(
            shape, fitter.I, fitter.weight,
            getattr(fitter, "fixedScale", None),
            getattr(fitter, "fixedBackground", None))
        return a*shape + b

    #bumps introspects the model signature to discover the parameter names,
    #so **kwargs is rejected: the function has to be built with the actual
    #names as named arguments. Hence the exec of a generated def, which is
    #ugly but is the only way to give an arbitrary parameter set a real
    #signature.
    src = ("def model(x, %s):\n    return _core([%s])\n"
           % (", ".join(names), ", ".join(names)))
    ns = {"_core": _core}
    exec(src, ns)
    model = ns["model"]

    dy = fitter.dI if fitter.dI is not None else None
    curve = Curve(model, fitter.Q, fitter.I, dy,
                  **{n: fitter.parameters[n][0] for n in names})
    for n in names:
        _, lo, hi = fitter.parameters[n]
        getattr(curve, n).range(lo, hi)
    problem = FitProblem(curve)

    opts = dict(options)
    if method == "dream":
        opts.update(burn=burn, samples=samples, pop=pop)
    elif steps is not None:
        opts.update(steps=steps)
    result = bf.fit(problem, method=method, **opts)

    #bumps orders the fitted parameters ITSELF -- alphabetically, not in the
    #order they were declared -- so result.x must be matched by LABEL, not by
    #position. Zipping it with `names` silently transposed srel and phi in an
    #earlier version, giving a plausible-looking fit (chi2_red 95 rather than
    #0.90) with two parameters holding each other's values while the third
    #was correct. Nothing raised; it merely looked like a poor optimiser.
    labels = list(problem.labels())
    byLabel = dict(zip(labels, np.asarray(result.x, float)))
    missing = [n for n in names if n not in byLabel]
    if missing:
        raise RuntimeError(f"bumps did not return values for {missing}; "
                           f"it reported {labels}")
    xbest = np.array([byLabel[n] for n in names], float)

    model_shape = fitter._modelShape(xbest)
    a, b, _extras = _linearScaleAndBackground(
        model_shape, fitter.I, fitter.weight,
        getattr(fitter, "fixedScale", None),
        getattr(fitter, "fixedBackground", None))
    fit = a*model_shape + b
    ndof = max(fitter.Q.size - len(names) - 2, 1)
    if fitter.dI is not None:
        chi2 = float(np.sum(((fit - fitter.I)/fitter.dI)**2)/ndof)
    else:
        chi2 = float(np.sum((np.log(fit) - np.log(fitter.I))**2)/ndof)

    out = {
        "parameters": dict(zip(names, xbest)),
        "scale": a, "background": b, "chi2_reduced": chi2,
        "fit": fit, "Q": fitter.Q, "success": True,
        "message": f"bumps/{method}", "aborted": False,
        "nEvaluations": fitter.nEvaluations,
        "failedEvaluations": fitter.failures,
        "bumpsOrder": labels,
    }
    dx = getattr(result, "dx", None)
    if dx is not None:
        dxByLabel = dict(zip(labels, np.asarray(dx, float)))
        out["uncertainty"] = {n: dxByLabel[n] for n in names if n in dxByLabel}
    state = getattr(problem, "state", None) or getattr(result, "state", None)
    if state is not None:
        try:
            draw = state.draw()
            out["correlation"] = np.corrcoef(draw.points.T)
        except Exception:
            pass
    return out
