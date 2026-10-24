# Optimization plan — combigrid

Ranked work packages (WPs). The master keeps this file current (status, ranks, new WPs from
summaries); evidence and profiles: [ANALYSIS.md](ANALYSIS.md). Numbers: baseline, judged
configuration, 16 threads unless stated; e2e medians from the baseline A/B self-tests.

**Ranking rule**: expected absolute time saved on the e2e suite, cases with d ≥ 7 first (they
run longest and the owner prioritizes high dimensions), then all cases; ties broken by operator
weight quadrature (SG, DSG) > global interpolation > linear interpolation > generation. WPs that
need a functional/interface/flag change are marked **needs decision** and are not scheduled
before the owner decided. Re-rank after every re-profiling (rule 7).

Baseline e2e time per operator group (sum of `total` medians, `baseline/e2e_t16.jsonl`;
generation < 1 % except MIVec): quad/sg 13.6 s (11.2 s at d ≥ 7) · quad/dsg 16.8 s (16.5 s at
d ≥ 7) · interp 4.1 s · interplin 3.1 s · mivec 5.5 s (of which generation 3.0 s).

| Rank | ID | Title | Status | Expected saving (e2e) |
|---|---|---|---|---|
| 1 | WP-01 | SG quadrature: per-grid-point work | ready | 5–6.5 s, ~2x on quad/sg d ≥ 5 |
| 2 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | 2.5–4 s (b1 d15/d20) |
| 3 | WP-03 | Downwards closure of MI sets (MIVecSGGenInstr) | ready | ~2.8 s, generation ≥ 10x at d ≥ 10 |
| 4 | WP-04 | DSG quadrature: row loop | ready | 1–2 s (d ≥ 7) |
| 5 | WP-05 | SG quadrature: per-tensor-grid costs (1D weights, allocations) | ready (after WP-01) | 0.3–1 s; low d and many small grids |
| 6 | WP-06 | Global interpolation: operator overhead | ready | ~0.8 s |
| 7 | WP-07 | Generic barycentric formula: cache the weights | ready | ~0.1 s e2e (90x per 1D call) |
| 8 | WP-08 | Linear interpolation: corner loop | ready | ~0.7 s |
| – | D-1 | Multi-point interpolation API (evaluate once, interpolate many) | needs decision | most of interp/interplin (≥ 3 s), scales with #points |
| – | D-2 | Compiler flag `-fno-semantic-interposition` (or hidden visibility) | needs decision | a few % to 10 % on hot helpers; re-evaluate after WP-01 |
| – | D-3 | Armadillo/OpenBLAS thread pool in every process | needs decision | cold runs only (no effect on warm runs) |

---

## WP-01 — SG quadrature: per-grid-point work
- **Location**: `combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.cpp`
  (`quadrature_operator::processTensorGrid`, `getWeightForGP`, `getWeights`),
  `miscellaneous/data_scratches/quadrature_scratch.hpp`; uses
  `miscellaneous/iterators/tensor_grid_iterator.hpp`, `SourceFunc::evaluateNormalizedInPlace`.
- **Hypothesis**: for every grid point the operator rewrites all d coordinates (mutable iterator),
  maps them onto the domain (`tools::denormalizeDataVector`, exported, called through the PLT),
  recomputes the d-fold weight product (`getWeightForGP`, PLT call) and goes through
  `evaluateNormalizedInPlace`. The deltas operator already avoids this: map the 1D nodes of the
  tensor grid onto the domain once per tensor grid (scratch), iterate rows with dimension 0
  innermost and update only changed coordinates, keep the weight product of dimensions ≥ 1 per
  row (multiply the row sum once), call `SourceFunc::evaluate` directly.
- **Evidence**: profile `quad/sg/equi-exp/b0/d10/L4`: `denormalizeDataVector` 37 %,
  `processTensorGrid` 23 %, `getWeightForGP` 13 %, `evaluateNormalizedInPlace` 3 %, integrand
  16 %; ~555 instructions per grid point at IPC 2.2, almost no cache misses (instruction-bound).
  Micro: `micro/quad/process_tg/equi/b0/d10/L4` 36.0 ns/point vs. speed of light
  `micro/sol/quad_tg/equi/b0/d10/L4` 12.0 ns/point (3.0x); d5: 23.0 vs. 10.3 ns (2.2x).
- **Expected gain**: operator 1.8–2.5x for `quad/sg/*-exp|*-uq` at d ≥ 5, ~1.3x for the
  integrand-heavy `equi-sin`; also the quadrature phase of `mivec/*`.
- **Risk**: low. Summation order inside a tensor grid changes (row sums) → round-off level
  changes only (refcheck noise allowance). Keep zero-point tensor grids and d = 1 working.
- **Tests**: `tests/operators/quadrature/quadrature_test.cpp`; add pinning tests comparing
  `processTensorGrid` with a naive sum over `getGridPoint(idx)` for anisotropic grids, boundary
  offsets 0/1, shifted domains, every node family with a quadrature rule.
- **Micro**: `micro/quad/process_tg/*`, `micro/quad/process_all_tgs/*`, `micro/sol/quad_tg/*`.
- **E2E filter**: `^quad/sg/|^mivec/`
- **Functional change**: none. **Dependencies**: none.

## WP-02 — DSG quadrature: per-delta-tensor-grid overhead at high d
- **Location**: `operators/quadrature/deltas_sparse_grid_quadrature.{hpp,cpp}`
  (`compSubTrie`/`appendSubTrieChildren`, `prepareDeltaTensorGrid`), `comp1DWeights` and the
  `CTCoeffTrie` construction (serial, before the parallel region),
  `miscellaneous/data_scratches/delta_quadrature_scratch.hpp`.
- **Hypothesis**: with boundary offset 1 at high d most delta tensor grids have very few points
  (level 0 has one node), but each one builds its own sub-trie of all tensor grids ℓ ≥ m
  (recursive, `push_back` into per-depth vectors) and maps its nodes; the per-tensor-grid work
  dominates. Reuse sub-tries between consecutive delta grids (sort/visit order), precompute what
  depends on m only partially, avoid reallocation; parallelize or speed up the serial
  preparation (0.19 s single-threaded at d20 L6).
- **Evidence**: profile `quad/dsg/equi-exp/b1/d20/L6`: `appendSubTrieChildren` **63 %**,
  `processDeltaTensorGrid` 23 %, integrand 7 %; ~1600 instructions and ~1.5 cache misses per grid
  point. Micro `micro/dsg/process_delta_tg/equi/b1/d20/L6` 319 ns/point vs. 13 ns/point for a
  large delta grid at d10; `micro/dsg/prepare/equi/b1/d20/L6` 0.2 s (serial); e2e
  `quad/dsg/equi-exp/b1/d20/L6` 4.3 s, `.../b1/d15/L7` 2.5 s.
- **Expected gain**: 1.5–3x on `quad/dsg/*/b1/d15|d20`.
- **Risk**: medium (trie logic; existing tests in `tests/operators/quadrature/delta_quadrature_test.cpp`
  cover sub-tries and global weights).
- **Tests**: existing delta quadrature tests (Sub_Trie_Of_Delta_Tensor_Grids,
  Global_Weights_Of_Delta_Grid_Points, Matches_Quadrature_*); extend for b1 at d ≥ 8.
- **Micro**: `micro/dsg/process_delta_tg/equi/b1/d20/L6`, `micro/dsg/prepare/equi/b1/d20/L6`.
- **E2E filter**: `^quad/dsg/`
- **Functional change**: none. **Dependencies**: none.

## WP-03 — Downwards closure of multi-index sets
- **Location**: `tools/downwards_closedness.hpp` (`genMIVecDownwardsClosure`), helpers in
  `tools/paretoMaxima.*`, `tools/multiindex_bounding_box_generation.*`,
  `tools/multiindex_domination.*`; called by `MIVecSGGenInstr` and `LvlMIVec::downwardsClosure`.
- **Hypothesis**: the closure scans the whole bounding box ∏(max_k + 1) and tests every box point
  against all Pareto maxima, O(|box| · |P| · d), which explodes with d although the closure is
  small. Generate the closure directly from the Pareto maxima (e.g. predecessor enumeration with
  a hash set / per-maximum box enumeration with deduplication), O(|closure| · d) up to hashing.
  **Keep the output order** of the current implementation (sort if necessary): the order of the
  multi-indices (and thus of the tensor grids) is observable; changing it needs the owner.
- **Evidence**: profile `mivec/sg/cc-exp/b1/d20/S7/n300`: `genMIVecDownwardsClosure` 96 %
  (d10: 49 %); e2e generation 0.79 s (d10), 1.04 s (d15), 1.12 s (d20) at 16 threads (scales
  only 4.8x from 1 thread); micro `micro/mivec/downwards_closure/d10/S10/n300` 270 ms single
  threaded for 3 522 closure elements (77 µs each).
- **Expected gain**: generation ≥ 10x at d ≥ 10 (≈ 2.8 s of the e2e suite).
- **Risk**: low–medium (correctness of the closure; tests exist in
  `tests/multiindex_vector/multiindex_vector_downwards_closure_test.cpp`).
- **Tests**: closure tests (incl. order), refcheck `closureSize`/`closureCoeffHash`.
- **Micro**: `micro/mivec/downwards_closure/*`, `micro/mivec/ct_coeffs/*`.
- **E2E filter**: `^mivec/`
- **Functional change**: none if the order is preserved. **Dependencies**: none.

## WP-04 — DSG quadrature: row loop
- **Location**: `deltas_sparse_grid_quadrature.hpp` (`forEachGPWithGlobalWeight`,
  `compRowWeights`, `updateCoeffSums`), `processDeltaTensorGrid`.
- **Hypothesis**: the row loop calls the source function through `std::function` per point with a
  `DataVector`; row weights are recomputed per row over all leaf trie nodes. Check vectorization
  of `compRowWeights`, hoisting, and the cost split between weights and calls.
- **Evidence**: profile `quad/dsg/equi-exp/b0/d10/L7`: `processDeltaTensorGrid` (inlined loops)
  41 %, integrand + `exp` 47 %; ~218 instructions per point; micro
  `micro/dsg/process_delta_tg/equi/b0/d10/L7` 12.9 ns/point.
  Upper bound ≈ 1.9x (integrand share); realistic 1.2–1.4x.
- **Expected gain**: 1–2 s on `quad/dsg/*` at d ≥ 7.
- **Risk**: low–medium. **Tests**: delta quadrature tests. **Micro**: `micro/dsg/process_delta_tg/*`
  (add a speed-of-light case). **E2E filter**: `^quad/dsg/` · **Functional change**: none.
- **Dependencies**: after WP-02 (same files; re-profile in between).

## WP-05 — SG quadrature: per-tensor-grid costs
- **Location**: `quadrature.cpp` (`getWeights`), `quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.cpp`
  (`genWeightsInplace` allocates and runs an FFT per call), `tools/operators/quadrature/quadrature_utilities.cpp`,
  iterator construction (allocations per tensor grid).
- **Hypothesis**: 1D weights depend only on (quadrature rule, number of nodes) but are recomputed
  for every tensor grid and dimension (CC: FFT + ~6 allocations per call). Precompute per
  dimension and level once per `quadrature()` call (as `comp1DWeights` does for the deltas
  operator) and look them up.
- **Evidence**: `micro/quad/process_tg/cc/b0/d2/L16` 39.4 ns/point vs. equi 16.7 ns/point (CC weights
  of a 131073-node dimension); profile `quad/sg/cc-uq/b0/d2/L20`: Eigen FFT 14 % + `genWeightsFFT`
  4 %; `micro/quad/process_all_tgs/equi/b1/d20/L4` 64.7 ns/point vs. 36 ns/point in a large grid.
  Also: `quad/sg/equi-exp/b0/d2/L20` scales only 5.5x (41 grids for 16 threads; no parallelism
  inside a grid) — load balancing at low d belongs here or into a follow-up WP.
- **Expected gain**: 1.1–1.4x on `quad/sg/cc-*` at d ≤ 3 and on many-small-grid cases (b1 d15/d20).
- **Risk**: low. **Tests**: quadrature tests + CC rule tests. **Micro**: `micro/quad/process_tg/cc/*`,
  `micro/quad/process_all_tgs/*`, `micro/quadrule/cc_weights/*`. **E2E filter**: `^quad/sg/|^mivec/`
- **Functional change**: none. **Dependencies**: after WP-01 (same function).

## WP-06 — Global interpolation: operator overhead
- **Location**: `operators/global_interpolation/global_interpolation.cpp`.
- **Hypothesis**: per tensor grid and query point the operator regenerates the 1D nodes
  (`genNodes`, allocation + `cos` for CC although the tensor grid stores them), assembles every
  grid point with `getGridPoint(idx)` (div/mod per dimension), maps it onto the domain per point,
  copies slices into fresh vectors, and `hasTrivialResult` scans all nodes per 1D call; one
  parallel region per query point over few tensor grids (`guided`) leaves threads spinning.
- **Evidence**: profile `interp/sg/cc/b0/d5/L6/p64`: `denormalizeDataVector` 12 %,
  `getGridPoint` 11 %, `hasTrivialResult` 3 %, integrand (`cos`) ~52 %; `interp/sg/cheb1/b0/d5`:
  libgomp spinning ≥ 15 %. Micro `micro/interp/global_tg/cc/b0/d5/L6` 59 ns/point.
- **Expected gain**: 1.2–1.5x on `interp/*` (integrand-dominated).
- **Risk**: low. **Tests**: `tests/operators/global_interpolation/interpolation_test.cpp` (errors
  are checked relative to max |f|, see `maxAbsOfLinearFunc`). **Micro**: `micro/interp/global_tg/*`.
- **E2E filter**: `^interp/` · **Functional change**: none. **Dependencies**: none.

## WP-07 — Generic barycentric formula: cache the weights
- **Location**: `operators/global_interpolation/methods/default_methods/barycentric_formula.cpp`,
  `tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.cpp`.
- **Hypothesis**: `BarycentricFormula::interpolate` recomputes the O(n²) log-space weights on every
  1D call (equidistant and Chebyshev-1 nodes). Weights depend only on the node distribution:
  compute them once (per tensor grid and dimension in the operator, or cached per (node family,
  n)) and evaluate in O(n).
- **Evidence**: `micro/interp/barycentric_1d/generic/n65` 18 µs per call (273 ns/node) vs.
  optimized CC formula 0.18 µs (2.8 ns/node); profile `interp/sg/cheb1/b0/d5/L4/p64`:
  `barycentricFormulaWeights` + `log` 17 %. Grows quadratically with the level.
- **Expected gain**: small in the e2e suite (~0.1 s, sizes are small), large for higher levels
  with equidistant/Chebyshev-1 nodes. Cheap to do.
- **Risk**: low (thread safety of a cache; prefer computing per tensor grid in the operator, which
  may require an internal signature change of `InterpolationMethod` — internal, not public API).
- **Tests**: barycentric formula tests (`tests/operators/global_interpolation/interpolation_methods/...`).
- **Micro**: `micro/interp/barycentric_1d/generic/*`, `micro/interp/global_tg/cheb1/*`.
- **E2E filter**: `^interp/sg/(cheb1|equi)/` · **Functional change**: none.
- **Dependencies**: after or together with WP-06 (same call path) — master decides.

## WP-08 — Linear interpolation: corner loop
- **Location**: `operators/linear_interpolation/linear_interpolation.cpp`, `TensorGrid::getNeighborsForLinInterpolation`.
- **Hypothesis**: for each of the 2^d corners the weight and coordinates are recomputed in O(d)
  (`computeNeighborGPWithWeight`, not inlined), then mapped onto the domain per corner. Map the
  neighbor coordinates once per tensor grid, update weights/coordinates incrementally (Gray code
  or per-dimension factors), skip corners with weight 0.
- **Evidence**: profile `interplin/sg/equi/b1/d8/L5/p256`: `computeNeighborGPWithWeight` 17 %,
  `denormalizeDataVector` 11 %, integrand (`cos`) ~57 %; micro `micro/interplin/tg/equi/b1/d8/L5`
  86 ns per corner.
- **Expected gain**: 1.2–1.4x on `interplin/*`.
- **Risk**: low (skipping zero-weight corners changes the evaluation count → refcheck INFO;
  results unchanged). **Tests**: linear interpolation tests. **E2E filter**: `^interplin/`
- **Functional change**: none. **Dependencies**: none.

## D-1 — Multi-point interpolation API (needs decision)
- **Idea**: `interpolate` / `interpolateLinear` evaluate the source function at all grid points
  of all tensor grids **for every query point** (64 points → 64x the evaluations). A batched
  overload (points → values) or a surrogate object that stores the function values per tensor
  grid evaluates each grid point once and interpolates all points from the stored values.
- **Expected gain**: interp/interplin e2e cases are dominated by integrand evaluations (52–57 %
  `cos` + integrand): ~#points-fold fewer evaluations, i.e. most of the 7 s of these groups; far more
  for real (expensive) source functions.
- **Interface**: new public API (and Python bindings); existing functions unchanged. Owner decides.

## D-2 — `-fno-semantic-interposition` / visibility (needs decision)
- SG++ compiles with `-fPIC` without `-fno-semantic-interposition`: exported functions are never
  inlined, not even within their own translation unit (PLT calls of hot helpers such as
  `getWeightForGP`, `denormalizeDataVector`, `getGridPoint`). The flag (combigrid only, or all
  modules) would let GCC inline them. Re-measure after WP-01/WP-06/WP-08 (they remove the hottest
  PLT calls structurally).

## D-3 — Armadillo/OpenBLAS thread pool (needs decision, low)
- With `USE_ARMADILLO=1`, `libsgppbase.so` links Armadillo → OpenBLAS, whose thread pool spins at
  process start (5–18 % of the samples of cold single-pipeline runs). No measurable effect on warm
  runs. Options: not linking Armadillo when Eigen provides the FFT, or nothing.
