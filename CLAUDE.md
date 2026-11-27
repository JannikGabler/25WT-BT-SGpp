# CLAUDE.md — Working on the SG++ /combigrid/ module

Scope: editing, building and testing the Combination Technique (CT) module in
[combigrid/](combigrid/). Other SG++ modules, bindings and framework internals matter only insofar as
they affect `combigrid/`.

Read on demand:

| Topic | File |
|---|---|
| Directory layout, key classes (incl. DSG operators, cross-call operator data cache, source-function caching), design rationale, vocabulary, base dependencies, known limitations, thesis discrepancies | [combigrid/claude/module-reference.md](combigrid/claude/module-reference.md) |
| SCons internals, flag sources, optional deps (Eigen/Armadillo/Dakota), test options, CI matrix, other pointers | [combigrid/claude/build-and-ci.md](combigrid/claude/build-and-ci.md) |
| Python bindings (pysgpp) | [combigrid/claude/python-bindings.md](combigrid/claude/python-bindings.md) |
| Thesis (math, proofs, algorithms, benchmarks) | [combigrid/claude/bachelor-thesis/README.md](combigrid/claude/bachelor-thesis/README.md) |
| Performance optimization phase | §8 |

---

## 1. Overview

SG++ is a C++ HPC framework for **sparse grids** ([README.md](README.md)). This module implements
the **dimensionally-adaptive sparse-grid combination technique**: `f ≈ Σ_ℓ c_ℓ · f_ℓ`, where `f_ℓ`
lives on the (anisotropic) full tensor grid of level multi-index `ℓ` and `c_ℓ ∈ ℤ` is the CT
coefficient. Only [base/](base/) is a dependency (`DataVector`, `DataMatrix`, exceptions); all other
modules are irrelevant.

- Level multi-indices `ℓ ∈ ℕ^d` are **0-based**. Nodes per dimension come from `Lvl2GPCntFunc`;
  `doublingLvl2GPCntFunction`: 2^{ℓ+1}−1 inner nodes.
- **Complete sparse grid** (max level L): tensor grid ℓ kept iff `L−(d−1) ≤ |ℓ|₁ ≤ L`, coefficient
  `(−1)^{L−|ℓ|₁} · C(d−1, L−|ℓ|₁)`.
- **Downwards-closed** MI set: every component-wise predecessor of a member is a member.
  `MIVecSGGenInstr` always computes the closure.
- The CT is **exact** on the span of the hierarchical increments of its tensor grids (thesis §2.4).

## 2. Module essentials

Pipeline: a generation instruction (`CompleteSGGenInstr(maxLvl, nDim)` or `MIVecSGGenInstr(miVec)`
for custom / adaptive MI sets) → `SparseGrid` (struct of arrays in `SparseGridBase`: levels
`LvlMIVec`, coefficients, two `NodeTable`s = 1D node lists once per (dim, level), on the domain and on
[0,1]; tensor grid i is a view `TensorGridCTDataView {mi, coefficient, tensorGrid}` with domain or unit
nodes: `getDomainTensorGrid(i)` / `getUnitTensorGrid(i)`, ranges `domainTensorGrids()` /
`unitTensorGrids()`, no default iteration; `toTensorGridCTData()` copies)
→ operators `interpolate`, `interpolateLinear`, `quadrature`. A `DeltasSparseGrid` (nested nodes
only) has its own overloads of all three operators that evaluate every grid point / distinct corner
once per call (thread-count independent); the `SparseGrid` operators implement the standard CT.
`textbookQuadrature` / `textbookInterpolate` / `textbookInterpolateLinear` (`SparseGrid` only,
[operators/textbook/](combigrid/src/sgpp/combigrid/operators/textbook/)) are naive, serial
per-tensor-grid reference versions for reading and as test oracles (same results within
round-off, same evaluation points); not for production.
**Dimension-adaptive refinement** (`DimensionAdaptiveRefinement` / `refine`,
[refinement/](combigrid/src/sgpp/combigrid/refinement/)): grows a `SparseGrid` in place
(Gerstner–Griebel, guided by a `RefinementTarget`: quadrature, global / linear interpolation at
points, or any per-tensor-grid operator); afterwards the grid is an ordinary grid. Not in Python yet.
User functions are wrapped in `SourceFunc` (f: Ω → ℝ^m: `VectorSourceFunc`); weights, basis values
and interpolation cells live on [0,1]^d (`getUnitNodeTable()`), the source function is evaluated at
the domain nodes (`getDomainNodeTable()`, mapped with `tools::mapToDomain`, bitwise the points of
all operators). Everything public is also in Python. Details of the following in the module
reference:
- **Cross-call operator data cache** (`SparseGridBase`): per-grid operator data kept across calls,
  controlled by the operator's options (last, defaulted argument, field
  `useCrossCallOperatorDataCache`; default on for the three DSG operators, off for `SparseGrid`
  quadrature: `QuadratureOptions` vs. `DeltasSparseGridQuadratureOptions`; SG interpolations keep
  nothing); public `clearCrossCallOperatorDataCache()`,
  `getCrossCallOperatorDataCacheEntries()`, `getCrossCallOperatorDataCacheBytes()`.
- **Source-function caching**: `CachedSourceFunc` / `CachedVectorSourceFunc` look points up in a
  shared `SourceFuncCache` (`SourceFuncCacheConfig`; policies `MemoryOnly` (default),
  `TemporaryStorage` / `PersistentStorage` (LMDB), `Adaptive`). Needs Boost ≥ 1.81 + xxHash ≥ 0.8 at
  build time (LMDB for the storage policies), else the constructors throw
  `base::dependency_exception`.

```cpp
CompleteSGGenInstr genInstr(/*maxLvl=*/10, /*nDim=*/3);
genInstr.setDomain({0, 1});
genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
genInstr.setBoundaryLevelOffset(0);
const SparseGrid sg(genInstr);
const double val = interpolate(SourceFunc(myFunc), point, sg);
```

Examples: [combigrid/examples/c++/](combigrid/examples/c++/). Umbrella header
[combigrid/src/sgpp_combigrid.hpp](combigrid/src/sgpp_combigrid.hpp) = exactly the public
(Python-bound) API; internal helpers are included directly. Parallelism thresholds:
[constants.hpp](combigrid/src/sgpp/combigrid/constants.hpp); integer types:
[type_defs.hpp](combigrid/src/sgpp/combigrid/type_defs.hpp).

---

## 3. Build

SCons, combigrid only. Wrapper scripts in the repo root:
- [compileDebug.sh](compileDebug.sh): `scons -j16 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 OPT=0` —
  **default** (asserts enabled).
- [compileNormal.sh](compileNormal.sh): `OPT=1 ARCH=avx2 USE_EIGEN=1 USE_ARMADILLO=1` +
  `-march=native -flto=auto` in `CPPFLAGS`/`LINKFLAGS` — for anything runtime-relevant.
- [compileASan.sh](compileASan.sh): debug + `-fsanitize=address -fno-omit-frame-pointer`.

`VERBOSE=1` shows commands, `-c` cleans, `scons -h` lists all variables. `SG_ALL=0` needs both
`SG_BASE=1` and `SG_COMBIGRID=1` ("You must enable at least one module"); `SG_ALL=1` builds every
module. Libraries land in [lib/](lib/); run binaries with `export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH`.

**Performance-relevant configuration** (judged): `compileNormal.sh` flags = `-O3 -DNDEBUG` (no
asserts), GCC `-fno-semantic-interposition` (from `OPT=1`; inlining of exported functions despite
`-fPIC`), LTO, `-march=native`, Eigen + Armadillo (= `combigrid/perf/scripts/perf.py build normal`),
16 threads. Strict aliasing is on for all SG++ code (only the SWIG Java wrapper gets
`-fno-strict-aliasing`): type punning via pointer casts is UB — use `std::bit_cast` / `memcpy`.
Benchmarks must use this configuration. `ARCH=avx2` matches the benchmarking machine and may be
missing elsewhere: optimizations for it are fine if they cost nothing significant on other
architectures.

## 4. Testing

Boost.Test; all of [combigrid/tests/](combigrid/tests/) builds into one executable that SCons runs
after building (`RUN_BOOST_TESTS=0` to skip running, `COMPILE_BOOST_TESTS=0` to skip building).
Re-run directly: `./combigrid/tests/test_combigrid_boost --log_level=test_suite`
(`--run_test=Suite/Case`, `--list_content`).

New test: a `*.cpp` in the matching subdirectory (picked up automatically) with
`#define BOOST_TEST_DYN_LINK`, `#include <boost/test/unit_test.hpp>`, `BOOST_AUTO_TEST_SUITE` /
`BOOST_AUTO_TEST_CASE`, the copyright banner; **no** `BOOST_TEST_MODULE` (defined in
[test_base.cpp](combigrid/tests/test_base.cpp)).

**Portability**: tests must pass regardless of machine, architecture, compiler and build options
(flags, optional libraries, ...) and should generally be portable (i.e. test in every environment). 
Their scope or method may adapt to such characteristics where that helps (e.g. larger sizes only with 
an FFT backend). Exceptions are tests of functionality that itself depends on a specific library or 
build flag; where this is a case distinction (e.g. with / without a library), every branch must be 
covered by tests.

---

## 5. Conventions

- **New files**: under the matching subdirectory of
  [combigrid/src/sgpp/combigrid/](combigrid/src/sgpp/combigrid/) (picked up automatically). Public
  API headers go into the umbrella header (required for Python exposure).
- **Operator internals**: internal methods used by end-user operations go into their own files
  under [combigrid/src/sgpp/combigrid/tools/](combigrid/src/sgpp/combigrid/tools/) (e.g.
  [tools/operators/quadrature/](combigrid/src/sgpp/combigrid/tools/operators/quadrature/) for
  `quadrature`), not into the operator's own file.
- **C++23** (combigrid need not compile with older standards). `#pragma once`.
- **Namespaces**: `sgpp::combigrid` (public), `::tools` (under `tools/`), `::misc` (under
  `miscellaneous/`), `::constants`, per-operator namespaces for internals (e.g.
  `tools::quadrature`), topic sub-namespaces under `tools/` where a directory groups related
  helpers (e.g. `tools::numerics` in `tools/numerics/`), anonymous namespaces for TU-local
  helpers. No `using namespace` in headers.
- **Copyright banner** — every `.cpp`/`.hpp` starts with exactly:
  ```
  // Copyright (C) 2008-today The SG++ project
  // This file is part of the SG++ project. For conditions of distribution and
  // use, please see the copyright notice provided with SG++ or at
  // sgpp.sparsegrids.org
  ```
- **Includes**: angle brackets + absolute paths only (`#include <sgpp/combigrid/...>`); enforced.
- **Formatting**: `.clang-format` (Google, `ColumnLimit: 100`, `IncludeBlocks: Preserve`); run
  `clang-format -i <file>`. cpplint runs in the build (line length 100).
- **Doxygen**: Javadoc blocks (`/** */`, `@brief`, `@param`, `@return`, `@note`, `@f$…@f$`) for
  public API; new files get `@file` + `@brief`.
- **Comments stay general**: code comments / Doxygen (sources, tests, benchmarks, `.i`, examples)
  are written for every SG++ reader. Optimization reasoning and measurements are welcome but
  self-contained (what was compared, on what, the outcome); no WP- / RP- ids, "owner decided …",
  session / summary / `perf/results/` references or decision dates — those belong in
  `combigrid/perf/` and `combigrid/claude/`.
- **Naming**: `PascalCase` types, `camelCase` functions / members (or trailing `_`),
  `SCREAMING_SNAKE_CASE` constexpr constants, `snake_case` files.
- **Declarations / definitions** (guideline): documented declaration in the header, definition in
  the `.cpp` (comments explain complex parts, not a copy of the docs). Header-inline only where
  measurably faster even with LTO.
- **One type per file** avoid multiple or nested structs, classes, etc. in a single file.
- **Methods > 30 lines** get decomposed (rare exceptions for optimization).
- **Extending existing code** (guideline, not a hard rule): when extending existing functionality
  (e.g. adding a parameter to an operation), prefer adjusting the existing code over adding new code
  on top of it whenever the new code would make the existing code obsolete / unnecessary (e.g. the
  new code could completely replace the existing one).
- **Errors**: `assert` for internal invariants; otherwise throw an exception
- **Parallelism**: OpenMP in the operator drivers and internally (gated by `constants.hpp`).

## 6. Gotchas

- `MIVecSGGenInstr` stores a **reference** to its `LvlMIVec`: keep the vector alive as long as the
  instruction and its clones (`clone()` copies the reference). Grids copy only the instruction's
  `SparseGridSetup` (`getSetup()`) and need neither afterwards. Python keeps it alive.
- Node getters return raw `NodeGenFunc*` to static singletons — never delete or wrap them.
  `getClenshawCurtisNodeGenFunc()` and `getSecondTypeChebyshevNodeGenFunc()` are the **same**
  singleton.
- `setBoundaryLevelOffset(k)`: boundary nodes in dimension j only if ℓ[j] ≥ k (0 = always).
- Zero-coefficient tensor grids are never stored in a `SparseGrid` (`CompleteSGGenInstr` and
  `MIVecSGGenInstr`); a `DeltasSparseGrid` keeps every MI of the downwards closure.
- `SparseGrid::operator==` is O(nTG²) — avoid in loops.
- Cross-call operator data cache: an operator touches it only with its option on (off = no read,
  no write, no lock); per-grid data of a new operator goes through
  `tools::cross_call::getOperatorData` and reports `memoryBytes()`; results must not depend on the
  option; hold the returned `shared_ptr` for the whole call.
- Orphaned OpenMP worksharing (`#pragma omp for` outside the function's own `parallel`) must not
  run on a serial path that skips the operator's parallel region: it would bind to a caller's
  parallel region and split the call's work across the caller's team.
- An exception that leaves an OpenMP region calls `std::terminate`: obtain everything that may
  throw (e.g. quadrature rules) serially before the region. Operators get 1D rules from
  `NodeGenFunc::getQuadRule(n, includeBoundary)` (`lvl >= offset`); its default returns
  `getQuadRule(n)`, which assumes the boundary nodes (equi / CC are not interpolatory for levels
  below an offset ≥ 2; cheb1 overrides it).
- `.lint` / `.os` / `.gcda` files without a matching source are stale artifacts — do not resurrect
  sources from them.
- Combigrid is only built in CI's **linux-full** job.
- The thesis predates refactors: never copy its code without checking the API (code wins).
- Source-function cache: keys are the user-domain bit patterns → any new unit → domain mapping
  must use `tools::mapToDomain`; every operator calls the virtual `finishOperatorCall()` of its
  source function once at its end; cached functions are passed by reference (slicing into the base
  class does not compile). It pays only for functions ≳ 0.3 µs.
- Optional libraries (Boost.Unordered, xxHash, LMDB) are detected only in
  [tools/defines/dependencies_defines.hpp](combigrid/src/sgpp/combigrid/tools/defines/dependencies_defines.hpp);
  public classes stay unconditional and throw without them, public headers include no library
  header (details, fallback flags and the header check: build-and-ci.md).
- Dimension-adaptive refinement changes the grid in place: during a call the grid must not be used
  (its node rows are borrowed); afterwards its TGs are in colexicographic order (as
  `MIVecSGGenInstr` gives the same index set), its cross-call operator data is dropped and views /
  iterators into it are invalid. The refinement object keeps its state between calls; a grid change
  behind its back (storage version) makes it re-evaluate the grid.
- Not implemented (details: module reference): direct refinement of a `DeltasSparseGrid`,
  dimension-wise boundary offsets, Leja nodes.

## 7. Python bindings (pysgpp)

Read [combigrid/claude/python-bindings.md](combigrid/claude/python-bindings.md) before changing the
bindings or a bound C++ API. Essentials:
- SWIG interface in [combigrid/build/pysgpp/](combigrid/build/pysgpp/) (`combigrid.i`); one flat
  module `pysgpp`, namespaces dropped, names as in C++.
- Build: `scons -j8 OPT=0 SG_ALL=0 SG_BASE=1 SG_COMBIGRID=1 SG_PYTHON=1 COMPILE_BOOST_TESTS=0
  RUN_PYTHON_TESTS=0`, then `export LD_LIBRARY_PATH=<repo>/lib:$LD_LIBRARY_PATH` and
  `PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp:$PYTHONPATH`; verify with
  [combigrid/examples/python/](combigrid/examples/python/).
- **C++ API changes can break the bindings even when C++ build and tests pass**: new internal helpers
  in umbrella-header headers need `%ignore`; methods moved into unwrapped bases (e.g.
  `SparseGridBase`) and types without typemap (`std::unique_ptr`, `std::span`, iterators) need
  `%extend` forwarders. Rebuild with `SG_PYTHON=1` after such changes.
- **Never run clang-format on `.i` files** (also not format-on-save): SWIG then fails with
  `Unknown SWIG preprocessor directive`.

## 8. Performance optimization phase

Benchmarks (micro / phase / end-to-end), A/B comparisons, reference check, protected files and
all commands: [combigrid/perf/README.md](combigrid/perf/README.md). Status and plan:
[combigrid/perf/STATUS.md](combigrid/perf/STATUS.md), [PLAN.md](combigrid/perf/PLAN.md). The
master session follows the skill `perf-master`, work sessions follow the skill `perf-work`.
Protected files (baseline, reference results, tolerances, `perf.py`, the e2e/reference
benchmarks, pre-existing benchmarks) must not be edited; a hook blocks it.
