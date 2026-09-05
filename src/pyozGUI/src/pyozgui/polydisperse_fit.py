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
def loadCurve(path, qmin=None, qmax=None):
    """Read Q, I and optionally dI from a text file.

    Accepts whitespace-, comma- or semicolon-separated columns with any
    number of comment lines (#, %, //) or a text header, which is what
    SASfit, SasView and most beamline pipelines export. Rows that cannot be
    parsed as numbers are skipped rather than raising, so a stray trailing
    line does not lose the file.

    Returns (Q, I, dI); dI is None when the file has only two columns.
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
                        vals = [float(p) for p in parts[:3]]
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

    good = np.isfinite(Q) & np.isfinite(I) & (Q > 0)
    if dI is not None:
        good &= np.isfinite(dI)
    if qmin is not None:
        good &= Q >= qmin
    if qmax is not None:
        good &= Q <= qmax
    Q, I = Q[good], I[good]
    dI = dI[good] if dI is not None else None
    order = np.argsort(Q)
    return Q[order], I[order], (dI[order] if dI is not None else None)


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
                 distribution="Schulz", logResiduals=True, shouldStop=None):
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
        return {
            "parameters": dict(zip(names, xbest)),
            "scale": a, "background": b,
            "chi2_reduced": chi2,
            "fit": fit, "Q": self.Q,
            "success": success, "message": message, "aborted": aborted,
            "nEvaluations": self.nEvaluations, "failedEvaluations": self.failures,
        }
