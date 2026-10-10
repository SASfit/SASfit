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

    from sasfit_inversion.solver_registry import run_solver
    result = run_solver("bayesian_evidence", A, b, dI, penalty="hansen_printed")
    # old key still works as an alias
    alias = run_solver("bayesian_evidence_hansen", A, b, dI)
    assert np.allclose(alias.x, result.x)
    # default penalty is Hansen eq. 19 (curvature)
    default = run_solver("bayesian_evidence", A, b, dI)
    assert default.diagnostics["penalty"] == "hansen_eq19"
    print(f"default (eq. 19): chi2_r={default.chi2_r_history[-1]:.4g}, "
          f"ln Z={default.diagnostics['log_evidence_normalized']:.1f} vs printed "
          f"{result.diagnostics['log_evidence_normalized']:.1f}")
    assert default.diagnostics["log_evidence_normalized"] > result.diagnostics["log_evidence_normalized"]
    chi2 = result.chi2_r_history[-1]
    print(f"bayesian_evidence_hansen: chi2_r={chi2:.4g}")
    print(f"  {result.diagnostics.get('lambda_selection')}")
    print(f"  p(r) range: [{result.x.min():.4g}, {result.x.max():.4g}]")

    assert chi2 < 5.0, f"expected chi2_r well under the ~586/34600 seen with the other regularized solvers, got {chi2}"
    print("OK")


if __name__ == "__main__":
    main()
