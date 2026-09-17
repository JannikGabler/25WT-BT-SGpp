# CLAUDE.md — Working on the SG++ /combigrid/ module

Scope of this file: pragmatic context for editing, building and testing the
Combination Technique (CT) module in [combigrid/](combigrid/). Everything
else (other SG++ modules, python/java/matlab bindings, framework internals) is
touched only insofar as it affects work inside `combigrid/`.

---

## 1. Project overview

SG++ is a C++ HPC framework for **sparse grids** (see [README.md](README.md),
https://sgpp.github.io/SGpp/). It supports both classical hierarchical
sparse grids and the **dimensionally-adaptive sparse-grid combination
technique** (CT) — the subject of this module.

The CT approximates a function on a sparse grid as a linear combination of
(anisotropic) full tensor grids,
`f ≈ Σ_ℓ c_ℓ · f_ℓ`,
where each `f_ℓ` lives on the tensor grid of level multi-index `ℓ` and
`c_ℓ ∈ ℤ` is the combination coefficient. This lets one avoid the curse
of dimensionality without inheriting the hierarchical machinery of a
"real" sparse grid.

Key mathematical facts (thesis §2):
- Level multi-indices `ℓ ∈ ℕ^d` are **0-based** in this codebase. A level-ℓ tensor grid has `n_k(ℓ[k])` nodes in dimension k as given by `Lvl2GPCntFunc`. With `doublingLvl2GPCntFunction`, n_k(ℓ) = 2^{ℓ+1}−1 inner nodes.
- **Complete sparse grid** with max level L: keep tensor grid ℓ iff `L−(d−1) ≤ |ℓ|₁ ≤ L`; CT coefficient = `(−1)^{L−|ℓ|₁} · C(d−1, L−|ℓ|₁)`.
- **Downwards-closedness**: a multi-index set I ⊆ ℕ^d where for every ℓ ∈ I all component-wise predecessors (ℓ' ≤ ℓ) are also in I. `MIVecSGGenInstr` always computes this closure automatically unless `assumeDownwardsClosedness=true`.
- CT is **exact** for functions in the space spanned by the hierarchical increments of all tensor grids in the sparse grid (thesis §2.4). Outside this space, accuracy degrades gracefully with the maximum level.

Repository is organized as **one directory per module**:

| module | purpose | relevance for CT work |
|---|---|---|
| [base/](base/) | data types, grids, basis functions, utilities | dependency (see §3) |
| [combigrid/](combigrid/) | this module — CT | **primary** |
| [pde/](pde/), [solver/](solver/), [quadrature/](quadrature/), [optimization/](optimization/), [datadriven/](datadriven/), [misc/](misc/) | other math/ML functionality | not relevant |
| [pysgpp/](pysgpp/), [jsgpp/](jsgpp/), [matsgpp/](matsgpp/) | Python/Java/MATLAB SWIG bindings | Python bindings of the CT module live in [combigrid/build/pysgpp/](combigrid/build/pysgpp/) (see §11); Java/MATLAB expose nothing of CT |
| [tools/](tools/) | build/CI/lint helpers | style checks |
| [site_scons/](site_scons/) | shared SCons Python helpers | build system |

---

## 2. The combigrid module

Purpose: build and evaluate CT sparse-grid approximations for
interpolation and quadrature. Directory layout under
[combigrid/src/sgpp/combigrid/](combigrid/src/sgpp/combigrid/):

```
constants.hpp                    tunable thresholds for parallel dispatch
type_defs.hpp                    LvlType, GPCntType, MI/MIVec aliases, HyperCubeArea
functions/
  level_to_grid_point_count_functions/   linear / doubling growth rules (level → #nodes)
  node_generation_functions/             1D node families (equidistant, Chebyshev, CC)
    default_functions/                     concrete NodeGenFunc subclasses
    getter/                                accessors that return the (static) singletons
  source_functions/                      SourceFunc wrapper around a user callable
grids/
  tensor_grid.hpp/.cpp                   anisotropic tensor-product grid (per-dim node lists)
  sparse_grid.hpp/.cpp                   CT sparse grid = vector<TensorGridCTData> + gen. instr.
miscellaneous/
  tensor_grid/                           TensorGridCTData struct (mi + coefficient + grid)
  caching/                               source-function cache, persistent cache helpers
  hashing/                               std::pair, DataVector hashers
  multiindex_vector_lookup.hpp           MI → index hash map used by SGGenInstr
  bounding_boxes/                        discrete rectangular boxes over MI grids
multiindices/
  multiindex.hpp                         MI<T>: fixed-length integer tuple template
  multiindex_vector.hpp                  MIVec<T>: flat array of MIs, ~ a matrix
  multiindex_hash.hpp / *_hash.hpp       hashing utilities for MIs
operators/
  global_interpolation/                  tensor-product global interp. (barycentric, ...)
    methods/                               interpolation_method.hpp + concrete rules
  linear_interpolation/                  multilinear (2^d-corner) interpolation
  quadrature/                            tensor-product quadrature
    quadrature_rules/                      Clenshaw-Curtis, etc.
sparse_grid_generation_instructions/
  sg_gen_instruction.hpp                 abstract SGGenInstr base
  complete_sg_gen_instruction.hpp        standard "|ℓ|_1 ≤ L" scheme
  multiindex_vector_sg_gen_instruction.hpp  bring-your-own MI set (adaptive schemes)
  (full_grid_/full_/multiindex_set_ variants exist but the .cpp/.hpp are absent —
   only .lint / .os files remain; treat as deprecated / not built)
tools/
  combitech_coefficients/                computeCTCoeffs, computeCTCoeffSingle
  sparse_grid/                           sparse_grid_generation.{cpp,hpp}, node_lookup
  operators/                             per-operator scratch buffers (QuadScratch, ...)
  multiindex/                            multiindex_utilities (unique, dominates, ...)
  multiindex_vector/                     component-wise max, downwards closure helpers
  math/                                  binomial, power, ceil
  data_vector/                           normalization ([0,1]^d ↔ domain)
  comparison/                            nearly_equal (float tolerance)
  concurrency.{cpp,hpp}                  small thread-pool / task-queue used internally
  downwards_closedness.{cpp,hpp}         set predicates & downward-closure completion
  paretoMaxima.{cpp,hpp}                 pareto-front over MIVec
  hashing/                               fnv_1a_hash
  benchmarking/                          µ-benchmarks driver code
```

### Key classes / entry points

- `sgpp::combigrid::SparseGrid` — [combigrid/src/sgpp/combigrid/grids/sparse_grid.hpp](combigrid/src/sgpp/combigrid/grids/sparse_grid.hpp). The top-level container: `nDim`, list of `TensorGridCTData`, optional shared `SGGenInstr`. Constructible directly from an `SGGenInstr`.
- `sgpp::combigrid::TensorGrid` — [combigrid/src/sgpp/combigrid/grids/tensor_grid.hpp](combigrid/src/sgpp/combigrid/grids/tensor_grid.hpp). Anisotropic Cartesian grid: per-dim counts + flat per-dim node lists, row-major (first-dim-fastest) linear indexing.
- `sgpp::combigrid::TensorGridCTData` — [combigrid/src/sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp](combigrid/src/sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp). `{LvlMI mi, CTCoeffType coefficient, TensorGrid tensorGrid}`.
- `sgpp::combigrid::SGGenInstr` — [combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp](combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp). Abstract generator: holds domain, per-dim `NodeGenFunc*`, per-dim `Lvl2GPCntFunc`, boundary-level offset; subclasses implement `genMIVec()` / `genMIVecWithCoeff()` / `clone()`.
- Concrete generators:
  - `CompleteSGGenInstr(maxLvl, nDim)` — standard `|ℓ|₁ ≤ maxLvl` scheme; uses the closed-form CT coefficient formula `(−1)^q · C(d−1, q)` where `q = maxLvl − |ℓ|₁`, so zero-coefficient MIs are never materialized.
  - `MIVecSGGenInstr(const LvlMIVec&)` — external MI set (entry point for adaptive schemes). Stores a **reference** to the input vector (caller keeps it alive). On construction it: (1) computes the downwards closure via Pareto-maxima bounding-box scan, (2) calls `tools::computeCTCoeffs()` (dimension-separated O(|I|·d) algorithm), (3) drops zero-coefficient MIs. Pass `assumeDownwardsClosedness=true` to skip step 1 when the input is already closed (thesis §4.5.5; existence of this flag is unverified in actual .hpp — check before using).
- `sgpp::combigrid::SourceFunc` — [combigrid/src/sgpp/combigrid/functions/source_functions/source_function.hpp](combigrid/src/sgpp/combigrid/functions/source_functions/source_function.hpp). Wraps a `std::function<double(const base::DataVector&)>`; can normalize `[0,1]^d ↔ domain` and is intended to cache (toggled by `constants::source_func::USE_CACHE`) but not properly implemented yet.
- `sgpp::combigrid::NodeGenFunc` — [combigrid/src/sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp](combigrid/src/sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp). Abstract 1D node family; also returns the matching 1D `QuadRule*` and `InterpolationMethod*`. Concrete singletons obtained via `get*NodeGenFunc()` helpers under `functions/node_generation_functions/getter/`:
  - `getEquidistantNodeGenFunc()` — uniform nodes on [0,1], selects Simpson or trapezoidal quadrature depending on parity.
  - `getClenshawCurtisNodeGenFunc()` / `getSecondTypeChebyshevNodeGenFunc()` — aliased; Chebyshev 2nd-kind nodes + CC quadrature + closed-form barycentric weights.
  - `getFirstTypeChebyshevNodeGenFunc()` — Chebyshev 1st-kind nodes (distinct from CC; no closed-form quadrature).
  - All getters return `NodeGenFunc*` (raw pointer to a process-lifetime static singleton). Do not delete or wrap in `shared_ptr`.
- `Lvl2GPCntFunc` — free function pointer type; ready-made: `linearLvl2GPCntFunction` (= ℓ+1), `doublingLvl2GPCntFunction` (= 2^{ℓ+1}−1, inner nodes only, no boundary). The doubling function is the correct CC-compatible growth; despite the comment in the Doxygen header having a typo, the code itself is `tools::pow(2, lvl+1) - 1`.
- Operators (free functions taking a `SparseGrid` + `SourceFunc`):
  - `interpolate(sourceFunc, point, sg)` — global tensor-product interp. ([operators/global_interpolation/global_interpolation.hpp](combigrid/src/sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp)).
  - `interpolateLinear(sourceFunc, point, sg)` — multilinear ([operators/linear_interpolation/linear_interpolation.hpp](combigrid/src/sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp)).
  - `quadrature(sourceFunc, sg)` — sparse-grid integral ([operators/quadrature/quadrature.hpp](combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.hpp)).
- `tools::computeCTCoeffs(miVec)` — CT coefficients for a downwards-closed MI set ([tools/combitech_coefficients/combitech_coefficients.hpp](combigrid/src/sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp)).

### Design rationale (thesis §5, summary)

- **Two-domain design**: all node positions, quadrature weights, and interpolation are computed on the internal unit domain [0,1]^d. The user domain Ω = [a_k, b_k] is used in exactly two places: (1) function evaluations apply the affine map T(u)[k] = (b_k−a_k)·u_k + a_k before calling the user function; (2) quadrature results are multiplied by |Ω| = ∏(b_k−a_k). Users never transform coordinates manually — `SourceFunc` + `SGGenInstr.setDomain()` handles it.
- **Node generation abstraction**: `NodeGenFunc` always receives the count of *inner* nodes only (never boundary). Boundary nodes are appended separately by `SGGenInstr` based on `boundaryLevelOffset`. This means the level↔node-count mapping (`Lvl2GPCntFunc`) and the actual node position generation are independent, swappable abstractions.
- **On-demand tensor grid assembly**: `TensorGrid` permanently stores only 1D node distributions per dimension. The d-dimensional grid-point tuples are assembled on demand (via `getGridPoints(DataMatrix&)`). Memory cost: O(d·n) per tensor grid vs O(n^d) for full materialization — at d=10, level=5, this is several orders of magnitude. This is the primary memory advantage over other frameworks.
- **Global interpolation**: dimension-wise barycentric interpolation of the 2nd kind. Weights are pre-computed in **log space** to avoid overflow/underflow and cached per node distribution. For Chebyshev 2nd-kind nodes, closed-form weights are used directly (no O(n²) computation needed). Evaluation is O(n) per dimension per interpolation point. The d-dim computation reduces to d nested 1D interpolations (innermost = dim 1) applied to the tensor of function values.
- **Downwards-closure computation**: finds Pareto maxima of the input `LvlMIVec`, constructs the component-wise bounding box, then includes each MI in the bounding box iff it is ≤ at least one Pareto maximum. Located in `tools/paretoMaxima.hpp` + `tools/downwards_closedness.hpp`.
- **Quadrature rule selection** happens automatically per dimension based on node type and parity: equidistant + odd #nodes → composite Simpson (O(h⁴) error); equidistant + even #nodes → trapezoidal (O(h²) error); Chebyshev 2nd kind → Clenshaw-Curtis via Waldvogel FFT (O(n log n) weight computation). The result is then multiplied by the domain volume |Ω|.

### Typical CT computation

Example ([combigrid/examples/c++/global_interpolation/sin_func_full_sg_interpolation.cpp](combigrid/examples/c++/global_interpolation/sin_func_full_sg_interpolation.cpp)):

```cpp
CompleteSGGenInstr genInstr(/*maxLvl=*/10, /*nDim=*/3);
genInstr.setDomain({0, 1});                                 // [0,1]^3
genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
genInstr.setBoundaryLevelOffset(0);
const SparseGrid sg(genInstr);                              // materialize tensor grids
const double val = interpolate(SourceFunc(myFunc), point, sg);
```

For adaptive / custom MI sets, use `MIVecSGGenInstr(miVec)` instead of
`CompleteSGGenInstr` — see [combigrid/examples/c++/global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp](combigrid/examples/c++/global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp).

Umbrella header: [combigrid/src/sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp). Includes exactly the public (Python-bound) API headers; the SWIG wrapper is compiled against it (§11). C++ code may still include the specific headers it needs (as the examples do).

### Terminology / vocabulary map

| Concept | Code name | Notes |
|---|---|---|
| level multi-index | `LvlMI` = `MI<LvlType>` = `MI<unsigned int>` | 0-based; `|ℓ|₁` is the 1-norm |
| grid-point multi-index | `GPMI` = `MI<GPCntType>` | indexes positions within a tensor grid |
| downwards-closed MI set | `LvlMIVec` after `downwardsClosure()` | all predecessor MIs must be present |
| Pareto maximum | `tools::paretoMaxima()` | MI p where no other MI ≥ p component-wise |
| CT coefficient | `CTCoeffType` = `int` (can be negative!) | stored per TG in `TensorGridCTData::coefficient` |
| sparse grid generation blueprint | `SGGenInstr` | captures node type, domain, boundary offset |
| boundary level offset | `setBoundaryLevelOffset(k)` | adds boundary nodes in dim d only when ℓ[d] ≥ k; 0 = always include boundary |
| on-demand assembly | `TensorGrid::getGridPoints(DataMatrix&)` | assembles d-dim points lazily from stored 1D distributions |
| source function | `SourceFunc` | wraps `std::function<double(const DataVector&)>`, handles domain mapping |

### Umbrella / catch-all constants

- Tunable parallelism thresholds live in [combigrid/src/sgpp/combigrid/constants.hpp](combigrid/src/sgpp/combigrid/constants.hpp) — one namespace per subsystem (`mi_vec`, `sg_gen_instr`, `ct_coefficients`, ...). Change these before benchmarking parallel scaling.
- Integer types (`LvlType`, `GPCntType`, `CTCoeffType`) are `unsigned int` / `int` in [combigrid/src/sgpp/combigrid/type_defs.hpp](combigrid/src/sgpp/combigrid/type_defs.hpp); widen when using higher levels or larger dimensions.

---

## 3. Dependencies on the base module

The CT module declares only `sgppbase` in [combigrid/SConscript](combigrid/SConscript). In practice, exactly these `base/` symbols are included from `combigrid/`:

- `sgpp::base::DataVector` — [base/src/sgpp/base/datatypes/DataVector.hpp](base/src/sgpp/base/datatypes/DataVector.hpp): the pervasive 1D double array used for grid points, node lists, coordinates.
- `sgpp::base::DataMatrix` — [base/src/sgpp/base/datatypes/DataMatrix.hpp](base/src/sgpp/base/datatypes/DataMatrix.hpp): used by `TensorGrid::getGridPoints(base::DataMatrix&)` to materialize all grid points.
- `sgpp::base::not_implemented_exception` — [base/src/sgpp/base/exception/not_implemented_exception.hpp](base/src/sgpp/base/exception/not_implemented_exception.hpp).

Combigrid tests additionally include `sgpp::base::RandomNumberGenerator` ([base/src/sgpp/base/tools/RandomNumberGenerator.hpp](base/src/sgpp/base/tools/RandomNumberGenerator.hpp)) to make fuzz tests deterministic.

None of `base/`'s hierarchical grid, basis, or operation machinery is used — CT is largely self-contained.

---

## 4. Build

Build system: **SCons** (Python), driven by the top-level [SConstruct](SConstruct) and per-module [SConscript](combigrid/SConscript) files. Common module logic (source discovery, library building, boost tests, style checks) lives in [site_scons/ModuleHelper.py](site_scons/ModuleHelper.py); configuration/detection in [site_scons/SGppConfigure.py](site_scons/SGppConfigure.py).

Modules are enabled/disabled via `SG_ALL=<bool>` (default `1`) and per-module `SG_<NAME>=<bool>` overrides (`SG_BASE`, `SG_COMBIGRID`, …). Python bindings via `SG_PYTHON=<bool>`.

### Preferred iteration commands (combigrid only)

Wrapper scripts checked into repo root — read them if in doubt:
- [compileNormal.sh](compileNormal.sh) — optimized: `scons -j8 CPPFLAGS="-march=native -flto=auto" LINKFLAGS="-march=native -flto=auto" OPT=1 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1`
- [compileDebug.sh](compileDebug.sh) — debug: `scons -j8 OPT=0 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1`
- [compileASan.sh](compileASan.sh) — AddressSanitizer: adds `-fsanitize=address -fno-omit-frame-pointer` to both `CPPFLAGS` and `LINKFLAGS`, `OPT=0`.

Minimal one-shot command (equivalent to `compileDebug.sh` without native tuning):

```bash
scons -j$(nproc) OPT=0 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1
```

Add `VERBOSE=1` to see compiler/linker commands, `-c` to clean. All build variables and their descriptions: `scons -h`.

### After building

- Shared libs land in [lib/](lib/): `libsgppbase.so`, `libsgppcombigrid.so`.
- To run built binaries directly, `export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH` (see [INSTRUCTIONS](INSTRUCTIONS)).
- Compilation database is emitted to [compile_commands.json](compile_commands.json). (The `*.os`, `*.gcda`, `*.lint` files scattered throughout the tree are SCons build artifacts / coverage data / cpplint markers — leave them alone; `scons -c` cleans them.)

### Common pitfalls

- Setting `SG_ALL=0` **without** setting `SG_COMBIGRID=1` **and** `SG_BASE=1` disables everything → SCons errors with "You must enable at least one module".
- The default `SG_ALL=1` will try to build every module (needs Boost, SWIG, GSL, Eigen, Armadillo, …). Stick to the `SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1` pattern to iterate on CT quickly.
- Style checks (`CHECK_STYLE=1`, default) will "touch" a `.lint` companion for every `.cpp`/`.hpp` on success. This is why every source file has a matching `.lint` sibling in the tree.
- Custom flags via `CPPFLAGS=`/`LINKFLAGS=` are appended, not overridden.
- Build variables persist in [buildVars.out](buildVars.out) via SCons's `vars.Save`.

### Optional deps documented for `combigrid`

- `USE_DAKOTA=1` (default off): the SConstruct declares this as `sgpp::combigrid`-only, and [combigrid/doc/doxygen/install_dakota.doxy](combigrid/doc/doxygen/install_dakota.doxy) describes integration. There is currently **no `Dakota`/`.cpp`/`.hpp` use** in `combigrid/src/` — flag exists but is dormant. **Unverified** whether it is still meant to be wired up.

---

## 5. Testing

Framework: **Boost.Test** (dynamic-link, `BOOST_TEST_DYN_LINK`). All test translation units under [combigrid/tests/](combigrid/tests/) are compiled into a single executable named `test_combigrid_boost` next to them. Entry point / `BOOST_TEST_MODULE` definition: [combigrid/tests/test_base.cpp](combigrid/tests/test_base.cpp).

Test layout mirrors src layout: [grids/](combigrid/tests/grids/), [operators/](combigrid/tests/operators/), [sparse_grid_generation_instructions/](combigrid/tests/sparse_grid_generation_instructions/), [multiindex_vector/](combigrid/tests/multiindex_vector/), [miscellaneous/](combigrid/tests/miscellaneous/), [node_generation_functions/](combigrid/tests/node_generation_functions/), [tools/](combigrid/tests/tools/).

### Running

- Build and run everything (only combigrid):
  ```bash
  scons -j$(nproc) SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 \
        COMPILE_BOOST_TESTS=1 RUN_BOOST_TESTS=1
  ```
  `RUN_BOOST_TESTS=1` is the default — building `combigrid` triggers a test run through SCons unless disabled.
- Skip execution but still compile: `RUN_BOOST_TESTS=0`.
- Skip both: `COMPILE_BOOST_TESTS=0` (also skips style + boost linkage).
- Directly re-run without rebuild: `./combigrid/tests/test_combigrid_boost --log_level=test_suite`. Use standard Boost.Test filters (`--run_test=SuiteName/CaseName`, `--list_content`) to narrow down.

Boost headers/lib paths configurable via `BOOST_INCLUDE_PATH=` / `BOOST_LIBRARY_PATH=` (defaults: `/usr/include`, `/usr/lib/x86_64-linux-gnu` on Linux).

### Adding a new test

1. Drop a new `*.cpp` under the appropriate subdirectory of [combigrid/tests/](combigrid/tests/) — `ModuleHelper.buildBoostTests()` walks the directory and picks it up automatically. No SConscript edit required.
2. **Do not** define `BOOST_TEST_MODULE` (already done in [test_base.cpp](combigrid/tests/test_base.cpp)); just include `<boost/test/unit_test.hpp>` and use `BOOST_AUTO_TEST_SUITE(...)` / `BOOST_AUTO_TEST_CASE(...)`.
3. Do `#define BOOST_TEST_DYN_LINK` at the top; existing tests follow this pattern.
4. Include SG++ headers with **angle brackets** and **absolute** paths (`#include <sgpp/combigrid/...>`) — the include-style checker rejects `"..."` and relative paths.
5. Add the copyright banner (see §7).

Python tests: `RUN_PYTHON_TESTS=1` is defaulted on by SCons but is a no-op for combigrid — the module does not register any Python tests (`combigrid/SConscript` does not call `runPythonTests()`). The Python bindings (§11) are verified through the Python examples in [combigrid/examples/python/](combigrid/examples/python/) (`RUN_PYTHON_EXAMPLES=1`).

---

## 6. Development workflow

### Adding a new source file / class

1. Put `foo.hpp` / `foo.cpp` under the matching subdirectory of [combigrid/src/sgpp/combigrid/](combigrid/src/sgpp/combigrid/). No SConscript changes needed — `ModuleHelper.scanSource()` walks `src/` recursively and picks up every `*.cpp`/`*.hpp` (`excludeFiles` list is empty for combigrid).
2. Match the layered namespace convention: `sgpp::combigrid` for public API; `sgpp::combigrid::tools` for helpers under `tools/`; `sgpp::combigrid::misc` for helpers under `miscellaneous/`; anonymous namespaces for TU-local helpers.
3. Include with angle brackets + absolute paths: `#include <sgpp/combigrid/foo/bar.hpp>`. `tools/check_includes.py` will reject anything else.
4. Add the SG++ copyright banner (§7).
5. If the header is part of the public API, add it to [combigrid/src/sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp). This is required if it is to be exposed to Python (the SWIG wrapper is compiled against the umbrella header).

### Bindings

Python bindings: see §11. Adding new C++ files does **not** require touching the bindings unless the new API should be available in `pysgpp`. Changing the signature of a bound function can break the SWIG build (`SG_PYTHON=1`) even if the C++ build is fine — rebuild with `SG_PYTHON=1` after API changes.

### Formatting

- `.clang-format` at repo root: `BasedOnStyle: Google`, `ColumnLimit: 100`, `IncludeBlocks: Preserve`. Run `clang-format -i <file>` before committing.
- cpplint is invoked automatically per source file when `CHECK_STYLE=1` (default). Config: [CPPLINT.cfg](CPPLINT.cfg) — filters out `build/c++11` and `runtime/references`; line length is enforced at 100.

### What CI checks

[.github/workflows/ci.yml](.github/workflows/ci.yml) runs 5 matrix jobs on push to `master` and every PR:
- **windows** — MinGW/GNU on Windows with `SG_PYTHON=1`, static/shared × 2.
- **linux-minimal** — `SG_PYTHON=0`, examples run, `-Werror`.
- **linux-medium** — `SG_ALL=0 SG_BASE=1 SG_PYTHON=1`, `-Werror`, runs C++ + Python examples.
- **linux-full** — everything on, `-Werror`.
- **linux-packaging** — deb + Python wheel packaging test.

Consequence: combigrid changes only surface in the **linux-full** job (medium disables combigrid; minimal only builds base). Locally reproducing that means the default `scons` invocation with Boost, SWIG, numpy, GSL, Eigen, Armadillo, Gmm++, UMFPACK installed. Additionally: [.jenkins/Jenkinsfile](.jenkins/Jenkinsfile) drives the Stuttgart Jenkins.

There is **no pre-commit hook** in the repo. All style/lint enforcement is via the scons build itself (see [SConstruct:390-431](SConstruct#L390-L431)).

---

## 7. Conventions

- **C++ standard**: C++11 (see [site_scons/SGppConfigure.py:185-198](site_scons/SGppConfigure.py#L185-L198); C++14 only when `USE_HPX=1`). Combigrid uses features up to C++11 (lambdas, `override`, `std::shared_ptr`, `std::unordered_map`, structured bindings **not** allowed).
- **Namespaces**: `sgpp::combigrid` is the primary; nested `tools`, `misc`, `constants`, plus per-operator inline namespaces (`global_interpolation`, `quadrature_operator`, `linear_interpolation`) used to scope internal helpers. Full path spelled out — no `using namespace` in headers.
- **Header guards**: Use `#pragma once`
- **File header**: every `.cpp`/`.hpp` must start with the exact 4-line SG++ copyright banner:
  ```
  // Copyright (C) 2008-today The SG++ project
  // This file is part of the SG++ project. For conditions of distribution and
  // use, please see the copyright notice provided with SG++ or at
  // sgpp.sparsegrids.org
  ```
  Enforced by `tools/check_copyright_banners.py`. Missing → build fails when `CHECK_STYLE=1`.
- **Includes**: angle brackets only, absolute paths (`#include <sgpp/...>`, `#include <cstdint>`), grouped conventionally but not re-sorted by clang-format (`IncludeBlocks: Preserve`). Enforced by `tools/check_includes.py`.
- **Doxygen**: heavy use of Javadoc-style block comments (`/** ... */`) with `@brief`, `@param`, `@return`, `@note`, and `@f$ ... @f$` for LaTeX math. Follow this pattern for new public API; new files typically get a top-of-file `@file` + `@brief`.
- **Naming**: `PascalCase` for types (`SparseGrid`, `SourceFunc`, `CompleteSGGenInstr`); `camelCase` for functions/methods (`nDim()`, `setDomain()`, `computeCTCoeffs`); trailing underscore or plain camelCase for member variables (`nDim_`, `tensorGridData`); `SCREAMING_SNAKE_CASE` for `constexpr` constants; `snake_case` file names.
- **Error handling**: `assert` for internal invariants (e.g. `assert(point.size() == sparseGrid.nDim())`); `throw` a subclass of `sgpp::base::*_exception` for actual user-input errors. Do not `throw std::exception`.
- **Parallelism**: OpenMP (`#pragma omp parallel for`) is used in the operator top-level drivers ([global_interpolation.cpp:29](combigrid/src/sgpp/combigrid/operators/global_interpolation/global_interpolation.cpp#L29), [quadrature.cpp:30](combigrid/src/sgpp/combigrid/operators/quadrature/quadrature.cpp#L30), [linear_interpolation.cpp:37](combigrid/src/sgpp/combigrid/operators/linear_interpolation/linear_interpolation.cpp#L37)) and internally, gated by the thresholds in `constants.hpp`. There is also a custom lightweight thread-pool in [tools/concurrency.hpp](combigrid/src/sgpp/combigrid/tools/concurrency.hpp).

---

## 8. Gotchas

- The [combigrid/src/sgpp/combigrid/](combigrid/src/sgpp/combigrid/) tree contains many `*.hpp.lint` / `*.cpp.lint` / `*.os` / `*.gcda` files with **no** matching `.hpp`/`.cpp`. Those are stale artifacts left after files were renamed/removed and are not built. Examples: `sparse_grid_generation_instructions/full_grid_sg_gen_instruction.hpp.lint`, `sparse_grid_generation_instructions/full_sg_gen_instruction.hpp.lint`, `mi_test.hpp.lint`, `constants.hpp.lint`. Do not resurrect them from `.lint` — they will be regenerated on next build.
- `MIVecSGGenInstr` **references** its input `LvlMIVec` — the caller must keep it alive for the lifetime of the instruction and any `SparseGrid` derived from it ([multiindex_vector_sg_gen_instruction.hpp:44](combigrid/src/sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp#L44)). `clone()` copies the reference, so the clone stored inside a `SparseGrid` dangles too if the vector dies (only matters if `genMIVec*()` is called on it). The Python bindings keep the vector alive automatically (§11).
- `SparseGrid::getMaxTGGPCnt()` / `getMaxTGSumOverGPCntsPerDim()` are **caches that must be set explicitly** via the corresponding setters; they are not recomputed on add/replace. Do not read them without a prior set.
- `SparseGrid::operator==` is `O(nTG²)` (order-insensitive set comparison) — avoid in loops.
- The umbrella header [sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp) contains only the public API (internal helpers such as bounding boxes, lookups, scratch buffers are not included). Include specific headers for anything internal.
- The Python bindings are part of the flat `pysgpp` module (no `pysgpp.combigrid` submodule): `pysgpp.SparseGrid`, `pysgpp.quadrature`, `pysgpp.computeCTCoeffs` (C++ namespaces such as `tools::` are dropped). See §11 for limitations (e.g. Python source functions run serially under the GIL, C++ `assert`s abort the interpreter in `OPT=0` builds).
- `constants::source_func::USE_CACHE = false` in [constants.hpp](combigrid/src/sgpp/combigrid/constants.hpp) — the `SourceFunc` cache exists but is not yet properly implemented.
- Parallelism thresholds in [constants.hpp](combigrid/src/sgpp/combigrid/constants.hpp) gate concurrency per subsystem: `mi_vec::CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000` (component-wise-max), `ct_coefficients::MIN_MIS_FOR_CONCURRENCY = 1000`. Tune before benchmarking if working with small or unusually large MI sets.
- `setBoundaryLevelOffset(0)` enables boundary nodes in every dimension for every tensor grid (ℓ[k] ≥ 0 is always true). `setBoundaryLevelOffset(1)` (the SGMK default) adds boundary only when ℓ[k] ≥ 1, skipping the coarsest level. Very large values effectively disable boundary entirely.
- `CompleteSGGenInstr` skips tensor grids with a zero CT coefficient (those with `|ℓ|₁ < maxLvl−(d−1)`) entirely — they are never stored in the `SparseGrid`. This is the memory-efficient path; `MIVecSGGenInstr` applies the same filter after calling `computeCTCoeffs`.
- The Clenshaw-Curtis getter (`getClenshawCurtisNodeGenFunc()`) and the second-type Chebyshev getter (`getSecondTypeChebyshevNodeGenFunc()`) return the **same singleton** — they are aliases. Use either consistently; do not compare pointer addresses to distinguish them.
- CI's `linux-medium` explicitly builds only `SG_BASE`. Combigrid regressions are only caught by the `linux-full` job — expect PR feedback there, not earlier.
- **Thesis API discrepancies** (if copying from the thesis — code wins): 
The thesis might be outdated - in particular outdated code. Therefore, code from the thesis should not be copy-pasted without checking against the actual API.
- **Dimension-adaptive refinement is not implemented**. `MIVecSGGenInstr` is the entry point for custom MI sets, but the adaptive loop (error estimation, set expansion) must be built by the caller. This is listed as the highest-priority future-work item in the thesis.
- **No native vector-valued source functions**: `SourceFunc` wraps `double(DataVector&)` only. For multi-output problems, run multiple separate `SourceFunc`/`quadrature` calls.
- **Single global `boundaryLevelOffset`**: the offset applies identically across all dimensions. Dimension-wise offsets (for anisotropic boundary treatment) are future work.
- **No Leja nodes**: only equidistant and Chebyshev 1st/2nd-kind are implemented. Leja nodes (non-nested, better convergence properties in some regimes) are a planned extension.
- **No reduced sparse-grid representation**: the CT always processes full tensor grids. Frameworks that collapse shared grid points (Tasmanian, SGMK) become faster at higher d (≥ 7 roughly) because duplicate points are evaluated only once. The current module has the best memory efficiency but is slower at high d for that reason.
- **Source function cache is binary**: `constants::source_func::USE_CACHE` in [constants.hpp](combigrid/src/sgpp/combigrid/constants.hpp) enables caching for all `SourceFunc` instances at once. Per-function or on-demand caching (like SGMK's approach) is future work.

---

## 9. Thesis as on-demand reference

A faithful Markdown conversion of the thesis that produced this module lives in
[claude/bachelor-thesis/](claude/bachelor-thesis/) (read-only). Use it for
precise definitions and proofs; always verify API signatures against the actual
source code because the thesis predates minor refactors.

| Task | Read |
|---|---|
| Sparse-grid math: multi-indices, levels, CT coefficients, inclusion-exclusion, complete sparse grids, boundary treatment | [02-sparse-grids.md](claude/bachelor-thesis/02-sparse-grids.md) |
| Public API overview, feature table, configuration walkthrough | [04-new-ct-module-for-sgpp.md](claude/bachelor-thesis/04-new-ct-module-for-sgpp.md) |
| Internal algorithms: 2-domain design, node-gen abstraction, on-demand assembly, barycentric interpolation, quadrature rule selection, CT-coefficient algorithm (dim-separated, with proof), boundary management | [05-algorithmic-and-design-choices.md](claude/bachelor-thesis/05-algorithmic-and-design-choices.md) |
| Three complete worked examples (quadrature, global interpolation, forward UQ) with expected numerical results | [06-application.md](claude/bachelor-thesis/06-application.md) |
| Why SG++ outperforms competitors on memory/CPU; why Tasmanian/SGMK win at higher d; benchmark methodology (cgroup, `time -v`, `perf stat`, warm-up/measurement phases) | [07-comparison-between-frameworks.md](claude/bachelor-thesis/07-comparison-between-frameworks.md) |
| What is intentionally not implemented and why (adaptive refinement, Leja nodes, reduced representation, etc.) | [08-conclusion-and-outlook.md](claude/bachelor-thesis/08-conclusion-and-outlook.md) |
| Four-framework benchmark code (SG++/SGMK/Tasmanian/sparseSpACE) and the bash benchmarking script | [09-appendix.md](claude/bachelor-thesis/09-appendix.md) |

**Known thesis–code discrepancies** (code is truth, documented in §8 Gotchas above):
(a) `setBoundaryUsage()` does not exist — use `setBoundaryLevelOffset(LvlType)`; 
(b) `MIVecSGGenInstr` takes `const LvlMIVec&` not `shared_ptr<LvlMIVec>`;
(c) node getters return `NodeGenFunc*` (raw pointer to a static singleton), not `shared_ptr<NodeGenFunc>`.

---

## 10. Other pointers

- Umbrella docs: https://sgpp.github.io/SGpp/ (API reference), http://sgpp.sparsegrids.org (project home).
- Build variables: `scons -h` in the repo root (auto-generated from [SConstruct](SConstruct)).
- Module SConscript template / conventions: [site_scons/ModuleHelper.py](site_scons/ModuleHelper.py).
- Compiler / library detection: [site_scons/SGppConfigure.py](site_scons/SGppConfigure.py).
- Combigrid Doxygen bits: [combigrid/doc/doxygen/](combigrid/doc/doxygen/) (currently only `install_dakota.doxy`).
- Runnable minimal C++ examples: [combigrid/examples/c++/quadrature/simple_const_func_full_sg_quadrature.cpp](combigrid/examples/c++/quadrature/simple_const_func_full_sg_quadrature.cpp), [combigrid/examples/c++/global_interpolation/sin_func_full_sg_interpolation.cpp](combigrid/examples/c++/global_interpolation/sin_func_full_sg_interpolation.cpp).
- Adaptive-MI-set example: [combigrid/examples/c++/global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp](combigrid/examples/c++/global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp).
- Test entry point: [combigrid/tests/test_base.cpp](combigrid/tests/test_base.cpp); representative test showing usage patterns: [combigrid/tests/grids/sparse_grid_construction_test.cpp](combigrid/tests/grids/sparse_grid_construction_test.cpp).
- Platform install notes: [INSTRUCTIONS](INSTRUCTIONS), [INSTRUCTIONS_MAC](INSTRUCTIONS_MAC), [INSTRUCTIONS_WINDOWS](INSTRUCTIONS_WINDOWS).
- License: [LICENSE.md](LICENSE.md) (modified BSD).

---

## 11. Python bindings (pysgpp)

### Building and running

```bash
scons -j8 OPT=0 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 SG_PYTHON=1 COMPILE_BOOST_TESTS=0 RUN_PYTHON_TESTS=0
export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH
export PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp:$PYTHONPATH
python3 -c "import pysgpp; print(pysgpp.SparseGrid)"
```

- Requires SWIG ≥ 3.0.4 (developed with 4.2.0), Python 3 headers (`python3-dev`) and NumPy (base's `numpy.i`).
- Every SCons run re-runs SWIG and recompiles the whole `pysgpp/pysgpp_wrap.cc` (~2 min): [pysgpp/SConscript](pysgpp/SConscript) deletes the wrapper on purpose because SCons' SWIG dependency tracking is unreliable.
- All modules are wrapped into **one** flat module (`pysgpp_swig`, re-exported as `pysgpp`), built by [pysgpp/pysgpp.i](pysgpp/pysgpp.i) which `%include`s `base/build/pysgpp/base.i` first and `combigrid/build/pysgpp/combigrid.i` last. `pysgpp.i` also defines the global `%exception` (every `std::exception` → `RuntimeError`) and compiles the wrapper against each module's umbrella header.
- Python examples: [combigrid/examples/python/](combigrid/examples/python/), 1:1 ports of the C++ examples in `examples/c++/global_interpolation/` and `examples/c++/quadrature/` (same file names). They are run by SCons with `RUN_PYTHON_EXAMPLES=1` (registered in [combigrid/SConscript](combigrid/SConscript), since `runExamples()` does not recurse into subfolders) or directly with `python3 <file>.py`.
- Comparing results with C++: operators use OpenMP `reduction(+)`, so with several threads the last ~1e-15 of a result varies between runs **in C++ as well**. Use `OMP_NUM_THREADS=1` for bit-identical C++/Python comparisons.

### Files

| file | contents |
|---|---|
| [combigrid/build/pysgpp/combigrid.i](combigrid/build/pysgpp/combigrid.i) | entry point: `%shared_ptr` declarations, container templates, generation instructions, grids, operators, CT coefficients |
| [combigrid/build/pysgpp/MultiIndex.i](combigrid/build/pysgpp/MultiIndex.i) | `MI<unsigned int>` → `LvlMI` (+ `GPMI` alias), `MIVec<unsigned int>` → `LvlMIVec` |
| [combigrid/build/pysgpp/Functions.i](combigrid/build/pysgpp/Functions.i) | growth functions (`Lvl2GPCntFunc`), `NodeGenFunc` and its getters |
| [combigrid/build/pysgpp/SourceFunc.i](combigrid/build/pysgpp/SourceFunc.i) | `SourceFunc` from a Python callable; GIL/exception machinery; `COMBIGRID_SOURCE_FUNC_EXCEPTION` |
| [combigrid/src/sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp) | umbrella header the wrapper is compiled against (exactly the bound headers) |

### What is exposed

Names are identical to C++; argument order and defaults are unchanged.

- **Multi-indices**: `LvlMI` / `GPMI` (constructors `()`, `(count)`, `(count, value)`, `(list)`; `toLinearIndex`, `productofElems`, `sumOfElems`, `nDim`, `at`, `front`, `back`, `empty`, `size`, `capacity`, `reserve`, `shrink_to_fit`, `clear`, `push_back`, `pop_back`, `resize`, `swap`; `== != < <= > >=`, `+ - += -=`; `len`, `[]`, `str`). `LvlMIVec` (constructors `(nDim, nMI)`, `(list of lists)`, `(list of LvlMI)`; `nDim`, `nMI`, `miVec(i, d)`, `setMI`, `moveMI`, `resize`, `shrink_to_fit`, `isDownwardsClosed`, `downwardsClosure`, `componentWiseMax`, `paretoMaxima`; `len`, `[]`).
- **Functions**: `SourceFunc(callable)` with `evaluate`, `evaluateNormalizedInPlace`, `evaluateNormalizedOutOfPlace`; `linearLvl2GPCntFunction`, `doublingLvl2GPCntFunction`; `NodeGenFunc` (`genNodes`, `genNodesWithBoundary`, `genNodesWithoutBoundary`, `id`, `==`) and `getEquidistantNodeGenFunc`, `getClenshawCurtisNodeGenFunc`, `getFirstTypeChebyshevNodeGenFunc`, `getSecondTypeChebyshevNodeGenFunc`.
- **Generation instructions**: `SGGenInstr` (all getters/setters, `genMIVec`, `genMIVecWithCoeff` → tuple `(LvlMIVec, tuple of ints)`, `clone`, `resize`, `getVolumeOfDomain`, `getUniqueNodeGenFuncs`), `CompleteSGGenInstr(maxLvl, nDim)` + `setMaxLvl`, `MIVecSGGenInstr(miVec)`.
- **Grids**: `SparseGrid(genInstr)` (`nDim`, `nTG`, `getTensorGrid(idx)`, `getTensorGrid(LvlMI)` → `TensorGridCTData` or `None`, `getTensorGrids` → tuple of copies, `getGenInstr`, `getMaxTGGPCnt`, `getMaxTGSumOverGPCntsPerDim`, `==`, `len`, `[]`, iteration); read-only `TensorGridCTData` (`mi`, `coefficient`, `tensorGrid`, `==`); `TensorGrid` (constructors, `nDim`, `nGP`, `getGPCntPerDim`, `getNodesPerDim`, all `getGridPoint` / `getGridPoints` overloads, `getGridPointAndMI` (out-parameter overload or tuple `(DataVector, LvlMI)`), `==`).
- **Operators / tools**: `interpolate`, `interpolateLinear`, `quadrature`, `computeCTCoeffs`, `computeCTCoeffSingle` (→ tuple of ints).
- **Helper container types** (base naming convention): `UnsignedIntVector`, `UnsignedIntVectorVector`, `LvlMIVector`, `DoubleDoublePair`, `HyperCubeArea`, `NodeGenFuncVector`, `TensorGridCTDataVector`. Plain Python lists/tuples are accepted wherever these are expected.

C++ → Python translation:

| C++ | Python |
|---|---|
| `double f(const DataVector& p)`; `SourceFunc sf(f)` | `def f(p): ...` (`p` is a **tuple** of floats); `sf = pysgpp.SourceFunc(f)` |
| `genInstr.setDomain({-1, 1})` | `genInstr.setDomain((-1, 1))` |
| `HyperCubeArea{{0, 1}, {-1, 1}}` | `[(0, 1), (-1, 1)]` |
| `LvlMIVec mis{{0, 5, 0}, {0, 4, 1}}` | `pysgpp.LvlMIVec([[0, 5, 0], [0, 4, 1]])` |
| `LvlMI mi{1, 2}` | `pysgpp.LvlMI([1, 2])` |
| `DataVector point{0.1, 0.2}` | `pysgpp.DataVector([0.1, 0.2])` |
| `setLvl2GPCntFunc(doublingLvl2GPCntFunction)` | `setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)` |
| `tools::computeCTCoeffs(mis)` | `pysgpp.computeCTCoeffs(mis)` |

### Deliberately not exposed

| C++ API | reason |
|---|---|
| `SparseGrid(nDim)`, `SparseGrid(nDim, nTG)`, `addTensorGrid`, `setTensorGrid`, `setGenInstr`, `setMaxTGGPCnt`, `setMaxTGSumOverGPCntsPerDim` | manual assembly is extension work; a grid without instruction or with unset size caches crashes the operators |
| `QuadRule`, `InterpolationMethod` and their implementations/getters, `NodeGenFunc::getQuadRule` / `getInterpolationMethod`, concrete `node_gen_funcs::*` classes | extension machinery; node types are chosen via the getters, quadrature/interpolation rules are selected automatically |
| `global_interpolation::*`, `linear_interpolation::*`, `quadrature_operator::*` | internal; because SWIG flattens namespaces they would otherwise be merged into the public `interpolate` / `quadrature` as extra overloads |
| `tools::computeCTCoeffsNaive` | reference implementation for tests |
| `constants::*` | `constexpr`, cannot be changed at runtime |
| everything else in `misc::` / `tools::` (lookups, bounding boxes, caches, scratch buffers, concurrency, math/hash helpers, benchmarker) | internal |
| iterators, rvalue-reference overloads, `std::initializer_list` constructors, conversion operators, `MI::data/insert/erase`, `MIVec::data/lookup/clearCachedValues`, `MIVecElemProxy`, `MI` operators mixing `MI` and `std::vector` | no Python meaning / covered by other overloads |
| directors (Python subclasses of `SGGenInstr`, `NodeGenFunc`, ...) | extension points; overrides would be called from OpenMP threads |

### Limitations (cannot be wrapped faithfully)

- **`Lvl2GPCntFunc` is a context-free C function pointer.** The two predefined functions are exposed as function-pointer constants (`%callback`); they can be passed and compared (`==`) but **not called** from Python. Custom growth functions written in Python are impossible without changing the C++ type (e.g. to `std::function`).
- **Python source functions are evaluated serially** (every call acquires the GIL), so OpenMP does not speed them up.
- **C++ `assert`s abort the Python process** in `OPT=0` builds (e.g. `interpolate` with a point of the wrong dimension); with `OPT=1` they are compiled out (undefined behaviour). The bindings add no extra validation. Likewise, C++ accessors without bounds checks stay unchecked (`MIVec::setMI`, `miVec(i, d)`, `SparseGrid.getTensorGrid(idx)`, `TensorGrid.getGridPoint(idx)`); only the Python-added `[]` operators raise `IndexError`.
- `SparseGrid.getGenInstr()` returns an `SGGenInstr` proxy (no downcast), and SWIG drops the `const` of `shared_ptr<const SGGenInstr>`: setters called on it modify the instruction the sparse grid's operators use.
- `LvlMI` comparisons are the C++ **component-wise partial order**, not lexicographic (`sorted()` on multi-indices is meaningless); `LvlMI` is unhashable.

### Non-obvious typemap and ownership decisions

1. **`SourceFunc` from a Python callable** ([SourceFunc.i](combigrid/build/pysgpp/SourceFunc.i)), modelled on base's `OperationQuadratureMC.i` (reuses its `PyObject *pyfunc` typemap; callable receives a tuple).
   - `swig -threads` releases the GIL around every wrapped call and the operators call the function from OpenMP threads → every call into Python (and every `Py_INCREF`/`Py_DECREF`) happens under `PyGILState_Ensure`.
   - A Python exception cannot cross an OpenMP region: the first exception is stored (`PyErr_Fetch`), the evaluation returns NaN, later evaluations return NaN without calling Python, and `COMBIGRID_SOURCE_FUNC_EXCEPTION(function)` re-raises it after the C++ call returns. The `SourceFunc` is reusable afterwards.
   - `SourceFunc::func` is private, so the wrapper keeps a registry `SourceFunc*` → holder (entry removed in the `%extend` destructor).
2. **Keep-alive references (`_owner`)**: proxies returned by reference into another object store their owner's proxy via `%pythonappend` — `SparseGrid.getTensorGrid` (both overloads), `SparseGrid.getGenInstr`, `SGGenInstr.clone`, `TensorGrid.getGPCntPerDim` / `getNodesPerDim`, `TensorGridCTData.mi` / `tensorGrid` (re-implemented as Python properties, since `%pythonappend` does not apply to data members). `MIVecSGGenInstr` stores `_miVec`, `SparseGrid` stores `_genInstr` (the clone inside the grid references the caller's `LvlMIVec`). Base does **not** do this (e.g. `Grid.getStorage()` can dangle). Verified with valgrind: 0 errors; a control run with `_owner` removed produces invalid reads.
3. **`%shared_ptr` only for `SGGenInstr`, `CompleteSGGenInstr`, `MIVecSGGenInstr`.** `MIVec::componentWiseMax` / `paretoMaxima` return copies instead (helpers `*Copy` renamed to the C++ name), because `%shared_ptr(std::vector<size_t>)` would change how base's `SizeVector` is wrapped everywhere.
4. **`MI::operator+=` / `-=` are not wrapped directly**: SWIG would return a second, non-owning proxy that replaces the owning one on `a += b`, deleting the object. `__iadd__` / `__isub__` are Python methods calling `__iaddImpl` / `__isubImpl` and returning `self`.
5. **Pairs returned by value** (`genMIVecWithCoeff`, `getGridPointAndMI(idx)`) use custom `out` typemaps producing tuples: `std_pair.i` cannot instantiate `pair<MIVec, ...>` because `MIVec` is not copy-assignable (`const` member).
6. **`std::vector<Lvl2GPCntFunc>`** (`setLvl2GPCntFuncs` / `getLvl2GPCntFuncs`) uses custom `in`/`out` typemaps: `std_vector.i` does not compile for function-pointer elements.
7. **`SparseGrid::getTensorGrid(const LvlMI&)`** returns a private `const_iterator` typedef (does not compile in the wrapper). Replaced by the `%extend` helper `getTensorGridByMI`, `%rename`d to `getTensorGrid`. An `%ignore` with a given signature also hides an `%extend` method of the same name and signature — hence the helper name + `%rename` pattern.
8. Node getters return raw pointers to static singletons without `%newobject` → non-owning proxies.

### How to extend the bindings

1. Apply the scoping rule: bind what a default user needs to build, configure, evaluate and query a surrogate; skip extension machinery unless it appears in a signature a default user calls.
2. Add the header to [sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp) — otherwise the generated `pysgpp_wrap.cc` does not compile (undeclared identifiers).
3. Put `%ignore` / `%rename` / `%extend` / `%pythonappend` / `%exception` directives **before** the `%include "combigrid/src/..."` of the header (features only apply to declarations parsed afterwards). New `%shared_ptr` declarations go to the top of `combigrid.i`, before any class that uses the type.
4. Instantiate container/templates with `%template` using base's naming (`<Elem>Vector`, `<A><B>Pair`) after the element type is declared. Check [base/build/pysgpp/base.i](base/build/pysgpp/base.i) first — an existing instantiation must not be duplicated (all modules share one namespace).
5. Ownership: a method returning a reference/pointer into `self` → `%pythonappend ... %{ val._owner = self %}`; a constructor/setter that stores a reference to an argument → keep the argument alive on `self`.
6. Any new wrapped function that evaluates a `SourceFunc` (with the `SourceFunc` as **first** argument) must get `COMBIGRID_SOURCE_FUNC_EXCEPTION(<qualified name>)` before its `%include`; otherwise Python exceptions are turned into NaN and only surface at the next checked call.
7. SWIG gotchas: `%pythoncode` blocks containing `#` comments must use `%{ ... %}` (with `{ ... }` SWIG parses `#` lines as preprocessor directives); new public free-function names must not clash with other modules' names; internal helper namespaces must be `%ignore`d.
8. Rebuild with the command above and check the log for SWIG warnings mentioning `combigrid/` (the remaining expected ones are 503 for `MI`'s friend operators and 389 for `operator[]`, both handled by `%extend`). Inspect the generated proxies in `lib/pysgpp/pysgpp_swig.py`, run [combigrid/examples/python/](combigrid/examples/python/), and comment every non-obvious directive in the `.i` file.
