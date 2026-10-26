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
