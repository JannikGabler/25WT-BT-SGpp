# WP-83 — SG quadrature with cheb1 nodes: `std::terminate` fix + Chebyshev (first kind) quadrature rule

Status: PROPOSED · Date: 2026-11-22 · Base commit: d3b7f00ba
Recommendation: accept — cheb1 quadrature works (C++, textbook, Python), exact on the CT's polynomial spaces at every boundary offset; a missing rule now throws catchably; existing results bitwise, judged suite neutral.
Machine: `perf.py calibrate` ratios e2e 1.002 / 0.999 (burst / sustained), micro 0.998 / 1.001 at start; 1.002 / 0.999, 1.002 / 1.001 before measuring

## Change
- **Owner decisions in-session (2026-11-22)**, prompted by a finding: cheb1 grids with boundary offset 0 / 1 have the node set {0, n − 2 cheb1 nodes, 1}, on which Fejér-1 weights are wrong (O(1/m) error, no convergence). The rule interface only gets n. Decided: (1) an additive virtual `NodeGenFunc::getQuadRule(nNodes, includeBoundary)`, whose default returns `getQuadRule(nNodes)` (equi / CC bitwise unchanged); cheb1 overrides it. (2) With boundary nodes, the **interpolatory** rule (∫ of the interpolant), not Fejér + zero endpoint weights. Both have the same convergence order; interpolatory is 1–4 orders more accurate at equal m for smooth f (exp(3x), m = 15: 8.5e-21 vs. 4.0e-17) and consistent with `interpolate`.
- New `quadrature_rules::FejerFirstQuadRule(includeBoundary)` + getter `getFejerFirstQuadRule(bool)` (internal like the CC rule, not in the umbrella header):
  - Without boundary: Fejér's first rule.
  - With boundary: Fejér weights of the m inner nodes, minus the closed form W·(ℓ_k(1) + ℓ_k(−1)), with endpoint weight W = ¼∫(1+t)T_m. Inner weights are positive (checked m < 3000); W = −O(1/m²); Σ|w| = 1 + 4|W|.
  - Fejér's sums are a length-n DFT. n = 2^{ℓ+1} − 1 has large prime factors (131071 is prime), so they are computed by Bluestein's algorithm: 3 forward complex FFTs of the next power of two, one Eigen plan (inverse by conjugation), exponents reduced exactly mod 2n. Armadillo path; O(n²) direct sums without an FFT backend. Weights are bitwise symmetric.
- Bug fix: `QuadWeightTable`'s constructor obtains every row's rule serially (`rules_`, `lvl >= offset`). The parallel `computeAllRows` only calls `genWeightsInplace`, so a family without a rule throws before any OpenMP region. A failed build is not cached. `textbookQuadrature` and the DSG `comp1DWeights` pass the boundary flag too.
- Files: new `operators/quadrature/quadrature_rules/{default_rules,getters}/fejer_first_quadrature_rule{,_getter}.{hpp,cpp}` (442 lines incl. Doxygen). Changed: `node_generation_function.{hpp,cpp}`, the 3 default families' headers (`using` / overrides) + cheb1 `.cpp`, `quadrature_weight_table.{hpp,cpp}`, `textbook_quadrature.{hpp,cpp}`, `deltas_sparse_grid_quadrature_tools.cpp`, `quadrature.hpp` (docs), `perf_micro.cpp`. Docs: `module-reference.md`, `python-bindings.md`, CLAUDE.md §6 (exceptions in OpenMP regions / `getQuadRule` overload). `combigrid.i` unchanged: the `%ignore` of `getQuadRule` covers both overloads.

## Gain vs. cost
- Gain: none intended (bug fix + feature). Judged suite neutral (29 cases, all `total` within noise): suite 9.227 → 9.266 s (0.996x), `expensive/` 11.676 → 11.683 s (0.999x). Rule cost: cheb1 ≈ 4.8x CC at equal n (n = 131073: 29.9 vs. 6.25 ms; n = 2^20 + 1: 330 vs. 68 ms). SG quadrature at 1 thread: cheb1 / CC d2 L16 156 / 112 ms, d3 L8 3.41 / 3.33 ms, b1 d5 L6 4.80 / 4.76 ms.
- Memory: unchanged (geomean B/A 1.003, 0 `MEM+` / `MEM-`; largest 1.013x, 12.4 → 12.6 MiB). The Bluestein rule allocates 3 × 16·M bytes transiently per row (M = next power of two ≥ n; 96 MiB at a level-20 row).
- API / functionality: agreed by the owner. cheb1 quadrature in `quadrature` (scalar / vector), `textbookQuadrature` and Python. New additive virtual `getQuadRule(n, includeBoundary)` (owner 2026-11-22). Missing rules throw instead of terminating. Existing results bitwise (`grid_storage_pinning`, CC bitwise pins).
- Maintainability / readability: +≈ 160 / −52 lines in existing files; rule methods ≤ 30 lines; the math is documented in the class and method Doxygen.

## Tests
- Pinning tests (passed on the unchanged code, debug 3×): `grid_storage_pinning` (bitwise SG / DSG quadrature hashes, debug + optimized), `quadrature_weights` (incl. `Clenshaw_Curtis_Weights_Bitwise`, rule weights bitwise in TGs), CC rule tests, `equidistant_nodes` / `clenshaw_curtis_nodes`, `vector_quadrature`, `vector_delta_quadrature`, `delta_quadrature`, `textbook_operators`.
- New / extended tests:
  - `fejer_first_quadrature_rule` (7 cases): mpmath references (60 digits, moment system) for n 3 / 6 and boundary 3 / 4 / 9 to 4e-16; Chebyshev-moment exactness up to degree n − 1 for n 1–80, both variants; positivity (Fejér; inner weights with boundary), bitwise symmetry, Σ = 1 up to n = 131071; FFT vs. naive (incl. primes 8191, 2047); family → rule mapping (equi / CC unchanged pointers).
  - `first_type_chebyshev_quadrature` (3 cases): CT exactness on the product polynomials of a TG's exactness space for d 1–4, all level sums, b0 / b1 / no boundary, unit / shifted domains, plus an anisotropic MIVec grid; scalar, vector m = 2 and textbook. A family without a rule throws `not_implemented_exception` at 1 and 16 threads (scalar, vector, cached option; nothing cached).
  - `textbook_operators`: cheb1 now `quadrature` == `textbookQuadrature` (scalar + vector m = 3) and the same evaluation points.
  - `repeated_operator_calls`: cheb1 quadrature included.
  - The cached-source-function tests keep cheb1 without quadrature: there it is coupled to building a DSG.
- Results: debug `test --repeat 3` ✓ (v1 and final) · normal `--repeat 3` ✓ · ASan full suite ✓ · fallbacks: Armadillo-only (+ ASan) and no FFT backend, rule / cheb1 / textbook / weight suites ✓ · Python `results/WP-83/python_checks.py` ✓ at 1 and 16 threads (WP-79 checks still ✓).

## Reference check
- WARN at 1 and 16 threads, 145 OK: only the known owner-accepted `interp/sg/equi/b0/d2/L5/cos-shifted` (×2.34), identical on WP-83-before. The reference suite has no cheb1 quadrature case.

## Benchmarks (A = WP-83-before, B = WP-83-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread; `^micro/(quadrule|quad)/` without vector cases; 16 common cases, sum 634.57 → 634.31 ms, 1.000x):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| quad/operator/cc/b0/d2/L16 | 112.44 ms | 112.31 ms | 0.915 | [0.711, 1.176] |
| quad/process_all_tgs/cc/b1/d10/L5 | 44.34 ms | 44.49 ms | 0.995 | [0.991, 0.999] (SLOWER, unchanged kernel) |
| quadrule/cc_weights/n33 | 987.8 ns | 966.4 ns | 1.026 | [1.005, 1.047] (unchanged code) |
| new: quadrule/cheb1_weights/n33 · n1025 · n32769 · n131073 · n1048577 | – | 3.91 µs · 115 µs · 7.28 ms · 29.9 ms · 330 ms | – | – |
| new: quadrule/fejer1_weights/n131071 · n1048577 | – | 28.6 ms · 708 ms | – | – |

L2/L3 e2e (16 threads; `^(quad|mivec)/sg/|^expensive/(quad|mivec)/`, 29 cases, the DSG `expensive/` cases included): 0 `total` / `operator` outside noise; 1 SLOWER `generation` (unchanged code, ms-scale page faults):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d10/L4 | total | 1.306 s | 1.307 s | 0.998 | [0.992, 1.004] |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.399 s | 2.398 s | 1.000 | [0.992, 1.008] |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.476 s | 2.476 s | 1.000 | [0.996, 1.003] |
| quad/sg/cc-uq/b0/d10/L4 | generation | 1.99 ms | 2.25 ms | 0.704 | [0.508, 0.976] |

Sum of medians: suite A = 9.227 s, B = 9.266 s (0.996x, within noise); `expensive/` 11.676 / 11.683 s; cached 576.5 / 577.4 ms · Full reports: `combigrid/perf/results/WP-83/`

Peak memory: no flags; largest B/A `expensive/mivec/sg/.../c100us-ramp` 12.4 → 12.6 MiB (1.013); `quad/sg/cc-uq/b0/d2/L20` 195.4 → 190.6 MiB (0.975, spread 5 %). Geomean B/A = 1.003 · process floor A = 10.3 MiB, B = 10.0 MiB

The e2e "result sanity" line (`quad/sg/equi-exp/b1/d15/L6` 3.1e-9 → 8.3e-9) is the order-dependent 16-thread `reduction(+)`: A's own 5 rounds differ by up to 5e-7 absolute (WP-61's topic). The bitwise pins are at 1 thread.

## Attempts not kept
- v1 Bluestein with one `Eigen::FFT` per transform and `fft.inv`: 3 twiddle tables per row. The final variant uses one object and only forward transforms. v1 → final (`results/WP-83/micro-variant-single-plan.md`): cheb1 weights 1.08–1.26x, fejer1 1.11–1.28x, cheb1 SG d2 L16 operator 161.7 → 157.6 ms (1.027x); CC / equi neutral.
- Enabling cheb1 quadrature in the cached-source-function tests: reverted, because their `withQuadrature` flag also builds a DSG, which cheb1 cannot have.

## Profile shift / follow-ups
- Rule cost (v1 profile, n = 131073): kissfft butterflies 43 %, sincos 22 %. Left: K + 1 sincos for the final chirp factor (they could be copied from the chirp at +8n bytes), 3 transient buffers. Matters only for 1D / 2D cheb1 grids at levels ≥ 15. A plan or weight cache across rows / calls is not worth it (rows are computed once per table, cross-call cache).
- Equi / CC for levels below an offset ≥ 2: their closed rules are applied to inner-only nodes (Simpson on {¼, ½, ¾}: x² off by 0.0625). The convergence order is kept, but `quadrature` ≠ ∫ of the interpolant there. Fix = override the new overload with the open / interpolatory rules (changes results at b ≥ 2 → owner decision). Also relevant for dimension-adaptive refinement with offsets.
- Audit of throws inside the SG / DSG quadrature regions after the fix:
  - user C++ source functions that throw from `evaluate` → `std::terminate`. Python callables are captured, and the cache defers its errors to `finishOperatorCall`.
  - `bad_alloc` of per-thread scratch.
  - Other operators: SG global interpolation calls `getInterpolationMethod` inside its region (`fillBasisValues`; built-in families never throw, user families could). A generic capture-and-rethrow around the TG loops would cover all of these (cost to measure).
- Dimension-adaptive refinement: rules are per (n, includeBoundary) and stateless, and weight rows per (dim, level) are independent. Adding levels only adds rows, so nothing blocks step-by-step refinement. A refinement loop that rebuilds the `QuadWeightTable` recomputes every row: cheap except for cheb1 / CC rows at levels ≥ 15.
- Case realism: no judged e2e / reference case uses cheb1 quadrature, so the feature is judged by its micro cases only. A protected `quad/sg/cheb1-*` reference case would need the owner.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
