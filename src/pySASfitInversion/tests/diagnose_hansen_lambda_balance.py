"""
Diagnostic: does the Bayesian-evidence-maximizing lambda for the Hansen
boundary-constrained solver give the most REGIONALLY BALANCED fit across
q, or would a different (non-maximal-evidence) lambda on the same grid
trade a bit of evidence for less high-q overfitting / low-q underfitting?

Background: tests/diagnose_hansen_residuals.py showed that at the
evidence-maximizing lambda, chi2_r~1.56 overall hides severe regional
imbalance: low q mean chi2 contribution ~3.2 (underfit), high q ~0.02
(the fit is tracking noise ~7x more closely than it statistically
should there). This script reuses the SAME evidence_search(...) call
(so the lambda grid and MAP solutions are identical to what the
registered 'bayesian_evidence_hansen' solver computes) and reports the
regional breakdown at EVERY trial lambda, not just the evidence argmax,
so we can see whether moving along the lambda axis can fix the regional
imbalance or whether it is an inherent single-global-lambda limitation.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_lambda_balance.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.regularization import hansen_smoothness_cholesky
from sasfit_inversion.solvers import svd_methods
from sasfit_inversion.solvers import bayesian_evidence


def regional_breakdown(q, resid):
    edges = np.quantile(q, [0, 1 / 3, 2 / 3, 1.0])
    labels = ["low q", "mid q", "high q"]
    out = []
    for i, label in enumerate(labels):
        mask = (q >= edges[i]) & (q <= edges[i + 1] if i == 2 else q < edges[i + 1])
        out.append((label, np.mean(resid[mask] ** 2), np.std(resid[mask])))
    return out


def main():
    data = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = data.q, data.I, data.dI
    b = I - 0.1

    r = np.linspace(1.0, 500.0, 50)  # match the GUI screenshot's 50 r-points
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
    n = A.shape[1]
    L = hansen_smoothness_cholesky(n)

    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 60)

    best, results = bayesian_evidence.evidence_search(A, b, dI, L, lam_grid)

    print(f"evidence-maximizing lambda = {best.lam:.6g}  (Ng={best.n_good_params:.1f})")
    print()
    header = (f"{'lambda':>12} {'log_evid':>12} {'Ng':>7} {'chi2_r':>8} | "
              f"{'low chi2':>9} {'mid chi2':>9} {'high chi2':>9} | "
              f"{'low std':>8} {'mid std':>8} {'high std':>8}  marker")
    print(header)
    print("-" * len(header))

    best_idx = max(range(len(results)), key=lambda i: results[i].log_evidence)
    for i, res in enumerate(results):
        fitted_b = A @ res.p_map
        resid = (fitted_b - b) / dI
        chi2_r = np.mean(resid ** 2)
        regions = regional_breakdown(q, resid)
        marker = "  <== evidence max" if i == best_idx else ""
        print(f"{res.lam:12.4g} {res.log_evidence:12.4g} {res.n_good_params:7.1f} {chi2_r:8.3g} | "
              f"{regions[0][1]:9.3g} {regions[1][1]:9.3g} {regions[2][1]:9.3g} | "
              f"{regions[0][2]:8.3g} {regions[1][2]:8.3g} {regions[2][2]:8.3g}{marker}")

    print()
    print("Interpretation: look for a lambda where low/mid/high chi2 contributions")
    print("are all reasonably close to 1 simultaneously (a 'balanced' row), and")
    print("compare its log_evid to the evidence maximum's. If no row achieves that")
    print("-- if high-q chi2 only approaches 1 once low/mid q are badly underfit")
    print("(chi2 >> 1), or vice versa -- that confirms the imbalance is inherent to")
    print("a single global lambda on this kernel/noise structure, not a suboptimal")
    print("choice of lambda within Hansen's evidence framework.")


if __name__ == "__main__":
    main()
