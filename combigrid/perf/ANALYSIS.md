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
