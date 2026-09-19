/*
 * src/sasfit_common/sasfit_qdht.cpp
 *
 * Copyright (c) 2008-2026, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

// sasfit_qdht(): a Quasi-Discrete Hankel Transform strategy for
// sasfit_hankel(), implementing:
//   - Yu, Guo, Huang, Yan & Wang, Opt. Lett. 23, 409 (1998) -- original
//     order-0 QDHT.
//   - Guizar-Sicairos & Gutierrez-Vega, J. Opt. Soc. Am. A 21, 53 (2004)
//     -- generalization to arbitrary integer order p, used directly here.
//   - Malinka, J. Opt. Soc. Am. A 38, 401 (2021) -- half-weighted last
//     node, giving the correct half-sum value at a function's jump
//     discontinuity at the truncation radius, used to evaluate the
//     transform at an arbitrary continuous frequency rather than only
//     the method's native (Bessel-zero-based) grid points. (Malinka's
//     second refinement, an analytic K0-based tail extrapolation for
//     functions without genuine compact support, was evaluated and
//     deliberately deferred -- it requires an additional closed-form
//     derivation and fitting step; the current generous-but-not-extreme
//     truncation already validates to 1e-2 to 1e-6 on an infinite-tail
//     test case, see below.)
//
// Reference PDFs for these papers are filed under
// FormFactors4SASfit/In Progress/HankelTransform.
//
// UNLIKE sasfit_fftlog (which wants the domain padded FAR beyond a
// function's natural extent, to avoid FFT-periodicity ringing), QDHT
// wants the truncation radius R chosen SNUGLY around the function's
// actual extent: for fixed N, a larger R spreads the same sampling
// budget more thinly and directly costs resolution in the meaningful
// region -- confirmed: R chosen ~500x too large (copying sasfit_fftlog's
// padding heuristic verbatim, before this was understood) gave >400%
// errors across every test case despite the underlying algorithm
// matching the original paper's own published accuracy (eps2=4.8e-5 at
// N=1024 on their own top-hat test case) when R is chosen appropriately.
//
// QDHT is ALSO far more sensitive to exactly where R sits relative to a
// genuine compact-support edge than sasfit_bestlime is (which tolerates
// a similar mismatch far more gracefully): being a truncated Bessel-
// function series, a poorly-placed R produces visible Gibbs-phenomenon-
// style ringing, not just reduced accuracy. Directly measured (cylinder
// cross-section test case, true edge at r=20): R placed exactly at the
// edge gives >100% error and 12 sign changes in the error across a
// dense q-scan (a ringing signature); R 10% beyond the precisely-
// bisected edge gives <1% error and only floating-point-level sign
// changes; R 25-30% beyond, or 2x beyond (matching sasfit_fftlog's
// padding philosophy), both degrade back to several-percent error with
// ringing returning. The precise bisection-refined edge (same technique
// as sasfit_bestlime.cpp) with a tuned 10% margin is used below --
// finding the edge only approximately (e.g. a coarse relative-threshold
// scan) is not sufficient here.
//
// ALSO UNLIKE sasfit_fftlog/sasfit_bestlime: QDHT's native transform
// convention has explicit 2*pi factors --
//   f2(nu) = 2*pi * int f1(r) J_p(2*pi*nu*r) r dr
// -- differing from the "r dr, no 2*pi" convention sasfit_hankel()'s
// other strategies use. Confirmed as a real, easy-to-miss bug: matching
// against exact references in the STANDARD convention while computing
// in QDHT's NATIVE convention gave >400% errors that looked identical
// in character to the R-selection bug above, and was only found by
// re-validating directly against the original paper's own worked
// example (which uses its native convention throughout, and matched to
// 1e-4 once compared correctly). The conversion used below,
// I(x) = qdht_eval(x/(2*pi)) / (2*pi), is exact, not an approximation.
//
// Validated against six diverse test cases: a smooth Gaussian (near
// machine precision), a sphere's real-space correlation function
// (compact support, mild edge singularity), a cylinder cross-section's
// correlation function (compact support, sharper branch-point edge),
// a DAB-type exponential correlation function (genuine infinite tail),
// and a spherical form factor transformed in the q->r direction
// (oscillatory input, diffraction minima, including at very small
// output r). All six validate to 1e-2 or better; most to 1e-5 or
// better, with no visible ringing (dense-scan error sign changes are
// floating-point-level only, not a real oscillatory artifact).

#include <math.h>
#include <stdexcept>
#include <vector>
#include <utility>
#include <limits>
#include <new>
#include <algorithm>
#include <cstring>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_spline.h>
extern "C"
{
    #include "sasfit_constants.h"
    #include "sasfit_function.h"
    #include "sasfit_message.h"
    #include "sasfit_hankel.h"
}

namespace {

struct QDHTPlan {
    int N = 0;
    double p = 0.0;
    double R = 0.0, V = 0.0, S = 0.0;
    std::vector<double> alpha;          // zeros 1..N+1 of J_p
    std::vector<double> Jp1;            // |J_{p+1}(alpha_n)|, n=1..N
    std::vector<double> r_nodes;
    std::vector<double> weighted_coef;  // f1(r_n)/Jp1(n)^2, half-weighted at n=N-1
};

QDHTPlan qdht_build_plan(int N, double p, double R) {
    QDHTPlan plan;
    plan.N = N;
    plan.p = p;
    plan.R = R;
    plan.alpha.resize(N + 1);
    for (int i = 0; i < N + 1; ++i) {
        plan.alpha[i] = gsl_sf_bessel_zero_Jnu(p, i + 1);
    }
    plan.S = plan.alpha[N];
    plan.V = plan.S / (2.0 * M_PI * R);
    plan.Jp1.resize(N);
    plan.r_nodes.resize(N);
    for (int n = 0; n < N; ++n) {
        plan.Jp1[n] = std::fabs(gsl_sf_bessel_Jnu(p + 1.0, plan.alpha[n]));
        plan.r_nodes[n] = plan.alpha[n] / (2.0 * M_PI * plan.V);
    }
    return plan;
}

// Precomputes the function-specific coefficients (the expensive part,
// N evaluations of f) so that many subsequent qdht_eval() calls at
// different frequencies are cheap -- same "build once, evaluate many
// q cheaply" efficiency niche as sasfit_fftlog's cached spline.
void qdht_prepare(QDHTPlan& plan, const std::vector<double>& f1_at_nodes) {
    const int N = plan.N;
    plan.weighted_coef.resize(N);
    for (int n = 0; n < N; ++n) {
        double c = f1_at_nodes[n] / (plan.Jp1[n] * plan.Jp1[n]);
        if (n == N - 1) {
            c *= 0.5; // Malinka (2021): half-weight the last node
        }
        plan.weighted_coef[n] = c;
    }
}

// Evaluates f2(nu) at an arbitrary continuous nu (QDHT's own native
// convention: f2(nu) = 2*pi * int f1(r) J_p(2*pi*nu*r) r dr), derived
// directly from the plan's already-validated forward-transform formula
// before it is specialized to the method's native discrete grid.
double qdht_eval(const QDHTPlan& plan, double nu) {
    const int N = plan.N;
    double sum = 0.0;
    const double arg_scale = 2.0 * M_PI * plan.R * nu / plan.S;
    for (int n = 0; n < N; ++n) {
        sum += plan.weighted_coef[n] * gsl_sf_bessel_Jnu(plan.p, plan.alpha[n] * arg_scale);
    }
    return sum / (M_PI * plan.V * plan.V);
}

// ---- Malinka (2021) analytic tail extrapolation ----
//
// QDHT truncates the input at R, discarding int_R^inf f1(r) J_p(x r) r dr.
// For a compactly-supported f1 that remainder is exactly zero, but for a
// power-law or exponential tail it is not, and it is precisely what biases
// the x->0 limit (which is just int f1(r) r dr) low. Malinka's refinement
// models the discarded tail as f1(r) ~ A*K0(b*r), fits (A,b) from the last
// two radial samples, and adds the resulting integral in closed form.
//
// The closed form used here was re-derived from scratch via the Bessel
// recurrences d/dr[r*K1(br)] = -b*r*K0(br) and d/dr[r*J1(pr)] = p*r*J0(pr):
//
//   int_L^inf K0(bt) J0(pt) t dt
//       = L*[b*K1(bL)*J0(pL) - p*K0(bL)*J1(pL)] / (b^2 + p^2)
//
// NOTE: the form obtained by text-extracting the published paper was
// missing the leading L on the second bracket term; that version does NOT
// reproduce direct numerical integration (errors of 60-500%), whereas the
// expression above matches it to ~1e-13. Verified independently before use.
//
// Only the order-0 case is handled (the K0 tail model is an order-0
// construction); for nu=1 the correction is skipped and the uncorrected
// value returned, which is the previous, already-validated behaviour.

// Solves f1(r1)/f1(r2) = K0(r1*b)/K0(r2*b) for b, with r1 < r2.
double qdht_solve_tail_b(double r1, double v1, double r2, double v2) {
    // K0(r1*b)/K0(r2*b) with r1 < r2 is INCREASING in b. Getting this
    // direction wrong silently returns a wildly wrong b and a correction
    // roughly 20x too small (observed during development), so the
    // bisection below is written against the verified direction.
    const double target = v1 / v2;
    auto ratio = [&](double b) {
        return gsl_sf_bessel_K0(r1 * b) / gsl_sf_bessel_K0(r2 * b);
    };
    double blo = 1e-10, bhi = 50.0 / r2;
    if (!std::isfinite(ratio(bhi)) || ratio(bhi) < target) {
        return bhi;
    }
    if (ratio(blo) > target) {
        return blo;
    }
    for (int i = 0; i < 200; ++i) {
        const double bm = 0.5 * (blo + bhi);
        if (ratio(bm) < target) {
            blo = bm;
        } else {
            bhi = bm;
        }
    }
    return 0.5 * (blo + bhi);
}

// Returns the estimated discarded tail integral in the STANDARD
// convention (int_R^inf f1(r) J_0(x r) r dr), or 0.0 when the tail does
// not look like a decaying positive function the K0 model can represent.
double qdht_tail_correction(double R, double fR, double r_prev, double f_prev, double x) {
    if (!(fR > 0.0) || !(f_prev > 0.0) || !(f_prev > fR)) {
        return 0.0;
    }
    const double b = qdht_solve_tail_b(r_prev, f_prev, R, fR);
    if (!(b > 0.0) || !std::isfinite(b)) {
        return 0.0;
    }
    const double K0bR = gsl_sf_bessel_K0(R * b);
    if (!(K0bR > 0.0) || !std::isfinite(K0bR)) {
        return 0.0;
    }
    const double A = fR / K0bR;
    const double K1bR = gsl_sf_bessel_K1(R * b);
    const double J0xR = gsl_sf_bessel_J0(x * R);
    const double J1xR = gsl_sf_bessel_J1(x * R);
    const double corr = A * R * (b * K1bR * J0xR - x * K0bR * J1xR) / (b * b + x * x);
    return std::isfinite(corr) ? corr : 0.0;
}

// ============================================================
// Self-reciprocal QDHT grid: forward -> correct -> backward
// ============================================================
//
// QDHTPlan/qdht_eval above answer "give me the transform at one
// arbitrary continuous frequency", which is the right shape for
// sasfit_qdht() as a sasfit_hankel() strategy callback, but the wrong
// shape for a forward -> apply-a-correction -> inverse round trip:
// there is no inverse there at all, and evaluating a whole corrected
// function point-by-point via qdht_eval costs O(N) per point with no
// exact-round-trip guarantee.
//
// QDHTReciprocalPlanImpl is the alternative shape for that job. Its
// kernel matrix
//
//   T[m,n] = (2/S) J_p(alpha_m alpha_n/S) / (J_{p+1}(alpha_m) J_{p+1}(alpha_n))
//
// is symmetric AND self-reciprocal (T*T = I to machine precision) once
// the samples either side of it are rescaled by JR = J_{p+1}/R going
// in and JV = J_{p+1}/k_max coming out (or the same pair the other way
// round for the inverse) -- so the SAME kernel matrix serves as both
// forward and inverse. No matrix inversion is ever needed. Verified
// independently (Python/NumPy prototype) against the exact Gaussian
// Hankel pair (1.7e-16 relative error) and against an analytic
// forward->correct->backward case -- multiplying by exp(-(k sigma)^2)
// in k-space and inverting reproduces the known closed-form Gaussian-
// convolution result to 3e-16.
//
// Forward and inverse MUST share the same (R, N, p) for the identity
// T*T=I to hold exactly -- there is no freedom to pick a different
// truncation radius for the outgoing and returning leg of a round
// trip and still get an exact identity back.
//
// Kept as an implementation detail (anonymous namespace, std::vector-
// based): sasfit_hankel.h is pulled into plain C translation units
// (sasfit.c is .c, not .cpp) via the extern "C" block above, so
// nothing using std::vector or a C++ struct can appear in that header.
// The public surface is the opaque-handle, raw-array C ABI below
// (sasfit_qdht_plan / sasfit_qdht_build_plan / ...), which wraps this.

struct QDHTReciprocalPlanImpl {
    int N = 0;
    double p = 0.0, R = 0.0, S = 0.0, k_max = 0.0;
    std::vector<double> alpha;    // N zeros of J_p
    std::vector<double> Jp1;      // |J_{p+1}(alpha_n)|
    std::vector<double> r_nodes;  // r_n = R alpha_n / S
    std::vector<double> k_nodes;  // k_n = alpha_n / R
    std::vector<double> JR, JV;   // diagonal rescalings either side of kernel
    std::vector<double> kernel;   // N*N, row-major, symmetric
};

QDHTReciprocalPlanImpl qdht_build_reciprocal_plan_impl(int N, double p, double R) {
    QDHTReciprocalPlanImpl plan;
    plan.N = N;
    plan.p = p;
    plan.R = R;

    plan.alpha.resize(N);
    for (int i = 0; i < N; ++i) {
        plan.alpha[i] = gsl_sf_bessel_zero_Jnu(p, i + 1);
    }
    // S is the (N+1)-th zero -- one beyond the grid, exactly as in
    // QDHTPlan above -- fixing both the grid spacing and the
    // reachable k_max = S/R.
    plan.S = gsl_sf_bessel_zero_Jnu(p, N + 1);
    plan.k_max = plan.S / R;

    plan.Jp1.resize(N);
    plan.r_nodes.resize(N);
    plan.k_nodes.resize(N);
    plan.JR.resize(N);
    plan.JV.resize(N);
    for (int n = 0; n < N; ++n) {
        plan.Jp1[n] = std::fabs(gsl_sf_bessel_Jnu(p + 1.0, plan.alpha[n]));
        plan.r_nodes[n] = plan.alpha[n] * R / plan.S;
        plan.k_nodes[n] = plan.alpha[n] / R;
        plan.JR[n] = plan.Jp1[n] / R;
        plan.JV[n] = plan.Jp1[n] / plan.k_max;
    }

    plan.kernel.assign(static_cast<size_t>(N) * N, 0.0);
    for (int m = 0; m < N; ++m) {
        for (int n = 0; n < N; ++n) {
            plan.kernel[static_cast<size_t>(m) * N + n] =
                2.0 * gsl_sf_bessel_Jnu(p, plan.alpha[m] * plan.alpha[n] / plan.S)
                / (plan.S * plan.Jp1[m] * plan.Jp1[n]);
        }
    }
    return plan;
}

// Shared O(N^2) matvec used by both forward and inverse below.
// Replace with cblas_dgemv here if N ever gets large enough that the
// hand-rolled loop shows up in a profile -- the kernel is a plain
// dense matrix, so BLAS should give a solid constant-factor win with
// no change to the math on either side of this call.
std::vector<double> qdht_matvec_impl(const QDHTReciprocalPlanImpl& plan,
                                      const std::vector<double>& x) {
    const int N = plan.N;
    std::vector<double> y(N, 0.0);
    for (int m = 0; m < N; ++m) {
        double s = 0.0;
        const double* row = &plan.kernel[static_cast<size_t>(m) * N];
        for (int n = 0; n < N; ++n) {
            s += row[n] * x[n];
        }
        y[m] = s;
    }
    return y;
}

std::vector<double> qdht_forward_impl(const QDHTReciprocalPlanImpl& plan,
                                       const std::vector<double>& f_r) {
    std::vector<double> scaled(plan.N);
    for (int n = 0; n < plan.N; ++n) {
        scaled[n] = f_r[n] / plan.JR[n];
    }
    std::vector<double> F = qdht_matvec_impl(plan, scaled);
    for (int m = 0; m < plan.N; ++m) {
        F[m] *= plan.JV[m];
    }
    return F;
}

std::vector<double> qdht_inverse_impl(const QDHTReciprocalPlanImpl& plan,
                                       const std::vector<double>& F_k) {
    std::vector<double> scaled(plan.N);
    for (int n = 0; n < plan.N; ++n) {
        scaled[n] = F_k[n] / plan.JV[n];
    }
    std::vector<double> f = qdht_matvec_impl(plan, scaled);
    for (int m = 0; m < plan.N; ++m) {
        f[m] *= plan.JR[m];
    }
    return f;
}

} // namespace

// ============================================================
// Public C ABI (opaque handle + raw arrays), declared in
// sasfit_hankel.h for use from sasfit.c and elsewhere.
// ============================================================

struct sasfit_qdht_plan {
    QDHTReciprocalPlanImpl impl;
};

extern "C" {

sasfit_qdht_plan* sasfit_qdht_build_plan(int N, double nu, double R) {
    if (N <= 0 || !(R > 0.0) || (nu != 0.0 && nu != 1.0)) {
        return NULL;
    }
    sasfit_qdht_plan* plan = new (std::nothrow) sasfit_qdht_plan();
    if (!plan) return NULL;
    plan->impl = qdht_build_reciprocal_plan_impl(N, nu, R);
    return plan;
}

void sasfit_qdht_free_plan(sasfit_qdht_plan* plan) {
    delete plan;
}

int sasfit_qdht_plan_size(const sasfit_qdht_plan* plan) {
    return plan ? plan->impl.N : 0;
}

double sasfit_qdht_plan_r_node(const sasfit_qdht_plan* plan, int n) {
    if (!plan || n < 0 || n >= plan->impl.N) return 0.0;
    return plan->impl.r_nodes[n];
}

double sasfit_qdht_plan_k_node(const sasfit_qdht_plan* plan, int n) {
    if (!plan || n < 0 || n >= plan->impl.N) return 0.0;
    return plan->impl.k_nodes[n];
}

// f_r/F_k: caller-owned arrays of length sasfit_qdht_plan_size(plan).
void sasfit_qdht_forward_grid(const sasfit_qdht_plan* plan, const double* f_r, double* F_k) {
    if (!plan || !f_r || !F_k) return;
    std::vector<double> f(f_r, f_r + plan->impl.N);
    std::vector<double> F = qdht_forward_impl(plan->impl, f);
    std::copy(F.begin(), F.end(), F_k);
}

void sasfit_qdht_inverse_grid(const sasfit_qdht_plan* plan, const double* F_k, double* f_r) {
    if (!plan || !F_k || !f_r) return;
    std::vector<double> F(F_k, F_k + plan->impl.N);
    std::vector<double> f = qdht_inverse_impl(plan->impl, F);
    std::copy(f.begin(), f.end(), f_r);
}

// Convenience composition: forward -> correction(k) -> backward.
// correction is called with SASfit's usual (double, void*) convention,
// once per k_node.
//
// DESIGN NOTE: kept as a thin wrapper around
// sasfit_qdht_forward_grid/sasfit_qdht_inverse_grid, not a separate
// implementation. Callers that want F(k) on its own, or that want to
// try several corrections against one forward pass, should call
// sasfit_qdht_forward_grid/sasfit_qdht_inverse_grid directly instead,
// so the O(N^2) forward pass is only ever paid once per distinct f.
//
// f_corrected_out must have length sasfit_qdht_plan_size(plan); the
// matching r for f_corrected_out[n] is sasfit_qdht_plan_r_node(plan, n).
void sasfit_qdht_round_trip(const sasfit_qdht_plan* plan,
                             double (*f)(double, void*), void* fparams,
                             double (*correction)(double, void*), void* cparams,
                             double* f_corrected_out) {
    if (!plan || !f || !correction || !f_corrected_out) return;
    const int N = plan->impl.N;
    std::vector<double> f_vals(N);
    for (int n = 0; n < N; ++n) {
        f_vals[n] = f(plan->impl.r_nodes[n], fparams);
    }
    std::vector<double> F_vals = qdht_forward_impl(plan->impl, f_vals);
    for (int m = 0; m < N; ++m) {
        F_vals[m] *= correction(plan->impl.k_nodes[m], cparams);
    }
    std::vector<double> f_corr = qdht_inverse_impl(plan->impl, F_vals);
    std::copy(f_corr.begin(), f_corr.end(), f_corrected_out);
}

} // extern "C"

// ============================================================
// Keyed LRU cache of (plan, spline) results
// ============================================================
//
// The grid-based transform above only gives values at the plan's own
// r_nodes/k_nodes, not at whatever Q a fit actually asks for -- some
// interpolation is needed regardless, so a spline is not just a speed
// optimization, it is the natural way to answer an arbitrary-Q query
// at all. Caching the (plan, spline) pair keyed on a caller-supplied
// fingerprint avoids repeating the O(N^2) transform(s) when nothing
// that affects the underlying function actually changed between
// calls.
//
// The fingerprint is an opaque, caller-defined, fixed-size byte blob
// compared byte-for-byte (memcmp), deliberately exact rather than a
// tolerance/ratio check: if the caller's fingerprint struct is zero-
// initialized and filled consistently, an unchanged fingerprint really
// does mean an unchanged function, with no risk of a near-miss false
// hit. Eviction is least-recently-used, tracked with a monotonically
// increasing logical clock (not wall-clock time) -- the slot with the
// smallest last-used value is evicted first when the cache is full.

namespace {

struct sasfit_qdht_cache_slot {
    bool valid = false;
    std::vector<unsigned char> key;
    unsigned long last_used = 0;
    sasfit_qdht_plan* plan = nullptr;
    gsl_spline* spline = nullptr;
    gsl_interp_accel* accel = nullptr;
};

void sasfit_qdht_cache_slot_release(sasfit_qdht_cache_slot& slot) {
    if (slot.spline) { gsl_spline_free(slot.spline); slot.spline = nullptr; }
    if (slot.accel)  { gsl_interp_accel_free(slot.accel); slot.accel = nullptr; }
    if (slot.plan)   { sasfit_qdht_free_plan(slot.plan); slot.plan = nullptr; }
    slot.valid = false;
}

} // namespace

struct sasfit_qdht_cache {
    size_t key_size = 0;
    unsigned long clock = 0;
    std::vector<sasfit_qdht_cache_slot> slots;
};

extern "C" {

sasfit_qdht_cache* sasfit_qdht_cache_create(int capacity, size_t key_size) {
    if (capacity <= 0 || key_size == 0) return NULL;
    sasfit_qdht_cache* cache = new (std::nothrow) sasfit_qdht_cache();
    if (!cache) return NULL;
    cache->key_size = key_size;
    cache->slots.resize(static_cast<size_t>(capacity));
    return cache;
}

void sasfit_qdht_cache_free(sasfit_qdht_cache* cache) {
    if (!cache) return;
    for (auto& slot : cache->slots) {
        sasfit_qdht_cache_slot_release(slot);
    }
    delete cache;
}

int sasfit_qdht_cache_lookup(sasfit_qdht_cache* cache, const void* key,
                              sasfit_qdht_plan** plan_out,
                              gsl_spline** spline_out,
                              gsl_interp_accel** accel_out) {
    if (!cache || !key) return 0;
    for (auto& slot : cache->slots) {
        if (slot.valid && slot.key.size() == cache->key_size
            && std::memcmp(slot.key.data(), key, cache->key_size) == 0) {
            slot.last_used = ++cache->clock;
            if (plan_out)   *plan_out = slot.plan;
            if (spline_out) *spline_out = slot.spline;
            if (accel_out)  *accel_out = slot.accel;
            return 1;
        }
    }
    return 0;
}

void sasfit_qdht_cache_insert(sasfit_qdht_cache* cache, const void* key,
                               sasfit_qdht_plan* plan,
                               gsl_spline* spline,
                               gsl_interp_accel* accel) {
    if (!cache || !key) return;

    sasfit_qdht_cache_slot* target = NULL;
    for (auto& slot : cache->slots) {
        if (!slot.valid) { target = &slot; break; }
    }
    if (!target) {
        sasfit_qdht_cache_slot* oldest = &cache->slots[0];
        for (auto& slot : cache->slots) {
            if (slot.last_used < oldest->last_used) oldest = &slot;
        }
        target = oldest;
    }

    sasfit_qdht_cache_slot_release(*target);
    target->key.assign(static_cast<const unsigned char*>(key),
                        static_cast<const unsigned char*>(key) + cache->key_size);
    target->plan = plan;
    target->spline = spline;
    target->accel = accel;
    target->last_used = ++cache->clock;
    target->valid = true;
}

} // extern "C"

scalar sasfit_qdht(double nu, double (*f)(double, void *), double x, void *fparams) {
    if (f == NULL || !(x > 0.0) || (nu != 0.0 && nu != 1.0)) {
        return NAN;
    }

    try {
        // Cache the whole prepared plan across consecutive calls with
        // the same (f, fparams, nu) -- same safe ratio-of-confirmed-
        // nonzero-anchors validation as sasfit_bestlime/sasfit_fftlog
        // (see sasfit_bestlime.cpp for the detailed rationale: it is
        // invariant to pure scale/background-type parameter changes,
        // which dominate a typical finite-difference gradient
        // evaluation, while still catching genuine shape/size changes).
        //
        // NOT thread-safe (function-local static state) -- fine only if
        // this function is not called concurrently from multiple threads
        // for different (f, fparams) at the same time.
        static double (*cached_f)(double, void *) = nullptr;
        static void *cached_fparams = nullptr;
        static double cached_nu = -1.0;
        static double cached_anchor_lo = 0.0, cached_anchor_hi = 0.0;
        static double cached_ratio = 0.0;
        static QDHTPlan cached_plan;
        static bool have_plan = false;
        static double cached_x_max = 0.0;
        // Tail-extrapolation state: only populated when f1 has NO genuine
        // compact support (for a compactly-supported f1 the discarded
        // remainder is exactly zero and no correction is wanted).
        static bool cached_tail_active = false;
        static double cached_tail_R = 0.0, cached_tail_fR = 0.0;
        static double cached_tail_rprev = 0.0, cached_tail_fprev = 0.0;

        bool cache_hit = false;
        if (have_plan && cached_f == f && cached_fparams == fparams && cached_nu == nu
            && x <= cached_x_max) {
            const double val_lo = f(cached_anchor_lo, fparams);
            const double val_hi = f(cached_anchor_hi, fparams);
            const double ratio = val_hi / (val_lo + 1e-300);
            constexpr double tol = 1e-3;
            if (std::fabs(ratio - cached_ratio) <= tol * (std::fabs(ratio) + std::fabs(cached_ratio) + 1e-300)) {
                cache_hit = true;
            }
        }

        if (!cache_hit) {
            // Same coarse log-spaced probe as sasfit_bestlime/
            // sasfit_fftlog to find where |r*f(r)| peaks.
            std::vector<std::pair<double, double>> samples;
            samples.reserve(121);
            double r0 = 1.0;
            double best_val = -1.0;
            for (int i = -60; i <= 60; ++i) {
                const double r = pow(10.0, i / 15.0);
                const double val = fabs(r * f(r, fparams));
                if (std::isfinite(val)) {
                    samples.push_back({r, val});
                    if (val > best_val) {
                        best_val = val;
                        r0 = r;
                    }
                }
            }
            const double peak_r = r0;

            // Precise, bisection-refined edge detection (same technique
            // as sasfit_bestlime.cpp) -- NOT a coarse relative-threshold
            // scan, and not optional: see file header for the measured
            // sensitivity of this method's accuracy (and ringing) to
            // exactly where R sits relative to a genuine compact-support
            // edge.
            double edge = -1.0;
            constexpr double zero_threshold = 1e-300;
            double bracket_lo = -1.0, bracket_hi = -1.0;
            for (size_t i = 0; i < samples.size(); ++i) {
                if (samples[i].first < r0 * 2.0) {
                    continue;
                }
                if (samples[i].second < zero_threshold) {
                    bool persistent = true;
                    for (size_t j = i; j < samples.size(); ++j) {
                        if (samples[j].second >= zero_threshold) {
                            persistent = false;
                            break;
                        }
                    }
                    if (persistent) {
                        bracket_hi = samples[i].first;
                        bracket_lo = (i > 0) ? samples[i - 1].first : 0.0;
                        break;
                    }
                }
            }
            if (bracket_hi > 0.0) {
                for (int iter = 0; iter < 60; ++iter) {
                    const double mid = 0.5 * (bracket_lo + bracket_hi);
                    const double val = fabs(mid * f(mid, fparams));
                    if (val < zero_threshold) {
                        bracket_hi = mid;
                    } else {
                        bracket_lo = mid;
                    }
                }
                edge = bracket_hi;
            }
            // Reject false-positive "edges" from floating-point underflow
            // of a smooth infinite-tail function (confirmed: both a
            // Gaussian and a DAB-type exponential hit exact 0.0 at very
            // large r despite having no genuine compact support) -- a
            // real compact-support edge sits at a modest multiple of
            // peak_r (~2x for the sphere/cylinder test cases), not
            // hundreds of times larger.
            if (edge > 20.0 * peak_r) {
                edge = -1.0;
            }

            double R;
            if (edge > 0.0) {
                // Genuine compact support: use the precisely-bisected
                // edge with a tuned 10% margin -- empirically the best
                // validated choice (smallest error, no visible ringing;
                // see file header for the measured sensitivity to this
                // margin).
                R = edge * 1.10;
            } else {
                // No compact support: locate where |r*f(r)| has fallen to
                // a small fraction of its peak.
                //
                // The threshold is 1e-6, NOT something much smaller. This
                // matters enormously for power-law-decaying inputs, which
                // are the norm in SAS: a sphere form factor decays only
                // as q^-4, so it takes a VERY long way out to fall to
                // (say) 1e-10 of its peak, whereas an exponential tail
                // gets there quickly. An earlier 1e-10 threshold picked
                // R = 2929*peak_r for a 25%-polydisperse sphere form
                // factor (vs ~136*peak_r at 1e-6), which with a fixed N
                // starved the resolution so badly that H(0) came out 11x
                // too small and H(r) was non-monotonic -- exactly the
                // "ringing plus wrong baseline" symptom seen against
                // HANKEL_BESTLIME and HANKEL_FFTLOG on the same model
                // (those two agree with each other to within plotting
                // resolution). That combination also made GSL's
                // gsl_sf_bessel_Jnu fail outright with a domain error on
                // the resulting huge arguments. At 1e-6 the same case
                // reproduces the reference answer to within 0.00% of
                // H(0) across the whole r range.
                R = peak_r * 10.0;
                const double rel_threshold = 1e-6 * best_val;
                for (size_t i = 0; i < samples.size(); ++i) {
                    if (samples[i].first < peak_r) {
                        continue;
                    }
                    if (samples[i].second < rel_threshold) {
                        R = samples[i].first;
                        break;
                    }
                }
            }

            // N is chosen from QDHT's own defining constraint, not fixed.
            // The method ties the input and output extents together by
            //     S = alpha_{p,N+1} = 2*pi*R*V,
            // so the reachable output range is x_max = 2*pi*V = S/R. To
            // evaluate at the requested x at all, the grid must satisfy
            // S >~ R*x; since alpha_{p,N+1} ~ pi*N for large N, that is
            // N >~ R*x/pi. A factor-2 margin is applied, and the result
            // clamped to a sane range. Without this, a large R silently
            // produced an output grid that did not reach the requested x
            // (confirmed: this is what made the DAB-type exponential case
            // ring at large x while the same code was accurate at small
            // x). The cache below is keyed on x falling within the built
            // plan's reach, so a later, larger x triggers a rebuild
            // rather than being silently extrapolated.
            int N = static_cast<int>(std::ceil(2.0 * R * x / M_PI));
            if (N < 2048) {
                // Floor of 2048, not something smaller: measured on a
                // DAB-type exponential tail, raising the floor from 512
                // to 2048 cut the worst error from 6.3e-2 to 1.2e-3,
                // while making no difference to the polydisperse-sphere
                // case (already at the 1e-5 level). Cost is linear in N
                // here -- no transform matrix is formed, only O(N)
                // coefficient vectors, and each evaluation is an O(N) sum.
                N = 2048;
            }
            if (N > 8192) {
                N = 8192;
            }

            QDHTPlan plan = qdht_build_plan(N, nu, R);
            std::vector<double> f1_vals(N);
            for (int n = 0; n < N; ++n) {
                const double fv = f(plan.r_nodes[n], fparams);
                f1_vals[n] = std::isfinite(fv) ? fv : 0.0;
            }
            qdht_prepare(plan, f1_vals);
            // Reachable output range of this plan: x_max = S/R.
            cached_x_max = plan.S / plan.R;

            // Capture the last two radial samples for Malinka's tail
            // extrapolation. Enabled only for order 0 without compact
            // support -- see the helper's comment block above.
            cached_tail_active = false;
            if (nu == 0.0 && edge <= 0.0 && N >= 2) {
                cached_tail_R = plan.r_nodes[N - 1];
                cached_tail_fR = f1_vals[N - 1];
                cached_tail_rprev = plan.r_nodes[N - 2];
                cached_tail_fprev = f1_vals[N - 2];
                cached_tail_active = true;
            }

            cached_plan = std::move(plan);
            have_plan = true;

            cached_f = f;
            cached_fparams = fparams;
            cached_nu = nu;
            cached_anchor_lo = 0.5 * peak_r;
            cached_anchor_hi = peak_r;
            const double v_lo = f(cached_anchor_lo, fparams);
            const double v_hi = f(cached_anchor_hi, fparams);
            cached_ratio = v_hi / (v_lo + 1e-300);
        }

        // Convert QDHT's native 2*pi-containing convention to the
        // standard "r dr, no 2*pi" convention sasfit_hankel()'s other
        // strategies use (see file header) -- exact, not approximate.
        double result = qdht_eval(cached_plan, x / (2.0 * M_PI)) / (2.0 * M_PI);

        // Add back the analytically-estimated truncated tail. This is
        // already in the standard convention, so it is added AFTER the
        // 2*pi conversion, not before. Validated against direct numerical
        // integration of the true remainder: recovers it to ~0.02-10%
        // (typically well under 1%), which is what removes the residual
        // low bias in the x->0 limit for power-law and exponential tails.
        if (cached_tail_active) {
            result += qdht_tail_correction(cached_tail_R, cached_tail_fR,
                                           cached_tail_rprev, cached_tail_fprev, x);
        }
        return result;
    } catch (const std::exception& error) {
        sasfit_err("sasfit_qdht: %s\n", error.what());
        return NAN;
    } catch (...) {
        sasfit_err("sasfit_qdht: unknown exception\n");
        return NAN;
    }
}
