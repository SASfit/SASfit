"""
Real-data validation of the new 'bayesian_evidence_hann_tapered' solver
(solver_registry.py) against the existing 'bayesian_evidence_hansen' hard-
boundary solver, on the same real test.dat dataset and Dmax used
throughout this investigation.

This follows up a synthetic-sphere test (known ground truth, 5 noise
seeds) that found the Hann-tapered boundary consistently gave lower RMSE
and a high-q chi2 contribution closer to 1 than Hansen's hard p(Dmax)=0
boundary -- but that synthetic test didn't reproduce the SEVERITY of the
real data's high-q pathology (chi2 contribution ~0.02 there, vs ~0.5-1.2
in the synthetic test), so this script checks whether the improvement
holds up, and by how much, on the actual problem.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_tapered.py
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.background import fit_background
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solver_registry import SOLVER_REGISTRY


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

    bg = fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI)
    b = I - bg.evaluate(q)

    dmax = float(np.pi / q.min())
    n_r = 150
    r = np.linspace(1.0, dmax, n_r)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)

    print(f"Dmax={dmax:.4g}, n_r={n_r}")
    print()

    for key in ["bayesian_evidence_hansen", "bayesian_evidence_hann_tapered"]:
        result = SOLVER_REGISTRY[key].run(A, b, dI)
        chi2_r = result.chi2_r_history[-1]
        resid = (result.fitted_b - b) / dI
        regions = regional_breakdown(q, resid)
        p = result.x
        print(f"{key}: chi2_r={chi2_r:.4g}")
        print(f"  {result.diagnostics['lambda_selection']}")
        for label, mc, sd in regions:
            print(f"   {label:>7}: chi2 contrib={mc:.4g}  std(resid)={sd:.4g}")
        print(f"  p(r) range: [{p.min():.4g}, {p.max():.4g}]  (any NaN: {np.any(np.isnan(p))})")
        print()


if __name__ == "__main__":
    main()
