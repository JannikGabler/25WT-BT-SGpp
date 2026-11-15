# WP-56 — SG quadrature: CC weights off the critical path (real FFT, eager weight table, scheduling)

Status: PROPOSED · Date: 2026-11-15 · Base commit: d426bddcf
Recommendation: accept — cc d2 L20 792 → 438 ms total (1.84x), equi b1 d10 L7 1.22x, mivec cc d10 1.32x, −551 ms over 26 cases, cc d2 L20 peak RSS −129 MiB; one small unexplained regression (equi d5 L10 +3 ms); weight change at round-off as agreed
Machine: `perf.py calibrate` ratios e2e 1.003 / 1.001, micro 0.997 / 0.998 (start); 1.001 / 1.000, 0.998 / 0.999 (before the final A/Bs)

## Change
- (a) `ClenshawCurtisQuadRule::genWeightsFFTInplace`: Waldvogel's length-2m input is zero at odd indices, so its bins are **one real FFT of length m = n − 1** of the even entries (built in `out` itself): a quarter of the transform and temporaries. Only bins k ≤ m/2 are used; each weight is written with its mirror (bitwise symmetric, no reversal pass). Length 1 (2 nodes) bypasses Eigen's kissfft (fails for n = 1). Naive-path TODOs documented, banner added.
- (b) `misc::QuadWeightTable::computeAllRows()`: orphaned `omp for schedule(dynamic, 1) nowait` over the rows sorted by size (largest first, sorted in the constructor); called by all threads at the start of `quadrature` and `combineTensorGrids` (vector). The lazy `call_once` stays as the wait / fallback. TG loop `schedule(guided)` → `schedule(dynamic)` in both.
- (c) former WP-19 (slabs): implemented, measured, dropped on the owner's decision (code complexity), see Attempts.
- Files: `clenshaw_curtis_quadrature_rule.{cpp,hpp}`, `quadrature_weight_table.{hpp,cpp}`, `quadrature.cpp`, `quadrature_tools.cpp`, `perf_micro.cpp` (+2 cases), tests (below), `combigrid/claude/module-reference.md` (table, quadrature parallelism, CC rule).

## Gain vs. cost
- Gain: `quad/sg/cc-uq/b0/d2/L20` total 792 → 438 ms (1.84x; operator 716 → 356 ms, 2.04x), `equi-exp/b1/d10/L7` 1.22x, `mivec/sg/cc-exp/b1/d10` 1.32x, dsg cc d5 L12 1.01x (weights only). Sum over 26 `^(quad|mivec)/` cases 27.79 → 27.24 s (−551 ms). `quad/sg/equi-exp/b0/d5/L10` SLOWER: +3 ms of 262 (0.981 [0.966, 0.996], 8 rounds); `WP-56-eager-dyn` = `WP-56-after` on it (1.000), perf stat (3 processes) before / after: cycles 1.001, instructions 0.999, op-cache misses 0.997, task-clock 1.001; guided vs. dynamic and lazy vs. eager were each neutral on it → unexplained, minimal.
- Memory: geomean B/A 0.981; MEM- `cc-uq/b0/d2/L20` 323.4 → 194.6 MiB (0.602), `cc-uq/b0/d3/L14` 17.1 → 15.3 MiB (0.897); largest B/A `equi-exp/b0/d2/L20` 1.027 (unflagged). 0 MEM+.
- API / functionality: CC weights change at round-off (agreed 2026-11-15): max |Δw| 4.2e-22 at n = 2^21+1, errors vs. a high-precision reference the same size old / new (below). No public API change, no umbrella header touched (bindings unaffected).
- Maintainability: CC rule shorter (one pass after the FFT); `computeAllRows` ≈ 25 lines + docs in one class, two call sites.

## Tests
- Pinning (passed 3× on the unchanged code): `quadrature_weights/*`, CC rule tests, `grid_storage_pinning`, `equidistant_nodes` / `clenshaw_curtis_nodes` quadrature, `vector_quadrature`, `quadratureDelta_*`; new before the change: `weights_match_long_double_closed_form` (27 sizes n = 2…2049, ≤ 8 ε/m), `large_rules_are_symmetric_and_exact_on_chebyshev_polynomials` (n = 4096, 4097, 4098, 2^15+1, 2^17+1).
- New: `quadrature_weights/Eager_Fill_Equals_Lazy_Fill` (cc / mixed, outside a region, 2 and 16 threads: bitwise the lazy weights). Re-pinned: `CC_PIN_HASH_DEBUG`; `grid_storage_pinning` bitwise hashes of the cc / mixed configurations (55 debug, 50 opt of 252; point hashes and interpolation values unchanged; `results/WP-56/repin_grid_storage.py`, `golden-*.txt`). The CC hash check now requires `USE_EIGEN` (Armadillo has other round-off).
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ · Armadillo-only FFT (slab variant, same FFT code): CC tests ✓; `grid_storage_pinning` fails on the 55 Eigen-recorded cc / mixed hashes (as at HEAD, not re-verified there). Known flaky test did not recur.

## Reference check
- `perf.py refcheck`: WARN (1 and 16 threads), only the known unrelated `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34, also in WP-56-before); 131 OK. `results/WP-56/refcheck.txt`.
- CC weights (`cc-weights-accuracy.md`; closed form in mpmath for n ≤ 129, 80-bit long double for all n): max abs error old / new 2.0e-18 / 2.0e-18 (n = 129), 3.5e-19 / 3.8e-19 (1025), 2.3e-22 / 3.5e-22 (2^20+1), 2.2e-22 / 1.6e-22 (2^21+1); max rel error identical (boundary weights ≈ 1/m²).

## Benchmarks (A = WP-56-before, B = WP-56-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quadrule/cc_weights/n33 | 1.93us | 955.6ns | 2.023 | [2.010, 2.035] |
| micro/quadrule/cc_weights/n32769 | 3.18ms | 1.41ms | 2.262 | [2.254, 2.271] |
| micro/quadrule/cc_weights/n1048577 (new) | 145.35ms | 69.67ms | 2.089 | [2.053, 2.126] |
| micro/quad/operator/cc/b0/d2/L16 | 122.38ms | 112.13ms | 1.100 | [1.079, 1.121] |
| micro/quad/operator/equi/b0/d2/L16 | 101.70ms | 100.57ms | 1.011 | [1.007, 1.015] |
| micro/quad/process_tg/cc/b0/d2/L16 | 5.91ms | 5.91ms | 1.001 | ~ |

L2/L3 e2e (16 threads):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | total / operator | 791.50 / 715.66ms | 438.15 / 355.83ms | 1.843 / 2.042 | [1.690, 2.009] / [1.883, 2.215] |
| quad/sg/equi-exp/b1/d10/L7 | total | 300.12ms | 247.95ms | 1.215 | [1.170, 1.262] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 490.17ms | 376.13ms | 1.318 | [1.220, 1.424] |
| quad/sg/equi-exp/b0/d2/L20 | total (8 rounds) | 341.29ms | 347.73ms | 0.988 | [0.917, 1.063] ~ |
| quad/sg/equi-exp/b0/d5/L10 | total (8 rounds) | 262.08ms | 265.10ms | 0.981 | [0.966, 0.996] SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.99ms | 76.34ms | 1.004 | ~ |

Sum of medians (26 cases): A = 27.793 s, B = 27.241 s, saved = 551 ms (1.020x) · `results/WP-56/{micro,e2e,e2e-recheck-equi}.md`. Floor 10.3 / 10.4 MiB.

## Attempts not kept
- Slabs (former WP-19): TGs ≥ 2^20 points summed in ≈ 2^18-point slabs along the largest dimension as OpenMP tasks, slab sums in slab order (≈ 250 lines incl. docs, 3 tests). Same-session A/B with slabs vs. before (`e2e-slabs.md`): cc d2 L20 total 776 → 372 ms (2.12x), equi d2 L20 1.15x, mivec d10 1.45x, −704 ms; vs. eager + dynamic without slabs (4 rounds): equi d2 L20 operator 315 → 245 ms, cc d2 L20 358 → 304 ms. Dropped by the owner (2026-11-15): complexity vs. ≈ 60 + 70 ms on two low-d cases (+ ≈ 25–35 ms on mivec d5 / d10, cross-run estimate). Snapshot `WP-56-after-slabs`.
- Lazy table + dynamic (option 3): 8 rounds vs. eager, all 26 cases (`e2e-lazy-vs-eager-noslabs.md`): cc d2 L20 operator 421 vs. 361 ms (eager 1.153 [1.079, 1.231]; lazy rounds 384–458 ms), cc d7 L7 0.982 [0.966, 0.999] (+4 ms, rows ≤ 257 nodes: placement presumed), rest ~; sum 72 ms. With slabs, lazy beat eager (0.977, 8 rounds), hence the slab variant was lazy.
- `schedule(guided)`: dynamic faster on equi b1 d10 L7 1.192, mivec d10 1.097, b1 d15 1.028 (with slabs, `e2e-guided-vs-dynamic-slabs.md`).
- Not tried: reused FFT plan (needs a persistent plan cache, ≈ 24 MB at 2^21 beyond the call); TG ordering.

## Profile shift / follow-ups
- cc d2 L20 (`timeline/cc_uq_b0_d2_L20-WP-56-after.txt`): one thread computes the level-20 row (≈ 205 ms under load, ≈ 70 ms of it kernel / page faults; 70–140 ms alone) and then its 6.3 M-point TG (≈ 110 ms); several threads idle during the last 100–200 ms. Only splitting that TG removes it (slabs, dropped).
- Global interpolation (`interpolate`, vector `combineTensorGrids`) still uses `schedule(guided)` over the TGs per query point; dynamic gave up to 1.22x in the quadrature → small WP with an A/B on `^interp/sg/`. A shared per-call basis-value table (like `QuadWeightTable`) is not worth it: basis values are O(n) (CC) per TG and dimension, the generic O(n²) path ≈ 3 ms judged (RP-4).
- DSG `comp1DWeights` is serial and recomputes the rows of dimensions sharing node lists; share them via the unit node table (a few ms at dsg cc d5 L12).
- SG quadrature's `reduction(+)` over threads is order-dependent (pre-existing); summing per-TG results in TG order would make results bitwise thread-count independent (owner decision).
- Armadillo-only builds: `grid_storage_pinning` bitwise hashes are Eigen-specific (could get the same `USE_EIGEN` guard).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
