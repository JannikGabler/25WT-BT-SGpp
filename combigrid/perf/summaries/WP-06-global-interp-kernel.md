# WP-06 — Global interpolation: per-tensor-grid kernel of the `SparseGrid` operator

Status: PROPOSED · Date: 2026-11-13 · Base commit: 266193bc6
Recommendation: accept. `^interp/sg/` saves 1.47 s (5.67 → 4.20 s, 1.35x); the d ≥ 7 cases save 0.63 s; cc is 1.23–1.32x, cheb1 / equi 2.96–5.33x. The kernel shrinks to about 30 lines because it reuses the quadrature row loop and runs at the integrand-only floor for d ≥ 5. The only cost is a refcheck WARN on the known Lebesgue-~1e16 case. The non-finite-value behavior change was decided in the session (owner: option A, keep and document; see API).
Machine: `perf.py calibrate` ratios e2e 1.004 / 0.999, micro 1.000 / 0.998 (burst / sustained, before the measurements; session start 1.011 / 1.000, 0.995 / 0.996)

## Change
- On a tensor grid, the interpolant at x is Σ_p Π_k L_{k,p_k}(x_k) f(p). The new `tools::global_interpolation::processTensorGrid`:
  - computes the 1D basis values once per tensor grid and dimension (`InterpolationMethod::basisValues`, WP-45), from the nodes the TG stores (no regeneration);
  - then runs the quadrature row loop with those basis values in place of the weights (`quadrature::mapNodesOntoDomain` / `setUpIterDims` / `sumOverRows`).
  - This gives premapped nodes (`mapToDomain`, the same bits as before), odometer rows with no div/mod, dimensions with one node skipped (basis value exactly 1), one per-thread scratch (`misc::GlobalInterpScratch` = `QuadratureScratch` + one node vector) and no per-TG allocations.
  - `hasTrivialResult` now runs once per dimension, not per 1D call, and the slice copies are gone.
  - For cheb1 / equi, the generic O(n²) barycentric weights are now computed once per TG and dimension instead of once per 1D call (part of WP-07's lever).
- Driver: `interpolate(SourceFunc)` uses `parallel` + `for nowait` with one scratch per thread. The vector path goes through `combineTensorGrids`, like the quadrature. It remains a per-TG combination technique (no dedup).
- Quadrature (internal only): `mapNodesOntoDomain` now takes the domain instead of the context, since it used only `context.domain`. `sumOverRows` gets explicit instantiations for both value types and Doxygen. The old allocating `processTensorGrid(f, point, tgData, genInstr[, out])` remain as wrappers (used by tests and micro cases). `getInterpolationCntPerDim`, `interpolateFirstDim`, `interpolateLaterDim` and `processTensorGridInternal` are removed.
- Files:
  - Source: `operators/global_interpolation/global_interpolation.cpp`, `tools/operators/global_interpolation/global_interpolation_tools.{hpp,cpp}`, `tools/operators/quadrature/quadrature_tools.{hpp,cpp}`, new `miscellaneous/data_scratches/global_interpolation_scratch.hpp`.
  - Tests and benchmarks: new `tests/operators/global_interpolation/interpolation_kernel_pinning_test.cpp`; `perf_micro.cpp` (+ `micro/interp/sg/cheb1/b0/d5/L4`).
  - Docs: `claude/module-reference.md` (data_scratches, design note "Global interpolation"); Doxygen of `operators/global_interpolation/{global_interpolation,deltas_sparse_grid_global_interpolation}.hpp` (file brief + non-finite note).

## Gain vs. cost
- Gain (16 t, `total`):
  - cc: d10 1.978 → 1.595 s (1.230x), d7 1.255 → 1.004 s (1.241x), d5 1.442 → 1.126 s (1.280x), d3 1.306x, d2 1.321x.
  - Generic formula: cheb1 d5 344 → 117 ms (2.96x), cheb1 d3 5.33x, equi d3 4.30x.
  - Expected for cc was 1.12–1.24x / 0.64 s. The measured gain is above that because the basis values also replace the per-slice barycentric formula (n divisions per slice → one FMA per point).
- Memory: geomean B/A 0.975.
  - MEM-: cc d10 24.3 → 12.1 MiB (no per-TG result vectors in thread arenas), cc d7 13.2 → 12.1 MiB.
  - MEM+: cc d2 15.2 → 16.1 MiB. The per-thread scratch is sized for the largest Σ n_k (8k nodes at d2 L12, ×2 arrays ×16 threads).
- API / functionality: public API and evaluation counts/points are unchanged. The internal tools API changed (scratch overloads).
  - **Behavior change (owner decision 2026-11-13: option A, keep and document):** if the source function returns inf/NaN at a grid point and the query lies on a node in some dimension, the old trivial 1D path returned the node value and ignored the other values. Now they enter with weight 0, and 0·inf gives NaN.
    - Example: log(x₀) + x₁, cc b0 L4, query (0.5, 0.3): before −0.393 (exact), after NaN.
    - The old robustness was partial and depended on the order of the dimensions: (0.3, 0.5) was NaN before as well, and every off-node query was NaN in both versions.
    - The DSG operator (WP-45) already behaved this way.
    - Documented as `@note` in `global_interpolation.hpp` (scalar + vector) and `deltas_sparse_grid_global_interpolation.hpp`.
    - Rejected options: (B) fix the dimensions whose basis values are a unit vector (old behavior regardless of dimension order, n_k times fewer evaluations at on-node queries; changes evaluation counts; DSG would differ) — possible follow-up; (C) a per-point branch on weight 0 in the shared row loop.
  - Python: the doc notes touch umbrella headers (comments only). Rebuilt with `SG_PYTHON=1` (OPT=0) and ran all 11 examples in `combigrid/examples/python/` ✓. `lib/` now holds that OPT=0 build: run `perf.py build normal` before any measurement.
- Maintainability: the tools `.cpp` has about the same length (148 → 144 lines). The kernel itself is two calls (prepare + `sumOverRows`), and the rest is wrappers and the vector driver. The collapse templates and slice copies are gone, and quadrature and interpolation share one row loop. Code size: scalar `sumOverRows` is shared (0x71d B, out of line before as well). The vector instantiation is now a function of its own instead of being inlined into the vector quadrature kernel.

## Tests
- Pinning (green on the unchanged code, debug ×3):
  - `interpolation_pinning/Global_Interpolation_Golden_Values` (d1–4, 1e-12 relative);
  - `vector_global_interpolation/*`, `{equidistant,clenshaw_curtis}_node_interpolation/*`, `dsg_global_interpolation` tests, quadrature suites.
- New `interpolation_kernel_pinning/*` (3 cases):
  - golden values of the old kernel for equi / cc / cheb1, b0 / b1, unit / shifted domain, complete d5 L2/L3, d7 L1/L2, d10 L2 and anisotropic MIVec d5 / d6;
  - 3 query points (generic, on a node, on the lower boundary), scalar and vector (m = 3);
  - tolerance 64 eps·Σ|c|·Σ|L f| (round-off pin, not bitwise);
  - every grid point of every TG evaluated exactly once, at bitwise `mapToDomain` points.
  - Measured deviation from the goldens: old normal build max 0.28 units (vs. the OPT=0 recording); new kernel max 1.44, mean 0.18 units; vector = scalar golden within 1.46.
- Results: debug `test --repeat 3` ✓ · normal ×3 ✓ · ASan ✓ (8 GB cap).

## Reference check
- WARN at 1 and 16 threads (131 OK, `results/WP-06/refcheck.txt`): `interp/sg/equi/b0/d2/L5/cos-shifted`, value 3: 1.63e-6 → 3.80e-6 (x2.34). This is the documented Lebesgue-~1e16 case.
  - The other values: 5 shrank (8.2e-6 → 6.2e-6), 7 shrank (0.324 → 0.264), the rest are equal or within 1e-12.
  - Summed error 0.444 (ref) / 0.467 (before) → 0.396 (after). The error is round-off-dominated, and any reordering moves it.

## Benchmarks (A = WP-06-before, B = WP-06-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread, `results/WP-06/micro.md`, 55 cases):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|
| interp/global_tg cc d10 L2 / d5 L6 / d2 L12 | 46.67 / 1.12 / 1.36 ms | 39.27 / 0.912 / 1.01 ms | 1.215 / 1.229 / 1.341 | all > 1 |
| interp/global_tg cheb1 d5 L4 / equi d3 L5 | 516 / 121 µs | 244 / 70 µs | 2.124 / 1.734 | > 1 |
| interp/sg cc d8 L2 / d5 L5 / d2 L12; cheb1 d5 L4 (new); equi d3 L5 | 155.2 / 74.0 / 16.6; 41.5; 3.26 ms | 126.7 / 61.7 / 13.1; 18.6; 1.11 ms | 1.225 / 1.201 / 1.300; 2.237; 2.943 | > 1 |
| interp/global_tg_vec m1 / m4 (cc d10, d5, d2; cheb1) | | | 1.19–2.05 / 1.06–1.28 | > 1 |
| interp/sg_vec cc d5 / d2 m4 | 261.7 / 55.3 ms | 245.9 / 52.7 ms | 1.064 / 1.051 | > 1 |
| floor `sol/interp_tg_integrand` cc d10 / d5 / cheb1 (unchanged) | – | 38.9 ms / 887 µs / 227 µs | B at 1.01 / 1.03 / 1.07x of floor | |
| controls: interp/dsg (5), quad/process_tg (5), process_all_tgs (2) | | | 0.948–1.011 | noise (cc d2 L16 1.008 faster) |
| quad/process_tg_vec equi d10 L4 m1 (vector quadrature) | 37.42 ms | 37.76 ms | **0.991** | [0.987, 0.995] SLOWER |

The vector-quadrature 0.991 has −0.3 % instructions, +1 % cycles and equal op-cache misses (`perf stat`, 2 runs each): placement or loop codegen of the now out-of-line `sumOverRows<VectorValues>` (one call per TG). The m4 / d2 / all-TGs vector cases are within noise. It is a feature path, not in the judged suite: minimal.

L2/L3 e2e (16 threads, `results/WP-06/e2e.md`, `^(interp/|quad/sg/)`, 27 cases, 5 rounds; operator ≈ total):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d10/L2/p8 | total | 1.978 s | 1.595 s | 1.230 | [1.195, 1.266] |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.255 s | 1.004 s | 1.241 | [1.217, 1.266] |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.442 s | 1.126 s | 1.280 | [1.267, 1.294] |
| interp/sg/cc/b0/d3/L9/p64 | total | 243.3 ms | 186.1 ms | 1.306 | [1.280, 1.333] |
| interp/sg/cc/b0/d2/L12/p64 | total | 174.3 ms | 130.7 ms | 1.321 | [1.276, 1.368] |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 344.5 ms | 116.5 ms | 2.955 | [2.917, 2.994] |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 182.7 ms | 34.2 ms | 5.330 | [5.252, 5.408] |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.0 ms | 11.9 ms | 4.295 | [4.252, 4.339] |
| interp/dsg/* (6), quad/sg/* (13) | total | | | 0.975–1.019 | all noise (dsg d10 1.006 "faster") |

Sum of medians (total, 27 cases): A = 15.691 s, B = 14.282 s, saved = 1.409 s (1.099x); interp/sg only: 5.671 → 4.205 s. Two `generation` SLOWER flags (interp cc d10 35 → 43 µs, d5 109 → 144 µs) are in untouched code at µs scale (page-fault noise).

Peak memory: geomean B/A = 0.975; MEM- cc d10 24.3 → 12.1 MiB, cc d7 13.2 → 12.1 MiB; MEM+ cc d2 15.2 → 16.1 MiB · process floor A = 10.5 MiB, B = 10.2 MiB

## Attempts not kept
- Nested collapse with premapped nodes, odometer and reused scratch (the hypothesis' kernel, = WP-36 "sol", measured in the same A/B as `micro/sol/interp_tg`): cc d10 41.4 / d5 0.999 / d2 1.05 ms, cheb1 480 µs. The basis-value kernel is 5–10 % (cc) and 2x (cheb1) faster, so the nested variant was not implemented in production.

## Profile shift / follow-ups
- interp/sg cc is now at the integrand floor (L1 1.01–1.03x at d ≥ 5); only fewer evaluations help: the DSG overload (WP-45) or multi-point (WP-09).
- WP-07: the remaining generic cost is per TG and dimension: O(n²) log weights + allocation + the mutex in `getBarycentricFormula`. The gain left is smaller than estimated (cheb1 d5 now 117 ms).
- RP-1's Clang-vs-GCC note on the first-dimension collapse is moot: the code is replaced by the quadrature row loop.
- Strict aliasing: the replaced kernel had no pre-WP-14 workarounds; `sumOverRows`' local copies stay (opaque `evaluate` calls).
- Stale pre-existing cpplint warnings in `quadrature_tools.{hpp,cpp}` (TODO without username, missing final newline).
- **New WP requested by the owner (2026-11-13): generalize the shared weighted-sum kernels.** Refactoring only: no API change, no Python impact, results bitwise unchanged; check benchmarks for neutrality (code placement). Code shared by several operators is named and placed as quadrature code:
  - SG (since this WP): `tools::quadrature::sumOverRows` / `mapNodesOntoDomain` / `setUpIterDims` and `misc::QuadratureScratch` are also the kernel of the global interpolation; `misc::GlobalInterpScratch` wraps `QuadratureScratch`.
  - DSG (since WP-45): `tools::dsg_quadrature::processDeltaTensorGrid` (+ `prepareDeltaTensorGrid`, `compSubTrie`, `updateCoeffSums`, `compRowWeights`), `misc::DeltaQuadratureScratch` and `ProjectedSubTrieBuilder` (`tools/operators/quadrature/`) also run the DSG global interpolation.
  - `dsg_quadrature::mapNodeIdx` / `NodeIdxLvlMapFunc` / `compNodeIdxLvlMapFuncs` (delta → regular node index) is DSG structure and is used by `DSGGlobalInterpIndex`.
  - Proposal: move the weighted-sum kernels and their scratches to an operator-neutral place, e.g. `tools/operators/weighted_sum/` with names like `processWeightedTensorGrid`, `WeightedSumScratch`, `DeltaWeightedSumScratch`. Move the node-index mapping next to the DSG generation tools. Docs should say that quadrature (quadrature weights) and global interpolation (1D basis values) both use these.
  - Quadrature-specific stays in place: `comp1DWeights`, `combineDeltaTensorGrids`, quadrature rules, `getWeights`.
  - Already neutral, no change: `GenericDSGOperatorContext`, `CTCoeffTrie`, `OperatorDataCache`, `FlatNestedVec`, `ClosureTrie`.
  - The DSG linear interpolation shares nothing with the other DSG operators.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep combigrid/tests/operators/global_interpolation/interpolation_kernel_pinning_test.cpp`
to keep the new pinning tests)
