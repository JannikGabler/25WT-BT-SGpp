# WP-77 — TG-loop load balance: largest-first tensor-grid order

Status: PROPOSED · Date: 2026-11-25 · Base commit: 4523e028b
Recommendation: accept — suite −0.39 s (1.011x) and `expensive/` −1.86 s (1.117x), results bitwise unchanged at every thread count, ≈ 60 lines of code; costs: grid construction +0.1–3.5 ms, grid heap +8 B per TG, cached SG quadrature +7 %. SG interplin `dynamic` (owner's decision): `-ramp` 1.18x, d2 L16 −4 %.
Machine: `perf.py calibrate` ratios e2e 1.004 / 1.001 (burst / sustained), micro 0.998 / 1.001

## Change
- Every grid computes a largest-first processing order once, in `SparseGridBase::computeMetaValues` (the existing pass over the levels):
  - new `getTGProcessingOrder()`, internal to the operators and not in Python;
  - size class of nGP = bit width + 3 mantissa bits (`tools::sparse_grid::sizeClassOf`);
  - stable O(nTG) counting sort (`orderBySizeClassDescending` / `largestFirstOrder`, new `tools/sparse_grid/tensor_grid_processing_order.{hpp,cpp}`);
  - TGs with fewer than total/4096 points share the last class, i.e. keep index order (`constants::sparse_grid_storage::SMALL_TG_POINT_SHARE_DIVISOR`).
- The TG loops iterate this order with `schedule(dynamic)` and write to the TG-indexed slots (WP-61 / WP-48), so the order cannot change any bit:
  - SG quadrature / mivec (scalar + vector): `tools::quadrature::estimateTensorGrids`;
  - DSG quadrature: `estimateDeltaTensorGrids`;
  - SG global interpolation: `forEachTensorGrid`, was `schedule(guided)`.
- SG interplin: `schedule(static)` → `schedule(dynamic)` in index order (no order: uniform 2^d corners per TG). **Owner's decision in the session**: the prompt's rule kept static because of d2 L16 (−4 %), the owner accepts that price for source functions of position-dependent cost (`-ramp`).
- Docs: Doxygen of the new code and of the getter, `combigrid/claude/module-reference.md` (storage + schedules), `combigrid.i` comment (getter not forwarded). Python: rebuilt with `SG_PYTHON=1`, all 12 examples exit 0, `results/WP-77/python_examples.txt`.
- Files: `grids/sparse_grid_base.{hpp,cpp}`, `constants.hpp`, `tools/sparse_grid/tensor_grid_processing_order.{hpp,cpp}` (new), `tools/operators/quadrature/{quadrature,deltas_sparse_grid_quadrature}_tools.cpp`, `tools/operators/global_interpolation/global_interpolation_tools.cpp`, `tools/operators/linear_interpolation/linear_interpolation_tools.cpp` (schedule), `build/pysgpp/combigrid.i` (comment), `benchmarks/perf/perf_micro.cpp` (+1 case), tests (below), `claude/module-reference.md`.

## Gain vs. cost
- **Gain (full suite A/B, 16 t)**: suite 36.326 → 35.935 s (−391 ms, 1.011x); `expensive/` 17.751 → 15.894 s (−1.858 s, 1.117x). Main cases:
  - mivec/sg d10 S12: 1.16x; d15 S10: 1.33x;
  - quad/dsg b0 d3 L16: 1.107x; d5 L12: 1.03x;
  - interp/sg d2 L12: 1.49x; cheb1 d3 L6: 1.50x;
  - quad/sg d2 L20: 1.11x (CI touches 1);
  - `expensive/mivec/sg` d15 c100us: 2.156 → 1.564 s; `-ramp`: 2.475 → 1.846 s; d20 c1us: 1.35x;
  - `expensive/mivec/dsg` d15 c100us: 0.880 → 0.552 s; `expensive/interp/sg` ramp: 1.20x.
  - These match the RP-6 model (mivec/sg c100us predicted 1.56 s, measured 1.564 s).
  - SG interplin dynamic (`e2e-interplin.md`, before vs. final after, 12 rounds): `expensive/` −213 ms (1.072x; `-ramp` 1.288 → 1.092 s, 1.180x), cached −87 ms (c100us-cached 1.47x, c1us-cached 1.07x), d10 L5 1.013x.
- **Costs**:
  - Grid construction (1 t micro): DSG d20 L6 16.8 → 18.9 ms; d15 L7 +0.7 ms; SG d20 L5 +0.4 ms; SG d5 L10 +23 µs. At 16 t, e2e generation: DSG d20 L6 11.0 → 13.9 ms (+0.25 % of the case).
  - SG interplin dynamic: d2 L16 p16384 0.959 [0.923, 0.997] (static kept each thread's node rows in its caches across calls), d5 L8 0.987; 1 t micro: calls of 1–90 µs 2–4 % slower (hand-out overhead), ≥ 250 µs neutral (`micro-interplin.md`).
  - `expensive/ cached` (unranked, order only) 1.043 → 1.089 s: SG quad c100us-cached 0.927, interp cached 0.976. Cause: the 16 largest, overlapping TGs now miss concurrently in the cache. Counted: 97.1–98.0 k instead of 90.56 k underlying evaluations (`results/WP-77/cached_eval_count.cpp`).
- **Memory**: geomean B/A 1.005. MEM+: quad/dsg b1 d15 L7 51.6 → 54.4 MiB, b1 d20 L6 82.8 → 86.3, b0 d3 L16 43.8 → 45.2. MEM−: quad/dsg b0 d10 L7 30.1 → 27.8, cc-uq d10 L7 30.0 → 28.8. The persistent part is the order (8 B per TG; grid heap of DSG d20 L6 19.35 → 21.19 MB); the rest is presumably the construction-time size-class buffer kept by the allocator.
- **API / functionality**: no change in results or behaviour. One addition: a public getter on `SparseGridBase` documented as internal to the operators (like `getCrossCallOperatorDataCache`), not in Python.
- **Maintainability**: about 60 lines of source (one helper file + 1 line per loop) and a constant; no new code paths.

## Tests
- Pinning (passed on the unchanged code, debug ×3): `thread_count_independent_quadrature`, `thread_count_independent_operators` (1/2/3/16 t, repeated 16-t calls, bitwise equal to the serial TG-order compensated combination). Both were extended with a strongly skewed MI set (`mivec skewed d3`: TGs of 1 to ≈ 4000 points, the largest above a thread's share) for cc / equi / cheb1, b0 / b1, SG + DSG. `grid_storage_pinning` and `repeated_operator_calls` are unchanged and pass.
- New: `tensor_grid_processing_order` (6 cases): size classes and their monotonicity, stable counting sort, floor class, grid orders (SG / DSG, skewed / complete / 1D, b0 / b1), and index order on DSG b1 d15 L7.
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ (memory-capped). Flaky `equidistant_nodes` did not recur.

## Reference check
- `perf.py refcheck`: WARN (1 and 16 t), only the known owner-accepted `interp/sg/equi/b0/d2/L5/cos-shifted` (x2.34, the same in WP-77-before); 145 OK.
- Bits (`results/WP-77/reference_bits.txt`): 0 of 146 cases differ between before and after at 1 / 2 / 3 / 16 / 16 t, and 0 of 146 depend on the thread count (checked for the order-only tree and again for the final tree with interplin dynamic). Refcheck and normal tests ×3 re-run on the final tree: same result.

## Benchmarks (A = WP-77-before, B = WP-77-order = order only, interplin static; speedup = A/B, > 1 is faster)
`WP-77-after` (final tree) = `WP-77-order` + interplin dynamic; its own A/B: `e2e-interplin.md` / `micro-interplin.md` (numbers under Gain vs. cost).
L1 micro (1 t, `results/WP-77/micro.md`). The drivers are neutral (per-TG kernels unchanged):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quad/operator (12 cases) | | | 0.97–1.006 | all ~ |
| micro/dsg/operator/equi/b1/d15/L4 | 20.51 ms | 20.56 ms | 0.998 | ~ |
| micro/interp/sg/cc/b0/d5/L5 | 61.65 ms | 62.20 ms | 0.991 | [0.987, 0.994] |
| micro/interplin/sg (9 cases) | | | 0.996–1.006 | all ~ |
| micro/gen/dsg/equi/b1/d20/L6 | 16.77 ms | 18.86 ms | 0.886 | [0.871, 0.902] |
| micro/gen/sg/equi/b1/d10/L7 (new) | 2.33 ms | 2.42 ms | 0.961 | [0.952, 0.971] |

L2/L3 e2e (16 t, full suite `results/WP-77/e2e-full.md`; relevant filter `e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| expensive/mivec/sg/…/d15/S6/n300/c100us | total | 2.156 s | 1.564 s | 1.377 | [1.372, 1.382] |
| expensive/mivec/dsg/…/d15/S6/n300/c100us | total | 880.2 ms | 551.7 ms | 1.596 | [1.590, 1.602] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 377.5 ms | 323.4 ms | 1.159 | [1.135, 1.184] |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.7 ms | 81.0 ms | 1.118 | [1.111, 1.125] |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.4 ms | 83.9 ms | 1.485 | [1.366, 1.614] |
| quad/dsg/equi-exp/b1/d20/L6 | total / generation | 1.137 s / 11.0 ms | 1.141 s / 13.9 ms | 0.995 / 0.80 | ~ / SLOWER |
| interplin/dsg/equi/b1/d2/L16 (untouched code) | operator | 150.9 ms | 152.6 ms | 0.985 | [0.973, 0.996] |

Sum of medians (full suite): suite A = 36.326 s, B = 35.935 s, saved 391 ms · `expensive/` saved 1.858 s · cached −46 ms. The `interplin/dsg` row is code placement: that operator does not read the order. Full reports: `combigrid/perf/results/WP-77/`.

Peak memory (full suite): 3 MEM+ / 2 MEM− (values under Gain vs. cost) · geomean B/A = 1.005 · process floor A = B = 10.3 MiB

## Attempts not kept
- **Order without a floor** (all TGs sorted; `results/WP-77/unfloored/`, snapshot `WP-77-unfloored`): suite −365 ms. Lost locality on grids of many tiny delta TGs: quad/dsg b1 d20 L6 operator 0.981 [0.976, 0.985], d15 L7 0.991. The floor (total/4096, ≥ 99.8 % of the modeled efficiency) made both neutral: suite −422 ms on the same filter.
- **Split variant (TGs above the balanced share in row ranges)**: not implemented. Model (`split_sim.py` → `split-sim.md`, cost = points): with the order, only two grids remain above the bound:
  - `expensive/mivec/sg` d15 S6 (largest TG 1.72x the share): both c100us cases −0.65 s each, ≈ −1.2 s;
  - `mivec/sg` d15 S10 (1.36x): ≈ −7 ms.
  - Every other case is ≥ 0.998 already or not helped. d2 L20 / interp d2 stay at 0.93–0.94: their largest TG is below the share, the loss is the hand-out granularity.
  - The cost: a row-range entry into `weighted_sum::sumOverRows`, a per-call task list with a thread-count-independent threshold, partial buffers (scalar + vector), a round-off change. A row-dimension TG (e.g. 2^21 × 3 nodes) gives only 3 rows → needs sub-row splitting.
  - → Owner decision: worth it only if mivec-like SG grids with expensive functions matter (DSG already reaches the bound there: 0.552 s vs. SG 1.564 s).

## Profile shift / follow-ups
- Cache: concurrent misses on the same key evaluate it twice. Waiting on in-flight keys in `SourceFuncCache` would turn the cached SG quad's +7 % into a gain (≈ 7 k duplicate 100 µs evaluations per call on d10 L4).
- Memory: a `uint32_t` order halves the persistent part (DSG d20 L6 −0.9 MiB) but needs a guard for nTG ≥ 2^32. Avoiding the size-class buffer would need a second nGP pass.
- Adaptive refinement: the order is per grid, so a refined grid (a new `SparseGrid` from `MIVecSGGenInstr`) recomputes it in O(nTG). An in-place-growing grid would need an incremental merge into the counting-sort buckets; the floor depends on the total, so it shifts slowly.
- SG interplin dynamic costs d2 L16 4 % (locality) and small 1-t calls 2–4 %: a static schedule for cheap calls (e.g. by grid size or a per-call cost estimate) could win both back, if it pays its complexity.
- SG interp at 1 t is 0.4–0.9 % slower in micro (dynamic hand-out vs. guided). Not visible at 16 t.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add `--keep combigrid/tests/operators/thread_count_independent_operators_test.cpp --keep combigrid/tests/operators/quadrature/thread_count_independent_quadrature_test.cpp` to keep the skewed-grid pinning configs)
