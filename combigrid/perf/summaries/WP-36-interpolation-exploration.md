# WP-36 — Interpolation operators: exploration after WP-29c (measurement only)

Status: PROPOSED · Date: 2026-11-11 · Base commit: 82e48913a
Recommendation: accept (benchmarks only) — owner decision needed on two new levers: evaluate each distinct point once per call, measured 3.45 s for global interpolation and estimated ≈ 2.3–3.0 s for multilinear interpolation, out of the 10.2 s these operators take. WP-06/07/08 confirmed and re-scoped (WP-07/08 twice their estimates). WP-09 gains nothing on the judged suite.
Machine: `perf.py calibrate` ratios e2e 1.008 / 1.001, micro 1.001 / 1.001 (burst / sustained)

## Change
- No production code. New unprotected benchmarks (kept, as WP-24/25): driver `perf_interp_explore`. On the 12 e2e problems it prints counts, speed-of-light floors and prototypes (kernel, cached weights, multi-point, per-call dedup, WP-26 cache); results are checked against the API. Shared loops in `perf_interp_sol.hpp`; 16 micro cases `micro/sol/{interp_tg,interp_tg_integrand,interplin_tg,interplin_tg_integrand}/...` and `micro/sol/barycentric_1d/generic_cached_weights/n{9,65}`.
- Files: `combigrid/benchmarks/perf/{perf_interp_explore.cpp, perf_interp_sol.hpp}` (new), `perf_micro.cpp` (+3 registration functions); `ANALYSIS.md` (section "Exploration WP-36"); `results/WP-36/` (`exploration.md` = full report).

## Gain vs. cost
- Gain: none directly; decision data for interp 5.70 s + interplin 4.51 s (16 t; d ≥ 7 3.25 + 3.96 s).
- Memory: no change. The 12 cases peak at 12.0–24.2 MiB.
- API / functionality: none. Maintainability: +≈ 1100 benchmark lines (driver 718, header 263, micro 115), outside the library.

## Tests
- Pinning tests for later WPs: `interpolation_pinning_test.cpp` (384 golden values), `vector_interpolation_test.cpp`, `interpolation_test.cpp`, the barycentric tests. Normal build of HEAD: `perf.py test` ✓ (once, as asked). Debug / ASan: n.a.

## Reference check
- `perf.py refcheck --snapshot WP-36`: OK, 104/104 at 1 and 16 threads (`results/WP-36/refcheck.txt`).

## Benchmarks
L1 micro (1 t): A/A `WP-36` vs `WP-36-sol` (with the new cases) on 15 existing interp / interplin / sourcefunc / sol cases: all `~ (noise)`, sum 69.12 → 69.07 ms (1.001x; `micro-aa.md`). Speed of light (largest TG, ns per point; production / sol kernel / integrand only):

| case | production | sol kernel | integrand only |
|---|---|---|---|
| interp cc d10 L2 / d5 L6 / d2 L12 | 263.6 / 107.3 / 52.1 | 228.8 / 95.1 / 42.5 | 216.2 / 84.8 / 34.7 |
| interplin d10 L5 / d8 L5 | 232.2 / 163.9 | 188.9 / 140.8 | 187.0 / 140.0 |
| barycentric generic n9 / n65 (per node) | 90.1 / 572.5 | cached weights 1.24 / 1.29 | – |

L3 (16 t, driver, 5 repetitions interleaved; savings = api − variant; all results equal the API to ≤ 1.8e-13):

| lever | measured | saved (12 cases) | d ≥ 7 |
|---|---|---|---|
| global kernel (premapped nodes, odometer, scratch reused) | cc 1.12–1.24x | 0.64 s | 0.42 s |
| generic weights cached per TG | cheb1 d3 10.3x, d5 2.62x, equi d3 6.73x | 0.42 s | – |
| corner loop (Gray code, doubling weight table) | interplin d5–d10 1.26–1.32x, = integrand floor | 1.01 s | 0.91 s |
| global: distinct points once per call (deltas index + gather) | cc 1.69–3.90x (d10 1.97 → 0.73 s) | **3.45 s** | **2.19 s** |
| multi-point API (+ dedup) | cc 6.3–10.6x (7.6–12.9x) | 0 judged | – |
| WP-26 `CachedSourceFunc` | 0.41–0.94x, hit rates 0.59–0.998 | slower | – |

Full tables, profile split, counts, front-end, thread scan: `combigrid/perf/results/WP-36/exploration.md`.

## Attempts not kept
- none (the prototypes are driver variants, kept as measuring sticks).

## Profile shift / follow-ups
Ranked as the PLAN rule asks (absolute e2e time saved, d ≥ 7 first).
- **(1) New "WP-45" (needs decision): global interpolation, each distinct point once per call.** This applies to nested families (cc, equi). Values go into a deltas-indexed store (delta level and rank per node; offset per λ). Each TG gathers its values and collapses as now. Measured 3.45 s (d ≥ 7 2.19 s); WP-06's assembly part is absorbed. Decisions needed:
  - memory: + distinct × m × 8 B per call (cc d10 +18.5 MiB, peak 24 → ≈ 43 MiB);
  - evaluation count: ÷ 2.75–7.79 (refcheck INFO); results bitwise per TG;
  - where the index lives: rebuilt per call (O(closure · d), µs) or kept in `SparseGrid`;
  - cheb1 (not nested): keep the current path.
  For dimension-adaptive refinement it helps (values per delta TG, reused across steps).
- **(2) New "WP-46" (needs decision, prototype first): multilinear interpolation, each distinct corner once per call.** Per call evaluations ÷ 8.30 / 6.97 / 6.73 (d10 / d8 / d5). Distinct corners = Σ_{|λ|≤L} 2^{#zeros(λ)} (370,688 at d10), a per-query deltas structure that needs a sparse λ → offset map at d10. Estimate ≈ 2.3–3.0 s (d ≥ 7 ≈ 2.1–2.8 s), unmeasured. Index-based only: the hashed cache is 0.60x at d10.
- **(3) WP-08 re-scoped (no decision): corner loop.** Location `tools/operators/linear_interpolation/linear_interpolation_tools.cpp`:
  - map the 2d neighbor coordinates per TG;
  - enumerate corners in Gray order, or in index order with incremental updates (keeps the summation order bitwise);
  - build the weight table by doubling (same factors in the same order → bitwise weights).
  Measured 1.01 s (d ≥ 7 0.91 s), reaching the integrand floor. It also replaces the alignment candidate (op-cache misses 15.4 per 1000 at d10, 5x RP-2; 6.6 % front-end stall cycles). It is the base for WP-46.
- **(4) WP-06 re-scoped: global kernel.** Premapped nodes per TG, odometer, no per-TG allocations (2d + 3) or node regeneration, scratch reused. Measured 0.64 s (d ≥ 7 0.42 s); merge into WP-45, or run it alone if WP-45 is rejected.
- **(5) WP-07 re-scoped (no decision, cheap): generic barycentric weights once per TG and dimension** (or per node family and n). Measured 0.42 s (d < 7); it also removes 205k allocations per call (cheb1 d5) and most of those cases' libgomp share.
- **(6) WP-09 (owner): multi-point API.** Judged-suite gain 0. For new-API callers cc 6.3–10.6x, with dedup 7.6–12.9x; generic families need WP-07 (19.5–58.8x).
  - Recommended design: a surrogate on WP-45's value store (distinct × m × 8 B; cc d10 19.4 MB) instead of per-TG tensors (Σ nGP × 8 B = 81.6 MB).
  - Interplin: points share few corners per TG (÷ 1.07–2.97). Its value lies in parallelizing over points for tiny calls (d2 0.105 s → ≈ 0.02 s, estimate).
  - Order: after WP-45.
- **Parallelization**: large cases scale 9.7–10.9x (1 → 16 t), so there is no WP. Capping threads in the short cases ≈ 0.05 s; parallelism over query points only via WP-09; nested regions not needed.
- **WP-17**: no interpolation part. The per-point call is ≤ 1.7 % (4 % at cc d2); batches are TG-wide (global) or per-call delta grids (WP-45/46), the input for WP-09 and for a cache `findBatch` with expensive functions.
- **WP-30**: the m = 1 gap is vector-only code (multi-output formula + out-of-line `hasTrivialResult`, span handler, m-loops). All levers above apply to both value types.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; `--keep combigrid/benchmarks/perf/perf_interp_explore.cpp --keep combigrid/benchmarks/perf/perf_interp_sol.hpp --keep combigrid/benchmarks/perf/perf_micro.cpp` keeps the benchmarks)
