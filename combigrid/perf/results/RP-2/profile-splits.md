# RP-2 — derived profile splits (snapshot `rp-2` = 5215417ea)

Raw top lists: `profiles/*.txt` (`perf.py profile`, self time, 1 warm-up + 2 samples, 999 Hz;
`micro_dsg_prepare` 9999 Hz, 40 samples). Shares = % of all samples of the process (16 threads
unless stated); the integrand includes its libm calls, PLT stubs and
`std::_Function_handler::_M_invoke`.

Method (changed from RP-1): inlined helpers are attributed by the **inline chain** of every
sampled address (`perf script -F ip,sym,symoff,dso -G`, then `addr2line -a -i` on the
library of the snapshot). The innermost frame that lies in a known helper decides the category,
so STL code (`std::vector` accesses, `std::fill_n`, iterators) counts for the helper that inlined
it. RP-1 split by source line (`--sort srcline`) and kept such lines in "other", so its
sub-shares are not 1:1 comparable (its "other" 6.9 % of b0 d10 is redistributed here); the
per-symbol totals are comparable.

## quad/dsg

| case (rp-2 total) | row loop `forEachGPWithGlobalWeight` | `updateCoeffSums` / `compRowWeights` / row + advance / `std::function` call / lambda | projection builder (`addChildren` + `build` + slot helpers) | integrand | other |
|---|---|---|---|---|---|
| equi-exp/b0/d10/L7 (5.41 s) | 43.1 % [44.8] | 14.4 / 11.6 / 9.4 / 5.3 / 2.4 | < 0.3 % | 52.5 % [50.5] | memset 2.1 % |
| cc-uq/b0/d10/L7 (6.77 s) | 32.8 % [35.0] | 10.9 / 8.3 / 7.4 / 4.4 / 1.9 | < 0.3 % | 63.6 % [61.1] | memset 1.6 % |
| equi-exp/b0/d7/L10 (1.14 s) | 42.9 % | 14.2 / 11.1 / 9.3 / 8.3 (call + lambda) | 0.5 % | 48.5 % | memset 2.0 %, OpenBLAS 1.6 % |
| cc-uq/b0/d7/L10 (1.44 s) | 33.9 % | 11.4 / 8.7 / 7.4 / 6.5 (call + lambda) | < 0.3 % | 58.5 % | memset 1.6 %, OpenBLAS 1.4 % |
| equi-exp/b1/d20/L6 (1.23 s) | 21.3 % [23.3] | 4.8 / 5.9 / 5.4 / 3.5 / 1.7 | **13.6 %** (12.0 + 1.2 + 0.4) + memset 1.0 | 52.8 % [≈ 7] | libgomp 2.0 %, OpenBLAS 1.7 %, `prepareDeltaTensorGrid` 0.4 % |
| equi-exp/b1/d15/L7 (1.22 s) | 24.8 % [35.8] | 5.5 / 6.6 / 6.2 / 4.5 / 2.0 | **9.6 %** (8.3 + 0.9 + 0.4) + memset 1.0 | 55.6 % [≈ 13.5] | libgomp 1.9 %, OpenBLAS 1.8 % |

[RP-1 values in brackets.] d7 was not profiled in RP-1 (RP-1 assumed d10 shares): they match d10.

Not visible in the 16-thread samples (single-threaded phases):
- Serial preparation of the b1 operator (`comp1DWeights` + `CTCoeffTrie`):
  `micro/dsg/prepare/equi/b1/d20/L6` **65.7 ms** in `run micro` (WP-02's 113 ms was the
  0-warm-up `ab micro` figure) = 5.4 % of the d20 wall time.
  - 1-thread profile: `CTCoeffTrie` constructor 35.8 % of the samples = radix pass
    (`appendNodesOfDepth`) 23.0 + level copy from `TensorGridCTData` (`copyNonZeroCoeffTGs`)
    8.6 + `FlatNestedVec` growth 2.3 + `compMaxSuffixLvlSums` 1.9 + rest 0.3.
  - `comp1DWeights` 7.9 % (its max-level scan reads `tgData.mi[dim]` of every tensor grid).
  - memset/memmove 6.1 %, kernel (page faults of the fresh vectors) ≈ 11 %; the rest is the setup
    (DSG construction) of the micro case.
  - Cache misses (`perf record -e cache-misses`, same case): level copy 22.4 % and
    `comp1DWeights` 18.0 % of all misses, radix pass 11.4 %. The two walks over the
    tensor-grid list (pointer per `mi`) are ≈ 16.5 % of the samples ≈ 18 ms per call.
- `generation` (parallel, 16 threads): d20 61–63 ms, d15 43 ms warm; about half of it is
  page faults (`generation-page-faults.md`).

## quad/sg (symbols; unchanged since RP-1 within ≈ 1 point)

| case (rp-2 total) | integrand | `processTensorGrid` | other |
|---|---|---|---|
| equi-sin/b0/d7/L7 (2.42 s) | 91.1 % [91.6] | 6.4 % [6.3] | OpenBLAS 0.8 % |
| cc-uq/b0/d10/L4 (1.31 s) | 77.9 % [78.5] | 18.7 % [18.4] | OpenBLAS 1.4 % |
| equi-exp/b0/d10/L4 (0.94 s) | 71.1 % [70.9] | 24.9 % [25.2] | OpenBLAS 1.9 % |
| equi-exp/b1/d20/L5 (0.24 s) | 64.6 % [65.2] | 12.4 % [12.5] | libgomp ≈ 8 %, OpenBLAS 7.2 %; per-TG setup (`getWeights` 0.2 %) and generation symbols ≈ 1.5 % |
| cc-uq/b0/d2/L20 (1.19 s) | ≈ 35 % [≈ 31] | 5.5 % [5.5] | CC weights: kissfft 15.0 % + `genWeightsFFT` 3.7 + sincos/cos 5.1 ≈ 24 % (+ their page faults) [36 % incl. faults]; kernel 21.9 % [22.7]; `DataVector` zero-fill 2.1 %; OpenBLAS 5.4 %, libgomp 4.4 % |
| equi-exp/b0/d2/L20 (0.40 s) | 48.4 % [≈ 48] | 12.3 % [12.2] | kernel 17.9 % [18.1] (scratch page faults), `DataVector` zero-fill 2.7 %, OpenBLAS 8.9 %, libgomp 8.8 % |

## interp / interplin (symbols; unchanged since RP-1)

| case (rp-2 total) | integrand (`cos` + `cosProd` + PLT) | operator symbols | libgomp |
|---|---|---|---|
| interp/sg/cc/b0/d10/L2/p8 (1.99 s) | 71.0 % [70.9] | `interpolateFirstDim` 18.9 % [19.1], barycentric 3.6 % [3.5] | 3.6 % |
| interp/sg/cc/b0/d5/L6/p64 (1.48 s) | 66.8 % [66.6] | `interpolateFirstDim` 19.9 % [20.2], barycentric 5.8 % [5.8] | 3.8 % |
| interp/sg/cheb1/b0/d5/L4/p64 (0.34 s) | ≈ 25 % | generic weights `barycentricFormulaWeights` 9.3 + `log` 13.0 + `exp` 1.7 = 24.0 % [25.8] + `_mid_memalign` 2.7 %; `interpolateFirstDim` 7.6 % | **18.9 %** [21.3] |
| interplin/sg/equi/b1/d10/L5/p32 (2.52 s) | 66.4 % [67.7] | `computeNeighborGPWithWeight` 19.9 % [19.8], `interpolate` 7.6 % [7.3] | 3.2 % |
| interplin/sg/equi/b1/d8/L5/p256 (1.58 s) | 65.9 % [68.7] | `computeNeighborGPWithWeight` 16.5 % [16.6], `interpolate` 8.0 % [8.4] | 5.5 % |

## mivec

| case (rp-2 total / generation) | `genMIVecDownwardsClosure` | rest |
|---|---|---|
| d10/S12/n1000 (1.90 s / 1.41 s) | 77.5 % [78.6] | quadrature ≈ 18 % |
| d15/S10/n300 (1.89 s / 1.86 s) | 96.1 % [96.6] | OpenBLAS 1.0 % |
| d20/S7/n300 (1.83 s / 1.83 s) | 97.2 % [97.6] | OpenBLAS 1.0 % |

Hottest instructions of the closure loop (rp-2, d20, `perf annotate`): the 32-byte alignment test
`test $0x1f` 7.3 % (+ `or` 7.0 %), the AVX2 `vptest` early-break loop, the per-test index
multiply (`imul`) — the same structure as in RP-1. See `mivec-rp-1-vs-rp-2.md` for the
unchanged-source speed-up.

## SG generation (1 thread, `micro/gen/sparse_grid/cc/b1/d20/L5`, 68.4 ms, incl. destruction)

Node generation (`cos` 16.8 + `SecondTypeChebyshevNodeGenFunc::genNodesWithoutBoundary` 9.1 +
`getNodesPerDimForTG` 6.1 + `genNodesWithBoundary` 1.6 + `cos@plt` 1.8) ≈ 35 % (recomputed per
tensor grid: shared 1D arrays, WP-18, would compute each once); malloc/free/memalign ≈ 18 %;
`getGPCntPerDim` + `doublingLvl2GPCntFunction` 6.8 %; kernel ≈ 7 %; MI generation (`addBarPosAsMI`) 1.6 %.
