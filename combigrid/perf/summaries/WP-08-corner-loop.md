# WP-08 — Linear interpolation: corner loop (+ cross-TG corner dedup prototype)

Status: PROPOSED · Date: 2026-11-11 · Base commit: c3cc48360
Recommendation: accept the kernel. interplin is 1.25–1.33x faster at d5–d10 and saves 1.02 s (d ≥ 7: 0.94 s). Per-TG results stay bitwise identical and nothing changes in the API. **Cross-TG dedup: the owner chose option (c) (2026-11-11)**, a `DeltasSparseGrid` variant of `interpolateLinear`. It is a new WP; the `SparseGrid` operator stays with the per-TG kernel. The prototype saves another 2.1 s (to 2.5 s) on the judged cases.
Machine: `perf.py calibrate` ratios e2e 1.002 / 0.999, micro 0.999 / 1.001 (burst / sustained; before measuring: 1.002 / 0.999, 1.005 / 1.000)

## Change
- `tools/operators/linear_interpolation/linear_interpolation_tools.cpp` (`sumOverCorners` and the new helpers `prepareCorners`, `weights1D`, `moveToCorner`, declared and documented in the `.hpp`). Per TG, the 2d bracket coordinates are mapped onto the domain once (`tools::mapToDomain`) and the 1D weights are computed once.
  - The corner weights come from a table built by doubling. The products and their order are the same as before, so every weight is bitwise unchanged.
  - Corners are visited in index order, as before; on average 2 coordinates change per corner.
  - This removes `computeNeighborGPWithWeight`: out of line, with d divisions and d coordinates per corner, plus `denormalizeDataVector` per corner. Both value types share the kernel.
- Memory stays bounded at high d. The table covers the first min(d, `constants::lin_interpolation::WEIGHT_TABLE_MAX_DIM` = 10) dimensions, i.e. at most 8 KiB per thread. The higher dimensions' factors are fixed per block of 2^10 corners and multiplied per corner after the table entry, which keeps the order of the definition.
- `LinInterpScratch` holds the corner data in one buffer (one allocation per thread and call): table, bracket, factors, high factors. Its constructor signature is unchanged. Unused includes were dropped.
- Helpers moved out of the anonymous namespace at the owner's request (2026-11-11). `weights1D` and `moveToCorner` are still inlined. `prepareCorners` is now one out-of-line call per TG.
  - Measured `WP-08-after` → `WP-08-export`: micro 40 cases all noise (sum 0.997x, `micro-export.md`); e2e interplin all noise at 5 rounds (`e2e-export.md`).
  - At 8 rounds d8 shows 0.948x (`e2e-export-r8.md`). This is bimodal process noise: an A/A of `WP-08-after` alone splits into rounds at 1.19–1.23 s and 1.30–1.37 s (`e2e-aa-d8.md`). Both binaries take ≈ 1.20 s in the fast mode, and d10 is 1.014x (noise).
  - The headline numbers below are `WP-08-before` vs. `WP-08-after` (helpers TU-local).
- Prototype only, not in the library: cross-TG corner dedup in `perf_interp_explore.cpp` (`LinDedupIndex`, `runLinearDedup`).
- Files: `constants.hpp`, `miscellaneous/data_scratches/linear_interpolation_scratch.hpp`, `tools/operators/linear_interpolation/linear_interpolation_tools.{hpp,cpp}` (+158 / −71). Tests: new `tests/tools/operators/linear_interpolation/linear_interpolation_tools_test.cpp`. Benchmarks: `perf_micro.cpp` (+4 cases), `perf_interp_explore.cpp` (+300). Results in `results/WP-08/`. Docs: no references to update.

## Gain vs. cost
- Gain (16 t, `total`): d10 **1.334x** (2.454 → 1.837 s), d8 1.267x, d5 1.251x, d2 within noise. Group 4.525 → 3.506 s, **saved 1.020 s** (expected 1.01 s); production reaches the WP-36 speed-of-light kernel (driver: api 1.843 vs. sol 1.865 s at d10).
- Memory: unchanged (geomean B/A 1.009, 0 flags); scratch +2^min(d,10) + 5d − min(d,10) doubles per thread.
- API / functionality: none; evaluation count and order unchanged; Python bindings rebuilt (`SG_PYTHON=1`, all 10 examples ✓; only a transitively included header changed).
- Maintainability: kernel +60 net lines; one generic path for all d (no dispatch). `sumOverCorners` has a 35-line body (the hoisted locals across the opaque call are the reason, WP-01).

## Tests
- Pinning (passed on the unchanged code, debug ×3): `interpolation_pinning/Linear_Interpolation_Golden_Values` (192 golden values), `vector_interpolation/*` (linear: vector == scalar, same points), and the new `linear_interpolation_tools/Kernel_Equals_Definition_{Low,High}_Dimensions`. The new test checks bitwise the evaluated points (count, order, coordinates) and every corner weight (indicator functions; at d12, 64 spread corners per TG) against a direct implementation of the definition. It covers scalar and vector (m = 3), equi / cc, b0 / b1 / b2, unit and shifted domains, d = 1–4 complete + anisotropic MIVec and d = 12, and points in the interior, on nodes and on the domain boundary (lower, upper, mixed corners). Operator values are checked within round-off.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ (plus the dedup prototype under ASan ✓); after the helper move debug and normal `--repeat 3` ✓. No flaky recurrence.

## Reference check
- OK, 104/104 at 1 and 16 threads (`results/WP-08/refcheck.txt`). At 1 thread the per-TG values, points and weights are bitwise identical (`results/WP-08/bitwise.md`). Operator results of interplin d5 still differ by ≤ 7.1e-15 (d1–d3 bitwise equal). The cause is the unchanged driver: after the inlining changed, GCC contracts `result += c·v` into an FMA.

## Benchmarks (A = WP-08-before, B = WP-08-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `results/WP-08/micro.md`, 45 cases, sum 1.124x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interplin/tg d10 L5 / d8 L5 / d5 L8 (new) | 229.1 / 42.04 / 3.42 µs | 178.1 / 36.32 / 2.98 µs | 1.273 / 1.158 / 1.146 | [1.228, 1.319] / [1.151, 1.166] / [1.141, 1.150] |
| interplin/tg d16 L2 (new, beyond the table) / d2 L16 | 30.70 ms / 205.2 ns | 23.53 ms / 202.8 ns | 1.303 / 1.011 | [1.279, 1.328] / [1.001, 1.022] |
| tg_scalar_x_m d10 m1 … m32 | | | 1.349 … 1.393 | all faster |
| tg_vec d10 m1 / m2 / m8 / m32 | | | 1.299 / 1.191 / 1.065 / 1.020 | all faster |
| tg_vec d2 m4 / m8 | 651.6 / 1190 ns | 654.6 / 1200 ns | 0.994 / 0.987 | SLOWER (≤ 1.3 %, 4 corners per TG) |
| sol/interplin_tg* (controls, unchanged code) | | | 0.989–1.007 | 9 of 10 noise |

Production vs. speed of light (after): d10 178.1 µs vs. sol 191.9 µs, d8 36.3 vs. 36.1, d5 2.98 vs. 2.96, d16 23.53 vs. 22.81 ms (sol keeps a full 2^16 table).

L2/L3 e2e (16 threads; `results/WP-08/e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interplin d10 L5 p32 | total / operator | 2.454 / 2.451 s | 1.837 / 1.834 s | 1.334 / 1.335 | [1.307, 1.362] |
| interplin d8 L5 p256 | total / operator | 1.517 / 1.514 s | 1.195 / 1.193 s | 1.267 / 1.268 | [1.238, 1.296] |
| interplin d5 L8 p1024 | total / operator | 443.7 / 440.9 ms | 362.6 / 359.6 ms | 1.251 / 1.253 | [1.163, 1.345] |
| interplin d2 L16 p16384 | total | 111.0 ms | 111.0 ms | 1.013 | [0.963, 1.066] noise |

Sum of medians: A = 4.525 s, B = 3.506 s, saved = 1.020 s. Peak memory: 13.0–19.9 MiB, B/A 0.996–1.014, geomean 1.009, 0 MEM flags; floor A 10.1, B 10.2 MiB.

## Cross-TG corner dedup (prototype; owner decision)
In one call, every distinct corner is evaluated once. A corner coordinate is keyed by the level λ_j at which it first appears, plus one bit for the 2 coordinates of level 0. The store has a block of 2^#zeros(λ) slots per λ with |λ| ≤ L, ranked lexicographically. The index is updated incrementally, like the corners, with no hash.
- Passes (one region): mark the corners of all TGs, evaluate the marked slots, then gather per TG with the new kernel's weights and order. Per-TG sums are bitwise those of the kernel.
- "all" variant: without the marking pass, every slot is evaluated. Unused slots evaluate at the domain corner, which is observable.
- Driver `results/WP-08/explore-t16.md`: 16 t, 7 repetitions, interleaved. Speedups are vs. the new API. All results agree with the API within ≤ 2.0e-13.

| case | evals per call api → distinct | store per call | lin dedup [s] | lin dedup all [s] |
|---|---|---|---|---|
| d10 L5 p32 (api 1.843 s) | 3,075,072 → 370,688 (÷ 8.30) | 3.2 MiB | 0.554 (3.33x) | 0.406 (4.54x) |
| d8 L5 p256 (1.206 s) | 329,472 → 47,264 (÷ 6.97) | 415 KiB | 0.490 (2.46x) | 0.350 (3.44x) |
| d5 L8 p1024 (0.360 s) | 39,392 → 5,852 (÷ 6.73) | 52 KiB | 0.245 (1.47x) | 0.153 (2.35x) |
| d2 L16 p16384 (0.103 s) | 132 → 68 (÷ 1.94) | 1.7 KiB | 0.337 (0.30x) | 0.329 (0.31x) |

Measured gains, with gating so that d2 keeps the per-TG kernel: marking saves **2.12 s** (3.51 → 1.39 s; d ≥ 7 2.00 s); "all" saves 2.50 s. Measured from the pre-WP-08 code: 4.53 → 1.39 s, which beats the WP-36 estimate of 2.3–3.0 s. Without the marking pass, d10 has a floor of ≈ 0.14 s of integrand time. The rest is index walking and gathering (≈ 33 ns per corner and pass), which leaves room for further work.
- **(a) Per-TG kernel only (this WP).** 1.02 s; no further cost.
- **(b) Dedup inside the `SparseGrid` operator** (≈ 400–600 lines + tests):
  - Needs nested nodes with a bracket that adds ≤ 1 coordinate per level. It must be detected per grid/call (cheb1 and linear growth fall back).
  - Needs a λ → slot index for arbitrary downward-closed sets: a trie with incremental walk, since the simplex rank fits complete grids only. The index is cached in `SparseGrid` or rebuilt per call (O(closure·d)).
  - Needs a work threshold (d2: 3 barriers per 6.8 µs call), a per-call store of distinct × (m·8 + 1) B, and a vector-valued variant.
  - Observable: the evaluation count (refcheck INFO) and the evaluation order.
  - It breaks the classical per-TG operator that the owner kept for global interpolation.
- **(c) A `DeltasSparseGrid` variant of `interpolateLinear`** (≈ the same code; recommended and **chosen by the owner on 2026-11-11**):
  - The DSG already guarantees nested nodes (it throws otherwise) and holds exactly the closure λ list. The index is built once with the grid; the CT coefficients come from the closure.
  - The `SparseGrid` operator stays classical, matching the WP-45 decision for global interpolation.
  - The "all" variant should not go into production: evaluating unused slots is observable.
- Refinement: the values depend on the query point, so neither (b) nor (c) reuses values across refinement steps. Per-TG (per-λ) contributions remain available in pass 3. A trie index can be extended incrementally, so neither option blocks refinement.

## Attempts not kept
- none in production. The first operator-level tolerance in the new test (one sum over all corners) was too tight for 4096 equal-weight corners and was fixed to per-TG sums. In the driver the dedup variants stay as measuring sticks.

## Profile shift / follow-ups
- The interplin kernel now sits at the integrand floor: only fewer evaluations help.
- **New WP (owner decision 2026-11-11: option (c))**: `interpolateLinear` on a `DeltasSparseGrid`, evaluating every distinct corner once per call.
  - Design: a λ index built once with the DSG (a trie or rank over the closure); CT coefficients from the closure; the marking pass (no evaluations at non-corner points); scalar and `VectorSourceFunc`.
  - Small calls: a work gate or the user's choice.
  - Expected (prototype): d10 1.84 → 0.55 s, d8 1.21 → 0.49 s, d5 0.36 → 0.25 s. The judged suite measures only the `SparseGrid` operator, so it needs its own benchmark cases (unprotected driver / micro), as WP-45.
- Pin the CT combination `result += c·v` of the operator drivers to `std::fma` (as `mapToDomain`), so that results do not depend on LTO inlining (`results/WP-08/bitwise.md`).
- interplin d8 at 16 t is bimodal in the identical binary (≈ 1.20 / 1.30 s per process, A/A `e2e-aa-d8.md`). It was not seen in the first A/B of this session. Check the memory layout / false sharing (cf. WP-23) before trusting single d8 rounds.
- d2 interplin is region-bound (libgomp 71 %, WP-36): a work gate or thread cap would save ≈ 20 ms.
- The public `linear_interpolation.hpp` includes the internal scratch header (and now `constants.hpp`) without using it; drop the include.
- WP-30: tg_vec d2 m4/m8 0.987–0.994x, and vector d10 m1 is 1.04x the scalar path. A pre-existing warning: `grids/tensor_grid.cpp:34` unused `sum` (NDEBUG).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; `--keep combigrid/tests/tools/operators/linear_interpolation/linear_interpolation_tools_test.cpp --keep combigrid/benchmarks/perf/perf_interp_explore.cpp --keep combigrid/benchmarks/perf/perf_micro.cpp` keeps the test and benchmarks)
