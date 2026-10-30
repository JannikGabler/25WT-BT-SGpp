# Analysis — combigrid performance (initial profile = RP-0)

Setup session, 2026-10-22/24. Code: commit 84299eeca (unchanged). Judged configuration
(`perf.py build normal`), Ryzen 7 5825U, **CPU boost off** (all cores at 2.0 GHz; the owner
disabled boost and the baseline was re-recorded on 2026-10-24), platform profile `performance`,
AC, idle; e2e at 16 threads, micro at 1 thread. Raw data: `baseline/` (protected). Later re-profilings append sections at the end.

## Contents
1. [Measurement noise](#measurement-noise)
2. [Where the time goes (baseline e2e)](#where-the-time-goes)
3. [Hot spots per operator (profiles)](#hot-spots-per-operator)
4. [Micro-benchmarks and speed of light](#micro-benchmarks-and-speed-of-light)
5. [Thread scaling](#thread-scaling)
6. [Hardware counters](#hardware-counters)
7. [Further findings](#further-findings)
8. [From findings to work packages](#from-findings-to-work-packages)
9. [Re-profiling RP-1](#re-profiling-rp-1) (after WP-01, WP-13, WP-14; incl. peak memory)
10. [Re-profiling RP-2](#re-profiling-rp-2) (after WP-20, WP-02; WP-21/WP-22 sizing, code placement, peak memory)
11. [Re-profiling RP-3](#re-profiling-rp-3) (after WP-03; mivec split, WP-29 kernel shares, OpenBLAS cold first call, peak memory)

## Measurement noise

Self-comparisons of the `baseline` snapshot with itself (`perf.py ab ... baseline baseline`):

- With 3 rounds × 2 samples and a bootstrap over samples, 18 of 114 rows were flagged as
  significant although both sides were identical: the dominant noise is **between processes**
  (memory layout, thread placement; some cases are bimodal by up to 9 %), not between samples of
  one process. Hence the A/B design: many rounds (separate processes, A and B interleaved), one
  sample each, t-interval over the per-round log ratios, spread bounded below by a per-case noise
  model.
- The noise model (`baseline/noise_e2e.json`, `noise_micro.json`) is the robust per-round std
  (1.4826 · MAD, insensitive to a single outlier round) of the log speedup over 6 rounds of the
  self-comparison:

  - e2e: per-round log-speedup std of `total` over 38 cases: median 0.50%, 90th pct 2.00%, max 8.83% (quad/sg/equi-exp/b0/d2/L20); top 3: interplin/sg/equi/b1/d2/L16/p16384 3.0%, mivec/sg/cc-exp/b1/d5/S16/n1000 4.3%, quad/sg/equi-exp/b0/d2/L20 8.8%
    -> 95% CI half-width with 5 rounds at the median spread: ±0.62%, at the 90th pct: ±2.48%
  - micro: per-round log-speedup std of `call` over 29 cases: median 0.39%, 90th pct 0.91%, max 1.71% (micro/interp/global_tg/cheb1/b0/d5/L4); top 3: micro/quad/process_tg/equi/b0/d10/L4 1.0%, micro/interp/global_tg/cc/b0/d5/L6 1.1%, micro/interp/global_tg/cheb1/b0/d5/L4 1.7%
    -> 95% CI half-width with 5 rounds at the median spread: ±0.49%, at the 90th pct: ±1.13%

- **Throttling with boost on (why boost is off)**: with boost on, the 26-minute self-comparisons
  (2026-10-24, noon) ran 1.6x slower at 16 threads than the same binaries two days earlier (past
  midnight), while short and 1-thread runs were barely affected and foreign CPU load was < 0.5 %:
  the laptop throttled under sustained full load, depending on the conditions. The owner then
  disabled boost and improved the airflow; all numbers in this file are from the re-recording
  with boost off, where burst and sustained throughput are equal (calibration: 703 vs. 705 ms)
  and the six rounds of the e2e self-comparison took 288, 288, 288, 288, 288, 287 s. Boost off
  costs 1.62x at 16 threads (sum over the suite 43.2 s → 70.0 s) and ~2.1x at 1 thread, and makes
  the noise smaller (see above; with boost on: e2e median 0.67 %, micro 1.05 %). `perf.py`
  reports foreign CPU load and round durations in every A/B report, and `perf.py calibrate`
  compares burst and sustained throughput with `baseline/calibration.json`.
- Reference check noise: reassociation across 1–16 threads moves results by at most 1.54
  round-off units (README, "Reference check"); the tolerance allows 4 units plus 10 % error growth.

## Where the time goes

Baseline e2e (16 threads; medians of 5 samples; `ns/point` = operator time per evaluated grid
point, `thread·ns/point` = the same times 16; for `interp` the grid points are counted once per
query point):

| case | total | generation | operator | nTG | grid points | ns/point (16 thr) | thread·ns/point | rel. error |
|---|---|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 732.3 ms | 29.7 ms | 693.4 ms | 41 | 1.43e+08 | 4.86 | 78 | 3.0e-12 |
| quad/sg/equi-exp/b0/d5/L10 | 710.1 ms | 1.9 ms | 707.7 ms | 2751 | 1.4e+08 | 5.05 | 81 | 4.7e-13 |
| quad/sg/equi-exp/b0/d7/L7 | 1.61 s | 1.7 ms | 1.61 s | 3431 | 2.64e+08 | 6.09 | 97 | 5.9e-11 |
| quad/sg/equi-exp/b0/d10/L4 | 3.32 s | 2.4 ms | 3.31 s | 1001 | 4.26e+08 | 7.77 | 124 | 3.6e-09 |
| quad/sg/equi-exp/b1/d10/L7 | 1.12 s | 2.3 ms | 1.11 s | 19448 | 1.21e+08 | 9.22 | 147 | 2.9e-10 |
| quad/sg/equi-exp/b1/d15/L6 | 2.39 s | 7.3 ms | 2.38 s | 54264 | 2.13e+08 | 11.18 | 179 | 1.7e-09 |
| quad/sg/equi-exp/b1/d20/L5 | 1.12 s | 8.5 ms | 1.10 s | 53130 | 7.72e+07 | 14.31 | 229 | 3.8e-08 |
| quad/sg/equi-sin/b0/d7/L7 | 3.52 s | 1.8 ms | 3.52 s | 3431 | 2.64e+08 | 13.32 | 213 | 8.2e-12 |
| quad/sg/cc-uq/b0/d2/L20 | 1.36 s | 77.6 ms | 1.28 s | 41 | 1.43e+08 | 9.00 | 144 | 9.9e-12 |
| quad/sg/cc-uq/b0/d3/L14 | 179.5 ms | 2.1 ms | 177.3 ms | 316 | 3.55e+07 | 4.99 | 80 | 1.2e-12 |
| quad/sg/cc-uq/b0/d5/L10 | 780.3 ms | 2.4 ms | 777.4 ms | 2751 | 1.4e+08 | 5.54 | 89 | 8.1e-12 |
| quad/sg/cc-uq/b0/d7/L7 | 1.81 s | 1.9 ms | 1.81 s | 3431 | 2.64e+08 | 6.83 | 109 | 2.6e-12 |
| quad/sg/cc-uq/b0/d10/L4 | 3.62 s | 2.4 ms | 3.61 s | 1001 | 4.26e+08 | 8.48 | 136 | 1.2e-10 |
| quad/dsg/equi-exp/b0/d3/L16 | 93.7 ms | 748 µs | 92.5 ms | 969 | 3.21e+07 | 2.88 | 46 | 1.1e-12 |
| quad/dsg/equi-exp/b0/d5/L12 | 221.2 ms | 1.0 ms | 219.0 ms | 6188 | 7.7e+07 | 2.84 | 45 | 3.6e-12 |
| quad/dsg/equi-exp/b0/d7/L10 | 1.23 s | 2.3 ms | 1.22 s | 19448 | 4.19e+08 | 2.92 | 47 | 2.1e-11 |
| quad/dsg/equi-exp/b0/d10/L7 | 5.66 s | 2.6 ms | 5.66 s | 19448 | 1.83e+09 | 3.09 | 49 | 1.1e-10 |
| quad/dsg/equi-exp/b1/d15/L7 | 3.98 s | 52.0 ms | 3.89 s | 170544 | 3.54e+08 | 11.00 | 176 | 8.6e-10 |
| quad/dsg/equi-exp/b1/d20/L6 | 6.78 s | 78.3 ms | 6.66 s | 230230 | 2.81e+08 | 23.74 | 380 | 6.8e-09 |
| quad/dsg/cc-uq/b0/d5/L12 | 267.7 ms | 2.4 ms | 264.1 ms | 6188 | 7.7e+07 | 3.43 | 55 | 1.3e-13 |
| quad/dsg/cc-uq/b0/d7/L10 | 1.52 s | 3.3 ms | 1.52 s | 19448 | 4.19e+08 | 3.61 | 58 | 2.2e-12 |
| quad/dsg/cc-uq/b0/d10/L7 | 7.04 s | 3.0 ms | 7.04 s | 19448 | 1.83e+09 | 3.84 | 61 | 2.8e-11 |
| interp/sg/cc/b0/d2/L12/p64 | 235.1 ms | 225 µs | 234.8 ms | 25 | 3.6e+05 | 10.18 | 163 | 2.0e-15 |
| interp/sg/cc/b0/d3/L9/p64 | 309.5 ms | 78 µs | 309.3 ms | 136 | 5.57e+05 | 8.68 | 139 | 0.0e+00 |
| interp/sg/cc/b0/d5/L6/p64 | 1.74 s | 109 µs | 1.74 s | 456 | 2.2e+06 | 12.37 | 198 | 7.1e-08 |
| interp/sg/cc/b0/d7/L4/p16 | 1.48 s | 84 µs | 1.48 s | 330 | 5.06e+06 | 18.28 | 292 | 1.3e-04 |
| interp/sg/cc/b0/d10/L2/p8 | 2.23 s | 44 µs | 2.23 s | 66 | 1.02e+07 | 27.39 | 438 | 3.5e-02 |
| interp/sg/cheb1/b0/d3/L6/p64 | 188.2 ms | 29 µs | 188.2 ms | 64 | 3.89e+04 | 75.64 | 1210 | 1.9e-13 |
| interp/sg/cheb1/b0/d5/L4/p64 | 368.8 ms | 38 µs | 368.7 ms | 126 | 2.06e+05 | 27.92 | 447 | 2.3e-04 |
| interp/sg/equi/b0/d3/L5/p64 | 53.5 ms | 20 µs | 53.4 ms | 46 | 1.52e+04 | 54.96 | 879 | 3.5e-02 |
| interplin/sg/equi/b1/d2/L16/p16384 | 117.2 ms | 357 µs | 112.1 ms | 33 | 6.55e+06 | nan | nan | 1.9e-10 |
| interplin/sg/equi/b1/d5/L8/p1024 | 515.7 ms | 2.2 ms | 512.7 ms | 1231 | 3.72e+06 | nan | nan | 1.7e-04 |
| interplin/sg/equi/b1/d8/L5/p256 | 1.69 s | 2.3 ms | 1.69 s | 1287 | 7.47e+05 | nan | nan | 3.2e-02 |
| interplin/sg/equi/b1/d10/L5/p32 | 2.70 s | 1.7 ms | 2.69 s | 3003 | 2.3e+06 | nan | nan | 4.6e-02 |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 2.26 s | 4.6 ms | 2.25 s | 777 | 4.13e+08 | 5.45 | 87 | 9.1e-13 |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 3.26 s | 1.37 s | 1.88 s | 2724 | 1.68e+08 | 11.21 | 179 | 1.2e-08 |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.89 s | 1.79 s | 100.3 ms | 516 | 6.08e+06 | 16.52 | 264 | 1.9e-03 |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 1.91 s | 1.90 s | 9.8 ms | 353 | 4.51e+05 | 21.72 | 348 | 3.7e-02 |

| group | sum total | sum operator | sum total (d ≥ 7) |
|---|---|---|---|
| quad/sg | 22.28 s | 22.10 s | 18.51 s |
| quad/dsg | 26.80 s | 26.56 s | 26.22 s |
| interp/sg | 6.61 s | 6.61 s | 3.71 s |
| interplin/sg | 5.02 s | 5.01 s | 4.39 s |
| mivec/sg | 9.31 s | 4.24 s | 7.06 s |

Observations:
- **Grid generation is negligible** (≤ 1–2 % of `total`) for complete sparse grids, including
  d = 20 with 230 230 delta tensor grids. Exception: `MIVecSGGenInstr` at d ≥ 10, where the
  downwards closure dominates (1.4–1.9 s of 1.9–3.3 s).
- **The operators dominate.** With the cheap `exp` integrand, complete-SG quadrature costs
  78–124 thread·ns per grid point for b0 (d = 2…10) and 147–229 for the many small grids of b1
  (d = 10…20), versus 45–49 for large delta grids of the deltas quadrature (same integrand): the
  per-point and per-grid overhead of the complete-SG operator is the largest single potential.
- DSG quadrature evaluates every grid point once and costs ~45–60 thread·ns per point for large
  delta grids (the pre-existing benchmarks d5 L10 show an order of magnitude over SG quadrature),
  but its b1 cases at d = 15/20 cost 176–380 thread·ns per point: most time is spent outside the
  per-point loop (sub-tries, see profiles).
- Interpolation (global and multilinear) re-evaluates the integrand at every grid point of every
  tensor grid **for every query point**; the integrand (`cos`) is about half of the time.

## Hot spots per operator

`perf.py profile` (self time, 16 threads; full text in `.work/` of the setup session; numbers
rounded):

| case | top self-time symbols (share of all samples) |
|---|---|
| `quad/sg/equi-exp/b0/d10/L4` | `denormalizeDataVector` 37 % · `processTensorGrid` 23 % · `getWeightForGP` 13 % · integrand + `exp` 16 % · `evaluateNormalizedInPlace` 3 % |
| `quad/sg/equi-exp/b1/d20/L5` (53 130 small grids) | `denormalizeDataVector` 39 % · `processTensorGrid` 28 % · `getWeightForGP` 12 % · integrand + `exp` 10 % |
| `quad/sg/equi-sin/b0/d7/L7` | `sin` + integrand 62 % · `denormalizeDataVector` 15 % · `processTensorGrid` 11 % · `getWeightForGP` 6 % |
| `quad/sg/cc-uq/b0/d2/L20` | Eigen FFT (`kiss_cpx_fft`) 9 % + `genWeightsFFT` 2 % (with boost on: 14 % + 4 %) · `processTensorGrid` 13 % · `denormalizeDataVector` 11 % · integrand + `exp` 23 % · `getWeightForGP` 5 % |
| `quad/dsg/equi-exp/b0/d10/L7` | `processDeltaTensorGrid` (inlined row loop) 41 % · integrand + `exp` 47 % · `std::function`/`SourceFunc::evaluate` 4 % · `memset` 2 % |
| `quad/dsg/equi-exp/b1/d20/L6` (230 230 delta grids) | `appendSubTrieChildren` (sub-trie per delta grid) **63 %** · `processDeltaTensorGrid` 23 % · integrand 7 % |
| `interp/sg/cc/b0/d5/L6/p64` | `cos` + integrand 52 % · `denormalizeDataVector` 12 % · `TensorGrid::getGridPoint` 11 % · `hasTrivialResult` 3 % · `interpolateFirstDim` 2 % |
| `interp/sg/cheb1/b0/d5/L4/p64` | barycentric weights (`barycentricFormulaWeights` + `log`) 18 % · `cos` + integrand 21 % · libgomp spin ≥ 15 % · `denormalizeDataVector` 5 % · `getGridPoint` 4 % |
| `interplin/sg/equi/b1/d8/L5/p256` | `cos` + integrand 57 % · `computeNeighborGPWithWeight` 17 % · `denormalizeDataVector` 11 % · libgomp 4 % |
| `mivec/sg/cc-exp/b1/d10/S12/n1000` | `genMIVecDownwardsClosure` 49 % · `denormalizeDataVector` 19 % · `processTensorGrid` 10 % · `getWeightForGP` 7 % |
| `mivec/sg/cc-exp/b1/d20/S7/n300` | `genMIVecDownwardsClosure` **96 %** |

OpenBLAS (1–8 %) appears in these profiles because profiled processes are short (1 warm-up +
2 samples); see "Further findings".

## Micro-benchmarks and speed of light

Baseline micro suite (1 thread; `micro/sol/...` = hand-written lower bound for the same task with
the same integrand call):

| case | time/call | time/item | items |
|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | 13.2 ms | 33.57 ns | 3.932e+05 |
| micro/quad/process_tg/equi/b0/d5/L10 | 7.8 ms | 46.73 ns | 1.66e+05 |
| micro/quad/process_tg/equi/b0/d10/L4 | 47.1 ms | 72.47 ns | 6.495e+05 |
| micro/quad/process_tg/cc/b0/d2/L16 | 31.9 ms | 81.24 ns | 3.932e+05 |
| micro/quad/process_tg/cc/b0/d10/L4 | 47.8 ms | 73.51 ns | 6.495e+05 |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 538.5 ms | 134.26 ns | 4.011e+06 |
| micro/sol/quad_tg/equi/b0/d5/L10 | 3.5 ms | 21.12 ns | 1.66e+05 |
| micro/sol/quad_tg/equi/b0/d10/L4 | 16.3 ms | 25.05 ns | 6.495e+05 |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | 8.0 ms | 12.25 ns | 6.495e+05 |
| micro/sourcefunc/eval_normalized_in_place/d10 | 57 µs | 55.80 ns | 1024 |
| micro/quadrule/cc_weights/n33 | 2 µs | 68.25 ns | 33 |
| micro/quadrule/cc_weights/n1025 | 53 µs | 51.44 ns | 1025 |
| micro/quadrule/cc_weights/n32769 | 2.3 ms | 69.21 ns | 3.277e+04 |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | 7.9 ms | 23.77 ns | 3.318e+05 |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 69.9 ms | 27.74 ns | 2.519e+06 |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 2.7 ms | 664.54 ns | 4096 |
| micro/dsg/prepare/equi/b1/d20/L6 | 287.7 ms | 1249.54 ns | 2.302e+05 |
| micro/interp/global_tg/cc/b0/d5/L6 | 1.3 ms | 127.39 ns | 1.045e+04 |
| micro/interp/global_tg/cheb1/b0/d5/L4 | 555 µs | 207.52 ns | 2673 |
| micro/interp/barycentric_1d/generic/n9 | 1 µs | 93.53 ns | 9 |
| micro/interp/barycentric_1d/generic/n65 | 38 µs | 577.23 ns | 65 |
| micro/interp/barycentric_1d/opt_cc/n9 | 0 µs | 6.52 ns | 9 |
| micro/interp/barycentric_1d/opt_cc/n65 | 0 µs | 5.79 ns | 65 |
| micro/interplin/tg/equi/b1/d8/L5 | 47 µs | 185.48 ns | 256 |
| micro/gen/complete_mis_with_coeffs/d20/L6 | 16.3 ms | 70.90 ns | 2.302e+05 |
| micro/gen/sparse_grid/cc/b1/d20/L5 | 83.0 ms | 1562.46 ns | 5.313e+04 |
| micro/mivec/downwards_closure/d10/S10/n300 | 592.9 ms | 168346.75 ns | 3522 |
| micro/mivec/downwards_closure/d20/S6/n100 | 19.4 ms | 66535.74 ns | 291 |
| micro/mivec/ct_coeffs/d10/S12/n1000 | 15.5 ms | 1077.10 ns | 1.438e+04 |

Key ratios:
- Complete-SG tensor-grid quadrature vs. speed of light: d = 10: 72.5 vs. 25.1 ns/point (2.9x),
  d = 5: 46.7 vs. 21.1 ns/point (2.2x).
- Many small tensor grids (b1, d = 20, 10 626 grids): 134 ns/point vs. 72.5 ns/point in a large
  grid — per-grid costs (weights, iterator allocations) matter in this regime.
- DSG: 27.7 ns/point for a large delta grid (d = 10) vs. 665 ns/point for a typical tiny delta
  grid at b1 d = 20 (sub-trie construction per delta grid); the serial preparation (1D weights +
  trie) takes 0.29 s at b1 d = 20 L6.
- Generic barycentric formula: 577 ns per node and call (O(n²) weights recomputed on every call)
  vs. 5.8 ns for the closed-form CC formula (~100x).
- Downwards closure: 593 ms single-threaded for a closure of 3 522 multi-indices (d = 10, 300
  input MIs), i.e. 168 µs per closure element; in the e2e suite 1.4–1.9 s of generation at
  16 threads for d = 10–20.

## Thread scaling

`operator` time, baseline snapshot, 1–16 threads:

| case | 1 thread | 4 | 8 | 16 | speedup 16 vs 1 | speedup 8 vs 1 |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 4.56 s | 1.26 s | 696.0 ms | 695.2 ms | 6.6x | 6.6x |
| quad/sg/equi-exp/b0/d5/L10 | 6.26 s | 1.57 s | 797.4 ms | 703.6 ms | 8.9x | 7.9x |
| quad/sg/equi-exp/b1/d15/L6 | 22.04 s | 5.64 s | 2.82 s | 2.34 s | 9.4x | 7.8x |
| quad/dsg/equi-exp/b0/d7/L10 | 11.26 s | 2.85 s | 1.42 s | 1.22 s | 9.2x | 7.9x |
| quad/dsg/equi-exp/b1/d15/L7 | 34.81 s | 8.82 s | 4.51 s | 3.88 s | 9.0x | 7.7x |
| interp/sg/cc/b0/d5/L6/p64 | 17.55 s | 4.44 s | 2.25 s | 1.74 s | 10.1x | 7.8x |
| interplin/sg/equi/b1/d5/L8/p1024 | 4.93 s | 1.26 s | 642.9 ms | 604.9 ms | 8.1x | 7.7x |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 11.82 s | 3.44 s | 1.74 s | 1.94 s | 6.1x | 6.8x |

- **8 cores scale almost linearly** (7.7–7.9x with 8 threads; all cores run at the same 2.0 GHz
  with boost off). **SMT** (16 vs. 8 threads) adds 0–30 %: most for the integrand-heavy
  interpolation (1.29x) and the many-small-grid cases (1.2x), nothing for `quad/sg/.../d2/L20`,
  and `mivec/.../d10` gets slower. (With boost on, 8 threads reached only 6–7x and SMT ≤ 10 %,
  because the all-core frequency drops below the single-core boost.)
- **Load imbalance at low d**: `quad/sg/equi-exp/b0/d2/L20` (41 tensor grids for 16 threads,
  sizes differ by orders of magnitude, no parallelism inside a tensor grid) scales 6.6x only.
- **Serial generation work**: `mivec/.../d10` scales 6.1x (closure generation).

## Hardware counters

`perf stat` (16 threads, whole process incl. warm-up):

| case | instructions | cycles | IPC (per hw thread) | instructions per grid point | cache misses |
|---|---|---|---|---|---|
| `quad/sg/equi-exp/b0/d10/L4` | 7.1e11 | 3.2e11 | 2.23 | ≈ 555 | 3.7e6 |
| `quad/dsg/equi-exp/b0/d10/L7` | 1.20e12 | 5.4e11 | 2.20 | ≈ 218 | 2.3e7 |
| `quad/dsg/equi-exp/b1/d20/L6` | 1.34e12 | 6.3e11 | 2.12 | ≈ 1590 | 1.25e9 |

(3 pipeline runs each: 1 warm-up + 2 samples; boost off, 2.0 GHz. Instruction and cycle counts
were the same with boost on.)
Complete-SG quadrature is **instruction-bound** (high IPC, almost no cache misses): ~555
instructions per grid point at d = 10 for a d-dimensional affine map, a d-fold weight product,
the iterator update and one `exp` — the speed-of-light loop needs a fraction of that. The deltas
operator needs 218 per point for large delta grids, but ~1600 per point plus 1.25e9 cache misses
in the b1 d = 20 case (sub-trie construction: memory-bound pointer work).

## Further findings

- **PLT calls of exported helpers**: SG++ builds with `-fPIC` without
  `-fno-semantic-interposition`, so exported functions (e.g. `denormalizeDataVector`,
  `getWeightForGP`, `TensorGrid::getGridPoint`) are never inlined, not even within their own
  translation unit; they show up as separate hot symbols. Structural fixes (WP-01/06/08) remove
  the hottest calls; the flag itself is decision D-2.
- **OpenBLAS thread pool**: `USE_ARMADILLO=1` links Armadillo (and OpenBLAS) into
  `libsgppbase.so`; OpenBLAS starts and spins its threads at process start (5–18 % of the samples
  of short cold runs). Warm runs: no measurable effect (four cases with and without
  `OPENBLAS_NUM_THREADS=1`, and per-sample timings of fresh processes: differences < 2 %).
  Decision D-3.
- **Global interpolation regenerates nodes**: `interpolateFirstDim` / `interpolateLaterDim` call
  `genNodes` (allocation, `cos` for Chebyshev nodes) per tensor grid, dimension and query point,
  although the tensor grid stores its nodes.
- **CC weights**: `ClenshawCurtisQuadRule::genWeightsInplace` allocates ~6 vectors and runs an FFT
  for every tensor grid and dimension in every `quadrature()` call; Eigen's FFT is 13 % of the
  d = 2 CC case.
- **Cold vs. warm**: the first pipeline in a process is ~2x slower (thread creation, page faults,
  OpenBLAS); all suite numbers are warm (one warm-up per process).

## From findings to work packages

| Finding | WP |
|---|---|
| per-point overhead of complete-SG quadrature (denormalize, weight product, iterator) | WP-01 |
| per-delta-grid sub-trie construction at high d, serial preparation | WP-02 |
| bounding-box downwards closure | WP-03 |
| DSG row loop | WP-04 |
| per-tensor-grid weight generation and allocations (CC FFT) | WP-05 |
| global interpolation overhead (nodes, `getGridPoint`, scheduling) | WP-06 |
| O(n²) barycentric weights per call | WP-07 |
| multilinear corner loop | WP-08 |
| re-evaluation of the integrand per query point | D-1 (interface) |
| PLT calls of exported helpers | D-2 (flag) |
| OpenBLAS thread pool | D-3 (build) |

## Re-profiling RP-1

Work session RP-1, 2026-10-28. Code: f48d2bbc1 (= `rp-1` snapshot: WP-01, WP-13, WP-14, WP-15/16
infrastructure). Judged configuration, boost off, `calibrate` OK (e2e 1.004 burst / 1.002
sustained, micro 1.005 / 1.003). Reports: `results/RP-1/` (`e2e.md` full suite baseline → rp-1,
`micro.md`, `groups.md`, `profile-splits.md`, `profiles/`, `heap/`, `perfstat-*.txt`,
`scaling-t*.json`, `mivec-master-vs-rp-1.md`). Since WP-13 exported helpers are inlined:
their shares below come from `perf report --sort sym,srcfile|srcline` splits of the caller
(method: `results/RP-1/profile-splits.md`).

### Cumulative progress baseline → rp-1

Full e2e A/B (`ab e2e baseline rp-1`, 38 cases, 5 rounds, foreign load ≤ 0.4 %):

| group | baseline | rp-1 | speedup | d ≥ 7: baseline → rp-1 | peak RSS geomean | verdicts (total) |
|---|---|---|---|---|---|---|
| quad/sg | 22.35 s | 9.19 s | 2.43x | 18.51 → 6.93 s (2.67x) | 1.080 (3 MEM+) | 13 faster |
| quad/dsg | 26.72 s | 27.10 s | 0.986x | 26.12 → 26.53 s (0.985x) | 0.999 | 3 faster, 3 ~, 3 SLOWER (b1 d15 0.968, b1 d20 0.962, cc d10 0.993) |
| interp | 6.61 s | 5.70 s | 1.16x | 3.72 → 3.25 s (1.14x) | 1.006 | 8 faster |
| interplin | 5.01 s | 4.52 s | 1.11x | 4.39 → 3.96 s (1.11x) | 0.993 | 3 faster, 1 ~ |
| mivec | 9.27 s | 6.91 s | 1.34x | 7.05 → 6.16 s (1.14x) | 1.011 | 2 faster, 2 SLOWER (d15 0.902, d20 0.924) |
| **all** | **69.95 s** | **53.41 s** | **1.31x** | 59.78 → 46.84 s (1.28x) | **1.028** | 29 faster, 4 ~, 5 SLOWER |

mivec phases: operator 4.19 → 1.25 s (3.34x), generation 5.07 → 5.65 s (0.897x). The three
`MEM+` cases are all complete-SG quadrature at low d: `quad/sg/equi-exp/b0/d2/L20` 363.7 →
619.9 MiB (1.70x), `quad/sg/cc-uq/b0/d2/L20` 786.2 → 1079.1 MiB (1.37x), `quad/sg/cc-uq/b0/d3/L14`
37.8 → 40.5 MiB (1.07x); all other cases within their spread. Micro (29 common cases): 1.905 →
1.405 s (1.36x), 19 faster, 6 ~, 4 SLOWER (`downwards_closure` d10/d20 0.955/0.948,
`ct_coeffs` 0.959, `dsg/process_delta_tg/b1/d20` 0.985).

### Where the time goes (rp-1)

quad/dsg is now **51 %** of the suite (27.1 of 53.4 s) and holds the four longest cases:
`quad/dsg/equi-exp/b1/d20/L6` 7.04 s, `quad/dsg/cc-uq/b0/d10/L7` 7.02 s,
`quad/dsg/equi-exp/b0/d10/L7` 5.66 s, `quad/dsg/equi-exp/b1/d15/L7` 4.09 s (together 23.8 s =
45 %). Next: `interplin/.../d10/L5/p32` 2.47 s, `quad/sg/equi-sin/b0/d7/L7` 2.41 s (integrand
92 %), mivec d15/d20 2.10/2.07 s (generation only), `interp/sg/cc/b0/d10/L2/p8` 1.99 s, mivec d10
1.99 s. Generation phases ≤ 2 % except MIVec (5.65 s = 11 % of the suite). Per-case table:
`results/RP-1/groups.md`.

### Hot spots per operator (profiles, 16 threads)

| case (rp-1 total) | top shares (setup values in brackets) |
|---|---|
| `quad/dsg/equi-exp/b1/d20/L6` (7.04 s) | `appendSubTrieChildren` **64.8 %** [63 %] + `compSubTrie` 0.9 % · row loop 23.3 % [23 %] of which `updateCoeffSums` 13.3 % · integrand 7 % · serial preparation 0.29 s (micro) |
| `quad/dsg/equi-exp/b1/d15/L7` (4.09 s) | `appendSubTrieChildren` **44.4 %** + `compSubTrie` 1.3 % · row loop 35.8 % of which `updateCoeffSums` 19.9 % · integrand 13.5 % |
| `quad/dsg/equi-exp/b0/d10/L7` (5.66 s) | row loop **44.8 %** [41 %] = `updateCoeffSums` 15.0 / `compRowWeights` 9.1 / row + advance 9.2 / `std::function` call 4.6 / other 6.9 · integrand **50.5 %** [47 %] · memset 2.4 % |
| `quad/dsg/cc-uq/b0/d10/L7` (7.02 s) | row loop **35.0 %** = 11.8 / 6.4 / 7.4 / 3.9 / 5.5 · integrand **61.1 %** |
| `quad/sg/equi-exp/b0/d10/L4` (0.94 s) | integrand 70.9 % · `processTensorGrid` 25.2 % (`sumOverRows` 16.1, `std::function` call 7.4) · [setup: `denormalizeDataVector` 37 %, `getWeightForGP` 13 % — gone] |
| `quad/sg/equi-sin/b0/d7/L7` (2.41 s) | `sin` + integrand **91.6 %** [62 %] · `processTensorGrid` 6.3 % |
| `quad/sg/equi-exp/b1/d20/L5` (0.24 s) | integrand 65.2 % · `processTensorGrid` 12.5 % · per-tensor-grid setup (weights, mapping) ≈ 0.6 % · libgomp + OpenBLAS 17.4 % (1 thread: integrand 80.6 %, operator 15.4 %) |
| `quad/sg/cc-uq/b0/d2/L20` (1.19 s) | **CC 1D weights 36 %** (1 thread 44 %; Eigen kissfft 14.5 %, twiddles 6.2 %) [9 % + 2 %] · `QuadratureScratch` zero-fill + page faults **10.7 %** · integrand ≈ 31 % · kernel 22.7 % |
| `quad/sg/equi-exp/b0/d2/L20` (0.40 s) | integrand ≈ 48 % · `QuadratureScratch` zero-fill + page faults **17.8 %** · `processTensorGrid` 12.2 % · libgomp + OpenBLAS 17.4 % (41 grids, load imbalance) |
| `interp/sg/cc/b0/d10/L2/p8` (1.99 s) | `cos` + integrand 70.9 % · `interpolateFirstDim` 19.1 % = `denormalizeDataVector` 8.4 / `getGridPoint` 5.0 / vector access 3.1 / `std::function` 1.6 · barycentric (CC) 3.5 % · libgomp 3.9 % |
| `interp/sg/cc/b0/d5/L6/p64` (1.45 s) | `cos` + integrand 66.6 % [52 %] · `denormalizeDataVector` 6.8 % [12 %] · `getGridPoint` 7.8 % (+ 2.8 % vector access) [11 %] · `hasTrivialResult` + `nearlyEqual` 2.4 % [3 %] · libgomp 4.8 % |
| `interp/sg/cheb1/b0/d5/L4/p64` (0.34 s) | generic barycentric weights (`barycentricFormulaWeights` 9.0 + `log` 13.9 + `exp` 2.9) 25.8 % [18 %] + their allocations 5.5 % · libgomp **21.3 %** [≥ 15 %] · `cos` + integrand ≈ 25 % · 1 thread: weights 34.3 % + allocations 8.2 % |
| `interplin/sg/equi/b1/d10/L5/p32` (2.47 s) | `cos` + integrand 67.7 % · `computeNeighborGPWithWeight` 19.8 % · `denormalizeDataVector` (inlined) 4.3 % |
| `interplin/sg/equi/b1/d8/L5/p256` (1.50 s) | `cos` + integrand 68.7 % [57 %] · `computeNeighborGPWithWeight` 16.6 % [17 %] · `denormalizeDataVector` 5.0 % [11 %] |
| `mivec/sg/cc-exp/b1/d10/S12/n1000` (1.99 s) | `genMIVecDownwardsClosure` **78.6 %** [49 %] · quadrature ≈ 17 % |
| `mivec/.../d15/S10/n300`, `.../d20/S7/n300` (2.10 / 2.07 s) | `genMIVecDownwardsClosure` **96.6 % / 97.6 %** [– / 96 %] |

Observations:
- **The complete-SG operator is integrand-bound now** at d ≥ 5: operator overhead 6–25 %, of
  which the `std::function` dispatch of `SourceFunc::evaluate` (inlined call lines + `_M_invoke`)
  is 4–9 % — the same holds for the DSG row loop (5–6 %).
- **Per-tensor-grid costs of quad/sg are negligible** for many small grids (b1 d15/d20 < 1 %);
  they matter only for CC at low d, where the FFT weights of the few huge 1D rules dominate.
- **DSG b1: the trie work per delta grid is 66–79 %** (`appendSubTrieChildren` + `compSubTrie` +
  `updateCoeffSums`), **DSG b0: the row loop is 35–45 %**, with `updateCoeffSums` and
  `compRowWeights` (per-row work over all trie nodes, rows are short) as the largest parts.
- **Interpolation**: the helpers named in WP-06/WP-08 now hide in the callers (inlined), at
  roughly half of their setup shares; the integrand is 67–71 %.
- **libgomp spinning**: 21 % in `interp/sg/cheb1/b0/d5/L4/p64`, 4–9 % elsewhere; OpenBLAS threads
  4–9 % of the samples of short processes (`quad/sg/.../b1/d20/L5`, `.../d2/L20`).

### Micro-benchmarks and speed of light (rp-1, 1 thread)

| case | setup | rp-1 |
|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 vs. micro/sol/quad_tg/equi/b0/d10/L4 | 72.5 vs. 25.1 ns/point (2.9x) | **21.8 vs. 24.1 ns/point (0.90x)** |
| micro/quad/process_tg/equi/b0/d5/L10 vs. sol | 46.7 vs. 21.1 ns/point | 18.5 vs. 21.3 ns/point (0.87x) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 134.3 ns/point | 27.4 ns/point (1.26x of a large grid) |
| micro/quad/process_tg/cc/b0/d2/L16 vs. equi | 81.2 vs. 33.6 ns/point | 64.9 vs. 16.1 ns/point: CC weights ≈ 75 % of the call |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 27.7 ns/point | 27.8 ns/point = **1.28x the SG kernel** per point (152 vs. 215 instructions/point) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 · micro/dsg/prepare/equi/b1/d20/L6 | 665 ns/point · 288 ms | 693 ns/point · 292 ms (serial) |
| micro/interp/global_tg/cc/b0/d5/L6 · cheb1/b0/d5/L4 | 127.4 · 207.5 ns/point | 107.6 · 190.3 ns/point |
| micro/interp/barycentric_1d/generic/n65 vs. opt_cc/n65 | 577 vs. 5.8 ns/node | 576 vs. 3.0 ns/node (190x) |
| micro/interplin/tg/equi/b1/d8/L5 | 185.5 ns/corner | 166.0 ns/corner |
| micro/mivec/downwards_closure/d10/S10/n300 · d20/S6/n100 | 593 · 19.4 ms | 637 · 21.0 ms |
| micro/gen/complete_mis_with_coeffs/d20/L6 · gen/sparse_grid/cc/b1/d20/L5 | 16.3 · 83.0 ms | 9.4 · 65.7 ms |

Note: `ab micro` runs 0 warm-ups, so allocation-heavy cases show first-call times there
(`cc_weights/n32769` 4.4 vs. 2.3 ms, `gen/complete_mis_with_coeffs` 44.9 vs. 9.4 ms in `run
micro`); their A/B ratios are still valid (same conditions on both sides).

### Thread scaling (operator time, rp-1)

| case | 1 thread | 8 | 16 | 16 vs. 1 | peak RSS 1 / 8 / 16 threads |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d10/L4 | 8.85 s | 1.12 s | 0.93 s | 9.5x | 10.8 / 11.4 / 12.8 MiB |
| quad/sg/equi-exp/b1/d20/L5 | 2.05 s | 0.26 s | 0.21 s | 9.8x | 52.6 / 53.2 / 57.6 MiB |
| quad/sg/equi-exp/b0/d2/L20 | 2.03 s | 0.35 s | 0.33 s | 6.2x | 137 / 362 / 620 MiB |
| quad/sg/cc-uq/b0/d2/L20 | 4.66 s | 1.23 s | 1.10 s | **4.2x** | 298 / 800 / 1079 MiB |
| interp/sg/cheb1/b0/d5/L4/p64 | 2.64 s | 0.37 s | 0.34 s | 7.8x | 10.0 / 11.1 / 12.1 MiB |

(16 threads: medians of the A/B; 1 and 8 threads: one `run e2e`.) The d2/L20 cases scale poorly
(41 tensor grids whose sizes span three orders of magnitude, no parallelism inside a grid; for
CC the FFT weights of the level-20/19 rules sit on the critical path), and their peak memory
grows with the thread count (per-thread scratch, below).

### Hardware counters (`perf stat`, 16 threads, 1 warm-up + 2 samples)

| case | instructions | IPC | instructions per grid point | cache misses | page faults |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d10/L4 | 1.94e11 [7.1e11] | 2.10 [2.23] | ≈ 152 [555] | 1.6e6 | 1.6e3 |
| quad/dsg/equi-exp/b0/d10/L7 | 1.18e12 [1.20e12] | 2.19 [2.20] | ≈ 215 [218] | 2.3e7 | 1.0e4 |
| quad/dsg/equi-exp/b1/d20/L6 | 1.32e12 [1.34e12] | 2.05 [2.12] | ≈ 1570 [1590] | 1.26e9 [1.25e9] | 1.2e5 |
| quad/sg/equi-exp/b0/d2/L20 | 4.78e10 | 1.47 | ≈ 112 | 2.3e7 | **4.0e5** |
| quad/sg/cc-uq/b0/d2/L20 | 8.19e10 | 1.40 | ≈ 191 | 2.7e8 | **1.1e6** |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 3.53e11 | 1.76 | – | 3.4e6 | 1.2e3 |
| interp/sg/cc/b0/d5/L6/p64 | 2.79e11 | 1.98 | ≈ 660 per evaluation (5 `cos`) | 3.6e6 | 1.2e3 |

### MIVec generation: old master vs. rp-1

`ab e2e report-master rp-1` (mivec d10/d15/d20; master 3ffe4d382 built with its own flags, no
`-DNDEBUG`, naive CC weights, no Armadillo): generation master 1.03 / 1.55 / 1.52 s vs. rp-1
1.51 / 2.07 / 2.07 s (rp-1 0.688x / 0.748x / 0.733x; baseline 1.37 / 1.79 / 1.90 s). Both spend
91–98 % in `genMIVecDownwardsClosure` (same bounding-box algorithm). The difference is the inlined
dominance test: master compares `miVec(idx, dim)` element by element (`miDominatesMI`, scalar
early exit, unrolled ×8, IPC 2.11 at d20 / 2.48 at d15), while baseline and rp-1 go through
`miVecDominatesMI(miVec, paretoMaxima, MIView)` → `operator>=` →
`tools::mi_view::lessEqualInEveryDim`, which GCC 15 compiles to an AVX2 early-break loop with a
per-test prologue (index multiply, 32-byte alignment test of both operands, remainder dispatch)
and a scalar fallback for misaligned operands (rp-1: 24 % of the function's samples; baseline:
the alignment test `and $0x1f` alone is its hottest instruction, 10.4 %). The test usually exits
after few components, so the vector path does not pay off: rp-1 needs 15 % more instructions (d20;
−9 % at d15) at IPC 1.76 (+29–37 % cycles). The vectorization exists since the MIView refactor
(baseline); WP-14 (strict aliasing) made it 11–19 % slower. Master pays 4.5 % for malloc/free of
a `std::vector` copy per box element, which rp-1 avoids.

### Peak memory

Peak RSS = process floor (10.2–10.6 MiB: libraries incl. Armadillo/LAPACK/OpenBLAS; master
without them: 6.0 MiB; `OPENBLAS_NUM_THREADS=1` saves ≈ 1 MiB) + thread stacks/arenas (≈ 7–18
MiB at 16 threads: RSS − heap peak − floor) + case data. 22 of 38 cases stay below 20 MiB. heaptrack (16 threads, capped
at 8 GiB, `results/RP-1/heap/`) on the largest peaks:

| case (rp-1 peak RSS) | heap peak | owners at the peak |
|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 (1079 MiB) | 998 MiB | **per-thread `QuadratureScratch`: `weights1D` 268 MiB + `nodesPerDim` 268 MiB** (16 × 16.8 MiB each, sized by the largest tensor grid, zero-filled in every `quadrature()` call) · CC FFT temporaries (`genWeightsFFT`: `A`, `b`, `B`, `s`, `wcc`, `w01`, Eigen plan) ≈ 360 MiB, depending on how many large rules are computed concurrently · tensor grid node lists 101 MiB |
| quad/sg/equi-exp/b0/d2/L20 (620 MiB) | 638 MiB | the same two scratch buffers 537 MiB (84 %) · node lists 101 MiB |
| quad/dsg/equi-exp/b1/d20/L6 (329 MiB) | 301 MiB | **per-thread sub-trie vectors** (`appendSubTrieChildren` push_back, cpp:109) 154 MiB (51 %) · 230 230 delta tensor grid node lists 64 MiB (21 %, one small allocation each) · tensor grid data 26 MiB · MIs/counts 18 MiB · coefficient trie 19 MiB |
| quad/dsg/equi-exp/b1/d15/L7 (233 MiB) | 196 MiB | sub-trie vectors 102 MiB (52 %) · node lists 43 MiB · tensor grid data 19 MiB · MIs/counts 10 MiB · coefficient trie 10 MiB |
| quad/dsg/equi-exp/b0/d3/L16 (63 MiB) | 38 MiB | node lists 12.6 MiB · `DeltaQuadratureScratch` 2 × 7.9 MiB (same zero-filled per-thread pattern) · 1D weights 8.4 MiB |
| quad/sg/equi-exp/b1/d20/L5, b1/d15/L6 (58, 55 MiB) | 36, 33 MiB | node lists 16.8 / 16.9 MiB (≈ 50 k small allocations) · tensor grid data and MIs ≈ 13–19 MiB |
| quad/sg/cc-uq/b0/d3/L14 (40.5 MiB) | 19 MiB | scratch 2 × 3.9 MiB (the WP-01 `MEM+`: `nodesPerDim` +3.9 MiB) · node lists 5.5 MiB · CC temporaries 3.4 MiB |

**Cause of the WP-01 step**: WP-01 added `QuadratureScratch::nodesPerDim` (quadrature_scratch.hpp:47;
the nodes mapped onto the domain once per tensor grid). Like the older `weights1D`, every thread
allocates it in each `quadrature()` call with `sparseGrid.getMaxTGSumOverGPCntsPerDim()` doubles
and zero-fills it (`DataVector(n)` = `assign(n, 0.0)`): 2^21 + 4 doubles = 16.8 MiB × 16 threads
= 268 MiB. heaptrack WP-01-before vs. WP-01-after (`quad/sg/cc-uq/b0/d2/L20`): the
DataVector-allocation group grows 377.5 → 637.5 MiB, exactly by the new 251.7 MiB (15 workers) +
16.8 MiB (master) buffer; peak heap 672 → 1050 MiB. `quad/sg/equi-exp/b0/d2/L20` follows the same
pattern (+256 MiB measured). Runtime side effect: the zero-fill faults ≈ 525 MiB of fresh pages
per call (4.0e5 page faults over 3 pipelines), 10.7 % / 17.8 % of the samples of the two cases.
Only the threads processing the largest tensor grids need the full size; at 1 thread the peak is
137 / 298 MiB.

Material reductions (MiB / share of the peak; cost):
- Per-thread quadrature scratch without zero-fill or grown on demand (only touched pages count):
  ≈ −350…−500 MiB on the d2/L20 cases (what remains is at most the buffers of the 16 largest grids,
  ≈ 190 MiB; equi 620 → ≈ 120–270 MiB, cc 1079 → ≈ 580–730 MiB), −4 MiB on cc d3; runtime up to
  −0.07 / −0.13 s; a few lines in `QuadratureScratch` /
  `getWeights` / `mapNodesOntoDomain` (and `DeltaQuadratureScratch`). No API change.
- CC weights computed once per (rule, #nodes) per `quadrature()` call (WP-05) and with fewer
  temporaries in `genWeightsFFT`: up to ≈ −300 MiB on `quad/sg/cc-uq/b0/d2/L20`, runtime −0.4 s.
- DSG sub-trie scratch (WP-02): 154 / 102 MiB = 51 % of the heap peaks of b1 d20/d15; a design
  that does not materialize one sub-trie per delta grid per thread saves most of it; a cache of
  sub-tries could increase it.
- Shared 1D node arrays instead of per-tensor-grid copies: −64 MiB (b1 d20 DSG, 19 %), −101 MiB
  (d2/L20, 16 %), −17 MiB (b1 d20 SG, 29 %); changes the public `TensorGrid` storage → interface.
- Not linking Armadillo (WP-10): −1…−4.5 MiB per process (floor 10.5 → ≥ 6 MiB).

### Findings → work packages (RP-1)

Re-ranking with expected savings: summary `summaries/RP-1-reprofiling.md` ("Profile shift /
follow-ups"). New in RP-1: quadrature scratch zero-fill (memory + page faults), the vectorized
MI dominance test (mivec generation 1.33–1.46x slower than master), `std::function` dispatch per
grid point (4–9 % of the quadrature kernels; interface), load imbalance of quad/sg at low d,
shared node arrays (memory; interface).

## Re-profiling RP-2

Work session RP-2, 2026-10-29. Code: 5215417ea (= `rp-2` snapshot; since RP-1 the accepted WP-20
and WP-02). Judged configuration, boost off, `calibrate` OK (e2e 1.003 burst / 0.999 sustained,
micro 0.994 / 0.998). Reports in `results/RP-2/`:
- A/Bs: `e2e.md/.json` (full suite baseline → rp-2), `e2e-rp-1-vs-rp-2-*.md` (direct checks of
  shifts in unchanged code); `micro-run.json`.
- derived data: `groups.md`, `profile-splits.md`, `wp21-sizing.md`, `wp22-sizing.md`,
  `frontend-per-symbol.md`, `perfstat-frontend-rp-2.txt`, `mivec-rp-1-vs-rp-2.md`,
  `generation-page-faults.md`;
- top lists and summaries: `profiles/`, `heap/`.

Method change: shares of inlined helpers come from the inline chain (`addr2line -i`) of every
sampled address instead of RP-1's `--sort srcline` (STL lines count for the helper that inlined
them; see `profile-splits.md`).

### Cumulative progress baseline → rp-2

Full e2e A/B (`ab e2e baseline rp-2`, 38 cases, 5 rounds, foreign load ≤ 1.5 %). rp-1 = the RP-1
report (other session); per-group rp-1 → rp-2 from those numbers:

| group | baseline | rp-1 | rp-2 | baseline → rp-2 | rp-1 → rp-2 | d ≥ 7: baseline → rp-2 | peak RSS geomean | verdicts (total) |
|---|---|---|---|---|---|---|---|---|
| quad/sg | 22.29 s | 9.19 s | 9.18 s | 2.43x | 1.001x | 18.53 → 6.93 s (2.67x) | 1.064 (2 MEM+) | 13 faster |
| quad/dsg | 26.73 s | 27.10 s | **17.74 s** | 1.51x | **1.528x** | 26.13 → 17.20 s (1.52x) | 0.979 (6 MEM+, 2 MEM−) | 9 faster |
| interp | 6.64 s | 5.70 s | 5.73 s | 1.16x | 0.993x | 3.72 → 3.27 s (1.14x) | 0.999 | 5 faster, 3 ~ |
| interplin | 5.09 s | 4.52 s | 4.65 s | 1.09x | 0.971x (drift; direct A/B: noise) | 4.41 → 4.10 s (1.08x) | 1.009 | 3 faster, 1 ~ |
| mivec | 9.28 s | 6.91 s | 6.35 s | 1.46x | 1.088x | 7.08 → 5.61 s (1.26x) | 1.004 | 3 faster, 1 ~ |
| **all** | **70.03 s** | **53.41 s** | **43.65 s** | **1.604x** | **1.224x** | 59.87 → 37.10 s (1.61x) | **1.017** | 33 faster, 5 ~, 0 SLOWER |

- mivec phases: generation 5.11 → 5.10 s (rp-1 5.65 s), operator 4.18 → 1.24 s.
- quad/dsg b0 15.30 s, b1 2.44 s (rp-1: 15.97 / 11.13 s).
- Direct rp-1 → rp-2 A/Bs of unchanged code:
  - mivec generation d10/d15/d20 1.063 / 1.131 / 1.130x faster (code generation, below);
  - interplin all noise;
  - quad/dsg b1 `generation` 0.843 / 0.851x (page faults, below).
- Peak memory: 8 `MEM+`.
  - WP-01 scratch: `quad/sg/equi-exp/b0/d2/L20` 363.7 → 619.7 MiB, `quad/sg/cc-uq/b0/d2/L20`
    833.3 → 1072.2 MiB.
  - WP-02 slot arrays: the six quad/dsg b0 cases +1.2…+5.8 MiB (1.05–1.16x).
- 2 `MEM−`: `quad/dsg/equi-exp/b1/d20/L6` 330.6 → 217.1 MiB, `.../b1/d15/L7` 230.7 → 150.9 MiB.
- Floors: 10.4 / 10.1 MiB. Per-case table: `results/RP-2/groups.md`.

### Where the time goes (rp-2)

| group | share of the suite | of which |
|---|---|---|
| quad/dsg | 40.6 % | b0 35.1 % · b1 5.6 % |
| quad/sg | 21.0 % | |
| mivec | 14.5 % | generation 11.7 % |
| interp | 13.1 % | |
| interplin | 10.7 % | |

Longest cases:

| case | total |
|---|---|
| `quad/dsg/cc-uq/b0/d10/L7` | 6.77 s |
| `quad/dsg/equi-exp/b0/d10/L7` | 5.41 s |
| `interplin/sg/equi/b1/d10/L5/p32` | 2.52 s |
| `quad/sg/equi-sin/b0/d7/L7` | 2.42 s |
| `interp/sg/cc/b0/d10/L2/p8` | 1.99 s |
| mivec d10 / d15 / d20 | 1.90 / 1.89 / 1.83 s |
| `interplin/sg/equi/b1/d8/L5/p256` | 1.58 s |
| `interp/sg/cc/b0/d5/L6/p64` | 1.48 s |
| `quad/dsg/cc-uq/b0/d7/L10` | 1.44 s |
| `quad/sg/cc-uq/b0/d10/L4` | 1.31 s |
| `quad/dsg/equi-exp/b1/d20/L6`, `.../b1/d15/L7` | 1.23, 1.22 s |

The two b0 d10 DSG cases alone are 28 % of the suite.

### Hot spots per operator (profiles, 16 threads; RP-1 values in brackets)

| case (rp-2 total) | top shares |
|---|---|
| `quad/dsg/equi-exp/b0/d10/L7` (5.41 s) | row loop **43.1 %** [44.8] = `updateCoeffSums` 14.4 · `compRowWeights` 11.6 · row + advance 9.4 · `std::function` call 5.3 · lambda 2.4 · integrand **52.5 %** [50.5] · memset 2.1 % |
| `quad/dsg/cc-uq/b0/d10/L7` (6.77 s) | row loop **32.8 %** [35.0] = 10.9 / 8.3 / 7.4 / 4.4 / 1.9 · integrand **63.6 %** [61.1] |
| `quad/dsg/equi-exp/b0/d7/L10`, `cc-uq/b0/d7/L10` (1.14, 1.44 s) | row loop 42.9 / 33.9 % (as d10; first measurement at d7) · integrand 48.5 / 58.5 % |
| `quad/dsg/equi-exp/b1/d20/L6` (1.23 s) | integrand **52.8 %** · row loop 21.3 % (4.8 / 5.9 / 5.4 / 3.5 / 1.7) · projection builder **13.6 %** (`addChildren` 12.0) + memset 1.0 · libgomp 2.0, OpenBLAS 1.7 · not in the samples: serial preparation 65.7 ms (5.4 % of the wall time) and generation 61 ms (5 %) |
| `quad/dsg/equi-exp/b1/d15/L7` (1.22 s) | integrand **55.6 %** · row loop 24.8 % (5.5 / 6.6 / 6.2 / 4.5 / 2.0) · projection builder **9.6 %** (`addChildren` 8.3) + memset 1.0 · generation 43 ms |
| `quad/sg/*` | unchanged within ≈ 1 point: equi-exp/b0/d10/L4 integrand 71.1 %, `processTensorGrid` 24.9 % [25.2]; equi-sin/b0/d7/L7 91.1 / 6.4 %; cc-uq/b0/d10/L4 77.9 / 18.7 %; b1/d20/L5 64.6 / 12.4 % + libgomp/OpenBLAS ≈ 15 %; cc-uq/b0/d2/L20 CC weights ≈ 24 % + kernel 21.9 % [22.7]; equi-exp/b0/d2/L20 kernel 17.9 % [18.1] |
| `interp/*`, `interplin/*` | unchanged: `interpolateFirstDim` 18.9 % (cc d10) / 19.9 % (cc d5) [19.1 / 20.2]; cheb1 d5 generic weights + `log`/`exp` 24.0 % [25.8], libgomp 18.9 % [21.3]; `computeNeighborGPWithWeight` 19.9 / 16.5 % [19.8 / 16.6] (d10 / d8) |
| `mivec/...` d10 / d15 / d20 | `genMIVecDownwardsClosure` **77.5 / 96.1 / 97.2 %** [78.6 / 96.6 / 97.6] |

Observations:
- **DSG b0 is unchanged in structure.** The row loop lost 1.7–2.2 points (WP-02/WP-20's
  1.4–4.6 %), and d7 matches d10.
  - The per-row trie work (`updateCoeffSums` + `compRowWeights`) is 26 % (equi) / 19 % (cc) of
    the b0 cases ≈ 3.3 s in absolute time (b0 d7/d10).
  - The row loop costs 5.53 s over b0 d7/d10.
- **DSG b1: the framework is no longer the main cost.** The integrand is 53–56 %, the row loop
  21–25 %, the projection builder 10–14 % (≈ 0.31 s over d15 + d20).
  - The single-threaded parts (serial preparation 66 ms, the parallel generation 61 ms) are each
    ≈ 5 % of the d20 wall time.
- quad/sg, interp, interplin and mivec: no change since RP-1 (no code of theirs changed).

### Micro-benchmarks (rp-2, `run micro`, 1 thread)

| case | rp-1 | rp-2 |
|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 vs. micro/sol/quad_tg/equi/b0/d10/L4 | 21.8 vs. 24.1 ns/point | 21.84 vs. 24.10 ns/point (0.91x) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 27.8 ns/point (1.28x the SG kernel) | **26.12 ns/point (1.20x the SG kernel)** |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | – | 28.3 ns/point |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 693 ns/point | **30.2 ns/point** |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 · b1/d15/L7/s128 | – | 48.5 · 33.3 ns/point |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 · b0/d10/L7/s16 | – | 19.65 · 9.17 ms |
| micro/dsg/prepare/equi/b1/d20/L6 (serial) | 292 ms | **65.7 ms** (WP-02's 113 ms = 0-warm-up `ab micro`) |
| micro/mivec/downwards_closure/d10/S10/n300 · d20/S6/n100 | 637 · 21.0 ms | 621 · 21.1 ms |
| micro/mivec/ct_coeffs/d10/S12/n1000 | – | 15.6 ms |
| micro/interp/global_tg/cc/b0/d5/L6 · cheb1/b0/d5/L4 | 107.6 · 190.3 ns/point | 107.7 · 192.1 ns/point |
| micro/interp/barycentric_1d/generic/n65 vs. opt_cc/n65 | 576 vs. 3.0 ns/node | 577 vs. 3.0 ns/node |
| micro/interplin/tg/equi/b1/d8/L5 | 166.0 ns/corner | 169.3 ns/corner |
| micro/gen/complete_mis_with_coeffs/d20/L6 · gen/sparse_grid/cc/b1/d20/L5 | 9.4 · 65.7 ms | 9.4 · 68.4 ms |

Thread scaling was not re-measured: no code that RP-1 measured there changed.

### DSG b1: WP-21 sizing (`results/RP-2/wp21-sizing.md`)

Delta grids with the same single-node-dimension set P share the projected coefficient function.

| | d20 L6 | d15 L7 |
|---|---|---|
| distinct sets P vs. delta grids | 60 460 vs. 230 230 (3.8 per set) | 16 384 vs. 170 544 (10.4 per set) |
| DFS node visits (exact count for complete grids, reproduces WP-02's 6.2e7) | 6.20e7 | 4.84e7 |
| per-set cache | −34.5 % | −49.8 % |

The cache saves this little because each set's smallest member (m = 1_P), which has the
largest sub-trie, must still be built.

The DFS also rejects 4.8e7 / 3.6e7 children with `continue` (44 % / 42 % of its child
iterations). A slack bound `lvl ≤ deltaLvl + remaining slack` (children are sorted, so it can
`break`) is valid for every set and removes them.

Builder time ≈ 0.31 s (d15 + d20) → expected **0.1–0.2 s** for both levers.

The serial preparation (65.7 ms at d20) can be added to the same WP:
- radix pass 64 % of the trie constructor;
- the two tensor-grid scans ≈ 18 ms;
- `FlatNestedVec` doubling +8.4 MB at the d20 heap peak.

### Tensor grids as struct of arrays: WP-22 sizing (`results/RP-2/wp22-sizing.md`)

Each `TensorGridCTData` entry is 112 B inline plus three heap allocations (level MI 4d B, counts
4d B, node list). At d20, 690 690 of the process's 692 268 allocations belong to the 230 230
tensor grids.

**Memory**: the b1 d20 DSG holds 126.4 MB (requested; ≈ 137 MB with glibc chunk overhead):

| part | size |
|---|---|
| struct array | 25.8 MB |
| level MIs | 18.4 MB |
| counts | 18.4 MB |
| node lists | 63.8 MB |

The heap peak (178.4 MB) occurs in the operator's `CTCoeffTrie` construction, which adds
51.7 MB of transients: the column-major level copy 18.4 MB, the node `FlatNestedVec` doubling
16.8 + 8.4 MB, the order arrays 6.3 MB.

Savings:
- SoA (levels in one `LvlMIVec` taken over from the generation, coefficients in one array,
  counts derived, nodes flat): −50 MB at d20, −36 MB at d15, −12 MB at sg b1 d20 L5.
- Together with WP-18 (shared node arrays): −96…−114 / −68…−78 / −25…−29 MB, and −101 MB at
  sg d2 L20.
- mivec cases: nil (heap 0.4 MB).

**Runtime**: walking the tensor-grid list shows up in three places:
- the serial preparation: level copy 8.6 % of its samples / 22.4 % of its cache misses,
  `comp1DWeights`' max-level scan 7.9 / 18.0 %; ≈ 18 ms per call at d20;
- generation: 3 allocations per tensor grid; malloc/free ≈ 18 % of the SG generation micro;
- the per-tensor-grid operator setup: ≤ 0.4 %.

Expected **≈ 0.03–0.07 s** on the suite (minimal), mostly in the generation of the b1 cases.

**Interface**: `TensorGridCTData` and its members are exposed by reference in many places:
- `SparseGridBase::getTensorGrid(idx)`, `getTensorGrid(mi)` (iterator), `getTensorGrids()`
  (`std::span`), the iterators incl. a mutable one;
- `TensorGrid::getGPCntPerDim()` / `getNodesPerDim()`;
- internal operator helpers in public headers;
- Python: read-only `mi` / `tensorGrid` properties with `_owner`, `getTensorGrids()` as
  `TensorGridCTDataVector`;
- the protected `perf_e2e.cpp:81` and `perf_reference.cpp:136-144`. They need a proxy with the
  same member names (`.mi`, `.coefficient`, `.tensorGrid.nGP()/getGPCntPerDim()/getNodesPerDim()`),
  or the owner adapts them.

**Merge with WP-18**: yes, it is the same storage and accessors; doing them separately would
change the public access twice.

### Code placement (`results/RP-2/frontend-per-symbol.md`)

Process-wide op-cache misses are 2.5–4.3 per 1000 accesses in the b0 / interp / interplin /
mivec cases, and 10–15 at b1 high d. Front-end stall cycles:

| loop / case | front-end stalls | assessment |
|---|---|---|
| quad/sg `processTensorGrid` | 0.5 % | not front-end-bound |
| DSG b0 row loop (aligned since WP-02) | 0.7 % | not front-end-bound |
| interp cc d5 | 0.9 % | not front-end-bound |
| interp cc d10 | 2.8 % | candidate |
| interplin d8 / d10 | 2.9 / 5.7 % | candidate (`computeNeighborGPWithWeight` 4.6–6.3 % of its own cycles) |
| mivec closure | 4.1 % | candidate |

The per-symbol split comes from non-precise samples (skid toward the callees), so it is only
indicative.

Alignment candidates (propose only):
- `linear_interpolation::computeNeighborGPWithWeight` / the corner loop (WP-08);
- the closure's dominance loop (WP-03 rewrites it);
- `interpolateFirstDim` at d10 (WP-06).

Their upper bound is the stall share (≤ 3–6 % of those cases).

### Unchanged code, changed timings

- **mivec generation 1.06–1.13x faster since rp-1** (direct A/B), with the closure's source
  unchanged.
  - LTO compiled the OpenMP function differently: 8.3 → 11.0 KB, −2.1 % instructions, −11.5 %
    cycles, IPC 1.76 → 1.94. Op-cache miss rates are equal, so it is not placement.
  - The hot loop still has RP-1's structure (alignment test `test $0x1f` 7.3 %, `vptest`
    early-break loop, per-test `imul`).
  - The vectorized dominance test is fragile. WP-03's scalar early-exit first step now saves ≈ 1.0 s
    (to master speed 1.03 / 1.55 / 1.52 s) instead of 1.55 s.
- **quad/dsg b1 `generation` 0.84–0.85x since rp-1** (WP-02's 50.97 → 59.54 ms persists:
  50.9 → 60.9 ms).
  - Cause: page faults. With glibc trimming and the mmap threshold disabled (diagnostic
    `GLIBC_TUNABLES`), warm generation is 30.5–31 ms in both builds.
  - The warm-up's freed operator memory was larger in rp-1 (154 MB of sub-trie vectors), so the
    next generation reused more mapped pages.
  - Not a regression; about half of the warm b1 d20 generation is faulting in the grid's 126 MB,
    which WP-22/WP-18 reduce.

### Peak memory (rp-2)

Largest peaks above the floor (10.1 MiB):

| case | peak RSS |
|---|---|
| `quad/sg/cc-uq/b0/d2/L20` | 1072 MiB |
| `quad/sg/equi-exp/b0/d2/L20` | 620 MiB |
| `quad/dsg/equi-exp/b1/d20/L6` | 217 MiB |
| `.../b1/d15/L7` | 151 MiB |
| `quad/dsg/equi-exp/b0/d3/L16` | 62.6 MiB |
| `quad/sg/equi-exp/b1/d20/L5` | 56.9 MiB |
| `.../b1/d15/L6` | 52.8 MiB |

All other cases are ≤ 43 MiB. heaptrack (16 threads, capped at 8 GiB, `results/RP-2/heap/`):

| case (peak RSS) | heap peak | owners at the peak |
|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 (1072 MiB) | 998 MB | as in RP-1: per-thread `QuadratureScratch` 503 + 34 MB · CC FFT temporaries 361 MB · node lists 101 MB (→ WP-05, WP-18) |
| quad/sg/equi-exp/b0/d2/L20 (620 MiB) | 638 MB | scratch 537 MB · node lists 101 MB (→ WP-05, WP-18) |
| quad/dsg/equi-exp/b1/d20/L6 (217 MiB) | 178 MB | DSG 126 MB (node lists 64, struct array 26, level MIs 18, counts 18) + `CTCoeffTrie` construction 52 MB (→ WP-22 + WP-18; trie reservation → WP-21) |
| quad/dsg/equi-exp/b1/d15/L7 (151 MiB) | 111 MB | DSG 83 MB (nodes 43, struct 19, MIs 10, counts 10) + trie construction 28 MB |
| quad/dsg/equi-exp/b0/d10/L7 (42 MiB) | 24 MB | per-thread projected trie nodes 8.9 MB + slot blocks 4.6 MB (the WP-02 `MEM+`; `TrieNode` 24 → 16 B in WP-04: ≈ −3 MB) · node lists 4.8 MB |
| quad/dsg/equi-exp/b0/d3/L16 (63 MiB) | 39 MB | `DeltaQuadratureScratch` 15.7 MB (zero-filled per thread, → WP-05's on-demand pattern) · node lists 12.6 MB · 1D weights 8.4 MB |
| quad/sg/equi-exp/b1/d20/L5 (57 MiB) | 36 MB | node lists 16.8 MB · struct array 6.0 · MIs 4.3 · counts 4.3 · generated `LvlMIVec` 4.3 MB |

Material reductions (unchanged from RP-1 unless stated):
- WP-05: −350…−500 MiB on the d2 cases.
- WP-22 + WP-18: b1 d20 −96…−114 MB, d15 −68…−78 MB, sg d2 −101 MB, sg b1 d20 −25 MB (new
  sizing above).
- Trie node reservation: −8 MB at the b1 d20 peak.
- WP-04 `TrieNode`: −3 MB per b0 case.

### Findings → work packages (RP-2)

Re-ranking with expected savings: summary `summaries/RP-2-reprofiling.md` ("Profile shift /
follow-ups"). In short:
- WP-03 (≈ 4.5 s, first step ≈ 1.0 s) now clearly ranks first, before WP-04 (1.5–2.5 s).
- WP-21 is sized at 0.1–0.2 s.
- WP-22 is minimal in runtime and material in memory; it should be merged with WP-18.
- No new hot spots. New small findings: the slack break in the sub-trie DFS, trie node
  reservation, generation page faults, and the fragile closure code generation.

## Re-profiling RP-3

Work session RP-3, 2026-10-30. Code: 0c6ecfc4b (= `rp-3` snapshot; since RP-2 the accepted WP-03,
the `SourceFunc` cleanup 0273aa1d7 (unused cache member removed) and the WP-24/WP-25 benchmarks;
no other production code). Judged configuration, boost off, `calibrate` OK (e2e 1.002 burst /
1.001 sustained, micro 1.032 / 1.034). Reports in `results/RP-3/`:
- A/Bs: `e2e.md/.json` (full suite baseline → rp-3), `e2e-rp-2-vs-rp-3-interp.md/.json` (direct
  check of interp/interplin); `micro-run.json`;
- derived data: `groups.md`, `profile-splits.md`, `mivec-generation-split.md`,
  `openblas-cold-start.md` (+ `openblas-diag-raw.json`);
- top lists and inline-chain splits: `profiles/` (21 cases);
- scripts: `profile_split.py` (RP-2 method), `groups.py`, `openblas_diag.py`,
  `mivec_gen_split.cpp` (scratch diagnostic, not built by SCons).

### Cumulative progress baseline → rp-3

Full e2e A/B (`ab e2e baseline rp-3`, 38 cases, 5 rounds, foreign load ≤ 0.3 %). rp-2 = the RP-2
report (other session):

| group | baseline | rp-2 | rp-3 | baseline → rp-3 | rp-2 → rp-3 | d ≥ 7: baseline → rp-3 | peak RSS geomean | verdicts (total) |
|---|---|---|---|---|---|---|---|---|
| quad/sg | 22.28 s | 9.18 s | 9.20 s | 2.42x | 0.997x | 18.50 → 6.93 s (2.67x) | 1.064 (2 MEM+) | 13 faster |
| quad/dsg | 26.70 s | 17.74 s | 17.75 s | 1.50x | 1.000x | 26.10 → 17.20 s (1.52x) | 0.973 (6 MEM+, 2 MEM−) | 9 faster |
| interp | 6.60 s | 5.73 s | 5.70 s | 1.16x | 1.006x | 3.70 → 3.25 s (1.14x) | 1.003 | 7 faster, 1 ~ |
| interplin | 5.02 s | 4.65 s | 4.55 s | 1.10x | 1.023x (drift; direct A/B: noise) | 4.40 → 4.00 s (1.10x) | 0.995 | 4 faster |
| mivec | 9.32 s | 6.35 s | **1.26 s** | **7.42x** | **5.05x** | 7.05 → 0.51 s (13.7x) | 0.995 (1 MEM−) | 4 faster |
| **all** | **69.92 s** | **43.65 s** | **38.45 s** | **1.818x** | **1.135x** | 59.76 → 31.90 s (1.87x) | **1.014** | 37 faster, 1 ~, 0 SLOWER |

- mivec: generation 13.8 ms (rp-2 5.10 s), operator 1.241 s; d10 / d15 / d20 3.92 / 68.5 / 593x
  since rp-2, d5 unchanged.
- quad/dsg b0 15.30 s, b1 2.45 s (rp-2 15.30 / 2.44 s).
- Direct rp-2 → rp-3 A/B of interp + interplin (12 cases): all noise except
  `interp/sg/cheb1/b0/d5/L4/p64` 0.980 [0.962, 0.998] (337 → 342 ms, unchanged source:
  placement / LTO); RP-2's interplin numbers were session drift.
- Peak memory: 8 `MEM+` (as RP-2: sg d2 WP-01 scratch 364 → 620 / 805 → 1064 MiB; 6 dsg b0 WP-02
  slot arrays +1.1…+5.1 MiB), 3 `MEM−` (dsg b1 d20 331 → 218 MiB, d15 234 → 150 MiB, mivec d10
  17.7 → 17.1 MiB). rp-2 → rp-3 per case geomean 0.995 (cross-session). Floors 10.3 / 10.3 MiB.

### Where the time goes (rp-3)

| group | share of the suite | of which |
|---|---|---|
| quad/dsg | **46.2 %** | b0 39.8 % · b1 6.4 % |
| quad/sg | 23.9 % | |
| interp | 14.8 % | |
| interplin | 11.8 % | |
| mivec | 3.3 % | operator 3.2 % · generation 0.04 % |

Longest cases: `quad/dsg/cc-uq/b0/d10/L7` 6.79 s, `quad/dsg/equi-exp/b0/d10/L7` 5.40 s (together
31.7 % of the suite), `interplin/.../d10/L5/p32` 2.47 s, `quad/sg/equi-sin/b0/d7/L7` 2.42 s,
`interp/sg/cc/b0/d10/L2/p8` 1.99 s, `interplin/.../d8/L5/p256` 1.53 s, `interp/sg/cc/b0/d5/L6/p64`
1.45 s, `quad/dsg/cc-uq/b0/d7/L10` 1.44 s, `quad/sg/cc-uq/b0/d10/L4` 1.31 s, `interp/sg/cc/b0/d7/L4/p16`
1.26 s, dsg b1 d20 / d15 1.23 / 1.22 s, sg cc d2 L20 1.20 s. mivec: d5 0.74 s, d10 0.48 s.

### Hot spots per operator (profiles, 16 threads; RP-2 values in brackets)

Details and the inline-chain method: `results/RP-3/profile-splits.md`.

| case (rp-3 total) | top shares |
|---|---|
| `quad/dsg/equi-exp/b0/d10/L7` (5.40 s) | row loop **43.2 %** [43.1] = `updateCoeffSums` 14.5 · `compRowWeights` 11.6 · row + advance + lambda 11.6 · call 5.4 + `_M_invoke` 1.4 · integrand 52.4 % [52.5] · memset 2.6 % |
| `quad/dsg/cc-uq/b0/d10/L7` (6.79 s) | row loop 33.1 % [32.8] = 10.8 / 8.5 / 9.4 / 4.4 + 1.2 · integrand 63.3 % [63.6] |
| dsg b0 d7 equi / cc (1.14 / 1.44 s) | row loop 43.3 / 33.8 % [42.9 / 33.9] · integrand 48.2 / 58.7 % |
| `quad/dsg/equi-exp/b1/d20/L6` (1.23 s) | integrand 51.9 % [52.8] · row loop 21.1 % [21.3] · projection builder 13.1 % [13.6] · libgomp 2.8, OpenBLAS 2.7 |
| `quad/dsg/equi-exp/b1/d15/L7` (1.22 s) | integrand 55.2 % [55.6] · row loop 24.9 % [24.8] · builder 9.0 % [9.6] |
| `quad/sg/*` | equi-exp/b0/d10/L4 `processTensorGrid` 25.2 % [24.9] (rows 16.1, call 7.1, weight products 1.7); equi-sin/b0/d7/L7 integrand 91.4 / kernel 6.3 %; cc-uq/b0/d10/L4 77.7 / 18.6 %; b1/d20/L5 66.8 / 12.7 % + OpenBLAS 7.5 + libgomp 6.6 %; cc-uq/b0/d2/L20 CC weights ≈ 24.8 % [≈ 24] + kernel 23.1 % [21.9] |
| `interp/*` | cc d10 / d7 / d5: `interpolateFirstDim` 19.1 / 17.8 / 20.0 % [18.9 / – / 19.9] (incl. `getGridPoint` 5.6 / 7.4 / 8.5) + barycentric 3.5 / 4.8 / 6.0 %; cheb1 d5: generic weights + `log`/`exp` 24.8 % [24.0], allocations 7.2 %, libgomp 21.3 % [18.9] |
| `interplin/*` | `computeNeighborGPWithWeight` 20.5 / 17.6 % [19.9 / 16.5], `interpolate` 7.6 / 8.2 % (d10 / d8); integrand 66.7 / 66.6 % |
| `mivec/...` d5 / d10 | integrand 76.9 / 70.2 % · `processTensorGrid` 15.2 / 11.8 % · libgomp 3.6 / 8.9 · OpenBLAS 3.0 / 6.4 % · generation symbols ≤ 0.2 % [closure 77.5 % at d10] |
| `mivec/...` d15 / d20 | spinning threads: libgomp 47.0 / 50.6 %, OpenBLAS 29.0 / 39.7 % (cold first call, below) [closure 96.1 / 97.2 %] |

Observations:
- **Unchanged code, unchanged shares**: quad/dsg, quad/sg, interp and interplin are within ≈ 1
  point of RP-2 (no LTO shift of unchanged code this time, unlike the closure in RP-2).
- Absolute (shares × rp-3 medians): DSG b0 d7/d10 row loop **5.55 s** [5.53], per-row trie work
  **3.30 s** [3.3], dispatch 0.92 s [0.9]; b1 row loop 0.56 s, builder 0.27 s [≈ 0.31], dispatch
  0.13 s; quad/sg dispatch ≈ 0.45–0.55 s (0.29 s in the profiled cases) + mivec 0.11 s;
  `interpolateFirstDim` + barycentric 0.73 s at d ≥ 7; corner loop 1.09 s (d8 + d10).
- `libsgppbase.so` ≤ 0.06 % at d ≥ 7 (WP-23: ≤ 0.09 %).

### WP-29 kernels: the "before" picture

| kernel (WP-29 step) | case | kernel share | dispatch inside | integrand |
|---|---|---|---|---|
| SG row loop `processTensorGrid` / `sumOverRows` (a) | quad/sg equi-exp b0 d10 / cc-uq b0 d10 / equi-sin b0 d7 / equi-exp b1 d20 | 25.2 / 18.6 / 6.3 / 12.7 % | 9.0 / 6.7 / 2.7 / 5.1 % | 70.3 / 77.7 / 91.4 / 66.8 % |
| SG row loop in mivec (a) | mivec d5 / d10 | 15.2 / 11.8 % | 10.2 / 6.9 % | 76.9 / 70.2 % |
| DSG row loop `forEachGPWithGlobalWeight` (b) | dsg equi / cc b0 d10, equi / cc b0 d7 | 43.2 / 33.1 / 43.3 / 33.8 % | 6.8 / 5.6 / 7.3 / 6.0 % | 52.4 / 63.3 / 48.2 / 58.7 % |
| DSG row loop (b) | dsg b1 d20 / d15 | 21.1 / 24.9 % (+ builder 13.1 / 9.0 %) | 4.6 / 5.7 % | 51.9 / 55.2 % |
| `interpolateFirstDim` + barycentric (c) | interp cc d10 / d7 / d5 | 22.6 / 22.6 / 26.0 % | 2.5 / 1.1 / 1.8 % | 70.7 / 70.7 / 67.2 % |
| generic barycentric formula (c) | interp cheb1 d5 | weights 24.8 + allocations 7.2 + `interpolateFirstDim` 7.3 % | 0.6 % | ≈ 24 % |
| corner loop `computeNeighborGPWithWeight` + `interpolate` (c) | interplin d10 / d8 | 28.1 / 25.8 % | 0.7 / 0.8 % | 66.7 / 66.6 % |

A vector run with m outputs multiplies the integrand part by m and keeps the per-point framework
work (trie weights, row odometer, `getGridPoint`, corner weights) once per point; the larger the
kernel share, the larger the vector gain (WP-24: DSG 1.89–2.77x, SG 1.33–2.56x at m = 32).

### mivec after WP-03 (`results/RP-3/mivec-generation-split.md`)

- The group is **operator-bound**: operator 1.241 s, generation 13.8 ms (1.1 %).
- Generation split (scratch program, same MI sets, 16 threads, warm), d10: closure 0.74 ms (10 %),
  **CT coefficients 4.82 ms (64 %)**, filter + partition 1.38 ms (18 %), tensor-grid
  materialization 0.61 ms (8 %); d15 / d20: CT coefficients 70 / 71 %. The CT coefficients scale
  only 1.7–3.8x from 1 to 16 threads (d10 14.8 → 4.8 ms).
- 13.8 ms = 0.036 % of the suite: no WP. For dimension-adaptive refinement the incremental update
  of the CT coefficients would be the relevant part.

### OpenBLAS pool: cold first call (`results/RP-3/openblas-cold-start.md`)

Diagnostic with `OPENBLAS_NUM_THREADS=1` (no code change), 16 threads:
- The judged `total` (after a warm-up) is unchanged (0.97–1.02).
- The **first operator call of a process** is **+63…+130 ms** slower with the pool:
  - mivec d20 135.7 vs. 6.2 ms (22x);
  - interp equi d3 169 vs. 54 ms;
  - dsg b0 d10 5476 vs. 5408 ms.
- Process wall time +56…+135 ms; `--list` (libraries only) only +2–3 ms.
- Mechanism (`strace`): the 15 OpenBLAS threads busy-wait after their start; OpenMP barrier
  wake-ups then complete only every scheduler tick (4 ms) until the pool sleeps.
- The "OpenBLAS x %" samples in short-case profiles are this warm-up spinning.
- Relevant for every one-shot process: C++ programs, Boost tests, presumably Python scripts.

### Micro-benchmarks (rp-3, `run micro` filtered, 1 thread; RP-2 in brackets)

Calibration micro 1.032 (RP-2 0.994), so +1–3 % across the board is machine state.

| case | rp-3 [rp-2] |
|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 vs. micro/sol/quad_tg/equi/b0/d10/L4 | 21.98 vs. 24.82 ns/point [21.84 vs. 24.10] |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 26.50 ns/point [26.12] = 1.21x the SG kernel [1.20] |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 · b1/d20/L6/s256 · b1/d15/L7/s128 | 28.62 · 48.77 · 33.40 ns/point [28.3 · 48.5 · 33.3] |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 · prepare/equi/b1/d20/L6 | 20.03 ms [19.65] · 63.9 ms [65.7] |
| micro/quad/process_tg/cc/b0/d2/L16 vs. equi/b0/d2/L16 | 64.3 vs. 15.6 ns/point (CC weights per grid → WP-05) |
| micro/interp/global_tg/cc/b0/d5/L6 · cheb1/b0/d5/L4 | 108.7 · 194.4 ns/point [107.7 · 192.1] |
| micro/interp/barycentric_1d/generic/n65 vs. opt_cc/n65 | 580 vs. 3.1 ns/node [577 vs. 3.0] |
| micro/interplin/tg/equi/b1/d8/L5 | 167.8 ns/corner [169.3] |
| micro/mivec/downwards_closure/d10/S12/n1000 · d15/S10/n300 · d20/S7/n300 | 573 · 92 · 41 µs (WP-03-before: 12.7 · 15.0 · 15.8 s, `ab micro`) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | 15.8 ms [15.6] |
| micro/gen/sparse_grid/cc/b1/d20/L5 · complete_mis_with_coeffs/d20/L6 | 69.7 · 10.1 ms [68.4 · 9.4] |

### Findings → work packages (RP-3)

Re-ranking with expected savings: summary `summaries/RP-3-reprofiling.md` ("Profile shift /
follow-ups"). In short:
- The estimates of WP-04 (1.5–2.5 s), WP-17 (≈ 1–1.5 s), WP-06 (0.6–0.7 s), WP-08 (≈ 0.5 s),
  WP-21 (0.1–0.2 s) and WP-05 (0.5–0.8 s) stand.
- MI-set generation is finished as a runtime topic (0.04 % of the suite).
- WP-10 gets new decision data: the cold first call (+63…+130 ms per process).
- WP-32 / WP-33 keep their ranks.
- No new runtime hot spot.
