# combigrid module reference

On-demand reference for [CLAUDE.md](../../CLAUDE.md): directory layout, key classes, design
rationale, vocabulary, base dependencies, known limitations. The code wins over this file —
verify signatures before relying on them.

## Directory layout

Under [combigrid/src/sgpp/combigrid/](../src/sgpp/combigrid/) (`.lint`/`.os`/`.gcda` files without
a matching source are stale build artifacts):

```
constants.hpp          parallelism thresholds per subsystem (mi_vec, sg_gen_instr, ct_coefficients,
                       sg_gen_node_lookup); source_func_cache (shards, arena chunk, default limits,
                       spill batch, records per LMDB transaction, ADAPTIVE_* factors / windows)
type_defs.hpp          LvlType, GPCntType, CTCoeffType, MI/MIVec aliases, HyperCubeArea
functions/
  level_to_grid_point_count_functions/  linear / doubling growth (level → #inner nodes)
  node_generation_functions/            1D node families; default_functions/ (concrete),
                                        getter/ (static singletons)
  source_functions/                     SourceFunc, VectorSourceFunc (m outputs; virtual
                                        end-of-operator hook); caching/ (always compiled):
                                        SourceFuncCache, SourceFuncCacheConfig, CachePolicy,
                                        AdaptiveCacheMode, CacheStatistics, CachedSourceFunc,
                                        CachedVectorSourceFunc
grids/
  tensor_grid            anisotropic tensor grid (per-dim node lists)
  sparse_grid_base       common base: nDim, vector<TensorGridCTData> (not SWIG-wrapped)
  sparse_grid            CT sparse grid (+ optional shared SGGenInstr)
  deltas_sparse_grid     delta representation for the DSG quadrature and linear interpolation
                         (every grid point in exactly one delta tensor grid; not in the umbrella
                         header / Python); holds an OperatorDataCache (lazily built operator data)
  extensive_sparse_grid  commented out
miscellaneous/         (namespace misc)
  tensor_grid/           TensorGridCTData {mi, coefficient, tensorGrid}
  ct_coefficient_trie/   CTCoeffTrie: prefix tree over the tensor grids' level MIs (DSG quadrature)
  operator_data_cache/   OperatorDataCache: thread-safe, type-keyed store of lazily built
                         query-independent operator data of a grid (DeltasSparseGrid::
                         getOperatorData; e.g. DSGLinInterpIndex), so the grid's header names no
                         operator type
  closure_trie/          ClosureTrie: trie over a downwards-closed MI set, MI -> dense rank by one
                         lookup per dimension (DSG linear interpolation; reusable for other DSG
                         operators)
  data_contexts/         read-only data of one operator call (QuadratureContext, DeltaQuadratureContext;
                         DSGLinInterpIndex (per grid) and DSGLinInterpQuery (per call) of the DSG
                         linear interpolation)
  data_scratches/        per-thread scratch (quadrature, delta quadrature, linear interpolation, DSG
                         linear interpolation)
  operators/value_types/ ScalarValues / VectorValues: value types the operator kernels are
                         templates over (scalar vs. m outputs)
  flat_nested_vector/    FlatNestedVec(View): ragged nested vectors in one contiguous array
  iterators/             TensorGridIterator (grid points + MIs without materializing the grid)
  bounding_boxes/        discrete rectangular / unit boxes over MI grids + iterators
  caching/               boost_point_value_table/: BoostPointValueTable (sharded table of the
                         source-function cache) and its parts in namespace
                         misc::boost_point_value_table: Shard (spin lock, counters, RecordArena,
                         boost::unordered_flat_set of record ids), Probe, RecordHash (XXH3 stored
                         in the record), RecordEqual; lmdb_point_value_store/: LMDBPointValueStore
                         (disk store of the storage policies, LMDB file) and its parts in
                         misc::lmdb_point_value_store (FileInterface = its pimpl, FileHeader,
                         ReadSlot); record_layout.hpp: misc::source_func_caching::RecordLayout.
                         Library-free headers (WP-40): SourceFuncCache, the table and the store
                         forward-declare the next layer (table, store; Shard; FileInterface);
                         only shard, file_interface, read_slot (and tools/caching/lmdb_tools,
                         dependencies_defines) include Boost.Unordered / xxhash.h / lmdb.h or
                         test configuration macros, and only .cpp files include those;
                         adaptive/: AdaptiveCacheController (state machine of
                         CachePolicy::Adaptive) and its parts in misc::adaptive_caching: CostTimer
                         (rdtscp / steady_clock, calibrated), CostSample(Kind), CostSampleWindow
                         (lock-free 64-slot windows, CostSampleSlot = sequence lock),
                         CostEstimate, HitRateTracker, AdaptiveCounters, AdaptiveRuleInput,
                         adaptive_rules (nextAdaptiveMode: pure, tested with injected costs)
  exceptions/            CacheFileException (namespace sgpp::combigrid; file_exception owning
                         its message)
  concurrency/           ConcurrentTaskQueue (node-lookup pipeline, commented out), SpinLock
  hashing/               std::pair hasher
  vector_map/            VectorMap (tiny linear-search map)
  multiindex_vector_lookup*, multiindex_lookup_equal   MI → index hash map
multiindices/          MI<T>, MIVec<T> (flat array ~ matrix), views, references, iterators, hashes,
                       lexicographic less
operators/
  global_interpolation/  interpolate(); methods/ (InterpolationMethod, barycentric formula,
                         optimized barycentric formula for Chebyshev 2nd kind, getters)
  linear_interpolation/  interpolateLinear() for SparseGrid (linear_interpolation.cpp) and
                         DeltasSparseGrid (deltas_sparse_grid_linear_interpolation.cpp)
  quadrature/            quadrature() for SparseGrid (quadrature.cpp) and DeltasSparseGrid
                         (deltas_sparse_grid_quadrature.cpp); quadrature_rules/ (Clenshaw-Curtis,
                         Simpson, trapezoidal, getters); fast_quadrature.* commented out
sparse_grid_generation_instructions/   SGGenInstr, CompleteSGGenInstr, MIVecSGGenInstr
tools/                 (namespace tools)
  combitech_coefficients/               computeCTCoeffs, computeCTCoeffSingle
  sparse_grid_generation/               SparseGrid / DeltasSparseGrid generation, node lookup
  sparse_grid_generation_instructions/  MI generation of the complete sparse grid
  operators/             per-tensor-grid kernels of the operators: quadrature/ (quadrature_tools
                         (SG), deltas_sparse_grid_quadrature_tools (DSG internals),
                         ProjectedSubTrieBuilder (DSG)), global_interpolation/
                         (global_interpolation_tools, namespace tools::global_interpolation;
                         interpolation_methods/: generic barycentric weights),
                         linear_interpolation/ (linear_interpolation_tools, namespace
                         tools::lin_interpolation; dsg_linear_interpolation_tools, namespace
                         tools::dsg_lin_interpolation)
  downwards_closedness, paretoMaxima, multiindex_bounding_box_generation, multiindex_domination
  multiindex/, multiindex_view/, multiindex_vector/ (component-wise max)   MI helpers
  math/ (binomial, pow, ceil), comparison/ (nearly_equal), data_vector/ (normalization
  [0,1]^d ↔ domain; mapToDomain = std::fma, the one unit → domain mapping of all operators),
  hashing/ (FNV-1a), collections/ (k-way unique merge), functions/ (equidistant
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
  evaluates every grid point once, with global weights from a `CTCoeffTrie`;
  `interpolateLinear(const SourceFunc&, const DataVector&, const DeltasSparseGrid&)` (+ vector
  overloads; [deltas_sparse_grid_linear_interpolation.hpp](../src/sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp))
  evaluates every distinct corner of the combination tensor grids once per call (WP-47): a corner
  coordinate is identified by the level at which it first brackets the query coordinate, a corner
  by a slot of a per-call store (one block per closure MI, `DSGLinInterpIndex`, built on the first
  call and kept in the grid); three passes in one parallel region (mark the corners of all
  tensor grids, evaluate the marked slots, gather per tensor grid with the `SparseGrid` kernel's
  weights and order), serial below `DSG_PARALLEL_MIN_CORNER_VISITS`. C++ only.
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
  wraps `std::function<double(const base::DataVector&)>`; maps `[0,1]^d → domain` before calling it
  (`tools::mapToDomain`). End-of-operator hook: every operator calls the virtual
  `finishOperatorCall()` once at its end (empty for plain functions, overridden by the cached
  classes; never per evaluation). Same for `VectorSourceFunc`. Virtual destructor, defaulted copy /
  move; copying or assigning a subclass object (slicing) is a deleted overload
  (`tools::StrictSubclassOf`, [tools/concepts/](../src/sgpp/combigrid/tools/concepts/)).
- Caching ([functions/source_functions/caching/](../src/sgpp/combigrid/functions/source_functions/caching/),
  WP-26; works if Boost ≥ 1.81 + xxHash ≥ 0.8 were found when building, see
  `tools/defines/dependencies_defines.hpp`; otherwise the `SourceFuncCache` constructor, and with it
  every cached function, throws `base::dependency_exception` naming the reason):
  `CachedSourceFunc(func, cache | SourceFuncCacheConfig{})` / `CachedVectorSourceFunc(func, m, …)`
  are final subclasses holding the cache (`shared_ptr`): the stored callable looks the point up in
  the `SourceFuncCache` and calls `func` on misses only; `finishOperatorCall()` is overridden to
  notify the cache. Operators and plain functions are unchanged (design W); they must be passed by
  reference (copying into a plain `SourceFunc` does not compile). Keys = bit patterns of the user-domain point (identical across operators, grids,
  refinement steps and calls); one cache per function (m fixed by the first cached function,
  another m throws; d by the first lookup, other d bypass); NaN values are not stored;
  `SourceFuncCacheConfig::memoryLimit` (default 1 GiB) is a soft limit split over `nShards` shards
  (power of two in [1, 64], default 64; at the limit: stop inserting); `statistics()` (lookups,
  hits, inserts, rejectedInserts, entries, bytes, operatorCalls); `clear()`. Misuse throws
  `std::invalid_argument`. Table: `misc::BoostPointValueTable` (8 (d + m + 1) bytes per record +
  index).
  Pays for functions ≳ 0.3 µs (WP-25 break-even) and Python callables (hits skip the GIL).
  Storage policies (WP-28, `SourceFuncCacheConfig::policy` = `CachePolicy::MemoryOnly` (default) |
  `TemporaryStorage` | `PersistentStorage`, fields `file`, `storageSpaceLimit`; need LMDB ≥ 0.9.14,
  linked by SCons if found (`SGppConfigure.detectLMDB` → `SGPP_COMBIGRID_LMDB_LINKED`), else they
  throw `base::dependency_exception`): memory stays primary (records + indices ≤ `memoryLimit`);
  beyond it, values go to an LMDB file (`misc::LMDBPointValueStore`: key = XXH3 of the point,
  value = point + values; per-shard batches of 256 records, found in the shard meanwhile) up to
  `storageSpaceLimit` (then one `std::cerr` warning, `rejectedInserts`); misses read the file only
  once it holds points memory does not. Persistent: `file` created or loaded at construction
  (header magic / version / key scheme / d / m, size vs. last page, every key = hash of its point;
  mismatch or damage → `base::file_exception`; exclusive `flock`; LMDB lock file `file-lock`);
  every `finishOperatorCall()` writes the call's new values (sorted, one transaction per 64Ki
  records, `MDB_NOSYNC`: crash-safe against program crashes); `flush()` also `fsync`s; the
  destructor writes like the hook (no `fsync`). Temporary: unnamed file (created + unlinked at construction in `$TMPDIR`,
  else `/var/tmp`), `config().file = /proc/self/fd/N`, holds only the values beyond memory.
  Points of another d than the file's / first lookup's throw at the end of the operator call.
  `clear(deletePersistentFile = false)`: persistent + false keeps the file (values found from
  disk). Statistics + `diskHits`, `diskEntries`, `loadedEntries`, `fileBytes`. Costs (WP-28, 1
  thread, d5 / d20): disk hit 0.58 / 0.73 µs (memory 0.09 / 0.17), new point persistent 1.0–1.5
  / 2.2–3.2 µs (memory 0.17), spill 3.7 / 4.9 µs, load 0.16 / 0.29 µs per record; LMDB writes are
  serialized. No `functionId`: a file of another function with the same d, m is used silently.
  Adaptive policy (WP-27, `CachePolicy::Adaptive`, decisions:
  [perf/results/WP-27/decisions.md](../perf/results/WP-27/decisions.md)): picks
  `AdaptiveCacheMode` None / MemoryOnly (start) / TemporaryStorage from sampled costs. One point in
  P (hash bits 32.., P = 2^k from the calibrated timer cost: rdtscp 37 ns → 64; steady_clock with
  HPET 1.9 µs → 2048) is timed (hit, miss + function alone, disk); every 64 samples the completing
  thread decides (`try_lock`): c_f vs. E = (h c_hit + (1 − h) o_miss) / h per tier, factors 0.8
  (→ None) / 1.25 (probe stays) / 1.5 (→ TemporaryStorage, memory full) / 1.2 (→ MemoryOnly) in
  `constants.hpp`; h = exact marginal hit rate of the last 2^14 lookups, used from 2^16 lookups on
  (before: bound h = 1, cold start never switches off); while the file is in use only its rule
  applies (chain None ← MemoryOnly ← TemporaryStorage). Switches act at once (also mid-call;
  results bitwise the same). None: the callable checks the mode word and calls the function (cost-0
  integrands incl. probes, steady state: 0.82–0.91x of plain at 1 thread, 0.85–1.09x at 16;
  one fresh call 0.71–0.86x / 0.57–0.82x vs. "always" 0.14–0.30x / 0.07–0.17x; WP-27); left only by probes at the hook (after 1, 2, 4, … 64 calls in
  None; the call that switched off does not count). Each mode uses only its tiers; the temporary
  file is created at the first value beyond memory in TemporaryStorage (inside the parallel region,
  under the cache mutex; failure → MemoryOnly for good and one `base::file_exception` at the end of
  that operator call) and kept until destruction; `config().file` stays empty. Other d bypass
  silently. `CacheStatistics`: `adaptiveMode`, `modeSwitches`, `noneOperatorCalls`,
  `functionCostNs`, `memoryHitCostNs`, `memoryMissOverheadNs`, `diskHitCostNs`,
  `diskMissOverheadNs`. Works without LMDB (never TemporaryStorage). Memory hits stay inline in
  the callable (an out-of-line hit path cost +17 ns per evaluation).
- `VectorSourceFunc` ([functions/source_functions/vector_source_function.hpp](../src/sgpp/combigrid/functions/source_functions/vector_source_function.hpp)):
  f: Ω → ℝ^m; wraps `std::function<void(const base::DataVector&, std::span<double>)>` + `nOutputs`
  (m ≥ 1; 0 throws `base::data_exception`); `nOutputs()`, `evaluate(point, out)`,
  `evaluateNormalizedInPlace(point, area, out)`. `out.size()` is only asserted (no per-evaluation
  check). All operators take it through separate overloads; their kernels are templates over
  `ScalarValues` / `VectorValues` (no runtime dispatch, scalar path unchanged). The global
  interpolation interpolates all m outputs per 1D step through the multi-output overload
  `InterpolationMethod::interpolate(pos, nodes, values /* n × m, node-major */, m, out)` (default:
  the scalar formula per output; both barycentric formulas override it: factors once per node).
  Output k equals the scalar run of component k within round-off (not bitwise). For m = 1 use
  `SourceFunc` (the vector path is slower there). Python: `VectorSourceFunc(callable, nOutputs)`
  (callable returns a sequence of m numbers; [python-bindings.md](python-bindings.md)).
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
  `quadrature(sourceFunc, sg)` (SparseGrid or DeltasSparseGrid); vector-valued (all operators):
  `interpolate(vectorSourceFunc, point, sg, out)`, `interpolateLinear(…, out)`,
  `quadrature(vectorSourceFunc, sg, out)` (`std::span<double>` of size m, wrong size throws
  `std::invalid_argument`), each also returning a `base::DataVector` (no `out`; the overloads
  Python binds). OpenMP over tensor grids in the drivers (`quadrature.cpp`,
  `deltas_sparse_grid_quadrature.cpp`, `global_interpolation.cpp`, `linear_interpolation.cpp`);
  the per-tensor-grid kernels live in `tools/operators/` (`tools::quadrature::processTensorGrid`,
  `tools::dsg_quadrature::processDeltaTensorGrid`, `tools::global_interpolation::processTensorGrid`,
  `tools::lin_interpolation::processTensorGrid`; scalar and vector overloads around one template
  kernel each); generation and CT coefficients are parallel above the thresholds in
  `constants.hpp`.
- `tools::computeCTCoeffs(miVec)`: CT coefficients of a downwards-closed MI set.

## Design rationale (thesis §5)

- **Two domains**: nodes, weights and interpolation live on [0,1]^d. The user domain Ω = ∏[a_k, b_k]
  enters only (1) in function evaluations (affine map `std::fma(u_k, b_k−a_k, a_k)`,
  `tools::mapToDomain`, used by all operators so they pass bitwise identical points; WP-26) and (2)
  as the factor |Ω| of quadrature results. `SourceFunc` + `SGGenInstr::setDomain()` handle it.
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
- **Vector-valued source functions in Python** (WP-29d): `VectorSourceFunc` and the
  `DataVector`-returning overloads of all three operators are bound; the DSG operators (quadrature
  and linear interpolation, also the scalar ones) and the `std::span` overloads are C++ only. Python callables run serially under the
  GIL; a vector callable needs one call per grid point for all m outputs.
- **Source-function caching** (WP-26, WP-28, WP-27): policy "always" (`CachedSourceFunc`; "none"
  = plain `SourceFunc`) in memory or with a temporary / persistent LMDB file, or adaptive
  (`CachePolicy::Adaptive`: None / MemoryOnly / TemporaryStorage from measured costs; not the
  default; never persistent; decisions are myopic: a function at hit rate 0 in the current
  operator (DSG quadrature) is switched off even if later calls would reuse its values). Unavailable (constructors throw) without Boost ≥ 1.81 +
  xxHash ≥ 0.8 (storage policies: also LMDB). Disk writes cost µs per new point and are
  serialized (one LMDB writer), so files pay only for expensive functions. Without a cache,
  shared points are re-evaluated across tensor grids and operator calls. Python misses still run
  serially under the GIL.
- **Reduced representation**: `SparseGrid` operators process full tensor grids, so shared points are
  evaluated repeatedly (Tasmanian / SGMK win at d ≳ 7 for this reason); the DSG quadrature
  evaluates every point once, the DSG linear interpolation every distinct corner once per call
  (no global interpolation on the DSG yet).
- One global `boundaryLevelOffset` for all dimensions; no Leja nodes.
- `USE_DAKOTA` is declared combigrid-only in SConstruct but unused in `combigrid/src/` (dormant).

## Thesis

Markdown conversion in [bachelor-thesis/](bachelor-thesis/) (read-only; its
[README.md](bachelor-thesis/README.md) has a "what to read for which task" table). It predates
refactors; known discrepancies (code is truth): `setBoundaryUsage()` → `setBoundaryLevelOffset(LvlType)`;
`MIVecSGGenInstr` takes `const LvlMIVec&` (not `shared_ptr`); node getters return `NodeGenFunc*`
(not `shared_ptr`).
