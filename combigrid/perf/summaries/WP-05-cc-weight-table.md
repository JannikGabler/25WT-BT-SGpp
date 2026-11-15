# WP-05 — SG quadrature: shared, lazily filled 1D weight table; no per-thread `weights1D`

Status: PROPOSED · Date: 2026-11-15 · Base commit: 14cf31e23
Recommendation: accept: `quad/sg/cc-uq/b0/d2/L20` runs 1.555x faster (−460 ms) and peak memory drops 342 → 110 MiB (equi d2 L20) and 862 → 310 MiB (cc d2 L20). The e2e sum improves 1.010x. The one SLOWER case (DSG cc d10, 0.978) is code placement (same instruction count, 11x op-cache misses). Results are bitwise the same weights in debug builds; in the optimized build they differ at round-off only (refcheck = before).
Machine: `perf.py calibrate` ratios e2e 1.003 / 0.999, micro 0.997 / 1.000 (start); 1.005 / 0.999, 1.000 / 1.002 (before the A/Bs)

## Change
- **`misc::QuadWeightTable`** (new, `miscellaneous/data_contexts/quadrature_weight_table.{hpp,cpp}`). It holds one weight list per row of the grid's unit node table. The weights depend only on the rule and the node count, and rows are shared by dims with equal `NodeGenFunc*` + growth. A row is computed on first use (`weights(dim, lvl)`) by the thread that needs it, which also allocates it: an acquire-load fast path, then `std::call_once`. Threads that need the row meanwhile wait, and unused rows are never computed. Lazy instead of eager+barrier: the largest CC row (n = 2^21+1) costs 356–455 ms in one FFT, so an eager build would idle 15 threads behind it. The table lives for one `quadrature()` call. `QuadratureContext` now holds `{sparseGrid, weightTable}` (instead of `nodeGenFuncs`).
- **Scratch**: `WeightedSumScratch::weights1D` (`DataVector(maxTGSumOverGPCntsPerDim)`, zero-filled per thread and call: 16 × 16.8 MiB at d2 L20) is replaced by `weightsOfDim` (nDim pointers). The SG quadrature points it into the table (`tools::quadrature::getWeights`, a lookup per dimension instead of a virtual `getQuadRule` + generation per TG and dimension). The global interpolation keeps its buffer, now `GlobalInterpScratch::basisValues` (same size and behavior as before), and sets the pointers. `setUpIterDims` reads `weightsOfDim`. `QuadratureScratch(nDim, nOutputs)` lost the size parameter and its unused `quadRules`. The now-unused `getQuadRulesForTensorGrid` / `getQuadRulesForAllDims` were removed.
- **CC rule** (`clenshaw_curtis_quadrature_rule.cpp`): `genWeightsInplace` writes directly into the output (private `genWeightsFFTInplace`; was a `DataVector` + copy). The temporaries drop from A, b, B(2m complex), s, wcc, w01 to b + B(m+1 complex): Eigen `HalfSpectrum`, which computes the same first m+1 bins and skips the reflection. The arithmetic and loop structure are the same; old and new sources give bitwise identical weights under the same flags (see Reference check). `micro/quadrule/cc_weights/n32769` 1.38x.
- Docs: Doxygen of all touched types, `combigrid/claude/module-reference.md` (data_contexts / data_scratches).
- Files: new `miscellaneous/data_contexts/quadrature_weight_table.{hpp,cpp}`, `tests/operators/quadrature/quadrature_weights_test.cpp`. Changed: `quadrature_context.hpp`, `{weighted_sum,quadrature,global_interpolation}_scratch.hpp`, `operators/quadrature/quadrature.cpp`, `tools/operators/quadrature/quadrature_tools.{hpp,cpp}`, `tools/operators/weighted_sum/weighted_sum_tools.{hpp,cpp}`, `tools/operators/global_interpolation/global_interpolation_tools.{hpp,cpp}`, `clenshaw_curtis_quadrature_rule.{hpp,cpp}`, tests `quadrature_test.cpp` / `vector_quadrature_test.cpp` (context/scratch construction), `perf_micro.cpp`. No public / umbrella header changed (no Python rebuild needed).

## Gain vs. cost
- Gain (16 t): `quad/sg/cc-uq/b0/d2/L20` 1.237 s → 777 ms (1.555x, −460 ms; operator 1.612x), `cc-uq/b0/d3/L14` 1.113x (−7.6 ms), `cc-uq/b0/d5/L10` 1.014x, `cc-uq/b0/d10/L4` 1.010x (−10 ms). `quad/sg/equi-exp/b0/d2/L20` 1.071x (~, CI [0.960, 1.195]). High d (equi b1 d10–d20, mivec) within noise (0.985–1.028). e2e sum 38.028 → 37.660 s (1.010x, −368 ms). Interpolation (shared scratch) neutral in e2e and micro.
- SLOWER: `quad/dsg/cc-uq/b0/d10/L7` 0.979 (+182 ms), recheck with 8 rounds 0.978. The DSG kernel and node generation are unchanged; only the CC weight bits feed it (`comp1DWeights`). `perf stat`: identical instructions (493.1e9) and branch misses, op-cache misses 0.16e9 → 1.9e9 (`results/WP-05/perf-stat-dsg-cc-d10.txt`). This is code placement after the LTO re-link; the same kernel on equi d10 L7 is 0.999. Three generation timings below 1.3 ms are SLOWER (code untouched; page-fault noise).
- Memory: geomean B/A 0.938, 5 MEM−, 0 MEM+: sg equi d2 L20 341.7 → 110.0 MiB, sg cc d2 L20 862.2 → 310.0 MiB, cc d3 L14 33.8 → 17.2, cc d5 L10 14.2 → 12.9, mivec cc b1 d5 14.6 → 13.4 MiB.
- API / functionality: none (internal `misc` / `tools` types only). Numerical: bitwise in debug; optimized CC weights differ at round-off because of FMA contraction in the re-compiled FFT (see below).
- Maintainability: +1 small class (≈ 60 lines with docs). The quadrature scratch shrinks. The CC rule is split into two helpers, and its "TODO: Optimize" comments are gone with the rewrite.

## Tests
- Pinning (passed 3× on the unchanged code): new `quadrature_weights/Tensor_Grids_Use_The_Rule_Weights_Bitwise` (equi/cc/mixed per dim, doubling/linear, offsets 0/1/2, unit/unequal shifted domains, d1–3, complete + MIVec up to CC n = 129, every TG and dim bitwise vs. `getQuadRule(n)->genWeights(n)`); `quadrature_weights/Clenshaw_Curtis_Weights_Bitwise` (debug golden hash for n = 2..32769); existing `grid_storage_pinning` (SG quadrature bitwise, scalar + vector), `tools_quadrature_tensor_grids`, CC rule, vector quadrature, caching suites.
- New after the change: `Weight_Table_Shares_Rows_Of_Dimensions_With_The_Same_Rule` (pointer identity for shared rows, own rows for other families, unequal domains); `Weight_Table_Is_Thread_Count_Independent` (2/4/16 threads request all rows concurrently in thread-dependent orders: bitwise equal to the serial table, CC n up to 8193).
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ (memory-capped) · flaky test not observed. Armadillo backend and no-FFT fallback built standalone ✓ (`results/WP-05/cc_weights/README.md`).

## Reference check
- `perf.py refcheck` (1 and 16 threads): 131 OK, 1 WARN, `interp/sg/equi/b0/d2/L5/cos-shifted` x2.34. This is the known WARN since WP-06, identical in `WP-05-before`.
- CC weights (`results/WP-05/cc_weights/`): debug hash unchanged. Old vs. new source under the same flags: 0 differing weights for n ≤ 2^21+1. The optimized library differs by ≤ 4.4e-16·(1/m) absolute (the WP-29a LTO effect). **Optional question answered**: with `-ffp-contract=off` for this TU, the weights are bitwise identical at -O0, -O2, -O3 and -O3+LTO; contraction is the only build dependence. Not applied: it would change today's optimized weights by ≤ 6.7e-16/m, so it is a flag decision for the owner.

## Benchmarks (A = WP-05-before, B = WP-05-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `micro/quad/operator/*` new: the whole `quadrature()` call):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/operator/cc/b0/d2/L16 | 172.05 ms | 122.11 ms | 1.409 | [1.404, 1.415] |
| quad/operator/equi/b0/d2/L16 | 100.84 ms | 101.66 ms | 0.999 | [0.979, 1.019] |
| quad/operator/cc/b1/d10/L5 | 52.35 ms | 44.37 ms | 1.194 | [1.163, 1.226] |
| quad/operator/equi/b1/d20/L4 | 109.41 ms | 106.79 ms | 1.034 | [1.015, 1.054] |
| quad/process_all_tgs/cc/b1/d10/L5 | 52.72 ms | 44.43 ms | 1.187 | [1.176, 1.198] |
| quad/process_all_tgs/equi/b1/d20/L4 | 109.52 ms | 107.11 ms | 1.022 | [1.017, 1.027] |
| quad/process_tg/cc/b0/d2/L16 | 24.64 ms | 5.92 ms | 4.164 | [4.144, 4.184] |
| quad/process_tg/equi/b0/d10/L4 | 14.01 ms | 14.12 ms | 0.986 | [0.973, 0.999] (repeat: 0.983 ~) |
| quadrule/cc_weights/n32769 | 4.38 ms | 3.17 ms | 1.382 | [1.376, 1.387] |
| quadrule/cc_weights/n33 | 2.14 µs | 1.93 µs | 1.104 | [1.089, 1.119] |

`process_tg` / `process_all_tgs` now build the weight table in setup, so the first measured call fills it and later calls only look weights up. They measure the per-TG cost, which no longer includes weight generation. `process_tg` equi d5 / d10 and cc d10 were 0.986–0.994 (SLOWER) in the first run and within noise in a repeat (`micro-interp-controls.md`, with 9 interp and 6 untouched control cases, all ~). The row loop is unchanged.

L2/L3 e2e (16 threads, full suite, 5 rounds):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | total | 1.237 s | 777.1 ms | 1.555 | [1.449, 1.668] |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.159 s | 707.2 ms | 1.612 | [1.500, 1.733] |
| quad/sg/cc-uq/b0/d3/L14 | total | 84.46 ms | 76.88 ms | 1.113 | [1.052, 1.178] |
| quad/sg/cc-uq/b0/d5/L10 | total | 323.98 ms | 319.47 ms | 1.014 | [1.001, 1.027] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.324 s | 1.314 s | 1.010 | [1.005, 1.016] |
| quad/sg/equi-exp/b0/d2/L20 | total | 370.16 ms | 336.90 ms | 1.071 | [0.960, 1.195] |
| quad/sg/equi-exp/b1/d20/L5 | total | 215.10 ms | 213.24 ms | 1.008 | [0.995, 1.021] |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.887 s | 7.069 s | 0.979 | [0.961, 0.996] (8 rounds: 0.978) |

Sum of medians (38 cases): A = 38.028 s, B = 37.660 s, saved = 368 ms (1.010x) · Full reports: `combigrid/perf/results/WP-05/`

Peak memory:

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 862.2 MiB | 310.0 MiB | 0.359 | MEM- |
| quad/sg/equi-exp/b0/d2/L20 | 341.7 MiB | 110.0 MiB | 0.322 | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 33.8 MiB | 17.2 MiB | 0.508 | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 14.2 MiB | 12.9 MiB | 0.908 | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 14.6 MiB | 13.4 MiB | 0.923 | MEM- |

Geomean B/A = 0.938 · largest B/A `quad/dsg/equi-exp/b0/d7/L10` 1.020 (~) · process floor A = 10.5 MiB, B = 10.2 MiB

## Attempts not kept
- Eager table filled by an `omp for` before the TG loop (with an implicit barrier): not implemented. The measured single-call cost of the largest CC row (n = 2^21+1: 455 ms before, 356 ms after) would put 15 threads idle behind one FFT. The lazy table puts that FFT on the thread that needs the row first, as the per-TG computation did before, and removes the redundant FFTs on the other threads.

## Profile shift / follow-ups
- cc d2 L20: the single FFT for the largest row (≈ 356 ms of the 707 ms operator) is now the critical path. Ideas: a DCT-I / half-length real FFT of length m (changes bits at round-off), parallel FFT for huge rows, or computing it while other TGs run (WP-19 load balancing).
- FMA reproducibility of the CC weights: compile `clenshaw_curtis_quadrature_rule.cpp` with `-ffp-contract=off` (or a pragma) → build-independent weights (round-off change; owner decision).
- `DeltaWeightedSumScratch::rowWeights` is the remaining zero-filled per-thread buffer: `DataVector(maxTGSumOverGPCntsPerDim)`, but only one row length is used (`compRowWeights` zeroes `nGP0`). It could grow on demand like `rowNodes`. Small for the suite's DSG cases (d ≥ 3, ≤ ~1–2 MiB/thread), 16 MiB/thread for a d2 L20 DSG.
- `GlobalInterpScratch::basisValues` is still zero-filled with the max TG sum per thread (as before): the same page-fault cost at d2 for the SG interpolation.
- Hygiene: `clenshaw_curtis_quadrature_rule.{hpp,cpp}` lack the copyright banner (cpplint warning); `quadrature_context.hpp` has "TODO: Document" as its file comment; the commented-out `fast_quadrature.cpp` still references `getQuadRulesForTensorGrid` (removed).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep combigrid/tests/operators/quadrature/quadrature_weights_test.cpp` to keep the CC-weight pin; its
weight-table tests need the new type, the `Tensor_Grids_Use_The_Rule_Weights_Bitwise` helper must
then use the old `getWeights` / `weights1D`)
