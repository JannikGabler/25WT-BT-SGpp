# WP-79: textbook vs. optimized operators (speed reference, not judged)

`perf.py run micro --snapshot WP-79-after --filter '^micro/textbook/'` (1 thread, judged build
configuration, 2026-11-19; raw: `micro-textbook-speed.json`). Same `SparseGrid`, integrand
`perf::cosProd` (d cosines per evaluation) on the shifted domain, one query point for the
interpolations. Items: grid points of all tensor grids (interplin: 2^d corners per tensor grid).

| operator | grid | textbook | optimized | textbook / optimized |
|---|---|---|---|---|
| quadrature | cc b0 d3 L6 | 2.95 ms (76 ns/pt) | 2.09 ms (54 ns/pt) | 1.41x |
| quadrature | equi b1 d5 L5 | 9.07 ms (125 ns/pt) | 6.48 ms (90 ns/pt) | 1.40x |
| interpolate | cc b0 d3 L5 | 1.22 ms (81 ns/pt) | 0.83 ms (55 ns/pt) | 1.47x |
| interpolate | cc b0 d5 L4 | 25.5 ms (123 ns/pt) | 18.6 ms (90 ns/pt) | 1.37x |
| interpolateLinear | equi b1 d5 L5 | 0.85 ms (106 ns/corner) | 0.73 ms (91 ns/corner) | 1.17x |
| interpolateLinear | equi b1 d8 L4 | 21.2 ms (167 ns/corner) | 18.8 ms (148 ns/corner) | 1.13x |

Reading: at 1 thread and with this integrand (≈ 40–90 ns per evaluation) the textbook operators
cost 1.1–1.5x the optimized ones; the difference is the framework work per point (index
decomposition, d-fold weight product, `getGridPoint`, the `std::function` of the
`VectorSourceFunc` wrapper of the scalar overload). With cheaper integrands the ratio grows (the
optimized quadrature's row loop needs ≈ 25 ns/point at d10 without the integrand,
`micro/sol/quad_tg`), with expensive integrands it tends to 1. The optimized operators also run
in parallel (16 threads) and the textbook ones do not, so at 16 threads the gap is roughly the
parallel speedup on top. The C++ example (`examples/c++/textbook/`, OPT=0 build, 16 threads,
cc d3 L5) showed 5x (quadrature), 35x (interpolate), 10x (interpolateLinear).
