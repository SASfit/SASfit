"""
Non-uniform r grids for the point-grid solvers (2026-10-10): uniform /
quadratic / log spacing, p(r) piecewise linear between the grid points,
kernel integrated on a fine grid (kernels.build_kernel_on_nodes).

Run from the pySASfitInversion directory:
    python3 tests/test_r_grid_spacing.py
"""
import os
import sys

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from sasfit_inversion.background import fit_background
from sasfit_inversion.io_utils import load_sas_data
from sasfit_inversion.kernel_registry import KERNEL_REGISTRY
from sasfit_inversion.kernels import (
    build_kernel_on_nodes, build_size_distribution_kernel, cubic_basis, hat_basis, make_r_grid,
)
from sasfit_inversion.solver_registry import run_solver


def main():
    for sp in ("uniform", "quadratic", "log"):
        g = make_r_grid(782.3, 60, sp)
        assert g[0] == 0 and np.isclose(g[-1], 782.3) and np.all(np.diff(g) > 0), sp
    H = hat_basis(make_r_grid(10.0, 7, "log"), np.linspace(0, 10, 101))
    assert np.allclose(H.sum(axis=1), 1.0)

    # On a fine uniform grid the node kernel reproduces the direct kernel's
    # action on a smooth p(r) (sphere PDDF, R=100).
    d = load_sas_data(os.path.join(os.path.dirname(__file__), "data", "test.dat"))
    q, I, dI = d.q, d.I, d.dI
    f = KERNEL_REGISTRY["sinc_4pi"].func
    R = 100.0
    p = lambda r: np.where(r < 2 * R, r**2 * (1 - 0.75 * r / R + (r / R) ** 3 / 16), 0.0)
    r_ref = np.linspace(0, 2 * R, 4001)
    I_ref = build_size_distribution_kernel(q, r_ref, f) @ p(r_ref)
    rn = make_r_grid(2 * R, 60, "quadratic")
    I_nodes = build_kernel_on_nodes(q, rn, f) @ p(rn)
    rel = np.max(np.abs(I_nodes - I_ref) / np.abs(I_ref).max())
    print(f"[node kernel vs reference integral] max rel. deviation {rel:.2e}")
    assert rel < 2e-3

    # test.dat, j0: non-uniform grids with 60 points fit (direct kernel
    # does not), signed EM and Bayesian evidence both reach chi2_r <= ~1
    b = I - fit_background(q, I, q_min=0.094016, q_max=0.385030, dI=dI).evaluate(q)
    rmax = np.pi / q.min()
    for sp in ("quadratic", "log"):
        rn = make_r_grid(rmax, 60, sp)
        A_direct = build_size_distribution_kernel(q, rn, f)
        A_nodes = build_kernel_on_nodes(q, rn, f)
        c_direct = run_solver("bayesian_evidence", A_direct, b, dI).chi2_r_history[-1]
        be = run_solver("bayesian_evidence", A_nodes, b, dI)
        em = run_solver("em_general_signed_kernel", A_nodes, b, dI)
        print(f"[{sp:9s} 60 pts] Bayesian evidence chi2_r direct={c_direct:.3g} "
              f"fine-integrated={be.chi2_r_history[-1]:.3f}; signed EM {em.chi2_r_history[-1]:.3f}; "
              f"corr {np.corrcoef(be.x, em.x)[0, 1]:.3f}")
        assert c_direct > 5 and be.chi2_r_history[-1] < 1.2 and em.chi2_r_history[-1] < 1.2

    # Cubic (not-a-knot) interpolation between nodes (2026-10-10): exact for
    # cubics, partition of unity, and coarse grids fit where linear fails.
    rn = make_r_grid(10.0, 9, "log")
    rf = np.linspace(0, 10, 201)
    C = cubic_basis(rn, rf)
    assert np.allclose(C.sum(axis=1), 1.0)
    assert np.allclose(C @ (rn**3 - 2 * rn), rf**3 - 2 * rf)
    for sp, n, lin_min, cub_max in [("uniform", 30, 50.0, 1.5), ("log", 30, 1.5, 1.0)]:
        rn = make_r_grid(rmax, n, sp)
        c_lin = run_solver("bayesian_evidence", build_kernel_on_nodes(q, rn, f), b, dI).chi2_r_history[-1]
        c_cub = run_solver("bayesian_evidence", build_kernel_on_nodes(q, rn, f, interpolation="cubic"),
                           b, dI).chi2_r_history[-1]
        print(f"[{sp:9s} {n} pts] Bayesian evidence chi2_r linear={c_lin:.3g} cubic={c_cub:.3f}")
        assert c_lin > lin_min and c_cub < cub_max, (sp, c_lin, c_cub)
    print("OK")


if __name__ == "__main__":
    main()
