# WP-17 — Batched source-function evaluation (proposal)

Status: NO-GAIN · Date: 2026-11-03 · Base commit: e5bd574ce
Recommendation: reject. The judged suite is 4 % slower (38.32 → 39.93 s, 0.960x, 29 of 38 cases SLOWER, none faster). The hypothesis does not hold: an opaque per-point call costs about the same anywhere, and the removable `std::function` overhead is only ≈ 5–8 instructions per point. The real gain is in Python (16 threads 25.1 → 0.88 µs per evaluation, 28x), and it can be had without this kernel change (options below).
Machine: `perf.py calibrate` ratios e2e 1.001 / 0.999, micro 1.006 / 1.003 (burst / sustained), OK at the start and before the A/Bs.

## Change (measured variant, left in the tree for review)
- **Evaluator design** (`miscellaneous/source_function_evaluation/`, internal `misc`):
  - `SourceFunc` / `VectorSourceFunc` hold one `std::unique_ptr<SourceFuncEvaluator>` instead of the `std::function`. Copies clone it, so copies keep their own callable, and the hook / slicing protection is unchanged.
  - The evaluator's virtual methods: `evaluate(point)`, `evaluateBlock(PointBlock)`, `evaluatePoints(DataMatrix)`, `clone`.
  - `PointwiseSourceFuncEvaluator<Func>` is built by a new template constructor, so lambdas get inlined into the block loop. The `std::function` constructors use the same class.
  - `BatchSourceFuncEvaluator` takes the new public `SourceFunc(BatchFunc)` / `VectorSourceFunc(BatchFunc, m)`, with `BatchFunc = void(const DataMatrix&, std::span<double>)`.
  - Cached functions use `CachedSourceFuncEvaluator` (block lookup, misses evaluated in one call of the inner evaluator, NaN not stored) and `AdaptiveCachedSourceFuncEvaluator` (mode read once per block, mode None passes the block through).
  - New public batch constructors of `CachedSourceFunc` / `CachedVectorSourceFunc`, and `evaluate(DataMatrix, span)`.
  - Interpolation stays per point (`evaluate(point)`).
- **Blocks** (`misc::PointBlock`):
  - A block is the rows of the leading iterated (SG) or projected (DSG) dimensions with at most `constants::source_func_evaluation::MAX_BLOCK_POINTS = 256` points, or a 256-point piece of a longer row.
  - The evaluator sets the coordinates itself (`forEachPointOfBlock`: flat loop, no copy of points).
  - Kernels: `sumOverRows` records the weight product per row and sums the block at its last row (`addRowsOfBlock` / `addSplitRow`). `forEachBlockWithGlobalWeights` writes the row weights into per-row slices and then runs one dot product per block. `forEachGPWithGlobalWeight` remains, built on top of it, for the tests.
- **Python** (`SourceFunc.i`, `VectorSourceFunc.i`, `CachedSourceFunc.i`):
  - `PyEvaluator` takes the GIL once per block.
  - New `SourceFunc.fromBatch(f)`, `VectorSourceFunc.fromBatch(f, m)`, `CachedSourceFunc.fromBatch(f[, cache|config])`, `CachedVectorSourceFunc.fromBatch(...)`: `f` receives a NumPy array (n, d) and returns n·m numbers.
  - `evaluate(DataMatrix)` → `DataVector`.
  - The C++-only constructors are under `#ifndef SWIG`.
- **Files**:
  - New sources: `miscellaneous/source_function_evaluation/{point_block,source_func_evaluator,pointwise_source_func_evaluator,batch_source_func_evaluator.{hpp,cpp},cached_source_func_evaluator.{hpp,cpp},adaptive_cached_source_func_evaluator.{hpp,cpp},cache_miss_batch,thread_scratch}`.
  - Changed sources: `source_function.{hpp,cpp}`, `vector_source_function.{hpp,cpp}`, `caching/{cached_source_function,cached_vector_source_function}.{hpp,cpp}`, `source_function_cache.hpp` (friends), the value types, both scratches, `quadrature_tools.{hpp,cpp}`, `deltas_sparse_grid_quadrature_tools.{hpp,cpp}`, `constants.hpp`.
  - Bindings: the three `.i` files.
  - Tests: new `batched_source_function_test.cpp`.
  - Benchmarks: `perf_micro.cpp` (new cases below).
  - Docs: CLAUDE.md §2, `module-reference.md`, `python-bindings.md`.

## Gain vs. cost
- **Gain (judged)**: none. The suite is 1.6 s slower.
  - quad/dsg b0: 0.925–0.975.
  - quad/sg b0: 0.936–0.969.
  - b1: 0.957–0.984.
  - interp / interplin: 0.945–0.998. The per-point virtual evaluator call costs more than the `std::function` call did.
  - mivec: within noise.
- **Why** (callgrind, `results/WP-17/evaluate-asm-before.txt`):
  - The old per-point path was ≈ 13 instructions around the call plus a 2-instruction thunk. The evaluator's loop is ≈ 9 instructions per point plus ≈ 3–5 per row for its own row odometer. b0 rows hold 3.0 points, so the row iteration now runs twice (kernel and evaluator).
  - Instructions: SG `process_tg` b0 d10 +4 %, DSG `process_delta_tg` +1.6 %, but the time is +7 %. Evaluating 243 points, then summing them, loses the overlap of the latency-bound integrand with the kernel's row work (loop fission).
- **Gain (new API)**:
  - Python per-point callable at 16 threads: 25.1 → 0.88 µs per evaluation (28x). At 1 thread: 0.78 → 0.63–0.68 µs.
  - NumPy batch callable: 0.18 µs per point (1 thread), 0.56 µs (16 threads).
  - C++: inlinable lambda ≈ function pointer for `expSum` (0.96–1.00 of the time); the gain exists only for trivial integrands (dispatch-only row loop 1.30–1.74x). A C++ batch callable is 1.24–1.33x **slower** (copying the block into the matrix; with scalar libm `exp` there is nothing to vectorize) — `micro-batch-forms-t16.txt`.
  - Cache: warm operators 1.12–1.29x, cold "always" 0.85–0.95x.
- **Memory**: geomean B/A 1.003, 1 MEM− (`quad/dsg/equi-exp/b0/d3/L16` 62.5 → 57.6 MiB), largest B/A `quad/sg/equi-exp/b1/d20/L5` 1.054 (within its 12 % bimodal spread).
- **API / functionality**: the agreed proposal. Existing constructors, callables and results are kept (refcheck OK). New public: the template constructors, `BatchFunc` constructors, `evaluate(DataMatrix)`, `evaluateBlock`, `fromBatch` in Python.
- **Maintainability**: ≈ +900 lines (10 new files). The kernels gain block bookkeeping. `SourceFunc` copy semantics become clone-based.

## Tests
- **Pinning** (passed unchanged before the change, debug ×3): `quadrature_test`, `delta_quadrature_test` (incl. `Global_Weights_Of_Delta_Grid_Points_Boundary_Offset_0`), `vector_quadrature_test` (incl. `vector_delta_quadrature/Outputs_Equal_Scalar_Quadrature_Boundary_Offset_0`), `source_function_test` (incl. `copies_call_copies_of_the_callable`, identical points), `vector_source_function_test`, `cached_source_function_test`, `adaptive_cached_source_function_test` (`overrideSamplingShift(0)` cases), interpolation tests.
- **New** (`batched_source_function_test.cpp`):
  - `point_block/{visits_the_tensor_product_in_order, block_shapes}`.
  - `batched_source_function/every_form_evaluates_blocks_points_and_matrices_alike`.
  - `operators_give_bitwise_equal_results_for_every_form`, at 1 thread: `std::function` / function pointer / lambda / batch / cached / cached vector / adaptive. Covers every operator, equi / CC / cheb1, b0 / b1, unit and shifted domains, d = 1 and 2 with rows longer than a block, d = 3 and 5 with many blocks.
  - `batch_callable_receives_blocks_of_at_most_max_points`, `nested_evaluation_inside_a_batch_callable`, `cached_batch_with_hits_misses_and_nan`.
- **Results**: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ (8 GB cap) · TSan ✓ (`results/WP-17/tsan.sh`: new block driver + WP-27 / WP-28 drivers, `tsan.txt`).
- **Other checks**:
  - DISABLE matrix (BOOST / LMDB / both): build + full suite ✓ (`nolib-builds.txt`).
  - Public-header check 3/3 ✓.
  - `SG_PYTHON=1` build ✓: 10/10 examples, `python_checks.py` 1 / 16 threads ✓, and the WP-26 / 27 / 28 / 29d Python checks ✓ (`python-checks.txt`).
  - The known-flaky test did not fail.

## Reference check
- `perf.py refcheck`: OK (104 OK at 1 and 16 threads, 0 INFO / WARN).
- Not bitwise: 60 of 72 quadrature outputs differ by 1e-16 to 1e-13 relative; interpolation is bitwise unchanged. Cause: the block sums are vectorized multiply-then-add instead of a fused multiply-add per point.
- The error against the exact solution grows only where it is round-off-sized: at most +0.14 round-off units of the combination (e.g. `quad/sg/cc/b0/d5/L5/sin` 2.1e-16 → 1.2e-15). Geomean error ratio 1.12 over the differing cases (`reference-errors.txt`).
- An explicit `std::fma` in `addScaled` still left 34 quadrature cases different and changed 4 interplin cases, so it was dropped.

## Benchmarks (A = WP-17-before, B = WP-17-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, 176 cases: classic + `_vec` m4 + `cache/op`; sum 15.748 → 14.695 s, dominated by the new `quad/row_loop` b0 case):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quad/row_loop/equi/b0/d10/L4 (new, opaque const) | 3.692s | 2.786s | 1.302 | [1.252, 1.353] |
| micro/quad/row_loop/equi/b1/d20/L4 (new) | 37.17ms | 24.84ms | 1.496 | [1.483, 1.508] |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 (lambda, now inlined) | 200.90ms | 110.34ms | 1.737 | [1.547, 1.951] |
| micro/dsg/row_loop_opaque/equi/b0/d10/L7/s256 (new) | 124.45ms | 125.80ms | 0.985 | [0.963, 1.008] |
| micro/quad/process_tg/equi/b0/d10/L4 | 13.87ms | 14.91ms | 0.931 | [0.920, 0.942] |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 109.90ms | 107.43ms | 1.019 | [1.005, 1.033] |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | 223.03ms | 241.86ms | 0.920 | [0.911, 0.929] |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | 91.06ms | 92.71ms | 0.987 | [0.974, 1.001] |
| micro/sourcefunc/eval_normalized_in_place/d10 | 35.02us | 38.47us | 0.909 | [0.905, 0.914] |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | 308.85ms | 295.74ms | 1.050 | [1.039, 1.061] |

L2/L3 e2e (16 threads, full suite; `operator` follows `total`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.792s | 7.208s | 0.938 | [0.929, 0.947] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.404s | 5.791s | 0.931 | [0.925, 0.938] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.401s | 2.420s | 0.991 | [0.983, 0.999] |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.450s | 2.517s | 0.945 | [0.886, 1.007] |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.973s | 2.011s | 0.977 | [0.964, 0.990] |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.302s | 1.355s | 0.962 | [0.956, 0.969] |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.246s | 0.984 | [0.980, 0.988] |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.04ms | 240.06ms | 0.978 | [0.959, 0.998] |

Sum of medians (38 cases): A = 38.316 s, B = 39.931 s, saved = −1.616 s (0.960x) · Full reports: `combigrid/perf/results/WP-17/` (`micro`, `e2e`, `micro-batch-forms-t16.txt`, `python-timing.txt`, `reference-errors.txt`, `refcheck.txt`)

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 57.6 MiB | 0.922 | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 57.7 MiB | 60.8 MiB | 1.054 | (spread) |

Geomean B/A = 1.003 · process floor A = 10.3, B = 10.4 MiB

## Attempts not kept
- **v1** (rows summed inside the kernel's row loop, nested loop in the evaluator): SG `process_tg` 0.857, DSG 0.85–0.89. GCC's `-funroll-loops` (SG++ default) unrolls the 3-point row loops ×4–8 with a dispatch prologue, ≈ 68 vs. 39 instructions of machinery per point.
- **`#pragma GCC unroll 1`** on the evaluator / row dot: 9-instruction point loop, but slightly worse overall (310 → 317 M instructions); dropped (owner prefers no pragmas).
- **Block caps 16 / 64** instead of 256: worse everywhere (cap 16: SG 0.87, DSG 0.91).
- **Four partial sums** in the DSG block dot: `process_delta_tg` 0.936 → 0.964, the other cases unchanged.
- **Explicit `std::fma`** in `addScaled`: see Reference check.

## Profile shift / follow-ups
- **Owner decision, options:**
  - (a) Reject (`perf.py reject --apply`; the only file worth keeping on its own is the new opaque-constant row-loop micro cases in `perf_micro.cpp`).
  - (b) WP-34 without kernel change: run Python per-point callables serially (1 thread ≈ 0.78 µs vs. 25 µs at 16 threads, ≈ 32x).
  - (c) Two paths: the old per-point kernel loops (and `std::function` for per-point calls) for C++ per-point callables, the block path for batch / Python / cached evaluators. Expected: judged ≈ 1.00x, Python 28x / NumPy batch ≈ 4x, warm cache 1.1–1.3x. Cost: a second loop per kernel and a split by evaluator kind (not measured; against the owner's one-path preference).
  - (d) Accept as is (−1.6 s judged, Python gains).
- **For PLAN.md:** the "dispatch 4–9 %" estimates (RP-1 – RP-3) counted the unavoidable call itself. Removable per-point overhead is ≈ 5–8 instructions, so no other WP should rank on it.
- **WP-44** (DSG single-pass row weights) is independent of this WP.
- **Not done:** `findBatch` / `insertBatch` grouped by shard (misses are batched, lookups are per point); one timed batch per call for the adaptive policy (per-point sampling kept).
- **Dimension-adaptive refinement:** not blocked. Cache keys are unchanged; blocks are per tensor grid, so per-TG contributions stay available.
- **Strict aliasing:** no pre-WP-14 hoisting simplified (the touched loops kept their locals; loops around opaque calls still need them).
- **Pre-existing, not touched:** `tensor_grid.cpp:34` unused `sum`.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
