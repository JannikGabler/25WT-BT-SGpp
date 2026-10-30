# RP-3 — derived profile splits (snapshot `rp-3` = 0c6ecfc4b)

Raw top lists: `profiles/*-t16.txt` (`perf.py profile`, self time, 1 warm-up + 2 samples, 999 Hz,
16 threads). Inline-chain splits: `profiles/*-t16-split.txt`, made by `profile_split.py` (RP-2
method: `perf script -F ip,sym,symoff,dso -G --no-demangle`, address = `nm` address of the symbol
+ offset, `addr2line -a -i -f -C` on the snapshot's library; the innermost frame that is not STL /
`MI::operator[]` / `std::fill_n` decides the helper). Shares = % of all samples of the process.
The integrand includes its libm calls, PLT stubs and `std::_Function_handler::_M_invoke`.
"call" = the inlined `SourceFunc::evaluate` (indirect `std::function` call) + `_M_invoke`.
[RP-2 values in brackets.] Absolute times = share × rp-3 median `total` (`e2e.md`).

## quad/dsg

| case (rp-3 total) | row loop `forEachGPWithGlobalWeight` | `updateCoeffSums` / `compRowWeights` / row + advance + lambda / call (`evaluate` + `_M_invoke`) | projection builder | integrand | other |
|---|---|---|---|---|---|
| equi-exp/b0/d10/L7 (5.40 s) | 43.2 % [43.1] | 14.5 / 11.6 / 11.6 / 5.4 + 1.4 | < 0.3 % | 52.4 % [52.5] | memset 2.6 %, libgomp 0.4, OpenBLAS 0.4 |
| cc-uq/b0/d10/L7 (6.79 s) | 33.1 % [32.8] | 10.8 / 8.5 / 9.4 / 4.4 + 1.2 | < 0.3 % | 63.3 % [63.6] | memset 2.0 % |
| equi-exp/b0/d7/L10 (1.14 s) | 43.3 % [42.9] | 14.5 / 11.2 / 11.8 / 5.8 + 1.5 | 1.3 % | 48.2 % [48.5] | memset 2.4 %, libgomp 1.7, OpenBLAS 1.7 |
| cc-uq/b0/d7/L10 (1.44 s) | 33.8 % [33.9] | 11.5 / 8.3 / 9.2 / 4.8 + 1.2 | 1.1 % | 58.7 % [58.5] | memset 1.9 %, libgomp 1.8, OpenBLAS 1.5 |
| equi-exp/b1/d20/L6 (1.23 s) | 21.1 % [21.3] | 4.8 / 5.9 / 6.9 / 3.6 + 1.0 | **13.1 %** [13.6] (`addChildren` 11.9) + memset 1.2 | 51.9 % [52.8] | kernel 4.0, libgomp 2.8 [2.0], OpenBLAS 2.7 [1.7], `prepareDeltaTensorGrid` 0.4 |
| equi-exp/b1/d15/L7 (1.22 s) | 24.9 % [24.8] | 5.7 / 6.5 / 8.2 / 4.5 + 1.2 | **9.0 %** [9.6] (`addChildren` 8.3) + memset 1.2 | 55.2 % [55.6] | kernel 2.8, libgomp 2.6, OpenBLAS 2.1 |

Absolute (b0 d7/d10): row loop **5.55 s** [5.53], of which per-row trie work
(`updateCoeffSums` + `compRowWeights`) **3.30 s** [3.3] and dispatch **0.92 s** [0.9]. b1
d15 + d20: row loop 0.56 s, trie work 0.28 s, builder 0.27 s [≈ 0.31], dispatch 0.13 s [0.13].

## quad/sg (SG row loop = `processTensorGrid` with the inlined `sumOverRows`)

| case (rp-3 total) | integrand | `processTensorGrid` = rows + advance / call / `updateWeightProducts` | other |
|---|---|---|---|
| equi-sin/b0/d7/L7 (2.42 s) | 91.4 % [91.1] | 6.3 % [6.4] = 3.8 / 2.2 / 0.3 | OpenBLAS 0.9, libgomp 0.6 |
| cc-uq/b0/d10/L4 (1.31 s) | 77.7 % [77.9] | 18.6 % [18.7] = 11.8 / 5.3 / 1.3 | OpenBLAS 1.6, libgomp 0.9 |
| equi-exp/b0/d10/L4 (0.94 s) | 70.3 % [71.1] | 25.2 % [24.9] = 16.1 / 7.1 / 1.7 | OpenBLAS 2.2, libgomp 1.3 |
| equi-exp/b1/d20/L5 (0.23 s) | 66.8 % [64.6] | 12.7 % [12.4] = 7.9 / 3.9 / 0.6 (+ `getWeights` 0.3) | OpenBLAS 7.5, libgomp 6.6, kernel 4.1 |
| cc-uq/b0/d2/L20 (1.20 s) | 32.8 % [≈ 35] | 5.3 % [5.5] = 2.5 / 2.5 | CC weights: kissfft 14.9 + `genWeightsFFT` 3.8 + sincos/cos 5.0 + `cexp` 1.1 ≈ 24.8 % [≈ 24]; kernel 23.1 % [21.9]; `DataVector` zero-fill 2.4 % [2.1]; OpenBLAS 5.2, libgomp 4.6 |

Dispatch (call + `_M_invoke`) in the profiled cases 0.29 s of 6.10 s; the other quad/sg cases
(3.1 s, 5–9 %) add ≈ 0.15–0.28 s → quad/sg ≈ 0.45–0.55 s [≈ 0.4]; mivec d5/d10 0.11 s.

## interp / interplin

| case (rp-3 total) | integrand (`cos` + `cosProd` + PLT + `_M_invoke`) | operator symbols | libgomp |
|---|---|---|---|
| interp/sg/cc/b0/d10/L2/p8 (1.99 s) | 70.7 % [71.0] | `interpolateFirstDim` 19.1 % [18.9] (self 11.8, `getGridPoint` 5.6, call 1.6); barycentric 3.5 % [3.6] | 3.9 % |
| interp/sg/cc/b0/d7/L4/p16 (1.26 s, first profile) | 70.7 % | `interpolateFirstDim` 17.8 % (9.2 / 7.4 / 1.1); barycentric 4.8 % | 3.4 % |
| interp/sg/cc/b0/d5/L6/p64 (1.45 s) | 67.2 % [66.8] | `interpolateFirstDim` 20.0 % [19.9] (9.8 / 8.5 / 1.4); barycentric 6.0 % [5.8] | 4.1 % |
| interp/sg/cheb1/b0/d5/L4/p64 (0.34 s) | ≈ 24.4 % | generic weights `barycentricFormulaWeights` 9.8 + `log` 13.2 + `exp` 1.8 = 24.8 % [24.0] (+ `exp` wrapper and `log` stubs 3.1 %); allocations (`_mid_memalign`, `posix_memalign`, free, `operator new`) 7.2 %; `interpolateFirstDim` 7.3 %; `hasTrivialResult` 1.6 % | **21.3 %** [18.9] |
| interplin/sg/equi/b1/d10/L5/p32 (2.47 s) | 66.7 % [66.4] | `computeNeighborGPWithWeight` 20.5 % [19.9], `interpolate` 7.6 % [7.6] (call 0.7) | 2.5 % |
| interplin/sg/equi/b1/d8/L5/p256 (1.53 s) | 66.6 % [65.9] | `computeNeighborGPWithWeight` 17.6 % [16.5], `interpolate` 8.2 % [8.0] (call 0.8) | 4.7 % |

Absolute: `interpolateFirstDim` + barycentric at d ≥ 7 0.73 s (d5 0.38 s); corner loop
(`computeNeighborGPWithWeight` + `interpolate`) d10 + d8 1.09 s.

## mivec (operator-bound since WP-03)

| case (rp-3 total / generation) | integrand | `processTensorGrid` = rows / call | libgomp | OpenBLAS | generation symbols |
|---|---|---|---|---|---|
| d5/S16/n1000 (743 ms / 3.0 ms) | 76.9 % | 15.2 % = 7.4 / 7.7 | 3.6 % | 3.0 % | < 0.1 % |
| d10/S12/n1000 (484 ms / 7.5 ms) | 70.2 % | 11.8 % = 6.4 / 5.2 | 8.9 % | 6.4 % | `computeCTCoeffs` 0.2 % |
| d15/S10/n300 (27.5 ms / 2.4 ms) | 16.0 % | 1.8 % | **47.0 %** | **29.0 %** | `computeCTCoeffs` 0.6 % |
| d20/S7/n300 (3.1 ms / 0.8 ms) | 2.0 % | < 0.4 % | **50.6 %** | **39.7 %** | – |

The d15/d20 processes are dominated by spinning threads (libgomp after the parallel regions,
the OpenBLAS pool after its start); these samples fall mostly into the warm-up (cold first call,
`openblas-cold-start.md`), not into the measured sample. Generation split: `mivec-generation-split.md`.

## WP-29 "before" picture: the four kernels at the top cases

| kernel (WP-29 step) | case | kernel share | of which: per-point dispatch (call + `_M_invoke`) | integrand |
|---|---|---|---|---|
| SG row loop `processTensorGrid`/`sumOverRows` (a) | quad/sg equi-exp b0 d10 / cc-uq b0 d10 / equi-sin b0 d7 / equi-exp b1 d20 | 25.2 / 18.6 / 6.3 / 12.7 % | 9.0 / 6.7 / 2.7 / 5.1 % | 70.3 / 77.7 / 91.4 / 66.8 % |
| SG row loop in mivec (a) | mivec d5 / d10 | 15.2 / 11.8 % | 10.2 / 6.9 % | 76.9 / 70.2 % |
| DSG row loop `forEachGPWithGlobalWeight` + `processDeltaTensorGrid` lambda (b) | quad/dsg equi b0 d10 / cc b0 d10 / equi b0 d7 / cc b0 d7 | 43.2 / 33.1 / 43.3 / 33.8 % | 6.8 / 5.6 / 7.3 / 6.0 % | 52.4 / 63.3 / 48.2 / 58.7 % |
| DSG row loop (b) | quad/dsg b1 d20 / d15 | 21.1 / 24.9 % (+ builder 13.1 / 9.0 %) | 4.6 / 5.7 % | 51.9 / 55.2 % |
| `interpolateFirstDim` + barycentric formula (c) | interp cc d10 / d7 / d5 | 22.6 / 22.6 / 26.0 % | 2.5 / 1.1 / 1.8 % | 70.7 / 70.7 / 67.2 % |
| generic barycentric formula (c) | interp cheb1 d5 | weights 24.8 % + allocations 7.2 % + `interpolateFirstDim` 7.3 % | 0.6 % | ≈ 24 % |
| multilinear corner loop `computeNeighborGPWithWeight` + `interpolate` (c) | interplin d10 / d8 | 28.1 / 25.8 % | 0.7 / 0.8 % | 66.7 / 66.6 % |

Absolute kernel time (rp-3): SG row loop 0.73 s in the profiled quad/sg cases + 0.17 s in mivec
d5/d10; DSG row loop 5.55 s (b0 d7/d10) + 0.56 s (b1); `interpolateFirstDim` + barycentric
0.73 s (d ≥ 7) + 0.38 s (d5); corner loop 1.09 s (d8 + d10).

## Shared libraries (all 21 profiles)

- `libsgppbase.so`: ≤ 0.06 % at d ≥ 7 (WP-23: ≤ 0.09 %); 2.4 % at sg cc d2 (zero-fill of
  `DataVector` scratch → WP-05), 1.7 % at interp cheb1 d5 (`operator new` → WP-07).
- OpenBLAS: 0.3–2.2 % of the long cases, 5–8 % of the 0.23–1.2 s cases (sg b1 d20, sg cc d2,
  mivec d10), 29–40 % of mivec d15/d20; libgomp 0.4–6.6 %, 21.3 % at interp cheb1 d5 (few, short
  tensor grids per query point), 47–51 % at mivec d15/d20.
