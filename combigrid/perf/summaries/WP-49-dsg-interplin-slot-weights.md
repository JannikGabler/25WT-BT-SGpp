# WP-49 — DSG linear interpolation: global slot weights by sum factorization (+ small-call gate)

Status: PROPOSED · Date: 2026-11-17 · Base commit: 0c5071770
Recommendation: accept. `interplin/dsg` is 1.50–1.58x faster at d ≥ 5 and 1.13x faster at d2 (e2e, 16 t), saving 0.363 s over `^interplin/`. Results no longer depend on the thread count, and the evaluation set is unchanged. One test needs an owner decision: the bitwise goldens of `grid_storage_pinning` were re-pinned for DSG entries only.
Machine: `perf.py calibrate` session start e2e 1.001 / 0.999, micro 1.000 / 1.005; before the A/Bs e2e 1.003 / 1.000, micro 1.030 / 1.034 (burst / sustained)

## Change
- **Slot weights by sum factorization** instead of WP-47's per-TG mark and gather walks (2^d corners per TG).
  - Definition: W(λ, s) = Σ_ℓ c_ℓ Π_j v_j(ℓ_j; id_j).
  - Computation: d in-place 1D transforms along the **fibers** of the closure. A fiber of dimension k is the set of leaves that differ only in ℓ_k. Each level adds its input, times the row's two factors, to the two ids that bracket x_k at that level, in ascending order of the level. Cost: O(d · slots), independent of nTG and 2^d.
  - This does not use the trie over the box [λ, last(id)] that the prompt suggested. A per-slot box sum would cost Σ_slots |box|; the fiber transforms reuse the partial sums.
- **Marking derived from the weight pass**: a `bool` flag per slot is transformed alongside the weight, with "or" instead of the sum, starting from c_ℓ ≠ 0. It ends true exactly at the corners of the combination TGs, including slots whose weight cancels to 0. So the evaluation set and count are WP-47's. This is confirmed by `pointHash` in `grid_storage_pinning` (unchanged in all 108 DSG configurations) and by the counting tests.
- **Evaluate and sum**: each reached slot is evaluated once and W·f is summed per fixed chunk of 16 leaves (`DSG_SUM_CHUNK_LEAVES`); the chunk sums are then added in order. The result is bitwise independent of the thread count. Barriers per call: d + 1 (WP-47: 2). The value store is gone: memory per slot is 8 + 1 B for any m (WP-47: 8m + 1).
- **Details that paid off**:
  - Dim-0 fibers are contiguous runs of leaves: their range is zero-filled and only nonzero coefficients are pushed (d2 9.1 → 7.9 µs, bitwise identical).
  - The reach array is `bool`, not `uint8_t`: stores of a char type may alias everything and forced reloads of the index pointers every level (d2 9.6 → 9.1 µs).
  - The per-call doubles share buffers (`std::span`s), and the per-thread scratch needs 2 allocations; 10 → 7 allocations per scalar call (d2 7.86 → 7.66 µs at 6 allocations).
- **Context / scratch split** (owner, after the measurements): WP-47's `DSGLinInterpQuery` mixed read-only and written data. It is split along the convention "context = set once, then only read; scratch = written temporaries":
  - `misc::DSGLinInterpContext` (`data_contexts/`): corner ids, 1D factors, coordinates. Filled by `prepareContext` (formerly `prepareQuery`), then passed as `const&`.
  - `misc::DSGLinInterpSlotScratch` (`data_scratches/`): slot weights, reach flags, chunk sums. One per call, shared by the threads, with disjoint writes per pass.
  - `DSGLinInterpScratch` stays per thread.
  - Purely structural. Micro A/B `WP-49-after` vs. the split build (exploratory, 5 rounds): d10 1.012, d8 1.018, d5 0.996, d2 0.990, vec 0.992 / 0.996, sum 1.007x — all within the placement band; d2 pays one more allocation (6 → 7). The e2e results below are those of `WP-49-after` (before the split).
- **Small-call gate re-measured** (16 t, cosProd, per call, serial vs. parallel): 132 visits 8.4 vs. 12.4 µs, 512 visits 15.0 vs. 16.3 µs, 872 visits 25.0 vs. 19.4 µs, 1104 visits 29.7 vs. 24.9 µs. `DSG_PARALLEL_MIN_CORNER_VISITS` therefore changes from 256 to 640. No judged case changes its path: d2 has 132 visits, the others ≥ 39k. The d2 call has bypassed the parallel region since WP-47.
- Files:
  - `tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.{hpp,cpp}` (`transformFiber`, `transformDimension`, `evaluateChunks`, `sumChunks`; mark, gather and corner walk removed)
  - `miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.{hpp,cpp}` (fibers, leaf coefficients, blocks of ≥ 1 slot); `..._query.{hpp,cpp}` renamed (`git mv`) to `..._context.{hpp,cpp}`
  - new `data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.{hpp,cpp}`; `data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp`, `operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.{hpp,cpp}` (driver; Doxygen only in the header), `constants.hpp`
  - Tests: `operators/deltas_sparse_grid_linear_interpolation_test.cpp`, `grids/grid_storage_pinning_test.cpp`
  - Docs: CLAUDE.md §2, `claude/module-reference.md`
- `PLAN.md` / `STATUS.md` were also modified in the tree during the session (the master's edit from 16:24), not by this WP.

## Gain vs. cost
- Gain (e2e, 16 t, 8 rounds, `total`): d10 430 → 272 ms (**1.58x**), d8 371 → 240 ms (**1.54x**), d5 183 → 122 ms (**1.50x**), d2 178 → 158 ms (**1.13x**). That is 0.363 s saved over `^interplin/` (4.670 → 4.307 s); `sg` controls are within noise.
  - Against the prompt's estimate (0.15–0.25 s + 0.07 s at d2): d ≥ 5 saves 0.34 s, more than expected. d2 saves only 0.021 s; it stays slower than `interplin/sg` d2 (114 ms at 16 t), because the d2 call is serial and SG d2 parallelizes.
- Memory: geomean B/A 0.995, 1 MEM+ and 1 MEM−.
  - MEM+ `dsg/d10` 15.3 → 15.9 MiB (1.040, spread 5.3 / 3.9 %); MEM− `dsg/d8` 12.8 → 12.3 MiB.
  - Per call (m = 1) unchanged at 9 B per slot (d10: 370,688 slots = 3.2 MiB); for m > 1, 8(m−1) B per slot less.
  - Per grid, the index grows by the fibers (d · nLeaves · 8 B of blocks, 9 B per fiber) plus 8 B per leaf (d10 L5: 20,020 fibers, +434 KiB).
- API / functionality: none. Public signatures are unchanged; the Doxygen now states thread-count independence. Results change bits (round-off only). Python rebuilt with `SG_PYTHON=1`, all 11 examples ✓; the DSG example gives SG − DSG = 2.8e-17 with 170 vs. 512 evaluations.
- Maintainability: the operator code shrinks (mark, gather and `forEachCornerSlot` are gone). The fiber transform is ~60 lines plus Doxygen with the derivation. Net +105 lines in src/tests (545+ / 440−), mostly tests and docs.
- Refinement (owner 10-29):
  - Per-TG / per-MI sums are no longer formed on this path.
  - Helpful: the transposed transform (pull values back through the same 1D maps) would give I_ℓ f(x) for **every** closure MI in O(d · slots), which is cheaper than WP-47's per-TG gather. The fibers are built for any downwards-closed set (O(d² · |closure|), `findLeaf`).
  - Blocking as before: the index is immutable and needs invalidation or rebuild when the MI set grows.

## Tests
- Pinning (green on the unchanged code, debug ×3):
  - `dsg_linear_interpolation/*` (DSG == SG, evaluation set and count, vector), extended before the change with d10 L2 (parallel) and d2 L16 (small call, serial) in the DSG == SG / counting configurations and d2 L16 in the vector test;
  - `grid_storage_pinning` (`pointHash`).
- New: `dsg_linear_interpolation/Results_Independent_Of_Thread_Count`, bitwise for 1 vs. 2 / 3 / 16 threads, scalar + m = 3, on d2 L16 (serial), d5 L5 and d10 L2. It fails on the unchanged code (112 checks; WP-47's reduction depends on the thread count) and passes now. Vector m = 3 vs. 3 scalar runs within round-off is the existing `Vector_Outputs_Equal_Scalar_Interpolation` (+ d2 L16).
- **Re-pinned (owner decision)**: the `grid_storage_pinning` bitwise hashes of the DSG configurations (debug 72, opt 96 of 252) were re-pinned with `results/WP-56/repin_grid_storage.py`. Inputs and lists: `results/WP-49/golden-{debug,opt}.txt`, `repin-{debug,opt}.txt`. No point hash and no SG entry changed. Debug keeps the 36 d1 hashes (the 1D transform reproduces the per-TG sums); opt changes them too (contracted multiply-adds in the old path).
- Results: debug `--repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓. The known flaky test did not recur.

## Reference check
- `perf.py refcheck` (1 and 16 threads): 131 OK, 0 INFO, 1 WARN — the known `interp/sg/equi/b0/d2/L5/cos-shifted` x2.34 (since WP-06, identical in WP-49-before). All 16 `interplin/dsg` cases are OK, with unchanged evaluation counts (`results/WP-49/refcheck.txt`).

## Benchmarks (A = WP-49-before, B = WP-49-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `results/WP-49/micro.md`; sum 1.044 → 0.980 s):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interplin/dsg d10 / d8 / d5 | 139.6 / 14.47 / 1.36 ms | 88.45 / 9.02 / 0.698 ms | **1.579 / 1.615 / 1.965** | [1.568, 1.589] / [1.575, 1.657] / [1.894, 2.039] |
| interplin/dsg d2 (small call) | 8.19 µs | 7.63 µs | **1.069** | [1.057, 1.081] |
| interplin/dsg_vec m4 d8 / d2 | 38.47 ms / 16.27 µs | 32.22 ms / 15.54 µs | **1.208 / 1.047** | [1.180, 1.236] / [1.030, 1.064] |
| interplin/sg, sg_vec (6 controls, untouched) | – | – | 0.995–1.008 | all noise |

L2/L3 e2e (16 threads, 8 rounds, `results/WP-49/e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interplin/dsg/equi/b1/d10/L5/p32 | total / operator | 430.3 / 428.8 ms | 271.7 / 270.2 ms | **1.580 / 1.584** | [1.569, 1.591] |
| interplin/dsg/equi/b1/d8/L5/p256 | total / operator | 370.6 / 367.6 ms | 239.9 / 235.9 ms | **1.536 / 1.544** | [1.488, 1.586] |
| interplin/dsg/equi/b1/d5/L8/p1024 | total / operator | 182.7 / 182.1 ms | 122.4 / 121.9 ms | **1.496 / 1.499** | [1.485, 1.507] |
| interplin/dsg/equi/b1/d2/L16/p16384 | total / operator | 178.3 / 172.9 ms | 157.5 / 152.1 ms | **1.131 / 1.136** | [1.119, 1.143] |
| interplin/sg (4 controls) | total | – | – | 0.981–1.013 | all noise |

Sum of medians (8 cases): A = 4.670 s, B = 4.307 s, saved = 0.363 s (1.084x). `dsg/d5` `generation` is SLOWER (82.8 → 126.1 µs, +0.04 ms; also in an exploratory run); its code is untouched, so this is presumably the heap state after the warm-up's operator calls.

| case | A | B | B/A | flag |
|---|---|---|---|---|
| interplin/dsg/equi/b1/d10/L5/p32 | 15.3 MiB | 15.9 MiB | 1.040 | MEM+ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.3 MiB | 0.965 | MEM− |

Geomean B/A = 0.995 · process floor A = 10.4 MiB, B = 10.3 MiB

## Attempts not kept
- Pull formulation: per id, a list of its 1D weights over its consecutive bracketing levels (built per call), with output = Σ over the list. It started at d2 12.9 µs (inner-index loop outside), then 9.46 µs; d5 700 µs, d10 86.8 ms. The push variant was as fast, needs no per-id lists and does not rely on contiguity, so it replaced the pull variant.
- Register accumulation of the two bracketing ids, to avoid store-to-load chains on the persisting id: d5 731 vs. 709 µs, d8 9.55 vs. 9.05 ms, d10 91.9 vs. 88.2 ms, d2 7.53 vs. 7.86 µs. A branch per level and a level walk per inner index made it slower at d ≥ 5. Counters show the push loop is instruction-bound (≈ 50 instr. per level step), not chain-bound.

## Profile shift / follow-ups
- 1 t, d10: integrand ≈ 77 %, evaluate 7 %, transform 1.8 % (at the floor; WP-47: integrand ≈ 40 %, gather 26 %, mark 22 %). d5: integrand 64 %, transform 15 %, evaluate + prepareBlock 12 %. d2: transform ≈ 26 %, evaluate ≈ 25 %, integrand ≈ 30 %, prepareDimension 6 %.
- 16 t, d5: transform 17 % and libgomp + OpenBLAS ≈ 40 % of the samples (d + 1 barriers, waiting).
  - Fusing the evaluation into the last dimension's transform (chunk = fiber of dim d−1) saves one barrier and one pass.
  - Vectorizing the transform over the inner index helps fibers with many sub-index bits (d ≥ 8).
- d2 vs. `interplin/sg` d2 (0.158 vs. 0.114 s at 16 t): serial by design. What remains per call is ≈ 1.6 µs transform of dim 1, ≈ 2 µs evaluation bookkeeping and ≈ 0.5 µs brackets.
- Strict aliasing: stores through `uint8_t` / `char` arrays in hot loops force reloads of other pointers (here 5 %). Other combigrid hot loops with byte arrays are candidates for `bool` / non-char types. The hoisted pointers in `evaluateBlock` (across `SourceFunc::evaluate`) still pay and were kept.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; add `--keep combigrid/tests/operators/deltas_sparse_grid_linear_interpolation_test.cpp` to keep the extended tests; the thread-count test then fails on WP-47's code)
