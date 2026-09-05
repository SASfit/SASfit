---
title: Polydisperse Multi-Yukawa Structure Factor — Mathematical Specification (DRAFT)
status: M=1 complete for all N -- contact value (7h) and eq.35/38 misprints (7k) author-verified, S_ij(Q) assembly (7i), independent numerical validation (7l). Open: delta_i != 1, M>1, complex z_n
---

# Polydisperse Multi-Yukawa Structure Factor: Mathematical Specification

**Status: DRAFT.** The $M=1$ case is now implemented (`polydisperse_yukawa_msa.py`) and validated to machine precision against the compiled `liboneyukawa` reference — see §7h, which also corrects the wrong conclusions reached in §7e and §7g. This document consolidates the literature derivation into one
precise, self-contained statement of what is given, what is unknown, and which
equations close the system. Per this repository's `AI_USAGE.md`, this is scaffolding
for human review, not an authoritative derivation — every equation below should be
checked against the cited source before anything is implemented from it. Equation
tags: **[BA-n]** = Blum & Arias 2006 (arXiv:cond-mat/0602477), **[BH02-n]** = Blum &
Hernando 2002 (J. Phys.: Condens. Matter 14, 11933), **[BH78-n]** = Blum & Høye 1978
(J. Stat. Phys. 19, 317). All three are in
`FormFactors4SASfit/In Progress/4Claude/`. [BA] and [BH02] were confirmed by reading
rendered page images directly (not the OCR text layer, which garbles dense
equations); [BH78] is a poor-quality 1978 scan and is cited here only for
cross-context, not as a source of transcribed equations.

## 1. Physical setup

We have $N$ species $i=1\ldots N$ with hard-core diameters $\sigma_i$ and number
densities $\rho_i$ (this is where size polydispersity enters — each species is one
bin of a discretized size distribution). The direct correlation function outside
contact is a sum of $M$ Yukawa terms [BH02-5]:

$$c_{ij}(r) = \sum_{n=1}^{M} K_{ij}^{(n)}\, e^{-z_n(r-\sigma_{ij})}/r, \qquad r>\sigma_{ij}=\tfrac12(\sigma_i+\sigma_j)$$

with $g_{ij}(r)=0$ for $r\le\sigma_{ij}$. In the **factored (charge-like) case**
[BA-4]:

$$K_{ij}^{(n)} = K^{(n)}\,\delta_i^{(n)}\delta_j^{(n)}, \qquad \delta_i^{(n)} = d_i^{(n)} e^{-z_n\sigma_i/2}$$

$d_i^{(n)}$ is a per-species amplitude for Yukawa term $n$ — this is where **charge
(coupling) polydispersity** enters, as a separate degree of freedom from size
polydispersity. Modeling choice, not fixed by the math: $d_i^{(n)}$ could be uniform
across species (pure size-polydispersity study), proportional to $\sigma_i$ or to a
per-species valence/charge, etc. — needs an explicit decision when we get to code.

**Given inputs:** $\{\sigma_i,\rho_i\}_{i=1}^N$, $\{z_n, K^{(n)}\}_{n=1}^M$,
$\{\delta_i^{(n)}\}$ (or equivalently $d_i^{(n)}$).

## 2. Known (purely geometric) quantities

No unknowns enter these — pure functions of $\{\sigma_i,\rho_i,z_n\}$:

$$\zeta_k = \sum_i \rho_i \sigma_i^k \quad[BA\text{-}78\,/\,BH02\text{-}40], \qquad \Delta = 1-\pi\zeta_3/6 \quad[BA\text{-}79\,/\,BH02\text{-}41]$$

$$A_j^0 = \frac{2\pi}{\Delta}\Big[1+\tfrac12\zeta_2\tfrac{\pi}{\Delta}\sigma_j\Big], \qquad \beta_j^0 = \frac{\pi}{\Delta}\sigma_j \quad[BA\text{-}71\,/\,BH02\text{-}25,26]$$

Confirmed identical in both papers via page images (an earlier pass flagged an
apparent mismatch here, but that was from comparing a verified [BA] image reading
against a garbled OCR-text reading of [BH02] — the clean [BH02] page image matches
[BA] exactly). [BH02-28] gives a useful alternative, more direct route to $P^{(n)}$
in terms of $\Delta^{(n)}$ and $\hat B_\ell(z_n)$ directly, bypassing $\Pi,X$:
$$P^{(n)} = \Big(\zeta_2-\tfrac{\Delta z_n}{\pi}\Big)\Delta^{(n)} + \sum_\ell\rho_\ell\big[\sigma_\ell^2\phi_0(\sigma_\ell z_n)\hat B_\ell(z_n) + \sigma_\ell\delta_\ell^{(n)}\big]$$

Special functions, incomplete-gamma based [BA-83,84 / BH02-44,45,46]:
$$\phi_0(x)=\frac{1-e^{-x}}{x}, \quad \phi_1(x)=\frac{1-x-e^{-x}}{x^2}=x\psi_1(x)-\phi_0(x)/2, \quad \psi_1(x)=\frac{1-x/2-(1+x/2)e^{-x}}{x^3}$$
all evaluated at $x=z_n\sigma_i$ for the various $(n,i)$ combinations that appear below.

## 3. Unknowns

Two coupled families, each of size $N\times M$ ($N$ species $\times$ $M$ Yukawa terms):

- **Baxter coefficients** $a_i^{(n)}$ — the primary closure unknowns.
- **$\hat B_j(z_n)$** — a Laplace-transform-at-the-pole quantity,
  $\hat B_j(z_n) = 2\pi\sum_i \rho_i\delta_i^{(n)}\tilde g_{ij}(z_n)e^{z_n\sigma_{ij}}$
  [BH02-43 / BA-82], where $\tilde g_{ij}(s)=\int_0^\infty dr\, r\, g_{ij}(r)e^{-sr}$
  [BH02-42]. Despite the definition referencing the full pair correlation function,
  $\hat B_j(z_n)$ closes **algebraically** in terms of the $a_i^{(n)}$ (see §5) — no
  actual Laplace transform needs to be computed at runtime.

**Open question, not yet resolved (see §7):** Blum-Arias's main text (its eq. 25) also
defines a contact-value-based quantity $\mathcal{B}_i^{(m)}=2\pi\sum_k\rho_k\,
g_{ik}(\sigma_{ik})\,\delta_k^{(m)}$, which *looks* different from $\hat B$ (no
exponential prefactor, plain contact value instead of a transform). Working
assumption for this spec: $\mathcal{B}\equiv\hat B$ (standard residue-at-the-pole
identity in this formalism). **Not proven here** — flagged as the thing the M=1
validation (§7) needs to confirm.

## 4. Auxiliary quantities (linear in the unknowns)

These require the geometric quantities from §2 plus the unknowns from §3, but no
further unknowns:

$$\hat I_{j\ell}^{(n)} = \delta_{j\ell} + \rho_\ell\Big[\beta_j^0\tfrac{\sigma_\ell^2}{2}\phi_0(z_n\sigma_\ell) - (A_j^0+z_n\beta_j^0)\sigma_\ell^3\psi_1(z_n\sigma_\ell)\Big] \quad [BH02\text{-}35]$$
$$\hat J_{j\ell}^{(n)} = \delta_{j\ell}\sigma_j\phi_0(z_n\sigma_\ell) - 2\rho_\ell\beta_j^0\sigma_\ell^3\psi_1(z_n\sigma_\ell) \quad [BH02\text{-}38]$$
$$\hat\xi_j^{(n)} = -\frac{1}{z_n^2}\sum_\ell \rho_\ell\delta_\ell^{(n)}\Big[z_n\beta_j^0 + A_j^0\big(1+\tfrac{z_n\sigma_\ell}{2}\big)\Big] \quad [BH02\text{-}36]$$
$$\hat\gamma_j^{(n)} = \delta_j^{(n)} - \frac{2\beta_j^0}{z_n^2}\sum_\ell\rho_\ell\delta_\ell^{(n)}\big(1+\tfrac{z_n\sigma_\ell}{2}\big) \quad [BH02\text{-}39]$$

then, **linear in $\hat B$**:

$$\Pi_j^{(n)} = \hat\xi_j^{(n)} + \sum_\ell \hat I_{j\ell}^{(n)}\hat B_\ell(z_n), \qquad X_j^{(n)} = \hat\gamma_j^{(n)} + \sum_\ell \hat J_{j\ell}^{(n)}\hat B_\ell(z_n) \quad [BH02\text{-}34,37]$$

Rescaled (transposed!) versions used in the closure equation itself [BH02-56] — **note
the index transpose, easy to get backwards**:

$$\mathcal I_{j\ell}^{(n)} = \hat I_{\ell j}^{(n)}\,\rho_\ell/\rho_j, \qquad \mathcal J_{j\ell}^{(n)} = \hat J_{\ell j}^{(n)}\,\rho_\ell/\rho_j$$

## 5. The closing equations

**(a) Primary MSA closure** — pins down $a_j^{(n)}$, for every species $j$ and every
Yukawa term $n$ [BH02-55], confirmed against the rendered page image:

$$2\pi K^{(n)}\delta_j^{(n)}/z_n \;+\; \sum_\ell a_\ell^{(n)}\mathcal I_{j\ell}^{(n)} \;-\; \sum_m \frac{1}{z_n+z_m}\Big\{\sum_k \rho_k a_k^{(n)}a_k^{(m)}\Big\}\Big\{\sum_\ell\big[\mathcal J_{j\ell}^{(n)}(\Pi_\ell^{(m)}-z_m X_\ell^{(m)}) - \mathcal I_{j\ell}^{(n)}X_\ell^{(m)}\big]\Big\} = 0$$

This is **quadratic (bilinear)** in $\{a_k^{(n)}\}$ via the $\sum_k\rho_k a_k^{(n)}a_k^{(m)}$
factor — structurally the same class of equation as the Baxter factorization already
solved elsewhere in this codebase (Robertus SHS, one-Yukawa MSA quartic).

**(b) $\hat B$ self-consistency** — closes $\hat B_j(z_n)$ in terms of the same
unknowns, via [BH02-63/71] (general $s$, evaluated at $s=z_n$):

$$2\pi K^{(n)}\hat B_j(z_n) = \sum_m \frac{z_n}{z_n+z_m}\Big\{\sum_k\rho_k a_k^{(n)}a_k^{(m)}\Big\}\big[-\Pi_j^{(m)}+z_m X_j^{(m)}\big] + \tilde\Delta_j(z_n) \quad [BH02\text{-}63]$$

with, confirmed via page image [BH02-64]:

$$\tilde\Delta_j(z_n) = -\sum_\ell\rho_\ell a_\ell^{(n)}\Big\{\frac{1}{z_n}A_\ell^0\big(1+\tfrac{z_n\sigma_j}{2}\big)+\beta_\ell^0\Big\} - \sum_m\sum_\ell\rho_\ell a_\ell^{(n)}a_\ell^{(m)}\Big\{\frac{\pi}{z_n\Delta}P^{(m)} + \frac{z_m}{z_n+z_m}\Big[\Delta^{(m)}+\frac{\sigma_j\pi}{2\Delta}P^{(m)}\Big]\Big\}$$

Note $\tilde\Delta_j(z_n)$ depends on $a$ (linearly and quadratically) and, through
$P^{(m)},\Delta^{(m)}$, indirectly on $\hat B$ as well — so (b) really is a coupled
self-consistency, not $\hat B$ expressed purely in terms of $a$.

Since $\Pi,X$ are themselves linear in $\hat B$ (§4), (b) is, **for fixed $a$**, a
linear system in $\hat B$. And (a), for fixed $\hat B$ (hence fixed $\Pi,X,\mathcal
I,\mathcal J$), is a quadratic system in $a$. This is the basis for the proposed
numerical scheme below.

## 6. Proposed numerical solution strategy (not yet implemented)

Picard-style alternation, consistent with this project's existing numerical OZ
solver approach (Picard iteration on a matrix-generalized fixed point):

1. Initialize $a_i^{(n)}$ (e.g. zero, or the corresponding monodisperse RMSA/one-Yukawa
   solution per species as a warm start).
2. Given $a$, solve the linear system (b) for $\hat B_j(z_n)$ (all $j,n$ jointly —
   $\hat I,\hat J$ don't depend on $\hat B$, so this is a genuine linear solve, size
   $N\times M$).
3. Given $\hat B$ (hence $\Pi,X$), update $a$ from the quadratic system (a) — either
   a full Newton step on the $N\times M$-dimensional residual, or a Picard step
   treating the $\sum_k\rho_k a_k^{(n)}a_k^{(m)}$ factor as frozen from the previous
   iterate (simpler, matches this project's existing Picard-iteration convention, at
   the cost of needing more iterations).
4. Iterate 2–3 to convergence (residual of (a) and (b) jointly below tolerance).
5. Recover $A_j,\beta_j$ [BA-70], then $q_{ij}(r)$ [BH02-17,18], then
   $\tilde g_{ij}(s)\to S_{ij}(Q)$ via $s\to iQ$ — **this final assembly step (partial
   structure factors → the measurable, form-factor-weighted $S(Q)$) has not been
   worked out in this session and is a separate open item.**

This is a proposal for review, not a decision — flagging for explicit sign-off before
any of it becomes code, per this repo's human-verification policy.

## 7. Validation targets (task 5)

- **N=1, M=1**: must reduce to the existing, already-validated `rmsaWrapper.py`
  (Hayter-Penfold RMSA) and `oneYukawaWrapper.py` (one-Yukawa MSA) results, and to
  Blum-Arias §6's worked one-component continued fraction.
- **N>1, M=1** (polydisperse diameters, single Yukawa term): Blum-Hernando §6 ("The
  1-Yukawa limit", eq. 97–105) gives an explicit closed form for this exact case —
  contact value, excess entropy, excess pressure — and is a stronger test than N=1
  alone since it directly exercises the diameter-polydispersity machinery.
- Either of these reductions failing to match would be the signal that the
  $\mathcal B\equiv\hat B$ assumption (§3) or the $A_j^0$ discrepancy (§2) is wrong.

## 8. Open items

1. ~~**$\mathcal B$ vs $\hat B$** (§3)~~ — **moot as of §7h.** The corrected
   contact-value formula does not use $\mathcal B$ (eq. 25 [BA]) at all: it is
   built from $A_j,\beta_j,a_j$ and $C_{ij}^{(m)}$, with $C$ eliminated via
   eq. 21/22 in terms of $\hat B$, whose own definition (eq. 43 [BH02]) is
   independently confirmed by the exact `Y_g` $\equiv\tilde g$ identification.
   So the question of whether $\mathcal B\equiv\hat B$ no longer gates
   anything. (§7c's earlier test of that substitution found a $<1\%$ effect
   because both variants it compared shared the eq. 26 error.)
2. **Final $S_{ij}(Q)$ assembly** (§6 step 5) — **DONE and validated at
   $N{=}1$** (§7i): $S^{\rm AL}=[m\,m^\dagger]^{-1}$, matching `liboneyukawa`
   to $\le3\times10^{-10}$. Correct for $N>1$ only up to the eq. 72 defect in
   item 4 below. Previously recorded here as: The blocker recorded here previously (the $N{=}1$
   $K\neq0$ contact-value discrepancy) is resolved and author-verified in §7h,
   and the closure it sits on is confirmed against eq. 68 [BH02], so the
   general $S_{ij}(Q)$ formula can now be derived on a sound footing. The
   exact `liboneyukawa` correspondence established in §7h ($\tilde q=2\pi$
   `Y_q`, `Y_g`$\,\equiv\tilde g$, and the $A,\beta,C,D$ map) also gives a
   ready-made $N{=}1$ validation target for whatever $S(Q)$ expression comes
   out — the C source's own `Y_pc`/`SqOneYukawa` can be compared term by term.
3. **$\delta_i\neq1$ (charge/coupling polydispersity)** — the contact-value
   formula's $C_{ij}$ elimination uses $\tilde g$'s symmetry, which closes in
   terms of $\hat B$ alone only for $\delta_i=1$ (common coupling, arbitrary
   size polydispersity). For general $\delta_i^{(n)}$, $\hat B_j$ contracts
   $\tilde g$ with a $\delta_i$ weight and no longer determines the individual
   $\tilde g_{ik}$, so $C_{ij}$ must come from the full eq. 21 solve. Not yet
   derived; `contact_value()` raises `NotImplementedError` in that case.
4. ~~**$N>1$: the eq. 72 scaling condition is violated**~~ — **RESOLVED in
   §7k**: eq. 35 and eq. 38 are misprinted in [BH02] ($j\leftrightarrow\ell$),
   invisible at $N{=}1$. Rebuilding $\hat{\mathcal I},\hat{\mathcal J}$ from
   eq. 30/32/33 makes all six $N>1$ conditions hold simultaneously at machine
   precision. Previously recorded as:
   [BH02] eq. 72 requires $\Gamma=-\Pi_i/X_i$ (at $M{=}1$) to be
   species-independent: $N{-}1$ constraints the closure of §5 never imposes.
   Our $N>1$ solutions violate it, and the violation quantitatively predicts
   the observed $N>1$ errors — off-diagonal $S_{i\neq j}$ tails (0.05-2.5%,
   §7i) and eq. 100 ($10^{-3}$-$8\times10^{-3}$, §7j) — while everything at
   $N{=}1$, and every *diagonal* quantity, stays exact. Fixing this means
   imposing the §4 symmetry system (eq. 72-87, i.e. $\Gamma,\Lambda,\Upsilon$
   and eq. 74/75) as part of the solve rather than treating the $a_j$ as
   unconstrained. Note this is a **closure** defect, not an assembly one, and
   it is what the paper's abstract means by "$M$ equations together with
   $M(M-1)$ symmetry conditions".
5. ~~**$K=0$ exactly**~~ — `solve()` still hits a singular matrix at
   precisely $K=0$, but `solve_gamma()` handles it exactly ($\Gamma=0$), so
   this is no longer a gap. `solve()` could still get a short-circuit.
6. **Complex $z_n$** — the papers allow complex Yukawa exponents (for oscillatory
   closures); this spec assumes real $z_n$ throughout. Revisit if complex terms are
   ever needed.
7. **Choice of $d_i^{(n)}$** (§1) — physical modeling decision (size-only vs.
   size+charge polydispersity), not a math question; needs your input before coding.

## 7a. Validation results so far (implemented in `polydisperse_yukawa_msa.py`)

A first Python implementation of sections 2–6 (Picard scheme: linear solve for
$\hat B$ given $a$, nonlinear solve for $a$ given $\hat B$) has been built and
checked:

- **N=1 hard-sphere limit** ($K\to$ small/handled as a separate direct check of
  $A_j^0,\beta_j^0$, since $a$ and $\hat B$ trivially decouple exactly at $K=0$):
  contact value matches the exact PY result $g(\sigma^+)=(1+\varphi/2)/(1-\varphi)^2$
  to machine precision.
- **N=2 polydisperse hard-sphere limit**: contact values $g_{ij}(\sigma_{ij}^+)$
  match the exact Lebowitz PY-mixture formula
  $g_{ij}=\frac{1}{\Delta}+\frac{\pi\zeta_2}{2\Delta^2}\frac{\sigma_i\sigma_j}{\sigma_i+\sigma_j}$
  to machine precision — validates the multi-species geometric machinery (§2, §4)
  beyond N=1. (Note: an initial attempt at this check used the wrong prefactor
  convention from memory and appeared to fail by ~25–30%; re-deriving with the
  standard $\xi_2=\frac{\pi}{6}\zeta_2$ convention fixed it. Flagging this so the
  lesson survives: don't trust a from-memory literature formula as ground truth
  without checking its convention.)
- **N=1, $K\neq0$ full solve**: the primary closure residual (§5a) converges to
  $\sim10^{-16}$ — the nonlinear solve is internally self-consistent.
- **Direct $K\neq0$ cross-check against `oneYukawaWrapper.py`/`liboneyukawa.so`:
  now done** — see §7b. Uses the large-$q$ asymptotic tail of $S(Q)$ (no FFT, no
  grid) to read the C library's own contact value directly, since it's exact at
  $K\to0$ to 5 significant figures. Result: a real, non-numerical mismatch for
  $K\neq0$ that grows like $O(K)$ and has the wrong sign of trend — logged as an
  open item, not yet resolved.

## 7b. Task 10 diagnostic: $K\neq0$ cross-check against `liboneyukawa.so` (N=1)

Before deriving the general $S_{ij}(Q)$ assembly formula from scratch, tried an
independent numerical route to get *a* contact value out of the reference C
library, to have something concrete to derive towards. Two attempts:

1. **FFT/`PairCorrelation` route** (call `Y_SolveEquations`, build $S(Q)$ on its
   own 16384-point/$q_{max}=1000$ grid, run it through the library's own
   `PairCorrelation` FFT-inversion, read off $g(r)$ near $r=\sigma$). Even at
   $K\to0$ (exact answer known: $g=(1+\varphi/2)/(1-\varphi)^2=1.71875$ at
   $\varphi=0.2$), this gave $\approx1.672$–$1.675$ — a genuine $\sim2.6\%$ miss,
   diagnosed as Gibbs-ringing/finite-grid error from FFT-inverting a function
   with a jump discontinuity at $r=\sigma$ (the real-space grid spacing
   $dr\approx0.00628$ doesn't land exactly on $r=1$, and a truncated Fourier sum
   at a jump doesn't converge pointwise to the physical limit). **Abandoned** —
   too imprecise to trust for anything beyond an order-of-magnitude sanity check.
2. **Large-$q$ asymptotic tail route** (no FFT, no grid): for any fluid with a
   hard core at $r=\sigma=1$, standard liquid-state asymptotics give
   $S(q)-1 \to 4\pi\rho\sigma\,g(\sigma^+)\cos(q\sigma)/q^2$ as $q\to\infty$ (in
   reduced units, $\rho=6\varphi/\pi$, this is $S(q)-1\to
   24\varphi\,g(\sigma^+)\cos(q)/q^2$). Extracting the amplitude from
   `SqOneYukawa` evaluated directly at $q\in[300,995]$ (no discretization at
   all) reproduces the exact $K\to0$ hard-sphere value to 5 significant figures
   ($1.71887$ vs. $1.71875$) — this method is trustworthy and was used for
   everything below.

**Result: for $K\neq0$, this tail-extracted contact value does *not* match this
spec's `contact_value()` output**, and the mismatch is not numerical noise:

| $K$ | tail-extracted $g(\sigma^+)$ (C lib) | this spec's `contact_value()` | ratio |
|---|---|---|---|
| 0.01 | 1.30443 | 1.29543 | 1.007 |
| 0.1  | 1.37779 | 1.28762 | 1.070 |
| 0.4  | 1.62487 | 1.26125 | 1.288 |
| 1.6  | 2.65702 | 1.15025 | 2.310 |

(all at $Z=6$, $\varphi=0.1$). Two things stand out:

- The deviation is $O(K)$ at small $K$ (linear, not quadratic) — points at a
  missing/mis-signed term linear in $a$ or $\hat B$, not a higher-order effect.
- **The sign of the trend is wrong.** The C library's own docstring states its
  potential convention as $V(r)=-K\,e^{-Z(r-1)}/r$ for $r>1$ ($K>0$ =
  attractive), and its tail-extracted contact value correctly *increases* with
  $K$ (attraction should enhance the contact probability). This spec's
  `contact_value()` *decreases* with $K$ instead over the same range. Flipping
  the sign of $K$ fed into this spec's solver (`mine(Z, -K, phi)`) does flip the
  trend direction to increasing, but doesn't fix the magnitude (e.g. at
  $K=1.6$: tail says $2.657$, sign-flipped solver says only $1.428$) — so this
  is not simply a $K\to-K$ convention mismatch between the two codes; something
  in the closure/contact-value derivation itself needs to be re-checked.
- The closure residual for $a$ (§5a) is still $\sim10^{-16}$ throughout, and the
  C library's own `Y_CheckSolution` also returns true throughout — both solves
  are internally self-consistent, they just aren't computing the same physical
  answer. That rules out a loose numerical-solver bug and points at the
  hand-transcribed equations themselves (most likely candidates: the primary
  closure eq. (§5a), the $\hat B$ self-consistency eq. (§5b), or the
  `contact_value()` formula (eq. 26 [BA]/20 [BH02]) — needs a fresh line-by-line
  re-check against the page images, not another numerical patch).

Checked more broadly across $(Z,K,\varphi)$ to rule out this being an artifact
of one parameter choice — the mismatch is systematic everywhere tried, both
solves stay self-consistent, and the ratio grows monotonically with $K$:

| $Z$ | $K$ | $\varphi$ | tail-g (C lib) | `contact_value()` | ratio |
|---|---|---|---|---|---|
| 3  | 0.1 | 0.05 | 1.22052 | 1.13278 | 1.077 |
| 3  | 0.5 | 0.15 | 1.79230 | 1.40956 | 1.272 |
| 3  | 1.0 | 0.25 | 2.41716 | 1.75355 | 1.378 |
| 6  | 0.1 | 0.05 | 1.22644 | 1.13137 | 1.084 |
| 6  | 0.5 | 0.10 | 1.70814 | 1.25235 | 1.364 |
| 6  | 1.0 | 0.10 | 2.13166 | 1.20693 | 1.766 |
| 6  | 2.0 | 0.20 | 3.10590 | 1.35960 | 2.284 |
| 10 | 0.5 | 0.10 | 1.73756 | 1.26668 | 1.372 |
| 10 | 2.0 | 0.30 | 3.62635 | 1.98834 | 1.824 |

**Third independent check, requested explicitly: `ozLib`'s own real-space
numerical MSA solve.** Rather than trust only `liboneyukawa.so`'s
semi-analytic Baxter-factorization solve, ran the *same* physical system
(hard sphere + Yukawa tail, MSA closure) through this project's own
general-purpose real-space fixed-point OZ solver (`ozLib.solve(potential=
'Yukawa', potentialArgs=(1/Z, -K, True), closure='MSA', ...)`, `scipy
Anderson`) — a completely different numerical method (grid-based Picard/
Anderson fixed point, not a quartic/Baxter factorization at all), reading
$g(\sigma^+)$ off by linear extrapolation of the first few grid points
outside the hard core back to $r=\sigma$ (grid resolved to
$dr=\sigma/4000$). Convention check: $V(r)=-K e^{-Z(r-1)}/r$ for $r>1$
matches `setYukawaPotential(shieldingLength=1/Z,
interactionStrength=-K, doAddHardSphere=True)` exactly (its own
`boltzmannOfP2Ppotential` formula was read to confirm the sign).

| $Z$ | $K$ | $\varphi$ | `liboneyukawa` (tail-extracted) | `ozLib` (real-space MSA) | this spec's `contact_value()` |
|---|---|---|---|---|---|
| 6 | 0.0    | 0.20 | 1.71887 | 1.71940 | 1.71875 |
| 6 | 0.5    | 0.10 | 1.70814 | 1.70844 | 1.25235 |
| 6 | 1.0    | 0.10 | 2.13166 | 2.13203 | 1.20693 |
| 6 | 2.0    | 0.20 | 3.10590 | 3.10680 | 1.35960 |
| 3 | 0.5    | 0.15 | 1.79230 | 1.79271 | 1.40956 |

`liboneyukawa.so` and `ozLib` — two independent numerical methods, one
semi-analytic (Baxter/quartic) and one fully numerical (real-space fixed
point), sharing nothing but the MSA closure itself — agree with each other
to 3–4 significant figures across every case, including $K=0$. This makes it
essentially certain the bug is in this spec's own closure/contact-value
derivation (`polydisperse_yukawa_msa.py`), not in either reference. The
reference value to re-derive against is now solid; next step is the
line-by-line re-check of the primary closure/$\hat B$/contact-value
equations mentioned above.

This is flagged as an open item rather than resolved unilaterally, per this
project's AI_USAGE.md policy on core derivations.

## 7c. Task 10 diagnostic (cont'd): checked the contact-value formula (eq. 26) itself

Re-read [BA] maintext page 6 (eq. 21-27) and Appendix I page 14 (eq. 74-82)
side by side. Confirmed: eq. (26) (the contact-value formula) explicitly
uses the **un-hatted** $\mathcal B_i^{(m)}$ (eq. 25:
$\mathcal B_i^{(m)}=2\pi\sum_k\rho_k\delta_k^{(m)}g_{ik}(\sigma_{ik})$ — a
direct, self-referential function of the contact values themselves), while
`contact_value()` had been plugging in the **hatted** $\hat B$ (the quantity
solved for via the $\Pi/X/\Delta$ machinery, eq. 74-82) instead. These are
genuinely different definitions in the paper.

Tested whether this substitution is actually the bug. Since $\mathcal B$
(eq. 25) is defined in terms of the contact value it appears in, eq. 25 +
eq. 26 together form a **linear, self-consistent system that can be solved
directly for $g_{ij}(\sigma_{ij})$ without $\hat B$ at all** — for $N{=}1,
M{=}1$ this reduces by hand to
$$g(\sigma)=\frac{A\sigma/2+\beta-z\delta a\,e^{-z\sigma}}{2\pi(\sigma-\rho\delta a\,e^{-z\sigma})}.$$
Compared against the old ($\hat B$-based) formula at several $(Z,K,\varphi)$,
including small $Z$ where $e^{-z\sigma}$ is not tiny (so the two versions of
"B" would matter most if they actually differed numerically):

| $Z$ | $K$ | reference | old (uses $\hat B$) | new (uses eq. 25's $\mathcal B$) |
|---|---|---|---|---|
| 6.0 | 0.5 | 1.708 | 1.2521 | 1.2521 |
| 6.0 | 1.0 | 2.132 | 1.2069 | 1.2065 |
| 2.0 | 0.3 | 1.498 | 1.2785 | 1.2698 |
| 2.5 | 0.5 | 1.654 | 1.2515 | 1.2416 |

The two formulas agree with each other to within ~1% everywhere (the
$\hat B$/$\mathcal B$ substitution only ever enters multiplied by
$a\,e^{-z\sigma}$, which stays small), and — critically — **both are still
off from the reference by the same large margin** (15-40%, not 1%). So
switching which "B" feeds eq. 26 does not close the gap.

**Conclusion: eq. 26 (the contact-value formula) itself is very likely not
where the bug is.** $A$, $\beta$, and $a$ are the common inputs to both
versions and are themselves unchanged by this test, and $A,\beta$ (eq. 70)
depend on $\hat B$ far more strongly, through $P^{(n)},\Delta^{(n)}$ (eq.
72-73) — with no $e^{-z\sigma}$ suppression there. That makes $A,\beta,a$
(hence the primary closure equation §5a and the $\Pi/X/\hat B$
self-consistency loop §5b/eq. 74-82 that feeds them) the next place to
re-check by hand, not the contact-value formula.

## 7d. Task 10 diagnostic (cont'd): re-checked the primary closure equation and everything feeding it

Re-read [BH02] pages 5-8 (eq. 19-39, 51-66) side by side with the code and
found one real, confirmed transcription bug: the $\hat{\mathcal I}_{j\ell}^{(n)}$
and $\hat{\mathcal J}_{j\ell}^{(n)}$ matrices (eq. 35, 38) had the row index
($j$, which $A^0,\beta^0,\sigma,\phi_0,\psi_1$'s "fixed" argument should
carry) and the summed index ($\ell$, which $A_\ell^0,\beta_\ell^0$ should
carry) swapped. **Fixed in `polydisperse_yukawa_msa.py`.** But this bug is
mathematically invisible whenever $N=1$ (row and summed index are always the
same single value, so which array gets which label doesn't matter) — and
every $K\neq0$ cross-check run so far (§7b, §7c) has been $N=1$. Confirmed
numerically: re-running the N=1 reference table after the fix reproduces
*exactly* the same (wrong) numbers as before. So this fix is real and
necessary for the eventual N>1 polydisperse case, but it does **not**
explain the discrepancy this task has been chasing.

Went on to re-verify, against the page images, everything else that could
matter for N=1: $A_j^0,\beta_j^0$ (eq. 25-26), $\hat\xi_j,\hat\gamma_j$ (eq.
36, 39), $\Pi_j,X_j$ (eq. 34, 37), $P^{(n)},\Delta^{(n)}$ both forms (eq. 27
and 29), the full $\hat B$ self-consistency equation (eq. 62-64 — the
corrected BH02 result, not the "two typos" BVH92 form eq. 65 shown next to
it for comparison), and the primary closure equation itself (eq. 55). Every
one of these transcribes exactly against the source. Also checked the
closure's own normalization (eq. 5: $c_{ij}(r)=K_{ij}^{(n)}e^{-z_n(r-
\sigma_{ij})}/r$) against the potential convention used by
`liboneyukawa`/`ozLib` — matches with no hidden prefactor. Also ruled out
root multiplicity: scanning $a\in[-20,20]$ for $(Z,K,\varphi)=(6,1,0.1)$
finds only two self-consistent roots — $a\approx-0.971$ (physical, positive
contact value, the one the solver already converges to from every sane
starting guess) and $a\approx-14.24$ (unphysical, negative contact value).

**Net result: every individual equation checks out against the page images,
one real (but N=1-irrelevant) bug was found and fixed, and the N=1
discrepancy against `liboneyukawa`/`ozLib` remains unexplained.** This
suggests either a transcription error subtle enough to have survived
several independent re-readings, or a convention mismatch between how [BA]
and [BH02] are being combined (they are two different papers, and this spec
has been cross-using equations from both). The most decisive remaining
diagnostic is probably to derive the actual Baxter $Q(r)$ function (eq.
17-18) explicitly for N=1 and compare it term-by-term against
`liboneyukawa`'s own $(a,b,c,d)$ Baxter coefficients, rather than continuing
to compare only the final contact value — that would localize the
divergence to a specific intermediate quantity instead of the whole pipeline
at once.

## 7e. Task 10 diagnostic (cont'd): the gap is structural, not a wrong root

> **SUPERSEDED by §7h.** The conclusion drawn below — that the closure
> solution branch must be wrong — was incorrect. The closure was right; the
> contact-value formula used to compute the $g$ column of the table below was
> the bug. Kept for the record of what was ruled out (root multiplicity,
> root selection), which remains valid.

Tested directly whether *any* value of $a$ makes this spec's own
`contact_value()` (fed self-consistent $\hat B, A, \beta$ computed from that
trial $a$ via the already-verified eq. 23-24/27-33 machinery) match the
reference, for $(Z,K,\varphi)=(6,1,0.1)$, reference $g=2.13166$:

| $a$ | $g$(contact) from this $a$ | closure residual at this $a$ |
|---|---|---|
| $-0.971$ (the solver's own root) | 1.207 | $\sim10^{-16}$ |
| $6.0$ | 1.857 | 8.86 |
| $8.0$ | 2.098 | 12.21 |
| $8.2$ | $\approx2.132$ (matches reference) | $\approx12.6$ |
| $-14.24$ (the other root) | $-1.155$ (unphysical) | $\sim10^{-16}$ |

The scan is monotonic and smooth throughout — there is no zero-crossing of
the closure residual anywhere near $a\approx8.2$; the residual there is
$\sim12.6$, not a near-miss. So this is **not** a root-selection bug (no
"right" root is being missed nearby) — the value of $a$ that would make the
(already page-image-verified) contact/assembly formulas reproduce the
correct physics simply isn't a solution of this spec's primary closure
equation at all.

That means one of two things: either the primary closure equation's
solution branch is wrong by a large, non-subtle margin (not a sign flip or
index swap of the kind already found and fixed — something more
fundamental), or the contact-value/$A$/$\beta$ assembly has a substantial
error that the earlier $\hat B$-vs-$\mathcal B$ test (§7c, which only found a
<1% effect) did not probe. Every individual equation in the chain has now
been checked character-by-character against the [BH02] page images and
transcribes exactly, so continuing to re-read the same equations is
unlikely to find this on its own — this is the point flagged as needing
human derivation/verification per AI_USAGE.md, rather than further
unilateral guessing.

**Options for the next step**, none clearly better without your input:
(a) hand-derive the $N{=}1,M{=}1$ closure from Baxter's original 1968
one-Yukawa solution independently (bypassing [BH02]/[BA] entirely) as a
from-scratch cross-check; (b) work through eq. 54 (the *original*,
unsimplified closure, before the algebra that produces eq. 55) by hand
together, since an error could be in the eq.54$\to$55 algebraic reduction
itself rather than in any single displayed equation; (c) attempt the
Baxter $Q(r)$-vs-$(a,b,c,d)$ comparison after all, accepting the risk of
reverse-engineering undocumented C conventions (a first check already
confirms $|a_{\rm Clib}|$ matches the standard PY value
$(1+2\eta)/(1-\eta)^2$ at $K\to0$, but $b_{\rm Clib}$ did not match the
formula recalled from memory for the standard Baxter $b$ coefficient, which
was not independently re-derived before rejecting it).

## 7f. Task 10 diagnostic (cont'd): Baxter (1968)/Wertheim (1963) coefficient check

> **RESOLVED in §7h.** The $A=2\pi a$ half of the mapping was right; the
> $b$-coefficient mismatch was indeed the C library's $K\to0$ root selection,
> as this section suspected. The corrected $\beta$ relation is
> $\beta=\pi a+2\pi b$ (not $\beta^0-A^0/2$ rescaled), and it matches exactly.

Per option (c) above, read the two primary sources directly (not from memory)
to independently confirm the standard Percus-Yevick hard-sphere Baxter
coefficients, rather than trust a recalled formula: Baxter, J. Chem. Phys.
49, 2770 (1968), and Wertheim, Phys. Rev. Lett. 10, 321 (1963) — both
supplied by you in `4Claude/`. Both give the same hard-sphere-limit
factorization coefficients for $-C(x)=\alpha+\beta x+\gamma x^2+\delta x^3$
(Baxter's $\mu=0$ limit of his eq. 17-18; Wertheim's eq. 5), confirming the
textbook formula from memory was right for *that* representation. Neither
paper directly states the two-parameter linear form
$Q(r)=\tfrac12 a(r^2-1)+b(r-1)$ that `liboneyukawa`'s own `(a,b,c,d)` naming
suggests, so that mapping had to be derived by hand from this spec's own
already-verified eq. 18 [BH02], specialized to $N{=}1,K{=}0$: it reduces to
$q_{ij}^0(r)=(r-1)[(A^0/2)r+\beta^0]$, i.e. $a=A^0$, $b=\beta^0-A^0/2$.

Numerically, $a_{\rm Clib}=-A^0/(2\pi)$ holds cleanly (confirms
$a_{\rm Clib}^2=\alpha_{\rm Baxter}=(1+2\eta)^2/(1-\eta)^4$ to 5-6 sig figs),
but the same $-1/(2\pi)$ rescaling does **not** reproduce $b_{\rm Clib}$
(predicted $\approx0.185$ vs. actual $\approx0.679$ at $\varphi=0.1$).
Suspected (not confirmed) cause: `Y_SolveEquations`'s own $(c,d)$
coefficients stay large ($\approx-141,150$) even at $K=10^{-6}$, instead of
vanishing as a true $K\to0$ hard-sphere limit should — i.e. an
ill-conditioning artifact of the C library's own solver near $K=0$, not
necessarily a sign of anything wrong on this spec's side. This mapping
route was set aside as unreliable to keep pursuing on its own (script:
`a_scan_and_baxter_id_diagnostics.py`, Part 2).

## 7g. Task 10 diagnostic (cont'd): direct evaluation of the ORIGINAL closure (eq. 54)

> **SUPERSEDED by §7h — this section's conclusion was wrong.** The nonzero
> eq. 54 residual reported below came from an error in my own by-hand
> elimination of $C_{ij}^{(n)}$, not from the solver. Eq. 68 [BH02] — the
> paper's own headline equation, derived by an independent route — is
> satisfied by the same numbers to machine precision. The label "decisive"
> has been removed from the heading accordingly.

Per your instruction, worked through eq. 54 [BH02] itself — the closure
condition *before* the algebra that reduces it to eq. 55 — rather than
continuing to inspect eq. 55 in isolation (already confirmed, in §7d, to
transcribe exactly against its own page image and to be satisfied to
$\sim10^{-16}$ by the solver's converged $a,\hat B$).

For $N{=}1,M{=}1$ every sum collapses to one term and $\lambda_{11}=0$, so
eq. 54 becomes explicit once $D^{(1)}$, $C^{(1)}$ and
$\tilde q(\mathrm{i}z)$ are known:

- $D^{(1)}=-\delta a\,e^{z\sigma}$ (eq. 22).
- $\hat B(z)=2\pi\rho\delta\,\tilde g(z)\,e^{z\sigma}$ (eq. 43, $N{=}1$) $\Rightarrow \tilde g(z)=\hat B(z)e^{-z\sigma}/(2\pi\rho\delta)$ — this lets $\tilde g(z)$, which eq. 21 needs, be eliminated in favor of the already-independently-verified $\hat B$ (from the separate eq. 62-64 self-consistency loop) instead of the pair correlation function itself.
- Eq. 21 ($C^{(1)}+D^{(1)}=\frac{2\pi}{z}\rho\tilde g(z)D^{(1)}$) then gives $C^{(1)}=-a\hat B/z+\delta a\,e^{z\sigma}$ directly, with no circularity.
- $\tilde q(\mathrm{i}z)$ from eq. 53 ($N{=}1,\lambda{=}0$, single $m{=}n$ term, $z_m/(s{+}z_m)=1/2$ at $s=z$):
  $\tilde q(\mathrm{i}z)=\sigma^3\psi_1(z\sigma)A+\sigma^2\phi_1(z\sigma)\beta+\frac{1}{2z}\big[(C{+}D)-Ce^{-z\sigma}-z\sigma\phi_0(z\sigma)Ce^{-z\sigma}\big]$.
- Eq. 54 itself ($N{=}1$, $\delta_{\ell j}$ trivial): $2\pi K\delta^2/z \stackrel{?}{=} D^{(1)}[1-\rho\,\tilde q(\mathrm{i}z)]$.

As an internal consistency check (independent of whether the eq. 43$\leftrightarrow$eq. 21 identification above is the intended reading), $\tilde q(\mathrm{i}z)$ computed via eq. 53 was cross-checked against solving eq. 51 directly for $\tilde q(\mathrm{i}z)$ using the same $\tilde g(z)$ and eq. 52's $\tilde q^{0\prime}(\mathrm{i}z)$: these two independent routes agree to $\sim10^{-16}$–$10^{-19}$, so the eq. 51/52/53 transcription is at least self-consistent (script: `eq54_direct_check.py`).

**Result, using the solver's own converged $a,A,\beta,\hat B$ (the same numbers that satisfy eq. 55 to $\sim10^{-16}$):**

| $(Z,K,\varphi)$ | eq. 55 residual | eq. 54 lhs | eq. 54 rhs | eq. 54 residual |
|---|---|---|---|---|
| $(6,\,1,\,0.1)$ | $-6.9\times10^{-17}$ | $1.0472$ | $422.47$ | $-421.4$ |
| $(6,\,0.5,\,0.1)$ | $-4.7\times10^{-17}$ | $0.5236$ | $211.23$ | $-210.7$ |
| $(2,\,0.3,\,0.1)$ | $9.7\times10^{-17}$ | $0.9425$ | $6.964$ | $-6.02$ |
| $(2.5,\,0.5,\,0.1)$ | $-5.6\times10^{-17}$ | $1.2566$ | $15.31$ | $-14.05$ |

**Eq. 54 is not satisfied — by a large margin, the same order of magnitude
as the "structural gap" already found in §7e** — even though eq. 55 (its
claimed simplification) is satisfied to machine precision by the same
numbers, and even though every equation used to evaluate eq. 54 (eq.
21/22/43/51/52/53) individually transcribes exactly against the page
images. This localizes the discrepancy specifically to the step connecting
eq. 54 to eq. 55 (the algebra "using the results of the last section" that
the paper does not show in full) rather than to any single displayed
equation, the contact-value formula (§7c), or root selection (§7d/7e) — all
of which are now separately ruled out. The most likely explanations, in
order of suspicion: (1) an error in how $C_{ij}^{(n)}$ is meant to be
eliminated in the true 54→55 reduction — the identification used here
(via eq. 43, i.e. treating $\tilde g_{ik}^{(n)}$ in eq. 21 as $\tilde
g_{ik}(z_n)$ in the sense of eq. 42/43) is a reasonable reading but is an
*inference*, not a step shown explicitly on any page read so far; (2) a
genuine transcription gap in this spec's eq. 55/eq. 34-39 relative to what
BH02 actually derived, not caught by comparing eq. 55 alone to its own
image since the error would be in *how the terms in eq. 55 map back to
eq. 54's original quantities*, not in eq. 55's displayed symbols
themselves. This needs your judgement on whether the eq. 43 identification
is the one BH02 intend, or whether there is missing text (e.g. an
un-photographed page) defining $C_{ij}^{(n)}$ or $\tilde g_{ik}^{(n)}$
explicitly elsewhere.

## 7h. Task 10 RESOLVED: the bug was the contact-value formula, not the closure

Reading the **full** [BH02] paper (13 pages) rather than only the page images
we had been working from settled this. Two things came out of it.

### The closure was right all along

Page 11939-11940 contains eq. 60-71, a derivation of the closure by a route
entirely independent of eq. 54/55: combine eq. 5 with eq. 51, then run the
$\hat B$ self-consistency chain. Its endpoint, **eq. 68**, is the equation
printed verbatim in the paper's own abstract as *the* general solution.
Evaluated on this spec's own converged $(a,A,\beta,\hat B)$:

| $(Z,K,\varphi)$ | eq. 55 residual | eq. 68 residual |
|---|---|---|
| $(6,\,1,\,0.1)$ | $-6.9\times10^{-17}$ | $0.0$ |
| $(6,\,0.5,\,0.1)$ | $-4.7\times10^{-17}$ | $5.6\times10^{-17}$ |
| $(2,\,0.3,\,0.1)$ | $9.7\times10^{-17}$ | $5.6\times10^{-17}$ |
| $(2.5,\,0.5,\,0.1)$ | $-5.6\times10^{-17}$ | $5.6\times10^{-17}$ |

Also checked the $\Gamma/\Lambda/\Upsilon$ scaling-matrix machinery of §4
(eq. 72-87), which is the architecture the abstract describes and which this
spec had never imposed: at $N{=}1,M{=}1$, eq. 82 ($\tilde M\Lambda=\Gamma$)
reduces to $\Pi=-\tfrac12\rho aX^2$ and holds to $10^{-16}$-$10^{-15}$.
So the closure solution satisfies both the coded equation and the paper's
headline equation, reached by a separate derivation. **§7g's conclusion was
wrong**: the eq. 54 residual came from an error in my own by-hand
$C_{ij}^{(n)}$ elimination for eq. 54, not from the solver. Script:
`eq68_check.py`, `eq82_scaling_check.py`.

### The `liboneyukawa` convention maps exactly onto [BH02]

Read the C source's own closure equations (`2Y_OneYukawa.c`:
`Y_LinearEquation_1/2/3`, `Y_NonlinearEquation`, `Y_sigma`, `Y_tau`, `Y_q`,
`Y_g`) instead of guessing. With $12\varphi \equiv 2\pi\rho$ exactly
($\rho=6\varphi/\pi$):

- $\tilde q(\mathrm{i}s)=2\pi\,\texttt{Y\_q}(s)$, and $\texttt{Y\_g}(s)=\tilde g(s)$ **exactly** — the C library's `Y_g` *is* $\mathcal L[r g(r)]$.
- eq. 43 $\Rightarrow \hat B = 12\varphi e^{Z}\texttt{Y\_g}(Z)$; the C source's own $v=24\varphi K e^{Z}\texttt{Y\_g}(Z)$ is therefore $v=2K\hat B$.
- Matching eq. 52 against $s\,\texttt{Y\_tau}(s)$ term by term in $1/s^2,1/s,1/(s{+}Z)$:
  $$A=2\pi a,\qquad \beta=\pi a+2\pi b,\qquad C^{(1)}=2\pi c,\qquad D^{(1)}=2\pi d$$
  Under this map, eq. 21 becomes *identical* to the C source's own
  `Y_NonlinearEquation` — a structural confirmation, not a fit.

Validated in the exact hard-sphere limit (solving the C source's *own* linear
equations at $c=d=0$, bypassing `Y_SolveEquations`, which is ill-conditioned
near $K=0$): $2\pi a$ and $\pi a+2\pi b$ reproduce $A^0_j$ and $\beta^0_j$
to **all displayed digits** for $\varphi\in[0.05,0.4]$. This also disposes of
§7f's "$b$ mismatch": the culprit was the C library's quartic solver
returning a wrong root at $K=10^{-6}$ ($b\approx0.679$ instead of the correct
$-0.185$), exactly as suspected there — the mapping was fine.

Then, at $K\neq0$: converting the C library's solution to $(a_{\rm BH},\hat B)$
via $a_{\rm BH}=-2\pi d\,e^{-Z}$ and $\hat B=12\varphi e^{Z}\texttt{Y\_g}(Z)$
and solving this spec's own closure residual for the $K$ that zeroes it gives
$K_{\rm recovered}/K_{\rm C\text{-}lib} = 1.000000$ in every case — and the
recovered $a_{\rm BH},\hat B$ equal this spec's solved values to all digits.
**There is no convention mismatch, and the two solvers agree exactly.**
Script: `convention_map_check.py`.

### The actual bug

That left the contact-value assembly as the only remaining candidate — and
feeding the C library's *own* coefficients through it reproduced this spec's
wrong answer, confirming it. The correct formula follows from the large-$s$
asymptotics of the C library's own $\tilde g$:
$s\,\texttt{Y\_tau}(s)=a/s^2+(a{+}b)/s-cZe^{-Z}/(s{+}Z)$ and
$\texttt{Y\_q}(s)\to0$, so
$\sigma g(\sigma^+)=\lim_{s\to\infty}s e^{s\sigma}\tilde g(s)=(a+b)-cZe^{-Z}$
(exact, every term accounted for). Mapping back:

$$\boxed{2\pi\sigma_{ij}\,g_{ij}(\sigma_{ij})=A_j\frac{\sigma_i}{2}+\beta_j-\sum_m z_m C_{ij}^{(m)}e^{-z_m\sigma_{ij}}}$$

— the same structure as eq. 20 [BH02] **but with $C$ only, not $(C+D)$.**
That is consistent rather than a contradiction: eq. 20 is
$q'_{ij}(\sigma_{ji})$ for the *full* factor function
$q=q^0+\sum_n D e^{-z_n r}$ (eq. 17), whereas the contact value picks up only
the core-region part $q^{0\prime}$ (eq. 18). The formula previously coded,
eq. 26 [BA] with $-(z\delta_i-\hat B_i)a_j e^{-z\sigma_{ij}}$, is a third form
again and matches neither — its $-z a_j e^{-z\sigma_{ij}}$ carries a spurious
$e^{-z\sigma_{ij}}$, which at large $z$ silently deletes nearly the whole
term. That is precisely the size and sign of the long-running discrepancy,
and why §7c's $\hat B$-vs-$\mathcal B$ test saw only a $<1\%$ effect: both
variants it compared shared this same error.

Eliminating $C_{ij}$ via eq. 21 + eq. 22 (using $\tilde g$'s symmetry, valid
for $\delta_i=1$: common coupling, arbitrary size polydispersity) gives
$z C_{ij}e^{-z\sigma_{ij}} = z a_j - a_j\hat B_i e^{-z\sigma_i}$ and hence the
implementable form

$$2\pi\sigma_{ij}\,g_{ij}(\sigma_{ij}) = A_j\frac{\sigma_i}{2}+\beta_j - z\,a_j + a_j\hat B_i e^{-z\sigma_i}$$

### Validation of the fix

| check | result |
|---|---|
| vs `liboneyukawa` exact $(a{+}b)-cZe^{-Z}$, 10 cases: $Z\in[0.5,10]$, $K\in[0.1,3]$, $\varphi\in[0.05,0.35]$ | agrees to $10^{-14}$-$10^{-16}$ rel (worst $6.4\times10^{-14}$) |
| $K\to0$, $N=1$, $\varphi\in[0.05,0.4]$ | exact PY $(1+\eta/2)/(1-\eta)^2$ to $\sim10^{-11}$ |
| $K\to0$, $N=2$ binary mixtures | exact Lebowitz values to $\sim10^{-10}$ |
| symmetry $g_{ij}=g_{ji}$ (eq. 77 [BH02]) at $N=2$, $K\neq0$ | holds to $\sim10^{-12}$, **not imposed** |

The symmetry check is worth noting: the corrected formula looks asymmetric in
$i,j$, and comes out symmetric anyway — an independent confirmation, since
nothing in the derivation enforced it.

`polydisperse_yukawa_msa.py`'s `contact_value()` is updated accordingly, with
the derivation in its docstring and a guard raising `NotImplementedError` for
$\delta_i\neq1$ (see Open items). Scripts: `contact_from_clib_gtilde.py`,
`contact_formula_fix.py`, `contact_exact_verify.py`.

### Process note

Two of my earlier conclusions in this section were wrong and were corrected
by reading further rather than by re-reading the same pages: §7e/7g's "the
closure is structurally broken" (it was not) and §7f's suspicion that the
$b$-coefficient mismatch might indicate a derivation error (it was the C
library's root selection at $K\to0$, as originally suspected). In both cases
the error was over-trusting a by-hand derivation of an equation the paper
does not display, and then treating its failure as evidence about the code.
**Verification status (AI_USAGE.md): the corrected contact-value formula in
this section has been independently verified by J. Kohlbrecher (2026-08-28).**
It therefore counts as author-verified science, not AI scaffolding, and the
$M{=}1$ contact-value result can be built on. The numerical evidence above
(machine-precision agreement with the reference implementation, exact PY and
Lebowitz limits, unimposed $g_{ij}=g_{ji}$ symmetry) stands as corroboration
of that verification rather than as a substitute for it.

## 7i. Item 2 DONE (N=1): the general $S_{ij}(Q)$ assembly

### Result

$$\boxed{\;m_{ij}(Q)=\delta_{ij}-\sqrt{\rho_i\rho_j}\,\tilde q_{ij}(\mathrm{i}s)\big|_{s=-\mathrm{i}Q},\qquad \mathbf S^{\rm AL}(Q)=\big[\,m(Q)\,m(Q)^\dagger\,\big]^{-1}\;}$$

for the Ashcroft-Landreth partials $S_{ij}=\delta_{ij}+\sqrt{\rho_i\rho_j}\,\tilde h_{ij}(Q)$,
with $\tilde q_{ij}$ from [BH02] eq. 53 and, for $M{=}1$, $\delta_i=1$:

$$\tilde q_{ij}(\mathrm{i}s)=e^{-s\lambda_{ji}}\Big[\sigma_i^3\psi_1(s\sigma_i)A_j+\sigma_i^2\phi_1(s\sigma_i)\beta_j+\tfrac{1}{s+z}\big((C_{ij}{+}D_{ij})e^{-z\lambda_{ji}}-C_{ij}e^{-z\sigma_{ij}}-z\sigma_i\phi_0(s\sigma_i)C_{ij}e^{-z\sigma_{ij}}\big)\Big]$$

$$D_{ij}=-a_je^{z\sigma_{ij}},\qquad C_{ij}=-a_je^{z\sigma_j/2}\Big[\tfrac{\hat B_ie^{-z\sigma_i/2}}{z}-e^{z\sigma_i/2}\Big]$$

($C_{ij}$ eliminated from eq. 21/22 exactly as in §7h; $\lambda_{ji}=\tfrac12(\sigma_j-\sigma_i)$.)
Conjugating the Baxter-Wertheim factorization (eq. 7 + eq. 12) by
$\mathbf D=\mathrm{diag}(\sqrt{\rho_i})$ turns it into the manifestly Hermitian
$m\,m^\dagger$ form above, so $\mathbf S^{\rm AL}$ comes out real symmetric and
positive definite automatically — those are *consequences* here, hence usable
as checks rather than things to impose. Since $q(r)$ is real, evaluating at
$s=+\mathrm iQ$ merely conjugates $m$ and leaves $\mathbf S^{\rm AL}$ unchanged,
so no separate $-Q$ evaluation is needed. Implemented in
`polydisperse_yukawa_sq.py`; validation in `sq_validation.py`.

### Route not taken, and why

[BA] eq. 28-31 give what looks like a more convenient route: $\mathbf
M_{ij}(s)=\delta_{ij}-\rho_i\tilde q_{ij}(\mathrm is)$ as a sum of $M{+}2$
rank-one terms, with eq. 34 collapsing $\det\mathbf M$ to an
$(M{+}2)\times(M{+}2)$ determinant $D_\tau$. Transcribed and tested, this
reproduced $\det\mathbf M\equiv D_\tau$ exactly (so the rank-one vectors are at
least self-consistent), but the resulting $\mathbf M$ was wrong by a large,
$s$-dependent factor (6.4 at $s{=}0.5$ rising to 284 at $s{=}10$), and eq. 33's
$2\pi\tilde g=-\tilde\mu/D_\tau$ then missed [the exactly-known] $\tilde g$ by
the same factor. The [BH02] eq. 52/53 route, by contrast, reproduces
`liboneyukawa`'s $\tilde g\equiv$`Y_g` to **all 8 displayed digits at every
$s$ tested**, so it was used instead. Whether the fault is a misreading of
[BA] eq. 29's exponential prefactors (a term-by-term comparison against eq. 52
shows [BA]'s third term carrying $e^{-s\sigma_i/2}$ where eq. 52 has
$e^{-s\sigma_{ij}}$) or a typo in that equation is **not resolved** — the
route was simply abandoned once a validated alternative existed. Diagnostic:
`BA_vs_BH02_localize.py`, `M_matrix_diagnostic.py`.

### Latent bug found and fixed

Testing at $Q\to0$ exposed that two of the three auxiliary functions in
`polydisperse_yukawa_msa.py` had **series branches contradicting their own
closed forms** — $\phi_1$'s had the wrong *sign* ($+1/2$ vs the correct $-1/2$
at $x{=}0$) and $\psi_1$'s was unrelated to the correct $-1/12$. Both were
dormant (their thresholds, $|x|<10^{-6}$ and $10^{-5}$, are never reached for
physical $z\sigma$), **so no previously reported result is affected**, but the
$S(Q)$ assembly evaluates the same functions at $s=-\mathrm iQ$ where small
$|x|$ *is* reached. Both are corrected, all three are now complex-safe, and
the switch-over moved to $|x|=0.5$ with 16 series terms — also fixing a
precision hole where $\psi_1$'s closed form (numerator $O(x^3/12)$) is
worthless below $|x|\sim10^{-4}$. They now satisfy eq. 45's identity
$\phi_1=x\psi_1-\phi_0/2$ to machine precision across the switch-over.

### Validation

| check | result |
|---|---|
| $N{=}1$ vs `SqOneYukawa`/`Y_pc`, 10 parameter sets $\times$ 14 $Q\in[0.1,400]$ | **$\le2.7\times10^{-10}$** rel |
| $N{=}1$, $Q\to0$ vs the C library's exact `q==0` branch | $10^{-14}$-$10^{-16}$ rel |
| monodisperse collapse: $N{=}3$, all $\sigma_i$ equal, vs $N{=}1$ | $\sim10^{-16}$ |
| $\mathbf S=\mathbf S^{T}$ (not imposed), $N{=}2,3$ | $\le7\times10^{-18}$ |
| $\mathbf S$ positive definite, $N{=}2,3$ | min eigenvalue $\approx0.35$-$0.38$ |
| $\mathbf S(Q{\to}\infty)\to\mathbf I$ | $\sim10^{-7}$ at $Q{=}4000$ |
| $N{>}1$ large-$Q$ tail vs verified contact values, **diagonal** $S_{ii}$ | $\sim10^{-6}$ (extraction noise floor) |
| $N{>}1$ large-$Q$ tail, **off-diagonal** $S_{i\neq j}$ | **0.05-2.5%** — see below |

Between $Q=0.05$ and $0.1$ the reference itself degrades (its `Y_pc` carries
$1/q^6$ terms that cancel catastrophically); restricting to $Q\ge0.1$ is what
takes the agreement from $\sim10^{-8}$ to $\sim10^{-10}$, and the $Q\to0$ row
above compares against the C library's own exact small-$q$ branch instead.

### The one real gap: $N>1$ violates the eq. 72 scaling condition

The off-diagonal tail discrepancy is **not** in the $S(Q)$ assembly. [BH02]
eq. 72 requires $\Pi_i^{(n)}=-\sum_m\Gamma_{nm}X_i^{(m)}$, i.e. at $M{=}1$ that
$\Gamma=-\Pi_i/X_i$ be **species-independent** — $N{-}1$ constraints that this
spec's closure (§5) never imposes. Our solutions violate it, and the violation
predicts the error:

| $\sigma_2/\sigma_1$ | $\Gamma$ rel. spread | off-diag tail err | diag tail err |
|---|---|---|---|
| 1.00 | $4.9\times10^{-16}$ | $2.5\times10^{-6}$ | $2.5\times10^{-6}$ |
| 1.15 | $1.4\times10^{-2}$ | $7.9\times10^{-4}$ | $2.8\times10^{-6}$ |
| 1.60 | $6.7\times10^{-2}$ | $1.5\times10^{-3}$ | $7.5\times10^{-6}$ |
| 2.50 | $2.95\times10^{-1}$ | $1.0\times10^{-2}$ | $5.0\times10^{-6}$ |

At zero polydispersity $\Gamma$'s spread is exactly zero and the off-diagonal
error drops to the diagonal's noise floor; thereafter the two rise together
while the diagonal stays flat. The same defect shows up independently in
eq. 100 (§7j), which is satisfied *exactly* at $N{=}1$ and misses by
$10^{-3}$-$8\times10^{-3}$ at $N{=}2,3$. So $N{=}1$ is complete and validated
to machine precision; **$N>1$ needs the §4 symmetry/scaling conditions
(eq. 72-87) actually imposed on the closure solve** — that is now the top open
item, and it is a closure problem, not an assembly problem.

## 7j. eq. 100 [BH02]: an independent confirmation of §7h

Page 11943 (not previously read) carries a second, independent contact-value
formula,
$2\pi\sigma_{ij}[g_{ij}(\sigma_{ij})-g^0_{ij}(\sigma_{ij})]=-(z+\Gamma)X_ia_j$
with $g^0$ the hard-sphere mixture contact value. At $N{=}1$ it reproduces the
author-verified §7h formula with **relative error exactly 0.0** — a genuinely
independent check, since it routes through $\Gamma$ and $X_i$ rather than
through $A,\beta,C$. Its companion eq. 100b,
$2\pi K=-2\Gamma(z+\Gamma)/D_2$ with $D_2=\sum_k\rho_kX_k^2$ (eq. 101), holds
to $10^{-14}$-$10^{-16}$ at $N{=}1$. (Two helper identities quoted mid-derivation
on that page, $\sum_k\rho_ka_k^2=4\pi K$ and
$\tfrac1\pi\sum_k\rho_ka_kA_k^0=z\pi K\Delta_n/\Delta$, do **not** hold in our
variables and are presumed to use a different normalization of $a_k$ local to
that section; they are not needed anywhere.) Script: `eq100_gamma_check.py`.

## 7k. N>1 RESOLVED: eq. 35 and eq. 38 are misprinted

### What the fix actually was

**Not** what §7i predicted. The hypothesis there — that the $a_j$ are
over-determined, and that at $M{=}1$ the true unknown is the scalar $\Gamma$
with $a_j=2\Gamma X_j/D_2$ derived from it — was **wrong**. Implementing it
made things *differently* broken: eq. 72 became exact by construction, but
eq. 55, eq. 68 and the physical symmetry $g_{ij}=g_{ji}$ all broke (the last
from $10^{-12}$ to $10^{-2}$). Trading one violated condition for another is
the signature of an inconsistent *ingredient*, not a miscounted unknown — and
that is what pointed at the real bug.

**$\hat{\mathcal I}$ (eq. 35) and $\hat{\mathcal J}$ (eq. 38) are both
misprinted in [BH02]**, each with a $j\leftrightarrow\ell$ interchange that is
invisible at $N{=}1$ (where $j=\ell$ always) and wrong for every $N>1$.

### How that was established, without fitting

[BH02] eq. 30, 32, 33 give $\Delta^{(n)}$, $X_i$, $\Pi_j$ **explicitly in
$\hat B$**, touching neither matrix:

$$\Delta^{(n)}=-\tfrac{2\pi}{z^2\Delta}\textstyle\sum_\ell\rho_\ell(1{+}\tfrac{z\sigma_\ell}{2})\delta_\ell-\tfrac{2\pi}{\Delta}\sum_\ell\rho_\ell\sigma_\ell^3\psi_1(z\sigma_\ell)\hat B_\ell$$
$$X_i=\delta_i+\sigma_i\phi_0(z\sigma_i)\hat B_i+\sigma_i\Delta^{(n)},\qquad \Pi_j=\hat B_j+(1{+}\tfrac{\sigma_jz}{2})\Delta^{(n)}+\tfrac{\sigma_j}{2}\textstyle\sum_\ell\rho_\ell\beta^0_\ell X_\ell$$

Substituting eq. 30 into eq. 33 and matching eq. 37 gives

$$\hat{\mathcal J}_{j\ell}=\delta_{j\ell}\sigma_j\phi_0(z\sigma_j)-2\rho_\ell\beta^0_{\;j}\,\sigma_\ell^3\psi_1(z\sigma_\ell)$$

against eq. 38's printed $-2\rho_\ell\beta^0_{\;\ell}\sigma_j^3\psi_1(z\sigma_j)$:
$j$ and $\ell$ swapped. The decisive point is that **the same substitution
reproduces eq. 39's $\hat\gamma_j$ exactly as printed**, and eq. 36's
$\hat\xi_j$ likewise comes out exact — so the two *vectors* are right and only
the two *matrices* carry the interchange. That is corroboration from the
paper's own equations, not a fit to our numerics. Independently, eq. 27 and
eq. 28 agree on $P^{(n)}$ to machine precision at all $N$ (eq. 31 does not,
even at $N{=}1$, so eq. 31 is also suspect and is not used).

Numerically, with arbitrary random $\hat B$: the derived $\hat{\mathcal J}$
matches eq. 30/32/33 to $\le3\times10^{-16}$ at $N=1,2,3$, while eq. 38 as
printed misses by $10^{-3}$-$5\times10^{-3}$; eq. 35 as printed misses by
$2\times10^{-2}$-$6\times10^{-2}$; eq. 36 and eq. 39 are exact throughout.

`polydisperse_yukawa_msa.py` therefore builds $\hat{\mathcal I},\hat{\mathcal
J},\hat\xi,\hat\gamma$ by **exactly probing the affine map
$\hat B\mapsto(\Pi,X)$ of eq. 30/32/33** — transcription-free, so no printed
matrix is relied on at all. The as-printed versions are retained as
`Ihat_printed`/`Jhat_printed`. Note this **supersedes the §7d
$\hat{\mathcal I}/\hat{\mathcal J}$ "fix"**, which rearranged indices within
the printed (wrong) form; it was never testable at $N{=}1$ and is now moot.

### Everything at $N>1$ now closes at machine precision

$N{=}2$ ($\sigma{=}[1,2]$), $N{=}2$ ($[1,1.5]$), $N{=}3$ ($[1,1.5,2.5]$):

| condition | before | after |
|---|---|---|
| eq. 72 $\Gamma$ species-independence | $7.7\times10^{-3}$-$9.2\times10^{-2}$ | $\le1.5\times10^{-16}$ |
| eq. 55 componentwise closure | $10^{-16}$ | $\le2.2\times10^{-15}$ |
| eq. 68 contracted closure | $10^{-17}$ | $\le5.6\times10^{-16}$ |
| eq. 100 contact value | $1.7\times10^{-3}$-$1.0\times10^{-2}$ | $\le2.4\times10^{-16}$ |
| $g_{ij}=g_{ji}$ (eq. 77, not imposed) | — | $\le6.7\times10^{-16}$ |
| off-diagonal $S_{i\neq j}$ large-$Q$ tail | $7\times10^{-3}$-$2.5\times10^{-2}$ | $\sim10^{-6}$ (= diagonal noise floor) |

All conditions hold **simultaneously** now, which is exactly what was
impossible before. The polydispersity scan that originally exposed the defect
is now flat across the whole range:

| $\sigma_2/\sigma_1$ | 1.00 | 1.15 | 1.60 | 2.00 | 2.50 | 3.50 |
|---|---|---|---|---|---|---|
| $\Gamma$ rel. spread (was $\to0.30$) | 0 | $3.7\times10^{-16}$ | $1.3\times10^{-16}$ | $2.3\times10^{-15}$ | $5.4\times10^{-16}$ | $6.2\times10^{-15}$ |
| off-diag tail err (was $\to1.0\times10^{-2}$) | $2.5\times10^{-6}$ | $3.6\times10^{-6}$ | $4.4\times10^{-6}$ | $8.7\times10^{-7}$ | $1.6\times10^{-6}$ | $1.2\times10^{-6}$ |

Regressions all hold: $N{=}1$ $S(Q)$ vs `liboneyukawa` $\le2.7\times10^{-10}$,
$N{=}1$ contact value $\le6.4\times10^{-14}$, $K\to0$ PY $\le8\times10^{-11}$
and $N{=}2,3$ Lebowitz $\le8.6\times10^{-11}$.

### Two solvers, kept deliberately

With the matrices corrected, `solve()` (the $N$ amplitudes $a_j$ from eq. 55
componentwise) and `solve_gamma()` (the scalar $\Gamma$ plus $\hat B_i$ from
eq. 72 + eq. 100b, with $a_j=2\Gamma X_j/D_2$) return **identical** results —
so eq. 55 and eq. 72 were never in conflict; the printed matrices merely made
them look it. `solve_gamma()` is retained as an independent cross-check: it
carries a different unknown structure and a 1-D root find (Γ is scalar at
$M{=}1$, since eq. 72 is linear in $\hat B$ at fixed Γ), so agreement between
the two is a real check rather than a tautology. It also handles $K=0$
exactly, closing open item 5.

### Verification status

**Per AI_USAGE.md: the $\hat{\mathcal J}$ derivation above, and the conclusion
that [BH02] eq. 35 and eq. 38 are misprinted, have been independently verified
by J. Kohlbrecher (2026-08-29)** and therefore count as author-verified
science. Supporting evidence, for the record: eq. 36/39 and eq. 27/28 fall out
exactly right from the same reading, and six independent conditions go from
mutually unsatisfiable to simultaneously satisfied at machine precision.

The scope caveat noted here originally — that every $N>1$ check was internal
to [BH02]'s own equation set — has since been closed by the independent
numerical validation in §7l, which confirms the corrected form and rejects
eq. 35/38 as printed from the outside.

## 7l. Externally independent $N>1$ validation

Everything in §7h-7k is either internal to [BH02]'s equation set or ties
$S_{ij}(Q)$ back to our own contact values. With four misprints now found in
that paper, that is not enough to rest on. So: an independent **numerical**
multicomponent OZ solver (`mixture_oz_numerical.py`), which shares no equation
with the analytic route — it iterates the matrix OZ relation on a radial grid
under the same physical closure ([BH02] eq. 5/6) and nothing else.

$$\hat C = \mathbf D\tilde{\mathbf C}\mathbf D,\quad \hat H=(\mathbf I-\hat C)^{-1}\hat C,\quad \mathbf S=(\mathbf I-\hat C)^{-1},\qquad \mathbf D=\mathrm{diag}(\sqrt{\rho_i})$$

with Picard iteration on $\gamma=h-c$, and 3D radial transforms by DST-I
(forward/round-trip verified against the exact pair $e^{-ar}\leftrightarrow
8\pi a/(a^2+k^2)^2$ to $2.6\times10^{-8}$ / $8.4\times10^{-12}$).

**Calibrating the method.** Convergence is **first order** in the grid spacing
$dr$ — the hard-core discontinuity, not a defect — verified against the exact
$N{=}1$ answer: the error ratio between $dr=0.005$ and $0.0025$ is
$2.007, 2.003, 2.006, 1.999, 1.998$ across five $Q$. Richardson extrapolation
($p{=}1$) therefore applies and buys $\sim20\times$: extrapolated $S(Q)$ hits
$2\times10^{-7}$-$9\times10^{-5}$ and the extrapolated contact value
$1.4\times10^{-4}$, against exact references. So the method's own floor is
$\sim4\times10^{-5}$ absolute on $S$ and $\sim10^{-4}$ relative on contact
values — comfortably below the 0.3-2.5% $N>1$ effects at issue.

**The test.** Compare the numerical solution against the analytic one computed
two ways: with $\hat{\mathcal I},\hat{\mathcal J}$ rebuilt from eq. 30/32/33
(§7k's corrected form) and with eq. 35/38 **exactly as printed**.

Contact values, relative error vs numerical:

| case | $(i,j)$ | corrected | as printed |
|---|---|---|---|
| $N{=}2$, $\sigma{=}[1,2]$ | (0,0) / (0,1) / (1,1) | $1.5{\cdot}10^{-4}$ / $1.0{\cdot}10^{-4}$ / $7.5{\cdot}10^{-5}$ | $1.5{\cdot}10^{-3}$ / $6.2{\cdot}10^{-3}$ / $4.3{\cdot}10^{-3}$ |
| $N{=}3$, $\sigma{=}[1,1.5,2.5]$ | worst of six | $5.8\times10^{-5}$ | $1.5\times10^{-2}$ |

$S_{ij}(Q)$, absolute error (the AL off-diagonals pass through small values —
$S_{01}$ reaches $0.0033$ — so relative error is a misleading metric there):

| | corrected | as printed |
|---|---|---|
| $N{=}2$, $S_{00}$ | $1.9$-$4.9\times10^{-5}$ | up to $5.1\times10^{-4}$ |
| $N{=}2$, $S_{01}$ | $1.8\times10^{-6}$-$4.7\times10^{-5}$ | up to $4.0\times10^{-4}$ |
| $N{=}3$, worst rel. | $1.7\times10^{-3}$ | $1.9\times10^{-2}$ |

**The corrected form sits at the numerical method's own accuracy floor
throughout; the as-printed form is 10-450$\times$ above it.** An entirely
external method therefore confirms the §7k derivation and rejects [BH02]'s
eq. 35/38 as printed. That closes the scope caveat noted at the end of §7k:
the $N>1$ solution no longer rests on the paper's own equation set.

**Gotcha worth recording.** A first pass at this comparison showed 1-19%
errors for *both* variants. Cause was mine, not the physics: the analytic
$S(Q)$ was evaluated at the requested $Q$ while the numerical one sat at the
nearest grid $k$, differing by up to $0.07$ — and $|dS/dk|$ near the peak is
large enough for that alone to dominate. Always evaluate the analytic side at
the numerical grid's own $k$ (`external_N_validation.py` does).

Secondary benefit: this solver is the matrix generalization of the numerical
OZ route that was the project's original implementation plan, so it is
reusable beyond validation — e.g. for closures with no analytic solution, or
as a cross-check when $M>1$ is added.

## 9. References

- L. Blum, M. Arias, "Structure of multi-component/multi-Yukawa mixtures,"
  arXiv:cond-mat/0602477 (2006). Local: `0602477v2.pdf`.
- L. Blum, J. A. Hernando, "Yukawa fluids in the mean scaling approximation: I. The
  general solution," J. Phys.: Condens. Matter 14, 11933 (2002). Local:
  `L_Blum_2002_J._Phys.__Condens._Matter_14_11933.pdf`.
- L. Blum, J. S. Høye, "Solution of the Ornstein-Zernike Equation with Yukawa Closure
  for a Mixture," J. Stat. Phys. 19, 317 (1978). Local: `BF01011750.pdf` (poor OCR —
  cross-context only, not a transcription source).
- L. Blum, M. Ubriaco, "Analytical solution of the Yukawa closure of the OZ equation
  IV: the general 1-component case," Mol. Phys. 98, 829 (2000). One-component only.
  Local: `Analytical solution of the Yukawa closure of the Ornstein-Zernik equation
  IV  the general 1-component case.pdf`.
- L. Blum, M. Ubriaco, "Variational extensions of the mean spherical approximation,"
  Physica A 279, 224 (2000). One-component only. Local:
  `1-s2.0-S0378437199005348-main.pdf`.

All local files are in `FormFactors4SASfit/In Progress/4Claude/`.
