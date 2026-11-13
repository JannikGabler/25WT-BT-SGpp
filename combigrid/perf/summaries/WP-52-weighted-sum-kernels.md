# WP-52 — Shared weighted-sum kernels: operator-neutral names and place

Status: PROPOSED · Date: 2026-11-13 · Base commit: 85106468a (WP-06; committed during the session, same content as the staged tree the before snapshot was built from)
Recommendation: accept — refactoring as requested; 1-thread results byte-identical, e2e sum 0.999x (one interp/sg case −1.0 %, op-cache placement at identical instructions)
Machine: `perf.py calibrate` ratios e2e 1.001 / 1.000, micro 0.994 / 0.998 (session start; before the A/Bs 1.001 / 1.001, 0.999 / 1.002)

## Change
- SG: the row loop and its scratch move from quadrature to `tools/operators/weighted_sum/weighted_sum_tools.{hpp,cpp}` (namespace `tools::weighted_sum`) and `misc::WeightedSumScratch`. `QuadratureScratch` = `{WeightedSumScratch rows; quadRules}` (constructor unchanged), symmetric to `GlobalInterpScratch` = `{rows; nodes1D}`. Explicit instantiations of `sumOverRows` (both value types) stay in the new `.cpp`.
- DSG: the global-weight kernel moves to `weighted_sum/deltas_sparse_grid_weighted_sum_tools.{hpp,cpp}` (namespace `tools::dsg_weighted_sum`) together with `ProjectedSubTrieBuilder`. `[[gnu::noinline, gnu::aligned(64)]]` on `forEachGPWithGlobalWeight`, header-inline `updateCoeffSums`, hidden visibility of the builder are kept. The delta → regular node-index map moves to `tools/sparse_grid_generation/deltas_sparse_grid_generation.{hpp,cpp}`.
- Quadrature keeps `processTensorGrid`, `combineTensorGrids`, `prepareTensorGrid`, `getWeights`, `getQuadRules*`, `comp1DWeights`, `combineDeltaTensorGrids`. Function bodies are moved verbatim. Doxygen (file docs of both kernels, scratches, users) says quadrature uses quadrature weights and global interpolation uses 1D basis values. Stale cpplint items fixed where code moved: the `TODO: Document` blocks in `quadrature_tools.hpp` are documented, the `TODO: Delete` assert is removed, final newlines added, and `deltas_sparse_grid_generation.{hpp,cpp}` got the copyright banner and its wrong `@file` name.
- Follow-up on the owner's review (same session; snapshot `WP-52-types-inline`): the nested types of `DeltaWeightedSumScratch` move out (one type per file). `TrieNode` → `misc::ProjectedTrieNode` (`data_scratches/projected_trie_node.hpp`). `ProjLeafSlot` and the builder's private `DoubleDouble` (both `{hi, lo}`) become one `misc::DoubleDouble` (`miscellaneous/numerics/double_double.hpp`, the owner's choice of place) with `operator*(double)` (FMA), `operator+=` (TwoSum, was `ProjectedSubTrieBuilder::addTo`) and `value()`; header-inline (see Attempts). The builder's anonymous namespace becomes private members (`static deltaNodesStart`, alias `Weights1DOfDim`; `NO_BLOCK` spelled `DeltaWeightedSumScratch::NO_BLOCK`). Kept: `sumWithGlobalWeights` stays in the anonymous namespace (TU-local helper per CLAUDE.md, around the placement-sensitive DSG row loop) and `ProjectedSubTrieBuilder::Path` stays nested (private search state).
- **Renames (old → new)**: `misc::QuadratureScratch` (row-loop part) → `misc::WeightedSumScratch` (`TG_SUM_ACC`/`ROW_SUM_ACC`/`N_ACCS` with it); `tools::quadrature::{sumOverRows, mapNodesOntoDomain, setUpIterDims, findRowDim, updateWeightProducts}` → `tools::weighted_sum::…`; `misc::DeltaQuadratureScratch` (`delta_quadrature_scratch.hpp`) → `misc::DeltaWeightedSumScratch` (`delta_weighted_sum_scratch.hpp`); `tools::dsg_quadrature::{processDeltaTensorGrid, forEachGPWithGlobalWeight, compSubTrie, prepareDeltaTensorGrid, updateCoeffSums, compRowWeights}` (+ TU-local `sumWithGlobalWeights`) → `tools::dsg_weighted_sum::…`; `tools::ProjectedSubTrieBuilder`: `tools/operators/quadrature/` → `tools/operators/weighted_sum/` (name kept); `tools::dsg_quadrature::mapNodeIdx` → `tools::deltas_sparse_grid_generation::mapDeltaNodeIdxToRegular`; `tools::dsg_quadrature::compNodeIdxLvlMapFuncs` → `tools::deltas_sparse_grid_generation::compNodeIdxLvlMapFuncs`; `misc::DeltaWeightedSumScratch::TrieNode` → `misc::ProjectedTrieNode`; `misc::DeltaWeightedSumScratch::ProjLeafSlot` and `tools::ProjectedSubTrieBuilder::DoubleDouble` → `misc::DoubleDouble`; `ProjectedSubTrieBuilder::addTo(slot, term)` → `slot += term`; TU-local `deltaNodesStart` → private `ProjectedSubTrieBuilder::deltaNodesStart`.
- Files: new `miscellaneous/data_scratches/{weighted_sum_scratch,delta_weighted_sum_scratch,projected_trie_node}.hpp`, `miscellaneous/numerics/double_double.hpp`, test `tests/miscellaneous/numerics/double_double_test.cpp`, `tools/operators/weighted_sum/{weighted_sum_tools,deltas_sparse_grid_weighted_sum_tools,projected_sub_trie_builder}.{hpp,cpp}`; removed `delta_quadrature_scratch.hpp`, `quadrature/projected_sub_trie_builder.{hpp,cpp}`; changed `quadrature_scratch.hpp`, `global_interpolation_scratch.hpp`, `quadrature_tools.{hpp,cpp}`, `deltas_sparse_grid_quadrature_tools.{hpp,cpp}`, `global_interpolation_tools.{hpp,cpp}`, `deltas_sparse_grid_global_interpolation_tools.{hpp,cpp}`, `deltas_sparse_grid_global_interpolation_index.cpp`, `deltas_sparse_grid_quadrature.cpp`, `deltas_sparse_grid_generation.{hpp,cpp}`, tests `delta_quadrature_test.cpp`, `vector_quadrature_test.cpp` (names / includes only), `perf_micro.cpp` (names only; case names unchanged), CLAUDE.md §2, `combigrid/claude/module-reference.md`.

## Gain vs. cost
- Gain: none intended. e2e `^(quad/|interp/)` (36 cases): all `~`, sum 32.069 → 32.101 s (0.999x). Re-run of `interp/sg/` with 8 rounds: `interp/sg/cc/b0/d10/L2/p8` 0.990 [0.984, 0.996] SLOWER (+16 ms of 1.6 s), the other 7 `~`, sum 0.996x. Cause: placement at identical instructions (op-cache misses +20 %; see Profile shift).
- Memory: unchanged (geomean B/A 0.998, no flag; floor 10.4 / 10.2 MiB).
- API / functionality: none. No umbrella-header header changed. As a precaution the bindings were rebuilt with `SG_PYTHON=1` (via `.venv`) and all 11 `combigrid/examples/python/` scripts pass.
- Maintainability / readability: shared code now sits in a place and under names that say what it is. The quadrature files shrink to their quadrature-specific parts. Sources: +227 / −1272 lines in existing files, +1294 lines in the 8 new files (net +249, mostly Doxygen).

## Tests
- Pinning tests (passed 3× on the unchanged code): `tools_quadrature_tensor_grids`, `equidistant_nodes`, `clenshaw_curtis_nodes`, `quadratureDelta_operator`, `quadratureDelta_internals`, `vector_quadrature`, `vector_delta_quadrature`, `interpolation_kernel_pinning`, `interpolation_pinning`, `dsg_global_interpolation`, `vector_global_interpolation`, `equidistant_node_interpolation`, `clenshaw_curtis_node_interpolation`, `NodeGenFunc_node_index_maps` (full suite run).
- New / extended tests: `double_double` (6 cases: exact FMA / TwoSum error terms, low parts, a sum that cancels in double, `value`); otherwise only names / include paths changed.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan n.a. (no memory / lifetime change). The known flaky case did not recur. Types follow-up: debug and normal `--repeat 3` ✓ (final variant); 2 of ~50 further full-suite runs failed once each. The one caught (30-run debug loop): `equidistant_nodes/` and `clenshaw_curtis_nodes/linear_func_unit_cube_1D`, seed 1794588878, relative difference 1.6e-14 vs tolerance 1e-14 (`a/2 + b` = 0.0079 cancels). Pre-existing: replayed with that seed, the pre-WP-52 debug build (HEAD, same flags incl. Eigen / Armadillo) gives the bitwise same results; normal builds before / after are bitwise identical too. Another time-seeded test with a relative tolerance near 0 (see follow-ups).

## Reference check
- `perf.py refcheck` (also of `WP-52-types-inline`, `results/WP-52/refcheck-types.txt`): WARN, identical to WP-52-before (131 OK, 1 WARN `interp/sg/equi/b0/d2/L5/cos-shifted` x2.34, present before this WP). Raw `perf_reference` output byte-identical at 1 thread (WP-52-after and WP-52-types-inline vs WP-52-before). At 16 threads, A/A differs as much as A/B (scheduling-order reassociation). `results/WP-52/placement-and-bitwise.md`.

## Benchmarks (A = WP-52-before, B = WP-52-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; 46 cases: 37 spot cases + 9 `_vec` m4; 44 `~`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/interp/sg/cc/b0/d8/L2 | 126.53ms | 128.62ms | 0.983 | [0.980, 0.987] SLOWER |
| micro/quad/process_tg/cc/b0/d10/L4 | 14.03ms | 13.93ms | 1.007 | [1.003, 1.011] faster |
| micro/quad/process_tg/equi/b0/d10/L4 | 13.98ms | 13.90ms | 1.006 | [0.994, 1.018] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 66.06ms | 66.06ms | 1.001 | [0.992, 1.009] |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | 56.06ms | 55.60ms | 1.017 | [0.983, 1.051] |
| micro/interp/global_tg/cc/b0/d5/L6 | 911.37us | 910.38us | 1.003 | [0.990, 1.016] |
| micro/interp/dsg/cc/b0/d8/L2 | 35.09ms | 35.04ms | 1.000 | [0.994, 1.006] |

Sum of medians: 2.529 s → 2.519 s (1.004x).

L2/L3 e2e (16 threads; largest cases and the outliers):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.846s | 6.853s | 0.998 | [0.992, 1.004] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.363s | 5.364s | 1.000 | [0.997, 1.004] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.402s | 2.407s | 0.998 | [0.989, 1.006] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.320s | 1.320s | 0.999 | [0.988, 1.010] |
| interp/sg/cc/b0/d10/L2/p8 (8 rounds) | total | 1.599s | 1.615s | 0.990 | [0.984, 0.996] SLOWER |
| interp/sg/cc/b0/d5/L6/p64 (8 rounds) | total | 1.128s | 1.126s | 1.002 | [0.999, 1.006] |
| interp/dsg/cc/b0/d10/L2/p8 | total | 393.17ms | 392.62ms | 0.997 | [0.984, 1.011] |

Sum of medians (36 cases): A = 32.069 s, B = 32.101 s, saved = −32 ms (0.999x) · Full reports: `combigrid/perf/results/WP-52/` (`micro`, `e2e`, `e2e-interp-sg-r8`)

Peak memory: no flags; largest B/A `interp/dsg/cc/b0/d2/L12/p64` 16.1 → 16.4 MiB (1.015, inside spread). Geomean B/A = 0.998 · process floor A = 10.4 MiB, B = 10.2 MiB

Types follow-up (A = WP-52-after, B = WP-52-types-inline): micro, 15 scalar DSG cases (`sub_tries`, `prepare`, `process_delta_tg(s)`, `interp/dsg`): 14 `~`, `process_delta_tgs/equi/b1/d15/L7/s128` 0.992 [0.986, 0.998]; repeated with 8 rounds (`sub_tries` + `process_delta_tgs`, 6 cases): all `~`, sum 1.000x (`micro-types-r8`). e2e `^(quad|interp)/dsg/` (15 cases): all `~` (total, operator, generation), sum 18.645 → 18.646 s (1.000x); memory geomean 1.001, no flag. Builder code unchanged in size (`addChildren` 3254 B, `appendProjectedChildren` 1366 B; `build` 15003 → 15035 B), no `DoubleDouble` symbol left.

## Attempts not kept
- `aligned(64)` on `sumOverRows` against the placement shift: not tried (beyond a refactoring; see follow-ups).
- `DoubleDouble` operations defined in `double_double.cpp` (the default per CLAUDE.md): LTO left 15 calls of `operator*` in `ProjectedSubTrieBuilder::build` (into which it inlines `addChildren`) out of line; micro `sub_tries/equi/b1/d20/L6/s256` 0.936 [0.928, 0.943] SLOWER, `process_delta_tgs` d20 / d15 0.981 / 0.984 SLOWER (`micro-types-cpp`). Hence header-inline (reason in the class doc).

## Profile shift / follow-ups
- Placement: all kernels keep their size (`sumOverRows` 1821 / 3913 B, `forEachGPWithGlobalWeight` 3900 / 4557 B still at offset 0). The scalar `sumOverRows` now starts at offset 48 (was 32) in its 64-byte line. `interp/sg/cc` d8/d10 L2 (many tiny tensor grids) show the same instructions with op-cache misses +9 % (micro) / +20 % (e2e), giving +1.2–1.8 % cycles. Control cases and quadrature are unchanged.
- Candidate WP (RP-2 follow-up "code placement"): align `weighted_sum::sumOverRows` (`[[gnu::aligned(64)]]`, as WP-02 did for the DSG row loop). Measure on interp/sg L2 and quad/sg; WP-29c saw op-cache misses ×6 from aligning the collapse templates, so measure, don't assume.
- Tests: the time-seeded `linear_func_*` quadrature tests use a relative tolerance (1e-12 %) on `a/2 + b`, which can cancel; an absolute tolerance (or bounded draws) would remove these rare failures.
- PLAN.md / earlier summaries locate the moved functions under the old names (list above).

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`)
