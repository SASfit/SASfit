/*
 * src/sasfit_common/include/sasfit_lru_cache.h
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

// Generic keyed least-recently-used cache, header-only, C++ internal
// use only (no C ABI here -- for that, see sasfit_qdht_cache in
// sasfit_hankel.h/sasfit_qdht.cpp, which wraps this same idea behind
// an opaque handle + void*/release-callback for C callers).
//
// This factors out the caching SHAPE shared by sasfit_qdht.cpp,
// sasfit_fftlog.cpp, and sasfit_bestlime.cpp: all three cache a
// build-once, evaluate-many result behind a single function-local
// static slot, validated on every call by re-evaluating two confirmed-
// nonzero anchor points and comparing their RATIO against what was
// recorded at cache-build time (see sasfit_bestlime.cpp for the
// detailed rationale -- ratio, not raw value, because SASfit's
// finite-difference fit gradients perturb every parameter of a model
// including pure scale/background ones, which change f's magnitude
// without changing where it peaks; raw-value comparison would
// invalidate the cache on those too, and using a blindly-placed
// anchor risks a false 0/0 match for a compactly-supported model).
//
// A single slot means calls that ALTERNATE between more than one
// distinct (f, fparams, nu) triple -- e.g. two coexisting populations
// in one fit, evaluated interleaved -- thrash: every switch is a full
// cache miss and rebuild, even though the other entry is still valid
// and will be wanted again on the very next call. This template
// generalizes "one static slot" to "N slots, least-recently-used
// evicted", with everything else (what gets cached, and the anchor-
// ratio validation of a hit) left to the caller, unchanged.
//
// Key is a small, fixed-layout POD struct compared byte-for-byte
// (std::memcmp) -- deliberately exact, not fuzzy: zero-initialize it
// (Key key{};) before filling in fields, or padding bytes left
// indeterminate by the compiler will make two logically-identical keys
// compare unequal. This cache does not replace the anchor-ratio
// validation callers already do -- Key should identify the (f,
// fparams, nu) triple only; re-validate the hit the same way as
// before once lookup() returns a match.
//
// T owns whatever payload it manages (a spline, a plan, an integrator,
// ...) via ordinary RAII: insert() destroys and overwrites a slot's
// existing T by plain assignment, so as long as T's destructor (or a
// member's own destructor) frees what it owns, no separate
// release/free plumbing is needed. A payload built from a raw
// C-library handle (gsl_spline*, etc.) needs a small wrapper struct
// with a destructor and a move constructor/assignment (delete the
// copy operations) -- see the worked example in sasfit_fftlog.cpp's
// integration notes.
//
// NOT thread-safe (plain std::vector<Slot>, no locking) -- matches the
// existing NOT-thread-safe function-local static state in all three
// caller files; do not share one instance across threads without
// adding your own locking.

#include <vector>
#include <cstring>
#include <cstddef>

template <typename Key, typename T>
class SasfitLruCache {
public:
    explicit SasfitLruCache(int capacity)
        : slots_(static_cast<size_t>(capacity > 0 ? capacity : 1)) {}

    // Returns a pointer to the cached T for key, or nullptr on a miss.
    // Marks the entry most-recently-used on a hit. The caller still
    // does its own anchor-ratio re-validation of *what lookup()
    // returns* before trusting it -- a Key match only means "this slot
    // was built for the same (f, fparams, nu)", not "still valid".
    T* lookup(const Key& key) {
        for (auto& slot : slots_) {
            if (slot.valid && std::memcmp(&slot.key, &key, sizeof(Key)) == 0) {
                slot.last_used = ++clock_;
                return &slot.value;
            }
        }
        return nullptr;
    }

    // Returns a reference to a slot for key, ready to be filled in by
    // the caller. Evicts the least-recently-used entry if the cache is
    // full and key is not already present (if key IS already present,
    // that same slot is reused/overwritten). Any T previously
    // occupying the returned slot is destroyed via plain assignment
    // before this returns.
    T& insert(const Key& key) {
        Slot* target = nullptr;
        for (auto& slot : slots_) {
            if (slot.valid && std::memcmp(&slot.key, &key, sizeof(Key)) == 0) {
                target = &slot;
                break;
            }
        }
        if (!target) {
            for (auto& slot : slots_) {
                if (!slot.valid) { target = &slot; break; }
            }
        }
        if (!target) {
            target = &slots_[0];
            for (auto& slot : slots_) {
                if (slot.last_used < target->last_used) {
                    target = &slot;
                }
            }
        }
        target->value = T();
        target->key = key;
        target->last_used = ++clock_;
        target->valid = true;
        return target->value;
    }

private:
    struct Slot {
        bool valid = false;
        Key key{};
        unsigned long last_used = 0;
        T value{};
    };

    std::vector<Slot> slots_;
    unsigned long clock_ = 0;
};
