# WP-04 — DSG quadrature: row loop (b0) (+ TrieNode 24 → 16 B)

Status: PROPOSED → **rejected by the owner (2026-11-03)**: code reverted, findings kept · Date: 2026-11-03 · Base commit: d76fd5272 (measured variant committed as checkpoint b15c3fccd)
Recommendation (session): accept. All 9 quad/dsg cases were faster (1.023–1.060x); the suite sum dropped 17.758 → 16.969 s (−0.79 s, 1.047x). Memory was neutral, results bitwise identical.
Owner decision: the gain does not justify the more complex row loop (block loop, compiler pragmas). The `SourceFunc` calls (WP-17) come first.
Machine: `perf.py calibrate` ratios e2e 1.001 burst / 1.000 sustained, micro 1.000 / 1.001 (re-run before the A/Bs: OK, ≤ 0.2 % off)

## Owner decision (2026-11-03): what stays in the tree
- **Kept:**
  - The tests: `Global_Weights_Of_Delta_Grid_Points_Boundary_Offset_0`, `vector_delta_quadrature/Outputs_Equal_Scalar_Quadrature_Boundary_Offset_0`, the shared `checkGlobalWeightsOfDeltaTGs` helper with the round-off-aware sum check (`BruteForceGlobalWeights::withMagnitude`).
  - The micro cases `micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256` and `micro/dsg/row_loop/...`: constant integrand, i.e. row loop + dispatch only. WP-17 should use them as its L1 reference.
  - One doc fix (missing period in the `forEachGPWithGlobalWeight` note).
- **Reverted:** the block loop, the single-pass row weights, the `unroll 1` pragmas, the functor by value, the `sumOverChildren` helper, the scratch doc changes, the 32-bit `CTCoeffTrie` construction and `tools/math/checked_narrow.hpp` with its test.
  - Not done: moving the original `compRowWeights` into the header. LTO already inlines it (no symbol in the before profile), so it would only guard against a later LTO change.
- **Re-apply:** `git apply combigrid/perf/results/WP-04/block-loop.patch` restores the measured variant (source diff d76fd5272..b15c3fccd, incl. `checked_narrow`). `trienode16.patch` applies on top of it.
- Verified after the revert: debug and normal `test --repeat 3` ✓, refcheck OK (1 and 16 threads).

## Contribution of the parts (exp case = `micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256`, row loop = `micro/dsg/row_loop/...` same cfg; instructions exact, cycles single runs ±1–2 %; cumulative)

| step | row loop instr/pt | exp instr/pt | exp cycles/pt |
|---|---|---|---|
| before | 117.6 | 226.6 | 59.1 |
| a) single-pass row weights (no `memset`) + functor by value | 117.6 | 228.7 | 61.4 |
| b) + `compRowWeights` header-inline | 110.2 | 221.3 | 57.1 |
| c) b + `unroll 1` (+ `novector`), per row | 106.8 | 217.9 | 56.9 |
| d) b + blocks of rows, no pragmas | 111.3 | 222.5 | 56.8 |
| e) b + blocks + `unroll 1` (measured variant) | 97.9 | 209.1 | 53.4 |

- a) alone needs b): it made `compRowWeights` too big for LTO to inline.
- Blocks and `unroll 1` only pay together: −12 instructions. Alone they give −3.4 and +1.
- A cheaper future option is a)+b) only: about 1/3 of the instruction gain and no pragmas. It was not measured e2e.

## Change (measured variant, reverted)
- **Blocks of rows** (`deltas_sparse_grid_quadrature_tools.hpp`):
  - `forEachGPWithGlobalWeight` iterates over blocks of rows: all nodes of projected depth 1, or as many as `scratch.rowWeights` holds.
  - The new header-inline `compBlockWeights` computes a whole block's global weights in one loop, with no source-function calls in between. It replaces the per-row `compRowWeights` (`memset` + 2 passes over the depth-0 nodes), and the per-row advance / `updateCoeffSums(…, 1)`.
  - `updateCoeffSums` now covers depths ≥ 2 only. Same visiting order and rounding sequence, so results are bitwise identical (perf_reference at 1 thread: all 104 outputs identical).
  - Cause: rows at b0 are 2–4 points long (3.05 per row at d10 L7), and the per-row machinery was ≈ 2/3 of the row loop's instructions (callgrind).
- **`#pragma GCC unroll 1`** on the short loops (children, row axpy, points, depth advance). GCC unrolled ×4/×8 with modulo dispatch and peeling for 1–4 iterations, which cost more than it saved. `novector` was not needed; it was GCC-only and slightly worse. The functor is now taken by value, and the child sum is shared (`sumOverChildren`).
- **`CTCoeffTrie` construction** uses 32-bit `order` / `rangeStarts` / `counts`, with a bound check: `std::overflow_error` beyond 2^32 − 1 non-zero tensor grids, via the new `tools::checkedNarrow`.
- Files: `tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.{hpp,cpp}`, `miscellaneous/data_scratches/delta_quadrature_scratch.hpp` (docs), `miscellaneous/ct_coefficient_trie/ct_coefficient_trie.{hpp,cpp}`, new `tools/math/checked_narrow.hpp`; tests (below); `benchmarks/perf/perf_micro.cpp` (new cases). No `.md` docs mention these internals; Doxygen updated. Python bindings unaffected (no umbrella header changed).

## Gain vs. cost
- Gain: quad/dsg b0 d10 equi 5.406 → 5.125 s (1.055x), cc 6.785 → 6.535 s (1.040x); d7 1.051–1.060x; b1 d15/d20 1.052/1.040x. Sum −0.79 s (1.047x), below the expected 1.5–2.5 s. The exp case runs at IPC ≈ 3.8 (throughput-bound, not latency), so only removed instructions count: −17 instructions/point of 227 (row loop with constant integrand 118 → 98).
- Memory: unchanged (geomean B/A 1.004, 0 flags, largest 1.033 `equi-exp/b0/d7/L10` within its 4 % spread).
- API / functionality: none (internal helpers; `forEachGPWithGlobalWeight` takes the functor by value).
- Maintainability: +1 short helper and a block loop (~+30 lines). The `unroll 1` pragmas are justified in the Doxygen notes (understood by GCC and clang).

## Tests
- Pinning (passed on the unchanged code, debug ×3): new `quadratureDelta_internals/Global_Weights_Of_Delta_Grid_Points_Boundary_Offset_0` (d5/6, complete L2/L3 + anisotropic, equi/CC, random domain, every point vs. brute force); new `vector_delta_quadrature/Outputs_Equal_Scalar_Quadrature_Boundary_Offset_0` (vector = m scalar runs at b0, d5/6); existing `Global_Weights_Of_Delta_Grid_Points(_High_Dimensions)`, `Delta_Tensor_Grids_Independent_Of_Visit_Order`, `Sub_Trie_Of_Delta_Tensor_Grids*`, `Matches_Quadrature_*`, `vector_delta_quadrature/*`. The sum-of-weights check now compares with the brute-force sum within 16 ulp × Σ|c·w| (equidistant 17-node rules are off from 1 by 8e-12); the helper is shared with the high-d test.
- New: `checked_narrow/*` (bound of the 32-bit trie construction).
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ (8 GB cap). The known-flaky test did not fail.

## Reference check
- `perf.py refcheck`: OK (104 OK at 1 and 16 threads, 0 INFO/WARN). Outputs at 1 thread are bitwise equal to WP-04-before.

## Benchmarks (A = WP-04-before, B = WP-04-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | 222.98ms | 202.37ms | 1.100 | [1.092, 1.109] |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | 282.34ms | 261.95ms | 1.078 | [1.074, 1.082] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 66.20ms | 62.22ms | 1.072 | [1.057, 1.087] |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 (new, const integrand) | 86.10ms | 66.31ms | 1.321 | [1.245, 1.402] |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 (new) | 191.55ms | 92.86ms | 1.797 | [1.426, 2.265] (A bimodal) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | 90.97ms | 85.42ms | 1.064 | [1.050, 1.078] |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | 308.72ms | 300.63ms | 1.027 | [1.023, 1.031] |
| micro/dsg/prepare/equi/b1/d20/L6 (32-bit trie construction) | 114.10ms | 108.06ms | 1.058 | [1.048, 1.067] |
| micro/quad/process_tg/equi/b0/d10/L4 (untouched control) | 13.91ms | 14.04ms | 0.987 | [0.975, 0.999] |

19 cases, sum 2.465 → 2.201 s (1.120x). SG-kernel parity: `process_delta_tg` b0 d10 is 24.7 ns/point vs. 21.8 for the SG kernel (was 26.2).

L2/L3 e2e (16 threads; `operator` within ±0.1 % of `total` in every case):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.785s | 6.535s | 1.040 | [1.032, 1.048] |
| quad/dsg/equi-exp/b0/d10/L7 | 5.406s | 5.125s | 1.055 | [1.051, 1.059] |
| quad/dsg/cc-uq/b0/d7/L10 | 1.440s | 1.369s | 1.051 | [1.047, 1.055] |
| quad/dsg/equi-exp/b0/d7/L10 | 1.136s | 1.077s | 1.060 | [1.048, 1.073] |
| quad/dsg/equi-exp/b1/d20/L6 | 1.225s | 1.177s | 1.040 | [1.035, 1.045] |
| quad/dsg/equi-exp/b1/d15/L7 | 1.222s | 1.160s | 1.052 | [1.042, 1.062] |
| quad/dsg/cc-uq/b0/d5/L12 | 248.44ms | 239.67ms | 1.037 | [1.032, 1.042] |
| quad/dsg/equi-exp/b0/d5/L12 | 201.99ms | 195.17ms | 1.036 | [1.029, 1.042] |
| quad/dsg/equi-exp/b0/d3/L16 | 93.74ms | 91.14ms | 1.023 | [1.010, 1.037] |

Sum of medians: A = 17.758 s, B = 16.969 s, saved = 0.789 s (1.047x) · Reports: `combigrid/perf/results/WP-04/` (`micro`, `e2e`, `e2e-v24-vs-v16`, `e2e-strict-aliasing`, `refcheck.txt`, `trienode16.patch`)

Peak memory: no flags. Largest: `equi-exp/b0/d7/L10` 42.0 → 43.4 MiB (1.033, spread 4.1 / 3.6 %), `b1/d20` 217.4 → 215.5 MiB (0.991). Geomean B/A = 1.004 · process floor A = 10.3, B = 10.4 MiB

## Attempts not kept
- **`TrieNode` 24 → 16 B** (32-bit weight offset + 32-bit `childEnd`, bound check in `comp1DWeights`; same block loop). A/B vs. the kept 24-B variant (`e2e-v24-vs-v16`):
  - Memory −3.0…−3.6 MiB at b0 d5–d10 (4 MEM−, geomean 0.964); b1 unchanged.
  - Runtime: b0 1.5–2.2 % slower (sum 16.870 → 17.183 s, 0.982x); b1 within noise.
  - Cause (callgrind): +4.2 instructions/point, about +1 add per child weight access, the rest register allocation. Folding the index into the base pointer gave nothing.
  - **Owner decision**: `git apply combigrid/perf/results/WP-04/trienode16.patch` (incl. tests) if the 3 MiB matter more than 0.3 s.
- Fused per-row weights without blocks: −2.7 ns/point on the row loop, but 0 on the exp case (same instruction count). Header-inline `compRowWeights`: −7 instructions/point. i0-outer reduction: +3. Per-row variant + `unroll 1`: −3.4. Blocks without pragmas: ±0. Blocks + `unroll 1` + `novector`: −18 (−20 without `novector` → kept).
- Not tried: 32-bit `projChildBlocks` (builder arrays; needs a bound for the slot count, which can exceed the number of tensor grids).

## Profile shift / follow-ups
- Row loop now ≈ 98 instructions/point (constant integrand, b0 d10 L7 s256; callgrind): `compBlockWeights` ≈ 31, block/point loop ≈ 23, call + `std::function` handler ≈ 21, `updateCoeffSums` (depth ≥ 2, slack > 0 grids) ≈ 17.
- **WP-17**: a block of rows is a natural batch: its points (n0 × nRows, coordinates differ only in projected depths 0 and 1) and all its weights exist before the first call. A batch API per block would remove ≈ 21 instructions/point of dispatch plus the per-point spill/reload; no further loop change is needed.
- `updateCoeffSums`: per-parent loop overhead dominates (1–3 children). A flat child loop with a parent index would need 4 B more per node (conflicts with the 16-B layout).
- Strict aliasing (WP-20 remainder, `toolchain.py build gcc-nsa`, `e2e-strict-aliasing`): the same source with `-fno-strict-aliasing` is 3.5–4.5 % **slower** on b0 d7/d10 (0.955–0.965, sum 14.105 → 14.698 s). The ≈ 1 % cost WP-20 found in `updateCoeffSums` / `compRowWeights` is gone; nothing left to fix at the source level.
- Dimension-adaptive CT: unaffected. The block loop works on any projected sub-trie, and the coefficient trie and its construction keep their interface. The 32-bit construction bounds the tensor-grid count at 2^32 − 1 (no practical limit).
- Pre-existing, not touched: `grids/tensor_grid.cpp:34` unused variable `sum` (warning in normal builds, in HEAD). `perf.py verify` fails on the owner's committed thesis-benchmark changes (`benchmarks/{quadrature,dsg_quadrature}`, `bachelor_thesis/comparison`, `benchmarker.hpp`), not edited: they need `perf.py protect --authorized-by-owner`.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
