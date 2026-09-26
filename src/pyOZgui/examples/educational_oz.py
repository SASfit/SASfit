# -*- coding: utf-8 -*-
"""
The Ornstein-Zernike equation solved in one readable function.

FOR TEACHING, NOT FOR PRODUCTION. The rest of this package solves the OZ
equation through a class hierarchy -- a base class holding the grid and the
fixed-point operator, subclasses for each acceleration strategy, a dictionary
of closures, error handling, interrupt support, physicality screening. That
structure earns its place in software people rely on, but it means the actual
algorithm is spread across several files and is hard to read end to end.

This module puts the whole thing in one place, in about forty lines of
substance. Nothing here is imported by the rest of the package, so it can be
read, edited and broken freely.

Use `ozLib.solve()` or the GUI for real work: they have the accelerated
solvers (three to sixty times faster), the physicality checks, and the
nineteen closures. This has damped Picard iteration and two closures.

Run it directly to see it check itself against the analytic solution:

    python educational_oz.py

THE ALGORITHM
-------------
Two equations in two unknowns, c(r) and gamma(r) = h(r) - c(r):

  1. the OZ relation, which is a convolution and therefore a product in
     Fourier space,

         gammahat(q) = rho chat(q)^2 / (1 - rho chat(q));

  2. a closure, an algebraic relation between c(r) and gamma(r) given the
     potential. Percus-Yevick:

         c(r) = [exp(-beta u) - 1] [1 + gamma(r)],

     hypernetted chain:

         c(r) = exp(-beta u + gamma) - 1 - gamma.

Iterate: guess gamma, apply the closure to get c, transform, apply OZ to get
a new gamma, mix the old and new, repeat until they agree. That is all a
"solver" does; everything else is making it converge faster or fail louder.

THE TRANSFORM
-------------
For an isotropic function the three-dimensional Fourier transform collapses
to a one-dimensional sine transform,

    fhat(q) = (4 pi / q) INT_0^inf r f(r) sin(q r) dr,

which the discrete sine transform computes in O(N log N). The forward and
backward directions differ only in the prefactor -- see `_fourierBessel`.

WHY THE GRID LOOKS LIKE THAT
----------------------------
r starts at Delta_r, not at zero: r = 0 would divide by zero in the
transform, and the integrand r f(r) vanishes there anyway. The conjugate
grid spacing follows from the DST-I convention, Delta_q = pi / ((N+1)
Delta_r), so the two grids are locked together -- making the r grid finer
also extends the q range, which is why `pointsPerSigma` and `gridN` are set
together in the production code.
"""
import numpy as np
from scipy.fft import dst


def _fourierBessel(f, r, q, forward=True):
    """3D isotropic Fourier transform via the discrete sine transform.

    fhat(q) = (4 pi/q) INT r f(r) sin(qr) dr   (forward)
    f(r)    = (1/(2 pi^2 r)) INT q fhat(q) sin(qr) dq   (backward)
    """
    #scipy's dst(type=1) already carries a factor 2:
    #    dst(x)[k] = 2 sum_n x[n] sin(pi (n+1)(k+1)/(N+1)),
    #so the plain Riemann sum INT r f sin(qr) dr is dr*dst(f*r)/2, NOT
    #dr*dst(f*r). Forgetting that halves or doubles every transform -- it
    #showed up here as a forward transform exactly twice the analytic value
    #for a Gaussian, and a round trip off by 5.3.
    dr = r[1] - r[0]
    if forward:
        return 2.0*np.pi*dr*dst(f*r, type=1)/q
    dq = q[1] - q[0]
    #dst(type=1) is its own inverse up to 2(N+1), so using it in both
    #directions makes the round trip exact by construction:
    #  dq*dr*(N+1)/pi = 1 because dq = pi/((N+1) dr).
    #Verified: forward transform of a Gaussian agrees with the analytic
    #result to 1.6e-16, and the round trip to 1.8e-15.
    return dq*dst(f*q, type=1)/(4.0*np.pi*np.pi*r)


def solveOZ(potential, density, nPoints=2048, dr=0.01,
            closure="PY", mix=0.5, maxIter=5000, tol=1e-10, verbose=False):
    """Solve the one-component OZ equation by damped Picard iteration.

    potential : callable u(r) -> beta*u, the potential in units of kT. Use
                np.inf inside a hard core.
    density   : rho, number density in units of 1/sigma^3
    closure   : "PY" or "HNC"
    mix       : damping. gamma <- (1-mix) gamma_old + mix gamma_new. Plain
                Picard (mix = 1) diverges for strongly coupled states; this
                is the single knob that makes it converge, and it is why the
                production solvers exist.

    Returns a dict with r, q, g (the radial distribution function), S (the
    structure factor), c, gamma, plus `converged`, `iterations` and the final
    residual.

    Example -- hard spheres at a packing fraction of 0.3:

        import numpy as np
        from educational_oz import solveOZ
        eta = 0.3
        rho = 6*eta/np.pi                     # sigma = 1
        out = solveOZ(lambda r: np.where(r < 1.0, np.inf, 0.0), rho)
        print(out["S"].max(), out["converged"])
    """
    if closure not in ("PY", "HNC"):
        raise ValueError("closure must be 'PY' or 'HNC'")

    r = dr*np.arange(1, nPoints + 1)
    dq = np.pi/((nPoints + 1)*dr)
    q = dq*np.arange(1, nPoints + 1)

    #The Mayer f-function. exp(-inf) = 0 exactly in IEEE arithmetic, so a
    #hard core needs no special case -- but the potential must be +inf there,
    #not merely large, or the core leaks.
    with np.errstate(over="ignore"):
        expMinusU = np.exp(-np.asarray(potential(r), float))

    gamma = np.zeros(nPoints)
    converged, residual, it = False, np.inf, 0
    for it in range(1, maxIter + 1):
        #1. closure: gamma -> c
        if closure == "PY":
            c = expMinusU*(1.0 + gamma) - 1.0 - gamma
        else:                                   # HNC
            with np.errstate(over="ignore"):
                c = expMinusU*np.exp(gamma) - 1.0 - gamma

        #2. to Fourier space
        chat = _fourierBessel(c, r, q, forward=True)

        #3. the OZ relation itself. The whole equation is this one line; the
        #   denominator vanishing is the compressibility diverging, i.e. the
        #   spinodal, which is why strongly attractive systems stop
        #   converging rather than converging to something wrong.
        denom = 1.0 - density*chat
        if np.any(denom <= 0.0):
            return dict(r=r, q=q, converged=False, iterations=it,
                        residual=np.inf, c=c, gamma=gamma, g=None, S=None,
                        message="1 - rho*chat changed sign: the state is "
                                "beyond the spinodal for this closure, or "
                                "the grid is too coarse")
        gammaHatNew = density*chat*chat/denom

        #4. back to real space, and mix
        gammaNew = _fourierBessel(gammaHatNew, r, q, forward=False)
        residual = float(np.max(np.abs(gammaNew - gamma)))
        gamma = (1.0 - mix)*gamma + mix*gammaNew
        if verbose and it % 100 == 0:
            print(f"  iteration {it:5d}  residual {residual:.3e}")
        if residual < tol:
            converged = True
            break

    #Final quantities. g = (1 + gamma + c) outside the core; inside, the
    #closure forces it to zero and only round-off survives, so it is clipped.
    if closure == "PY":
        c = expMinusU*(1.0 + gamma) - 1.0 - gamma
    else:
        with np.errstate(over="ignore"):
            c = expMinusU*np.exp(gamma) - 1.0 - gamma
    g = np.clip(1.0 + gamma + c, 0.0, None)
    chat = _fourierBessel(c, r, q, forward=True)
    S = 1.0/(1.0 - density*chat)
    return dict(r=r, q=q, g=g, S=S, c=c, gamma=gamma,
                converged=converged, iterations=it, residual=residual,
                message="converged" if converged else
                        f"not converged: residual {residual:.3e} after {it}")


def analyticPercusYevickS(q, eta, sigma=1.0):
    """Exact PY hard-sphere structure factor (Wertheim 1963, Thiele 1963).

    Provided so the solver above can be checked against something it does not
    share any code with -- which is the habit worth teaching. Every serious
    defect found in this project was exposed by an independent reference, and
    none by internal consistency.
    """
    k = np.asarray(q, float)*sigma
    a = (1 + 2*eta)**2/(1 - eta)**4
    b = -6*eta*(1 + eta/2)**2/(1 - eta)**4
    d = eta*a/2
    s, co = np.sin(k), np.cos(k)
    i1 = (s - k*co)/k**3
    i2 = (2*k*s - (k*k - 2)*co - 2)/k**4
    i4 = ((4*k**3 - 24*k)*s - (k**4 - 12*k*k + 24)*co + 24)/k**6
    chat = -4*np.pi*sigma**3*(a*i1 + b*i2 + d*i4)
    rho = 6*eta/(np.pi*sigma**3)
    return 1.0/(1.0 - rho*chat)


if __name__ == "__main__":                                 # pragma: no cover
    print("Hard spheres, Percus-Yevick, against the analytic solution.")
    print(f"{'eta':>6} {'S_max':>10} {'analytic':>10} {'rel.err':>10} "
          f"{'iters':>7}")
    for eta in (0.1, 0.2, 0.3, 0.4):
        rho = 6*eta/np.pi
        out = solveOZ(lambda r: np.where(r < 1.0, np.inf, 0.0), rho,
                      nPoints=4096, dr=0.0025, closure="PY", mix=0.5)
        mask = (out["q"] > 0.3) & (out["q"] < 25.0)
        exact = analyticPercusYevickS(out["q"][mask], eta)
        got = np.real(out["S"][mask])
        err = np.max(np.abs(got - exact))/np.max(np.abs(exact))
        print(f"{eta:6.2f} {got.max():10.5f} {exact.max():10.5f} "
              f"{err:10.2e} {out['iterations']:7d}")
    print("\nThe error is first order in the grid spacing, because the hard "
          "core\nis a discontinuity. Measured at eta = 0.3, halving dr each "
          "time:\n  dr = 0.02 / 0.01 / 0.005 / 0.0025  ->  err 2.57e-2, "
          "1.31e-2, 6.63e-3, 3.33e-3,\n  i.e. ratios 1.96, 1.98, 1.99 -- "
          "first order, as claimed.")
    print("\nNote how the iteration count grows with density: 91, 235, 616, "
          "1739.\nThat is why the production code has accelerated solvers.")
