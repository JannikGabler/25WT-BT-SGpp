# WP-36 — interpolation operators after WP-29c: derived report

Snapshot `WP-36` = HEAD 82e48913a (normal build; `perf.py test` ✓). `WP-36-sol` = HEAD + the new
benchmark files (`perf_micro.cpp` cases `micro/sol/{interp_tg,interp_tg_integrand,interplin_tg,
interplin_tg_integrand,barycentric_1d/generic_cached_weights}/...`, `perf_interp_sol.hpp`,
driver `perf_interp_explore.cpp`); libraries identical. `calibrate`: e2e 1.008 / 1.001
(burst / sustained), micro 1.001 / 1.001. Foreign load ≤ 0.8 % in all runs.

Raw files: `e2e-t{1,2,4,8,16}.{txt,json}` (`run e2e`), `profiles/` (17 `perf.py profile` top
lists + RP-3 inline-chain splits, 4 micro profiles), `split-table.md` (`split_table.py`),
`explore-t16.md` (driver), `micro-run.{txt,json}`, `micro-aa.{md,json}`, `perfstat-frontend.txt`,
`allocations-t16.md`; scripts `profile_all.sh`, `resplit.sh`, `threadscan.sh`, `frontend.sh`,
`allocs.sh`, `split_table.py`.

## 1. Times, scaling, memory (`run e2e`, `total`)

| case | 16 t | 1 t | 1 → 16 t | per call (16 t) | TGs | peak RSS 16 t |
|---|---|---|---|---|---|---|
| interp cc d10 L2 p8 | 1.988 s | 20.39 s | 10.25x | 248 ms | 66 | 24.2 MiB |
| interp cc d7 L4 p16 | 1.260 s | 13.72 s | 10.89x | 78.8 ms | 330 | 13.3 MiB |
| interp cc d5 L6 p64 | 1.445 s | 14.82 s | 10.26x | 22.6 ms | 456 | 12.0 MiB |
| interp cc d3 L9 p64 | 0.241 s | 2.235 s | 9.28x | 3.76 ms | 136 | 12.4 MiB |
| interp cc d2 L12 p64 | 0.174 s | 1.068 s | 6.14x | 2.72 ms | 25 | 14.7 MiB |
| interp cheb1 d5 L4 p64 | 0.351 s | 2.617 s | 7.47x | 5.48 ms | 126 | 12.2 MiB |
| interp cheb1 d3 L6 p64 | 0.182 s | 0.689 s | 3.78x | 2.85 ms | 64 | 12.1 MiB |
| interp equi d3 L5 p64 | 0.061 s | 0.207 s | 3.39x | 0.96 ms | 46 | 12.2 MiB |
| interplin d10 L5 p32 | 2.446 s | 25.82 s | 10.56x | 76.4 ms | 3003 | 13.3 MiB |
| interplin d8 L5 p256 | 1.515 s | 14.65 s | 9.67x | 5.92 ms | 1287 | 12.8 MiB |
| interplin d5 L8 p1024 | 0.441 s | 4.318 s | 9.79x | 0.43 ms | 1231 | 13.0 MiB |
| interplin d2 L16 p16384 | 0.111 s | 0.215 s | 1.95x | 6.8 µs | 33 | 19.8 MiB |

Groups (16 t): interp 5.702 s (d ≥ 7: 3.248 s), interplin 4.512 s (d ≥ 7: 3.961 s); = rp-3
(5.70 / 4.55 s). Thread scan of the short cases (`e2e-t{2,4,8}`; 1 / 2 / 4 / 8 / 16 t):
interplin d2 215 / 148 / 110 / **91** / 111 ms, cheb1 d3 689 / 373 / 221 / **154** / 182 ms,
cc d2 1068 / 540 / 312 / 172 / 174 ms, equi d3 207 / 115 / 78 / 67 / 61 ms, cheb1 d5 2617 / 1327
/ 691 / 372 / 351 ms. Capping the thread count would save ≈ 0.05 s in total.

## 2. Profile split (`split-table.md`; % of all samples of the process)

| case | integrand (`cos`, `cosProd`, PLT, `_M_invoke`) | operator kernel | of which `getGridPoint` / call / `mapToDomain` / `nearlyEqual` | barycentric (opt.) | generic weights + `log`/`exp` | allocation | libgomp | OpenBLAS |
|---|---|---|---|---|---|---|---|---|
| interp cc d10 (16 t) | 73.5 | 16.6 | 5.7 / 1.3 / 0.9 / 0.6 | 3.4 | – | 0.0 | 3.9 | 1.2 |
| interp cc d10 (1 t) | 78.4 | 15.1 | 6.1 / 0.5 / 0.7 / 0.7 | 3.1 | – | 0.0 | – | – |
| interp cc d7 (16 t) | 71.6 | 17.1 | 7.8 / 0.8 / 0.6 / 0.8 | 4.5 | – | 0.0 | 3.8 | 1.6 |
| interp cc d5 (16 t) | 67.8 | 19.8 | 8.5 / 1.5 / 0.7 / 1.2 | 5.7 | – | 0.1 | 4.2 | 1.4 |
| interp cc d3 (16 t) | 54.1 | 17.0 | 7.1 / 1.6 / 0.4 / 1.0 | 4.8 | – | 0.1 | 14.6 | 7.3 |
| interp cc d2 (16 t) | 29.1 | 14.2 | 4.1 / 4.0 / 0.3 / 0.7 | 3.3 | – | 0.0 | 40.5 | 10.2 |
| interp cheb1 d5 (16 t) | 23.9 | 6.6 | 2.9 / 0.5 / – / 0.4 | – | 28.1 | 6.8 | 27.1 | 5.3 |
| interp cheb1 d3 (16 t) | 4.9 | 1.7 | | – | 24.5 | 1.3 | 55.8 | 9.6 |
| interp equi d3 (16 t) | 4.0 | 1.3 | | – | 14.6 | 1.2 | 54.8 | 19.7 |
| interplin d10 (16 t) | 66.3 | 27.3 (`computeNeighborGPWithWeight` 20.6, region + `sumOverCorners` 6.7) | – / 0.6 / 0.6 / – | – | – | 0.0 | 3.9 | 0.9 |
| interplin d10 (1 t) | 68.6 | 27.9 (21.5 + 6.3) | | – | – | 0.0 | – | – |
| interplin d8 (16 t) | 66.4 | 24.5 (16.8 + 7.6) | | – | – | 0.0 | 6.5 | 1.4 |
| interplin d5 (16 t) | 57.9 | 25.8 (15.1 + 8.9 + neighbors 1.9) | | – | – | 0.0 | 9.9 | 4.4 |
| interplin d2 (16 t) | 3.4 | 9.9 | | – | – | 1.2 | **71.0** | 10.6 |

- Kernel self share (`processTensorGridInternal` without helpers) 7.7–8.9 %: slice loop, stores,
  copies of the later-dimension slices; no symbol of the collapse steps survives LTO.
- Node regeneration: `genNodes` per TG, dimension and call; for cc / cheb1 one `cos` per node,
  counted in `__cos_fma`. Regenerated nodes per integrand coordinate (driver): cc d2 6.8 %, d3
  1.3 %, d5 0.16 %, d7 0.03 %, d10 0.002 %; cheb1 d3 2.3 %, equi d3 3.0 % → ≤ 1.4 % of the
  samples (cc d2), negligible at d ≥ 5. `genNodesWithoutBoundary` itself ≤ 1.5 % (micro cc d2).
- `hasTrivialResult` (`nearlyEqual` scan per 1D call): 0.6–1.2 % (cc), 1.3 % (cheb1 d5).
- OpenMP: one parallel region per call (global: `parallel for reduction`, `guided`; linear:
  `parallel` + `for nowait`, `static`). libgomp 3.8–4.2 % in the large cases, 27–71 % in the short
  ones (calls of 7 µs … 5.5 ms: barrier / spin). OpenBLAS = pool spin-up of the process.
- Allocations per call (`allocations-t16.md`, heaptrack, 16 t): global 2d + 3 per TG
  (`getInterpolationCntPerDim`, nodes per dimension, gp, results, funcValues, inputValues) →
  cc d10 1587, d7 5943, d5 6387, d3 1363, d2 203 per call; generic formula + 3 per non-trivial 1D
  call → cheb1 d5 **205,833**, cheb1 d3 23,479, equi d3 10,519 per call; interplin 35 per call
  (normalized point + 2 scratch vectors per thread). Only the generic-formula cases pay for them.

## 3. Speed of light

L1 (1 t, `micro-run.txt`, largest TG, ns per grid point / corner):

| kernel | production | sol kernel (premapped nodes, odometer, scratch reused; corners: Gray code + doubling weight table) | integrand only (same points) |
|---|---|---|---|
| interp cc d10 L2 | 263.6 | 228.8 (0.87) | 216.2 (0.82) |
| interp cc d5 L6 | 107.3 | 95.1 (0.89) | 84.8 (0.79) |
| interp cc d2 L12 | 52.1 | 42.5 (0.82) | 34.7 (0.67) |
| interp cheb1 d5 L4 (generic weights per 1D call) | 188.8 | 179.9 (0.95) | 85.0 (0.45) |
| interplin d10 L5 | 232.2 | 188.9 (0.81) | 187.0 (0.81) |
| interplin d8 L5 | 163.9 | 140.8 (0.86) | 140.0 (0.85) |
| interplin d2 L16 | 52.0 | 51.8 (1.00) | 49.0 (0.94) |
| barycentric 1D generic n9 / n65 (ns per node) | 90.1 / 572.5 | cached weights 1.24 / 1.29 | opt. CC formula 3.54 / 3.03 |

L3 (16 t, `explore-t16.md`; driver in-process, 5 repetitions, interleaved; api = the e2e operator
phase; all variants' results equal api within ≤ 1.8e-13 absolute — summation order only):

| case | api | kernel (WP-06/08 sol) | kernel + cached weights (WP-07) | integrand only (`sol_api`) | dedup per call | multi-point | multi + cached w. | multi + dedup | WP-26 cache (hit rate) |
|---|---|---|---|---|---|---|---|---|---|
| cc d10 | 1.972 | 1.720 (1.15) | – | 1.598 (0.81) | **0.725 (2.72)** | 0.314 (6.27) | – | 0.178 (11.1) | 2.491 (0.79) [0.970] |
| cc d7 | 1.262 | 1.094 (1.15) | – | 1.003 (0.79) | **0.324 (3.90)** | 0.145 (8.70) | – | 0.098 (12.9) | 1.475 (0.86) [0.992] |
| cc d5 | 1.442 | 1.292 (1.12) | – | 1.133 (0.79) | **0.395 (3.65)** | 0.136 (10.6) | – | 0.123 (11.7) | 2.071 (0.70) [0.998] |
| cc d3 | 0.241 | 0.208 (1.16) | – | 0.186 (0.77) | 0.091 (2.66) | 0.025 (9.80) | – | 0.023 (10.4) | 0.484 (0.50) |
| cc d2 | 0.172 | 0.139 (1.24) | – | 0.127 (0.73) | 0.102 (1.69) | 0.022 (7.75) | – | 0.023 (7.64) | 0.425 (0.41) |
| cheb1 d5 | 0.341 | 0.325 (1.05) | **0.130 (2.62)** | 0.111 (0.33) | n/a (not nested) | 0.182 (1.87) | 0.017 (19.5) | n/a | 0.367 (0.93) |
| cheb1 d3 | 0.181 | 0.178 (1.02) | **0.018 (10.3)** | 0.015 (0.08) | n/a | 0.122 (1.48) | 0.003 (58.8) | n/a | 0.193 (0.94) |
| equi d3 | 0.051 | 0.050 (1.02) | **0.008 (6.73)** | 0.006 (0.12) | 0.047 (1.09)¹ | 0.036 (1.43) | 0.001 (39.3) | 0.035 (1.47)¹ | 0.055 (0.92) |
| interplin d10 | 2.463 | **1.864 (1.32)** | – | 1.876 (0.76) | (est., §5) | – | – | – | 4.111 (0.60) [0.878] |
| interplin d8 | 1.514 | **1.201 (1.26)** | – | 1.196 (0.79) | (est., §5) | – | – | – | 2.278 (0.66) [0.939] |
| interplin d5 | 0.444 | 0.351 (1.27) | – | 0.342 (0.77) | (est., §5) | – | – | – | 0.806 (0.55) [0.962] |
| interplin d2 | 0.105 | 0.100 (1.05) | – | 0.098 (0.93) | – | – | – | – | 0.179 (0.59) [0.591] |

¹ dedup / multi prototypes use the production generic formula (weights per 1D call dominate).

- **kernel** (no API change): cc 1.12–1.24x, saves 0.64 s (d ≥ 7 0.42 s); it reaches 1.07–1.14x
  of the integrand-only floor. interplin d5–d10 1.26–1.32x, saves 1.01 s (d ≥ 7 0.91 s) and
  **reaches the floor** (kernel ≈ `sol_api`): beyond it only fewer evaluations help.
- **cached generic weights**: cheb1 / equi 2.6–10.3x, saves 0.42 s (all d < 7); with them the
  kernel is again within 1.2x of the floor. (Kernel alone 1.01–1.05x.)
- **dedup per call** (prototype, nested families, single-point API unchanged): every distinct
  sparse-grid point evaluated once per call (values stored per delta tensor grid, index: delta
  level + rank per dimension and node, dense offset table over the λ ≤ L), then per TG gathered
  in index order and collapsed. cc 1.69–3.90x, saves **3.45 s (d ≥ 7 2.19 s)**; evaluations per
  call ÷ 2.75 / 4.86 / 7.79 / 7.48 / 4.21 (d2 … d10, WP-25). Values per call: 131k / 115k / 283k /
  676k / 2.42M points (cc d10 +18.5 MiB).
- **multi-point** (prototype of WP-09 option (a): per TG evaluated once, then the d nested 1D
  interpolations per query point): cc 6.3–10.6x; with dedup 7.6–12.9x; generic families 1.4–1.9x
  unless the weights are cached (19.5–58.8x).
- **WP-26 cache** (`CachedSourceFunc`, default config, emptied per repetition): hit rates
  0.59–0.998, but 0.41–0.94x: a hash lookup costs more than `cosProd` (≈ 35–216 ns per point at
  1 t), as documented (pays only for functions ≳ 0.3 µs). Reuse must be index-based for cheap
  integrands.

## 4. Evaluation counts (driver structure table; WP-25 `gain-bound.md`)

| case | evals per call | p calls | multi-point (TG once) | distinct per call (dedup) | distinct all calls |
|---|---|---|---|---|---|
| cc d10 / d7 / d5 / d3 / d2 | 10.20M / 5.06M / 2.20M / 557k / 360k | × 8 / 16 / 64 / 64 / 64 | ÷ p | 2.42M / 676k / 283k / 115k / 131k (÷ 4.21 / 7.48 / 7.79 / 4.86 / 2.75) | = per call (÷ 34–499 over p calls) |
| cheb1 d5 / d3, equi d3 | 206k / 38.9k / 15.2k | × 64 | ÷ 64 | 58.2k / 18.1k / 3.7k (hash only for cheb1: not nested) | |
| interplin d10 / d8 / d5 / d2 | 3.08M / 329k / 39.4k / 132 | × 32 / 256 / 1024 / 16384 | distinct corners per TG over all p: ÷ **1.07 / 1.90 / 2.97 / 1.18** | ÷ **8.30 / 6.97 / 6.73 / 1.94** | ÷ 9.21 / 16.3 / 26.2 / 2.57 |

Interplin reuse is across tensor grids within one call, not across query points: a TG-batched
multi-point variant saves ≤ 7 % of the evaluations at d10.

## 5. Multilinear corners: structure of the per-call reuse (estimate for a dedup)

For one query point x and dimension j, the bracket (lo, hi) at level l shrinks with l (nested
equidistant nodes; b1 sentinels 0 / 1 at level 0): 2 new coordinates at level 0, 1 per finer
level. A corner of TG ℓ is identified by the level λ_j at which each of its coordinates appeared
first; with all |λ| ≤ L present (d8, d10), the distinct corners per call are
Σ_{|λ|≤L} 2^{#zeros(λ)} = **370,688 (d10 L5), 47,264 (d8 L5)** = WP-25's measured 3.08M / 8.30
and 329k / 6.97. The value layout is the same deltas structure as the global dedup (offset per λ
+ local index), but query-point dependent coordinates and a sparse λ → offset map at d10
((L+1)^d = 6^10 too large for a dense table). Estimate (16 t): integrand ÷ 8.3 / 7.0 (d10 1.88 →
0.23 s, d8 1.20 → 0.17 s) + index / gather ≈ 5–15 ns per corner (0.05–0.15 s) + weights → d10
≈ 0.4–0.7 s (from 2.46), d8 ≈ 0.3–0.45 s (from 1.51), d5 ≈ 0.1 s (from 0.44): **≈ 2.3–3.0 s**
(d ≥ 7 ≈ 2.1–2.8 s). Not measured: a prototype in `perf_interp_explore.cpp` is the first step.

## 6. Front-end (`perfstat-frontend.txt`, 16 t, process-wide; RP-2 in brackets)

| case | IPC | op-cache misses / 1000 accesses | front-end stall cycles | I-cache misses / 1e6 instr. |
|---|---|---|---|---|
| interp cc d10 | 1.72 [1.71] | 3.75 [3.90] | 2.81 % [2.82] | 109 [112] |
| interp cc d7 | 1.83 | 3.72 | 2.20 % | 109 |
| interp cc d5 | 1.98 [1.96] | 3.95 [4.31] | 0.92 % [0.88] | 117 [123] |
| interplin d10 | 1.66 [1.66] | **15.42** [3.09] | **6.57 %** [5.65] | 101 [103] |
| interplin d8 | 1.62 [1.80] | 4.68 [3.34] | 2.70 % [2.86] | 108 [100] |

Interplin d10's op-cache misses rose 5x since RP-2 (code placement after WP-29c); the stall share
(6.6 %, ≈ 0.16 s) bounds an alignment-only gain. The kernel rewrite (WP-08) replaces this loop.

## 7. Vector path at m = 1 (`micro-run.txt`, 1 t; `profiles/micro_*`)

vector / scalar time: interp cc d2 1.22, cc d5 1.12, cc d10 1.05, cheb1 d5 1.02; interplin d2
1.11, d8 1.02, d10 1.04 (= WP-29c's 0.82–0.98x / 0.89–1.01x). Shared by both instantiations
(templates over the value type): grid-point assembly, domain mapping, node regeneration, per-TG
allocations, collapse loops (global); neighbors, `computeNeighborGPWithWeight` (out of line, the
same function: 20.1 vs 17.0 % at d10) and the region (linear). Vector-only: the multi-output
`OptBarycentricFormula::interpolate` + out-of-line multi-output `hasTrivialResult` (cc d2:
16.6 + 4.7 % vs scalar formula 9.2 %), the `VectorSourceFunc` handler (span; 15.8 vs 12.6 %),
m-loops of `addScaled` / `store`. Every proposal below (kernel, cached weights, dedup) applies to
both instantiations; the m = 1 gap itself stays WP-30.

## 8. Hypotheses of WP-06 / 07 / 08 after WP-29c

- **WP-06** (operator overhead): still true, smaller than estimated: per-point assembly
  (`getGridPoint` 5.7–8.5 %), mapping (0.6–0.9 %), regeneration (≤ 1.4 %, d2 only), allocations
  (≈ 0 except generic), `hasTrivialResult` (0.6–1.2 %); libgomp 4 % at d ≥ 5. Measured sol kernel
  0.64 s (d ≥ 7 0.42 s) vs. 0.6–0.7 s estimated. Location now
  `tools/operators/global_interpolation/global_interpolation_tools.cpp` (`processTensorGridInternal`).
- **WP-07** (generic weights): true and **2x larger** than estimated: 0.42 s (cheb1 d3 10.3x,
  cheb1 d5 2.6x, equi d3 6.7x) vs. ≈ 0.2 s; the weights also cause 205k allocations per call
  (cheb1 d5) and most of the libgomp share of these cases (imbalance over few TGs).
- **WP-08** (corner loop): true: `computeNeighborGPWithWeight` 20.6 / 16.8 % (out of line, d
  divisions per corner) + region 6.7 / 7.6 %; sol kernel saves 1.01 s (d ≥ 7 0.91 s) vs. ≈ 0.5 s
  estimated and reaches the integrand floor. Alignment candidate absorbed by the rewrite.
