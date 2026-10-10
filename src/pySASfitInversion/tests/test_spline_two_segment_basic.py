"""
Regression test for the region-adaptive and Glatter-style B-spline solvers
(bayesian_evidence_two_segment, bayesian_evidence_spline,
em_discrepancy_spline) on the real test.dat benchmark, plus the
"target chi2_r unreachable" handling.

Reference values (2026-10-10, sinc_4pi kernel, r = linspace(0, pi/q_min, N_s),
N_s = Shannon number = 95, background fitted over q in [0.094, 0.385]):
  bayesian_evidence_hansen   chi2_r=0.379  high-q chi2 contribution 0.019
  bayesian_evidence_spline   chi2_r=1.07   high-q chi2 contribution 1.72
i.e. the spline basis removes the high-q noise-tracking that every
point-grid solver shows.

Run from the pySASfitInversion directory:
    python3 tests/test_spline_two_segment_basic.py
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.background import fit_background
from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY, SplineOptions, run_solver
from sasfit_inversion.uncertainty import bootstrap_uncertainty


def high_q_chi2(q, b, fitted, dI):
    edge = np.quantile(q, 2 / 3)
    m = q >= edge
    return float(np.mean(((fitted[m] - b[m]) / dI[m]) ** 2))


def setup_sinc():
    d = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = d.q, d.I, d.dI
    b = I - fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI).evaluate(q)
    dmax = np.pi / q.min()
    n_s = int(round((q.max() - q.min()) * dmax / np.pi))
    r = np.linspace(0.0, dmax, n_s)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
    return q, b, dI, A


def main():
    q, b, dI, A = setup_sinc()

    hansen = run_solver("bayesian_evidence_hansen", A, b, dI)
    spline = run_solver("bayesian_evidence_spline", A, b, dI)
    hq_h = high_q_chi2(q, b, hansen.fitted_b, dI)
    hq_s = high_q_chi2(q, b, spline.fitted_b, dI)
    c2_s = spline.chi2_r_history[-1]
    print(f"[hansen point grid] chi2_r={hansen.chi2_r_history[-1]:.3f} high-q chi2={hq_h:.3f}")
    print(f"[B-spline basis]    chi2_r={c2_s:.3f} high-q chi2={hq_s:.3f}")
    assert 0.7 < c2_s < 1.5, c2_s
    assert hq_s > 10 * hq_h, (hq_s, hq_h)  # no high-q noise-tracking
    assert "x_sigma" in spline.diagnostics

    seg = run_solver("bayesian_evidence_two_segment", A, b, dI)
    print(f"[two-segment]       chi2_r={seg.chi2_r_history[-1]:.3f}")
    assert np.isfinite(seg.chi2_r_history[-1])
    seg1 = run_solver("bayesian_evidence_two_segment", A, b, dI, target_chi2_r=1.0)
    assert abs(seg1.chi2_r_history[-1] - 1.0) < 1e-3, seg1.chi2_r_history[-1]

    # Bootstrap + forced chi2_r=1 (was std(p) up to ~400 in the GUI on
    # 2026-10-10: replicates centred on the noisy data have chi2_r ~2, so
    # forcing 1 removed the regularization on some of them).
    run = lambda A_, b_, d_, **k: run_solver("bayesian_evidence_two_segment", A_, b_, d_, **k)
    fn = lambda br: run(A, br, dI, target_chi2_r=1.0)
    boot = bootstrap_uncertainty(fn, b, dI, n_boot=20, rng=np.random.default_rng(0),
                                 center=seg1.fitted_b)
    print(f"[bootstrap, two-segment, chi2_r=1] max std(p)={boot.x_std.max():.3f}")
    assert boot.x_std.max() < 0.1 * np.abs(seg1.x).max(), boot.x_std.max()

    # Spline basis with too few coefficients cannot reach chi2_r=1: must
    # return the closest achievable fit with a warning, not raise.
    few = run_solver("bayesian_evidence_spline", A, b, dI, n_coeffs=20, target_chi2_r=1.0)
    note = few.diagnostics["lambda_selection"]
    print(f"[spline, 20 coeffs, target 1] chi2_r={few.chi2_r_history[-1]:.3f}")
    assert few.chi2_r_history[-1] > 1.0 and "not reachable" in note, note

    # Knot spacing + fine quadrature: log knots must fit at least as well as
    # uniform at 20 coefficients on this PDDF, with balanced high-q residuals.
    from sasfit_inversion import spline_basis
    for sp in spline_basis.SPACINGS:
        Bsp = spline_basis.build_bspline_basis(A.shape[1], 20, spacing=sp)
        assert np.all(Bsp[0] == 0) and np.all(Bsp[-1] == 0), sp
    dmax_ = np.pi / q.min()
    r_q = np.linspace(0.0, dmax_, 4 * (A.shape[1] - 1) + 1)
    A_q = build_size_distribution_kernel(q, r_q, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
    uni = run_solver("bayesian_evidence_spline", A, b, dI, n_coeffs=20, A_quad=A_q)
    lg = run_solver("bayesian_evidence_spline", A, b, dI, n_coeffs=20, spacing="log", A_quad=A_q)
    c_u, c_l = uni.chi2_r_history[-1], lg.chi2_r_history[-1]
    hq_l = high_q_chi2(q, b, lg.fitted_b, dI)
    print(f"[spline 20 coeffs, 4x quad] uniform chi2_r={c_u:.3f}  log chi2_r={c_l:.3f} (high-q {hq_l:.2f})")
    assert c_l < c_u and 0.7 < c_l < 1.2 and hq_l > 0.5, (c_u, c_l, hq_l)

    # Generic spline option (run_solver) for solvers without their own
    # basis: every Tikhonov/Bayesian variant must become balanced at high q.
    opt = SplineOptions(n_coeffs=20, spacing="log", A_quad=A_q)
    for key in ["bayesian_evidence_hansen", "bayesian_evidence_two_segment",
                "bayesian_evidence_hann_tapered", "general_tikhonov_gcv",
                "tikhonov_standard_gcv"]:
        g = run_solver(key, A, b, dI)
        s_ = run_solver(key, A, b, dI, spline=opt)
        hg, hs = high_q_chi2(q, b, g.fitted_b, dI), high_q_chi2(q, b, s_.fitted_b, dI)
        print(f"[generic spline] {key:32s} high-q chi2 grid={hg:.3f} spline={hs:.3f} "
              f"chi2_r={s_.chi2_r_history[-1]:.3f}")
        assert 0.7 < hs < 1.5 and 0.7 < s_.chi2_r_history[-1] < 1.2, (key, hs)
        assert s_.x[0] == 0 and s_.x[-1] == 0
        assert "B-spline basis" in s_.diagnostics["lambda_selection"]
    sb = run_solver("bayesian_evidence_hansen", A, b, dI, spline=opt)
    assert "x_sigma" in sb.diagnostics and sb.diagnostics["x_sigma"].shape == sb.x.shape

    # EM in spline-coefficient space on the (non-negative) sphere kernel:
    # must agree with plain point-grid EM rather than fall back to an
    # over-smoothed L-curve solution (was chi2_r~48 before the fix).
    d = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    r = np.linspace(0.0, 500.0, 150)
    A_s = build_size_distribution_kernel(d.q, r, KERNEL_REGISTRY["sphere_rg"].func, alpha=6.0)
    em_s = run_solver("em_discrepancy_spline", A_s, d.I - 0.1, d.dI)
    em_p = run_solver("em_discrepancy", A_s, d.I - 0.1, d.dI)
    pk_s, pk_p = r[np.argmax(em_s.x)], r[np.argmax(em_p.x)]
    print(f"[EM spline, sphere] chi2_r={em_s.chi2_r_history[-1]:.3f} peak r={pk_s:.0f} "
          f"(point-grid EM peak r={pk_p:.0f})")
    assert em_s.chi2_r_history[-1] < 1.5
    assert abs(pk_s - pk_p) < 20
    r_q2 = np.linspace(0.0, 500.0, 4 * 149 + 1)
    opt_s = SplineOptions(40, "uniform",
                          build_size_distribution_kernel(d.q, r_q2, KERNEL_REGISTRY["sphere_rg"].func, alpha=6.0))
    for key in ["em_discrepancy", "bayesian_evidence_hansen", "general_tikhonov_gcv"]:
        res = run_solver(key, A_s, d.I - 0.1, d.dI, spline=opt_s)
        Dv = res.x[1:] * r[1:] ** -3.0
        pk = r[1:][np.argmax(Dv)]
        print(f"[generic spline, sphere] {key:26s} chi2_r={res.chi2_r_history[-1]:.3f} volume peak r={pk:.0f}")
        assert res.chi2_r_history[-1] < 1.3 and 65 < pk < 95, (key, pk)
    # Duplicates retired 2026-10-10 must equal the generic path exactly.
    o20 = SplineOptions(20, "log", A_q)
    a_ = run_solver("bayesian_evidence_spline", A, b, dI, n_coeffs=20, spacing="log", A_quad=A_q).x
    c_ = run_solver("bayesian_evidence", A, b, dI, spline=o20, penalty="second_derivative").x
    assert np.allclose(a_, c_), np.abs(a_ - c_).max()
    print("OK")


if __name__ == "__main__":
    main()
