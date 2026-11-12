# CLAUDE.md — Working on the SG++ /combigrid/ module

Scope: editing, building and testing the Combination Technique (CT) module in
[combigrid/](combigrid/). Other SG++ modules, bindings and framework internals matter only insofar as
they affect `combigrid/`.

Read on demand:

| Topic | File |
|---|---|
| Directory layout, key classes, design rationale, vocabulary, base dependencies, known limitations, thesis discrepancies | [combigrid/claude/module-reference.md](combigrid/claude/module-reference.md) |
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
for custom / adaptive MI sets) → `SparseGrid` (list of `TensorGridCTData {mi, coefficient,
tensorGrid}`) → operators `interpolate`, `interpolateLinear`, `quadrature`. A `DeltasSparseGrid` +
its `quadrature` overload (C++ only) evaluates every grid point once; its `interpolateLinear`
overloads (C++ only, WP-47) every distinct corner once per call (index built on the first call). User functions are wrapped in
`SourceFunc` (f: Ω → ℝ^m: `VectorSourceFunc`, all operators (`interpolate`, `interpolateLinear`,
both quadratures; Python: all but the C++-only DSG quadrature)); nodes / weights live on [0,1]^d,
the user domain enters only via `setDomain()` (all operators map with `tools::mapToDomain` =
`std::fma`, so they evaluate bitwise identical points). Optional caching (WP-26): `CachedSourceFunc`
/ `CachedVectorSourceFunc` (a `SourceFunc` / `VectorSourceFunc` whose callable looks every point up
in a shared `SourceFuncCache`; `SourceFuncCacheConfig{.memoryLimit, .nShards, .policy, .file,
.storageSpaceLimit}`, `statistics()`, `flush()`, `clear(deletePersistentFile)`; Python too); always
compiled, but works only if Boost ≥ 1.81 and xxHash ≥ 0.8 are found at build time, else its
constructors throw `base::dependency_exception`. Policies (WP-28): `CachePolicy::MemoryOnly`
(default), `TemporaryStorage` (values beyond memory go to an unnamed LMDB file, deleted with the
process), `PersistentStorage` (LMDB file `file` loaded at creation, written at every operator end);
both need LMDB (linked by SCons if found). `CachePolicy::Adaptive` (WP-27) switches itself between
None / MemoryOnly (start) / TemporaryStorage from sampled costs (`CacheStatistics::adaptiveMode`;
factors and windows `ADAPTIVE_*` in `constants.hpp`; internals `miscellaneous/caching/adaptive/`).

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
  `miscellaneous/`), `::constants`, per-operator namespaces for internals, anonymous namespaces for
  TU-local helpers. No `using namespace` in headers.
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
- **Errors**: `assert` for internal invariants; otherwise throw an exception
- **Parallelism**: OpenMP in the operator drivers and internally (gated by `constants.hpp`).

## 6. Gotchas

- `MIVecSGGenInstr` stores a **reference** to its `LvlMIVec`: keep the vector alive as long as the
  instruction and any derived `SparseGrid` (`clone()` copies the reference). Python keeps it alive.
- Node getters return raw `NodeGenFunc*` to static singletons — never delete or wrap them.
  `getClenshawCurtisNodeGenFunc()` and `getSecondTypeChebyshevNodeGenFunc()` are the **same**
  singleton.
- `setBoundaryLevelOffset(k)`: boundary nodes in dimension j only if ℓ[j] ≥ k (0 = always).
- Zero-coefficient tensor grids are never stored (`CompleteSGGenInstr` and `MIVecSGGenInstr`).
- `SparseGrid::operator==` is O(nTG²) — avoid in loops.
- `.lint` / `.os` / `.gcda` files without a matching source are stale artifacts — do not resurrect
  sources from them.
- Combigrid is only built in CI's **linux-full** job.
- The thesis predates refactors: never copy its code without checking the API (code wins).
- Source-function cache: one `SourceFuncCache` per function (m fixed by the first cached function,
  another m throws; points of another d bypass it, with the storage policies the operator then
  throws); NaN values are not stored; it only pays for functions ≳ 0.3 µs (the judged suite uses
  plain `SourceFunc`); the disk tier only for much more expensive ones (LMDB writes ≈ 1–5 µs per
  new point, serialized). A persistent file is identified by d and m only (no function id), locked
  exclusively, never corrupted by program crashes; damaged / mismatching files throw
  `base::file_exception` (`CacheFileException`: base exceptions keep only the `const char*`,
  so composed messages need it). Its keys are the user-domain bit
  patterns: any new unit → domain mapping must use `tools::mapToDomain`. The end-of-operator hook
  is the virtual `finishOperatorCall()` of `SourceFunc` / `VectorSourceFunc` (virtual destructors).
  Copying a subclass object into the base class (slicing: would lose the override) does not
  compile (deleted constrained converting constructor / assignment, `tools::StrictSubclassOf`);
  pass cached functions by reference. These special members are hidden from SWIG (`#ifndef SWIG`).
- Optional libraries are detected only in
  [tools/defines/dependencies_defines.hpp](combigrid/src/sgpp/combigrid/tools/defines/dependencies_defines.hpp)
  (`__has_include` + version macros; the only SCons code is `detectLMDB`, which links LMDB and
  defines `SGPP_COMBIGRID_LMDB_LINKED`): code that needs them tests its macro
  (`SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE`, `SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE`,
  only in `.cpp` / internal files: it includes `xxhash.h` / `lmdb.h`; else `..._EXCEPTION` holds the
  reason). Public classes stay unconditional and throw without the library; public headers only
  forward-declare the table / store (no library header, no configuration-dependent layout; check:
  `combigrid/perf/results/WP-40/check_public_headers.sh`). Force the fallbacks with
  `CPPFLAGS=-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE` / `-DSGPP_COMBIGRID_DISABLE_LMDB_STORE`.
- Adaptive cache policy: the mode word is read per evaluation; None leaves only by probes at the
  end-of-operator hook; one point in P is timed with `misc::adaptive_caching::CostTimer` (rdtscp;
  `steady_clock` costs 1.9 µs on the owner's laptop: HPET clocksource). Tests force timing of every
  evaluation with `CostTimer::overrideSamplingShift(0)` and test the rules with injected costs.
- Not implemented (details: module reference): dimension-adaptive refinement (caller builds the loop
  on `MIVecSGGenInstr`), dimension-wise boundary offsets, Leja nodes.

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
