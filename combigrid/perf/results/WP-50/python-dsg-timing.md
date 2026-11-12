# WP-50: Python, DeltasSparseGrid vs. SparseGrid operators with Python callables

Script: `python_dsg_timing.py` (callables and columns explained there). Bindings: scratch build of the WP-50 .i files (python-bindings.md "Fast check", wrapper `-O2`) against the `perf.py build normal` libraries (= WP-50-after, `.text` identical to WP-50-before). Machine idle, boost off, `envcheck` OK before each run. 1 thread: `OMP_PLACES={4}`; 16 threads: `OMP_PROC_BIND=close OMP_PLACES=threads`. speedup = SparseGrid / DeltasSparseGrid (same callable, same grid instruction, grids built beforehand, DSG per-grid index built by an untimed warm-up call).

Reading: the speedup follows the ratio of the Python call counts (the time per call is the same for both grid types), so Python callers get the evaluation savings of the C++ DSG operators directly. At 16 threads every Python call costs ≈ 23 µs instead of 0.6–0.9 µs (GIL contention under the operators' OpenMP loops, WP-34; not addressed here): with Python callables, 1 thread is 30–40x faster than 16 for both grid types.

## 1 thread (quad/interp cc d5 L5 and d10 L2, interp equi d3 L5, interplin d5 L6 and d8 L5)

| case | function | SparseGrid | DeltasSparseGrid | speedup | Python calls SG / DSG (ratio) | µs per call SG / DSG |
|---|---|---|---|---|---|---|
| quad/cc/b0/d5/L5 | scalar | 404.9 ms | 62.1 ms | 6.52 | 698885 / 102785 (6.80) | 0.58 / 0.60 |
| quad/cc/b0/d5/L5 | vector m=4 | 544.0 ms | 82.5 ms | 6.59 | 698885 / 102785 (6.80) | 0.78 / 0.80 |
| quad/cc/b0/d10/L2 | scalar | 7374.7 ms | 1768.8 ms | 4.17 | 10195794 / 2421009 (4.21) | 0.72 / 0.73 |
| quad/cc/b0/d10/L2 | vector m=4 | 9294.1 ms | 2228.9 ms | 4.17 | 10195794 / 2421009 (4.21) | 0.91 / 0.92 |
| interp/cc/b0/d5/L5 | scalar | 425.4 ms | 62.8 ms | 6.78 | 698885 / 102785 (6.80) | 0.61 / 0.61 |
| interp/cc/b0/d5/L5 | vector m=4 | 578.0 ms | 82.3 ms | 7.02 | 698885 / 102785 (6.80) | 0.83 / 0.80 |
| interp/cc/b0/d10/L2 | scalar | 7801.7 ms | 1773.9 ms | 4.40 | 10195794 / 2421009 (4.21) | 0.77 / 0.73 |
| interp/cc/b0/d10/L2 | vector m=4 | 10078.8 ms | 2227.8 ms | 4.52 | 10195794 / 2421009 (4.21) | 0.99 / 0.92 |
| interp/equi/b0/d3/L5 | scalar | 11.0 ms | 2.3 ms | 4.85 | 15188 / 3713 (4.09) | 0.73 / 0.61 |
| interp/equi/b0/d3/L5 | vector m=4 | 13.9 ms | 3.0 ms | 4.67 | 15188 / 3713 (4.09) | 0.92 / 0.80 |
| interplin/equi/b1/d5/L6 | scalar | 8.6 ms | 1.9 ms | 4.52 | 14592 / 2662 (5.48) | 0.59 / 0.72 |
| interplin/equi/b1/d5/L6 | vector m=4 | 11.6 ms | 2.4 ms | 4.84 | 14592 / 2662 (5.48) | 0.79 / 0.90 |
| interplin/equi/b1/d8/L5 | scalar | 216.9 ms | 33.1 ms | 6.56 | 329472 / 40816 (8.07) | 0.66 / 0.81 |
| interplin/equi/b1/d8/L5 | vector m=4 | 276.7 ms | 40.6 ms | 6.81 | 329472 / 40816 (8.07) | 0.84 / 1.00 |

## 16 threads, --small (one level less)

| case | function | SparseGrid | DeltasSparseGrid | speedup | Python calls SG / DSG (ratio) | µs per call SG / DSG |
|---|---|---|---|---|---|---|
| quad/cc/b0/d5/L4 | scalar | 4885.2 ms | 861.9 ms | 5.67 | 206358 / 36033 (5.73) | 23.67 / 23.92 |
| quad/cc/b0/d5/L4 | vector m=4 | 4966.2 ms | 879.1 ms | 5.65 | 206358 / 36033 (5.73) | 24.07 / 24.40 |
| quad/cc/b0/d10/L1 | scalar | 23801.5 ms | 9474.0 ms | 2.51 | 1043199 / 452709 (2.30) | 22.82 / 20.93 |
| quad/cc/b0/d10/L1 | vector m=4 | 24151.7 ms | 9673.4 ms | 2.50 | 1043199 / 452709 (2.30) | 23.15 / 21.37 |
| interp/cc/b0/d5/L4 | scalar | 4858.3 ms | 851.2 ms | 5.71 | 206358 / 36033 (5.73) | 23.54 / 23.62 |
| interp/cc/b0/d5/L4 | vector m=4 | 4987.8 ms | 878.8 ms | 5.68 | 206358 / 36033 (5.73) | 24.17 / 24.39 |
| interp/cc/b0/d10/L1 | scalar | 23800.1 ms | 10686.2 ms | 2.23 | 1043199 / 452709 (2.30) | 22.81 / 23.61 |
| interp/cc/b0/d10/L1 | vector m=4 | 23921.4 ms | 11012.1 ms | 2.17 | 1043199 / 452709 (2.30) | 22.93 / 24.32 |
| interp/equi/b0/d3/L4 | scalar | 133.2 ms | 37.3 ms | 3.57 | 5687 / 1505 (3.78) | 23.42 / 24.79 |
| interp/equi/b0/d3/L4 | vector m=4 | 143.8 ms | 38.2 ms | 3.76 | 5687 / 1505 (3.78) | 25.29 / 25.38 |
| interplin/equi/b1/d5/L5 | scalar | 190.9 ms | 42.3 ms | 4.52 | 8032 / 1682 (4.78) | 23.77 / 25.12 |
| interplin/equi/b1/d5/L5 | vector m=4 | 196.1 ms | 44.9 ms | 4.37 | 8032 / 1682 (4.78) | 24.41 / 26.67 |
| interplin/equi/b1/d8/L4 | scalar | 2927.4 ms | 506.5 ms | 5.78 | 126720 / 20992 (6.04) | 23.10 / 24.13 |
| interplin/equi/b1/d8/L4 | vector m=4 | 2975.5 ms | 512.3 ms | 5.81 | 126720 / 20992 (6.04) | 23.48 / 24.41 |
