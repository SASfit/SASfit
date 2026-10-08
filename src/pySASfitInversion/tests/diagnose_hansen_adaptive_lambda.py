"""
Diagnostic/validation for the new two-segment adaptive-lambda extension
(regularization.hansen_smoothness_two_segment +
solvers/bayesian_evidence.py's log_evidence_blocks/evidence_search_blocks).

Does an evidence-maximizing (lambda_lo, lambda_hi, split_idx) give a more
regionally balanced fit than the single global lambda, on the same real
data, at the now-justified Dmax = pi/q_min? This is the direct follow-up
to the systematic rule-out in diagnose_hansen_lambda_balance.py,
diagnose_hansen_dmax_scan.py and diagnose_hansen_nr_scan.py, all of which
showed the high-q noise-tracking survives unchanged across lambda, Dmax,
n_r and r_min individually -- i.e. a genuine single-global-lambda
limitation, which this adaptive extension is meant to address.

Run from the pySASfitInversion directory:
    python3 tests/diagnose_hansen_adaptive_lambda.py
"""
import sys, os, time
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.background import fit_background
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.regularization import hansen_smoothness_cholesky, hansen_smoothness_two_segment
from sasfit_inversion.solvers import svd_methods, bayesian_evidence
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

    # --- baseline: single global lambda (existing bayesian_evidence_hansen) ---
    t0 = time.time()
    baseline = SOLVER_REGISTRY["bayesian_evidence_hansen"].run(A, b, dI)
    chi2_r_base = baseline.chi2_r_history[-1]
    resid_base = (baseline.fitted_b - b) / dI
    regions_base = regional_breakdown(q, resid_base)
    print(f"BASELINE (single global lambda): chi2_r={chi2_r_base:.4g}  "
          f"[{time.time()-t0:.1f}s]")
    print(f"  {baseline.diagnostics['lambda_selection']}")
    for label, mean_chi2, std_resid in regions_base:
        print(f"  {label:>7}: chi2 contrib={mean_chi2:.4g}, std(resid)={std_resid:.4g}")
    print()

    # lambda scale, same heuristic as the registered solver
    svd_A = svd_methods.compute_svd(A)
    scale = float(np.median(svd_A.s)) ** 2
    lam_grid_1d = np.geomspace(max(scale * 1e-6, 1e-300), scale * 1e6, 15)

    # --- adaptive: search split_idx x (lambda_lo, lambda_hi) jointly ---
    split_candidates = [int(f * n_r) for f in (0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85)]
    split_candidates = sorted(set(s for s in split_candidates if 1 <= s <= n_r - 2))

    best_overall = None
    best_split = None
    t0 = time.time()
    for split_idx in split_candidates:
        A_lo, A_hi, n_lo, n_hi = hansen_smoothness_two_segment(n_r, split_idx)
        best, _ = bayesian_evidence.evidence_search_blocks(
            A, b, dI, (A_lo, A_hi), (n_lo, n_hi), (lam_grid_1d, lam_grid_1d)
        )
        print(f"  split_idx={split_idx:4d} (r={r[split_idx]:7.1f})  "
              f"log_evid={best.log_evidence:10.4g}  "
              f"lam_lo={best.lams[0]:10.4g}  lam_hi={best.lams[1]:10.4g}  Ng={best.n_good_params:.1f}")
        if best_overall is None or best.log_evidence > best_overall.log_evidence:
            best_overall = best
            best_split = split_idx
    print(f"  [{time.time()-t0:.1f}s for {len(split_candidates)} splits x "
          f"{len(lam_grid_1d)}x{len(lam_grid_1d)} grid]")
    print()

    fitted_b_adapt = A @ best_overall.p_map
    resid_adapt = (fitted_b_adapt - b) / dI
    chi2_r_adapt = np.mean(resid_adapt ** 2)
    regions_adapt = regional_breakdown(q, resid_adapt)

    print(f"ADAPTIVE (best split_idx={best_split}, r={r[best_split]:.1f}): "
          f"chi2_r={chi2_r_adapt:.4g}")
    print(f"  lam_lo={best_overall.lams[0]:.4g}  lam_hi={best_overall.lams[1]:.4g}  "
          f"Ng={best_overall.n_good_params:.1f}")
    for label, mean_chi2, std_resid in regions_adapt:
        print(f"  {label:>7}: chi2 contrib={mean_chi2:.4g}, std(resid)={std_resid:.4g}")

    print()
    print("Comparison (want: all three regions closer to 1 than baseline, especially")
    print("high-q rising toward 1 instead of staying pinned near 0.02):")
    print(f"{'region':>7} {'baseline chi2':>14} {'adaptive chi2':>14}")
    for (label, mb, _), (_, ma, _) in zip(regions_base, regions_adapt):
        print(f"{label:>7} {mb:14.4g} {ma:14.4g}")
    print(f"{'overall':>7} {chi2_r_base:14.4g} {chi2_r_adapt:14.4g}")


if __name__ == "__main__":
    main()
