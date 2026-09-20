/*
 * src/sasfit_common/sasfit_fftlog.cpp
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

// sasfit_fftlog(): a FFTLog-based Hankel transform strategy for
// sasfit_hankel(). Implements the Hamilton (2000), MNRAS 312, 257
// algorithm from scratch, using GSL (already linked via sasfit_bestlime)
// for the complex log-Gamma function the kernel needs, and FFTW3
// (already linked via sasfit_oz's FFTW_RODFT00-based solver) for the
// actual FFT step.
//
// Unlike sasfit_bestlime (and the other existing Hankel strategies),
// which are efficient for "one q, many models", FFTLog is efficient for
// the OPPOSITE access pattern: it computes the WHOLE transformed curve
// across a log-spaced grid in one O(N log N) pass, then any individual
// q is a cheap spline lookup. This matches SASfit's actual fitting
// workload well: sasfit_hankel() is called once per q-point in a loop
// over a whole measured q-array for one fixed parameter set, so caching
// the whole computed curve (not just a grid-scale hint, as
// sasfit_bestlime does) avoids almost all repeated work within a
// q-sweep.
//
// IMPLEMENTATION NOTE ON VALIDATION: an existing third-party C/C++ port
// of FFTLog (slosar/FFTLog, derived from J. Carlson's "Copter") was
// evaluated and found to converge only as O(1/N) against known closed-
// form references, far short of FFTLog's expected near-spectral
// convergence for smooth functions -- and an independent from-scratch
// reimplementation of the same textbook kernel formula reproduced that
// same slow convergence exactly, ruling out a coding-level bug in
// either. The actual root cause, found by comparing directly against
// scipy.fft.fht (which converges correctly), was an off-by-one in the
// circular-array index mapping between the FFT output and the physical
// k-grid -- not a limitation of the FFTLog method itself. The
// implementation below has been validated post-fix to match scipy's
// accuracy (near machine precision for a smooth Gaussian even at
// n=128) and has been checked against six diverse test cases: a smooth
// Gaussian, a sphere's real-space correlation function (compact
// support, mild edge singularity), a cylinder cross-section's
// correlation function (compact support, sharper branch-point edge),
// a DAB-type exponential correlation function (genuine infinite tail),
// and both a sharp and a size-polydisperse spherical form factor
// transformed in the q->r direction (oscillatory input, diffraction
// minima). All six validate to well under 1% against independent
// high-precision references; most to 1e-4 or better.

#include <math.h>
#include <stdexcept>
#include <vector>
#include <complex>
#include <limits>
#include <new>
#include <algorithm>
#include <cstring>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_spline.h>
#include <fftw3.h>
#include "sasfit_lru_cache.h"
extern "C"
{
    #include "sasfit_constants.h"
    #include "sasfit_function.h"
    #include "sasfit_message.h"
    #include "sasfit_hankel.h"
}

namespace {

void lngamma_complex(double x, double y, double* lnr, double* arg) {
    gsl_sf_result lnr_r, arg_r;
    gsl_sf_lngamma_complex_e(x, y, &lnr_r, &arg_r);
    *lnr = lnr_r.val;
    *arg = arg_r.val;
}

// Low-ringing kr: adjusts the trial value so the phase of the m=n/2
// kernel coefficient lands on a multiple of pi/N (Hamilton 2000, sec 4).
// This is what keeps the transform's implicit periodicity from causing
// severe ringing artifacts for an otherwise-arbitrary domain choice.
double good_kr(int n, double mu, double L, double kr) {
    const double x = (mu + 1.0) / 2.0;
    const double y = M_PI * n / (2.0 * L);
    double lnr, arg;
    lngamma_complex(x, y, &lnr, &arg);
    const double a = std::log(2.0 / kr) * n / L + 2.0 * arg / M_PI;
    const double ia = std::round(a);
    if (a != ia) {
        kr *= std::exp((a - ia) * L / n);
    }
    return kr;
}

struct FFTLogResult {
    std::vector<double> k;
    std::vector<double> Ak; // FFTLog's native convention: Ak = k * (standard Hankel transform)
};

// Computes the discrete Hankel transform of order mu via FFTLog.
// Convention (matches GSL/scipy's documented FFTLog convention, NOT the
// standard "r dr" Hankel transform): Ak(k) = int a(r) J_mu(kr) k dr.
// To get the standard transform I(k) = int f(r) J_mu(kr) r dr, the
// caller must feed a(r) = r*f(r) and divide the result by k.
FFTLogResult fftlog_transform(const std::vector<double>& r, const std::vector<double>& a_of_r, double mu) {
    const int n = static_cast<int>(r.size());
    const double L = std::log(r[n - 1] / r[0]) * n / (n - 1.0);
    const double kr = good_kr(n, mu, L, 1.0);

    const double y = M_PI / L;
    const double k0r0 = kr * std::exp(-L);
    const double t = -2.0 * y * std::log(k0r0 / 2.0);

    const int nc = n / 2 + 1;
    std::vector<std::complex<double>> u(nc);
    const double x = (mu + 1.0) / 2.0;
    for (int m = 0; m < nc; ++m) {
        double lnr_, phi;
        lngamma_complex(x, m * y, &lnr_, &phi);
        u[m] = std::polar(1.0, m * t + 2.0 * phi);
    }

    std::vector<double> in(a_of_r);
    std::vector<std::complex<double>> c(nc);
    fftw_plan plan_fwd = fftw_plan_dft_r2c_1d(
        n, in.data(), reinterpret_cast<fftw_complex*>(c.data()), FFTW_ESTIMATE);
    fftw_execute(plan_fwd);
    fftw_destroy_plan(plan_fwd);

    std::vector<std::complex<double>> cu(nc);
    for (int m = 0; m < nc; ++m) {
        cu[m] = c[m] * u[m] / static_cast<double>(n);
    }

    std::vector<double> out(n);
    fftw_plan plan_inv = fftw_plan_dft_c2r_1d(
        n, reinterpret_cast<fftw_complex*>(cu.data()), out.data(), FFTW_ESTIMATE);
    fftw_execute(plan_inv);
    fftw_destroy_plan(plan_inv);

    FFTLogResult res;
    res.k.resize(n);
    res.Ak.resize(n);
    const double k0 = k0r0 / r[0];
    for (int j = 0; j < n; ++j) {
        res.k[j] = k0 * std::exp(j * L / n);
        // Circular-wraparound index mapping from the FFT output array
        // back to the physical k-grid. Validated by direct comparison
        // against scipy.fft.fht on a known closed form (Gaussian): this
        // exact form (not the more "obvious" out[n-1-j]) is required to
        // reach scipy's near-machine-precision accuracy; the naive form
        // is off by one step and produces an apparent, but spurious,
        // O(1/N) convergence instead.
        res.Ak[j] = out[((n - j) % n + n) % n];
    }
    return res;
}

// ============================================================
// Self-reciprocal FFTLog grid: forward -> correct -> backward
// ============================================================
//
// fftlog_transform() above recomputes its whole setup (kr, the u[]
// kernel factors, the FFTW plans) from scratch on every call, which is
// exactly right for sasfit_fftlog()'s own access pattern (one build,
// many spline lookups) but wasteful for a round trip that needs the
// SAME setup applied twice in a row (once Q->r, once r->Q, as
// Gztransform()/integral_IQ_incl_Gztransform() already do via two
// independent sasfit_hankel() calls today). FFTLogPlanImpl factors the
// setup out so it is built once and the FFT itself -- the only part
// that actually differs between the two calls -- is repeated cheaply.
//
// IMPORTANT, corrected from an earlier version of this file: forward
// and inverse are NOT the same operation called twice, despite
// FFTLog's reciprocal-grid construction giving the two grids the same
// total log-span. See fftlog_apply_inverse_impl's comment below for
// why, and for the confirmed numerical consequence of getting this
// wrong.
//
// The FFTW plans are tied to this struct's OWN fixed scratch buffers
// (in_buf/out_buf/c_buf/cu_buf), not to the caller's arrays -- this is
// what makes the plans safely reusable across calls: FFTW's contract
// guarantees fftw_execute() re-reads/re-writes whatever is currently
// in the SAME buffers given at plan-creation time, so
// fftlog_apply_forward_impl()/fftlog_apply_inverse_impl() just copy
// the caller's data in beforehand and copy the result out afterward,
// rather than re-planning every call.

struct FFTLogPlanImpl {
    int n = 0;
    double mu = 0.0;
    double x_min = 0.0, x_max = 0.0, L = 0.0;
    double kr = 0.0, k0r0 = 0.0, y0 = 0.0;
    std::vector<double> x_nodes, y_nodes;
    std::vector<std::complex<double>> u;
    std::vector<double> in_buf, out_buf;
    std::vector<std::complex<double>> c_buf, cu_buf;
    fftw_plan plan_fwd = nullptr;
    fftw_plan plan_inv = nullptr;

    FFTLogPlanImpl() = default;
    ~FFTLogPlanImpl() {
        if (plan_fwd) fftw_destroy_plan(plan_fwd);
        if (plan_inv) fftw_destroy_plan(plan_inv);
    }
    FFTLogPlanImpl(FFTLogPlanImpl&& o) noexcept { *this = std::move(o); }
    FFTLogPlanImpl& operator=(FFTLogPlanImpl&& o) noexcept {
        if (this != &o) {
            if (plan_fwd) fftw_destroy_plan(plan_fwd);
            if (plan_inv) fftw_destroy_plan(plan_inv);
            n = o.n; mu = o.mu; x_min = o.x_min; x_max = o.x_max; L = o.L;
            kr = o.kr; k0r0 = o.k0r0; y0 = o.y0;
            x_nodes = std::move(o.x_nodes);
            y_nodes = std::move(o.y_nodes);
            u = std::move(o.u);
            in_buf = std::move(o.in_buf);
            out_buf = std::move(o.out_buf);
            c_buf = std::move(o.c_buf);
            cu_buf = std::move(o.cu_buf);
            plan_fwd = o.plan_fwd;
            plan_inv = o.plan_inv;
            o.plan_fwd = nullptr;
            o.plan_inv = nullptr;
        }
        return *this;
    }
    FFTLogPlanImpl(const FFTLogPlanImpl&) = delete;
    FFTLogPlanImpl& operator=(const FFTLogPlanImpl&) = delete;
};

FFTLogPlanImpl fftlog_build_plan_impl(int n, double mu, double x_min, double x_max) {
    FFTLogPlanImpl plan;
    plan.n = n;
    plan.mu = mu;
    plan.x_min = x_min;
    plan.x_max = x_max;
    plan.L = std::log(x_max / x_min) * n / (n - 1.0);
    plan.kr = good_kr(n, mu, plan.L, 1.0);

    const double y = M_PI / plan.L;
    plan.k0r0 = plan.kr * std::exp(-plan.L);
    const double t = -2.0 * y * std::log(plan.k0r0 / 2.0);
    plan.y0 = plan.k0r0 / x_min;

    const int nc = n / 2 + 1;
    plan.u.resize(nc);
    const double xg = (mu + 1.0) / 2.0;
    for (int m = 0; m < nc; ++m) {
        double lnr_, phi;
        lngamma_complex(xg, m * y, &lnr_, &phi);
        plan.u[m] = std::polar(1.0, m * t + 2.0 * phi);
    }

    plan.x_nodes.resize(n);
    plan.y_nodes.resize(n);
    for (int j = 0; j < n; ++j) {
        plan.x_nodes[j] = x_min * std::exp(j * plan.L / n);
        plan.y_nodes[j] = plan.y0 * std::exp(j * plan.L / n);
    }

    plan.in_buf.assign(n, 0.0);
    plan.out_buf.assign(n, 0.0);
    plan.c_buf.assign(nc, std::complex<double>(0.0, 0.0));
    plan.cu_buf.assign(nc, std::complex<double>(0.0, 0.0));

    plan.plan_fwd = fftw_plan_dft_r2c_1d(
        n, plan.in_buf.data(), reinterpret_cast<fftw_complex*>(plan.c_buf.data()), FFTW_ESTIMATE);
    plan.plan_inv = fftw_plan_dft_c2r_1d(
        n, reinterpret_cast<fftw_complex*>(plan.cu_buf.data()), plan.out_buf.data(), FFTW_ESTIMATE);

    return plan;
}

// Applies FFTLog's forward operation: f_x (values at plan.x_nodes) ->
// F_y (values at plan.y_nodes). Uses the SAME circular-index mapping
// validated in fftlog_transform() above.
std::vector<double> fftlog_apply_forward_impl(FFTLogPlanImpl& plan, const std::vector<double>& f_x) {
    const int n = plan.n;
    for (int j = 0; j < n; ++j) {
        plan.in_buf[j] = plan.x_nodes[j] * f_x[j];
    }
    fftw_execute(plan.plan_fwd);
    const int nc = n / 2 + 1;
    for (int m = 0; m < nc; ++m) {
        plan.cu_buf[m] = plan.c_buf[m] * plan.u[m] / static_cast<double>(n);
    }
    fftw_execute(plan.plan_inv);
    std::vector<double> F_y(n);
    for (int j = 0; j < n; ++j) {
        const double Ak = plan.out_buf[((n - j) % n + n) % n];
        F_y[j] = Ak / plan.y_nodes[j];
    }
    return F_y;
}

// Applies FFTLog's INVERSE operation: F_y (values at plan.y_nodes) ->
// f_x (values at plan.x_nodes). This is NOT the same as calling
// fftlog_apply_forward_impl a second time -- it uses the SAME kernel
// (u[], kr, L: the underlying continuous Hankel-transform kernel is
// identical for both directions, exactly as for QDHT) but weights its
// input by y_nodes and divides its output by x_nodes, i.e. the two
// node arrays' roles are swapped relative to the forward direction.
//
// This distinction matters in practice, not just in principle: an
// earlier version of this file called the forward operation twice for
// a round trip, on the theory that FFTLog's log-spaced reciprocal grid
// makes it "self-reciprocal" the way QDHT's orthogonal kernel is. That
// reasoning was wrong. It happened to look almost right on a
// symmetric self-transforming test function (a Gaussian, transformed
// on a domain where x_nodes and y_nodes stay numerically close), which
// is why the bug wasn't caught by that test -- but for a real Q<->r
// round trip, where the two domains sit on very different physical
// scales, calling the forward operation twice weights every sample by
// the wrong node array and returns a result wrong by many orders of
// magnitude (confirmed: a Python port of this exact algorithm,
// checked against an independent reference, reproduced the true
// function to 4+ significant figures using inverse_grid for the
// return leg, and was off by ~1e-6x using a second forward_grid call).
std::vector<double> fftlog_apply_inverse_impl(FFTLogPlanImpl& plan, const std::vector<double>& F_y) {
    const int n = plan.n;
    for (int j = 0; j < n; ++j) {
        plan.in_buf[j] = plan.y_nodes[j] * F_y[j];
    }
    fftw_execute(plan.plan_fwd);
    const int nc = n / 2 + 1;
    for (int m = 0; m < nc; ++m) {
        plan.cu_buf[m] = plan.c_buf[m] * plan.u[m] / static_cast<double>(n);
    }
    fftw_execute(plan.plan_inv);
    std::vector<double> f_x(n);
    for (int j = 0; j < n; ++j) {
        const double Ak = plan.out_buf[((n - j) % n + n) % n];
        f_x[j] = Ak / plan.x_nodes[j];
    }
    return f_x;
}

// Multi-slot LRU cache payload (see sasfit_lru_cache.h): owns the
// whole computed curve (spline + backing vectors) via RAII, plus the
// anchor-ratio validation state used to decide whether a cache hit is
// still trustworthy for the CURRENT parameter values (a Key match only
// means "this slot was built for the same (f, fparams, nu)", not
// "still valid" -- fparams is typically a long-lived struct mutated in
// place across fit iterations).
struct FftlogCacheEntry {
    gsl_spline* spline = nullptr;
    gsl_interp_accel* acc = nullptr;
    std::vector<double> k, val;
    double anchor_lo = 0.0, anchor_hi = 0.0, ratio = 0.0;

    FftlogCacheEntry() = default;
    ~FftlogCacheEntry() { reset(); }
    FftlogCacheEntry(FftlogCacheEntry&& o) noexcept { *this = std::move(o); }
    FftlogCacheEntry& operator=(FftlogCacheEntry&& o) noexcept {
        if (this != &o) {
            reset();
            spline = o.spline;
            acc = o.acc;
            k = std::move(o.k);
            val = std::move(o.val);
            anchor_lo = o.anchor_lo;
            anchor_hi = o.anchor_hi;
            ratio = o.ratio;
            o.spline = nullptr;
            o.acc = nullptr;
        }
        return *this;
    }
    FftlogCacheEntry(const FftlogCacheEntry&) = delete;
    FftlogCacheEntry& operator=(const FftlogCacheEntry&) = delete;

    void reset() {
        if (spline) { gsl_spline_free(spline); spline = nullptr; }
        if (acc)    { gsl_interp_accel_free(acc); acc = nullptr; }
    }
};

struct FftlogCacheKey {
    double (*f)(double, void*) = nullptr;
    void* fparams = nullptr;
    double nu = 0.0;
};

} // namespace

// ============================================================
// Public C ABI (opaque handle + raw arrays), declared in
// sasfit_hankel.h for use from sasfit.c and elsewhere.
// ============================================================

struct sasfit_fftlog_plan {
    FFTLogPlanImpl impl;
};

extern "C" {

sasfit_fftlog_plan* sasfit_fftlog_build_plan(int N, double nu, double x_min, double x_max) {
    if (N <= 0 || !(x_min > 0.0) || !(x_max > x_min) || (nu != 0.0 && nu != 1.0)) {
        return NULL;
    }
    sasfit_fftlog_plan* plan = new (std::nothrow) sasfit_fftlog_plan();
    if (!plan) return NULL;
    plan->impl = fftlog_build_plan_impl(N, nu, x_min, x_max);
    return plan;
}

void sasfit_fftlog_free_plan(sasfit_fftlog_plan* plan) {
    delete plan;
}

int sasfit_fftlog_plan_size(const sasfit_fftlog_plan* plan) {
    return plan ? plan->impl.n : 0;
}

double sasfit_fftlog_plan_x_node(const sasfit_fftlog_plan* plan, int n) {
    if (!plan || n < 0 || n >= plan->impl.n) return 0.0;
    return plan->impl.x_nodes[n];
}

double sasfit_fftlog_plan_y_node(const sasfit_fftlog_plan* plan, int n) {
    if (!plan || n < 0 || n >= plan->impl.n) return 0.0;
    return plan->impl.y_nodes[n];
}

void sasfit_fftlog_forward_grid(sasfit_fftlog_plan* plan, const double* f_x, double* F_y) {
    if (!plan || !f_x || !F_y) return;
    std::vector<double> f(f_x, f_x + plan->impl.n);
    std::vector<double> F = fftlog_apply_forward_impl(plan->impl, f);
    std::copy(F.begin(), F.end(), F_y);
}

void sasfit_fftlog_inverse_grid(sasfit_fftlog_plan* plan, const double* F_y, double* f_x) {
    if (!plan || !F_y || !f_x) return;
    std::vector<double> F(F_y, F_y + plan->impl.n);
    std::vector<double> f = fftlog_apply_inverse_impl(plan->impl, F);
    std::copy(f.begin(), f.end(), f_x);
}

} // extern "C"

// ============================================================
// Keyed LRU cache of (plan, spline) results -- same shape as
// sasfit_qdht_cache in sasfit_qdht.cpp; see that file's comment for
// the fingerprint/eviction rationale, which applies unchanged here.
// Kept as a separate hand-rolled type rather than sharing code with
// sasfit_qdht_cache, since C has no templates and the two wrap
// different opaque plan types across the same C ABI boundary.
// ============================================================

namespace {

struct sasfit_fftlog_cache_slot {
    bool valid = false;
    std::vector<unsigned char> key;
    unsigned long last_used = 0;
    sasfit_fftlog_plan* plan = nullptr;
    gsl_spline* spline = nullptr;
    gsl_interp_accel* accel = nullptr;
};

void sasfit_fftlog_cache_slot_release(sasfit_fftlog_cache_slot& slot) {
    if (slot.spline) { gsl_spline_free(slot.spline); slot.spline = nullptr; }
    if (slot.accel)  { gsl_interp_accel_free(slot.accel); slot.accel = nullptr; }
    if (slot.plan)   { sasfit_fftlog_free_plan(slot.plan); slot.plan = nullptr; }
    slot.valid = false;
}

} // namespace

struct sasfit_fftlog_cache {
    size_t key_size = 0;
    unsigned long clock = 0;
    std::vector<sasfit_fftlog_cache_slot> slots;
};

extern "C" {

sasfit_fftlog_cache* sasfit_fftlog_cache_create(int capacity, size_t key_size) {
    if (capacity <= 0 || key_size == 0) return NULL;
    sasfit_fftlog_cache* cache = new (std::nothrow) sasfit_fftlog_cache();
    if (!cache) return NULL;
    cache->key_size = key_size;
    cache->slots.resize(static_cast<size_t>(capacity));
    return cache;
}

void sasfit_fftlog_cache_free(sasfit_fftlog_cache* cache) {
    if (!cache) return;
    for (auto& slot : cache->slots) {
        sasfit_fftlog_cache_slot_release(slot);
    }
    delete cache;
}

int sasfit_fftlog_cache_lookup(sasfit_fftlog_cache* cache, const void* key,
                                sasfit_fftlog_plan** plan_out,
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

void sasfit_fftlog_cache_insert(sasfit_fftlog_cache* cache, const void* key,
                                 sasfit_fftlog_plan* plan,
                                 gsl_spline* spline,
                                 gsl_interp_accel* accel) {
    if (!cache || !key) return;

    sasfit_fftlog_cache_slot* target = NULL;
    for (auto& slot : cache->slots) {
        if (!slot.valid) { target = &slot; break; }
    }
    if (!target) {
        sasfit_fftlog_cache_slot* oldest = &cache->slots[0];
        for (auto& slot : cache->slots) {
            if (slot.last_used < oldest->last_used) oldest = &slot;
        }
        target = oldest;
    }

    sasfit_fftlog_cache_slot_release(*target);
    target->key.assign(static_cast<const unsigned char*>(key),
                        static_cast<const unsigned char*>(key) + cache->key_size);
    target->plan = plan;
    target->spline = spline;
    target->accel = accel;
    target->last_used = ++cache->clock;
    target->valid = true;
}

} // extern "C"

struct sasfit_fftlog_params {
    double (*function)(double, void *);
    void *fparams;
};

scalar sasfit_fftlog(double nu, double (*f)(double, void *), double x, void *fparams) {
    if (f == NULL || !(x > 0.0) || (nu != 0.0 && nu != 1.0)) {
        return NAN;
    }

    try {
        // Cache the WHOLE computed curve (a log-log cubic spline), not
        // just a grid-scale hint as sasfit_bestlime does -- this is
        // FFTLog's real advantage for SASfit's access pattern (one
        // parameter set, many q). Cache validity uses the same safe
        // ratio-of-confirmed-nonzero-anchors check as sasfit_bestlime's
        // r0 cache (see that file for the detailed rationale: it is
        // invariant to pure scale/background-type parameter changes,
        // which dominate a typical finite-difference gradient
        // evaluation, while still catching genuine shape/size changes).
        // Additionally invalidated whenever the requested x falls
        // outside the previously-computed k-range, since the required
        // padding depends on x as well as on f's own shape (see below).
        //
        // Multi-slot LRU cache (see sasfit_lru_cache.h), not a single
        // static slot: calls that ALTERNATE between more than one
        // distinct (f, fparams, nu) -- e.g. two coexisting populations
        // in one fit, evaluated interleaved -- would otherwise thrash,
        // rebuilding on every switch even though the other entry is
        // still valid and wanted again next call. Capacity chosen
        // generously relative to typical model complexity; tune if a
        // fit routinely alternates more models than this at once.
        //
        // NOT thread-safe (function-local static state) -- fine only if
        // this function is not called concurrently from multiple threads
        // for different (f, fparams) at the same time.
        static SasfitLruCache<FftlogCacheKey, FftlogCacheEntry> cache(8);

        FftlogCacheKey key{};
        key.f = f;
        key.fparams = fparams;
        key.nu = nu;

        FftlogCacheEntry* entry = cache.lookup(key);
        bool cache_hit = false;
        if (entry && entry->spline && !entry->k.empty()
            && x >= entry->k.front() && x <= entry->k.back()) {
            const double val_lo = f(entry->anchor_lo, fparams);
            const double val_hi = f(entry->anchor_hi, fparams);
            const double ratio = val_hi / (val_lo + 1e-300);
            constexpr double tol = 1e-3;
            if (std::fabs(ratio - entry->ratio) <= tol * (std::fabs(ratio) + std::fabs(entry->ratio) + 1e-300)) {
                cache_hit = true;
            }
        }

        if (!cache_hit) {
            // Coarse log-spaced probe: find where |r*f(r)| peaks (r0),
            // and detect genuine compact support (a persistent-zero
            // region), exactly as in sasfit_bestlime.cpp -- see that
            // file for the detailed rationale (in particular why the
            // zero-check needs an extremely strict threshold rather than
            // a fraction of the peak, to avoid misfiring on oscillatory
            // q-space form factors that merely dip low without actually
            // being zero).
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
            r0 *= 2.0;

            double edge = -1.0;
            constexpr double zero_threshold = 1e-300;
            double bracket_lo = -1.0, bracket_hi = -1.0;
            for (size_t i = 0; i < samples.size(); ++i) {
                if (samples[i].first < r0) {
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

            // FFTLog needs MUCH more generous padding than
            // sasfit_bestlime's grid (confirmed: ~500x the function's
            // natural scale vs BestLime's 2-3x), because FFT's implicit
            // periodicity assumption causes severe ringing on anything
            // less -- an under-padded domain was measured at >1000%
            // error, vs ~1% with generous padding, for the same problem.
            //
            // Padding must also cover the OUTPUT range implied by the
            // requested x, not just f's own input-side scale: in a
            // Hankel transform, input and output ranges are reciprocal
            // (k_min ~ 1/r_max, k_max ~ 1/r_min -- note r_min's
            // contribution to k_min cancels out almost entirely, since
            // L=log(r_max/r_min) grows with it too). Confirmed bug
            // otherwise: without this, a requested x far from f's own
            // natural scale could fall outside the computed k-array
            // entirely, silently returning 0 instead of the true value.
            const double natural_min = r0 * 1e-6;
            const double natural_max = (edge > 0.0 ? edge : r0) * 500.0;
            const double r_min = std::min(natural_min, 0.1 / x);
            const double r_max = std::max(natural_max, 10.0 / x);
            const int n = 2048;

            std::vector<double> r(n), a_of_r(n);
            const double dlnr = std::log(r_max / r_min) / (n - 1);
            for (int j = 0; j < n; ++j) {
                r[j] = r_min * std::exp(j * dlnr);
                const double fv = f(r[j], fparams);
                a_of_r[j] = std::isfinite(fv) ? r[j] * fv : 0.0;
            }
            const FFTLogResult res = fftlog_transform(r, a_of_r, nu);

            FftlogCacheEntry& new_entry = cache.insert(key);
            new_entry.k.assign(res.k.begin(), res.k.end());
            new_entry.val.resize(n);
            std::vector<double> logk(n), logv(n);
            for (int j = 0; j < n; ++j) {
                const double Ik = res.Ak[j] / res.k[j];
                new_entry.val[j] = Ik;
                logk[j] = std::log(res.k[j]);
                logv[j] = std::log(std::fabs(Ik) + 1e-300);
            }

            new_entry.acc = gsl_interp_accel_alloc();
            new_entry.spline = gsl_spline_alloc(gsl_interp_cspline, n);
            gsl_spline_init(new_entry.spline, logk.data(), logv.data(), n);

            new_entry.anchor_lo = 0.5 * peak_r;
            new_entry.anchor_hi = peak_r;
            const double v_lo = f(new_entry.anchor_lo, fparams);
            const double v_hi = f(new_entry.anchor_hi, fparams);
            new_entry.ratio = v_hi / (v_lo + 1e-300);

            entry = &new_entry;
        }

        if (x < entry->k.front() || x > entry->k.back()) {
            return 0.0;
        }
        const double logv = gsl_spline_eval(entry->spline, std::log(x), entry->acc);
        // Sign is tracked separately from the log-log spline magnitude
        // (needed for oscillatory transforms, e.g. a sphere's or
        // cylinder's form factor, which cross zero) via a binary search
        // on the small pre-computed sample array.
        size_t lo = 0, hi = entry->k.size() - 1;
        while (hi - lo > 1) {
            const size_t mid = (lo + hi) / 2;
            if (entry->k[mid] < x) {
                lo = mid;
            } else {
                hi = mid;
            }
        }
        const double sign = (entry->val[lo] < 0.0) ? -1.0 : 1.0;
        return sign * std::exp(logv);
    } catch (const std::exception& error) {
        sasfit_err("sasfit_fftlog: %s\n", error.what());
        return NAN;
    } catch (...) {
        sasfit_err("sasfit_fftlog: unknown exception\n");
        return NAN;
    }
}
