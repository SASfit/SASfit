"""
Diagnostic: chi2_r kept dropping as Dmax (r_max) was increased by hand in
the GUI (500 -> 1.593, 800 -> 0.379, 1200 -> 0.303), confirming Dmax=500
was truncating the real pair-distance support (p(Dmax)=0 boundary
condition clamping real density to zero too early, which low-q data is
most sensitive to).

Two things need checking before trusting a large Dmax at face value:

1. Where does chi2_r actually level off? A geometric scan over Dmax
   should show chi2_r falling fast while real structure is still being
   truncated, then flattening out once Dmax comfortably exceeds the true
   particle size -- NOT continuing to fall monotonically forever (if it
   keeps falling with no sign of flattening, more Dmax is still buying a
   better fit, i.e. still truncating).

2. Is a falling chi2_r at larger Dmax "real", or just more free
   parameters interpolating the data? Ng (number of good parameters)
   should stop tracking the data count once Dmax is generously large --
   and p(r) itself should decay to ~0 well *before* r=Dmax, not still be
   sitting at a significant fraction of its peak value at the boundary.
   If p(r) is still large at r=Dmax, that Dmax is still clamping real
   density to zero and is not yet a safe choice.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_dmax_scan.py
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

    # Match the GUI exactly: fitted background over [0.094016, 0.385030]
    # (note BackgroundFitResult.evaluate() zeroes out the c*q^-alpha term,
    # so this is effectively just the flat backgr constant -- verified not
    # to be the source of the earlier discrepancy).
    bg = fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI)
    b = I - bg.evaluate(q)
    print(f"background: backgr={bg.backgr:.6g} (c*q^-alpha term zeroed per evaluate())")
    print()

    dmax_values = [500, 650, 800, 1000, 1200, 1500, 1800, 2200, 2700]
    n_r = 150  # match the GUI's r grid points (was wrongly 50 in the first pass)

    print(f"{'Dmax':>8} {'chi2_r':>8} {'lambda':>10} {'Ng':>6} {'p(r) at 0.9*Dmax':>18} "
          f"{'p(r) at Dmax':>14} {'relative to peak':>18}")
    results_by_dmax = {}
    for dmax in dmax_values:
        r = np.linspace(1.0, float(dmax), n_r)
        A = build_size_distribution_kernel(q, r, KERNEL_REGISTRY["sinc_4pi"].func, alpha=0.0)
        result = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
        p = result.x
        chi2_r = result.chi2_r_history[-1]
        lam = float(result.diagnostics["lambda_selection"].split("lambda=")[1].split(",")[0])
        ng = float(result.diagnostics["lambda_selection"].split("Ng=")[1])
        peak = np.max(np.abs(p))
        idx_90 = int(0.9 * (n_r - 1))
        p_90 = p[idx_90]
        p_end = p[-1]
        rel = abs(p_end) / peak if peak > 0 else float("nan")
        print(f"{dmax:8d} {chi2_r:8.4g} {lam:10.4g} {ng:6.1f} {p_90:18.4g} {p_end:14.4g} {rel:18.4g}")
        results_by_dmax[dmax] = (r, p, result)

    print()
    print("Interpretation: chi2_r should flatten out once Dmax safely exceeds the")
    print("true particle size. 'relative to peak' (|p(Dmax)|/max|p(r)|) should be")
    print("small (e.g. <0.02-0.05) once Dmax is large enough -- if it's still a")
    print("sizeable fraction of the peak, p(r) has not decayed and Dmax is still")
    print("truncating real structure.")

    # Regional residual breakdown at EVERY Dmax tested -- not just the last
    # (possibly numerically broken) one -- to see whether the low-q/high-q
    # imbalance persists through the well-behaved plateau (650-1800) too,
    # or is specific to Dmax=500 / the broken large-Dmax rows.
    print()
    print("Regional residual breakdown vs Dmax:")
    print(f"{'Dmax':>8} {'low chi2':>10} {'mid chi2':>10} {'high chi2':>10} | "
          f"{'low std':>9} {'mid std':>9} {'high std':>9}")
    for dmax in dmax_values:
        r, p, result = results_by_dmax[dmax]
        resid = (result.fitted_b - b) / dI
        regions = regional_breakdown(q, resid)
        print(f"{dmax:8d} {regions[0][1]:10.4g} {regions[1][1]:10.4g} {regions[2][1]:10.4g} | "
              f"{regions[0][2]:9.4g} {regions[1][2]:9.4g} {regions[2][2]:9.4g}")


if __name__ == "__main__":
    main()
