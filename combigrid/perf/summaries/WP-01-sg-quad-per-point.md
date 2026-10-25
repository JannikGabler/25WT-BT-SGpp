# WP-01 — SG quadrature: per-grid-point work

Status: PROPOSED · Date: 2026-10-24 · Base commit: 931158248
Recommendation: accept — the e2e time of the relevant cases halves (15.8 s saved of 31.6 s), refcheck is fully OK, no API change.
Machine: `perf.py calibrate` ratios e2e 1.002 burst / 1.000 sustained, micro 0.998 / 1.001 (1.000 = as at baseline time)

## Change
- `quadrature_operator::processTensorGrid` (quadrature.cpp) now follows the DSG operator's design. It maps the 1D nodes onto the domain once per tensor grid and iterates row by row with the row dimension innermost. TU-local helpers: `mapNodesOntoDomain`, `findRowDim`, `setUpIterDims`, `updateWeightProducts`, and `sumOverRows` (the hot loop). The split into these helpers was done on 2026-10-25 at the owner's request; it is performance-neutral, see "Benchmarks". A row step updates only the coordinates and the suffix weight products of the dimensions that changed. Each row sum is multiplied once by the weight of the other dimensions, and the integrand is called through `SourceFunc::evaluate` directly. No more `denormalizeDataVector`, `getWeightForGP`, `evaluateNormalizedInPlace` or mutable-iterator rewrites per point.
- Only the dimensions with more than one node are iterated: the row dimension is the first such dimension, and 1-node dimensions (level 0 with boundary offset ≥ 1) contribute a constant coordinate and weight factor. This is generic: no case dispatch. Without it, every (often length-1) row at b1 walked through all leading 1-node dimensions (see "Attempts").
- `getWeightForGP` was removed: it is now unused. Its stale `%ignore` in `combigrid.i` was removed as python-bindings.md prescribes, which does not change the wrapper. `QuadratureScratch` gained the iteration state. Copyright banners were added to the 3 touched files (cpplint flagged them).
- Files: `operators/quadrature/quadrature.{cpp,hpp}`, `miscellaneous/data_scratches/quadrature_scratch.hpp`, `build/pysgpp/combigrid.i` (1 line), `tests/operators/quadrature/quadrature_test.cpp`, `examples/c++/benchmarks/perf/perf_micro.cpp` (+1 case)

## Gain vs. cost
- Gain: quad/sg group 22.30 → 9.41 s, mivec group 9.27 → 6.39 s (only the operator phase changes; mivec totals at d ≥ 15 are generation-bound). That is 15.8 s saved, i.e. 2.0x over the compared cases, or ≈ 22 % of the 70 s suite. High d: equi-exp b0/d10 3.48x, b1/d15 4.44x, b1/d20 4.66x, cc-uq/d10 2.75x. The integrand-heavy equi-sin/d7 gains 1.45x. Expected was 8–10.5 s: the 1-node-dimension handling adds the b1 gains.
- API / functionality: none. `quadrature_operator::getWeightForGP` (internal, `%ignore`d) is removed. `quadrature.hpp` is a bound header: the fast SWIG check (python-bindings.md) gives only the expected warning 389, and the quadrature Python examples run. Summation order inside a tensor grid changes (row sums), so results change at round-off level only.
- Maintainability / readability: +≈ 110 lines in quadrature.cpp (5 small documented helpers; `processTensorGrid` itself is 14 lines, the longest helper `sumOverRows` 52) and +8 scratch fields, documented. The structure mirrors the DSG operator. The "iterated dimensions" indirection is the only extra concept.

## Tests
- Pinning tests (passed on the unchanged code before the change, debug ×3), new suite `quadrature_operator_tensor_grids`:
  - `Process_Tensor_Grid_Matches_Naive_Sum`: per tensor grid, `processTensorGrid` vs. a naive sum over `getGridPointAndMI` × 1D weights. It checks the set of evaluated points and the number of evaluations, plus `quadrature()` vs. the naive combination. Covers equi / CC / mixed nodes, doubling and linear growth (Simpson + trapezoidal), offsets 0/1/2, unit and shifted/scaled domains, d = 1…4, complete and random MIVec sets.
  - `Single_Point_Tensor_Grids`, `Zero_Point_Tensor_Grid`.
  - The existing `equidistant_nodes`/`clenshaw_curtis_nodes` suites and the DSG-vs-SG comparisons also pin the results.
- New / extended tests: the suite above.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓

## Reference check
- `perf.py refcheck`: OK at 1 and 16 threads (104 OK, 0 INFO/WARN/FAIL) — `results/WP-01/refcheck.txt`.

## Benchmarks (A = WP-01-before, B = WP-01-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | 13.20ms | 6.92ms | 1.909 | [1.901, 1.916] |
| micro/quad/process_tg/equi/b0/d5/L10 | 7.56ms | 3.04ms | 2.480 | [2.448, 2.513] |
| micro/quad/process_tg/equi/b0/d10/L4 | 47.40ms | 14.01ms | 3.367 | [3.255, 3.482] |
| micro/quad/process_tg/cc/b0/d2/L16 | 31.86ms | 25.92ms | 1.222 | [1.201, 1.243] |
| micro/quad/process_tg/cc/b0/d10/L4 | 47.21ms | 13.97ms | 3.391 | [3.343, 3.441] |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 536.42ms | 111.22ms | 4.790 | [4.668, 4.915] |
| micro/quad/process_all_tgs/cc/b1/d10/L5 (new) | 176.61ms | 53.21ms | 3.364 | [3.166, 3.574] |

Speed-of-light gap: process_tg d10 14.01 ms vs. `micro/sol/quad_tg` 15.88 ms (was 2.9x, now 0.88x). d5: 3.04 vs. 3.55 ms (was 2.2x, now 0.86x). The operator beats the reference loop, which recomputes the row weight in O(d) per row.

L2/L3 e2e (16 threads; `total` and the changed phase `operator`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d10/L4 | total | 3.313s | 947.06ms | 3.481 | [3.410, 3.553] |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.623s | 1.320s | 2.746 | [2.730, 2.762] |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.517s | 2.426s | 1.450 | [1.438, 1.462] |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.372s | 533.87ms | 4.440 | [4.384, 4.496] |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.114s | 240.04ms | 4.662 | [4.592, 4.734] |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.818s | 715.71ms | 2.544 | [2.525, 2.563] |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.628s | 515.71ms | 3.166 | [3.133, 3.199] |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.410s | 1.262s | 1.146 | [1.042, 1.260] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total / operator | 3.247s / 1.870s | 1.862s / 478.05ms | 1.736 / 3.766 | [1.682, 1.792] / [3.464, 4.095] |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.220s | 816.47ms | 2.727 | [2.585, 2.877] |

All 17 compared cases are `faster` (operator 1.15x–5.2x). `generation` is unchanged within noise; the one `faster` verdict, d10/L4 at 2.66 → 2.20 ms on untouched code, is an artifact of a 2 ms phase.
Sum of medians (17 cases): A = 31.571s, B = 15.802s, saved = 15.768s (1.998x). Foreign load ≤ 0.4 %, round durations constant (98 s). Full reports: `combigrid/perf/results/WP-01/`

Helper split (snapshot `WP-01-split` = final tree, vs. `WP-01-after`):
- Micro: 1.000x over 9 cases, all within noise (`split-micro.md`).
- e2e `^quad/sg/`: 1.004x over 13 cases, within noise (`split-e2e.md`).
- Callgrind: `processTensorGrid` 77,010,000 vs. 77,009,517 instructions.
- Single-thread `perf_reference` output is bitwise identical, and refcheck is OK.

## Attempts not kept
- Variant A, rows always along dimension 0 (exactly as the prompt describes). Exploratory single runs, 1 thread: b0 equal to the final variant (d10 21.5 vs. 21.6 ns/point in the final A/B). b1 clearly worse: process_all_tgs/equi/b1/d20/L4 47.2 vs. 27.9 ns/point, cc/b1/d10/L5 30.7 vs. 23.9 ns/point. Dropped in favour of iterating only the multi-node dimensions.
- Two helper splits whose row helpers read the scratch vectors per row were measured against `WP-01-after` and dropped:
  - helpers taking `QuadratureScratch&`: 2–7 % slower;
  - a local class holding pointer and row members: 4–9 % slower, all methods inlined.
  - Cause: `SourceFunc::evaluate` is opaque, so GCC reloads scratch data after every call; with 3-point rows that adds ≈ 5 instructions and cycles per row. Kept: the loop copies everything into locals first, as before the split.

## Profile shift / follow-ups
- After (16 threads, self time): equi-exp/b0/d10/L4: integrand `expSum` 26 % + `exp` 36 % (+ exp@plt 4 %), `processTensorGrid` 23 %, `SourceFunc::evaluate` 2.8 % + its PLT stub 1.5 %, `std::function` invoker 2.4 %. b1/d20/L5: integrand + `exp` ≈ 64 %, `processTensorGrid` 11 %, OpenBLAS 6.9 % (thread pool, cf. D-3/WP-10), libgomp ≈ 6 %.
- PLT calls left (for D-2):
  - Per point: only `SourceFunc::evaluate@plt` (1.0–1.5 % of cycles). Without interposition, LTO could also inline the 2–3 % `evaluate` wrapper. So D-2's potential on this path is now ≲ 4 % for cheap integrands.
  - Per row: none.
  - Per tensor grid: `getWeights`, `TensorGrid::nGP/nDim/getGPCntPerDim/getNodesPerDim` (trivial getters, out-of-line in tensor_grid.cpp).
- WP-05 (per-tensor-grid costs): b1/d20 still runs at 27.9 ns/point vs. 21.6 at b0/d10 (small grids). cc/b0/d2/L16 is at 66 ns/point because of the per-grid CC weight computation, so caching 1D weights per (rule, level) should pay off.
- WP-04 (DSG row loop): delta grids at b1 have the same 1-node dimensions, so the "iterated dimensions" idea should carry over.
- WP-06/WP-08: interpolation and linear interpolation still call `evaluateNormalizedInPlace` → `denormalizeDataVector` per point. The same node premapping applies.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add `--keep <path>` to keep e.g. new tests).
**Caution**: `reject` restores *every* tracked change vs. HEAD. The tree also holds changes that are not part of WP-01: the master's `combigrid/perf/{PLAN,STATUS}.md`, a new guideline line in `CLAUDE.md`, and the staged `.gitignore`. Commit those first or pass `--keep` for them. WP-01-only revert: `git restore` the 6 files listed under "Change".
