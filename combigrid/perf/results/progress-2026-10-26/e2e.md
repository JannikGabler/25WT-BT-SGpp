# e2e progress: thesis state → after WP-13

End-to-end pipelines of `perf_e2e` (same cases as [WP-13/e2e.md](../WP-13/e2e.md)): L3 `total`
and L2 phases `operator` / `generation`, 16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`),
boost off, `performance` governor, AC. Medians; speedup = first state that has the case / after
WP-13 (> 1: faster now).

| column | code state | build | measurement |
|---|---|---|---|
| master (thesis) | `master` 3ffe4d382 (2026-05-03, end of the bachelor thesis) | its own site_scons as at that time (`-O3`, `-std=c++11` set last, **no `-DNDEBUG`**: asserts active) + the `compileNormal.sh` flags (`-march=native -flto=auto`, `ARCH=avx2`, `USE_EIGEN=1`; `USE_ARMADILLO=1` is rejected by this SConstruct) | 2026-10-26, 2 processes per case (each 1 warm-up + 1 sample), median |
| e6a21c331 (first DSG) | `dev` e6a21c331 (2026-10-18, first deltas-sparse-grid quadrature; 66 commits after master) | its own site_scons (`-O3 -DNDEBUG`, C++23) + `compileNormal.sh` flags + `USE_EIGEN=1 USE_ARMADILLO=1` | 2026-10-26, 2 processes per case (each 1 warm-up + 1 sample), median; DSG cases: 1 process, 1 sample, no warm-up, 45 s timeout |
| baseline (before WP-01) | 84299eeca (2026-10-21; setup commit fed2cdb4e, 7 commits after e6a21c331) | judged configuration (`perf.py build normal`), without `-fno-semantic-interposition` | `baseline/e2e_t16.jsonl` (recorded 2026-10-24, boost off), 1 process (1 warm-up + 5 samples) |
| after WP-13 | a16733d65 (WP-01 + WP-13 accepted) | judged configuration incl. `-fno-semantic-interposition` | column B of [WP-13/e2e.md](../WP-13/e2e.md) (2026-10-26, 5 interleaved processes × (1 warm-up + 1 sample)) |

| case | metric | master (thesis) | e6a21c331 (first DSG) | baseline (before WP-01) | after WP-13 | speedup (first → WP-13) |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 1.363s | 666.05ms | 732.26ms | 407.54ms | 3.35x |
| quad/sg/equi-exp/b0/d2/L20 | operator | 1.281s | 633.09ms | 693.35ms | 333.19ms | 3.84x |
| quad/sg/equi-exp/b0/d2/L20 | generation | 70.40ms | 29.67ms | 29.70ms | 32.53ms | 2.16x |
| quad/sg/equi-exp/b0/d5/L10 | total | 1.019s | 712.73ms | 710.14ms | 249.89ms | 4.08x |
| quad/sg/equi-exp/b0/d5/L10 | operator | 1.017s | 710.10ms | 707.70ms | 247.52ms | 4.11x |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.34ms | 2.05ms | 1.91ms | 2.04ms | 0.66x |
| quad/sg/equi-exp/b0/d7/L7 | total | 2.296s | 1.654s | 1.613s | 500.36ms | 4.59x |
| quad/sg/equi-exp/b0/d7/L7 | operator | 2.294s | 1.652s | 1.610s | 497.93ms | 4.61x |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.24ms | 1.67ms | 1.72ms | 1.79ms | 0.69x |
| quad/sg/equi-exp/b0/d10/L4 | total | 4.419s | 3.437s | 3.318s | 910.71ms | 4.85x |
| quad/sg/equi-exp/b0/d10/L4 | operator | 4.417s | 3.431s | 3.315s | 908.50ms | 4.86x |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.60ms | 5.13ms | 2.38ms | 2.17ms | 0.74x |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.554s | 1.137s | 1.119s | 301.21ms | 5.16x |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.548s | 1.128s | 1.113s | 292.71ms | 5.29x |
| quad/sg/equi-exp/b1/d10/L7 | generation | 2.52ms | 4.14ms | 2.29ms | 5.40ms | 0.47x |
| quad/sg/equi-exp/b1/d15/L6 | total | 3.223s | 2.480s | 2.393s | 520.71ms | 6.19x |
| quad/sg/equi-exp/b1/d15/L6 | operator | 3.206s | 2.464s | 2.377s | 496.49ms | 6.46x |
| quad/sg/equi-exp/b1/d15/L6 | generation | 8.09ms | 7.29ms | 7.27ms | 13.92ms | 0.58x |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.536s | 1.101s | 1.123s | 231.13ms | 6.65x |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.518s | 1.081s | 1.105s | 205.31ms | 7.39x |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.96ms | 10.67ms | 8.52ms | 18.29ms | 0.49x |
| quad/sg/equi-sin/b0/d7/L7 | total | 4.310s | 3.536s | 3.524s | 2.416s | 1.78x |
| quad/sg/equi-sin/b0/d7/L7 | operator | 4.305s | 3.533s | 3.522s | 2.414s | 1.78x |
| quad/sg/equi-sin/b0/d7/L7 | generation | 4.24ms | 1.72ms | 1.77ms | 1.86ms | 2.28x |
| quad/sg/cc-uq/b0/d2/L20 | total | ≥ 42 min (est.) | ≥ 42 min (est.) | 1.365s | 1.212s | > 2079x |
| quad/sg/cc-uq/b0/d2/L20 | operator | ≥ 42 min (est.) | ≥ 42 min (est.) | 1.283s | 1.140s | > 2210x |
| quad/sg/cc-uq/b0/d2/L20 | generation | n/m | n/m | 77.56ms | 82.03ms | - |
| quad/sg/cc-uq/b0/d3/L14 | total | 1.142s | 1.100s | 179.48ms | 86.67ms | 13.17x |
| quad/sg/cc-uq/b0/d3/L14 | operator | 1.139s | 1.097s | 177.28ms | 83.96ms | 13.57x |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.12ms | 2.06ms | 2.08ms | 2.08ms | 1.02x |
| quad/sg/cc-uq/b0/d5/L10 | total | 1.071s | 786.87ms | 780.35ms | 328.59ms | 3.26x |
| quad/sg/cc-uq/b0/d5/L10 | operator | 1.069s | 784.06ms | 777.35ms | 325.72ms | 3.28x |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.84ms | 2.19ms | 2.37ms | 2.31ms | 0.80x |
| quad/sg/cc-uq/b0/d7/L7 | total | 2.451s | 1.815s | 1.808s | 700.15ms | 3.50x |
| quad/sg/cc-uq/b0/d7/L7 | operator | 2.448s | 1.812s | 1.805s | 697.64ms | 3.51x |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.39ms | 1.87ms | 1.88ms | 1.92ms | 0.73x |
| quad/sg/cc-uq/b0/d10/L4 | total | 4.663s | 3.616s | 3.616s | 1.317s | 3.54x |
| quad/sg/cc-uq/b0/d10/L4 | operator | 4.661s | 3.614s | 3.614s | 1.315s | 3.54x |
| quad/sg/cc-uq/b0/d10/L4 | generation | 1.70ms | 2.31ms | 2.43ms | 2.34ms | 0.72x |
| quad/dsg/equi-exp/b0/d3/L16 | total | - | 21.268s | 93.66ms | 97.68ms | 217.73x |
| quad/dsg/equi-exp/b0/d3/L16 | operator | - | 21.229s | 92.53ms | 96.41ms | 220.19x |
| quad/dsg/equi-exp/b0/d3/L16 | generation | - | 37.71ms | 747.92us | 1.05ms | 35.91x |
| quad/dsg/equi-exp/b0/d5/L12 | total | - | > 45 s (timeout) | 221.23ms | 210.62ms | > 214x |
| quad/dsg/equi-exp/b0/d5/L12 | operator | - | > 45 s | 218.98ms | 208.05ms | > 216x |
| quad/dsg/equi-exp/b0/d5/L12 | generation | - | n/m | 1.01ms | 1.30ms | - |
| quad/dsg/equi-exp/b0/d7/L10 | total | - | > 4.3 min | 1.231s | 1.189s | > 219x |
| quad/dsg/equi-exp/b0/d7/L10 | operator | - | > 4.3 min | 1.224s | 1.181s | > 220x |
| quad/dsg/equi-exp/b0/d7/L10 | generation | - | n/m | 2.34ms | 3.06ms | - |
| quad/dsg/equi-exp/b0/d10/L7 | total | - | > 45 s (timeout) | 5.663s | 5.526s | > 8.1x |
| quad/dsg/equi-exp/b0/d10/L7 | operator | - | > 45 s | 5.656s | 5.516s | > 8.2x |
| quad/dsg/equi-exp/b0/d10/L7 | generation | - | n/m | 2.56ms | 4.15ms | - |
| quad/dsg/equi-exp/b1/d15/L7 | total | - | > 45 s (timeout) | 3.979s | 3.897s | > 12x |
| quad/dsg/equi-exp/b1/d15/L7 | operator | - | > 45 s | 3.895s | 3.838s | > 12x |
| quad/dsg/equi-exp/b1/d15/L7 | generation | - | n/m | 52.03ms | 35.67ms | - |
| quad/dsg/equi-exp/b1/d20/L6 | total | - | > 45 s (timeout) | 6.781s | 6.678s | > 6.7x |
| quad/dsg/equi-exp/b1/d20/L6 | operator | - | > 45 s | 6.659s | 6.587s | > 6.8x |
| quad/dsg/equi-exp/b1/d20/L6 | generation | - | n/m | 78.27ms | 50.71ms | - |
| quad/dsg/cc-uq/b0/d5/L12 | total | - | > 45 s (timeout) | 267.70ms | 258.11ms | > 174x |
| quad/dsg/cc-uq/b0/d5/L12 | operator | - | > 45 s | 264.12ms | 254.53ms | > 177x |
| quad/dsg/cc-uq/b0/d5/L12 | generation | - | n/m | 2.39ms | 2.45ms | - |
| quad/dsg/cc-uq/b0/d7/L10 | total | - | > 45 s (timeout) | 1.524s | 1.490s | > 30x |
| quad/dsg/cc-uq/b0/d7/L10 | operator | - | > 45 s | 1.516s | 1.480s | > 30x |
| quad/dsg/cc-uq/b0/d7/L10 | generation | - | n/m | 3.35ms | 5.03ms | - |
| quad/dsg/cc-uq/b0/d10/L7 | total | - | > 45 s (timeout) | 7.044s | 6.955s | > 6.5x |
| quad/dsg/cc-uq/b0/d10/L7 | operator | - | > 45 s | 7.037s | 6.945s | > 6.5x |
| quad/dsg/cc-uq/b0/d10/L7 | generation | - | n/m | 3.02ms | 3.28ms | - |
| interp/sg/cc/b0/d2/L12/p64 | total | 250.33ms | 230.32ms | 235.12ms | 188.16ms | 1.33x |
| interp/sg/cc/b0/d2/L12/p64 | operator | 250.06ms | 230.07ms | 234.84ms | 187.72ms | 1.33x |
| interp/sg/cc/b0/d2/L12/p64 | generation | 211.86us | 205.12us | 224.96us | 205.61us | 1.03x |
| interp/sg/cc/b0/d3/L9/p64 | total | 323.23ms | 308.85ms | 309.48ms | 267.88ms | 1.21x |
| interp/sg/cc/b0/d3/L9/p64 | operator | 323.07ms | 308.69ms | 309.32ms | 267.71ms | 1.21x |
| interp/sg/cc/b0/d3/L9/p64 | generation | 86.53us | 77.77us | 77.52us | 73.12us | 1.18x |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.796s | 1.741s | 1.744s | 1.623s | 1.11x |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.796s | 1.741s | 1.744s | 1.623s | 1.11x |
| interp/sg/cc/b0/d5/L6/p64 | generation | 121.98us | 108.99us | 108.74us | 104.06us | 1.17x |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.508s | 1.474s | 1.478s | 1.405s | 1.07x |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.507s | 1.474s | 1.478s | 1.405s | 1.07x |
| interp/sg/cc/b0/d7/L4/p16 | generation | 89.68us | 84.54us | 84.16us | 67.33us | 1.33x |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.295s | 2.245s | 2.234s | 2.173s | 1.06x |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.295s | 2.245s | 2.234s | 2.173s | 1.06x |
| interp/sg/cc/b0/d10/L2/p8 | generation | 261.31us | 42.43us | 44.14us | 36.67us | 7.13x |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 194.75ms | 186.93ms | 188.25ms | 184.04ms | 1.06x |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 194.66ms | 186.84ms | 188.16ms | 183.95ms | 1.06x |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 32.93us | 31.46us | 29.12us | 28.36us | 1.16x |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 406.66ms | 369.32ms | 368.83ms | 353.36ms | 1.15x |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 406.52ms | 369.18ms | 368.69ms | 353.24ms | 1.15x |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 52.21us | 39.53us | 37.51us | 35.55us | 1.47x |
| interp/sg/equi/b0/d3/L5/p64 | total | 55.42ms | 53.37ms | 53.49ms | 52.08ms | 1.06x |
| interp/sg/equi/b0/d3/L5/p64 | operator | 55.34ms | 53.29ms | 53.42ms | 52.00ms | 1.06x |
| interp/sg/equi/b0/d3/L5/p64 | generation | 28.29us | 21.96us | 20.39us | 24.38us | 1.16x |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 158.48ms | 112.08ms | 117.18ms | 110.37ms | 1.44x |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 151.64ms | 106.91ms | 112.14ms | 104.19ms | 1.46x |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.09ms | 357.62us | 357.38us | 1.79ms | 1.17x |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 510.64ms | 531.38ms | 515.72ms | 479.21ms | 1.07x |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 508.57ms | 528.56ms | 512.67ms | 476.71ms | 1.07x |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 1.34ms | 2.10ms | 2.23ms | 2.15ms | 0.62x |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.680s | 1.673s | 1.694s | 1.679s | 1.00x |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.678s | 1.670s | 1.691s | 1.675s | 1.00x |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 1.56ms | 2.27ms | 2.35ms | 2.36ms | 0.66x |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.701s | 2.691s | 2.696s | 2.681s | 1.01x |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.699s | 2.688s | 2.694s | 2.678s | 1.01x |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.10ms | 2.42ms | 1.73ms | 1.77ms | 0.62x |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 3.211s | 2.250s | 2.256s | 719.39ms | 4.46x |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 3.204s | 2.245s | 2.251s | 713.95ms | 4.49x |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 6.35ms | 4.65ms | 4.60ms | 4.64ms | 1.37x |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.615s | 3.141s | 3.256s | 1.812s | 1.99x |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 2.574s | 1.866s | 1.884s | 485.64ms | 5.30x |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.039s | 1.273s | 1.373s | 1.327s | 0.78x |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.693s | 1.763s | 1.890s | 1.704s | 0.99x |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 135.62ms | 99.44ms | 100.33ms | 24.78ms | 5.47x |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.557s | 1.663s | 1.787s | 1.679s | 0.93x |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.668s | 1.651s | 1.910s | 1.876s | 0.89x |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 12.34ms | 9.91ms | 9.80ms | 2.07ms | 5.97x |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.655s | 1.641s | 1.900s | 1.874s | 0.88x |

Sums of the `total` medians per case group (`quad/sg` without the estimated `cc-uq/b0/d2/L20`; `>`: contains lower bounds; `quad/dsg` does not exist on master):

| group | cases | master (thesis) | e6a21c331 (first DSG) | baseline (before WP-01) | after WP-13 | speedup (first → WP-13) |
|---|---|---|---|---|---|---|
| quad/sg | 12 | 29.048s | 22.042s | 20.916s | 7.971s | 3.64x |
| quad/dsg | 9 | - | > 596.268s | 26.804s | 26.301s | > 23x |
| interp/sg | 8 | 6.829s | 6.610s | 6.612s | 6.246s | 1.09x |
| interplin/sg | 4 | 5.050s | 5.007s | 5.023s | 4.950s | 1.02x |
| mivec/sg | 4 | 10.186s | 8.805s | 9.312s | 6.112s | 1.67x |
| **all except quad/dsg and quad/sg/cc-uq/b0/d2/L20** | 28 | **51.113s** | **42.463s** | **41.863s** | **25.279s** | **2.02x** |

## Notes

- **Clenshaw-Curtis weights on master and e6a21c331 are the naive O(n²) formula.** In both
  states the FFT (Waldvogel) branches of `clenshaw_curtis_quadrature_rule.cpp` do not compile
  (const member call to a free function, `std::vector` → `DataVector`, undeclared `len`; the
  mirror loop bound `m < b.size()` would never terminate), so every build of these states that
  compiled used the naive fallback. The worktree builds disable the `__has_include` detection
  (which today finds `<armadillo>` in `/usr/include` and fails to compile) to reproduce exactly
  that. The FFT works since 91dbd3b1b (before the baseline). Affected: every `cc` quadrature
  case (`quad/*/cc-uq`, `mivec`); interpolation does not use quadrature weights.
- **`quad/sg/cc-uq/b0/d2/L20` was not run on master / e6a21c331**: its level-20 1D rule has
  2,097,153 nodes, the naive formula needs (2²⁰)² ≈ 1.1·10¹² dependent recurrence steps for that
  single weight vector, measured at 2.3 ns/step (2.0 GHz) → ≥ 42 min per run (estimate).
- **DSG quadrature of e6a21c331** is the first implementation (before 45fa2f1c8 "factorizing the
  global weights along a coefficient trie"). Cases that did not finish within 45 s are lower
  bounds; for `quad/dsg/equi-exp/b0/d7/L10` an earlier attempt ran 521 s for warm-up + sample
  without finishing (→ > 4.3 min per run).
- **Build changes needed only to compile the old states with today's GCC 15.2** (no effect on
  the generated code of the library): master `-include cstdint` (GCC 15 no longer includes it
  transitively). `perf_e2e` did not exist then: the protected sources were copied unchanged
  except master without the DSG cases and e6a21c331 with a `quadrature()` → `quadratureDelta()`
  forwarder; on master only `perf_e2e.cpp` itself is compiled with `-std=c++23` (no `-DNDEBUG`).
- **Comparability**: `perf.py calibrate` before the measurements: baseline binaries at 1.007 /
  1.003 (16-thread e2e, burst / sustained) and 1.004 / 0.999 (1-thread micro) of
  `baseline/calibration.json`, i.e. the machine state equals the baseline recording within 1 %.
  With 2 processes per state the numbers carry process-to-process noise of a few % (bimodal
  cases up to ~9 %); differences below ~5 % between master / e6a21c331 / baseline are not
  meaningful. Millisecond-scale `generation` times depend on the measurement pattern (the
  baseline's 5 in-process samples run warm, the other columns use fresh processes): compare
  `total` / `operator`.
- **What lies between the columns**: master → e6a21c331: the owner's work after the thesis
  (DSG quadrature, refactorings, site_scons `-DNDEBUG`/C++23); e6a21c331 → baseline: DSG trie
  factorization, CC FFT backend fix, benchmark work; baseline → WP-13: WP-01 (SG quadrature
  per-grid-point work) and WP-13 (`-fno-semantic-interposition`); WP-11 only measured.
- Raw data: `run-master{,-2}.json`, `run-dev{,-2}.json`, `run-dev-dsg.jsonl`, `calibrate.txt` in this
  directory; snapshots `report-master` / `report-dev` in `combigrid/perf/.work/snapshots/`.

<details><summary>Relative errors vs. the exact value (last measured process of each state; the binding accuracy check is <code>refcheck</code>)</summary>

| case | master (thesis) | e6a21c331 (first DSG) | baseline (before WP-01) | after WP-13 |
|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 3.01e-12 | 3.01e-12 | 3.01e-12 | 7.18e-14 |
| quad/sg/equi-exp/b0/d5/L10 | 2.26e-13 | 2.69e-13 | 4.73e-13 | 8.25e-13 |
| quad/sg/equi-exp/b0/d7/L7 | 6.26e-11 | 5.89e-11 | 5.89e-11 | 1.75e-11 |
| quad/sg/equi-exp/b0/d10/L4 | 3.63e-09 | 3.63e-09 | 3.63e-09 | 3.39e-09 |
| quad/sg/equi-exp/b1/d10/L7 | 2.87e-10 | 1.31e-10 | 2.87e-10 | 2.97e-10 |
| quad/sg/equi-exp/b1/d15/L6 | 1.68e-09 | 1.96e-09 | 1.65e-09 | 1.81e-09 |
| quad/sg/equi-exp/b1/d20/L5 | 3.91e-08 | 3.87e-08 | 3.83e-08 | 3.83e-08 |
| quad/sg/equi-sin/b0/d7/L7 | 7.66e-12 | 5.41e-12 | 8.17e-12 | 1.31e-11 |
| quad/sg/cc-uq/b0/d2/L20 | - | - | 9.93e-12 | 3.62e-13 |
| quad/sg/cc-uq/b0/d3/L14 | 2.36e-09 | 2.36e-09 | 1.19e-12 | 1.39e-13 |
| quad/sg/cc-uq/b0/d5/L10 | 1.33e-11 | 1.34e-11 | 8.07e-12 | 3.81e-12 |
| quad/sg/cc-uq/b0/d7/L7 | 8.95e-12 | 6.24e-12 | 2.57e-12 | 7.32e-12 |
| quad/sg/cc-uq/b0/d10/L4 | 1.24e-10 | 1.24e-10 | 1.25e-10 | 1.35e-10 |
| quad/dsg/equi-exp/b0/d3/L16 | - | 1.16e-12 | 1.15e-12 | 1.15e-12 |
| quad/dsg/equi-exp/b0/d5/L12 | - | - | 3.61e-12 | 3.60e-12 |
| quad/dsg/equi-exp/b0/d7/L10 | - | - | 2.10e-11 | 2.09e-11 |
| quad/dsg/equi-exp/b0/d10/L7 | - | - | 1.06e-10 | 1.05e-10 |
| quad/dsg/equi-exp/b1/d15/L7 | - | - | 8.61e-10 | 1.16e-09 |
| quad/dsg/equi-exp/b1/d20/L6 | - | - | 6.80e-09 | 5.92e-09 |
| quad/dsg/cc-uq/b0/d5/L12 | - | - | 1.33e-13 | 1.35e-13 |
| quad/dsg/cc-uq/b0/d7/L10 | - | - | 2.20e-12 | 2.28e-12 |
| quad/dsg/cc-uq/b0/d10/L7 | - | - | 2.82e-11 | 2.80e-11 |
| interp/sg/cc/b0/d2/L12/p64 | 2.04e-15 | 2.04e-15 | 2.04e-15 | 1.90e-15 |
| interp/sg/cc/b0/d3/L9/p64 | 2.15e-15 | 3.28e-15 | 0.00e+00 | 1.24e-15 |
| interp/sg/cc/b0/d5/L6/p64 | 7.11e-08 | 7.11e-08 | 7.11e-08 | 7.11e-08 |
| interp/sg/cc/b0/d7/L4/p16 | 1.25e-04 | 1.25e-04 | 1.25e-04 | 1.25e-04 |
| interp/sg/cc/b0/d10/L2/p8 | 3.50e-02 | 3.50e-02 | 3.50e-02 | 3.50e-02 |
| interp/sg/cheb1/b0/d3/L6/p64 | 1.95e-13 | 1.95e-13 | 1.94e-13 | 1.94e-13 |
| interp/sg/cheb1/b0/d5/L4/p64 | 2.28e-04 | 2.28e-04 | 2.28e-04 | 2.28e-04 |
| interp/sg/equi/b0/d3/L5/p64 | 3.54e-02 | 3.54e-02 | 3.54e-02 | 3.54e-02 |
| interplin/sg/equi/b1/d2/L16/p16384 | 1.92e-10 | 1.92e-10 | 1.92e-10 | 1.92e-10 |
| interplin/sg/equi/b1/d5/L8/p1024 | 1.71e-04 | 1.71e-04 | 1.71e-04 | 1.71e-04 |
| interplin/sg/equi/b1/d8/L5/p256 | 3.19e-02 | 3.19e-02 | 3.19e-02 | 3.19e-02 |
| interplin/sg/equi/b1/d10/L5/p32 | 4.61e-02 | 4.61e-02 | 4.61e-02 | 4.61e-02 |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 2.54e-12 | 2.54e-12 | 9.07e-13 | 1.35e-13 |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1.24e-08 | 1.24e-08 | 1.24e-08 | 1.24e-08 |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.92e-03 | 1.92e-03 | 1.92e-03 | 1.92e-03 |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 3.66e-02 | 3.66e-02 | 3.66e-02 | 3.66e-02 |

</details>
