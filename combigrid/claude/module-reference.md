# combigrid module reference

On-demand reference for [CLAUDE.md](../../CLAUDE.md): directory layout, key classes, design
rationale, vocabulary, base dependencies, known limitations. The code wins over this file —
verify signatures before relying on them.

## Directory layout

Under [combigrid/src/sgpp/combigrid/](../src/sgpp/combigrid/) (`.lint`/`.os`/`.gcda` files without
a matching source are stale build artifacts):

```
constants.hpp          parallelism thresholds per subsystem (mi_vec, sg_gen_instr, ct_coefficients,
                       sg_gen_node_lookup)
type_defs.hpp          LvlType, GPCntType, CTCoeffType, MI/MIVec aliases, HyperCubeArea
functions/
  level_to_grid_point_count_functions/  linear / doubling growth (level → #inner nodes)
  node_generation_functions/            1D node families; default_functions/ (concrete),
                                        getter/ (static singletons)
  source_functions/                     SourceFunc, VectorSourceFunc (m outputs)
grids/
  tensor_grid            anisotropic tensor grid (per-dim node lists)
  sparse_grid_base       common base: nDim, vector<TensorGridCTData> (not SWIG-wrapped)
  sparse_grid            CT sparse grid (+ optional shared SGGenInstr)
  deltas_sparse_grid     delta representation for the DSG quadrature (every grid point in exactly
                         one delta tensor grid; not in the umbrella header / Python)
  extensive_sparse_grid  commented out
miscellaneous/         (namespace misc)
  tensor_grid/           TensorGridCTData {mi, coefficient, tensorGrid}
  ct_coefficient_trie/   CTCoeffTrie: prefix tree over the tensor grids' level MIs (DSG quadrature)
  data_contexts/         read-only data of one operator call (QuadratureContext, DeltaQuadratureContext)
  data_scratches/        per-thread scratch (quadrature, delta quadrature, linear interpolation)
  operators/value_types/ ScalarValues / VectorValues: value types the operator kernels are
                         templates over (scalar vs. m outputs)
  flat_nested_vector/    FlatNestedVec(View): ragged nested vectors in one contiguous array
  iterators/             TensorGridIterator (grid points + MIs without materializing the grid)
  bounding_boxes/        discrete rectangular / unit boxes over MI grids + iterators
  concurrency/           ConcurrentTaskQueue (node-lookup pipeline)
  hashing/               std::pair, DataVector hashers
  vector_map/            VectorMap (tiny linear-search map)
  multiindex_vector_lookup*, multiindex_lookup_equal   MI → index hash map
multiindices/          MI<T>, MIVec<T> (flat array ~ matrix), views, references, iterators, hashes,
                       lexicographic less
operators/
  global_interpolation/  interpolate(); methods/ (InterpolationMethod, barycentric formula,
                         optimized barycentric formula for Chebyshev 2nd kind, getters)
  linear_interpolation/  interpolateLinear()
  quadrature/            quadrature() for SparseGrid (quadrature.cpp) and DeltasSparseGrid
                         (deltas_sparse_grid_quadrature.cpp); quadrature_rules/ (Clenshaw-Curtis,
                         Simpson, trapezoidal, getters); fast_quadrature.* commented out
sparse_grid_generation_instructions/   SGGenInstr, CompleteSGGenInstr, MIVecSGGenInstr
tools/                 (namespace tools)
  combitech_coefficients/               computeCTCoeffs, computeCTCoeffSingle
  sparse_grid_generation/               SparseGrid / DeltasSparseGrid generation, node lookup
  sparse_grid_generation_instructions/  MI generation of the complete sparse grid
  operators/             quadrature_tools (SG), deltas_sparse_grid_quadrature_tools (DSG internals),
                         ProjectedSubTrieBuilder (DSG), generic barycentric formula
  downwards_closedness, paretoMaxima, multiindex_bounding_box_generation, multiindex_domination
  multiindex/, multiindex_view/, multiindex_vector/ (component-wise max)   MI helpers
  math/ (binomial, pow, ceil), comparison/ (nearly_equal), data_vector/ (normalization
  [0,1]^d ↔ domain), hashing/ (FNV-1a), collections/ (k-way unique merge), functions/ (equidistant
  node utilities), os/, concurrency.hpp (range partitioning), iterators.hpp, benchmarking/
```

Benchmarks live outside `src/` in [combigrid/benchmarks/](../benchmarks/) (`perf/` = the
optimization-phase harness, see [combigrid/perf/README.md](../perf/README.md)).

## Key classes / entry points

- `SparseGrid` ([grids/sparse_grid.hpp](../src/sgpp/combigrid/grids/sparse_grid.hpp)): top-level
  container (`nDim`, list of `TensorGridCTData`, optional shared `SGGenInstr`); constructible from an
  `SGGenInstr`. Common parts live in `SparseGridBase`.
- `DeltasSparseGrid` ([grids/deltas_sparse_grid.hpp](../src/sgpp/combigrid/grids/deltas_sparse_grid.hpp)):
  constructed from an `SGGenInstr`; `quadrature(const SourceFunc&, const DeltasSparseGrid&)`
  ([deltas_sparse_grid_quadrature.hpp](../src/sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp))
  evaluates every grid point once, with global weights from a `CTCoeffTrie`. C++ only.
- `TensorGrid` ([grids/tensor_grid.hpp](../src/sgpp/combigrid/grids/tensor_grid.hpp)): per-dim counts
  + flat per-dim node lists, first-dimension-fastest linear indexing.
- `TensorGridCTData`
  ([miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp](../src/sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp)):
  `{LvlMI mi, CTCoeffType coefficient, TensorGrid tensorGrid}`.
- `SGGenInstr`
  ([sparse_grid_generation_instructions/sg_gen_instruction.hpp](../src/sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp)):
  abstract generator holding domain, per-dim `NodeGenFunc*`, per-dim `Lvl2GPCntFunc`, boundary level
  offset; subclasses implement `genFullMIVec()` / `genReducedMIVec()` (+ `…WithCoeffs()`) / `clone()`.
  - `CompleteSGGenInstr(maxLvl, nDim)`: `|ℓ|₁ ≤ maxLvl`; closed-form coefficient `(−1)^q · C(d−1, q)`
    with `q = maxLvl − |ℓ|₁`, so zero-coefficient MIs are never materialized.
  - `MIVecSGGenInstr(const LvlMIVec&)`: external MI set (entry point for adaptive schemes). Stores a
    **reference** to the input (caller keeps it alive). On construction: (1) downwards closure by
    recursive enumeration over the dimensions (colexicographic order; since WP-03), (2)
    `tools::computeCTCoeffs()` (dimension-separated O(|I|·d)), (3) drops zero-coefficient MIs.
- `SourceFunc` ([functions/source_functions/source_function.hpp](../src/sgpp/combigrid/functions/source_functions/source_function.hpp)):
  wraps `std::function<double(const base::DataVector&)>`; maps `[0,1]^d → domain` before calling it.
  No caching.
- `VectorSourceFunc` ([functions/source_functions/vector_source_function.hpp](../src/sgpp/combigrid/functions/source_functions/vector_source_function.hpp)):
  f: Ω → ℝ^m; wraps `std::function<void(const base::DataVector&, std::span<double>)>` + `nOutputs`
  (m ≥ 1; 0 throws `base::data_exception`); `nOutputs()`, `evaluate(point, out)`,
  `evaluateNormalizedInPlace(point, area, out)`. `out.size()` is only asserted (no per-evaluation
  check). Operators (so far both quadratures) take it through separate overloads; their kernels are
  templates over `ScalarValues` / `VectorValues` (no runtime dispatch, scalar path unchanged).
  Output k equals the scalar run of component k within round-off (not bitwise). For m = 1 use
  `SourceFunc` (the vector path is slower there).
- `NodeGenFunc`
  ([functions/node_generation_functions/node_generation_function.hpp](../src/sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp)):
  abstract 1D node family; also returns the matching 1D `QuadRule*` and `InterpolationMethod*`.
  Getters (raw pointers to process-lifetime singletons — never delete or wrap):
  `getEquidistantNodeGenFunc()` (Simpson / trapezoidal by parity),
  `getClenshawCurtisNodeGenFunc()` = `getSecondTypeChebyshevNodeGenFunc()` (same singleton; CC
  quadrature, closed-form barycentric weights), `getFirstTypeChebyshevNodeGenFunc()` (no closed-form
  quadrature).
- `Lvl2GPCntFunc`: function pointer; `linearLvl2GPCntFunction` (ℓ+1),
  `doublingLvl2GPCntFunction` (2^{ℓ+1}−1 inner nodes, CC-compatible; the Doxygen comment has a typo,
  the code is `tools::pow(2, lvl+1) - 1`).
- Operators: `interpolate(sourceFunc, point, sg)`, `interpolateLinear(sourceFunc, point, sg)`,
  `quadrature(sourceFunc, sg)` (SparseGrid or DeltasSparseGrid); vector-valued (quadrature only so
  far, both grid types): `quadrature(vectorSourceFunc, sg, out)` (`std::span<double>` of size m,
  wrong size throws `std::invalid_argument`) and `base::DataVector quadrature(vectorSourceFunc,
  sg)`. OpenMP over tensor grids in the drivers (`quadrature.cpp`,
  `deltas_sparse_grid_quadrature.cpp`, `global_interpolation.cpp`, `linear_interpolation.cpp`);
  generation and CT coefficients are parallel above the thresholds in `constants.hpp`.
- `tools::computeCTCoeffs(miVec)`: CT coefficients of a downwards-closed MI set.

## Design rationale (thesis §5)

- **Two domains**: nodes, weights and interpolation live on [0,1]^d. The user domain Ω = ∏[a_k, b_k]
  enters only (1) in function evaluations (affine map `(b_k−a_k)·u_k + a_k`) and (2) as the factor
  |Ω| of quadrature results. `SourceFunc` + `SGGenInstr::setDomain()` handle it.
- **Node generation**: `NodeGenFunc` receives the number of *inner* nodes; `SGGenInstr` appends
  boundary nodes per `boundaryLevelOffset`. Level → count (`Lvl2GPCntFunc`) and node positions are
  independent abstractions.
- **On-demand assembly**: `TensorGrid` stores only 1D node lists (O(d·n) instead of O(n^d)); d-dim
  points are assembled on demand (`getGridPoints(DataMatrix&)`, iterators). Main memory advantage over
  other frameworks.
- **Global interpolation**: dimension-wise barycentric formula of the 2nd kind, d nested 1D
  interpolations (innermost = dim 1). Weights in log space; the generic formula (equidistant,
  Chebyshev 1st kind) recomputes them in O(n²) per 1D call (not cached); Chebyshev 2nd kind uses
  closed-form weights. Given the weights, O(n) per dimension and point.
- **Quadrature rules** per dimension by node type and parity: equidistant odd → composite Simpson,
  even → trapezoidal; Chebyshev 2nd kind → Clenshaw-Curtis (Waldvogel FFT with `USE_EIGEN` /
  `USE_ARMADILLO`, else O(n²)). Result × |Ω|.

## Vocabulary

| Concept | Code | Notes |
|---|---|---|
| level multi-index | `LvlMI` = `MI<LvlType>` | 0-based; `|ℓ|₁` = 1-norm |
| grid-point multi-index | `GPMI` = `MI<GPCntType>` | position within a tensor grid |
| downwards-closed set | `LvlMIVec` | all component-wise predecessors present |
| Pareto maximum | `tools::paretoMaxima()` | no other MI ≥ it component-wise |
| CT coefficient | `CTCoeffType` = `int` | can be negative; `TensorGridCTData::coefficient` |
| boundary level offset | `setBoundaryLevelOffset(k)` | boundary nodes in dim j only if ℓ[j] ≥ k; 0 = always |

Integer types (`LvlType`, `GPCntType`, `CTCoeffType`) are `unsigned int` / `int` in
[type_defs.hpp](../src/sgpp/combigrid/type_defs.hpp); widen for higher levels / dimensions.

## Base-module dependencies

`combigrid/SConscript` declares only `sgppbase`. Used: `base::DataVector` (points, node lists),
`base::DataMatrix` (`TensorGrid::getGridPoints`), `base::not_implemented_exception`; tests also use
`base::RandomNumberGenerator` (deterministic fuzz tests). None of base's hierarchical grid / basis /
operation machinery.

## Known limitations (future work, thesis §8)

- **Dimension-adaptive refinement** is not implemented; the caller builds the loop on
  `MIVecSGGenInstr` (highest-priority future work; planned by the owner).
- **Vector-valued source functions only partly implemented**: `VectorSourceFunc` works with the
  `SparseGrid` and `DeltasSparseGrid` quadrature (C++ only; perf phase WP-29a/b). Interpolation,
  linear interpolation and the Python bindings follow (WP-29c–d); until then one `SourceFunc` /
  operator call per output there.
- **No source-function caching**: shared points are re-evaluated across tensor grids and operator
  calls. Opt-in design chosen (cache in the callable stored in a plain `SourceFunc`), not
  implemented ([PLAN.md](../perf/PLAN.md) WP-26–WP-28).
- **Reduced representation**: `SparseGrid` operators process full tensor grids, so shared points are
  evaluated repeatedly (Tasmanian / SGMK win at d ≳ 7 for this reason); only the DSG quadrature
  evaluates every point once.
- One global `boundaryLevelOffset` for all dimensions; no Leja nodes.
- `USE_DAKOTA` is declared combigrid-only in SConstruct but unused in `combigrid/src/` (dormant).

## Thesis

Markdown conversion in [bachelor-thesis/](bachelor-thesis/) (read-only; its
[README.md](bachelor-thesis/README.md) has a "what to read for which task" table). It predates
refactors; known discrepancies (code is truth): `setBoundaryUsage()` → `setBoundaryLevelOffset(LvlType)`;
`MIVecSGGenInstr` takes `const LvlMIVec&` (not `shared_ptr`); node getters return `NodeGenFunc*`
(not `shared_ptr`).
