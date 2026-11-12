# WP-47 — `DeltasSparseGrid` linear interpolation operator (each distinct corner once per call)

Status: PROPOSED · Date: 2026-11-12 · Base commit: 6df60cb50
Recommendation: accept, and apply the drafted judged cases. The new DSG `interpolateLinear` is **4.3–4.6x / 3.2–3.3x / 1.9–2.0x faster** than the `SparseGrid` operator at d10 / d8 / d5 (16 t, two final runs; 1 t: 4.5x / 3.4–3.5x / 2.5–2.6x). Each distinct corner is evaluated exactly once; results agree within ≤ 1.7e-13. The `SparseGrid` operator is unchanged (e2e within noise). d2 stays slower (0.60x at 16 t) and is documented.
Machine: `perf.py calibrate` ratios e2e 1.001 / 1.000, micro 0.996 / 0.998 (burst / sustained; session start 1.013 / 1.000, 0.995 / 0.999)

## Change
- New public overloads `interpolateLinear(SourceFunc | VectorSourceFunc [, span], point, const DeltasSparseGrid&)` in `operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.{hpp,cpp}`. They are C++ only and not in the umbrella header, like the DSG quadrature. The Doxygen documents the evaluation count and order (they differ from the `SparseGrid` operator), the per-call memory and the d2 limitation.
- How corners map to slots: a corner coordinate in dimension j is identified by (λ_j, s_j). λ_j is the level at which the coordinate first brackets x_j; s_j picks among the ≤ 2 new coordinates of that level. The prototype's simplex rank is replaced by `misc::ClosureTrie` (`miscellaneous/closure_trie/`). It works for any downwards-closed set: the child of level l is `firstChild[node] + l`, so a multi-index needs d lookups and no comparisons, and leaves are dense ranks. The slot is `slotOffset[leaf] + sub`. Both are updated incrementally per corner (dimensions 0…t, as `moveToCorner`).
- `misc::DSGLinInterpIndex` (query-independent) is built **lazily on the first call** and kept in the DSG's generic `misc::OperatorDataCache` (`miscellaneous/operator_data_cache/`: thread-safe, one lazily built object per type, behind `DeltasSparseGrid::getOperatorData()`; one lock + scan per call, tens of ns). The grid header names no operator type, so WP-45 can attach its index the same way. It holds the trie, the sorted delta nodes per (dimension, level), copied from the tensor grids λ·e_j, and the max new coordinates per level: 2 at level 0, else counted from the node gaps, giving 1 for doubling growth. It also stores the slot offsets and the combination TGs (c ≠ 0). Grids used only for quadrature pay nothing.
- Per call (`misc::DSGLinInterpQuery`): per dimension and level, the bracket comes from the union of the delta nodes, using exactly the rules of `TensorGrid::getNeighborsForLinInterpolation`. The search gallops from the previous level's position. The query stores ids, 1D weights and coordinates mapped with `tools::mapToDomain`. The store is m doubles + 1 mark byte per slot.
- Three passes in one parallel region (`tools/operators/linear_interpolation/dsg_linear_interpolation_tools.{hpp,cpp}`):
  1. mark the corner slots of all combination TGs (static over TGs; relaxed load-then-store);
  2. evaluate the marked slots (dynamic over blocks);
  3. gather per TG, reusing WP-08's `fillWeightTable` (extracted from `prepareCorners`, inlined, bitwise unchanged), `weights1D` and the kernel's corner order and high-dimension factors. Per-TG sums are therefore those of the `SparseGrid` kernel; TGs are combined via `addScaled` in the reduction.
- **Work gate**: below `constants::lin_interpolation::DSG_PARALLEL_MIN_CORNER_VISITS` = 256 (nTG·2^d) the passes run on the calling thread **outside any parallel region**. libgomp makes a futex syscall at every barrier even in a 1-thread team: `if(false)` cost 3 syscalls per call (12.1 → 8.8 µs at d2). Crossover measured between 132 and 872 visits (`variants.md`).
- Value types: `evaluateInto(point, dst)` and `valueAt(store, slot)` added to `ScalarValues` / `VectorValues`.
- Files: new `closure_trie.{hpp,cpp}`, `operator_data_cache.{hpp,cpp}`, `data_contexts/dsg_lin_interp_{index,query}.{hpp,cpp}`, `data_scratches/dsg_lin_interp_scratch.hpp`, `dsg_linear_interpolation_tools.{hpp,cpp}`, `deltas_sparse_grid_linear_interpolation.{hpp,cpp}` (1602 lines, about half Doxygen). Changed: `deltas_sparse_grid.hpp` (generic cache member + accessor), `constants.hpp`, `scalar_values.hpp`, `vector_values.hpp`, `linear_interpolation_tools.{hpp,cpp}`. Benchmarks: `perf_micro.cpp` (+10 cases), `perf_interp_explore.cpp` ("dsg api" column). Docs: CLAUDE.md §2, `claude/module-reference.md`, `claude/python-bindings.md`.

## Gain vs. cost
- Gain: the judged suite is unchanged by design, since its cases use the `SparseGrid` operator. On the 4 interplin problems, DSG vs. `SparseGrid` operator (`perf_interp_explore`, all query points, 16 t, 7 reps):
  - Final snapshot (`explore-t16.md`): d10 1.971 → 0.432 s (**4.56x**), d8 1.218 → 0.371 s (**3.28x**), d5 0.360 → 0.181 s (**1.98x**), d2 0.106 → 0.175 s (**0.60x**). The previous snapshot (identical kernel, before the `OperatorDataCache` refactor) gave 1.861 → 0.430 s (4.32x), 3.17x, 1.94x, 0.61x. The DSG times agree within 2 %; the `SparseGrid` d10 time varies between runs.
  - 1 t, 16 points: 4.45x / 3.44x / 2.52x / 1.11x (previous snapshot 4.52x / 3.51x / 2.56x / 0.99x).
  - This beats the WP-08 prototype (3.53x / 2.48x / 1.50x / 0.31x in the same run). At d10 it is within 6 % of the prototype's "all" variant, which is not shippable.
  - Evaluations per call: ÷ 8.30 / 6.97 / 6.73 / 1.94.
- Memory: `SparseGrid` cases unchanged (geomean B/A 0.998, 0 flags). Per DSG call: 9 B per slot (d10 L5: 3.2 MiB; d8 415 KiB; d5 52 KiB); per grid: the index (d2 L16: about 2 MiB of delta nodes; judged d2 case 20.3 vs. 19.5 MiB peak, d10 16.6 vs. 13.6 MiB).
- API / functionality: new overloads only (agreed, option (c)). DSG gains the generic member `operatorData` and the accessor `getOperatorData()` (first draft: a lin-interp-specific member + friend + forward declarations in `deltas_sparse_grid.hpp`; replaced at the owner's request 2026-11-12). Python bindings rebuilt with `SG_PYTHON=1`, all 10 examples ✓ (only transitively included headers changed).
- Maintainability: ≈ 750 code lines plus docs, one generic path for all d, node families and MI sets (no dispatch). `forEachCornerSlot` is the only non-obvious part.

## Tests
- Pinning (green on unchanged code, debug ×3): `interpolation_pinning/Linear_Interpolation_Golden_Values`, `vector_interpolation/*`, `linear_interpolation_tools/Kernel_Equals_Definition_{Low,High}_Dimensions`; all still green.
- New `dsg_linear_interpolation/*`:
  - DSG == SG within round-off: equi/cc, b0/b1/b2, unit/shifted, d1–4 complete and anisotropic MIVec, plus d5 L5, d5 L4, d11, d12, d2 L9, d1 L7. Points: interior ×2, on nodes, on the domain boundary, mixed.
  - Counting function: every point the SG operator evaluates is evaluated exactly once and nothing else is evaluated (all configurations up to d11).
  - Vector (m = 3) == scalar per component == SG vector.
  - Wrong size throws; concurrent first calls share one index; slot structure for doubling growth; bracket rules; `lowerBoundFrom` == `std::lower_bound` for every hint.
- New `closure_trie/*`: ranks, child walk vs. find, rejected inputs.
- New `operator_data_cache/*`: one object per type, built once (also under 8 concurrent first calls), a throwing build stores nothing, objects deleted with the cache.
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓ (final code). Flaky tests, unrelated (time-seeded, near-zero expected values):
  - `sin_func_random_cube_3D` (known) failed once before the change and passed on rerun.
  - **New recurrence:** `equidistant_nodes/linear_func_unit_cube_1D` (SparseGrid quadrature, untouched) failed once in a normal run (expected 0.0016 from a·0.5 + b, tolerance 1e-14 relative). It passed 5/5 reruns.

## Reference check
- `perf.py refcheck`: OK, 104/104 at 1 and 16 threads (`results/WP-47/refcheck.txt`).
- Drafted judged cases in a worktree: 120/120 OK (`judged-refcheck.txt`).

## Benchmarks (A = WP-47-before, B = WP-47-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `results/WP-47/micro.md`, final snapshot, 9 common cases sum 1.000x; dsg/sg_vec cases exist in B only):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interplin/tg d10 / d8 / d5 / d2 / d16 (controls) | 178.2 / 36.40 / 2.99 µs / 203.2 ns / 23.74 ms | 177.8 / 36.41 / 3.00 µs / 212.7 ns / 23.66 ms | 1.003 / 1.000 / 0.995 / **0.950** / 1.001 | d2 [0.941, 0.959] and d5 [0.991, 0.999] SLOWER, others noise |
| interplin/sg (whole operator, 1 point) d10 / d8 / d5 / d2 | 606.3 / 47.83 / 3.74 ms / 8.49 µs | 606.1 / 47.87 / 3.76 ms / 8.52 µs | 0.998 / 1.000 / 0.985 / 0.998 | all noise |
| **interplin/dsg** d10 / d8 / d5 / d2 (B only; vs. sg in B) | – | 140.0 / 14.57 / 1.38 ms / 8.54 µs | **4.33 / 3.29 / 2.72 / 1.00** vs. sg | – |
| **interplin/dsg_vec vs. sg_vec**, m = 4, d8 / d2 (B only) | – | 38.62 ms vs. 193.4 ms / 16.63 vs. 24.50 µs | **5.01 / 1.47** | – |

The control shifts are code placement; the SparseGrid kernel's source is unchanged:
- `fillWeightTable` is inlined in both builds (0 calls in `objdump`).
- tg/d2 (0.950) appeared only after the `OperatorDataCache` refactor, which changed only the grid header and recompiled the library. `perf stat`: op-cache misses 5.4M → 27.0M at fewer instructions.
- The whole-operator control sg/d2, which runs this kernel, is unchanged (0.998). The first snapshot showed tg/d2 1.000 and tg/d5 0.993–0.994.

L2/L3 e2e (16 t, `^interplin/`, final snapshot):
- 5 rounds (`e2e.md`): all 4 within noise, but d5 / d8 / d10 all at 0.987–0.989 (sum 3.511 → 3.569 s, 0.984x).
- 8 rounds (`e2e-r8.md`): d2 0.988, d5 1.006, d8 1.016, d10 1.002, all noise. Sum A = 3.598 s, B = 3.579 s (1.005x).
- Earlier snapshots of the same operator code: 0.999x, 1.017x, 1.009x.
- Peak memory: geomean 1.001, 0 MEM±.

Judged-case draft (`results/WP-47/judged-cases.{md,patch}`): adds 4 e2e `interplin/dsg/...` cases (same problems as `interplin/sg`) and 16 reference cases (104 → 120). DSG values agree with the unoptimized SG references within 7.6e-15. No tolerance override is needed. The new cases take 0.18 / 0.18–0.25 (bimodal) / 0.37 / 0.44 s at d2/d5/d8/d10, **+1.2 s per suite pass**. Apply, then `perf.py protect --authorized-by-owner`.

## Attempts not kept (exploratory loop, `results/WP-47/variants.md`)
- Serial path inside `omp parallel if(false)`: 3 futex syscalls per call (d2 12.1 vs. 8.8 µs) → regions bypassed below the gate.
- Branchless lower bound: d2 8.8 → 9.6 µs → `std::lower_bound` (then galloping).
- Claim variant (CAS per slot, the claimer evaluates; 2 passes, 1 barrier): 16 t d10 12.91 vs. 12.83 ms, d5 269 vs. 226 µs; 1 t d10 124.7 vs. 116.2 ms → reverted.
- Per-thread mark bitsets (no shared writes): d10 12.40 vs. 12.32 ms, d8 1.61 vs. 1.51 ms, d2 14.0 vs. 8.8 µs → reverted.
- Gate 4096 (first draft) → 256 after measuring the crossover.

## Profile shift / follow-ups
- d10 at 1 t: integrand ≈ 40 %, gather 26 % and mark 22 % (≈ 10 and 8.5 ns per corner visit, both walk-bound), evaluate 6 %. Next step: global slot weights instead of per-TG gathers. Slot weight = Σ_ℓ c_ℓ Π_j w_j(ℓ_j), factorized along a trie over the box [λ, last-level(id)] as in the DSG quadrature. This drops the 2^d walk per TG but loses per-TG sums.
- d2 (≈ 9–11 µs vs. 6.3 µs for the SG operator at 16 t): about 15 allocations per call, plus per-TG setup. A single-buffer scratch/query (as `LinInterpScratch`) saves ≈ 0.5 µs.
- **WP-45 reuse** (DSG global interpolation): `ClosureTrie` (λ → dense rank / DSG TG index), the per-grid `OperatorDataCache` (`getOperatorData().get<T>(build)`), the per-call store + three-pass skeleton with the serial gate, and `evaluateInto` / `valueAt`. Global interpolation needs one value per DSG grid point (delta TG order) instead of corner slots.
- **Refinement** (owner 2026-10-29): helpful — the gather keeps per-TG (per-MI) contributions (`sumTGCorners`), and the trie / index rebuild in O(|closure|·d + nodes). Blocking: the index is immutable and keyed to the grid. A growing MI set needs an append (or rebuild) path; `ClosureTrie` cannot insert in place (flat arrays). Store values depend on x, so there is no reuse across steps.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add `--keep combigrid/benchmarks/perf/perf_micro.cpp --keep combigrid/benchmarks/perf/perf_interp_explore.cpp` to keep the benchmarks)
