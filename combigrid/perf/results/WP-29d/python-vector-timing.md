# WP-29d: Python, one VectorSourceFunc call vs. m SourceFunc calls

Script: `python_vector_timing.py` (callables and columns explained there). Bindings: scratch build of the WP-29d .i files (python-bindings.md "Fast check", wrapper `-O2`) against the `perf.py build normal` libraries (= WP-29d-after). Machine as for the A/Bs (boost off, idle; `envcheck` OK before each run). 1 thread: `OMP_PLACES={4}`; 16 threads: `OMP_PROC_BIND=close OMP_PLACES=threads`. speedup = (m scalar runs) / (one vector run).

## 1 thread, default grids (quad/interp L4, interplin L5)


| case | callable | m | m scalar runs | vector run | speedup | µs per Python call (scalar / vector) |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | same work | 1 | 138.5 ms | 195.7 ms | 0.71 | 0.67 / 0.95 |
| quad/sg/cc/b0/d5/L4 | shared | 1 | 138.5 ms | 202.4 ms | 0.68 | 0.67 / 0.98 |
| quad/sg/cc/b0/d5/L4 | same work | 4 | 557.9 ms | 391.8 ms | 1.42 | 0.68 / 1.90 |
| quad/sg/cc/b0/d5/L4 | shared | 4 | 557.9 ms | 227.7 ms | 2.45 | 0.68 / 1.10 |
| quad/sg/cc/b0/d5/L4 | same work | 32 | 4471.5 ms | 2218.1 ms | 2.02 | 0.68 / 10.75 |
| quad/sg/cc/b0/d5/L4 | shared | 32 | 4471.5 ms | 544.2 ms | 8.22 | 0.68 / 2.64 |
| interp/sg/cc/b0/d5/L4 | same work | 1 | 148.0 ms | 195.4 ms | 0.76 | 0.72 / 0.95 |
| interp/sg/cc/b0/d5/L4 | shared | 1 | 148.0 ms | 201.7 ms | 0.73 | 0.72 / 0.98 |
| interp/sg/cc/b0/d5/L4 | same work | 4 | 588.3 ms | 389.4 ms | 1.51 | 0.71 / 1.89 |
| interp/sg/cc/b0/d5/L4 | shared | 4 | 588.3 ms | 232.8 ms | 2.53 | 0.71 / 1.13 |
| interp/sg/cc/b0/d5/L4 | same work | 32 | 4704.4 ms | 2064.1 ms | 2.28 | 0.71 / 10.00 |
| interp/sg/cc/b0/d5/L4 | shared | 32 | 4704.4 ms | 532.4 ms | 8.84 | 0.71 / 2.58 |
| interplin/sg/equi/b1/d5/L5 | same work | 1 | 6.1 ms | 8.1 ms | 0.76 | 0.77 / 1.01 |
| interplin/sg/equi/b1/d5/L5 | shared | 1 | 6.1 ms | 8.4 ms | 0.73 | 0.77 / 1.04 |
| interplin/sg/equi/b1/d5/L5 | same work | 4 | 24.5 ms | 15.5 ms | 1.59 | 0.76 / 1.92 |
| interplin/sg/equi/b1/d5/L5 | shared | 4 | 24.5 ms | 9.4 ms | 2.60 | 0.76 / 1.17 |
| interplin/sg/equi/b1/d5/L5 | same work | 32 | 194.8 ms | 80.7 ms | 2.41 | 0.76 / 10.04 |
| interplin/sg/equi/b1/d5/L5 | shared | 32 | 194.8 ms | 20.5 ms | 9.48 | 0.76 / 2.56 |

## 1 thread, --small grids (quad/interp L3, interplin L4)


| case | callable | m | m scalar runs | vector run | speedup | µs per Python call (scalar / vector) |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L3 | same work | 1 | 37.4 ms | 51.8 ms | 0.72 | 0.68 / 0.94 |
| quad/sg/cc/b0/d5/L3 | shared | 1 | 37.4 ms | 53.3 ms | 0.70 | 0.68 / 0.97 |
| quad/sg/cc/b0/d5/L3 | same work | 4 | 150.7 ms | 102.3 ms | 1.47 | 0.68 / 1.86 |
| quad/sg/cc/b0/d5/L3 | shared | 4 | 150.7 ms | 60.1 ms | 2.51 | 0.68 / 1.09 |
| quad/sg/cc/b0/d5/L3 | same work | 32 | 1217.5 ms | 546.1 ms | 2.23 | 0.69 / 9.91 |
| quad/sg/cc/b0/d5/L3 | shared | 32 | 1217.5 ms | 136.0 ms | 8.95 | 0.69 / 2.47 |
| interp/sg/cc/b0/d5/L3 | same work | 1 | 39.7 ms | 52.4 ms | 0.76 | 0.72 / 0.95 |
| interp/sg/cc/b0/d5/L3 | shared | 1 | 39.7 ms | 53.7 ms | 0.74 | 0.72 / 0.97 |
| interp/sg/cc/b0/d5/L3 | same work | 4 | 158.6 ms | 104.1 ms | 1.52 | 0.72 / 1.89 |
| interp/sg/cc/b0/d5/L3 | shared | 4 | 158.6 ms | 61.6 ms | 2.58 | 0.72 / 1.12 |
| interp/sg/cc/b0/d5/L3 | same work | 32 | 1269.7 ms | 548.9 ms | 2.31 | 0.72 / 9.96 |
| interp/sg/cc/b0/d5/L3 | shared | 32 | 1269.7 ms | 138.6 ms | 9.16 | 0.72 / 2.52 |
| interplin/sg/equi/b1/d5/L4 | same work | 1 | 3.1 ms | 4.0 ms | 0.76 | 0.77 / 1.00 |
| interplin/sg/equi/b1/d5/L4 | shared | 1 | 3.1 ms | 4.1 ms | 0.75 | 0.77 / 1.03 |
| interplin/sg/equi/b1/d5/L4 | same work | 4 | 12.3 ms | 7.7 ms | 1.60 | 0.76 / 1.91 |
| interplin/sg/equi/b1/d5/L4 | shared | 4 | 12.3 ms | 4.7 ms | 2.64 | 0.76 / 1.16 |
| interplin/sg/equi/b1/d5/L4 | same work | 32 | 98.5 ms | 40.0 ms | 2.46 | 0.76 / 9.93 |
| interplin/sg/equi/b1/d5/L4 | shared | 32 | 98.5 ms | 10.1 ms | 9.79 | 0.76 / 2.49 |

## 16 threads, --small grids


| case | callable | m | m scalar runs | vector run | speedup | µs per Python call (scalar / vector) |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L3 | same work | 1 | 1274.6 ms | 1278.2 ms | 1.00 | 23.13 / 23.20 |
| quad/sg/cc/b0/d5/L3 | shared | 1 | 1274.6 ms | 1329.7 ms | 0.96 | 23.13 / 24.13 |
| quad/sg/cc/b0/d5/L3 | same work | 4 | 5142.9 ms | 1399.8 ms | 3.67 | 23.34 / 25.40 |
| quad/sg/cc/b0/d5/L3 | shared | 4 | 5142.9 ms | 1327.9 ms | 3.87 | 23.34 / 24.10 |
| quad/sg/cc/b0/d5/L3 | same work | 32 | 41325.0 ms | 1698.3 ms | 24.33 | 23.44 / 30.82 |
| quad/sg/cc/b0/d5/L3 | shared | 32 | 41325.0 ms | 1418.6 ms | 29.13 | 23.44 / 25.75 |
| interp/sg/cc/b0/d5/L3 | same work | 1 | 1295.6 ms | 1348.9 ms | 0.96 | 23.51 / 24.48 |
| interp/sg/cc/b0/d5/L3 | shared | 1 | 1295.6 ms | 1365.8 ms | 0.95 | 23.51 / 24.79 |
| interp/sg/cc/b0/d5/L3 | same work | 4 | 5149.6 ms | 1391.4 ms | 3.70 | 23.37 / 25.25 |
| interp/sg/cc/b0/d5/L3 | shared | 4 | 5149.6 ms | 1349.7 ms | 3.82 | 23.37 / 24.50 |
| interp/sg/cc/b0/d5/L3 | same work | 32 | 41503.8 ms | 1707.8 ms | 24.30 | 23.54 / 31.00 |
| interp/sg/cc/b0/d5/L3 | shared | 32 | 41503.8 ms | 1423.7 ms | 29.15 | 23.54 / 25.84 |
| interplin/sg/equi/b1/d5/L4 | same work | 1 | 99.6 ms | 92.9 ms | 1.07 | 24.70 / 23.04 |
| interplin/sg/equi/b1/d5/L4 | shared | 1 | 99.6 ms | 94.9 ms | 1.05 | 24.70 / 23.52 |
| interplin/sg/equi/b1/d5/L4 | same work | 4 | 386.4 ms | 103.9 ms | 3.72 | 23.96 / 25.78 |
| interplin/sg/equi/b1/d5/L4 | shared | 4 | 386.4 ms | 94.9 ms | 4.07 | 23.96 / 23.54 |
| interplin/sg/equi/b1/d5/L4 | same work | 32 | 3117.2 ms | 127.2 ms | 24.50 | 24.16 / 31.55 |
| interplin/sg/equi/b1/d5/L4 | shared | 32 | 3117.2 ms | 101.9 ms | 30.59 | 24.16 / 25.27 |

## Attribution of the m = 1 cost (1 thread, quad/sg/cc/b0/d5/L4, median of 7)

The binding adds ≈ 27 ns per call for m = 1 (constant list vs. constant float); the rest of the m = 1 slowdown is the callable building its list (`[x]` +0.07 µs, a comprehension over `range(m)` +0.2 µs) plus the C++ vector path at m = 1 (WP-30).

evaluations per quadrature: 206358

| callable | µs per call |
|---|---|
| scalar: return 1.0 | 0.256 |
| vector m=1: return const list | 0.283 |
| vector m=1: return const tuple | 0.286 |
| vector m=1: return [1.0] | 0.355 |
| scalar: exp(0.5*sum(p)) | 0.535 |
| vector m=1: [exp(0.5*sum(p))] | 0.632 |
| vector m=1: [exp(..) + k for k in range(1)] | 0.828 |
