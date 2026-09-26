import numpy as np
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import build_size_distribution_kernel
from sasfit_inversion.solvers import em, lambda_search

DATA_PATH = os.path.join(os.path.dirname(__file__), "data", "test.dat")

"""
This is the JAC 2022 paper's own benchmark dataset (Jemian 2013, data/test.sas
-> test.dat), a simulated bilognormal sphere distribution (modes 60nm/180nm,
relative widths 0.25/0.30) + Gaussian noise, with published reconstructions
in Figure 5 (main sharp peak ~75-85nm, smaller broad secondary hump
~180-220nm in volume-weighted D_V(R)).

Getting this test to actually match Figure 5 required finding and fixing a
real bug (2026-09-25): sphere_rayleigh_gans_intensity was missing the r^6
(Volume^2) prefactor -- returning only the normalized shape P(qr)=1 at
q=0. That's not a cosmetic omission: it changes the RELATIVE weighting
between particle sizes in a polydisperse sum, since real particles scatter
with intensity proportional to Volume^2. Without it, a blind inversion
recovered a spurious single peak dominated by ~300nm content with
negligible signal near the true 60nm mode. See kernel_registry.py's
docstring for the full story.

With the r^6 term restored, alpha=6 in build_size_distribution_kernel
becomes essential for numerical conditioning (not optional): it divides
the r^6 back out of the kernel (r^-6 * r^6 * P(qr)^2 = P(qr)^2), avoiding
a ~15-orders-of-magnitude dynamic range in the raw kernel matrix that
otherwise breaks the EM update numerically. Solve for x=N(r)*r^6, recover
N(r) or D_V(R)=N(r)*r^3=x*r^-3 afterwards.

Background: a flat 0.1 (Joachim-supplied, not the 3-parameter power-law
fit) -- fitting backgr+c*q^-alpha to this simulated benchmark's noisy flat
tail is poorly determined (see background.py's degeneracy note) and,
worse, lets the solver spuriously absorb the background into a fake
small-particle population even while chi2_r looks fine. Subtracting the
known flat value avoids that.
"""


def main():
    data = load_sas_data(DATA_PATH)

    I_sub = data.I - 0.1  # known flat background (Joachim, 2026-09-25)
    sphere_kernel = KERNEL_REGISTRY["sphere_rg"].func

    r_grid = np.linspace(1, 500, 150)
    A = build_size_distribution_kernel(data.q, r_grid, sphere_kernel, alpha=6.0)

    def solve_fn(h):
        return em.solve(A, I_sub, data.dI, max_iterations=5000, smoothing_h=h)

    search = lambda_search.discrepancy_principle_search(
        solve_fn, target_chi2_r=1.0, param_start=0.3, param_floor=1e-9
    )
    result = search.result
    print(f"smoothing h={search.param:.4g}, chi2_r={result.chi2_r_history[-1]:.4f}")

    x = result.x  # x = N(r) * r^6
    Dv = x * r_grid ** (-3)  # D_V(R) = N(r)*r^3 = x*r^-3
    Dv_norm = Dv / Dv.max()

    main_peak_idx = int(np.argmax(Dv_norm))
    main_peak_r = r_grid[main_peak_idx]
    print(f"main peak at r={main_peak_r:.1f}nm (paper's Fig. 5: ~75-85nm)")

    # secondary hump: look for the local max in the r>120nm region
    tail_mask = r_grid > 120
    secondary_idx = int(np.argmax(Dv_norm[tail_mask]))
    secondary_r = r_grid[tail_mask][secondary_idx]
    print(f"secondary hump near r={secondary_r:.1f}nm (paper's Fig. 5: ~180-220nm)")

    assert abs(result.chi2_r_history[-1] - 1.0) < 0.1, "discrepancy principle should land at chi2_r=1 on this benchmark"
    assert 60 < main_peak_r < 100, "main peak should land near the paper's published ~75-85nm"
    assert 140 < secondary_r < 260, "secondary hump should land near the paper's published ~180-220nm"
    print("OK -- reproduces the qualitative shape of the paper's own Figure 5")


if __name__ == "__main__":
    main()
