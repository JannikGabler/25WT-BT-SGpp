# RP-1 — derived profile splits (snapshot `rp-1` = f48d2bbc1)

Raw top lists: `profiles/*.txt` (`perf.py profile`, self time, 1 warm-up + 2 samples, 999 Hz).
Since WP-13 (`-fno-semantic-interposition`) exported helpers are inlined, so their cost hides in
the caller's symbol. The splits below attribute the samples of one symbol to the inlined code:
`perf report --no-children --sort sym,srcfile` (by source file of the sample address) or
`--sort srcline` (line ranges mapped to the inlined helper). `perf script` callchains through libm
(no frame pointers) are unreliable and are not used for shares. Shares = % of all samples of the
process (16 threads unless stated); the integrand includes its libm calls, PLT stubs and
`std::_Function_handler::_M_invoke`.

## quad/dsg

| case (rp-1 total) | trie per delta grid: `appendSubTrieChildren` + `compSubTrie` | row loop `forEachGPWithGlobalWeight` (inlined) | of which `updateCoeffSums` / `compRowWeights` / row + advance / `std::function` call / other | integrand | other |
|---|---|---|---|---|---|
| equi-exp/b1/d20/L6 (7.04 s) | 64.8 + 0.9 = 65.7 % | 23.3 % | 13.3 / 2.3 / 4.7 / 0.6 / 2.4 | ≈ 7.0 % | memset 0.6 % |
| equi-exp/b1/d15/L7 (4.09 s) | 44.4 + 1.3 = 45.7 % | 35.8 % | 19.9 / 4.6 / 6.2 / 1.3 / 3.8 | ≈ 13.5 % | memset 1.2 % |
| equi-exp/b0/d10/L7 (5.66 s) | 0.6 % | 44.8 % | 15.0 / 9.1 / 9.2 / 4.6 / 6.9 (incl. lambda 2.3) | 50.5 % | memset 2.4 % |
| cc-uq/b0/d10/L7 (7.02 s) | 0.5 % | 35.0 % | 11.8 / 6.4 / 7.4 / 3.9 / 5.5 (incl. lambda 1.8) | 61.1 % | memset 1.9 % |

Serial preparation before the parallel region (`comp1DWeights` + `CTCoeffTrie`):
`micro/dsg/prepare/equi/b1/d20/L6` 292 ms single-threaded (4 % of the d20 wall time; invisible
in the 16-thread profile because no other thread runs).

## quad/sg

| case (rp-1 total) | integrand | `processTensorGrid` (= `sumOverRows` / `std::function` call / `updateWeightProducts`) | per-tensor-grid setup (`getWeights` incl. 1D rules, `mapNodesOntoDomain`, `setUpIterDims`) | `QuadratureScratch` construction (zero-fill + page faults, inclusive) | libgomp + OpenBLAS |
|---|---|---|---|---|---|
| equi-sin/b0/d7/L7 (2.41 s) | 91.6 % | 6.3 % (3.9 / 2.1 / 0.3) | < 0.1 % | – | 1.5 % |
| cc-uq/b0/d10/L4 (1.31 s) | 78.5 % | 18.4 % (11.7 / 5.5 / 1.3) | < 0.1 % | – | 2.3 % |
| equi-exp/b0/d10/L4 (0.94 s) | 70.9 % | 25.2 % (16.1 / 7.4 / 1.7) | < 0.1 % | – | 3.4 % |
| equi-exp/b1/d15/L6 (0.53 s) | 73.1 % | 15.4 % (9.7 / 4.7 / 0.7) | ≈ 0.3 % | – | 8.9 % |
| equi-exp/b1/d20/L5 (0.24 s) | 65.2 % | 12.5 % (7.9 / 3.6 / 0.7) | ≈ 0.6 % (generation `genTGForMI` 2.4 %) | ≤ 1.3 % | 17.4 % |
| — same, 1 thread | 80.6 % | 15.4 % | < 1 % | ≤ 0.7 % | – |
| cc-uq/b0/d2/L20 (1.19 s) | ≈ 31 % | 5.5 % (2.5 / 2.5 / 0.1) | **CC weights 36 %** (Eigen kissfft 14.5, `genWeightsFFT` 3.9, twiddles sincos/cos/cexp 6.2, FFT buffer page faults) | **10.7 %** | 9.8 % |
| — same, 1 thread | ≈ 42 % | 7.0 % | **CC weights 44 %** | 0.6 % | – |
| equi-exp/b0/d2/L20 (0.40 s) | ≈ 48 % | 12.2 % (5.9 / 5.5 / 0.2) | ≈ 1 % | **17.8 %** (kernel page faults 12.4, `DataVector(n, 0.0)` 3.3) | 17.4 % |

Kernel samples (`[unknown] [k]`) of the d2/L20 cases: 18.1 % (equi) / 22.7 % (cc). By user-space
caller: `QuadratureScratch` constructor (both DataVectors, quadrature_scratch.hpp:45/47) 12.4 % /
7.2 %; CC weight buffers (`genWeightsFFT` vectors, Eigen twiddles/plan) ≈ 7 % (cc); tensor grid
node generation 1.2 %. `perf stat`: 4.0e5 / 1.1e6 page faults over 3 pipelines (equi: ≈ 525 MiB
re-faulted per `quadrature()` call ≈ 16 × 2 × 16.8 MiB).

## interp / interplin

| case (rp-1 total) | integrand | `interpolateFirstDim` split: `denormalizeDataVector` (normalization.cpp) / `getGridPoint` (tensor_grid.cpp) / vector access / `std::function` / own | barycentric (`OptBarycentricFormula` resp. generic: `barycentricFormulaWeights` + `log` + `exp`) | `hasTrivialResult` + `nearlyEqual` | allocation | libgomp | OpenBLAS |
|---|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d10/L2/p8 (1.99 s) | 70.9 % | 19.1 % = 8.4 / 5.0 / 3.1 / 1.6 / 1.1 | 3.5 % | 1.3 % | – | 3.9 % | 1.0 % |
| interp/sg/cc/b0/d5/L6/p64 (1.45 s) | 66.6 % | 20.2 % = 6.8 / 7.8 / 2.8 / 1.4 / 1.4 | 5.8 % | 2.4 % | – | 4.8 % | 1.4 % |
| interp/sg/cheb1/b0/d5/L4/p64 (0.34 s) | ≈ 25 % | 7.7 % = 2.1 / 3.4 / 1.0 / 0.5 / 0.7 | 9.0 + 13.9 + 2.9 = 25.8 % | 1.5 % | 5.5 % (memalign/free) | **21.3 %** | 5.8 % |
| — same, 1 thread | ≈ 37 % | 11.0 % | 12.2 + 18.0 + 4.1 = 34.3 % | 2.5 % | 8.2 % | – | – |
| interplin/sg/equi/b1/d10/L5/p32 (2.47 s) | 67.7 % | `computeNeighborGPWithWeight` 19.8 % (17.9 + vector 1.8); `interpolate` 7.3 % = denormalize 4.3 / vector 1.7 / own 0.7 / `std::function` 0.6 | – | – | – | 2.9 % | 0.9 % |
| interplin/sg/equi/b1/d8/L5/p256 (1.50 s) | 68.7 % | `computeNeighborGPWithWeight` 16.6 %; `interpolate` 8.4 % = denormalize 5.0 / vector 1.5 / own 1.0 / `std::function` 0.9 | – | – | – | 3.8 % | 1.4 % |

## mivec generation

| case (rp-1 total / generation) | `genMIVecDownwardsClosure` | quadrature (operator) | other |
|---|---|---|---|
| d10/S12/n1000 (1.99 s / 1.51 s) | 78.6 % | ≈ 17 % | libgomp 1.5 %, OpenBLAS 1.2 % |
| d15/S10/n300 (2.10 s / 2.07 s) | 96.6 % | < 1 % | libgomp/OpenBLAS 2 % |
| d20/S7/n300 (2.07 s / 2.07 s) | 97.6 % | < 0.1 % | libgomp/OpenBLAS 1.6 % |

Old master (3ffe4d382, `report-master`) vs. `rp-1` (`mivec-master-vs-rp-1.md`,
`perfstat-mivec-master-vs-rp-1.txt`, `profiles/*report-master*`, `*baseline*`):

| | master | baseline (fed2cdb4e) | rp-1 |
|---|---|---|---|
| generation d10 / d15 / d20 | 1.03 / 1.55 / 1.52 s | 1.37 / 1.79 / 1.90 s | 1.51 / 2.07 / 2.07 s |
| d20: instructions, IPC (perf stat, 3 pipelines) | 3.07e11, 2.11 | – | 3.53e11, 1.76 |
| d15: instructions, IPC | 3.88e11, 2.48 | – | 3.53e11, 1.75 |
| dominance test in the closure loop | scalar early-exit compare `miDominatesMI(miVec, idx, std::vector)` via `miVec(idx, dim)`, unrolled ×8, no prologue; + 4.5 % malloc/free (a `std::vector` copy per box element) | AVX2 early-break loop, hottest instruction the 32-byte alignment test `and $0x1f` (10.4 %) | same AVX2 loop (`vpcmpgtd`/`vptest`) with per-call prologue (index multiply, alignment test of both operands, remainder dispatch); 24 % of the function's samples in the scalar fallback for misaligned operands |

Cause: `tools::miVecDominatesMI(miVec, paretoMaxima, MIView)` → `operator>=` →
`tools::mi_view::lessEqualInEveryDim` (with `checkSameSize`), inlined into
`genMIVecDownwardsClosure`. GCC 15 vectorizes the early-exit loop; per test the loop exits after
few components, so the vector prologue/epilogue and the alignment dispatch cost more than they
save. Present since the MIView refactor (baseline), amplified by WP-14 (strict aliasing).
