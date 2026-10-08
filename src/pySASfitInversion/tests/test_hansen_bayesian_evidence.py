"""
Smoke test for the new 'bayesian_evidence_hansen' solver-registry entry
(Hansen (2000)'s boundary-constrained smoothness operator + the existing
Bayesian-evidence lambda search), on the real j0(qr)/PDDF-kernel inversion
of test.dat that motivated adding it (see
tests/diagnose_hansen_evidence.py for the original diagnostic).
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

    r = np.linspace(1.0, 500.0, 150)
    A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)

    assert "bayesian_evidence_hansen" in SOLVER_REGISTRY, "new solver not registered"
    result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
    chi2 = result.chi2_r_history[-1]
    print(f"bayesian_evidence_hansen: chi2_r={chi2:.4g}")
    print(f"  {result.diagnostics.get('lambda_selection')}")
    print(f"  p(r) range: [{result.x.min():.4g}, {result.x.max():.4g}]")

    assert chi2 < 5.0, f"expected chi2_r well under the ~586/34600 seen with the other regularized solvers, got {chi2}"
    print("OK")


if __name__ == "__main__":
    main()
