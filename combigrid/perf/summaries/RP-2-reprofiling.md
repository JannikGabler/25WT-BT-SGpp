# RP-2 — Re-profiling after WP-20 and WP-02 (runtime, peak memory, WP-21/WP-22 sizing)

Status: PROPOSED · Date: 2026-10-29 · Base commit: 5215417ea
Recommendation: accept (measurement only). Re-rank PLAN.md as proposed below:
- WP-03 is now clearly first: ≈ 4.5 s, vs. 1.5–2.5 s for WP-04.
- WP-21 is worth 0.1–0.2 s.
- WP-22 is minimal in runtime and material in memory; merge it with WP-18.
Machine: `perf.py calibrate` ratios e2e 1.003 burst / 0.999 sustained, micro 0.994 / 0.998 (1.000 = as at baseline time)

## Change
- None (measurement). Snapshot `rp-2` = HEAD (in-tree `perf.py build normal` of 5215417ea; "dirty" only because of the owner's untracked `compile*.sh`).
- Measurements:
  - `ab e2e baseline rp-2` (full suite);
  - direct `ab e2e rp-1 rp-2` checks of shifts in unchanged code (mivec + interplin, dsg b1);
  - `run micro` (all cases);
  - `perf.py profile` of 20 e2e cases (≥ 1 per group, high d first) + 2 micro cases, split by the inline chain of every sampled address (`addr2line -i`; method change vs. RP-1 noted in ANALYSIS.md);
  - `perf stat` / `perf record` of front-end and op-cache events;
  - cache-miss sampling of the DSG preparation;
  - heaptrack of 9 cases (separate runs, `systemd-run --user --scope -p MemoryMax=8G`);
  - a combinatorial count for WP-21;
  - a page-fault diagnosis of the b1 generation with `GLIBC_TUNABLES` (diagnostic only).
- Files: `ANALYSIS.md` (section "Re-profiling RP-2" + contents line); `results/RP-2/`:
  - reports: `e2e.md/.json`, `e2e-rp-1-vs-rp-2-{mivec-interplin,dsg-b1}.md/.json`, `micro-run.json`;
  - derived data: `groups.md`, `profile-splits.md`, `wp21-sizing.md`, `wp22-sizing.md`, `frontend-per-symbol.md`, `perfstat-frontend-rp-2.txt`, `mivec-rp-1-vs-rp-2.md`, `generation-page-faults.md`;
  - top lists and summaries: `profiles/` (22 top lists), `heap/` (9 heaptrack summaries).

## Gain vs. cost
- Gain, cumulative baseline → rp-2 (full suite, 16 threads):
  - **e2e 70.03 → 43.65 s (1.604x, 26.37 s saved)**; d ≥ 7: 59.87 → 37.10 s (1.61x).
  - Verdicts: 33 cases faster, 5 within noise, **0 SLOWER** (RP-1: 5 SLOWER).
  - Since RP-1 (53.41 s): 1.224x. quad/dsg 27.10 → 17.74 s (1.528x); mivec 6.91 → 6.35 s (1.088x, unchanged source, see Answers); quad/sg, interp unchanged; interplin 0.971x is cross-session drift (direct A/B: all noise).
  - Per group baseline → rp-2:

| group | baseline → rp-2 | speedup |
|---|---|---|
| quad/sg | 22.29 → 9.18 s | 2.43x |
| quad/dsg | 26.73 → 17.74 s | 1.51x |
| interp | 6.64 → 5.73 s | 1.16x |
| interplin | 5.09 → 4.65 s | 1.09x |
| mivec | 9.28 → 6.35 s | 1.46x |

- Memory, peak RSS geomean B/A 1.017:
  - 8 `MEM+`: quad/sg d2/L20 (WP-01 scratch) equi 363.7 → 619.7 MiB and cc 833.3 → 1072.2 MiB; 6 dsg b0 cases (WP-02 slot arrays) +1.2…+5.8 MiB.
  - 2 `MEM−`: dsg b1 d20 330.6 → 217.1 MiB, d15 230.7 → 150.9 MiB.
  - Group geomeans: quad/sg 1.064, quad/dsg 0.979, interp 0.999, interplin 1.009, mivec 1.004.
- API / functionality: none. Maintainability / readability: n/a.

## Tests
- n/a (no code changed). `perf.py build normal` + `perf.py test` passed before the snapshot. `perf.py verify` OK at start and end.

## Reference check
- n/a (no code changed).

## Benchmarks (A = baseline fed2cdb4e, B = rp-2 5215417ea; speedup = A/B)
L1 micro (1 thread, `run micro` absolute; rp-1 values from ANALYSIS.md RP-1; no micro A/B needed for an RP):

| case | rp-1 | rp-2 |
|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 27.8 ns/point | 26.12 ns/point (SG kernel `process_tg/equi/b0/d10/L4` 21.84) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 693 ns/point | 30.2 ns/point |
| micro/dsg/prepare/equi/b1/d20/L6 (serial) | 292 ms | 65.7 ms |
| micro/mivec/downwards_closure/d10/S10/n300 | 637 ms | 621 ms |

L3 e2e (16 threads, `total`, the longest rp-2 cases; full table `e2e.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.979 s | 6.772 s | 1.031 | [1.025, 1.037] |
| quad/dsg/equi-exp/b0/d10/L7 | 5.663 s | 5.406 s | 1.048 | [1.044, 1.052] |
| interplin/sg/equi/b1/d10/L5/p32 | 2.723 s | 2.515 s | 1.082 | [1.060, 1.105] |
| quad/sg/equi-sin/b0/d7/L7 | 3.534 s | 2.419 s | 1.442 | [1.385, 1.501] |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.916 s | 1.886 s | 1.015 | [0.998, 1.031] |
| quad/dsg/equi-exp/b1/d20/L6 | 6.758 s | 1.225 s | 5.517 | [5.497, 5.538] |
| quad/dsg/equi-exp/b1/d15/L7 | 3.962 s | 1.218 s | 3.262 | [3.234, 3.290] |

Sum of medians (38 cases): A = 70.025 s, B = 43.655 s, saved = 26.371 s; foreign load ≤ 1.5 % · Full reports: `combigrid/perf/results/RP-2/`

Peak memory (flagged and largest):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 833.3 MiB | 1072.2 MiB | 1.287 | MEM+ |
| quad/sg/equi-exp/b0/d2/L20 | 363.7 MiB | 619.7 MiB | 1.704 | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 330.6 MiB | 217.1 MiB | 0.657 | MEM− |
| quad/dsg/equi-exp/b1/d15/L7 | 230.7 MiB | 150.9 MiB | 0.654 | MEM− |
| quad/dsg/cc-uq/b0/d7/L10 | 37.2 MiB | 43.0 MiB | 1.157 | MEM+ |

Geomean B/A = 1.017 · process floor A = 10.4 MiB, B = 10.1 MiB

## Attempts not kept
- None. Method notes:
  - `perf report --sort symoff` with `--symbol-filter` and `perf script` with the default callchain output are unusable for splits; use `perf script -F ip,sym,symoff,dso -G` + `addr2line -a -i`.
  - Non-precise event sampling (front-end, op-cache) has skid: the row loop gets 28.8 % of the cycle samples instead of 43.1 %.

## Answers to the RP-2 questions (16-thread profiles; RP-1 values in brackets)
- **WP-04, DSG b0 d7/d10**: shares unchanged.
  - b0/d10/L7 row loop vs. integrand: equi-exp **43.1 vs. 52.5 %** [44.8 / 50.5]; cc-uq **32.8 vs. 63.6 %** [35.0 / 61.1]. d7 is the same (42.9 / 33.9 %).
  - Inside the loop: `updateCoeffSums` 14.4 / 10.9, `compRowWeights` 11.6 / 8.3, row + advance 9.4 / 7.4, `std::function` call 5.3 / 4.4 (+ `_M_invoke` 1.4 / 1.2), lambda 2.4 / 1.9 %.
  - The row loop costs 5.53 s over b0 d7/d10, of which per-row trie work ≈ 3.3 s.
  - Micro: 26.1 vs. 21.8 ns/point (SG kernel) → parity ≈ 2.1 s. **WP-04 re-estimated at 1.5–2.5 s** [1.5–3].
- **DSG b1 split**:
  - d20 (1.23 s): integrand 52.8 % · row loop 21.3 % · projection builder **13.6 %** (`addChildren` 12.0) + memset 1.0 · serial preparation **65.7 ms = 5.4 % of the wall time** (radix pass 64 % of the `CTCoeffTrie` constructor, level copy from `TensorGridCTData` 24 %) · generation 61 ms (5 %).
  - d15 (1.22 s): 55.6 / 24.8 / 9.6 % · generation 43 ms.
  - WP-21 count:
    - Sets vs. delta grids: d20 60 460 vs. 230 230 (3.8 per set); d15 16 384 vs. 170 544 (10.4).
    - A per-set cache saves only 35 / 50 % of the 6.2e7 / 4.8e7 DFS visits, because each set's smallest member must still be built.
    - The DFS also rejects 44 / 42 % of its child iterations with `continue`. A slack bound with `break` removes them and is valid for any set.
    - **WP-21 ≈ 0.1–0.2 s** (builder ≈ 0.31 s).
- **WP-03, mivec**: `genMIVecDownwardsClosure` **77.5 / 96.1 / 97.2 %** [78.6 / 96.6 / 97.6].
  - Generation is 1.06–1.13x faster than rp-1 although the source is unchanged. LTO compiled the closure differently (8.3 → 11.0 KB, −2 % instructions, IPC 1.76 → 1.94); same AVX2/alignment-test loop.
  - Closure 5.06 s at rp-2 → **WP-03 ≈ 4.5 s** [5.1]. The first step (scalar early exit, master speed 1.03 / 1.55 / 1.52 s) is now **≈ −1.0 s** [−1.55].
  - **WP-03 ranks first.**
- **quad/sg, interp, interplin**: unchanged within ≈ 1 point (`processTensorGrid` 24.9 % [25.2], `interpolateFirstDim` 18.9 / 19.9 % [19.1 / 20.2], `computeNeighborGPWithWeight` 19.9 / 16.5 % [19.8 / 16.6], cheb1 weights 24.0 % [25.8], CC d2 weights ≈ 24 % + kernel 21.9 % [36 % incl. faults / 22.7]).
  - WP-05/06/07/08/19 estimates stay.
- **WP-22 struct of arrays** (`results/RP-2/wp22-sizing.md`):
  - Memory: per entry 112 B inline + 3 allocations (level MI 4d B, counts 4d B, nodes). b1 d20: 25.8 + 18.4 + 18.4 + 63.8 MB (690 690 of the process's 692 268 allocations).
  - SoA (contiguous levels taken over from the generation, coefficient array, derived counts, flat nodes): **−50 MB** (d20; heap peak 178 MB), −36 MB (d15), −12 MB (sg b1 d20 L5), nil at sg d2 and mivec.
  - Runtime: tensor-grid-list walks are ≈ 18 ms of the serial preparation (level copy + `comp1DWeights` max-level scan: 16.5 % of its samples, 40 % of its cache misses) plus 3 allocations per grid in generation; the per-grid operator setup is ≤ 0.4 %. **≈ 0.03–0.07 s expected (minimal).**
  - Interface: listed in wp22-sizing.md: `getTensorGrid(idx/mi)`, `getTensorGrids()` span, iterators incl. a mutable one, `TensorGrid::getGPCntPerDim/getNodesPerDim` by reference, Python `mi` / `tensorGrid` properties and `TensorGridCTDataVector`.
  - The protected `perf_e2e.cpp:81` / `perf_reference.cpp:136-144` use `tgData.mi`, `.coefficient`, `.tensorGrid.*`, so a proxy must keep these names, or the owner adapts the files.
  - **Merge with WP-18: yes** (same storage and accessors). Combined: b1 d20 −96…−114 MB (RSS 217 → ≈ 110–125 MiB), d15 −68…−78 MB, sg d2 −101 MB.
- **Code placement**: op-cache misses are 2.5–4.3 per 1000 accesses (b1 high d 10–15).
  - Not front-end-bound: SG `processTensorGrid` (0.5 % stall cycles), the DSG b0 row loop (0.7 %).
  - Alignment candidates: `computeNeighborGPWithWeight` (interplin 2.9 / 5.7 % stalls, 4.6–6.3 % of its own cycles) → WP-08; the closure loop (4.1 %) → WP-03; `interpolateFirstDim` at d10 (2.8 %) → WP-06. Upper bound: the stall share.
- **b1 `generation` slower**: persists (d20 50.9 → 60.9 ms, d15 35.6 → 42.7 ms).
  - Cause: page faults. With glibc trim/mmap thresholds disabled, both builds take 30.5–31 ms. rp-1's warm-up freed 154 MB more operator memory, which the next generation reused.
  - Not a regression: about half of the warm generation is faulting in the grid (→ WP-22/WP-18).
- **Memory**:
  - Largest peaks above the floor: sg cc d2 1072, sg equi d2 620 MiB (scratch 537 + CC FFT 361 + nodes 101 MB, as in RP-1 → WP-05, WP-18); dsg b1 d20 217, d15 151 MiB (the DSG 126 / 83 MB + trie construction transient 52 / 28 MB → WP-22+18, trie reservation); dsg b0 d3 63 MiB (`DeltaQuadratureScratch` 15.7 MB zero-filled per thread → WP-05 pattern); sg b1 d20 57 MiB.
  - dsg b0 `MEM+`: per-thread builder arrays 13.5 MB of the 24 MB heap (b0 d10) → WP-04 `TrieNode` 24 → 16 B ≈ −3 MB.

## Profile shift / follow-ups
Proposed re-ranking (expected e2e saving at rp-2, suite 43.65 s; d ≥ 7 first; [RP-1 estimate]):
1. **WP-03** downwards closure: **≈ 4.5 s** [5.1], all d ≥ 10.
   - First step (scalar early-exit dominance test) ≈ 1.0 s [1.55]; it also makes the closure's speed independent of LTO decisions elsewhere (13 % swing observed).
   - Check the loop's front-end stalls (4.1 %). Memory nil.
2. **WP-04** DSG row loop (b0) + `TrieNode` 24 → 16 B: **1.5–2.5 s** [1.5–3], b0 d7/d10; memory ≈ −3 MB per b0 case.
3. **WP-17** batched source evaluation: ≈ 1–1.5 s, unchanged (dispatch ≈ 0.9 s DSG b0, 0.13 s b1, ≈ 0.4 s quad/sg); interface.
4. **WP-09**: unchanged (decision data; gates WP-06/WP-08).
5. **WP-06**: 0.6–0.7 s, unchanged (+ alignment check of `interpolateFirstDim`).
6. **WP-08**: ≈ 0.5 s, unchanged (+ alignment of the corner loop: front-end stalls up to 5.7 % of the d10 case).
7. **WP-21**: **0.1–0.2 s** (sized; [0.1–0.3]).
   - Levers: (a) slack `break` in `addChildren`, (b) per-set cache or a visit order grouping delta grids by single-node-dimension set.
   - Optional: the serial preparation (radix pass, max levels from the instruction, exact node reservation) ≤ 0.03–0.05 s and −8 MB at the b1 d20 peak.
8. **WP-22 merged with WP-18** (tensor-grid storage: SoA + shared 1D nodes; proposal, interface):
   - Runtime ≈ 0.03–0.07 s (minimal) + WP-18's node generation (≈ 35 % of CC SG generation).
   - **Memory** b1 d20 −96…−114 MB, d15 −68…−78 MB, sg d2 −101 MB, sg b1 d20 −25 MB.
   - Constraint: the protected benchmarks' member access.
9. **WP-05**: 0.5–0.8 s (d ≤ 3), memory −350…−500 MiB, unchanged.
   - The same on-demand pattern for `DeltaQuadratureScratch` (15.7 MB at dsg b0 d3).
   - By the d ≥ 7 rule it ranks behind WP-21/WP-22. The master may move it up for its memory weight: it is the largest memory lever and 10x WP-22's runtime saving.
10. **WP-07** ≈ 0.2 s, **WP-19** 0.1–0.3 s, **WP-10** (warm nil), **WP-12** (after WP-03): unchanged.

New findings (no new WP needed):
- Slack break (→ WP-21).
- Trie node reservation (→ WP-21).
- Generation page faults (→ WP-22/WP-18).
- Fragile closure code generation (→ WP-03).
- Alignment candidates (→ WP-03/06/08).

Methodology:
- RP-2 attributes inlined helpers by inline chain, not by source line. Sub-shares of the DSG row loop are therefore not 1:1 comparable with RP-1; per-symbol totals are.
- Allocation-heavy generation timings carry ±20 ms of page-fault noise that depends on what the previous phase freed.

## Revert
Nothing to revert in code. To discard: `git restore combigrid/perf/ANALYSIS.md` and delete `combigrid/perf/results/RP-2/` and this summary.
