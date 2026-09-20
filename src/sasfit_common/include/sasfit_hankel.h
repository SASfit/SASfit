/*
 * src/sasfit_common/include/sasfit_hankel.h
 *
 * Copyright (c) 2008-2022, Paul Scherrer Institute (PSI)
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

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <stddef.h>
#include <gsl/gsl_spline.h>

void sasfit_set_FBT(double nu, int option, int N, double Q) ;
scalar sasfit_FBT(scalar x, double (*intKern_fct)(double, void *), void *) ;
scalar sasfit_qwe(double nu, double (*f)(double, void *), double x, void *fparams, int nIntervalsMax, scalar rtol, scalar atol);
scalar sasfit_HankelChave(double nu, double (*f)(double, void *), double x,  void *fparams, int nIntervalsMax, scalar rtol, scalar atol);
scalar sasfit_bestlime(double nu, double (*f)(double, void *), double x, void *fparams);
scalar sasfit_fftlog(double nu, double (*f)(double, void *), double x, void *fparams);
scalar sasfit_qdht(double nu, double (*f)(double, void *), double x, void *fparams);

/*
 * Self-reciprocal QDHT grid/kernel: build once per (N, nu, R) and
 * reuse. Unlike sasfit_qdht() above (single arbitrary continuous x,
 * cached internally per underlying function), this exposes the
 * transform on its own native grid so a forward pass, an elementwise
 * correction, and a backward pass can each be O(N) or O(N^2) without
 * ever re-deriving the grid or inverting a matrix -- forward and
 * inverse share exactly one kernel. See sasfit_qdht.cpp for the
 * derivation and validation. Opaque handle: callers never dereference
 * sasfit_qdht_plan directly, only through the functions below.
 */
typedef struct sasfit_qdht_plan sasfit_qdht_plan;

sasfit_qdht_plan* sasfit_qdht_build_plan(int N, double nu, double R);
void sasfit_qdht_free_plan(sasfit_qdht_plan* plan);
int sasfit_qdht_plan_size(const sasfit_qdht_plan* plan);
double sasfit_qdht_plan_r_node(const sasfit_qdht_plan* plan, int n);
double sasfit_qdht_plan_k_node(const sasfit_qdht_plan* plan, int n);

/* f_r/F_k: caller-owned arrays of length sasfit_qdht_plan_size(plan). */
void sasfit_qdht_forward_grid(const sasfit_qdht_plan* plan, const double* f_r, double* F_k);
void sasfit_qdht_inverse_grid(const sasfit_qdht_plan* plan, const double* F_k, double* f_r);

/*
 * Forward -> correction(k) -> backward in one call. correction uses
 * the same (double, void*) convention as f, called once per k node.
 * f_corrected_out must have length sasfit_qdht_plan_size(plan); the r
 * matching f_corrected_out[n] is sasfit_qdht_plan_r_node(plan, n).
 * Prefer sasfit_qdht_forward_grid/sasfit_qdht_inverse_grid directly if
 * you need F(k) itself, or want to try several corrections against
 * one forward pass -- this is a thin convenience wrapper around them.
 */
void sasfit_qdht_round_trip(const sasfit_qdht_plan* plan,
                             double (*f)(double, void*), void* fparams,
                             double (*correction)(double, void*), void* cparams,
                             double* f_corrected_out);

/*
 * Keyed LRU cache of (plan, spline) results. key is a caller-defined,
 * fixed-size (key_size bytes) fingerprint identifying everything that
 * determines the transformed function's SHAPE (e.g. the relevant fit
 * parameters) -- deliberately NOT the requested x/Q itself, since the
 * whole point is that one cached entry answers many different x/Q
 * queries via the spline. Compared byte-for-byte (memcmp), so keep
 * the fingerprint struct zero-initialized and filled consistently.
 * Evicts the least-recently-used entry when a lookup misses and the
 * cache is already at capacity. Freeing the cache frees every entry's
 * plan/spline/accel.
 */
typedef struct sasfit_qdht_cache sasfit_qdht_cache;

sasfit_qdht_cache* sasfit_qdht_cache_create(int capacity, size_t key_size);
void sasfit_qdht_cache_free(sasfit_qdht_cache* cache);

/* On a hit: marks the entry most-recently-used, fills any non-NULL
 * *_out pointers, and returns 1. On a miss: returns 0 and leaves the
 * *_out pointers untouched. */
int sasfit_qdht_cache_lookup(sasfit_qdht_cache* cache, const void* key,
                              sasfit_qdht_plan** plan_out,
                              gsl_spline** spline_out,
                              gsl_interp_accel** accel_out);

/* Inserts (or replaces, evicting LRU if full) the entry for key. Takes
 * ownership of plan/spline/accel -- they are freed automatically on
 * eviction or sasfit_qdht_cache_free(), so do not free them yourself
 * after a successful insert. */
void sasfit_qdht_cache_insert(sasfit_qdht_cache* cache, const void* key,
                               sasfit_qdht_plan* plan,
                               gsl_spline* spline,
                               gsl_interp_accel* accel);

/*
 * Self-reciprocal FFTLog grid/kernel: build once per (N, nu, x_min,
 * x_max) and reuse. Unlike QDHT's kernel matrix (a fixed symmetric
 * operator whose forward and inverse are genuinely the same object,
 * just with two different diagonal rescalings applied), FFTLog's
 * self-reciprocity is simpler still: the SAME operation, called twice
 * in a row, IS the round trip -- no separate forward/inverse entry
 * point at all, matching how Gztransform()/
 * integral_IQ_incl_Gztransform() already call sasfit_hankel() with
 * the identical shape for both the Q->r and r->Q legs today. x_nodes
 * and y_nodes are the plan's own log-spaced input and reciprocal
 * grids -- which side is "real space" and which is "Q space" is up to
 * the caller, not fixed by the plan (unlike QDHT's R vs k_max).
 * See sasfit_fftlog.cpp for the derivation (factored out of the
 * existing fftlog_transform()) and validation.
 *
 * NOTE the x_min/x_max sensitivity documented in sasfit_fftlog.cpp:
 * FFTLog needs much more generous padding than QDHT or BestLime (FFT's
 * implicit periodicity causes real ringing on under-padded domains,
 * not just wasted resolution) -- do not reuse a QDHT-style margin
 * heuristic here without re-deriving it for this domain.
 *
 * NOTE ALSO: unlike QDHT's self-reciprocal kernel, FFTLog's forward
 * and inverse are genuinely different operations (see
 * sasfit_fftlog_forward_grid/sasfit_fftlog_inverse_grid below) --
 * x_nodes/y_nodes name the plan's two grids, not "whichever side you
 * call first".
 */
typedef struct sasfit_fftlog_plan sasfit_fftlog_plan;

sasfit_fftlog_plan* sasfit_fftlog_build_plan(int N, double nu, double x_min, double x_max);
void sasfit_fftlog_free_plan(sasfit_fftlog_plan* plan);
int sasfit_fftlog_plan_size(const sasfit_fftlog_plan* plan);
double sasfit_fftlog_plan_x_node(const sasfit_fftlog_plan* plan, int n);
double sasfit_fftlog_plan_y_node(const sasfit_fftlog_plan* plan, int n);

/* f_x/F_y: caller-owned arrays of length sasfit_fftlog_plan_size(plan).
 * NOT const plan -- mutates the plan's internal FFTW scratch buffers
 * on every call (that reuse is exactly what makes repeated calls
 * cheap: the FFTW plans themselves, and the u[] kernel factors, are
 * built once and reused as-is).
 *
 * Unlike QDHT, FFTLog's forward and inverse legs are NOT the same
 * operation called twice: sasfit_fftlog_forward_grid weights its
 * input by x_nodes and produces output at y_nodes;
 * sasfit_fftlog_inverse_grid weights its input by y_nodes and
 * produces output at x_nodes. Both use the SAME kernel (u[], kr, L --
 * the same continuous Hankel-transform kernel for both directions,
 * exactly as for QDHT), only the node-array roles swap. Calling
 * forward_grid twice in a row (an earlier version of this API did
 * exactly that) silently produces a badly wrong answer whenever the
 * x-domain and y-domain sit on different physical scales -- which for
 * a real Q<->r round trip they always do -- so use inverse_grid, not
 * a second forward_grid call, for the return leg. */
void sasfit_fftlog_forward_grid(sasfit_fftlog_plan* plan, const double* f_x, double* F_y);
void sasfit_fftlog_inverse_grid(sasfit_fftlog_plan* plan, const double* F_y, double* f_x);

/*
 * Keyed LRU cache of (plan, spline) results -- same shape and
 * semantics as sasfit_qdht_cache above (see its comment for the
 * fingerprint/eviction rationale, which applies unchanged here); kept
 * as a separate type rather than generalizing sasfit_qdht_cache to an
 * arbitrary payload, since sasfit_qdht_cache is already deployed and
 * in use (MSASROUND) and changing its signature would risk breaking
 * that.
 */
typedef struct sasfit_fftlog_cache sasfit_fftlog_cache;

sasfit_fftlog_cache* sasfit_fftlog_cache_create(int capacity, size_t key_size);
void sasfit_fftlog_cache_free(sasfit_fftlog_cache* cache);

int sasfit_fftlog_cache_lookup(sasfit_fftlog_cache* cache, const void* key,
                                sasfit_fftlog_plan** plan_out,
                                gsl_spline** spline_out,
                                gsl_interp_accel** accel_out);

void sasfit_fftlog_cache_insert(sasfit_fftlog_cache* cache, const void* key,
                                 sasfit_fftlog_plan* plan,
                                 gsl_spline* spline,
                                 gsl_interp_accel* accel);
