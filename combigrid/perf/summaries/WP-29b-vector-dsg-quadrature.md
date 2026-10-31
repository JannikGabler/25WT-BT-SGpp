# WP-29b — Vector-valued source functions, design T, step (b): DSG quadrature

Status: PROPOSED · Date: 2026-10-31 · Base commit: 984c646b5
Recommendation: accept — the agreed design-T extension for the DSG; scalar path unchanged (e2e `^quad/dsg/` 1.001x, 0 SLOWER, memory 0.999, scalar row loop instruction-identical to before, refcheck OK); vector run 1.41–2.96x faster than m scalar runs for m = 2–32 (up to 3.18x incl. sub-tries)
Machine: `perf.py calibrate` ratios e2e 1.003 / 1.000, micro 1.006 / 1.004 (burst / sustained)

## Change
- `tools::dsg_quadrature::processDeltaTensorGrid(context, SourceFunc | VectorSourceFunc, deltaTG, scratch[, out])`: both are one TU-local template `sumWithGlobalWeights<ValueType>` around the unchanged row loop `forEachGPWithGlobalWeight` (`noinline`, `aligned(64)`: both instantiations start 64-byte aligned). The trie partial sums and row weights (= global weight) are computed once per point for all m outputs. There is one accumulator slot and no per-row m work (DSG weights are per point). `values` is captured by value, so the scalar closure has the same layout as before.
- `misc::DeltaQuadratureContext` no longer holds the source function. `DeltaQuadratureScratch(nDim, maxSum, nOutputs = 0)` gets `valueBuffer` + `TG_SUM_ACC` / `N_ACCS` (empty, i.e. no allocation, for the scalar path). New: `combineDeltaTensorGrids` (per-thread m-vectors, `schedule(dynamic)` as the scalar driver, `critical`); public `quadrature(const VectorSourceFunc&, const DeltasSparseGrid&, std::span<double>)` + `base::DataVector` variant. A wrong size throws **`std::invalid_argument`**, like the SG overload since the owner's accept commit (the prompt said `base::data_exception`, which the SG code no longer throws).
- `updateCoeffSums` is now defined header-inline (unchanged body), so that the scalar instantiation compiles to the same instructions as before. Reason: see "Attempts not kept" (V1).
- Files: `operators/quadrature/deltas_sparse_grid_quadrature.{hpp,cpp}`, `tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.{hpp,cpp}`, `miscellaneous/data_contexts/delta_quadrature_context.hpp`, `miscellaneous/data_scratches/delta_quadrature_scratch.hpp`; tests `vector_quadrature_test.cpp` (new suite), `delta_quadrature_test.cpp` (call sites); `perf_micro.cpp`, `perf_vector_proto.hpp` (call sites + new cases); docs CLAUDE.md §2/§6, `module-reference.md` (3 lines). The umbrella header, `combigrid.i` and `python-bindings.md` are unchanged (the DSG is not bound).

## Gain vs. cost
- Gain (feature): **scalar path within noise**: e2e `^quad/dsg/` sum 17.748 → 17.738 s (1.001x, 0 SLOWER, 0 faster). Vector (1 thread, production kernels, `vector-gain.md`; gain = m scalar runs / one vector run, m = 1/2/4/8/32): **b0 d10 L7 0.95 / 1.62 / 1.87 / 2.41 / 2.96**; b1 d20 L6 (largest TG) 0.93 / 1.41 / 1.49 / 1.79 / 1.96; **b1 d20 L6 every 256th TG incl. sub-tries 0.96 / 1.71 / 2.00 / 2.64 / 3.18**. WP-24 rt in the same run: 0.94/1.45/1.81/2.32/2.80 and 0.91/1.30/1.44/1.73/1.91. All vector outputs are bitwise equal to the scalar runs per delta TG (max ulp 0). m = 1 is 4–7 % slower than `SourceFunc`, as expected (WP-30).
- Memory: geomean B/A 0.999, 0 flags (largest: b1 d15 150.5 → 153.0 MiB, 1.017, inside the spread). Floor 10.0 → 10.2 MiB.
- API / functionality: the agreed design-T extension only (owner 2026-10-30). Two `quadrature` overloads; internal: context field removed, `processDeltaTensorGrid` takes the function (micro call sites adapted, same meaning; `micro/dsg/sub_tries` lost an unused `SourceFunc`). `updateCoeffSums` is no longer an exported symbol (internal tools function). Python: unchanged (SWIG never sees the DSG header). Checked with a fast check (scratch module) + a full `SG_PYTHON=1` build (only SWIG warning 389). All 8 `combigrid/examples/python` scripts run; `pysgpp` has neither `VectorSourceFunc` nor DSG names.
- Maintainability: +226 / −36 lines production code incl. docs. One kernel for both value types. The slot scheme of WP-29a is used unchanged.

## Tests
- Pinning tests (full suite, debug, 3× on the unchanged code ✓): `delta_quadrature_test`, `quadrature_test`, `vector_quadrature_test`, `vector_source_function_test`, `source_function_test`.
- New suite `vector_delta_quadrature` in `vector_quadrature_test.cpp`. `Outputs_Equal_Scalar_Quadrature`: equi / cc / mixed, b0/b1/b2, d = 1–4, unit + shifted domain, complete L0/L3 + MIVec, m = 1/3, both overloads. Per delta TG: same points in the same order as the scalar run, \|Δ\| ≤ 16 eps · Σ\|w·f\| (global weights have both signs). Operator: ≤ 16 eps · vol · Σ\|w·f\|. Vector DSG == vector SG within max(1e-11, 64 eps Σ\|c\|).
- Also new: `_High_Dimensions` (d = 8/10, b1/b2, deep projected sub-tries with double-double coefficients), `Zero_Point_Tensor_Grid`, `Wrong_Result_Size_Is_Rejected`. Only doubling growth: linear growth is not nested, so `DeltasSparseGrid` rejects it (`Non_Nested_Nodes_Throw`).
- The suite runs in 0.5 s (normal). A first version took 31 s: its MI set contained (1,…,1), whose full grid at d = 10 has 5^10 points. It now uses a sparse anisotropic set.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ (final code). 31 full-suite runs in this session without a failure; the time-seeded `sin_func_random_cube_3D` (WP-35) did not fail.

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (`refcheck.txt`; V1: `refcheck-v1.txt`, OK).

## Benchmarks (A = WP-29b-before, B = WP-29b-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `micro.md`; sum 649.78 → 647.98 ms, 1.003x; all 9 cases within noise):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| dsg/process_delta_tg/equi/b0/d10/L7 | 67.10 ms | 66.39 ms | 1.012 | [0.998, 1.026] |
| dsg/process_delta_tg/equi/b1/d20/L6 | 124.10 µs | 124.20 µs | 1.001 | [0.996, 1.005] |
| dsg/process_delta_tgs/equi/b0/d10/L6/s64 | 285.33 ms | 284.40 ms | 1.006 | [0.996, 1.016] |
| dsg/process_delta_tgs/equi/b1/d20/L6/s256 | 55.50 ms | 55.34 ms | 1.000 | [0.983, 1.017] |
| dsg/sub_tries/equi/b0/d10/L7/s16 | 8.99 ms | 8.88 ms | 1.013 | [0.996, 1.031] |

L2/L3 e2e (16 threads, `e2e.md`, 9 cases, 0 SLOWER / 0 faster):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.778 s | 6.778 s | 1.000 | [0.994, 1.006] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.410 s | 5.400 s | 1.003 | [0.999, 1.006] |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.401 s | 5.392 s | 1.003 | [0.999, 1.007] |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.442 s | 1.437 s | 1.003 | [0.999, 1.006] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225 s | 1.224 s | 1.000 | [0.995, 1.005] |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.215 s | 1.223 s | 0.995 | [0.985, 1.006] |

Sum of medians (total, 9 cases): A = 17.748 s, B = 17.738 s, saved = 0.011 s (1.001x, within noise) · Reports: `combigrid/perf/results/WP-29b/`
WP-29b-before vs. RP-3 (owner's restructuring): sum 17.748 vs. 17.746 s (+0.01 %), per case −0.4 … +0.3 % → no `ab rp-3` needed. (V1 run: 17.824 s, +0.44 %; b0 d3 L16 +3.1 % in that run only.) `aligned(64)` survived the move (before: 0x40a40).

Peak memory: geomean B/A = 0.999 · 0 MEM+/MEM- · process floor A = 10.0, B = 10.2 MiB

## Attempts not kept
- **V1: `updateCoeffSums` left in the .cpp.** With two instantiations, LTO stopped inlining the call before the row loop in the scalar one (3900 → 3308 B), which moved the loop in its op-cache windows. Result: instructions −0.15 %, cycles +1.2 %, op-cache misses +9 % (`placement-v1-v2.md`). Also, `ProjectedSubTrieBuilder::childBlock` was no longer inlined into `addChildren` (`sub_tries` b0 0.975 SLOWER). e2e 0.992x, 5 SLOWER (b0 1.0–1.4 %, `e2e-v1.md`). V2 (final) defines it header-inline, which restores the identical scalar code, cycles and op-cache misses, and the `childBlock` inlining.

## Profile shift / follow-ups
- WP-04 candidates (not done, scope): `forEachGPWithGlobalWeight` takes the functor by reference. After every opaque `evaluate` it reloads the closure fields, and the scalar accumulator lives in the caller's frame. Taking the functor by value or keeping the accumulator in a register (both instantiations) would avoid that. Strict aliasing: nothing to simplify in the touched code (its hoisting is across the opaque call).
- WP-30: DSG vector path at m = 1 (0.93–0.96x).
- Placement: LTO inlining in the DSG unit shifts with unrelated code size (V1). If a later WP sees b0 shifts, check op-cache misses and `childBlock` inlining first; `[[gnu::always_inline]]` on `childBlock` is a candidate.
- Stale: `combigrid.i:245–246` `%ignore quadrature_operator::processTensorGrid/getWeights` refers to a namespace that no longer exists (WP-29d). Comments still name `delta_quadrature_operator::` (`delta_quadrature_context.hpp:35`, `delta_quadrature_scratch.hpp:26/31/57/67`, `projected_sub_trie_builder.hpp:9/25/51`) and `tools::{Scalar,Vector}Values` (`quadrature_scratch.hpp:29/38/42/105`, `scalar_values.hpp:11/13`, `quadrature_tools.cpp:220`). The WP-29a summary says `base::data_exception` for a wrong size, but the code throws `std::invalid_argument` (I corrected the module-reference line I updated). `quadrature.cpp` (SG) uses a `using namespace` (cpplint `build/namespaces` when touched; fixed in the DSG file). `tensor_grid.cpp:34`: unused variable `sum` warning in normal builds.
- Dimension-adaptive refinement: not blocked. The context without the function serves several functions. The vector run evaluates the same points in the same order as the scalar one (tested), so it has the same cache keys. Limits: DSG contributions are per delta TG with global weights (mixing the coefficients of all ℓ ≥ m), not per-ℓ increments, so error indicators come from the SG kernel. Weights and trie are rebuilt per call (`prepare` b1 d20: 114 ms at 1 thread); an incremental loop needs a trie with insertion.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; the new
test suite depends on the feature, so it goes along with it)
