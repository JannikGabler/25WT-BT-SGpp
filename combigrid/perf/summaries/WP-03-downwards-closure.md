# WP-03 — Downwards closure of multi-index sets

Status: PROPOSED · Date: 2026-10-29 · Base commit: 50a8737e0
Recommendation: accept — `^mivec/` 5.08x (6.32 → 1.24 s, −5.08 s), same closure incl. order, refcheck OK, memory unchanged
Machine: `perf.py calibrate` ratios e2e 1.010 (burst) / 1.000 (sustained), micro 0.996 / 1.002

## Change
- `genMIVecDownwardsClosure` (`tools/downwards_closedness.hpp`): the bounding-box scan with a dominance test against all Pareto maxima (O(|box|·|P|·d)) is replaced by a recursive enumeration over the dimensions, last dimension outermost (`appendClosurePart`, `appendClosureRow`). Each level keeps the input MIs that dominate the entries fixed so far; the entries of the current dimension run up to their maximum. This yields the closure directly in colexicographic order (= old box order, dimension 0 fastest) without duplicates, dominance tests, hashing or sorting. Generators = all input MIs: computing the Pareto maxima costs more than it saves (see below). Serial (µs–ms); scratch = one pointer per input MI and per positive entry in dims ≥ 1. `mergeLocalClosures` is removed.
- First step, measured separately: `tools::mi_view::lessEqualInEveryDim` gets `#pragma GCC novector` (guarded by GCC ≥ 14, so other compilers are unaffected), a scalar early-exit loop instead of GCC 15's AVX2 early-break loop with its alignment prologue. Since the new closure no longer reaches it, its remaining effect is on `MIVec::paretoMaxima()` (non-DWC) and view `<=`/`>=`. It can be kept or dropped independently (3 lines).
- Files: `src/.../tools/downwards_closedness.hpp` (+ copyright banner), `src/.../tools/multiindex_view/multiindex_view_utilities.hpp`, `benchmarks/perf/perf_micro.cpp`, `tests/multiindex_vector/multiindex_vector_downwards_closure_test.cpp`, `tests/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction_test.cpp`

## Gain vs. cost
- Gain: `^mivec/` total 5.08x, saved 5.08 s (expected ≈ 4.5 s): d10 3.87x (1.89 s → 484 ms), d15 71x (1.87 s → 26 ms), d20 531x (1.83 s → 3.6 ms), d5 within noise. Generation d10/d15/d20: 1.41 s / 1.84 s / 1.83 s → 7.5 / 2.4 / 1.3 ms. The pipelines are now operator-bound. Step 1 alone: 1.41x, −1.85 s (expected −1.0 s).
- Memory: unchanged (geomean B/A 0.992, no flag; largest 1.002 at d15).
- API / functionality: none. Same closure, same order (pinned), same nDim = 0 / empty behavior. Bound declarations are unchanged. A side effect is gone: the old closure computed and cached `paretoMaxima()` in the (const) input MIVec; the new one does not compute them (not observable).
- Maintainability / readability: header +118/−50 lines (mostly Doxygen, incl. the banner). The recursion is 30 lines of code; the order follows from the construction (documented). Worst case: many duplicated/dominated inputs (e.g. 10^5 copies of one MI) cost n·|box|. Pareto reduction is cheaper only there.

## Tests
- Pinning tests (passed on the unchanged code before the change; new, as nothing pinned the order): suite `MIVec_downwardsClosureOrder` (explicit sequences, zero MI, empty/nDim = 0, single MI, duplicates, already-closed inputs incl. complete sets d1–4, uniform random d1–4, sparse anisotropic d5/d10/d15/d20 with fixed seeds, time-seeded set, thread-count independence 1/3/16; oracle = colex-sorted union of predecessor boxes) and `OutputOrderIsColexClosureOrder` (`MIVecSGGenInstr` full + reduced: MIs, order, coefficients). Existing: `MIVec_downwardsClosure`, `MIVec_isDownwardsClosed`, the `MIVecSGGenInstr` tests.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan ✓. Python bindings: SWIG generation clean (only warning 389). Wrapper compile/examples not possible on this machine (no NumPy).

## Reference check
- `perf.py refcheck`: OK (104/104 at 1 and 16 threads; `closureSize`, `closureCoeffHash` and structure hashes identical)

## Benchmarks (A = WP-03-before, B = WP-03-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| downwards_closure/d10/S10/n300 | 583.95 ms | 323.24 µs | 1807 | [1800, 1814] |
| downwards_closure/d20/S6/n100 | 17.63 ms | 10.91 µs | 1615 | [1597, 1633] |
| downwards_closure/d5/S16/n1000 (new) | 11.78 ms | 487.55 µs | 24.2 | [23.9, 24.5] |
| downwards_closure/d10/S12/n1000 (new) | 12.718 s | 1.54 ms | 8237 | [8206, 8268] |
| downwards_closure/d15/S10/n300 (new) | 15.020 s | 570.02 µs | 26356 | [26235, 26478] |
| downwards_closure/d20/S7/n300 (new) | 15.847 s | 110.56 µs | 143331 | [141934, 144742] |
| pareto_maxima/d10/S12/n1000 (new) | 6.00 ms | 6.00 ms | 1.003 (noise) | [0.986, 1.021] |
| pareto_maxima/d20/S7/n300 (new) | 123.96 µs | 90.93 µs | 1.364 | [1.357, 1.371] |
| ct_coeffs/d10/S12/n1000 | 15.97 ms | 15.83 ms | 1.022 (noise) | [0.983, 1.063] |

The micro closure times include ≈ 120 page faults per call (the harness frees the closure after each call; d15: 78 883 faults per run, ≈ 4 µs each). A scratch loop measures 90 µs for d15. In e2e the closure is built once.

L2/L3 e2e (16 threads; `total` and the changed phase):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 736.80 ms | 729.54 ms | 1.012 (noise) | [0.959, 1.067] |
| | generation | 4.65 ms | 3.03 ms | 1.615 | [1.362, 1.915] |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.887 s | 483.82 ms | 3.870 | [3.754, 3.990] |
| | generation | 1.406 s | 7.50 ms | 188.1 | [185.7, 190.5] |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.865 s | 26.14 ms | 71.04 | [70.30, 71.79] |
| | generation | 1.841 s | 2.42 ms | 762.2 | [757.5, 766.9] |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.831 s | 3.61 ms | 530.5 | [481.6, 584.4] |
| | generation | 1.829 s | 1.30 ms | 1620 | [1187, 2210] |

Sum of medians (compared cases): A = 6.320 s, B = 1.243 s, saved = 5.077 s · Step 1 alone (`e2e-scalar`, B = WP-03-scalar): A = 6.347 s, B = 4.500 s, saved = 1.847 s (d10/d15/d20 total 1.431/1.443/1.598x, d5 noise; micro closure d10/d20/d5 1.562/1.587/1.189x) · Full reports: `combigrid/perf/results/WP-03/` (`micro`, `e2e`, `micro-scalar`, `e2e-scalar`, `refcheck.txt`)

Peak memory (max RSS per process, same report; no case flagged):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| mivec/.../d5/S16/n1000 | 16.1 MiB | 16.0 MiB | 0.995 | ~ |
| mivec/.../d10/S12/n1000 | 17.8 MiB | 17.3 MiB | 0.973 | ~ |
| mivec/.../d15/S10/n300 | 12.9 MiB | 12.9 MiB | 1.002 | ~ |
| mivec/.../d20/S7/n300 | 12.3 MiB | 12.3 MiB | 0.999 | ~ |

Geomean B/A = 0.992 · process floor A = 10.4 MiB, B = 10.3 MiB

## Attempts not kept
- Pareto maxima as generators (exploratory scratch loop, 1 thread, not perf.py): recursion d5/d10/d15/d20 0.27/1.26/0.08/0.03 ms vs. all inputs 0.55/1.44/0.10/0.04 ms, but the non-DWC Pareto maxima cost 0.32/1.08/0.14/0.14 ms, so slower in total everywhere.
- Exact sizing (counting pass, one `reserve`, then filling; scratch): 0.79/1.48/0.11/0.055 ms vs. geometric growth 0.57/1.24/0.091/0.041 ms. The counting pass costs more than reallocation saves.

## Profile shift / follow-ups
- `mivec/.../d10` (16 threads) after: `expSum` + `exp` ≈ 68 %, `processTensorGrid` 12 %, OpenBLAS 5.5 % (thread pool, cf. WP-10); the closure is no longer visible. mivec generation ≤ 7.5 ms: low priority from now on (CT coefficients 15.8 ms at 1 thread for 14 384 MIs are the largest remaining part).
- `isMIVecDownwardsClosed` tests all 2^d unit offsets per MI (d20: 10^6). The d immediate predecessors suffice (O(n·d) lookups). Not on pipeline paths (API `isDownwardsClosed()`, tests).
- Dead code: `tools/multiindex_bounding_box_generation.hpp` and `tools/multiindex_domination.hpp` (`miVecDominatesMI`, marked "TODO Remove"; still included by `multiindex_vector_component_wise_max_test.cpp`) have no live users in `src/` any more.
- Strict aliasing: the touched code had no pre-WP-14 workarounds.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
