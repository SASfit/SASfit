import numpy as np
import pytest

from pysldprofile import DensityProfileFit, GaussianResolution, SlitResolution, models


def _sphere_data(seed=1, noise=0.05, res=None, R=7.5):
    q = np.linspace(0.12, 1.0, 50)
    return models.simulate(q, models.homogeneous, R, rel_noise=noise, abs_noise=0.0, resolution=res, seed=seed)


def test_solve_fixed_hyperparameters_homogeneous_sphere():
    q, I, s, It, sc = _sphere_data()
    f = DensityProfileFit(q, I, s, n=40)
    sol, _ = f.best_alpha(7.5)
    assert sol.converged
    assert 0.5 < sol.chi2 / q.size < 1.5
    assert abs(sol.Rg() - np.sqrt(0.6) * 7.5) < 0.1
    assert np.isclose(np.median(sol.rho), np.sqrt(sc), rtol=0.1)


def test_fit_finds_r_max_with_slit_smearing():
    res = SlitResolution(0.1)
    q, I, s, It, sc = _sphere_data(res=res)
    f = DensityProfileFit(q, I, s, n=40, resolution=res)
    r = f.fit(r_max_range=(6.0, 9.5), n_coarse=8, n_fine=7)
    assert abs(r.best.r_max - 7.5) < 0.3
    assert abs(r.Rg[0] - np.sqrt(0.6) * 7.5) < 0.1
    assert r.rho_err.shape == r.rho.shape


def test_background_is_recovered():
    q, I, s, It, sc = _sphere_data()
    b = 0.01 * It[0]
    f = DensityProfileFit(q, I + b, s, n=40, background=True)
    sol, _ = f.best_alpha(7.5)
    assert abs(sol.background - b) < 0.2 * b


def test_maxent_profile_is_positive():
    q, I, s, It, sc = _sphere_data()
    f = DensityProfileFit(q, I, s, n=40, regularization="maxent")
    sol, _ = f.best_alpha(7.5)
    assert np.all(sol.rho > 0)
    assert 0.5 < sol.chi2 / q.size < 1.5


@pytest.mark.parametrize("geometry", ["cylinder", "lamella"])
def test_other_geometries(geometry):
    prof = models.core_shell(2.5, 1.0, 0.5)
    q = np.linspace(0.15, 1.5, 50)
    q, I, s, It, sc = models.simulate(q, prof, 5.0, geometry=geometry, seed=3)
    f = DensityProfileFit(q, I, s, n=40, geometry=geometry)
    sol, _ = f.best_alpha(5.0)
    assert 0.5 < sol.chi2 / q.size < 1.5
    inner = sol.rho[: 10].mean()
    outer = sol.rho[25:35].mean()
    assert inner > outer  # core denser than shell


def test_gaussian_resolution_rows_normalised():
    q = np.linspace(0.1, 1, 20)
    qf, R = GaussianResolution(0.02).setup(q)
    assert np.allclose(R.sum(1), 1)
    qf, R = SlitResolution(0.1).setup(q)
    assert np.allclose(R.sum(1), 1)


def test_mass_basis_fits_gaussian_coil_without_minima():
    """Debye data: rho-basis fails (form-factor zeros), radial-mass basis fits with a smooth monotone profile."""
    Rg = 217.0
    q = np.geomspace(0.0045, 0.45, 90)
    x = (q * Rg) ** 2
    I = 2 * (np.exp(-x) + x - 1) / x**2 * 2000
    s = 0.02 * I
    I = I + np.random.default_rng(0).normal(size=q.size) * s
    f = DensityProfileFit(q, I, s, n=60, basis="mass")
    sol, _ = f.best_alpha(700.0)
    assert sol.chi2 / q.size < 1.5
    assert abs(sol.Rg() - Rg) < 0.1 * Rg
    m = sol.mass_on(np.array([20.0, 100.0, 200.0, 400.0]))
    assert np.all(np.diff(m) < 0)  # radial mass decreases monotonically
    f2 = DensityProfileFit(q, I, s, n=60, basis="rho")
    sol2, _ = f2.best_alpha(700.0)
    assert sol2.chi2 > 5 * sol.chi2


def test_mass_basis_amplitude_matches_cusp():
    from pysldprofile import geometry as g
    from scipy.special import sici

    # g = 1 on [0, R] (rho = 1/r^2): F = 4 pi Si(qR)/q
    q = np.linspace(0.01, 1, 30)
    a = g.amplitude_matrix(q, 50.0, 20, "sphere", "mass").sum(1)
    assert np.allclose(a, 4 * np.pi * sici(q * 50.0)[0] / q)


def test_general_exponent_and_scan():
    """Star-like rho ~ r^-4/3: the evidence scan over p picks 4/3; the background is recovered."""
    import os

    from pysldprofile import load_data

    q, I, s, _ = load_data(os.path.join(os.path.dirname(__file__), "..", "examples", "data", "star_p4over3_bg0.05.dat"))
    f = DensityProfileFit(q, I, s, n=50, background=True)
    p, le, sols = f.scan_exponent([0.8, 4 / 3, 2.0], 450.0)
    assert np.isclose(p[np.argmax(le)], 4 / 3)
    assert abs(sols[1].background - 0.05) < 0.003
    assert f.basis == "rho"  # restored


def test_fixed_background_value():
    q, I, s, It, sc = _sphere_data()
    f = DensityProfileFit(q, I + 0.01, s, n=40, background_value=0.01)
    sol, _ = f.best_alpha(7.5)
    assert sol.background == 0.01
    assert 0.5 < sol.chi2 / q.size < 1.5


def test_nonlinear_power_law_background():
    """Coil + A q^-4 upturn + constant: the free exponent n and the coil Rg are recovered."""
    from pysldprofile import Background, Constant, PowerLaw

    Rg = 217.0
    q = np.geomspace(0.0025, 0.45, 100)
    x = (q * Rg) ** 2
    It = 2 * (np.exp(-x) + x - 1) / x**2 * 2000 + 2e-6 * q**-4 + 0.1
    s = 0.02 * It
    I = It + np.random.default_rng(3).normal(size=q.size) * s
    bg = Background([Constant(), PowerLaw(n=3.0, fit_n=True)])
    f = DensityProfileFit(q, I, s, n=50, basis="mass", outer="zero", background=bg)
    sol, _ = f.best_alpha(700.0)
    assert sol.chi2 / q.size < 1.5
    assert abs(sol.bg_params["PowerLaw.n"][0] - 4.0) < 0.2
    assert abs(sol.bg_params["Constant.c"][0] - 0.1) < 0.02
    assert abs(sol.Rg() - Rg) < 0.1 * Rg
    assert sol.forward_amplitude0() > 0  # sign convention


def test_ornstein_zernike_background():
    from pysldprofile import Background, Constant, OrnsteinZernike

    prof = lambda r: 1 - 0.6 * np.exp(-((np.asarray(r) - 3) ** 2) / 0.8**2)  # noqa: E731
    q = np.linspace(0.15, 2.0, 80)
    P = models.intensity_from_profile(q, prof, 5.0)
    P = P / models.intensity_from_profile(np.array([1e-6]), prof, 5.0)[0]
    It = P + 0.02 / (1 + q**2) + 1e-3
    s = 0.01 * It + 1e-4
    I = It + np.random.default_rng(4).normal(size=q.size) * s
    f = DensityProfileFit(q, I, s, n=50, background=Background([Constant(), OrnsteinZernike()]))
    sol, _ = f.best_alpha(5.0)
    assert sol.chi2 / q.size < 1.5
    assert abs(sol.bg_params["OrnsteinZernike.I_L"][0] - 0.02) < 0.004


def test_explicit_uniform_grid_equals_default():
    from pysldprofile.geometry import uniform_grid

    q, I, s, It, sc = _sphere_data()
    a, _ = DensityProfileFit(q, I, s, n=30).best_alpha(7.5)
    b, _ = DensityProfileFit(q, I, s, n=30, grid=uniform_grid(30)).best_alpha(7.5)
    assert np.isclose(a.log_evidence, b.log_evidence, rtol=1e-6)
    assert np.allclose(a.rho, b.rho, rtol=1e-5, atol=1e-8 * np.abs(a.rho).max())


def test_adaptive_grid_improves_coarse_star_fit():
    import os

    from pysldprofile import load_data

    q, I, s, _ = load_data(os.path.join(os.path.dirname(__file__), "..", "examples", "data", "star_p4over3_bg0.05.dat"))
    fu = DensityProfileFit(q, I, s, n=30, basis=4 / 3, background=True)
    su, _ = fu.best_alpha(450.0)
    g = fu.adaptive_grid(450.0)
    w = np.diff(g) * 450.0
    assert w.min() >= min(np.pi / q.max(), 15.0) * 0.999  # resolution floor
    fa = DensityProfileFit(q, I, s, n=30, basis=4 / 3, background=True, grid=g)
    sa, _ = fa.best_alpha(450.0)
    assert sa.log_evidence > su.log_evidence + 3
    assert abs(sa.Rg() - su.Rg()) < 2.0
    r = np.linspace(10, 400, 50)
    assert np.all(np.isfinite(sa.rho_on(r)))


def test_rician_resolution_properties():
    from pysldprofile import Resolution

    q = np.geomspace(0.004, 0.4, 90)
    dq = 0.02 * q + 0.0015
    for k in ("rician", "gaussian"):
        qe, W = Resolution(dq, kernel=k).setup(q)
        assert np.allclose(W.sum(1), 1)
        assert qe.min() > 0
        # dQ -> 0 reproduces the unsmeared curve (measured q are on the grid)
        qe0, W0 = Resolution(1e-12 * q, kernel=k).setup(q)
        f = lambda x: 1 / (1 + (x * 100) ** 2)  # noqa: E731
        assert np.allclose(W0 @ f(qe0), f(q), rtol=1e-12)
    # small dQ/Q: Rician ~ Gaussian; large dQ/Q: they differ
    qe, Wr = Resolution(0.02 * q, kernel="rician").setup(q)
    _, Wg = Resolution(0.02 * q, kernel="gaussian").setup(q)
    f = lambda x: x ** -2.0  # noqa: E731
    assert np.max(np.abs(Wr @ f(qe) / (Wg @ f(qe)) - 1)) < 2e-3
    # FWHM conversion
    _, Wf = Resolution(2.3548200450309493 * 0.02 * q, kernel="rician", fwhm=True).setup(q)
    assert np.allclose(Wf, Wr)


def test_fit_with_rician_resolution():
    from pysldprofile import Resolution

    q = np.linspace(0.15, 1.5, 50)
    dq = 0.05 * q + 0.01
    prof = models.core_shell(2.5, 1.0, 0.5)
    q, I, s, It, sc = models.simulate(q, prof, 5.0, resolution=Resolution(dq), seed=5)
    f = DensityProfileFit(q, I, s, n=40, resolution=Resolution(dq))
    sol, _ = f.best_alpha(5.0)
    assert 0.5 < sol.chi2 / q.size < 1.5
    f0 = DensityProfileFit(q, I, s, n=40)  # ignoring the resolution fits worse
    sol0, _ = f0.best_alpha(5.0)
    assert sol0.chi2 > sol.chi2
