"""
Diagnostic: is the 'bayesian_evidence_hansen' solver's fit actually
overfitting noise at high q (as it visually appears to in the GUI plot),
or is chi2_r~1.56 spread roughly evenly across q, with the high-q "wiggle"
just being what an appropriately-sized fit normally looks like there?

Checks the normalized residual (fit-data)/db per point, binned by q
region, and reports whether high-q residuals are anomalously SMALL
(genuine overfitting/noise-tracking) vs close to +-1 (expected size for a
correctly regularized fit) vs anomalously LARGE (underfitting there).

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_residuals.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY


def main():
    data = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = data.q, data.I, data.dI
    b = I - 0.1

    r = np.linspace(1.0, 500.0, 50)  # match the GUI screenshot's 50 r-points
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)

    result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
    fitted_b = result.fitted_b
    resid = (fitted_b - b) / dI

    print(f"overall chi2_r = {np.mean(resid**2):.4g}  (n={len(q)})")
    print()

    # Split into q terciles (roughly low / mid / high q) and report chi2
    # contribution and residual stats per region.
    edges = np.quantile(q, [0, 1/3, 2/3, 1.0])
    labels = ["low q", "mid q", "high q"]
    print(f"{'region':>8} {'q range':>22} {'n_pts':>6} {'mean chi2 contrib':>18} {'mean |resid|':>13} {'std(resid)':>11}")
    for i, label in enumerate(labels):
        mask = (q >= edges[i]) & (q <= edges[i + 1] if i == 2 else q < edges[i + 1])
        n_pts = mask.sum()
        mean_chi2 = np.mean(resid[mask] ** 2)
        mean_abs_resid = np.mean(np.abs(resid[mask]))
        std_resid = np.std(resid[mask])
        print(f"{label:>8} [{q[mask].min():.4g}, {q[mask].max():.4g}]".ljust(31) +
              f"{n_pts:6d} {mean_chi2:18.4g} {mean_abs_resid:13.4g} {std_resid:11.4g}")

    print()
    print("Interpretation: mean chi2 contribution and std(resid) should each be")
    print("close to 1 in every region for a correctly-sized fit. Well below 1")
    print("(e.g. <0.3) means the fit is tracking noise there (too little")
    print("regularization locally); well above 1 means underfitting there.")

    # Also print the last 15 points' individual residuals explicitly, since
    # that's the region the GUI screenshot shows "following the noise".
    print()
    print("Last 15 data points (highest q) individually:")
    print(f"{'q':>10} {'I-backgr':>12} {'fit':>12} {'dI':>10} {'resid/dI':>10}")
    for i in range(len(q) - 15, len(q)):
        print(f"{q[i]:10.4g} {b[i]:12.4g} {fitted_b[i]:12.4g} {dI[i]:10.4g} {resid[i]:10.4g}")


if __name__ == "__main__":
    main()
