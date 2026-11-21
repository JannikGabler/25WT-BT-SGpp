# WP-79 — Textbook operator variants (naive per-tensor-grid CT)

Status: PROPOSED · Date: 2026-11-19 · Base commit: 941aff7d2
Recommendation: accept — three readable textbook operators (C++ + Python), equal to the optimized operators up to round-off with bitwise the same evaluation points; judged suite neutral.
Machine: `perf.py calibrate` ratios e2e 1.015 / 0.999 (burst / sustained), micro 0.997 / 1.002 at start; 1.001 / 1.000, 1.005 / 1.003 before measuring

## Change
- Owner decisions (in-session, 2026-11-18): names `textbookQuadrature` / `textbookInterpolate` /
  `textbookInterpolateLinear` (prefix, same in C++ and Python); scalar + vector-valued with **one
  vector core** per operator (the `SourceFunc` overload wraps `f` as a 1-output
  `VectorSourceFunc`, then calls the user's `finishOperatorCall()` once); 1D solvers: the node
  family's quadrature rule (`getQuadRule(n)->genWeights(n)`), Lagrange basis written out as the
  product formula, multilinear cell written out. Owner 2026-11-19 (mid-session): the textbook
  operators explain the CT; stability / accuracy are secondary, easy fixes only.
- New `operators/textbook/textbook_{quadrature,global_interpolation,linear_interpolation}.{hpp,cpp}`:
  plain serial loop over the TGs (`domainTensorGrids()` / `unitTensorGrids()`), each TG solved on its
  own (`getGridPoint` + d-fold weight product; linear: `std::ranges::lower_bound` cell + 2^d corners
  via `tools::mapToDomain`), `Σ c_ℓ · Op_ℓ`. Helpers in anonymous namespaces of each `.cpp`; Doxygen
  with the formulas, complexity, numerics note, pointer to the optimized operator. Overloads as the
  optimized SG operators (double / `std::span` / `DataVector`); no options, no OpenMP, no caches.
- Integration: umbrella header; `combigrid.i` (`%ignore` of the span overloads,
  `COMBIGRID_SOURCE_FUNC_EXCEPTION` for all three); examples
  `examples/c++/textbook/sin_func_textbook_operators.cpp`, `examples/python/sin_func_textbook_operators.py`;
  6 micro cases `micro/textbook/{quad,interp,interplin}/{textbook,optimized}/...`; docs CLAUDE.md §2,
  `module-reference.md` (layout + entry), `python-bindings.md`.

## Gain vs. cost
- Gain: none intended (feature). Judged suite neutral: suite 36.504 → 36.423 s (1.002x),
  `expensive/` 17.874 → 17.884 s (0.999x). Speed reference (1 thread, `cosProd`): textbook /
  optimized 1.40–1.41x quadrature, 1.37–1.47x interpolate, 1.13–1.17x interpolateLinear
  (`results/WP-79/micro-textbook-speed.md`); the optimized operators are also parallel.
- Memory: unchanged (geomean B/A 1.008, 0 `MEM+`/`MEM-`; largest interp/sg equi d3 L5 12.1 → 12.4 MiB).
- API / functionality: 3 new public functions × 3 overloads (C++), 3 Python functions; agreed by the owner. Existing operators untouched.
- Maintainability / readability: +≈ 430 lines C++ (3 × ≈ 150, about half Doxygen), test suite ≈ 370 lines.

## Tests
- Pinning: no existing code changed; full suite on the unchanged source (debug, 3×) passed before the change.
- New: `tests/operators/textbook_operators_test.cpp` (suite `textbook_operators`, 8 cases):
  textbook == optimized (scalar + vector m = 3) for 3 node families × d 1–4 × levels 0–4/0–3 ×
  b0/b1/b2 × unit/shifted + anisotropic MIVec grids, 4 query points (generic, on a node, near the
  lower corner, upper corner), rel. tol 1e-11 (equidistant level 4 = 33 nodes: 1e-6, Runge, measured
  ≤ 3e-8); exactness (CC: ∫ and interpolant of x^7 y^3, multilinear on b1 equi); same evaluation
  points as multisets of bit patterns; one `finishOperatorCall` per call; argument checks; cheb1
  quadrature throws.
- Results: debug `test --repeat 3` ✓ · normal `--repeat 3` ✓ · ASan (new suite, 8G cap) ✓ ·
  Python `results/WP-79/python_checks.py` ✓ at 1 and 16 threads (results, Python call counts,
  exceptions, span overloads hidden, DSG rejected, `CachedSourceFunc` hits) · Python + C++ examples ✓.

## Reference check
- WARN at 1 and 16 threads, 145 OK: only the known owner-accepted case
  `interp/sg/equi/b0/d2/L5/cos-shifted` (×2.34, STATUS.md "tolerance stays"); identical on WP-79-before.

## Benchmarks (A = WP-79-before, B = WP-79-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; classic cases `'^micro/(?!cache/|expensive/|textbook/)(?!.*(_vec|_scalar_x_m)/)'`, 143 cases; unchanged code):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| sum of medians | 15.263 s | 15.274 s | 0.999 | – |
| tg/iterate_mutable/equi/b0/d10/L4 (largest shift) | 7.90 ms | 7.47 ms | 1.054 | [1.043, 1.064] |
| mivec/downwards_closure/d20/S7/n300 (largest slowdown) | 111.3 µs | 113.9 µs | 0.975 | [0.963, 0.987] |

8 faster / 5 SLOWER outside the noise band, all in unchanged code (placement; three new TUs in the library).

L2/L3 e2e (16 threads, full suite, 73 cases): 1 SLOWER, 1 faster, rest noise.

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.36 ms | 248.98 ms | 0.994 | [0.990, 0.998] |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 334.02 ms | 332.87 ms | 1.004 | [1.001, 1.008] |

Sum of medians: suite A = 36.504 s, B = 36.423 s (1.002x); `expensive/` 17.874 / 17.884 s; cached 1.048 / 1.045 s · Full reports: `combigrid/perf/results/WP-79/`

Peak memory: geomean B/A = 1.008, 0 flags · process floor A = 10.3 MiB, B = 10.3 MiB

## Attempts not kept
- Lagrange product with numerator / denominator formed on distances × 4 (interval of capacity 1): 1D CC / cheb1 sound up to level 9 instead of 8 (level ≥ 10 NaN from partial products all the same); dropped after the owner's note (readability over range). Plain product: ≤ 1e-14 vs. barycentric up to level 8 (513 nodes), errors ≈ 10 from level 9 on (documented in the Doxygen note).
- `{1, 0}` weights for a degenerate linear cell → `{0.5, 0.5}` as `interpolateLinear` (same results also for inf values).

## Profile shift / follow-ups
- Bug in the existing SG `quadrature` (not changed here): with cheb1 nodes `getQuadRule` throws inside the OpenMP region → `std::terminate` instead of `not_implemented_exception` (the textbook operator throws cleanly). Candidate: compute/validate the rules before the parallel region.
- `interpolateLinear` semantics surfaced while writing the oracle: without boundary nodes the cell next to the boundary reaches to the domain boundary and `f` is evaluated there (outside the grid). Documented in the textbook header; worth a sentence in the optimized operator's docs.
- Dimension-adaptive CT: the textbook loop forms `(c_ℓ, Op_ℓ f)` per TG; a variant returning the per-TG values (e.g. a callback or vector of per-TG results) would give refinement indicators directly; not exposed (owner to decide).
- cpplint `build/include_order` fires on every combigrid file with the module's include convention (not counted by `perf.py build`); a `CPPLINT.cfg` filter would silence it repo-wide.
- Strict aliasing: no code written for `-fno-strict-aliasing` touched.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
