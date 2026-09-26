#!/usr/bin/env python3
"""Unit tests for the numerical pieces that have failed silently before.

    cd src/pyOZgui/tools
    python numerics_test.py

Writes numerics_test.json; progress to stderr. No pytest dependency -- the
project's other tools are plain scripts writing JSON and this follows them.

WHY THESE FOUR, and not a general test suite.

Each guards a defect that was found in session 5 by measurement, having
survived review, reasoning and in three cases an explicit assurance that the
code was correct. They share a shape: the wrong answer looked entirely
plausible, nothing raised, and the only way to see it was to compute a number
and compare it with what it should have been.

  1. the linear solve, where a condition number of 1e20 made numpy discard
     two of three coefficients as noise and return exactly zero;
  2. the resolution kernels, where a Gaussian was used for years in a regime
     it does not describe, and where a mis-normalised kernel would be
     invisible;
  3. the size-class quadrature, where the moment matching everything
     downstream assumes is never otherwise checked;
  4. the power-law clamp, where the answer depended on an arbitrary guard
     against division by zero rather than on the physics.

A test that merely runs the code would have passed in every one of those
cases. These assert VALUES.
"""
import json
import os
import sys

import numpy as np

_HERE = os.path.dirname(os.path.abspath(__file__))
for _cand in (_HERE,
              os.path.join(_HERE, os.pardir, "src", "pyozgui"),
              os.path.join(_HERE, os.pardir)):
    _cand = os.path.abspath(_cand)
    if os.path.isfile(os.path.join(_cand, "ozLib.py")):
        if _cand not in sys.path:
            sys.path.insert(0, _cand)
        break
else:
    raise ImportError(f"cannot locate ozLib.py relative to {_HERE!r}")

OUT = os.environ.get("NUMERICS_OUT", "numerics_test.json")
RESULTS = {}


def _say(m):
    print(m, file=sys.stderr, flush=True)


def _save():
    with open(OUT, "w") as fh:
        json.dump(RESULTS, fh, indent=2, sort_keys=True)
        fh.write("\n")


def check(name, ok, detail=""):
    RESULTS[name] = {"pass": bool(ok), "detail": detail}
    _save()
    return bool(ok)


# ---------------------------------------------------------------------------
def testLinearSolveConditioning():
    """The linear solve must survive columns spanning twenty orders.

    THE DEFECT. Scale, background and the power-law amplitude enter the
    intensity linearly and are obtained exactly by weighted least squares at
    every iteration. But the design columns need not be comparable: a model
    built from scattering length densities in 1/cm^2 carries the contrast
    SQUARED, so it has magnitude ~1e20 while the constant column is 1.

    numpy.linalg.lstsq, like any rank-revealing solver, discards singular
    values below max(M,N)*eps. At a condition number of 1e20 the background
    and power-law coefficients therefore came back as EXACTLY zero, and the
    solve reported success. The symptom was a fit that a hand-chosen
    background beat by 30 per cent in chi-squared -- indistinguishable from a
    broken optimiser, and it took most of a session to find.

    Column equilibration fixes it. This test is the regression guard: it
    fails if anyone removes the scaling, and it fails LOUDLY because the
    coefficients come back as zero rather than as something merely inaccurate.
    """
    from polydisperse_fit import _linearScaleAndBackground as lsb
    Q = np.logspace(np.log10(0.018), np.log10(1.18), 171)
    #A model of magnitude 1e20, as an SLD in 1/cm^2 produces.
    m = (np.abs(np.sin(9*Q))/(1 + Q**2) + 0.05)*1e20
    col = Q**(-2.5)
    trueScale, trueBg, trueA = 2.3e-20, 0.5, 2e-3
    obs = trueScale*m + trueBg + trueA*col
    w = 1.0/(0.02*obs)

    a, b, c = lsb(m, obs, w, extraColumns=(col,))
    ok = (np.isclose(a, trueScale, rtol=1e-4)
          and np.isclose(b, trueBg, rtol=1e-4)
          and np.isclose(c[0], trueA, rtol=1e-4))
    check("linearSolve_illConditioned", ok,
          f"scale={a:.5e} (true {trueScale:.1e}), bg={b:.6f} (true {trueBg}), "
          f"A={c[0]:.5e} (true {trueA:.0e}); "
          f"cond={np.linalg.cond(np.stack([m, np.ones_like(m), col], 1)):.2e}")

    #Well-conditioned problems must be UNCHANGED by the equilibration.
    m2 = np.abs(np.sin(9*Q)) + 0.4
    obs2 = 3.7*m2 + 0.42
    a2, b2, _ = lsb(m2, obs2, np.ones_like(Q))
    check("linearSolve_wellConditioned", 
          np.isclose(a2, 3.7, rtol=1e-9) and np.isclose(b2, 0.42, rtol=1e-9),
          f"scale={a2:.8f} (true 3.7), bg={b2:.8f} (true 0.42)")

    #A held coefficient must stay held, and the rest still solved exactly.
    a3, b3, _ = lsb(m2, obs2, np.ones_like(Q), fixedBackground=0.42)
    check("linearSolve_fixedBackground",
          np.isclose(a3, 3.7, rtol=1e-9) and np.isclose(b3, 0.42),
          f"scale={a3:.8f}, bg held at {b3}")

    #The power-law amplitude is constrained non-negative: a Porod term is a
    #surface contribution and cannot remove scattering. When the data would
    #prefer a negative one the constraint must BIND at zero, exactly, rather
    #than be approximated.
    obs3 = 2.0*m2 + 0.5 - 5e-3*col
    _, _, cFree = lsb(m2, obs3, np.ones_like(Q), extraColumns=(col,))
    _, _, cCons = lsb(m2, obs3, np.ones_like(Q), extraColumns=(col,),
                      nonNegativeExtras=True)
    check("linearSolve_nonNegativeAmplitude",
          cFree[0] < 0 and cCons[0] == 0.0,
          f"unconstrained A={cFree[0]:+.3e} (should be negative), "
          f"constrained A={cCons[0]:+.3e} (should be exactly 0)")


# ---------------------------------------------------------------------------
def testResolutionKernels():
    """Both kernels must vanish into the unsmeared curve as dQ -> 0.

    THE POINT. A mis-normalised smearing kernel is invisible: the curve still
    looks like a scattering curve, merely scaled or shifted a little, and
    every fit absorbs the difference into the scale. The dQ -> 0 limit is the
    one check that catches it, because a kernel whose rows do not sum to one
    cannot reproduce the input however narrow it becomes.

    The second test is the reason the Rician kernel exists at all. A
    small-angle instrument broadens Q in the detector plane, in two
    dimensions, and the data are radially averaged; the distribution of the
    MAGNITUDE is Rician, not Gaussian. The Gaussian is its large-Q0/sigma
    limit and is fine wherever dQ/Q is small -- but for the data this was
    developed against dQ/Q reaches 0.55 at the lowest point, where a Gaussian
    puts 44 per cent of that point's weight below Q_min and some of it at
    NEGATIVE Q. The leading factor of Q in the Rician form excludes that by
    construction.
    """
    from polydisperse_fit import Resolution
    Q = np.logspace(np.log10(0.02), np.log10(1.2), 80)
    f = lambda q: (np.sin(30*q)/(30*q))**2 + 1e-6

    for kernel in ("gaussian", "rician"):
        r = Resolution(Q, 1e-9*Q, kernel=kernel)
        err = float(np.max(np.abs(r(f(r.Qext))/f(Q) - 1.0)))
        check(f"resolution_{kernel}_dQtoZero", err < 1e-12,
              f"max relative error {err:.3e} (must vanish: a kernel that "
              f"cannot reproduce the input as dQ -> 0 is mis-normalised)")

        r2 = Resolution(Q, 0.05*Q + 0.01, kernel=kernel)
        rows = np.asarray(r2.W).sum(axis=1)
        check(f"resolution_{kernel}_normalised",
              bool(np.allclose(rows, 1.0, atol=1e-12)),
              f"row sums span {rows.min():.12f}..{rows.max():.12f}")

    #At a wide resolution the Rician form must place LESS weight below Q_min
    #than the Gaussian, since the factor of Q suppresses the origin. This is
    #the whole reason for preferring it, so it is asserted rather than
    #assumed.
    dQ = 0.55*Q                       # deliberately wide, as the real data are
    below = {}
    for kernel in ("gaussian", "rician"):
        r = Resolution(Q, dQ, kernel=kernel)
        mask = np.asarray(r.Qext) < Q.min()
        below[kernel] = float(np.asarray(r.W)[0, mask].sum())
    check("resolution_ricianSuppressesOrigin",
          below["rician"] < below["gaussian"],
          f"weight of the first point below Q_min: "
          f"gaussian {below['gaussian']:.4f}, rician {below['rician']:.4f}")


# ---------------------------------------------------------------------------
def testDistributionMoments():
    """Every offered distribution must reproduce its mean and width.

    The quadrature nodes are MOMENT-MATCHED, which is why three size classes
    suffice where naive binning needs dozens, and why the manuscript's
    accuracy claims hold at small class counts. Nothing else checks it: a
    distribution whose nodes drifted would give a plausible curve at a
    slightly wrong polydispersity, and the fit would absorb the difference.

    Gaussian is expected to be the worst at large srel and is tested at a
    looser tolerance for a stated reason: at 30 per cent width a Gaussian puts
    real weight below zero, and truncating it shifts both moments. That is a
    property of the distribution, not a defect, but it makes Gaussian the
    least trustworthy of the five at wide distributions and the test records
    that rather than hiding it.
    """
    import polydisperse_nodes as P
    for dist in P.DISTRIBUTIONS:
        for srel in (0.10, 0.25):
            sig, x = P.quantileClasses(dist, srel, 40, meanSigma=1.0)
            m1 = float(np.sum(x*sig))
            m2 = float(np.sum(x*sig*sig))
            cv = float(np.sqrt(max(m2 - m1*m1, 0.0))/m1)
            tol = 5e-3 if dist == "Gaussian" else 1e-4
            check(f"moments_{dist}_srel{srel}",
                  abs(m1 - 1.0) < tol and abs(cv - srel) < tol,
                  f"mean={m1:.6f} (want 1), cv={cv:.6f} (want {srel}), "
                  f"tolerance {tol}")


# ---------------------------------------------------------------------------
def testPorodClamp():
    """The smeared power law must not depend on the division-by-zero guard.

    THE DEFECT. A background term A*Q^(-4+d) diverges at the origin, while
    the smearing kernel reaches below the measured range -- for data with
    dQ/Q = 0.55 at Q_min the grid would run to NEGATIVE Q but for a guard
    clamping it at Q_min/1000. Evaluating the power law there is
    indefensible and was measurably so: the smeared value at the lowest
    measured point came out 6.6e5 times the unsmeared one, and moving that
    arbitrary guard from Q_min/1000 to Q_min/10 changed the answer by five
    orders of magnitude.

    The power law describes the MEASURED range and says nothing about
    Q -> 0, where any real curve turns over. Holding it constant below the
    lowest measured Q removes the arbitrariness, and this test asserts that
    the result no longer moves when the guard does.
    """
    from polydisperse_fit import Resolution
    Q = np.logspace(np.log10(0.018), np.log10(1.18), 171)
    dQ = 0.55*Q*np.exp(-2*Q)          # wide at low Q, as real dQ columns are
    qMin = float(Q.min())

    values = {}
    for frac in (1e-3, 1e-2, 1e-1):
        lo = max(float(np.min(Q - 3*dQ)), qMin*frac)
        n = max(int(3*Q.size), Q.size + 2)
        Qe = np.unique(np.concatenate(
            [np.linspace(lo, float(np.max(Q + 3*dQ)), n), Q]))
        dq = np.gradient(Qe)
        z = (Qe[None, :] - Q[:, None])/dQ[:, None]
        W = np.exp(-0.5*z*z)*dq[None, :]
        W = W/W.sum(axis=1, keepdims=True)
        #CLAMPED, as PolydisperseFit._porodColumn does.
        values[frac] = float((W @ np.power(np.maximum(Qe, qMin), -2.5))[0])

    v = list(values.values())
    spread = (max(v) - min(v))/max(abs(x) for x in v)
    check("porodClamp_guardIndependent", spread < 0.05,
          f"smeared value at Q_min across guards 1e-3/1e-2/1e-1: "
          f"{v[0]:.6g}, {v[1]:.6g}, {v[2]:.6g}; relative spread "
          f"{spread:.4f} (must be small: unclamped it was five orders)")

    #And the clamp must be INVISIBLE where it should be. Away from the low-Q
    #edge the smeared power law is unaffected by it, so a test that only
    #checked stability could be satisfied by clamping everything flat.
    r = Resolution(Q, dQ, kernel="rician")
    colClamped = np.power(np.maximum(np.asarray(r.Qext), qMin), -2.5)
    smeared = np.asarray(r(colClamped), float)
    raw = np.power(Q, -2.5)
    i = int(np.argmin(np.abs(Q - 0.5)))
    ratio = float(smeared[i]/raw[i])
    check("porodClamp_invisibleAwayFromEdge", abs(ratio - 1.0) < 0.2,
          f"smeared/unsmeared at Q=0.5 is {ratio:.4f} (must be near 1: the "
          f"clamp may not distort the range that was measured)")


# ---------------------------------------------------------------------------
def testDreamPlumbing():
    """The dream posterior keys must exist and have the right SHAPES.

    WHY A TEST RATHER THAN A RUN. `fitWithBumps(..., method="dream")` has been
    wired for some time and never once run to completion -- at 2.5 s per model
    evaluation and ten thousand evaluations it is an overnight job, so nobody
    has finished one. It is now reachable from the Fitter dropdown, which
    means a user can invoke untested plumbing.

    The physics is not the question here; three pieces of plumbing are, and
    they have different failure modes:

      * `result.dx` must exist, or `uncertainty` is silently absent. Mild:
        a missing key is noticed.
      * `problem.state` / `result.state` is read inside a bare
        `except Exception: pass`, so if the attribute path is wrong
        `correlation` VANISHES WITH NO MESSAGE. That is the defect shape this
        whole file exists to catch.
      * `state.draw().points` must be (samples, parameters), because the code
        forms `corrcoef(points.T)`. Transposed, it returns a samples x
        samples matrix -- plausible-looking, enormous, and wrong.

    So this runs a DELIBERATELY TINY chain: 3 size classes, 20 Q points, two
    free parameters, a few hundred evaluations. A couple of minutes rather
    than a night, and it answers all three.

    WHAT IT DOES NOT ANSWER, and must not be read as answering. The data are
    generated by the same model with NO NOISE, so the likelihood is nearly
    flat and chi2 comes out around 4e-7. The uncertainties that come back are
    then essentially the prior widths -- 0.097 on a phi bounded [0.10, 0.40]
    -- and the correlation is near zero (0.06) where phi and srel would be
    expected to correlate strongly. Both are correct outputs for a
    zero-noise fit and neither says the posterior is MEANINGFUL.

    That still needs one run on real data with real error bars. This test
    establishes only that the keys exist, have the right shapes, and are not
    being swallowed by the bare except -- which is what was actually in
    doubt.
    """
    try:
        import bumps                                          # noqa: F401
    except ImportError:
        check("dream_available", True, "bumps not installed -- skipped")
        return

    from polydisperse_fit import PolydisperseFit, fitWithBumps
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    Q = np.logspace(np.log10(0.08), np.log10(2.0), 20)
    ref = G("HardSphere", (), phi=0.25, srel=0.15, nbins=3,
            closure="Percus-Yevick")
    I = 3.0*ref.I_exact(Q) + 0.02
    dI = 0.03*I

    fitter = PolydisperseFit(
        Q, I, dI, potential="HardSphere", closure="Percus-Yevick",
        parameters={"phi": (0.22, 0.10, 0.40), "srel": (0.13, 0.05, 0.30)},
        fixed={"phi": 0.22, "srel": 0.13}, nbins=3)

    out = fitWithBumps(fitter, method="dream",
                       burn=100, samples=2000, pop=8)

    check("dream_completes", bool(out.get("success")),
          f"chi2_red={out.get('chi2_reduced')}, "
          f"{out.get('nEvaluations')} evaluations, "
          f"{out.get('failedEvaluations')} failed")

    #The label-order trap, already met once: bumps orders parameters
    #alphabetically, not as declared. Zipping result.x with `names` silently
    #transposed srel and phi in an earlier version, giving chi2_red 95
    #instead of 0.90 with two parameters holding each other's values.
    pars = out.get("parameters") or {}
    check("dream_parametersByLabel",
          set(pars) == {"phi", "srel"}
          and 0.10 <= pars.get("phi", -1) <= 0.40
          and 0.05 <= pars.get("srel", -1) <= 0.30,
          f"returned {pars}; bumps order was {out.get('bumpsOrder')}")

    unc = out.get("uncertainty")
    ok = isinstance(unc, dict) and bool(unc)
    check("dream_uncertaintyPresent", ok,
          f"{ {k: float(v) for k, v in unc.items()} }" if ok
          else "ABSENT -- result.dx was not populated")
    if isinstance(unc, dict) and unc:
        check("dream_uncertaintyPositive",
              all(isinstance(v, float) and v > 0 for v in unc.values()),
              f"{ {k: float(v) for k, v in unc.items()} }")

    corr = out.get("correlation")
    #THE ONE THAT MATTERS. Absent means the state attribute path is wrong and
    #the bare except swallowed it.
    #
    #The detail string BRANCHES on the outcome. An earlier version printed
    #the failure text unconditionally, so a passing check read "absent:
    #problem.state did not resolve" -- the verdict said one thing and the
    #text beside it said the opposite, which is worse than no text.
    check("dream_correlationPresent", corr is not None,
          "present" if corr is not None else
          "ABSENT: problem.state / result.state did not resolve, and the "
          "bare `except Exception: pass` hid it")
    if corr is not None:
        c = np.asarray(corr, float)
        n = len(pars)
        check("dream_correlationShape", c.shape == (n, n),
              f"{c.shape}" if c.shape == (n, n) else
              f"{c.shape}, want ({n}, {n}): a samples x samples matrix means "
              f"draw.points is transposed relative to corrcoef(points.T)")
        if c.shape == (n, n):
            check("dream_correlationValid",
                  bool(np.allclose(np.diag(c), 1.0, atol=1e-8)
                       and np.all(np.abs(c) <= 1.0 + 1e-8)
                       and np.allclose(c, c.T, atol=1e-8)),
                  f"diagonal {np.diag(c)}, max |off-diagonal| "
                  f"{np.max(np.abs(c - np.eye(n))):.4f}")


# ---------------------------------------------------------------------------
def testCompressibility():
    """S(0) against the exact Percus-Yevick compressibility.

    THE ONLY ABSOLUTE CHECK IN THIS PACKAGE. Every other comparison sets one
    calculation against another -- against jscatter, against mixscatter,
    against a coarser grid -- so all of them can agree while all of them are
    wrong. For monodisperse hard spheres under Percus-Yevick there is a
    closed form,

        S(0) = (1 - phi)^4 / (1 + 2 phi)^2,

    and extrapolating the computed partials to the origin measures accuracy
    rather than agreement.

    WHAT IT FOUND, and why this test now exists. At phi = 0.40 with 100
    radial points per diameter the first-order DST-I gives S(0) low by
    5.27 per cent. The second-order DST-IV, on the same grid, is low by
    0.036 per cent -- 150 times better before any refinement.

    The cause is where the discontinuity lands. A DST-I puts nodes at
    (n+1)dr, so the hard core at r = sigma falls ON a node and is carried by
    a point that is neither inside nor outside; the excluded volume is then
    wrong at first order in dr. A DST-IV puts them at (n+1/2)dr, the core
    falls between points, and the step is resolved to second order.

    WHY IT WENT UNNOTICED FOR SO LONG, which is the part worth remembering.
    The error does not depend on the real-space range at all: extending
    r_max from 41 to 655 sigma changed S(0) in the sixth decimal. And
    refining the resolution DOES reduce it, by a clean factor of 3.96 per
    fourfold step -- so a convergence study sees orderly first-order
    behaviour and concludes the method is working. It is, slowly, towards an
    answer whose leading error is set by grid alignment rather than by
    resolution. Nothing looked wrong: every solve converged, residuals
    reached 1e-12, and S(Q) came out smooth and positive.

    It also explains the mixture validation tab's standing puzzle -- a fixed
    point at 7e-12 differing from two analytic references by 1e-3, immune to
    refinement, growing with phi. Same effect, other side.

    S(0) is the isothermal compressibility, so this is a systematic bias in
    exactly the quantity phi controls: a fit absorbs it by moving phi, and
    the denser the sample the more it moves.
    """
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    def s0(phi, transformType, pps, gridN):
        sas = G("HardSphere", (), phi=phi, srel=1e-6, nbins=1,
                closure="Percus-Yevick", meanRadius=1.0,
                gridN=gridN, pointsPerSigma=pps,
                transformType=transformType)
        #Fit A + B q^2 over the quadratic region directly, bypassing
        #S_partials, so this measures the SOLVE and not the extrapolation
        #code path.
        m = max(2, min(int(np.searchsorted(sas._q, 0.5)), 400, sas._q.size))
        qf = sas._q[:m]
        M = np.stack([np.ones_like(qf), qf*qf], axis=1)
        c, *_ = np.linalg.lstsq(M, sas._S_AL[0, 0, :m], rcond=None)
        return float(c[0])

    for phi in (0.30, 0.40):
        exact = (1.0 - phi)**4/(1.0 + 2.0*phi)**2

        #TYPE 4 must be accurate on a COARSE grid. This is the headline: the
        #threshold is 0.1 per cent against a measured 0.015 and 0.036, so it
        #has a comfortable margin and would still catch a regression that
        #cost an order of magnitude.
        e4 = abs(s0(phi, 4, 100, 4096) - exact)/exact
        check(f"S0_type4_phi{phi}", e4 < 1e-3,
              f"error {e4*100:.4f} % at 100 points per diameter "
              f"(measured 0.015 % at phi=0.30, 0.036 % at 0.40)")

        #TYPE 1 must still converge at FIRST ORDER. A ratio near 4 for a
        #fourfold refinement is the signature; near 1 would mean the error is
        #not discretisation at all, and near 16 would mean someone had
        #quietly fixed the alignment, which would be welcome but should not
        #pass silently.
        a = abs(s0(phi, 1, 100, 4095) - exact)/exact
        b = abs(s0(phi, 1, 400, 16383) - exact)/exact
        ratio = a/b if b > 0 else float("inf")
        check(f"S0_type1_firstOrder_phi{phi}", 3.0 < ratio < 5.5,
              f"{a*100:.3f} % -> {b*100:.3f} % for a fourfold refinement, "
              f"ratio {ratio:.2f} (want ~4; measured 3.96)")

        #AND TYPE 4 MUST BEAT TYPE 1 BY TWO ORDERS on the same grid. Stated
        #as its own check because it is the practical conclusion: choosing
        #the transform matters more than choosing the resolution, and that is
        #the opposite of how a cost table reads.
        check(f"S0_type4BeatsType1_phi{phi}", e4 < a/50.0,
              f"type 1 {a*100:.3f} %, type 4 {e4*100:.4f} % on the same "
              f"grid -- a factor of {a/e4:.0f}")


# ---------------------------------------------------------------------------
def testLowQextrapolation():
    """S_partials below the solver's grid: the code path, not just the solve.

    `testCompressibility` fits the stored array directly and so measures the
    SOLVE. This measures the extrapolation that stands between that array and
    a caller -- a different thing, and the one a fit actually meets.

    WHAT IT REPLACED. The partials used to be CLAMPED below the solver's
    first grid point: held flat at S(q_0). Smearing the lowest measured
    points pulls values in from there, so a structure factor with a strong
    low-q upturn was biased towards its value at q_0, and the bias grew with
    the resolution width. The replacement is a Taylor form,
    S_ij(q) = A + B q^2, which is the leading behaviour rather than a
    convenient fit: S(q) for a liquid is even and analytic at the origin,
    since the transform of a short-ranged radially symmetric h(r) has no odd
    term.

    A AND B COME FROM A FIT OVER THE WHOLE QUADRATIC REGION, not from the
    two lowest points. The grid is uniform in q with spacing pi/r_max, so a
    finer grid puts MORE points below any given q; a two-point estimate
    would determine B from the difference of two nearly equal numbers and
    get worse as the grid is refined, which is precisely backwards. The
    stability check below is what catches a regression to that.
    """
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    for phi in (0.10, 0.30, 0.40):
        exact = (1.0 - phi)**4/(1.0 + 2.0*phi)**2
        sas = G("HardSphere", (), phi=phi, srel=1e-6, nbins=1,
                closure="Percus-Yevick", meanRadius=1.0,
                gridN=4096, pointsPerSigma=100, transformType=4)
        #Type 4, so the SOLVE is accurate to 0.04 per cent and anything
        #worse is the extrapolation's doing rather than inherited.
        q0 = sas._q[0]/sas._L
        s0 = float(sas.S_partials(np.array([1e-9]))[0][0, 0])
        err = abs(s0 - exact)/exact
        check(f"lowQ_extrapolatesToAnalyticS0_phi{phi}", err < 5e-3,
              f"S(0) = {s0:.6f} against exact {exact:.6f}, {err*100:.3f} % "
              f"(lowest grid point is q*sigma = {sas._q[0]:.4f})")

        #CONTINUITY across the grid edge. A large jump would show up in a
        #smeared fit as a kink at Q_min that no model produces.
        #
        #The tolerance is RELATIVE and deliberately loose. A and B come from
        #a least-squares fit over the whole quadratic region, so the
        #extrapolation does NOT pass exactly through S(q_0) -- it is not
        #supposed to, and a two-point fit that did would be continuous by
        #construction and worse, since it determines B from the difference
        #of two nearly equal numbers. Measured jumps are 1 to 3e-5 absolute
        #on an S(0) of order 0.04 to 0.46, i.e. a few parts in 1e4: small
        #against the 0.04 per cent accuracy the fit itself achieves, which
        #is the figure that matters.
        a = sas.S_partials(np.array([q0*0.999]))[0][0, 0]
        b = sas.S_partials(np.array([q0*1.001]))[0][0, 0]
        rel = abs(a - b)/max(abs(a), 1e-30)
        check(f"lowQ_continuous_phi{phi}", rel < 2e-3,
              f"jump across q_0 is {abs(a-b):.3e} absolute, {rel*100:.4f} % "
              f"relative (a least-squares fit does not interpolate q_0)")

    #STABILITY UNDER REFINEMENT. The extrapolated S(0) must not depend on
    #how many grid points happen to lie in the fit window. Two grids a
    #factor of four apart in resolution, with the real-space range held
    #constant, must agree closely.
    vals = []
    for pps, k in ((100, 12), (400, 14)):
        sas = G("HardSphere", (), phi=0.30, srel=1e-6, nbins=1,
                closure="Percus-Yevick", meanRadius=1.0,
                gridN=2**k, pointsPerSigma=pps, transformType=4)
        vals.append(float(sas.S_partials(np.array([1e-9]))[0][0, 0]))
    spread = abs(vals[0] - vals[1])/abs(vals[0])
    check("lowQ_stableUnderRefinement", spread < 2e-3,
          f"S(0) = {vals[0]:.6f} at 100 pts/diameter, {vals[1]:.6f} at 400, "
          f"relative spread {spread:.2e} (a two-point fit would WORSEN here)")


# ---------------------------------------------------------------------------
def testDivergenceRefused():
    """A diverging S(q) must be refused, not extrapolated through.

    S ~ c q^-alpha means the compressibility is running away -- the state
    point is at or near a spinodal -- and there is no finite S(0) to
    extrapolate TO. A parabola fitted through it returns a perfectly
    respectable-looking number that means nothing, and the fit downstream
    would use it without complaint. Refusing is the only honest option.

    The discriminant is the log-log slope of the trace over the lowest few
    grid points, with the threshold at -0.1. That is not a guess: ordinary
    hard spheres measure within 1e-3 of zero across the useful density
    range, so there are three orders of headroom, while a genuine 1/q
    divergence gives -1.000 exactly.

    Both halves matter. A guard that never fires is useless, and one that
    fires on healthy systems is worse than none -- it would make the tab
    refuse to compute the very states an interacting model exists for.
    """
    from generic_polydisperse_sas import GenericPolydisperseSAS as G

    #MUST NOT FIRE on ordinary systems, across the density range.
    for phi in (0.10, 0.30, 0.45):
        sas = G("HardSphere", (), phi=phi, srel=0.15, nbins=3,
                closure="Percus-Yevick", meanRadius=1.0)
        try:
            v = sas.S_partials(np.array([sas._q[0]/sas._L*0.2]))[0]
            ok = bool(np.all(np.diag(v) > 0))
            check(f"divergence_notRefused_phi{phi}", ok,
                  f"slope {sas._lowQslope:+.5f}, trace {np.trace(v):.5f}")
        except RuntimeError as exc:
            check(f"divergence_notRefused_phi{phi}", False,
                  f"FALSE ALARM: {str(exc)[:110]}")

    #MUST FIRE on a genuine divergence. Built by scaling the stored partials
    #by 1/q rather than by hunting for a real near-critical state point,
    #which would make the test slow and its threshold dependent on finding
    #one.
    sas = G("HardSphere", (), phi=0.30, srel=0.15, nbins=3,
            closure="Percus-Yevick", meanRadius=1.0)
    qq = sas._q.copy()
    sas._S_AL = sas._S_AL*(qq[None, None, :]/qq[0])**-1.0
    sas.__dict__.pop("_lowQslope", None)
    try:
        sas.S_partials(np.array([qq[0]*0.2/sas._L]))
        check("divergence_refused", False,
              "NOT refused: a 1/q divergence was extrapolated through, and "
              "the finite S(0) it returned would be used by a fit")
    except RuntimeError as exc:
        slope = sas.__dict__.get("_lowQslope")
        check("divergence_refused", True,
              f"refused, measured slope {slope:+.4f} (want -1.000 for 1/q)")

    #ABOVE the grid, S -> delta_ij: no correlation at short wavelength.
    sas2 = G("HardSphere", (), phi=0.30, srel=0.15, nbins=3,
             closure="Percus-Yevick", meanRadius=1.0)
    hi = sas2.S_partials(np.array([sas2._q[-1]*2.0/sas2._L]))[0]
    check("highQ_tendsToIdentity",
          bool(np.allclose(hi, np.eye(hi.shape[0]))),
          f"max deviation from the identity {np.max(np.abs(hi - np.eye(hi.shape[0]))):.2e}")


# ---------------------------------------------------------------------------
def main():
    for name, fn in (("linear solve conditioning", testLinearSolveConditioning),
                     ("resolution kernels", testResolutionKernels),
                     ("distribution moments", testDistributionMoments),
                     ("power-law clamp", testPorodClamp),
                     ("compressibility vs analytic PY", testCompressibility),
                     ("low-q extrapolation", testLowQextrapolation),
                     ("divergence refusal", testDivergenceRefused),
                     ("dream posterior plumbing", testDreamPlumbing)):
        _say(name)
        try:
            fn()
        except Exception as exc:
            import traceback
            check(f"{fn.__name__}_RAISED", False,
                  f"{type(exc).__name__}: {exc}")
            traceback.print_exc()

    failed = sorted(k for k, v in RESULTS.items() if not v["pass"])
    RESULTS["FAILURES"] = failed
    _save()
    _say(f"done -- {OUT}; {len(failed)} failure(s): {failed}" if failed
         else f"done -- {OUT}; {len(RESULTS) - 1} checks passed")


if __name__ == "__main__":
    main()
