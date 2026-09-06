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

    def __init__(self, Q, dQ, fwhm=False, nSigma=3.0, oversample=3):
        Q = np.asarray(Q, float)
        dQ = np.asarray(dQ, float)
        if dQ.shape != Q.shape:
            raise ValueError("dQ must have the same shape as Q")
        if fwhm:
            dQ = dQ/2.3548200450309493
        self.Q = Q
        self.dQ = dQ

        #The extended grid must reach BELOW Q.min(): smearing the lowest
        #measured point pulls in model values at smaller Q, and a structure
        #factor can vary steeply there. Clipped at a small positive value
        #because Q <= 0 is meaningless and the form factor diverges.
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
        z = (self.Qext[None, :] - Q[:, None])/dQ[:, None]
        W = np.exp(-0.5*z*z)*dq[None, :]
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
def _linearScaleAndBackground(model, obs, weight):
    """Exact weighted least-squares scale and background for a fixed shape.

    Minimises sum w^2 (a*model + b - obs)^2 over (a, b). Returns (a, b).
    """
    w2 = weight**2
    S1 = np.sum(w2)
    Sm = np.sum(w2*model)
    Smm = np.sum(w2*model*model)
    So = np.sum(w2*obs)
    Smo = np.sum(w2*model*obs)
    det = Smm*S1 - Sm*Sm
    if not np.isfinite(det) or abs(det) < 1e-300:
        # degenerate: a flat model carries no shape information, so fit the
        # background alone and leave the scale at 1.
        return 1.0, (So/S1 if S1 > 0 else 0.0)
    a = (Smo*S1 - Sm*So)/det
    b = (Smm*So - Sm*Smo)/det
    return a, b


class PolydisperseFit:
    """Least-squares fit of the polydisperse model to a measured curve.

    parameters : {name: (initial, lower, upper)}
        Names understood are

          meanRadius, srel, phi, closureParam, closureParam2
          pot0, pot1, ...      the potential's own arguments, in order

        Anything not listed is held fixed at the value given in `fixed`.
    """

    SHAPE_KEYS = ("meanRadius", "srel", "phi", "closureParam", "closureParam2")

    def __init__(self, Q, I, dI=None, potential="HardSphere",
                 potentialArgs=(), closure="Percus-Yevick",
                 parameters=None, fixed=None, nbins=3, nFF=60,
                 distribution="Schulz", logResiduals=True, shouldStop=None,
                 resolution=None):
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
        try:
            sas = GenericPolydisperseSAS(
                self.potential, tuple(args),
                phi=kw.get("phi", self.fixed.get("phi", 0.1)),
                srel=kw.get("srel", self.fixed.get("srel", 0.2)),
                nbins=self.nbins, nFF=self.nFF,
                closure=self.closure,
                closureParam=kw.get("closureParam"),
                closureParam2=kw.get("closureParam2"),
                meanRadius=kw.get("meanRadius"),
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

    def _residuals(self, vector):
        if self.shouldStop is not None and self.shouldStop():
            raise FitAborted(f"interrupted after {self.nEvaluations} evaluations")
        model = self._modelShape(vector)
        if model is None or not np.all(np.isfinite(model)):
            return np.full(self.Q.size, 1e3)
        a, b = _linearScaleAndBackground(model, self.I, self.weight)
        fit = a*model + b
        r = self._logResid(fit) if self.logResiduals else self.weight*(fit - self.I)
        cost = float(np.dot(r, r))
        if np.isfinite(cost) and cost < self.bestCost:
            #Remember the best point, so an aborted fit still returns
            #something useful rather than whatever was being probed when the
            #interrupt arrived.
            self.bestCost, self.bestX = cost, np.array(vector, float)
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

        model = self._modelShape(xbest)
        a, b = _linearScaleAndBackground(model, self.I, self.weight)
        fit = a*model + b
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
        jac = None if aborted else getattr(res, "jac", None)
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
        a, b = _linearScaleAndBackground(shape, fitter.I, fitter.weight)
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
    a, b = _linearScaleAndBackground(model_shape, fitter.I, fitter.weight)
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
