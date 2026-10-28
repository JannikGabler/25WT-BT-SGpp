# Optimization plan — combigrid

Ranked work packages (WPs). The master keeps this file current (status, ranks, new WPs from
summaries); evidence and profiles: [ANALYSIS.md](ANALYSIS.md). Numbers: baseline (boost off,
re-recorded 2026-10-24), judged configuration, 16 threads unless stated; e2e medians from
`baseline/e2e_t16.jsonl`, micro from `baseline/micro_t1.jsonl`. WP-13 adds
`-fno-semantic-interposition` to GCC `OPT=1` builds via site_scons; the baseline (built from
fed2cdb4e with its own site_scons) stays without it, so the flag counts as an optimization.
WP-14 turns strict aliasing on (GCC default; `-fno-strict-aliasing` only for the SWIG Java
wrapper); the baseline keeps `-fno-strict-aliasing`. Numbers below predate RP-1.

**Ranking rule**: expected absolute time saved on the e2e suite, cases with d ≥ 7 first (they
run longest and the owner prioritizes high dimensions), then all cases; ties broken by operator
weight quadrature (SG, DSG) > global interpolation > linear interpolation > generation. WPs that
need a functional/interface/flag change are marked **needs decision** and are not scheduled
before the owner decided. Re-rank after every re-profiling (rule 7).
**Memory** (owner 2026-10-28): runtime stays the ranking metric; peak memory (max RSS per e2e
process, from WP-15 on) is monitored, reported with every runtime change and weighed in
decisions. Memory reduction is a valid secondary goal where profiles show potential.

Baseline e2e time per operator group (sum of `total` medians, `baseline/e2e_t16.jsonl`;
generation ≤ 1–2 % except MIVec; boost off, total 70.0 s): quad/sg 22.3 s (18.5 s at d ≥ 7) ·
quad/dsg 26.8 s (26.2 s at d ≥ 7) · interp 6.6 s · interplin 5.0 s · mivec 9.3 s (of which
generation 5.1 s).

| Rank | ID | Title | Status | Expected saving (e2e) |
|---|---|---|---|---|
| – | WP-01 | SG quadrature: per-grid-point work | accepted (2026-10-25) | measured: 15.8 s saved; quad/sg 22.3 → 9.4 s (2.4x), mivec 9.3 → 6.4 s; memory (WP-15): `quad/sg/cc-uq/b0/d2/L20` 806 → 1076 MiB (1.335x) |
| – | WP-11 | Toolchain measurement: GCC flags, Clang ThinLTO, Clang full LTO (measurement only) | accepted (2026-10-25): gcc-nosi → WP-13 | measured: gcc-nosi e2e 54.20 → 52.91 s (1.024x); Clang not adopted |
| – | WP-13 | `-fno-semantic-interposition` in site_scons (GCC, `OPT=1`, all modules) | accepted (2026-10-26) | measured: e2e 54.03 → 52.79 s (1.023x, 1.24 s) |
| – | WP-14 | `-fno-strict-aliasing` only for the SWIG Java wrapper; datadriven punning → `std::bit_cast` | accepted (2026-10-28) | measured: e2e 52.77 → 53.42 s (0.988x, −0.65 s); interp/interplin 1.10x, quad/dsg 0.970x, mivec 0.886x |
| – | WP-15 | Peak-memory measurement in the benchmark harness (owner's request; protected `perf.py`) | accepted (2026-10-28) | measured: runtime impact nil (full suite 1.0002x, 0 SLOWER); floor 10.2 MiB |
| 1 | RP-1 | Re-profiling after WP-01/WP-13/WP-14 (+ cumulative memory) | in progress (prompt 2026-10-28, with memory) | re-ranks everything below |
| 2 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | 4–6.5 s (b1 d15/d20) |
| 3 | WP-03 | Downwards closure of MI sets (MIVecSGGenInstr) | ready | ~5.4 s (WP-14 made the closure 11–19 % slower), generation ≥ 10x at d ≥ 10 |
| 4 | WP-09 | Multi-point interpolation API (from D-1, proposal for the owner) | ready | new-API callers only: interp ≥ 2x, interplin unknown; old API must not regress |
| 5 | WP-04 | DSG quadrature: row loop | ready (after WP-02) | 1.5–3 s (d ≥ 7) |
| 6 | WP-05 | SG quadrature: per-tensor-grid costs (1D weights, allocations) | ready | 0.5–1.5 s; low d and many small grids |
| 7 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09 decision) | ~1.3 s (before WP-13/WP-14, which gave interp 1.06x · 1.10x) |
| 8 | WP-07 | Generic barycentric formula: cache the weights | ready | ~0.2 s e2e (100x per 1D call) |
| 9 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09 decision) | ~1.1 s (before WP-14: interplin 1.10x) |
| 10 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3) | ready | cold process start only (5–18 % of cold samples) |
| 11 | WP-12 | Toolchain re-measurement after WP-02/WP-03 (optional, from WP-11) | planned (owner decides) | decision data only |
| – | D-1 | Multi-point interpolation API (evaluate once, interpolate many) | decided 2026-10-24 → WP-09 | – |
| – | D-2 | Compiler flag `-fno-semantic-interposition` (or hidden visibility) | done: WP-13 accepted 2026-10-26 | measured: e2e 1.023x |
| – | D-3 | Armadillo/OpenBLAS thread pool in every process | decided 2026-10-24 → WP-10 | – |

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
  Micro: `micro/quad/process_tg/equi/b0/d10/L4` 72.5 ns/point vs. speed of light
  `micro/sol/quad_tg/equi/b0/d10/L4` 25.1 ns/point (2.9x); d5: 46.7 vs. 21.1 ns (2.2x).
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
- **Result (accepted 2026-10-25)**: rows along the first multi-node dimension, nodes premapped
  per tensor grid, 1-node dimensions folded into constant factors. quad/sg 22.30 → 9.41 s,
  mivec 9.27 → 6.39 s (15.8 s saved, 2.0x over 17 cases); d10 b0 3.5x, b1 d15/d20 4.4/4.7x,
  equi-sin d7 1.45x; micro now 0.86–0.88x of the old speed-of-light loop; refcheck 104/104 OK.

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
  preparation (0.29 s single-threaded at d20 L6).
- **Evidence**: profile `quad/dsg/equi-exp/b1/d20/L6`: `appendSubTrieChildren` **63 %**,
  `processDeltaTensorGrid` 23 %, integrand 7 %; ~1600 instructions and ~1.5 cache misses per grid
  point. Micro `micro/dsg/process_delta_tg/equi/b1/d20/L6` 665 ns/point vs. 28 ns/point for a
  large delta grid at d10; `micro/dsg/prepare/equi/b1/d20/L6` 0.29 s (serial); e2e
  `quad/dsg/equi-exp/b1/d20/L6` 6.8 s, `.../b1/d15/L7` 4.0 s.
- **Expected gain**: 1.5–3x on `quad/dsg/*/b1/d15|d20`.
- **Risk**: medium (trie logic; existing tests in `tests/operators/quadrature/delta_quadrature_test.cpp`
  cover sub-tries and global weights).
- **Tests**: existing delta quadrature tests (Sub_Trie_Of_Delta_Tensor_Grids,
  Global_Weights_Of_Delta_Grid_Points, Matches_Quadrature_*); extend for b1 at d ≥ 8.
- **Micro**: `micro/dsg/process_delta_tg/equi/b1/d20/L6`, `micro/dsg/prepare/equi/b1/d20/L6`.
- **E2E filter**: `^quad/dsg/`
- **Functional change**: none. **Dependencies**: none.
- **From WP-11**: with gcc-nosi `appendSubTrieChildren` is still 64 % of `quad/dsg/equi-exp/b1/d20/L6`
  (16 threads; Clang ThinLTO 66 %, 11 % more cycles there); quad/dsg is the largest group
  (26.3 s with gcc-nosi), so WP-02 stays the top optimization candidate.
- **From WP-14** (strict aliasing on): the dsg trie functions are uniformly ~5 % slower
  (quad/dsg 0.970x; `quad/dsg/equi-exp/b1/d20/L6` 6.67 → 7.03 s). Measure the rewrite under the
  final flags; fix code generation at the source level (re-adding `-fno-strict-aliasing` per file
  would be a flag change → owner).

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
  (d10: 50 %); e2e generation 1.37 s (d10), 1.79 s (d15), 1.90 s (d20) at 16 threads (scales
  only 6.1x from 1 thread); micro `micro/mivec/downwards_closure/d10/S10/n300` 593 ms single
  threaded for 3 522 closure elements (168 µs each).
- **Expected gain**: generation ≥ 10x at d ≥ 10 (≈ 4.8 s of the e2e suite).
- **Risk**: low–medium (correctness of the closure; tests exist in
  `tests/multiindex_vector/multiindex_vector_downwards_closure_test.cpp`).
- **Tests**: closure tests (incl. order), refcheck `closureSize`/`closureCoeffHash`.
- **Micro**: `micro/mivec/downwards_closure/*`, `micro/mivec/ct_coeffs/*`.
- **E2E filter**: `^mivec/`
- **Functional change**: none if the order is preserved. **Dependencies**: none.
- **From WP-11**: Clang ThinLTO generates MI sets 1.4–1.9x faster than GCC (e2e mivec d20/d15/d10
  1.60x/1.47x/1.29x; micro `downwards_closure` 1.23–1.42x, `complete_mis_with_coeffs` 1.57x).
  Compare the generated code of the dominance/closure loops: a source-level change may give GCC
  the same (in addition to the algorithmic change).
- **From the progress report (2026-10-26)**: mivec `generation` at d15/d20 was faster on the old
  master 3ffe4d382 (1.56/1.66 s) than at baseline (1.79/1.90 s); RP-1 locates the cause.
- **From WP-14** (strict aliasing on): `genMIVecDownwardsClosure` 11–19 % slower (instructions
  +3.9 %, cycles +23 %, IPC 2.07 → 1.75; micro `downwards_closure` 0.94–0.96x; e2e generation
  d15 1.68 → 2.07 s, d20 1.88 → 2.07 s; mivec 0.886x). The rewrite replaces this loop anyway;
  measure under the final flags (flag changes → owner).

## WP-04 — DSG quadrature: row loop
- **Location**: `deltas_sparse_grid_quadrature.hpp` (`forEachGPWithGlobalWeight`,
  `compRowWeights`, `updateCoeffSums`), `processDeltaTensorGrid`.
- **Hypothesis**: the row loop calls the source function through `std::function` per point with a
  `DataVector`; row weights are recomputed per row over all leaf trie nodes. Check vectorization
  of `compRowWeights`, hoisting, and the cost split between weights and calls.
- **Evidence**: profile `quad/dsg/equi-exp/b0/d10/L7`: `processDeltaTensorGrid` (inlined loops)
  41 %, integrand + `exp` 47 %; ~218 instructions per point; micro
  `micro/dsg/process_delta_tg/equi/b0/d10/L7` 27.7 ns/point.
  Upper bound ≈ 1.9x (integrand share); realistic 1.2–1.4x.
- **Expected gain**: 1.5–3 s on `quad/dsg/*` at d ≥ 7.
- **Risk**: low–medium. **Tests**: delta quadrature tests. **Micro**: `micro/dsg/process_delta_tg/*`
  (add a speed-of-light case). **E2E filter**: `^quad/dsg/` · **Functional change**: none.
- **Dependencies**: after WP-02 (same files; re-profile in between). From WP-01: delta grids at
  b1 have the same 1-node dimensions; iterating only the multi-node dimensions should carry over.

## WP-05 — SG quadrature: per-tensor-grid costs
- **Location**: `quadrature.cpp` (`getWeights`), `quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.cpp`
  (`genWeightsInplace` allocates and runs an FFT per call), `tools/operators/quadrature/quadrature_utilities.cpp`,
  iterator construction (allocations per tensor grid).
- **Hypothesis**: 1D weights depend only on (quadrature rule, number of nodes) but are recomputed
  for every tensor grid and dimension (CC: FFT + ~6 allocations per call). Precompute per
  dimension and level once per `quadrature()` call (as `comp1DWeights` does for the deltas
  operator) and look them up.
- **Evidence**: `micro/quad/process_tg/cc/b0/d2/L16` 81.2 ns/point vs. equi 33.6 ns/point (CC weights
  of a 131073-node dimension); profile `quad/sg/cc-uq/b0/d2/L20`: Eigen FFT 9 % + `genWeightsFFT`
  2 %; `micro/quad/process_all_tgs/equi/b1/d20/L4` 134 ns/point vs. 72.5 ns/point in a large grid.
  Also: `quad/sg/equi-exp/b0/d2/L20` scales only 6.6x (41 grids for 16 threads; no parallelism
  inside a grid) — load balancing at low d belongs here or into a follow-up WP.
- **Expected gain**: 1.1–1.4x on `quad/sg/cc-*` at d ≤ 3 and on many-small-grid cases (b1 d15/d20).
- **Risk**: low. **Tests**: quadrature tests + CC rule tests. **Micro**: `micro/quad/process_tg/cc/*`,
  `micro/quad/process_all_tgs/*`, `micro/quadrule/cc_weights/*`. **E2E filter**: `^quad/sg/|^mivec/`
- **Functional change**: none. **Dependencies**: WP-01 accepted (same function).
- **From WP-01 (after its change)**: `micro/quad/process_tg/cc/b0/d2/L16` 66 ns/point vs. equi
  ~34 ns/point (per-grid CC weight computation) → cache 1D weights per (rule, #nodes);
  `process_all_tgs/equi/b1/d20/L4` 27.9 ns/point vs. 21.6 at b0/d10 (many small grids).

## WP-06 — Global interpolation: operator overhead
- **Location**: `operators/global_interpolation/global_interpolation.cpp`.
- **Hypothesis**: per tensor grid and query point the operator regenerates the 1D nodes
  (`genNodes`, allocation + `cos` for CC although the tensor grid stores them), assembles every
  grid point with `getGridPoint(idx)` (div/mod per dimension), maps it onto the domain per point,
  copies slices into fresh vectors, and `hasTrivialResult` scans all nodes per 1D call; one
  parallel region per query point over few tensor grids (`guided`) leaves threads spinning.
- **Evidence**: profile `interp/sg/cc/b0/d5/L6/p64`: `denormalizeDataVector` 12 %,
  `getGridPoint` 11 %, `hasTrivialResult` 3 %, integrand (`cos`) ~52 %; `interp/sg/cheb1/b0/d5`:
  libgomp spinning ≥ 15 %. Micro `micro/interp/global_tg/cc/b0/d5/L6` 127 ns/point.
- **Expected gain**: 1.2–1.5x on `interp/*` (integrand-dominated).
- **Risk**: low. **Tests**: `tests/operators/global_interpolation/interpolation_test.cpp` (errors
  are checked relative to max |f|, see `maxAbsOfLinearFunc`). **Micro**: `micro/interp/global_tg/*`.
- **E2E filter**: `^interp/` · **Functional change**: none. **Dependencies**: none; re-assess the
  expected gain after the owner's decision on WP-09 (a batched API removes per-query-point work).
  From WP-01: the operator still calls `evaluateNormalizedInPlace` → `denormalizeDataVector` per
  point; WP-01's node premapping per tensor grid applies here.
- **From WP-11**: `-fno-semantic-interposition` (WP-13) removes the PLT calls of exported
  helpers (interp 1.065x); RP-1 re-measures the shares of `denormalizeDataVector`/`getGridPoint`.
  On `interp/sg/cc/b0/d10/L2/p8` Clang needs 32 % fewer cycles in `interpolateFirstDim` (34 vs.
  50 G): compare the generated code. Refcheck: `interp/sg/equi/b0/d2/L5/cos-shifted` WARNs on any
  FMA-placement change in the barycentric formula (owner 2026-10-25: tolerance stays; explain it).

## WP-07 — Generic barycentric formula: cache the weights
- **Location**: `operators/global_interpolation/methods/default_methods/barycentric_formula.cpp`,
  `tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.cpp`.
- **Hypothesis**: `BarycentricFormula::interpolate` recomputes the O(n²) log-space weights on every
  1D call (equidistant and Chebyshev-1 nodes). Weights depend only on the node distribution:
  compute them once (per tensor grid and dimension in the operator, or cached per (node family,
  n)) and evaluate in O(n).
- **Evidence**: `micro/interp/barycentric_1d/generic/n65` 38 µs per call (577 ns/node) vs.
  optimized CC formula 0.38 µs (5.8 ns/node); profile `interp/sg/cheb1/b0/d5/L4/p64`:
  `barycentricFormulaWeights` + `log` 18 %. Grows quadratically with the level.
- **Expected gain**: small in the e2e suite (~0.2 s, sizes are small), large for higher levels
  with equidistant/Chebyshev-1 nodes. Cheap to do.
- **Risk**: low (thread safety of a cache; prefer computing per tensor grid in the operator, which
  may require an internal signature change of `InterpolationMethod` — internal, not public API).
- **Tests**: barycentric formula tests (`tests/operators/global_interpolation/interpolation_methods/...`).
- **Micro**: `micro/interp/barycentric_1d/generic/*`, `micro/interp/global_tg/cheb1/*`.
- **E2E filter**: `^interp/sg/(cheb1|equi)/` · **Functional change**: none.
- **Dependencies**: after or together with WP-06 (same call path) — master decides.
- **From WP-11**: `interp/sg/equi/b0/d2/L5/cos-shifted` WARNs whenever FMA placement in
  `BarycentricFormula::evaluateFormula` changes (Clang: error 1.6e-6 → 4.2e-6; pathological case,
  Lebesgue constant ~1e16). Owner 2026-10-25: tolerance stays; the summary explains a WARN.

## WP-08 — Linear interpolation: corner loop
- **Location**: `operators/linear_interpolation/linear_interpolation.cpp`, `TensorGrid::getNeighborsForLinInterpolation`.
- **Hypothesis**: for each of the 2^d corners the weight and coordinates are recomputed in O(d)
  (`computeNeighborGPWithWeight`, not inlined), then mapped onto the domain per corner. Map the
  neighbor coordinates once per tensor grid, update weights/coordinates incrementally (Gray code
  or per-dimension factors), skip corners with weight 0.
- **Evidence**: profile `interplin/sg/equi/b1/d8/L5/p256`: `computeNeighborGPWithWeight` 17 %,
  `denormalizeDataVector` 11 %, integrand (`cos`) ~58 %; micro `micro/interplin/tg/equi/b1/d8/L5`
  185 ns per corner.
- **Expected gain**: 1.2–1.4x on `interplin/*`.
- **Risk**: low (skipping zero-weight corners changes the evaluation count → refcheck INFO;
  results unchanged). **Tests**: linear interpolation tests. **E2E filter**: `^interplin/`
- **Functional change**: none. **Dependencies**: none; re-assess after the owner's decision on
  WP-09. From WP-01: same per-point `evaluateNormalizedInPlace` path; premap the nodes.

## WP-09 — Multi-point interpolation API (from D-1; proposal, owner decides after the results)
- **Source**: D-1. Owner, 2026-10-24: "Approve it as a work package. A new API can be considered
  if the performance gain is worth it. I will decide on this after having the results."
- **Location**: `operators/global_interpolation/global_interpolation.{hpp,cpp}`,
  `operators/linear_interpolation/linear_interpolation.{hpp,cpp}`, umbrella
  `combigrid/src/sgpp_combigrid.hpp`, bindings `combigrid/build/pysgpp/` (new declarations in
  bound headers are wrapped by SWIG automatically).
- **Hypothesis**: `interpolate` evaluates the source function at every grid point of every tensor
  grid for every query point (`interp/sg/cc/b0/d5/L6/p64`: 2.2e6 evaluations = 64 × ~34k SG
  points). A multi-point variant evaluates each tensor grid once and interpolates all query
  points from its value tensor; per query point only the d nested 1D interpolations remain.
  Design options (session compares, picks one, justifies): (a) batched free functions
  (points → values) processing tensor grid by tensor grid, stateless, memory = largest tensor
  grid + #points; (b) a surrogate object storing the values per tensor grid, memory = all SG
  points, reusable for later queries. Existing functions keep signatures and results.
  `interpolateLinear` probably evaluates only the 2^d corner neighbors per point and tensor grid
  (verify): batching then helps only through shared corners or whole-grid evaluation —
  quantify from the evaluation counts before implementing it there.
- **Evidence**: baseline single-point API: interp 6.6 s, interplin 5.0 s (d ≥ 7: 3.7 s / 4.4 s);
  integrand (`cos`) 52–57 % plus per-point assembly/mapping (`getGridPoint` +
  `denormalizeDataVector`) 23 % (`interp/sg/cc/b0/d5/L6/p64`) / 28 %
  (`interplin/sg/equi/b1/d8/L5/p256`, incl. `computeNeighborGPWithWeight`). Evaluation counts:
  ANALYSIS.md e2e table.
- **Expected gain**: only for callers of the new API: interp ≥ 2x on e2e-like cases, up to
  ~#points-fold for expensive source functions; interplin unknown until corner reuse is
  quantified. The existing e2e suite measures the old API and must not regress.
- **Measurement (deviates from the standard)**: the new API does not exist in the before
  snapshot. (1) Standard A/B `^interp/|^interplin/` on the existing cases: no regression.
  (2) Gain: an unprotected comparison benchmark (new micro cases or a separate driver, never
  `perf_e2e.cpp`) running the same problems/points as the `interp/*` and `interplin/*` e2e cases
  (from `perf_problems.hpp`), once as a loop over the single-point API and once with the new API,
  interleaved, ≥ 5 rounds, 16 threads; report speedup, evaluation counts and peak memory per case.
  (3) New-API results equal the single-point results within round-off on all interp/interplin
  reference cases.
- **Risk**: medium — public API surface (C++ and Python), parallelization changes (over points
  vs. tensor grids), memory of option (b).
- **Tests**: new: batched == single-point for several node families, boundary offsets 0/1,
  non-unit domains, anisotropic MI sets, d = 1, zero and one query points; existing interpolation
  tests unchanged.
- **E2E filter**: `^interp/|^interplin/` (no-regression check).
- **Functional change**: new public API, agreed as a proposal for this WP (owner decides after
  the results); existing API and results unchanged. Python bindings: expose if straightforward;
  the `SG_PYTHON=1` build must succeed in any case.
- **Dependencies**: none. Decides the value of WP-06/WP-08; WP-07 stays relevant either way.

## WP-10 — Do not link Armadillo when Eigen provides the FFT (from D-3)
- **Source**: D-3. Owner, 2026-10-24: keep the WP; if it only reduces start-up times it is
  probably not worth implementing — decided after the results are measured. So: **measure
  first** (cold-start cost with and without the OpenBLAS pool, e.g. `OPENBLAS_NUM_THREADS=1`
  as a no-code proxy), implement only if the numbers are clearly above noise, and report the
  cost/benefit either way.
- **Location**: `base/SConscript` (`USE_EIGEN` / `USE_ARMADILLO` handling), FFT backend selection
  in `operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.cpp`
  (Eigen wins when both are set).
- **Hypothesis**: with `USE_EIGEN=1 USE_ARMADILLO=1`, `libsgppbase.so` links Armadillo → OpenBLAS
  although the FFT uses Eigen; OpenBLAS starts its thread pool in every process.
- **Evidence**: 5–18 % of the samples of cold single-pipeline runs (ANALYSIS.md); no effect on
  warm runs. After WP-01: OpenBLAS 6.9 % of the samples in the profile of
  `quad/sg/equi-exp/b1/d20/L5` (a 240 ms process): the share grows as the operators get faster.
- **Expected gain**: cold process start (short programs, Python scripts, tests); the warm e2e
  suite is expected unchanged (within noise).
- **Measurement**: linked libraries (`ldd`) before/after, wall time of repeated fresh processes
  of short e2e cases (and a Python import), A/B on a few short cases.
- **Risk**: other modules or the Python build may use Armadillo (check first); all four
  `USE_EIGEN`/`USE_ARMADILLO` combinations must still build and pass the tests.
- **Functional change**: build configuration (linking only), agreed in principle by the owner.
  No new flags. If other users of Armadillo make a clean change impossible: BLOCKED with options.
- **Dependencies**: none; lowest priority.

## WP-11 — Toolchain measurement: GCC flags vs. Clang ThinLTO vs. Clang full LTO
- **Source**: D-2 and the owner's question on Clang vs. GCC. Owner, 2026-10-25: "create a
  toolchain measurement WP but please add a third configuration that measures Clang with full
  LTO (not ThinLTO). Please schedule this work package as the next work package."
- **Goal**: data for the owner's decision whether to change the judged toolchain/flags.
  Measurement only: SConstruct, `site_scons/`, `compileNormal.sh` and `perf.py` stay unchanged.
- **Configurations** (otherwise the judged configuration: `OPT=1 ARCH=avx2 USE_EIGEN=1
  USE_ARMADILLO=1`, `-march=native`; source = HEAD with WP-01):
  1. `gcc` — reference, = `perf.py build normal` (GCC 15.2, `-flto=auto`).
  2. `gcc-nosi` — + `-fno-semantic-interposition`.
  3. `gcc-nosi-bsym` — 2 + `-fno-plt` + `-Wl,-Bsymbolic-functions`.
  4. `clang-thin` — `COMPILER=clang` (Clang 21.1.8), `-flto=thin`, `-fuse-ld=lld`.
  5. `clang-full` — `COMPILER=clang`, full LTO (`-flto`), `-fuse-ld=lld`.
  Optional: the better Clang variant + `-fno-semantic-interposition` (Clang inlines exported
  functions by default; the flag adds direct calls).
- **Per configuration**: Boost tests (`--repeat 3`); refcheck at 16 threads (OK/WARN/FAIL counts;
  FP contraction defaults differ: GCC `fast`, Clang `on`); full e2e A/B vs. `gcc` (all 38 cases,
  flags affect every operator; ≈ 18–23 min each) and micro A/B vs. `gcc`; for the best variant per
  compiler a profile of 2 high-d cases incl. the OpenMP runtime share (libgomp vs. libomp).
- **Build times** (wall, `-j16`, idle): clean build, rebuild after touching one `.cpp`
  (`quadrature.cpp`) and after touching a widely included header, for `gcc`, `clang-thin` (with
  and without a ThinLTO cache), `clang-full`, and the debug builds (`OPT=0`) of both compilers.
  Count compiler warnings per compiler.
- **Tooling constraint**: `perf.py build normal` hard-codes the judged scons arguments, benchmarks
  only accept builds with a valid build stamp, and `perf.py` is protected. Do not edit protected
  files or bypass the hook. Preferred: a new unprotected wrapper (e.g.
  `combigrid/perf/scripts/toolchain.py`) that reuses perf.py's build/snapshot/ab/refcheck by
  import with the variant's scons arguments and records the real compiler and flags in each
  snapshot's metadata. If that is impossible without changing perf.py: BLOCKED, describing the
  minimal perf.py extension the owner would apply.
- **Pitfalls**: SCons persists variables in `buildVars.out`, so `COMPILER=clang` or extra flags can
  leak into later builds: restore it / pass `COMPILER=gnu` and the original flags explicitly, and
  verify each snapshot's compiler (`readelf -p .comment`). Configurations share `lib/` and the
  object files (full rebuild per switch). Clang binaries load `/usr/lib/x86_64-linux-gnu/libomp.so.5`
  (package `libomp5` 22.1.2), not Clang 21's own 21.1.8: record the runtime used (`ldd`). `perf.py`
  sets the standard OMP variables (both runtimes honour them); do not tune runtime-specific ones.
  End state: judged `gcc` build in the tree, tests and refcheck OK, `buildVars.out` as before.
- **Python**: for the best non-reference variant, check that `SG_PYTHON=1` builds and the
  combigrid Python examples run (no measurement).
- **Expected**: `gcc-nosi` ≲ 4 % on the SG quadrature path, more on interpolation/DSG paths that
  still call exported helpers; Clang vs. GCC typically within ±10 % per group, either direction;
  the OpenMP runtime may matter for small parallel regions (interpolation).
- **Functional change**: none in the tree. Adopting a configuration changes the judged
  configuration: owner decides after the results (baseline re-recording is owner-only).
- **Dependencies**: WP-01 committed. RP-1 follows directly, so it profiles the then-judged
  configuration.
- **Result (2026-10-25, accepted)**: gcc-nosi e2e 1.024x (quad/sg 1.017x, quad/dsg 1.018x, interp
  1.065x, interplin 1.000x, mivec 1.043x; 25 faster / 1 SLOWER at 0.990x), refcheck OK, 100/104
  bitwise identical. Clang ThinLTO 1.034x but quadrature 0.96–0.98x, refcheck WARN, 612 warnings;
  full LTO 1.018x. Owner: adopt `-fno-semantic-interposition` via site_scons for GCC `OPT=1`
  builds of all modules (WP-13); baseline unchanged; `toolchain.py` kept.

## WP-13 — `-fno-semantic-interposition` in site_scons (GCC, `OPT=1`)
- **Source**: WP-11 summary + owner's decision 2026-10-25 (switched from "judged configuration
  only" to SCons after the master's answer: safe in the GNU path, baseline stays valid).
- **Location**: `site_scons/SGppConfigure.py`, `configureGNUCompiler` (covers `gnu`, `openmpi`,
  `mpich`); the generic `OPT` branch at line ~43 applies to every compiler, so not there. Not on
  `win32`/`cygwin` (as `-fPIC` at line ~106). Compile and link flags (WP-11 passed it in
  CPPFLAGS and LINKFLAGS; the LTO link step optimizes). CLAUDE.md §4: what `OPT=1` adds.
- **Expected**: as gcc-nosi in WP-11: e2e 54.20 → 52.91 s (1.024x; quad/sg 1.017x, quad/dsg
  1.018x, interp 1.065x, interplin 1.000x, mivec 1.043x), micro geomean 1.062x, refcheck OK
  (100/104 bitwise, 4 uq ≤ 1.1e-15 relative); PLT calls 2090 → 1709, local aliases 7 → 85.
- **Other modules**: all GCC `OPT=1` builds get the flag. Behavior changes only for interposed
  or duplicate exported symbols: check strong exported symbols defined in more than one
  `lib/libsgpp*.so`; build and test the other modules with and without the flag if the
  dependencies allow (pre-existing failures must be identical).
- **Risk**: low (no source change; Windows CI untested locally → excluded). `OPT=0` builds unchanged.
- **E2E filter**: all (`.`), micro: all.
- **Functional change**: build configuration, approved by the owner 2026-10-25.
- **Result (2026-10-26, accepted)**: GCC ≥ 5, `OPT=1`, not cygwin/win32, `AppendUnique` to
  CPPFLAGS/LINKFLAGS, placed after `-fno-strict-aliasing -funroll-loops`. e2e 1.023x (quad/sg
  1.015x, quad/dsg 1.016x, interp 1.062x, interplin 1.007x, mivec 1.041x), micro 1.059x, 0 SLOWER
  e2e; code bitwise = WP-11 gcc-nosi; refcheck OK; 816/816 Boost cases in all 8 modules
  (SG_ALL=1 SG_PYTHON=0; GSL, MPI, OpenCL, CGAL, UMFPACK, Gmm++ not found/used); Python examples
  OK. No duplicate exported functions across SG++ libraries (94 duplicates are RTTI data).

## WP-14 — `-fno-strict-aliasing`: measure its cost and restrict its scope
- **Source**: owner 2026-10-25, after the master's research on whether SWIG still needs the flag.
- **Background**: `configureGNUCompiler` adds `-fno-strict-aliasing` to every object of every
  module (`site_scons/SGppConfigure.py` ~502–508, together with `-funroll-loops`; Intel path ~665
  too; the Clang path never adds it). Justification in the comment: SWIG Java docs / CHANGES 2006.
  Research (2026-10-25): SWIG 4.x **Java** wrappers still type-pun jlong ↔ pointer (`java.swg`:
  `$1 = *($&1_ltype)&$input;`), and the SWIG 4.3 Java manual (§27.2.4, §27.8.5) still requires the
  flag with GCC optimization. **Python**: not needed as far as known: PEP 3123 fixed
  `PyObject_HEAD` in Python 3.0, this machine's Python passes no such flag to extensions
  (`sysconfig` CFLAGS), SWIG 4's Python typemaps convert via a `void*` temporary +
  `reinterpret_cast`. WP-11's Clang build (no flag) passed combigrid tests, refcheck and all
  combigrid Python examples. The wrappers inherit the flag via `env.Clone()`
  (`pysgpp/SConscript:29`, `jsgpp/SConscript:24`) and are compiled with `-w` (warnings hidden).
- **Part A, measure** (judged build incl. WP-13 vs. the same + `-fstrict-aliasing` appended to
  CPPFLAGS/LINKFLAGS, last flag wins; `toolchain.py` variant): Boost tests `--repeat 3`, refcheck
  1/16 threads (strict aliasing does not change FP semantics: a result change hints at code that
  relies on type punning), full e2e + micro A/B. Strict-aliasing scan: build all modules with
  strict aliasing on (`-Wall` then enables `-Wstrict-aliasing=3`; optionally `=2` for more,
  with false positives) and list the warnings per module and file.
- **Part B, restrict** (all builds of the complete project: `SG_ALL=1`, `SG_PYTHON=1`, `SG_JAVA=1`
  if a JDK is available; all Boost tests, C++ examples, Python examples and Python tests;
  outcomes compared with the unchanged build, pre-existing failures must be identical):
  1. Preferred: drop the flag from the global GNU flags and add it only to the SWIG wrapper
     environments that need it: `javaEnv` (required per SWIG docs); `pyEnv` only if evidence says
     so. Adding it in the wrapper SConscript applies to every compiler (also fixes the Clang
     path for jsgpp); report it.
  2. Fallback (owner's): if library code itself needs the flag (scan warnings, failing tests,
     result changes), add it globally only when bindings are compiled (`SG_PYTHON`/`SG_JAVA`,
     narrowed to the bindings that need it). Per-module restriction may be described as an
     additional option with evidence, not implemented.
  No new warnings allowed (CI builds with `-Werror`); `-funroll-loops` and the Intel path stay
  unchanged.
- **Interaction with WP-13** (owner's question 2026-10-26): TBAA only helps where GCC sees the
  accesses in one body; WP-13's inlining of exported helpers creates such bodies, and IPA modref
  summaries (used only for non-interposable functions) carry type information only with strict
  aliasing, so the gains compound (WP-14 measures strict aliasing *given* WP-13). Risk side:
  punning across former call boundaries was hidden by opaque PLT calls; inlining + strict
  aliasing can now miscompile it, and `-Wstrict-aliasing` does not see it. WP-13's all-module
  tests still had `-fno-strict-aliasing`. Clang (inlines exported functions, never adds the flag)
  was already in the combined state (WP-11: combigrid tests + Python examples OK).
  Runtime check (optional, available): Clang 21 TypeSanitizer `-fsanitize=type` (experimental;
  runtime `libclang_rt.tysan-x86_64.a` installed): Boost tests of all modules, reports of real
  violations in library code → fallback.
- **Machine**: JDK installed by the owner 2026-10-25 (openjdk-21-jdk-headless 21.0.12; `javac`,
  `java` in /usr/bin), so `SG_JAVA=1` is built and tested like the other bindings.
- **Expected gain**: unknown (TBAA lets GCC keep int bounds/sizes in registers across `double`
  stores and vectorize more); measurement first.
- **E2E filter**: all (`.`), micro: all. **Risk**: medium for other modules (hidden type punning);
  low for combigrid (Clang evidence).
- **Functional change**: build configuration, approved by the owner 2026-10-25 (restrict the
  flag's scope; fallback: only when bindings are compiled). **Dependencies**: WP-13 accepted
  (same function in site_scons; complete-project test runs of WP-13 serve as the reference).
- **Result (2026-10-28, accepted)**: preferred restriction holds: flag dropped globally, added to
  `javaEnv` only (every compiler); pysgpp needs none. Owner also chose to fix datadriven's 8
  pointer-cast punning sites with `std::bit_cast` (results bitwise identical). e2e 0.988x
  (−0.65 s; interp 1.097x, interplin 1.101x, quad/sg 0.995x, quad/dsg 0.970x, mivec 0.886x),
  micro geomean 1.033x, refcheck OK (6 CC values ±1–9 ulp via FMA in Eigen's kissfft), complete
  project pass/fail identical. Follow-ups → WP-02, WP-03.

## RP-1 — Re-profiling after WP-01/WP-13/WP-14
- **Source**: rule 7 (WP-01 changed quad/sg by 2.4x; three accepted changes since the setup
  profile; WP-14 shifted mivec generation and the dsg trie).
- **Content** (no source changes): `calibrate`; snapshot `rp-1` of HEAD (`baseline` exists);
  `ab e2e baseline rp-1` (cumulative progress, all cases); `profile` of the top cases by absolute
  time (high d first, ≥ 1 per operator group); micro runs of the hot spots; mivec generation
  d15/d20 `report-master` vs. `rp-1` (cause of the old master's advantage, input to WP-03);
  section "Re-profiling RP-1" in ANALYSIS.md; re-ranked expected savings in the summary.
- **Questions**: is `appendSubTrieChildren` still ~63 % at `quad/dsg/*/b1/d20`; what dominates
  the largest b0 dsg cases (`cc-uq`/`equi-exp` d10 L7, WP-04); quad/sg split integrand vs.
  per-tensor-grid costs (WP-05); closure share at d10/d15/d20 (WP-03); `denormalizeDataVector`,
  `getGridPoint`, libgomp spinning in interp (WP-06, WP-07) and interplin (WP-08).
- **Memory (after WP-15)**: cumulative peak memory baseline → rp-1 per case; cases with the
  largest peak above the process floor and what dominates them (diagnostic heap profile in
  separate runs, never during timing); memory-reduction WP proposals only where the potential
  is material. From WP-15: `quad/sg/cc-uq/b0/d2/L20` 806 → 1076 MiB since WP-01 (snapshots
  `WP-01-before`/`WP-01-after` exist): find the cause; does `quad/sg/equi-exp/b0/d2/L20`
  (620 MiB) follow the same pattern.

## WP-15 — Peak-memory measurement in the benchmark harness
- **Source**: owner 2026-10-28: "The benchmarks are currently only tracking runtime and not
  memory usage. [...] The most relevant value is the maximum memory usage. The memory
  measurements should ideally have no or only a very tiny impact on the performance
  measurements." Runtime stays the plan's metric; memory is monitored, reported with runtime
  changes and weighed in decisions; memory reduction is a valid secondary goal.
- **Location**: `combigrid/perf/scripts/perf.py` (**protected**; process launch of `run`/`ab`
  e2e, report tables, saved JSON, footer); docs: `combigrid/perf/README.md`,
  `summaries/TEMPLATE.md`, `.claude/skills/perf-work/SKILL.md`, `.claude/skills/perf-master/SKILL.md`.
- **Approach**: peak RSS of each e2e benchmark process measured by the kernel outside the
  process: no code in the measured binary, timings (taken inside the binary) unaffected, works
  for every existing snapshot incl. `baseline` (cumulative memory in RP-1). Per case: median per
  snapshot, ratio B/A, flag beyond a threshold from an A/A check; footer: geomean ratio, largest
  increase. Document the process floor (libraries, OpenMP, OpenBLAS) so values read as
  floor + case memory. e2e only (micro processes are dominated by the harness).
- **Pitfall (verified by the master 2026-10-28)**: `ru_maxrss` of a child includes the hiwater
  of its pre-exec image; a child spawned from Python reports **the driver's own peak**
  (`/bin/true` from a 300 MiB Python process: 319432 KiB via `os.wait4`). GNU time
  (`/usr/bin/time -f %M`, installed) forks from a small process and reports 1536 KiB. So:
  launch through such a small launcher (GNU time with `-o FILE`, or equivalent) or another
  method that measures only the benchmark's own image; never `RUSAGE_CHILDREN` (running
  maximum over all children).
- **Not now (proposal only)**: per-phase peaks inside `perf_e2e.cpp` (`VmHWM`, reset via
  `/proc/self/clear_refs` "5" between phases): changes protected C++ files, and the `baseline`
  snapshot binary lacks it.
- **Validation**: values vs. a manual `/usr/bin/time -v` run of one case; A/A
  `ab e2e WP-14-after WP-14-after` over a selection (≥ 1 case per group, high d): runtime ≈ 1.000
  within noise (no impact), memory spread → threshold.
- **Protected files**: only `perf.py` may change (owner's authorization 2026-10-28). The owner
  disables the hook for this session; afterwards reviews the diff, re-enables the hook, runs
  `perf.py protect --authorized-by-owner`. No change to `baseline/`, `reference/`, the C++
  benchmark files, timing, case order, rounds, noise model or verdict logic.
- **Expected gain**: none (infrastructure). **Risk**: low. **Dependencies**: WP-14 committed.
- **Result (2026-10-28, accepted)**: GNU time launcher (`-f %M -o .work/peak_rss.txt`) for the
  e2e processes of `run`/`ab`; `peak_rss_kib` per JSON record, `process_floor_kib` (`perf_e2e
  --list`, 10.1–10.3 MiB). Runtime impact nil (full suite on/off 1.0002x, 0 SLOWER; ≈ 1.2 ms per
  process outside the timed region). Flag `MEM±` = |B/A − 1| > 3 % and no overlap of rounds (A/A:
  ≤ 0.8 % false flags per case). Owner: `run e2e` stays at one process per case (per-case times
  differ from the one-process `baseline/e2e_t16.jsonl` for a few small cases; compare within one
  mode). 22/38 cases < 20 MiB (floor-dominated); largest `quad/sg/cc-uq/b0/d2/L20` 1.07 GiB.
  Per-phase peaks: option for a later memory WP.

## WP-12 — Toolchain re-measurement after WP-02/WP-03 (optional)
- **Source**: WP-11 summary: the Clang vs. GCC picture may change once WP-02 (DSG sub-tries, where
  Clang is slower) and WP-03 (MI closure, where Clang is 1.4–1.9x faster) changed the code.
- **Content**: `toolchain.py` build + snapshot of `gcc` (judged, incl. nosi) and `clang-thin`,
  refcheck, full e2e and micro A/B; no build-time measurements.
- **Status**: planned; the owner decides after WP-02/WP-03 whether to run it.

## D-1 — Multi-point interpolation API (needs decision)
- **Idea**: `interpolate` / `interpolateLinear` evaluate the source function at all grid points
  of all tensor grids **for every query point** (64 points → 64x the evaluations). A batched
  overload (points → values) or a surrogate object that stores the function values per tensor
  grid evaluates each grid point once and interpolates all points from the stored values.
- **Expected gain**: interp/interplin e2e cases are dominated by integrand evaluations (52–57 %
  `cos` + integrand): ~#points-fold fewer evaluations, i.e. most of the 11.6 s of these groups; far more
  for real (expensive) source functions.
- **Interface**: new public API (and Python bindings); existing functions unchanged. Owner decides.
- **Decision (2026-10-24)**: approved as work package WP-09; adoption of the API is decided after
  its results.

## D-2 — `-fno-semantic-interposition` / visibility (needs decision)
- SG++ compiles with `-fPIC` without `-fno-semantic-interposition`: exported functions are never
  inlined, not even within their own translation unit (PLT calls of hot helpers such as
  `getWeightForGP`, `denormalizeDataVector`, `getGridPoint`). The flag (combigrid only, or all
  modules) would let GCC inline them. Re-measure after WP-01/WP-06/WP-08 (they remove the hottest
  PLT calls structurally).
- Effects explained to the owner 2026-10-24 (inlining/IPA/direct calls of exported functions in
  their own library, also across TUs with LTO; only breaks deliberate symbol interposition,
  e.g. LD_PRELOAD of an SG++ symbol; no API/ABI change; possible round-off changes from FMA
  contraction across former call boundaries). Still open; proposed: measurement-only WP after
  WP-01.
- Owner asked (2026-10-24) whether `-fPIC` itself can go. Facts given: `-fPIC` is added for every
  object on non-Windows platforms (`site_scons/SGppConfigure.py:107`, also with
  `BUILD_STATICLIB=1`); shared libraries and the Python module `_pysgpp_swig.so` require PIC;
  the PIC addressing itself is nearly free on x86-64, the cost is GCC's interposition rule.
  Variants for the measurement WP: `-fno-semantic-interposition` (compile time: inlining + direct
  calls), `-Wl,-Bsymbolic-functions` (link time: direct calls only), both; dropping `-fPIC` for
  static C++-only builds (GCC 15.2 here defaults to PIE) only as a reference point, since it
  changes the build model.
- WP-01 findings: on the SG quadrature path only `SourceFunc::evaluate@plt` is left per point
  (1.0–1.5 % of cycles; the `evaluate` wrapper 2–3 % more could be inlined) → ≲ 4 % there; per
  tensor grid `getWeights` and the trivial `TensorGrid` getters (out-of-line in tensor_grid.cpp).
- Owner 2026-10-25: measure in WP-11 together with Clang (ThinLTO and full LTO).
- **Decision (2026-10-25, after WP-11)**: first "judged configuration only", then switched by the
  owner to site_scons: `-fno-semantic-interposition` in `configureGNUCompiler` when `OPT=1`, not on
  win32/cygwin, all modules → WP-13. compileNormal.sh/perf.py unchanged, no baseline
  re-recording. `-fno-plt`/`-Bsymbolic-functions` not adopted (no gain).

## D-3 — Armadillo/OpenBLAS thread pool (needs decision, low)
- With `USE_ARMADILLO=1`, `libsgppbase.so` links Armadillo → OpenBLAS, whose thread pool spins at
  process start (5–18 % of the samples of cold single-pipeline runs). No measurable effect on warm
  runs. Options: not linking Armadillo when Eigen provides the FFT, or nothing.
- **Decision (2026-10-24)**: "Sure" → WP-10 (do not link Armadillo when Eigen provides the FFT).
