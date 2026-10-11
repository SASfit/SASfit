import numpy as np
from scipy.special import j1

from pysldprofile import geometry as g
from pysldprofile.regularization import smoothness_operator

Q = np.linspace(0.01, 2.0, 60)
R = 7.5


def test_sphere_amplitude_exact():
    a = g.amplitude_matrix(Q, R, 80, "sphere") @ np.ones(80)
    x = Q * R
    ex = 4 * np.pi * (np.sin(x) - x * np.cos(x)) / Q**3
    assert np.allclose(a, ex, rtol=1e-10, atol=1e-10 * ex[0])


def test_cylinder_and_lamella_amplitude_exact():
    x = Q * R
    a = g.amplitude_matrix(Q, R, 50, "cylinder").sum(1)
    assert np.allclose(a, 2 * np.pi * R**2 * j1(x) / x, atol=1e-10)
    a = g.amplitude_matrix(Q, R, 50, "lamella").sum(1)
    assert np.allclose(a, 2 * np.sin(x) / Q, atol=1e-10)


def test_q_to_zero_limits():
    for geom, v in (("sphere", 4 / 3 * np.pi * R**3), ("cylinder", np.pi * R**2), ("lamella", 2 * R)):
        assert np.isclose(g.volume_weights(R, 40, geom).sum(), v)


def test_sphere_pddf_reproduces_intensity():
    rho = np.where(np.arange(60) < 30, 1.0, -0.4)
    r, p = g.pddf(rho, R, "sphere", r=np.linspace(0, 2 * R, 4001))
    Ip = 4 * np.pi * np.trapezoid(p[None, :] * np.sinc(np.outer(Q, r) / np.pi), r, axis=1)
    Ia = (g.amplitude_matrix(Q, R, 60, "sphere") @ rho) ** 2
    assert np.max(np.abs(Ip - Ia)) < 2e-3 * Ia[0]


def test_radius_of_gyration():
    assert np.isclose(g.radius_of_gyration(np.ones(50), R, "sphere"), np.sqrt(0.6) * R)
    assert np.isclose(g.radius_of_gyration(np.ones(50), R, "cylinder"), R / np.sqrt(2))
    assert np.isclose(g.radius_of_gyration(np.ones(50), R, "lamella"), R / np.sqrt(3))


def test_smoothness_operator():
    D = smoothness_operator(10, "zero")
    assert D.shape == (10, 10) and abs(np.linalg.det(D)) > 0
    D = smoothness_operator(10, "free")
    assert D.shape == (9, 10)
    assert np.allclose(D @ np.ones(10), 0)  # constants are free with free end


def test_general_exponent_matches_closed_forms():
    q = np.linspace(1e-4, 0.5, 30)
    for geom in ("sphere", "cylinder", "lamella"):
        d = g.DIM[geom]
        for p in (0.0, float(d - 1)):
            A = g.amplitude_matrix(q, 400.0, 60, geom, p)
            B = g._amplitude_numeric(q, 400.0, 60, geom, p)
            assert np.allclose(A, B, rtol=1e-7, atol=1e-7 * np.abs(A).max())
    # Rg of rho ~ r^-p on [0, R]: sqrt((3-p)/(5-p)) R
    assert np.isclose(g.radius_of_gyration(np.ones(50), 400.0, "sphere", 4 / 3), np.sqrt((3 - 4 / 3) / (5 - 4 / 3)) * 400)


def test_nonuniform_grid_amplitudes_and_rg():
    # a homogeneous sphere is the same object on any grid
    q = np.linspace(1e-3, 0.5, 30)
    t = g.power_grid(40, 1.7)
    A = g.amplitude_matrix(q, 50.0, 40, "sphere", "rho", t) @ np.ones(40)
    B = g.amplitude_matrix(q, 50.0, 40, "sphere", "rho") @ np.ones(40)
    assert np.allclose(A, B, rtol=1e-10, atol=1e-10 * np.abs(B).max())
    assert np.isclose(g.radius_of_gyration(np.ones(40), 50.0, "sphere", "rho", t), np.sqrt(0.6) * 50)
    # general exponent on a non-uniform grid: quadrature vs closed form
    A = g.amplitude_matrix(q, 50.0, 40, "sphere", 2.0, t)
    B = g._amplitude_numeric(q, 50.0, 40, "sphere", 2.0, grid=t)
    assert np.allclose(A, B, rtol=1e-7, atol=1e-7 * np.abs(A).max())
