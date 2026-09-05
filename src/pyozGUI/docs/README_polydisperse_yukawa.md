# Polydisperse one-Yukawa MSA (M=1, N species) — file guide

Status: **M=1 complete and validated for all N, for general $\delta_i$, and with
an analytic RMSA rescaling.** See `docs/polydisperse_yukawa_spec.md` sections
7h-7m for the derivations and `docs/report/report.pdf` for the written-up
version (23 pp.: the three [BH02] misprints, the eq.20 reading trap, and
their status in the literature).

Sign convention: MSA sets $c=-\beta U$, so **$K>0$ is attraction and $K<0$ is
repulsion.**

## Core

| file | role |
|---|---|
| `polydisperse_yukawa_msa.py` | closure solver. `solve()` (amplitudes $a_j$, eq.55) and `solve_gamma()` (scalar $\Gamma$, eq.72+100b) — they agree identically; both are kept as mutual cross-checks. Also `contact_value()`. |
| `polydisperse_yukawa_sq.py`  | $S_{ij}(Q)$ assembly, $S^{\rm AL}=[m\,m^\dagger]^{-1}$ (spec 7i). |
| `polydisperse_rmsa.py`       | analytic polydisperse RMSA: `solve_rmsa()` rescales every diameter by one **common** $\lambda$ until $\min_{ij}g_{ij}=0$. The rescaling enters as $\delta'_i=\exp[-\tfrac z2(\lambda-1)\sigma_i]$, i.e. it is exactly the general-$\delta$ machinery (spec 7m; report §9). |
| `delta_general.py`           | second route to general $\delta_i$ ($T_i$ from eq.51/52/53 instead of from $\hat B$). Superseded as production code by the closed form, retained as a cross-check — agrees to 9e-16. |

## GUI

The whole stack is reachable from `oZgui.py` as a second top-level tab,
**Polydisperse Yukawa**, next to the existing **OZ solver** tab.

| file | role |
|---|---|
| `polydisperse_yukawa_tab.py` | the tab: a self-contained `ttk.Frame` subclass. Knows nothing about the `OZgui` class, so it can be dropped in or removed without touching the solver. Also runnable standalone: `python polydisperse_yukawa_tab.py` |

It offers the size distribution, form factor, $\phi$, $z$, $K$, MSA/RMSA
closure and $Q$ range on the left, and plots of $I(Q)$, the relative error of
each approximation, $S_{ij}(Q)$, $S(Q)$, the discretised distribution and a
numerical summary on the right. Long solves run on a worker thread with
progress in the status line, so the GUI stays responsive. **Compute / Clear /
Export** sit in a footer pinned to the bottom of the control panel, with
everything above them scrolling, so the buttons stay reachable at any window
size and on any platform's font metrics.

**"Solve via" is not a fixed-point accelerator.** Unlike the OZ solver tab,
there is no iteration on $c(r)$ here to accelerate: the closure is solved in
closed form. The dropdown selects between the two independent algebraic
routes of `polydisperse_yukawa_msa.py` — the scalar $\Gamma$ route
(`solve_gamma`, eq.72+100b, the default, and the only one valid at exactly
$K=0$) and the amplitude route (`solve`, eq.55) — and "both" runs the two and
reports their relative difference in the Summary tab as a live self-check
(typically $\sim\!10^{-11}$, set by the amplitude route's `fsolve`
tolerance).

The OZ solver tab is **unchanged**: its panels are built into a notebook frame
instead of directly into the root window (`self.root` -> `self._ozHost`, four
lines) and nothing else. Verified by walking both widget trees: 176 widgets
before and after, identical classes, texts, values, states and widths.

To add another tool tab later, append to `EXTRA_TABS` near the top of
`oZgui.py` — a broken or missing module costs that one tab, not the GUI.

## SAS layer

| file | role |
|---|---|
| `polydisperse_sas_base.py` | exact $I(Q)$ from partial structure factors + **all six** SASfit approximations. Shared by both models so the comparison is byte-identical. |
| `polydisperse_yukawa_sas.py` | size distributions, form factors, and the Yukawa model on that base |
| `robertus_shs_sas.py` | the Robertus sticky-hard-sphere model on the same base |
| `robertusWrapper.py` | ctypes wrapper around SASfit's own `robertus_shs_core.c` |
| `sas_validation.py`, `sas_figures.py` | validation suite and figures |
| `notebooks/polydisperse_yukawa_SAS_demo.ipynb` | worked demo |

Size distributions (all number-weighted, in radius), each with its own exact
quadrature so the moments are reproduced to machine precision:

| class | parameters | quadrature | support |
|---|---|---|---|
| `LogNormal`   | `R0`, `sigma_ln`, `rule` | `rule="legendre"` (default): Gauss-Legendre in log space, truncated at `nsig`. `rule="moment"`: Golub-Welsch from the analytic moments — exact, needs `mpmath` | $(0,\infty)$ |
| `SchulzZimm`  | `Rmean`, `z` or `relsd` | generalised Gauss-Laguerre | $(0,\infty)$ |
| `Gamma`       | `k`, `theta`            | as `SchulzZimm` (delegates to it) | $(0,\infty)$ |
| `Beta`        | `Rmin`, `Rmax`, `a`, `b` | Gauss-Jacobi | $[R_{\min},R_{\max}]$, $R_{\min}>0$ |

`Gamma` is the **same family** as `SchulzZimm` ($k=z+1$, $\theta=\langle R\rangle/(z+1)$) in
the standard shape/scale parameterisation; it delegates rather than duplicating,
so the two cannot drift apart. `Beta` is the only one with compact support, so it fixes the radius range
exactly rather than truncating a tail, and it requires strictly positive
bounds $0<R_{\min}<R_{\max}$. `nsig` is accepted but ignored for `Beta`.

The log-normal is the only one without a closed-form moment-exact Gaussian
rule, and its default rule is measurably the weakest: **1.1e-5** on
$\langle R^6\rangle$ at N=12, against ~1e-16 for the others. Since
$I(Q\to0)$ is set by $\langle R^3\rangle^2$ and $\langle R^6\rangle$, that is
the low-$Q$ error floor. `rule="moment"` removes it (measured: **0.0** on
$\langle R^6\rangle$, and 2e-11 instead of 5.7e-3 on $I(Q<0.02)$ at N=8), at
the cost of an `mpmath` dependency and a wider node span. The GUI exposes it
as a "moment-exact nodes" checkbox on the log-normal.

**Bin count is not the limit it was thought to be.** The `zsigma_max > 45`
guard used to test $z\sigma$ over *all* bins, and fired on healthy
calculations: a Gauss-Laguerre grid puts its outermost nodes at huge radii
carrying no weight at all (at rel.s.d. 0.5 and N=96 the largest node sits at
$z\sigma = 548$ with $w = 5\times10^{-151}$ — an empty species). The guard now
measures the span of bins that actually hold particles, which barely grows
with N. Measured N=12..200 at nominal $z\sigma$ up to 548: contact-matrix
symmetry, the eq.72 residual and the $S_{ij}$ sum rule all stay at 1e-15, and
$I(Q)$ converges (rel.s.d. 0.2: 1.5e-3 at N=12, 1.2e-9 at N=24, 9e-14 at
N=48). N=200 costs ~5 s.

SASfit's six approaches (from `doc/manual/SASfit_ch3.tex`; the GUI exposes 0-4,
the manual documents a sixth): 0 monodisperse, 1 decoupling (Kotlarchyk-Chen),
2 local monodisperse (Pedersen), 3 partial structure factors, 4 scaling
(Gazzillo), 5 van der Waals one-fluid. In 3-5 the "partial" structure factor is
the MONODISPERSE $S$ at the mean radius $(R_i+R_j)/2$, not a true $S_{ij}$.

Building the Robertus engine outside Windows (it needs GSL + SUNDIALS >= 6;
the source targets 7.x, so on 6.x add the shim):

    gcc -O2 -fPIC -shared -include rshs/sundials6_shim.h -o librobertus.so \
        robertus_shs_core.c -lgsl -lgslcblas -lsundials_kinsol \
        -lsundials_nvecserial -lm

On Windows the wrapper finds SASfit's own prebuilt
`src/plugins/robertus_shs/lib/libsasfit_robertus_shs.dll` automatically, or set
`ROBERTUS_SHS_LIB`.

## Validation (run these after any change)

| file | checks |
|---|---|
| `contact_exact_verify.py`   | contact value vs liboneyukawa's exact $(a{+}b)-cZe^{-Z}$ (6e-14); plus the $K{=}0$ limit for $N{=}1,2,3$ against the Lebowitz PY hard-sphere **mixture** solution (4e-16). Note this block must call `solve_gamma()`, not `solve()`. |
| `sq_validation.py`          | $N>1$ large-$Q$ tails of $S_{ij}$ vs contact values |
| `gamma_solve_validation.py` | the six $N>1$ conditions: eq.72, 55, 68, 100, $g_{ij}=g_{ji}$, tails |
| `PiX_two_routes.py`         | eq.34/37 vs eq.30/32/33 — catches species-index errors |
| `Jhat_derivation_check.py`  | derived vs printed $\hat{\mathcal J}$; the three $P^{(n)}$ expressions |
| `eq100_gamma_check.py`      | eq.100/100b independent contact-value route |
| `eq68_check.py`, `eq82_scaling_check.py` | contracted closure; $\Gamma/\Lambda$ scaling |
| `mixture_oz_numerical.py`   | independent NUMERICAL multicomponent OZ-MSA solver (shares no equation with the analytic route) |
| `external_N_validation.py`  | the external $N>1$ check: numerical (Richardson-extrapolated) vs corrected vs as-printed analytic |
| `contact_formula_fix.py`    | derivation + test of the corrected contact value (why eq.20 is *not* it) |
| `contact_from_clib_gtilde.py` | a fourth, fully independent contact value, from the large-$s$ asymptotics of the C library's own $\tilde g(s)$ — no FFT, no tail fit, no [BH02] formula |
| `convention_map_check.py`   | the $(a,b,c,d)\leftrightarrow(A,\beta,C,D)$ mapping onto `liboneyukawa` |
| `baxter_primary_check.py`   | the $K\to0$ limit against Baxter (1968) $\alpha,\beta$ |
| `eq67_alt_closure_check.py` | eq.67, the contracted alternative closure |
| `rmsa_extension.py`         | RMSA: analytic $\lambda$ vs the numerical bisection, and vs Hansen-Hayter at $N{=}1$ |
| `delta_general.py`          | general-$\delta$ closed form vs the eq.51/52/53 fixed point |
| `blum_arias_IJ_check.py`    | adjudicates eq.35/38 against [BA] eq.76 — probe matches [BA] to 1e-15, misses BH02 by up to 18% |
| `eq22_symmetry_test.py`     | decides the eq.22 index from $g_{ij}=g_{ji}$ alone: 1e-16 vs 14–35% asymmetry |
| `cubature_convergence_check.py` | adaptive `hcubature` evaluation of the LMA (1D) and pair-sum (2D) integrals, to confirm the fixed node set is not contaminating the approximation-error figures. Measured: quadrature 1e-6–1e-4 vs scheme error 3e-3–3e-2, so the comparison measures schemes. Needs `pip install cubature`; the 2D part is behind `--pairs` (minutes) |

Notes on the numerical solver: convergence is **first order** in `dr` (hard-core
discontinuity), so run two spacings and Richardson-extrapolate — that reaches
~4e-5 absolute on $S$ and ~1e-4 relative on contact values. When comparing,
evaluate the analytic side at the numerical grid's **own** `k` values; the
nearest grid point can be off by ~0.07 in `k`, which alone swamps the comparison.

## Known misprints in Blum & Hernando 2002 (see spec 7k, report §3-4)

**Literature status:** two of the three real misprints (and the eq.20 reading
trap) are *already corrected*, silently, in Blum's own sequel — Blum & Arias,
arXiv:cond-mat/0602477 (2006) — whose Appendix I says it is *quoting* BH02 and
then prints different indices, with no erratum or remark. Our
transcription-free reconstruction matches [BA] to 1e-15 and misses BH02's
printed eq.35/38 by up to 18%. The sequel carries its own misprint in return
(eq.74 vs BH02 eq.32), so neither paper is authoritative for any single index.
Run `blum_arias_IJ_check.py` and `eq22_symmetry_test.py` to see both.


- **eq. 35** ($\hat{\mathcal I}$) and **eq. 38** ($\hat{\mathcal J}$): $j\leftrightarrow\ell$
  interchange. Invisible at $N{=}1$, wrong for all $N>1$. The code rebuilds both by
  probing eq.30/32/33 instead. eq. 36 and eq. 39 are correct as printed.
- **eq. 31** ($P^{(n)}$): disagrees with eq. 27 and eq. 28 (which agree with each other).
  Not used.
- **eq. 20** is *correct as printed* — it is $q'_{ij}(\sigma_{ji})$ for the full $q$,
  and the paper says it is for the factor-function symmetry requirements. It is simply
  not the contact value: that is the *jump* $q'(\sigma^-)-q'(\sigma^+)$, so only $C$
  survives, not $(C{+}D)$ (spec 7h). [BA] eq.26 writes the jump explicitly, and equals
  our contact value to 4e-16 under the scaling map in report §3.1.
- **eq. 22** ($D_{ij}$): $\delta$ sits on the wrong index. It must be
  $D^{(n)}_{ij}=-\delta^{(n)}_i a^{(n)}_j e^{z_n\sigma_{ij}}$. Invisible whenever
  $\delta_i\equiv1$ — every $\delta{=}1$ result is bit-identical before and after —
  but it is what makes charge polydispersity, and hence RMSA, close in closed form
  (report §3.5).
- **[BA] eq. 28/29**: the rank-one form disagrees with eq.52/53 by a large
  $s$-dependent factor; not used (spec 7i).

## Open

1. $M>1$ (multi-Yukawa) — the $M(M{-}1)$ symmetry conditions of [BH02] §4 bite here.
2. Complex $z_n$ (oscillatory closures).
3. Very broad distributions (rel. s.d. $\gtrsim0.4$) are untested.
4. Which RMSA rescaling condition to impose is a *modelling* choice, not a
   derivation: one common $\lambda$ with $\min_{ij}g_{ij}=0$ is implemented;
   per-species $\lambda_i$ is a defensible alternative.
5. `solve()` is singular at exactly $K=0$; use `solve_gamma()` there.

(Resolved since the first draft: $\delta_i\neq1$ — closed form, see eq.22 above.)
