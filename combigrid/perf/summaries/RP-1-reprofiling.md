# RP-1 — Re-profiling after WP-01, WP-13, WP-14 (runtime and peak memory)

Status: PROPOSED · Date: 2026-10-28 · Base commit: f48d2bbc1
Recommendation: accept (measurement only). Re-rank PLAN.md as proposed below. quad/dsg is now 51 % of the suite, WP-02/WP-03 stay on top, and a cheap scratch fix removes the WP-01 memory step.
Machine: `perf.py calibrate` ratios e2e 1.004 burst / 1.002 sustained, micro 1.005 / 1.003 (1.000 = as at baseline time)

## Change
- None (measurement). Snapshot `rp-1` = HEAD. Section "Re-profiling RP-1" appended to `ANALYSIS.md` (+ contents line).
- Files: `ANALYSIS.md`; `results/RP-1/`:
  - reports: `e2e.md/.json`, `micro.md/.json`, `micro-run.json`, `scaling-t1/t8.json`, `mivec-master-vs-rp-1.md/.json`
  - derived data: `groups.md`, `profile-splits.md`, `perfstat-*.txt`
  - top lists and summaries: `profiles/*.txt` (25 top lists), `heap/*.txt` (13 heaptrack summaries)

## Gain vs. cost
- Gain (cumulative baseline → rp-1, full suite, 16 threads):
  - **e2e 69.95 → 53.41 s (1.31x, 16.5 s saved)**; d ≥ 7: 59.78 → 46.84 s (1.28x). 29 cases faster, 4 within noise, 5 SLOWER (all dsg b1/cc-d10 and mivec d15/d20).
  - quad/sg 22.35 → 9.19 s (2.43x). quad/dsg 26.72 → 27.10 s (0.986x).
  - interp 6.61 → 5.70 s (1.16x). interplin 5.01 → 4.52 s (1.11x).
  - mivec 9.27 → 6.91 s (1.34x): operator 3.34x, generation 5.07 → 5.65 s (0.897x).
  - Micro (29 cases): 1.905 → 1.405 s (1.36x).
- Memory: geomean B/A 1.028.
  - 3 `MEM+`, all quad/sg at low d: equi-exp/b0/d2/L20 363.7 → 619.9 MiB (1.70x), cc-uq/b0/d2/L20 786.2 → 1079.1 MiB (1.37x), cc-uq/b0/d3/L14 37.8 → 40.5 MiB.
  - Group geomeans: quad/sg 1.080, quad/dsg 0.999, interp 1.006, interplin 0.993, mivec 1.011.
  - Floor 10.2 / 10.3 MiB.
- API / functionality: none. Maintainability / readability: n/a.

## Tests
- n/a (no code changed). `perf.py verify` OK at start and end. Heap tools ran under `systemd-run --user --scope -p MemoryMax=8G`.

## Reference check
- n/a (no code changed).

## Benchmarks (A = baseline fed2cdb4e, B = rp-1 f48d2bbc1; speedup = A/B)
L1 micro (1 thread, selection; all in `micro.md`):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 | 46.89ms | 13.88ms | 3.373 | [3.333, 3.413] |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 538.01ms | 109.22ms | 4.956 | [4.854, 5.061] |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 2.76ms | 2.80ms | 0.985 | [0.977, 0.993] |
| micro/mivec/downwards_closure/d10/S10/n300 | 593.46ms | 621.86ms | 0.955 | [0.951, 0.959] |

L3 e2e (16 threads, `total`, the longest rp-1 cases):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | 6.766s | 7.035s | 0.962 | [0.958, 0.965] |
| quad/dsg/cc-uq/b0/d10/L7 | 6.971s | 7.020s | 0.993 | [0.987, 0.999] |
| quad/dsg/equi-exp/b0/d10/L7 | 5.665s | 5.657s | 1.001 | [0.998, 1.005] |
| quad/dsg/equi-exp/b1/d15/L7 | 3.966s | 4.091s | 0.968 | [0.964, 0.971] |
| interplin/sg/equi/b1/d10/L5/p32 | 2.701s | 2.468s | 1.073 | [1.013, 1.137] |
| quad/sg/equi-sin/b0/d7/L7 | 3.524s | 2.410s | 1.460 | [1.448, 1.472] |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.894s | 2.102s | 0.902 | [0.894, 0.910] |

Sum of medians (38 cases): A = 69.953 s, B = 53.414 s, saved = 16.539 s · Full reports: `combigrid/perf/results/RP-1/`

Peak memory (flagged and largest):

| case | A | B | B/A | flag |
|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 786.2 MiB | 1079.1 MiB | 1.373 | MEM+ |
| quad/sg/equi-exp/b0/d2/L20 | 363.7 MiB | 619.9 MiB | 1.704 | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 331.3 MiB | 328.8 MiB | 0.992 | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 232.4 MiB | 233.4 MiB | 1.004 | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 37.8 MiB | 40.5 MiB | 1.072 | MEM+ |

Geomean B/A = 1.028 · process floor A = 10.2 MiB, B = 10.3 MiB

## Attempts not kept
- None. Method note: since WP-13 the helpers are inlined. Their shares come from `perf report --sort sym,srcfile|srcline` splits. `perf script` callchains through libm are unreliable.

## Answers to the RP-1 questions (16-thread profiles; setup values in brackets)
- **WP-02**: `appendSubTrieChildren` **64.8 %** (b1 d20 L6) / **44.4 %** (b1 d15 L7) [63 %].
  - Also `compSubTrie` 0.9 / 1.3 % and `updateCoeffSums` 13.3 / 19.9 %, which is per-trie work too, so per-delta-grid trie work totals 79 / 66 %.
  - Serial preparation 0.29 s.
- **WP-04**: b0 d10 L7 row loop vs. integrand: equi-exp **44.8 % vs. 50.5 %** [41 / 47 %]; cc-uq **35.0 % vs. 61.1 %**.
  - Inside the loop: `updateCoeffSums` 15.0 / 11.8, `compRowWeights` 9.1 / 6.4, row + advance 9.2 / 7.4, `std::function` 4.6 / 3.9 %.
- **WP-05**: cc d2 L20: CC 1D weights **36 %** (1 thread 44 %), scratch zero-fill 10.7 %, integrand 31 %; scales only 4.2x.
  - b1 d15/d20: per-tensor-grid costs **< 1 %** (weights ≈ 0.2 %); integrand 65–73 %, `processTensorGrid` 12–15 %, libgomp + OpenBLAS 9–17 %.
- **WP-03**: `genMIVecDownwardsClosure` **78.6 / 96.6 / 97.6 %** at d10/d15/d20 [50 / – / 96 %].
  - rp-1 generation is 1.33–1.46x slower than the old master. Responsible: `tools::mi_view::lessEqualInEveryDim` (via `miVecDominatesMI` → `operator>=`), which GCC 15 compiles to an AVX2 early-break loop with an alignment-dispatch prologue (IPC 1.76 vs. 2.11–2.48).
  - Present since the MIView refactor (baseline); amplified by WP-14.
- **WP-06/WP-07**: `interp/sg/cc/b0/d5/L6/p64`: `denormalizeDataVector` **6.8 %**, `getGridPoint` **7.8 %** (+2.8 % vector access), `hasTrivialResult` + `nearlyEqual` **2.4 %**, libgomp 4.8 % [12 / 11 / 3 / –].
  - d10 L2 p8: 8.4 / 5.0 / 1.3 / 3.9 %.
  - cheb1 d5 L4 p64: `barycentricFormulaWeights` + `log` **22.9 %** (+ `exp` 2.9 %, their allocations 5.5 %) [18 %], libgomp **21.3 %** [≥ 15 %].
- **WP-08**: `interplin/.../d8/L5/p256`: `computeNeighborGPWithWeight` **16.6 %**, `denormalizeDataVector` **5.0 %** [17 / 11 %]; d10 L5 p32: 19.8 / 4.3 %.
- **Memory, WP-01 step**: WP-01 added `QuadratureScratch::nodesPerDim` (quadrature_scratch.hpp:47). Every thread allocates and zero-fills it in each `quadrature()` call at the size of the largest tensor grid: 16 × 16.8 MiB = **268 MiB**.
  - heaptrack before/after: exactly +268 MiB in the DataVector-allocation group. The measured RSS step is +293 MiB (cc) / +256 MiB (equi); the equi case follows the same pattern.
  - At 1 thread the peaks are 137 / 298 MiB.
- **Memory, largest peaks**:
  - cc d2 L20 1079 MiB: scratch 2 × 268, CC FFT temporaries ≈ 360, node lists 101.
  - equi d2 L20 620 MiB: scratch 537 (84 %), node lists 101.
  - dsg b1 d20 329 MiB: per-thread sub-tries 154 (51 % of the heap), node lists 64, tensor grid data 26, trie 19, MIs 18.
  - dsg b1 d15 233 MiB: sub-tries 102, node lists 43.
  - The other 34 cases are ≤ 63 MiB, mostly floor (10.3 MiB, of which ≤ 4.5 MiB from the Armadillo/LAPACK/OpenBLAS stack) + thread stacks + node lists.
- **Memory, material reductions** (details in ANALYSIS.md, "Peak memory"):
  - Scratch without zero-fill / grown on demand: **−350…−500 MiB** (equi d2 −56…−80 %, cc d2 −32…−46 %). Runtime −0.1…−0.2 s, ~20 lines, no API change.
  - CC weights per (rule, n): −300 MiB (WP-05).
  - DSG sub-tries: −150 MiB (WP-02).
  - Shared node arrays: −64…−101 MiB (interface change).
  - WP-10: −1…−4.5 MiB per process.
- **New hot spots without a WP**:
  - Scratch zero-fill (memory + 11–18 % of samples in the d2 cases).
  - Vectorized MI dominance test.
  - `std::function` dispatch per grid point (4–9 % of the quadrature kernels).
  - Low-d load imbalance of quad/sg (scaling 4.2x / 6.2x).
  - 230 k per-tensor-grid node allocations (64 MiB).

## Profile shift / follow-ups
Proposed re-ranking (expected e2e saving at rp-1; d ≥ 7 first; memory effect):
1. **WP-02** DSG per-delta-grid trie work. Scope: include `updateCoeffSums` and the serial preparation.
   - Expected: **4.1–5.5 s**, all at d ≥ 15 (trie work 5.6 s at d20 and 2.7 s at d15; 2–3x on it) [was 4–6.5 s].
   - Memory: up to −150 MiB if sub-tries are not materialized per thread; a sub-trie cache could raise the peak (report it).
2. **WP-03** downwards closure. Expected **≈ 5.1 s** (generation 5.65 s, ≥ 10x; all d ≥ 10). Memory nil.
   - First step / fallback: a scalar early-exit dominance test (or `#pragma GCC novector`) gives master speed, **−1.55 s** (d10 0.48, d15 0.52, d20 0.55 s), a few lines.
   - Ties with WP-02 on the gain; by the tie-break rule DSG (quadrature) ranks first. WP-03 is the more certain gain, so the master may swap them.
3. **WP-04** DSG row loop (b0). Expected **1.5–3 s** at d ≥ 7, after WP-02. Memory nil.
   - The loop is 35–45 % (≈ 6.1 s) of the four b0 d7/d10 cases (d7 shares assumed as at d10, not profiled). Target: SG-kernel parity (215 → ≈ 152 instructions/point, 27.8 vs. 21.8 ns/point).
4. **NEW-A** quadrature scratch without zero-fill / grown on demand (SG + DSG) — memory WP.
   - Location: `quadrature_scratch.hpp` (ctor), `quadrature.cpp` (`getWeights`, `mapNodesOntoDomain`), `delta_quadrature_scratch.hpp`.
   - Hypothesis: per-thread buffers sized by the largest grid and zero-filled in every call; only the threads processing the largest grids need that size.
   - Evidence: heaptrack 2 × 268 MiB; 4.0e5 / 1.1e6 page faults; 17.8 / 10.7 % of the samples; 1-thread peaks 137 / 298 MiB.
   - Gain: **−350…−500 MiB** on the two largest peaks, runtime −0.1…−0.2 s (d2 only), low risk. Schedule early (or inside WP-05).
5. **WP-05** reduced to CC 1D weights at low d: cache per (rule, #nodes) per call, fewer `genWeightsFFT` temporaries. The many-small-grid part is obsolete (< 1 %).
   - Expected: 0.4–0.6 s, cc d2/L20 only [was 0.5–1.5 s]; memory up to −300 MiB on cc d2/L20.
6. **WP-06** interp overhead: 0.6–0.7 s (d ≥ 7 ≈ 0.4 s) [~1.3 s]. Removable parts: denormalize + `getGridPoint` + vector access 16–17 %.
7. **WP-08** interplin corner loop: ≈ 0.5 s (d ≥ 7 ≈ 0.45 s) [~1.1 s].
8. **WP-07** generic barycentric weights: ≈ 0.2 s, d < 7 (weights + allocations 31 % of cheb1 d5).
9. **WP-09**: value unchanged (integrand 67–71 % per query point; new-API callers only).
10. **WP-10**: warm e2e nil; floor −1…−4.5 MiB per process. **WP-12**: unchanged.

New proposals:
- **NEW-C** quad/sg load balancing at low d (split huge tensor grids into row ranges): d2 scaling 4.2x / 6.2x → 0.1–0.3 s. Low priority, after WP-05/NEW-A.
- **D-4 (needs decision)** batched `SourceFunc` evaluation (a row of points per call) instead of a per-point `std::function` dispatch.
  - Effect: 4–9 % of the quad/sg and DSG kernels, ≈ 1–1.5 s; enables vectorized integrands.
  - Cost: public API + Python bindings.
- **NEW-D (needs decision, memory)** shared 1D node arrays per (dim, level) instead of per-tensor-grid copies.
  - Effect: −64 MiB (dsg b1 d20, 19 %), −101 MiB (sg d2/L20, 16 %), −17 MiB (sg b1 d20, 29 %); 230 k fewer allocations in generation.
  - Cost: changes the public `TensorGrid` storage.
- Methodology: `ab micro` runs 0 warm-ups, so allocation-heavy micro cases show first-call times (`gen/complete_mis_with_coeffs` 44.9 vs. 9.4 ms). Ratios stay valid; absolute numbers should come from `run micro`.

## Revert
Nothing to revert in code. To discard: `git restore combigrid/perf/ANALYSIS.md` and delete `combigrid/perf/results/RP-1/` and this summary.
