# WP-47 — variants (exploratory; not A/B-grade)

Standalone loop `loop_driver.cpp` (`<d> <L> <calls> [sg]`: equi, b1, complete grid, integrand
prod cos(x_i), x[0] varied per call; compiled with `g++ -O3 -march=native -fopenmp` against the
in-tree normal build), µs or ms per call, OMP_PROC_BIND=close, OMP_PLACES=threads. Single runs,
spread between repeated runs ≈ ±5 %.

| variant | 16 t d10 L5 | 16 t d8 L5 | 16 t d5 L8 | d2 L16 (1 t / 16 t) | 1 t d10 L5 |
|---|---|---|---|---|---|
| 3 passes, serial path inside `omp parallel if(false)` | – | – | – | 12.1 / 12.4 µs | – |
| serial path outside any parallel region (kept) | 12.83 ms | 1.56 ms | 226 µs | 8.8 / 9.2 µs | 116.2 ms |
| + branchless lower bound (dropped) | – | – | – | 9.6 / 9.7 µs | – |
| claim variant: CAS per slot, claimer evaluates, 2 passes (dropped) | 12.91 ms | 1.61 ms | 269 µs | 9.0 / 8.9 µs | 124.7 ms |
| flat per-TG corner ids instead of per-dimension pointers (kept) | 12.32 ms | 1.51 ms | 235 µs | – | 113.2 ms |
| per-thread mark bitsets, OR per 64-slot word (dropped) | 12.40 ms | 1.61 ms | 236 µs | 14.0 / 14.1 µs | 113.7 ms |
| decomposition into helpers (kept) | 11.98 ms | 1.55 ms | 252 µs | 9.3 / 9.4 µs | 113.3 ms |
| galloping search from the previous level (kept) | 12.13 ms | 1.47 ms | 231 µs | 8.7 / 8.8 µs | 112.8 ms |

SparseGrid operator in the same driver: d2 L16 8.9 µs (1 t) / 6.3 µs (16 t), d5 L8 296 µs (16 t).
The galloping search mainly helps random query points: d2 in perf_interp_explore 13.1 → 10.7 µs.

Work gate (`DSG_PARALLEL_MIN_CORNER_VISITS`), 16 t, always parallel vs. always serial:

| problem (corner visits per pass) | parallel | serial |
|---|---|---|
| d2 L16 (132) | 13.6 µs | 9.4 µs |
| d3 L8 (872) | 22.1 µs | 38.3 µs |
| d3 L12 (1880) | 40.0 µs | 85.2 µs |
| d4 L6 (3120) | 46.1 µs | 125.8 µs |
| d4 L8 (6800) | 91.0 µs | 281.2 µs |
| d5 L5 (8032) | 100.3 µs | 320.2 µs |
| d6 L5 | 339.6 µs | 1192.5 µs |

→ gate 256 (first version 4096).

Index build on the first call (`first_call_driver.cpp`, 1 t, d2 L16): 40 ms (binary search per
delta node in `countNewCoords`) → 19 ms (linear walk) → 7.7 ms (reserved vectors, no sort of
sorted nodes). Grid generation of the same DSG: ≈ 4 ms.
