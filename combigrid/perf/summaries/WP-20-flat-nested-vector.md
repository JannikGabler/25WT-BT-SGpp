# WP-20 — `FlatNestedVector` under strict aliasing

Status: PROPOSED · Date: 2026-10-28 · Base commit: b0b7faac7 (+ the master's uncommitted PLAN/STATUS edits)
Recommendation: accept. No UB in `FlatNestedVec`. A 4-line access change in the DSG trie function recovers more than WP-14's loss: quad/dsg 27.08 → 26.10 s (1.037x, −0.98 s; b1/d20 1.076x). Results are bitwise identical, with no API change.
Machine: `perf.py calibrate` ratios e2e 1.002 burst / 1.000 sustained, micro 1.000 / 1.002

## Change
- **No UB found**: `FlatNestedVec` is one `std::vector<Index>` per depth plus one `std::vector<T>`. It has no casts, no byte storage and no storage reuse; the users access it only through typed spans/views. `-Wstrict-aliasing=3/2/1` over all 543 TUs (base, combigrid, tests, benchmarks; WP-14's `sascan.py`) finds 0 warnings, before and after. combigrid's only casts are `char*` I/O (`default_serializer.hpp`) and `memcpy`, both well-defined.
- `deltas_sparse_grid_quadrature.cpp: appendSubTrieChildren`: reads the grandchild range and the leaf coefficient from the local span/pointer (`nodesOfDim[childIdx-1].childEnd`, `leafCoeffs[childIdx]`). Before, it used `coeffTrie.childBegin()` / `leafCoeffs()`, which re-walk the `FlatNestedVec` (offsets → offsets array → data → node) after every recursive call. The recursion is opaque, so this holds under either flag. Same values, same trie algorithm; −3.1 % instructions.
- `flat_nested_vector.hpp`: the "Hot loops" doc said SG++ compiles with `-fno-strict-aliasing` and that every element store reloads all pointers. It is rewritten with the measured facts (below): chained access now equals a local view, except when the loop stores pointers to `T` or `Index` values, or calls opaque functions.
- `quadrature_context.hpp` only included the header (the SG quadrature does not use `FlatNestedVec`): dead include removed. Also added the missing banner and `<vector>`, which cpplint reported once the file was re-linted.
- Files: `deltas_sparse_grid_quadrature.cpp`, `flat_nested_vector.hpp`, `quadrature_context.hpp`, `tests/miscellaneous/flat_nested_vector_test.cpp`, `benchmarks/perf/perf_micro.cpp`, `perf/scripts/toolchain.py` (variant `gcc-nsa`), `perf/results/WP-20/`.

## Gain vs. cost
- Gain: `^quad/` (22 cases) 36.25 → 35.31 s, **1.027x, −0.94 s**; quad/dsg 27.08 → 26.10 s (1.037x; WP-14 lost 0.970x there). All quad/dsg `total`s are faster except b0/d3 (noise):
  - the trie-bound b1/d20/L6 **1.076x** (−0.50 s) and b1/d15/L7 **1.068x** (−0.26 s);
  - the row-loop-bound b0 cases 1.012–1.019x. quad/sg: 13 × noise (unchanged code).
  - At 16 threads the gain exceeds the 1-thread 4.5 %, as WP-14's loss did (5 % vs 2.5 %); plausibly two hyperthreads share one front end.
- Memory: unchanged, geomean B/A 1.001, 0 `MEM+`/`MEM-` (largest 1.026x `quad/sg/cc-uq/b0/d3/L14`, inside its 5–7 % spread). Process floor 10.0 / 10.3 MiB.
- API / functionality: none. Results bitwise identical (1-thread `perf_reference` output `cmp`-equal). Bindings unaffected (no bound header changed; `quadrature.hpp` merely loses a transitive include it never used).
- Maintainability / readability: +4 lines in `appendSubTrieChildren`; the container doc is now correct for the actual flags.

## Attribution of WP-14's loss (A = `WP-20-nsa` = same source + `-fno-strict-aliasing`, B = `WP-20-before`)
- L1 (`attrib-micro`): `sub_tries` b1/d20 **0.975** [0.971, 0.979], b0/d10 0.976 [0.969, 0.982], `process_delta_tg` b1/d20 0.977. The row loop (`process_delta_tg` b0/d10 1.005, b0/d5 1.007) and `quad/process_tg` (0.995) are neutral. The loss sits in `appendSubTrieChildren` only.
- Cause: code generation, not aliasing reloads. `perf stat` of `sub_tries` b1/d20 (3 alternating runs): strict aliasing executes −0.13 % instructions but +1–2 % cycles (lower IPC). The function grows 3286 → 3388 bytes (+35 `mov`, a few more stack loads/stores). The hottest instruction is the same in both (29 % of its cycles: the mispredicted `lower_bound` branch on `child->lvl`).
- Not the container's layout: in a probe, a struct-pointer store (like `push_back`'s end pointer) makes GCC 15 reload the three array pointers of a container object with `-fno-strict-aliasing` (8 loads) but not with strict aliasing (2 loads), with or without LTO.
- Probe with the real header and judged flags (`results/WP-20/fnv_probe.cpp`), ns/element:

| access | chained `fv[i][j][k]` | local view/span | nested vector |
|---|---|---|---|
| writes, `-fno-strict-aliasing` | 4.49 | 0.66 | 1.76 |
| writes, strict aliasing | 0.64 | 0.64 | 0.67 |
| reads + `push_back({const double*, size_t, double})`, strict aliasing | 0.77 | 0.57 | – |
| same, `Index = uint32_t` | 0.75 | 0.57 | – |

  The remaining gap is the `const double*` store: GCC gives `double*` and `const double*` one alias set, so it aliases the container's `T*`. A narrower `Index` (so `size_t` stores stop aliasing the offsets) does not help here, so an offset-layout change is not worth it.
- Final state vs. the old flag (A = `WP-20-nsa`, B = `WP-20-after`; `attrib-after-{micro,e2e}`): L1 `sub_tries` b1/d20 1.021 [1.011, 1.032]. e2e quad/dsg 26.27 → 26.10 s (1.006x): b1/d20 1.020x, b1/d15 1.018x. One residual: `equi-exp/b0/d10/L7` 0.990 [0.985, 0.994] SLOWER, a row-loop-bound case (`updateCoeffSums`/`compRowWeights`, no `FlatNestedVec`), so it is outside this WP (→ WP-04).

## Tests
- Pinning tests (passed on the unchanged code before the change): `testFlatNestedVec/*` (20 cases), `quadratureDelta_internals/{Coefficient_Trie_Structure, Coefficient_Trie_Of_Sparse_Grids, One_Dimensional_Weights, Sub_Trie_Of_Delta_Tensor_Grids, Global_Weights_Of_Delta_Grid_Points, Delta_Tensor_Grids_Reproduce_Quadrature}` (d = 1–5, offsets 0/1/2/1000), `quadratureDelta_operator/Matches_Quadrature_*`, `quadrature_test`.
- New tests: `testConstAccessPaths`, `testElementsOfTheIndexType` (`T == Index`, chained read-modify-write, `back()` writes as in `CTCoeffTrie`), `testStructElements`, `testGrowthAcrossReallocations` (views re-obtained after > 3 reallocations, `shrink_to_fit`), `testReserveKeepsContents`.
- New micro cases: `micro/dsg/sub_tries/equi/b1/d20/L6/s256` and `.../b0/d10/L7/s16` (`compSubTrie` for every s-th delta grid: the per-delta-grid trie work that `process_delta_tg` on the largest grid misses).
- Results: debug `test --repeat 3` ✓ · normal ✓ (3/3) · ASan ✓ (capped at 8 GB) · strict-aliasing scan 0/0/0 ✓

## Reference check
- `perf.py refcheck`: **OK**, 104 OK at 1 and 16 threads (`refcheck.txt`); 1-thread outputs bitwise identical to `WP-20-before`.

## Benchmarks (A = WP-20-before, B = WP-20-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `micro.md`, `micro-b0d10-r8.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | 173.40 ms | 167.76 ms | 1.045 | [1.007, 1.084] |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | 23.68 ms | 22.96 ms | 1.036 | [1.019, 1.053] |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 2.81 ms | 2.71 ms | 1.034 | [1.026, 1.042] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 (8 rounds) | 68.85 ms | 68.91 ms | 1.000 | [0.959, 1.043] |
| micro/dsg/prepare/equi/b1/d20/L6 | 328.66 ms | 329.18 ms | 0.997 | [0.986, 1.007] |
| micro/quad/process_tg/equi/b0/d10/L4 | 13.84 ms | 13.88 ms | 0.997 | [0.985, 1.009] |

L2/L3 e2e (16 threads; `total` and `operator`; `e2e.md`):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.024 s | 6.521 s | 1.076 | [1.072, 1.080] |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.934 s | 6.432 s | 1.077 | [1.073, 1.081] |
| quad/dsg/equi-exp/b1/d15/L7 | total | 4.085 s | 3.826 s | 1.068 | [1.064, 1.072] |
| quad/dsg/cc-uq/b0/d10/L7 | total | 7.012 s | 6.936 s | 1.012 | [1.006, 1.018] |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.663 s | 5.573 s | 1.017 | [1.013, 1.021] |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.217 s | 1.196 s | 1.019 | [1.014, 1.024] |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.509 s | 1.488 s | 1.013 | [1.009, 1.017] |

Sum of medians (22 cases): A = 36.254 s, B = 35.309 s, saved = 0.944 s (1.027x); foreign load 0.2 %. All 13 quad/sg cases are noise. The sanity line (`quad/sg/cc-uq/b0/d7/L7` 2.1e-13 → 4.3e-12) is 16-thread reduction order in unchanged code: A's own rounds scatter by the same 5e-12.

Peak memory (max RSS per process, same report; largest and DSG high-d):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 1060.7 MiB | 1066.5 MiB | 1.005 | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 326.4 MiB | 329.3 MiB | 1.009 | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 231.3 MiB | 232.9 MiB | 1.007 | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.3 MiB | 41.3 MiB | 1.026 | ~ |

Geomean B/A = 1.001 · process floor A = 10.0 MiB, B = 10.3 MiB

## Attempts not kept
- V2 (offsets of a type distinct from the payload, so that `size_t` stores stop aliasing them): probed with `Index = uint32_t` first. Chained read+push 0.75 vs 0.77 ns, local 0.57 in both, so there is no gain to implement; the alias that matters is pointer-to-`T`.

## Profile shift / follow-ups
- For WP-02: the trie work is bound by the `lower_bound` branch (29 % of `appendSubTrieChildren`) and `push_back`/recursion overhead, not by `FlatNestedVec` access. Keep the views/spans local wherever the loop `push_back`s `TrieNode`s (`const double*` field) or recurses. The per-call `context.weights1D[dim]` view (≈ 6 loads) could become per-delta-grid precomputed data if WP-02 restructures `compSubTrie`. Chained element writes need no workaround any more. `CTCoeffTrie::childBegin/childEnd` are fine outside hot loops; hot code should take `nodesOfDepth()` once.
- For WP-04: strict aliasing still costs ~1 % on `quad/dsg/equi-exp/b0/d10/L7` (the row loop, vs. `WP-20-nsa`). The `gcc-nsa` variant (`toolchain.py build gcc-nsa`) allows the same attribution there.
- Pre-existing lint findings that resurfaced when touched files were re-linted: cpplint's `<algorithm>` hint for the member `swap` in `flat_nested_vector.hpp` (false positive) and `TODO` without username in `quadrature.cpp:242`.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply --keep combigrid/perf/PLAN.md combigrid/perf/STATUS.md`
(keeps `summaries/` and `results/`). `--keep` protects the master's still uncommitted PLAN/STATUS edits; it takes several paths after one flag, and a second `--keep` would replace the first (checked with the dry run). Append
`combigrid/tests/miscellaneous/flat_nested_vector_test.cpp combigrid/benchmarks/perf/perf_micro.cpp combigrid/perf/scripts/toolchain.py`
to keep the tests, the micro cases and the `gcc-nsa` variant.
