# Analysis — combigrid performance (initial profile = RP-0)

Setup session, 2026-10-22/24. Code: commit 84299eeca (unchanged). Judged configuration
(`perf.py build normal`), Ryzen 7 5825U, boost on, AC, idle; e2e at 16 threads, micro at
1 thread. Raw data: `baseline/` (protected). Later re-profilings append sections at the end.

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

  - e2e: per-round log-speedup std of `total` over 38 cases: median 0.67%, 90th pct 2.59%, max 7.62% (quad/sg/equi-exp/b0/d2/L20); top 3: interp/sg/cheb1/b0/d3/L6/p64 3.6%, interplin/sg/equi/b1/d2/L16/p16384 4.2%, quad/sg/equi-exp/b0/d2/L20 7.6%
    -> 95% CI half-width with 5 rounds at the median spread: ±0.84%, at the 90th pct: ±3.22%
  - micro: per-round log-speedup std of `call` over 29 cases: median 1.05%, 90th pct 2.66%, max 5.37% (micro/quad/process_tg/equi/b0/d10/L4); top 3: micro/dsg/prepare/equi/b1/d20/L6 2.7%, micro/interplin/tg/equi/b1/d8/L5 2.8%, micro/quad/process_tg/equi/b0/d10/L4 5.4%
    -> 95% CI half-width with 5 rounds at the median spread: ±1.30%, at the 90th pct: ±3.30%

- **Throttling under sustained load**: the 26-minute noise self-comparisons (2026-10-24, noon) ran
  1.6x slower at 16 threads than the same binaries two days earlier (past midnight); 1-thread runs
  were ~5 % slower, short runs and single-process runs (calibration, baseline recording after an
  idle period) were not slower. Foreign CPU load (now measured per round) was < 0.5 %, settings
  were identical: the laptop throttles under sustained full load, more strongly in warmer
  conditions; a 3-minute A/B lost ~8 % after its first round. A and B are interleaved per case,
  so comparisons stay valid; the noise model is relative and robust against the throttling onset.
  Consequences: `perf.py` reports foreign CPU load and round durations in every A/B report, and
  `perf.py calibrate` compares burst and sustained throughput with `baseline/calibration.json`.
- Reference check noise: reassociation across 1–16 threads moves results by at most 1.54
  round-off units (README, "Reference check"); the tolerance allows 4 units plus 10 % error growth.

## Where the time goes

Baseline e2e (16 threads; medians of 5 samples; `ns/point` = operator time per evaluated grid
point, `thread·ns/point` = the same times 16; for `interp` the grid points are counted once per
query point):

| case | total | generation | operator | nTG | grid points | ns/point (16 thr) | thread·ns/point | rel. error |
|---|---|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 417.9 ms | 21.0 ms | 387.4 ms | 41 | 1.43e+08 | 2.72 | 43 | 3.0e-12 |
| quad/sg/equi-exp/b0/d5/L10 | 408.1 ms | 966 µs | 406.2 ms | 2751 | 1.4e+08 | 2.90 | 46 | 4.1e-13 |
| quad/sg/equi-exp/b0/d7/L7 | 937.2 ms | 953 µs | 935.8 ms | 3431 | 2.64e+08 | 3.54 | 57 | 6.0e-11 |
| quad/sg/equi-exp/b0/d10/L4 | 2.02 s | 1.4 ms | 2.02 s | 1001 | 4.26e+08 | 4.73 | 76 | 3.6e-09 |
| quad/sg/equi-exp/b1/d10/L7 | 657.6 ms | 1.2 ms | 654.3 ms | 19448 | 1.21e+08 | 5.42 | 87 | 2.9e-10 |
| quad/sg/equi-exp/b1/d15/L6 | 1.43 s | 6.2 ms | 1.42 s | 54264 | 2.13e+08 | 6.67 | 107 | 1.4e-09 |
| quad/sg/equi-exp/b1/d20/L5 | 669.8 ms | 6.9 ms | 656.6 ms | 53130 | 7.72e+07 | 8.50 | 136 | 3.9e-08 |
| quad/sg/equi-sin/b0/d7/L7 | 2.17 s | 1.1 ms | 2.17 s | 3431 | 2.64e+08 | 8.20 | 131 | 8.5e-12 |
| quad/sg/cc-uq/b0/d2/L20 | 977.1 ms | 45.9 ms | 925.9 ms | 41 | 1.43e+08 | 6.49 | 104 | 9.9e-12 |
| quad/sg/cc-uq/b0/d3/L14 | 108.5 ms | 1.3 ms | 107.1 ms | 316 | 3.55e+07 | 3.02 | 48 | 1.2e-12 |
| quad/sg/cc-uq/b0/d5/L10 | 484.6 ms | 1.5 ms | 482.8 ms | 2751 | 1.4e+08 | 3.44 | 55 | 6.5e-12 |
| quad/sg/cc-uq/b0/d7/L7 | 1.12 s | 1.2 ms | 1.12 s | 3431 | 2.64e+08 | 4.23 | 68 | 6.2e-12 |
| quad/sg/cc-uq/b0/d10/L4 | 2.24 s | 1.5 ms | 2.24 s | 1001 | 4.26e+08 | 5.25 | 84 | 1.2e-10 |
| quad/dsg/equi-exp/b0/d3/L16 | 57.1 ms | 634 µs | 56.2 ms | 969 | 3.21e+07 | 1.75 | 28 | 1.1e-12 |
| quad/dsg/equi-exp/b0/d5/L12 | 136.7 ms | 643 µs | 135.2 ms | 6188 | 7.7e+07 | 1.76 | 28 | 3.6e-12 |
| quad/dsg/equi-exp/b0/d7/L10 | 768.2 ms | 1.3 ms | 763.9 ms | 19448 | 4.19e+08 | 1.82 | 29 | 2.1e-11 |
| quad/dsg/equi-exp/b0/d10/L7 | 3.53 s | 1.3 ms | 3.52 s | 19448 | 1.83e+09 | 1.92 | 31 | 1.1e-10 |
| quad/dsg/equi-exp/b1/d15/L7 | 2.52 s | 32.3 ms | 2.47 s | 170544 | 3.54e+08 | 6.96 | 111 | 8.5e-10 |
| quad/dsg/equi-exp/b1/d20/L6 | 4.30 s | 47.8 ms | 4.23 s | 230230 | 2.81e+08 | 15.07 | 241 | 6.3e-09 |
| quad/dsg/cc-uq/b0/d5/L12 | 165.7 ms | 1.4 ms | 163.6 ms | 6188 | 7.7e+07 | 2.12 | 34 | 1.2e-13 |
| quad/dsg/cc-uq/b0/d7/L10 | 951.7 ms | 1.8 ms | 946.7 ms | 19448 | 4.19e+08 | 2.26 | 36 | 2.3e-12 |
| quad/dsg/cc-uq/b0/d10/L7 | 4.39 s | 1.6 ms | 4.39 s | 19448 | 1.83e+09 | 2.40 | 38 | 2.8e-11 |
| interp/sg/cc/b0/d2/L12/p64 | 133.8 ms | 123 µs | 133.7 ms | 25 | 3.6e+05 | 5.80 | 93 | 2.0e-15 |
| interp/sg/cc/b0/d3/L9/p64 | 191.0 ms | 49 µs | 190.8 ms | 136 | 5.57e+05 | 5.35 | 86 | 1.1e-16 |
| interp/sg/cc/b0/d5/L6/p64 | 1.11 s | 70 µs | 1.11 s | 456 | 2.2e+06 | 7.84 | 125 | 7.1e-08 |
| interp/sg/cc/b0/d7/L4/p16 | 927.6 ms | 51 µs | 927.5 ms | 330 | 5.06e+06 | 11.47 | 183 | 1.3e-04 |
| interp/sg/cc/b0/d10/L2/p8 | 1.38 s | 31 µs | 1.38 s | 66 | 1.02e+07 | 16.98 | 272 | 3.5e-02 |
| interp/sg/cheb1/b0/d3/L6/p64 | 101.2 ms | 16 µs | 101.2 ms | 64 | 3.89e+04 | 40.68 | 651 | 1.9e-13 |
| interp/sg/cheb1/b0/d5/L4/p64 | 224.4 ms | 22 µs | 224.3 ms | 126 | 2.06e+05 | 16.98 | 272 | 2.3e-04 |
| interp/sg/equi/b0/d3/L5/p64 | 28.8 ms | 12 µs | 28.8 ms | 46 | 1.52e+04 | 29.59 | 473 | 3.5e-02 |
| interplin/sg/equi/b1/d2/L16/p16384 | 58.1 ms | 288 µs | 55.3 ms | 33 | 6.55e+06 | nan | nan | 1.9e-10 |
| interplin/sg/equi/b1/d5/L8/p1024 | 318.0 ms | 1.4 ms | 316.2 ms | 1231 | 3.72e+06 | nan | nan | 1.7e-04 |
| interplin/sg/equi/b1/d8/L5/p256 | 1.06 s | 1.5 ms | 1.06 s | 1287 | 7.47e+05 | nan | nan | 3.2e-02 |
| interplin/sg/equi/b1/d10/L5/p32 | 1.70 s | 1.1 ms | 1.70 s | 3003 | 2.3e+06 | nan | nan | 4.6e-02 |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 1.40 s | 2.6 ms | 1.40 s | 777 | 4.13e+08 | 3.39 | 54 | 9.1e-13 |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1.92 s | 791.1 ms | 1.12 s | 2724 | 1.68e+08 | 6.66 | 107 | 1.2e-08 |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.10 s | 1.04 s | 59.2 ms | 516 | 6.08e+06 | 9.74 | 156 | 1.9e-03 |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 1.12 s | 1.12 s | 5.2 ms | 353 | 4.51e+05 | 11.58 | 185 | 3.7e-02 |

| group | sum total | sum operator | sum total (d ≥ 7) |
|---|---|---|---|
| quad/sg | 13.64 s | 13.51 s | 11.24 s |
| quad/dsg | 16.82 s | 16.67 s | 16.46 s |
| interp/sg | 4.10 s | 4.10 s | 2.31 s |
| interplin/sg | 3.13 s | 3.13 s | 2.76 s |
| mivec/sg | 5.54 s | 2.58 s | 4.14 s |

Observations:
- **Grid generation is negligible** (< 1 % of `total`) for complete sparse grids, including
  d = 20 with 53 130 tensor grids. Exception: `MIVecSGGenInstr` at d ≥ 10, where the downwards
  closure dominates (0.8–1.1 s of 1.1–1.8 s).
- **The operators dominate.** With the cheap `exp` integrand, complete-SG quadrature costs
  43–76 thread·ns per grid point for b0 (d = 2…10) and 87–136 for the many small grids of b1
  (d = 10…20), versus 28–38 for large delta grids of the deltas quadrature (same integrand): the
  per-point and per-grid overhead of the complete-SG operator is the largest single potential.
- DSG quadrature evaluates every grid point once and costs ~30 thread·ns per point for large
  delta grids (the pre-existing benchmarks d5 L10 show an order of magnitude over SG quadrature),
  but its b1 cases at d = 15/20 cost 110–240 thread·ns per point: most time is spent outside the
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
| `quad/sg/cc-uq/b0/d2/L20` | Eigen FFT (`kiss_cpx_fft`) 14 % + `genWeightsFFT` 4 % · `processTensorGrid` 11 % · `denormalizeDataVector` 8 % · integrand + `exp` 14 % · `getWeightForGP` 5 % |
| `quad/dsg/equi-exp/b0/d10/L7` | `processDeltaTensorGrid` (inlined row loop) 41 % · integrand + `exp` 47 % · `std::function`/`SourceFunc::evaluate` 4 % · `memset` 2 % |
| `quad/dsg/equi-exp/b1/d20/L6` (230 230 delta grids) | `appendSubTrieChildren` (sub-trie per delta grid) **63 %** · `processDeltaTensorGrid` 23 % · integrand 7 % |
| `interp/sg/cc/b0/d5/L6/p64` | `cos` + integrand 52 % · `denormalizeDataVector` 12 % · `TensorGrid::getGridPoint` 11 % · `hasTrivialResult` 3 % · `interpolateFirstDim` 2 % |
| `interp/sg/cheb1/b0/d5/L4/p64` | barycentric weights (`barycentricFormulaWeights` + `log`) 17 % · `cos` + integrand 21 % · libgomp spin ≥ 15 % · `denormalizeDataVector` 5 % · `getGridPoint` 4 % |
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
| micro/quad/process_tg/equi/b0/d2/L16 | 6.6 ms | 16.66 ns | 3.932e+05 |
| micro/quad/process_tg/equi/b0/d5/L10 | 3.8 ms | 23.03 ns | 1.66e+05 |
| micro/quad/process_tg/equi/b0/d10/L4 | 23.4 ms | 36.00 ns | 6.495e+05 |
| micro/quad/process_tg/cc/b0/d2/L16 | 15.5 ms | 39.36 ns | 3.932e+05 |
| micro/quad/process_tg/cc/b0/d10/L4 | 23.5 ms | 36.24 ns | 6.495e+05 |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | 259.5 ms | 64.70 ns | 4.011e+06 |
| micro/sol/quad_tg/equi/b0/d5/L10 | 1.7 ms | 10.34 ns | 1.66e+05 |
| micro/sol/quad_tg/equi/b0/d10/L4 | 7.8 ms | 12.04 ns | 6.495e+05 |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | 3.9 ms | 5.93 ns | 6.495e+05 |
| micro/sourcefunc/eval_normalized_in_place/d10 | 27 µs | 26.80 ns | 1024 |
| micro/quadrule/cc_weights/n33 | 1 µs | 31.56 ns | 33 |
| micro/quadrule/cc_weights/n1025 | 24 µs | 23.80 ns | 1025 |
| micro/quadrule/cc_weights/n32769 | 1.0 ms | 31.91 ns | 3.277e+04 |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | 3.9 ms | 11.68 ns | 3.318e+05 |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | 32.6 ms | 12.94 ns | 2.519e+06 |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | 1.3 ms | 318.95 ns | 4096 |
| micro/dsg/prepare/equi/b1/d20/L6 | 197.1 ms | 855.89 ns | 2.302e+05 |
| micro/interp/global_tg/cc/b0/d5/L6 | 616 µs | 58.97 ns | 1.045e+04 |
| micro/interp/global_tg/cheb1/b0/d5/L4 | 265 µs | 98.98 ns | 2673 |
| micro/interp/barycentric_1d/generic/n9 | 0 µs | 44.13 ns | 9 |
| micro/interp/barycentric_1d/generic/n65 | 18 µs | 272.66 ns | 65 |
| micro/interp/barycentric_1d/opt_cc/n9 | 0 µs | 3.00 ns | 9 |
| micro/interp/barycentric_1d/opt_cc/n65 | 0 µs | 2.82 ns | 65 |
| micro/interplin/tg/equi/b1/d8/L5 | 22 µs | 85.67 ns | 256 |
| micro/gen/complete_mis_with_coeffs/d20/L6 | 8.5 ms | 37.11 ns | 2.302e+05 |
| micro/gen/sparse_grid/cc/b1/d20/L5 | 39.4 ms | 741.10 ns | 5.313e+04 |
| micro/mivec/downwards_closure/d10/S10/n300 | 270.3 ms | 76736.91 ns | 3522 |
| micro/mivec/downwards_closure/d20/S6/n100 | 8.8 ms | 30231.63 ns | 291 |
| micro/mivec/ct_coeffs/d10/S12/n1000 | 7.0 ms | 485.80 ns | 1.438e+04 |

Key ratios:
- Complete-SG tensor-grid quadrature vs. speed of light: d = 10: 36.0 vs. 12.0 ns/point (3.0x),
  d = 5: 23.0 vs. 10.3 ns/point (2.2x).
- Many small tensor grids (b1, d = 20, 10 626 grids): 64.7 ns/point vs. 36 ns/point in a large
  grid — per-grid costs (weights, iterator allocations) matter in this regime.
- DSG: 13 ns/point for a large delta grid (d = 10) vs. 319 ns/point for a typical tiny delta grid
  at b1 d = 20 (sub-trie construction per delta grid); the serial preparation (1D weights + trie)
  takes 0.19 s at b1 d = 20 L6.
- Generic barycentric formula: 273 ns per node and call (O(n²) weights recomputed on every call)
  vs. 2.8 ns for the closed-form CC formula (~95x).
- Downwards closure: 270 ms single-threaded for a closure of 3 522 multi-indices (d = 10, 300
  input MIs), i.e. 77 µs per closure element; in the e2e suite 0.8–1.1 s of generation at
  16 threads for d = 10–20.

## Thread scaling

`operator` time, baseline snapshot, 1–16 threads:

| case | 1 thread | 4 | 8 | 16 | speedup 16 vs 1 | speedup 8 vs 1 |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 2.28 s | 640.2 ms | 393.6 ms | 415.4 ms | 5.5x | 5.8x |
| quad/sg/equi-exp/b0/d5/L10 | 3.10 s | 803.1 ms | 447.2 ms | 440.1 ms | 7.1x | 6.9x |
| quad/sg/equi-exp/b1/d15/L6 | 10.60 s | 2.87 s | 1.60 s | 1.48 s | 7.2x | 6.6x |
| quad/dsg/equi-exp/b0/d7/L10 | 5.39 s | 1.46 s | 823.6 ms | 765.3 ms | 7.0x | 6.5x |
| quad/dsg/equi-exp/b1/d15/L7 | 16.08 s | 4.67 s | 2.66 s | 2.49 s | 6.5x | 6.1x |
| interp/sg/cc/b0/d5/L6/p64 | 7.98 s | 2.31 s | 1.29 s | 1.10 s | 7.3x | 6.2x |
| interplin/sg/equi/b1/d5/L8/p1024 | 2.24 s | 659.6 ms | 370.4 ms | 316.0 ms | 7.1x | 6.1x |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 5.40 s | 1.80 s | 1.04 s | 1.13 s | 4.8x | 5.2x |

- **SMT adds almost nothing**: 16 vs. 8 threads gains 0–10 %; `quad/sg/.../d2/L20` and
  `mivec/.../d10` even get slower. 8 threads reach 6–7x of 1 thread (8 cores; the all-core
  frequency is lower than the single-core boost). Performance is judged at 16 threads, so
  optimizations should not rely on SMT hiding latency.
- **Load imbalance at low d**: `quad/sg/equi-exp/b0/d2/L20` (41 tensor grids for 16 threads,
  sizes differ by orders of magnitude, no parallelism inside a tensor grid) scales 5.5x only.
- **Serial generation work**: `mivec/.../d10` scales 4.8x (closure generation).

## Hardware counters

`perf stat` (16 threads, whole process incl. warm-up):

| case | instructions | cycles | IPC (per hw thread) | instructions per grid point | cache misses |
|---|---|---|---|---|---|
| `quad/sg/equi-exp/b0/d10/L4` | 7.1e11 | 3.2e11 | 2.23 | ≈ 555 | 3.7e6 |
| `quad/dsg/equi-exp/b0/d10/L7` | 1.20e12 | 5.4e11 | 2.20 | ≈ 218 | 2.3e7 |
| `quad/dsg/equi-exp/b1/d20/L6` | 1.34e12 | 6.3e11 | 2.12 | ≈ 1590 | 1.25e9 |

(3 pipeline runs each: 1 warm-up + 2 samples; average clock under 16-thread load ≈ 3.1 GHz.)
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
