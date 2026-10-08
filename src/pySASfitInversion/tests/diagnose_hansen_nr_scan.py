"""
Diagnostic: is the persistent high-q "noise tracking" (chi2 contribution
~0.02 regardless of Dmax or lambda, see diagnose_hansen_lambda_balance.py
and diagnose_hansen_dmax_scan.py) actually an effect of r-grid resolution
(n_r) rather than being purely inherent to the kernel/method?

Rationale: the Shannon-number estimate for this q-range (q_max*Dmax/pi)
is far smaller than n_r=150 -- i.e. the data can only really support on
the order of ~90-100 independent p(r) values at Dmax~780, not 150. With
more r-points than that, Hansen's smoothness regularizer (a fixed-form
local curvature penalty) may not cost enough to suppress fine,
small-amplitude wiggles that are cheap in r-index space but happen to
alias onto exactly the oscillation period the high-q Debye kernel is
sensitive to -- i.e. oversampling r could be *handing* the high-q region
noise-fitting freedom that a coarser, information-matched r-grid would
not have.

If that's right: reducing n_r at FIXED Dmax should reduce the high-q
noise-tracking (push its chi2 contribution up toward 1) without
re-introducing the low-q underfit that Dmax (not n_r) was responsible
for fixing. If high-q chi2 contribution stays ~0.02 regardless of n_r
too, the effect is genuinely inherent to the kernel/evidence method, not
a samplig artifact.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_nr_scan.py
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

    dmax = float(np.pi / q.min())  # the now-justified Dmax, ~782
    print(f"Dmax = pi/q_min = {dmax:.4g} (fixed for this scan)")
    print()

    n_r_values = [20, 30, 45, 61, 75, 91, 110, 150, 200]

    print(f"{'n_r':>5} {'chi2_r':>8} {'lambda':>10} {'Ng':>6} | "
          f"{'low chi2':>9} {'mid chi2':>9} {'high chi2':>9} | "
          f"{'low std':>8} {'mid std':>8} {'high std':>8}")
    for n_r in n_r_values:
        r = np.linspace(1.0, dmax, n_r)
        A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
        result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
        chi2_r = result.chi2_r_history[-1]
        lam = float(result.diagnostics["lambda_selection"].split("lambda=")[1].split(",")[0])
        ng = float(result.diagnostics["lambda_selection"].split("Ng=")[1])
        resid = (result.fitted_b - b) / dI
        regions = regional_breakdown(q, resid)
        print(f"{n_r:5d} {chi2_r:8.3g} {lam:10.4g} {ng:6.1f} | "
              f"{regions[0][1]:9.3g} {regions[1][1]:9.3g} {regions[2][1]:9.3g} | "
              f"{regions[0][2]:8.3g} {regions[1][2]:8.3g} {regions[2][2]:8.3g}")

    print()
    print("Interpretation: if high-q chi2 contribution rises toward ~1 as n_r")
    print("drops (while low/mid-q stay reasonable, not blowing up), the high-q")
    print("overfitting is an r-grid-resolution artifact, fixable by matching n_r")
    print("to the data's actual information content (~Shannon number) rather than")
    print("oversampling r. If high-q chi2 stays ~0.02 across all n_r, the effect")
    print("is inherent to the kernel/global-lambda method, not a sampling issue.")

    # --- r_min scan, at a fixed representative n_r, fixed Dmax ---
    # Two distinct concerns with r_min (separate from the n_r/Dmax question
    # above): (1) Hansen's boundary condition forces p=0 at the grid's
    # first node, which approximates the physical p(0)=0 condition only as
    # well as r_min approximates 0; (2) for the j0(qr) kernel,
    # sin(4*pi*q*r)/(q*r) -> 4*pi as r->0, so small r_min makes the first
    # few columns of A nearly degenerate (a different conditioning issue
    # than the high-q oscillatory one this script otherwise probes).
    print()
    print("r_min scan (n_r=91, Dmax fixed):")
    print(f"{'r_min':>7} {'chi2_r':>8} {'lambda':>10} {'Ng':>6} | "
          f"{'low chi2':>9} {'mid chi2':>9} {'high chi2':>9} | "
          f"{'p(r_min)/peak':>13} {'p(r_min2)/peak':>14}")
    for r_min in [0.1, 0.5, 1.0, 2.0, 5.0, 10.0]:
        r = np.linspace(r_min, dmax, 91)
        A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
        result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
        chi2_r = result.chi2_r_history[-1]
        lam = float(result.diagnostics["lambda_selection"].split("lambda=")[1].split(",")[0])
        ng = float(result.diagnostics["lambda_selection"].split("Ng=")[1])
        resid = (result.fitted_b - b) / dI
        regions = regional_breakdown(q, resid)
        p = result.x
        peak = np.max(np.abs(p))
        rel0 = abs(p[0]) / peak if peak > 0 else float("nan")
        rel1 = abs(p[1]) / peak if peak > 0 else float("nan")
        print(f"{r_min:7.3g} {chi2_r:8.3g} {lam:10.4g} {ng:6.1f} | "
              f"{regions[0][1]:9.3g} {regions[1][1]:9.3g} {regions[2][1]:9.3g} | "
              f"{rel0:13.4g} {rel1:14.4g}")

    print()
    print("Interpretation: chi2_r and the regional breakdown should be roughly")
    print("stable across r_min once it's comfortably smaller than real structure")
    print("in p(r) -- if small r_min (0.1) shows degraded conditioning (chi2_r")
    print("jumps, Ng behaves oddly) relative to r_min~1-5, that's the near-r=0")
    print("kernel-degeneracy effect, separate from the high-q/n_r question above.")


if __name__ == "__main__":
    main()
