# WP-02 — DSG quadrature: per-delta-tensor-grid trie work at high d

Status: PROPOSED · Date: 2026-10-28 · Base commit: 29ade3a58
Recommendation: accept. quad/dsg b1/d20 6.54 → 1.23 s (5.34x), b1/d15 3.83 → 1.22 s (3.14x), all b0 cases 1.4–4.6 % faster or noise; −110/−81 MiB peak RSS on b1 at the cost of +2–6 MiB on b0; no API change, refcheck OK.
Machine: `perf.py calibrate` ratios e2e 1.003 burst / 0.999 sustained, micro 0.998 / 1.000

## Change
- **Pruning (lever 1)**: `CTCoeffTrie::Node` gains `maxSuffixLvlSum` (max of ℓ_{k+1}+…+ℓ_{d−1} below the node; it fills the padding, Node stays 16 B). The sub-trie DFS skips children with `maxSuffixLvlSum < m_{k+1}+…+m_{d−1}`. This is necessary for any set and exact for complete grids; dead branches of other sets are still removed afterwards. Before, the DFS visited every prefix ≥ m: 2.4e9 nodes at d20 for 6.2e7 useful ones (38x). Results bitwise identical.
- **Trie construction (lever 3, serial preparation)**: the `CTCoeffTrie` constructor builds the trie depth by depth with a stable counting sort per node range (MSD radix) on a column-major copy of the levels. This replaces the `std::sort` of `TensorGridCTData*` with pointer-chasing lexicographic compares: O(n·d) instead of O(n log n) compares. The bound is computed bottom-up. Same trie, same leaf order.
- **Projected sub-trie (lever 2)**: in a dimension where the delta grid has one node (level 0 at b ≥ 1), the weight w_k(ℓ_k)(0) is constant. `compSubTrie` folds these weights into the coefficients and merges the tensor grids by their levels in the remaining dimensions P (`scratch.projDims`). The trie therefore has |P| depths instead of d. Rows run along P[0], and the row odometer and `updateCoeffSums` only touch P.
  - The DFS accumulates into lazily allocated slot blocks (one slot per level). It then copies the non-empty slots in level order into `trieNodes`.
  - Blocks are bounded by the same 1-norm argument (exact for complete grids).
  - Coefficients are summed in double-double (FMA TwoProduct + TwoSum, see Reference check).
  - At b0 every dimension is projected, so results there are bitwise those of before.
- Split (owner request, after the A/Bs below): the builder lives in `tools/operators/quadrature/projected_sub_trie_builder.{hpp,cpp}` (`tools::ProjectedSubTrieBuilder`, `[[gnu::visibility("hidden")]]`); `forEachGPWithGlobalWeight` is `[[gnu::aligned(64)]]`. Same instructions in the moved hot functions as before; the alignment makes the b0 row loop independent of code placement (see Attempts not kept).
- Files: `deltas_sparse_grid_quadrature.{hpp,cpp}`, `tools/operators/quadrature/projected_sub_trie_builder.{hpp,cpp}` (new), `ct_coefficient_trie.{hpp,cpp}`, `delta_quadrature_scratch.hpp`, `tests/operators/quadrature/delta_quadrature_test.cpp`, `benchmarks/perf/perf_micro.cpp`, `perf/results/WP-02/`.

## Gain vs. cost
- Gain: sum of medians of the 9 quad/dsg cases 26.14 → 17.91 s (**1.46x, −8.23 s**). b1/d20 5.34x (−5.32 s), b1/d15 3.14x (−2.60 s). b0 1.014–1.046x (−0.31 s together), b0/d3 noise.
  - Attribution (chained A/Bs, `total`): pruning d20 2.45x / d15 1.67x / b0 noise; trie construction 1.15x / 1.08x / b0/d10 1.015x; projection 1.89x / 1.73x / b0 noise.
- Memory: b1 **MEM−**: d20 327.6 → 217.4 MiB, d15 233.1 → 151.8 MiB. The per-thread sub-trie vectors (156 MB of the d20 heap peak in heaptrack) are gone; the peak is now the DSG itself (178 MB heap, in the generation phase). b0 **MEM+** on 6 cases, +1.9 to +6.4 MiB (+8 to +17 %): the slot arrays come on top of `trieNodes`, plus the transient level copy of the trie construction (b0/d10 +1.5 MiB of it). Geomean B/A 0.981.
- API / functionality: none. Internal only: the `CTCoeffTrie::Node` field (aggregate order {lvl, maxSuffixLvlSum, childEnd}), the `DeltaQuadratureScratch` layout, and `compSubTrie` now produces the projected sub-trie. No bound header changed (`deltas_sparse_grid_quadrature.hpp` and the trie are not in `sgpp_combigrid.hpp`).
- Maintainability / readability: about +250 lines in src (projection builder ~190 in its own file, radix construction ~60), documented in the namespace and `compSubTrie` docs. The projection plus double-double accumulation is the main added complexity; the pruning is 6 lines.

## Tests
- Pinning tests (passed on the unchanged code, debug ×3 + normal): existing `quadratureDelta_internals/*`, `quadratureDelta_operator/Matches_Quadrature_*`. New before the change:
  - `Sub_Trie_Of_Delta_Tensor_Grids_High_Dimensions`: d8/d10, b0/1/2, complete, anisotropic increasing/decreasing weights, random MIVec, levels outside the set → empty.
  - `Global_Weights_Of_Delta_Grid_Points_High_Dimensions`: d8, b1/2, brute-force oracle.
  - `Delta_Tensor_Grids_Independent_Of_Visit_Order`: shuffled order with a shared scratch vs a fresh one, bitwise.
  - `Matches_Quadrature_High_Dimensions_Boundary_Offsets`: d8–12, b1/2, complete + anisotropic (seeded random domains); tolerance scaled by Σ|c_ℓ|, because d12/L4 differs by 1.2e-11 in round-off already on the old code.
- Adapted with the code: `Coefficient_Trie_Structure` (expected nodes with the new field), `trieLeaves` (checks `maxSuffixLvlSum` by brute force), the sub-trie checks (now against a brute-force projection built from `regularNodes`/quadrature rules). New: `Coefficient_Trie_Independent_Of_Input_Order`.
- New micro cases: `micro/dsg/process_delta_tgs/equi/{b1/d20/L6/s256, b1/d15/L7/s128, b0/d10/L6/s64}` (whole per-delta-grid work, 1 thread).
- Results: debug `test --repeat 3` ✓ · normal ✓ (3/3) · ASan ✓ (capped at 8 GB). During the split, one debug run reported 2 failures whose output was lost; not reproduced in 30 further full debug runs and 120 seeded runs of the likeliest candidate (that test now prints its seed). Split: 1-thread `perf_reference` output bitwise identical to WP-02-after.

## Reference check
- `perf.py refcheck`: **OK**, 104 OK at 1 and 16 threads (`refcheck.txt`). Its DSG cases (d ≤ 10) are dominated by discretization error.
- At d15/d20 the e2e results are round-off dominated. The exact CT value (`expSum` is a product, so Σ_q c_q [t^{L−q}] Q(t)^d in `Fraction`s, with the library's 1D q(l)) gives, as (result − CT)/CT:

| case | before | projection in double (dropped) | final (double-double) |
|---|---|---|---|
| b1/d15/L7 | +1.1…1.4e-9 | −1.05e-8 | +4.0e-10 |
| b1/d20/L6 | −5.6…−6.5e-9 | +0.7…2.1e-9 | −6.7e-9 |

  The CT discretization errors are 8e-12 / 4.9e-10. The remaining d20 offset is common to both versions (rounded 1D weights × Σ|c| ≈ 5e6). The e2e sanity line stays in the same order: worst growth cc-uq/b0/d5 1.19e-13 → 1.36e-13.

## Benchmarks (A = WP-02-before, B = WP-02-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `micro.md`; repeated once because of a foreign-load WARN):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | 256.03 ms | 55.05 ms | 4.643 | [4.497, 4.794] |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | 277.65 ms | 90.90 ms | 3.057 | [3.037, 3.077] |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | 297.43 ms | 284.01 ms | 1.047 | [1.043, 1.051] |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | 167.83 ms | 19.93 ms | 8.411 | [8.334, 8.490] |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | 22.93 ms | 9.10 ms | 2.514 | [2.491, 2.536] |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 2.70 ms | 123.89 µs | 21.438 | [20.454, 22.468] |
| micro/dsg/prepare/equi/b1/d20/L6 (serial) | 332.75 ms | 113.08 ms | 2.930 | [2.889, 2.971] |

L2/L3 e2e (16 threads; `e2e.md`; attribution: `e2e-lever{1,2,3}-*.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.543 s | 1.226 s | 5.338 | [5.312, 5.365] |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.450 s | 1.126 s | 5.734 | [5.697, 5.772] |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.825 s | 1.223 s | 3.135 | [3.104, 3.167] |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.923 s | 6.827 s | 1.018 | [1.011, 1.025] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.578 s | 5.467 s | 1.018 | [1.013, 1.023] |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.211 s | 1.157 s | 1.046 | [1.028, 1.065] |

Sum of medians (9 cases): A = 26.137 s, B = 17.909 s, saved = 8.228 s; foreign load ≤ 0.9 %.
Split (A = WP-02-after, B = WP-02-split-a64 / -final, same hot code): e2e sum 17.951 → 17.760 s (1.011x; 0 SLOWER, b0/d5, b0/d10, cc-uq/b0/d10, b1/d15 faster by 0.6–1.9 %, `e2e-split-aligned.md`); micro all noise except `sub_tries` b0/d10 0.993 [0.989, 0.997] (`micro-split-aligned.md`).

`generation` of b1/d20 is SLOWER (50.97 → 59.54 ms, 0.852) with unchanged code, only in the projection A/B. Likely (not verified): the warm pipeline reuses less freed operator memory and pays more page faults.

Peak memory (max RSS per process, same report):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | 327.6 MiB | 217.4 MiB | 0.664 | MEM− |
| quad/dsg/equi-exp/b1/d15/L7 | 233.1 MiB | 151.8 MiB | 0.651 | MEM− |
| quad/dsg/cc-uq/b0/d7/L10 | 37.2 MiB | 43.6 MiB | 1.172 | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 37.3 MiB | 41.9 MiB | 1.122 | MEM+ |

Geomean B/A = 0.981 (6 MEM+, all b0, +1.9…+6.4 MiB; 2 MEM−) · process floor A = 10.4 MiB, B = 10.2 MiB

## Attempts not kept
- Projected coefficients summed in plain double (v3): round-off 9x larger at b1/d15 (−1.05e-8 vs +1.2e-9 relative to the exact CT). Cause: leaf terms are formed top-down and summed, without the nested sums' exact integer cancellation. Replaced by double-double, which costs ≤ 1–2 % (exploratory).
- Slot blocks of the global maximum width with 24-byte slots (v3): b0/d10 peak RSS 36.5 → 65 MiB (+80 %). Replaced by blocks bounded by the 1-norm and split arrays (8 B child indices, 16 B leaf slots): 41.9 MiB.
- Split without hidden visibility: exported members compile differently (`addChildren` 212 of 738 instructions differ), `sub_tries` 0.987 / 0.973 in two A/Bs.
- Split with hidden visibility but unaligned row loop: e2e 0.972x, all b0 1.3–4 % and b1/d15 2.2 % SLOWER (`e2e-split-hidden.md`) at identical instructions. Only the row loop's start moved (offset 48 → 32 in its 64-byte line): +3.2 % cycles, +18 % op-cache misses, concentrated in `forEachGPWithGlobalWeight`.
- Linear scan instead of `lower_bound` for the first child ≥ m_k: `sub_tries` b1/d20 0.948 [0.933, 0.963] SLOWER, the rest noise.

## Profile shift / follow-ups
- d20 after (16 threads): integrand ~55–60 % (`expSum` + libm `exp` + `std::function`), row loop 21 %, projection DFS (`addChildren`) 12 %. The framework is no longer the main cost at b1.
- DFS: delta grids with the same set of single-node dimensions share the projected coefficient function, so a per-thread cache keyed by that set could skip most DFS work (~12 %). The per-call `weights1D[dim]` view could also become per-grid data.
- b0 memory: `TrieNode` with 32-bit weight offset and childEnd (24 → 16 B) would roughly offset the slot arrays. The serial trie construction could use a 32-bit `order` array.
- Code placement: the b0 row loop varied by ±3–4 % with unrelated library changes before it was aligned, so part of WP-02's b0 gain (1.4–4.6 %) may have been placement luck. Other hot loops (SG quadrature, interpolation) may be just as placement-sensitive: check op-cache misses whenever a WP shows b0-like shifts in unchanged code.
- b1 peak memory is now the DSG itself (node `DataVector`s 64 MB, per-TG `LvlMI`s 37 MB at d20), outside this WP.
- Strict aliasing: no pre-WP-14 workaround in the touched code; the row loop's local copies before the opaque `SourceFunc::evaluate` stay (WP-01). Pre-existing warnings (untouched files): `tensor_grid.cpp:34` unused `sum`, `quadrature_utilities.cpp` `#pragma once`.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; append
`--keep combigrid/tests/operators/quadrature/delta_quadrature_test.cpp combigrid/benchmarks/perf/perf_micro.cpp`
to keep the tests and micro cases. The adapted trie/sub-trie tests then need the old expectations back, see the diff.)
