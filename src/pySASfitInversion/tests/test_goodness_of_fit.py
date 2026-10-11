"""
Error models for data without dI, goodness-of-fit statistics, and the
evidence-based estimate of an overall error scale (2026-10-10).

Run from the pySASfitInversion directory:
    python3 tests/test_goodness_of_fit.py
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.background import fit_background
from sasfit_inversion.goodness_of_fit import estimate_errors, fit_statistics
from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import run_solver

HERE = os.path.dirname(__file__)


def main():
    d = load_sas_data(os.path.join(HERE, "data", "test.dat"))
    q, I, dI = d.q, d.I, d.dI

    # scatter estimate vs the file's dI
    s = estimate_errors(q, I, "scatter")
    ratio = s / dI
    print(f"[scatter estimate] median ratio to file dI {np.median(ratio):.2f}, "
          f"range {ratio.min():.2f}-{ratio.max():.2f}")
    assert 0.8 < np.median(ratio) < 1.5 and ratio.min() > 0.3 and ratio.max() < 3.5
    # Joachim's local cubic fits through 5 points (default for missing dI)
    sp = estimate_errors(q, I, "poly")
    rp = sp / dI
    print(f"[5-point cubic estimate] median ratio to file dI {np.median(rp):.2f}, "
          f"5-95 % {np.percentile(rp, 5):.2f}-{np.percentile(rp, 95):.2f}")
    assert 0.8 < np.median(rp) < 1.5
    # synthetic: known 3 % noise on a smooth curve; per-point estimate
    # (window 1) is unbiased in sigma^2, the combined one close in median
    from sasfit_inversion.goodness_of_fit import local_polynomial_scatter
    rng = np.random.default_rng(0)
    qs = np.geomspace(0.004, 0.4, 200)
    I0 = 1e3 * np.exp(-(qs * 20) ** 2 / 3) + 1.0
    r1, r15 = [], []
    for _ in range(30):
        Is = I0 * (1 + 0.03 * rng.normal(size=qs.size))
        r1.append(local_polynomial_scatter(qs, Is, window=1) / 0.03)
        r15.append(local_polynomial_scatter(qs, Is, window=15) / 0.03)
    m1, m15 = np.mean(np.array(r1) ** 2), np.median(np.array(r15))
    print(f"[5-point cubic, synthetic 3 %] <s^2>/sigma^2 = {m1:.3f} (window 1), "
          f"median s/sigma = {m15:.3f} (window 15)")
    assert abs(m1 - 1) < 0.1 and abs(m15 - 1) < 0.1
    assert np.allclose(estimate_errors(q, I, "relative", 0.05), 0.05 * I)
    assert np.allclose(estimate_errors(q, I, "poisson") ** 2, I)

    # 2-column file loads without dI
    d2 = load_sas_data(os.path.join(HERE, "data", "test_2col.dat"))
    assert d2.dI is None and len(d2.q) == len(q)

    # statistics: chi2 needs sigma, Pearson/deviance scale with the unit, KL does not
    F = I * (1 + 0.01 * np.sin(30 * q))
    st1, st10 = fit_statistics(I, F, dI), fit_statistics(10 * I, 10 * F)
    assert np.isclose(st10["pearson_r"], 10 * st1["pearson_r"])
    assert np.isclose(st10["deviance_r"], 10 * st1["deviance_r"], rtol=1e-6)
    assert np.isclose(st10["kl"], st1["kl"])
    assert "chi2_r" in st1 and "chi2_r" not in st10
    print(f"[statistics] chi2_r={st1['chi2_r']:.3g} Pearson/M={st1['pearson_r']:.3g} "
          f"deviance/M={st1['deviance_r']:.3g} KL={st1['kl']:.3g}")

    # evidence estimate of an overall error scale recovers a wrong level
    b = I - fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI).evaluate(q)
    dmax = np.pi / q.min()
    A = build_size_distribution_kernel(q, np.linspace(0, dmax, 95), KERNEL_REGISTRY["sinc_4pi"].func)
    ref = run_solver("bayesian_evidence", A, b, dI)
    for factor in (3.0, 1 / 3):
        res = run_solver("bayesian_evidence", A, b, factor * dI, noise_scale="estimate")
        s_est = res.diagnostics["noise_scale"]
        corr = np.corrcoef(res.x, ref.x)[0, 1]
        print(f"[noise scale] dI x {factor:.3g}: s={s_est:.3g} (product {s_est * factor:.3g}), "
              f"corr with reference {corr:.4f}")
        assert 0.8 < s_est * factor < 1.25 and corr > 0.999
    # without dI: scatter errors give the same p(r)
    res = run_solver("bayesian_evidence", A, b, s, noise_scale="estimate")
    corr = np.corrcoef(res.x, ref.x)[0, 1]
    print(f"[no dI] scatter errors + estimated scale: corr {corr:.4f}")
    assert corr > 0.995
    print("OK")


if __name__ == "__main__":
    main()
