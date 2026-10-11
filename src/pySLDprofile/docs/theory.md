# Theory and implementation notes

Reference: S. Hansen, J. Appl. Cryst. 49 (2016) 856–865 (equation numbers below refer to that paper).

## Forward model

ρ(r), 0 ≤ r ≤ r_max, is represented by N piecewise-constant shells, ρ_k on [k, k+1]·r_max/N.
For a centro-symmetric scatterer the orientationally averaged intensity is the square of a real amplitude that is
linear in ρ:

| geometry | amplitude a_k(q) (exact shell integral) | I(q) |
|---|---|---|
| sphere   | 4π ∫ r² sin(qr)/(qr) dr | F² |
| cylinder (∞ long, per length) | 2π ∫ r J₀(qr) dr | (π/q) F² |
| lamella (∞, per area, symmetric) | 2 ∫ cos(qx) dx | (2π/q²) F² |

with F(q) = Σ_k a_k(q) ρ_k. In matrix form this is Hansen's eq. (11), I_i = ρᵀ C_i ρ. He builds C_i from the
p(r) kernel A_ij and Glatter's overlap integrals V_jkl (eqs. 7–10). For these symmetries C_i equals
c(q_i) a(q_i) a(q_i)ᵀ exactly, so the package uses the analytic shell integrals. That gives the same model without
the p(r) discretisation error (the paper uses N_P = 160). With smearing,
C_i = Σ_m R_im c(q'_m) a(q'_m) a(q'_m)ᵀ, where the model is evaluated on a fine q′ grid and smeared with a matrix R:

* `Resolution(dq, kernel="rician"|"gaussian", fwhm=False)`: pinhole resolution. It is the same operator as pyOZgui's
  `polydisperse_fit.Resolution`; the weight matrices agree exactly. The Rician kernel
  P(Q|Q₀) = (Q/σ²)·exp(−(Q²+Q₀²)/2σ²)·I₀(QQ₀/σ²) is the default. It describes 2-D isotropic broadening in the detector
  plane followed by radial averaging, and is evaluated with the scaled Bessel function `i0e`. The Gaussian kernel is
  its large-Q/σ limit and puts weight at Q < 0 when σ/Q is large. Other details:
  * dQ is σ (`fwhm=True` divides by 2.3548);
  * the extended grid is linspace(max(min(Q−3σ), 10⁻³Q_min), max(Q+3σ), 3M) plus the measured Q, so dQ → 0 is exact;
  * trapezoid weights, rows normalised to 1;
  * the background is not smeared.

  `RicianResolution` and `GaussianResolution` are shortcuts for the two kernels.
* `SlitResolution(c)`: I_m(q) = 2∫₀^∞ P(t) I(√(q²+t²)) dt with P(t) = exp(−t²/c²)/(c√π), as in sect. 3.1.
* `MatrixResolution(q_fine, R)`: any user-supplied smearing.

A flat background b can be fitted as an extra, unregularised parameter. This plays the role of the paper's
"free p(0)" term, because p(0) at r = 0 adds a q-independent constant.

p(r) = r^(dim−1) γ(r) (eqs. 4–5) is computed from ρ for output only. For the sphere it is exact, via the shell-pair
identity sinc(qs)·sinc(qt) = (1/2st) ∫_{|s−t|}^{s+t} x·sinc(qx) dx. For the lamella and cylinder it is computed
numerically (1-D correlation, or 2-D FFT autocorrelation).

## Representation: ρ or radial mass (`basis`)

`basis="rho"` (the paper's choice) stores constant ρ_k in each shell. A smooth, compact ρ then always gives an
amplitude F(q) with real zeros, so I = F² has sharp minima. That works for particles with a well-defined surface,
but it fails for data that decay as a power law without minima (χ²/M ≈ 50–90 in tests). The solver then adds edges
and ringing to ρ(r).

Such data need not imply polydispersity. For a Gaussian coil, inverting √(Debye) gives a positive, monotonically
decreasing ρ(r) ≈ (√2/Rg)/(2π²r²)·f(r), with f → 1 for r ≪ Rg and f → 0 at about 3–4 Rg. Its F² is the Debye
function exactly, and F has no zeros. The q⁻² tail comes from the 1/r² cusp at the centre.

`basis="mass"` stores the radial mass g = r^(dim−1)ρ. g is constant within a shell, so ρ = g/r^(dim−1) there, and the
shell amplitudes are exact:

* sphere: 4π [Si(qr)/q]
* cylinder: 2π [∫₀^{qr} J₀/q]
* lamella: same as `rho`

The regularisation acts on g, which is smooth (flat at the centre) for coil-, star- and micelle-corona-like profiles.
On simulated Debye data (Rg = 217 Å, 2 % noise) this gives χ²/M = 0.68, Rg ≈ 221–229 Å and a monotone g(r) that
matches the exact inversion. The `rho` basis on the same data gives χ²/M = 92.

Information limit: structure beyond about π/q_min is barely determined. For coils the evidence for r_max is
therefore flat beyond about 3 Rg, and Rg computed from the profile becomes sensitive to the weakly determined outer
tail at large r_max. Using `outer="zero"` and a fixed or limited r_max gives stabler Rg values. If the evidence
maximum lies at the upper end of the coarse r_max grid, `fit` extends the grid automatically (`max_extend`).

### General exponent p and its Bayesian selection

The representation is generalised to c_k = r^p ρ (any p < dim), so ρ = c_k/r^p within a shell. For p = 0 and
p = dim−1 the shell integrals are analytic. For other p they are computed by Gauss quadrature: Gauss–Jacobi with
weight r^(dim−1−p) on the first shell, where the integrand can be singular, and Gauss–Legendre on the others. The
quadrature agrees with the analytic forms to 10⁻⁷ or better.

p decides which quantity the smoothness prior favours. Different p are therefore different models, and the Bayesian
evidence can compare them (`DensityProfileFit.scan_exponent(p_values, r_max)`; GUI: **Scan p**).

On simulated star data with ρ ∝ r^(−4/3)·exp(−(r/250)⁴) and background 0.05, a scan with the background *fitted*
selects p = 4/3 by ≥ 30 in ln Ev and recovers the background as 0.0500. A slightly wrong *fixed* background (0.0513)
shifts the preferred p to the upper end of the range. So fit the background when scanning p, or fix it only from an
independent measurement.

Rg for ρ = c/r^p uses the analytic shell moments ∫r^(d+1−p)dr / ∫r^(d−1−p)dr.

### Background models (linear and nonlinear)

I_model = (smeared) c(q)F² + B(q; θ), with B a sum of terms (`pysldprofile.background`):

| term | B(q) | parameters |
|---|---|---|
| `Constant` | c | c (fixed or fitted) |
| `PowerLaw` | A q⁻ⁿ | A > 0, n > 0 (n fixed or fitted) |
| `OrnsteinZernike` | I_L/(1+q²ξ²) | I_L > 0, ξ > 0 (ξ fixed or fitted) |
| `FunctionTerm` | any f(q, θ) | user-defined, numerical Jacobian |

The parameters do *not* have to be linear. The model is already nonlinear in ρ and is minimised by Gauss–Newton, so
each free background parameter is just another unregularised column ∂I/∂θ of the Jacobian. Positive parameters are
fitted as ln θ. In the evidence they carry a flat prior and are integrated out in the same Laplace approximation as
ρ: the background block enters ln det(αA + B), not ln det(αA). Their errors come from the same posterior covariance.
The background is evaluated at the measured q and is not smeared.

The evidence also compares background models. On coil data with an added 2·10⁻⁶ q⁻⁴ upturn, a constant-only fit
gives χ²/M = 54. With a power law and n free the fit recovers n = 4.04 ± 0.08 and Rg = 219–224 Å (true 217 Å).
Fixing n = 4 is preferred by the evidence (ln Ev −89.5 vs −93.3).

The limit is **identifiability**, not linearity. A background term with the same q-dependence as the particle
scattering cannot be separated from it. Examples are A q⁻² next to a coil profile, or q⁻⁴ next to a sharp particle
surface. The fit then puts the intensity in either one, and the evidence shows a flat direction. The q range should
contain the regime where the background dominates (e.g. the low-q upturn).

`background=True` (fitted constant) and `background_value=b₀` (fixed constant) remain as shortcuts.

The sign of ρ is undetermined by I = F². The convention used is F(0) = ∫ρ dV > 0.

## Shell grid: uniform, power law, adaptive (`grid`)

The shells can sit on any grid of normalised edges 0 = t₀ < … < t_N = 1 (r_k = r_max·t_k). The grid scales with
r_max, so a single grid serves the whole r_max scan.

* `grid=None`: uniform, as in the paper.
* `grid=("power", γ)`: t_k = (k/N)^γ, so γ > 1 is finer at the centre and γ < 1 finer towards r_max.
* `grid=array`: e.g. from `DensityProfileFit.adaptive_grid(r_max)`.

On non-uniform grids the smoothness prior is written on the actual node positions. Each row is the second
derivative on the uneven grid, scaled by −h̄²/2 (h̄ = r_max/N) and weighted by √(Δr_j/h̄). S then approximates
(h̄⁴/4)∫c″(r)²dr, i.e. smoothness *in r*, whatever the node distribution. For a uniform grid the rows are exactly
Hansen's eq. (14).

### Adaptive grid (equidistribution)

A first fit on the uniform grid, deliberately over-smoothed (α raised by e²), gives c(r) for the represented
quantity r^p ρ. The nodes are then placed so that each shell carries the same integral of

  M(r) = 1 + β |c′(r)| / ⟨|c′|⟩

For a piecewise-constant representation the error per shell is about Δr|c′|, so this equalises it. β defaults
to 3. Two limits apply:

* M is capped at 8 × its minimum.
* Shells are never narrower than min(π/q_max, r_max/N). Structure finer than π/q_max is not determined by I(q),
  and in tests refining below it lowered the evidence (star, N = 80: ln Ev −99 without the floor vs −88.5 with it).

Results (fixed r_max, ln evidence):

| data | N | uniform | adaptive |
|---|---|---|---|
| star, r^(4/3)ρ | 30 | −86.6 | **−76.9** |
| star, r^(4/3)ρ | 80 | −88.6 | −88.5 |
| coil, r²ρ | 30 | −71.1 | **−70.0** |
| coil, r²ρ | 80 | −72.5 | −75.1 |
| sphere with dip, ρ | 30 / 80 | −44.9 / −44.0 | −44.9 / −44.0 |

Adaptive grids pay off when N is small, i.e. when the uniform spacing is coarser than the data resolution. Adaptive
N = 30 then matches or beats uniform N = 80 at a fraction of the cost, with unchanged Rg. Once the uniform spacing
is at or below π/q_max, the uniform grid is already optimal.

Caveat: the adaptive grid is derived from the data, so the prior on that grid partly depends on the data.
Evidence comparisons between uniform and adaptive grids are therefore indicative, not strict.

## Regularisation

Φ(ρ) = α S(ρ) + χ²/2 (eq. 12; the χ²/2 convention of Hansen 2000 is used, so that the evidence formula below holds).

* **Smoothness** (eq. 14): S = Σ (f_j − (f_{j−1}+f_{j+1})/2)². At r = 0 the profile is continued symmetrically
  (f_{−1} = f_1). At r_max the end point is either free (`outer="free"`, the paper's default for ρ) or pulled to
  zero (`outer="zero"`, the paper's "ρ(r_max) = 0"). With a free end, constant profiles cost nothing, so a ridge of
  10⁻⁶·tr(L)/N keeps A non-singular for the evidence.
* **Maximum entropy** (eq. 15): S = Σ f ln(f/m) − f + m, with prior m. The prior shape is a function of r/r_max or an
  array, and its amplitude is fitted to the data. Iterations run in z = ln ρ, which keeps ρ > 0 and uses the
  entropy metric.

## Minimisation

The model is quadratic in ρ, so Φ is minimised with a Levenberg–Marquardt iteration (Gauss–Newton Hessian
JᵀJ + α∇∇S, Jacobi-scaled). Hansen points out that the quadratic problem has **local minima** (sect. 4), and two
measures address this:

1. **α-continuation.** For every r_max, α starts about e⁶ above the natural scale tr(B)/tr(A), where the solution is
   essentially the homogeneous profile or the prior. It is then lowered in steps of Δln α = 0.5, and each solution
   starts from the previous one.
2. **Extra starting points.** Besides the homogeneous profile on [0, r_max], a homogeneous step profile ρ = c for
   r < R ≤ r_max is used, with R and c set by least squares. Without it, a too large r_max converges to a wrong
   branch (χ²/M ≈ 70 in tests). Whenever χ²/M > 2 during the path, both starts are retried.

## Hyper-parameters (Bayesian)

For fixed (α, r_max) the Gaussian approximation of the evidence is

  ln P(α, r_max) = −αS − χ²/2 − ½ ln det(αA + B) + ½ ln det(αA),

with A = ∇∇S and B = JᵀJ. When a background is fitted, b gets a flat prior and is integrated out. The determinants
are evaluated after Jacobi scaling, which matters for max-ent, where A = diag(1/f).

* α: the evidence maximum along the continuation path, refined with a bounded 1-D search.
* r_max: the posterior P(r_max) ∝ ∫ P(α, r_max) d ln α on a coarse grid (default 0.7–1.5 × the Guinier estimate),
  followed by a fine grid around the maximum. The result is reported as mean ± std, plus the best value.
* Error bars on ρ(r), Rg and b: the evidence-weighted average over all solutions on the fine grid. Each solution
  adds its posterior covariance (αA + B)⁻¹ through Monte-Carlo draws. This follows Hansen's IFT error estimate.

Initial r_max comes from a Guinier fit: sphere √(5/3)·Rg; cylinder √2·R_c, from ln(qI) vs q²; lamella √3·R_t, from
ln(q²I) vs q².

## Validation (tests and examples)

* Homogeneous sphere, R = 7.5 nm, 5 % noise, slit smearing (c = 0.1 nm⁻¹), q ∈ [0.12, 1] nm⁻¹: best r_max = 7.51 nm,
  Rg = 5.82 ± 0.01 nm (true value 5.81). With an added background b = 0.01 I(q₁), b is recovered to 0.1 %.
* Inhomogeneous profiles (density dip at 3 nm, R = 5 nm, 1 % + 10⁻⁴ noise) for all three geometries: r_max is
  within 1–4 % of 5 nm and χ²/M ≈ 0.95. As in the paper, the smoothness constraint fills in the minimum partly;
  max-ent with a uniform prior pulls the ends towards the prior.

## Differences from the paper

* The exact rank-one amplitude form replaces the V_jkl overlap-integral route. The model is identical, with no
  p(r) discretisation.
* Hansen's iterative algorithm (after Glatter 1981) is not described in detail, so it is replaced by
  Levenberg–Marquardt with α-continuation.
* The r_max posterior shows a tail towards large r_max when the outer end is free: the profile can then go to zero
  inside r_max. The paper reports narrower posteriors (e.g. 7.508 ± 0.015 nm). Using `outer="zero"` or a narrower
  `r_max_range` gives a sharper estimate.
* The width c of the slit profile in the paper's Fig. 1 is garbled in the PDF text. The examples use c = 0.1 nm⁻¹.
