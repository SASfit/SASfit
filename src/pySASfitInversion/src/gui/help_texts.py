"""
Help texts for the GUI's info panel (2026-10-10).

All explanatory text of the GUI lives here instead of in tooltips and
inline hint labels. ``gui.app.InfoPanel`` shows exactly one of these
entries -- the one belonging to the control under the mouse (or the item
highlighted in an open drop-down list) -- and sizes itself to the longest
entry, so the panel never needs to scroll or resize while hovering.

Keys:
  plain names           one entry per control ("r_max", "spline", ...)
  "kernel:<key>"        one entry per kernel of KERNEL_REGISTRY
  "solver:<key>"        one entry per solver of SOLVER_REGISTRY
  "option:<name>"       one entry per solver option (SolverSpec.options)

"{dynamic}" in a text is replaced by the GUI with current information
(suggested grid values, the loaded data, background fit and solve
results); the panel is resized to fit those texts too.

Texts are Qt rich text (a small HTML subset). Keep the first line a bold
title; the panel layout relies on nothing else.
"""
from __future__ import annotations

NO_DATA = "<br>No data loaded yet. Use <i>Load data file...</i>."
NO_BACKGROUND = (
    "<br>Not fitted yet: no background is subtracted. Fit "
    "backgr + c·q<sup>−α</sup> or enter a fixed value."
)
NO_RESULT = "<br>Not solved yet. Choose kernel, grid and solver, then press <i>Solve</i>."

DEFAULT = (
    "<b>Info</b><br>Move the mouse over a control to see what it does. "
    "In an open drop-down list, the highlighted entry is explained."
)

HELP: dict[str, str] = {
    # ---------------------------------------------------------------- data
    "load": (
        "<b>Load data file</b><br>ASCII file with columns q, I(q) and optionally "
        "ΔI(q); without ΔI the errors come from an error model (see 'Error bars'). "
        "Header and comment lines are skipped. After loading, r<sub>max</sub> "
        "and the number of r grid points are suggested from the q range "
        "(see those fields)."
    ),
    "data_info": "<b>Data</b>{dynamic}",
    "error_model": (
        "<b>Error bars</b><br>Uncertainties σ used for the weights, χ²<sub>r</sub> "
        "and the discrepancy principle. <i>from file</i>: the ΔI column. "
        "<i>local cubic fits</i> (default without ΔI): a cubic in ln I vs ln q "
        "through each point and its two neighbours on each side; "
        "σ² = RSS/(5 − 4) of that fit (1 degree of freedom, so the plain mean "
        "square deviation would be 5× too small). <i>neighbour interpolation</i>: "
        "each point vs the line through its two neighbours (more biased by "
        "curvature). Both are combined over a window of points (robust median). "
        "<i>Poisson</i>: σ² = I, only if I is in counts. <i>relative error</i>: "
        "σ = r·I. If only the overall level of σ is uncertain, the "
        "Bayesian-evidence solver can estimate it (option 'Error bars')."
    ),
    "error_window": (
        "<b>Combined over N points</b><br>The single-point estimates of the "
        "local fits have only one degree of freedom and scatter strongly "
        "(5 %–95 % range 0.06σ–2σ); weights 1/σ² from them can be wrong by "
        "orders of magnitude. They are combined by a running median over N "
        "points (scaled to be unbiased; ×1.4826 for one degree of freedom). "
        "N = 1 gives the plain per-point estimate. Default 15."
    ),
    "error_rel": (
        "<b>Relative error</b><br>σ<sub>i</sub> = r·|I<sub>i</sub>| for the "
        "'relative error' model, in percent."
    ),
    # ---------------------------------------------------------- background
    "bg_qmin": (
        "<b>Background fit: q min</b><br>Lower end of the q range used to fit "
        "I(q) ≈ backgr + c·q<sup>−α</sup>. Only the constant backgr is "
        "subtracted before the inversion; the power law only helps to "
        "separate it from the particle scattering."
    ),
    "bg_qmax": (
        "<b>Background fit: q max</b><br>Upper end of the background fit "
        "range. Usually the high-q end of the data, where the particle "
        "scattering has decayed."
    ),
    "fit_bg": (
        "<b>Fit background</b><br>Weighted least-squares fit of backgr + "
        "c·q<sup>−α</sup> in the chosen q range. The result line shows the "
        "parameters with errors and warns if the fit is extrapolated far "
        "outside its range or if backgr and c are nearly degenerate."
    ),
    "bg_result": "<b>Background</b>{dynamic}",
    "use_fixed_bg": (
        "<b>Fixed background</b><br>Subtract a constant entered by hand "
        "instead of the fitted backgr."
    ),
    "fixed_bg": (
        "<b>Fixed background value</b><br>Constant subtracted from I(q) when "
        "'Use fixed value instead' is checked."
    ),
    # -------------------------------------------------------------- kernel
    "kernel": (
        "<b>Kernel</b><br>Forward model I(q) = ∫ K(q,r) x(r) dr. The j₀ "
        "kernel gives the pair-distance distribution p(r) (signed kernel); "
        "the sphere kernel gives a size distribution (non-negative kernel). "
        "Changing the kernel re-suggests r<sub>max</sub> and the grid size."
    ),
    "kernel:sinc_4pi": (
        "<b>Kernel 4π j₀(qr)</b><br>I(q) = 4π ∫<sub>0</sub><sup>D<sub>max</sub></sup> "
        "p(r) sin(qr)/(qr) dr. Unknown: pair-distance distribution p(r), "
        "r = pair distance, p(0) = p(D<sub>max</sub>) = 0. The kernel changes "
        "sign for qr &gt; π, so only the signed-kernel solvers apply: "
        "Bayesian evidence, Tikhonov/TSVD, ARLS, EM for signed kernels. "
        "No size weighting α."
    ),
    "kernel:sphere_rg": (
        "<b>Kernel: sphere (Rayleigh-Gans)</b><br>I(q) = ∫ N(R) V(R)² "
        "P(qR) dR with P the sphere form factor; r is the RADIUS. The "
        "solver works on x(R) = N(R)·R<sup>α</sup>; α = 6 divides the "
        "R<sup>6</sup> of V² out of the kernel (conditioning). Non-negative "
        "kernel: all solvers apply."
    ),
    "alpha": (
        "<b>Size-weighting exponent α</b><br>The unknown is x(r) = "
        "N(r)·r<sup>α</sup> and the kernel is divided by r<sup>α</sup>. "
        "α = 6 (default for spheres) gives a volume-squared weighted "
        "distribution and a well-conditioned kernel; α = 0 the number "
        "distribution with ~10<sup>15</sup> dynamic range in the kernel."
    ),
    # ---------------------------------------------------------------- grid
    "r_max": (
        "<b>r max</b><br>Upper end of the r grid (the grid always starts at "
        "r = 0). Suggested from Glatter's consistency condition "
        "q<sub>min</sub> ≤ π/D<sub>max</sub>, i.e. D<sub>max</sub> ≈ "
        "π/q<sub>min</sub>; for the sphere kernel r is a radius, so "
        "r<sub>max</sub> = D<sub>max</sub>/2. Override freely.{dynamic}"
    ),
    "r_n": (
        "<b>r grid points</b><br>Number of unknowns on the point grid (or "
        "the display resolution in B-spline mode). Suggested as the Shannon "
        "number N<sub>s</sub> = (q<sub>max</sub> − q<sub>min</sub>)·"
        "r<sub>max</sub>/π, the number of independent parameters the data "
        "can determine. With the j₀ kernel a uniform grid needs a step "
        "≤ π/q<sub>max</sub> unless the kernel is integrated between the "
        "points (done automatically, see 'p(r) between grid points')."
        "{dynamic}"
    ),
    "r_spacing": (
        "<b>r grid spacing</b><br>uniform: r<sub>i</sub> = r<sub>max</sub>·u; "
        "quadratic: r<sub>max</sub>·u²; logarithmic: 0 plus a geometric "
        "series from r<sub>max</sub>/(2n) to r<sub>max</sub> (u equidistant "
        "in [0,1]). For non-uniform grids, and for uniform grids coarser "
        "than 1.1·π/q<sub>max</sub>, p(r) is interpolated between the "
        "points and the kernel is integrated on a fine grid (≥ 4 points per "
        "π/q<sub>max</sub>). Smoothness penalties act between neighbouring "
        "points: on a log grid they mean smoothness in log r."
    ),
    "r_interp": (
        "<b>p(r) between grid points</b><br>linear: hat functions; cubic: "
        "C² interpolating spline with not-a-knot ends. Cubic allows much "
        "coarser grids (test.dat, j₀: uniform 30 points χ²<sub>r</sub> 282 "
        "→ 1.2). The cubic spline can overshoot between points, so the "
        "solvers that need p ≥ 0 or a non-negative kernel (EM + smoothing, "
        "EM + MaxEnt, Hansen MaxEnt, ARLS ≥ 0) always use linear; for those "
        "use the B-spline option for a smooth representation."
    ),
    # -------------------------------------------------------------- solver
    "solver": (
        "<b>Solver</b><br>Inversion method and its automatic choice of the "
        "regularization strength. Open the list and move over an entry to "
        "see its description."
    ),
    "solver:em_discrepancy": (
        "<b>EM + smoothing</b><br>Expectation-maximization (Richardson-Lucy) "
        "for x ≥ 0, each step followed by a 3-point smoothing with weight h; "
        "Biggs-Andrews accelerated. h by the discrepancy principle "
        "(χ²<sub>r</sub> = 1; falls back to the L-curve, or to h = 0 in "
        "B-spline mode, if unreachable) or by the L-curve corner. Needs a "
        "non-negative kernel (sphere)."
    ),
    "solver:maxent_em": (
        "<b>EM + MaxEnt (SASfit)</b><br>EM step plus the entropy correction "
        "of SASfit's DR_EM_ME (Kohlbrecher &amp; Bressler 2022, eq. 47–55) "
        "with a constant or an adaptive (Gaussian-smoothed) prior; λ by a "
        "grid search targeting χ²<sub>r</sub> = 1. Needs a non-negative "
        "kernel."
    ),
    "solver:hansen_maxent": (
        "<b>MaxEnt, Hansen functional</b><br>Minimizes ‖(Ax − b)/ΔI‖² + "
        "λ² Σ x<sub>j</sub> ln(x<sub>j</sub>/m) with a flat default model "
        "m, x ≥ 0 (bound-constrained L-BFGS-B plus Newton polish); λ by "
        "the discrepancy principle. Positive solutions only."
    ),
    "solver:bayesian_evidence": (
        "<b>IFT / Tikhonov, Bayesian evidence</b><br>Minimizes χ² + λ‖Lx‖² "
        "with λ maximizing the Bayesian evidence (Vestergaard &amp; Hansen "
        "2006), optionally forced to χ²<sub>r</sub> = 1. Penalty L: Hansen "
        "(2000) eq. 19 (curvature with p(0) = p(D<sub>max</sub>) = 0, "
        "default), Hansen's printed matrix (1st difference), or a free 2nd "
        "derivative. Reports the normalized ln Z for comparing penalties and "
        "an analytic (Laplace) error band. Recommended for j₀/PDDF."
    ),
    "solver:general_tikhonov_gcv": (
        "<b>General-form Tikhonov, GCV</b><br>Minimizes χ² + λ‖Lx‖² with a "
        "2nd-difference L, λ by generalized cross-validation. No positivity "
        "constraint; works with signed kernels."
    ),
    "solver:tikhonov_standard_gcv": (
        "<b>Standard Tikhonov, GCV</b><br>Minimizes χ² + λ‖x‖² (no "
        "smoothness penalty), λ by generalized cross-validation via the "
        "SVD of the weighted kernel."
    ),
    "solver:tsvd_gcv": (
        "<b>Truncated SVD, GCV</b><br>Keeps the k largest singular values of "
        "the weighted kernel; k by generalized cross-validation."
    ),
    "solver:arlsnn": (
        "<b>ARLS, x ≥ 0</b><br>Automatically regularized least squares "
        "(Jones 2023) with non-negativity, no parameter to choose; a quick "
        "sanity check."
    ),
    "solver:em_general_signed_kernel": (
        "<b>EM for signed kernels</b><br>Chae, Martin &amp; Walker (2019): "
        "K = K⁺ − K⁻ and a shift p + t make the problem non-negative; rows "
        "weighted so that EM minimizes χ². Regularization: smoothing (as EM "
        "+ smoothing) or MaxEnt with the positive/negative entropy of a "
        "signed p, adaptive or constant prior; strength by the discrepancy "
        "principle. For the j₀ kernel; for size distributions only if "
        "negative values are acceptable."
    ),
    # ------------------------------------------------------ solver options
    "option:selection": (
        "<b>Smoothing choice</b><br>discrepancy principle: largest h with "
        "χ²<sub>r</sub> = 1. L-curve corner: point of maximum curvature of "
        "(log ‖residual‖, log roughness)."
    ),
    "option:prior": (
        "<b>MaxEnt prior</b><br>adaptive: the default model is the current "
        "solution smoothed with a Gaussian (σ² = 1 grid step²), i.e. MaxEnt "
        "acts as a smoothness prior. constant: flat default model."
    ),
    "option:penalty": (
        "<b>Smoothness penalty</b><br>Hansen eq. 19 (default): "
        "Σ<sub>j=2</sub><sup>N−1</sup> [p<sub>j</sub> − (p<sub>j−1</sub> + "
        "p<sub>j+1</sub>)/2]² + p<sub>1</sub>²/2 + p<sub>N</sub>²/2 (curvature, "
        "end values pulled to 0). Printed matrix: tridiagonal 1, −½ of the "
        "paper's text, i.e. ½Σ(p<sub>j</sub> − p<sub>j+1</sub>)² + "
        "p<sub>1</sub>²/2 + p<sub>N</sub>²/2 (1st difference). 2nd derivative: "
        "Σ(p<sub>j−1</sub> − 2p<sub>j</sub> + p<sub>j+1</sub>)², free ends "
        "(improper prior, no ln Z)."
    ),
    "option:noise_scale": (
        "<b>Error bars (Bayesian evidence)</b><br>as given: σ as supplied by "
        "the file or the error model. estimated: σ<sub>i</sub> → s·σ<sub>i</sub> "
        "with one overall factor s chosen by maximizing the evidence jointly "
        "with λ (likelihood normalization −M ln s). Use it when the relative "
        "errors are right but their level is not, e.g. estimated or guessed "
        "errors. χ²<sub>r</sub> is still reported for the unscaled σ; s is "
        "shown in the result."
    ),
    "option:regularization": (
        "<b>Regularization (signed EM)</b><br>smoothing: 3-point smoothing "
        "each step. MaxEnt adaptive: entropy pulls p towards its smoothed "
        "self. MaxEnt constant: pulls p towards 0; converges slowly on point "
        "grids, use with B-splines."
    ),
    "target_chi2": (
        "<b>Force χ²<sub>r</sub> = 1</b><br>By default λ maximizes the "
        "Bayesian evidence, which need not give χ²<sub>r</sub> = 1. Checked: "
        "λ is raised (or lowered by at most 10³) until χ²<sub>r</sub> = 1; "
        "if unreachable, the closest fit is returned with a note."
    ),
    # ------------------------------------------------------------- splines
    "spline": (
        "<b>Represent p(r) by B-splines</b><br>p(r) = Σ c<sub>k</sub> "
        "B<sub>k</sub>(r) with cubic B-splines, B(0) = B(r<sub>max</sub>) = 0 "
        "built in (Glatter 1977). Works with every solver, which then solves "
        "for the coefficients c on the kernel integrated over the basis. "
        "Fewer free parameters: stops the fit from following the noise at "
        "high q for log-spaced data. B-splines are ≥ 0, so positivity is "
        "preserved for the EM solvers."
    ),
    "spline_n": (
        "<b>B-spline coefficients</b><br>Number of basis functions, i.e. of "
        "free parameters. Fewer = smoother; typical 15–30. The r grid "
        "points only set the resolution on which p(r) is shown."
    ),
    "spline_spacing": (
        "<b>B-spline knot spacing</b><br>uniform, quadratic or logarithmic "
        "in r (same rules as the r grid spacing). Log/quadratic suits a "
        "PDDF that rises steeply at small r (test.dat: χ²<sub>r</sub> ≈ 1 "
        "with 20 coefficients); for size distributions with structure at "
        "large r use uniform."
    ),
    # --------------------------------------------------------- uncertainty
    "mc": (
        "<b>Monte Carlo uncertainty</b><br>Parametric bootstrap: N data sets "
        "fit + Gaussian noise of width ΔI are inverted with the same "
        "settings; the plot shows mean ± standard deviation. Works for every "
        "solver, costs N solves. The Bayesian-evidence solver also shows an "
        "analytic (Laplace) band without this option."
    ),
    "n_boot": (
        "<b>Bootstrap N</b><br>Number of bootstrap replicates; the error of "
        "the standard deviation estimate is about 1/√(2N)."
    ),
    "solve": (
        "<b>Solve</b><br>Builds the kernel for the current settings, runs the "
        "solver and plots the fit and the distribution."
    ),
    "result": "<b>Result</b>{dynamic}",
}


def text(key: str) -> str:
    """Help text for ``key`` (empty string if there is none)."""
    return HELP.get(key, "")
